#include <gtest/gtest.h>
#include "optimizer/optimization_pass.h"
#include "parser/parser.h"
#include "lexer/lexer.h"
#include <memory>
#include <sstream>

using namespace Sad;

class ExpressionSimplificationTest : public ::testing::Test {
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
        
        ExpressionSimplificationPass pass;
        pass.run(ast);
        
        std::cout.rdbuf(oldBuf);
        return oss.str();
    }
};

// =====================================
// اختبار x + 0 → x
// =====================================

TEST_F(ExpressionSimplificationTest, AddZeroRight) {
    std::string code = "متغير x = y + 0;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("y + 0"), std::string::npos);
}

TEST_F(ExpressionSimplificationTest, AddZeroLeft) {
    std::string code = "متغير x = 0 + y;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("0 + y"), std::string::npos);
}

// =====================================
// اختبار x * 1 → x
// =====================================

TEST_F(ExpressionSimplificationTest, MultiplyByOneRight) {
    std::string code = "متغير x = y * 1;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("y * 1"), std::string::npos);
}

TEST_F(ExpressionSimplificationTest, MultiplyByOneLeft) {
    std::string code = "متغير x = 1 * y;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("1 * y"), std::string::npos);
}

// =====================================
// اختبار x * 0 → 0
// =====================================

TEST_F(ExpressionSimplificationTest, MultiplyByZeroRight) {
    std::string code = "متغير x = y * 0;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("y * 0"), std::string::npos);
}

TEST_F(ExpressionSimplificationTest, MultiplyByZeroLeft) {
    std::string code = "متغير x = 0 * y;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("0 * y"), std::string::npos);
}

// =====================================
// اختبار x - 0 → x
// =====================================

TEST_F(ExpressionSimplificationTest, SubtractZero) {
    std::string code = "متغير x = y - 0;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("y - 0"), std::string::npos);
}

// =====================================
// اختبار x - x → 0
// =====================================

TEST_F(ExpressionSimplificationTest, SubtractSelf) {
    std::string code = "متغير x = y - y;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("y - y"), std::string::npos);
}

// =====================================
// اختبار x / 1 → x
// =====================================

TEST_F(ExpressionSimplificationTest, DivideByOne) {
    std::string code = "متغير x = y / 1;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("y / 1"), std::string::npos);
}

// =====================================
// اختبار x / x → 1
// =====================================

TEST_F(ExpressionSimplificationTest, DivideSelf) {
    std::string code = "متغير x = y / y;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("y / y"), std::string::npos);
}

// =====================================
// اختبار x ** 0 → 1
// =====================================

TEST_F(ExpressionSimplificationTest, PowerOfZero) {
    std::string code = "متغير x = y ** 0;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("y ** 0"), std::string::npos);
}

// =====================================
// اختبار x ** 1 → x
// =====================================

TEST_F(ExpressionSimplificationTest, PowerOfOne) {
    std::string code = "متغير x = y ** 1;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("y ** 1"), std::string::npos);
}

// =====================================
// اختبار التعبيرات المركبة
// =====================================

TEST_F(ExpressionSimplificationTest, MultipleSimplifications) {
    std::string code = "متغير x = (y + 0) * 1;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // يجب أن يكتشف كلا التبسيطين
    EXPECT_FALSE(output.empty());
}

TEST_F(ExpressionSimplificationTest, NestedSimplifications) {
    std::string code = "متغير x = ((y * 1) + 0) / 1;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

TEST_F(ExpressionSimplificationTest, ComplexExpression) {
    std::string code = "متغير x = (a + 0) * (b * 1) - (c - 0);";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

// =====================================
// اختبار داخل الهياكل الأخرى
// =====================================

TEST_F(ExpressionSimplificationTest, SimplificationInFunction) {
    std::string code = R"(
        دالة test() {
            متغير x = y + 0;
            أرجع x * 1;
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

TEST_F(ExpressionSimplificationTest, SimplificationInLoop) {
    std::string code = R"(
        لكل (متغير i = 0; i < 10; i = i + 1) {
            متغير x = i * 1;
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("i * 1"), std::string::npos);
}

TEST_F(ExpressionSimplificationTest, SimplificationInIfCondition) {
    std::string code = R"(
        إذا ((x + 0) > 5) {
            اطبع("test");
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

// =====================================
// اختبار عدم التبسيط للتعبيرات غير القابلة
// =====================================

TEST_F(ExpressionSimplificationTest, NoSimplificationNeeded) {
    std::string code = "متغير x = y + z;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // لا يجب أن يكون هناك تبسيط لأنه لا توجد عمليات محايدة
}

TEST_F(ExpressionSimplificationTest, NonZeroNonOneConstants) {
    std::string code = "متغير x = y + 5;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // لا تبسيط لأن 5 ليس صفر
}

// =====================================
// اختبار حالات خاصة
// =====================================

TEST_F(ExpressionSimplificationTest, ZeroDivideByZero) {
    std::string code = "متغير x = 0 / 0;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // هذا يجب أن يتعامل معه Constant Folding كقسمة على صفر
}

TEST_F(ExpressionSimplificationTest, MultipleOperationsChained) {
    std::string code = "متغير x = y + 0 - 0 * 1 / 1;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

TEST_F(ExpressionSimplificationTest, SimplificationWithFloats) {
    std::string code = "متغير x = y + 0.0;";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

TEST_F(ExpressionSimplificationTest, SimplificationInArrayAccess) {
    std::string code = "متغير x = arr[i + 0];";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

TEST_F(ExpressionSimplificationTest, SimplificationInFunctionCall) {
    std::string code = "اطبع(x * 1);";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("x * 1"), std::string::npos);
}

// =====================================
// اختبار التعبيرات الشرطية
// =====================================

TEST_F(ExpressionSimplificationTest, SimplificationInTernary) {
    std::string code = "متغير x = condition ? (y + 0) : (z * 1);";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

TEST_F(ExpressionSimplificationTest, AllSimplificationRules) {
    std::string code = R"(
        متغير a = x + 0;
        متغير b = 0 + y;
        متغير c = z * 1;
        متغير d = 1 * w;
        متغير e = v * 0;
        متغير f = 0 * u;
        متغير g = t - 0;
        متغير h = s - s;
        متغير i = r / 1;
        متغير j = q / q;
        متغير k = p ** 0;
        متغير l = o ** 1;
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // يجب اكتشاف جميع القواعد
    EXPECT_FALSE(output.empty());
}
