// بسم الله الرحمن الرحيم
// HTTP Base Tests

#include "network/http/http_base.h"
#include <gtest/gtest.h>

using namespace sad::network::http;

// ==========================================
// HttpMethod Tests
// ==========================================

TEST(HttpBaseTest, MethodToString) {
    EXPECT_EQ(method_to_string(HttpMethod::GET), "GET");
    EXPECT_EQ(method_to_string(HttpMethod::POST), "POST");
    EXPECT_EQ(method_to_string(HttpMethod::PUT), "PUT");
    EXPECT_EQ(method_to_string(HttpMethod::DELETE), "DELETE");
    EXPECT_EQ(method_to_string(HttpMethod::PATCH), "PATCH");
    EXPECT_EQ(method_to_string(HttpMethod::HEAD), "HEAD");
    EXPECT_EQ(method_to_string(HttpMethod::OPTIONS), "OPTIONS");
    EXPECT_EQ(method_to_string(HttpMethod::CONNECT), "CONNECT");
    EXPECT_EQ(method_to_string(HttpMethod::TRACE), "TRACE");
}

// ==========================================
// HttpStatus Tests
// ==========================================

TEST(HttpBaseTest, StatusToString) {
    EXPECT_EQ(status_to_string(HttpStatus::OK), "OK");
    EXPECT_EQ(status_to_string(HttpStatus::Created), "Created");
    EXPECT_EQ(status_to_string(HttpStatus::NoContent), "No Content");
    EXPECT_EQ(status_to_string(HttpStatus::MovedPermanently), "Moved Permanently");
    EXPECT_EQ(status_to_string(HttpStatus::Found), "Found");
    EXPECT_EQ(status_to_string(HttpStatus::BadRequest), "Bad Request");
    EXPECT_EQ(status_to_string(HttpStatus::Unauthorized), "Unauthorized");
    EXPECT_EQ(status_to_string(HttpStatus::Forbidden), "Forbidden");
    EXPECT_EQ(status_to_string(HttpStatus::NotFound), "Not Found");
    EXPECT_EQ(status_to_string(HttpStatus::InternalServerError), "Internal Server Error");
}

TEST(HttpBaseTest, StatusToArabic) {
    EXPECT_EQ(status_to_arabic(HttpStatus::OK), "نجح");
    EXPECT_EQ(status_to_arabic(HttpStatus::Created), "تم الإنشاء");
    EXPECT_EQ(status_to_arabic(HttpStatus::BadRequest), "طلب خاطئ");
    EXPECT_EQ(status_to_arabic(HttpStatus::NotFound), "غير موجود");
    EXPECT_EQ(status_to_arabic(HttpStatus::InternalServerError), "خطأ في الخادم");
}

// ==========================================
// URL Encoding Tests
// ==========================================

TEST(HttpBaseTest, UrlEncode) {
    EXPECT_EQ(url_encode("hello world"), "hello%20world");
    EXPECT_EQ(url_encode("hello+world"), "hello%2Bworld");
    EXPECT_EQ(url_encode("hello@example.com"), "hello%40example.com");
    EXPECT_EQ(url_encode("hello/world"), "hello%2Fworld");
    EXPECT_EQ(url_encode("مرحبا"), "%D9%85%D8%B1%D8%AD%D8%A8%D8%A7");
    EXPECT_EQ(url_encode("abc123"), "abc123");
    EXPECT_EQ(url_encode(""), "");
}

TEST(HttpBaseTest, UrlDecode) {
    EXPECT_EQ(url_decode("hello%20world"), "hello world");
    EXPECT_EQ(url_decode("hello%2Bworld"), "hello+world");
    EXPECT_EQ(url_decode("hello%40example.com"), "hello@example.com");
    EXPECT_EQ(url_decode("hello%2Fworld"), "hello/world");
    EXPECT_EQ(url_decode("%D9%85%D8%B1%D8%AD%D8%A8%D8%A7"), "مرحبا");
    EXPECT_EQ(url_decode("abc123"), "abc123");
    EXPECT_EQ(url_decode(""), "");
}

