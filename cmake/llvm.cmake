# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/llvm.cmake
# الوصف: إعدادات LLVM الاختيارية / Optional LLVM Configuration
# ═══════════════════════════════════════════════════════════════════════════════
# هذا الملف يفصل إعدادات LLVM عن ملف البناء الرئيسي لتسهيل الصيانة.
# LLVM اختياري - المفسر يعمل بدونه، المترجم sadc يحتاجه.
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
