# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# tools/hub/CMakeLists.txt
# (AR) مركز أدوات لغة ص — تنفيذي مستقل لا يعتمد على المفسّر ولا المترجم
# (EN) Sad Tool Hub — standalone executable, no interpreter/compiler dependency
# ═══════════════════════════════════════════════════════════════════════════════

# ─────────────────────────────────────────────────────────────────────
# (AR) مكتبة ساكنة قابلة للاختبار
# (EN) Static library for testability
# ─────────────────────────────────────────────────────────────────────
add_library(sad_hub_lib STATIC
    src/tool_registry.cpp
    src/command_dispatcher.cpp
    src/help_renderer.cpp
)

target_include_directories(sad_hub_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/shared/utils/include
)

target_compile_features(sad_hub_lib PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(sad_hub_lib PRIVATE /utf-8 /wd4819 /FS)
endif()

# (AR) المكتبة لا تعتمد على أي شيء من lang core — مستقلة تماماً
# (EN) Library has no dependency on lang core — fully standalone

# ─────────────────────────────────────────────────────────────────────
# (AR) تنفيذي مركز الأدوات: sad-hub.exe
# (EN) Hub executable: sad-hub.exe
# ─────────────────────────────────────────────────────────────────────
add_executable(sad_hub src/main.cpp)

target_link_libraries(sad_hub PRIVATE sad_hub_lib)

set_target_properties(sad_hub PROPERTIES
    OUTPUT_NAME "sad-hub"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

if(MSVC)
    target_compile_options(sad_hub PRIVATE /utf-8 /wd4819 /FS)
endif()

# ─────────────────────────────────────────────────────────────────────
# (AR) sad.exe = sad-hub.exe — نقطة الدخول الموحَّدة (لا توافقية خلفية)
#      كل استدعاء `sad <subcommand>` يمر عبر الهب.
# (EN) sad.exe = sad-hub.exe — unified entry point (no backward compat).
#      Every `sad <subcommand>` call goes through the hub.
# ─────────────────────────────────────────────────────────────────────
if(WIN32)
    set(_SAD_ALIAS_NAME "sad.exe")
else()
    set(_SAD_ALIAS_NAME "sad")
endif()

add_custom_command(TARGET sad_hub POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:sad_hub>"
        "$<TARGET_FILE_DIR:sad_hub>/${_SAD_ALIAS_NAME}"
    COMMENT "(AR) نسخ sad-hub → sad / (EN) Copy sad-hub → sad"
    VERBATIM
)

message(STATUS "✓ مركز الأدوات / Tool Hub: sad-hub")
