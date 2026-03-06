# ═══════════════════════════════════════════════════════════════════════════════
#                نظام بناء وقت التشغيل — لغة ص لأندرويد
# ═══════════════════════════════════════════════════════════════════════════════
#
# يبني مكتبة libsad_android_runtime.so (أو .a) من ملفات C النقية.
#
# الأهداف (Targets):
# ────────────────
#   sad_android_runtime_shared → libsad_android_runtime.so  (للتطبيقات)
#   sad_android_runtime_static → libsad_android_runtime.a   (للمترجم)
#
# البناء للأندرويد:
# ─────────────────
#   cmake -S . -B build-android \
#     -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake \
#     -DANDROID_ABI=arm64-v8a \
#     -DANDROID_PLATFORM=android-24 \
#     -DSAD_ANDROID_BUILD=ON
#
# البناء للحاسوب (اختبار):
# ──────────────────────
#   cmake -S . -B build-host
#   cmake --build build-host
#
# ═══════════════════════════════════════════════════════════════════════════════

cmake_minimum_required(VERSION 3.15)

project(SadAndroidRuntime
    VERSION     1.0.0
    LANGUAGES   C
    DESCRIPTION "وقت تشغيل لغة ص لأندرويد — Sad Android Runtime"
)

# ─────────────────────────────────────────
# §١ — خيارات البناء
# ─────────────────────────────────────────

# هل نبني لأندرويد؟ (يُكتشف تلقائياً أو يُحدد يدوياً)
option(SAD_ANDROID_BUILD   "بناء لنظام أندرويد"           OFF)
option(SAD_ENABLE_TLS      "تفعيل دعم TLS/SSL"            ON)
option(SAD_BUILD_TESTS     "بناء الاختبارات"               OFF)
option(SAD_BUILD_SHARED    "بناء مكتبة ديناميكية (.so)"   ON)
option(SAD_BUILD_STATIC    "بناء مكتبة ساكنة (.a)"        ON)

# اكتشاف أندرويد تلقائياً
if(ANDROID OR CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(SAD_ANDROID_BUILD ON)
    message(STATUS "  [ص] اكتُشف نظام أندرويد — ABI: ${ANDROID_ABI}")
endif()

# معيار C
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ─────────────────────────────────────────
# §٢ — ملفات المصدر
# ─────────────────────────────────────────

set(SAD_RUNTIME_SOURCES
    # إدارة الذاكرة — Arena + Pool + RC
    sad_android_memory.c

    # النصوص — UTF-8 + SSO + عمليات عربية
    sad_android_string.c

    # المصفوفات الديناميكية
    sad_android_array.c

    # الخرائط — Robin Hood Hashing
    sad_android_map.c

    # نظام القيم الموحّد
    sad_android_value.c

    # الخيوط والقنوات ومجمّع الخيوط
    sad_android_thread.c

    # الشبكات — TCP, TLS, WebSocket, HTTP
    sad_android_network.c

    # الإدخال/الإخراج والملفات
    sad_android_io.c
)

set(SAD_RUNTIME_HEADERS
    sad_android_memory.h
    sad_android_string.h
    sad_android_array.h
    sad_android_map.h
    sad_android_value.h
    sad_android_thread.h
    sad_android_network.h
    sad_android_io.h
)

# ─────────────────────────────────────────
# §٣ — أعلام المُترجم
# ─────────────────────────────────────────

set(SAD_COMPILE_OPTIONS
    -Wall
    -Wextra
    -Wpedantic
    -Wno-unused-parameter
    -fPIC                      # ضروري للمكتبات المشتركة
    -fvisibility=hidden        # إخفاء الرموز افتراضياً (أداء أفضل)
)

# أعلام خاصة بأندرويد
if(SAD_ANDROID_BUILD)
    list(APPEND SAD_COMPILE_OPTIONS
        -D__ANDROID__
        -DSAD_PLATFORM_ANDROID=1
    )
endif()

# أعلام TLS
if(SAD_ENABLE_TLS)
    list(APPEND SAD_COMPILE_OPTIONS -DHAVE_OPENSSL=1)
endif()

# أعلام التصحيح
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND SAD_COMPILE_OPTIONS
        -g
        -O0
        -DSAD_DEBUG=1
        -fsanitize=address       # كشف أخطاء الذاكرة
    )
else()
    list(APPEND SAD_COMPILE_OPTIONS
        -O2
        -DNDEBUG
    )
