# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/platform.cmake
# الوصف: إعدادات المنصة والمترجم / Platform & Compiler Configuration
# ═══════════════════════════════════════════════════════════════════════════════

# ──────────────────────────────────────────────────────────────────────
# تعريفات المعالج المسبق / Preprocessor Definitions
# ──────────────────────────────────────────────────────────────────────
if(ENABLE_DEBUG_LOGGING)
    add_compile_definitions(SAD_DEBUG_ENABLED)
endif()

# دعم UTF-8 على Windows / UTF-8 support on Windows
if(WIN32)
    add_compile_definitions(_UNICODE UNICODE NOMINMAX)
    add_compile_options(/utf-8)
endif()

# ──────────────────────────────────────────────────────────────────────
# خيارات التحذيرات / Warning Options
# ──────────────────────────────────────────────────────────────────────
if(MSVC)
    add_compile_options(
        /W3 /WX- /permissive- /FS
        # تعطيل التحذيرات الشائعة من LLVM وغيرها
        /wd4100  # unreferenced parameter
        /wd4244  # conversion
        /wd4267  # size_t conversion
        /wd4324  # structure padding
        /wd4458  # declaration hides member
        /wd4624  # destructor implicitly deleted
        /wd4245  # signed/unsigned mismatch
        /wd4127  # conditional expression is constant
        /wd4189  # local variable not referenced
        /wd4146  # unary minus on unsigned
        /wd4310  # cast truncates constant
    )
else()
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter
    )
endif()
