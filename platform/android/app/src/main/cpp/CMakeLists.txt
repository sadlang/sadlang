# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
#  CMakeLists.txt لبناء مكتبة لغة ص الأصلية على Android
#  Sad Language Native Library for Android
# ═══════════════════════════════════════════════════════════════════════════════

cmake_minimum_required(VERSION 3.22.1)

project("sadnotes" VERSION 1.0.0 LANGUAGES CXX C)

# ─────────────────────────────────────────────────────────────────────
# إعدادات C++17
# ─────────────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# دعم UTF-8
add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)

# ─────────────────────────────────────────────────────────────────────
# مسارات المصادر
# ─────────────────────────────────────────────────────────────────────
set(SAD_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../../..")

# Include paths
include_directories(
    ${SAD_ROOT}/shared/lexer/include
    ${SAD_ROOT}/shared/parser/include
    ${SAD_ROOT}/shared/ast/include
    ${SAD_ROOT}/shared/types/include
    ${SAD_ROOT}/shared/errors/include
    ${SAD_ROOT}/interpreter_new/include
)

# ─────────────────────────────────────────────────────────────────────
# ملفات المصدر
# ─────────────────────────────────────────────────────────────────────

# JNI Bridge
set(JNI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/sad_jni_bridge.cpp
)

# Shared Core (نسخة مصغرة للأندرويد)
file(GLOB_RECURSE LEXER_SOURCES "${SAD_ROOT}/shared/lexer/src/*.cpp")
file(GLOB_RECURSE PARSER_SOURCES "${SAD_ROOT}/shared/parser/src/*.cpp")
file(GLOB_RECURSE AST_SOURCES "${SAD_ROOT}/shared/ast/src/*.cpp")
file(GLOB_RECURSE TYPES_SOURCES "${SAD_ROOT}/shared/types/src/*.cpp")
file(GLOB_RECURSE ERRORS_SOURCES "${SAD_ROOT}/shared/errors/src/*.cpp")
file(GLOB_RECURSE INTERPRETER_SOURCES "${SAD_ROOT}/interpreter_new/src/*.cpp")

# ─────────────────────────────────────────────────────────────────────
# المكتبة المشتركة
# ─────────────────────────────────────────────────────────────────────
add_library(sadnotes SHARED
    ${JNI_SOURCES}
    ${LEXER_SOURCES}
    ${PARSER_SOURCES}
    ${AST_SOURCES}
    ${TYPES_SOURCES}
    ${ERRORS_SOURCES}
    ${INTERPRETER_SOURCES}
)

# ─────────────────────────────────────────────────────────────────────
# الربط
# ─────────────────────────────────────────────────────────────────────
find_library(log-lib log)

target_link_libraries(sadnotes
    ${log-lib}
    android
)

# ─────────────────────────────────────────────────────────────────────
# تعريفات المعالج المسبق
# ─────────────────────────────────────────────────────────────────────
target_compile_definitions(sadnotes PRIVATE
    SAD_ANDROID=1
    SAD_MOBILE=1
)
