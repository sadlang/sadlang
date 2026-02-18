// ======================================================================
// class_manager.cpp - تنفيذ مدير الأصناف / Class Manager Implementation
// ======================================================================
// الوصف بالعربية:
//   تنفيذ جميع دوال ClassManager المُعرّفة في class_manager.h
//   يدير تسجيل وتتبع جميع الأصناف في البرنامج
//
// English Description:
//   Implementation of all ClassManager functions defined in class_manager.h
//   Manages registration and tracking of all classes in program
// ======================================================================

#include "class_manager.h"
#include <iostream>
#include <algorithm>
#include <unordered_set>

namespace Sad {
namespace Data {

// ======================================================================
// Singleton Instance / النسخة المفردة
// ======================================================================

ClassManager* ClassManager::instance_ = nullptr;

ClassManager* ClassManager::getInstance() {
    // (AR) الحصول على النسخة الوحيدة من المدير
    // (EN) Get singleton instance of manager
    
    if (!instance_) {
        instance_ = new ClassManager();
    }
    return instance_;
}

void ClassManager::resetInstance() {
    // (AR) إعادة تعيين المدير (للاختبارات)
    // (EN) Reset manager (for testing)
    
    if (instance_) {
        delete instance_;
        instance_ = nullptr;
    }
}

// ======================================================================
// تسجيل الأصناف / Class Registration
// ======================================================================

bool ClassManager::registerClass(std::unique_ptr<ClassType> classType) {
    // (AR) تسجيل صنف جديد
    // (EN) Register new class
    
    if (!classType) return false;
    
    const std::string& className = classType->name;
    
    // (AR) فحص عدم وجود اسم مكرر - أو تحديث الموجود
    // (EN) Check for duplicate name - or update existing
    auto it = classes_.find(className);
    if (it != classes_.end()) {
        // (AR) إذا كان الصنف الموجود فارغاً (تسجيل مؤقت)، استبدله
        // (EN) If existing class is empty (temporary registration), replace it
        if (it->second->fields.empty() && it->second->methods.empty()) {
            #ifdef DEBUG_OOP
std::cout << "[ClassManager] تحديث التسجيل المؤقت للصنف: " << className << "\n";
#endif
            classes_[className] = std::move(classType);
            return true;
        }
        
        std::cerr << "خطأ: الصنف '" << className << "' مسجل مسبقاً\n";
        std::cerr << "Error: Class '" << className << "' already registered\n";
        return false;
    }
    
    // (AR) تسجيل الصنف
    // (EN) Register class
    classes_[className] = std::move(classType);
    
    return true;
}

bool ClassManager::registerClass(ClassType* classType) {
    // (AR) تسجيل صنف بمؤشر عادي (للتوافق القديم)
    // (EN) Register class with raw pointer (legacy compatibility)
    
    if (!classType) return false;
    
    return registerClass(std::unique_ptr<ClassType>(classType));
}

// ======================================================================
// البحث والوصول / Lookup and Access
// ======================================================================

ClassType* ClassManager::getClass(const std::string& className) {
    // (AR) الحصول على صنف بالاسم
    // (EN) Get class by name
    
    auto it = classes_.find(className);
    if (it != classes_.end()) {
        return it->second.get();
    }
    
    return nullptr;
}

const ClassType* ClassManager::getClass(const std::string& className) const {
    // (AR) الحصول على صنف بالاسم (نسخة const)
    // (EN) Get class by name (const version)
    
    auto it = classes_.find(className);
    if (it != classes_.end()) {
        return it->second.get();
    }
    
    return nullptr;
}

bool ClassManager::hasClass(const std::string& className) const {
    // (AR) فحص وجود صنف
    // (EN) Check if class exists
    
    return classes_.find(className) != classes_.end();
}

size_t ClassManager::getClassCount() const {
    // (AR) عدد الأصناف المسجلة
    // (EN) Number of registered classes
    
    return classes_.size();
}

std::vector<std::string> ClassManager::getAllClassNames() const {
    // (AR) قائمة بأسماء جميع الأصناف
    // (EN) List of all class names
    
    std::vector<std::string> names;
    names.reserve(classes_.size());
    
    for (const auto& pair : classes_) {
        names.push_back(pair.first);
    }
    
    return names;
}

// ======================================================================
// الحذف / Removal
// ======================================================================

bool ClassManager::removeClass(const std::string& className) {
    // (AR) حذف صنف
    // (EN) Remove class
    
    auto it = classes_.find(className);
    if (it == classes_.end()) {
        return false;
    }
    
    classes_.erase(it);
    return true;
}

void ClassManager::clearAll() {
    // (AR) حذف جميع الأصناف
    // (EN) Remove all classes
    
    classes_.clear();
}

// ======================================================================
// التحقق من الوراثة / Inheritance Checking
// ======================================================================

bool ClassManager::validateInheritanceChain(const std::string& className) const {
    // (AR) التحقق من صحة سلسلة الوراثة
    // (EN) Validate inheritance chain
    
    auto it = classes_.find(className);
    if (it == classes_.end()) {
        return false;
    }
    
    ClassType* classType = it->second.get();
    
    // (AR) فحص وجود جميع الأصناف الأساسية
    // (EN) Check existence of all base classes
    ClassType* current = classType->baseClass;
    while (current) {
        if (!hasClass(current->name)) {
            std::cerr << "خطأ: الصنف الأساسي '" << current->name << "' غير موجود\n";
            std::cerr << "Error: Base class '" << current->name << "' not found\n";
            return false;
        }
        current = current->baseClass;
    }
    
    // (AR) فحص عدم وجود دوائر في الوراثة
    // (EN) Check for circular inheritance
    std::unordered_set<std::string> visited;
    return !hasCircularInheritance(className, visited);
}

bool ClassManager::hasCircularInheritance(const std::string& className,
                                          std::unordered_set<std::string>& visited) const {
    // (AR) فحص وجود دوائر في الوراثة
    // (EN) Check for circular inheritance
    
    // (AR) إذا زرنا هذا الصنف مسبقاً، يوجد دائرة
    // (EN) If we visited this class before, there's a cycle
    if (visited.find(className) != visited.end()) {
        std::cerr << "خطأ: دائرة في الوراثة عند الصنف '" << className << "'\n";
        std::cerr << "Error: Circular inheritance at class '" << className << "'\n";
        return true;
    }
    
    auto it = classes_.find(className);
    if (it == classes_.end()) {
        return false;
    }
    
    ClassType* classType = it->second.get();
    if (!classType->baseClass) {
        return false;
    }
    
    // (AR) تمييز الصنف كمزور
    // (EN) Mark class as visited
    visited.insert(className);
    
    // (AR) فحص الصنف الأساسي
    // (EN) Check base class
    bool hasCycle = hasCircularInheritance(classType->baseClass->name, visited);
    
    // (AR) إزالة التمييز
    // (EN) Unmark class
    visited.erase(className);
    
    return hasCycle;
}

std::vector<std::string> ClassManager::getInheritanceChain(const std::string& className) const {
    // (AR) الحصول على سلسلة الوراثة الكاملة
    // (EN) Get complete inheritance chain
    
    std::vector<std::string> chain;
    
    auto it = classes_.find(className);
    if (it == classes_.end()) {
        return chain;
    }
    
    ClassType* current = it->second.get();
    
    // (AR) بناء السلسلة من الأعلى للأسفل
    // (EN) Build chain from top to bottom
    while (current) {
        chain.insert(chain.begin(), current->name);
        current = current->baseClass;
    }
    
    return chain;
}

// ======================================================================
// التصحيح والمعلومات / Debugging and Info
// ======================================================================

void ClassManager::printDebugInfo() const {
    // (AR) طباعة معلومات جميع الأصناف
    // (EN) Print information about all classes
    
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════╗\n";
    std::cout << "║         CLASS MANAGER DEBUG INFO             ║\n";
    std::cout << "╠══════════════════════════════════════════════╣\n";
    std::cout << "║ Total Classes: " << classes_.size() << "\n";
    std::cout << "╠══════════════════════════════════════════════╣\n";
    
    for (const auto& pair : classes_) {
        std::cout << "║ \n";
        pair.second->printDebugInfo();
    }
    
    std::cout << "╚══════════════════════════════════════════════╝\n";
}

void ClassManager::printClassInfo(const std::string& className) const {
    // (AR) طباعة معلومات صنف معين
    // (EN) Print information about specific class
    
    auto it = classes_.find(className);
    if (it == classes_.end()) {
        std::cerr << "خطأ: الصنف '" << className << "' غير موجود\n";
        std::cerr << "Error: Class '" << className << "' not found\n";
        return;
    }
    
    it->second->printDebugInfo();
}

std::string ClassManager::getStatistics() const {
    // (AR) الحصول على إحصائيات
    // (EN) Get statistics
    
    std::ostringstream oss;
    oss << "╔═══════════════════════════════════╗\n";
    oss << "║  ClassManager Statistics          ║\n";
    oss << "╠═══════════════════════════════════╣\n";
    oss << "║ Total Classes: " << classes_.size() << "\n";
    
    size_t totalFields = 0;
    size_t totalMethods = 0;
    size_t withConstructor = 0;
    size_t withDestructor = 0;
    size_t withInheritance = 0;
    
    for (const auto& pair : classes_) {
        ClassType* cls = pair.second.get();
        totalFields += cls->fields.size();
        totalMethods += cls->methods.size();
        if (cls->hasConstructor()) withConstructor++;
        if (cls->hasDestructor()) withDestructor++;
        if (cls->baseClass) withInheritance++;
    }
    
    oss << "║ Total Fields: " << totalFields << "\n";
    oss << "║ Total Methods: " << totalMethods << "\n";
    oss << "║ With Constructor: " << withConstructor << "\n";
    oss << "║ With Destructor: " << withDestructor << "\n";
    oss << "║ With Inheritance: " << withInheritance << "\n";
    oss << "╚═══════════════════════════════════╝\n";
    
    return oss.str();
}

// ======================================================================
// الواجهات والسمات / Traits and Interfaces
// ======================================================================

bool ClassManager::registerTrait(TraitDefinition trait) {
    if (traits_.find(trait.name) != traits_.end()) {
        return false; // (AR) السمة موجودة مسبقاً
    }
    std::string name = trait.name;
    traits_[name] = std::move(trait);
    return true;
}

const TraitDefinition* ClassManager::getTrait(const std::string& traitName) const {
    auto it = traits_.find(traitName);
    if (it != traits_.end()) {
        return &it->second;
    }
    return nullptr;
}

bool ClassManager::hasTrait(const std::string& traitName) const {
    return traits_.find(traitName) != traits_.end();
}

bool ClassManager::validateTraitImpl(const std::string& className, const std::string& traitName) const {
    auto* cls = const_cast<ClassManager*>(this)->getClass(className);
    auto* trait = getTrait(traitName);
    if (!cls || !trait) return false;
    
    // (AR) التحقق من أن الصنف يحتوي على جميع الدوال المطلوبة
    for (const auto& requiredMethod : trait->requiredMethods) {
        if (requiredMethod.hasDefaultImpl) continue; // has default, ok to skip
        
        bool found = false;
        for (const auto& classMethod : cls->methods) {
            if (classMethod.name == requiredMethod.name) {
                found = true;
                break;
            }
        }
        // (AR) البحث في سلسلة الوراثة
        if (!found) {
            ClassType* parent = cls->baseClass;
            while (parent && !found) {
                for (const auto& m : parent->methods) {
                    if (m.name == requiredMethod.name) {
                        found = true;
                        break;
                    }
                }
                parent = parent->baseClass;
            }
        }
        
        if (!found) return false;
    }
    
    // (AR) التحقق من السمات الأساسية أيضاً
    for (const auto& superTrait : trait->superTraits) {
        if (!validateTraitImpl(className, superTrait)) return false;
    }
    
    return true;
}

bool ClassManager::registerTraitImpl(const std::string& className, const std::string& traitName) {
    auto* cls = getClass(className);
    if (!cls) return false;
    if (!hasTrait(traitName)) return false;
    
    // (AR) التحقق من التنفيذ
    if (!validateTraitImpl(className, traitName)) return false;
    
    // (AR) تسجيل أن الصنف ينفذ الواجهة
    cls->implementedTraits.push_back(traitName);
    return true;
}

bool ClassManager::classImplementsTrait(const std::string& className, const std::string& traitName) const {
    auto it = classes_.find(className);
    if (it == classes_.end()) return false;
    
    const auto& traits = it->second->implementedTraits;
    for (const auto& t : traits) {
        if (t == traitName) return true;
    }
    
    // (AR) البحث في سلسلة الوراثة
    if (it->second->baseClass) {
        return classImplementsTrait(it->second->baseClass->name, traitName);
    }
    
    return false;
}

} // namespace Data
} // namespace Sad

// ======================================================================
// نهاية الملف / End of File
// ======================================================================
