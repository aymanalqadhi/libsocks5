#ifndef LIBSOCKS5_BASIC_STREAM_HPP
#define LIBSOCKS5_BASIC_STREAM_HPP

#include "socks5/detail/async/util.hpp"

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/system_error.hpp>

#include <cstdint>
#include <string_view>

namespace socks5 {

template <typename AsyncStream>
struct basic_stream {
    using executor_type = typename AsyncStream::executor_type;

    basic_stream(boost::asio::io_context &io) : io_ {io}, sock_ {io} {
    }

    template <typename CompletionToken>
    decltype(auto) async_socks5_open(boost::asio::ip::tcp::endpoint ep,
                                     CompletionToken &&             token) {
        return sock_.async_connect(std::move(ep),
                                   std::forward<CompletionToken>(token));
    }

    template <typename CompletionToken>
    decltype(auto) async_socks5_open(std::string_view  addr,
                                     std::uint16_t     port,
                                     CompletionToken &&token) {
        boost::system::error_code err;
        auto                      ip = boost::asio::ip::make_address(addr, err);

        if (err) {
            GENERATE_ERROR_HANDLER(token, handler, result);
            handler(err);
            return result.get();
        } else {
            return async_socks5_open({std::move(ip), port},
                                     std::forward<CompletionToken>(token));
        }
    }

    inline void socks5_close() {
        sock_.close();
    }

    inline auto is_socks5_open() const noexcept -> bool {
        return sock_.is_open();
    }

    inline auto get_executor() -> executor_type {
        return sock_.get_executor();
    }

  private:
    boost::asio::io_context &io_;
    AsyncStream              sock_;
};

} // namespace socks5

#endif
