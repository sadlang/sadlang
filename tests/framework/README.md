# بسم الله الرحمن الرحيم

# إطار اختبارات لغة ص الموحد / Sad Test Framework

إطار اختبارات C++ موحد للغة ص يدعم أسلوبين:
- **xUnit** — للاختبارات الوحدوية (unit tests)
- **BDD** — لاختبارات التكامل والسلوك (behavior tests)

## 🚀 البدء السريع

### 1. الاستخدام في ملف اختبار

```cpp
#include "tests/framework/sad_test.h"

// ═══════════════════════════════════════════════════════════════════
// أسلوب xUnit — للاختبارات الوحدوية
// ═══════════════════════════════════════════════════════════════════
TEST(Lexer, يحلل_الكلمات_المفتاحية_العربية) {
    auto tokens = lex(u8"دالة");
    ASSERT_EQ(tokens.size(), 2);  // KEYWORD + EOF
    ASSERT_EQ(tokens[0].type, TokenType::KEYWORD_FUNCTION);
}

TEST(Parser, يرفض_بناء_جملة_خاطئ) {
    ASSERT_THROWS(parse(u8"دالة ("), SyntaxError);
}

// ═══════════════════════════════════════════════════════════════════
// أسلوب BDD — لاختبارات السلوك والتكامل
// ═══════════════════════════════════════════════════════════════════
describe("المحلل النحوي", []() {
    it("يحلل دالة بسيطة", []() {
        auto ast = parse(u8"دالة جمع() نهاية");
        expect(ast).to_not_be_null();
        expect(ast->name).to_equal("جمع");
    });
    
    it("يدعم الوراثة", []() {
        auto ast = parse(u8"صنف ابن يرث أب نهاية");
        expect(ast->parent).to_equal("أب");
    });
    
    xit("سيُضاف لاحقاً: generics");  // اختبار مُتخطى
});

int main(int argc, char** argv) {
    return SAD_RUN_TESTS();
}
```

### 2. البناء مع CMake

```cmake
# في CMakeLists.txt الخاص باختباراتك
target_link_libraries(my_tests PRIVATE sad_test_framework)
```

---

## 📚 API Reference

### Assertions (xUnit)

| Macro | الوصف |
|-------|-------|
| `ASSERT_TRUE(cond)` | تأكد أن الشرط صحيح |
| `ASSERT_FALSE(cond)` | تأكد أن الشرط خاطئ |
| `ASSERT_EQ(a, b)` | تأكد من المساواة `a == b` |
| `ASSERT_NE(a, b)` | تأكد من عدم المساواة `a != b` |
| `ASSERT_LT(a, b)` | تأكد أن `a < b` |
| `ASSERT_LE(a, b)` | تأكد أن `a <= b` |
| `ASSERT_GT(a, b)` | تأكد أن `a > b` |
| `ASSERT_GE(a, b)` | تأكد أن `a >= b` |
| `ASSERT_NULL(ptr)` | تأكد أن المؤشر `nullptr` |
| `ASSERT_NOT_NULL(ptr)` | تأكد أن المؤشر ليس `nullptr` |
| `ASSERT_NEAR(a, b, tol)` | تأكد أن `|a - b| <= tol` |
| `ASSERT_THROWS(expr, Type)` | تأكد من رمي استثناء من نوع معين |
| `ASSERT_NO_THROW(expr)` | تأكد من عدم رمي أي استثناء |
| `FAIL(msg)` | فشل الاختبار برسالة |

### Expectations (BDD)

```cpp
expect(value).to_equal(expected);
expect(value).to_not().to_equal(other);
expect(value).to_be_true();
expect(value).to_be_false();
expect(ptr).to_be_null();
expect(ptr).to_not_be_null();
expect(num).to_be_greater_than(5);
expect(num).to_be_less_than(10);

// للنصوص:
expect(str).to_contain("جزء");
expect(str).to_be_empty();
expect(str).to_not_be_empty();
```

---

## 🎯 ميزات

- ✅ **أسماء اختبارات عربية** — دعم UTF-8 كامل
- ✅ **ألوان في الطرفية** — Windows + Linux + macOS
- ✅ **تصدير النتائج** — XML (JUnit format) + JSON
- ✅ **فلترة الاختبارات** — `--filter=اسم`
- ✅ **تسجيل تلقائي** — الاختبارات تُسجل تلقائياً
- ✅ **Header-only** — لا حاجة لبناء مكتبة

---

## 🔧 خيارات سطر الأوامر

```bash
./tests --filter=Lexer          # تشغيل اختبارات تحتوي "Lexer"
./tests --filter=يحلل           # تشغيل اختبارات تحتوي "يحلل"
./tests -v                      # وضع verbose
```

---

## 📊 تصدير النتائج

```cpp
int main(int argc, char** argv) {
    int result = SAD_RUN_TESTS();
    
    // تصدير للتكامل مع CI
    SadTest::TestRunner::instance().exportXML("results.xml");
    SadTest::TestRunner::instance().exportJSON("results.json");
    
    return result;
}
```

---

## 📁 هيكل المجلد

```
tests/framework/
├── sad_test.h           # إطار الاختبارات (header-only)
├── sad_benchmark.h      # إطار قياس الأداء (header-only)
├── CMakeLists.txt       # تكوين البناء
├── README.md            # هذا الملف
├── selftest.cpp         # اختبار ذاتي للإطار
└── benchmark_example.cpp # مثال على قياس الأداء
```

---

## 📊 إطار قياس الأداء (Benchmark Framework)

### الاستخدام

```cpp
#include "tests/framework/sad_benchmark.h"

BENCHMARK(Lexer, تحليل_ملف_كبير) {
    for (auto _ : state) {
        lex(largeFile);
    }
}

BENCHMARK(Parser, تحليل_1000_دالة)->Iterations(100)->WarmupRuns(5);

int main(int argc, char** argv) {
    int result = SAD_RUN_BENCHMARKS();
    
    // تصدير للتتبع في CI
    SadBench::BenchmarkRunner::instance().exportJSON("results.json");
    
    return result;
}
```

### خيارات البناء

```cpp
BENCHMARK(Suite, Name)
    ->Iterations(1000)      // عدد التكرارات
    ->WarmupRuns(3)         // تشغيلات تسخين
    ->MeasureMemory(true);  // قياس الذاكرة
```

### المخرجات

```
╔════════════════════════════════════════════════════════════╗
║  قياس أداء لغة ص / Sad Language Benchmarks                 ║
╚════════════════════════════════════════════════════════════╝

━━━ Lexer ━━━
  تحليل_نص_قصير ... 0.015 ms (±0.002 ms, 66.7K ops/s)
  تحليل_نص_متوسط ... 1.234 ms (±0.105 ms, 810.4 ops/s)

━━━ Parser ━━━
  تحليل_1000_دالة ... 12.567 ms (±1.234 ms, 79.6 ops/s) ▲15.2%  ← أبطأ من baseline
```

### مقارنة مع Baseline

```bash
# أول مرة: إنشاء baseline
./benchmarks
cp benchmark_results.json baseline.json

# مقارنة مع baseline
./benchmarks --baseline=baseline.json
```

---

*أُعدّ بواسطة: عمر — مهندس البنية التحتية*
*تاريخ: مارس 2026*
