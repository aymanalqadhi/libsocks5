#ifndef LIBSOCKS5_DETAIL_SYNC_IO_HPP
#define LIBSOCKS5_DETAIL_SYNC_IO_HPP

#include "socks5/detail/sync/open.hpp"
#include "socks5/detail/type_traits/util.hpp"
#include "socks5/request.hpp"
#include "socks5/response.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/system_error.hpp>

#include <array>
#include <cstdint>
#include <cstdio>

namespace socks5::detail::sync {

template <typename Stream, typename ConstBuffer>
inline auto write(Stream &                   s,
                  const ConstBuffer &        buf,
                  boost::system::error_code &ec) noexcept -> std::size_t {
    if (!socks5::detail::sync::is_open(s, ec)) {
        return 0UL;
    }

    return boost::asio::write(s, buf, ec);
}

template <typename Stream, typename MutableBuffer>
inline auto read(Stream &                   s,
                 const MutableBuffer &      buf,
                 boost::system::error_code &ec) noexcept -> std::size_t {
    if (!socks5::detail::sync::is_open(s, ec)) {
        return 0UL;
    }

    return boost::asio::read(s, buf, ec);
}

template <typename Stream, typename ConstBuffer, typename MutableBuffer>
inline auto write_read(Stream &                   s,
                       const ConstBuffer &        inbuf,
                       const MutableBuffer &      outbuf,
                       boost::system::error_code &ec) noexcept -> std::size_t {
    if (auto tx = socks5::detail::sync::write(s, inbuf, ec); !ec) {
        return tx + socks5::detail::sync::read(s, outbuf, ec);
    }

    return 0UL;
}

template <typename Stream, std::uint8_t version>
inline auto send_request(Stream &                        s,
                         const socks5::request<version> &req,
                         boost::system::error_code &     ec) noexcept
    -> std::size_t {
    return socks5::detail::sync::write(s, boost::asio::buffer(req.buffer()),
                                       ec);
}

template <typename Stream, typename ResponseContainer>
inline auto read_response(Stream &                             s,
                          socks5::response<ResponseContainer> &resp,
                          boost::system::error_code &          ec) noexcept
    -> std::size_t {
    return socks5::detail::sync::read(s, boost::asio::buffer(resp.buffer()),
                                      ec);
}

template <std::uint8_t version, typename Stream, typename ResponseContainer>
inline void send_request(Stream &                             s,
                         const socks5::request<version> &     req,
                         socks5::response<ResponseContainer> &resp,
                         boost::system::error_code &          ec) noexcept {
    if (send_request(s, req, ec); !ec) {
        read_response(s, resp, ec);
    }
}

template <typename Address,
          typename Stream,
          typename = std::enable_if_t<socks5::detail::type_traits::is_any_v<
              Address,
              boost::asio::ip::address_v4,
              boost::asio::ip::address_v6>>>
inline void read_endpoint(Stream &                        s,
                          boost::asio::ip::tcp::endpoint &ep,
                          boost::system::error_code &     ec) noexcept {
    constexpr auto size = std::tuple_size_v<typename Address::bytes_type>;

    socks5::response_fixed<size + 2> resp {};
    if (read_response(s, resp, ec); !ec) {
        ep.address(resp.template take<Address>());
        ep.port(resp.template take<std::uint16_t>());
    }
}

template <typename Stream>
inline void read_endpoint(Stream &                        s,
                          boost::asio::ip::tcp::endpoint &ep,
                          boost::system::error_code &     ec) noexcept {
    std::uint8_t type;

    if (socks5::detail::sync::read(s, boost::asio::buffer(&type, 1), ec), ec) {
        return;
    }

    assert(type == 1 || type == 4);

    if (type == 1) {
        return read_endpoint<boost::asio::ip::address_v4>(s, ep, ec);
    }

    read_endpoint<boost::asio::ip::address_v6>(s, ep, ec);
}

} // namespace socks5::detail::sync

#endif
