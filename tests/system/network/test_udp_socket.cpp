// بسم الله الرحمن الرحيم
// test_udp_socket.cpp - Unit tests for UdpSocket class

#include <gtest/gtest.h>
#include "network/udp_socket.h"
#include "network/network_error.h"
#include <thread>
#include <chrono>

using namespace sad::network;

// Helper function to find available port
static int find_available_port() {
    static int port = 10000;
    return port++;
}

// Test basic UDP socket creation
TEST(UdpSocket, Creation) {
    UdpSocket socket;
    EXPECT_TRUE(socket.is_valid());
}

// Test UDP socket binding
TEST(UdpSocket, Binding) {
    UdpSocket socket;
    int port = find_available_port();
    
    EXPECT_NO_THROW(socket.bind(port));
    
    // Try binding to same port again (should fail)
    UdpSocket socket2;
    EXPECT_THROW(socket2.bind(port), NetworkException);
}

// Test basic send/receive
TEST(UdpSocket, BasicSendReceive) {
    int port = find_available_port();
    
    UdpSocket receiver;
    receiver.bind(port);
    
    UdpSocket sender;
    std::string msg = "Hello, UDP!";
    sender.send_to(msg.c_str(), msg.size(), SocketAddress("127.0.0.1", port));
    
    char buffer[100];
    SocketAddress from;
    size_t bytes = receiver.receive_from(buffer, sizeof(buffer), from);
    
    EXPECT_GT(bytes, 0);
    EXPECT_EQ(std::string(buffer, bytes), msg);
    EXPECT_EQ(from.get_ip(), "127.0.0.1");
}

// Test broadcast
TEST(UdpSocket, Broadcast) {
    int port = find_available_port();
    
    UdpSocket receiver;
    receiver.bind(port);
    receiver.set_broadcast(true);
    
    UdpSocket sender;
    sender.set_broadcast(true);
    
    std::string msg = "Broadcast message";
    sender.send_to(
        msg.c_str(),
        msg.size(),
        SocketAddress("255.255.255.255", port)
    );
    
    receiver.set_receive_timeout(2000); // 2 seconds
    
    char buffer[100];
    SocketAddress from;
    size_t bytes = receiver.receive_from(buffer, sizeof(buffer), from);
    
    EXPECT_GT(bytes, 0);
    EXPECT_EQ(std::string(buffer, bytes), msg);
}

// Test multicast join/leave
TEST(UdpSocket, MulticastJoinLeave) {
    UdpSocket socket;
    int port = find_available_port();
    socket.bind(port);
    
    std::string multicast_group = "239.255.0.1";
    
    EXPECT_NO_THROW(socket.join_multicast_group(multicast_group));
    EXPECT_NO_THROW(socket.leave_multicast_group(multicast_group));
}

// Test multicast communication
TEST(UdpSocket, MulticastCommunication) {
    std::string multicast_group = "239.255.0.1";
    int port = find_available_port();
    
    // Receiver thread
    std::thread receiver_thread([multicast_group, port]() {
        UdpSocket receiver;
        receiver.set_reuse_address(true);
        receiver.bind(port);
        receiver.join_multicast_group(multicast_group);
        receiver.set_receive_timeout(5000); // 5 seconds
        
        char buffer[100];
        SocketAddress from;
        size_t bytes = receiver.receive_from(buffer, sizeof(buffer), from);
        
        EXPECT_GT(bytes, 0);
        EXPECT_EQ(std::string(buffer, bytes), "Multicast message");
    });
    
    // Give receiver time to setup
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // Sender
    UdpSocket sender;
    sender.set_multicast_ttl(1); // Local network only
    
    std::string msg = "Multicast message";
    sender.send_to(
        msg.c_str(),
        msg.size(),
        SocketAddress(multicast_group, port)
    );
    
    receiver_thread.join();
}

// Test multicast TTL
TEST(UdpSocket, MulticastTTL) {
    UdpSocket socket;
    
    EXPECT_NO_THROW(socket.set_multicast_ttl(1));
    EXPECT_NO_THROW(socket.set_multicast_ttl(64));
    EXPECT_NO_THROW(socket.set_multicast_ttl(255));
    
    EXPECT_THROW(socket.set_multicast_ttl(256), NetworkException);
}

// Test multicast loopback
TEST(UdpSocket, MulticastLoopback) {
    UdpSocket socket;
    
    EXPECT_NO_THROW(socket.set_multicast_loopback(true));
    EXPECT_NO_THROW(socket.set_multicast_loopback(false));
}

// Test socket options
TEST(UdpSocket, SocketOptions) {
    UdpSocket socket;
    
    // Test SO_REUSEADDR
    EXPECT_NO_THROW(socket.set_reuse_address(true));
    
    // Test SO_BROADCAST
    EXPECT_NO_THROW(socket.set_broadcast(true));
}

// Test receive timeout
TEST(UdpSocket, ReceiveTimeout) {
    UdpSocket socket;
    int port = find_available_port();
    socket.bind(port);
    socket.set_receive_timeout(1000); // 1 second
    
    auto start = std::chrono::steady_clock::now();
    
    char buffer[100];
    SocketAddress from;
    EXPECT_THROW(
        socket.receive_from(buffer, sizeof(buffer), from),
        NetworkException
    );
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start
    );
    
    EXPECT_GT(duration.count(), 900);
    EXPECT_LT(duration.count(), 1500);
}

