// ════════════════════════════════════════════════════════════════════════════════
// ملف: union_type.cpp
// File: union_type.cpp
//
// الوصف: تنفيذ أنواع الاتحاد (Union Types)
// Description: Union Types implementation
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// المصدر: compiler/type_system/include/union_type.h
// Source: compiler/type_system/include/union_type.h
// ════════════════════════════════════════════════════════════════════════════════

#include "union_type.h"
#include <sstream>
#include <algorithm>
#include <unordered_set>

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// المُنشئات / Constructors
// ════════════════════════════════════════════════════════════════════════════════

UnionType::UnionType(const TypeList& alternatives)
    : Type(SadTypeKind::Union)
    , alternatives_(alternatives)
{
    // تسطيح الأنواع المتداخلة / Flatten nested unions
    flatten();
    
    // إزالة التكرار / Remove duplicates
    removeDuplicates();
    
    // ترتيب الأنواع / Sort alternatives
    sortAlternatives();
}

UnionType::UnionType(TypePtr type1, TypePtr type2)
    : Type(SadTypeKind::Union)
    , alternatives_({type1, type2})
{
    flatten();
    removeDuplicates();
    sortAlternatives();
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال مجردة مُطبقة / Implemented Abstract Functions
// ════════════════════════════════════════════════════════════════════════════════

std::string UnionType::getArabicName() const {
    if (alternatives_.empty()) return "فراغ";
    
    std::ostringstream oss;
    for (size_t i = 0; i < alternatives_.size(); ++i) {
        if (i > 0) oss << " | ";
        oss << alternatives_[i]->getArabicName();
    }
    return oss.str();
}

std::string UnionType::getEnglishName() const {
    if (alternatives_.empty()) return "void";
    
    std::ostringstream oss;
    for (size_t i = 0; i < alternatives_.size(); ++i) {
        if (i > 0) oss << " | ";
        oss << alternatives_[i]->getEnglishName();
    }
    return oss.str();
}

std::string UnionType::toString() const {
    return getEnglishName();
}

bool UnionType::equals(const Type* other) const {
    if (!other) return false;
    if (other->getKind() != SadTypeKind::Union) return false;
    
    const UnionType* otherUnion = static_cast<const UnionType*>(other);
    
    // التحقق من العدد / Check count
    if (alternatives_.size() != otherUnion->alternatives_.size()) {
        return false;
    }
    
    // التحقق من جميع الأنواع / Check all types
    for (size_t i = 0; i < alternatives_.size(); ++i) {
        if (!alternatives_[i]->equals(otherUnion->alternatives_[i].get())) {
            return false;
        }
    }
    
    return true;
}

std::shared_ptr<Type> UnionType::clone() const {
    TypeList clonedAlternatives;
    for (const auto& alt : alternatives_) {
        clonedAlternatives.push_back(alt->clone());
    }
    return std::make_shared<UnionType>(clonedAlternatives);
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال الأنواع الاتحادية / Union Type-Specific Methods
// ════════════════════════════════════════════════════════════════════════════════

bool UnionType::contains(const TypePtr& type) const {
    if (!type) return false;
    
    for (const auto& alt : alternatives_) {
        if (alt->equals(type.get())) {
            return true;
        }
    }
    
    return false;
}

bool UnionType::containsTypeName(const std::string& typeName) const {
    for (const auto& alt : alternatives_) {
        // Check both Arabic and English names, and toString()
        if (alt->toString() == typeName ||
            alt->getArabicName() == typeName ||
            alt->getEnglishName() == typeName) {
            return true;
        }
    }
    return false;
}

UnionTypePtr UnionType::simplify() const {
    // إذا كانت القائمة فارغة / If empty
    if (alternatives_.empty()) {
        return nullptr;
    }
    
    // إذا كان هناك نوع واحد فقط / If only one type
    if (alternatives_.size() == 1) {
        // لا نحتاج union / Don't need union
        return nullptr;
    }
    
    // إنشاء نسخة جديدة / Create new copy
    TypeList simplified = alternatives_;
    
    // إزالة التكرار / Remove duplicates
    auto it = std::unique(simplified.begin(), simplified.end(),
        [](const TypePtr& a, const TypePtr& b) {
            return a->equals(b.get());
        });
    simplified.erase(it, simplified.end());
    
    // إذا بقي نوع واحد / If one type remains
    if (simplified.size() == 1) {
        return nullptr;
    }
    
    return std::make_shared<UnionType>(simplified);
}

UnionTypePtr UnionType::intersection(const UnionTypePtr& other) const {
    if (!other) return nullptr;
    
    TypeList result;
    
    // إيجاد الأنواع المشتركة / Find common types
    for (const auto& myType : alternatives_) {
        if (other->contains(myType)) {
            result.push_back(myType);
        }
    }
    
    if (result.empty()) return nullptr;
    
    return std::make_shared<UnionType>(result);
}

UnionTypePtr UnionType::unionWith(const UnionTypePtr& other) const {
    if (!other) return std::make_shared<UnionType>(alternatives_);
    
    TypeList result = alternatives_;
    
    // إضافة الأنواع من الآخر / Add types from other
    for (const auto& otherType : other->getAlternatives()) {
        bool found = false;
        for (const auto& myType : alternatives_) {
            if (myType->equals(otherType.get())) {
                found = true;
                break;
            }
        }
        if (!found) {
            result.push_back(otherType);
        }
    }
    
    return std::make_shared<UnionType>(result);
}

UnionTypePtr UnionType::difference(const TypePtr& type) const {
    if (!type) return std::make_shared<UnionType>(alternatives_);
    
    TypeList result;
    
    // إزالة النوع المُحدد / Remove specified type
    for (const auto& alt : alternatives_) {
        if (!alt->equals(type.get())) {
            result.push_back(alt);
        }
    }
    
    if (result.empty()) return nullptr;
    
    return std::make_shared<UnionType>(result);
}

UnionTypePtr UnionType::difference(const UnionTypePtr& other) const {
    if (!other) return std::make_shared<UnionType>(alternatives_);
    
    TypeList result;
    
    // إزالة جميع الأنواع الموجودة في other / Remove all types in other
    for (const auto& alt : alternatives_) {
        if (!other->contains(alt)) {
            result.push_back(alt);
        }
    }
    
    if (result.empty()) return nullptr;
    
    return std::make_shared<UnionType>(result);
}

TypePtr UnionType::getAlternative(size_t index) const {
    if (index >= alternatives_.size()) {
        return nullptr;
    }
    return alternatives_[index];
}

bool UnionType::containsNull() const {
    for (const auto& alt : alternatives_) {
        if (alt->getKind() == SadTypeKind::Void) {
            return true;
        }
    }
    return false;
}

UnionTypePtr UnionType::removeNull() const {
    TypeList result;
    
    for (const auto& alt : alternatives_) {
        if (alt->getKind() != SadTypeKind::Void) {
            result.push_back(alt);
        }
    }
    
    if (result.empty()) return nullptr;
    
    return std::make_shared<UnionType>(result);
}

// ════════════════════════════════════════════════════════════════════════════════
// علاقات الأنواع / Type Relations
// ════════════════════════════════════════════════════════════════════════════════

bool UnionType::isAssignableTo(const Type* other) const {
    if (!other) return false;
    
    // التحقق من التساوي / Check equality
    if (equals(other)) return true;
    
    // إذا كان الآخر union / If other is union
    if (other->getKind() == SadTypeKind::Union) {
        const UnionType* otherUnion = static_cast<const UnionType*>(other);
        
        // جميع بدائلنا يجب أن تكون في الآخر / All our alternatives must be in other
        for (const auto& alt : alternatives_) {
            if (!otherUnion->contains(alt)) {
                return false;
            }
        }
        
        return true;
    }
    
    // إذا لم يكن union، نتحقق إذا كانت جميع بدائلنا assignable
    // If not union, check if all our alternatives are assignable
    for (const auto& alt : alternatives_) {
        if (!alt->isAssignableTo(other)) {
            return false;
        }
    }
    
    return true;
}

bool UnionType::isSubtypeOf(const Type* other) const {
    if (!other) return false;
    
    // Union هو subtype إذا كانت جميع بدائله subtypes
    // Union is subtype if all alternatives are subtypes
    for (const auto& alt : alternatives_) {
        if (!alt->isSubtypeOf(other)) {
            return false;
        }
    }
    
    return true;
}

bool UnionType::isSuperTypeOf(const TypePtr& type) const {
    if (!type) return false;
    
    // Union هو supertype إذا كان أحد بدائله supertype
    // Union is supertype if one of alternatives is supertype
    for (const auto& alt : alternatives_) {
        if (type->isSubtypeOf(alt.get())) {
            return true;
        }
    }
    
    return false;
}

// ════════════════════════════════════════════════════════════════════════════════
// الدوال الداخلية / Internal Methods
// ════════════════════════════════════════════════════════════════════════════════

void UnionType::flatten() {
    // تسطيح الأنواع المتداخلة / Flatten nested unions
    // (Integer | (String | Boolean)) → Integer | String | Boolean
    
    TypeList flattened;
    
    for (const auto& alt : alternatives_) {
        if (alt->getKind() == SadTypeKind::Union) {
            // إذا كان النوع union، أضف بدائله / If type is union, add its alternatives
            const UnionType* nestedUnion = static_cast<const UnionType*>(alt.get());
            for (const auto& nestedAlt : nestedUnion->getAlternatives()) {
                flattened.push_back(nestedAlt);
            }
        } else {
            flattened.push_back(alt);
        }
    }
    
    alternatives_ = flattened;
}

void UnionType::removeDuplicates() {
    // إزالة الأنواع المكررة / Remove duplicate types
    
    TypeList unique;
    
    for (const auto& alt : alternatives_) {
        bool isDuplicate = false;
        
        for (const auto& existing : unique) {
            if (existing->equals(alt.get())) {
                isDuplicate = true;
                break;
            }
        }
        
        if (!isDuplicate) {
            unique.push_back(alt);
        }
    }
    
    alternatives_ = unique;
}

void UnionType::sortAlternatives() {
    // ترتيب الأنواع للحصول على تمثيل متسق
    // Sort types for consistent representation
    
    std::sort(alternatives_.begin(), alternatives_.end(),
        [](const TypePtr& a, const TypePtr& b) {
            return a->toString() < b->toString();
        });
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة / Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

UnionTypePtr makeUnionType(const TypeList& alternatives) {
    return std::make_shared<UnionType>(alternatives);
}

UnionTypePtr makeUnionType(TypePtr type1, TypePtr type2) {
    return std::make_shared<UnionType>(type1, type2);
}

UnionTypePtr makeUnionType(TypePtr type1, TypePtr type2, TypePtr type3) {
    return std::make_shared<UnionType>(TypeList{type1, type2, type3});
}

bool isUnionType(const TypePtr& type) {
    return type && type->getKind() == SadTypeKind::Union;
}

UnionTypePtr asUnionType(const TypePtr& type) {
    if (!isUnionType(type)) {
        return nullptr;
    }
    return std::static_pointer_cast<UnionType>(type);
}

TypePtr unionOf(TypePtr type1, TypePtr type2) {
    if (!type1) return type2;
    if (!type2) return type1;
    
    // إذا كانا نفس النوع / If same type
    if (type1->equals(type2.get())) {
        return type1;
    }
    
    // إذا كان أحدهما subtype من الآخر / If one is subtype of other
    if (type1->isSubtypeOf(type2.get())) {
        return type2;  // النوع الأعم / The more general type
    }
    if (type2->isSubtypeOf(type1.get())) {
        return type1;
    }
    
    // إنشاء union / Create union
    return makeUnionType(type1, type2);
}

TypePtr unionOf(const TypeList& types) {
    if (types.empty()) return nullptr;
    if (types.size() == 1) return types[0];
    
    auto result = makeUnionType(types);
    auto simplified = result->simplify();
    
    // إذا تم التبسيط إلى نوع واحد / If simplified to one type
    if (!simplified) {
        return types[0];
    }
    
    return simplified;
}

TypePtr intersectionOf(TypePtr type1, TypePtr type2) {
    if (!type1 || !type2) return nullptr;
    
    // إذا كانا نفس النوع / If same type
    if (type1->equals(type2.get())) {
        return type1;
    }
    
    // إذا كان أحدهما subtype من الآخر / If one is subtype
    if (type1->isSubtypeOf(type2.get())) {
        return type1;  // النوع الأخص / The more specific type
    }
    if (type2->isSubtypeOf(type1.get())) {
        return type2;
    }
    
    // إذا كان كلاهما union / If both are unions
    if (isUnionType(type1) && isUnionType(type2)) {
        auto union1 = asUnionType(type1);
        auto union2 = asUnionType(type2);
        auto result = union1->intersection(union2);
        
        // إذا كان التقاطع عنصر واحد، أرجعه مباشرة
        // If intersection is single element, return it directly
        if (result && result->size() == 1) {
            return result->getAlternative(0);
        }
        
        return result;
    }
    
    // إذا كان أحدهما union / If one is union
    if (isUnionType(type1)) {
        auto union1 = asUnionType(type1);
        if (union1->contains(type2)) {
            return type2;
        }
    }
    
    if (isUnionType(type2)) {
        auto union2 = asUnionType(type2);
        if (union2->contains(type1)) {
            return type1;
        }
    }
    
    // لا يوجد تقاطع / No intersection
    return nullptr;
}

TypePtr differenceOf(TypePtr from, TypePtr toRemove) {
    if (!from) return nullptr;
    if (!toRemove) return from;
    
    // إذا كانا نفس النوع / If same type
    if (from->equals(toRemove.get())) {
        return nullptr;  // لا يبقى شيء / Nothing remains
    }
    
    // إذا كان from هو union / If from is union
    if (isUnionType(from)) {
        auto unionFrom = asUnionType(from);
        
        if (isUnionType(toRemove)) {
            // إزالة union من union / Remove union from union
            auto unionRemove = asUnionType(toRemove);
            return unionFrom->difference(unionRemove);
        } else {
            // إزالة نوع من union / Remove type from union
            return unionFrom->difference(toRemove);
        }
    }
    
    // إذا لم يكن union، لا يمكن الطرح / If not union, can't subtract
    return from;
}

} // namespace TypeSystem
} // namespace Sad
