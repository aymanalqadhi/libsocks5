#ifndef LIBSOCKS_RESPONSE_HPP
#define LIBSOCKS_RESPONSE_HPP

#include "socks5/detail/type_traits/iterator.hpp"
#include "socks5/message.hpp"

#include <boost/asio/ip/address.hpp>

#include <cassert>
#include <cstdint>
#include <stdio.h>
#include <type_traits>
#include <vector>

namespace socks5 {

template <typename Container = std::vector<std::uint8_t>>
struct response final : socks5::message<Container> {
    response(Container buf)
        : socks5::message<Container> {std::move(buf)}, pos_ {0} {
    }

    response() : response(Container {}) {
    }

    template <typename T, typename = std::enable_if_t<std::is_unsigned_v<T>>>
    inline auto take(T &out) -> bool {
        if (available() < sizeof(T)) {
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
        if (available() < count) {
            return false;
        }

        std::copy_n(current(), count, itr);
        advance(count);

        return true;
    }

    inline auto take(boost::asio::ip::address_v4 &out) -> bool {
        return take_address(out);
    }

    inline auto take(boost::asio::ip::address_v6 &out) -> bool {
        return take_address(out);
    }

    inline auto take(std::string &out) -> bool {
        if (available() < 1) {
            return false;
        }

        auto len = pop_front();

        if (available() < len) {
            return false;
        }

        out.resize(len);
        std::copy_n(current(), len, out.begin());
        advance(len);

        return true;
    }

    template <typename T>
    inline auto take() -> T {
        T ret {};
        take(ret);
        return ret;
    }

    inline auto skip(std::size_t count) -> bool {
        if (available() < count) {
            return false;
        }

        pos_ += count;
        return true;
    }

    inline auto current() const noexcept -> typename Container::const_iterator {
        return message<Container>::buffer().cbegin() + pos_;
    }

    inline auto available() const noexcept -> std::size_t {
        return std::distance(message<Container>::buffer().cbegin() + pos_,
                             message<Container>::buffer().cend());
    }

  private:
    inline void advance(std::size_t n) noexcept {
        assert(n <= available());
        pos_ += n;
    }

    inline auto pop_front() -> std::uint8_t {
        std::uint8_t ret {*current()};
        advance(1);
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

    std::size_t pos_;
};

template <std::size_t size>
using response_fixed = response<std::array<std::uint8_t, size>>;

} // namespace socks5

#endif
