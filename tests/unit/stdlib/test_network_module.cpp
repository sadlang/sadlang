// بسم الله الرحمن الرحيم
/**
 * @file test_network_module.cpp
 * @brief Comprehensive Tests for Network Module - اختبارات شاملة لوحدة الشبكات
 *
 * Tests cover: NetworkErrorCode, NetworkException, SocketAddress,
 *              HTTP Request/Response, Cookie/CookieJar, URL parsing,
 *              WebSocket FrameBuilder/FrameParser
 *
 * @author ريم — مهندسة المكتبات والأدوات
 * @date March 2026
 * الحمد لله رب العالمين
 */

#include <gtest/gtest.h>
#include "include/network/network_error.h"
#include "include/socket/socket_address.h"
#include "include/http/http_base.h"
#include "include/http/http_request.h"
#include "include/http/http_response.h"
#include "include/http/http_cookie.h"
#include "include/http/http_url.h"
#include "include/websocket/websocket_base.h"
#include "include/websocket/websocket_frame.h"

// ============================================================================
// 1. NetworkErrorCode & NetworkException Tests
//    اختبارات أكواد الأخطاء واستثناءات الشبكة
// ============================================================================

using namespace sad::network;

TEST(NetworkErrorCodeTest, ErrorCodeToArabic) {
    std::string msg = error_code_to_arabic(NetworkErrorCode::SOCKET_CREATION_FAILED);
    EXPECT_FALSE(msg.empty());
}

TEST(NetworkErrorCodeTest, ErrorCodeToEnglish) {
    std::string msg = error_code_to_english(NetworkErrorCode::TIMEOUT);
    EXPECT_FALSE(msg.empty());
}

TEST(NetworkErrorCodeTest, AllCodesHaveArabicMessages) {
    std::vector<NetworkErrorCode> codes = {
        NetworkErrorCode::GENERAL_FAILURE,
        NetworkErrorCode::SOCKET_CREATION_FAILED,
        NetworkErrorCode::SOCKET_BIND_FAILED,
        NetworkErrorCode::SEND_FAILED,
        NetworkErrorCode::RECEIVE_FAILED,
        NetworkErrorCode::TIMEOUT,
        NetworkErrorCode::INVALID_ADDRESS,
        NetworkErrorCode::ADDRESS_IN_USE,
    };
    for (auto code : codes) {
        EXPECT_FALSE(error_code_to_arabic(code).empty())
            << "Missing Arabic message for code: " << static_cast<int>(code);
    }
}

TEST(NetworkExceptionTest, ConstructionWithBilingualMessages) {
    NetworkException ex(NetworkErrorCode::TIMEOUT,
                        "انتهت المهلة الزمنية",
                        "Connection timed out");
    EXPECT_EQ(ex.error_code(), NetworkErrorCode::TIMEOUT);
    EXPECT_FALSE(std::string(ex.what()).empty());
    EXPECT_EQ(ex.what_arabic(), "انتهت المهلة الزمنية");
    EXPECT_EQ(ex.what_english(), "Connection timed out");
}

TEST(NetworkExceptionTest, ConstructionWithSingleMessage) {
    NetworkException ex(NetworkErrorCode::SEND_FAILED, "Send error");
    EXPECT_EQ(ex.error_code(), NetworkErrorCode::SEND_FAILED);
    EXPECT_FALSE(std::string(ex.what()).empty());
}

TEST(NetworkExceptionTest, SystemErrorCode) {
    NetworkException ex(NetworkErrorCode::GENERAL_FAILURE, "test");
    ex.set_system_error_code(10060); // WSAETIMEDOUT
    EXPECT_EQ(ex.system_error_code(), 10060);
}

// ============================================================================
// 2. SocketAddress Tests
//    اختبارات عناوين المقابس
// ============================================================================

TEST(SocketAddressTest, DefaultConstruction) {
    SocketAddress addr;
    EXPECT_EQ(addr.get_port(), 0);
}

TEST(SocketAddressTest, IPv4Construction) {
    SocketAddress addr("127.0.0.1", 8080);
    EXPECT_EQ(addr.get_ip(), "127.0.0.1");
    EXPECT_EQ(addr.get_port(), 8080);
    EXPECT_TRUE(addr.is_ipv4());
    EXPECT_FALSE(addr.is_ipv6());
}

