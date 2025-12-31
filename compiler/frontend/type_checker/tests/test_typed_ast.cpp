/*
 * ============================================================================
 * Typed AST Tests - اختبارات Typed AST
 * Typed AST Tests
 * ============================================================================
 * 
 * الوصف (Description):
 *   اختبارات شاملة لنظام الأنواع المطبوع
 *   Comprehensive tests for Typed AST system
 * 
 * المكونات المُختبَرة (Tested Components):
 *   - Type hierarchy
 *   - TypeFactory
 *   - Type operations
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 31, 2025
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#include <gtest/gtest.h>
#include "typed_ast.h"

using namespace Sad::TypeChecker;

// ============================================================================
// Basic Type Tests - اختبارات الأنواع الأساسية
// ============================================================================

/**
 * اختبار: إنشاء أنواع بدائية / Test: Create primitive types
 */
TEST(TypedASTTest, CreatePrimitiveTypes) {
    auto intType = TypeFactory::getIntType();
    auto floatType = TypeFactory::getFloatType();
    auto stringType = TypeFactory::getStringType();
    auto boolType = TypeFactory::getBoolType();
    auto voidType = TypeFactory::getVoidType();
    
    EXPECT_EQ(intType->getKind(), Type::Kind::INT);
    EXPECT_EQ(floatType->getKind(), Type::Kind::FLOAT);
    EXPECT_EQ(stringType->getKind(), Type::Kind::STRING);
    EXPECT_EQ(boolType->getKind(), Type::Kind::BOOL);
    EXPECT_EQ(voidType->getKind(), Type::Kind::VOID);
}

/**
 * اختبار: مقارنة الأنواع / Test: Type equality
 */
TEST(TypedASTTest, TypeEquality) {
    auto int1 = TypeFactory::getIntType();
    auto int2 = TypeFactory::getIntType();
    auto float1 = TypeFactory::getFloatType();
    
    EXPECT_TRUE(int1->equals(int2.get()));
    EXPECT_FALSE(int1->equals(float1.get()));
}

/**
 * اختبار: تحويل الأنواع / Test: Type casting
 */
TEST(TypedASTTest, TypeCasting) {
    auto intType = TypeFactory::getIntType();
    auto floatType = TypeFactory::getFloatType();
    auto stringType = TypeFactory::getStringType();
    
    // int يمكن تحويله إلى float / int can cast to float
    EXPECT_TRUE(intType->canCastTo(floatType.get()));
    
    // float يمكن تحويله إلى int (قد يفقد دقة) / float can cast to int
    EXPECT_TRUE(floatType->canCastTo(intType.get()));
    
    // string لا يمكن تحويله إلى int مباشرة / string cannot cast to int directly
    EXPECT_FALSE(stringType->canCastTo(intType.get()));
}

/**
 * اختبار: تمثيل الأنواع كنص / Test: Type to string
 */
TEST(TypedASTTest, TypeToString) {
    auto intType = TypeFactory::getIntType();
    auto stringType = TypeFactory::getStringType();
    
    EXPECT_EQ(intType->toString(), "int");
    EXPECT_EQ(stringType->toString(), "string");
}

// ============================================================================
// Array Type Tests - اختبارات نوع المصفوفات
// ============================================================================

/**
 * اختبار: إنشاء نوع مصفوفة / Test: Create array type
 */
TEST(ArrayTypeTest, CreateArrayType) {
    auto intType = TypeFactory::getIntType();
    auto arrayType = std::make_shared<ArrayType>(intType);
    
    EXPECT_EQ(arrayType->getKind(), Type::Kind::ARRAY);
    EXPECT_TRUE(arrayType->getElementType()->equals(intType.get()));
}

/**
 * اختبار: مصفوفات متداخلة / Test: Nested arrays
 */
TEST(ArrayTypeTest, NestedArrays) {
    auto intType = TypeFactory::getIntType();
    auto arrayType = std::make_shared<ArrayType>(intType);
    auto arrayArrayType = std::make_shared<ArrayType>(arrayType);
    
    EXPECT_EQ(arrayArrayType->getKind(), Type::Kind::ARRAY);
    
    auto innerArray = std::dynamic_pointer_cast<ArrayType>(
        arrayArrayType->getElementType()
    );
    EXPECT_NE(innerArray, nullptr);
    EXPECT_TRUE(innerArray->getElementType()->equals(intType.get()));
}

// ============================================================================
// Function Type Tests - اختبارات نوع الدوال
// ============================================================================

/**
 * اختبار: إنشاء نوع دالة / Test: Create function type
 */
