// بسم الله الرحمن الرحيم
/**
 * @file repl_commands.cpp
 * @brief تنفيذ معالج الأوامر الخاصة / Commands Handler Implementation
 * 
 * @author Sad Language Team
 * @date January 2026
 */

#include "repl_commands.h"
#include "repl_engine.h"
#include "repl_colors.h" // (AR) ثوابت ألوان ANSI مشتركة (م-2) / (EN) shared ANSI color constants (م-2)
#include "repl_sot_generated.h" // (AR) كتالوج «مصدر حقيقة الأدوات» — أخطاء/رسائل/أوامر REPL / (EN) Tool-SoT catalog
#include "shell_executor.h"     // (AR) مُنفِّذ الأوامر الخارجيّة / (EN) external-command executor
#include "shell_lexer.h"        // (AR) مُحلِّل سطر الصدَفة (اقتباس/أنابيب/توسيع بيئة) / (EN) shell-line lexer
#include <iostream>
#include <sstream>
#include <string_view>
#include <algorithm>
#include <cstdio>  // (AR) snprintf لسطر «التاريخ» العربيّ / (EN) snprintf for the Arabic date line
#include <cstdlib> // (AR) getenv/setenv/_putenv_s لمتغيّرات البيئة / (EN) env-var access
#include <ctime>   // (AR) time/localtime لمكوّنات «التاريخ» الرقميّة / (EN) numeric date components

#ifdef _WIN32
#include <windows.h>
#include <process.h> // (AR) _getpid لـ‹$$› / (EN) _getpid for ‹$$›
#else
#include <unistd.h> // (AR) getpid لـ‹$$› + chdir/getcwd لـ‹:اذهب› / (EN) getpid for ‹$$›, chdir/getcwd for ‹:cd›
extern char** environ; // (AR) قائمة بيئة العمليّة (POSIX) لسرد ‹:بيئة› / (EN) process env list
#endif
// (AR) يُضمَّن عمدًا بعد <unistd.h> العالميّ أعلاه (نفس فخّ repl_engine.cpp الموثَّق):
//      utf8_utils.h يُضمّن <unistd.h> داخل نطاق أسماءٍ على غير ويندوز، فتقديمه يبتلع
//      getpid/chdir/getcwd في sad::utf8 ويكسر البناء على لينكس.
// (EN) Included AFTER the global <unistd.h> on purpose (same documented repl_engine.cpp
//      trap): utf8_utils.h pulls <unistd.h> inside a namespace on non-Windows, so placing
//      it first swallows getpid/chdir/getcwd into sad::utf8 and breaks the Linux build.
#include "utf8_utils.h" // (AR) to_wstring/from_wstring لمسارات ويندوز العربيّة (‹:اذهب›) / (EN) wide-path helpers for Arabic Windows paths (‹:cd›)

