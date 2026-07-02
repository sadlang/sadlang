// ===================================================================
// صNet — طبقة النقل: مقابس UDP/TCP متعددة المنصات
// (AR) يُغلّف واجهات نظام التشغيل (WinSock2 / POSIX) بواجهة موحدة.
//      جميع عمليات الشبكة الفعلية في صNet تمر عبر هذه الطبقة.
// (EN) Cross-platform socket abstraction over WinSock2 / POSIX.
// المكون: features/network/sadnet (طبقة النقل)
// المسار: features/network/sadnet/include/sadnet/core/transport.h
// ===================================================================
#ifndef SAD_SADNET_CORE_TRANSPORT_H
#define SAD_SADNET_CORE_TRANSPORT_H

#include "sadnet/core/types.h"
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
using socket_t = SOCKET;
#define SADNET_INVALID_SOCKET INVALID_SOCKET
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using socket_t = int;
#define SADNET_INVALID_SOCKET (-1)
#endif

namespace sad::net
{

    // ─────────────────────────────────────────────────────────────────────
    // (AR) تهيئة/تنظيف Winsock — يُستدعى تلقائياً
    // ─────────────────────────────────────────────────────────────────────

    /// تهيئة مكتبة المقابس (WinSock2 على ويندوز) — thread-safe
    void platform_socket_init();

    /// تنظيف مكتبة المقابس
    void platform_socket_cleanup();

    // ─────────────────────────────────────────────────────────────────────
    // (AR) مقبس UDP — لبروتوكول صNet الأساسي
    // ─────────────────────────────────────────────────────────────────────

    class UdpSocket
    {
        socket_t sock_ = SADNET_INVALID_SOCKET;
        bool bound_ = false;

    public:
        UdpSocket();
        ~UdpSocket();

        // الأصناف غير قابلة للنسخ، قابلة للنقل
        UdpSocket(const UdpSocket &) = delete;
        UdpSocket &operator=(const UdpSocket &) = delete;
        UdpSocket(UdpSocket &&other) noexcept;
        UdpSocket &operator=(UdpSocket &&other) noexcept;

        /// ربط المقبس بعنوان ومنفذ محلي
        Result<void> bind(const Endpoint &local);

        /// ربط على أي عنوان + منفذ محدد
        Result<void> bind(uint16_t port);

        /// إرسال بيانات إلى عنوان بعيد
        Result<size_t> send_to(const Endpoint &dest, const uint8_t *data, size_t len);
        Result<size_t> send_to(const Endpoint &dest, const Buffer &buf);

        /// استقبال بيانات مع عنوان المُرسل
        struct RecvResult
        {
            Buffer data;
            Endpoint sender;
        };
        Result<RecvResult> recv_from(size_t max_size = MAX_UDP_PACKET);

        /// تعيين مهلة الاستقبال (مللي ثانية، 0 = بلا مهلة)
        Result<void> set_recv_timeout(uint32_t ms);

        /// تعيين مهلة الإرسال
        Result<void> set_send_timeout(uint32_t ms);

        /// تفعيل/تعطيل إعادة استخدام العنوان
        Result<void> set_reuse_addr(bool enable);

        /// تفعيل البث المتعدد (multicast)
        Result<void> join_multicast(const std::string &group);
        Result<void> leave_multicast(const std::string &group);

        /// إغلاق المقبس
        void close();

        /// هل المقبس صالح؟
        bool is_valid() const { return sock_ != SADNET_INVALID_SOCKET; }

        /// الحصول على مقبض المقبس الأصلي (لـ select/poll)
        socket_t native_handle() const { return sock_; }
    };

    // ─────────────────────────────────────────────────────────────────────
    // (AR) مقبس TCP — للاتصالات المباشرة (TURN relay، HTTP fallback)
    // ─────────────────────────────────────────────────────────────────────

    class TcpSocket
    {
        socket_t sock_ = SADNET_INVALID_SOCKET;
        bool connected_ = false;

    public:
        TcpSocket();
        ~TcpSocket();

        TcpSocket(const TcpSocket &) = delete;
        TcpSocket &operator=(const TcpSocket &) = delete;
        TcpSocket(TcpSocket &&other) noexcept;
        TcpSocket &operator=(TcpSocket &&other) noexcept;

        /// إنشاء من مقبض أصلي (من accept)
        explicit TcpSocket(socket_t raw);

        /// الاتصال بعنوان بعيد
        Result<void> connect(const Endpoint &remote);

        /// إرسال بيانات
        Result<size_t> send(const uint8_t *data, size_t len);
        Result<size_t> send(const Buffer &buf);

        /// استقبال بيانات
        Result<Buffer> recv(size_t max_size = 8192);

        /// ربط + استماع (للخوادم)
        Result<void> bind_listen(const Endpoint &local, int backlog = 128);

        /// قبول اتصال وارد
        Result<TcpSocket> accept();

        /// تعيين المهلات
        Result<void> set_recv_timeout(uint32_t ms);
        Result<void> set_send_timeout(uint32_t ms);

        /// إغلاق
        void close();

        bool is_valid() const { return sock_ != SADNET_INVALID_SOCKET; }
        bool is_connected() const { return connected_; }
        socket_t native_handle() const { return sock_; }
    };

} // namespace sad::net

#endif // SAD_SADNET_CORE_TRANSPORT_H
