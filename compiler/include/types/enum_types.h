// Disable Unicode warning for Arabic strings
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file enum_types.h
 * @brief Arabic Enum Type System
 * 
 * Enum types in Sad language:
 * - ta3dad (enumeration)
 * - Variants with optional data
 * - Pattern matching support
 * - Generic enum support
 * 
 * @author Sad Language Team
 * @date January 2026
 * @version 1.0
 */

#ifndef SAD_TYPES_ENUM_TYPES_H
#define SAD_TYPES_ENUM_TYPES_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <variant>
#include <unordered_map>
#include "arabic_types.h"
#include "trait_system.h"
#include "generics.h"
#include "struct_types.h"

namespace Sad {
namespace TypeSystem {

// Forward declarations
class EnumType;
class EnumVariant;
class EnumRegistry;

using EnumTypePtr = std::shared_ptr<EnumType>;
using EnumVariantPtr = std::shared_ptr<EnumVariant>;

// =============================================================================
//                    Enum Variant Kind
// =============================================================================

/**
 * @enum VariantKind
 * @brief Type of enum variant
 */
enum class VariantKind {
    Unit,       // No data (e.g., Option::None)
    Tuple,      // Tuple data (e.g., Option::Some(T))
    Struct      // Struct data (e.g., Result::Err { code, message })
};

// =============================================================================
//                    Enum Variant
// =============================================================================

/**
 * @class EnumVariant
 * @brief A variant in an enum type
 */
class EnumVariant {
public:
    /**
     * @brief Create a unit variant
     */
    static EnumVariantPtr createUnit(const std::string& name, int64_t discriminant);
    
    /**
     * @brief Create a tuple variant
     */
    static EnumVariantPtr createTuple(const std::string& name, int64_t discriminant,
                                       const std::vector<TypePtr>& fieldTypes);
    
    /**
     * @brief Create a struct variant
     */
    static EnumVariantPtr createStruct(const std::string& name, int64_t discriminant,
                                        const std::vector<std::pair<std::string, TypePtr>>& fields);
    
    // ==========================================================================
    //                    Variant Properties
    // ==========================================================================
    
    const std::string& getName() const { return name_; }
    VariantKind getKind() const { return kind_; }
    int64_t getDiscriminant() const { return discriminant_; }
    
    /**
     * @brief Get tuple field types (for Tuple variants)
     */
    const std::vector<TypePtr>& getTupleTypes() const { return tupleTypes_; }
    
    /**
     * @brief Get struct fields (for Struct variants)
     */
    const std::vector<std::pair<std::string, TypePtr>>& getStructFields() const {
        return structFields_;
    }
    
    /**
     * @brief Check if variant has data
     */
    bool hasData() const { return kind_ != VariantKind::Unit; }
    
    /**
     * @brief Get variant size in bytes
     */
    size_t getDataSize() const;

private:
    EnumVariant(const std::string& name, VariantKind kind, int64_t discriminant);
    
    std::string name_;
    VariantKind kind_;
    int64_t discriminant_;
    std::vector<TypePtr> tupleTypes_;
    std::vector<std::pair<std::string, TypePtr>> structFields_;
};

// =============================================================================
//                    Enum Type
// =============================================================================

/**
 * @enum EnumRepr
 * @brief Discriminant representation
 */
enum class EnumRepr {
    Auto,   // Compiler-chosen
    I8,     // i8
    I16,    // i16
    I32,    // i32
    I64,    // i64
    U8,     // u8
    U16,    // u16
    U32,    // u32
    U64     // u64
};

/**
 * @class EnumType
 * @brief Represents an enum type in Sad
 */
class EnumType : public std::enable_shared_from_this<EnumType> {
public:
    /**
     * @brief Create an enum type
     */
    EnumType(const std::string& arabicName, const std::string& englishName);
    
    // ==========================================================================
    //                    Identification
    // ==========================================================================
    
    const std::string& getArabicName() const { return arabicName_; }
    const std::string& getEnglishName() const { return englishName_; }
    std::string getName(bool arabic = true) const;
    size_t getId() const { return id_; }
    
    // ==========================================================================
    //                    Variants
    // ==========================================================================
    
    /**
     * @brief Add a variant
     */
    void addVariant(EnumVariantPtr variant);
    
    /**
     * @brief Add a unit variant (no data)
     */
    void addUnitVariant(const std::string& name);
    
    /**
     * @brief Add a tuple variant
     */
    void addTupleVariant(const std::string& name, const std::vector<TypePtr>& types);
    
    /**
     * @brief Add a struct variant
     */
    void addStructVariant(const std::string& name,
                          const std::vector<std::pair<std::string, TypePtr>>& fields);
    
    /**
     * @brief Get all variants
     */
    const std::vector<EnumVariantPtr>& getVariants() const { return variants_; }
    
    /**
     * @brief Find a variant by name
     */
    EnumVariantPtr findVariant(const std::string& name) const;
    
    /**
     * @brief Get variant count
     */
    size_t getVariantCount() const { return variants_.size(); }
    
    // ==========================================================================
    //                    Generics
    // ==========================================================================
    
    /**
     * @brief Add a generic type parameter
     */
    void addTypeParameter(GenericParameterPtr param);
    
