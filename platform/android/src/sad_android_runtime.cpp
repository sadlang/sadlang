// ═══════════════════════════════════════════════════════════════════════════════
// sad_android_runtime.cpp - Runtime كامل لتنفيذ كود ص على أندرويد
// Full Sad Runtime for Android — integrates Lexer, Parser, Interpreter
// ═══════════════════════════════════════════════════════════════════════════════

#include <android/log.h>
#include <string>
#include <memory>
#include <mutex>
#include <sstream>
#include <iostream>

// المحلل المعجمي والنحوي والمفسر الفعلي
#include "lexer_core.h"
#include "parser_core.h"
#include "interpreter_core.h"
#include "native_ui_storage.h"

#define LOG_TAG "SadRuntime"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

// ═══════════════════════════════════════════════════════════════════════════════
// Runtime State
// ═══════════════════════════════════════════════════════════════════════════════

namespace {
    bool g_initialized = false;
    std::string g_lastError;
    std::string g_lastOutput;
    std::string g_lastUIHtml;
    std::mutex g_runtimeMutex;
    std::unique_ptr<Sad::Interpreter::Interpreter> g_interpreter;

    // (AR) التقاط stdout/stderr لقراءة مخرجات اطبع()
    // (EN) Capture stdout/stderr to read print() output
    struct OutputCapture {
        std::stringstream buf;
        std::streambuf* old_cout = nullptr;
        std::streambuf* old_cerr = nullptr;
        OutputCapture() {
            old_cout = std::cout.rdbuf(buf.rdbuf());
            old_cerr = std::cerr.rdbuf(buf.rdbuf());
        }
        ~OutputCapture() { restore(); }
        void restore() {
            std::cout << std::flush;
            std::cerr << std::flush;
            if (old_cout) { std::cout.rdbuf(old_cout); old_cout = nullptr; }
            if (old_cerr) { std::cerr.rdbuf(old_cerr); old_cerr = nullptr; }
        }
        std::string str() { return buf.str(); }
    };
}

// ═══════════════════════════════════════════════════════════════════════════════
// Runtime API (C Interface)
// ═══════════════════════════════════════════════════════════════════════════════

