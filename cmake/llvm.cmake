# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/llvm.cmake
# الوصف: إعدادات LLVM الاختيارية / Optional LLVM Configuration
# المسؤول: خالد — مهندس المترجم والنواة
# ═══════════════════════════════════════════════════════════════════════════════
#
# نظرة عامة:
#   هذا الملف يدير اكتشاف وربط مكتبات LLVM 18 بمشروع لغة ص (Sad Language).
#   LLVM يُستخدم حصرياً بواسطة المترجم sadc لتحويل شجرة AST إلى كود أصلي
#   عبر سلسلة: AST → SIR → LLVM IR → ملف تنفيذي.
#
#   المفسر sad.exe لا يحتاج LLVM ويعمل بشكل مستقل تماماً.
#
# المعماريات المدعومة:
#   x86_64, AArch64 (ARM64), WebAssembly, RISC-V, ARM, PowerPC,
#   AMDGPU, AVR, SPARC, وغيرها.
#
# الخيارات:
#   ENABLE_LLVM_BACKEND=ON|OFF  — تفعيل/تعطيل دعم LLVM (افتراضي: ON)
#   LLVM_DIR=<path>             — مسار مخصص لـ LLVMConfig.cmake
#
# الاستخدام:
#   cmake -B build -DENABLE_LLVM_BACKEND=ON
#   cmake --build build --config Release --target sadc
#
# ═══════════════════════════════════════════════════════════════════════════════

if(NOT ENABLE_LLVM_BACKEND)
    message(STATUS "⊘ LLVM Backend معطّل / LLVM Backend disabled")
    return()
endif()

# تحديد مسار LLVM تلقائياً إن لم يُعطَ / Auto-detect LLVM path if not provided
if(NOT LLVM_DIR)
    # جرّب المسارات الشائعة / Try common paths
    foreach(_llvm_hint
        "C:/llvm_dev/LLVM/lib/cmake/llvm"
        "C:/Program Files/LLVM/lib/cmake/llvm"
        "C:/LLVM/lib/cmake/llvm"
        "C:/LLVM-Dev/lib/cmake/llvm"
        "/usr/lib/llvm-18/lib/cmake/llvm"
        "/usr/lib/llvm-17/lib/cmake/llvm"
        "/usr/local/opt/llvm@18/lib/cmake/llvm"
        "/opt/homebrew/opt/llvm@18/lib/cmake/llvm"
    )
        if(EXISTS "${_llvm_hint}/LLVMConfig.cmake")
            set(LLVM_DIR "${_llvm_hint}" CACHE PATH "LLVM CMake directory" FORCE)
            break()
        endif()
    endforeach()
endif()

# البحث عن LLVM / Find LLVM
find_package(LLVM QUIET CONFIG)

if(NOT LLVM_FOUND)
    message(WARNING "⚠️  LLVM غير موجود. سيتم تعطيل LLVM Backend.")
    message(WARNING "⚠️  LLVM not found. LLVM Backend disabled.")
    message(WARNING "   cmake -DLLVM_DIR=C:/Program Files/LLVM/lib/cmake/llvm ..")
    set(ENABLE_LLVM_BACKEND OFF)
    set(LLVM_FOUND FALSE)
    return()
endif()

message(STATUS "✅ LLVM ${LLVM_PACKAGE_VERSION} موجود / Found")
message(STATUS "   المسار / Directory: ${LLVM_DIR}")

# ═══════════════════════════════════════════════════════════════════════════════
# فحص توافق Debug/Release بين LLVM والمشروع
# ───────────────────────────────────────────────────────────────────────────────
# المشكلة: على Windows مع MSVC، مكتبات LLVM تُبنى عادة في وضع Release.
# عند بناء مشروع لغة ص في وضع Debug، يستخدم MSVC قيمة _ITERATOR_DEBUG_LEVEL=2
# بينما مكتبات LLVM Release تستخدم _ITERATOR_DEBUG_LEVEL=0.
# هذا التعارض يؤدي إلى آلاف أخطاء الربط (linker errors) لأن الرموز
# المُزخرفة (decorated names) تختلف بين الوضعين.
#
# الحل المعتمد: نكتشف هذا التعارض مبكراً ونعرض رسالة واضحة مع الحلول.
# المترجم sadc يجب أن يُبنى دائماً في Release على Windows:
#   cmake --build build --config Release --target sadc
#
# Check Debug/Release compatibility between LLVM and project.
# On Windows+MSVC, LLVM is typically built in Release mode. When the project
# is built in Debug, _ITERATOR_DEBUG_LEVEL mismatch causes thousands of
# linker errors. We detect this early and provide clear guidance.
# ═══════════════════════════════════════════════════════════════════════════════

