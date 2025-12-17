// بسم الله الرحمن الرحيم
/**
 * @file http_module.h
 * @brief HTTP/HTTPS Module for Sad Language - وحدة HTTP/HTTPS للغة ص
 * 
 * وحدة شاملة لعمليات HTTP/HTTPS تتضمن:
 * - HTTP Client (عميل HTTP)
 * - HTTP Server (خادم HTTP)
 * - WebSocket Support (دعم WebSocket)
 * - Request/Response Objects (كائنات الطلب/الاستجابة)
 * - URL Encoding/Decoding (تشفير/فك تشفير URL)
 * - Headers & Cookies Management (إدارة الترويسات والكوكيز)
 * - HTTPS/TLS Support (دعم HTTPS/TLS)
 * 
 * Comprehensive HTTP/HTTPS module including:
 * - HTTP Client
 * - HTTP Server
 * - WebSocket Support
 * - Request/Response Objects
 * - URL Encoding/Decoding
 * - Headers & Cookies Management
 * - HTTPS/TLS Support
 * 
 * @author Sad Language Team
 * @date December 2025
 * @version 1.0.0
 * 
 * الحمد لله رب العالمين
 */

#ifndef SAD_STDLIB_HTTP_MODULE_H
#define SAD_STDLIB_HTTP_MODULE_H

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <cstdint>

namespace sad {
namespace stdlib {
namespace http {

// ============================================================================
// Forward Declarations - التصريحات المسبقة
// ============================================================================

class HttpRequest;
class HttpResponse;
class HttpClient;
class HttpServer;
class WebSocket;

// ============================================================================
// HTTP Method Enumeration - تعداد طرق HTTP
// ============================================================================

/**
 * @brief HTTP Methods - طرق HTTP
 * 
 * طرق HTTP القياسية
 * Standard HTTP methods
 */
enum class HttpMethod {
    GET,        // جلب البيانات / Retrieve data
    POST,       // إرسال بيانات جديدة / Submit new data
    PUT,        // تحديث بيانات / Update data
    DELETE,     // حذف بيانات / Delete data
    PATCH,      // تحديث جزئي / Partial update
    HEAD,       // جلب الترويسات فقط / Retrieve headers only
    OPTIONS,    // جلب الخيارات المتاحة / Retrieve available options
    CONNECT,    // إنشاء نفق / Establish tunnel
    TRACE       // تتبع الطلب / Trace request
};

/**
 * @brief تحويل HttpMethod إلى سلسلة نصية / Convert HttpMethod to string
 */
std::string method_to_string(HttpMethod method);

/**
 * @brief تحويل سلسلة نصية إلى HttpMethod / Convert string to HttpMethod
 */
HttpMethod string_to_method(const std::string& method_str);

// ============================================================================
// HTTP Status Codes - رموز حالة HTTP
// ============================================================================

/**
 * @brief HTTP Status Codes - رموز حالة HTTP
 */
enum class HttpStatus {
    // 1xx Informational
    Continue = 100,
    SwitchingProtocols = 101,
    
    // 2xx Success
    OK = 200,
    Created = 201,
    Accepted = 202,
    NoContent = 204,
    
    // 3xx Redirection
    MovedPermanently = 301,
    Found = 302,
    SeeOther = 303,
    NotModified = 304,
    TemporaryRedirect = 307,
    PermanentRedirect = 308,
    
    // 4xx Client Errors
    BadRequest = 400,
    Unauthorized = 401,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    NotAcceptable = 406,
    RequestTimeout = 408,
    Conflict = 409,
    Gone = 410,
    PayloadTooLarge = 413,
    URITooLong = 414,
    UnsupportedMediaType = 415,
    TooManyRequests = 429,
    
