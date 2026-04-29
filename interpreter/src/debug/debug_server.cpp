/**
 * @file debug_server.cpp
 * @brief (AR) تنفيذ خادم التصحيح — بروتوكول JSON عبر stdin/stdout
 * @brief (EN) Debug Server implementation — JSON protocol over stdin/stdout
 *
 * @author فريق لغة ص
 * @date 2025
 */

#include <string>
#include "debug_server.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "statements.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <regex>
#include "sad_type_system.h"
// (AR) تحويل آمن مع كشف الفيض (size_t→int) — يجب أن يكون خارج #ifdef _WIN32
//      لأن الكود الذي يستخدمه يُجمَّع على كل المنصات.
// (EN) Bounds-checked size_t→int — must live outside #ifdef _WIN32 because
//      the call sites are compiled on all platforms.
#include "safe_arithmetic.h"

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#endif

namespace Sad
{
    namespace Debug
    {

        // ═══════════════════════════════════════════════════════════════════
        // Singleton
        // ═══════════════════════════════════════════════════════════════════
        DebugServer *DebugServer::instance_ = nullptr;

        DebugServer *DebugServer::getInstance() { return instance_; }
        void DebugServer::setInstance(DebugServer *instance) { instance_ = instance; }

        // ═══════════════════════════════════════════════════════════════════
        // Construction / Destruction
        // ═══════════════════════════════════════════════════════════════════
        DebugServer::DebugServer()
        {
            setInstance(this);
        }

        DebugServer::~DebugServer()
        {
            shouldDisconnect_ = true;
            connected_ = false;
            cv_.notify_all();
            if (readerThread_.joinable())
            {
                readerThread_.join();
            }
            if (instance_ == this)
            {
                instance_ = nullptr;
            }
        }

        // ═══════════════════════════════════════════════════════════════════
        // I/O — stdin/stdout JSON Lines
        // ═══════════════════════════════════════════════════════════════════

        std::string DebugServer::readLine()
        {
            std::string line;
            if (!std::getline(std::cin, line))
            {
                shouldDisconnect_ = true;
                return "";
            }
            // (AR) إزالة \r في حالة Windows
            if (!line.empty() && line.back() == '\r')
            {
                line.pop_back();
            }
            return line;
        }

        void DebugServer::writeLine(const std::string &line)
        {
            // (AR) كتابة سطر JSON واحد + سطر جديد إلى stdout
            // (EN) Write single JSON line + newline to stdout
            std::cout << line << "\n"
                      << std::flush;
        }

        void DebugServer::sendEvent(const std::string &event, const std::string &bodyJson)
        {
            std::string msg = "{\"type\":\"event\",\"event\":\"" + event + "\",\"body\":" + bodyJson + "}";
            writeLine(msg);
        }

        void DebugServer::sendResponse(int requestSeq, const std::string &command, const std::string &bodyJson)
        {
            std::string msg = "{\"type\":\"response\",\"request_seq\":" + std::to_string(requestSeq) +
                              ",\"success\":true,\"command\":\"" + command + "\",\"body\":" + bodyJson + "}";
            writeLine(msg);
        }

        void DebugServer::sendErrorResponse(int requestSeq, const std::string &command, const std::string &message)
        {
            std::string msg = "{\"type\":\"response\",\"request_seq\":" + std::to_string(requestSeq) +
                              ",\"success\":false,\"command\":\"" + command +
                              "\",\"message\":\"" + escapeJson(message) + "\"}";
            writeLine(msg);
        }

        // ═══════════════════════════════════════════════════════════════════
        // JSON Helpers (minimal — no external dependency)
        // ═══════════════════════════════════════════════════════════════════

        std::string DebugServer::escapeJson(const std::string &s)
        {
            std::string out;
            out.reserve(s.size() + 16);
            for (char c : s)
            {
                switch (c)
                {
                case '"':
                    out += "\\\"";
                    break;
                case '\\':
                    out += "\\\\";
                    break;
                case '\n':
                    out += "\\n";
                    break;
                case '\r':
                    out += "\\r";
                    break;
                case '\t':
                    out += "\\t";
                    break;
                default:
                    out += c;
                    break;
                }
            }
            return out;
        }

