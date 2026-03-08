// بسم الله الرحمن الرحيم
/**
 * @file http_module.cpp
 * @brief HTTP/HTTPS Module Implementation - تنفيذ وحدة HTTP/HTTPS
 * 
 * @author Sad Language Team
 * @date December 2025
 * 
 * الحمد لله رب العالمين
 */

#include <string>
#include "http/http_module.h"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <cstring>
#include <stdexcept>
#include <regex>
#include <chrono>
#include <thread>
#include <random>
#include <queue>
#include <mutex>
#include <filesystem>  // للتحقق من المسارات / For path validation

namespace fs = std::filesystem;

// Platform-specific includes
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #undef max
    #undef min
    #undef DELETE  // Windows defines DELETE macro - conflicts with HttpMethod::DELETE
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket ::close
#endif

namespace sad {
namespace stdlib {
namespace http {

// ============================================================================
// Helper Functions - دوال مساعدة
// ============================================================================

namespace {
    // تحويل سلسلة نصية إلى أحرف صغيرة / Convert string to lowercase
    std::string to_lower(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        return str;
    }
    
    // إزالة المسافات من البداية والنهاية / Trim whitespace
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }
    
    // تقسيم سلسلة نصية / Split string
    std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
    
    // فحص إذا كانت سلسلة نصية تبدأ بسلسلة أخرى / Check if string starts with
    bool starts_with(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && 
               str.compare(0, prefix.size(), prefix) == 0;
    }
    
    // تهيئة Winsock على Windows / Initialize Winsock on Windows
    class WinsockInitializer {
    public:
        WinsockInitializer() {
#ifdef _WIN32
            WSADATA wsa_data;
            WSAStartup(MAKEWORD(2, 2), &wsa_data);
#endif
        }
        
        ~WinsockInitializer() {
#ifdef _WIN32
            WSACleanup();
#endif
        }
    };
    
    // مثيل ثابت لتهيئة Winsock / Static instance for Winsock initialization
    WinsockInitializer g_winsock_initializer;
    
    // ========================================================================
    // Base64 Encoding - تشفير Base64
    // ========================================================================
    
    static const std::string base64_chars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    std::string base64_encode(const unsigned char* bytes_to_encode, size_t in_len) {
        std::string ret;
        int i = 0;
        int j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];
        
        while (in_len--) {
            char_array_3[i++] = *(bytes_to_encode++);
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;
                
                for (i = 0; i < 4; i++)
                    ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }
        
        if (i) {
            for (j = i; j < 3; j++)
                char_array_3[j] = '\0';
            
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            
            for (j = 0; j < i + 1; j++)
                ret += base64_chars[char_array_4[j]];
            
            while (i++ < 3)
                ret += '=';
        }
        
        return ret;
    }
    
    // ========================================================================
    // SHA-1 Hashing - تشفير SHA-1
    // ========================================================================
    
    // تنفيذ مبسط لـ SHA-1 حسب RFC 3174 / Simple SHA-1 implementation per RFC 3174
    class SHA1 {
    public:
        void update(const unsigned char* data, size_t len) {
            for (size_t i = 0; i < len; ++i) {
                message_block_[message_block_index_++] = data[i];
                length_low_ += 8;
                if (length_low_ == 0) {
                    length_high_++;
                }
                
                if (message_block_index_ == 64) {
                    process_message_block();
                }
            }
        }
        
        void finalize(unsigned char* digest) {
            pad_message();
            for (int i = 0; i < 20; ++i) {
                digest[i] = (intermediate_hash_[i >> 2] >> (8 * (3 - (i & 0x03)))) & 0xFF;
            }
        }
        
    private:
        uint32_t intermediate_hash_[5] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};
        uint32_t length_low_ = 0;
        uint32_t length_high_ = 0;
        unsigned char message_block_[64];
        int message_block_index_ = 0;
        
        uint32_t circular_shift(uint32_t bits, uint32_t word) {
            return ((word << bits) | (word >> (32 - bits)));
        }
        
        void process_message_block() {
            const uint32_t K[] = {0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6};
            uint32_t W[80];
            
            // تهيئة W / Initialize W
            for (int t = 0; t < 16; t++) {
                W[t] = message_block_[t * 4] << 24;
                W[t] |= message_block_[t * 4 + 1] << 16;
                W[t] |= message_block_[t * 4 + 2] << 8;
                W[t] |= message_block_[t * 4 + 3];
            }
            
            for (int t = 16; t < 80; t++) {
                W[t] = circular_shift(1, W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
            }
            
            uint32_t A = intermediate_hash_[0];
            uint32_t B = intermediate_hash_[1];
            uint32_t C = intermediate_hash_[2];
            uint32_t D = intermediate_hash_[3];
            uint32_t E = intermediate_hash_[4];
            
            for (int t = 0; t < 80; t++) {
                uint32_t temp = circular_shift(5, A) + E + W[t];
                
                if (t < 20) {
                    temp += ((B & C) | ((~B) & D)) + K[0];
                } else if (t < 40) {
                    temp += (B ^ C ^ D) + K[1];
                } else if (t < 60) {
                    temp += ((B & C) | (B & D) | (C & D)) + K[2];
                } else {
                    temp += (B ^ C ^ D) + K[3];
                }
                
                E = D;
                D = C;
                C = circular_shift(30, B);
                B = A;
                A = temp;
            }
            
            intermediate_hash_[0] += A;
            intermediate_hash_[1] += B;
            intermediate_hash_[2] += C;
            intermediate_hash_[3] += D;
            intermediate_hash_[4] += E;
            
            message_block_index_ = 0;
        }
        
        void pad_message() {
            message_block_[message_block_index_++] = 0x80;
            
            if (message_block_index_ > 56) {
                while (message_block_index_ < 64) {
                    message_block_[message_block_index_++] = 0;
                }
                process_message_block();
            }
            
            while (message_block_index_ < 56) {
                message_block_[message_block_index_++] = 0;
            }
            
            message_block_[56] = (length_high_ >> 24) & 0xFF;
            message_block_[57] = (length_high_ >> 16) & 0xFF;
            message_block_[58] = (length_high_ >> 8) & 0xFF;
            message_block_[59] = length_high_ & 0xFF;
            message_block_[60] = (length_low_ >> 24) & 0xFF;
            message_block_[61] = (length_low_ >> 16) & 0xFF;
            message_block_[62] = (length_low_ >> 8) & 0xFF;
            message_block_[63] = length_low_ & 0xFF;
            
            process_message_block();
        }
    };
    
