/**
 * @file dap_server.cpp
 * @brief تنفيذ خادم DAP
 * @author أحمد
 * @date أكتوبر 2026
 */

#include "dap_server.h"
#include &lt;iostream&gt;
#include &lt;sstream&gt;
#include &lt;regex&gt;
#include &lt;chrono&gt;

namespace Sad {
namespace Debug {

//==============================================================================
// Protocol Implementation
//==============================================================================

namespace Protocol {

std::string readMessage(std::istream&amp; in) {
    std::string line;
    int contentLength = 0;
    
    // قراءة الرؤوس
    while (std::getline(in, line)) {
        // إزالة \r إذا وجد
        if (!line.empty() &amp;&amp; line.back() == '\r') {
            line.pop_back();
        }
        
        if (line.empty()) {
            break; // نهاية الرؤوس
        }
        
        // Content-Length header
        if (line.find("Content-Length:") == 0) {
            contentLength = std::stoi(line.substr(15));
        }
    }
    
    if (contentLength &lt;= 0) {
        return "";
    }
    
    // قراءة المحتوى
    std::string content(contentLength, '\0');
    in.read(&amp;content[0], contentLength);
    
    return content;
}

void writeMessage(std::ostream&amp; out, const std::string&amp; content) {
    out &lt;&lt; "Content-Length: " &lt;&lt; content.length() &lt;&lt; "\r\n";
    out &lt;&lt; "\r\n";
    out &lt;&lt; content;
    out.flush();
}

std::string buildResponse(int requestSeq, bool success, 
                           const std::string&amp; command,
                           const std::string&amp; body) {
    std::ostringstream ss;
    ss &lt;&lt; "{"
       &lt;&lt; "\"seq\":0,"
       &lt;&lt; "\"type\":\"response\","
       &lt;&lt; "\"request_seq\":" &lt;&lt; requestSeq &lt;&lt; ","
       &lt;&lt; "\"success\":" &lt;&lt; (success ? "true" : "false") &lt;&lt; ","
       &lt;&lt; "\"command\":\"" &lt;&lt; command &lt;&lt; "\","
       &lt;&lt; "\"body\":" &lt;&lt; body
       &lt;&lt; "}";
    return ss.str();
}

std::string buildEvent(const std::string&amp; event, const std::string&amp; body) {
    std::ostringstream ss;
    ss &lt;&lt; "{"
       &lt;&lt; "\"seq\":0,"
       &lt;&lt; "\"type\":\"event\","
       &lt;&lt; "\"event\":\"" &lt;&lt; event &lt;&lt; "\","
       &lt;&lt; "\"body\":" &lt;&lt; body
       &lt;&lt; "}";
    return ss.str();
}

} // namespace Protocol

//==============================================================================
// DAPServer Implementation
//==============================================================================

class DAPServer::Impl {
public:
    std::shared_ptr&lt;DebugEventListener&gt; listener_;
    std::unique_ptr&lt;DebugEngine&gt; engine_;
    std::atomic&lt;bool&gt; running_{false};
    std::map&lt;std::string, std::vector&lt;Breakpoint&gt;&gt; breakpoints_;
    int nextBreakpointId_ = 1;
    int nextVariablesRef_ = 1;
    std::mutex mutex_;
    
