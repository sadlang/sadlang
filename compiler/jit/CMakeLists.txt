# ============================================================================
# CMakeLists.txt - JIT Engine Build Configuration
# ============================================================================

cmake_minimum_required(VERSION 3.15)

# JIT Library
add_library(sad_jit STATIC
    src/jit_cache.cpp
    src/hot_path_detector.cpp
    src/jit_engine.cpp
    src/jit_bridge.cpp
)

target_include_directories(sad_jit PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(sad_jit PUBLIC cxx_std_17)

# Enable threading
find_package(Threads REQUIRED)
target_link_libraries(sad_jit PUBLIC Threads::Threads)

# Tests
if(BUILD_TESTS)
    # JIT Cache Tests
    add_executable(test_jit_cache
        tests/test_jit_cache.cpp
    )
    target_link_libraries(test_jit_cache PRIVATE sad_jit)
    add_test(NAME JITCacheTests COMMAND test_jit_cache)
    
    # JIT Bridge Tests
    add_executable(test_jit_bridge
        tests/test_jit_bridge.cpp
    )
    target_link_libraries(test_jit_bridge PRIVATE sad_jit)
    add_test(NAME JITBridgeTests COMMAND test_jit_bridge)
endif()
