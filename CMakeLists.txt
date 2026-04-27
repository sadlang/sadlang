# ======================================================================
# CMakeLists.txt - لغة ص البرمجية / Sad Programming Language
# ======================================================================
# الوصف / Description:
#   ملف بناء CMake الرئيسي لمشروع لغة ص البرمجية العربية
#   Main CMake build file for Sad Arabic Programming Language project
#
# المتطلبات / Requirements:
#   - CMake 3.15 أو أحدث / or newer
#   - مترجم C++17 / C++17 compatible compiler
#   - مكتبات النظام الأساسية / System standard libraries
#
# الاستخدام / Usage:
#   cmake -S . -B build
#   cmake --build build --config Debug
#
# البنية / Structure:
#   cmake/llvm.cmake              - إعدادات LLVM الاختيارية
#   cmake/platform.cmake          - خيارات المنصة والمترجم
#   cmake/sources.cmake           - تعريفات ملفات المصدر
#   cmake/libraries.cmake         - المكتبات (sad_core, graphics, VM)
#   cmake/network.cmake           - مكتبات الشبكات (TCP/UDP, HTTP, WebSocket)
#   cmake/executables.cmake       - البرامج التنفيذية (sad, sadc)
#   cmake/tools.cmake             - أدوات التطوير (LSP, Formatter, pkg, REPL)
#   cmake/tests.cmake             - الاختبارات
#   cmake/tests_comprehensive.cmake - الاختبارات الشاملة (900+ اختبار)
# ======================================================================

cmake_minimum_required(VERSION 3.15)

# ══════════════════════════════════════════════════════════════════════
# معلومات المشروع / Project Information
# ══════════════════════════════════════════════════════════════════════
project(SadLanguage
    VERSION 1.0.0
    DESCRIPTION "لغة ص - لغة برمجة عربية حديثة / Sad - Modern Arabic Programming Language"
    LANGUAGES C CXX
)

# Beta version suffix (for display/packaging)
set(SAD_VERSION_SUFFIX "-beta.1")

# ══════════════════════════════════════════════════════════════════════
# إعدادات C++ / C++ Settings
# ══════════════════════════════════════════════════════════════════════
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ══════════════════════════════════════════════════════════════════════
# مسارات الإخراج / Output Paths
# ══════════════════════════════════════════════════════════════════════
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ══════════════════════════════════════════════════════════════════════
# خيارات البناء / Build Options
# ══════════════════════════════════════════════════════════════════════
option(BUILD_TESTS "بناء الاختبارات / Build tests" OFF)
option(BUILD_EXAMPLES "بناء الأمثلة / Build examples" ON)
option(ENABLE_DEBUG_LOGGING "تفعيل سجلات التنقيح / Enable debug logging" OFF)
option(BUILD_PKG_MANAGER "بناء مدير الحزم / Build package manager" ON)
option(ENABLE_LLVM_BACKEND "تفعيل LLVM Backend / Enable LLVM Backend" ON)
option(BUILD_WASM "بناء هدف WebAssembly / Build WebAssembly target" OFF)

# ══════════════════════════════════════════════════════════════════════
# تحميل وحدات CMake / Load CMake Modules
# ══════════════════════════════════════════════════════════════════════

# 1. إعدادات LLVM الاختيارية (يجب أن تأتي قبل reorganized.cmake) / Optional LLVM Configuration
include(${CMAKE_SOURCE_DIR}/cmake/llvm.cmake)

# 2. إعدادات المنصة (preprocessor, warnings) / Platform Configuration
# (AR) يجب تحميلها قبل reorganized.cmake لتنتشر الخيارات إلى المجلدات الفرعية
# (EN) Must be loaded before reorganized.cmake so options propagate to subdirectories
include(${CMAKE_SOURCE_DIR}/cmake/platform.cmake)

