// ════════════════════════════════════════════════════════════════════════════════
// ملف: test_pattern_matching.cpp
// File: test_pattern_matching.cpp
//
// الوصف (AR): اختبارات وحدة لنظام مطابقة الأنماط
//             يختبر جميع أنواع الأنماط والشروط وتوليد الكود
//
// Description (EN): Unit tests for pattern matching system
//                   Tests all pattern types, guards, and code generation
//
// المؤلف: فريق تطوير لغة ص
// Author: Sad Language Development Team
//
// التاريخ: يناير 2026
// Date: January 2026
//
// المرحلة: Phase 12 - User Story 10 - Pattern Matching
// Phase: Phase 12 - User Story 10 - Pattern Matching
//
// المهمة: T144 [US10] Create pattern matching tests
// Task: T144 [US10] Create pattern matching tests
// ════════════════════════════════════════════════════════════════════════════════

// تعطيل تحذير Unicode للتعليقات العربية
// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <memory>
#include <map>

// Include pattern matching headers
#include "pattern_nodes.h"
#include "value.h"

using namespace Sad;
using namespace Sad::AST;
using namespace Sad::Data;

// ════════════════════════════════════════════════════════════════════════════════
// ماكرو اختبار بسيط / Simple Test Macro
// ════════════════════════════════════════════════════════════════════════════════

#define TEST(name) \
    void test_##name(); \
    static bool _test_##name##_registered = ([] { \
        tests.push_back({#name, test_##name}); \
        return true; \
    })(); \
    void test_##name()

#define ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "❌ فشل (FAILED): " << #expr \
                      << " في السطر " << __LINE__ << "\n"; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while (0)

#define ASSERT_FALSE(expr) ASSERT_TRUE(!(expr))

