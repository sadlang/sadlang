// ======================================================================
// object_instance.h - كائن نسخة من صنف / Object Instance
// ======================================================================
// الوصف بالعربية:
//   تعريف بنية بيانات لتمثيل كائن (نسخة من صنف) في زمن التنفيذ
//   يحتوي على قيم الخصائص والمرجع للصنف الأصلي
//
// English Description:
//   Definition of object instance data structure for runtime objects
//   Contains field values and reference to original class
//
// ملاحظات / Notes:
//   - يُنشأ من ClassType عبر createInstance()
//     Created from ClassType via createInstance()
//   - يُخزّن في ObjectManager لإدارة ذاكرة الكائنات
//     Stored in ObjectManager for object memory management
//   - يوفر واجهة للوصول وتعديل الخصائص
//     Provides interface for field access and modification
//   - توثيق ثنائي اللغة (عربي/إنجليزي)
//     Bilingual documentation (Arabic/English)
// ======================================================================

#pragma once

#include "class_type.h"
#include "value.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace Sad {
namespace Data {

// ======================================================================
// صنف ObjectInstance الرئيسي / Main ObjectInstance Class
// ======================================================================

/**
 * @class ObjectInstance
 * @brief (AR) كائن نسخة من صنف في زمن التنفيذ
 * @brief (EN) Runtime instance of a class
 * 
 * الوظائف الرئيسية:
 * - AR: تخزين قيم خصائص الكائن
 * - EN: Store object field values
 * - AR: توفير واجهة للوصول وتعديل الخصائص
 * - EN: Provide interface for field access/modification
 * - AR: الاحتفاظ بمرجع للصنف الأصلي
 * - EN: Keep reference to original class
 * - AR: دعم استدعاء الطرق
 * - EN: Support method invocation
 * 
 * مثال استخدام:
 * ```
 * // إنشاء صنف
 * ClassType* personClass = new ClassType("شخص");
 * personClass->addField("الاسم", stringType, PUBLIC);
 * personClass->addField("العمر", intType, PUBLIC);
 * 
 * // إنشاء كائن
 * ObjectInstance* ahmed = personClass->createInstance();
 * ahmed->setField("الاسم", Value("أحمد"));
 * ahmed->setField("العمر", Value(25));
 * 
 * // قراءة قيمة
 * Value name = ahmed->getField("الاسم");
 * std::cout << name.toString(); // أحمد
 * ```
 * 
 * ملاحظات إضافية:
 * - AR: كل كائن له معرف فريد (objectId)
 * - EN: Each object has unique identifier (objectId)
 * - AR: يمكن أن يكون له كائن أساسي (baseInstance) للوراثة
 * - EN: Can have base object instance (baseInstance) for inheritance
 */
class ObjectInstance {
public:
    // ──────────────────────────────────────────────────────────────────
    // البيانات الأساسية / Basic Data
    // ──────────────────────────────────────────────────────────────────
    
    ClassType* classType;                               ///< (AR) الصنف الأصلي / (EN) original class
    size_t objectId;                                    ///< (AR) معرف فريد / (EN) unique identifier
    std::unique_ptr<ObjectInstance> baseInstance;       ///< (AR) كائن الصنف الأساسي / (EN) base class instance
    
    // ──────────────────────────────────────────────────────────────────
    // خريطة الخصائص / Field Values Map
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) خريطة قيم الخصائص
     * @brief (EN) Field values map
     * 
     * ملاحظات إضافية:
     * - AR: المفتاح: اسم الخاصية
     * - EN: Key: field name
     * - AR: القيمة: Value تحتوي على البيانات
     * - EN: Value: Value containing data
     * - AR: لا تشمل الخصائص الثابتة (موجودة في ClassType)
     * - EN: Doesn't include static fields (stored in ClassType)
     */
    std::unordered_map<std::string, Value> fields;
    
    // ──────────────────────────────────────────────────────────────────
    // معلومات إضافية / Additional Info
    // ──────────────────────────────────────────────────────────────────
    
    bool isConstructed;                                 ///< (AR) هل اُستدعي الباني؟ / (EN) was constructor called?
    
