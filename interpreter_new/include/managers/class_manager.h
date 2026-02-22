// ======================================================================
// class_manager.h - مدير الأصناف / Class Manager
// ======================================================================
// الوصف بالعربية:
//   مدير مركزي لتخزين وإدارة جميع تعريفات الأصناف في البرنامج
//   يوفر واجهة للبحث والتسجيل والحذف
//
// English Description:
//   Central manager for storing and managing all class definitions in program
//   Provides interface for lookup, registration, and removal
//
// ملاحظات / Notes:
//   - Singleton pattern لضمان مدير واحد فقط
//     Singleton pattern to ensure only one manager
//   - يُستخدم من Parser عند تحليل تعريف صنف
//     Used by Parser when parsing class declaration
//   - يُستخدم من Interpreter عند إنشاء كائنات
//     Used by Interpreter when creating objects
//   - توثيق ثنائي اللغة (عربي/إنجليزي)
//     Bilingual documentation (Arabic/English)
// ======================================================================

#pragma once

#include "class_type.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <vector>
#include <functional>

namespace Sad {
namespace Data {

// ======================================================================
// بنية تعريف الواجهة/السمة / Trait Definition Structure
// ======================================================================

/**
 * @brief (AR) تمثيل دالة مطلوبة في واجهة
 * @brief (EN) Required method in a trait/interface
 */
struct TraitMethodInfo {
    std::string name;                           ///< (AR) اسم الدالة / (EN) method name
    std::vector<DataType> paramTypes;           ///< (AR) أنواع المعاملات / (EN) parameter types
    DataType returnType;                        ///< (AR) نوع الإرجاع / (EN) return type
    bool hasDefaultImpl;                        ///< (AR) هل لها تنفيذ افتراضي / (EN) has default implementation
    
    TraitMethodInfo() : returnType(DataType::NONE), hasDefaultImpl(false) {}
    TraitMethodInfo(const std::string& n, DataType ret, bool hasDef = false)
        : name(n), returnType(ret), hasDefaultImpl(hasDef) {}
    TraitMethodInfo(TraitMethodInfo&&) = default;
    TraitMethodInfo& operator=(TraitMethodInfo&&) = default;
    TraitMethodInfo(const TraitMethodInfo&) = default;
    TraitMethodInfo& operator=(const TraitMethodInfo&) = default;
};

/**
 * @brief (AR) تعريف واجهة/سمة
 * @brief (EN) Trait/Interface definition
 */
struct TraitDefinition {
    std::string name;                               ///< (AR) اسم السمة / (EN) trait name
    std::vector<TraitMethodInfo> requiredMethods;    ///< (AR) الدوال المطلوبة / (EN) required methods
    std::vector<std::string> superTraits;            ///< (AR) السمات الأساسية / (EN) parent traits
    
