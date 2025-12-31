/*
 * ============================================================================
 * Type Error Reporting Tests - اختبارات نظام الأخطاء
 * Type Error Reporting Tests
 * ============================================================================
 * 
 * الوصف (Description):
 *   اختبارات شاملة لنظام Type Error Reporting
 *   Comprehensive tests for Type Error Reporting system
 * 
 * المكونات المُختبَرة (Tested Components):
 *   - TypeError class
 *   - TypeErrorBuilder
 *   - TypeErrorReporter
 *   - FixSuggestion
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 31, 2025
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#include <gtest/gtest.h>
#include "type_errors.h"
#include "lexer/token.h"

using namespace Sad::TypeChecker;

// ============================================================================
// TypeError Tests - اختبارات TypeError
// ============================================================================

/**
 * اختبار: إنشاء خطأ بسيط / Test: Create simple error
 */
TEST(TypeErrorTest, CreateSimpleError) {
    // إنشاء خطأ بسيط / Create simple error
    TypeError error(
        TypeError::Severity::ERROR,
        "E001",
        "متغير غير معرّف",
        "Undefined variable",
        Lexer::Position{10, 5}
    );
    
    // التحقق من البيانات / Verify data
    EXPECT_EQ(error.getSeverity(), TypeError::Severity::ERROR);
    EXPECT_EQ(error.getCode(), "E001");
    EXPECT_EQ(error.getMessageAr(), "متغير غير معرّف");
    EXPECT_EQ(error.getMessageEn(), "Undefined variable");
    EXPECT_EQ(error.getPrimaryLocation().line, 10);
    EXPECT_EQ(error.getPrimaryLocation().column, 5);
}

/**
 * اختبار: خطأ مع أنواع / Test: Error with types
 */
TEST(TypeErrorTest, ErrorWithTypes) {
    auto intType = TypeFactory::getIntType();
    auto stringType = TypeFactory::getStringType();
    
    TypeError error(
        TypeError::Severity::ERROR,
        "E002",
        "تعارض في الأنواع",
        "Type mismatch",
        Lexer::Position{15, 10}
    );
    
    error.setExpectedType(intType);
    error.setActualType(stringType);
    
    // التحقق من الأنواع / Verify types
    EXPECT_NE(error.getExpectedType(), nullptr);
    EXPECT_NE(error.getActualType(), nullptr);
    EXPECT_EQ(error.getExpectedType()->getKind(), Type::Kind::INT);
    EXPECT_EQ(error.getActualType()->getKind(), Type::Kind::STRING);
}

/**
 * اختبار: إضافة مواقع ثانوية / Test: Add secondary locations
 */
TEST(TypeErrorTest, AddSecondaryLocations) {
    TypeError error(
        TypeError::Severity::ERROR,
        "E003",
        "إعادة تعريف متغير",
        "Variable redefinition",
        Lexer::Position{20, 8}
    );
    
    error.addSecondaryLocation(Lexer::Position{10, 5});
    error.addSecondaryLocation(Lexer::Position{15, 12});
    
    // التحقق من عدد المواقع / Verify location count
    EXPECT_EQ(error.getSecondaryLocations().size(), 2);
    EXPECT_EQ(error.getSecondaryLocations()[0].line, 10);
    EXPECT_EQ(error.getSecondaryLocations()[1].line, 15);
}

/**
 * اختبار: إضافة اقتراحات / Test: Add suggestions
 */
TEST(TypeErrorTest, AddSuggestions) {
    TypeError error(
        TypeError::Severity::ERROR,
        "E004",
        "خطأ في النوع",
        "Type error",
        Lexer::Position{25, 3}
    );
    
    error.addSuggestion(FixSuggestion{
        "جرب تحويل النوع",
        Lexer::Position{25, 3},
        "int(value)"
    });
    
    error.addSuggestion(FixSuggestion{
        "أو غيّر نوع المتغير",
        Lexer::Position{24, 5},
        std::nullopt
    });
    
    // التحقق من الاقتراحات / Verify suggestions
    EXPECT_EQ(error.getSuggestions().size(), 2);
    EXPECT_EQ(error.getSuggestions()[0].message, "جرب تحويل النوع");
    EXPECT_TRUE(error.getSuggestions()[0].replacement.has_value());
    EXPECT_FALSE(error.getSuggestions()[1].replacement.has_value());
}

// ============================================================================
// TypeErrorBuilder Tests - اختبارات TypeErrorBuilder
// ============================================================================

/**
 * اختبار: Builder Pattern أساسي / Test: Basic builder pattern
 */
