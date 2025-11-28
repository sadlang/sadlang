# 🧪 المرحلة 4: الاختبار الشامل والتوثيق
## Phase 4: Comprehensive Testing and Documentation

<div dir="rtl">

## 📋 نظرة عامة / Overview

**المدة المتوقعة**: 2-4 أيام  
**الأولوية**: عالية (P0)  
**التبعيات**: المراحل 1, 2, 3

### الأهداف
- اختبارات شاملة لجميع مكونات نظام الأخطاء
- توثيق كامل للـ API
- أمثلة وبرامج تعليمية
- دمج مع CI/CD
- دليل مساهمة محدث

---

## 🎯 المهام التفصيلية / Detailed Tasks

### المهمة 4.1: اختبارات الوحدات الشاملة ✅

**الملف**: `tests/errors/test_error_system.cpp`

#### بنية الاختبارات:

```cpp
/**
 * @file test_error_system.cpp
 * @brief (AR) اختبارات شاملة لنظام إدارة الأخطاء
 *        (EN) Comprehensive tests for error management system
 * 
 * @author Sad Language Development Team
 * @date November 23, 2025
 */

#include <gtest/gtest.h>
#include "errors/error_manager.h"
#include "errors/error_codes.h"
#include "errors/diagnostic.h"
#include "errors/source_location.h"

using namespace Sad::Errors;

// ====================================================================
// (AR) اختبارات ErrorCode / (EN) ErrorCode Tests
// ====================================================================

/**
 * @brief (AR) يختبر تحويل ErrorCode إلى نص
 *        (EN) Tests ErrorCode to string conversion
 */
TEST(ErrorCodesTest, GetCodeString) {
    EXPECT_EQ(getErrorCodeString(ErrorCode::LEX_INVALID_CHARACTER), "LEX001");
    EXPECT_EQ(getErrorCodeString(ErrorCode::SYN_MISSING_SEMICOLON), "SYN002");
    EXPECT_EQ(getErrorCodeString(ErrorCode::SEM_UNDEFINED_VARIABLE), "SEM001");
    EXPECT_EQ(getErrorCodeString(ErrorCode::RUN_DIVISION_BY_ZERO), "RUN001");
}

/**
 * @brief (AR) يختبر الحصول على وصف الخطأ بالعربية
 *        (EN) Tests getting error description in Arabic
 */
TEST(ErrorCodesTest, GetDescriptionArabic) {
    std::string desc = getErrorDescription(
        ErrorCode::SYN_MISSING_SEMICOLON,
        Language::ARABIC
    );
    
    EXPECT_FALSE(desc.empty());
    EXPECT_TRUE(desc.find("فاصلة") != std::string::npos ||
                desc.find(";") != std::string::npos);
}

/**
 * @brief (AR) يختبر الحصول على وصف الخطأ بالإنجليزية
 *        (EN) Tests getting error description in English
 */
TEST(ErrorCodesTest, GetDescriptionEnglish) {
    std::string desc = getErrorDescription(
        ErrorCode::SYN_MISSING_SEMICOLON,
        Language::ENGLISH
    );
    
    EXPECT_FALSE(desc.empty());
    EXPECT_TRUE(desc.find("semicolon") != std::string::npos ||
                desc.find(";") != std::string::npos);
}

/**
 * @brief (AR) يختبر الحصول على فئة الخطأ
 *        (EN) Tests getting error category
 */
TEST(ErrorCodesTest, GetCategory) {
    EXPECT_EQ(getErrorCategory(ErrorCode::LEX_INVALID_CHARACTER), "Lexical");
    EXPECT_EQ(getErrorCategory(ErrorCode::SYN_MISSING_SEMICOLON), "Syntax");
    EXPECT_EQ(getErrorCategory(ErrorCode::SEM_UNDEFINED_VARIABLE), "Semantic");
    EXPECT_EQ(getErrorCategory(ErrorCode::RUN_DIVISION_BY_ZERO), "Runtime");
}

// ====================================================================
// (AR) اختبارات SourceLocation / (EN) SourceLocation Tests
// ====================================================================

class SourceLocationTest : public ::testing::Test {
protected:
    SourceLocation validLoc{"test.s", 5, 10, 42, 3};
    SourceLocation invalidLoc;
};

TEST_F(SourceLocationTest, IsValid) {
    EXPECT_TRUE(validLoc.isValid());
    EXPECT_FALSE(invalidLoc.isValid());
}

TEST_F(SourceLocationTest, ToString) {
    EXPECT_EQ(validLoc.toString(), "test.s:5:10");
    EXPECT_EQ(invalidLoc.toString(), "<unknown location>");
}

TEST_F(SourceLocationTest, ToDetailedString) {
    std::string detailed = validLoc.toDetailedString();
    EXPECT_TRUE(detailed.find("test.s:5:10") != std::string::npos);
    EXPECT_TRUE(detailed.find("length: 3") != std::string::npos);
}

TEST_F(SourceLocationTest, Comparison) {
    SourceLocation loc1{"test.s", 5, 10};
    SourceLocation loc2{"test.s", 5, 10};
    SourceLocation loc3{"test.s", 6, 10};
    
    EXPECT_TRUE(loc1 == loc2);
    EXPECT_TRUE(loc1 < loc3);
    EXPECT_FALSE(loc3 < loc1);
}

// ====================================================================
// (AR) اختبارات Diagnostic / (EN) Diagnostic Tests
// ====================================================================

class DiagnosticTest : public ::testing::Test {
protected:
    SourceLocation location{"test.s", 5, 10, 42, 1};
    
    Diagnostic createTestDiagnostic() {
        return Diagnostic(
            ErrorCode::SYN_MISSING_SEMICOLON,
            DiagnosticSeverity::ERROR,
            location,
            "متوقع ';' بعد الجملة",
            "Expected ';' after statement"
        );
    }
};

TEST_F(DiagnosticTest, BasicProperties) {
    Diagnostic diag = createTestDiagnostic();
    
    EXPECT_EQ(diag.getCode(), ErrorCode::SYN_MISSING_SEMICOLON);
    EXPECT_EQ(diag.getSeverity(), DiagnosticSeverity::ERROR);
    EXPECT_EQ(diag.getLocation(), location);
    EXPECT_EQ(diag.getMessage(Language::ARABIC), "متوقع ';' بعد الجملة");
    EXPECT_EQ(diag.getMessage(Language::ENGLISH), "Expected ';' after statement");
}

TEST_F(DiagnosticTest, AddFixIt) {
    Diagnostic diag = createTestDiagnostic();
    
    FixItHint hint(
        FixItHint::Type::INSERT,
        SourceLocation{"test.s", 5, 11},
        ";",
        "أضف ';'",
        "Add ';'"
    );
    
    diag.addFixIt(hint);
    
    EXPECT_EQ(diag.getFixIts().size(), 1);
    EXPECT_EQ(diag.getFixIts()[0].replacement, ";");
}

TEST_F(DiagnosticTest, AddNote) {
    Diagnostic diag = createTestDiagnostic();
    
    Diagnostic note(
        ErrorCode::SYN_MISSING_SEMICOLON,
        DiagnosticSeverity::NOTE,
        SourceLocation{"test.s", 3, 5},
        "تم الإعلان هنا",
        "Declared here"
    );
    
    diag.addNote(note);
    
    EXPECT_EQ(diag.getNotes().size(), 1);
}

TEST_F(DiagnosticTest, FormatArabic) {
    Diagnostic diag = createTestDiagnostic();
    std::string formatted = diag.format(Language::ARABIC, false);
    
    EXPECT_TRUE(formatted.find("خطأ") != std::string::npos);
    EXPECT_TRUE(formatted.find("SYN002") != std::string::npos);
    EXPECT_TRUE(formatted.find("test.s:5:10") != std::string::npos);
    EXPECT_TRUE(formatted.find("متوقع ';' بعد الجملة") != std::string::npos);
}

TEST_F(DiagnosticTest, FormatEnglish) {
    Diagnostic diag = createTestDiagnostic();
    std::string formatted = diag.format(Language::ENGLISH, false);
    
    EXPECT_TRUE(formatted.find("error") != std::string::npos ||
                formatted.find("ERROR") != std::string::npos);
    EXPECT_TRUE(formatted.find("SYN002") != std::string::npos);
    EXPECT_TRUE(formatted.find("Expected ';' after statement") != std::string::npos);
}

TEST_F(DiagnosticTest, ToJSON) {
    Diagnostic diag = createTestDiagnostic();
    std::string json = diag.toJSON();
    
    EXPECT_TRUE(json.find("\"code\": \"SYN002\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"severity\": \"error\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"file\": \"test.s\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"line\": 5") != std::string::npos);
    EXPECT_TRUE(json.find("\"column\": 10") != std::string::npos);
}

// ====================================================================
// (AR) اختبارات ErrorManager / (EN) ErrorManager Tests
// ====================================================================

class ErrorManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        ErrorManager::getInstance().clear();
    }
    
    void TearDown() override {
        ErrorManager::getInstance().clear();
    }
};

TEST_F(ErrorManagerTest, Singleton) {
    ErrorManager& mgr1 = ErrorManager::getInstance();
    ErrorManager& mgr2 = ErrorManager::getInstance();
    
    EXPECT_EQ(&mgr1, &mgr2);  // Same instance
}

TEST_F(ErrorManagerTest, ReportError) {
    ErrorManager::getInstance().reportError(
        ErrorCode::SYN_MISSING_SEMICOLON,
        SourceLocation{"test.s", 5, 10},
        "متوقع ';'",
        "Expected ';'"
    );
    
    EXPECT_TRUE(ErrorManager::getInstance().hasErrors());
    EXPECT_EQ(ErrorManager::getInstance().getErrorCount(), 1);
    EXPECT_EQ(ErrorManager::getInstance().getWarningCount(), 0);
}

TEST_F(ErrorManagerTest, ReportWarning) {
    ErrorManager::getInstance().reportWarning(
        ErrorCode::SYN_UNEXPECTED_TOKEN,
        SourceLocation{"test.s", 3, 5},
        "رمز غير متوقع",
        "Unexpected token"
    );
    
    EXPECT_FALSE(ErrorManager::getInstance().hasErrors());
    EXPECT_EQ(ErrorManager::getInstance().getErrorCount(), 0);
    EXPECT_EQ(ErrorManager::getInstance().getWarningCount(), 1);
}

TEST_F(ErrorManagerTest, MultipleErrors) {
    for (int i = 0; i < 5; i++) {
        ErrorManager::getInstance().reportError(
            ErrorCode::LEX_INVALID_CHARACTER,
            SourceLocation{"test.s", i + 1, 1},
            "خطأ " + std::to_string(i),
            "Error " + std::to_string(i)
        );
    }
    
    EXPECT_EQ(ErrorManager::getInstance().getErrorCount(), 5);
    EXPECT_EQ(ErrorManager::getInstance().getAllDiagnostics().size(), 5);
}

TEST_F(ErrorManagerTest, Clear) {
    ErrorManager::getInstance().reportError(
        ErrorCode::SYN_MISSING_SEMICOLON,
        SourceLocation{"test.s", 5, 10},
        "خطأ",
        "Error"
    );
    
    EXPECT_TRUE(ErrorManager::getInstance().hasErrors());
    
    ErrorManager::getInstance().clear();
    
    EXPECT_FALSE(ErrorManager::getInstance().hasErrors());
    EXPECT_EQ(ErrorManager::getInstance().getErrorCount(), 0);
}

TEST_F(ErrorManagerTest, MaxErrors) {
    ErrorManager::getInstance().setMaxErrors(3);
    
    for (int i = 0; i < 10; i++) {
        ErrorManager::getInstance().reportError(
            ErrorCode::LEX_INVALID_CHARACTER,
            SourceLocation{"test.s", i + 1, 1},
            "خطأ",
            "Error"
        );
    }
    
    // Should stop after max errors
    EXPECT_LE(ErrorManager::getInstance().getErrorCount(), 3);
}

// ====================================================================
// (AR) اختبارات DiagnosticBuilder / (EN) DiagnosticBuilder Tests
// ====================================================================

TEST(DiagnosticBuilderTest, BasicBuild) {
    ErrorManager::getInstance().clear();
    
    DiagnosticBuilder(
        ErrorCode::SYN_MISSING_SEMICOLON,
        DiagnosticSeverity::ERROR,
        SourceLocation{"test.s", 5, 10}
    )
    .withMessage("متوقع ';'", "Expected ';'")
    .report();
    
    EXPECT_TRUE(ErrorManager::getInstance().hasErrors());
    EXPECT_EQ(ErrorManager::getInstance().getErrorCount(), 1);
}

TEST(DiagnosticBuilderTest, WithFixIt) {
    ErrorManager::getInstance().clear();
    
    DiagnosticBuilder(
        ErrorCode::SYN_MISSING_SEMICOLON,
        DiagnosticSeverity::ERROR,
        SourceLocation{"test.s", 5, 10}
    )
    .withMessage("متوقع ';'", "Expected ';'")
    .withFixItInsert(SourceLocation{"test.s", 5, 11}, ";")
    .report();
    
    auto diags = ErrorManager::getInstance().getAllDiagnostics();
    ASSERT_EQ(diags.size(), 1);
    EXPECT_FALSE(diags[0].getFixIts().empty());
}

// ====================================================================
// (AR) اختبارات التنسيق / (EN) Formatting Tests
// ====================================================================

TEST(FormattingTest, ColorizedOutput) {
    Diagnostic diag(
        ErrorCode::SYN_MISSING_SEMICOLON,
        DiagnosticSeverity::ERROR,
        SourceLocation{"test.s", 5, 10},
        "متوقع ';'",
        "Expected ';'"
    );
    
    std::string sourceCode = "عرض(5)\n";
    std::string formatted = diag.format(Language::ARABIC, true, sourceCode);
    
    // Should contain ANSI color codes
    EXPECT_TRUE(formatted.find("\033[") != std::string::npos);
    // Should contain error icon
    EXPECT_TRUE(formatted.find("❌") != std::string::npos);
}

TEST(FormattingTest, PlainOutput) {
    Diagnostic diag(
        ErrorCode::SYN_MISSING_SEMICOLON,
        DiagnosticSeverity::ERROR,
        SourceLocation{"test.s", 5, 10},
        "متوقع ';'",
        "Expected ';'"
    );
    
    std::string formatted = diag.format(Language::ENGLISH, false);
    
    // Should NOT contain ANSI color codes
    EXPECT_TRUE(formatted.find("\033[") == std::string::npos);
}

// ====================================================================
// (AR) اختبارات Thread Safety / (EN) Thread Safety Tests
// ====================================================================

#include <thread>
#include <vector>

TEST(ThreadSafetyTest, ConcurrentReporting) {
    ErrorManager::getInstance().clear();
    
    const int numThreads = 10;
    const int errorsPerThread = 100;
    
    std::vector<std::thread> threads;
    
    for (int t = 0; t < numThreads; t++) {
        threads.emplace_back([t, errorsPerThread]() {
            for (int i = 0; i < errorsPerThread; i++) {
                ErrorManager::getInstance().reportError(
                    ErrorCode::LEX_INVALID_CHARACTER,
                    SourceLocation{"test.s", t * 100 + i, 1},
                    "خطأ متزامن",
                    "Concurrent error"
                );
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(ErrorManager::getInstance().getErrorCount(),
              numThreads * errorsPerThread);
}

// ====================================================================
// (AR) Main / (EN) Main
// ====================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```

