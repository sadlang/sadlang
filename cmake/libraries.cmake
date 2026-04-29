# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/libraries.cmake
# الوصف: المكتبات الأساسية والفرعية / Core & Sub-Libraries
# ═══════════════════════════════════════════════════════════════════════════════

# ──────────────────────────────────────────────────────────────────────
# الآلة الافتراضية / Bytecode Virtual Machine
# ──────────────────────────────────────────────────────────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/vm/CMakeLists.txt")
    add_subdirectory(vm)
    message(STATUS "✓ الآلة الافتراضية / Bytecode VM")
endif()

# ──────────────────────────────────────────────────────────────────────
# وقت التشغيل الجديد / Runtime New (VM, Memory, Thread, FFI, ABI)
# ──────────────────────────────────────────────────────────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/runtime/CMakeLists.txt")
    add_subdirectory(runtime)
    message(STATUS "✓ وقت التشغيل الجديد / Runtime New: VM, Memory, Thread, FFI, ABI, Exception, Sandbox")
endif()

# ──────────────────────────────────────────────────────────────────────
# مكتبة النواة / Core Library (sad_core)
# ──────────────────────────────────────────────────────────────────────
# (AR) مصادر freestanding تُضاف فقط عند الطلب (تحتوي على معرّفات عربية غير مدعومة في MSVC)
# (EN) Freestanding sources only added when requested (Arabic identifiers unsupported by MSVC without special setup)
if(ENABLE_FREESTANDING)
    set(ALL_SOURCES ${ALL_SOURCES} ${FREESTANDING_SOURCES})
endif()

add_library(sad_core STATIC ${ALL_SOURCES})

# (AR) Phase: dedup sad_shared/sad_core — sad_core يربط sad_shared كـ PUBLIC
#      ليُمرّر includes ويتجنب ازدواج بناء (lexer/parser/ast/types/errors/modules/utils
#      + class_manager). كان كل ملف من 49 ملفاً مشتركاً يُترجَم مرتين قبل هذا الإصلاح.
# (EN) Phase: dedup sad_shared/sad_core — sad_core links sad_shared PUBLIC to
#      forward includes and avoid build duplication (lexer/parser/ast/types/errors/
#      modules/utils + class_manager). Each of 49 shared files used to compile twice
#      before this fix.
target_link_libraries(sad_core PUBLIC sad_shared)

# (AR) Ownership Unification: sad_core يربط sad_ownership ليستهلك المفسّر
#      نظام الملكية الموحَّد عبر wrapper في interpreter/src/managers/ownership_manager.cpp
# (EN) Ownership Unification: sad_core links sad_ownership so the interpreter
#      consumes the unified ownership system via the wrapper in
#      interpreter/src/managers/ownership_manager.cpp
target_link_libraries(sad_core PUBLIC sad_ownership)

if(MSVC)
    target_compile_options(sad_core PRIVATE /FS /utf-8 /Z7)
endif()

# (AR) تفعيل المسار الحقيقي لوحدة HTTP داخل sad_core وربط مكتبات الشبكة
#      حتى تتوفر دوال stdlib/network للمفسر ولكل المستهلكين لـ sad_core.
# (EN) Enable the real HTTP path inside sad_core and link network libraries
#      so stdlib/network builtins are available to the interpreter and all sad_core consumers.
# ملاحظة (AR): الربط الفعلي يتم في cmake/network.cmake لأن أهداف sad_network/
#              sad_http/sad_websocket تُعرَّف هناك بعد تحميل هذا الملف.
# Note (EN): Actual linking happens in cmake/network.cmake because the
#            sad_network/sad_http/sad_websocket targets are defined there
#            AFTER this file has been included.

# ربط مكتبة وقت التشغيل بالمفسر / Link runtime to interpreter
# (AR) sad_rt_runtime معرّف في runtime/CMakeLists.txt كمكتبة INTERFACE تجمع abi + ffi + ui
# (EN) sad_rt_runtime is defined in runtime/CMakeLists.txt as an INTERFACE library
#      aggregating abi + ffi + ui
if(TARGET sad_rt_runtime)
    target_link_libraries(sad_core PRIVATE sad_rt_runtime)
    target_include_directories(sad_core PRIVATE ${CMAKE_SOURCE_DIR}/runtime/include)
    message(STATUS "✓ ربط runtime بالمفسر / Linked runtime to interpreter")
endif()

