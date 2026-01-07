# بسم الله الرحمن الرحيم
# 📄 Phase 2.1.2 - HTTP Client & Server Implementation Plan
# خطة تنفيذ HTTP Client & Server

**التاريخ / Date:** 7 يناير 2026  
**المرحلة / Phase:** 2.1.2 - HTTP Client & Server  
**الحالة / Status:** ⏳ **مخطط / PLANNED**  
**الأولوية / Priority:** 🔴 **حرجة / CRITICAL**  
**المدة المتوقعة / Duration:** 2-3 أسابيع (الأسبوع 3-4 من Phase 2.1)

---

## 🎯 الهدف الاستراتيجي / Strategic Objective

بناء مكتبة HTTP كاملة تدعم Client & Server مع HTTPS، تمكن من بناء Web APIs و Web Services في لغة Sad.

### لماذا HTTP؟
```
✅ أساسية لأي تطبيق ويب
✅ تمكن بناء REST APIs
✅ أساس Microservices
✅ منافسة مع Express.js, Gin, Actix-web
✅ البناء على Phase 2.1.1 (Sockets)
```

---

## 📊 نظرة عامة / Overview

### المتطلبات الأساسية
```
✅ Phase 2.1.1 مكتمل (TCP/UDP Sockets)
✅ OpenSSL/LibreSSL متوفر (HTTPS)
✅ CMake build system جاهز
✅ Testing framework جاهز
```

### الميزات المستهدفة
```cpp
HTTP Client:
  ✅ GET, POST, PUT, DELETE, PATCH methods
  ✅ Custom headers
  ✅ Query parameters
  ✅ Request body (JSON, form-data, raw)
  ✅ Response parsing
  ✅ HTTPS support
  ✅ Connection pooling
  ✅ Timeout & retry
  ✅ Cookie management
  ✅ Redirect handling

HTTP Server:
  ✅ Routing system (path, method)
  ✅ Middleware chain
  ✅ Request/Response objects
  ✅ Static file serving
  ✅ JSON parsing/serialization
  ✅ Cookie & session management
  ✅ CORS support
  ✅ Compression (gzip)
  ✅ HTTPS support
  ✅ Multi-threading
```

---

## 🏗️ البنية المعمارية / Architecture

### Component Structure
```
HTTP Layer
├── HttpClient
│   ├── Request Building
│   ├── Connection Pool
│   ├── Response Parsing
│   └── Error Handling
│
├── HttpServer
│   ├── Request Parsing
│   ├── Router
│   ├── Middleware Chain
│   ├── Response Building
│   └── Connection Management
│
├── HttpRequest
│   ├── Method, Path, Headers
│   ├── Query Parameters
│   ├── Body (JSON, Form, Raw)
│   └── Cookies
│
├── HttpResponse
│   ├── Status Code
│   ├── Headers
│   ├── Body
│   └── Cookies
│
└── Utilities
    ├── URL Parser
    ├── HTTP Parser
    ├── JSON Parser
    └── Cookie Parser
```

### Class Hierarchy
```
                    HttpBase
                        |
        +---------------+---------------+
        |                               |
   HttpClient                      HttpServer
        |                               |
        |                         HttpRouter
        |                               |
   HttpRequest                   HttpMiddleware
        |                               |
   HttpResponse                  HttpHandler
```

---

## 📁 هيكل الملفات / File Structure

### Header Files (include/network/http/)
```cpp
include/network/http/
├── http_base.h              // Base HTTP utilities
│   - HTTP version enum
│   - HTTP methods enum
│   - Status codes enum
│   - Common headers
│   - URL parsing
│   (~150 lines)
│
├── http_request.h           // HTTP Request representation
│   - class HttpRequest
│   - method(), path(), headers()
│   - query(), body(), cookies()
│   - set_header(), add_query()
│   - json(), form_data()
│   (~200 lines)
│
├── http_response.h          // HTTP Response representation
│   - class HttpResponse
│   - status(), headers(), body()
│   - set_status(), set_header()
│   - json(), html(), text()
│   - redirect(), set_cookie()
│   (~200 lines)
│
├── http_client.h            // HTTP Client
│   - class HttpClient
│   - get(), post(), put(), delete()
│   - request() general method
│   - Connection pooling
│   - Timeout handling
│   - Cookie jar
│   (~250 lines)
│
├── http_server.h            // HTTP Server
│   - class HttpServer
│   - listen(), stop()
│   - get(), post(), put(), delete()
│   - use() for middleware
│   - static_files()
│   - Connection handling
│   (~300 lines)
│
├── http_router.h            // Routing system
│   - class HttpRouter
│   - Route matching
│   - Path parameters (/users/:id)
│   - Route priorities
│   - Nested routes
│   (~250 lines)
│
├── http_middleware.h        // Middleware support
│   - Middleware interface
│   - Middleware chain
│   - Built-in middleware:
│     * Logger
│     * CORS
│     * Compression
│     * Static files
│   (~200 lines)
│
├── http_cookie.h            // Cookie handling
│   - class Cookie
│   - Parsing & serialization
│   - Cookie attributes
│   - Cookie jar
│   (~150 lines)
│
├── http_url.h               // URL utilities
│   - class URL
│   - Parsing
│   - Query string handling
│   - URL encoding/decoding
│   (~150 lines)
│
└── http_parser.h            // HTTP message parser
    - Request parser
    - Response parser
    - Chunked encoding
    - Multipart form-data
    (~300 lines)
```

