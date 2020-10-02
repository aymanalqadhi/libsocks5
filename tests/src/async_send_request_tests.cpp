#include "socks5/detail/async/send_request.hpp"
#include "socks5/tests/fixtures/socket.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/asio/read.hpp>
#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <string_view>

#include <iostream>

namespace socks5::tests {

struct AsyncSendRequestTest : socks5::tests::fixtures::SocketTest<true> {};

TEST_F(AsyncSendRequestTest, NumericalRequestTest) {
    constexpr std::uint8_t       b1 {0xAB};
    constexpr std::uint16_t      b2 {0xABCD};
    constexpr std::uint32_t      b4 {0xABCDEF01};
    constexpr std::uint64_t      b8 {0xABCDEF0123456789};
    constexpr std::string_view   str {"Hello, World!"};
    std::array<std::uint8_t, 29> buf {};

    socks5::detail::async::async_send_request(
        socket, [](const auto &err) { EXPECT_FALSE(err); },
        static_cast<std::uint8_t>(0x05), b1, b2, b4, b8, str);
    io.run();

    EXPECT_NO_THROW(
        boost::asio::read(socket.output_pipe(), boost::asio::buffer(buf)));
    EXPECT_EQ(buf[0], 0x05);
    EXPECT_EQ(buf[1], 0xAB);
    EXPECT_EQ(buf[2], 0xAB);
    EXPECT_EQ(buf[3], 0xCD);
    EXPECT_EQ(buf[4], 0xAB);
    EXPECT_EQ(buf[5], 0xCD);
    EXPECT_EQ(buf[6], 0xEF);
    EXPECT_EQ(buf[7], 0x01);
    EXPECT_EQ(buf[8], 0xAB);
    EXPECT_EQ(buf[9], 0xCD);
    EXPECT_EQ(buf[10], 0xEF);
    EXPECT_EQ(buf[11], 0x01);
    EXPECT_EQ(buf[12], 0x23);
    EXPECT_EQ(buf[13], 0x45);
    EXPECT_EQ(buf[14], 0x67);
    EXPECT_EQ(buf[15], 0x89);
    EXPECT_TRUE(
        std::equal(buf.begin() + 17, buf.end(), str.begin(), str.end() - 1));
}

} // namespace socks5::tests
