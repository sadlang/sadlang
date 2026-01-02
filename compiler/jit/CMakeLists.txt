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
    # TODO: إضافة هذه الملفات عند إكمالها
    # TODO: Add these files when completed
    # src/jit_profiler.cpp
    # src/jit_metrics.cpp
    src/jit_hash.cpp
)

# LLVM Backend Integration (Phase 1.2)
if(ENABLE_LLVM_BACKEND AND LLVM_FOUND)
    target_sources(sad_jit PRIVATE
        ../backend/llvm/src/llvm_generator.cpp
        ../backend/llvm/src/llvm_context.cpp
    )
    
    target_include_directories(sad_jit PRIVATE
        ../backend/llvm/include
        ${LLVM_INCLUDE_DIRS}
    )
    
    target_link_libraries(sad_jit PRIVATE
        ${LLVM_LIBS}
    )
    
    target_compile_definitions(sad_jit PRIVATE
        ENABLE_LLVM_BACKEND
    )
    
    message(STATUS "✅ JIT Engine: LLVM Backend enabled")
else()
    message(STATUS "⚠️  JIT Engine: LLVM Backend disabled (using simulation)")
endif()

target_include_directories(sad_jit PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
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
    
    # JIT Profiler Tests
    add_executable(test_jit_profiler
        tests/test_jit_profiler.cpp
    )
    target_link_libraries(test_jit_profiler PRIVATE sad_jit)
    add_test(NAME JITProfilerTests COMMAND test_jit_profiler)
    
    # JIT Metrics Tests
    add_executable(test_jit_metrics
        tests/test_jit_metrics.cpp
    )
    target_link_libraries(test_jit_metrics PRIVATE sad_jit)
    add_test(NAME JITMetricsTests COMMAND test_jit_metrics)
    
    # JIT Hash Tests
    add_executable(test_jit_hash
        tests/test_jit_hash.cpp
    )
    target_link_libraries(test_jit_hash PRIVATE sad_jit)
    add_test(NAME JITHashTests COMMAND test_jit_hash)
endif()
