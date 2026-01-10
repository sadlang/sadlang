// بسم الله الرحمن الرحيم
// HTTP Server Implementation

#include "http_server.h"
#include "tcp_socket.h"
#include <thread>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <filesystem>

namespace sad {
namespace network {
namespace http {

namespace fs = std::filesystem;

// ==========================================
// Route Structure
// ==========================================

struct Route {
    HttpMethod method;
    std::string path;
    HttpHandler handler;
    
    bool matches(HttpMethod m, const std::string& p) const {
        return method == m && path == p;
    }
};

// ==========================================
// Implementation Class
// ==========================================

class HttpServer::Impl {
public:
    std::string address_ = "0.0.0.0";
    int port_ = 8080;
    bool running_ = false;
    int thread_pool_size_ = 4;
    size_t max_request_size_ = 10 * 1024 * 1024; // 10MB
    int timeout_ms_ = 30000; // 30 seconds
    bool cors_enabled_ = false;
    std::string cors_origin_ = "*";
    bool compression_enabled_ = false;
    
    std::vector<Route> routes_;
    std::vector<Middleware> global_middlewares_;
    HttpHandler not_found_handler_;
    std::function<void(const std::exception&, HttpResponse&)> error_handler_;
    
    std::unique_ptr<TcpSocket> server_socket_;
    std::vector<std::thread> worker_threads_;
    
