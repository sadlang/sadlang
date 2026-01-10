// بسم الله الرحمن الرحيم
// HTTP Cookie Implementation

#include "http_cookie.h"
#include <sstream>
#include <algorithm>
#include <ctime>

namespace sad {
namespace network {
namespace http {

// ==========================================
// Cookie Implementation Class
// ==========================================

class Cookie::Impl {
public:
    std::string name_;
    std::string value_;
    std::string domain_;
    std::string path_ = "/";
    int max_age_ = -1; // -1 = session cookie
    std::chrono::system_clock::time_point expires_;
    bool secure_ = false;
    bool http_only_ = false;
    SameSite same_site_ = SameSite::Lax;
};

// ==========================================
// Cookie Constructors
// ==========================================

Cookie::Cookie() : pImpl(std::make_unique<Impl>()) {}

Cookie::Cookie(const std::string& name, const std::string& value)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->name_ = name;
    pImpl->value_ = value;
}

Cookie::Cookie(const Cookie& other)
    : pImpl(std::make_unique<Impl>(*other.pImpl)) {}

Cookie::Cookie(Cookie&& other) noexcept = default;

Cookie::~Cookie() = default;

Cookie& Cookie::operator=(const Cookie& other) {
    if (this != &other) {
        *pImpl = *other.pImpl;
    }
    return *this;
}

Cookie& Cookie::operator=(Cookie&& other) noexcept = default;

// ==========================================
// Basic Properties
// ==========================================

const std::string& Cookie::name() const { return pImpl->name_; }
void Cookie::set_name(const std::string& name) { pImpl->name_ = name; }

const std::string& Cookie::value() const { return pImpl->value_; }
void Cookie::set_value(const std::string& value) { pImpl->value_ = value; }

// ==========================================
// Domain & Path
// ==========================================

const std::string& Cookie::domain() const { return pImpl->domain_; }
void Cookie::set_domain(const std::string& domain) { pImpl->domain_ = domain; }

const std::string& Cookie::path() const { return pImpl->path_; }
void Cookie::set_path(const std::string& path) { pImpl->path_ = path; }

// ==========================================
// Expiration
// ==========================================

int Cookie::max_age() const { return pImpl->max_age_; }
void Cookie::set_max_age(int seconds) { pImpl->max_age_ = seconds; }

std::chrono::system_clock::time_point Cookie::expires() const {
    return pImpl->expires_;
}

void Cookie::set_expires(const std::chrono::system_clock::time_point& time) {
    pImpl->expires_ = time;
}

bool Cookie::is_expired() const {
    if (pImpl->max_age_ == 0) {
        return true;
    }
    
    if (pImpl->expires_ != std::chrono::system_clock::time_point{}) {
        return std::chrono::system_clock::now() > pImpl->expires_;
    }
    
    return false;
}

bool Cookie::is_session_cookie() const {
    return pImpl->max_age_ == -1 &&
           pImpl->expires_ == std::chrono::system_clock::time_point{};
}

// ==========================================
// Security Flags
// ==========================================

bool Cookie::is_secure() const { return pImpl->secure_; }
void Cookie::set_secure(bool secure) { pImpl->secure_ = secure; }

bool Cookie::is_http_only() const { return pImpl->http_only_; }
void Cookie::set_http_only(bool http_only) { pImpl->http_only_ = http_only; }

Cookie::SameSite Cookie::same_site() const { return pImpl->same_site_; }
void Cookie::set_same_site(SameSite same_site) { pImpl->same_site_ = same_site; }

// ==========================================
// Serialization
// ==========================================

std::string Cookie::to_set_cookie_header() const {
    std::ostringstream header;
    
    header << pImpl->name_ << "=" << pImpl->value_;
    
    if (!pImpl->domain_.empty()) {
        header << "; Domain=" << pImpl->domain_;
    }
    
    if (!pImpl->path_.empty()) {
        header << "; Path=" << pImpl->path_;
    }
    
    if (pImpl->max_age_ >= 0) {
        header << "; Max-Age=" << pImpl->max_age_;
    }
    
    if (pImpl->expires_ != std::chrono::system_clock::time_point{}) {
        std::time_t expires_time = std::chrono::system_clock::to_time_t(pImpl->expires_);
        char expires_buf[100];
        std::strftime(expires_buf, sizeof(expires_buf), "%a, %d %b %Y %H:%M:%S GMT",
                     std::gmtime(&expires_time));
        header << "; Expires=" << expires_buf;
    }
    
    if (pImpl->secure_) {
        header << "; Secure";
    }
    
    if (pImpl->http_only_) {
        header << "; HttpOnly";
    }
    
    switch (pImpl->same_site_) {
        case SameSite::None:
            header << "; SameSite=None";
            break;
        case SameSite::Lax:
            header << "; SameSite=Lax";
            break;
        case SameSite::Strict:
            header << "; SameSite=Strict";
            break;
    }
    
    return header.str();
}

bool Cookie::from_set_cookie_header(const std::string& header_value) {
    if (header_value.empty()) {
        return false;
    }
    
    std::istringstream stream(header_value);
    std::string token;
    bool first = true;
    
    while (std::getline(stream, token, ';')) {
        // Trim whitespace
        size_t start = token.find_first_not_of(" \t");
        size_t end = token.find_last_not_of(" \t");
        if (start == std::string::npos) continue;
        token = token.substr(start, end - start + 1);
        
        if (first) {
            // Parse name=value
            size_t eq = token.find('=');
            if (eq == std::string::npos) return false;
            pImpl->name_ = token.substr(0, eq);
            pImpl->value_ = token.substr(eq + 1);
            first = false;
        } else {
            // Parse attributes
            size_t eq = token.find('=');
            std::string attr_name;
            std::string attr_value;
            
            if (eq != std::string::npos) {
                attr_name = token.substr(0, eq);
                attr_value = token.substr(eq + 1);
            } else {
                attr_name = token;
            }
            
            // Convert to lowercase for comparison
            std::string attr_lower = attr_name;
            std::transform(attr_lower.begin(), attr_lower.end(),
                          attr_lower.begin(), ::tolower);
            
            if (attr_lower == "domain") {
                pImpl->domain_ = attr_value;
            } else if (attr_lower == "path") {
                pImpl->path_ = attr_value;
            } else if (attr_lower == "max-age") {
                try {
                    pImpl->max_age_ = std::stoi(attr_value);
                } catch (...) {}
            } else if (attr_lower == "secure") {
                pImpl->secure_ = true;
            } else if (attr_lower == "httponly") {
                pImpl->http_only_ = true;
            } else if (attr_lower == "samesite") {
                std::string value_lower = attr_value;
                std::transform(value_lower.begin(), value_lower.end(),
                              value_lower.begin(), ::tolower);
                if (value_lower == "none") {
                    pImpl->same_site_ = SameSite::None;
                } else if (value_lower == "lax") {
                    pImpl->same_site_ = SameSite::Lax;
                } else if (value_lower == "strict") {
                    pImpl->same_site_ = SameSite::Strict;
                }
            }
        }
    }
    
    return !pImpl->name_.empty();
}

std::string Cookie::to_cookie_header() const {
    return pImpl->name_ + "=" + pImpl->value_;
}

bool Cookie::is_valid() const {
    return !pImpl->name_.empty();
}

// ==========================================
// CookieJar Implementation Class
// ==========================================

class CookieJar::Impl {
public:
    std::map<std::string, Cookie> cookies_;
};

// ==========================================
// CookieJar Implementation
// ==========================================

CookieJar::CookieJar() : pImpl(std::make_unique<Impl>()) {}

CookieJar::~CookieJar() = default;

void CookieJar::add_cookie(const Cookie& cookie) {
    if (cookie.is_valid()) {
        pImpl->cookies_[cookie.name()] = cookie;
    }
}

std::vector<Cookie> CookieJar::get_cookies(const std::string& url) const {
    std::vector<Cookie> result;
    
    for (const auto& [name, cookie] : pImpl->cookies_) {
        if (!cookie.is_expired()) {
            // TODO: Match domain and path against URL
            // For now, return all valid cookies
            result.push_back(cookie);
        }
    }
    
    return result;
}

Cookie CookieJar::get_cookie(const std::string& name) const {
    auto it = pImpl->cookies_.find(name);
    return (it != pImpl->cookies_.end()) ? it->second : Cookie();
}

bool CookieJar::has_cookie(const std::string& name) const {
    return pImpl->cookies_.find(name) != pImpl->cookies_.end();
}

void CookieJar::remove_cookie(const std::string& name) {
    pImpl->cookies_.erase(name);
}

void CookieJar::clear() {
    pImpl->cookies_.clear();
}

void CookieJar::remove_expired() {
    auto it = pImpl->cookies_.begin();
    while (it != pImpl->cookies_.end()) {
        if (it->second.is_expired()) {
            it = pImpl->cookies_.erase(it);
        } else {
            ++it;
        }
    }
}

std::vector<Cookie> CookieJar::all_cookies() const {
    std::vector<Cookie> result;
    for (const auto& [name, cookie] : pImpl->cookies_) {
        result.push_back(cookie);
    }
    return result;
}

size_t CookieJar::size() const {
    return pImpl->cookies_.size();
}

} // namespace http
} // namespace network
} // namespace sad
