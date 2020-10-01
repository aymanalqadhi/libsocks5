#include <gtest/gtest.h>

#include "socks5/basic_stream.hpp"
#include "socks5/tests/fakes/socket.hpp"

#include <iostream>

#include <algorithm>
#include <string>
#include <vector>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>

namespace socks5::tests {

struct BasicStreamTests : ::testing::Test {
    using socket_type = socks5::tests::fakes::socket;

    BasicStreamTests() : socket {io} {
    }

    boost::asio::io_context           io;
    socks5::basic_stream<socket_type> socket;
};

TEST_F(BasicStreamTests, ConnectTest) {
    ASSERT_FALSE(socket.is_socks5_open());

    socket.async_socks5_open("1.2.3.4", 1234, [this](const auto &err) {
        EXPECT_FALSE(err);
        EXPECT_TRUE(socket.is_socks5_open());
    });
    io.run();

    socket.socks5_close();
    ASSERT_FALSE(socket.is_socks5_open());
}

} // namespace socks5::tests