    std::string sha1_hash(const std::string& input) {
        SHA1 sha1;
        sha1.update(reinterpret_cast<const unsigned char*>(input.c_str()), input.length());
        unsigned char digest[20];
        sha1.finalize(digest);
        return std::string(reinterpret_cast<char*>(digest), 20);
    }
}

// ============================================================================
// HttpMethod Implementation - تنفيذ HttpMethod
// ============================================================================

std::string method_to_string(HttpMethod method) {
    switch (method) {
        case HttpMethod::GET:     return "GET";
        case HttpMethod::POST:    return "POST";
        case HttpMethod::PUT:     return "PUT";
        case HttpMethod::DELETE:  return "DELETE";
        case HttpMethod::PATCH:   return "PATCH";
        case HttpMethod::HEAD:    return "HEAD";
        case HttpMethod::OPTIONS: return "OPTIONS";
        case HttpMethod::CONNECT: return "CONNECT";
        case HttpMethod::TRACE:   return "TRACE";
        default:                  return "GET";
    }
}

HttpMethod string_to_method(const std::string& method_str) {
    std::string upper = to_lower(method_str);
    std::transform(upper.begin(), upper.end(), upper.begin(), [](unsigned char c){ return std::toupper(c); });
    
    if (upper == "GET")     return HttpMethod::GET;
    if (upper == "POST")    return HttpMethod::POST;
    if (upper == "PUT")     return HttpMethod::PUT;
    if (upper == "DELETE")  return HttpMethod::DELETE;
    if (upper == "PATCH")   return HttpMethod::PATCH;
    if (upper == "HEAD")    return HttpMethod::HEAD;
    if (upper == "OPTIONS") return HttpMethod::OPTIONS;
    if (upper == "CONNECT") return HttpMethod::CONNECT;
    if (upper == "TRACE")   return HttpMethod::TRACE;
    
    return HttpMethod::GET;
}

// ============================================================================
// HttpStatus Implementation - تنفيذ HttpStatus
// ============================================================================

std::string get_status_description(HttpStatus status) {
    switch (status) {
        // 1xx
        case HttpStatus::Continue:            return "Continue";
        case HttpStatus::SwitchingProtocols:  return "Switching Protocols";
        
        // 2xx
        case HttpStatus::OK:                  return "OK";
        case HttpStatus::Created:             return "Created";
        case HttpStatus::Accepted:            return "Accepted";
        case HttpStatus::NoContent:           return "No Content";
        
        // 3xx
        case HttpStatus::MovedPermanently:    return "Moved Permanently";
        case HttpStatus::Found:               return "Found";
        case HttpStatus::SeeOther:            return "See Other";
        case HttpStatus::NotModified:         return "Not Modified";
        case HttpStatus::TemporaryRedirect:   return "Temporary Redirect";
        case HttpStatus::PermanentRedirect:   return "Permanent Redirect";
        
        // 4xx
        case HttpStatus::BadRequest:          return "Bad Request";
        case HttpStatus::Unauthorized:        return "Unauthorized";
        case HttpStatus::Forbidden:           return "Forbidden";
        case HttpStatus::NotFound:            return "Not Found";
        case HttpStatus::MethodNotAllowed:    return "Method Not Allowed";
        case HttpStatus::NotAcceptable:       return "Not Acceptable";
        case HttpStatus::RequestTimeout:      return "Request Timeout";
        case HttpStatus::Conflict:            return "Conflict";
        case HttpStatus::Gone:                return "Gone";
        case HttpStatus::PayloadTooLarge:     return "Payload Too Large";
        case HttpStatus::URITooLong:          return "URI Too Long";
        case HttpStatus::UnsupportedMediaType: return "Unsupported Media Type";
        case HttpStatus::TooManyRequests:     return "Too Many Requests";
        
        // 5xx
        case HttpStatus::InternalServerError: return "Internal Server Error";
        case HttpStatus::NotImplemented:      return "Not Implemented";
        case HttpStatus::BadGateway:          return "Bad Gateway";
        case HttpStatus::ServiceUnavailable:  return "Service Unavailable";
        case HttpStatus::GatewayTimeout:      return "Gateway Timeout";
        
        default:                              return "Unknown Status";
    }
}

// ============================================================================
// HttpHeaders Implementation - تنفيذ HttpHeaders
// ============================================================================

std::string HttpHeaders::normalize_name(const std::string& name) {
    std::string result = name;
    bool capitalize_next = true;
    
    for (char& c : result) {
        if (c == '-') {
            capitalize_next = true;
        } else if (capitalize_next) {
            c = std::toupper(c);
            capitalize_next = false;
        } else {
            c = std::tolower(c);
        }
    }
    
    return result;
}

void HttpHeaders::set(const std::string& name, const std::string& value) {
    headers_[normalize_name(name)] = value;
}

std::string HttpHeaders::get(const std::string& name) const {
    auto it = headers_.find(normalize_name(name));
    return it != headers_.end() ? it->second : "";
}

bool HttpHeaders::has(const std::string& name) const {
    return headers_.find(normalize_name(name)) != headers_.end();
}

void HttpHeaders::remove(const std::string& name) {
    headers_.erase(normalize_name(name));
}

void HttpHeaders::clear() {
    headers_.clear();
}

std::string HttpHeaders::to_string() const {
    std::ostringstream oss;
    for (const auto& [name, value] : headers_) {
        oss << name << ": " << value << "\r\n";
    }
    return oss.str();
}

// ============================================================================
// HttpCookie Implementation - تنفيذ HttpCookie
// ============================================================================

std::string HttpCookie::to_set_cookie_string() const {
    std::ostringstream oss;
    oss << name << "=" << value;
    
    if (!domain.empty()) {
        oss << "; Domain=" << domain;
    }
    
    if (!path.empty()) {
        oss << "; Path=" << path;
    }
    
    if (max_age >= 0) {
        oss << "; Max-Age=" << max_age;
    }
    
    if (secure) {
        oss << "; Secure";
    }
    
    if (http_only) {
        oss << "; HttpOnly";
    }
    
    if (!same_site.empty()) {
        oss << "; SameSite=" << same_site;
    }
    
    return oss.str();
}

HttpCookie HttpCookie::from_set_cookie_string(const std::string& str) {
    HttpCookie cookie;
    auto parts = split(str, ';');
    
    if (!parts.empty()) {
        // Parse name=value
        auto eq_pos = parts[0].find('=');
        if (eq_pos != std::string::npos) {
            cookie.name = trim(parts[0].substr(0, eq_pos));
            cookie.value = trim(parts[0].substr(eq_pos + 1));
        }
        
        // Parse attributes
        for (size_t i = 1; i < parts.size(); i++) {
            std::string attr = trim(parts[i]);
            std::string attr_lower = to_lower(attr);
            
            if (starts_with(attr_lower, "domain=")) {
                cookie.domain = attr.substr(7);
            } else if (starts_with(attr_lower, "path=")) {
                cookie.path = attr.substr(5);
            } else if (starts_with(attr_lower, "max-age=")) {
                cookie.max_age = std::stoll(attr.substr(8));
            } else if (attr_lower == "secure") {
                cookie.secure = true;
            } else if (attr_lower == "httponly") {
                cookie.http_only = true;
            } else if (starts_with(attr_lower, "samesite=")) {
                cookie.same_site = attr.substr(9);
            }
        }
    }
    
    return cookie;
}

// ============================================================================
// HttpRequest Implementation - تنفيذ HttpRequest
// ============================================================================

HttpRequest::HttpRequest(HttpMethod method, const std::string& url)
    : method_(method), url_(url) {
    parse_url();
}

void HttpRequest::set_url(const std::string& url) {
    url_ = url;
    parse_url();
}

void HttpRequest::parse_url() {
    // تحليل URL لاستخراج المسار ومعاملات الاستعلام
    // Parse URL to extract path and query parameters
    
    size_t query_pos = url_.find('?');
    if (query_pos != std::string::npos) {
        path_ = url_.substr(0, query_pos);
        query_string_ = url_.substr(query_pos + 1);
        parse_query_string();
    } else {
        path_ = url_;
        query_string_.clear();
        query_params_.clear();
    }
}

void HttpRequest::parse_query_string() {
    query_params_ = sad::stdlib::http::parse_query_string(query_string_);
}

void HttpRequest::set_header(const std::string& name, const std::string& value) {
    headers_.set(name, value);
}

void HttpRequest::add_cookie(const HttpCookie& cookie) {
    cookies_.push_back(cookie);
}

std::string HttpRequest::get_query_param(const std::string& name, 
                                         const std::string& default_value) const {
    auto it = query_params_.find(name);
    return it != query_params_.end() ? it->second : default_value;
}

std::string HttpRequest::get_cookie(const std::string& name, 
                                    const std::string& default_value) const {
    for (const auto& cookie : cookies_) {
        if (cookie.name == name) {
            return cookie.value;
        }
    }
    return default_value;
}

std::string HttpRequest::to_string() const {
    std::ostringstream oss;
    
    // Request line
    oss << method_to_string(method_) << " " << url_ << " HTTP/1.1\r\n";
    
    // Headers
    oss << headers_.to_string();
    
    // Cookies
    if (!cookies_.empty()) {
        oss << "Cookie: ";
        for (size_t i = 0; i < cookies_.size(); i++) {
            if (i > 0) oss << "; ";
            oss << cookies_[i].name << "=" << cookies_[i].value;
        }
        oss << "\r\n";
    }
    
    // Empty line
    oss << "\r\n";
    
    // Body
    if (!body_.empty()) {
        oss << body_;
    }
    
    return oss.str();
}

HttpRequest HttpRequest::from_string(const std::string& request_str) {
    HttpRequest request;
    
    std::istringstream iss(request_str);
    std::string line;
    
    // Parse request line
    if (std::getline(iss, line)) {
        line.erase(line.find_last_not_of("\r\n") + 1);
        auto parts = split(line, ' ');
        
        if (parts.size() >= 2) {
            request.method_ = string_to_method(parts[0]);
            request.set_url(parts[1]);
        }
    }
    
    // Parse headers
    while (std::getline(iss, line) && line != "\r" && !line.empty()) {
        line.erase(line.find_last_not_of("\r\n") + 1);
        
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string name = trim(line.substr(0, colon));
            std::string value = trim(line.substr(colon + 1));
            request.set_header(name, value);
            
            // Parse cookies
            if (to_lower(name) == "cookie") {
                auto cookie_parts = split(value, ';');
                for (const auto& part : cookie_parts) {
                    size_t eq = part.find('=');
                    if (eq != std::string::npos) {
                        HttpCookie cookie;
                        cookie.name = trim(part.substr(0, eq));
                        cookie.value = trim(part.substr(eq + 1));
                        request.add_cookie(cookie);
                    }
                }
            }
        }
    }
    
