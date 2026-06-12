# Parser & AST Tests / اختبارات المحلل النحوي وشجرة AST

**التاريخ / Date:** 2025-11-11  
**الحالة / Status:** هيكل الاختبارات / Test Framework  
**التغطية المستهدفة / Target Coverage:** > 80%

---

## 📋 نظرة عامة / Overview

هذا المجلد يحتوي على جميع اختبارات المحلل النحوي (Parser) وشجرة التركيب المجرد (AST) للغة "ص" (Sad Language).

This folder contains all parser and Abstract Syntax Tree (AST) tests for the Sad Language.

### 🎯 الأهداف / Goals

1. **التحقق من صحة التحليل / Validate Parsing**
   - التأكد من تحليل جميع البُنى النحوية بشكل صحيح
   - اختبار 15 مستوى أولوية للعوامل
   - التحقق من بناء AST الصحيح

2. **اختبار معالجة الأخطاء / Test Error Handling**
   - Panic Mode Recovery
   - Error synchronization
   - Descriptive error messages

3. **التغطية الشاملة / Comprehensive Coverage**
   - 100% تغطية للميزات المُنفذة (85% من القواعد)
   - Edge cases و corner cases
   - اختبارات الأداء الأساسية

---

## 📁 هيكل المجلد / Folder Structure

```
tests/parser_tests/
├── README.md                           # هذا الملف / This file
├── test_framework.h                    # إطار الاختبار / Test framework
├── test_runner.cpp                     # مُشغل الاختبارات / Test runner
│
├── literals/                           # اختبارات القيم الحرفية
│   ├── test_numbers.cpp                # أرقام: int, float, binary, hex
│   ├── test_strings.cpp                # نصوص ومتعدد الأسطر
│   └── test_booleans_null.cpp          # منطقية و null
│
├── expressions/                        # اختبارات التعبيرات
│   ├── test_arithmetic.cpp             # +, -, *, /, %, **
│   ├── test_comparison.cpp             # <, >, <=, >=, ==, !=
│   ├── test_logical.cpp                # &&, ||, !
│   ├── test_precedence.cpp             # 15 مستوى أولوية
│   ├── test_assignment.cpp             # =, +=, -=, *=, /=
│   ├── test_postfix.cpp                # call(), member., index[]
│   └── test_comprehensions.cpp         # [x for x in list]
│
├── statements/                         # اختبارات الجمل
│   ├── test_if_else.cpp                # if-else-elseif chains
│   ├── test_loops.cpp                  # while, for
│   ├── test_control_flow.cpp           # return, break, continue
│   ├── test_blocks.cpp                 # { } scoping
│   ├── test_try_catch.cpp              # exception handling
│   └── test_with.cpp                   # resource management
│
├── declarations/                       # اختبارات التصريحات
│   ├── test_variables.cpp              # var declarations
│   ├── test_functions.cpp              # function declarations
│   ├── test_classes.cpp                # class declarations
│   ├── test_enums.cpp                  # enum declarations
│   └── test_imports.cpp                # import/export
│
├── arrays_maps/                        # اختبارات المصفوفات والخرائط
│   ├── test_arrays.cpp                 # [1, 2, 3]
│   ├── test_maps.cpp                   # {key: value}
│   └── test_access.cpp                 # arr[i], map[key]
│
├── error_handling/                     # اختبارات معالجة الأخطاء
│   ├── test_syntax_errors.cpp          # أخطاء نحوية
│   ├── test_recovery.cpp               # Panic mode
│   └── test_messages.cpp               # Error messages quality
│
├── integration/                        # اختبارات التكامل
│   ├── test_complete_programs.cpp      # برامج كاملة
│   ├── test_lexer_parser.cpp           # Lexer → Parser
│   └── test_parser_ast.cpp             # Parser → AST
│
└── benchmarks/                         # اختبارات الأداء
    ├── test_performance.cpp            # سرعة التحليل
    └── test_memory.cpp                 # استهلاك الذاكرة
```

---

## 🧪 إطار الاختبار / Test Framework

نستخدم إطار اختبار بسيط مبني على C++ مع دعم:

### الميزات / Features

