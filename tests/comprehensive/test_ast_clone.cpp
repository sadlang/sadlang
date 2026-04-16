/**
 * @file test_ast_clone.cpp
 * @brief (AR) اختبار عملي لاستنساخ عُقد AST مع كود ص حقيقي
 * @brief (EN) Practical test for AST cloning with real Sad code
 *
 * (AR) يختبر: تحليل كود ص فعلي → استنساخ AST → تحقق من صحة النسخة
 * (EN) Tests: parse real Sad code → clone AST → verify clone correctness
 */

#include "sad_test_framework.h"

#if __has_include("lexer_core.h")
    #include "lexer_core.h"
    #define HAS_LEXER 1
#else
    #define HAS_LEXER 0
#endif

#if __has_include("parser_core.h")
    #include "parser_core.h"
    #define HAS_PARSER 1
#else
    #define HAS_PARSER 0
#endif

#if __has_include("ast_node.h")
    #include "ast_node.h"
    #include "ast_clone.h"
    #include "expressions.h"
    #include "statements.h"
    #include "declarations.h"
    #include "class_nodes.h"
    #define HAS_AST 1
#else
    #define HAS_AST 0
#endif

#include <string>
#include <iostream>

// ======================================================================
// (AR) كلمات مفتاحية عربية / (EN) Arabic keywords
// ======================================================================
#define AR_VAR      "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1"
#define AR_CONST    "\xD8\xAB\xD8\xA7\xD8\xA8\xD8\xAA"
#define AR_FUNC     "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9"
#define AR_RETURN   "\xD8\xA7\xD8\xB1\xD8\xAC\xD8\xB9"
#define AR_END      "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9"
#define AR_IF       "\xD8\xA5\xD8\xB0\xD8\xA7"
#define AR_ELSE     "\xD9\x88\xD8\xA5\xD9\x84\xD8\xA7"
#define AR_WHILE    "\xD8\xA8\xD9\x8A\xD9\x86\xD9\x85\xD8\xA7"
#define AR_FOR      "\xD9\x84\xD9\x83\xD9\x84"
#define AR_IN       "\xD9\x81\xD9\x8A"
#define AR_TRUE     "\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD"
#define AR_FALSE    "\xD8\xAE\xD8\xB7\xD8\xA3"
#define AR_CLASS    "\xD8\xB5\xD9\x86\xD9\x81"
#define AR_CONSTR   "\xD8\xA8\xD8\xA7\xD9\x86\xD9\x8A"
#define AR_THIS     "\xD9\x87\xD8\xB0\xD8\xA7"
#define AR_NEW      "\xD8\xAC\xD8\xAF\xD9\x8A\xD8\xAF"
#define AR_TRY      "\xD8\xAD\xD8\xA7\xD9\x88\xD9\x84"
#define AR_CATCH    "\xD8\xA7\xD9\x85\xD8\xB3\xD9\x83"
#define AR_THROW    "\xD8\xA7\xD8\xB1\xD9\x85\xD9\x8A"
#define AR_SWITCH   "\xD8\xB7\xD8\xA7\xD8\xA8\xD9\x82"
#define AR_WHEN     "\xD8\xB9\xD9\x86\xD8\xAF\xD9\x85\xD8\xA7"
#define AR_DEFAULT  "\xD8\xA7\xD9\x81\xD8\xAA\xD8\xB1\xD8\xA7\xD8\xB6\xD9\x8A"
#define AR_BREAK    "\xD8\xAA\xD9\x88\xD9\x82\xD9\x81"

// ======================================================================
// (AR) دوال مساعدة / (EN) Helper functions
// ======================================================================
#if HAS_LEXER && HAS_PARSER

static Sad::AST::StmtList parse(const std::string& source) {
    Sad::Lexer::LexerCore lexer(source);
    Sad::Parser::ParserCore parser(lexer);
    return parser.parseProgram();
}

#endif

