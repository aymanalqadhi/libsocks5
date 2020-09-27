#ifndef LIBSOCKS_RESPONSE_HPP
#define LIBSOCKS_RESPONSE_HPP

#include "socks5/detail/type_traits/iterator.hpp"
#include "socks5/message.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <deque>
#include <type_traits>

#include <boost/asio/ip/address.hpp>

namespace socks5 {

struct response final : socks5::message<std::deque<std::uint8_t>> {
    using container_type = std::deque<std::uint8_t>;

    response(container_type buf)
        : socks5::message<container_type> {std::move(buf)} {
    }

    template <typename T, typename = std::enable_if_t<std::is_unsigned_v<T>>>
    inline auto take(T &out) -> bool {
        if (size() < sizeof(T)) {
            return false;
        }

        for (std::size_t i {0}; i < sizeof(T); ++i) {
            out = (out << 8) | pop_front();
        }

        return true;
    }

    template <typename Iterator,
              typename = std::enable_if_t<
                  socks5::detail::type_traits::is_iterator_v<Iterator>>>
    inline auto take(Iterator itr, std::size_t count) -> bool {
        if (size() < count) {
            return false;
        }

        std::copy(buf_.begin(), buf_.begin() + count, itr);
        buf_.erase(buf_.begin(), buf_.begin() + count);

        return true;
    }

    inline auto take(boost::asio::ip::address_v4 &out) -> bool {
        return take_address(out);
    }

    inline auto take(boost::asio::ip::address_v6 &out) -> bool {
        return take_address(out);
    }

  private:
    inline auto pop_front() -> std::uint8_t {
        assert(size() > 0);

        auto ret = buf_.front();
        buf_.pop_front();

        return ret;
    }

    template <typename Address>
    inline auto take_address(Address &out) -> bool {
        typename Address::bytes_type bytes;

        if (!take(bytes.begin(), bytes.size())) {
            return false;
        }

        out = Address {bytes};
        return true;
    }
};

} // namespace socks5

#endif
