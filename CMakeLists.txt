# ======================================================================
# CMakeLists.txt - لغة ص البرمجيّة / Sad Programming Language
# ======================================================================
# الوصف / Description:
#   ملفّ بناء CMake الرئيسيّ لمشروع لغة ص البرمجيّة العربيّة
#   Main CMake build file for Sad Arabic Programming Language project
#
# المتطلبات / Requirements:
#   - CMake 3.15 أو أحدث / or newer
#   - مترجم C++17 / C++17 compatible compiler
#   - مكتبات النظام الأساسيّة / System standard libraries
#
# الاستخدام / Usage:
#   cmake -S . -B build
#   cmake --build build --config Debug
#
# البنية / Structure:
#   cmake/llvm.cmake              - إعدادات LLVM الاختياريّة
#   cmake/platform.cmake          - خيارات المنصة والمترجم
#   cmake/sources.cmake           - تعريفات ملفات المصدر
#   cmake/libraries.cmake         - المكتبات (sad_interp, sad_runtime, graphics)
#   cmake/network.cmake           - مكتبات الشبكات (TCP/UDP, HTTP, WebSocket)
#   cmake/executables.cmake       - مكتبات الدعم التنفيذيّة (sad_mobile)؛ التنفيذيّان في apps/
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
    DESCRIPTION "لغة ص - لغة برمجة عربيّة حديثة / Sad - Modern Arabic Programming Language"
    LANGUAGES C CXX
)

# (AR) لاحقة إصدار بيتا (للعرض والتغليف) / (EN) Beta version suffix (for display/packaging)
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
# (AR) TEST-005 T1.3: اختبارات الشبكة تجري نداءات خارجيّة — معزولة خلف علم
#      افتراضيّه OFF حتى لا تُفشِل CI بأسباب بيئيّة (Architecture §9)
# (EN) TEST-005 T1.3: network tests make external calls — isolated behind a
#      flag (default OFF) so environment issues never fail CI (Architecture §9)
option(SAD_ENABLE_NETWORK_TESTS "تفعيل اختبارات الشبكة الخارجيّة / Enable external network tests" OFF)
option(BUILD_EXAMPLES "بناء الأمثلة / Build examples" ON)
option(ENABLE_DEBUG_LOGGING "تفعيل سجلّات التنقيح / Enable debug logging" OFF)
option(BUILD_PKG_MANAGER "بناء مدير الحزم / Build package manager" ON)
option(ENABLE_LLVM_BACKEND "تفعيل LLVM Backend / Enable LLVM Backend" ON)
option(BUILD_WASM "بناء هدف WebAssembly / Build WebAssembly target" OFF)

# ══════════════════════════════════════════════════════════════════════
# تحميل وحدات CMake / Load CMake Modules
# ══════════════════════════════════════════════════════════════════════

# 1. إعدادات LLVM الاختياريّة (يجب أن تأتي قبل cmake/core_components.cmake)
#    / Optional LLVM configuration (must precede cmake/core_components.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/llvm.cmake)

# 1.5. yaml-cpp الاختيارية لـ sadinfo / Optional yaml-cpp for sadinfo
# (AR) يجب أن تُحمَّل قبل cmake/core_components.cmake وtools.cmake لتعريف SAD_HAS_YAML_CPP
#      و target yaml-cpp قبل أن يتحقق منها tools/sadinfo/CMakeLists.txt.
# (EN) Must load before cmake/core_components.cmake and tools.cmake to define SAD_HAS_YAML_CPP
#      and the yaml-cpp target before tools/sadinfo/CMakeLists.txt checks them.
include(${CMAKE_SOURCE_DIR}/cmake/yaml_cpp.cmake)

# 2. إعدادات المنصة (preprocessor, warnings) / Platform Configuration
# (AR) يجب تحميلها قبل cmake/core_components.cmake لتنتشر الخيارات إلى المجلّدات الفرعيّة
# (EN) Must be loaded before cmake/core_components.cmake so options propagate to subdirectories
include(${CMAKE_SOURCE_DIR}/cmake/platform.cmake)

