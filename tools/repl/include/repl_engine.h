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

#include "lexer_core.h"
#include "parser_core.h"
// (AR) مسار جذريّ (يُحلّ عبر ${CMAKE_SOURCE_DIR}) — ثابت رغم البنية غير المسطّحة
// (EN) root-relative (resolved via ${CMAKE_SOURCE_DIR}) — stable under non-flat layout
#include "interpreter/include/core/interpreter_core.h"
#include "error_manager.h"
#include "history_manager.h"
#include "repl_commands.h"
#include "repl_line_editor.h" // (AR) المُكمِّل + محرّر السطر (إدخال غنيّ اختياريّ) / (EN) completer + line editor (optional rich input)
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>

namespace Sad {
namespace REPL {

/**
 * @brief إعدادات REPL / REPL Configuration
 */
struct REPLConfig {
    bool enableColor = true;            ///< (AR) تفعيل الألوان / (EN) Enable colors
    bool enableHistory = true;          ///< (AR) تفعيل التاريخ / (EN) Enable history
    bool enableAutoComplete = true;     ///< (AR) تفعيل الإكمال التلقائي / (EN) Enable auto-completion
    bool enableLineEditor = false;      ///< (AR) محرّر سطر تفاعليّ غنيّ (أسهم/Tab)، اختياريّ عبر --rich / (EN) rich interactive line editor, opt-in via --rich
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

    /**
     * @brief الحصول على المفسّر الدائم / Get the persistent interpreter
     * @details يتيح للأوامر (:vars/:funcs) قراءة مدير المتغيّرات/الدوال مباشرةً.
     *          / Lets commands (:vars/:funcs) read the variable/function managers directly.
     */
    Interpreter::Interpreter* getInterpreter() const { return interpreter_.get(); }

    /**
     * @brief الحصول على مدير التاريخ / Get the history manager
     * @details يتيح لأمر :history سرد الأوامر السابقة. / Lets :history list prior commands.
     */
    HistoryManager* getHistory() const { return history_.get(); }

    /**
     * @brief هل كان ناتج آخر تقييم رسالة خطأ؟ / Was the last evaluate() result an error message?
     * @details يُميّز مسار الطباعة الأخطاء (⇒ stderr أحمر) عن القيم (⇒ stdout)، بلا
     *          استنتاجٍ نصّيّ هشّ من بادئة الرسالة. / Lets the print path route errors
     *          (⇒ red stderr) apart from values (⇒ stdout), without fragile string sniffing.
     */
    bool lastResultIsError() const { return lastResultIsError_; }

    /**
     * @brief اسم نوع تعبيرٍ مفرد (لأمر :type) / Type name of a single expression (for :type)
     * @details يقيّم التعبير **مباشرةً** عبر interpreter_->evaluateExpression لا كجملة، إذ
     *          إنّ جملة التعبير المجرَّدة تُرجع Void فتُفقَد قيمتها؛ ثمّ يشتقّ الاسم من
     *          getKind() + المُحوِّل القانونيّ sadTypeKindArabicName (المُشتقّ من SoT).
     *          يُبقي شجرة التعبير في المرسى طوال الجلسة (كسائر الأشجار).
     *          / Evaluates the expression DIRECTLY via interpreter_->evaluateExpression, not
     *          as a statement — a bare expression statement returns Void, losing its value;
     *          then derives the name from getKind() + the canonical (SoT-sourced)
     *          sadTypeKindArabicName. Keeps the expression tree in the arena for the session.
     * @param expr نصّ التعبير / expression source
     * @param typeNameOut اسم النوع ثنائيّ اللغة عند النجاح / bilingual type name on success
     * @param errorOut رسالة الخطأ عند الفشل / error message on failure
     * @return true عند النجاح / true on success
     */
    bool exprTypeName(const std::string& expr, std::string& typeNameOut, std::string& errorOut);

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
     * @brief تنفيذ مخزن الأسطر المتعدّدة ثمّ تصفيره والعودة لحالة الجاهزيّة
     *        / Execute the multiline buffer, then clear it and return to Ready
     */
    void runMultilineBuffer();

    /**
     * @brief هل المصدر المتراكم يمثّل بنية غير مكتملة (كتلة مفتوحة تنتظر «نهاية»،
     *        أو قوس ‎(‎/‏‎[‎ غير مغلق)؟ يُقطّع المصدر بالمعجم ويعدّ عمق الكتل بكلمات
     *        الفتح/الإغلاق المأخوذة من SoT (KeywordTable::getKeywordsByRole).
     *        / Is the accumulated source an incomplete construct (an open block
     *        awaiting «نهاية», or an unbalanced ( / [ )? Tokenizes via the lexer
     *        and counts block depth using SoT-sourced opener/closer keywords.
     * @param src المصدر المتراكم / accumulated source
     */
    bool isBufferIncomplete(const std::string& src) const;

    /**
     * @brief استخراج مُعرّفات المستخدم (متغيّر/ثابت/دالة) لتغذية المُكمِّل
     *        / Extract user identifiers (var/const/func) to feed the completer
     * @param code الكود المُقيَّم / Evaluated code
     */
    void extractIdentifiers(const std::string& code);

    /**
     * @brief نصّ الموجّه الحاليّ (مع الألوان حسب الحالة) / Current prompt text (colored per state)
     */
    std::string promptText() const;

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

