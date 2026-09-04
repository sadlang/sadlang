# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/install.cmake
# الوصف: قواعد التثبيت وإعداد CPack / Installation Rules & CPack Configuration
# ═══════════════════════════════════════════════════════════════════════════════
#
# (AR) يجمع جميع قواعد install() في مكان واحد ويضيف إعداد CPack
#      المكونات مصممة وفق docs/architecture-tools-hub.md (3 مستويات توزيع):
#        فردي   : sad-runtime, sad-compiler, sad-fmt, sad-check
#        إضافي  : sad-tools-extra (lsp, pkg, analyze)
#        حزمة   : sad-developer (runtime+compiler+fmt+check+tools-extra)
#        كل شيء : sad-everything (الحزمة + ios + android + docs + examples)
#
# (EN) Centralizes install() rules and configures CPack.
#      Components match docs/architecture-tools-hub.md (3 distribution tiers).
#
# الاستخدام / Usage:
#   cmake --build build --target package        # توليد حزمة / build packages
#   cmake --install build --prefix /usr/local   # تثبيت محلي / local install
# ═══════════════════════════════════════════════════════════════════════════════

include(GNUInstallDirs)

# ══════════════════════════════════════════════════════════════════════
# (AR) المكوّن hub: نقطة الدخول الموحَّدة sad — اسمٌ واحدٌ لا نسخةٌ ولقب.
# (EN) Component hub: the single entry point `sad` — one name, no alias copy.
# ══════════════════════════════════════════════════════════════════════
if(TARGET sad)
    install(TARGETS sad
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT hub
    )
endif()

# ══════════════════════════════════════════════════════════════════════
# (AR) المكوّن runtime: المكتبة القياسية
#      🔑 كان الاسمُ يصفُ «المفسر sad-run + المكتبة القياسية»، والكتلةُ
#      تحتَه لا تُركّبُ إلّا `stdlib/` ومكتباتِ الميزات. فالمكوّنُ يحملُ اسمَ
#      محرّكٍ محذوفٍ ولا يشحنُ منه شيئًا.
# (EN) Component runtime: the standard library. The comment described
#      "interpreter sad-run + standard library" while the block below installs
#      only stdlib/ and the feature stdlibs: the component carried the name of
#      a deleted engine and shipped none of it.
# ══════════════════════════════════════════════════════════════════════

install(DIRECTORY ${CMAKE_SOURCE_DIR}/stdlib/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/sad-lang/stdlib
    COMPONENT runtime
    FILES_MATCHING
        PATTERN "*.ص"
        PATTERN "*.h"
        PATTERN "*.cpp"
        PATTERN "*.md"
    PATTERN "node_modules" EXCLUDE
    PATTERN "__pycache__" EXCLUDE
    PATTERN ".git" EXCLUDE
)

# (AR) وحدات stdlib الخاصّة بالميزات (RFC #19): تُجمَّع في نفس مجلّد stdlib المُثبَّت
#      كي يجدها محلّل الوحدات (الذي يصعد المجلدات بحثاً عن stdlib/) في التوزيعة.
#      مثال: features/graphics/stdlib/رسومات.ص ⇐ <datadir>/sad-lang/stdlib/رسومات.ص.
# (EN) Per-feature stdlib modules (RFC #19): staged into the SAME installed stdlib dir
#      so the module resolver (which climbs to find stdlib/) locates them in the
#      distribution. e.g. features/graphics/stdlib/رسومات.ص ⇒ <datadir>/sad-lang/stdlib/.
install(DIRECTORY ${CMAKE_SOURCE_DIR}/features/graphics/stdlib/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/sad-lang/stdlib
    COMPONENT runtime
    FILES_MATCHING
        PATTERN "*.ص"
)

