/**
 * @file builtin_module_websocket.cpp
 * @brief (AR) وحدة WebSocket — عميل وخادم اتصال ثنائي الاتجاه
 * @brief (EN) WebSocket module — client and server bidirectional connections
 *
 * @details
 * (AR) توفر هذه الوحدة دوالاً عربية لـ WebSocket عبر معرّفات صحيحة:
 *   1. عميل WebSocket: اتصال، إرسال، استقبال، إغلاق
 *   2. خادم WebSocket: تشغيل، بث، إدارة غرف، إغلاق
 *
 * (EN) Arabic functions for WebSocket via integer handles:
 *   1. WebSocket Client: connect, send, receive, close
 *   2. WebSocket Server: start, broadcast, room management, close
 *
 * @see features/network/core/include/websocket/websocket_bindings.h
 */

#include "interpreter_core.h"
#include "value.h"
#include "builtin_registry.h"

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>

#ifdef HAS_NETWORK_LIB
extern "C"
{
#include "websocket/websocket_bindings.h"
}
#endif

namespace Sad
{
    namespace Interpreter
    {
        namespace Bwsc = Sad::Builtins::Names::WebSocketClient;
        namespace Bwss = Sad::Builtins::Names::WebSocketServer;

#ifdef HAS_NETWORK_LIB

        // ════════════════════════════════════════════════════════════
        // (AR) جداول مؤشرات الـ WebSocket — handle integer → void*
        // (EN) WebSocket pointer tables — handle integer → void*
        // ════════════════════════════════════════════════════════════
        static std::mutex s_wsMutex;
        static std::unordered_map<int64_t, void *> s_wsClients;
        static std::unordered_map<int64_t, void *> s_wsServers;
        static std::atomic<int64_t> s_nextWsClientId{1};
        static std::atomic<int64_t> s_nextWsServerId{1};

        static int64_t storeWsClient(void *p)
        {
            std::lock_guard<std::mutex> lock(s_wsMutex);
            int64_t id = s_nextWsClientId++;
            s_wsClients[id] = p;
            return id;
        }

        static void *getWsClient(int64_t id)
        {
            std::lock_guard<std::mutex> lock(s_wsMutex);
            auto it = s_wsClients.find(id);
            return (it != s_wsClients.end()) ? it->second : nullptr;
        }

        static void eraseWsClient(int64_t id)
        {
            std::lock_guard<std::mutex> lock(s_wsMutex);
            s_wsClients.erase(id);
        }

        static int64_t storeWsServer(void *p)
        {
            std::lock_guard<std::mutex> lock(s_wsMutex);
            int64_t id = s_nextWsServerId++;
            s_wsServers[id] = p;
            return id;
        }

        static void *getWsServer(int64_t id)
        {
            std::lock_guard<std::mutex> lock(s_wsMutex);
            auto it = s_wsServers.find(id);
            return (it != s_wsServers.end()) ? it->second : nullptr;
        }

        static void eraseWsServer(int64_t id)
        {
            std::lock_guard<std::mutex> lock(s_wsMutex);
            s_wsServers.erase(id);
        }

        // ════════════════════════════════════════════════════════════
        // (AR) مساعدات لاستخراج الوسائط
        // ════════════════════════════════════════════════════════════
        static std::string argStr(
            const std::vector<std::shared_ptr<Data::Value>> &args,
            size_t i, const std::string &def = "")
        {
            if (i < args.size() && args[i])
                return args[i]->toString();
            return def;
        }

        static double argNum(
            const std::vector<std::shared_ptr<Data::Value>> &args,
            size_t i, double def = 0.0)
        {
            if (i < args.size() && args[i])
                return args[i]->toDouble();
            return def;
        }

#endif // HAS_NETWORK_LIB

        // ════════════════════════════════════════════════════════════
        // (AR) تسجيل دوال WebSocket في المفسر
        // (EN) Register WebSocket functions in interpreter
        // ════════════════════════════════════════════════════════════
        void registerBuiltinsWebSocket(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

#ifdef HAS_NETWORK_LIB

            // ─────────── عميل WebSocket ───────────

            // أنشئ_عميل_ويبسوكت() → معرّف
            fm.registerBuiltinFunction(std::string(Bwsc::NEW_CLIENT),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           (void)args;
                                           void *c = sad_ws_client_new();
                                           if (!c)
                                               return std::make_shared<Data::Value>(static_cast<double>(-1));
                                           return std::make_shared<Data::Value>(static_cast<double>(storeWsClient(c)));
                                       });

            // أزل_عميل_ويبسوكت(معرّف) → منطقي
            fm.registerBuiltinFunction(std::string(Bwsc::FREE_CLIENT),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           void *c = getWsClient(id);
                                           if (!c)
                                               return std::make_shared<Data::Value>(false);
                                           sad_ws_client_free(c);
                                           eraseWsClient(id);
                                           return std::make_shared<Data::Value>(true);
                                       });

