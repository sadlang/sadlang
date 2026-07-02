// بسم الله الرحمن الرحيم
// websocket_server.h - WebSocket Server
// خادم WebSocket

#ifndef SAD_WEBSOCKET_SERVER_H
#define SAD_WEBSOCKET_SERVER_H

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
#include <map>
#include <set>
#include <functional>
#include <vector>

namespace sad {
namespace network {
namespace websocket {

// Forward declaration
class WebSocketServer;

/**
 * @brief WebSocket Connection / اتصال WebSocket
 * 
 * يمثل اتصال عميل واحد بالخادم
 * Represents a single client connection to the server
 */
class WebSocketConnection {
public:
    using ConnectionId = uint64_t;
    
    /**
     * @brief Get connection ID / الحصول على معرف الاتصال
     */
    ConnectionId get_id() const { return id_; }
    
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
    
    /**
     * @brief Close connection / إغلاق الاتصال
     */
    void close(CloseCode code = CloseCode::Normal, const std::string& reason = "");
    
    /**
     * @brief Check if connected / التحقق من الاتصال
     */
    bool is_connected() const { return connected_; }
    
    /**
     * @brief Get remote address / الحصول على العنوان البعيد
     */
    std::string get_remote_address() const { return remote_address_; }
    
    /**
     * @brief Get remote port / الحصول على المنفذ البعيد
     */
    uint16_t get_remote_port() const { return remote_port_; }
    
    /**
     * @brief Get custom data / الحصول على بيانات مخصصة
     */
    void* get_user_data() const { return user_data_; }
    
    /**
     * @brief Set custom data / تعيين بيانات مخصصة
     */
    void set_user_data(void* data) { user_data_ = data; }
    
    /**
     * @brief Join room / الانضمام لغرفة
     */
    void join_room(const std::string& room);
    
    /**
     * @brief Leave room / مغادرة غرفة
     */
    void leave_room(const std::string& room);
    
    /**
     * @brief Check if in room / التحقق من الوجود في غرفة
     */
    bool is_in_room(const std::string& room) const;
    
    /**
     * @brief Get rooms / الحصول على الغرف
     */
    std::set<std::string> get_rooms() const;
    
    /**
     * @brief Constructor (public for make_shared) / المُنشئ (عام لـ make_shared)
     */
    WebSocketConnection(ConnectionId id, SOCKET socket, 
                        const std::string& addr, uint16_t port,
                        WebSocketServer* server);

private:
    friend class WebSocketServer;
    
    bool send_frame(const std::vector<uint8_t>& frame);
    
    ConnectionId id_;
    SOCKET socket_;
    std::atomic<bool> connected_{true};
    std::string remote_address_;
    uint16_t remote_port_;
    void* user_data_ = nullptr;
    WebSocketServer* server_;
    std::set<std::string> rooms_;
    mutable std::mutex rooms_mutex_;
    std::mutex send_mutex_;
    
    // Fragmented message handling
    std::vector<uint8_t> fragment_buffer_;
    MessageType fragment_type_ = MessageType::Text;
};

using ConnectionPtr = std::shared_ptr<WebSocketConnection>;

// ============================================================================
// Server Callbacks - دوال استجابة الخادم
// ============================================================================

using OnConnectionCallback = std::function<void(ConnectionPtr)>;
using OnDisconnectCallback = std::function<void(ConnectionPtr, CloseCode, const std::string&)>;
using OnServerMessageCallback = std::function<void(ConnectionPtr, const WebSocketMessage&)>;
using OnServerErrorCallback = std::function<void(ConnectionPtr, const std::string&)>;

/**
 * @brief WebSocket Server / خادم WebSocket
 * 
 * خادم WebSocket يدعم اتصالات متعددة وإدارة الغرف
 * WebSocket server supporting multiple connections and room management
 */
class WebSocketServer {
public:
    /**
     * @brief Constructor / المُنشئ
     */
    WebSocketServer();
    
    /**
     * @brief Destructor / المُدمر
     */
    ~WebSocketServer();
    
    // Disable copy / منع النسخ
    WebSocketServer(const WebSocketServer&) = delete;
    WebSocketServer& operator=(const WebSocketServer&) = delete;
    
    // ============================================================================
    // Server Control - التحكم بالخادم
    // ============================================================================
    
    /**
     * @brief Start server / بدء الخادم
     * @param port المنفذ
     * @param host العنوان (افتراضي: 0.0.0.0)
     * @return true إذا نجح البدء
     */
    bool start(uint16_t port, const std::string& host = "0.0.0.0");
    
