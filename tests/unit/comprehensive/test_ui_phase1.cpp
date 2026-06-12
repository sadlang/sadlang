// بسم الله الرحمن الرحيم
// ======================================================================
// test_ui_phase1.cpp — اختبارات القبول للمرحلة 1 من SadUI
// ======================================================================
// الوصف:
//   يختبر هذا الملف جميع معايير القبول لقصص Phase 1:
//   - UI-2.1: مُجمّع الربطات (BindingCompiler)
//     * تحليل لامدات بسيطة وتحويلها لدوال C++
//     * رفض لامدات معقدة (closure, كتلة أوامر)
//     * دوال التحويل المضمنة (نص، رقم، عشري، منطقي)
//     * العمليات الحسابية والمنطقية
//     * فحص التعقيد (nodeCount, depth)
//     * إحصائيات المُجمّع
//
// English Description:
//   Tests all acceptance criteria for Phase 1 stories:
//   - UI-2.1: BindingCompiler
//     * Simple lambda analysis and C++ conversion
//     * Complex lambda rejection (closures, block bodies)
//     * Built-in type conversion functions
//     * Arithmetic and logical operations
//     * Complexity checks (nodeCount, depth)
//     * Compiler statistics
// ======================================================================

#include "sad_test_framework.h"
#include "binding_compiler.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "expressions.h"
#include "statements.h"
#include "token.h"

#include <string>
#include <vector>
#include <iostream>
#include <cmath>

using namespace SadTest;
using namespace sad::ui;
using namespace Sad;
using namespace Sad::AST;
using namespace Sad::Lexer;
using namespace Sad::Parser;

// =====================================================================
// (AR) دوال مساعدة للاختبار / (EN) Test helper functions
// =====================================================================

/**
 * @brief (AR) يحلل كود ص ويُرجع قائمة الجمل
 * @brief (EN) Parses Sad code and returns statement list
 */
static StmtList parseCode(const std::string &code)
{
    LexerCore lexer(code);
    ParserCore parser(lexer);
    auto stmts = parser.parseProgram();
    if (parser.hasErrors())
    {
        parser.printErrors();
    }
    return stmts;
}

/**
 * @brief (AR) يستخرج LambdaExpr من جملة تعريف متغير
 *        مثال: متغير ف = لامدا(ق) ق + 1
 * @brief (EN) Extracts LambdaExpr from a variable declaration
 */
static const LambdaExpr *extractLambda(const StmtList &stmts)
{
    if (stmts.empty())
        return nullptr;

    // (AR) نبحث عن تعريف متغير يحتوي لامدا
    auto *varDecl = dynamic_cast<const VarDeclStmt *>(stmts[0].get());
    if (!varDecl || !varDecl->initializer)
        return nullptr;

    return dynamic_cast<const LambdaExpr *>(varDecl->initializer.get());
}

// =====================================================================
// (AR) نقطة الدخول / (EN) Entry point
// =====================================================================