    TraitDefinition() = default;
    explicit TraitDefinition(const std::string& n) : name(n) {}
};

// ======================================================================
// صنف ClassManager الرئيسي / Main ClassManager Class
// ======================================================================

/**
 * @class ClassManager
 * @brief (AR) مدير مركزي لجميع الأصناف في البرنامج
 * @brief (EN) Central manager for all classes in program
 * 
 * الوظائف الرئيسية:
 * - AR: تسجيل أصناف جديدة
 * - EN: Register new classes
 * - AR: البحث عن أصناف بالاسم
 * - EN: Lookup classes by name
 * - AR: التحقق من وجود صنف
 * - EN: Check class existence
 * - AR: إدارة دورة حياة الأصناف
 * - EN: Manage class lifecycle
 * 
 * مثال استخدام:
 * ```cpp
 * // الحصول على المدير
 * ClassManager* manager = ClassManager::getInstance();
 * 
 * // تسجيل صنف جديد
 * ClassType* personClass = new ClassType("شخص");
 * manager->registerClass(personClass);
 * 
 * // البحث عن صنف
 * ClassType* found = manager->getClass("شخص");
 * if (found) {
 *     ObjectInstance* obj = found->createInstance();
 * }
 * ```
 * 
 * ملاحظات إضافية:
 * - AR: يستخدم نمط Singleton لضمان مدير واحد فقط
 * - EN: Uses Singleton pattern to ensure single manager
 * - AR: يدير الذاكرة تلقائياً عبر unique_ptr
 * - EN: Manages memory automatically via unique_ptr
 * - AR: آمن للاستخدام من أجزاء مختلفة من المترجم
 * - EN: Safe to use from different parts of interpreter
 */
class ClassManager {
public:
    // ──────────────────────────────────────────────────────────────────
    // Singleton Pattern / نمط المفرد
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) الحصول على النسخة الوحيدة من المدير
     * @brief (EN) Get the singleton instance of manager
     * 
     * @return (ClassManager*) — (AR) مؤشر للمدير / (EN) pointer to manager
     * 
     * ملاحظات إضافية:
     * - AR: يُنشأ المدير تلقائياً في أول استدعاء
     * - EN: Manager is created automatically on first call
     * - AR: آمن للاستخدام من threads متعددة (thread-safe)
     * - EN: Thread-safe for multi-threaded access
     */
    static ClassManager* getInstance();
    
    /**
     * @brief (AR) إعادة تعيين المدير (للاختبارات)
     * @brief (EN) Reset manager (for testing)
     * 
     * ملاحظات إضافية:
     * - AR: يحذف جميع الأصناف المسجلة
     * - EN: Deletes all registered classes
     * - AR: يُستخدم في unit tests فقط
     * - EN: Used in unit tests only
     */
    static void resetInstance();
    
    // منع النسخ والإسناد / Prevent copying and assignment
    ClassManager(const ClassManager&) = delete;
    ClassManager& operator=(const ClassManager&) = delete;
    
    // ──────────────────────────────────────────────────────────────────
    // تسجيل الأصناف / Class Registration
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) تسجيل صنف جديد
     * @brief (EN) Register a new class
     * 
     * @param classType (std::unique_ptr<ClassType>) — (AR) الصنف المراد تسجيله / (EN) class to register
     * @return (bool) — (AR) true إذا نجح التسجيل / (EN) true if registration succeeded
     * 
     * ملاحظات إضافية:
     * - AR: يُرجع false إذا كان الاسم مُستخدم مسبقاً
     * - EN: Returns false if name is already used
     * - AR: ينقل ملكية الكائن للمدير (move semantics)
     * - EN: Transfers ownership to manager (move semantics)
     * 
     * مثال:
     * ```cpp
     * auto personClass = std::make_unique<ClassType>("شخص");
     * personClass->addField("الاسم", stringType, PUBLIC);
     * 
     * if (!manager->registerClass(std::move(personClass))) {
     *     std::cerr << "فشل التسجيل - الاسم موجود مسبقاً" << std::endl;
     * }
     * ```
     */
    bool registerClass(std::unique_ptr<ClassType> classType);
    
    /**
     * @brief (AR) تسجيل صنف بمؤشر عادي (للتوافق القديم)
     * @brief (EN) Register class with raw pointer (legacy compatibility)
     * 
     * @param classType (ClassType*) — (AR) الصنف / (EN) class
     * @return (bool) — (AR) true إذا نجح / (EN) true if succeeded
     * 
     * ملاحظات إضافية:
     * - AR: ينقل الملكية للمدير
     * - EN: Transfers ownership to manager
     */
    bool registerClass(ClassType* classType);
    
