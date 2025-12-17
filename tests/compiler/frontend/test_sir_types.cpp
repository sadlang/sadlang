/*
 * اختبارات أنواع SIR - SIR Types Tests
 * 
 * الوصف: اختبارات شاملة لنظام الأنواع في SIR
 * Description: Comprehensive tests for SIR type system
 *
 * يختبر:
 * Tests:
 * - إنشاء الأنواع / Type creation
 * - التوافق بين الأنواع / Type compatibility
 * - حجم ومحاذاة الأنواع / Type size and alignment
 * - تحويل الأنواع / Type conversion
 */

#include <cassert>
#include <iostream>
#include <memory>
#include "sir_types.h"
#include "type_info.h"

using namespace Sad::Compiler::SIR;

// ============================================================================
// Basic Type Creation Tests
// ============================================================================

void testBasicTypeCreation() {
    std::cout << "Testing basic type creation..." << std::endl;
    
    // اختبار الأنواع البسيطة / Test primitive types
    auto i64Type = makeI64Type();
    assert(i64Type->getBaseType() == SIRType::I64);
    assert(i64Type->toString() == "i64");
    
    auto f64Type = makeF64Type();
    assert(f64Type->getBaseType() == SIRType::F64);
    assert(f64Type->toString() == "f64");
    
    auto boolType = makeBoolType();
    assert(boolType->getBaseType() == SIRType::BOOL);
    assert(boolType->toString() == "bool");
    
    auto voidType = makeVoidType();
    assert(voidType->getBaseType() == SIRType::VOID);
    assert(voidType->toString() == "void");
    
    std::cout << "  ✓ Basic types created successfully" << std::endl;
}

// ============================================================================
// Pointer Type Tests
// ============================================================================

void testPointerTypes() {
    std::cout << "Testing pointer types..." << std::endl;
    
    // اختبار مؤشر i64 / Test i64 pointer
    auto i64Type = makeI64Type();
    auto ptrType = makePtrType(i64Type);
    
    assert(ptrType->getBaseType() == SIRType::PTR);
    auto ptrTypeCast = std::dynamic_pointer_cast<PointerType>(ptrType);
    assert(ptrTypeCast && ptrTypeCast->getPointeeType() != nullptr);
    assert(ptrTypeCast->getPointeeType()->getBaseType() == SIRType::I64);
    assert(ptrType->toString() == "i64*");
    
    // اختبار مؤشر مؤشر / Test pointer to pointer
    auto ptrPtrType = makePtrType(ptrType);
    assert(ptrPtrType->toString() == "i64**");
    
    std::cout << "  ✓ Pointer types work correctly" << std::endl;
}

// ============================================================================
// Array Type Tests
// ============================================================================

void testArrayTypes() {
    std::cout << "Testing array types..." << std::endl;
    
    // اختبار مصفوفة i64 / Test i64 array
    auto i64Type = makeI64Type();
    auto arrayType = makeArrayType(i64Type, 10);
    
    assert(arrayType->getBaseType() == SIRType::ARRAY);
    auto arrTypeCast = std::dynamic_pointer_cast<ArrayType>(arrayType);
    assert(arrTypeCast && arrTypeCast->getElementType() != nullptr);
    assert(arrTypeCast->getElementType()->getBaseType() == SIRType::I64);
    assert(arrTypeCast->getArraySize() == 10);
    assert(arrayType->toString() == "[10 x i64]");
    
    // اختبار مصفوفة من المؤشرات / Test array of pointers
    auto ptrType = makePtrType(i64Type);
    auto ptrArrayType = makeArrayType(ptrType, 5);
    assert(ptrArrayType->toString() == "[5 x i64*]");
    
    std::cout << "  ✓ Array types work correctly" << std::endl;
}

// ============================================================================
// String Type Tests
// ============================================================================

void testStringTypes() {
    std::cout << "Testing string types..." << std::endl;
    
    auto stringType = makeStringType();
    
    assert(stringType->getBaseType() == SIRType::STRING);
    assert(stringType->toString() == "string");
    
    std::cout << "  ✓ String type works correctly" << std::endl;
}

// ============================================================================
// Struct Type Tests
// ============================================================================

void testStructTypes() {
    std::cout << "Testing struct types..." << std::endl;
    
    // إنشاء struct بسيط / Create simple struct
    std::vector<std::shared_ptr<TypeInfo>> fields;
    fields.push_back(makeI64Type());
    fields.push_back(makeF64Type());
    fields.push_back(makeBoolType());
    
    auto structType = makeStructType("Point", fields);
    
    assert(structType->getBaseType() == SIRType::STRUCT);
    auto structTypeCast = std::dynamic_pointer_cast<StructType>(structType);
    assert(structTypeCast && structTypeCast->getStructName() == "Point");
    assert(structTypeCast->getFieldTypes().size() == 3);
    assert(structType->toString() == "%Point");
    
    std::cout << "  ✓ Struct types work correctly" << std::endl;
}

// ============================================================================
// Function Type Tests
// ============================================================================

