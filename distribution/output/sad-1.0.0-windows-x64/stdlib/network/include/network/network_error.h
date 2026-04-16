// ==========================================================================
// network_error.h
// Network Error Handling for Sad Language / معالجة أخطاء الشبكات للغة ص
// ==========================================================================
// Description:
//   Exception classes for network operations with Arabic and English messages
//   فئات الاستثناءات لعمليات الشبكات مع رسائل عربية وإنجليزية
//
// Phase: 2.1.1 - TCP/UDP Sockets
// Author: Sad Language Team
// ==========================================================================

#pragma once

#include <exception>
#include <string>
#include <system_error>

namespace sad {
namespace network {

// ==========================================================================
// Network Error Types / أنواع أخطاء الشبكات
// ==========================================================================

enum class NetworkErrorCode {
    // فشل عام / General Failure
    GENERAL_FAILURE,
    
    // أخطاء المقبس / Socket Errors
    SOCKET_CREATION_FAILED,     // فشل إنشاء المقبس
    SOCKET_BIND_FAILED,          // فشل ربط المقبس
    SOCKET_LISTEN_FAILED,        // فشل الاستماع
    SOCKET_ACCEPT_FAILED,        // فشل قبول اتصال
    SOCKET_CONNECT_FAILED,       // فشل الاتصال
    SOCKET_ALREADY_CONNECTED,    // المقبس متصل بالفعل
    SOCKET_NOT_CONNECTED,        // المقبس غير متصل
    SOCKET_CLOSED,               // المقبس مغلق
    
    // أخطاء الإرسال/الاستقبال / Send/Receive Errors
    SEND_FAILED,                 // فشل الإرسال
    RECEIVE_FAILED,              // فشل الاستقبال
    PARTIAL_SEND,                // إرسال جزئي
    PARTIAL_RECEIVE,             // استقبال جزئي
    CONNECTION_RESET,            // إعادة تعيين الاتصال
    CONNECTION_ABORTED,          // إلغاء الاتصال
    
    // أخطاء العناوين / Address Errors
    INVALID_ADDRESS,             // عنوان غير صالح
    ADDRESS_IN_USE,              // العنوان قيد الاستخدام
    ADDRESS_NOT_AVAILABLE,       // العنوان غير متاح
    
    // أخطاء الخيارات / Option Errors
    SET_OPTION_FAILED,           // فشل تعيين الخيار
    GET_OPTION_FAILED,           // فشل قراءة الخيار
    
    // أخطاء المهلة الزمنية / Timeout Errors
    TIMEOUT,                     // انتهت المهلة الزمنية
    WOULD_BLOCK,                 // العملية ستحجب
    
    // أخطاء Broadcast/Multicast
    BROADCAST_NOT_SUPPORTED,     // البث العام غير مدعوم
    MULTICAST_JOIN_FAILED,       // فشل الانضمام للبث الجماعي
    MULTICAST_LEAVE_FAILED,      // فشل المغادرة من البث الجماعي
};

// ==========================================================================
// NetworkException - Base Exception Class / فئة الاستثناء الأساسية
// ==========================================================================

class NetworkException : public std::exception {
public:
    // Constructor with Arabic and English messages
    NetworkException(NetworkErrorCode error_code,
                    const std::string& arabic_msg,
                    const std::string& english_msg);
    
    // Constructor with error code and custom message
    NetworkException(NetworkErrorCode error_code,
                    const std::string& message);
    
    // Get error message (English by default)
    const char* what() const noexcept override;
    
    // Get Arabic error message
    std::string what_arabic() const noexcept;
    
    // Get English error message
    std::string what_english() const noexcept;
    
    // Get error code
    NetworkErrorCode error_code() const noexcept;
    
    // Get system error code (if applicable)
    int system_error_code() const noexcept;
    
    // Set system error code
    void set_system_error_code(int code);

private:
    NetworkErrorCode m_error_code;
    std::string m_arabic_message;
    std::string m_english_message;
    int m_system_error_code;
};

// ==========================================================================
// Helper Functions / دوال مساعدة
// ==========================================================================

// Get last system network error
int get_last_network_error();

// Convert error code to Arabic message
std::string error_code_to_arabic(NetworkErrorCode code);

// Convert error code to English message
std::string error_code_to_english(NetworkErrorCode code);

// Convert system error to NetworkErrorCode
NetworkErrorCode system_error_to_network_error(int system_code);

// Create NetworkException from current system error
NetworkException create_network_exception_from_system_error(
    NetworkErrorCode fallback_code = NetworkErrorCode::GENERAL_FAILURE
);

} // namespace network
} // namespace sad
