// ════════════════════════════════════════════════════════════════════════════════
// ملف: type_guard.cpp
// File: type_guard.cpp
//
// الوصف: تنفيذ Type Guards (حراس الأنواع)
// Description: Type Guards implementation
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// Phase: 1.3.5.1 - Type Guards
// ════════════════════════════════════════════════════════════════════════════════

#include "type_guard.h"
#include "union_type.h"
#include "primitive_type.h"
#include "type_registry.h"
#include <sstream>
#include <algorithm>
#include <iostream>

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// TypeGuardResult Implementation
// ════════════════════════════════════════════════════════════════════════════════

TypeGuardResult::TypeGuardResult(bool success, TypePtr narrowedType, const std::string& reason)
    : success_(success)
    , narrowedType_(narrowedType)
    , reason_(reason)
{
}

std::string TypeGuardResult::toString() const {
    std::ostringstream oss;
    if (success_) {
        oss << "نجاح (Success): النوع المضيّق (Narrowed type) = ";
        if (narrowedType_) {
            oss << narrowedType_->toString();
        } else {
            oss << "null";
        }
    } else {
        oss << "فشل (Failure): " << reason_;
    }
    return oss.str();
}

// ════════════════════════════════════════════════════════════════════════════════
// TypeGuard Constructors
// ════════════════════════════════════════════════════════════════════════════════

TypeGuard::TypeGuard(TypeGuardKind kind, const std::string& variableName, TypePtr targetType)
    : kind_(kind)
    , variableName_(variableName)
    , targetType_(targetType)
{
}

TypeGuard::TypeGuard(TypeGuardKind kind, const std::vector<TypeGuardPtr>& guards)
    : kind_(kind)
    , subGuards_(guards)
{
}

TypeGuard::TypeGuard(const std::string& functionName, const std::string& variableName, TypePtr returnType)
    : kind_(TypeGuardKind::UserDefined)
    , variableName_(variableName)
    , targetType_(returnType)
    , functionName_(functionName)
{
}

// ════════════════════════════════════════════════════════════════════════════════
// TypeGuard Core Methods
// ════════════════════════════════════════════════════════════════════════════════

