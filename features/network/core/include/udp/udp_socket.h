// ==========================================================================
// udp_socket.h
// UDP Socket Class / فئة مقبس UDP
// ==========================================================================

#pragma once

#include "socket_base.h"
#include <vector>
#include <string>

namespace sad {
namespace network {

// ==========================================================================
// UdpSocket - Connectionless datagram communication
// ==========================================================================

class UdpSocket : public SocketBase {
public:
    // Constructor
    UdpSocket();
    
    // Bind to address/port
    void bind(const SocketAddress& address);
    void bind(uint16_t port);
    
    // Send datagram
    size_t send_to(const void* data, size_t size, const SocketAddress& address);
    size_t send_to(const std::vector<uint8_t>& data, const SocketAddress& address);
    size_t send_to(const std::string& text, const SocketAddress& address);
    
    // Receive datagram
    size_t receive_from(void* buffer, size_t size, SocketAddress& sender);
    size_t receive_from(std::vector<uint8_t>& buffer, size_t max_size, SocketAddress& sender);
    std::string receive_string_from(size_t max_size, SocketAddress& sender);
    
    // Connect UDP (optional - for send/receive without address)
    void connect(const SocketAddress& address);
    void disconnect();
    bool is_connected() const;
    
    // Send/receive on connected UDP
    size_t send(const void* data, size_t size);
    size_t send(const std::vector<uint8_t>& data);
    size_t send(const std::string& text);
    size_t receive(void* buffer, size_t size);
    size_t receive(std::vector<uint8_t>& buffer, size_t max_size);
    std::string receive_string(size_t max_size = 4096);
    
    // Broadcast
    void enable_broadcast(bool enable);
    void send_broadcast(const void* data, size_t size, uint16_t port);
    void send_broadcast(const std::vector<uint8_t>& data, uint16_t port);
    void send_broadcast(const std::string& text, uint16_t port);
    
    // Multicast
    void join_multicast_group(const std::string& group_address);
    void leave_multicast_group(const std::string& group_address);
    void set_multicast_ttl(int ttl);
    void set_multicast_loopback(bool enable);

private:
    bool m_is_connected;
};

} // namespace network
} // namespace sad
