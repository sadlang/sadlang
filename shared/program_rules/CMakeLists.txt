# ============================================================================
# (AR) مكتبة قواعد البرنامج المشتركة - sad_program_rules
# (EN) Shared Program-Rules Library - sad_program_rules
#
# (AR) المكوّن الموحَّد لقواعد البرنامج البنيويّة عبر المحرّكين (مصدر حقيقة واحد):
#      - sad-run (المفسّر)  — يستدعي checkMainFunctionRule() قبل التنفيذ
#      - sad-build (المترجم) — يستدعي checkMainFunctionRule() بعد التحليل قبل SIR
#      يبدأ بقاعدة الدالة الرئيسية (SEM018)، كنمط sad_null_safety تمامًا.
#
# (EN) Unified structural program-rules component for both engines (single source
#      of truth), mirroring the shared null-safety pattern. Starts with the
#      main-function rule (SEM018).
# ============================================================================

cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SAD_PROGRAM_RULES_SOURCES "src/*.cpp")
file(GLOB SAD_PROGRAM_RULES_HEADERS "include/program_rules/*.h")

add_library(sad_program_rules STATIC ${SAD_PROGRAM_RULES_SOURCES} ${SAD_PROGRAM_RULES_HEADERS})

target_include_directories(sad_program_rules
    PUBLIC
        # (AR) المسار العام للرؤوس — يتيح #include "program_rules/main_function_rule.h"
        # (EN) Public header path
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        # (AR) رؤوس المشروع المشتركة المطلوبة (ast, lexer, ...)
        # (EN) Required shared project headers
        ${CMAKE_SOURCE_DIR}/shared/ast/include
        ${CMAKE_SOURCE_DIR}/shared/types/include
        ${CMAKE_SOURCE_DIR}/shared/lexer/include
        ${CMAKE_SOURCE_DIR}/shared/parser/include
        ${CMAKE_SOURCE_DIR}/shared/errors/include
        ${CMAKE_SOURCE_DIR}/shared/utils/include
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(sad_program_rules
    PUBLIC
        sad_shared
)

if(MSVC)
    target_compile_options(sad_program_rules PRIVATE /utf-8 /wd4819 /FS)
endif()

message(STATUS "sad_program_rules: built from shared/program_rules/ (unified main-function rule SEM018)")
