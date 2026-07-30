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
