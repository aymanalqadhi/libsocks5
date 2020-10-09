#include "socks5/basic_socket.hpp"
#include "socks5/tests/fixtures/basic_socks5_socket.hpp"

#include <boost/system/error_code.hpp>
#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <type_traits>

namespace socks5::tests {

struct SyncBasicSocketTest
    : socks5::tests::fixtures::SyncBasicSocketFixtrue<socks5::basic_socket> {};

TEST_F(SyncBasicSocketTest, OpenTest) {
    socks5_open();
}

TEST_F(SyncBasicSocketTest, CloseTest) {
    socks5_open();
    socks5_close();
}

TEST_F(SyncBasicSocketTest, AuthNoneTest) {
    socks5_open();
    socks5_auth_none();
    socks5_close();
}

TEST_F(SyncBasicSocketTest, AuthUsernamePasswordTest) {
    socks5_open();
    socks5_auth_username_password();
    socks5_close();
}

} // namespace socks5::tests
