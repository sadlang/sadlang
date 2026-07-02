# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/network.cmake
# الوصف: مكتبات الشبكات / Network Libraries (TCP, UDP, HTTP, WebSocket)
# ═══════════════════════════════════════════════════════════════════════════════

# ──────────────────────────────────────────────────────────────────────
# مكتبة الشبكات الأساسية / Core Network Library
# ──────────────────────────────────────────────────────────────────────
set(NETWORK_SOURCES
    features/network/core/src/network/network_error.cpp
    features/network/core/src/network/network_bindings_new.cpp
    features/network/core/src/socket/socket_address.cpp
    features/network/core/src/socket/socket_base.cpp
    features/network/core/src/tcp/tcp_socket.cpp
    features/network/core/src/udp/udp_socket.cpp
)

add_library(sad_network STATIC ${NETWORK_SOURCES})

target_include_directories(sad_network PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/network
    ${CMAKE_SOURCE_DIR}/include/network/http
    ${CMAKE_SOURCE_DIR}/features/network/core
    ${CMAKE_SOURCE_DIR}/features/network/core/include
    ${CMAKE_SOURCE_DIR}/features/network/core/include/network
    ${CMAKE_SOURCE_DIR}/features/network/core/include/socket
    ${CMAKE_SOURCE_DIR}/features/network/core/include/tcp
    ${CMAKE_SOURCE_DIR}/features/network/core/include/udp
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
    features/network/core/src/http/http_base.cpp
    features/network/core/src/http/http_request.cpp
    features/network/core/src/http/http_response.cpp
    features/network/core/src/http/http_url.cpp
    features/network/core/src/http/http_cookie.cpp
    features/network/core/src/http/http_client.cpp
    features/network/core/src/http/http_server.cpp
    features/network/core/src/http/http_bindings.cpp
)

add_library(sad_http STATIC ${HTTP_SOURCES})

target_include_directories(sad_http PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/features/network/core
    ${CMAKE_SOURCE_DIR}/features/network/core/include
    ${CMAKE_SOURCE_DIR}/features/network/core/include/http
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
    features/network/core/src/websocket/websocket_client.cpp
    features/network/core/src/websocket/websocket_server.cpp
    features/network/core/src/websocket/websocket_bindings.cpp
)

add_library(sad_websocket STATIC ${WEBSOCKET_SOURCES})

target_include_directories(sad_websocket PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/features/network/core
    ${CMAKE_SOURCE_DIR}/features/network/core/include
    ${CMAKE_SOURCE_DIR}/features/network/core/include/websocket
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