# ══════════════════════════════════════════════════════════════════════
# توافق MSVC مع LLVM Release فقط
# MSVC compatibility with Release-only LLVM
# ══════════════════════════════════════════════════════════════════════
# (AR) عند تفعيل وضع التوافق، يجب توحيد Runtime على /MD لكلّ الأهداف
#      حتى لا تظهر رموز Debug CRT مثل __imp__calloc_dbg أثناء الربط.
# (EN) In compatibility mode, force /MD runtime for all targets
#      to avoid Debug CRT symbols like __imp__calloc_dbg at link time.
if(MSVC AND ENABLE_LLVM_BACKEND AND SAD_FORCE_MSVC_LLVM_RELEASE_COMPAT)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL" CACHE STRING "MSVC runtime library" FORCE)

    # (AR) في Debug نلغي تعريف _DEBUG ونوحّد STL debug iterators مع Release.
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
# تضمين المكوّنات الفرعيّة الرئيسيّة
# Include core sub-components (shared/, interpreter/, compiler/)
# ══════════════════════════════════════════════════════════════════════
include(${CMAKE_SOURCE_DIR}/cmake/core_components.cmake)

# ══════════════════════════════════════════════════════════════════════
# وحدات إضافية / Additional Modules
# ══════════════════════════════════════════════════════════════════════

# 3. مجلّدات التضمين / Include Directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/features/network/core
    ${CMAKE_SOURCE_DIR}/stdlib
    # (AR) Phase F-XX: أُعيدت هيكلة shared/builtins/ إلى include/runtime + src/runtime
    # (EN) Phase F-XX: shared/builtins/ restructured to include/runtime + src/runtime
    ${CMAKE_SOURCE_DIR}/shared/builtins/include/runtime
    ${CMAKE_SOURCE_DIR}/stdlib/low_level/include
    ${CMAKE_SOURCE_DIR}/stdlib/mobile
    ${CMAKE_SOURCE_DIR}/compiler/frontend/include
    # البنية المُعاد تنظيمها / Reorganized structure
    ${CMAKE_SOURCE_DIR}/shared/utils/include
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
    ${CMAKE_SOURCE_DIR}/shared/parser/include
    ${CMAKE_SOURCE_DIR}/shared/ast/include
    ${CMAKE_SOURCE_DIR}/shared/types/include
    ${CMAKE_SOURCE_DIR}/shared/errors/include
    # (AR) Phase A2: سياسة الذاكرة — مُضافة كـ include directory لتمكين "memory/policy/gc_mode.h"
    # (EN) Phase A2: memory policy — added as include directory to enable "memory/policy/gc_mode.h"
    ${CMAKE_SOURCE_DIR}/shared/memory_policy/include
    ${CMAKE_SOURCE_DIR}/shared/modules/include
    ${CMAKE_SOURCE_DIR}/shared/builtins/include
    # (AR) م3 (RFC sadlang-rfcs#10): أُزيلت مسارات interpreter/include* العامّة من هنا
    #      ونُقلت إلى sad_interp PUBLIC (+ sad_runtime PRIVATE) في cmake/libraries.cmake.
    #      الهدف: ألّا تَرى أهدافُ نظام المترجم (التي تربط sad_shared فقط) ترويسات
    #      المفسّر. user_thrown.h (نوع استثناء مشترك) نُقل إلى shared/errors/include.
    #      يثبّت السلامةَ حارسُ check_interpreter_boundary.py.
    # (EN) Phase-3: removed the global interpreter/include* paths from here; moved to
    #      sad_interp PUBLIC (+ sad_runtime PRIVATE). The compiler subsystem (links
    #      sad_shared only) no longer sees interpreter headers. user_thrown.h (a shared
    #      exception type) relocated to shared/errors/include.
    ${CMAKE_SOURCE_DIR}/compiler/include
    ${CMAKE_SOURCE_DIR}/compiler/include/frontend
    ${CMAKE_SOURCE_DIR}/compiler/include/backend
    ${CMAKE_SOURCE_DIR}/compiler/include/types
    # (AR) مسار include/optimizer أُزيل — المجلّد محذوف بعد توحيد passes في middle/
    # (EN) include/optimizer path removed — folder deleted after unifying passes in middle/
    # (AR) مسارات include لوضع Freestanding
    # (EN) Include paths for Freestanding mode
    ${CMAKE_SOURCE_DIR}/compiler/include/pipeline
    ${CMAKE_SOURCE_DIR}/runtime/freestanding
)

