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
#include "interpreter/include/core/interpreter_core.h"
#include "shared/types/include/class_manager.h"
// (AR) م2-أ: لا يُثبَّت جسر الواجهات في wasm — بناء wasm لا يضمّ مصادر sad_ui،
//      والمسار البديل (uiEvalBridge()==nullptr) يجعل الرسومات بلا أثر بلا انهيار.
// (EN) Phase 2-A: no UI bridge in wasm — the wasm build excludes sad_ui sources; the
//      headless fallback (null bridge) makes UI a no-op without crashing.

// ================================================
// (AR) إعادة توجيه cout إلى بفر داخلي
// (EN) Redirect cout to internal buffer
// ================================================
// (AR) الاستعادة في الهادم — لا في `stop()` وحدَها. كان `stop()` هو الطريقَ
//      الوحيد، ومسارُ الاستثناء لا يمرّ به: يرمي المحلّلُ فيخرج `cap` من نطاقه
//      ويُدمَّر، و`std::cout` ما زال يشير إلى `streambuf` داخله — **مؤشّرٌ
//      معلَّقٌ إلى كائنٍ مُدمَّر**. فكلُّ طباعةٍ بعد ذلك سلوكٌ غيرُ معرَّف، وكلُّ
//      تشغيلةٍ تالية تردّ خرجًا فارغًا و«فشل» بلا رسالة.
//
//      وهذا يقع على المتعلّم لا في مختبر: الصفحةُ تشغّل نسخةً واحدةً طوال
//      الزيارة، فمن أخطأ نحويًّا مرّةً يُقال له بعدها إنّ كودَه الصحيحَ فاشل —
//      ولا شيءَ في كودِه فاشل. وأخفت هذا زمنًا قائمةُ مصادرَ يدويّةٌ كانت تبني
//      «مفسّرًا» أصغر لا يبلغ هذا المسار.
// (EN) Restore in the destructor, not only in stop(). The exception path never
//      reached stop(): the parser throws, `cap` is destroyed, and std::cout still
//      points at a streambuf inside it — a dangling pointer. Every later write is
//      UB, and every later run returns empty output with a silent failure.
struct OutputCapture {
    // (AR) مخزنان لا مخزنٌ واحد. كان `cout` و`cerr` يُصبّان في مخزنٍ واحدٍ يُسلَّم
    //      كلُّه حقلَ `output`، فيرى المتعلّمُ تحذيرَ المحلّلِ **داخل خرج برنامجه**.
    //      و`sad-run` يفصلهما — التشخيصُ إلى `stderr` والخرجُ إلى `stdout` —
    //      فتُقارَن الصفحةُ بالأصليّ فتنحرف عنه في كلّ برنامجٍ يُنتج تحذيرًا.
    //      قِيس ذلك حيًّا على تحدّي التعليقات (`web-l1-02`): برنامجٌ ناجحٌ خرجُه
    //      «فهمت التعليقات!» وحدَها، وردّ المتصفّحُ تحذيرَ SYN024 ملصوقًا قبله.
    // (EN) Two buffers, not one. cout and cerr both fed a single buffer handed back
    //      as `output`, so a parser warning appeared inside the program's own output.
    //      sad-run separates them, so the page diverged from native on any program
    //      that emits a warning. Measured on the comments challenge (web-l1-02).
    std::ostringstream outBuf;
    std::ostringstream errBuf;
    std::streambuf* old_cout = nullptr;
    std::streambuf* old_cerr = nullptr;

    void start() {
        old_cout = std::cout.rdbuf(outBuf.rdbuf());
        old_cerr = std::cerr.rdbuf(errBuf.rdbuf());
    }

    // (AR) `noexcept` لأنّها تُنادى من الهادم / (EN) called from the destructor
    void restore() noexcept {
        if (old_cout) { std::cout.rdbuf(old_cout); old_cout = nullptr; }
        if (old_cerr) { std::cerr.rdbuf(old_cerr); old_cerr = nullptr; }
    }

    void flushBoth() {
        std::cout << std::flush;
        std::cerr << std::flush;
    }

    // (AR) قراءةُ التشخيصِ دون إنهاء الالتقاط — لنعرف أفارغٌ هو فنُكمله.
    // (EN) Read diagnostics without ending the capture, to tell whether to fill it.
    std::string peekErrors() { flushBoth(); return errBuf.str(); }

    void stop() { flushBoth(); restore(); }

    std::string output() { flushBoth(); return outBuf.str(); }
    std::string errors() { flushBoth(); return errBuf.str(); }

