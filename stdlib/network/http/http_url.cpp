// بسم الله الرحمن الرحيم
// HTTP URL Implementation

#include "network/http/http_url.h"
#include "network/http/http_base.h"
#include <algorithm>
#include <sstream>

namespace sad {
namespace network {
namespace http {

// ==========================================
// Implementation Class
// ==========================================

class URL::Impl {
public:
    std::string scheme_;
    std::string username_;
    std::string password_;
    std::string host_;
    uint16_t port_ = 0;
    std::string path_;
    std::string query_;
    std::string fragment_;
    bool valid_ = false;
};

// ==========================================
// Constructors
// ==========================================

URL::URL() : pImpl(std::make_unique<Impl>()) {}

URL::URL(const std::string& url_str) : pImpl(std::make_unique<Impl>()) {
    parse(url_str);
}

URL::URL(const URL& other) : pImpl(std::make_unique<Impl>(*other.pImpl)) {}

URL::URL(URL&& other) noexcept = default;

URL::~URL() = default;

URL& URL::operator=(const URL& other) {
    if (this != &other) {
        *pImpl = *other.pImpl;
    }
    return *this;
}

URL& URL::operator=(URL&& other) noexcept = default;

// ==========================================
// Parsing
// ==========================================

bool URL::parse(const std::string& url_str) {
    if (url_str.empty()) {
        pImpl->valid_ = false;
        return false;
    }
    
    std::string remaining = url_str;
    
    // Parse scheme (http://, https://, etc.)
    size_t scheme_end = remaining.find("://");
    if (scheme_end != std::string::npos) {
        pImpl->scheme_ = remaining.substr(0, scheme_end);
        std::transform(pImpl->scheme_.begin(), pImpl->scheme_.end(),
                      pImpl->scheme_.begin(), ::tolower);
        remaining = remaining.substr(scheme_end + 3);
    }
    
    // Parse username:password@host:port/path?query#fragment
    // First, extract fragment
    size_t fragment_pos = remaining.find('#');
    if (fragment_pos != std::string::npos) {
        pImpl->fragment_ = remaining.substr(fragment_pos + 1);
        remaining = remaining.substr(0, fragment_pos);
    }
    
    // Extract query
    size_t query_pos = remaining.find('?');
    if (query_pos != std::string::npos) {
        pImpl->query_ = remaining.substr(query_pos + 1);
        remaining = remaining.substr(0, query_pos);
    }
    
    // Extract path
    size_t path_pos = remaining.find('/');
    std::string authority;
    if (path_pos != std::string::npos) {
        pImpl->path_ = remaining.substr(path_pos);
        authority = remaining.substr(0, path_pos);
    } else {
        authority = remaining;
        pImpl->path_ = "/";
    }
    
    // Parse authority: [username:password@]host[:port]
    if (!authority.empty()) {
        // Extract username:password
        size_t at_pos = authority.find('@');
        if (at_pos != std::string::npos) {
            std::string userinfo = authority.substr(0, at_pos);
            authority = authority.substr(at_pos + 1);
            
            size_t colon_pos = userinfo.find(':');
            if (colon_pos != std::string::npos) {
                pImpl->username_ = userinfo.substr(0, colon_pos);
                pImpl->password_ = userinfo.substr(colon_pos + 1);
            } else {
                pImpl->username_ = userinfo;
            }
        }
        
        // Parse host:port
        size_t port_pos = authority.rfind(':');
        if (port_pos != std::string::npos) {
            pImpl->host_ = authority.substr(0, port_pos);
            try {
                pImpl->port_ = static_cast<uint16_t>(std::stoi(authority.substr(port_pos + 1)));
            } catch (...) {
                pImpl->port_ = 0;
            }
        } else {
            pImpl->host_ = authority;
            pImpl->port_ = 0;
        }
        
        // Lowercase host
        std::transform(pImpl->host_.begin(), pImpl->host_.end(),
                      pImpl->host_.begin(), ::tolower);
    }
    
    pImpl->valid_ = !pImpl->host_.empty() || !pImpl->path_.empty();
    return pImpl->valid_;
}

bool URL::is_valid() const {
    return pImpl->valid_;
}

// ==========================================
// Components Access
// ==========================================

const std::string& URL::scheme() const { return pImpl->scheme_; }
void URL::set_scheme(const std::string& scheme) { pImpl->scheme_ = scheme; }

const std::string& URL::host() const { return pImpl->host_; }
void URL::set_host(const std::string& host) { pImpl->host_ = host; }

uint16_t URL::port() const { return pImpl->port_; }
void URL::set_port(uint16_t port) { pImpl->port_ = port; }

uint16_t URL::default_port() const {
    if (pImpl->scheme_ == "http") return 80;
    if (pImpl->scheme_ == "https") return 443;
    if (pImpl->scheme_ == "ftp") return 21;
    if (pImpl->scheme_ == "ws") return 80;
    if (pImpl->scheme_ == "wss") return 443;
    return 0;
}

uint16_t URL::effective_port() const {
    return pImpl->port_ > 0 ? pImpl->port_ : default_port();
}

const std::string& URL::path() const { return pImpl->path_; }
void URL::set_path(const std::string& path) { pImpl->path_ = path; }

const std::string& URL::query() const { return pImpl->query_; }
void URL::set_query(const std::string& query) { pImpl->query_ = query; }

QueryParams URL::query_params() const {
    return parse_query_string(pImpl->query_);
}

void URL::set_query_params(const QueryParams& params) {
    std::string query = build_query_string(params);
    if (!query.empty() && query[0] == '?') {
        query = query.substr(1);
    }
    pImpl->query_ = query;
}

const std::string& URL::fragment() const { return pImpl->fragment_; }
void URL::set_fragment(const std::string& fragment) { pImpl->fragment_ = fragment; }

const std::string& URL::username() const { return pImpl->username_; }
void URL::set_username(const std::string& username) { pImpl->username_ = username; }

const std::string& URL::password() const { return pImpl->password_; }
void URL::set_password(const std::string& password) { pImpl->password_ = password; }

// ==========================================
// Building
// ==========================================

std::string URL::to_string() const {
    std::ostringstream url;
    
    // Scheme
    if (!pImpl->scheme_.empty()) {
        url << pImpl->scheme_ << "://";
    }
    
    // Username:password
    if (!pImpl->username_.empty()) {
        url << pImpl->username_;
        if (!pImpl->password_.empty()) {
            url << ":" << pImpl->password_;
        }
        url << "@";
    }
    
    // Host
    url << pImpl->host_;
    
    // Port (only if not default)
    if (pImpl->port_ > 0 && pImpl->port_ != default_port()) {
        url << ":" << pImpl->port_;
    }
    
    // Path
    if (pImpl->path_.empty()) {
        url << "/";
    } else {
        url << pImpl->path_;
    }
    
    // Query
    if (!pImpl->query_.empty()) {
        url << "?" << pImpl->query_;
    }
    
    // Fragment
    if (!pImpl->fragment_.empty()) {
        url << "#" << pImpl->fragment_;
    }
    
    return url.str();
}

std::string URL::path_and_query() const {
    std::string result = pImpl->path_.empty() ? "/" : pImpl->path_;
    
    if (!pImpl->query_.empty()) {
        result += "?" + pImpl->query_;
    }
    
    if (!pImpl->fragment_.empty()) {
        result += "#" + pImpl->fragment_;
    }
    
    return result;
}

std::string URL::authority() const {
    std::ostringstream auth;
    
    if (!pImpl->username_.empty()) {
        auth << pImpl->username_;
        if (!pImpl->password_.empty()) {
            auth << ":" << pImpl->password_;
        }
        auth << "@";
    }
    
    auth << pImpl->host_;
    
    if (pImpl->port_ > 0 && pImpl->port_ != default_port()) {
        auth << ":" << pImpl->port_;
    }
    
    return auth.str();
}

// ==========================================
// Utilities
// ==========================================

bool URL::is_absolute() const {
    return !pImpl->scheme_.empty() && !pImpl->host_.empty();
}

bool URL::is_relative() const {
    return !is_absolute();
}

bool URL::is_secure() const {
    return pImpl->scheme_ == "https" || pImpl->scheme_ == "wss";
}

URL URL::resolve(const URL& base, const URL& relative) {
    if (relative.is_absolute()) {
        return relative;
    }
    
    URL result = base;
    
    if (!relative.path().empty()) {
        if (relative.path()[0] == '/') {
            // Absolute path
            result.set_path(relative.path());
        } else {
            // Relative path
            std::string base_path = base.path();
            size_t last_slash = base_path.rfind('/');
            if (last_slash != std::string::npos) {
                result.set_path(base_path.substr(0, last_slash + 1) + relative.path());
            } else {
                result.set_path("/" + relative.path());
            }
        }
    }
    
    if (!relative.query().empty()) {
        result.set_query(relative.query());
    }
    
    if (!relative.fragment().empty()) {
        result.set_fragment(relative.fragment());
    }
    
    return result;
}

void URL::normalize() {
    // Lowercase scheme and host
    std::transform(pImpl->scheme_.begin(), pImpl->scheme_.end(),
                  pImpl->scheme_.begin(), ::tolower);
    std::transform(pImpl->host_.begin(), pImpl->host_.end(),
                  pImpl->host_.begin(), ::tolower);
    
    // Remove default port
    if (pImpl->port_ == default_port()) {
        pImpl->port_ = 0;
    }
    
    // Ensure path starts with /
    if (!pImpl->path_.empty() && pImpl->path_[0] != '/') {
        pImpl->path_ = "/" + pImpl->path_;
    }
    
    // Remove empty path segments and resolve . and ..
    // This is a simplified implementation
    if (!pImpl->path_.empty()) {
        std::vector<std::string> segments;
        std::istringstream path_stream(pImpl->path_);
        std::string segment;
        
        while (std::getline(path_stream, segment, '/')) {
            if (segment == "..") {
                if (!segments.empty()) {
                    segments.pop_back();
                }
            } else if (!segment.empty() && segment != ".") {
                segments.push_back(segment);
            }
        }
        
        pImpl->path_ = "/";
        for (size_t i = 0; i < segments.size(); ++i) {
            if (i > 0) pImpl->path_ += "/";
            pImpl->path_ += segments[i];
        }
    }
}

} // namespace http
} // namespace network
} // namespace sad
