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

# ═══════════════════════════════════════════════════════════════════════════════
# (AR) اكتشاف ثنائي Debug/Release لمكتبات LLVM
# ───────────────────────────────────────────────────────────────────────────────
# على MSVC، مكتبات LLVM Release تستخدم /MD و Debug تستخدم /MDd.
# خلط الأنظمة يسبب آلاف أخطاء _ITERATOR_DEBUG_LEVEL.
# الحل: نبحث عن تثبيتين منفصلين (Release + Debug) ونربط حسب الإعداد.
#
# (EN) Dual Debug/Release LLVM library detection
# On MSVC, Release LLVM uses /MD and Debug uses /MDd.
# Mixing causes thousands of _ITERATOR_DEBUG_LEVEL errors.
# Solution: detect separate installations and link per-configuration.
# ═══════════════════════════════════════════════════════════════════════════════

set(SAD_LLVM_DEBUG_COMPATIBLE TRUE)
set(SAD_LLVM_HAS_DEBUG FALSE)
set(SAD_LLVM_DEBUG_LIB_DIR "")

if(MSVC)
    # (AR) البحث عن تثبيت LLVM Debug في مسارات معروفة
    # (EN) Look for Debug LLVM installation in known paths
    get_filename_component(_llvm_root "${LLVM_DIR}/../../.." ABSOLUTE)

    # (AR) المسار 1: مجلد LLVM-Debug بجوار LLVM الأصلي (مثال: C:/llvm_dev/LLVM-Debug)
    get_filename_component(_llvm_parent "${_llvm_root}/.." ABSOLUTE)
    set(_debug_search_paths
        "${_llvm_parent}/LLVM-Debug/lib/cmake/llvm"
        "${_llvm_root}-Debug/lib/cmake/llvm"
        "C:/llvm_dev/LLVM-Debug/lib/cmake/llvm"
        "C:/LLVM-Debug/lib/cmake/llvm"
    )

    foreach(_debug_hint ${_debug_search_paths})
        if(EXISTS "${_debug_hint}/LLVMConfig.cmake")
            get_filename_component(_debug_lib_dir "${_debug_hint}/../../.." ABSOLUTE)
            set(SAD_LLVM_DEBUG_LIB_DIR "${_debug_lib_dir}/lib")
            # (AR) مسار include لـ LLVM Debug — يحتوي على abi-breaking.h بقيمة مختلفة
            # (EN) Include path for LLVM Debug — contains abi-breaking.h with different value
            set(SAD_LLVM_DEBUG_INCLUDE_DIR "${_debug_lib_dir}/include")
            set(SAD_LLVM_HAS_DEBUG TRUE)
            message(STATUS "   LLVM Debug found: ${_debug_lib_dir}")
            message(STATUS "   LLVM Debug include: ${SAD_LLVM_DEBUG_INCLUDE_DIR}")
            break()
        endif()
    endforeach()

    unset(_llvm_root)
    unset(_llvm_parent)
    unset(_debug_search_paths)

    # (AR) إذا لم يُوجد Debug LLVM، نعرض تحذير
    # (EN) If no Debug LLVM found, show warning
    if(NOT SAD_LLVM_HAS_DEBUG)
        set(SAD_LLVM_DEBUG_COMPATIBLE FALSE)
        message(WARNING "")
        message(WARNING "=== LLVM Debug/Release ===")
        message(WARNING "  LLVM Release found but no Debug installation detected.")
        message(WARNING "  sadc can only be built in Release mode.")
        message(WARNING "  To enable Debug builds:")
        message(WARNING "    1. Build LLVM Debug from source")
        message(WARNING "    2. Install to C:/llvm_dev/LLVM-Debug")
        message(WARNING "    3. Re-run cmake to auto-detect")
        message(WARNING "  Or build sadc in Release only:")
        message(WARNING "    cmake --build build --config Release --target sadc")
        message(WARNING "")
    endif()
endif()

