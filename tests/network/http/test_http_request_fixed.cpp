// بسم الله الرحمن الرحيم
// HTTP Request Tests - Fixed

#include "network/http/http_request.h"
#include "network/http/http_cookie.h"
#include <gtest/gtest.h>

using namespace sad::network::http;

// ==========================================
// Basic Creation Tests
// ==========================================

TEST(HttpRequestTest, DefaultConstructor) {
    HttpRequest request;
    
    EXPECT_EQ(request.method(), HttpMethod::GET);
    EXPECT_EQ(request.path(), "/");
    EXPECT_EQ(request.body(), "");
}

TEST(HttpRequestTest, ConstructorWithMethod) {
    HttpRequest request(HttpMethod::POST, "/api/users");
    
    EXPECT_EQ(request.method(), HttpMethod::POST);
    EXPECT_EQ(request.path(), "/api/users");
}

TEST(HttpRequestTest, ConstructorWithBody) {
    HttpRequest request(HttpMethod::POST, "/api/data", "{\"key\": \"value\"}");
    
    EXPECT_EQ(request.method(), HttpMethod::POST);
    EXPECT_EQ(request.path(), "/api/data");
    EXPECT_EQ(request.body(), "{\"key\": \"value\"}");
}

// ==========================================
// Method Tests
// ==========================================

TEST(HttpRequestTest, SetMethod) {
    HttpRequest request;
    
    request.set_method(HttpMethod::PUT);
    EXPECT_EQ(request.method(), HttpMethod::PUT);
}

// ==========================================
// Path Tests
// ==========================================

TEST(HttpRequestTest, SetPath) {
    HttpRequest request;
    
    request.set_path("/api/posts/123");
    EXPECT_EQ(request.path(), "/api/posts/123");
}

// ==========================================
// Header Tests
// ==========================================

TEST(HttpRequestTest, SetAndGetHeader) {
    HttpRequest request;
    
    request.set_header("Content-Type", "application/json");
    
    EXPECT_TRUE(request.has_header("Content-Type"));
    EXPECT_EQ(request.header("Content-Type"), "application/json");
}

TEST(HttpRequestTest, HeaderCaseInsensitive) {
    HttpRequest request;
    
    request.set_header("Content-Type", "text/plain");
    
    EXPECT_TRUE(request.has_header("content-type"));
    EXPECT_EQ(request.header("CONTENT-TYPE"), "text/plain");
}

TEST(HttpRequestTest, RemoveHeader) {
    HttpRequest request;
    
    request.set_header("X-Custom", "value");
    EXPECT_TRUE(request.has_header("X-Custom"));
    
    request.remove_header("X-Custom");
    EXPECT_FALSE(request.has_header("X-Custom"));
}

TEST(HttpRequestTest, ClearHeaders) {
    HttpRequest request;
    
    request.set_header("Header1", "value1");
    request.set_header("Header2", "value2");
    
    EXPECT_TRUE(request.has_header("Header1"));
    EXPECT_TRUE(request.has_header("Header2"));
    
    request.clear_headers();
    
    EXPECT_FALSE(request.has_header("Header1"));
    EXPECT_FALSE(request.has_header("Header2"));
}

// ==========================================
// Query Parameter Tests
// ==========================================

TEST(HttpRequestTest, SetAndGetQueryParam) {
    HttpRequest request;
    
    request.set_query_param("page", "1");
    request.set_query_param("limit", "10");
    
    EXPECT_TRUE(request.has_query_param("page"));
    EXPECT_EQ(request.query_param("page"), "1");
    EXPECT_EQ(request.query_param("limit"), "10");
}

TEST(HttpRequestTest, RemoveQueryParam) {
    HttpRequest request;
    
    request.set_query_param("filter", "active");
    EXPECT_TRUE(request.has_query_param("filter"));
    
    request.remove_query_param("filter");
    EXPECT_FALSE(request.has_query_param("filter"));
}

TEST(HttpRequestTest, ClearQueryParams) {
    HttpRequest request;
    
    request.set_query_param("a", "1");
    request.set_query_param("b", "2");
    
    EXPECT_TRUE(request.has_query_param("a"));
    EXPECT_TRUE(request.has_query_param("b"));
    
    request.clear_query_params();
    
    EXPECT_FALSE(request.has_query_param("a"));
    EXPECT_FALSE(request.has_query_param("b"));
}

// ==========================================
// Body Tests
// ==========================================

TEST(HttpRequestTest, SetBody) {
    HttpRequest request;
    
    request.set_body("Request body");
    EXPECT_EQ(request.body(), "Request body");
}

TEST(HttpRequestTest, SetJson) {
    HttpRequest request;
    
    request.set_json("{\"key\": \"value\"}");
    
    EXPECT_EQ(request.body(), "{\"key\": \"value\"}");
    EXPECT_EQ(request.header("Content-Type"), "application/json");
}

TEST(HttpRequestTest, ClearBody) {
    HttpRequest request;
    
    request.set_body("Some content");
    EXPECT_FALSE(request.body().empty());
    
    request.clear_body();
    EXPECT_TRUE(request.body().empty());
}

// ==========================================
// Serialization Tests
// ==========================================

TEST(HttpRequestTest, SerializeGet) {
    HttpRequest request(HttpMethod::GET, "/api/users");
    request.set_header("Host", "example.com");
    
    std::string serialized = request.serialize();
    
    EXPECT_TRUE(serialized.find("GET /api/users HTTP/1.1") != std::string::npos);
    EXPECT_TRUE(serialized.find("Host: example.com") != std::string::npos);
}

TEST(HttpRequestTest, SerializePost) {
    HttpRequest request(HttpMethod::POST, "/api/data", "test body");
    request.set_header("Content-Type", "text/plain");
    
    std::string serialized = request.serialize();
    
    EXPECT_TRUE(serialized.find("POST /api/data HTTP/1.1") != std::string::npos);
    EXPECT_TRUE(serialized.find("Content-Type: text/plain") != std::string::npos);
    EXPECT_TRUE(serialized.find("test body") != std::string::npos);
}

TEST(HttpRequestTest, Deserialize) {
    std::string raw = "POST /api/users HTTP/1.1\r\n"
                     "Host: example.com\r\n"
                     "Content-Type: application/json\r\n"
                     "Content-Length: 18\r\n"
                     "\r\n"
                     "{\"name\":\"John\"}";
    
    HttpRequest request;
    EXPECT_TRUE(request.deserialize(raw));
    
    EXPECT_EQ(request.method(), HttpMethod::POST);
    EXPECT_EQ(request.path(), "/api/users");
    EXPECT_EQ(request.header("Host"), "example.com");
    EXPECT_EQ(request.header("Content-Type"), "application/json");
    EXPECT_EQ(request.body(), "{\"name\":\"John\"}");
}

// ==========================================
// Round Trip Tests
// ==========================================

TEST(HttpRequestTest, RoundTrip) {
    HttpRequest original(HttpMethod::PUT, "/api/posts/123", "updated content");
    original.set_header("Authorization", "Bearer token123");
    original.set_header("Content-Type", "text/plain");
    original.set_query_param("notify", "true");
    
    std::string serialized = original.serialize();
    
    HttpRequest parsed;
    EXPECT_TRUE(parsed.deserialize(serialized));
    
    EXPECT_EQ(parsed.method(), original.method());
    EXPECT_EQ(parsed.path(), original.path());
    EXPECT_EQ(parsed.header("Authorization"), original.header("Authorization"));
    EXPECT_EQ(parsed.header("Content-Type"), original.header("Content-Type"));
    EXPECT_EQ(parsed.body(), original.body());
}
