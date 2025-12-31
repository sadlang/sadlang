/*
 * ============================================================================
 * Type Context Tests - اختبارات سياق الأنواع
 * Type Context Tests
 * ============================================================================
 * 
 * الوصف (Description):
 *   اختبارات شاملة لنظام Type Context
 *   Comprehensive tests for Type Context system
 * 
 * المكونات المُختبَرة (Tested Components):
 *   - Environment (البيئة)
 *   - Symbol Table (جدول الرموز)
 *   - Scope Management (إدارة النطاقات)
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 31, 2025
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#include <gtest/gtest.h>
#include "type_context.h"

using namespace Sad::TypeChecker;

// ============================================================================
// Symbol Tests - اختبارات الرموز
// ============================================================================

/**
 * اختبار: إنشاء رمز / Test: Create symbol
 */
TEST(SymbolTest, CreateSymbol) {
    auto intType = TypeFactory::getIntType();
    Symbol symbol("x", intType, Symbol::Kind::VARIABLE);
    
    EXPECT_EQ(symbol.getName(), "x");
    EXPECT_TRUE(symbol.getType()->equals(intType.get()));
    EXPECT_EQ(symbol.getKind(), Symbol::Kind::VARIABLE);
    EXPECT_TRUE(symbol.isMutable());
    EXPECT_FALSE(symbol.isInitialized());
}

/**
 * اختبار: رمز ثابت / Test: Constant symbol
 */
TEST(SymbolTest, ConstantSymbol) {
    auto stringType = TypeFactory::getStringType();
    Symbol symbol("name", stringType, Symbol::Kind::VARIABLE, false);
    
    EXPECT_FALSE(symbol.isMutable());
}

/**
 * اختبار: تهيئة رمز / Test: Initialize symbol
 */
TEST(SymbolTest, InitializeSymbol) {
    auto floatType = TypeFactory::getFloatType();
    Symbol symbol("pi", floatType, Symbol::Kind::VARIABLE);
    
    EXPECT_FALSE(symbol.isInitialized());
    
    symbol.setInitialized(true);
    
    EXPECT_TRUE(symbol.isInitialized());
}

// ============================================================================
// Scope Tests - اختبارات النطاقات
// ============================================================================

/**
 * اختبار: إنشاء نطاق / Test: Create scope
 */
TEST(ScopeTest, CreateScope) {
    Scope scope(Scope::Type::GLOBAL, "global");
    
    EXPECT_EQ(scope.getType(), Scope::Type::GLOBAL);
    EXPECT_EQ(scope.getName(), "global");
    EXPECT_EQ(scope.getSymbols().size(), 0);
}

/**
 * اختبار: إضافة رموز / Test: Add symbols
 */
TEST(ScopeTest, AddSymbols) {
    Scope scope(Scope::Type::FUNCTION, "main");
    auto intType = TypeFactory::getIntType();
    
    auto symbol1 = std::make_shared<Symbol>("x", intType, Symbol::Kind::VARIABLE);
    auto symbol2 = std::make_shared<Symbol>("y", intType, Symbol::Kind::VARIABLE);
    
    scope.addSymbol("x", symbol1);
    scope.addSymbol("y", symbol2);
    
    EXPECT_EQ(scope.getSymbols().size(), 2);
}

/**
 * اختبار: البحث عن رمز / Test: Lookup symbol
 */
TEST(ScopeTest, LookupSymbol) {
    Scope scope(Scope::Type::BLOCK, "block1");
    auto stringType = TypeFactory::getStringType();
    
    auto symbol = std::make_shared<Symbol>("name", stringType, Symbol::Kind::VARIABLE);
    scope.addSymbol("name", symbol);
    
    auto found = scope.lookupSymbol("name");
    EXPECT_NE(found, nullptr);
    EXPECT_EQ(found->getName(), "name");
    
    auto notFound = scope.lookupSymbol("age");
    EXPECT_EQ(notFound, nullptr);
}

/**
 * اختبار: رموز متعددة / Test: Multiple symbols
 */
TEST(ScopeTest, MultipleSymbols) {
    Scope scope(Scope::Type::CLASS, "Person");
    
    auto intType = TypeFactory::getIntType();
    auto stringType = TypeFactory::getStringType();
    auto boolType = TypeFactory::getBoolType();
    
    scope.addSymbol("age", std::make_shared<Symbol>("age", intType, Symbol::Kind::VARIABLE));
    scope.addSymbol("name", std::make_shared<Symbol>("name", stringType, Symbol::Kind::VARIABLE));
    scope.addSymbol("active", std::make_shared<Symbol>("active", boolType, Symbol::Kind::VARIABLE));
    
    EXPECT_EQ(scope.getSymbols().size(), 3);
    EXPECT_NE(scope.lookupSymbol("age"), nullptr);
    EXPECT_NE(scope.lookupSymbol("name"), nullptr);
    EXPECT_NE(scope.lookupSymbol("active"), nullptr);
}

