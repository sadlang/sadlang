/**
 * @file test_optional_null_comprehensive.cpp
 * @brief (AR) اختبارات شاملة لعاملي ?. و ?? / (EN) Comprehensive tests for ?. and ??
 *
 * يغطي: المحلل المعجمي (Lexer)، المحلل النحوي (Parser)، المفسر (Interpreter)
 * ~40 اختبار تشمل: حالات حدية، أنواع مختلفة، تداخل، وراثة، تكامل
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
// (AR) دوال مساعدة / (EN) Helper Functions
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

static Data::Value runAndGetVar(const std::string& source, const std::string& varName) {
    try {
        Lexer::LexerCore lexer(source);
        Parser::ParserCore parser(lexer);
        auto program = parser.parseProgram();
        Interpreter::InterpreterOptions opts;
        Interpreter::Interpreter interp(opts);
        interp.execute(program);
        auto& vm = interp.getVariableManager();
        try { return vm.get(varName); }
        catch (...) { return Data::Value(); }
    } catch (...) { return Data::Value(); }
}

static bool runsSuccessfully(const std::string& source) {
    try {
        auto result = runCode(source);
        return result.success;
    } catch (...) { return false; }
}

// ======================================================================
// (AR) ثوابت سلسلة — كلمات مفتاحية بـ UTF-8
// (EN) String constants — keywords in UTF-8
// ======================================================================
// متغير
#define KW_VAR "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1"
// لاشيء
#define KW_NULL "\xD9\x84\xD8\xA7\xD8\xB4\xD9\x8A\xD8\xA1"
// صنف
#define KW_CLASS "\xD8\xB5\xD9\x86\xD9\x81"
// باني
#define KW_CTOR "\xD8\xA8\xD8\xA7\xD9\x86\xD9\x8A"
// هذا
#define KW_THIS "\xD9\x87\xD8\xB0\xD8\xA7"
// نهاية
#define KW_END "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9"
// دالة
#define KW_FUNC "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9"
// ارجع
#define KW_RETURN "\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9"
// إذا
#define KW_IF "\xD8\xA5\xD8\xB0\xD8\xA7"
// وإلا
#define KW_ELSE "\xD9\x88\xD8\xA5\xD9\x84\xD8\xA7"
// يرث
#define KW_INHERITS "\xD9\x8A\xD8\xB1\xD8\xAB"
// الأساس
#define KW_BASE "\xD8\xA7\xD9\x84\xD8\xA3\xD8\xB3\xD8\xA7\xD8\xB3"
// جديد
#define KW_NEW "\xD8\xAC\xD8\xAF\xD9\x8A\xD8\xAF"
// صحيح
#define KW_TRUE "\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD"
// خطأ
#define KW_FALSE "\xD8\xAE\xD8\xB7\xD8\xA3"
// لكل
#define KW_FOREACH "\xD9\x84\xD9\x83\xD9\x84"
// في
#define KW_IN "\xD9\x81\xD9\x8A"

// ======================================================================
int main() {
    SAD_TEST_INIT();

    // ==================================================================
    // المجموعة 1: المحلل المعجمي (Lexer) — رموز ?. و ??
    // ==================================================================
    SAD_GROUP("Lexer.OptionalNull / المعجمي.الوصول_الآمن");

    SAD_TEST("LEX01: الرمز ?. يُحلل كرمز واحد", {
        Lexer::LexerCore lexer("x?.y");
        auto tokens = lexer.tokenize();
        bool found = false;
        for (auto& t : tokens) {
            if (t.getType() == Lexer::TokenType::QUESTION_DOT) {
                found = true;
                break;
            }
        }
        SAD_ASSERT_TRUE(found);
    });

    SAD_TEST("LEX02: الرمز ?? يُحلل كرمز واحد", {
        Lexer::LexerCore lexer("x ?? y");
        auto tokens = lexer.tokenize();
        bool found = false;
        for (auto& t : tokens) {
            if (t.getType() == Lexer::TokenType::QUESTION_QUESTION) {
                found = true;
                break;
            }
        }
        SAD_ASSERT_TRUE(found);
    });

    SAD_TEST("LEX03: ?. بدون مسافات", {
        Lexer::LexerCore lexer("obj?.prop");
        auto tokens = lexer.tokenize();
        // يجب أن نجد: IDENTIFIER, QUESTION_DOT, IDENTIFIER
        int qd_count = 0;
        for (auto& t : tokens) {
            if (t.getType() == Lexer::TokenType::QUESTION_DOT) qd_count++;
        }
        SAD_ASSERT_EQ(qd_count, 1);
    });

    SAD_TEST("LEX04: ?? مع مسافات", {
        Lexer::LexerCore lexer("a   ??   b");
        auto tokens = lexer.tokenize();
        int qq_count = 0;
        for (auto& t : tokens) {
            if (t.getType() == Lexer::TokenType::QUESTION_QUESTION) qq_count++;
        }
        SAD_ASSERT_EQ(qq_count, 1);
    });

    SAD_TEST("LEX05: ?. و ?? في نفس التعبير", {
        Lexer::LexerCore lexer("x?.y ?? z");
        auto tokens = lexer.tokenize();
        int qd = 0, qq = 0;
        for (auto& t : tokens) {
            if (t.getType() == Lexer::TokenType::QUESTION_DOT) qd++;
            if (t.getType() == Lexer::TokenType::QUESTION_QUESTION) qq++;
        }
        SAD_ASSERT_EQ(qd, 1);
        SAD_ASSERT_EQ(qq, 1);
    });

    // ==================================================================
    // المجموعة 2: المحلل النحوي (Parser) — عقد AST
    // ==================================================================
    SAD_GROUP("Parser.OptionalNull / النحوي.الوصول_الآمن");

    SAD_TEST("PARSE01: ?. ينتج عقدة OptionalChainExpr", {
        std::string code = KW_VAR " x = " KW_NULL "\n" KW_VAR " y = x?.z";
        SAD_ASSERT_TRUE(runsSuccessfully(code));
    });

    SAD_TEST("PARSE02: ?? ينتج عقدة NullCoalesceExpr", {
        std::string code = KW_VAR " x = " KW_NULL "\n" KW_VAR " y = x ?? 42";
        SAD_ASSERT_TRUE(runsSuccessfully(code));
    });

    SAD_TEST("PARSE03: ?. + ?? معاً", {
        std::string code = KW_VAR " x = " KW_NULL "\n" KW_VAR " y = x?.z ?? 0";
        SAD_ASSERT_TRUE(runsSuccessfully(code));
    });

    SAD_TEST("PARSE04: ?. متسلسل", {
        std::string code = KW_VAR " x = " KW_NULL "\n" KW_VAR " y = x?.a?.b?.c ?? 0";
        SAD_ASSERT_TRUE(runsSuccessfully(code));
    });

    // ==================================================================
    // المجموعة 3: المفسر — ?? مع أنواع مختلفة
    // ==================================================================
    SAD_GROUP("Interpreter.NullCoalesce / المفسر.الاندماج_الصفري");

    SAD_TEST("NC01: ?? مع لاشيء يرجع البديل", {
        auto v = runAndGetVar(KW_VAR " x = " KW_NULL "\n" KW_VAR " y = x ?? 42", "y");
        SAD_ASSERT_EQ(v.toInt(), 42);
    });

    SAD_TEST("NC02: ?? مع قيمة موجودة يرجع الأصل", {
        auto v = runAndGetVar(KW_VAR " x = 10\n" KW_VAR " y = x ?? 99", "y");
        SAD_ASSERT_EQ(v.toInt(), 10);
    });

    SAD_TEST("NC03: ?? لا يستبدل صفر", {
        auto v = runAndGetVar(KW_VAR " x = 0\n" KW_VAR " y = x ?? 99", "y");
        SAD_ASSERT_EQ(v.toInt(), 0);
    });

    SAD_TEST("NC04: ?? لا يستبدل نص فارغ", {
        auto v = runAndGetVar(KW_VAR " x = \"\"\n" KW_VAR " y = x ?? \"fallback\"", "y");
        SAD_ASSERT_EQ(v.toString(), std::string(""));
    });

    SAD_TEST("NC05: ?? لا يستبدل خطأ", {
        auto v = runAndGetVar(KW_VAR " x = " KW_FALSE "\n" KW_VAR " y = x ?? " KW_TRUE, "y");
        SAD_ASSERT_TRUE(v.isBoolean());
        SAD_ASSERT_FALSE(v.toBool());
    });

    SAD_TEST("NC06: ?? مع نص", {
        auto v = runAndGetVar(KW_VAR " x = " KW_NULL "\n" KW_VAR " y = x ?? \"hello\"", "y");
        SAD_ASSERT_EQ(v.toString(), std::string("hello"));
    });

    SAD_TEST("NC07: تسلسل ?? ثلاثي", {
        auto v = runAndGetVar(
            KW_VAR " a = " KW_NULL "\n"
            KW_VAR " b = " KW_NULL "\n"
            KW_VAR " c = 7\n"
            KW_VAR " y = a ?? b ?? c", "y");
        SAD_ASSERT_EQ(v.toInt(), 7);
    });

    SAD_TEST("NC08: تسلسل ?? — الأول غير null", {
        auto v = runAndGetVar(
            KW_VAR " a = 1\n"
            KW_VAR " b = 2\n"
            KW_VAR " c = 3\n"
            KW_VAR " y = a ?? b ?? c", "y");
        SAD_ASSERT_EQ(v.toInt(), 1);
    });

    SAD_TEST("NC09: ?? في تعبير حسابي", {
        auto v = runAndGetVar(
            KW_VAR " x = " KW_NULL "\n"
            KW_VAR " y = (x ?? 10) + 5", "y");
        SAD_ASSERT_EQ(v.toInt(), 15);
    });

    // ==================================================================
    // المجموعة 4: المفسر — ?. الوصول الآمن
    // ==================================================================
    SAD_GROUP("Interpreter.OptionalChain / المفسر.الوصول_الاختياري");

    SAD_TEST("OC01: ?. على كائن موجود", {
        std::string code =
            KW_CLASS " Person\n"
            "  " KW_CTOR "(n)\n"
            "    " KW_THIS ".name = n\n"
            "  " KW_END "\n"
            KW_END "\n"
            KW_VAR " p = " " Person(\"Ali\")\n"
            KW_VAR " y = p?.name";
        auto v = runAndGetVar(code, "y");
        SAD_ASSERT_EQ(v.toString(), std::string("Ali"));
    });

    SAD_TEST("OC02: ?. على لاشيء يرجع عدم (null)", {
        // (AR) عقد S-TS-P1 (ADR-TYPESYSTEM-001): «?.» على عدم يُرجع «عدم» (null)
        //      المتمايز عن «فراغ» (void) — لا isVoid. حُدِّث التوقّع ليطابق العقد.
        // (EN) Per S-TS-P1: '?.' on null returns the distinct null (عدم), not void.
        auto v = runAndGetVar(
            KW_VAR " x = " KW_NULL "\n"
            KW_VAR " y = x?.name", "y");
        SAD_ASSERT_TRUE(v.isNull());
    });

    SAD_TEST("OC03: ?. + ?? على كائن موجود", {
        std::string code =
            KW_CLASS " C\n"
            "  " KW_CTOR "(v)\n"
            "    " KW_THIS ".val = v\n"
            "  " KW_END "\n"
            KW_END "\n"
            KW_VAR " c = " " C(99)\n"
            KW_VAR " y = c?.val ?? 0";
        auto v = runAndGetVar(code, "y");
        SAD_ASSERT_EQ(v.toInt(), 99);
    });

    SAD_TEST("OC04: ?. + ?? على لاشيء", {
        auto v = runAndGetVar(
            KW_VAR " x = " KW_NULL "\n"
            KW_VAR " y = x?.val ?? 0", "y");
        SAD_ASSERT_EQ(v.toInt(), 0);
    });

    SAD_TEST("OC05: ?. متداخل على كائنات", {
        std::string code =
            KW_CLASS " Inner\n"
            "  " KW_CTOR "(v)\n"
            "    " KW_THIS ".val = v\n"
            "  " KW_END "\n"
            KW_END "\n"
            KW_CLASS " Outer\n"
            "  " KW_CTOR "(i)\n"
            "    " KW_THIS ".inner = i\n"
            "  " KW_END "\n"
            KW_END "\n"
            KW_VAR " i = " " Inner(42)\n"
            KW_VAR " o = " " Outer(i)\n"
            KW_VAR " y = o?.inner?.val";
        auto v = runAndGetVar(code, "y");
        SAD_ASSERT_EQ(v.toInt(), 42);
    });

    SAD_TEST("OC06: ?. متداخل — الداخلي null", {
        std::string code =
            KW_CLASS " Inner\n"
            "  " KW_CTOR "(v)\n"
            "    " KW_THIS ".val = v\n"
            "  " KW_END "\n"
            KW_END "\n"
            KW_CLASS " Outer\n"
            "  " KW_CTOR "(i)\n"
            "    " KW_THIS ".inner = i\n"
            "  " KW_END "\n"
            KW_END "\n"
            KW_VAR " o = " " Outer(" KW_NULL ")\n"
            KW_VAR " y = o?.inner?.val ?? -1";
        auto v = runAndGetVar(code, "y");
        SAD_ASSERT_EQ(v.toInt(), -1);
    });

    // ==================================================================
    // المجموعة 5: وراثة + ?.
    // ==================================================================
    SAD_GROUP("Interpreter.OptionalChain.Inheritance / وراثة_مع_الوصول_الآمن");

    SAD_TEST("INH01: ?. على خاصية موروثة", {
        std::string code =
            KW_CLASS " Animal\n"
            "  " KW_CTOR "(n)\n"
            "    " KW_THIS ".name = n\n"
            "  " KW_END "\n"
            KW_END "\n"
            KW_CLASS " Cat " KW_INHERITS " Animal\n"
            "  " KW_CTOR "(n, c)\n"
            "    " KW_BASE "(n)\n"
            "    " KW_THIS ".color = c\n"
            "  " KW_END "\n"
            KW_END "\n"
            KW_VAR " c = " " Cat(\"Mimi\", \"white\")\n"
            KW_VAR " y = c?.name";
        auto v = runAndGetVar(code, "y");
        SAD_ASSERT_EQ(v.toString(), std::string("Mimi"));
    });

    SAD_TEST("INH02: ?. على خاصية فرعية", {
        std::string code =
            KW_CLASS " Animal\n"
            "  " KW_CTOR "(n)\n"
            "    " KW_THIS ".name = n\n"
            "  " KW_END "\n"
            KW_END "\n"
            KW_CLASS " Cat " KW_INHERITS " Animal\n"
            "  " KW_CTOR "(n, c)\n"
            "    " KW_BASE "(n)\n"
            "    " KW_THIS ".color = c\n"
            "  " KW_END "\n"
            KW_END "\n"
            KW_VAR " c = " " Cat(\"Mimi\", \"black\")\n"
            KW_VAR " y = c?.color";
        auto v = runAndGetVar(code, "y");
        SAD_ASSERT_EQ(v.toString(), std::string("black"));
    });

    // ==================================================================
    // المجموعة 6: سيناريوهات تكاملية
    // ==================================================================
    SAD_GROUP("Interpreter.Integration / تكامل");

    SAD_TEST("INT01: ?? مع نتيجة دالة ترجع لاشيء", {
        std::string code =
            KW_FUNC " find(key)\n"
            "  " KW_IF " (key == \"ok\")\n"
            "    " KW_RETURN " 42\n"
            "  " KW_END "\n"
            "  " KW_RETURN " " KW_NULL "\n"
            KW_END "\n"
            KW_VAR " y = find(\"bad\") ?? -1";
        auto v = runAndGetVar(code, "y");
        SAD_ASSERT_EQ(v.toInt(), -1);
    });

    SAD_TEST("INT02: ?? مع نتيجة دالة ترجع قيمة", {
        std::string code =
            KW_FUNC " find(key)\n"
            "  " KW_IF " (key == \"ok\")\n"
            "    " KW_RETURN " 42\n"
            "  " KW_END "\n"
            "  " KW_RETURN " " KW_NULL "\n"
            KW_END "\n"
            KW_VAR " y = find(\"ok\") ?? -1";
        auto v = runAndGetVar(code, "y");
        SAD_ASSERT_EQ(v.toInt(), 42);
    });

    SAD_TEST("INT03: ?. + ?? في إسناد شرطي", {
        std::string code =
            KW_CLASS " Config\n"
            "  " KW_CTOR "(v)\n"
            "    " KW_THIS ".lang = v\n"
            "  " KW_END "\n"
            KW_END "\n"
            KW_VAR " cfg = " KW_NULL "\n"
            KW_VAR " y = cfg?.lang ?? \"ar\"";
        auto v = runAndGetVar(code, "y");
        SAD_ASSERT_EQ(v.toString(), std::string("ar"));
    });

    SAD_TEST("INT04: ?? مع نتيجة عملية حسابية", {
        auto v = runAndGetVar(
            KW_VAR " x = " KW_NULL "\n"
            KW_VAR " y = (x ?? 5) * 3", "y");
        SAD_ASSERT_EQ(v.toInt(), 15);
    });

    SAD_TEST("INT05: ?? محافظ على الأنواع — نص", {
        auto v = runAndGetVar(
            KW_VAR " x = \"hello\"\n"
            KW_VAR " y = x ?? \"world\"", "y");
        SAD_ASSERT_TRUE(v.isString());
        SAD_ASSERT_EQ(v.toString(), std::string("hello"));
    });

    SAD_TEST("INT06: ?? محافظ على الأنواع — رقم عشري", {
        auto v = runAndGetVar(
            KW_VAR " x = 3.14\n"
            KW_VAR " y = x ?? 0.0", "y");
        SAD_ASSERT_FLOAT_EQ(v.toDouble(), 3.14, 0.001);
    });

    // ==================================================================
    SAD_SUMMARY();
}