# (AR) الشبكة الكلاسيكيّة انتقلت: stdlib/network ⟵ features/network/core (RFC sadlang-rfcs#10 §أ)؛
#      تُثبَّت في نفس موضعها القديم داخل التوزيعة حفظًا لتكافؤ المحتوى.
# (EN) Classic network moved: stdlib/network ⟶ features/network/core (RFC sadlang-rfcs#10 §أ);
#      staged at its old distribution path to keep content parity.
install(DIRECTORY ${CMAKE_SOURCE_DIR}/features/network/core/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/sad-lang/stdlib/network
    COMPONENT runtime
    FILES_MATCHING
        PATTERN "*.ص"
        PATTERN "*.h"
        PATTERN "*.cpp"
        PATTERN "*.md"
)

# (AR) SDL2 DLL مطلوبة للمفسر على ويندوز
# (EN) SDL2 DLL required for interpreter on Windows
if(WIN32)
    set(_SDL2_DLL "${CMAKE_SOURCE_DIR}/features/graphics/third_party/SDL2/SDL2-2.28.5/lib/x64/SDL2.dll")
    if(EXISTS "${_SDL2_DLL}")
        install(FILES "${_SDL2_DLL}"
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT runtime
        )
    endif()
endif()

# ══════════════════════════════════════════════════════════════════════
# (AR) المكوّن compiler: المترجم الأصلي sad-build (LLVM)
# (EN) Component compiler: native compiler sad-build (LLVM)
# ══════════════════════════════════════════════════════════════════════
if(TARGET sad-build)
    install(TARGETS sad-build
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT compiler
    )
endif()

# ══════════════════════════════════════════════════════════════════════
# (AR) المكوّن fmt: منسّق الكود sad-fmt
# (EN) Component fmt: code formatter sad-fmt
# ══════════════════════════════════════════════════════════════════════
if(TARGET sad-fmt)
    install(TARGETS sad-fmt
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT fmt
    )
endif()

# ══════════════════════════════════════════════════════════════════════
# (AR) المكوّن check: فاحص الملكية الثابت sad-check
# (EN) Component check: static ownership checker sad-check
# ══════════════════════════════════════════════════════════════════════
if(TARGET sad-check)
    install(TARGETS sad-check
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT check
    )
endif()

# ══════════════════════════════════════════════════════════════════════
# (AR) المكوّن tools-extra: أدوات إضافية (LSP، مدير الحزم، Analyzer)
#      🔑 كان مكتوبًا هنا «repl, profiler» والحلقةُ تحتَه لا تُركّبُ إلّا
#      ثلاثةَ أهدافٍ ولا هدفَ باسمِ أيٍّ منهما في المستودعِ كلِّه. فالتعليقُ
#      كان نسخةً من قائمةٍ تحتَه مباشرةً — وتباعدَ عنها.
#      ⚠️ والنُّسَخُ **ثلاثٌ لا اثنتان**: كُتِبَ هنا أوّلًا «نسخةً ثانية»
#      فأُصلحت اثنتان وبقيت الثالثةُ في ترويسةِ الملفِّ (السطر ١٠) تعدُّهما
#      عضوَين في `sad-tools-extra`. عُدَّتِ النُّسَخُ وصُحّحت الثلاث.
# (EN) Component tools-extra: extra tools (LSP, package manager, analyzer).
#      The comment used to say "repl, profiler" while the loop below
#      installs three targets and neither name exists as a target anywhere.
#      It was a copy of the list directly beneath it, and it drifted. There
#      were THREE copies, not two — this comment first said "a second copy",
#      so two were fixed and the third survived in the file header (line 10),
#      which still counted both as members of sad-tools-extra. Counted and
#      corrected in all three places.
# ══════════════════════════════════════════════════════════════════════
foreach(_tool sad-lsp sad-pkg sad-analyze)
    if(TARGET ${_tool})
        install(TARGETS ${_tool}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT tools-extra
        )
    endif()
endforeach()

