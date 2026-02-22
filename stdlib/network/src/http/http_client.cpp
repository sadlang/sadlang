// بسم الله الرحمن الرحيم
// HTTP Client Implementation

#include "http_client.h"
#include "http_url.h"
#include "tcp_socket.h"
#include <sstream>
#include <algorithm>

namespace {
// Simple Base64 encoder for HTTP Basic Auth
static const char BASE64_CHARS[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string base64_encode_simple(const std::string& input) {
    std::string result;
    result.reserve(((input.size() + 2) / 3) * 4);
    const auto* bytes = reinterpret_cast<const unsigned char*>(input.data());
    size_t len = input.size();
    for (size_t i = 0; i < len; i += 3) {
        unsigned int b = (bytes[i] << 16);
        if (i + 1 < len) b |= (bytes[i + 1] << 8);
        if (i + 2 < len) b |= bytes[i + 2];
        result += BASE64_CHARS[(b >> 18) & 0x3F];
        result += BASE64_CHARS[(b >> 12) & 0x3F];
        result += (i + 1 < len) ? BASE64_CHARS[(b >> 6) & 0x3F] : '=';
        result += (i + 2 < len) ? BASE64_CHARS[b & 0x3F] : '=';
    }
    return result;
}
} // anonymous namespace

namespace sad {
namespace network {
namespace http {

// ==========================================
// Implementation Class
// ==========================================

class HttpClient::Impl {
public:
    std::string base_url_;
    HeadersCI default_headers_;
    CookieJar cookie_jar_;
    int timeout_ms_ = 30000; // 30 seconds default
    int max_redirects_ = 5;
    bool follow_redirects_ = true;
    bool enable_cookies_ = true;
    bool connection_pool_enabled_ = false;
    int max_connections_ = 10;
    std::string last_error_;
    bool is_ok_ = true;
    