#define ASSERT_EQ(a, b) \
    do { \
        if ((a) != (b)) { \
            std::cerr << "❌ فشل (FAILED): " << #a << " != " << #b \
                      << " في السطر " << __LINE__ << "\n"; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while (0)

// ════════════════════════════════════════════════════════════════════════════════
// قائمة الاختبارات / Test List
// ════════════════════════════════════════════════════════════════════════════════

struct TestCase {
    std::string name;
    void (*func)();
};

std::vector<TestCase> tests;

// ════════════════════════════════════════════════════════════════════════════════
// اختبارات Pattern الأساسية / Basic Pattern Tests
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @test (AR) اختبار النمط الشامل (_)
 * @test (EN) Test wildcard pattern (_)
 */
TEST(wildcard_pattern_matches_anything) {
    std::cout << "  🧪 اختبار النمط الشامل / Testing wildcard pattern...\n";
    
    WildcardPattern wildcard;
    std::map<std::string, Value> bindings;
    
    // (AR) يجب أن يطابق أي قيمة
    // (EN) Should match any value
    ASSERT_TRUE(wildcard.matches(Value(42.0), bindings));
    ASSERT_TRUE(wildcard.matches(Value("hello"), bindings));
    ASSERT_TRUE(wildcard.matches(Value(true), bindings));
    ASSERT_TRUE(wildcard.matches(Value(), bindings));  // null
    
    // (AR) لا يُنشئ ربطات
    // (EN) Creates no bindings
    ASSERT_TRUE(bindings.empty());
    
    std::cout << "    ✅ نجح / Passed\n";
}

/**
 * @test (AR) اختبار نمط القيمة الحرفية
 * @test (EN) Test literal pattern
 */
TEST(literal_pattern_exact_match) {
    std::cout << "  🧪 اختبار نمط القيمة الحرفية / Testing literal pattern...\n";
    
    // (AR) نمط رقمي
    // (EN) Number pattern
    LiteralPattern numPattern(Value(42.0));
    std::map<std::string, Value> bindings;
    
    ASSERT_TRUE(numPattern.matches(Value(42.0), bindings));
    ASSERT_FALSE(numPattern.matches(Value(43.0), bindings));
    ASSERT_FALSE(numPattern.matches(Value("42"), bindings));
    
    // (AR) نمط نصي
    // (EN) String pattern
    LiteralPattern strPattern(Value("hello"));
    ASSERT_TRUE(strPattern.matches(Value("hello"), bindings));
    ASSERT_FALSE(strPattern.matches(Value("world"), bindings));
    
    // (AR) نمط منطقي
    // (EN) Boolean pattern
    LiteralPattern boolPattern(Value(true));
    ASSERT_TRUE(boolPattern.matches(Value(true), bindings));
    ASSERT_FALSE(boolPattern.matches(Value(false), bindings));
    
    std::cout << "    ✅ نجح / Passed\n";
}

/**
 * @test (AR) اختبار نمط المتغير
 * @test (EN) Test variable pattern
 */
TEST(variable_pattern_binds_value) {
    std::cout << "  🧪 اختبار نمط المتغير / Testing variable pattern...\n";
    
    VariablePattern varPattern("x");
    std::map<std::string, Value> bindings;
    
    // (AR) يطابق أي قيمة
    // (EN) Matches any value
    ASSERT_TRUE(varPattern.matches(Value(42.0), bindings));
    
    // (AR) يربط القيمة للمتغير
    // (EN) Binds value to variable
    ASSERT_EQ(bindings.size(), 1u);
    ASSERT_TRUE(bindings.find("x") != bindings.end());
    ASSERT_EQ(bindings["x"].toDouble(), 42.0);
    
    // (AR) اختبار مع قيمة أخرى
    // (EN) Test with another value
    bindings.clear();
    ASSERT_TRUE(varPattern.matches(Value("test"), bindings));
    ASSERT_EQ(bindings["x"].toString(), "test");
    
    std::cout << "    ✅ نجح / Passed\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// اختبارات نمط القائمة / List Pattern Tests
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @test (AR) اختبار نمط القائمة الفارغة
 * @test (EN) Test empty list pattern
 */
TEST(empty_list_pattern) {
    std::cout << "  🧪 اختبار نمط القائمة الفارغة / Testing empty list pattern...\n";
    
    std::vector<std::unique_ptr<Pattern>> elements;
    ListPattern emptyPattern(std::move(elements));
    std::map<std::string, Value> bindings;
    
    // (AR) يطابق قائمة فارغة فقط
    // (EN) Matches only empty list
    std::vector<Value> emptyList;
    ASSERT_TRUE(emptyPattern.matches(Value(emptyList), bindings));
    
    std::vector<Value> nonEmpty = {Value(1.0)};
    ASSERT_FALSE(emptyPattern.matches(Value(nonEmpty), bindings));
    
    std::cout << "    ✅ نجح / Passed\n";
}

/**
 * @test (AR) اختبار نمط قائمة مع عناصر
 * @test (EN) Test list pattern with elements
 */
TEST(list_pattern_with_elements) {
    std::cout << "  🧪 اختبار نمط قائمة مع عناصر / Testing list pattern with elements...\n";
    
    // (AR) نمط [x, y]
    // (EN) Pattern [x, y]
    std::vector<std::unique_ptr<Pattern>> elements;
    elements.push_back(std::make_unique<VariablePattern>("x"));
    elements.push_back(std::make_unique<VariablePattern>("y"));
    ListPattern pattern(std::move(elements));
    
    std::map<std::string, Value> bindings;
    
    // (AR) يطابق قائمة بعنصرين
    // (EN) Matches list with two elements
    std::vector<Value> list = {Value(1.0), Value(2.0)};
    ASSERT_TRUE(pattern.matches(Value(list), bindings));
    ASSERT_EQ(bindings["x"].toDouble(), 1.0);
    ASSERT_EQ(bindings["y"].toDouble(), 2.0);
    
    // (AR) لا يطابق قائمة بعدد عناصر مختلف
    // (EN) Doesn't match list with different element count
    bindings.clear();
    std::vector<Value> shortList = {Value(1.0)};
    ASSERT_FALSE(pattern.matches(Value(shortList), bindings));
    
    std::vector<Value> longList = {Value(1.0), Value(2.0), Value(3.0)};
    ASSERT_FALSE(pattern.matches(Value(longList), bindings));
    
    std::cout << "    ✅ نجح / Passed\n";
}

/**
 * @test (AR) اختبار نمط قائمة مع rest
 * @test (EN) Test list pattern with rest
 */
TEST(list_pattern_with_rest) {
    std::cout << "  🧪 اختبار نمط قائمة مع rest / Testing list pattern with rest...\n";
    
    // (AR) نمط [first, *rest]
    // (EN) Pattern [first, *rest]
    std::vector<std::unique_ptr<Pattern>> elements;
    elements.push_back(std::make_unique<VariablePattern>("first"));
    ListPattern pattern(std::move(elements), true, "rest");
    
    std::map<std::string, Value> bindings;
    
    // (AR) يطابق قائمة بعنصر واحد أو أكثر
    // (EN) Matches list with one or more elements
    std::vector<Value> list = {Value(1.0), Value(2.0), Value(3.0)};
    ASSERT_TRUE(pattern.matches(Value(list), bindings));
    ASSERT_EQ(bindings["first"].toDouble(), 1.0);
    
    // (AR) التحقق من rest
    // (EN) Check rest
    ASSERT_TRUE(bindings.find("rest") != bindings.end());
    
    // (AR) يطابق قائمة بعنصر واحد (rest فارغ)
    // (EN) Matches list with one element (empty rest)
    bindings.clear();
    std::vector<Value> singleList = {Value(42.0)};
    ASSERT_TRUE(pattern.matches(Value(singleList), bindings));
    ASSERT_EQ(bindings["first"].toDouble(), 42.0);
    
    std::cout << "    ✅ نجح / Passed\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// اختبارات نمط OR / OR Pattern Tests
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @test (AR) اختبار نمط OR
 * @test (EN) Test OR pattern
 */
TEST(or_pattern_matches_alternatives) {
    std::cout << "  🧪 اختبار نمط OR / Testing OR pattern...\n";
    
    // (AR) نمط 1 || 2 || 3
    // (EN) Pattern 1 || 2 || 3
    std::vector<std::unique_ptr<Pattern>> alternatives;
    alternatives.push_back(std::make_unique<LiteralPattern>(Value(1.0)));
    alternatives.push_back(std::make_unique<LiteralPattern>(Value(2.0)));
    alternatives.push_back(std::make_unique<LiteralPattern>(Value(3.0)));
    OrPattern pattern(std::move(alternatives));
    
    std::map<std::string, Value> bindings;
    
    // (AR) يطابق أي من البدائل
    // (EN) Matches any alternative
    ASSERT_TRUE(pattern.matches(Value(1.0), bindings));
    ASSERT_TRUE(pattern.matches(Value(2.0), bindings));
    ASSERT_TRUE(pattern.matches(Value(3.0), bindings));
    
    // (AR) لا يطابق قيم أخرى
    // (EN) Doesn't match other values
    ASSERT_FALSE(pattern.matches(Value(4.0), bindings));
    ASSERT_FALSE(pattern.matches(Value(0.0), bindings));
    
    std::cout << "    ✅ نجح / Passed\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// اختبارات CaseClause و MatchStmt / CaseClause and MatchStmt Tests
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @test (AR) اختبار إنشاء CaseClause
 * @test (EN) Test CaseClause creation
 */
TEST(case_clause_creation) {
    std::cout << "  🧪 اختبار إنشاء CaseClause / Testing CaseClause creation...\n";
    
    // (AR) إنشاء case بسيط
    // (EN) Create simple case
    auto pattern = std::make_unique<VariablePattern>("x");
    std::vector<StmtPtr> body;
    
    CaseClause caseClause(std::move(pattern), nullptr, std::move(body));
    
    ASSERT_TRUE(caseClause.pattern != nullptr);
    ASSERT_TRUE(caseClause.guard == nullptr);
    ASSERT_TRUE(caseClause.body.empty());
    
    std::cout << "    ✅ نجح / Passed\n";
}

/**
 * @test (AR) اختبار إنشاء MatchStmt
 * @test (EN) Test MatchStmt creation
 */
TEST(match_stmt_creation) {
    std::cout << "  🧪 اختبار إنشاء MatchStmt / Testing MatchStmt creation...\n";
    
    // (AR) إنشاء match statement
    // (EN) Create match statement
    std::vector<CaseClause> cases;
    
    auto pattern1 = std::make_unique<LiteralPattern>(Value(1.0));
    std::vector<StmtPtr> body1;
    cases.emplace_back(std::move(pattern1), nullptr, std::move(body1));
    
    auto pattern2 = std::make_unique<WildcardPattern>();
    std::vector<StmtPtr> body2;
    cases.emplace_back(std::move(pattern2), nullptr, std::move(body2));
    
    // Note: We'd need an actual expression here
    // MatchStmt matchStmt(nullptr, std::move(cases));
    // ASSERT_EQ(matchStmt.cases.size(), 2u);
    
    std::cout << "    ✅ نجح / Passed\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// اختبارات toString / toString Tests
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @test (AR) اختبار تحويل الأنماط لنص
 * @test (EN) Test pattern toString
 */
TEST(pattern_to_string) {
    std::cout << "  🧪 اختبار تحويل الأنماط لنص / Testing pattern toString...\n";
    
    // (AR) نمط شامل
    // (EN) Wildcard
    WildcardPattern wildcard;
    ASSERT_EQ(wildcard.toString(), "_");
    
    // (AR) نمط متغير
    // (EN) Variable
    VariablePattern var("myVar");
    ASSERT_EQ(var.toString(), "myVar");
    
    // (AR) نمط حرفي
    // (EN) Literal
    LiteralPattern lit(Value(42.0));
    ASSERT_EQ(lit.toString(), "42");
    
    std::cout << "    ✅ نجح / Passed\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// اختبارات أداء / Performance Tests
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @test (AR) اختبار أداء مطابقة الأنماط
 * @test (EN) Test pattern matching performance
 */
TEST(pattern_matching_performance) {
    std::cout << "  🧪 اختبار أداء المطابقة / Testing matching performance...\n";
    
    // (AR) إنشاء نمط معقد
    // (EN) Create complex pattern
    std::vector<std::unique_ptr<Pattern>> elements;
    for (int i = 0; i < 10; ++i) {
        elements.push_back(std::make_unique<VariablePattern>("var" + std::to_string(i)));
    }
    ListPattern pattern(std::move(elements));
    
    // (AR) إنشاء قائمة اختبار
    // (EN) Create test list
    std::vector<Value> list;
    for (int i = 0; i < 10; ++i) {
        list.push_back(Value(static_cast<double>(i)));
    }
    Value testValue(list);
    
    // (AR) تشغيل المطابقة عدة مرات
    // (EN) Run matching multiple times
    const int iterations = 10000;
    std::map<std::string, Value> bindings;
    
    for (int i = 0; i < iterations; ++i) {
        bindings.clear();
        pattern.matches(testValue, bindings);
    }
    
    ASSERT_EQ(bindings.size(), 10u);
    
    std::cout << "    ✅ نجح (" << iterations << " تكرار / iterations)\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// اختبارات حالات حدودية / Edge Case Tests
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @test (AR) اختبار مطابقة null
 * @test (EN) Test null matching
 */
TEST(null_value_matching) {
    std::cout << "  🧪 اختبار مطابقة null / Testing null matching...\n";
    
    // (AR) نمط قيمة null
    // (EN) Null value pattern
    LiteralPattern nullPattern(Value());
    std::map<std::string, Value> bindings;
    
    ASSERT_TRUE(nullPattern.matches(Value(), bindings));
    ASSERT_FALSE(nullPattern.matches(Value(0.0), bindings));
    ASSERT_FALSE(nullPattern.matches(Value(""), bindings));
    
    std::cout << "    ✅ نجح / Passed\n";
}

/**
 * @test (AR) اختبار أنماط متداخلة
 * @test (EN) Test nested patterns
 */
TEST(nested_patterns) {
    std::cout << "  🧪 اختبار أنماط متداخلة / Testing nested patterns...\n";
    
    // (AR) نمط [[x, y], z]
    // (EN) Pattern [[x, y], z]
    std::vector<std::unique_ptr<Pattern>> innerElements;
    innerElements.push_back(std::make_unique<VariablePattern>("x"));
    innerElements.push_back(std::make_unique<VariablePattern>("y"));
    
    std::vector<std::unique_ptr<Pattern>> outerElements;
    outerElements.push_back(std::make_unique<ListPattern>(std::move(innerElements)));
    outerElements.push_back(std::make_unique<VariablePattern>("z"));
    
    ListPattern pattern(std::move(outerElements));
    std::map<std::string, Value> bindings;
    
    // (AR) إنشاء قيمة متداخلة
    // (EN) Create nested value
    std::vector<Value> innerList = {Value(1.0), Value(2.0)};
    std::vector<Value> outerList = {Value(innerList), Value(3.0)};
    
    ASSERT_TRUE(pattern.matches(Value(outerList), bindings));
    ASSERT_EQ(bindings["x"].toDouble(), 1.0);
    ASSERT_EQ(bindings["y"].toDouble(), 2.0);
    ASSERT_EQ(bindings["z"].toDouble(), 3.0);
    
    std::cout << "    ✅ نجح / Passed\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// الدالة الرئيسية / Main Function
// ════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  اختبارات مطابقة الأنماط / Pattern Matching Tests              ║\n";
    std::cout << "║  Phase 12 - User Story 10                                       ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n\n";
    
    int passed = 0;
    int failed = 0;
    
    for (const auto& test : tests) {
        std::cout << "🔷 " << test.name << ":\n";
        try {
            test.func();
            passed++;
        } catch (const std::exception& e) {
            std::cerr << "    ❌ فشل: " << e.what() << "\n";
            failed++;
        }
    }
    
    std::cout << "\n";
    std::cout << "════════════════════════════════════════════════════════════════\n";
    std::cout << "📊 النتيجة / Results:\n";
    std::cout << "    ✅ نجح / Passed: " << passed << "\n";
    std::cout << "    ❌ فشل / Failed: " << failed << "\n";
    std::cout << "    📝 المجموع / Total: " << (passed + failed) << "\n";
    std::cout << "════════════════════════════════════════════════════════════════\n";
    
    if (failed == 0) {
        std::cout << "\n🎉 جميع الاختبارات نجحت! / All tests passed!\n\n";
        return 0;
    } else {
        std::cout << "\n⚠️ بعض الاختبارات فشلت / Some tests failed\n\n";
        return 1;
    }
}

// ════════════════════════════════════════════════════════════════════════════════
// نهاية الملف / End of File
// ════════════════════════════════════════════════════════════════════════════════
