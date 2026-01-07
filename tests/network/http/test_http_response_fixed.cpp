// بسم الله الرحمن الرحيم
// HTTP Response Tests - Fixed

#include "network/http/http_response.h"
#include <gtest/gtest.h>

using namespace sad::network::http;

// ==========================================
// Basic Creation Tests
// ==========================================

TEST(HttpResponseTest, DefaultConstructor) {
    HttpResponse response;
    
    EXPECT_EQ(response.status(), HttpStatus::OK);
    EXPECT_EQ(response.status_code(), 200);
    EXPECT_EQ(response.body(), "");
}

TEST(HttpResponseTest, ConstructorWithStatus) {
    HttpResponse response(HttpStatus::NotFound);
    
    EXPECT_EQ(response.status(), HttpStatus::NotFound);
    EXPECT_EQ(response.status_code(), 404);
}

TEST(HttpResponseTest, ConstructorWithStatusAndBody) {
    HttpResponse response(HttpStatus::OK, "Success");
    
    EXPECT_EQ(response.status(), HttpStatus::OK);
    EXPECT_EQ(response.body(), "Success");
}

// ==========================================
// Status Tests
// ==========================================

TEST(HttpResponseTest, SetStatus) {
    HttpResponse response;
    
    response.set_status(HttpStatus::Created);
    EXPECT_EQ(response.status(), HttpStatus::Created);
    EXPECT_EQ(response.status_code(), 201);
}

TEST(HttpResponseTest, StatusText) {
    HttpResponse response(HttpStatus::OK);
    
    std::string text = response.status_text();
    EXPECT_FALSE(text.empty());
}

TEST(HttpResponseTest, StatusTextArabic) {
    HttpResponse response(HttpStatus::NotFound);
    
    std::string arabic = response.status_text_arabic();
    EXPECT_FALSE(arabic.empty());
}

TEST(HttpResponseTest, IsSuccess) {
    HttpResponse response(HttpStatus::OK);
    EXPECT_TRUE(response.is_success());
    
    response.set_status(HttpStatus::Created);
    EXPECT_TRUE(response.is_success());
    
    response.set_status(HttpStatus::BadRequest);
    EXPECT_FALSE(response.is_success());
}

TEST(HttpResponseTest, IsRedirect) {
    HttpResponse response(HttpStatus::MovedPermanently);
    EXPECT_TRUE(response.is_redirect());
    
    response.set_status(HttpStatus::Found);
    EXPECT_TRUE(response.is_redirect());
    
    response.set_status(HttpStatus::OK);
    EXPECT_FALSE(response.is_redirect());
}

TEST(HttpResponseTest, IsClientError) {
    HttpResponse response(HttpStatus::BadRequest);
    EXPECT_TRUE(response.is_client_error());
    
    response.set_status(HttpStatus::NotFound);
    EXPECT_TRUE(response.is_client_error());
    
    response.set_status(HttpStatus::OK);
    EXPECT_FALSE(response.is_client_error());
}

TEST(HttpResponseTest, IsServerError) {
    HttpResponse response(HttpStatus::InternalServerError);
    EXPECT_TRUE(response.is_server_error());
    
    response.set_status(HttpStatus::BadGateway);
    EXPECT_TRUE(response.is_server_error());
    
    response.set_status(HttpStatus::OK);
    EXPECT_FALSE(response.is_server_error());
}

// ==========================================
// Header Tests
// ==========================================

TEST(HttpResponseTest, SetAndGetHeader) {
    HttpResponse response;
    
    response.set_header("Content-Type", "application/json");
    
    EXPECT_TRUE(response.has_header("Content-Type"));
    EXPECT_EQ(response.header("Content-Type"), "application/json");
}

TEST(HttpResponseTest, HeaderCaseInsensitive) {
    HttpResponse response;
    
    response.set_header("Content-Type", "text/plain");
    
    EXPECT_TRUE(response.has_header("content-type"));
    EXPECT_EQ(response.header("CONTENT-TYPE"), "text/plain");
}

TEST(HttpResponseTest, RemoveHeader) {
    HttpResponse response;
    
    response.set_header("X-Custom", "value");
    EXPECT_TRUE(response.has_header("X-Custom"));
    
    response.remove_header("X-Custom");
    EXPECT_FALSE(response.has_header("X-Custom"));
}

TEST(HttpResponseTest, ClearHeaders) {
    HttpResponse response;
    
    response.set_header("Header1", "value1");
    response.set_header("Header2", "value2");
    
    EXPECT_TRUE(response.has_header("Header1"));
    EXPECT_TRUE(response.has_header("Header2"));
    
    response.clear_headers();
    
    EXPECT_FALSE(response.has_header("Header1"));
    EXPECT_FALSE(response.has_header("Header2"));
}

// ==========================================
// Body Tests
// ==========================================

TEST(HttpResponseTest, SetBody) {
    HttpResponse response;
    
    response.set_body("Response body");
    EXPECT_EQ(response.body(), "Response body");
}

