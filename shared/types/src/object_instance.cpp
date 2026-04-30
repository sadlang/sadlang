// ======================================================================
// object_instance.cpp - تنفيذ كائن نسخة من صنف / Object Instance Implementation
// ======================================================================
// الوصف بالعربية:
//   تنفيذ جميع دوال ObjectInstance المُعرّفة في object_instance.h
//   يشمل: الوصول للخصائص، استدعاء الطرق، الوراثة
//
// English Description:
//   Implementation of all ObjectInstance functions defined in object_instance.h
//   Includes: field access, method calls, inheritance
// ======================================================================

#include "object_instance.h"
#include "class_type.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <type_traits>

namespace Sad
{
    namespace Data
    {

        // عداد عام لتوليد معرفات فريدة
        // Global counter for generating unique IDs
        static std::atomic<size_t> globalObjectIdCounter{1};

        // ──────────────────────────────────────────────────────────────────────
        // (AR) محوّل داخلي لاستخراج المؤشر الخام من ObjectPtr بشكل شفاف.
        //      أثناء B-step5b ObjectPtr قد يكون shared_ptr<ObjectInstance> أو
        //      ObjectInstance* خام. التحميل الزائد يختار التطبيق المناسب.
        // (EN) Internal helper to extract raw ObjectInstance* from ObjectPtr
        //      transparently. During B-step5b ObjectPtr may be shared_ptr or
        //      raw — overload resolution picks the right one.
        // ──────────────────────────────────────────────────────────────────────
        static inline ObjectInstance *rawFromObjectPtr(const std::shared_ptr<ObjectInstance> &p) { return p.get(); }
        static inline ObjectInstance *rawFromObjectPtr(ObjectInstance *p) { return p; }

        // ──────────────────────────────────────────────────────────────────────
        // (AR) خطّافات تتبّع التخصيص (B-step4)
        //      انظر التعليق المفصّل في object_instance.h. الفكرة الجوهرية:
        //      hooks ساكنة قابلة للتسجيل من المستهلك (مثل المفسّر) ليربط
        //      ctor/dtor بمحرك GC الموحَّد دون تبعية صلبة من shared/types.
        //      التزامن: قفل قصير لقراءة/كتابة المؤشرات + نسخ الـ functor
        //      محلياً قبل الاستدعاء (يمنع deadlock إن أنشأ الـ hook كائناً).
        // (EN) Allocation tracking hooks (B-step4) — see object_instance.h for
        //      full rationale. Static, registrable, lock-protected, copy-then-
        //      invoke to avoid re-entrancy deadlocks.
        // ──────────────────────────────────────────────────────────────────────
        namespace
        {
            std::mutex &hooksMutex()
            {
                // (AR) Meyer's singleton — يُنشأ مرة واحدة بأمان
                // (EN) Meyer's singleton — initialized exactly once, thread-safe
                static std::mutex m;
                return m;
            }

            ObjectInstance::ObjectAllocHook &allocHookSlot()
            {
                static ObjectInstance::ObjectAllocHook h;
                return h;
            }

            ObjectInstance::ObjectFreeHook &freeHookSlot()
            {
                static ObjectInstance::ObjectFreeHook h;
                return h;
            }
        } // namespace

        void ObjectInstance::setAllocHook(ObjectAllocHook hook)
        {
            std::lock_guard<std::mutex> lk(hooksMutex());
            allocHookSlot() = std::move(hook);
        }

        void ObjectInstance::setFreeHook(ObjectFreeHook hook)
        {
            std::lock_guard<std::mutex> lk(hooksMutex());
            freeHookSlot() = std::move(hook);
        }

        void ObjectInstance::clearHooks()
        {
            std::lock_guard<std::mutex> lk(hooksMutex());
            allocHookSlot() = nullptr;
            freeHookSlot() = nullptr;
        }

        bool ObjectInstance::hasAllocHook()
        {
            std::lock_guard<std::mutex> lk(hooksMutex());
            return static_cast<bool>(allocHookSlot());
        }

        // ======================================================================
        // المنشئات والهدامات / Constructors and Destructors
        // ======================================================================

        ObjectInstance::ObjectInstance(ClassType *cls, size_t id)
            : classType(cls), objectId(id), isConstructed(false)
        {
            // (AR) إنشاء كائن جديد
            // (EN) Create new object

            // (AR) B-step4: استدعاء خطّاف التخصيص إن كان مسجَّلاً.
            //      ننسخ الـ functor محلياً تحت قفل قصير ثم نستدعيه خارج
            //      القفل لتجنّب re-entrancy إن قام الـ hook بإنشاء كائن آخر.
            //      sizeof(ObjectInstance) تقدير معقول للحجم — حقول fields
            //      تتغيّر فلا نحاول حسابها هنا (هذه إحصائيات تقريبية).
            // (EN) B-step4: invoke alloc hook if registered. Copy functor
            //      under short lock, then invoke outside lock to avoid
            //      re-entrancy. sizeof(ObjectInstance) is an approximation.
            ObjectAllocHook localHook;
            {
                std::lock_guard<std::mutex> lk(hooksMutex());
                localHook = allocHookSlot();
            }
            if (localHook)
            {
                localHook(this, sizeof(ObjectInstance));
            }
        }