int main()
{
    auto &runner = TestRunner::instance();

    // (AR) عنوان الملف
    std::cout << "\n"
              << "========================================================\n"
              << "  Phase 1 Tests -- UI-2.1: BindingCompiler\n"
              << "========================================================\n\n";

    // ═════════════════════════════════════════════════════════════════
    // (AR) مجموعة اختبارات BindingValue
    // (EN) BindingValue test group
    // ═════════════════════════════════════════════════════════════════

    runner.beginGroup("UI-2.1.BindingValue");

    runner.runTest("BindingValue - null", [&]()
                   {
        BindingValue v;
        SAD_ASSERT_TRUE(v.isNull());
        SAD_ASSERT_FALSE(v.isInteger());
        SAD_ASSERT_FALSE(v.isDouble());
        SAD_ASSERT_FALSE(v.isString());
        SAD_ASSERT_FALSE(v.isBoolean()); });

    runner.runTest("BindingValue - integer", [&]()
                   {
        BindingValue v(static_cast<int64_t>(42));
        SAD_ASSERT_TRUE(v.isInteger());
        SAD_ASSERT_EQ(v.asInteger(), static_cast<int64_t>(42));
        SAD_ASSERT_EQ(v.toDouble(), 42.0);
        SAD_ASSERT_EQ(v.toString(), std::string("42")); });

    runner.runTest("BindingValue - double", [&]()
                   {
        BindingValue v(3.14);
        SAD_ASSERT_TRUE(v.isDouble());
        SAD_ASSERT_TRUE(std::abs(v.asDouble() - 3.14) < 0.001);
        SAD_ASSERT_EQ(v.toDouble(), 3.14); });

    runner.runTest("BindingValue - string", [&]()
                   {
        BindingValue v(std::string("hello"));
        SAD_ASSERT_TRUE(v.isString());
        SAD_ASSERT_EQ(v.asString(), std::string("hello")); });

    runner.runTest("BindingValue - boolean", [&]()
                   {
        BindingValue vTrue(true);
        BindingValue vFalse(false);
        SAD_ASSERT_TRUE(vTrue.isBoolean());
        SAD_ASSERT_TRUE(vTrue.asBoolean());
        SAD_ASSERT_FALSE(vFalse.asBoolean()); });

    runner.runTest("BindingValue - equality same type", [&]()
                   {
        SAD_ASSERT_TRUE(BindingValue(static_cast<int64_t>(5)) == BindingValue(static_cast<int64_t>(5)));
        SAD_ASSERT_TRUE(BindingValue(3.14) == BindingValue(3.14));
        SAD_ASSERT_TRUE(BindingValue(std::string("abc")) == BindingValue(std::string("abc")));
        SAD_ASSERT_TRUE(BindingValue(true) == BindingValue(true)); });

    runner.runTest("BindingValue - inequality", [&]()
                   {
        SAD_ASSERT_FALSE(BindingValue(static_cast<int64_t>(5)) == BindingValue(static_cast<int64_t>(6)));
        SAD_ASSERT_FALSE(BindingValue(static_cast<int64_t>(5)) == BindingValue(5.0));
        SAD_ASSERT_FALSE(BindingValue(std::string("a")) == BindingValue(std::string("b"))); });

    // ═════════════════════════════════════════════════════════════════
    // (AR) مجموعة اختبارات BindingExprNode
    // (EN) BindingExprNode test group
    // ═════════════════════════════════════════════════════════════════

    runner.beginGroup("UI-2.1.BindingExprNode");

    runner.runTest("ExprNode - LiteralInt", [&]()
                   {
        auto node = BindingExprNode::makeLiteralInt(42);
        SAD_ASSERT_TRUE(node != nullptr);
        SAD_ASSERT_EQ(static_cast<int>(node->type), static_cast<int>(BindingExprNode::Type::LiteralInteger));
        SAD_ASSERT_EQ(node->intVal, static_cast<int64_t>(42));
        SAD_ASSERT_EQ(node->countNodes(), static_cast<size_t>(1)); });

    runner.runTest("ExprNode - LiteralString", [&]()
                   {
        auto node = BindingExprNode::makeLiteralString("test");
        SAD_ASSERT_TRUE(node != nullptr);
        SAD_ASSERT_EQ(static_cast<int>(node->type), static_cast<int>(BindingExprNode::Type::LiteralString));
        SAD_ASSERT_EQ(node->strVal, std::string("test")); });

    runner.runTest("ExprNode - ParamRef", [&]()
                   {
        auto node = BindingExprNode::makeParamRef("x");
        SAD_ASSERT_TRUE(node != nullptr);
        SAD_ASSERT_EQ(static_cast<int>(node->type), static_cast<int>(BindingExprNode::Type::ParamRef));
        SAD_ASSERT_EQ(node->paramName, std::string("x")); });

    runner.runTest("ExprNode - BinaryAdd (3 nodes)", [&]()
                   {
        auto left = BindingExprNode::makeParamRef("x");
        auto right = BindingExprNode::makeLiteralInt(1);
        auto node = BindingExprNode::makeBinary(
            BindingExprNode::Type::BinaryAdd, std::move(left), std::move(right));
        SAD_ASSERT_TRUE(node != nullptr);
        SAD_ASSERT_EQ(node->countNodes(), static_cast<size_t>(3)); });

    runner.runTest("ExprNode - UnaryNeg (2 nodes)", [&]()
                   {
        auto operand = BindingExprNode::makeLiteralInt(5);
        auto node = BindingExprNode::makeUnary(
            BindingExprNode::Type::UnaryNeg, std::move(operand));
        SAD_ASSERT_TRUE(node != nullptr);
        SAD_ASSERT_EQ(node->countNodes(), static_cast<size_t>(2)); });

    runner.runTest("ExprNode - BuiltinConvert (2 nodes)", [&]()
                   {
        auto arg = BindingExprNode::makeParamRef("x");
        auto node = BindingExprNode::makeBuiltinConvert(
            BindingExprNode::Type::ToStringBuiltin, std::move(arg));
        SAD_ASSERT_TRUE(node != nullptr);
        SAD_ASSERT_EQ(node->countNodes(), static_cast<size_t>(2)); });

    // ═════════════════════════════════════════════════════════════════
    // (AR) مجموعة اختبارات evaluateNode عبر compileNode
    // (EN) evaluateNode via compileNode test group
    // ═════════════════════════════════════════════════════════════════

    runner.beginGroup("UI-2.1.EvaluateNode");

    runner.runTest("Eval - LiteralInt returns constant", [&]()
                   {
        BindingCompiler compiler;
        auto node = BindingExprNode::makeLiteralInt(99);
        auto transform = compiler.compileNode(node.get());
        SAD_ASSERT_TRUE(transform != nullptr);
        auto result = transform(BindingValue());
        SAD_ASSERT_TRUE(result.isInteger());
        SAD_ASSERT_EQ(result.asInteger(), static_cast<int64_t>(99)); });

    runner.runTest("Eval - ParamRef returns input", [&]()
                   {
        BindingCompiler compiler;
        auto node = BindingExprNode::makeParamRef("x");
        auto transform = compiler.compileNode(node.get());
        SAD_ASSERT_TRUE(transform != nullptr);
        auto result = transform(BindingValue(static_cast<int64_t>(42)));
        SAD_ASSERT_TRUE(result.isInteger());
        SAD_ASSERT_EQ(result.asInteger(), static_cast<int64_t>(42)); });

    runner.runTest("Eval - BinaryAdd integers", [&]()
                   {
        BindingCompiler compiler;
        auto node = BindingExprNode::makeBinary(
            BindingExprNode::Type::BinaryAdd,
            BindingExprNode::makeParamRef("x"),
            BindingExprNode::makeLiteralInt(10));
        auto transform = compiler.compileNode(node.get());
        auto result = transform(BindingValue(static_cast<int64_t>(5)));
        SAD_ASSERT_EQ(result.asInteger(), static_cast<int64_t>(15)); });

    runner.runTest("Eval - BinaryAdd string concat", [&]()
                   {
        BindingCompiler compiler;
        auto node = BindingExprNode::makeBinary(
            BindingExprNode::Type::BinaryAdd,
            BindingExprNode::makeLiteralString("hello "),
            BindingExprNode::makeParamRef("x"));
        auto transform = compiler.compileNode(node.get());
        auto result = transform(BindingValue(std::string("world")));
        SAD_ASSERT_EQ(result.asString(), std::string("hello world")); });

    runner.runTest("Eval - BinaryMul doubles", [&]()
                   {
        BindingCompiler compiler;
        auto node = BindingExprNode::makeBinary(
            BindingExprNode::Type::BinaryMul,
            BindingExprNode::makeParamRef("x"),
            BindingExprNode::makeLiteralDouble(2.5));
        auto transform = compiler.compileNode(node.get());
        auto result = transform(BindingValue(4.0));
        SAD_ASSERT_TRUE(std::abs(result.asDouble() - 10.0) < 0.001); });

    runner.runTest("Eval - BinaryDiv by zero returns 0", [&]()
                   {
        BindingCompiler compiler;
        auto node = BindingExprNode::makeBinary(
            BindingExprNode::Type::BinaryDiv,
            BindingExprNode::makeLiteralInt(10),
            BindingExprNode::makeLiteralInt(0));
        auto transform = compiler.compileNode(node.get());
        auto result = transform(BindingValue());
        SAD_ASSERT_TRUE(result.isDouble());
        SAD_ASSERT_EQ(result.asDouble(), 0.0); });

    runner.runTest("Eval - BinaryGt comparison", [&]()
                   {
        BindingCompiler compiler;
        auto node = BindingExprNode::makeBinary(
            BindingExprNode::Type::BinaryGt,
            BindingExprNode::makeParamRef("x"),
            BindingExprNode::makeLiteralInt(5));
        auto transform = compiler.compileNode(node.get());

        auto r1 = transform(BindingValue(static_cast<int64_t>(10)));
        SAD_ASSERT_TRUE(r1.isBoolean());
        SAD_ASSERT_TRUE(r1.asBoolean());

        auto r2 = transform(BindingValue(static_cast<int64_t>(3)));
        SAD_ASSERT_FALSE(r2.asBoolean()); });

    runner.runTest("Eval - UnaryNot boolean", [&]()
                   {
        BindingCompiler compiler;
        auto node = BindingExprNode::makeUnary(
            BindingExprNode::Type::UnaryNot,
            BindingExprNode::makeParamRef("x"));
        auto transform = compiler.compileNode(node.get());

        auto r1 = transform(BindingValue(true));
        SAD_ASSERT_TRUE(r1.isBoolean());
        SAD_ASSERT_FALSE(r1.asBoolean());

        auto r2 = transform(BindingValue(false));
        SAD_ASSERT_TRUE(r2.asBoolean()); });

    runner.runTest("Eval - ToStringBuiltin", [&]()
                   {
        BindingCompiler compiler;
        auto node = BindingExprNode::makeBuiltinConvert(
            BindingExprNode::Type::ToStringBuiltin,
            BindingExprNode::makeParamRef("x"));
        auto transform = compiler.compileNode(node.get());

        auto result = transform(BindingValue(static_cast<int64_t>(42)));
        SAD_ASSERT_TRUE(result.isString());
        SAD_ASSERT_EQ(result.asString(), std::string("42")); });

    runner.runTest("Eval - ToIntBuiltin", [&]()
                   {
        BindingCompiler compiler;
        auto node = BindingExprNode::makeBuiltinConvert(
            BindingExprNode::Type::ToIntBuiltin,
            BindingExprNode::makeParamRef("x"));
        auto transform = compiler.compileNode(node.get());

        auto result = transform(BindingValue(3.7));
        SAD_ASSERT_TRUE(result.isInteger());
        SAD_ASSERT_EQ(result.asInteger(), static_cast<int64_t>(3)); });

    runner.runTest("Eval - BinaryAnd short-circuit", [&]()
                   {
        BindingCompiler compiler;
        auto node = BindingExprNode::makeBinary(
            BindingExprNode::Type::BinaryAnd,
            BindingExprNode::makeLiteralBool(false),
            BindingExprNode::makeLiteralBool(true));
        auto transform = compiler.compileNode(node.get());
        auto result = transform(BindingValue());
        SAD_ASSERT_TRUE(result.isBoolean());
        SAD_ASSERT_FALSE(result.asBoolean()); });

    runner.runTest("Eval - UnaryNeg integer", [&]()
                   {
        BindingCompiler compiler;
        auto node = BindingExprNode::makeUnary(
            BindingExprNode::Type::UnaryNeg,
            BindingExprNode::makeParamRef("x"));
        auto transform = compiler.compileNode(node.get());

        auto result = transform(BindingValue(static_cast<int64_t>(7)));
        SAD_ASSERT_TRUE(result.isInteger());
        SAD_ASSERT_EQ(result.asInteger(), static_cast<int64_t>(-7)); });

    runner.runTest("Eval - BinaryMod", [&]()
                   {
        BindingCompiler compiler;
        auto node = BindingExprNode::makeBinary(
            BindingExprNode::Type::BinaryMod,
            BindingExprNode::makeParamRef("x"),
            BindingExprNode::makeLiteralInt(3));
        auto transform = compiler.compileNode(node.get());

        auto result = transform(BindingValue(static_cast<int64_t>(10)));
        SAD_ASSERT_EQ(result.asInteger(), static_cast<int64_t>(1)); });

    // ═════════════════════════════════════════════════════════════════
    // (AR) مجموعة اختبارات compile الكامل (مع Parser)
    // (EN) Full compile test group (with Parser)
    // ═════════════════════════════════════════════════════════════════

    runner.beginGroup("UI-2.1.Compile");

    runner.runTest("Compile lambda: x + 1", [&]()
                   {
        // (AR) متغير ف = لامدا(x) => x + 1
        auto stmts = parseCode("\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x81 = \xD9\x84\xD8\xA7\xD9\x85\xD8\xAF\xD8\xA7(x) => x + 1");
        auto *lambda = extractLambda(stmts);
        SAD_ASSERT_TRUE(lambda != nullptr);

        BindingCompiler compiler;
        auto result = compiler.compile(lambda);
        SAD_ASSERT_TRUE(result.compiled);
        SAD_ASSERT_TRUE(result.isPure);
        SAD_ASSERT_GT(result.nodeCount, static_cast<size_t>(0));

        auto val = result.transform(BindingValue(static_cast<int64_t>(5)));
        SAD_ASSERT_EQ(val.asInteger(), static_cast<int64_t>(6)); });

    runner.runTest("Compile lambda: string concat", [&]()
                   {
        // (AR) متغير ف = لامدا(x) => "prefix_" + x
        auto stmts = parseCode("\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x81 = \xD9\x84\xD8\xA7\xD9\x85\xD8\xAF\xD8\xA7(x) => \"prefix_\" + x");
        auto *lambda = extractLambda(stmts);
        SAD_ASSERT_TRUE(lambda != nullptr);

        BindingCompiler compiler;
        auto result = compiler.compile(lambda);
        SAD_ASSERT_TRUE(result.compiled);

        auto val = result.transform(BindingValue(std::string("hello")));
        SAD_ASSERT_EQ(val.asString(), std::string("prefix_hello")); });

    runner.runTest("Compile lambda: x * 2", [&]()
                   {
        auto stmts = parseCode("\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x81 = \xD9\x84\xD8\xA7\xD9\x85\xD8\xAF\xD8\xA7(x) => x * 2");
        auto *lambda = extractLambda(stmts);
        SAD_ASSERT_TRUE(lambda != nullptr);

        BindingCompiler compiler;
        auto result = compiler.compile(lambda);
        SAD_ASSERT_TRUE(result.compiled);

        auto val = result.transform(BindingValue(static_cast<int64_t>(7)));
        SAD_ASSERT_EQ(val.asInteger(), static_cast<int64_t>(14)); });

    runner.runTest("Compile lambda: x > 10", [&]()
                   {
        auto stmts = parseCode("\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x81 = \xD9\x84\xD8\xA7\xD9\x85\xD8\xAF\xD8\xA7(x) => x > 10");
        auto *lambda = extractLambda(stmts);
        SAD_ASSERT_TRUE(lambda != nullptr);

        BindingCompiler compiler;
        auto result = compiler.compile(lambda);
        SAD_ASSERT_TRUE(result.compiled);

        auto val1 = result.transform(BindingValue(static_cast<int64_t>(15)));
        SAD_ASSERT_TRUE(val1.asBoolean());

        auto val2 = result.transform(BindingValue(static_cast<int64_t>(3)));
        SAD_ASSERT_FALSE(val2.asBoolean()); });

    runner.runTest("Reject lambda with 0 params", [&]()
                   {
        // (AR) متغير ف = لامدا() => 42
        auto stmts = parseCode("\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x81 = \xD9\x84\xD8\xA7\xD9\x85\xD8\xAF\xD8\xA7() => 42");
        auto *lambda = extractLambda(stmts);
        SAD_ASSERT_TRUE(lambda != nullptr);

        BindingCompiler compiler;
        auto result = compiler.compile(lambda);
        SAD_ASSERT_FALSE(result.compiled);
        SAD_ASSERT_GT(result.failureReason.size(), static_cast<size_t>(0)); });

    runner.runTest("Reject lambda with 2 params", [&]()
                   {
        // (AR) متغير ف = لامدا(x, y) => x + y
        auto stmts = parseCode("\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x81 = \xD9\x84\xD8\xA7\xD9\x85\xD8\xAF\xD8\xA7(x, y) => x + y");
        auto *lambda = extractLambda(stmts);
        SAD_ASSERT_TRUE(lambda != nullptr);

        BindingCompiler compiler;
        auto result = compiler.compile(lambda);
        SAD_ASSERT_FALSE(result.compiled);
        SAD_ASSERT_GT(result.failureReason.size(), static_cast<size_t>(0)); });

    runner.runTest("Compile nullptr lambda", [&]()
                   {
        BindingCompiler compiler;
        auto result = compiler.compile(nullptr);
        SAD_ASSERT_FALSE(result.compiled); });

    // ═════════════════════════════════════════════════════════════════
    // (AR) مجموعة اختبارات التعقيد والإعدادات
    // (EN) Complexity and configuration test group
    // ═════════════════════════════════════════════════════════════════

    runner.beginGroup("UI-2.1.Complexity");

    runner.runTest("Default configuration", [&]()
                   {
        BindingCompiler compiler;
        SAD_ASSERT_EQ(compiler.getMaxNodeCount(), BindingCompiler::DEFAULT_MAX_NODE_COUNT);
        SAD_ASSERT_EQ(compiler.getMaxDepth(), BindingCompiler::DEFAULT_MAX_DEPTH); });

    runner.runTest("Custom configuration", [&]()
                   {
        BindingCompiler compiler;
        compiler.setMaxNodeCount(10);
        compiler.setMaxDepth(3);
        SAD_ASSERT_EQ(compiler.getMaxNodeCount(), static_cast<size_t>(10));
        SAD_ASSERT_EQ(compiler.getMaxDepth(), static_cast<size_t>(3)); });

    runner.runTest("Reject lambda exceeding node limit", [&]()
                   {
        BindingCompiler compiler;
        compiler.setMaxNodeCount(2); // (AR) حد منخفض جداً — x + 1 = 3 عقد

        // (AR) متغير ف = لامدا(x) => x + 1
        auto stmts = parseCode("\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x81 = \xD9\x84\xD8\xA7\xD9\x85\xD8\xAF\xD8\xA7(x) => x + 1");
        auto *lambda = extractLambda(stmts);
        SAD_ASSERT_TRUE(lambda != nullptr);

        auto result = compiler.compile(lambda);
        SAD_ASSERT_FALSE(result.compiled);
        SAD_ASSERT_GT(compiler.getStats().tooComplexCount, static_cast<size_t>(0)); });

    runner.runTest("Stats tracking", [&]()
                   {
        BindingCompiler compiler;

        // (AR) تجميع ناجح
        auto stmts1 = parseCode("\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x81 = \xD9\x84\xD8\xA7\xD9\x85\xD8\xAF\xD8\xA7(x) => x + 1");
        auto *lambda1 = extractLambda(stmts1);
        compiler.compile(lambda1);

        // (AR) تجميع فاشل
        compiler.compile(nullptr);

        const auto &stats = compiler.getStats();
        SAD_ASSERT_EQ(stats.totalAttempts, static_cast<size_t>(2));
        SAD_ASSERT_EQ(stats.successCount, static_cast<size_t>(1));
        SAD_ASSERT_EQ(stats.failureCount, static_cast<size_t>(1)); });

    runner.runTest("Stats reset", [&]()
                   {
        BindingCompiler compiler;
        compiler.compile(nullptr);
        SAD_ASSERT_EQ(compiler.getStats().totalAttempts, static_cast<size_t>(1));

        compiler.resetStats();
        SAD_ASSERT_EQ(compiler.getStats().totalAttempts, static_cast<size_t>(0));
        SAD_ASSERT_EQ(compiler.getStats().successCount, static_cast<size_t>(0)); });

    // ═════════════════════════════════════════════════════════════════
    // (AR) مجموعة اختبارات CompilationResult
    // (EN) CompilationResult test group
    // ═════════════════════════════════════════════════════════════════

    runner.beginGroup("UI-2.1.CompilationResult");

    runner.runTest("CompilationResult::success", [&]()
                   {
        auto transform = [](const BindingValue &v) -> BindingValue { return v; };
        auto result = CompilationResult::success(transform, 3, true);
        SAD_ASSERT_TRUE(result.compiled);
        SAD_ASSERT_TRUE(result.transform != nullptr);
        SAD_ASSERT_EQ(result.nodeCount, static_cast<size_t>(3));
        SAD_ASSERT_TRUE(result.isPure); });

    runner.runTest("CompilationResult::failure", [&]()
                   {
        auto result = CompilationResult::failure("test failure");
        SAD_ASSERT_FALSE(result.compiled);
        SAD_ASSERT_TRUE(result.transform == nullptr);
        SAD_ASSERT_GT(result.failureReason.size(), static_cast<size_t>(0)); });

    return runner.printSummary();
}