    // 5xx Server Errors
    InternalServerError = 500,
    NotImplemented = 501,
    BadGateway = 502,
    ServiceUnavailable = 503,
    GatewayTimeout = 504
};

/**
 * @brief الحصول على وصف رمز الحالة / Get status code description
 */
std::string get_status_description(HttpStatus status);

// ============================================================================
// HTTP Headers - ترويسات HTTP
// ============================================================================

/**
 * @brief HTTP Headers Container - حاوية ترويسات HTTP
 * 
 * حاوية لتخزين وإدارة ترويسات HTTP
 * Container for storing and managing HTTP headers
 */
class HttpHeaders {
public:
    HttpHeaders() = default;
    
    /**
     * @brief تعيين قيمة ترويسة / Set header value
     * @param name اسم الترويسة / Header name
     * @param value قيمة الترويسة / Header value
     */
    void set(const std::string& name, const std::string& value);
    
    /**
     * @brief الحصول على قيمة ترويسة / Get header value
     * @param name اسم الترويسة / Header name
     * @return قيمة الترويسة أو سلسلة فارغة / Header value or empty string
     */
    std::string get(const std::string& name) const;
    
    /**
     * @brief فحص وجود ترويسة / Check if header exists
     * @param name اسم الترويسة / Header name
     * @return صحيح إذا كانت موجودة / True if exists
     */
    bool has(const std::string& name) const;
    
    /**
     * @brief حذف ترويسة / Remove header
     * @param name اسم الترويسة / Header name
     */
    void remove(const std::string& name);
    
    /**
     * @brief مسح جميع الترويسات / Clear all headers
     */
    void clear();
    
    /**
     * @brief الحصول على جميع الترويسات / Get all headers
     * @return خريطة الترويسات / Headers map
     */
    const std::map<std::string, std::string>& all() const { return headers_; }
    
    /**
     * @brief تحويل إلى سلسلة نصية HTTP / Convert to HTTP string
     */
    std::string to_string() const;

private:
    std::map<std::string, std::string> headers_;
    
    // تحويل اسم الترويسة إلى صيغة قياسية / Normalize header name
    static std::string normalize_name(const std::string& name);
};

// ============================================================================
// HTTP Cookies - كوكيز HTTP
// ============================================================================

/**
 * @brief HTTP Cookie - كوكي HTTP
 */
struct HttpCookie {
    std::string name;           // اسم الكوكي / Cookie name
    std::string value;          // قيمة الكوكي / Cookie value
    std::string domain;         // النطاق / Domain
    std::string path;           // المسار / Path
    int64_t max_age = -1;       // العمر الأقصى بالثواني / Max age in seconds
    bool secure = false;        // آمن (HTTPS فقط) / Secure (HTTPS only)
    bool http_only = false;     // HTTP فقط / HTTP only
    std::string same_site;      // نفس الموقع / Same site policy
    
    /**
     * @brief تحويل إلى سلسلة Set-Cookie / Convert to Set-Cookie string
     */
    std::string to_set_cookie_string() const;
    
    /**
     * @brief تحليل من سلسلة Set-Cookie / Parse from Set-Cookie string
     */
    static HttpCookie from_set_cookie_string(const std::string& str);
};

// ============================================================================
// HTTP Request - طلب HTTP
// ============================================================================

/**
 * @brief HTTP Request Object - كائن طلب HTTP
 * 
 * يمثل طلب HTTP كامل مع الطريقة، المسار، الترويسات، والجسم
 * Represents a complete HTTP request with method, path, headers, and body
 */
class HttpRequest {
public:
    HttpRequest() = default;
    HttpRequest(HttpMethod method, const std::string& url);
    
    // Getters
    HttpMethod get_method() const { return method_; }
    const std::string& get_url() const { return url_; }
    const std::string& get_path() const { return path_; }
    const std::string& get_query_string() const { return query_string_; }
    const std::map<std::string, std::string>& get_query_params() const { return query_params_; }
    const HttpHeaders& get_headers() const { return headers_; }
    const std::string& get_body() const { return body_; }
    const std::vector<HttpCookie>& get_cookies() const { return cookies_; }
    
