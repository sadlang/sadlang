// بسم الله الرحمن الرحيم
// websocket_base.h - WebSocket Base Types and Constants
// أنواع وثوابت WebSocket الأساسية

#ifndef SAD_WEBSOCKET_BASE_H
#define SAD_WEBSOCKET_BASE_H

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

namespace sad {
namespace network {
namespace websocket {

// ============================================================================
// Constants - الثوابت
// ============================================================================

// WebSocket version / إصدار WebSocket
constexpr int WEBSOCKET_VERSION = 13;

// Default ports / المنافذ الافتراضية
constexpr uint16_t WS_DEFAULT_PORT = 80;
constexpr uint16_t WSS_DEFAULT_PORT = 443;

// WebSocket magic GUID (RFC 6455)
constexpr const char* WEBSOCKET_GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

// Maximum frame payload size / أقصى حجم للإطار
constexpr size_t MAX_FRAME_PAYLOAD = 16 * 1024 * 1024;  // 16 MB
constexpr size_t MAX_HEADER_SIZE = 14;  // Maximum frame header size

// ============================================================================
// Enums - التعدادات
// ============================================================================

/**
 * @brief WebSocket opcode types / أنواع رموز WebSocket
 */
enum class WebSocketOpcode : uint8_t {
    Continuation = 0x0,  // إكمال / Continuation frame
    Text = 0x1,          // نص / Text frame
    Binary = 0x2,        // ثنائي / Binary frame
    // Reserved: 0x3 - 0x7
    Close = 0x8,         // إغلاق / Connection close
    Ping = 0x9,          // نبض / Ping
    Pong = 0xA,          // استجابة / Pong
    // Reserved: 0xB - 0xF
};

/**
 * @brief WebSocket message type / نوع رسالة WebSocket
 */
enum class MessageType {
    Text,     // نص
    Binary,   // ثنائي
    Ping,     // نبض
    Pong,     // استجابة
    Close     // إغلاق
};

/**
 * @brief WebSocket connection state / حالة اتصال WebSocket
 */
enum class ConnectionState {
    Connecting,   // جاري الاتصال
    Open,         // مفتوح
    Closing,      // جاري الإغلاق
    Closed        // مغلق
};

/**
 * @brief WebSocket close codes (RFC 6455) / رموز الإغلاق
 */
enum class CloseCode : uint16_t {
    Normal = 1000,           // إغلاق عادي
    GoingAway = 1001,        // مغادرة
    ProtocolError = 1002,    // خطأ بروتوكول
    UnsupportedData = 1003,  // بيانات غير مدعومة
    NoStatusReceived = 1005, // لا حالة
    AbnormalClose = 1006,    // إغلاق غير طبيعي
    InvalidPayload = 1007,   // بيانات غير صالحة
    PolicyViolation = 1008,  // انتهاك سياسة
    MessageTooBig = 1009,    // رسالة كبيرة جداً
    MissingExtension = 1010, // امتداد ناقص
    InternalError = 1011,    // خطأ داخلي
    TlsHandshake = 1015      // فشل TLS
};

// ============================================================================
// Structures - الهياكل
// ============================================================================

/**
 * @brief WebSocket frame structure / هيكل إطار WebSocket
 */
struct WebSocketFrame {
    bool fin = true;              // نهاية الرسالة
    bool rsv1 = false;            // محجوز 1
    bool rsv2 = false;            // محجوز 2
    bool rsv3 = false;            // محجوز 3
    WebSocketOpcode opcode = WebSocketOpcode::Text;
    bool masked = false;          // مقنع
    uint64_t payload_length = 0;  // طول البيانات
    uint8_t masking_key[4] = {0}; // مفتاح القناع
    std::vector<uint8_t> payload; // البيانات
    
    // دوال مساعدة / Helper methods
    bool is_control() const {
        return static_cast<uint8_t>(opcode) >= 0x8;
    }
    
    bool is_data() const {
        return !is_control();
    }
};

/**
 * @brief WebSocket message / رسالة WebSocket
 */
struct WebSocketMessage {
    MessageType type = MessageType::Text;
    std::vector<uint8_t> data;
    std::string as_string() const {
        return std::string(data.begin(), data.end());
    }
};

/**
 * @brief WebSocket close info / معلومات الإغلاق
 */
struct CloseInfo {
    CloseCode code = CloseCode::Normal;
    std::string reason;
};

// ============================================================================
// Callbacks - دوال الاستجابة
// ============================================================================

using OnOpenCallback = std::function<void()>;
using OnCloseCallback = std::function<void(CloseCode, const std::string&)>;
using OnMessageCallback = std::function<void(const WebSocketMessage&)>;
using OnErrorCallback = std::function<void(const std::string&)>;
using OnPingCallback = std::function<void(const std::vector<uint8_t>&)>;
using OnPongCallback = std::function<void(const std::vector<uint8_t>&)>;

// ============================================================================
// Utility Functions - دوال مساعدة
// ============================================================================

/**
 * @brief Get opcode name / الحصول على اسم الرمز
 */
inline const char* opcode_to_string(WebSocketOpcode opcode) {
    switch (opcode) {
        case WebSocketOpcode::Continuation: return "CONTINUATION";
        case WebSocketOpcode::Text: return "TEXT";
        case WebSocketOpcode::Binary: return "BINARY";
        case WebSocketOpcode::Close: return "CLOSE";
        case WebSocketOpcode::Ping: return "PING";
        case WebSocketOpcode::Pong: return "PONG";
        default: return "UNKNOWN";
    }
}

/**
 * @brief Get state name / الحصول على اسم الحالة
 */
inline const char* state_to_string(ConnectionState state) {
    switch (state) {
        case ConnectionState::Connecting: return "CONNECTING";
        case ConnectionState::Open: return "OPEN";
        case ConnectionState::Closing: return "CLOSING";
        case ConnectionState::Closed: return "CLOSED";
        default: return "UNKNOWN";
    }
}

/**
 * @brief Get close code name / الحصول على اسم رمز الإغلاق
 */
inline const char* close_code_to_string(CloseCode code) {
    switch (code) {
        case CloseCode::Normal: return "عادي / Normal";
        case CloseCode::GoingAway: return "مغادرة / Going Away";
        case CloseCode::ProtocolError: return "خطأ بروتوكول / Protocol Error";
        case CloseCode::UnsupportedData: return "بيانات غير مدعومة / Unsupported Data";
        case CloseCode::NoStatusReceived: return "لا حالة / No Status";
        case CloseCode::AbnormalClose: return "إغلاق غير طبيعي / Abnormal Close";
        case CloseCode::InvalidPayload: return "بيانات غير صالحة / Invalid Payload";
        case CloseCode::PolicyViolation: return "انتهاك سياسة / Policy Violation";
        case CloseCode::MessageTooBig: return "رسالة كبيرة / Message Too Big";
        case CloseCode::MissingExtension: return "امتداد ناقص / Missing Extension";
        case CloseCode::InternalError: return "خطأ داخلي / Internal Error";
        case CloseCode::TlsHandshake: return "فشل TLS / TLS Handshake Failed";
        default: return "غير معروف / Unknown";
    }
}

} // namespace websocket
} // namespace network
} // namespace sad

#endif // SAD_WEBSOCKET_BASE_H
