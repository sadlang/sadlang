// بسم الله الرحمن الرحيم
// HTTP Server - خادم HTTP
// HTTP Server Implementation

#ifndef SAD_NETWORK_HTTP_SERVER_H
#define SAD_NETWORK_HTTP_SERVER_H

#include "http_request.h"
#include "http_response.h"
#include <string>
#include <memory>
#include <functional>

namespace sad {
namespace network {
namespace http {

// Handler function type
using HttpHandler = std::function<void(const HttpRequest&, HttpResponse&)>;

// Middleware function type
using Middleware = std::function<void(const HttpRequest&, HttpResponse&, std::function<void()>)>;

/**
 * @brief HTTP Server
 * 
 * خادم HTTP يدعم:
 * - Route registration (GET, POST, PUT, DELETE, etc.)
 * - Middleware chain
 * - Static file serving
 * - Multi-threading
 * - Request parsing
 * - Response building
 */
class HttpServer {
public:
    // ==========================================
    // Constructors
    // ==========================================
    
    /**
     * @brief Default constructor (port 8080)
     */
    HttpServer();
    
    /**
     * @brief Construct with port
     */
    explicit HttpServer(int port);
    
    /**
     * @brief Construct with address and port
     */
    HttpServer(const std::string& address, int port);
    
    /**
     * @brief Destructor
     */
    ~HttpServer();
    
    // Disable copy, enable move
    HttpServer(const HttpServer&) = delete;
    HttpServer& operator=(const HttpServer&) = delete;
    HttpServer(HttpServer&&) noexcept;
    HttpServer& operator=(HttpServer&&) noexcept;
    
    // ==========================================
    // Route Registration
    // ==========================================
    
    /**
     * @brief Register GET route
     */
    void get(const std::string& path, HttpHandler handler);
    
    /**
     * @brief Register POST route
     */
    void post(const std::string& path, HttpHandler handler);
    
    /**
     * @brief Register PUT route
     */
    void put(const std::string& path, HttpHandler handler);
    
    /**
     * @brief Register DELETE route
     */
    void delete_(const std::string& path, HttpHandler handler);
    
    /**
     * @brief Register PATCH route
     */
    void patch(const std::string& path, HttpHandler handler);
    
    /**
     * @brief Register route for any method
     */
    void any(const std::string& path, HttpHandler handler);
    
    /**
     * @brief Register route with specific method
     */
    void route(HttpMethod method, const std::string& path, HttpHandler handler);
    
    // ==========================================
    // Middleware
    // ==========================================
    
    /**
     * @brief Add global middleware
     */
    void use(Middleware middleware);
    
    /**
     * @brief Add middleware for specific path
     */
    void use(const std::string& path, Middleware middleware);
    
    // ==========================================
    // Static Files
    // ==========================================
    
    /**
     * @brief Serve static files from directory
     * @param route_path URL path prefix (e.g., "/static")
     * @param directory File system directory
     */
    void static_files(const std::string& route_path, const std::string& directory);
    
    // ==========================================
    // Server Control
    // ==========================================
    
    /**
     * @brief Start server and listen for connections
     */
    void listen();
    
    /**
     * @brief Start server on specific port
     */
    void listen(int port);
    
    /**
     * @brief Start server on specific address and port
     */
    void listen(const std::string& address, int port);
    
    /**
     * @brief Stop server
     */
    void stop();
    
    /**
     * @brief Check if server is running
     */
    bool is_running() const;
    
    // ==========================================
    // Configuration
    // ==========================================
    
    /**
     * @brief Set number of worker threads
     */
    void set_thread_pool_size(int size);
    
    /**
     * @brief Set maximum request size in bytes
     */
    void set_max_request_size(size_t size);
    
    /**
     * @brief Set request timeout in milliseconds
     */
    void set_timeout(int timeout_ms);
    
    /**
     * @brief Enable CORS with specific origin
     */
    void enable_cors(const std::string& origin = "*");
    
    /**
     * @brief Enable gzip compression
     */
    void enable_compression(bool enable = true);
    
    // ==========================================
    // Error Handling
    // ==========================================
    
    /**
     * @brief Set custom 404 handler
     */
    void set_not_found_handler(HttpHandler handler);
    
    /**
     * @brief Set custom error handler
     */
    void set_error_handler(std::function<void(const std::exception&, HttpResponse&)> handler);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace http
} // namespace network
} // namespace sad

#endif // SAD_NETWORK_HTTP_SERVER_H