    /**
     * @brief تصفير تشخيصات ErrorManager المفرد قبل تحليلٍ جديد
     *        / Clear the singleton ErrorManager diagnostics before a fresh parse.
     * @details بوّابة parser.hasErrors() تفوّض للمفرد العالميّ، فأيّ تشخيص خلّفه
     *          سطرٌ/تحميلٌ سابق يسمّم التحليل التالي حتّى إعادة تشغيل العمليّة
     *          (استرداد الصدَفة مكسور بدونه). يُستدعى عند **مدخل** كلّ مسار تحليل
     *          (evaluate/loadFile) لا عند الخروج، كي يبقى getErrors() قادرًا على
     *          قراءة تشخيص الإدخال الحاليّ بعد العودة. لا يمسّ متغيّرات المفسّر
     *          (تعيش في interpreter_ لا في ErrorManager).
     *          / The parser.hasErrors() gate delegates to the global singleton, so a
     *          diagnostic a prior line/load left behind poisons the next parse until
     *          the process restarts (shell recovery is broken without this). Called at
     *          the ENTRY of each parse path (evaluate/loadFile), never on exit, so
     *          getErrors() can still read the current input's diagnostics after return.
     *          Does not touch interpreter variables (they live in interpreter_).
     */
    void resetDiagnostics();
    
private:
    REPLConfig config_;                                         ///< الإعدادات / Configuration
    REPLState state_;                                           ///< الحالة / State
    bool lastResultIsError_ = false;                            ///< (AR) هل ناتج آخر evaluate خطأ؟ / (EN) was the last evaluate() result an error?
    std::unique_ptr<HistoryManager> history_;                   ///< مدير التاريخ / History manager
    std::unique_ptr<REPLCommands> commands_;                    ///< معالج الأوامر / Command handler
    std::unique_ptr<Interpreter::Interpreter> interpreter_;     ///< المفسر / Interpreter
    // Note: ErrorManager is singleton, accessed via getInstance()
    std::vector<std::string> multilineBuffer_;                  ///< مخزن متعدد الأسطر / Multiline buffer

    // (AR) مرسى الأشجار: يُبقي كلّ شجرة AST مُحلَّلة (لكلّ سطر/ملفّ) حيّةً طوال الجلسة.
    //      المفسّر الدائم يخزّن قيمًا مفهرَسة بعنوان عقدة AST (كاش الحرفيّات) ويحمل مؤشّرات
    //      إلى أجسام الدوال/رموزها؛ لو حُرِّرت شجرة السطر بعد تنفيذه لأُعيد استخدام عناوين
    //      عقدها في السطر التالي فتُقرأ قيمٌ بائتة (تلف السلاسل/الأرقام، انهيار الدوال).
    //      إبقاؤها حيّة يمنع إعادة العناوين. إعادة تخصيص الـvector تنقل مؤشّرات الـStmtList
    //      لا العُقَد الكوميّة، فالعناوين تبقى ثابتة. (ينمو مع الجلسة — مقبول تفاعليًّا؛
    //      يُصفَّر بـ:reset عبر reset().)
    // (EN) AST arena: keeps every parsed tree (per line/file) alive for the whole session.
    //      The persistent interpreter memoizes values keyed by AST node address (literal cache)
    //      and holds pointers into function bodies/tokens; freeing a line's tree after executing
    //      it lets the next line reuse those node addresses, yielding stale reads (string/number
    //      corruption, function crashes). Keeping them alive prevents address reuse. Vector
    //      reallocation moves the StmtList pointers, not the heap nodes, so node addresses stay
    //      stable. (Grows with the session — acceptable interactively; cleared on :reset.)
    std::vector<AST::StmtList> astArena_;

    // (AR) كلمات فتح/إغلاق الكتل مأخوذةً من SoT المعجم (لا قائمة مضمّنة) — لكشف
    //      اكتمال الكتل متعدّدة الأسطر (دالة/صنف/إذا/... ... نهاية).
    // (EN) block opener/closer keywords sourced from the lexicon SoT (no inline
    //      list) — used to detect completeness of multiline blocks (… نهاية).
    std::unordered_set<std::string> blockOpeners_;              ///< كلمات فتح الكتلة / block-opener spellings
    std::unordered_set<std::string> blockClosers_;              ///< كلمات إغلاق الكتلة / block-closer spellings
    std::unordered_set<std::string> blockContinuations_;        ///< كلمات بينيّة (وإلا/امسك/…) لتمييز else-if / inter-block keywords (else/catch/…) to spot else-if
    // (AR) تهجئات «لامدا»: تُعالَج بنظرةٍ أماميّة في isBufferIncomplete (لا في المجموعة
    //      العامّة) لأنّ صيغتها التعبيريّة «=> تعبير» لا تُغلَق بـ«نهاية» بينما الكتليّة تُغلَق.
    // (EN) «لامدا» spellings: handled by a lookahead in isBufferIncomplete (not the generic
    //      set) since its «=> expr» form needs no «نهاية» while its block form does.
    std::unordered_set<std::string> lambdaWords_;               ///< تهجئات لامدا / lambda spellings

    // (AR) إدخال غنيّ اختياريّ (opt-in عبر --rich، ويُفعَّل فقط على طرفيّة تفاعليّة)
    // (EN) optional rich input (opt-in via --rich, enabled only on an interactive TTY)
    std::unique_ptr<AutoCompleter> completer_;                  ///< المُكمِّل التلقائي / Auto-completer
    std::unique_ptr<LineEditor> lineEditor_;                    ///< محرّر السطر التفاعليّ / Interactive line editor
    bool useLineEditor_ = false;                                ///< هل نستعمل محرّر السطر فعلًا؟ / Actually use the line editor?
};

} // namespace REPL
} // namespace Sad
