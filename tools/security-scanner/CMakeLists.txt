# ============================================================================
# tools/security-scanner/CMakeLists.txt
# (AR) أداة sad-guard: ماسح أمني ساكن للغة ص.
# (EN) sad-guard: Sad's static security scanner.
# ============================================================================
#
# (AR) البنية:
#   - sad_security_scanner_lib (STATIC): منطق الماسح وتقارير JSON/HTML/SARIF
#       يُستخدم من قِبل ملف الاختبار test_security_scanner ومن sad-guard.
#   - sad-guard (EXE): واجهة سطر أوامر رفيعة تستدعي Sad::Security::guard_main.
# (EN) Layout:
#   - sad_security_scanner_lib (STATIC): scanner core + report formatters.
#       Consumed by test_security_scanner unit test and by sad-guard.
#   - sad-guard (EXE): tiny CLI shim invoking Sad::Security::guard_main.
# ============================================================================

# ----------------------------------------------------------------------------
# (AR) المكتبة: منطق الماسح
# (EN) Library: scanner core
# ----------------------------------------------------------------------------
add_library(sad_security_scanner_lib STATIC
    src/security_scanner.cpp
    src/security_scanner_analysis.cpp
)

target_include_directories(sad_security_scanner_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# (AR) الاعتماد على الطبقة الأمنية المشتركة (BoundsChecker/InputSanitizer/...)
# (EN) Depend on the shared security primitives (BoundsChecker/InputSanitizer/...)
target_link_libraries(sad_security_scanner_lib
    PUBLIC
        sad_security_core
)

if(MSVC)
    target_compile_options(sad_security_scanner_lib PRIVATE /utf-8 /FS /wd4819)
endif()

target_compile_features(sad_security_scanner_lib PUBLIC cxx_std_17)

# ----------------------------------------------------------------------------
# (AR) التنفيذي: sad-guard
# (EN) Executable: sad-guard
# ----------------------------------------------------------------------------
add_executable(sad-guard src/main.cpp)

target_link_libraries(sad-guard PRIVATE sad_security_scanner_lib)

set_target_properties(sad-guard PROPERTIES
    OUTPUT_NAME              "sad-guard"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(MSVC)
    target_compile_options(sad-guard PRIVATE /utf-8 /FS /wd4819)
endif()

# ----------------------------------------------------------------------------
# (AR) اختبار الوحدة (يُبنى فقط إذا فُعّلت الاختبارات)
# (EN) Unit test (built only when tests are enabled)
# ----------------------------------------------------------------------------
if(BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_security_scanner.cpp")
    add_executable(test_security_scanner tests/test_security_scanner.cpp)
    target_link_libraries(test_security_scanner PRIVATE sad_security_scanner_lib)
    set_target_properties(test_security_scanner PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
    )
    if(MSVC)
        target_compile_options(test_security_scanner PRIVATE /utf-8 /FS /wd4819)
    endif()
endif()

message(STATUS "Configured sad-guard (security scanner)")
