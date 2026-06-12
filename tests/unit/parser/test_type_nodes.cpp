/**
 * @file test_type_nodes.cpp
 * @brief Test cases for Type Annotation Nodes / حالات اختبار لعُقد تعليمات الأنواع
 * @author Sad Language Team
 * @date January 3, 2026
 * 
 * هذا الملف يختبر جميع أنواع عُقد تعليمات الأنواع المتقدمة
 * This file tests all advanced type annotation node types
 */

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <memory>
#include "parser/ast/type_nodes.h"
#include "data/types/data_types.h"

using namespace Sad::AST;
using namespace Sad::Data;

// عداد الاختبارات / Test counter
int tests_passed = 0;
int tests_failed = 0;

/**
 * @brief تشغيل اختبار وطباعة النتيجة / Run a test and print result
 */
void runTest(const std::string& testName, bool condition) {
    if (condition) {
        std::cout << "✓ " << testName << " - نجح / PASSED" << std::endl;
        tests_passed++;
    } else {
        std::cout << "✗ " << testName << " - فشل / FAILED" << std::endl;
        tests_failed++;
    }
}

/**
 * @brief اختبار SimpleTypeNode / Test SimpleTypeNode
 */
void testSimpleTypeNode() {
    std::cout << "\n=== اختبار SimpleTypeNode / Testing SimpleTypeNode ===" << std::endl;
    
    // اختبار 1: إنشاء نوع بسيط من DataType
    // Test 1: Create simple type from DataType
    auto intType = makeSimpleType(DataType::INTEGER);
    runTest("SimpleType from INTEGER", 
            intType->kind == TypeAnnotationKind::SIMPLE &&
            intType->toString() == "رقم");
    
    // اختبار 2: إنشاء نوع بسيط من اسم
    // Test 2: Create simple type from name
    auto strType = makeSimpleType("نص");
    runTest("SimpleType from string name", 
            strType->toString() == "نص");
    
    // اختبار 3: دعم اللغة الإنجليزية
    // Test 3: English language support
    auto boolType = makeSimpleType("boolean");
    runTest("SimpleType from English name (preserves original)", 
            boolType->toString() == "boolean");
    
    // اختبار 4: التوافق بين الأنواع المتطابقة
    // Test 4: Compatibility between identical types
    auto intType2 = makeSimpleType(DataType::INTEGER);
    runTest("Simple type compatibility (identical)", 
            intType->isCompatibleWith(*intType2));
    
    // اختبار 5: التحويل الضمني من INTEGER إلى FLOAT
    // Test 5: Implicit conversion from INTEGER to FLOAT
    auto floatType = makeSimpleType(DataType::FLOAT);
    runTest("Simple type implicit conversion (int->float)", 
            intType->isCompatibleWith(*floatType));
}

/**
 * @brief اختبار GenericTypeNode / Test GenericTypeNode
 */
void testGenericTypeNode() {
    std::cout << "\n=== اختبار GenericTypeNode / Testing GenericTypeNode ===" << std::endl;
    
    // اختبار 1: مصفوفة<رقم>
    // Test 1: Array<number>
    std::vector<TypeNodePtr> params1 = { makeSimpleType(DataType::INTEGER) };
    auto arrayInt = makeGenericType("مصفوفة", params1);
    runTest("GenericType Array<Integer>", 
            arrayInt->toString() == "مصفوفة<رقم>");
    
    // اختبار 2: قاموس<نص، رقم>
    // Test 2: Dictionary<string, number>
    std::vector<TypeNodePtr> params2 = { 
        makeSimpleType(DataType::STRING), 
        makeSimpleType(DataType::INTEGER) 
    };
    auto dictStrInt = makeGenericType("قاموس", params2);
    runTest("GenericType Dictionary<String, Integer>", 
            dictStrInt->toString() == "قاموس<نص، رقم>");
    
    // اختبار 3: التوافق بين أنواع Generic متطابقة
    // Test 3: Compatibility between identical generic types
    std::vector<TypeNodePtr> params3 = { makeSimpleType(DataType::INTEGER) };
    auto arrayInt2 = makeGenericType("مصفوفة", params3);
    runTest("Generic type compatibility (identical)", 
            arrayInt->isCompatibleWith(*arrayInt2));
    
    // اختبار 4: عدم التوافق مع Generic مختلف
    // Test 4: Incompatibility with different generic
    std::vector<TypeNodePtr> params4 = { makeSimpleType(DataType::STRING) };
    auto arrayStr = makeGenericType("مصفوفة", params4);
    runTest("Generic type incompatibility (different param)", 
            !arrayInt->isCompatibleWith(*arrayStr));
}