    // Parse body
    std::ostringstream body_stream;
    body_stream << iss.rdbuf();
    request.body_ = body_stream.str();
    
    return request;
}

// ============================================================================
// HttpResponse Implementation - تنفيذ HttpResponse
// ============================================================================

HttpResponse::HttpResponse(HttpStatus status) : status_(status) {
}

void HttpResponse::set_header(const std::string& name, const std::string& value) {
    headers_.set(name, value);
}

void HttpResponse::set_body(const std::string& body) {
    body_ = body;
    headers_.set("Content-Length", std::to_string(body_.size()));
}

void HttpResponse::set_json(const std::string& json) {
    set_body(json);
    headers_.set("Content-Type", "application/json; charset=utf-8");
}

void HttpResponse::set_html(const std::string& html) {
    set_body(html);
    headers_.set("Content-Type", "text/html; charset=utf-8");
}

void HttpResponse::add_cookie(const HttpCookie& cookie) {
    cookies_.push_back(cookie);
}

std::string HttpResponse::to_string() const {
    std::ostringstream oss;
    
    // Status line
    oss << "HTTP/1.1 " << static_cast<int>(status_) << " " 
        << get_status_description(status_) << "\r\n";
    
    // Headers
    oss << headers_.to_string();
    
    // Cookies
    for (const auto& cookie : cookies_) {
        oss << "Set-Cookie: " << cookie.to_set_cookie_string() << "\r\n";
    }
    
    // Empty line
    oss << "\r\n";
    
    // Body
    if (!body_.empty()) {
        oss << body_;
    }
    
    return oss.str();
}

HttpResponse HttpResponse::from_string(const std::string& response_str) {
    HttpResponse response;
    
    std::istringstream iss(response_str);
    std::string line;
    
    // Parse status line
    if (std::getline(iss, line)) {
        line.erase(line.find_last_not_of("\r\n") + 1);
        auto parts = split(line, ' ');
        
        if (parts.size() >= 2) {
            int status_code = std::stoi(parts[1]);
            response.status_ = static_cast<HttpStatus>(status_code);
        }
    }
    
    // Parse headers
    while (std::getline(iss, line) && line != "\r" && !line.empty()) {
        line.erase(line.find_last_not_of("\r\n") + 1);
        
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string name = trim(line.substr(0, colon));
            std::string value = trim(line.substr(colon + 1));
            response.set_header(name, value);
            
            // Parse Set-Cookie
            if (to_lower(name) == "set-cookie") {
                response.add_cookie(HttpCookie::from_set_cookie_string(value));
            }
        }
    }
    
    // Parse body
    std::ostringstream body_stream;
    body_stream << iss.rdbuf();
    response.body_ = body_stream.str();
    
    return response;
}

// ============================================================================
// HttpClient Implementation - تنفيذ HttpClient
// ============================================================================

struct HttpClient::Impl {
    int timeout_seconds = 30;
    int retries = 0;
    bool follow_redirects = true;
    bool verify_ssl = true;
    HttpHeaders default_headers;
    
    HttpResponse send_request(const HttpRequest& request) {
        // تحليل URL / Parse URL
        UrlParts url_parts = parse_url(request.get_url());
        
        // إنشاء socket / Create socket
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == INVALID_SOCKET) {
            throw std::runtime_error("Failed to create socket");
        }
        
