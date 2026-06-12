// بسم الله الرحمن الرحيم
/**
 * @file test_network_comprehensive.cpp
 * @brief Comprehensive Tests for Network Module - اختبارات شاملة لوحدة الشبكات
 *
 * Tests: NetworkErrorCode, NetworkException, SocketAddress,
 *        HTTP Request/Response/Cookie/URL, WebSocket Frames
 *
 * @author ريم — مهندسة المكتبات والأدوات
 * @date March 2026
 */

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include "sad_test_framework.h"
#include "network/network_error.h"
#include "socket/socket_address.h"
#include "http/http_base.h"
#include "http/http_request.h"
#include "http/http_response.h"
#include "http/http_cookie.h"
#include "http/http_url.h"
#include "websocket/websocket_base.h"
#include "websocket/websocket_frame.h"

using namespace sad::network;
using namespace sad::network::http;
using namespace sad::network::websocket;

int main() {
    SAD_TEST_INIT();

    // ================================================================
    // 1. NetworkErrorCode & NetworkException
    // ================================================================
    SAD_GROUP("أكواد أخطاء الشبكة / Network Error Codes");

    SAD_TEST("error_code_to_arabic returns non-empty", {
        std::string msg = error_code_to_arabic(NetworkErrorCode::SOCKET_CREATION_FAILED);
        SAD_ASSERT_FALSE(msg.empty());
    });

    SAD_TEST("error_code_to_english returns non-empty", {
        std::string msg = error_code_to_english(NetworkErrorCode::TIMEOUT);
        SAD_ASSERT_FALSE(msg.empty());
    });

    SAD_TEST("all codes have arabic messages", {
        std::vector<NetworkErrorCode> codes = {
            NetworkErrorCode::GENERAL_FAILURE,
            NetworkErrorCode::SOCKET_CREATION_FAILED,
            NetworkErrorCode::SEND_FAILED,
            NetworkErrorCode::TIMEOUT,
            NetworkErrorCode::INVALID_ADDRESS,
        };
        for (auto code : codes) {
            SAD_ASSERT_FALSE(error_code_to_arabic(code).empty());
        }
    });

    SAD_TEST("NetworkException bilingual construction", {
        NetworkException ex(NetworkErrorCode::TIMEOUT,
                            "انتهت المهلة الزمنية", "Connection timed out");
        SAD_ASSERT_TRUE(ex.error_code() == NetworkErrorCode::TIMEOUT);
        SAD_ASSERT_EQ(ex.what_arabic(), std::string("انتهت المهلة الزمنية"));
        SAD_ASSERT_EQ(ex.what_english(), std::string("Connection timed out"));
    });

    SAD_TEST("NetworkException system error code", {
        NetworkException ex(NetworkErrorCode::GENERAL_FAILURE, "test");
        ex.set_system_error_code(10060);
        SAD_ASSERT_EQ(ex.system_error_code(), 10060);
    });

    // ================================================================
    // 2. SocketAddress
    // ================================================================
    SAD_GROUP("عناوين المقابس / Socket Addresses");

    SAD_TEST("default construction", {
        SocketAddress addr;
        SAD_ASSERT_EQ(addr.get_port(), (uint16_t)0);
    });

    SAD_TEST("IPv4 construction", {
        SocketAddress addr("127.0.0.1", 8080);
        SAD_ASSERT_EQ(addr.get_ip(), std::string("127.0.0.1"));
        SAD_ASSERT_EQ(addr.get_port(), (uint16_t)8080);
        SAD_ASSERT_TRUE(addr.is_ipv4());
    });

    SAD_TEST("port-only construction", {
        SocketAddress addr(3000);
        SAD_ASSERT_EQ(addr.get_port(), (uint16_t)3000);
    });

    SAD_TEST("static factories", {
        auto local = SocketAddress::localhost_ipv4(3000);
        SAD_ASSERT_EQ(local.get_ip(), std::string("127.0.0.1"));
        SAD_ASSERT_EQ(local.get_port(), (uint16_t)3000);
    });

    SAD_TEST("IPv4 validation", {
        SAD_ASSERT_TRUE(SocketAddress::is_valid_ipv4("192.168.1.1"));
        SAD_ASSERT_TRUE(SocketAddress::is_valid_ipv4("0.0.0.0"));
        SAD_ASSERT_FALSE(SocketAddress::is_valid_ipv4("999.999.999.999"));
        SAD_ASSERT_FALSE(SocketAddress::is_valid_ipv4(""));
    });

    SAD_TEST("IPv6 validation", {
        SAD_ASSERT_TRUE(SocketAddress::is_valid_ipv6("::1"));
        SAD_ASSERT_FALSE(SocketAddress::is_valid_ipv6("not-ipv6"));
    });

    // ================================================================
    // 3. HTTP Base
    // ================================================================
    SAD_GROUP("أساسيات HTTP / HTTP Base");

    SAD_TEST("method to/from string", {
        SAD_ASSERT_EQ(method_to_string(HttpMethod::GET), std::string("GET"));
        SAD_ASSERT_EQ(method_to_string(HttpMethod::POST), std::string("POST"));
        SAD_ASSERT_TRUE(string_to_method("DELETE") == HttpMethod::DELETE);
    });

    SAD_TEST("status to string", {
        SAD_ASSERT_EQ(status_to_string(HttpStatus::OK), std::string("OK"));
        SAD_ASSERT_EQ(status_to_string(HttpStatus::NotFound), std::string("Not Found"));
    });

    SAD_TEST("status to arabic", {
        std::string arabic = status_to_arabic(HttpStatus::OK);
        SAD_ASSERT_FALSE(arabic.empty());
    });

    SAD_TEST("URL encode/decode", {
        SAD_ASSERT_EQ(url_encode("hello world"), std::string("hello%20world"));
        SAD_ASSERT_EQ(url_decode("hello%20world"), std::string("hello world"));
    });

    SAD_TEST("query string build/parse", {
        QueryParams params = {{"name", "ahmed"}, {"age", "25"}};
        std::string qs = build_query_string(params);
        SAD_ASSERT_FALSE(qs.empty());
        auto parsed = parse_query_string(qs);
        SAD_ASSERT_EQ(parsed["name"], std::string("ahmed"));
        SAD_ASSERT_EQ(parsed["age"], std::string("25"));
    });

    // ================================================================
    // 4. HTTP Request
    // ================================================================
    SAD_GROUP("طلبات HTTP / HTTP Requests");

    SAD_TEST("default request is GET", {
        HttpRequest req;
        SAD_ASSERT_TRUE(req.method() == HttpMethod::GET);
    });

    SAD_TEST("set properties", {
        HttpRequest req;
        req.set_method(HttpMethod::POST);
        req.set_path("/api/users");
        req.set_body("{\"name\": \"أحمد\"}");
        req.set_header("Content-Type", "application/json");
        SAD_ASSERT_TRUE(req.method() == HttpMethod::POST);
        SAD_ASSERT_EQ(req.path(), std::string("/api/users"));
        SAD_ASSERT_TRUE(req.has_header("Content-Type"));
    });

    SAD_TEST("header manipulation", {
        HttpRequest req;
        req.set_header("X-Custom", "value1");
        SAD_ASSERT_TRUE(req.has_header("X-Custom"));
        req.remove_header("X-Custom");
        SAD_ASSERT_FALSE(req.has_header("X-Custom"));
    });

    SAD_TEST("to_string contains method", {
        HttpRequest req;
        req.set_method(HttpMethod::GET);
        req.set_path("/");
        std::string s = req.to_string();
        SAD_ASSERT_CONTAINS(s, "GET");
    });

    // ================================================================
    // 5. HTTP Response
    // ================================================================
    SAD_GROUP("ردود HTTP / HTTP Responses");

    SAD_TEST("default response is 200 OK", {
        HttpResponse resp;
        SAD_ASSERT_TRUE(resp.status() == HttpStatus::OK);
        SAD_ASSERT_TRUE(resp.is_success());
    });

    SAD_TEST("status categories", {
        HttpResponse resp;
        resp.set_status(HttpStatus::NotFound);
        SAD_ASSERT_EQ(resp.status_code(), 404);
        SAD_ASSERT_TRUE(resp.is_client_error());
        SAD_ASSERT_FALSE(resp.is_success());

        resp.set_status(HttpStatus::InternalServerError);
        SAD_ASSERT_TRUE(resp.is_server_error());
    });

    SAD_TEST("set body variants", {
        HttpResponse resp;
        resp.set_body("Hello");
        SAD_ASSERT_EQ(resp.body(), std::string("Hello"));

        resp.set_json("{\"ok\": true}");
        SAD_ASSERT_CONTAINS(resp.body(), "ok");

        resp.set_text("نص عادي");
        SAD_ASSERT_EQ(resp.body(), std::string("نص عادي"));
    });

    SAD_TEST("redirect", {
        HttpResponse resp;
        resp.redirect("https://example.com", true);
        SAD_ASSERT_TRUE(resp.is_redirect());
    });

    // ================================================================
    // 6. Cookie & CookieJar
    // ================================================================
    SAD_GROUP("الكوكيز / Cookies");

    SAD_TEST("basic cookie", {
        Cookie c("session_id", "abc123");
        SAD_ASSERT_EQ(c.name(), std::string("session_id"));
        SAD_ASSERT_EQ(c.value(), std::string("abc123"));
        SAD_ASSERT_TRUE(c.is_session_cookie());
    });

    SAD_TEST("cookie properties", {
        Cookie c("lang", "ar");
        c.set_domain(".example.com");
        c.set_path("/");
        c.set_secure(true);
        c.set_http_only(true);
        c.set_max_age(3600);
        SAD_ASSERT_EQ(c.domain(), std::string(".example.com"));
        SAD_ASSERT_TRUE(c.is_secure());
        SAD_ASSERT_TRUE(c.is_http_only());
        SAD_ASSERT_FALSE(c.is_session_cookie());
    });

    SAD_TEST("cookie jar add/retrieve", {
        CookieJar jar;
        jar.add_cookie(Cookie("a", "1"));
        jar.add_cookie(Cookie("b", "2"));
        SAD_ASSERT_EQ(jar.size(), (size_t)2);
        SAD_ASSERT_TRUE(jar.has_cookie("a"));
        SAD_ASSERT_FALSE(jar.has_cookie("c"));
    });

    SAD_TEST("cookie jar remove/clear", {
        CookieJar jar;
        jar.add_cookie(Cookie("x", "1"));
        jar.add_cookie(Cookie("y", "2"));
        jar.remove_cookie("x");
        SAD_ASSERT_EQ(jar.size(), (size_t)1);
        jar.clear();
        SAD_ASSERT_EQ(jar.size(), (size_t)0);
    });

    // ================================================================
    // 7. URL Parsing
    // ================================================================
    SAD_GROUP("تحليل الروابط / URL Parsing");

    SAD_TEST("parse HTTP URL", {
        URL url;
        SAD_ASSERT_TRUE(url.parse("http://example.com:8080/path?q=1#frag"));
        SAD_ASSERT_TRUE(url.is_valid());
        SAD_ASSERT_EQ(url.scheme(), std::string("http"));
        SAD_ASSERT_EQ(url.host(), std::string("example.com"));
        SAD_ASSERT_EQ(url.port(), 8080);
        SAD_ASSERT_EQ(url.path(), std::string("/path"));
    });

    SAD_TEST("HTTPS URL", {
        URL url;
        url.parse("https://api.example.com/v1/users");
        SAD_ASSERT_TRUE(url.is_valid());
        SAD_ASSERT_TRUE(url.is_secure());
        SAD_ASSERT_TRUE(url.is_absolute());
    });

    SAD_TEST("default ports", {
        URL http_url;
        http_url.parse("http://example.com/");
        SAD_ASSERT_EQ(http_url.default_port(), 80);
    });

    SAD_TEST("query params", {
        URL url;
        url.parse("http://example.com/search?q=test&page=2");
        auto params = url.query_params();
        SAD_ASSERT_EQ(params["q"], std::string("test"));
        SAD_ASSERT_EQ(params["page"], std::string("2"));
    });

    // ================================================================
    // 8. WebSocket Frames
    // ================================================================
    SAD_GROUP("إطارات WebSocket / WebSocket Frames");

    SAD_TEST("opcode values", {
        SAD_ASSERT_EQ((int)WebSocketOpcode::Text, 0x1);
        SAD_ASSERT_EQ((int)WebSocketOpcode::Binary, 0x2);
        SAD_ASSERT_EQ((int)WebSocketOpcode::Close, 0x8);
        SAD_ASSERT_EQ((int)WebSocketOpcode::Ping, 0x9);
    });

    SAD_TEST("close codes", {
        SAD_ASSERT_EQ((int)CloseCode::Normal, 1000);
        SAD_ASSERT_EQ((int)CloseCode::GoingAway, 1001);
    });

    SAD_TEST("build text frame", {
        auto frame = FrameBuilder::build_text("مرحبا");
        SAD_ASSERT_FALSE(frame.empty());
        SAD_ASSERT_GT(frame.size(), (size_t)2);
    });

    SAD_TEST("build binary frame", {
        std::vector<uint8_t> data = {0x01, 0x02, 0x03};
        auto frame = FrameBuilder::build_binary(data);
        SAD_ASSERT_FALSE(frame.empty());
    });

    SAD_TEST("build ping/pong", {
        auto ping = FrameBuilder::build_ping();
        SAD_ASSERT_FALSE(ping.empty());
        auto pong = FrameBuilder::build_pong();
        SAD_ASSERT_FALSE(pong.empty());
    });

    SAD_TEST("build close frame", {
        auto close = FrameBuilder::build_close(CloseCode::Normal, "Goodbye");
        SAD_ASSERT_FALSE(close.empty());
    });

    SAD_TEST("parse incomplete data", {
        std::vector<uint8_t> partial = {0x81};
        auto state = FrameParser::parse(partial);
        SAD_ASSERT_TRUE(state.result == FrameParser::ParseResult::Incomplete);
    });

    // ================================================================
    // 9. Edge Cases
    // ================================================================
    SAD_GROUP("حالات الحافة / Edge Cases");

    SAD_TEST("Arabic in HTTP body", {
        HttpRequest req;
        req.set_body("بسم الله الرحمن الرحيم");
        SAD_ASSERT_EQ(req.body(), std::string("بسم الله الرحمن الرحيم"));
    });

    SAD_TEST("Arabic URL encoding round-trip", {
        std::string encoded = url_encode("مفتاح");
        std::string decoded = url_decode(encoded);
        SAD_ASSERT_EQ(decoded, std::string("مفتاح"));
    });

    return _runner.printSummary();
}
