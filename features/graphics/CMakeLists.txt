# ═══════════════════════════════════════════════════════════════════════════════
# ملف: CMakeLists.txt
# المسار: sad_graphics/CMakeLists.txt
# ═══════════════════════════════════════════════════════════════════════════════
#
# الوصف:
# ------
# ملف بناء CMake لنظام واجهات لغة ص (SadUI).
#
# البنية:
# ───────
#   sad_graphics/
#   ├── core/         → النواة المشتركة (أنواع، عقد، IR، تخطيط، حالة)
#   ├── backends/     → مولّدات الكود لكل منصة
#   │   ├── desktop/  → SDL2+OpenGL (سطح المكتب و WASM)
#   │   ├── android/  → Jetpack Compose
#   │   ├── ios/      → SwiftUI
#   │   └── web/      → HTML5/CSS3/JS
#   ├── widgets/      → عناصر واجهة جاهزة (نص، زر، صورة...)
#   └── pipeline/     → خط أنابيب المعالجة الرئيسي
#
# الاستخدام:
# ──────────
#   # في CMakeLists.txt الرئيسي:
#   add_subdirectory(sad_graphics)
#   target_link_libraries(my_target PRIVATE sad_graphics)
#
# ═══════════════════════════════════════════════════════════════════════════════

cmake_minimum_required(VERSION 3.15)
project(SadUI LANGUAGES CXX)

# ── ملفات المصدر ──────────────────────────────────────────────────

# النواة
set(SAD_UI_CORE_SOURCES
    core/src/types.cpp
    core/src/node.cpp
    core/src/nav.cpp
    core/src/window_control.cpp
    core/src/ir.cpp
    core/src/ir_builder.cpp
    core/src/hybrid_routing.cpp
    core/src/style.cpp
    core/src/layout.cpp
    core/src/state.cpp
    core/src/animation.cpp
    core/src/platform_renderer.cpp
    core/src/theme.cpp
    core/src/gesture.cpp
    core/src/scroll_physics.cpp
    core/src/accessibility.cpp
    core/src/form_validation.cpp
    core/src/focus.cpp
    core/src/image_cache.cpp
    core/src/notification.cpp
    core/src/touch_processor.cpp
    core/src/keyboard_processor.cpp
    core/src/mouse_processor.cpp
    core/src/reconciler.cpp
    core/src/widget_node.cpp
    core/src/ui_arena.cpp
    core/src/command_queue.cpp
    core/src/ui_event_loop.cpp
    core/src/property_binding.cpp
    core/src/print_tree.cpp
)

# النظام التفاعلي (reactive — المرحلة 1)
set(SAD_UI_REACTIVE_SOURCES
    reactive/binding_compiler.cpp
)

# سطح المكتب
set(SAD_UI_DESKTOP_SOURCES
    backends/desktop/src/renderer.cpp
    backends/desktop/src/renderer_draw.cpp
    backends/desktop/src/renderer_canvas.cpp
    backends/desktop/src/text_editing.cpp
    backends/desktop/src/window.cpp
)

# أندرويد
set(SAD_UI_ANDROID_SOURCES
    backends/android/src/compose_codegen.cpp
)
if(ANDROID)
    list(APPEND SAD_UI_ANDROID_SOURCES backends/android/src/android_renderer.cpp)
endif()

# iOS
set(SAD_UI_IOS_SOURCES
    backends/ios/src/swiftui_codegen.cpp
)
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    enable_language(OBJCXX)
    list(APPEND SAD_UI_IOS_SOURCES backends/ios/src/ios_renderer.mm)
endif()

# ويب
set(SAD_UI_WEB_SOURCES
    backends/web/src/html_codegen.cpp
    backends/web/src/html_css_codegen.cpp
    backends/web/src/html_element_codegen.cpp
    backends/web/src/html_element_advanced_codegen.cpp
    backends/web/src/html_script_codegen.cpp
    backends/web/src/html_style_codegen.cpp
)
if(EMSCRIPTEN)
    list(APPEND SAD_UI_WEB_SOURCES backends/web/src/web_renderer.cpp)
endif()