        std::string DebugServer::jsonPair(const std::string &key, const std::string &value)
        {
            return "\"" + key + "\":\"" + escapeJson(value) + "\"";
        }

        std::string DebugServer::jsonPairNum(const std::string &key, int value)
        {
            return "\"" + key + "\":" + std::to_string(value);
        }

        std::string DebugServer::jsonPairBool(const std::string &key, bool value)
        {
            return "\"" + key + "\":" + (value ? "true" : "false");
        }

        int DebugServer::extractIntField(const std::string &json, const std::string &field)
        {
            try
            {
                std::string pattern = "\"" + field + "\":";
                auto pos = json.find(pattern);
                if (pos == std::string::npos)
                    return -1;
                pos += pattern.size();
                // skip whitespace
                while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t'))
                    pos++;
                std::string num;
                while (pos < json.size() && (json[pos] >= '0' && json[pos] <= '9'))
                {
                    num += json[pos++];
                }
                return num.empty() ? -1 : std::stoi(num);
            }
            catch (const std::exception &)
            {
                return -1;
            }
        }

        std::string DebugServer::extractStringField(const std::string &json, const std::string &field)
        {
            std::string pattern = "\"" + field + "\":\"";
            auto pos = json.find(pattern);
            if (pos == std::string::npos)
                return "";
            pos += pattern.size();
            std::string result;
            bool escaped = false;
            while (pos < json.size())
            {
                char c = json[pos++];
                if (escaped)
                {
                    if (c == 'n')
                        result += '\n';
                    else if (c == 't')
                        result += '\t';
                    else if (c == '\\')
                        result += '\\';
                    else if (c == '"')
                        result += '"';
                    else
                        result += c;
                    escaped = false;
                }
                else if (c == '\\')
                {
                    escaped = true;
                }
                else if (c == '"')
                {
                    break;
                }
                else
                {
                    result += c;
                }
            }
            return result;
        }

        std::vector<int> DebugServer::extractIntArray(const std::string &json, const std::string &field)
        {
            std::vector<int> result;
            try
            {
                std::string pattern = "\"" + field + "\":[";
                auto pos = json.find(pattern);
                if (pos == std::string::npos)
                    return result;
                pos += pattern.size();
                std::string num;
                while (pos < json.size() && json[pos] != ']')
                {
                    char c = json[pos++];
                    if (c >= '0' && c <= '9')
                    {
                        num += c;
                    }
                    else if (!num.empty())
                    {
                        result.push_back(std::stoi(num));
                        num.clear();
                    }
                }
                if (!num.empty())
                {
                    result.push_back(std::stoi(num));
                }
            }
            catch (const std::exception &)
            {
                // (AR) تجاهل أخطاء التحويل / (EN) Ignore conversion errors
            }
            return result;
        }

        // ═══════════════════════════════════════════════════════════════════
        // Interpreter References
        // ═══════════════════════════════════════════════════════════════════

        void DebugServer::setInterpreterRefs(Data::VariableManager *varMgr,
                                             Data::FunctionManager *funcMgr,
                                             Data::ScopeManager *scopeMgr)
        {
            varMgr_ = varMgr;
            funcMgr_ = funcMgr;
            scopeMgr_ = scopeMgr;
        }

        // ═══════════════════════════════════════════════════════════════════
        // Run — Main loop
        // ═══════════════════════════════════════════════════════════════════

        void DebugServer::run(const std::string &sourceFile)
        {
            sourceFile_ = sourceFile;

            // (AR) قراءة أسطر الملف المصدري
            // (EN) Read source file lines
            {
                std::ifstream f(sourceFile);
                if (f.is_open())
                {
                    std::string line;
                    while (std::getline(f, line))
                    {
                        sourceLines_.push_back(line);
                    }
                }
            }

            connected_ = true;

            // (AR) إرسال حدث التهيئة
            // (EN) Send initialized event
            sendEvent("initialized");

            // (AR) انتظار أمر "configurationDone" أو "launch" من المحول
            // (EN) Wait for "configurationDone" or "launch" from adapter
            // (AR) خيط القراءة يعمل في الخلفية
            // (EN) Reader thread runs in background
            readerThread_ = std::thread(&DebugServer::readerLoop, this);
        }

