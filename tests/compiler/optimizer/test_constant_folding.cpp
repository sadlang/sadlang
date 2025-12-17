/**
 * @file test_constant_folding.cpp
 * @brief اختبارات تمرير طي الثوابت
 * @brief Constant Folding Pass Tests
 * 
 * @details
 * (AR) يختبر جميع قدرات تمرير طي الثوابت.
 * (EN) Tests all capabilities of constant folding pass.
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 */

#include <gtest/gtest.h>
#include "../../../compiler/optimizer/include/constant_folding_pass.h"
#include "../../../compiler/optimizer/include/optimizer.h"
#include "../../../compiler/frontend/include/sir_builder.h"
#include "sir_test_utils.h"

using namespace Sad::Compiler::Optimizer;
using namespace Sad::Compiler::SIR;
using namespace Sad::Compiler::Testing;

/**
 * @brief صنف أساسي للاختبارات / Base test fixture
 */
class ConstantFoldingTest : public ::testing::Test {
protected:
    void SetUp() override {
        pass = std::make_unique<ConstantFoldingPass>();
    }

    void TearDown() override {
        pass.reset();
    }

    std::unique_ptr<ConstantFoldingPass> pass;
};

// ============================================================================
// اختبارات العمليات الحسابية على الأعداد الصحيحة
// Integer Arithmetic Operations Tests
// ============================================================================

/**
 * @brief اختبار الجمع / Test Addition
 */
TEST_F(ConstantFoldingTest, IntAddition) {
    // 2 + 3 = 5
    auto result = pass->foldIntBinary(SIROpcode::ADD, 2, 3);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 5);
}

/**
 * @brief اختبار الطرح / Test Subtraction
 */
TEST_F(ConstantFoldingTest, IntSubtraction) {
    // 10 - 3 = 7
    auto result = pass->foldIntBinary(SIROpcode::SUB, 10, 3);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 7);
}

/**
 * @brief اختبار الضرب / Test Multiplication
 */
TEST_F(ConstantFoldingTest, IntMultiplication) {
    // 4 * 5 = 20
    auto result = pass->foldIntBinary(SIROpcode::MUL, 4, 5);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 20);
}

/**
 * @brief اختبار القسمة / Test Division
 */
TEST_F(ConstantFoldingTest, IntDivision) {
    // 20 / 4 = 5
    auto result = pass->foldIntBinary(SIROpcode::DIV, 20, 4);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 5);
}

/**
 * @brief اختبار باقي القسمة / Test Modulo
 */
TEST_F(ConstantFoldingTest, IntModulo) {
    // 17 % 5 = 2
    auto result = pass->foldIntBinary(SIROpcode::MOD, 17, 5);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 2);
}

// ============================================================================
// اختبارات القسمة على صفر
// Division by Zero Tests
// ============================================================================

/**
 * @brief اختبار القسمة على صفر / Test Division by Zero
 */
TEST_F(ConstantFoldingTest, DivisionByZero) {
    auto result = pass->foldIntBinary(SIROpcode::DIV, 10, 0);
    EXPECT_FALSE(result.has_value());  // يجب أن تفشل / Should fail
}

/**
 * @brief اختبار باقي القسمة على صفر / Test Modulo by Zero
 */
TEST_F(ConstantFoldingTest, ModuloByZero) {
    auto result = pass->foldIntBinary(SIROpcode::MOD, 10, 0);
    EXPECT_FALSE(result.has_value());  // يجب أن تفشل / Should fail
}

// ============================================================================
// اختبارات الفيض
// Overflow Tests
// ============================================================================

/**
 * @brief اختبار فيض الجمع / Test Addition Overflow
 */
TEST_F(ConstantFoldingTest, AdditionOverflow) {
    int64_t max = std::numeric_limits<int64_t>::max();
    auto result = pass->foldIntBinary(SIROpcode::ADD, max, 1);
    EXPECT_FALSE(result.has_value());  // يجب أن تفشل / Should fail
}

/**
 * @brief اختبار فيض الطرح / Test Subtraction Underflow
 */
TEST_F(ConstantFoldingTest, SubtractionUnderflow) {
    int64_t min = std::numeric_limits<int64_t>::min();
    auto result = pass->foldIntBinary(SIROpcode::SUB, min, 1);
    EXPECT_FALSE(result.has_value());  // يجب أن تفشل / Should fail
}

/**
 * @brief اختبار فيض الضرب / Test Multiplication Overflow
 */