# ══════════════════════════════════════════════════════════════════════
# توافق MSVC مع LLVM Release فقط
# MSVC compatibility with Release-only LLVM
# ══════════════════════════════════════════════════════════════════════
# (AR) عند تفعيل وضع التوافق، يجب توحيد Runtime على /MD لكل الأهداف
#      حتى لا تظهر رموز Debug CRT مثل __imp__calloc_dbg أثناء الربط.
# (EN) In compatibility mode, force /MD runtime for all targets
#      to avoid Debug CRT symbols like __imp__calloc_dbg at link time.
if(MSVC AND ENABLE_LLVM_BACKEND AND SAD_FORCE_MSVC_LLVM_RELEASE_COMPAT)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL" CACHE STRING "MSVC runtime library" FORCE)

    # (AR) في Debug نلغي تعريف _DEBUG ونوحد STL debug iterators مع Release.
    # (EN) In Debug, undefine _DEBUG and align STL debug iterators with Release.
    add_compile_options($<$<CONFIG:Debug>:/U_DEBUG>)
    add_compile_definitions(
        $<$<CONFIG:Debug>:NDEBUG>
        $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=0>
        $<$<CONFIG:Debug>:_HAS_ITERATOR_DEBUGGING=0>
    )

    message(STATUS "MSVC release-compat: forcing /MD runtime and Release-compatible debug defines")
endif()

# ══════════════════════════════════════════════════════════════════════
# تضمين المكونات الفرعية المعاد تنظيمها
# Include reorganized sub-components (shared/, interpreter/, compiler/)
# ══════════════════════════════════════════════════════════════════════
include(${CMAKE_SOURCE_DIR}/reorganized.cmake)

# ══════════════════════════════════════════════════════════════════════
# وحدات إضافية / Additional Modules
# ══════════════════════════════════════════════════════════════════════

# 3. مجلدات التضمين / Include Directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/stdlib/network
    ${CMAKE_SOURCE_DIR}/stdlib
    ${CMAKE_SOURCE_DIR}/stdlib/network
    # (AR) Phase 5 (F-05): runtime الأساسي نُقل إلى shared/builtins/runtime/
    # (EN) Phase 5 (F-05): core runtime moved to shared/builtins/runtime/
    ${CMAKE_SOURCE_DIR}/shared/builtins/runtime
    ${CMAKE_SOURCE_DIR}/stdlib/low_level/include
    ${CMAKE_SOURCE_DIR}/stdlib/mobile
    ${CMAKE_SOURCE_DIR}/compiler/frontend/include
    # البنية المعاد تنظيمها / Reorganized structure
    ${CMAKE_SOURCE_DIR}/shared/utils/include
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
    ${CMAKE_SOURCE_DIR}/shared/parser/include
    ${CMAKE_SOURCE_DIR}/shared/ast/include
    ${CMAKE_SOURCE_DIR}/shared/types/include
    ${CMAKE_SOURCE_DIR}/shared/errors/include
    ${CMAKE_SOURCE_DIR}/shared/modules/include
    ${CMAKE_SOURCE_DIR}/shared/builtins/include
    ${CMAKE_SOURCE_DIR}/interpreter/include
    ${CMAKE_SOURCE_DIR}/interpreter/include/core
    ${CMAKE_SOURCE_DIR}/interpreter/include/visitors
    ${CMAKE_SOURCE_DIR}/interpreter/include/managers
    ${CMAKE_SOURCE_DIR}/interpreter/include/builtins
    ${CMAKE_SOURCE_DIR}/interpreter/include/debug
    ${CMAKE_SOURCE_DIR}/interpreter/include/ui
    ${CMAKE_SOURCE_DIR}/interpreter/src/ui
    ${CMAKE_SOURCE_DIR}/compiler/include
    ${CMAKE_SOURCE_DIR}/compiler/include/frontend
    ${CMAKE_SOURCE_DIR}/compiler/include/backend
    ${CMAKE_SOURCE_DIR}/compiler/include/types
    ${CMAKE_SOURCE_DIR}/compiler/include/optimizer
    # (AR) مسارات include لوضع Freestanding
    # (EN) Include paths for Freestanding mode
    ${CMAKE_SOURCE_DIR}/compiler/include/pipeline
    ${CMAKE_SOURCE_DIR}/runtime/freestanding
)