### Implementation Files (stdlib/network/http/)
```cpp
stdlib/network/http/
├── http_base.cpp            (~200 lines)
├── http_request.cpp         (~350 lines)
├── http_response.cpp        (~350 lines)
├── http_client.cpp          (~600 lines)
├── http_server.cpp          (~800 lines)
├── http_router.cpp          (~450 lines)
├── http_middleware.cpp      (~400 lines)
├── http_cookie.cpp          (~250 lines)
├── http_url.cpp             (~300 lines)
└── http_parser.cpp          (~500 lines)

Total: ~4,200 lines C++
```

### Sad Language Interface
```cpp
stdlib/network/http.s        // Main interface
stdlib/network/http/
├── client.s                 // Client wrapper
├── server.s                 // Server wrapper
└── examples/
    ├── simple_get.s         // GET request example
    ├── post_json.s          // POST JSON example
    ├── simple_server.s      // Basic server
    ├── rest_api.s           // REST API example
    └── static_server.s      // Static file server

Total: ~1,500 lines Sad
```

### Documentation
```markdown
docs/network/ar/http/
├── README.md                // Navigation
├── 01_مقدمة_HTTP.md        // Introduction
├── 02_HTTP_Client.md        // Client guide
├── 03_HTTP_Server.md        // Server guide
├── 04_Routing.md            // Routing system
├── 05_Middleware.md         // Middleware guide
├── 06_أمثلة_REST_API.md    // REST API examples
├── 07_HTTPS.md              // HTTPS/SSL guide
├── 08_ممارسات.md           // Best practices
└── 09_API_Reference.md      // Complete API

Total: ~6,000 lines Arabic docs
```

### Tests
```cpp
tests/network/http/
├── test_http_request.cpp
├── test_http_response.cpp
├── test_http_client.cpp
├── test_http_server.cpp
├── test_http_router.cpp
├── test_http_middleware.cpp
├── test_http_cookie.cpp
├── test_http_url.cpp
└── test_http_parser.cpp

Total: ~2,500 lines tests
```

---

## 📝 تفاصيل التصميم / Design Details

### 1. HttpClient Class

```cpp
// include/network/http/http_client.h
namespace sad::network::http {

enum class HttpMethod {
    GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS
};

enum class HttpVersion {
    HTTP_1_0,
    HTTP_1_1,
    HTTP_2_0
};

class HttpClient {
public:
    HttpClient();
    explicit HttpClient(const std::string& base_url);
    ~HttpClient();

    // Request methods
    HttpResponse get(const std::string& url);
    HttpResponse post(const std::string& url, const std::string& body);
    HttpResponse put(const std::string& url, const std::string& body);
    HttpResponse delete_(const std::string& url);
    HttpResponse patch(const std::string& url, const std::string& body);
    
    // General request
    HttpResponse request(HttpMethod method, const std::string& url,
                         const HttpRequest& req = HttpRequest());
    
    // Configuration
    void set_timeout(int seconds);
    void set_max_redirects(int max);
    void set_verify_ssl(bool verify);
    void set_default_header(const std::string& key, const std::string& value);
    void set_user_agent(const std::string& agent);
    
    // Connection pooling
    void enable_connection_pool(bool enable);
    void set_max_connections(int max);
    
    // Cookie management
    void enable_cookies(bool enable);
    void add_cookie(const Cookie& cookie);
    std::vector<Cookie> get_cookies() const;
    
    // Authentication
    void set_basic_auth(const std::string& username, const std::string& password);
    void set_bearer_token(const std::string& token);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace sad::network::http
```

