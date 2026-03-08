# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/install.cmake
# الوصف: قواعد التثبيت وإعداد CPack / Installation Rules & CPack Configuration
# ═══════════════════════════════════════════════════════════════════════════════
#
# يجمّع جميع قواعد install() في مكان واحد ويضيف إعداد CPack
# لتوليد حزم .deb و .rpm و NSIS مباشرة من CMake.
#
# الاستخدام:
#   cmake --build build --target package        # توليد حزمة
#   cmake --install build --prefix /usr/local   # تثبيت محلي
#
# ═══════════════════════════════════════════════════════════════════════════════

include(GNUInstallDirs)

# ══════════════════════════════════════════════════════════════════════
# ① قواعد التثبيت — البرامج التنفيذية / Install Rules — Executables
# ══════════════════════════════════════════════════════════════════════

# المفسر الرئيسي / Main Interpreter
install(TARGETS sad
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT interpreter
)

# المترجم sadc (إذا تم بناؤه) / Compiler sadc (if built)
if(TARGET sadc)
    install(TARGETS sadc
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT compiler
    )
endif()

# خادم LSP / LSP Server
if(TARGET sad-lsp-server)
    install(TARGETS sad-lsp-server
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT tools
    )
endif()

# أداة التنسيق / Code Formatter
if(TARGET sad-fmt)
    install(TARGETS sad-fmt
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT tools
    )
endif()

# مدير الحزم / Package Manager
if(TARGET sad-pkg)
    install(TARGETS sad-pkg
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT tools
    )
endif()

# أداة iOS / iOS Tool
if(TARGET sad-ios)
    install(TARGETS sad-ios
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT tools
    )
endif()

# أداة Android / Android Tool
if(TARGET sad-android)
    install(TARGETS sad-android
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT tools
    )
endif()

# ══════════════════════════════════════════════════════════════════════
# ② المكتبة القياسية / Standard Library
# ══════════════════════════════════════════════════════════════════════

install(DIRECTORY ${CMAKE_SOURCE_DIR}/stdlib/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/sad-lang/stdlib
    COMPONENT stdlib
    FILES_MATCHING
        PATTERN "*.ص"
        PATTERN "*.h"
        PATTERN "*.cpp"
        PATTERN "*.md"
    PATTERN "node_modules" EXCLUDE
    PATTERN "__pycache__" EXCLUDE
    PATTERN ".git" EXCLUDE
)

# ══════════════════════════════════════════════════════════════════════
# ③ الوثائق / Documentation
# ══════════════════════════════════════════════════════════════════════

install(FILES
    ${CMAKE_SOURCE_DIR}/README.md
    ${CMAKE_SOURCE_DIR}/LICENSE
    ${CMAKE_SOURCE_DIR}/CHANGELOG.md
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
    COMPONENT docs
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/docs/book/
    DESTINATION ${CMAKE_INSTALL_DOCDIR}/book
    COMPONENT docs
    FILES_MATCHING PATTERN "*.md"
)

# ══════════════════════════════════════════════════════════════════════
# ④ أمثلة / Examples
# ══════════════════════════════════════════════════════════════════════

install(DIRECTORY ${CMAKE_SOURCE_DIR}/examples/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/sad-lang/examples
    COMPONENT examples
    FILES_MATCHING PATTERN "*.ص"
)

# ══════════════════════════════════════════════════════════════════════
# ⑤ SDL2 DLL (Windows فقط) / SDL2 DLL (Windows only)
# ══════════════════════════════════════════════════════════════════════

if(WIN32)
    set(_SDL2_DLL "${CMAKE_SOURCE_DIR}/graphics/third_party/SDL2/SDL2-2.28.5/lib/x64/SDL2.dll")
    if(EXISTS "${_SDL2_DLL}")
        install(FILES "${_SDL2_DLL}"
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT interpreter
        )
    endif()
endif()

# ══════════════════════════════════════════════════════════════════════
# ⑥ إعداد CPack — توليد حزم التوزيع / CPack Configuration
# ══════════════════════════════════════════════════════════════════════

set(CPACK_PACKAGE_NAME "sad-lang")
set(CPACK_PACKAGE_VENDOR "Sad Language Team")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "لغة ص — لغة برمجة عربية حديثة / Sad — Modern Arabic Programming Language")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://sad-lang.org")
set(CPACK_PACKAGE_CONTACT "team@sad-lang.org")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/website/docs/public/sad-icon.png")

# ── مكونات الحزمة / Package Components
set(CPACK_COMPONENTS_ALL interpreter compiler tools stdlib docs examples)

set(CPACK_COMPONENT_INTERPRETER_DISPLAY_NAME "المفسر / Interpreter (sad)")
set(CPACK_COMPONENT_INTERPRETER_DESCRIPTION  "المفسر الرئيسي لتشغيل ملفات .ص")
set(CPACK_COMPONENT_INTERPRETER_REQUIRED ON)

set(CPACK_COMPONENT_COMPILER_DISPLAY_NAME "المترجم / Compiler (sadc)")
set(CPACK_COMPONENT_COMPILER_DESCRIPTION  "مترجم LLVM لتوليد ملفات تنفيذية أصلية")

set(CPACK_COMPONENT_TOOLS_DISPLAY_NAME "الأدوات / Tools")
set(CPACK_COMPONENT_TOOLS_DESCRIPTION  "LSP server, formatter, package manager")

set(CPACK_COMPONENT_STDLIB_DISPLAY_NAME "المكتبة القياسية / Standard Library")
set(CPACK_COMPONENT_STDLIB_DESCRIPTION  "المكتبات العربية: io, math, string, network, graphics")

set(CPACK_COMPONENT_DOCS_DISPLAY_NAME "التوثيق / Documentation")
set(CPACK_COMPONENT_DOCS_DESCRIPTION  "الكتاب التعليمي والمراجع")

set(CPACK_COMPONENT_EXAMPLES_DISPLAY_NAME "الأمثلة / Examples")
set(CPACK_COMPONENT_EXAMPLES_DESCRIPTION  "برامج .ص نموذجية")

# ── DEB (Debian/Ubuntu) ──
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sad Language Team <team@sad-lang.org>")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.31), libstdc++6 (>= 10)")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://sad-lang.org")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)

# ── RPM (Fedora/RHEL/openSUSE) ──
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Development/Languages")
set(CPACK_RPM_PACKAGE_URL "https://sad-lang.org")
set(CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.31, libstdc++ >= 10")
set(CPACK_RPM_FILE_NAME RPM-DEFAULT)

# ── NSIS (Windows) ──
set(CPACK_NSIS_DISPLAY_NAME "لغة ص — Sad Programming Language")
set(CPACK_NSIS_PACKAGE_NAME "Sad Language")
set(CPACK_NSIS_URL_INFO_ABOUT "https://sad-lang.org")
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/website/docs/public/sad-icon.ico")

# ── اختيار المولدات تلقائياً / Auto-select generators by platform ──
if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
endif()

include(CPack)

message(STATUS "✓ قواعد التثبيت و CPack / Install rules & CPack configured")
