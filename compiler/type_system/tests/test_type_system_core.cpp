// ════════════════════════════════════════════════════════════════════════════════
// ملف: test_type_system_core.cpp
// File: test_type_system_core.cpp
//
// الوصف: اختبار Phase 1.3.0 - نواة نظام الأنواع
// Description: Test for Phase 1.3.0 - Type System Core
//
// المؤلف: Sad Language Type System Tests
// Author: Sad Language Type System Tests
//
// التاريخ: 2 يناير 2026
// Date: January 2, 2026
// ════════════════════════════════════════════════════════════════════════════════

#include <iostream>   // لـ std::cout / For std::cout
#include <string>     // لـ std::string / For std::string
#include <cassert>    // لـ assert / For assert

// استيراد نظام الأنواع / Import type system
#include "../include/type.h"
#include "../include/primitive_type.h"
#include "../include/type_registry.h"

using namespace Sad::TypeSystem;

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة للاختبار / Test Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

// طباعة نتيجة اختبار / Print test result
void printTestResult(const std::string& testName, bool passed) {
    // طباعة الاسم / Print name
    std::cout << "[" << (passed ? "✅ نجح" : "❌ فشل") << "] " << testName << std::endl;
    
    // إذا فشل، أوقف / If failed, abort
    if (!passed) {
        std::cerr << "خطأ: الاختبار فشل / Error: Test failed" << std::endl;
        exit(1); // خروج مع خطأ / Exit with error
    }
}

// ════════════════════════════════════════════════════════════════════════════════
// اختبارات الأنواع البدائية / Primitive Type Tests
// ════════════════════════════════════════════════════════════════════════════════

// اختبار 1: إنشاء الأنواع البدائية / Test 1: Create primitive types
void testCreatePrimitiveTypes() {
    std::cout << "\n=== اختبار 1: إنشاء الأنواع البدائية ===" << std::endl;
    
    // إنشاء نوع فراغ / Create Void type
    TypePtr voidType = createVoidType();
    assert(voidType != nullptr); // التحقق من عدم null / Check not null
    assert(voidType->isVoid());  // التحقق من النوع / Check type
    printTestResult("إنشاء نوع فراغ / Create Void type", true);
    
    // إنشاء نوع رقم / Create Integer type
    TypePtr intType = createIntegerType();
    assert(intType != nullptr);
    assert(intType->isInteger());
    printTestResult("إنشاء نوع رقم / Create Integer type", true);
    
    // إنشاء نوع عشري / Create Float type
    TypePtr floatType = createFloatType();
    assert(floatType != nullptr);
    assert(floatType->isFloat());
    printTestResult("إنشاء نوع عشري / Create Float type", true);
    
    // إنشاء نوع منطقي / Create Boolean type
    TypePtr boolType = createBooleanType();
    assert(boolType != nullptr);
    assert(boolType->isBoolean());
    printTestResult("إنشاء نوع منطقي / Create Boolean type", true);
    
    // إنشاء نوع نص / Create String type
    TypePtr stringType = createStringType();
    assert(stringType != nullptr);
    assert(stringType->isString());
    printTestResult("إنشاء نوع نص / Create String type", true);
}

// اختبار 2: أسماء الأنواع / Test 2: Type names
void testTypeNames() {
    std::cout << "\n=== اختبار 2: أسماء الأنواع ===" << std::endl;
    
    // نوع رقم / Integer type
    TypePtr intType = createIntegerType();
    assert(intType->getArabicName() == "رقم");
    assert(intType->getEnglishName() == "Integer");
    printTestResult("اسم نوع رقم / Integer type name", true);
    
    // نوع نص / String type
    TypePtr stringType = createStringType();
    assert(stringType->getArabicName() == "نص");
    assert(stringType->getEnglishName() == "String");
    printTestResult("اسم نوع نص / String type name", true);
    
    // نوع منطقي / Boolean type
    TypePtr boolType = createBooleanType();
    assert(boolType->getArabicName() == "منطقي");
    assert(boolType->getEnglishName() == "Boolean");
    printTestResult("اسم نوع منطقي / Boolean type name", true);
}

