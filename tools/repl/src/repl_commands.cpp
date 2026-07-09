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
#include <iostream>
#include <sstream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

namespace Sad {
namespace REPL {

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
    
    // Find command / البحث عن الأمر
    auto it = commands_.find(cmdName);
    if (it != commands_.end()) {
        return it->second.handler(repl_, args);
    }
    
    std::cout << "أمر غير معروف / Unknown command: " << cmdName << std::endl;
    std::cout << "اكتب ':help' للمساعدة / Type ':help' for help" << std::endl;
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

void REPLCommands::registerAllCommands()
{
    registerCommand("help", "مساعدة",
        "عرض المساعدة / Show help",
        ":help",
        cmdHelp);
    
    registerCommand("exit", "خروج",
        "الخروج من REPL / Exit REPL",
        ":exit",
        cmdExit);
    
    registerCommand("quit", "إنهاء",
        "الخروج من REPL / Exit REPL",
        ":quit",
        cmdExit);
    
    registerCommand("clear", "مسح",
        "مسح الشاشة / Clear screen",
        ":clear",
        cmdClear);
    
    registerCommand("type", "نوع",
        "عرض نوع التعبير / Show expression type",
        ":type <expression>",
        cmdType);
    
    registerCommand("load", "حمل",
        "تحميل وتنفيذ ملف / Load and execute file",
        ":load <filename>",
        cmdLoad);
    
    registerCommand("history", "تاريخ",
        "عرض تاريخ الأوامر / Show command history",
        ":history",
        cmdHistory);
    
    registerCommand("reset", "إعادة",
        "إعادة تعيين حالة REPL / Reset REPL state",
        ":reset",
        cmdReset);
    
    registerCommand("vars", "متغيرات",
        "عرض جميع المتغيرات / Show all variables",
        ":vars",
        cmdVars);
    
    registerCommand("funcs", "دوال",
        "عرض جميع الدوال / Show all functions",
        ":funcs",
        cmdFuncs);
}

// ============================================================================
// Command Handlers / معالجات الأوامر
// ============================================================================

bool REPLCommands::cmdHelp(REPLEngine* repl, const std::vector<std::string>& args)
{
    auto& config = repl->getConfig();
    
    if (config.enableColor) {
        std::cout << BOLD << CYAN << "الأوامر المتاحة / Available Commands:" 
                  << RESET << "\n\n";
    } else {
        std::cout << "الأوامر المتاحة / Available Commands:\n\n";
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

bool REPLCommands::cmdType(REPLEngine* repl, const std::vector<std::string>& args)
{
    if (args.empty()) {
        std::cout << "الاستخدام / Usage: :type <expression>" << std::endl;
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
        std::cout << "الاستخدام / Usage: :load <filename>" << std::endl;
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
        std::cout << "لا تاريخ بعد / No history yet." << std::endl;
        return true;
    }

    const auto& all = hist->getAll();
    std::cout << "تاريخ الأوامر / Command History (" << all.size() << "):" << std::endl;
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
        std::cout << "لا مفسّر / No interpreter." << std::endl;
        return true;
    }

    auto& vars = interp->getVariableManager();
    std::vector<std::string> names = vars.getVariableNames();
    if (names.empty()) {
        std::cout << "لا متغيّرات معرّفة / No variables defined." << std::endl;
        return true;
    }

    std::sort(names.begin(), names.end());
    std::cout << "المتغيّرات / Variables (" << names.size() << "):" << std::endl;
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
        std::cout << "لا مفسّر / No interpreter." << std::endl;
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
        std::cout << "لا دوال معرّفة من المستخدم / No user-defined functions." << std::endl;
        return true;
    }

    std::cout << "الدوال / Functions (" << lines.size() << "):" << std::endl;
    for (const auto& line : lines) {
        std::cout << "  " << line << std::endl;
    }
    return true;
}

} // namespace REPL
} // namespace Sad
