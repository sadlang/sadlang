// بسم الله الرحمن الرحيم
// websocket_client.cpp - WebSocket Client Implementation
// تنفيذ عميل WebSocket

#include "../include/websocket/websocket_client.h"

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
// Implementation Structure - هيكل التنفيذ
// ============================================================================

struct WebSocketClient::Impl {
    SOCKET socket = INVALID_SOCKET;
    std::atomic<ConnectionState> state{ConnectionState::Closed};
    
    // URL components
    std::string url;
    std::string host;
    uint16_t port = 80;
    std::string path = "/";
    bool secure = false;
    
    // Callbacks
    OnOpenCallback on_open_cb;
    OnCloseCallback on_close_cb;
    OnMessageCallback on_message_cb;
    OnErrorCallback on_error_cb;
    OnPingCallback on_ping_cb;
    OnPongCallback on_pong_cb;
    
    // Configuration
    int receive_timeout_ms = 30000;
    int send_timeout_ms = 30000;
    bool auto_reconnect = false;
    int max_reconnect_attempts = 5;
    int reconnect_delay_ms = 1000;
    int ping_interval_ms = 0;
    
    // Threading
    std::thread receiver_thread;
    std::thread heartbeat_thread;
    std::atomic<bool> running{false};
    
    // Message queue
    std::queue<WebSocketMessage> message_queue;
    mutable std::mutex queue_mutex;
    std::condition_variable queue_cv;
    
    // Fragmented message handling
    std::vector<uint8_t> fragment_buffer;
    MessageType fragment_type = MessageType::Text;
    
    // Buffer for reading
    std::vector<uint8_t> read_buffer;
    
    // Error
    std::string last_error;
    std::mutex error_mutex;
    
    void set_error(const std::string& error) {
        std::lock_guard<std::mutex> lock(error_mutex);
        last_error = error;
        if (on_error_cb) {
            on_error_cb(error);
        }
    }
};

// ============================================================================
// Constructor / Destructor
// ============================================================================

WebSocketClient::WebSocketClient() : impl_(std::make_unique<Impl>()) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

WebSocketClient::~WebSocketClient() {
    close();
#ifdef _WIN32
    // WSACleanup(); // Don't cleanup here - might affect other sockets
#endif
}

WebSocketClient::WebSocketClient(WebSocketClient&&) noexcept = default;
WebSocketClient& WebSocketClient::operator=(WebSocketClient&&) noexcept = default;

// ============================================================================
// Connection Methods
// ============================================================================

bool WebSocketClient::connect(const std::string& url) {
    return connect(url, {});
}

bool WebSocketClient::connect(const std::string& url, 
                              const std::vector<std::pair<std::string, std::string>>& headers) {
    if (impl_->state != ConnectionState::Closed) {
        impl_->set_error("Already connected / متصل مسبقاً");
        return false;
    }
    
    impl_->state = ConnectionState::Connecting;
    impl_->url = url;
    
    // Parse URL
    std::regex url_regex(R"(^(wss?)://([^:/]+)(?::(\d+))?(/.*)?$)");
    std::smatch matches;
    
    if (!std::regex_match(url, matches, url_regex)) {
        impl_->set_error("Invalid URL format / صيغة URL غير صالحة");
        impl_->state = ConnectionState::Closed;
        return false;
    }
    
    std::string scheme = matches[1].str();
    impl_->secure = (scheme == "wss");
    impl_->host = matches[2].str();
    impl_->port = matches[3].matched ? std::stoi(matches[3].str()) : 
                  (impl_->secure ? WSS_DEFAULT_PORT : WS_DEFAULT_PORT);
    impl_->path = matches[4].matched ? matches[4].str() : "/";
    
    // Create socket
    impl_->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (impl_->socket == INVALID_SOCKET) {
        impl_->set_error("Failed to create socket / فشل إنشاء المقبس");
        impl_->state = ConnectionState::Closed;
        return false;
    }
    
    // Resolve host
    struct hostent* host_info = gethostbyname(impl_->host.c_str());
    if (!host_info) {
        closesocket(impl_->socket);
        impl_->socket = INVALID_SOCKET;
        impl_->set_error("Failed to resolve host / فشل حل اسم المضيف");
        impl_->state = ConnectionState::Closed;
        return false;
    }
    
    // Connect
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(impl_->port);
    std::memcpy(&server_addr.sin_addr, host_info->h_addr_list[0], host_info->h_length);
    
    if (::connect(impl_->socket, reinterpret_cast<struct sockaddr*>(&server_addr), 
                  sizeof(server_addr)) == SOCKET_ERROR) {
        closesocket(impl_->socket);
        impl_->socket = INVALID_SOCKET;
        impl_->set_error("Connection failed / فشل الاتصال");
        impl_->state = ConnectionState::Closed;
        return false;
    }
    
    // Perform WebSocket handshake
    if (!do_handshake()) {
        closesocket(impl_->socket);
        impl_->socket = INVALID_SOCKET;
        impl_->state = ConnectionState::Closed;
        return false;
    }
    
    impl_->state = ConnectionState::Open;
    impl_->running = true;
    
    // Start receiver thread
    impl_->receiver_thread = std::thread(&WebSocketClient::receiver_thread_func, this);
    
    // Start heartbeat thread if configured
    if (impl_->ping_interval_ms > 0) {
        impl_->heartbeat_thread = std::thread(&WebSocketClient::heartbeat_thread_func, this);
    }
    
    // Call open callback
    if (impl_->on_open_cb) {
        impl_->on_open_cb();
    }
    
    return true;
}

