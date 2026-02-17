// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: http_server.cpp
 * الوصف: تنفيذ خادم HTTP لمستودع حزم لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * هذا الملف يحتوي على التنفيذ الكامل لخادم HTTP.
 * يستخدم مقابس النظام (Winsock2 على ويندوز, POSIX على لينكس).
 *
 * آلية العمل:
 *   ١. المُنشئ يحفظ الإعدادات
 *   ٢. start() يُنشئ المقبس ويبدأ الاستماع
 *   ٣. كل اتصال وارد يُعالج في خيط منفصل
 *   ٤. handle_connection() يقرأ الطلب، يحلله، يجد المعالج، يرسل الاستجابة
 *   ٥. stop() يُغلق المقبس ويوقف الخادم
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "http_server.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <thread>
#include <chrono>

// ─────────────────────────────────────────────────────────────────────────────
// رؤوس المقابس حسب نظام التشغيل
// ─────────────────────────────────────────────────────────────────────────────

#ifdef _WIN32
    // ويندوز: نستخدم Winsock2
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")

    // على ويندوز، المقبس نوعه SOCKET (unsigned __int64)
    // نحوّل بين SOCKET و uintptr_t
    #define CLOSE_SOCKET(s) closesocket((SOCKET)(s))
    #define INVALID_SOCK    ((uintptr_t)INVALID_SOCKET)

    /**
     * @brief تهيئة مكتبة Winsock (مطلوبة على ويندوز فقط)
     *
     * يجب استدعاء WSAStartup() قبل أي عملية شبكة على ويندوز.
     * نطلب الإصدار ٢.٢ وهو المعياري.
     * WSACleanup() يُستدعى تلقائياً عند إنهاء البرنامج.
     */
    static bool init_winsock() {
        WSADATA wsa;
        return WSAStartup(MAKEWORD(2, 2), &wsa) == 0;
    }

    static void cleanup_winsock() {
        WSACleanup();
    }
#else
    // لينكس/ماك: نستخدم مقابس POSIX
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>

    #define CLOSE_SOCKET(s) close((int)(s))
    #define INVALID_SOCK    ((uintptr_t)-1)

    static bool init_winsock() { return true; }
    static void cleanup_winsock() {}
#endif