# (AR) إضافة تعريفات ومسارات LLVM / (EN) Add LLVM definitions and paths
add_definitions(${LLVM_DEFINITIONS})
link_directories(${LLVM_LIBRARY_DIRS})
add_compile_definitions(ENABLE_LLVM_BACKEND)

# ═══════════════════════════════════════════════════════════════════════════════
# (AR) متغير LLVM include ثنائي الإعداد — للاستخدام مع target_include_directories
# ───────────────────────────────────────────────────────────────────────────────
# المشكلة: ملف abi-breaking.h في LLVM Debug يُعرّف LLVM_ENABLE_ABI_BREAKING_CHECKS=1
#          بينما في LLVM Release يُعرّفها =0. عدم التوافق يُسبب LNK2038 errors.
# الحل: نستخدم generator expression لاختيار مسار include الصحيح حسب إعداد البناء.
#
# (EN) Dual-config LLVM include variable — for use with target_include_directories
# Problem: abi-breaking.h in LLVM Debug defines LLVM_ENABLE_ABI_BREAKING_CHECKS=1
#          while in LLVM Release it's =0. Mismatch causes LNK2038 errors.
# Solution: Use generator expression to select correct include path per build config.
# ═══════════════════════════════════════════════════════════════════════════════
if(SAD_LLVM_HAS_DEBUG AND MSVC)
    # (AR) اختيار مسار include حسب إعداد البناء (Debug أو Release)
    # (EN) Select include path based on build configuration (Debug or Release)
    set(SAD_LLVM_INCLUDES
        $<$<CONFIG:Debug>:${SAD_LLVM_DEBUG_INCLUDE_DIR}>
        $<$<NOT:$<CONFIG:Debug>>:${LLVM_INCLUDE_DIRS}>
    )
    message(STATUS "   LLVM dual-config includes: Debug=${SAD_LLVM_DEBUG_INCLUDE_DIR}, Release=${LLVM_INCLUDE_DIRS}")
else()
    # (AR) استخدام مسار LLVM Release فقط
    # (EN) Use LLVM Release include path only
    set(SAD_LLVM_INCLUDES ${LLVM_INCLUDE_DIRS})
endif()

# (AR) ملاحظة: لا نستخدم include_directories() لأنها لا تدعم generator expressions.
#      بدلاً من ذلك، يجب استخدام target_include_directories(target PRIVATE ${SAD_LLVM_INCLUDES})
#      في كل هدف يستخدم LLVM headers.
# (EN) Note: We don't use include_directories() as it doesn't support generator expressions.
#      Instead, use target_include_directories(target PRIVATE ${SAD_LLVM_INCLUDES})
#      for each target that uses LLVM headers.

# (AR) مكتبات LLVM المطلوبة / (EN) Required LLVM libraries
set(LLVM_LINK_COMPONENTS
    Core Support ExecutionEngine MCJIT OrcJIT RuntimeDyld Target
    X86 AArch64 AMDGPU ARM AVR BPF Hexagon Lanai LoongArch
    Mips MSP430 NVPTX PowerPC RISCV Sparc SystemZ VE
    WebAssembly XCore native MC CodeGen AsmParser AsmPrinter
)

# (AR) على لينكس: ربط بمكتبة LLVM المشتركة الواحدة
# (EN) On Linux with shared LLVM, link to single shared lib
if(EXISTS "${LLVM_LIBRARY_DIRS}/libLLVM-${LLVM_VERSION_MAJOR}.so")
    set(LLVM_LIBS LLVM-${LLVM_VERSION_MAJOR})
    message(STATUS "   Using shared LLVM library: libLLVM-${LLVM_VERSION_MAJOR}.so")
else()
    llvm_map_components_to_libnames(LLVM_LIBS ${LLVM_LINK_COMPONENTS})
    message(STATUS "   LLVM libs count: ${LLVM_LIBS}")
endif()

