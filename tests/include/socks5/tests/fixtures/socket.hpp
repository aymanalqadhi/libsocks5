#ifndef LIBSOCKS5_TESTS_FIXTURES_SOCKET_HPP
#define LIBSOCKS5_TESTS_FIXTURES_SOCKET_HPP

#include "socks5/tests/fakes/socket.hpp"

#include "gtest/gtest.h"
#include <boost/asio/io_context.hpp>

namespace socks5::tests::fixtures {

template <bool Connected = false>
struct SocketTest : ::testing::Test {
    SocketTest() : socket {io, Connected} {
    }

    void SetUp() override {
    }

    boost::asio::io_context      io;
    socks5::tests::fakes::socket socket;
};

} // namespace socks5::tests::fixtures

#endif
