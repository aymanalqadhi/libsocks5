#ifndef LIBSOCKS_DETAIL_TYPE_TRAITS_ITERATOR_HPP
#define LIBSOCKS_DETAIL_TYPE_TRAITS_ITERATOR_HPP

#include <algorithm>
#include <type_traits>

namespace socks5::detail::type_traits {

template <typename T, typename = void>
struct is_iterator : std::false_type {};

template <typename T>
struct is_iterator<
    T,
    std::enable_if_t<
        !std::is_same_v<typename std::iterator_traits<T>::value_type, void>>>
    : std::true_type {};

template <typename T>
constexpr auto is_iterator_v = is_iterator<T>::value;

} // namespace socks5::detail::type_traits

#endif
