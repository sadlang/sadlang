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
        "C:/Program Files/LLVM/lib/cmake/llvm"
        "C:/LLVM/lib/cmake/llvm"
        "C:/LLVM-Dev/lib/cmake/llvm"
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

llvm_map_components_to_libnames(LLVM_LIBS ${LLVM_LINK_COMPONENTS})
message(STATUS "   LLVM libs count: ${LLVM_LIBS}")
