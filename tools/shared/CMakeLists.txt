# بسم الله الرحمن الرحيم
# ============================================================================
# tools/shared/CMakeLists.txt — مكتبة مشتركة بين أدوات لغة ص
# Shared library for Sad language tooling (formatter, LSP, ...)
# ============================================================================
#
# (AR) هذه الطبقة المشتركة لأدوات اللغة (ليست للغة نفسها). توفر تصنيفاً
#      للكلمات المفتاحية مبنياً مباشرة على `Sad::Lexer::KeywordTable`،
#      مما يضمن أن أي تعديل في الكلمات المحجوزة ينعكس تلقائياً على
#      المنسق و LSP وأي أداة مستقبلية.
# (EN) Tools-shared layer (not part of the language). Provides keyword
#      classification built directly on `Sad::Lexer::KeywordTable`, ensuring
#      changes to reserved keywords propagate automatically to formatter,
#      LSP, and any future tool.
# ============================================================================

cmake_minimum_required(VERSION 3.15)

message(STATUS "════════════════════════════════════════════════════════")
message(STATUS "   إعداد طبقة الأدوات المشتركة - Configuring Tools-Shared Layer")
message(STATUS "════════════════════════════════════════════════════════")

set(TOOLS_SHARED_SOURCES
    src/formatter_keywords.cpp
)

add_library(sad_tools_shared STATIC ${TOOLS_SHARED_SOURCES})

target_include_directories(sad_tools_shared
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/shared/lexer/include
        ${CMAKE_SOURCE_DIR}/shared/types/include
        ${CMAKE_SOURCE_DIR}/shared/errors/include
)

target_compile_features(sad_tools_shared PUBLIC cxx_std_17)

# (AR) ربط بطبقة المعجم المشتركة للحصول على KeywordTable
# (EN) Link to shared lexer layer to access KeywordTable
if(TARGET sad_shared)
    target_link_libraries(sad_tools_shared PUBLIC sad_shared)
endif()

if(MSVC)
    target_compile_options(sad_tools_shared PRIVATE /utf-8 /wd4819 /FS)
endif()

message(STATUS "✓ طبقة الأدوات المشتركة جاهزة / Tools-Shared layer ready")
