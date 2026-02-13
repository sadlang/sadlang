// Disable Unicode warning for Arabic strings
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file test_advanced_types.cpp
 * @brief Unit tests for Arabic Advanced Type System
 * 
 * Tests:
 * - Primitive types (arabic_types)
 * - Struct types (struct_types)
 * - Enum types (enum_types)
 * - Union types (union_types)
 * - Generics (generics)
 * - Traits (trait_system)
 * 
 * @author Sad Language Team
 * @date January 2026
 */

#include <gtest/gtest.h>
#include "types/arabic_types.h"
#include "types/struct_types.h"
#include "types/enum_types.h"
#include "types/union_types.h"
#include "types/generics.h"
#include "types/trait_system.h"

using namespace Sad::TypeSystem;

// =============================================================================
//                    Primitive Types Tests
// =============================================================================

class PrimitiveTypesTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Registry is singleton, already initialized
    }
};

TEST_F(PrimitiveTypesTest, RegistryInstance) {
    auto& registry = PrimitiveTypeRegistry::instance();
    EXPECT_FALSE(registry.getAllTypes().empty());
}

TEST_F(PrimitiveTypesTest, FindByArabicName) {
    auto& registry = PrimitiveTypeRegistry::instance();
    
    // Test unsigned integers
    auto ha8 = registry.getInfoByArabicName(ArabicTypeNames::HA8);
    ASSERT_NE(ha8, nullptr);
    EXPECT_EQ(ha8->kind, PrimitiveKind::UInt8);
    EXPECT_EQ(ha8->sizeInBytes, 1);
    EXPECT_FALSE(ha8->isSigned);
    EXPECT_TRUE(ha8->isInteger);
}

TEST_F(PrimitiveTypesTest, FindByEnglishName) {
    auto& registry = PrimitiveTypeRegistry::instance();
    
    auto i64 = registry.getInfoByEnglishName("i64");
    ASSERT_NE(i64, nullptr);
    EXPECT_EQ(i64->kind, PrimitiveKind::Int64);
    EXPECT_EQ(i64->sizeInBytes, 8);
    EXPECT_TRUE(i64->isSigned);
}

TEST_F(PrimitiveTypesTest, FloatingPointTypes) {
    auto& registry = PrimitiveTypeRegistry::instance();
    
    auto f32 = registry.getInfoByEnglishName("f32");
    ASSERT_NE(f32, nullptr);
    EXPECT_TRUE(f32->isFloating);
    EXPECT_TRUE(f32->isNumeric);
    EXPECT_FALSE(f32->isInteger);
    
    auto f64 = registry.getInfoByEnglishName("f64");
    ASSERT_NE(f64, nullptr);
    EXPECT_EQ(f64->sizeInBytes, 8);
}

TEST_F(PrimitiveTypesTest, ImplicitConversion) {
    // i8 -> i16 is safe
    EXPECT_TRUE(PrimitiveTypeRegistry::isImplicitConversionSafe(
        PrimitiveKind::Int8, PrimitiveKind::Int16));
    
    // i64 -> i32 is NOT safe (narrowing)
    EXPECT_FALSE(PrimitiveTypeRegistry::isImplicitConversionSafe(
        PrimitiveKind::Int64, PrimitiveKind::Int32));
    
    // f32 -> f64 is safe
    EXPECT_TRUE(PrimitiveTypeRegistry::isImplicitConversionSafe(
        PrimitiveKind::Float32, PrimitiveKind::Float64));
}

TEST_F(PrimitiveTypesTest, WiderType) {
    // i8 + i16 -> i16
    EXPECT_EQ(PrimitiveTypeRegistry::getWiderType(
        PrimitiveKind::Int8, PrimitiveKind::Int16), PrimitiveKind::Int16);
    
    // i32 + f64 -> f64 (float takes precedence)
    EXPECT_EQ(PrimitiveTypeRegistry::getWiderType(
        PrimitiveKind::Int32, PrimitiveKind::Float64), PrimitiveKind::Float64);
}

// =============================================================================
//                    Struct Types Tests
// =============================================================================

class StructTypesTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clear any previous test registrations if needed
    }
};

TEST_F(StructTypesTest, CreateSimpleStruct) {
    auto point = std::make_shared<StructType>("نقطة", "Point");
    point->addField("x", nullptr);  // In future: use actual type
    point->addField("y", nullptr);
    
    EXPECT_EQ(point->getFieldCount(), 2);
    EXPECT_TRUE(point->hasField("x"));
    EXPECT_TRUE(point->hasField("y"));
    EXPECT_FALSE(point->hasField("z"));
}

