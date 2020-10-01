#ifndef LIBSOCKS5_BASIC_STREAM_HPP
#define LIBSOCKS5_BASIC_STREAM_HPP

#include "socks5/detail/async/util.hpp"

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/tcp.hpp>

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