        ObjectInstance::~ObjectInstance()
        {
            // (AR) الهدام - يُستدعى تلقائياً عند حذف الكائن
            // (EN) Destructor - called automatically when object is deleted

            // (AR) ملاحظة: استدعاء الهدام المخصص يتم من ObjectManager
            // (EN) Note: custom destructor call is handled by ObjectManager

            // (AR) B-step4: استدعاء خطّاف التحرير إن كان مسجَّلاً.
            //      نفس نمط النسخ-ثم-الاستدعاء كما في ctor. نتجاهل أي
            //      استثناء يخرج من الـ hook (لا يجوز رمي استثناءات من dtor).
            // (EN) B-step4: invoke free hook if registered. Same copy-then-
            //      invoke pattern. Swallow any hook exception (dtors must
            //      not throw).
            ObjectFreeHook localHook;
            {
                std::lock_guard<std::mutex> lk(hooksMutex());
                localHook = freeHookSlot();
            }
            if (localHook)
            {
                try
                {
                    localHook(this);
                }
                catch (...)
                {
                    // (AR) ابتلاع الاستثناء — dtor لا يجوز أن يرمي
                    // (EN) Swallow — dtor must not throw
                }
            }
        }

        // ======================================================================
        // (AR) تعداد الأطفال للـ GC mark phase
        // (EN) Visit child pointers for GC mark phase
        // ======================================================================

        void ObjectInstance::visitChildren(const std::function<void(void *)> &visitor) const
        {
            // (AR) لكل حقل من نوع OBJECT، استدعِ visitor على المؤشر الخام.
            //      نستخرج المؤشر من ObjectPtr بطريقة شفافة بصرف النظر عن
            //      كونها shared_ptr (لها .get()) أو raw pointer (هي نفسها مؤشر).
            // (EN) For each OBJECT-typed field, invoke visitor on the raw pointer.
            for (const auto &kv : fields)
            {
                const Value &v = kv.second;
                if (v.isObject())
                {
                    auto child = v.toObject();
                    ObjectInstance *raw = rawFromObjectPtr(child);
                    if (raw != nullptr)
                    {
                        visitor(static_cast<void *>(raw));
                    }
                }
            }
            // (AR) تعداد الكائن الأساسي (وراثة) — unique_ptr يبقى خاماً عبر get().
            if (baseInstance)
            {
                visitor(static_cast<void *>(baseInstance.get()));
            }
        }

        // ======================================================================
        // الوصول للخصائص / Field Access
        // ======================================================================

        Value *ObjectInstance::getField(const std::string &fieldName)
        {
            // (AR) الحصول على قيمة خاصية
            // (EN) Get field value

            // (AR) البحث في الكائن الحالي
            // (EN) Search in current object
            auto it = fields.find(fieldName);
            if (it != fields.end())
            {
                return &it->second;
            }

            // (AR) البحث في الكائن الأساسي
            // (EN) Search in base object
            if (baseInstance)
            {
                return baseInstance->getField(fieldName);
            }

            return nullptr;
        }

