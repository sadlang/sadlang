/**
 * @file sad_wasm.cpp
 * @brief (AR) نقطة دخول WebAssembly لمفسر لغة ص — تُصدّر C API للمتصفح
 * @brief (EN) WebAssembly entry point for Sad interpreter — exports C API to browser
 *
 * الترجمة / Build:
 *   emcc sad_wasm.cpp [...sources...] \
 *     -O2 -s WASM=1 -s EXPORTED_FUNCTIONS='["_sad_execute","_sad_render_ui","_sad_version","_malloc","_free"]' \
 *     -s EXPORTED_RUNTIME_METHODS='["cwrap","ccall","UTF8ToString","stringToUTF8","lengthBytesUTF8"]' \
 *     -s MODULARIZE=1 -s EXPORT_NAME=SadWasm \
 *     -s ALLOW_MEMORY_GROWTH=1 \
 *     -o sad.js
 */

#include <string>
#include <sstream>
#include <iostream>
#include <cstring>
#include <cstdlib>

// --- لغة ص core includes ---
#include "shared/lexer/include/lexer_core.h"
#include "shared/parser/include/parser_core.h"
#include "interpreter_new/include/core/interpreter_core.h"

// ================================================
// (AR) إعادة توجيه cout إلى بفر داخلي
// (EN) Redirect cout to internal buffer
// ================================================
struct OutputCapture {
    std::ostringstream buf;
    std::streambuf* old_cout = nullptr;
    std::streambuf* old_cerr = nullptr;

    void start() {
        old_cout = std::cout.rdbuf(buf.rdbuf());
        old_cerr = std::cerr.rdbuf(buf.rdbuf());
    }

    std::string stop() {
        std::cout << std::flush;
        std::cerr << std::flush;
        if (old_cout) { std::cout.rdbuf(old_cout); old_cout = nullptr; }
        if (old_cerr) { std::cerr.rdbuf(old_cerr); old_cerr = nullptr; }
        return buf.str();
    }
};

// ================================================
// (AR) بناء استجابة JSON بسيطة
// (EN) Build simple JSON response
// ================================================
static std::string escape_json(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (unsigned char c : s) {
        if      (c == '"')  out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else                out += c;
    }
    return out;
}

static std::string make_json(bool success, const std::string& output, const std::string& error = "") {
    return "{\"success\":" + std::string(success ? "true" : "false") +
           ",\"output\":\"" + escape_json(output) +
           "\",\"error\":\"" + escape_json(error) + "\"}";
}