    ~OutputCapture() { restore(); }

    // (AR) النسخُ يعني مخزنَين وتوجيهًا واحدًا — تُمنَع صراحةً لا سهوًا.
    OutputCapture() = default;
    OutputCapture(const OutputCapture&) = delete;
    OutputCapture& operator=(const OutputCapture&) = delete;
};
// ================================================
// (AR) تصفيرُ حالةِ الأخطاء قبل كلّ تشغيلة
// (EN) Reset error state before every run
// ================================================
// (AR) ‏`ErrorManager` **مفردةٌ عالميّةٌ للعمليّة**، و`hasErrors()` في المحلّل
//      يقرأ منها. و`sad-run` يمسحها في أوّل كلّ تشغيلة (apps/sad-run/main.cpp)
//      لأنّه عمليّةٌ لبرنامجٍ واحد — أمّا هنا فالعمليّةُ واحدةٌ لمئات البرامج،
//      ولم يكن أحدٌ يمسحها. فأوّلُ خطأٍ نحويٍّ يجعل `hasErrors()` صادقةً **إلى
//      الأبد**: كلُّ برنامجٍ صحيحٍ بعده يُختَم «فشل» بخرجٍ فارغٍ ورسالةٍ فارغة،
//      لأنّ الأخطاء المطبوعة سبق أن استُهلكت.
//
//      وهذا يقع على المتعلّم مباشرةً: الصفحةُ تشغّل نسخةً واحدةً طوال الزيارة،
//      فمن أخطأ نحويًّا مرّةً يُقال له بعدها إنّ كودَه الصحيحَ فاشل — بلا سببٍ
//      يُقرأ. وأخفاه زمنًا أنّ بناء wasm كان من قائمةِ مصادرَ يدويّةٍ لا تبلغ
//      هذا المسار، وأنّ `sad-run` لا يراه بنيويًّا.
// (EN) ErrorManager is a process-wide singleton and parser.hasErrors() reads it.
//      sad-run clears it per run because it is one process per program; here one
//      process serves hundreds. The first syntax error made hasErrors() true
//      forever, so every valid program afterwards was reported as failed.
//
// (AR) وليست `ErrorManager` وحدَها: `ClassManager` مفردةٌ للعمليّة كذلك، ولا
//      يستبدل `registerClass` تعريفًا قائمًا له حقولٌ أو دوالّ — يُبقي الأوّلَ
//      ويُرجِع `true`. فمن كتب `صنف مِعيار` بطريقةٍ تُعيد ١١١ ثمّ عدّلها لتُعيد
//      ٢٢٢ وضغط «شغّل» يُقال له ١١١، بنجاحٍ وبلا تحذير. قِيس ذلك حيًّا: ثلاثُ
//      تشغيلاتٍ متعاقبةٍ والجوابُ ١١١ في كلٍّ منها. وهو أسوأُ من الخطأ الصريح،
//      لأنّ المتعلّم يرى تعديلَه يُنكَر عليه بلا سبب.
//
//      والمسحُ مقصورٌ على المفردتين اللتين قِيس تسرّبُهما. غيرُهما
//      (`ObjectManager` و`ModuleCache` وسواهما) يتراكم كذلك، لكنّ تراكمَه لم
//      يُثبَت أثرُه على الخرج بعد — ولا يُمسح ما لم يُقَس، لئلّا يُكنَس عيبٌ
//      حيٌّ تحت السجّاد.
// (EN) ClassManager is a process-wide singleton too, and registerClass keeps the
//      OLD definition when one already has fields/methods — a learner editing a
//      class silently keeps running the previous version. Measured live.
//      Only the two singletons whose leakage was measured are cleared.
static void reset_run_state(const std::string& source) {
    auto& errors = Sad::Errors::ErrorManager::getInstance();
    errors.clear();
    // (AR) المصدرُ يُسجَّل كما يفعل `sad-run`، فتحمل رسالةُ الخطأ سطرَها.
    errors.setSourceCode(source, "<المتصفّح>");

    if (auto* classes = Sad::Data::ClassManager::getInstance()) {
        classes->clearAll();
    }

    // (AR) لا ألوانَ ANSI في المتصفّح: التشخيصُ يُسلَّم نصًّا في JSON لا إلى طرفيّة،
    //      فرموزُ `\033[…m` تصير حروفًا مشوّهةً في صندوق الخطأ.
    // (EN) No ANSI in the browser: diagnostics go into JSON, not a terminal.
    errors.setColorize(false);
}

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
        // (AR) وكلُّ محرفِ تحكّمٍ آخرَ يُهرَّب بـ`\uXXXX`. لم يكن يُهرَّب، فأيُّ
        //      بايتٍ دون 0x20 — و`\033` من ألوان ANSI أوّلُها — يُنتج **JSON غيرَ
        //      صالح** يُسقط `JSON.parse` في العامل، فيُقرأ العطبُ «انهيارًا» لا
        //      خطأً في الكود. والقاعدةُ العامّةُ أوثقُ من إطفاء اللون وحدَه: أيُّ
        //      مصدرٍ يبعث محرفَ تحكّمٍ لاحقًا لن يكسر العقد.
        // (EN) Escape every other control char as \uXXXX. Unescaped, any byte below
        //      0x20 — ANSI's \033 first among them — makes the JSON invalid and the
        //      worker reads a plain error as a crash.
        else if (c < 0x20) {
            static constexpr const char* HEX = "0123456789abcdef";
            out += "\\u00";
            out += HEX[(c >> 4) & 0xF];
            out += HEX[c & 0xF];
        }
        else                out += c;
    }
    return out;
}

