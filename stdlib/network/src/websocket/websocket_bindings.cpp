// بسم الله الرحمن الرحيم
//
// websocket_bindings.cpp
// (AR) تنفيذ واجهة C الخارجية لمكتبة WebSocket.
//      جميع الدوال تستخدم try/catch لمنع تسرب الاستثناءات إلى C ABI.
//      المؤشرات الفاسدة تُعالَج بأمان (تُرجع null/0/false).
//      النصوص المُرجعة تُخزَّن في thread_local std::string لضمان السلامة.
//
// (EN) Implementation of C ABI for the WebSocket library.
//      All functions use try/catch to prevent exceptions leaking through C ABI.
//      Bad pointers are handled safely (return null/0/false).
//      Returned strings are stored in thread_local std::string for safety.

#include "websocket/websocket_bindings.h"
#include "websocket/websocket_client.h"
#include "websocket/websocket_server.h"
#include "websocket/websocket_base.h"

#include <string>
#include <vector>
#include <memory>
#include <cstring>

using namespace sad::network::websocket;

namespace
{
    // (AR) حماية مؤشر العميل: يُرجع false عند nullptr.
    // (EN) Protect client pointer: returns false on nullptr.
    inline WebSocketClient *as_client(void *p) noexcept
    {
        return p ? static_cast<WebSocketClient *>(p) : nullptr;
    }

    inline WebSocketServer *as_server(void *p) noexcept
    {
        return p ? static_cast<WebSocketServer *>(p) : nullptr;
    }

    inline WebSocketConnection *as_conn(void *p) noexcept
    {
        return p ? static_cast<WebSocketConnection *>(p) : nullptr;
    }

    // (AR) تخزين آمن للنصوص المُرجعة لكل خيط.
    // (EN) Thread-local safe storage for returned strings.
    thread_local std::string g_last_recv_buffer;
    thread_local std::string g_last_url_buffer;
    thread_local std::string g_last_error_buffer;
    thread_local std::string g_last_addr_buffer;
}

