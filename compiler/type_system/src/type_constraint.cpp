// ════════════════════════════════════════════════════════════════════════════════
// ملف: type_constraint.cpp
// File: type_constraint.cpp
//
// الوصف: تنفيذ قيود الأنواع (Type Constraints)
// Description: Type Constraints implementation
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// المصدر: compiler/type_system/include/type_constraint.h (lines 1-350)
// Source: compiler/type_system/include/type_constraint.h (lines 1-350)
// ════════════════════════════════════════════════════════════════════════════════

#include "type_constraint.h"
#include <sstream>
#include <algorithm>

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// المُنشئات / Constructors
// Source: type_constraint.h:54-77
// ════════════════════════════════════════════════════════════════════════════════

TypeConstraint::TypeConstraint(ConstraintKind kind, TypePtr constraintType)
    : kind_(kind)
    , constraintType_(constraintType)
    , protocolName_()
    , leftParam_()
    , rightParam_()
    , description_()
{
}

TypeConstraint::TypeConstraint(const std::string& leftParam, const std::string& rightParam)
    : kind_(ConstraintKind::SameType)
    , constraintType_(nullptr)
    , protocolName_()
    , leftParam_(leftParam)
    , rightParam_(rightParam)
    , description_()
{
}

TypeConstraint::TypeConstraint(const std::string& protocolName)
    : kind_(ConstraintKind::Protocol)
    , constraintType_(nullptr)
    , protocolName_(protocolName)
    , leftParam_()
    , rightParam_()
    , description_()
{
}