// (AR) آخرُ ملاذٍ حين يفشل التنفيذُ ولا يقول أحدٌ لماذا. الفراغُ في هذا الموضع
//      يُعرَض للمتعلّم صندوقًا أحمرَ خاليًا: يعرف أنّه أخفق ولا يعرف بمَ. وقد قِيس
//      حيًّا على `س.ص.ع` — خرجٌ فارغٌ ورسالةٌ فارغةٌ و«فشل». فالنصُّ هنا ليس علاجًا
//      للعيب بل **إظهارٌ** له: يجعله مرئيًّا في الصفحة وفي بوّابة التكافؤ بدل أن
//      يمرّ صمتًا، حتّى يُلاحَق في المفسّر حيث موضعُه.
// (EN) Last resort when execution fails and nothing says why. An empty reason renders
//      as a blank red box: the learner knows they failed, not why. Measured on `س.ص.ع`.
//      This does not fix the defect — it makes it visible instead of silent.
// (AR) وما لا اسمَ له من الاستثناءات — نصٌّ مسمًّى لا سلسلةٌ مبثوثةٌ في الكود.
// (EN) The unnamed exception path — a named constant, not an inline literal.
static constexpr const char* UNEXPECTED_ERROR = "خطأ غير متوقع / Unknown error";

static constexpr const char* NO_REASON_GIVEN =
    "أخفق التنفيذُ ولم يذكر المفسّرُ سببًا. هذا عيبٌ في المفسّر لا في كودك.";

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

    // (AR) خارج `try` كي يبلغه `catch`: ما طُبع قبل الرمي جزءٌ من جواب المتعلّم،
    //      وابتلاعُه يجعل برنامجًا طبع عشرة أسطرٍ ثمّ تعثّر يبدو كأنّه لم يطبع شيئًا.
    OutputCapture cap;
    try {
        cap.start();
        reset_run_state(source);

        // (AR) التحليل المعجمي / (EN) Lexing
        Sad::Lexer::LexerCore lexer(source);

        // (AR) التحليل النحوي / (EN) Parsing
        Sad::Parser::ParserCore parser(lexer);
        auto program = parser.parseProgram();

        if (parser.hasErrors()) {
            // (AR) كان التعليقُ هنا يقول «الأخطاءُ سبق أن طُبعت إلى cerr» — وهو
            //      صحيحٌ في بعض المسارات فقط. المساراتُ التي تُسجّل في
            //      `ErrorManager` مركزيًّا لا تطبع شيئًا؛ الطباعةُ في `printAll()`
            //      وحدَها، ويناديها `sad-run` عبر `parser.printErrors()`. ولم يكن
            //      أحدٌ يناديها هنا، فيردّ المتصفّحُ «فشل» **برسالةٍ فارغة**: قِيس
            //      ذلك حيًّا على `متغير = ` — لا سببَ ولا سطرَ ولا رمز.
            //      فإن خلا الملتقَطُ نُفرِغ السجلَّ المركزيَّ فيه.
            // (EN) The old comment assumed parser errors were already on cerr; the
            //      centrally-collected ones are not — printAll() prints, and only
            //      sad-run called it. The browser answered "failed" with an empty
            //      reason (measured on `متغير = `). Flush the registry if empty.
            if (cap.peekErrors().empty()) {
                // (AR) `flush` لا `printErrors`: الثانيةُ تنادي `printAll()` بلونها
                //      الافتراضيّ فتتجاهل `setColorize`، والأولى تحترم اللونَ
                //      **والمصدرَ المسجَّل** معًا فيحمل التشخيصُ سطرَه.
                // (EN) flush() honours colorize and the registered source; printErrors()
                //      calls printAll() with its default colour and ignores both.
                Sad::Errors::ErrorManager::getInstance().flush(std::cerr);
            }
            cap.stop();
            error = cap.errors();
            output = "";
            if (error.empty()) {
                error = NO_REASON_GIVEN;
            }
        } else {
            // (AR) التنفيذ / (EN) Execution
            Sad::Interpreter::InterpreterOptions opts;
            opts.printResults     = false;
            opts.enableDebugMode  = false;
            opts.enableTypeCheck  = false;  // (AR) للأداء في المتصفح / (EN) performance in browser

            Sad::Interpreter::Interpreter interp(opts);
            auto result = interp.execute(program);

            success = result.success;

            if (!result.success) {
                error = result.errorMessage;
                // (AR) والمفسّرُ قد يفشل بلا رسالةٍ كذلك: نُفرِغ السجلَّ المركزيَّ
                //      في مجرى التشخيص قبل إنهائه، لعلّ فيه ما لم يُطبع.
                // (EN) The interpreter can fail message-less too: flush the registry.
                if (error.empty() && cap.peekErrors().empty()) {
                    // (AR) `flush` لا `printErrors`: الثانيةُ تنادي `printAll()` بلونها
                //      الافتراضيّ فتتجاهل `setColorize`، والأولى تحترم اللونَ
                //      **والمصدرَ المسجَّل** معًا فيحمل التشخيصُ سطرَه.
                // (EN) flush() honours colorize and the registered source; printErrors()
                //      calls printAll() with its default colour and ignores both.
                Sad::Errors::ErrorManager::getInstance().flush(std::cerr);
                }
            }

            cap.stop();
            output = cap.output();

            // (AR) والتشخيصُ يبقى تشخيصًا وإن نجح البرنامج: لا يُلصَق بخرجه.
            //      فإن أخفق ولم يُسمَّ سببُه أخذناه من مجرى التشخيص، وإلّا قلنا
            //      صراحةً إنّ سببًا لم يُذكَر.
            // (EN) Diagnostics stay diagnostics even on success — never glued to output.
            if (!success && error.empty()) {
                const std::string diagnostics = cap.errors();
                error = diagnostics.empty() ? NO_REASON_GIVEN : diagnostics;
            }
        }
    } catch (const std::exception& e) {
        cap.stop();
        output = cap.output();
        error = e.what();
    } catch (...) {
        cap.stop();
        output = cap.output();
        error = UNEXPECTED_ERROR;
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

    OutputCapture cap;
    try {
        cap.start();
        reset_run_state(source);

        Sad::Lexer::LexerCore lexer(source);
        Sad::Parser::ParserCore parser(lexer);
        auto program = parser.parseProgram();

        if (parser.hasErrors()) {
            // (AR) العقدُ نفسُه في `sad_execute`: التشخيصُ من مجراه، ولا صمت.
            // (EN) Same contract as sad_execute: diagnostics from their own stream.
            if (cap.peekErrors().empty()) {
                Sad::Errors::ErrorManager::getInstance().flush(std::cerr);
            }
            cap.stop();
            error = cap.errors();
            output = "";
            if (error.empty()) {
                error = NO_REASON_GIVEN;
            }
        } else {
            Sad::Interpreter::InterpreterOptions opts;
            opts.printResults = false;
            opts.enableDebugMode = false;
            opts.enableTypeCheck = false;

            Sad::Interpreter::Interpreter interp(opts);
            auto result = interp.execute(program);

            success = result.success;

            if (!result.success) {
                error = result.errorMessage;
                if (error.empty() && cap.peekErrors().empty()) {
                    Sad::Errors::ErrorManager::getInstance().flush(std::cerr);
                }
            }

            cap.stop();
            output = cap.output();

            if (!success && error.empty()) {
                const std::string diagnostics = cap.errors();
                error = diagnostics.empty() ? NO_REASON_GIVEN : diagnostics;
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
        cap.stop();
        output = cap.output();
        error = e.what();
    } catch (...) {
        cap.stop();
        output = cap.output();
        error = UNEXPECTED_ERROR;
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