extern "C"
{

    // ========================================================================
    // (AR) عميل WebSocket
    // ========================================================================

    void *sad_ws_client_new(void)
    {
        try
        {
            return new WebSocketClient();
        }
        catch (...)
        {
            return nullptr;
        }
    }

    void sad_ws_client_free(void *client)
    {
        if (!client)
            return;
        try
        {
            delete as_client(client);
        }
        catch (...)
        {
            // (AR) لا يمكن إعادة الاستثناء عبر C ABI
        }
    }

    bool sad_ws_client_connect(void *client, const char *url)
    {
        WebSocketClient *c = as_client(client);
        if (!c || !url)
            return false;
        try
        {
            return c->connect(std::string(url));
        }
        catch (...)
        {
            return false;
        }
    }

    void sad_ws_client_close(void *client, uint16_t code, const char *reason)
    {
        WebSocketClient *c = as_client(client);
        if (!c)
            return;
        try
        {
            CloseCode cc = static_cast<CloseCode>(code == 0 ? 1000 : code);
            c->close(cc, reason ? std::string(reason) : std::string());
        }
        catch (...)
        {
        }
    }

    bool sad_ws_client_is_connected(void *client)
    {
        WebSocketClient *c = as_client(client);
        if (!c)
            return false;
        try
        {
            return c->is_connected();
        }
        catch (...)
        {
            return false;
        }
    }

    bool sad_ws_client_send(void *client, const char *message)
    {
        WebSocketClient *c = as_client(client);
        if (!c || !message)
            return false;
        try
        {
            return c->send(std::string(message));
        }
        catch (...)
        {
            return false;
        }
    }

    bool sad_ws_client_send_binary(void *client, const char *data, size_t size)
    {
        WebSocketClient *c = as_client(client);
        if (!c || !data || size == 0)
            return false;
        try
        {
            std::vector<uint8_t> buf(
                reinterpret_cast<const uint8_t *>(data),
                reinterpret_cast<const uint8_t *>(data) + size);
            return c->send_binary(buf);
        }
        catch (...)
        {
            return false;
        }
    }

    bool sad_ws_client_ping(void *client)
    {
        WebSocketClient *c = as_client(client);
        if (!c)
            return false;
        try
        {
            return c->ping();
        }
        catch (...)
        {
            return false;
        }
    }

    const char *sad_ws_client_receive(void *client, int timeout_ms)
    {
        g_last_recv_buffer.clear();
        WebSocketClient *c = as_client(client);
        if (!c)
            return g_last_recv_buffer.c_str();
        try
        {
            WebSocketMessage msg = c->receive(timeout_ms);
            // (AR) نُحوّل البيانات إلى نص. للنصوص يُمرَّر كما هو.
            //      للثنائيات يُمرَّر كنص خام (قد يحتوي 0).
            // (EN) Convert payload to string. For text passes as-is.
            //      For binary passes as raw bytes (may contain 0).
            g_last_recv_buffer.assign(
                reinterpret_cast<const char *>(msg.data.data()),
                msg.data.size());
        }
        catch (...)
        {
            g_last_recv_buffer.clear();
        }
        return g_last_recv_buffer.c_str();
    }

    bool sad_ws_client_has_message(void *client)
    {
        WebSocketClient *c = as_client(client);
        if (!c)
            return false;
        try
        {
            return c->has_message();
        }
        catch (...)
        {
            return false;
        }
    }

    const char *sad_ws_client_get_url(void *client)
    {
        g_last_url_buffer.clear();
        WebSocketClient *c = as_client(client);
        if (!c)
            return g_last_url_buffer.c_str();
        try
        {
            g_last_url_buffer = c->get_url();
        }
        catch (...)
        {
            g_last_url_buffer.clear();
        }
        return g_last_url_buffer.c_str();
    }

    const char *sad_ws_client_last_error(void *client)
    {
        g_last_error_buffer.clear();
        WebSocketClient *c = as_client(client);
        if (!c)
            return g_last_error_buffer.c_str();
        try
        {
            g_last_error_buffer = c->get_last_error();
        }
        catch (...)
        {
            g_last_error_buffer.clear();
        }
        return g_last_error_buffer.c_str();
    }

    void sad_ws_client_set_recv_timeout(void *client, int timeout_ms)
    {
        WebSocketClient *c = as_client(client);
        if (!c)
            return;
        try
        {
            c->set_receive_timeout(timeout_ms);
        }
        catch (...)
        {
        }
    }

    void sad_ws_client_set_send_timeout(void *client, int timeout_ms)
    {
        WebSocketClient *c = as_client(client);
        if (!c)
            return;
        try
        {
            c->set_send_timeout(timeout_ms);
        }
        catch (...)
        {
        }
    }

    // ========================================================================
    // (AR) خادم WebSocket
    // ========================================================================

    void *sad_ws_server_new(void)
    {
        try
        {
            return new WebSocketServer();
        }
        catch (...)
        {
            return nullptr;
        }
    }

    void sad_ws_server_free(void *server)
    {
        if (!server)
            return;
        try
        {
            delete as_server(server);
        }
        catch (...)
        {
        }
    }

    bool sad_ws_server_start(void *server, uint16_t port, const char *host)
    {
        WebSocketServer *s = as_server(server);
        if (!s)
            return false;
        try
        {
            std::string h = host ? std::string(host) : std::string("0.0.0.0");
            return s->start(port, h);
        }
        catch (...)
        {
            return false;
        }
    }

    void sad_ws_server_stop(void *server)
    {
        WebSocketServer *s = as_server(server);
        if (!s)
            return;
        try
        {
            s->stop();
        }
        catch (...)
        {
        }
    }

    bool sad_ws_server_is_running(void *server)
    {
        WebSocketServer *s = as_server(server);
        if (!s)
            return false;
        try
        {
            return s->is_running();
        }
        catch (...)
        {
            return false;
        }
    }

    int sad_ws_server_connection_count(void *server)
    {
        WebSocketServer *s = as_server(server);
        if (!s)
            return 0;
        try
        {
            return static_cast<int>(s->get_connection_count());
        }
        catch (...)
        {
            return 0;
        }
    }

    uint16_t sad_ws_server_get_port(void *server)
    {
        WebSocketServer *s = as_server(server);
        if (!s)
            return 0;
        try
        {
            return s->get_port();
        }
        catch (...)
        {
            return 0;
        }
    }

    int sad_ws_server_broadcast(void *server, const char *message)
    {
        WebSocketServer *s = as_server(server);
        if (!s || !message)
            return 0;
        try
        {
            int count = static_cast<int>(s->get_connection_count());
            s->broadcast(std::string(message));
            return count;
        }
        catch (...)
        {
            return 0;
        }
    }

    int sad_ws_server_broadcast_binary(void *server, const char *data, size_t size)
    {
        WebSocketServer *s = as_server(server);
        if (!s || !data || size == 0)
            return 0;
        try
        {
            int count = static_cast<int>(s->get_connection_count());
            std::vector<uint8_t> buf(
                reinterpret_cast<const uint8_t *>(data),
                reinterpret_cast<const uint8_t *>(data) + size);
            s->broadcast_binary(buf);
            return count;
        }
        catch (...)
        {
            return 0;
        }
    }

    int sad_ws_server_broadcast_to_room(void *server, const char *room, const char *message)
    {
        WebSocketServer *s = as_server(server);
        if (!s || !room || !message)
            return 0;
        try
        {
            std::string r(room);
            int count = static_cast<int>(s->get_room_size(r));
            s->broadcast_to_room(r, std::string(message));
            return count;
        }
        catch (...)
        {
            return 0;
        }
    }

    void sad_ws_server_close_all(void *server)
    {
        WebSocketServer *s = as_server(server);
        if (!s)
            return;
        try
        {
            s->close_all_connections();
        }
        catch (...)
        {
        }
    }

    int sad_ws_server_room_count(void *server)
    {
        WebSocketServer *s = as_server(server);
        if (!s)
            return 0;
        try
        {
            return static_cast<int>(s->get_rooms().size());
        }
        catch (...)
        {
            return 0;
        }
    }

    int sad_ws_server_room_size(void *server, const char *room)
    {
        WebSocketServer *s = as_server(server);
        if (!s || !room)
            return 0;
        try
        {
            return static_cast<int>(s->get_room_size(std::string(room)));
        }
        catch (...)
        {
            return 0;
        }
    }

    // ========================================================================
    // (AR) إدارة الاتصالات في الخادم
    // ========================================================================

    void *sad_ws_server_get_connection(void *server, uint64_t conn_id)
    {
        WebSocketServer *s = as_server(server);
        if (!s)
            return nullptr;
        try
        {
            ConnectionPtr ptr = s->get_connection(conn_id);
            // (AR) نُرجع raw pointer من shared_ptr — يبقى حياً ما دام الخادم حياً.
            // (EN) Return raw pointer from shared_ptr — alive while server lives.
            return ptr ? ptr.get() : nullptr;
        }
        catch (...)
        {
            return nullptr;
        }
    }

    bool sad_ws_conn_send(void *conn, const char *message)
    {
        WebSocketConnection *c = as_conn(conn);
        if (!c || !message)
            return false;
        try
        {
            return c->send(std::string(message));
        }
        catch (...)
        {
            return false;
        }
    }

    void sad_ws_conn_close(void *conn, uint16_t code, const char *reason)
    {
        WebSocketConnection *c = as_conn(conn);
        if (!c)
            return;
        try
        {
            CloseCode cc = static_cast<CloseCode>(code == 0 ? 1000 : code);
            c->close(cc, reason ? std::string(reason) : std::string());
        }
        catch (...)
        {
        }
    }

    void sad_ws_conn_join_room(void *conn, const char *room)
    {
        WebSocketConnection *c = as_conn(conn);
        if (!c || !room)
            return;
        try
        {
            c->join_room(std::string(room));
        }
        catch (...)
        {
        }
    }

    void sad_ws_conn_leave_room(void *conn, const char *room)
    {
        WebSocketConnection *c = as_conn(conn);
        if (!c || !room)
            return;
        try
        {
            c->leave_room(std::string(room));
        }
        catch (...)
        {
        }
    }

    const char *sad_ws_conn_remote_address(void *conn)
    {
        g_last_addr_buffer.clear();
        WebSocketConnection *c = as_conn(conn);
        if (!c)
            return g_last_addr_buffer.c_str();
        try
        {
            g_last_addr_buffer = c->get_remote_address();
        }
        catch (...)
        {
            g_last_addr_buffer.clear();
        }
        return g_last_addr_buffer.c_str();
    }

    uint64_t sad_ws_conn_id(void *conn)
    {
        WebSocketConnection *c = as_conn(conn);
        if (!c)
            return 0;
        try
        {
            return c->get_id();
        }
        catch (...)
        {
            return 0;
        }
    }

} // extern "C"
