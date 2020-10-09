#include <gtest/gtest.h>

#include "socks5/request.hpp"

#include <algorithm>
#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <boost/asio/ip/address.hpp>

namespace socks5::tests {

struct RequestTests : ::testing::Test {
    socks5::request<5> req {};
};

TEST_F(RequestTests, PutUintTest) {
    req.put<std::uint8_t>(0xAB);

    ASSERT_EQ(req.size(), 1);
    EXPECT_EQ(req[0], 0xAB);

    req.put<std::uint16_t>(0xABCD);
    ASSERT_EQ(req.size(), 3);
    EXPECT_EQ(req[1], 0xAB);
    EXPECT_EQ(req[2], 0xCD);

    req.put<std::uint32_t>(0xABCDEF01);
    ASSERT_EQ(req.size(), 7);
    EXPECT_EQ(req[3], 0xAB);
    EXPECT_EQ(req[4], 0xCD);
    EXPECT_EQ(req[5], 0xEF);
    EXPECT_EQ(req[6], 0x01);

    req.put<std::uint64_t>(0xABCDEF0123456789);
    ASSERT_EQ(req.size(), 15);
    EXPECT_EQ(req[7], 0xAB);
    EXPECT_EQ(req[8], 0xCD);
    EXPECT_EQ(req[9], 0xEF);
    EXPECT_EQ(req[10], 0x01);
    EXPECT_EQ(req[11], 0x23);
    EXPECT_EQ(req[12], 0x45);
    EXPECT_EQ(req[13], 0x67);
    EXPECT_EQ(req[14], 0x89);
}

TEST_F(RequestTests, PutStringTest) {
    std::string str {"Hello, World!"};

    req.put(str);
    ASSERT_EQ(str.size() + 1, req.size());
    EXPECT_EQ(str.size(), req.buffer()[0]);
    EXPECT_TRUE(std::equal(req.begin() + 1, req.end(), str.begin(), str.end()));
}

TEST_F(RequestTests, PutIPv4Test) {
    boost::asio::ip::address_v4 addr {{0xAB, 0xCD, 0xEF, 0x01}};

    req.put(addr);
    ASSERT_EQ(req.size(), 4);
    EXPECT_EQ(req[0], 0xAB);
    EXPECT_EQ(req[1], 0xCD);
    EXPECT_EQ(req[2], 0xEF);
    EXPECT_EQ(req[3], 0x01);
}

TEST_F(RequestTests, PutIPv6Test) {
    boost::asio::ip::address_v6 addr {{0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
                                       0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD,
                                       0xEE, 0xFF}};

    req.put(addr);
    ASSERT_EQ(req.size(), 16);
    EXPECT_EQ(req[0], 0x00);
    EXPECT_EQ(req[1], 0x11);
    EXPECT_EQ(req[2], 0x22);
    EXPECT_EQ(req[3], 0x33);
    EXPECT_EQ(req[4], 0x44);
    EXPECT_EQ(req[5], 0x55);
    EXPECT_EQ(req[6], 0x66);
    EXPECT_EQ(req[7], 0x77);
    EXPECT_EQ(req[8], 0x88);
    EXPECT_EQ(req[9], 0x99);
    EXPECT_EQ(req[10], 0xAA);
    EXPECT_EQ(req[11], 0xBB);
    EXPECT_EQ(req[12], 0xCC);
    EXPECT_EQ(req[13], 0xDD);
    EXPECT_EQ(req[14], 0xEE);
    EXPECT_EQ(req[15], 0xFF);
}

} // namespace socks5::tests
