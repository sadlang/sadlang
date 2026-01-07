// بسم الله الرحمن الرحيم
// HTTP Client - عميل HTTP
// HTTP Client Implementation

#ifndef SAD_NETWORK_HTTP_CLIENT_H
#define SAD_NETWORK_HTTP_CLIENT_H

#include "network/http/http_request.h"
#include "network/http/http_response.h"
#include "network/http/http_cookie.h"
#include <string>
#include <memory>
#include <functional>

namespace sad {
namespace network {
namespace http {

/**
 * @brief HTTP Client for making HTTP requests
 * 
 * عميل HTTP يدعم:
 * - جميع HTTP Methods (GET, POST, PUT, DELETE, etc.)
 * - إدارة Headers
 * - إدارة Cookies تلقائياً
 * - Timeout & Retry
 * - Redirect handling
 * - Connection pooling
 */
class HttpClient {
public:
    // ==========================================
    // Constructors
    // ==========================================
    
    /**
     * @brief Default constructor
     */
    HttpClient();
    
    /**
     * @brief Construct with base URL
     */
    explicit HttpClient(const std::string& base_url);
    
    /**
     * @brief Destructor
     */
    ~HttpClient();
    
    // Disable copy, enable move
    HttpClient(const HttpClient&) = delete;
    HttpClient& operator=(const HttpClient&) = delete;
    HttpClient(HttpClient&&) noexcept;
    HttpClient& operator=(HttpClient&&) noexcept;
    
    // ==========================================
    // HTTP Methods
    // ==========================================
    
    /**
     * @brief Send GET request
     * @param url URL to request
     * @return HTTP response
     */
    HttpResponse get(const std::string& url);
    
    /**
     * @brief Send POST request
     * @param url URL to request
     * @param body Request body
     * @return HTTP response
     */
    HttpResponse post(const std::string& url, const std::string& body);
    
    /**
     * @brief Send PUT request
     */
    HttpResponse put(const std::string& url, const std::string& body);
    
    /**
     * @brief Send DELETE request
     */
    HttpResponse delete_(const std::string& url);
    
    /**
     * @brief Send PATCH request
     */
    HttpResponse patch(const std::string& url, const std::string& body);
    
    /**
     * @brief Send HEAD request
     */
    HttpResponse head(const std::string& url);
    
    /**
     * @brief Send custom request
     * @param request Complete HTTP request
     * @return HTTP response
     */
    HttpResponse send(const HttpRequest& request);
    
    // ==========================================
    // Configuration
    // ==========================================
    
    /**
     * @brief Set base URL for all requests
     */
    void set_base_url(const std::string& base_url);
    
    /**
     * @brief Get base URL
     */
    const std::string& base_url() const;
    
    /**
     * @brief Set default header for all requests
     */
    void set_default_header(const std::string& key, const std::string& value);
    
    /**
     * @brief Remove default header
     */
    void remove_default_header(const std::string& key);
    
    /**
     * @brief Set User-Agent header
     */
    void set_user_agent(const std::string& user_agent);
    
    /**
     * @brief Set request timeout in milliseconds
     * @param timeout_ms Timeout in milliseconds (0 = no timeout)
     */
    void set_timeout(int timeout_ms);
    
    /**
     * @brief Get current timeout
     */
    int timeout() const;
    
    /**
     * @brief Set maximum number of redirects to follow
     * @param max_redirects Maximum redirects (0 = no redirects)
     */
    void set_max_redirects(int max_redirects);
    
    /**
     * @brief Enable/disable automatic redirect following
     */
    void set_follow_redirects(bool follow);
    
    // ==========================================
    // Cookie Management
    // ==========================================
    
    /**
     * @brief Enable/disable automatic cookie handling
     */
    void enable_cookies(bool enable);
    
    /**
     * @brief Add cookie to jar
     */
    void add_cookie(const Cookie& cookie);
    
    /**
     * @brief Get cookie jar
     */
    CookieJar& cookie_jar();
    
    /**
     * @brief Get cookie jar (const)
     */
    const CookieJar& cookie_jar() const;
    
    /**
     * @brief Clear all cookies
     */
    void clear_cookies();
    
    // ==========================================
    // Authentication
    // ==========================================
    
    /**
     * @brief Set Basic Authentication
     * @param username Username
     * @param password Password
     */
    void set_basic_auth(const std::string& username, const std::string& password);
    
    /**
     * @brief Set Bearer Token authentication
     * @param token Bearer token
     */
    void set_bearer_token(const std::string& token);
    
    /**
     * @brief Clear authentication
     */
    void clear_auth();
    
    // ==========================================
    // Connection Pooling
    // ==========================================
    
    /**
     * @brief Enable/disable connection pooling
     */
    void enable_connection_pool(bool enable);
    
    /**
     * @brief Set maximum connections in pool
     */
    void set_max_connections(int max_connections);
    
    /**
     * @brief Close all connections in pool
     */
    void close_all_connections();
    
    // ==========================================
    // Error Handling
    // ==========================================
    
    /**
     * @brief Get last error message
     */
    const std::string& last_error() const;
    
    /**
     * @brief Check if last request was successful
     */
    bool is_ok() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace http
} // namespace network
} // namespace sad

#endif // SAD_NETWORK_HTTP_CLIENT_H
