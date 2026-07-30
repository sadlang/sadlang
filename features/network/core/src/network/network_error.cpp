// ==========================================================================
// network_error.cpp
// Network Error Implementation / تطبيق معالجة أخطاء الشبكات
// ==========================================================================

#include "network_error.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <errno.h>
    #include <cstring>
#endif

namespace sad {
namespace network {

// ==========================================================================
// NetworkException Implementation
// ==========================================================================

NetworkException::NetworkException(NetworkErrorCode error_code,
                                 const std::string& arabic_msg,
                                 const std::string& english_msg)
    : m_error_code(error_code)
    , m_arabic_message(arabic_msg)
    , m_english_message(english_msg)
    , m_system_error_code(0)
{
}

NetworkException::NetworkException(NetworkErrorCode error_code,
                                 const std::string& message)
    : m_error_code(error_code)
    , m_arabic_message(message)
    , m_english_message(message)
    , m_system_error_code(0)
{
}

const char* NetworkException::what() const noexcept {
    return m_english_message.c_str();
}

std::string NetworkException::what_arabic() const noexcept {
    return m_arabic_message;
}

std::string NetworkException::what_english() const noexcept {
    return m_english_message;
}

NetworkErrorCode NetworkException::error_code() const noexcept {
    return m_error_code;
}

int NetworkException::system_error_code() const noexcept {
    return m_system_error_code;
}

void NetworkException::set_system_error_code(int code) {
    m_system_error_code = code;
}

// ==========================================================================
// Helper Functions Implementation
// ==========================================================================

int get_last_network_error() {
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

std::string error_code_to_arabic(NetworkErrorCode code) {
    switch (code) {
        case NetworkErrorCode::GENERAL_FAILURE:
            return "فشل عام في عملية الشبكة";
        case NetworkErrorCode::SOCKET_CREATION_FAILED:
            return "فشل إنشاء المقبس";
        case NetworkErrorCode::SOCKET_BIND_FAILED:
            return "فشل ربط المقبس بالعنوان";
        case NetworkErrorCode::SOCKET_LISTEN_FAILED:
            return "فشل بدء الاستماع على المقبس";
        case NetworkErrorCode::SOCKET_ACCEPT_FAILED:
            return "فشل قبول اتصال وارد";
        case NetworkErrorCode::SOCKET_CONNECT_FAILED:
            return "فشل الاتصال بالخادم";
        case NetworkErrorCode::SOCKET_ALREADY_CONNECTED:
            return "المقبس متصل بالفعل";
        case NetworkErrorCode::SOCKET_NOT_CONNECTED:
            return "المقبس غير متصل";
        case NetworkErrorCode::SOCKET_CLOSED:
            return "المقبس مغلق";
        case NetworkErrorCode::SEND_FAILED:
            return "فشل إرسال البيانات";
        case NetworkErrorCode::RECEIVE_FAILED:
            return "فشل استقبال البيانات";
        case NetworkErrorCode::PARTIAL_SEND:
            return "تم إرسال جزء من البيانات فقط";
        case NetworkErrorCode::PARTIAL_RECEIVE:
            return "تم استقبال جزء من البيانات فقط";
        case NetworkErrorCode::CONNECTION_RESET:
            return "تم إعادة تعيين الاتصال من قبل الطرف الآخر";
        case NetworkErrorCode::CONNECTION_ABORTED:
            return "تم إلغاء الاتصال";
        case NetworkErrorCode::INVALID_ADDRESS:
            return "عنوان IP غير صالح";
        case NetworkErrorCode::ADDRESS_IN_USE:
            return "العنوان أو المنفذ قيد الاستخدام بالفعل";
        case NetworkErrorCode::ADDRESS_NOT_AVAILABLE:
            return "العنوان غير متاح";
        case NetworkErrorCode::SET_OPTION_FAILED:
            return "فشل تعيين خيار المقبس";
        case NetworkErrorCode::GET_OPTION_FAILED:
            return "فشل قراءة خيار المقبس";
        case NetworkErrorCode::TIMEOUT:
            return "انتهت المهلة الزمنية للعملية";
        case NetworkErrorCode::WOULD_BLOCK:
            return "العملية ستحجب - لا توجد بيانات متاحة حالياً";
        case NetworkErrorCode::TLS_NOT_AVAILABLE:
            return "الطبقة الآمنة TLS غير مبنية في هذا الإصدار — "
                   "رُفض الطلب قبل إرسال أي بايت. لا تستخدم https/wss حتى تتوفر TLS";
        case NetworkErrorCode::BROADCAST_NOT_SUPPORTED:
            return "البث العام غير مدعوم أو غير مفعّل";
        case NetworkErrorCode::MULTICAST_JOIN_FAILED:
            return "فشل الانضمام إلى مجموعة البث الجماعي";
        case NetworkErrorCode::MULTICAST_LEAVE_FAILED:
            return "فشل المغادرة من مجموعة البث الجماعي";
        default:
            return "خطأ شبكة غير معروف";
    }
}

std::string error_code_to_english(NetworkErrorCode code) {
    switch (code) {
        case NetworkErrorCode::GENERAL_FAILURE:
            return "General network operation failure";
        case NetworkErrorCode::SOCKET_CREATION_FAILED:
            return "Failed to create socket";
        case NetworkErrorCode::SOCKET_BIND_FAILED:
            return "Failed to bind socket to address";
        case NetworkErrorCode::SOCKET_LISTEN_FAILED:
            return "Failed to start listening on socket";
        case NetworkErrorCode::SOCKET_ACCEPT_FAILED:
            return "Failed to accept incoming connection";
        case NetworkErrorCode::SOCKET_CONNECT_FAILED:
            return "Failed to connect to server";
        case NetworkErrorCode::SOCKET_ALREADY_CONNECTED:
            return "Socket is already connected";
        case NetworkErrorCode::SOCKET_NOT_CONNECTED:
            return "Socket is not connected";
        case NetworkErrorCode::SOCKET_CLOSED:
            return "Socket is closed";
        case NetworkErrorCode::SEND_FAILED:
            return "Failed to send data";
        case NetworkErrorCode::RECEIVE_FAILED:
            return "Failed to receive data";
        case NetworkErrorCode::PARTIAL_SEND:
            return "Only part of the data was sent";
        case NetworkErrorCode::PARTIAL_RECEIVE:
            return "Only part of the data was received";
        case NetworkErrorCode::CONNECTION_RESET:
            return "Connection reset by peer";
        case NetworkErrorCode::CONNECTION_ABORTED:
            return "Connection aborted";
        case NetworkErrorCode::INVALID_ADDRESS:
            return "Invalid IP address";
        case NetworkErrorCode::ADDRESS_IN_USE:
            return "Address or port is already in use";
        case NetworkErrorCode::ADDRESS_NOT_AVAILABLE:
            return "Address is not available";
        case NetworkErrorCode::SET_OPTION_FAILED:
            return "Failed to set socket option";
        case NetworkErrorCode::GET_OPTION_FAILED:
            return "Failed to get socket option";
        case NetworkErrorCode::TIMEOUT:
            return "Operation timed out";
        case NetworkErrorCode::WOULD_BLOCK:
            return "Operation would block - no data available now";
        case NetworkErrorCode::TLS_NOT_AVAILABLE:
            return "TLS layer is not built in this release — request refused before "
                   "sending any byte. Do not use https/wss until TLS is available";
        case NetworkErrorCode::BROADCAST_NOT_SUPPORTED:
            return "Broadcast is not supported or not enabled";
        case NetworkErrorCode::MULTICAST_JOIN_FAILED:
            return "Failed to join multicast group";
        case NetworkErrorCode::MULTICAST_LEAVE_FAILED:
            return "Failed to leave multicast group";
        default:
            return "Unknown network error";
    }
}

NetworkErrorCode system_error_to_network_error(int system_code) {
#ifdef _WIN32
    switch (system_code) {
        case WSAECONNREFUSED:
            return NetworkErrorCode::SOCKET_CONNECT_FAILED;
        case WSAEADDRINUSE:
            return NetworkErrorCode::ADDRESS_IN_USE;
        case WSAEADDRNOTAVAIL:
            return NetworkErrorCode::ADDRESS_NOT_AVAILABLE;
        case WSAECONNRESET:
            return NetworkErrorCode::CONNECTION_RESET;
        case WSAECONNABORTED:
            return NetworkErrorCode::CONNECTION_ABORTED;
        case WSAETIMEDOUT:
            return NetworkErrorCode::TIMEOUT;
        case WSAEWOULDBLOCK:
            return NetworkErrorCode::WOULD_BLOCK;
        case WSAEISCONN:
            return NetworkErrorCode::SOCKET_ALREADY_CONNECTED;
        case WSAENOTCONN:
            return NetworkErrorCode::SOCKET_NOT_CONNECTED;
        default:
            return NetworkErrorCode::GENERAL_FAILURE;
    }
#else
    switch (system_code) {
        case ECONNREFUSED:
            return NetworkErrorCode::SOCKET_CONNECT_FAILED;
        case EADDRINUSE:
            return NetworkErrorCode::ADDRESS_IN_USE;
        case EADDRNOTAVAIL:
            return NetworkErrorCode::ADDRESS_NOT_AVAILABLE;
        case ECONNRESET:
            return NetworkErrorCode::CONNECTION_RESET;
        case ECONNABORTED:
            return NetworkErrorCode::CONNECTION_ABORTED;
        case ETIMEDOUT:
            return NetworkErrorCode::TIMEOUT;
        case EWOULDBLOCK:
#if EAGAIN != EWOULDBLOCK
        case EAGAIN:
#endif
            return NetworkErrorCode::WOULD_BLOCK;
        case EISCONN:
            return NetworkErrorCode::SOCKET_ALREADY_CONNECTED;
        case ENOTCONN:
            return NetworkErrorCode::SOCKET_NOT_CONNECTED;
        default:
            return NetworkErrorCode::GENERAL_FAILURE;
    }
#endif
}

NetworkException create_network_exception_from_system_error(
    NetworkErrorCode fallback_code
) {
    int sys_error = get_last_network_error();
    NetworkErrorCode code = system_error_to_network_error(sys_error);
    
    // If we couldn't map it, use the fallback
    if (code == NetworkErrorCode::GENERAL_FAILURE && 
        fallback_code != NetworkErrorCode::GENERAL_FAILURE) {
        code = fallback_code;
    }
    
    std::string arabic_msg = error_code_to_arabic(code);
    std::string english_msg = error_code_to_english(code);
    
    // Add system error code to message
#ifdef _WIN32
    arabic_msg += " (رمز الخطأ: " + std::to_string(sys_error) + ")";
    english_msg += " (Error code: " + std::to_string(sys_error) + ")";
#else
    arabic_msg += " (" + std::string(strerror(sys_error)) + ")";
    english_msg += " (" + std::string(strerror(sys_error)) + ")";
#endif
    
    NetworkException ex(code, arabic_msg, english_msg);
    ex.set_system_error_code(sys_error);
    return ex;
}

} // namespace network
} // namespace sad