- ✅ **Assertions**: `ASSERT_TRUE`, `ASSERT_EQ`, `ASSERT_THROWS`
- ✅ **Test Groups**: تنظيم الاختبارات في مجموعات
- ✅ **Setup/Teardown**: تهيئة وتنظيف قبل/بعد كل اختبار
- ✅ **Colored Output**: ألوان للنجاح/الفشل
- ✅ **Statistics**: عدد الاختبارات الناجحة/الفاشلة

### مثال استخدام / Usage Example

```cpp
#include "../test_framework.h"
#include "../../../include/parser/parser_core.h"

using namespace Sad;

TEST_GROUP(ArithmeticExpressions) {
    Lexer::LexerCore lexer;
    Parser::ParserCore* parser;
    
    void setup() {
        parser = nullptr;
    }
    
    void teardown() {
        delete parser;
    }
    
    TEST(SimpleAddition) {
        lexer.setSource("1 + 2");
        parser = new Parser::ParserCore(lexer);
        auto program = parser->parseProgram();
        
        ASSERT_FALSE(parser->hasErrors());
        ASSERT_EQ(program.size(), 1);
    }
    
    TEST(Precedence) {
        lexer.setSource("2 + 3 * 4");  // Should be: 2 + (3 * 4) = 14
        parser = new Parser::ParserCore(lexer);
        auto program = parser->parseProgram();
        
        ASSERT_FALSE(parser->hasErrors());
        // Verify AST structure shows correct precedence
    }
};

RUN_TEST_GROUP(ArithmeticExpressions);
```

---

## 📊 خطة الاختبارات / Test Plan

### Phase 1: اختبارات أساسية / Basic Tests (أسبوع 1)

**الهدف / Goal:** اختبار الميزات المُنفذة (85%)

| الفئة / Category | الاختبارات / Tests | الحالة / Status |
|-----------------|--------------------|--------------------|
| Literals | 30 tests | ⏳ TODO |
| Expressions | 50 tests | ⏳ TODO |
| Statements | 40 tests | ⏳ TODO |
| **Total** | **120 tests** | **0% complete** |

### Phase 2: اختبارات متقدمة / Advanced Tests (أسبوع 2)

| الفئة / Category | الاختبارات / Tests | الحالة / Status |
|-----------------|--------------------|--------------------|
| Declarations | 35 tests | ⏳ TODO |
| Arrays & Maps | 25 tests | ⏳ TODO |
| Error Handling | 30 tests | ⏳ TODO |
| **Total** | **90 tests** | **0% complete** |

### Phase 3: اختبارات التكامل / Integration Tests (أسبوع 3)

| الفئة / Category | الاختبارات / Tests | الحالة / Status |
|-----------------|--------------------|--------------------|
| Complete Programs | 20 tests | ⏳ TODO |
| Lexer-Parser | 15 tests | ⏳ TODO |
| Performance | 10 tests | ⏳ TODO |
| **Total** | **45 tests** | **0% complete** |

---

## 🎯 معايير النجاح / Success Criteria

### للمرحلة الأولى / For Phase 1
- [ ] 120 اختبار أساسي تمر بنجاح
- [ ] 0 أخطاء تصريف
- [ ] جميع الميزات المُنفذة (85%) مُختبرة
- [ ] توثيق كامل لكل اختبار

### للمشروع الكامل / For Full Project
- [ ] 255+ اختبار إجمالي
- [ ] تغطية كود > 80%
- [ ] جميع Edge cases مُختبرة
- [ ] اختبارات الأداء تمر
- [ ] CI/CD pipeline جاهز

---

## 🚀 كيفية التشغيل / How to Run

### بناء الاختبارات / Build Tests

```bash
# من المجلد الرئيسي / From root directory
cd build
cmake ..
make parser_tests
```

### تشغيل جميع الاختبارات / Run All Tests

```bash
./tests/parser_tests/test_runner
```

### تشغيل مجموعة محددة / Run Specific Group

```bash
./tests/parser_tests/test_runner --group=expressions
```

### تشغيل اختبار واحد / Run Single Test

```bash
./tests/parser_tests/test_runner --test=ArithmeticExpressions.SimpleAddition
```