/**
 * @brief اختبار UnionTypeNode / Test UnionTypeNode
 */
void testUnionTypeNode() {
    std::cout << "\n=== اختبار UnionTypeNode / Testing UnionTypeNode ===" << std::endl;
    
    // اختبار 1: رقم | نص
    // Test 1: number | string
    std::vector<TypeNodePtr> types1 = { 
        makeSimpleType(DataType::INTEGER), 
        makeSimpleType(DataType::STRING) 
    };
    auto unionIntStr = makeUnionType(types1);
    runTest("UnionType number|string", 
            unionIntStr->toString() == "رقم | نص");
    
    // اختبار 2: التوافق مع أحد عناصر الاتحاد
    // Test 2: Compatibility with one of the union elements
    auto intType = makeSimpleType(DataType::INTEGER);
    runTest("Union type compatibility (contains element)", 
            unionIntStr->isCompatibleWith(*intType));
    
    // اختبار 3: عدم التوافق مع نوع غير موجود في الاتحاد
    // Test 3: Incompatibility with type not in union
    auto boolType = makeSimpleType(DataType::BOOLEAN);
    runTest("Union type incompatibility (not in union)", 
            !unionIntStr->isCompatibleWith(*boolType));
}

/**
 * @brief اختبار OptionalTypeNode / Test OptionalTypeNode
 */
void testOptionalTypeNode() {
    std::cout << "\n=== اختبار OptionalTypeNode / Testing OptionalTypeNode ===" << std::endl;
    
    // اختبار 1: رقم?
    // Test 1: number?
    auto intType = makeSimpleType(DataType::INTEGER);
    auto optIntType = makeOptionalType(intType);
    runTest("OptionalType number?", 
            optIntType->toString() == "رقم?");
    
    // اختبار 2: التوافق بين Optional متطابقة
    // Test 2: Compatibility between identical optionals
    auto intType2 = makeSimpleType(DataType::INTEGER);
    auto optIntType2 = makeOptionalType(intType2);
    runTest("Optional type compatibility (identical)", 
            optIntType->isCompatibleWith(*optIntType2));
    
    // اختبار 3: التوافق مع النوع الأساسي
    // Test 3: Compatibility with base type
    auto intType3 = makeSimpleType(DataType::INTEGER);
    runTest("Optional type compatibility (with base)", 
            optIntType->isCompatibleWith(*intType3));
}

/**
 * @brief اختبار FunctionTypeNode / Test FunctionTypeNode
 */
void testFunctionTypeNode() {
    std::cout << "\n=== اختبار FunctionTypeNode / Testing FunctionTypeNode ===" << std::endl;
    
    // اختبار 1: (رقم، نص) -> منطقي
    // Test 1: (number, string) -> boolean
    std::vector<TypeNodePtr> params = { 
        makeSimpleType(DataType::INTEGER), 
        makeSimpleType(DataType::STRING) 
    };
    auto returnType = makeSimpleType(DataType::BOOLEAN);
    auto funcType = makeFunctionType(params, returnType);
    runTest("FunctionType (number, string) -> boolean", 
            funcType->toString() == "(رقم، نص) -> منطقي");
    
    // اختبار 2: التوافق بين Function types متطابقة
    // Test 2: Compatibility between identical function types
    std::vector<TypeNodePtr> params2 = { 
        makeSimpleType(DataType::INTEGER), 
        makeSimpleType(DataType::STRING) 
    };
    auto returnType2 = makeSimpleType(DataType::BOOLEAN);
    auto funcType2 = makeFunctionType(params2, returnType2);
    runTest("Function type compatibility (identical)", 
            funcType->isCompatibleWith(*funcType2));
    
    // اختبار 3: عدم التوافق مع عدد معاملات مختلف
    // Test 3: Incompatibility with different param count
    std::vector<TypeNodePtr> params3 = { makeSimpleType(DataType::INTEGER) };
    auto funcType3 = makeFunctionType(params3, returnType);
    runTest("Function type incompatibility (different params)", 
            !funcType->isCompatibleWith(*funcType3));
}

/**
 * @brief اختبار ArrayTypeNode / Test ArrayTypeNode
 */