    /**
     * @brief Get type parameters
     */
    const std::vector<GenericParameterPtr>& getTypeParameters() const {
        return typeParameters_;
    }
    
    /**
     * @brief Check if enum is generic
     */
    bool isGeneric() const { return !typeParameters_.empty(); }
    
    /**
     * @brief Instantiate with concrete types
     */
    EnumTypePtr instantiate(const std::vector<TypePtr>& typeArgs) const;
    
    // ==========================================================================
    //                    Properties
    // ==========================================================================
    
    /**
     * @brief Get representation
     */
    EnumRepr getRepr() const { return repr_; }
    void setRepr(EnumRepr repr) { repr_ = repr; }
    
    /**
     * @brief Check if all variants are unit (C-style enum)
     */
    bool isCLike() const;
    
    /**
     * @brief Check if enum is Option-like (None + Some(T))
     */
    bool isOptionLike() const;
    
    /**
     * @brief Check if enum is Result-like (Ok(T) + Err(E))
     */
    bool isResultLike() const;
    
    /**
     * @brief Get size in bytes
     */
    size_t getSizeInBytes() const;
    
    /**
     * @brief Get discriminant size
     */
    size_t getDiscriminantSize() const;

private:
    std::string arabicName_;
    std::string englishName_;
    size_t id_;
    std::vector<EnumVariantPtr> variants_;
    std::vector<GenericParameterPtr> typeParameters_;
    EnumRepr repr_ = EnumRepr::Auto;
    int64_t nextDiscriminant_ = 0;
    
    static size_t nextId_;
};

// =============================================================================
//                    Enum Registry
// =============================================================================

/**
 * @class EnumRegistry
 * @brief Registry for enum types (Singleton)
 */
class EnumRegistry {
public:
    /**
     * @brief Get the singleton instance
     */
    static EnumRegistry& instance();
    
    // ==========================================================================
    //                    Registration
    // ==========================================================================
    
    /**
     * @brief Register an enum type
     */
    void registerEnum(EnumTypePtr enumType);
    
    /**
     * @brief Find an enum by name
     */
    EnumTypePtr findEnum(const std::string& name) const;
    
    /**
     * @brief Get all registered enums
     */
    std::vector<EnumTypePtr> getAllEnums() const;
    
    /**
     * @brief Check if a name is an enum type
     */
    bool isEnumType(const std::string& name) const;
    
    // ==========================================================================
    //                    Core Enums
    // ==========================================================================
    
    /**
     * @brief Get Option<T> type
     */
    EnumTypePtr getOptionType() const { return optionType_; }
    
    /**
     * @brief Get Result<T, E> type
     */
    EnumTypePtr getResultType() const { return resultType_; }
    
    /**
     * @brief Initialize core enums
     */
    void initializeCoreEnums();

private:
    EnumRegistry();
    
    std::unordered_map<std::string, EnumTypePtr> enumsByName_;
    EnumTypePtr optionType_;
    EnumTypePtr resultType_;
};

// =============================================================================
//                    Enum Builder (Fluent API)
// =============================================================================

/**
 * @class EnumBuilder
 * @brief Builder for creating enum types
 */
class EnumBuilder {
public:
    /**
     * @brief Start building an enum
     */
    EnumBuilder(const std::string& arabicName, const std::string& englishName);
    
    /**
     * @brief Add a unit variant
     */
    EnumBuilder& unit(const std::string& name);
    
    /**
     * @brief Add a tuple variant
     */
    EnumBuilder& tuple(const std::string& name, std::initializer_list<TypePtr> types);
    
    /**
     * @brief Add a struct variant
     */
    EnumBuilder& structVariant(const std::string& name,
                               std::initializer_list<std::pair<std::string, TypePtr>> fields);
    
    /**
     * @brief Add a generic parameter
     */
    EnumBuilder& generic(const std::string& paramName);
    
    /**
     * @brief Set representation
     */
    EnumBuilder& repr(EnumRepr r);
    
    /**
     * @brief Build and register the enum
     */
    EnumTypePtr build();

private:
    EnumTypePtr enum_;
    size_t genericIndex_ = 0;
};

// =============================================================================
//                    Arabic Enum Keywords
// =============================================================================

namespace ArabicEnumKeywords {
    extern const char* const TA3DAD;    // UTF-8 for تعداد
    extern const char* const WUHDA;     // UTF-8 for وحدة (unit)
    extern const char* const QEEMA;     // UTF-8 for قيمة (value)
    extern const char* const IKHTYARI;  // UTF-8 for اختياري (Option)
    extern const char* const LA_SHAY;   // UTF-8 for لا_شيء (None)
    extern const char* const BA3D;      // UTF-8 for بعض (Some)
    extern const char* const NATEEJA;   // UTF-8 for نتيجة (Result)
    extern const char* const NAJAH;     // UTF-8 for نجاح (Ok)
    extern const char* const KHATA;     // UTF-8 for خطأ (Err)
}

// =============================================================================
//                    Helper Functions
// =============================================================================

/**
 * @brief Create Option<T> type
 */
EnumTypePtr createOptionType();

/**
 * @brief Create Result<T, E> type
 */
EnumTypePtr createResultType();

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_TYPES_ENUM_TYPES_H
