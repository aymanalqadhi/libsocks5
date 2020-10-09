#ifndef LIBSOCKS5_MESSAGE_HPP
#define LIBSOCKS5_MESSAGE_HPP

#include <cstddef>
#include <utility>

namespace socks5 {

template <typename Container>
struct message {
    using container_type = Container;
    using iterator_type  = typename container_type::iterator;

    message() = default;

    message(Container buf) : buf_ {std::move(buf)} {
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

    [[nodiscard]] inline auto buffer() const noexcept
        -> const container_type & {
        return buf_;
    }

    [[nodiscard]] inline auto buffer() noexcept -> container_type & {
        return buf_;
    }

  protected:
    Container buf_;
};

} // namespace socks5

#endif
