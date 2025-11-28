// ======================================================================
// object_manager.h - مدير الكائنات / Object Manager
// ======================================================================
// الوصف بالعربية:
//   مدير مركزي لتخزين وإدارة جميع كائنات البرنامج في زمن التنفيذ
//   يوفر واجهة لإنشاء وحذف وتتبع الكائنات
//
// English Description:
//   Central manager for storing and managing all program objects at runtime
//   Provides interface for creating, deleting, and tracking objects
//
// ملاحظات / Notes:
//   - Singleton pattern لضمان مدير واحد فقط
//     Singleton pattern to ensure only one manager
//   - يدير ذاكرة الكائنات تلقائياً
//     Manages object memory automatically
//   - يوفر Garbage Collection بسيط
//     Provides simple Garbage Collection
//   - توثيق ثنائي اللغة (عربي/إنجليزي)
//     Bilingual documentation (Arabic/English)
// ======================================================================

#pragma once

#include "data/types/object_instance.h"
#include "data/types/class_type.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <unordered_set>

namespace Sad {
namespace Data {

// ======================================================================
// صنف ObjectManager الرئيسي / Main ObjectManager Class
// ======================================================================

/**
 * @class ObjectManager
 * @brief (AR) مدير مركزي لجميع كائنات البرنامج
 * @brief (EN) Central manager for all program objects
 * 
 * الوظائف الرئيسية:
 * - AR: إنشاء كائنات جديدة
 * - EN: Create new objects
 * - AR: تتبع جميع الكائنات الحية
 * - EN: Track all live objects
 * - AR: حذف كائنات غير مُستخدمة (Garbage Collection)
 * - EN: Delete unused objects (Garbage Collection)
 * - AR: البحث عن كائنات بالمعرف
 * - EN: Lookup objects by ID
 * 
 * مثال استخدام:
 * ```cpp
 * // الحصول على المدير
 * ObjectManager* manager = ObjectManager::getInstance();
 * 
 * // إنشاء كائن
 * ClassType* personClass = classManager->getClass("شخص");
 * ObjectInstance* ahmed = manager->createObject(personClass);
 * 
 * // تتبع الكائن
 * size_t id = ahmed->objectId;
 * ObjectInstance* found = manager->getObject(id);
 * 
 * // حذف الكائن
 * manager->deleteObject(id);
 * ```
 * 
 * ملاحظات إضافية:
 * - AR: يستخدم نمط Singleton لضمان مدير واحد فقط
 * - EN: Uses Singleton pattern to ensure single manager
 * - AR: يدير الذاكرة تلقائياً عبر unique_ptr
 * - EN: Manages memory automatically via unique_ptr
 * - AR: يوفر Garbage Collection بسيط (reference counting)
 * - EN: Provides simple Garbage Collection (reference counting)
 */
class ObjectManager {
public:
    // ──────────────────────────────────────────────────────────────────
    // Singleton Pattern / نمط المفرد
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) الحصول على النسخة الوحيدة من المدير
     * @brief (EN) Get the singleton instance of manager
     * 
     * @return (ObjectManager*) — (AR) مؤشر للمدير / (EN) pointer to manager
     */
    static ObjectManager* getInstance();
    
    /**
     * @brief (AR) إعادة تعيين المدير (للاختبارات)
     * @brief (EN) Reset manager (for testing)
     */
    static void resetInstance();
    
    // منع النسخ والإسناد / Prevent copying and assignment
    ObjectManager(const ObjectManager&) = delete;
    ObjectManager& operator=(const ObjectManager&) = delete;
    