namespace Sad {
namespace REPL {

// (AR) مِقبض العمليّة الحاليّة (‹$$›) — دالّة نقيّة عبر المنصّة. / (EN) current process id (‹$$›).
static long currentPid()
{
#ifdef _WIN32
    return static_cast<long>(_getpid());
#else
    return static_cast<long>(getpid());
#endif
}

// (AR) مُحلِّل المتغيّرات المربوط بمثيل REPL: يعالج الوسائط الخاصّة أوّلًا (‹$?› رمز خروج آخر
//      ‹:شغّل›، ‹$$› مِقبض الصدَفة)، وإلّا فقيمة متغيّر البيئة (فارغة إن لم يُعرَّف). ربطُه بالمثيل
//      (لا حالة عالميّة) يتيح ‹$?› قراءةَ lastRunExitCode_ الخاصّ بهذه الصدَفة. ‹?›/‹$› ليسا
//      اسمَي بيئةٍ صالحين فلا يتصادمان مع getenv. / (EN) instance-bound variable resolver:
//      special params first (‹$?› = last ‹:run› exit code, ‹$$› = shell pid), else the env
//      var's value (empty if unset). Binding to the instance (no global state) lets ‹$?› read
//      this shell's private lastRunExitCode_. ‹?›/‹$› are not valid env names, so no getenv clash.
static std::string resolveVar(REPLEngine* repl, const std::string& name)
{
    if (name == "?")
    {
        return std::to_string(repl->getCommands()->lastExitCode());
    }
    if (name == "$")
    {
        return std::to_string(currentPid());
    }
    const char* v = std::getenv(name.c_str());
    return v ? std::string(v) : std::string();
}

// (AR) اسم متغيّر بيئة صالح: يبدأ بحرف ASCII أو ‹_› أو بايت ≥0x80 (حرف عربيّ)، ثمّ حروف/
//      أرقام/‹_›/≥0x80، بلا ‹=›. مطابقٌ لقاعدة الاسم في مُحلِّل التوسيع. / (EN) a valid env-var
//      name: starts with an ASCII letter, ‹_›, or a byte ≥0x80 (Arabic), then alnum/‹_›/≥0x80;
//      no ‹=›. Matches the name rule in the expansion lexer.
static bool isValidEnvName(const std::string& name)
{
    if (name.empty())
    {
        return false;
    }
    auto isStart = [](unsigned char c)
    { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_' || c >= 0x80; };
    auto isRest = [](unsigned char c)
    {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
               c == '_' || c >= 0x80;
    };
    if (!isStart(static_cast<unsigned char>(name[0])))
    {
        return false;
    }
    for (std::size_t i = 1; i < name.size(); ++i)
    {
        if (!isRest(static_cast<unsigned char>(name[i])))
        {
            return false;
        }
    }
    return true;
}

// (AR) يضبط متغيّر بيئةٍ للعمليّة (يرثه ما يُطلَق لاحقًا) — عبر الواجهة المنصّيّة.
//      ⚠️ تباعد منصّة موثّق لقيمةٍ فارغة (‹:بيئة X=›): POSIX يضبط X مُعرَّفًا فارغًا، بينما
//      Windows‏ (_putenv_s بقيمة فارغة) يحذف X. النظام الهدف (sad-os) لينكس، والحالة هامشيّة.
// (EN) sets a process env var (inherited by later launches) via the platform API.
//      ⚠️ documented platform divergence for an empty value (‹:env X=›): POSIX sets X to a
//      defined-empty value, while Windows (_putenv_s with "") deletes X. The target OS
//      (sad-os) is Linux and the case is a corner case.
static bool setEnvVar(const std::string& name, const std::string& value)
{
#ifdef _WIN32
    return _putenv_s(name.c_str(), value.c_str()) == 0;
#else
    return setenv(name.c_str(), value.c_str(), 1) == 0;
#endif
}

// (AR) «التاريخ» بلا وسائط (تعريب ٨ — الشريحة الأولى): سطر عربيّ التسميات مبنيّ من
//      مكوّنات struct tm الرقميّة عبر localtime — **لا strftime %A/%B** (محليّة C
//      تجعلهما إنجليزيّين دومًا، والقياس الحيّ في مذكّرة التصميم أثبت أنّ LANG بلا أثر).
//      اسم اليوم من جدول SoT المولَّد (calendar.yaml)، والصيغة كلّها من SoT — صفر
//      حرفيّات منطقيّة هنا. الأشهر رقميّة والأرقام غربيّة حتى قرارات المالك المعلَّقة.
//      يعيد false عند تعذّر القراءة (فيفوّض المستدعي للمسار الخارجيّ بدل إخفاء العطل).
// (EN) argument-less «التاريخ»: Arabic-labeled line from numeric localtime components
//      (never strftime %A/%B — C locale keeps them English). Day name and format come
//      from the generated calendar SoT; zero logical literals. Returns false if the
//      clock cannot be read (caller falls back to the external path — no silent hide).
static bool printArabicDateLine()
{
    std::time_t now = std::time(nullptr);
    std::tm tmv{};
#ifdef _WIN32
    if (localtime_s(&tmv, &now) != 0)
    {
        return false;
    }
#else
    if (localtime_r(&now, &tmv) == nullptr)
    {
        return false;
    }
#endif
    const char* day = SoT::weekdayName(tmv.tm_wday);
    if (day == nullptr)
    {
        return false;
    }
    // (AR) صيغة SoT: %s اسم اليوم ثمّ سنة/شهر/يوم/ساعة/دقيقة/ثانية — عقدها يحرسه
    //      المولِّد ساكنًا (gen_tool_sot: %s واحد + ستّة أعداد) فلا انفجار تنسيق.
    // (EN) SoT format contract (one %s + six ints) is statically enforced by the generator.
    char line[192];
    int n = std::snprintf(line, sizeof(line), SoT::kDateLineFormat, day,
                          tmv.tm_year + 1900, tmv.tm_mon + 1, tmv.tm_mday,
                          tmv.tm_hour, tmv.tm_min, tmv.tm_sec);
    if (n <= 0 || static_cast<std::size_t>(n) >= sizeof(line))
    {
        return false;
    }
    std::cout << line << std::endl;
    return true;
}

// (AR) الثوابت مُعرَّفة مرّة واحدة في repl_colors.h (م-2: إزالة التكرار).
// (EN) constants defined once in repl_colors.h (م-2: de-duplicated).
using namespace Colors;

REPLCommands::REPLCommands(REPLEngine* repl)
    : repl_(repl)
{
    registerAllCommands();
}

bool REPLCommands::process(const std::string& command)
{
    // Remove leading colon and whitespace / إزالة النقطتين والمسافات
    std::string cmd = command;
    if (cmd[0] == ':') {
        cmd = cmd.substr(1);
    }
    
    // Split into command and arguments / تقسيم لأمر ووسائط
    std::istringstream iss(cmd);
    std::string cmdName;
    iss >> cmdName;

    std::vector<std::string> args;
    std::string arg;
    while (iss >> arg) {
        args.push_back(arg);
    }

    // (AR) التقط الوسائط الخامّ (كلّ ما بعد اسم الأمر، بلا تقسيم) — يحترم الاقتباس
    //      والأنابيب في ‹:شغّل›. مستقلّ عن iss كي لا يفسد تقسيم args أعلاه.
    // (EN) capture the raw args (everything after the command name, unsplit) so ‹:run›
    //      can honor quotes/pipes. Computed from `cmd` independently of the args split.
    pendingRawArgs_.clear();
    if (size_t nameStart = cmd.find_first_not_of(" \t"); nameStart != std::string::npos) {
        size_t nameEnd = cmd.find_first_of(" \t", nameStart);
        if (nameEnd != std::string::npos) {
            if (size_t argStart = cmd.find_first_not_of(" \t", nameEnd);
                argStart != std::string::npos) {
                pendingRawArgs_ = cmd.substr(argStart);
            }
        }
    }

    // Find command / البحث عن الأمر
    auto it = commands_.find(cmdName);
    if (it != commands_.end()) {
        return it->second.handler(repl_, args);
    }

    // (AR) اسمٌ عربيّ صريح لأمر نواة (معجم الموزِّع)؟ نفّذه مباشرةً كأمرٍ خارجيّ — فيكتب المستخدم
    //      ‹:اسرد /tmp› بدل ‹:شغّل ls /tmp›. نُعيد بناء الوسائط الخامّ لتبدأ بالاسم العربيّ نفسه
    //      (المرحلة الأولى)، فيترجمه مترجم المراحل في cmdRun إلى exec. / (EN) an explicit Arabic
    //      kernel-command name (dispatcher lexicon)? run it directly as an external command — the
    //      user types ‹:اسرد /tmp› instead of ‹:شغّل ls /tmp›. Rebuild the raw args to begin with
    //      the Arabic name (as the first stage's program) so cmdRun's stage translator maps it.
    if (SoT::appletExec(cmdName) != nullptr) {
        // (AR) تعريب ٨ (الشريحة الأولى): «التاريخ» **بلا وسائط** أمرٌ داخليّ يعرض سطرًا
        //      عربيّ التسميات من SoT التقويم. موضع الالتقاط هنا (فرع الموزِّع قبل إعادة
        //      بناء الوسائط الخامّ) هو الأنظف: (١) لا يمسّ «تاريخ» بلا أل التعريف —
        //      أمر history قائم يلتقطه commands_ أعلاه قبل هذا الفرع أصلًا؛ (٢) أيّ
        //      وسائط (‹:التاريخ -s …›/‹+FMT›) تسقط للمسار الخارجيّ كما هي فلا ينكسر
        //      ضبط الساعة؛ (٣) exec ‏‹date› الإنجليزيّ الخامّ عبر ‹:شغّل› يبقى متاحًا
        //      للتوافق. حارس التصادم الساكن في المولِّد يثبت وصول كلٍّ لوجهته.
        //      حافّة: فشل قراءة الساعة يفوّض للمسار الخارجيّ (لا إخفاء عطل).
        // (EN) argument-less «التاريخ» is a builtin rendering the Arabic date line from
        //      the calendar SoT. Interception here (dispatcher branch, before rebuilding
        //      raw args) never touches the history command «تاريخ» (matched by commands_
        //      above), keeps every argumented form (-s/+FMT) on the external path, and
        //      leaves raw English ‹date› via ‹:run› untouched. On a clock-read failure
        //      it falls through to the external path (no silent masking).
        if (cmdName == SoT::kDateAppletArabic && pendingRawArgs_.empty() &&
            printArabicDateLine()) {
            return true;
        }
        pendingRawArgs_ = pendingRawArgs_.empty() ? cmdName : (cmdName + " " + pendingRawArgs_);
        return cmdRun(repl_, args);
    }

    // (AR) رسالة «أمر غير معروف» وتلميحها من كتالوج SoT (لا حرفيّات مبعثرة).
    // (EN) Unknown-command message + hint from the SoT catalog (no scattered literals).
    std::cout << SoT::errorMessage(SoT::Error::UNKNOWN_COMMAND, cmdName) << std::endl;
    if (const auto* e = SoT::findError(SoT::Error::UNKNOWN_COMMAND); e && e->hintAr) {
        std::cout << e->hintAr << SoT::kBilingualSeparator << e->hintEn << std::endl;
    }
    return true;
}

bool REPLCommands::isCommand(const std::string& text) const
{
    if (text.empty() || text[0] != ':') {
        return false;
    }
    
    std::string cmd = text.substr(1);
    size_t spacePos = cmd.find_first_of(" \t");
    if (spacePos != std::string::npos) {
        cmd = cmd.substr(0, spacePos);
    }

    // (AR) أمرٌ مدمج أو اسمٌ عربيّ صريح لأمر نواة (معجم الموزِّع) ⇒ سطرُ صدَفةٍ لا تعبير ص.
    // (EN) a built-in command or an explicit Arabic kernel-command name (dispatcher lexicon) ⇒
    //      a shell line, not a ص expression.
    return commands_.find(cmd) != commands_.end() || SoT::appletExec(cmd) != nullptr;
}

std::vector<CommandInfo> REPLCommands::getAllCommands() const
{
    std::vector<CommandInfo> result;
    for (const auto& pair : commands_) {
        result.push_back(pair.second);
    }
    return result;
}

const CommandInfo* REPLCommands::getCommandInfo(const std::string& name) const
{
    auto it = commands_.find(name);
    if (it != commands_.end()) {
        return &it->second;
    }
    return nullptr;
}

void REPLCommands::registerCommand(const std::string& name,
                                   const std::string& arabicName,
                                   const std::string& description,
                                   const std::string& usage,
                                   CommandFunc handler)
{
    CommandInfo info;
    info.name = name;
    info.arabicName = arabicName;
    info.description = description;
    info.usage = usage;
    info.handler = handler;
    
    commands_[name] = info;
    if (!arabicName.empty()) {
        commands_[arabicName] = info;
    }
}

// (AR) يربط مفتاح المعالِج الرمزيّ (من SoT) بدالّة C++ الساكنة المقابلة. البيانات
//      الوصفيّة (الأسماء/الأوصاف/الاستعمال) من الكتالوج؛ المنطق يبقى هنا.
// (EN) Maps a symbolic handler key (from SoT) to its static C++ function. Metadata
//      (names/descriptions/usage) comes from the catalog; the logic stays here.
CommandFunc REPLCommands::handlerFor(SoT::CommandHandler h)
{
    switch (h) {
        case SoT::CommandHandler::HELP:    return cmdHelp;
        case SoT::CommandHandler::EXIT:    return cmdExit;
        case SoT::CommandHandler::CLEAR:   return cmdClear;
        case SoT::CommandHandler::TYPE:    return cmdType;
        case SoT::CommandHandler::LOAD:    return cmdLoad;
        case SoT::CommandHandler::HISTORY: return cmdHistory;
        case SoT::CommandHandler::RESET:   return cmdReset;
        case SoT::CommandHandler::VARS:    return cmdVars;
        case SoT::CommandHandler::FUNCS:   return cmdFuncs;
        case SoT::CommandHandler::RUN:     return cmdRun;
        case SoT::CommandHandler::ENV:     return cmdEnv;
        case SoT::CommandHandler::CD:      return cmdCd;
    }
    return nullptr;
}

// (AR) سطر «الاستخدام» لأمرٍ ما: تسمية USAGE_LABEL ثنائيّة اللغة + صيغة الاستعمال
//      المسجَّلة (من الكتالوج) — يُطبع عند نقص وسائط أمرٍ يتطلّبها.
// (EN) A command's usage line: the bilingual USAGE_LABEL + its registered usage
//      syntax (from the catalog) — printed when a required argument is missing.
std::string REPLCommands::usageLine(REPLEngine* repl, const std::string& name)
{
    std::string usage;
    if (const CommandInfo* info = repl->getCommands()->getCommandInfo(name)) {
        usage = info->usage;
    }
    return SoT::messageBoth(SoT::Message::USAGE_LABEL) +
           std::string(SoT::kDetailSeparator) + usage;
}

void REPLCommands::registerAllCommands()
{
    // (AR) تُبنى الأوامر كلّها من كتالوج SoT (language-truth/tools/repl/commands.yaml)
    //      بدل استدعاءات registerCommand المكتوبة يدوياً بحرفيّات مبعثرة. الوصف
    //      المعروض يُركَّب ثنائيّ اللغة «ar / en» من فاصل الكتالوج نفسه.
    // (EN) All commands are built from the SoT catalog instead of hand-written
    //      registerCommand calls. The displayed description is composed bilingually.
    for (std::size_t i = 0; i < SoT::kCommandsCount; ++i) {
        const SoT::CommandEntry& c = SoT::kCommands[i];
        CommandFunc fn = handlerFor(c.handler);
        if (!fn) {
            continue;
        }
        std::string description =
            std::string(c.descAr) + SoT::kBilingualSeparator + c.descEn;
        registerCommand(c.name, c.arabicName, description, c.usage, fn);
    }
}

// ============================================================================
// Command Handlers / معالجات الأوامر
// ============================================================================

// (AR) تصريح مسبق: مُعرَّفة أدناه قرب أوامر الصدَفة، ويحتاجها مسار «:مساعدة <اسم>».
// (EN) forward declaration: defined below near the shell commands; the help-by-name path needs it.
static void printReplError(SoT::Error code, std::string_view detail);

// (AR) قناة التفويض الإنجليزيّة المعلَنة (تعريب ١٠): مساعدة busybox بنيويًّا إنجليزيّة
//      (LOCALE_SUPPORT=n)، فلا نلفّها ولا نترجمها — سطرُ تفويضٍ صريح يدلّ عليها.
//      ثوابت مسمّاة (لا حرفيّات منطقيّة مبعثرة في جسد المعالج).
// (EN) the declared English delegation channel: busybox help is structurally English,
//      so we point to it explicitly instead of wrapping/translating it. Named constants.
static constexpr const char* kEnglishHelpFlag = "--help";
static constexpr const char* kShellCommandPrefix = ":";

bool REPLCommands::cmdHelp(REPLEngine* repl, const std::vector<std::string>& args)
{
    // (AR) «:مساعدة <اسم>» (تعريب ١٠ — الشريحة ١): وسيطٌ واحد يُفسَّر اسمًا —
    //      (١) أمر REPL (إنجليزيّ/عربيّ) ⇒ وصفه الثنائيّ وصيغته من كتالوج commands.yaml؛
    //      (٢) اسم آبلت عربيّ صريح ⇒ سطر الوصف العربيّ من معجم SoT (applets.yaml) +
    //          سطر التفويض «التفصيل الإنجليزيّ: :اسم --help»؛ آبلت بلا وصفٍ بعدُ ⇒
    //          رسالة صريحة (HELP_NO_DESC) + سطر التفويض — لا صمت؛
    //      (٣) اسم مجهول ⇒ خطأ الكتالوج UNKNOWN_COMMAND.
    //      بلا وسائط: الكتالوج القائم أدناه كما هو حرفيًّا (لا تغيير في السلوك).
    // (EN) «:help <name>»: a REPL command shows its bilingual catalog entry; an Arabic
    //      applet shows its Arabic SoT description (or an explicit "no description yet")
    //      plus the English --help delegation line; an unknown name is a catalog error.
    //      Without arguments the existing catalog listing below is untouched.
    if (!args.empty()) {
        // (AR) يُقبل الاسم بنقطتيه أو بدونهما («:مساعدة :اسرد» = «:مساعدة اسرد»).
        //      حدّ معلَن (هامش أميليا 2026-07-17): ما بعد الوسيط الأوّل يُتجاهَل —
        //      «:مساعدة اسرد اعرض» يعرض مساعدة «اسرد» فقط (وسيط واحد لكلّ نداء).
        // (EN) accept the name with or without its leading colon. Declared limit:
        //      anything past args[0] is ignored — one name per invocation.
        std::string name = args[0];
        if (!name.empty() && name[0] == kShellCommandPrefix[0]) {
            name = name.substr(1);
        }
        if (const CommandInfo* info = repl->getCommands()->getCommandInfo(name)) {
            std::cout << kShellCommandPrefix << info->name;
            if (!info->arabicName.empty() && info->arabicName != info->name) {
                std::cout << SoT::kBilingualSeparator << kShellCommandPrefix
                          << info->arabicName;
            }
            std::cout << "\n  " << info->description << "\n  "
                      << usageLine(repl, name) << std::endl;
            return true;
        }
        if (const SoT::AppletEntry* applet = SoT::findApplet(name)) {
            if (applet->descAr != nullptr) {
                std::cout << applet->descAr << std::endl;
            } else {
                std::cout << SoT::messageAr(SoT::Message::HELP_NO_DESC) << std::endl;
            }
            std::cout << SoT::messageAr(SoT::Message::HELP_DELEGATE_LABEL)
                      << SoT::kDetailSeparator << kShellCommandPrefix
                      << applet->arabic << " " << kEnglishHelpFlag << std::endl;
            return true;
        }
        printReplError(SoT::Error::UNKNOWN_COMMAND, name);
        return true;
    }

    auto& config = repl->getConfig();
    
    const std::string commandsHeader =
        SoT::messageBoth(SoT::Message::COMMANDS_HEADER) + ":";
    if (config.enableColor) {
        std::cout << BOLD << CYAN << commandsHeader << RESET << "\n\n";
    } else {
        std::cout << commandsHeader << "\n\n";
    }
    
    auto commands = repl->getCommands()->getAllCommands();
    
    // Remove duplicates (Arabic aliases) / إزالة المكررات
    std::sort(commands.begin(), commands.end(), 
        [](const CommandInfo& a, const CommandInfo& b) {
            return a.name < b.name;
        });
    
    for (const auto& cmd : commands) {
        if (cmd.name == cmd.arabicName) continue;
        
        if (config.enableColor) {
            std::cout << "  " << YELLOW << ":" << cmd.name << RESET;
            if (!cmd.arabicName.empty()) {
                std::cout << " / " << YELLOW << ":" << cmd.arabicName << RESET;
            }
            std::cout << "\n    " << cmd.description << "\n";
            std::cout << "    " << GREEN << cmd.usage << RESET << "\n\n";
        } else {
            std::cout << "  :" << cmd.name;
            if (!cmd.arabicName.empty()) {
                std::cout << " / :" << cmd.arabicName;
            }
            std::cout << "\n    " << cmd.description << "\n";
            std::cout << "    " << cmd.usage << "\n\n";
        }
    }
    
    return true;
}

bool REPLCommands::cmdExit(REPLEngine* repl, const std::vector<std::string>& args)
{
    return false; // Signal to exit / إشارة للخروج
}

bool REPLCommands::cmdClear(REPLEngine* repl, const std::vector<std::string>& args)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    return true;
}

// (AR) يطبع رسالة خطأ من كتالوج SoT (الموجز ثمّ التلميح إن وُجد). detail اختياريّ.
// (EN) prints an SoT-catalog error (brief then hint if present). detail is optional.
static void printReplError(SoT::Error code, std::string_view detail = {})
{
    std::cout << SoT::errorMessage(code, detail) << std::endl;
    if (const auto* e = SoT::findError(code); e && e->hintAr)
    {
        std::cout << e->hintAr << SoT::kBilingualSeparator << e->hintEn << std::endl;
    }
}

// (AR) ‹:شغّل›/‹:run› — تشغيل برنامج خارجيّ متزامنًا (إرغونوميا الصدَفة). يُحلَّل السطر
//      الخامّ محترمًا الاقتباس (‹"…"›/‹'…'›) والهروب (‹\›) والأنابيب (‹|›) وإعادة التوجيه
//      (‹<›/‹>›/‹>>› للإدخال/الإخراج، و‹2>›/‹2>>›/‹&>›/‹2>&1› للخطأ). أمرٌ مفرد بلا توجيه ⇒
//      runExternal؛ غير ذلك ⇒ runPipeline. أخطاء التحليل والإطلاق تُبلَّغ عبر كتالوج SoT.
//      يرث stdin/stdout/stderr ما لم يُعِد التوجيه.
//      وسلاسلَ شرطيّة (‹&&› شغّل إن نجح السابق، ‹||› إن فشل). كلّ مقطعٍ أمرٌ مفرد ⇒ runExternal،
//      وإلّا runPipeline. أخطاء التحليل والإطلاق عبر كتالوج SoT.
// (EN) ‹:run› — run an external command synchronously (shell ergonomics). The raw line is
//      parsed honoring quotes, ‹\› escaping, ‹|› pipes, redirection (‹<›/‹>›/‹>>› for
//      stdin/stdout, ‹2>›/‹2>>›/‹&>›/‹2>&1› for stderr), and conditional chains (‹&&› run if the
//      previous succeeded, ‹||› if it failed). Each segment: a plain single command ⇒
//      runExternal; otherwise ⇒ runPipeline. Errors go via the SoT catalog.
bool REPLCommands::cmdRun(REPLEngine* repl, const std::vector<std::string>& args)
{
    (void)args; // (AR) نستعمل الوسائط الخامّ لا المقسّمة بالمسافات / raw args, not the whitespace split
    const std::string& raw = repl->getCommands()->pendingRawArgs_;
    if (raw.empty())
    {
        std::cout << usageLine(repl, "run") << std::endl;
        return true;
    }

    // (AR) مُحلِّلٌ مربوطٌ بهذا المثيل كي يوسّع ‹$?›/‹$$› إضافةً لمتغيّرات البيئة. ملاحظة: التوسيع
    //      يقع مرّةً عند التحليل، فـ‹$?› داخل السلسلة يعكس الأمرَ السابق لا مقطعًا سابقًا في نفس
    //      السطر (البوّابة ‹&&›/‹||› تستعمل رموز الخروج الحيّة، فالمنطق الشرطيّ سليم). / (EN)
    //      resolver bound to this instance so ‹$?›/‹$$› expand alongside env vars. Note: expansion
    //      happens once at parse time, so ‹$?› inside a chain reflects the previous command, not
    //      an earlier segment on the same line (the ‹&&›/‹||› gating uses live exit codes, so the
    //      conditional logic is correct).
    auto resolver = [repl](const std::string& n) { return resolveVar(repl, n); };
    ShellCommandLine parsed = parseCommandLine(raw, resolver);
    switch (parsed.status)
    {
        case ShellParseStatus::UnterminatedQuote:
            printReplError(SoT::Error::SHELL_UNTERMINATED_QUOTE);
            return true;
        case ShellParseStatus::EmptyStage:
            printReplError(SoT::Error::SHELL_EMPTY_STAGE);
            return true;
        case ShellParseStatus::RedirNoTarget:
            printReplError(SoT::Error::REDIRECT_NO_TARGET);
            return true;
        case ShellParseStatus::Ok:
            break;
    }
    if (parsed.segments.empty()) // (AR) دفاعيّ: خامّ بلا أمرٍ فعليّ / defensive: raw with no real command
    {
        std::cout << usageLine(repl, "run") << std::endl;
        return true;
    }

    // (AR) الموزِّع العربيّ: ترجم برنامجَ كلّ مرحلة (argv[0]) من اسمٍ عربيٍّ صريح إلى برنامج
    //      التنفيذ الحقيقيّ قبل الإطلاق — فتُكتب الأوامر بالعربيّة الخالصة، وتعمل الأنابيب
    //      والسلاسل العربيّة (‹اسرد | التقط ص›). اسمٌ غير مُعرَّف في المعجم يبقى كما هو (توافق
    //      خلفيّ: ‹ls› الإنجليزيّ يمرّ مباشرةً إلى execvp). المعجم من مصدر حقيقة الأداة (SoT).
    // (EN) Arabic dispatcher: translate each stage's program (argv[0]) from an explicit Arabic
    //      name to its real exec before launching — so commands are written in pure Arabic and
    //      Arabic pipes/chains work (‹اسرد | التقط ص›). An unregistered name is kept as-is
    //      (backward compat: English ‹ls› flows straight to execvp). Lexicon from the tool SoT.
    for (ShellSegment& seg : parsed.segments)
    {
        for (ShellStage& st : seg.stages)
        {
            if (!st.argv.empty())
            {
                if (const char* real = SoT::appletExec(st.argv[0]))
                {
                    st.argv[0] = real;
                }
            }
        }
    }

    // (AR) هل يحوي مقطعٌ إعادة توجيه (إدخال/إخراج/خطأ)؟ / (EN) does a segment redirect?
    auto segHasRedirect = [](const ShellSegment& seg)
    {
        for (const ShellStage& st : seg.stages)
        {
            if (!st.inFile.empty() || !st.outFile.empty() || !st.errFile.empty() || st.errToOut)
            {
                return true;
            }
        }
        return false;
    };

#ifdef _WIN32
    // (AR) الأنابيب المتعدّدة وإعادة التوجيه غير مدعومة على Windows (POSIX فقط)؛ لكنّ سلاسل
    //      ‹&&›/‹||› من أوامر مفردة تعمل (تسلسل runExternal). / (EN) multi-stage pipes and
    //      redirection are unsupported on Windows (POSIX only); but ‹&&›/‹||› chains of plain
    //      single commands work (sequential runExternal).
    for (const ShellSegment& seg : parsed.segments)
    {
        if (seg.stages.size() > 1)
        {
            printReplError(SoT::Error::PIPE_UNSUPPORTED);
            return true;
        }
        if (segHasRedirect(seg))
        {
            printReplError(SoT::Error::REDIRECT_UNSUPPORTED);
            return true;
        }
    }
#endif

    // (AR) نفّذ المقاطع يسارًا-يمينًا مع البوّابة الشرطيّة: ‹&&› يتخطّى إن فشل السابق، ‹||› يتخطّى
    //      إن نجح. رمز الخروج المتتبَّع من آخر مقطعٍ نُفِّذ فعلًا (فشلُ الإطلاق = 127، ولا يوقف
    //      السلسلة — كصدَفة). ‹$?› النهائيّ = رمز آخر منفَّذ. / (EN) run segments left-to-right with
    //      conditional gating: ‹&&› skips if the previous failed, ‹||› skips if it succeeded. The
    //      tracked exit code is the last actually-executed segment's (a launch failure = 127 and
    //      does not abort the chain — shell-like). Final ‹$?› = the last executed segment's code.
    int lastExit = 0; // (AR) المقطع الأوّل (First) يعمل دائمًا فلا يُقرأ هذا / the First segment
                      //      always runs, so this seed is never gated on
    for (const ShellSegment& seg : parsed.segments)
    {
        if (seg.op == ChainOp::And && lastExit != 0)
        {
            continue; // (AR) ‹&&›: السابق فشل ⇒ تخطَّ / previous failed ⇒ skip
        }
        if (seg.op == ChainOp::Or && lastExit == 0)
        {
            continue; // (AR) ‹||›: السابق نجح ⇒ تخطَّ / previous succeeded ⇒ skip
        }

        std::cout.flush();
        // (AR) أمرٌ مفرد بلا توجيه ⇒ المسار المُختبَر runExternal؛ غير ذلك ⇒ runPipeline.
        // (EN) plain single command ⇒ the tested runExternal path; otherwise ⇒ runPipeline.
        const bool plainSingle = (seg.stages.size() == 1 && !segHasRedirect(seg));
        LaunchFailure fail;
        ShellResult r = plainSingle ? runExternal(seg.stages[0].argv)
                                    : runPipeline(seg.stages, fail);
        if (!r.spawned)
        {
            if (fail.isRedirect)
            {
                // (AR) تعذّر فتح ملفّ إعادة التوجيه / redirection file could not be opened
                printReplError(SoT::Error::REDIRECT_FAILED, fail.file);
            }
            else
            {
                // (AR) اسم البرنامج المُخفِق: من المرحلة الفاشلة، أو أوّل برنامج للمقطع.
                // (EN) failing program: the failed stage, or the segment's first program.
                const std::string& prog =
                    !fail.program.empty() ? fail.program : seg.stages[0].argv[0];
                printReplError(SoT::Error::RUN_FAILED, prog);
            }
            lastExit = 127; // (AR) اصطلاح «الأمر غير موجود» / the "command not found" idiom
        }
        else
        {
            lastExit = r.exitCode;
        }
        // (AR) Ctrl-C أثناء مقطعٍ يُجهض السلسلة كلّها (كصدَفة: لا يُشغَّل ‹|| بديل› بعد مقاطعة).
        //      رمز الخروج (130) محفوظ في lastExit ⇒ ‹$?› صحيح. / (EN) Ctrl-C during a segment
        //      aborts the whole chain (shell-like: no ‹|| fallback› runs after an interrupt).
        //      The exit code (130) is kept in lastExit ⇒ ‹$?› is correct.
        if (r.interrupted)
        {
            break;
        }
    }
    // (AR) خزّن رمز الخروج النهائيّ للسلسلة لـ‹$?›. / (EN) record the chain's final exit code for ‹$?›.
    repl->getCommands()->lastRunExitCode_ = lastExit;
    return true;
}

// (AR) ‹:بيئة›/‹:env› — بلا وسائط: يسرد كلّ متغيّرات البيئة. ‹NAME=VALUE›: يضبط متغيّرًا
//      للعمليّة (يرثه ما يُطلَق لاحقًا عبر ‹:شغّل›)؛ القيمة تُوسَّع فيها ‹$VAR› فيُمكن البناء
//      التراكميّ (‹:بيئة مسار=$PATH:/x›). ‹NAME› وحده: يعرض قيمته. اسمٌ غير صالح ⇒ خطأ SoT.
// (EN) ‹:env› — no args: list all env vars. ‹NAME=VALUE›: set a process var (inherited by
//      later ‹:run› launches); the value has ‹$VAR› expanded so it can build on itself
//      (‹:env PATH2=$PATH:/x›). ‹NAME› alone: show its value. An invalid name ⇒ SoT error.
bool REPLCommands::cmdEnv(REPLEngine* repl, const std::vector<std::string>& args)
{
    (void)args; // (AR) نستعمل الوسائط الخامّ (القيمة قد تحوي مسافات) / raw args (value may have spaces)
    const std::string& raw = repl->getCommands()->pendingRawArgs_;

    // (AR) بلا وسائط: سرد كلّ البيئة (KEY=VALUE لكلّ سطر) / no args: list the whole environment
    if (raw.empty())
    {
#ifdef _WIN32
        for (char** e = _environ; e && *e; ++e)
        {
            std::cout << *e << std::endl;
        }
#else
        for (char** e = environ; e && *e; ++e)
        {
            std::cout << *e << std::endl;
        }
#endif
        return true;
    }

    std::size_t eq = raw.find('=');
    if (eq == std::string::npos)
    {
        // (AR) ‹:بيئة NAME› — اعرض قيمة المتغيّر (KEY=VALUE) / show a single var's value
        std::string name = raw;
        // (AR) قصّ الفراغ المحيط / trim surrounding whitespace
        name.erase(0, name.find_first_not_of(" \t"));
        if (auto p = name.find_last_not_of(" \t"); p != std::string::npos)
        {
            name.erase(p + 1);
        }
        if (!isValidEnvName(name))
        {
            printReplError(SoT::Error::ENV_INVALID_NAME, name);
            return true;
        }
        // (AR) اسمٌ صالح لا يكون ‹?›/‹$›، فالمُحلِّل المربوط يعادل بحث البيئة هنا. / a valid name is
        //      never ‹?›/‹$›, so the bound resolver equals a plain env lookup here.
        std::cout << name << "=" << resolveVar(repl, name) << std::endl;
        return true;
    }

    // (AR) ‹:بيئة NAME=VALUE› — اضبط. الاسم = ما قبل أوّل ‹=› (مقصوص)، والقيمة = ما بعده
    //      (مع توسيع ‹$VAR› كي يُمكن البناء التراكميّ). / set NAME to VALUE (‹$VAR› expanded).
    std::string name = raw.substr(0, eq);
    name.erase(0, name.find_first_not_of(" \t"));
    if (auto p = name.find_last_not_of(" \t"); p != std::string::npos)
    {
        name.erase(p + 1);
    }
    if (!isValidEnvName(name))
    {
        printReplError(SoT::Error::ENV_INVALID_NAME, name);
        return true;
    }
    // (AR) القيمة تُوسَّع فيها ‹$VAR›/‹$?›/‹$$› (البناء التراكميّ: ‹:بيئة LAST=$?›).
    // (EN) the value expands ‹$VAR›/‹$?›/‹$$› (cumulative build: ‹:env LAST=$?›).
    std::string value = expandEnvVars(raw.substr(eq + 1),
                                      [repl](const std::string& n) { return resolveVar(repl, n); });
    if (!setEnvVar(name, value))
    {
        printReplError(SoT::Error::ENV_INVALID_NAME, name);
    }
    return true;
}

// (AR) ‹:اذهب›/‹:cd› — تغيير مجلّد عمل الصدَفة نفسها. لا يصلح آبلتًا خارجيًّا (chdir في
//      عمليّة ابنٍ لا يمسّ الأب) فهو أمرٌ مدمج بالضرورة. المسار يُؤخَذ من الوسائط الخامّ
//      (يحترم المسافات في أسماء المجلّدات العربيّة) مع توسيع ‹$VAR›. بلا وسيط ⇒ HOME.
//      بعد النجاح يُحدَّث PWD (فترثه العمليّات المُطلَقة عبر ‹:شغّل› ويقرؤه ‹$PWD›).
// (EN) ‹:cd› — change the shell's OWN working directory. Impossible as an external applet
//      (a child's chdir never affects the parent), so it must be a builtin. The path comes
//      from the raw args (honors spaces in Arabic directory names) with ‹$VAR› expansion.
//      No arg ⇒ HOME. On success PWD is updated (inherited by ‹:run› children, read by ‹$PWD›).
bool REPLCommands::cmdCd(REPLEngine* repl, const std::vector<std::string>& args)
{
    (void)args; // (AR) نستعمل الوسائط الخامّ (المسار قد يحوي مسافات) / raw args (path may have spaces)
    std::string path = repl->getCommands()->pendingRawArgs_;
    // (AR) قصّ الفراغ المحيط / trim surrounding whitespace
    path.erase(0, path.find_first_not_of(" \t"));
    if (auto p = path.find_last_not_of(" \t"); p != std::string::npos)
    {
        path.erase(p + 1);
    }
    if (path.empty())
    {
        path = resolveVar(repl, "HOME");
        if (path.empty())
        {
            // (AR) لا وسيط ولا HOME — لا وجهة معلومة / no arg and no HOME — nowhere to go
            std::cout << usageLine(repl, "cd") << std::endl;
            return true;
        }
    }
    else
    {
        // (AR) توسيع ‹$VAR› في المسار (مثل ‹:اذهب $HOME/مشاريع›) / expand ‹$VAR› in the path
        path = expandEnvVars(path,
                             [repl](const std::string& n) { return resolveVar(repl, n); });
    }

    // (AR) على ويندوز: الواجهات العريضة (W) حصرًا — نسخ ANSI (A) تفسّر UTF-8 بترميز صفحة
    //      النظام فتشوّه المسارات العربيّة (رصدتها مراجعة أميليا قبل أيّ استخدام فعليّ).
    // (EN) On Windows: wide (W) APIs only — the ANSI (A) variants decode via the system
    //      code page and mangle Arabic UTF-8 paths (caught in review before any real use).
#ifdef _WIN32
    const bool ok = SetCurrentDirectoryW(sad::utf8::to_wstring(path).c_str()) != 0;
#else
    const bool ok = ::chdir(path.c_str()) == 0;
#endif
    if (!ok)
    {
        printReplError(SoT::Error::CD_FAILED, path);
        return true;
    }

    // (AR) حدِّث PWD بالمسار المطلق الفعليّ (getcwd يطوي ‹..› والروابط النسبيّة). فشل
    //      الاستعلام بعد نجاح الانتقال حافّة نادرة تُبقي PWD السابق — مقبولة موثَّقة.
    // (EN) refresh PWD with the real absolute path (getcwd folds ‹..›/relative links). A
    //      query failure after a successful chdir leaves the old PWD — rare, documented.
#ifdef _WIN32
    // (AR) استعلام الطول أوّلًا (0) بدل MAX_PATH الثابت — لا قصّ للمسارات الطويلة.
    // (EN) query the needed length first (0) instead of a fixed MAX_PATH — no truncation.
    if (DWORD n = GetCurrentDirectoryW(0, nullptr); n != 0)
    {
        std::wstring wcwd(n, L'\0');
        if (GetCurrentDirectoryW(n, wcwd.data()) != 0)
        {
            wcwd.resize(wcslen(wcwd.c_str()));
            setEnvVar("PWD", sad::utf8::from_wstring(wcwd));
        }
    }
#else
    if (char* cwd = ::getcwd(nullptr, 0))
    {
        setEnvVar("PWD", cwd);
        std::free(cwd);
    }
#endif
    return true;
}

bool REPLCommands::cmdType(REPLEngine* repl, const std::vector<std::string>& args)
{
    if (args.empty()) {
        std::cout << usageLine(repl, "type") << std::endl;
        return true;
    }
    
    // Combine arguments / دمج الوسائط
    std::string expr;
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) expr += " ";
        expr += args[i];
    }