TypeGuardResultPtr TypeGuard::apply(TypePtr currentType) const {
    if (!currentType) {
        return std::make_shared<TypeGuardResult>(
            false, 
            nullptr, 
            "النوع الحالي null (Current type is null)"
        );
    }
    
    // تطبيق الحارس حسب النوع / Apply guard based on kind
    switch (kind_) {
        case TypeGuardKind::IsType: {
            // نضيّق النوع أولاً / Narrow the type first
            TypePtr narrowed = narrow(currentType);
            
            // نتحقق إذا كانت النتيجة Never (فشل) / Check if result is Never (failure)
            auto& registry = TypeRegistry::getInstance();
            if (narrowed && narrowed->equals(registry.getNeverType().get())) {
                return std::make_shared<TypeGuardResult>(
                    false,
                    nullptr,
                    "النوع لا يطابق الحارس (Type doesn't match guard)"
                );
            }
            
            return std::make_shared<TypeGuardResult>(true, narrowed);
        }
        
        case TypeGuardKind::TypeOf: {
            if (matchesTypeOf(currentType)) {
                return std::make_shared<TypeGuardResult>(true, currentType);
            } else {
                return std::make_shared<TypeGuardResult>(
                    false,
                    nullptr,
                    "typeof لا يطابق (typeof doesn't match)"
                );
            }
        }
        
        case TypeGuardKind::InstanceOf: {
            if (matchesInstanceOf(currentType)) {
                return std::make_shared<TypeGuardResult>(true, currentType);
            } else {
                return std::make_shared<TypeGuardResult>(
                    false,
                    nullptr,
                    "instanceof لا يطابق (instanceof doesn't match)"
                );
            }
        }
        
        case TypeGuardKind::Null: {
            if (currentType->getKind() == SadTypeKind::Void) {
                auto& registry = TypeRegistry::getInstance();
                return std::make_shared<TypeGuardResult>(true, registry.getVoidType());
            } else {
                return std::make_shared<TypeGuardResult>(
                    false,
                    nullptr,
                    "القيمة ليست null (Value is not null)"
                );
            }
        }
        
        case TypeGuardKind::NotNull: {
            if (currentType->getKind() != SadTypeKind::Void) {
                // إزالة null من Union إن وُجد / Remove null from Union if present
                TypePtr narrowed = narrow(currentType);
                return std::make_shared<TypeGuardResult>(true, narrowed);
            } else {
                return std::make_shared<TypeGuardResult>(
                    false,
                    nullptr,
                    "القيمة null (Value is null)"
                );
            }
        }
        
        case TypeGuardKind::And: {
            // تطبيق جميع الحراس الفرعية / Apply all sub-guards
            TypePtr result = currentType;
            for (const auto& guard : subGuards_) {
                auto guardResult = guard->apply(result);
                if (!guardResult->isSuccess()) {
                    return guardResult; // فشل أحد الحراس / One guard failed
                }
                result = guardResult->getNarrowedType();
            }
            return std::make_shared<TypeGuardResult>(true, result);
        }
        
        case TypeGuardKind::Or: {
            // نجاح أي حارس يكفي / Success of any guard is enough
            std::vector<TypePtr> narrowedTypes;
            for (const auto& guard : subGuards_) {
                auto guardResult = guard->apply(currentType);
                if (guardResult->isSuccess()) {
                    narrowedTypes.push_back(guardResult->getNarrowedType());
                }
            }
            
            if (narrowedTypes.empty()) {
                return std::make_shared<TypeGuardResult>(
                    false,
                    nullptr,
                    "جميع الحراس الفرعية فشلت (All sub-guards failed)"
                );
            }
            
            // دمج الأنواع في Union / Combine types into Union
            if (narrowedTypes.size() == 1) {
                return std::make_shared<TypeGuardResult>(true, narrowedTypes[0]);
            } else {
                auto unionType = std::make_shared<UnionType>(narrowedTypes);
                return std::make_shared<TypeGuardResult>(true, unionType);
            }
        }
        
        case TypeGuardKind::Not: {
            if (subGuards_.empty()) {
                return std::make_shared<TypeGuardResult>(
                    false,
                    nullptr,
                    "حارس Not يحتاج إلى حارس فرعي (Not guard needs sub-guard)"
                );
            }
            
            auto innerResult = subGuards_[0]->apply(currentType);
            if (innerResult->isSuccess()) {
                // الحارس الداخلي نجح، نحن نريد العكس / Inner guard succeeded, we want opposite
                // يجب استخدام narrowElse() من الحارس الداخلي / Use narrowElse() from inner guard
                TypePtr elseType = subGuards_[0]->narrowElse(currentType);
                return std::make_shared<TypeGuardResult>(true, elseType);
            } else {
                // الحارس الداخلي فشل، النوع الأصلي صالح / Inner guard failed, original type valid
                return std::make_shared<TypeGuardResult>(true, currentType);
            }
        }
        
        case TypeGuardKind::UserDefined: {
            // للحراس المعرّفة، نفترض أنها صحيحة ونرجع النوع المستهدف
            // For user-defined guards, assume correct and return target type
            if (targetType_) {
                return std::make_shared<TypeGuardResult>(true, targetType_);
            } else {
                return std::make_shared<TypeGuardResult>(true, currentType);
            }
        }
        
        default:
            return std::make_shared<TypeGuardResult>(
                false,
                nullptr,
                "نوع حارس غير مدعوم (Unsupported guard kind)"
            );
    }
}