TEST_F(ConstantFoldingTest, MultiplicationOverflow) {
    int64_t max = std::numeric_limits<int64_t>::max();
    auto result = pass->foldIntBinary(SIROpcode::MUL, max, 2);
    EXPECT_FALSE(result.has_value());  // يجب أن تفشل / Should fail
}

// ============================================================================
// اختبارات العمليات المنطقية
// Bitwise Operations Tests
// ============================================================================

/**
 * @brief اختبار AND المنطقي / Test Bitwise AND
 */
TEST_F(ConstantFoldingTest, BitwiseAnd) {
    // 0b1100 & 0b1010 = 0b1000 (12 & 10 = 8)
    auto result = pass->foldIntBinary(SIROpcode::AND, 12, 10);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 8);
}

/**
 * @brief اختبار OR المنطقي / Test Bitwise OR
 */
TEST_F(ConstantFoldingTest, BitwiseOr) {
    // 0b1100 | 0b1010 = 0b1110 (12 | 10 = 14)
    auto result = pass->foldIntBinary(SIROpcode::OR, 12, 10);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 14);
}

/**
 * @brief اختبار XOR المنطقي / Test Bitwise XOR
 */
TEST_F(ConstantFoldingTest, BitwiseXor) {
    // 0b1100 ^ 0b1010 = 0b0110 (12 ^ 10 = 6)
    auto result = pass->foldIntBinary(SIROpcode::XOR, 12, 10);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 6);
}

/**
 * @brief اختبار الإزاحة اليسارية / Test Left Shift
 */
TEST_F(ConstantFoldingTest, LeftShift) {
    // 5 << 2 = 20
    auto result = pass->foldIntBinary(SIROpcode::SHL, 5, 2);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 20);
}

/**
 * @brief اختبار الإزاحة اليمينية / Test Right Shift
 */
TEST_F(ConstantFoldingTest, RightShift) {
    // 20 >> 2 = 5
    auto result = pass->foldIntBinary(SIROpcode::SHR, 20, 2);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 5);
}

/**
 * @brief اختبار إزاحة غير صالحة / Test Invalid Shift
 */
TEST_F(ConstantFoldingTest, InvalidShift) {
    // إزاحة سالبة / Negative shift
    auto result1 = pass->foldIntBinary(SIROpcode::SHL, 5, -1);
    EXPECT_FALSE(result1.has_value());
    
    // إزاحة كبيرة جداً / Too large shift
    auto result2 = pass->foldIntBinary(SIROpcode::SHL, 5, 64);
    EXPECT_FALSE(result2.has_value());
}

// ============================================================================
// اختبارات العمليات على الأعداد العشرية
// Float Operations Tests
// ============================================================================

/**
 * @brief اختبار جمع الأعداد العشرية / Test Float Addition
 */
TEST_F(ConstantFoldingTest, FloatAddition) {
    // 2.5 + 3.7 = 6.2
    auto result = pass->foldFloatBinary(SIROpcode::FADD, 2.5, 3.7);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result.value(), 6.2);
}

/**
 * @brief اختبار طرح الأعداد العشرية / Test Float Subtraction
 */
TEST_F(ConstantFoldingTest, FloatSubtraction) {
    // 10.5 - 3.2 = 7.3
    auto result = pass->foldFloatBinary(SIROpcode::FSUB, 10.5, 3.2);
    ASSERT_TRUE(result.has_value());
    EXPECT_NEAR(result.value(), 7.3, 0.0001);
}

/**
 * @brief اختبار ضرب الأعداد العشرية / Test Float Multiplication
 */
TEST_F(ConstantFoldingTest, FloatMultiplication) {
    // 2.5 * 4.0 = 10.0
    auto result = pass->foldFloatBinary(SIROpcode::FMUL, 2.5, 4.0);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result.value(), 10.0);
}

/**
 * @brief اختبار قسمة الأعداد العشرية / Test Float Division
 */
TEST_F(ConstantFoldingTest, FloatDivision) {
    // 10.0 / 2.5 = 4.0
    auto result = pass->foldFloatBinary(SIROpcode::FDIV, 10.0, 2.5);
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(result.value(), 4.0);
}

/**
 * @brief اختبار قسمة عشرية على صفر / Test Float Division by Zero
 */
TEST_F(ConstantFoldingTest, FloatDivisionByZero) {
    auto result = pass->foldFloatBinary(SIROpcode::FDIV, 10.0, 0.0);
    EXPECT_FALSE(result.has_value());  // يجب أن تفشل / Should fail
}

// ============================================================================
// اختبارات عمليات المقارنة
// Comparison Operations Tests
// ============================================================================

/**
 * @brief اختبار المساواة / Test Equality
 */
