// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file arabic_types.h
 * @brief Arabic Type System Definitions
 * 
 * This file provides the mapping between Arabic type names and C++ types.
 * Used by the Sad language compiler for Arabic-first type support.
 * 
 * Arabic Type Names:
 * - Unsigned integers: ha8, ha16, ha32, ha64
 * - Signed integers: ain8, ain16, ain32, ain64
 * - Float types: aashri32, aashri64
 * - Other: mantiqy (bool), harf (char), raqm (int)
 * 
 * @author Sad Language Team
 * @date January 2026
 * @version 1.0
 */

#ifndef SAD_TYPES_ARABIC_TYPES_H
#define SAD_TYPES_ARABIC_TYPES_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>

namespace Sad {
namespace TypeSystem {

// =============================================================================
//                    C++ Type Aliases for Arabic Types
// =============================================================================

// Unsigned Integers
using ha8_t   = uint8_t;    // ح8
using ha16_t  = uint16_t;   // ح16
using ha32_t  = uint32_t;   // ح32
using ha64_t  = uint64_t;   // ح64

// Signed Integers
using ain8_t  = int8_t;     // ع8
using ain16_t = int16_t;    // ع16
using ain32_t = int32_t;    // ع32
using ain64_t = int64_t;    // ع64

// Default Integer (raqm = int64)
using raqm_t  = int64_t;    // رقم

// Floating Point
using aashri32_t = float;   // عش32
using aashri64_t = double;  // عش64
using ashri_t    = double;  // عشري (alias for عش64)

// Other Types
using mantiqy_t = bool;     // منطقي
using harf_t    = char32_t; // حرف (Unicode character)
using byte_t    = uint8_t;  // بايت

// =============================================================================
//                    Primitive Type Kind Enum
// =============================================================================

/**
 * @enum PrimitiveKind
 * @brief All primitive type kinds in Sad language
 */
enum class PrimitiveKind {
    // Invalid/unknown
    Unknown,
    
    // Void type
    Void,
    
    // Never type (for functions that never return)
    Never,
    
    // Boolean
    Bool,
    
    // Character (Unicode)
    Char,
    
    // Byte
    Byte,
    
    // Unsigned integers
    UInt8,
    UInt16,
    UInt32,
    UInt64,
    
    // Signed integers
    Int8,
    Int16,
    Int32,
    Int64,
    
    // Floating point
    Float32,
    Float64,
    
    // Count (for iteration)
    COUNT
};

// =============================================================================
//                    Primitive Type Info Structure
// =============================================================================

/**
 * @struct PrimitiveTypeInfo
 * @brief Information about a primitive type
 */
struct PrimitiveTypeInfo {
    PrimitiveKind kind;
    std::string arabicName;     // Arabic name
    std::string englishName;    // English name
    size_t sizeInBytes;         // Size in bytes
    size_t alignment;           // Alignment
    bool isSigned;              // Is signed?
    bool isInteger;             // Is integer?
    bool isFloating;            // Is floating point?
    bool isNumeric;             // Is numeric?
    
    /**
     * @brief Get the type name in the specified language
     */
    const std::string& getName(bool arabic = true) const {
        return arabic ? arabicName : englishName;
    }
};

// =============================================================================
//                    Primitive Type Registry
// =============================================================================

/**
 * @class PrimitiveTypeRegistry
 * @brief Registry for primitive type information (Singleton)
 */
class PrimitiveTypeRegistry {
public:
    /**
     * @brief Get the singleton instance
     */
    static PrimitiveTypeRegistry& instance();
    
    // ==========================================================================
    //                    Type Information Access
    // ==========================================================================
    
    /**
     * @brief Get type info by kind
     */
    const PrimitiveTypeInfo* getInfo(PrimitiveKind kind) const;
    
    /**
     * @brief Get type info by Arabic name
     */
    const PrimitiveTypeInfo* getInfoByArabicName(const std::string& name) const;
    
    /**
     * @brief Get type info by English name
     */
    const PrimitiveTypeInfo* getInfoByEnglishName(const std::string& name) const;
    
    /**
     * @brief Check if a name is a primitive type name
     */
    bool isPrimitiveName(const std::string& name) const;
    
    /**
     * @brief Get kind by name (Arabic or English)
     */
    PrimitiveKind getKindByName(const std::string& name) const;
    
    /**
     * @brief Get all primitive types
     */
    const std::vector<PrimitiveTypeInfo>& getAllTypes() const { return types_; }
    
    // ==========================================================================
    //                    Type Conversion
    // ==========================================================================
    
    /**
     * @brief Get the Arabic name for a kind
     */
    static std::string primitiveKindToArabic(PrimitiveKind kind);
    
    /**
     * @brief Get the English name for a kind
     */
    static std::string primitiveKindToEnglish(PrimitiveKind kind);
    
    /**
     * @brief Get size in bytes for a kind
     */
    static size_t primitiveKindSizeInBytes(PrimitiveKind kind);
    
    // ==========================================================================
    //                    Type Compatibility
    // ==========================================================================
    
    /**
     * @brief Check if implicit conversion is safe
     * @return true if from can be safely converted to to
     */
    static bool isImplicitConversionSafe(PrimitiveKind from, PrimitiveKind to);
    
    /**
     * @brief Get the wider type for binary operations
     */
    static PrimitiveKind getWiderType(PrimitiveKind a, PrimitiveKind b);

private:
    PrimitiveTypeRegistry();
    void initializeTypes();
    
    std::vector<PrimitiveTypeInfo> types_;
    std::unordered_map<std::string, size_t> arabicNameIndex_;
    std::unordered_map<std::string, size_t> englishNameIndex_;
};

// =============================================================================
//                    Arabic Type Name Constants (UTF-8)
// =============================================================================

namespace ArabicTypeNames {
    // Unsigned integers
    extern const char* const HA8;       // Arabic UTF-8 for ح8
    extern const char* const HA16;      // Arabic UTF-8 for ح16
    extern const char* const HA32;      // Arabic UTF-8 for ح32
    extern const char* const HA64;      // Arabic UTF-8 for ح64
    
    // Signed integers
    extern const char* const AIN8;      // Arabic UTF-8 for ع8
    extern const char* const AIN16;     // Arabic UTF-8 for ع16
    extern const char* const AIN32;     // Arabic UTF-8 for ع32
    extern const char* const AIN64;     // Arabic UTF-8 for ع64
    extern const char* const RAQM;      // Arabic UTF-8 for رقم
    
    // Floating point
    extern const char* const AASHRI32;  // Arabic UTF-8 for عش32
    extern const char* const AASHRI64;  // Arabic UTF-8 for عش64
    extern const char* const ASHRI;     // Arabic UTF-8 for عشري
    
    // Other
    extern const char* const MANTIQY;   // Arabic UTF-8 for منطقي
    extern const char* const HARF;      // Arabic UTF-8 for حرف
    extern const char* const TABEEI8;   // Arabic UTF-8 for طبيعي8
    extern const char* const FARAGH;    // Arabic UTF-8 for فراغ
    extern const char* const ABADAN;    // Arabic UTF-8 for أبداً
}

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_TYPES_ARABIC_TYPES_H
