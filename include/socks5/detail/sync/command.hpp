#ifndef LIBSOCKS5_DETAIL_SYNC_COMMAND_HPP
#define LIBSOCKS5_DETAIL_SYNC_COMMAND_HPP

#include "socks5/detail/sync/io.hpp"
#include "socks5/error_code.hpp"
#include "socks5/request.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>

#include <array>

namespace socks5::detail::sync {

enum class command : std::uint8_t {
    connect       = 0x01,
    bind          = 0x02,
    udp_associate = 0x3
};

template <command cmd, typename Stream>
inline void send_command(Stream &                              s,
                         const boost::asio::ip::tcp::endpoint &remote,
                         boost::asio::ip::tcp::endpoint &      local,
                         boost::system::error_code &           ec) noexcept {
    socks5::request<5> req {};
    req.put<std::uint8_t>(static_cast<std::uint8_t>(cmd));
    req.put<std::uint8_t>(0x00);
    req.put<std::uint8_t>(remote.address().is_v4() ? 1 : 4);

    std::array<std::uint8_t, 3> resp1 {};

    if (socks5::detail::sync::write_read(s, boost::asio::buffer(req.buffer()),
                                         boost::asio::buffer(resp1), ec);
        ec) {
        return;
    } else if (resp1[1] != 0) {
        ec = socks5::make_error_code(static_cast<socks5::error_code>(resp1[1]));
        return;
    }

    socks5::detail::sync::read_endpoint(s, local, ec);
}

#define MAKE_COMMAND(name, cmd)                                                \
    template <typename Stream>                                                 \
    inline void name(Stream &s, const boost::asio::ip::tcp::endpoint &remote,  \
                     boost::asio::ip::tcp::endpoint &local,                    \
                     boost::system::error_code &     ec) noexcept {                 \
        send_command<cmd>(s, remote, local, ec);                               \
    }

MAKE_COMMAND(tcp_connect, command::connect)
MAKE_COMMAND(tcp_bind, command::bind)
MAKE_COMMAND(udp_associate, command::udp_associate)

#undef MAKE_COMMAND

} // namespace socks5::detail::sync

#endif