// اختبار 3: مقارنة الأنواع / Test 3: Type comparison
void testTypeComparison() {
    std::cout << "\n=== اختبار 3: مقارنة الأنواع ===" << std::endl;
    
    // إنشاء نوعين رقم / Create two Integer types
    TypePtr int1 = createIntegerType();
    TypePtr int2 = createIntegerType();
    
    // يجب أن يكونا متساويين / Should be equal
    assert(int1->equals(int2.get()));
    assert(typesEqual(int1, int2));
    printTestResult("تساوي نفس النوع / Same type equality", true);
    
    // إنشاء أنواع مختلفة / Create different types
    TypePtr intType = createIntegerType();
    TypePtr floatType = createFloatType();
    
    // يجب أن يكونا مختلفين / Should be different
    assert(!intType->equals(floatType.get()));
    assert(!typesEqual(intType, floatType));
    printTestResult("اختلاف أنواع مختلفة / Different type inequality", true);
}

// اختبار 4: حجم الأنواع / Test 4: Type sizes
void testTypeSizes() {
    std::cout << "\n=== اختبار 4: حجم الأنواع ===" << std::endl;
    
    // نوع فراغ / Void type
    TypePtr voidType = createVoidType();
    assert(voidType->getSizeInBytes() == 0);
    printTestResult("حجم نوع فراغ = 0 / Void type size = 0", true);
    
    // نوع منطقي / Boolean type
    TypePtr boolType = createBooleanType();
    assert(boolType->getSizeInBytes() == 1);
    printTestResult("حجم نوع منطقي = 1 / Boolean type size = 1", true);
    
    // نوع رقم / Integer type
    TypePtr intType = createIntegerType();
    assert(intType->getSizeInBytes() == 8);
    printTestResult("حجم نوع رقم = 8 / Integer type size = 8", true);
    
    // نوع عشري / Float type
    TypePtr floatType = createFloatType();
    assert(floatType->getSizeInBytes() == 8);
    printTestResult("حجم نوع عشري = 8 / Float type size = 8", true);
}

// ════════════════════════════════════════════════════════════════════════════════
// اختبارات سجل الأنواع / Type Registry Tests
// ════════════════════════════════════════════════════════════════════════════════

// اختبار 5: سجل الأنواع / Test 5: Type registry
void testTypeRegistry() {
    std::cout << "\n=== اختبار 5: سجل الأنواع ===" << std::endl;
    
    // الحصول على السجل / Get registry
    TypeRegistry& registry = TypeRegistry::getInstance();
    
    // الحصول على نوع رقم من السجل / Get Integer type from registry
    TypePtr int1 = registry.getIntegerType();
    TypePtr int2 = registry.getIntegerType();
    
    // يجب أن يكونا نفس الكائن (Type Interning) / Should be same object (Type Interning)
    assert(int1.get() == int2.get());
    printTestResult("Type Interning للأنواع البدائية / Primitive type interning", true);
    
    // الحصول على أنواع مختلفة / Get different types
    TypePtr intType = registry.getIntegerType();
    TypePtr floatType = registry.getFloatType();
    TypePtr boolType = registry.getBooleanType();
    TypePtr stringType = registry.getStringType();
    
    // التحقق من الأنواع / Verify types
    assert(intType->isInteger());
    assert(floatType->isFloat());
    assert(boolType->isBoolean());
    assert(stringType->isString());
    printTestResult("الحصول على جميع الأنواع البدائية / Get all primitive types", true);
}

// اختبار 6: أنواع خاصة / Test 6: Special types
void testSpecialTypes() {
    std::cout << "\n=== اختبار 6: أنواع خاصة ===" << std::endl;
    
    // الحصول على السجل / Get registry
    TypeRegistry& registry = TypeRegistry::getInstance();
    
    // نوع Any / Any type
    TypePtr anyType = registry.getAnyType();
    assert(anyType->isAny());
    printTestResult("نوع Any / Any type", true);
    
    // نوع Never / Never type
    TypePtr neverType = registry.getNeverType();
    assert(neverType->isNever());
    printTestResult("نوع Never / Never type", true);
    
    // نوع Unknown / Unknown type
    TypePtr unknownType = registry.getUnknownType();
    assert(unknownType->isUnknown());
    printTestResult("نوع Unknown / Unknown type", true);
}

