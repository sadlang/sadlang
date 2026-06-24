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

# ──────────────────────────────────────────────────────────────────────
# الـlinker الكلاسيكي على macOS / Classic linker on macOS
# ──────────────────────────────────────────────────────────────────────
# (AR) السبب الجذري لانهيارات SIGTRAP/EXC_BREAKPOINT العشوائية أثناء C++
#      exception unwinding (داخل _Unwind_GetIP في libunwind.dylib) على
#      macOS/arm64 تحديداً: الـlinker الجديد ("ld-prime") الذي اعتمدته
#      Apple كافتراضي ابتداءً من Xcode 15 لديه علّة معروفة وموثَّقة من
#      عدة مشاريع مفتوحة المصدر (مثل TRIQS/triqs#945) تتعلق بكيفية
#      تنظيمه لمعلومات unwind/eh_frame على ARM64 — تتفاقم تحديداً عند
#      الربط مع مكتبات LLVM/clang إضافية (كما نفعل هنا عبر "llvm@18"
#      من Homebrew لباكند LLVM). الحل الموثَّق والمؤكَّد: فرض الـlinker
#      القديم "ld_classic" بدل الجديد. هذا الخيار سيُزال في إصدارات
#      Xcode أحدث من 15.x (راجع ملاحظات إصدار Xcode 16)، فقد يحتاج
#      لإعادة تقييم عند ترقية صورة الـCI macOS مستقبلاً.
# (EN) Root cause of random SIGTRAP/EXC_BREAKPOINT crashes during C++
#      exception unwinding (inside _Unwind_GetIP in libunwind.dylib)
#      specifically on macOS/arm64: the new linker ("ld-prime") that
#      Apple made the default starting with Xcode 15 has a known bug,
#      documented across several open-source projects (e.g.
#      TRIQS/triqs#945), in how it lays out unwind/eh_frame info on
#      ARM64 — it's aggravated specifically when linking against extra
#      LLVM/clang libraries (as we do here via Homebrew's "llvm@18" for
#      the LLVM backend). The documented, confirmed fix: force the
#      classic linker "ld_classic" instead of the new one. This flag is
#      slated for removal in Xcode versions newer than 15.x (see the
#      Xcode 16 release notes), so it may need revisiting whenever the
#      CI macOS image is upgraded past Xcode 15.
if(APPLE)
    add_link_options(-Wl,-ld_classic)
endif()
