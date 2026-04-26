// ════════════════════════════════════════════════════════════════════════════════
// ملف: constraint.cpp
// File: constraint.cpp
//
// الوصف: تنفيذ قيود الأنواع (Type Constraints)
// Description: Implementation of type constraints
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
// ════════════════════════════════════════════════════════════════════════════════

#include "constraint.h"      // المصدر / Source (constraint.h:1-end)
#include <sstream>           // لـ ostringstream / For ostringstream

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ الصنف: Constraint
// Class Implementation: Constraint
// المصدر / Source: constraint.h:45-140
// ════════════════════════════════════════════════════════════════════════════════

// المُنشئ الرئيسي / Main constructor
// التعريف / Definition: constraint.h:60
Constraint::Constraint(ConstraintKind kind, 
                     TypePtr type1, 
                     TypePtr type2,
                     const std::string& location)
    : kind_(kind),         // تعيين نوع القيد / Set constraint kind
      type1_(type1),       // تعيين النوع الأول / Set first type
      type2_(type2),       // تعيين النوع الثاني / Set second type
      location_(location)  // تعيين الموقع / Set location
{
}

// منشئ النسخ / Copy constructor
// التعريف / Definition: constraint.h:67
Constraint::Constraint(const Constraint& other)
    : kind_(other.kind_),
      type1_(other.type1_),
      type2_(other.type2_),
      location_(other.location_)
{
}

// منشئ النقل / Move constructor  
// التعريف / Definition: constraint.h:70
Constraint::Constraint(Constraint&& other) noexcept
    : kind_(other.kind_),
      type1_(std::move(other.type1_)),
      type2_(std::move(other.type2_)),
      location_(std::move(other.location_))
{
}

// معامل تعيين النسخ / Copy assignment
// التعريف / Definition: constraint.h:78
Constraint& Constraint::operator=(const Constraint& other) {
    if (this != &other) {
        kind_ = other.kind_;
        type1_ = other.type1_;
        type2_ = other.type2_;
        location_ = other.location_;
    }
    return *this;
}

// معامل تعيين النقل / Move assignment
// التعريف / Definition: constraint.h:81
Constraint& Constraint::operator=(Constraint&& other) noexcept {
    if (this != &other) {
        kind_ = other.kind_;
        type1_ = std::move(other.type1_);
        type2_ = std::move(other.type2_);
        location_ = std::move(other.location_);
    }
    return *this;
}

// تحويل القيد إلى نص تمثيلي / Convert constraint to string
// التعريف / Definition: constraint.h:120
std::string Constraint::toString() const {
    std::ostringstream oss;
    
    // إضافة النوع الأول / Add first type
    if (type1_) {
        oss << type1_->toString();  // type1_ من (constraint.h:183)
    } else {
        oss << "null";
    }
    
    // إضافة رمز القيد / Add constraint symbol
    oss << " " << kindToString(kind_) << " ";  // kind_ من (constraint.h:179)
    
    // إضافة النوع الثاني / Add second type
    if (type2_) {
        oss << type2_->toString();  // type2_ من (constraint.h:186)
    } else {
        oss << "null";
    }
    
    // إضافة الموقع إن وُجد / Add location if exists
    if (!location_.empty()) {  // location_ من (constraint.h:190)
        oss << " at " << location_;
    }
    
    return oss.str();
}

// تحويل نوع القيد إلى نص / Convert constraint kind to string
// التعريف / Definition: constraint.h:125
std::string Constraint::kindToString(ConstraintKind kind) {
    switch (kind) {
        case ConstraintKind::Equality:   return "=";
        case ConstraintKind::Subtype:    return "<:";
        case ConstraintKind::HasMember:  return "has";
        case ConstraintKind::Callable:   return "callable";
        case ConstraintKind::Iterable:   return "iterable";
        default:                         return "?";
    }
}

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ الصنف: ConstraintSet
// Class Implementation: ConstraintSet
// المصدر / Source: constraint.h:145-254
// ════════════════════════════════════════════════════════════════════════════════

// منشئ افتراضي / Default constructor
// التعريف / Definition: constraint.h:154
ConstraintSet::ConstraintSet() {
    // لا يوجد عمل إضافي / No additional work
}

// منشئ مع قائمة قيود / Constructor with constraint list
// التعريف / Definition: constraint.h:157
ConstraintSet::ConstraintSet(const std::vector<Constraint>& constraints)
    : constraints_(constraints)  // نسخ القائمة / Copy list
{
}

// إضافة قيد / Add constraint
// التعريف / Definition: constraint.h:167
void ConstraintSet::add(const Constraint& constraint) {
    constraints_.push_back(constraint);  // constraints_ من (constraint.h:251)
}

// إضافة قيد مساواة / Add equality constraint
// التعريف / Definition: constraint.h:174
void ConstraintSet::addEquality(TypePtr type1, TypePtr type2, const std::string& location) {
    add(Constraint(ConstraintKind::Equality, type1, type2, location));
}

// إضافة قيد نوع فرعي / Add subtype constraint
// التعريف / Definition: constraint.h:181
void ConstraintSet::addSubtype(TypePtr subtype, TypePtr supertype, const std::string& location) {
    add(Constraint(ConstraintKind::Subtype, subtype, supertype, location));
}

// دمج مجموعة قيود أخرى / Merge another constraint set
// التعريف / Definition: constraint.h:186
void ConstraintSet::merge(const ConstraintSet& other) {
    constraints_.insert(constraints_.end(), 
                       other.constraints_.begin(), 
                       other.constraints_.end());
}

// مسح جميع القيود / Clear all constraints
// التعريف / Definition: constraint.h:189
void ConstraintSet::clear() {
    constraints_.clear();  // constraints_ من (constraint.h:251)
}

// تحويل المجموعة إلى نص تمثيلي / Convert set to string
// التعريف / Definition: constraint.h:206
std::string ConstraintSet::toString() const {
    if (constraints_.empty()) {
        return "{}";
    }
    
    std::ostringstream oss;
    oss << "{";
    
    bool first = true;
    for (const auto& c : constraints_) {  // constraints_ من (constraint.h:251)
        if (!first) {
            oss << ", ";
        }
        first = false;
        oss << c.toString();
    }
    
    oss << "}";
    return oss.str();
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة / Helper Functions
// المصدر / Source: constraint.h:258-279
// ════════════════════════════════════════════════════════════════════════════════

// إنشاء قيد مساواة / Create equality constraint
// التعريف / Definition: constraint.h:263
Constraint createEqualityConstraint(TypePtr type1, TypePtr type2, 
                                   const std::string& location) {
    return Constraint(ConstraintKind::Equality, type1, type2, location);
}

// إنشاء قيد نوع فرعي / Create subtype constraint
// التعريف / Definition: constraint.h:272
Constraint createSubtypeConstraint(TypePtr subtype, TypePtr supertype,
                                  const std::string& location) {
    return Constraint(ConstraintKind::Subtype, subtype, supertype, location);
}

} // namespace TypeSystem
} // namespace Sad
