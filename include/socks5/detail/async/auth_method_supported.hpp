#ifndef LIBSOCKS5_DETAIL_ASYNC_AUTH_METHOD_SUPPORTED_HPP
#define LIBSOCKS5_DETAIL_ASYNC_AUTH_METHOD_SUPPORTED_HPP

#include "socks5/detail/async/send_request.hpp"
#include "socks5/detail/async/util.hpp"
#include "socks5/error_code.hpp"

#include <boost/asio/read.hpp>
#include <boost/system/error_code.hpp>

#include <array>
#include <cassert>
#include <cstdint>
#include <type_traits>

namespace socks5::detail::async {

template <typename AsyncReadStream, typename Handler>
struct async_auth_method_supported_op final {
    static constexpr auto buffer_size = 2;

    async_auth_method_supported_op(AsyncReadStream &sock, Handler &&handler)
        : sock_ {sock},
          handler_ {std::forward<Handler>(handler)},
          buf_ {std::make_unique<std::uint8_t>(buffer_size)} {
    }

    void operator()(const boost::system::error_code &err, std::size_t rx) {
        if (err) {
            return handler_(err, 0x00);
        }

        assert(rx == 2 && buf_.get()[0] == 0x05);
        handler_(socks5::make_error_code(
                     buf_.get()[1] == 0xFF
                         ? socks5::error_code::auth_method_not_supported
                         : socks5::error_code::success),
                 buf_.get()[1]);
    }

    template <typename... Method>
    inline void initiate(Method... methods) {
        static_assert(sizeof...(Method) < 0xFF,
                      "Up to 254 Methods are allowed");

        socks5::detail::async::async_send_request(
            sock_,
            [self {std::move(*this)}](const auto &err) mutable {
                if (err) {
                    return self.handler_(err, 0);
                }

                boost::asio::async_read(
                    self.sock_,
                    boost::asio::buffer(self.buf_.get(), buffer_size),
                    std::move(self));
            },
            static_cast<std::uint8_t>(0x05),
            static_cast<std::uint8_t>(sizeof...(Method)),
            static_cast<std::uint8_t>(methods)...);
    }

  private:
    AsyncReadStream &             sock_;
    Handler                       handler_;
    std::unique_ptr<std::uint8_t> buf_;
};

template <typename AsyncStream, typename CompletionToken, typename... Method>
inline decltype(auto) async_auth_method_supported(AsyncStream &     sock,
                                                  CompletionToken &&token,
                                                  Method... methods) {
    GENERATE_COMPLETION_HANDLER(
        void(const boost::system::error_code &, std::uint8_t), token, handler,
        result);

    static_assert((std::is_unsigned_v<Method>, ...),
                  "Numerical values are required");

    async_auth_method_supported_op<AsyncStream, handler_type> {
        sock, std::move(handler)}
        .initiate(std::forward<Method>(methods)...);

    return result.get();
}

} // namespace socks5::detail::async

#endif
