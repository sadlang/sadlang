// بسم الله الرحمن الرحيم
/**
 * @file repl_commands.h
 * @brief معالج الأوامر الخاصة بـ REPL / REPL Special Commands Handler
 * 
 * (AR) يعالج الأوامر الخاصة التي تبدأ بـ ":"
 * (EN) Handles special commands that start with ":"
 * 
 * الأوامر المدعومة / Supported Commands:
 * - :help / :مساعدة        - عرض المساعدة / Show help
 * - :exit / :خروج          - الخروج / Exit REPL
 * - :quit / :إنهاء         - الخروج / Exit REPL
 * - :clear / :مسح          - مسح الشاشة / Clear screen
 * - :type <expr> / :نوع    - عرض نوع التعبير / Show expression type
 * - :load <file> / :حمل    - تحميل ملف / Load file
 * - :history / :تاريخ      - عرض التاريخ / Show history
 * - :reset / :إعادة        - إعادة تعيين REPL / Reset REPL
 * - :vars / :متغيرات       - عرض المتغيرات / Show variables
 * - :funcs / :دوال         - عرض الدوال / Show functions
 * 
 * @author Sad Language Team
 * @date January 2026
 */

#pragma once

// (AR) مسار جذريّ (يُحلّ عبر ${CMAKE_SOURCE_DIR}) — ثابت رغم البنية غير المسطّحة
// (EN) root-relative (resolved via ${CMAKE_SOURCE_DIR}) — stable under non-flat layout
#include "interpreter/include/core/interpreter_core.h"
#include "history_manager.h"
#include <string>
#include <functional>
#include <map>

namespace Sad {
namespace REPL {

// Forward declarations
class REPLEngine;

/**
 * @brief نوع دالة الأمر / Command function type
 */
using CommandFunc = std::function<bool(REPLEngine*, const std::vector<std::string>&)>;

/**
 * @brief معلومات الأمر / Command information
 */
struct CommandInfo {
    std::string name;               ///< اسم الأمر / Command name
    std::string arabicName;         ///< الاسم العربي / Arabic name
    std::string description;        ///< الوصف / Description
    std::string usage;              ///< الاستخدام / Usage
    CommandFunc handler;            ///< معالج الأمر / Command handler
};

/**
 * @brief معالج الأوامر الخاصة / Special Commands Handler
 */
class REPLCommands {
public:
    /**
     * @brief البناء / Constructor
     * @param repl مرجع لمحرك REPL / Reference to REPL engine
     */
    explicit REPLCommands(REPLEngine* repl);
    
    /**
     * @brief معالجة أمر / Process command
     * @param command الأمر (مع أو بدون ":") / Command (with or without ":")
     * @return true إذا كان أمر صحيح ونُفذ / true if valid command and executed
     */
    bool process(const std::string& command);
    
    /**
     * @brief فحص إذا كان النص أمر / Check if text is a command
     * @param text النص / Text
     * @return true إذا كان أمر / true if command
     */
    bool isCommand(const std::string& text) const;
    
    /**
     * @brief الحصول على قائمة جميع الأوامر / Get list of all commands
     * @return قائمة الأوامر / Command list
     */
    std::vector<CommandInfo> getAllCommands() const;
    
    /**
     * @brief الحصول على معلومات أمر / Get command information
     * @param name اسم الأمر / Command name
     * @return معلومات الأمر أو nullptr / Command info or nullptr
     */
    const CommandInfo* getCommandInfo(const std::string& name) const;

private:
    /**
     * @brief تسجيل أمر / Register command
     * @param name الاسم الإنجليزي / English name
     * @param arabicName الاسم العربي / Arabic name
     * @param description الوصف / Description
     * @param usage الاستخدام / Usage
     * @param handler المعالج / Handler
     */
    void registerCommand(const std::string& name,
                        const std::string& arabicName,
                        const std::string& description,
                        const std::string& usage,
                        CommandFunc handler);
    
    /**
     * @brief تسجيل جميع الأوامر / Register all commands
     */
    void registerAllCommands();
    
    // ========================================
    // معالجات الأوامر / Command Handlers
    // ========================================
    
    /**
     * @brief عرض المساعدة / Show help
     */
    static bool cmdHelp(REPLEngine* repl, const std::vector<std::string>& args);
    
    /**
     * @brief الخروج / Exit
     */
    static bool cmdExit(REPLEngine* repl, const std::vector<std::string>& args);
    
    /**
     * @brief مسح الشاشة / Clear screen
     */
    static bool cmdClear(REPLEngine* repl, const std::vector<std::string>& args);
    
    /**
     * @brief عرض نوع التعبير / Show expression type
     */
    static bool cmdType(REPLEngine* repl, const std::vector<std::string>& args);
    
    /**
     * @brief تحميل ملف / Load file
     */
    static bool cmdLoad(REPLEngine* repl, const std::vector<std::string>& args);
    
    /**
     * @brief عرض التاريخ / Show history
     */
    static bool cmdHistory(REPLEngine* repl, const std::vector<std::string>& args);
    
    /**
     * @brief إعادة تعيين REPL / Reset REPL
     */
    static bool cmdReset(REPLEngine* repl, const std::vector<std::string>& args);
    
    /**
     * @brief عرض المتغيرات / Show variables
     */
    static bool cmdVars(REPLEngine* repl, const std::vector<std::string>& args);
    
    /**
     * @brief عرض الدوال / Show functions
     */
    static bool cmdFuncs(REPLEngine* repl, const std::vector<std::string>& args);
    
private:
    REPLEngine* repl_;                          ///< محرك REPL / REPL engine
    std::map<std::string, CommandInfo> commands_; ///< الأوامر / Commands
};

} // namespace REPL
} // namespace Sad
