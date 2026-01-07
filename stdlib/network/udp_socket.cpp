// ==========================================================================
// udp_socket.cpp
// UDP Socket Implementation / تطبيق مقبس UDP
// ==========================================================================

#include "network/udp_socket.h"
#include "network/network_error.h"
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif

namespace sad {
namespace network {

UdpSocket::UdpSocket()
    : SocketBase()
    , m_is_connected(false)
{
    create_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

void UdpSocket::bind(const SocketAddress& address) {
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

void UdpSocket::bind(uint16_t port) {
    bind(SocketAddress::any_ipv4(port));
}

size_t UdpSocket::send_to(const void* data, size_t size, 
                          const SocketAddress& address) {
    if (!is_valid()) {
        throw NetworkException(NetworkErrorCode::SOCKET_CLOSED,
                              "المقبس مغلق",
                              "Socket is closed");
    }
    
    int sent = ::sendto(m_socket, static_cast<const char*>(data), 
                        static_cast<int>(size), 0,
                        address.get_sockaddr(), address.get_sockaddr_length());
    
    if (sent < 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SEND_FAILED);
    }
    
    return static_cast<size_t>(sent);
}

size_t UdpSocket::send_to(const std::vector<uint8_t>& data, 
                          const SocketAddress& address) {
    return send_to(data.data(), data.size(), address);
}

size_t UdpSocket::send_to(const std::string& text, 
                          const SocketAddress& address) {
    return send_to(text.data(), text.size(), address);
}

size_t UdpSocket::receive_from(void* buffer, size_t size, 
                               SocketAddress& sender) {
    if (!is_valid()) {
        throw NetworkException(NetworkErrorCode::SOCKET_CLOSED,
                              "المقبس مغلق",
                              "Socket is closed");
    }
    
    sockaddr_storage sender_addr;
    socklen_t sender_len = sizeof(sender_addr);
    
    int received = ::recvfrom(m_socket, static_cast<char*>(buffer), 
                              static_cast<int>(size), 0,
                              reinterpret_cast<sockaddr*>(&sender_addr),
                              &sender_len);
    
    if (received < 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::RECEIVE_FAILED);
    }
    
    sender = SocketAddress(sender_addr);
    return static_cast<size_t>(received);
}

size_t UdpSocket::receive_from(std::vector<uint8_t>& buffer, size_t max_size,
                               SocketAddress& sender) {
    buffer.resize(max_size);
    size_t received = receive_from(buffer.data(), max_size, sender);
    buffer.resize(received);
    return received;
}

std::string UdpSocket::receive_string_from(size_t max_size, 
                                           SocketAddress& sender) {
    std::vector<uint8_t> buffer(max_size);
    size_t received = receive_from(buffer.data(), max_size, sender);
    return std::string(buffer.begin(), buffer.begin() + received);
}

void UdpSocket::connect(const SocketAddress& address) {
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

void UdpSocket::disconnect() {
    if (!is_valid() || !m_is_connected) {
        return;
    }
    
#ifdef _WIN32
    // Windows: Connect to AF_UNSPEC to disconnect
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_UNSPEC;
    ::connect(m_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
#else
    // Linux: Connect to AF_UNSPEC to disconnect
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_UNSPEC;
    ::connect(m_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
#endif
    
    m_is_connected = false;
}

bool UdpSocket::is_connected() const {
    return m_is_connected;
}

size_t UdpSocket::send(const void* data, size_t size) {
    if (!m_is_connected) {
        throw NetworkException(NetworkErrorCode::SOCKET_NOT_CONNECTED,
                              "UDP غير متصل - استخدم send_to أو اتصل أولاً",
                              "UDP not connected - use send_to or connect first");
    }
    
    int sent = ::send(m_socket, static_cast<const char*>(data), 
                      static_cast<int>(size), 0);
    
    if (sent < 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SEND_FAILED);
    }
    
    return static_cast<size_t>(sent);
}

size_t UdpSocket::send(const std::vector<uint8_t>& data) {
    return send(data.data(), data.size());
}

size_t UdpSocket::send(const std::string& text) {
    return send(text.data(), text.size());
}

size_t UdpSocket::receive(void* buffer, size_t size) {
    if (!m_is_connected) {
        throw NetworkException(NetworkErrorCode::SOCKET_NOT_CONNECTED,
                              "UDP غير متصل - استخدم receive_from",
                              "UDP not connected - use receive_from");
    }
    
    int received = ::recv(m_socket, static_cast<char*>(buffer), 
                          static_cast<int>(size), 0);
    
    if (received < 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::RECEIVE_FAILED);
    }
    
    return static_cast<size_t>(received);
}

size_t UdpSocket::receive(std::vector<uint8_t>& buffer, size_t max_size) {
    buffer.resize(max_size);
    size_t received = receive(buffer.data(), max_size);
    buffer.resize(received);
    return received;
}

std::string UdpSocket::receive_string(size_t max_size) {
    std::vector<uint8_t> buffer(max_size);
    size_t received = receive(buffer.data(), max_size);
    return std::string(buffer.begin(), buffer.begin() + received);
}

void UdpSocket::enable_broadcast(bool enable) {
    if (!is_valid()) {
        throw NetworkException(NetworkErrorCode::SOCKET_CLOSED,
                              "المقبس مغلق",
                              "Socket is closed");
    }
    
    int opt = enable ? 1 : 0;
    if (setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST,
                   reinterpret_cast<const char*>(&opt), sizeof(opt)) != 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SET_OPTION_FAILED);
    }
}

void UdpSocket::send_broadcast(const void* data, size_t size, uint16_t port) {
    SocketAddress broadcast_addr("255.255.255.255", port);
    send_to(data, size, broadcast_addr);
}

void UdpSocket::send_broadcast(const std::vector<uint8_t>& data, uint16_t port) {
    send_broadcast(data.data(), data.size(), port);
}

void UdpSocket::send_broadcast(const std::string& text, uint16_t port) {
    send_broadcast(text.data(), text.size(), port);
}

void UdpSocket::join_multicast_group(const std::string& group_address) {
    if (!is_valid()) {
        throw NetworkException(NetworkErrorCode::SOCKET_CLOSED,
                              "المقبس مغلق",
                              "Socket is closed");
    }
    
    struct ip_mreq mreq;
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    
    if (inet_pton(AF_INET, group_address.c_str(), &mreq.imr_multiaddr) != 1) {
        throw NetworkException(NetworkErrorCode::INVALID_ADDRESS,
                              "عنوان multicast غير صالح",
                              "Invalid multicast address");
    }
    
    if (setsockopt(m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                   reinterpret_cast<const char*>(&mreq), sizeof(mreq)) != 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::MULTICAST_JOIN_FAILED);
    }
}

