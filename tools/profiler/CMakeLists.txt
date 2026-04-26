# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: tools/profiler/CMakeLists.txt
# الوصف: بناء مصحح أداء لغة ص (sad-profiler)
# ═══════════════════════════════════════════════════════════════════════════════
#
# هذا الملف يُعرّف هدفين:
#   1. sad_profiler_lib  — مكتبة ثابتة تحتوي على نواة المصحح (للدمج مع المفسر)
#   2. sad-profiler      — الأداة التنفيذية المستقلة (CLI)
#
# الاستخدام:
#   cmake --build build --config Debug --target sad-profiler
#   cmake --build build --config Debug --target sad_profiler_lib
# ═══════════════════════════════════════════════════════════════════════════════

# ──────────────────────────────────────────────────────────────────────
# (AR) مكتبة نواة المصحح / (EN) Profiler core library
# ──────────────────────────────────────────────────────────────────────
add_library(sad_profiler_lib STATIC
    src/profiler_core.cpp
)

target_include_directories(sad_profiler_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# (AR) لا تعتمد على مكونات المفسر — المكتبة مستقلة
# (EN) Does not depend on interpreter components — the library is self-contained
set_target_properties(sad_profiler_lib PROPERTIES
    OUTPUT_NAME "sad_profiler"
)

# (AR) دعم UTF-8 على MSVC / (EN) UTF-8 support on MSVC
if(MSVC)
    target_compile_options(sad_profiler_lib PRIVATE /utf-8)
endif()

# ──────────────────────────────────────────────────────────────────────
# (AR) الأداة التنفيذية / (EN) Executable tool
# ──────────────────────────────────────────────────────────────────────
add_executable(sad-profiler
    src/main.cpp
)

target_include_directories(sad-profiler PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
    ${CMAKE_SOURCE_DIR}/shared/parser/include
    ${CMAKE_SOURCE_DIR}/shared/ast/include
    ${CMAKE_SOURCE_DIR}/shared/types/include
    ${CMAKE_SOURCE_DIR}/shared/errors/include
    ${CMAKE_SOURCE_DIR}/shared/utils/include
    ${CMAKE_SOURCE_DIR}/interpreter/include
    ${CMAKE_SOURCE_DIR}/interpreter/include/core
    ${CMAKE_SOURCE_DIR}/interpreter/include/visitors
    ${CMAKE_SOURCE_DIR}/interpreter/include/managers
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(sad-profiler PRIVATE
    sad_profiler_lib
    sad_core
    sad_semantic
)

# (AR) ربط مكتبات النظام / (EN) Link system libraries
if(WIN32)
    target_link_libraries(sad-profiler PRIVATE ws2_32)
endif()

set_target_properties(sad-profiler PROPERTIES
    OUTPUT_NAME "sad-profiler"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

message(STATUS "✓ مصحح الأداء / Profiler: sad-profiler")
