// بسم الله الرحمن الرحيم
// HTTP Cookie Tests - Fixed

#include "network/http/http_cookie.h"
#include <gtest/gtest.h>
#include <chrono>

using namespace sad::network::http;

// ==========================================
// Basic Creation Tests
// ==========================================

TEST(HttpCookieTest, CreateCookie) {
    Cookie cookie("session", "abc123");
    
    EXPECT_EQ(cookie.name(), "session");
    EXPECT_EQ(cookie.value(), "abc123");
}

TEST(HttpCookieTest, DefaultValues) {
    Cookie cookie;
    
    EXPECT_EQ(cookie.name(), "");
    EXPECT_EQ(cookie.value(), "");
    EXPECT_EQ(cookie.domain(), "");
    EXPECT_EQ(cookie.path(), "");
    EXPECT_EQ(cookie.max_age(), -1); // Session cookie
    EXPECT_FALSE(cookie.is_secure());
    EXPECT_FALSE(cookie.is_http_only());
}

TEST(HttpCookieTest, SetBasicProperties) {
    Cookie cookie;
    cookie.set_name("user_id");
    cookie.set_value("12345");
    cookie.set_domain("example.com");
    cookie.set_path("/api");
    
    EXPECT_EQ(cookie.name(), "user_id");
    EXPECT_EQ(cookie.value(), "12345");
    EXPECT_EQ(cookie.domain(), "example.com");
    EXPECT_EQ(cookie.path(), "/api");
}

// ==========================================
// Expiration Tests
// ==========================================

TEST(HttpCookieTest, SessionCookie) {
    Cookie cookie("session", "token");
    
    EXPECT_TRUE(cookie.is_session_cookie());
    EXPECT_EQ(cookie.max_age(), -1);
}

TEST(HttpCookieTest, MaxAge) {
    Cookie cookie("user", "data");
    cookie.set_max_age(3600); // 1 hour
    
    EXPECT_FALSE(cookie.is_session_cookie());
    EXPECT_EQ(cookie.max_age(), 3600);
}

TEST(HttpCookieTest, Expires) {
    Cookie cookie("token", "value");
    
    auto now = std::chrono::system_clock::now();
    auto expires = now + std::chrono::hours(24);
    
    cookie.set_expires(expires);
    
    EXPECT_FALSE(cookie.is_session_cookie());
    EXPECT_FALSE(cookie.is_expired());
}

TEST(HttpCookieTest, IsExpired) {
    Cookie cookie("old", "data");
    
    // Set expired time (1 hour ago)
    auto past = std::chrono::system_clock::now() - std::chrono::hours(1);
    cookie.set_expires(past);
    
    EXPECT_TRUE(cookie.is_expired());
}

// ==========================================
// Security Attributes Tests
// ==========================================

TEST(HttpCookieTest, SecureFlag) {
    Cookie cookie("secure_token", "value");
    
    EXPECT_FALSE(cookie.is_secure());
    
    cookie.set_secure(true);
    EXPECT_TRUE(cookie.is_secure());
    
    cookie.set_secure(false);
    EXPECT_FALSE(cookie.is_secure());
}

TEST(HttpCookieTest, HttpOnlyFlag) {
    Cookie cookie("http_only_token", "value");
    
    EXPECT_FALSE(cookie.is_http_only());
    
    cookie.set_http_only(true);
    EXPECT_TRUE(cookie.is_http_only());
    
    cookie.set_http_only(false);
    EXPECT_FALSE(cookie.is_http_only());
}

TEST(HttpCookieTest, SameSite) {
    Cookie cookie("token", "value");
    
    // Default
    EXPECT_EQ(cookie.same_site(), Cookie::SameSite::None);
    
    // Set to Strict
    cookie.set_same_site(Cookie::SameSite::Strict);
    EXPECT_EQ(cookie.same_site(), Cookie::SameSite::Strict);
    
    // Set to Lax
    cookie.set_same_site(Cookie::SameSite::Lax);
    EXPECT_EQ(cookie.same_site(), Cookie::SameSite::Lax);
}

// ==========================================
// Serialization Tests
// ==========================================

TEST(HttpCookieTest, ToSetCookieHeader) {
    Cookie cookie("session", "abc123");
    cookie.set_path("/");
    cookie.set_max_age(3600);
    
    std::string header = cookie.to_set_cookie_header();
    
    EXPECT_TRUE(header.find("session=abc123") != std::string::npos);
    EXPECT_TRUE(header.find("Path=/") != std::string::npos);
    EXPECT_TRUE(header.find("Max-Age=3600") != std::string::npos);
}

