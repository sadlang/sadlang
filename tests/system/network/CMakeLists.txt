# بسم الله الرحمن الرحيم
# Network Tests CMakeLists.txt

cmake_minimum_required(VERSION 3.15)

# Find Google Test
find_package(GTest REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${GTEST_INCLUDE_DIRS}
)

# Network Error Tests
add_executable(test_network_error test_network_error.cpp)
target_link_libraries(test_network_error
    PRIVATE
        sad_network
        GTest::GTest
        GTest::Main
)

# Socket Address Tests
add_executable(test_socket_address test_socket_address.cpp)
target_link_libraries(test_socket_address
    PRIVATE
        sad_network
        GTest::GTest
        GTest::Main
)

# TCP Socket Tests
add_executable(test_tcp_socket test_tcp_socket.cpp)
target_link_libraries(test_tcp_socket
    PRIVATE
        sad_network
        GTest::GTest
        GTest::Main
)

# UDP Socket Tests
add_executable(test_udp_socket test_udp_socket.cpp)
target_link_libraries(test_udp_socket
    PRIVATE
        sad_network
        GTest::GTest
        GTest::Main
)

# Add tests to CTest
enable_testing()

add_test(NAME NetworkError COMMAND test_network_error)
add_test(NAME SocketAddress COMMAND test_socket_address)
add_test(NAME TcpSocket COMMAND test_tcp_socket)
add_test(NAME UdpSocket COMMAND test_udp_socket)

# Set test properties
set_tests_properties(NetworkError PROPERTIES TIMEOUT 30)
set_tests_properties(SocketAddress PROPERTIES TIMEOUT 30)
set_tests_properties(TcpSocket PROPERTIES TIMEOUT 60)
set_tests_properties(UdpSocket PROPERTIES TIMEOUT 60)

# Copy test data if needed
# file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_data
#      DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

message(STATUS "Network tests configured successfully")
