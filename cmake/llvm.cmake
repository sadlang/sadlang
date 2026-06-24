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
#   cmake --build build --config Release --target sad-build
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
#   cmake --build build --config Release --target sad-build
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
            # (AR) مجلد cmake لـ LLVM Debug — لحساب إغلاق مكتبات Debug من LLVMConfig
            #      الخاص به (رسم اعتماداته أغنى من Release بسبب كود التوكيدات).
            # (EN) Debug LLVM cmake dir — to compute the Debug lib closure from its
            #      own LLVMConfig (its dep graph is richer than Release due to
            #      assertion code, e.g. LLParser→LLVMBinaryFormat).
            set(SAD_LLVM_DEBUG_CMAKE_DIR "${_debug_hint}")
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
        message(WARNING "    cmake --build build --config Release --target sad-build")
        message(WARNING "")
    endif()
endif()

# (AR) إضافة تعريفات ومسارات LLVM / (EN) Add LLVM definitions and paths
add_definitions(${LLVM_DEFINITIONS})

# ═══════════════════════════════════════════════════════════════════════════════
# (AR) مسار بحث الرابط لمكتبات LLVM — مُدرِك للتهيئة (إصلاح بناء Debug)
# ───────────────────────────────────────────────────────────────────────────────
# السبب الجذري لفشل بناء sad-build في Debug: كان link_directories يضيف مجلد LLVM
# Release كمسار بحث عام في *كل* التهيئات. فأيّ مكتبة LLVM تُحَلّ بالاسم (لا بالمسار
# الكامل) أثناء الربط في Debug — مثل LLVMPasses/LLVMCoroutines المسحوبتين كاعتماد
# عابر — تأتي من Release، فينشأ تعارض _ITERATOR_DEBUG_LEVEL/RuntimeLibrary/ABI.
#
# الحل: نجعل مسار البحث مُدرِكًا للتهيئة عبر generator expression — مجلد Debug في
# تهيئة Debug، ومجلد Release فيما عداها. بهذا لا يتسرّب أيّ ملف Release إلى Debug،
# ويتكامل مع قائمة المكتبات المزدوجة (debug/optimized) بالمسارات الكاملة أدناه.
#
# (EN) Config-aware LLVM library search path — the Debug-build fix.
# Root cause of sad-build Debug link failure: link_directories added the LLVM
# Release lib dir as a global search path in *all* configs, so any LLVM lib
# resolved by name (e.g. transitively pulled LLVMPasses/LLVMCoroutines) came from
# Release in Debug → _ITERATOR_DEBUG_LEVEL/RuntimeLibrary/ABI mismatch. We make
# the search path config-aware: Debug dir in Debug, Release dir otherwise.
# ═══════════════════════════════════════════════════════════════════════════════
if(SAD_LLVM_HAS_DEBUG AND MSVC)
    link_directories(
        "$<$<CONFIG:Debug>:${SAD_LLVM_DEBUG_LIB_DIR}>"
        "$<$<NOT:$<CONFIG:Debug>>:${LLVM_LIBRARY_DIRS}>"
    )
else()
    # (AR) لا تثبيت Debug — مسار Release فقط (المترجم يُبنى في Release)
    # (EN) No Debug install — Release path only (compiler builds in Release)
    link_directories(${LLVM_LIBRARY_DIRS})
endif()

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

# (AR) بعض توزيعات LLVM (مثل Homebrew) تبني PassBuilder بدمج Polly، فيُشار إلى
#      getPollyPluginInfo() دون توفّر تعريفه ما لم نربط Polly. نضيفها إن وُجدت.
# (EN) Some LLVM distributions (e.g. Homebrew) link Polly into the PassBuilder,
#      leaving getPollyPluginInfo() undefined unless Polly is linked. Add if present.
if(EXISTS "${LLVM_LIBRARY_DIRS}/libPolly.a")
    list(APPEND LLVM_LIBS
        "${LLVM_LIBRARY_DIRS}/libPolly.a"
        "${LLVM_LIBRARY_DIRS}/libPollyISL.a")
    message(STATUS "   ✓ Polly مدمج / linked: ${LLVM_LIBRARY_DIRS}/libPolly.a")
endif()

