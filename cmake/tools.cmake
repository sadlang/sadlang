# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/tools.cmake
# الوصف: أدوات التطوير / Development Tools (LSP, Formatter, REPL, Package Manager)
# ═══════════════════════════════════════════════════════════════════════════════

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

    # تثبيت / Installation
    if(TARGET sad-lsp-server)
        install(TARGETS sad-lsp-server RUNTIME DESTINATION bin COMPONENT tools)
    endif()
    if(TARGET sad-fmt)
        install(TARGETS sad-fmt RUNTIME DESTINATION bin COMPONENT tools)
    endif()

    message(STATUS "✓ خادم LSP / LSP Server: sad-lsp-server")
    message(STATUS "✓ أداة التنسيق / Formatter: sad-fmt")
endif()

# ──────────────────────────────────────────────────────────────────────
# مدير الحزم / Package Manager
# ──────────────────────────────────────────────────────────────────────
option(BUILD_PKG_MANAGER "بناء مدير الحزم / Build package manager" ON)

if(BUILD_PKG_MANAGER)
    add_executable(sad-pkg tools/pkg/cli_v2.cpp)

    target_include_directories(sad-pkg PRIVATE
        ${CMAKE_SOURCE_DIR}/tools/pkg ${CMAKE_SOURCE_DIR}/include)

    if(WIN32)
        target_link_libraries(sad-pkg PRIVATE winhttp)
    endif()

    set_target_properties(sad-pkg PROPERTIES
        OUTPUT_NAME "sad-pkg" RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

    install(TARGETS sad-pkg RUNTIME DESTINATION bin COMPONENT tools)
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