TEST(TypeErrorBuilderTest, BasicBuilder) {
    auto error = TypeErrorBuilder::error("E001")
        .messageAr("رسالة عربية")
        .messageEn("English message")
        .location(Lexer::Position{5, 10})
        .build();
    
    EXPECT_EQ(error.getSeverity(), TypeError::Severity::ERROR);
    EXPECT_EQ(error.getCode(), "E001");
    EXPECT_EQ(error.getMessageAr(), "رسالة عربية");
    EXPECT_EQ(error.getMessageEn(), "English message");
    EXPECT_EQ(error.getPrimaryLocation().line, 5);
}

/**
 * اختبار: بناء تحذير / Test: Build warning
 */
TEST(TypeErrorBuilderTest, BuildWarning) {
    auto warning = TypeErrorBuilder::warning("W001")
        .messageAr("تحذير")
        .messageEn("Warning")
        .location(Lexer::Position{8, 15})
        .build();
    
    EXPECT_EQ(warning.getSeverity(), TypeError::Severity::WARNING);
    EXPECT_EQ(warning.getCode(), "W001");
}

/**
 * اختبار: Builder مع أنواع / Test: Builder with types
 */
TEST(TypeErrorBuilderTest, BuilderWithTypes) {
    auto intType = TypeFactory::getIntType();
    auto floatType = TypeFactory::getFloatType();
    
    auto error = TypeErrorBuilder::error("E002")
        .messageAr("تعارض أنواع")
        .messageEn("Type mismatch")
        .location(Lexer::Position{12, 20})
        .expectedType(intType)
        .actualType(floatType)
        .build();
    
    EXPECT_NE(error.getExpectedType(), nullptr);
    EXPECT_NE(error.getActualType(), nullptr);
    EXPECT_EQ(error.getExpectedType()->getKind(), Type::Kind::INT);
    EXPECT_EQ(error.getActualType()->getKind(), Type::Kind::FLOAT);
}

/**
 * اختبار: Builder مع اقتراحات / Test: Builder with suggestions
 */
TEST(TypeErrorBuilderTest, BuilderWithSuggestions) {
    auto error = TypeErrorBuilder::error("E003")
        .messageAr("خطأ")
        .messageEn("Error")
        .location(Lexer::Position{18, 25})
        .addSuggestion("اقتراح 1", Lexer::Position{18, 25})
        .addSuggestion("اقتراح 2", Lexer::Position{18, 30}, "fix_code()")
        .build();
    
    EXPECT_EQ(error.getSuggestions().size(), 2);
    EXPECT_EQ(error.getSuggestions()[0].message, "اقتراح 1");
    EXPECT_FALSE(error.getSuggestions()[0].replacement.has_value());
    EXPECT_TRUE(error.getSuggestions()[1].replacement.has_value());
}

// ============================================================================
// TypeErrorReporter Tests - اختبارات TypeErrorReporter
// ============================================================================

/**
 * اختبار: إضافة وحساب الأخطاء / Test: Add and count errors
 */
TEST(TypeErrorReporterTest, AddAndCountErrors) {
    TypeErrorReporter reporter;
    
    EXPECT_FALSE(reporter.hasErrors());
    EXPECT_EQ(reporter.getErrorCount(), 0);
    
    reporter.addError("E001", "خطأ 1", "Error 1", Lexer::Position{5, 5});
    reporter.addError("E002", "خطأ 2", "Error 2", Lexer::Position{10, 10});
    
    EXPECT_TRUE(reporter.hasErrors());
    EXPECT_EQ(reporter.getErrorCount(), 2);
}

/**
 * اختبار: إضافة وحساب التحذيرات / Test: Add and count warnings
 */
TEST(TypeErrorReporterTest, AddAndCountWarnings) {
    TypeErrorReporter reporter;
    
    EXPECT_FALSE(reporter.hasWarnings());
    EXPECT_EQ(reporter.getWarningCount(), 0);
    
    reporter.addWarning("W001", "تحذير 1", "Warning 1", Lexer::Position{3, 8});
    reporter.addWarning("W002", "تحذير 2", "Warning 2", Lexer::Position{7, 12});
    
    EXPECT_TRUE(reporter.hasWarnings());
    EXPECT_EQ(reporter.getWarningCount(), 2);
}

/**
 * اختبار: مسح الأخطاء / Test: Clear errors
 */
