// بسم الله الرحمن الرحيم
// test_socket_address.cpp - Unit tests for SocketAddress class

#include <gtest/gtest.h>
#include "network/socket_address.h"
#include "network/network_error.h"

using namespace sad::network;

// Test IPv4 address construction
TEST(SocketAddress, IPv4Construction) {
    SocketAddress addr("192.168.1.1", 8080);
    
    EXPECT_EQ(addr.get_ip(), "192.168.1.1");
    EXPECT_EQ(addr.get_port(), 8080);
    EXPECT_TRUE(addr.is_ipv4());
    EXPECT_FALSE(addr.is_ipv6());
}

// Test IPv6 address construction
TEST(SocketAddress, IPv6Construction) {
    SocketAddress addr("::1", 9090);
    
    EXPECT_EQ(addr.get_ip(), "::1");
    EXPECT_EQ(addr.get_port(), 9090);
    EXPECT_FALSE(addr.is_ipv4());
    EXPECT_TRUE(addr.is_ipv6());
}

// Test localhost construction
TEST(SocketAddress, LocalhostConstruction) {
    SocketAddress addr("localhost", 8080);
    
    // localhost should resolve to 127.0.0.1 or ::1
    std::string ip = addr.get_ip();
    EXPECT_TRUE(ip == "127.0.0.1" || ip == "::1");
    EXPECT_EQ(addr.get_port(), 8080);
}

// Test wildcard address
TEST(SocketAddress, WildcardAddress) {
    SocketAddress addr4(8080); // IPv4 wildcard
    EXPECT_EQ(addr4.get_ip(), "0.0.0.0");
    EXPECT_EQ(addr4.get_port(), 8080);
    EXPECT_TRUE(addr4.is_ipv4());
}

// Test invalid IPv4 address
TEST(SocketAddress, InvalidIPv4) {
    EXPECT_THROW(
        SocketAddress("999.999.999.999", 8080),
        NetworkException
    );
    
    EXPECT_THROW(
        SocketAddress("192.168.1", 8080),
        NetworkException
    );
    
    EXPECT_THROW(
        SocketAddress("not-an-ip", 8080),
        NetworkException
    );
}

// Test invalid port numbers
TEST(SocketAddress, InvalidPort) {
    EXPECT_THROW(
        SocketAddress("192.168.1.1", 0),
        NetworkException
    );
    
    EXPECT_THROW(
        SocketAddress("192.168.1.1", 65536),
        NetworkException
    );
    
    EXPECT_THROW(
        SocketAddress("192.168.1.1", -1),
        NetworkException
    );
}

// Test IPv4 validation
TEST(SocketAddress, IPv4Validation) {
    EXPECT_TRUE(SocketAddress::is_valid_ipv4("192.168.1.1"));
    EXPECT_TRUE(SocketAddress::is_valid_ipv4("0.0.0.0"));
    EXPECT_TRUE(SocketAddress::is_valid_ipv4("255.255.255.255"));
    EXPECT_TRUE(SocketAddress::is_valid_ipv4("127.0.0.1"));
    
    EXPECT_FALSE(SocketAddress::is_valid_ipv4("256.1.1.1"));
    EXPECT_FALSE(SocketAddress::is_valid_ipv4("192.168.1"));
    EXPECT_FALSE(SocketAddress::is_valid_ipv4("192.168.1.1.1"));
    EXPECT_FALSE(SocketAddress::is_valid_ipv4("abc.def.ghi.jkl"));
    EXPECT_FALSE(SocketAddress::is_valid_ipv4(""));
}

// Test IPv6 validation
TEST(SocketAddress, IPv6Validation) {
    EXPECT_TRUE(SocketAddress::is_valid_ipv6("::1"));
    EXPECT_TRUE(SocketAddress::is_valid_ipv6("::"));
    EXPECT_TRUE(SocketAddress::is_valid_ipv6("2001:db8::1"));
    EXPECT_TRUE(SocketAddress::is_valid_ipv6("fe80::1"));
    EXPECT_TRUE(SocketAddress::is_valid_ipv6("2001:0db8:0000:0000:0000:0000:0000:0001"));
    
    EXPECT_FALSE(SocketAddress::is_valid_ipv6("192.168.1.1"));
    EXPECT_FALSE(SocketAddress::is_valid_ipv6("gggg::1"));
    EXPECT_FALSE(SocketAddress::is_valid_ipv6(""));
}

// Test port validation
TEST(SocketAddress, PortValidation) {
    EXPECT_TRUE(SocketAddress::is_valid_port(80));
    EXPECT_TRUE(SocketAddress::is_valid_port(8080));
    EXPECT_TRUE(SocketAddress::is_valid_port(65535));
    EXPECT_TRUE(SocketAddress::is_valid_port(1));
    
    EXPECT_FALSE(SocketAddress::is_valid_port(0));
    EXPECT_FALSE(SocketAddress::is_valid_port(-1));
    EXPECT_FALSE(SocketAddress::is_valid_port(65536));
    EXPECT_FALSE(SocketAddress::is_valid_port(100000));
}