### 2. HttpServer Class

```cpp
// include/network/http/http_server.h
namespace sad::network::http {

using HttpHandler = std::function<void(const HttpRequest&, HttpResponse&)>;
using Middleware = std::function<void(const HttpRequest&, HttpResponse&, 
                                       std::function<void()> next)>;

class HttpServer {
public:
    HttpServer();
    explicit HttpServer(const std::string& address);
    explicit HttpServer(int port);
    ~HttpServer();

    // Route registration
    void get(const std::string& path, HttpHandler handler);
    void post(const std::string& path, HttpHandler handler);
    void put(const std::string& path, HttpHandler handler);
    void delete_(const std::string& path, HttpHandler handler);
    void patch(const std::string& path, HttpHandler handler);
    void any(const std::string& path, HttpHandler handler);
    
    // Middleware
    void use(Middleware middleware);
    void use(const std::string& path, Middleware middleware);
    
    // Static files
    void static_files(const std::string& route_path, 
                     const std::string& directory);
    
    // Server control
    void listen();
    void listen(int port);
    void listen(const std::string& address);
    void stop();
    bool is_running() const;
    
    // Configuration
    void set_thread_pool_size(int size);
    void set_max_request_size(size_t size);
    void set_timeout(int seconds);
    void enable_cors(bool enable);
    void enable_compression(bool enable);
    
    // HTTPS
    void enable_https(const std::string& cert_file, 
                     const std::string& key_file);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace sad::network::http
```

### 3. HttpRequest Class

```cpp
// include/network/http/http_request.h
namespace sad::network::http {

class HttpRequest {
public:
    HttpRequest();
    HttpRequest(HttpMethod method, const std::string& url);
    
    // Method & URL
    HttpMethod method() const;
    void set_method(HttpMethod method);
    
    std::string url() const;
    void set_url(const std::string& url);
    
    std::string path() const;
    std::string query_string() const;
    
    // Headers
    std::string header(const std::string& key) const;
    void set_header(const std::string& key, const std::string& value);
    void remove_header(const std::string& key);
    std::map<std::string, std::string> headers() const;
    
    // Query parameters
    std::string query(const std::string& key) const;
    void set_query(const std::string& key, const std::string& value);
    std::map<std::string, std::string> queries() const;
    
    // Body
    std::string body() const;
    void set_body(const std::string& body);
    void set_json(const std::string& json);
    void set_form_data(const std::map<std::string, std::string>& data);
    
    // Cookies
    std::string cookie(const std::string& name) const;
    void set_cookie(const Cookie& cookie);
    std::vector<Cookie> cookies() const;
    
    // Content type
    std::string content_type() const;
    void set_content_type(const std::string& type);
    
    // Serialization
    std::string to_string() const;
    static HttpRequest from_string(const std::string& raw);

private:
    HttpMethod method_;
    std::string url_;
    std::map<std::string, std::string> headers_;
    std::map<std::string, std::string> queries_;
    std::string body_;
    std::vector<Cookie> cookies_;
};

} // namespace sad::network::http
```

### 4. HttpResponse Class

```cpp
// include/network/http/http_response.h
namespace sad::network::http {

class HttpResponse {
public:
    HttpResponse();
    explicit HttpResponse(int status_code);
    
    // Status
    int status() const;
    void set_status(int code);
    std::string status_text() const;
    
    // Headers
    std::string header(const std::string& key) const;
    void set_header(const std::string& key, const std::string& value);
    std::map<std::string, std::string> headers() const;
    
    // Body
    std::string body() const;
    void set_body(const std::string& body);
    
    // Convenience methods
    void text(const std::string& text);
    void html(const std::string& html);
    void json(const std::string& json);
    void redirect(const std::string& url, int status = 302);
    void send_file(const std::string& path);
    
    // Cookies
    void set_cookie(const Cookie& cookie);
    std::vector<Cookie> cookies() const;
    
    // Content type
    std::string content_type() const;
    void set_content_type(const std::string& type);
    
    // Serialization
    std::string to_string() const;
    static HttpResponse from_string(const std::string& raw);

private:
    int status_code_;
    std::map<std::string, std::string> headers_;
    std::string body_;
    std::vector<Cookie> cookies_;
};

} // namespace sad::network::http
```