    // ──────────────────────────────────────────────────────────────────
    // البحث والوصول / Lookup and Access
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) الحصول على صنف بالاسم
     * @brief (EN) Get class by name
     * 
     * @param className (std::string) — (AR) اسم الصنف / (EN) class name
     * @return (ClassType*) — (AR) مؤشر للصنف أو nullptr / (EN) pointer to class or nullptr
     * 
     * ملاحظات إضافية:
     * - AR: يُرجع nullptr إذا لم يُوجد الصنف
     * - EN: Returns nullptr if class not found
     * - AR: المدير يحتفظ بالملكية (لا تحذف المؤشر)
     * - EN: Manager retains ownership (don't delete pointer)
     * 
     * مثال:
     * ```cpp
     * ClassType* person = manager->getClass("شخص");
     * if (!person) {
     *     throw std::runtime_error("الصنف 'شخص' غير موجود");
     * }
     * ```
     */
    ClassType* getClass(const std::string& className);
    
    /**
     * @brief (AR) الحصول على صنف بالاسم (نسخة const)
     * @brief (EN) Get class by name (const version)
     */
    const ClassType* getClass(const std::string& className) const;
    
    /**
     * @brief (AR) هل الصنف موجود؟
     * @brief (EN) Does class exist?
     * 
     * @param className (std::string) — (AR) اسم الصنف / (EN) class name
     * @return (bool) — (AR) true إذا وُجد / (EN) true if exists
     */
    bool hasClass(const std::string& className) const;
    
    /**
     * @brief (AR) الحصول على عدد الأصناف المسجلة
     * @brief (EN) Get number of registered classes
     * 
     * @return (size_t) — (AR) عدد الأصناف / (EN) number of classes
     */
    size_t getClassCount() const;
    
    /**
     * @brief (AR) الحصول على قائمة بأسماء جميع الأصناف
     * @brief (EN) Get list of all class names
     * 
     * @return (std::vector<std::string>) — (AR) قائمة الأسماء / (EN) list of names
     */
    std::vector<std::string> getAllClassNames() const;
    
    // ──────────────────────────────────────────────────────────────────
    // الحذف / Removal
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) حذف صنف
     * @brief (EN) Remove a class
     * 
     * @param className (std::string) — (AR) اسم الصنف / (EN) class name
     * @return (bool) — (AR) true إذا نجح الحذف / (EN) true if removal succeeded
     * 
     * ملاحظات إضافية:
     * - AR: يُرجع false إذا لم يُوجد الصنف
     * - EN: Returns false if class not found
     * - AR: احذر: قد يؤدي لمشاكل إذا كانت هناك كائنات من الصنف
     * - EN: Warning: may cause issues if objects of this class exist
     */
    bool removeClass(const std::string& className);
    
    /**
     * @brief (AR) حذف جميع الأصناف
     * @brief (EN) Remove all classes
     * 
     * ملاحظات إضافية:
     * - AR: يُستخدم عند إنهاء البرنامج أو إعادة التهيئة
     * - EN: Used when program ends or reinitialization
     */
    void clearAll();
    
    // ──────────────────────────────────────────────────────────────────
    // التحقق من الوراثة / Inheritance Checking
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) التحقق من صحة سلسلة الوراثة
     * @brief (EN) Validate inheritance chain
     * 
     * @param className (std::string) — (AR) اسم الصنف / (EN) class name
     * @return (bool) — (AR) true إذا كانت السلسلة صحيحة / (EN) true if chain is valid
     * 
     * ملاحظات إضافية:
     * - AR: يفحص عدم وجود دوائر في الوراثة
     * - EN: Checks for circular inheritance
     * - AR: يفحص وجود جميع الأصناف الأساسية
     * - EN: Checks existence of all base classes
     */
    bool validateInheritanceChain(const std::string& className) const;
    
    /**
     * @brief (AR) الحصول على سلسلة الوراثة الكاملة
     * @brief (EN) Get complete inheritance chain
     * 
     * @param className (std::string) — (AR) اسم الصنف / (EN) class name
     * @return (std::vector<std::string>) — (AR) قائمة الأسماء من الأعلى للأسفل / (EN) list of names top to bottom
     * 
     * مثال:
     * ```cpp
     * // إذا كان: كلب يرث حيوان يرث كائن
     * auto chain = manager->getInheritanceChain("كلب");
     * // النتيجة: ["كائن", "حيوان", "كلب"]
     * ```
     */
    std::vector<std::string> getInheritanceChain(const std::string& className) const;
    
    // ──────────────────────────────────────────────────────────────────
    // التصحيح والمعلومات / Debugging and Info
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) طباعة معلومات جميع الأصناف
     * @brief (EN) Print information about all classes
     */
    void printDebugInfo() const;
    
    /**
     * @brief (AR) طباعة معلومات صنف معين
     * @brief (EN) Print information about specific class
     * 
     * @param className (std::string) — (AR) اسم الصنف / (EN) class name
     */
    void printClassInfo(const std::string& className) const;
    
    /**
     * @brief (AR) الحصول على إحصائيات
     * @brief (EN) Get statistics
     * 
     * @return (std::string) — (AR) معلومات إحصائية / (EN) statistical information
     */
    std::string getStatistics() const;

    // ──────────────────────────────────────────────────────────────────
    // الواجهات والسمات / Traits and Interfaces
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) تسجيل واجهة/سمة جديدة
     * @brief (EN) Register a new trait/interface
     */
    bool registerTrait(TraitDefinition trait);
    
    /**
     * @brief (AR) الحصول على تعريف واجهة
     * @brief (EN) Get trait definition
     */
    const TraitDefinition* getTrait(const std::string& traitName) const;
    
    /**
     * @brief (AR) هل الواجهة موجودة؟
     * @brief (EN) Does trait exist?
     */
    bool hasTrait(const std::string& traitName) const;
    
    /**
     * @brief (AR) التحقق من أن الصنف ينفذ جميع دوال الواجهة
     * @brief (EN) Verify class implements all trait methods
     */
    bool validateTraitImpl(const std::string& className, const std::string& traitName) const;
    
    /**
     * @brief (AR) تسجيل أن صنفاً ينفذ واجهة
     * @brief (EN) Register that a class implements a trait
     */
    bool registerTraitImpl(const std::string& className, const std::string& traitName);
    
    /**
     * @brief (AR) هل الصنف ينفذ واجهة معينة؟
     * @brief (EN) Does class implement a specific trait?
     */
    bool classImplementsTrait(const std::string& className, const std::string& traitName) const;

