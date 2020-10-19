#ifndef LIBSOCKS5_DETAIL_SYNC_COMMAND_HPP
#define LIBSOCKS5_DETAIL_SYNC_COMMAND_HPP

#include "socks5/detail/sync/io.hpp"
#include "socks5/error_code.hpp"
#include "socks5/request.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>

#include <array>
#include <cstdint>
#include <string_view>

namespace socks5::detail::sync {

namespace detail {

enum class command : std::uint8_t {
    connect       = 0x01,
    bind          = 0x02,
    udp_associate = 0x3
};

template <command      cmd,
          std::uint8_t address_type,
          typename Stream,
          typename Address>
inline void do_send_command(Stream &                        s,
                            const Address &                 addr,
                            std::uint16_t                   port,
                            boost::asio::ip::tcp::endpoint &local,
                            boost::system::error_code &     ec) {
    socks5::request<5>          req {};
    std::array<std::uint8_t, 3> resp {};

    req.put<std::uint8_t>(static_cast<std::uint8_t>(cmd));
    req.put<std::uint8_t>(0x00);
    req.put<std::uint8_t>(address_type);
    req.put(addr);
    req.put<std::uint16_t>(port);

    if (socks5::detail::sync::write_read(s, boost::asio::buffer(req.buffer()),
                                         boost::asio::buffer(resp), ec);
        ec) {
        return;
    } else if (resp[1] != 0) {
        ec = socks5::make_error_code(static_cast<socks5::error_code>(resp[1]));
        return;
    }

    socks5::detail::sync::read_endpoint(s, local, ec);
}

} // namespace detail

using detail::command;

template <command cmd, typename Stream>
inline void send_command(Stream &                              s,
                         const boost::asio::ip::tcp::endpoint &remote,
                         boost::asio::ip::tcp::endpoint &      local,
                         boost::system::error_code &           ec) noexcept {
    if (remote.address().is_v4()) {
        detail::do_send_command<cmd, 1>(s, remote.address(), remote.port(),
                                        local, ec);
    } else {
        detail::do_send_command<cmd, 4>(s, remote.address(), remote.port(),
                                        local, ec);
    }
}

template <command cmd, typename Stream>
inline void send_command(Stream &                        s,
                         std::string_view                domain,
                         std::uint8_t                    port,
                         boost::asio::ip::tcp::endpoint &local,
                         boost::system::error_code &     ec) noexcept {
    detail::do_send_command<cmd, 3>(s, domain, port, local, ec);
}

#define MAKE_COMMAND(name, cmd)                                                \
    template <typename Stream>                                                 \
    inline void name(Stream &s, const boost::asio::ip::tcp::endpoint &remote,  \
                     boost::asio::ip::tcp::endpoint &local,                    \
                     boost::system::error_code &     ec) noexcept {            \
        send_command<cmd>(s, remote, local, ec);                               \
    }                                                                          \
                                                                               \
    template <typename Stream>                                                 \
    inline void name(Stream &s, std::string_view domain, std::uint8_t port,    \
                     boost::asio::ip::tcp::endpoint &local,                    \
                     boost::system::error_code &     ec) noexcept {            \
        send_command<cmd>(s, domain, port, local, ec);                         \
    }

MAKE_COMMAND(tcp_connect, command::connect)
MAKE_COMMAND(tcp_bind, command::bind)
MAKE_COMMAND(udp_associate, command::udp_associate)

#undef MAKE_COMMAND

} // namespace socks5::detail::sync

#endif
