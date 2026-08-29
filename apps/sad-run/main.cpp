// بسم الله الرحمن الرحيم
// sad - Sad Language Interpreter
// مفسر لغة ص البرمجية العربية
// Simple main.cpp that uses the interpreter directly

#include "lexer_core.h"
#include "parser_core.h"
#include "interpreter_core.h"
#include "error_manager.h"
#include "error_hints.h"
#include "explanation_level.h"
#include "value.h"
// (AR) مُستخرج التوثيق من شجرة AST — ميزة مدمجة في sad
// (EN) AST-based documentation extractor — built-in feature of sad
#include "docs_extractor.h"
#include "pdf_exporter.h"
// (AR) م3 شريحة2: بعد نقل main إلى apps/ صار المسار النسبيّ القديم لاغيًا؛
//      ترويستا المفسّر تُحَلّان عبر مسار التضمين interpreter/include المضاف للهدف.
// (EN) Phase-3 apps/ move: the old relative path is invalid here; these interpreter
//      headers resolve via the interpreter/include dir added to this target.
#include "user_thrown.h"
#include "debug/debug_server.h"
#ifndef SAD_ENABLE_GRAPHICS
#define SAD_ENABLE_GRAPHICS 1 // (AR) افتراضيًّا مفعّلة؛ يُعطّل ببناء headless
#endif
#if SAD_ENABLE_GRAPHICS
#include "ui/sad_graphics_bridge.h" // (AR) م2-أ: تثبيت جسر الواجهات (واجهة عامّة) / (EN) install UI bridge (public API)
#endif

// CLI Commands for mobile etc.
#include "cli_commands.hpp"

#include "cli_flags_generated.h" // (AR) المصدر الوحيد لأعلام الأدوات
#include "utf8_utils.h"
#include "profiler_core.h"                  // (AR) مصحح الأداء / (EN) Profiler
#include "profiler_hooks.h"                 // (AR) خطافات المصحح / (EN) Profiler hooks
#include "hot_reload_engine.h"              // (AR) محرك إعادة التحميل الساخن / (EN) Hot Reload Engine
#include "memory/policy/memory_mode_flag.h" // (AR) Phase A2: محلل أعلام سياسة الذاكرة / (EN) Phase A2: memory policy flag parser

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <clocale>
#include <vector>
#include <thread>
#include <chrono>
#include <filesystem>
#include <system_error>

#ifdef _WIN32
#include <windows.h>
#endif

// (AR) تصريحٌ مُسبَقٌ: نصُّ المساعدةِ يقرأُ أسماءَ الأعلامِ من المصدرِ الوحيدِ
//      كما يفعلُ مُحلِّلُ الأعلامِ نفسُه — والتعريفُ أسفلَ الملفّ.
// (EN) Forward declaration: the help text reads flag names from the same SoT
//      helper the parser uses; the definition lives further down.
static const char *sadCanonical(sad::cli::FlagAction action);

// طباعة المساعدة
// Print help
void print_help(const char *program_name)
{
    std::cout << "لغة ص - Sad Programming Language v1.0.0-beta.1\n"
              << "الاستخدام / Usage: " << program_name << " <ملف.ص>\n\n"
              << "الأوامر / Commands:\n"
              << "  <ملف.ص>       تنفيذ ملف / Execute file\n"
              << "\n"
              << "  أوامر فرعية / Subcommands:\n"
              << "  new/جديد  build/بناء  run/شغّل  test/اختبر  format/نسّق  check/تحقق\n"
              << "  doc/وثّق  add/أضف  publish/انشر  mobile/هاتف  help/مساعدة  version/نسخة\n"
              << "\n"
              // ─────────────────────────────────────────────────────────────
              // (AR) الأعلام الطويلة تُولَّد من المصدر الوحيد (cli_flags_generated.h):
              //      أعلام المفسّر + عائلة الذاكرة (يبتلعها الماسح المسبق فتعمل
              //      في sad-run). اسم عربيّ قانونيّ وحيد لكلّ علم — بلا مرادفات.
              // (EN) Long flags generated from the shared SoT: interpreter flags +
              //      the memory family (consumed by the pre-scanner). One canonical
              //      Arabic name each — no aliases.
              // ─────────────────────────────────────────────────────────────
              << "  أعلام طويلة — أسماء عربيّة قانونيّة وحيدة (بلا مرادفات):\n"
              << "  long flags — single canonical Arabic names (no aliases):\n";
    for (std::size_t i = 0; i < sad::cli::kFlagCount; ++i)
    {
        const auto &spec = sad::cli::kFlags[i];
        // (AR) اعرض ما يقبله المفسّر فقط: أعلامه + أعلام الذاكرة.
        // (EN) Show only what the interpreter accepts: its flags + memory family.
        if (!spec.for_interpreter && !spec.for_memory)
        {
            continue;
        }
        std::string name = spec.canonical;
        if (spec.kind == sad::cli::FlagKind::Value && spec.value_hint[0] != '\0')
        {
            name += "=<";
            name += spec.value_hint;
            name += ">";
        }
        std::cout << "    " << name << "\n"
                  << "        " << spec.desc_ar << " / " << spec.desc_en << "\n";
    }
    // ─────────────────────────────────────────────────────────────────────
    // (AR) 🔑 وصفُ العلمِ وحدَه لا يُخبِرُ عن حالتِه الافتراضيّة. فسطرُ
    //      `--فحص-الأنواع` يقولُ «تفعيل» فيُقرَأُ أنّ الفحصَ مُطفأٌ حتّى يُطلَب،
    //      وهو مرفوعٌ ابتداءً منذ ٢٦ آب ٢٠٢٦. **والوصفُ الصادقُ لا يكفي إن
    //      كانَ ما يسكتُ عنه يُقلَبُ فهمُه.** والاسمانِ يُقرآنِ من المصدرِ
    //      الوحيدِ لا يُكتَبانِ هنا، كي لا يتباعدَ نصُّ المساعدةِ عن العَلَم.
    // (EN) A flag's description does not state its default: "--فحص-الأنواع →
    //      enable" reads as "off until asked", yet it has been on by default
    //      since 2026-08-26. Flag spellings are read from the SoT, never typed
    //      here, so help text cannot drift from the flags themselves.
    // ─────────────────────────────────────────────────────────────────────
    std::cout << "\n"
              << "  فحص الأنواع مرفوع افتراضيًّا في المفسّر؛ "
              << sadCanonical(sad::cli::FlagAction::TypeCheckOff) << " يُطفئه.\n"
              << "  Type checking is ON by default here; "
              << sadCanonical(sad::cli::FlagAction::TypeCheckOff) << " turns it off.\n";
    std::cout << "\n"
              << "  توليد الواجهات الرسومية يتم عبر المترجم / UI generation lives in the compiler:\n"
              << "    sad-build --واجهات <ملف.ص>\n"
              << std::endl;
}