// ============================================================================
// Environment Tests - اختبارات البيئة
// ============================================================================

/**
 * اختبار: إنشاء بيئة / Test: Create environment
 */
TEST(EnvironmentTest, CreateEnvironment) {
    Environment env;
    
    EXPECT_EQ(env.getCurrentScopeDepth(), 0);
}

/**
 * اختبار: دفع وسحب نطاقات / Test: Push and pop scopes
 */
TEST(EnvironmentTest, PushPopScopes) {
    Environment env;
    
    env.pushScope(Scope::Type::GLOBAL, "global");
    EXPECT_EQ(env.getCurrentScopeDepth(), 1);
    
    env.pushScope(Scope::Type::FUNCTION, "main");
    EXPECT_EQ(env.getCurrentScopeDepth(), 2);
    
    env.pushScope(Scope::Type::BLOCK, "block1");
    EXPECT_EQ(env.getCurrentScopeDepth(), 3);
    
    env.popScope();
    EXPECT_EQ(env.getCurrentScopeDepth(), 2);
    
    env.popScope();
    EXPECT_EQ(env.getCurrentScopeDepth(), 1);
}

/**
 * اختبار: إضافة رمز للنطاق الحالي / Test: Add symbol to current scope
 */
TEST(EnvironmentTest, AddSymbolToCurrentScope) {
    Environment env;
    env.pushScope(Scope::Type::GLOBAL, "global");
    
    auto intType = TypeFactory::getIntType();
    auto symbol = std::make_shared<Symbol>("x", intType, Symbol::Kind::VARIABLE);
    
    env.addSymbol("x", symbol);
    
    auto found = env.lookupSymbol("x");
    EXPECT_NE(found, nullptr);
    EXPECT_EQ(found->getName(), "x");
}

/**
 * اختبار: البحث في نطاقات متعددة / Test: Lookup in multiple scopes
 */
TEST(EnvironmentTest, LookupInMultipleScopes) {
    Environment env;
    
    auto intType = TypeFactory::getIntType();
    auto stringType = TypeFactory::getStringType();
    
    // نطاق عام / Global scope
    env.pushScope(Scope::Type::GLOBAL, "global");
    env.addSymbol("globalVar", 
        std::make_shared<Symbol>("globalVar", intType, Symbol::Kind::VARIABLE));
    
    // نطاق دالة / Function scope
    env.pushScope(Scope::Type::FUNCTION, "main");
    env.addSymbol("localVar", 
        std::make_shared<Symbol>("localVar", stringType, Symbol::Kind::VARIABLE));
    
    // يجب أن نجد كلاهما / Should find both
    EXPECT_NE(env.lookupSymbol("globalVar"), nullptr);
    EXPECT_NE(env.lookupSymbol("localVar"), nullptr);
    
    env.popScope();
    
    // في النطاق العام، لا نجد المتغير المحلي / In global scope, don't find local
    EXPECT_NE(env.lookupSymbol("globalVar"), nullptr);
    EXPECT_EQ(env.lookupSymbol("localVar"), nullptr);
}

/**
 * اختبار: إخفاء المتغيرات / Test: Variable shadowing
 */
TEST(EnvironmentTest, VariableShadowing) {
    Environment env;
    
    auto intType = TypeFactory::getIntType();
    auto stringType = TypeFactory::getStringType();
    
    // نطاق خارجي / Outer scope
    env.pushScope(Scope::Type::GLOBAL, "global");
    env.addSymbol("x", 
        std::make_shared<Symbol>("x", intType, Symbol::Kind::VARIABLE));
    
    // نطاق داخلي مع متغير بنفس الاسم / Inner scope with same name
    env.pushScope(Scope::Type::BLOCK, "block1");
    env.addSymbol("x", 
        std::make_shared<Symbol>("x", stringType, Symbol::Kind::VARIABLE));
    
    // يجب أن نجد النوع الداخلي / Should find inner type
    auto found = env.lookupSymbol("x");
    EXPECT_NE(found, nullptr);
    EXPECT_TRUE(found->getType()->equals(stringType.get()));
    
    env.popScope();
    
    // الآن يجب أن نجد النوع الخارجي / Now should find outer type
    found = env.lookupSymbol("x");
    EXPECT_NE(found, nullptr);
    EXPECT_TRUE(found->getType()->equals(intType.get()));
}

// ============================================================================
// TypeContext Tests - اختبارات TypeContext
// ============================================================================

/**
 * اختبار: إنشاء TypeContext / Test: Create TypeContext
 */
TEST(TypeContextTest, CreateTypeContext) {
    auto context = std::make_shared<TypeContext>();
    
    EXPECT_NE(context->getEnvironment(), nullptr);
}

/**
 * اختبار: إضافة أخطاء وتحذيرات / Test: Add errors and warnings
 */
