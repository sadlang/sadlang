// ==========================================================================
// socket_base.h
// Base Socket Class / الفئة الأساسية للمقابس
// ==========================================================================

#pragma once

#include "network/socket_address.h"
#include <memory>

#ifdef _WIN32
    #include <winsock2.h>
    using socket_t = SOCKET;
    const socket_t INVALID_SOCKET_VALUE = INVALID_SOCKET;
#else
    using socket_t = int;
    const socket_t INVALID_SOCKET_VALUE = -1;
#endif

namespace sad {
namespace network {

// ==========================================================================
// SocketBase - Base class for TCP and UDP sockets
// ==========================================================================

class SocketBase {
public:
    // Constructor
    SocketBase();
    
    // Destructor (virtual for inheritance)
    virtual ~SocketBase();
    
    // Delete copy constructor and assignment (sockets are unique)
    SocketBase(const SocketBase&) = delete;
    SocketBase& operator=(const SocketBase&) = delete;
    
    // Move constructor and assignment
    SocketBase(SocketBase&& other) noexcept;
    SocketBase& operator=(SocketBase&& other) noexcept;
    
    // Close the socket
    void close();
    
    // Check if socket is valid/open
    bool is_valid() const;
    
    // Set socket timeout (milliseconds, 0 = no timeout)
    void set_receive_timeout(int milliseconds);
    void set_send_timeout(int milliseconds);
    
    // Enable/disable blocking mode
    void set_blocking(bool blocking);
    
    // Enable/disable address reuse (SO_REUSEADDR)
    void set_reuse_address(bool reuse);
    
    // Get local address (after bind)
    SocketAddress get_local_address() const;
    
    // Get remote address (for connected sockets)
    SocketAddress get_remote_address() const;

protected:
    socket_t m_socket;
    
    // Create socket (to be called by derived classes)
    void create_socket(int domain, int type, int protocol);
    
    // Initialize Winsock (Windows only)
    static void initialize_winsock();
    static void cleanup_winsock();
    
private:
    static bool s_winsock_initialized;
    static int s_winsock_ref_count;
};

} // namespace network
} // namespace sad
