#ifndef LIBSOCKS_RESPONSE_HPP
#define LIBSOCKS_RESPONSE_HPP

#include <array>
#include <cassert>
#include <cstdint>
#include <type_traits>
#include <vector>

#include <boost/asio/ip/address.hpp>

namespace socks5 {

struct response final {
    response(std::vector<std::uint8_t> buf) : buf_ {std::move(buf)} {
    }

    inline auto pop() -> std::uint8_t {
        assert(available() > 0);

        auto ret = buf_.back();
        buf_.pop_back();

        return ret;
    }

    template <typename T, typename = std::enable_if_t<std::is_unsigned_v<T>>>
    inline auto take(T &out) -> bool {
        if (available() < sizeof(T)) {
            return false;
        }

        for (std::size_t i {0}; i < sizeof(T); ++i) {
            out = (out << 8) | pop();
        }

        return true;
    }

    template <typename Iterator>
    inline auto take(Iterator itr, std::size_t count) -> bool {
        if (available() < count) {
            return false;
        }

        std::copy(buf_.end() - count, buf_.end(), itr);
        return true;
    }

    inline auto take(boost::asio::ip::address_v4 &out) -> bool {
        return take_address(out);
    }

    inline auto take(boost::asio::ip::address_v6 &out) -> bool {
        return take_address(out);
    }

    inline auto buffer() const noexcept -> const std::vector<std::uint8_t> & {
        return buf_;
    }

    inline auto available() const noexcept -> std::size_t {
        return buf_.size();
    }

  private:
    template <typename Address>
    inline auto take_address(Address &out) -> bool {
        typename Address::bytes_type bytes;

        if (!take(bytes.begin(), bytes.size())) {
            return false;
        }

        out = Address {bytes};
        return true;
    }

    std::vector<std::uint8_t> buf_;
};

} // namespace socks5

#endif
