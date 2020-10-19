#ifndef LIBSOCKS5_BASIC_TCP_SOCKET_HPP
#define LIBSOCKS5_BASIC_TCP_SOCKET_HPP

#include "socks5/basic_socket.hpp"
#include "socks5/detail/sync/command.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>

#include <array>
#include <cassert>
#include <string_view>

namespace socks5 {

template <typename Stream>
struct basic_tcp_socket final : socks5::basic_socket<Stream> {
    basic_tcp_socket(boost::asio::io_context &io)
        : basic_socket<Stream> {io}, is_open_ {false} {
    }

    inline void connect(const boost::asio::ip::tcp::endpoint &ep,
                        boost::system::error_code &           ec) noexcept {
        if (do_connect(ec, ep); !ec) {
            remote_ = ep;
        }
    }

    inline void connect(std::string_view           domain,
                        std::uint16_t              port,
                        boost::system::error_code &ec) noexcept {
        do_connect(ec, domain, port);
    }

    inline void close() {
        basic_socket<Stream>::socks5_close();
        is_open_ = false;
    }

    template <typename ConstBuffer>
    inline auto write_some(const ConstBuffer &        buf,
                           boost::system::error_code &ec) noexcept
        -> std::size_t {
        if (!is_open()) {
            ec = boost::asio::error::not_connected;
            return 0UL;
        }

        return basic_socket<Stream>::stream().write_some(buf, ec);
    }

    template <typename MutableBuffer>
    inline auto read_some(const MutableBuffer &      buf,
                          boost::system::error_code &ec) noexcept
        -> std::size_t {
        if (!is_open()) {
            ec = boost::asio::error::not_connected;
            return 0UL;
        }

        return basic_socket<Stream>::stream().read_some(buf, ec);
    }

    inline auto is_open() const noexcept -> bool {
        return basic_socket<Stream>::is_socks5_ready() && is_open_;
    }

    inline auto local_endpoint() const noexcept
        -> const boost::asio::ip::tcp::endpoint & {
        return local_;
    }

    inline auto remote_endpoint() const noexcept
        -> const boost::asio::ip::tcp::endpoint & {
        return remote_;
    }

  private:
    template <typename... EndpontParam>
    inline void do_connect(boost::system::error_code &ec,
                           EndpontParam &&... ep) noexcept {
        if (!basic_socket<Stream>::is_socks5_ready(ec)) {
            return;
        }

        if (socks5::detail::sync::tcp_connect(basic_socket<Stream>::stream(),
                                              std::forward<EndpontParam>(ep)...,
                                              local_, ec);
            !ec) {
            is_open_ = true;
        }
    }

    bool                           is_open_;
    boost::asio::ip::tcp::endpoint local_, remote_;
};

} // namespace socks5

#endif
