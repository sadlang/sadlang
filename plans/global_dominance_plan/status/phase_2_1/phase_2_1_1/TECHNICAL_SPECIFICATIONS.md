# بسم الله الرحمن الرحيم
# 📋 Phase 2.1.1 - Technical Specifications
# المواصفات الفنية التفصيلية

**المرحلة / Phase:** 2.1.1 - TCP/UDP Sockets  
**التاريخ / Date:** 7 يناير 2026  
**الحالة / Status:** ✅ Implemented

---

## 🏗️ Architecture Overview / نظرة عامة على البنية

### Class Hierarchy
```
                    ┌─────────────┐
                    │ SocketBase  │
                    │  (Base)     │
                    └──────┬──────┘
                           │
              ┌────────────┴────────────┐
              │                         │
       ┌──────▼──────┐          ┌──────▼──────┐
       │  TcpSocket  │          │  UdpSocket  │
       │  (Derived)  │          │  (Derived)  │
       └─────────────┘          └─────────────┘

    ┌──────────────┐         ┌──────────────────┐
    │SocketAddress │         │NetworkException  │
    │  (Helper)    │         │  (Exception)     │
    └──────────────┘         └──────────────────┘
```

### Component Dependencies
```
┌────────────────────────────────────────┐
│         Application Code               │
└──────────────┬─────────────────────────┘
               │
               ▼
┌────────────────────────────────────────┐
│      TcpSocket / UdpSocket             │
│      (High-level API)                  │
└──────────────┬─────────────────────────┘
               │
               ▼
┌────────────────────────────────────────┐
│         SocketBase                     │
│    (Common functionality)              │
└──────────────┬─────────────────────────┘
               │
               ▼
┌────────────────────────────────────────┐
│   OS Socket API (Winsock2/POSIX)      │
└────────────────────────────────────────┘
```

---

## 📦 Class Specifications / مواصفات الفئات

### 1. NetworkException

**Purpose:** Exception handling for all network operations

**Header:** `include/network/network_error.h`  
**Implementation:** `stdlib/network/network_error.cpp`

#### Class Definition
```cpp
class NetworkException : public std::exception {
public:
    NetworkException(NetworkErrorCode error_code,
                    const std::string& arabic_msg,
                    const std::string& english_msg);
    
    NetworkException(NetworkErrorCode error_code,
                    const std::string& message);
    
    const char* what() const noexcept override;
    std::string what_arabic() const noexcept;
    std::string what_english() const noexcept;
    NetworkErrorCode error_code() const noexcept;
    int system_error_code() const noexcept;
    void set_system_error_code(int code);

private:
    NetworkErrorCode m_error_code;
    std::string m_arabic_message;
    std::string m_english_message;
    int m_system_error_code;
};
```

#### Error Codes (24 types)
```cpp
enum class NetworkErrorCode {
    // General (1)
    GENERAL_FAILURE,
    
    // Socket Operations (8)
    SOCKET_CREATION_FAILED,
    SOCKET_BIND_FAILED,
    SOCKET_LISTEN_FAILED,
    SOCKET_ACCEPT_FAILED,
    SOCKET_CONNECT_FAILED,
    SOCKET_ALREADY_CONNECTED,
    SOCKET_NOT_CONNECTED,
    SOCKET_CLOSED,
    
    // Data Transfer (7)
    SEND_FAILED,
    RECEIVE_FAILED,
    PARTIAL_SEND,
    PARTIAL_RECEIVE,
    CONNECTION_RESET,
    CONNECTION_ABORTED,
    TIMEOUT,
    WOULD_BLOCK,
    
    // Address (3)
    INVALID_ADDRESS,
    ADDRESS_IN_USE,
    ADDRESS_NOT_AVAILABLE,
    
    // Options (2)
    SET_OPTION_FAILED,
    GET_OPTION_FAILED,
    
    // Broadcast/Multicast (3)
    BROADCAST_NOT_SUPPORTED,
    MULTICAST_JOIN_FAILED,
    MULTICAST_LEAVE_FAILED
};
```