# ══════════════════════════════════════════════════════════════════════
# (AR) المكوّن mobile: أدوات الهاتف (iOS + Android + APK)
# (EN) Component mobile: mobile tools (iOS + Android + APK)
# ══════════════════════════════════════════════════════════════════════
foreach(_tool sad-ios sad-android sad-apk)
    if(TARGET ${_tool})
        install(TARGETS ${_tool}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT mobile
        )
    endif()
endforeach()

# ══════════════════════════════════════════════════════════════════════
# (AR) المكوّن docs: التوثيق والكتاب التعليمي
# (EN) Component docs: documentation and reference book
# ══════════════════════════════════════════════════════════════════════
install(FILES
    ${CMAKE_SOURCE_DIR}/README.md
    ${CMAKE_SOURCE_DIR}/LICENSE
    ${CMAKE_SOURCE_DIR}/CHANGELOG.md
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
    COMPONENT docs
)

if(EXISTS "${CMAKE_SOURCE_DIR}/docs/book")
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/docs/book/
        DESTINATION ${CMAKE_INSTALL_DOCDIR}/book
        COMPONENT docs
        FILES_MATCHING PATTERN "*.md"
    )
endif()

# (AR) 🔑 حُذفت قاعدةُ تركيبِ `docs/architecture-tools-hub.md`.
#      وهي وثيقةُ **تصميمٍ داخليّةٌ** (مقترحٌ معتمَدٌ بتاريخِ يناير ٢٠٢٦،
#      فيه «معايير قبول» و«تقييم مخاطر» و«خطّة تنفيذ») كانت تُشحَنُ داخلَ
#      حزمةِ المستخدم. ومنذُ حذفِ المفسّرِ صارت تصفُ للمستخدمِ منتجًا
#      لا وجودَ له: `sad-run.exe` و`sad-repl.exe` ومكوّنَ `sad-runtime`.
#      والوثيقةُ باقيةٌ في الشجرةِ سجلًّا للقرار — والسجلُّ لا يُشحَن.
# (EN) The install rule for docs/architecture-tools-hub.md was removed. It
#      is an INTERNAL design document — an approved January 2026 proposal
#      with acceptance criteria, a risk table and a rollout plan — and it
#      was being shipped inside the user package. Since the interpreter was
#      deleted it describes a product that does not exist: sad-run.exe,
#      sad-repl.exe, a sad-runtime component. The document stays in the tree
#      as the record of a decision; a record is not shipped.

# ══════════════════════════════════════════════════════════════════════
# (AR) المكوّن examples: ملفات .ص نموذجية
# (EN) Component examples: sample .ص files
# ══════════════════════════════════════════════════════════════════════
install(DIRECTORY ${CMAKE_SOURCE_DIR}/examples/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/sad-lang/examples
    COMPONENT examples
    FILES_MATCHING PATTERN "*.ص"
)

# ══════════════════════════════════════════════════════════════════════
# (AR) إعداد CPack — توليد حزم التوزيع
# (EN) CPack Configuration — distribution package generation
# ══════════════════════════════════════════════════════════════════════
set(CPACK_PACKAGE_NAME "sad-lang")
set(CPACK_PACKAGE_VENDOR "Sad Language Team")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "لغة ص — لغة برمجة عربية حديثة / Sad — Modern Arabic Programming Language")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://sad-lang.org")
set(CPACK_PACKAGE_CONTACT "team@sad-lang.org")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
if(EXISTS "${CMAKE_SOURCE_DIR}/website/docs/public/sad-icon.png")
    set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/website/docs/public/sad-icon.png")
endif()

# ──────────────────────────────────────────────────────────────────────
# (AR) المكوّنات (granular components — مستوى التوزيع الفردي)
#      نبنيها ديناميكياً بحيث لا تُدرج إلا المكوّنات التي تم بناء أهدافها
# (EN) Components (granular — individual distribution tier)
#      Built dynamically — only include components whose targets exist
# ──────────────────────────────────────────────────────────────────────
set(CPACK_COMPONENTS_ALL hub runtime fmt check tools-extra docs examples)
if(TARGET sad-build)
    list(APPEND CPACK_COMPONENTS_ALL compiler)
