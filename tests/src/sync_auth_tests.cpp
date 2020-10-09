#include "socks5/detail/sync/auth.hpp"
#include "socks5/response.hpp"
#include "socks5/tests/fixtures/socket.hpp"
#include "socks5/tests/util/random.hpp"

#include <boost/system/error_code.hpp>
#include <gtest/gtest.h>

#include <string>

namespace socks5::tests {

struct SyncAuthTests : socks5::tests::fixtures::SocketTest<true> {
    boost::system::error_code ec;
};

TEST_F(SyncAuthTests, AuthBeginSuccessTest) {
    constexpr auto method =
        socks5::detail::sync::auth_method::username_password;
    constexpr std::array<std::uint8_t, 2> respbuf {
        5, static_cast<std::uint8_t>(method)};

    auto tx =
        boost::asio::write(socket.input_pipe(), boost::asio::buffer(respbuf));
    EXPECT_EQ(tx, 2);

    socks5::detail::sync::auth_begin<method>(socket, ec);
    EXPECT_FALSE(ec);

    socks5::response_fixed<3> resp {};
    socks5::detail::sync::read_response(socket.output_pipe(), resp, ec);
    EXPECT_FALSE(ec);
    EXPECT_EQ(5, resp.take<std::uint8_t>());
    EXPECT_EQ(1, resp.take<std::uint8_t>());
    EXPECT_EQ(static_cast<std::uint8_t>(method), resp.take<std::uint8_t>());
}

TEST_F(SyncAuthTests, AuthBeginFailureTest) {
    constexpr auto method =
        socks5::detail::sync::auth_method::username_password;
    constexpr std::array<std::uint8_t, 2> respbuf {5, 0xFF};

    auto tx =
        boost::asio::write(socket.input_pipe(), boost::asio::buffer(respbuf));
    EXPECT_EQ(tx, 2);

    socks5::detail::sync::auth_begin<method>(socket, ec);
    EXPECT_TRUE(ec);
    EXPECT_EQ(ec, socks5::make_error_code(
                      socks5::error_code::auth_method_not_supported));
    EXPECT_FALSE(socket.is_open());
}

TEST_F(SyncAuthTests, AuthNoneTest) {
    constexpr std::array<std::uint8_t, 2> respbuf {
        5, static_cast<std::uint8_t>(socks5::detail::sync::auth_method::none)};

    auto tx =
        boost::asio::write(socket.input_pipe(), boost::asio::buffer(respbuf));
    EXPECT_EQ(tx, 2);

    socks5::detail::sync::auth_none(socket, ec);
    EXPECT_FALSE(ec);
}

TEST_F(SyncAuthTests, AuthUsernamePasswordTest) {
    constexpr auto method = static_cast<std::uint8_t>(
        socks5::detail::sync::auth_method::username_password);
    std::vector<std::uint8_t> respbuf {5, method, 5, 0};

    auto tx =
        boost::asio::write(socket.input_pipe(), boost::asio::buffer(respbuf));
    EXPECT_EQ(tx, 4);

    std::string inuname {socks5::tests::util::random_string(0xAB)},
        inpwd {socks5::tests::util::random_string(0xAB)}, outuname {},
        outpwd {};

    socks5::detail::sync::auth_username_password(socket, inuname, inpwd, ec);
    EXPECT_FALSE(ec);

    respbuf.resize(3 + 1 + 2 + inuname.size() + inpwd.size());
    tx = boost::asio::read(socket.output_pipe(), boost::asio::buffer(respbuf));
    socks5::response<decltype(respbuf)> resp {std::move(respbuf)};

    EXPECT_EQ(tx, resp.size());
    EXPECT_EQ(5, resp.take<std::uint8_t>());
    EXPECT_EQ(1, resp.take<std::uint8_t>());
    EXPECT_EQ(method, resp.take<std::uint8_t>());
    EXPECT_EQ(1, resp.take<std::uint8_t>());
    EXPECT_EQ(inuname, resp.take<std::string>());
    EXPECT_EQ(inpwd, resp.take<std::string>());
}

} // namespace socks5::tests
