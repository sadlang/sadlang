# ============================================================================
# shared/security/CMakeLists.txt
# (AR) مكتبة sad_security_core المشتركة بين المترجم/المفسر/VM/runtime.
# (EN) sad_security_core: shared security primitives (compiler/interpreter/VM/runtime).
# ============================================================================
#
# (AR) المحتوى:
#   - BoundsChecker     (header-only) — فحص حدود وذاكرة
#   - SafeArithmetic    (header-only, templates) — حساب بدون فيض
#   - InputSanitizer    (.cpp) — تعقيم أوامر/مسارات/SQL/HTML/أعداد
#   - SafeAllocator     (.cpp) — تخصيص آمن + AllocationTracker
#   - TaintTracker      (.cpp) — تتبع تلوث ساكن/ديناميكي + C ABI
# (EN) Components:
#   - BoundsChecker (header-only) — bounds & memory checks
#   - SafeArithmetic (header-only, templates) — overflow-free arithmetic
#   - InputSanitizer (.cpp) — command/path/SQL/HTML/number sanitizers
#   - SafeAllocator (.cpp) — safe alloc + AllocationTracker
#   - TaintTracker (.cpp) — static/dynamic taint tracking + C ABI
# ============================================================================

set(SAD_SECURITY_SOURCES
    src/input_sanitizer.cpp
    src/safe_allocator.cpp
    src/taint_tracker.cpp
)

add_library(sad_security_core STATIC ${SAD_SECURITY_SOURCES})

target_include_directories(sad_security_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# (AR) إعدادات MSVC: UTF-8 + multi-process pdb safety
# (EN) MSVC settings: UTF-8 source files + safe parallel pdb writes
if(MSVC)
    target_compile_options(sad_security_core PRIVATE /utf-8 /FS /wd4819)
endif()

# (AR) معايير C++ المتسقة مع باقي المشروع
# (EN) C++ standard consistent with the rest of the project
target_compile_features(sad_security_core PUBLIC cxx_std_17)

message(STATUS "Configured sad_security_core (shared security layer)")
