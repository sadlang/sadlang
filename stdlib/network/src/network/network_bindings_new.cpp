// بسم الله الرحمن الرحيم
// تنفيذ واجهة C++ للغة ص
// Implementation of C++ interface for Sad language
// Updated to match current API

#include "network/network_bindings.h"
#include "socket/socket_base.h"
#include "tcp/tcp_socket.h"
#include "udp/udp_socket.h"
#include "socket/socket_address.h"
#include "network/network_error.h"
#include <cstring>
#include <string>
#include <memory>
#include <sstream>

using namespace sad::network;

// Global error state
static std::string last_error_message;
static int last_error_code = 0;

// Helper to set error
static void set_last_error(const NetworkException &ex)
{
    last_error_code = static_cast<int>(ex.error_code());
    last_error_message = ex.what();
}

// Helper to clear error
static void clear_error()
{
    last_error_code = 0;
    last_error_message.clear();
}

// Helper: Convert SocketAddress to string
static std::string socket_address_to_string(const SocketAddress &addr)
{
    std::ostringstream oss;
    oss << addr.get_ip() << ":" << addr.get_port();
    return oss.str();
}

static bool ensure_socket_handle(void *socket)
{
    if (socket)
    {
        return true;
    }
    last_error_code = static_cast<int>(NetworkErrorCode::GENERAL_FAILURE);
    last_error_message = "null socket handle";
    return false;
}

