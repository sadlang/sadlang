# بسم الله الرحمن الرحيم
# 🚀 Phase 2.1.1 - Next Steps & Future Work
# الخطوات القادمة والعمل المستقبلي

**المرحلة / Phase:** 2.1.1 - TCP/UDP Sockets  
**الحالة الحالية / Current Status:** ✅ Core Implementation Complete  
**التاريخ / Date:** 7 يناير 2026

---

## 📋 Overview / نظرة عامة

هذا الملف يحدد الخطوات القادمة لاستكمال Phase 2.1.1 وتجهيزها للاستخدام الكامل في لغة ص.

---

## ⏳ Pending Work / العمل المتبقي

### Priority 1: Testing (High Priority) 🔴

#### 1.1 Unit Tests
```
Status: ❌ Not Started
Estimated: 8-12 hours
Priority: Critical

Files to Create:
  📁 tests/network/
    ├── test_network_error.cpp
    ├── test_socket_address.cpp
    ├── test_socket_base.cpp
    ├── test_tcp_socket.cpp
    └── test_udp_socket.cpp
```

**test_network_error.cpp** (200 lines)
```cpp
TEST(NetworkError, ErrorCodeToArabic) {
    std::string msg = error_code_to_arabic(
        NetworkErrorCode::SOCKET_CONNECT_FAILED
    );
    EXPECT_EQ(msg, "فشل الاتصال بالخادم");
}

TEST(NetworkError, SystemErrorConversion) {
    #ifdef _WIN32
        NetworkErrorCode code = system_error_to_network_error(WSAECONNREFUSED);
        EXPECT_EQ(code, NetworkErrorCode::SOCKET_CONNECT_FAILED);
    #endif
}

TEST(NetworkException, ArabicMessage) {
    NetworkException ex(NetworkErrorCode::TIMEOUT,
                       "انتهت المهلة",
                       "Timeout");
    EXPECT_EQ(ex.what_arabic(), "انتهت المهلة");
}
```

**test_socket_address.cpp** (250 lines)
```cpp
TEST(SocketAddress, IPv4Construction) {
    SocketAddress addr("192.168.1.1", 8080);
    EXPECT_EQ(addr.get_ip(), "192.168.1.1");
    EXPECT_EQ(addr.get_port(), 8080);
    EXPECT_TRUE(addr.is_ipv4());
}

TEST(SocketAddress, IPv6Construction) {
    SocketAddress addr("::1", 8080);
    EXPECT_TRUE(addr.is_ipv6());
}

TEST(SocketAddress, InvalidAddress) {
    EXPECT_THROW(
        SocketAddress("invalid", 8080),
        NetworkException
    );
}

TEST(SocketAddress, Validation) {
    EXPECT_TRUE(SocketAddress::is_valid_ipv4("192.168.1.1"));
    EXPECT_FALSE(SocketAddress::is_valid_ipv4("999.999.999.999"));
    EXPECT_TRUE(SocketAddress::is_valid_ipv6("::1"));
}
```

**test_tcp_socket.cpp** (400 lines)
```cpp
TEST(TcpSocket, BasicServerClient) {
    // Server thread
    TcpSocket server;
    server.bind(9000);
    server.listen();
    
    std::thread server_thread([&server]() {
        TcpSocket client = server.accept();
        char buffer[100];
        size_t bytes = client.receive(buffer, sizeof(buffer));
        client.send_all(buffer, bytes);
    });
    
    // Client
    TcpSocket client;
    client.connect("127.0.0.1", 9000);
    std::string msg = "test";
    client.send_all(msg);
    
    char buffer[100];
    size_t bytes = client.receive(buffer, sizeof(buffer));
    EXPECT_EQ(std::string(buffer, bytes), msg);
    
    server_thread.join();
}

TEST(TcpSocket, SendAll) {
    // Test that send_all handles partial sends
}

TEST(TcpSocket, Timeout) {
    TcpSocket socket;
    socket.set_receive_timeout(1000);
    socket.bind(9001);
    socket.listen();
    
    auto start = std::chrono::steady_clock::now();
    EXPECT_THROW(
        socket.accept(), // Should timeout
        NetworkException
    );
    auto duration = std::chrono::steady_clock::now() - start;
    
    // Should be approximately 1 second
    EXPECT_GT(duration, std::chrono::milliseconds(900));
}
```

