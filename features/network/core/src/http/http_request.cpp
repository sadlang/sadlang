// بسم الله الرحمن الرحيم
// HTTP Request Implementation

#include "http_request.h"
#include "http_base.h"
#include <sstream>
#include <algorithm>

namespace sad {
namespace network {
namespace http {

// ==========================================
// Implementation Class
// ==========================================

class HttpRequest::Impl {
public:
    HttpMethod method_ = HttpMethod::GET;
    std::string path_ = "/";
    HttpVersion version_ = HttpVersion::HTTP_1_1;
    HeadersCI headers_;
    QueryParams query_params_;
    std::string body_;
    std::map<std::string, std::string> cookies_;
    std::string url_;
};

// ==========================================
// Constructors
// ==========================================

HttpRequest::HttpRequest() : pImpl(std::make_unique<Impl>()) {}

HttpRequest::HttpRequest(HttpMethod method, const std::string& path)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->method_ = method;
    pImpl->path_ = path;
}

HttpRequest::HttpRequest(const HttpRequest& other)
    : pImpl(std::make_unique<Impl>(*other.pImpl)) {}

HttpRequest::HttpRequest(HttpRequest&& other) noexcept = default;

HttpRequest::~HttpRequest() = default;

HttpRequest& HttpRequest::operator=(const HttpRequest& other) {
    if (this != &other) {
        *pImpl = *other.pImpl;
    }
    return *this;
}

HttpRequest& HttpRequest::operator=(HttpRequest&& other) noexcept = default;

// ==========================================
// Basic Accessors
// ==========================================

HttpMethod HttpRequest::method() const {
    return pImpl->method_;
}

void HttpRequest::set_method(HttpMethod method) {
    pImpl->method_ = method;
}

const std::string& HttpRequest::path() const {
    return pImpl->path_;
}

void HttpRequest::set_path(const std::string& path) {
    pImpl->path_ = path;
}

HttpVersion HttpRequest::version() const {
    return pImpl->version_;
}

void HttpRequest::set_version(HttpVersion version) {
    pImpl->version_ = version;
}

// ==========================================
// Headers
// ==========================================

const HeadersCI& HttpRequest::headers() const {
    return pImpl->headers_;
}

std::string HttpRequest::header(const std::string& key) const {
    auto it = pImpl->headers_.find(key);
    return (it != pImpl->headers_.end()) ? it->second : "";
}

bool HttpRequest::has_header(const std::string& key) const {
    return pImpl->headers_.find(key) != pImpl->headers_.end();
}

void HttpRequest::set_header(const std::string& key, const std::string& value) {
    pImpl->headers_[key] = value;
}

void HttpRequest::remove_header(const std::string& key) {
    pImpl->headers_.erase(key);
}

void HttpRequest::clear_headers() {
    pImpl->headers_.clear();
}

// ==========================================
// Query Parameters
// ==========================================

const QueryParams& HttpRequest::query_params() const {
    return pImpl->query_params_;
}

std::string HttpRequest::query_param(const std::string& key) const {
    auto it = pImpl->query_params_.find(key);
    return (it != pImpl->query_params_.end()) ? it->second : "";
}

bool HttpRequest::has_query_param(const std::string& key) const {
    return pImpl->query_params_.find(key) != pImpl->query_params_.end();
}

void HttpRequest::set_query_param(const std::string& key, const std::string& value) {
    pImpl->query_params_[key] = value;
}

void HttpRequest::remove_query_param(const std::string& key) {
    pImpl->query_params_.erase(key);
}

void HttpRequest::clear_query_params() {
    pImpl->query_params_.clear();
}

// ==========================================
// Body
// ==========================================

const std::string& HttpRequest::body() const {
    return pImpl->body_;
}

void HttpRequest::set_body(const std::string& body) {
    pImpl->body_ = body;
    set_header(headers::ContentLength, std::to_string(body.size()));
}

void HttpRequest::set_json(const std::string& json) {
    set_body(json);
    set_header(headers::ContentType, mime_types::ApplicationJson);
}

void HttpRequest::set_form_data(const std::map<std::string, std::string>& data) {
    std::ostringstream form;
    bool first = true;
    
    for (const auto& [key, value] : data) {
        if (!first) form << "&";
        first = false;
        form << url_encode(key) << "=" << url_encode(value);
    }
    
    set_body(form.str());
    set_header(headers::ContentType, mime_types::ApplicationFormUrlencoded);
}

void HttpRequest::clear_body() {
    pImpl->body_.clear();
    remove_header(headers::ContentLength);
}

// ==========================================
// Cookies
// ==========================================

std::string HttpRequest::cookie(const std::string& name) const {
    auto it = pImpl->cookies_.find(name);
    return (it != pImpl->cookies_.end()) ? it->second : "";
}

bool HttpRequest::has_cookie(const std::string& name) const {
    return pImpl->cookies_.find(name) != pImpl->cookies_.end();
}

void HttpRequest::set_cookie(const std::string& name, const std::string& value) {
    pImpl->cookies_[name] = value;
    
    // Build Cookie header
    std::ostringstream cookie_header;
    bool first = true;
    for (const auto& [n, v] : pImpl->cookies_) {
        if (!first) cookie_header << "; ";
        first = false;
        cookie_header << n << "=" << v;
    }
    
    set_header(headers::Cookie, cookie_header.str());
}

const std::map<std::string, std::string>& HttpRequest::cookies() const {
    return pImpl->cookies_;
}

void HttpRequest::clear_cookies() {
    pImpl->cookies_.clear();
    remove_header(headers::Cookie);
}

// ==========================================
// URL and Path
// ==========================================

const std::string& HttpRequest::url() const {
    return pImpl->url_;
}

void HttpRequest::set_url(const std::string& url) {
    pImpl->url_ = url;
    
    // Parse URL to extract path and query parameters
    size_t query_pos = url.find('?');
    if (query_pos != std::string::npos) {
        pImpl->path_ = url.substr(0, query_pos);
        std::string query = url.substr(query_pos);
        pImpl->query_params_ = parse_query_string(query);
    } else {
        pImpl->path_ = url;
    }
}

std::string HttpRequest::build_url() const {
    std::string url = pImpl->path_;
    if (!pImpl->query_params_.empty()) {
        url += build_query_string(pImpl->query_params_);
    }
    return url;
}

// ==========================================
// Serialization
// ==========================================

std::string HttpRequest::to_string() const {
    std::ostringstream request;
    
    // Request line: METHOD PATH HTTP/VERSION
    request << method_to_string(pImpl->method_) << " "
            << build_url() << " "
            << version_to_string(pImpl->version_) << "\r\n";
    
    // Headers
    for (const auto& [key, value] : pImpl->headers_) {
        request << key << ": " << value << "\r\n";
    }
    
    // Empty line
    request << "\r\n";
    
    // Body
    if (!pImpl->body_.empty()) {
        request << pImpl->body_;
    }
    
    return request.str();
}

bool HttpRequest::from_string(const std::string& request_str) {
    std::istringstream stream(request_str);
    std::string line;
    
    // Parse request line
    if (!std::getline(stream, line) || line.empty()) {
        return false;
    }
    
    // Remove \r if present
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    
    // Parse: METHOD PATH HTTP/VERSION
    std::istringstream line_stream(line);
    std::string method_str, path_and_query, version_str;
    
    if (!(line_stream >> method_str >> path_and_query >> version_str)) {
        return false;
    }
    
    try {
        pImpl->method_ = string_to_method(method_str);
    } catch (...) {
        return false;
    }
    
    // Parse path and query
    set_url(path_and_query);
    
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
    
    // Parse cookies from Cookie header
    if (has_header(headers::Cookie)) {
        pImpl->cookies_.clear();
        std::string cookie_header = header(headers::Cookie);
        size_t start = 0;
        size_t pos;
        
        while ((pos = cookie_header.find(';', start)) != std::string::npos) {
            std::string cookie_pair = cookie_header.substr(start, pos - start);
            size_t eq = cookie_pair.find('=');
            if (eq != std::string::npos) {
                std::string name = cookie_pair.substr(0, eq);
                std::string value = cookie_pair.substr(eq + 1);
                // Trim spaces
                name.erase(0, name.find_first_not_of(" \t"));
                name.erase(name.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                pImpl->cookies_[name] = value;
            }
            start = pos + 1;
        }
        
        // Last cookie
        if (start < cookie_header.size()) {
            std::string cookie_pair = cookie_header.substr(start);
            size_t eq = cookie_pair.find('=');
            if (eq != std::string::npos) {
                std::string name = cookie_pair.substr(0, eq);
                std::string value = cookie_pair.substr(eq + 1);
                name.erase(0, name.find_first_not_of(" \t"));
                name.erase(name.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                pImpl->cookies_[name] = value;
            }
        }
    }
    
    // Parse body (rest of the stream)
    pImpl->body_.clear();
    std::string body_content;
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
