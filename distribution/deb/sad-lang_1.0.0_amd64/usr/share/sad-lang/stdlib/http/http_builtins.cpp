// بسم الله الرحمن الرحيم
/**
 * @file http_builtins.cpp
 * @brief HTTP Builtin Functions for Sad Interpreter - دوال HTTP المدمجة للمفسر
 * 
 * @author Sad Language Team
 * @date December 2025
 * 
 * الحمد لله رب العالمين
 */

#include "http/http_module.h"
#include "data/types/value.h"
#include <memory>

namespace sad {
namespace stdlib {
namespace http {

using namespace Sad::Data;

// ============================================================================
// Helper Functions - دوال مساعدة
// ============================================================================

namespace {
    std::string get_string_arg(const std::vector<Value>& args, size_t index, const std::string& name) {
        if (index >= args.size()) {
            throw std::runtime_error(name + ": Missing argument at index " + std::to_string(index));
        }
        if (!args[index].isString()) {
            throw std::runtime_error(name + ": Expected string argument at index " + std::to_string(index));
        }
        return args[index].toString();
    }

    int64_t get_int_arg(const std::vector<Value>& args, size_t index, int64_t default_value = 0) {
        if (index >= args.size()) {
            return default_value;
        }
        if (!args[index].isInteger()) {
            return default_value;
        }
        return args[index].toInt();
    }

    /**
     * @brief تحويل Value إلى std::map / Convert Value to std::map
     * @param val قيمة من نوع MAP / Value of MAP type
     * @return خريطة من السلاسل النصية / Map of strings
     */
    std::map<std::string, std::string> value_to_map(const Value& val) {
        std::map<std::string, std::string> result;
        
        if (val.isMap()) {
            // استخراج جميع أزواج المفتاح-القيمة / Extract all key-value pairs
            auto map = val.toMap();
            for (const auto& [key, value] : map) {
                // تحويل كل قيمة إلى نص / Convert each value to string
                result[key] = value.toString();
            }
        }
        
        return result;
    }

    /**
     * @brief تحويل std::map إلى Value / Convert std::map to Value
     * @param map خريطة من السلاسل النصية / Map of strings
     * @return قيمة من نوع MAP / Value of MAP type
     */
    Value map_to_value(const std::map<std::string, std::string>& map) {
        // إنشاء MapType وملؤها / Create and populate MapType
        Value::MapType valueMap;
        for (const auto& [key, value] : map) {
            valueMap[key] = Value(value);
        }
        return Value(valueMap);
    }

    /**
     * @brief تحويل HttpResponse إلى Value / Convert HttpResponse to Value
     * @param response استجابة HTTP / HTTP response
     * @return قيمة من نوع MAP تحتوي على جميع حقول الاستجابة / MAP Value containing all response fields
     * 
     * @details
     * (AR) يحول استجابة HTTP إلى كائن قاموس يحتوي على:
     *      - status: رمز الحالة (عدد صحيح)
     *      - headers: ترويسات HTTP (قاموس)
     *      - body: جسم الاستجابة (نص)
     *      - cookies: مصفوفة الكوكيز (مصفوفة من القواميس)
     * 
     * (EN) Converts HTTP response to dictionary object containing:
     *      - status: Status code (integer)
     *      - headers: HTTP headers (map)
     *      - body: Response body (string)
     *      - cookies: Cookie array (array of maps)
     */
    Value response_to_value(const HttpResponse& response) {
        Value::MapType result;
        
        // رمز الحالة / Status code
        result["status"] = Value(static_cast<int>(response.get_status()));
        
        // الترويسات / Headers
        Value::MapType headersMap;
        for (const auto& [key, value] : response.get_headers().all()) {
            headersMap[key] = Value(value);
        }
        result["headers"] = Value(headersMap);
        
        // الجسم / Body
        result["body"] = Value(response.get_body());
        
        // الكوكيز / Cookies
        Value::ArrayType cookiesArray;
        for (const auto& cookie : response.get_cookies()) {
            Value::MapType cookieMap;
            cookieMap["name"] = Value(cookie.name);
            cookieMap["value"] = Value(cookie.value);
            cookieMap["domain"] = Value(cookie.domain);
            cookieMap["path"] = Value(cookie.path);
            cookieMap["max_age"] = Value(static_cast<int>(cookie.max_age));
            cookieMap["secure"] = Value(cookie.secure);
            cookieMap["http_only"] = Value(cookie.http_only);
            cookieMap["same_site"] = Value(cookie.same_site);
            
            cookiesArray.push_back(Value(cookieMap));
        }
        result["cookies"] = Value(cookiesArray);
        
        return Value(result);
    }