**test_udp_socket.cpp** (300 lines)
```cpp
TEST(UdpSocket, BasicSendReceive) {
    UdpSocket receiver;
    receiver.bind(8000);
    
    UdpSocket sender;
    std::string msg = "test";
    sender.send_to(msg, SocketAddress("127.0.0.1", 8000));
    
    char buffer[100];
    SocketAddress from;
    size_t bytes = receiver.receive_from(buffer, sizeof(buffer), from);
    
    EXPECT_EQ(std::string(buffer, bytes), msg);
}

TEST(UdpSocket, Broadcast) {
    UdpSocket receiver;
    receiver.bind(8001);
    
    UdpSocket sender;
    sender.enable_broadcast(true);
    sender.send_broadcast("broadcast", 8001);
    
    // Receiver should get the message
}

TEST(UdpSocket, Multicast) {
    UdpSocket receiver;
    receiver.bind(8002);
    receiver.join_multicast_group("239.255.0.1");
    
    UdpSocket sender;
    sender.send_to("multicast", SocketAddress("239.255.0.1", 8002));
    
    // Receiver should get the message
}
```

#### Test Coverage Goals
```
Component              Target Coverage
═══════════════════════════════════════
NetworkException           >95%
SocketAddress             >90%
SocketBase                >85%
TcpSocket                 >85%
UdpSocket                 >85%
───────────────────────────────────────
Overall                   >85%
```

---

### Priority 2: Sad Language Bindings (High Priority) 🔴

#### 2.1 Create Sad Interface Files
```
Status: ❌ Not Started
Estimated: 12-16 hours
Priority: Critical

Files to Create:
  📁 stdlib/network/
    ├── socket.s              (Main interface)
    ├── tcp.s                 (TCP helpers)
    ├── udp.s                 (UDP helpers)
    └── examples/
        ├── tcp_server.s
        ├── tcp_client.s
        ├── udp_server.s
        ├── udp_client.s
        ├── chat_server.s
        ├── file_transfer.s
        └── multicast_example.s
```

**stdlib/network/socket.s** (300 lines)
```sad
# مكتبة الشبكات الأساسية

# أنواع
نوع عنوان_مقبس = {
    آي_بي: نص
    منفذ: رقم
}

نوع مقبس_tcp = {
    مؤشر: رقم_كبير  # Pointer to C++ object
}

نوع مقبس_udp = {
    مؤشر: رقم_كبير
}

# دوال TCP
دالة انشئ_مقبس_tcp() -> مقبس_tcp {
    # Call C++ TcpSocket constructor
    خارجية "tcp_socket_create"
}

دالة اتصل(مقبس: مقبس_tcp، آي_بي: نص، منفذ: رقم) {
    # Call C++ TcpSocket::connect
    خارجية "tcp_socket_connect"
}

دالة اربط(مقبس: مقبس_tcp، منفذ: رقم) {
    خارجية "tcp_socket_bind"
}

دالة استمع(مقبس: مقبس_tcp، عدد_الانتظار: رقم = 10) {
    خارجية "tcp_socket_listen"
}

دالة اقبل(مقبس: مقبس_tcp) -> مقبس_tcp {
    خارجية "tcp_socket_accept"
}

دالة ارسل(مقبس: مقبس_tcp، بيانات: نص) -> رقم {
    خارجية "tcp_socket_send"
}

دالة استقبل(مقبس: مقبس_tcp، حجم: رقم) -> نص {
    خارجية "tcp_socket_receive"
}

دالة ارسل_كامل(مقبس: مقبس_tcp، بيانات: نص) {
    خارجية "tcp_socket_send_all"
}

دالة اغلق(مقبس: مقبس_tcp) {
    خارجية "tcp_socket_close"
}

# دوال UDP
دالة انشئ_مقبس_udp() -> مقبس_udp {
    خارجية "udp_socket_create"
}

دالة ارسل_الى(مقبس: مقبس_udp، بيانات: نص، عنوان: عنوان_مقبس) -> رقم {
    خارجية "udp_socket_send_to"
}

دالة استقبل_من(مقبس: مقبس_udp، حجم: رقم) -> {بيانات: نص، من: عنوان_مقبس} {
    خارجية "udp_socket_receive_from"
}

دالة انضم_لمجموعة_البث(مقبس: مقبس_udp، عنوان_المجموعة: نص) {
    خارجية "udp_socket_join_multicast"
}

# دوال مساعدة
دالة عنوان_محلي(منفذ: رقم) -> عنوان_مقبس {
    ارجع {
        آي_بي: "127.0.0.1"
        منفذ: منفذ
    }
}

دالة عنوان_اي(منفذ: رقم) -> عنوان_مقبس {
    ارجع {
        آي_بي: "0.0.0.0"
        منفذ: منفذ
    }
}
```