        bool ObjectInstance::setField(const std::string &fieldName, const Value &value)
        {
            // (AR) تعيين قيمة خاصية — مع إطلاق مراقب التغييرات إن وُجد
            // (EN) Set field value — with observer notification if registered

            // (AR) فحص وجود الخاصية في تعريف الصنف
            // (EN) Check if field exists in class definition
            // (AR) أمان: classType قد يكون nullptr للكائنات الداخلية (قنوات وغيرها)
            // (EN) Safety: classType may be nullptr for internal objects (channels etc.)
            if (!classType)
            {
                // (AR) إطلاق المراقب قبل الكتابة (كائنات داخلية)
                // (EN) Fire observer before write (internal objects)
                if (onFieldChanged_)
                {
                    auto it = fields.find(fieldName);
                    Value oldValue = (it != fields.end()) ? it->second : Value();
                    fields[fieldName] = value;
                    onFieldChanged_(this, fieldName, oldValue, value);
                }
                else
                {
                    fields[fieldName] = value;
                }
                return true;
            }
            ClassField *fieldDef = classType->findField(fieldName);
            if (!fieldDef)
            {
                // (AR) حقل ديناميكي — السماح بإنشائه مباشرة مع إطلاق المراقب
                // (EN) Dynamic field — allow direct creation with observer notification
                if (onFieldChanged_)
                {
                    auto it = fields.find(fieldName);
                    Value oldValue = (it != fields.end()) ? it->second : Value();
                    fields[fieldName] = value;
                    onFieldChanged_(this, fieldName, oldValue, value);
                }
                else
                {
                    fields[fieldName] = value;
                }
                return true;
            }

            // (AR) فحص أن الخاصية ليست ثابتة
            // (EN) Check that field is not static
            if (fieldDef->isStatic)
            {
                // (AR) الخصائص الثابتة تُخزّن في الصنف، ليس الكائن
                // (EN) Static fields are stored in class, not object
                return false;
            }

            // (AR) تعيين القيمة مع إطلاق المراقب
            // (EN) Set value with observer notification
            if (onFieldChanged_)
            {
                auto it = fields.find(fieldName);
                Value oldValue = (it != fields.end()) ? it->second : Value();
                fields[fieldName] = value;
                onFieldChanged_(this, fieldName, oldValue, value);
            }
            else
            {
                fields[fieldName] = value;
            }
            return true;
        }

        bool ObjectInstance::hasField(const std::string &fieldName) const
        {
            // (AR) فحص وجود خاصية
            // (EN) Check if field exists

            if (fields.find(fieldName) != fields.end())
            {
                return true;
            }

            if (baseInstance)
            {
                return baseInstance->hasField(fieldName);
            }

            return false;
        }

        std::vector<std::string> ObjectInstance::getFieldNames() const
        {
            // (AR) الحصول على أسماء جميع الخصائص
            // (EN) Get all field names

            std::vector<std::string> names;

            // (AR) إضافة خصائص الكائن الحالي
            // (EN) Add current object fields
            for (const auto &pair : fields)
            {
                names.push_back(pair.first);
            }

            // (AR) إضافة خصائص الكائن الأساسي (مع إزالة التكرار)
            // (EN) Add base object fields (deduplicated)
            if (baseInstance)
            {
                auto baseNames = baseInstance->getFieldNames();
                for (const auto &baseName : baseNames)
                {
                    if (std::find(names.begin(), names.end(), baseName) == names.end())
                    {
                        names.push_back(baseName);
                    }
                }
            }

            return names;
        }

        // ======================================================================
        // استدعاء الطرق / Method Invocation
        // ======================================================================

        bool ObjectInstance::hasMethod(const std::string &methodName) const
        {
            // (AR) فحص وجود طريقة
            // (EN) Check if method exists
            // (AR) أمان: classType قد يكون nullptr للكائنات الداخلية
            // (EN) Safety: classType may be nullptr for internal objects
            if (!classType)
                return false;
            return classType->findMethod(methodName) != nullptr;
        }

        ClassMethod *ObjectInstance::getMethod(const std::string &methodName)
        {
            // (AR) الحصول على طريقة
            // (EN) Get method
            // (AR) أمان: classType قد يكون nullptr للكائنات الداخلية
            // (EN) Safety: classType may be nullptr for internal objects
            if (!classType)
                return nullptr;
            return classType->findMethod(methodName);
        }

        // ======================================================================
        // معلومات الصنف / Class Information
        // ======================================================================

        bool ObjectInstance::isInstanceOf(const std::string &className) const
        {
            // (AR) فحص نوع الكائن — يمشي على سلسلة الوراثة كاملة
            // (EN) Check object type — walks the full inheritance chain

            const ClassType *current = classType;
            while (current)
            {
                if (current->name == className)
                    return true;
                current = current->baseClass;
            }
            return false;
        }

        bool ObjectInstance::isInstanceOf(const ClassType *cls) const
        {
            // (AR) فحص نوع الكائن
            // (EN) Check object type

            if (!cls)
                return false;
            // (AR) أمان: classType قد يكون nullptr للكائنات الداخلية
            // (EN) Safety: classType may be nullptr for internal objects
            if (!classType)
                return false;
            return classType->inheritsFrom(cls);
        }

        // ======================================================================
        // الوراثة / Inheritance
        // ======================================================================

        void ObjectInstance::setBaseInstance(std::unique_ptr<ObjectInstance> base)
        {
            // (AR) تعيين كائن الصنف الأساسي
            // (EN) Set base class instance

            baseInstance = std::move(base);
        }

        // ======================================================================
        // تهيئة الخصائص / Field Initialization
        // ======================================================================

