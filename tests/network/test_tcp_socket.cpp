// بسم الله الرحمن الرحيم
// test_tcp_socket.cpp - Unit tests for TcpSocket class

#include <gtest/gtest.h>
#include "network/tcp_socket.h"
#include "network/network_error.h"
#include <thread>
#include <chrono>
#include <vector>

using namespace sad::network;

// Helper function to find available port
static int find_available_port() {
    static int port = 9000;
    return port++;
}

// Test basic socket creation
TEST(TcpSocket, Creation) {
    TcpSocket socket;
    EXPECT_FALSE(socket.is_connected());
    EXPECT_TRUE(socket.is_valid());
}

// Test socket binding
TEST(TcpSocket, Binding) {
    TcpSocket socket;
    int port = find_available_port();
    
    EXPECT_NO_THROW(socket.bind(port));
    
    // Try binding to same port again (should fail)
    TcpSocket socket2;
    EXPECT_THROW(socket2.bind(port), NetworkException);
}

// Test socket listening
TEST(TcpSocket, Listening) {
    TcpSocket socket;
    int port = find_available_port();
    
    socket.bind(port);
    EXPECT_NO_THROW(socket.listen());
    EXPECT_NO_THROW(socket.listen(128)); // Custom backlog
}

// Test basic client-server connection
TEST(TcpSocket, BasicConnection) {
    int port = find_available_port();
    
    // Server thread
    std::thread server_thread([port]() {
        TcpSocket server;
        server.bind(port);
        server.listen();
        
        TcpSocket client = server.accept();
        EXPECT_TRUE(client.is_connected());
        
        char buffer[100];
        size_t bytes = client.receive(buffer, sizeof(buffer));
        EXPECT_GT(bytes, 0);
        
        client.send_all(buffer, bytes); // Echo back
    });
    
    // Give server time to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Client
    TcpSocket client;
    EXPECT_NO_THROW(client.connect("127.0.0.1", port));
    EXPECT_TRUE(client.is_connected());
    
    std::string msg = "Hello, World!";
    client.send_all(msg.c_str(), msg.size());
    
    char buffer[100];
    size_t bytes = client.receive(buffer, sizeof(buffer));
    EXPECT_EQ(std::string(buffer, bytes), msg);
    
    server_thread.join();
}