private:
    // ──────────────────────────────────────────────────────────────────
    // البيانات الخاصة / Private Data
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) خريطة الأصناف المسجلة
     * @brief (EN) Map of registered classes
     * 
     * ملاحظات إضافية:
     * - AR: المفتاح: اسم الصنف
     * - EN: Key: class name
     * - AR: القيمة: مؤشر ذكي للصنف
     * - EN: Value: smart pointer to class
     */
    std::unordered_map<std::string, std::unique_ptr<ClassType>> classes_;
    
    /**
     * @brief (AR) خريطة الواجهات المسجلة
     * @brief (EN) Map of registered traits
     */
    std::unordered_map<std::string, TraitDefinition> traits_;
    
    /**
     * @brief (AR) المدير المفرد الوحيد
     * @brief (EN) Singleton instance
     */
    static ClassManager* instance_;
    
    // ──────────────────────────────────────────────────────────────────
    // المنشئات الخاصة / Private Constructors
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) منشئ خاص (Singleton)
     * @brief (EN) Private constructor (Singleton)
     */
    ClassManager() = default;
    
    /**
     * @brief (AR) هدام
     * @brief (EN) Destructor
     */
    ~ClassManager() = default;
    
    // ──────────────────────────────────────────────────────────────────
    // دوال مساعدة خاصة / Private Helper Functions
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) فحص وجود دوائر في الوراثة
     * @brief (EN) Check for circular inheritance
     */
    bool hasCircularInheritance(const std::string& className, 
                                 std::unordered_set<std::string>& visited) const;
};

} // namespace Data
} // namespace Sad

// ======================================================================
// نهاية الملف / End of File
// ======================================================================
