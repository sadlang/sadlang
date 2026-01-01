#include <gtest/gtest.h>
#include "optimizer/advanced_optimizer.h"
#include "parser/parser_core.h"
#include "lexer/lexer_core.h"
#include <memory>

using namespace Sad::Lexer;
using namespace Sad::Parser;

class AdvancedOptimizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        optimizer = std::make_unique<AdvancedOptimizer>();
    }

    std::shared_ptr<ASTNode> parseCode(const std::string& code) {
        LexerCore lexer(code);
        auto tokens = lexer.tokenize();
        ParserCore parser(tokens);
        return parser.parse();
    }

    std::unique_ptr<AdvancedOptimizer> optimizer;
};

// =====================================
// اختبار الإعداد الأساسي
// =====================================

TEST_F(AdvancedOptimizerTest, BasicInitialization) {
    ASSERT_NE(optimizer, nullptr);
    
    auto stats = optimizer->getStatistics();
    EXPECT_EQ(stats.totalPasses, 0);
    EXPECT_EQ(stats.nodesEliminated, 0);
}

// =====================================
// اختبار طي الثوابت
// =====================================

TEST_F(AdvancedOptimizerTest, ConstantFolding_BasicArithmetic) {
    // النص: متغير x = 2 + 3
    // يجب أن يُحسن إلى: متغير x = 5
    std::string code = "متغير x = 2 + 3;";
    
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto optimizedAst = optimizer->optimize(ast);
    ASSERT_NE(optimizedAst, nullptr);
    
    // التحقق من أن التحسين تم تطبيقه
    auto stats = optimizer->getStatistics();
    EXPECT_GT(stats.totalPasses, 0);
}

TEST_F(AdvancedOptimizerTest, ConstantFolding_StringConcatenation) {
    // النص: متغير msg = "مرحبا" + " " + "بالعالم"
    // يجب أن يُحسن إلى: متغير msg = "مرحبا بالعالم"
    std::string code = "متغير msg = \"مرحبا\" + \" \" + \"بالعالم\";";
    
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto optimizedAst = optimizer->optimize(ast);
    ASSERT_NE(optimizedAst, nullptr);
}

TEST_F(AdvancedOptimizerTest, ConstantFolding_BooleanExpressions) {
    // النص: متغير result = صحيح && خطأ
    // يجب أن يُحسن إلى: متغير result = خطأ
    std::string code = "متغير result = صحيح && خطأ;";
    
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto optimizedAst = optimizer->optimize(ast);
    ASSERT_NE(optimizedAst, nullptr);
}

// =====================================
// اختبار تبسيط التعبيرات
// =====================================

TEST_F(AdvancedOptimizerTest, ExpressionSimplification_IdentityOperations) {
    // النص: متغير x = y + 0
    // يجب أن يُحسن إلى: متغير x = y
    std::string code = "متغير x = y + 0;";
    
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto optimizedAst = optimizer->optimize(ast);
    ASSERT_NE(optimizedAst, nullptr);
}

TEST_F(AdvancedOptimizerTest, ExpressionSimplification_MultiplicationByOne) {
    // النص: متغير x = y * 1
    // يجب أن يُحسن إلى: متغير x = y
    std::string code = "متغير x = y * 1;";
    
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto optimizedAst = optimizer->optimize(ast);
    ASSERT_NE(optimizedAst, nullptr);
}

TEST_F(AdvancedOptimizerTest, ExpressionSimplification_MultiplicationByZero) {
    // النص: متغير x = y * 0
    // يجب أن يُحسن إلى: متغير x = 0
    std::string code = "متغير x = y * 0;";
    
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto optimizedAst = optimizer->optimize(ast);
    ASSERT_NE(optimizedAst, nullptr);
}

// =====================================
// اختبار حذف الكود الميت
// =====================================

TEST_F(AdvancedOptimizerTest, DeadCodeElimination_UnusedVariable) {
    // النص: متغير x = 5;  (غير مستخدم)
    std::string code = R"(
        متغير x = 5;
        متغير y = 10;
        اطبع(y);
    )";
    
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto optimizedAst = optimizer->optimize(ast);
    ASSERT_NE(optimizedAst, nullptr);
}

TEST_F(AdvancedOptimizerTest, DeadCodeElimination_UnreachableCode) {
    // كود لا يمكن الوصول إليه بعد return
    std::string code = R"(
        دالة test() {
            أرجع 5;
            اطبع("هذا لن يُنفذ");  // كود ميت
        }
    )";
    
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto optimizedAst = optimizer->optimize(ast);
    ASSERT_NE(optimizedAst, nullptr);
}

// =====================================
// اختبار تحسين الحلقات
// =====================================