    // ──────────────────────────────────────────────────────────────────
    // إنشاء الكائنات / Object Creation
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) إنشاء كائن جديد من صنف
     * @brief (EN) Create new object from class
     * 
     * @param classType (ClassType*) — (AR) الصنف / (EN) class
     * @return (ObjectInstance*) — (AR) مؤشر للكائن الجديد / (EN) pointer to new object
     * 
     * ملاحظات إضافية:
     * - AR: يُنشئ معرف فريد تلقائياً
     * - EN: Generates unique ID automatically
     * - AR: يُهيّأ جميع الخصائص بقيمها الافتراضية
     * - EN: Initializes all fields with default values
     * - AR: لا يستدعي الباني (يُستدعى لاحقاً من Interpreter)
     * - EN: Doesn't call constructor (called later by Interpreter)
     * 
     * مثال:
     * ```cpp
     * ClassType* personClass = classManager->getClass("شخص");
     * ObjectInstance* obj = objectManager->createObject(personClass);
     * 
     * // الآن استدعِ الباني من Interpreter
     * interpreter->callConstructor(obj, args);
     * ```
     */
    ObjectInstance* createObject(ClassType* classType);
    
    /**
     * @brief (AR) إنشاء كائن من صنف بالاسم
     * @brief (EN) Create object from class by name
     * 
     * @param className (std::string) — (AR) اسم الصنف / (EN) class name
     * @return (ObjectInstance*) — (AR) مؤشر للكائن أو nullptr / (EN) pointer to object or nullptr
     * 
     * ملاحظات إضافية:
     * - AR: يُرجع nullptr إذا لم يُوجد الصنف
     * - EN: Returns nullptr if class not found
     */
    ObjectInstance* createObject(const std::string& className);
    
