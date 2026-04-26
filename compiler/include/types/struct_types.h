// Disable Unicode warning for Arabic strings
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file struct_types.h
 * @brief Arabic Struct Type System
 * 
 * Struct types in Sad language:
 * - bunya / heikal (structure)
 * - Named fields with types
 * - Generic struct support
 * - Memory layout control
 * 
 * @author Sad Language Team
 * @date January 2026
 * @version 1.0
 */

#ifndef SAD_TYPES_STRUCT_TYPES_H
#define SAD_TYPES_STRUCT_TYPES_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>
#include "arabic_types.h"
#include "trait_system.h"
#include "generics.h"

namespace Sad {
namespace TypeSystem {

// Forward declarations
class StructType;
class StructField;
class StructRegistry;

using StructTypePtr = std::shared_ptr<StructType>;
using StructFieldPtr = std::shared_ptr<StructField>;

// =============================================================================
//                    Struct Field
// =============================================================================

/**
 * @class StructField
 * @brief A field in a struct type
 */
class StructField {
public:
    /**
     * @brief Create a struct field
     */
    StructField(const std::string& name, TypePtr type, size_t index);
    
    // ==========================================================================
    //                    Field Properties
    // ==========================================================================
    
    const std::string& getName() const { return name_; }
    TypePtr getType() const { return type_; }
    size_t getIndex() const { return index_; }
    
    /**
     * @brief Get the field offset in bytes
     */
    size_t getOffset() const { return offset_; }
    void setOffset(size_t offset) { offset_ = offset; }
    
    /**
     * @brief Check if field is public
     */
    bool isPublic() const { return isPublic_; }
    void setPublic(bool value) { isPublic_ = value; }
    
    /**
     * @brief Check if field is mutable
     */
    bool isMutable() const { return isMutable_; }
    void setMutable(bool value) { isMutable_ = value; }
    
    /**
     * @brief Get default value (if any)
     */
    bool hasDefault() const { return hasDefault_; }
    void setHasDefault(bool value) { hasDefault_ = value; }

private:
    std::string name_;
    TypePtr type_;
    size_t index_;
    size_t offset_ = 0;
    bool isPublic_ = true;
    bool isMutable_ = true;
    bool hasDefault_ = false;
};

// =============================================================================
//                    Struct Type
// =============================================================================

/**
 * @enum StructLayout
 * @brief Memory layout strategy for structs
 */
enum class StructLayout {
    Auto,       // Compiler-determined (default)
    C,          // C-compatible layout
    Packed,     // No padding
    Repr32,     // 32-bit aligned
    Repr64      // 64-bit aligned
};

/**
 * @class StructType
 * @brief Represents a struct type in Sad
 */
class StructType : public std::enable_shared_from_this<StructType> {
public:
    /**
     * @brief Create a struct type
     */
    StructType(const std::string& arabicName, const std::string& englishName);
    
    // ==========================================================================
    //                    Identification
    // ==========================================================================
    
    const std::string& getArabicName() const { return arabicName_; }
    const std::string& getEnglishName() const { return englishName_; }
    std::string getName(bool arabic = true) const;
    size_t getId() const { return id_; }
    
    // ==========================================================================
    //                    Fields
    // ==========================================================================
    
    /**
     * @brief Add a field to the struct
     */
    void addField(const std::string& name, TypePtr type);
    
    /**
     * @brief Get all fields
     */
    const std::vector<StructFieldPtr>& getFields() const { return fields_; }
    
    /**
     * @brief Find a field by name
     */
    StructFieldPtr findField(const std::string& name) const;
    
    /**
     * @brief Get field count
     */
    size_t getFieldCount() const { return fields_.size(); }
    
    /**
     * @brief Check if struct has a field
     */
    bool hasField(const std::string& name) const;
    
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
     * @brief Check if struct is generic
     */
    bool isGeneric() const { return !typeParameters_.empty(); }
    
    /**
     * @brief Instantiate with concrete types
     */
    StructTypePtr instantiate(const std::vector<TypePtr>& typeArgs) const;
    
    // ==========================================================================
    //                    Traits
    // ==========================================================================
    
    /**
     * @brief Add an implemented trait
     */
    void addTrait(TraitPtr trait);
    
    /**
     * @brief Get implemented traits
     */
    const std::vector<TraitPtr>& getTraits() const { return traits_; }
    
    /**
     * @brief Check if struct implements a trait
     */
    bool implementsTrait(TraitPtr trait) const;
    
    // ==========================================================================
    //                    Layout
    // ==========================================================================
    