---

## 📝 إرشادات كتابة الاختبارات / Test Writing Guidelines

### 1. التسمية / Naming

```cpp
// ✅ Good: واضح ومحدد
TEST(ParseSimpleAddition) { ... }
TEST(PrecedenceMultiplicationOverAddition) { ... }

// ❌ Bad: غامض
TEST(Test1) { ... }
TEST(DoStuff) { ... }
```

### 2. البنية / Structure

```cpp
TEST(TestName) {
    // Arrange: تحضير البيانات
    lexer.setSource("1 + 2");
    parser = new Parser::ParserCore(lexer);
    
    // Act: تنفيذ الإجراء
    auto program = parser->parseProgram();
    
    // Assert: التحقق من النتيجة
    ASSERT_FALSE(parser->hasErrors());
    ASSERT_EQ(program.size(), 1);
}
```

### 3. التغطية / Coverage

اختبر:
- ✅ الحالة الطبيعية / Normal case
- ✅ Edge cases (حدود)
- ✅ Error cases (أخطاء)
- ✅ Corner cases (حالات نادرة)

### 4. التوثيق / Documentation

```cpp
/**
 * @test ParseComplexExpression
 * @brief (AR) يختبر تحليل تعبير معقد مع عدة مستويات أولوية.
 *        (EN) Tests parsing of complex expression with multiple precedence levels.
 * 
 * Input: "2 + 3 * 4 - 5 / 2"
 * Expected AST:
 *   BinaryExpr(-,
 *     BinaryExpr(+, 2, BinaryExpr(*, 3, 4)),
 *     BinaryExpr(/, 5, 2)
 *   )
 */
TEST(ParseComplexExpression) { ... }
```

---

## 🐛 أمثلة اختبارات / Test Examples

### اختبار تعبير بسيط / Simple Expression Test

```cpp
TEST(SimpleAddition) {
    const char* source = "1 + 2";
    Lexer::LexerCore lexer(source);
    Parser::ParserCore parser(lexer);
    
    auto program = parser.parseProgram();
    
    ASSERT_FALSE(parser.hasErrors());
    ASSERT_EQ(program.size(), 1);
    
    auto* exprStmt = dynamic_cast<ExprStmt*>(program[0].get());
    ASSERT_NOT_NULL(exprStmt);
    
    auto* binary = dynamic_cast<BinaryExpr*>(exprStmt->getExpression());
    ASSERT_NOT_NULL(binary);
    ASSERT_EQ(binary->getOperator(), TokenType::OP_PLUS);
}
```

### اختبار معالجة خطأ / Error Handling Test

```cpp
TEST(MissingSemicolon) {
    const char* source = "var x = 5";  // Missing semicolon
    Lexer::LexerCore lexer(source);
    Parser::ParserCore parser(lexer);
    
    auto program = parser.parseProgram();
    
    ASSERT_TRUE(parser.hasErrors());
    auto errors = parser.getErrors();
    ASSERT_GT(errors.size(), 0);
    ASSERT_CONTAINS(errors[0], "Expected ';'");
}
```

### اختبار تكامل / Integration Test

```cpp
TEST(CompleteProgram) {
    const char* source = R"(
        function factorial(n) {
            if (n <= 1) {
                return 1;
            }
            return n * factorial(n - 1);
        }
        
        var result = factorial(5);
    )";
    
    Lexer::LexerCore lexer(source);
    Parser::ParserCore parser(lexer);
    
    auto program = parser.parseProgram();
    
    ASSERT_FALSE(parser.hasErrors());
    ASSERT_EQ(program.size(), 2);  // function + var
}
```

---

## 📈 تتبع التقدم / Progress Tracking

```
Phase 1 (Basic):      0/120 tests (0%)  ⏳ In Progress
Phase 2 (Advanced):   0/90 tests (0%)   ⏳ Waiting
Phase 3 (Integration): 0/45 tests (0%)   ⏳ Waiting
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total:                0/255 tests (0%)
```

**آخر تحديث / Last Updated:** 2025-11-11  
**الحالة / Status:** بدء المرحلة 1 / Starting Phase 1 🚀
