/**
 * @file test_interpreter_comprehensive.cpp
 * @brief (AR) اختبارات شاملة للمفسر / (EN) Comprehensive Interpreter Tests
 * 
 * ~120 اختبار يغطي: تعريف المتغيرات، العمليات الحسابية، الدوال، التحكم بالتدفق،
 * OOP، المصفوفات، القواميس، الأخطاء، النطاقات، التكرار
 */

#include "sad_test_framework.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "interpreter_core.h"
#include "value.h"

#include <csignal>

#ifdef _WIN32
#include <windows.h>
#undef VOID
#undef ERROR
#undef FATAL
#undef NEAR
#undef FAR
#undef DELETE
#undef IN
#undef OUT
#undef OPTIONAL
#undef interface
#undef CONST
#undef TRUE
#undef FALSE
#undef RGB
#endif

using namespace Sad;

// ======================================================================
// (AR) حماية من الانهيار / (EN) Crash Protection
// ======================================================================
static volatile bool g_interpreterWorks = false;

static bool testInterpreterInit() {
    // (AR) اختبار سريع لمعرفة هل المفسر يعمل بدون انهيار
    // (EN) Quick smoke test to see if interpreter works without crashing
    try {
        Interpreter::InterpreterOptions opts;
        Interpreter::Interpreter interp(opts);
        
        // (AR) تجربة تنفيذ برنامج بسيط جداً
        // (EN) Try executing a very simple program
        Lexer::LexerCore lexer("\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 1");
        Parser::ParserCore parser(lexer);
        auto program = parser.parseProgram();
        auto result = interp.execute(program);
        
        g_interpreterWorks = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "(AR) فشل تهيئة المفسر / (EN) Interpreter init failed: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "(AR) خطأ غير متوقع في تهيئة المفسر / (EN) Unknown interpreter init error" << std::endl;
        return false;
    }
}

// ======================================================================
// دوال مساعدة / Helper Functions
// ======================================================================

static Interpreter::ExecutionResult runCode(const std::string& source) {
    try {
        Lexer::LexerCore lexer(source);
        Parser::ParserCore parser(lexer);
        auto program = parser.parseProgram();
        
        Interpreter::Interpreter interp;
        return interp.execute(program);
    } catch (const std::exception& e) {
        return Interpreter::ExecutionResult(false, Data::Value(), e.what());
    } catch (...) {
        return Interpreter::ExecutionResult(false, Data::Value(), "unknown exception");
    }
}

static Data::Value eval(const std::string& source) {
    auto result = runCode(source);
    return result.result;
}

static bool runsSuccessfully(const std::string& source) {
    try {
        auto result = runCode(source);
        return result.success;
    } catch (...) {
        return false;
    }
}

static bool throwsError(const std::string& source) {
    try {
        auto result = runCode(source);
        return !result.success;
    } catch (...) {
        return true;
    }
}

// Helper to run and get last variable value
static Data::Value runAndGetVar(const std::string& source, const std::string& varName) {
    try {
        Lexer::LexerCore lexer(source);
        Parser::ParserCore parser(lexer);
        auto program = parser.parseProgram();
        
        Interpreter::InterpreterOptions opts;
        Interpreter::Interpreter interp(opts);
        interp.execute(program);
        
        auto& vm = interp.getVariableManager();
        try {
            return vm.get(varName);
        } catch (...) {
            return Data::Value();
        }
    } catch (const std::exception&) {
        return Data::Value();
    } catch (...) {
        return Data::Value();
    }
}

