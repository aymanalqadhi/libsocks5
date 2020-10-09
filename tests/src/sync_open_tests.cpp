#include "socks5/detail/sync/open.hpp"
#include "socks5/tests/fixtures/socket.hpp"
#include "socks5/tests/util/random.hpp"

#include <boost/system/error_code.hpp>

namespace socks5::tests {

struct SyncOpenTests : socks5::tests::fixtures::SocketTest<false> {};

TEST_F(SyncOpenTests, IsOpenTest) {
    boost::system::error_code ec {};
    auto is_open = socks5::detail::sync::is_open(socket, ec);

    EXPECT_FALSE(socket.is_open());
    EXPECT_FALSE(is_open);
    EXPECT_EQ(ec, boost::asio::error::not_connected);

    socks5::detail::sync::open(socket, socks5::tests::util::random_endpoint(),
                               ec);
    is_open = socks5::detail::sync::is_open(socket, ec);

    EXPECT_TRUE(socket.is_open());
    EXPECT_TRUE(is_open);
    EXPECT_FALSE(ec);
}

TEST_F(SyncOpenTests, FailureTest) {
    boost::system::error_code ec {};
    auto                      ep = socks5::tests::util::random_endpoint();

    socket.always_fails(true);
    socks5::detail::sync::open(socket, ep, ec);

    EXPECT_NE(socket.remote_endpoint(), ep);
    EXPECT_FALSE(socket.is_open());
    EXPECT_TRUE(ec);
}

TEST_F(SyncOpenTests, SuccessTest) {
    boost::system::error_code ec {};

    auto ep = socks5::tests::util::random_endpoint();
    socks5::detail::sync::open(socket, ep, ec);

    EXPECT_EQ(socket.remote_endpoint(), ep);
    EXPECT_TRUE(socket.is_open());
    EXPECT_FALSE(ec);
}

} // namespace socks5::tests
