#include "socks5/basic_tcp_socket.hpp"
#include "socks5/request.hpp"
#include "socks5/tests/fixtures/basic_socks5_socket.hpp"
#include "socks5/tests/util/random.hpp"

#include <boost/system/error_code.hpp>
#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <vector>

namespace socks5::tests {

struct SyncBasicTcpSocketTest : socks5::tests::fixtures::SyncBasicSocketFixtrue<
                                    socks5::basic_tcp_socket> {
    inline void tcp_connect() {
        auto ep  = socks5::tests::util::random_endpoint();
        auto bnd = socks5::tests::util::random_endpoint();
        auto req = socks5::make_request<5>();
        req.put<std::uint8_t>(0);
        req.put<std::uint8_t>(0);
        req.put(bnd.address());
        req.put(bnd.port());

        auto tx = boost::asio::write(socket.stream().input_pipe(),
                                     boost::asio::buffer(req.buffer()));
        EXPECT_EQ(tx, req.size());

        EXPECT_FALSE(socket.is_open());
        socket.connect(ep, ec);
        EXPECT_FALSE(ec);
        EXPECT_TRUE(socket.is_open());
        EXPECT_EQ(socket.local_endpoint(), bnd);
        EXPECT_EQ(socket.remote_endpoint(), ep);
    }
};

TEST_F(SyncBasicTcpSocketTest, ConnectTest) {
    socks5_open();
    socks5_auth_none();
    tcp_connect();
    socks5_close();
}

TEST_F(SyncBasicTcpSocketTest, WriteSomeTest) {
    socks5_open();
    socks5_auth_none();
    tcp_connect();

    auto str = socks5::tests::util::random_string(0x10);

    socket.stream().clear_output_pipe();
    auto tx = boost::asio::write(socket, boost::asio::buffer(str), ec);
    EXPECT_FALSE(ec);
    EXPECT_EQ(tx, str.size());

    std::vector<std::uint8_t> buf(str.size());
    tx = boost::asio::read(socket.stream().output_pipe(),
                           boost::asio::buffer(buf));
    EXPECT_EQ(tx, buf.size());
    EXPECT_TRUE(std::equal(buf.begin(), buf.end(), str.begin(), str.end()));

    socks5_close();
}

TEST_F(SyncBasicTcpSocketTest, ReadSomeTest) {
    socks5_open();
    socks5_auth_none();
    tcp_connect();

    auto str = socks5::tests::util::random_string(0x10);

    socket.stream().clear_input_pipe();
    auto tx = boost::asio::write(socket.stream().input_pipe(),
                                 boost::asio::buffer(str));
    EXPECT_EQ(tx, str.size());

    std::vector<std::uint8_t> buf(str.size());
    tx = boost::asio::read(socket, boost::asio::buffer(buf), ec);
    EXPECT_FALSE(ec);
    EXPECT_EQ(tx, buf.size());
    EXPECT_TRUE(std::equal(buf.begin(), buf.end(), str.begin(), str.end()));

    socks5_close();
}

} // namespace socks5::tests
