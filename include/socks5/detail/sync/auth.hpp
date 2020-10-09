#ifndef LIBSOCKS5_DETAIL_SYNC_AUTH_HPP
#define LIBSOCKS5_DETAIL_SYNC_AUTH_HPP

#include "socks5/detail/sync/io.hpp"
#include "socks5/error_code.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/system/system_error.hpp>

#include <array>
#include <cassert>
#include <cstdint>
#include <string_view>

namespace socks5::detail::sync {

enum class auth_method : std::uint8_t {
    none              = 0x00,
    username_password = 0x02,
    not_acceptable    = 0xFF
};

template <auth_method method, typename Stream>
inline void auth_begin(Stream &s, boost::system::error_code &ec) {
    std::array<std::uint8_t, 3> in {5, 1, static_cast<std::uint8_t>(method)};
    std::array<std::uint8_t, 2> out {};

    if (socks5::detail::sync::write_read(s, boost::asio::buffer(in),
                                         boost::asio::buffer(out), ec);
        !ec && out[1] == 0xFF) {
        ec = socks5::make_error_code(
            socks5::error_code::auth_method_not_supported);
        s.close();
        return;
    }

    assert(out[1] == static_cast<std::uint8_t>(method));
}

template <typename Stream>
inline void auth_none(Stream &s, boost::system::error_code &ec) {
    auth_begin<auth_method::none>(s, ec);
}

template <typename Stream>
inline void auth_username_password(Stream &                   s,
                                   std::string_view           uname,
                                   std::string_view           passwd,
                                   boost::system::error_code &ec) {
    if (auth_begin<auth_method::username_password>(s, ec); ec) {
        return;
    }

    auto                        req = socks5::make_request<1>(uname, passwd);
    std::array<std::uint8_t, 2> outbuf {};

    if (socks5::detail::sync::write_read(s, boost::asio::buffer(req.buffer()),
                                         boost::asio::buffer(outbuf), ec);
        !ec && outbuf[1] != 0x00) {
        ec = socks5::make_error_code(socks5::error_code::auth_failed);
        s.close();
    }
}

} // namespace socks5::detail::sync

#endif