void UdpSocket::leave_multicast_group(const std::string& group_address) {
    if (!is_valid()) {
        throw NetworkException(NetworkErrorCode::SOCKET_CLOSED,
                              "المقبس مغلق",
                              "Socket is closed");
    }
    
    struct ip_mreq mreq;
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    
    if (inet_pton(AF_INET, group_address.c_str(), &mreq.imr_multiaddr) != 1) {
        throw NetworkException(NetworkErrorCode::INVALID_ADDRESS,
                              "عنوان multicast غير صالح",
                              "Invalid multicast address");
    }
    
    if (setsockopt(m_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                   reinterpret_cast<const char*>(&mreq), sizeof(mreq)) != 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::MULTICAST_LEAVE_FAILED);
    }
}

void UdpSocket::set_multicast_ttl(int ttl) {
    if (!is_valid()) {
        throw NetworkException(NetworkErrorCode::SOCKET_CLOSED,
                              "المقبس مغلق",
                              "Socket is closed");
    }
    
#ifdef _WIN32
    DWORD ttl_value = static_cast<DWORD>(ttl);
#else
    int ttl_value = ttl;
#endif
    
    if (setsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_TTL,
                   reinterpret_cast<const char*>(&ttl_value), 
                   sizeof(ttl_value)) != 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SET_OPTION_FAILED);
    }
}

void UdpSocket::set_multicast_loopback(bool enable) {
    if (!is_valid()) {
        throw NetworkException(NetworkErrorCode::SOCKET_CLOSED,
                              "المقبس مغلق",
                              "Socket is closed");
    }
    
#ifdef _WIN32
    DWORD opt = enable ? 1 : 0;
#else
    int opt = enable ? 1 : 0;
#endif
    
    if (setsockopt(m_socket, IPPROTO_IP, IP_MULTICAST_LOOP,
                   reinterpret_cast<const char*>(&opt), sizeof(opt)) != 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SET_OPTION_FAILED);
    }
}

} // namespace network
} // namespace sad