bool TypeGuard::matches(TypePtr type) const {
    if (!type) {
        return false;
    }
    
    // Null و NotNull لا يحتاجان targetType / Null and NotNull don't need targetType
    if (kind_ == TypeGuardKind::Null || kind_ == TypeGuardKind::NotNull) {
        // يتم التعامل معهما مباشرةً / Handle directly
    } else if (!targetType_) {
        return false;
    }
    
    switch (kind_) {
        case TypeGuardKind::IsType:
            return matchesIsType(type);
            
        case TypeGuardKind::TypeOf:
            return matchesTypeOf(type);
            
        case TypeGuardKind::InstanceOf:
            return matchesInstanceOf(type);
            
        case TypeGuardKind::Null:
            return type->getKind() == SadTypeKind::Void;
            
        case TypeGuardKind::NotNull:
            return type->getKind() != SadTypeKind::Void;
            
        case TypeGuardKind::And:
            // جميع الحراس يجب أن تنجح / All guards must succeed
            for (const auto& guard : subGuards_) {
                if (!guard->matches(type)) {
                    return false;
                }
            }
            return true;
            
        case TypeGuardKind::Or:
            // أي حارس ينجح يكفي / Any guard succeeding is enough
            for (const auto& guard : subGuards_) {
                if (guard->matches(type)) {
                    return true;
                }
            }
            return false;
            
        case TypeGuardKind::Not:
            // عكس نتيجة الحارس الفرعي / Opposite of sub-guard result
            if (!subGuards_.empty()) {
                return !subGuards_[0]->matches(type);
            }
            return false;
            
        default:
            return false;
    }
}

TypePtr TypeGuard::narrow(TypePtr originalType) const {
    if (!originalType) {
        return nullptr;
    }
    
    // إذا كان Union أو Optional، نضيّق بإزالة الأنواع غير المطابقة
    // If Union or Optional, narrow by removing non-matching types
    if (originalType->getKind() == SadTypeKind::Union || originalType->getKind() == SadTypeKind::Optional) {
        auto unionType = std::static_pointer_cast<UnionType>(originalType);
        return narrowUnionType(unionType);
    }
    
    // إذا كان النوع بسيط وينطبق عليه الحارس، نرجعه كما هو
    // If simple type and guard matches, return as is
    if (matches(originalType)) {
        if (kind_ == TypeGuardKind::IsType && targetType_) {
            return targetType_;
        }
        return originalType;
    }
    
    // إذا لم ينطبق، نرجع Never
    // If doesn't match, return Never
    auto& registry = TypeRegistry::getInstance();
    return registry.getNeverType();
}

TypePtr TypeGuard::narrowElse(TypePtr originalType) const {
    if (!originalType) {
        return nullptr;
    }
    
    // إذا كان Union أو Optional، نزيل الأنواع المطابقة
    // If Union or Optional, remove matching types
    if (originalType->getKind() == SadTypeKind::Union || originalType->getKind() == SadTypeKind::Optional) {
        auto unionType = std::static_pointer_cast<UnionType>(originalType);
        const auto& alternatives = unionType->getAlternatives();
        
        std::vector<TypePtr> remaining;
        for (const auto& alt : alternatives) {
            if (!matches(alt)) {
                remaining.push_back(alt);
            }
        }
        
        if (remaining.empty()) {
            auto& registry = TypeRegistry::getInstance();
            return registry.getNeverType();
        }
        
        if (remaining.size() == 1) {
            return remaining[0];
        }
        
        return std::make_shared<UnionType>(remaining);
    }
    
    // إذا كان النوع بسيط ولا ينطبق عليه الحارس، نرجعه
    // If simple type and guard doesn't match, return it
    if (!matches(originalType)) {
        return originalType;
    }
    
    // إذا كان ينطبق، نرجع Never
    // If it matches, return Never
    auto& registry = TypeRegistry::getInstance();
    return registry.getNeverType();
}

// ════════════════════════════════════════════════════════════════════════════════
// TypeGuard Helper Methods
// ════════════════════════════════════════════════════════════════════════════════

std::string TypeGuard::toString() const {
    return toArabic() + " / " + toEnglish();
}

std::string TypeGuard::toArabic() const {
    std::ostringstream oss;
    
    switch (kind_) {
        case TypeGuardKind::IsType:
            oss << variableName_ << " نوع ";
            if (targetType_) {
                oss << targetType_->getArabicName();
            }
            break;
            
        case TypeGuardKind::TypeOf:
            oss << "نوع_من(" << variableName_ << ")";
            break;
            
        case TypeGuardKind::InstanceOf:
            oss << variableName_ << " من ";
            if (targetType_) {
                oss << targetType_->getArabicName();
            }
            break;
            
        case TypeGuardKind::Null:
            oss << variableName_ << " == عدم";
            break;
            
        case TypeGuardKind::NotNull:
            oss << variableName_ << " != عدم";
            break;
            
        case TypeGuardKind::And:
            oss << "(";
            for (size_t i = 0; i < subGuards_.size(); ++i) {
                if (i > 0) oss << " و ";
                oss << subGuards_[i]->toArabic();
            }
            oss << ")";
            break;
            
        case TypeGuardKind::Or:
            oss << "(";
            for (size_t i = 0; i < subGuards_.size(); ++i) {
                if (i > 0) oss << " أو ";
                oss << subGuards_[i]->toArabic();
            }
            oss << ")";
            break;
            
        case TypeGuardKind::Not:
            oss << "ليس (";
            if (!subGuards_.empty()) {
                oss << subGuards_[0]->toArabic();
            }
            oss << ")";
            break;
            
        case TypeGuardKind::UserDefined:
            oss << functionName_ << "(" << variableName_ << ")";
            break;
            
        default:
            oss << "حارس مجهول";
    }
    
    return oss.str();
}