    // (AR) ع-2: اقرأ النوع الحقيقيّ بتقييم المدخل **كتعبيرٍ** (لا كجملة) عبر exprTypeName —
    //      إذ جملة التعبير المجرَّدة تُرجع Void فتُفقَد قيمتها ونوعُها. بلا isdigit على بايتات
    //      UTF-8 (سلوك غير مُعرَّف) وبلا استدلالٍ نصّيّ هشّ من نصّ الناتج.
    // (EN) ع-2: read the real type by evaluating the input AS AN EXPRESSION (not a statement)
    //      via exprTypeName — a bare expression statement returns Void, losing its value and
    //      type. No isdigit on UTF-8 bytes (UB), no fragile text inference.
    std::string typeName, errorMsg;
    if (repl->exprTypeName(expr, typeName, errorMsg)) {
        std::cout << typeName << std::endl;
    } else {
        std::cerr << errorMsg << std::endl; // (AR) الخطأ إلى stderr / (EN) error to stderr
    }
    return true;
}

bool REPLCommands::cmdLoad(REPLEngine* repl, const std::vector<std::string>& args)
{
    if (args.empty()) {
        std::cout << usageLine(repl, "load") << std::endl;
        return true;
    }

    repl->loadFile(args[0]);
    return true;
}

bool REPLCommands::cmdHistory(REPLEngine* repl, const std::vector<std::string>& args)
{
    (void)args;
    // (AR) ع-3: سرد فعليّ من مدير التاريخ (كان كعبًا «Coming soon»).
    // (EN) ع-3: real listing from the history manager (was a "Coming soon" stub).
    HistoryManager* hist = repl->getHistory();
    if (!hist || hist->getAll().empty()) {
        std::cout << SoT::messageBoth(SoT::Message::NO_HISTORY) << std::endl;
        return true;
    }

    const auto& all = hist->getAll();
    std::cout << SoT::messageBoth(SoT::Message::HISTORY_HEADER)
              << " (" << all.size() << "):" << std::endl;
    for (size_t i = 0; i < all.size(); ++i) {
        std::cout << "  " << (i + 1) << "  " << all[i] << std::endl;
    }
    return true;
}