// ================================================
// (AR) واجهة C المُصدَّرة للـ JS
// (EN) C API exported to JavaScript
// ================================================
extern "C" {

/**
 * @brief (AR) تنفيذ كود مصدري وإرجاع JSON
 * @brief (EN) Execute source code and return JSON
 *
 * @param source_utf8  (AR) الكود المصدري بـ UTF-8 / (EN) Source code in UTF-8
 * @param stdin_utf8   (AR) مدخل stdin (اختياري) / (EN) stdin input (optional)
 * @return (AR) مؤشر إلى نص JSON مُخصَّص بـ malloc — يجب تحريره بـ free()
 * @return (EN) malloc'd JSON string — must be free()'d by caller
 */
char* sad_execute(const char* source_utf8, const char* stdin_utf8) {
    if (!source_utf8) {
        std::string err = make_json(false, "", "source is null");
        char* ret = (char*)malloc(err.size() + 1);
        memcpy(ret, err.c_str(), err.size() + 1);
        return ret;
    }

    std::string source(source_utf8);
    std::string output;
    std::string error;
    bool success = false;

    try {
        // (AR) التقاط الإخراج / (EN) Capture output
        OutputCapture cap;
        cap.start();

        // (AR) التحليل المعجمي / (EN) Lexing
        Sad::Lexer::LexerCore lexer(source);

        // (AR) التحليل النحوي / (EN) Parsing
        Sad::Parser::ParserCore parser(lexer);
        auto program = parser.parseProgram();

        if (parser.hasErrors()) {
            output = cap.stop();
            // (AR) اقرأ أخطاء المحلل / (EN) Read parser errors
            std::ostringstream eos;
            // parser errors already printed to cerr (captured in buf)
            error = output;
            output = "";
        } else {
            // (AR) التنفيذ / (EN) Execution
            Sad::Interpreter::InterpreterOptions opts;
            opts.printResults     = false;
            opts.enableDebugMode  = false;
            opts.enableTypeCheck  = false;  // (AR) للأداء في المتصفح / (EN) performance in browser

            Sad::Interpreter::Interpreter interp(opts);
            auto result = interp.execute(program);

            output = cap.stop();
            success = result.success;

            if (!result.success) {
                error = result.errorMessage;
            }
        }
    } catch (const std::exception& e) {
        error = e.what();
    } catch (...) {
        error = "خطأ غير متوقع / Unknown error";
    }

    std::string json = make_json(success, output, error);
    char* ret = (char*)malloc(json.size() + 1);
    memcpy(ret, json.c_str(), json.size() + 1);
    return ret;
}

/**
 * @brief (AR) تنفيذ كود واجهة وإرجاع HTML مولّد
 * @brief (EN) Execute UI code and return generated HTML
 *
 * يُنفّذ كود ص ويلتقط HTML المولّد من _محرك_واجهات.
 * الاستجابة JSON: {"success":bool,"html":"...","output":"...","error":"..."}
 */
char* sad_render_ui(const char* source_utf8) {
    if (!source_utf8) {
        std::string err = "{\"success\":false,\"html\":\"\",\"output\":\"\",\"error\":\"source is null\"}";
        char* ret = (char*)malloc(err.size() + 1);
        memcpy(ret, err.c_str(), err.size() + 1);
        return ret;
    }

    std::string source(source_utf8);
    std::string output;
    std::string error;
    std::string html;
    bool success = false;

    try {
        OutputCapture cap;
        cap.start();

        Sad::Lexer::LexerCore lexer(source);
        Sad::Parser::ParserCore parser(lexer);
        auto program = parser.parseProgram();

        if (parser.hasErrors()) {
            output = cap.stop();
            error = output;
            output = "";
        } else {
            Sad::Interpreter::InterpreterOptions opts;
            opts.printResults = false;
            opts.enableDebugMode = false;
            opts.enableTypeCheck = false;

            Sad::Interpreter::Interpreter interp(opts);
            auto result = interp.execute(program);

            output = cap.stop();
            success = result.success;

            if (!result.success) {
                error = result.errorMessage;
            }

            // استخراج HTML المولّد من العلامات الخاصة
            const std::string startTag = "<!--SAD_UI_HTML_START-->\n";
            const std::string endTag = "\n<!--SAD_UI_HTML_END-->";
            auto startPos = output.find("<!--SAD_UI_HTML_START-->");
            auto endPos = output.find("<!--SAD_UI_HTML_END-->");
            if (startPos != std::string::npos && endPos != std::string::npos) {
                size_t htmlStart = startPos + std::string("<!--SAD_UI_HTML_START-->\n").size();
                html = output.substr(htmlStart, endPos - htmlStart);
                // إزالة HTML من الإخراج العادي
                std::string before = output.substr(0, startPos);
                std::string after = output.substr(endPos + std::string("<!--SAD_UI_HTML_END-->\n").size());
                output = before + after;
                // تنظيف أسطر فارغة
                while (!output.empty() && output.back() == '\n') output.pop_back();
            }
        }
    } catch (const std::exception& e) {
        error = e.what();
    } catch (...) {
        error = "خطأ غير متوقع / Unknown error";
    }

    std::string json = "{\"success\":" + std::string(success ? "true" : "false") +
                       ",\"html\":\"" + escape_json(html) +
                       "\",\"output\":\"" + escape_json(output) +
                       "\",\"error\":\"" + escape_json(error) + "\"}";
    char* ret = (char*)malloc(json.size() + 1);
    memcpy(ret, json.c_str(), json.size() + 1);
    return ret;
}

/**
 * @brief (AR) إرجاع إصدار المفسر / (EN) Return interpreter version
 */
const char* sad_version() {
    return "1.0.0";
}

} // extern "C"