**الوقت المتوقع**: 8-12 ساعة  
**تغطية الكود المطلوبة**: > 90%

---

### المهمة 4.2: اختبارات التكامل ✅

**الملف**: `tests/integration/test_error_integration.cpp`

```cpp
/**
 * @brief (AR) اختبار تكامل Lexer مع ErrorManager
 *        (EN) Tests Lexer integration with ErrorManager
 */
TEST(LexerIntegrationTest, InvalidCharacterHandling) {
    ErrorManager::getInstance().clear();
    
    std::string source = "متغير @ = 5;";
    Lexer lexer(source, "test.s");
    
    auto tokens = lexer.scanTokens();
    
    EXPECT_TRUE(ErrorManager::getInstance().hasErrors());
    
    auto diags = ErrorManager::getInstance().getAllDiagnostics();
    ASSERT_GE(diags.size(), 1);
    EXPECT_EQ(diags[0].getCode(), ErrorCode::LEX_INVALID_CHARACTER);
    EXPECT_EQ(diags[0].getLocation().line, 1);
}

/**
 * @brief (AR) اختبار تكامل Parser مع ErrorManager
 *        (EN) Tests Parser integration with ErrorManager
 */
TEST(ParserIntegrationTest, MissingSemicolonDetection) {
    ErrorManager::getInstance().clear();
    
    std::string source = "عرض(5)";  // Missing semicolon
    Lexer lexer(source, "test.s");
    auto tokens = lexer.scanTokens();
    
    ParserCore parser(tokens, "test.s");
    auto ast = parser.parse();
    
    EXPECT_TRUE(ErrorManager::getInstance().hasErrors());
    
    auto diags = ErrorManager::getInstance().getAllDiagnostics();
    ASSERT_GE(diags.size(), 1);
    EXPECT_EQ(diags[0].getCode(), ErrorCode::SYN_MISSING_SEMICOLON);
}

/**
 * @brief (AR) اختبار Fix-it Hints
 *        (EN) Tests Fix-it Hints
 */
TEST(FixItTest, SemicolonInsertion) {
    ErrorManager::getInstance().clear();
    
    std::string source = "عرض(5)";
    Lexer lexer(source, "test.s");
    auto tokens = lexer.scanTokens();
    
    ParserCore parser(tokens, "test.s");
    auto ast = parser.parse();
    
    auto diags = ErrorManager::getInstance().getAllDiagnostics();
    ASSERT_GE(diags.size(), 1);
    
    const auto& fixits = diags[0].getFixIts();
    ASSERT_FALSE(fixits.empty());
    EXPECT_EQ(fixits[0].type, FixItHint::Type::INSERT);
    EXPECT_EQ(fixits[0].replacement, ";");
}
```

