// ==========================================================================
// socket_base.cpp
// Base Socket Implementation / تطبيق الفئة الأساسية للمقابس
// ==========================================================================

#include "socket_base.h"
#include "network_error.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
#endif

namespace sad {
namespace network {

bool SocketBase::s_winsock_initialized = false;
int SocketBase::s_winsock_ref_count = 0;

SocketBase::SocketBase()
    : m_socket(INVALID_SOCKET_VALUE)
{
    initialize_winsock();
}

SocketBase::~SocketBase() {
    close();
    cleanup_winsock();
}

SocketBase::SocketBase(SocketBase&& other) noexcept
    : m_socket(other.m_socket)
{
    other.m_socket = INVALID_SOCKET_VALUE;
}

SocketBase& SocketBase::operator=(SocketBase&& other) noexcept {
    if (this != &other) {
        close();
        m_socket = other.m_socket;
        other.m_socket = INVALID_SOCKET_VALUE;
    }
    return *this;
}

void SocketBase::close() {
    if (m_socket != INVALID_SOCKET_VALUE) {
#ifdef _WIN32
        closesocket(m_socket);
#else
        ::close(m_socket);
#endif
        m_socket = INVALID_SOCKET_VALUE;
    }
}

bool SocketBase::is_valid() const {
    return m_socket != INVALID_SOCKET_VALUE;
}

void SocketBase::set_receive_timeout(int milliseconds) {
    if (!is_valid()) {
        throw NetworkException(NetworkErrorCode::SOCKET_CLOSED,
                              "المقبس مغلق",
                              "Socket is closed");
    }
    
#ifdef _WIN32
    DWORD timeout = milliseconds;
    if (setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO,
                   reinterpret_cast<const char*>(&timeout), sizeof(timeout)) != 0) {
#else
    struct timeval timeout;
    timeout.tv_sec = milliseconds / 1000;
    timeout.tv_usec = (milliseconds % 1000) * 1000;
    if (setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO,
                   &timeout, sizeof(timeout)) != 0) {
#endif
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SET_OPTION_FAILED);
    }
}

void SocketBase::set_send_timeout(int milliseconds) {
    if (!is_valid()) {
        throw NetworkException(NetworkErrorCode::SOCKET_CLOSED,
                              "المقبس مغلق",
                              "Socket is closed");
    }
    
#ifdef _WIN32
    DWORD timeout = milliseconds;
    if (setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO,
                   reinterpret_cast<const char*>(&timeout), sizeof(timeout)) != 0) {
#else
    struct timeval timeout;
    timeout.tv_sec = milliseconds / 1000;
    timeout.tv_usec = (milliseconds % 1000) * 1000;
    if (setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO,
                   &timeout, sizeof(timeout)) != 0) {
#endif
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SET_OPTION_FAILED);
    }
}

void SocketBase::set_blocking(bool blocking) {
    if (!is_valid()) {
        throw NetworkException(NetworkErrorCode::SOCKET_CLOSED,
                              "المقبس مغلق",
                              "Socket is closed");
    }
    
#ifdef _WIN32
    u_long mode = blocking ? 0 : 1;
    if (ioctlsocket(m_socket, FIONBIO, &mode) != 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SET_OPTION_FAILED);
    }
#else
    int flags = fcntl(m_socket, F_GETFL, 0);
    if (flags == -1) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::GET_OPTION_FAILED);
    }
    
    if (blocking) {
        flags &= ~O_NONBLOCK;
    } else {
        flags |= O_NONBLOCK;
    }
    
    if (fcntl(m_socket, F_SETFL, flags) == -1) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SET_OPTION_FAILED);
    }
#endif
}

void SocketBase::set_reuse_address(bool reuse) {
    if (!is_valid()) {
        throw NetworkException(NetworkErrorCode::SOCKET_CLOSED,
                              "المقبس مغلق",
                              "Socket is closed");
    }
    
    int opt = reuse ? 1 : 0;
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR,
                   reinterpret_cast<const char*>(&opt), sizeof(opt)) != 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SET_OPTION_FAILED);
    }
}

SocketAddress SocketBase::get_local_address() const {
    if (!is_valid()) {
        throw NetworkException(NetworkErrorCode::SOCKET_CLOSED,
                              "المقبس مغلق",
                              "Socket is closed");
    }
    
    sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    
    if (getsockname(m_socket, reinterpret_cast<sockaddr*>(&addr), &addr_len) != 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::GENERAL_FAILURE);
    }
    
    return SocketAddress(addr);
}

SocketAddress SocketBase::get_remote_address() const {
    if (!is_valid()) {
        throw NetworkException(NetworkErrorCode::SOCKET_CLOSED,
                              "المقبس مغلق",
                              "Socket is closed");
    }
    
    sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    
    if (getpeername(m_socket, reinterpret_cast<sockaddr*>(&addr), &addr_len) != 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SOCKET_NOT_CONNECTED);
    }
    
    return SocketAddress(addr);
}

void SocketBase::create_socket(int domain, int type, int protocol) {
    m_socket = socket(domain, type, protocol);
    if (m_socket == INVALID_SOCKET_VALUE) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SOCKET_CREATION_FAILED);
    }
}

void SocketBase::initialize_winsock() {
#ifdef _WIN32
    if (!s_winsock_initialized) {
        WSADATA wsa_data;
        int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
        if (result != 0) {
            throw NetworkException(NetworkErrorCode::GENERAL_FAILURE,
                                  "فشل تهيئة Winsock",
                                  "Failed to initialize Winsock");
        }
        s_winsock_initialized = true;
    }
    ++s_winsock_ref_count;
#endif
}

void SocketBase::cleanup_winsock() {
#ifdef _WIN32
    --s_winsock_ref_count;
    if (s_winsock_ref_count == 0 && s_winsock_initialized) {
        WSACleanup();
        s_winsock_initialized = false;
    }
#endif
}

} // namespace network
} // namespace sad
