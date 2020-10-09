#ifndef LIBSOCKS5_REQUEST_HPP
#define LIBSOCKS5_REQUEST_HPP

#include "socks5/detail/type_traits/iterator.hpp"
#include "socks5/message.hpp"

#include <boost/asio/ip/address.hpp>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <string_view>
#include <type_traits>
#include <vector>

namespace socks5 {

namespace detail {

template <typename T,
          std::size_t size = sizeof(T),
          typename         = std::enable_if_t<std::is_unsigned_v<T>>>
inline void append_uint(T num, std::vector<std::uint8_t> &buf) noexcept {
    static_assert(size >= 1 && size <= 8 && size <= sizeof(T));

    buf.emplace_back((num >> (8 * (size - 1))) & 0xFF);
    if constexpr (size - 1 > 0) {
        append_uint<T, size - 1>(num, buf);
    }
}

} // namespace detail

template <std::uint8_t version>
struct request final : socks5::message<std::vector<std::uint8_t>> {
    request() {
        put<std::uint8_t>(version);
    }

    template <typename... Arg>
    request(Arg &&... args) : request() {
        (put(std::forward<Arg>(args)), ...);
    }

    template <typename T, typename = std::enable_if_t<std::is_unsigned_v<T>>>
    inline void put(T num) noexcept {
        socks5::detail::append_uint(num, message::buf_);
    }

    template <typename Iterator,
              typename = std::enable_if_t<
                  socks5::detail::type_traits::is_iterator_v<Iterator>>>
    inline void put(const Iterator &begin, const Iterator &end) {
        std::copy(begin, end, std::back_inserter(message::buf_));
    }

    inline void put(std::string_view str) {
        assert(str.length() <= 0xFF);

        put<std::uint8_t>(str.length());
        put(str.cbegin(), str.cend());
    }

    inline void put(const boost::asio::ip::address_v4 &addr) {
        const auto &bytes = addr.to_bytes();
        put<std::uint8_t>(1);
        put(bytes.cbegin(), bytes.cend());
    }

    inline void put(const boost::asio::ip::address_v6 &addr) {
        const auto &bytes = addr.to_bytes();
        put<std::uint8_t>(4);
        put(bytes.cbegin(), bytes.cend());
    }

    inline void put(const boost::asio::ip::address &addr) {
        if (addr.is_v4()) {
            put(addr.to_v4());
        } else {
            put(addr.to_v6());
        }
    }
};

template <std::uint8_t version, typename... T>
inline auto make_request(T &&... args) -> socks5::request<version> {
    socks5::request<version> ret {};
    (ret.put(std::forward<T>(args)), ...);
    return ret;
}

} // namespace socks5

#endif
