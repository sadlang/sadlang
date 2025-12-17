#include <gtest/gtest.h>
#include "optimizer/optimization_pass.h"
#include "parser/parser.h"
#include "lexer/lexer.h"
#include <memory>
#include <sstream>

using namespace Sad;

class DeadCodeEliminationTest : public ::testing::Test {
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
        
        DeadCodeEliminationPass pass;
        pass.run(ast);
        
        std::cout.rdbuf(oldBuf);
        return oss.str();
    }
};

// =====================================
// اختبار المتغيرات غير المستخدمة
// =====================================

TEST_F(DeadCodeEliminationTest, UnusedVariable) {
    std::string code = R"(
        متغير x = 5;
        متغير y = 10;
        اطبع(y);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // يجب أن يكتشف أن x غير مستخدم
    EXPECT_NE(output.find("unused"), std::string::npos);
}

TEST_F(DeadCodeEliminationTest, MultipleUnusedVariables) {
    std::string code = R"(
        متغير a = 1;
        متغير b = 2;
        متغير c = 3;
        متغير d = 4;
        اطبع(c);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // يجب أن يكتشف a, b, d كمتغيرات غير مستخدمة
    EXPECT_FALSE(output.empty());
}

TEST_F(DeadCodeEliminationTest, AllVariablesUsed) {
    std::string code = R"(
        متغير x = 5;
        متغير y = 10;
        اطبع(x + y);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // لا يجب أن يكون هناك تحذيرات لأن جميع المتغيرات مستخدمة
}

// =====================================
// اختبار الكود غير القابل للوصول
// =====================================