// ======================================================================
int main() {
    SAD_TEST_INIT();

#if HAS_LEXER && HAS_PARSER && HAS_AST
    using namespace Sad::AST;

    // ══════════════════════════════════════════════════════════════════
    // (AR) مجموعة 1: استنساخ التعابير الأساسية
    // (EN) Group 1: Basic Expression Cloning
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("ASTClone.BasicExpressions / استنساخ.التعابير_الأساسية");

    SAD_TEST("CLONE01: استنساخ تعبير حرفي رقم", {
        auto stmts = parse(AR_VAR " x = 42");
        SAD_ASSERT_TRUE(stmts.size() >= 1);
        auto clone = cloneStatement(*stmts[0]);
        SAD_ASSERT_TRUE(clone != nullptr);
        SAD_ASSERT_EQ(clone->toString(), stmts[0]->toString());
    });

    SAD_TEST("CLONE02: استنساخ تعبير حرفي نص", {
        auto stmts = parse(AR_VAR " s = \"hello\"");
        SAD_ASSERT_TRUE(stmts.size() >= 1);
        auto clone = cloneStatement(*stmts[0]);
        SAD_ASSERT_TRUE(clone != nullptr);
        SAD_ASSERT_EQ(clone->toString(), stmts[0]->toString());
    });

    SAD_TEST("CLONE03: استنساخ تعبير ثنائي (جمع)", {
        auto stmts = parse(AR_VAR " x = 2 + 3");
        SAD_ASSERT_TRUE(stmts.size() >= 1);
        auto clone = cloneStatement(*stmts[0]);
        SAD_ASSERT_TRUE(clone != nullptr);
        SAD_ASSERT_EQ(clone->toString(), stmts[0]->toString());
    });

    SAD_TEST("CLONE04: استنساخ تعبير ثلاثي (شرطي)", {
        auto stmts = parse(AR_VAR " x = 1 > 0 ? 10 : 20");
        SAD_ASSERT_TRUE(stmts.size() >= 1);
        auto clone = cloneStatement(*stmts[0]);
        SAD_ASSERT_TRUE(clone != nullptr);
        SAD_ASSERT_EQ(clone->toString(), stmts[0]->toString());
    });

    SAD_TEST("CLONE05: استنساخ مصفوفة", {
        auto stmts = parse(AR_VAR " arr = [1, 2, 3]");
        SAD_ASSERT_TRUE(stmts.size() >= 1);
        auto clone = cloneStatement(*stmts[0]);
        SAD_ASSERT_TRUE(clone != nullptr);
        SAD_ASSERT_EQ(clone->toString(), stmts[0]->toString());
    });

    // ══════════════════════════════════════════════════════════════════
    // (AR) مجموعة 2: استنساخ العبارات المركبة
    // (EN) Group 2: Compound Statement Cloning
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("ASTClone.CompoundStatements / استنساخ.العبارات_المركبة");

    SAD_TEST("CLONE06: استنساخ إذا/وإلا", {
        std::string code = AR_VAR " x = 5\n" AR_IF " (x > 0)\n  " AR_VAR " y = 1\n" AR_ELSE "\n  " AR_VAR " y = 2\n" AR_END "\n";
        auto stmts = parse(code);
        SAD_ASSERT_TRUE(stmts.size() >= 2);
        // Clone the if-else (second statement)
        auto clone = cloneStatement(*stmts[1]);
        SAD_ASSERT_TRUE(clone != nullptr);
        // Verify it's an IfStmt
        auto* ifClone = dynamic_cast<IfStmt*>(clone.get());
        SAD_ASSERT_TRUE(ifClone != nullptr);
        SAD_ASSERT_TRUE(ifClone->condition != nullptr);
        SAD_ASSERT_TRUE(ifClone->thenBranch != nullptr);
        SAD_ASSERT_TRUE(ifClone->elseBranch != nullptr);
    });

    SAD_TEST("CLONE07: استنساخ حلقة بينما", {
        std::string code = AR_VAR " x = 0\n" AR_WHILE " (x < 10)\n  " AR_VAR " x = x + 1\n" AR_END "\n";
        auto stmts = parse(code);
        SAD_ASSERT_TRUE(stmts.size() >= 2);
        auto clone = cloneStatement(*stmts[1]);
        SAD_ASSERT_TRUE(clone != nullptr);
        auto* whileClone = dynamic_cast<WhileStmt*>(clone.get());
        SAD_ASSERT_TRUE(whileClone != nullptr);
        SAD_ASSERT_TRUE(whileClone->condition != nullptr);
        SAD_ASSERT_TRUE(whileClone->body != nullptr);
    });

    SAD_TEST("CLONE08: استنساخ حلقة لكل", {
        std::string code = AR_FOR " x " AR_IN " [1, 2, 3]\n  " AR_VAR " y = x\n" AR_END "\n";
        auto stmts = parse(code);
        SAD_ASSERT_TRUE(stmts.size() >= 1);
        auto clone = cloneStatement(*stmts[0]);
        SAD_ASSERT_TRUE(clone != nullptr);
        auto* forClone = dynamic_cast<ForRangeStmt*>(clone.get());
        SAD_ASSERT_TRUE(forClone != nullptr);
        SAD_ASSERT_TRUE(forClone->iterable != nullptr);
        SAD_ASSERT_TRUE(forClone->body != nullptr);
    });

    // ══════════════════════════════════════════════════════════════════
    // (AR) مجموعة 3: استنساخ التصريحات (دوال وأصناف)
    // (EN) Group 3: Declaration Cloning (functions and classes)
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("ASTClone.Declarations / استنساخ.التصريحات");

    SAD_TEST("CLONE09: استنساخ دالة بسيطة", {
        std::string code = AR_FUNC " add(a, b)\n  " AR_RETURN " a + b\n" AR_END "\n";
        auto stmts = parse(code);
        SAD_ASSERT_TRUE(stmts.size() >= 1);
        auto clone = cloneStatement(*stmts[0]);
        SAD_ASSERT_TRUE(clone != nullptr);
        auto* fnClone = dynamic_cast<FunctionDecl*>(clone.get());
        SAD_ASSERT_TRUE(fnClone != nullptr);
        SAD_ASSERT_EQ(fnClone->name, std::string("add"));
        SAD_ASSERT_EQ(fnClone->parameters.size(), (size_t)2);
        SAD_ASSERT_TRUE(fnClone->body != nullptr);
    });

    SAD_TEST("CLONE10: استنساخ دالة معقدة", {
        std::string code =
            AR_FUNC " compute(x)\n"
            "  " AR_IF " (x > 10)\n"
            "    " AR_RETURN " x * 2\n"
            "  " AR_ELSE "\n"
            "    " AR_RETURN " x + 1\n"
            "  " AR_END "\n"
            AR_END "\n";
        auto stmts = parse(code);
        SAD_ASSERT_TRUE(stmts.size() >= 1);
        auto clone = cloneStatement(*stmts[0]);
        SAD_ASSERT_TRUE(clone != nullptr);
        auto* fn = dynamic_cast<FunctionDecl*>(clone.get());
        SAD_ASSERT_TRUE(fn != nullptr);
        SAD_ASSERT_EQ(fn->name, std::string("compute"));
    });

    // ══════════════════════════════════════════════════════════════════
    // (AR) مجموعة 4: التحقق من عمق النسخ (ليست مراجع)
    // (EN) Group 4: Verify deep copy (not references)
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("ASTClone.DeepCopy / استنساخ.النسخ_العميق");

    SAD_TEST("CLONE11: النسخة مستقلة (تعديل الأصل لا يؤثر)", {
        auto stmts = parse(AR_VAR " x = 42");
        SAD_ASSERT_TRUE(stmts.size() >= 1);
        auto clone = cloneStatement(*stmts[0]);
        SAD_ASSERT_TRUE(clone != nullptr);
        // Get addresses - must be different
        SAD_ASSERT_TRUE(clone.get() != stmts[0].get());
        // Check inner expression (VarDeclStmt initializer)
        auto* orig = dynamic_cast<VarDeclStmt*>(stmts[0].get());
        auto* copy = dynamic_cast<VarDeclStmt*>(clone.get());
        SAD_ASSERT_TRUE(orig != nullptr);
        SAD_ASSERT_TRUE(copy != nullptr);
        // Pointers must differ
        SAD_ASSERT_TRUE(orig->initializer.get() != copy->initializer.get());
        // But values must match
        SAD_ASSERT_EQ(orig->name, copy->name);
        SAD_ASSERT_EQ(orig->isConst, copy->isConst);
    });

    SAD_TEST("CLONE12: استنساخ تعبير مباشر (cloneExpression)", {
        auto stmts = parse(AR_VAR " x = 10 + 20 * 3");
        SAD_ASSERT_TRUE(stmts.size() >= 1);
        auto* vd = dynamic_cast<VarDeclStmt*>(stmts[0].get());
        SAD_ASSERT_TRUE(vd != nullptr);
        SAD_ASSERT_TRUE(vd->initializer != nullptr);
        auto exprClone = cloneExpression(*vd->initializer);
        SAD_ASSERT_TRUE(exprClone != nullptr);
        // Different pointer
        SAD_ASSERT_TRUE(exprClone.get() != vd->initializer.get());
        // Same toString
        SAD_ASSERT_EQ(exprClone->toString(), vd->initializer->toString());
    });

    SAD_TEST("CLONE13: استنساخ break/continue (عبارات بسيطة)", {
        auto breakClone = cloneStatement(BreakStmt());
        SAD_ASSERT_TRUE(breakClone != nullptr);
        SAD_ASSERT_TRUE(dynamic_cast<BreakStmt*>(breakClone.get()) != nullptr);

        auto contClone = cloneStatement(ContinueStmt());
        SAD_ASSERT_TRUE(contClone != nullptr);
        SAD_ASSERT_TRUE(dynamic_cast<ContinueStmt*>(contClone.get()) != nullptr);
    });

    // ══════════════════════════════════════════════════════════════════
    // (AR) مجموعة 5: حالات الحدود
    // (EN) Group 5: Edge Cases
    // ══════════════════════════════════════════════════════════════════
    SAD_GROUP("ASTClone.EdgeCases / استنساخ.حالات_الحدود");

    SAD_TEST("CLONE14: استنساخ كتلة فارغة", {
        StmtList empty;
        auto block = BlockStmt(std::move(empty));
        auto clone = cloneStatement(block);
        SAD_ASSERT_TRUE(clone != nullptr);
        auto* blockClone = dynamic_cast<BlockStmt*>(clone.get());
        SAD_ASSERT_TRUE(blockClone != nullptr);
        SAD_ASSERT_EQ(blockClone->statements.size(), (size_t)0);
    });

    SAD_TEST("CLONE15: cloneExprPtr(nullptr) → nullptr", {
        ExprPtr null = nullptr;
        auto result = cloneExprPtr(null);
        SAD_ASSERT_TRUE(result == nullptr);
    });

    SAD_TEST("CLONE16: cloneStmtPtr(nullptr) → nullptr", {
        StmtPtr null = nullptr;
        auto result = cloneStmtPtr(null);
        SAD_ASSERT_TRUE(result == nullptr);
    });

    SAD_TEST("CLONE17: استنساخ return بدون قيمة", {
        auto ret = ReturnStmt();
        auto clone = cloneStatement(ret);
        SAD_ASSERT_TRUE(clone != nullptr);
        auto* retClone = dynamic_cast<ReturnStmt*>(clone.get());
        SAD_ASSERT_TRUE(retClone != nullptr);
        SAD_ASSERT_TRUE(retClone->value == nullptr);
    });

    SAD_TEST("CLONE18: استنساخ حاول/امسك", {
        std::string code =
            AR_TRY "\n"
            "  " AR_VAR " x = 1\n"
            AR_CATCH " e\n"
            "  " AR_VAR " y = 2\n"
            AR_END "\n";
        auto stmts = parse(code);
        SAD_ASSERT_TRUE(stmts.size() >= 1);
        auto clone = cloneStatement(*stmts[0]);
        SAD_ASSERT_TRUE(clone != nullptr);
        auto* tryClone = dynamic_cast<TryStmt*>(clone.get());
        SAD_ASSERT_TRUE(tryClone != nullptr);
        SAD_ASSERT_TRUE(tryClone->tryBlock != nullptr);
        SAD_ASSERT_TRUE(tryClone->catchClauses.size() >= 1);
    });

#else
    SAD_GROUP("ASTClone / استنساخ_عُقد");
    SAD_SKIP("All", "Lexer/Parser/AST not available");
#endif

    SAD_SUMMARY();
}
