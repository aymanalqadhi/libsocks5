#ifndef LIBSOCKS5_TESTS_FAKES_SOCKET_HPP
#define LIBSOCKS5_TESTS_FAKES_SOCKET_HPP

#include "socks5/detail/async/util.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/process/async_pipe.hpp>
#include <boost/system/system_error.hpp>

#include <cassert>
#include <stdexcept>

namespace socks5::tests::fakes {

struct socket final {
    using executor_type = boost::asio::io_context::executor_type;

    inline auto get_executor() -> executor_type {
        return io_.get_executor();
    }

    socket(boost::asio::io_context &io)
        : io_ {io},
          input_pipe_ {io_},
          output_pipe_ {io_},
          connected_ {false},
          fails_ {false} {
    }

    template <typename CompletionToken>
    inline decltype(auto)
    async_connect(const boost::asio::ip::tcp::endpoint &ep,
                  CompletionToken &&                    token) {
        GENERATE_COMPLETION_HANDLER(void(const boost::system::error_code &),
                                    token, handler, result);

        assert(!connected_.load());

        if (fails_.load()) {
            return handler(
                boost::asio::error::make_error_code(boost::asio::error::fault));
        }

        connected_.store(true);
        ep_ = ep;

        handler(boost::system::error_code {});

        return result.get();
    }

    inline void close() {
        assert(connected_);

        if (fails_.load()) {
            throw std::logic_error {"Cannot close socket"};
        }

        input_pipe_.close();
        output_pipe_.close();

        connected_.store(false);
    }

    template <typename CompletionToken>
    inline void async_read_some(boost::asio::mutable_buffer buf,
                                CompletionToken &&          token) {
        GENERATE_COMPLETION_HANDLER(
            void(const boost::system::error_code &, std::size_t), token,
            handler, result);

        assert(connected_);

        if (fails_) {
            handler(boost::asio::error::fault, 0);
            return;
        }

        input_pipe_.async_read_some(buf, std::move(handler));
        return result.get();
    }

    template <typename CompletionToken>
    inline void async_write_some(boost::asio::const_buffer buf,
                                 CompletionToken &&        token) {
        GENERATE_COMPLETION_HANDLER(
            void(const boost::system::error_code &, std::size_t), token,
            handler, result);

        assert(connected_);

        if (fails_) {
            handler(boost::asio::error::fault, 0);
            return;
        }

        output_pipe_.async_write_some(buf, std::move(handler));
        return result.get();
    }

    [[nodiscard]] inline auto always_fails() const noexcept -> bool {
        return fails_.load();
    }

    inline void always_fails(bool value) noexcept {
        fails_.store(value);
    }

    [[nodiscard]] inline auto is_open() const noexcept -> bool {
        return connected_;
    }

    [[nodiscard]] inline auto input_pipe() noexcept
        -> boost::process::async_pipe & {
        return input_pipe_;
    }

    [[nodiscard]] inline auto output_pipe() noexcept
        -> boost::process::async_pipe & {
        return output_pipe_;
    }

    [[nodiscard]] inline auto remote_endpoint() noexcept
        -> boost::asio::ip::tcp::endpoint & {
        return ep_;
    }

  private:
    boost::asio::io_context &  io_;
    boost::process::async_pipe input_pipe_;
    boost::process::async_pipe output_pipe_;

    boost::asio::ip::tcp::endpoint ep_;
    std::atomic_bool               connected_, fails_;
};

} // namespace socks5::tests::fakes

#endif
