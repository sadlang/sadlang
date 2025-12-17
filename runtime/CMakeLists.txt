# ========================================
# SadLanguage Runtime Library Build
# المكتبة التنفيذية للغة سد
# ========================================

cmake_minimum_required(VERSION 3.15)

# مشروع المكتبة التنفيذية / Runtime Library Project
project(sad_runtime 
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "SadLanguage Runtime Library"
)

# معايير C++17 / C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ========================================
# Source Files - ملفات المصدر
# ========================================

set(RUNTIME_SOURCES
    src/runtime.cpp
    src/runtime_io.cpp
    src/runtime_math.cpp
    src/runtime_string.cpp
)

set(RUNTIME_HEADERS
    include/runtime.h
)

# ========================================
# Runtime Library - مكتبة التنفيذ
# ========================================

add_library(sad_runtime STATIC
    ${RUNTIME_SOURCES}
    ${RUNTIME_HEADERS}
)

# مسارات التضمين / Include Directories
target_include_directories(sad_runtime
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../vm/include
)

# خيارات التحويل البرمجي / Compiler Options
if(MSVC)
    target_compile_options(sad_runtime PRIVATE
        /W4          # تحذيرات المستوى 4 / Warning level 4
        /WX          # عامل التحذيرات كأخطاء / Treat warnings as errors
        /permissive- # الوضع المتوافق / Conformance mode
        /Zc:__cplusplus
    )
else()
    target_compile_options(sad_runtime PRIVATE
        -Wall
        -Wextra
        -Werror
        -pedantic
    )
endif()

# ========================================
# Platform-Specific Settings
# ========================================

if(WIN32)
    # إعدادات Windows
    target_compile_definitions(sad_runtime PRIVATE
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    )
endif()

# ========================================
# Installation - التثبيت
# ========================================

install(TARGETS sad_runtime
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${RUNTIME_HEADERS}
    DESTINATION include/sad
)

# ========================================
# Summary - الملخص
# ========================================

message(STATUS "========================================")
message(STATUS "SadLanguage Runtime Library")
message(STATUS "المكتبة التنفيذية للغة سد")
message(STATUS "========================================")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "========================================")
message(STATUS "Components:")
message(STATUS "  - I/O Functions (14 functions)")
message(STATUS "  - Math Library (12 functions)")
message(STATUS "  - String Library (10 functions)")
message(STATUS "  - Memory Operations (5 functions)")
message(STATUS "  - Stage 1 Compatibility (6 functions)")
message(STATUS "  - Array Utilities (6 functions)")
message(STATUS "  - System Functions (4 functions)")
message(STATUS "  Total: 57 runtime functions")
message(STATUS "========================================")
