// بسم الله الرحمن الرحيم
// HTTP URL - معالج URL
// URL Parser and Handler

#ifndef SAD_NETWORK_HTTP_URL_H
#define SAD_NETWORK_HTTP_URL_H

#include "http_base.h"
#include <string>
#include <memory>

namespace sad {
namespace network {
namespace http {

/**
 * @brief URL parser and builder
 * 
 * معالج URL يقوم بـ:
 * - تحليل URL كامل
 * - استخراج المكونات (scheme, host, port, path, query, fragment)
 * - بناء URL من المكونات
 * - ترميز/فك ترميز URL
 */
class URL {
public:
    // ==========================================
    // Constructors
    // ==========================================
    
    /**
     * @brief Default constructor
     */
    URL();
    
    /**
     * @brief Construct from URL string
     * @param url_str Full URL string
     */
    explicit URL(const std::string& url_str);
    
    /**
     * @brief Copy constructor
     */
    URL(const URL& other);
    
    /**
     * @brief Move constructor
     */
    URL(URL&& other) noexcept;
    
    /**
     * @brief Destructor
     */
    ~URL();
    
    /**
     * @brief Copy assignment
     */
    URL& operator=(const URL& other);
    
    /**
     * @brief Move assignment
     */
    URL& operator=(URL&& other) noexcept;
    
    // ==========================================
    // Parsing
    // ==========================================
    
    /**
     * @brief Parse URL from string
     * @param url_str URL string (e.g., "https://example.com:8080/path?key=value#section")
     * @return true if parsing succeeded
     */
    bool parse(const std::string& url_str);
    
    /**
     * @brief Check if URL is valid
     */
    bool is_valid() const;
    
    // ==========================================
    // Components Access
    // ==========================================
    
    /**
     * @brief Get scheme (e.g., "http", "https")
     */
    const std::string& scheme() const;
    
    /**
     * @brief Set scheme
     */
    void set_scheme(const std::string& scheme);
    
    /**
     * @brief Get host (e.g., "example.com", "192.168.1.1")
     */
    const std::string& host() const;
    
    /**
     * @brief Set host
     */
    void set_host(const std::string& host);
    
    /**
     * @brief Get port
     * @return Port number, or 0 if not specified
     */
    uint16_t port() const;
    
    /**
     * @brief Set port
     */
    void set_port(uint16_t port);
    
    /**
     * @brief Get default port for scheme
     * http -> 80, https -> 443, ftp -> 21
     */
    uint16_t default_port() const;
    
    /**
     * @brief Get effective port (specified port or default)
     */
    uint16_t effective_port() const;
    
    /**
     * @brief Get path (e.g., "/api/users")
     */
    const std::string& path() const;
    
    /**
     * @brief Set path
     */
    void set_path(const std::string& path);
    
    /**
     * @brief Get query string (e.g., "key1=value1&key2=value2")
     */
    const std::string& query() const;
    
    /**
     * @brief Set query string
     */
    void set_query(const std::string& query);
    
    /**
     * @brief Get query parameters
     */
    QueryParams query_params() const;
    
    /**
     * @brief Set query parameters
     */
    void set_query_params(const QueryParams& params);
    
    /**
     * @brief Get fragment (anchor, e.g., "section1")
     */
    const std::string& fragment() const;
    
    /**
     * @brief Set fragment
     */
    void set_fragment(const std::string& fragment);
    
    /**
     * @brief Get username (if present in URL)
     */
    const std::string& username() const;
    
    /**
     * @brief Set username
     */
    void set_username(const std::string& username);
    
    /**
     * @brief Get password (if present in URL)
     */
    const std::string& password() const;
    
    /**
     * @brief Set password
     */
    void set_password(const std::string& password);
    
    // ==========================================
    // Building
    // ==========================================
    
    /**
     * @brief Build full URL from components
     * @return Full URL string
     */
    std::string to_string() const;
    
    /**
     * @brief Get URL without scheme and host
     * @return Path + query + fragment (e.g., "/path?key=value#section")
     */
    std::string path_and_query() const;
    
    /**
     * @brief Get authority (username:password@host:port)
     */
    std::string authority() const;
    
    // ==========================================
    // Utilities
    // ==========================================
    
    /**
     * @brief Check if URL is absolute (has scheme and host)
     */
    bool is_absolute() const;
    
    /**
     * @brief Check if URL is relative
     */
    bool is_relative() const;
    
    /**
     * @brief Check if scheme is secure (https, wss, etc.)
     */
    bool is_secure() const;
    
    /**
     * @brief Resolve relative URL against base URL
     * @param base Base URL
     * @param relative Relative URL
     * @return Resolved absolute URL
     */
    static URL resolve(const URL& base, const URL& relative);
    
    /**
     * @brief Normalize URL (lowercase scheme/host, remove default port, etc.)
     */
    void normalize();
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace http
} // namespace network
} // namespace sad

#endif // SAD_NETWORK_HTTP_URL_H