TEST_F(DeadCodeEliminationTest, UnreachableAfterReturn) {
    std::string code = R"(
        دالة test() {
            أرجع 5;
            اطبع("unreachable");
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // يجب أن يكتشف الكود غير القابل للوصول
    EXPECT_NE(output.find("unreachable"), std::string::npos);
}

TEST_F(DeadCodeEliminationTest, MultipleStatementsAfterReturn) {
    std::string code = R"(
        دالة test() {
            أرجع 10;
            متغير x = 5;
            اطبع(x);
            متغير y = 20;
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("unreachable"), std::string::npos);
}

TEST_F(DeadCodeEliminationTest, NoUnreachableCode) {
    std::string code = R"(
        دالة test() {
            متغير x = 5;
            اطبع(x);
            أرجع x;
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // لا يجب أن يكون هناك تحذير
}

// =====================================
// اختبار الكود الميت في الجمل الشرطية
// =====================================

TEST_F(DeadCodeEliminationTest, UnreachableInIfBranch) {
    std::string code = R"(
        دالة test() {
            إذا (صحيح) {
                أرجع 5;
                اطبع("dead");
            }
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("unreachable"), std::string::npos);
}

TEST_F(DeadCodeEliminationTest, UnreachableInElseBranch) {
    std::string code = R"(
        دالة test() {
            إذا (condition) {
                اطبع("then");
            } وإلا {
                أرجع 10;
                اطبع("dead");
            }
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("unreachable"), std::string::npos);
}

TEST_F(DeadCodeEliminationTest, BothBranchesReturn) {
    std::string code = R"(
        دالة test() {
            إذا (condition) {
                أرجع 5;
            } وإلا {
                أرجع 10;
            }
            اطبع("unreachable");
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("unreachable"), std::string::npos);
}

// =====================================
// اختبار المتغيرات المعرفة ولكن غير مستخدمة
// =====================================

TEST_F(DeadCodeEliminationTest, VariableDefinedButNeverRead) {
    std::string code = R"(
        متغير x = calculateValue();
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("unused"), std::string::npos);
}

TEST_F(DeadCodeEliminationTest, VariableUsedInExpression) {
    std::string code = R"(
        متغير x = 5;
        متغير y = x + 10;
        اطبع(y);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // x مستخدم في تعبير، لا يجب أن يكون ميتاً
}

// =====================================
// اختبار الكود الميت في الحلقات
// =====================================

TEST_F(DeadCodeEliminationTest, UnreachableInLoop) {
    std::string code = R"(
        لكل (متغير i = 0; i < 10; i = i + 1) {
            أرجع i;
            اطبع("dead");
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("unreachable"), std::string::npos);
}

TEST_F(DeadCodeEliminationTest, UnusedVariableInLoop) {
    std::string code = R"(
        لكل (متغير i = 0; i < 10; i = i + 1) {
            متغير x = i * 2;
            اطبع(i);
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("unused"), std::string::npos);
}

// =====================================
// اختبار المعاملات والدوال
// =====================================

TEST_F(DeadCodeEliminationTest, UnusedParameter) {
    std::string code = R"(
        دالة test(x, y) {
            أرجع x;
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // y غير مستخدم
    EXPECT_NE(output.find("unused"), std::string::npos);
}

TEST_F(DeadCodeEliminationTest, AllParametersUsed) {
    std::string code = R"(
        دالة add(x, y) {
            أرجع x + y;
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // جميع المعاملات مستخدمة
}

// =====================================
// اختبار حالات معقدة
// =====================================

TEST_F(DeadCodeEliminationTest, ComplexDeadCode) {
    std::string code = R"(
        دالة test() {
            متغير a = 1;
            متغير b = 2;
            متغير c = 3;
            
            إذا (a > 0) {
                أرجع b;
            } وإلا {
                أرجع b;
            }
            
            اطبع(c);
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // c غير مستخدم و print(c) غير قابل للوصول
    EXPECT_FALSE(output.empty());
}

TEST_F(DeadCodeEliminationTest, NestedFunctionsWithDeadCode) {
    std::string code = R"(
        دالة outer() {
            دالة inner() {
                متغير x = 5;
                أرجع 10;
                اطبع(x);
            }
            أرجع inner();
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

// =====================================
// اختبار المتغيرات في النطاقات المختلفة
// =====================================

TEST_F(DeadCodeEliminationTest, VariableShadowing) {
    std::string code = R"(
        متغير x = 5;
        {
            متغير x = 10;
            اطبع(x);
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // المتغير الخارجي x غير مستخدم
    EXPECT_FALSE(output.empty());
}

TEST_F(DeadCodeEliminationTest, GlobalAndLocalVariables) {
    std::string code = R"(
        متغير global = 100;
        
        دالة test() {
            متغير local = 50;
            أرجع global;
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // local غير مستخدم
    EXPECT_NE(output.find("unused"), std::string::npos);
}

// =====================================
// اختبار الإسنادات المتعددة
// =====================================

TEST_F(DeadCodeEliminationTest, OverwrittenVariable) {
    std::string code = R"(
        متغير x = 5;
        x = 10;
        x = 15;
        اطبع(x);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // هذا يُعتبر redundant assignment وليس dead code
}

TEST_F(DeadCodeEliminationTest, VariableUsedBeforeOverwrite) {
    std::string code = R"(
        متغير x = 5;
        اطبع(x);
        x = 10;
        اطبع(x);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // جميع الإسنادات لها استخدام
}

// =====================================
// اختبار حالات خاصة
// =====================================

TEST_F(DeadCodeEliminationTest, EmptyFunction) {
    std::string code = R"(
        دالة empty() {
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // لا يجب أن يكون هناك كود ميت في دالة فارغة
}

TEST_F(DeadCodeEliminationTest, OnlyReturnStatement) {
    std::string code = R"(
        دالة test() {
            أرجع 42;
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // لا كود ميت
}

TEST_F(DeadCodeEliminationTest, ComplexControlFlow) {
    std::string code = R"(
        دالة test(condition) {
            متغير x = 1;
            متغير y = 2;
            متغير z = 3;
            
            إذا (condition) {
                اطبع(x);
                أرجع y;
            }
            
            اطبع(z);
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // جميع المتغيرات مستخدمة في مسارات مختلفة
}