TEST(TypeContextTest, AddErrorsAndWarnings) {
    auto context = std::make_shared<TypeContext>();
    
    EXPECT_FALSE(context->hasErrors());
    EXPECT_EQ(context->getErrors().size(), 0);
    
    context->addError("خطأ 1");
    context->addError("خطأ 2");
    
    EXPECT_TRUE(context->hasErrors());
    EXPECT_EQ(context->getErrors().size(), 2);
    
    context->addWarning("تحذير 1");
    EXPECT_EQ(context->getWarnings().size(), 1);
}

/**
 * اختبار: إعادة تعيين السياق / Test: Reset context
 */
TEST(TypeContextTest, ResetContext) {
    auto context = std::make_shared<TypeContext>();
    
    context->addError("خطأ");
    context->addWarning("تحذير");
    
    EXPECT_TRUE(context->hasErrors());
    
    context->reset();
    
    EXPECT_FALSE(context->hasErrors());
    EXPECT_EQ(context->getErrors().size(), 0);
    EXPECT_EQ(context->getWarnings().size(), 0);
}

// ============================================================================
// Integration Tests - اختبارات التكامل
// ============================================================================

/**
 * اختبار: سيناريو كامل / Test: Complete scenario
 */
TEST(TypeContextIntegrationTest, CompleteScenario) {
    auto context = std::make_shared<TypeContext>();
    auto env = context->getEnvironment();
    
    auto intType = TypeFactory::getIntType();
    auto stringType = TypeFactory::getStringType();
    auto floatType = TypeFactory::getFloatType();
    
    // نطاق عام / Global scope
    env->pushScope(Scope::Type::GLOBAL, "global");
    env->addSymbol("PI", 
        std::make_shared<Symbol>("PI", floatType, Symbol::Kind::VARIABLE, false));
    
    // نطاق دالة / Function scope
    env->pushScope(Scope::Type::FUNCTION, "calculate");
    env->addSymbol("x", 
        std::make_shared<Symbol>("x", intType, Symbol::Kind::PARAMETER));
    env->addSymbol("y", 
        std::make_shared<Symbol>("y", intType, Symbol::Kind::PARAMETER));
    
    // نطاق كتلة / Block scope
    env->pushScope(Scope::Type::BLOCK, "if_block");
    env->addSymbol("result", 
        std::make_shared<Symbol>("result", stringType, Symbol::Kind::VARIABLE));
    
    // التحقق من البحث / Verify lookups
    EXPECT_NE(env->lookupSymbol("PI"), nullptr);
    EXPECT_NE(env->lookupSymbol("x"), nullptr);
    EXPECT_NE(env->lookupSymbol("y"), nullptr);
    EXPECT_NE(env->lookupSymbol("result"), nullptr);
    
    EXPECT_EQ(env->getCurrentScopeDepth(), 3);
    
    // سحب النطاقات / Pop scopes
    env->popScope(); // block
    EXPECT_EQ(env->lookupSymbol("result"), nullptr);
    
    env->popScope(); // function
    EXPECT_EQ(env->lookupSymbol("x"), nullptr);
    
    env->popScope(); // global
}

/**
 * اختبار: نطاقات متداخلة معقدة / Test: Complex nested scopes
 */
TEST(TypeContextIntegrationTest, ComplexNestedScopes) {
    auto context = std::make_shared<TypeContext>();
    auto env = context->getEnvironment();
    
    auto intType = TypeFactory::getIntType();
    
    // Global → Function → Block → Block → Block
    env->pushScope(Scope::Type::GLOBAL, "global");
    env->addSymbol("level0", 
        std::make_shared<Symbol>("level0", intType, Symbol::Kind::VARIABLE));
    
    env->pushScope(Scope::Type::FUNCTION, "func");
    env->addSymbol("level1", 
        std::make_shared<Symbol>("level1", intType, Symbol::Kind::VARIABLE));
    
    env->pushScope(Scope::Type::BLOCK, "block1");
    env->addSymbol("level2", 
        std::make_shared<Symbol>("level2", intType, Symbol::Kind::VARIABLE));
    
    env->pushScope(Scope::Type::BLOCK, "block2");
    env->addSymbol("level3", 
        std::make_shared<Symbol>("level3", intType, Symbol::Kind::VARIABLE));
    
    env->pushScope(Scope::Type::BLOCK, "block3");
    env->addSymbol("level4", 
        std::make_shared<Symbol>("level4", intType, Symbol::Kind::VARIABLE));
    
    // يجب أن نجد جميع المستويات / Should find all levels
    EXPECT_NE(env->lookupSymbol("level0"), nullptr);
    EXPECT_NE(env->lookupSymbol("level1"), nullptr);
    EXPECT_NE(env->lookupSymbol("level2"), nullptr);
    EXPECT_NE(env->lookupSymbol("level3"), nullptr);
    EXPECT_NE(env->lookupSymbol("level4"), nullptr);
    
    EXPECT_EQ(env->getCurrentScopeDepth(), 5);
}

// ============================================================================
// EOF
// ============================================================================