# macOS
set(SAD_UI_MACOS_SOURCES
    backends/macos/src/appkit_codegen.cpp
)
if(APPLE AND NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
    enable_language(OBJCXX)
    # macos_renderer.mm excluded: incomplete native renderer (undeclared
    # members hitTest/fireEvent/focusedNode_ ...). Not needed by interpreter.
    # list(APPEND SAD_UI_MACOS_SOURCES backends/macos/src/macos_renderer.mm)
endif()

# الوضع المستقل (Freestanding — بدون SDL2/libc)
# مُصيّر يعمل مباشرة على framebuffer — لأنظمة التشغيل والـ bare-metal
option(SAD_UI_ENABLE_FREESTANDING "تفعيل خلفية الوضع المستقل / Enable freestanding renderer" ON)
set(SAD_UI_FREESTANDING_SOURCES
    backends/freestanding/src/renderer.cpp
    backends/freestanding/src/builtin_font.cpp
    # (AR) تشكيل عربيّ متعدّد المنصّات — يعتمده renderer.cpp (يُترجَم في كلّ المنصّات).
    #      linuxfb/psf_font/evdev لينكس-خاصّة تبنيها sad-os خارجيًّا فلا تُدرَج هنا.
    # (EN) Cross-platform Arabic shaper — required by renderer.cpp (built on all
    #      platforms). linuxfb/psf_font/evdev are Linux-only (built by sad-os).
    backends/freestanding/src/arabic_shaper.cpp
)

# العناصر
set(SAD_UI_WIDGETS_SOURCES
    widgets/src/text.cpp
    widgets/src/button.cpp
    widgets/src/image.cpp
    widgets/src/column.cpp
    widgets/src/row.cpp
    widgets/src/input_widgets.cpp
    widgets/src/layout_widgets.cpp
    widgets/src/container_widgets.cpp
    widgets/src/navigation_widgets.cpp
    widgets/src/feedback_widgets.cpp
    widgets/src/advanced_widgets.cpp
    # ── عناصر جديدة (30 عنصر) ──
    widgets/data/src/data_widgets.cpp
    widgets/media/src/media_widgets.cpp
    widgets/animation/src/animation_widgets.cpp
    widgets/controls/src/control_widgets.cpp
    widgets/navigation_v2/src/nav_v2_widgets.cpp
)

# خط الأنابيب
set(SAD_UI_PIPELINE_SOURCES
    pipeline/src/pipeline.cpp
)

# ── تجميع جميع المصادر ────────────────────────────────────────────

set(SAD_UI_ALL_SOURCES
    ${SAD_UI_CORE_SOURCES}
    ${SAD_UI_REACTIVE_SOURCES}
    ${SAD_UI_DESKTOP_SOURCES}
    ${SAD_UI_ANDROID_SOURCES}
    ${SAD_UI_IOS_SOURCES}
    ${SAD_UI_WEB_SOURCES}
    ${SAD_UI_MACOS_SOURCES}
    ${SAD_UI_FREESTANDING_SOURCES}
    ${SAD_UI_WIDGETS_SOURCES}
    ${SAD_UI_PIPELINE_SOURCES}
)

# ── المكتبة ────────────────────────────────────────────────────────

add_library(sad_graphics STATIC ${SAD_UI_ALL_SOURCES})

# ── مسارات التضمين ──────────────────────────────────────────────

target_include_directories(sad_graphics
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/core/include
        ${CMAKE_CURRENT_SOURCE_DIR}/reactive
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/desktop/include
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/android/include
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/ios/include
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/web/include
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/macos/include
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/freestanding/include
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets/include
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets/data/include
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets/media/include
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets/animation/include
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets/controls/include
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets/navigation_v2/include
        ${CMAKE_CURRENT_SOURCE_DIR}/pipeline/include
        ${CMAKE_SOURCE_DIR}/features/graphics/third_party
    PRIVATE
        # (AR) مسارات AST المشتركة — يحتاجها binding_compiler.cpp فقط
        # (EN) Shared AST headers — needed only by binding_compiler.cpp
        ${CMAKE_SOURCE_DIR}/shared/ast/include
        ${CMAKE_SOURCE_DIR}/shared/types/include
        # (② rfcs#46) تخطيط حدث المُولَّد (sad_event_layout_generated.h) — يستهلكه المُرسِل
        ${CMAKE_SOURCE_DIR}/shared/types/generated
        ${CMAKE_SOURCE_DIR}/shared/lexer/include
)

# ── إعدادات المترجم ─────────────────────────────────────────────

target_compile_features(sad_graphics PUBLIC cxx_std_17)

if(MSVC)
    # دعم UTF-8 لملفات المصدر العربية
    target_compile_options(sad_graphics PRIVATE /utf-8 /wd4819 /FS)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(sad_graphics PRIVATE -finput-charset=UTF-8)
endif()

# ══════════════════════════════════════════════════════════════════════════════
# SDL2 و SDL2_ttf (اختياري لسطح المكتب) / SDL2 & SDL2_ttf (optional for desktop)
# ══════════════════════════════════════════════════════════════════════════════
option(SAD_UI_ENABLE_SDL2 "تفعيل عرض SDL2 لسطح المكتب / Enable SDL2 rendering for desktop" ON)

if(SAD_UI_ENABLE_SDL2)
    # أولاً: محاولة إيجاد SDL2 على النظام
    find_package(SDL2 CONFIG QUIET)
    
    if(SDL2_FOUND)
        message(STATUS "  [sad_graphics] ✓ SDL2 موجود على النظام / Found on system")
        target_link_libraries(sad_graphics PUBLIC SDL2::SDL2)
        target_compile_definitions(sad_graphics PUBLIC SAD_UI_USE_SDL2)
        
        # محاولة إيجاد SDL2_ttf
        find_package(SDL2_ttf CONFIG QUIET)
        if(SDL2_ttf_FOUND)
            message(STATUS "  [sad_graphics] ✓ SDL2_ttf موجود / Found")
            target_link_libraries(sad_graphics PUBLIC SDL2_ttf::SDL2_ttf)
            target_compile_definitions(sad_graphics PUBLIC SAD_UI_HAS_SDL_TTF)
        else()
            message(STATUS "  [sad_graphics] ⚠ SDL2_ttf غير موجود — عرض النصوص معطّل / Not found — text rendering disabled")
        endif()
    else()
        # ثانياً: استخدام SDL2 المُضمن في المشروع / Use bundled SDL2
        set(SDL2_BUNDLED_DIR "${CMAKE_SOURCE_DIR}/features/graphics/third_party/SDL2/SDL2-2.28.5")
        set(SDL2_BUNDLED_INCLUDE "${SDL2_BUNDLED_DIR}/include")
        
        # تحديد مكتبة x64 أو x86
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(SDL2_BUNDLED_LIB_DIR "${SDL2_BUNDLED_DIR}/lib/x64")
        else()
            set(SDL2_BUNDLED_LIB_DIR "${SDL2_BUNDLED_DIR}/lib/x86")
        endif()
        
        set(SDL2_BUNDLED_LIB "${SDL2_BUNDLED_LIB_DIR}/SDL2.lib")
        set(SDL2_BUNDLED_MAIN "${SDL2_BUNDLED_LIB_DIR}/SDL2main.lib")
        set(SDL2_BUNDLED_DLL "${SDL2_BUNDLED_LIB_DIR}/SDL2.dll")
        
        if(EXISTS "${SDL2_BUNDLED_LIB}" AND EXISTS "${SDL2_BUNDLED_INCLUDE}")
            message(STATUS "  [sad_graphics] ✓ SDL2 مُضمن / Using bundled SDL2 from ${SDL2_BUNDLED_DIR}")
            
            # إضافة مسارات التضمين
            target_include_directories(sad_graphics PUBLIC ${SDL2_BUNDLED_INCLUDE})
            
            # ربط المكتبات
            target_link_libraries(sad_graphics PUBLIC ${SDL2_BUNDLED_LIB} ${SDL2_BUNDLED_MAIN})
            
            # تفعيل الـ macro
            target_compile_definitions(sad_graphics PUBLIC SAD_UI_USE_SDL2)
            
            # نسخ SDL2.dll إلى مجلد الإخراج تلقائياً
            # (AR) ننشئ مجلد الإخراج أولاً: عند بناء sad_graphics (مكتبة) منفردًا قبل
            #      ربط أيّ تنفيذيّ، لا يكون bin/$<CONFIG>/ موجودًا بعد فيفشل النسخ.
            # (EN) Create the output dir first: when sad_graphics (a lib) is built before
            #      any executable links, bin/$<CONFIG>/ may not exist yet and the copy fails.
            if(EXISTS "${SDL2_BUNDLED_DLL}")
                add_custom_command(TARGET sad_graphics POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory
                    "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/"
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${SDL2_BUNDLED_DLL}"
                    "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/"
                    COMMENT "نسخ SDL2.dll إلى مجلد الإخراج / Copying SDL2.dll to output"
                )
            endif()
            
            # ═══ SDL2_ttf المُضمن ═══
            set(SDL2_TTF_BUNDLED_DIR "${CMAKE_SOURCE_DIR}/features/graphics/third_party/SDL2_ttf-2.20.2")
            set(SDL2_TTF_BUNDLED_INCLUDE "${SDL2_TTF_BUNDLED_DIR}/include")
            if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                set(SDL2_TTF_BUNDLED_LIB_DIR "${SDL2_TTF_BUNDLED_DIR}/lib/x64")
            else()
                set(SDL2_TTF_BUNDLED_LIB_DIR "${SDL2_TTF_BUNDLED_DIR}/lib/x86")
            endif()
            set(SDL2_TTF_BUNDLED_LIB "${SDL2_TTF_BUNDLED_LIB_DIR}/SDL2_ttf.lib")
            set(SDL2_TTF_BUNDLED_DLL "${SDL2_TTF_BUNDLED_LIB_DIR}/SDL2_ttf.dll")
            
            if(EXISTS "${SDL2_TTF_BUNDLED_LIB}" AND EXISTS "${SDL2_TTF_BUNDLED_INCLUDE}")
                message(STATUS "  [sad_graphics] ✓ SDL2_ttf مُضمن / Using bundled SDL2_ttf")
                target_include_directories(sad_graphics PUBLIC ${SDL2_TTF_BUNDLED_INCLUDE})
                target_link_libraries(sad_graphics PUBLIC ${SDL2_TTF_BUNDLED_LIB})
                target_compile_definitions(sad_graphics PUBLIC SAD_UI_HAS_SDL_TTF)
                
                # نسخ SDL2_ttf.dll (ننشئ مجلد الإخراج أولاً — انظر ملاحظة SDL2.dll أعلاه)
                if(EXISTS "${SDL2_TTF_BUNDLED_DLL}")
                    add_custom_command(TARGET sad_graphics POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E make_directory
                        "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/"
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${SDL2_TTF_BUNDLED_DLL}"
                        "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/"
                        COMMENT "نسخ SDL2_ttf.dll إلى مجلد الإخراج"
                    )
                endif()
            else()
                message(STATUS "  [sad_graphics] ⚠ SDL2_ttf غير مُضمن — عرض النصوص معطّل")
            endif()
        else()
            message(STATUS "  [sad_graphics] ⚠ SDL2 غير موجود — سيتم التجميع بدون عرض رسومي / Not found — compiling without graphical rendering")
            message(STATUS "    المتوقع: ${SDL2_BUNDLED_LIB}")
        endif()
    endif()
endif()

# ══════════════════════════════════════════════════════════════════════════════
# دعم المنصات المتعددة عبر sdl2_platforms.cmake
# ══════════════════════════════════════════════════════════════════════════════

# تضمين ملف إعداد SDL2 للمنصات المتعددة (إذا لم يكن سطح المكتب)
if(NOT WIN32 AND NOT UNIX OR EMSCRIPTEN OR ANDROID OR IOS)
    include(${CMAKE_SOURCE_DIR}/cmake/sdl2_platforms.cmake OPTIONAL RESULT_VARIABLE SDLPlatformsIncluded)
    if(SDLPlatformsIncluded)
        message(STATUS "  [sad_graphics] ✓ تم تحميل sdl2_platforms.cmake")
        # إعداد SDL2 للمنصة الحالية
        setup_sdl2_for_platform(sad_graphics)
        add_platform_compile_options(sad_graphics)
    endif()
endif()

# ── تعريفات خاصة بنوع الواجهة الهجينة ──
target_compile_definitions(sad_graphics PUBLIC
    SAD_UI_HYBRID_ROUTING  # تفعيل التوجيه الهجين
)

# ══════════════════════════════════════════════════════════════════════════════
# مكتبة الوضع الحرّ الكاملة (fb0/evdev) — بلا SDL2 / Full freestanding lib (no SDL2)
# ──────────────────────────────────────────────────────────────────────────────
# (AR) نظير sad_graphics لكن بخلفيّة الوضع الحرّ الكاملة (linuxfb/evdev/psf/app_runner)
#      وبتعريف SAD_UI_FREESTANDING دون SAD_UI_USE_SDL2 — فلا يستورد الثنائيّ SDL2.
#      المصادر اللينكسيّة (linuxfb/evdev/app_runner) محروسة بـ__linux__ فتُترجَم
#      فارغةً على غير لينكس ⇒ الهدف يُضاف بأمان في كلّ المنصّات (إضافة غير كاسرة).
#      يستهلكه ربطُ sad-build عند «--حرّ» (بديل sad_graphics+SDL2). النواة والعناصر
#      backend-agnostic فتُعاد ترجمتها هنا بلا SDL2.
# (EN) Freestanding twin of sad_graphics: full fb0/evdev backend, SAD_UI_FREESTANDING
#      and NO SAD_UI_USE_SDL2 ⇒ binary never imports SDL2. Linux sources are guarded
#      by __linux__ (compile empty elsewhere) so the target is portable/additive.
# ══════════════════════════════════════════════════════════════════════════════
option(SAD_UI_ENABLE_FREESTANDING_LIB
    "بناء مكتبة الوضع الحرّ الكاملة sad_graphics_freestanding / Build full freestanding lib" ON)

if(SAD_UI_ENABLE_FREESTANDING_LIB)
    set(SAD_UI_FREESTANDING_FULL_SOURCES
        backends/freestanding/src/renderer.cpp
        backends/freestanding/src/builtin_font.cpp
        backends/freestanding/src/arabic_shaper.cpp
        backends/freestanding/src/linuxfb.cpp
        backends/freestanding/src/evdev_input.cpp
        backends/freestanding/src/psf_font.cpp
        backends/freestanding/src/app_runner.cpp
    )

    # (AR) خلفيّة الويب مطلوبة لأنّ جسر وقت التشغيل (sad_ui_runtime) يصدّر
    #      «توليد_ويب» (HtmlCodegen) في الـABI — توليد نصّيّ backend-agnostic.
    add_library(sad_graphics_freestanding STATIC
        ${SAD_UI_CORE_SOURCES}
        ${SAD_UI_WIDGETS_SOURCES}
        ${SAD_UI_WEB_SOURCES}
        ${SAD_UI_FREESTANDING_FULL_SOURCES}
    )

    target_include_directories(sad_graphics_freestanding
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/core/include
            ${CMAKE_CURRENT_SOURCE_DIR}/backends/freestanding/include
            ${CMAKE_CURRENT_SOURCE_DIR}/backends/web/include
            ${CMAKE_CURRENT_SOURCE_DIR}/widgets/include
            ${CMAKE_CURRENT_SOURCE_DIR}/widgets/data/include
            ${CMAKE_CURRENT_SOURCE_DIR}/widgets/media/include
            ${CMAKE_CURRENT_SOURCE_DIR}/widgets/animation/include
            ${CMAKE_CURRENT_SOURCE_DIR}/widgets/controls/include
            ${CMAKE_CURRENT_SOURCE_DIR}/widgets/navigation_v2/include
            ${CMAKE_SOURCE_DIR}/features/graphics/third_party
    )

    target_compile_features(sad_graphics_freestanding PUBLIC cxx_std_17)

    # (AR) SAD_UI_FREESTANDING يُفعّل فرع fb0/evdev في sad_app_run؛ لا SAD_UI_USE_SDL2.
    target_compile_definitions(sad_graphics_freestanding PUBLIC
        SAD_UI_FREESTANDING
        SAD_UI_HYBRID_ROUTING
    )

    if(MSVC)
        target_compile_options(sad_graphics_freestanding PRIVATE /utf-8 /wd4819 /FS)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(sad_graphics_freestanding PRIVATE -finput-charset=UTF-8)
    endif()

    message(STATUS "  [sad_graphics_freestanding] ✓ مكتبة الوضع الحرّ الكاملة (fb0/evdev، بلا SDL2)")
endif()

# ── رسالة تأكيد ──────────────────────────────────────────────────

message(STATUS "  [sad_graphics] نظام واجهات ص — ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "    النواة:    ${CMAKE_CURRENT_SOURCE_DIR}/core/")
message(STATUS "    المنصات:   desktop, android, ios, web, macos")
message(STATUS "    العناصر:   text, button, image, column, row + 30 widget builders")
message(STATUS "    الهجين:    طريقة_عرض (أصلي/مرسوم/تلقائي)")
