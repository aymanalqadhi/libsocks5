#ifndef LIBSOCKS5_BASIC_SOCKET_HPP
#define LIBSOCKS5_BASIC_SOCKET_HPP

#include "socks5/detail/sync/auth.hpp"
#include "socks5/detail/sync/open.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/system/system_error.hpp>

#include <cstdint>
#include <string_view>

namespace socks5 {

template <typename Stream>
struct basic_socket {
    using executor_type = typename boost::asio::io_context::executor_type;

    basic_socket(boost::asio::io_context &io)
        : io_ {io}, stream_ {io}, is_authed_ {false} {
    }

    inline void socks5_open(const boost::asio::ip::tcp::endpoint &ep,
                            boost::system::error_code &           ec) {
        socks5::detail::sync::open(stream_, ep, ec);
    }

    inline void socks5_auth(boost::system::error_code &ec) noexcept {
        if (is_socks5_authed()) {
            return;
        }

        if (socks5::detail::sync::auth_none(stream_, ec); !ec) {
            is_authed_ = true;
        }
    }

    inline void socks5_auth(std::string_view           uname,
                            std::string_view           passwd,
                            boost::system::error_code &ec) {
        if (is_socks5_authed()) {
            return;
        }

        socks5::detail::sync::auth_username_password(stream_, std::move(uname),
                                                     std::move(passwd), ec);
        if (!ec) {
            is_authed_ = true;
        }
    }

    inline void socks5_close() {
        stream_.close();
        is_authed_ = false;
    }

    inline auto is_socks5_open() const noexcept -> bool {
        return stream_.is_open();
    }

    inline auto is_socks5_authed() const noexcept -> bool {
        return is_authed_;
    }

    inline auto is_socks5_ready() const noexcept -> bool {
        return is_socks5_open() && is_socks5_authed();
    }

    inline auto stream() noexcept -> Stream & {
        return stream_;
    }

    inline auto stream() const noexcept -> const Stream & {
        return stream_;
    }

    inline auto get_executor() -> executor_type {
        return io_.get_executor();
    }

  protected:
    inline auto is_socks5_ready(boost::system::error_code &ec) -> bool {
        if (is_socks5_ready()) {
            return true;
        }

        ec = boost::asio::error::not_connected;
        return false;
    }

  private:
    boost::asio::io_context &io_;
    Stream                   stream_;
    bool                     is_authed_;
};

} // namespace socks5

#endif
