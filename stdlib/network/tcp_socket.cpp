// ==========================================================================
// tcp_socket.cpp
// TCP Socket Implementation / تطبيق مقبس TCP
// ==========================================================================

#include "network/tcp_socket.h"
#include "network/network_error.h"
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <arpa/inet.h>
#endif

namespace sad {
namespace network {

TcpSocket::TcpSocket()
    : SocketBase()
    , m_is_connected(false)
    , m_is_listening(false)
{
    create_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

TcpSocket::TcpSocket(socket_t sock)
    : SocketBase()
    , m_is_connected(true)
    , m_is_listening(false)
{
    m_socket = sock;
}

void TcpSocket::connect(const SocketAddress& address) {
    if (m_is_connected) {
        throw NetworkException(NetworkErrorCode::SOCKET_ALREADY_CONNECTED,
                              "المقبس متصل بالفعل",
                              "Socket is already connected");
    }
    
    if (!is_valid()) {
        throw NetworkException(NetworkErrorCode::SOCKET_CLOSED,
                              "المقبس مغلق",
                              "Socket is closed");
    }
    
    if (::connect(m_socket, address.get_sockaddr(), 
                  address.get_sockaddr_length()) != 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SOCKET_CONNECT_FAILED);
    }
    
    m_is_connected = true;
}

void TcpSocket::connect(const std::string& host, uint16_t port) {
    connect(SocketAddress(host, port));
}

void TcpSocket::bind(const SocketAddress& address) {
    if (!is_valid()) {
        throw NetworkException(NetworkErrorCode::SOCKET_CLOSED,
                              "المقبس مغلق",
                              "Socket is closed");
    }
    
    if (::bind(m_socket, address.get_sockaddr(), 
               address.get_sockaddr_length()) != 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SOCKET_BIND_FAILED);
    }
}

void TcpSocket::bind(uint16_t port) {
    bind(SocketAddress::any_ipv4(port));
}

void TcpSocket::listen(int backlog) {
    if (!is_valid()) {
        throw NetworkException(NetworkErrorCode::SOCKET_CLOSED,
                              "المقبس مغلق",
                              "Socket is closed");
    }
    
    if (::listen(m_socket, backlog) != 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SOCKET_LISTEN_FAILED);
    }
    
    m_is_listening = true;
}

TcpSocket TcpSocket::accept() {
    if (!is_valid() || !m_is_listening) {
        throw NetworkException(NetworkErrorCode::GENERAL_FAILURE,
                              "المقبس غير جاهز للاستماع",
                              "Socket is not in listening state");
    }
    
    sockaddr_storage client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    socket_t client_sock = ::accept(m_socket, 
                                     reinterpret_cast<sockaddr*>(&client_addr),
                                     &client_len);
    
    if (client_sock == INVALID_SOCKET_VALUE) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SOCKET_ACCEPT_FAILED);
    }
    
    return TcpSocket(client_sock);
}

SocketAddress TcpSocket::accept(TcpSocket& client_socket) {
    if (!is_valid() || !m_is_listening) {
        throw NetworkException(NetworkErrorCode::GENERAL_FAILURE,
                              "المقبس غير جاهز للاستماع",
                              "Socket is not in listening state");
    }
    
    sockaddr_storage client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    socket_t client_sock = ::accept(m_socket, 
                                     reinterpret_cast<sockaddr*>(&client_addr),
                                     &client_len);
    
    if (client_sock == INVALID_SOCKET_VALUE) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SOCKET_ACCEPT_FAILED);
    }
    
    client_socket = TcpSocket(client_sock);
    return SocketAddress(client_addr);
}

size_t TcpSocket::send(const void* data, size_t size) {
    if (!is_valid() || !m_is_connected) {
        throw NetworkException(NetworkErrorCode::SOCKET_NOT_CONNECTED,
                              "المقبس غير متصل",
                              "Socket is not connected");
    }
    
    int sent = ::send(m_socket, static_cast<const char*>(data), 
                      static_cast<int>(size), 0);
    
    if (sent < 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SEND_FAILED);
    }
    
    return static_cast<size_t>(sent);
}

size_t TcpSocket::send(const std::vector<uint8_t>& data) {
    return send(data.data(), data.size());
}

size_t TcpSocket::send(const std::string& text) {
    return send(text.data(), text.size());
}

size_t TcpSocket::receive(void* buffer, size_t size) {
    if (!is_valid() || !m_is_connected) {
        throw NetworkException(NetworkErrorCode::SOCKET_NOT_CONNECTED,
                              "المقبس غير متصل",
                              "Socket is not connected");
    }
    
    int received = ::recv(m_socket, static_cast<char*>(buffer), 
                          static_cast<int>(size), 0);
    
    if (received < 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::RECEIVE_FAILED);
    }
    
    if (received == 0) {
        m_is_connected = false;
    }
    
    return static_cast<size_t>(received);
}

