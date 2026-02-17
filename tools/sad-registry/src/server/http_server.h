// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: http_server.h
 * الوصف: خادم HTTP مبسط بدون تبعيات خارجية
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * هذا الملف يوفر خادم HTTP كامل الوظائف مبني من الصفر.
 * لا يعتمد على أي مكتبة خارجية - يستخدم فقط مقابس النظام (sockets).
 *
 * المزايا:
 *   - يدعم الطلبات: GET, POST, PUT, DELETE, PATCH, OPTIONS
 *   - يدعم تقديم الملفات الثابتة (HTML, CSS, JS, صور)
 *   - يدعم ترميز UTF-8 بالكامل (للمسارات العربية)
 *   - يدعم رؤوس CORS للسماح بالوصول من واجهات ويب خارجية
 *   - يدعم التعامل مع عدة اتصالات متزامنة (عبر خيوط)
 *   - يكتشف نوع المحتوى (Content-Type) تلقائياً
 *   - يدعم تحليل استعلامات URL (?key=value)
 *   - يدعم تحليل جسم الطلب بصيغة JSON
 *
 * البنية:
 *   HttpRequest  → هيكل يمثل الطلب الوارد
 *   HttpResponse → هيكل يمثل الاستجابة المرسلة
 *   HttpServer   → الخادم الذي يستمع ويوزع الطلبات
 *
 * المنصات المدعومة:
 *   - Windows (Winsock2)
 *   - Linux/macOS (POSIX sockets)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace sad {
namespace registry {

// ═══════════════════════════════════════════════════════════════════════════════
// طلب HTTP - يمثل الطلب الوارد من العميل
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct HttpRequest
 * @brief يمثل طلب HTTP وارد من العميل
 *
 * يحتوي على جميع المعلومات المُستخرجة من الطلب الخام:
 *   - الفعل (GET/POST/etc.)
 *   - المسار (/api/v1/packages)
 *   - الرؤوس (Content-Type, Authorization, etc.)
 *   - جسم الطلب (JSON للـ POST/PUT)
 *   - معاملات الاستعلام (?page=1&limit=20)
 *   - معاملات المسار (/packages/:name → params["name"])
 *
 * مثال:
 *   استعلام: GET /api/v1/حزم?بحث=شبكة&صفحة=1
 *   يُصبح:
 *     method = "GET"
 *     path = "/api/v1/حزم"
 *     query_params["بحث"] = "شبكة"
 *     query_params["صفحة"] = "1"
 */
struct HttpRequest {
    /// الفعل: GET, POST, PUT, DELETE, PATCH, OPTIONS
    std::string method;

    /// المسار الكامل (بدون معاملات الاستعلام)
    std::string path;

    /// الرسالة الخام (للتشخيص)
    std::string raw_url;

    /// رؤوس الطلب (اسم → قيمة)
    std::map<std::string, std::string> headers;

    /// جسم الطلب (للـ POST/PUT)
    std::string body;

    /// معاملات الاستعلام في URL (?key=value&key2=value2)
    std::map<std::string, std::string> query_params;

    /// معاملات المسار (/packages/:name → "name" → "json")
    std::map<std::string, std::string> path_params;

    /// عنوان IP للعميل
    std::string remote_addr;

    // ─────────────────────────────────────────────────────────────────────
    // دوال مساعدة
    // ─────────────────────────────────────────────────────────────────────

    /**
     * @brief الحصول على قيمة رأس معين
     * @param name اسم الرأس (غير حساس لحالة الأحرف)
     * @param default_val القيمة الافتراضية إذا لم يُوجد الرأس
     * @return قيمة الرأس أو القيمة الافتراضية
     */
    std::string get_header(const std::string& name,
                           const std::string& default_val = "") const {
        // البحث بدون حساسية لحالة الأحرف
        for (const auto& [k, v] : headers) {
            std::string lower_k = k;
            std::string lower_name = name;
            std::transform(lower_k.begin(), lower_k.end(), lower_k.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
            std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
            if (lower_k == lower_name) return v;
        }
        return default_val;
    }

    /**
     * @brief الحصول على معامل استعلام
     * @param name اسم المعامل
     * @param default_val القيمة الافتراضية
     */
    std::string get_query(const std::string& name,
                          const std::string& default_val = "") const {
        auto it = query_params.find(name);
        return it != query_params.end() ? it->second : default_val;
    }

    /**
     * @brief الحصول على معامل مسار
     */
    std::string get_param(const std::string& name,
                          const std::string& default_val = "") const {
        auto it = path_params.find(name);
        return it != path_params.end() ? it->second : default_val;
    }

    /**
     * @brief هل نوع المحتوى هو JSON؟
     */
    bool is_json() const {
        return get_header("Content-Type").find("application/json") != std::string::npos;
    }

    /**
     * @brief الحصول على رمز المصادقة من رأس Authorization
     * @return الرمز (بدون "Bearer ") أو سلسلة فارغة
     *
     * يدعم صيغتين:
     *   Authorization: Bearer <token>
     *   Authorization: Token <token>
     */
    std::string get_auth_token() const {
        std::string auth = get_header("Authorization");
        if (auth.substr(0, 7) == "Bearer ") return auth.substr(7);
        if (auth.substr(0, 6) == "Token ")  return auth.substr(6);
        return "";
    }

    /**
     * @brief الحصول على رقم الصفحة من معاملات الاستعلام
     * @return رقم الصفحة (١ على الأقل)
     */
    int get_page() const {
        std::string p = get_query("page", get_query("صفحة", "1"));
        int page = std::atoi(p.c_str());
        return std::max(1, page);
    }

    /**
     * @brief الحصول على عدد العناصر لكل صفحة
     * @return العدد (بين ١ و١٠٠)
     */
    int get_per_page() const {
        std::string p = get_query("per_page", get_query("لكل_صفحة", "20"));
        int pp = std::atoi(p.c_str());
        return std::max(1, std::min(100, pp));
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// استجابة HTTP - يمثل الاستجابة المرسلة للعميل
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct HttpResponse
 * @brief يمثل استجابة HTTP مرسلة للعميل
 *
 * يتم بناء الاستجابة باستخدام دوال البناء المتسلسلة (fluent API):
 *   HttpResponse::json(data).status(200).header("X-Custom", "value")
 */
struct HttpResponse {
    /// رمز الحالة (200, 404, 500, إلخ)
    int status_code = 200;

    /// جسم الاستجابة
    std::string body;

    /// رؤوس الاستجابة
    std::map<std::string, std::string> headers;

    // ─────────────────────────────────────────────────────────────────────
    // دوال بناء سلسلية (Fluent API)
    // ─────────────────────────────────────────────────────────────────────

    /**
     * @brief إنشاء استجابة JSON
     */
    static HttpResponse json(const std::string& json_body, int code = 200) {
        HttpResponse res;
        res.status_code = code;
        res.body = json_body;
        res.headers["Content-Type"] = "application/json; charset=utf-8";
        return res;
    }

    /**
     * @brief إنشاء استجابة نص عادي
     */
    static HttpResponse text(const std::string& text_body, int code = 200) {
        HttpResponse res;
        res.status_code = code;
        res.body = text_body;
        res.headers["Content-Type"] = "text/plain; charset=utf-8";
        return res;
    }

    /**
     * @brief إنشاء استجابة HTML
     */
    static HttpResponse html(const std::string& html_body, int code = 200) {
        HttpResponse res;
        res.status_code = code;
        res.body = html_body;
        res.headers["Content-Type"] = "text/html; charset=utf-8";
        return res;
    }

    /**
     * @brief إنشاء استجابة خطأ JSON
     */
    static HttpResponse error(const std::string& message, int code = 400) {
        std::string json_body = "{\"error\":true,\"message\":\"" + message + "\",\"code\":" + std::to_string(code) + "}";
        return json(json_body, code);
    }

    /**
     * @brief إنشاء استجابة خطأ JSON (ترتيب بديل: كود ثم رسالة)
     */
    static HttpResponse error(int code, const std::string& message) {
        return error(message, code);
    }

    /**
     * @brief إنشاء استجابة نجاح JSON
     */
    static HttpResponse success(const std::string& message = "تمت العملية بنجاح") {
        std::string json_body = "{\"success\":true,\"message\":\"" + message + "\"}";
        return json(json_body, 200);
    }

    /**
     * @brief إنشاء استجابة 404 - غير موجود
     */
    static HttpResponse not_found(const std::string& message = "المورد غير موجود") {
        return error(message, 404);
    }

    /**
     * @brief إنشاء استجابة 401 - غير مصرح
     */
    static HttpResponse unauthorized(const std::string& message = "يجب تسجيل الدخول أولاً") {
        return error(message, 401);
    }

    /**
     * @brief إنشاء استجابة 403 - محظور
     */
    static HttpResponse forbidden(const std::string& message = "ليس لديك صلاحية لهذه العملية") {
        return error(message, 403);
    }

    /**
     * @brief إنشاء استجابة 500 - خطأ داخلي
     */
    static HttpResponse internal_error(const std::string& message = "خطأ داخلي في الخادم") {
        return error(message, 500);
    }

    /**
     * @brief تعيين رأس
     */
    HttpResponse& set_header(const std::string& name, const std::string& value) {
        headers[name] = value;
        return *this;
    }

    /**
     * @brief بناء الاستجابة الخام لإرسالها عبر المقبس
     * @return نص HTTP كامل (سطر الحالة + رؤوس + جسم)
     */
    std::string build() const {
        std::ostringstream oss;

        // سطر الحالة
        oss << "HTTP/1.1 " << status_code << " " << status_text() << "\r\n";

        // الرؤوس
        for (const auto& [name, value] : headers) {
            oss << name << ": " << value << "\r\n";
        }

        // رؤوس إضافية تلقائية
        oss << "Content-Length: " << body.size() << "\r\n";
        oss << "Connection: close\r\n";

        // رؤوس CORS - نسمح بالوصول من أي مصدر
        oss << "Access-Control-Allow-Origin: *\r\n";
        oss << "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, PATCH, OPTIONS\r\n";
        oss << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n";

        // خادم
        oss << "Server: sad-registry/1.0\r\n";

        // فاصل بين الرؤوس والجسم
        oss << "\r\n";

        // الجسم
        oss << body;

        return oss.str();
    }

private:
    /**
     * @brief نص رمز الحالة
     */
    std::string status_text() const {
        switch (status_code) {
            case 200: return "OK";
            case 201: return "Created";
            case 204: return "No Content";
            case 301: return "Moved Permanently";
            case 302: return "Found";
            case 304: return "Not Modified";
            case 400: return "Bad Request";
            case 401: return "Unauthorized";
            case 403: return "Forbidden";
            case 404: return "Not Found";
            case 405: return "Method Not Allowed";
            case 409: return "Conflict";
            case 413: return "Payload Too Large";
            case 429: return "Too Many Requests";
            case 500: return "Internal Server Error";
            case 502: return "Bad Gateway";
            case 503: return "Service Unavailable";
            default:  return "Unknown";
        }
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// نوع معالج الطلب
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief نوع دالة معالجة الطلب
 *
 * كل نقطة API مرتبطة بدالة معالجة تأخذ طلباً وترجع استجابة.
 * مثال:
 *   [](const HttpRequest& req) -> HttpResponse {
 *       return HttpResponse::json("{\"hello\":\"مرحباً\"}");
 *   }
 */
using RequestHandler = std::function<HttpResponse(const HttpRequest&)>;

// ═══════════════════════════════════════════════════════════════════════════════
// خادم HTTP الرئيسي
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class HttpServer
 * @brief خادم HTTP مبسط لمستودع الحزم
 *
 * هذا الخادم مبني من الصفر بدون أي تبعيات خارجية.
 * يستخدم مقابس النظام (Winsock على Windows, POSIX على Linux).
 *
 * آلية العمل:
 *   ١. يفتح مقبس استماع على المنفذ المحدد
 *   ٢. ينتظر اتصالات واردة
 *   ٣. عند ورود اتصال، يُنشئ خيطاً جديداً لمعالجته
 *   ٤. يحلل الطلب الخام ويحوله إلى HttpRequest
 *   ٥. يبحث عن المعالج المناسب في جدول المسارات
 *   ٦. ينفذ المعالج ويرسل الاستجابة
 *   ٧. يغلق الاتصال
 *
 * استخدام:
 * @code
 *   HttpServer server("0.0.0.0", 8080);
 *
 *   server.get("/api/v1/packages", [](const HttpRequest& req) {
 *       return HttpResponse::json("{\"packages\":[]}");
 *   });
 *
 *   server.post("/api/v1/publish", [](const HttpRequest& req) {
 *       // معالجة النشر...
 *       return HttpResponse::success("تم النشر بنجاح");
 *   });
 *
 *   server.start(); // يحجز حتى الإيقاف
 * @endcode
 */
class HttpServer {
public:
    // ─────────────────────────────────────────────────────────────────────
    // الإنشاء والإعداد
    // ─────────────────────────────────────────────────────────────────────

    /**
     * @brief المُنشئ
     * @param host عنوان الاستماع (مثل "0.0.0.0" لجميع العناوين)
     * @param port منفذ الاستماع
     */
    HttpServer(const std::string& host, int port);

    /**
     * @brief المُهدم - يُوقف الخادم ويُغلق المقبس
     */
    ~HttpServer();

    // ─────────────────────────────────────────────────────────────────────
    // تسجيل المسارات (Routes)
    // ─────────────────────────────────────────────────────────────────────

    /**
     * @brief تسجيل معالج GET
     * @param path نمط المسار (يدعم :param لمعاملات المسار)
     * @param handler دالة المعالجة
     *
     * أمثلة:
     *   get("/api/v1/packages", handler)           - مسار ثابت
     *   get("/api/v1/packages/:name", handler)     - مع معامل
     *   get("/api/v1/packages/:name/:version", handler) - معاملان
     */
    void get(const std::string& path, RequestHandler handler);

    /**
     * @brief تسجيل معالج POST
     */
    void post(const std::string& path, RequestHandler handler);

    /**
     * @brief تسجيل معالج PUT
     */
    void put(const std::string& path, RequestHandler handler);

    /**
     * @brief تسجيل معالج DELETE
     */
    void del(const std::string& path, RequestHandler handler);

    /**
     * @brief تسجيل معالج PATCH
     */
    void patch(const std::string& path, RequestHandler handler);

    // ─────────────────────────────────────────────────────────────────────
    // الملفات الثابتة
    // ─────────────────────────────────────────────────────────────────────

    /**
     * @brief تعيين مجلد الملفات الثابتة (HTML, CSS, JS)
     * @param dir مسار المجلد
     *
     * أي طلب GET لا يطابق مسار API يُبحث عنه في هذا المجلد.
     * مثلاً: GET /index.html → يقرأ ملف {dir}/index.html
     */
    void set_static_dir(const std::string& dir);

    // ─────────────────────────────────────────────────────────────────────
    // التشغيل والإيقاف
    // ─────────────────────────────────────────────────────────────────────

    /**
     * @brief تشغيل الخادم والبدء بالاستماع
     * @return true إذا نجح التشغيل
     *
     * هذه الدالة تحجز (blocking) حتى يتم استدعاء stop().
     * تُنشئ مقبس استماع وتبدأ بقبول الاتصالات.
     */
    bool start();

    /**
     * @brief إيقاف الخادم بأمان
     *
     * تُعلّم الخادم للتوقف عن قبول اتصالات جديدة.
     * تنتظر انتهاء الطلبات الحالية قبل الإغلاق.
     */
    void stop();

    /**
     * @brief هل الخادم يعمل؟
     */
    bool is_running() const { return running_.load(); }

private:
    // ─────────────────────────────────────────────────────────────────────
    // البنية الداخلية للمسارات
    // ─────────────────────────────────────────────────────────────────────

    /**
     * @struct Route
     * @brief يمثل مساراً مسجلاً مع معالجه
     *
     * كل مسار له:
     *   - نمط (pattern): المسار مع أسماء المعاملات
     *   - أجزاء (parts): المسار مقسّم بـ /
     *   - معالج (handler): الدالة المسؤولة عن الطلب
     */
    struct Route {
        std::string method;                  // الفعل (GET/POST/etc.)
        std::string pattern;                 // نمط المسار الأصلي
        std::vector<std::string> parts;      // أجزاء المسار مقسمة بـ /
        RequestHandler handler;              // دالة المعالجة
    };

    // ─────────────────────────────────────────────────────────────────────
    // الأعضاء الخاصة
    // ─────────────────────────────────────────────────────────────────────

    std::string host_;                       // عنوان الاستماع
    int port_;                               // المنفذ
    std::atomic<bool> running_{false};       // هل الخادم يعمل؟
    std::vector<Route> routes_;              // جدول المسارات المسجلة
    std::string static_dir_;                 // مجلد الملفات الثابتة
    std::mutex routes_mutex_;                // قفل لحماية جدول المسارات

    // مقبس الخادم (نستخدم uintptr_t ليتوافق مع Windows و POSIX)
    uintptr_t server_socket_ = 0;

    // ─────────────────────────────────────────────────────────────────────
    // دوال داخلية
    // ─────────────────────────────────────────────────────────────────────

    /**
     * @brief تسجيل مسار جديد
     */
    void add_route(const std::string& method, const std::string& path,
                   RequestHandler handler);

    /**
     * @brief معالجة اتصال وارد (تعمل في خيط منفصل)
     * @param client_socket مقبس العميل
     * @param client_addr عنوان العميل
     */
    void handle_connection(uintptr_t client_socket, const std::string& client_addr);

    /**
     * @brief تحليل طلب HTTP خام إلى HttpRequest
     * @param raw النص الخام المستقبل
     * @return الطلب المُحلَّل
     */
    HttpRequest parse_request(const std::string& raw, const std::string& client_addr);

    /**
     * @brief مطابقة الطلب مع المسارات المسجلة
     * @param method الفعل
     * @param path المسار
     * @param[out] params المعاملات المستخرجة
     * @return مؤشر على المسار المطابق، أو nullptr
     */
    const Route* match_route(const std::string& method, const std::string& path,
                             std::map<std::string, std::string>& params);

    /**
     * @brief تقديم ملف ثابت
     */
    HttpResponse serve_static_file(const std::string& path);

    /**
     * @brief تحديد نوع المحتوى من امتداد الملف
     */
    static std::string get_content_type(const std::string& path);

    /**
     * @brief فك ترميز URL (%20 → مسافة)
     */
    static std::string url_decode(const std::string& encoded);

    /**
     * @brief تقسيم مسار بعلامة /
     */
    static std::vector<std::string> split_path(const std::string& path);
};

} // namespace registry
} // namespace sad