void testArrayTypeNode() {
    std::cout << "\n=== اختبار ArrayTypeNode / Testing ArrayTypeNode ===" << std::endl;
    
    // اختبار 1: رقم[]
    // Test 1: number[]
    auto elementType = makeSimpleType(DataType::INTEGER);
    auto arrayType = makeArrayType(elementType);
    runTest("ArrayType number[]", 
            arrayType->toString() == "رقم[]");
    
    // اختبار 2: التوافق بين Array types متطابقة
    // Test 2: Compatibility between identical array types
    auto elementType2 = makeSimpleType(DataType::INTEGER);
    auto arrayType2 = makeArrayType(elementType2);
    runTest("Array type compatibility (identical)", 
            arrayType->isCompatibleWith(*arrayType2));
    
    // اختبار 3: عدم التوافق مع array type مختلف
    // Test 3: Incompatibility with different array type
    auto elementType3 = makeSimpleType(DataType::STRING);
    auto arrayType3 = makeArrayType(elementType3);
    runTest("Array type incompatibility (different element)", 
            !arrayType->isCompatibleWith(*arrayType3));
}

/**
 * @brief اختبار DictionaryTypeNode / Test DictionaryTypeNode
 */
void testDictionaryTypeNode() {
    std::cout << "\n=== اختبار DictionaryTypeNode / Testing DictionaryTypeNode ===" << std::endl;
    
    // اختبار 1: {نص: رقم}
    // Test 1: {string: number}
    auto keyType = makeSimpleType(DataType::STRING);
    auto valueType = makeSimpleType(DataType::INTEGER);
    auto dictType = makeDictionaryType(keyType, valueType);
    runTest("DictionaryType {string: number}", 
            dictType->toString() == "{نص: رقم}");
    
    // اختبار 2: التوافق بين Dictionary types متطابقة
    // Test 2: Compatibility between identical dictionary types
    auto keyType2 = makeSimpleType(DataType::STRING);
    auto valueType2 = makeSimpleType(DataType::INTEGER);
    auto dictType2 = makeDictionaryType(keyType2, valueType2);
    runTest("Dictionary type compatibility (identical)", 
            dictType->isCompatibleWith(*dictType2));
}

/**
 * @brief اختبار TupleTypeNode / Test TupleTypeNode
 */
void testTupleTypeNode() {
    std::cout << "\n=== اختبار TupleTypeNode / Testing TupleTypeNode ===" << std::endl;
    
    // اختبار 1: (رقم، نص، منطقي)
    // Test 1: (number, string, boolean)
    std::vector<TypeNodePtr> elements = { 
        makeSimpleType(DataType::INTEGER), 
        makeSimpleType(DataType::STRING),
        makeSimpleType(DataType::BOOLEAN)
    };
    auto tupleType = std::make_shared<TupleTypeNode>(elements);
    runTest("TupleType (number, string, boolean)", 
            tupleType->toString() == "(رقم، نص، منطقي)");
    
    // اختبار 2: التوافق بين Tuple types متطابقة
    // Test 2: Compatibility between identical tuple types
    std::vector<TypeNodePtr> elements2 = { 
        makeSimpleType(DataType::INTEGER), 
        makeSimpleType(DataType::STRING),
        makeSimpleType(DataType::BOOLEAN)
    };
    auto tupleType2 = std::make_shared<TupleTypeNode>(elements2);
    runTest("Tuple type compatibility (identical)", 
            tupleType->isCompatibleWith(*tupleType2));
}

/**
 * @brief الدالة الرئيسية / Main function
 */
int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "   اختبارات عُقد تعليمات الأنواع المتقدمة" << std::endl;
    std::cout << "   Advanced Type Annotation Nodes Tests" << std::endl;
    std::cout << "==================================================" << std::endl;
    
    // تشغيل جميع الاختبارات / Run all tests
    testSimpleTypeNode();
    testGenericTypeNode();
    testUnionTypeNode();
    testOptionalTypeNode();
    testFunctionTypeNode();
    testArrayTypeNode();
    testDictionaryTypeNode();
    testTupleTypeNode();
    
    // طباعة النتيجة النهائية / Print final result
    std::cout << "\n==================================================" << std::endl;
    std::cout << "النتيجة النهائية / Final Results:" << std::endl;
    std::cout << "✓ نجح / Passed: " << tests_passed << std::endl;
    std::cout << "✗ فشل / Failed: " << tests_failed << std::endl;
    std::cout << "المجموع / Total: " << (tests_passed + tests_failed) << std::endl;
    std::cout << "==================================================" << std::endl;
    
    return (tests_failed == 0) ? 0 : 1;
}