size_t TcpSocket::receive(std::vector<uint8_t>& buffer, size_t max_size) {
    buffer.resize(max_size);
    size_t received = receive(buffer.data(), max_size);
    buffer.resize(received);
    return received;
}

std::string TcpSocket::receive_string(size_t max_size) {
    std::vector<uint8_t> buffer(max_size);
    size_t received = receive(buffer.data(), max_size);
    return std::string(buffer.begin(), buffer.begin() + received);
}

void TcpSocket::send_all(const void* data, size_t size) {
    const uint8_t* ptr = static_cast<const uint8_t*>(data);
    size_t total_sent = 0;
    
    while (total_sent < size) {
        size_t sent = send(ptr + total_sent, size - total_sent);
        if (sent == 0) {
            throw NetworkException(NetworkErrorCode::CONNECTION_RESET,
                                  "تم إغلاق الاتصال أثناء الإرسال",
                                  "Connection closed during send");
        }
        total_sent += sent;
    }
}

void TcpSocket::send_all(const std::vector<uint8_t>& data) {
    send_all(data.data(), data.size());
}

void TcpSocket::send_all(const std::string& text) {
    send_all(text.data(), text.size());
}

void TcpSocket::receive_all(void* buffer, size_t size) {
    uint8_t* ptr = static_cast<uint8_t*>(buffer);
    size_t total_received = 0;
    
    while (total_received < size) {
        size_t received = receive(ptr + total_received, size - total_received);
        if (received == 0) {
            throw NetworkException(NetworkErrorCode::CONNECTION_RESET,
                                  "تم إغلاق الاتصال أثناء الاستقبال",
                                  "Connection closed during receive");
        }
        total_received += received;
    }
}

void TcpSocket::receive_all(std::vector<uint8_t>& buffer, size_t size) {
    buffer.resize(size);
    receive_all(buffer.data(), size);
}

void TcpSocket::shutdown_send() {
    if (is_valid()) {
#ifdef _WIN32
        ::shutdown(m_socket, SD_SEND);
#else
        ::shutdown(m_socket, SHUT_WR);
#endif
    }
}

void TcpSocket::shutdown_receive() {
    if (is_valid()) {
#ifdef _WIN32
        ::shutdown(m_socket, SD_RECEIVE);
#else
        ::shutdown(m_socket, SHUT_RD);
#endif
    }
}

void TcpSocket::shutdown_both() {
    if (is_valid()) {
#ifdef _WIN32
        ::shutdown(m_socket, SD_BOTH);
#else
        ::shutdown(m_socket, SHUT_RDWR);
#endif
    }
}

void TcpSocket::set_nodelay(bool nodelay) {
    if (!is_valid()) {
        throw NetworkException(NetworkErrorCode::SOCKET_CLOSED,
                              "المقبس مغلق",
                              "Socket is closed");
    }
    
    int opt = nodelay ? 1 : 0;
    if (setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY,
                   reinterpret_cast<const char*>(&opt), sizeof(opt)) != 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SET_OPTION_FAILED);
    }
}

void TcpSocket::set_keepalive(bool keepalive) {
    if (!is_valid()) {
        throw NetworkException(NetworkErrorCode::SOCKET_CLOSED,
                              "المقبس مغلق",
                              "Socket is closed");
    }
    
    int opt = keepalive ? 1 : 0;
    if (setsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE,
                   reinterpret_cast<const char*>(&opt), sizeof(opt)) != 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SET_OPTION_FAILED);
    }
}

void TcpSocket::set_linger(bool enable, int seconds) {
    if (!is_valid()) {
        throw NetworkException(NetworkErrorCode::SOCKET_CLOSED,
                              "المقبس مغلق",
                              "Socket is closed");
    }
    
#ifdef _WIN32
    struct linger lng;
    lng.l_onoff = enable ? 1 : 0;
    lng.l_linger = static_cast<u_short>(seconds);
#else
    struct linger lng;
    lng.l_onoff = enable ? 1 : 0;
    lng.l_linger = seconds;
#endif
    
    if (setsockopt(m_socket, SOL_SOCKET, SO_LINGER,
                   reinterpret_cast<const char*>(&lng), sizeof(lng)) != 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SET_OPTION_FAILED);
    }
}

bool TcpSocket::is_connected() const {
    return is_valid() && m_is_connected;
}

} // namespace network
} // namespace sad