**examples/tcp_server.s** (50 lines)
```sad
# خادم TCP بسيط

استورد شبكة من "stdlib/network"

دالة رئيسية() {
    # انشئ خادم
    خادم = شبكة.انشئ_مقبس_tcp()
    
    # اربط المنفذ
    شبكة.اربط(خادم، 8080)
    اكتب("الخادم يستمع على المنفذ 8080...")
    
    # ابدأ الاستماع
    شبكة.استمع(خادم)
    
    # اقبل الاتصالات
    بينما صحيح {
        عميل = شبكة.اقبل(خادم)
        اكتب("عميل جديد متصل!")
        
        # استقبل البيانات
        بيانات = شبكة.استقبل(عميل، 1024)
        اكتب("استلمت: " + بيانات)
        
        # ارجع نفس البيانات (echo)
        شبكة.ارسل_كامل(عميل، بيانات)
        
        # اغلق اتصال العميل
        شبكة.اغلق(عميل)
    }
}
```

**examples/tcp_client.s** (40 lines)
```sad
# عميل TCP بسيط

استورد شبكة من "stdlib/network"

دالة رئيسية() {
    # انشئ عميل
    عميل = شبكة.انشئ_مقبس_tcp()
    
    # اتصل بالخادم
    شبكة.اتصل(عميل، "127.0.0.1"، 8080)
    اكتب("متصل بالخادم!")
    
    # ارسل رسالة
    رسالة = "مرحباً من لغة ص!"
    شبكة.ارسل_كامل(عميل، رسالة)
    اكتب("أرسلت: " + رسالة)
    
    # استقبل الرد
    رد = شبكة.استقبل(عميل، 1024)
    اكتب("استلمت: " + رد)
    
    # اغلق الاتصال
    شبكة.اغلق(عميل)
}
```

#### C++ Binding Implementation
```cpp
// File: stdlib/network/bindings.cpp

#include "interpreter/interpreter.h"
#include "network/tcp_socket.h"
#include "network/udp_socket.h"

using namespace sad;
using namespace sad::network;

// TCP Socket bindings
extern "C" {
    void* tcp_socket_create() {
        return new TcpSocket();
    }
    
    void tcp_socket_connect(void* socket, const char* ip, int port) {
        TcpSocket* tcp = static_cast<TcpSocket*>(socket);
        tcp->connect(std::string(ip), static_cast<uint16_t>(port));
    }
    
    void tcp_socket_bind(void* socket, int port) {
        TcpSocket* tcp = static_cast<TcpSocket*>(socket);
        tcp->bind(static_cast<uint16_t>(port));
    }
    
    void tcp_socket_listen(void* socket, int backlog) {
        TcpSocket* tcp = static_cast<TcpSocket*>(socket);
        tcp->listen(backlog);
    }
    
    void* tcp_socket_accept(void* socket) {
        TcpSocket* tcp = static_cast<TcpSocket*>(socket);
        TcpSocket client = tcp->accept();
        return new TcpSocket(std::move(client));
    }
    
    int tcp_socket_send(void* socket, const char* data, int size) {
        TcpSocket* tcp = static_cast<TcpSocket*>(socket);
        return tcp->send(data, size);
    }
    
    // ... more bindings
}

// Register with interpreter
void register_network_bindings(Interpreter& interp) {
    interp.register_external_function("tcp_socket_create", 
                                     (void*)tcp_socket_create);
    interp.register_external_function("tcp_socket_connect", 
                                     (void*)tcp_socket_connect);
    // ... register all functions
}
```

---

### Priority 3: Integration with Interpreter (Medium Priority) 🟡

#### 3.1 Interpreter Integration
```
Status: ❌ Not Started
Estimated: 6-8 hours
Priority: Medium

Tasks:
  - [ ] Add network module loader
  - [ ] Register external functions
  - [ ] Handle exceptions (C++ → Sad)
  - [ ] Memory management for socket objects
  - [ ] Threading support for servers
```

**Interpreter Changes Required:**
```cpp
// In interpreter/interpreter.h
class Interpreter {
public:
    // Add external function registration
    void register_external_function(
        const std::string& name,
        void* function_ptr
    );
    
    // Add module loading
    void load_module(const std::string& module_name);
    
private:
    std::map<std::string, void*> m_external_functions;
};

// Exception conversion
SadValue convert_cpp_exception(const std::exception& e);
```

---

### Priority 4: Performance Testing (Medium Priority) 🟡

#### 4.1 Benchmarks
```
Status: ❌ Not Started
Estimated: 4-6 hours
Priority: Medium

Files to Create:
  📁 benchmarks/network/
    ├── tcp_throughput.cpp
    ├── tcp_latency.cpp
    ├── udp_throughput.cpp
    └── concurrent_connections.cpp
```

