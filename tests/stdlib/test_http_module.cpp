// بسم الله الرحمن الرحيم
/**
 * @file test_http_module.cpp
 * @brief Comprehensive Tests for HTTP Module - اختبارات شاملة لوحدة HTTP
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#include <gtest/gtest.h>
#include "stdlib/http/http_module.h"
#include <thread>
#include <chrono>

using namespace sad::stdlib::http;

// ============================================================================
// Test Fixture - إعداد الاختبار
// ============================================================================

class HttpTest : public ::testing::Test {
protected:
    void SetUp() override {
        // إعداد بيئة الاختبار / Setup test environment
    }
    
    void TearDown() override {
        // تنظيف بعد الاختبار / Cleanup after test
    }
};

// ============================================================================
// HttpMethod Tests - اختبارات HttpMethod
// ============================================================================

TEST_F(HttpTest, MethodToString) {
    EXPECT_EQ("GET", method_to_string(HttpMethod::GET));
    EXPECT_EQ("POST", method_to_string(HttpMethod::POST));
    EXPECT_EQ("PUT", method_to_string(HttpMethod::PUT));
    EXPECT_EQ("DELETE", method_to_string(HttpMethod::DELETE));
    EXPECT_EQ("PATCH", method_to_string(HttpMethod::PATCH));
    EXPECT_EQ("HEAD", method_to_string(HttpMethod::HEAD));
    EXPECT_EQ("OPTIONS", method_to_string(HttpMethod::OPTIONS));
}

TEST_F(HttpTest, StringToMethod) {
    EXPECT_EQ(HttpMethod::GET, string_to_method("GET"));
    EXPECT_EQ(HttpMethod::POST, string_to_method("POST"));
    EXPECT_EQ(HttpMethod::PUT, string_to_method("PUT"));
    EXPECT_EQ(HttpMethod::DELETE, string_to_method("DELETE"));
    EXPECT_EQ(HttpMethod::PATCH, string_to_method("PATCH"));
    EXPECT_EQ(HttpMethod::GET, string_to_method("get")); // Case insensitive
    EXPECT_EQ(HttpMethod::POST, string_to_method("Post"));
}

// ============================================================================
// HttpStatus Tests - اختبارات HttpStatus
// ============================================================================

TEST_F(HttpTest, StatusDescription) {
    EXPECT_EQ("OK", get_status_description(HttpStatus::OK));
    EXPECT_EQ("Created", get_status_description(HttpStatus::Created));
    EXPECT_EQ("Not Found", get_status_description(HttpStatus::NotFound));
    EXPECT_EQ("Internal Server Error", get_status_description(HttpStatus::InternalServerError));
    EXPECT_EQ("Bad Request", get_status_description(HttpStatus::BadRequest));
}

// ============================================================================
// HttpHeaders Tests - اختبارات HttpHeaders
// ============================================================================

TEST_F(HttpTest, HeadersSetGet) {
    HttpHeaders headers;
    
    headers.set("Content-Type", "application/json");
    EXPECT_EQ("application/json", headers.get("Content-Type"));
    
    // Case insensitive
    EXPECT_EQ("application/json", headers.get("content-type"));
    EXPECT_EQ("application/json", headers.get("CONTENT-TYPE"));
}

TEST_F(HttpTest, HeadersHas) {
    HttpHeaders headers;
    
    headers.set("Authorization", "Bearer token123");
    EXPECT_TRUE(headers.has("Authorization"));
    EXPECT_TRUE(headers.has("authorization"));
    EXPECT_FALSE(headers.has("X-Custom-Header"));
}

TEST_F(HttpTest, HeadersRemove) {
    HttpHeaders headers;
    
    headers.set("X-Custom", "value");
    EXPECT_TRUE(headers.has("X-Custom"));
    
    headers.remove("X-Custom");
    EXPECT_FALSE(headers.has("X-Custom"));
}

TEST_F(HttpTest, HeadersClear) {
    HttpHeaders headers;
    
    headers.set("Header1", "value1");
    headers.set("Header2", "value2");
    
    headers.clear();
    
    EXPECT_FALSE(headers.has("Header1"));
    EXPECT_FALSE(headers.has("Header2"));
}

TEST_F(HttpTest, HeadersToString) {
    HttpHeaders headers;
    
    headers.set("Content-Type", "text/html");
    headers.set("Content-Length", "1234");
    
    std::string str = headers.to_string();
    
    EXPECT_TRUE(str.find("Content-Type: text/html") != std::string::npos);
    EXPECT_TRUE(str.find("Content-Length: 1234") != std::string::npos);
}

// ============================================================================
// HttpCookie Tests - اختبارات HttpCookie
// ============================================================================

TEST_F(HttpTest, CookieToSetCookieString) {
    HttpCookie cookie;
    cookie.name = "session_id";
    cookie.value = "abc123";
    cookie.domain = "example.com";
    cookie.path = "/";
    cookie.max_age = 3600;
    cookie.secure = true;
    cookie.http_only = true;
    
    std::string str = cookie.to_set_cookie_string();
    
    EXPECT_TRUE(str.find("session_id=abc123") != std::string::npos);
    EXPECT_TRUE(str.find("Domain=example.com") != std::string::npos);
    EXPECT_TRUE(str.find("Path=/") != std::string::npos);
    EXPECT_TRUE(str.find("Max-Age=3600") != std::string::npos);
    EXPECT_TRUE(str.find("Secure") != std::string::npos);
    EXPECT_TRUE(str.find("HttpOnly") != std::string::npos);
}

TEST_F(HttpTest, CookieFromSetCookieString) {
    std::string cookie_str = "user_id=12345; Domain=example.com; Path=/; Max-Age=7200; Secure; HttpOnly";
    
    HttpCookie cookie = HttpCookie::from_set_cookie_string(cookie_str);
    
    EXPECT_EQ("user_id", cookie.name);
    EXPECT_EQ("12345", cookie.value);
    EXPECT_EQ("example.com", cookie.domain);
    EXPECT_EQ("/", cookie.path);
    EXPECT_EQ(7200, cookie.max_age);
    EXPECT_TRUE(cookie.secure);
    EXPECT_TRUE(cookie.http_only);
}

// ============================================================================
// HttpRequest Tests - اختبارات HttpRequest
// ============================================================================

TEST_F(HttpTest, RequestConstruction) {
    HttpRequest request(HttpMethod::GET, "https://example.com/api/users");
    
    EXPECT_EQ(HttpMethod::GET, request.get_method());
    EXPECT_EQ("https://example.com/api/users", request.get_url());
}

TEST_F(HttpTest, RequestSetMethod) {
    HttpRequest request;
    
    request.set_method(HttpMethod::POST);
    EXPECT_EQ(HttpMethod::POST, request.get_method());
}

TEST_F(HttpTest, RequestSetUrl) {
    HttpRequest request;
    
    request.set_url("/api/data?key=value");
    EXPECT_EQ("/api/data?key=value", request.get_url());
}

TEST_F(HttpTest, RequestQueryParams) {
    HttpRequest request;
    request.set_url("/search?q=test&page=2&limit=10");
    
    EXPECT_EQ("test", request.get_query_param("q"));
    EXPECT_EQ("2", request.get_query_param("page"));
    EXPECT_EQ("10", request.get_query_param("limit"));
    EXPECT_EQ("default", request.get_query_param("missing", "default"));
}

TEST_F(HttpTest, RequestSetHeader) {
    HttpRequest request;
    
    request.set_header("User-Agent", "TestClient/1.0");
    request.set_header("Accept", "application/json");
    
    EXPECT_EQ("TestClient/1.0", request.get_headers().get("User-Agent"));
    EXPECT_EQ("application/json", request.get_headers().get("Accept"));
}

TEST_F(HttpTest, RequestAddCookie) {
    HttpRequest request;
    
    HttpCookie cookie;
    cookie.name = "session";
    cookie.value = "xyz789";
    
    request.add_cookie(cookie);
    
    EXPECT_EQ("xyz789", request.get_cookie("session"));
}

TEST_F(HttpTest, RequestToString) {
    HttpRequest request(HttpMethod::POST, "/api/users");
    request.set_header("Content-Type", "application/json");
    request.set_body("{\"name\":\"أحمد\"}");
    
    std::string str = request.to_string();
    
    EXPECT_TRUE(str.find("POST /api/users HTTP/1.1") != std::string::npos);
    EXPECT_TRUE(str.find("Content-Type: application/json") != std::string::npos);
    EXPECT_TRUE(str.find("{\"name\":\"أحمد\"}") != std::string::npos);
}

TEST_F(HttpTest, RequestFromString) {
    std::string request_str = 
        "GET /api/data?key=value HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: TestClient\r\n"
        "\r\n";
    
    HttpRequest request = HttpRequest::from_string(request_str);
    
    EXPECT_EQ(HttpMethod::GET, request.get_method());
    EXPECT_TRUE(request.get_url().find("/api/data") != std::string::npos);
    EXPECT_EQ("example.com", request.get_headers().get("Host"));
    EXPECT_EQ("TestClient", request.get_headers().get("User-Agent"));
}

// ============================================================================
// HttpResponse Tests - اختبارات HttpResponse
// ============================================================================

TEST_F(HttpTest, ResponseConstruction) {
    HttpResponse response(HttpStatus::OK);
    
    EXPECT_EQ(HttpStatus::OK, response.get_status());
}

TEST_F(HttpTest, ResponseSetStatus) {
    HttpResponse response;
    
    response.set_status(HttpStatus::Created);
    EXPECT_EQ(HttpStatus::Created, response.get_status());
}

TEST_F(HttpTest, ResponseSetBody) {
    HttpResponse response;
    
    response.set_body("Hello World");
    EXPECT_EQ("Hello World", response.get_body());
    EXPECT_EQ("11", response.get_headers().get("Content-Length"));
}

TEST_F(HttpTest, ResponseSetJson) {
    HttpResponse response;
    
    response.set_json("{\"status\":\"success\"}");
    
    EXPECT_EQ("{\"status\":\"success\"}", response.get_body());
    EXPECT_TRUE(response.get_headers().get("Content-Type").find("application/json") != std::string::npos);
}

TEST_F(HttpTest, ResponseSetHtml) {
    HttpResponse response;
    
    response.set_html("<h1>Test</h1>");
    
    EXPECT_EQ("<h1>Test</h1>", response.get_body());
    EXPECT_TRUE(response.get_headers().get("Content-Type").find("text/html") != std::string::npos);
}

TEST_F(HttpTest, ResponseAddCookie) {
    HttpResponse response;
    
    HttpCookie cookie;
    cookie.name = "token";
    cookie.value = "abc123";
    
    response.add_cookie(cookie);
    
    ASSERT_EQ(1, response.get_cookies().size());
    EXPECT_EQ("token", response.get_cookies()[0].name);
}

TEST_F(HttpTest, ResponseToString) {
    HttpResponse response(HttpStatus::OK);
    response.set_header("Content-Type", "text/plain");
    response.set_body("Success");
    
    std::string str = response.to_string();
    
    EXPECT_TRUE(str.find("HTTP/1.1 200 OK") != std::string::npos);
    EXPECT_TRUE(str.find("Content-Type: text/plain") != std::string::npos);
    EXPECT_TRUE(str.find("Success") != std::string::npos);
}

TEST_F(HttpTest, ResponseFromString) {
    std::string response_str = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 20\r\n"
        "\r\n"
        "{\"status\":\"ok\"}";
    
    HttpResponse response = HttpResponse::from_string(response_str);
    
    EXPECT_EQ(HttpStatus::OK, response.get_status());
    EXPECT_EQ("application/json", response.get_headers().get("Content-Type"));
    EXPECT_EQ("{\"status\":\"ok\"}", response.get_body());
}

// ============================================================================
// URL Utility Tests - اختبارات مساعدات URL
// ============================================================================

TEST_F(HttpTest, UrlEncode) {
    EXPECT_EQ("Hello%20World", url_encode("Hello World"));
    EXPECT_EQ("test%40example.com", url_encode("test@example.com"));
    
    // UTF-8 encoding
    std::string arabic = url_encode("مرحبا");
    EXPECT_TRUE(arabic.find("%") != std::string::npos);
}

TEST_F(HttpTest, UrlDecode) {
    EXPECT_EQ("Hello World", url_decode("Hello%20World"));
    EXPECT_EQ("Hello World", url_decode("Hello+World"));
    EXPECT_EQ("test@example.com", url_decode("test%40example.com"));
}

TEST_F(HttpTest, UrlEncodeDecodeRoundtrip) {
    std::string original = "Hello World! Test@123";
    std::string encoded = url_encode(original);
    std::string decoded = url_decode(encoded);
    
    EXPECT_EQ(original, decoded);
}

TEST_F(HttpTest, ParseQueryString) {
    auto params = parse_query_string("name=Ahmad&age=25&city=Riyadh");
    
    EXPECT_EQ("Ahmad", params["name"]);
    EXPECT_EQ("25", params["age"]);
    EXPECT_EQ("Riyadh", params["city"]);
}

TEST_F(HttpTest, ParseQueryStringWithEncoding) {
    auto params = parse_query_string("name=Ahmad%20Ali&email=test%40example.com");
    
    EXPECT_EQ("Ahmad Ali", params["name"]);
    EXPECT_EQ("test@example.com", params["email"]);
}

TEST_F(HttpTest, BuildQueryString) {
    std::map<std::string, std::string> params;
    params["name"] = "Ahmad";
    params["age"] = "25";
    
    std::string query = build_query_string(params);
    
    EXPECT_TRUE(query.find("name=Ahmad") != std::string::npos);
    EXPECT_TRUE(query.find("age=25") != std::string::npos);
    EXPECT_TRUE(query.find("&") != std::string::npos);
}

TEST_F(HttpTest, ParseUrl) {
    UrlParts parts = parse_url("https://example.com:8080/api/users?page=1#section");
    
    EXPECT_EQ("https", parts.scheme);
    EXPECT_EQ("example.com", parts.host);
    EXPECT_EQ(8080, parts.port);
    EXPECT_EQ("/api/users", parts.path);
    EXPECT_EQ("page=1", parts.query);
    EXPECT_EQ("section", parts.fragment);
}

TEST_F(HttpTest, ParseUrlDefaultPort) {
    UrlParts parts1 = parse_url("http://example.com/path");
    EXPECT_EQ(80, parts1.port);
    
    UrlParts parts2 = parse_url("https://example.com/path");
    EXPECT_EQ(443, parts2.port);
}

TEST_F(HttpTest, BuildUrl) {
    UrlParts parts;
    parts.scheme = "https";
    parts.host = "example.com";
    parts.port = 8080;
    parts.path = "/api/data";
    parts.query = "key=value";
    parts.fragment = "top";
    
    std::string url = build_url(parts);
    
    EXPECT_TRUE(url.find("https://") != std::string::npos);
    EXPECT_TRUE(url.find("example.com") != std::string::npos);
    EXPECT_TRUE(url.find(":8080") != std::string::npos);
    EXPECT_TRUE(url.find("/api/data") != std::string::npos);
    EXPECT_TRUE(url.find("?key=value") != std::string::npos);
    EXPECT_TRUE(url.find("#top") != std::string::npos);
}

// ============================================================================
// HttpClient Tests - اختبارات HttpClient
// ============================================================================

TEST_F(HttpTest, ClientConstruction) {
    ASSERT_NO_THROW({
        HttpClient client;
    });
}

TEST_F(HttpTest, ClientSetTimeout) {
    HttpClient client;
    
    ASSERT_NO_THROW({
        client.set_timeout(30);
    });
}

TEST_F(HttpTest, ClientSetUserAgent) {
    HttpClient client;
    
    ASSERT_NO_THROW({
        client.set_user_agent("TestClient/1.0");
    });
}

TEST_F(HttpTest, ClientSetDefaultHeader) {
    HttpClient client;
    
    ASSERT_NO_THROW({
        client.set_default_header("X-Custom", "value");
    });
}

// Note: Actual HTTP requests require network connection
// These tests would be integration tests rather than unit tests

// ============================================================================
// HttpServer Tests - اختبارات HttpServer
// ============================================================================

TEST_F(HttpTest, ServerConstruction) {
    ASSERT_NO_THROW({
        HttpServer server;
    });
}

TEST_F(HttpTest, ServerRegisterGetHandler) {
    HttpServer server;
    
    ASSERT_NO_THROW({
        server.get("/test", [](const HttpRequest& req) {
            HttpResponse res(HttpStatus::OK);
            res.set_body("Test response");
            return res;
        });
    });
}

TEST_F(HttpTest, ServerRegisterPostHandler) {
    HttpServer server;
    
    ASSERT_NO_THROW({
        server.post("/api/data", [](const HttpRequest& req) {
            HttpResponse res(HttpStatus::Created);
            res.set_json("{\"status\":\"created\"}");
            return res;
        });
    });
}

TEST_F(HttpTest, ServerUseMiddleware) {
    HttpServer server;
    
    ASSERT_NO_THROW({
        server.use([](HttpRequest& req, HttpResponse& res) {
            // Middleware logic
            return true;
        });
    });
}

TEST_F(HttpTest, ServerSetMaxConnections) {
    HttpServer server;
    
    ASSERT_NO_THROW({
        server.set_max_connections(100);
    });
}

// ============================================================================
// Integration Tests - اختبارات التكامل
// ============================================================================

TEST_F(HttpTest, RequestResponseRoundtrip) {
    // إنشاء طلب / Create request
    HttpRequest request(HttpMethod::POST, "/api/users");
    request.set_header("Content-Type", "application/json");
    request.set_body("{\"name\":\"Ahmad\"}");
    
    // تحويل إلى سلسلة نصية / Convert to string
    std::string request_str = request.to_string();
    
    // تحليل من السلسلة النصية / Parse from string
    HttpRequest parsed_request = HttpRequest::from_string(request_str);
    
    // التحقق / Verify
    EXPECT_EQ(request.get_method(), parsed_request.get_method());
    EXPECT_EQ(request.get_url(), parsed_request.get_url());
    EXPECT_EQ(request.get_body(), parsed_request.get_body());
}

TEST_F(HttpTest, UrlParseAndBuild) {
    std::string original_url = "https://example.com:8080/api/users?page=1&limit=10#results";
    
    // تحليل / Parse
    UrlParts parts = parse_url(original_url);
    
    // بناء / Build
    std::string rebuilt_url = build_url(parts);
    
    // التحقق من المكونات الأساسية / Verify core components
    EXPECT_TRUE(rebuilt_url.find("https://") != std::string::npos);
    EXPECT_TRUE(rebuilt_url.find("example.com") != std::string::npos);
    EXPECT_TRUE(rebuilt_url.find("8080") != std::string::npos);
    EXPECT_TRUE(rebuilt_url.find("/api/users") != std::string::npos);
}

TEST_F(HttpTest, CompleteHttpWorkflow) {
    // بناء طلب كامل / Build complete request
    HttpRequest request(HttpMethod::POST, "https://api.example.com/users");
    request.set_header("Content-Type", "application/json");
    request.set_header("Authorization", "Bearer token123");
    
    HttpCookie cookie;
    cookie.name = "session";
    cookie.value = "abc123";
    request.add_cookie(cookie);
    
    request.set_body("{\"name\":\"Ahmad\",\"age\":25}");
    
    // بناء استجابة / Build response
    HttpResponse response(HttpStatus::Created);
    response.set_header("Content-Type", "application/json");
    response.set_json("{\"id\":1,\"name\":\"Ahmad\",\"age\":25}");
    
    HttpCookie response_cookie;
    response_cookie.name = "user_id";
    response_cookie.value = "1";
    response_cookie.http_only = true;
    response.add_cookie(response_cookie);
    
    // التحقق / Verify
    EXPECT_EQ(HttpMethod::POST, request.get_method());
    EXPECT_EQ(HttpStatus::Created, response.get_status());
    EXPECT_EQ("Bearer token123", request.get_headers().get("Authorization"));
    EXPECT_TRUE(response.get_headers().get("Content-Type").find("application/json") != std::string::npos);
}

// ============================================================================
// Edge Cases Tests - اختبارات الحالات الحدية
// ============================================================================

TEST_F(HttpTest, EmptyUrl) {
    ASSERT_NO_THROW({
        HttpRequest request;
        request.set_url("");
    });
}

TEST_F(HttpTest, EmptyBody) {
    HttpResponse response;
    response.set_body("");
    
    EXPECT_EQ("", response.get_body());
}

TEST_F(HttpTest, LargeBody) {
    std::string large_body(10000, 'x');
    
    HttpResponse response;
    response.set_body(large_body);
    
    EXPECT_EQ(large_body, response.get_body());
}

TEST_F(HttpTest, SpecialCharactersInUrl) {
    std::string url = "/api/search?q=test+value&special=@#$%";
    
    ASSERT_NO_THROW({
        HttpRequest request;
        request.set_url(url);
    });
}

TEST_F(HttpTest, UnicodeInHeaders) {
    HttpHeaders headers;
    
    ASSERT_NO_THROW({
        headers.set("X-Custom", "مرحبا");
    });
    
    EXPECT_EQ("مرحبا", headers.get("X-Custom"));
}

// ============================================================================
// Main Test Runner - مشغل الاختبارات الرئيسي
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// الحمد لله رب العالمين
