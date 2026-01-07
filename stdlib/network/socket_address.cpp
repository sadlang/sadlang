// ==========================================================================
// socket_address.cpp
// Socket Address Implementation / تطبيق عناوين المقابس
// ==========================================================================

#include "network/socket_address.h"
#include "network/network_error.h"
#include <cstring>

namespace sad {
namespace network {

SocketAddress::SocketAddress() {
    std::memset(&m_addr, 0, sizeof(m_addr));
    m_addr_len = 0;
}

SocketAddress::SocketAddress(const std::string& ip, uint16_t port) {
    std::memset(&m_addr, 0, sizeof(m_addr));
    
    // Try IPv4 first
    sockaddr_in* addr4 = reinterpret_cast<sockaddr_in*>(&m_addr);
    if (inet_pton(AF_INET, ip.c_str(), &addr4->sin_addr) == 1) {
        addr4->sin_family = AF_INET;
        addr4->sin_port = htons(port);
        m_addr_len = sizeof(sockaddr_in);
        return;
    }
    
    // Try IPv6
    sockaddr_in6* addr6 = reinterpret_cast<sockaddr_in6*>(&m_addr);
    if (inet_pton(AF_INET6, ip.c_str(), &addr6->sin6_addr) == 1) {
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = htons(port);
        m_addr_len = sizeof(sockaddr_in6);
        return;
    }
    
    throw NetworkException(NetworkErrorCode::INVALID_ADDRESS,
                          "عنوان IP غير صالح: " + ip,
                          "Invalid IP address: " + ip);
}

SocketAddress::SocketAddress(uint16_t port) {
    std::memset(&m_addr, 0, sizeof(m_addr));
    sockaddr_in* addr4 = reinterpret_cast<sockaddr_in*>(&m_addr);
    addr4->sin_family = AF_INET;
    addr4->sin_addr.s_addr = INADDR_ANY;
    addr4->sin_port = htons(port);
    m_addr_len = sizeof(sockaddr_in);
}

SocketAddress::SocketAddress(const sockaddr_storage& addr) {
    std::memcpy(&m_addr, &addr, sizeof(addr));
    if (m_addr.ss_family == AF_INET) {
        m_addr_len = sizeof(sockaddr_in);
    } else if (m_addr.ss_family == AF_INET6) {
        m_addr_len = sizeof(sockaddr_in6);
    } else {
        m_addr_len = 0;
    }
}

std::string SocketAddress::get_ip() const {
    char ip_str[INET6_ADDRSTRLEN];
    
    if (m_addr.ss_family == AF_INET) {
        const sockaddr_in* addr4 = reinterpret_cast<const sockaddr_in*>(&m_addr);
        inet_ntop(AF_INET, &addr4->sin_addr, ip_str, sizeof(ip_str));
    } else if (m_addr.ss_family == AF_INET6) {
        const sockaddr_in6* addr6 = reinterpret_cast<const sockaddr_in6*>(&m_addr);
        inet_ntop(AF_INET6, &addr6->sin6_addr, ip_str, sizeof(ip_str));
    } else {
        return "";
    }
    
    return std::string(ip_str);
}

uint16_t SocketAddress::get_port() const {
    if (m_addr.ss_family == AF_INET) {
        const sockaddr_in* addr4 = reinterpret_cast<const sockaddr_in*>(&m_addr);
        return ntohs(addr4->sin_port);
    } else if (m_addr.ss_family == AF_INET6) {
        const sockaddr_in6* addr6 = reinterpret_cast<const sockaddr_in6*>(&m_addr);
        return ntohs(addr6->sin6_port);
    }
    return 0;
}

bool SocketAddress::is_ipv4() const {
    return m_addr.ss_family == AF_INET;
}

bool SocketAddress::is_ipv6() const {
    return m_addr.ss_family == AF_INET6;
}

sockaddr* SocketAddress::get_sockaddr() {
    return reinterpret_cast<sockaddr*>(&m_addr);
}

const sockaddr* SocketAddress::get_sockaddr() const {
    return reinterpret_cast<const sockaddr*>(&m_addr);
}

socklen_t SocketAddress::get_sockaddr_length() const {
    return m_addr_len;
}

SocketAddress SocketAddress::any_ipv4(uint16_t port) {
    return SocketAddress(port);
}

SocketAddress SocketAddress::any_ipv6(uint16_t port) {
    sockaddr_storage addr;
    std::memset(&addr, 0, sizeof(addr));
    sockaddr_in6* addr6 = reinterpret_cast<sockaddr_in6*>(&addr);
    addr6->sin6_family = AF_INET6;
    addr6->sin6_addr = in6addr_any;
    addr6->sin6_port = htons(port);
    return SocketAddress(addr);
}

SocketAddress SocketAddress::localhost_ipv4(uint16_t port) {
    return SocketAddress("127.0.0.1", port);
}

SocketAddress SocketAddress::localhost_ipv6(uint16_t port) {
    return SocketAddress("::1", port);
}

bool SocketAddress::is_valid_ipv4(const std::string& ip) {
    sockaddr_in addr;
    return inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) == 1;
}

bool SocketAddress::is_valid_ipv6(const std::string& ip) {
    sockaddr_in6 addr;
    return inet_pton(AF_INET6, ip.c_str(), &addr.sin6_addr) == 1;
}

} // namespace network
} // namespace sad
