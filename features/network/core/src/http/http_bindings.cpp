// بسم الله الرحمن الرحيم
// HTTP Bindings Implementation
// تنفيذ روابط HTTP

#include "http_bindings.h"
#include "http_client.h"
#include "http_server.h"
#include "http_request.h"
#include "http_response.h"
#include <cstring>
#include <memory>

using namespace sad::network::http;

// ==========================================
// Helper Functions
// ==========================================

/**
 * @brief تحويل std::string إلى C-string مع تخصيص ذاكرة
 * @note يجب على المستدعي تحرير الذاكرة باستخدام sad_free_string
 */
static char *string_to_c_str(const std::string &str)
{
    size_t len = str.length() + 1;
    char *result = new (std::nothrow) char[len];
    if (!result)
    {
        return nullptr; // فشل التخصيص
    }
    std::strncpy(result, str.c_str(), len);
    result[len - 1] = '\0'; // ضمان إنهاء النص
    return result;
}

/**
 * @brief تحرير الذاكرة المخصصة بواسطة string_to_c_str
 */
extern "C" void sad_free_string(char *str)
{
    delete[] str;
}

// ==========================================
// HTTP Client Implementation
// ==========================================

void *sad_http_client_new()
{
    try
    {
        return new HttpClient();
    }
    catch (...)
    {
        return nullptr;
    }
}

void sad_http_client_free(void *client)
{
    if (client)
    {
        delete static_cast<HttpClient *>(client);
    }
}

void *sad_http_client_get(void *client, const char *url)
{
    if (!client || !url)
        return nullptr;

    try
    {
        HttpClient *http_client = static_cast<HttpClient *>(client);
        HttpResponse response = http_client->get(url);
        return new HttpResponse(std::move(response));
    }
    catch (...)
    {
        return nullptr;
    }
}

void *sad_http_client_post(void *client, const char *url, const char *body)
{
    if (!client || !url)
        return nullptr;

    try
    {
        HttpClient *http_client = static_cast<HttpClient *>(client);
        std::string body_str = body ? body : "";
        HttpResponse response = http_client->post(url, body_str);
        return new HttpResponse(std::move(response));
    }
    catch (...)
    {
        return nullptr;
    }
}

void *sad_http_client_put(void *client, const char *url, const char *body)
{
    if (!client || !url)
        return nullptr;

    try
    {
        HttpClient *http_client = static_cast<HttpClient *>(client);
        std::string body_str = body ? body : "";
        HttpResponse response = http_client->put(url, body_str);
        return new HttpResponse(std::move(response));
    }
    catch (...)
    {
        return nullptr;
    }
}

void *sad_http_client_delete(void *client, const char *url)
{
    if (!client || !url)
        return nullptr;

    try
    {
        HttpClient *http_client = static_cast<HttpClient *>(client);
        HttpResponse response = http_client->delete_(url);
        return new HttpResponse(std::move(response));
    }
    catch (...)
    {
        return nullptr;
    }
}

void *sad_http_client_patch(void *client, const char *url, const char *body)
{
    if (!client || !url)
        return nullptr;

    try
    {
        HttpClient *http_client = static_cast<HttpClient *>(client);
        std::string body_str = body ? body : "";
        HttpResponse response = http_client->patch(url, body_str);
        return new HttpResponse(std::move(response));
    }
    catch (...)
    {
        return nullptr;
    }
}

void sad_http_client_set_base_url(void *client, const char *base_url)
{
    if (!client || !base_url)
        return;

    try
    {
        HttpClient *http_client = static_cast<HttpClient *>(client);
        http_client->set_base_url(base_url);
    }
    catch (...)
    {
    }
}

void sad_http_client_set_header(void *client, const char *key, const char *value)
{
    if (!client || !key || !value)
        return;

    try
    {
        HttpClient *http_client = static_cast<HttpClient *>(client);
        http_client->set_default_header(key, value);
    }
    catch (...)
    {
    }
}

void sad_http_client_set_timeout(void *client, int timeout_ms)
{
    if (!client)
        return;

    try
    {
        HttpClient *http_client = static_cast<HttpClient *>(client);
        http_client->set_timeout(timeout_ms);
    }
    catch (...)
    {
    }
}

void sad_http_client_set_bearer_token(void *client, const char *token)
{
    if (!client || !token)
        return;

    try
    {
        HttpClient *http_client = static_cast<HttpClient *>(client);
        http_client->set_bearer_token(token);
    }
    catch (...)
    {
    }
}

char *sad_http_client_last_error(void *client)
{
    if (!client)
        return nullptr;

    try
    {
        HttpClient *http_client = static_cast<HttpClient *>(client);
        return string_to_c_str(http_client->last_error());
    }
    catch (...)
    {
        return nullptr;
    }
}