        void DebugServer::readerLoop()
        {
            while (connected_ && !shouldDisconnect_)
            {
                std::string line = readLine();
                if (line.empty() && shouldDisconnect_)
                    break;
                if (!line.empty())
                {
                    processCommand(line);
                }
            }
        }

        // ═══════════════════════════════════════════════════════════════════
        // Command Processing
        // ═══════════════════════════════════════════════════════════════════

        void DebugServer::processCommand(const std::string &jsonLine)
        {
            // (AR) استخراج الحقول الأساسية
            std::string command = extractStringField(jsonLine, "command");
            int seq = extractIntField(jsonLine, "seq");

            if (command == "setBreakpoints")
            {
                handleSetBreakpoints(seq, jsonLine);
            }
            else if (command == "continue")
            {
                handleContinue(seq);
            }
            else if (command == "next")
            {
                handleNext(seq);
            }
            else if (command == "stepIn")
            {
                handleStepIn(seq);
            }
            else if (command == "stepOut")
            {
                handleStepOut(seq);
            }
            else if (command == "pause")
            {
                handlePause(seq);
            }
            else if (command == "stackTrace")
            {
                handleStackTrace(seq);
            }
            else if (command == "scopes")
            {
                handleScopes(seq, jsonLine);
            }
            else if (command == "variables")
            {
                handleVariables(seq, jsonLine);
            }
            else if (command == "evaluate")
            {
                handleEvaluate(seq, jsonLine);
            }
            else if (command == "disconnect")
            {
                handleDisconnect(seq);
            }
            else if (command == "initialize")
            {
                // (AR) رد بالقدرات
                sendResponse(seq, "initialize",
                             "{\"supportsConfigurationDoneRequest\":true,"
                             "\"supportsSetVariable\":false,"
                             "\"supportsConditionalBreakpoints\":true,"
                             "\"supportsHitConditionalBreakpoints\":true,"
                             "\"supportsEvaluateForHovers\":true,"
                             "\"supportsStepBack\":false,"
                             "\"supportsRestartFrame\":false,"
                             "\"supportsExceptionInfoRequest\":true,"
                             "\"supportTerminateDebuggee\":true,"
                             "\"supportsDelayedStackTraceLoading\":false}");
            }
            else if (command == "launch" || command == "attach")
            {
                sendResponse(seq, command);
                // (AR) الآن جاهز للتنفيذ
            }
            else if (command == "configurationDone")
            {
                sendResponse(seq, "configurationDone");
            }
            else if (command == "threads")
            {
                // (AR) خيط واحد فقط
                sendResponse(seq, "threads",
                             "{\"threads\":[{\"id\":1,\"name\":\"الرئيسي / Main Thread\"}]}");
            }
            else
            {
                sendErrorResponse(seq, command, "أمر غير مدعوم / Unsupported command: " + command);
            }
        }

        // ═══════════════════════════════════════════════════════════════════
        // Breakpoints
        // ═══════════════════════════════════════════════════════════════════