    // Setters
    void set_method(HttpMethod method) { method_ = method; }
    void set_url(const std::string& url);
    void set_header(const std::string& name, const std::string& value);
    void set_body(const std::string& body) { body_ = body; }
    void add_cookie(const HttpCookie& cookie);
    
    /**
     * @brief الحصول على معامل استعلام / Get query parameter
     */
    std::string get_query_param(const std::string& name, const std::string& default_value = "") const;
    
    /**
     * @brief الحصول على كوكي / Get cookie
     */
    std::string get_cookie(const std::string& name, const std::string& default_value = "") const;
    
    /**
     * @brief تحويل إلى سلسلة HTTP خام / Convert to raw HTTP string
     */
    std::string to_string() const;
    
    /**
     * @brief تحليل من سلسلة HTTP خام / Parse from raw HTTP string
     */
    static HttpRequest from_string(const std::string& request_str);

private:
    HttpMethod method_ = HttpMethod::GET;
    std::string url_;
    std::string path_;
    std::string query_string_;
    std::map<std::string, std::string> query_params_;
    HttpHeaders headers_;
    std::string body_;
    std::vector<HttpCookie> cookies_;
    
    void parse_url();
    void parse_query_string();
};

// ============================================================================
// HTTP Response - استجابة HTTP
// ============================================================================

/**
 * @brief HTTP Response Object - كائن استجابة HTTP
 * 
 * يمثل استجابة HTTP كاملة مع رمز الحالة، الترويسات، والجسم
 * Represents a complete HTTP response with status code, headers, and body
 */
class HttpResponse {
public:
    HttpResponse() = default;
    HttpResponse(HttpStatus status);
    
    // Getters
    HttpStatus get_status() const { return status_; }
    const HttpHeaders& get_headers() const { return headers_; }
    const std::string& get_body() const { return body_; }
    const std::vector<HttpCookie>& get_cookies() const { return cookies_; }
    
    // Setters
    void set_status(HttpStatus status) { status_ = status; }
    void set_header(const std::string& name, const std::string& value);
    void set_body(const std::string& body);
    void set_json(const std::string& json);
    void set_html(const std::string& html);
    void add_cookie(const HttpCookie& cookie);
    
    /**
     * @brief تحويل إلى سلسلة HTTP خام / Convert to raw HTTP string
     */
    std::string to_string() const;
    
    /**
     * @brief تحليل من سلسلة HTTP خام / Parse from raw HTTP string
     */
    static HttpResponse from_string(const std::string& response_str);

private:
    HttpStatus status_ = HttpStatus::OK;
    HttpHeaders headers_;
    std::string body_;
    std::vector<HttpCookie> cookies_;
};

// ============================================================================
// HTTP Client - عميل HTTP
// ============================================================================

/**
 * @brief HTTP Client - عميل HTTP
 * 
 * عميل HTTP متقدم لإرسال الطلبات واستقبال الاستجابات
 * Advanced HTTP client for sending requests and receiving responses
 * 
 * @example
 * HttpClient client;
 * client.set_timeout(30);
 * 
 * auto response = client.get("https://api.example.com/data");
 * if (response.get_status() == HttpStatus::OK) {
 *     std::cout << response.get_body() << std::endl;
 * }
 */
class HttpClient {
public:
    HttpClient();
    ~HttpClient();
    
    /**
     * @brief إرسال طلب GET / Send GET request
     * @param url عنوان URL / URL
     * @return استجابة HTTP / HTTP response
     */
    HttpResponse get(const std::string& url);
    
    /**
     * @brief إرسال طلب GET مع ترويسات / Send GET request with headers
     */
    HttpResponse get(const std::string& url, const HttpHeaders& headers);
    