# متغير لتتبع حالة التوافق — يُستخدم لاحقاً عند بناء sadc
# Compatibility flag — used later when building sadc target
set(SAD_LLVM_DEBUG_COMPATIBLE TRUE)

if(MSVC)
    # ─── الخطوة 1: فحص وجود مكتبات LLVM في وضع Debug ───
    # نتحقق من عدة مسارات محتملة لأن بنية المجلدات قد تختلف
    # Check multiple possible paths for Debug LLVM libs
    set(_llvm_has_debug FALSE)
    
    # المسار الأول: بنية LLVM القياسية (lib/cmake/llvm/../../../Debug/lib)
    if(EXISTS "${LLVM_DIR}/../../../Debug/lib")
        set(_llvm_has_debug TRUE)
    endif()
    
    # المسار الثاني: مسار LLVM بديل (بجوار مجلد lib الحالي)
    get_filename_component(_llvm_root "${LLVM_DIR}/../../.." ABSOLUTE)
    if(EXISTS "${_llvm_root}/Debug/lib")
        set(_llvm_has_debug TRUE)
    endif()

    # ─── الخطوة 2: تحديد وضع البناء الفعلي ───
    # في مولدات متعددة الإعدادات (مثل Visual Studio)، CMAKE_BUILD_TYPE فارغ
    # لذلك نتحقق من CMAKE_CONFIGURATION_TYPES أيضاً
    # For multi-config generators, CMAKE_BUILD_TYPE is empty at configure time
    set(_is_debug_possible FALSE)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_is_debug_possible TRUE)
    elseif(NOT CMAKE_BUILD_TYPE AND CMAKE_CONFIGURATION_TYPES)
        # مولد متعدد الإعدادات — Debug محتمل في وقت البناء
        set(_is_debug_possible TRUE)
    endif()

    # ─── الخطوة 3: إصدار التحذير المناسب ───
    if(_is_debug_possible AND NOT _llvm_has_debug)
        set(SAD_LLVM_DEBUG_COMPATIBLE FALSE)
        message(WARNING "")
        message(WARNING "╔══════════════════════════════════════════════════════════════╗")
        message(WARNING "║  ⚠️  تعارض Debug/Release بين LLVM والمشروع                 ║")
        message(WARNING "║  ⚠️  LLVM Debug/Release Mismatch Detected                   ║")
        message(WARNING "╠══════════════════════════════════════════════════════════════╣")
        message(WARNING "║                                                              ║")
        message(WARNING "║  LLVM مبني في Release لكن المشروع قد يُبنى في Debug.        ║")
        message(WARNING "║  هذا سيسبب ~3900 خطأ ربط بسبب _ITERATOR_DEBUG_LEVEL.       ║")
        message(WARNING "║                                                              ║")
        message(WARNING "║  LLVM is Release-built but project may build in Debug.       ║")
        message(WARNING "║  This causes ~3900 linker errors from iterator mismatch.     ║")
        message(WARNING "║                                                              ║")
        message(WARNING "╠══════════════════════════════════════════════════════════════╣")
        message(WARNING "║  الحلول / Solutions:                                         ║")
        message(WARNING "║  ① ابنِ sadc في Release (الأسهل والأسرع):                   ║")
        message(WARNING "║     cmake --build build --config Release --target sadc       ║")
        message(WARNING "║  ② ابنِ LLVM من المصدر في Debug (بطيء لكن شامل):            ║")
        message(WARNING "║     cmake -DCMAKE_BUILD_TYPE=Debug ../llvm                   ║")
        message(WARNING "╚══════════════════════════════════════════════════════════════╝")
        message(WARNING "")
    endif()
    
    # تنظيف المتغيرات المؤقتة
    unset(_llvm_has_debug)
    unset(_llvm_root)
    unset(_is_debug_possible)