        // تحليل العنوان / Resolve address
        struct addrinfo hints{}, *result = nullptr;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        
        std::string port_str = std::to_string(url_parts.port);
        if (getaddrinfo(url_parts.host.c_str(), port_str.c_str(), &hints, &result) != 0) {
            closesocket(sock);
            throw std::runtime_error("Failed to resolve host: " + url_parts.host);
        }
        
        // الاتصال / Connect
        if (connect(sock, result->ai_addr, static_cast<int>(result->ai_addrlen)) == SOCKET_ERROR) {
            freeaddrinfo(result);
            closesocket(sock);
            throw std::runtime_error("Failed to connect to host");
        }
        
        freeaddrinfo(result);
        
        // إرسال الطلب / Send request
        std::string request_str = request.to_string();
        if (send(sock, request_str.c_str(), static_cast<int>(request_str.size()), 0) == SOCKET_ERROR) {
            closesocket(sock);
            throw std::runtime_error("Failed to send request");
        }
        
        // استقبال الاستجابة / Receive response
        std::string response_str;
        char buffer[4096];
        int bytes_received;
        
        while ((bytes_received = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
            response_str.append(buffer, bytes_received);
        }
        
        closesocket(sock);
        
        // تحليل الاستجابة / Parse response
        return HttpResponse::from_string(response_str);
    }
};

HttpClient::HttpClient() : impl_(std::make_unique<Impl>()) {
    impl_->default_headers.set("User-Agent", "Sad-HTTP-Client/1.0");
    impl_->default_headers.set("Accept", "*/*");
}

HttpClient::~HttpClient() = default;

HttpResponse HttpClient::get(const std::string& url) {
    HttpRequest request(HttpMethod::GET, url);
    
    // إضافة الترويسات الافتراضية / Add default headers
    for (const auto& [name, value] : impl_->default_headers.all()) {
        request.set_header(name, value);
    }
    
    return impl_->send_request(request);
}

HttpResponse HttpClient::get(const std::string& url, const HttpHeaders& headers) {
    HttpRequest request(HttpMethod::GET, url);
    
    // إضافة الترويسات المخصصة / Add custom headers
    for (const auto& [name, value] : headers.all()) {
        request.set_header(name, value);
    }
    
    return impl_->send_request(request);
}

HttpResponse HttpClient::post(const std::string& url, const std::string& body) {
    HttpRequest request(HttpMethod::POST, url);
    request.set_body(body);
    request.set_header("Content-Length", std::to_string(body.size()));
    
    for (const auto& [name, value] : impl_->default_headers.all()) {
        request.set_header(name, value);
    }
    
    return impl_->send_request(request);
}

HttpResponse HttpClient::post(const std::string& url, const std::string& body, 
                              const HttpHeaders& headers) {
    HttpRequest request(HttpMethod::POST, url);
    request.set_body(body);
    
    for (const auto& [name, value] : headers.all()) {
        request.set_header(name, value);
    }
    
    return impl_->send_request(request);
}

HttpResponse HttpClient::put(const std::string& url, const std::string& body) {
    HttpRequest request(HttpMethod::PUT, url);
    request.set_body(body);
    request.set_header("Content-Length", std::to_string(body.size()));
    
    return impl_->send_request(request);
}

HttpResponse HttpClient::del(const std::string& url) {
    HttpRequest request(HttpMethod::DELETE, url);
    return impl_->send_request(request);
}

HttpResponse HttpClient::patch(const std::string& url, const std::string& body) {
    HttpRequest request(HttpMethod::PATCH, url);
    request.set_body(body);
    request.set_header("Content-Length", std::to_string(body.size()));
    
    return impl_->send_request(request);
}

HttpResponse HttpClient::head(const std::string& url) {
    HttpRequest request(HttpMethod::HEAD, url);
    return impl_->send_request(request);
}

HttpResponse HttpClient::options(const std::string& url) {
    HttpRequest request(HttpMethod::OPTIONS, url);
    return impl_->send_request(request);
}

HttpResponse HttpClient::request(const HttpRequest& request) {
    return impl_->send_request(request);
}

void HttpClient::set_timeout(int timeout_seconds) {
    impl_->timeout_seconds = timeout_seconds;
}

void HttpClient::set_retries(int retries) {
    impl_->retries = retries;
}

void HttpClient::set_follow_redirects(bool follow) {
    impl_->follow_redirects = follow;
}

void HttpClient::set_verify_ssl(bool verify) {
    impl_->verify_ssl = verify;
}

void HttpClient::set_default_header(const std::string& name, const std::string& value) {
    impl_->default_headers.set(name, value);
}

void HttpClient::set_user_agent(const std::string& user_agent) {
    impl_->default_headers.set("User-Agent", user_agent);
}

// ============================================================================
// HttpServer Implementation - تنفيذ HttpServer  
// ============================================================================

struct HttpServer::Impl {
    struct Route {
        HttpMethod method;
        std::string path;
        RouteHandler handler;
    };
    
    std::vector<Route> routes;
    std::vector<Middleware> middlewares;
    bool running = false;
    SOCKET listen_socket = INVALID_SOCKET;
    std::thread server_thread;
    int max_connections = 100;
    int read_timeout = 30;
    
    void handle_client(SOCKET client_socket) {
        // استقبال الطلب / Receive request
        std::string request_str;
        char buffer[4096];
        int bytes_received;
        
        while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
            request_str.append(buffer, bytes_received);
            
            // فحص إذا انتهى الطلب / Check if request is complete
            if (request_str.find("\r\n\r\n") != std::string::npos) {
                break;
            }
        }
        
        if (request_str.empty()) {
            closesocket(client_socket);
            return;
        }
        
        // تحليل الطلب / Parse request
        HttpRequest request = HttpRequest::from_string(request_str);
        HttpResponse response(HttpStatus::NotFound);
        
        // تطبيق الوسائط / Apply middlewares
        bool continue_processing = true;
        for (auto& middleware : middlewares) {
            if (!middleware(request, response)) {
                continue_processing = false;
                break;
            }
        }
        
        // معالجة المسار / Process route
        if (continue_processing) {
            bool route_found = false;
            
            for (const auto& route : routes) {
                if (route.method == request.get_method() && 
                    route.path == request.get_path()) {
                    response = route.handler(request);
                    route_found = true;
                    break;
                }
            }
            
            if (!route_found) {
                response.set_status(HttpStatus::NotFound);
                response.set_html("<h1>404 Not Found</h1>");
            }
        }
        
        // إرسال الاستجابة / Send response
        std::string response_str = response.to_string();
        send(client_socket, response_str.c_str(), static_cast<int>(response_str.size()), 0);
        