bool REPLCommands::cmdReset(REPLEngine* repl, const std::vector<std::string>& args)
{
    repl->reset();
    return true;
}

bool REPLCommands::cmdVars(REPLEngine* repl, const std::vector<std::string>& args)
{
    (void)args;
    // (AR) ع-3: سرد فعليّ من مدير المتغيّرات مع نوع كلٍّ وقيمته (كان كعبًا «Coming soon»).
    // (EN) ع-3: real listing from the variable manager with each var's type and value.
    auto* interp = repl->getInterpreter();
    if (!interp) {
        std::cout << SoT::messageBoth(SoT::Message::NO_INTERPRETER) << std::endl;
        return true;
    }

    auto& vars = interp->getVariableManager();
    std::vector<std::string> names = vars.getVariableNames();
    if (names.empty()) {
        std::cout << SoT::messageBoth(SoT::Message::NO_VARS) << std::endl;
        return true;
    }

    std::sort(names.begin(), names.end());
    std::cout << SoT::messageBoth(SoT::Message::VARS_HEADER)
              << " (" << names.size() << "):" << std::endl;
    for (const auto& name : names) {
        const Data::Value& value = vars.get(name);
        std::cout << "  " << name << " : "
                  << ::Sad::Types::sadTypeKindArabicName(value.getKind())
                  << " = " << value.toString() << std::endl;
    }
    return true;
}