        void DebugServer::handleSetBreakpoints(int seq, const std::string &json)
        {
            std::string file = extractStringField(json, "path");
            if (file.empty())
            {
                file = extractStringField(json, "name");
            }

            // (AR) مسح نقاط التوقف القديمة لهذا الملف
            breakpoints_[file].clear();

            // (AR) استخراج أسطر نقاط التوقف
            std::vector<int> lines = extractIntArray(json, "lines");

            // (AR) إذا لم نجد "lines" مباشرة، نبحث في "breakpoints" array
            if (lines.empty())
            {
                // (AR) بحث يدوي عن "breakpoints":[{"line":N}, ...]
                std::string bpPattern = "\"breakpoints\":[";
                auto pos = json.find(bpPattern);
                if (pos != std::string::npos)
                {
                    pos += bpPattern.size();
                    std::string bpSection = json.substr(pos);
                    // استخراج كل "line":N
                    std::string linePattern = "\"line\":";
                    size_t searchPos = 0;
                    while ((searchPos = bpSection.find(linePattern, searchPos)) != std::string::npos)
                    {
                        searchPos += linePattern.size();
                        std::string num;
                        while (searchPos < bpSection.size() && bpSection[searchPos] >= '0' && bpSection[searchPos] <= '9')
                        {
                            num += bpSection[searchPos++];
                        }
                        if (!num.empty())
                        {
                            lines.push_back(std::stoi(num));
                        }
                    }
                }
            }

            // (AR) إنشاء نقاط التوقف
            std::string bpArray = "[";
            for (size_t i = 0; i < lines.size(); i++)
            {
                DebugBreakpoint bp;
                bp.id = nextBreakpointId_++;
                bp.file = file;
                bp.line = lines[i];
                bp.enabled = true;
                bp.verified = true;
                breakpoints_[file].push_back(bp);

                if (i > 0)
                    bpArray += ",";
                bpArray += "{" + jsonPairNum("id", bp.id) + "," +
                           jsonPairBool("verified", true) + "," +
                           jsonPairNum("line", bp.line) + "}";
            }
            bpArray += "]";

            sendResponse(seq, "setBreakpoints", "{\"breakpoints\":" + bpArray + "}");
        }

        // ═══════════════════════════════════════════════════════════════════
        // Execution Control
        // ═══════════════════════════════════════════════════════════════════

        void DebugServer::handleContinue(int seq)
        {
            sendResponse(seq, "continue", "{\"allThreadsContinued\":true}");
            {
                std::lock_guard<std::mutex> lock(mutex_);
                pendingAction_ = DebugAction::CONTINUE;
                stepping_ = false;
                stepOverDepth_ = -1;
                stepOutDepth_ = -1;
            }
            cv_.notify_all();
        }

        void DebugServer::handleNext(int seq)
        {
            sendResponse(seq, "next");
            {
                std::lock_guard<std::mutex> lock(mutex_);
                pendingAction_ = DebugAction::STEP_OVER;
                stepping_ = true;
                stepOverDepth_ = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(callStack_.size(), "debug_server_size");
            }
            cv_.notify_all();
        }

        void DebugServer::handleStepIn(int seq)
        {
            sendResponse(seq, "stepIn");
            {
                std::lock_guard<std::mutex> lock(mutex_);
                pendingAction_ = DebugAction::STEP_IN;
                stepping_ = true;
                stepOverDepth_ = -1;
                stepOutDepth_ = -1;
            }
            cv_.notify_all();
        }

        void DebugServer::handleStepOut(int seq)
        {
            sendResponse(seq, "stepOut");
            {
                std::lock_guard<std::mutex> lock(mutex_);
                pendingAction_ = DebugAction::STEP_OUT;
                stepping_ = true;
                stepOutDepth_ = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(callStack_.size(), "debug_server_size") - 1;
            }
            cv_.notify_all();
        }

        void DebugServer::handlePause(int seq)
        {
            sendResponse(seq, "pause");
            {
                std::lock_guard<std::mutex> lock(mutex_);
                pendingAction_ = DebugAction::PAUSE;
            }
        }

        void DebugServer::handleDisconnect(int seq)
        {
            sendResponse(seq, "disconnect");
            shouldDisconnect_ = true;
            connected_ = false;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                pendingAction_ = DebugAction::DISCONNECT;
            }
            cv_.notify_all();
        }

        // ═══════════════════════════════════════════════════════════════════
        // Stack Trace
        // ═══════════════════════════════════════════════════════════════════

        void DebugServer::handleStackTrace(int seq)
        {
            auto frames = collectStackFrames();
            std::string framesJson = "[";
            for (size_t i = 0; i < frames.size(); i++)
            {
                if (i > 0)
                    framesJson += ",";
                framesJson += "{" +
                              jsonPairNum("id", frames[i].id) + "," +
                              jsonPair("name", frames[i].name) + "," +
                              "\"source\":{" + jsonPair("path", frames[i].file) + "}," +
                              jsonPairNum("line", frames[i].line) + "," +
                              jsonPairNum("column", frames[i].column) +
                              "}";
            }
            framesJson += "]";

            sendResponse(seq, "stackTrace",
                         "{\"stackFrames\":" + framesJson + "," +
                             jsonPairNum("totalFrames", ::Sad::Security::SafeArithmetic::assertSafeCast<int>(frames.size(), "debug_server_size")) + "}");
        }

