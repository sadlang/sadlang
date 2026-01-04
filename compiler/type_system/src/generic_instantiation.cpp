// ════════════════════════════════════════════════════════════════════════════════
// ملف: generic_instantiation.cpp
// File: generic_instantiation.cpp
//
// الوصف: تنفيذ instantiation للأنواع العامة (Monomorphization)
// Description: Generic instantiation implementation (Monomorphization)
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// المصدر: compiler/type_system/include/generic_instantiation.h (lines 1-280)
// Source: compiler/type_system/include/generic_instantiation.h (lines 1-280)
// ════════════════════════════════════════════════════════════════════════════════

#include "generic_instantiation.h"
#include "type_constraint.h"
#include <sstream>

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// المُنشئات / Constructors
// Source: generic_instantiation.h:43-48
// ════════════════════════════════════════════════════════════════════════════════

GenericInstantiator::GenericInstantiator()
    : instantiationCache_()
    , stats_()
{
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال Instantiation الرئيسية / Main Instantiation Methods
// Source: generic_instantiation.h:55-104
// ════════════════════════════════════════════════════════════════════════════════

GenericTypePtr GenericInstantiator::instantiate(const GenericTypePtr& genericType, 
                                                 const TypeList& typeArgs) {
    if (!genericType) return nullptr;
    
    // استخدام دالة specialize من GenericType
    // Use specialize from GenericType
    auto specialized = genericType->specialize(typeArgs);
    
    if (specialized) {
        ++stats_.totalInstantiations;
    }
    
    return specialized;
}

GenericTypePtr GenericInstantiator::instantiateWithValidation(const GenericTypePtr& genericType, 
                                                               const TypeList& typeArgs,
                                                               std::vector<std::string>* errors) {
    if (!genericType) {
        if (errors) {
            errors->push_back("Generic type is null");
        }
        ++stats_.validationFailures;
        return nullptr;
    }
    
    // التحقق من صحة الـ instantiation / Validate instantiation
    if (!validate(genericType, typeArgs, errors)) {
        ++stats_.validationFailures;
        return nullptr;
    }
    
    // إنشاء الـ instantiation / Create instantiation
    return instantiate(genericType, typeArgs);
}

GenericTypePtr GenericInstantiator::getOrInstantiate(const GenericTypePtr& genericType, 
                                                      const TypeList& typeArgs) {
    if (!genericType) return nullptr;
    
    // الحصول على مفتاح cache / Get cache key
    std::string key = getCacheKey(genericType, typeArgs);
    
    // البحث في cache / Search in cache
    auto it = instantiationCache_.find(key);
    if (it != instantiationCache_.end()) {
        ++stats_.cacheHits;
        return it->second;
    }
    
    // إنشاء جديد / Create new
    ++stats_.cacheMisses;
    auto instantiated = instantiate(genericType, typeArgs);
    
    if (instantiated) {
        // تخزين في cache / Store in cache
        instantiationCache_[key] = instantiated;
    }
    
    return instantiated;
}

// ════════════════════════════════════════════════════════════════════════════════
// Type Substitution
// Source: generic_instantiation.h:111-140
// ════════════════════════════════════════════════════════════════════════════════

TypePtr GenericInstantiator::substituteTypeParameters(const TypePtr& type, 
                                                       const std::unordered_map<std::string, TypePtr>& substitutionMap) {
    if (!type || substitutionMap.empty()) {
        return type;
    }
    
    return substituteSingleType(type, substitutionMap);
}

TypeList GenericInstantiator::substituteTypeList(const TypeList& types, 
                                                  const std::unordered_map<std::string, TypePtr>& substitutionMap) {
    if (substitutionMap.empty()) {
        return types;
    }
    
    TypeList substituted;
    substituted.reserve(types.size());
    
    for (const auto& type : types) {
        substituted.push_back(substituteTypeParameters(type, substitutionMap));
    }
    
    return substituted;
}

// ════════════════════════════════════════════════════════════════════════════════
// Validation
// Source: generic_instantiation.h:147-184
// ════════════════════════════════════════════════════════════════════════════════

bool GenericInstantiator::validate(const GenericTypePtr& genericType, 
                                    const TypeList& typeArgs,
                                    std::vector<std::string>* errors) {
    if (!genericType) {
        if (errors) errors->push_back("Generic type is null");
        return false;
    }
    
    // التحقق من عدد المعاملات / Check parameter count
    if (!validateParameterCount(genericType, typeArgs)) {
        if (errors) {
            std::ostringstream oss;
            oss << "Type parameter count mismatch: expected " 
                << genericType->getTypeParameterCount() 
                << ", got " << typeArgs.size();
            errors->push_back(oss.str());
        }
        return false;
    }
    
    // التحقق من القيود / Check constraints
    if (!validateConstraints(genericType, typeArgs, errors)) {
        return false;
    }
    
    return true;
}

bool GenericInstantiator::validateParameterCount(const GenericTypePtr& genericType, 
                                                  const TypeList& typeArgs) {
    if (!genericType) return false;
    
    return genericType->hasTypeParameterCount(typeArgs.size());
}

bool GenericInstantiator::validateConstraints(const GenericTypePtr& genericType, 
                                               const TypeList& typeArgs,
                                               std::vector<std::string>* errors) {
    if (!genericType) return false;
    
    const auto& typeParams = genericType->getTypeParameters();
    
    // التحقق من كل معامل نوع / Check each type parameter
    for (size_t i = 0; i < typeParams.size() && i < typeArgs.size(); ++i) {
        const TypeParameterPtr& param = typeParams[i];
        const TypePtr& arg = typeArgs[i];
        
        // التحقق من القيود / Check constraints
        if (!param->satisfiesConstraints(arg)) {
            if (errors) {
                std::ostringstream oss;
                oss << "Type argument '" << arg->toString() 
                    << "' does not satisfy constraints for parameter '" 
                    << param->getName() << "'";
                errors->push_back(oss.str());
            }
            return false;
        }
    }
    
    return true;
}

// ════════════════════════════════════════════════════════════════════════════════
// Cache Management
// Source: generic_instantiation.h:191-221
// ════════════════════════════════════════════════════════════════════════════════

void GenericInstantiator::clearCache() {
    instantiationCache_.clear();
}

size_t GenericInstantiator::getCacheSize() const {
    return instantiationCache_.size();
}

bool GenericInstantiator::isCached(const std::string& key) const {
    return instantiationCache_.find(key) != instantiationCache_.end();
}

std::string GenericInstantiator::getCacheKey(const GenericTypePtr& genericType, 
                                              const TypeList& typeArgs) const {
    if (!genericType) return "";
    
    std::ostringstream oss;
    oss << genericType->getBaseName() << "<";
    
    for (size_t i = 0; i < typeArgs.size(); ++i) {
        if (i > 0) oss << ",";
        oss << typeArgs[i]->toString();
    }
    
    oss << ">";
    return oss.str();
}

// ════════════════════════════════════════════════════════════════════════════════
// Statistics
// Source: generic_instantiation.h:228-253
// ════════════════════════════════════════════════════════════════════════════════

void GenericInstantiator::resetStatistics() {
    stats_ = Statistics();
}

// ════════════════════════════════════════════════════════════════════════════════
// الدوال الداخلية / Internal Methods
// Source: generic_instantiation.h:260-280
// ════════════════════════════════════════════════════════════════════════════════

TypePtr GenericInstantiator::substituteSingleType(const TypePtr& type, 
                                                   const std::unordered_map<std::string, TypePtr>& substitutionMap) {
    if (!type) return nullptr;
    
    // إذا كان النوع معامل نوع / If type is type parameter
    if (isTypeParameter(type)) {
        TypeParameterPtr param = asTypeParameter(type);
        const std::string& name = param->getName();
        
        // البحث في خريطة الاستبدال / Search in substitution map
        auto it = substitutionMap.find(name);
        if (it != substitutionMap.end()) {
            return it->second;
        }
        
        // لم يُوجد استبدال / No substitution found
        return type;
    }
    
    // إذا كان النوع عاماً / If type is generic
    if (isGenericType(type)) {
        GenericTypePtr genericType = asGenericType(type);
        return substituteInGenericType(genericType, substitutionMap);
    }
    
    // أنواع أخرى لا تحتاج إلى استبدال / Other types don't need substitution
    return type;
}

TypePtr GenericInstantiator::substituteInGenericType(const GenericTypePtr& genericType, 
                                                      const std::unordered_map<std::string, TypePtr>& substitutionMap) {
    if (!genericType) return nullptr;
    
    // إذا كان النوع مُخصصاً / If type is specialized
    if (genericType->isSpecialized()) {
        // استبدال في الأنواع الفعلية / Substitute in type arguments
        const TypeList& typeArgs = genericType->getTypeArguments();
        TypeList newArgs = substituteTypeList(typeArgs, substitutionMap);
        
        // إنشاء نوع جديد / Create new type
        return makeSpecializedGenericType(genericType->getBaseName(), newArgs);
    }
    
    // نوع عام غير مُخصص / Non-specialized generic type
    return genericType;
}

bool GenericInstantiator::validateSingleConstraint(const TypeConstraintPtr& constraint, 
                                                    const TypePtr& type,
                                                    std::string* error) {
    if (!constraint || !type) return false;
    
    bool satisfied = constraint->isSatisfied(type);
    
    if (!satisfied && error) {
        std::ostringstream oss;
        oss << "Type '" << type->toString() 
            << "' does not satisfy constraint: " << constraint->toString();
        *error = oss.str();
    }
    
    return satisfied;
}

// ════════════════════════════════════════════════════════════════════════════════
// Global Instance
// Source: generic_instantiation.h:288-294
// ════════════════════════════════════════════════════════════════════════════════

GenericInstantiator& getGlobalInstantiator() {
    static GenericInstantiator instance;
    return instance;
}

} // namespace TypeSystem
} // namespace Sad
