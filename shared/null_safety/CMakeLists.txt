# ============================================================================
# (AR) مكتبة أمان null المشتركة - sad_null_safety
# (EN) Shared Null-Safety Library - sad_null_safety
#
# (AR) المكوّن الموحَّد لتحليل أمان null عبر كل المسارات (مصدر حقيقة واحد):
#      - sad-run (المفسّر)  — يستدعي analyze() قبل التنفيذ
#      - sad-build (المترجم) — يستدعي analyze() في مرحلة التحليل قبل SIR
#      كنمط فاحص الاستعارة (shared/ownership) تمامًا.
#
# (EN) Unified null-safety analysis component for all execution paths
#      (single source of truth), mirroring the shared ownership pattern.
#
# @phase NS-01 — هيكل المكوّن المشترك + استدعاؤه من المحرّكين (بلا منطق بعد)
# @see _bmad-output/systems/null-safety/
# ============================================================================

cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SAD_NULL_SAFETY_SOURCES "src/*.cpp")
file(GLOB SAD_NULL_SAFETY_HEADERS "include/null_safety/*.h")

add_library(sad_null_safety STATIC ${SAD_NULL_SAFETY_SOURCES} ${SAD_NULL_SAFETY_HEADERS})

target_include_directories(sad_null_safety
    PUBLIC
        # (AR) المسار العام للرؤوس — يتيح #include "null_safety/null_safety_analyzer.h"
        # (EN) Public header path
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        # (AR) رؤوس المشروع المشتركة المطلوبة (ast, types, errors, ...)
        # (EN) Required shared project headers
        ${CMAKE_SOURCE_DIR}/shared/ast/include
        ${CMAKE_SOURCE_DIR}/shared/types/include
        ${CMAKE_SOURCE_DIR}/shared/lexer/include
        ${CMAKE_SOURCE_DIR}/shared/parser/include
        ${CMAKE_SOURCE_DIR}/shared/errors/include
        ${CMAKE_SOURCE_DIR}/shared/utils/include
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(sad_null_safety
    PUBLIC
        sad_shared
        sad_type_system
)

if(MSVC)
    target_compile_options(sad_null_safety PRIVATE /utf-8 /wd4819 /FS)
endif()

message(STATUS "sad_null_safety: built from shared/null_safety/ (unified null-safety, NS-01 scaffold)")