    /**
     * @brief إرسال طلب POST / Send POST request
     * @param url عنوان URL / URL
     * @param body جسم الطلب / Request body
     * @return استجابة HTTP / HTTP response
     */
    HttpResponse post(const std::string& url, const std::string& body);
    
    /**
     * @brief إرسال طلب POST مع ترويسات / Send POST request with headers
     */
    HttpResponse post(const std::string& url, const std::string& body, const HttpHeaders& headers);
    
    /**
     * @brief إرسال طلب PUT / Send PUT request
     */
    HttpResponse put(const std::string& url, const std::string& body);
    
    /**
     * @brief إرسال طلب DELETE / Send DELETE request
     */
    HttpResponse del(const std::string& url);
    
    /**
     * @brief إرسال طلب PATCH / Send PATCH request
     */
    HttpResponse patch(const std::string& url, const std::string& body);
    
    /**
     * @brief إرسال طلب HEAD / Send HEAD request
     */
    HttpResponse head(const std::string& url);
    
    /**
     * @brief إرسال طلب OPTIONS / Send OPTIONS request
     */
    HttpResponse options(const std::string& url);
    
    /**
     * @brief إرسال طلب مخصص / Send custom request
     */
    HttpResponse request(const HttpRequest& request);
    
    /**
     * @brief تعيين مهلة الاتصال (بالثواني) / Set connection timeout (seconds)
     */
    void set_timeout(int timeout_seconds);
    
    /**
     * @brief تعيين عدد إعادة المحاولات / Set number of retries
     */
    void set_retries(int retries);
    
    /**
     * @brief تعيين متابعة إعادة التوجيه / Set follow redirects
     */
    void set_follow_redirects(bool follow);
    
    /**
     * @brief تعيين التحقق من SSL / Set SSL verification
     */
    void set_verify_ssl(bool verify);
    
    /**
     * @brief تعيين ترويسة افتراضية / Set default header
     */
    void set_default_header(const std::string& name, const std::string& value);
    
    /**
     * @brief تعيين User-Agent / Set User-Agent
     */
    void set_user_agent(const std::string& user_agent);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// ============================================================================
// HTTP Server - خادم HTTP
// ============================================================================

/**
 * @brief Route Handler Function - دالة معالج المسار
 */
using RouteHandler = std::function<HttpResponse(const HttpRequest&)>;

/**
 * @brief Middleware Function - دالة الوسيط
 */
using Middleware = std::function<bool(HttpRequest&, HttpResponse&)>;

/**
 * @brief HTTP Server - خادم HTTP
 * 
 * خادم HTTP متقدم مع دعم التوجيه والوسائط
 * Advanced HTTP server with routing and middleware support
 * 
 * @example
 * HttpServer server;
 * 
 * server.get("/api/users", [](const HttpRequest& req) {
 *     HttpResponse res(HttpStatus::OK);
 *     res.set_json("{\"users\": []}");
 *     return res;
 * });
 * 
 * server.listen(8080);
 */
class HttpServer {
public:
    HttpServer();
    ~HttpServer();
    
    /**
     * @brief تسجيل معالج GET / Register GET handler
     */
    void get(const std::string& path, RouteHandler handler);
    
    /**
     * @brief تسجيل معالج POST / Register POST handler
     */
    void post(const std::string& path, RouteHandler handler);
    
    /**
     * @brief تسجيل معالج PUT / Register PUT handler
     */
    void put(const std::string& path, RouteHandler handler);
    
    /**
     * @brief تسجيل معالج DELETE / Register DELETE handler
     */
    void del(const std::string& path, RouteHandler handler);
    
    /**
     * @brief تسجيل معالج PATCH / Register PATCH handler
     */
    void patch(const std::string& path, RouteHandler handler);
    
    /**
     * @brief تسجيل معالج لأي طريقة / Register handler for any method
     */
    void route(HttpMethod method, const std::string& path, RouteHandler handler);
    
    /**
     * @brief إضافة وسيط / Add middleware
     */
    void use(Middleware middleware);
    
