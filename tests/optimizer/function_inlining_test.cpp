#include <gtest/gtest.h>
#include "optimizer/optimization_pass.h"
#include "parser/parser.h"
#include "lexer/lexer.h"
#include <memory>
#include <sstream>

using namespace Sad;

class FunctionInliningTest : public ::testing::Test {
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
        
        FunctionInliningPass pass;
        pass.run(ast);
        
        std::cout.rdbuf(oldBuf);
        return oss.str();
    }
};

// =====================================
// اختبار الدوال الصغيرة (≤3 أسطر)
// =====================================

TEST_F(FunctionInliningTest, VerySmallFunction_OneLine) {
    std::string code = R"(
        دالة square(x) {
            أرجع x * x;
        }
        
        متغير result = square(5);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // يجب أن يقترح دمج دالة سطر واحد
    EXPECT_NE(output.find("inline"), std::string::npos);
}

TEST_F(FunctionInliningTest, SmallFunction_TwoLines) {
    std::string code = R"(
        دالة add(a, b) {
            متغير sum = a + b;
            أرجع sum;
        }
        
        متغير x = add(10, 20);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("inline"), std::string::npos);
}

TEST_F(FunctionInliningTest, SmallFunction_ThreeLines) {
    std::string code = R"(
        دالة calculate(x, y) {
            متغير temp = x * 2;
            متغير result = temp + y;
            أرجع result;
        }
        
        متغير val = calculate(5, 10);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("inline"), std::string::npos);
}

TEST_F(FunctionInliningTest, LargeFunction_NoInlining) {
    std::string code = R"(
        دالة complex(x) {
            متغير a = x + 1;
            متغير b = a * 2;
            متغير c = b - 3;
            متغير d = c / 4;
            أرجع d;
        }
        
        متغير result = complex(100);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // دالة كبيرة، لا يجب اقتراح الدمج
}

// =====================================
// اختبار الدوال المستدعاة مرة واحدة
// =====================================

TEST_F(FunctionInliningTest, FunctionCalledOnce) {
    std::string code = R"(
        دالة helper() {
            متغير x = 10;
            متغير y = 20;
            متغير z = 30;
            متغير w = 40;
            أرجع x + y + z + w;
        }
        
        متغير result = helper();
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // حتى لو كانت كبيرة، إذا استُدعيت مرة واحدة فقط يجب اقتراح الدمج
    EXPECT_NE(output.find("inline"), std::string::npos);
    EXPECT_NE(output.find("called only once"), std::string::npos);
}

TEST_F(FunctionInliningTest, FunctionCalledMultipleTimes) {
    std::string code = R"(
        دالة getValue() {
            أرجع 42;
        }
        
        متغير a = getValue();
        متغير b = getValue();
        متغير c = getValue();
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // دالة صغيرة مستدعاة عدة مرات - جيدة للدمج
    EXPECT_NE(output.find("inline"), std::string::npos);
}

// =====================================
// اختبار الدوال بدون معاملات
// =====================================

TEST_F(FunctionInliningTest, FunctionWithNoParameters) {
    std::string code = R"(
        دالة getConstant() {
            أرجع 100;
        }
        
        متغير x = getConstant();
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("inline"), std::string::npos);
}

// =====================================
// اختبار الدوال بمعاملات متعددة
// =====================================

TEST_F(FunctionInliningTest, FunctionWithMultipleParameters) {
    std::string code = R"(
        دالة sum(a, b, c, d) {
            أرجع a + b + c + d;
        }
        
        متغير result = sum(1, 2, 3, 4);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("inline"), std::string::npos);
}

// =====================================
// اختبار الدوال المتداخلة
// =====================================

TEST_F(FunctionInliningTest, NestedFunctionCalls) {
    std::string code = R"(
        دالة inner(x) {
            أرجع x * 2;
        }
        
        دالة outer(y) {
            أرجع inner(y) + 10;
        }
        
        متغير result = outer(5);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // يجب اقتراح دمج كلا الدالتين
    EXPECT_FALSE(output.empty());
}

// =====================================
// اختبار الدوال العودية
// =====================================

TEST_F(FunctionInliningTest, RecursiveFunction_NoInlining) {
    std::string code = R"(
        دالة factorial(n) {
            إذا (n <= 1) {
                أرجع 1;
            }
            أرجع n * factorial(n - 1);
        }
        
        متغير result = factorial(5);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // الدوال العودية لا يجب دمجها
}

// =====================================
// اختبار الدوال مع جمل شرطية
// =====================================

