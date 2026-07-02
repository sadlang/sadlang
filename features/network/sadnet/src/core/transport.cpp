// ===================================================================
// صNet — تنفيذ طبقة النقل (UDP/TCP)
// (AR) مقابس UDP و TCP متعددة المنصات مع دعم WinSock2/POSIX
// (EN) Cross-platform UDP/TCP socket implementation
// المكون: features/network/sadnet (طبقة النقل UDP/TCP)
// المسار: features/network/sadnet/src/core/transport.cpp
// ===================================================================

#include "sadnet/core/transport.h"
#include <cstring>
#include <mutex>

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#else
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#endif

namespace sad::net
{

    // ─────────────────────────────────────────────────────────────────────
    // (AR) تهيئة/تنظيف المنصة
    // ─────────────────────────────────────────────────────────────────────

    static std::once_flag s_init_flag;
    static bool s_initialized = false;

    void platform_socket_init()
    {
        std::call_once(s_init_flag, []()
                       {
#ifdef _WIN32
        WSADATA wsa;
        WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
        s_initialized = true; });
    }

    void platform_socket_cleanup()
    {
#ifdef _WIN32
        if (s_initialized)
        {
            WSACleanup();
            s_initialized = false;
        }
#endif
    }

    // ─────────────────────────────────────────────────────────────────────
    // (AR) دوال مساعدة محلية
    // ─────────────────────────────────────────────────────────────────────

    /// تحويل Endpoint إلى sockaddr_in
    static bool endpoint_to_sockaddr(const Endpoint &ep, struct sockaddr_in &addr)
    {
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(ep.port);
        if (ep.address.empty() || ep.address == "0.0.0.0")
        {
            addr.sin_addr.s_addr = INADDR_ANY;
            return true;
        }
        return inet_pton(AF_INET, ep.address.c_str(), &addr.sin_addr) == 1;
    }

    /// تحويل sockaddr_in إلى Endpoint
    static Endpoint sockaddr_to_endpoint(const struct sockaddr_in &addr)
    {
        char buf[INET_ADDRSTRLEN] = {};
        inet_ntop(AF_INET, &addr.sin_addr, buf, sizeof(buf));
        return Endpoint(buf, ntohs(addr.sin_port), false);
    }

    /// تحويل خطأ النظام إلى NetError
    static NetError system_error_to_net()
    {
#ifdef _WIN32
        int err = WSAGetLastError();
        switch (err)
        {
        case WSAETIMEDOUT:
            return NetError::TIMEOUT;
        case WSAECONNREFUSED:
            return NetError::CONNECTION_REFUSED;
        case WSAECONNRESET:
            return NetError::CONNECTION_RESET;
        case WSAENETUNREACH:
            return NetError::NETWORK_UNREACHABLE;
        case WSAEADDRINUSE:
            return NetError::ADDRESS_IN_USE;
        default:
            return NetError::INTERNAL_ERROR;
        }
#else
        switch (errno)
        {
        case ETIMEDOUT:
            return NetError::TIMEOUT;
        case ECONNREFUSED:
            return NetError::CONNECTION_REFUSED;
        case ECONNRESET:
            return NetError::CONNECTION_RESET;
        case ENETUNREACH:
            return NetError::NETWORK_UNREACHABLE;
        case EADDRINUSE:
            return NetError::ADDRESS_IN_USE;
        default:
            return NetError::INTERNAL_ERROR;
        }
#endif
    }

    static void close_socket(socket_t &s)
    {
        if (s != SADNET_INVALID_SOCKET)
        {
#ifdef _WIN32
            closesocket(s);
#else
            ::close(s);
#endif
            s = SADNET_INVALID_SOCKET;
        }
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) تنفيذ UdpSocket
    // ═════════════════════════════════════════════════════════════════════

    UdpSocket::UdpSocket()
    {
        platform_socket_init();
        sock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }

    UdpSocket::~UdpSocket()
    {
        close();
    }

    UdpSocket::UdpSocket(UdpSocket &&other) noexcept
        : sock_(other.sock_), bound_(other.bound_)
    {
        other.sock_ = SADNET_INVALID_SOCKET;
        other.bound_ = false;
    }

    UdpSocket &UdpSocket::operator=(UdpSocket &&other) noexcept
    {
        if (this != &other)
        {
            close();
            sock_ = other.sock_;
            bound_ = other.bound_;
            other.sock_ = SADNET_INVALID_SOCKET;
            other.bound_ = false;
        }
        return *this;
    }

    Result<void> UdpSocket::bind(const Endpoint &local)
    {
        if (!is_valid())
            return Result<void>::fail(NetError::INTERNAL_ERROR);
        struct sockaddr_in addr;
        if (!endpoint_to_sockaddr(local, addr))
            return Result<void>::fail(NetError::INVALID_ARGUMENT);
        if (::bind(sock_, (struct sockaddr *)&addr, sizeof(addr)) != 0)
            return Result<void>::fail(system_error_to_net());
        bound_ = true;
        return Result<void>::success();
    }

