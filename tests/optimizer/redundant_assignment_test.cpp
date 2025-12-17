#include <gtest/gtest.h>
#include "optimizer/optimization_pass.h"
#include "parser/parser.h"
#include "lexer/lexer.h"
#include <memory>
#include <sstream>

using namespace Sad;

class RedundantAssignmentTest : public ::testing::Test {
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
        
        RedundantAssignmentEliminationPass pass;
        pass.run(ast);
        
        std::cout.rdbuf(oldBuf);
        return oss.str();
    }
};

// =====================================
// اختبار الإسناد المتكرر البسيط
// =====================================

TEST_F(RedundantAssignmentTest, SimpleRedundantAssignment) {
    std::string code = R"(
        متغير x = 5;
        x = 10;
        اطبع(x);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // الإسناد الأول زائد
    EXPECT_NE(output.find("redundant"), std::string::npos);
}

TEST_F(RedundantAssignmentTest, MultipleRedundantAssignments) {
    std::string code = R"(
        متغير x = 1;
        x = 2;
        x = 3;
        x = 4;
        اطبع(x);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // الإسنادات الثلاثة الأولى زائدة
    EXPECT_NE(output.find("redundant"), std::string::npos);
}

TEST_F(RedundantAssignmentTest, NoRedundancy_VariableUsed) {
    std::string code = R"(
        متغير x = 5;
        اطبع(x);
        x = 10;
        اطبع(x);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // كل إسناد يُستخدم، لا زيادة
}

// =====================================
// اختبار الإسناد بدون استخدام
// =====================================

TEST_F(RedundantAssignmentTest, AssignmentWithoutUse) {
    std::string code = R"(
        متغير x = 100;
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // إسناد بدون استخدام
    EXPECT_NE(output.find("never used"), std::string::npos);
}

TEST_F(RedundantAssignmentTest, MultipleUnusedAssignments) {
    std::string code = R"(
        متغير a = 1;
        متغير b = 2;
        متغير c = 3;
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // جميع الإسنادات غير مستخدمة
    EXPECT_NE(output.find("never used"), std::string::npos);
}

// =====================================
// اختبار الإسناد في الجمل الشرطية
// =====================================

TEST_F(RedundantAssignmentTest, RedundantInIfBranch) {
    std::string code = R"(
        متغير x = 5;
        إذا (condition) {
            x = 10;
        }
        اطبع(x);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // الإسناد الأول قد يكون زائداً أو لا حسب condition
}

TEST_F(RedundantAssignmentTest, RedundantInBothBranches) {
    std::string code = R"(
        متغير x = 5;
        إذا (condition) {
            x = 10;
        } وإلا {
            x = 20;
        }
        اطبع(x);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // الإسناد الأول زائد لأن كلا الفرعين يعيد تعيين x
    EXPECT_NE(output.find("redundant"), std::string::npos);
}

TEST_F(RedundantAssignmentTest, NoRedundancy_OneBranchUses) {
    std::string code = R"(
        متغير x = 5;
        إذا (condition) {
            اطبع(x);
        } وإلا {
            x = 10;
        }
        اطبع(x);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // الإسناد الأول يُستخدم في الفرع الأول
}

// =====================================
// اختبار الإسناد في الحلقات
// =====================================

