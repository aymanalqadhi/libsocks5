#ifndef LIBSOCKS5_DETAIL_ASYNC_UTIL_HPP
#define LIBSOCKS5_DETAIL_ASYNC_UTIL_HPP

#include <boost/asio/async_result.hpp>

#define GENERATE_COMPLETION_HANDLER(signature, t, h, r)                        \
    using result_type =                                                        \
        boost::asio::async_result<std::decay_t<decltype(t)>, signature>;       \
    using handler_type = typename result_type::completion_handler_type;        \
                                                                               \
    handler_type h {std::forward<decltype(t)>(t)};                             \
    result_type  r {h};

#define GENERATE_ERROR_HANDLER(t, h, r)                                        \
    GENERATE_COMPLETION_HANDLER(void(const boost::system::error_code &), t, h, \
                                r)

#define GENERATE_TRANSMISSION_ERROR_HANDLER(t, h, r)                           \
    GENERATE_COMPLETION_HANDLER(                                               \
        void(std::size_t, const boost::system::error_code &), t, h, r)

#endif