    /**
     * @brief تقديم ملفات ثابتة / Serve static files
     */
    void serve_static(const std::string& url_prefix, const std::string& directory);
    
    /**
     * @brief بدء الاستماع / Start listening
     */
    void listen(uint16_t port, const std::string& host = "0.0.0.0");
    
    /**
     * @brief إيقاف الخادم / Stop server
     */
    void stop();
    
    /**
     * @brief فحص إذا كان الخادم يعمل / Check if server is running
     */
    bool is_running() const;
    
    /**
     * @brief تعيين حد الاتصالات المتزامنة / Set max concurrent connections
     */
    void set_max_connections(int max_connections);
    
    /**
     * @brief تعيين مهلة القراءة / Set read timeout
     */
    void set_read_timeout(int timeout_seconds);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// ============================================================================
// WebSocket - ويب سوكت
// ============================================================================

/**
 * @brief WebSocket Message Type - نوع رسالة WebSocket
 */
enum class WebSocketMessageType {
    Text,       // رسالة نصية / Text message
    Binary,     // رسالة ثنائية / Binary message
    Ping,       // طلب Ping
    Pong,       // رد Pong
    Close       // إغلاق الاتصال / Close connection
};

/**
 * @brief WebSocket - ويب سوكت
 * 
 * اتصال WebSocket ثنائي الاتجاه
 * Bidirectional WebSocket connection
 */
class WebSocket {
public:
    WebSocket();
    ~WebSocket();
    
    /**
     * @brief الاتصال بخادم WebSocket / Connect to WebSocket server
     */
    bool connect(const std::string& url);
    
    /**
     * @brief إرسال رسالة نصية / Send text message
     */
    void send_text(const std::string& message);
    
    /**
     * @brief إرسال رسالة ثنائية / Send binary message
     */
    void send_binary(const std::vector<uint8_t>& data);
    
    /**
     * @brief استقبال رسالة / Receive message
     */
    std::string receive();
    
    /**
     * @brief فحص إذا كان هناك رسائل متاحة / Check if messages are available
     */
    bool has_message() const;
    
    /**
     * @brief إغلاق الاتصال / Close connection
     */
    void close();
    
    /**
     * @brief فحص إذا كان الاتصال مفتوح / Check if connection is open
     */
    bool is_open() const;
    
    /**
     * @brief تعيين معالج الرسائل / Set message handler
     */
    void on_message(std::function<void(const std::string&, WebSocketMessageType)> handler);
    
    /**
     * @brief تعيين معالج الاتصال / Set connect handler
     */
    void on_connect(std::function<void()> handler);
    
    /**
     * @brief تعيين معالج الإغلاق / Set close handler
     */
    void on_close(std::function<void()> handler);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// ============================================================================
// Utility Functions - دوال مساعدة
// ============================================================================

/**
 * @brief تشفير URL / URL encode
 */
std::string url_encode(const std::string& str);

/**
 * @brief فك تشفير URL / URL decode
 */
std::string url_decode(const std::string& str);

/**
 * @brief تحليل معاملات الاستعلام / Parse query parameters
 */
std::map<std::string, std::string> parse_query_string(const std::string& query);

/**
 * @brief بناء سلسلة استعلام / Build query string
 */
std::string build_query_string(const std::map<std::string, std::string>& params);

/**
 * @brief تحليل URL / Parse URL
 */
struct UrlParts {
    std::string scheme;     // http, https, ws, wss
    std::string host;
    uint16_t port;
    std::string path;
    std::string query;
    std::string fragment;
};

UrlParts parse_url(const std::string& url);

/**
 * @brief بناء URL / Build URL
 */
std::string build_url(const UrlParts& parts);

} // namespace http
} // namespace stdlib
} // namespace sad

#endif // SAD_STDLIB_HTTP_MODULE_H

// الحمد لله رب العالمين
