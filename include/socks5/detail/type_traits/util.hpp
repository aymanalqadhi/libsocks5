#ifndef LIBSOCKS5_DETAIL_TYPE_TRAITS_UTIL_HPP
#define LIBSOCKS5_DETAIL_TYPE_TRAITS_UTIL_HPP

#include <type_traits>

namespace socks5::detail::type_traits {

template <typename T, typename... Rest>
struct is_any : std::false_type {};

template <typename T, typename First>
struct is_any<T, First> : std::is_same<T, First> {};

template <typename T, typename First, typename... Rest>
struct is_any<T, First, Rest...>
    : std::integral_constant<bool,
                             std::is_same<T, First>::value ||
                                 is_any<T, Rest...>::value> {};

template <typename T, typename... Other>
constexpr auto is_any_v = is_any<T, Other...>::value;

template <typename T, typename... Other>
constexpr auto is_any_decay_v = is_any<T, std::decay_t<Other>...>::value;

} // namespace socks5::detail::type_traits

#endif