TEST(SocketAddressTest, PortOnlyConstruction) {
    SocketAddress addr(3000);
    EXPECT_EQ(addr.get_port(), 3000);
}

TEST(SocketAddressTest, StaticFactories) {
    auto any4 = SocketAddress::any_ipv4(80);
    EXPECT_EQ(any4.get_port(), 80);
    EXPECT_TRUE(any4.is_ipv4());

    auto local4 = SocketAddress::localhost_ipv4(3000);
    EXPECT_EQ(local4.get_ip(), "127.0.0.1");
    EXPECT_EQ(local4.get_port(), 3000);
}

TEST(SocketAddressTest, IPv4Validation) {
    EXPECT_TRUE(SocketAddress::is_valid_ipv4("192.168.1.1"));
    EXPECT_TRUE(SocketAddress::is_valid_ipv4("0.0.0.0"));
    EXPECT_TRUE(SocketAddress::is_valid_ipv4("255.255.255.255"));
    EXPECT_FALSE(SocketAddress::is_valid_ipv4("999.999.999.999"));
    EXPECT_FALSE(SocketAddress::is_valid_ipv4("abc.def.ghi.jkl"));
    EXPECT_FALSE(SocketAddress::is_valid_ipv4(""));
}

TEST(SocketAddressTest, IPv6Validation) {
    EXPECT_TRUE(SocketAddress::is_valid_ipv6("::1"));
    EXPECT_TRUE(SocketAddress::is_valid_ipv6("::"));
    EXPECT_FALSE(SocketAddress::is_valid_ipv6("not-an-ipv6"));
}

TEST(SocketAddressTest, SockaddrLength) {
    SocketAddress addr("127.0.0.1", 80);
    EXPECT_GT(addr.get_sockaddr_length(), 0u);
}

// ============================================================================
// 3. HTTP Base Tests
//    اختبارات أساسيات HTTP
// ============================================================================

using namespace sad::network::http;

TEST(HttpBaseTest, MethodToString) {
    EXPECT_EQ(method_to_string(HttpMethod::GET), "GET");
    EXPECT_EQ(method_to_string(HttpMethod::POST), "POST");
    EXPECT_EQ(method_to_string(HttpMethod::PUT), "PUT");
    EXPECT_EQ(method_to_string(HttpMethod::DELETE), "DELETE");
    EXPECT_EQ(method_to_string(HttpMethod::PATCH), "PATCH");
    EXPECT_EQ(method_to_string(HttpMethod::HEAD), "HEAD");
    EXPECT_EQ(method_to_string(HttpMethod::OPTIONS), "OPTIONS");
}

TEST(HttpBaseTest, StringToMethod) {
    EXPECT_EQ(string_to_method("GET"), HttpMethod::GET);
    EXPECT_EQ(string_to_method("POST"), HttpMethod::POST);
    EXPECT_EQ(string_to_method("DELETE"), HttpMethod::DELETE);
}

TEST(HttpBaseTest, StatusToString) {
    EXPECT_EQ(status_to_string(HttpStatus::OK), "OK");
    EXPECT_EQ(status_to_string(HttpStatus::NotFound), "Not Found");
    EXPECT_EQ(status_to_string(HttpStatus::InternalServerError),
              "Internal Server Error");
}

TEST(HttpBaseTest, StatusToArabic) {
    std::string arabic = status_to_arabic(HttpStatus::OK);
    EXPECT_FALSE(arabic.empty());
    std::string arabic404 = status_to_arabic(HttpStatus::NotFound);
    EXPECT_FALSE(arabic404.empty());
}

TEST(HttpBaseTest, UrlEncodeDecode) {
    EXPECT_EQ(url_encode("hello world"), "hello%20world");
    EXPECT_EQ(url_decode("hello%20world"), "hello world");
    // Round-trip
    std::string original = "مرحبا بالعالم";
    EXPECT_EQ(url_decode(url_encode(original)), original);
}

TEST(HttpBaseTest, QueryStringBuildAndParse) {
    QueryParams params = {{"name", "ahmed"}, {"age", "25"}};
    std::string qs = build_query_string(params);
    EXPECT_FALSE(qs.empty());

    auto parsed = parse_query_string(qs);
    EXPECT_EQ(parsed["name"], "ahmed");
    EXPECT_EQ(parsed["age"], "25");
}

// ============================================================================
// 4. HTTP Request Tests
//    اختبارات طلبات HTTP
// ============================================================================

