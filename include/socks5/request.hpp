#ifndef LIBSOCKS5_REQUEST_HPP
#define LIBSOCKS5_REQUEST_HPP

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <string_view>
#include <type_traits>
#include <vector>

#include <boost/asio/ip/address.hpp>

namespace socks5 {

struct request final {
    request() = default;

    template <typename T,
              std::size_t size = sizeof(T),
              typename         = std::enable_if_t<std::is_unsigned_v<T>>>
    inline void put(T num) noexcept {
        static_assert(size >= 1 && size <= 8 && size <= sizeof(T));

        buf_.push_back((num >> (8 * (size - 1))) & 0xFF);
        if constexpr (size - 1 > 0) {
            put<T, size - 1>(num, buf_);
        }
    }

    template <typename Iterator>
    inline void put(const Iterator &begin, const Iterator &end) {
        std::copy(begin, end, std::back_inserter(buf_));
    }

    inline void put(std::string_view str) {
        assert(str.length() <= 0xFF);

        put<std::uint8_t>(str.length());
        put(str.cbegin(), str.cend());
    }

    inline void put(const boost::asio::ip::address_v4 &addr) {
        const auto &bytes = addr.to_bytes();
        put(bytes.cbegin(), bytes.cend());
    }

    inline void put(const boost::asio::ip::address_v6 &addr) {
        const auto &bytes = addr.to_bytes();
        put(bytes.cbegin(), bytes.cend());
    }

    inline auto buffer() const noexcept -> const std::vector<std::uint8_t> & {
        return buf_;
    }

    inline auto size() const noexcept -> std::size_t {
        return buf_.size();
    }

  private:
    std::vector<std::uint8_t> buf_;
};

} // namespace socks5

#endif
