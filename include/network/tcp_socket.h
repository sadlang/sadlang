// ==========================================================================
// tcp_socket.h
// TCP Socket Class / فئة مقبس TCP
// ==========================================================================

#pragma once

#include "network/socket_base.h"
#include <vector>
#include <string>

namespace sad {
namespace network {

// ==========================================================================
// TcpSocket - Reliable stream-oriented connection
// ==========================================================================

class TcpSocket : public SocketBase {
public:
    // Constructor
    TcpSocket();
    
    // Create from existing socket (for accept())
    explicit TcpSocket(socket_t sock);
    
    // Connect to server (client)
    void connect(const SocketAddress& address);
    void connect(const std::string& host, uint16_t port);
    
    // Bind and listen (server)
    void bind(const SocketAddress& address);
    void bind(uint16_t port);  // Bind to any address
    void listen(int backlog = 10);
    
    // Accept incoming connection (server)
    TcpSocket accept();
    SocketAddress accept(TcpSocket& client_socket);
    
    // Send data
    size_t send(const void* data, size_t size);
    size_t send(const std::vector<uint8_t>& data);
    size_t send(const std::string& text);
    
    // Receive data
    size_t receive(void* buffer, size_t size);
    size_t receive(std::vector<uint8_t>& buffer, size_t max_size);
    std::string receive_string(size_t max_size = 4096);
    
    // Send all data (loop until complete)
    void send_all(const void* data, size_t size);
    void send_all(const std::vector<uint8_t>& data);
    void send_all(const std::string& text);
    
    // Receive exact amount (loop until complete)
    void receive_all(void* buffer, size_t size);
    void receive_all(std::vector<uint8_t>& buffer, size_t size);
    
    // Shutdown connection (graceful)
    void shutdown_send();      // No more sends
    void shutdown_receive();   // No more receives
    void shutdown_both();      // Complete shutdown
    
    // TCP Options
    void set_nodelay(bool nodelay);  // Disable Nagle's algorithm
    void set_keepalive(bool keepalive);
    void set_linger(bool enable, int seconds);
    
    // Check connection state
    bool is_connected() const;

private:
    bool m_is_connected;
    bool m_is_listening;
};

} // namespace network
} // namespace sad
