// بسم الله الرحمن الرحيم
// websocket_server.cpp - WebSocket Server Implementation
// تنفيذ خادم WebSocket

#include "../include/websocket/websocket_server.h"

#include <sstream>
#include <regex>
#include <random>
#include <chrono>
#include <cstring>
#include <algorithm>

// Base64 encoding
static const char base64_chars[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static std::string base64_encode(const unsigned char* data, size_t len) {
    std::string result;
    result.reserve((len + 2) / 3 * 4);
    
    for (size_t i = 0; i < len; i += 3) {
        unsigned int n = data[i] << 16;
        if (i + 1 < len) n |= data[i + 1] << 8;
        if (i + 2 < len) n |= data[i + 2];
        
        result += base64_chars[(n >> 18) & 0x3F];
        result += base64_chars[(n >> 12) & 0x3F];
        result += (i + 1 < len) ? base64_chars[(n >> 6) & 0x3F] : '=';
        result += (i + 2 < len) ? base64_chars[n & 0x3F] : '=';
    }
    
    return result;
}

// Simple SHA1 implementation for WebSocket key
static void sha1_transform(uint32_t state[5], const unsigned char buffer[64]) {
    uint32_t a, b, c, d, e, w[80];
    
    for (int i = 0; i < 16; i++) {
        w[i] = (buffer[i*4] << 24) | (buffer[i*4+1] << 16) | 
               (buffer[i*4+2] << 8) | buffer[i*4+3];
    }
    
    for (int i = 16; i < 80; i++) {
        uint32_t t = w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16];
        w[i] = (t << 1) | (t >> 31);
    }
    
    a = state[0]; b = state[1]; c = state[2]; d = state[3]; e = state[4];
    
    for (int i = 0; i < 80; i++) {
        uint32_t f, k;
        if (i < 20) { f = (b & c) | ((~b) & d); k = 0x5A827999; }
        else if (i < 40) { f = b ^ c ^ d; k = 0x6ED9EBA1; }
        else if (i < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8F1BBCDC; }
        else { f = b ^ c ^ d; k = 0xCA62C1D6; }
        
        uint32_t temp = ((a << 5) | (a >> 27)) + f + e + k + w[i];
        e = d; d = c; c = (b << 30) | (b >> 2); b = a; a = temp;
    }
    
    state[0] += a; state[1] += b; state[2] += c; state[3] += d; state[4] += e;
}

static std::string sha1_hash(const std::string& input) {
    uint32_t state[5] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};
    size_t len = input.length();
    
    std::vector<unsigned char> buffer(((len + 8) / 64 + 1) * 64, 0);
    std::memcpy(buffer.data(), input.c_str(), len);
    buffer[len] = 0x80;
    
    uint64_t bits = len * 8;
    size_t pad_len = buffer.size();
    buffer[pad_len - 8] = (bits >> 56) & 0xFF;
    buffer[pad_len - 7] = (bits >> 48) & 0xFF;
    buffer[pad_len - 6] = (bits >> 40) & 0xFF;
    buffer[pad_len - 5] = (bits >> 32) & 0xFF;
    buffer[pad_len - 4] = (bits >> 24) & 0xFF;
    buffer[pad_len - 3] = (bits >> 16) & 0xFF;
    buffer[pad_len - 2] = (bits >> 8) & 0xFF;
    buffer[pad_len - 1] = bits & 0xFF;
    
    for (size_t i = 0; i < buffer.size(); i += 64) {
        sha1_transform(state, &buffer[i]);
    }
    
    std::string result(20, 0);
    for (int i = 0; i < 5; i++) {
        result[i*4]   = (state[i] >> 24) & 0xFF;
        result[i*4+1] = (state[i] >> 16) & 0xFF;
        result[i*4+2] = (state[i] >> 8) & 0xFF;
        result[i*4+3] = state[i] & 0xFF;
    }
    
    return result;
}