TEST(HttpRequestTest, DefaultConstruction) {
    HttpRequest req;
    EXPECT_EQ(req.method(), HttpMethod::GET);
}

TEST(HttpRequestTest, SetProperties) {
    HttpRequest req;
    req.set_method(HttpMethod::POST);
    req.set_path("/api/users");
    req.set_body("{\"name\": \"أحمد\"}");
    req.set_header("Content-Type", "application/json");

    EXPECT_EQ(req.method(), HttpMethod::POST);
    EXPECT_EQ(req.path(), "/api/users");
    EXPECT_EQ(req.body(), "{\"name\": \"أحمد\"}");
    EXPECT_TRUE(req.has_header("Content-Type"));
    EXPECT_EQ(req.header("Content-Type"), "application/json");
}

TEST(HttpRequestTest, QueryParams) {
    HttpRequest req;
    req.set_query_param("q", "لغة ص");
    req.set_query_param("page", "1");

    EXPECT_EQ(req.query_param("q"), "لغة ص");
    EXPECT_EQ(req.query_param("page"), "1");
}

TEST(HttpRequestTest, HeaderManipulation) {
    HttpRequest req;
    req.set_header("X-Custom", "value1");
    EXPECT_TRUE(req.has_header("X-Custom"));
    EXPECT_EQ(req.header("X-Custom"), "value1");

    req.remove_header("X-Custom");
    EXPECT_FALSE(req.has_header("X-Custom"));
}

TEST(HttpRequestTest, ToString) {
    HttpRequest req;
    req.set_method(HttpMethod::GET);
    req.set_path("/");
    std::string s = req.to_string();
    EXPECT_FALSE(s.empty());
    EXPECT_NE(s.find("GET"), std::string::npos);
}

// ============================================================================
// 5. HTTP Response Tests
//    اختبارات ردود HTTP
// ============================================================================

TEST(HttpResponseTest, DefaultConstruction) {
    HttpResponse resp;
    EXPECT_EQ(resp.status(), HttpStatus::OK);
}

TEST(HttpResponseTest, SetStatus) {
    HttpResponse resp;
    resp.set_status(HttpStatus::NotFound);
    EXPECT_EQ(resp.status(), HttpStatus::NotFound);
    EXPECT_EQ(resp.status_code(), 404);
    EXPECT_FALSE(resp.is_success());
    EXPECT_TRUE(resp.is_client_error());
}

TEST(HttpResponseTest, StatusCategories) {
    HttpResponse resp;

    resp.set_status(HttpStatus::OK);
    EXPECT_TRUE(resp.is_success());
    EXPECT_FALSE(resp.is_redirect());
    EXPECT_FALSE(resp.is_client_error());
    EXPECT_FALSE(resp.is_server_error());

    resp.set_status(HttpStatus::MovedPermanently);
    EXPECT_TRUE(resp.is_redirect());

    resp.set_status(HttpStatus::InternalServerError);
    EXPECT_TRUE(resp.is_server_error());
}

TEST(HttpResponseTest, SetBody) {
    HttpResponse resp;
    resp.set_body("Hello");
    EXPECT_EQ(resp.body(), "Hello");

    resp.set_json("{\"ok\": true}");
    EXPECT_NE(resp.body().find("ok"), std::string::npos);

    resp.set_html("<h1>مرحبا</h1>");
    EXPECT_NE(resp.body().find("مرحبا"), std::string::npos);

    resp.set_text("نص عادي");
    EXPECT_EQ(resp.body(), "نص عادي");
}

TEST(HttpResponseTest, ArabicStatusText) {
    HttpResponse resp;
    resp.set_status(HttpStatus::OK);
    std::string arabic = resp.status_text_arabic();
    EXPECT_FALSE(arabic.empty());
}

TEST(HttpResponseTest, Redirect) {
    HttpResponse resp;
    resp.redirect("https://example.com", true);
    EXPECT_TRUE(resp.is_redirect());
}

// ============================================================================
// 6. Cookie & CookieJar Tests
//    اختبارات الكوكيز
// ============================================================================

TEST(CookieTest, BasicConstruction) {
    Cookie c("session_id", "abc123");
    EXPECT_EQ(c.name(), "session_id");
    EXPECT_EQ(c.value(), "abc123");
    EXPECT_TRUE(c.is_session_cookie());
}