# ═══════════════════════════════════════════════════════════════════════════════
# (AR) قائمة مكتبات LLVM بالأسماء المجرّدة — مضادّة للتسرّب جذريًا
# ───────────────────────────────────────────────────────────────────────────────
# بدل المسارات الكاملة المزدوجة (التي كانت تُسرّب نسخ Release من مستهلكين ساكنين
# فتُنتج LNK2038/LNK1169)، نمرّر كل مكتبة باسمها المجرّد فقط (LLVMCore.lib…).
# يبحث الرابط عنها في مسار البحث المُدرِك للتهيئة (link_directories أعلاه): مجلد
# Debug في تهيئة Debug، ومجلد Release فيما عداها. بما أنّ لكل تهيئة *مجلد بحث واحد*،
# يستحيل أن يُحَلّ الاسم إلا لنسخة تلك التهيئة — لا خلط ولا تسرّب ممكن بنيويًّا.
#
# نحسب الأسماء من إغلاق Debug (الأشمل، فيه حواف التوكيدات مثل LLVMBinaryFormat)؛
# وهذه الأسماء نفسها تُحَلّ في Release من مجلده (إغلاق Release مجموعة فرعية منه).
#
# (EN) Bare-name LLVM lib list — structurally leak-proof. Instead of dual full
# paths (which leaked Release copies from static consumers → LNK2038/LNK1169), we
# pass each lib by bare name (LLVMCore.lib…). The linker resolves it via the
# config-aware link_directories: Debug dir in Debug, Release dir otherwise. Since
# each config has exactly ONE search dir, a name can only resolve to that config's
# copy — no mixing possible. Names come from the Debug closure (the superset,
# including assertion-only edges); the same names resolve in Release from its dir.
# ═══════════════════════════════════════════════════════════════════════════════
if(SAD_LLVM_HAS_DEBUG AND MSVC)
    # (AR) نسرد *كل* مكتبات LLVM الموجودة في مجلد Debug كأسماء مجرّدة. هذا أضمن من
    #      الاعتماد على إغلاق llvm_map (الذي يُسقط حواف التوكيدات الخاصة بـ Debug
    #      مثل LLVMBinaryFormat/LLVMTargetParser). مجلد Debug يحوي مكتبات LLVM
    #      اللازمة فقط (لا clang)، فالقائمة كاملة بالبناء. الأسماء المجرّدة تُحَلّ عبر
    #      link_directories المُدرِك للتهيئة (Debug في Debug، Release فيما عداها) —
    #      مضادّ للتسرّب لأن لكل تهيئة مجلد بحث واحد. والمكتبات غير المُشار إليها لا
    #      تُضيف رموزًا للرابط (يَسحب الكائنات المطلوبة فقط من كل أرشيف).
    # (EN) List ALL LLVM libs present in the Debug dir as bare names. More robust
    #      than relying on llvm_map's closure (which drops Debug assertion-only
    #      edges like LLVMBinaryFormat/LLVMTargetParser). The Debug dir holds only
    #      the needed LLVM libs (no clang), so the set is complete by construction.
    #      Bare names resolve via the config-aware link dir — leak-proof (one dir
    #      per config). Unreferenced archives contribute nothing to the link.
    file(GLOB _llvm_debug_libs "${SAD_LLVM_DEBUG_LIB_DIR}/LLVM*.lib")
    set(_llvm_names "")
    foreach(_l ${_llvm_debug_libs})
        get_filename_component(_n "${_l}" NAME)      # LLVMCore.lib
        # (AR) استثناء مكتبة الاستيراد للـ DLL الديناميكي (LLVM-C.lib → LLVM-C.dll):
        #      ربطها يجعل sad-build يعتمد على DLL وقت التشغيل (خطأ 0xC0000135). نحن
        #      نربط مكتبات LLVM الساكنة فقط.
        # (EN) Exclude the dynamic C-API import lib (LLVM-C.lib → LLVM-C.dll):
        #      linking it makes sad-build depend on a runtime DLL (0xC0000135). We
        #      link the static LLVM component libs only.
        if(_n STREQUAL "LLVM-C.lib")
            continue()
        endif()
        list(APPEND _llvm_names "${_n}")
    endforeach()
    list(REMOVE_DUPLICATES _llvm_names)
    set(LLVM_LIBS ${_llvm_names})
    list(LENGTH _llvm_names _n_count)
    message(STATUS "   LLVM bare-name dual-config: ${_n_count} libs, per-config link dir")
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
    # (AR) نفس المبدأ المضاد للتسرّب: أسماء مجرّدة تُحَلّ عبر link_directories
    #      المُدرِك للتهيئة (Debug في Debug، Release فيما عداها).
    # (EN) Same leak-proof bare-name approach, resolved via config-aware link dir.
    set(_lld_names "")
    foreach(_lib ${LLD_LIBS})
        get_filename_component(_lld_name "${_lib}" NAME)
        list(APPEND _lld_names "${_lld_name}")
    endforeach()
    set(LLD_LIBS ${_lld_names})
    message(STATUS "   LLD bare-name dual-config: per-config link dir")
endif()

unset(_lld_all_found)
unset(_lld_components)