# 3.9. توليد كود المعجم من YAML / Keyword codegen from YAML (v4.1)
# (AR) يجب أن يسبق sources.cmake لأنّه يُنتج keywords_generated.{h,cpp}
# (EN) Must run before sources.cmake — it produces keywords_generated.{h,cpp}
include(${CMAKE_SOURCE_DIR}/cmake/codegen.cmake)

# 4. تعريفات ملفات المصدر / Source File Definitions
include(${CMAKE_SOURCE_DIR}/cmake/sources.cmake)

# 5. المكتبات الأساسية / Core Libraries (sad_interp, sad_runtime, graphics)
include(${CMAKE_SOURCE_DIR}/cmake/libraries.cmake)

# 6. مكتبات الشبكات / Networking Libraries (TCP/UDP, HTTP, WebSocket)
include(${CMAKE_SOURCE_DIR}/cmake/network.cmake)

# 6.5. صNet — مكتبة الشبكات اللامركزيّة / SadNet Decentralized Networking
include(${CMAKE_SOURCE_DIR}/cmake/sadnet.cmake)

# 7. مكتبات الدعم التنفيذيّة / Executable-support libs (sad_mobile, flush_smoke)
include(${CMAKE_SOURCE_DIR}/cmake/executables.cmake)

# 7-ب. التطبيقات التنفيذيّة الرفيعة / Lightweight apps (sad-run, sad-build)
# (AR) م3 RFC sadlang-rfcs#10: نقاط الدخول التنفيذيّة في apps/ — بعد executables.cmake
#      لأنّ sad-build يربط sad_mobile المُعرَّف هناك. / (EN) After executables.cmake
#      because sad-build links sad_mobile defined there.
add_subdirectory(apps)

# 8. أدوات التطوير / Development Tools (LSP, Formatter, pkg, REPL)
include(${CMAKE_SOURCE_DIR}/cmake/tools.cmake)

# 9. الاختبارات / Tests
include(${CMAKE_SOURCE_DIR}/cmake/tests.cmake)

# 10. الاختبارات الشاملة / Comprehensive Test Suite (900+ tests)
include(${CMAKE_SOURCE_DIR}/cmake/tests_comprehensive.cmake)

# 11. اختبارات التنفيذ المزدوج / Dual Execution Tests (ADR-03)
include(${CMAKE_SOURCE_DIR}/cmake/dual_tests.cmake)

# 11-ب. اختبارات استخراج التوثيق المزدوج / Doc Generation Dual Tests (sad ↔ sadc)
include(${CMAKE_SOURCE_DIR}/cmake/doc_gen_dual_tests.cmake)

# 12. WebAssembly (اختياري) / WebAssembly (optional)
if(BUILD_WASM)
    include(${CMAKE_SOURCE_DIR}/cmake/wasm.cmake)
endif()

# 13. قواعد التثبيت وCPack / Install Rules & CPack
include(${CMAKE_SOURCE_DIR}/cmake/install.cmake)

# 14. حارس الملفات اليتيمة / Orphan Sources Guard
#     (AR) يفشل البناء إذا وُجد .cpp في compiler/src أو interpreter/src
#          غير مذكور في أيّ target. يمنع تراكم الكود الميت.
#     (EN) Fails build on orphan .cpp files. Prevents dead code accumulation.
include(${CMAKE_SOURCE_DIR}/cmake/orphan_sources_guard.cmake)
sad_check_orphan_sources()

# ══════════════════════════════════════════════════════════════════════
# الأمثلة / Examples
# ══════════════════════════════════════════════════════════════════════
if(BUILD_EXAMPLES)
    message(STATUS "✓ الأمثلة مُفعَّلة / Examples enabled")
endif()

# ══════════════════════════════════════════════════════════════════════
# رسائل التأكيد / Confirmation Messages
# ══════════════════════════════════════════════════════════════════════
message(STATUS "══════════════════════════════════════════════════")
message(STATUS "  لغة ص البرمجيّة / Sad Programming Language")
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
# ملاحظات للمطوّرين / Developer Notes
# ══════════════════════════════════════════════════════════════════════
# - جميع الملفّات يجب أن تكون UTF-8 لدعم العربيّة / All files must be UTF-8
# - استخدم debugPrint() بدلًا من std::cout / Use debugPrint() instead of std::cout
# - شغّل الاختبارات قبل الإيداع / Run tests before committing:
#     cmake --build build --config Debug --target comprehensive_tests
#     ctest --test-dir build -R Comprehensive
# ══════════════════════════════════════════════════════════════════════