// Test send_all ensures all data is sent
TEST(TcpSocket, SendAll) {
    int port = find_available_port();
    
    std::thread server_thread([port]() {
        TcpSocket server;
        server.bind(port);
        server.listen();
        
        TcpSocket client = server.accept();
        
        // Receive large data
        std::vector<char> buffer(10000);
        size_t total = 0;
        while (total < buffer.size()) {
            size_t bytes = client.receive(
                buffer.data() + total,
                buffer.size() - total
            );
            if (bytes == 0) break;
            total += bytes;
        }
        
        EXPECT_EQ(total, buffer.size());
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    TcpSocket client;
    client.connect("127.0.0.1", port);
    
    // Send large data
    std::vector<char> data(10000, 'X');
    EXPECT_NO_THROW(client.send_all(data.data(), data.size()));
    
    server_thread.join();
}

// Test socket timeout
TEST(TcpSocket, ReceiveTimeout) {
    TcpSocket socket;
    socket.set_receive_timeout(1000); // 1 second
    
    int port = find_available_port();
    socket.bind(port);
    socket.listen();
    
    auto start = std::chrono::steady_clock::now();
    
    // Accept with timeout (should fail after ~1 second)
    EXPECT_THROW({
        TcpSocket client = socket.accept();
    }, NetworkException);
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start
    );
    
    // Should be approximately 1 second
    EXPECT_GT(duration.count(), 900);
    EXPECT_LT(duration.count(), 1500);
}

// Test send timeout
TEST(TcpSocket, SendTimeout) {
    TcpSocket socket;
    socket.set_send_timeout(1000);
    
    // Timeout should be set correctly
    EXPECT_NO_THROW(socket.get_send_timeout());
}

// Test socket options
TEST(TcpSocket, SocketOptions) {
    TcpSocket socket;
    
    // Test SO_REUSEADDR
    EXPECT_NO_THROW(socket.set_reuse_address(true));
    
    // Test SO_KEEPALIVE
    EXPECT_NO_THROW(socket.set_keepalive(true));
    
    // Test TCP_NODELAY (disable Nagle's algorithm)
    EXPECT_NO_THROW(socket.set_no_delay(true));
}

// Test socket shutdown
TEST(TcpSocket, Shutdown) {
    int port = find_available_port();
    
    std::thread server_thread([port]() {
        TcpSocket server;
        server.bind(port);
        server.listen();
        
        TcpSocket client = server.accept();
        
        // Wait for client to shutdown write
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Try to read (should get 0 bytes indicating EOF)
        char buffer[100];
        size_t bytes = client.receive(buffer, sizeof(buffer));
        EXPECT_EQ(bytes, 0);
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    TcpSocket client;
    client.connect("127.0.0.1", port);
    
    // Shutdown write side
    EXPECT_NO_THROW(client.shutdown_write());
    
    server_thread.join();
}

// Test connection to invalid address
TEST(TcpSocket, InvalidConnection) {
    TcpSocket socket;
    
    // Try to connect to non-existent server
    EXPECT_THROW(
        socket.connect("127.0.0.1", 1),
        NetworkException
    );
}

// Test multiple clients
TEST(TcpSocket, MultipleClients) {
    int port = find_available_port();
    const int num_clients = 5;
    
    std::thread server_thread([port, num_clients]() {
        TcpSocket server;
        server.bind(port);
        server.listen(num_clients);
        
        for (int i = 0; i < num_clients; ++i) {
            TcpSocket client = server.accept();
            
            char buffer[100];
            size_t bytes = client.receive(buffer, sizeof(buffer));
            client.send_all(buffer, bytes);
        }
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::vector<std::thread> client_threads;
    for (int i = 0; i < num_clients; ++i) {
        client_threads.emplace_back([port, i]() {
            TcpSocket client;
            client.connect("127.0.0.1", port);
            
            std::string msg = "Client " + std::to_string(i);
            client.send_all(msg.c_str(), msg.size());
            
            char buffer[100];
            size_t bytes = client.receive(buffer, sizeof(buffer));
            EXPECT_EQ(std::string(buffer, bytes), msg);
        });
    }
    
    for (auto& thread : client_threads) {
        thread.join();
    }
    
    server_thread.join();
}

// Test get_peer_address
TEST(TcpSocket, GetPeerAddress) {
    int port = find_available_port();
    
    std::thread server_thread([port]() {
        TcpSocket server;
        server.bind(port);
        server.listen();
        
        TcpSocket client = server.accept();
        
        SocketAddress peer = client.get_peer_address();
        EXPECT_EQ(peer.get_ip(), "127.0.0.1");
        EXPECT_GT(peer.get_port(), 0);
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    TcpSocket client;
    client.connect("127.0.0.1", port);
    
    server_thread.join();
}

// Test get_local_address
TEST(TcpSocket, GetLocalAddress) {
    TcpSocket socket;
    int port = find_available_port();
    
    socket.bind(port);
    
    SocketAddress local = socket.get_local_address();
    EXPECT_EQ(local.get_port(), port);
}

// Test socket close
TEST(TcpSocket, Close) {
    TcpSocket socket;
    int port = find_available_port();
    
    socket.bind(port);
    EXPECT_TRUE(socket.is_valid());
    
    socket.close();
    EXPECT_FALSE(socket.is_valid());
    
    // Double close should be safe
    EXPECT_NO_THROW(socket.close());
}

// Test move semantics
TEST(TcpSocket, MoveSemantics) {
    TcpSocket socket1;
    int port = find_available_port();
    socket1.bind(port);
    
    // Move constructor
    TcpSocket socket2 = std::move(socket1);
    EXPECT_TRUE(socket2.is_valid());
    EXPECT_FALSE(socket1.is_valid());
    
    // Move assignment
    TcpSocket socket3;
    socket3 = std::move(socket2);
    EXPECT_TRUE(socket3.is_valid());
    EXPECT_FALSE(socket2.is_valid());
}

// Test receive with exact size
TEST(TcpSocket, ReceiveExact) {
    int port = find_available_port();
    
    std::thread server_thread([port]() {
        TcpSocket server;
        server.bind(port);
        server.listen();
        
        TcpSocket client = server.accept();
        std::string msg = "Hello123";
        client.send_all(msg.c_str(), msg.size());
    });
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    TcpSocket client;
    client.connect("127.0.0.1", port);
    
    // Receive exactly 8 bytes
    std::vector<char> buffer(8);
    size_t bytes = client.receive_exact(buffer.data(), buffer.size());
    EXPECT_EQ(bytes, 8);
    EXPECT_EQ(std::string(buffer.data(), buffer.size()), "Hello123");
    
    server_thread.join();
}

// Main test entry point
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