std::string TypeGuard::toEnglish() const {
    std::ostringstream oss;
    
    switch (kind_) {
        case TypeGuardKind::IsType:
            oss << variableName_ << " is ";
            if (targetType_) {
                oss << targetType_->getEnglishName();
            }
            break;
            
        case TypeGuardKind::TypeOf:
            oss << "typeof(" << variableName_ << ")";
            break;
            
        case TypeGuardKind::InstanceOf:
            oss << variableName_ << " instanceof ";
            if (targetType_) {
                oss << targetType_->getEnglishName();
            }
            break;
            
        case TypeGuardKind::Null:
            oss << variableName_ << " == null";
            break;
            
        case TypeGuardKind::NotNull:
            oss << variableName_ << " != null";
            break;
            
        case TypeGuardKind::And:
            oss << "(";
            for (size_t i = 0; i < subGuards_.size(); ++i) {
                if (i > 0) oss << " && ";
                oss << subGuards_[i]->toEnglish();
            }
            oss << ")";
            break;
            
        case TypeGuardKind::Or:
            oss << "(";
            for (size_t i = 0; i < subGuards_.size(); ++i) {
                if (i > 0) oss << " || ";
                oss << subGuards_[i]->toEnglish();
            }
            oss << ")";
            break;
            
        case TypeGuardKind::Not:
            oss << "!(";
            if (!subGuards_.empty()) {
                oss << subGuards_[0]->toEnglish();
            }
            oss << ")";
            break;
            
        case TypeGuardKind::UserDefined:
            oss << functionName_ << "(" << variableName_ << ")";
            break;
            
        default:
            oss << "unknown guard";
    }
    
    return oss.str();
}

// ════════════════════════════════════════════════════════════════════════════════
// TypeGuard Factory Methods
// ════════════════════════════════════════════════════════════════════════════════

TypeGuardPtr TypeGuard::makeIsTypeGuard(const std::string& variableName, TypePtr targetType) {
    return std::make_shared<TypeGuard>(TypeGuardKind::IsType, variableName, targetType);
}

TypeGuardPtr TypeGuard::makeTypeOfGuard(const std::string& variableName, const std::string& typeName) {
    auto& registry = TypeRegistry::getInstance();
    TypePtr targetType;
    
    // تحويل اسم النوع إلى Type / Convert type name to Type
    if (typeName == "رقم" || typeName == "integer" || typeName == "number") {
        targetType = registry.getIntegerType();
    } else if (typeName == "نص" || typeName == "string") {
        targetType = registry.getStringType();
    } else if (typeName == "منطقي" || typeName == "boolean" || typeName == "bool") {
        targetType = registry.getBooleanType();
    } else if (typeName == "عشري" || typeName == "مضاعف" || typeName == "float" || typeName == "double") {
        targetType = registry.getFloatType();
    } else {
        targetType = nullptr;
    }
    
    return std::make_shared<TypeGuard>(TypeGuardKind::TypeOf, variableName, targetType);
}

TypeGuardPtr TypeGuard::makeInstanceOfGuard(const std::string& variableName, TypePtr classType) {
    return std::make_shared<TypeGuard>(TypeGuardKind::InstanceOf, variableName, classType);
}

TypeGuardPtr TypeGuard::makeNullGuard(const std::string& variableName) {
    auto& registry = TypeRegistry::getInstance();
    return std::make_shared<TypeGuard>(TypeGuardKind::Null, variableName, registry.getVoidType());
}