extern "C" {

/**
 * @brief (AR) تهيئة runtime ص — ينشئ مفسر حقيقي
 * @brief (EN) Initialize Sad runtime — creates real interpreter
 */
bool sad_runtime_init() {
    std::lock_guard<std::mutex> lock(g_runtimeMutex);
    
    if (g_initialized) {
        LOGI("Runtime already initialized");
        return true;
    }
    
    LOGI("═══════════════════════════════════════");
    LOGI("  تهيئة Runtime ص / Initializing Sad Runtime");
    LOGI("═══════════════════════════════════════");
    
    try {
        Sad::Interpreter::InterpreterOptions opts;
        opts.printResults = false;
        g_interpreter = std::make_unique<Sad::Interpreter::Interpreter>(opts);
        
        g_initialized = true;
        g_lastError.clear();
        
        LOGI("[✓] Runtime initialized — interpreter ready");
    } catch (const std::exception& e) {
        g_lastError = std::string("Init failed: ") + e.what();
        LOGE("%s", g_lastError.c_str());
        return false;
    }
    
    return true;
}

/**
 * @brief (AR) إيقاف runtime ص
 * @brief (EN) Shutdown Sad runtime
 */
void sad_runtime_shutdown() {
    std::lock_guard<std::mutex> lock(g_runtimeMutex);
    
    if (!g_initialized) return;
    
    LOGI("Shutting down Sad runtime");
    g_interpreter.reset();
    g_initialized = false;
    g_lastError.clear();
    g_lastOutput.clear();
    LOGI("Runtime shutdown complete");
}

/**
 * @brief (AR) تنفيذ كود ص — يستخدم المفسر الحقيقي
 * @brief (EN) Execute Sad code — uses real interpreter pipeline
 */
bool sad_execute_code(const char* code) {
    std::lock_guard<std::mutex> lock(g_runtimeMutex);
    
    if (!g_initialized || !g_interpreter) {
        g_lastError = "Runtime not initialized";
        LOGE("%s", g_lastError.c_str());
        return false;
    }
    
    if (!code || !*code) {
        g_lastError = "Empty code";
        LOGE("%s", g_lastError.c_str());
        return false;
    }
    
    LOGD("Executing code: %.100s%s", code, strlen(code) > 100 ? "..." : "");
    
    try {
        // التقاط مخرجات cout/cerr
        OutputCapture cap;

        // 1. تحليل معجمي
        std::string sourceCode(code);
        Sad::Lexer::LexerCore lexer(sourceCode);
        
        // 2. تحليل نحوي
        Sad::Parser::ParserCore parser(lexer);
        auto program = parser.parseProgram();
        
        if (parser.hasErrors()) {
            cap.restore();
            g_lastError = "Parse errors encountered";
            g_lastOutput = cap.str();
            LOGE("[ص] Parse error");
            return false;
        }
        
        // 3. تنفيذ
        auto result = g_interpreter->execute(program);
        cap.restore();

        std::string captured = cap.str();
        
        // استخراج HTML المولّد إذا وُجد (للتوافق مع WASM)
        g_lastUIHtml.clear();
        const std::string startMarker = "<!--SAD_UI_HTML_START-->";
        const std::string endMarker = "<!--SAD_UI_HTML_END-->";
        auto startPos = captured.find(startMarker);
        auto endPos = captured.find(endMarker);
        if (startPos != std::string::npos && endPos != std::string::npos) {
            size_t htmlStart = startPos + startMarker.size();
            // تخطي سطر جديد بعد العلامة
            if (htmlStart < captured.size() && captured[htmlStart] == '\n') htmlStart++;
            g_lastUIHtml = captured.substr(htmlStart, endPos - htmlStart);
            // إزالة سطر جديد قبل نهاية العلامة
            while (!g_lastUIHtml.empty() && g_lastUIHtml.back() == '\n')
                g_lastUIHtml.pop_back();
            // إزالة HTML من المخرجات العادية
            captured = captured.substr(0, startPos) + captured.substr(endPos + endMarker.size());
        }
        
        // اكتشاف علامة الواجهة الأصلية (بدلاً من HTML)
        const std::string nativeMarker = "<!--SAD_NATIVE_UI_READY-->";
        auto nativePos = captured.find(nativeMarker);
        if (nativePos != std::string::npos) {
            // إزالة العلامة من المخرجات
            captured = captured.substr(0, nativePos) + 
                       captured.substr(nativePos + nativeMarker.size());
            LOGI("[ص] واجهة أصلية جاهزة للعرض");
        }
        
        if (result.success) {
            g_lastError.clear();
            g_lastOutput = captured;
            if (g_lastOutput.empty()) {
                g_lastOutput = result.result.toString();
            }
            LOGI("[ص] Execution success");
            return true;
        } else {
            g_lastError = result.errorMessage;
            g_lastOutput = captured;
            LOGE("[ص] Error: %s", g_lastError.c_str());
            return false;
        }
    } catch (const std::exception& e) {
        g_lastError = e.what();
        LOGE("[ص] Exception: %s", g_lastError.c_str());
        return false;
    }
}

/**
 * @brief (AR) الحصول على آخر خطأ
 * @brief (EN) Get last error message
 */
const char* sad_get_last_error() {
    return g_lastError.empty() ? nullptr : g_lastError.c_str();
}

/**
 * @brief (AR) الحصول على آخر مخرجات
 * @brief (EN) Get last output
 */
const char* sad_get_last_output() {
    return g_lastOutput.empty() ? nullptr : g_lastOutput.c_str();
}

/**
 * @brief (AR) التحقق من حالة التهيئة
 * @brief (EN) Check if runtime is initialized
 */
bool sad_is_initialized() {
    std::lock_guard<std::mutex> lock(g_runtimeMutex);
    return g_initialized;
}

/**
 * @brief (AR) إصدار المكتبة
 * @brief (EN) Library version
 */
const char* sad_version() {
    return "1.0.0";
}

/**
 * @brief (AR) الحصول على HTML الواجهة المولّدة
 * @brief (EN) Get generated UI HTML
 */
const char* sad_get_ui_html() {
    return g_lastUIHtml.empty() ? nullptr : g_lastUIHtml.c_str();
}

/**
 * @brief (AR) هل يوجد واجهة أصلية جاهزة للعرض؟
 * @brief (EN) Is there a native UI ready to render?
 */
bool sad_has_native_ui() {
    return sad_native_ui_has();
}

/**
 * @brief (AR) الحصول على وحدة IR المخزّنة
 * @brief (EN) Get stored IR module (for NativeUIBuilder)
 */
std::shared_ptr<sad::ui::IRModule> sad_get_native_ui_module() {
    return sad_native_ui_get();
}

/**
 * @brief (AR) مسح الواجهة الأصلية المعلّقة
 * @brief (EN) Clear pending native UI
 */
void sad_clear_native_ui() {
    sad_native_ui_clear();
}

} // extern "C"