    void handle_client(TcpSocket client_socket);
    HttpResponse process_request(const HttpRequest& request);
    Route* find_route(HttpMethod method, const std::string& path);
    std::string read_file(const std::string& path);
    std::string get_mime_type(const std::string& path);
};

// ==========================================
// Constructors
// ==========================================

HttpServer::HttpServer() : pImpl(std::make_unique<Impl>()) {
    // Set default 404 handler
    set_not_found_handler([](const HttpRequest& req, HttpResponse& res) {
        res.set_status(HttpStatus::NotFound);
        res.set_text("404 Not Found: " + req.path());
    });
}

HttpServer::HttpServer(int port) : HttpServer() {
    pImpl->port_ = port;
}

HttpServer::HttpServer(const std::string& address, int port) : HttpServer(port) {
    pImpl->address_ = address;
}

HttpServer::~HttpServer() {
    stop();
}

HttpServer::HttpServer(HttpServer&&) noexcept = default;
HttpServer& HttpServer::operator=(HttpServer&&) noexcept = default;

// ==========================================
// Route Registration
// ==========================================

void HttpServer::get(const std::string& path, HttpHandler handler) {
    route(HttpMethod::GET, path, handler);
}

void HttpServer::post(const std::string& path, HttpHandler handler) {
    route(HttpMethod::POST, path, handler);
}

void HttpServer::put(const std::string& path, HttpHandler handler) {
    route(HttpMethod::PUT, path, handler);
}

void HttpServer::delete_(const std::string& path, HttpHandler handler) {
    route(HttpMethod::DELETE, path, handler);
}

void HttpServer::patch(const std::string& path, HttpHandler handler) {
    route(HttpMethod::PATCH, path, handler);
}

void HttpServer::any(const std::string& path, HttpHandler handler) {
    // Register for all methods
    for (int m = static_cast<int>(HttpMethod::GET); 
         m <= static_cast<int>(HttpMethod::TRACE); ++m) {
        route(static_cast<HttpMethod>(m), path, handler);
    }
}

void HttpServer::route(HttpMethod method, const std::string& path, HttpHandler handler) {
    Route r;
    r.method = method;
    r.path = path;
    r.handler = handler;
    pImpl->routes_.push_back(r);
}

// ==========================================
// Middleware
// ==========================================

void HttpServer::use(Middleware middleware) {
    pImpl->global_middlewares_.push_back(middleware);
}

void HttpServer::use(const std::string& path, Middleware middleware) {
    // Wrap middleware to check path
    pImpl->global_middlewares_.push_back(
        [path, middleware](const HttpRequest& req, HttpResponse& res, std::function<void()> next) {
            if (req.path().find(path) == 0) {
                middleware(req, res, next);
            } else {
                next();
            }
        }
    );
}

// ==========================================
// Static Files
// ==========================================

void HttpServer::static_files(const std::string& route_path, const std::string& directory) {
    get(route_path + "/*", [directory, route_path](const HttpRequest& req, HttpResponse& res) {
        std::string requested_path = req.path();
        
        // Remove route_path prefix
        if (requested_path.find(route_path) == 0) {
            requested_path = requested_path.substr(route_path.length());
        }
        
        // Build file path
        fs::path file_path = fs::path(directory) / requested_path;
        
        if (!fs::exists(file_path) || !fs::is_regular_file(file_path)) {
            res.set_status(HttpStatus::NotFound);
            res.set_text("File not found");
            return;
        }
        
        // Read file
        std::ifstream file(file_path, std::ios::binary);
        if (!file) {
            res.set_status(HttpStatus::InternalServerError);
            res.set_text("Failed to read file");
            return;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Set content type
        std::string ext = file_path.extension().string();
        if (ext == ".html") res.set_header(headers::ContentType, mime_types::TextHtml);
        else if (ext == ".css") res.set_header(headers::ContentType, mime_types::TextCss);
        else if (ext == ".js") res.set_header(headers::ContentType, mime_types::TextJavascript);
        else if (ext == ".json") res.set_header(headers::ContentType, mime_types::ApplicationJson);
        else if (ext == ".png") res.set_header(headers::ContentType, mime_types::ImagePng);
        else if (ext == ".jpg" || ext == ".jpeg") res.set_header(headers::ContentType, mime_types::ImageJpeg);
        else res.set_header(headers::ContentType, mime_types::ApplicationOctetStream);
        
        res.set_body(content);
    });
}

// ==========================================
// Server Control
// ==========================================

void HttpServer::listen() {
    listen(pImpl->address_, pImpl->port_);
}

void HttpServer::listen(int port) {
    listen(pImpl->address_, port);
}

void HttpServer::listen(const std::string& address, int port) {
    if (pImpl->running_) {
        return;
    }
    
    pImpl->address_ = address;
    pImpl->port_ = port;
    pImpl->running_ = true;
    
    // Create server socket
    pImpl->server_socket_ = std::make_unique<TcpSocket>();
    pImpl->server_socket_->bind(port);
    pImpl->server_socket_->listen(100);
    
    // Main accept loop
    while (pImpl->running_) {
        try {
            TcpSocket client = pImpl->server_socket_->accept();
            
            // Handle client in thread
            std::thread client_thread(&HttpServer::Impl::handle_client, pImpl.get(), std::move(client));
            client_thread.detach();
            
        } catch (const std::exception& ex) {
            if (pImpl->running_) {
                // Log error but continue
            }
        }
    }
}

void HttpServer::stop() {
    pImpl->running_ = false;
    if (pImpl->server_socket_) {
        pImpl->server_socket_->close();
    }
}

bool HttpServer::is_running() const {
    return pImpl->running_;
}

// ==========================================
// Implementation Methods
// ==========================================

void HttpServer::Impl::handle_client(TcpSocket client_socket) {
    try {
        client_socket.set_receive_timeout(timeout_ms_);
        
        // Receive request
        std::string request_data;
        std::string chunk;
        
        // Read headers
        bool headers_complete = false;
        while (!headers_complete && request_data.size() < max_request_size_) {
            chunk = client_socket.receive(4096);
            if (chunk.empty()) break;
            
            request_data += chunk;
            
            if (request_data.find("\r\n\r\n") != std::string::npos) {
                headers_complete = true;
            }
        }
        
        // Parse request
        HttpRequest request;
        if (!request.from_string(request_data)) {
            HttpResponse error_response(HttpStatus::BadRequest);
            error_response.set_text("Bad Request");
            client_socket.send_all(error_response.to_string());
            client_socket.close();
            return;
        }
        
        // Process request
        HttpResponse response = process_request(request);
        
        // Apply CORS if enabled
        if (cors_enabled_) {
            response.set_cors(cors_origin_);
        }
        
        // Send response
        client_socket.send_all(response.to_string());
        client_socket.close();
        
    } catch (const std::exception& ex) {
        try {
            HttpResponse error_response(HttpStatus::InternalServerError);
            if (error_handler_) {
                error_handler_(ex, error_response);
            } else {
                error_response.set_text("Internal Server Error");
            }
            client_socket.send_all(error_response.to_string());
        } catch (...) {}
        
        client_socket.close();
    }
}

HttpResponse HttpServer::Impl::process_request(const HttpRequest& request) {
    HttpResponse response;
    
    // Find matching route
    Route* route = find_route(request.method(), request.path());
    
    if (route) {
        // Execute global middlewares first
        size_t middleware_index = 0;
        std::function<void()> next;
        
        next = [&]() {
            if (middleware_index < global_middlewares_.size()) {
                auto& middleware = global_middlewares_[middleware_index++];
                middleware(request, response, next);
            } else {
                // Execute handler
                route->handler(request, response);
            }
        };
        
        next();
    } else {
        // 404 Not Found
        if (not_found_handler_) {
            not_found_handler_(request, response);
        } else {
            response.set_status(HttpStatus::NotFound);
            response.set_text("Not Found");
        }
    }
    
    return response;
}

Route* HttpServer::Impl::find_route(HttpMethod method, const std::string& path) {
    for (auto& route : routes_) {
        // Exact match
        if (route.matches(method, path)) {
            return &route;
        }
        
        // Wildcard match (/* at end)
        if (route.path.ends_with("/*")) {
            std::string prefix = route.path.substr(0, route.path.length() - 2);
            if (path.find(prefix) == 0 && route.method == method) {
                return &route;
            }
        }
    }
    return nullptr;
}

// ==========================================
// Configuration
// ==========================================

void HttpServer::set_thread_pool_size(int size) {
    pImpl->thread_pool_size_ = size;
}

void HttpServer::set_max_request_size(size_t size) {
    pImpl->max_request_size_ = size;
}

void HttpServer::set_timeout(int timeout_ms) {
    pImpl->timeout_ms_ = timeout_ms;
}

void HttpServer::enable_cors(const std::string& origin) {
    pImpl->cors_enabled_ = true;
    pImpl->cors_origin_ = origin;
}

void HttpServer::enable_compression(bool enable) {
    pImpl->compression_enabled_ = enable;
}

// ==========================================
// Error Handling
// ==========================================

void HttpServer::set_not_found_handler(HttpHandler handler) {
    pImpl->not_found_handler_ = handler;
}

void HttpServer::set_error_handler(std::function<void(const std::exception&, HttpResponse&)> handler) {
    pImpl->error_handler_ = handler;
}

} // namespace http
} // namespace network
} // namespace sad