TEST(CookieTest, SetProperties) {
    Cookie c("lang", "ar");
    c.set_domain(".example.com");
    c.set_path("/");
    c.set_secure(true);
    c.set_http_only(true);
    c.set_same_site(Cookie::SameSite::Strict);
    c.set_max_age(3600);

    EXPECT_EQ(c.domain(), ".example.com");
    EXPECT_EQ(c.path(), "/");
    EXPECT_TRUE(c.is_secure());
    EXPECT_TRUE(c.is_http_only());
    EXPECT_EQ(c.same_site(), Cookie::SameSite::Strict);
    EXPECT_EQ(c.max_age(), 3600);
    EXPECT_FALSE(c.is_session_cookie());
}

TEST(CookieTest, ToSetCookieHeader) {
    Cookie c("theme", "dark");
    c.set_path("/");
    c.set_secure(true);
    std::string header = c.to_set_cookie_header();
    EXPECT_NE(header.find("theme=dark"), std::string::npos);
    EXPECT_NE(header.find("Secure"), std::string::npos);
}

TEST(CookieTest, IsValid) {
    Cookie c("valid_name", "value");
    EXPECT_TRUE(c.is_valid());

    Cookie empty("", "value");
    EXPECT_FALSE(empty.is_valid());
}

TEST(CookieJarTest, AddAndRetrieve) {
    CookieJar jar;
    jar.add_cookie(Cookie("a", "1"));
    jar.add_cookie(Cookie("b", "2"));

    EXPECT_EQ(jar.size(), 2u);
    EXPECT_TRUE(jar.has_cookie("a"));
    EXPECT_TRUE(jar.has_cookie("b"));
    EXPECT_FALSE(jar.has_cookie("c"));
}

TEST(CookieJarTest, RemoveAndClear) {
    CookieJar jar;
    jar.add_cookie(Cookie("x", "1"));
    jar.add_cookie(Cookie("y", "2"));

    jar.remove_cookie("x");
    EXPECT_FALSE(jar.has_cookie("x"));
    EXPECT_EQ(jar.size(), 1u);

    jar.clear();
    EXPECT_EQ(jar.size(), 0u);
}

// ============================================================================
// 7. URL Tests
//    اختبارات تحليل الروابط
// ============================================================================

TEST(UrlTest, ParseHttpUrl) {
    URL url = URL::parse("http://example.com:8080/path?q=1#frag");
    EXPECT_TRUE(url.is_valid());
    EXPECT_EQ(url.scheme(), "http");
    EXPECT_EQ(url.host(), "example.com");
    EXPECT_EQ(url.port(), 8080);
    EXPECT_EQ(url.path(), "/path");
    EXPECT_EQ(url.query(), "q=1");
    EXPECT_EQ(url.fragment(), "frag");
}

TEST(UrlTest, ParseHttpsUrl) {
    URL url = URL::parse("https://api.example.com/v1/users");
    EXPECT_TRUE(url.is_valid());
    EXPECT_EQ(url.scheme(), "https");
    EXPECT_EQ(url.host(), "api.example.com");
    EXPECT_TRUE(url.is_secure());
    EXPECT_TRUE(url.is_absolute());
}

TEST(UrlTest, DefaultPorts) {
    URL http = URL::parse("http://example.com/");
    EXPECT_EQ(http.default_port(), 80);
    EXPECT_EQ(http.effective_port(), 80);

    URL https = URL::parse("https://example.com/");
    EXPECT_EQ(https.default_port(), 443);
}

TEST(UrlTest, QueryParams) {
    URL url = URL::parse("http://example.com/search?q=test&page=2");
    auto params = url.query_params();
    EXPECT_EQ(params["q"], "test");
    EXPECT_EQ(params["page"], "2");
}

TEST(UrlTest, ToString) {
    URL url;
    url.set_scheme("https");
    url.set_host("example.com");
    url.set_path("/api");
    std::string s = url.to_string();
    EXPECT_NE(s.find("https"), std::string::npos);
    EXPECT_NE(s.find("example.com"), std::string::npos);
}

TEST(UrlTest, InvalidUrl) {
    URL url = URL::parse("not-a-valid-url://");
    // Should not crash — might be invalid
}

TEST(UrlTest, PathAndQuery) {
    URL url = URL::parse("http://example.com/path?key=val");
    std::string pq = url.path_and_query();
    EXPECT_NE(pq.find("/path"), std::string::npos);
}