bool WebSocketClient::do_handshake() {
    // Generate random WebSocket key
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    
    unsigned char key_bytes[16];
    for (int i = 0; i < 16; ++i) {
        key_bytes[i] = static_cast<unsigned char>(dis(gen));
    }
    std::string ws_key = base64_encode(key_bytes, 16);
    
    // Build upgrade request
    std::ostringstream request;
    request << "GET " << impl_->path << " HTTP/1.1\r\n";
    request << "Host: " << impl_->host;
    if ((impl_->secure && impl_->port != 443) || (!impl_->secure && impl_->port != 80)) {
        request << ":" << impl_->port;
    }
    request << "\r\n";
    request << "Upgrade: websocket\r\n";
    request << "Connection: Upgrade\r\n";
    request << "Sec-WebSocket-Key: " << ws_key << "\r\n";
    request << "Sec-WebSocket-Version: " << WEBSOCKET_VERSION << "\r\n";
    request << "\r\n";
    
    std::string request_str = request.str();
    if (::send(impl_->socket, request_str.c_str(), static_cast<int>(request_str.length()), 0) == SOCKET_ERROR) {
        impl_->set_error("Failed to send handshake / فشل إرسال المصافحة");
        return false;
    }
    
    // Read response
    char buffer[4096];
    int received = recv(impl_->socket, buffer, sizeof(buffer) - 1, 0);
    if (received <= 0) {
        impl_->set_error("Failed to receive handshake response / فشل استقبال رد المصافحة");
        return false;
    }
    buffer[received] = '\0';
    std::string response(buffer);
    
    // Verify response
    if (response.find("101") == std::string::npos) {
        impl_->set_error("Invalid handshake response status / حالة رد غير صالحة");
        return false;
    }
    
    // Verify Sec-WebSocket-Accept
    std::string magic = WEBSOCKET_GUID;
    std::string accept_key = ws_key + magic;
    std::string sha1_result = sha1_hash(accept_key);
    std::string expected_accept = base64_encode(reinterpret_cast<const unsigned char*>(sha1_result.c_str()), 20);
    
    if (response.find("Sec-WebSocket-Accept: " + expected_accept) == std::string::npos) {
        impl_->set_error("Invalid Sec-WebSocket-Accept / مفتاح قبول غير صالح");
        return false;
    }
    
    return true;
}

void WebSocketClient::close(CloseCode code, const std::string& reason) {
    if (impl_->state == ConnectionState::Closed) {
        return;
    }
    
    impl_->state = ConnectionState::Closing;
    impl_->running = false;
    
    // Send close frame
    if (impl_->socket != INVALID_SOCKET) {
        auto close_frame = FrameBuilder::build_close(code, reason);
        ::send(impl_->socket, reinterpret_cast<const char*>(close_frame.data()), 
             static_cast<int>(close_frame.size()), 0);
        closesocket(impl_->socket);
        impl_->socket = INVALID_SOCKET;
    }
    
    // Wait for threads
    if (impl_->receiver_thread.joinable()) {
        impl_->receiver_thread.join();
    }
    if (impl_->heartbeat_thread.joinable()) {
        impl_->heartbeat_thread.join();
    }
    
    impl_->state = ConnectionState::Closed;
    
    // Call close callback
    if (impl_->on_close_cb) {
        impl_->on_close_cb(code, reason);
    }
}

bool WebSocketClient::is_connected() const {
    return impl_->state == ConnectionState::Open;
}

ConnectionState WebSocketClient::get_state() const {
    return impl_->state;
}

// ============================================================================
// Sending Methods
// ============================================================================