namespace sad {
namespace registry {

// ═══════════════════════════════════════════════════════════════════════════════
// المُنشئ والمُهدم
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * المُنشئ - يحفظ إعدادات الخادم فقط
 * لا يفتح أي مقبس هنا - يتم ذلك في start()
 */
HttpServer::HttpServer(const std::string& host, int port)
    : host_(host)
    , port_(port)
    , server_socket_(INVALID_SOCK) {
}

/**
 * المُهدم - يتأكد من إغلاق الخادم
 */
HttpServer::~HttpServer() {
    stop();
    cleanup_winsock();
}

// ═══════════════════════════════════════════════════════════════════════════════
// تسجيل المسارات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief تسجيل معالج GET
 */
void HttpServer::get(const std::string& path, RequestHandler handler) {
    add_route("GET", path, std::move(handler));
}

/**
 * @brief تسجيل معالج POST
 */
void HttpServer::post(const std::string& path, RequestHandler handler) {
    add_route("POST", path, std::move(handler));
}

/**
 * @brief تسجيل معالج PUT
 */
void HttpServer::put(const std::string& path, RequestHandler handler) {
    add_route("PUT", path, std::move(handler));
}

/**
 * @brief تسجيل معالج DELETE
 */
void HttpServer::del(const std::string& path, RequestHandler handler) {
    add_route("DELETE", path, std::move(handler));
}

/**
 * @brief تسجيل معالج PATCH
 */
void HttpServer::patch(const std::string& path, RequestHandler handler) {
    add_route("PATCH", path, std::move(handler));
}

/**
 * @brief تسجيل مسار جديد في جدول المسارات
 *
 * يقسم المسار إلى أجزاء لتسهيل المطابقة.
 * مثلاً: "/api/v1/packages/:name" → ["api", "v1", "packages", ":name"]
 * الأجزاء التي تبدأ بـ : هي معاملات متغيرة.
 */
void HttpServer::add_route(const std::string& method, const std::string& path,
                           RequestHandler handler) {
    std::lock_guard<std::mutex> lock(routes_mutex_);

    Route route;
    route.method = method;
    route.pattern = path;
    route.parts = split_path(path);
    route.handler = std::move(handler);

    routes_.push_back(std::move(route));
}

/**
 * @brief تعيين مجلد الملفات الثابتة
 */
void HttpServer::set_static_dir(const std::string& dir) {
    static_dir_ = dir;
}

// ═══════════════════════════════════════════════════════════════════════════════
// التشغيل والإيقاف
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief تشغيل الخادم
 *
 * الخطوات:
 *   ١. تهيئة Winsock (على ويندوز فقط)
 *   ٢. إنشاء مقبس TCP
 *   ٣. تفعيل خيار SO_REUSEADDR (لتجنب "Address already in use")
 *   ٤. ربط المقبس بالعنوان والمنفذ
 *   ٥. بدء الاستماع (listen)
 *   ٦. حلقة قبول الاتصالات (accept loop)
 *   ٧. لكل اتصال: إنشاء خيط لمعالجته
 */
bool HttpServer::start() {
    // الخطوة ١: تهيئة Winsock
    if (!init_winsock()) {
        std::cerr << "  ✗ فشل تهيئة مكتبة الشبكة (Winsock)\n";
        return false;
    }

    // الخطوة ٢: إنشاء مقبس TCP
    // AF_INET = IPv4, SOCK_STREAM = TCP, IPPROTO_TCP = بروتوكول TCP
    server_socket_ = (uintptr_t)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket_ == INVALID_SOCK) {
        std::cerr << "  ✗ فشل إنشاء المقبس\n";
        return false;
    }

    // الخطوة ٣: تفعيل إعادة استخدام العنوان
    // هذا يمنع خطأ "Address already in use" عند إعادة تشغيل الخادم بسرعة
    int opt = 1;
#ifdef _WIN32
    setsockopt((SOCKET)server_socket_, SOL_SOCKET, SO_REUSEADDR,
               (const char*)&opt, sizeof(opt));
#else
    setsockopt((int)server_socket_, SOL_SOCKET, SO_REUSEADDR,
               &opt, sizeof(opt));
#endif

    // الخطوة ٤: ربط المقبس بالعنوان والمنفذ
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(port_));

    // تحويل العنوان النصي إلى ثنائي
    if (host_ == "0.0.0.0") {
        addr.sin_addr.s_addr = INADDR_ANY; // الاستماع على جميع العناوين
    } else {
        inet_pton(AF_INET, host_.c_str(), &addr.sin_addr);
    }

    if (bind((int)server_socket_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "  ✗ فشل ربط المقبس بالمنفذ " << port_ << "\n";
        CLOSE_SOCKET(server_socket_);
        return false;
    }

    // الخطوة ٥: بدء الاستماع
    // ١٢٨ = الحد الأقصى لقائمة انتظار الاتصالات
    if (listen((int)server_socket_, 128) < 0) {
        std::cerr << "  ✗ فشل بدء الاستماع\n";
        CLOSE_SOCKET(server_socket_);
        return false;
    }

    // الخطوة ٦: حلقة قبول الاتصالات
    running_ = true;

    while (running_.load()) {
        // هيكل عنوان العميل
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);

        // قبول اتصال وارد (يحجز حتى وصول اتصال)
#ifdef _WIN32
        uintptr_t client_socket = (uintptr_t)accept(
            (SOCKET)server_socket_, (struct sockaddr*)&client_addr, &client_len);
#else
        uintptr_t client_socket = (uintptr_t)accept(
            (int)server_socket_, (struct sockaddr*)&client_addr,
            (socklen_t*)&client_len);
#endif

        // التحقق من صحة المقبس
        if (client_socket == INVALID_SOCK) {
            if (running_.load()) {
                // خطأ حقيقي أثناء التشغيل - قد يكون بسبب امتلاء الموارد
                std::cerr << "  ⚠ فشل قبول اتصال\n";
            }
            // إذا !running_ فالخادم يُوقف - عادي
            continue;
        }

        // استخراج عنوان IP العميل
        char ip_buf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip_buf, sizeof(ip_buf));
        std::string client_ip = ip_buf;