TEST_F(ConstantFoldingTest, EqualityComparison) {
    auto result1 = pass->foldIntComparison(SIROpcode::ICMP_EQ, 5, 5);
    ASSERT_TRUE(result1.has_value());
    EXPECT_TRUE(result1.value());
    
    auto result2 = pass->foldIntComparison(SIROpcode::ICMP_EQ, 5, 3);
    ASSERT_TRUE(result2.has_value());
    EXPECT_FALSE(result2.value());
}

/**
 * @brief اختبار عدم المساواة / Test Inequality
 */
TEST_F(ConstantFoldingTest, InequalityComparison) {
    auto result1 = pass->foldIntComparison(SIROpcode::ICMP_NE, 5, 3);
    ASSERT_TRUE(result1.has_value());
    EXPECT_TRUE(result1.value());
    
    auto result2 = pass->foldIntComparison(SIROpcode::ICMP_NE, 5, 5);
    ASSERT_TRUE(result2.has_value());
    EXPECT_FALSE(result2.value());
}

/**
 * @brief اختبار أقل من / Test Less Than
 */
TEST_F(ConstantFoldingTest, LessThanComparison) {
    auto result1 = pass->foldIntComparison(SIROpcode::ICMP_LT, 3, 5);
    ASSERT_TRUE(result1.has_value());
    EXPECT_TRUE(result1.value());
    
    auto result2 = pass->foldIntComparison(SIROpcode::ICMP_LT, 5, 3);
    ASSERT_TRUE(result2.has_value());
    EXPECT_FALSE(result2.value());
}

/**
 * @brief اختبار أقل من أو يساوي / Test Less Than or Equal
 */
TEST_F(ConstantFoldingTest, LessThanOrEqualComparison) {
    auto result1 = pass->foldIntComparison(SIROpcode::ICMP_LE, 3, 5);
    ASSERT_TRUE(result1.has_value());
    EXPECT_TRUE(result1.value());
    
    auto result2 = pass->foldIntComparison(SIROpcode::ICMP_LE, 5, 5);
    ASSERT_TRUE(result2.has_value());
    EXPECT_TRUE(result2.value());
    
    auto result3 = pass->foldIntComparison(SIROpcode::ICMP_LE, 7, 5);
    ASSERT_TRUE(result3.has_value());
    EXPECT_FALSE(result3.value());
}

/**
 * @brief اختبار أكبر من / Test Greater Than
 */
TEST_F(ConstantFoldingTest, GreaterThanComparison) {
    auto result1 = pass->foldIntComparison(SIROpcode::ICMP_GT, 5, 3);
    ASSERT_TRUE(result1.has_value());
    EXPECT_TRUE(result1.value());
    
    auto result2 = pass->foldIntComparison(SIROpcode::ICMP_GT, 3, 5);
    ASSERT_TRUE(result2.has_value());
    EXPECT_FALSE(result2.value());
}

/**
 * @brief اختبار أكبر من أو يساوي / Test Greater Than or Equal
 */
TEST_F(ConstantFoldingTest, GreaterThanOrEqualComparison) {
    auto result1 = pass->foldIntComparison(SIROpcode::ICMP_GE, 5, 3);
    ASSERT_TRUE(result1.has_value());
    EXPECT_TRUE(result1.value());
    
    auto result2 = pass->foldIntComparison(SIROpcode::ICMP_GE, 5, 5);
    ASSERT_TRUE(result2.has_value());
    EXPECT_TRUE(result2.value());
    
    auto result3 = pass->foldIntComparison(SIROpcode::ICMP_GE, 3, 5);
    ASSERT_TRUE(result3.has_value());
    EXPECT_FALSE(result3.value());
}

// ============================================================================
// اختبارات العمليات الأحادية
// Unary Operations Tests
// ============================================================================

/**
 * @brief اختبار النفي الحسابي / Test Arithmetic Negation
 */
TEST_F(ConstantFoldingTest, ArithmeticNegation) {
    // -5 = -5
    auto result1 = pass->foldIntUnary(SIROpcode::NEG, 5);
    ASSERT_TRUE(result1.has_value());
    EXPECT_EQ(result1.value(), -5);
    
    // -(-5) = 5
    auto result2 = pass->foldIntUnary(SIROpcode::NEG, -5);
    ASSERT_TRUE(result2.has_value());
    EXPECT_EQ(result2.value(), 5);
}

/**
 * @brief اختبار النفي المنطقي / Test Bitwise NOT
 */