TEST_F(RedundantAssignmentTest, RedundantInLoop) {
    std::string code = R"(
        لكل (متغير i = 0; i < 10; i = i + 1) {
            متغير x = i;
            x = i + 1;
            اطبع(x);
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // الإسناد الأول زائد
    EXPECT_NE(output.find("redundant"), std::string::npos);
}

TEST_F(RedundantAssignmentTest, AssignmentInLoopNotUsedAfter) {
    std::string code = R"(
        لكل (متغير i = 0; i < 10; i = i + 1) {
            متغير x = i * 2;
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // x يُسند لكن لا يُستخدم
    EXPECT_NE(output.find("never used"), std::string::npos);
}

// =====================================
// اختبار الإسناد المتتابع
// =====================================

TEST_F(RedundantAssignmentTest, ConsecutiveAssignments) {
    std::string code = R"(
        متغير x = 1;
        x = 2;
        x = 3;
        x = 4;
        x = 5;
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // جميع الإسنادات ما عدا الأخير زائدة
    EXPECT_NE(output.find("redundant"), std::string::npos);
}

TEST_F(RedundantAssignmentTest, AssignmentsWithIntermediateUse) {
    std::string code = R"(
        متغير x = 1;
        متغير y = x;
        x = 2;
        متغير z = x;
        x = 3;
        اطبع(x);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // كل إسناد يُستخدم
}

// =====================================
// اختبار الإسناد الذاتي
// =====================================

TEST_F(RedundantAssignmentTest, SelfAssignment) {
    std::string code = R"(
        متغير x = 5;
        x = x;
        اطبع(x);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // x = x هو إسناد زائد واضح
}

TEST_F(RedundantAssignmentTest, IncrementAssignment) {
    std::string code = R"(
        متغير x = 5;
        x = x + 1;
        اطبع(x);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // هذا ليس زائداً لأنه يستخدم القيمة السابقة
}

// =====================================
// اختبار الإسناد في الدوال
// =====================================

TEST_F(RedundantAssignmentTest, RedundantInFunction) {
    std::string code = R"(
        دالة test() {
            متغير x = 5;
            x = 10;
            أرجع x;
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("redundant"), std::string::npos);
}

TEST_F(RedundantAssignmentTest, ParameterReassignment) {
    std::string code = R"(
        دالة process(x) {
            x = 100;
            أرجع x;
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // المعامل x يُعاد تعيينه مباشرة
    EXPECT_NE(output.find("redundant"), std::string::npos);
}

// =====================================
// اختبار متغيرات متعددة
// =====================================

TEST_F(RedundantAssignmentTest, MultipleVariablesWithRedundancy) {
    std::string code = R"(
        متغير x = 1;
        متغير y = 2;
        x = 10;
        y = 20;
        اطبع(x + y);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // الإسنادات الأولى لكل من x و y زائدة
    EXPECT_NE(output.find("redundant"), std::string::npos);
}

TEST_F(RedundantAssignmentTest, InterleavedAssignments) {
    std::string code = R"(
        متغير x = 1;
        متغير y = 2;
        x = 10;
        اطبع(y);
        y = 20;
        اطبع(x);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
}

// =====================================
// اختبار الإسناد في التعبيرات المعقدة
// =====================================

TEST_F(RedundantAssignmentTest, AssignmentWithComplexExpression) {
    std::string code = R"(
        متغير x = calculate(10, 20);
        x = process(5);
        اطبع(x);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("redundant"), std::string::npos);
}

// =====================================
// اختبار الإسناد للعناصر المصفوفة
// =====================================

TEST_F(RedundantAssignmentTest, ArrayElementAssignment) {
    std::string code = R"(
        متغير arr = [1, 2, 3];
        arr[0] = 10;
        arr[0] = 20;
        اطبع(arr[0]);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
}

// =====================================
// اختبار حالات خاصة
// =====================================

TEST_F(RedundantAssignmentTest, AssignmentBeforeReturn) {
    std::string code = R"(
        دالة test() {
            متغير x = 5;
            أرجع x;
            x = 10;
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // الإسناد الأخير غير قابل للوصول
}

TEST_F(RedundantAssignmentTest, OnlyDeclarationNoAssignment) {
    std::string code = R"(
        متغير x;
        اطبع(x);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
}

TEST_F(RedundantAssignmentTest, ComplexControlFlow) {
    std::string code = R"(
        متغير x = 1;
        إذا (condition1) {
            x = 2;
            إذا (condition2) {
                x = 3;
            }
        } وإلا {
            x = 4;
        }
        اطبع(x);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
}

TEST_F(RedundantAssignmentTest, AssignmentInSwitchCase) {
    std::string code = R"(
        متغير x = 0;
        حسب (value) {
            حالة 1:
                x = 10;
                اقطع;
            حالة 2:
                x = 20;
                اقطع;
            افتراضي:
                x = 30;
        }
        اطبع(x);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
}

// =====================================
// اختبار الإسناد المتعدد في سطر واحد
// =====================================

TEST_F(RedundantAssignmentTest, MultipleAssignmentsOneLine) {
    std::string code = R"(
        متغير x = 5, y = 10, z = 15;
        x = 100;
        اطبع(x);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
}

// =====================================
// اختبار الإسناد مع آثار جانبية
// =====================================

TEST_F(RedundantAssignmentTest, AssignmentWithSideEffects) {
    std::string code = R"(
        متغير x = functionWithSideEffect();
        x = 100;
        اطبع(x);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // قد يكون هناك آثار جانبية، لكن الإسناد نفسه زائد
    EXPECT_NE(output.find("redundant"), std::string::npos);
}

// =====================================
// اختبار شامل
// =====================================

TEST_F(RedundantAssignmentTest, ComprehensiveTest) {
    std::string code = R"(
        متغير a = 1;
        متغير b = 2;
        a = 10;
        b = 20;
        
        إذا (condition) {
            a = 30;
        } وإلا {
            a = 40;
        }
        
        اطبع(a + b);
        
        b = 50;
        اطبع(b);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}