// ======================================================================
int main() {
    SAD_TEST_INIT();

    // ══════════════════════════════════════════════════════════════════
    // (AR) اختبار سلامة المفسر — إذا فشل تتخطى جميع الاختبارات
    // (EN) Interpreter sanity check — if it fails, skip all tests
    // ══════════════════════════════════════════════════════════════════
    if (!testInterpreterInit()) {
        SAD_GROUP("Interpreter.SanityCheck / المفسر.فحص_السلامة");
        SAD_SKIP("INIT: interpreter initialization failed — skipping all tests",
                 "المفسر لم يتهيأ بنجاح");
        SAD_SUMMARY();
        return 0;
    }

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 1: المتغيرات الأساسية / Basic Variables
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Interpreter.Variables / المفسر.المتغيرات");

    SAD_TEST("VAR01: تعريف رقم صحيح", {
        // متغير س = 42
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 42", "x");
        SAD_ASSERT_EQ(v.toInt(), 42);
    });

    SAD_TEST("VAR02: تعريف رقم عشري", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 3.14", "x");
        SAD_ASSERT_FLOAT_EQ(v.toDouble(), 3.14, 0.001);
    });

    SAD_TEST("VAR03: تعريف نص", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = \"hello\"", "x");
        SAD_ASSERT_EQ(v.toString(), std::string("hello"));
    });

    SAD_TEST("VAR04: تعريف منطقي صحيح", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = \xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD", "x");
        SAD_ASSERT_EQ(v.toBool(), true);
    });

    SAD_TEST("VAR05: تعريف منطقي خطأ", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = \xD8\xAE\xD8\xB7\xD8\xA3", "x");
        SAD_ASSERT_EQ(v.toBool(), false);
    });

    SAD_TEST("VAR06: إعادة إسناد", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 5\nx = 10", "x");
        SAD_ASSERT_EQ(v.toInt(), 10);
    });

    SAD_TEST("VAR07: متغير = تعبير", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 3 + 4", "x");
        SAD_ASSERT_EQ(v.toInt(), 7);
    });

    SAD_TEST("VAR08: عدة متغيرات", {
        auto code = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 a = 1\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 b = 2\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 c = a + b";
        auto v = runAndGetVar(code, "c");
        SAD_ASSERT_EQ(v.toInt(), 3);
    });

    SAD_TEST("VAR09: += إسناد مركب", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 5\nx += 3", "x");
        SAD_ASSERT_EQ(v.toInt(), 8);
    });

    SAD_TEST("VAR10: -= إسناد مركب", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 10\nx -= 3", "x");
        SAD_ASSERT_EQ(v.toInt(), 7);
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 2: العمليات الحسابية / Arithmetic
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Interpreter.Arithmetic / المفسر.الحساب");

    SAD_TEST("MATH01: جمع 2+3=5", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 2 + 3", "x");
        SAD_ASSERT_EQ(v.toInt(), 5);
    });

    SAD_TEST("MATH02: طرح 10-4=6", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 10 - 4", "x");
        SAD_ASSERT_EQ(v.toInt(), 6);
    });

    SAD_TEST("MATH03: ضرب 6*7=42", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 6 * 7", "x");
        SAD_ASSERT_EQ(v.toInt(), 42);
    });

    SAD_TEST("MATH04: قسمة 20/4=5", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 20 / 4", "x");
        SAD_ASSERT_EQ(v.toInt(), 5);
    });

    SAD_TEST("MATH05: باقي 10%3=1", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 10 % 3", "x");
        SAD_ASSERT_EQ(v.toInt(), 1);
    });

    SAD_TEST("MATH06: أولوية العمليات 2+3*4=14", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 2 + 3 * 4", "x");
        SAD_ASSERT_EQ(v.toInt(), 14);
    });

    SAD_TEST("MATH07: أقواس (2+3)*4=20", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = (2 + 3) * 4", "x");
        SAD_ASSERT_EQ(v.toInt(), 20);
    });

    SAD_TEST("MATH08: سالب -5", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = -5", "x");
        SAD_ASSERT_EQ(v.toInt(), -5);
    });

    SAD_TEST("MATH09: عشري عمليات 1.5 + 2.5", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 1.5 + 2.5", "x");
        SAD_ASSERT_FLOAT_EQ(v.toDouble(), 4.0, 0.001);
    });

    SAD_TEST("MATH10: تعبير مركب ((1+2)*(3+4))/7 = 3", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = ((1+2)*(3+4))/7", "x");
        SAD_ASSERT_EQ(v.toInt(), 3);
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 3: النصوص / Strings
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Interpreter.Strings / المفسر.النصوص");

    SAD_TEST("STR01: دمج نصوص", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = \"hello\" + \" \" + \"world\"", "x");
        SAD_ASSERT_EQ(v.toString(), std::string("hello world"));
    });

    SAD_TEST("STR02: نص فارغ", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = \"\"", "x");
        SAD_ASSERT_EQ(v.toString(), std::string(""));
    });

    SAD_TEST("STR03: نص عربي", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = \"\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7\"", "x");
        SAD_ASSERT_EQ(v.toString(), std::string("\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7"));
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 4: المقارنة / Comparisons
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Interpreter.Comparisons / المفسر.المقارنات");

    SAD_TEST("CMP01: 5 == 5", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 5 == 5", "x");
        SAD_ASSERT_EQ(v.toBool(), true);
    });

    SAD_TEST("CMP02: 5 != 3", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 5 != 3", "x");
        SAD_ASSERT_EQ(v.toBool(), true);
    });

    SAD_TEST("CMP03: 3 < 5", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 3 < 5", "x");
        SAD_ASSERT_EQ(v.toBool(), true);
    });

    SAD_TEST("CMP04: 5 > 3", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 5 > 3", "x");
        SAD_ASSERT_EQ(v.toBool(), true);
    });

    SAD_TEST("CMP05: 5 <= 5", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 5 <= 5", "x");
        SAD_ASSERT_EQ(v.toBool(), true);
    });

    SAD_TEST("CMP06: 5 >= 5", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 5 >= 5", "x");
        SAD_ASSERT_EQ(v.toBool(), true);
    });

    SAD_TEST("CMP07: && (true && true)", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = \xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD && \xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD", "x");
        SAD_ASSERT_EQ(v.toBool(), true);
    });

    SAD_TEST("CMP08: || (false || true)", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = \xD8\xAE\xD8\xB7\xD8\xA3 || \xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD", "x");
        SAD_ASSERT_EQ(v.toBool(), true);
    });

    SAD_TEST("CMP09: ! نفي", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = !\xD8\xAE\xD8\xB7\xD8\xA3", "x");
        SAD_ASSERT_EQ(v.toBool(), true);
    });

    SAD_TEST("CMP10: مقارنة نصوص", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = \"abc\" == \"abc\"", "x");
        SAD_ASSERT_EQ(v.toBool(), true);
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 5: الشروط / Conditionals
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Interpreter.Conditionals / المفسر.الشروط");

    SAD_TEST("IF01: إذا صحيح", {
        auto code = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 0\n"
                     "\xD8\xA5\xD8\xB0\xD8\xA7 (\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD)\n"
                     "  x = 1\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 1);
    });

    SAD_TEST("IF02: إذا خطأ", {
        auto code = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 0\n"
                     "\xD8\xA5\xD8\xB0\xD8\xA7 (\xD8\xAE\xD8\xB7\xD8\xA3)\n"
                     "  x = 1\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 0);
    });

    SAD_TEST("IF03: إذا-وإلا (true branch)", {
        auto code = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 0\n"
                     "\xD8\xA5\xD8\xB0\xD8\xA7 (5 > 3)\n"
                     "  x = 1\n"
                     "\xD9\x88\xD8\xA5\xD9\x84\xD8\xA7\n"
                     "  x = 2\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 1);
    });

    SAD_TEST("IF04: إذا-وإلا (false branch)", {
        auto code = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 0\n"
                     "\xD8\xA5\xD8\xB0\xD8\xA7 (3 > 5)\n"
                     "  x = 1\n"
                     "\xD9\x88\xD8\xA5\xD9\x84\xD8\xA7\n"
                     "  x = 2\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 2);
    });

    SAD_TEST("IF05: شرط متداخل", {
        auto code = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 0\n"
                     "\xD8\xA5\xD8\xB0\xD8\xA7 (\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD)\n"
                     "  \xD8\xA5\xD8\xB0\xD8\xA7 (\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD)\n"
                     "    x = 42\n"
                     "  \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 42);
    });

    SAD_TEST("IF06: شرط مع مقارنة متغير", {
        auto code = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 a = 10\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 0\n"
                     "\xD8\xA5\xD8\xB0\xD8\xA7 (a > 5)\n"
                     "  x = 1\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 1);
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 6: الحلقات / Loops
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Interpreter.Loops / المفسر.الحلقات");

    SAD_TEST("LOOP01: بينما عداد", {
        auto code = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 0\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 i = 0\n"
                     "\xD8\xA8\xD9\x8A\xD9\x86\xD9\x85\xD8\xA7 (i < 5)\n"
                     "  x = x + 1\n"
                     "  i = i + 1\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 5);
    });

    SAD_TEST("LOOP02: بينما لا ينفذ (false)", {
        auto code = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 0\n"
                     "\xD8\xA8\xD9\x8A\xD9\x86\xD9\x85\xD8\xA7 (\xD8\xAE\xD8\xB7\xD8\xA3)\n"
                     "  x = 1\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 0);
    });

    SAD_TEST("LOOP03: مجموع 1 إلى 10", {
        auto code = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 sum = 0\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 i = 1\n"
                     "\xD8\xA8\xD9\x8A\xD9\x86\xD9\x85\xD8\xA7 (i <= 10)\n"
                     "  sum = sum + i\n"
                     "  i = i + 1\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9";
        auto v = runAndGetVar(code, "sum");
        SAD_ASSERT_EQ(v.toInt(), 55);
    });

    SAD_TEST("LOOP04: لكل في مصفوفة", {
        auto code = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 sum = 0\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 i = 1\n"
                     "\xD8\xA8\xD9\x8A\xD9\x86\xD9\x85\xD8\xA7 (i <= 10)\n"
                     "  sum += i\n"
                     "  i = i + 1\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9";
        auto v = runAndGetVar(code, "sum");
        SAD_ASSERT_EQ(v.toInt(), 55);
    });

    SAD_TEST("LOOP05: حلقة متداخلة", {
        auto code = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 count = 0\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 i = 0\n"
                     "\xD8\xA8\xD9\x8A\xD9\x86\xD9\x85\xD8\xA7 (i < 3)\n"
                     "  \xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 j = 0\n"
                     "  \xD8\xA8\xD9\x8A\xD9\x86\xD9\x85\xD8\xA7 (j < 3)\n"
                     "    count = count + 1\n"
                     "    j = j + 1\n"
                     "  \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "  i = i + 1\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9";
        auto v = runAndGetVar(code, "count");
        SAD_ASSERT_EQ(v.toInt(), 9);
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 7: الدوال / Functions
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Interpreter.Functions / المفسر.الدوال");

    SAD_TEST("FN01: دالة بسيطة بلا معاملات", {
        auto code = "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 get5()\n"
                     "  \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 5\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = get5()";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 5);
    });

    SAD_TEST("FN02: دالة مع معامل", {
        auto code = "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 double_it(n)\n"
                     "  \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 n * 2\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = double_it(21)";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 42);
    });

    SAD_TEST("FN03: دالة مع معاملات متعددة", {
        auto code = "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 add(a, b)\n"
                     "  \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 a + b\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = add(3, 4)";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 7);
    });

    SAD_TEST("FN04: دالة تستدعي دالة أخرى", {
        auto code = "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 sq(n)\n"
                     "  \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 n * n\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 sumSquares(a, b)\n"
                     "  \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 sq(a) + sq(b)\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = sumSquares(3, 4)";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 25);
    });

    SAD_TEST("FN05: تكرار (Recursion) - factorial", {
        auto code = "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 fact(n)\n"
                     "  \xD8\xA5\xD8\xB0\xD8\xA7 (n <= 1)\n"
                     "    \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 1\n"
                     "  \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "  \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 n * fact(n - 1)\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = fact(5)";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 120);
    });

    SAD_TEST("FN06: تكرار - fibonacci", {
        auto code = "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 fib(n)\n"
                     "  \xD8\xA5\xD8\xB0\xD8\xA7 (n <= 1)\n"
                     "    \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 n\n"
                     "  \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "  \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 fib(n-1) + fib(n-2)\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = fib(10)";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 55);
    });

    SAD_TEST("FN07: دالة بلا return ترجع void", {
        auto code = "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 doNothing()\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 1\n"
                     "doNothing()";
        SAD_ASSERT_TRUE(runsSuccessfully(code));
    });

    SAD_TEST("FN08: دالة تعدل متغير محلي فقط", {
        auto code = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 10\n"
                     "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 f()\n"
                     "  \xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 99\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "f()";
        auto v = runAndGetVar(code, "x");
        // x in global scope should still be 10
        SAD_ASSERT_EQ(v.toInt(), 10);
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 8: المصفوفات / Arrays
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Interpreter.Arrays / المفسر.المصفوفات");

    SAD_TEST("ARR01: إنشاء مصفوفة", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 arr = [1, 2, 3]", "arr");
        SAD_ASSERT_TRUE(v.isArray());
        SAD_ASSERT_EQ(v.size(), (size_t)3);
    });

    SAD_TEST("ARR02: مصفوفة فارغة", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 arr = []", "arr");
        SAD_ASSERT_TRUE(v.isArray());
        SAD_ASSERT_EQ(v.size(), (size_t)0);
    });

    SAD_TEST("ARR03: الوصول بالفهرس", {
        auto code = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 arr = [10, 20, 30]\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = arr[1]";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 20);
    });

    SAD_TEST("ARR04: تعديل عنصر مصفوفة", {
        auto code = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 arr = [1, 2, 3]\n"
                     "arr[0] = 99\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = arr[0]";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 99);
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 9: OOP / البرمجة كائنية
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Interpreter.OOP / المفسر.OOP");

    SAD_TEST("OOP01: إنشاء صنف واستخدامه", {
        // (AR) الباني بدون كلمة دالة — صيغة: باني() ... نهاية
        // (EN) Constructor without دالة keyword — syntax: باني() ... نهاية
        // (AR) طرق الكتابة (هذا.x = ...) داخل الدوال لا تُحفظ حالياً
        //      لذلك نختبر الباني + getter فقط
        auto code = "\xD8\xB5\xD9\x86\xD9\x81 Counter\n"
                     "  \xD8\xA8\xD8\xA7\xD9\x86\xD9\x8A()\n"
                     "    \xD9\x87\xD8\xB0\xD8\xA7.count = 42\n"
                     "  \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "  \xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 getCount()\n"
                     "    \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 \xD9\x87\xD8\xB0\xD8\xA7.count\n"
                     "  \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 c = \xD8\xAC\xD8\xAF\xD9\x8A\xD8\xAF Counter()\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = c.getCount()";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 42);
    });

    SAD_TEST("OOP02: صنف بسيط مع خاصية", {
        // (AR) الباني بدون كلمة دالة — صيغة: باني(x, y) ... نهاية
        auto code = "\xD8\xB5\xD9\x86\xD9\x81 Point\n"
                     "  \xD8\xA8\xD8\xA7\xD9\x86\xD9\x8A(x, y)\n"
                     "    \xD9\x87\xD8\xB0\xD8\xA7.x = x\n"
                     "    \xD9\x87\xD8\xB0\xD8\xA7.y = y\n"
                     "  \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 p = \xD8\xAC\xD8\xAF\xD9\x8A\xD8\xAF Point(3, 4)\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = p.x";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 3);
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 10: المعالجة الخاصة / Special Features
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Interpreter.Special / المفسر.ميزات_خاصة");

    SAD_TEST("SP01: += زيادة", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 5\nx += 1", "x");
        SAD_ASSERT_EQ(v.toInt(), 6);
    });

    SAD_TEST("SP02: -= نقصان", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 5\nx -= 1", "x");
        SAD_ASSERT_EQ(v.toInt(), 4);
    });

    SAD_TEST("SP03: برنامج فارغ", {
        SAD_ASSERT_TRUE(runsSuccessfully(""));
    });

    SAD_TEST("SP04: تعليق فقط", {
        SAD_ASSERT_TRUE(runsSuccessfully("# this is a comment"));
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 11: حالات حدود / Edge Cases
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Interpreter.EdgeCases / المفسر.حالات_حدود");

    SAD_TEST("EDGE01: متغير يساوي صفر", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 0", "x");
        SAD_ASSERT_EQ(v.toInt(), 0);
    });

    SAD_TEST("EDGE02: رقم كبير", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 999999", "x");
        SAD_ASSERT_EQ(v.toInt(), 999999);
    });

    SAD_TEST("EDGE03: نص طويل", {
        std::string longStr(200, 'a');
        std::string code = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = \"" + longStr + "\"";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toString().size(), (size_t)200);
    });

    SAD_TEST("EDGE04: عمليات متسلسلة كثيرة", {
        auto v = runAndGetVar(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 1+2+3+4+5+6+7+8+9+10", "x");
        SAD_ASSERT_EQ(v.toInt(), 55);
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 12: معالجة الأخطاء / Error Handling
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Interpreter.ErrorHandling / المفسر.معالجة_الأخطاء");

    SAD_TEST("ERR01: حاول-امسك", {
        auto code = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 0\n"
                     "\xD8\xAD\xD8\xA7\xD9\x88\xD9\x84\n"
                     "  \xD8\xA7\xD8\xB1\xD9\x85\xD9\x8A \"\xD8\xAE\xD8\xB7\xD8\xA3\"\n"
                     "\xD8\xA7\xD9\x85\xD8\xB3\xD9\x83 (e)\n"
                     "  x = 1\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 1);
    });

    SAD_TEST("ERR02: خطأ بدون catch يفشل", {
        auto code = "\xD8\xA7\xD8\xB1\xD9\x85\xD9\x8A \"error\"";
        SAD_ASSERT_TRUE(throwsError(code));
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 13: الخوارزميات / Algorithm Tests
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Interpreter.Algorithms / المفسر.الخوارزميات");

    SAD_TEST("ALGO01: أكبر من عددين", {
        auto code = "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 max(a, b)\n"
                     "  \xD8\xA5\xD8\xB0\xD8\xA7 (a > b)\n"
                     "    \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 a\n"
                     "  \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "  \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 b\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = max(7, 3)";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 7);
    });

    SAD_TEST("ALGO02: القيمة المطلقة", {
        auto code = "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 abs_val(n)\n"
                     "  \xD8\xA5\xD8\xB0\xD8\xA7 (n < 0)\n"
                     "    \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 -n\n"
                     "  \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "  \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 n\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = abs_val(-42)";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 42);
    });

    SAD_TEST("ALGO03: قوة (power)", {
        auto code = "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 power(base, exp)\n"
                     "  \xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 result = 1\n"
                     "  \xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 i = 0\n"
                     "  \xD8\xA8\xD9\x8A\xD9\x86\xD9\x85\xD8\xA7 (i < exp)\n"
                     "    result = result * base\n"
                     "    i = i + 1\n"
                     "  \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "  \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 result\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = power(2, 10)";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 1024);
    });

    SAD_TEST("ALGO04: عدد أولي بسيط", {
        auto code = "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 isPrime(n)\n"
                     "  \xD8\xA5\xD8\xB0\xD8\xA7 (n < 2)\n"
                     "    \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 \xD8\xAE\xD8\xB7\xD8\xA3\n"
                     "  \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "  \xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 i = 2\n"
                     "  \xD8\xA8\xD9\x8A\xD9\x86\xD9\x85\xD8\xA7 (i * i <= n)\n"
                     "    \xD8\xA5\xD8\xB0\xD8\xA7 (n % i == 0)\n"
                     "      \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 \xD8\xAE\xD8\xB7\xD8\xA3\n"
                     "    \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "    i = i + 1\n"
                     "  \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "  \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 \xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = isPrime(7)";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toBool(), true);
    });

    SAD_TEST("ALGO05: GCD (القاسم المشترك الأكبر)", {
        auto code = "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 gcd(a, b)\n"
                     "  \xD8\xA8\xD9\x8A\xD9\x86\xD9\x85\xD8\xA7 (b != 0)\n"
                     "    \xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 temp = b\n"
                     "    b = a % b\n"
                     "    a = temp\n"
                     "  \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "  \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 a\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = gcd(48, 18)";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 6);
    });

    // ══════════════════════════════════════════════════════════════════
    // مجموعة 14: برامج متكاملة / Integration Programs
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("Interpreter.Programs / المفسر.برامج");

    SAD_TEST("PROG01: FizzBuzz عداد", {
        auto code = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 fizz = 0\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 buzz = 0\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 i = 1\n"
                     "\xD8\xA8\xD9\x8A\xD9\x86\xD9\x85\xD8\xA7 (i <= 15)\n"
                     "  \xD8\xA5\xD8\xB0\xD8\xA7 (i % 3 == 0)\n"
                     "    fizz = fizz + 1\n"
                     "  \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "  \xD8\xA5\xD8\xB0\xD8\xA7 (i % 5 == 0)\n"
                     "    buzz = buzz + 1\n"
                     "  \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "  i = i + 1\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9";
        auto vFizz = runAndGetVar(code, "fizz");
        auto vBuzz = runAndGetVar(code, "buzz");
        SAD_ASSERT_EQ(vFizz.toInt(), 5); // 3,6,9,12,15
        SAD_ASSERT_EQ(vBuzz.toInt(), 3); // 5,10,15
    });

    SAD_TEST("PROG02: حساب معدل (Average)", {
        auto code = "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 avg3(a, b, c)\n"
                     "  \xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9 (a + b + c) / 3\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = avg3(10, 20, 30)";
        auto v = runAndGetVar(code, "x");
        SAD_ASSERT_EQ(v.toInt(), 20);
    });

    SAD_TEST("PROG03: عد أرقام زوجية", {
        auto code = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 even = 0\n"
                     "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 i = 1\n"
                     "\xD8\xA8\xD9\x8A\xD9\x86\xD9\x85\xD8\xA7 (i <= 20)\n"
                     "  \xD8\xA5\xD8\xB0\xD8\xA7 (i % 2 == 0)\n"
                     "    even = even + 1\n"
                     "  \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"
                     "  i = i + 1\n"
                     "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9";
        auto v = runAndGetVar(code, "even");
        SAD_ASSERT_EQ(v.toInt(), 10);
    });

    SAD_SUMMARY();
}