    Result<void> UdpSocket::bind(uint16_t port)
    {
        return bind(Endpoint("0.0.0.0", port));
    }

    Result<size_t> UdpSocket::send_to(const Endpoint &dest,
                                      const uint8_t *data, size_t len)
    {
        if (!is_valid())
            return Result<size_t>::fail(NetError::INTERNAL_ERROR);
        struct sockaddr_in addr;
        if (!endpoint_to_sockaddr(dest, addr))
            return Result<size_t>::fail(NetError::INVALID_ARGUMENT);
        auto sent = sendto(sock_, (const char *)data, (int)len, 0,
                           (struct sockaddr *)&addr, sizeof(addr));
        if (sent < 0)
            return Result<size_t>::fail(system_error_to_net());
        return Result<size_t>::success(static_cast<size_t>(sent));
    }

    Result<size_t> UdpSocket::send_to(const Endpoint &dest, const Buffer &buf)
    {
        return send_to(dest, buf.data(), buf.size());
    }

    Result<UdpSocket::RecvResult> UdpSocket::recv_from(size_t max_size)
    {
        if (!is_valid())
            return Result<RecvResult>::fail(NetError::INTERNAL_ERROR);

        std::vector<uint8_t> tmp(max_size);
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);

        auto received = recvfrom(sock_, (char *)tmp.data(), (int)max_size, 0,
                                 (struct sockaddr *)&addr, &addr_len);
        if (received < 0)
            return Result<RecvResult>::fail(system_error_to_net());

        RecvResult rr;
        rr.data = Buffer(tmp.data(), static_cast<size_t>(received));
        rr.sender = sockaddr_to_endpoint(addr);
        return Result<RecvResult>::success(std::move(rr));
    }

    Result<void> UdpSocket::set_recv_timeout(uint32_t ms)
    {
        if (!is_valid())
            return Result<void>::fail(NetError::INTERNAL_ERROR);
#ifdef _WIN32
        DWORD timeout = ms;
        if (setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO,
                       (const char *)&timeout, sizeof(timeout)) != 0)
            return Result<void>::fail(system_error_to_net());
#else
        struct timeval tv;
        tv.tv_sec = ms / 1000;
        tv.tv_usec = (ms % 1000) * 1000;
        if (setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO,
                       &tv, sizeof(tv)) != 0)
            return Result<void>::fail(system_error_to_net());