    /**
     * @brief Get memory layout
     */
    StructLayout getLayout() const { return layout_; }
    void setLayout(StructLayout layout) { layout_ = layout; }
    
    /**
     * @brief Calculate and get size in bytes
     */
    size_t getSizeInBytes() const;
    
    /**
     * @brief Get alignment requirement
     */
    size_t getAlignment() const;
    
    /**
     * @brief Calculate field offsets
     */
    void calculateLayout();
    
    // ==========================================================================
    //                    Properties
    // ==========================================================================
    
    /**
     * @brief Check if struct is copy-able (all fields are Copy)
     */
    bool isCopy() const { return isCopy_; }
    void setCopy(bool value) { isCopy_ = value; }
    
    /**
     * @brief Check if struct is tuple-like
     */
    bool isTuple() const { return isTuple_; }
    void setTuple(bool value) { isTuple_ = value; }

private:
    std::string arabicName_;
    std::string englishName_;
    size_t id_;
    std::vector<StructFieldPtr> fields_;
    std::vector<GenericParameterPtr> typeParameters_;
    std::vector<TraitPtr> traits_;
    StructLayout layout_ = StructLayout::Auto;
    bool isCopy_ = false;
    bool isTuple_ = false;
    mutable size_t cachedSize_ = 0;
    mutable bool layoutCalculated_ = false;
    
    static size_t nextId_;
};

// =============================================================================
//                    Struct Registry
// =============================================================================

/**
 * @class StructRegistry
 * @brief Registry for struct types (Singleton)
 */
class StructRegistry {
public:
    /**
     * @brief Get the singleton instance
     */
    static StructRegistry& instance();
    
    // ==========================================================================
    //                    Registration
    // ==========================================================================
    
    /**
     * @brief Register a struct type
     */
    void registerStruct(StructTypePtr structType);
    
    /**
     * @brief Find a struct by name
     */
    StructTypePtr findStruct(const std::string& name) const;
    
    /**
     * @brief Get all registered structs
     */
    std::vector<StructTypePtr> getAllStructs() const;
    
    /**
     * @brief Check if a name is a struct type
     */
    bool isStructType(const std::string& name) const;
    
    // ==========================================================================
    //                    Instantiation Cache
    // ==========================================================================
    
    /**
     * @brief Get or create a generic instantiation
     */
    StructTypePtr getOrCreateInstantiation(
        StructTypePtr genericStruct,
        const std::vector<TypePtr>& typeArgs);
    
    /**
     * @brief Clear instantiation cache
     */
    void clearInstantiationCache();

private:
    StructRegistry() = default;
    
    std::unordered_map<std::string, StructTypePtr> structsByName_;
    std::unordered_map<std::string, StructTypePtr> instantiations_;
};

// =============================================================================
//                    Struct Builder (Fluent API)
// =============================================================================

/**
 * @class StructBuilder
 * @brief Builder for creating struct types
 */
class StructBuilder {
public:
    /**
     * @brief Start building a struct
     */
    StructBuilder(const std::string& arabicName, const std::string& englishName);
    
    /**
     * @brief Add a field
     */
    StructBuilder& field(const std::string& name, TypePtr type);
    
    /**
     * @brief Add a generic parameter
     */
    StructBuilder& generic(const std::string& paramName);
    
    /**
     * @brief Add a generic parameter with bounds
     */
    StructBuilder& genericBounded(const std::string& paramName, TraitPtr bound);
    
    /**
     * @brief Set layout
     */
    StructBuilder& layout(StructLayout layout);
    
    /**
     * @brief Mark as copy-able
     */
    StructBuilder& copy();
    
    /**
     * @brief Mark as tuple
     */
    StructBuilder& tuple();
    
    /**
     * @brief Build and register the struct
     */
    StructTypePtr build();

private:
    StructTypePtr struct_;
    size_t genericIndex_ = 0;
};

// =============================================================================
//                    Arabic Struct Keywords
// =============================================================================

namespace ArabicStructKeywords {
    extern const char* const BUNYA;     // UTF-8 for بنية
    extern const char* const HEIKAL;    // UTF-8 for هيكل
    extern const char* const HAQLA;     // UTF-8 for حقل (field)
    extern const char* const AAM;       // UTF-8 for عام (public)
    extern const char* const KHASS;     // UTF-8 for خاص (private)
    extern const char* const MUTAHAREK; // UTF-8 for متحرك (mutable)
    extern const char* const THABET;    // UTF-8 for ثابت (const)
}

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_TYPES_STRUCT_TYPES_H