// قراءة ملف
// Read file
std::string read_file(const std::string &filename)
{
    // (AR) استخدام utf8_open_ifstream لدعم أسماء الملفات العربية
    // (EN) Use utf8 open for Arabic filename support
    auto file = sad::utf8::open_ifstream(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("لم يتم العثور على الملف: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


// ════════════════════════════════════════════════════════════════════════════
// (AR) مساعدان يقرآن أسماء الأعلام من المصدر الوحيد المولَّد — لا سلاسل حرّة.
//      sadCanonical: الاسم القانونيّ لإجراءٍ ما.
//      sadValueOf  : يطابق «الاسم=قيمة» ويستخرج القيمة.
// (EN) Helpers reading flag names from the generated SoT — no raw literals.
// ════════════════════════════════════════════════════════════════════════════
static const char *sadCanonical(sad::cli::FlagAction action)
{
    for (std::size_t i = 0; i < sad::cli::kFlagCount; ++i)
    {
        if (sad::cli::kFlags[i].action == action)
            return sad::cli::kFlags[i].canonical;
    }
    return "";
}

static bool sadValueOf(const std::string &arg, sad::cli::FlagAction action, std::string &out)
{
    const std::string prefix = std::string(sadCanonical(action)) + "=";
    if (arg.size() > prefix.size() && arg.compare(0, prefix.size(), prefix) == 0)
    {
        out = arg.substr(prefix.size());
        return true;
    }
    return false;
}

int main(int argc, char *argv[])
{
    // إعداد دعم UTF-8 للعربية / Setup UTF-8 support for Arabic
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    // (AR) ضبط الـ locale لدعم UTF-8 في std::cout
    // (EN) Set locale for UTF-8 support in std::cout
    setlocale(LC_ALL, ".UTF-8");
    // (AR) الحصول على معاملات UTF-8 لدعم أسماء الملفات العربية
    // (EN) Get UTF-8 args for Arabic filename support
    auto utf8_args = sad::utf8::get_utf8_args();
    std::vector<char *> new_argv;
    for (auto &s : utf8_args)
        new_argv.push_back(s.data());
    argc = static_cast<int>(new_argv.size());
    argv = new_argv.data();
#else
    std::setlocale(LC_ALL, "");
#endif

    // (AR) م2-أ (sadlang-rfcs#10): تثبيت جسر الواجهات قبل إنشاء أيّ مفسّر — يسجّل
    //      بذرة تقييم الواجهات ومزوّد وحدات الرسومات في القلب. القلب نفسه لا يعرف
    //      sad_ui؛ هذا الثنائيّ (sad-run) وحده يربط الجسر.
    // (EN) Phase 2-A: install the UI bridge before constructing any interpreter —
    //      registers the widget-eval seam + graphics module provider into the core.
    //      Only sad-run links the bridge; the core itself does not know sad_ui.
#if SAD_ENABLE_GRAPHICS
    Sad::Interpreter::installSadGraphicsBridge();
#endif

    // Check arguments
    if (argc < 2)
    {
        print_help(argv[0]);
        return 1;
    }

    std::string arg = argv[1];

    // (AR) المساعدة/الإصدار: الاسم العربيّ القانونيّ من المصدر الوحيد، مع بقاء
    //      -h القصير القياسيّ عرفًا. (EN) Help/version: canonical Arabic name from
    //      the SoT; the standard short -h stays as a convention.
    if (arg == sadCanonical(sad::cli::FlagAction::HelpRequested) ||
        arg == sad::cli::short_flags::HelpShort)
    {
        print_help(argv[0]);
        return 0;
    }

    if (arg == sadCanonical(sad::cli::FlagAction::VersionRequested))
    {
        std::cout << "sad version 1.0.0-beta.1\n";
        return 0;
    }

    // ═══════════════════════════════════════════════════════════════════════
    // التحقق من أوامر CLI (هاتف/mobile, مساعدة/help, نسخة/version)
    // Check for CLI commands before falling through to file execution
    // ═══════════════════════════════════════════════════════════════════════
    if (arg == "mobile" || arg == "\xD9\x87\xD8\xA7\xD8\xAA\xD9\x81" /* هاتف */ ||
        arg == "build" || arg == "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1" /* بناء */ ||
        arg == "test" || arg == "\xD8\xA7\xD8\xAE\xD8\xAA\xD8\xA8\xD8\xB1" /* اختبر */ ||
        arg == "run" || arg == "\xD8\xB4\xD8\xBA\xD9\x91\xD9\x84" /* شغّل */ ||
        arg == "new" || arg == "\xD8\xAC\xD8\xAF\xD9\x8A\xD8\xAF" /* جديد */ ||
        arg == "help" || arg == "\xD9\x85\xD8\xB3\xD8\xA7\xD8\xB9\xD8\xAF\xD8\xA9" /* مساعدة */ ||
        arg == "version" || arg == "\xD9\x86\xD8\xB3\xD8\xAE\xD8\xA9" /* نسخة */ ||
        arg == "format" || arg == "\xD9\x86\xD8\xB3\xD9\x91\xD9\x82" /* نسّق */ ||
        arg == "check" || arg == "\xD8\xAA\xD8\xAD\xD9\x82\xD9\x82" /* تحقق */ ||
        arg == "doc" || arg == "\xD9\x88\xD8\xAB\xD9\x91\xD9\x82" /* وثّق */ ||
        arg == "add" || arg == "\xD8\xA3\xD8\xB6\xD9\x81" /* أضف */ ||
        arg == "publish" || arg == "\xD8\xA7\xD9\x86\xD8\xB4\xD8\xB1" /* انشر */)
    {

        sad::cli::CommandManager manager;
        return manager.run(argc, argv);
    }

    // ═══════════════════════════════════════════════════════════════════════
    // (AR) Phase A2: تحليل أعلام سياسة الذاكرة (--dev/--prod/--learn/--gc=...)
    //               قبل الـ argv loop العادي حتى يستهلك المحلل أعلامه ويُعيد
    //               الباقي فقط (لتجنّب ظهور "علم غير معروف" لاحقاً).
    // (EN) Phase A2: Parse memory policy flags (--dev/--prod/--learn/--gc=...)
    //               BEFORE the main argv loop so the parser consumes its flags
    //               and returns only the remainder (avoids spurious "unknown flag").
    // ═══════════════════════════════════════════════════════════════════════
    ::Sad::Memory::MemoryModeSettings g_memoryPolicy{};
    bool g_memoryPolicySet = false;
    std::vector<std::string> g_memoryRemaining; // (AR) لإبقاء الذاكرة حية / (EN) keep storage alive
    std::vector<char *> g_memoryArgvStorage;
    {
        ::Sad::Memory::MemoryModeFlag memFlagParser;
        auto memResult = memFlagParser.parse(argc, argv);
        if (!memResult.success)
        {
            for (const auto &err : memResult.errors)
                std::cerr << "خطأ سياسة الذاكرة / Memory policy error: " << err << "\n";
            return 1;
        }
        for (const auto &warn : memResult.warnings)
            std::cerr << "تحذير سياسة الذاكرة / Memory policy warning: " << warn << "\n";

        // (AR) إذا حُذف على الأقل علم واحد فقد ضُبطت السياسة صراحةً.
        // (EN) If at least one flag was consumed the policy was explicitly set.
        // (AR) ملاحظة: parse(argc, argv) يتجاوز argv[0]، لذا remainingArgs لا يضم
        //      اسم البرنامج. عدد الأعلام المستهلكة = (argc - 1) - remainingArgs.size().
        // (EN) Note: parse(argc, argv) skips argv[0], so remainingArgs excludes
        //      the program name. Consumed = (argc - 1) - remainingArgs.size().
        const size_t originalUserArgs = static_cast<size_t>(argc > 0 ? argc - 1 : 0);
        if (memResult.remainingArgs.size() != originalUserArgs)
        {
            g_memoryPolicy = memResult.settings;
            g_memoryPolicySet = true;
            // (AR) إعادة بناء argc/argv: argv[0] (اسم البرنامج) + الأعلام المتبقية.
            // (EN) Rebuild argc/argv: argv[0] (program name) + remaining flags.
            g_memoryRemaining = std::move(memResult.remainingArgs);
            g_memoryArgvStorage.reserve(g_memoryRemaining.size() + 1);
            g_memoryArgvStorage.push_back(argv[0]);
            for (auto &s : g_memoryRemaining)
                g_memoryArgvStorage.push_back(s.data());
            argc = static_cast<int>(g_memoryArgvStorage.size());
            argv = g_memoryArgvStorage.data();
            if (argc < 2)
            {
                print_help(g_memoryArgvStorage[0]);
                return 1;
            }
            arg = argv[1];
        }
    }

    // Execute file
    try
    {
        // Parse CLI flags
        bool enableOwnership = false;
        bool debugOwnership = false;
        // ────────────────────────────────────────────────────────────────────
        // (AR) 🔑 مرفوعٌ افتراضيًّا بقرارِ المالك (٢٦ آب ٢٠٢٦). وكانَ `false`، فكانَ
        //      فاحصُ الأنواعِ **كلُّه** — لا حارسًا بعينِه — مُطفَأً في المسارِ
        //      الافتراضيِّ للمحرّكِ الثاني. وأثرُ ذلك أنّ مصفوفةَ التكافؤِ كانت
        //      تُقارِنُ مترجّمًا يفحصُ الأنواعَ بمفسّرٍ لا يفحصُها، فتُعلِنُ اتّفاقًا
        //      عن سؤالٍ لم يُسأَلْ أحدُ الطرفَين عنه أصلًا.
        //      وقِيسَ الثمنُ قبلَ الرفعِ لا بعدَه: ١٠ بذورٍ من ٤٥٢٣ تتبدّلُ، وكلُّها
        //      بذورٌ سالبةٌ يُنتظَرُ فشلُها — وصفرُ بذرةٍ خضراءَ تحمرّ.
        //      ومَن أرادَ المسارَ القديمَ فـ`--بلا-فحص-أنواع` يُنزِلُه.
        // (EN) 🔑 Default-on by owner ruling (2026-08-26). It was `false`, which left
        //      the WHOLE type checker — not one guard — off on the interpreter's
        //      default path, so the parity matrix compared a type-checking compiler
        //      against a non-type-checking interpreter and reported agreement on a
        //      question neither side had been asked. Cost measured before the flip:
        //      10 seeds of 4523 change, all of them negative seeds already expected
        //      to fail; zero green seeds turn red. `--بلا-فحص-أنواع` restores the
        //      old path.
        // ────────────────────────────────────────────────────────────────────
        bool enableTypeCheck = true;
        bool debugTypeCheck = false;
        bool strictTypeCheck = false;
        bool enableSecurity = false;
        bool debugSecurity = false;
        bool strictSecurity = false;
        bool enableDebug = false;
        bool showOptStats = false;
        bool useDebugServer = false;
        bool enableProfile = false;
        bool enableHotReload = false;
        // (AR) خيارات استخراج التوثيق / (EN) Documentation extraction options
        bool emitDocs = false;
        std::string docsOutput;
        // (AR) توثيق مشروع متعدد الملفات / (EN) Multi-file project docs
        std::string docsProject;
        std::string docsProjectName;
        // (AR) صيغة الإخراج: markdown (الافتراضي)، json، html
        // (EN) Output format: markdown (default), json, html
        std::string docsFormat = "markdown";
        // (AR) أنماط استبعاد لوضع المشروع / (EN) Exclude patterns for project mode
        std::vector<std::string> docsExcludes;
        std::string profileOutput;
        std::string profileFormat = "text";
        int profileTop = 20;
        // (AR) المرحلة 3 — أعلام شرح الخطأ ولغة الإخراج
        // (EN) Phase 3 — explanation level + output language flags
        std::string explainLevelStr; // brief|normal|detailed|teacher
        std::string langStr;         // ar|en|both
        std::string filename;
        for (int i = 1; i < argc; ++i)
        {
            std::string a = argv[i];
            std::string val;
            // ────────────────────────────────────────────────────────────────
            // (AR) أعلام المفسّر: أسماء عربيّة قانونيّة وحيدة من المصدر الوحيد
            //      (cli_flags_generated.h) — بلا مرادفات ولا توافق خلفيّ. توحّدت
            //      الملكية مع فاحص الاستعارة (--فحص-الاستعارة/--تنقيح-الاستعارة).
            // (EN) Interpreter flags: single canonical Arabic names from the shared
            //      SoT — no aliases. Ownership unified with the borrow checker.
            // ────────────────────────────────────────────────────────────────
            if (a == sadCanonical(sad::cli::FlagAction::BorrowCheckOn))
            {
                enableOwnership = true;
            }
            else if (a == sadCanonical(sad::cli::FlagAction::DebugBorrow))
            {
                enableOwnership = true;
                debugOwnership = true;
            }
            else if (a == sadCanonical(sad::cli::FlagAction::TypeCheckOn))
            {
                enableTypeCheck = true;
            }
            // (AR) 🔑 لم يكن لهذا العلمِ معالجٌ هنا قطُّ، وكانَ مصدرُ الحقيقةِ يحصرُه
            //      بالمترجّمِ مُعلِّلًا بأنّ الفحصَ مُطفأٌ في المفسّرِ أصلًا. فلمّا رُفِعَ
            //      الافتراضيُّ صارَ غيابُ المعالجِ حبسًا: لا سبيلَ للنزولِ عن الفحص.
            //      **وحقلُ المصدرِ النائمُ يصيرُ حامِلًا ساعةَ يتبدّلُ ما حولَه.**
            // (EN) 🔑 This flag never had a handler here; the SoT scoped it to the
            //      compiler because the interpreter's check was off anyway. Raising
            //      the default turned that absence into a trap — no way to opt out.
            //
            // (AR) ⚠️ ويُطفئُ معه تابعَيه. فالعلمان `--تنقيح-الأنواع` و`--أنواع-صارمة`
            //      يرفعان `enableTypeCheck` ضمنًا، فلو تُرِكَ تابعاهما مرفوعَين بعدَ
            //      الإطفاءِ لَخرجَت حالةٌ متناقضةٌ: «فحصٌ صارمٌ مُنقَّحٌ لا يُشغَّل».
            //      وهي اليومَ تُهمَلُ في `interpreter_core` لأنّه يسألُ عن
            //      `enableTypeCheck` أوّلًا — **وحقلٌ متناقضٌ يُهمَلُ اليومَ يصيرُ
            //      حامِلًا يومَ يُقرَأُ وحدَه**، فيُطفَأُ عندَ منبعِه لا عندَ قارئِه.
            //      والدلالةُ «آخِرُ علمٍ يغلب»: `--بلا-فحص-أنواع --أنواع-صارمة`
            //      يرفعُ الفحصَ صارمًا، والعكسُ يُطفئُه بتمامِه.
            // (EN) ⚠️ Turns its two dependents off with it: --تنقيح-الأنواع and
            //      --أنواع-صارمة each raise enableTypeCheck implicitly, so leaving
            //      them set after a disable would yield the contradictory state
            //      "strict, debugged type-checking that never runs". Today
            //      interpreter_core ignores them because it tests enableTypeCheck
            //      first — but a contradictory field ignored today becomes
            //      load-bearing the day someone reads it alone. Semantics are
            //      last-flag-wins.
            else if (a == sadCanonical(sad::cli::FlagAction::TypeCheckOff))
            {
                enableTypeCheck = false;
                debugTypeCheck = false;
                strictTypeCheck = false;
            }
            else if (a == sadCanonical(sad::cli::FlagAction::DebugTypes))
            {
                enableTypeCheck = true;
                debugTypeCheck = true;
            }
            else if (a == sadCanonical(sad::cli::FlagAction::StrictTypes))
            {
                enableTypeCheck = true;
                strictTypeCheck = true;
            }
            else if (a == sadCanonical(sad::cli::FlagAction::Security))
            {
                enableSecurity = true;
            }
            else if (a == sadCanonical(sad::cli::FlagAction::DebugSecurity))
            {
                enableSecurity = true;
                debugSecurity = true;
            }
            else if (a == sadCanonical(sad::cli::FlagAction::StrictSecurity))
            {
                enableSecurity = true;
                strictSecurity = true;
            }
            else if (a == sadCanonical(sad::cli::FlagAction::DebugMode))
            {
                enableDebug = true;
            }
            else if (a == sadCanonical(sad::cli::FlagAction::OptStats))
            {
                showOptStats = true;
            }
            else if (a == sadCanonical(sad::cli::FlagAction::DebugServer))
            {
                useDebugServer = true;
            }
            else if (a == sadCanonical(sad::cli::FlagAction::Profile))
            {
                enableProfile = true;
            }
            else if (a == sadCanonical(sad::cli::FlagAction::HotReload))
            {
                enableHotReload = true;
            }
            else if (sadValueOf(a, sad::cli::FlagAction::ProfileFormat, val))
            {
                profileFormat = val;
            }
            else if (sadValueOf(a, sad::cli::FlagAction::ProfileOutput, val))
            {
                profileOutput = val;
            }
            else if (sadValueOf(a, sad::cli::FlagAction::ProfileTop, val))
            {
                try
                {
                    profileTop = std::stoi(val);
                }
                catch (...)
                {
                }
            }
            // ────────────────────────────────────────────────────────────────
            // (AR) أعلام التوثيق/الشرح/اللغة: أسماء عربيّة قانونيّة وحيدة من
            //      المصدر الوحيد (cli_flags_generated.h) — بلا مرادفات ولا
            //      توافق خلفيّ، ومطابِقة حرفيًّا لأسماء المترجم (sad-build).
            // (EN) Docs/explain/lang flags: single canonical Arabic names from the
            //      shared SoT — identical to the compiler's, no aliases.
            // ────────────────────────────────────────────────────────────────
            else if (a == sadCanonical(sad::cli::FlagAction::EmitDocs))
            {
                emitDocs = true;
            }
            else if (sadValueOf(a, sad::cli::FlagAction::DocsOut, val))
            {
                emitDocs = true;
                docsOutput = val;
            }
            else if (sadValueOf(a, sad::cli::FlagAction::DocsProject, val))
            {
                emitDocs = true;
                docsProject = val;
            }
            else if (sadValueOf(a, sad::cli::FlagAction::DocsProjectName, val))
            {
                docsProjectName = val;
            }
            else if (sadValueOf(a, sad::cli::FlagAction::DocsFormat, val))
            {
                docsFormat = val;
                emitDocs = true;
            }
            else if (sadValueOf(a, sad::cli::FlagAction::DocsExclude, val))
            {
                docsExcludes.push_back(val);
            }
            else if (sadValueOf(a, sad::cli::FlagAction::Explain, val))
            {
                explainLevelStr = val;
            }
            else if (a == sadCanonical(sad::cli::FlagAction::Explain))
            {
                explainLevelStr = sad::cli::values::ExplainDefault;
            }
            else if (sadValueOf(a, sad::cli::FlagAction::OutputLanguage, val))
            {
                langStr = val;
            }
            else if (a == sadCanonical(sad::cli::FlagAction::HelpRequested) ||
                     a == sad::cli::short_flags::HelpShort)
            {
                print_help(argv[0]);
                return 0;
            }
            else if (a == sadCanonical(sad::cli::FlagAction::VersionRequested))
            {
                std::cout << "sad version 1.0.0-beta.1\n";
                return 0;
            }
            else if (!a.empty() && a[0] == '-')
            {
                // (AR) فشل عالٍ لا صامت: أيّ علمٍ مجهول يُرفَض (كالمترجم) بدل تجاهله —
                //      يُنهي التوافق الخلفيّ الصامت للأسماء المحذوفة. (EN) Fail loud,
                //      not silent: reject any unknown flag like the compiler does.
                std::cerr << sad::cli::messages::UnknownOptionPrefix << a << "\n";
                return 1;
            }
            else
            {
                filename = a;
            }
        }

        // ===================================================================
        // (AR) المرحلة 3 — تطبيق إعدادات شرح الخطأ ولغة الإخراج على ErrorManager
        // (EN) Phase 3 — Apply explanation level + language settings to EM
        // ===================================================================
        {
            auto &em = Sad::Errors::ErrorManager::getInstance();
            em.initializeDefaults();
            if (!explainLevelStr.empty())
            {
                em.setExplanationLevel(Sad::Errors::parseExplanationLevel(explainLevelStr));
            }
            if (!langStr.empty())
            {
                em.setLanguage(Sad::Errors::parseLanguage(langStr));
            }
        }
        if (filename.empty())
        {
            // (AR) في وضع توثيق المشروع لا نحتاج ملف إدخال صريح
            // (EN) In project docs mode no explicit input file is required
            if (!(emitDocs && !docsProject.empty()))
            {
                print_help(argv[0]);
                return 1;
            }
        }

        // ===================================================================
        // (AR) وضع توثيق المشروع متعدد الملفات (--docs-project=<dir>)
        // (EN) Multi-file project documentation mode (--docs-project=<dir>)
        //
        // (AR) يُنفَّذ قبل قراءة أي ملف إدخال لأنه يمسح المجلد بالكامل.
        // (EN) Runs before reading any input file because it scans the whole dir.
        // ===================================================================
        if (emitDocs && !docsProject.empty())
        {
            using DocsExtractor = Sad::AST::DocsExtractor;
            auto file_paths = docsExcludes.empty()
                                  ? DocsExtractor::collectSadFiles(docsProject)
                                  : DocsExtractor::collectSadFiles(docsProject, docsExcludes);
            if (file_paths.empty())
            {
                std::cerr << "\033[91m❌ لم يُعثر على ملفات .ص أو .sad في: "
                          << docsProject << "\033[0m\n";
                std::cerr << "\033[91m❌ No .ص or .sad files in: "
                          << docsProject << "\033[0m\n";
                return 1;
            }

            std::vector<Sad::AST::StmtList> programs;
            programs.reserve(file_paths.size());
            std::vector<DocsExtractor::FileEntry> entries;
            entries.reserve(file_paths.size());

            size_t failures = 0;
            for (const auto &path : file_paths)
            {
                std::string src;
                try
                {
                    src = read_file(path);
                }
                catch (...)
                {
                    std::cerr << "  warning: cannot read " << path << "\n";
                    ++failures;
                    continue;
                }

                Sad::Lexer::LexerCore lex(src);
                Sad::Parser::ParserCore par(lex);
                auto prog = par.parseProgram();
                if (par.hasErrors())
                {
                    std::cerr << "  warning: parse errors in " << path << "\n";
                    par.printErrors();
                    ++failures;
                    continue;
                }
                programs.push_back(std::move(prog));
                entries.push_back({path, &programs.back()});
            }

            if (entries.empty())
            {
                std::cerr << "\033[91m❌ لا توجد ملفات قابلة للتحليل\033[0m\n";
                return 1;
            }

            std::string projectName = docsProjectName;
            if (projectName.empty())
            {
                std::error_code ec;
                std::filesystem::path p(docsProject);
                auto canon = std::filesystem::weakly_canonical(p, ec);
                if (!ec)
                    projectName = canon.filename().u8string();
                if (projectName.empty())
                    projectName = p.filename().u8string();
                if (projectName.empty())
                    projectName = docsProject;
            }

            std::string md = DocsExtractor::extractProjectMarkdown(
                projectName, entries);

            // (AR) دعم تصدير المشروع إلى PDF: نولّد HTML قابل للطباعة من Markdown
            // (EN) Project PDF export: wrap project Markdown in printable HTML
            bool isProjPdf = (docsFormat == "pdf");
            if (isProjPdf && docsOutput.empty())
            {
                std::cerr << "\033[91m❌ تصدير PDF يتطلب --docs-out=<ملف.pdf>\033[0m\n";
                return 1;
            }

            if (isProjPdf)
            {
                std::string mer = DocsExtractor::buildProjectClassDiagram(entries);
                std::string html = DocsExtractor::markdownToPrintableHtml(md, projectName, mer);
                std::string tmpHtml = docsOutput + ".tmp.html";
                {
                    std::ofstream tf(tmpHtml, std::ios::binary | std::ios::trunc);
                    if (!tf.is_open())
                    {
                        std::cerr << "\033[91m❌ تعذّرت كتابة HTML المؤقت\033[0m\n";
                        return 1;
                    }
                    tf << html;
                }
                std::string err;
                bool ok = Sad::AST::PdfExporter::convert(tmpHtml, docsOutput, err);
                std::error_code ec_rm;
                std::filesystem::remove(tmpHtml, ec_rm);
                if (!ok)
                {
                    std::cerr << "\033[91m❌ فشل تصدير PDF: " << err << "\033[0m\n";
                    return 1;
                }
                std::cerr << "\033[32m✅ تم حفظ PDF المشروع في: " << docsOutput
                          << " (" << entries.size() << " ملف)\033[0m\n";
                return 0;
            }

            if (!docsOutput.empty())
            {
                std::ofstream outFile(docsOutput, std::ios::binary | std::ios::trunc);
                if (!outFile.is_open())
                {
                    std::cerr << "\033[91m❌ تعذّرت كتابة التوثيق إلى: "
                              << docsOutput << "\033[0m\n";
                    return 1;
                }
                outFile << md;
                std::cerr << "\033[32m✅ تم حفظ توثيق المشروع في: " << docsOutput
                          << " (" << entries.size() << " ملف";
                if (failures)
                    std::cerr << "، " << failures << " متخطّى";
                std::cerr << ")\033[0m\n";
            }
            else
            {
                std::cout << md;
            }
            return 0;
        }

        // Read source file
        std::string source = read_file(filename);

        // Clear error manager
        Sad::Errors::ErrorManager::getInstance().clear();

        // (AR) حفظ كود المصدر لعرض الأسطر في رسائل الخطأ
        // (EN) Store source code for displaying lines in error messages
        Sad::Errors::ErrorManager::getInstance().setSourceCode(source, filename);

        // Lexer - create lexer and tokenize
        Sad::Lexer::LexerCore lexer(source);

        // Parser - parse directly from lexer
        Sad::Parser::ParserCore parser(lexer);
        auto program = parser.parseProgram();

        // Check for parser errors
        if (parser.hasErrors())
        {
            parser.printErrors();
            return 1;
        }

        // ===================================================================
        // (AR) وضع استخراج التوثيق: تحليل الملف ثم إخراج Markdown والخروج
        // (EN) Documentation extraction mode: parse then emit Markdown and exit
        //
        // (AR) هذا المسار يستخدم نفس Lexer/Parser/AST للغة، لذلك التوثيق
        //      المُستخرج مرتبط دلالياً بالعنصر الصحيح (وليس مسحاً نصياً).
        // (EN) This path uses the language's own Lexer/Parser/AST so the
        //      extracted docs are semantically attached to the correct decl
        //      (not a textual scan).
        // ===================================================================
        if (emitDocs)
        {
            std::string md;
            bool isPdf = (docsFormat == "pdf");
            if (docsFormat == "json")
                md = Sad::AST::DocsExtractor::extractJson(program, filename);
            else if (docsFormat == "html")
                md = Sad::AST::DocsExtractor::extractHtml(program, filename);
            else if (isPdf)
                md = Sad::AST::DocsExtractor::extractPrintableHtml(program, filename);
            else
                md = Sad::AST::DocsExtractor::extractMarkdown(program, filename);
            if (!docsOutput.empty())
            {
                // (AR) لـ PDF: نكتب HTML مؤقت ثم نحوّله
                std::string htmlPath = isPdf ? (docsOutput + ".tmp.html") : docsOutput;
                std::ofstream outFile(htmlPath);
                if (!outFile.is_open())
                {
                    std::cerr << "\033[91m❌ فشل فتح ملف التوثيق: "
                              << htmlPath << "\033[0m" << std::endl;
                    std::cerr << "\033[91m❌ Failed to open docs file: "
                              << htmlPath << "\033[0m" << std::endl;
                    return 1;
                }
                outFile << md;
                outFile.close();
                if (isPdf)
                {
                    std::string err;
                    bool ok = Sad::AST::PdfExporter::convert(htmlPath, docsOutput, err);
                    std::error_code ec;
                    std::filesystem::remove(htmlPath, ec);
                    if (!ok)
                    {
                        std::cerr << "\033[91m❌ فشل تصدير PDF: " << err
                                  << "\033[0m" << std::endl;
                        return 1;
                    }
                    std::cerr << "\033[32m✅ تم حفظ PDF في: " << docsOutput
                              << "\033[0m" << std::endl;
                    std::cerr << "\033[32m✅ PDF saved to: " << docsOutput
                              << "\033[0m" << std::endl;
                }
                else
                {
                    std::cerr << "\033[32m✅ تم حفظ التوثيق في: " << docsOutput
                              << "\033[0m" << std::endl;
                    std::cerr << "\033[32m✅ Docs saved to: " << docsOutput
                              << "\033[0m" << std::endl;
                }
            }
            else
            {
                if (isPdf)
                {
                    std::cerr << "\033[91m❌ يتطلب --docs-format=pdf مساراً عبر --docs-out\033[0m\n";
                    return 1;
                }
                std::cout << md;
            }
            return 0;
        }

        // ===================================================================
        // وضع خادم التصحيح: DAP debug server mode
        // Debug Server Mode: JSON protocol over stdin/stdout
        // ===================================================================
        if (useDebugServer)
        {
            Sad::Interpreter::InterpreterOptions options;
            options.enableDebugMode = false;
            options.printResults = false;
            options.currentFilePath = filename;
            options.memoryPolicy = g_memoryPolicy;
            options.memoryPolicySet = g_memoryPolicySet;

            Sad::Interpreter::Interpreter interpreter(options);

            // (AR) إنشاء خادم التصحيح وربطه بالمفسر
            // (EN) Create debug server and connect to interpreter
            Sad::Debug::DebugServer debugServer;
            debugServer.setInterpreterRefs(
                &interpreter.getVariableManager(),
                &interpreter.getFunctionManager(),
                &interpreter.getScopeManager());
            debugServer.setInterpreter(&interpreter);
            Sad::Debug::DebugServer::setInstance(&debugServer);

            // (AR) تشغيل خادم التصحيح (سيقرأ الأوامر في خيط منفصل)
            // (EN) Start debug server (will read commands in separate thread)
            debugServer.run(filename);

            // (AR) تنفيذ البرنامج — الخطافات ستوقف التنفيذ عند نقاط التوقف
            // (EN) Execute program — hooks will pause at breakpoints
            auto result = interpreter.execute(program);

            // (AR) إرسال حدث الانتهاء
            // (EN) Send terminated event
            debugServer.sendEvent("terminated");
            debugServer.sendEvent("exited", "{\"exitCode\":" + std::to_string(result.success ? 0 : 1) + "}");

            return result.success ? 0 : 1;
        }

        // ===================================================================
        // الوضع العادي: تنفيذ شجرة AST مباشرة
        // Normal Mode: Tree-walking interpreter
        // ===================================================================

        // Interpreter - create and execute
        Sad::Interpreter::InterpreterOptions options;
        options.enableDebugMode = enableDebug || showOptStats;
        options.printResults = false;
        options.enableOwnership = enableOwnership;
        options.ownershipArabicMessages = true;
        options.ownershipDebugMode = debugOwnership;
        options.enableTypeCheck = enableTypeCheck;
        options.typeCheckDebugMode = debugTypeCheck;
        options.typeCheckStrictMode = strictTypeCheck;
        options.enableSecurity = enableSecurity;
        options.securityStrictMode = strictSecurity;
        options.securityDebugMode = debugSecurity;
        options.currentFilePath = filename; // (AR) مسار الملف الحالي لنظام الاستيراد / (EN) Current file path for import system
        options.enableHotReload = enableHotReload;
        // (AR) Phase A2: ربط سياسة الذاكرة المُستخرجة من سطر الأوامر
        // (EN) Phase A2: wire CLI-parsed memory policy
        options.memoryPolicy = g_memoryPolicy;
        options.memoryPolicySet = g_memoryPolicySet;

        Sad::Interpreter::Interpreter interpreter(options);

        // ═══════════════════════════════════════════════════════════════
        // (AR) إعداد مصحح الأداء إذا طُلب التنميط
        // (EN) Setup profiler if profiling was requested
        // ═══════════════════════════════════════════════════════════════
        std::unique_ptr<Sad::Tools::ProfilerCore> profiler;
        if (enableProfile)
        {
            Sad::Tools::ProfilerOptions profOpts;
            profOpts.topFunctionsCount = profileTop;
            profOpts.colorOutput = true;
            profOpts.trackCallTree = true;
            profOpts.trackMemory = true;
            if (profileFormat == "json")
                profOpts.format = Sad::Tools::ReportFormat::JSON;
            else if (profileFormat == "html")
                profOpts.format = Sad::Tools::ReportFormat::HTML;
            else if (profileFormat == "flame")
                profOpts.format = Sad::Tools::ReportFormat::FLAMEGRAPH;
            else if (profileFormat == "csv")
                profOpts.format = Sad::Tools::ReportFormat::CSV;
            else
                profOpts.format = Sad::Tools::ReportFormat::TEXT;
            if (!profileOutput.empty())
                profOpts.outputFile = profileOutput;

            profiler = std::make_unique<Sad::Tools::ProfilerCore>(profOpts);
            Sad::Tools::setGlobalProfiler(profiler.get());
            profiler->start();
        }

        // Execute program
        auto result = interpreter.execute(program);

        // ═══════════════════════════════════════════════════════════════
        // (AR) وضع إعادة التحميل الساخن: مراقبة الملف وإعادة التنفيذ
        // (EN) Hot Reload Mode: watch file and re-execute on changes
        // ═══════════════════════════════════════════════════════════════
        if (enableHotReload && result.success)
        {
            Sad::HotReload::HotReloadOptions hrOpts;
            hrOpts.preserveState = true;
            hrOpts.showNotifications = true;

            Sad::HotReload::HotReloadEngine hotEngine(hrOpts);
            hotEngine.attach(&interpreter, filename);

            std::cout << "\033[36m🔥 وضع المراقبة نشط — اضغط Ctrl+C للخروج\033[0m\n";
            std::cout << "\033[36m🔥 Hot Reload active — press Ctrl+C to exit\033[0m\n";

            hotEngine.start();

            // (AR) حلقة انتظار — المراقبة تعمل في خيط منفصل
            // (EN) Wait loop — file watching runs in background thread
            while (true)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        // ═══════════════════════════════════════════════════════════════
        // (AR) إيقاف المصحح وتوليد التقرير
        // (EN) Stop profiler and generate report
        // ═══════════════════════════════════════════════════════════════
        if (enableProfile && profiler)
        {
            profiler->stop();
            std::string report = profiler->generateReport(profiler->getOptions().format);
            if (!profileOutput.empty())
            {
                std::ofstream outFile(profileOutput);
                if (outFile.is_open())
                {
                    outFile << report;
                    std::cerr << "\n\033[32m✅ تم حفظ تقرير الأداء في: " << profileOutput << "\033[0m\n";
                }
                else
                {
                    std::cerr << "\033[91m❌ فشل فتح ملف الإخراج: " << profileOutput << "\033[0m\n";
                    std::cout << report;
                }
            }
            else
            {
                std::cout << report;
            }
            Sad::Tools::setGlobalProfiler(nullptr);
        }

        if (!result.success)
        {
            // Print enriched diagnostics from ErrorManager
            if (Sad::Errors::ErrorManager::getInstance().hasErrors())
            {
                Sad::Errors::ErrorManager::getInstance().printAll();
            }
            else
            {
                // (AR) عرض خطأ وقت التشغيل مع سطر الكود المصدري
                // (EN) Display runtime error with source code line
                std::cerr << "\033[91m\033[1m❌ خطأ في التنفيذ / Runtime Error:\033[0m " << result.errorMessage << std::endl;

                // (AR) محاولة استخراج رقم السطر من رسالة الخطأ (at LINE:COL)
                // (EN) Try to extract line number from error message (at LINE:COL)
                std::string msg = result.errorMessage;
                size_t atPos = msg.rfind("at ");
                if (atPos != std::string::npos)
                {
                    std::string posStr = msg.substr(atPos + 3);
                    size_t colonPos = posStr.find(':');
                    if (colonPos != std::string::npos)
                    {
                        try
                        {
                            int lineNum = std::stoi(posStr.substr(0, colonPos));
                            int colNum = std::stoi(posStr.substr(colonPos + 1));
                            const std::string &src = Sad::Errors::ErrorManager::getInstance().getSourceCode();
                            if (!src.empty() && lineNum > 0)
                            {
                                // (AR) استخراج السطر من الكود المصدري
                                // (EN) Extract line from source code
                                std::istringstream srcStream(src);
                                std::string srcLine;
                                int currentLine = 0;
                                while (std::getline(srcStream, srcLine))
                                {
                                    currentLine++;
                                    if (currentLine == lineNum)
                                    {
                                        std::cerr << "\033[36m" << "  " << lineNum << " │ " << srcLine << "\033[0m" << std::endl;
                                        std::cerr << "  " << std::string(std::to_string(lineNum).length(), ' ') << " │ "
                                                  << std::string(colNum > 0 ? colNum - 1 : 0, ' ') << "\033[91m\033[1m^ هنا\033[0m" << std::endl;
                                        break;
                                    }
                                }
                            }
                        }
                        catch (...)
                        {
                        }
                    }
                }

                // (AR) عرض نصيحة ذكية للمبتدئين إن وُجدت
                // (EN) Show beginner-friendly hint if available
                auto &hintEngine = Sad::Errors::ErrorHintEngine::getInstance();
                const auto *hint = hintEngine.findHint("", result.errorMessage);
                if (hint)
                {
                    std::cerr << hintEngine.formatHint(*hint, true);
                }
            }
            return 1;
        }

        // ═══════════════════════════════════════════════════════════════
        // (AR) خطأٌ مسجَّلٌ = تنفيذٌ فاشل، وإن أكمل البرنامجُ مساره.
        //
        //      كان عرضُ التشخيصات مشروطًا بـ(!result.success) وحدَه، والموزِّعُ
        //      يسجّل SEM004 لدالّةٍ غير معرّفة ثمّ **يواصل** بقيمة «لاشيء»
        //      (expression_evaluator_calls_dispatch.cpp) دون أن يُفشل التنفيذ.
        //      فالنتيجة: نداءُ دالّةٍ لا وجودَ لها داخل «رئيسية» يطبع «لاشيء»
        //      ويخرج بالرمز ٠ **بلا تشخيصٍ البتّة** — والتشخيصُ مسجَّلٌ فعلًا،
        //      لكنّه يُطرَح مع المحرّك عند الخروج.
        //
        //      وهو أخطرُ أنواع الفشل: مخرَجٌ خاطئٌ يمرّ صامتًا. والمصرّفُ يرفض
        //      النداءَ نفسَه بـ«استدعاء دالة غير معرّفة»، فالتباعدُ مضاعَف.
        // (EN) A recorded error means a failed run, even if the program finished.
        //      Diagnostics used to be printed only when (!result.success), yet the
        //      dispatcher records SEM004 for an undefined function and then CONTINUES
        //      with «لاشيء» without failing the run. Net effect: calling a function
        //      that does not exist inside «رئيسية» printed «لاشيء» and exited 0 with
        //      NO diagnostic at all — the diagnostic existed but died with the engine.
        //      That is the worst failure mode: wrong output passing silently. The
        //      compiler rejects the very same call, so the divergence is twofold.
        // ═══════════════════════════════════════════════════════════════
        if (Sad::Errors::ErrorManager::getInstance().hasErrors())
        {
            Sad::Errors::ErrorManager::getInstance().printAll();
            return 1;
        }

        return 0;
    }
    catch (const Sad::Interpreter::ExitException &exitEx)
    {
        // (AR) خروج نظيف من البرنامج
        // (EN) Clean program exit
        return exitEx.getExitCode();
    }
    catch (const std::exception &e)
    {
        std::cerr << "\033[91m\033[1m❌ خطأ / Error:\033[0m " << e.what() << std::endl;
        return 1;
    }
}
