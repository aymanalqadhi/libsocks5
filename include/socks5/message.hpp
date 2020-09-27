#ifndef LIBSOCKS5_MESSAGE_HPP
#define LIBSOCKS5_MESSAGE_HPP

#include <cstdint>
#include <vector>

namespace socks5 {

struct message {
    using container_type = std::vector<std::uint8_t>;
    using iterator_type  = container_type::iterator;

    message() = default;

    message(std::vector<std::uint8_t> buf) : buf_ {std::move(buf)} {
    }

    inline auto begin() noexcept -> iterator_type {
        return buf_.begin();
    }

    inline auto end() noexcept -> iterator_type {
        return buf_.end();
    }

    inline auto size() const noexcept -> std::size_t {
        return buf_.size();
    }

    inline auto operator[](std::size_t i) -> iterator_type::value_type {
        return buf_[i];
    }

  protected:
    container_type buf_;
};

} // namespace socks5

#endif
