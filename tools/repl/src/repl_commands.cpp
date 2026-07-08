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

// Color codes / أكواد الألوان (static to avoid multiple definition)
static const char* RESET = "\033[0m";
static const char* BOLD = "\033[1m";
static const char* GREEN = "\033[32m";
static const char* YELLOW = "\033[33m";
static const char* CYAN = "\033[36m";

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
    
    // Evaluate and get type / تقييم والحصول على النوع
    std::string result = repl->evaluate(expr);
    
    // Simple type detection / كشف نوع بسيط
    std::string type = "غير معروف / Unknown";
    
    if (result.find("Error") == std::string::npos) {
        if (result == "true" || result == "false" || 
            result == "صحيح" || result == "خطأ") {
            type = "منطقي / Boolean";
        } else if (result[0] == '"' || result[0] == '\'') {
            type = "نص / String";
        } else if (result[0] == '[') {
            type = "مصفوفة / Array";
        } else if (result[0] == '{') {
            type = "كائن / Object";
        } else if (result.find('.') != std::string::npos) {
            type = "عشري / Float";
        } else if (std::isdigit(result[0]) || result[0] == '-') {
            type = "رقم / Number";
        } else if (result.find("دالة") != std::string::npos || 
                   result.find("function") != std::string::npos) {
            type = "دالة / Function";
        }
    }
    
    std::cout << type << std::endl;
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
    // Note: This requires access to history manager
    // For now, just print a message
    std::cout << "تاريخ الأوامر / Command History:" << std::endl;
    std::cout << "(سيتم تنفيذه قريباً / Coming soon)" << std::endl;
    return true;
}

bool REPLCommands::cmdReset(REPLEngine* repl, const std::vector<std::string>& args)
{
    repl->reset();
    return true;
}

bool REPLCommands::cmdVars(REPLEngine* repl, const std::vector<std::string>& args)
{
    std::cout << "المتغيرات / Variables:" << std::endl;
    std::cout << "(سيتم تنفيذه قريباً / Coming soon)" << std::endl;
    return true;
}

bool REPLCommands::cmdFuncs(REPLEngine* repl, const std::vector<std::string>& args)
{
    std::cout << "الدوال / Functions:" << std::endl;
    std::cout << "(سيتم تنفيذه قريباً / Coming soon)" << std::endl;
    return true;
}

} // namespace REPL
} // namespace Sad