#### Helper Functions
```cpp
// Get last network error from OS
int get_last_network_error();

// Convert error code to messages
std::string error_code_to_arabic(NetworkErrorCode code);
std::string error_code_to_english(NetworkErrorCode code);

// Convert system error to NetworkErrorCode
NetworkErrorCode system_error_to_network_error(int system_code);

// Create exception from current system error
NetworkException create_network_exception_from_system_error(
    NetworkErrorCode fallback_code = NetworkErrorCode::GENERAL_FAILURE
);
```

#### Usage Example
```cpp
try {
    socket.connect("192.168.1.1", 8080);
}
catch (const NetworkException& e) {
    std::cout << "Arabic: " << e.what_arabic() << std::endl;
    std::cout << "English: " << e.what_english() << std::endl;
    std::cout << "Code: " << (int)e.error_code() << std::endl;
    std::cout << "System: " << e.system_error_code() << std::endl;
}
```

---

### 2. SocketAddress

**Purpose:** IPv4/IPv6 address management

**Header:** `include/network/socket_address.h`  
**Implementation:** `stdlib/network/socket_address.cpp`

#### Class Definition
```cpp
class SocketAddress {
public:
    // Constructors
    SocketAddress();
    SocketAddress(const std::string& ip, uint16_t port);
    SocketAddress(uint16_t port);
    SocketAddress(const sockaddr_storage& addr);
    
    // Accessors
    std::string get_ip() const;
    uint16_t get_port() const;
    bool is_ipv4() const;
    bool is_ipv6() const;
    
    // System interface
    sockaddr* get_sockaddr();
    const sockaddr* get_sockaddr() const;
    socklen_t get_sockaddr_length() const;
    
    // Static helpers
    static SocketAddress any_ipv4(uint16_t port);
    static SocketAddress any_ipv6(uint16_t port);
    static SocketAddress localhost_ipv4(uint16_t port);
    static SocketAddress localhost_ipv6(uint16_t port);
    static bool is_valid_ipv4(const std::string& ip);
    static bool is_valid_ipv6(const std::string& ip);

private:
    sockaddr_storage m_addr;
    socklen_t m_addr_len;
};
```

#### Internal Storage
```cpp
// sockaddr_storage can hold both IPv4 and IPv6
struct sockaddr_storage {
    // At least 128 bytes
    // Can hold sockaddr_in (IPv4) or sockaddr_in6 (IPv6)
};
```

#### IPv4 Structure
```cpp
struct sockaddr_in {
    short sin_family;           // AF_INET
    unsigned short sin_port;    // Port number
    struct in_addr sin_addr;    // IPv4 address
    char sin_zero[8];           // Padding
};
```

#### IPv6 Structure
```cpp
struct sockaddr_in6 {
    short sin6_family;          // AF_INET6
    unsigned short sin6_port;   // Port number
    unsigned long sin6_flowinfo;// Flow info
    struct in6_addr sin6_addr;  // IPv6 address
    unsigned long sin6_scope_id;// Scope ID
};
```

#### Usage Examples
```cpp
// IPv4
SocketAddress addr1("192.168.1.100", 8080);
std::cout << addr1.get_ip() << ":" << addr1.get_port();

// IPv6
SocketAddress addr2("2001:0db8::1", 8080);
std::cout << (addr2.is_ipv6() ? "IPv6" : "IPv4");

// Any address (INADDR_ANY)
SocketAddress addr3 = SocketAddress::any_ipv4(3000);

// Localhost
SocketAddress addr4 = SocketAddress::localhost_ipv4(8080);

// Validation
if (SocketAddress::is_valid_ipv4("192.168.1.1")) {
    // Valid
}
```

---

### 3. SocketBase

**Purpose:** Base class with common socket functionality

**Header:** `include/network/socket_base.h`  
**Implementation:** `stdlib/network/socket_base.cpp`

