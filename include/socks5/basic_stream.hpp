#ifndef LIBSOCKS5_BASIC_STREAM_HPP
#define LIBSOCKS5_BASIC_STREAM_HPP

#include <boost/asio/ip/tcp.hpp>

#include <cstdint>
#include <string_view>

namespace socks5 {

template <typename AsyncStream>
struct basic_stream {
    using executor_type = typename AsyncStream::executor_type;

    basic_stream(boost::asio::io_context &io) : io_ {io} {
    }

    template <typename CompletionToken>
    decltype(auto) async_socks5_open(std::string_view host,
                                     std::uint16_t    port = 1080) {
        // TODO: Implement
    }

    inline void socks5_close() {
        // TODO: Implement
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
