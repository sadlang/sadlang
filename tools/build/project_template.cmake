# بسم الله الرحمن الرحيم
# CMakeLists.txt Template for Sad Language Projects
# قالب CMakeLists.txt لمشاريع لغة Sad
# Phase 7.3: Build System Integration

#[=======================================================================[
This is a template CMakeLists.txt file for Sad language projects.
Copy this file to your project root and customize as needed.

هذا قالب ملف CMakeLists.txt لمشاريع لغة Sad.
انسخ هذا الملف إلى جذر مشروعك وخصصه حسب الحاجة.
#]=======================================================================]

cmake_minimum_required(VERSION 3.15)

# ============================================================================
# الحمد لله - Project Setup / إعداد المشروع
# ============================================================================

project(MyProject
    VERSION 1.0.0
    DESCRIPTION "My Sad Language Project"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# Find Sad Compiler / البحث عن مترجم Sad
# ============================================================================

# Add Sad CMake modules to path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Find Sad compiler
find_package(Sad REQUIRED)

if(NOT Sad_FOUND)
    message(FATAL_ERROR "Sad compiler not found! / مترجم Sad غير موجود!")
endif()

# ============================================================================
# Project Configuration / إعدادات المشروع
# ============================================================================

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Source directories
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# Build options
option(BUILD_TESTS "Build tests / بناء الاختبارات" ON)
option(BUILD_DOCS "Build documentation / بناء التوثيق" OFF)
option(ENABLE_DEBUG "Enable debug info / تفعيل معلومات التنقيح" OFF)
option(OPTIMIZE_SIZE "Optimize for size / تحسين الحجم" OFF)

# ============================================================================
# Compilation Settings / إعدادات الترجمة
# ============================================================================

# Optimization level
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SAD_OPTIMIZATION O0)
    set(SAD_DEBUG_FLAG DEBUG)
elseif(OPTIMIZE_SIZE)
    set(SAD_OPTIMIZATION Os)
else()
    set(SAD_OPTIMIZATION O3)
endif()

# ============================================================================
# Source Files / ملفات المصدر
# ============================================================================

# Collect all .s files
file(GLOB_RECURSE SAD_SOURCES
    ${SRC_DIR}/*.s
    ${SRC_DIR}/*.sad
)

message(STATUS "Found ${CMAKE_CURRENT_LIST_LINE} Sad source file(s)")

# ============================================================================
# الحمد لله - Build Targets / أهداف البناء
# ============================================================================

# Main executable
add_sad_executable(myprogram
    SOURCES ${SAD_SOURCES}
    OUTPUT myprogram
    OPTIMIZATION ${SAD_OPTIMIZATION}
    ${SAD_DEBUG_FLAG}
    VERBOSE
)

# Example: Add a library
# add_sad_library(mylib
#     SOURCES ${SRC_DIR}/lib.s
#     TYPE SHARED
#     OPTIMIZATION ${SAD_OPTIMIZATION}
# )

# ============================================================================
# Testing / الاختبارات
# ============================================================================

if(BUILD_TESTS)
    enable_testing()
    
    # Find all test files
    file(GLOB TEST_SOURCES ${TEST_DIR}/*.s)
    
    foreach(TEST_SOURCE ${TEST_SOURCES})
        # Get test name from filename
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
        
        # Add test
        sad_add_test(${TEST_NAME}
            SOURCE ${TEST_SOURCE}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    endforeach()
    
    message(STATUS "الحمد لله - Testing enabled with ${CMAKE_CURRENT_LIST_LINE} test(s)")
endif()

# ============================================================================
# Documentation / التوثيق
# ============================================================================

if(BUILD_DOCS)
    # Add documentation generation
    add_custom_target(docs
        COMMAND ${CMAKE_COMMAND} -E echo "الحمد لله - Generating documentation..."
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/docs
        COMMENT "Generating project documentation"
    )
    
    message(STATUS "Documentation target added: make docs")
endif()

# ============================================================================
# Installation / التثبيت
# ============================================================================

# Install executable
install(
    FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/myprogram
    DESTINATION bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                GROUP_READ GROUP_EXECUTE
                WORLD_READ WORLD_EXECUTE
)

# Install libraries (if any)
# install(
#     FILES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libmylib.so
#     DESTINATION lib
# )

# ============================================================================
# Print Configuration / طباعة الإعدادات
# ============================================================================

message(STATUS "═══════════════════════════════════════════════")
message(STATUS "الحمد لله - Sad Project Configuration")
message(STATUS "═══════════════════════════════════════════════")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Sad Compiler: ${SADC_EXECUTABLE}")
message(STATUS "Optimization: -${SAD_OPTIMIZATION}")
message(STATUS "Debug Info: ${ENABLE_DEBUG}")
message(STATUS "Build Tests: ${BUILD_TESTS}")
message(STATUS "Build Docs: ${BUILD_DOCS}")
message(STATUS "Output Directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "═══════════════════════════════════════════════")

# ============================================================================
# الحمد لله - CMakeLists.txt complete / الملف مكتمل
# ============================================================================
