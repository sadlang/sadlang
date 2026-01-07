// بسم الله الرحمن الرحيم
// HTTP Request - طلب HTTP
// HTTP Request Representation

#ifndef SAD_NETWORK_HTTP_REQUEST_H
#define SAD_NETWORK_HTTP_REQUEST_H

#include "network/http/http_base.h"
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace sad {
namespace network {
namespace http {

/**
 * @brief HTTP Request representation
 * 
 * تمثيل طلب HTTP يحتوي على:
 * - الطريقة (GET, POST, etc.)
 * - المسار/URL
 * - الرؤوس (Headers)
 * - معاملات الاستعلام (Query Parameters)
 * - الجسم (Body)
 * - الكوكيز (Cookies)
 */
class HttpRequest {
public:
    // ==========================================
    // Constructors
    // ==========================================
    
    /**
     * @brief Default constructor
     */
    HttpRequest();
    
    /**
     * @brief Construct with method and path
     */
    HttpRequest(HttpMethod method, const std::string& path);
    
    /**
     * @brief Copy constructor
     */
    HttpRequest(const HttpRequest& other);
    
    /**
     * @brief Move constructor
     */
    HttpRequest(HttpRequest&& other) noexcept;
    
    /**
     * @brief Destructor
     */
    ~HttpRequest();
    
    /**
     * @brief Copy assignment
     */
    HttpRequest& operator=(const HttpRequest& other);
    
    /**
     * @brief Move assignment
     */
    HttpRequest& operator=(HttpRequest&& other) noexcept;
    
    // ==========================================
    // Basic Accessors
    // ==========================================
    
    /**
     * @brief Get HTTP method
     */
    HttpMethod method() const;
    
    /**
     * @brief Set HTTP method
     */
    void set_method(HttpMethod method);
    
    /**
     * @brief Get request path
     */
    const std::string& path() const;
    
    /**
     * @brief Set request path
     */
    void set_path(const std::string& path);
    
    /**
     * @brief Get HTTP version
     */
    HttpVersion version() const;
    
    /**
     * @brief Set HTTP version
     */
    void set_version(HttpVersion version);
    
    // ==========================================
    // Headers
    // ==========================================
    
    /**
     * @brief Get all headers
     */
    const HeadersCI& headers() const;
    
    /**
     * @brief Get header value
     * @param key Header name (case-insensitive)
     * @return Header value or empty string if not found
     */
    std::string header(const std::string& key) const;
    
    /**
     * @brief Check if header exists
     */
    bool has_header(const std::string& key) const;
    
    /**
     * @brief Set header value
     */
    void set_header(const std::string& key, const std::string& value);
    
    /**
     * @brief Remove header
     */
    void remove_header(const std::string& key);
    
    /**
     * @brief Clear all headers
     */
    void clear_headers();
    
    // ==========================================
    // Query Parameters
    // ==========================================
    
    /**
     * @brief Get all query parameters
     */
    const QueryParams& query_params() const;
    
    /**
     * @brief Get query parameter value
     */
    std::string query_param(const std::string& key) const;
    
    /**
     * @brief Check if query parameter exists
     */
    bool has_query_param(const std::string& key) const;
    
    /**
     * @brief Set query parameter
     */
    void set_query_param(const std::string& key, const std::string& value);
    
    /**
     * @brief Remove query parameter
     */
    void remove_query_param(const std::string& key);
    
    /**
     * @brief Clear all query parameters
     */
    void clear_query_params();
    
    // ==========================================
    // Body
    // ==========================================
    
    /**
     * @brief Get request body
     */
    const std::string& body() const;
    
    /**
     * @brief Set request body
     */
    void set_body(const std::string& body);
    
    /**
     * @brief Set body from JSON
     */
    void set_json(const std::string& json);
    
    /**
     * @brief Set body from form data
     */
    void set_form_data(const std::map<std::string, std::string>& data);
    
    /**
     * @brief Clear body
     */
    void clear_body();
    
    // ==========================================
    // Cookies
    // ==========================================
    
    /**
     * @brief Get cookie value
     */
    std::string cookie(const std::string& name) const;
    
    /**
     * @brief Check if cookie exists
     */
    bool has_cookie(const std::string& name) const;
    
    /**
     * @brief Set cookie
     */
    void set_cookie(const std::string& name, const std::string& value);
    
    /**
     * @brief Get all cookies
     */
    const std::map<std::string, std::string>& cookies() const;
    
    /**
     * @brief Clear all cookies
     */
    void clear_cookies();
    
    // ==========================================
    // URL and Path
    // ==========================================
    
    /**
     * @brief Get full URL (if set)
     */
    const std::string& url() const;
    
    /**
     * @brief Set full URL
     * This will parse the URL and extract:
     * - Path
     * - Query parameters
     * - Fragment (if any)
     */
    void set_url(const std::string& url);
    
    /**
     * @brief Build full URL from components
     */
    std::string build_url() const;
    
    // ==========================================
    // Serialization
    // ==========================================
    
    /**
     * @brief Convert request to HTTP message string
     * Format: METHOD PATH HTTP/VERSION\r\n
     *         Headers\r\n
     *         \r\n
     *         Body
     */
    std::string to_string() const;
    
    /**
     * @brief Parse HTTP request from string
     * @param request_str The HTTP request string
     * @return true if parsing succeeded
     */
    bool from_string(const std::string& request_str);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace http
} // namespace network
} // namespace sad

#endif // SAD_NETWORK_HTTP_REQUEST_H