    // عميل HTTP عام / Global HTTP client
    static HttpClient g_http_client;
}

// ============================================================================
// HTTP Client Functions - دوال عميل HTTP
// ============================================================================

/**
 * @brief طلب GET / GET request
 * @example
 * استجابة = http_get("https://api.example.com/data")
 * اطبع(استجابة.body)
 */
Value builtin_http_get(const std::vector<Value>& args) {
    std::string url = get_string_arg(args, 0, "http_get");
    
    try {
        HttpResponse response = g_http_client.get(url);
        return response_to_value(response);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("http_get: ") + e.what());
    }
}

/**
 * @brief طلب POST / POST request
 * @example
 * بيانات = "{\"name\": \"أحمد\"}"
 * استجابة = http_post("https://api.example.com/users", بيانات)
 */
Value builtin_http_post(const std::vector<Value>& args) {
    std::string url = get_string_arg(args, 0, "http_post");
    std::string body = get_string_arg(args, 1, "http_post");
    
    try {
        HttpResponse response = g_http_client.post(url, body);
        return response_to_value(response);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("http_post: ") + e.what());
    }
}

/**
 * @brief طلب PUT / PUT request
 */
Value builtin_http_put(const std::vector<Value>& args) {
    std::string url = get_string_arg(args, 0, "http_put");
    std::string body = get_string_arg(args, 1, "http_put");
    
    try {
        HttpResponse response = g_http_client.put(url, body);
        return response_to_value(response);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("http_put: ") + e.what());
    }
}

/**
 * @brief طلب DELETE / DELETE request
 */
Value builtin_http_delete(const std::vector<Value>& args) {
    std::string url = get_string_arg(args, 0, "http_delete");
    
    try {
        HttpResponse response = g_http_client.del(url);
        return response_to_value(response);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("http_delete: ") + e.what());
    }
}

/**
 * @brief طلب PATCH / PATCH request
 */
Value builtin_http_patch(const std::vector<Value>& args) {
    std::string url = get_string_arg(args, 0, "http_patch");
    std::string body = get_string_arg(args, 1, "http_patch");
    
    try {
        HttpResponse response = g_http_client.patch(url, body);
        return response_to_value(response);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("http_patch: ") + e.what());
    }
}

/**
 * @brief طلب HEAD / HEAD request
 */
Value builtin_http_head(const std::vector<Value>& args) {
    std::string url = get_string_arg(args, 0, "http_head");
    
    try {
        HttpResponse response = g_http_client.head(url);
        return response_to_value(response);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("http_head: ") + e.what());
    }
}

// ============================================================================
// URL Utility Functions - دوال مساعدة URL
// ============================================================================

/**
 * @brief تشفير URL / URL encode
 * @example
 * مشفر = url_encode("مرحبا بالعالم")  # => "%D9%85%D8%B1%D8%AD%D8%A8%D8%A7..."
 */
Value builtin_url_encode(const std::vector<Value>& args) {
    std::string str = get_string_arg(args, 0, "url_encode");
    
    try {
        std::string encoded = url_encode(str);
        return Value(encoded);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("url_encode: ") + e.what());
    }
}

/**
 * @brief فك تشفير URL / URL decode
 */
Value builtin_url_decode(const std::vector<Value>& args) {
    std::string str = get_string_arg(args, 0, "url_decode");
    
    try {
        std::string decoded = url_decode(str);
        return Value(decoded);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("url_decode: ") + e.what());
    }
}

/**
 * @brief تحليل URL / Parse URL
 * @example
 * أجزاء = url_parse("https://example.com:8080/path?key=value#section")
 * اطبع(أجزاء.host)    # => "example.com"
 * اطبع(أجزاء.port)    # => 8080
 */
Value builtin_url_parse(const std::vector<Value>& args) {
    std::string url = get_string_arg(args, 0, "url_parse");
    
    try {
        UrlParts parts = parse_url(url);
        
        // تحويل UrlParts إلى Value map object
        // Convert UrlParts to Value map object
        Value::MapType result;
        result["scheme"] = Value(parts.scheme);
        result["host"] = Value(parts.host);
        result["port"] = Value(static_cast<int>(parts.port));
        result["path"] = Value(parts.path);
        result["query"] = Value(parts.query);
        result["fragment"] = Value(parts.fragment);
        
        return Value(result);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("url_parse: ") + e.what());
    }
}

/**
 * @brief بناء URL / Build URL
 * @example
 * أجزاء = {
 *     "scheme": "https",
 *     "host": "example.com",
 *     "port": 8080,
 *     "path": "/api/users",
 *     "query": "page=1",
 *     "fragment": "section"
 * }
 * رابط = url_build(أجزاء)
 * اطبع(رابط)  # => "https://example.com:8080/api/users?page=1#section"
 */