int sad_http_client_is_ok(void *client)
{
    if (!client)
        return 0;

    try
    {
        HttpClient *http_client = static_cast<HttpClient *>(client);
        return http_client->is_ok() ? 1 : 0;
    }
    catch (...)
    {
        return 0;
    }
}

// ==========================================
// HTTP Response Implementation
// ==========================================

void sad_http_response_free(void *response)
{
    if (response)
    {
        delete static_cast<HttpResponse *>(response);
    }
}

int sad_http_response_status(void *response)
{
    if (!response)
        return 0;

    try
    {
        HttpResponse *http_response = static_cast<HttpResponse *>(response);
        return static_cast<int>(http_response->status());
    }
    catch (...)
    {
        return 0;
    }
}

char *sad_http_response_body(void *response)
{
    if (!response)
        return nullptr;

    try
    {
        HttpResponse *http_response = static_cast<HttpResponse *>(response);
        return string_to_c_str(http_response->body());
    }
    catch (...)
    {
        return nullptr;
    }
}

char *sad_http_response_header(void *response, const char *key)
{
    if (!response || !key)
        return nullptr;

    try
    {
        HttpResponse *http_response = static_cast<HttpResponse *>(response);
        return string_to_c_str(http_response->header(key));
    }
    catch (...)
    {
        return nullptr;
    }
}

int sad_http_response_is_success(void *response)
{
    if (!response)
        return 0;

    try
    {
        HttpResponse *http_response = static_cast<HttpResponse *>(response);
        return http_response->is_success() ? 1 : 0;
    }
    catch (...)
    {
        return 0;
    }
}

// ==========================================
// HTTP Server Implementation
// ==========================================

// Handler wrapper to convert C function pointer to C++ lambda
struct HandlerWrapper
{
    void *sad_handler;

    void operator()(const HttpRequest &request, HttpResponse &response)
    {
        // This would need integration with Sad's function call mechanism
        // For now, this is a placeholder
    }
};

void *sad_http_server_new(int port)
{
    try
    {
        return new HttpServer(port);
    }
    catch (...)
    {
        return nullptr;
    }
}

void sad_http_server_free(void *server)
{
    if (server)
    {
        delete static_cast<HttpServer *>(server);
    }
}

// (AR) حُذفت sad_http_server_{get,post,put,delete} — مسارٌ ميت كان يسجّل معالجاً
//      يردّ نصّاً ثابتاً ويُهمل معامل handler كلّه. المسار الحيّ هو نظائر _cb
//      التي يستعملها المفسّر حصراً (builtin_module_http.cpp).
// (EN) Removed sad_http_server_{get,post,put,delete} — a dead path that registered a
//      handler replying with a fixed string and ignored the handler parameter entirely.
//      The live path is the _cb variants, used exclusively by the interpreter.

void sad_http_server_listen(void *server)
{
    if (!server)
        return;

    try
    {
        HttpServer *http_server = static_cast<HttpServer *>(server);
        http_server->listen();
    }
    catch (...)
    {
    }
}

int sad_http_server_listen_on(void *server, int port)
{
    if (!server)
        return 0;

    try
    {
        HttpServer *http_server = static_cast<HttpServer *>(server);
        // (AR) خادمٌ عاملٌ مسبقًا: listen تعود فورًا دون تكريم المنفذ — فشل صريح
        //      لا «نجاح» كاذب. والاستثناء الهارب (فشل bind مثلًا) فشلٌ كذلك.
        // (EN) Already-running server: listen returns immediately without
        //      honoring the port — report failure, not a false success. An
        //      escaping exception (e.g. bind failure) is a failure too.
        if (http_server->is_running())
            return 0;
        // (AR) توحيد دلالة المنفذ ≤ 0 هنا (طبقة واحدة للمحركين): غير الموجب
        //      يسقط إلى منفذ الإنشاء بدل محاولة ربط منفذ 0 — يطابق حارس
        //      المفسر فلا يتباعد المصرف عنه.
        // (EN) Port ≤ 0 semantics unified HERE (one layer for both engines):
        //      non-positive falls back to the creation-time port instead of
        //      binding port 0 — matches the interpreter's guard so the
        //      compiled path cannot diverge.
        if (port > 0)
            http_server->listen(port);
        else
            http_server->listen();
        return 1;
    }
    catch (...)
    {
        return 0;
    }
}

void sad_http_server_stop(void *server)
{
    if (!server)
        return;

    try
    {
        HttpServer *http_server = static_cast<HttpServer *>(server);
        http_server->stop();
    }
    catch (...)
    {
    }
}

void sad_http_server_enable_cors(void *server, const char *origin)
{
    if (!server || !origin)
        return;

    try
    {
        HttpServer *http_server = static_cast<HttpServer *>(server);
        http_server->enable_cors(origin);
    }
    catch (...)
    {
    }
}