    // ──────────────────────────────────────────────────────────────────
    // المنشئات / Constructors
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) منشئ مع صنف ومعرف
     * @brief (EN) Constructor with class and ID
     * 
     * @param cls (ClassType*) — (AR) الصنف الأصلي / (EN) original class
     * @param id (size_t) — (AR) معرف فريد / (EN) unique ID
     */
    ObjectInstance(ClassType* cls, size_t id);
    
    /**
     * @brief (AR) هدام - يستدعي الهدام المخصص إن وُجد
     * @brief (EN) Destructor - calls custom destructor if exists
     */
    ~ObjectInstance();
    
    // منع النسخ لتجنب مشاكل الذاكرة
    // Prevent copying to avoid memory issues
    ObjectInstance(const ObjectInstance&) = delete;
    ObjectInstance& operator=(const ObjectInstance&) = delete;
    
    // ──────────────────────────────────────────────────────────────────
    // الوصول للخصائص / Field Access
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) الحصول على قيمة خاصية
     * @brief (EN) Get field value
     * 
     * @param fieldName (std::string) — (AR) اسم الخاصية / (EN) field name
     * @return (Value*) — (AR) مؤشر للقيمة أو nullptr / (EN) pointer to value or nullptr
     * 
     * ملاحظات إضافية:
     * - AR: يبحث في الكائن الحالي ثم الكائن الأساسي
     * - EN: Searches in current object then base object
     * - AR: يُرجع nullptr إذا لم تُوجد الخاصية
     * - EN: Returns nullptr if field not found
     * 
     * مثال:
     * ```
     * Value* name = ahmed->getField("الاسم");
     * if (name) {
     *     std::cout << name->toString();
     * }
     * ```
     */
    Value* getField(const std::string& fieldName);
    
    /**
     * @brief (AR) تعيين قيمة خاصية
     * @brief (EN) Set field value
     * 
     * @param fieldName (std::string) — (AR) اسم الخاصية / (EN) field name
     * @param value (const Value&) — (AR) القيمة الجديدة / (EN) new value
     * @return (bool) — (AR) true إذا نجحت العملية / (EN) true if successful
     * 
     * ملاحظات إضافية:
     * - AR: يفحص وجود الخاصية في الصنف
     * - EN: Checks if field exists in class
     * - AR: يفحص توافق النوع
     * - EN: Checks type compatibility
     * - AR: يُرجع false إذا فشلت العملية
     * - EN: Returns false if operation fails
     * 
     * مثال:
     * ```
     * bool success = ahmed->setField("العمر", Value(26));
     * if (!success) {
     *     std::cerr << "فشل تعيين الخاصية" << std::endl;
     * }
     * ```
     */
    bool setField(const std::string& fieldName, const Value& value);
    
    /**
     * @brief (AR) هل الخاصية موجودة؟
     * @brief (EN) Does field exist?
     * 
     * @param fieldName (std::string) — (AR) اسم الخاصية / (EN) field name
     * @return (bool) — (AR) true إذا وُجدت / (EN) true if exists
     */
    bool hasField(const std::string& fieldName) const;
    
    /**
     * @brief (AR) الحصول على جميع أسماء الخصائص
     * @brief (EN) Get all field names
     * 
     * @return (std::vector<std::string>) — (AR) قائمة الأسماء / (EN) list of names
     */
    std::vector<std::string> getFieldNames() const;
    
    // ──────────────────────────────────────────────────────────────────
    // استدعاء الطرق / Method Invocation
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) هل الطريقة موجودة؟
     * @brief (EN) Does method exist?
     * 
     * @param methodName (std::string) — (AR) اسم الطريقة / (EN) method name
     * @return (bool) — (AR) true إذا وُجدت / (EN) true if exists
     */
    bool hasMethod(const std::string& methodName) const;
    
    /**
     * @brief (AR) الحصول على طريقة
     * @brief (EN) Get method
     * 
     * @param methodName (std::string) — (AR) اسم الطريقة / (EN) method name
     * @return (ClassMethod*) — (AR) مؤشر للطريقة أو nullptr / (EN) pointer to method or nullptr
     * 
     * ملاحظات إضافية:
     * - AR: يبحث في الصنف الحالي ثم الأصناف الأساسية
     * - EN: Searches in current class then base classes
     */
    ClassMethod* getMethod(const std::string& methodName);
    
    // ──────────────────────────────────────────────────────────────────
    // معلومات الصنف / Class Information
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) الحصول على اسم الصنف
     * @brief (EN) Get class name
     * 
     * @return (std::string) — (AR) اسم الصنف / (EN) class name
     */
    std::string getClassName() const { return classType->name; }
    
    /**
     * @brief (AR) الحصول على الصنف الأصلي
     * @brief (EN) Get original class
     * 
     * @return (ClassType*) — (AR) مؤشر للصنف / (EN) pointer to class
     */
    ClassType* getClass() const { return classType; }
    
    /**
     * @brief (AR) هل الكائن من نوع صنف معين؟
     * @brief (EN) Is object instance of a specific class?
     * 
     * @param className (std::string) — (AR) اسم الصنف / (EN) class name
     * @return (bool) — (AR) true إذا كان من هذا النوع / (EN) true if instance of this class
     * 
     * ملاحظات إضافية:
     * - AR: يفحص الصنف الحالي والأصناف الأساسية
     * - EN: Checks current class and base classes
     */
    bool isInstanceOf(const std::string& className) const;
    
    /**
     * @brief (AR) هل الكائن من نوع صنف معين؟
     * @brief (EN) Is object instance of a specific class?
     * 
     * @param cls (const ClassType*) — (AR) الصنف / (EN) class
     * @return (bool) — (AR) true إذا كان من هذا النوع / (EN) true if instance of this class
     */
    bool isInstanceOf(const ClassType* cls) const;
    
    // ──────────────────────────────────────────────────────────────────
    // الباني والهدام / Constructor and Destructor
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) وضع علامة أن الباني اُستدعي
     * @brief (EN) Mark that constructor was called
     */
    void markConstructed() { isConstructed = true; }
    
    /**
     * @brief (AR) هل اُستدعي الباني؟
     * @brief (EN) Was constructor called?
     * 
     * @return (bool) — (AR) true إذا اُستدعي / (EN) true if called
     */
    bool wasConstructed() const { return isConstructed; }
    
    // ──────────────────────────────────────────────────────────────────
    // الوراثة / Inheritance
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) الحصول على كائن الصنف الأساسي
     * @brief (EN) Get base class instance
     * 
     * @return (ObjectInstance*) — (AR) مؤشر للكائن الأساسي أو nullptr / (EN) pointer to base instance or nullptr
     */
    ObjectInstance* getBaseInstance() const { return baseInstance.get(); }
    
    /**
     * @brief (AR) تعيين كائن الصنف الأساسي
     * @brief (EN) Set base class instance
     * 
     * @param base (std::unique_ptr<ObjectInstance>) — (AR) الكائن الأساسي / (EN) base instance
     */
    void setBaseInstance(std::unique_ptr<ObjectInstance> base);
    
    // ──────────────────────────────────────────────────────────────────
    // تهيئة الخصائص / Field Initialization
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) تهيئة جميع الخصائص بقيمها الافتراضية
     * @brief (EN) Initialize all fields with default values
     * 
     * ملاحظات إضافية:
     * - AR: تُستدعى تلقائياً عند إنشاء الكائن
     * - EN: Called automatically when object is created
     * - AR: تُهيّأ خصائص الصنف الحالي والأصناف الأساسية
     * - EN: Initializes fields of current class and base classes
     */
    void initializeFields();
    
    // ──────────────────────────────────────────────────────────────────
    // التمثيل النصي / String Representation
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) تحويل الكائن إلى نص
     * @brief (EN) Convert object to string
     * 
     * @return (std::string) — (AR) تمثيل نصي / (EN) string representation
     * 
     * مثال:
     * "<شخص#123: الاسم='أحمد', العمر=25>"
     */
    std::string toString() const;
    
    /**
     * @brief (AR) طباعة معلومات الكائن للتصحيح
     * @brief (EN) Print object information for debugging
     */
    void printDebugInfo() const;
    
    // ──────────────────────────────────────────────────────────────────
    // معلومات الذاكرة / Memory Information
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) الحصول على حجم الذاكرة المستخدمة تقريبياً
     * @brief (EN) Get approximate memory usage
     * 
     * @return (size_t) — (AR) الحجم بالبايتات / (EN) size in bytes
     */
    size_t getMemoryUsage() const;
    