void testFunctionTypes() {
    std::cout << "Testing function types..." << std::endl;
    
    // إنشاء نوع دالة / Create function type
    std::vector<std::shared_ptr<TypeInfo>> paramTypes;
    paramTypes.push_back(makeI64Type());
    paramTypes.push_back(makeF64Type());
    
    auto returnType = makeBoolType();
    auto funcType = makeFunctionType(returnType, paramTypes);
    
    assert(funcType->getBaseType() == SIRType::FUNCTION);
    auto funcTypeCast = std::dynamic_pointer_cast<FunctionType>(funcType);
    assert(funcTypeCast && funcTypeCast->getReturnType() != nullptr);
    assert(funcTypeCast->getReturnType()->getBaseType() == SIRType::BOOL);
    assert(funcTypeCast->getParameterTypes().size() == 2);
    
    std::cout << "  ✓ Function types work correctly" << std::endl;
}

// ============================================================================
// Type Compatibility Tests
// ============================================================================

void testTypeCompatibility() {
    std::cout << "Testing type compatibility..." << std::endl;
    
    // نفس النوع متوافق / Same type is compatible
    auto i64Type1 = makeI64Type();
    auto i64Type2 = makeI64Type();
    assert(i64Type1->equals(i64Type2));
    
    // أنواع مختلفة غير متوافقة / Different types incompatible
    auto f64Type = makeF64Type();
    assert(!i64Type1->equals(f64Type));
    
    // المؤشرات من نفس النوع متوافقة / Same pointer types compatible
    auto ptr1 = makePtrType(i64Type1);
    auto ptr2 = makePtrType(i64Type2);
    assert(ptr1->equals(ptr2));
    
    // المؤشرات لأنواع مختلفة غير متوافقة / Different pointer types incompatible
    auto ptrF64 = makePtrType(f64Type);
    assert(!ptr1->equals(ptrF64));
    
    std::cout << "  ✓ Type compatibility works correctly" << std::endl;
}

// ============================================================================
// Type Size Tests
// ============================================================================

void testTypeSize() {
    std::cout << "Testing type sizes..." << std::endl;
    
    // الأنواع البسيطة / Primitive types
    assert(makeI64Type()->getSize() == 8);
    assert(makeF64Type()->getSize() == 8);
    assert(makeBoolType()->getSize() == 1);
    
    // المؤشرات / Pointers
    assert(makePtrType(makeI64Type())->getSize() == 8);
    
    // المصفوفات / Arrays
    auto arrayType = makeArrayType(makeI64Type(), 10);
    assert(arrayType->getSize() == 80); // 10 * 8
    
    // السلاسل النصية / Strings (pointer size)
    assert(makeStringType()->getSize() == 8);
    
    std::cout << "  ✓ Type sizes calculated correctly" << std::endl;
}

// ============================================================================
// Type Alignment Tests
// ============================================================================

void testTypeAlignment() {
    std::cout << "Testing type alignment..." << std::endl;
    
    // الأنواع البسيطة / Primitive types
    assert(makeI64Type()->getAlignment() == 8);
    assert(makeF64Type()->getAlignment() == 8);
    assert(makeBoolType()->getAlignment() == 1);
    
    // المؤشرات / Pointers
    assert(makePtrType(makeI64Type())->getAlignment() == 8);
    
    // المصفوفات / Arrays
    assert(makeArrayType(makeI64Type(), 10)->getAlignment() == 8);
    
    std::cout << "  ✓ Type alignment calculated correctly" << std::endl;
}

// ============================================================================
// String Conversion Tests
// ============================================================================

void testStringConversion() {
    std::cout << "Testing string conversion..." << std::endl;
    
    // SIRType to string
    assert(std::string(sirTypeToString(SIRType::I64)) == "i64");
    assert(std::string(sirTypeToString(SIRType::F64)) == "f64");
    assert(std::string(sirTypeToString(SIRType::BOOL)) == "bool");
    assert(std::string(sirTypeToString(SIRType::PTR)) == "ptr");
    assert(std::string(sirTypeToString(SIRType::ARRAY)) == "array");
    assert(std::string(sirTypeToString(SIRType::STRING)) == "string");
    
    // String to SIRType
    assert(stringToSIRType("i64") == SIRType::I64);
    assert(stringToSIRType("f64") == SIRType::F64);
    assert(stringToSIRType("bool") == SIRType::BOOL);
    assert(stringToSIRType("string") == SIRType::STRING);
    
    std::cout << "  ✓ String conversion works correctly" << std::endl;
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "SIR Types Test Suite" << std::endl;
    std::cout << "اختبارات أنواع SIR" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    try {
        testBasicTypeCreation();
        testPointerTypes();
        testArrayTypes();
        testStringTypes();
        testStructTypes();
        testFunctionTypes();
        testTypeCompatibility();
        testTypeSize();
        testTypeAlignment();
        testStringConversion();
        
        std::cout << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "✓ All tests passed!" << std::endl;
        std::cout << "✓ جميع الاختبارات نجحت!" << std::endl;
        std::cout << "========================================" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << std::endl;
        std::cerr << "========================================" << std::endl;
        std::cerr << "✗ Test failed: " << e.what() << std::endl;
        std::cerr << "✗ فشل الاختبار" << std::endl;
        std::cerr << "========================================" << std::endl;
        
        return 1;
    }
}