TEST(HttpResponseTest, SetJson) {
    HttpResponse response;
    
    response.set_json("{\"key\": \"value\"}");
    
    EXPECT_EQ(response.body(), "{\"key\": \"value\"}");
    EXPECT_EQ(response.header("Content-Type"), "application/json");
}

TEST(HttpResponseTest, SetHtml) {
    HttpResponse response;
    
    response.set_html("<html><body>Hello</body></html>");
    
    EXPECT_EQ(response.body(), "<html><body>Hello</body></html>");
    EXPECT_EQ(response.header("Content-Type"), "text/html");
}

TEST(HttpResponseTest, SetText) {
    HttpResponse response;
    
    response.set_text("Plain text response");
    
    EXPECT_EQ(response.body(), "Plain text response");
    EXPECT_EQ(response.header("Content-Type"), "text/plain");
}

TEST(HttpResponseTest, ClearBody) {
    HttpResponse response;
    
    response.set_body("Some content");
    EXPECT_FALSE(response.body().empty());
    
    response.clear_body();
    EXPECT_TRUE(response.body().empty());
}

// ==========================================
// Cookie Tests
// ==========================================

TEST(HttpResponseTest, SetCookie) {
    HttpResponse response;
    
    response.set_cookie("session", "abc123", "/", 3600);
    
    // Check Set-Cookie header was added
    EXPECT_TRUE(response.has_header("Set-Cookie"));
}

TEST(HttpResponseTest, SetCookieFull) {
    HttpResponse response;
    
    response.set_cookie_full("token", "value", "/api", "example.com", 
                            7200, true, true);
    
    EXPECT_TRUE(response.has_header("Set-Cookie"));
}

TEST(HttpResponseTest, DeleteCookie) {
    HttpResponse response;
    
    response.delete_cookie("old_session", "/");
    
    // Should set Max-Age=0 to delete cookie
    EXPECT_TRUE(response.has_header("Set-Cookie"));
}

// ==========================================
// Convenience Methods Tests
// ==========================================

TEST(HttpResponseTest, RedirectPermanent) {
    HttpResponse response;
    
    response.redirect("/new-location", true);
    
    EXPECT_EQ(response.status(), HttpStatus::MovedPermanently);
    EXPECT_EQ(response.header("Location"), "/new-location");
}

TEST(HttpResponseTest, RedirectTemporary) {
    HttpResponse response;
    
    response.redirect("/temp-location", false);
    
    EXPECT_EQ(response.status(), HttpStatus::Found);
    EXPECT_EQ(response.header("Location"), "/temp-location");
}

TEST(HttpResponseTest, EnableCors) {
    HttpResponse response;
    
    response.enable_cors("*");
    
    EXPECT_TRUE(response.has_header("Access-Control-Allow-Origin"));
    EXPECT_EQ(response.header("Access-Control-Allow-Origin"), "*");
}

TEST(HttpResponseTest, EnableCorsSpecificOrigin) {
    HttpResponse response;
    
    response.enable_cors("https://example.com");
    
    EXPECT_EQ(response.header("Access-Control-Allow-Origin"), "https://example.com");
}

// ==========================================
// Serialization Tests
// ==========================================

TEST(HttpResponseTest, SerializeBasic) {
    HttpResponse response(HttpStatus::OK, "Hello");
    response.set_header("Content-Type", "text/plain");
    
    std::string serialized = response.serialize();
    
    EXPECT_TRUE(serialized.find("HTTP/1.1 200") != std::string::npos);
    EXPECT_TRUE(serialized.find("Content-Type: text/plain") != std::string::npos);
    EXPECT_TRUE(serialized.find("Hello") != std::string::npos);
}

TEST(HttpResponseTest, DeserializeBasic) {
    std::string raw = "HTTP/1.1 200 OK\r\n"
                     "Content-Type: text/plain\r\n"
                     "Content-Length: 5\r\n"
                     "\r\n"
                     "Hello";
    
    HttpResponse response;
    EXPECT_TRUE(response.deserialize(raw));
    
    EXPECT_EQ(response.status(), HttpStatus::OK);
    EXPECT_EQ(response.header("Content-Type"), "text/plain");
    EXPECT_EQ(response.body(), "Hello");
}

// ==========================================
// Round Trip Tests
// ==========================================

TEST(HttpResponseTest, RoundTrip) {
    HttpResponse original(HttpStatus::Created, "{\"id\": 123}");
    original.set_header("Content-Type", "application/json");
    original.set_header("X-Request-ID", "abc-123");
    
    std::string serialized = original.serialize();
    
    HttpResponse parsed;
    EXPECT_TRUE(parsed.deserialize(serialized));
    
    EXPECT_EQ(parsed.status(), original.status());
    EXPECT_EQ(parsed.header("Content-Type"), original.header("Content-Type"));
    EXPECT_EQ(parsed.header("X-Request-ID"), original.header("X-Request-ID"));
    EXPECT_EQ(parsed.body(), original.body());
}