    // ──────────────────────────────────────────────────────────────────
    // البحث والوصول / Lookup and Access
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) الحصول على كائن بالمعرف
     * @brief (EN) Get object by ID
     * 
     * @param objectId (size_t) — (AR) معرف الكائن / (EN) object ID
     * @return (ObjectInstance*) — (AR) مؤشر للكائن أو nullptr / (EN) pointer to object or nullptr
     */
    ObjectInstance* getObject(size_t objectId);
    
    /**
     * @brief (AR) الحصول على كائن بالمعرف (نسخة const)
     * @brief (EN) Get object by ID (const version)
     */
    const ObjectInstance* getObject(size_t objectId) const;
    
    /**
     * @brief (AR) هل الكائن موجود؟
     * @brief (EN) Does object exist?
     * 
     * @param objectId (size_t) — (AR) معرف الكائن / (EN) object ID
     * @return (bool) — (AR) true إذا وُجد / (EN) true if exists
     */
    bool hasObject(size_t objectId) const;
    
    /**
     * @brief (AR) الحصول على عدد الكائنات الحية
     * @brief (EN) Get number of live objects
     * 
     * @return (size_t) — (AR) عدد الكائنات / (EN) number of objects
     */
    size_t getObjectCount() const;
    
    /**
     * @brief (AR) الحصول على جميع كائنات صنف معين
     * @brief (EN) Get all objects of specific class
     * 
     * @param className (std::string) — (AR) اسم الصنف / (EN) class name
     * @return (std::vector<ObjectInstance*>) — (AR) قائمة الكائنات / (EN) list of objects
     */
    std::vector<ObjectInstance*> getObjectsOfClass(const std::string& className);
    
    /**
     * @brief (AR) الحصول على جميع كائنات صنف معين (طريقة بديلة)
     * @brief (EN) Get all objects of specific class (alternative method)
     */
    std::vector<ObjectInstance*> getObjectsByClass(const std::string& className);
    
    /**
     * @brief (AR) الحصول على قائمة بمعرفات جميع الكائنات
     * @brief (EN) Get list of all object IDs
     * 
     * @return (std::vector<size_t>) — (AR) قائمة المعرفات / (EN) list of IDs
     */
    std::vector<size_t> getAllObjectIds() const;
    
    // ──────────────────────────────────────────────────────────────────
    // الحذف / Deletion
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) حذف كائن
     * @brief (EN) Delete an object
     * 
     * @param objectId (size_t) — (AR) معرف الكائن / (EN) object ID
     * @return (bool) — (AR) true إذا نجح الحذف / (EN) true if deletion succeeded
     * 
     * ملاحظات إضافية:
     * - AR: يستدعي الهدام تلقائياً
     * - EN: Calls destructor automatically
     * - AR: يُرجع false إذا لم يُوجد الكائن
     * - EN: Returns false if object not found
     */
    bool deleteObject(size_t objectId);
    
    /**
     * @brief (AR) حذف جميع الكائنات
     * @brief (EN) Delete all objects
     * 
     * ملاحظات إضافية:
     * - AR: يستدعي الهدام لكل كائن
     * - EN: Calls destructor for each object
     */
    void clearAll();
    
    /**
     * @brief (AR) حذف جميع الكائنات (طريقة بديلة)
     * @brief (EN) Delete all objects (alternative method)
     */
    void deleteAllObjects();
    
    /**
     * @brief (AR) حذف جميع كائنات صنف معين
     * @brief (EN) Delete all objects of specific class
     * 
     * @param className (std::string) — (AR) اسم الصنف / (EN) class name
     * @return (size_t) — (AR) عدد الكائنات المحذوفة / (EN) number of deleted objects
     */
    size_t deleteObjectsOfClass(const std::string& className);
    
    // ──────────────────────────────────────────────────────────────────
    // Garbage Collection / جمع القمامة
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) تشغيل جامع القمامة
     * @brief (EN) Run garbage collector
     * 
     * @return (size_t) — (AR) عدد الكائنات المحذوفة / (EN) number of deleted objects
     * 
     * ملاحظات إضافية:
     * - AR: يحذف الكائنات غير المُشار إليها
     * - EN: Deletes unreferenced objects
     * - AR: يُستدعى تلقائياً عند الحاجة
     * - EN: Called automatically when needed
     * - AR: يمكن استدعاؤه يدوياً لتحسين الأداء
     * - EN: Can be called manually for performance
     */
    size_t runGarbageCollection();
    
    /**
     * @brief (AR) تمييز كائن كمُستخدم (للـ GC)
     * @brief (EN) Mark object as used (for GC)
     * 
     * @param objectId (size_t) — (AR) معرف الكائن / (EN) object ID
     */
    void markObjectUsed(size_t objectId);
    
    /**
     * @brief (AR) تمييز كائن (طريقة بديلة)
     * @brief (EN) Mark object (alternative method)
     */
    void markObject(size_t objectId);
    
    /**
     * @brief (AR) إزالة التمييز عن كائن
     * @brief (EN) Unmark object
     */
    void unmarkObject(size_t objectId);
    
    /**
     * @brief (AR) هل الكائن مُميّز؟
     * @brief (EN) Is object marked?
     */
    bool isMarked(size_t objectId) const;
    
    /**
     * @brief (AR) مسح جميع التمييزات
     * @brief (EN) Clear all marks
     */
    void clearMarks();
    
    /**
     * @brief (AR) هل الكائن مُستخدم؟
     * @brief (EN) Is object marked as used?
     * 
     * @param objectId (size_t) — (AR) معرف الكائن / (EN) object ID
     * @return (bool) — (AR) true إذا مُستخدم / (EN) true if used
     */
    bool isObjectMarked(size_t objectId) const;
    
    // ──────────────────────────────────────────────────────────────────
    // إحصائيات الذاكرة / Memory Statistics
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) الحصول على حجم الذاكرة المُستخدمة تقريبياً
     * @brief (EN) Get approximate memory usage
     * 
     * @return (size_t) — (AR) الحجم بالبايتات / (EN) size in bytes
     */
    size_t getTotalMemoryUsage() const;
    
    /**
     * @brief (AR) الحصول على حجم ذاكرة صنف معين
     * @brief (EN) Get memory usage of specific class
     * 
     * @param className (std::string) — (AR) اسم الصنف / (EN) class name
     * @return (size_t) — (AR) الحجم بالبايتات / (EN) size in bytes
     */
    size_t getClassMemoryUsage(const std::string& className) const;
    
    /**
     * @brief (AR) الحصول على حجم ذاكرة كائن معين
     * @brief (EN) Get memory usage of specific object
     */
    size_t getObjectMemoryUsage(size_t objectId) const;
    
    /**
     * @brief (AR) الحصول على تقرير الذاكرة
     * @brief (EN) Get memory report
     */
    std::string getMemoryReport() const;
    
    /**
     * @brief (AR) الحصول على متوسط حجم الكائن
     * @brief (EN) Get average object size
     * 
     * @return (size_t) — (AR) الحجم بالبايتات / (EN) size in bytes
     */
    size_t getAverageObjectSize() const;
    
    /**
     * @brief (AR) تفعيل/تعطيل جمع القمامة
     * @brief (EN) Enable/disable garbage collection
     */
    void enableGarbageCollection(bool enable);
    
    /**
     * @brief (AR) هل جمع القمامة مُفعّل؟
     * @brief (EN) Is garbage collection enabled?
     */
    bool isGarbageCollectionEnabled() const;
    
    // ──────────────────────────────────────────────────────────────────
    // التصحيح والمعلومات / Debugging and Info
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) طباعة معلومات جميع الكائنات
     * @brief (EN) Print information about all objects
     */
    void printDebugInfo() const;
    
    /**
     * @brief (AR) طباعة معلومات كائن معين
     * @brief (EN) Print information about specific object
     * 
     * @param objectId (size_t) — (AR) معرف الكائن / (EN) object ID
     */
    void printObjectInfo(size_t objectId) const;
    
    /**
     * @brief (AR) الحصول على إحصائيات
     * @brief (EN) Get statistics
     * 
     * @return (std::string) — (AR) معلومات إحصائية / (EN) statistical information
     */
    std::string getStatistics() const;
    
    /**
     * @brief (AR) طباعة تقرير الذاكرة
     * @brief (EN) Print memory report
     */
    void printMemoryReport() const;