#endif
        return Result<void>::success();
    }

    Result<void> UdpSocket::set_send_timeout(uint32_t ms)
    {
        if (!is_valid())
            return Result<void>::fail(NetError::INTERNAL_ERROR);
#ifdef _WIN32
        DWORD timeout = ms;
        if (setsockopt(sock_, SOL_SOCKET, SO_SNDTIMEO,
                       (const char *)&timeout, sizeof(timeout)) != 0)
            return Result<void>::fail(system_error_to_net());
#else
        struct timeval tv;
        tv.tv_sec = ms / 1000;
        tv.tv_usec = (ms % 1000) * 1000;
        if (setsockopt(sock_, SOL_SOCKET, SO_SNDTIMEO,
                       &tv, sizeof(tv)) != 0)
            return Result<void>::fail(system_error_to_net());
#endif
        return Result<void>::success();
    }

    Result<void> UdpSocket::set_reuse_addr(bool enable)
    {
        if (!is_valid())
            return Result<void>::fail(NetError::INTERNAL_ERROR);
        int val = enable ? 1 : 0;
        if (setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR,
                       (const char *)&val, sizeof(val)) != 0)
            return Result<void>::fail(system_error_to_net());
        return Result<void>::success();
    }

    Result<void> UdpSocket::join_multicast(const std::string &group)
    {
        if (!is_valid())
            return Result<void>::fail(NetError::INTERNAL_ERROR);
        struct ip_mreq mreq;
        std::memset(&mreq, 0, sizeof(mreq));
        inet_pton(AF_INET, group.c_str(), &mreq.imr_multiaddr);
        mreq.imr_interface.s_addr = INADDR_ANY;
        if (setsockopt(sock_, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                       (const char *)&mreq, sizeof(mreq)) != 0)
            return Result<void>::fail(system_error_to_net());
        return Result<void>::success();
    }

    Result<void> UdpSocket::leave_multicast(const std::string &group)
    {
        if (!is_valid())
            return Result<void>::fail(NetError::INTERNAL_ERROR);
        struct ip_mreq mreq;
        std::memset(&mreq, 0, sizeof(mreq));
        inet_pton(AF_INET, group.c_str(), &mreq.imr_multiaddr);
        mreq.imr_interface.s_addr = INADDR_ANY;
        if (setsockopt(sock_, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                       (const char *)&mreq, sizeof(mreq)) != 0)
            return Result<void>::fail(system_error_to_net());
        return Result<void>::success();
    }

    void UdpSocket::close()
    {
        close_socket(sock_);
        bound_ = false;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) تنفيذ TcpSocket
    // ═════════════════════════════════════════════════════════════════════

    TcpSocket::TcpSocket()
    {
        platform_socket_init();
        sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }

    TcpSocket::~TcpSocket()
    {
        close();
    }

    TcpSocket::TcpSocket(socket_t raw) : sock_(raw), connected_(true) {}

    TcpSocket::TcpSocket(TcpSocket &&other) noexcept
        : sock_(other.sock_), connected_(other.connected_)
    {
        other.sock_ = SADNET_INVALID_SOCKET;
        other.connected_ = false;
    }

    TcpSocket &TcpSocket::operator=(TcpSocket &&other) noexcept
    {
        if (this != &other)
        {
            close();
            sock_ = other.sock_;
            connected_ = other.connected_;
            other.sock_ = SADNET_INVALID_SOCKET;
            other.connected_ = false;
        }
        return *this;
    }

    Result<void> TcpSocket::connect(const Endpoint &remote)
    {
        if (!is_valid())
            return Result<void>::fail(NetError::INTERNAL_ERROR);
        struct sockaddr_in addr;
        if (!endpoint_to_sockaddr(remote, addr))
            return Result<void>::fail(NetError::INVALID_ARGUMENT);
        if (::connect(sock_, (struct sockaddr *)&addr, sizeof(addr)) != 0)
            return Result<void>::fail(system_error_to_net());
        connected_ = true;
        return Result<void>::success();
    }

    Result<size_t> TcpSocket::send(const uint8_t *data, size_t len)
    {
        if (!is_valid() || !connected_)
            return Result<size_t>::fail(NetError::INTERNAL_ERROR);
        auto sent = ::send(sock_, (const char *)data, (int)len, 0);
        if (sent < 0)
            return Result<size_t>::fail(system_error_to_net());
        return Result<size_t>::success(static_cast<size_t>(sent));
    }

    Result<size_t> TcpSocket::send(const Buffer &buf)
    {
        return send(buf.data(), buf.size());
    }

    Result<Buffer> TcpSocket::recv(size_t max_size)
    {
        if (!is_valid())
            return Result<Buffer>::fail(NetError::INTERNAL_ERROR);
        std::vector<uint8_t> tmp(max_size);
        auto received = ::recv(sock_, (char *)tmp.data(), (int)max_size, 0);
        if (received < 0)
            return Result<Buffer>::fail(system_error_to_net());
        if (received == 0)
        {
            connected_ = false;
            return Result<Buffer>::fail(NetError::CONNECTION_RESET);
        }
        return Result<Buffer>::success(Buffer(tmp.data(), static_cast<size_t>(received)));
    }

    Result<void> TcpSocket::bind_listen(const Endpoint &local, int backlog)
    {
        if (!is_valid())
            return Result<void>::fail(NetError::INTERNAL_ERROR);

        int val = 1;
        setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, (const char *)&val, sizeof(val));

        struct sockaddr_in addr;
        if (!endpoint_to_sockaddr(local, addr))
            return Result<void>::fail(NetError::INVALID_ARGUMENT);

        if (::bind(sock_, (struct sockaddr *)&addr, sizeof(addr)) != 0)
            return Result<void>::fail(system_error_to_net());
        if (::listen(sock_, backlog) != 0)
            return Result<void>::fail(system_error_to_net());

        return Result<void>::success();
    }

    Result<TcpSocket> TcpSocket::accept()
    {
        if (!is_valid())
            return Result<TcpSocket>::fail(NetError::INTERNAL_ERROR);
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        socket_t client = ::accept(sock_, (struct sockaddr *)&addr, &len);
        if (client == SADNET_INVALID_SOCKET)
            return Result<TcpSocket>::fail(system_error_to_net());
        return Result<TcpSocket>::success(TcpSocket(client));
    }

    Result<void> TcpSocket::set_recv_timeout(uint32_t ms)
    {
        if (!is_valid())
            return Result<void>::fail(NetError::INTERNAL_ERROR);
#ifdef _WIN32
        DWORD timeout = ms;
        if (setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO,
                       (const char *)&timeout, sizeof(timeout)) != 0)
            return Result<void>::fail(system_error_to_net());
#else
        struct timeval tv;
        tv.tv_sec = ms / 1000;
        tv.tv_usec = (ms % 1000) * 1000;
        if (setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) != 0)
            return Result<void>::fail(system_error_to_net());
#endif
        return Result<void>::success();
    }

    Result<void> TcpSocket::set_send_timeout(uint32_t ms)
    {
        if (!is_valid())
            return Result<void>::fail(NetError::INTERNAL_ERROR);
#ifdef _WIN32
        DWORD timeout = ms;
        if (setsockopt(sock_, SOL_SOCKET, SO_SNDTIMEO,
                       (const char *)&timeout, sizeof(timeout)) != 0)
            return Result<void>::fail(system_error_to_net());
#else
        struct timeval tv;
        tv.tv_sec = ms / 1000;
        tv.tv_usec = (ms % 1000) * 1000;
        if (setsockopt(sock_, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) != 0)
            return Result<void>::fail(system_error_to_net());
#endif
        return Result<void>::success();
    }

    void TcpSocket::close()
    {
        close_socket(sock_);
        connected_ = false;
    }

} // namespace sad::net
