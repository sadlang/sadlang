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
# (AR) تنفيذي مركز الأدوات: sad.exe — الهدفُ والمُخرَجُ والاسمُ المنشورُ
#      اسمٌ واحدٌ لا ثلاثة. ولا شَرطةَ فيه عمدًا: المركزُ يكتشفُ إخوتَه
#      بالبادئةِ «sad-»، فاسمٌ بلا شَرطةٍ يمنعُه من تسجيلِ نفسِه أمرًا.
# (EN) Hub executable: sad.exe — target, output and published name are ONE.
#      No dash on purpose: the hub discovers siblings by the "sad-" prefix,
#      so a dashless name keeps it from registering itself as a subcommand.
# ─────────────────────────────────────────────────────────────────────
add_executable(sad src/main.cpp)

target_link_libraries(sad PRIVATE sad_hub_lib)

set_target_properties(sad PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

if(MSVC)
    target_compile_options(sad PRIVATE /utf-8 /wd4819 /FS)
endif()

message(STATUS "✓ مركز الأدوات / Tool Hub: sad")
