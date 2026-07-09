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
#include <cstdlib> // (AR) getenv/setenv/_putenv_s لمتغيّرات البيئة / (EN) env-var access

#ifdef _WIN32
#include <windows.h>
#else
extern char** environ; // (AR) قائمة بيئة العمليّة (POSIX) لسرد ‹:بيئة› / (EN) process env list
#endif

namespace Sad {
namespace REPL {

// (AR) مُحلِّل البيئة الافتراضيّ: قيمة المتغيّر من بيئة العمليّة (فارغة إن لم يُعرَّف). يمرّره
//      ‹:شغّل› للمُحلِّل، فما يضبطه ‹:بيئة› عبر setenv يظهر هنا ويرثه ما يُطلَق. / (EN) default
//      resolver: a variable's value from the process env (empty if unset). ‹:run› passes it to
//      the lexer, so whatever ‹:env› sets via setenv shows here and is inherited by launches.
static std::string envResolve(const std::string& name)
{
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
    size_t spacePos = cmd.find(' ');
    if (spacePos != std::string::npos) {
        cmd = cmd.substr(0, spacePos);
    }
    
    return commands_.find(cmd) != commands_.end();
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

bool REPLCommands::cmdHelp(REPLEngine* repl, const std::vector<std::string>& args)
{
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
//      (‹<›/‹>›/‹>>›). أمرٌ مفرد بلا توجيه ⇒ runExternal؛ غير ذلك ⇒ runPipeline. أخطاء
//      التحليل والإطلاق تُبلَّغ عبر كتالوج SoT. يرث stdin/stdout/stderr ما لم يُعِد التوجيه.
// (EN) ‹:run› — run an external command synchronously (shell ergonomics). The raw line is
//      parsed honoring quotes, ‹\› escaping, ‹|› pipes and ‹<›/‹>›/‹>>› redirection. A plain
//      single command ⇒ runExternal; otherwise ⇒ runPipeline. Errors go via the SoT catalog.
bool REPLCommands::cmdRun(REPLEngine* repl, const std::vector<std::string>& args)
{
    (void)args; // (AR) نستعمل الوسائط الخامّ لا المقسّمة بالمسافات / raw args, not the whitespace split
    const std::string& raw = repl->getCommands()->pendingRawArgs_;
    if (raw.empty())
    {
        std::cout << usageLine(repl, "run") << std::endl;
        return true;
    }

    ShellPipeline parsed = parseShellPipeline(raw, envResolve);
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
    if (parsed.stages.empty()) // (AR) دفاعيّ: خامّ بلا وسائط فعليّة / defensive: raw with no real args
    {
        std::cout << usageLine(repl, "run") << std::endl;
        return true;
    }

    // (AR) هل تحوي أيّ مرحلة إعادة توجيه؟ / (EN) does any stage carry a redirection?
    bool hasRedirect = false;
    for (const ShellStage& st : parsed.stages)
    {
        if (!st.inFile.empty() || !st.outFile.empty())
        {
            hasRedirect = true;
            break;
        }
    }

#ifdef _WIN32
    // (AR) الأنابيب المتعدّدة وإعادة التوجيه غير مدعومة على Windows في هذا الإصدار (POSIX فقط).
    // (EN) multi-stage pipes and redirection are unsupported on Windows in this release.
    if (parsed.stages.size() > 1)
    {
        printReplError(SoT::Error::PIPE_UNSUPPORTED);
        return true;
    }
    if (hasRedirect)
    {
        printReplError(SoT::Error::REDIRECT_UNSUPPORTED);
        return true;
    }
#endif

    std::cout.flush();
    // (AR) أمرٌ مفرد بلا توجيه ⇒ المسار المُختبَر runExternal؛ غير ذلك ⇒ runPipeline.
    // (EN) plain single command ⇒ the tested runExternal path; otherwise ⇒ runPipeline.
    const bool plainSingle = (parsed.stages.size() == 1 && !hasRedirect);
    LaunchFailure fail;
    ShellResult r = plainSingle ? runExternal(parsed.stages[0].argv)
                                : runPipeline(parsed.stages, fail);
    if (!r.spawned)
    {
        if (fail.isRedirect)
        {
            // (AR) تعذّر فتح ملفّ إعادة التوجيه / redirection file could not be opened
            printReplError(SoT::Error::REDIRECT_FAILED, fail.file);
        }
        else
        {
            // (AR) اسم البرنامج المُخفِق: من المرحلة الفاشلة، أو أوّل برنامج للأمر المفرد.
            // (EN) failing program: the failed stage, or the single command's program.
            const std::string& prog =
                !fail.program.empty() ? fail.program : parsed.stages[0].argv[0];
            printReplError(SoT::Error::RUN_FAILED, prog);
        }
    }
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
        std::cout << name << "=" << envResolve(name) << std::endl;
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
    std::string value = expandEnvVars(raw.substr(eq + 1), envResolve);
    if (!setEnvVar(name, value))
    {
        printReplError(SoT::Error::ENV_INVALID_NAME, name);
    }
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