namespace sad {
namespace network {
namespace websocket {

// ============================================================================
// WebSocketConnection Implementation
// ============================================================================

WebSocketConnection::WebSocketConnection(ConnectionId id, SOCKET socket,
                                          const std::string& addr, uint16_t port,
                                          WebSocketServer* server)
    : id_(id), socket_(socket), remote_address_(addr), 
      remote_port_(port), server_(server) {}

bool WebSocketConnection::send(const std::string& message) {
    if (!connected_) return false;
    
    auto frame = FrameBuilder::build_text(message, false);  // Server doesn't mask
    return send_frame(frame);
}

bool WebSocketConnection::send_binary(const std::vector<uint8_t>& data) {
    if (!connected_) return false;
    
    auto frame = FrameBuilder::build_binary(data, false);
    return send_frame(frame);
}

bool WebSocketConnection::ping(const std::vector<uint8_t>& data) {
    if (!connected_) return false;
    
    auto frame = FrameBuilder::build_ping(data, false);
    return send_frame(frame);
}

void WebSocketConnection::close(CloseCode code, const std::string& reason) {
    if (!connected_) return;
    
    auto frame = FrameBuilder::build_close(code, reason, false);
    send_frame(frame);
    
    connected_ = false;
#ifdef _WIN32
    closesocket(socket_);
#else
    ::close(socket_);
#endif
}

bool WebSocketConnection::send_frame(const std::vector<uint8_t>& frame) {
    std::lock_guard<std::mutex> lock(send_mutex_);
    int sent = ::send(socket_, reinterpret_cast<const char*>(frame.data()),
                      static_cast<int>(frame.size()), 0);
    return sent == static_cast<int>(frame.size());
}

void WebSocketConnection::join_room(const std::string& room) {
    {
        std::lock_guard<std::mutex> lock(rooms_mutex_);
        rooms_.insert(room);
    }
    server_->add_to_room(id_, room);
}

void WebSocketConnection::leave_room(const std::string& room) {
    {
        std::lock_guard<std::mutex> lock(rooms_mutex_);
        rooms_.erase(room);
    }
    server_->remove_from_room(id_, room);
}

bool WebSocketConnection::is_in_room(const std::string& room) const {
    std::lock_guard<std::mutex> lock(rooms_mutex_);
    return rooms_.find(room) != rooms_.end();
}

std::set<std::string> WebSocketConnection::get_rooms() const {
    std::lock_guard<std::mutex> lock(rooms_mutex_);
    return rooms_;
}

// ============================================================================
// WebSocketServer Implementation Structure
// ============================================================================

struct WebSocketServer::Impl {
    SOCKET server_socket = INVALID_SOCKET;
    std::atomic<bool> running{false};
    uint16_t port = 0;
    std::string host;
    
    // Connections
    std::map<WebSocketConnection::ConnectionId, ConnectionPtr> connections;
    mutable std::mutex connections_mutex;
    std::atomic<WebSocketConnection::ConnectionId> next_id{1};
    
    // Rooms
    std::map<std::string, std::set<WebSocketConnection::ConnectionId>> rooms;
    mutable std::mutex rooms_mutex;
    
    // Callbacks
    OnConnectionCallback on_connection_cb;
    OnDisconnectCallback on_disconnect_cb;
    OnServerMessageCallback on_message_cb;
    OnServerErrorCallback on_error_cb;
    
    // Configuration
    size_t max_connections = 1000;
    int ping_interval_ms = 30000;
    int connection_timeout_ms = 60000;
    
    // Threading
    std::thread accept_thread;
    std::vector<std::thread> client_threads;
};

// ============================================================================
// WebSocketServer Constructor / Destructor
// ============================================================================

WebSocketServer::WebSocketServer() : impl_(std::make_unique<Impl>()) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

WebSocketServer::~WebSocketServer() {
    stop();
}

// ============================================================================
// Server Control
// ============================================================================

bool WebSocketServer::start(uint16_t port, const std::string& host) {
    if (impl_->running) {
        return false;
    }
    
    impl_->port = port;
    impl_->host = host;
    
    // Create server socket
    impl_->server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (impl_->server_socket == INVALID_SOCKET) {
        return false;
    }
    
    // Allow address reuse
    int opt = 1;
    setsockopt(impl_->server_socket, SOL_SOCKET, SO_REUSEADDR, 
               reinterpret_cast<const char*>(&opt), sizeof(opt));
    
    // Bind
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (host == "0.0.0.0" || host.empty()) {
        server_addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);
    }
    
