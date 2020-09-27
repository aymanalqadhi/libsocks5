#include <gtest/gtest.h>

#include "socks5/response.hpp"

#include <string>
#include <vector>

#include <boost/asio/ip/address.hpp>

namespace socks5::tests {

TEST(ResponseTests, TakeUintTest) {
    socks5::response resp {{0xAB, 0xAB, 0xCD, 0xAB, 0xCD, 0xEF, 0x01, 0xAB,
                            0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89}};

    std::uint8_t  b1;
    std::uint16_t b2;
    std::uint32_t b4;
    std::uint64_t b8;

    ASSERT_EQ(resp.size(), 1 + 2 + 4 + 8);

    EXPECT_TRUE(resp.take(b1));
    EXPECT_TRUE(resp.take(b2));
    EXPECT_TRUE(resp.take(b4));
    EXPECT_TRUE(resp.take(b8));

    ASSERT_EQ(resp.size(), 0);

    EXPECT_EQ(b1, 0xAB);
    EXPECT_EQ(b2, 0xABCD);
    EXPECT_EQ(b4, 0xABCDEF01);
    EXPECT_EQ(b8, 0xABCDEF0123456789);
}

TEST(ResponseTests, TakeRangeTest) {
    std::string      orig {"Hello, World!"}, out;
    socks5::response resp {{orig.begin(), orig.end()}};

    ASSERT_EQ(resp.size(), orig.size());

    EXPECT_TRUE(resp.take(out.end(), 5));
    EXPECT_STREQ("Hello", out.c_str());
    EXPECT_EQ(resp.size(), 8);

    out.clear();

    EXPECT_TRUE(resp.skip(2));
    EXPECT_EQ(resp.size(), 6);
    EXPECT_TRUE(resp.take(out.end(), 5));
    EXPECT_EQ(resp.size(), 1);
    EXPECT_STREQ("World", out.c_str());
}

} // namespace socks5::tests