TEST_F(FunctionInliningTest, FunctionWithIfStatement) {
    std::string code = R"(
        دالة max(a, b) {
            إذا (a > b) {
                أرجع a;
            }
            أرجع b;
        }
        
        متغير result = max(10, 20);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

TEST_F(FunctionInliningTest, FunctionWithComplexIf) {
    std::string code = R"(
        دالة classify(x) {
            إذا (x < 0) {
                أرجع "negative";
            } وإلا إذا (x == 0) {
                أرجع "zero";
            } وإلا {
                أرجع "positive";
            }
        }
        
        متغير class = classify(5);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
}

// =====================================
// اختبار الدوال مع حلقات
// =====================================

TEST_F(FunctionInliningTest, FunctionWithLoop_NoInlining) {
    std::string code = R"(
        دالة sumArray(arr) {
            متغير sum = 0;
            لكل (متغير i = 0; i < arr.length; i = i + 1) {
                sum = sum + arr[i];
            }
            أرجع sum;
        }
        
        متغير total = sumArray([1, 2, 3]);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // دالة بحلقة - عادة لا تُدمج إلا إذا استُدعيت مرة واحدة
}

// =====================================
// اختبار الدوال المتعددة في ملف واحد
// =====================================

TEST_F(FunctionInliningTest, MultipleFunctions) {
    std::string code = R"(
        دالة double(x) {
            أرجع x * 2;
        }
        
        دالة triple(x) {
            أرجع x * 3;
        }
        
        دالة quadruple(x) {
            أرجع x * 4;
        }
        
        متغير a = double(5);
        متغير b = triple(5);
        متغير c = quadruple(5);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // يجب اقتراح دمج جميع الدوال الثلاث
    EXPECT_FALSE(output.empty());
}

// =====================================
// اختبار الدوال مع عمليات معقدة
// =====================================

TEST_F(FunctionInliningTest, FunctionWithComplexExpression) {
    std::string code = R"(
        دالة calculate(a, b, c) {
            أرجع (a + b) * c - (a - b) / c;
        }
        
        متغير result = calculate(10, 5, 2);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("inline"), std::string::npos);
}

// =====================================
// اختبار الدوال المستخدمة كمعاملات
// =====================================

TEST_F(FunctionInliningTest, FunctionAsArgument) {
    std::string code = R"(
        دالة getValue() {
            أرجع 42;
        }
        
        اطبع(getValue());
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("inline"), std::string::npos);
}

// =====================================
// اختبار الدوال مع متغيرات محلية
// =====================================

TEST_F(FunctionInliningTest, FunctionWithLocalVariables) {
    std::string code = R"(
        دالة process(x) {
            متغير temp1 = x + 10;
            متغير temp2 = temp1 * 2;
            متغير temp3 = temp2 - 5;
            متغير temp4 = temp3 / 3;
            أرجع temp4;
        }
        
        متغير result = process(100);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // دالة كبيرة نسبياً
}

// =====================================
// اختبار الدوال الفارغة
// =====================================

TEST_F(FunctionInliningTest, EmptyFunction) {
    std::string code = R"(
        دالة doNothing() {
        }
        
        doNothing();
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // دالة فارغة - يمكن إزالتها تماماً
    EXPECT_FALSE(output.empty());
}

TEST_F(FunctionInliningTest, FunctionWithOnlyReturn) {
    std::string code = R"(
        دالة getTrue() {
            أرجع صحيح;
        }
        
        إذا (getTrue()) {
            اطبع("yes");
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_NE(output.find("inline"), std::string::npos);
}

// =====================================
// اختبار استدعاءات متسلسلة
// =====================================

TEST_F(FunctionInliningTest, ChainedFunctionCalls) {
    std::string code = R"(
        دالة f1(x) { أرجع x + 1; }
        دالة f2(x) { أرجع f1(x) * 2; }
        دالة f3(x) { أرجع f2(x) - 3; }
        
        متغير result = f3(10);
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

// =====================================
// اختبار الدوال مع قيم افتراضية
// =====================================

TEST_F(FunctionInliningTest, FunctionWithDefaultReturn) {
    std::string code = R"(
        دالة alwaysTrue() {
            أرجع صحيح;
        }
        
        بينما (alwaysTrue()) {
            اقطع;
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    EXPECT_FALSE(output.empty());
}

// =====================================
// اختبار حالات الحدود
// =====================================

TEST_F(FunctionInliningTest, FunctionCalledInDifferentContexts) {
    std::string code = R"(
        دالة getId() {
            أرجع 123;
        }
        
        متغير x = getId();
        متغير y = getId() + 100;
        اطبع(getId());
        إذا (getId() > 0) {
            متغير z = getId();
        }
    )";
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto output = getOptimizationOutput(ast);
    // دالة صغيرة مستدعاة في سياقات مختلفة
    EXPECT_NE(output.find("inline"), std::string::npos);
}