    if (bind(impl_->server_socket, reinterpret_cast<struct sockaddr*>(&server_addr),
             sizeof(server_addr)) == SOCKET_ERROR) {
        closesocket(impl_->server_socket);
        impl_->server_socket = INVALID_SOCKET;
        return false;
    }
    
    // Listen
    if (listen(impl_->server_socket, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(impl_->server_socket);
        impl_->server_socket = INVALID_SOCKET;
        return false;
    }
    
    impl_->running = true;
    impl_->accept_thread = std::thread(&WebSocketServer::accept_thread_func, this);
    
    return true;
}

void WebSocketServer::stop() {
    if (!impl_->running) {
        return;
    }
    
    impl_->running = false;
    
    // Close all connections
    close_all_connections();
    
    // Close server socket
    if (impl_->server_socket != INVALID_SOCKET) {
        closesocket(impl_->server_socket);
        impl_->server_socket = INVALID_SOCKET;
    }
    
    // Wait for accept thread
    if (impl_->accept_thread.joinable()) {
        impl_->accept_thread.join();
    }
    
    // Wait for client threads
    for (auto& thread : impl_->client_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    impl_->client_threads.clear();
}

bool WebSocketServer::is_running() const {
    return impl_->running;
}

uint16_t WebSocketServer::get_port() const {
    return impl_->port;
}

// ============================================================================
// Client Management
// ============================================================================

size_t WebSocketServer::get_connection_count() const {
    std::lock_guard<std::mutex> lock(impl_->connections_mutex);
    return impl_->connections.size();
}

std::vector<ConnectionPtr> WebSocketServer::get_connections() const {
    std::lock_guard<std::mutex> lock(impl_->connections_mutex);
    std::vector<ConnectionPtr> result;
    result.reserve(impl_->connections.size());
    for (const auto& pair : impl_->connections) {
        result.push_back(pair.second);
    }
    return result;
}

ConnectionPtr WebSocketServer::get_connection(WebSocketConnection::ConnectionId id) const {
    std::lock_guard<std::mutex> lock(impl_->connections_mutex);
    auto it = impl_->connections.find(id);
    return (it != impl_->connections.end()) ? it->second : nullptr;
}

void WebSocketServer::close_connection(WebSocketConnection::ConnectionId id,
                                        CloseCode code, const std::string& reason) {
    ConnectionPtr conn;
    {
        std::lock_guard<std::mutex> lock(impl_->connections_mutex);
        auto it = impl_->connections.find(id);
        if (it != impl_->connections.end()) {
            conn = it->second;
        }
    }
    
    if (conn) {
        conn->close(code, reason);
    }
}

void WebSocketServer::close_all_connections(CloseCode code, const std::string& reason) {
    std::vector<ConnectionPtr> conns;
    {
        std::lock_guard<std::mutex> lock(impl_->connections_mutex);
        for (const auto& pair : impl_->connections) {
            conns.push_back(pair.second);
        }
    }
    
    for (auto& conn : conns) {
        conn->close(code, reason);
    }
}

// ============================================================================
// Broadcasting
// ============================================================================

void WebSocketServer::broadcast(const std::string& message) {
    auto frame = FrameBuilder::build_text(message, false);
    
    std::lock_guard<std::mutex> lock(impl_->connections_mutex);
    for (const auto& pair : impl_->connections) {
        if (pair.second->is_connected()) {
            pair.second->send_frame(frame);
        }
    }
}

void WebSocketServer::broadcast_binary(const std::vector<uint8_t>& data) {
    auto frame = FrameBuilder::build_binary(data, false);
    
    std::lock_guard<std::mutex> lock(impl_->connections_mutex);
    for (const auto& pair : impl_->connections) {
        if (pair.second->is_connected()) {
            pair.second->send_frame(frame);
        }
    }
}

void WebSocketServer::broadcast_to_room(const std::string& room, const std::string& message) {
    auto frame = FrameBuilder::build_text(message, false);
    
    std::set<WebSocketConnection::ConnectionId> room_members;
    {
        std::lock_guard<std::mutex> lock(impl_->rooms_mutex);
        auto it = impl_->rooms.find(room);
        if (it != impl_->rooms.end()) {
            room_members = it->second;
        }
    }
    
    std::lock_guard<std::mutex> lock(impl_->connections_mutex);
    for (auto id : room_members) {
        auto it = impl_->connections.find(id);
        if (it != impl_->connections.end() && it->second->is_connected()) {
            it->second->send_frame(frame);
        }
    }
}

void WebSocketServer::broadcast_binary_to_room(const std::string& room, 
                                                const std::vector<uint8_t>& data) {
    auto frame = FrameBuilder::build_binary(data, false);
    
    std::set<WebSocketConnection::ConnectionId> room_members;
    {
        std::lock_guard<std::mutex> lock(impl_->rooms_mutex);
        auto it = impl_->rooms.find(room);
        if (it != impl_->rooms.end()) {
            room_members = it->second;
        }
    }
    
    std::lock_guard<std::mutex> lock(impl_->connections_mutex);
    for (auto id : room_members) {
        auto it = impl_->connections.find(id);
        if (it != impl_->connections.end() && it->second->is_connected()) {
            it->second->send_frame(frame);
        }
    }
}

void WebSocketServer::broadcast_except(const std::string& message, 
                                        WebSocketConnection::ConnectionId except_id) {
    auto frame = FrameBuilder::build_text(message, false);
    
    std::lock_guard<std::mutex> lock(impl_->connections_mutex);
    for (const auto& pair : impl_->connections) {
        if (pair.first != except_id && pair.second->is_connected()) {
            pair.second->send_frame(frame);
        }
    }
}

// ============================================================================
// Room Management
// ============================================================================

std::set<std::string> WebSocketServer::get_rooms() const {
    std::lock_guard<std::mutex> lock(impl_->rooms_mutex);
    std::set<std::string> result;
    for (const auto& pair : impl_->rooms) {
        result.insert(pair.first);
    }
    return result;
}

std::vector<ConnectionPtr> WebSocketServer::get_room_connections(const std::string& room) const {
    std::set<WebSocketConnection::ConnectionId> room_members;
    {
        std::lock_guard<std::mutex> lock(impl_->rooms_mutex);
        auto it = impl_->rooms.find(room);
        if (it != impl_->rooms.end()) {
            room_members = it->second;
        }
    }
    
    std::vector<ConnectionPtr> result;
    std::lock_guard<std::mutex> lock(impl_->connections_mutex);
    for (auto id : room_members) {
        auto it = impl_->connections.find(id);
        if (it != impl_->connections.end()) {
            result.push_back(it->second);
        }
    }
    return result;
}

size_t WebSocketServer::get_room_size(const std::string& room) const {
    std::lock_guard<std::mutex> lock(impl_->rooms_mutex);
    auto it = impl_->rooms.find(room);
    return (it != impl_->rooms.end()) ? it->second.size() : 0;
}

void WebSocketServer::add_to_room(WebSocketConnection::ConnectionId id, const std::string& room) {
    std::lock_guard<std::mutex> lock(impl_->rooms_mutex);
    impl_->rooms[room].insert(id);
}

void WebSocketServer::remove_from_room(WebSocketConnection::ConnectionId id, const std::string& room) {
    std::lock_guard<std::mutex> lock(impl_->rooms_mutex);
    auto it = impl_->rooms.find(room);
    if (it != impl_->rooms.end()) {
        it->second.erase(id);
        if (it->second.empty()) {
            impl_->rooms.erase(it);
        }
    }
}

void WebSocketServer::remove_from_all_rooms(WebSocketConnection::ConnectionId id) {
    std::lock_guard<std::mutex> lock(impl_->rooms_mutex);
    for (auto it = impl_->rooms.begin(); it != impl_->rooms.end();) {
        it->second.erase(id);
        if (it->second.empty()) {
            it = impl_->rooms.erase(it);
        } else {
            ++it;
        }
    }
}

// ============================================================================
// Callbacks
// ============================================================================

void WebSocketServer::on_connection(OnConnectionCallback callback) {
    impl_->on_connection_cb = std::move(callback);
}

void WebSocketServer::on_disconnect(OnDisconnectCallback callback) {
    impl_->on_disconnect_cb = std::move(callback);
}

void WebSocketServer::on_message(OnServerMessageCallback callback) {
    impl_->on_message_cb = std::move(callback);
}

void WebSocketServer::on_error(OnServerErrorCallback callback) {
    impl_->on_error_cb = std::move(callback);
}

// ============================================================================
// Configuration
// ============================================================================

void WebSocketServer::set_max_connections(size_t max_connections) {
    impl_->max_connections = max_connections;
}

void WebSocketServer::set_ping_interval(int interval_ms) {
    impl_->ping_interval_ms = interval_ms;
}

void WebSocketServer::set_connection_timeout(int timeout_ms) {
    impl_->connection_timeout_ms = timeout_ms;
}

// ============================================================================
// Thread Functions
// ============================================================================

void WebSocketServer::accept_thread_func() {
    while (impl_->running) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        
        SOCKET client_socket = accept(impl_->server_socket,
                                       reinterpret_cast<struct sockaddr*>(&client_addr),
                                       &addr_len);
        
        if (client_socket == INVALID_SOCKET) {
            if (impl_->running) {
                continue;  // Try again
            }
            break;
        }
        
        // Check max connections
        {
            std::lock_guard<std::mutex> lock(impl_->connections_mutex);
            if (impl_->connections.size() >= impl_->max_connections) {
                closesocket(client_socket);
                continue;
            }
        }
        
        // Get client info
        char addr_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, addr_str, INET_ADDRSTRLEN);
        uint16_t client_port = ntohs(client_addr.sin_port);
        
        // Perform handshake
        std::string error;
        if (!perform_handshake(client_socket, error)) {
            closesocket(client_socket);
            continue;
        }
        
        // Create connection
        auto id = impl_->next_id++;
        auto conn = std::make_shared<WebSocketConnection>(id, client_socket, 
                                                          addr_str, client_port, this);
        
        {
            std::lock_guard<std::mutex> lock(impl_->connections_mutex);
            impl_->connections[id] = conn;
        }
        
        // Notify connection callback
        if (impl_->on_connection_cb) {
            impl_->on_connection_cb(conn);
        }
        
        // Start client handler thread
        impl_->client_threads.emplace_back(&WebSocketServer::handle_client, this, conn);
    }
}

bool WebSocketServer::perform_handshake(SOCKET client_socket, std::string& error) {
    char buffer[4096];
    int received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (received <= 0) {
        error = "Failed to receive handshake request";
        return false;
    }
    buffer[received] = '\0';
    std::string request(buffer);
    
    // Parse request
    if (request.find("Upgrade: websocket") == std::string::npos) {
        error = "Not a WebSocket upgrade request";
        return false;
    }
    
    // Extract Sec-WebSocket-Key
    std::regex key_regex(R"(Sec-WebSocket-Key:\s*([^\r\n]+))");
    std::smatch matches;
    if (!std::regex_search(request, matches, key_regex)) {
        error = "Missing Sec-WebSocket-Key";
        return false;
    }
    std::string ws_key = matches[1].str();
    
    // Generate accept key
    std::string magic = WEBSOCKET_GUID;
    std::string accept_key = ws_key + magic;
    std::string sha1_result = sha1_hash(accept_key);
    std::string accept = base64_encode(reinterpret_cast<const unsigned char*>(sha1_result.c_str()), 20);
    
    // Send response
    std::ostringstream response;
    response << "HTTP/1.1 101 Switching Protocols\r\n";
    response << "Upgrade: websocket\r\n";
    response << "Connection: Upgrade\r\n";
    response << "Sec-WebSocket-Accept: " << accept << "\r\n";
    response << "\r\n";
    
    std::string response_str = response.str();
    if (send(client_socket, response_str.c_str(), static_cast<int>(response_str.length()), 0) == SOCKET_ERROR) {
        error = "Failed to send handshake response";
        return false;
    }
    
    return true;
}

void WebSocketServer::handle_client(ConnectionPtr conn) {
    std::vector<uint8_t> buffer(8192);
    std::vector<uint8_t> read_buffer;
    
    while (impl_->running && conn->is_connected()) {
        int received = recv(conn->socket_, reinterpret_cast<char*>(buffer.data()),
                           static_cast<int>(buffer.size()), 0);
        
        if (received <= 0) {
            break;
        }
        
        // Append to read buffer
        read_buffer.insert(read_buffer.end(), buffer.begin(), buffer.begin() + received);
        
        // Try to parse frames
        while (!read_buffer.empty()) {
            auto parse_result = FrameParser::parse(read_buffer);
            
            if (parse_result.result == FrameParser::ParseResult::Incomplete) {
                break;
            }
            
            if (parse_result.result == FrameParser::ParseResult::Error) {
                if (impl_->on_error_cb) {
                    impl_->on_error_cb(conn, parse_result.error_message);
                }
                conn->close(CloseCode::ProtocolError, parse_result.error_message);
                goto disconnect;
            }
            
            // Process frame
            const auto& frame = parse_result.frame;
            
            switch (frame.opcode) {
                case WebSocketOpcode::Text: {
                    if (!frame.fin) {
                        conn->fragment_buffer_ = frame.payload;
                        conn->fragment_type_ = MessageType::Text;
                        break;
                    }
                    
                    if (impl_->on_message_cb) {
                        WebSocketMessage msg;
                        msg.type = MessageType::Text;
                        msg.data = frame.payload;
                        impl_->on_message_cb(conn, msg);
                    }
                    break;
                }
                
                case WebSocketOpcode::Binary: {
                    if (!frame.fin) {
                        conn->fragment_buffer_ = frame.payload;
                        conn->fragment_type_ = MessageType::Binary;
                        break;
                    }
                    
                    if (impl_->on_message_cb) {
                        WebSocketMessage msg;
                        msg.type = MessageType::Binary;
                        msg.data = frame.payload;
                        impl_->on_message_cb(conn, msg);
                    }
                    break;
                }
                
                case WebSocketOpcode::Continuation: {
                    conn->fragment_buffer_.insert(conn->fragment_buffer_.end(),
                                                   frame.payload.begin(), frame.payload.end());
                    
                    if (frame.fin && impl_->on_message_cb) {
                        WebSocketMessage msg;
                        msg.type = conn->fragment_type_;
                        msg.data = std::move(conn->fragment_buffer_);
                        conn->fragment_buffer_.clear();
                        impl_->on_message_cb(conn, msg);
                    }
                    break;
                }
                
                case WebSocketOpcode::Close: {
                    auto close_info = FrameParser::parse_close_payload(frame.payload);
                    conn->close(close_info.code, close_info.reason);
                    goto disconnect;
                }
                
                case WebSocketOpcode::Ping: {
                    auto pong_frame = FrameBuilder::build_pong(frame.payload, false);
                    conn->send_frame(pong_frame);
                    break;
                }
                
                case WebSocketOpcode::Pong:
                    // No action needed
                    break;
                
                default:
                    break;
            }
            
            // Calculate and remove processed data
            size_t header_size = 2;
            if (frame.payload_length >= 126) header_size += 2;
            if (frame.payload_length >= 65536) header_size += 6;
            if (frame.masked) header_size += 4;
            size_t frame_size = header_size + frame.payload_length;
            
            read_buffer.erase(read_buffer.begin(), read_buffer.begin() + frame_size);
        }
    }
    
disconnect:
    // Remove from rooms
    remove_from_all_rooms(conn->get_id());
    
    // Remove from connections
    remove_connection(conn->get_id());
    
    // Notify disconnect callback
    if (impl_->on_disconnect_cb) {
        impl_->on_disconnect_cb(conn, CloseCode::Normal, "");
    }
}

void WebSocketServer::remove_connection(WebSocketConnection::ConnectionId id) {
    std::lock_guard<std::mutex> lock(impl_->connections_mutex);
    impl_->connections.erase(id);
}

} // namespace websocket
} // namespace network
} // namespace sad
