# بسم الله الرحمن الرحيم
# CMakeLists.txt for sad-repl

cmake_minimum_required(VERSION 3.15)

# ========================================
# REPL Target / هدف REPL
# ========================================

# (AR) ملاحظة: بنية غير مسطّحة — المصادر في src/ والرؤوس في include/
#      (تطابق عُرف المستودع: compiler/{src,include} وshared/*/include).
# (EN) Note: non-flat layout — sources under src/, headers under include/
#      (matches repo convention: compiler/{src,include} and shared/*/include).

# (AR) جمع المصادر / (EN) Collect sources
set(REPL_SOURCES
    src/main.cpp
    src/repl_engine.cpp
    src/history_manager.cpp
    src/repl_commands.cpp
    src/repl_line_editor.cpp
    src/shell_executor.cpp
    src/shell_lexer.cpp
    # (AR) كتالوج «مصدر حقيقة الأدوات» المولَّد (مُلتزَم؛ يُوَلَّد بـ x.py gen) —
    #      أخطاء الأداة + رسائل واجهتها + أوامرها من language-truth/tools/repl/*.yaml.
    # (EN) Generated Tool-SoT catalog (committed; produced by x.py gen).
    generated/repl_sot_generated.cpp
)

set(REPL_HEADERS
    include/repl_engine.h
    include/history_manager.h
    include/repl_commands.h
    include/repl_line_editor.h
    include/shell_executor.h
    include/shell_lexer.h
)

# (AR) إنشاء التنفيذي sad-repl / (EN) Create sad-repl executable
add_executable(sad-repl ${REPL_SOURCES} ${REPL_HEADERS})

# (AR) مسارات التضمين / (EN) Include directories
target_include_directories(sad-repl PRIVATE
    # (AR) رؤوس الأداة الخاصّة (البنية غير المسطّحة) — تُتيح #include "repl_engine.h" من src/
    # (EN) tool's own headers (non-flat layout) — lets src/ use #include "repl_engine.h"
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    # (AR) رأس كتالوج SoT المولَّد — يُتيح #include "repl_sot_generated.h"
    # (EN) generated Tool-SoT catalog header — enables #include "repl_sot_generated.h"
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/compiler/include
    ${CMAKE_SOURCE_DIR}
    # (AR) Phase 4 (F-03): مسارات shared لتجنب #include النسبية
    # (EN) Phase 4 (F-03): shared paths to avoid relative #includes
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
    ${CMAKE_SOURCE_DIR}/shared/parser/include
    ${CMAKE_SOURCE_DIR}/shared/ast/include
    ${CMAKE_SOURCE_DIR}/shared/types/include
    ${CMAKE_SOURCE_DIR}/shared/errors/include
    ${CMAKE_SOURCE_DIR}/shared/utils/include
)

# (AR) ربط المكتبات / (EN) Link libraries
target_link_libraries(sad-repl PRIVATE
    # (AR) م2-أ: جسر الواجهات — REPL ينفّذ كود مستخدم تفاعليًّا فيدعم الرسومات
    #      (repl_engine.cpp يستدعي installSadGraphicsBridge). يربط sad_core + sad_graphics عبوريًّا.
    # (EN) Phase 2-A: UI bridge — the REPL executes user code interactively, so it
    #      supports UI (repl_engine.cpp installs it). Pulls sad_core + sad_graphics.
    $<$<TARGET_EXISTS:sad_graphics_bridge>:sad_graphics_bridge>
    sad_core
    sad_semantic
)

# (AR) معيار C++ / (EN) C++ Standard
set_target_properties(sad-repl PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# (AR) مجلّد الإخراج / (EN) Output directory
# (AR) نستعمل ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} كبقيّة الأهداف ليقع sad-repl في
#      نفس مجلّد sad-hub (الذي يكتشف الأدوات بمسح أشقّائه). الصيغة القديمة
#      bin/${CMAKE_BUILD_TYPE} كانت تُنتج bin/Debug/Debug على MSVC (إلحاق تلقائيّ
#      للتهيئة) وbin/Debug على Linux بينما الأشقّاء في bin/ — فيختفي repl من --list.
# (EN) Use ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} like every other target so sad-repl
#      lands next to sad-hub (which discovers tools by scanning siblings). The old
#      bin/${CMAKE_BUILD_TYPE} produced bin/Debug/Debug on MSVC (auto config append)
#      and bin/Debug on Linux while siblings sit in bin/ — hiding repl from --list.
set_target_properties(sad-repl PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# ========================================
# إعدادات خاصّة بالمنصّة / Platform-specific settings
# ========================================

if(WIN32)
    # (AR) ويندوز: تفعيل UTF-8 / (EN) Windows: enable UTF-8
    target_compile_options(sad-repl PRIVATE /utf-8)
endif()

# (AR) علَم headless: يُسقط جسر SDL من REPL / (EN) headless flag drops the SDL bridge
target_compile_definitions(sad-repl PRIVATE SAD_ENABLE_GRAPHICS=$<BOOL:${SAD_ENABLE_GRAPHICS}>)

# (AR) ربط ساكن اختياريّ لأنظمة مضمّنة/initramfs: يُضمّن libstdc++/libgcc ساكنًا
#      (يبقى glibc ديناميكيًّا). الربط الساكن الكامل (-static) محجوب حاليًّا بربط
#      sad_core لـopenssl/sqlite بمسارات مطلقة — دَين موثّق (جعل تلك البُنى اختياريّة).
# (EN) Optional static link for embedded/initramfs: statically embeds libstdc++/libgcc
#      (glibc stays dynamic). Full -static is blocked by sad_core absolute-path
#      openssl/sqlite links — documented debt (make those deps optional).
if(SAD_STATIC_REPL AND NOT MSVC)
    target_link_options(sad-repl PRIVATE -static-libgcc -static-libstdc++)
endif()

# ========================================
# Installation / التثبيت
# ========================================

install(TARGETS sad-repl
    RUNTIME DESTINATION bin
    COMPONENT repl
)

# (AR) تثبيت README / (EN) Install README
install(FILES README.md
    DESTINATION share/doc/sad-repl
    COMPONENT repl
)

message(STATUS "✓ بيئة التطوير التفاعلية / REPL: sad-repl")
