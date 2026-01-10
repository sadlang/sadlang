// ════════════════════════════════════════════════════════════════════════════════
// ملف: generic_type.cpp
// File: generic_type.cpp
//
// الوصف: تنفيذ الأنواع العامة (Generics)
// Description: Generic Types implementation
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// المصدر: compiler/type_system/include/generic_type.h (lines 1-380)
// Source: compiler/type_system/include/generic_type.h (lines 1-380)
// ════════════════════════════════════════════════════════════════════════════════

#include "generic_type.h"
#include "type_parameter.h"
#include <sstream>
#include <algorithm>

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// المُنشئات / Constructors
// Source: generic_type.h:47-60
// ════════════════════════════════════════════════════════════════════════════════

GenericType::GenericType(const std::string& baseName, 
                         const std::vector<TypeParameterPtr>& typeParameters)
    : Type(TypeKind::Generic)
    , baseName_(baseName)
    , typeParameters_(typeParameters)
    , typeArguments_()
    , variance_(Variance::Invariant)
{
    // بناء خريطة المعاملات / Build parameter map
    buildParameterMap();
}

GenericType::GenericType(const std::string& baseName, 
                         const TypeList& typeArguments)
    : Type(TypeKind::Generic)
    , baseName_(baseName)
    , typeParameters_()
    , typeArguments_(typeArguments)
    , variance_(Variance::Invariant)
{
    // نوع عام مُخصص / Specialized generic type
    // لا يحتاج إلى معاملات نوع / No type parameters needed
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال مجردة مُطبقة / Implemented Abstract Functions
// Source: generic_type.h:67-95
// ════════════════════════════════════════════════════════════════════════════════

std::string GenericType::getArabicName() const {
    // إذا كان النوع مُخصصاً / If specialized
    if (isSpecialized()) {
        std::ostringstream oss;
        oss << baseName_ << "<";
        
        for (size_t i = 0; i < typeArguments_.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << typeArguments_[i]->getArabicName();
        }
        
        oss << ">";
        return oss.str();
    }
    
    // إذا كان النوع عاماً (غير مُخصص) / If generic (not specialized)
    std::ostringstream oss;
    oss << baseName_ << "<";
    
    for (size_t i = 0; i < typeParameters_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << typeParameters_[i]->getArabicName();
    }
    
    oss << ">";
    return oss.str();
}

std::string GenericType::getEnglishName() const {
    // نفس المنطق / Same logic
    if (isSpecialized()) {
        std::ostringstream oss;
        oss << baseName_ << "<";
        
        for (size_t i = 0; i < typeArguments_.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << typeArguments_[i]->getEnglishName();
        }
        
        oss << ">";
        return oss.str();
    }
    
    std::ostringstream oss;
    oss << baseName_ << "<";
    
    for (size_t i = 0; i < typeParameters_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << typeParameters_[i]->getEnglishName();
    }
    
    oss << ">";
    return oss.str();
}

std::string GenericType::toString() const {
    // استخدام الاسم الإنجليزي للتمثيل القياسي / Use English name for standard representation
    return getEnglishName();
}

bool GenericType::equals(const Type* other) const {
    // التحقق من nullptr
    if (!other) return false;
    
    // التحقق من نوع النوع / Check type kind
    if (other->getKind() != TypeKind::Generic) return false;
    
    // تحويل آمن / Safe cast
    const GenericType* otherGeneric = static_cast<const GenericType*>(other);
    
    // التحقق من الاسم الأساسي / Check base name
    if (baseName_ != otherGeneric->baseName_) return false;
    
    // التحقق من حالة التخصيص / Check specialization status
    if (isSpecialized() != otherGeneric->isSpecialized()) return false;
    
    if (isSpecialized()) {
        // مقارنة الأنواع الفعلية / Compare type arguments
        if (typeArguments_.size() != otherGeneric->typeArguments_.size()) {
            return false;
        }
        
        for (size_t i = 0; i < typeArguments_.size(); ++i) {
            if (!typeArguments_[i]->equals(otherGeneric->typeArguments_[i].get())) {
                return false;
            }
        }
    } else {
        // مقارنة معاملات النوع / Compare type parameters
        if (typeParameters_.size() != otherGeneric->typeParameters_.size()) {
            return false;
        }
        
        for (size_t i = 0; i < typeParameters_.size(); ++i) {
            if (!typeParameters_[i]->equals(otherGeneric->typeParameters_[i].get())) {
                return false;
            }
        }
    }
    
    return true;
}

std::shared_ptr<Type> GenericType::clone() const {
    if (isSpecialized()) {
        // استنساخ نوع مُخصص / Clone specialized type
        return std::make_shared<GenericType>(baseName_, typeArguments_);
    } else {
        // استنساخ نوع عام / Clone generic type
        return std::make_shared<GenericType>(baseName_, typeParameters_);
    }
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال الأنواع العامة / Generic-Specific Methods
// Source: generic_type.h:102-245
// ════════════════════════════════════════════════════════════════════════════════

// NOTE: getBaseName(), getTypeParameters(), getTypeArguments(), isSpecialized()
// are defined inline in header file

std::shared_ptr<GenericType> GenericType::specialize(const TypeList& typeArgs) const {
    // التحقق من العدد / Validate count
    if (!hasTypeParameterCount(typeArgs.size())) {
        return nullptr; // خطأ / Error
    }
    
    // التحقق من صحة الأنواع / Validate types
    for (size_t i = 0; i < typeArgs.size(); ++i) {
        TypeParameterPtr param = typeParameters_[i];
        
        // التحقق من القيود / Check constraints
        if (!param->satisfiesConstraints(typeArgs[i])) {
            return nullptr; // النوع لا يُطابق القيود / Type doesn't satisfy constraints
        }
    }
    
    // إنشاء نوع مُخصص / Create specialized type
    auto specialized = std::make_shared<GenericType>(baseName_, typeArgs);
    specialized->setVariance(variance_);
    
    return specialized;
}

std::unordered_map<std::string, TypePtr> GenericType::getSubstitutionMap() const {
    std::unordered_map<std::string, TypePtr> map;
    
    if (isSpecialized()) {
        // خريطة من أسماء المعاملات إلى الأنواع الفعلية
        // Map from parameter names to actual types
        for (size_t i = 0; i < typeParameters_.size() && i < typeArguments_.size(); ++i) {
            map[typeParameters_[i]->getName()] = typeArguments_[i];
        }
    }
    
    return map;
}

TypePtr GenericType::substituteTypeParameters(TypePtr type) const {
    if (!isSpecialized()) {
        // لا توجد معاملات مُستبدلة / No substitutions available
        return type;
    }
    
    // إذا كان النوع معامل نوع / If type is a type parameter
    if (type && type->isTypeParameter()) {
        TypeParameterPtr param = asTypeParameter(type);
        TypePtr substitution = getTypeArgument(param);
        
        if (substitution) {
            return substitution;
        }
    }
    
    // إذا كان النوع عاماً / If type is generic
    if (type && type->getKind() == TypeKind::Generic) {
        GenericTypePtr genericType = asGenericType(type);
        
        // استبدال معاملات الأنواع الفرعية / Substitute nested type parameters
        TypeList newArgs;
        for (const auto& arg : genericType->getTypeArguments()) {
            newArgs.push_back(substituteTypeParameters(arg));
        }
        
        return genericType->specialize(newArgs);
    }
    
    // النوع لا يحتاج إلى استبدال / Type doesn't need substitution
    return type;
}

// NOTE: hasTypeParameterCount() and getTypeParameterCount() are inline in header

TypeParameterPtr GenericType::getTypeParameter(size_t index) const {
    if (index >= typeParameters_.size()) {
        return nullptr;
    }
    
    return typeParameters_[index];
}

TypeParameterPtr GenericType::getTypeParameter(const std::string& name) const {
    auto it = parameterNameToIndex_.find(name);
    
    if (it == parameterNameToIndex_.end()) {
        return nullptr;
    }
    
    return typeParameters_[it->second];
}

TypePtr GenericType::getTypeArgument(size_t index) const {
    if (!isSpecialized() || index >= typeArguments_.size()) {
        return nullptr;
    }
    
    return typeArguments_[index];
}

TypePtr GenericType::getTypeArgument(const TypeParameterPtr& param) const {
    if (!isSpecialized() || !param) {
        return nullptr;
    }
    
    // البحث عن المعامل في القائمة / Find parameter in list
    for (size_t i = 0; i < typeParameters_.size(); ++i) {
        if (typeParameters_[i]->equals(param.get())) {
            if (i < typeArguments_.size()) {
                return typeArguments_[i];
            }
        }
    }
    
    return nullptr;
}

// ════════════════════════════════════════════════════════════════════════════════
// علاقات الأنواع / Type Relations
// Source: generic_type.h:252-273
// ════════════════════════════════════════════════════════════════════════════════

bool GenericType::isAssignableTo(const Type* other) const {
    if (!other) return false;
    
    // التحقق من التساوي أولاً / Check equality first
    if (equals(other)) return true;
    
    // إذا كان النوع الآخر عاماً / If other is generic
    if (other->getKind() == TypeKind::Generic) {
        const GenericType* otherGeneric = static_cast<const GenericType*>(other);
        
        // التحقق من الاسم الأساسي / Check base name
        if (baseName_ != otherGeneric->baseName_) return false;
        
        // التحقق من variance
        if (variance_ == Variance::Covariant) {
            // Covariant: Array<Dog> <: Array<Animal>
            // إذا كانت جميع الأنواع الفعلية فرعية / If all type args are subtypes
            if (isSpecialized() && otherGeneric->isSpecialized()) {
                if (typeArguments_.size() != otherGeneric->typeArguments_.size()) {
                    return false;
                }
                
                for (size_t i = 0; i < typeArguments_.size(); ++i) {
                    if (!typeArguments_[i]->isSubtypeOf(otherGeneric->typeArguments_[i].get())) {
                        return false;
                    }
                }
                
                return true;
            }
        } else if (variance_ == Variance::Contravariant) {
            // Contravariant: Comparator<Animal> <: Comparator<Dog>
            if (isSpecialized() && otherGeneric->isSpecialized()) {
                if (typeArguments_.size() != otherGeneric->typeArguments_.size()) {
                    return false;
                }
                
                for (size_t i = 0; i < typeArguments_.size(); ++i) {
                    if (!otherGeneric->typeArguments_[i]->isSubtypeOf(typeArguments_[i].get())) {
                        return false;
                    }
                }
                
                return true;
            }
        } else {
            // Invariant: تطابق تام / Exact match required
            return equals(other);
        }
    }
    
    return false;
}

bool GenericType::isSubtypeOf(const Type* other) const {
    // نفس المنطق / Same logic
    return isAssignableTo(other);
}

// ════════════════════════════════════════════════════════════════════════════════
// Variance
// Source: generic_type.h:280-289
// ════════════════════════════════════════════════════════════════════════════════

// NOTE: getVariance() and setVariance() are inline in header

// ════════════════════════════════════════════════════════════════════════════════
// الدوال الداخلية / Internal Methods
// Source: generic_type.h:296-310
// ════════════════════════════════════════════════════════════════════════════════

void GenericType::buildParameterMap() {
    // بناء خريطة من أسماء المعاملات إلى الفهارس
    // Build map from parameter names to indices
    parameterNameToIndex_.clear();
    
    for (size_t i = 0; i < typeParameters_.size(); ++i) {
        const std::string& name = typeParameters_[i]->getName();
        parameterNameToIndex_[name] = i;
    }
}

bool GenericType::validateTypeArguments() const {
    if (!isSpecialized()) return true;
    
    // التحقق من عدد الأنواع الفعلية / Verify type argument count
    if (typeArguments_.size() != typeParameters_.size()) {
        return false;
    }
    
    // التحقق من القيود / Verify constraints
    for (size_t i = 0; i < typeArguments_.size(); ++i) {
        if (!typeParameters_[i]->satisfiesConstraints(typeArguments_[i])) {
            return false;
        }
    }
    
    return true;
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة / Helper Functions
// Source: generic_type.h:317-376
// ════════════════════════════════════════════════════════════════════════════════

GenericTypePtr makeGenericType(const std::string& baseName, 
                                const std::vector<TypeParameterPtr>& typeParameters) {
    return std::make_shared<GenericType>(baseName, typeParameters);
}

GenericTypePtr makeSpecializedGenericType(const std::string& baseName, 
                                           const TypeList& typeArguments) {
    return std::make_shared<GenericType>(baseName, typeArguments);
}

bool isGenericType(const TypePtr& type) {
    return type && type->getKind() == TypeKind::Generic;
}

GenericTypePtr asGenericType(const TypePtr& type) {
    if (!isGenericType(type)) {
        return nullptr;
    }
    
    return std::static_pointer_cast<GenericType>(type);
}

} // namespace TypeSystem
} // namespace Sad