TypeConstraint::TypeConstraint(ConstraintKind kind, const std::string& description)
    : kind_(kind)
    , constraintType_(nullptr)
    , protocolName_()
    , leftParam_()
    , rightParam_()
    , description_(description)
{
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال التحقق / Validation Methods
// Source: type_constraint.h:120-190
// ════════════════════════════════════════════════════════════════════════════════

bool TypeConstraint::isSatisfied(const TypePtr& type) const {
    if (!type) return false;
    
    switch (kind_) {
        case ConstraintKind::Subtype:
            return checkSubtypeConstraint(type);
        
        case ConstraintKind::Protocol:
            return checkProtocolConstraint(type);
        
        case ConstraintKind::SameType:
            // SameType يحتاج إلى parameter map
            // SameType requires parameter map
            return false;
        
        case ConstraintKind::Constructor:
            return checkConstructorConstraint(type);
        
        case ConstraintKind::Method:
            return checkMethodConstraint(type);
        
        case ConstraintKind::Property:
            return checkPropertyConstraint(type);
        
        case ConstraintKind::Numeric:
            return checkNumericConstraint(type);
        
        case ConstraintKind::Iterable:
            return checkIterableConstraint(type);
        
        case ConstraintKind::Custom:
            // Custom constraints need specialized handling
            return false;
        
        default:
            return false;
    }
}

bool TypeConstraint::isSatisfied(const TypePtr& type, 
                                  const std::unordered_map<std::string, TypePtr>& parameterMap) const {
    if (!type) return false;
    
    // إذا كان القيد SameType، استخدم parameter map
    // If constraint is SameType, use parameter map
    if (kind_ == ConstraintKind::SameType) {
        return checkSameTypeConstraint(type, parameterMap);
    }
    
    // باقي القيود لا تحتاج parameter map
    // Other constraints don't need parameter map
    return isSatisfied(type);
}

std::string TypeConstraint::toString() const {
    std::ostringstream oss;
    
    switch (kind_) {
        case ConstraintKind::Subtype:
            if (constraintType_) {
                oss << ": " << constraintType_->toString();
            }
            break;
        
        case ConstraintKind::Protocol:
            if (!protocolName_.empty()) {
                oss << ": " << protocolName_;
            } else if (constraintType_) {
                oss << ": " << constraintType_->toString();
            }
            break;
        
        case ConstraintKind::SameType:
            oss << leftParam_ << " == " << rightParam_;
            break;
        
        case ConstraintKind::Constructor:
            oss << "constructor" << (description_.empty() ? "" : " (" + description_ + ")");
            break;
        
        case ConstraintKind::Method:
            oss << "method: " << description_;
            break;
        
        case ConstraintKind::Property:
            oss << "property: " << description_;
            break;
        
        case ConstraintKind::Numeric:
            oss << "Numeric";
            break;
        
        case ConstraintKind::Iterable:
            oss << "Iterable";
            break;
        
        case ConstraintKind::Custom:
            oss << description_;
            break;
    }
    
    return oss.str();
}

bool TypeConstraint::equals(const TypeConstraint& other) const {
    // مقارنة نوع القيد / Compare constraint kind
    if (kind_ != other.kind_) return false;
    
    // مقارنة حسب النوع / Compare by kind
    switch (kind_) {
        case ConstraintKind::Subtype:
        case ConstraintKind::Protocol:
            if (constraintType_ && other.constraintType_) {
                return constraintType_->equals(other.constraintType_.get());
            }
            return protocolName_ == other.protocolName_;
        
        case ConstraintKind::SameType:
            return leftParam_ == other.leftParam_ && rightParam_ == other.rightParam_;
        
        case ConstraintKind::Constructor:
        case ConstraintKind::Method:
        case ConstraintKind::Property:
        case ConstraintKind::Custom:
            return description_ == other.description_;
        
        case ConstraintKind::Numeric:
        case ConstraintKind::Iterable:
            return true; // لا توجد بيانات إضافية / No additional data
        
        default:
            return false;
    }
}

// ════════════════════════════════════════════════════════════════════════════════
// الدوال الداخلية / Internal Methods
// Source: type_constraint.h:197-240
// ════════════════════════════════════════════════════════════════════════════════

bool TypeConstraint::checkSubtypeConstraint(const TypePtr& type) const {
    if (!constraintType_) return false;
    
    // التحقق من كون النوع فرعياً من النوع المقيد
    // Check if type is subtype of constraint type
    return type->isSubtypeOf(constraintType_.get());
}

bool TypeConstraint::checkProtocolConstraint(const TypePtr& type) const {
    // TODO: تنفيذ كامل بعد إضافة نظام الـ protocols
    // TODO: Full implementation after adding protocols system
    
    // مؤقتاً، تحقق من اسم الـ protocol
    // Temporarily, check protocol name
    
    if (!protocolName_.empty()) {
        // بعض الـ protocols المعروفة
        // Some known protocols
        
        if (protocolName_ == "Comparable" || protocolName_ == "قابل_للمقارنة") {
            // الأنواع الرقمية و Strings قابلة للمقارنة
            // Numeric types and Strings are comparable
            TypeKind kind = type->getKind();
            return kind == TypeKind::Integer || 
                   kind == TypeKind::Float || 
                   kind == TypeKind::String;
        }
        
        if (protocolName_ == "Numeric" || protocolName_ == "رقمي") {
            TypeKind kind = type->getKind();
            return kind == TypeKind::Integer || kind == TypeKind::Float;
        }
        
        if (protocolName_ == "Iterable" || protocolName_ == "قابل_للتكرار") {
            TypeKind kind = type->getKind();
            return kind == TypeKind::Array || 
                   kind == TypeKind::String ||
                   kind == TypeKind::Dictionary;
        }
    }
    
    return false;
}

bool TypeConstraint::checkSameTypeConstraint(const TypePtr& type, 
                                              const std::unordered_map<std::string, TypePtr>& parameterMap) const {
    // البحث عن المعاملين في الخريطة
    // Find both parameters in map
    
    auto leftIt = parameterMap.find(leftParam_);
    auto rightIt = parameterMap.find(rightParam_);
    
    if (leftIt == parameterMap.end() || rightIt == parameterMap.end()) {
        return false;
    }
    
    // التحقق من تساوي النوعين
    // Check if types are equal
    return leftIt->second->equals(rightIt->second.get());
}

bool TypeConstraint::checkConstructorConstraint(const TypePtr& type) const {
    // TODO: تنفيذ كامل بعد إضافة نظام الأصناف
    // TODO: Full implementation after adding class system
    
    // مؤقتاً، جميع الأنواع لها منشئات افتراضية
    // Temporarily, all types have default constructors
    return true;
}

bool TypeConstraint::checkMethodConstraint(const TypePtr& type) const {
    // TODO: تنفيذ كامل بعد إضافة نظام الأصناف
    // TODO: Full implementation after adding class system
    
    return false;
}

bool TypeConstraint::checkPropertyConstraint(const TypePtr& type) const {
    // TODO: تنفيذ كامل بعد إضافة نظام الأصناف
    // TODO: Full implementation after adding class system
    
    return false;
}

bool TypeConstraint::checkNumericConstraint(const TypePtr& type) const {
    TypeKind kind = type->getKind();
    return kind == TypeKind::Integer || kind == TypeKind::Float;
}

bool TypeConstraint::checkIterableConstraint(const TypePtr& type) const {
    TypeKind kind = type->getKind();
    return kind == TypeKind::Array || 
           kind == TypeKind::String ||
           kind == TypeKind::Dictionary;
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة / Helper Functions
// Source: type_constraint.h:250-330
// ════════════════════════════════════════════════════════════════════════════════

TypeConstraintPtr makeSubtypeConstraint(TypePtr baseType) {
    return std::make_shared<TypeConstraint>(ConstraintKind::Subtype, baseType);
}

TypeConstraintPtr makeProtocolConstraint(const std::string& protocolName) {
    return std::make_shared<TypeConstraint>(protocolName);
}

TypeConstraintPtr makeSameTypeConstraint(const std::string& leftParam, 
                                         const std::string& rightParam) {
    return std::make_shared<TypeConstraint>(leftParam, rightParam);
}

TypeConstraintPtr makeNumericConstraint() {
    return std::make_shared<TypeConstraint>(ConstraintKind::Numeric, "Numeric");
}

TypeConstraintPtr makeIterableConstraint() {
    return std::make_shared<TypeConstraint>(ConstraintKind::Iterable, "Iterable");
}

TypeConstraintPtr makeConstructorConstraint(const std::string& description) {
    return std::make_shared<TypeConstraint>(ConstraintKind::Constructor, description);
}

TypeConstraintPtr makeMethodConstraint(const std::string& methodName) {
    return std::make_shared<TypeConstraint>(ConstraintKind::Method, methodName);
}

TypeConstraintPtr makePropertyConstraint(const std::string& propertyName) {
    return std::make_shared<TypeConstraint>(ConstraintKind::Property, propertyName);
}

std::vector<TypeConstraintPtr> combineConstraints(const std::vector<TypeConstraintPtr>& constraints) {
    std::vector<TypeConstraintPtr> combined;
    
    // إزالة التكرار / Remove duplicates
    for (const auto& constraint : constraints) {
        bool isDuplicate = false;
        
        for (const auto& existing : combined) {
            if (constraint->equals(*existing)) {
                isDuplicate = true;
                break;
            }
        }
        
        if (!isDuplicate) {
            combined.push_back(constraint);
        }
    }
    
    return combined;
}

bool satisfiesAllConstraints(const TypePtr& type, 
                              const std::vector<TypeConstraintPtr>& constraints) {
    if (!type) return false;
    
    // التحقق من جميع القيود / Check all constraints
    for (const auto& constraint : constraints) {
        if (!constraint->isSatisfied(type)) {
            return false;
        }
    }
    
    return true;
}

} // namespace TypeSystem
} // namespace Sad