endif()
# (AR) mobile فقط إذا تم بناء أحد أدواتها
# (EN) mobile only if at least one mobile tool built
foreach(_t sad-ios sad-android sad-apk)
    if(TARGET ${_t})
        list(APPEND CPACK_COMPONENTS_ALL mobile)
        break()
    endif()
endforeach()
list(REMOVE_DUPLICATES CPACK_COMPONENTS_ALL)

# ── (AR) عرض المكونات / (EN) component metadata ─────────────────────
set(CPACK_COMPONENT_HUB_DISPLAY_NAME "الواجهة الموحَّدة / Hub (sad)")
set(CPACK_COMPONENT_HUB_DESCRIPTION  "(AR) نقطة الدخول الموحَّدة sad.exe — تستدعي بقية الأدوات\n(EN) Unified entry point sad.exe — dispatches to other tools")
set(CPACK_COMPONENT_HUB_REQUIRED ON)

set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "زمن التشغيل / Runtime")
set(CPACK_COMPONENT_RUNTIME_DESCRIPTION  "(AR) المكتبة القياسية ومكتبات وقت التشغيل
(EN) Standard library and runtime archives")

set(CPACK_COMPONENT_COMPILER_DISPLAY_NAME "المترجم / Compiler (sad-build)")
set(CPACK_COMPONENT_COMPILER_DESCRIPTION  "(AR) مترجم LLVM لتوليد ملفات تنفيذية أصلية\n(EN) LLVM compiler producing native executables")

set(CPACK_COMPONENT_FMT_DISPLAY_NAME "المنسّق / Formatter (sad-fmt)")
set(CPACK_COMPONENT_FMT_DESCRIPTION  "(AR) منسّق كود ص الرسمي\n(EN) Official Sad code formatter")

set(CPACK_COMPONENT_CHECK_DISPLAY_NAME "فاحص الملكية / Checker (sad-check)")
set(CPACK_COMPONENT_CHECK_DESCRIPTION  "(AR) فاحص ملكية ثابت — يكتشف use-after-move ومشاكل العمر\n(EN) Static ownership checker — detects use-after-move and lifetime issues")

set(CPACK_COMPONENT_TOOLS-EXTRA_DISPLAY_NAME "أدوات إضافية / Extra Tools")
# (AR) ⚠️ هذا الوصفُ يُعرَضُ في نافذةِ المُثبِّتِ عندَ اختيارِ المكوّنات،
#      فكان يَعِدُ المستخدمَ بـREPL وProfiler ثمّ لا يُنسَخُ أيٌّ منهما.
# (EN) This text is shown in the installer component picker, so it promised
#      the user a REPL and a profiler that are then never copied.
set(CPACK_COMPONENT_TOOLS-EXTRA_DESCRIPTION  "(AR) خادم LSP، مدير الحزم، أداة التحليل\n(EN) LSP server, package manager, analyzer")

set(CPACK_COMPONENT_MOBILE_DISPLAY_NAME "أدوات الهاتف / Mobile")
set(CPACK_COMPONENT_MOBILE_DESCRIPTION  "(AR) بناة iOS و Android و APK\n(EN) iOS, Android, and APK builders")

set(CPACK_COMPONENT_DOCS_DISPLAY_NAME "التوثيق / Documentation")
set(CPACK_COMPONENT_DOCS_DESCRIPTION  "(AR) الكتاب التعليمي والمراجع\n(EN) Tutorial book and references")

set(CPACK_COMPONENT_EXAMPLES_DISPLAY_NAME "الأمثلة / Examples")
set(CPACK_COMPONENT_EXAMPLES_DESCRIPTION  "(AR) برامج .ص نموذجية\n(EN) Sample .ص programs")

# ──────────────────────────────────────────────────────────────────────
# (AR) المجموعات (Groups — مستوى توزيع الحزم)
# (EN) Groups (Groups — bundle distribution tier)
# ──────────────────────────────────────────────────────────────────────
set(CPACK_COMPONENT_HUB_GROUP "Core")
set(CPACK_COMPONENT_RUNTIME_GROUP "Core")
set(CPACK_COMPONENT_COMPILER_GROUP "Compiler")
set(CPACK_COMPONENT_FMT_GROUP "Tools")
set(CPACK_COMPONENT_CHECK_GROUP "Tools")
set(CPACK_COMPONENT_TOOLS-EXTRA_GROUP "Tools")
set(CPACK_COMPONENT_MOBILE_GROUP "Mobile")
set(CPACK_COMPONENT_DOCS_GROUP "Docs")
set(CPACK_COMPONENT_EXAMPLES_GROUP "Docs")

set(CPACK_COMPONENT_GROUP_CORE_DISPLAY_NAME "النواة / Core")
set(CPACK_COMPONENT_GROUP_CORE_DESCRIPTION  "(AR) الحد الأدنى لتشغيل برامج ص\n(EN) Minimum to run Sad programs")

set(CPACK_COMPONENT_GROUP_COMPILER_DISPLAY_NAME "مترجم / Compiler")
set(CPACK_COMPONENT_GROUP_TOOLS_DISPLAY_NAME "الأدوات / Tools")
set(CPACK_COMPONENT_GROUP_MOBILE_DISPLAY_NAME "الهاتف / Mobile")
set(CPACK_COMPONENT_GROUP_DOCS_DISPLAY_NAME "التوثيق / Docs")

# ──────────────────────────────────────────────────────────────────────
# (AR) DEB (Debian/Ubuntu)
# ──────────────────────────────────────────────────────────────────────
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sad Language Team <team@sad-lang.org>")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.31), libstdc++6 (>= 10)")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://sad-lang.org")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
# (AR) حزمة DEB لكل مكوّن (granular)
# (EN) per-component DEB packages
set(CPACK_DEB_COMPONENT_INSTALL ON)

