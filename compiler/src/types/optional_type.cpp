// ════════════════════════════════════════════════════════════════════════════════
// ملف: optional_type.cpp
// File: optional_type.cpp
//
// الوصف: تنفيذ الأنواع الاختيارية (Optional Types)
// Description: Optional Types implementation
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// المصدر: compiler/type_system/include/optional_type.h
// Source: compiler/type_system/include/optional_type.h
// ════════════════════════════════════════════════════════════════════════════════

#include "optional_type.h"
#include "type_registry.h"

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// المُنشئات / Constructors
// ════════════════════════════════════════════════════════════════════════════════

OptionalType::OptionalType(TypePtr innerType)
    : UnionType(innerType, TypeRegistry::getInstance().getVoidType())  // T | null
    , innerType_(innerType)
{
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال مُعاد تعريفها / Overridden Functions
// ════════════════════════════════════════════════════════════════════════════════

std::string OptionalType::getArabicName() const {
    return innerType_->getArabicName() + "?";
}

std::string OptionalType::getEnglishName() const {
    return innerType_->getEnglishName() + "?";
}

std::string OptionalType::toString() const {
    return innerType_->toString() + "?";
}

std::shared_ptr<Type> OptionalType::clone() const {
    return std::make_shared<OptionalType>(innerType_->clone());
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال الأنواع الاختيارية / Optional Type-Specific Methods
// ════════════════════════════════════════════════════════════════════════════════

TypePtr OptionalType::getInnerType() const {
    return innerType_;
}

bool OptionalType::isPresent() const {
    // هذه دالة مفاهيمية / Conceptual function
    // التحقق الفعلي يتم في runtime / Actual check at runtime
    return true;
}

TypePtr OptionalType::toNonOptional() const {
    return innerType_;
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة / Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

OptionalTypePtr makeOptionalType(TypePtr innerType) {
    if (!innerType) return nullptr;
    
    // تجنب double wrapping / Avoid double wrapping
    if (isOptionalType(innerType)) {
        return asOptionalType(innerType);
    }
    
    return std::make_shared<OptionalType>(innerType);
}

bool isOptionalType(const TypePtr& type) {
    if (!type) return false;
    
    // التحقق المباشر / Direct check
    if (auto optType = std::dynamic_pointer_cast<OptionalType>(type)) {
        return true;
    }
    
    // التحقق من union type / Check if union type
    if (type->getKind() == SadTypeKind::Union) {
        auto unionType = asUnionType(type);
        if (unionType) {
            return isUnionOptional(unionType);
        }
    }
    
    return false;
}

OptionalTypePtr asOptionalType(const TypePtr& type) {
    if (!isOptionalType(type)) {
        return nullptr;
    }
    
    return std::dynamic_pointer_cast<OptionalType>(type);
}

TypePtr unwrapOptional(const TypePtr& type) {
    if (!type) return nullptr;
    
    // إذا كان optional، استخرج النوع الداخلي / If optional, extract inner type
    if (auto optType = asOptionalType(type)) {
        return optType->getInnerType();
    }
    
    // إذا كان union type قد يكون optional / If union might be optional
    if (auto unionType = asUnionType(type)) {
        if (isUnionOptional(unionType)) {
            // استخراج النوع غير null / Extract non-null type
            auto alternatives = unionType->getAlternatives();
            for (const auto& alt : alternatives) {
                if (alt->getKind() != SadTypeKind::Void) {
                    return alt;
                }
            }
        }
    }
    
    // ليس optional، إرجاع نفس النوع / Not optional, return same type
    return type;
}

TypePtr toOptional(const TypePtr& type) {
    if (!type) return nullptr;
    
    // إذا كان بالفعل optional، لا نغلفه مرة أخرى / If already optional, don't wrap again
    if (isOptionalType(type)) {
        return type;
    }
    
    return makeOptionalType(type);
}

bool isUnionOptional(const UnionTypePtr& unionType) {
    if (!unionType) return false;
    
    // optional union يجب أن يكون له بديلان فقط / Optional union must have exactly 2 alternatives
    if (unionType->size() != 2) {
        return false;
    }
    
    // أحدهما يجب أن يكون null / One must be null
    if (!unionType->containsNull()) {
        return false;
    }
    
    // الآخر يجب أن لا يكون null / The other must not be null
    const auto& alternatives = unionType->getAlternatives();
    int nonNullCount = 0;
    
    for (const auto& alt : alternatives) {
        if (alt->getKind() != SadTypeKind::Void) {
            nonNullCount++;
        }
    }
    
    // يجب أن يكون هناك نوع واحد غير null فقط / Must be exactly one non-null type
    return nonNullCount == 1;
}

TypePtr unionToOptional(const UnionTypePtr& unionType) {
    if (!unionType) return nullptr;
    
    // التحقق إذا كان optional / Check if optional
    if (!isUnionOptional(unionType)) {
        return unionType;  // إرجاع نفس الـ union / Return same union
    }
    
    // استخراج النوع غير null / Extract non-null type
    const auto& alternatives = unionType->getAlternatives();
    for (const auto& alt : alternatives) {
        if (alt->getKind() != SadTypeKind::Void) {
            return makeOptionalType(alt);
        }
    }
    
    return unionType;
}

} // namespace TypeSystem
} // namespace Sad