TEST(FunctionTypeTest, CreateFunctionType) {
    auto intType = TypeFactory::getIntType();
    auto stringType = TypeFactory::getStringType();
    
    std::vector<std::shared_ptr<Type>> params = {intType, stringType};
    auto funcType = std::make_shared<FunctionType>(params, intType);
    
    EXPECT_EQ(funcType->getKind(), Type::Kind::FUNCTION);
    EXPECT_EQ(funcType->getParamTypes().size(), 2);
    EXPECT_TRUE(funcType->getReturnType()->equals(intType.get()));
}

/**
 * اختبار: دالة بدون معاملات / Test: Function with no parameters
 */
TEST(FunctionTypeTest, NoParameterFunction) {
    auto voidType = TypeFactory::getVoidType();
    
    std::vector<std::shared_ptr<Type>> params;
    auto funcType = std::make_shared<FunctionType>(params, voidType);
    
    EXPECT_EQ(funcType->getParamTypes().size(), 0);
    EXPECT_TRUE(funcType->getReturnType()->equals(voidType.get()));
}

/**
 * اختبار: دالة variadic / Test: Variadic function
 */
TEST(FunctionTypeTest, VariadicFunction) {
    auto intType = TypeFactory::getIntType();
    
    std::vector<std::shared_ptr<Type>> params = {intType};
    auto funcType = std::make_shared<FunctionType>(params, intType, true);
    
    EXPECT_TRUE(funcType->isVariadic());
}

// ============================================================================
// Dictionary Type Tests - اختبارات نوع القواميس
// ============================================================================

/**
 * اختبار: إنشاء نوع قاموس / Test: Create dictionary type
 */
TEST(DictTypeTest, CreateDictType) {
    auto stringType = TypeFactory::getStringType();
    auto intType = TypeFactory::getIntType();
    
    auto dictType = std::make_shared<DictType>(stringType, intType);
    
    EXPECT_EQ(dictType->getKind(), Type::Kind::DICT);
    EXPECT_TRUE(dictType->getKeyType()->equals(stringType.get()));
    EXPECT_TRUE(dictType->getValueType()->equals(intType.get()));
}

// ============================================================================
// Optional Type Tests - اختبارات النوع الاختياري
// ============================================================================

/**
 * اختبار: إنشاء نوع اختياري / Test: Create optional type
 */
TEST(OptionalTypeTest, CreateOptionalType) {
    auto intType = TypeFactory::getIntType();
    auto optionalType = std::make_shared<OptionalType>(intType);
    
    EXPECT_EQ(optionalType->getKind(), Type::Kind::OPTIONAL);
    EXPECT_TRUE(optionalType->getInnerType()->equals(intType.get()));
}

/**
 * اختبار: Optional متداخل / Test: Nested optional
 */
TEST(OptionalTypeTest, NestedOptional) {
    auto intType = TypeFactory::getIntType();
    auto opt1 = std::make_shared<OptionalType>(intType);
    auto opt2 = std::make_shared<OptionalType>(opt1);
    
    auto innerOpt = std::dynamic_pointer_cast<OptionalType>(
        opt2->getInnerType()
    );
    EXPECT_NE(innerOpt, nullptr);
    EXPECT_TRUE(innerOpt->getInnerType()->equals(intType.get()));
}

// ============================================================================
// Union Type Tests - اختبارات نوع الاتحاد
// ============================================================================

/**
 * اختبار: إنشاء نوع اتحاد / Test: Create union type
 */
TEST(UnionTypeTest, CreateUnionType) {
    auto intType = TypeFactory::getIntType();
    auto stringType = TypeFactory::getStringType();
    
    std::vector<std::shared_ptr<Type>> types = {intType, stringType};
    auto unionType = std::make_shared<UnionType>(types);
    
    EXPECT_EQ(unionType->getKind(), Type::Kind::UNION);
    EXPECT_EQ(unionType->getTypes().size(), 2);
}

/**
 * اختبار: اتحاد متعدد الأنواع / Test: Multi-type union
 */
TEST(UnionTypeTest, MultiTypeUnion) {
    auto intType = TypeFactory::getIntType();
    auto floatType = TypeFactory::getFloatType();
    auto stringType = TypeFactory::getStringType();
    auto boolType = TypeFactory::getBoolType();
    
    std::vector<std::shared_ptr<Type>> types = {
        intType, floatType, stringType, boolType
    };
    auto unionType = std::make_shared<UnionType>(types);
    
    EXPECT_EQ(unionType->getTypes().size(), 4);
}

// ============================================================================
// Class Type Tests - اختبارات نوع الصنف
// ============================================================================

/**
 * اختبار: إنشاء نوع صنف / Test: Create class type
 */