    HttpResponse send_request(const HttpRequest& request);
    HttpResponse handle_redirects(HttpRequest request, int redirect_count = 0);
    std::string build_full_url(const std::string& url);
};

// ==========================================
// Constructors
// ==========================================

HttpClient::HttpClient() : pImpl(std::make_unique<Impl>()) {
    set_user_agent("Sad-HTTP/1.0");
}

HttpClient::HttpClient(const std::string& base_url) : HttpClient() {
    pImpl->base_url_ = base_url;
}

HttpClient::~HttpClient() = default;

HttpClient::HttpClient(HttpClient&&) noexcept = default;
HttpClient& HttpClient::operator=(HttpClient&&) noexcept = default;

// ==========================================
// HTTP Methods
// ==========================================

HttpResponse HttpClient::get(const std::string& url) {
    HttpRequest request(HttpMethod::GET, url);
    return send(request);
}

HttpResponse HttpClient::post(const std::string& url, const std::string& body) {
    HttpRequest request(HttpMethod::POST, url);
    request.set_body(body);
    return send(request);
}

HttpResponse HttpClient::put(const std::string& url, const std::string& body) {
    HttpRequest request(HttpMethod::PUT, url);
    request.set_body(body);
    return send(request);
}

HttpResponse HttpClient::delete_(const std::string& url) {
    HttpRequest request(HttpMethod::DELETE, url);
    return send(request);
}

HttpResponse HttpClient::patch(const std::string& url, const std::string& body) {
    HttpRequest request(HttpMethod::PATCH, url);
    request.set_body(body);
    return send(request);
}

HttpResponse HttpClient::head(const std::string& url) {
    HttpRequest request(HttpMethod::HEAD, url);
    return send(request);
}

HttpResponse HttpClient::send(const HttpRequest& request) {
    HttpRequest req = request;
    
    // Apply default headers
    for (const auto& [key, value] : pImpl->default_headers_) {
        if (!req.has_header(key)) {
            req.set_header(key, value);
        }
    }
    
    // Apply cookies if enabled
    if (pImpl->enable_cookies_) {
        std::string url = pImpl->build_full_url(req.path());
        auto cookies = pImpl->cookie_jar_.get_cookies(url);
        for (const auto& cookie : cookies) {
            req.set_cookie(cookie.name(), cookie.value());
        }
    }
    
    // Handle redirects if enabled
    if (pImpl->follow_redirects_) {
        return pImpl->handle_redirects(req, 0);
    } else {
        return pImpl->send_request(req);
    }
}

// ==========================================
// Implementation Methods
// ==========================================

HttpResponse HttpClient::Impl::send_request(const HttpRequest& request) {
    try {
        // Parse URL
        std::string full_url = build_full_url(request.path());
        URL url(full_url);
        
        if (!url.is_valid()) {
            last_error_ = "Invalid URL: " + full_url;
            is_ok_ = false;
            return HttpResponse(HttpStatus::BadRequest);
        }
        
        // Create TCP connection
        TcpSocket socket;
        socket.set_receive_timeout(timeout_ms_);
        socket.set_send_timeout(timeout_ms_);
        
        std::string host = url.host();
        uint16_t port = url.effective_port();
        
        // Connect
        socket.connect(host, port);
        
        // Build and send HTTP request
        HttpRequest req = request;
        req.set_header(headers::Host, host);
        req.set_path(url.path_and_query());
        
        std::string request_str = req.to_string();
        socket.send_all(request_str);
        
        // Receive response
        std::string response_data;
        std::string chunk;
        
        // Read headers first
        bool headers_complete = false;
        while (!headers_complete) {
            chunk = socket.receive(4096);
            if (chunk.empty()) break;
            
            response_data += chunk;
            
            // Check if headers are complete (\r\n\r\n)
            if (response_data.find("\r\n\r\n") != std::string::npos) {
                headers_complete = true;
            }
        }
        
        // Parse response
        HttpResponse response;
        if (!response.from_string(response_data)) {
            last_error_ = "Failed to parse HTTP response";
            is_ok_ = false;
            return HttpResponse(HttpStatus::InternalServerError);
        }
        
        // Read body if Content-Length is specified
        if (response.has_header(headers::ContentLength)) {
            try {
                size_t content_length = std::stoul(response.header(headers::ContentLength));
                size_t body_received = response.body().size();
                
                while (body_received < content_length) {
                    chunk = socket.receive(4096);
                    if (chunk.empty()) break;
                    response.set_body(response.body() + chunk);
                    body_received += chunk.size();
                }
            } catch (...) {}
        }
        
        // Store cookies if enabled
        if (enable_cookies_ && response.has_header(headers::SetCookie)) {
            Cookie cookie;
            if (cookie.from_set_cookie_header(response.header(headers::SetCookie))) {
                cookie_jar_.add_cookie(cookie);
            }
        }
        
        socket.close();
        
        is_ok_ = true;
        last_error_.clear();
        return response;
        
    } catch (const std::exception& ex) {
        last_error_ = std::string("HTTP request failed: ") + ex.what();
        is_ok_ = false;
        return HttpResponse(HttpStatus::InternalServerError);
    }
}

HttpResponse HttpClient::Impl::handle_redirects(HttpRequest request, int redirect_count) {
    if (redirect_count >= max_redirects_) {
        last_error_ = "Too many redirects";
        is_ok_ = false;
        return HttpResponse(HttpStatus::InternalServerError);
    }
    
    HttpResponse response = send_request(request);
    
    // Check if redirect
    if (response.is_redirect() && response.has_header(headers::Location)) {
        std::string location = response.header(headers::Location);
        
        // Handle relative URLs
        URL current_url(build_full_url(request.path()));
        URL redirect_url(location);
        
        if (redirect_url.is_relative()) {
            redirect_url = URL::resolve(current_url, redirect_url);
        }
        
        // Create new request for redirect
        HttpRequest redirect_request;
        
        // Use GET for redirects (per HTTP spec)
        redirect_request.set_method(HttpMethod::GET);
        redirect_request.set_path(redirect_url.path_and_query());
        
        // Copy headers
        for (const auto& [key, value] : request.headers()) {
            redirect_request.set_header(key, value);
        }
        
        return handle_redirects(redirect_request, redirect_count + 1);
    }
    
    return response;
}

std::string HttpClient::Impl::build_full_url(const std::string& url) {
    if (url.find("http://") == 0 || url.find("https://") == 0) {
        return url;
    }
    
    if (base_url_.empty()) {
        return url;
    }
    
    // Remove trailing slash from base_url
    std::string base = base_url_;
    if (!base.empty() && base.back() == '/') {
        base.pop_back();
    }
    
    // Add leading slash to url if needed
    std::string path = url;
    if (!path.empty() && path[0] != '/') {
        path = "/" + path;
    }
    
    return base + path;
}

// ==========================================
// Configuration
// ==========================================

void HttpClient::set_base_url(const std::string& base_url) {
    pImpl->base_url_ = base_url;
}

const std::string& HttpClient::base_url() const {
    return pImpl->base_url_;
}

void HttpClient::set_default_header(const std::string& key, const std::string& value) {
    pImpl->default_headers_[key] = value;
}

void HttpClient::remove_default_header(const std::string& key) {
    pImpl->default_headers_.erase(key);
}

void HttpClient::set_user_agent(const std::string& user_agent) {
    set_default_header(headers::UserAgent, user_agent);
}

void HttpClient::set_timeout(int timeout_ms) {
    pImpl->timeout_ms_ = timeout_ms;
}

int HttpClient::timeout() const {
    return pImpl->timeout_ms_;
}

void HttpClient::set_max_redirects(int max_redirects) {
    pImpl->max_redirects_ = max_redirects;
}

void HttpClient::set_follow_redirects(bool follow) {
    pImpl->follow_redirects_ = follow;
}

// ==========================================
// Cookie Management
// ==========================================

void HttpClient::enable_cookies(bool enable) {
    pImpl->enable_cookies_ = enable;
}

void HttpClient::add_cookie(const Cookie& cookie) {
    pImpl->cookie_jar_.add_cookie(cookie);
}

CookieJar& HttpClient::cookie_jar() {
    return pImpl->cookie_jar_;
}

const CookieJar& HttpClient::cookie_jar() const {
    return pImpl->cookie_jar_;
}

void HttpClient::clear_cookies() {
    pImpl->cookie_jar_.clear();
}

// ==========================================
// Authentication
// ==========================================

void HttpClient::set_basic_auth(const std::string& username, const std::string& password) {
    std::string credentials = username + ":" + password;
    std::string auth = "Basic " + base64_encode_simple(credentials);
    set_default_header(headers::Authorization, auth);
}

void HttpClient::set_bearer_token(const std::string& token) {
    set_default_header(headers::Authorization, "Bearer " + token);
}

void HttpClient::clear_auth() {
    remove_default_header(headers::Authorization);
}

// ==========================================
// Connection Pooling
// ==========================================

void HttpClient::enable_connection_pool(bool enable) {
    pImpl->connection_pool_enabled_ = enable;
}

void HttpClient::set_max_connections(int max_connections) {
    pImpl->max_connections_ = max_connections;
}

void HttpClient::close_all_connections() {
    // Reset connection pool state
    // (AR) إعادة تعيين حالة مجموعة الاتصالات
    pImpl->connection_pool_enabled_ = false;
    pImpl->max_connections_ = 10;
}

// ==========================================
// Error Handling
// ==========================================

const std::string& HttpClient::last_error() const {
    return pImpl->last_error_;
}

bool HttpClient::is_ok() const {
    return pImpl->is_ok_;
}

} // namespace http
} // namespace network
} // namespace sad