// Test to_string conversion
TEST(SocketAddress, ToString) {
    SocketAddress addr4("192.168.1.1", 8080);
    EXPECT_EQ(addr4.to_string(), "192.168.1.1:8080");
    
    SocketAddress addr6("::1", 9090);
    EXPECT_EQ(addr6.to_string(), "[::1]:9090");
}

// Test from_string parsing
TEST(SocketAddress, FromString) {
    SocketAddress addr1 = SocketAddress::from_string("192.168.1.1:8080");
    EXPECT_EQ(addr1.get_ip(), "192.168.1.1");
    EXPECT_EQ(addr1.get_port(), 8080);
    
    SocketAddress addr2 = SocketAddress::from_string("[::1]:9090");
    EXPECT_EQ(addr2.get_ip(), "::1");
    EXPECT_EQ(addr2.get_port(), 9090);
    
    EXPECT_THROW(
        SocketAddress::from_string("invalid"),
        NetworkException
    );
    
    EXPECT_THROW(
        SocketAddress::from_string("192.168.1.1"),
        NetworkException
    );
}

// Test sockaddr conversion
TEST(SocketAddress, SockaddrConversion) {
    SocketAddress addr("192.168.1.100", 8080);
    
    const sockaddr* sa = addr.get_sockaddr();
    EXPECT_NE(sa, nullptr);
    
    socklen_t len = addr.get_sockaddr_len();
    EXPECT_GT(len, 0);
    
    // For IPv4, length should be sizeof(sockaddr_in)
    if (addr.is_ipv4()) {
        EXPECT_EQ(len, sizeof(sockaddr_in));
    }
}

// Test copy constructor
TEST(SocketAddress, CopyConstructor) {
    SocketAddress addr1("10.0.0.1", 3000);
    SocketAddress addr2 = addr1;
    
    EXPECT_EQ(addr2.get_ip(), addr1.get_ip());
    EXPECT_EQ(addr2.get_port(), addr1.get_port());
    EXPECT_EQ(addr2.is_ipv4(), addr1.is_ipv4());
}

// Test assignment operator
TEST(SocketAddress, AssignmentOperator) {
    SocketAddress addr1("10.0.0.1", 3000);
    SocketAddress addr2("192.168.1.1", 8080);
    
    addr2 = addr1;
    
    EXPECT_EQ(addr2.get_ip(), addr1.get_ip());
    EXPECT_EQ(addr2.get_port(), addr1.get_port());
}

// Test move constructor
TEST(SocketAddress, MoveConstructor) {
    SocketAddress addr1("10.0.0.1", 3000);
    SocketAddress addr2 = std::move(addr1);
    
    EXPECT_EQ(addr2.get_ip(), "10.0.0.1");
    EXPECT_EQ(addr2.get_port(), 3000);
}

// Test equality comparison
TEST(SocketAddress, EqualityComparison) {
    SocketAddress addr1("192.168.1.1", 8080);
    SocketAddress addr2("192.168.1.1", 8080);
    SocketAddress addr3("192.168.1.2", 8080);
    SocketAddress addr4("192.168.1.1", 8081);
    
    EXPECT_TRUE(addr1 == addr2);
    EXPECT_FALSE(addr1 == addr3);
    EXPECT_FALSE(addr1 == addr4);
    
    EXPECT_FALSE(addr1 != addr2);
    EXPECT_TRUE(addr1 != addr3);
    EXPECT_TRUE(addr1 != addr4);
}

// Test address family detection
TEST(SocketAddress, AddressFamily) {
    SocketAddress addr4("192.168.1.1", 8080);
    EXPECT_EQ(addr4.get_family(), AF_INET);
    
    SocketAddress addr6("::1", 9090);
    EXPECT_EQ(addr6.get_family(), AF_INET6);
}

// Test special addresses
TEST(SocketAddress, SpecialAddresses) {
    // Loopback
    SocketAddress loopback4("127.0.0.1", 8080);
    EXPECT_TRUE(loopback4.is_loopback());
    
    SocketAddress loopback6("::1", 8080);
    EXPECT_TRUE(loopback6.is_loopback());
    
    // Any/wildcard
    SocketAddress any4("0.0.0.0", 8080);
    EXPECT_TRUE(any4.is_any());
    
    SocketAddress any6("::", 8080);
    EXPECT_TRUE(any6.is_any());
}

// Test multicast addresses
TEST(SocketAddress, MulticastAddresses) {
    SocketAddress mcast4("224.0.0.1", 8080);
    EXPECT_TRUE(mcast4.is_multicast());
    
    SocketAddress mcast6("ff02::1", 8080);
    EXPECT_TRUE(mcast6.is_multicast());
    
    SocketAddress normal("192.168.1.1", 8080);
    EXPECT_FALSE(normal.is_multicast());
}

// Test broadcast address
TEST(SocketAddress, BroadcastAddress) {
    SocketAddress bcast("255.255.255.255", 8080);
    EXPECT_TRUE(bcast.is_broadcast());
    
    SocketAddress normal("192.168.1.1", 8080);
    EXPECT_FALSE(normal.is_broadcast());
}

// Main test entry point
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