endif()

# إضافة تعريفات LLVM / Add LLVM definitions
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_compile_definitions(ENABLE_LLVM_BACKEND)

# مكتبات LLVM المطلوبة / Required LLVM libraries
set(LLVM_LINK_COMPONENTS
    Core Support ExecutionEngine MCJIT OrcJIT RuntimeDyld Target
    X86 AArch64 AMDGPU ARM AVR BPF Hexagon Lanai LoongArch
    Mips MSP430 NVPTX PowerPC RISCV Sparc SystemZ VE
    WebAssembly XCore native MC CodeGen AsmParser AsmPrinter
)

# On Linux with shared LLVM, link to the single shared library (includes all targets)
# On Windows/static builds, use individual component libraries
if(EXISTS "${LLVM_LIBRARY_DIRS}/libLLVM-${LLVM_VERSION_MAJOR}.so")
    set(LLVM_LIBS LLVM-${LLVM_VERSION_MAJOR})
    message(STATUS "   Using shared LLVM library: libLLVM-${LLVM_VERSION_MAJOR}.so")
else()
    llvm_map_components_to_libnames(LLVM_LIBS ${LLVM_LINK_COMPONENTS})
    message(STATUS "   LLVM libs count: ${LLVM_LIBS}")
endif()

# ═══════════════════════════════════════════════════════════════════════════════
# اكتشاف مكتبات LLD للرابط المدمج / LLD Library Discovery for Embedded Linker
# ═══════════════════════════════════════════════════════════════════════════════
# LLD هو رابط LLVM الذي يدعم صيغ COFF (Windows)، ELF (Linux)، Mach-O (macOS)، 
# MinGW، و WebAssembly. عند تضمينه في sadc، يصبح المترجم مكتفياً ذاتياً
# بدون الحاجة لرابط خارجي (clang أو link.exe).
#
# LLD is the LLVM linker supporting COFF, ELF, Mach-O, MinGW, and WebAssembly.
# When embedded in sadc, the compiler becomes self-contained without needing
# an external linker (clang or link.exe).
# ═══════════════════════════════════════════════════════════════════════════════
set(LLD_LIBS "")
set(HAS_EMBEDDED_LLD FALSE)

set(_lld_components lldCOFF lldCommon lldELF lldMachO lldMinGW lldWasm)
set(_lld_all_found TRUE)

foreach(_lld_comp ${_lld_components})
    find_library(_lld_lib_${_lld_comp}
        NAMES ${_lld_comp}
        PATHS ${LLVM_LIBRARY_DIRS}
        NO_DEFAULT_PATH
    )
    if(_lld_lib_${_lld_comp})
        list(APPEND LLD_LIBS ${_lld_lib_${_lld_comp}})
    else()
        set(_lld_all_found FALSE)
        message(STATUS "   ⊘ LLD component not found: ${_lld_comp}")
    endif()
endforeach()

# (AR) مكتبات LLVM الإضافية اللازمة لـ LLD
# (EN) Additional LLVM libraries required by LLD
if(_lld_all_found)
    set(_lld_llvm_deps LLVMLTO LLVMLibDriver LLVMOption LLVMLinker)
    foreach(_dep ${_lld_llvm_deps})
        find_library(_lld_dep_${_dep}
            NAMES ${_dep}
            PATHS ${LLVM_LIBRARY_DIRS}
            NO_DEFAULT_PATH
        )
        if(_lld_dep_${_dep})
            list(APPEND LLD_LIBS ${_lld_dep_${_dep}})
        else()
            set(_lld_all_found FALSE)
            message(STATUS "   ⊘ LLD LLVM dependency not found: ${_dep}")
        endif()
    endforeach()
endif()

if(_lld_all_found)
    set(HAS_EMBEDDED_LLD TRUE)
    message(STATUS "✅ LLD موجود — الرابط المدمج مفعّل / LLD found — embedded linker enabled")
    message(STATUS "   LLD libs: ${LLD_LIBS}")
else()
    message(STATUS "⊘ LLD غير مكتمل — سيتم استخدام رابط خارجي / LLD incomplete — external linker")
endif()

unset(_lld_all_found)
unset(_lld_components)
