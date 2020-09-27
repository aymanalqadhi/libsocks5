#ifndef LIBSOCKS5_MESSAGE_HPP
#define LIBSOCKS5_MESSAGE_HPP

#include <cstddef>
#include <utility>

namespace socks5 {

template <typename Container>
struct message {
    using iterator_type = typename Container::iterator;

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

    inline auto operator[](std::size_t i) ->
        typename iterator_type::value_type {
        return buf_[i];
    }

  protected:
    Container buf_;
};

} // namespace socks5

#endif