        void ObjectInstance::initializeFields()
        {
            // (AR) تهيئة جميع الخصائص بقيمها الافتراضية
            // (EN) Initialize all fields with default values

            // (AR) تهيئة خصائص الصنف الأساسي أولاً
            // (EN) Initialize base class fields first
            if (baseInstance)
            {
                baseInstance->initializeFields();
            }

            // (AR) أمان: classType قد يكون nullptr للكائنات الداخلية (قنوات وغيرها)
            // (EN) Safety: classType may be nullptr for internal objects (channels etc.)
            if (!classType)
                return;

            // (AR) تهيئة خصائص الصنف الحالي
            // (EN) Initialize current class fields
            for (const auto &field : classType->fields)
            {
                if (!field.isStatic)
                {
                    // (AR) الخصائص غير الثابتة تُخزّن في الكائن
                    // (EN) Non-static fields are stored in object
                    fields[field.name] = field.defaultValue;
                }
            }
        }

        // ======================================================================
        // التمثيل النصي / String Representation
        // ======================================================================

        std::string ObjectInstance::toString() const
        {
            // (AR) تحويل الكائن إلى نص
            // (EN) Convert object to string

            std::ostringstream oss;
            // (AR) أمان: classType قد يكون nullptr للكائنات الداخلية
            // (EN) Safety: classType may be nullptr for internal objects
            oss << "<" << getClassName() << "#" << objectId << ": ";

            bool first = true;
            for (const auto &pair : fields)
            {
                if (!first)
                    oss << ", ";
                oss << pair.first << "=" << pair.second.toString();
                first = false;
            }

            oss << ">";
            return oss.str();
        }

        void ObjectInstance::printDebugInfo() const
        {
            // (AR) طباعة معلومات الكائن للتصحيح
            // (EN) Print object information for debugging

            std::cout << "╔════════════════════════════════════════╗\n";
            // (AR) أمان: استخدام getClassName() بدلاً من classType->name مباشرة
            // (EN) Safety: use getClassName() instead of classType->name directly
            std::cout << "║   Object #" << objectId << " of " << getClassName() << "\n";
            std::cout << "╠════════════════════════════════════════╣\n";
            std::cout << "║ Constructed: " << (isConstructed ? "Yes" : "No") << "\n";
            std::cout << "║ Fields: " << fields.size() << "\n";

            for (const auto &pair : fields)
            {
                std::cout << "║   - " << pair.first << " = "
                          << pair.second.toString() << "\n";
            }

            if (baseInstance)
            {
                std::cout << "║ Base Instance: #" << baseInstance->objectId << "\n";
            }

            std::cout << "╚════════════════════════════════════════╝\n";
        }

        // ======================================================================
        // معلومات الذاكرة / Memory Information
        // ======================================================================

        size_t ObjectInstance::getMemoryUsage() const
        {
            // (AR) حساب حجم الذاكرة المستخدمة تقريبياً
            // (EN) Calculate approximate memory usage

            size_t size = sizeof(ObjectInstance);

            // (AR) حجم خريطة الخصائص
            // (EN) Size of fields map
            size += fields.size() * (sizeof(std::string) + sizeof(Value));

            // (AR) حجم الكائن الأساسي
            // (EN) Size of base instance
            if (baseInstance)
            {
                size += baseInstance->getMemoryUsage();
            }

            return size;
        }

        // ======================================================================
        // دوال مساعدة عامة / Global Helper Functions
        // ======================================================================

        size_t generateObjectId()
        {
            // (AR) توليد معرف فريد للكائن
            // (EN) Generate unique object ID

            return globalObjectIdCounter++;
        }

        bool compareObjects(const ObjectInstance *obj1, const ObjectInstance *obj2)
        {
            // (AR) مقارنة كائنين
            // (EN) Compare two objects

            if (!obj1 || !obj2)
                return false;

            // (AR) يجب أن يكونا من نفس الصنف
            // (EN) Must be of same class
            if (obj1->classType != obj2->classType)
                return false;

            // (AR) مقارنة جميع الخصائص
            // (EN) Compare all fields
            if (obj1->fields.size() != obj2->fields.size())
                return false;

            for (const auto &pair : obj1->fields)
            {
                auto it = obj2->fields.find(pair.first);
                if (it == obj2->fields.end())
                    return false;

                // (AR) مقارنة القيم باستخدام operator==
                // (EN) Compare values using operator==
                Value cmpResult = (pair.second == it->second);
                if (!cmpResult.toBool())
                    return false;
            }

            // (AR) مقارنة الكائنات الأساسية
            // (EN) Compare base instances
            if (obj1->baseInstance && obj2->baseInstance)
            {
                return compareObjects(obj1->baseInstance.get(), obj2->baseInstance.get());
            }

            return !obj1->baseInstance && !obj2->baseInstance;
        }

    } // namespace Data
} // namespace Sad

// ======================================================================
// نهاية الملف / End of File
// ======================================================================
