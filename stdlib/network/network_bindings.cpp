// بسم الله الرحمن الرحيم
// تنفيذ واجهة C++ للغة ص
// Implementation of C++ interface for Sad language

#include "network/network_bindings.h"
#include "network/tcp_socket.h"
#include "network/udp_socket.h"
#include "network/socket_address.h"
#include "network/network_error.h"
#include <cstring>
#include <string>
#include <memory>

using namespace sad::network;

// Global error state
static std::string last_error_message;
static int last_error_code = 0;

// Helper to set error
static void set_last_error(const NetworkException& ex) {
    last_error_code = static_cast<int>(ex.get_code());
    last_error_message = ex.what();
}

// Helper to clear error
static void clear_error() {
    last_error_code = 0;
    last_error_message.clear();
}

extern "C" {

// ===== NetworkError Functions =====

const char* sad_network_error_arabic(int error_code) {
    static std::string result;
    result = error_code_to_arabic(static_cast<NetworkErrorCode>(error_code));
    return result.c_str();
}

const char* sad_network_error_english(int error_code) {
    static std::string result;
    result = error_code_to_english(static_cast<NetworkErrorCode>(error_code));
    return result.c_str();
}

// ===== SocketAddress Functions =====

void* sad_socket_address_create(const char* ip, uint16_t port) {
    try {
        clear_error();
        return new SocketAddress(ip, port);
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return nullptr;
    }
}

void* sad_socket_address_create_v6(const char* ip, uint16_t port) {
    try {
        clear_error();
        return new SocketAddress(ip, port);
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return nullptr;
    }
}

void sad_socket_address_delete(void* addr) {
    delete static_cast<SocketAddress*>(addr);
}

const char* sad_socket_address_get_ip(void* addr) {
    static std::string result;
    result = static_cast<SocketAddress*>(addr)->get_ip();
    return result.c_str();
}

uint16_t sad_socket_address_get_port(void* addr) {
    return static_cast<SocketAddress*>(addr)->get_port();
}

bool sad_socket_address_is_ipv4(void* addr) {
    return static_cast<SocketAddress*>(addr)->is_ipv4();
}

bool sad_socket_address_is_ipv6(void* addr) {
    return static_cast<SocketAddress*>(addr)->is_ipv6();
}

const char* sad_socket_address_to_string(void* addr) {
    static std::string result;
    result = static_cast<SocketAddress*>(addr)->to_string();
    return result.c_str();
}

// ===== TcpSocket Functions =====

void* sad_tcp_socket_create() {
    try {
        clear_error();
        return new TcpSocket();
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return nullptr;
    }
}

void sad_tcp_socket_delete(void* socket) {
    delete static_cast<TcpSocket*>(socket);
}

bool sad_tcp_socket_bind(void* socket, uint16_t port) {
    try {
        clear_error();
        static_cast<TcpSocket*>(socket)->bind(port);
        return true;
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return false;
    }
}

bool sad_tcp_socket_bind_addr(void* socket, const char* address, uint16_t port) {
    try {
        clear_error();
        static_cast<TcpSocket*>(socket)->bind(address, port);
        return true;
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return false;
    }
}

bool sad_tcp_socket_listen(void* socket, int backlog) {
    try {
        clear_error();
        static_cast<TcpSocket*>(socket)->listen(backlog);
        return true;
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return false;
    }
}

void* sad_tcp_socket_accept(void* socket) {
    try {
        clear_error();
        TcpSocket client = static_cast<TcpSocket*>(socket)->accept();
        return new TcpSocket(std::move(client));
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return nullptr;
    }
}

bool sad_tcp_socket_connect(void* socket, const char* address, uint16_t port) {
    try {
        clear_error();
        static_cast<TcpSocket*>(socket)->connect(address, port);
        return true;
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return false;
    }
}

int sad_tcp_socket_send(void* socket, const char* data, size_t size) {
    try {
        clear_error();
        return static_cast<int>(
            static_cast<TcpSocket*>(socket)->send(data, size)
        );
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return -1;
    }
}

bool sad_tcp_socket_send_all(void* socket, const char* data, size_t size) {
    try {
        clear_error();
        static_cast<TcpSocket*>(socket)->send_all(data, size);
        return true;
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return false;
    }
}

int sad_tcp_socket_receive(void* socket, char* buffer, size_t size) {
    try {
        clear_error();
        return static_cast<int>(
            static_cast<TcpSocket*>(socket)->receive(buffer, size)
        );
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return -1;
    }
}

bool sad_tcp_socket_receive_exact(void* socket, char* buffer, size_t size) {
    try {
        clear_error();
        static_cast<TcpSocket*>(socket)->receive_exact(buffer, size);
        return true;
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return false;
    }
}

bool sad_tcp_socket_is_connected(void* socket) {
    return static_cast<TcpSocket*>(socket)->is_connected();
}

bool sad_tcp_socket_is_valid(void* socket) {
    return static_cast<TcpSocket*>(socket)->is_valid();
}

void sad_tcp_socket_close(void* socket) {
    static_cast<TcpSocket*>(socket)->close();
}

void sad_tcp_socket_shutdown_write(void* socket) {
    try {
        static_cast<TcpSocket*>(socket)->shutdown_write();
    } catch (const NetworkException& ex) {
        set_last_error(ex);
    }
}

void sad_tcp_socket_shutdown_read(void* socket) {
    try {
        static_cast<TcpSocket*>(socket)->shutdown_read();
    } catch (const NetworkException& ex) {
        set_last_error(ex);
    }
}

void sad_tcp_socket_set_receive_timeout(void* socket, int milliseconds) {
    try {
        static_cast<TcpSocket*>(socket)->set_receive_timeout(milliseconds);
    } catch (const NetworkException& ex) {
        set_last_error(ex);
    }
}

void sad_tcp_socket_set_send_timeout(void* socket, int milliseconds) {
    try {
        static_cast<TcpSocket*>(socket)->set_send_timeout(milliseconds);
    } catch (const NetworkException& ex) {
        set_last_error(ex);
    }
}

void sad_tcp_socket_set_reuse_address(void* socket, bool enable) {
    try {
        static_cast<TcpSocket*>(socket)->set_reuse_address(enable);
    } catch (const NetworkException& ex) {
        set_last_error(ex);
    }
}

void sad_tcp_socket_set_keepalive(void* socket, bool enable) {
    try {
        static_cast<TcpSocket*>(socket)->set_keepalive(enable);
    } catch (const NetworkException& ex) {
        set_last_error(ex);
    }
}

void sad_tcp_socket_set_no_delay(void* socket, bool enable) {
    try {
        static_cast<TcpSocket*>(socket)->set_no_delay(enable);
    } catch (const NetworkException& ex) {
        set_last_error(ex);
    }
}

void* sad_tcp_socket_get_peer_address(void* socket) {
    try {
        clear_error();
        SocketAddress addr = static_cast<TcpSocket*>(socket)->get_peer_address();
        return new SocketAddress(addr);
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return nullptr;
    }
}

void* sad_tcp_socket_get_local_address(void* socket) {
    try {
        clear_error();
        SocketAddress addr = static_cast<TcpSocket*>(socket)->get_local_address();
        return new SocketAddress(addr);
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return nullptr;
    }
}

// ===== UdpSocket Functions =====

void* sad_udp_socket_create() {
    try {
        clear_error();
        return new UdpSocket();
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return nullptr;
    }
}

void sad_udp_socket_delete(void* socket) {
    delete static_cast<UdpSocket*>(socket);
}

bool sad_udp_socket_bind(void* socket, uint16_t port) {
    try {
        clear_error();
        static_cast<UdpSocket*>(socket)->bind(port);
        return true;
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return false;
    }
}

bool sad_udp_socket_bind_addr(void* socket, const char* address, uint16_t port) {
    try {
        clear_error();
        static_cast<UdpSocket*>(socket)->bind(address, port);
        return true;
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return false;
    }
}

int sad_udp_socket_send_to(void* socket, const char* data, size_t size,
                             const char* address, uint16_t port) {
    try {
        clear_error();
        SocketAddress addr(address, port);
        return static_cast<int>(
            static_cast<UdpSocket*>(socket)->send_to(data, size, addr)
        );
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return -1;
    }
}

int sad_udp_socket_receive_from(void* socket, char* buffer, size_t size,
                                  char* from_address, uint16_t* from_port) {
    try {
        clear_error();
        SocketAddress from;
        size_t bytes = static_cast<UdpSocket*>(socket)->receive_from(
            buffer, size, from
        );
        
        if (from_address) {
            std::string ip = from.get_ip();
            std::strncpy(from_address, ip.c_str(), 46); // IPv6 max length
            from_address[45] = '\0';
        }
        
        if (from_port) {
            *from_port = from.get_port();
        }
        
        return static_cast<int>(bytes);
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return -1;
    }
}

bool sad_udp_socket_connect(void* socket, const char* address, uint16_t port) {
    try {
        clear_error();
        static_cast<UdpSocket*>(socket)->connect(address, port);
        return true;
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return false;
    }
}

int sad_udp_socket_send(void* socket, const char* data, size_t size) {
    try {
        clear_error();
        return static_cast<int>(
            static_cast<UdpSocket*>(socket)->send(data, size)
        );
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return -1;
    }
}

int sad_udp_socket_receive(void* socket, char* buffer, size_t size) {
    try {
        clear_error();
        return static_cast<int>(
            static_cast<UdpSocket*>(socket)->receive(buffer, size)
        );
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return -1;
    }
}

bool sad_udp_socket_join_multicast(void* socket, const char* group) {
    try {
        clear_error();
        static_cast<UdpSocket*>(socket)->join_multicast_group(group);
        return true;
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return false;
    }
}

bool sad_udp_socket_leave_multicast(void* socket, const char* group) {
    try {
        clear_error();
        static_cast<UdpSocket*>(socket)->leave_multicast_group(group);
        return true;
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return false;
    }
}

void sad_udp_socket_set_multicast_ttl(void* socket, int ttl) {
    try {
        static_cast<UdpSocket*>(socket)->set_multicast_ttl(ttl);
    } catch (const NetworkException& ex) {
        set_last_error(ex);
    }
}

void sad_udp_socket_set_multicast_loopback(void* socket, bool enable) {
    try {
        static_cast<UdpSocket*>(socket)->set_multicast_loopback(enable);
    } catch (const NetworkException& ex) {
        set_last_error(ex);
    }
}

void sad_udp_socket_set_broadcast(void* socket, bool enable) {
    try {
        static_cast<UdpSocket*>(socket)->set_broadcast(enable);
    } catch (const NetworkException& ex) {
        set_last_error(ex);
    }
}

void sad_udp_socket_set_receive_timeout(void* socket, int milliseconds) {
    try {
        static_cast<UdpSocket*>(socket)->set_receive_timeout(milliseconds);
    } catch (const NetworkException& ex) {
        set_last_error(ex);
    }
}

void sad_udp_socket_set_reuse_address(void* socket, bool enable) {
    try {
        static_cast<UdpSocket*>(socket)->set_reuse_address(enable);
    } catch (const NetworkException& ex) {
        set_last_error(ex);
    }
}

bool sad_udp_socket_is_valid(void* socket) {
    return static_cast<UdpSocket*>(socket)->is_valid();
}

void sad_udp_socket_close(void* socket) {
    static_cast<UdpSocket*>(socket)->close();
}

void* sad_udp_socket_get_local_address(void* socket) {
    try {
        clear_error();
        SocketAddress addr = static_cast<UdpSocket*>(socket)->get_local_address();
        return new SocketAddress(addr);
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return nullptr;
    }
}

void sad_udp_socket_set_receive_buffer_size(void* socket, int size) {
    try {
        static_cast<UdpSocket*>(socket)->set_receive_buffer_size(size);
    } catch (const NetworkException& ex) {
        set_last_error(ex);
    }
}

void sad_udp_socket_set_send_buffer_size(void* socket, int size) {
    try {
        static_cast<UdpSocket*>(socket)->set_send_buffer_size(size);
    } catch (const NetworkException& ex) {
        set_last_error(ex);
    }
}

int sad_udp_socket_get_receive_buffer_size(void* socket) {
    try {
        return static_cast<UdpSocket*>(socket)->get_receive_buffer_size();
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return -1;
    }
}

int sad_udp_socket_get_send_buffer_size(void* socket) {
    try {
        return static_cast<UdpSocket*>(socket)->get_send_buffer_size();
    } catch (const NetworkException& ex) {
        set_last_error(ex);
        return -1;
    }
}

// ===== Utility Functions =====

bool sad_network_init() {
#ifdef _WIN32
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    return result == 0;
#else
    return true; // No initialization needed on Unix-like systems
#endif
}

void sad_network_cleanup() {
#ifdef _WIN32
    WSACleanup();
#endif
}

int sad_network_get_last_error() {
    return last_error_code;
}

const char* sad_network_get_last_error_message() {
    return last_error_message.c_str();
}

} // extern "C"
