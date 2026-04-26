// ════════════════════════════════════════════════════════════════════════════════
// ملف: type_parameter.cpp
// File: type_parameter.cpp
//
// الوصف: تنفيذ معاملات الأنواع (Type Parameters)
// Description: Type Parameters implementation
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// المصدر: compiler/type_system/include/type_parameter.h (lines 1-380)
// Source: compiler/type_system/include/type_parameter.h (lines 1-380)
// ════════════════════════════════════════════════════════════════════════════════

#include "type_parameter.h"
#include "type_constraint.h"
#include <sstream>

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// المُنشئات / Constructors
// Source: type_parameter.h:42-60
// ════════════════════════════════════════════════════════════════════════════════

TypeParameter::TypeParameter(const std::string& name)
    : Type(SadTypeKind::TypeParameter)
    , name_(name)
    , constraints_()
    , upperBound_(nullptr)
    , lowerBound_(nullptr)
    , variance_(Variance::Invariant)
{
}

TypeParameter::TypeParameter(const std::string& name, 
                             const std::vector<TypeConstraintPtr>& constraints)
    : Type(SadTypeKind::TypeParameter)
    , name_(name)
    , constraints_(constraints)
    , upperBound_(nullptr)
    , lowerBound_(nullptr)
    , variance_(Variance::Invariant)
{
}

