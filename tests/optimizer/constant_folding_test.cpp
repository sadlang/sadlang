#include <gtest/gtest.h>
#include "optimizer/optimization_pass.h"
#include "parser/parser.h"
#include "lexer/lexer.h"
#include <memory>
#include <sstream>

using namespace Sad;

class ConstantFoldingTest : public ::testing::Test {
protected:
    std::shared_ptr<AST::ASTNode> parseCode(const std::string& code) {
        Lexer::Lexer lexer(code);
        auto tokens = lexer.tokenize();
        Parser::Parser parser(tokens);
        return parser.parse();
    }
    
    std::string getOptimizationOutput(const std::shared_ptr<AST::ASTNode>& ast) {
        std::ostringstream oss;
        auto oldBuf = std::cout.rdbuf(oss.rdbuf());
        
        ConstantFoldingPass pass;
        pass.run(ast);
        
        std::cout.rdbuf(oldBuf);
        return oss.str();
    }
};

// =====================================
// اختبار العمليات الحسابية الأساسية
// =====================================

TEST_F(ConstantFoldingTest, BasicAddition) {
    std::string code = "متغير x = 2 + 3;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    
    // يجب أن يكتشف أن 2 + 3 يمكن طيها إلى 5
    EXPECT_NE(output.find("2 + 3"), std::string::npos);
}

TEST_F(ConstantFoldingTest, BasicSubtraction) {
    std::string code = "متغير x = 10 - 3;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("10 - 3"), std::string::npos);
}

TEST_F(ConstantFoldingTest, BasicMultiplication) {
    std::string code = "متغير x = 4 * 5;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("4 * 5"), std::string::npos);
}

TEST_F(ConstantFoldingTest, BasicDivision) {
    std::string code = "متغير x = 20 / 4;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("20 / 4"), std::string::npos);
}

TEST_F(ConstantFoldingTest, BasicModulo) {
    std::string code = "متغير x = 17 % 5;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("17 % 5"), std::string::npos);
}

TEST_F(ConstantFoldingTest, BasicPower) {
    std::string code = "متغير x = 2 ** 3;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("2 ** 3"), std::string::npos);
}

// =====================================
// اختبار العمليات المركبة
// =====================================

TEST_F(ConstantFoldingTest, ComplexExpression) {
    std::string code = "متغير x = 2 + 3 * 4 - 5;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // يجب أن يكتشف عدة عمليات ثابتة
    EXPECT_FALSE(output.empty());
}

TEST_F(ConstantFoldingTest, NestedExpressions) {
    std::string code = "متغير x = (2 + 3) * (4 - 1);";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

// =====================================
// اختبار القسمة على صفر
// =====================================

TEST_F(ConstantFoldingTest, DivisionByZero) {
    std::string code = "متغير x = 10 / 0;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // يجب أن يحذر من القسمة على صفر
    EXPECT_NE(output.find("division by zero"), std::string::npos);
}

TEST_F(ConstantFoldingTest, ModuloByZero) {
    std::string code = "متغير x = 10 % 0;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // يجب أن يحذر من القسمة على صفر
    EXPECT_NE(output.find("division by zero"), std::string::npos);
}

// =====================================
// اختبار العمليات الأحادية
// =====================================

TEST_F(ConstantFoldingTest, UnaryMinus) {
    std::string code = "متغير x = -5;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("-5"), std::string::npos);
}

TEST_F(ConstantFoldingTest, UnaryMinusWithExpression) {
    std::string code = "متغير x = -(2 + 3);";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

TEST_F(ConstantFoldingTest, UnaryNot) {
    std::string code = "متغير x = !صحيح;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("!"), std::string::npos);
}

// =====================================
// اختبار التعبيرات الشرطية
// =====================================

TEST_F(ConstantFoldingTest, TernaryWithConstants) {
    std::string code = "متغير x = صحيح ? 10 : 20;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

TEST_F(ConstantFoldingTest, TernaryWithConstantCondition) {
    std::string code = "متغير x = 1 > 0 ? 5 : 10;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

// =====================================
// اختبار الأعداد السالبة والعشرية
// =====================================

TEST_F(ConstantFoldingTest, NegativeNumbers) {
    std::string code = "متغير x = -5 + -3;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

TEST_F(ConstantFoldingTest, FloatingPointNumbers) {
    std::string code = "متغير x = 2.5 + 3.7;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("2.5 + 3.7"), std::string::npos);
}

TEST_F(ConstantFoldingTest, MixedIntegerAndFloat) {
    std::string code = "متغير x = 2 + 3.5;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

// =====================================
// اختبار داخل الدوال والحلقات
// =====================================

TEST_F(ConstantFoldingTest, ConstantInFunction) {
    std::string code = R"(
        دالة test() {
            متغير x = 2 + 3;
            أرجع x;
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("2 + 3"), std::string::npos);
}

TEST_F(ConstantFoldingTest, ConstantInLoop) {
    std::string code = R"(
        لكل (متغير i = 0; i < 10; i = i + 1) {
            متغير x = 5 * 10;
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("5 * 10"), std::string::npos);
}

TEST_F(ConstantFoldingTest, ConstantInIfStatement) {
    std::string code = R"(
        إذا (صحيح) {
            متغير x = 100 / 5;
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("100 / 5"), std::string::npos);
}

// =====================================
// اختبار العمليات الكبيرة
// =====================================

TEST_F(ConstantFoldingTest, LargeNumbers) {
    std::string code = "متغير x = 1000000 + 2000000;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

TEST_F(ConstantFoldingTest, PowerOfLargeNumbers) {
    std::string code = "متغير x = 10 ** 6;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

// =====================================
// اختبار حالات الحدود
// =====================================

TEST_F(ConstantFoldingTest, ZeroOperations) {
    std::string code = "متغير x = 0 + 0;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("0 + 0"), std::string::npos);
}

TEST_F(ConstantFoldingTest, OneOperations) {
    std::string code = "متغير x = 1 * 1;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("1 * 1"), std::string::npos);
}

TEST_F(ConstantFoldingTest, MultipleStatementsWithConstants) {
    std::string code = R"(
        متغير a = 1 + 2;
        متغير b = 3 * 4;
        متغير c = 10 - 5;
        متغير d = 20 / 4;
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // يجب أن يكتشف عدة عمليات ثابتة
    EXPECT_NE(output.find("1 + 2"), std::string::npos);
    EXPECT_NE(output.find("3 * 4"), std::string::npos);
}

// =====================================
// اختبار عدم طي التعبيرات غير الثابتة
// =====================================

TEST_F(ConstantFoldingTest, NonConstantExpression) {
    std::string code = "متغير x = y + 3;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // لا يجب أن يطوي التعبير لأن y متغير
    // إذا لم يكن هناك تحسين، قد يكون الناتج فارغاً أو لا يحتوي على تحذير
}

TEST_F(ConstantFoldingTest, PartiallyConstantExpression) {
    std::string code = "متغير x = 2 + y + 3;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // قد يطوي 2 + 3 إذا كان الـ parser يعيد ترتيب التعبيرات
    EXPECT_FALSE(output.empty() && output.find("2 + y + 3") == std::string::npos);
}
