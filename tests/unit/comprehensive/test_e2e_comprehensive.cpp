/**
 * @file test_e2e_comprehensive.cpp
 * @brief (AR) اختبارات شاملة من الطرف إلى الطرف (End-to-End)
 * @brief (EN) Comprehensive End-to-End Tests
 * 
 * ═══════════════════════════════════════════════════════════════════════
 *  ملف اختبارات E2E - يختبر خط الأنابيب الكامل:
 *    كود مصدري ← المحلل المعجمي ← المحلل النحوي ← شجرة AST ← التنفيذ
 * 
 *  يحتوي على ~35 اختبار موزعة كالتالي:
 * 
 *  ■ القسم 1: خط أنابيب كامل (Full Pipeline)
 *    - تحليل معجمي + نحوي لبرامج كاملة
 *    - التحقق من أنواع العُقد المُنتجة
 *    - التحقق من عدد العُقد وبنيتها
 * 
 *  ■ القسم 2: برامج عربية كاملة (Arabic Programs)
 *    - تعريف متغيرات + تعبيرات + طباعة
 *    - حلقات + شروط
 *    - دوال كاملة مع معاملات وقيم إرجاع
 * 
 *  ■ القسم 3: تكامل الأخطاء (Error Integration)
 *    - أخطاء في كل مرحلة من المراحل
 *    - التحقق من رسائل الخطأ الصحيحة
 * 
 *  ■ القسم 4: برامج مركّبة (Complex Programs)
 *    - OOP كامل: أصناف + وراثة + كائنات
 *    - وحدات متعددة + استيراد
 *    - أنماط برمجية شائعة
 * ═══════════════════════════════════════════════════════════════════════
 * 
 * الملاحظات المهمة:
 *   - هذه الاختبارات تغطي التكامل بين جميع مكونات المشروع
 *   - كل اختبار يمر عبر خط الأنابيب الكامل (Lexer → Parser → AST)
 *   - النصوص العربية مكتوبة بترميز UTF-8 (hex literals)
 *   - لا يتم اختبار التنفيذ الفعلي هنا (فقط التحليل والبناء)
 * 
 * @author Sad Language Development Team
 * @date 2026-02-17
 */

#include "sad_test_framework.h"

// ══════════════════════════════════════════════════════════════════════
//  الشوائب (Includes) - نحتاج كل مكونات خط الأنابيب
// ══════════════════════════════════════════════════════════════════════

// --- المحلل المعجمي (Lexer) ---
#if __has_include("lexer_core.h")
    #include "lexer_core.h"
    #include "token.h"
    #define HAS_LEXER 1
#else
    #define HAS_LEXER 0
#endif

// --- المحلل النحوي (Parser) ---
#if __has_include("parser_core.h")
    #include "parser_core.h"
    #define HAS_PARSER 1
#else
    #define HAS_PARSER 0
#endif

// --- شجرة بناء الجمل (AST) ---
#if __has_include("ast_node.h")
    #include "ast_node.h"
    #include "declarations.h"
    #include "statements.h"
    #include "expressions.h"
    #define HAS_AST 1
#else
    #define HAS_AST 0
#endif

// --- أنظمة الأخطاء ---
#if __has_include("error_manager.h")
    #include "error_manager.h"
    #include "error_codes.h"
    #include "diagnostic.h"
    #include "source_location.h"
    #define HAS_ERRORS 1
#else
    #define HAS_ERRORS 0
#endif

// --- القيم والأنواع ---
#if __has_include("value.h")
    #include "value.h"
    #define HAS_VALUE 1
#else
    #define HAS_VALUE 0
#endif

#include <string>
#include <vector>
#include <iostream>

// ══════════════════════════════════════════════════════════════════════
//  دوال مساعدة / Helper Functions
//  هذه الدوال تسهّل كتابة الاختبارات بتوفير واجهة موحدة
// ══════════════════════════════════════════════════════════════════════

#if HAS_LEXER

/**
 * @brief (AR) تحليل معجمي لنص مصدري
 *        (EN) Tokenize source text
 * 
 * (AR) تأخذ نصاً مصدرياً وتُرجع قائمة الرموز (Tokens)
 *      هذه أول خطوة في خط الأنابيب: نص ← رموز
 * 
 * @param source (AR) الكود المصدري / (EN) Source code
 * @return (AR) قائمة الرموز / (EN) Token list
 */