**الوقت المتوقع**: 4-6 ساعات

---

### المهمة 4.3: توثيق API الكامل ✅

**الملف**: `docs/api/error_management_api.md`

```markdown
# 📚 Error Management API Documentation
## توثيق واجهة برمجة نظام إدارة الأخطاء

## Overview / نظرة عامة

This document provides complete API documentation for the Sad Language Error Management System.

يوفر هذا المستند توثيقاً كاملاً لواجهة برمجة نظام إدارة الأخطاء في لغة "ص".

---

## Table of Contents / جدول المحتويات

1. [ErrorCode](#errorcode)
2. [SourceLocation](#sourcelocation)
3. [Diagnostic](#diagnostic)
4. [ErrorManager](#errormanager)
5. [DiagnosticBuilder](#diagnosticbuilder)
6. [Examples](#examples)

---

## ErrorCode

### Description / الوصف

Enumeration of all error codes in the Sad language.

تعداد لجميع رموز الأخطاء في لغة "ص".

### Categories / الفئات

- **LEX001-LEX099**: Lexical errors / أخطاء معجمية
- **SYN001-SYN099**: Syntax errors / أخطاء نحوية
- **SEM001-SEM099**: Semantic errors / أخطاء دلالية
- **RUN001-RUN099**: Runtime errors / أخطاء وقت التشغيل

### Functions / الدوال

```cpp
// (AR) يحول رمز الخطأ إلى نص
// (EN) Converts error code to string
std::string getErrorCodeString(ErrorCode code);