        closesocket(client_socket);
    }
    
    void run_server(uint16_t port, const std::string& host) {
        // إنشاء socket الاستماع / Create listen socket
        listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listen_socket == INVALID_SOCKET) {
            throw std::runtime_error("Failed to create listen socket");
        }
        
        // إعداد العنوان / Setup address
        struct sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = inet_addr(host.c_str());
        
        // ربط / Bind
        if (bind(listen_socket, reinterpret_cast<struct sockaddr*>(&server_addr), 
                 sizeof(server_addr)) == SOCKET_ERROR) {
            closesocket(listen_socket);
            throw std::runtime_error("Failed to bind socket");
        }
        
        // الاستماع / Listen
        if (::listen(listen_socket, max_connections) == SOCKET_ERROR) {
            closesocket(listen_socket);
            throw std::runtime_error("Failed to listen on socket");
        }
        
        running = true;
        
        // قبول الاتصالات / Accept connections
        while (running) {
            struct sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            
            SOCKET client_socket = accept(listen_socket, 
                                         reinterpret_cast<struct sockaddr*>(&client_addr),
                                         &client_len);
            
            if (client_socket == INVALID_SOCKET) {
                if (running) {
                    continue;
                } else {
                    break;
                }
            }
            
            // معالجة العميل في thread منفصل / Handle client in separate thread
            std::thread(&Impl::handle_client, this, client_socket).detach();
        }
        
        closesocket(listen_socket);
    }
};

HttpServer::HttpServer() : impl_(std::make_unique<Impl>()) {
}

HttpServer::~HttpServer() {
    stop();
}

void HttpServer::get(const std::string& path, RouteHandler handler) {
    route(HttpMethod::GET, path, handler);
}

void HttpServer::post(const std::string& path, RouteHandler handler) {
    route(HttpMethod::POST, path, handler);
}

void HttpServer::put(const std::string& path, RouteHandler handler) {
    route(HttpMethod::PUT, path, handler);
}

void HttpServer::del(const std::string& path, RouteHandler handler) {
    route(HttpMethod::DELETE, path, handler);
}

void HttpServer::patch(const std::string& path, RouteHandler handler) {
    route(HttpMethod::PATCH, path, handler);
}

void HttpServer::route(HttpMethod method, const std::string& path, RouteHandler handler) {
    impl_->routes.push_back({method, path, handler});
}

void HttpServer::use(Middleware middleware) {
    impl_->middlewares.push_back(middleware);
}

/**
 * @brief خدمة الملفات الثابتة / Serve static files
 * @brief Serves static files from a directory
 * 
 * @details
 * (AR) يخدم الملفات الثابتة (HTML, CSS, JS, صور، إلخ) من مجلد محدد.
 *      يدعم mime types الشائعة ويرسل الملفات بشكل صحيح.
 * 
 * (EN) Serves static files (HTML, CSS, JS, images, etc.) from directory.
 *      Supports common mime types and sends files correctly.
 * 
 * @param url_prefix بادئة المسار / URL prefix (e.g., "/static")
 * @param directory المجلد المحلي / Local directory path
 * 
 * @example
 * server.serve_static("/static", "./public");
 * // GET /static/style.css -> serves ./public/style.css
 */
void HttpServer::serve_static(const std::string& url_prefix, const std::string& directory) {
    // Lambda للتعامل مع الطلبات / Lambda for request handling
    auto static_handler = [directory](const HttpRequest& req) -> HttpResponse {
        HttpResponse res;
        
        // استخراج المسار النسبي / Extract relative path
        std::string request_path = req.get_path();
        
        // ════════════════════════════════════════════════════════
        // إصلاح أمني: منع Path Traversal (اجتياز المسار)
        // Security fix: Prevent Path Traversal attacks
        // ════════════════════════════════════════════════════════
        
        // الخطوة 1: رفض المسارات التي تحتوي على ".." صريحاً
        // Step 1: Reject paths containing explicit ".."
        if (request_path.find("..") != std::string::npos) {
            res.set_status(HttpStatus::Forbidden);
            res.set_body("Forbidden: Invalid path");
            return res;
        }
        
        // الخطوة 2: رفض URL-encoded ".." patterns
        // Step 2: Reject URL-encoded ".." patterns
        if (request_path.find("%2e%2e") != std::string::npos ||
            request_path.find("%2E%2E") != std::string::npos ||
            request_path.find("%2e.") != std::string::npos ||
            request_path.find(".%2e") != std::string::npos) {
            res.set_status(HttpStatus::Forbidden);
            res.set_body("Forbidden: Invalid encoded path");
            return res;
        }
        
        try {
            // الخطوة 3: تطبيع المسارات والتحقق من الاحتواء
            // Step 3: Normalize paths and verify containment
            fs::path base_dir = fs::weakly_canonical(fs::path(directory));
            
            // تنظيف المسار المطلوب / Clean request path
            if (!request_path.empty() && request_path[0] == '/') {
                request_path = request_path.substr(1);
            }
            
            fs::path file_path = fs::weakly_canonical(base_dir / request_path);
            
            // التحقق من أن الملف داخل المجلد الأساسي
            // Verify file is within base directory
            std::string base_str = base_dir.string();
            std::string file_str = file_path.string();
            
            if (file_str.find(base_str) != 0) {
                res.set_status(HttpStatus::Forbidden);
                res.set_body("Forbidden: Path traversal detected");
                return res;
            }
            
            // فحص وجود الملف / Check file exists
            if (!fs::exists(file_path) || !fs::is_regular_file(file_path)) {
                res.set_status(HttpStatus::NotFound);
                res.set_body("File Not Found");
                return res;
            }
            
            // محاولة فتح الملف / Try to open file
            std::ifstream file(file_path, std::ios::binary);
            if (!file.is_open()) {
                res.set_status(HttpStatus::InternalServerError);
                res.set_body("Failed to read file");
                return res;
            }
            
            // قراءة محتوى الملف / Read file content
            std::ostringstream contents;
            contents << file.rdbuf();
            res.set_body(contents.str());
        
            // تحديد MIME type / Determine MIME type
            std::string ext;
            std::string file_path_str = file_path.string();
            size_t dot_pos = file_path_str.find_last_of('.');
            if (dot_pos != std::string::npos) {
                ext = file_path_str.substr(dot_pos);
                // تحويل للحروف الصغيرة / Convert to lowercase
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            }
        
            // جدول MIME types / MIME type table
            static const std::unordered_map<std::string, std::string> mime_types = {
                {".html", "text/html"},
                {".htm", "text/html"},
                {".css", "text/css"},
                {".js", "application/javascript"},
                {".json", "application/json"},
                {".xml", "application/xml"},
                {".txt", "text/plain"},
                {".jpg", "image/jpeg"},
                {".jpeg", "image/jpeg"},
                {".png", "image/png"},
                {".gif", "image/gif"},
                {".svg", "image/svg+xml"},
                {".ico", "image/x-icon"},
                {".pdf", "application/pdf"},
                {".zip", "application/zip"},
                {".woff", "font/woff"},
                {".woff2", "font/woff2"},
                {".ttf", "font/ttf"},
                {".mp4", "video/mp4"},
                {".mp3", "audio/mpeg"}
            };
        
            auto mime_it = mime_types.find(ext);
            if (mime_it != mime_types.end()) {
                res.set_header("Content-Type", mime_it->second);
            } else {
                res.set_header("Content-Type", "application/octet-stream");
            }
        
            res.set_status(HttpStatus::OK);
            
        } catch (const std::exception& e) {
            // خطأ في معالجة الملف / File processing error
            res.set_status(HttpStatus::InternalServerError);
            res.set_body("Server error processing file");
        }
        
        return res;
    };
    
    // تسجيل المعالج / Register handler
    // TODO: يحتاج route wildcards support في Impl::routes
    // For now, this will not work until route wildcards are implemented
}