// ==========================================
// (AR) ع-14: سياق المعالج للمسار المصرَّف — thread-local للطلب/الرد الحاليين.
//      المفسر يمرر مؤشري الطلب/الرد عبر thread-local خاص به داخل
//      builtin_module_http.cpp، أما التنفيذي المصرف فمعالجه دالة ص بلا
//      معاملات تنادي دوال الطلب/الرد بلا سياق — فيوفر هذا الزوج السياق،
//      ودوال الطلب/الرد أدناه تسقط إليه عند تمرير nullptr.
// (EN) ع-14: handler context for the COMPILED path — thread-local current
//      request/response. The interpreter keeps its own thread-locals in
//      builtin_module_http.cpp; a compiled Sad handler is a zero-parameter
//      function calling the request/response functions with no context, so
//      this pair supplies it and the functions below fall back to it when
//      passed nullptr.
static thread_local void *t_currentRequest = nullptr;
static thread_local void *t_currentResponse = nullptr;

void *sad_http_current_request(void)
{
    return t_currentRequest;
}

void *sad_http_current_response(void)
{
    return t_currentResponse;
}

// (AR) ع-14: ترامبولين المسار المصرف — user_data هو مؤشر دالة ص المصرفة
//      (بلا معاملات). يثبت السياق في thread-local ثم يناديها ثم يمسحه.
// (EN) ع-14: compiled-route trampoline — user_data is the compiled Sad
//      handler (zero-parameter function pointer). Pins the context in the
//      thread-locals, calls it, clears them.
namespace
{
    // (AR) حارس RAII لسياق المسار: يحفظ السابق ويستعيده فتصمد الاستعادة أمام
    //      أي مسار خروج ولا تفقد إعادة الدخول المتداخلة سياق الخارج.
    // (EN) RAII route-context guard: saves and restores the previous values so
    //      restoration survives every exit path and nested re-entry keeps the
    //      outer context.
    struct RouteContextGuard
    {
        void *previousRequest;
        void *previousResponse;

        RouteContextGuard(void *request, void *response)
            : previousRequest(t_currentRequest), previousResponse(t_currentResponse)
        {
            t_currentRequest = request;
            t_currentResponse = response;
        }

        ~RouteContextGuard()
        {
            t_currentRequest = previousRequest;
            t_currentResponse = previousResponse;
        }
    };
} // namespace

void sad_http_route_trampoline(void *request, void *response, void *user_data)
{
    if (!user_data)
        return;
    RouteContextGuard contextGuard(request, response);
    // (AR) حد الالتقاط: catch(...) يلتقط استثناءات C++ فقط لا SEH (انتهاك
    //      وصول في الكود المصرف يظل قاتلا)، واستثناء C++ يرمى من مساعد وقت
    //      تشغيل عبر إطارات IR بلا جداول unwind على Win64 غير معرف السلوك
    //      قبل بلوغ الالتقاط أصلا — فهذا صمام تحسيني لا ضمان أمان شامل.
    // (EN) Catch limits: catch(...) takes C++ exceptions only, not SEH (an
    //      access violation in compiled code is still fatal), and a C++
    //      exception thrown through unwind-table-less IR frames on Win64 is
    //      UB before ever reaching the catch — this is a best-effort valve,
    //      not a blanket safety guarantee.
    try
    {
        reinterpret_cast<void (*)(void)>(user_data)();
    }
    catch (...)
    {
    }
}

// Callback-based Route Registration
// (AR) تسجيل مسارات بمعالجات أصلية — تلتقط callback + user_data في lambda
// ==========================================

void sad_http_server_get_cb(void *server, const char *path,
                            sad_http_native_handler handler, void *user_data)
{
    if (!server || !path || !handler)
        return;
    try
    {
        auto *http_server = static_cast<HttpServer *>(server);
        http_server->get(path, [handler, user_data](const HttpRequest &req, HttpResponse &res)
                         { handler(const_cast<HttpRequest *>(&req), &res, user_data); });
    }
    catch (...)
    {
    }
}

void sad_http_server_post_cb(void *server, const char *path,
                             sad_http_native_handler handler, void *user_data)
{
    if (!server || !path || !handler)
        return;
    try
    {
        auto *http_server = static_cast<HttpServer *>(server);
        http_server->post(path, [handler, user_data](const HttpRequest &req, HttpResponse &res)
                          { handler(const_cast<HttpRequest *>(&req), &res, user_data); });
    }
    catch (...)
    {
    }
}

void sad_http_server_put_cb(void *server, const char *path,
                            sad_http_native_handler handler, void *user_data)
{
    if (!server || !path || !handler)
        return;
    try
    {
        auto *http_server = static_cast<HttpServer *>(server);
        http_server->put(path, [handler, user_data](const HttpRequest &req, HttpResponse &res)
                         { handler(const_cast<HttpRequest *>(&req), &res, user_data); });
    }
    catch (...)
    {
    }
}