**tcp_throughput.cpp**
```cpp
// Measure TCP throughput
void benchmark_tcp_throughput() {
    const size_t DATA_SIZE = 100 * 1024 * 1024; // 100 MB
    const size_t CHUNK_SIZE = 64 * 1024;        // 64 KB
    
    // Server
    TcpSocket server;
    server.bind(9000);
    server.listen();
    
    // Client
    TcpSocket client;
    client.connect("127.0.0.1", 9000);
    
    TcpSocket peer = server.accept();
    
    // Measure send
    auto start = high_resolution_clock::now();
    
    size_t total = 0;
    std::vector<uint8_t> data(CHUNK_SIZE, 'x');
    
    while (total < DATA_SIZE) {
        client.send_all(data);
        total += CHUNK_SIZE;
    }
    
    auto duration = high_resolution_clock::now() - start;
    double seconds = duration_cast<milliseconds>(duration).count() / 1000.0;
    double mbps = (DATA_SIZE / (1024.0 * 1024.0)) / seconds;
    
    std::cout << "Throughput: " << mbps << " MB/s" << std::endl;
}
```

**Performance Goals:**
```
Metric                    Target        Notes
══════════════════════════════════════════════
TCP Throughput           >500 MB/s     Localhost
TCP Latency              <1 ms         Small messages
UDP Throughput           >800 MB/s     Large datagrams
Concurrent Connections   >10,000       With proper limits
```

---

### Priority 5: Documentation Updates (Low Priority) 🟢

#### 5.1 Additional Documentation
```
Status: ⏳ Partial
Estimated: 4-6 hours
Priority: Low

Additional Files Needed:
  📁 docs/network/ar/
    ├── 11_اختبار_التطبيقات.md
    ├── 12_الأداء_والتحسين.md
    └── 13_حل_مشاكل_متقدمة.md
```

---

## 🎯 Roadmap / خارطة الطريق

### Week 1 (8-14 Jan 2026)
```
Day 1-2: Unit Tests
  ✅ Write all unit tests
  ✅ Achieve >85% coverage
  ✅ Fix any bugs found

Day 3-4: Sad Language Bindings
  ✅ Create socket.s interface
  ✅ Implement C++ bindings
  ✅ Write example programs

Day 5: Integration
  ✅ Integrate with interpreter
  ✅ Test Sad programs
  ✅ Fix issues
```

### Week 2 (15-21 Jan 2026)
```
Day 1-2: Performance Testing
  ✅ Write benchmarks
  ✅ Measure performance
  ✅ Optimize if needed

Day 3: Documentation Updates
  ✅ Add testing guide
  ✅ Add performance guide

Day 4-5: Phase 2.1.2 Planning
  ✅ HTTP Client & Server design
  ✅ Start implementation
```

---

## 📊 Success Criteria / معايير النجاح

### Phase 2.1.1 COMPLETE When:
```
✅ Core C++ Implementation
✅ CMake Integration
✅ Arabic Documentation (11 files)
✅ Completion Report

⏳ Unit Tests (>85% coverage)
⏳ Sad Language Bindings
⏳ Example Programs in Sad
⏳ Integration Tests
⏳ Performance Benchmarks
⏳ Interpreter Integration

Final Checklist:
  [ ] All tests passing
  [ ] Can write networking apps in Sad language
  [ ] Performance meets goals
  [ ] Documentation complete
  [ ] Ready for Phase 2.1.2
```

---

## 🚦 Blockers / المعوقات

### Current Blockers
```
None - Clear path forward
```

### Potential Risks
```
Risk: Unit test complexity
Mitigation: Start with simple tests, add complexity gradually

Risk: Sad language binding complexity
Mitigation: Start with basic functions, expand incrementally

Risk: Performance issues
Mitigation: Benchmark early, optimize as needed
```

---

## 💡 Ideas for Future Enhancements

### Beyond Phase 2.1.1
```
1. Async I/O Support
   - Non-blocking sockets
   - Event loop integration
   - Async/await pattern in Sad

2. Connection Pooling
   - Reuse connections
   - Better performance

3. Protocol Helpers
   - HTTP parser library
   - WebSocket framing
   - Common protocol implementations

4. DNS Resolution
   - Hostname lookup
   - Async DNS

5. Unix Domain Sockets
   - IPC on same machine
   - Better performance than TCP

6. SCTP Support
   - Message-oriented protocol
   - Multiple streams

7. Network Monitoring
   - Statistics collection
   - Traffic analysis
   - Connection tracking
```

---

**تاريخ التحديث / Last Updated:** 7 يناير 2026  
**حالة الوثيقة / Document Status:** ✅ Active Planning Document  
**المراجعة القادمة / Next Review:** 14 يناير 2026