            // اتصل_بويبسوكت(معرّف، رابط) → منطقي
            fm.registerBuiltinFunction(std::string(Bwsc::CONNECT),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           std::string url = argStr(args, 1);
                                           void *c = getWsClient(id);
                                           if (!c)
                                               return std::make_shared<Data::Value>(false);
                                           return std::make_shared<Data::Value>(
                                               sad_ws_client_connect(c, url.c_str()));
                                       });

            // أغلق_ويبسوكت(معرّف، رمز، سبب) → فراغ
            fm.registerBuiltinFunction(std::string(Bwsc::CLOSE),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           uint16_t code = static_cast<uint16_t>(argNum(args, 1, 1000));
                                           std::string reason = argStr(args, 2);
                                           void *c = getWsClient(id);
                                           if (c)
                                               sad_ws_client_close(c, code, reason.c_str());
                                           return std::make_shared<Data::Value>(true);
                                       });

            // هل_متصل_ويبسوكت(معرّف) → منطقي
            fm.registerBuiltinFunction(std::string(Bwsc::IS_CONNECTED),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           void *c = getWsClient(id);
                                           if (!c)
                                               return std::make_shared<Data::Value>(false);
                                           return std::make_shared<Data::Value>(sad_ws_client_is_connected(c));
                                       });

            // أرسل_ويبسوكت(معرّف، رسالة) → منطقي
            fm.registerBuiltinFunction(std::string(Bwsc::SEND),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           std::string msg = argStr(args, 1);
                                           void *c = getWsClient(id);
                                           if (!c)
                                               return std::make_shared<Data::Value>(false);
                                           return std::make_shared<Data::Value>(
                                               sad_ws_client_send(c, msg.c_str()));
                                       });

            // نبض_ويبسوكت(معرّف) → منطقي
            fm.registerBuiltinFunction(std::string(Bwsc::PING),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           void *c = getWsClient(id);
                                           if (!c)
                                               return std::make_shared<Data::Value>(false);
                                           return std::make_shared<Data::Value>(sad_ws_client_ping(c));
                                       });

            // استقبل_ويبسوكت(معرّف، مهلة_بالمللي) → نص
            fm.registerBuiltinFunction(std::string(Bwsc::RECEIVE),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           int timeout = static_cast<int>(argNum(args, 1, 5000));
                                           void *c = getWsClient(id);
                                           if (!c)
                                               return std::make_shared<Data::Value>(std::string(""));
                                           const char *res = sad_ws_client_receive(c, timeout);
                                           return std::make_shared<Data::Value>(std::string(res ? res : ""));
                                       });

            // يوجد_رسالة_ويبسوكت(معرّف) → منطقي
            fm.registerBuiltinFunction(std::string(Bwsc::HAS_MESSAGE),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           void *c = getWsClient(id);
                                           if (!c)
                                               return std::make_shared<Data::Value>(false);
                                           return std::make_shared<Data::Value>(sad_ws_client_has_message(c));
                                       });

            // رابط_ويبسوكت(معرّف) → نص
            fm.registerBuiltinFunction(std::string(Bwsc::GET_URL),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           void *c = getWsClient(id);
                                           if (!c)
                                               return std::make_shared<Data::Value>(std::string(""));
                                           const char *u = sad_ws_client_get_url(c);
                                           return std::make_shared<Data::Value>(std::string(u ? u : ""));
                                       });

            // خطأ_ويبسوكت(معرّف) → نص
            fm.registerBuiltinFunction(std::string(Bwsc::LAST_ERROR),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           void *c = getWsClient(id);
                                           if (!c)
                                               return std::make_shared<Data::Value>(std::string(""));
                                           const char *e = sad_ws_client_last_error(c);
                                           return std::make_shared<Data::Value>(std::string(e ? e : ""));
                                       });

            // حدد_مهلة_استقبال_ويبسوكت(معرّف، مللي) → فراغ
            fm.registerBuiltinFunction(std::string(Bwsc::SET_RECV_TIMEOUT),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           int t = static_cast<int>(argNum(args, 1, 0));
                                           void *c = getWsClient(id);
                                           if (c)
                                               sad_ws_client_set_recv_timeout(c, t);
                                           return std::make_shared<Data::Value>(true);
                                       });

            // حدد_مهلة_إرسال_ويبسوكت(معرّف، مللي) → فراغ
            fm.registerBuiltinFunction(std::string(Bwsc::SET_SEND_TIMEOUT),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           int t = static_cast<int>(argNum(args, 1, 0));
                                           void *c = getWsClient(id);
                                           if (c)
                                               sad_ws_client_set_send_timeout(c, t);
                                           return std::make_shared<Data::Value>(true);
                                       });

            // ─────────── خادم WebSocket ───────────

            // أنشئ_خادم_ويبسوكت() → معرّف
            fm.registerBuiltinFunction(std::string(Bwss::NEW_SERVER),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           (void)args;
                                           void *s = sad_ws_server_new();
                                           if (!s)
                                               return std::make_shared<Data::Value>(static_cast<double>(-1));
                                           return std::make_shared<Data::Value>(static_cast<double>(storeWsServer(s)));
                                       });

            // أزل_خادم_ويبسوكت(معرّف) → منطقي
            fm.registerBuiltinFunction(std::string(Bwss::FREE_SERVER),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           void *s = getWsServer(id);
                                           if (!s)
                                               return std::make_shared<Data::Value>(false);
                                           sad_ws_server_free(s);
                                           eraseWsServer(id);
                                           return std::make_shared<Data::Value>(true);
                                       });

            // ابدأ_خادم_ويبسوكت(معرّف، منفذ، مضيف) → منطقي
            fm.registerBuiltinFunction(std::string(Bwss::START),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           uint16_t port = static_cast<uint16_t>(argNum(args, 1, 0));
                                           std::string host = argStr(args, 2, "0.0.0.0");
                                           void *s = getWsServer(id);
                                           if (!s)
                                               return std::make_shared<Data::Value>(false);
                                           return std::make_shared<Data::Value>(
                                               sad_ws_server_start(s, port, host.c_str()));
                                       });

            // أوقف_خادم_ويبسوكت(معرّف) → فراغ
            fm.registerBuiltinFunction(std::string(Bwss::STOP),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           void *s = getWsServer(id);
                                           if (s)
                                               sad_ws_server_stop(s);
                                           return std::make_shared<Data::Value>(true);
                                       });

            // هل_يعمل_خادم_ويبسوكت(معرّف) → منطقي
            fm.registerBuiltinFunction(std::string(Bwss::IS_RUNNING),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           void *s = getWsServer(id);
                                           if (!s)
                                               return std::make_shared<Data::Value>(false);
                                           return std::make_shared<Data::Value>(sad_ws_server_is_running(s));
                                       });

            // عدد_اتصالات_ويبسوكت(معرّف) → رقم
            fm.registerBuiltinFunction(std::string(Bwss::CONN_COUNT),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           void *s = getWsServer(id);
                                           if (!s)
                                               return std::make_shared<Data::Value>(0.0);
                                           return std::make_shared<Data::Value>(
                                               static_cast<double>(sad_ws_server_connection_count(s)));
                                       });

            // منفذ_خادم_ويبسوكت(معرّف) → رقم
            fm.registerBuiltinFunction(std::string(Bwss::GET_PORT),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           void *s = getWsServer(id);
                                           if (!s)
                                               return std::make_shared<Data::Value>(0.0);
                                           return std::make_shared<Data::Value>(
                                               static_cast<double>(sad_ws_server_get_port(s)));
                                       });

            // بث_ويبسوكت(معرّف، رسالة) → رقم (عدد الذين أُرسل لهم)
            fm.registerBuiltinFunction(std::string(Bwss::BROADCAST),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           std::string msg = argStr(args, 1);
                                           void *s = getWsServer(id);
                                           if (!s)
                                               return std::make_shared<Data::Value>(0.0);
                                           return std::make_shared<Data::Value>(
                                               static_cast<double>(sad_ws_server_broadcast(s, msg.c_str())));
                                       });

            // بث_لغرفة_ويبسوكت(معرّف، غرفة، رسالة) → رقم
            fm.registerBuiltinFunction(std::string(Bwss::BROADCAST_ROOM),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           std::string room = argStr(args, 1);
                                           std::string msg = argStr(args, 2);
                                           void *s = getWsServer(id);
                                           if (!s)
                                               return std::make_shared<Data::Value>(0.0);
                                           return std::make_shared<Data::Value>(
                                               static_cast<double>(sad_ws_server_broadcast_to_room(
                                                   s, room.c_str(), msg.c_str())));
                                       });

            // عدد_غرف_ويبسوكت(معرّف) → رقم
            fm.registerBuiltinFunction(std::string(Bwss::ROOM_COUNT),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           void *s = getWsServer(id);
                                           if (!s)
                                               return std::make_shared<Data::Value>(0.0);
                                           return std::make_shared<Data::Value>(
                                               static_cast<double>(sad_ws_server_room_count(s)));
                                       });

            // حجم_غرفة_ويبسوكت(معرّف، غرفة) → رقم
            fm.registerBuiltinFunction(std::string(Bwss::ROOM_SIZE),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           std::string room = argStr(args, 1);
                                           void *s = getWsServer(id);
                                           if (!s)
                                               return std::make_shared<Data::Value>(0.0);
                                           return std::make_shared<Data::Value>(
                                               static_cast<double>(sad_ws_server_room_size(s, room.c_str())));
                                       });

            // أغلق_كل_اتصالات_ويبسوكت(معرّف) → فراغ
            fm.registerBuiltinFunction(std::string(Bwss::CLOSE_ALL),
                                       [](Sad::Interpreter::BuiltinContext &ctx)
                                           -> std::shared_ptr<Data::Value>
                                       {
                const auto &args = ctx.args(); (void)args;
                                           int64_t id = static_cast<int64_t>(argNum(args, 0, -1));
                                           void *s = getWsServer(id);
                                           if (s)
                                               sad_ws_server_close_all(s);
                                           return std::make_shared<Data::Value>(true);
                                       });

#else // HAS_NETWORK_LIB

            // (AR) مكتبة الشبكة غير متاحة - تسجيل دوال بديلة تُرجع خطأ
            (void)fm;

#endif // HAS_NETWORK_LIB
        }

    } // namespace Interpreter
} // namespace Sad