void sad_http_server_delete_cb(void *server, const char *path,
                               sad_http_native_handler handler, void *user_data)
{
    if (!server || !path || !handler)
        return;
    try
    {
        auto *http_server = static_cast<HttpServer *>(server);
        http_server->delete_(path, [handler, user_data](const HttpRequest &req, HttpResponse &res)
                             { handler(const_cast<HttpRequest *>(&req), &res, user_data); });
    }
    catch (...)
    {
    }
}

// ==========================================
// HTTP Request Functions (for handlers)
// ==========================================

char *sad_http_request_method(void *request)
{
    if (!request)
        request = t_currentRequest; // (AR) ع-14: سقوط إلى سياق المسار المصرّف
    if (!request)
        return nullptr;

    try
    {
        HttpRequest *http_request = static_cast<HttpRequest *>(request);
        return string_to_c_str(method_to_string(http_request->method()));
    }
    catch (...)
    {
        return nullptr;
    }
}

char *sad_http_request_path(void *request)
{
    if (!request)
        request = t_currentRequest; // (AR) ع-14: سقوط إلى سياق المسار المصرّف
    if (!request)
        return nullptr;

    try
    {
        HttpRequest *http_request = static_cast<HttpRequest *>(request);
        return string_to_c_str(http_request->path());
    }
    catch (...)
    {
        return nullptr;
    }
}

char *sad_http_request_body(void *request)
{
    if (!request)
        request = t_currentRequest; // (AR) ع-14: سقوط إلى سياق المسار المصرّف
    if (!request)
        return nullptr;

    try
    {
        HttpRequest *http_request = static_cast<HttpRequest *>(request);
        return string_to_c_str(http_request->body());
    }
    catch (...)
    {
        return nullptr;
    }
}

char *sad_http_request_header(void *request, const char *key)
{
    if (!request)
        request = t_currentRequest; // (AR) ع-14: سقوط إلى سياق المسار المصرّف
    if (!request || !key)
        return nullptr;

    try
    {
        HttpRequest *http_request = static_cast<HttpRequest *>(request);
        return string_to_c_str(http_request->header(key));
    }
    catch (...)
    {
        return nullptr;
    }
}

char *sad_http_request_query_param(void *request, const char *key)
{
    if (!request)
        request = t_currentRequest; // (AR) ع-14: سقوط إلى سياق المسار المصرّف
    if (!request || !key)
        return nullptr;

    try
    {
        HttpRequest *http_request = static_cast<HttpRequest *>(request);
        return string_to_c_str(http_request->query_param(key));
    }
    catch (...)
    {
        return nullptr;
    }
}

// ==========================================
// HTTP Response Functions (for handlers)
// ==========================================

void sad_http_response_set_status(void *response, int status)
{
    if (!response)
        response = t_currentResponse; // (AR) ع-14: سقوط إلى سياق المسار المصرّف
    if (!response)
        return;

    try
    {
        HttpResponse *http_response = static_cast<HttpResponse *>(response);
        http_response->set_status(static_cast<HttpStatus>(status));
    }
    catch (...)
    {
    }
}

void sad_http_response_set_body(void *response, const char *body)
{
    if (!response)
        response = t_currentResponse; // (AR) ع-14: سقوط إلى سياق المسار المصرّف
    if (!response || !body)
        return;

    try
    {
        HttpResponse *http_response = static_cast<HttpResponse *>(response);
        http_response->set_body(body);
    }
    catch (...)
    {
    }
}

void sad_http_response_set_json(void *response, const char *json)
{
    if (!response)
        response = t_currentResponse; // (AR) ع-14: سقوط إلى سياق المسار المصرّف
    if (!response || !json)
        return;

    try
    {
        HttpResponse *http_response = static_cast<HttpResponse *>(response);
        http_response->set_json(json);
    }
    catch (...)
    {
    }
}

void sad_http_response_set_html(void *response, const char *html)
{
    if (!response)
        response = t_currentResponse; // (AR) ع-14: سقوط إلى سياق المسار المصرّف
    if (!response || !html)
        return;

    try
    {
        HttpResponse *http_response = static_cast<HttpResponse *>(response);
        http_response->set_html(html);
    }
    catch (...)
    {
    }
}

void sad_http_response_set_header(void *response, const char *key, const char *value)
{
    if (!response)
        response = t_currentResponse; // (AR) ع-14: سقوط إلى سياق المسار المصرّف
    if (!response || !key || !value)
        return;

    try
    {
        HttpResponse *http_response = static_cast<HttpResponse *>(response);
        http_response->set_header(key, value);
    }
    catch (...)
    {
    }
}

// ==========================================
// Helper Functions
// ==========================================

void sad_http_free_string(char *str)
{
    if (str)
    {
        delete[] str;
    }
}