#### Class Definition
```cpp
class SocketBase {
public:
    SocketBase();
    virtual ~SocketBase();
    
    // Delete copy
    SocketBase(const SocketBase&) = delete;
    SocketBase& operator=(const SocketBase&) = delete;
    
    // Move semantics
    SocketBase(SocketBase&& other) noexcept;
    SocketBase& operator=(SocketBase&& other) noexcept;
    
    // Basic operations
    void close();
    bool is_valid() const;
    
    // Timeout configuration
    void set_receive_timeout(int milliseconds);
    void set_send_timeout(int milliseconds);
    
    // Blocking mode
    void set_blocking(bool blocking);
    
    // Socket options
    void set_reuse_address(bool reuse);
    
    // Address queries
    SocketAddress get_local_address() const;
    SocketAddress get_remote_address() const;

protected:
    socket_t m_socket;
    
    void create_socket(int domain, int type, int protocol);
    
    static void initialize_winsock();
    static void cleanup_winsock();

private:
    static bool s_winsock_initialized;
    static int s_winsock_ref_count;
};
```

#### Platform-specific Types
```cpp
#ifdef _WIN32
    using socket_t = SOCKET;
    const socket_t INVALID_SOCKET_VALUE = INVALID_SOCKET;
#else
    using socket_t = int;
    const socket_t INVALID_SOCKET_VALUE = -1;
#endif
```

#### Socket Options Implementation
```cpp
// SO_REUSEADDR
void SocketBase::set_reuse_address(bool reuse) {
    int opt = reuse ? 1 : 0;
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR,
               reinterpret_cast<const char*>(&opt), sizeof(opt));
}

// SO_RCVTIMEO
void SocketBase::set_receive_timeout(int milliseconds) {
#ifdef _WIN32
    DWORD timeout = milliseconds;
#else
    struct timeval timeout;
    timeout.tv_sec = milliseconds / 1000;
    timeout.tv_usec = (milliseconds % 1000) * 1000;
#endif
    setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO,
               reinterpret_cast<const char*>(&timeout), sizeof(timeout));
}
```

#### RAII Pattern
```cpp
// Constructor
SocketBase::SocketBase() : m_socket(INVALID_SOCKET_VALUE) {
    initialize_winsock();
}

// Destructor
SocketBase::~SocketBase() {
    close();
    cleanup_winsock();
}

// Close implementation
void SocketBase::close() {
    if (m_socket != INVALID_SOCKET_VALUE) {
#ifdef _WIN32
        closesocket(m_socket);
#else
        ::close(m_socket);
#endif
        m_socket = INVALID_SOCKET_VALUE;
    }
}
```

---

### 4. TcpSocket

**Purpose:** TCP client and server implementation

**Header:** `include/network/tcp_socket.h`  
**Implementation:** `stdlib/network/tcp_socket.cpp`

#### Class Definition
```cpp
class TcpSocket : public SocketBase {
public:
    TcpSocket();
    explicit TcpSocket(socket_t sock);
    
    // Client operations
    void connect(const SocketAddress& address);
    void connect(const std::string& host, uint16_t port);
    
    // Server operations
    void bind(const SocketAddress& address);
    void bind(uint16_t port);
    void listen(int backlog = 10);
    TcpSocket accept();
    SocketAddress accept(TcpSocket& client_socket);
    
    // Data transfer
    size_t send(const void* data, size_t size);
    size_t send(const std::vector<uint8_t>& data);
    size_t send(const std::string& text);
    
    size_t receive(void* buffer, size_t size);
    size_t receive(std::vector<uint8_t>& buffer, size_t max_size);
    std::string receive_string(size_t max_size = 4096);
    
    // Reliable transfer
    void send_all(const void* data, size_t size);
    void send_all(const std::vector<uint8_t>& data);
    void send_all(const std::string& text);
    
    void receive_all(void* buffer, size_t size);
    void receive_all(std::vector<uint8_t>& buffer, size_t size);
    
    // Connection management
    void shutdown_send();
    void shutdown_receive();
    void shutdown_both();
    
    // TCP options
    void set_nodelay(bool nodelay);
    void set_keepalive(bool keepalive);
    void set_linger(bool enable, int seconds);
    
    // State
    bool is_connected() const;

private:
    bool m_is_connected;
    bool m_is_listening;
};
```

