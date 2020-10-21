#ifndef LIBSOCKS5_DETAIL_THROW_ERROR_HPP
#define LIBSOCKS5_DETAIL_THROW_ERROR_HPP

#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

namespace socks5::detail {

inline void throw_error(const boost::system::error_code &ec) {
    if (ec) {
        throw boost::system::system_error {ec};
    }
}

} // namespace socks5::detail

#endif