// Example / مثال:
getErrorCodeString(ErrorCode::SYN_MISSING_SEMICOLON);  // Returns "SYN002"
```

```cpp
// (AR) يرجع وصف الخطأ باللغة المحددة
// (EN) Returns error description in specified language
std::string getErrorDescription(ErrorCode code, Language lang);

// Example / مثال:
getErrorDescription(ErrorCode::SYN_MISSING_SEMICOLON, Language::ARABIC);
// Returns: "فاصلة منقوطة مفقودة"
```

---

## SourceLocation

### Description / الوصف

Represents a location in source code.

يمثل موقعاً في الكود المصدري.

### Constructor / البناء

```cpp
SourceLocation(
    const std::string& filename,
    size_t line,
    size_t column,
    size_t offset = 0,
    size_t length = 1
);
```

### Methods / الطرق

```cpp
bool isValid() const;
std::string toString() const;  // Returns "file:line:col"
```

---

## Complete Examples / أمثلة كاملة

See: `examples/error_handling/`

```

**الوقت المتوقع**: 3-4 ساعات

---

### المهمة 4.4: دمج مع CI/CD ✅

**الملف**: `.github/workflows/error-tests.yml`

```yaml
name: Error Management Tests

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main, develop ]

jobs:
  error-tests:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest]
        build_type: [Debug, Release]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}
    
    - name: Build
      run: cmake --build build --config ${{ matrix.build_type }}
    
    - name: Run Error System Tests
      run: |
        cd build
        ctest -C ${{ matrix.build_type }} -R "Error*" --output-on-failure
    
    - name: Generate Coverage Report
      if: matrix.build_type == 'Debug'
      run: |
        # Generate coverage...
```

