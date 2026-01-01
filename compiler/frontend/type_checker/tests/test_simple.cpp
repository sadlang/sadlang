/*
 * ============================================================================
 * Simple Type Checker Tests - اختبارات بسيطة لمدقق الأنواع
 * ============================================================================
 */

#include <gtest/gtest.h>
#include "../include/typed_ast.h"
#include "../include/type_context.h"
#include "../include/type_errors.h"

using namespace Sad::TypeChecker;

// ============================================================================
// Basic Type Tests / اختبارات الأنواع الأساسية
// ============================================================================

TEST(TypedASTTest, BasicTypeCreation) {
    auto intType = std::make_shared<Type>(TypeKind::INT);
    EXPECT_EQ(intType->kind, TypeKind::INT);
    EXPECT_EQ(intType->toString(), "int");
}

TEST(TypedASTTest, FunctionTypeCreation) {
    auto intType = std::make_shared<Type>(TypeKind::INT);
    auto strType = std::make_shared<Type>(TypeKind::STRING);
    
    std::vector<std::shared_ptr<Type>> paramTypes = {intType, strType};
    auto funcType = std::make_shared<FunctionType>(paramTypes, intType);
    
    EXPECT_EQ(funcType->paramTypes.size(), 2);
    EXPECT_EQ(funcType->returnType->kind, TypeKind::INT);
}

TEST(TypedASTTest, ArrayTypeCreation) {
    auto intType = std::make_shared<Type>(TypeKind::INT);
    auto arrayType = std::make_shared<ArrayType>(intType);
    
    EXPECT_EQ(arrayType->kind, TypeKind::ARRAY);
    EXPECT_EQ(arrayType->elementType->kind, TypeKind::INT);
}

// ============================================================================
// Type Context Tests / اختبارات سياق الأنواع
// ============================================================================

TEST(TypeContextTest, BasicSymbolCreation) {
    auto symbol = std::make_shared<Symbol>(
        "x",
        std::make_shared<Type>(TypeKind::INT),
        SymbolKind::VARIABLE
    );
    
    EXPECT_EQ(symbol->name, "x");
    EXPECT_EQ(symbol->type->kind, TypeKind::INT);
    EXPECT_EQ(symbol->kind, SymbolKind::VARIABLE);
}

TEST(TypeContextTest, ScopeCreation) {
    Scope scope(Scope::Type::GLOBAL, "global");
    
    auto symbol = std::make_shared<Symbol>(
        "x",
        std::make_shared<Type>(TypeKind::INT),
        SymbolKind::VARIABLE
    );
    
    scope.addSymbol("x", symbol);
    
    auto found = scope.findSymbol("x");
    ASSERT_TRUE(found != nullptr);
    EXPECT_EQ(found->name, "x");
}

TEST(TypeContextTest, EnvironmentScopes) {
    auto env = std::make_shared<Environment>();
    
    // Add symbol to global scope
    auto symbol = std::make_shared<Symbol>(
        "globalVar",
        std::make_shared<Type>(TypeKind::INT),
        SymbolKind::VARIABLE
    );
    env->addSymbol("globalVar", symbol);
    
    // Push new scope
    env->pushScope(Scope::Type::BLOCK, "block1");
    
    // Add local symbol
    auto localSymbol = std::make_shared<Symbol>(
        "localVar",
        std::make_shared<Type>(TypeKind::STRING),
        SymbolKind::VARIABLE
    );
    env->addSymbol("localVar", localSymbol);
    
    // Should find both
    EXPECT_TRUE(env->findSymbol("globalVar") != nullptr);
    EXPECT_TRUE(env->findSymbol("localVar") != nullptr);
    
    // Pop scope
    env->popScope();
    
    // Should only find global
    EXPECT_TRUE(env->findSymbol("globalVar") != nullptr);
    EXPECT_FALSE(env->findSymbol("localVar") != nullptr);
}

// ============================================================================
// Type Error Reporter Tests / اختبارات مبلغ أخطاء الأنواع
// ============================================================================

TEST(TypeErrorReporterTest, BasicErrorCreation) {
    TypeErrorReporter reporter;
    
    auto error = TypeErrorBuilder()
        .setCode("E001")
        .setMessage("Type mismatch")
        .setArabicMessage("عدم تطابق الأنواع")
        .setSeverity(ErrorSeverity::ERROR)
        .setLine(10)
        .setColumn(5)
        .build();
    
    reporter.addError(error);
    
    EXPECT_TRUE(reporter.hasErrors());
    EXPECT_EQ(reporter.getErrorCount(), 1);
}

TEST(TypeErrorReporterTest, MultipleErrors) {
    TypeErrorReporter reporter;
    
    for (int i = 0; i < 5; ++i) {
        auto error = TypeErrorBuilder()
            .setCode("E" + std::to_string(i))
            .setMessage("Error " + std::to_string(i))
            .setSeverity(ErrorSeverity::ERROR)
            .build();
        reporter.addError(error);
    }
    
    EXPECT_EQ(reporter.getErrorCount(), 5);
}

TEST(TypeErrorReporterTest, WarningsVsErrors) {
    TypeErrorReporter reporter;
    
    // Add errors
    for (int i = 0; i < 3; ++i) {
        auto error = TypeErrorBuilder()
            .setCode("E" + std::to_string(i))
            .setMessage("Error")
            .setSeverity(ErrorSeverity::ERROR)
            .build();
        reporter.addError(error);
    }
    
    // Add warnings  
    for (int i = 0; i < 2; ++i) {
        auto warning = TypeErrorBuilder()
            .setCode("W" + std::to_string(i))
            .setMessage("Warning")
            .setSeverity(ErrorSeverity::WARNING)
            .build();
        reporter.addError(warning);
    }
    
    EXPECT_EQ(reporter.getErrorCount(), 3);
    EXPECT_EQ(reporter.getWarningCount(), 2);
}

// ============================================================================
// Main Test Entry Point
// ============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
