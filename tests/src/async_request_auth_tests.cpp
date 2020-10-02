#include "socks5/detail/async/request_auth.hpp"
#include "socks5/error_code.hpp"
#include "socks5/tests/fixtures/socket.hpp"
#include "socks5/tests/util/random.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <string_view>

#include <iostream>

#include <version>
namespace socks5::tests {

struct AsyncRequestAuthTest
    : socks5::tests::fixtures::SocketTest<true> {};

TEST_F(AsyncRequestAuthTest, NormalValuesTest) {
    std::array<std::uint8_t, 2> respbuf {0x05, 0xFF};

    auto rnd = []() -> std::uint8_t {
        return socks5::tests::util::random<std::uint8_t>();
    };

    auto r1 = rnd(), r2 = rnd(), r3 = rnd(), r4 = rnd(), chosen = rnd();

    boost::asio::write(socket.input_pipe(), boost::asio::buffer(respbuf));
    socks5::detail::async::async_request_auth(
        socket,
        [](const auto &err, auto selected) {
            EXPECT_TRUE(err);
            EXPECT_EQ(err.value(),
                      static_cast<int>(
                          socks5::error_code::auth_method_not_supported));
        },
        r1, r2, r3, r4, chosen);
    io.run();

    respbuf[1] = chosen;
    boost::asio::write(socket.input_pipe(), boost::asio::buffer(respbuf));
    socks5::detail::async::async_request_auth(
        socket, [](const auto &err, auto selected) { EXPECT_FALSE(err); }, r1,
        r2, r3, r4, chosen);
    io.run();
}

} // namespace socks5::tests