        std::vector<DebugStackFrame> DebugServer::collectStackFrames()
        {
            std::vector<DebugStackFrame> frames;

            // (AR) الإطار الحالي (أعلى المكدس)
            DebugStackFrame top;
            top.id = 0;
            top.name = callStack_.empty() ? "<\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A>" : callStack_.back().functionName; // <رئيسي>
            top.file = currentFile_;
            top.line = currentLine_;
            top.column = 1;
            frames.push_back(top);

            // (AR) إطارات الاستدعاء السابقة
            for (int i = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(callStack_.size(), "debug_server_size") - 1; i >= 0; i--)
            {
                // (AR) تخطي الإطار الأخير (مُدرج كإطار أعلى)
                if (i == ::Sad::Security::SafeArithmetic::assertSafeCast<int>(callStack_.size(), "debug_server_size") - 1 && !callStack_.empty())
                    continue;

                DebugStackFrame sf;
                sf.id = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(frames.size(), "debug_server_size");
                sf.name = callStack_[i].functionName;
                sf.file = callStack_[i].file;
                sf.line = callStack_[i].line;
                sf.column = 1;
                frames.push_back(sf);
            }

            return frames;
        }

        // ═══════════════════════════════════════════════════════════════════
        // Scopes & Variables
        // ═══════════════════════════════════════════════════════════════════

        void DebugServer::handleScopes(int seq, const std::string &json)
        {
            int frameId = extractIntField(json, "frameId");
            (void)frameId; // (AR) نستخدم frameId لاحقاً لتحديد النطاق الصحيح

            // (AR) ثلاثة نطاقات: محلي، عام، ثوابت
            std::string scopes = "[";
            scopes += "{\"name\":\"\xD9\x85\xD8\xAD\xD9\x84\xD9\x8A / Local\"," // محلي
                      "\"variablesReference\":1,"
                      "\"expensive\":false}";
            scopes += ",{\"name\":\"\xD8\xB9\xD8\xA7\xD9\x85 / Global\"," // عام
                      "\"variablesReference\":2,"
                      "\"expensive\":false}";
            scopes += "]";

            sendResponse(seq, "scopes", "{\"scopes\":" + scopes + "}");
        }

        void DebugServer::handleVariables(int seq, const std::string &json)
        {
            int ref = extractIntField(json, "variablesReference");
            auto vars = collectVariables(ref);

            std::string varsJson = "[";
            for (size_t i = 0; i < vars.size(); i++)
            {
                if (i > 0)
                    varsJson += ",";
                varsJson += "{" +
                            jsonPair("name", vars[i].name) + "," +
                            jsonPair("value", vars[i].value) + "," +
                            jsonPair("type", vars[i].type) + "," +
                            jsonPairNum("variablesReference", vars[i].variablesReference) +
                            "}";
            }
            varsJson += "]";

            sendResponse(seq, "variables", "{\"variables\":" + varsJson + "}");
        }