# ═══════════════════════════════════════════════════════════════════════════════
# (AR) بناء قائمة المكتبات ثنائية الإعداد (Debug + Release)
# ───────────────────────────────────────────────────────────────────────────────
# عند توفر تثبيت Debug و Release معاً، نستخدم كلمات CMake الخاصة
# (debug/optimized) لربط المكتبة الصحيحة حسب إعداد البناء.
# هذا يسمح لـ sadc بالبناء في كلا الوضعين Debug و Release.
#
# (EN) Build dual-config library list (Debug + Release)
# When both Debug and Release LLVM are available, use CMake's
# debug/optimized keywords to link the right libs per config.
# This allows sadc to build in both Debug and Release.
# ═══════════════════════════════════════════════════════════════════════════════
if(SAD_LLVM_HAS_DEBUG AND MSVC)
    set(LLVM_LIBS_DUAL "")
    foreach(_lib ${LLVM_LIBS})
        # (AR) استخراج اسم الملف فقط (بدون المسار) لبناء المسار الكامل
        # (EN) Extract filename only to build full path for each config
        get_filename_component(_lib_name "${_lib}" NAME)
        if("${_lib_name}" STREQUAL "")
            set(_lib_name "${_lib}")
        endif()
        # (AR) تطبيع الامتداد إلى .lib دائماً لأن بعض توزيعات LLVM
        #      قد تُرجع أسماء بعناصر .obj في قائمة المكونات.
        #      الربط النهائي في MSVC يحتاج ملفات مكتبة .lib.
        # (EN) Normalize extension to .lib because some LLVM distributions
        #      may expose component names with .obj suffixes.
        #      MSVC final linking must use .lib archives.
        get_filename_component(_lib_stem "${_lib_name}" NAME_WE)
        set(_lib_name "${_lib_stem}.lib")
        # (AR) إضافة debug lib من مسار Debug و optimized lib من مسار Release
        list(APPEND LLVM_LIBS_DUAL
            debug "${SAD_LLVM_DEBUG_LIB_DIR}/${_lib_name}"
            optimized "${_lib}"
        )
    endforeach()
    set(LLVM_LIBS ${LLVM_LIBS_DUAL})
    message(STATUS "   LLVM dual-config: Debug + Release libs linked per configuration")
    unset(LLVM_LIBS_DUAL)
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
        message(STATUS "   LLD component not found: ${_lld_comp}")
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
            message(STATUS "   LLD LLVM dependency not found: ${_dep}")
        endif()
    endforeach()
endif()

if(_lld_all_found)
    set(HAS_EMBEDDED_LLD TRUE)
    message(STATUS "   LLD found - embedded linker enabled")
    message(STATUS "   LLD libs: ${LLD_LIBS}")
else()
    message(STATUS "   LLD incomplete - will use external linker")
endif()

# ═══════════════════════════════════════════════════════════════════════════════
# (AR) بناء قائمة LLD ثنائية الإعداد (Debug + Release)
# (EN) Build dual-config LLD library list (Debug + Release)
# ═══════════════════════════════════════════════════════════════════════════════
if(SAD_LLVM_HAS_DEBUG AND MSVC AND HAS_EMBEDDED_LLD)
    set(LLD_LIBS_DUAL "")
    foreach(_lib ${LLD_LIBS})
        get_filename_component(_lib_name "${_lib}" NAME)
        if("${_lib_name}" STREQUAL "")
            set(_lib_name "${_lib}")
        endif()
        # (AR) نفس التطبيع هنا لضمان المسارات الثنائية Debug/Release
        # (EN) Same normalization here to keep dual-config paths consistent
        get_filename_component(_lib_stem "${_lib_name}" NAME_WE)
        set(_lib_name "${_lib_stem}.lib")
        list(APPEND LLD_LIBS_DUAL
            debug "${SAD_LLVM_DEBUG_LIB_DIR}/${_lib_name}"
            optimized "${_lib}"
        )
    endforeach()
    set(LLD_LIBS ${LLD_LIBS_DUAL})
    message(STATUS "   LLD dual-config: Debug + Release libs linked per configuration")
    unset(LLD_LIBS_DUAL)
endif()

unset(_lld_all_found)
unset(_lld_components)