TEST_F(StructTypesTest, StructBuilder) {
    auto rect = StructBuilder("مستطيل", "Rectangle")
        .field("width", nullptr)
        .field("height", nullptr)
        .copy()
        .build();
    
    EXPECT_EQ(rect->getFieldCount(), 2);
    EXPECT_TRUE(rect->isCopy());
    EXPECT_FALSE(rect->isTuple());
}

TEST_F(StructTypesTest, GenericStruct) {
    auto list = std::make_shared<StructType>("قائمة", "List");
    auto paramT = std::make_shared<GenericParameter>("T", 0);
    list->addTypeParameter(paramT);
    
    EXPECT_TRUE(list->isGeneric());
    EXPECT_EQ(list->getTypeParameters().size(), 1);
}

TEST_F(StructTypesTest, StructRegistry) {
    auto& registry = StructRegistry::instance();
    
    auto person = std::make_shared<StructType>("شخص", "Person");
    person->addField("name", nullptr);
    person->addField("age", nullptr);
    
    registry.registerStruct(person);
    
    auto found = registry.findStruct("Person");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->getFieldCount(), 2);
    
    auto foundArabic = registry.findStruct("شخص");
    EXPECT_EQ(found, foundArabic);
}

// =============================================================================
//                    Enum Types Tests
// =============================================================================

class EnumTypesTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(EnumTypesTest, CreateSimpleEnum) {
    auto color = std::make_shared<EnumType>("لون", "Color");
    color->addUnitVariant("أحمر");
    color->addUnitVariant("أخضر");
    color->addUnitVariant("أزرق");
    
    EXPECT_EQ(color->getVariantCount(), 3);
    EXPECT_TRUE(color->isCLike());
}

TEST_F(EnumTypesTest, EnumWithData) {
    auto message = std::make_shared<EnumType>("رسالة", "Message");
    message->addUnitVariant("Quit");
    message->addTupleVariant("Move", {nullptr, nullptr}); // x, y
    message->addStructVariant("Write", {{"text", nullptr}});
    
    EXPECT_EQ(message->getVariantCount(), 3);
    EXPECT_FALSE(message->isCLike());
}

TEST_F(EnumTypesTest, OptionType) {
    auto& registry = EnumRegistry::instance();
    auto option = registry.getOptionType();
    
    ASSERT_NE(option, nullptr);
    EXPECT_TRUE(option->isGeneric());
    EXPECT_TRUE(option->isOptionLike());
    EXPECT_EQ(option->getVariantCount(), 2);
}

TEST_F(EnumTypesTest, ResultType) {
    auto& registry = EnumRegistry::instance();
    auto result = registry.getResultType();
    
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(result->isGeneric());
    EXPECT_TRUE(result->isResultLike());
    EXPECT_EQ(result->getTypeParameters().size(), 2);
}

TEST_F(EnumTypesTest, EnumBuilder) {
    auto status = EnumBuilder("حالة", "Status")
        .unit("Pending")
        .unit("Running")
        .unit("Completed")
        .unit("Failed")
        .repr(EnumRepr::U8)
        .build();
    
    EXPECT_EQ(status->getVariantCount(), 4);
    EXPECT_EQ(status->getRepr(), EnumRepr::U8);
    EXPECT_TRUE(status->isCLike());
}

// =============================================================================
//                    Union Types Tests
// =============================================================================

class UnionTypesTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(UnionTypesTest, CreateSimpleUnion) {
    auto value = std::make_shared<UnionType>("قيمة", "Value");
    value->addField("i", nullptr);  // int
    value->addField("f", nullptr);  // float
    value->addField("p", nullptr);  // pointer
    
    EXPECT_EQ(value->getFieldCount(), 3);
    EXPECT_TRUE(value->requiresUnsafe());
}

TEST_F(UnionTypesTest, UnionBuilder) {
    auto data = UnionBuilder("بيانات", "Data")
        .field("bytes", nullptr)
        .field("words", nullptr)
        .build();
    
    EXPECT_EQ(data->getFieldCount(), 2);
}

TEST_F(UnionTypesTest, UnionRegistry) {
    auto& registry = UnionRegistry::instance();
    
    auto number = std::make_shared<UnionType>("عدد", "Number");
    number->addField("integer", nullptr);
    number->addField("decimal", nullptr);
    
    registry.registerUnion(number);
    
    EXPECT_NE(registry.findUnion("Number"), nullptr);
    EXPECT_TRUE(registry.isUnionType("عدد"));
}

// =============================================================================
//                    Generics Tests
// =============================================================================

class GenericsTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(GenericsTest, GenericParameter) {
    auto param = std::make_shared<GenericParameter>("T", 0);
    EXPECT_EQ(param->getName(), "T");
    EXPECT_EQ(param->getIndex(), 0);
    EXPECT_FALSE(param->hasBounds());
}

