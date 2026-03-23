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
if(EXISTS "${CMAKE_SOURCE_DIR}/runtime_new/CMakeLists.txt")
    add_subdirectory(runtime_new)
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

if(MSVC)
    target_compile_options(sad_core PRIVATE /FS /utf-8 /Z7)
endif()

# ربط وقت التشغيل الجديد / Link runtime_new
if(TARGET sad_rt_runtime)
    target_link_libraries(sad_core PRIVATE sad_rt_runtime)
    target_include_directories(sad_core PRIVATE ${CMAKE_SOURCE_DIR}/runtime_new/include)
    target_compile_definitions(sad_core PRIVATE HAS_RUNTIME_NEW)
    message(STATUS "✓ ربط runtime_new بالمفسر / Linked runtime_new to interpreter")
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
    target_include_directories(sad_core PRIVATE ${CMAKE_SOURCE_DIR}/tools/profiler/include)
    message(STATUS "✓ ربط مصحح الأداء بالمفسر / Linked profiler to interpreter")
endif()

# (AR) إضافة مسارات إعادة التحميل الساخن / (EN) Add hot reload include paths
target_include_directories(sad_core PRIVATE ${CMAKE_SOURCE_DIR}/shared/hot_reload/include)
message(STATUS "✓ إعادة التحميل الساخن / Hot Reload")

# ──────────────────────────────────────────────────────────────────────
# المكونات المشتركة والمترجم / Shared & Compiler (if not already added)
# ──────────────────────────────────────────────────────────────────────
if(NOT TARGET sad_shared)
    add_subdirectory(shared)
endif()
if(NOT TARGET sad_new_compiler)
    add_subdirectory(compiler_new)
endif()
