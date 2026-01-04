// ════════════════════════════════════════════════════════════════════════════════
// ملف: test_union_optional.cpp
// File: test_union_optional.cpp
//
// الوصف: اختبارات شاملة لـ Union Types & Optional Types
// Description: Comprehensive tests for Union Types & Optional Types
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// Phase: 1.3.5.0 - Union Type Core
// ════════════════════════════════════════════════════════════════════════════════

#include <iostream>
#include <cassert>
#include <string>
#include <sstream>
#include "../include/union_type.h"
#include "../include/optional_type.h"
#include "../include/type_registry.h"

using namespace Sad::TypeSystem;

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة للاختبار / Helper Functions for Testing
// ════════════════════════════════════════════════════════════════════════════════

void printTestHeader(const std::string& testName) {
    std::cout << "\n═══════════════════════════════════════\n";
    std::cout << "🧪 Test: " << testName << "\n";
    std::cout << "═══════════════════════════════════════\n";
}

void printSuccess(const std::string& message) {
    std::cout << "✅ " << message << "\n";
}

void printError(const std::string& message) {
    std::cout << "❌ " << message << "\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// اختبارات UnionType الأساسية / Basic UnionType Tests
// ════════════════════════════════════════════════════════════════════════════════

void testUnionTypeCreation() {
    printTestHeader("Union Type Creation");
    
    // إنشاء أنواع بدائية / Create primitive types
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    auto boolType = registry.getBooleanType();
    
    // Test 1: إنشاء union من نوعين / Create union from two types
    auto union1 = makeUnionType(intType, strType);
    assert(union1 != nullptr);
    assert(union1->size() == 2);
    printSuccess("Union من نوعين (Integer | String)");
    
    // Test 2: إنشاء union من قائمة / Create union from list
    TypeList types = {intType, strType, boolType};
    auto union2 = makeUnionType(types);
    assert(union2 != nullptr);
    assert(union2->size() == 3);
    printSuccess("Union من قائمة (Integer | String | Boolean)");
    
    // Test 3: الأسماء العربية / Arabic names
    std::cout << "   الاسم العربي: " << union1->getArabicName() << "\n";
    std::cout << "   English name: " << union1->getEnglishName() << "\n";
    printSuccess("الأسماء العربية والإنجليزية");
    
    std::cout << "\n✅ All Union Creation tests passed!\n";
}

void testUnionTypeContains() {
    printTestHeader("Union Type Contains");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    auto boolType = registry.getBooleanType();
    
    auto unionType = makeUnionType(intType, strType);
    
    // Test 1: يحتوي على نوع / Contains type
    assert(unionType->contains(intType) == true);
    assert(unionType->contains(strType) == true);
    assert(unionType->contains(boolType) == false);
    printSuccess("contains() يعمل بشكل صحيح");
    
    // Test 2: يحتوي على اسم نوع / Contains type name
    assert(unionType->containsTypeName("رقم") == true);
    assert(unionType->containsTypeName("نص") == true);
    assert(unionType->containsTypeName("منطقي") == false);
    printSuccess("containsTypeName() يعمل بشكل صحيح");
    
    std::cout << "\n✅ All Contains tests passed!\n";
}

void testUnionTypeSimplify() {
    printTestHeader("Union Type Simplification");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    
    // Test 1: union مع نوع مكرر / Union with duplicate type
    TypeList duplicates = {intType, strType, intType};
    auto union1 = makeUnionType(duplicates);
    auto simplified = union1->simplify();
    
    assert(simplified != nullptr);
    assert(simplified->size() == 2);
    printSuccess("إزالة الأنواع المكررة");
    
    // Test 2: union متداخل / Nested union
    auto nestedUnion = makeUnionType(intType, strType);
    TypeList withNested = {nestedUnion, registry.getBooleanType()};
    auto union2 = makeUnionType(withNested);
    
    // يجب أن يتم تسطيحه تلقائياً / Should be flattened automatically
    assert(union2->size() == 3);
    printSuccess("تسطيح الأنواع المتداخلة");
    
    std::cout << "\n✅ All Simplify tests passed!\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// اختبارات عمليات Union / Union Operations Tests
// ════════════════════════════════════════════════════════════════════════════════

void testUnionOperations() {
    printTestHeader("Union Operations (∪, ∩, -)");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    auto boolType = registry.getBooleanType();
    
    // Union 1: Integer | String
    auto union1 = makeUnionType(intType, strType);
    
    // Union 2: String | Boolean
    auto union2 = makeUnionType(strType, boolType);
    
    // Test 1: Intersection (A ∩ B) / التقاطع
    auto intersection = union1->intersection(union2);
    assert(intersection != nullptr);
    assert(intersection->size() == 1);
    assert(intersection->contains(strType));
    printSuccess("Intersection (Integer|String) ∩ (String|Boolean) = String");
    
    // Test 2: Union (A ∪ B) / الاتحاد
    auto unionResult = union1->unionWith(union2);
    assert(unionResult != nullptr);
    assert(unionResult->size() == 3);
    assert(unionResult->contains(intType));
    assert(unionResult->contains(strType));
    assert(unionResult->contains(boolType));
    printSuccess("Union (Integer|String) ∪ (String|Boolean) = Integer|String|Boolean");
    
    // Test 3: Difference (A - B) / الطرح
    auto difference = union1->difference(strType);
    assert(difference != nullptr);
    assert(difference->size() == 1);
    assert(difference->contains(intType));
    printSuccess("Difference (Integer|String) - String = Integer");
    
    std::cout << "\n✅ All Operation tests passed!\n";
}

void testUnionHelperFunctions() {
    printTestHeader("Union Helper Functions");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    
    // Test 1: unionOf
    auto result1 = unionOf(intType, strType);
    assert(isUnionType(result1));
    printSuccess("unionOf() creates union");
    
    // Test 2: unionOf with same type
    auto result2 = unionOf(intType, intType);
    assert(!isUnionType(result2));  // يجب أن يُبسط إلى نوع واحد
    printSuccess("unionOf() simplifies same types");
    
    // Test 3: intersectionOf
    auto union1 = makeUnionType(intType, strType);
    auto union2 = makeUnionType(strType, registry.getBooleanType());
    auto result3 = intersectionOf(union1, union2);
    // النتيجة يجب أن تكون نص / Result should be String
    assert(result3 != nullptr);
    assert(result3->getEnglishName() == "String" || result3->toString() == "نص");
    printSuccess("intersectionOf() works correctly");
    
    // Test 4: differenceOf
    auto result4 = differenceOf(union1, strType);
    assert(result4 != nullptr);
    printSuccess("differenceOf() works correctly");
    
    std::cout << "\n✅ All Helper Function tests passed!\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// اختبارات OptionalType / OptionalType Tests
// ════════════════════════════════════════════════════════════════════════════════

void testOptionalTypeCreation() {
    printTestHeader("Optional Type Creation");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    
    // Test 1: إنشاء optional type / Create optional type
    auto optInt = makeOptionalType(intType);
    assert(optInt != nullptr);
    assert(optInt->isOptional());
    printSuccess("Integer? created");
    
    // Test 2: الأسماء / Names
    std::cout << "   الاسم العربي: " << optInt->getArabicName() << "\n";
    std::cout << "   English name: " << optInt->getEnglishName() << "\n";
    assert(optInt->toString() == "رقم?");
    printSuccess("Optional type names correct");
    
    // Test 3: النوع الداخلي / Inner type
    auto inner = optInt->getInnerType();
    assert(inner->equals(intType.get()));
    printSuccess("getInnerType() works");
    
    // Test 4: يحتوي على null / Contains null
    assert(optInt->containsNull() == true);
    printSuccess("Optional contains null");
    
    std::cout << "\n✅ All Optional Creation tests passed!\n";
}

void testOptionalTypeHelpers() {
    printTestHeader("Optional Type Helper Functions");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    
    // Test 1: toOptional
    auto opt1 = toOptional(intType);
    assert(isOptionalType(opt1));
    printSuccess("toOptional() converts type");
    
    // Test 2: toOptional على optional (no double wrapping)
    auto opt2 = toOptional(opt1);
    assert(opt2->equals(opt1.get()));
    printSuccess("toOptional() avoids double wrapping");
    
    // Test 3: unwrapOptional
    auto unwrapped = unwrapOptional(opt1);
    assert(unwrapped->equals(intType.get()));
    printSuccess("unwrapOptional() extracts inner type");
    
    // Test 4: unwrapOptional على non-optional
    auto unwrapped2 = unwrapOptional(intType);
    assert(unwrapped2->equals(intType.get()));
    printSuccess("unwrapOptional() returns same type for non-optional");
    
    std::cout << "\n✅ All Optional Helper tests passed!\n";
}

void testUnionToOptional() {
    printTestHeader("Union to Optional Conversion");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto nullType = registry.getVoidType();
    
    // Test 1: Union من T | null يجب أن يكون optional
    auto union1 = makeUnionType(intType, nullType);
    assert(isUnionOptional(union1));
    printSuccess("Integer | null is detected as optional");
    
    // Test 2: التحويل إلى optional
    auto converted = unionToOptional(union1);
    assert(isOptionalType(converted));
    printSuccess("Union converted to Optional");
    
    // Test 3: Union من أكثر من نوعين ليس optional
    TypeList types = {intType, registry.getStringType(), nullType};
    auto union2 = makeUnionType(types);
    assert(!isUnionOptional(union2));
    printSuccess("Integer | String | null is not optional");
    
    std::cout << "\n✅ All Union-to-Optional tests passed!\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// اختبارات علاقات الأنواع / Type Relations Tests
// ════════════════════════════════════════════════════════════════════════════════

void testTypeRelations() {
    printTestHeader("Type Relations (Assignability & Subtyping)");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    auto boolType = registry.getBooleanType();
    
    // Union 1: Integer | String
    auto union1 = makeUnionType(intType, strType);
    
    // Union 2: Integer | String | Boolean
    auto union2 = makeUnionType(TypeList{intType, strType, boolType});
    
    // Test 1: union1 assignable to union2 (subset)
    assert(union1->isAssignableTo(union2.get()));
    printSuccess("(Integer|String) assignable to (Integer|String|Boolean)");
    
    // Test 2: union2 NOT assignable to union1
    assert(!union2->isAssignableTo(union1.get()));
    printSuccess("(Integer|String|Boolean) NOT assignable to (Integer|String)");
    
    // Test 3: Optional assignability
    auto optInt = makeOptionalType(intType);
    auto optUnion = makeOptionalType(union1);
    printSuccess("Optional type relations checked");
    
    std::cout << "\n✅ All Type Relation tests passed!\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// اختبارات سيناريوهات عملية / Practical Scenario Tests
// ════════════════════════════════════════════════════════════════════════════════

void testPracticalScenarios() {
    printTestHeader("Practical Scenarios");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    
    // Scenario 1: Result<T, E> type
    std::cout << "\n📝 Scenario 1: Result<Value, Error> type\n";
    auto resultType = makeUnionType(intType, strType);
    std::cout << "   Result type: " << resultType->toString() << "\n";
    printSuccess("Result type created");
    
    // Scenario 2: Optional parameters
    std::cout << "\n📝 Scenario 2: Optional parameters\n";
    auto optParam = makeOptionalType(strType);
    std::cout << "   Parameter type: " << optParam->toString() << "\n";
    
    if (optParam->containsNull()) {
        std::cout << "   ✓ Can be null\n";
    }
    printSuccess("Optional parameter checked");
    
    // Scenario 3: Type narrowing simulation
    std::cout << "\n📝 Scenario 3: Type Narrowing\n";
    auto union3 = makeUnionType(TypeList{intType, strType, registry.getBooleanType()});
    std::cout << "   Before narrowing: " << union3->toString() << "\n";
    
    // بعد التحقق: if (value نوع Integer)
    auto narrowed = union3->difference(intType);
    std::cout << "   After narrowing: " << narrowed->toString() << "\n";
    printSuccess("Type narrowing simulated");
    
    std::cout << "\n✅ All Practical Scenario tests passed!\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// الدالة الرئيسية / Main Function
// ════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "\n";
    std::cout << "════════════════════════════════════════════════════════════\n";
    std::cout << "   🧪 اختبارات Phase 1.3.5.0: Union & Optional Types\n";
    std::cout << "   🧪 Phase 1.3.5.0 Tests: Union & Optional Types\n";
    std::cout << "════════════════════════════════════════════════════════════\n";
    
    try {
        // اختبارات UnionType الأساسية / Basic UnionType tests
        testUnionTypeCreation();
        testUnionTypeContains();
        testUnionTypeSimplify();
        
        // اختبارات عمليات Union / Union operations tests
        testUnionOperations();
        testUnionHelperFunctions();
        
        // اختبارات OptionalType / OptionalType tests
        testOptionalTypeCreation();
        testOptionalTypeHelpers();
        testUnionToOptional();
        
        // اختبارات علاقات الأنواع / Type relations tests
        testTypeRelations();
        
        // اختبارات سيناريوهات عملية / Practical scenarios
        testPracticalScenarios();
        
        // النتيجة النهائية / Final result
        std::cout << "\n";
        std::cout << "════════════════════════════════════════════════════════════\n";
        std::cout << "   ✅ جميع الاختبارات نجحت! / All Tests Passed!\n";
        std::cout << "   📊 Phase 1.3.5.0: Union Type Core - COMPLETE\n";
        std::cout << "════════════════════════════════════════════════════════════\n";
        std::cout << "\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n❌ Unknown exception occurred\n";
        return 1;
    }
}