bool WebSocketClient::send(const std::string& message) {
    if (impl_->state != ConnectionState::Open) {
        return false;
    }
    
    auto frame = FrameBuilder::build_text(message, true);
    return send_frame(frame);
}

bool WebSocketClient::send_binary(const std::vector<uint8_t>& data) {
    if (impl_->state != ConnectionState::Open) {
        return false;
    }
    
    auto frame = FrameBuilder::build_binary(data, true);
    return send_frame(frame);
}

bool WebSocketClient::ping(const std::vector<uint8_t>& data) {
    if (impl_->state != ConnectionState::Open) {
        return false;
    }
    
    auto frame = FrameBuilder::build_ping(data, true);
    return send_frame(frame);
}

bool WebSocketClient::send_frame(const std::vector<uint8_t>& frame) {
    int sent = ::send(impl_->socket, reinterpret_cast<const char*>(frame.data()), 
                      static_cast<int>(frame.size()), 0);
    return sent == static_cast<int>(frame.size());
}

// ============================================================================
// Receiving Methods
// ============================================================================

WebSocketMessage WebSocketClient::receive() {
    return receive(-1);
}

WebSocketMessage WebSocketClient::receive(int timeout_ms) {
    std::unique_lock<std::mutex> lock(impl_->queue_mutex);
    
    if (timeout_ms < 0) {
        impl_->queue_cv.wait(lock, [this] { 
            return !impl_->message_queue.empty() || !impl_->running; 
        });
    } else {
        impl_->queue_cv.wait_for(lock, std::chrono::milliseconds(timeout_ms), [this] {
            return !impl_->message_queue.empty() || !impl_->running;
        });
    }
    
    if (impl_->message_queue.empty()) {
        return WebSocketMessage{};
    }
    
    WebSocketMessage msg = std::move(impl_->message_queue.front());
    impl_->message_queue.pop();
    return msg;
}

bool WebSocketClient::has_message() const {
    std::lock_guard<std::mutex> lock(impl_->queue_mutex);
    return !impl_->message_queue.empty();
}

void WebSocketClient::poll() {
    // Just trigger processing - receiver thread handles actual reception
}

// ============================================================================
// Callback Methods
// ============================================================================

void WebSocketClient::on_open(OnOpenCallback callback) {
    impl_->on_open_cb = std::move(callback);
}

void WebSocketClient::on_close(OnCloseCallback callback) {
    impl_->on_close_cb = std::move(callback);
}

void WebSocketClient::on_message(OnMessageCallback callback) {
    impl_->on_message_cb = std::move(callback);
}

void WebSocketClient::on_error(OnErrorCallback callback) {
    impl_->on_error_cb = std::move(callback);
}

void WebSocketClient::on_ping(OnPingCallback callback) {
    impl_->on_ping_cb = std::move(callback);
}

void WebSocketClient::on_pong(OnPongCallback callback) {
    impl_->on_pong_cb = std::move(callback);
}

// ============================================================================
// Configuration
// ============================================================================

void WebSocketClient::set_receive_timeout(int timeout_ms) {
    impl_->receive_timeout_ms = timeout_ms;
}

void WebSocketClient::set_send_timeout(int timeout_ms) {
    impl_->send_timeout_ms = timeout_ms;
}

void WebSocketClient::set_auto_reconnect(bool enable, int max_attempts, int delay_ms) {
    impl_->auto_reconnect = enable;
    impl_->max_reconnect_attempts = max_attempts;
    impl_->reconnect_delay_ms = delay_ms;
}

void WebSocketClient::set_ping_interval(int interval_ms) {
    impl_->ping_interval_ms = interval_ms;
}

// ============================================================================
// Information
// ============================================================================

std::string WebSocketClient::get_remote_address() const {
    return impl_->host + ":" + std::to_string(impl_->port);
}

std::string WebSocketClient::get_url() const {
    return impl_->url;
}

std::string WebSocketClient::get_last_error() const {
    std::lock_guard<std::mutex> lock(impl_->error_mutex);
    return impl_->last_error;
}

// ============================================================================
// Thread Functions
// ============================================================================

