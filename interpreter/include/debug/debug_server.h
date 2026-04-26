/**
 * @file debug_server.h
 * @brief (AR) خادم التصحيح — بروتوكول JSON عبر stdin/stdout للتواصل مع محول DAP
 * @brief (EN) Debug Server — JSON protocol over stdin/stdout to communicate with DAP adapter
 *
 * يعمل هذا الخادم كوسيط بين المفسر ومحول DAP في إضافة VS Code.
 * البروتوكول: سطر JSON واحد لكل رسالة (JSON Lines / NDJSON)
 *
 * This server acts as intermediary between the interpreter and the DAP adapter
 * in the VS Code extension.
 * Protocol: one JSON line per message (JSON Lines / NDJSON)
 *
 * @author فريق لغة ص
 * @date 2025
 */

#pragma once

#include "interpreter_core.h"
#include "value.h"
#include "variable_manager.h"
#include "function_manager.h"
#include "scope_manager.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <sstream>

namespace Sad {
namespace Debug {

// ═══════════════════════════════════════════════════════════════════
// (AR) معلومات نقطة التوقف
// (EN) Breakpoint information
// ═══════════════════════════════════════════════════════════════════
struct DebugBreakpoint {
    int id = 0;                     ///< معرّف فريد / Unique ID
    std::string file;               ///< مسار الملف / File path
    int line = 0;                   ///< رقم السطر / Line number
    bool enabled = true;            ///< مفعّل / Enabled
    std::string condition;          ///< شرط (اختياري) / Condition (optional)
    int hitCount = 0;               ///< عدد مرات الوصول / Hit count
    int hitCondition = 0;           ///< شرط العدد (اختياري) / Hit count condition
    bool verified = false;          ///< تم التحقق / Verified
};

// ═══════════════════════════════════════════════════════════════════
// (AR) معلومات متغير للتصحيح
// (EN) Debug variable information
// ═══════════════════════════════════════════════════════════════════
struct DebugVariable {
    std::string name;               ///< اسم المتغير / Variable name
    std::string value;              ///< القيمة كنص / Value as string
    std::string type;               ///< النوع / Type
    int variablesReference = 0;     ///< مرجع للمتغيرات الفرعية / Reference for child variables
};

// ═══════════════════════════════════════════════════════════════════
// (AR) إطار في مكدس الاستدعاءات
// (EN) Stack frame
// ═══════════════════════════════════════════════════════════════════
struct DebugStackFrame {
    int id = 0;                     ///< معرّف الإطار / Frame ID
    std::string name;               ///< اسم الدالة / Function name
    std::string file;               ///< مسار الملف / File path
    int line = 0;                   ///< رقم السطر / Line number
    int column = 0;                 ///< رقم العمود / Column number
};

// ═══════════════════════════════════════════════════════════════════
// (AR) حالة المصحح
// (EN) Debugger state
// ═══════════════════════════════════════════════════════════════════
enum class DebugAction {
    NONE,           ///< لا شيء / None
    CONTINUE,       ///< استمرار / Continue
    STEP_IN,        ///< خطوة للداخل / Step into
    STEP_OVER,      ///< تخطي / Step over
    STEP_OUT,       ///< خروج من الدالة / Step out
    PAUSE,          ///< إيقاف مؤقت / Pause
    DISCONNECT      ///< فصل / Disconnect
};

// ═══════════════════════════════════════════════════════════════════
// (AR) خادم التصحيح
// (EN) Debug Server
// ═══════════════════════════════════════════════════════════════════
class DebugServer {
public:
    DebugServer();
    ~DebugServer();

    /**
     * @brief (AR) تشغيل خادم التصحيح — يقرأ الأوامر من stdin ويُرسل الأحداث إلى stdout
     * @brief (EN) Run debug server — reads commands from stdin, sends events to stdout
     * @param sourceFile مسار الملف المصدري / Source file path
     */
    void run(const std::string& sourceFile);

    // ═══════════════════════════════════════════════════════════════
    // (AR) خطافات التصحيح — تُستدعى من المفسر
    // (EN) Debug hooks — called from interpreter
    // ═══════════════════════════════════════════════════════════════

    /**
     * @brief (AR) يُستدعى قبل تنفيذ كل جملة
     * @brief (EN) Called before each statement execution
     * @param file اسم الملف / File name
     * @param line رقم السطر / Line number
     * @param column رقم العمود / Column number
     */
    void onBeforeStatement(const std::string& file, int line, int column);

    /**
     * @brief (AR) يُستدعى عند دخول دالة
     * @brief (EN) Called when entering a function
     * @param functionName اسم الدالة / Function name
     * @param file اسم الملف / File name
     * @param line رقم السطر / Line number
     */
    void onFunctionEnter(const std::string& functionName, const std::string& file, int line);

    /**
     * @brief (AR) يُستدعى عند الخروج من دالة
     * @brief (EN) Called when exiting a function
     */
    void onFunctionExit();

    /**
     * @brief (AR) يُستدعى عند حدوث استثناء
     * @brief (EN) Called when an exception occurs
     * @param message رسالة الخطأ / Error message
     * @param file اسم الملف / File name
     * @param line رقم السطر / Line number
     */
    void onException(const std::string& message, const std::string& file, int line);

