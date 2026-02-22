// Disable Unicode warning for Arabic strings
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file union_types.h
 * @brief Arabic Union Type System
 * 
 * Union types in Sad language:
 * - itihad (union) - C-style unions
 * - Tagged unions via enums
 * - Unsafe union access
 * 
 * @author Sad Language Team
 * @date January 2026
 * @version 1.0
 */

#ifndef SAD_TYPES_UNION_TYPES_H
#define SAD_TYPES_UNION_TYPES_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>
#include "arabic_types.h"
#include "struct_types.h"

namespace Sad {
namespace TypeSystem {

// Forward declarations
class UnionType;
class UnionField;
class UnionRegistry;

using UnionTypePtr = std::shared_ptr<UnionType>;
using UnionFieldPtr = std::shared_ptr<UnionField>;

// =============================================================================
//                    Union Field
// =============================================================================

/**
 * @class UnionField
 * @brief A field in a union type
 */
class UnionField {
public:
    /**
     * @brief Create a union field
     */
    UnionField(const std::string& name, TypePtr type, size_t index);
    
    const std::string& getName() const { return name_; }
    TypePtr getType() const { return type_; }
    size_t getIndex() const { return index_; }
    
    /**
     * @brief Get field size in bytes
     */
    size_t getSizeInBytes() const;

private:
    std::string name_;
    TypePtr type_;
    size_t index_;
};

// =============================================================================
//                    Union Type
// =============================================================================

/**
 * @class UnionType
 * @brief Represents a union type in Sad (C-style union)
 * 
 * Unions are unsafe - only use in unsafe blocks
 */
class UnionType : public std::enable_shared_from_this<UnionType> {
public:
    /**
     * @brief Create a union type
     */
    UnionType(const std::string& arabicName, const std::string& englishName);
    
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
     * @brief Add a field to the union
     */
    void addField(const std::string& name, TypePtr type);
    
    /**
     * @brief Get all fields
     */
    const std::vector<UnionFieldPtr>& getFields() const { return fields_; }
    
    /**
     * @brief Find a field by name
     */
    UnionFieldPtr findField(const std::string& name) const;
    
    /**
     * @brief Get field count
     */
    size_t getFieldCount() const { return fields_.size(); }
    
    // ==========================================================================
    //                    Layout
    // ==========================================================================
    
    /**
     * @brief Get size in bytes (max of all fields)
     */
    size_t getSizeInBytes() const;
    
    /**
     * @brief Get alignment requirement
     */
    size_t getAlignment() const;
    
    // ==========================================================================
    //                    Safety
    // ==========================================================================
    
    /**
     * @brief Check if accessing union requires unsafe
     */
    bool requiresUnsafe() const { return true; }

private:
    std::string arabicName_;
    std::string englishName_;
    size_t id_;
    std::vector<UnionFieldPtr> fields_;
    
    static size_t nextId_;
};

// =============================================================================
//                    Union Registry
// =============================================================================

/**
 * @class UnionRegistry
 * @brief Registry for union types (Singleton)
 */
class UnionRegistry {
public:
    /**
     * @brief Get the singleton instance
     */
    static UnionRegistry& instance();
    
    /**
     * @brief Register a union type
     */
    void registerUnion(UnionTypePtr unionType);
    
    /**
     * @brief Find a union by name
     */
    UnionTypePtr findUnion(const std::string& name) const;
    
    /**
     * @brief Get all registered unions
     */
    std::vector<UnionTypePtr> getAllUnions() const;
    
    /**
     * @brief Check if a name is a union type
     */
    bool isUnionType(const std::string& name) const;

private:
    UnionRegistry() = default;
    
    std::unordered_map<std::string, UnionTypePtr> unionsByName_;
};

// =============================================================================
//                    Union Builder (Fluent API)
// =============================================================================

/**
 * @class UnionBuilder
 * @brief Builder for creating union types
 */
class UnionBuilder {
public:
    /**
     * @brief Start building a union
     */
    UnionBuilder(const std::string& arabicName, const std::string& englishName);
    
    /**
     * @brief Add a field
     */
    UnionBuilder& field(const std::string& name, TypePtr type);
    
    /**
     * @brief Build and register the union
     */
    UnionTypePtr build();

private:
    UnionTypePtr union_;
};

// =============================================================================
//                    Arabic Union Keywords
// =============================================================================

namespace ArabicUnionKeywords {
    extern const char* const ITIHAD;    // UTF-8 for اتحاد
}

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_TYPES_UNION_TYPES_H
