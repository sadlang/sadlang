# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف / File: cmake/tools.cmake
# الوصف / Desc: أدوات التطوير / Development Tools (LSP, Formatter, REPL, Package Manager)
# ═══════════════════════════════════════════════════════════════════════════════

# ──────────────────────────────────────────────────────────────────────
# (AR) طبقة الأدوات المشتركة — يجب تسجيلها أولاً لأن LSP والمنسق يعتمدان عليها
# (EN) Tools-shared layer — must be registered first; LSP & formatter depend on it
# ──────────────────────────────────────────────────────────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/tools/shared/CMakeLists.txt")
    add_subdirectory(tools/shared)
    message(STATUS "✓ طبقة الأدوات المشتركة / Tools-Shared: sad_tools_shared")
endif()

# ──────────────────────────────────────────────────────────────────────
# (AR) مركز الأدوات الموحَّد — تنفيذي مستقل لا يعتمد على المفسّر/المترجم
# (EN) Unified Tool Hub — standalone executable, no interpreter/compiler dep
# (AR) يُبنى دائماً لأنه نقطة الدخول الجديدة للمستخدم (sad <cmd>)
# (EN) Always built as it's the new user entry point (sad <cmd>)
# ──────────────────────────────────────────────────────────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/tools/hub/CMakeLists.txt")
    add_subdirectory(tools/hub)
    message(STATUS "✓ مركز الأدوات / Tool Hub: sad-hub")
endif()

# ──────────────────────────────────────────────────────────────────────
# (AR) فاحص الملكية الثابت — يستخدم shared/ownership فقط (لا مفسّر/مترجم)
# (EN) Static ownership checker — uses only shared/ownership
# ──────────────────────────────────────────────────────────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/tools/check/CMakeLists.txt")
    add_subdirectory(tools/check)
    message(STATUS "✓ فاحص الملكية / Ownership Checker: sad-check")
endif()

# ──────────────────────────────────────────────────────────────────────
# (AR) الماسح الأمني الساكن — يعتمد فقط على shared/security
# (EN) Static security scanner — depends only on shared/security
# (AR) نُقل من compiler/src/security/ ليصبح أداة مستقلة (sad-guard) بدون
#      الاعتماد على المترجم.
# (EN) Moved from compiler/src/security/ to be a standalone tool (sad-guard)
#      with no compiler dependency.
# ──────────────────────────────────────────────────────────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/tools/security-scanner/CMakeLists.txt")
    add_subdirectory(tools/security-scanner)
    message(STATUS "✓ الماسح الأمني / Security Scanner: sad-guard")
endif()

# ──────────────────────────────────────────────────────────────────────
# خادم LSP / Language Server Protocol
# ──────────────────────────────────────────────────────────────────────
option(BUILD_LSP_SERVER "بناء خادم LSP / Build LSP server" ON)

if(BUILD_LSP_SERVER)
    # مكتبة JSON / JSON library
    find_package(nlohmann_json QUIET)
    if(NOT nlohmann_json_FOUND)
        message(STATUS "⬇ تحميل nlohmann/json / Downloading nlohmann/json...")
        include(FetchContent)
        FetchContent_Declare(json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG v3.11.3
            GIT_SHALLOW TRUE
        )
        set(JSON_BuildTests OFF CACHE INTERNAL "")
        set(JSON_Install OFF CACHE INTERNAL "")
        FetchContent_MakeAvailable(json)
    endif()

    # خادم LSP القديم تم دمجه في الجديد — تم حذفه
    # Legacy LSP server merged into the new one — removed

    # خادم LSP الجديد / New LSP Server
    if(EXISTS "${CMAKE_SOURCE_DIR}/tools/lsp/CMakeLists.txt")
        add_subdirectory(tools/lsp)
    endif()

    # أداة التنسيق / Code Formatter
    if(EXISTS "${CMAKE_SOURCE_DIR}/tools/formatter/CMakeLists.txt")
        add_subdirectory(tools/formatter)
    endif()

    # المحلل المتقدم / Advanced Analyzer
    if(EXISTS "${CMAKE_SOURCE_DIR}/tools/analyze/CMakeLists.txt")
        add_subdirectory(tools/analyze)
    endif()

    # (AR) قواعد التثبيت مُجمّعة في cmake/install.cmake
    # (EN) Install rules consolidated in cmake/install.cmake

    message(STATUS "✓ خادم LSP / LSP Server: sad-lsp-server")
    message(STATUS "✓ أداة التنسيق / Formatter: sad-fmt")
    message(STATUS "✓ المحلل المتقدم / Advanced Analyzer: sad-analyze")
endif()

# ──────────────────────────────────────────────────────────────────────
# مدير الحزم / Package Manager
# ──────────────────────────────────────────────────────────────────────
option(BUILD_PKG_MANAGER "بناء مدير الحزم / Build package manager" ON)

if(BUILD_PKG_MANAGER)
    add_executable(sad-pkg tools/pkg/cli_v2.cpp)

    target_include_directories(sad-pkg PRIVATE
        ${CMAKE_SOURCE_DIR}/tools/pkg ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/shared/utils/include)

    if(WIN32)
        target_link_libraries(sad-pkg PRIVATE winhttp)
    endif()

    set_target_properties(sad-pkg PROPERTIES
        OUTPUT_NAME "sad-pkg" RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

    message(STATUS "✓ مدير الحزم / Package Manager: sad-pkg")
endif()

# ──────────────────────────────────────────────────────────────────────
# بيئة التطوير التفاعلية / REPL
# ──────────────────────────────────────────────────────────────────────
option(BUILD_REPL "بناء REPL / Build REPL" ON)

if(BUILD_REPL AND EXISTS "${CMAKE_SOURCE_DIR}/tools/repl/CMakeLists.txt")
    add_subdirectory(tools/repl)
    message(STATUS "✓ REPL: sad-repl")
endif()

# ──────────────────────────────────────────────────────────────────────
# أداة بناء APK / APK Builder Tool
# ──────────────────────────────────────────────────────────────────────
option(BUILD_APK_BUILDER "بناء أداة APK / Build APK builder" ON)

if(BUILD_APK_BUILDER AND EXISTS "${CMAKE_SOURCE_DIR}/tools/apk_builder/CMakeLists.txt")
    add_subdirectory(tools/apk_builder)
    message(STATUS "✓ أداة بناء APK / APK Builder: sad-apk")
endif()

# ──────────────────────────────────────────────────────────────────────
# أداة بناء Android / Android Builder Tool
# ──────────────────────────────────────────────────────────────────────
option(BUILD_ANDROID_TOOL "بناء أداة Android / Build Android tool" ON)

if(BUILD_ANDROID_TOOL AND NOT ANDROID AND EXISTS "${CMAKE_SOURCE_DIR}/tools/android/CMakeLists.txt")
    add_subdirectory(tools/android)
    message(STATUS "✓ أداة Android / Android Tool: sad-android")
endif()

# ──────────────────────────────────────────────────────────────────────
# مصحح الأداء / Performance Profiler
# (AR) ملاحظة: add_subdirectory يتم في CMakeLists.txt الرئيسي قبل sad_interp (= sad_core)
# (EN) Note: add_subdirectory is done in root CMakeLists.txt before sad_interp (= sad_core)
# ──────────────────────────────────────────────────────────────────────