    /**
     * @brief Stop server / إيقاف الخادم
     */
    void stop();
    
    /**
     * @brief Check if running / التحقق من التشغيل
     */
    bool is_running() const;
    
    /**
     * @brief Get port / الحصول على المنفذ
     */
    uint16_t get_port() const;
    
    // ============================================================================
    // Client Management - إدارة العملاء
    // ============================================================================
    
    /**
     * @brief Get connection count / الحصول على عدد الاتصالات
     */
    size_t get_connection_count() const;
    
    /**
     * @brief Get all connections / الحصول على جميع الاتصالات
     */
    std::vector<ConnectionPtr> get_connections() const;
    
    /**
     * @brief Get connection by ID / الحصول على اتصال بالمعرف
     */
    ConnectionPtr get_connection(WebSocketConnection::ConnectionId id) const;
    
    /**
     * @brief Close connection / إغلاق اتصال
     */
    void close_connection(WebSocketConnection::ConnectionId id, 
                          CloseCode code = CloseCode::Normal,
                          const std::string& reason = "");
    
    /**
     * @brief Close all connections / إغلاق جميع الاتصالات
     */
    void close_all_connections(CloseCode code = CloseCode::GoingAway,
                               const std::string& reason = "Server shutting down");
    
    // ============================================================================
    // Broadcasting - البث
    // ============================================================================
    
    /**
     * @brief Broadcast text message to all / بث رسالة نصية للجميع
     */
    void broadcast(const std::string& message);
    
    /**
     * @brief Broadcast binary to all / بث بيانات ثنائية للجميع
     */
    void broadcast_binary(const std::vector<uint8_t>& data);
    
    /**
     * @brief Broadcast to room / بث لغرفة
     */
    void broadcast_to_room(const std::string& room, const std::string& message);
    
    /**
     * @brief Broadcast binary to room / بث ثنائي لغرفة
     */
    void broadcast_binary_to_room(const std::string& room, const std::vector<uint8_t>& data);
    
    /**
     * @brief Broadcast except / بث باستثناء
     */
    void broadcast_except(const std::string& message, WebSocketConnection::ConnectionId except_id);
    
    // ============================================================================
    // Room Management - إدارة الغرف
    // ============================================================================
    
    /**
     * @brief Get rooms / الحصول على الغرف
     */
    std::set<std::string> get_rooms() const;
    
    /**
     * @brief Get connections in room / الحصول على اتصالات الغرفة
     */
    std::vector<ConnectionPtr> get_room_connections(const std::string& room) const;
    
    /**
     * @brief Get room size / الحصول على حجم الغرفة
     */
    size_t get_room_size(const std::string& room) const;
    
    // ============================================================================
    // Callbacks - دوال الاستجابة
    // ============================================================================
    
    /**
     * @brief Set connection callback / تعيين دالة الاتصال
     */
    void on_connection(OnConnectionCallback callback);
    
    /**
     * @brief Set disconnect callback / تعيين دالة قطع الاتصال
     */
    void on_disconnect(OnDisconnectCallback callback);
    
    /**
     * @brief Set message callback / تعيين دالة الرسالة
     */
    void on_message(OnServerMessageCallback callback);
    
    /**
     * @brief Set error callback / تعيين دالة الخطأ
     */
    void on_error(OnServerErrorCallback callback);
    
    // ============================================================================
    // Configuration - الإعدادات
    // ============================================================================
    
    /**
     * @brief Set max connections / تعيين أقصى عدد اتصالات
     */
    void set_max_connections(size_t max_connections);
    
    /**
     * @brief Set ping interval / تعيين فترة النبض
     */
    void set_ping_interval(int interval_ms);
    
    /**
     * @brief Set connection timeout / تعيين مهلة الاتصال
     */
    void set_connection_timeout(int timeout_ms);

private:
    friend class WebSocketConnection;
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
    
    void accept_thread_func();
    void handle_client(ConnectionPtr conn);
    bool perform_handshake(SOCKET client_socket, std::string& error);
    void remove_connection(WebSocketConnection::ConnectionId id);
    
    // Room management internal
    void add_to_room(WebSocketConnection::ConnectionId id, const std::string& room);
    void remove_from_room(WebSocketConnection::ConnectionId id, const std::string& room);
    void remove_from_all_rooms(WebSocketConnection::ConnectionId id);
};

} // namespace websocket
} // namespace network
} // namespace sad

#endif // SAD_WEBSOCKET_SERVER_H
