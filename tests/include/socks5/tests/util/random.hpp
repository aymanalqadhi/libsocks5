#ifndef LIBSOCKS5_TESTS_UTIL_RANDOM_HPP
#define LIBSOCKS5_TESTS_UTIL_RANDOM_HPP

#include <algorithm>
#include <cstddef>
#include <limits>
#include <random>
#include <string>
#include <type_traits>

namespace socks5::tests::util {

enum class random_string_options : int {
    alpha      = 1,
    num        = 2,
    sym        = 4,
    alphanum   = alpha | num,
    lower_case = 8,
    upper_case = 16,
    mixed_case = lower_case | upper_case,
    mixed      = alpha | num | lower_case | upper_case | sym,
};

inline constexpr auto operator&(random_string_options lhs,
                                random_string_options rhs) noexcept -> bool {
    return (static_cast<int>(lhs) & static_cast<int>(rhs)) != 0;
}

inline constexpr auto operator|(random_string_options lhs,
                                random_string_options rhs) noexcept
    -> random_string_options {
    return static_cast<random_string_options>(static_cast<int>(lhs) |
                                              static_cast<int>(rhs));
}

namespace detail {

template <random_string_options opts>
inline auto build_random_string_pool() noexcept -> std::string {
    constexpr auto lower_case_alpha = "abcdefghijklmnopqrstuvwxyz";
    constexpr auto upper_case_alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    constexpr auto num              = "1234567890";
    constexpr auto sym              = "`~!@#$%^&*()-_=+\"\\/?.>,<";

    std::string pool {};

    if constexpr (opts & random_string_options::alpha) {
        if constexpr (!(opts & random_string_options::lower_case) &&
                      !(opts & random_string_options::upper_case)) {
            pool.append(lower_case_alpha);
        } else {
            if constexpr (opts & random_string_options::lower_case) {
                pool.append(lower_case_alpha);
            }

            if constexpr (opts & random_string_options::upper_case) {
                pool.append(upper_case_alpha);
            }
        }
    }

    if constexpr (opts & random_string_options::num) {
        pool.append(num);
    }

    if constexpr (opts & random_string_options::sym) {
        pool.append(sym);
    }

    return pool;
}

} // namespace detail

template <random_string_options opts = random_string_options::mixed>
inline auto random_string(std::size_t len) -> std::string {
    std::mt19937 rng {std::random_device {}()};
    rng.seed(rng() * rng.default_seed);

    auto pool = socks5::tests::util::detail::build_random_string_pool<opts>();
    std::shuffle(pool.begin(), pool.end(), rng);

    return pool.substr(0, len);
}

template <typename T,
          typename = std::enable_if_t<std::is_integral_v<T> ||
                                      std::is_floating_point_v<T>>>
inline auto random(T min = std::numeric_limits<T>::min(),
                   T max = std::numeric_limits<T>::max()) -> T {
    std::mt19937 rng {std::random_device {}()};

    if constexpr (std::is_integral_v<T>) {
        return std::uniform_int_distribution<T> {min, max}(rng);
    } else {
        return std::uniform_real_distribution<T> {min, max}(rng);
    }
}

} // namespace socks5::tests::util

#endif
