// بسم الله الرحمن الرحيم
// HTTP URL Tests - Fixed

#include "network/http/http_url.h"
#include <gtest/gtest.h>

using namespace sad::network::http;

// ==========================================
// Basic Parsing Tests
// ==========================================

TEST(HttpUrlTest, ParseSimpleUrl) {
    URL url;
    EXPECT_TRUE(url.parse("http://example.com"));
    
    EXPECT_EQ(url.scheme(), "http");
    EXPECT_EQ(url.host(), "example.com");
    EXPECT_EQ(url.effective_port(), 80);
    EXPECT_EQ(url.path(), "");
}

TEST(HttpUrlTest, ParseUrlWithPath) {
    URL url;
    EXPECT_TRUE(url.parse("http://example.com/path/to/resource"));
    
    EXPECT_EQ(url.scheme(), "http");
    EXPECT_EQ(url.host(), "example.com");
    EXPECT_EQ(url.path(), "/path/to/resource");
}

TEST(HttpUrlTest, ParseUrlWithPort) {
    URL url;
    EXPECT_TRUE(url.parse("http://example.com:8080"));
    
    EXPECT_EQ(url.host(), "example.com");
    EXPECT_EQ(url.port(), 8080);
}

TEST(HttpUrlTest, ParseUrlWithQuery) {
    URL url;
    EXPECT_TRUE(url.parse("http://example.com/search?q=test&page=1"));
    
    EXPECT_EQ(url.path(), "/search");
    EXPECT_EQ(url.query(), "q=test&page=1");
}

TEST(HttpUrlTest, ParseUrlWithFragment) {
    URL url;
    EXPECT_TRUE(url.parse("http://example.com/page#section"));
    
    EXPECT_EQ(url.path(), "/page");
    EXPECT_EQ(url.fragment(), "section");
}

TEST(HttpUrlTest, ParseCompleteUrl) {
    URL url;
    EXPECT_TRUE(url.parse("https://user:pass@example.com:8443/api/v1?key=value#top"));
    
    EXPECT_EQ(url.scheme(), "https");
    EXPECT_EQ(url.username(), "user");
    EXPECT_EQ(url.password(), "pass");
    EXPECT_EQ(url.host(), "example.com");
    EXPECT_EQ(url.port(), 8443);
    EXPECT_EQ(url.path(), "/api/v1");
    EXPECT_EQ(url.query(), "key=value");
    EXPECT_EQ(url.fragment(), "top");
}

// ==========================================
// Building Tests
// ==========================================

TEST(HttpUrlTest, BuildUrl) {
    URL url;
    url.set_scheme("https");
    url.set_host("example.com");
    url.set_path("/api/data");
    
    std::string full_url = url.to_string();
    EXPECT_EQ(full_url, "https://example.com/api/data");
}

TEST(HttpUrlTest, BuildUrlWithPort) {
    URL url;
    url.set_scheme("http");
    url.set_host("localhost");
    url.set_port(3000);
    url.set_path("/test");
    
    std::string full_url = url.to_string();
    EXPECT_EQ(full_url, "http://localhost:3000/test");
}

TEST(HttpUrlTest, BuildUrlWithQuery) {
    URL url;
    url.set_scheme("https");
    url.set_host("api.example.com");
    url.set_path("/search");
    url.set_query("q=test&lang=ar");
    
    std::string full_url = url.to_string();
    EXPECT_EQ(full_url, "https://api.example.com/search?q=test&lang=ar");
}

// ==========================================
// Default Port Tests
// ==========================================

TEST(HttpUrlTest, DefaultPortHttp) {
    URL url;
    url.set_scheme("http");
    EXPECT_EQ(url.default_port(), 80);
}

TEST(HttpUrlTest, DefaultPortHttps) {
    URL url;
    url.set_scheme("https");
    EXPECT_EQ(url.default_port(), 443);
}

TEST(HttpUrlTest, EffectivePort) {
    URL url;
    url.set_scheme("http");
    url.set_host("example.com");
    
    // Without explicit port, should use default
    EXPECT_EQ(url.effective_port(), 80);
    
    // With explicit port
    url.set_port(8080);
    EXPECT_EQ(url.effective_port(), 8080);
}

// ==========================================
// URL Type Tests
// ==========================================

TEST(HttpUrlTest, IsAbsolute) {
    URL url;
    url.parse("http://example.com/path");
    EXPECT_TRUE(url.is_absolute());
    EXPECT_FALSE(url.is_relative());
}

TEST(HttpUrlTest, IsRelative) {
    URL url;
    url.set_path("/api/users");
    EXPECT_TRUE(url.is_relative());
    EXPECT_FALSE(url.is_absolute());
}

TEST(HttpUrlTest, IsSecure) {
    URL url;
    url.parse("https://example.com");
    EXPECT_TRUE(url.is_secure());
    
    url.parse("http://example.com");
    EXPECT_FALSE(url.is_secure());
}

// ==========================================
// Resolution Tests
// ==========================================

TEST(HttpUrlTest, ResolveAbsolutePath) {
    URL base;
    base.parse("http://example.com/api/users");
    
    URL relative;
    relative.set_path("/api/posts");
    
    URL resolved = URL::resolve(base, relative);
    EXPECT_EQ(resolved.to_string(), "http://example.com/api/posts");
}

TEST(HttpUrlTest, ResolveRelativePath) {
    URL base;
    base.parse("http://example.com/api/users/123");
    
    URL relative;
    relative.set_path("posts");
    
    URL resolved = URL::resolve(base, relative);
    EXPECT_EQ(resolved.to_string(), "http://example.com/api/users/posts");
}

TEST(HttpUrlTest, ResolveParentPath) {
    URL base;
    base.parse("http://example.com/api/users/123");
    
    URL relative;
    relative.set_path("../posts");
    
    URL resolved = URL::resolve(base, relative);
    EXPECT_EQ(resolved.to_string(), "http://example.com/api/posts");
}

// ==========================================
// Normalization Tests
// ==========================================

TEST(HttpUrlTest, NormalizeLowercase) {
    URL url;
    url.parse("HTTP://EXAMPLE.COM/Path");
    url.normalize();
    
    EXPECT_EQ(url.scheme(), "http");
    EXPECT_EQ(url.host(), "example.com");
}

TEST(HttpUrlTest, NormalizeRemoveDefaultPort) {
    URL url;
    url.parse("http://example.com:80/path");
    url.normalize();
    
    std::string normalized = url.to_string();
    EXPECT_EQ(normalized, "http://example.com/path");
}

TEST(HttpUrlTest, NormalizeResolveDots) {
    URL url;
    url.parse("http://example.com/a/b/../c/./d");
    url.normalize();
    
    EXPECT_EQ(url.path(), "/a/c/d");
}

// ==========================================
// Round Trip Tests
// ==========================================

TEST(HttpUrlTest, RoundTrip) {
    std::string original = "https://example.com:8080/api/v1?key=value#section";
    
    URL url;
    url.parse(original);
    
    std::string reconstructed = url.to_string();
    EXPECT_EQ(reconstructed, original);
}

TEST(HttpUrlTest, RoundTripWithAuth) {
    std::string original = "http://user:pass@example.com/api";
    
    URL url;
    url.parse(original);
    
    std::string reconstructed = url.to_string();
    EXPECT_EQ(reconstructed, original);
}