TEST(TypeErrorReporterTest, ClearErrors) {
    TypeErrorReporter reporter;
    
    reporter.addError("E001", "خطأ", "Error", Lexer::Position{5, 5});
    reporter.addWarning("W001", "تحذير", "Warning", Lexer::Position{6, 6});
    
    EXPECT_TRUE(reporter.hasErrors());
    EXPECT_TRUE(reporter.hasWarnings());
    
    reporter.clear();
    
    EXPECT_FALSE(reporter.hasErrors());
    EXPECT_FALSE(reporter.hasWarnings());
    EXPECT_EQ(reporter.getErrorCount(), 0);
    EXPECT_EQ(reporter.getWarningCount(), 0);
}

/**
 * اختبار: تعيين الملف الحالي / Test: Set current file
 */
TEST(TypeErrorReporterTest, SetCurrentFile) {
    TypeErrorReporter reporter;
    
    reporter.setCurrentFile("test.s");
    // لا يمكن اختبار القيمة مباشرة لكن نتحقق من عدم حدوث استثناء
    // Can't test value directly but verify no exception
    EXPECT_NO_THROW(reporter.addError("E001", "خطأ", "Error", Lexer::Position{1, 1}));
}

/**
 * اختبار: تعيين خطوط المصدر / Test: Set source lines
 */
TEST(TypeErrorReporterTest, SetSourceLines) {
    TypeErrorReporter reporter;
    
    std::vector<std::string> lines = {
        "متغير س: رقم = 10",
        "متغير ص: نص = \"مرحبا\"",
        "طباعة(س + ص)"
    };
    
    reporter.setSourceLines(lines);
    // التحقق من عدم حدوث استثناء / Verify no exception
    EXPECT_NO_THROW(reporter.addError("E001", "خطأ", "Error", Lexer::Position{2, 5}));
}

// ============================================================================
// FixSuggestion Tests - اختبارات FixSuggestion
// ============================================================================

/**
 * اختبار: اقتراح بسيط / Test: Simple suggestion
 */
TEST(FixSuggestionTest, SimpleSuggestion) {
    FixSuggestion suggestion{
        "استخدم int()",
        Lexer::Position{10, 5},
        std::nullopt
    };
    
    EXPECT_EQ(suggestion.message, "استخدم int()");
    EXPECT_EQ(suggestion.location.line, 10);
    EXPECT_FALSE(suggestion.replacement.has_value());
}

/**
 * اختبار: اقتراح مع كود بديل / Test: Suggestion with replacement
 */
TEST(FixSuggestionTest, SuggestionWithReplacement) {
    FixSuggestion suggestion{
        "غيّر إلى",
        Lexer::Position{15, 8},
        "int(value)"
    };
    
    EXPECT_EQ(suggestion.message, "غيّر إلى");
    EXPECT_TRUE(suggestion.replacement.has_value());
    EXPECT_EQ(suggestion.replacement.value(), "int(value)");
}

// ============================================================================
// Integration Tests - اختبارات التكامل
// ============================================================================

/**
 * اختبار: سيناريو كامل / Test: Complete scenario
 */
TEST(ErrorReportingIntegrationTest, CompleteScenario) {
    TypeErrorReporter reporter;
    reporter.setCurrentFile("test.s");
    
    // إضافة خطأ معقد / Add complex error
    auto intType = TypeFactory::getIntType();
    auto stringType = TypeFactory::getStringType();
    
    auto error = TypeErrorBuilder::error("E001")
        .messageAr("تعارض في الأنواع: لا يمكن إسناد نص إلى رقم")
        .messageEn("Type mismatch: cannot assign string to integer")
        .location(Lexer::Position{5, 15})
        .expectedType(intType)
        .actualType(stringType)
        .addSuggestion("جرب تحويل النوع باستخدام رقم()", Lexer::Position{5, 20}, "رقم(value)")
        .addNote("المتغيرات في ص لها أنواع ثابتة")
        .build();
    
    reporter.addError(error);
    
    // إضافة تحذير / Add warning
    auto warning = TypeErrorBuilder::warning("W001")
        .messageAr("متغير غير مستخدم: x")
        .messageEn("Unused variable: x")
        .location(Lexer::Position{3, 8})
        .addSuggestion("احذف المتغير إذا لم تحتاجه", Lexer::Position{3, 8})
        .build();
    
    reporter.addWarning(warning);
    
    // التحقق من النتائج / Verify results
    EXPECT_TRUE(reporter.hasErrors());
    EXPECT_TRUE(reporter.hasWarnings());
    EXPECT_EQ(reporter.getErrorCount(), 1);
    EXPECT_EQ(reporter.getWarningCount(), 1);
}

// ============================================================================
// EOF
// ============================================================================