TEST(ClassTypeTest, CreateClassType) {
    auto classType = std::make_shared<ClassType>("Person");
    
    EXPECT_EQ(classType->getKind(), Type::Kind::CLASS);
    EXPECT_EQ(classType->getName(), "Person");
}

/**
 * اختبار: إضافة أعضاء / Test: Add members
 */
TEST(ClassTypeTest, AddMembers) {
    auto classType = std::make_shared<ClassType>("Person");
    auto stringType = TypeFactory::getStringType();
    auto intType = TypeFactory::getIntType();
    
    classType->addMember("name", stringType);
    classType->addMember("age", intType);
    
    auto nameType = classType->getMemberType("name");
    auto ageType = classType->getMemberType("age");
    
    EXPECT_NE(nameType, nullptr);
    EXPECT_NE(ageType, nullptr);
    EXPECT_TRUE(nameType->equals(stringType.get()));
    EXPECT_TRUE(ageType->equals(intType.get()));
}

/**
 * اختبار: إضافة دوال / Test: Add methods
 */
TEST(ClassTypeTest, AddMethods) {
    auto classType = std::make_shared<ClassType>("Calculator");
    auto intType = TypeFactory::getIntType();
    
    std::vector<std::shared_ptr<Type>> params = {intType, intType};
    auto funcType = std::make_shared<FunctionType>(params, intType);
    
    classType->addMethod("add", funcType);
    
    auto addMethod = classType->getMethodType("add");
    EXPECT_NE(addMethod, nullptr);
}

// ============================================================================
// Type Factory Tests - اختبارات مصنع الأنواع
// ============================================================================

/**
 * اختبار: مصنع يعيد نفس المثيل / Test: Factory returns same instance
 */
TEST(TypeFactoryTest, SingletonInstances) {
    auto int1 = TypeFactory::getIntType();
    auto int2 = TypeFactory::getIntType();
    
    // يجب أن يكونا نفس المثيل / Should be same instance
    EXPECT_EQ(int1.get(), int2.get());
}

/**
 * اختبار: جميع الأنواع البدائية / Test: All primitive types
 */
TEST(TypeFactoryTest, AllPrimitiveTypes) {
    EXPECT_NE(TypeFactory::getIntType(), nullptr);
    EXPECT_NE(TypeFactory::getFloatType(), nullptr);
    EXPECT_NE(TypeFactory::getStringType(), nullptr);
    EXPECT_NE(TypeFactory::getBoolType(), nullptr);
    EXPECT_NE(TypeFactory::getVoidType(), nullptr);
}

// ============================================================================
// Complex Type Tests - اختبارات الأنواع المعقدة
// ============================================================================

/**
 * اختبار: دالة تُرجع مصفوفة / Test: Function returning array
 */
TEST(ComplexTypeTest, FunctionReturningArray) {
    auto intType = TypeFactory::getIntType();
    auto arrayType = std::make_shared<ArrayType>(intType);
    
    std::vector<std::shared_ptr<Type>> params;
    auto funcType = std::make_shared<FunctionType>(params, arrayType);
    
    EXPECT_EQ(funcType->getKind(), Type::Kind::FUNCTION);
    
    auto returnType = funcType->getReturnType();
    EXPECT_EQ(returnType->getKind(), Type::Kind::ARRAY);
}

/**
 * اختبار: مصفوفة من دوال / Test: Array of functions
 */
TEST(ComplexTypeTest, ArrayOfFunctions) {
    auto intType = TypeFactory::getIntType();
    
    std::vector<std::shared_ptr<Type>> params = {intType};
    auto funcType = std::make_shared<FunctionType>(params, intType);
    
    auto arrayType = std::make_shared<ArrayType>(funcType);
    
    EXPECT_EQ(arrayType->getKind(), Type::Kind::ARRAY);
    
    auto elemType = arrayType->getElementType();
    EXPECT_EQ(elemType->getKind(), Type::Kind::FUNCTION);
}

/**
 * اختبار: قاموس معقد / Test: Complex dictionary
 */
TEST(ComplexTypeTest, ComplexDictionary) {
    auto stringType = TypeFactory::getStringType();
    auto intType = TypeFactory::getIntType();
    
    // Dict<string, Array<int>>
    auto arrayType = std::make_shared<ArrayType>(intType);
    auto dictType = std::make_shared<DictType>(stringType, arrayType);
    
    EXPECT_EQ(dictType->getKind(), Type::Kind::DICT);
    EXPECT_EQ(dictType->getValueType()->getKind(), Type::Kind::ARRAY);
}

// ============================================================================
// EOF
// ============================================================================
