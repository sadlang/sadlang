# ═══════════════════════════════════════════════════════════════════════════════════
# مكتبة سياسة الذاكرة الموحَّدة لـ shared/memory_policy/
# Unified Memory Policy library for the Sad language
# ═══════════════════════════════════════════════════════════════════════════════════
#
# (AR) تجمع هذه المكتبة كل ما يخص اختيار وإدارة وضع الذاكرة:
#      - أوضاع المطور: --dev / --prod / --learn / --hybrid / --auto
#      - استراتيجيات GC: refcount / atomic / tracing / incremental / none
#      - مستويات الملكية: off / warnings / strict / ultra
#      - تحليل CLI flags + متغيرات البيئة + ملفات الإعداد
#
#      كانت هذه الملفات تسكن compiler/{include,src}/memory/ بشكل معزول لا يصل
#      إليها المفسر ولا الـ VM، مما خلق خرقاً دلالياً (راجع docs/معمارية_الذاكرة_الموحدة.md).
#      نُقلت إلى shared/ لتوحيد سياسة اللغة عبر جميع المسارات.
#
# (EN) Centralised library for the language's memory mode policy. Previously
#      isolated under compiler/memory/, now lives in shared/ so interpreter,
#      compiler, and VM all consult the same source of truth.
# ═══════════════════════════════════════════════════════════════════════════════════

set(SAD_MEMORY_POLICY_SOURCES
    src/gc_mode.cpp
    src/memory_mode_flag.cpp
)

add_library(sad_memory_policy STATIC ${SAD_MEMORY_POLICY_SOURCES})

target_include_directories(sad_memory_policy PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(sad_memory_policy PRIVATE /utf-8 /FS /wd4819)
endif()

target_compile_features(sad_memory_policy PUBLIC cxx_std_17)
