# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: tools/formatter/CMakeLists.txt
# (AR) الوصف: مكتبة منسّق كود لغة ص (sad_formatter_lib) + الأداة (sad-fmt)
# (EN) Build: SadFormatter library + sad-fmt CLI
# ═══════════════════════════════════════════════════════════════════════════════
#
# (AR) المنسق ليس جزءاً من المترجم/المفسر — هو أداة تحرير. لذلك يعيش هنا
#      في tools/formatter ويعتمد فقط على tools/shared (الذي بدوره يعتمد
#      على shared/lexer للحصول على KeywordTable). أي تغيير في الكلمات
#      المحجوزة في shared/lexer ينتشر تلقائياً إلى المنسق و LSP.
# (EN) The formatter is editor tooling, not part of the compiler/interpreter.
#      Lives in tools/formatter and depends only on tools/shared (which in
#      turn uses shared/lexer's KeywordTable). Reserved-keyword changes in
#      shared/lexer propagate automatically to formatter and LSP.
# ═══════════════════════════════════════════════════════════════════════════════

cmake_minimum_required(VERSION 3.15)

message(STATUS "════════════════════════════════════════════════════════")
message(STATUS "   إعداد منسّق لغة ص - Configuring Sad Formatter")
message(STATUS "════════════════════════════════════════════════════════")

# ═══════════════════════════════════════════════════════════════════════════════
# 1) المكتبة الثابتة sad_formatter_lib — يستهلكها CLI و LSP والاختبارات
# ═══════════════════════════════════════════════════════════════════════════════
set(SAD_FORMATTER_LIB_SOURCES
    src/sad_formatter.cpp
    src/sad_formatter_rebuild.cpp
)

add_library(sad_formatter_lib STATIC ${SAD_FORMATTER_LIB_SOURCES})

target_compile_features(sad_formatter_lib PUBLIC cxx_std_17)

target_include_directories(sad_formatter_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/format
)

# (AR) ربط بطبقة الأدوات المشتركة (التي توفر تصنيف الكلمات)
# (EN) Link to the tools-shared layer (which provides keyword classification)
if(TARGET sad_tools_shared)
    target_link_libraries(sad_formatter_lib PUBLIC sad_tools_shared)
else()
    message(FATAL_ERROR "sad_formatter_lib يتطلب sad_tools_shared — تأكد من تسجيل tools/shared قبل tools/formatter في cmake/tools.cmake / sad_formatter_lib requires sad_tools_shared; register tools/shared before tools/formatter in cmake/tools.cmake")
endif()

if(MSVC)
    target_compile_options(sad_formatter_lib PRIVATE /utf-8 /wd4819 /FS)
endif()

# (AR) اسم بديل لتوافق الكود الذي يربط بـ "sad_formatter" (مثل tools/lsp)
# (EN) Alias for code linking against legacy target name "sad_formatter" (e.g. tools/lsp)
add_library(sad_formatter ALIAS sad_formatter_lib)

message(STATUS "✓ مكتبة المنسق / Formatter library: sad_formatter_lib")

# ═══════════════════════════════════════════════════════════════════════════════
# 2) الملف التنفيذي sad-fmt
# ═══════════════════════════════════════════════════════════════════════════════
add_executable(sad-fmt fmt_main.cpp)

target_compile_features(sad-fmt PUBLIC cxx_std_17)

target_include_directories(sad-fmt PRIVATE
    ${CMAKE_SOURCE_DIR}/shared/utils/include
)

target_link_libraries(sad-fmt PRIVATE sad_formatter_lib)

if(MSVC)
    target_compile_options(sad-fmt PRIVATE /wd4819 /FS /utf-8)
endif()

set_target_properties(sad-fmt PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    OUTPUT_NAME "sad-fmt"
)

message(STATUS "✓ أداة التنسيق / Formatter CLI: sad-fmt")
message(STATUS "  - المسار / Path: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sad-fmt.exe")

# ═══════════════════════════════════════════════════════════════════════════════
# 3) الاختبارات (اختيارية) / Tests (optional)
# ═══════════════════════════════════════════════════════════════════════════════
if(BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_formatter.cpp")
    add_executable(test_formatter tests/test_formatter.cpp)
    target_link_libraries(test_formatter PRIVATE sad_formatter_lib)
    if(MSVC)
        target_compile_options(test_formatter PRIVATE /utf-8 /wd4819 /FS)
    endif()
    set_target_properties(test_formatter PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)
    message(STATUS "✓ اختبار المنسق / Formatter test: test_formatter")
endif()