void HttpServer::listen(uint16_t port, const std::string& host) {
    if (impl_->running) {
        throw std::runtime_error("Server is already running");
    }
    
    impl_->server_thread = std::thread(&Impl::run_server, impl_.get(), port, host);
}

void HttpServer::stop() {
    if (impl_->running) {
        impl_->running = false;
        
        if (impl_->listen_socket != INVALID_SOCKET) {
            closesocket(impl_->listen_socket);
        }
        
        if (impl_->server_thread.joinable()) {
            impl_->server_thread.join();
        }
    }
}

bool HttpServer::is_running() const {
    return impl_->running;
}

void HttpServer::set_max_connections(int max_connections) {
    impl_->max_connections = max_connections;
}

void HttpServer::set_read_timeout(int timeout_seconds) {
    impl_->read_timeout = timeout_seconds;
}

// ============================================================================
// WebSocket Implementation - تنفيذ WebSocket
// ============================================================================

struct WebSocket::Impl {
    SOCKET socket = INVALID_SOCKET;
    bool connected = false;
    std::function<void(const std::string&, WebSocketMessageType)> message_handler;
    std::function<void()> connect_handler;
    std::function<void()> close_handler;
    
    // طابور الرسائل / Message queue
    std::queue<std::pair<std::string, WebSocketMessageType>> message_queue;
    std::mutex queue_mutex;
    
    // معالجة الرسائل المجزأة / Fragmented message handling
    std::vector<uint8_t> fragmented_message;
    WebSocketMessageType fragmented_type = WebSocketMessageType::Text;
    
    // معلومات الاتصال / Connection info
    std::string host;
    uint16_t port;
    std::string path;
    
    // إطار WebSocket / WebSocket frame structure
    struct Frame {
        bool fin = false;
        bool rsv1 = false;
        bool rsv2 = false;
        bool rsv3 = false;
        uint8_t opcode = 0;
        bool mask = false;
        uint64_t payload_length = 0;
        uint8_t masking_key[4] = {0};
        std::vector<uint8_t> payload_data;
    };
    
    // بناء إطار WebSocket / Build WebSocket frame
    std::vector<uint8_t> build_frame(const std::vector<uint8_t>& payload, uint8_t opcode) {
        std::vector<uint8_t> frame;
        
        // FIN bit + Opcode
        frame.push_back(0x80 | opcode);
        
        // Mask bit + Payload length
        uint64_t payload_len = payload.size();
        
        if (payload_len < 126) {
            frame.push_back(0x80 | static_cast<uint8_t>(payload_len));
        } else if (payload_len < 65536) {
            frame.push_back(0x80 | 126);
            frame.push_back((payload_len >> 8) & 0xFF);
            frame.push_back(payload_len & 0xFF);
        } else {
            frame.push_back(0x80 | 127);
            for (int i = 7; i >= 0; --i) {
                frame.push_back((payload_len >> (i * 8)) & 0xFF);
            }
        }
        
        // توليد masking key عشوائي / Generate random masking key
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        
        uint8_t masking_key[4];
        for (int i = 0; i < 4; ++i) {
            masking_key[i] = dis(gen);
            frame.push_back(masking_key[i]);
        }
        
        // إضافة البيانات المقنعة / Add masked payload
        for (size_t i = 0; i < payload.size(); ++i) {
            frame.push_back(payload[i] ^ masking_key[i % 4]);
        }
        
        return frame;
    }
    
    // قراءة إطار WebSocket / Read WebSocket frame
    bool read_frame(Frame& frame) {
        // قراءة البايتين الأولين / Read first 2 bytes
        unsigned char header[2];
        if (recv(socket, reinterpret_cast<char*>(header), 2, 0) != 2) {
            return false;
        }
        
        frame.fin = (header[0] & 0x80) != 0;
        frame.rsv1 = (header[0] & 0x40) != 0;
        frame.rsv2 = (header[0] & 0x20) != 0;
        frame.rsv3 = (header[0] & 0x10) != 0;
        frame.opcode = header[0] & 0x0F;
        frame.mask = (header[1] & 0x80) != 0;
        frame.payload_length = header[1] & 0x7F;
        
        // قراءة طول البيانات الممتد / Read extended payload length
        if (frame.payload_length == 126) {
            unsigned char len_bytes[2];
            if (recv(socket, reinterpret_cast<char*>(len_bytes), 2, 0) != 2) {
                return false;
            }
            frame.payload_length = (len_bytes[0] << 8) | len_bytes[1];
        } else if (frame.payload_length == 127) {
            unsigned char len_bytes[8];
            if (recv(socket, reinterpret_cast<char*>(len_bytes), 8, 0) != 8) {
                return false;
            }
            frame.payload_length = 0;
            for (int i = 0; i < 8; ++i) {
                frame.payload_length = (frame.payload_length << 8) | len_bytes[i];
            }
        }
        
        // قراءة masking key (إذا موجود) / Read masking key (if present)
        if (frame.mask) {
            if (recv(socket, reinterpret_cast<char*>(frame.masking_key), 4, 0) != 4) {
                return false;
            }
        }
        
        // قراءة البيانات / Read payload data
        if (frame.payload_length > 0) {
            frame.payload_data.resize(frame.payload_length);
            uint64_t total_received = 0;
            while (total_received < frame.payload_length) {
                int received = recv(socket, 
                                   reinterpret_cast<char*>(frame.payload_data.data() + total_received),
                                   frame.payload_length - total_received, 0);
                if (received <= 0) {
                    return false;
                }
                total_received += received;
            }
            
            // فك القناع / Unmask
            if (frame.mask) {
                for (size_t i = 0; i < frame.payload_data.size(); ++i) {
                    frame.payload_data[i] ^= frame.masking_key[i % 4];
                }
            }
        }
        
        return true;
    }
};

WebSocket::WebSocket() : impl_(std::make_unique<Impl>()) {
}

WebSocket::~WebSocket() {
    close();
}