private:
    // ──────────────────────────────────────────────────────────────────
    // البيانات الخاصة / Private Data
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) خريطة الكائنات الحية
     * @brief (EN) Map of live objects
     * 
     * ملاحظات إضافية:
     * - AR: المفتاح: معرف الكائن
     * - EN: Key: object ID
     * - AR: القيمة: مؤشر ذكي للكائن
     * - EN: Value: smart pointer to object
     */
    std::unordered_map<size_t, std::unique_ptr<ObjectInstance>> objects_;
    
    /**
     * @brief (AR) مجموعة الكائنات المُميّزة للـ GC
     * @brief (EN) Set of marked objects for GC
     */
    std::unordered_set<size_t> markedObjects_;
    
    /**
     * @brief (AR) علامات GC (true = مُستخدم)
     * @brief (EN) GC marks (true = used)
     */
    std::unordered_map<size_t, bool> gcMarks_;
    
    /**
     * @brief (AR) هل جمع القمامة مُفعّل؟
     * @brief (EN) Is garbage collection enabled?
     */
    bool gcEnabled_;
    
    /**
     * @brief (AR) عداد لتوليد معرفات فريدة
     * @brief (EN) Counter for generating unique IDs
     */
    size_t nextObjectId_;
    
    /**
     * @brief (AR) المدير المفرد الوحيد
     * @brief (EN) Singleton instance
     */
    static ObjectManager* instance_;
    
    // ──────────────────────────────────────────────────────────────────
    // المنشئات الخاصة / Private Constructors
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) منشئ خاص (Singleton)
     * @brief (EN) Private constructor (Singleton)
     */
    ObjectManager();
    
    /**
     * @brief (AR) هدام
     * @brief (EN) Destructor
     */
    ~ObjectManager() = default;
    
    // ──────────────────────────────────────────────────────────────────
    // دوال مساعدة خاصة / Private Helper Functions
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) توليد معرف فريد جديد
     * @brief (EN) Generate new unique ID
     */
    size_t generateObjectId();
};

} // namespace Data
} // namespace Sad

// ======================================================================
// نهاية الملف / End of File
// ======================================================================
