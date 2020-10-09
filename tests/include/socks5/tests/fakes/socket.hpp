#ifndef LIBSOCKS5_TESTS_FAKES_SOCKET_HPP
#define LIBSOCKS5_TESTS_FAKES_SOCKET_HPP

#include "socks5/detail/async/util.hpp"
#include "socks5/tests/util/random.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/process/async_pipe.hpp>
#include <boost/system/system_error.hpp>

#include <cassert>

namespace socks5::tests::fakes {

struct socket final {
    using executor_type = boost::asio::io_context::executor_type;

    inline auto get_executor() -> executor_type {
        return io_.get_executor();
    }

    socket(boost::asio::io_context &io, bool connected = false)
        : io_ {io},
          input_pipe_ {io_},
          output_pipe_ {io_},
          connected_ {false},
          fails_ {false} {

        if (connected) {
            connected_ = true;
            ep_        = socks5::tests::util::random_endpoint();
        }
    }

    template <typename CompletionToken>
    inline decltype(auto)
    async_connect(const boost::asio::ip::tcp::endpoint &ep,
                  CompletionToken &&                    token) {
        GENERATE_ERROR_HANDLER(token, handler, result);

        assert(!connected_);

        if (fails_) {
            handler(boost::asio::error::fault);
        } else {
            connected_ = true;
            ep_        = ep;
            handler(boost::system::error_code {});
        }

        return result.get();
    }

    inline void connect(const boost::asio::ip::tcp::endpoint &ep,
                        boost::system::error_code &           ec) {
        if (fails_) {
            ec = boost::asio::error::fault;
            return;
        }

        connected_ = true;
        ep_        = ep;
        ec         = {};
    }

    inline void close() {
        assert(connected_);

        if (fails_) {
            throw std::logic_error {"Cannot close socket"};
        }

        input_pipe_.close();
        output_pipe_.close();

        connected_ = false;
    }

    template <typename CompletionToken>
    inline void async_read_some(const boost::asio::mutable_buffer &buf,
                                CompletionToken &&                 token) {
        GENERATE_TRANSMISSION_ERROR_HANDLER(token, handler, result);

        assert(connected_);

        if (fails_) {
            handler(boost::asio::error::fault, 0);
        } else {
            input_pipe_.async_read_some(buf, std::move(handler));
        }

        return result.get();
    }

    inline auto read_some(const boost::asio::mutable_buffer &buf,
                          boost::system::error_code &ec) -> std::size_t {
        if (fails_) {
            ec = boost::asio::error::fault;
            return 0;
        }

        return input_pipe_.read_some(buf, ec);
    }

    template <typename CompletionToken>
    inline void async_write_some(const boost::asio::const_buffer &buf,
                                 CompletionToken &&               token) {
        GENERATE_TRANSMISSION_ERROR_HANDLER(token, handler, result);

        assert(connected_);

        if (fails_) {
            handler(boost::asio::error::fault, 0);
        } else {
            output_pipe_.async_write_some(buf, std::move(handler));
        }

        return result.get();
    }

    inline auto write_some(const boost::asio::const_buffer &buf,
                           boost::system::error_code &      ec) -> std::size_t {
        if (fails_) {
            ec = boost::asio::error::fault;
            return 0;
        }

        return output_pipe_.write_some(buf, ec);
    }

    [[nodiscard]] inline auto always_fails() const noexcept -> bool {
        return fails_;
    }

    inline void always_fails(bool value) noexcept {
        fails_ = value;
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

    inline void clear_input_pipe() {
        input_pipe_ = {io_};
    }

    inline void clear_output_pipe() {
        output_pipe_ = {io_};
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
