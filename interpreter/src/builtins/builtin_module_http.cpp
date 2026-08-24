/**
 * @file builtin_module_http.cpp
 * @brief (AR) وحدة الشبكة — عميل HTTP، خادم HTTP، أدوات الشبكة العامة
 * @brief (EN) Network module — HTTP client, HTTP server, network utilities
 *
 * @details
 * (AR) توفر هذه الوحدة دوالاً عربية دلالية للتعامل مع:
 *   1. عميل HTTP: إرسال طلبات (اجلب، أرسل، استبدل، احذف، عدّل)
 *   2. خادم HTTP: استقبال ومعالجة الطلبات
 *   3. أدوات عامة: تهيئة الشبكة، العناوين، الأخطاء
 *
 * (EN) This module provides Arabic semantic functions for:
 *   1. HTTP Client: sending requests (GET, POST, PUT, DELETE, PATCH)
 *   2. HTTP Server: receiving and handling requests
 *   3. Utilities: network init, addresses, errors
 *
 * @note الدوال تعتمد على واجهة C في features/network/core
 * @see builtin_registry.h — ثوابت الأسماء (HttpClient, HttpServer, NetworkUtils)
 * @see features/network/core/include/http_bindings.h — واجهة C للـ HTTP
 * @see features/network/core/include/network/network_bindings.h — واجهة C للشبكة
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
#include <sstream>
#include <functional>

// ═══════════════════════════════════════════════════════════════════════
// (AR) تضمين واجهات C الخارجية إن توفرت
// (EN) Include external C bindings if available
// ═══════════════════════════════════════════════════════════════════════
#ifdef HAS_NETWORK_LIB
extern "C"
{
#include "http_bindings.h"
#include "network/network_bindings.h"
#include "builtins/builtin_context.h"
}
#endif

namespace Sad
{
    namespace Interpreter
    {
        // ═══════════════════════════════════════════════════════════════
        // (AR) اختصارات فضاءات الأسماء
        // (EN) Namespace aliases for Names constants
        // ═══════════════════════════════════════════════════════════════
        namespace Bhc = Sad::Builtins::Names::HttpClient;
        namespace Bhs = Sad::Builtins::Names::HttpServer;
        namespace Bnu = Sad::Builtins::Names::NetworkUtils;

        // ═══════════════════════════════════════════════════════════════
        // (AR) دالة مساعدة: إنشاء قيمة خطأ
        // (EN) Helper: create error value
        // ═══════════════════════════════════════════════════════════════
        static std::shared_ptr<Data::Value> makeError(const std::string &msg)
        {
            return std::make_shared<Data::Value>("خطأ: " + msg);
        }

        // ═══════════════════════════════════════════════════════════════
        // (AR) دالة مساعدة: استخراج نص من وسيط
        // (EN) Helper: extract string from argument
        // ═══════════════════════════════════════════════════════════════
        static std::string getStringArg(
            const std::vector<std::shared_ptr<Data::Value>> &args,
            size_t index,
            const std::string &defaultVal = "")
        {
            if (index < args.size() && args[index])
            {
                return args[index]->toString();
            }
            return defaultVal;
        }

        // ═══════════════════════════════════════════════════════════════
        // (AR) دالة مساعدة: استخراج رقم من وسيط
        // (EN) Helper: extract number from argument
        // ═══════════════════════════════════════════════════════════════
        static double getNumArg(
            const std::vector<std::shared_ptr<Data::Value>> &args,
            size_t index,
            double defaultVal = 0.0)
        {
            if (index < args.size() && args[index])
            {
                return args[index]->toDouble();
            }
            return defaultVal;
        }

#ifdef HAS_NETWORK_LIB

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) القسم 1: عميل HTTP — إرسال واستقبال طلبات الويب
        // (EN) Section 1: HTTP Client — sending and receiving web requests
        // ═══════════════════════════════════════════════════════════════════════

        // ─── تخزين مؤشرات العملاء والاستجابات ───
        // (AR) ع-11: فضاءا المعرّفات كانا متصادمين — عدّادا العميل والاستجابة
        //      يبدآن كلاهما من 1، وقارئات الرد (نص_الرد...) تتوقع معرّف استجابة
        //      بينما stdlib شبكات.ص تمرر معرّف العميل. فكان «عميل 1 + طلب أول»
        //      يصادف الصواب، وكل طلب لاحق على العميل نفسه يعيد **الرد الأول**
        //      بصمت. الفصل: الاستجابات تبدأ من مليار، والقارئات تحل المعرّف
        //      استجابةً أولًا ثم عميلًا (آخر استجابة له) داخل withResponse.
        // (EN) ع-11: the id spaces collided — both counters started at 1 while the
        //      response readers expect a response id and stdlib passes the client
        //      id. First client + first request happened to match; every later
        //      request silently returned the FIRST body. Fix: responses start at
        //      one billion, readers resolve response-first then client-last
        //      inside withResponse.
        static std::mutex s_httpMutex;
        static std::unordered_map<int64_t, void *> s_httpClients;
        static std::unordered_map<int64_t, void *> s_httpResponses;
        static std::unordered_map<int64_t, int64_t> s_clientLastResponse;
        static std::atomic<int64_t> s_nextClientId{1};
        static std::atomic<int64_t> s_nextResponseId{1000000001};

        static int64_t storeClient(void *client)
        {
            std::lock_guard<std::mutex> lock(s_httpMutex);
            int64_t id = s_nextClientId++;
            s_httpClients[id] = client;
            return id;
        }

        static void *getClient(int64_t id)
        {
            std::lock_guard<std::mutex> lock(s_httpMutex);
            auto it = s_httpClients.find(id);
            return (it != s_httpClients.end()) ? it->second : nullptr;
        }

        static int64_t storeResponse(void *resp)
        {
            std::lock_guard<std::mutex> lock(s_httpMutex);
            int64_t id = s_nextResponseId++;
            s_httpResponses[id] = resp;
            return id;
        }

        // (AR) قراءة استجابة تحت القفل من أولها لآخرها: نمط «حل المعرف ثم
        //      getResponse ثم استعمال المؤشر خارج القفل» كان يفتح نافذة
        //      use-after-free — طلب ثان على العميل نفسه من خيط آخر يحرر
        //      الاستجابة السابقة (recordClientResponse) بين الجلب والاستعمال
        //      (رصدته المراجعة). هنا الحل والجلب والقراءة كلها تحت القفل،
        //      والمحرران يمحوان من الخريطة تحت القفل نفسه قبل التحرير، فلا
        //      يتقاطعان مع قارئ ممسك بالمؤشر.
        // (EN) Read a response entirely under the lock: the old resolve-then-
        //      get-then-use-outside-the-lock pattern opened a use-after-free
        //      window — a second request on the same client from another
        //      thread frees the previous response between fetch and use. The
        //      freers erase from the map under this same lock before freeing,
        //      so they can never overlap a reader holding the pointer.
        template <typename Fn>
        static auto withResponse(int64_t rawId, Fn &&fn)
            -> decltype(fn(static_cast<void *>(nullptr)))
        {
            std::lock_guard<std::mutex> lock(s_httpMutex);
            int64_t responseId = rawId;
            if (s_httpResponses.find(responseId) == s_httpResponses.end())
            {
                auto clientIt = s_clientLastResponse.find(rawId);
                responseId = (clientIt != s_clientLastResponse.end())
                                 ? clientIt->second
                                 : -1;
            }
            auto it = s_httpResponses.find(responseId);
            return fn(it != s_httpResponses.end() ? it->second : nullptr);
        }

        // (AR) تسجيل آخر استجابة لعميل مع تحرير السابقة: stdlib شبكات.ص تتجاهل
        //      المعرّف الخام العائد من مدمجات الطلب، فبقاء السابقة في الخريطة
        //      تسريبٌ بلا سقف (استجابة لكل طلب في خادمٍ طويل العمر). من يريد
        //      ردّين معًا يستعمل عميلين أو معرّفي الاستجابة الخامّين قبل الطلب
        //      التالي. التحرير خارج القفل.
        // (EN) Record a client's last response and free the previous one:
        //      stdlib شبكات.ص discards the raw ids returned by the request
        //      builtins, so keeping the old response leaks one response per
        //      request on a long-lived client. Two-responses patterns must use
        //      two clients or consume the raw id before the next request.
        //      The free happens outside the lock.
        static void recordClientResponse(int64_t clientId, int64_t responseId)
        {
            void *previous = nullptr;
            {
                std::lock_guard<std::mutex> lock(s_httpMutex);
                auto lastIt = s_clientLastResponse.find(clientId);
                if (lastIt != s_clientLastResponse.end())
                {
                    auto respIt = s_httpResponses.find(lastIt->second);
                    if (respIt != s_httpResponses.end())
                    {
                        previous = respIt->second;
                        s_httpResponses.erase(respIt);
                    }
                }
                s_clientLastResponse[clientId] = responseId;
            }
            if (previous)
                sad_http_response_free(previous);
        }

        // (AR) انتزاع استجابة ذريًّا للتحرير: يحلّ المعرّف (استجابة أو عميل)
        //      ويمحوه من الخريطتين تحت قفلٍ واحد ثم يعيد المؤشر ليُحرَّر خارج
        //      القفل — يسدّ نافذة التحرير المزدوج بين خيطين يتجاهلان الرد نفسه.
        // (EN) Atomically take a response for freeing: resolve the id (response
        //      or client), erase it from both maps under a single lock, and
        //      return the pointer to be freed outside the lock — closes the
        //      double-free window between two threads discarding the same
        //      response.
        static void *takeResponse(int64_t id)
        {
            std::lock_guard<std::mutex> lock(s_httpMutex);
            int64_t responseId = id;
            auto respIt = s_httpResponses.find(responseId);
            if (respIt == s_httpResponses.end())
            {
                auto clientIt = s_clientLastResponse.find(id);
                if (clientIt == s_clientLastResponse.end())
                    return nullptr;
                responseId = clientIt->second;
                respIt = s_httpResponses.find(responseId);
                if (respIt == s_httpResponses.end())
                    return nullptr;
            }
            void *resp = respIt->second;
            s_httpResponses.erase(respIt);
            // (AR) محو مداخل «آخر استجابة» المشيرة إلى المعرّف المحرَّر كي لا
            //      يبقى مؤشرٌ خامل إليه (خطرٌ لو أعيد استخدام المعرّفات يومًا)
            // (EN) Drop last-response entries pointing at the freed id so no
            //      stale reference survives (latent risk if ids ever recycle)
            for (auto it = s_clientLastResponse.begin(); it != s_clientLastResponse.end();)
            {
                if (it->second == responseId)
                    it = s_clientLastResponse.erase(it);
                else
                    ++it;
            }
            return resp;
        }

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) نظام معالجات الطلبات (Callback System)
        // (EN) HTTP Route Handler Callback System
        //
        // (AR) التصميم:
        //   1. HandlerContext يحمل مؤشر المفسر + اسم الدالة المسجلة
        //   2. عند تسجيل مسار (عند_طلب_جلب مثلاً)، يُنشأ HandlerContext
        //      ويُمرر كـ user_data إلى C API الخاص بـ callback
        //   3. عند وصول طلب HTTP، تُنادى nativeRouteHandler وتقوم بـ:
        //      أ. تخزين مؤشري الطلب والرد في thread-local
        //      ب. استدعاء دالة ص عبر callUserFunction
        //      ج. تنظيف thread-local بعد الانتهاء
        //   4. دوال الطلب/الرد (نوع_الطلب، عيّن_نص_الرد...) تقرأ/تكتب
        //      من/إلى thread-local request/response
        //
        // (EN) Design:
        //   1. HandlerContext holds interpreter pointer + registered function name
        //   2. When registering a route, a HandlerContext is created and passed
        //      as user_data to the callback-based C API
        //   3. When an HTTP request arrives, nativeRouteHandler is called which:
        //      a. Stores request/response pointers in thread-local storage
        //      b. Calls the Sad function via callUserFunction
        //      c. Cleans up thread-local after completion
        //   4. Request/Response functions (نوع_الطلب, عيّن_نص_الرد...) read/write
        //      from/to thread-local request/response
        //
        // (AR) أمان الخيوط:
        //   - mutex على المفسر لمنع التداخل بين معالجات متزامنة
        //   - thread-local للطلب/الرد لضمان عزل كل خيط
        // ═══════════════════════════════════════════════════════════════════════

        /**
         * @brief (AR) سياق معالج الطلب — يُخزن مع كل مسار مسجل
         * @brief (EN) Handler context — stored with each registered route
         */
        struct HandlerContext
        {
            Interpreter *interpreter; ///< (AR) مؤشر المفسر / (EN) Interpreter pointer
            std::string functionName; ///< (AR) اسم الدالة المسجلة في FunctionManager / (EN) Registered function name
        };

        // (AR) تخزين آمن لسياقات المعالجات — يجب أن تعيش طوال عمر الخادم
        // (EN) Thread-safe storage for handler contexts — must outlive server lifetime
        static std::mutex s_handlerCtxMutex;
        static std::vector<std::unique_ptr<HandlerContext>> s_handlerContexts;

        // (AR) مؤشرات thread-local للطلب والرد الحاليين — تُعيَّن داخل callback
        // (EN) Thread-local request/response pointers — set inside callback
        static thread_local void *tl_currentRequest = nullptr;
        static thread_local void *tl_currentResponse = nullptr;

        // (AR) mutex لتسلسل استدعاءات المفسر من خيوط HTTP المتعددة
        // (EN) Mutex to serialize interpreter calls from multiple HTTP threads
        static std::mutex s_interpreterCallMutex;

        /**
         * @brief (AR) دالة callback ثابتة تُستدعى من C API عند وصول طلب HTTP
         * @brief (EN) Static callback invoked by C API when HTTP request arrives
         *
         * @param request  (void*) مؤشر HttpRequest من مكتبة C
         * @param response (void*) مؤشر HttpResponse من مكتبة C
         * @param user_data (void*) مؤشر HandlerContext يحمل المفسر واسم الدالة
         */
        static void nativeRouteHandler(void *request, void *response, void *user_data)
        {
            auto *ctx = static_cast<HandlerContext *>(user_data);
            if (!ctx || !ctx->interpreter)
                return;

            // (AR) تعيين thread-local قبل استدعاء المفسر — تستخدمها دوال الطلب/الرد
            // (EN) Set thread-local before interpreter call — used by request/response functions
            tl_currentRequest = request;
            tl_currentResponse = response;

            try
            {
                // (AR) قفل المفسر — استدعاء واحد في كل لحظة من خيوط HTTP
                // (EN) Lock interpreter — single call at a time from HTTP threads
                std::lock_guard<std::mutex> lock(s_interpreterCallMutex);
                ctx->interpreter->callUserFunction(ctx->functionName, {});
            }
            catch (const std::exception &e)
            {
                // (AR) في حالة خطأ — عيّن رد 500 مع رسالة الخطأ
                // (EN) On error — set 500 response with error message
                if (response)
                {
                    sad_http_response_set_status(response, 500);
                    std::string errMsg = std::string("{\"error\":\"") + e.what() + "\"}";
                    sad_http_response_set_json(response, errMsg.c_str());
                }
            }
            catch (...)
            {
                if (response)
                {
                    sad_http_response_set_status(response, 500);
                    sad_http_response_set_json(response, "{\"error\":\"خطأ داخلي غير معروف\"}");
                }
            }

            // (AR) تنظيف thread-local بعد الانتهاء
            // (EN) Clean up thread-local after completion
            tl_currentRequest = nullptr;
            tl_currentResponse = nullptr;
        }

        /**
         * @brief (AR) دالة مساعدة لتسجيل مسار — تُنشئ HandlerContext وتُسجل في C API
         * @brief (EN) Helper to register a route — creates HandlerContext and registers with C API
         *
         * @param server مؤشر الخادم
         * @param method نوع الطلب (GET/POST/PUT/DELETE)
         * @param path المسار
         * @param funcName اسم الدالة المسجلة
         * @param interpreter مؤشر المفسر
         * @return true إذا نجح التسجيل
         */
        static bool registerRoute(void *server, const std::string &method,
                                  const std::string &path, const std::string &funcName,
                                  Interpreter *interpreter)
        {
            // (AR) إنشاء سياق المعالج وتخزينه
            auto ctx = std::make_unique<HandlerContext>();
            ctx->interpreter = interpreter;
            ctx->functionName = funcName;
            HandlerContext *rawCtx = ctx.get();

            {
                std::lock_guard<std::mutex> lock(s_handlerCtxMutex);
                s_handlerContexts.push_back(std::move(ctx));
            }

            // (AR) تسجيل المسار في C API مع المعالج الأصلي
            if (method == "GET")
                sad_http_server_get_cb(server, path.c_str(), nativeRouteHandler, rawCtx);
            else if (method == "POST")
                sad_http_server_post_cb(server, path.c_str(), nativeRouteHandler, rawCtx);
            else if (method == "PUT")
                sad_http_server_put_cb(server, path.c_str(), nativeRouteHandler, rawCtx);
            else if (method == "DELETE")
                sad_http_server_delete_cb(server, path.c_str(), nativeRouteHandler, rawCtx);
            else
                return false;

            return true;
        }

