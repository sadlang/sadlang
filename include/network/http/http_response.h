// بسم الله الرحمن الرحيم
// HTTP Response - استجابة HTTP
// HTTP Response Representation

#ifndef SAD_NETWORK_HTTP_RESPONSE_H
#define SAD_NETWORK_HTTP_RESPONSE_H

#include "network/http/http_base.h"
#include <string>
#include <map>
#include <memory>

namespace sad {
namespace network {
namespace http {

/**
 * @brief HTTP Response representation
 * 
 * تمثيل استجابة HTTP يحتوي على:
 * - رمز الحالة (Status Code)
 * - الرؤوس (Headers)
 * - الجسم (Body)
 * - الكوكيز (Cookies)
 */
class HttpResponse {
public:
    // ==========================================
    // Constructors
    // ==========================================
    
    /**
     * @brief Default constructor (200 OK)
     */
    HttpResponse();
    
    /**
     * @brief Construct with status code
     */
    explicit HttpResponse(HttpStatus status);
    
    /**
     * @brief Construct with status code and body
     */
    HttpResponse(HttpStatus status, const std::string& body);
    
    /**
     * @brief Copy constructor
     */
    HttpResponse(const HttpResponse& other);
    
    /**
     * @brief Move constructor
     */
    HttpResponse(HttpResponse&& other) noexcept;
    
    /**
     * @brief Destructor
     */
    ~HttpResponse();
    
    /**
     * @brief Copy assignment
     */
    HttpResponse& operator=(const HttpResponse& other);
    
    /**
     * @brief Move assignment
     */
    HttpResponse& operator=(HttpResponse&& other) noexcept;
    
    // ==========================================
    // Status
    // ==========================================
    
    /**
     * @brief Get HTTP status code
     */
    HttpStatus status() const;
    
    /**
     * @brief Set HTTP status code
     */
    void set_status(HttpStatus status);
    
    /**
     * @brief Get status code as integer
     */
    int status_code() const;
    
    /**
     * @brief Get status text (e.g., "OK", "Not Found")
     */
    std::string status_text() const;
    
    /**
     * @brief Get status text in Arabic
     */
    std::string status_text_arabic() const;
    
    /**
     * @brief Check if response is success (2xx)
     */
    bool is_success() const;
    
    /**
     * @brief Check if response is redirect (3xx)
     */
    bool is_redirect() const;
    
    /**
     * @brief Check if response is client error (4xx)
     */
    bool is_client_error() const;
    
    /**
     * @brief Check if response is server error (5xx)
     */
    bool is_server_error() const;
    
    // ==========================================
    // HTTP Version
    // ==========================================
    
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
    // Body
    // ==========================================
    
    /**
     * @brief Get response body
     */
    const std::string& body() const;
    
    /**
     * @brief Set response body
     */
    void set_body(const std::string& body);
    
    /**
     * @brief Set JSON body
     * Automatically sets Content-Type to application/json
     */
    void set_json(const std::string& json);
    
    /**
     * @brief Set HTML body
     * Automatically sets Content-Type to text/html
     */
    void set_html(const std::string& html);
    
    /**
     * @brief Set plain text body
     * Automatically sets Content-Type to text/plain
     */
    void set_text(const std::string& text);
    
    /**
     * @brief Clear body
     */
    void clear_body();
    
    // ==========================================
    // Cookies
    // ==========================================
    
    /**
     * @brief Set cookie
     * @param name Cookie name
     * @param value Cookie value
     * @param path Cookie path (default: "/")
     * @param max_age Max age in seconds (default: 0 = session cookie)
     */
    void set_cookie(const std::string& name, const std::string& value,
                   const std::string& path = "/", int max_age = 0);
    
    /**
     * @brief Set cookie with all options
     */
    void set_cookie_full(const std::string& name, const std::string& value,
                        const std::string& path, const std::string& domain,
                        int max_age, bool secure, bool http_only);
    
    /**
     * @brief Delete cookie
     */
    void delete_cookie(const std::string& name, const std::string& path = "/");
    
    // ==========================================
    // Convenience Methods
    // ==========================================
    
    /**
     * @brief Set redirect response
     * @param url URL to redirect to
     * @param permanent true for 301, false for 302
     */
    void redirect(const std::string& url, bool permanent = false);
    
    /**
     * @brief Set CORS headers
     * @param origin Allowed origin (e.g., "*" or "https://example.com")
     */
    void set_cors(const std::string& origin = "*");
    
    /**
     * @brief Enable compression (gzip)
     */
    void enable_compression(bool enable = true);
    
    // ==========================================
    // Serialization
    // ==========================================
    
    /**
     * @brief Convert response to HTTP message string
     * Format: HTTP/VERSION STATUS_CODE STATUS_TEXT\r\n
     *         Headers\r\n
     *         \r\n
     *         Body
     */
    std::string to_string() const;
    
    /**
     * @brief Parse HTTP response from string
     * @param response_str The HTTP response string
     * @return true if parsing succeeded
     */
    bool from_string(const std::string& response_str);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace http
} // namespace network
} // namespace sad

#endif // SAD_NETWORK_HTTP_RESPONSE_H