bool WebSocket::connect(const std::string& url) {
    // تحليل URL / Parse URL
    std::regex url_regex(R"(^(wss?)://([^:/]+)(?::(\d+))?(/.*)?$)");
    std::smatch matches;
    
    if (!std::regex_match(url, matches, url_regex)) {
        return false;
    }
    
    std::string scheme = matches[1].str();
    impl_->host = matches[2].str();
    impl_->port = matches[3].matched ? std::stoi(matches[3].str()) : (scheme == "wss" ? 443 : 80);
    impl_->path = matches[4].matched ? matches[4].str() : "/";
    
    // إنشاء socket / Create socket
    impl_->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (impl_->socket == INVALID_SOCKET) {
        return false;
    }
    
    // الاتصال بالخادم / Connect to server
    struct hostent* host_info = gethostbyname(impl_->host.c_str());
    if (!host_info) {
        closesocket(impl_->socket);
        impl_->socket = INVALID_SOCKET;
        return false;
    }
    
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(impl_->port);
    std::memcpy(&server_addr.sin_addr, host_info->h_addr_list[0], host_info->h_length);
    
    if (::connect(impl_->socket, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) {
        closesocket(impl_->socket);
        impl_->socket = INVALID_SOCKET;
        return false;
    }
    
    // توليد WebSocket key / Generate WebSocket key
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    unsigned char key_bytes[16];
    for (int i = 0; i < 16; ++i) {
        key_bytes[i] = dis(gen);
    }
    
    std::string ws_key = base64_encode(key_bytes, 16);
    
    // إرسال طلب الترقية / Send upgrade request
    std::ostringstream request;
    request << "GET " << impl_->path << " HTTP/1.1\r\n";
    request << "Host: " << impl_->host;
    if ((scheme == "ws" && impl_->port != 80) || (scheme == "wss" && impl_->port != 443)) {
        request << ":" << impl_->port;
    }
    request << "\r\n";
    request << "Upgrade: websocket\r\n";
    request << "Connection: Upgrade\r\n";
    request << "Sec-WebSocket-Key: " << ws_key << "\r\n";
    request << "Sec-WebSocket-Version: 13\r\n";
    request << "\r\n";
    
    std::string request_str = request.str();
    if (send(impl_->socket, request_str.c_str(), request_str.length(), 0) == SOCKET_ERROR) {
        closesocket(impl_->socket);
        impl_->socket = INVALID_SOCKET;
        return false;
    }
    
    // قراءة الاستجابة / Read response
    char buffer[4096];
    int received = recv(impl_->socket, buffer, sizeof(buffer) - 1, 0);
    if (received <= 0) {
        closesocket(impl_->socket);
        impl_->socket = INVALID_SOCKET;
        return false;
    }
    buffer[received] = '\0';
    
    std::string response(buffer);
    
    // التحقق من الترقية الناجحة / Verify successful upgrade
    if (response.find("101") == std::string::npos || 
        response.find("Upgrade: websocket") == std::string::npos) {
        closesocket(impl_->socket);
        impl_->socket = INVALID_SOCKET;
        return false;
    }
    
    // التحقق من Sec-WebSocket-Accept / Verify Sec-WebSocket-Accept
    std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::string accept_key = ws_key + magic;
    std::string sha1_result = sha1_hash(accept_key);
    std::string expected_accept = base64_encode(reinterpret_cast<const unsigned char*>(sha1_result.c_str()), 20);
    
    if (response.find("Sec-WebSocket-Accept: " + expected_accept) == std::string::npos) {
        closesocket(impl_->socket);
        impl_->socket = INVALID_SOCKET;
        return false;
    }
    
    impl_->connected = true;
    
    // استدعاء معالج الاتصال / Call connect handler
    if (impl_->connect_handler) {
        impl_->connect_handler();
    }
    
    return true;
}

void WebSocket::send_text(const std::string& message) {
    if (!impl_->connected || impl_->socket == INVALID_SOCKET) {
        return;
    }
    
    // تحويل إلى بايتات / Convert to bytes
    std::vector<uint8_t> payload(message.begin(), message.end());
    
    // بناء إطار نصي (opcode = 0x1) / Build text frame (opcode = 0x1)
    std::vector<uint8_t> frame = impl_->build_frame(payload, 0x01);
    
    // إرسال الإطار / Send frame
    send(impl_->socket, reinterpret_cast<const char*>(frame.data()), frame.size(), 0);
}

void WebSocket::send_binary(const std::vector<uint8_t>& data) {
    if (!impl_->connected || impl_->socket == INVALID_SOCKET) {
        return;
    }
    
    // بناء إطار ثنائي (opcode = 0x2) / Build binary frame (opcode = 0x2)
    std::vector<uint8_t> frame = impl_->build_frame(data, 0x02);
    
    // إرسال الإطار / Send frame
    send(impl_->socket, reinterpret_cast<const char*>(frame.data()), frame.size(), 0);
}

std::string WebSocket::receive() {
    if (!impl_->connected || impl_->socket == INVALID_SOCKET) {
        return "";
    }
    
    // قراءة إطار من الشبكة / Read frame from network
    Impl::Frame frame;
    if (!impl_->read_frame(frame)) {
        return "";
    }
    
    // معالجة حسب opcode / Process based on opcode
    switch (frame.opcode) {
        case 0x0: // Continuation frame
            // معالجة الرسائل المجزأة / Handle fragmented messages
            // Fragmented message handling
            {
                if (impl_->fragmented_message.empty()) {
                    // خطأ: continuation frame بدون initial frame
                    // Error: continuation frame without initial frame
                    return "";
                }
                
                // إضافة البيانات للرسالة المجزأة / Append to fragmented message
                impl_->fragmented_message.insert(
                    impl_->fragmented_message.end(),
                    frame.payload_data.begin(),
                    frame.payload_data.end()
                );
                
                // إذا كانت النهاية (FIN = 1)، أكمل الرسالة
                // If final frame (FIN = 1), complete the message
                if (frame.fin) {
                    std::string complete_message(
                        impl_->fragmented_message.begin(),
                        impl_->fragmented_message.end()
                    );
                    
                    // تحديد نوع الرسالة من impl_->fragmented_type
                    // Determine message type from impl_->fragmented_type
                    WebSocketMessageType msg_type = impl_->fragmented_type;
                    
                    // إضافة للطابور / Add to queue
                    {
                        std::lock_guard<std::mutex> lock(impl_->queue_mutex);
                        impl_->message_queue.push({complete_message, msg_type});
                    }
                    
                    // تنفيذ المعالج إذا وجد / Execute handler if exists
                    if (impl_->message_handler) {
                        impl_->message_handler(complete_message, msg_type);
                    }
                    
                    // تنظيف البيانات المؤقتة / Clear temporary data
                    impl_->fragmented_message.clear();
                    impl_->fragmented_type = WebSocketMessageType::Text;
                }
            }
            break;
            
        case 0x1: // Text frame
        {
            std::string message(frame.payload_data.begin(), frame.payload_data.end());
            
            // إذا لم تكن نهائية (FIN = 0)، هذه رسالة مجزأة
            // If not final (FIN = 0), this is a fragmented message
            if (!frame.fin) {
                // تخزين البيانات والنوع / Store data and type
                impl_->fragmented_message = frame.payload_data;
                impl_->fragmented_type = WebSocketMessageType::Text;
                return ""; // لم تكتمل الرسالة بعد / Message not complete yet
            }
            
            // إضافة للطابور / Add to queue
            {
                std::lock_guard<std::mutex> lock(impl_->queue_mutex);
                impl_->message_queue.push({message, WebSocketMessageType::Text});
            }
            
            // استدعاء معالج الرسائل / Call message handler
            if (impl_->message_handler) {
                impl_->message_handler(message, WebSocketMessageType::Text);
            }
            
            return message;
        }
        
        case 0x2: // Binary frame
        {
            std::string message(frame.payload_data.begin(), frame.payload_data.end());
            
            // إذا لم تكن نهائية (FIN = 0)، هذه رسالة مجزأة
            // If not final (FIN = 0), this is a fragmented message
            if (!frame.fin) {
                // تخزين البيانات والنوع / Store data and type
                impl_->fragmented_message = frame.payload_data;
                impl_->fragmented_type = WebSocketMessageType::Binary;
                return ""; // لم تكتمل الرسالة بعد / Message not complete yet
            }
            
            // إضافة للطابور / Add to queue
            {
                std::lock_guard<std::mutex> lock(impl_->queue_mutex);
                impl_->message_queue.push({message, WebSocketMessageType::Binary});
            }
            
            // استدعاء معالج الرسائل / Call message handler
            if (impl_->message_handler) {
                impl_->message_handler(message, WebSocketMessageType::Binary);
            }
            
            return message;
        }
        
        case 0x8: // Close frame
            close();
            break;
            
        case 0x9: // Ping frame
        {
            // إرسال Pong / Send Pong
            std::vector<uint8_t> pong_frame = impl_->build_frame(frame.payload_data, 0x0A);
            send(impl_->socket, reinterpret_cast<const char*>(pong_frame.data()), pong_frame.size(), 0);
            break;
        }
        
        case 0xA: // Pong frame
            // لا حاجة لفعل شيء / No action needed
            break;
            
        default:
            break;
    }
    
    return "";
}

bool WebSocket::has_message() const {
    std::lock_guard<std::mutex> lock(impl_->queue_mutex);
    return !impl_->message_queue.empty();
}

void WebSocket::close() {
    if (impl_->connected && impl_->socket != INVALID_SOCKET) {
        closesocket(impl_->socket);
        impl_->socket = INVALID_SOCKET;
        impl_->connected = false;
        
        if (impl_->close_handler) {
            impl_->close_handler();
        }
    }
}

bool WebSocket::is_open() const {
    return impl_->connected;
}

void WebSocket::on_message(std::function<void(const std::string&, WebSocketMessageType)> handler) {
    impl_->message_handler = handler;
}

void WebSocket::on_connect(std::function<void()> handler) {
    impl_->connect_handler = handler;
}

void WebSocket::on_close(std::function<void()> handler) {
    impl_->close_handler = handler;
}

// ============================================================================
// Utility Functions - دوال مساعدة
// ============================================================================

std::string url_encode(const std::string& str) {
    std::ostringstream oss;
    oss << std::hex << std::uppercase;
    
    for (unsigned char c : str) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            oss << c;
        } else {
            oss << '%' << std::setw(2) << static_cast<int>(c);
        }
    }
    
    return oss.str();
}