    /**
     * @brief (AR) يُستدعى عند طباعة مخرجات
     * @brief (EN) Called when output is printed
     * @param text النص / Text
     */
    void onOutput(const std::string& text);

    /**
     * @brief (AR) تعيين مراجع المفسر للوصول للمتغيرات
     * @brief (EN) Set interpreter references for variable access
     */
    void setInterpreterRefs(Data::VariableManager* varMgr,
                            Data::FunctionManager* funcMgr,
                            Data::ScopeManager* scopeMgr);

    /**
     * @brief (AR) تعيين مرجع المفسر الكامل (لتقييم التعبيرات)
     * @brief (EN) Set full interpreter reference (for expression evaluation)
     */
    void setInterpreter(Interpreter::Interpreter* interp) { interpreter_ = interp; }

    /**
     * @brief (AR) إرسال حدث إلى محول DAP
     * @brief (EN) Send event to DAP adapter
     */
    void sendEvent(const std::string& event, const std::string& bodyJson = "{}");

    /**
     * @brief (AR) تقييم تعبير في سياق التصحيح
     * @brief (EN) Evaluate expression in debug context
     */
    std::string evaluateExpression(const std::string& expression);

    /**
     * @brief (AR) هل المصحح متصل؟
     * @brief (EN) Is debugger connected?
     */
    bool isConnected() const { return connected_.load(); }

    /**
     * @brief (AR) هل يجب إيقاف التنفيذ؟
     * @brief (EN) Should we stop execution?
     */
    bool shouldStop() const { return shouldDisconnect_.load(); }

    /**
     * @brief (AR) الحصول على المثيل الوحيد (Singleton)
     * @brief (EN) Get singleton instance
     */
    static DebugServer* getInstance();
    static void setInstance(DebugServer* instance);

private:
    // ─── بروتوكول I/O ───
    void sendResponse(int requestSeq, const std::string& command, const std::string& bodyJson = "{}");
    void sendErrorResponse(int requestSeq, const std::string& command, const std::string& message);
    std::string readLine();
    void writeLine(const std::string& line);

    // ─── معالجة الأوامر ───
    void processCommand(const std::string& jsonLine);
    void handleSetBreakpoints(int seq, const std::string& json);
    void handleContinue(int seq);
    void handleNext(int seq);        // step over
    void handleStepIn(int seq);
    void handleStepOut(int seq);
    void handlePause(int seq);
    void handleStackTrace(int seq);
    void handleScopes(int seq, const std::string& json);
    void handleVariables(int seq, const std::string& json);
    void handleEvaluate(int seq, const std::string& json);
    void handleDisconnect(int seq);

    // ─── حالة التوقف والانتظار ───
    void waitForDebugAction();
    void stopAndWait(const std::string& reason, int line, const std::string& file);

    // ─── جمع المتغيرات ───
    std::vector<DebugVariable> collectVariables(int scopeRef);
    std::vector<DebugStackFrame> collectStackFrames();

    // ─── JSON helpers ───
    static std::string escapeJson(const std::string& s);
    static std::string jsonPair(const std::string& key, const std::string& value);
    static std::string jsonPairNum(const std::string& key, int value);
    static std::string jsonPairBool(const std::string& key, bool value);
    static int extractIntField(const std::string& json, const std::string& field);
    static std::string extractStringField(const std::string& json, const std::string& field);
    static std::vector<int> extractIntArray(const std::string& json, const std::string& field);

    // ─── بيانات التصحيح ───
    std::unordered_map<std::string, std::vector<DebugBreakpoint>> breakpoints_; // file -> breakpoints
    int nextBreakpointId_ = 1;

    // ─── مكدس الاستدعاءات ───
    struct CallFrame {
        std::string functionName;
        std::string file;
        int line;
        int depth;                  // عمق النطاق عند الدخول / Scope depth at entry
    };
    std::vector<CallFrame> callStack_;

    // ─── مراجع المفسر ───
    Data::VariableManager* varMgr_ = nullptr;
    Data::FunctionManager* funcMgr_ = nullptr;
    Data::ScopeManager* scopeMgr_ = nullptr;

    // ─── مرجع المفسر الكامل (لتقييم التعبيرات) ───
    Interpreter::Interpreter* interpreter_ = nullptr;

    // ─── حالة الخيط ───
    std::mutex mutex_;
    std::condition_variable cv_;
    DebugAction pendingAction_ = DebugAction::NONE;
    std::atomic<bool> connected_{false};
    std::atomic<bool> shouldDisconnect_{false};
    std::atomic<bool> paused_{false};

    // ─── Step tracking ───
    int stepOverDepth_ = -1;        // عمق النطاق للـ step over / Scope depth for step over
    int stepOutDepth_ = -1;         // عمق النطاق للـ step out / Scope depth for step out
    bool stepping_ = false;         // خطوة بخطوة / Stepping mode
    
    int currentLine_ = 0;
    std::string currentFile_;

    // ─── مصدر الملف ───
    std::string sourceFile_;
    std::vector<std::string> sourceLines_;

    // ─── Singleton ───
    static DebugServer* instance_;

    // ─── خيط القراءة ───
    std::thread readerThread_;
    void readerLoop();
};

} // namespace Debug
} // namespace Sad
