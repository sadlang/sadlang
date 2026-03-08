/**
 * @file selftest.cpp
 * @brief (AR) اختبار ذاتي لإطار الاختبارات — يتحقق أن الإطار يعمل
 * @brief (EN) Self-test for the test framework — verifies the framework works
 * 
 * @author عمر — مهندس البنية التحتية
 * @date 2026-03-06
 */

#include "sad_test.h"
#include <string>
#include <cmath>

using namespace SadTest;

// ══════════════════════════════════════════════════════════════════════════════
// اختبارات xUnit Style
// ══════════════════════════════════════════════════════════════════════════════

TEST(Framework, ASSERT_TRUE_يعمل) {
    ASSERT_TRUE(1 + 1 == 2);
    ASSERT_TRUE(true);
}

TEST(Framework, ASSERT_FALSE_يعمل) {
    ASSERT_FALSE(1 + 1 == 3);
    ASSERT_FALSE(false);
}

TEST(Framework, ASSERT_EQ_يعمل) {
    ASSERT_EQ(5, 5);
    ASSERT_EQ(std::string("مرحبا"), std::string("مرحبا"));
}

TEST(Framework, ASSERT_NE_يعمل) {
    ASSERT_NE(5, 6);
    ASSERT_NE(std::string("أ"), std::string("ب"));
}

TEST(Framework, ASSERT_LT_يعمل) {
    ASSERT_LT(1, 2);
    ASSERT_LT(-5, 0);
}

TEST(Framework, ASSERT_LE_يعمل) {
    ASSERT_LE(1, 2);
    ASSERT_LE(2, 2);
}

TEST(Framework, ASSERT_GT_يعمل) {
    ASSERT_GT(2, 1);
    ASSERT_GT(0, -5);
}

TEST(Framework, ASSERT_GE_يعمل) {
    ASSERT_GE(2, 1);
    ASSERT_GE(2, 2);
}

TEST(Framework, ASSERT_NULL_يعمل) {
    int* ptr = nullptr;
    ASSERT_NULL(ptr);
}

TEST(Framework, ASSERT_NOT_NULL_يعمل) {
    int x = 5;
    int* ptr = &x;
    ASSERT_NOT_NULL(ptr);
}

TEST(Framework, ASSERT_NEAR_يعمل) {
    ASSERT_NEAR(3.14159, 3.14, 0.01);
    ASSERT_NEAR(1.0, 1.0001, 0.001);
}

TEST(Framework, ASSERT_THROWS_يعمل) {
    ASSERT_THROWS(throw std::runtime_error("test"), std::runtime_error);
}

TEST(Framework, ASSERT_NO_THROW_يعمل) {
    ASSERT_NO_THROW(int x = 5; (void)x);
}

TEST(Framework, أسماء_عربية_تعمل) {
    // اختبار أن الأسماء العربية تُسجل وتُعرض بشكل صحيح
    int مجموع = 0;
    for (int ع = 1; ع <= 5; ++ع) {
        مجموع += ع;
    }
    ASSERT_EQ(مجموع, 15);
}

// ══════════════════════════════════════════════════════════════════════════════
// اختبارات BDD Style
// ══════════════════════════════════════════════════════════════════════════════

// نستخدم lambda لتسجيل الاختبارات (BDD يُنفذ في وقت التشغيل)
namespace {
    struct BDDRegistrar {
        BDDRegistrar() {
            describe("Expectations الأساسية", []() {
                it("to_equal يقارن بشكل صحيح", []() {
                    expect(5).to_equal(5);
                    expect(std::string("سلام")).to_equal(std::string("سلام"));
                });

                it("to_not().to_equal يعمل", []() {
                    expect(5).to_not().to_equal(6);
                });

                it("to_be_true يعمل", []() {
                    expect(true).to_be_true();
                    expect(1 > 0).to_be_true();
                });

                it("to_be_false يعمل", []() {
                    expect(false).to_be_false();
                    expect(1 < 0).to_be_false();
                });

                it("to_be_greater_than يعمل", []() {
                    expect(10).to_be_greater_than(5);
                });

                it("to_be_less_than يعمل", []() {
                    expect(3).to_be_less_than(10);
                });
            });

            describe("Expectations للنصوص", []() {
                it("to_contain يجد النص الفرعي", []() {
                    expect(std::string("مرحبا بالعالم")).to_contain("العالم");
                });

                it("to_not_be_empty يتحقق من النص", []() {
                    expect(std::string("نص")).to_not_be_empty();
                });
            });

            describe("اختبارات UTF-8", []() {
                it("يتعامل مع الحروف العربية", []() {
                    std::string نص = u8"لغة ص";
                    expect(نص).to_contain(u8"ص");
                });

                it("يتعامل مع الرموز الخاصة", []() {
                    std::string رسالة = u8"نجح ✓";
                    expect(رسالة).to_not_be_empty();
                });
            });
        }
    } bdd_registrar;
}

// ══════════════════════════════════════════════════════════════════════════════
// اختبار مُتخطى (للتوضيح)
// ══════════════════════════════════════════════════════════════════════════════
SKIP_TEST(Framework, اختبار_مُتخطى_للتوضيح)

// ══════════════════════════════════════════════════════════════════════════════
// نقطة الدخول
// ══════════════════════════════════════════════════════════════════════════════
int main(int argc, char** argv) {
    std::cout << u8"\n════════════════════════════════════════════════════════════\n";
    std::cout << u8"  اختبار ذاتي لإطار اختبارات لغة ص\n";
    std::cout << u8"  Sad Test Framework Self-Test\n";
    std::cout << u8"════════════════════════════════════════════════════════════\n";
    
    int result = SAD_RUN_TESTS();
    
    // تصدير النتائج للتكامل مع CI
    TestRunner::instance().exportJSON("framework_selftest.json");
    
    return result;
}