std::string url_decode(const std::string& str) {
    std::ostringstream oss;
    
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%' && i + 2 < str.length()) {
            std::string hex = str.substr(i + 1, 2);
            int value = std::stoi(hex, nullptr, 16);
            oss << static_cast<char>(value);
            i += 2;
        } else if (str[i] == '+') {
            oss << ' ';
        } else {
            oss << str[i];
        }
    }
    
    return oss.str();
}

std::map<std::string, std::string> parse_query_string(const std::string& query) {
    std::map<std::string, std::string> params;
    
    auto pairs = split(query, '&');
    for (const auto& pair : pairs) {
        size_t eq = pair.find('=');
        if (eq != std::string::npos) {
            std::string key = url_decode(pair.substr(0, eq));
            std::string value = url_decode(pair.substr(eq + 1));
            params[key] = value;
        }
    }
    
    return params;
}

std::string build_query_string(const std::map<std::string, std::string>& params) {
    std::ostringstream oss;
    bool first = true;
    
    for (const auto& [key, value] : params) {
        if (!first) oss << '&';
        oss << url_encode(key) << '=' << url_encode(value);
        first = false;
    }
    
    return oss.str();
}

UrlParts parse_url(const std::string& url) {
    UrlParts parts;
    
    // تحليل النظام (scheme) / Parse scheme
    size_t scheme_end = url.find("://");
    if (scheme_end != std::string::npos) {
        parts.scheme = url.substr(0, scheme_end);
        scheme_end += 3;
    } else {
        scheme_end = 0;
        parts.scheme = "http";
    }
    
    // تحليل المضيف والمنفذ / Parse host and port
    size_t path_start = url.find('/', scheme_end);
    std::string host_port = url.substr(scheme_end, 
                                       path_start == std::string::npos ? 
                                       std::string::npos : path_start - scheme_end);
    
    size_t port_pos = host_port.find(':');
    if (port_pos != std::string::npos) {
        parts.host = host_port.substr(0, port_pos);
        parts.port = static_cast<uint16_t>(std::stoi(host_port.substr(port_pos + 1)));
    } else {
        parts.host = host_port;
        parts.port = (parts.scheme == "https" || parts.scheme == "wss") ? 443 : 80;
    }
    
    // تحليل المسار والاستعلام / Parse path and query
    if (path_start != std::string::npos) {
        size_t query_start = url.find('?', path_start);
        size_t fragment_start = url.find('#', path_start);
        
        if (query_start != std::string::npos) {
            parts.path = url.substr(path_start, query_start - path_start);
            
            size_t query_end = fragment_start != std::string::npos ? 
                              fragment_start : std::string::npos;
            parts.query = url.substr(query_start + 1, 
                                    query_end == std::string::npos ? 
                                    std::string::npos : query_end - query_start - 1);
        } else {
            size_t path_end = fragment_start != std::string::npos ? 
                             fragment_start : std::string::npos;
            parts.path = url.substr(path_start, path_end == std::string::npos ? 
                                   std::string::npos : path_end - path_start);
        }
        
        if (fragment_start != std::string::npos) {
            parts.fragment = url.substr(fragment_start + 1);
        }
    } else {
        parts.path = "/";
    }
    
    return parts;
}

std::string build_url(const UrlParts& parts) {
    std::ostringstream oss;
    
    oss << parts.scheme << "://" << parts.host;
    
    if ((parts.scheme == "http" && parts.port != 80) ||
        (parts.scheme == "https" && parts.port != 443)) {
        oss << ':' << parts.port;
    }
    
    oss << parts.path;
    
    if (!parts.query.empty()) {
        oss << '?' << parts.query;
    }
    
    if (!parts.fragment.empty()) {
        oss << '#' << parts.fragment;
    }
    
    return oss.str();
}

} // namespace http
} // namespace stdlib
} // namespace sad

// الحمد لله رب العالمين