---

## 💻 أمثلة الاستخدام / Usage Examples

### HTTP Client - GET Request
```cpp
// Simple GET
استورد شبكة.http

دالة main() {
    متغير عميل = http.عميل()
    متغير رد = عميل.get("https://api.github.com/users/octocat")
    
    اذا (رد.نجح()) {
        اطبع("النتيجة: " + رد.json())
    } والا {
        اطبع("خطأ: " + رد.خطأ())
    }
}
```

### HTTP Client - POST JSON
```cpp
استورد شبكة.http

دالة create_user() {
    متغير عميل = http.عميل()
    
    متغير بيانات = {
        "اسم": "أحمد",
        "email": "ahmad@example.com",
        "عمر": 25
    }
    
    متغير رد = عميل.post(
        "https://api.example.com/users",
        json: بيانات,
        headers: {
            "Authorization": "Bearer token123",
            "Content-Type": "application/json"
        }
    )
    
    ارجع رد.json()
}
```

### HTTP Server - Simple API
```cpp
استورد شبكة.http

دالة main() {
    متغير خادم = http.خادم(":8080")
    
    // GET /
    خادم.get("/", (req, res) => {
        res.json({
            "رسالة": "مرحباً بك في API!",
            "نسخة": "1.0.0"
        })
    })
    
    // GET /users/:id
    خادم.get("/users/:id", (req, res) => {
        متغير id = req.param("id")
        متغير مستخدم = احصل_على_مستخدم(id)
        
        اذا (مستخدم) {
            res.json(مستخدم)
        } والا {
            res.status(404).json({"خطأ": "مستخدم غير موجود"})
        }
    })
    
    // POST /users
    خادم.post("/users", (req, res) => {
        متغير بيانات = req.json()
        متغير مستخدم = انشئ_مستخدم(بيانات)
        res.status(201).json(مستخدم)
    })
    
    اطبع("الخادم يعمل على http://localhost:8080")
    خادم.استمع()
}
```

### HTTP Server - Middleware
```cpp
استورد شبكة.http

دالة main() {
    متغير خادم = http.خادم(":8080")
    
    // Logger middleware
    خادم.استخدم((req, res, next) => {
        متغير بداية = الوقت.الآن()
        اطبع("طلب: " + req.method + " " + req.path)
        
        next()
        
        متغير مدة = الوقت.الآن() - بداية
        اطبع("اكتمل في " + مدة + "ms")
    })
    
    // Authentication middleware
    خادم.استخدم("/api/*", (req, res, next) => {
        متغير token = req.header("Authorization")
        
        اذا (!تحقق_token(token)) {
            res.status(401).json({"خطأ": "غير مصرح"})
            ارجع
        }
        
        next()
    })
    
    // CORS middleware
    خادم.استخدم((req, res, next) => {
        res.set_header("Access-Control-Allow-Origin", "*")
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE")
        next()
    })
    
    // Routes
    خادم.get("/api/data", (req, res) => {
        res.json({"بيانات": "محمية"})
    })
    
    خادم.استمع()
}
```

### Static File Server
```cpp
استورد شبكة.http

دالة main() {
    متغير خادم = http.خادم(":8080")
    
    // Serve static files
    خادم.ملفات_ثابتة("/", "./public")
    خادم.ملفات_ثابتة("/assets", "./public/assets")
    
    // API routes
    خادم.get("/api/status", (req, res) => {
        res.json({"حالة": "يعمل"})
    })
    
    اطبع("خادم الملفات يعمل على http://localhost:8080")
    خادم.استمع()
}
```

---

## ⏱️ خطة التنفيذ / Implementation Timeline

### Week 1 (8-14 يناير)
```
Day 1-2: HTTP Core
  ✅ HttpBase utilities
  ✅ HttpRequest class
  ✅ HttpResponse class
  ✅ HTTP Parser
  ✅ URL utilities
  ~1,200 lines C++

Day 3-4: HTTP Client
  ✅ HttpClient class
  ✅ Request building
  ✅ Response parsing
  ✅ Connection pooling
  ✅ Error handling
  ~800 lines C++

Day 5-7: Testing & Documentation
  ✅ Unit tests for client
  ✅ Integration tests
  ✅ Arabic documentation
  ✅ Usage examples
  ~1,500 lines total
```