    std::map&lt;std::string, bool&gt; capabilities_ = {
        {"supportsConfigurationDoneRequest", true},
        {"supportsFunctionBreakpoints", true},
        {"supportsConditionalBreakpoints", true},
        {"supportsHitConditionalBreakpoints", true},
        {"supportsEvaluateForHovers", true},
        {"supportsStepBack", false},
        {"supportsSetVariable", true},
        {"supportsRestartFrame", false},
        {"supportsGotoTargetsRequest", false},
        {"supportsStepInTargetsRequest", false},
        {"supportsCompletionsRequest", true},
        {"supportsModulesRequest", true},
        {"supportsExceptionOptions", true},
        {"supportsValueFormattingOptions", true},
        {"supportsExceptionInfoRequest", true},
        {"supportTerminateDebuggee", true},
        {"supportsDelayedStackTraceLoading", true},
        {"supportsLoadedSourcesRequest", true},
        {"supportsLogPoints", true},
        {"supportsTerminateThreadsRequest", false},
        {"supportsSetExpression", false},
        {"supportsTerminateRequest", true},
        {"supportsDataBreakpoints", false},
        {"supportsReadMemoryRequest", false},
        {"supportsDisassembleRequest", false},
        {"supportsCancelRequest", false},
        {"supportsBreakpointLocationsRequest", true},
        {"supportsClipboardContext", false}
    };
};

DAPServer::DAPServer() : impl_(std::make_unique&lt;Impl&gt;()) {
    impl_-&gt;engine_ = std::make_unique&lt;DebugEngine&gt;();
}

DAPServer::~DAPServer() {
    stop();
}

void DAPServer::runStdio() {
    impl_-&gt;running_ = true;
    
    while (impl_-&gt;running_) {
        std::string message = Protocol::readMessage(std::cin);
        if (message.empty()) {
            continue;
        }
        
        // معالجة الرسالة (تبسيط - يحتاج parser JSON حقيقي)
        // هنا نستخدم regex بسيط للتوضيح
        
        std::regex cmdRegex("\"command\"\\s*:\\s*\"([^\"]+)\"");
        std::smatch match;
        
        if (std::regex_search(message, match, cmdRegex)) {
            std::string command = match[1];
            
            if (command == "initialize") {
                handleInitialize({});
            } else if (command == "launch") {
                // استخراج المسار من الرسالة
                std::regex progRegex("\"program\"\\s*:\\s*\"([^\"]+)\"");
                std::smatch progMatch;
                if (std::regex_search(message, progMatch, progRegex)) {
                    handleLaunch(progMatch[1], {}, ".", false);
                }
            } else if (command == "disconnect") {
                handleDisconnect(true);
                break;
            } else if (command == "continue") {
                continue_(1);
            } else if (command == "next") {
                stepOver(1);
            } else if (command == "stepIn") {
                stepInto(1);
            } else if (command == "stepOut") {
                stepOut(1);
            }
        }
    }
}

void DAPServer::runTcp(int port) {
    // TODO: تنفيذ خادم TCP
    (void)port;
}

void DAPServer::stop() {
    impl_-&gt;running_ = false;
}

void DAPServer::setEventListener(std::shared_ptr&lt;DebugEventListener&gt; listener) {
    impl_-&gt;listener_ = listener;
}

void DAPServer::handleInitialize(const std::map&lt;std::string, std::string&gt;&amp; args) {
    (void)args;
    
    // بناء استجابة القدرات
    std::ostringstream body;
    body &lt;&lt; "{";
    bool first = true;
    for (const auto&amp; [key, value] : impl_-&gt;capabilities_) {
        if (!first) body &lt;&lt; ",";
        body &lt;&lt; "\"" &lt;&lt; key &lt;&lt; "\":" &lt;&lt; (value ? "true" : "false");
        first = false;
    }
    body &lt;&lt; "}";
    
    std::string response = Protocol::buildResponse(1, true, "initialize", body.str());
    Protocol::writeMessage(std::cout, response);
    
    // إرسال حدث initialized
    std::string event = Protocol::buildEvent("initialized", "{}");
    Protocol::writeMessage(std::cout, event);
    
    if (impl_-&gt;listener_) {
        impl_-&gt;listener_-&gt;onInitialized();
    }
}

void DAPServer::handleLaunch(const std::string&amp; program, 
                              const std::vector&lt;std::string&gt;&amp; args,
                              const std::string&amp; cwd,
                              bool stopOnEntry) {
    (void)args;
    (void)cwd;
    
    if (impl_-&gt;engine_-&gt;loadProgram(program)) {
        std::string response = Protocol::buildResponse(2, true, "launch", "{}");
        Protocol::writeMessage(std::cout, response);
        
        if (stopOnEntry) {
            if (impl_-&gt;listener_) {
                impl_-&gt;listener_-&gt;onStopped(1, StopReason::ENTRY, "نقطة الدخول");
            }
        } else {
            impl_-&gt;engine_-&gt;run();
        }
    } else {
        std::string response = Protocol::buildResponse(2, false, "launch", 
            "{\"error\":{\"id\":1,\"format\":\"فشل تحميل البرنامج\"}}");
        Protocol::writeMessage(std::cout, response);
    }
}

void DAPServer::handleAttach(int processId) {
    (void)processId;
    // TODO: تنفيذ الربط بعملية موجودة
}

void DAPServer::handleDisconnect(bool terminateDebuggee) {
    if (terminateDebuggee) {
        terminate();
    }
    
    std::string response = Protocol::buildResponse(99, true, "disconnect", "{}");
    Protocol::writeMessage(std::cout, response);
    
    if (impl_-&gt;listener_) {
        impl_-&gt;listener_-&gt;onTerminated(false);
    }
}

void DAPServer::continue_(int threadId) {
    (void)threadId;
    impl_-&gt;engine_-&gt;run();
    
    std::string response = Protocol::buildResponse(10, true, "continue", 
        "{\"allThreadsContinued\":true}");
    Protocol::writeMessage(std::cout, response);
    
    if (impl_-&gt;listener_) {
        impl_-&gt;listener_-&gt;onContinued(threadId, true);
    }
}

void DAPServer::stepOver(int threadId) {
    (void)threadId;
    // TODO: تنفيذ step over
}

void DAPServer::stepInto(int threadId) {
    (void)threadId;
    // TODO: تنفيذ step into
}

void DAPServer::stepOut(int threadId) {
    (void)threadId;
    // TODO: تنفيذ step out
}

void DAPServer::pause(int threadId) {
    (void)threadId;
    // TODO: تنفيذ pause
}

void DAPServer::terminate() {
    // إنهاء محرك التصحيح
    impl_-&gt;running_ = false;
    
    if (impl_-&gt;listener_) {
        impl_-&gt;listener_-&gt;onExited(0);
        impl_-&gt;listener_-&gt;onTerminated(false);
    }
}

std::vector&lt;Breakpoint&gt; DAPServer::setBreakpoints(
    const std::string&amp; source,
    const std::vector&lt;int&gt;&amp; lines,
    const std::vector&lt;std::string&gt;&amp; conditions) {
    
    std::lock_guard&lt;std::mutex&gt; lock(impl_-&gt;mutex_);
    
    // إزالة نقاط التوقف القديمة لهذا الملف
    impl_-&gt;breakpoints_[source].clear();
    
    std::vector&lt;Breakpoint&gt; result;
    
    for (size_t i = 0; i &lt; lines.size(); ++i) {
        Breakpoint bp;
        bp.id = impl_-&gt;nextBreakpointId_++;
        bp.verified = true;
        bp.source = source;
        bp.line = lines[i];
        bp.column = 1;
        
        if (i &lt; conditions.size()) {
            bp.condition = conditions[i];
        }
        
        impl_-&gt;breakpoints_[source].push_back(bp);
        impl_-&gt;engine_-&gt;setBreakpoint(source, lines[i], bp.condition);
        
        result.push_back(bp);
        
        if (impl_-&gt;listener_) {
            impl_-&gt;listener_-&gt;onBreakpointChanged(bp);
        }
    }
    
    return result;
}

std::vector&lt;Breakpoint&gt; DAPServer::setFunctionBreakpoints(
    const std::vector&lt;std::string&gt;&amp; functionNames) {
    
    std::vector&lt;Breakpoint&gt; result;
    
    for (const auto&amp; func : functionNames) {
        Breakpoint bp;
        bp.id = impl_-&gt;nextBreakpointId_++;
        bp.verified = false; // ننتظر حتى يتم العثور على الدالة
        bp.source = func;
        bp.line = 0;
        
        result.push_back(bp);
    }
    
    return result;
}

void DAPServer::setExceptionBreakpoints(const std::vector&lt;std::string&gt;&amp; filters) {
    (void)filters;
    // TODO: تنفيذ نقاط توقف الاستثناءات
}

std::vector&lt;Thread&gt; DAPServer::getThreads() {
    // لغة ص حالياً تدعم خيط واحد
    return {Thread(1, "الخيط الرئيسي")};
}

std::vector&lt;StackFrame&gt; DAPServer::getStackTrace(int threadId, int startFrame, int levels) {
    (void)threadId;
    (void)startFrame;
    (void)levels;
    
    return impl_-&gt;engine_-&gt;getCallStack();
}

std::vector&lt;Scope&gt; DAPServer::getScopes(int frameId) {
    std::vector&lt;Scope&gt; scopes;
    
    Scope local;
    local.name = "محلي";
    local.variablesReference = frameId * 1000 + 1;
    local.expensive = false;
    scopes.push_back(local);
    
    Scope global;
    global.name = "عام";
    global.variablesReference = frameId * 1000 + 2;
    global.expensive = false;
    scopes.push_back(global);
    
    return scopes;
}

std::vector&lt;Variable&gt; DAPServer::getVariables(int variablesReference, int start, int count) {
    (void)variablesReference;
    (void)start;
    (void)count;
    
    // TODO: جلب المتغيرات من محرك التصحيح
    return {};
}

Variable DAPServer::evaluate(const std::string&amp; expression, int frameId, const std::string&amp; context) {
    (void)context;
    
    auto result = impl_-&gt;engine_-&gt;evaluateExpression(expression, frameId);
    if (result.has_value()) {
        return result.value();
    }
    
    Variable var;
    var.name = expression;
    var.value = "خطأ في التقييم";
    var.type = "خطأ";
    return var;
}

Variable DAPServer::setVariable(int variablesReference, const std::string&amp; name, const std::string&amp; value) {
    (void)variablesReference;
    
    Variable var;
    var.name = name;
    var.value = value;
    
    return var;
}

std::map&lt;std::string, bool&gt; DAPServer::getCapabilities() const {
    return impl_-&gt;capabilities_;
}

//==============================================================================
// DebugEngine Implementation
//==============================================================================

class DebugEngine::Impl {
public:
    std::string programPath_;
    DebugState state_ = DebugState::STOPPED;
    std::vector&lt;StackFrame&gt; callStack_;
    std::map&lt;int, Breakpoint&gt; breakpoints_;
    int nextBreakpointId_ = 1;
};

DebugEngine::DebugEngine() : impl_(std::make_unique&lt;Impl&gt;()) {}

DebugEngine::~DebugEngine() = default;

bool DebugEngine::loadProgram(const std::string&amp; path) {
    impl_-&gt;programPath_ = path;
    impl_-&gt;state_ = DebugState::STOPPED;
    
    // TODO: تحميل البرنامج فعلياً
    // هنا يتم الربط بالمفسر
    
    // إنشاء إطار مكدس وهمي للاختبار
    StackFrame frame;
    frame.id = 1;
    frame.name = "رئيسية";
    frame.source = path;
    frame.line = 1;
    frame.column = 1;
    impl_-&gt;callStack_.push_back(frame);
    
    return true;
}

void DebugEngine::run() {
    impl_-&gt;state_ = DebugState::RUNNING;
    // TODO: تشغيل المفسر
}

bool DebugEngine::isStopped() const {
    return impl_-&gt;state_ == DebugState::STOPPED || impl_-&gt;state_ == DebugState::PAUSED;
}

DebugState DebugEngine::getState() const {
    return impl_-&gt;state_;
}

int DebugEngine::setBreakpoint(const std::string&amp; file, int line, const std::string&amp; condition) {
    Breakpoint bp;
    bp.id = impl_-&gt;nextBreakpointId_++;
    bp.verified = true;
    bp.source = file;
    bp.line = line;
    bp.condition = condition;
    
    impl_-&gt;breakpoints_[bp.id] = bp;
    
    return bp.id;
}

bool DebugEngine::removeBreakpoint(int id) {
    return impl_-&gt;breakpoints_.erase(id) &gt; 0;
}

std::optional&lt;Variable&gt; DebugEngine::getVariable(const std::string&amp; name, int frameId) {
    (void)name;
    (void)frameId;
    // TODO: جلب المتغير من المفسر
    return std::nullopt;
}

bool DebugEngine::setVariable(const std::string&amp; name, const std::string&amp; value, int frameId) {
    (void)name;
    (void)value;
    (void)frameId;
    // TODO: تعيين قيمة المتغير في المفسر
    return false;
}

std::optional&lt;Variable&gt; DebugEngine::evaluateExpression(const std::string&amp; expr, int frameId) {
    (void)expr;
    (void)frameId;
    // TODO: تقييم التعبير في المفسر
    return std::nullopt;
}

std::vector&lt;StackFrame&gt; DebugEngine::getCallStack() {
    return impl_-&gt;callStack_;
}

} // namespace Debug
} // namespace Sad
