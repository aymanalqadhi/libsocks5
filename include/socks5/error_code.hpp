#ifndef LIBSOCKS5_ERROR_CODE_HPP
#define LIBSOCKS5_ERROR_CODE_HPP

#include <boost/system/error_code.hpp>

#include <string>

namespace socks5 {

enum class [[nodiscard]] error_code : std::uint8_t {
    success                    = 0x00,
    general_failure            = 0x01,
    not_allowed                = 0x02,
    network_unreachable        = 0x03,
    host_unreachable           = 0x04,
    connection_refused         = 0x05,
    ttl_expired                = 0x06,
    command_not_supported      = 0x07,
    address_type_not_supported = 0x08,
    auth_method_not_supported  = 0x10,
    authentication_failed      = 0x11,
    unknown_error              = 0xFF};

namespace detail {

class error_category : public boost::system::error_category {
  public:
    [[nodiscard]] inline auto name() const noexcept -> const char * override {
        return "socks5";
    }

    [[nodiscard]] inline auto message(int c) const -> std::string override {
        switch (static_cast<socks5::error_code>(c)) {
        case socks5::error_code::success:
            return "succeeded";
        case socks5::error_code::general_failure:
            return "general SOCKS server failure";
        case socks5::error_code::not_allowed:
            return "connection not allowed by ruleset response";
        case socks5::error_code::network_unreachable:
            return "Network unreachable";
        case socks5::error_code::host_unreachable:
            return "Host unreachable";
        case socks5::error_code::connection_refused:
            return "Connection refused";
        case socks5::error_code::ttl_expired:
            return "TTL expired";
        case socks5::error_code::command_not_supported:
            return "Command not supported";
        case socks5::error_code::address_type_not_supported:
            return "Address type not supported";
        case socks5::error_code::auth_method_not_supported:
            return "Authentication method not supported";
        case socks5::error_code::authentication_failed:
            return "Authentication failed";
        default:
            return "Unknoww error";
        }
    }

    [[nodiscard]] inline auto default_error_condition(int c) const noexcept
        -> boost::system::error_condition override {
        switch (static_cast<socks5::error_code>(c)) {
        case socks5::error_code::success:
            return make_error_condition(boost::system::errc::success);
        case socks5::error_code::command_not_supported:
            return make_error_condition(boost::system::errc::invalid_argument);
        case socks5::error_code::address_type_not_supported:
            return make_error_condition(
                boost::system::errc::address_family_not_supported);
        default:
            return boost::system::error_condition(c, *this);
        }
    }

    static auto current() -> const error_category & {
        static error_category instance {};
        return instance;
    }
};

} // namespace detail

inline auto operator==(const boost::system::error_code &lhs,
                       error_code                       rhs) noexcept -> bool {
    return lhs.value() == static_cast<int>(rhs);
}

inline auto error_category() -> const boost::system::error_category & {
    return socks5::detail::error_category::current();
}

inline auto make_error_code(socks5::error_code e) -> boost::system::error_code {
    return {static_cast<int>(e), socks5::error_category()};
}

inline auto make_error_condition(socks5::error_code e) -> std::error_condition {
    return std::error_condition(static_cast<int>(e), socks5::error_category());
}

} // namespace socks5

namespace boost::system {

template <>
struct is_error_code_enum<socks5::error_code> : std::true_type {};

template <>
struct is_error_condition_enum<socks5::error_code> : std::true_type {};

} // namespace boost::system

#endif
