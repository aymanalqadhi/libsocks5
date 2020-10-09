#ifndef LIBSOCKS5_DETAIL_SYNC_OPEN_HPP
#define LIBSOCKS5_DETAIL_SYNC_OPEN_HPP

#include <boost/asio/error.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>

namespace socks5::detail::sync {

template <typename Stream>
inline auto is_open(Stream &s, boost::system::error_code &ec) noexcept -> bool {
    if (s.is_open()) {
        return true;
    }

    ec = boost::asio::error::not_connected;
    return false;
}

template <typename Stream>
inline void open(Stream &                              s,
                 const boost::asio::ip::tcp::endpoint &ep,
                 boost::system::error_code &           ec) {
    s.connect(ep, ec);
}

} // namespace socks5::detail::sync

#endif
