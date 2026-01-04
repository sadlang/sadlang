/**
 * @file test_type_narrowing.cpp
 * @brief Type Narrowing System Tests / اختبارات نظام تضييق الأنواع
 * 
 * Phase 1.3.5.2: Type Narrowing Integration Tests
 * 
 * @author Sad Language Team
 * @date 2026-01-03
 */

#include "type_narrowing.h"
#include "type_guard.h"
#include "type_registry.h"
#include "union_type.h"
#include "optional_type.h"
#include <cassert>
#include <iostream>
#include <memory>

using namespace Sad::TypeSystem;

// ════════════════════════════════════════════════════════════════════════════════
// Test Utilities / أدوات الاختبار
// ════════════════════════════════════════════════════════════════════════════════

void printHeader(const std::string& testName) {
    std::cout << "\n╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  " << testName << "\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n\n";
}

void printSuccess(const std::string& msg) {
    std::cout << "✓ نجح (Success): " << msg << "\n";
}

void printTest(const std::string& arabicMsg, const std::string& englishMsg) {
    std::cout << "اختبار (Test): " << arabicMsg << " / " << englishMsg << "\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// Test 1: Context Basic Operations
// ════════════════════════════════════════════════════════════════════════════════

void testContextBasics() {
    printHeader("اختبار (Test): العمليات الأساسية للسياق (Context Basic Operations)");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    
    TypeNarrowingContext context;
    
    // Test: تسجيل متغير / Register variable
    printTest("تسجيل متغير", "Register variable");
    context.registerVariable("value", intType);
    assert(context.hasVariable("value"));
    assert(context.getVariableType("value")->equals(intType.get()));
    printSuccess("تسجيل متغير ناجح");
    
    // Test: Scope management
    printTest("إدارة النطاقات", "Scope management");
    assert(context.getScopeDepth() == 0);
    context.pushScope();
    assert(context.getScopeDepth() == 1);
    context.pushScope();
    assert(context.getScopeDepth() == 2);
    context.popScope();
    assert(context.getScopeDepth() == 1);
    context.popScope();
    assert(context.getScopeDepth() == 0);
    printSuccess("إدارة النطاقات تعمل");
    
    // Test: تضييق متغير / Narrow variable
    printTest("تضييق متغير", "Narrow variable");
    context.pushScope();
    context.narrowVariable("value", strType);
    assert(context.getVariableType("value")->equals(strType.get()));
    assert(context.getOriginalType("value")->equals(intType.get()));
    printSuccess("تضييق متغير ناجح");
    
    std::cout << "\n✓✓✓ نجح الاختبار (Test Passed) ✓✓✓\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// Test 2: Guard Application
// ════════════════════════════════════════════════════════════════════════════════

void testGuardApplication() {
    printHeader("اختبار (Test): تطبيق الحراس (Guard Application)");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    auto boolType = registry.getBooleanType();
    
    // إنشاء Union: Integer | String | Boolean
    auto unionType = std::make_shared<UnionType>(
        std::vector<TypePtr>{intType, strType, boolType}
    );
    
    TypeNarrowingContext context;
    context.registerVariable("value", unionType);
    context.pushScope();
    
    // Test: IsType guard
    printTest("تطبيق IsType guard", "Apply IsType guard");
    auto intGuard = TypeGuard::makeIsTypeGuard("value", intType);
    bool success = context.applyGuard("value", intGuard);
    assert(success);
    
    TypePtr narrowed = context.getVariableType("value");
    std::cout << "النوع بعد التضييق: " << narrowed->toString() << "\n";
    assert(narrowed->equals(intType.get()));
    printSuccess("تطبيق IsType guard ناجح");
    
    // Test: NotNull guard على Optional
    printTest("تطبيق NotNull على Optional", "Apply NotNull on Optional");
    auto optionalInt = std::make_shared<OptionalType>(intType);
    context.registerVariable("optValue", optionalInt);
    
    auto notNullGuard = TypeGuard::makeNotNullGuard("optValue");
    success = context.applyGuard("optValue", notNullGuard);
    assert(success);
    
    narrowed = context.getVariableType("optValue");
    std::cout << "النوع بعد NotNull: " << narrowed->toString() << "\n";
    assert(narrowed->equals(intType.get())); // Optional → Integer
    printSuccess("تطبيق NotNull ناجح");
    
    std::cout << "\n✓✓✓ نجح الاختبار (Test Passed) ✓✓✓\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// Test 3: Else Guard (narrowElse)
// ════════════════════════════════════════════════════════════════════════════════

void testElseGuard() {
    printHeader("اختبار (Test): حارس else (Else Guard)");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    auto boolType = registry.getBooleanType();
    
    // Union: Integer | String | Boolean
    auto unionType = std::make_shared<UnionType>(
        std::vector<TypePtr>{intType, strType, boolType}
    );
    
    TypeNarrowingContext context;
    context.registerVariable("value", unionType);
    context.pushScope();
    
    // Test: applyElseGuard
    printTest("تطبيق else guard", "Apply else guard");
    auto intGuard = TypeGuard::makeIsTypeGuard("value", intType);
    bool success = context.applyElseGuard("value", intGuard);
    assert(success);
    
    TypePtr narrowed = context.getVariableType("value");
    std::cout << "النوع في else: " << narrowed->toString() << "\n";
    // يجب أن يكون String | Boolean (بدون Integer)
    assert(narrowed->getKind() == TypeKind::Union);
    auto unionNarrowed = std::static_pointer_cast<UnionType>(narrowed);
    assert(unionNarrowed->getAlternatives().size() == 2);
    printSuccess("else guard يزيل Integer");
    
    std::cout << "\n✓✓✓ نجح الاختبار (Test Passed) ✓✓✓\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// Test 4: Branch Merging (Same Type)
// ════════════════════════════════════════════════════════════════════════════════

void testBranchMergingSameType() {
    printHeader("اختبار (Test): دمج فروع بنفس النوع (Branch Merging - Same Type)");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    
    // Union: Integer | String
    auto unionType = std::make_shared<UnionType>(
        std::vector<TypePtr>{intType, strType}
    );
    
    // السياق الأساسي / Base context
    TypeNarrowingContext baseContext;
    baseContext.registerVariable("value", unionType);
    
    // Then context: value is Integer
    TypeNarrowingContext thenContext = baseContext;
    thenContext.pushScope();
    auto intGuard = TypeGuard::makeIsTypeGuard("value", intType);
    thenContext.applyGuard("value", intGuard);
    assert(thenContext.getVariableType("value")->equals(intType.get()));
    
    // Else context: value is String (narrowElse)
    TypeNarrowingContext elseContext = baseContext;
    elseContext.pushScope();
    elseContext.applyElseGuard("value", intGuard);
    assert(elseContext.getVariableType("value")->equals(strType.get()));
    
    // Merge: Integer (then) + String (else) → Integer | String
    printTest("دمج Integer + String", "Merge Integer + String");
    TypeNarrowingContext merged;
    merged.registerVariable("value", unionType);
    merged.pushScope();
    merged.mergeBranches(thenContext, elseContext);
    
    TypePtr result = merged.getVariableType("value");
    std::cout << "النوع بعد الدمج: " << result->toString() << "\n";
    assert(result->getKind() == TypeKind::Union);
    printSuccess("دمج الفروع ناجح - Union type");
    
    std::cout << "\n✓✓✓ نجح الاختبار (Test Passed) ✓✓✓\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// Test 5: Branch Merging (Identical)
// ════════════════════════════════════════════════════════════════════════════════

void testBranchMergingIdentical() {
    printHeader("اختبار (Test): دمج فروع متطابقة (Branch Merging - Identical)");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    
    auto unionType = std::make_shared<UnionType>(
        std::vector<TypePtr>{intType, strType}
    );
    
    TypeNarrowingContext baseContext;
    baseContext.registerVariable("value", unionType);
    
    // كلا الفرعين يُضيّقان إلى Integer
    // Both branches narrow to Integer
    TypeNarrowingContext thenContext = baseContext;
    thenContext.pushScope();
    thenContext.narrowVariable("value", intType);
    
    TypeNarrowingContext elseContext = baseContext;
    elseContext.pushScope();
    elseContext.narrowVariable("value", intType);
    
    // Merge: Integer + Integer → Integer
    printTest("دمج نفس النوع", "Merge same type");
    TypeNarrowingContext merged;
    merged.registerVariable("value", unionType);
    merged.pushScope();
    merged.mergeBranches(thenContext, elseContext);
    
    TypePtr result = merged.getVariableType("value");
    std::cout << "النوع بعد الدمج: " << result->toString() << "\n";
    assert(result->equals(intType.get()));
    printSuccess("دمج نفس النوع → نوع واحد");
    
    std::cout << "\n✓✓✓ نجح الاختبار (Test Passed) ✓✓✓\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// Test 6: Single Branch Merge (if without else)
// ════════════════════════════════════════════════════════════════════════════════

void testSingleBranchMerge() {
    printHeader("اختبار (Test): دمج فرع واحد (Single Branch Merge)");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    
    auto unionType = std::make_shared<UnionType>(
        std::vector<TypePtr>{intType, strType}
    );
    
    TypeNarrowingContext baseContext;
    baseContext.registerVariable("value", unionType);
    
    // Then context (no else)
    TypeNarrowingContext thenContext = baseContext;
    thenContext.pushScope();
    auto intGuard = TypeGuard::makeIsTypeGuard("value", intType);
    thenContext.applyGuard("value", intGuard);
    
    // Merge single branch
    printTest("دمج بدون else", "Merge without else");
    TypeNarrowingContext merged;
    merged.registerVariable("value", unionType);
    merged.pushScope();
    merged.mergeSingleBranch(thenContext);
    
    TypePtr result = merged.getVariableType("value");
    std::cout << "النوع بعد الدمج: " << result->toString() << "\n";
    // بدون else، نبقي النوع الأصلي / Without else, keep original
    assert(result->getKind() == TypeKind::Union);
    printSuccess("بدون else → نبقي النوع الأصلي");
    
    std::cout << "\n✓✓✓ نجح الاختبار (Test Passed) ✓✓✓\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// Test 7: Nested Scopes
// ════════════════════════════════════════════════════════════════════════════════

void testNestedScopes() {
    printHeader("اختبار (Test): نطاقات متداخلة (Nested Scopes)");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    auto boolType = registry.getBooleanType();
    
    auto unionType = std::make_shared<UnionType>(
        std::vector<TypePtr>{intType, strType, boolType}
    );
    
    TypeNarrowingContext context;
    context.registerVariable("value", unionType);
    
    // Scope 1: narrow to Integer | String
    printTest("نطاق 1", "Scope 1");
    context.pushScope();
    auto notBoolGuard = TypeGuard::makeNotGuard(
        TypeGuard::makeIsTypeGuard("value", boolType)
    );
    context.applyGuard("value", notBoolGuard);
    
    TypePtr narrowed1 = context.getVariableType("value");
    std::cout << "النطاق 1: " << narrowed1->toString() << "\n";
    assert(narrowed1->getKind() == TypeKind::Union);
    
    // Scope 2: narrow to Integer
    printTest("نطاق 2 (متداخل)", "Scope 2 (nested)");
    context.pushScope();
    auto intGuard = TypeGuard::makeIsTypeGuard("value", intType);
    context.applyGuard("value", intGuard);
    
    TypePtr narrowed2 = context.getVariableType("value");
    std::cout << "النطاق 2: " << narrowed2->toString() << "\n";
    assert(narrowed2->equals(intType.get()));
    printSuccess("النطاق الداخلي يُضيّق أكثر");
    
    // Pop scope 2
    context.popScope();
    TypePtr afterPop = context.getVariableType("value");
    std::cout << "بعد popScope: " << afterPop->toString() << "\n";
    assert(afterPop->getKind() == TypeKind::Union);
    printSuccess("بعد pop يعود للنطاق السابق");
    
    std::cout << "\n✓✓✓ نجح الاختبار (Test Passed) ✓✓✓\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// Test 8: Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

void testHelperFunctions() {
    printHeader("اختبار (Test): الدوال المساعدة (Helper Functions)");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    
    auto unionType = std::make_shared<UnionType>(
        std::vector<TypePtr>{intType, strType}
    );
    
    // Test: canNarrowVariable
    printTest("canNarrowVariable", "canNarrowVariable");
    auto intGuard = TypeGuard::makeIsTypeGuard("value", intType);
    bool canNarrow = canNarrowVariable("value", unionType, intGuard);
    std::cout << "يمكن تضييق: " << (canNarrow ? "نعم" : "لا") << "\n";
    assert(canNarrow);
    printSuccess("canNarrowVariable يعمل");
    
    // Test: applyNarrowingToVariable
    printTest("applyNarrowingToVariable", "applyNarrowingToVariable");
    TypeNarrowingContext context;
    context.registerVariable("value", unionType);
    context.pushScope();
    
    TypePtr narrowed = applyNarrowingToVariable(context, "value", intGuard);
    std::cout << "النوع المضيّق: " << narrowed->toString() << "\n";
    assert(narrowed->equals(intType.get()));
    printSuccess("applyNarrowingToVariable يعمل");
    
    // Test: createUnionType
    printTest("createUnionType", "createUnionType");
    TypePtr unionCreated = TypeNarrowingAnalyzer::createUnionType(intType, strType);
    std::cout << "Union مُنشأ: " << unionCreated->toString() << "\n";
    assert(unionCreated->getKind() == TypeKind::Union);
    printSuccess("createUnionType يعمل");
    
    std::cout << "\n✓✓✓ نجح الاختبار (Test Passed) ✓✓✓\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// Test 9: Real-world Scenario (Nullable Value)
// ════════════════════════════════════════════════════════════════════════════════

void testRealWorldScenario() {
    printHeader("اختبار (Test): سيناريو حقيقي (Real-world Scenario)");
    
    std::cout << "مثال: دالة معالج(قيمة: نص?)\n";
    std::cout << "Example: function handler(value: String?)\n\n";
    
    auto& registry = TypeRegistry::getInstance();
    auto strType = registry.getStringType();
    auto optionalStr = std::make_shared<OptionalType>(strType);
    
    TypeNarrowingContext baseContext;
    baseContext.registerVariable("value", optionalStr);
    
    std::cout << "النوع الأصلي: " << optionalStr->toString() << "\n";
    
    // if (value != null) branch
    printTest("فرع if (value != null)", "if (value != null) branch");
    TypeNarrowingContext thenContext = baseContext;
    thenContext.pushScope();
    auto notNullGuard = TypeGuard::makeNotNullGuard("value");
    thenContext.applyGuard("value", notNullGuard);
    
    TypePtr thenType = thenContext.getVariableType("value");
    std::cout << "في then: value هو " << thenType->toString() << "\n";
    assert(thenType->equals(strType.get())); // String (not String?)
    printSuccess("في then → String (آمن)");
    
    // else branch
    printTest("فرع else", "else branch");
    TypeNarrowingContext elseContext = baseContext;
    elseContext.pushScope();
    elseContext.applyElseGuard("value", notNullGuard);
    
    TypePtr elseType = elseContext.getVariableType("value");
    std::cout << "في else: value هو " << elseType->toString() << "\n";
    assert(elseType->getKind() == TypeKind::Void); // null
    printSuccess("في else → Void (null)");
    
    std::cout << "\nمثال على الكود:\n";
    std::cout << "دالة معالج(قيمة: نص?) {\n";
    std::cout << "    إذا (قيمة != عدم) {\n";
    std::cout << "        # هنا: قيمة: نص (مضمون ليس null)\n";
    std::cout << "        اطبع(قيمة.طول)  # آمن!\n";
    std::cout << "    } وإلا {\n";
    std::cout << "        # هنا: قيمة: عدم\n";
    std::cout << "        اطبع(\"القيمة null\")\n";
    std::cout << "    }\n";
    std::cout << "}\n";
    
    std::cout << "\n✓✓✓ نجح الاختبار (Test Passed) ✓✓✓\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// Main Test Runner
// ════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "\n╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║    اختبارات Type Narrowing (حراس الأنواع)                    ║\n";
    std::cout << "║                                                                ║\n";
    std::cout << "║    Type Narrowing Tests                                        ║\n";
    std::cout << "║    Phase 1.3.5.2                                               ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
    
    try {
        testContextBasics();
        testGuardApplication();
        testElseGuard();
        testBranchMergingSameType();
        testBranchMergingIdentical();
        testSingleBranchMerge();
        testNestedScopes();
        testHelperFunctions();
        testRealWorldScenario();
        
        std::cout << "\n╔════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║    ✓✓✓ جميع الاختبارات نجحت! (All Tests Passed!) ✓✓✓        ║\n";
        std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ فشل الاختبار (Test Failed): " << e.what() << "\n";
        return 1;
    }
}
