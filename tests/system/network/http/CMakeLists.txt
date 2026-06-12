# بسم الله الرحمن الرحيم
# CMake configuration for HTTP tests

cmake_minimum_required(VERSION 3.15)

# Google Test (already fetched via FetchContent in parent)
# If not available, try find_package as fallback
if(NOT TARGET GTest::gtest)
    find_package(GTest QUIET)
    if(NOT GTest_FOUND)
        message(STATUS "⚠ HTTP network tests skipped (GTest not available)")
        return()
    endif()
endif()

# HTTP Test Sources
set(HTTP_TEST_SOURCES
    test_http_base.cpp
    test_http_url_fixed.cpp
    test_http_cookie_fixed.cpp
    test_http_request_fixed.cpp
    test_http_response_fixed.cpp
)

# Create test executable
add_executable(http_network_tests ${HTTP_TEST_SOURCES})

# Include directories
target_include_directories(http_network_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(http_network_tests PRIVATE
    sad_http
    sad_network
    GTest::gtest
    GTest::gtest_main
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(http_network_tests PRIVATE ws2_32)
endif()

# Add to test suite
add_test(NAME HTTPNetworkTests COMMAND http_network_tests)

# Set output directory
set_target_properties(http_network_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
)

message(STATUS "✅ HTTP tests configured")