#### TCP Three-Way Handshake
```
Client                          Server
  │                                │
  │───── SYN ────────────────────→│
  │                                │
  │←──── SYN+ACK ─────────────────│
  │                                │
  │───── ACK ────────────────────→│
  │                                │
  │       Connection Established   │
```

#### send_all Implementation (Critical)
```cpp
void TcpSocket::send_all(const void* data, size_t size) {
    const uint8_t* ptr = static_cast<const uint8_t*>(data);
    size_t total_sent = 0;
    
    while (total_sent < size) {
        size_t sent = send(ptr + total_sent, size - total_sent);
        if (sent == 0) {
            throw NetworkException(NetworkErrorCode::CONNECTION_RESET,
                                  "تم إغلاق الاتصال أثناء الإرسال",
                                  "Connection closed during send");
        }
        total_sent += sent;
    }
}
```

#### TCP Options Details
```cpp
// TCP_NODELAY (Disable Nagle's algorithm)
// Nagle's algorithm: Combine small packets
// Use TCP_NODELAY for low-latency applications
void set_nodelay(bool nodelay);

// SO_KEEPALIVE (Keep connection alive)
// Send periodic probes to check if peer is alive
void set_keepalive(bool keepalive);

// SO_LINGER (Control close behavior)
// linger = true, seconds > 0: Wait for data to be sent
// linger = false: Immediate close (RST)
void set_linger(bool enable, int seconds);
```

---

### 5. UdpSocket

**Purpose:** UDP datagram communication

**Header:** `include/network/udp_socket.h`  
**Implementation:** `stdlib/network/udp_socket.cpp`

#### Class Definition
```cpp
class UdpSocket : public SocketBase {
public:
    UdpSocket();
    
    // Binding
    void bind(const SocketAddress& address);
    void bind(uint16_t port);
    
    // Datagram operations
    size_t send_to(const void* data, size_t size, 
                   const SocketAddress& address);
    size_t send_to(const std::vector<uint8_t>& data, 
                   const SocketAddress& address);
    size_t send_to(const std::string& text, 
                   const SocketAddress& address);
    
    size_t receive_from(void* buffer, size_t size, 
                        SocketAddress& sender);
    size_t receive_from(std::vector<uint8_t>& buffer, 
                        size_t max_size, SocketAddress& sender);
    std::string receive_string_from(size_t max_size, 
                                     SocketAddress& sender);
    
    // Connected UDP
    void connect(const SocketAddress& address);
    void disconnect();
    bool is_connected() const;
    
    size_t send(const void* data, size_t size);
    size_t send(const std::vector<uint8_t>& data);
    size_t send(const std::string& text);
    
    size_t receive(void* buffer, size_t size);
    size_t receive(std::vector<uint8_t>& buffer, size_t max_size);
    std::string receive_string(size_t max_size = 4096);
    
    // Broadcast
    void enable_broadcast(bool enable);
    void send_broadcast(const void* data, size_t size, uint16_t port);
    void send_broadcast(const std::vector<uint8_t>& data, uint16_t port);
    void send_broadcast(const std::string& text, uint16_t port);
    
    // Multicast
    void join_multicast_group(const std::string& group_address);
    void leave_multicast_group(const std::string& group_address);
    void set_multicast_ttl(int ttl);
    void set_multicast_loopback(bool enable);

private:
    bool m_is_connected;
};
```

#### Multicast Implementation
```cpp
void UdpSocket::join_multicast_group(const std::string& group_address) {
    struct ip_mreq mreq;
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, group_address.c_str(), &mreq.imr_multiaddr);
    
    setsockopt(m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,
               reinterpret_cast<const char*>(&mreq), sizeof(mreq));
}
```