TypeGuardPtr TypeGuard::makeNotNullGuard(const std::string& variableName) {
    return std::make_shared<TypeGuard>(TypeGuardKind::NotNull, variableName, nullptr);
}

TypeGuardPtr TypeGuard::makeAndGuard(const std::vector<TypeGuardPtr>& guards) {
    return std::make_shared<TypeGuard>(TypeGuardKind::And, guards);
}

TypeGuardPtr TypeGuard::makeOrGuard(const std::vector<TypeGuardPtr>& guards) {
    return std::make_shared<TypeGuard>(TypeGuardKind::Or, guards);
}

TypeGuardPtr TypeGuard::makeNotGuard(TypeGuardPtr guard) {
    return std::make_shared<TypeGuard>(TypeGuardKind::Not, std::vector<TypeGuardPtr>{guard});
}

TypeGuardPtr TypeGuard::makeUserDefinedGuard(
    const std::string& functionName,
    const std::string& variableName,
    TypePtr returnType
) {
    return std::make_shared<TypeGuard>(functionName, variableName, returnType);
}

// ════════════════════════════════════════════════════════════════════════════════
// TypeGuard Internal Helper Methods
// ════════════════════════════════════════════════════════════════════════════════

TypePtr TypeGuard::narrowUnionType(UnionTypePtr unionType) const {
    const auto& alternatives = unionType->getAlternatives();
    std::vector<TypePtr> matching;
    
    for (const auto& alt : alternatives) {
        if (matches(alt)) {
            matching.push_back(alt);
        }
    }
    
    if (matching.empty()) {
        auto& registry = TypeRegistry::getInstance();
        return registry.getNeverType();
    }
    
    if (matching.size() == 1) {
        return matching[0];
    }
    
    return std::make_shared<UnionType>(matching);
}

bool TypeGuard::matchesIsType(TypePtr type) const {
    if (!targetType_) {
        return false;
    }
    
    // فحص المساواة أو التوافق / Check equality or compatibility
    return type->equals(targetType_.get()) || type->isAssignableTo(targetType_.get());
}

bool TypeGuard::matchesTypeOf(TypePtr type) const {
    if (!targetType_) {
        return false;
    }
    
    // typeof يفحص النوع البدائي / typeof checks primitive type
    return type->getKind() == targetType_->getKind();
}

bool TypeGuard::matchesInstanceOf(TypePtr type) const {
    if (!targetType_) {
        return false;
    }
    
    // instanceof يفحص الصنف / instanceof checks class
    if (type->getKind() == SadTypeKind::Class && targetType_->getKind() == SadTypeKind::Class) {
        return type->isSubtypeOf(targetType_.get());
    }
    
    return false;
}

// ════════════════════════════════════════════════════════════════════════════════
// Global Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

bool canNarrow(TypePtr type, TypeGuardPtr guard) {
    if (!type || !guard) {
        return false;
    }
    
    // Union types يمكن دائماً تضييقها / Union types can always be narrowed
    if (type->getKind() == SadTypeKind::Union) {
        return true;
    }
    
    // الأنواع الأخرى تعتمد على الحارس / Other types depend on guard
    return guard->matches(type);
}

TypeGuardPtr combineGuards(TypeGuardPtr guard1, TypeGuardPtr guard2) {
    if (!guard1) return guard2;
    if (!guard2) return guard1;
    
    return TypeGuard::makeAndGuard({guard1, guard2});
}

TypeGuardPtr invertGuard(TypeGuardPtr guard) {
    if (!guard) {
        return nullptr;
    }
    
    return TypeGuard::makeNotGuard(guard);
}

TypePtr applyGuards(TypePtr type, const std::vector<TypeGuardPtr>& guards) {
    if (!type || guards.empty()) {
        return type;
    }
    
    TypePtr result = type;
    for (const auto& guard : guards) {
        auto guardResult = guard->apply(result);
        if (!guardResult->isSuccess()) {
            // فشل أحد الحراس / One guard failed
            auto& registry = TypeRegistry::getInstance();
            return registry.getNeverType();
        }
        result = guardResult->getNarrowedType();
    }
    
    return result;
}

} // namespace TypeSystem
} // namespace Sad