# ربط صNet — مكتبة الشبكات اللامركزية / Link SadNet
if(TARGET sadnet)
    target_link_libraries(sad_core PRIVATE sadnet)
    target_compile_definitions(sad_core PRIVATE HAS_SADNET)
    message(STATUS "✓ ربط صNet بالمفسر / Linked SadNet to interpreter")
endif()

# SQLite3 (اختياري) / SQLite3 (optional)
find_package(unofficial-sqlite3 CONFIG QUIET)
if(unofficial-sqlite3_FOUND)
    target_link_libraries(sad_core PRIVATE unofficial::sqlite3::sqlite3)
    target_compile_definitions(sad_core PRIVATE HAS_SQLITE3)
    message(STATUS "✓ SQLite3: مفعّل / Enabled")
else()
    find_package(SQLite3 QUIET)
    if(SQLite3_FOUND)
        target_link_libraries(sad_core PRIVATE ${SQLite3_LIBRARIES})
        target_include_directories(sad_core PRIVATE ${SQLite3_INCLUDE_DIRS})
        target_compile_definitions(sad_core PRIVATE HAS_SQLITE3)
        message(STATUS "✓ SQLite3: مفعّل / Enabled")
    else()
        message(STATUS "⚠ SQLite3: غير موجود / Not found")
    endif()
endif()

# OpenSSL (اختياري) / OpenSSL (optional)
find_package(OpenSSL QUIET)
if(OPENSSL_FOUND)
    target_link_libraries(sad_core PRIVATE OpenSSL::SSL OpenSSL::Crypto)
    target_include_directories(sad_core PRIVATE ${OPENSSL_INCLUDE_DIR})
    target_compile_definitions(sad_core PRIVATE HAS_OPENSSL)
    message(STATUS "✓ OpenSSL: مفعّل / Enabled")
else()
    message(STATUS "⚠ OpenSSL: غير موجود / Not found")
endif()

set_target_properties(sad_core PROPERTIES
    OUTPUT_NAME "sad_core"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

# ربط نظام واجهات sad_ui / Link SadUI system
if(TARGET sad_ui)
    target_link_libraries(sad_core PRIVATE sad_ui)
    target_include_directories(sad_core PRIVATE 
        ${CMAKE_SOURCE_DIR}/sad_ui/core/include
        ${CMAKE_SOURCE_DIR}/sad_ui/backends/desktop/include
    )
    # (AR) على Linux، SDL2 include يحتاج مسار صريح / (EN) On Linux, SDL2 needs explicit include path
    if(UNIX)
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(SDL2_PC QUIET sdl2)
            if(SDL2_PC_FOUND)
                target_include_directories(sad_core PRIVATE ${SDL2_PC_INCLUDE_DIRS})
            endif()
        endif()
    endif()
    message(STATUS "✓ ربط sad_ui بالمفسر / Linked sad_ui to interpreter")
endif()

# (AR) ربط مكتبة مصحح الأداء / (EN) Link profiler library
if(TARGET sad_profiler_lib)
    target_link_libraries(sad_core PRIVATE sad_profiler_lib)
    target_include_directories(sad_core PRIVATE ${CMAKE_SOURCE_DIR}/shared/profiler/include)
    message(STATUS "✓ ربط مصحح الأداء بالمفسر / Linked profiler to interpreter")
endif()

# (AR) إضافة مسارات إعادة التحميل الساخن / (EN) Add hot reload include paths
target_include_directories(sad_core PRIVATE ${CMAKE_SOURCE_DIR}/shared/hot_reload/include)
message(STATUS "✓ إعادة التحميل الساخن / Hot Reload")

# (AR) Phase 3 (F-01): مسار فاحص الأنواع المشترك بعد نقله من compiler/.
#      المفسر يضم "semantic/type_checker.h" مباشرة دون مسار نسبي.
# (EN) Phase 3 (F-01): shared type checker include path after move from compiler/.
#      Interpreter includes "semantic/type_checker.h" directly without relative path.
target_include_directories(sad_core PRIVATE ${CMAKE_SOURCE_DIR}/shared/semantic/include)

# ──────────────────────────────────────────────────────────────────────
# المكونات المشتركة والمترجم / Shared & Compiler (if not already added)
# ──────────────────────────────────────────────────────────────────────
if(NOT TARGET sad_shared)
    add_subdirectory(shared)
endif()
if(NOT TARGET sad_compiler)
    add_subdirectory(compiler)
endif()