endif()

# ─────────────────────────────────────────
# §٤ — المكتبات المطلوبة
# ─────────────────────────────────────────

set(SAD_LINK_LIBRARIES
    pthread           # الخيوط (POSIX threads)
)

if(SAD_ANDROID_BUILD)
    list(APPEND SAD_LINK_LIBRARIES
        log            # __android_log_print (logcat)
        android        # NativeActivity, AAssetManager
    )
endif()

# TLS/SSL
if(SAD_ENABLE_TLS)
    if(SAD_ANDROID_BUILD)
        # أندرويد يتضمن BoringSSL
        list(APPEND SAD_LINK_LIBRARIES ssl crypto)
    else()
        # البحث عن OpenSSL على الحاسوب
        find_package(OpenSSL QUIET)
        if(OpenSSL_FOUND)
            list(APPEND SAD_LINK_LIBRARIES OpenSSL::SSL OpenSSL::Crypto)
            message(STATUS "  [ص] OpenSSL مُكتشف: ${OPENSSL_VERSION}")
        else()
            message(WARNING "  [ص] OpenSSL غير موجود — TLS معطّل")
            set(SAD_ENABLE_TLS OFF)
            list(REMOVE_ITEM SAD_COMPILE_OPTIONS -DHAVE_OPENSSL=1)
        endif()
    endif()
endif()

# ─────────────────────────────────────────
# §٥ — المكتبة المشتركة (.so)
# ─────────────────────────────────────────

if(SAD_BUILD_SHARED)
    add_library(sad_android_runtime_shared SHARED
        ${SAD_RUNTIME_SOURCES}
    )

    target_compile_options(sad_android_runtime_shared PRIVATE
        ${SAD_COMPILE_OPTIONS}
    )

    target_link_libraries(sad_android_runtime_shared PRIVATE
        ${SAD_LINK_LIBRARIES}
    )

    set_target_properties(sad_android_runtime_shared PROPERTIES
        OUTPUT_NAME     "sad_android_runtime"
        VERSION         ${PROJECT_VERSION}
        SOVERSION       1
        C_VISIBILITY_PRESET hidden
        POSITION_INDEPENDENT_CODE ON
    )

    target_include_directories(sad_android_runtime_shared
        PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}
    )

    # على أندرويد: وضع .so في مجلد libs/
    if(SAD_ANDROID_BUILD)
        set_target_properties(sad_android_runtime_shared PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/libs/${ANDROID_ABI}"
        )
    endif()

    message(STATUS "  [ص] هدف: sad_android_runtime_shared (.so)")
endif()

# ─────────────────────────────────────────
# §٦ — المكتبة الساكنة (.a)
# ─────────────────────────────────────────

if(SAD_BUILD_STATIC)
    add_library(sad_android_runtime_static STATIC
        ${SAD_RUNTIME_SOURCES}
    )

    target_compile_options(sad_android_runtime_static PRIVATE
        ${SAD_COMPILE_OPTIONS}
    )

    target_link_libraries(sad_android_runtime_static PRIVATE
        ${SAD_LINK_LIBRARIES}
    )

    set_target_properties(sad_android_runtime_static PROPERTIES
        OUTPUT_NAME "sad_android_runtime"
    )

    target_include_directories(sad_android_runtime_static
        PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}
    )

    message(STATUS "  [ص] هدف: sad_android_runtime_static (.a)")
endif()

# ─────────────────────────────────────────
# §٧ — ملخص البناء
# ─────────────────────────────────────────

message(STATUS "")
message(STATUS "  ╔═══════════════════════════════════════╗")
message(STATUS "  ║   وقت تشغيل لغة ص — ملخص البناء     ║")
message(STATUS "  ╠═══════════════════════════════════════╣")
message(STATUS "  ║  النظام:   ${CMAKE_SYSTEM_NAME}")
message(STATUS "  ║  المعالج:  ${CMAKE_SYSTEM_PROCESSOR}")
if(SAD_ANDROID_BUILD)
message(STATUS "  ║  ABI:      ${ANDROID_ABI}")
message(STATUS "  ║  API:      ${ANDROID_PLATFORM}")
endif()
message(STATUS "  ║  TLS:      ${SAD_ENABLE_TLS}")
message(STATUS "  ║  النوع:    ${CMAKE_BUILD_TYPE}")
message(STATUS "  ║  ملفات:    ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "  ╚═══════════════════════════════════════╝")
message(STATUS "")
