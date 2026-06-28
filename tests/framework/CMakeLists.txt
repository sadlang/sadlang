# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# CMakeLists.txt — إطار اختبارات لغة ص الموحد
# Sad Language Unified Test Framework
# ═══════════════════════════════════════════════════════════════════════════════

cmake_minimum_required(VERSION 3.15)

message(STATUS "════════════════════════════════════════════════════════")
message(STATUS "   إعداد إطار الاختبارات / Configuring Test Framework")
message(STATUS "════════════════════════════════════════════════════════")

# ═══════════════════════════════════════════════════════════════════════════════
# مكتبة إطار الاختبارات (Header-Only)
# ═══════════════════════════════════════════════════════════════════════════════
add_library(sad_test_framework INTERFACE)

target_include_directories(sad_test_framework INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(sad_test_framework INTERFACE cxx_std_17)

# ═══════════════════════════════════════════════════════════════════════════════
# مكتبة إطار قياس الأداء (Header-Only)
# ═══════════════════════════════════════════════════════════════════════════════
add_library(sad_benchmark_framework INTERFACE)

target_include_directories(sad_benchmark_framework INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(sad_benchmark_framework INTERFACE cxx_std_17)

# Windows-specific: ربط psapi لقياس الذاكرة
if(WIN32)
    target_link_libraries(sad_benchmark_framework INTERFACE psapi)
endif()

# ═══════════════════════════════════════════════════════════════════════════════
# اختبار ذاتي للإطار / Self-test
# ═══════════════════════════════════════════════════════════════════════════════
if(BUILD_TESTS)
    add_executable(sad_test_framework_selftest
        selftest.cpp
    )

    target_link_libraries(sad_test_framework_selftest PRIVATE sad_test_framework)

    # تسجيل في CTest
    add_test(NAME FrameworkSelfTest COMMAND sad_test_framework_selftest)

    message(STATUS "✓ اختبار الإطار الذاتي مُضاف / Framework self-test added")
endif()

# ═══════════════════════════════════════════════════════════════════════════════
# مثال قياس الأداء / Benchmark Example
# ═══════════════════════════════════════════════════════════════════════════════
if(BUILD_TESTS)
    add_executable(sad_benchmark_example
        benchmark_example.cpp
    )

    target_link_libraries(sad_benchmark_example PRIVATE sad_benchmark_framework)

    message(STATUS "✓ مثال قياس الأداء مُضاف / Benchmark example added")
endif()

message(STATUS "✓ إطار الاختبارات جاهز / Test framework ready")
message(STATUS "  - اختبارات: ${CMAKE_CURRENT_SOURCE_DIR}/sad_test.h")
message(STATUS "  - قياس أداء: ${CMAKE_CURRENT_SOURCE_DIR}/sad_benchmark.h")
