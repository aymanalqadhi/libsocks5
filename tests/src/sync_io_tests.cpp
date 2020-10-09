#include "socks5/detail/sync/io.hpp"
#include "socks5/request.hpp"
#include "socks5/tests/fixtures/socket.hpp"
#include "socks5/tests/util/random.hpp"

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/error_code.hpp>
#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <vector>

#include <iostream>

namespace socks5::tests {

struct SyncIOTests : socks5::tests::fixtures::SocketTest<true> {
    inline auto random_buffer() const noexcept -> std::vector<std::uint8_t> {
        std::vector<std::uint8_t> ret(socks5::tests::util::random(0, 1024));

        for (auto &b : ret) {
            b = socks5::tests::util::random<std::uint8_t>();
        }

        return ret;
    }

    boost::system::error_code ec;
};

TEST_F(SyncIOTests, WriteTest) {
    std::vector<std::uint8_t> inbuf {random_buffer()};
    std::vector<std::uint8_t> outbuf(inbuf.size());
    std::size_t               tx;

    tx = socks5::detail::sync::write(socket, boost::asio::buffer(inbuf), ec);
    EXPECT_FALSE(ec);
    EXPECT_EQ(tx, inbuf.size());

    tx = boost::asio::read(socket.output_pipe(), boost::asio::buffer(outbuf),
                           ec);
    EXPECT_FALSE(ec);
    EXPECT_TRUE(
        std::equal(inbuf.begin(), inbuf.end(), outbuf.begin(), outbuf.end()));
    EXPECT_EQ(tx, outbuf.size());
}

TEST_F(SyncIOTests, ReadTest) {
    std::vector<std::uint8_t> inbuf {random_buffer()};
    std::vector<std::uint8_t> outbuf(inbuf.size());
    std::size_t               tx;

    tx =
        boost::asio::write(socket.input_pipe(), boost::asio::buffer(inbuf), ec);
    EXPECT_FALSE(ec);
    EXPECT_EQ(tx, inbuf.size());

    tx = socks5::detail::sync::read(socket, boost::asio::buffer(outbuf), ec);
    EXPECT_FALSE(ec);
    EXPECT_TRUE(
        std::equal(inbuf.begin(), inbuf.end(), outbuf.begin(), outbuf.end()));
    EXPECT_EQ(tx, outbuf.size());
}

TEST_F(SyncIOTests, ReadWriteTest) {
    auto buf1 = random_buffer();
    auto buf2 = random_buffer();

    std::vector<std::uint8_t> buf(buf2.size());
    std::size_t               tx;

    tx = boost::asio::write(socket.input_pipe(), boost::asio::buffer(buf2));
    EXPECT_EQ(tx, buf2.size());

    tx = socks5::detail::sync::write_read(socket, boost::asio::buffer(buf1),
                                          boost::asio::buffer(buf), ec);
    EXPECT_FALSE(ec);
    EXPECT_EQ(tx, buf1.size() + buf2.size());
    EXPECT_TRUE(std::equal(buf2.begin(), buf2.end(), buf.begin(), buf.end()));

    buf.resize(buf1.size());
    tx = boost::asio::read(socket.output_pipe(), boost::asio::buffer(buf));
    EXPECT_EQ(tx, buf1.size());
    EXPECT_TRUE(std::equal(buf1.begin(), buf1.end(), buf.begin(), buf.end()));
}

TEST_F(SyncIOTests, SendRequestTest) {
    auto buf = random_buffer();
    auto str = socks5::tests::util::random_string(0xFF);
    auto req = socks5::make_request<5>();

    req.put<std::uint8_t>(socks5::tests::util::random<std::uint8_t>());
    req.put<std::uint16_t>(socks5::tests::util::random<std::uint16_t>());
    req.put<std::uint32_t>(socks5::tests::util::random<std::uint32_t>());
    req.put<std::uint64_t>(socks5::tests::util::random<std::uint64_t>());
    req.put(buf.cbegin(), buf.cend());
    req.put(str);

    auto tx = socks5::detail::sync::send_request(socket, req, ec);
    EXPECT_FALSE(ec);
    EXPECT_EQ(tx, req.size());

    buf.resize(req.size());
    tx = boost::asio::read(socket.output_pipe(), boost::asio::buffer(buf));
    EXPECT_EQ(tx, buf.size());
    EXPECT_TRUE(std::equal(buf.begin(), buf.end(), req.begin(), req.end()));
}

TEST_F(SyncIOTests, ReadResponseTest) {
    auto req = socks5::make_request<5>();
    req.put<std::uint8_t>(socks5::tests::util::random<std::uint8_t>());
    req.put<std::uint16_t>(socks5::tests::util::random<std::uint16_t>());
    req.put<std::uint32_t>(socks5::tests::util::random<std::uint32_t>());
    req.put<std::uint64_t>(socks5::tests::util::random<std::uint64_t>());
    req.put(socks5::tests::util::random_string(0xFF));

    auto tx = boost::asio::write(socket.input_pipe(),
                                 boost::asio::buffer(req.buffer()));
    EXPECT_EQ(tx, req.size());

    socks5::response_fixed<272> resp {};
    socks5::detail::sync::read_response(socket, resp, ec);
    EXPECT_FALSE(ec);
    EXPECT_TRUE(std::equal(resp.begin(), resp.end(), req.begin(), req.end()));
}

TEST_F(SyncIOTests, ReadEndpointTest) {
    boost::asio::ip::tcp::endpoint ep1;

    auto ep2 = socks5::tests::util::random_endpoint();
    auto req = socks5::make_request<5>(ep2.address(), ep2.port());

    std::vector<std::uint8_t> buf {req.begin() + 1, req.end()};
    auto tx = boost::asio::write(socket.input_pipe(), boost::asio::buffer(buf));
    EXPECT_EQ(tx, buf.size());

    socks5::detail::sync::read_endpoint(socket, ep1, ec);
    EXPECT_FALSE(ec);
    EXPECT_EQ(ep1.address(), ep2.address());
    EXPECT_EQ(ep1.port(), ep2.port());
} // namespace socks5::tests

} // namespace socks5::tests