TEST_F(GenericsTest, GenericParameterWithBounds) {
    auto& traitRegistry = TraitRegistry::instance();
    auto copyTrait = traitRegistry.getCopyTrait();
    
    auto param = std::make_shared<GenericParameter>("T", 0);
    param->addTraitBound(copyTrait);
    
    EXPECT_TRUE(param->hasBounds());
    EXPECT_EQ(param->getBounds().getTraits().size(), 1);
}

TEST_F(GenericsTest, GenericType) {
    auto generic = std::make_shared<GenericType>("Container");
    auto param = std::make_shared<GenericParameter>("T", 0);
    generic->addParameter(param);
    
    EXPECT_EQ(generic->getBaseName(), "Container");
    EXPECT_EQ(generic->getParameterCount(), 1);
}

TEST_F(GenericsTest, GenericsParser) {
    GenericsParser parser;
    
    auto result = parser.parse("List<T>");
    EXPECT_TRUE(result.isValid);
    EXPECT_EQ(result.baseName, "List");
    EXPECT_EQ(result.parameters.size(), 1);
}

TEST_F(GenericsTest, ExtractTypeParameters) {
    auto [base, params] = extractTypeParameters("Map<K, V>");
    EXPECT_EQ(base, "Map");
    EXPECT_EQ(params.size(), 2);
    EXPECT_EQ(params[0], "K");
    EXPECT_EQ(params[1], "V");
}

TEST_F(GenericsTest, IsGenericTypeName) {
    EXPECT_TRUE(isGenericTypeName("List<T>"));
    EXPECT_TRUE(isGenericTypeName("Map<K, V>"));
    EXPECT_FALSE(isGenericTypeName("Integer"));
    EXPECT_FALSE(isGenericTypeName("String"));
}

// =============================================================================
//                    Traits Tests
// =============================================================================

class TraitsTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(TraitsTest, CoreTraits) {
    auto& registry = TraitRegistry::instance();
    
    EXPECT_NE(registry.getCopyTrait(), nullptr);
    EXPECT_NE(registry.getCloneTrait(), nullptr);
    EXPECT_NE(registry.getEqTrait(), nullptr);
    EXPECT_NE(registry.getOrdTrait(), nullptr);
    EXPECT_NE(registry.getHashTrait(), nullptr);
    EXPECT_NE(registry.getDisplayTrait(), nullptr);
    EXPECT_NE(registry.getDebugTrait(), nullptr);
}

TEST_F(TraitsTest, TraitNames) {
    auto& registry = TraitRegistry::instance();
    auto copyTrait = registry.getCopyTrait();
    
    EXPECT_EQ(copyTrait->getEnglishName(), "Copy");
    // Arabic name should be set
    EXPECT_FALSE(copyTrait->getArabicName().empty());
}

TEST_F(TraitsTest, TraitWithMethods) {
    auto& registry = TraitRegistry::instance();
    auto cloneTrait = registry.getCloneTrait();
    
    EXPECT_GT(cloneTrait->getMethodCount(), 0);
    auto* cloneMethod = cloneTrait->findMethod("clone");
    ASSERT_NE(cloneMethod, nullptr);
    EXPECT_TRUE(cloneMethod->requiresSelf);
}

TEST_F(TraitsTest, TraitHierarchy) {
    auto& registry = TraitRegistry::instance();
    auto cloneTrait = registry.getCloneTrait();
    auto copyTrait = registry.getCopyTrait();
    
    // Clone extends Copy
    EXPECT_TRUE(cloneTrait->extendsTraitTransitive(copyTrait.get()));
}

TEST_F(TraitsTest, TraitBounds) {
    auto& registry = TraitRegistry::instance();
    auto copyTrait = registry.getCopyTrait();
    auto eqTrait = registry.getEqTrait();
    
    TraitBound bounds;
    bounds.addTrait(copyTrait);
    bounds.addTrait(eqTrait);
    
    EXPECT_EQ(bounds.getTraits().size(), 2);
    
    std::string boundsStr = bounds.toString(false);
    EXPECT_TRUE(boundsStr.find("Copy") != std::string::npos);
    EXPECT_TRUE(boundsStr.find("Eq") != std::string::npos);
}

TEST_F(TraitsTest, MarkerTrait) {
    auto& registry = TraitRegistry::instance();
    auto copyTrait = registry.getCopyTrait();
    
    // Copy is a marker trait (no methods)
    EXPECT_TRUE(copyTrait->isMarker());
    EXPECT_TRUE(copyTrait->isAuto());
}

TEST_F(TraitsTest, FindTraitByName) {
    auto& registry = TraitRegistry::instance();
    
    auto found = registry.findTrait("Copy");
    EXPECT_NE(found, nullptr);
    
    auto notFound = registry.findTrait("NonExistent");
    EXPECT_EQ(notFound, nullptr);
}

// =============================================================================
//                    Main
// =============================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