# ──────────────────────────────────────────────────────────────────────
# (AR) RPM (Fedora/RHEL/openSUSE)
# ──────────────────────────────────────────────────────────────────────
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Development/Languages")
set(CPACK_RPM_PACKAGE_URL "https://sad-lang.org")
set(CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.31, libstdc++ >= 10")
set(CPACK_RPM_FILE_NAME RPM-DEFAULT)
set(CPACK_RPM_COMPONENT_INSTALL ON)

# ──────────────────────────────────────────────────────────────────────
# (AR) NSIS (Windows) — مع شاشة اختيار المكوّنات
# (EN) NSIS (Windows) — with component-selection screen
# ──────────────────────────────────────────────────────────────────────
set(CPACK_NSIS_DISPLAY_NAME "لغة ص — Sad Programming Language")
set(CPACK_NSIS_PACKAGE_NAME "Sad Language")
set(CPACK_NSIS_URL_INFO_ABOUT "https://sad-lang.org")
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
if(EXISTS "${CMAKE_SOURCE_DIR}/website/docs/public/sad-icon.ico")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/website/docs/public/sad-icon.ico")
endif()
# (AR) عرض شاشة اختيار المكوّنات
# (EN) Show component selection screen
set(CPACK_NSIS_COMPONENT_INSTALL ON)

# ──────────────────────────────────────────────────────────────────────
# (AR) ZIP / TGZ — حزم محمولة لكل مكوّن
# (EN) ZIP / TGZ — portable per-component archives
# ──────────────────────────────────────────────────────────────────────
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

# ──────────────────────────────────────────────────────────────────────
# (AR) اختيار المولدات تلقائياً حسب المنصة
# (EN) Auto-select generators by platform
# ──────────────────────────────────────────────────────────────────────
if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
endif()

include(CPack)

message(STATUS "✓ قواعد التثبيت و CPack / Install rules & CPack configured")
message(STATUS "  المكوّنات / Components: hub, runtime, compiler, fmt, check, tools-extra, mobile, docs, examples")