        // الخطوة ٧: إنشاء خيط لمعالجة الاتصال
        // نستخدم خيطاً منفصلاً (detached) لكل اتصال
        // هذا يسمح بمعالجة عدة طلبات بالتوازي
        std::thread(&HttpServer::handle_connection, this,
                    client_socket, client_ip).detach();
    }

    // إغلاق مقبس الخادم عند التوقف
    CLOSE_SOCKET(server_socket_);
    server_socket_ = INVALID_SOCK;

    return true;
}

/**
 * @brief إيقاف الخادم بأمان
 *
 * يُعلّم الخادم للتوقف ثم يُغلق المقبس.
 * إغلاق المقبس يُنهي استدعاء accept() المحجوز في start().
 */
void HttpServer::stop() {
    running_ = false;
    if (server_socket_ != INVALID_SOCK) {
        CLOSE_SOCKET(server_socket_);
        server_socket_ = INVALID_SOCK;
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// معالجة الاتصالات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief معالجة اتصال عميل واحد
 *
 * الخطوات:
 *   ١. قراءة البيانات الخام من المقبس
 *   ٢. تحليل الطلب (parse_request)
 *   ٣. معالجة طلبات OPTIONS (CORS preflight) تلقائياً
 *   ٤. البحث عن معالج مطابق في جدول المسارات
 *   ٥. إذا لم يُوجد: محاولة تقديم ملف ثابت
 *   ٦. إذا لم يُوجد ملف: إرجاع 404
 *   ٧. إرسال الاستجابة وإغلاق الاتصال
 *
 * @param client_socket مقبس العميل
 * @param client_addr عنوان IP العميل
 */
void HttpServer::handle_connection(uintptr_t client_socket,
                                   const std::string& client_addr) {
    // ─────────────────────────────────────────────────────────────────
    // الخطوة ١: قراءة البيانات الخام
    // ─────────────────────────────────────────────────────────────────
    // نقرأ حتى ١٦ كيلوبايت - يكفي لمعظم الطلبات
    // للطلبات الأكبر (مثل رفع الحزم)، نقرأ بناءً على Content-Length
    const size_t INITIAL_BUF_SIZE = 16384;
    std::string raw_data;
    raw_data.resize(INITIAL_BUF_SIZE);

    int bytes_received = recv((int)client_socket, &raw_data[0],
                              (int)INITIAL_BUF_SIZE, 0);

    if (bytes_received <= 0) {
        // فشل القراءة أو أغلق العميل الاتصال فوراً
        CLOSE_SOCKET(client_socket);
        return;
    }
    raw_data.resize(bytes_received);

    // ─────────────────────────────────────────────────────────────────
    // الخطوة ١.٥: قراءة بقية الجسم إذا كان هناك Content-Length أكبر
    // ─────────────────────────────────────────────────────────────────
    // نبحث عن فاصل الرأس والجسم
    size_t header_end = raw_data.find("\r\n\r\n");
    if (header_end != std::string::npos) {
        // نبحث عن Content-Length في الرؤوس
        std::string headers_part = raw_data.substr(0, header_end);
        size_t cl_pos = headers_part.find("Content-Length:");
        if (cl_pos == std::string::npos) {
            cl_pos = headers_part.find("content-length:");
        }

        if (cl_pos != std::string::npos) {
            size_t cl_end = headers_part.find("\r\n", cl_pos);
            std::string cl_str = headers_part.substr(cl_pos + 16,
                                    cl_end - cl_pos - 16);
            // إزالة المسافات
            while (!cl_str.empty() && cl_str[0] == ' ') cl_str.erase(0, 1);

            size_t content_length = std::stoull(cl_str);
            size_t body_start = header_end + 4;
            size_t body_received = raw_data.size() - body_start;

            // قراءة بقية الجسم إذا لم نستقبله كاملاً
            while (body_received < content_length && body_received < 50 * 1024 * 1024) {
                // الحد الأقصى ٥٠ ميغابايت (لملفات الحزم)
                char buf[8192];
                int n = recv((int)client_socket, buf, sizeof(buf), 0);
                if (n <= 0) break;
                raw_data.append(buf, n);
                body_received += n;
            }
        }
    }

    // ─────────────────────────────────────────────────────────────────
    // الخطوة ٢: تحليل الطلب
    // ─────────────────────────────────────────────────────────────────
    HttpRequest request = parse_request(raw_data, client_addr);

    // ─────────────────────────────────────────────────────────────────
    // الخطوة ٣: معالجة OPTIONS (CORS preflight)
    // ─────────────────────────────────────────────────────────────────
    // المتصفحات ترسل طلب OPTIONS قبل الطلبات العابرة للنطاقات
    HttpResponse response;
    if (request.method == "OPTIONS") {
        response.status_code = 204; // لا محتوى
        response.headers["Access-Control-Allow-Origin"] = "*";
        response.headers["Access-Control-Allow-Methods"] = "GET, POST, PUT, DELETE, PATCH, OPTIONS";
        response.headers["Access-Control-Allow-Headers"] = "Content-Type, Authorization";
        response.headers["Access-Control-Max-Age"] = "86400"; // يوم واحد

        std::string resp_str = response.build();
        send((int)client_socket, resp_str.c_str(), (int)resp_str.size(), 0);
        CLOSE_SOCKET(client_socket);
        return;
    }

    // ─────────────────────────────────────────────────────────────────
    // الخطوة ٤: البحث عن معالج في جدول المسارات
    // ─────────────────────────────────────────────────────────────────
    std::map<std::string, std::string> path_params;
    const Route* matched_route = match_route(request.method, request.path, path_params);

    if (matched_route) {
        // وُجد معالج - نملأ معاملات المسار وننفذ المعالج
        request.path_params = path_params;

        try {
            response = matched_route->handler(request);
        } catch (const std::exception& e) {
            // التقاط أي استثناء من المعالج وإرجاع خطأ ٥٠٠
            std::cerr << "  ⚠ خطأ في معالجة " << request.method << " "
                      << request.path << ": " << e.what() << "\n";
            response = HttpResponse::internal_error(
                std::string("خطأ داخلي: ") + e.what());
        }
    }
    // ─────────────────────────────────────────────────────────────────
    // الخطوة ٥: محاولة تقديم ملف ثابت
    // ─────────────────────────────────────────────────────────────────
    else if (request.method == "GET" && !static_dir_.empty()) {
        response = serve_static_file(request.path);
    }
    // ─────────────────────────────────────────────────────────────────
    // الخطوة ٦: لا يوجد معالج ولا ملف - 404
    // ─────────────────────────────────────────────────────────────────
    else {
        response = HttpResponse::not_found("المسار غير موجود: " + request.path);
    }

    // ─────────────────────────────────────────────────────────────────
    // الخطوة ٧: إرسال الاستجابة
    // ─────────────────────────────────────────────────────────────────
    std::string resp_str = response.build();
    send((int)client_socket, resp_str.c_str(), (int)resp_str.size(), 0);

    // طباعة سجل الطلب (بتنسيق مبسط)
    std::cout << "  " << request.method << " " << request.path
              << " → " << response.status_code << "\n";

    // ─────────────────────────────────────────────────────────────────
    // إغلاق الاتصال
    // ─────────────────────────────────────────────────────────────────
    CLOSE_SOCKET(client_socket);
}

// ═══════════════════════════════════════════════════════════════════════════════
// تحليل الطلبات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief تحليل طلب HTTP خام
 *
 * شكل الطلب الخام:
 *   GET /api/v1/packages?q=شبكة HTTP/1.1\r\n
 *   Host: localhost:8080\r\n
 *   Content-Type: application/json\r\n
 *   \r\n
 *   {"data":"value"}
 *
 * نستخرج منه:
 *   method = "GET"
 *   path = "/api/v1/packages"
 *   query_params["q"] = "شبكة"
 *   headers["Host"] = "localhost:8080"
 *   body = "{\"data\":\"value\"}"
 */
HttpRequest HttpServer::parse_request(const std::string& raw,
                                      const std::string& client_addr) {
    HttpRequest req;
    req.remote_addr = client_addr;

    // ─────────────────────────────────────────────────────────────────
    // تقسيم الطلب إلى سطور
    // ─────────────────────────────────────────────────────────────────
    size_t pos = 0;
    std::vector<std::string> lines;

    while (pos < raw.size()) {
        size_t end = raw.find("\r\n", pos);
        if (end == std::string::npos) {
            lines.push_back(raw.substr(pos));
            break;
        }
        lines.push_back(raw.substr(pos, end - pos));
        pos = end + 2;
    }

    if (lines.empty()) return req;

    // ─────────────────────────────────────────────────────────────────
    // تحليل السطر الأول: الفعل والمسار والإصدار
    // مثال: "GET /api/v1/packages?q=test HTTP/1.1"
    // ─────────────────────────────────────────────────────────────────
    std::istringstream first_line(lines[0]);
    std::string http_version;
    first_line >> req.method >> req.raw_url >> http_version;

    // فصل المسار عن معاملات الاستعلام
    size_t query_pos = req.raw_url.find('?');
    if (query_pos != std::string::npos) {
        // تحليل معاملات الاستعلام
        std::string query_string = req.raw_url.substr(query_pos + 1);
        req.path = url_decode(req.raw_url.substr(0, query_pos));

        // تقسيم المعاملات بـ &
        std::istringstream qss(query_string);
        std::string param;
        while (std::getline(qss, param, '&')) {
            size_t eq = param.find('=');
            if (eq != std::string::npos) {
                std::string key = url_decode(param.substr(0, eq));
                std::string value = url_decode(param.substr(eq + 1));
                req.query_params[key] = value;
            }
        }
    } else {
        req.path = url_decode(req.raw_url);
    }

    // ─────────────────────────────────────────────────────────────────
    // تحليل الرؤوس
    // ─────────────────────────────────────────────────────────────────
    size_t line_idx = 1;
    while (line_idx < lines.size() && !lines[line_idx].empty()) {
        const std::string& line = lines[line_idx];
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string name = line.substr(0, colon);
            std::string value = line.substr(colon + 1);
            // إزالة المسافات الأمامية من القيمة
            while (!value.empty() && value[0] == ' ') value.erase(0, 1);
            req.headers[name] = value;
        }
        line_idx++;
    }

    // ─────────────────────────────────────────────────────────────────
    // استخراج الجسم (بعد السطر الفارغ)
    // ─────────────────────────────────────────────────────────────────
    size_t body_start = raw.find("\r\n\r\n");
    if (body_start != std::string::npos) {
        req.body = raw.substr(body_start + 4);
    }

    return req;
}

// ═══════════════════════════════════════════════════════════════════════════════
// مطابقة المسارات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief مطابقة مسار الطلب مع المسارات المسجلة
 *
 * تدعم معاملات المسار (مثل :name و :version):
 *   نمط مسجل: /api/v1/packages/:name/:version
 *   طلب وارد: /api/v1/packages/json/1.2.3
 *   النتيجة: مطابقة! params["name"]="json", params["version"]="1.2.3"
 *
 * قواعد المطابقة:
 *   - يجب أن يتطابق عدد الأجزاء
 *   - الأجزاء الثابتة يجب أن تتطابق بالضبط
 *   - الأجزاء التي تبدأ بـ : تطابق أي قيمة وتُخزن كمعامل
 */
const HttpServer::Route* HttpServer::match_route(
        const std::string& method,
        const std::string& path,
        std::map<std::string, std::string>& params) {

    std::vector<std::string> request_parts = split_path(path);

    std::lock_guard<std::mutex> lock(routes_mutex_);

    for (const auto& route : routes_) {
        // التحقق من الفعل
        if (route.method != method) continue;

        // التحقق من عدد الأجزاء
        if (route.parts.size() != request_parts.size()) continue;

        // مطابقة كل جزء
        bool match = true;
        std::map<std::string, std::string> temp_params;

        for (size_t i = 0; i < route.parts.size(); i++) {
            if (route.parts[i][0] == ':') {
                // هذا معامل متغير - نحفظ القيمة
                temp_params[route.parts[i].substr(1)] = request_parts[i];
            } else if (route.parts[i] != request_parts[i]) {
                // جزء ثابت لا يطابق
                match = false;
                break;
            }
        }

        if (match) {
            params = temp_params;
            return &route;
        }
    }

    return nullptr; // لا يوجد مسار مطابق
}

// ═══════════════════════════════════════════════════════════════════════════════
// تقديم الملفات الثابتة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief تقديم ملف ثابت من مجلد الويب
 *
 * الخطوات:
 *   ١. تحويل المسار إلى مسار ملف محلي
 *   ٢. إذا كان المسار "/" أو فارغاً → نقدم index.html
 *   ٣. التحقق من عدم الخروج من المجلد (أمان: path traversal)
 *   ٤. قراءة الملف وتحديد نوع المحتوى
 *   ٥. إرجاع الملف أو 404
 */
HttpResponse HttpServer::serve_static_file(const std::string& path) {
    namespace fs = std::filesystem;

    // تحويل المسار
    std::string file_path = path;
    if (file_path == "/" || file_path.empty()) {
        file_path = "/index.html";
    }

    // إزالة / الأولى
    if (!file_path.empty() && file_path[0] == '/') {
        file_path = file_path.substr(1);
    }

    // بناء المسار الكامل
    fs::path full_path = fs::path(static_dir_) / file_path;

    // التحقق الأمني: منع path traversal (مثل ../../etc/passwd)
    // نتأكد أن المسار الحقيقي يقع داخل المجلد الثابت
    try {
        fs::path canonical_base = fs::weakly_canonical(fs::path(static_dir_));
        fs::path canonical_file = fs::weakly_canonical(full_path);
        std::string base_str = canonical_base.string();
        std::string file_str = canonical_file.string();

        if (file_str.substr(0, base_str.size()) != base_str) {
            // محاولة وصول خارج المجلد المسموح!
            return HttpResponse::forbidden("وصول غير مسموح");
        }
    } catch (...) {
        // خطأ في معالجة المسار
    }

    // التحقق من وجود الملف
    if (!fs::exists(full_path) || fs::is_directory(full_path)) {
        // محاولة إضافة .html
        full_path = fs::path(static_dir_) / (file_path + ".html");
        if (!fs::exists(full_path)) {
            // آخر محاولة: index.html في المجلد
            full_path = fs::path(static_dir_) / file_path / "index.html";
            if (!fs::exists(full_path)) {
                return HttpResponse::not_found("الملف غير موجود: " + path);
            }
        }
    }

    // قراءة الملف
    std::ifstream file(full_path, std::ios::binary);
    if (!file.is_open()) {
        return HttpResponse::internal_error("فشل قراءة الملف");
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    file.close();

    // بناء الاستجابة مع نوع المحتوى الصحيح
    HttpResponse response;
    response.status_code = 200;
    response.body = content;
    response.headers["Content-Type"] = get_content_type(full_path.string());

    // رأس التخزين المؤقت (ساعة واحدة للملفات الثابتة)
    response.headers["Cache-Control"] = "public, max-age=3600";

    return response;
}

// ═══════════════════════════════════════════════════════════════════════════════
// دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief تحديد نوع المحتوى (MIME type) من امتداد الملف
 *
 * يدعم أنواع الملفات الشائعة في تطبيقات الويب.
 * إذا لم يُعرف الامتداد، يُرجع application/octet-stream.
 */
std::string HttpServer::get_content_type(const std::string& path) {
    // استخراج الامتداد
    size_t dot = path.rfind('.');
    if (dot == std::string::npos) return "application/octet-stream";

    std::string ext = path.substr(dot);
    // تحويل إلى أحرف صغيرة
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // جدول أنواع المحتوى
    static const std::map<std::string, std::string> types = {
        // صفحات ويب
        {".html", "text/html; charset=utf-8"},
        {".htm",  "text/html; charset=utf-8"},
        {".css",  "text/css; charset=utf-8"},
        {".js",   "application/javascript; charset=utf-8"},
        {".mjs",  "application/javascript; charset=utf-8"},

        // بيانات
        {".json", "application/json; charset=utf-8"},
        {".xml",  "application/xml; charset=utf-8"},
        {".toml", "application/toml; charset=utf-8"},
        {".yaml", "text/yaml; charset=utf-8"},
        {".yml",  "text/yaml; charset=utf-8"},
        {".csv",  "text/csv; charset=utf-8"},

        // صور
        {".png",  "image/png"},
        {".jpg",  "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".gif",  "image/gif"},
        {".svg",  "image/svg+xml"},
        {".ico",  "image/x-icon"},
        {".webp", "image/webp"},

        // خطوط
        {".woff",  "font/woff"},
        {".woff2", "font/woff2"},
        {".ttf",   "font/ttf"},
        {".otf",   "font/otf"},
        {".eot",   "application/vnd.ms-fontobject"},

        // أرشيفات
        {".zip",   "application/zip"},
        {".gz",    "application/gzip"},
        {".tar",   "application/x-tar"},

        // نصوص
        {".txt",   "text/plain; charset=utf-8"},
        {".md",    "text/markdown; charset=utf-8"},
        {".sad",   "text/plain; charset=utf-8"},
        {".s",     "text/plain; charset=utf-8"},

        // أخرى
        {".wasm",  "application/wasm"},
        {".pdf",   "application/pdf"},
        {".map",   "application/json"},
    };

    auto it = types.find(ext);
    return it != types.end() ? it->second : "application/octet-stream";
}

/**
 * @brief فك ترميز URL
 *
 * يحوّل الأحرف المرمّزة بعلامة % إلى أحرفها الأصلية.
 * مثلاً: %D8%B4%D8%A8%D9%83%D8%A9 → شبكة
 *         %20 → مسافة
 *         %2F → /
 *
 * هذا ضروري لدعم المسارات والبحث بالعربية.
 */
std::string HttpServer::url_decode(const std::string& encoded) {
    std::string result;
    result.reserve(encoded.size());

    for (size_t i = 0; i < encoded.size(); i++) {
        if (encoded[i] == '%' && i + 2 < encoded.size()) {
            // تحويل الرمز السداسي عشري إلى بايت
            char hex[3] = { encoded[i + 1], encoded[i + 2], '\0' };
            char decoded = (char)strtol(hex, nullptr, 16);
            result += decoded;
            i += 2;
        } else if (encoded[i] == '+') {
            // في معاملات الاستعلام، + = مسافة
            result += ' ';
        } else {
            result += encoded[i];
        }
    }

    return result;
}

/**
 * @brief تقسيم مسار URL بعلامة /
 *
 * يُزيل الأجزاء الفارغة.
 * مثال: "/api/v1/packages" → ["api", "v1", "packages"]
 */
std::vector<std::string> HttpServer::split_path(const std::string& path) {
    std::vector<std::string> parts;
    std::istringstream iss(path);
    std::string part;

    while (std::getline(iss, part, '/')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }

    return parts;
}

} // namespace registry
} // namespace sad
