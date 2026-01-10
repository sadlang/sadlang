// بسم الله الرحمن الرحيم
/**
 * @file repl_engine.h
 * @brief محرك REPL للغة ص / REPL Engine for Sad Language
 * 
 * (AR) محرك تفاعلي للقراءة-التقييم-الطباعة-التكرار
 * (EN) Read-Eval-Print-Loop interactive engine
 * 
 * الميزات / Features:
 * - دعم multiline input
 * - تاريخ الأوامر (command history)
 * - تلقائي إكمال (auto-completion)
 * - أوامر خاصة (:type, :help, :exit, :load, :clear)
 * - معالجة أخطاء ذكية
 * - تلوين الإخراج
 * 
 * @author Sad Language Team
 * @date January 2026
 */

#pragma once

#include "../../shared/lexer/include/lexer_core.h"
#include "../../shared/parser/include/parser_core.h"
// interpreter_core.h path updated
#include "../../interpreter_new/include/core/interpreter_core.h"
#include "../../shared/errors/include/error_manager.h"
#include "history_manager.h"
#include "repl_commands.h"
#include <string>
#include <vector>
#include <memory>

namespace Sad {
namespace REPL {

/**
 * @brief إعدادات REPL / REPL Configuration
 */
struct REPLConfig {
    bool enableColor = true;            ///< (AR) تفعيل الألوان / (EN) Enable colors
    bool enableHistory = true;          ///< (AR) تفعيل التاريخ / (EN) Enable history
    bool enableAutoComplete = true;     ///< (AR) تفعيل الإكمال التلقائي / (EN) Enable auto-completion
    bool printResults = true;           ///< (AR) طباعة النتائج / (EN) Print results
    bool showPrompt = true;             ///< (AR) عرض موجه الأوامر / (EN) Show prompt
    size_t maxHistorySize = 1000;      ///< (AR) حجم التاريخ الأقصى / (EN) Max history size
    std::string historyFile = ".sad_history"; ///< (AR) ملف التاريخ / (EN) History file
    std::string promptSymbol = ">>> ";  ///< (AR) رمز الموجه / (EN) Prompt symbol
    std::string multilinePrompt = "... "; ///< (AR) موجه متعدد الأسطر / (EN) Multiline prompt
};

/**
 * @brief حالة REPL / REPL State
 */
enum class REPLState {
    Ready,              ///< (AR) جاهز / (EN) Ready
    MultiLine,          ///< (AR) متعدد الأسطر / (EN) Multi-line input
    Executing,          ///< (AR) قيد التنفيذ / (EN) Executing
    Error,              ///< (AR) خطأ / (EN) Error
    Exiting             ///< (AR) الخروج / (EN) Exiting
};

/**
 * @brief محرك REPL الأساسي / Core REPL Engine
 */
class REPLEngine {
public:
    /**
     * @brief البناء / Constructor
     * @param config إعدادات REPL / REPL configuration
     */
    explicit REPLEngine(const REPLConfig& config = REPLConfig{});
    
    /**
     * @brief المدمر / Destructor
     */
    ~REPLEngine();
    
    /**
     * @brief بدء حلقة REPL / Start REPL loop
     * @return كود الخروج / Exit code (0 = success)
     */
    int run();
    
    /**
     * @brief تقييم سطر واحد / Evaluate single line
     * @param line السطر للتقييم / Line to evaluate
     * @return النتيجة كنص / Result as string
     */
    std::string evaluate(const std::string& line);
    
    /**
     * @brief تقييم عدة أسطر / Evaluate multiple lines
     * @param lines الأسطر للتقييم / Lines to evaluate
     * @return النتيجة كنص / Result as string
     */
    std::string evaluateMultiline(const std::vector<std::string>& lines);
    
    /**
     * @brief تحميل ملف للتنفيذ / Load file for execution
     * @param filename اسم الملف / File name
     * @return true إذا نجح / true if successful
     */
    bool loadFile(const std::string& filename);
    
    /**
     * @brief إعادة تعيين الحالة / Reset state
     */
    void reset();
    
    /**
     * @brief الحصول على الحالة الحالية / Get current state
     * @return الحالة / State
     */
    REPLState getState() const { return state_; }
    
    /**
     * @brief الحصول على الإعدادات / Get configuration
     * @return الإعدادات / Configuration
     */
    const REPLConfig& getConfig() const { return config_; }
    
    /**
     * @brief تعيين الإعدادات / Set configuration
     * @param config الإعدادات الجديدة / New configuration
     */
    void setConfig(const REPLConfig& config) { config_ = config; }
    
    /**
     * @brief الحصول على معالج الأوامر / Get command handler
     * @return معالج الأوامر / Command handler
     */
    REPLCommands* getCommands() const { return commands_.get(); }

private:
    /**
     * @brief معالجة سطر إدخال / Process input line
     * @param line السطر / Line
     * @return true للاستمرار، false للخروج / true to continue, false to exit
     */
    bool processLine(const std::string& line);
    
    /**
     * @brief معالجة أمر خاص / Process special command
     * @param command الأمر / Command
     * @return true إذا كان أمر خاص / true if special command
     */
    bool processCommand(const std::string& command);
    
    /**
     * @brief معالجة كود عادي / Process normal code
     * @param code الكود / Code
     */
    void processCode(const std::string& code);
    
    /**
     * @brief فحص إذا كان السطر غير مكتمل / Check if line is incomplete
     * @param line السطر / Line
     * @return true إذا كان غير مكتمل / true if incomplete
     */
    bool isIncomplete(const std::string& line);
    
    /**
     * @brief طباعة الموجه / Print prompt
     */
    void printPrompt();
    
    /**
     * @brief طباعة النتيجة / Print result
     * @param result النتيجة / Result
     */
    void printResult(const Data::Value& result);
    
    /**
     * @brief طباعة الخطأ / Print error
     * @param message رسالة الخطأ / Error message
     */
    void printError(const std::string& message);
    
    /**
     * @brief طباعة رأس REPL / Print REPL header
     */
    void printHeader();
    
    /**
     * @brief قراءة سطر مع دعم التاريخ / Read line with history support
     * @return السطر المقروء / Read line
     */
    std::string readLine();
    
private:
    REPLConfig config_;                                         ///< الإعدادات / Configuration
    REPLState state_;                                           ///< الحالة / State
    std::unique_ptr<HistoryManager> history_;                   ///< مدير التاريخ / History manager
    std::unique_ptr<REPLCommands> commands_;                    ///< معالج الأوامر / Command handler
    std::unique_ptr<Interpreter::Interpreter> interpreter_;     ///< المفسر / Interpreter
    // Note: ErrorManager is singleton, accessed via getInstance()
    std::vector<std::string> multilineBuffer_;                  ///< مخزن متعدد الأسطر / Multiline buffer
    int bracketDepth_;                                          ///< عمق الأقواس / Bracket depth
    bool lastWasEmpty_;                                         ///< آخر سطر كان فارغ / Last was empty
};

} // namespace REPL
} // namespace Sad
