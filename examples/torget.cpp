#include "socks5/tcp_socket.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/error_code.hpp>

#include <cstdint>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <tuple>

namespace {

using namespace std::literals;

inline auto parse_url(std::string_view url)
    -> std::tuple<std::string, std::string> {
    static std::regex r {
        R"(^(http://)?([a-zA-Z0-9\.\-]+[a-zA-Z0-9]\.[a-zA-Z0-9]+)(/.*)?$)"};

    std::string urlstr {url};
    std::smatch m;

    if (std::regex_search(urlstr, m, r)) {
        return std::make_tuple(m[2], m[3].length() > 0 ? m[3] : "/"s);
    } else {
        throw std::runtime_error {"Invalid url"};
    }
}

inline auto create_tor_socket(boost::asio::io_context &io)
    -> socks5::tcp_stream {
    socks5::tcp_stream sock {io};

    std::clog << "[*] Opening SOCKS5 connection to 127.0.0.1:9050" << std::endl;
    sock.socks5_open(
        boost::asio::ip::tcp::endpoint {boost::asio::ip::tcp::v4(), 9050});

    std::clog << "[*] Authenticating SOCKS5 with none" << std::endl;
    sock.socks5_auth();

    return sock;
}

inline auto make_get_request(std::string_view domain, std::string_view path)
    -> std::string {
    std::stringstream req {};

    req << "GET " << (path.size() > 0 ? path : "/") << " HTTP/1.1\r\n";
    req << "Host: " << domain << "\r\n";
    req << "Connection: close\r\n";
    req << "\r\n";

    return req.str();
}

inline auto read_response(socks5::tcp_stream &sock) -> std::string {
    std::stringstream              resp {};
    std::array<std::uint8_t, 0xFF> buf {};
    std::size_t                    tx;

    std::cout << resp.str() << std::endl;
    boost::system::error_code ec;

    while ((tx = boost::asio::read(sock, boost::asio::buffer(buf), ec))) {
        std::string str {buf.cbegin(), buf.cbegin() + tx};
        resp << str;
    }

    if (ec != boost::asio::error::eof) {
        throw boost::system::system_error {ec};
    }

    return resp.str();
}

inline void tor_http_get(std::string_view url) {
    boost::asio::io_context io;

    auto sock                 = create_tor_socket(io);
    auto [domain, path] = parse_url(url);

    std::clog << "[*] Connecting to: " << domain << std::endl;
    sock.connect(domain, 80);

    auto req = make_get_request(domain, path);

    std::clog << "[*] Sending GET request" << std::endl;
    boost::asio::write(sock, boost::asio::buffer(req));

    std::clog << "[*] Reading response" << std::endl;
    std::cout << read_response(sock) << std::endl;
}

} // namespace

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " url" << std::endl;
        return 1;
    }

    try {
        tor_http_get(argv[1]);
        return 0;
    } catch (const std::exception &ex) {
        std::cerr << "[!] " << ex.what() << std::endl;
        return 1;
    }
}
