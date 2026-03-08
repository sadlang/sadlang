/**
 * @file dap_server.h
 * @brief خادم بروتوكول محول التصحيح (DAP) للغة ص
 * @brief (EN) Debug Adapter Protocol server for Sad language
 * 
 * @details يوفر تكامل التصحيح مع VS Code وأدوات أخرى
 * 
 * @author أحمد
 * @date أكتوبر 2026
 */

#ifndef SAD_DAP_SERVER_H
#define SAD_DAP_SERVER_H

#include &lt;string&gt;
#include &lt;vector&gt;
#include &lt;map&gt;
#include &lt;memory&gt;
#include &lt;functional&gt;
#include &lt;optional&gt;
#include &lt;thread&gt;
#include &lt;mutex&gt;
#include &lt;condition_variable&gt;
#include &lt;atomic&gt;
#include &lt;queue&gt;

namespace Sad {
namespace Debug {

//==============================================================================
// أنواع DAP الأساسية
//==============================================================================

/**
 * @brief نقطة توقف
 */
struct Breakpoint {
    int id;
    bool verified;
    std::string source;
    int line;
    int column;
    std::string condition;      // شرط التوقف (اختياري)
    std::string hitCondition;   // عدد مرات التفعيل
    std::string logMessage;     // رسالة للطباعة بدلاً من التوقف
    int hitCount;               // عدد مرات الوصول
    
    Breakpoint() : id(0), verified(false), line(0), column(0), hitCount(0) {}
};

/**
 * @brief إطار المكدس (Stack Frame)
 */
struct StackFrame {
    int id;
    std::string name;           // اسم الدالة
    std::string source;         // ملف المصدر
    int line;
    int column;
    int endLine;
    int endColumn;
    std::string moduleId;       // معرّف الوحدة
    
    StackFrame() : id(0), line(0), column(0), endLine(0), endColumn(0) {}
};

/**
 * @brief نطاق المتغيرات (Scope)
 */
struct Scope {
    std::string name;           // مثل: "محلي"، "عام"، "إغلاق"
    int variablesReference;     // مرجع لجلب المتغيرات
    int namedVariables;         // عدد المتغيرات المسماة
    int indexedVariables;       // عدد المتغيرات المُفهرسة
    bool expensive;             // هل الجلب مكلف؟
    
    Scope() : variablesReference(0), namedVariables(0), indexedVariables(0), expensive(false) {}
};

/**
 * @brief متغير
 */
struct Variable {
    std::string name;
    std::string value;
    std::string type;
    int variablesReference;     // &gt; 0 إذا كان هيكلياً (كائن/مصفوفة)
    int namedVariables;
    int indexedVariables;
    std::string evaluateName;   // التعبير للتقييم
    
    Variable() : variablesReference(0), namedVariables(0), indexedVariables(0) {}
};

/**
 * @brief خيط التنفيذ
 */
struct Thread {
    int id;
    std::string name;
    
    Thread() : id(0) {}
    Thread(int i, const std::string&amp; n) : id(i), name(n) {}
};

/**
 * @brief حالة التصحيح
 */
enum class DebugState {
    STOPPED,        // متوقف (عند breakpoint)
    RUNNING,        // يعمل
    STEPPING,       // يتقدم خطوة
    PAUSED,         // متوقف مؤقتاً
    TERMINATED      // انتهى
};

/**
 * @brief سبب التوقف
 */
enum class StopReason {
    BREAKPOINT,
    STEP,
    EXCEPTION,
    PAUSE,
    ENTRY,
    COMPLETION
};

//==============================================================================
// أحداث DAP
//==============================================================================

/**
 * @brief واجهة مستمع أحداث التصحيح
 */
class DebugEventListener {
public:
    virtual ~DebugEventListener() = default;
    
    // أحداث الحالة
    virtual void onInitialized() = 0;
    virtual void onStopped(int threadId, StopReason reason, const std::string&amp; description) = 0;
    virtual void onContinued(int threadId, bool allThreadsContinued) = 0;
    virtual void onExited(int exitCode) = 0;
    virtual void onTerminated(bool restart) = 0;
    
    // أحداث نقاط التوقف
    virtual void onBreakpointChanged(const Breakpoint&amp; bp) = 0;
    
    // أحداث الإخراج
    virtual void onOutput(const std::string&amp; category, 
                          const std::string&amp; output,
                          const std::string&amp; source = "",
                          int line = 0) = 0;
    
    // أحداث الخيوط
    virtual void onThreadStarted(int threadId) = 0;
    virtual void onThreadExited(int threadId) = 0;
    
    // أحداث الوحدات
    virtual void onModuleLoaded(const std::string&amp; moduleId, 
                                 const std::string&amp; moduleName) = 0;
};

//==============================================================================
// خادم DAP
//==============================================================================

/**
 * @brief خادم بروتوكول محول التصحيح
 */
class DAPServer {
public:
    DAPServer();
    ~DAPServer();
    
    //==========================================================================
    // التهيئة والتشغيل
    //==========================================================================
    
    /**
     * @brief بدء الخادم على stdin/stdout
     */
    void runStdio();
    
    /**
     * @brief بدء الخادم على منفذ TCP
     */
    void runTcp(int port);
    
    /**
     * @brief إيقاف الخادم
     */
    void stop();
    
    /**
     * @brief تسجيل مستمع الأحداث
     */
    void setEventListener(std::shared_ptr&lt;DebugEventListener&gt; listener);
    
    //==========================================================================
    // طلبات DAP
    //==========================================================================
    
    /**
     * @brief معالجة طلب initialize
     */
    void handleInitialize(const std::map&lt;std::string, std::string&gt;&amp; args);
    
