#ifndef LIBSOCKS5_DETAIL_ASYNC_SEND_REQUEST_HPP
#define LIBSOCKS5_DETAIL_ASYNC_SEND_REQUEST_HPP

#include "socks5/detail/async/util.hpp"
#include "socks5/request.hpp"

#include <boost/asio/write.hpp>

#include <cstddef>
#include <cstdint>

namespace socks5::detail::async {

template <typename AsyncWriteStream, typename Handler>
struct async_send_request_op final {
    async_send_request_op(AsyncWriteStream &sock, Handler &&handler)
        : sock_ {sock}, handler_ {std::forward<Handler>(handler)} {
    }

    void operator()(const boost::system::error_code &err, std::size_t tx) {
        handler_(err);
    }

    template <typename... Arg>
    inline void initiate(Arg &&... args) {
        req_.put<std::uint8_t>(0x05);
        (req_.put(std::forward<Arg>(args)), ...);

        boost::asio::async_write(sock_, boost::asio::buffer(req_.buffer()),
                                 std::move(*this));
    }

  private:
    AsyncWriteStream &sock_;
    Handler           handler_;
    socks5::request   req_;
};

template <typename AsyncWriteStream, typename CompletionToken, typename... Arg>
inline decltype(auto) async_send_request(AsyncWriteStream &sock,
                                         CompletionToken &&token,
                                         Arg &&... args) {
    GENERATE_ERROR_HANDLER(token, handler, result);

    async_send_request_op<AsyncWriteStream, handler_type> {sock,
                                                           std::move(handler)}
        .initiate(std::forward<Arg>(args)...);

    return result.get();
}

} // namespace socks5::detail::async

#endif
