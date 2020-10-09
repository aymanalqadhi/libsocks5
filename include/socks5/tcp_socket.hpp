#ifndef LIBSOCKS5_TCP_SOCKET_HPP
#define LIBSOCKS5_TCP_SOCKET_HPP

#include "socks5/basic_tcp_socket.hpp"

#include <boost/asio/ip/tcp.hpp>

namespace socks5 {

using tcp_stream = socks5::basic_tcp_socket<boost::asio::ip::tcp::socket>;

}

#endif