bool REPLCommands::cmdFuncs(REPLEngine* repl, const std::vector<std::string>& args)
{
    (void)args;
    // (AR) ع-3: سرد فعليّ لدوال المستخدم من مدير الدوال. نُرشِّح المدمجات (مئات مُسجَّلة
    //      عبر registerBuiltinFunction) بمقارنة FunctionType، فنعرض المُعرَّفة من المستخدم
    //      واللامدا فقط بتوقيعها.
    // (EN) ع-3: real listing of user functions. We filter out builtins (hundreds registered
    //      via registerBuiltinFunction) by FunctionType, showing only user-defined and
    //      lambda functions with their signatures.
    auto* interp = repl->getInterpreter();
    if (!interp) {
        std::cout << SoT::messageBoth(SoT::Message::NO_INTERPRETER) << std::endl;
        return true;
    }

    auto& funcs = interp->getFunctionManager();
    std::vector<std::string> names = funcs.getFunctionNames();
    std::sort(names.begin(), names.end());

    std::vector<std::string> lines;
    for (const auto& name : names) {
        for (const auto& def : funcs.getFunctionOverloads(name)) {
            if (def && def->getType() != Data::FunctionType::BUILT_IN) {
                lines.push_back(def->getSignature());
            }
        }
    }

    if (lines.empty()) {
        std::cout << SoT::messageBoth(SoT::Message::NO_FUNCS) << std::endl;
        return true;
    }

    std::cout << SoT::messageBoth(SoT::Message::FUNCS_HEADER)
              << " (" << lines.size() << "):" << std::endl;
    for (const auto& line : lines) {
        std::cout << "  " << line << std::endl;
    }
    return true;
}

} // namespace REPL
} // namespace Sad