        std::vector<DebugVariable> DebugServer::collectVariables(int scopeRef)
        {
            std::vector<DebugVariable> vars;
            if (!varMgr_ || !scopeMgr_)
                return vars;

            if (scopeRef == 1)
            {
                // (AR) المتغيرات المحلية — النطاق الحالي
                auto names = varMgr_->getVariableNames();
                for (const auto &name : names)
                {
                    const Data::Value *val = varMgr_->tryGet(name);
                    if (val)
                    {
                        DebugVariable dv;
                        dv.name = name;
                        dv.value = val->toString();
                        dv.type = val->getTypeName();
                        dv.variablesReference = 0;

                        // (AR) للكائنات والمصفوفات والخرائط، يمكن أن يكون لها فروع
                        if (val->getKind() == Types::SadTypeKind::Array)
                        {
                            dv.variablesReference = 100 + ::Sad::Security::SafeArithmetic::assertSafeCast<int>(vars.size(), "debug_server_size");
                        }
                        else if (val->getKind() == Types::SadTypeKind::Map)
                        {
                            dv.variablesReference = 200 + ::Sad::Security::SafeArithmetic::assertSafeCast<int>(vars.size(), "debug_server_size");
                        }
                        else if (val->getKind() == Types::SadTypeKind::Class)
                        {
                            dv.variablesReference = 300 + ::Sad::Security::SafeArithmetic::assertSafeCast<int>(vars.size(), "debug_server_size");
                        }

                        vars.push_back(dv);
                    }
                }
            }
            else if (scopeRef == 2)
            {
                // (AR) المتغيرات العامة
                // (EN) Global variables — walk to global scope
                auto *globalScope = scopeMgr_->getGlobalScope();
                auto *currentScope = scopeMgr_->getCurrentScope();

                // (AR) نبحث في النطاق العام مباشرة
                // (EN) We look in global scope directly
                // (AR) VariableManager يبحث في سلسلة النطاقات تلقائياً
                // نعرض المتغيرات من النطاق الحالي وسلسلته
                auto names = varMgr_->getVariableNames();
                for (const auto &name : names)
                {
                    const Data::Value *val = varMgr_->tryGet(name);
                    if (val)
                    {
                        DebugVariable dv;
                        dv.name = name;
                        dv.value = val->toString();
                        dv.type = val->getTypeName();
                        dv.variablesReference = 0;
                        vars.push_back(dv);
                    }
                }
            }

            return vars;
        }

        // ═══════════════════════════════════════════════════════════════════
        // Evaluate
        // ═══════════════════════════════════════════════════════════════════

        void DebugServer::handleEvaluate(int seq, const std::string &json)
        {
            std::string expression = extractStringField(json, "expression");
            std::string context = extractStringField(json, "context");

            if (expression.empty())
            {
                sendErrorResponse(seq, "evaluate", "تعبير فارغ / Empty expression");
                return;
            }

            std::string result = evaluateExpression(expression);
            sendResponse(seq, "evaluate",
                         "{" + jsonPair("result", result) + "," +
                             jsonPairNum("variablesReference", 0) + "}");
        }

        std::string DebugServer::evaluateExpression(const std::string &expression)
        {
            if (!varMgr_)
                return "<لا يوجد مفسر متصل>";

            // (AR) محاولة 1: هل هو اسم متغير؟
            const Data::Value *val = varMgr_->tryGet(expression);
            if (val)
            {
                return val->toString();
            }

            // (AR) محاولة 2: تحليل وتقييم التعبير
            try
            {
                if (interpreter_)
                {
                    Sad::Lexer::LexerCore lexer(expression);
                    Sad::Parser::ParserCore parser(lexer);
                    auto stmts = parser.parseProgram();
                    if (!stmts.empty() && !parser.hasErrors())
                    {
                        // استخراج التعبير من أول جملة ExprStmt
                        auto *exprStmt = dynamic_cast<AST::ExprStmt *>(stmts[0].get());
                        if (exprStmt && exprStmt->expression)
                        {
                            Data::Value result = interpreter_->evaluateExpression(*exprStmt->expression);
                            return result.toString();
                        }
                    }
                }
            }
            catch (const std::exception &e)
            {
                return std::string("<\xD8\xAE\xD8\xB7\xD8\xA3: ") + e.what() + ">"; // <خطأ: ...>
            }

            return "<\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x85\xD8\xB9\xD8\xB1\xD9\x88\xD9\x81>"; // <غير معروف>
        }

        // ═══════════════════════════════════════════════════════════════════
        // Debug Hooks — Called from interpreter execution
        // ═══════════════════════════════════════════════════════════════════

