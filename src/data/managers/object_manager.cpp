// ======================================================================
// object_manager.cpp - تنفيذ مدير الكائنات / Object Manager Implementation
// ======================================================================
// الوصف بالعربية:
//   تنفيذ جميع دوال ObjectManager المُعرّفة في object_manager.h
//   يدير دورة حياة جميع الكائنات وجمع القمامة
//
// English Description:
//   Implementation of all ObjectManager functions defined in object_manager.h
//   Manages lifecycle of all objects and garbage collection
// ======================================================================

#include "data/managers/object_manager.h"
#include "data/managers/class_manager.h"
#include <iostream>
#include <algorithm>
#include <map>

namespace Sad {
namespace Data {

// ======================================================================
// Singleton Instance / النسخة المفردة
// ======================================================================

ObjectManager* ObjectManager::instance_ = nullptr;

ObjectManager* ObjectManager::getInstance() {
    // (AR) الحصول على النسخة الوحيدة من المدير
    // (EN) Get singleton instance of manager
    
    if (!instance_) {
        instance_ = new ObjectManager();
    }
    return instance_;
}

void ObjectManager::resetInstance() {
    // (AR) إعادة تعيين المدير (للاختبارات)
    // (EN) Reset manager (for testing)
    
    if (instance_) {
        delete instance_;
        instance_ = nullptr;
    }
}

// ======================================================================
// إنشاء وتدمير الكائنات / Object Creation and Destruction
// ======================================================================

ObjectInstance* ObjectManager::createObject(const std::string& className) {
    // (AR) إنشاء كائن جديد من صنف معين
    // (EN) Create new object from specified class
    
    ClassManager* classMgr = ClassManager::getInstance();
    ClassType* classType = classMgr->getClass(className);
    
    if (!classType) {
        std::cerr << "خطأ: الصنف '" << className << "' غير موجود\n";
        std::cerr << "Error: Class '" << className << "' not found\n";
        return nullptr;
    }
    
    // (AR) إنشاء الكائن
    // (EN) Create object
    ObjectInstance* obj = classType->createInstance();
    
    if (!obj) {
        std::cerr << "خطأ: فشل إنشاء كائن من الصنف '" << className << "'\n";
        std::cerr << "Error: Failed to create object from class '" << className << "'\n";
        return nullptr;
    }
    
    // (AR) تسجيل الكائن
    // (EN) Register object
    size_t objId = obj->objectId;
    objects_[objId] = std::unique_ptr<ObjectInstance>(obj);
    
    return obj;
}

bool ObjectManager::deleteObject(size_t objectId) {
    // (AR) حذف كائن بالمُعرّف
    // (EN) Delete object by ID
    
    auto it = objects_.find(objectId);
    if (it == objects_.end()) {
        return false;
    }
    
    objects_.erase(it);
    return true;
}

void ObjectManager::deleteAllObjects() {
    // (AR) حذف جميع الكائنات
    // (EN) Delete all objects
    
    objects_.clear();
}

// ======================================================================
// البحث والوصول / Lookup and Access
// ======================================================================

ObjectInstance* ObjectManager::getObject(size_t objectId) {
    // (AR) الحصول على كائن بالمُعرّف
    // (EN) Get object by ID
    
    auto it = objects_.find(objectId);
    if (it != objects_.end()) {
        return it->second.get();
    }
    
    return nullptr;
}

const ObjectInstance* ObjectManager::getObject(size_t objectId) const {
    // (AR) الحصول على كائن بالمُعرّف (نسخة const)
    // (EN) Get object by ID (const version)
    
    auto it = objects_.find(objectId);
    if (it != objects_.end()) {
        return it->second.get();
    }
    
    return nullptr;
}

bool ObjectManager::hasObject(size_t objectId) const {
    // (AR) فحص وجود كائن
    // (EN) Check if object exists
    
    return objects_.find(objectId) != objects_.end();
}

size_t ObjectManager::getObjectCount() const {
    // (AR) عدد الكائنات الحية
    // (EN) Number of live objects
    
    return objects_.size();
}

std::vector<size_t> ObjectManager::getAllObjectIds() const {
    // (AR) قائمة بمُعرّفات جميع الكائنات
    // (EN) List of all object IDs
    
    std::vector<size_t> ids;
    ids.reserve(objects_.size());
    
    for (const auto& pair : objects_) {
        ids.push_back(pair.first);
    }
    
    return ids;
}

std::vector<ObjectInstance*> ObjectManager::getObjectsByClass(const std::string& className) {
    // (AR) الحصول على جميع كائنات صنف معين
    // (EN) Get all objects of specific class
    
    std::vector<ObjectInstance*> result;
    
    for (auto& pair : objects_) {
        ObjectInstance* obj = pair.second.get();
        if (obj->getClassName() == className) {
            result.push_back(obj);
        }
    }
    
    return result;
}

// ======================================================================
// جمع القمامة / Garbage Collection
// ======================================================================

void ObjectManager::markObject(size_t objectId) {
    // (AR) تمييز كائن كمستخدم
    // (EN) Mark object as used
    
    markedObjects_.insert(objectId);
}

void ObjectManager::unmarkObject(size_t objectId) {
    // (AR) إزالة تمييز كائن
    // (EN) Unmark object
    
    markedObjects_.erase(objectId);
}

bool ObjectManager::isMarked(size_t objectId) const {
    // (AR) فحص ما إذا كان كائن مُمَيّز
    // (EN) Check if object is marked
    
    return markedObjects_.find(objectId) != markedObjects_.end();
}

void ObjectManager::clearMarks() {
    // (AR) إزالة جميع التمييزات
    // (EN) Clear all marks
    
    markedObjects_.clear();
}

size_t ObjectManager::runGarbageCollection() {
    // (AR) تشغيل جمع القمامة
    // (EN) Run garbage collection
    
    size_t deletedCount = 0;
    
    // (AR) حذف الكائنات غير المُمَيّزة
    // (EN) Delete unmarked objects
    auto it = objects_.begin();
    while (it != objects_.end()) {
        size_t objId = it->first;
        
        if (markedObjects_.find(objId) == markedObjects_.end()) {
            // (AR) كائن غير مُمَيّز - حذفه
            // (EN) Unmarked object - delete it
            it = objects_.erase(it);
            deletedCount++;
        } else {
            ++it;
        }
    }
    
    // (AR) إزالة جميع التمييزات
    // (EN) Clear all marks
    markedObjects_.clear();
    
    return deletedCount;
}

void ObjectManager::enableGarbageCollection(bool enable) {
    // (AR) تفعيل/تعطيل جمع القمامة
    // (EN) Enable/disable garbage collection
    
    gcEnabled_ = enable;
}

bool ObjectManager::isGarbageCollectionEnabled() const {
    // (AR) فحص ما إذا كان جمع القمامة مفعّلاً
    // (EN) Check if garbage collection is enabled
    
    return gcEnabled_;
}

// ======================================================================
// الذاكرة / Memory Management
// ======================================================================

size_t ObjectManager::getTotalMemoryUsage() const {
    // (AR) حساب إجمالي استخدام الذاكرة
    // (EN) Calculate total memory usage
    
    size_t total = 0;
    
    for (const auto& pair : objects_) {
        total += pair.second->getMemoryUsage();
    }
    
    return total;
}

size_t ObjectManager::getObjectMemoryUsage(size_t objectId) const {
    // (AR) حساب استخدام ذاكرة كائن معين
    // (EN) Calculate memory usage of specific object
    
    auto it = objects_.find(objectId);
    if (it != objects_.end()) {
        return it->second->getMemoryUsage();
    }
    
    return 0;
}

std::string ObjectManager::getMemoryReport() const {
    // (AR) تقرير مفصّل عن الذاكرة
    // (EN) Detailed memory report
    
    std::ostringstream oss;
    oss << "\n";
    oss << "╔══════════════════════════════════════════════╗\n";
    oss << "║         MEMORY USAGE REPORT                  ║\n";
    oss << "╠══════════════════════════════════════════════╣\n";
    
    size_t totalMemory = getTotalMemoryUsage();
    oss << "║ Total Objects: " << objects_.size() << "\n";
    oss << "║ Total Memory: " << totalMemory << " bytes\n";
    oss << "║ Average Memory/Object: ";
    if (objects_.size() > 0) {
        oss << (totalMemory / objects_.size()) << " bytes\n";
    } else {
        oss << "N/A\n";
    }
    oss << "╠══════════════════════════════════════════════╣\n";
    
    // (AR) تجميع حسب الصنف
    // (EN) Group by class
    std::map<std::string, size_t> classCounts;
    std::map<std::string, size_t> classMemory;
    
    for (const auto& pair : objects_) {
        ObjectInstance* obj = pair.second.get();
        std::string className = obj->getClassName();
        
        classCounts[className]++;
        classMemory[className] += obj->getMemoryUsage();
    }
    
    oss << "║ By Class:\n";
    for (const auto& pair : classCounts) {
        oss << "║   " << pair.first << ": " << pair.second << " objects, ";
        oss << classMemory[pair.first] << " bytes\n";
    }
    
    oss << "╚══════════════════════════════════════════════╝\n";
    
    return oss.str();
}

// ======================================================================
// التصحيح والمعلومات / Debugging and Info
// ======================================================================

void ObjectManager::printDebugInfo() const {
    // (AR) طباعة معلومات جميع الكائنات
    // (EN) Print information about all objects
    
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════╗\n";
    std::cout << "║         OBJECT MANAGER DEBUG INFO            ║\n";
    std::cout << "╠══════════════════════════════════════════════╣\n";
    std::cout << "║ Total Objects: " << objects_.size() << "\n";
    std::cout << "║ GC Enabled: " << (gcEnabled_ ? "Yes" : "No") << "\n";
    std::cout << "║ Marked Objects: " << markedObjects_.size() << "\n";
    std::cout << "╠══════════════════════════════════════════════╣\n";
    
    for (const auto& pair : objects_) {
        std::cout << "║ Object #" << pair.first << ":\n";
        pair.second->printDebugInfo();
        std::cout << "║   Marked: " << (isMarked(pair.first) ? "Yes" : "No") << "\n";
    }
    
    std::cout << "╚══════════════════════════════════════════════╝\n";
}

void ObjectManager::printObjectInfo(size_t objectId) const {
    // (AR) طباعة معلومات كائن معين
    // (EN) Print information about specific object
    
    auto it = objects_.find(objectId);
    if (it == objects_.end()) {
        std::cerr << "خطأ: الكائن #" << objectId << " غير موجود\n";
        std::cerr << "Error: Object #" << objectId << " not found\n";
        return;
    }
    
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════╗\n";
    std::cout << "║         OBJECT #" << objectId << " INFO\n";
    std::cout << "╠══════════════════════════════════════════════╣\n";
    it->second->printDebugInfo();
    std::cout << "║ Marked: " << (isMarked(objectId) ? "Yes" : "No") << "\n";
    std::cout << "╚══════════════════════════════════════════════╝\n";
}

std::string ObjectManager::getStatistics() const {
    // (AR) الحصول على إحصائيات
    // (EN) Get statistics
    
    std::ostringstream oss;
    oss << "╔═══════════════════════════════════╗\n";
    oss << "║  ObjectManager Statistics         ║\n";
    oss << "╠═══════════════════════════════════╣\n";
    oss << "║ Total Objects: " << objects_.size() << "\n";
    oss << "║ Marked Objects: " << markedObjects_.size() << "\n";
    oss << "║ GC Enabled: " << (gcEnabled_ ? "Yes" : "No") << "\n";
    oss << "║ Total Memory: " << getTotalMemoryUsage() << " bytes\n";
    
    // (AR) تجميع حسب الصنف
    // (EN) Group by class
    std::map<std::string, size_t> classCounts;
    
    for (const auto& pair : objects_) {
        std::string className = pair.second->getClassName();
        classCounts[className]++;
    }
    
    oss << "║ By Class:\n";
    for (const auto& pair : classCounts) {
        oss << "║   " << pair.first << ": " << pair.second << "\n";
    }
    
    oss << "╚═══════════════════════════════════╝\n";
    
    return oss.str();
}

} // namespace Data
} // namespace Sad

// ======================================================================
// نهاية الملف / End of File
// ======================================================================