**الوقت المتوقع**: 2-3 ساعات

---

## 📊 التقدم والإنجاز / Progress Tracking

### قائمة المهام (Checklist):

- [ ] المهمة 4.1: اختبارات الوحدات (8-12 ساعة)
- [ ] المهمة 4.2: اختبارات التكامل (4-6 ساعات)
- [ ] المهمة 4.3: توثيق API (3-4 ساعات)
- [ ] المهمة 4.4: CI/CD Integration (2-3 ساعات)
- [ ] المهمة 4.5: أمثلة وبرامج تعليمية (2-3 ساعات)
- [ ] المهمة 4.6: دليل المساهمة (1-2 ساعة)

**إجمالي الوقت المتوقع**: 20-30 ساعة (~3-4 أيام عمل)

---

## 📈 معايير الإنجاز / Completion Criteria

- ✅ تغطية كود > 90%
- ✅ جميع الاختبارات تمر
- ✅ توثيق API كامل
- ✅ CI/CD يعمل
- ✅ أمثلة شاملة
- ✅ دليل مساهمة محدث

---

**التاريخ**: 23 نوفمبر 2025  
**الحالة**: جاهز للتنفيذ  
**الإنجاز الكامل**: نظام إدارة الأخطاء جاهز للاستخدام الإنتاجي

</div>