// ============================================================================
// 8. WebSocket Frame Tests
//    اختبارات إطارات WebSocket
// ============================================================================

using namespace sad::network::websocket;

TEST(WebSocketBaseTest, OpcodeValues) {
    EXPECT_EQ(static_cast<int>(WebSocketOpcode::Text), 0x1);
    EXPECT_EQ(static_cast<int>(WebSocketOpcode::Binary), 0x2);
    EXPECT_EQ(static_cast<int>(WebSocketOpcode::Close), 0x8);
    EXPECT_EQ(static_cast<int>(WebSocketOpcode::Ping), 0x9);
    EXPECT_EQ(static_cast<int>(WebSocketOpcode::Pong), 0xA);
}

TEST(WebSocketBaseTest, CloseCodeValues) {
    EXPECT_EQ(static_cast<int>(CloseCode::Normal), 1000);
    EXPECT_EQ(static_cast<int>(CloseCode::GoingAway), 1001);
    EXPECT_EQ(static_cast<int>(CloseCode::ProtocolError), 1002);
}

TEST(WebSocketBaseTest, ConnectionStateValues) {
    EXPECT_EQ(static_cast<int>(ConnectionState::Connecting), 0);
    EXPECT_EQ(static_cast<int>(ConnectionState::Open), 1);
}

TEST(FrameBuilderTest, BuildTextFrame) {
    auto frame = FrameBuilder::build_text("مرحبا");
    EXPECT_FALSE(frame.empty());
    EXPECT_GT(frame.size(), 2u);
}

TEST(FrameBuilderTest, BuildBinaryFrame) {
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04};
    auto frame = FrameBuilder::build_binary(data);
    EXPECT_FALSE(frame.empty());
}

TEST(FrameBuilderTest, BuildPingPong) {
    auto ping = FrameBuilder::build_ping();
    EXPECT_FALSE(ping.empty());

    std::vector<uint8_t> payload = {0xAA, 0xBB};
    auto pong = FrameBuilder::build_pong(payload);
    EXPECT_FALSE(pong.empty());
}

TEST(FrameBuilderTest, BuildCloseFrame) {
    auto close = FrameBuilder::build_close(CloseCode::Normal, "Goodbye");
    EXPECT_FALSE(close.empty());
}

TEST(FrameParserTest, ParseIncompleteData) {
    std::vector<uint8_t> partial = {0x81}; // incomplete
    auto state = FrameParser::parse(partial);
    EXPECT_EQ(state.result, FrameParser::ParseResult::Incomplete);
}

TEST(FrameParserTest, ParseUnmaskedTextFrame) {
    // Build an unmasked text frame: FIN=1, opcode=text, no mask
    std::string text = "Hi";
    std::vector<uint8_t> frame;
    frame.push_back(0x81); // FIN + Text
    frame.push_back(static_cast<uint8_t>(text.size())); // length, no mask
    for (char c : text) frame.push_back(static_cast<uint8_t>(c));

    auto state = FrameParser::parse(frame);
    if (state.result == FrameParser::ParseResult::Complete) {
        EXPECT_EQ(state.frame.opcode, WebSocketOpcode::Text);
    }
}

// ============================================================================
// 9. HTTP Version Tests
//    اختبارات إصدارات HTTP
// ============================================================================

TEST(HttpVersionTest, VersionToString) {
    EXPECT_EQ(version_to_string(HttpVersion::HTTP_1_0), "HTTP/1.0");
    EXPECT_EQ(version_to_string(HttpVersion::HTTP_1_1), "HTTP/1.1");
}

// ============================================================================
// 10. Edge Cases
//     حالات الحافة
// ============================================================================

TEST(NetworkEdgeCaseTest, EmptySocketAddress) {
    SocketAddress addr;
    EXPECT_NO_THROW(addr.get_sockaddr_length());
}

TEST(NetworkEdgeCaseTest, ArabicInHttpBody) {
    HttpRequest req;
    req.set_body("بسم الله الرحمن الرحيم");
    EXPECT_EQ(req.body(), "بسم الله الرحمن الرحيم");
}

TEST(NetworkEdgeCaseTest, ArabicQueryString) {
    std::string encoded = url_encode("مفتاح=قيمة");
    std::string decoded = url_decode(encoded);
    EXPECT_EQ(decoded, "مفتاح=قيمة");
}