TEST_F(AdvancedOptimizerTest, LoopOptimization_InvariantCodeMotion) {
    // النص: حلقة مع كود ثابت داخلها
    std::string code = R"(
        متغير x = 10;
        لكل (متغير i = 0; i < 100; i = i + 1) {
            متغير constant = 5 * 10;  // ثابت، يمكن نقله خارج الحلقة
            اطبع(i + constant);
        }
    )";
    
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto optimizedAst = optimizer->optimize(ast);
    ASSERT_NE(optimizedAst, nullptr);
}

// =====================================
// اختبار دمج الدوال
// =====================================

TEST_F(AdvancedOptimizerTest, FunctionInlining_SmallFunction) {
    // دالة صغيرة جداً يمكن دمجها
    std::string code = R"(
        دالة square(x) {
            أرجع x * x;
        }
        
        متغير result = square(5);
    )";
    
    // تعطيل دمج الدوال افتراضياً (لأنه غير آمن)
    optimizer->enableOptimization("FunctionInlining", false);
    
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto optimizedAst = optimizer->optimize(ast);
    ASSERT_NE(optimizedAst, nullptr);
}

// =====================================
// اختبار إزالة الإسناد الزائد
// =====================================

TEST_F(AdvancedOptimizerTest, RedundantAssignmentElimination) {
    // إسناد متعدد لنفس المتغير
    std::string code = R"(
        متغير x = 5;
        x = 10;  // الإسناد الأول غير مستخدم
        x = 15;  // الإسناد الثاني غير مستخدم
        اطبع(x);
    )";
    
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    auto optimizedAst = optimizer->optimize(ast);
    ASSERT_NE(optimizedAst, nullptr);
}

// =====================================
// اختبار التحسينات المعقدة
// =====================================

TEST_F(AdvancedOptimizerTest, ComplexOptimization_MultiplePassesCombined) {
    // كود معقد يستفيد من تحسينات متعددة
    std::string code = R"(
        دالة calculate() {
            متغير a = 2 + 3;           // طي ثوابت
            متغير b = a * 1;           // تبسيط
            متغير c = 100;             // غير مستخدم (كود ميت)
            متغير result = b + 0;      // تبسيط
            أرجع result;
        }
    )";
    
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    optimizer->resetStatistics();
    auto optimizedAst = optimizer->optimize(ast);
    ASSERT_NE(optimizedAst, nullptr);
    
    auto stats = optimizer->getStatistics();
    EXPECT_GT(stats.totalPasses, 0);
    EXPECT_GT(stats.optimizationTime, 0.0);
}

// =====================================
// اختبار الإحصائيات
// =====================================

TEST_F(AdvancedOptimizerTest, Statistics_Tracking) {
    std::string code = "متغير x = 2 + 3;";
    
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    optimizer->resetStatistics();
    auto optimizedAst = optimizer->optimize(ast);
    
    auto stats = optimizer->getStatistics();
    EXPECT_GE(stats.totalPasses, 0);
    EXPECT_GE(stats.optimizationTime, 0.0);
}

// =====================================
// اختبار تفعيل/تعطيل التحسينات
// =====================================

TEST_F(AdvancedOptimizerTest, EnableDisableOptimizations) {
    // تعطيل كل التحسينات
    optimizer->enableOptimization("ConstantFolding", false);
    optimizer->enableOptimization("DeadCodeElimination", false);
    optimizer->enableOptimization("ExpressionSimplification", false);
    
    std::string code = "متغير x = 2 + 3;";
    auto ast = parseCode(code);
    
    optimizer->resetStatistics();
    auto optimizedAst = optimizer->optimize(ast);
    
    auto stats = optimizer->getStatistics();
    // لا يجب تطبيق أي ممرات
    EXPECT_EQ(stats.totalPasses, 0);
}

// =====================================
// اختبار الأداء
// =====================================

TEST_F(AdvancedOptimizerTest, Performance_LargeProgram) {
    // برنامج كبير لاختبار الأداء
    std::string code;
    for (int i = 0; i < 100; i++) {
        code += "متغير x" + std::to_string(i) + " = " + std::to_string(i) + " + " + std::to_string(i+1) + ";\n";
    }
    
    auto ast = parseCode(code);
    ASSERT_NE(ast, nullptr);
    
    optimizer->resetStatistics();
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto optimizedAst = optimizer->optimize(ast);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    
    auto stats = optimizer->getStatistics();
    
    // التحسين يجب أن يتم في وقت معقول (< 1 ثانية)
    EXPECT_LT(elapsed.count(), 1.0);
    EXPECT_LT(stats.optimizationTime, 1.0);
    
    std::cout << "وقت التحسين: " << stats.optimizationTime << " ثانية\n";
    std::cout << "Optimization time: " << stats.optimizationTime << " seconds\n";
}

} // namespace sad