    /**
     * @brief معالجة طلب launch
     */
    void handleLaunch(const std::string&amp; program, 
                      const std::vector&lt;std::string&gt;&amp; args,
                      const std::string&amp; cwd,
                      bool stopOnEntry);
    
    /**
     * @brief معالجة طلب attach
     */
    void handleAttach(int processId);
    
    /**
     * @brief معالجة طلب disconnect
     */
    void handleDisconnect(bool terminateDebuggee);
    
    //==========================================================================
    // التحكم في التنفيذ
    //==========================================================================
    
    /**
     * @brief متابعة التنفيذ
     */
    void continue_(int threadId);
    
    /**
     * @brief التقدم خطوة (step over)
     */
    void stepOver(int threadId);
    
    /**
     * @brief الدخول في الدالة (step into)
     */
    void stepInto(int threadId);
    
    /**
     * @brief الخروج من الدالة (step out)
     */
    void stepOut(int threadId);
    
    /**
     * @brief إيقاف مؤقت
     */
    void pause(int threadId);
    
    /**
     * @brief إنهاء التنفيذ
     */
    void terminate();
    
    //==========================================================================
    // نقاط التوقف
    //==========================================================================
    
    /**
     * @brief تعيين نقاط توقف لملف
     */
    std::vector&lt;Breakpoint&gt; setBreakpoints(
        const std::string&amp; source,
        const std::vector&lt;int&gt;&amp; lines,
        const std::vector&lt;std::string&gt;&amp; conditions = {}
    );
    
    /**
     * @brief تعيين نقاط توقف على الدوال
     */
    std::vector&lt;Breakpoint&gt; setFunctionBreakpoints(
        const std::vector&lt;std::string&gt;&amp; functionNames
    );
    
    /**
     * @brief تعيين نقاط توقف على الاستثناءات
     */
    void setExceptionBreakpoints(
        const std::vector&lt;std::string&gt;&amp; filters
    );
    
    //==========================================================================
    // فحص الحالة
    //==========================================================================
    
    /**
     * @brief الحصول على الخيوط
     */
    std::vector&lt;Thread&gt; getThreads();
    
    /**
     * @brief الحصول على مكدس الاستدعاءات
     */
    std::vector&lt;StackFrame&gt; getStackTrace(int threadId, int startFrame = 0, int levels = 20);
    
    /**
     * @brief الحصول على النطاقات
     */
    std::vector&lt;Scope&gt; getScopes(int frameId);
    
    /**
     * @brief الحصول على المتغيرات
     */
    std::vector&lt;Variable&gt; getVariables(int variablesReference, int start = 0, int count = 100);
    
    /**
     * @brief تقييم تعبير
     */
    Variable evaluate(const std::string&amp; expression, int frameId, const std::string&amp; context = "watch");
    
    /**
     * @brief تعيين قيمة متغير
     */
    Variable setVariable(int variablesReference, const std::string&amp; name, const std::string&amp; value);
    
    //==========================================================================
    // المعلومات
    //==========================================================================
    
    /**
     * @brief الحصول على قدرات الخادم
     */
    std::map&lt;std::string, bool&gt; getCapabilities() const;

private:
    class Impl;
    std::unique_ptr&lt;Impl&gt; impl_;
};

//==============================================================================
// محرك التصحيح
//==============================================================================

/**
 * @brief محرك التصحيح للمفسر
 * 
 * @details يربط خادم DAP بالمفسر الفعلي
 */
class DebugEngine {
public:
    DebugEngine();
    ~DebugEngine();
    
    /**
     * @brief تحميل برنامج للتصحيح
     */
    bool loadProgram(const std::string&amp; path);
    
    /**
     * @brief تشغيل البرنامج
     */
    void run();
    
    /**
     * @brief التحقق من حالة التوقف
     */
    bool isStopped() const;
    
    /**
     * @brief الحصول على الحالة الحالية
     */
    DebugState getState() const;
    
    /**
     * @brief تعيين نقطة توقف
     */
    int setBreakpoint(const std::string&amp; file, int line, const std::string&amp; condition = "");
    
    /**
     * @brief إزالة نقطة توقف
     */
    bool removeBreakpoint(int id);
    
    /**
     * @brief الحصول على قيمة متغير
     */
    std::optional&lt;Variable&gt; getVariable(const std::string&amp; name, int frameId);
    
    /**
     * @brief تعيين قيمة متغير
     */
    bool setVariable(const std::string&amp; name, const std::string&amp; value, int frameId);
    
    /**
     * @brief تقييم تعبير
     */
    std::optional&lt;Variable&gt; evaluateExpression(const std::string&amp; expr, int frameId);
    
    /**
     * @brief الحصول على مكدس الاستدعاءات
     */
    std::vector&lt;StackFrame&gt; getCallStack();

private:
    class Impl;
    std::unique_ptr&lt;Impl&gt; impl_;
};

//==============================================================================
// تنسيق رسائل DAP (JSON-RPC)
//==============================================================================

namespace Protocol {
    
/**
 * @brief قراءة رسالة DAP من التيار
 */
std::string readMessage(std::istream&amp; in);

/**
 * @brief كتابة رسالة DAP إلى التيار
 */
void writeMessage(std::ostream&amp; out, const std::string&amp; content);

/**
 * @brief تحليل طلب DAP
 */
std::map&lt;std::string, std::string&gt; parseRequest(const std::string&amp; json);

/**
 * @brief بناء استجابة DAP
 */
std::string buildResponse(int requestSeq, bool success, 
                           const std::string&amp; command,
                           const std::string&amp; body = "{}");

/**
 * @brief بناء حدث DAP
 */
std::string buildEvent(const std::string&amp; event, const std::string&amp; body = "{}");

} // namespace Protocol

} // namespace Debug
} // namespace Sad

#endif // SAD_DAP_SERVER_H