// اختبار 7: علاقات الأنواع / Test 7: Type relations
void testTypeRelations() {
    std::cout << "\n=== اختبار 7: علاقات الأنواع ===" << std::endl;
    
    // الحصول على السجل / Get registry
    TypeRegistry& registry = TypeRegistry::getInstance();
    
    // اختبار Integer -> Float / Test Integer -> Float
    TypePtr intType = registry.getIntegerType();
    TypePtr floatType = registry.getFloatType();
    assert(intType->isAssignableTo(floatType.get()));
    printTestResult("Integer -> Float (assignable)", true);
    
    // اختبار Float -> Integer (يجب أن يفشل) / Test Float -> Integer (should fail)
    assert(!floatType->isAssignableTo(intType.get()));
    printTestResult("Float -> Integer (not assignable)", true);
    
    // اختبار Any يقبل كل شيء / Test Any accepts everything
    TypePtr anyType = registry.getAnyType();
    assert(intType->isAssignableTo(anyType.get()));
    assert(floatType->isAssignableTo(anyType.get()));
    printTestResult("Any يقبل كل الأنواع / Any accepts all types", true);
    
    // اختبار Never subtype لكل شيء / Test Never is subtype of everything
    TypePtr neverType = registry.getNeverType();
    assert(neverType->isAssignableTo(intType.get()));
    assert(neverType->isAssignableTo(anyType.get()));
    printTestResult("Never subtype لكل شيء / Never subtype of everything", true);
}

// اختبار 8: إحصائيات السجل / Test 8: Registry statistics
void testRegistryStatistics() {
    std::cout << "\n=== اختبار 8: إحصائيات السجل ===" << std::endl;
    
    // الحصول على السجل / Get registry
    TypeRegistry& registry = TypeRegistry::getInstance();
    
    // عدد الأنواع البدائية / Primitive type count
    size_t primitiveCount = registry.getPrimitiveTypeCount();
    assert(primitiveCount == 8); // 8 أنواع بدائية / 8 primitive types
    printTestResult("عدد الأنواع البدائية = 8 / Primitive count = 8", true);
    
    // عدد الأنواع الإجمالي / Total type count
    size_t totalCount = registry.getTypeCount();
    assert(totalCount >= primitiveCount); // على الأقل 8 / At least 8
    printTestResult("عدد الأنواع الإجمالي / Total type count", true);
}

// ════════════════════════════════════════════════════════════════════════════════
// الدالة الرئيسية / Main Function
// ════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "════════════════════════════════════════════════════" << std::endl;
    std::cout << "  اختبار Phase 1.3.0: نواة نظام الأنواع  " << std::endl;
    std::cout << "  Phase 1.3.0 Test: Type System Core      " << std::endl;
    std::cout << "════════════════════════════════════════════════════" << std::endl;
    
    try {
        // تشغيل جميع الاختبارات / Run all tests
        testCreatePrimitiveTypes();     // اختبار 1 / Test 1
        testTypeNames();                // اختبار 2 / Test 2
        testTypeComparison();           // اختبار 3 / Test 3
        testTypeSizes();                // اختبار 4 / Test 4
        testTypeRegistry();             // اختبار 5 / Test 5
        testSpecialTypes();             // اختبار 6 / Test 6
        testTypeRelations();            // اختبار 7 / Test 7
        testRegistryStatistics();       // اختبار 8 / Test 8
        
        // نجاح جميع الاختبارات / All tests passed
        std::cout << "\n════════════════════════════════════════════════════" << std::endl;
        std::cout << "  ✅ جميع الاختبارات نجحت (8/8)  " << std::endl;
        std::cout << "  ✅ All tests passed (8/8)        " << std::endl;
        std::cout << "════════════════════════════════════════════════════" << std::endl;
        
        return 0; // نجاح / Success
        
    } catch (const std::exception& e) {
        // خطأ / Error
        std::cerr << "\n❌ خطأ: " << e.what() << std::endl;
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1; // فشل / Failure
    }
}
