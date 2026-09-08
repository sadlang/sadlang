// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: shared/types/src/type_environment.cpp
// (AR) تنفيذُ بيئةِ الأنواع — منقولٌ من `type_inferencer.cpp` المحذوف.
// (EN) Implementation moved out of the deleted type_inferencer.cpp.
// ═══════════════════════════════════════════════════════════════════════════════

#include "type_environment.h"

namespace Sad {
namespace Types {

TypeEnvironment::TypeEnvironment() 
    : parent_(nullptr) {}  // لا يوجد أب / No parent

// المنشئ مع أب / Constructor with parent
// المصدر: type_inferencer.h:48 / Source: type_inferencer.h:48
TypeEnvironment::TypeEnvironment(std::shared_ptr<TypeEnvironment> parent)
    : parent_(parent) {}  // تخزين المؤشر للأب / Store pointer to parent

// ربط اسم بنوع / Bind name to type
// المصدر: type_inferencer.h:53 / Source: type_inferencer.h:53
void TypeEnvironment::bind(const std::string& name, SadTypePtr type) {
    if (name.empty()) {
        // اسم فارغ غير مسموح / Empty name not allowed
        return;
    }
    
    if (!type) {
        // نوع null غير مسموح / null type not allowed
        return;
    }
    
    bindings_[name] = type;  // إضافة/تحديث الربط / Add/update binding
}

// البحث عن نوع باسمه / Lookup type by name
// المصدر: type_inferencer.h:58 / Source: type_inferencer.h:58
SadTypePtr TypeEnvironment::lookup(const std::string& name) const {
    // البحث في البيئة الحالية / Search in current environment
    auto it = bindings_.find(name);
    if (it != bindings_.end()) {
        return it->second;  // تم العثور / Found
    }
    
    // البحث في البيئة الأب / Search in parent environment
    if (parent_) {
        return parent_->lookup(name);  // بحث تكراري / Recursive search
    }
    
    return nullptr;  // لم يتم العثور / Not found
}

// التحقق من وجود اسم / Check if name exists
// المصدر: type_inferencer.h:63 / Source: type_inferencer.h:63
bool TypeEnvironment::contains(const std::string& name) const {
    return lookup(name) != nullptr;  // يوجد إذا لم يكن null / Exists if not null
}

// إزالة ربط / Remove binding
// المصدر: type_inferencer.h:68 / Source: type_inferencer.h:68
void TypeEnvironment::unbind(const std::string& name) {
    bindings_.erase(name);  // حذف من الخريطة / Erase from map
}

// إنشاء بيئة فرعية / Create child environment
// المصدر: type_inferencer.h:73 / Source: type_inferencer.h:73
std::shared_ptr<TypeEnvironment> TypeEnvironment::createChild() {
    // إنشاء بيئة فرعية مع ربطها بالبيئة الأب / Create child env linked to parent
    return std::make_shared<TypeEnvironment>(shared_from_this());
}

// تنظيف جميع الروابط / Clear all bindings
// المصدر: type_inferencer.h:78 / Source: type_inferencer.h:78
void TypeEnvironment::clear() {
    bindings_.clear();  // مسح الخريطة / Clear map
}

// الحصول على جميع الأسماء / Get all names
// المصدر: type_inferencer.h:83 / Source: type_inferencer.h:83
std::vector<std::string> TypeEnvironment::getAllNames() const {
    std::vector<std::string> names;  // قائمة الأسماء / List of names
    
    // إضافة الأسماء من البيئة الحالية / Add names from current environment
    for (const auto& [name, type] : bindings_) {
        (void)type;  // تجنب تحذير / Avoid warning
        names.push_back(name);  // إضافة الاسم / Add name
    }
    
    return names;  // إرجاع القائمة / Return list
}

} // namespace Types
} // namespace Sad