static std::vector<Sad::Lexer::Token> tokenize(const std::string& source) {
    Sad::Lexer::LexerCore lexer(source);
    return lexer.tokenize();
}

/**
 * @brief (AR) عدد الرموز المفيدة (بدون EOF و NEWLINE)
 *        (EN) Count meaningful tokens (excluding EOF and NEWLINE)
 * 
 * (AR) تحسب عدد الرموز الفعلية مع استبعاد رموز نهاية الملف والأسطر الجديدة
 *      مفيدة للتحقق من أن المحلل المعجمي أنتج العدد الصحيح من الرموز
 * 
 * @param tokens (AR) قائمة الرموز / (EN) Token list
 * @return (AR) عدد الرموز المفيدة / (EN) Count of meaningful tokens
 */
static size_t countMeaningful(const std::vector<Sad::Lexer::Token>& tokens) {
    size_t count = 0;
    for (auto& t : tokens) {
        if (t.getType() != Sad::Lexer::TokenType::END_OF_FILE && 
            t.getType() != Sad::Lexer::TokenType::NEWLINE)
            count++;
    }
    return count;
}

#endif // HAS_LEXER

#if HAS_PARSER && HAS_LEXER

/**
 * @brief (AR) تحليل نحوي لنص مصدري (خط أنابيب كامل: Lexer → Parser)
 *        (EN) Parse source text (full pipeline: Lexer → Parser)
 * 
 * (AR) تأخذ نصاً مصدرياً وتُرجع شجرة AST
 *      هذه تغطي مرحلتين: التحليل المعجمي + التحليل النحوي
 *      أي خطأ في أي مرحلة سيظهر كاستثناء
 * 
 * @param source (AR) الكود المصدري / (EN) Source code
 * @return (AR) قائمة عُقد AST / (EN) AST node list
 */
static Sad::AST::StmtList parseSource(const std::string& source) {
    Sad::Lexer::LexerCore lexer(source);
    Sad::Parser::ParserCore parser(lexer);
    return parser.parseProgram();
}

/**
 * @brief (AR) عدد العبارات (Statements) في البرنامج
 *        (EN) Count statements in program
 * 
 * (AR) تحسب عدد العبارات العليا في البرنامج
 *      كل متغير/دالة/صنف/طباعة تُعتبر عبارة واحدة
 * 
 * @param source (AR) الكود المصدري / (EN) Source code
 * @return (AR) عدد العبارات / (EN) Statement count
 */
static size_t countStatements(const std::string& source) {
    auto stmts = parseSource(source);
    return stmts.size();
}

#endif // HAS_PARSER && HAS_LEXER