### Week 2 (15-21 يناير)
```
Day 1-2: HTTP Server Core
  ✅ HttpServer class
  ✅ Request parsing
  ✅ Connection handling
  ✅ Multi-threading
  ~1,000 lines C++

Day 3-4: Routing & Middleware
  ✅ HttpRouter class
  ✅ Route matching
  ✅ Middleware chain
  ✅ Built-in middleware
  ~800 lines C++

Day 5-6: Advanced Features
  ✅ Static file serving
  ✅ Cookie management
  ✅ CORS support
  ✅ Compression
  ~600 lines C++

Day 7: Testing & Polish
  ✅ Server tests
  ✅ Integration tests
  ✅ Documentation completion
  ✅ Performance testing
  ~1,000 lines tests
```

### Week 3 (22-28 يناير) - Buffer & HTTPS
```
Day 1-2: HTTPS Integration
  ✅ OpenSSL setup
  ✅ Certificate handling
  ✅ HTTPS client
  ✅ HTTPS server

Day 3-4: Sad Language Bindings
  ✅ http.s interface
  ✅ C++ bindings
  ✅ Example programs

Day 5-7: Final Testing & Documentation
  ✅ Comprehensive testing
  ✅ Performance benchmarks
  ✅ Complete documentation
  ✅ Release preparation
```

---

## 📊 مؤشرات النجاح / Success Criteria

### Functional Requirements
```
✅ HTTP Client can make GET, POST, PUT, DELETE requests
✅ HTTPS support with certificate verification
✅ HTTP Server can handle multiple concurrent connections
✅ Routing system supports path parameters
✅ Middleware chain works correctly
✅ Static file serving functional
✅ Cookie management working
✅ JSON parsing/serialization
✅ Error handling comprehensive
```

### Performance Requirements
```
✅ Client: >1000 requests/second
✅ Server: >5000 requests/second
✅ Latency: <10ms for simple requests
✅ Memory: <100MB for 1000 concurrent connections
✅ Connection pooling: reuse connections efficiently
```

### Quality Requirements
```
✅ Unit test coverage: >85%
✅ Integration tests: All scenarios covered
✅ Zero memory leaks (Valgrind clean)
✅ Clean compilation (0 warnings)
✅ Documentation: 100% API covered
✅ Examples: All features demonstrated
```

---

## 🚀 الخطوات التالية / Next Steps

### Phase 2.1.2 Immediate Actions
1. **Setup & Dependencies** (Day 1 morning)
   - Verify OpenSSL/LibreSSL available
   - Create file structure
   - Setup CMakeLists.txt

2. **Start Implementation** (Day 1 afternoon - Day 2)
   - HttpBase utilities
   - HttpRequest class
   - HttpResponse class

3. **Continue with Client** (Day 3-4)
   - HttpClient implementation
   - Connection pooling
   - Testing

### After Phase 2.1.2
```
Phase 2.1.3: WebSocket (Week 5-6)
  - Build on HTTP foundation
  - WebSocket client & server
  - Real-time communication

Phase 2.1.4: SSL/TLS & Completion (Week 7-8)
  - Advanced SSL features
  - Performance optimization
  - Final integration testing
```

---

## 📝 ملاحظات مهمة / Important Notes

### Dependencies
```cpp
// CMakeLists.txt addition
find_package(OpenSSL REQUIRED)

target_link_libraries(sad_http
    PRIVATE
        sad_network    # Phase 2.1.1
        OpenSSL::SSL
        OpenSSL::Crypto
)
```

### Platform Considerations
```
Windows:
  - Use Winsock2 (already in Phase 2.1.1)
  - OpenSSL from vcpkg or pre-built

Linux:
  - POSIX sockets (already in Phase 2.1.1)
  - OpenSSL from package manager

macOS:
  - Same as Linux
  - Consider LibreSSL alternative
```

### Security Considerations
```
✅ Input validation (headers, URL, body)
✅ Size limits (request, body, headers)
✅ SSL/TLS certificate verification
✅ Timeout on all operations
✅ Protection against slowloris
✅ Safe file serving (no directory traversal)
```

---

**آخر تحديث / Last Updated:** 7 يناير 2026  
**الحالة / Status:** ⏳ **جاهز للبدء / Ready to Start**  
**البدء المخطط / Planned Start:** 22 يناير 2026 (بعد Phase 2.1.1 extension)

---

*للمزيد من التفاصيل حول Phase 2.1.1، راجع:*  
*[Phase 2.1.1 Status](../phase_2_1_1/IMPLEMENTATION_STATUS.md)*
