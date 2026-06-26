# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/network.cmake
# الوصف: مكتبات الشبكات / Network Libraries (TCP, UDP, HTTP, WebSocket)
# ═══════════════════════════════════════════════════════════════════════════════

# ──────────────────────────────────────────────────────────────────────
# مكتبة الشبكات الأساسية / Core Network Library
# ──────────────────────────────────────────────────────────────────────
set(NETWORK_SOURCES
    stdlib/network/src/network/network_error.cpp
    stdlib/network/src/network/network_bindings_new.cpp
    stdlib/network/src/socket/socket_address.cpp
    stdlib/network/src/socket/socket_base.cpp
    stdlib/network/src/tcp/tcp_socket.cpp
    stdlib/network/src/udp/udp_socket.cpp
)

add_library(sad_network STATIC ${NETWORK_SOURCES})

target_include_directories(sad_network PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/network
    ${CMAKE_SOURCE_DIR}/include/network/http
    ${CMAKE_SOURCE_DIR}/stdlib/network
    ${CMAKE_SOURCE_DIR}/stdlib/network/include
    ${CMAKE_SOURCE_DIR}/stdlib/network/include/network
    ${CMAKE_SOURCE_DIR}/stdlib/network/include/socket
    ${CMAKE_SOURCE_DIR}/stdlib/network/include/tcp
    ${CMAKE_SOURCE_DIR}/stdlib/network/include/udp
)

target_compile_features(sad_network PUBLIC cxx_std_17)

if(WIN32)
    target_link_libraries(sad_network PUBLIC ws2_32)
    target_compile_definitions(sad_network PRIVATE _WIN32_WINNT=0x0601)
endif()

set_target_properties(sad_network PROPERTIES
    OUTPUT_NAME "sad_network"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

message(STATUS "✓ الشبكات / Network: TCP + UDP + IPv4/IPv6")

# ──────────────────────────────────────────────────────────────────────
# مكتبة HTTP / HTTP Library
# ──────────────────────────────────────────────────────────────────────
set(HTTP_SOURCES
    stdlib/network/src/http/http_base.cpp
    stdlib/network/src/http/http_request.cpp
    stdlib/network/src/http/http_response.cpp
    stdlib/network/src/http/http_url.cpp
    stdlib/network/src/http/http_cookie.cpp
    stdlib/network/src/http/http_client.cpp
    stdlib/network/src/http/http_server.cpp
    stdlib/network/src/http/http_bindings.cpp
)

add_library(sad_http STATIC ${HTTP_SOURCES})

target_include_directories(sad_http PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/stdlib/network
    ${CMAKE_SOURCE_DIR}/stdlib/network/include
    ${CMAKE_SOURCE_DIR}/stdlib/network/include/http
)

target_link_libraries(sad_http PUBLIC sad_network)
target_compile_features(sad_http PUBLIC cxx_std_17)

if(TARGET sad_core)
    target_link_libraries(sad_interp PUBLIC sad_network sad_http)
    target_compile_definitions(sad_interp PRIVATE HAS_NETWORK_LIB)
    message(STATUS "✓ دعم الشبكة HTTP بالمفسر / Enabled HTTP network support in interpreter")
endif()

set_target_properties(sad_http PROPERTIES
    OUTPUT_NAME "sad_http"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

message(STATUS "✓ HTTP: HTTP/1.0 + HTTP/1.1 + Cookies")

# ──────────────────────────────────────────────────────────────────────
# مكتبة WebSocket / WebSocket Library
# ──────────────────────────────────────────────────────────────────────
set(WEBSOCKET_SOURCES
    stdlib/network/src/websocket/websocket_client.cpp
    stdlib/network/src/websocket/websocket_server.cpp
    stdlib/network/src/websocket/websocket_bindings.cpp
)

add_library(sad_websocket STATIC ${WEBSOCKET_SOURCES})

target_include_directories(sad_websocket PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/stdlib/network
    ${CMAKE_SOURCE_DIR}/stdlib/network/include
    ${CMAKE_SOURCE_DIR}/stdlib/network/include/websocket
)

target_link_libraries(sad_websocket PUBLIC sad_network)
target_compile_features(sad_websocket PUBLIC cxx_std_17)

if(WIN32)
    target_link_libraries(sad_websocket PUBLIC ws2_32)
endif()

set_target_properties(sad_websocket PROPERTIES
    OUTPUT_NAME "sad_websocket"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

message(STATUS "✓ WebSocket: Client + Server")

# (AR) ربط sad_websocket بـ sad_core (يجب أن يتم هنا لأن libraries.cmake
#      تُحمَّل قبل network.cmake، فالشرط `if(TARGET sad_websocket)` هناك
#      يكون false دائماً ولا يُربط شيء).
# (EN) Link sad_websocket into sad_core here. libraries.cmake runs before
#      network.cmake so its `if(TARGET sad_websocket)` guard is always false.
if(TARGET sad_core)
    target_link_libraries(sad_interp PUBLIC sad_websocket)
    target_compile_definitions(sad_interp PRIVATE HAS_WEBSOCKET_LIB)
    message(STATUS "✓ ربط WebSocket بالمفسر / Linked WebSocket to interpreter")
endif()
