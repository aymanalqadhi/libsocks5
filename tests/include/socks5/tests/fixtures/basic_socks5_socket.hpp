#include "socks5/tests/fakes/socket.hpp"
#include "socks5/tests/util/random.hpp"

#include <boost/asio/write.hpp>
#include <boost/system/error_code.hpp>
#include <gtest/gtest.h>

namespace socks5::tests::fixtures {

template <template <typename> typename Socket,
          bool connected  = false,
          typename Stream = socks5::tests::fakes::socket>
struct SyncBasicSocketFixtrue : ::testing::Test {
    SyncBasicSocketFixtrue() : socket {io} {
        if constexpr (connected) {
            socks5_open();
        }
    }

    void SetUp() override {
    }

    inline void socks5_open() noexcept {
        auto ep = socks5::tests::util::random_endpoint();
        socket.socks5_open(ep, ec);
        EXPECT_FALSE(ec);
        EXPECT_TRUE(socket.is_socks5_open());
        EXPECT_FALSE(socket.is_socks5_authed());
        EXPECT_FALSE(socket.is_socks5_ready());
        EXPECT_EQ(ep, socket.stream().remote_endpoint());
    }

    inline void socks5_close() {
        socket.socks5_close();
        EXPECT_FALSE(socket.is_socks5_open());
        EXPECT_FALSE(socket.is_socks5_authed());
        EXPECT_FALSE(socket.is_socks5_ready());
    }

    inline void socks5_auth_none() {
        std::array<std::uint8_t, 2> sbuf {5, 0};
        boost::asio::write(socket.stream().input_pipe(),
                           boost::asio::buffer(sbuf));

        socket.socks5_auth(ec);

        EXPECT_FALSE(ec);
        EXPECT_TRUE(socket.is_socks5_authed());
        EXPECT_TRUE(socket.is_socks5_ready());
    }

    inline void socks5_auth_username_password() {
        std::array<std::uint8_t, 4> sbuf {5, 2, 5, 0};
        boost::asio::write(socket.stream().input_pipe(),
                           boost::asio::buffer(sbuf));

        socket.socks5_auth(socks5::tests::util::random_string(0xFF),
                           socks5::tests::util::random_string(0xFF), ec);

        EXPECT_FALSE(ec);
        EXPECT_TRUE(socket.is_socks5_authed());
        EXPECT_TRUE(socket.is_socks5_ready());
    }

    boost::system::error_code ec;
    boost::asio::io_context   io;
    Socket<Stream>            socket;
};

} // namespace socks5::tests::fixtures