void WebSocketClient::receiver_thread_func() {
    std::vector<uint8_t> buffer(8192);
    
    while (impl_->running && impl_->socket != INVALID_SOCKET) {
        int received = recv(impl_->socket, reinterpret_cast<char*>(buffer.data()), 
                           static_cast<int>(buffer.size()), 0);
        
        if (received <= 0) {
            if (impl_->running) {
                impl_->set_error("Connection lost / فقد الاتصال");
                close(CloseCode::AbnormalClose, "Connection lost");
            }
            break;
        }
        
        // Append to read buffer
        impl_->read_buffer.insert(impl_->read_buffer.end(), 
                                   buffer.begin(), buffer.begin() + received);
        
        // Try to parse frames
        while (!impl_->read_buffer.empty()) {
            auto parse_result = FrameParser::parse(impl_->read_buffer);
            
            if (parse_result.result == FrameParser::ParseResult::Incomplete) {
                break;  // Need more data
            }
            
            if (parse_result.result == FrameParser::ParseResult::Error) {
                impl_->set_error(parse_result.error_message);
                close(CloseCode::ProtocolError, parse_result.error_message);
                return;
            }
            
            // Process the frame
            process_frame(parse_result.frame);
            
            // Remove processed data from buffer
            size_t frame_size = 2 + parse_result.frame.payload_length;
            if (parse_result.frame.payload_length >= 126) frame_size += 2;
            if (parse_result.frame.payload_length >= 65536) frame_size += 6;
            if (parse_result.frame.masked) frame_size += 4;
            
            impl_->read_buffer.erase(impl_->read_buffer.begin(), 
                                      impl_->read_buffer.begin() + frame_size);
        }
    }
}

void WebSocketClient::process_frame(const WebSocketFrame& frame) {
    switch (frame.opcode) {
        case WebSocketOpcode::Text: {
            if (!frame.fin) {
                impl_->fragment_buffer = frame.payload;
                impl_->fragment_type = MessageType::Text;
                return;
            }
            
            WebSocketMessage msg;
            msg.type = MessageType::Text;
            msg.data = frame.payload;
            
            {
                std::lock_guard<std::mutex> lock(impl_->queue_mutex);
                impl_->message_queue.push(std::move(msg));
            }
            impl_->queue_cv.notify_one();
            
            if (impl_->on_message_cb) {
                WebSocketMessage cb_msg;
                cb_msg.type = MessageType::Text;
                cb_msg.data = frame.payload;
                impl_->on_message_cb(cb_msg);
            }
            break;
        }
        
        case WebSocketOpcode::Binary: {
            if (!frame.fin) {
                impl_->fragment_buffer = frame.payload;
                impl_->fragment_type = MessageType::Binary;
                return;
            }
            
            WebSocketMessage msg;
            msg.type = MessageType::Binary;
            msg.data = frame.payload;
            
            {
                std::lock_guard<std::mutex> lock(impl_->queue_mutex);
                impl_->message_queue.push(std::move(msg));
            }
            impl_->queue_cv.notify_one();
            
            if (impl_->on_message_cb) {
                WebSocketMessage cb_msg;
                cb_msg.type = MessageType::Binary;
                cb_msg.data = frame.payload;
                impl_->on_message_cb(cb_msg);
            }
            break;
        }
        
        case WebSocketOpcode::Continuation:
            handle_fragmented_message(frame);
            break;
        
        case WebSocketOpcode::Close: {
            auto close_info = FrameParser::parse_close_payload(frame.payload);
            close(close_info.code, close_info.reason);
            break;
        }
        
        case WebSocketOpcode::Ping: {
            // Send pong
            auto pong_frame = FrameBuilder::build_pong(frame.payload, true);
            send_frame(pong_frame);
            
            if (impl_->on_ping_cb) {
                impl_->on_ping_cb(frame.payload);
            }
            break;
        }
        
        case WebSocketOpcode::Pong:
            if (impl_->on_pong_cb) {
                impl_->on_pong_cb(frame.payload);
            }
            break;
        
        default:
            break;
    }
}

void WebSocketClient::handle_fragmented_message(const WebSocketFrame& frame) {
    impl_->fragment_buffer.insert(impl_->fragment_buffer.end(),
                                   frame.payload.begin(), frame.payload.end());
    
    if (frame.fin) {
        WebSocketMessage msg;
        msg.type = impl_->fragment_type;
        msg.data = std::move(impl_->fragment_buffer);
        impl_->fragment_buffer.clear();
        
        {
            std::lock_guard<std::mutex> lock(impl_->queue_mutex);
            impl_->message_queue.push(msg);
        }
        impl_->queue_cv.notify_one();
        
        if (impl_->on_message_cb) {
            impl_->on_message_cb(msg);
        }
    }
}

void WebSocketClient::heartbeat_thread_func() {
    while (impl_->running && impl_->ping_interval_ms > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(impl_->ping_interval_ms));
        
        if (impl_->running && impl_->state == ConnectionState::Open) {
            ping();
        }
    }
}

} // namespace websocket
} // namespace network
} // namespace sad