#endif // HAS_NETWORK_LIB

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) تسجيل دوال HTTP في المفسر
        // (EN) Register HTTP functions in the interpreter
        // ═══════════════════════════════════════════════════════════════════════
        void registerBuiltinsHttp(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

#ifdef HAS_NETWORK_LIB

            // ═════════════════════════════════════════════════════════════════
            // القسم 1: عميل HTTP
            // ═════════════════════════════════════════════════════════════════

            // ─── أنشئ_متصفح() → معرّف العميل ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    void *client = sad_http_client_new();
                    if (!client)
                    {
                        return std::make_shared<Data::Value>(static_cast<double>(-1));
                    }
                    int64_t id = storeClient(client);
                    return std::make_shared<Data::Value>(static_cast<double>(id));
                };
                fm.registerBuiltinFunction(std::string(Bhc::NEW_CLIENT), f);
            }

            // ─── أغلق_متصفح(معرّف) → صحيح/خطأ ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());
                    // (AR) الانتزاع من الخريطة تحت القفل **قبل** التحرير خارجه
                    //      (نمط takeResponse عينه): التحرير قبل المحو كان يفتح
                    //      نافذة use-after-free لخيط ينادي اجلب بين الاثنين —
                    //      شقيق نافذة الاستجابة المسدودة (مراجعة أميليا).
                    //      ومع إغلاق العميل تحرَّر آخر استجاباته أيضًا (ع-11)
                    //      وإلا سربت استجابة واحدة لكل عميل مغلق.
                    // (EN) Claim out of the map under the lock BEFORE freeing
                    //      outside it (the takeResponse pattern): freeing before
                    //      erasing opened a use-after-free window for a thread
                    //      calling اجلب in between — the client-side sibling of
                    //      the sealed response window (Amelia review). Closing
                    //      the client also frees its last response (ع-11).
                    void *client = nullptr;
                    void *lastResp = nullptr;
                    {
                        std::lock_guard<std::mutex> lock(s_httpMutex);
                        auto clientIt = s_httpClients.find(id);
                        if (clientIt == s_httpClients.end())
                            return std::make_shared<Data::Value>(false);
                        client = clientIt->second;
                        s_httpClients.erase(clientIt);
                        auto lastIt = s_clientLastResponse.find(id);
                        if (lastIt != s_clientLastResponse.end())
                        {
                            auto respIt = s_httpResponses.find(lastIt->second);
                            if (respIt != s_httpResponses.end())
                            {
                                lastResp = respIt->second;
                                s_httpResponses.erase(respIt);
                            }
                            s_clientLastResponse.erase(lastIt);
                        }
                    }
                    sad_http_client_free(client);
                    if (lastResp)
                        sad_http_response_free(lastResp);
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bhc::FREE_CLIENT), f);
            }

            // ─── اجلب(معرّف، رابط) → معرّف استجابة ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 2)
                        return makeError("اجلب تحتاج: معرّف_متصفح، رابط");
                    int64_t clientId = static_cast<int64_t>(args[0]->toDouble());
                    std::string url = args[1]->toString();
                    void *client = getClient(clientId);
                    if (!client)
                        return makeError("متصفح غير موجود");
                    void *resp = sad_http_client_get(client, url.c_str());
                    if (!resp)
                        return std::make_shared<Data::Value>(static_cast<double>(-1));
                    int64_t respId = storeResponse(resp);
                    recordClientResponse(clientId, respId); // (AR) ع-11
                    return std::make_shared<Data::Value>(static_cast<double>(respId));
                };
                fm.registerBuiltinFunction(std::string(Bhc::GET), f);
            }

            // ─── أرسل(معرّف، رابط، محتوى) → معرّف استجابة ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 3)
                        return makeError("أرسل تحتاج: معرّف_متصفح، رابط، محتوى");
                    int64_t clientId = static_cast<int64_t>(args[0]->toDouble());
                    std::string url = args[1]->toString();
                    std::string body = args[2]->toString();
                    void *client = getClient(clientId);
                    if (!client)
                        return makeError("متصفح غير موجود");
                    void *resp = sad_http_client_post(client, url.c_str(), body.c_str());
                    if (!resp)
                        return std::make_shared<Data::Value>(static_cast<double>(-1));
                    int64_t respId = storeResponse(resp);
                    recordClientResponse(clientId, respId); // (AR) ع-11
                    return std::make_shared<Data::Value>(static_cast<double>(respId));
                };
                fm.registerBuiltinFunction(std::string(Bhc::POST), f);
            }

            // ─── استبدل(معرّف، رابط، محتوى) → معرّف استجابة ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 3)
                        return makeError("استبدل تحتاج: معرّف_متصفح، رابط، محتوى");
                    int64_t clientId = static_cast<int64_t>(args[0]->toDouble());
                    std::string url = args[1]->toString();
                    std::string body = args[2]->toString();
                    void *client = getClient(clientId);
                    if (!client)
                        return makeError("متصفح غير موجود");
                    void *resp = sad_http_client_put(client, url.c_str(), body.c_str());
                    if (!resp)
                        return std::make_shared<Data::Value>(static_cast<double>(-1));
                    int64_t respId = storeResponse(resp);
                    recordClientResponse(clientId, respId); // (AR) ع-11
                    return std::make_shared<Data::Value>(static_cast<double>(respId));
                };
                fm.registerBuiltinFunction(std::string(Bhc::PUT), f);
            }

            // ─── احذف_مورد(معرّف، رابط) → معرّف استجابة ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 2)
                        return makeError("احذف_مورد تحتاج: معرّف_متصفح، رابط");
                    int64_t clientId = static_cast<int64_t>(args[0]->toDouble());
                    std::string url = args[1]->toString();
                    void *client = getClient(clientId);
                    if (!client)
                        return makeError("متصفح غير موجود");
                    void *resp = sad_http_client_delete(client, url.c_str());
                    if (!resp)
                        return std::make_shared<Data::Value>(static_cast<double>(-1));
                    int64_t respId = storeResponse(resp);
                    recordClientResponse(clientId, respId); // (AR) ع-11
                    return std::make_shared<Data::Value>(static_cast<double>(respId));
                };
                fm.registerBuiltinFunction(std::string(Bhc::DELETE_REQ), f);
            }

            // ─── عدّل_مورد(معرّف، رابط، محتوى) → معرّف استجابة ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 3)
                        return makeError("عدّل_مورد تحتاج: معرّف_متصفح، رابط، محتوى");
                    int64_t clientId = static_cast<int64_t>(args[0]->toDouble());
                    std::string url = args[1]->toString();
                    std::string body = args[2]->toString();
                    void *client = getClient(clientId);
                    if (!client)
                        return makeError("متصفح غير موجود");
                    void *resp = sad_http_client_patch(client, url.c_str(), body.c_str());
                    if (!resp)
                        return std::make_shared<Data::Value>(static_cast<double>(-1));
                    int64_t respId = storeResponse(resp);
                    recordClientResponse(clientId, respId); // (AR) ع-11
                    return std::make_shared<Data::Value>(static_cast<double>(respId));
                };
                fm.registerBuiltinFunction(std::string(Bhc::PATCH), f);
            }

            // ─── حدد_الموقع(معرّف، رابط_أساسي) ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 2)
                        return std::make_shared<Data::Value>(false);
                    int64_t clientId = static_cast<int64_t>(args[0]->toDouble());
                    std::string baseUrl = args[1]->toString();
                    void *client = getClient(clientId);
                    if (!client)
                        return std::make_shared<Data::Value>(false);
                    sad_http_client_set_base_url(client, baseUrl.c_str());
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bhc::SET_BASE_URL), f);
            }

            // ─── أضف_ترويسة(معرّف، مفتاح، قيمة) ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 3)
                        return std::make_shared<Data::Value>(false);
                    int64_t clientId = static_cast<int64_t>(args[0]->toDouble());
                    std::string key = args[1]->toString();
                    std::string val = args[2]->toString();
                    void *client = getClient(clientId);
                    if (!client)
                        return std::make_shared<Data::Value>(false);
                    sad_http_client_set_header(client, key.c_str(), val.c_str());
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bhc::SET_HEADER), f);
            }

            // ─── حدد_الانتظار(معرّف، مللي_ثانية) ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 2)
                        return std::make_shared<Data::Value>(false);
                    int64_t clientId = static_cast<int64_t>(args[0]->toDouble());
                    int timeout = static_cast<int>(args[1]->toDouble());
                    void *client = getClient(clientId);
                    if (!client)
                        return std::make_shared<Data::Value>(false);
                    sad_http_client_set_timeout(client, timeout);
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bhc::SET_TIMEOUT), f);
            }

            // ─── سجّل_دخول_برمز(معرّف، رمز) ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 2)
                        return std::make_shared<Data::Value>(false);
                    int64_t clientId = static_cast<int64_t>(args[0]->toDouble());
                    std::string token = args[1]->toString();
                    void *client = getClient(clientId);
                    if (!client)
                        return std::make_shared<Data::Value>(false);
                    sad_http_client_set_bearer_token(client, token.c_str());
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bhc::SET_BEARER), f);
            }

            // ─── سبب_الفشل(معرّف) → نص ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>("");
                    int64_t clientId = static_cast<int64_t>(args[0]->toDouble());
                    void *client = getClient(clientId);
                    if (!client)
                        return std::make_shared<Data::Value>("متصفح غير موجود");
                    char *err = sad_http_client_last_error(client);
                    std::string result = err ? err : "";
                    if (err)
                        sad_http_free_string(err);
                    return std::make_shared<Data::Value>(result);
                };
                fm.registerBuiltinFunction(std::string(Bhc::LAST_ERROR), f);
            }

            // ─── هل_نجح(معرّف) → منطقي ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    int64_t clientId = static_cast<int64_t>(args[0]->toDouble());
                    void *client = getClient(clientId);
                    if (!client)
                        return std::make_shared<Data::Value>(false);
                    return std::make_shared<Data::Value>(sad_http_client_is_ok(client) != 0);
                };
                fm.registerBuiltinFunction(std::string(Bhc::IS_OK), f);
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 2: الاستجابة — ما يعود من الخادم
            // ═════════════════════════════════════════════════════════════════

            // ─── تجاهل_الرد(معرّف) ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    // (AR) ع-11: يقبل معرّف استجابة أو عميل — انتزاع ذري ثم تحرير خارج القفل
                    // (EN) ع-11: accepts a response or client id — atomic take, free outside lock
                    void *resp = takeResponse(static_cast<int64_t>(args[0]->toDouble()));
                    if (!resp)
                        return std::make_shared<Data::Value>(false);
                    sad_http_response_free(resp);
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bhc::RESP_FREE), f);
            }

            // ─── رمز_الحالة(معرّف) → رقم ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(static_cast<double>(-1));
                    // (AR) ع-11: يقبل معرّف استجابة أو عميل — والقراءة كلها تحت القفل (سد UAF)
                    return withResponse(
                        static_cast<int64_t>(args[0]->toDouble()),
                        [](void *resp) -> std::shared_ptr<Data::Value>
                        {
                            if (!resp)
                                return std::make_shared<Data::Value>(static_cast<double>(-1));
                            return std::make_shared<Data::Value>(
                                static_cast<double>(sad_http_response_status(resp)));
                        });
                };
                fm.registerBuiltinFunction(std::string(Bhc::RESP_STATUS), f);
            }

            // ─── نص_الرد(معرّف) → نص ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>("");
                    // (AR) ع-11: يقبل معرّف استجابة أو عميل — والقراءة كلها تحت القفل (سد UAF)
                    return withResponse(
                        static_cast<int64_t>(args[0]->toDouble()),
                        [](void *resp) -> std::shared_ptr<Data::Value>
                        {
                            if (!resp)
                                return std::make_shared<Data::Value>("");
                            char *body = sad_http_response_body(resp);
                            std::string result = body ? body : "";
                            if (body)
                                sad_http_free_string(body);
                            return std::make_shared<Data::Value>(result);
                        });
                };
                fm.registerBuiltinFunction(std::string(Bhc::RESP_BODY), f);
            }

            // ─── معلومة_الرد(معرّف، مفتاح) → نص ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 2)
                        return std::make_shared<Data::Value>("");
                    std::string key = args[1]->toString();
                    // (AR) ع-11: يقبل معرّف استجابة أو عميل — والقراءة كلها تحت القفل (سد UAF)
                    return withResponse(
                        static_cast<int64_t>(args[0]->toDouble()),
                        [&key](void *resp) -> std::shared_ptr<Data::Value>
                        {
                            if (!resp)
                                return std::make_shared<Data::Value>("");
                            char *val = sad_http_response_header(resp, key.c_str());
                            std::string result = val ? val : "";
                            if (val)
                                sad_http_free_string(val);
                            return std::make_shared<Data::Value>(result);
                        });
                };
                fm.registerBuiltinFunction(std::string(Bhc::RESP_HEADER), f);
            }

            // ─── هل_الرد_ناجح(معرّف) → منطقي ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    // (AR) ع-11: يقبل معرّف استجابة أو عميل — والقراءة كلها تحت القفل (سد UAF)
                    return withResponse(
                        static_cast<int64_t>(args[0]->toDouble()),
                        [](void *resp) -> std::shared_ptr<Data::Value>
                        {
                            if (!resp)
                                return std::make_shared<Data::Value>(false);
                            return std::make_shared<Data::Value>(
                                sad_http_response_is_success(resp) != 0);
                        });
                };
                fm.registerBuiltinFunction(std::string(Bhc::RESP_SUCCESS), f);
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 2: خادم HTTP
            // (AR) الخادم يحتاج معالجة خاصة لأن handlers تحتاج ربط بدوال ص.
            //      حالياً: الدوال الأساسية (إنشاء، استماع، إيقاف، CORS) منفذة.
            //      دوال تسجيل المسارات (عند_طلب_جلب...) تحتاج آلية callback
            //      من C إلى المفسر — ستُنفذ لاحقاً مع نظام callbacks.
            // ═════════════════════════════════════════════════════════════════

            // --- تخزين مؤشرات الخوادم ---
            static std::mutex serverMtx;
            static std::unordered_map<int, void *> serverHandles;
            static int nextServerId = 1;

            auto storeServer = [](void *s) -> int
            {
                std::lock_guard<std::mutex> lock(serverMtx);
                int id = nextServerId++;
                serverHandles[id] = s;
                return id;
            };

            auto getServer = [](int id) -> void *
            {
                std::lock_guard<std::mutex> lock(serverMtx);
                auto it = serverHandles.find(id);
                return (it != serverHandles.end()) ? it->second : nullptr;
            };

            auto removeServer = [](int id) -> void
            {
                std::lock_guard<std::mutex> lock(serverMtx);
                serverHandles.erase(id);
            };

            // ─── أنشئ_خادم(منفذ) → رقم (معرّف الخادم) ───
            {
                auto f = [&storeServer](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(std::string("خطأ: يجب تحديد رقم المنفذ"));
                    int port = static_cast<int>(args[0]->toDouble());
                    void *server = sad_http_server_new(port);
                    if (!server)
                        return std::make_shared<Data::Value>(-1.0);
                    return std::make_shared<Data::Value>(static_cast<double>(storeServer(server)));
                };
                fm.registerBuiltinFunction(std::string(Bhs::NEW_SERVER), f);
            }

            // ─── أزل_خادم(معرّف) ───
            {
                auto f = [&getServer, &removeServer](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    int id = static_cast<int>(args[0]->toDouble());
                    void *server = getServer(id);
                    if (server)
                    {
                        sad_http_server_free(server);
                        removeServer(id);
                    }
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bhs::FREE_SERVER), f);
            }

            // ─── ابدأ_الاستماع(معرّف، [منفذ]) — المنفذ الاختياري يتغلب على منفذ الإنشاء ───
            {
                auto f = [&getServer](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    int id = static_cast<int>(args[0]->toDouble());
                    void *server = getServer(id);
                    if (!server)
                        return std::make_shared<Data::Value>(false);
                    // (AR) ع-10: المنفذ الثاني كان يُتجاهل صامتًا فيستمع الخادم على
                    //      منفذ الإنشاء (وغالبًا 0) — الآن يُكرَّم إن مُرِّر موجبًا،
                    //      وهو ما تعتمده stdlib شبكات.ص في «خادم_ويب.ابدأ(منفذ)».
                    // (EN) ع-10: the optional second port argument was silently
                    //      ignored (server kept the creation-time port, often 0) —
                    //      now honored when positive; stdlib شبكات.ص relies on it.
                    if (args.size() >= 2)
                    {
                        int explicitPort = static_cast<int>(args[1]->toDouble());
                        if (explicitPort > 0)
                        {
                            // (AR) نجاح صادق: 0 عند خادم عامل مسبقًا أو فشل الربط
                            // (EN) Honest success: 0 for already-running or bind failure
                            int ok = sad_http_server_listen_on(server, explicitPort);
                            return std::make_shared<Data::Value>(ok != 0);
                        }
                    }
                    sad_http_server_listen(server);
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bhs::LISTEN), f);
            }

            // ─── أوقف_الخادم(معرّف) ───
            {
                auto f = [&getServer](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    int id = static_cast<int>(args[0]->toDouble());
                    void *server = getServer(id);
                    if (!server)
                        return std::make_shared<Data::Value>(false);
                    sad_http_server_stop(server);
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bhs::STOP), f);
            }

            // ─── اسمح_بالوصول_الخارجي(معرّف، أصل) ───
            {
                auto f = [&getServer](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 2)
                        return std::make_shared<Data::Value>(false);
                    int id = static_cast<int>(args[0]->toDouble());
                    std::string origin = args[1]->toString();
                    void *server = getServer(id);
                    if (!server)
                        return std::make_shared<Data::Value>(false);
                    sad_http_server_enable_cors(server, origin.c_str());
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bhs::ENABLE_CORS), f);
            }

            // (AR) دوال تسجيل المسارات — عند_طلب_جلب، عند_طلب_إرسال، عند_طلب_استبدال، عند_طلب_حذف
            //      تستخدم نظام callbacks: تُنشئ HandlerContext وتُسجل مع C API
            //      المعاملات: (معرّف_خادم، مسار، دالة_معالجة)
            //      دالة_معالجة: إما اسم دالة (نص) أو مرجع دالة/لامدا (FUNCTION)
            // ═══════════════════════════════════════════════════════════════════

            // (AR) مولّد دالة تسجيل مسار — لتقليل التكرار
            auto makeRouteRegistrar = [&interpreter, &getServer](const std::string &httpMethod)
            {
                return [&interpreter, &getServer, httpMethod](
                           Sad::Interpreter::BuiltinContext &ctx)
                           -> std::shared_ptr<Data::Value>
                {
                    const auto &args = ctx.args(); (void)args;
                    // (AR) التحقق من المعاملات: (معرّف_خادم، مسار، معالج)
                    if (args.size() < 3)
                        return makeError("تحتاج 3 معاملات: (معرّف_خادم، مسار، دالة_معالجة)");

                    int serverId = static_cast<int>(args[0]->toDouble());
                    std::string path = args[1]->toString();

                    // (AR) استخراج اسم الدالة — يعمل مع FUNCTION (registeredName) و STRING
                    std::string funcName = args[2]->getFunctionName();
                    if (funcName.empty())
                        return makeError("المعامل الثالث يجب أن يكون دالة أو اسم دالة");

                    void *server = getServer(serverId);
                    if (!server)
                        return makeError("معرّف الخادم غير صالح");

                    bool ok = registerRoute(server, httpMethod, path, funcName, &interpreter);
                    return std::make_shared<Data::Value>(ok);
                };
            };

            fm.registerBuiltinFunction(std::string(Bhs::ON_GET), makeRouteRegistrar("GET"));
            fm.registerBuiltinFunction(std::string(Bhs::ON_POST), makeRouteRegistrar("POST"));
            fm.registerBuiltinFunction(std::string(Bhs::ON_PUT), makeRouteRegistrar("PUT"));
            fm.registerBuiltinFunction(std::string(Bhs::ON_DELETE), makeRouteRegistrar("DELETE"));

            // ═══════════════════════════════════════════════════════════════════
            // (AR) دوال قراءة الطلب — تقرأ من thread-local request الحالي
            //      تعمل فقط داخل معالج طلب (عندما tl_currentRequest != nullptr)
            // ═══════════════════════════════════════════════════════════════════

            // ─── نوع_الطلب() → نص (GET/POST/PUT/DELETE/...) ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    if (!tl_currentRequest)
                        return makeError("نوع_الطلب متاحة فقط داخل معالج طلب");
                    char *method = sad_http_request_method(tl_currentRequest);
                    std::string result = method ? std::string(method) : "";
                    if (method)
                        sad_http_free_string(method);
                    return std::make_shared<Data::Value>(result);
                };
                fm.registerBuiltinFunction(std::string(Bhs::REQ_METHOD), f);
            }

            // ─── وجهة_الطلب() → نص (المسار) ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    if (!tl_currentRequest)
                        return makeError("وجهة_الطلب متاحة فقط داخل معالج طلب");
                    char *path = sad_http_request_path(tl_currentRequest);
                    std::string result = path ? std::string(path) : "";
                    if (path)
                        sad_http_free_string(path);
                    return std::make_shared<Data::Value>(result);
                };
                fm.registerBuiltinFunction(std::string(Bhs::REQ_PATH), f);
            }

            // ─── بيانات_الطلب() → نص (جسم الطلب) ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    if (!tl_currentRequest)
                        return makeError("بيانات_الطلب متاحة فقط داخل معالج طلب");
                    char *body = sad_http_request_body(tl_currentRequest);
                    std::string result = body ? std::string(body) : "";
                    if (body)
                        sad_http_free_string(body);
                    return std::make_shared<Data::Value>(result);
                };
                fm.registerBuiltinFunction(std::string(Bhs::REQ_BODY), f);
            }

            // ─── معلومة_الطلب(اسم_الترويسة) → نص (قيمة الترويسة) ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (!tl_currentRequest)
                        return makeError("معلومة_الطلب متاحة فقط داخل معالج طلب");
                    if (args.empty())
                        return makeError("معلومة_الطلب تحتاج اسم الترويسة");
                    std::string key = args[0]->toString();
                    char *val = sad_http_request_header(tl_currentRequest, key.c_str());
                    std::string result = val ? std::string(val) : "";
                    if (val)
                        sad_http_free_string(val);
                    return std::make_shared<Data::Value>(result);
                };
                fm.registerBuiltinFunction(std::string(Bhs::REQ_HEADER), f);
            }

            // ─── قيمة_من_الرابط(اسم_المعامل) → نص (قيمة query param) ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (!tl_currentRequest)
                        return makeError("قيمة_من_الرابط متاحة فقط داخل معالج طلب");
                    if (args.empty())
                        return makeError("قيمة_من_الرابط تحتاج اسم المعامل");
                    std::string key = args[0]->toString();
                    char *val = sad_http_request_query_param(tl_currentRequest, key.c_str());
                    std::string result = val ? std::string(val) : "";
                    if (val)
                        sad_http_free_string(val);
                    return std::make_shared<Data::Value>(result);
                };
                fm.registerBuiltinFunction(std::string(Bhs::REQ_QUERY), f);
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) دوال كتابة الرد — تكتب إلى thread-local response الحالي
            //      تعمل فقط داخل معالج طلب (عندما tl_currentResponse != nullptr)
            // ═══════════════════════════════════════════════════════════════════

            // ─── عيّن_حالة_الرد(رمز) → منطقي ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (!tl_currentResponse)
                        return makeError("عيّن_حالة_الرد متاحة فقط داخل معالج طلب");
                    if (args.empty())
                        return makeError("عيّن_حالة_الرد تحتاج رمز الحالة (مثال: 200)");
                    int status = static_cast<int>(args[0]->toDouble());
                    sad_http_response_set_status(tl_currentResponse, status);
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bhs::RESP_SET_STATUS), f);
            }

            // ─── عيّن_نص_الرد(نص) → منطقي ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (!tl_currentResponse)
                        return makeError("عيّن_نص_الرد متاحة فقط داخل معالج طلب");
                    if (args.empty())
                        return makeError("عيّن_نص_الرد تحتاج نص الرد");
                    std::string body = args[0]->toString();
                    sad_http_response_set_body(tl_currentResponse, body.c_str());
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bhs::RESP_SET_BODY), f);
            }

            // ─── عيّن_رد_جيسون(نص_جيسون) → منطقي ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (!tl_currentResponse)
                        return makeError("عيّن_رد_جيسون متاحة فقط داخل معالج طلب");
                    if (args.empty())
                        return makeError("عيّن_رد_جيسون تحتاج نص JSON");
                    std::string json = args[0]->toString();
                    sad_http_response_set_json(tl_currentResponse, json.c_str());
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bhs::RESP_SET_JSON), f);
            }

            // ─── عيّن_رد_صفحة(نص_html) → منطقي ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (!tl_currentResponse)
                        return makeError("عيّن_رد_صفحة متاحة فقط داخل معالج طلب");
                    if (args.empty())
                        return makeError("عيّن_رد_صفحة تحتاج نص HTML");
                    std::string html = args[0]->toString();
                    sad_http_response_set_html(tl_currentResponse, html.c_str());
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bhs::RESP_SET_HTML), f);
            }

            // ─── عيّن_ترويسة_الرد(اسم، قيمة) → منطقي ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (!tl_currentResponse)
                        return makeError("عيّن_ترويسة_الرد متاحة فقط داخل معالج طلب");
                    if (args.size() < 2)
                        return makeError("عيّن_ترويسة_الرد تحتاج (اسم، قيمة)");
                    std::string key = args[0]->toString();
                    std::string val = args[1]->toString();
                    sad_http_response_set_header(tl_currentResponse, key.c_str(), val.c_str());
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bhs::RESP_SET_HEADER), f);
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 3: أدوات الشبكة العامة
            // ═════════════════════════════════════════════════════════════════

            // ─── جهّز_الشبكة() → منطقي ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    return std::make_shared<Data::Value>(sad_network_init());
                };
                fm.registerBuiltinFunction(std::string(Bnu::INIT), f);
            }

            // ─── نظّف_الشبكة() ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    sad_network_cleanup();
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bnu::CLEANUP), f);
            }

            // ─── رمز_آخر_خطأ() → رقم ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    return std::make_shared<Data::Value>(static_cast<double>(sad_network_get_last_error()));
                };
                fm.registerBuiltinFunction(std::string(Bnu::LAST_ERROR_CODE), f);
            }

            // ─── وصف_آخر_خطأ() → نص ───
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    const char *msg = sad_network_get_last_error_message();
                    return std::make_shared<Data::Value>(msg ? std::string(msg) : "");
                };
                fm.registerBuiltinFunction(std::string(Bnu::LAST_ERROR_MSG), f);
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 4: دوال العناوين
            // (AR) تغليف واجهة sad_socket_address_* من مكتبة C
            // ═════════════════════════════════════════════════════════════════

            // --- تخزين مؤشرات العناوين ---
            static std::mutex addrMtx;
            static std::unordered_map<int, void *> addrHandles;
            static int nextAddrId = 1;

            auto storeAddr = [](void *a) -> int
            {
                std::lock_guard<std::mutex> lock(addrMtx);
                int id = nextAddrId++;
                addrHandles[id] = a;
                return id;
            };

            auto getAddr = [](int id) -> void *
            {
                std::lock_guard<std::mutex> lock(addrMtx);
                auto it = addrHandles.find(id);
                return (it != addrHandles.end()) ? it->second : nullptr;
            };

            auto removeAddr = [](int id) -> void
            {
                std::lock_guard<std::mutex> lock(addrMtx);
                addrHandles.erase(id);
            };

            // ─── عنوان(ip، منفذ) → رقم (معرّف) ───
            {
                auto f = [&storeAddr](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 2)
                        return std::make_shared<Data::Value>(std::string("خطأ: عنوان() يحتاج (ip، منفذ)"));
                    std::string ip = args[0]->toString();
                    int port = static_cast<int>(args[1]->toDouble());
                    void *addr = sad_socket_address_create(ip.c_str(), static_cast<uint16_t>(port));
                    if (!addr)
                        return std::make_shared<Data::Value>(-1.0);
                    return std::make_shared<Data::Value>(static_cast<double>(storeAddr(addr)));
                };
                fm.registerBuiltinFunction(std::string(Bnu::ADDR_NEW), f);
            }

            // ─── عنوان_حديث(ip، منفذ) → رقم (معرّف) ───
            {
                auto f = [&storeAddr](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 2)
                        return std::make_shared<Data::Value>(std::string("خطأ: عنوان_حديث() يحتاج (ip، منفذ)"));
                    std::string ip = args[0]->toString();
                    int port = static_cast<int>(args[1]->toDouble());
                    void *addr = sad_socket_address_create_v6(ip.c_str(), static_cast<uint16_t>(port));
                    if (!addr)
                        return std::make_shared<Data::Value>(-1.0);
                    return std::make_shared<Data::Value>(static_cast<double>(storeAddr(addr)));
                };
                fm.registerBuiltinFunction(std::string(Bnu::ADDR_NEW_V6), f);
            }

            // ─── حرر_عنوان(معرّف) → منطقي ───
            {
                auto f = [&getAddr, &removeAddr](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    int id = static_cast<int>(args[0]->toDouble());
                    void *addr = getAddr(id);
                    if (addr)
                    {
                        sad_socket_address_delete(addr);
                        removeAddr(id);
                    }
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bnu::ADDR_FREE), f);
            }

            // ─── رقم_الجهاز(معرّف) → نص ───
            {
                auto f = [&getAddr](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(std::string(""));
                    int id = static_cast<int>(args[0]->toDouble());
                    void *addr = getAddr(id);
                    if (!addr)
                        return std::make_shared<Data::Value>(std::string(""));
                    const char *ip = sad_socket_address_get_ip(addr);
                    return std::make_shared<Data::Value>(ip ? std::string(ip) : std::string(""));
                };
                fm.registerBuiltinFunction(std::string(Bnu::ADDR_IP), f);
            }

            // ─── رقم_المنفذ(معرّف) → رقم ───
            {
                auto f = [&getAddr](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(0.0);
                    int id = static_cast<int>(args[0]->toDouble());
                    void *addr = getAddr(id);
                    if (!addr)
                        return std::make_shared<Data::Value>(0.0);
                    return std::make_shared<Data::Value>(static_cast<double>(sad_socket_address_get_port(addr)));
                };
                fm.registerBuiltinFunction(std::string(Bnu::ADDR_PORT), f);
            }

            // ─── هل_عنوان_قديم(معرّف) → منطقي ───
            {
                auto f = [&getAddr](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    int id = static_cast<int>(args[0]->toDouble());
                    void *addr = getAddr(id);
                    if (!addr)
                        return std::make_shared<Data::Value>(false);
                    return std::make_shared<Data::Value>(sad_socket_address_is_ipv4(addr));
                };
                fm.registerBuiltinFunction(std::string(Bnu::ADDR_IS_V4), f);
            }

            // ─── هل_عنوان_حديث(معرّف) → منطقي ───
            {
                auto f = [&getAddr](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    int id = static_cast<int>(args[0]->toDouble());
                    void *addr = getAddr(id);
                    if (!addr)
                        return std::make_shared<Data::Value>(false);
                    return std::make_shared<Data::Value>(sad_socket_address_is_ipv6(addr));
                };
                fm.registerBuiltinFunction(std::string(Bnu::ADDR_IS_V6), f);
            }

            // ─── العنوان_كنص(معرّف) → نص ───
            {
                auto f = [&getAddr](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(std::string(""));
                    int id = static_cast<int>(args[0]->toDouble());
                    void *addr = getAddr(id);
                    if (!addr)
                        return std::make_shared<Data::Value>(std::string(""));
                    const char *str = sad_socket_address_to_string(addr);
                    return std::make_shared<Data::Value>(str ? std::string(str) : std::string(""));
                };
                fm.registerBuiltinFunction(std::string(Bnu::ADDR_STR), f);
            }

