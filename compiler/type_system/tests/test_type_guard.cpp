// ════════════════════════════════════════════════════════════════════════════════
// ملف: test_type_guard.cpp
// File: test_type_guard.cpp
//
// الوصف: اختبارات Type Guards
// Description: Type Guards tests
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// Phase: 1.3.5.1 - Type Guards
// ════════════════════════════════════════════════════════════════════════════════

#include "type_guard.h"
#include "union_type.h"
#include "optional_type.h"
#include "primitive_type.h"
#include "type_registry.h"
#include <cassert>
#include <iostream>
#include <memory>

using namespace Sad::TypeSystem;

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة للاختبار / Test Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

void printTestHeader(const std::string& testName) {
    std::cout << "\n═══════════════════════════════════════════════════════\n";
    std::cout << "اختبار (Test): " << testName << "\n";
    std::cout << "═══════════════════════════════════════════════════════\n";
}

void printSuccess(const std::string& message) {
    std::cout << "✓ نجح (Success): " << message << "\n";
}

void printResult(const std::string& label, TypePtr type) {
    std::cout << label << ": " << (type ? type->toString() : "null") << "\n";
}

void printGuard(const std::string& label, TypeGuardPtr guard) {
    std::cout << label << ": " << guard->toString() << "\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// الاختبار 1: إنشاء حراس بسيطة
// Test 1: Create Simple Guards
// ════════════════════════════════════════════════════════════════════════════════

void testSimpleGuardCreation() {
    printTestHeader("إنشاء حراس بسيطة (Simple Guard Creation)");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    auto boolType = registry.getBooleanType();
    
    // إنشاء IsType guard
    auto guard1 = TypeGuard::makeIsTypeGuard("قيمة", intType);
    printGuard("حارس IsType", guard1);
    assert(guard1->getKind() == TypeGuardKind::IsType);
    assert(guard1->getVariableName() == "قيمة");
    assert(guard1->getTargetType()->equals(intType.get()));
    printSuccess("إنشاء IsType guard");
    
    // إنشاء TypeOf guard
    auto guard2 = TypeGuard::makeTypeOfGuard("متغير", "نص");
    printGuard("حارس TypeOf", guard2);
    assert(guard2->getKind() == TypeGuardKind::TypeOf);
    printSuccess("إنشاء TypeOf guard");
    
    // إنشاء Null guard
    auto guard3 = TypeGuard::makeNullGuard("عنصر");
    printGuard("حارس Null", guard3);
    assert(guard3->getKind() == TypeGuardKind::Null);
    printSuccess("إنشاء Null guard");
    
    // إنشاء NotNull guard
    auto guard4 = TypeGuard::makeNotNullGuard("عنصر");
    printGuard("حارس NotNull", guard4);
    assert(guard4->getKind() == TypeGuardKind::NotNull);
    printSuccess("إنشاء NotNull guard");
    
    std::cout << "\n✓✓✓ نجح الاختبار (Test Passed) ✓✓✓\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// الاختبار 2: مطابقة الأنواع البسيطة
// Test 2: Matching Simple Types
// ════════════════════════════════════════════════════════════════════════════════

void testSimpleTypeMatching() {
    printTestHeader("مطابقة الأنواع البسيطة (Simple Type Matching)");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    auto boolType = registry.getBooleanType();
    
    // IsType guard
    auto intGuard = TypeGuard::makeIsTypeGuard("قيمة", intType);
    printGuard("حارس رقم", intGuard);
    
    // يجب أن يطابق Integer
    assert(intGuard->matches(intType));
    printSuccess("يطابق Integer");
    
    // لا يجب أن يطابق String
    assert(!intGuard->matches(strType));
    printSuccess("لا يطابق String");
    
    // TypeOf guard
    auto typeofGuard = TypeGuard::makeTypeOfGuard("متغير", "نص");
    printGuard("حارس typeof نص", typeofGuard);
    
    assert(typeofGuard->matches(strType));
    printSuccess("typeof يطابق String");
    
    assert(!typeofGuard->matches(intType));
    printSuccess("typeof لا يطابق Integer");
    
    std::cout << "\n✓✓✓ نجح الاختبار (Test Passed) ✓✓✓\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// الاختبار 3: تضييق Union Types
// Test 3: Narrowing Union Types
// ════════════════════════════════════════════════════════════════════════════════

void testUnionTypeNarrowing() {
    printTestHeader("تضييق Union Types");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    auto boolType = registry.getBooleanType();
    
    // إنشاء Union: رقم | نص | منطقي
    auto unionType = std::make_shared<UnionType>(TypeList{intType, strType, boolType});
    printResult("النوع الأصلي", unionType);
    
    // تضييق إلى رقم
    auto intGuard = TypeGuard::makeIsTypeGuard("قيمة", intType);
    printGuard("الحارس", intGuard);
    
    auto narrowed1 = intGuard->narrow(unionType);
    printResult("بعد التضييق", narrowed1);
    assert(narrowed1->equals(intType.get()));
    printSuccess("تضييق Union إلى Integer");
    
    // تضييق إلى نص
    auto strGuard = TypeGuard::makeIsTypeGuard("قيمة", strType);
    auto narrowed2 = strGuard->narrow(unionType);
    printResult("بعد التضييق", narrowed2);
    assert(narrowed2->equals(strType.get()));
    printSuccess("تضييق Union إلى String");
    
    // narrowElse: استبعاد Integer يترك String | Boolean
    auto elseType = intGuard->narrowElse(unionType);
    printResult("else branch", elseType);
    assert(elseType->getKind() == TypeKind::Union);
    auto elseUnion = std::static_pointer_cast<UnionType>(elseType);
    assert(elseUnion->getAlternatives().size() == 2);
    printSuccess("narrowElse يستبعد النوع المطابق");
    
    std::cout << "\n✓✓✓ نجح الاختبار (Test Passed) ✓✓✓\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// الاختبار 4: Optional Types والحراس
// Test 4: Optional Types and Guards
// ════════════════════════════════════════════════════════════════════════════════

void testOptionalTypeGuards() {
    printTestHeader("Optional Types والحراس");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto voidType = registry.getVoidType();
    
    // إنشاء Optional: رقم?
    auto optionalInt = makeOptionalType(intType);
    printResult("النوع الأصلي", optionalInt);
    
    // حارس NotNull يزيل null
    auto notNullGuard = TypeGuard::makeNotNullGuard("قيمة");
    printGuard("حارس NotNull", notNullGuard);
    
    auto narrowed = notNullGuard->narrow(optionalInt);
    printResult("بعد NotNull", narrowed);
    assert(narrowed->equals(intType.get()));
    printSuccess("NotNull يضيّق Optional<T> إلى T");
    
    // حارس Null
    auto nullGuard = TypeGuard::makeNullGuard("قيمة");
    printGuard("حارس Null", nullGuard);
    
    auto narrowed2 = nullGuard->narrow(optionalInt);
    printResult("بعد Null", narrowed2);
    assert(narrowed2->equals(voidType.get()));
    printSuccess("Null يضيّق Optional<T> إلى Void");
    
    // else branch للـ NotNull يعطي null
    auto elseType = notNullGuard->narrowElse(optionalInt);
    printResult("NotNull else", elseType);
    assert(elseType->equals(voidType.get()));
    printSuccess("NotNull else يعطي Void");
    
    std::cout << "\n✓✓✓ نجح الاختبار (Test Passed) ✓✓✓\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// الاختبار 5: حراس مركبة (And, Or, Not)
// Test 5: Compound Guards (And, Or, Not)
// ════════════════════════════════════════════════════════════════════════════════

void testCompoundGuards() {
    printTestHeader("حراس مركبة (Compound Guards)");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    auto boolType = registry.getBooleanType();
    
    // Union: رقم | نص | منطقي
    auto unionType = std::make_shared<UnionType>(TypeList{intType, strType, boolType});
    printResult("النوع الأصلي", unionType);
    
    // حارس And: القيمة نوع رقم و القيمة != عدم
    auto intGuard = TypeGuard::makeIsTypeGuard("قيمة", intType);
    auto notNullGuard = TypeGuard::makeNotNullGuard("قيمة");
    auto andGuard = TypeGuard::makeAndGuard({intGuard, notNullGuard});
    printGuard("حارس And", andGuard);
    
    auto result1 = andGuard->apply(unionType);
    printResult("بعد And", result1->getNarrowedType());
    assert(result1->isSuccess());
    assert(result1->getNarrowedType()->equals(intType.get()));
    printSuccess("And guard نجح");
    
    // حارس Or: القيمة نوع رقم أو القيمة نوع نص
    auto strGuard = TypeGuard::makeIsTypeGuard("قيمة", strType);
    auto orGuard = TypeGuard::makeOrGuard({intGuard, strGuard});
    printGuard("حارس Or", orGuard);
    
    auto result2 = orGuard->apply(unionType);
    printResult("بعد Or", result2->getNarrowedType());
    assert(result2->isSuccess());
    auto orResult = result2->getNarrowedType();
    assert(orResult->getKind() == TypeKind::Union);
    auto orUnion = std::static_pointer_cast<UnionType>(orResult);
    assert(orUnion->getAlternatives().size() == 2);
    printSuccess("Or guard نجح (Integer | String)");
    
    // حارس Not: ليس (القيمة نوع رقم)
    auto notGuard = TypeGuard::makeNotGuard(intGuard);
    printGuard("حارس Not", notGuard);
    
    auto result3 = notGuard->apply(unionType);
    printResult("بعد Not", result3->getNarrowedType());
    assert(result3->isSuccess());
    auto notResult = result3->getNarrowedType();
    assert(notResult->getKind() == TypeKind::Union);
    auto notUnion = std::static_pointer_cast<UnionType>(notResult);
    assert(notUnion->getAlternatives().size() == 2); // String | Boolean
    printSuccess("Not guard نجح (String | Boolean)");
    
    std::cout << "\n✓✓✓ نجح الاختبار (Test Passed) ✓✓✓\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// الاختبار 6: حراس معرّفة من المستخدم
// Test 6: User-Defined Guards
// ════════════════════════════════════════════════════════════════════════════════

void testUserDefinedGuards() {
    printTestHeader("حراس معرّفة من المستخدم (User-Defined Guards)");
    
    auto& registry = TypeRegistry::getInstance();
    auto strType = registry.getStringType();
    auto anyType = registry.getAnyType();
    
    // دالة: هو_نص(القيمة: أي) -> القيمة نوع نص
    auto userGuard = TypeGuard::makeUserDefinedGuard("هو_نص", "قيمة", strType);
    printGuard("حارس معرّف", userGuard);
    
    assert(userGuard->getKind() == TypeGuardKind::UserDefined);
    assert(userGuard->getVariableName() == "قيمة");
    assert(userGuard->getTargetType()->equals(strType.get()));
    printSuccess("إنشاء حارس معرّف من المستخدم");
    
    // تطبيق على Any type
    auto result = userGuard->apply(anyType);
    printResult("بعد التطبيق", result->getNarrowedType());
    assert(result->isSuccess());
    assert(result->getNarrowedType()->equals(strType.get()));
    printSuccess("تطبيق حارس معرّف ينجح");
    
    std::cout << "\n✓✓✓ نجح الاختبار (Test Passed) ✓✓✓\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// الاختبار 7: TypeGuardResult
// Test 7: TypeGuardResult
// ════════════════════════════════════════════════════════════════════════════════

void testTypeGuardResult() {
    printTestHeader("TypeGuardResult");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    
    // نتيجة ناجحة
    auto successResult = std::make_shared<TypeGuardResult>(true, intType);
    std::cout << "نتيجة ناجحة: " << successResult->toString() << "\n";
    assert(successResult->isSuccess());
    assert(successResult->getNarrowedType()->equals(intType.get()));
    assert(successResult->getReason().empty());
    printSuccess("نتيجة ناجحة");
    
    // نتيجة فاشلة
    auto failResult = std::make_shared<TypeGuardResult>(false, nullptr, "النوع لا يطابق");
    std::cout << "نتيجة فاشلة: " << failResult->toString() << "\n";
    assert(!failResult->isSuccess());
    assert(failResult->getNarrowedType() == nullptr);
    assert(failResult->getReason() == "النوع لا يطابق");
    printSuccess("نتيجة فاشلة");
    
    std::cout << "\n✓✓✓ نجح الاختبار (Test Passed) ✓✓✓\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// الاختبار 8: دوال مساعدة عامة
// Test 8: Global Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

void testGlobalHelpers() {
    printTestHeader("دوال مساعدة عامة (Global Helper Functions)");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    
    auto unionType = std::make_shared<UnionType>(TypeList{intType, strType});
    auto intGuard = TypeGuard::makeIsTypeGuard("قيمة", intType);
    
    // canNarrow
    bool canNarrowUnion = canNarrow(unionType, intGuard);
    assert(canNarrowUnion);
    printSuccess("canNarrow مع Union");
    
    bool canNarrowInt = canNarrow(intType, intGuard);
    assert(canNarrowInt);
    printSuccess("canNarrow مع Integer");
    
    // combineGuards
    auto strGuard = TypeGuard::makeIsTypeGuard("قيمة", strType);
    auto combined = combineGuards(intGuard, strGuard);
    printGuard("حارس مدمج", combined);
    assert(combined->getKind() == TypeGuardKind::And);
    printSuccess("combineGuards");
    
    // invertGuard
    auto inverted = invertGuard(intGuard);
    printGuard("حارس معكوس", inverted);
    assert(inverted->getKind() == TypeGuardKind::Not);
    printSuccess("invertGuard");
    
    // applyGuards
    auto guards = std::vector<TypeGuardPtr>{intGuard};
    auto result = applyGuards(unionType, guards);
    printResult("بعد applyGuards", result);
    assert(result->equals(intType.get()));
    printSuccess("applyGuards");
    
    std::cout << "\n✓✓✓ نجح الاختبار (Test Passed) ✓✓✓\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// الاختبار 9: سيناريو واقعي - Result Type
// Test 9: Practical Scenario - Result Type
// ════════════════════════════════════════════════════════════════════════════════

void testPracticalResultType() {
    printTestHeader("سيناريو واقعي - Result Type");
    
    auto& registry = TypeRegistry::getInstance();
    auto intType = registry.getIntegerType();
    auto strType = registry.getStringType();
    
    // Result<Integer, String> = Integer | String
    auto resultType = std::make_shared<UnionType>(TypeList{intType, strType});
    printResult("Result<رقم, نص>", resultType);
    
    // فحص النجاح: إذا (النتيجة نوع رقم)
    auto successGuard = TypeGuard::makeIsTypeGuard("نتيجة", intType);
    printGuard("حارس النجاح", successGuard);
    
    auto successBranch = successGuard->narrow(resultType);
    printResult("فرع النجاح", successBranch);
    assert(successBranch->equals(intType.get()));
    printSuccess("فرع النجاح = Integer");
    
    // فرع الفشل: else
    auto failureBranch = successGuard->narrowElse(resultType);
    printResult("فرع الفشل", failureBranch);
    assert(failureBranch->equals(strType.get()));
    printSuccess("فرع الفشل = String");
    
    std::cout << "\n✓✓✓ نجح الاختبار (Test Passed) ✓✓✓\n";
}

// ════════════════════════════════════════════════════════════════════════════════
// الاختبار 10: سيناريو واقعي - Nullable Value
// Test 10: Practical Scenario - Nullable Value
// ════════════════════════════════════════════════════════════════════════════════

void testPracticalNullableValue() {
    printTestHeader("سيناريو واقعي - Nullable Value");
    
    auto& registry = TypeRegistry::getInstance();
    auto strType = registry.getStringType();
    auto voidType = registry.getVoidType();
    
    // String? = String | null
    auto nullableString = makeOptionalType(strType);
    printResult("String?", nullableString);
    
    // فحص: إذا (القيمة != عدم)
    auto notNullGuard = TypeGuard::makeNotNullGuard("قيمة");
    printGuard("حارس != عدم", notNullGuard);
    
    auto valueBranch = notNullGuard->narrow(nullableString);
    printResult("فرع القيمة", valueBranch);
    assert(valueBranch->equals(strType.get()));
    printSuccess("فرع != عدم = String");
    
    // فرع null: else
    auto nullBranch = notNullGuard->narrowElse(nullableString);
    printResult("فرع null", nullBranch);
    assert(nullBranch->equals(voidType.get()));
    printSuccess("فرع else = null");
    
    // استخدام القيمة بعد التضييق
    std::cout << "\nمثال كود:\n";
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
// الدالة الرئيسية / Main Function
// ════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "\n";
    std::cout << "════════════════════════════════════════════════════════════════\n";
    std::cout << "   اختبارات Type Guards (حراس الأنواع)                         \n";
    std::cout << "   Type Guards Tests                                          \n";
    std::cout << "   Phase 1.3.5.1                                              \n";
    std::cout << "════════════════════════════════════════════════════════════════\n";
    
    try {
        testSimpleGuardCreation();
        testSimpleTypeMatching();
        testUnionTypeNarrowing();
        testOptionalTypeGuards();
        testCompoundGuards();
        testUserDefinedGuards();
        testTypeGuardResult();
        testGlobalHelpers();
        testPracticalResultType();
        testPracticalNullableValue();
        
        std::cout << "\n";
        std::cout << "════════════════════════════════════════════════════════════════\n";
        std::cout << "   ✓✓✓ جميع الاختبارات نجحت! (All Tests Passed!) ✓✓✓        \n";
        std::cout << "════════════════════════════════════════════════════════════════\n";
        std::cout << "\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗✗✗ خطأ (Error): " << e.what() << " ✗✗✗\n";
        return 1;
    }
}
