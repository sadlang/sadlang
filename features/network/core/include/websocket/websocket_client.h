// بسم الله الرحمن الرحيم
// websocket_client.h - WebSocket Client
// عميل WebSocket

#ifndef SAD_WEBSOCKET_CLIENT_H
#define SAD_WEBSOCKET_CLIENT_H

#include "websocket_base.h"
#include "websocket_frame.h"

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

#include <string>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <queue>
#include <condition_variable>

namespace sad {
namespace network {
namespace websocket {

/**
 * @brief WebSocket Client / عميل WebSocket
 * 
 * عميل WebSocket يدعم الاتصال بخوادم WebSocket
 * WebSocket client supporting connection to WebSocket servers
 */
class WebSocketClient {
public:
    /**
     * @brief Constructor / المُنشئ
     */
    WebSocketClient();
    
    /**
     * @brief Destructor / المُدمر
     */
    ~WebSocketClient();
    
    // Disable copy / منع النسخ
    WebSocketClient(const WebSocketClient&) = delete;
    WebSocketClient& operator=(const WebSocketClient&) = delete;
    
    // Enable move / تمكين النقل
    WebSocketClient(WebSocketClient&&) noexcept;
    WebSocketClient& operator=(WebSocketClient&&) noexcept;
    
    // ============================================================================
    // Connection Methods - دوال الاتصال
    // ============================================================================
    
    /**
     * @brief Connect to WebSocket server / الاتصال بخادم WebSocket
     * @param url URL مثل ws://example.com/path أو wss://example.com/path
     * @return true إذا نجح الاتصال
     */
    bool connect(const std::string& url);
    
    /**
     * @brief Connect with custom headers / الاتصال مع رؤوس مخصصة
     */
    bool connect(const std::string& url, 
                 const std::vector<std::pair<std::string, std::string>>& headers);
    
    /**
     * @brief Close connection / إغلاق الاتصال
     */
    void close(CloseCode code = CloseCode::Normal, const std::string& reason = "");
    
    /**
     * @brief Check if connected / التحقق من الاتصال
     */
    bool is_connected() const;
    
    /**
     * @brief Get connection state / الحصول على حالة الاتصال
     */
    ConnectionState get_state() const;
    
    // ============================================================================
    // Sending Methods - دوال الإرسال
    // ============================================================================
    
    /**
     * @brief Send text message / إرسال رسالة نصية
     */
    bool send(const std::string& message);
    
    /**
     * @brief Send binary data / إرسال بيانات ثنائية
     */
    bool send_binary(const std::vector<uint8_t>& data);
    
    /**
     * @brief Send ping / إرسال نبض
     */
    bool ping(const std::vector<uint8_t>& data = {});
    
    // ============================================================================
    // Receiving Methods - دوال الاستقبال
    // ============================================================================
    
    /**
     * @brief Receive message (blocking) / استقبال رسالة (حظر)
     */
    WebSocketMessage receive();
    
    /**
     * @brief Receive with timeout / استقبال مع مهلة
     * @param timeout_ms المهلة بالميلي ثانية (-1 للانتظار اللانهائي)
     */
    WebSocketMessage receive(int timeout_ms);
    
    /**
     * @brief Check if message available / التحقق من وجود رسالة
     */
    bool has_message() const;
    
    /**
     * @brief Poll for messages / استطلاع الرسائل
     * معالجة الرسائل الواردة دون حظر
     */
    void poll();
    
    // ============================================================================
    // Callback Methods - دوال الاستجابة
    // ============================================================================
    
    /**
     * @brief Set open callback / تعيين دالة الفتح
     */
    void on_open(OnOpenCallback callback);
    
    /**
     * @brief Set close callback / تعيين دالة الإغلاق
     */
    void on_close(OnCloseCallback callback);
    
    /**
     * @brief Set message callback / تعيين دالة الرسالة
     */
    void on_message(OnMessageCallback callback);
    
    /**
     * @brief Set error callback / تعيين دالة الخطأ
     */
    void on_error(OnErrorCallback callback);
    
    /**
     * @brief Set ping callback / تعيين دالة النبض
     */
    void on_ping(OnPingCallback callback);
    
    /**
     * @brief Set pong callback / تعيين دالة الاستجابة
     */
    void on_pong(OnPongCallback callback);
    
    // ============================================================================
    // Configuration - الإعدادات
    // ============================================================================
    
    /**
     * @brief Set receive timeout / تعيين مهلة الاستقبال
     */
    void set_receive_timeout(int timeout_ms);
    
    /**
     * @brief Set send timeout / تعيين مهلة الإرسال
     */
    void set_send_timeout(int timeout_ms);
    
    /**
     * @brief Enable auto-reconnect / تفعيل إعادة الاتصال التلقائي
     */
    void set_auto_reconnect(bool enable, int max_attempts = 5, int delay_ms = 1000);
    
    /**
     * @brief Set ping interval / تعيين فترة النبض
     * @param interval_ms الفترة بالميلي ثانية (0 للتعطيل)
     */
    void set_ping_interval(int interval_ms);
    
    // ============================================================================
    // Information - المعلومات
    // ============================================================================
    
    /**
     * @brief Get remote address / الحصول على العنوان البعيد
     */
    std::string get_remote_address() const;
    
    /**
     * @brief Get URL / الحصول على URL
     */
    std::string get_url() const;
    
    /**
     * @brief Get last error / الحصول على آخر خطأ
     */
    std::string get_last_error() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    
    // Helper methods
    bool do_handshake();
    bool send_frame(const std::vector<uint8_t>& frame);
    void process_frame(const WebSocketFrame& frame);
    void handle_fragmented_message(const WebSocketFrame& frame);
    void receiver_thread_func();
    void heartbeat_thread_func();
};

} // namespace websocket
} // namespace network
} // namespace sad

#endif // SAD_WEBSOCKET_CLIENT_H
