// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// …„: type.cpp
// File: type.cpp
//
// ״§„ˆ״µ: ״×†״° ״§„†ˆ״¹ ״§„״£״³״§״³ ״§„…״¬״±״¯ „†״¸״§… ״§„״£†ˆ״§״¹
// Description: Implementation of the base abstract type for the type system
//
// ״§„…״₪„: Sad Language Type System
// Author: Sad Language Type System
//
// ״§„״×״§״±״®: 2 †״§״± 2026
// Date: January 2, 2026
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

#include "types/type.h"  // ״§״³״×״±״§״¯ ״×״¹״± ״§„״µ† / Import class definition
#include <algorithm>          // „€ std::equal / For std::equal

namespace Sad {
namespace TypeSystem {

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// ״×†״° ״§„…†״´״¦ / Constructor Implementation
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

// ״§„…†״´״¦ …״¹ †ˆ״¹ …״­״¯״¯ / Constructor with specific kind
Type::Type(TypeKind kind) : kind_(kind) {
    // ״×‡״¦״© †ˆ״¹ ״§„†ˆ״¹ / Initialize type kind
}

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// ״×†״° ״¹„״§‚״§״× ״§„״£†ˆ״§״¹ / Type Relations Implementation
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

// ״§„״×״­‚‚ …† ״¥…ƒ״§†״© ״×״­ˆ„ ‡״°״§ ״§„†ˆ״¹ ״¥„‰ †ˆ״¹ ״¢״®״± / Check if assignable to another type
bool Type::isAssignableTo(const Type* other) const {
    // ״§„״×״­‚‚ …† null pointer / Check for null pointer
    if (other == nullptr) {
        return false; // „״§ …ƒ† ״§„״×״­ˆ„ ״¥„‰ nullptr / Cannot assign to nullptr
    }
    
    // ״§„״×״­‚‚ …† ״§„״×״³״§ˆ ״§„…״¨״§״´״± / Check direct equality
    if (equals(other)) {
        return true; // †״³ ״§„†ˆ״¹ / Same type
    }
    
    // Any ‚״¨„ ״£ †ˆ״¹ / Any accepts any type
    if (other->isAny()) {
        return true; // …ƒ† ״×״­ˆ„ ״£ ״´״¡ ״¥„‰ Any / Can assign anything to Any
    }
    
    // „״§ …ƒ† ״×״­ˆ„ ״´״¡ ״¥„‰ Never / Nothing can be assigned to Never
    if (other->isNever()) {
        return false; // Never „״§ ‚״¨„ ״£ ‚…״© / Never accepts no values
    }
    
    // Never …ƒ† ״×״­ˆ„‡ ״¥„‰ ״£ ״´״¡ / Never can be assigned to anything
    if (isNever()) {
        return true; // Never subtype „ƒ„ ״§„״£†ˆ״§״¹ / Never is subtype of all types
    }
    
    // Unknown …ƒ† ״×״­ˆ„‡ ‚״· ״¥„‰ Any / Unknown can only be assigned to Any
    if (isUnknown()) {
        return other->isAny(); // Unknown -> Any ‚״· / Unknown -> Any only
    }
    
    // ״§„״×״­‚‚ …† ״§„״£״±‚״§…: Integer …ƒ† ״×״­ˆ„‡ ״¥„‰ Float / Numeric: Integer -> Float
    if (isInteger() && other->isFloat()) {
        return true; // ״±‚… ״µ״­״­ …ƒ† ״×״­ˆ„‡ ״¥„‰ ״¹״´״± / Integer can be assigned to float
    }
    
    //  ״§„״­״§„״§״× ״§„״£״®״±‰״ ״§״³״×״®״¯… subtyping / For other cases, use subtyping
    return isSubtypeOf(other);
}

// ״§„״×״­‚‚ …† ״¥…ƒ״§†״© ״§״³״×״¨״¯״§„ ‡״°״§ ״§„†ˆ״¹ ״¨†ˆ״¹ ״¢״®״± / Check if this is a subtype of another
bool Type::isSubtypeOf(const Type* other) const {
    // ״§„״×״­‚‚ …† null pointer / Check for null pointer
    if (other == nullptr) {
        return false; // „״§ …ƒ† ״£† ƒˆ† subtype …† nullptr / Cannot be subtype of nullptr
    }
    
    // ״§„״×״­‚‚ …† ״§„״×״³״§ˆ / Check equality
    if (equals(other)) {
        return true; // †ˆ״¹ ‡ˆ subtype …† †״³‡ / Type is subtype of itself
    }
    
    // Never subtype „ƒ„ ״§„״£†ˆ״§״¹ / Never is subtype of all types
    if (isNever()) {
        return true; // Never  ״£״³„ hierarchy / Never at bottom of hierarchy
    }
    
    // ƒ„ ״§„״£†ˆ״§״¹ subtype …† Any / All types are subtypes of Any
    if (other->isAny()) {
        return true; // Any  ״£״¹„‰ hierarchy / Any at top of hierarchy
    }
    
    // ״§„‚…״© ״§„״§״×״±״§״¶״©: false / Default: false
    // ״§„״£†ˆ״§״¹ ״§„…״­״¯״¯״© ״×״¬״§ˆ״² ‡״°‡ ״§„״¯״§„״© „„‚ˆ״§״¹״¯ ״§„״®״§״µ״© / Specific types override for custom rules
    return false;
}

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// ״×†״° ״§„״¯ˆ״§„ ״§„…״³״§״¹״¯״© / Helper Functions Implementation
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

// ״×״­ˆ„ TypeKind ״¥„‰ †״µ ״¹״±״¨ / Convert TypeKind to Arabic text
std::string typeKindToArabic(TypeKind kind) {
    // ״§״³״×״®״¯״§… switch „„״×״­ˆ„ / Use switch for conversion
    switch (kind) {
        // ״§„״£†ˆ״§״¹ ״§„״¨״¯״§״¦״© / Primitive Types
        case TypeKind::Void:
            return "״±״§״÷"; // Void
        case TypeKind::Integer:
            return "״±‚…"; // Integer
        case TypeKind::Float:
            return "״¹״´״±"; // Float
        case TypeKind::Boolean:
            return "…†״·‚"; // Boolean
        case TypeKind::String:
            return "†״µ"; // String
        
        // ״§„״£†ˆ״§״¹ ״§„…״±ƒ״¨״© / Composite Types
        case TypeKind::Array:
            return "…״µˆ״©"; // Array
        case TypeKind::Dictionary:
            return "‚״§…ˆ״³"; // Dictionary
        case TypeKind::Tuple:
            return "״µ"; // Tuple
        
        // ״§„״£†ˆ״§״¹ ״§„ˆ״¸״© / Function Types
        case TypeKind::Function:
            return "״¯״§„״©"; // Function
        
        // ״§„״£†ˆ״§״¹ ״§„ƒ״§״¦†״© / Object-Oriented Types
        case TypeKind::Class:
            return "״µ†"; // Class
        case TypeKind::Interface:
            return "ˆ״§״¬‡״©"; // Interface
        
        // ״§„״£†ˆ״§״¹ ״§„…״×‚״¯…״© / Advanced Types
        case TypeKind::Generic:
            return "†ˆ״¹_״¹״§…"; // Generic
        case TypeKind::TypeParameter:
            return "…״¹״§…„_†ˆ״¹"; // Type Parameter
        case TypeKind::Union:
            return "״§״×״­״§״¯"; // Union
        case TypeKind::Intersection:
            return "״×‚״§״·״¹"; // Intersection
        case TypeKind::Optional:
            return "״§״®״×״§״±"; // Optional
        
        // ״£†ˆ״§״¹ ״®״§״µ״© / Special Types
        case TypeKind::Any:
            return "״£"; // Any
        case TypeKind::Never:
            return "״£״¨״¯״§‹"; // Never
        case TypeKind::Unknown:
            return "…״¬‡ˆ„"; // Unknown
        case TypeKind::Error:
            return "״®״·״£"; // Error
        
        // ‚…״© ״§״×״±״§״¶״© („״§ †״¨״÷ ״§„ˆ״µˆ„) / Default (should not reach)
        default:
            return "†ˆ״¹_״÷״±_…״¹״±ˆ"; // Unknown type
    }
}

// ״×״­ˆ„ TypeKind ״¥„‰ †״µ ״¥†״¬„״² / Convert TypeKind to English text
std::string typeKindToEnglish(TypeKind kind) {
    // ״§״³״×״®״¯״§… switch „„״×״­ˆ„ / Use switch for conversion
    switch (kind) {
        // ״§„״£†ˆ״§״¹ ״§„״¨״¯״§״¦״© / Primitive Types
        case TypeKind::Void:
            return "Void";
        case TypeKind::Integer:
            return "Integer";
        case TypeKind::Float:
            return "Float";
        case TypeKind::Boolean:
            return "Boolean";
        case TypeKind::String:
            return "String";
        
        // ״§„״£†ˆ״§״¹ ״§„…״±ƒ״¨״© / Composite Types
        case TypeKind::Array:
            return "Array";
        case TypeKind::Dictionary:
            return "Dictionary";
        case TypeKind::Tuple:
            return "Tuple";
        
        // ״§„״£†ˆ״§״¹ ״§„ˆ״¸״© / Function Types
        case TypeKind::Function:
            return "Function";
        
        // ״§„״£†ˆ״§״¹ ״§„ƒ״§״¦†״© / Object-Oriented Types
        case TypeKind::Class:
            return "Class";
        case TypeKind::Interface:
            return "Interface";
        
        // ״§„״£†ˆ״§״¹ ״§„…״×‚״¯…״© / Advanced Types
        case TypeKind::Generic:
            return "Generic";
        case TypeKind::TypeParameter:
            return "TypeParameter";
        case TypeKind::Union:
            return "Union";
        case TypeKind::Intersection:
            return "Intersection";
        case TypeKind::Optional:
            return "Optional";
        
        // ״£†ˆ״§״¹ ״®״§״µ״© / Special Types
        case TypeKind::Any:
            return "Any";
        case TypeKind::Never:
            return "Never";
        case TypeKind::Unknown:
            return "Unknown";
        case TypeKind::Error:
            return "Error";
        
        // ‚…״© ״§״×״±״§״¶״© / Default
        default:
            return "UnknownType";
    }
}

// ״§„״×״­‚‚ …† ״×״³״§ˆ †ˆ״¹† / Check if two types are equal
bool typesEqual(const TypePtr& a, const TypePtr& b) {
    // ״§„״×״­‚‚ …† null pointers / Check for null pointers
    if (!a && !b) {
        return true; // ƒ„״§‡…״§ null / Both null
    }
    
    if (!a || !b) {
        return false; // ˆ״§״­״¯ null ˆ״§„״¢״®״± „״³ ƒ״°„ƒ / One is null, other is not
    }
    
    // ״§״³״×״®״¯״§… ״¯״§„״© equals / Use equals method
    return a->equals(b.get());
}

// ״§„״×״­‚‚ …† ״×״³״§ˆ ‚ˆ״§״¦… ״§„״£†ˆ״§״¹ / Check if two type lists are equal
bool typeListsEqual(const TypeList& a, const TypeList& b) {
    // ״§„״×״­‚‚ …† ״§„״£״­״¬״§… ״£ˆ„״§‹ / Check sizes first
    if (a.size() != b.size()) {
        return false; // ״£״­״¬״§… …״®״×„״© / Different sizes
    }
    
    // ״§„…‚״§״±†״© ״¹†״µ״± ״¨״¹†״µ״± / Compare element by element
    for (size_t i = 0; i < a.size(); ++i) {
        // ״§״³״×״®״¯״§… typesEqual „„…‚״§״±†״© / Use typesEqual for comparison
        if (!typesEqual(a[i], b[i])) {
            return false; // ״¹†״µ״± …״®״×„ / Different element
        }
    }
    
    // ״¬…״¹ ״§„״¹†״§״µ״± …״×״³״§ˆ״© / All elements equal
    return true;
}

} // namespace TypeSystem
} // namespace Sad