#else
            // ═══════════════════════════════════════════════════════════════
            // (AR) بدون مكتبة الشبكة — تسجيل دوال وهمية ترجع خطأ واضح
            // (EN) Without network lib — register stubs that return clear errors
            // ═══════════════════════════════════════════════════════════════

            auto stubFn = [](const std::string &name)
            {
                return [name](Sad::Interpreter::BuiltinContext &)
                           -> std::shared_ptr<Data::Value>
                {
                    return std::make_shared<Data::Value>(
                        "خطأ: الدالة " + name + " تتطلب مكتبة الشبكة (HAS_NETWORK_LIB)");
                };
            };

            // (AR) عميل HTTP — دوال وهمية
            fm.registerBuiltinFunction(std::string(Bhc::NEW_CLIENT), stubFn("أنشئ_متصفح"));
            fm.registerBuiltinFunction(std::string(Bhc::FREE_CLIENT), stubFn("أغلق_متصفح"));
            fm.registerBuiltinFunction(std::string(Bhc::GET), stubFn("اجلب"));
            fm.registerBuiltinFunction(std::string(Bhc::POST), stubFn("أرسل"));
            fm.registerBuiltinFunction(std::string(Bhc::PUT), stubFn("استبدل"));
            fm.registerBuiltinFunction(std::string(Bhc::DELETE_REQ), stubFn("احذف_مورد"));
            fm.registerBuiltinFunction(std::string(Bhc::PATCH), stubFn("عدّل_مورد"));
            fm.registerBuiltinFunction(std::string(Bhc::SET_BASE_URL), stubFn("حدد_الموقع"));
            fm.registerBuiltinFunction(std::string(Bhc::SET_HEADER), stubFn("أضف_ترويسة"));
            fm.registerBuiltinFunction(std::string(Bhc::SET_TIMEOUT), stubFn("حدد_الانتظار"));
            fm.registerBuiltinFunction(std::string(Bhc::SET_BEARER), stubFn("سجّل_دخول_برمز"));
            fm.registerBuiltinFunction(std::string(Bhc::LAST_ERROR), stubFn("سبب_الفشل"));
            fm.registerBuiltinFunction(std::string(Bhc::IS_OK), stubFn("هل_نجح"));
            // (AR) الاستجابة — دوال وهمية
            fm.registerBuiltinFunction(std::string(Bhc::RESP_FREE), stubFn("تجاهل_الرد"));
            fm.registerBuiltinFunction(std::string(Bhc::RESP_STATUS), stubFn("رمز_الحالة"));
            fm.registerBuiltinFunction(std::string(Bhc::RESP_BODY), stubFn("نص_الرد"));
            fm.registerBuiltinFunction(std::string(Bhc::RESP_HEADER), stubFn("معلومة_الرد"));
            fm.registerBuiltinFunction(std::string(Bhc::RESP_SUCCESS), stubFn("هل_الرد_ناجح"));
            // (AR) أدوات الشبكة — دوال وهمية
            fm.registerBuiltinFunction(std::string(Bnu::INIT), stubFn("جهّز_الشبكة"));
            fm.registerBuiltinFunction(std::string(Bnu::CLEANUP), stubFn("نظّف_الشبكة"));
            fm.registerBuiltinFunction(std::string(Bnu::LAST_ERROR_CODE), stubFn("رمز_آخر_خطأ"));
            fm.registerBuiltinFunction(std::string(Bnu::LAST_ERROR_MSG), stubFn("وصف_آخر_خطأ"));
            // (AR) خادم HTTP — دوال وهمية
            fm.registerBuiltinFunction(std::string(Bhs::NEW_SERVER), stubFn("أنشئ_خادم"));
            fm.registerBuiltinFunction(std::string(Bhs::FREE_SERVER), stubFn("أزل_خادم"));
            fm.registerBuiltinFunction(std::string(Bhs::ON_GET), stubFn("عند_طلب_جلب"));
            fm.registerBuiltinFunction(std::string(Bhs::ON_POST), stubFn("عند_طلب_إرسال"));
            fm.registerBuiltinFunction(std::string(Bhs::ON_PUT), stubFn("عند_طلب_استبدال"));
            fm.registerBuiltinFunction(std::string(Bhs::ON_DELETE), stubFn("عند_طلب_حذف"));
            fm.registerBuiltinFunction(std::string(Bhs::LISTEN), stubFn("ابدأ_الاستماع"));
            fm.registerBuiltinFunction(std::string(Bhs::STOP), stubFn("أوقف_الخادم"));
            fm.registerBuiltinFunction(std::string(Bhs::ENABLE_CORS), stubFn("اسمح_بالوصول_الخارجي"));
            fm.registerBuiltinFunction(std::string(Bhs::REQ_METHOD), stubFn("نوع_الطلب"));
            fm.registerBuiltinFunction(std::string(Bhs::REQ_PATH), stubFn("وجهة_الطلب"));
            fm.registerBuiltinFunction(std::string(Bhs::REQ_BODY), stubFn("بيانات_الطلب"));
            fm.registerBuiltinFunction(std::string(Bhs::REQ_HEADER), stubFn("معلومة_الطلب"));
            fm.registerBuiltinFunction(std::string(Bhs::REQ_QUERY), stubFn("قيمة_من_الرابط"));
            fm.registerBuiltinFunction(std::string(Bhs::RESP_SET_STATUS), stubFn("عيّن_حالة_الرد"));
            fm.registerBuiltinFunction(std::string(Bhs::RESP_SET_BODY), stubFn("عيّن_نص_الرد"));
            fm.registerBuiltinFunction(std::string(Bhs::RESP_SET_JSON), stubFn("عيّن_رد_جيسون"));
            fm.registerBuiltinFunction(std::string(Bhs::RESP_SET_HTML), stubFn("عيّن_رد_صفحة"));
            fm.registerBuiltinFunction(std::string(Bhs::RESP_SET_HEADER), stubFn("عيّن_ترويسة_الرد"));
            // (AR) عناوين الشبكة — دوال وهمية
            fm.registerBuiltinFunction(std::string(Bnu::ADDR_NEW), stubFn("عنوان"));
            fm.registerBuiltinFunction(std::string(Bnu::ADDR_NEW_V6), stubFn("عنوان_حديث"));
            fm.registerBuiltinFunction(std::string(Bnu::ADDR_FREE), stubFn("حرر_عنوان"));
            fm.registerBuiltinFunction(std::string(Bnu::ADDR_IP), stubFn("رقم_الجهاز"));
            fm.registerBuiltinFunction(std::string(Bnu::ADDR_PORT), stubFn("رقم_المنفذ"));
            fm.registerBuiltinFunction(std::string(Bnu::ADDR_IS_V4), stubFn("هل_عنوان_قديم"));
            fm.registerBuiltinFunction(std::string(Bnu::ADDR_IS_V6), stubFn("هل_عنوان_حديث"));
            fm.registerBuiltinFunction(std::string(Bnu::ADDR_STR), stubFn("العنوان_كنص"));

#endif // HAS_NETWORK_LIB
        }

    } // namespace Interpreter
} // namespace Sad