Value builtin_url_build(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("url_build: Missing argument");
    }
    
    if (!args[0].isMap()) {
        throw std::runtime_error("url_build: Expected map argument");
    }
    
    try {
        // تحويل Value map إلى UrlParts / Convert Value map to UrlParts
        auto map = args[0].toMap();
        UrlParts parts;
        
        // استخراج الحقول / Extract fields
        if (map.count("scheme")) {
            parts.scheme = map.at("scheme").toString();
        }
        if (map.count("host")) {
            parts.host = map.at("host").toString();
        }
        if (map.count("port")) {
            parts.port = static_cast<uint16_t>(map.at("port").toInt());
        }
        if (map.count("path")) {
            parts.path = map.at("path").toString();
        }
        if (map.count("query")) {
            parts.query = map.at("query").toString();
        }
        if (map.count("fragment")) {
            parts.fragment = map.at("fragment").toString();
        }
        
        // بناء الـ URL / Build the URL
        std::string url = build_url(parts);
        return Value(url);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("url_build: ") + e.what());
    }
}

/**
 * @brief تحليل معاملات الاستعلام / Parse query parameters
 * @example
 * معاملات = query_parse("name=أحمد&age=25")
 * اطبع(معاملات["name"])  # => "أحمد"
 */
Value builtin_query_parse(const std::vector<Value>& args) {
    std::string query = get_string_arg(args, 0, "query_parse");
    
    try {
        auto params = parse_query_string(query);
        return map_to_value(params);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("query_parse: ") + e.what());
    }
}

/**
 * @brief بناء سلسلة استعلام / Build query string
 */
Value builtin_query_build(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("query_build: Missing argument");
    }
    
    try {
        auto params = value_to_map(args[0]);
        std::string query = build_query_string(params);
        return Value(query);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("query_build: ") + e.what());
    }
}

// ============================================================================
// HTTP Client Configuration - إعدادات عميل HTTP
// ============================================================================

/**
 * @brief تعيين مهلة الاتصال / Set connection timeout
 * @example
 * http_set_timeout(30)  # 30 ثانية
 */
Value builtin_http_set_timeout(const std::vector<Value>& args) {
    int64_t timeout = get_int_arg(args, 0, 30);
    
    g_http_client.set_timeout(static_cast<int>(timeout));
    
    return Value();  // VOID
}

/**
 * @brief تعيين User-Agent / Set User-Agent
 */
Value builtin_http_set_user_agent(const std::vector<Value>& args) {
    std::string user_agent = get_string_arg(args, 0, "http_set_user_agent");
    
    g_http_client.set_user_agent(user_agent);
    
    return Value();  // VOID
}

/**
 * @brief تعيين ترويسة افتراضية / Set default header
 */
Value builtin_http_set_header(const std::vector<Value>& args) {
    std::string name = get_string_arg(args, 0, "http_set_header");
    std::string value = get_string_arg(args, 1, "http_set_header");
    
    g_http_client.set_default_header(name, value);
    
    return Value();  // VOID
}

// ============================================================================
// HTTP Server Functions - دوال خادم HTTP
// ============================================================================

// خادم HTTP عام / Global HTTP server
static std::unique_ptr<HttpServer> g_http_server;

/**
 * @brief إنشاء خادم HTTP / Create HTTP server
 * @example
 * http_server_create()
 */
Value builtin_http_server_create(const std::vector<Value>& args) {
    g_http_server = std::make_unique<HttpServer>();
    
    return Value();  // VOID
}

/**
 * @brief تسجيل معالج GET / Register GET handler
 * @example
 * http_server_get("/api/users", دالة(طلب):
 *     أرجع "{\"users\": []}"
 * )
 */
Value builtin_http_server_get(const std::vector<Value>& args) {
    if (!g_http_server) {
        throw std::runtime_error("http_server_get: Server not created. Call http_server_create() first");
    }
    
    std::string path = get_string_arg(args, 0, "http_server_get");
    
    // TODO: استخراج دالة المعالج من args[1]
    // TODO: تحويل Value function إلى RouteHandler
    // Need to extract function from Value and wrap it as C++ lambda
    // RouteHandler signature: HttpResponse(const HttpRequest&)
    
    return Value();  // VOID
}

/**
 * @brief تسجيل معالج POST / Register POST handler
 */
Value builtin_http_server_post(const std::vector<Value>& args) {
    if (!g_http_server) {
        throw std::runtime_error("http_server_post: Server not created");
    }
    
    std::string path = get_string_arg(args, 0, "http_server_post");
    
    // TODO: تحويل دالة المعالج من Value إلى RouteHandler
    
    return Value();  // VOID
}