TEST(HttpBaseTest, UrlEncodeDecodeRoundTrip) {
    std::string original = "hello world @#$%^&*()";
    std::string encoded = url_encode(original);
    std::string decoded = url_decode(encoded);
    EXPECT_EQ(decoded, original);
}

// ==========================================
// Query String Tests
// ==========================================

TEST(HttpBaseTest, BuildQueryString) {
    QueryParams params;
    params["name"] = "John Doe";
    params["age"] = "25";
    params["city"] = "Cairo";
    
    std::string query = build_query_string(params);
    EXPECT_TRUE(query.find("name=John%20Doe") != std::string::npos);
    EXPECT_TRUE(query.find("age=25") != std::string::npos);
    EXPECT_TRUE(query.find("city=Cairo") != std::string::npos);
}

TEST(HttpBaseTest, BuildQueryStringEmpty) {
    QueryParams params;
    std::string query = build_query_string(params);
    EXPECT_EQ(query, "");
}

TEST(HttpBaseTest, ParseQueryString) {
    std::string query = "name=John%20Doe&age=25&city=Cairo";
    QueryParams params = parse_query_string(query);
    
    EXPECT_EQ(params["name"], "John Doe");
    EXPECT_EQ(params["age"], "25");
    EXPECT_EQ(params["city"], "Cairo");
}

TEST(HttpBaseTest, ParseQueryStringEmpty) {
    std::string query = "";
    QueryParams params = parse_query_string(query);
    EXPECT_TRUE(params.empty());
}

TEST(HttpBaseTest, ParseQueryStringWithSpecialChars) {
    std::string query = "email=user%40example.com&path=%2Fhome%2Fuser";
    QueryParams params = parse_query_string(query);
    
    EXPECT_EQ(params["email"], "user@example.com");
    EXPECT_EQ(params["path"], "/home/user");
}

TEST(HttpBaseTest, QueryStringRoundTrip) {
    QueryParams original;
    original["name"] = "John Doe";
    original["email"] = "john@example.com";
    original["path"] = "/home/user";
    
    std::string query = build_query_string(original);
    QueryParams parsed = parse_query_string(query);
    
    EXPECT_EQ(parsed["name"], original["name"]);
    EXPECT_EQ(parsed["email"], original["email"]);
    EXPECT_EQ(parsed["path"], original["path"]);
}

// ==========================================
// Header Comparison Tests
// ==========================================

TEST(HttpBaseTest, CaseInsensitiveCompare) {
    EXPECT_TRUE(case_insensitive_compare("Content-Type", "content-type"));
    EXPECT_TRUE(case_insensitive_compare("CONTENT-TYPE", "content-type"));
    EXPECT_TRUE(case_insensitive_compare("content-type", "Content-Type"));
    EXPECT_FALSE(case_insensitive_compare("Content-Type", "Content-Length"));
}

// ==========================================
// Constants Tests
// ==========================================

TEST(HttpBaseTest, HeaderConstants) {
    EXPECT_EQ(headers::ContentType, "Content-Type");
    EXPECT_EQ(headers::ContentLength, "Content-Length");
    EXPECT_EQ(headers::Authorization, "Authorization");
    EXPECT_EQ(headers::UserAgent, "User-Agent");
    EXPECT_EQ(headers::Accept, "Accept");
}

TEST(HttpBaseTest, MimeTypeConstants) {
    EXPECT_EQ(mime_types::TextHtml, "text/html");
    EXPECT_EQ(mime_types::TextPlain, "text/plain");
    EXPECT_EQ(mime_types::ApplicationJson, "application/json");
    EXPECT_EQ(mime_types::ApplicationXml, "application/xml");
    EXPECT_EQ(mime_types::ImagePng, "image/png");
    EXPECT_EQ(mime_types::ImageJpeg, "image/jpeg");
}
