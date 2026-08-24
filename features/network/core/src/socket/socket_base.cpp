// ==========================================================================
// socket_base.cpp
// Base Socket Implementation / تطبيق الفئة الأساسية للمقابس
// ==========================================================================

#include "socket_base.h"
#include "network_error.h"

#include <mutex>

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

#ifdef _WIN32
// (AR) قفل عداد Winsock: الزيادات تقع في خيط القبول (منشئ النقل) والإنقاصات
//      في خيوط العمال (هدامات handle_client) — عد غير محمي = سباق بيانات
//      يضيع زيادة فيعبر العداد الصفر مبكرا ويطلق WSACleanup وسط العمل.
// (EN) Winsock counter lock: increments happen on the accept thread (move
//      ctor) and decrements on worker threads (handle_client destructors) —
//      unguarded ++/-- is a data race that can lose an increment, cross zero
//      early, and fire WSACleanup mid-flight.
static std::mutex s_winsock_mutex;
#endif

SocketBase::SocketBase()
    : m_socket(INVALID_SOCKET_VALUE)
{
    initialize_winsock();
}

SocketBase::~SocketBase() {
    close();
    cleanup_winsock();
}

// (AR) 🔑 ع-14: الناقل يزيد عداد Winsock المرجعي — القاعدة «كل كائن حي = مرجع
//      واحد» لا «كل مقبس = مرجع»: الهدام ينقص العداد لكل كائن (حتى المنقول
//      منه)، فكان كل اتصال مقبول يمر بثلاثة هدامات (نسخة حلقة القبول
//      المنقولة + مخزن الخيط + معامل handle_client) مقابل زيادة واحدة —
//      صافي −2 فيبلغ العداد صفرا بعد أول طلب ويستدعي WSACleanup الذي يهدم
//      كل مقابس العملية ومنها المستمع (مقيس على التنفيذي المصرف؛ المفسر
//      كان ناجيا مصادفة لأن WSAStartup ينادى فيه من مكون آخر فيرتفع عداد
//      النظام فوق الواحد).
// (EN) 🔑 ع-14: the move constructor must take a Winsock reference — the
//      invariant is "one live object = one ref", not "one socket = one ref",
//      because the destructor decrements for EVERY object, moved-from ones
//      included. Each accepted connection ran three destructors (the moved
//      accept-loop copy + the thread's stored copy + the handle_client
//      parameter) against a single increment — net −2, so the counter hit
//      zero after the first request and WSACleanup() tore down every socket
//      in the process, listener included (measured on compiled executables;
//      the interpreter merely survived because another component there calls
//      WSAStartup, keeping the OS-level count above one).
// (AR) أمان noexcept: فرع الرمي في initialize_winsock هو فشل WSAStartup، وهو
//      غير قابل للبلوغ هنا — وجود مصدر حي للنقل يضمن أن Winsock مهيأ مسبقا
//      فلا يُنادى WSAStartup بل يزاد العداد فقط.
// (EN) noexcept safety: initialize_winsock's only throwing branch is a failed
//      WSAStartup, unreachable here — a live move source guarantees Winsock is
//      already initialized, so only the counter increment runs.
SocketBase::SocketBase(SocketBase&& other) noexcept
    : m_socket(other.m_socket)
{
    initialize_winsock();
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
#ifdef _WIN32
    // (AR) 🔑 دلالة Winsock تخالف POSIX: SO_REUSEADDR على ويندوز يسمح لمقبس
    //      ثان بالربط على منفذ **قيد الاستماع النشط** — اختطاف منفذ محلي
    //      (رصدته المراجعة الأمنية). البديل SO_EXCLUSIVEADDRUSE: يمنع
    //      الاختطاف ولا يمنع إعادة الربط بعد إغلاق سليم للمستمع — وهي غاية
    //      «إعادة الاستخدام» المقصودة هنا (أزواج TIME_WAIT).
    // (EN) 🔑 Winsock semantics differ from POSIX: SO_REUSEADDR on Windows
    //      lets a second socket bind a port that is ACTIVELY LISTENING —
    //      local port hijacking. SO_EXCLUSIVEADDRUSE blocks hijacking while
    //      still allowing rebind after a clean listener close (the TIME_WAIT
    //      reuse this API is for).
    if (setsockopt(m_socket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                   reinterpret_cast<const char*>(&opt), sizeof(opt)) != 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SET_OPTION_FAILED);
    }
#else
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR,
                   reinterpret_cast<const char*>(&opt), sizeof(opt)) != 0) {
        throw create_network_exception_from_system_error(
            NetworkErrorCode::SET_OPTION_FAILED);
    }
#endif
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
    std::lock_guard<std::mutex> lock(s_winsock_mutex);
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
    std::lock_guard<std::mutex> lock(s_winsock_mutex);
    // (AR) حارس القاع: عداد سالب يعني اختلال توازن — لا ننقص تحت الصفر كي لا
    //      يعود فيمر بالصفر مرة أخرى مستدعيا WSACleanup في منتصف عمل المقابس.
    // (EN) Floor guard: a negative count means an imbalance — never go below
    //      zero, or a later decrement could cross zero again and fire
    //      WSACleanup mid-flight.
    if (s_winsock_ref_count > 0) {
        --s_winsock_ref_count;
    }
    if (s_winsock_ref_count == 0 && s_winsock_initialized) {
        WSACleanup();
        s_winsock_initialized = false;
    }
#endif
}

} // namespace network
} // namespace sad