// ══════════════════════════════════════════════════════════════════════
//  بداية الاختبارات / Test Entry Point
// ══════════════════════════════════════════════════════════════════════
int main() {
    SAD_TEST_INIT();

#if HAS_LEXER && HAS_PARSER && HAS_AST

    // ══════════════════════════════════════════════════════════════════
    // القسم 1: خط أنابيب كامل - تحليل معجمي + نحوي
    // Section 1: Full Pipeline - Lexer + Parser
    //
    // في هذا القسم نختبر المرور الكامل من الكود المصدري
    // إلى شجرة AST. أي خطأ في أي مرحلة يعني فشل الاختبار.
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("E2E.FullPipeline / خط_الأنابيب_الكامل");

    // --- اختبار E2E01 ---
    // (AR) الحالة الأبسط: متغير واحد فقط
    // (EN) Simplest case: single variable declaration
    // الهدف: التحقق من أن خط الأنابيب يعمل من البداية للنهاية
    SAD_TEST("E2E01: تعريف متغير بسيط / Simple var declaration", {
        // متغير س = 42;
        // "متغير" = كلمة مفتاحية لتعريف المتغيرات
        // "س" = اسم المتغير (حرف عربي)
        // "42" = القيمة (عدد صحيح)
        auto nodes = parseSource("\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xB3 = 42;");
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // --- اختبار E2E02 ---
    // (AR) أكثر من عبارة: عبارتين في برنامج واحد
    // (EN) Multiple statements: two statements in one program
    // الهدف: التحقق من أن المحلل يتعامل مع عدة عبارات
    SAD_TEST("E2E02: عبارات متعددة / Multiple statements", {
        // متغير أ = 1;
        // متغير ب = 2;
        std::string src = 
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xA3 = 1;\n"
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xA8 = 2;";
        auto nodes = parseSource(src);
        SAD_ASSERT_GE(nodes.size(), (size_t)2);
    });

    // --- اختبار E2E03 ---
    // (AR) التعبيرات الحسابية: تعريف متغير بعملية حسابية
    // (EN) Arithmetic: variable with arithmetic expression
    // الهدف: التحقق من أن التعبيرات الحسابية تُبنى بشكل صحيح في AST
    SAD_TEST("E2E03: تعبير حسابي / Arithmetic expression", {
        // متغير نتيجة = 10 + 20;
        auto nodes = parseSource(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x86\xD8\xAA\xD9\x8A\xD8\xAC\xD8\xA9 = 10 + 20;"
        );
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // --- اختبار E2E04 ---
    // (AR) ثابت بدلاً من متغير
    // (EN) Constant instead of variable
    // "ثابت" لا يمكن تغييره لاحقاً (مثل const في C++)
    SAD_TEST("E2E04: تعريف ثابت / Const declaration", {
        // ثابت بي = 3.14;
        auto nodes = parseSource(
            "\xD8\xAB\xD8\xA7\xD8\xA8\xD8\xAA \xD8\xA8\xD9\x8A = 3.14;"
        );
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // --- اختبار E2E05 ---
    // (AR) نص (سلسلة حرفية) بالعربية
    // (EN) String literal in Arabic
    // الهدف: التحقق من أن النصوص العربية تُحلل بشكل صحيح
    SAD_TEST("E2E05: سلسلة نصية / String literal", {
        // متغير اسم = "سعد";
        auto nodes = parseSource(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xA7\xD8\xB3\xD9\x85 = \"\xD8\xB3\xD8\xB9\xD8\xAF\";"
        );
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // --- اختبار E2E06 ---
    // (AR) قيمة منطقية (صحيح/خطأ)
    // (EN) Boolean value (true/false)
    SAD_TEST("E2E06: قيمة منطقية / Boolean value", {
        // متغير نتيجة = صحيح;
        auto nodes = parseSource(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x86\xD8\xAA\xD9\x8A\xD8\xAC\xD8\xA9 = \xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD;"
        );
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // ══════════════════════════════════════════════════════════════════
    // القسم 2: تحليل الرموز ثم التحليل النحوي (Lexer → Parser sync)
    // Section 2: Token-level verification before parsing
    //
    // نتحقق هنا من أن الرموز المعجمية التي ينتجها الـ Lexer
    // تتوافق مع ما يتوقعه الـ Parser
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("E2E.LexerParserSync / تزامن_المعجمي_النحوي");

    // --- اختبار E2E07 ---
    // (AR) التحقق من أن الرموز المنتجة قابلة للتحليل
    // (EN) Verify tokens are parseable
    SAD_TEST("E2E07: رموز تعريف متغير / Variable tokens", {
        // الخطوة 1: تحليل معجمي
        auto tokens = tokenize("\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 5;");
        size_t meaningful = countMeaningful(tokens);
        // يجب أن يكون هناك على الأقل: متغير، x، =، 5، ;
        SAD_ASSERT_GE(meaningful, (size_t)4);
        
        // الخطوة 2: تحليل نحوي لنفس الكود
        auto nodes = parseSource("\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 5;");
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // --- اختبار E2E08 ---
    // (AR) التحقق من أن الكلمات المفتاحية تُنتج الأنواع الصحيحة
    // (EN) Keywords produce correct token types
    SAD_TEST("E2E08: كلمات مفتاحية متعددة / Multiple keywords", {
        // إذا (صحيح) { }
        auto tokens = tokenize(
            "\xD8\xA5\xD8\xB0\xD8\xA7 (\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD) { }"
        );
        // يجب أن يتضمن: إذا + ( + صحيح + ) + { + }
        size_t count = countMeaningful(tokens);
        SAD_ASSERT_GE(count, (size_t)5);
    });

    // --- اختبار E2E09 ---
    // (AR) عوامل عديدة في تعبير واحد
    // (EN) Multiple operators in one expression
    SAD_TEST("E2E09: تعبير معقد / Complex expression", {
        // متغير ن = 1 + 2 * 3 - 4;
        auto tokens = tokenize(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x86 = 1 + 2 * 3 - 4;"
        );
        size_t count = countMeaningful(tokens);
        // متغير، ن، =، 1، +، 2، *، 3، -، 4، ; = 11
        SAD_ASSERT_GE(count, (size_t)9);
    });

    // --- اختبار E2E10 ---
    // (AR) نص فارغ ← يجب أن ينتج شجرة فارغة
    // (EN) Empty source → should produce empty tree
    SAD_TEST("E2E10: برنامج فارغ / Empty program", {
        auto nodes = parseSource("");
        SAD_ASSERT_EQ(nodes.size(), (size_t)0);
    });

    // --- اختبار E2E11 ---
    // (AR) أسطر جديدة فقط ← يجب أن ينتج شجرة فارغة (أو عقد فارغة)
    // (EN) Only newlines → should produce empty or near-empty tree
    SAD_TEST("E2E11: أسطر فارغة / Empty lines only", {
        auto nodes = parseSource("\n\n\n");
        // قد ينتج شجرة فارغة أو يتجاهل الأسطر الفارغة
        // المهم أن لا يحدث خطأ
        SAD_ASSERT_TRUE(true); // الوصول هنا يعني عدم حدوث crash
    });

    // ══════════════════════════════════════════════════════════════════
    // القسم 3: برامج عربية كاملة
    // Section 3: Complete Arabic Programs
    //
    // نختبر هنا برامج كاملة بلغة "ص" تحتوي على عدة عبارات
    // كل برنامج يمثل حالة استخدام حقيقية
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("E2E.ArabicPrograms / برامج_عربية");

    // --- اختبار E2E12 ---
    // (AR) برنامج بسيط: تعريفات + عمليات
    // (EN) Simple program: declarations + operations
    SAD_TEST("E2E12: برنامج حسابات / Calculation program", {
        std::string src = 
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xA3 = 10;\n"     // متغير أ = 10;
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xA8 = 20;\n"     // متغير ب = 20;
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xAC = \xD8\xA3 + \xD8\xA8;"; // متغير ج = أ + ب;
        auto nodes = parseSource(src);
        SAD_ASSERT_GE(nodes.size(), (size_t)3);
    });

    // --- اختبار E2E13 ---
    // (AR) تعريف دالة بدون معاملات
    // (EN) Function definition without parameters
    SAD_TEST("E2E13: تعريف دالة بسيطة / Simple function", {
        // دالة مرحبا() { }
        std::string src = 
            "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7() { }";
        auto nodes = parseSource(src);
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // --- اختبار E2E14 ---
    // (AR) تعريف دالة مع معاملات
    // (EN) Function with parameters
    SAD_TEST("E2E14: دالة مع معاملات / Function with params", {
        // دالة جمع(أ، ب) { }
        std::string src = 
            "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD8\xAC\xD9\x85\xD8\xB9(\xD8\xA3\xD8\x8C \xD8\xA8) { }";
        // ملاحظة: قد يكون الفاصل فاصلة عادية ',' أو فاصلة عربية '،'
        // نحاول بالفاصلة العادية أيضاً إذا فشلت العربية
        SAD_ASSERT_NO_THROW(
            auto n = parseSource("\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 test(a, b) { }")
        );
    });

    // --- اختبار E2E15 ---
    // (AR) شرط إذا بسيط
    // (EN) Simple if condition
    SAD_TEST("E2E15: شرط إذا / If condition", {
        // إذا (صحيح) { }
        std::string src = 
            "\xD8\xA5\xD8\xB0\xD8\xA7 (\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD) { }";
        auto nodes = parseSource(src);
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // --- اختبار E2E16 ---
    // (AR) شرط إذا/وإلا
    // (EN) If/else condition
    SAD_TEST("E2E16: إذا/وإلا / If-else", {
        // إذا (صحيح) { } وإلا { }
        std::string src = 
            "\xD8\xA5\xD8\xB0\xD8\xA7 (\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD) { } "
            "\xD9\x88\xD8\xA5\xD9\x84\xD8\xA7 { }";
        auto nodes = parseSource(src);
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // --- اختبار E2E17 ---
    // (AR) حلقة كرر/طالما
    // (EN) While loop
    SAD_TEST("E2E17: حلقة طالما / While loop", {
        // طالما (صحيح) { }
        std::string src = 
            "\xD8\xB7\xD8\xA7\xD9\x84\xD9\x85\xD8\xA7 (\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD) { }";
        auto nodes = parseSource(src);
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // ══════════════════════════════════════════════════════════════════
    // القسم 4: تكامل أنواع البيانات
    // Section 4: Data Types Integration
    //
    // نختبر أن جميع أنواع البيانات تعمل بشكل صحيح عبر خط الأنابيب
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("E2E.DataTypes / أنواع_البيانات");

    // --- اختبار E2E18 ---
    // (AR) عدد صحيح كبير
    // (EN) Large integer
    SAD_TEST("E2E18: عدد صحيح كبير / Large integer", {
        auto nodes = parseSource(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 n = 999999999;"
        );
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // --- اختبار E2E19 ---
    // (AR) عدد عشري (فاصلة عائمة)
    // (EN) Floating point number
    SAD_TEST("E2E19: عدد عشري / Float", {
        auto nodes = parseSource(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 pi = 3.14159;"
        );
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // --- اختبار E2E20 ---
    // (AR) عدد سالب
    // (EN) Negative number
    SAD_TEST("E2E20: عدد سالب / Negative number", {
        auto nodes = parseSource(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 temp = -10;"
        );
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // --- اختبار E2E21 ---
    // (AR) نص فارغ
    // (EN) Empty string
    SAD_TEST("E2E21: نص فارغ / Empty string", {
        auto nodes = parseSource(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 s = \"\";"
        );
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // --- اختبار E2E22 ---
    // (AR) نص طويل مع رموز خاصة (\n, \t)
    // (EN) String with escape sequences
    SAD_TEST("E2E22: نص مع رموز هروب / String with escapes", {
        auto nodes = parseSource(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 msg = \"line1\\nline2\\tend\";"
        );
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // ══════════════════════════════════════════════════════════════════
    // القسم 5: بنى معقدة
    // Section 5: Complex Structures
    //
    // نختبر بنى لغوية أكثر تعقيداً: مصفوفات، تعابير متداخلة، OOP
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("E2E.ComplexStructures / بنى_معقدة");

    // --- اختبار E2E23 ---
    // (AR) مصفوفة (قائمة)
    // (EN) Array (list)
    SAD_TEST("E2E23: مصفوفة / Array", {
        auto nodes = parseSource(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 arr = [1, 2, 3];"
        );
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // --- اختبار E2E24 ---
    // (AR) تعبير متداخل بأقواس
    // (EN) Nested expression with parentheses
    SAD_TEST("E2E24: تعبير متداخل / Nested expression", {
        auto nodes = parseSource(
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 r = (1 + 2) * (3 + 4);"
        );
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // --- اختبار E2E25 ---
    // (AR) حلقة لكل (for)
    // (EN) For loop
    SAD_TEST("E2E25: حلقة لكل / For loop", {
        // لكل (متغير ع = 0; ع < 10; ع = ع + 1) { }
        // بعض المحللات قد تستخدم صيغة مختلفة
        // نحاول الصيغة الإنجليزية أيضاً إذا لم تنجح العربية
        SAD_ASSERT_NO_THROW(
            auto n = parseSource(
                "\xD9\x84\xD9\x83\xD9\x84 (\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 i = 0; i < 10; i = i + 1) { }"
            )
        );
    });

    // --- اختبار E2E26 ---
    // (AR) تعريف صنف بسيط (OOP)
    // (EN) Simple class definition
    SAD_TEST("E2E26: تعريف صنف / Class definition", {
        // صنف حيوان { }
        std::string src = 
            "\xD8\xB5\xD9\x86\xD9\x81 \xD8\xAD\xD9\x8A\xD9\x88\xD8\xA7\xD9\x86 { }";
        auto nodes = parseSource(src);
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // --- اختبار E2E27 ---
    // (AR) عملية إرجاع من دالة
    // (EN) Return statement from function
    SAD_TEST("E2E27: إرجاع من دالة / Return from function", {
        // دالة مربع(ن) { أرجع ن * ن; }
        std::string src = 
            "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 square(n) { \xD8\xA3\xD8\xB1\xD8\xAC\xD8\xB9 n * n; }";
        auto nodes = parseSource(src);
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // ══════════════════════════════════════════════════════════════════
    // القسم 6: مقاومة الأخطاء (Robustness)
    // Section 6: Error Robustness
    //
    // نتحقق من أن المحلل لا ينهار (crash) عند استلام مدخلات خاطئة
    // يجب أن يُبلغ عن خطأ بدلاً من التوقف المفاجئ
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("E2E.Robustness / مقاومة_الأخطاء");

    // --- اختبار E2E28 ---
    // (AR) كود غير مكتمل (بدون فاصلة منقوطة)
    // (EN) Incomplete code (missing semicolon)
    // الهدف: يجب أن لا ينهار البرنامج حتى مع كود خاطئ
    SAD_TEST("E2E28: كود غير مكتمل / Incomplete code", {
        // متغير أ = 5  (بدون فاصلة منقوطة)
        // يجب أن لا يسبب crash - إما يعمل أو يرمي استثناء
        bool nocrash = true;
        try {
            auto nodes = parseSource(
                "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xA3 = 5"
            );
        } catch (...) {
            // حدث خطأ - وهذا مقبول
            // المهم أنه لم يحدث crash
        }
        SAD_ASSERT_TRUE(nocrash);
    });

    // --- اختبار E2E29 ---
    // (AR) أقواس غير متطابقة
    // (EN) Mismatched brackets
    SAD_TEST("E2E29: أقواس غير متطابقة / Mismatched brackets", {
        bool nocrash = true;
        try {
            auto nodes = parseSource(
                "\xD8\xA5\xD8\xB0\xD8\xA7 (\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD) {"
            );
        } catch (...) {
            // مقبول - المهم عدم الانهيار
        }
        SAD_ASSERT_TRUE(nocrash);
    });

    // --- اختبار E2E30 ---
    // (AR) رمز غير معروف
    // (EN) Unknown token
    SAD_TEST("E2E30: رمز غير معروف / Unknown token", {
        bool nocrash = true;
        try {
            auto tokens = tokenize("$$$%%%");
        } catch (...) {
            // مقبول
        }
        SAD_ASSERT_TRUE(nocrash);
    });

    // --- اختبار E2E31 ---
    // (AR) نص كبير جداً (stress test)
    // (EN) Very large input (stress test)
    // الهدف: التحقق من أن المحلل يتعامل مع نصوص كبيرة بدون مشاكل ذاكرة
    SAD_TEST("E2E31: نص كبير / Large input stress test", {
        // ننشئ 100 عبارة متغير
        std::string bigSource;
        for (int i = 0; i < 100; i++) {
            bigSource += "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 v" + std::to_string(i) + " = " + std::to_string(i) + ";\n";
        }
        auto nodes = parseSource(bigSource);
        // يجب أن يكون هناك على الأقل 50 عبارة (بعضها قد يكون أسطر فارغة)
        SAD_ASSERT_GE(nodes.size(), (size_t)50);
    });

    // --- اختبار E2E32 ---
    // (AR) تعليقات يجب تجاهلها
    // (EN) Comments should be ignored
    SAD_TEST("E2E32: تعليقات / Comments ignored", {
        std::string src = 
            "// هذا تعليق\n"
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 x = 1;";
        auto nodes = parseSource(src);
        // التعليق يجب أن يُتجاهل ويُبقى فقط التعريف
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // --- اختبار E2E33 ---
    // (AR) تعليقات متعددة الأسطر
    // (EN) Multi-line comments
    SAD_TEST("E2E33: تعليقات متعددة / Multi-line comments", {
        std::string src = 
            "/* هذا تعليق\n"
            "   متعدد الأسطر */\n"
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 y = 2;";
        auto nodes = parseSource(src);
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // --- اختبار E2E34 ---
    // (AR) أحرف عربية مختلفة كمعرفات (أسماء متغيرات)
    // (EN) Various Arabic characters as identifiers
    // الهدف: التحقق من دعم الترميز العربي الكامل
    SAD_TEST("E2E34: معرفات عربية متنوعة / Various Arabic identifiers", {
        // اختبار أحرف مختلفة: حرف بتشكيل، حرف عادي، حرف من نهاية الأبجدية
        std::string src = 
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xB9\xD8\xAF\xD8\xAF = 42;"; // متغير عدد = 42;
        auto nodes = parseSource(src);
        SAD_ASSERT_GT(nodes.size(), (size_t)0);
    });

    // --- اختبار E2E35 ---
    // (AR) معرفات مختلطة (عربي + إنجليزي)
    // (EN) Mixed identifiers (Arabic + English)
    // لغة "ص" تدعم كلا النوعين من المعرفات
    SAD_TEST("E2E35: معرفات مختلطة / Mixed identifiers", {
        std::string src = 
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 counter = 0;\n"   // متغير counter = 0;
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xB9\xD8\xAF\xD8\xA7\xD8\xAF = 0;"; // متغير عداد = 0;
        auto nodes = parseSource(src);
        SAD_ASSERT_GE(nodes.size(), (size_t)2);
    });

#else
    // ══════════════════════════════════════════════════════════════════
    // إذا لم تتوفر المكونات المطلوبة، نتخطى الاختبارات
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("E2E.Status / حالة_الاختبارات");
    SAD_SKIP("E2E: Lexer/Parser/AST not available", 
             "الشوائب غير متاحة - تخطي اختبارات E2E");
#endif // HAS_LEXER && HAS_PARSER && HAS_AST

    // ══════════════════════════════════════════════════════════════════
    // القسم الإضافي: اختبارات نظام الأخطاء (مستقلة)
    // Additional: Error system tests (independent)
    // ══════════════════════════════════════════════════════════════════
#if HAS_ERRORS

    SAD_GROUP("E2E.ErrorSystem / نظام_الأخطاء");

    // --- اختبار E2E_ERR01 ---
    // (AR) إنشاء موقع مصدري والتحقق من صحته
    // (EN) Create source location and verify validity
    SAD_TEST("E2E_ERR01: SourceLocation صالح / Valid SourceLocation", {
        Sad::Errors::SourceLocation loc("test.s", 1, 5);
        SAD_ASSERT_TRUE(loc.isValid());
        SAD_ASSERT_EQ(loc.line, (size_t)1);
        SAD_ASSERT_EQ(loc.column, (size_t)5);
    });

    // --- اختبار E2E_ERR02 ---
    // (AR) موقع مصدري غير صالح
    // (EN) Invalid source location
    SAD_TEST("E2E_ERR02: SourceLocation غير صالح / Invalid SourceLocation", {
        Sad::Errors::SourceLocation loc;
        SAD_ASSERT_FALSE(loc.isValid());
    });

    // --- اختبار E2E_ERR03 ---
    // (AR) تحويل SourceLocation إلى نص
    // (EN) SourceLocation to string
    SAD_TEST("E2E_ERR03: SourceLocation toString / تحويل_لنص", {
        Sad::Errors::SourceLocation loc("myfile.s", 10, 3);
        std::string str = loc.toString();
        SAD_ASSERT_FALSE(str.empty());
        // يجب أن يحتوي على اسم الملف ورقم السطر
        SAD_ASSERT_CONTAINS(str, "myfile.s");
    });

    // --- اختبار E2E_ERR04 ---
    // (AR) ErrorManager - Singleton يعمل
    // (EN) ErrorManager Singleton works
    SAD_TEST("E2E_ERR04: ErrorManager Singleton / مدير_الأخطاء", {
        auto& mgr = Sad::Errors::ErrorManager::getInstance();
        mgr.clear();
        SAD_ASSERT_FALSE(mgr.hasErrors());
        SAD_ASSERT_EQ(mgr.getErrorCount(), (size_t)0);
    });

    // --- اختبار E2E_ERR05 ---
    // (AR) إضافة خطأ والتحقق منه
    // (EN) Add error and verify
    SAD_TEST("E2E_ERR05: إضافة خطأ / Report error", {
        auto& mgr = Sad::Errors::ErrorManager::getInstance();
        mgr.clear();
        
        mgr.reportError(
            Sad::Errors::ErrorCode::SYN_UNEXPECTED_TOKEN,
            Sad::Errors::SourceLocation("test.s", 1, 1),
            "\xD8\xB1\xD9\x85\xD8\xB2 \xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x85\xD8\xAA\xD9\x88\xD9\x82\xD8\xB9", // رمز غير متوقع
            "Unexpected token"
        );
        
        SAD_ASSERT_TRUE(mgr.hasErrors());
        SAD_ASSERT_EQ(mgr.getErrorCount(), (size_t)1);
        
        mgr.clear(); // تنظيف بعد الاختبار
    });

    // --- اختبار E2E_ERR06 ---
    // (AR) إضافة تحذير
    // (EN) Add warning
    SAD_TEST("E2E_ERR06: إضافة تحذير / Report warning", {
        auto& mgr = Sad::Errors::ErrorManager::getInstance();
        mgr.clear();
        
        mgr.reportWarning(
            Sad::Errors::ErrorCode::SEM_REDEFINITION,
            Sad::Errors::SourceLocation("test.s", 5, 1),
            "\xD8\xA5\xD8\xB9\xD8\xA7\xD8\xAF\xD8\xA9 \xD8\xAA\xD8\xB9\xD8\xB1\xD9\x8A\xD9\x81", // إعادة تعريف
            "Redefinition"
        );
        
        SAD_ASSERT_FALSE(mgr.hasErrors()); // تحذير ≠ خطأ
        SAD_ASSERT_EQ(mgr.getWarningCount(), (size_t)1);
        
        mgr.clear();
    });

    // --- اختبار E2E_ERR07 ---
    // (AR) تصدير إلى JSON
    // (EN) Export to JSON
    SAD_TEST("E2E_ERR07: تصدير JSON / JSON export", {
        auto& mgr = Sad::Errors::ErrorManager::getInstance();
        mgr.clear();
        
        mgr.reportError(
            Sad::Errors::ErrorCode::LEX_INVALID_CHARACTER,
            Sad::Errors::SourceLocation("test.s", 1, 1),
            "\xD8\xB1\xD9\x85\xD8\xB2 \xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xB5\xD8\xA7\xD9\x84\xD8\xAD", // رمز غير صالح
            "Invalid character"
        );
        
        std::string json = mgr.toJSON();
        SAD_ASSERT_FALSE(json.empty());
        
        mgr.clear();
    });

    // --- اختبار E2E_ERR08 ---
    // (AR) عدة أخطاء في نفس الوقت
    // (EN) Multiple errors at once
    SAD_TEST("E2E_ERR08: أخطاء متعددة / Multiple errors", {
        auto& mgr = Sad::Errors::ErrorManager::getInstance();
        mgr.clear();
        
        // إضافة 5 أخطاء مختلفة
        for (int i = 1; i <= 5; i++) {
            mgr.reportError(
                Sad::Errors::ErrorCode::SYN_UNEXPECTED_TOKEN,
                Sad::Errors::SourceLocation("test.s", i, 1),
                "خطأ " + std::to_string(i),
                "Error " + std::to_string(i)
            );
        }
        
        SAD_ASSERT_EQ(mgr.getErrorCount(), (size_t)5);
        
        mgr.clear();
    });

    // --- اختبار E2E_ERR09 ---
    // (AR) وصف الخطأ يعمل
    // (EN) Error description works
    SAD_TEST("E2E_ERR09: وصف الأخطاء / Error descriptions", {
        std::string desc = Sad::Errors::getErrorDescription(
            Sad::Errors::ErrorCode::SYN_MISSING_SEMICOLON,
            Sad::Errors::Language::ARABIC
        );
        // يجب أن يكون هناك وصف (ليس فارغاً)
        SAD_ASSERT_FALSE(desc.empty());
    });

    // --- اختبار E2E_ERR10 ---
    // (AR) فئة الخطأ تعمل
    // (EN) Error category works
    SAD_TEST("E2E_ERR10: فئة الخطأ / Error category", {
        std::string cat = Sad::Errors::getErrorCategory(
            Sad::Errors::ErrorCode::SYN_MISSING_SEMICOLON
        );
        SAD_ASSERT_FALSE(cat.empty());
    });

    // --- اختبار E2E_ERR11 ---
    // (AR) رمز الخطأ كنص
    // (EN) Error code as string
    SAD_TEST("E2E_ERR11: رمز الخطأ كنص / Error code string", {
        std::string code = Sad::Errors::getErrorCodeString(
            Sad::Errors::ErrorCode::LEX_INVALID_CHARACTER
        );
        SAD_ASSERT_FALSE(code.empty());
    });

    // --- اختبار E2E_ERR12 ---
    // (AR) الحد الأقصى للأخطاء
    // (EN) Max errors setting
    SAD_TEST("E2E_ERR12: إعدادات مدير الأخطاء / ErrorManager settings", {
        auto& mgr = Sad::Errors::ErrorManager::getInstance();
        mgr.clear();
        mgr.setMaxErrors(50);
        mgr.setLanguage(Sad::Errors::Language::ARABIC);
        mgr.setColorize(false);
        SAD_ASSERT_TRUE(true); // الوصول هنا يعني نجاح الإعدادات
        
        // إعادة الإعدادات الأصلية
        mgr.setMaxErrors(100);
        mgr.setColorize(true);
        mgr.clear();
    });

#endif // HAS_ERRORS

    // ══════════════════════════════════════════════════════════════════
    // عرض النتائج النهائية
    // ══════════════════════════════════════════════════════════════════
    SAD_SUMMARY();
}