# 4. تعريفات ملفات المصدر / Source File Definitions
include(${CMAKE_SOURCE_DIR}/cmake/sources.cmake)

# 5. المكتبات الأساسية / Core Libraries (sad_core, graphics, VM)
include(${CMAKE_SOURCE_DIR}/cmake/libraries.cmake)

# 6. مكتبات الشبكات / Networking Libraries (TCP/UDP, HTTP, WebSocket)
include(${CMAKE_SOURCE_DIR}/cmake/network.cmake)

# 6.5. صNet — مكتبة الشبكات اللامركزية / SadNet Decentralized Networking
include(${CMAKE_SOURCE_DIR}/cmake/sadnet.cmake)

# 7. البرامج التنفيذية / Executables (sad, sadc)
include(${CMAKE_SOURCE_DIR}/cmake/executables.cmake)

# 8. أدوات التطوير / Development Tools (LSP, Formatter, pkg, REPL)
include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)

# 9. الاختبارات / Tests
include(${CMAKE_SOURCE_DIR}/cmake/tests.cmake)

# 10. الاختبارات الشاملة / Comprehensive Test Suite (900+ tests)
include(${CMAKE_SOURCE_DIR}/cmake/tests_comprehensive.cmake)

# 11. اختبارات التنفيذ المزدوج / Dual Execution Tests (ADR-03)
include(${CMAKE_SOURCE_DIR}/cmake/dual_tests.cmake)

# 11.b اختبارات استخراج التوثيق المزدوج / Doc Generation Dual Tests (sad ↔ sadc)
include(${CMAKE_SOURCE_DIR}/cmake/doc_gen_dual_tests.cmake)

# 12. WebAssembly (اختياري) / WebAssembly (optional)
if(BUILD_WASM)
    include(${CMAKE_SOURCE_DIR}/cmake/wasm.cmake)
endif()

# 13. قواعد التثبيت و CPack / Install Rules & CPack
include(${CMAKE_SOURCE_DIR}/cmake/install.cmake)

# ══════════════════════════════════════════════════════════════════════
# الأمثلة / Examples
# ══════════════════════════════════════════════════════════════════════
if(BUILD_EXAMPLES)
    message(STATUS "✓ الأمثلة مفعلة / Examples enabled")
endif()

# ══════════════════════════════════════════════════════════════════════
# رسائل التأكيد / Confirmation Messages
# ══════════════════════════════════════════════════════════════════════
message(STATUS "══════════════════════════════════════════════════")
message(STATUS "  لغة ص البرمجية / Sad Programming Language")
message(STATUS "══════════════════════════════════════════════════")
message(STATUS "  الإصدار / Version:        ${PROJECT_VERSION}")
message(STATUS "  معيار C++ / C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "  مجلد البناء / Build Dir:  ${CMAKE_BINARY_DIR}")
message(STATUS "  نوع البناء / Build Type:  ${CMAKE_BUILD_TYPE}")
message(STATUS "  المترجم / Compiler:       ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  LLVM:                      ${ENABLE_LLVM_BACKEND}")
message(STATUS "  الاختبارات / Tests:       ${BUILD_TESTS}")
message(STATUS "══════════════════════════════════════════════════")

# ══════════════════════════════════════════════════════════════════════
# ملاحظات للمطورين / Developer Notes
# ══════════════════════════════════════════════════════════════════════
# - جميع الملفات يجب أن تكون UTF-8 لدعم العربية / All files must be UTF-8
# - استخدم debugPrint() بدلاً من std::cout / Use debugPrint() instead of std::cout
# - شغّل الاختبارات قبل الإيداع / Run tests before committing:
#     cmake --build build --config Debug --target comprehensive_tests
#     ctest --test-dir build -R Comprehensive
# ══════════════════════════════════════════════════════════════════════
