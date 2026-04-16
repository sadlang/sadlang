// بسم الله الرحمن الرحيم
// HTTP Cookie - كوكيز HTTP
// HTTP Cookie Handling

#ifndef SAD_NETWORK_HTTP_COOKIE_H
#define SAD_NETWORK_HTTP_COOKIE_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <chrono>

namespace sad {
namespace network {
namespace http {

/**
 * @brief HTTP Cookie representation
 * 
 * تمثيل كوكي HTTP مع جميع السمات:
 * - Name & Value
 * - Domain & Path
 * - Expires & Max-Age
 * - Secure & HttpOnly
 * - SameSite
 */
class Cookie {
public:
    // ==========================================
    // SameSite Enum
    // ==========================================
    
    enum class SameSite {
        None,
        Lax,
        Strict
    };
    
    // ==========================================
    // Constructors
    // ==========================================
    
    /**
     * @brief Default constructor
     */
    Cookie();
    
    /**
     * @brief Construct with name and value
     */
    Cookie(const std::string& name, const std::string& value);
    
    /**
     * @brief Copy constructor
     */
    Cookie(const Cookie& other);
    
    /**
     * @brief Move constructor
     */
    Cookie(Cookie&& other) noexcept;
    
    /**
     * @brief Destructor
     */
    ~Cookie();
    
    /**
     * @brief Copy assignment
     */
    Cookie& operator=(const Cookie& other);
    
    /**
     * @brief Move assignment
     */
    Cookie& operator=(Cookie&& other) noexcept;
    
    // ==========================================
    // Basic Properties
    // ==========================================
    
    /**
     * @brief Get cookie name
     */
    const std::string& name() const;
    
    /**
     * @brief Set cookie name
     */
    void set_name(const std::string& name);
    
    /**
     * @brief Get cookie value
     */
    const std::string& value() const;
    
    /**
     * @brief Set cookie value
     */
    void set_value(const std::string& value);
    
    // ==========================================
    // Domain & Path
    // ==========================================
    
    /**
     * @brief Get domain
     */
    const std::string& domain() const;
    
    /**
     * @brief Set domain
     */
    void set_domain(const std::string& domain);
    
    /**
     * @brief Get path
     */
    const std::string& path() const;
    
    /**
     * @brief Set path
     */
    void set_path(const std::string& path);
    
    // ==========================================
    // Expiration
    // ==========================================
    
    /**
     * @brief Get max age in seconds
     * @return Max age, or -1 if not set (session cookie)
     */
    int max_age() const;
    
    /**
     * @brief Set max age in seconds
     * @param seconds Max age (0 = delete cookie, -1 = session cookie)
     */
    void set_max_age(int seconds);
    
    /**
     * @brief Get expires timestamp
     */
    std::chrono::system_clock::time_point expires() const;
    
    /**
     * @brief Set expires timestamp
     */
    void set_expires(const std::chrono::system_clock::time_point& time);
    
    /**
     * @brief Check if cookie is expired
     */
    bool is_expired() const;
    
    /**
     * @brief Check if cookie is session cookie (no max-age/expires)
     */
    bool is_session_cookie() const;
    
    // ==========================================
    // Security Flags
    // ==========================================
    
    /**
     * @brief Check if Secure flag is set
     */
    bool is_secure() const;
    
    /**
     * @brief Set Secure flag
     */
    void set_secure(bool secure);
    
    /**
     * @brief Check if HttpOnly flag is set
     */
    bool is_http_only() const;
    
    /**
     * @brief Set HttpOnly flag
     */
    void set_http_only(bool http_only);
    
    /**
     * @brief Get SameSite attribute
     */
    SameSite same_site() const;
    
    /**
     * @brief Set SameSite attribute
     */
    void set_same_site(SameSite same_site);
    
    // ==========================================
    // Serialization
    // ==========================================
    
    /**
     * @brief Convert to Set-Cookie header value
     * @return Set-Cookie header string
     */
    std::string to_set_cookie_header() const;
    
    /**
     * @brief Parse from Set-Cookie header
     * @param header_value Set-Cookie header value
     * @return true if parsing succeeded
     */
    bool from_set_cookie_header(const std::string& header_value);
    
    /**
     * @brief Convert to Cookie header format (name=value)
     */
    std::string to_cookie_header() const;
    
    /**
     * @brief Check if cookie is valid
     */
    bool is_valid() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Cookie Jar - stores and manages cookies
 * 
 * مخزن الكوكيز يدير:
 * - تخزين الكوكيز من الردود
 * - استرجاع الكوكيز للطلبات
 * - التحقق من صلاحية الكوكيز
 * - حذف الكوكيز المنتهية
 */
class CookieJar {
public:
    /**
     * @brief Default constructor
     */
    CookieJar();
    
    /**
     * @brief Destructor
     */
    ~CookieJar();
    
    /**
     * @brief Add cookie
     */
    void add_cookie(const Cookie& cookie);
    
    /**
     * @brief Get cookies for URL
     * @param url Request URL
     * @return Vector of applicable cookies
     */
    std::vector<Cookie> get_cookies(const std::string& url) const;
    
    /**
     * @brief Get cookie by name
     */
    Cookie get_cookie(const std::string& name) const;
    
    /**
     * @brief Check if cookie exists
     */
    bool has_cookie(const std::string& name) const;
    
    /**
     * @brief Remove cookie
     */
    void remove_cookie(const std::string& name);
    
    /**
     * @brief Clear all cookies
     */
    void clear();
    
    /**
     * @brief Remove expired cookies
     */
    void remove_expired();
    
    /**
     * @brief Get all cookies
     */
    std::vector<Cookie> all_cookies() const;
    
    /**
     * @brief Get cookie count
     */
    size_t size() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace http
} // namespace network
} // namespace sad

#endif // SAD_NETWORK_HTTP_COOKIE_H
