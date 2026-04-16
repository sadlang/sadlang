// ==========================================================================
// socket_address.h
// Socket Address Management / إدارة عناوين المقابس
// ==========================================================================

#pragma once

#include <string>
#include <cstdint>

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

// ==========================================================================
// SocketAddress Class - IPv4/IPv6 Address Management
// ==========================================================================

class SocketAddress {
public:
    // Constructors
    SocketAddress();  // Default - any address
    SocketAddress(const std::string& ip, uint16_t port);  // From IP string
    SocketAddress(uint16_t port);  // Any address with port
    SocketAddress(const sockaddr_storage& addr);  // From sockaddr
    
    // Get IP address as string
    std::string get_ip() const;
    
    // Get port number
    uint16_t get_port() const;
    
    // Check if IPv4
    bool is_ipv4() const;
    
    // Check if IPv6
    bool is_ipv6() const;
    
    // Get sockaddr pointer (for system calls)
    sockaddr* get_sockaddr();
    const sockaddr* get_sockaddr() const;
    
    // Get sockaddr length
    socklen_t get_sockaddr_length() const;
    
    // Static helpers
    static SocketAddress any_ipv4(uint16_t port);
    static SocketAddress any_ipv6(uint16_t port);
    static SocketAddress localhost_ipv4(uint16_t port);
    static SocketAddress localhost_ipv6(uint16_t port);
    
    // Parse IP address (validates format)
    static bool is_valid_ipv4(const std::string& ip);
    static bool is_valid_ipv6(const std::string& ip);

private:
    sockaddr_storage m_addr;
    socklen_t m_addr_len;
};

} // namespace network
} // namespace sad
