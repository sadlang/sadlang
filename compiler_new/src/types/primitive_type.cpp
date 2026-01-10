// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// …„: primitive_type.cpp
// File: primitive_type.cpp
//
// ״§„ˆ״µ: ״×†״° ״§„״£†ˆ״§״¹ ״§„״¨״¯״§״¦״© (״±‚…״ †״µ״ …†״·‚״ ...)
// Description: Implementation of primitive types (int, string, bool, ...)
//
// ״§„…״₪„: Sad Language Type System
// Author: Sad Language Type System
//
// ״§„״×״§״±״®: 2 †״§״± 2026
// Date: January 2, 2026
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

#include "types/primitive_type.h"  // ״§״³״×״±״§״¯ ״§„״×״¹״± / Import definition
#include <stdexcept>                    // „€ std::invalid_argument / For invalid_argument

namespace Sad {
namespace TypeSystem {

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// ״×†״° ״§„…†״´״¦ / Constructor Implementation
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

// ״§„…†״´״¦ …״¹ †ˆ״¹ …״­״¯״¯ / Constructor with specific kind
PrimitiveType::PrimitiveType(TypeKind kind) : Type(kind) {
    // ״§„״×״­‚‚ …† ״£† ״§„†ˆ״¹ …״³…ˆ״­ ״¨‡ / Verify that kind is allowed
    // ״§„״£†ˆ״§״¹ ״§„…״³…ˆ״­ ״¨‡״§: Void, Integer, Float, Boolean, String, Any, Never, Unknown
    // Allowed types: Void, Integer, Float, Boolean, String, Any, Never, Unknown
    bool isValidType = isPrimitive() || 
                      kind == TypeKind::Void || 
                      kind == TypeKind::Any || 
                      kind == TypeKind::Never || 
                      kind == TypeKind::Unknown;
    
    if (!isValidType) {
        // ״±… ״®״·״£ ״¥״°״§ „… ƒ† †ˆ״¹״§‹ …״³…ˆ״­״§‹ / Throw error if not allowed type
        throw std::invalid_argument("PrimitiveType: †ˆ״¹ ״÷״± ״¨״¯״§״¦ / non-primitive kind");
    }
}

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// ״×†״° ״§„״¯ˆ״§„ ״§„״§״×״±״§״¶״© / Virtual Functions Implementation
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

// ״§„״­״µˆ„ ״¹„‰ ״§״³… ״§„†ˆ״¹ ״¨״§„״¹״±״¨״© / Get type name in Arabic
std::string PrimitiveType::getArabicName() const {
    // ״§״³״×״®״¯״§… ״§„״¯״§„״© ״§„…״³״§״¹״¯״© / Use helper function
    return typeKindToArabic(getKind());
}

// ״§„״­״µˆ„ ״¹„‰ ״§״³… ״§„†ˆ״¹ ״¨״§„״¥†״¬„״²״© / Get type name in English
std::string PrimitiveType::getEnglishName() const {
    // ״§״³״×״®״¯״§… ״§„״¯״§„״© ״§„…״³״§״¹״¯״© / Use helper function
    return typeKindToEnglish(getKind());
}

// ״×״­ˆ„ ״§„†ˆ״¹ ״¥„‰ †״µ ״×…״«„ / Convert type to string representation
std::string PrimitiveType::toString() const {
    // ״¥״±״¬״§״¹ ״§„״§״³… ״§„״¹״±״¨ / Return Arabic name
    return getArabicName();
}

// ״§„״×״­‚‚ …† ״§„״×״³״§ˆ / Check equality
bool PrimitiveType::equals(const Type* other) const {
    // ״§„״×״­‚‚ …† null / Check for null
    if (other == nullptr) {
        return false; // null „״§ ״³״§ˆ ״´״¦״§‹ / null equals nothing
    }
    
    // ״§„״×״­‚‚ …† †״³ ״§„†ˆ״¹ / Check same kind
    return getKind() == other->getKind();
}

// ״§״³״×†״³״§״® ״§„†ˆ״¹ / Clone the type
std::shared_ptr<Type> PrimitiveType::clone() const {
    // ״¥†״´״§״¡ †״³״®״© ״¬״¯״¯״© / Create new copy
    return std::make_shared<PrimitiveType>(getKind());
}

// ״§„״­״µˆ„ ״¹„‰ ״­״¬… ״§„†ˆ״¹ ״¨״§„״¨״§״×״§״× / Get type size in bytes
size_t PrimitiveType::getSizeInBytes() const {
    // ״­״³״§״¨ ״§„״­״¬… ״¨†״§״¡ ״¹„‰ ״§„†ˆ״¹ / Calculate size based on kind
    switch (getKind()) {
        case TypeKind::Void:
            return 0;  // ״±״§״÷ „״§ ״­״¬… „‡ / Void has no size
        
        case TypeKind::Boolean:
            return 1;  // …†״·‚ = 1 ״¨״§״× / Boolean = 1 byte
        
        case TypeKind::Integer:
            return 8;  // ״±‚… ״µ״­״­ = 8 ״¨״§״× (64-bit) / Integer = 8 bytes (64-bit)
        
        case TypeKind::Float:
            return 8;  // ״¹״´״± = 8 ״¨״§״× (double) / Float = 8 bytes (double)
        
        case TypeKind::String:
            return sizeof(void*);  // †״µ = ״­״¬… …״₪״´״± / String = pointer size
        
        // ״§„״£†ˆ״§״¹ ״§„״®״§״µ״© / Special types
        case TypeKind::Any:
        case TypeKind::Never:
        case TypeKind::Unknown:
            return sizeof(void*);  // ״­״¬… …״₪״´״± „„״£†ˆ״§״¹ ״§„״®״§״µ״© / Pointer size for special types
        
        default:
            return 0;  // ‚…״© ״§״×״±״§״¶״© / Default value
    }
}

// ״§„״­״µˆ„ ״¹„‰ …״­״§״°״§״© ״§„†ˆ״¹ / Get type alignment
size_t PrimitiveType::getAlignment() const {
    // ״§„…״­״§״°״§״© ״¹״§״¯״© ״×״³״§ˆ ״§„״­״¬… / Alignment usually equals size
    // „„״£†ˆ״§״¹ ״§„״µ״÷״±״© / For small types
    
    switch (getKind()) {
        case TypeKind::Void:
            return 1;  // ״±״§״÷: …״­״§״°״§״© 1 / Void: alignment 1
        
        case TypeKind::Boolean:
            return 1;  // …†״·‚: …״­״§״°״§״© 1 / Boolean: alignment 1
        
        case TypeKind::Integer:
            return 8;  // ״±‚…: …״­״§״°״§״© 8 / Integer: alignment 8
        
        case TypeKind::Float:
            return 8;  // ״¹״´״±: …״­״§״°״§״© 8 / Float: alignment 8
        
        case TypeKind::String:
            return alignof(void*);  // †״µ: …״­״§״°״§״© ״§„…״₪״´״± / String: pointer alignment
        
        // ״§„״£†ˆ״§״¹ ״§„״®״§״µ״© / Special types
        case TypeKind::Any:
        case TypeKind::Never:
        case TypeKind::Unknown:
            return alignof(void*);  // …״­״§״°״§״© ״§„…״₪״´״± „„״£†ˆ״§״¹ ״§„״®״§״µ״© / Pointer alignment for special types
        
        default:
            return 1;  // ‚…״© ״§״×״±״§״¶״© / Default value
    }
}

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// ״×†״° ״¯ˆ״§„ ״§„…״µ†״¹ / Factory Functions Implementation
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

// ״¥†״´״§״¡ †ˆ״¹ ״±״§״÷ / Create Void type
TypePtr createVoidType() {
    // ״¥†״´״§״¡ ƒ״§״¦† PrimitiveType …״¹ †ˆ״¹ Void / Create PrimitiveType object with Void kind
    return std::make_shared<PrimitiveType>(TypeKind::Void);
}

// ״¥†״´״§״¡ †ˆ״¹ ״±‚… ״µ״­״­ / Create Integer type
TypePtr createIntegerType() {
    // ״¥†״´״§״¡ ƒ״§״¦† PrimitiveType …״¹ †ˆ״¹ Integer / Create PrimitiveType object with Integer kind
    return std::make_shared<PrimitiveType>(TypeKind::Integer);
}

// ״¥†״´״§״¡ †ˆ״¹ ״¹״´״± / Create Float type
TypePtr createFloatType() {
    // ״¥†״´״§״¡ ƒ״§״¦† PrimitiveType …״¹ †ˆ״¹ Float / Create PrimitiveType object with Float kind
    return std::make_shared<PrimitiveType>(TypeKind::Float);
}

// ״¥†״´״§״¡ †ˆ״¹ …†״·‚ / Create Boolean type
TypePtr createBooleanType() {
    // ״¥†״´״§״¡ ƒ״§״¦† PrimitiveType …״¹ †ˆ״¹ Boolean / Create PrimitiveType object with Boolean kind
    return std::make_shared<PrimitiveType>(TypeKind::Boolean);
}

// ״¥†״´״§״¡ †ˆ״¹ †״µ / Create String type
TypePtr createStringType() {
    // ״¥†״´״§״¡ ƒ״§״¦† PrimitiveType …״¹ †ˆ״¹ String / Create PrimitiveType object with String kind
    return std::make_shared<PrimitiveType>(TypeKind::String);
}

// ״¥†״´״§״¡ †ˆ״¹ ״¨״¯״§״¦ …† TypeKind / Create primitive type from TypeKind
TypePtr createPrimitiveType(TypeKind kind) {
    // ״§״³״×״®״¯״§… switch „״§״³״×״¯״¹״§״¡ ״§„״¯״§„״© ״§„…†״§״³״¨״© / Use switch to call appropriate function
    switch (kind) {
        case TypeKind::Void:
            return createVoidType();  // ״±״§״÷ / Void
        
        case TypeKind::Integer:
            return createIntegerType();  // ״±‚… / Integer
        
        case TypeKind::Float:
            return createFloatType();  // ״¹״´״± / Float
        
        case TypeKind::Boolean:
            return createBooleanType();  // …†״·‚ / Boolean
        
        case TypeKind::String:
            return createStringType();  // †״µ / String
        
        default:
            // ״±… ״®״·״£ „„״£†ˆ״§״¹ ״÷״± ״§„״¨״¯״§״¦״© / Throw error for non-primitive types
            throw std::invalid_argument("createPrimitiveType: †ˆ״¹ ״÷״± ״¨״¯״§״¦ / non-primitive kind");
    }
}

} // namespace TypeSystem
} // namespace Sad

