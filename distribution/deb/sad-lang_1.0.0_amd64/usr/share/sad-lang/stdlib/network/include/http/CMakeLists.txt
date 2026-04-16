# بسم الله الرحمن الرحيم
# CMake configuration for HTTP library

cmake_minimum_required(VERSION 3.15)

# HTTP Library Sources
set(HTTP_SOURCES
    stdlib/network/http/http_base.cpp
    stdlib/network/http/http_request.cpp
    stdlib/network/http/http_response.cpp
    stdlib/network/http/http_url.cpp
    stdlib/network/http/http_cookie.cpp
    stdlib/network/http/http_client.cpp
    stdlib/network/http/http_server.cpp
    stdlib/network/http_bindings.cpp
)

# HTTP Library Headers
set(HTTP_HEADERS
    include/network/http/http_base.h
    include/network/http/http_request.h
    include/network/http/http_response.h
    include/network/http/http_url.h
    include/network/http/http_cookie.h
    include/network/http/http_client.h
    include/network/http/http_server.h
    include/network/http_bindings.h
)

# Create HTTP library
add_library(sad_http STATIC ${HTTP_SOURCES} ${HTTP_HEADERS})

# Include directories
target_include_directories(sad_http PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Link with network library (TCP/UDP sockets)
target_link_libraries(sad_http PUBLIC
    sad_network
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(sad_http PUBLIC ws2_32)
endif()

# Set output directory
set_target_properties(sad_http PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>
)

# Install headers
install(DIRECTORY include/network/http
    DESTINATION include/network
    FILES_MATCHING PATTERN "*.h"
)

# Install library
install(TARGETS sad_http
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