private:
    // ──────────────────────────────────────────────────────────────────
    // دوال مساعدة خاصة / Private Helper Functions
    // ──────────────────────────────────────────────────────────────────
    
    /**
     * @brief (AR) البحث عن خاصية في السلسلة الكاملة
     * @brief (EN) Find field in full inheritance chain
     */
    Value* findFieldInChain(const std::string& fieldName);
    
    /**
     * @brief (AR) فحص رؤية خاصية
     * @brief (EN) Check field visibility
     */
    bool checkFieldAccess(const std::string& fieldName) const;
};

// ======================================================================
// دوال مساعدة عامة / Global Helper Functions
// ======================================================================

/**
 * @brief (AR) إنشاء معرف فريد للكائن
 * @brief (EN) Generate unique object ID
 * 
 * @return (size_t) — (AR) معرف فريد / (EN) unique ID
 */
size_t generateObjectId();

/**
 * @brief (AR) مقارنة كائنين
 * @brief (EN) Compare two objects
 * 
 * @param obj1 (const ObjectInstance*) — (AR) الكائن الأول / (EN) first object
 * @param obj2 (const ObjectInstance*) — (AR) الكائن الثاني / (EN) second object
 * @return (bool) — (AR) true إذا كانا متطابقين / (EN) true if equal
 * 
 * ملاحظات إضافية:
 * - AR: يقارن القيم وليس المراجع
 * - EN: Compares values not references
 */
bool compareObjects(const ObjectInstance* obj1, const ObjectInstance* obj2);

} // namespace Data
} // namespace Sad

// ======================================================================
// نهاية الملف / End of File
// ======================================================================
