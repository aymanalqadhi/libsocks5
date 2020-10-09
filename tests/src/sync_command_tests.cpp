#include "socks5/detail/sync/command.hpp"
#include "socks5/tests/fixtures/socket.hpp"
#include "socks5/tests/util/random.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>
#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <vector>

namespace socks5::tests {

namespace {

template <socks5::detail::sync::command cmd>
struct SOCKS5CommandTest : socks5::tests::fixtures::SocketTest<true> {
    inline void initiate_test() {
        remote = socks5::tests::util::random_endpoint();

        auto conn_ep = socks5::tests::util::random_endpoint();

        socks5::request<5> req {};
        req.put<std::uint8_t>(0);
        req.put<std::uint8_t>(0);
        req.put(conn_ep.address());
        req.put(conn_ep.port());

        auto tx = boost::asio::write(socket.input_pipe(),
                                     boost::asio::buffer(req.buffer()));
        EXPECT_EQ(tx, req.size());

        socks5::detail::sync::send_command<cmd>(socket, remote, local, ec);
        EXPECT_FALSE(ec);
        EXPECT_EQ(conn_ep, local);

        socks5::request<5> expected_req {};
        req.put<std::uint8_t>(5);
        req.put<std::uint8_t>(static_cast<std::uint8_t>(cmd));
        req.put<std::uint8_t>(0);
        req.put(remote.address());
        req.put(remote.port());

        std::vector<std::uint8_t> expected_buf(expected_req.size());
        tx = boost::asio::read(socket.output_pipe(),
                               boost::asio::buffer(expected_buf));
        EXPECT_EQ(tx, expected_req.size());
        EXPECT_TRUE(std::equal(expected_buf.begin(), expected_buf.end(),
                               expected_req.begin(), expected_req.end()));
    }

  private:
    boost::system::error_code      ec;
    boost::asio::ip::tcp::endpoint local, remote;
};

} // namespace

struct SyncTcpConnectCommandTest
    : SOCKS5CommandTest<socks5::detail::sync::command::connect> {};
struct SyncTcpBindCommandTest
    : SOCKS5CommandTest<socks5::detail::sync::command::bind> {};
struct SyncUdpAssociateCommandTest
    : SOCKS5CommandTest<socks5::detail::sync::command::udp_associate> {};

TEST_F(SyncTcpConnectCommandTest, InitTest) {
    initiate_test();
}

TEST_F(SyncTcpBindCommandTest, InitTest) {
    initiate_test();
}

TEST_F(SyncUdpAssociateCommandTest, InitTest) {
    initiate_test();
}

} // namespace socks5::tests
