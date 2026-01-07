// بسم الله الرحمن الرحيم
// HTTP Response Implementation

#include "network/http/http_response.h"
#include "network/http/http_base.h"
#include <sstream>
#include <ctime>

namespace sad {
namespace network {
namespace http {

// ==========================================
// Implementation Class
// ==========================================

class HttpResponse::Impl {
public:
    HttpStatus status_ = HttpStatus::OK;
    HttpVersion version_ = HttpVersion::HTTP_1_1;
    HeadersCI headers_;
    std::string body_;
};

// ==========================================
// Constructors
// ==========================================

HttpResponse::HttpResponse() : pImpl(std::make_unique<Impl>()) {
    // Set default headers
    set_header(headers::Server, "Sad/1.0");
    set_header(headers::ContentType, mime_types::TextPlain);
}

HttpResponse::HttpResponse(HttpStatus status) : HttpResponse() {
    pImpl->status_ = status;
}

HttpResponse::HttpResponse(HttpStatus status, const std::string& body)
    : HttpResponse(status) {
    set_body(body);
}

HttpResponse::HttpResponse(const HttpResponse& other)
    : pImpl(std::make_unique<Impl>(*other.pImpl)) {}

HttpResponse::HttpResponse(HttpResponse&& other) noexcept = default;

HttpResponse::~HttpResponse() = default;

HttpResponse& HttpResponse::operator=(const HttpResponse& other) {
    if (this != &other) {
        *pImpl = *other.pImpl;
    }
    return *this;
}

HttpResponse& HttpResponse::operator=(HttpResponse&& other) noexcept = default;

// ==========================================
// Status
// ==========================================

HttpStatus HttpResponse::status() const {
    return pImpl->status_;
}

void HttpResponse::set_status(HttpStatus status) {
    pImpl->status_ = status;
}

int HttpResponse::status_code() const {
    return static_cast<int>(pImpl->status_);
}

std::string HttpResponse::status_text() const {
    return status_to_string(pImpl->status_);
}

std::string HttpResponse::status_text_arabic() const {
    return status_to_arabic(pImpl->status_);
}

bool HttpResponse::is_success() const {
    int code = status_code();
    return code >= 200 && code < 300;
}

bool HttpResponse::is_redirect() const {
    int code = status_code();
    return code >= 300 && code < 400;
}

bool HttpResponse::is_client_error() const {
    int code = status_code();
    return code >= 400 && code < 500;
}

bool HttpResponse::is_server_error() const {
    int code = status_code();
    return code >= 500 && code < 600;
}

// ==========================================
// HTTP Version
// ==========================================

HttpVersion HttpResponse::version() const {
    return pImpl->version_;
}

void HttpResponse::set_version(HttpVersion version) {
    pImpl->version_ = version;
}

// ==========================================
// Headers
// ==========================================

const HeadersCI& HttpResponse::headers() const {
    return pImpl->headers_;
}

std::string HttpResponse::header(const std::string& key) const {
    auto it = pImpl->headers_.find(key);
    return (it != pImpl->headers_.end()) ? it->second : "";
}

bool HttpResponse::has_header(const std::string& key) const {
    return pImpl->headers_.find(key) != pImpl->headers_.end();
}

void HttpResponse::set_header(const std::string& key, const std::string& value) {
    pImpl->headers_[key] = value;
}

void HttpResponse::remove_header(const std::string& key) {
    pImpl->headers_.erase(key);
}

void HttpResponse::clear_headers() {
    pImpl->headers_.clear();
}

// ==========================================
// Body
// ==========================================

const std::string& HttpResponse::body() const {
    return pImpl->body_;
}

void HttpResponse::set_body(const std::string& body) {
    pImpl->body_ = body;
    set_header(headers::ContentLength, std::to_string(body.size()));
}

void HttpResponse::set_json(const std::string& json) {
    set_body(json);
    set_header(headers::ContentType, mime_types::ApplicationJson);
}

void HttpResponse::set_html(const std::string& html) {
    set_body(html);
    set_header(headers::ContentType, mime_types::TextHtml);
}

void HttpResponse::set_text(const std::string& text) {
    set_body(text);
    set_header(headers::ContentType, mime_types::TextPlain);
}

void HttpResponse::clear_body() {
    pImpl->body_.clear();
    remove_header(headers::ContentLength);
}

// ==========================================
// Cookies
// ==========================================

void HttpResponse::set_cookie(const std::string& name, const std::string& value,
                              const std::string& path, int max_age) {
    std::ostringstream cookie;
    cookie << name << "=" << value;
    
    if (!path.empty()) {
        cookie << "; Path=" << path;
    }
    
    if (max_age > 0) {
        cookie << "; Max-Age=" << max_age;
    }
    
    // Note: In a real implementation, we'd append to Set-Cookie header
    // allowing multiple cookies. For simplicity, we set one cookie here.
    set_header(headers::SetCookie, cookie.str());
}

void HttpResponse::set_cookie_full(const std::string& name, const std::string& value,
                                   const std::string& path, const std::string& domain,
                                   int max_age, bool secure, bool http_only) {
    std::ostringstream cookie;
    cookie << name << "=" << value;
    
    if (!path.empty()) {
        cookie << "; Path=" << path;
    }
    
    if (!domain.empty()) {
        cookie << "; Domain=" << domain;
    }
    
    if (max_age > 0) {
        cookie << "; Max-Age=" << max_age;
    }
    
    if (secure) {
        cookie << "; Secure";
    }
    
    if (http_only) {
        cookie << "; HttpOnly";
    }
    
    set_header(headers::SetCookie, cookie.str());
}

void HttpResponse::delete_cookie(const std::string& name, const std::string& path) {
    set_cookie(name, "", path, 0);
    // Add Expires header in the past
    std::ostringstream cookie;
    cookie << name << "=; Path=" << path << "; Expires=Thu, 01 Jan 1970 00:00:00 GMT";
    set_header(headers::SetCookie, cookie.str());
}

// ==========================================
// Convenience Methods
// ==========================================

void HttpResponse::redirect(const std::string& url, bool permanent) {
    set_status(permanent ? HttpStatus::MovedPermanently : HttpStatus::Found);
    set_header(headers::Location, url);
    set_text("Redirecting to " + url);
}

void HttpResponse::set_cors(const std::string& origin) {
    set_header(headers::AccessControlAllowOrigin, origin);
    set_header(headers::AccessControlAllowMethods, "GET, POST, PUT, DELETE, OPTIONS, PATCH");
    set_header(headers::AccessControlAllowHeaders, "Content-Type, Authorization");
}

void HttpResponse::enable_compression(bool enable) {
    if (enable) {
        set_header(headers::ContentEncoding, "gzip");
    } else {
        remove_header(headers::ContentEncoding);
    }
}

// ==========================================
// Serialization
// ==========================================

std::string HttpResponse::to_string() const {
    std::ostringstream response;
    
    // Status line: HTTP/VERSION STATUS_CODE STATUS_TEXT
    response << version_to_string(pImpl->version_) << " "
             << status_code() << " "
             << status_text() << "\r\n";
    
    // Add Date header if not present
    if (!has_header(headers::Date)) {
        std::time_t now = std::time(nullptr);
        char date_buf[100];
        std::strftime(date_buf, sizeof(date_buf), "%a, %d %b %Y %H:%M:%S GMT",
                     std::gmtime(&now));
        const_cast<HttpResponse*>(this)->set_header(headers::Date, date_buf);
    }
    
    // Headers
    for (const auto& [key, value] : pImpl->headers_) {
        response << key << ": " << value << "\r\n";
    }
    
    // Empty line
    response << "\r\n";
    
    // Body
    if (!pImpl->body_.empty()) {
        response << pImpl->body_;
    }
    
    return response.str();
}

bool HttpResponse::from_string(const std::string& response_str) {
    std::istringstream stream(response_str);
    std::string line;
    
    // Parse status line
    if (!std::getline(stream, line) || line.empty()) {
        return false;
    }
    
    // Remove \r if present
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    
    // Parse: HTTP/VERSION STATUS_CODE STATUS_TEXT
    std::istringstream line_stream(line);
    std::string version_str;
    int status_code;
    
    if (!(line_stream >> version_str >> status_code)) {
        return false;
    }
    
    pImpl->status_ = static_cast<HttpStatus>(status_code);
    
    // Parse headers
    pImpl->headers_.clear();
    while (std::getline(stream, line) && !line.empty() && line != "\r") {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            std::string value = line.substr(colon + 1);
            
            // Trim leading spaces from value
            size_t first = value.find_first_not_of(" \t");
            if (first != std::string::npos) {
                value = value.substr(first);
            }
            
            pImpl->headers_[key] = value;
        }
    }
    
    // Parse body (rest of the stream)
    pImpl->body_.clear();
    while (std::getline(stream, line)) {
        pImpl->body_ += line + "\n";
    }
    
    // Remove trailing newline if present
    if (!pImpl->body_.empty() && pImpl->body_.back() == '\n') {
        pImpl->body_.pop_back();
    }
    
    return true;
}

} // namespace http
} // namespace network
} // namespace sad