TypeParameter::TypeParameter(const std::string& name, TypePtr upperBound)
    : Type(SadTypeKind::TypeParameter)
    , name_(name)
    , constraints_()
    , upperBound_(upperBound)
    , lowerBound_(nullptr)
    , variance_(Variance::Invariant)
{
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال مجردة مُطبقة / Implemented Abstract Functions
// Source: type_parameter.h:67-85
// ════════════════════════════════════════════════════════════════════════════════

std::string TypeParameter::getArabicName() const {
    // معاملات الأنواع غالباً تستخدم أحرف إنجليزية (T, K, V)
    // Type parameters typically use English letters (T, K, V)
    // لكن يمكن إضافة وصف بالعربية إذا كان هناك قيود
    // But we can add Arabic description if there are constraints
    
    if (hasConstraints() || hasUpperBound()) {
        std::ostringstream oss;
        oss << name_;
        
        if (hasUpperBound()) {
            oss << ": " << upperBound_->getArabicName();
        } else if (!constraints_.empty()) {
            oss << ": ";
            for (size_t i = 0; i < constraints_.size(); ++i) {
                if (i > 0) oss << " و ";
                oss << constraints_[i]->toString();
            }
        }
        
        return oss.str();
    }
    
    return name_;
}

std::string TypeParameter::getEnglishName() const {
    if (hasConstraints() || hasUpperBound()) {
        std::ostringstream oss;
        oss << name_;
        
        if (hasUpperBound()) {
            oss << ": " << upperBound_->getEnglishName();
        } else if (!constraints_.empty()) {
            oss << ": ";
            for (size_t i = 0; i < constraints_.size(); ++i) {
                if (i > 0) oss << " & ";
                oss << constraints_[i]->toString();
            }
        }
        
        return oss.str();
    }
    
    return name_;
}

std::string TypeParameter::toString() const {
    return getEnglishName();
}

bool TypeParameter::equals(const Type* other) const {
    if (!other) return false;
    
    if (other->getKind() != SadTypeKind::TypeParameter) return false;
    
    const TypeParameter* otherParam = static_cast<const TypeParameter*>(other);
    
    // مقارنة الأسماء / Compare names
    if (name_ != otherParam->name_) return false;
    
    // مقارنة الحدود / Compare bounds
    if (hasUpperBound() != otherParam->hasUpperBound()) return false;
    if (hasUpperBound()) {
        if (!upperBound_->equals(otherParam->upperBound_.get())) {
            return false;
        }
    }
    
    if (hasLowerBound() != otherParam->hasLowerBound()) return false;
    if (hasLowerBound()) {
        if (!lowerBound_->equals(otherParam->lowerBound_.get())) {
            return false;
        }
    }
    
    // مقارنة عدد القيود / Compare constraint count
    if (constraints_.size() != otherParam->constraints_.size()) return false;
    
    // مقارنة القيود / Compare constraints
    for (size_t i = 0; i < constraints_.size(); ++i) {
        if (!constraints_[i]->equals(*otherParam->constraints_[i])) {
            return false;
        }
    }
    
    return true;
}

std::shared_ptr<Type> TypeParameter::clone() const {
    auto cloned = std::make_shared<TypeParameter>(name_, constraints_);
    cloned->setUpperBound(upperBound_);
    cloned->setLowerBound(lowerBound_);
    cloned->setVariance(variance_);
    return cloned;
}

// ════════════════════════════════════════════════════════════════════════════════
// الدوال الخاصة بمعاملات الأنواع / Type Parameter-Specific Methods
// Source: type_parameter.h:92-220
// ════════════════════════════════════════════════════════════════════════════════

void TypeParameter::addConstraint(const TypeConstraintPtr& constraint) {
    if (constraint) {
        constraints_.push_back(constraint);
    }
}

bool TypeParameter::satisfiesConstraints(const TypePtr& type) const {
    if (!type) return false;
    
    // التحقق من النوع العلوي / Check upper bound
    if (hasUpperBound()) {
        if (!type->isSubtypeOf(upperBound_.get())) {
            return false;
        }
    }
    
    // التحقق من النوع السفلي / Check lower bound
    if (hasLowerBound()) {
        if (!lowerBound_->isSubtypeOf(type.get())) {
            return false;
        }
    }
    
    // التحقق من القيود / Check constraints
    for (const auto& constraint : constraints_) {
        if (!checkConstraint(constraint, type)) {
            return false;
        }
    }
    
    return true;
}

// ════════════════════════════════════════════════════════════════════════════════
// علاقات الأنواع / Type Relations
// Source: type_parameter.h:227-242
// ════════════════════════════════════════════════════════════════════════════════

bool TypeParameter::isAssignableTo(const Type* other) const {
    if (!other) return false;
    
    // معامل النوع يمكن تحويله إلى:
    // Type parameter can be converted to:
    
    // 1. معامل نوع آخر بنفس الاسم / Another type parameter with same name
    if (other->getKind() == SadTypeKind::TypeParameter) {
        const TypeParameter* otherParam = static_cast<const TypeParameter*>(other);
        return name_ == otherParam->name_;
    }
    
    // 2. النوع العلوي / Upper bound
    if (hasUpperBound()) {
        if (upperBound_->equals(other)) {
            return true;
        }
        
        // أو أي نوع أعلى من النوع العلوي / Or any supertype of upper bound
        if (upperBound_->isSubtypeOf(other)) {
            return true;
        }
    }
    
    // 3. أي قيد من القيود / Any constraint
    for (const auto& constraint : constraints_) {
        if (constraint->isProtocolConstraint() || constraint->isSubtypeConstraint()) {
            TypePtr constraintType = constraint->getConstraintType();
            if (constraintType && constraintType->equals(other)) {
                return true;
            }
        }
    }
    
    return false;
}

bool TypeParameter::isSubtypeOf(const Type* other) const {
    // نفس المنطق / Same logic
    return isAssignableTo(other);
}

// ════════════════════════════════════════════════════════════════════════════════
// الدوال الداخلية / Internal Methods
// Source: type_parameter.h:249-257
// ════════════════════════════════════════════════════════════════════════════════

bool TypeParameter::checkConstraint(const TypeConstraintPtr& constraint, 
                                     const TypePtr& type) const {
    if (!constraint || !type) return false;
    
    // استخدام دالة isSatisfied من TypeConstraint
    // Use isSatisfied from TypeConstraint
    return constraint->isSatisfied(type);
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة / Helper Functions
// Source: type_parameter.h:270-330
// ════════════════════════════════════════════════════════════════════════════════

TypeParameterPtr makeTypeParameter(const std::string& name) {
    return std::make_shared<TypeParameter>(name);
}

TypeParameterPtr makeTypeParameterWithBound(const std::string& name, TypePtr upperBound) {
    return std::make_shared<TypeParameter>(name, upperBound);
}

TypeParameterPtr makeTypeParameterWithConstraints(const std::string& name, 
                                                   const std::vector<TypeConstraintPtr>& constraints) {
    return std::make_shared<TypeParameter>(name, constraints);
}

bool isTypeParameter(const TypePtr& type) {
    return type && type->getKind() == SadTypeKind::TypeParameter;
}

TypeParameterPtr asTypeParameter(const TypePtr& type) {
    if (!isTypeParameter(type)) {
        return nullptr;
    }
    
    return std::static_pointer_cast<TypeParameter>(type);
}

std::vector<TypeParameterPtr> makeTypeParameterList(const std::vector<std::string>& names) {
    std::vector<TypeParameterPtr> params;
    params.reserve(names.size());
    
    for (const auto& name : names) {
        params.push_back(makeTypeParameter(name));
    }
    
    return params;
}

} // namespace TypeSystem
} // namespace Sad