#### Multicast Address Ranges
```
IPv4 Multicast Ranges:
  224.0.0.0 - 224.0.0.255    Local network
  224.0.1.0 - 238.255.255.255 Internet-wide
  239.0.0.0 - 239.255.255.255 Organization-local

Common Multicast Groups:
  224.0.0.1   All hosts on subnet
  224.0.0.2   All routers on subnet
  224.0.0.5   OSPF routers
  239.255.0.1 Example private group
```

#### TTL (Time-To-Live) Values
```
TTL Value    Scope
───────────────────────────
    0        Same host
    1        Same subnet
   32        Same site
   64        Same region
  128        Same continent
  255        Unrestricted
```

---

## 🔧 Platform-Specific Implementation / التنفيذ الخاص بالمنصة

### Windows (Winsock2)

#### Initialization
```cpp
static bool s_winsock_initialized = false;
static int s_winsock_ref_count = 0;

void SocketBase::initialize_winsock() {
#ifdef _WIN32
    if (!s_winsock_initialized) {
        WSADATA wsa_data;
        int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
        if (result == 0) {
            s_winsock_initialized = true;
        }
    }
    s_winsock_ref_count++;
#endif
}

void SocketBase::cleanup_winsock() {
#ifdef _WIN32
    s_winsock_ref_count--;
    if (s_winsock_ref_count == 0 && s_winsock_initialized) {
        WSACleanup();
        s_winsock_initialized = false;
    }
#endif
}
```

#### Error Handling
```cpp
int get_last_network_error() {
#ifdef _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}
```

#### Winsock Error Codes
```cpp
WSAECONNREFUSED    (10061) - Connection refused
WSAEADDRINUSE      (10048) - Address already in use
WSAEADDRNOTAVAIL   (10049) - Cannot assign requested address
WSAECONNRESET      (10054) - Connection reset by peer
WSAECONNABORTED    (10053) - Software caused connection abort
WSAETIMEDOUT       (10060) - Connection timed out
WSAEWOULDBLOCK     (10035) - Resource temporarily unavailable
WSAEISCONN         (10056) - Socket is already connected
WSAENOTCONN        (10057) - Socket is not connected
```

### Linux (POSIX)

#### Socket Creation
```cpp
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int sock = socket(AF_INET, SOCK_STREAM, 0);
```

#### Error Handling
```cpp
#include <errno.h>
#include <cstring>

if (result < 0) {
    std::string error = strerror(errno);
}
```

#### POSIX Error Codes
```cpp
ECONNREFUSED  (111) - Connection refused
EADDRINUSE     (98) - Address already in use
EADDRNOTAVAIL  (99) - Cannot assign requested address
ECONNRESET    (104) - Connection reset by peer
ECONNABORTED  (103) - Software caused connection abort
ETIMEDOUT     (110) - Connection timed out
EWOULDBLOCK    (11) - Resource temporarily unavailable
EISCONN       (106) - Transport endpoint is already connected
ENOTCONN      (107) - Transport endpoint is not connected
```

---

## 📊 Performance Characteristics / خصائص الأداء

### TCP Performance

#### Nagle's Algorithm
```
Without TCP_NODELAY (Nagle enabled):
  - Small packets combined
  - Higher latency
  - Better bandwidth utilization
  - Good for bulk transfer

With TCP_NODELAY (Nagle disabled):
  - Immediate send
  - Lower latency
  - More packets
  - Good for interactive/real-time
```

#### Buffer Sizes
```cpp
// Default buffer sizes (platform-dependent)
TCP Send Buffer:    ~16 KB - 64 KB
TCP Receive Buffer: ~16 KB - 64 KB

// Adjust with setsockopt:
int buffer_size = 256 * 1024; // 256 KB
setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &buffer_size, sizeof(int));
setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &buffer_size, sizeof(int));
```

### UDP Performance