        void DebugServer::onBeforeStatement(const std::string &file, int line, int column)
        {
            if (!connected_ || shouldDisconnect_)
                return;
            (void)column;

            currentLine_ = line;
            currentFile_ = file;

            // (AR) التحقق من نقاط التوقف
            bool hitBreakpoint = false;
            for (auto &bp : breakpoints_[file])
            {
                if (bp.enabled && bp.line == line)
                {
                    bp.hitCount++;

                    // (AR) التحقق من الشرط
                    if (!bp.condition.empty())
                    {
                        std::string result = evaluateExpression(bp.condition);
                        if (result == "false" || result == "خطأ" || result == "0" || result.empty())
                        {
                            continue; // (AR) الشرط غير محقق
                        }
                    }

                    // (AR) التحقق من شرط العدد
                    if (bp.hitCondition > 0 && bp.hitCount < bp.hitCondition)
                    {
                        continue;
                    }

                    hitBreakpoint = true;
                    break;
                }
            }

            // (AR) التحقق من وضع الخطوة
            bool shouldPause = false;

            if (hitBreakpoint)
            {
                shouldPause = true;
            }
            else if (stepping_)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (pendingAction_ == DebugAction::STEP_IN)
                {
                    shouldPause = true;
                }
                else if (pendingAction_ == DebugAction::STEP_OVER)
                {
                    int currentDepth = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(callStack_.size(), "debug_server_size");
                    if (currentDepth <= stepOverDepth_)
                    {
                        shouldPause = true;
                    }
                }
                else if (pendingAction_ == DebugAction::STEP_OUT)
                {
                    int currentDepth = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(callStack_.size(), "debug_server_size");
                    if (currentDepth <= stepOutDepth_)
                    {
                        shouldPause = true;
                    }
                }
                else if (pendingAction_ == DebugAction::PAUSE)
                {
                    shouldPause = true;
                }
            }
            else
            {
                // (AR) التحقق من طلب إيقاف مؤقت
                std::lock_guard<std::mutex> lock(mutex_);
                if (pendingAction_ == DebugAction::PAUSE)
                {
                    shouldPause = true;
                }
            }

            if (shouldPause)
            {
                std::string reason = hitBreakpoint ? "breakpoint" : "step";
                stopAndWait(reason, line, file);
            }
        }

        void DebugServer::onFunctionEnter(const std::string &functionName, const std::string &file, int line)
        {
            if (!connected_)
                return;

            CallFrame frame;
            frame.functionName = functionName;
            frame.file = file;
            frame.line = line;
            frame.depth = scopeMgr_ ? scopeMgr_->getCurrentDepth() : 0;
            callStack_.push_back(frame);
        }

        void DebugServer::onFunctionExit()
        {
            if (!connected_)
                return;
            if (!callStack_.empty())
            {
                callStack_.pop_back();
            }
        }

        void DebugServer::onException(const std::string &message, const std::string &file, int line)
        {
            if (!connected_)
                return;

            currentLine_ = line;
            currentFile_ = file;

            sendEvent("stopped",
                      "{\"reason\":\"exception\"," +
                          jsonPairNum("threadId", 1) + "," +
                          jsonPair("text", message) + "," +
                          jsonPair("description", message) + "}");

            waitForDebugAction();
        }

        void DebugServer::onOutput(const std::string &text)
        {
            if (!connected_)
                return;
            sendEvent("output",
                      "{\"category\":\"stdout\"," +
                          jsonPair("output", text + "\n") + "}");
        }

        // ═══════════════════════════════════════════════════════════════════
        // Stop & Wait — pause execution and wait for user action
        // ═══════════════════════════════════════════════════════════════════

        void DebugServer::stopAndWait(const std::string &reason, int line, const std::string &file)
        {
            // (AR) إرسال حدث "stopped"
            sendEvent("stopped",
                      "{\"reason\":\"" + reason + "\"," +
                          jsonPairNum("threadId", 1) + "," +
                          jsonPairNum("line", line) + "," +
                          jsonPair("source", file) + "}");

            paused_ = true;
            waitForDebugAction();
            paused_ = false;
        }

        void DebugServer::waitForDebugAction()
        {
            // (AR) انتظار إجراء من المستخدم (continue, step, etc.)
            std::unique_lock<std::mutex> lock(mutex_);
            pendingAction_ = DebugAction::NONE;
            cv_.wait(lock, [this]()
                     { return pendingAction_ != DebugAction::NONE || shouldDisconnect_; });
        }

    } // namespace Debug
} // namespace Sad