TEST(HttpCookieTest, ToSetCookieHeaderWithSecurity) {
    Cookie cookie("secure_token", "value");
    cookie.set_secure(true);
    cookie.set_http_only(true);
    cookie.set_same_site(Cookie::SameSite::Strict);
    
    std::string header = cookie.to_set_cookie_header();
    
    EXPECT_TRUE(header.find("Secure") != std::string::npos);
    EXPECT_TRUE(header.find("HttpOnly") != std::string::npos);
    EXPECT_TRUE(header.find("SameSite=Strict") != std::string::npos);
}

TEST(HttpCookieTest, FromSetCookieHeader) {
    Cookie cookie;
    EXPECT_TRUE(cookie.from_set_cookie_header("session=abc123; Path=/; Max-Age=3600"));
    
    EXPECT_EQ(cookie.name(), "session");
    EXPECT_EQ(cookie.value(), "abc123");
    EXPECT_EQ(cookie.path(), "/");
    EXPECT_EQ(cookie.max_age(), 3600);
}

TEST(HttpCookieTest, FromSetCookieHeaderWithSecurity) {
    Cookie cookie;
    EXPECT_TRUE(cookie.from_set_cookie_header(
        "token=value; Secure; HttpOnly; SameSite=Strict"
    ));
    
    EXPECT_EQ(cookie.name(), "token");
    EXPECT_EQ(cookie.value(), "value");
    EXPECT_TRUE(cookie.is_secure());
    EXPECT_TRUE(cookie.is_http_only());
    EXPECT_EQ(cookie.same_site(), Cookie::SameSite::Strict);
}

TEST(HttpCookieTest, ToCookieHeader) {
    Cookie cookie("user_id", "12345");
    
    std::string header = cookie.to_cookie_header();
    EXPECT_EQ(header, "user_id=12345");
}

// ==========================================
// CookieJar Tests
// ==========================================

TEST(CookieJarTest, AddAndGetCookie) {
    CookieJar jar;
    Cookie cookie("session", "token123");
    
    jar.add(cookie);
    
    Cookie retrieved = jar.get("session");
    EXPECT_EQ(retrieved.name(), "session");
    EXPECT_EQ(retrieved.value(), "token123");
}

TEST(CookieJarTest, HasCookie) {
    CookieJar jar;
    Cookie cookie("user", "data");
    
    EXPECT_FALSE(jar.has("user"));
    
    jar.add(cookie);
    EXPECT_TRUE(jar.has("user"));
}

TEST(CookieJarTest, RemoveCookie) {
    CookieJar jar;
    Cookie cookie("temp", "value");
    
    jar.add(cookie);
    EXPECT_TRUE(jar.has("temp"));
    
    jar.remove("temp");
    EXPECT_FALSE(jar.has("temp"));
}

TEST(CookieJarTest, ClearAll) {
    CookieJar jar;
    
    jar.add(Cookie("cookie1", "value1"));
    jar.add(Cookie("cookie2", "value2"));
    jar.add(Cookie("cookie3", "value3"));
    
    EXPECT_TRUE(jar.has("cookie1"));
    EXPECT_TRUE(jar.has("cookie2"));
    EXPECT_TRUE(jar.has("cookie3"));
    
    jar.clear();
    
    EXPECT_FALSE(jar.has("cookie1"));
    EXPECT_FALSE(jar.has("cookie2"));
    EXPECT_FALSE(jar.has("cookie3"));
}

TEST(CookieJarTest, RemoveExpired) {
    CookieJar jar;
    
    // Add non-expired cookie
    Cookie fresh("fresh", "value");
    fresh.set_max_age(3600);
    jar.add(fresh);
    
    // Add expired cookie
    Cookie expired("expired", "value");
    auto past = std::chrono::system_clock::now() - std::chrono::hours(1);
    expired.set_expires(past);
    jar.add(expired);
    
    EXPECT_TRUE(jar.has("fresh"));
    EXPECT_TRUE(jar.has("expired"));
    
    jar.remove_expired();
    
    EXPECT_TRUE(jar.has("fresh"));
    EXPECT_FALSE(jar.has("expired"));
}

// ==========================================
// Round Trip Tests
// ==========================================

TEST(HttpCookieTest, RoundTrip) {
    Cookie original("session", "abc123");
    original.set_path("/api");
    original.set_domain("example.com");
    original.set_max_age(7200);
    original.set_secure(true);
    original.set_http_only(true);
    original.set_same_site(Cookie::SameSite::Lax);
    
    std::string header = original.to_set_cookie_header();
    
    Cookie parsed;
    EXPECT_TRUE(parsed.from_set_cookie_header(header));
    
    EXPECT_EQ(parsed.name(), original.name());
    EXPECT_EQ(parsed.value(), original.value());
    EXPECT_EQ(parsed.path(), original.path());
    EXPECT_EQ(parsed.domain(), original.domain());
    EXPECT_EQ(parsed.is_secure(), original.is_secure());
    EXPECT_EQ(parsed.is_http_only(), original.is_http_only());
    EXPECT_EQ(parsed.same_site(), original.same_site());
}