#### Datagram Size Limits
```
Maximum UDP datagram: 65,535 bytes (theoretical)
Maximum IP packet:    65,535 bytes

Practical limits:
  - Ethernet MTU: 1500 bytes
  - Safe UDP payload: ~1400 bytes
  - Avoid fragmentation: <1472 bytes

Fragmentation issues:
  - Increased latency
  - Higher loss probability
  - No partial recovery
```

#### Multicast Performance
```
Benefits:
  - One send, multiple receivers
  - Efficient for streaming
  - Reduced network load

Considerations:
  - TTL configuration important
  - Router support needed
  - Can cause network congestion
```

---

## 🔒 Security Considerations / اعتبارات الأمان

### Input Validation
```cpp
// Always validate addresses
if (!SocketAddress::is_valid_ipv4(ip)) {
    throw NetworkException(NetworkErrorCode::INVALID_ADDRESS,
                          "عنوان IP غير صالح");
}

// Validate port range
if (port < 1 || port > 65535) {
    throw std::invalid_argument("Invalid port");
}

// Validate buffer sizes
if (size > MAX_BUFFER_SIZE) {
    throw std::invalid_argument("Buffer too large");
}
```

### Resource Limits
```cpp
// Limit concurrent connections
const int MAX_CONNECTIONS = 100;

// Limit request size
const size_t MAX_REQUEST_SIZE = 1024 * 1024; // 1 MB

// Set timeouts
socket.set_receive_timeout(30000); // 30 seconds
socket.set_send_timeout(30000);
```

### Denial of Service Prevention
```cpp
// Rate limiting
// Connection limits
// Timeout enforcement
// Resource cleanup
```

---

## 🧪 Testing Strategy / استراتيجية الاختبار

### Unit Tests (Planned)
```
test_network_error.cpp
  - Error code conversion
  - Arabic message generation
  - System error mapping

test_socket_address.cpp
  - IPv4 parsing
  - IPv6 parsing
  - Validation functions
  - localhost/any helpers

test_tcp_socket.cpp
  - Connect/bind/listen/accept
  - Send/receive
  - send_all/receive_all
  - Shutdown
  - Options

test_udp_socket.cpp
  - send_to/receive_from
  - Broadcast
  - Multicast join/leave
  - Connected mode
```

### Integration Tests (Planned)
```
test_tcp_echo.cpp
  - Full echo server/client

test_udp_multicast.cpp
  - Multiple receivers

test_performance.cpp
  - Throughput
  - Latency
  - Concurrent connections
```

---

## 📚 API Usage Patterns / أنماط استخدام API

### Pattern 1: TCP Echo Server
```cpp
TcpSocket server;
server.bind(8080);
server.listen();

while (true) {
    TcpSocket client = server.accept();
    
    std::thread([client = std::move(client)]() mutable {
        char buffer[1024];
        size_t bytes = client.receive(buffer, sizeof(buffer));
        client.send_all(buffer, bytes);
    }).detach();
}
```

### Pattern 2: TCP Client with Retry
```cpp
const int MAX_RETRIES = 3;
int attempts = 0;

while (attempts < MAX_RETRIES) {
    try {
        TcpSocket client;
        client.set_receive_timeout(5000);
        client.connect("server.com", 8080);
        client.send_all("request");
        break;
    }
    catch (const NetworkException& e) {
        attempts++;
        std::this_thread::sleep_for(std::chrono::seconds(1 << attempts));
    }
}
```

### Pattern 3: UDP Multicast Receiver
```cpp
UdpSocket receiver;
receiver.bind(5000);
receiver.join_multicast_group("239.255.0.1");

while (true) {
    char buffer[1024];
    SocketAddress sender;
    size_t bytes = receiver.receive_from(buffer, sizeof(buffer), sender);
    process(buffer, bytes);
}
```

---

**تاريخ التحديث / Last Updated:** 7 يناير 2026  
**الإصدار / Version:** 1.0  
**الحالة / Status:** ✅ Complete and Accurate