// Test large datagram
TEST(UdpSocket, LargeDatagram) {
    int port = find_available_port();
    
    UdpSocket receiver;
    receiver.bind(port);
    
    UdpSocket sender;
    
    // Send large datagram (may be fragmented)
    std::vector<char> large_data(5000, 'X');
    EXPECT_NO_THROW(
        sender.send_to(
            large_data.data(),
            large_data.size(),
            SocketAddress("127.0.0.1", port)
        )
    );
    
    receiver.set_receive_timeout(2000);
    
    std::vector<char> buffer(6000);
    SocketAddress from;
    size_t bytes = receiver.receive_from(
        buffer.data(),
        buffer.size(),
        from
    );
    
    EXPECT_EQ(bytes, large_data.size());
}

// Test MTU-safe datagram
TEST(UdpSocket, MTUSafeDatagram) {
    int port = find_available_port();
    
    UdpSocket receiver;
    receiver.bind(port);
    
    UdpSocket sender;
    
    // Send MTU-safe datagram (1400 bytes)
    std::vector<char> safe_data(1400, 'Y');
    sender.send_to(
        safe_data.data(),
        safe_data.size(),
        SocketAddress("127.0.0.1", port)
    );
    
    receiver.set_receive_timeout(2000);
    
    std::vector<char> buffer(2000);
    SocketAddress from;
    size_t bytes = receiver.receive_from(
        buffer.data(),
        buffer.size(),
        from
    );
    
    EXPECT_EQ(bytes, safe_data.size());
}

// Test get_local_address
TEST(UdpSocket, GetLocalAddress) {
    UdpSocket socket;
    int port = find_available_port();
    
    socket.bind(port);
    
    SocketAddress local = socket.get_local_address();
    EXPECT_EQ(local.get_port(), port);
}

// Test socket close
TEST(UdpSocket, Close) {
    UdpSocket socket;
    int port = find_available_port();
    
    socket.bind(port);
    EXPECT_TRUE(socket.is_valid());
    
    socket.close();
    EXPECT_FALSE(socket.is_valid());
    
    // Double close should be safe
    EXPECT_NO_THROW(socket.close());
}

// Test move semantics
TEST(UdpSocket, MoveSemantics) {
    UdpSocket socket1;
    int port = find_available_port();
    socket1.bind(port);
    
    // Move constructor
    UdpSocket socket2 = std::move(socket1);
    EXPECT_TRUE(socket2.is_valid());
    EXPECT_FALSE(socket1.is_valid());
    
    // Move assignment
    UdpSocket socket3;
    socket3 = std::move(socket2);
    EXPECT_TRUE(socket3.is_valid());
    EXPECT_FALSE(socket2.is_valid());
}

// Test multiple datagrams
TEST(UdpSocket, MultipleDatagrams) {
    int port = find_available_port();
    
    UdpSocket receiver;
    receiver.bind(port);
    receiver.set_receive_timeout(2000);
    
    UdpSocket sender;
    
    const int num_messages = 10;
    
    // Send multiple datagrams
    for (int i = 0; i < num_messages; ++i) {
        std::string msg = "Message " + std::to_string(i);
        sender.send_to(
            msg.c_str(),
            msg.size(),
            SocketAddress("127.0.0.1", port)
        );
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Receive multiple datagrams
    for (int i = 0; i < num_messages; ++i) {
        char buffer[100];
        SocketAddress from;
        size_t bytes = receiver.receive_from(buffer, sizeof(buffer), from);
        
        std::string expected = "Message " + std::to_string(i);
        EXPECT_EQ(std::string(buffer, bytes), expected);
    }
}

// Test connect() for UDP (optional connected mode)
TEST(UdpSocket, ConnectedMode) {
    int port = find_available_port();
    
    UdpSocket receiver;
    receiver.bind(port);
    
    UdpSocket sender;
    sender.connect("127.0.0.1", port);
    
    // Now can use send() instead of send_to()
    std::string msg = "Connected UDP";
    sender.send(msg.c_str(), msg.size());
    
    receiver.set_receive_timeout(2000);
    
    char buffer[100];
    SocketAddress from;
    size_t bytes = receiver.receive_from(buffer, sizeof(buffer), from);
    
    EXPECT_EQ(std::string(buffer, bytes), msg);
}

// Test buffer size settings
TEST(UdpSocket, BufferSize) {
    UdpSocket socket;
    
    EXPECT_NO_THROW(socket.set_receive_buffer_size(65536));
    EXPECT_NO_THROW(socket.set_send_buffer_size(65536));
    
    int recv_size = socket.get_receive_buffer_size();
    int send_size = socket.get_send_buffer_size();
    
    EXPECT_GT(recv_size, 0);
    EXPECT_GT(send_size, 0);
}

// Test IPv6 UDP socket
TEST(UdpSocket, IPv6) {
    int port = find_available_port();
    
    try {
        UdpSocket receiver;
        receiver.bind("::1", port);
        
        UdpSocket sender;
        std::string msg = "IPv6 UDP";
        sender.send_to(
            msg.c_str(),
            msg.size(),
            SocketAddress("::1", port)
        );
        
        receiver.set_receive_timeout(2000);
        
        char buffer[100];
        SocketAddress from;
        size_t bytes = receiver.receive_from(buffer, sizeof(buffer), from);
        
        EXPECT_EQ(std::string(buffer, bytes), msg);
        EXPECT_TRUE(from.is_ipv6());
    } catch (const NetworkException&) {
        // IPv6 might not be available on this system
        GTEST_SKIP() << "IPv6 not available";
    }
}

// Main test entry point
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