TEST_F(ConstantFoldingTest, BitwiseNot) {
    // ~5 = -6 (two's complement)
    auto result = pass->foldIntUnary(SIROpcode::NOT, 5);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), ~5);
}

/**
 * @brief اختبار فيض النفي / Test Negation Overflow
 */
TEST_F(ConstantFoldingTest, NegationOverflow) {
    int64_t min = std::numeric_limits<int64_t>::min();
    auto result = pass->foldIntUnary(SIROpcode::NEG, min);
    EXPECT_FALSE(result.has_value());  // يجب أن تفشل / Should fail
}

// ============================================================================
// اختبارات إدارة جدول الثوابت
// Constants Table Management Tests
// ============================================================================

/**
 * @brief اختبار تسجيل واسترجاع الثوابت / Test Constant Recording and Retrieval
 */
TEST_F(ConstantFoldingTest, ConstantTableManagement) {
    // تسجيل ثابت / Record constant
    pass->recordConstant("%1", static_cast<int64_t>(42));
    
    // التحقق من وجوده / Check existence
    EXPECT_TRUE(pass->isConstant("%1"));
    EXPECT_FALSE(pass->isConstant("%2"));
    
    // الحصول على القيمة / Get value
    auto value = pass->getConstantValue("%1");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(std::get<int64_t>(value.value()), 42);
}

/**
 * @brief اختبار حذف ثابت / Test Constant Removal
 */
TEST_F(ConstantFoldingTest, ConstantRemoval) {
    pass->recordConstant("%1", static_cast<int64_t>(42));
    EXPECT_TRUE(pass->isConstant("%1"));
    
    pass->removeConstant("%1");
    EXPECT_FALSE(pass->isConstant("%1"));
}

/**
 * @brief اختبار إعادة تعيين الجدول / Test Table Reset
 */
TEST_F(ConstantFoldingTest, ConstantTableReset) {
    pass->recordConstant("%1", static_cast<int64_t>(42));
    pass->recordConstant("%2", static_cast<double>(3.14));
    
    EXPECT_TRUE(pass->isConstant("%1"));
    EXPECT_TRUE(pass->isConstant("%2"));
    
    pass->resetConstants();
    
    EXPECT_FALSE(pass->isConstant("%1"));
    EXPECT_FALSE(pass->isConstant("%2"));
}

// ============================================================================
// اختبارات الإحصائيات
// Statistics Tests
// ============================================================================

/**
 * @brief اختبار تتبع التعديلات / Test Modification Tracking
 */
TEST_F(ConstantFoldingTest, ModificationTracking) {
    EXPECT_EQ(pass->getModificationCount(), 0);
    
    // محاكاة تعديلات / Simulate modifications
    pass->resetStats();
    EXPECT_EQ(pass->getModificationCount(), 0);
}

/**
 * @brief اختبار اسم التمرير / Test Pass Name
 */
TEST_F(ConstantFoldingTest, PassName) {
    EXPECT_EQ(pass->getName(), "Constant Folding");
}

// ============================================================================
// اختبارات معقدة
// Complex Tests
// ============================================================================

/**
 * @brief اختبار تعبيرات متداخلة / Test Nested Expressions
 */
TEST_F(ConstantFoldingTest, NestedExpressions) {
    // ((2 + 3) * 4) / 2 = 10
    auto result1 = pass->foldIntBinary(SIROpcode::ADD, 2, 3);  // 5
    ASSERT_TRUE(result1.has_value());
    
    auto result2 = pass->foldIntBinary(SIROpcode::MUL, result1.value(), 4);  // 20
    ASSERT_TRUE(result2.has_value());
    
    auto result3 = pass->foldIntBinary(SIROpcode::DIV, result2.value(), 2);  // 10
    ASSERT_TRUE(result3.has_value());
    EXPECT_EQ(result3.value(), 10);
}

/**
 * @brief اختبار تعبيرات منطقية معقدة / Test Complex Bitwise Expressions
 */
TEST_F(ConstantFoldingTest, ComplexBitwiseExpressions) {
    // (12 & 10) | (5 ^ 3) = 8 | 6 = 14
    auto result1 = pass->foldIntBinary(SIROpcode::AND, 12, 10);  // 8
    ASSERT_TRUE(result1.has_value());
    
    auto result2 = pass->foldIntBinary(SIROpcode::XOR, 5, 3);    // 6
    ASSERT_TRUE(result2.has_value());
    
    auto result3 = pass->foldIntBinary(SIROpcode::OR, result1.value(), result2.value());  // 14
    ASSERT_TRUE(result3.has_value());
    EXPECT_EQ(result3.value(), 14);
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