extern "C"
{

    // ===== NetworkError Functions =====

    const char *sad_network_error_arabic(int error_code)
    {
        static std::string result;
        result = error_code_to_arabic(static_cast<NetworkErrorCode>(error_code));
        return result.c_str();
    }

    const char *sad_network_error_english(int error_code)
    {
        static std::string result;
        result = error_code_to_english(static_cast<NetworkErrorCode>(error_code));
        return result.c_str();
    }

    // ===== SocketAddress Functions =====

    void *sad_socket_address_create(const char *ip, uint16_t port)
    {
        try
        {
            clear_error();
            return new SocketAddress(ip, port);
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return nullptr;
        }
    }

    void *sad_socket_address_create_v6(const char *ip, uint16_t port)
    {
        try
        {
            clear_error();
            return new SocketAddress(ip, port);
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return nullptr;
        }
    }

    void sad_socket_address_delete(void *addr)
    {
        delete static_cast<SocketAddress *>(addr);
    }

    const char *sad_socket_address_get_ip(void *addr)
    {
        static std::string result;
        if (!addr)
        {
            result.clear();
            return result.c_str();
        }
        try
        {
            result = static_cast<SocketAddress *>(addr)->get_ip();
        }
        catch (...)
        {
            result.clear();
        }
        return result.c_str();
    }

    uint16_t sad_socket_address_get_port(void *addr)
    {
        if (!addr)
            return 0;
        try
        {
            return static_cast<SocketAddress *>(addr)->get_port();
        }
        catch (...)
        {
            return 0;
        }
    }

    bool sad_socket_address_is_ipv4(void *addr)
    {
        if (!addr)
            return false;
        try
        {
            return static_cast<SocketAddress *>(addr)->is_ipv4();
        }
        catch (...)
        {
            return false;
        }
    }

    bool sad_socket_address_is_ipv6(void *addr)
    {
        if (!addr)
            return false;
        try
        {
            return static_cast<SocketAddress *>(addr)->is_ipv6();
        }
        catch (...)
        {
            return false;
        }
    }

    const char *sad_socket_address_to_string(void *addr)
    {
        static std::string result;
        if (!addr)
        {
            result.clear();
            return result.c_str();
        }
        try
        {
            result = socket_address_to_string(*static_cast<SocketAddress *>(addr));
        }
        catch (...)
        {
            result.clear();
        }
        return result.c_str();
    }

    // ===== TcpSocket Functions =====

    void *sad_tcp_socket_create()
    {
        try
        {
            clear_error();
            return new TcpSocket();
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return nullptr;
        }
    }

    void sad_tcp_socket_delete(void *socket)
    {
        delete static_cast<TcpSocket *>(socket);
    }

    bool sad_tcp_socket_bind(void *socket, uint16_t port)
    {
        try
        {
            clear_error();
            static_cast<TcpSocket *>(socket)->bind(port);
            return true;
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return false;
        }
    }

    bool sad_tcp_socket_bind_addr(void *socket, const char *address, uint16_t port)
    {
        try
        {
            clear_error();
            SocketAddress addr(address, port);
            static_cast<TcpSocket *>(socket)->bind(addr);
            return true;
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return false;
        }
    }

    bool sad_tcp_socket_listen(void *socket, int backlog)
    {
        try
        {
            clear_error();
            static_cast<TcpSocket *>(socket)->listen(backlog);
            return true;
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return false;
        }
    }

    void *sad_tcp_socket_accept(void *socket)
    {
        try
        {
            clear_error();
            TcpSocket client = static_cast<TcpSocket *>(socket)->accept();
            return new TcpSocket(std::move(client));
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return nullptr;
        }
    }

    bool sad_tcp_socket_connect(void *socket, const char *address, uint16_t port)
    {
        try
        {
            clear_error();
            static_cast<TcpSocket *>(socket)->connect(address, port);
            return true;
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return false;
        }
    }

    int sad_tcp_socket_send(void *socket, const char *data, size_t size)
    {
        try
        {
            clear_error();
            return static_cast<int>(
                static_cast<TcpSocket *>(socket)->send(data, size));
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return -1;
        }
    }

    bool sad_tcp_socket_send_all(void *socket, const char *data, size_t size)
    {
        try
        {
            clear_error();
            static_cast<TcpSocket *>(socket)->send_all(data, size);
            return true;
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return false;
        }
    }

    int sad_tcp_socket_receive(void *socket, char *buffer, size_t size)
    {
        try
        {
            clear_error();
            return static_cast<int>(
                static_cast<TcpSocket *>(socket)->receive(buffer, size));
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return -1;
        }
    }

    bool sad_tcp_socket_receive_exact(void *socket, char *buffer, size_t size)
    {
        try
        {
            clear_error();
            // Using receive_all instead of receive_exact
            static_cast<TcpSocket *>(socket)->receive_all(buffer, size);
            return true;
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return false;
        }
    }

    bool sad_tcp_socket_is_connected(void *socket)
    {
        if (!ensure_socket_handle(socket))
            return false;
        try
        {
            return static_cast<TcpSocket *>(socket)->is_connected();
        }
        catch (...)
        {
            return false;
        }
    }

    bool sad_tcp_socket_is_valid(void *socket)
    {
        if (!ensure_socket_handle(socket))
            return false;
        try
        {
            return static_cast<TcpSocket *>(socket)->is_valid();
        }
        catch (...)
        {
            return false;
        }
    }

    void sad_tcp_socket_close(void *socket)
    {
        static_cast<TcpSocket *>(socket)->close();
    }

    void sad_tcp_socket_shutdown_write(void *socket)
    {
        try
        {
            // Using shutdown_send instead of shutdown_write
            static_cast<TcpSocket *>(socket)->shutdown_send();
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
        }
    }

    void sad_tcp_socket_shutdown_read(void *socket)
    {
        try
        {
            // Using shutdown_receive instead of shutdown_read
            static_cast<TcpSocket *>(socket)->shutdown_receive();
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
        }
    }

    void sad_tcp_socket_set_receive_timeout(void *socket, int milliseconds)
    {
        try
        {
            static_cast<TcpSocket *>(socket)->set_receive_timeout(milliseconds);
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
        }
    }

    void sad_tcp_socket_set_send_timeout(void *socket, int milliseconds)
    {
        try
        {
            static_cast<TcpSocket *>(socket)->set_send_timeout(milliseconds);
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
        }
    }

    void sad_tcp_socket_set_reuse_address(void *socket, bool enable)
    {
        try
        {
            static_cast<TcpSocket *>(socket)->set_reuse_address(enable);
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
        }
    }

    void sad_tcp_socket_set_keepalive(void *socket, bool enable)
    {
        try
        {
            static_cast<TcpSocket *>(socket)->set_keepalive(enable);
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
        }
    }

    void sad_tcp_socket_set_no_delay(void *socket, bool enable)
    {
        try
        {
            // Using set_nodelay instead of set_no_delay
            static_cast<TcpSocket *>(socket)->set_nodelay(enable);
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
        }
    }

    void *sad_tcp_socket_get_peer_address(void *socket)
    {
        try
        {
            clear_error();
            if (!ensure_socket_handle(socket))
            {
                return nullptr;
            }
            SocketAddress addr = static_cast<SocketBase *>(socket)->get_remote_address();
            return new SocketAddress(addr);
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return nullptr;
        }
    }

    const char *sad_tcp_socket_remote_addr(void *socket)
    {
        static thread_local std::string result;
        try
        {
            clear_error();
            if (!ensure_socket_handle(socket))
            {
                result.clear();
                return result.c_str();
            }
            SocketAddress addr = static_cast<SocketBase *>(socket)->get_remote_address();
            result = socket_address_to_string(addr);
            return result.c_str();
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            result.clear();
            return result.c_str();
        }
    }

    const char *sad_tcp_socket_receive_string(void *socket, size_t size)
    {
        static thread_local std::string result;
        if (!ensure_socket_handle(socket))
        {
            result.clear();
            return result.c_str();
        }

        std::string buffer(size, '\0');
        int received = sad_tcp_socket_receive(socket, buffer.data(), size);
        if (received < 0)
        {
            result.clear();
            return result.c_str();
        }

        buffer.resize(static_cast<size_t>(received));
        result = std::move(buffer);
        return result.c_str();
    }

    void *sad_tcp_socket_get_local_address(void *socket)
    {
        try
        {
            clear_error();
            SocketAddress addr = static_cast<TcpSocket *>(socket)->get_local_address();
            return new SocketAddress(addr);
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return nullptr;
        }
    }

    // ===== UdpSocket Functions =====

    void *sad_udp_socket_create()
    {
        try
        {
            clear_error();
            return new UdpSocket();
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return nullptr;
        }
    }

    void sad_udp_socket_delete(void *socket)
    {
        delete static_cast<UdpSocket *>(socket);
    }

    bool sad_udp_socket_bind(void *socket, uint16_t port)
    {
        try
        {
            clear_error();
            static_cast<UdpSocket *>(socket)->bind(port);
            return true;
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return false;
        }
    }

    bool sad_udp_socket_bind_addr(void *socket, const char *address, uint16_t port)
    {
        try
        {
            clear_error();
            SocketAddress addr(address, port);
            static_cast<UdpSocket *>(socket)->bind(addr);
            return true;
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return false;
        }
    }

    int sad_udp_socket_send_to(void *socket, const char *data, size_t size,
                               const char *address, uint16_t port)
    {
        try
        {
            clear_error();
            SocketAddress addr(address, port);
            return static_cast<int>(
                static_cast<UdpSocket *>(socket)->send_to(data, size, addr));
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return -1;
        }
    }

    int sad_udp_socket_receive_from(void *socket, char *buffer, size_t size,
                                    char *from_address, uint16_t *from_port)
    {
        try
        {
            clear_error();
            SocketAddress from;
            size_t bytes = static_cast<UdpSocket *>(socket)->receive_from(
                buffer, size, from);

            if (from_address)
            {
                std::string ip = from.get_ip();
                std::strncpy(from_address, ip.c_str(), 46); // IPv6 max length
                from_address[45] = '\0';
            }

            if (from_port)
            {
                *from_port = from.get_port();
            }

            return static_cast<int>(bytes);
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return -1;
        }
    }

    bool sad_udp_socket_connect(void *socket, const char *address, uint16_t port)
    {
        try
        {
            clear_error();
            SocketAddress addr(address, port);
            static_cast<UdpSocket *>(socket)->connect(addr);
            return true;
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return false;
        }
    }

    int sad_udp_socket_send(void *socket, const char *data, size_t size)
    {
        try
        {
            clear_error();
            return static_cast<int>(
                static_cast<UdpSocket *>(socket)->send(data, size));
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return -1;
        }
    }

    int sad_udp_socket_receive(void *socket, char *buffer, size_t size)
    {
        try
        {
            clear_error();
            return static_cast<int>(
                static_cast<UdpSocket *>(socket)->receive(buffer, size));
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return -1;
        }
    }

    bool sad_udp_socket_join_multicast(void *socket, const char *group)
    {
        try
        {
            clear_error();
            static_cast<UdpSocket *>(socket)->join_multicast_group(group);
            return true;
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return false;
        }
    }

    bool sad_udp_socket_leave_multicast(void *socket, const char *group)
    {
        try
        {
            clear_error();
            static_cast<UdpSocket *>(socket)->leave_multicast_group(group);
            return true;
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return false;
        }
    }

    void sad_udp_socket_set_multicast_ttl(void *socket, int ttl)
    {
        try
        {
            static_cast<UdpSocket *>(socket)->set_multicast_ttl(ttl);
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
        }
    }

    void sad_udp_socket_set_multicast_loopback(void *socket, bool enable)
    {
        try
        {
            static_cast<UdpSocket *>(socket)->set_multicast_loopback(enable);
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
        }
    }

    void sad_udp_socket_set_broadcast(void *socket, bool enable)
    {
        try
        {
            // Using enable_broadcast instead of set_broadcast
            static_cast<UdpSocket *>(socket)->enable_broadcast(enable);
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
        }
    }

    void sad_udp_socket_set_receive_timeout(void *socket, int milliseconds)
    {
        try
        {
            static_cast<UdpSocket *>(socket)->set_receive_timeout(milliseconds);
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
        }
    }

    const char *sad_udp_socket_receive_string(void *socket, size_t size)
    {
        static thread_local std::string result;
        if (!ensure_socket_handle(socket))
        {
            result.clear();
            return result.c_str();
        }

        std::string buffer(size, '\0');
        char from_address[46] = {0};
        uint16_t from_port = 0;
        int received = sad_udp_socket_receive_from(
            socket,
            buffer.data(),
            size,
            from_address,
            &from_port);

        if (received < 0)
        {
            result.clear();
            return result.c_str();
        }

        buffer.resize(static_cast<size_t>(received));
        result = std::move(buffer);
        return result.c_str();
    }

    void sad_udp_socket_set_reuse_address(void *socket, bool enable)
    {
        try
        {
            static_cast<UdpSocket *>(socket)->set_reuse_address(enable);
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
        }
    }

    bool sad_udp_socket_is_valid(void *socket)
    {
        return static_cast<UdpSocket *>(socket)->is_valid();
    }

    void sad_udp_socket_close(void *socket)
    {
        static_cast<UdpSocket *>(socket)->close();
    }

    void *sad_udp_socket_get_local_address(void *socket)
    {
        try
        {
            clear_error();
            SocketAddress addr = static_cast<UdpSocket *>(socket)->get_local_address();
            return new SocketAddress(addr);
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return nullptr;
        }
    }

    void sad_udp_socket_set_receive_buffer_size(void *socket, int size)
    {
        // Not available in current API - no-op with warning
        last_error_code = static_cast<int>(NetworkErrorCode::SET_OPTION_FAILED);
        last_error_message = "set_receive_buffer_size not available";
    }

    void sad_udp_socket_set_send_buffer_size(void *socket, int size)
    {
        // Not available in current API - no-op with warning
        last_error_code = static_cast<int>(NetworkErrorCode::SET_OPTION_FAILED);
        last_error_message = "set_send_buffer_size not available";
    }

    bool sad_socket_set_recv_timeout(void *socket, int milliseconds)
    {
        try
        {
            clear_error();
            if (!ensure_socket_handle(socket))
            {
                return false;
            }
            static_cast<SocketBase *>(socket)->set_receive_timeout(milliseconds);
            return true;
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return false;
        }
    }

    bool sad_socket_set_send_timeout(void *socket, int milliseconds)
    {
        try
        {
            clear_error();
            if (!ensure_socket_handle(socket))
            {
                return false;
            }
            static_cast<SocketBase *>(socket)->set_send_timeout(milliseconds);
            return true;
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return false;
        }
    }

    int sad_udp_socket_get_receive_buffer_size(void *socket)
    {
        // Not available in current API
        last_error_code = static_cast<int>(NetworkErrorCode::GET_OPTION_FAILED);
        last_error_message = "get_receive_buffer_size not available";
        return -1;
    }

    int sad_udp_socket_get_send_buffer_size(void *socket)
    {
        // Not available in current API
        last_error_code = static_cast<int>(NetworkErrorCode::GET_OPTION_FAILED);
        last_error_message = "get_send_buffer_size not available";
        return -1;
    }

    int sad_socket_local_port(void *socket)
    {
        try
        {
            clear_error();
            if (!ensure_socket_handle(socket))
                return -1;
            SocketAddress addr = static_cast<SocketBase *>(socket)->get_local_address();
            return static_cast<int>(addr.get_port());
        }
        catch (const NetworkException &ex)
        {
            set_last_error(ex);
            return -1;
        }
    }

    // ===== Utility Functions =====

    bool sad_network_init()
    {
#ifdef _WIN32
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        return result == 0;
#else
        return true; // No initialization needed on Unix-like systems
#endif
    }

    void sad_network_cleanup()
    {
#ifdef _WIN32
        WSACleanup();
#endif
    }

    int sad_network_get_last_error()
    {
        return last_error_code;
    }

    const char *sad_network_get_last_error_message()
    {
        return last_error_message.c_str();
    }

} // extern "C"
