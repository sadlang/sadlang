# ═══════════════════════════════════════════════════════════════════════════════════
# مكتبة محرّك جامع القمامة الموحَّدة لـ shared/memory_gc/
# Unified Garbage Collector engine library for the Sad language
# ═══════════════════════════════════════════════════════════════════════════════════
#
# (AR) تحوي هذه المكتبة المكونات الموحَّدة لجامع القمامة (GC) المستخدمة عبر
#      المسارات الثلاثة (مفسر / مترجم / VM):
#        - cycles/: كاشف دورات المراجع (CycleDetector)
#        - (مستقبلاً B2/B3) gc/: محرك GC الفعلي + سياسات الجمع + جدولة الدورات
#
#      كانت ملفات cycle_detector تسكن compiler/{include,src}/memory/ بدون
#      مستهلك خارجي، فنُقلت إلى shared/ تحضيراً للربط مع المفسر والـ VM.
#
# (EN) Centralised library for shared GC components consumed by interpreter,
#      compiler, and VM. Currently exports the cycle detector; future phases
#      will add the GC engine itself, collection policies, and scheduling.
#
# المراحل:
#   B-step1 (مكتمل): نقل cycle_detector.* + حذف gc_fallback_mode.cpp اليتيم
#   B-step2 (مكتمل): استخراج محرك GC إلى engine/garbage_collector.{h,cpp}
#   B-step3 (قيد):   ربط المفسر والـ VM بالمحرك الموحَّد
# ═══════════════════════════════════════════════════════════════════════════════════

set(SAD_MEMORY_GC_SOURCES
    src/cycles/cycle_detector.cpp
    src/cycles/cycle_detector_impl.cpp
    src/engine/garbage_collector.cpp
)

add_library(sad_memory_gc STATIC ${SAD_MEMORY_GC_SOURCES})

target_include_directories(sad_memory_gc PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(sad_memory_gc PRIVATE /utf-8 /FS /wd4819)
endif()

target_compile_features(sad_memory_gc PUBLIC cxx_std_17)

# (AR) لا اعتماديات داخلية حالياً — CycleDetector و GarbageCollector يستخدمان STL فقط.
# (EN) No internal dependencies yet — CycleDetector and GarbageCollector use only the STL. uses only the STL.