/**
 * @brief بدء الخادم / Start server
 * @example
 * http_server_listen(8080)
 * اطبع("الخادم يعمل على المنفذ 8080")
 */
Value builtin_http_server_listen(const std::vector<Value>& args) {
    if (!g_http_server) {
        throw std::runtime_error("http_server_listen: Server not created");
    }
    
    int64_t port = get_int_arg(args, 0, 8080);
    
    try {
        g_http_server->listen(static_cast<uint16_t>(port));
        return Value();  // VOID
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("http_server_listen: ") + e.what());
    }
}

/**
 * @brief إيقاف الخادم / Stop server
 */
Value builtin_http_server_stop(const std::vector<Value>& args) {
    if (!g_http_server) {
        throw std::runtime_error("http_server_stop: Server not created");
    }
    
    g_http_server->stop();
    
    return Value();  // VOID
}

/**
 * @brief فحص إذا كان الخادم يعمل / Check if server is running
 */
Value builtin_http_server_is_running(const std::vector<Value>& args) {
    bool isRunning = g_http_server && g_http_server->is_running();
    return Value(isRunning);
}

// ============================================================================
// Registration Function - دالة التسجيل
// ============================================================================

void register_http_functions(sad::interpreter::Interpreter& interp) {
    // HTTP Client - عميل HTTP
    interp.register_builtin("http_get", builtin_http_get);
    interp.register_builtin("اطلب_get", builtin_http_get);
    
    interp.register_builtin("http_post", builtin_http_post);
    interp.register_builtin("اطلب_post", builtin_http_post);
    
    interp.register_builtin("http_put", builtin_http_put);
    interp.register_builtin("اطلب_put", builtin_http_put);
    
    interp.register_builtin("http_delete", builtin_http_delete);
    interp.register_builtin("اطلب_delete", builtin_http_delete);
    
    interp.register_builtin("http_patch", builtin_http_patch);
    interp.register_builtin("اطلب_patch", builtin_http_patch);
    
    interp.register_builtin("http_head", builtin_http_head);
    interp.register_builtin("اطلب_head", builtin_http_head);
    
    // URL Utilities - مساعدات URL
    interp.register_builtin("url_encode", builtin_url_encode);
    interp.register_builtin("شفر_url", builtin_url_encode);
    
    interp.register_builtin("url_decode", builtin_url_decode);
    interp.register_builtin("فك_تشفير_url", builtin_url_decode);
    
    interp.register_builtin("url_parse", builtin_url_parse);
    interp.register_builtin("حلل_url", builtin_url_parse);
    
    interp.register_builtin("url_build", builtin_url_build);
    interp.register_builtin("ابن_url", builtin_url_build);
    
    interp.register_builtin("query_parse", builtin_query_parse);
    interp.register_builtin("حلل_استعلام", builtin_query_parse);
    
    interp.register_builtin("query_build", builtin_query_build);
    interp.register_builtin("ابن_استعلام", builtin_query_build);
    
    // HTTP Client Configuration - إعدادات العميل
    interp.register_builtin("http_set_timeout", builtin_http_set_timeout);
    interp.register_builtin("عين_مهلة_http", builtin_http_set_timeout);
    
    interp.register_builtin("http_set_user_agent", builtin_http_set_user_agent);
    interp.register_builtin("عين_وكيل_مستخدم", builtin_http_set_user_agent);
    
    interp.register_builtin("http_set_header", builtin_http_set_header);
    interp.register_builtin("عين_ترويسة_http", builtin_http_set_header);
    
    // HTTP Server - خادم HTTP
    interp.register_builtin("http_server_create", builtin_http_server_create);
    interp.register_builtin("أنشئ_خادم_http", builtin_http_server_create);
    
    interp.register_builtin("http_server_get", builtin_http_server_get);
    interp.register_builtin("خادم_get", builtin_http_server_get);
    
    interp.register_builtin("http_server_post", builtin_http_server_post);
    interp.register_builtin("خادم_post", builtin_http_server_post);
    
    interp.register_builtin("http_server_listen", builtin_http_server_listen);
    interp.register_builtin("ابدأ_خادم", builtin_http_server_listen);
    
    interp.register_builtin("http_server_stop", builtin_http_server_stop);
    interp.register_builtin("أوقف_خادم", builtin_http_server_stop);
    
    interp.register_builtin("http_server_is_running", builtin_http_server_is_running);
    interp.register_builtin("هل_خادم_يعمل", builtin_http_server_is_running);
}

} // namespace http
} // namespace stdlib
} // namespace sad

// الحمد لله رب العالمين
