// بسم الله الرحمن الرحيم
//
// websocket_bindings.h
// (AR) واجهة C الخارجية لمكتبة WebSocket — تُستخدم من المفسر والمترجم.
//      جميع الدوال آمنة من الاستثناءات (تستخدم try/catch داخلياً).
//      المؤشرات تُمرَّر كـ void* وتُحوَّل داخلياً إلى الأنواع الصحيحة.
//      handles فاشلة (nullptr) تُعالَج بأمان: تُرجع false/0/"".
//
// (EN) C ABI for the WebSocket library — used by both interpreter and compiler.
//      All functions are exception-safe (use internal try/catch).
//      Pointers are passed as void* and cast internally to proper types.
//      Failed handles (nullptr) are handled safely: return false/0/"".
//
// التصميم: يطابق نمط network_bindings.h لـ TCP/UDP/HTTP.

#ifndef SAD_WEBSOCKET_BINDINGS_H
#define SAD_WEBSOCKET_BINDINGS_H

#include <cstdint>
#include <cstddef>

#ifdef __cplusplus
extern "C"
{
#endif

    // ============================================================================
    // (AR) عميل WebSocket / (EN) WebSocket Client
    // ============================================================================

    /// (AR) إنشاء عميل WebSocket جديد. يُرجع handle أو nullptr.
    /// (EN) Create a new WebSocket client. Returns handle or nullptr.
    void *sad_ws_client_new(void);

    /// (AR) تحرير عميل WebSocket.
    /// (EN) Free WebSocket client.
    void sad_ws_client_free(void *client);

    /// (AR) الاتصال بخادم. url مثل "ws://example.com:80/path".
    /// (EN) Connect to a server. url like "ws://example.com:80/path".
    bool sad_ws_client_connect(void *client, const char *url);

    /// (AR) إغلاق الاتصال برمز ورسالة.
    /// (EN) Close connection with code and reason.
    void sad_ws_client_close(void *client, uint16_t code, const char *reason);

    /// (AR) هل الاتصال مفتوح؟
    /// (EN) Is the connection open?
    bool sad_ws_client_is_connected(void *client);

    /// (AR) إرسال رسالة نصية. يُرجع true عند النجاح.
    /// (EN) Send text message. Returns true on success.
    bool sad_ws_client_send(void *client, const char *message);

    /// (AR) إرسال بيانات ثنائية بالحجم المحدد.
    /// (EN) Send binary data with specified size.
    bool sad_ws_client_send_binary(void *client, const char *data, size_t size);

    /// (AR) إرسال نبض (Ping).
    /// (EN) Send ping frame.
    bool sad_ws_client_ping(void *client);

    /// (AR) استقبال رسالة (حظر مع مهلة بالميلي ثانية، -1 لانتظار لانهائي).
    ///      يُرجع نص الرسالة أو nullptr عند الفشل/المهلة.
    /// (EN) Receive message (blocks with timeout in ms, -1 for infinite).
    ///      Returns message text or nullptr on failure/timeout.
    const char *sad_ws_client_receive(void *client, int timeout_ms);

    /// (AR) هل توجد رسالة في قائمة الانتظار؟
    /// (EN) Is there a pending message?
    bool sad_ws_client_has_message(void *client);

    /// (AR) الحصول على URL المتصل.
    /// (EN) Get the connected URL.
    const char *sad_ws_client_get_url(void *client);

    /// (AR) الحصول على وصف آخر خطأ.
    /// (EN) Get the last error description.
    const char *sad_ws_client_last_error(void *client);

    /// (AR) ضبط مهلة الاستقبال بالميلي ثانية.
    /// (EN) Set receive timeout in ms.
    void sad_ws_client_set_recv_timeout(void *client, int timeout_ms);

    /// (AR) ضبط مهلة الإرسال بالميلي ثانية.
    /// (EN) Set send timeout in ms.
    void sad_ws_client_set_send_timeout(void *client, int timeout_ms);

    // ============================================================================
    // (AR) خادم WebSocket / (EN) WebSocket Server
    // ============================================================================

    /// (AR) إنشاء خادم WebSocket جديد.
    /// (EN) Create a new WebSocket server.
    void *sad_ws_server_new(void);

    /// (AR) تحرير خادم WebSocket (يوقفه أولاً).
    /// (EN) Free WebSocket server (stops it first).
    void sad_ws_server_free(void *server);

    /// (AR) بدء الخادم على منفذ ومضيف.
    /// (EN) Start server on port and host.
    bool sad_ws_server_start(void *server, uint16_t port, const char *host);

    /// (AR) إيقاف الخادم.
    /// (EN) Stop server.
    void sad_ws_server_stop(void *server);

    /// (AR) هل الخادم يعمل؟
    /// (EN) Is server running?
    bool sad_ws_server_is_running(void *server);

    /// (AR) عدد الاتصالات النشطة.
    /// (EN) Number of active connections.
    int sad_ws_server_connection_count(void *server);

    /// (AR) المنفذ الذي يستمع عليه الخادم.
    /// (EN) The port the server is listening on.
    uint16_t sad_ws_server_get_port(void *server);

    /// (AR) بث رسالة نصية لجميع العملاء. يُرجع عدد الذين أرسلت لهم.
    /// (EN) Broadcast text to all clients. Returns count sent to.
    int sad_ws_server_broadcast(void *server, const char *message);

    /// (AR) بث بيانات ثنائية للجميع.
    /// (EN) Broadcast binary to all.
    int sad_ws_server_broadcast_binary(void *server, const char *data, size_t size);

    /// (AR) بث رسالة لغرفة محددة.
    /// (EN) Broadcast to a specific room.
    int sad_ws_server_broadcast_to_room(void *server, const char *room, const char *message);

    /// (AR) إغلاق جميع الاتصالات.
    /// (EN) Close all connections.
    void sad_ws_server_close_all(void *server);

    /// (AR) الحصول على عدد الغرف.
    /// (EN) Get number of rooms.
    int sad_ws_server_room_count(void *server);

    /// (AR) عدد الاتصالات في غرفة.
    /// (EN) Number of connections in a room.
    int sad_ws_server_room_size(void *server, const char *room);

    // ============================================================================
    // (AR) إدارة الاتصالات في الخادم / (EN) Server Connection Management
    // ============================================================================

    /// (AR) الحصول على handle اتصال بمعرّفه.
    /// (EN) Get connection handle by ID.
    void *sad_ws_server_get_connection(void *server, uint64_t conn_id);

    /// (AR) إرسال رسالة لاتصال محدد.
    /// (EN) Send message to specific connection.
    bool sad_ws_conn_send(void *conn, const char *message);

    /// (AR) إغلاق اتصال محدد.
    /// (EN) Close specific connection.
    void sad_ws_conn_close(void *conn, uint16_t code, const char *reason);

    /// (AR) إضافة اتصال إلى غرفة.
    /// (EN) Add connection to a room.
    void sad_ws_conn_join_room(void *conn, const char *room);

    /// (AR) إزالة اتصال من غرفة.
    /// (EN) Remove connection from a room.
    void sad_ws_conn_leave_room(void *conn, const char *room);

    /// (AR) الحصول على العنوان البعيد للاتصال.
    /// (EN) Get remote address of connection.
    const char *sad_ws_conn_remote_address(void *conn);

    /// (AR) معرّف الاتصال.
    /// (EN) Connection ID.
    uint64_t sad_ws_conn_id(void *conn);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SAD_WEBSOCKET_BINDINGS_H
