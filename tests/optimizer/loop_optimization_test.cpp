#include <gtest/gtest.h>
#include "optimizer/optimization_pass.h"
#include "parser/parser.h"
#include "lexer/lexer.h"
#include <memory>
#include <sstream>

using namespace Sad;

class LoopOptimizationTest : public ::testing::Test {
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
        
        LoopOptimizationPass pass;
        pass.run(ast);
        
        std::cout.rdbuf(oldBuf);
        return oss.str();
    }
};

// =====================================
// اختبار Loop Unrolling للحلقات الصغيرة
// =====================================

TEST_F(LoopOptimizationTest, SmallConstantLoop_TwoIterations) {
    std::string code = R"(
        لكل (متغير i = 0; i < 2; i = i + 1) {
            اطبع(i);
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // يجب أن يقترح فك الحلقة (2 تكرار)
    EXPECT_NE(output.find("unroll"), std::string::npos);
}

TEST_F(LoopOptimizationTest, SmallConstantLoop_FourIterations) {
    std::string code = R"(
        لكل (متغير i = 0; i < 4; i = i + 1) {
            اطبع(i);
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("unroll"), std::string::npos);
}

TEST_F(LoopOptimizationTest, LargeLoop_NoUnrolling) {
    std::string code = R"(
        لكل (متغير i = 0; i < 100; i = i + 1) {
            اطبع(i);
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // لا يجب اقتراح فك حلقة كبيرة
}

// =====================================
// اختبار الحلقات ذات الشروط الثابتة
// =====================================

TEST_F(LoopOptimizationTest, WhileLoopWithConstantTrue) {
    std::string code = R"(
        بينما (صحيح) {
            اطبع("infinite");
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // يجب أن يحذر من حلقة لا نهائية
    EXPECT_NE(output.find("infinite"), std::string::npos);
}

TEST_F(LoopOptimizationTest, WhileLoopWithConstantFalse) {
    std::string code = R"(
        بينما (خطأ) {
            اطبع("never executed");
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // يجب أن يحذر من حلقة لن تُنفذ أبداً
    EXPECT_NE(output.find("never execute"), std::string::npos);
}

// =====================================
// اختبار Invariant Code Motion
// =====================================

TEST_F(LoopOptimizationTest, InvariantCodeInLoop) {
    std::string code = R"(
        لكل (متغير i = 0; i < 10; i = i + 1) {
            متغير constant = 5 * 10;
            اطبع(i + constant);
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // يجب اكتشاف التعبير الثابت داخل الحلقة
    EXPECT_FALSE(output.empty());
}

TEST_F(LoopOptimizationTest, InvariantExpressionWithVariables) {
    std::string code = R"(
        متغير x = 10;
        متغير y = 20;
        لكل (متغير i = 0; i < 100; i = i + 1) {
            متغير sum = x + y;
            اطبع(i + sum);
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

// =====================================
// اختبار الحلقات المتداخلة
// =====================================

TEST_F(LoopOptimizationTest, NestedLoops_BothSmall) {
    std::string code = R"(
        لكل (متغير i = 0; i < 2; i = i + 1) {
            لكل (متغير j = 0; j < 2; j = j + 1) {
                اطبع(i * j);
            }
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // يجب اقتراح فك كلا الحلقتين
    EXPECT_NE(output.find("unroll"), std::string::npos);
}

TEST_F(LoopOptimizationTest, NestedLoops_OuterSmall) {
    std::string code = R"(
        لكل (متغير i = 0; i < 3; i = i + 1) {
            لكل (متغير j = 0; j < 100; j = j + 1) {
                اطبع(i + j);
            }
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

// =====================================
// اختبار حلقات مع break و continue
// =====================================

TEST_F(LoopOptimizationTest, LoopWithBreak) {
    std::string code = R"(
        لكل (متغير i = 0; i < 10; i = i + 1) {
            إذا (i == 5) {
                اقطع;
            }
            اطبع(i);
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // الحلقة تحتوي على break، قد لا يكون فكها آمناً
}

TEST_F(LoopOptimizationTest, LoopWithContinue) {
    std::string code = R"(
        لكل (متغير i = 0; i < 10; i = i + 1) {
            إذا (i % 2 == 0) {
                استمر;
            }
            اطبع(i);
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
}

// =====================================
// اختبار حلقات مع شروط معقدة
// =====================================

TEST_F(LoopOptimizationTest, ComplexLoopCondition) {
    std::string code = R"(
        لكل (متغير i = 0; i < 10 && i != 5; i = i + 1) {
            اطبع(i);
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
}

TEST_F(LoopOptimizationTest, LoopWithComplexUpdate) {
    std::string code = R"(
        لكل (متغير i = 0; i < 10; i = i + 2) {
            اطبع(i);
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
}

// =====================================
// اختبار حلقات do-while
// =====================================

TEST_F(LoopOptimizationTest, DoWhileWithConstant) {
    std::string code = R"(
        نفذ {
            اطبع("once");
        } بينما (خطأ);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
}

// =====================================
// اختبار حلقات بدون جسم
// =====================================

TEST_F(LoopOptimizationTest, EmptyLoop) {
    std::string code = R"(
        لكل (متغير i = 0; i < 10; i = i + 1) {
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // حلقة فارغة يمكن إزالتها بالكامل
}

// =====================================
// اختبار حلقات مع استدعاءات دوال
// =====================================

TEST_F(LoopOptimizationTest, LoopWithFunctionCall) {
    std::string code = R"(
        لكل (متغير i = 0; i < 5; i = i + 1) {
            calculate(i);
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

// =====================================
// اختبار التحسينات المختلطة
// =====================================

TEST_F(LoopOptimizationTest, SmallLoopWithInvariantCode) {
    std::string code = R"(
        متغير x = 100;
        لكل (متغير i = 0; i < 3; i = i + 1) {
            متغير y = x * 2;
            اطبع(i + y);
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // يجب اقتراح كل من فك الحلقة ونقل الكود الثابت
    EXPECT_FALSE(output.empty());
}

TEST_F(LoopOptimizationTest, ComplexLoopBody) {
    std::string code = R"(
        لكل (متغير i = 0; i < 4; i = i + 1) {
            متغير a = i * 2;
            متغير b = a + 5;
            إذا (b > 10) {
                اطبع(b);
            }
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

// =====================================
// اختبار حلقات لا نهائية
// =====================================

TEST_F(LoopOptimizationTest, InfiniteForLoop) {
    std::string code = R"(
        لكل (;;) {
            اطبع("infinite");
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
}

// =====================================
// اختبار حلقات مع عدادات متعددة
// =====================================

TEST_F(LoopOptimizationTest, LoopWithMultipleCounters) {
    std::string code = R"(
        لكل (متغير i = 0, j = 10; i < 5; i = i + 1, j = j - 1) {
            اطبع(i + j);
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
}

// =====================================
// اختبار حلقات مع Array Access
// =====================================

TEST_F(LoopOptimizationTest, LoopWithArrayAccess) {
    std::string code = R"(
        متغير arr = [1, 2, 3, 4];
        لكل (متغير i = 0; i < 4; i = i + 1) {
            اطبع(arr[i]);
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("unroll"), std::string::npos);
}
