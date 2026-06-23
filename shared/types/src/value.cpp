/**
 * @file value.cpp
 * @brief (AR) تنفيذ فئة القيمة في وقت التشغيل
 * @brief (EN) Runtime value class implementation
 *
 * ═══════════════════════════════════════════════════════════════════════════
 * (AR) هذا الملف يحتوي على تنفيذ جميع دوال فئة Value
 *      بما في ذلك دعم نوع OBJECT الجديد لتمثيل كائنات الأصناف
 *
 *      التغييرات الرئيسية في الإصدار 2.0:
 *      - إضافة منشئ Value(ObjectPtr) لإنشاء قيم كائنات
 *      - إضافة toObject() للحصول على مؤشر الكائن
 *      - إضافة getClassName() للحصول على اسم الصنف
 *      - إضافة isObjectLike() للتوافق مع الكود القديم
 *      - تحديث toString(), toBool(), getTypeName(), clone() لدعم OBJECT
 *
 * (EN) This file contains the implementation of all Value class methods
 *      including support for the new OBJECT type for class instances
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * @author S Language Development Team
 * @date November 21, 2025 - يناير 2026
 * @version 2.0
 */

// NOTE(#23): (AR) لا يوجد حالياً تجميع/مشاركة النصوص (string interning/pool).
//            كل نص يُخزّن كنسخة مستقلة. يمكن إضافة string pool في المستقبل لتوفير الذاكرة.
// NOTE(#23): (EN) No string interning/deduplication is currently implemented.
//            Each string is stored as an independent copy. A string pool could be added later.
//
// NOTE(#26): (AR) MapType يستخدم std::unordered_map — ترتيب التكرار غير حتمي.
//            إذا أراد المستخدم ترتيباً ثابتاً، يجب استخدام مصفوفة من الأزواج أو std::map.
// NOTE(#26): (EN) MapType uses std::unordered_map — iteration order is non-deterministic.
//            If deterministic order is needed, use an array of pairs or std::map.

#include <string>
#include "value.h"
#include "sad_type_system.h"
#include "object_instance.h"
#include "memory/gc/engine/garbage_collector.h"
#include <cmath>
#include <iomanip>
#include <limits>
#include <climits>

namespace
{

    // (AR) حماية طفحان الأعداد الصحيحة — تمنع السلوك غير المحدد
    // (EN) Integer overflow protection — prevents undefined behavior
    inline bool willAddOverflow(int64_t a, int64_t b)
    {
        if (b > 0 && a > INT64_MAX - b)
            return true;
        if (b < 0 && a < INT64_MIN - b)
            return true;
        return false;
    }
    inline bool willSubOverflow(int64_t a, int64_t b)
    {
        if (b < 0 && a > INT64_MAX + b)
            return true;
        if (b > 0 && a < INT64_MIN + b)
            return true;
        return false;
    }
    inline bool willMulOverflow(int64_t a, int64_t b)
    {
        if (a == 0 || b == 0)
            return false;
        if (a > 0 && b > 0 && a > INT64_MAX / b)
            return true;
        if (a < 0 && b < 0 && a < INT64_MAX / b)
            return true;
        if (a > 0 && b < 0 && b < INT64_MIN / a)
            return true;
        if (a < 0 && b > 0 && a < INT64_MIN / b)
            return true;
        return false;
    }

    // (AR) حساب عدد أحرف UTF-8 (ليس البايتات)
    // (EN) Count UTF-8 characters (not bytes)
    inline size_t utf8CharCount(const std::string &s)
    {
        size_t count = 0;
        for (size_t i = 0; i < s.size();)
        {
            unsigned char c = static_cast<unsigned char>(s[i]);
            if (c < 0x80)
            {
                i += 1;
            }
            else if ((c & 0xE0) == 0xC0)
            {
                i += 2;
            }
            else if ((c & 0xF0) == 0xE0)
            {
                i += 3;
            }
            else if ((c & 0xF8) == 0xF0)
            {
                i += 4;
            }
            else
            {
                i += 1;
            } // invalid byte, skip
            ++count;
        }
        return count;
    }

    // (AR) الحد الأقصى لتكرار النصوص/المصفوفات لمنع استنفاد الذاكرة
    // (EN) Max repetition limit to prevent memory exhaustion
    static constexpr int MAX_REPETITION = 1000000;
    static constexpr size_t MAX_STRING_SIZE = 100 * 1024 * 1024; // 100MB

} // anonymous namespace

namespace Sad
{
    namespace Data
    {

        // (AR) اختصارات نظام الأنواع الغني — ADR-01 Phase 3
        // (EN) Rich type system shortcuts — ADR-01 Phase 3
        using Types::SadTypeKind;
        using Types::SadTypePtr;
        using Types::SadTypeRegistry;
        static inline SadTypeRegistry &reg() { return SadTypeRegistry::instance(); }

        // ========================================
        // (AR) تنفيذ مقارنة بنية الصف / (EN) TupleType equality implementation
        // ========================================
        bool Value::TupleType::operator==(const TupleType &other) const
        {
            if (elements.size() != other.elements.size())
                return false;
            for (size_t i = 0; i < elements.size(); ++i)
            {
                if (!(elements[i] == other.elements[i]).toBool())
                    return false;
            }
            return true;
        }

        // ========================================
        // Constructors / المُنشئات
        // ========================================

        Value::Value() : sadType_(reg().getVoid()), type_(::Sad::Types::SadTypeKind::Void), data_(std::monostate{}) {}

        // (AR) عدم (Null) — قيمة متمايزة عن فراغ (Void) — S-TS-P1
        // (EN) Null — a value distinct from void — S-TS-P1
        Value Value::makeNull()
        {
            Value v;
            v.type_ = Types::SadTypeKind::Null;
            v.sadType_ = reg().getNull();
            return v;
        }

        Value::Value(int val) : sadType_(reg().getInteger()), type_(::Sad::Types::SadTypeKind::Integer), data_(static_cast<int64_t>(val)) {}

        Value::Value(int64_t val) : sadType_(reg().getInteger()), type_(::Sad::Types::SadTypeKind::Integer), data_(val) {}

        Value::Value(double val) : sadType_(reg().getFloat()), type_(::Sad::Types::SadTypeKind::Float), data_(val) {}

        Value::Value(const std::string &val) : sadType_(reg().getString()), type_(::Sad::Types::SadTypeKind::String), data_(val) {}

        // (AR) منشئ نقل النص — يتجنب نسخ النص عند النقل / (EN) String move constructor — avoids copy on move
        Value::Value(std::string &&val) : sadType_(reg().getString()), type_(::Sad::Types::SadTypeKind::String), data_(std::move(val)) {}

        Value::Value(const char *val) : sadType_(reg().getString()), type_(::Sad::Types::SadTypeKind::String), data_(std::string(val)) {}

        Value::Value(bool val) : sadType_(reg().getBoolean()), type_(::Sad::Types::SadTypeKind::Boolean), data_(val) {}

        Value::Value(const ArrayType &val)
            : sadType_(reg().makeArray()), type_(::Sad::Types::SadTypeKind::Array), data_(std::make_shared<ArrayType>(val)) {}

        // (AR) منشئ نقل المصفوفة — يتجنب نسخ العناصر / (EN) Array move constructor — avoids element copy
        Value::Value(ArrayType &&val)
            : sadType_(reg().makeArray()), type_(::Sad::Types::SadTypeKind::Array), data_(std::make_shared<ArrayType>(std::move(val))) {}

        Value::Value(const MapType &val)
            : sadType_(reg().makeMap()), type_(::Sad::Types::SadTypeKind::Map), data_(std::make_shared<MapType>(val)) {}

        // (AR) منشئ نقل القاموس — يتجنب نسخ العناصر / (EN) Map move constructor — avoids element copy
        Value::Value(MapType &&val)
            : sadType_(reg().makeMap()), type_(::Sad::Types::SadTypeKind::Map), data_(std::make_shared<MapType>(std::move(val))) {}

        // ════════════════════════════════════════════════════════════════════════
        // (AR) منشئات الصف — يُنشئ قيمة من نوع TUPLE (مجموعة مرتبة غير قابلة للتغيير)
        //      الصف مشابه للمصفوفة لكنه ثابت الطول ويُعبّر عن مجموعة قيم مرتبطة
        //      يُستخدم TupleTag للتمييز بين منشئ الصف ومنشئ المصفوفة
        // (EN) Tuple constructors — creates TUPLE value (ordered immutable collection)
        //      Tuples are similar to arrays but fixed-length and express related values
        //      TupleTag disambiguates tuple constructor from array constructor
        // ════════════════════════════════════════════════════════════════════════
        Value::Value(TupleTag, const TupleType &val)
            : sadType_(reg().makeTuple({})), type_(::Sad::Types::SadTypeKind::Tuple), data_(std::make_shared<TupleType>(val)) {}

        Value::Value(TupleTag, TupleType &&val)
            : sadType_(reg().makeTuple({})), type_(::Sad::Types::SadTypeKind::Tuple), data_(std::make_shared<TupleType>(std::move(val))) {}

        // ════════════════════════════════════════════════════════════════════════
        // (AR) منشئ الكائن — يُنشئ قيمة من نوع OBJECT تحمل مؤشراً مشتركاً
        //      للكائن. الكائن يُمرّر بالمرجع عند تمريره للدوال أو تعيينه
        //      لمتغيرات أخرى (سلوك مشابه لـ Python/Java)
        //
        // (EN) Object constructor — creates an OBJECT value holding a shared_ptr
        //      to the object instance. Objects are passed by reference when passed
        //      to functions or assigned to other variables (Python/Java-like behavior)
        // ════════════════════════════════════════════════════════════════════════
        Value::Value(ObjectPtr obj)
            // (AR) [W15-01 / إصلاح جذري P0-1] نَسِم الكائن بنوع صنفه (Class) لا بـAny،
            //      ليُرجع getKind()=Class ⇒ نوع()=«كائن» — مطابقةً للمترجم (المرجع الصحيح).
            //      كان reg().getAny() يجعل نوع(زر()) ونوع(نقطة(5)) تُرجع «أي» خطأً (تباعد
            //      مزدوج: المفسّر≠المترجم). getOrCreateClass مُموَّه (interned) فالكلفة بحث
            //      مفهرس لا تخصيصٌ متكرّر. اسمٌ فارغ (كائن داخليّ بلا صنف) يبقى Class=«كائن».
            // (EN) [W15-01 / P0-1 root fix] Tag the object with its class type (Class) not
            //      Any, so getKind()=Class ⇒ نوع()=«object» — matching the compiler (the
            //      correct reference). reg().getAny() wrongly made نوع(button())/نوع(point(5))
            //      return «any» (interpreter≠compiler divergence). getOrCreateClass is
            //      interned (cached lookup, not re-alloc); an empty name (internal object
            //      without a class) still yields Class ⇒ «object».
            : sadType_(obj ? reg().getOrCreateClass(obj->getClassName()) : reg().getAny()),
              type_(::Sad::Types::SadTypeKind::Class), data_(obj)
        {
            // (AR) B-step5b: تأكّد أن الكائن مسجَّل في GC ومُجهَّز بـdestroyer + visitor.
            //      نتجنّب التسجيل المكرّر لأن الكائن قد يكون مُسجَّلاً مسبقاً (مثلاً في
            //      ClassType::createInstance) — في هذه الحالة نُحدّث destroyer/visitor فقط.
            //      destroyer مسؤول عن `delete` لأن الكائنات مُنشأة بـnew.
            //      visitor يفوّض إلى ObjectInstance::visitChildren للتعداد الدقيق.
            // (EN) B-step5b: ensure object is GC-tracked with destroyer + visitor.
            //      Avoid duplicate registration — if already tracked (e.g. by
            //      ClassType::createInstance), only update its destroyer/visitor.
            if (obj != nullptr)
            {
                auto &gc = ::Sad::Memory::GC::defaultEngine();
                if (!gc.isTracked(static_cast<void *>(obj)))
                {
                    gc.registerObject(static_cast<void *>(obj),
                                      static_cast<uint64_t>(sizeof(ObjectInstance)),
                                      [](void *p)
                                      { delete static_cast<ObjectInstance *>(p); });
                    gc.setVisitor(static_cast<void *>(obj),
                                  [](void *p, const std::function<void(void *)> &visitor)
                                  {
                                      static_cast<ObjectInstance *>(p)->visitChildren(visitor);
                                  });
                }
            }
        }

        // ════════════════════════════════════════════════════════════════════════
        // (AR) منشئ مرجع الدالة — يُنشئ قيمة من نوع FUNCTION تحمل مؤشراً مشتركاً
        //      لبنية FunctionRef. يسمح بتمريرها كقيم من الدرجة الأولى
        //
        // (EN) Function reference constructor — creates a FUNCTION value holding
        //      a shared_ptr to FunctionRef struct. Enables first-class functions
        // ════════════════════════════════════════════════════════════════════════
        Value::Value(FunctionRefPtr funcRef)
            : sadType_(reg().makeFunction({})), type_(::Sad::Types::SadTypeKind::Function), data_(std::move(funcRef)) {}

        Value::Value(const FunctionRef &funcRef)
            : sadType_(reg().makeFunction({})), type_(::Sad::Types::SadTypeKind::Function), data_(std::make_shared<FunctionRef>(funcRef)) {}

        // ========================================
        // Clone / النسخ العميق
        // ========================================

        Value Value::clone() const
        {
            switch (type_)
            {
            case ::Sad::Types::SadTypeKind::Array:
            {
                const auto &arr = *std::get<std::shared_ptr<ArrayType>>(data_);
                return Value(arr); // Creates new shared_ptr with copy
            }
            case ::Sad::Types::SadTypeKind::Map:
            {
                const auto &map = *std::get<std::shared_ptr<MapType>>(data_);
                return Value(map); // Creates new shared_ptr with copy
            }
            case ::Sad::Types::SadTypeKind::Tuple:
            {
                // (AR) نسخ عميق للصف / (EN) Deep clone for tuple
                const auto &tup = *std::get<std::shared_ptr<TupleType>>(data_);
                return Value(TupleTag{}, tup);
            }
            case ::Sad::Types::SadTypeKind::Class:
            {
                // (AR) نسخ عميق للكائن: نُنشئ ObjectInstance جديد بنفس الحقول.
                //      B-step5b: إنشاء خام بـnew، ثم Value(ObjectPtr) سيسجّله
                //      في GC تلقائياً مع destroyer مناسب.
                // (EN) Deep clone for object: create new ObjectInstance with same fields.
                //      B-step5b: raw `new`, then Value(ObjectPtr) registers in GC.
                ObjectInstance *srcPtr = std::get<ObjectInstance *>(data_);
                if (srcPtr)
                {
                    // (AR) نسخ عميق بمعرف فريد جديد
                    // (EN) Deep clone with new unique object ID
                    ObjectInstance *newObj = new ObjectInstance(srcPtr->classType, generateObjectId());
                    newObj->fields = srcPtr->fields;
                    newObj->isConstructed = srcPtr->isConstructed;
                    // (AR) نسخ الكائن الأساسي إذا وُجد
                    // (EN) Clone base instance if present
                    if (srcPtr->baseInstance)
                    {
                        newObj->baseInstance = std::make_unique<ObjectInstance>(
                            srcPtr->baseInstance->classType, generateObjectId());
                        newObj->baseInstance->fields = srcPtr->baseInstance->fields;
                        newObj->baseInstance->isConstructed = srcPtr->baseInstance->isConstructed;
                    }
                    return Value(newObj);
                }
                return *this;
            }
            case ::Sad::Types::SadTypeKind::Function:
            {
                // (AR) نسخ ضحل لمرجع الدالة — نفس FunctionRef (مشترك)
                // (EN) Shallow copy for function reference — same FunctionRef (shared)
                auto srcRef = std::get<std::shared_ptr<FunctionRef>>(data_);
                if (srcRef)
                {
                    // (AR) نسخ عميق — FunctionRef جديد بنفس المحتوى
                    // (EN) Deep copy — new FunctionRef with same content
                    return Value(std::make_shared<FunctionRef>(*srcRef));
                }
                return *this;
            }
            default:
                return *this; // Shallow copy for primitives
            }
        }

        // ========================================
        // Type Conversion / تحويل النوع
        // ========================================

        int Value::toInt() const
        {
            switch (type_)
            {
            case ::Sad::Types::SadTypeKind::Integer:
                return static_cast<int>(std::get<int64_t>(data_));

            case ::Sad::Types::SadTypeKind::Float:
                return static_cast<int>(std::get<double>(data_));

            case ::Sad::Types::SadTypeKind::Boolean:
                return std::get<bool>(data_) ? 1 : 0;

            case ::Sad::Types::SadTypeKind::String:
            {
                try
                {
                    return std::stoi(std::get<std::string>(data_));
                }
                catch (...)
                {
                    throwInvalidType("toInt - cannot convert string to integer");
                }
            }

            case ::Sad::Types::SadTypeKind::Void:
                throwInvalidType("toInt - cannot convert void to integer");
            }
            return 0;
        }

        int64_t Value::toInt64() const
        {
            switch (type_)
            {
            case ::Sad::Types::SadTypeKind::Integer:
                return std::get<int64_t>(data_);

            case ::Sad::Types::SadTypeKind::Float:
                return static_cast<int64_t>(std::get<double>(data_));

            case ::Sad::Types::SadTypeKind::Boolean:
                return std::get<bool>(data_) ? 1LL : 0LL;

            case ::Sad::Types::SadTypeKind::String:
            {
                try
                {
                    return std::stoll(std::get<std::string>(data_));
                }
                catch (...)
                {
                    throwInvalidType("toInt64 - cannot convert string to integer");
                }
            }

            case ::Sad::Types::SadTypeKind::Void:
                throwInvalidType("toInt64 - cannot convert void to integer");
            }
            return 0;
        }

        double Value::toDouble() const
        {
            switch (type_)
            {
            case ::Sad::Types::SadTypeKind::Integer:
                return static_cast<double>(std::get<int64_t>(data_));

            case ::Sad::Types::SadTypeKind::Float:
                return std::get<double>(data_);

            case ::Sad::Types::SadTypeKind::Boolean:
                return std::get<bool>(data_) ? 1.0 : 0.0;

            case ::Sad::Types::SadTypeKind::String:
            {
                try
                {
                    return std::stod(std::get<std::string>(data_));
                }
                catch (...)
                {
                    throwInvalidType("toDouble - cannot convert string to double");
                }
            }

            case ::Sad::Types::SadTypeKind::Void:
                throwInvalidType("toDouble - cannot convert void to double");
            }
            return 0.0;
        }

        std::string Value::toString() const
        {
            switch (type_)
            {
            case ::Sad::Types::SadTypeKind::Integer:
                return std::to_string(std::get<int64_t>(data_));

            case ::Sad::Types::SadTypeKind::Float:
            {
                // (AR) إصلاح: استخدام fixed مع 6 خانات عشرية + حذف أصفار زائدة
                //      لمطابقة سلوك valueToString() في io_functions.cpp
                //      هذا يضمن تناسق التنسيق بين اطبع_سطر(ع) و "نص" + ع
                // (EN) Fix: use fixed with 6 decimal places + strip trailing zeros
                //      to match valueToString() behavior in io_functions.cpp
                //      ensures consistent formatting between print(x) and "text" + x
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(6) << std::get<double>(data_);
                std::string result = oss.str();
                // (AR) حذف الأصفار الزائدة بعد النقطة العشرية
                // (EN) Strip trailing zeros after decimal point
                if (result.find('.') != std::string::npos)
                {
                    // (AR) حذف الأصفار الزائدة مع إبقاء رقم واحد بعد النقطة: 10.0 لا 10
                    // (EN) Remove trailing zeros but keep at least one digit after dot: 10.0 not 10
                    size_t dot_pos = result.find('.');
                    size_t last = result.find_last_not_of('0');
                    if (last <= dot_pos)
                        last = dot_pos + 1;
                    result.erase(last + 1);
                }
                return result;
            }

            case ::Sad::Types::SadTypeKind::String:
                return std::get<std::string>(data_);

            case ::Sad::Types::SadTypeKind::Boolean:
                return std::get<bool>(data_) ? "صحيح" : "خطأ";

            case ::Sad::Types::SadTypeKind::Array:
            {
                std::ostringstream oss;
                oss << "[";
                const auto &arr = *std::get<std::shared_ptr<ArrayType>>(data_);
                for (size_t i = 0; i < arr.size(); ++i)
                {
                    if (i > 0)
                        oss << ", ";
                    oss << arr[i].toString();
                }
                oss << "]";
                return oss.str();
            }

            case ::Sad::Types::SadTypeKind::Map:
            {
                std::ostringstream oss;
                oss << "{";
                const auto &map = *std::get<std::shared_ptr<MapType>>(data_);
                size_t i = 0;
                for (const auto &pair : map)
                {
                    if (i++ > 0)
                        oss << ", ";
                    oss << pair.first << ": " << pair.second.toString();
                }
                oss << "}";
                return oss.str();
            }

            case ::Sad::Types::SadTypeKind::Tuple:
            {
                // (AR) تحويل الصف إلى نص بصيغة (قيمة1، قيمة2، ...)
                // (EN) Convert tuple to string as (value1, value2, ...)
                std::ostringstream oss;
                oss << "(";
                const auto &tup = *std::get<std::shared_ptr<TupleType>>(data_);
                for (size_t i = 0; i < tup.size(); ++i)
                {
                    if (i > 0)
                        oss << "، ";
                    oss << tup[i].toString();
                }
                if (tup.size() == 1)
                    oss << "،"; // (AR) صف بعنصر واحد / (EN) single-element tuple trailing comma
                oss << ")";
                return oss.str();
            }

            case ::Sad::Types::SadTypeKind::Class:
            {
                // (AR) تحويل الكائن إلى نص — يستخدم دالة toString() من ObjectInstance
                //      إذا كان المؤشر فارغاً، يُرجع "كائن_فارغ"
                // (EN) Convert object to string — uses toString() from ObjectInstance
                //      If pointer is null, returns "null_object"
                const auto &objPtr = std::get<ObjectInstance *>(data_);
                if (objPtr)
                {
                    return objPtr->toString();
                }
                return "كائن_فارغ"; // null object
            }

            case ::Sad::Types::SadTypeKind::Function:
            {
                // (AR) تحويل مرجع الدالة إلى نص — يستخدم toString() من FunctionRef
                // (EN) Convert function reference to string — uses toString() from FunctionRef
                const auto &funcPtr = std::get<std::shared_ptr<FunctionRef>>(data_);
                if (funcPtr)
                {
                    return funcPtr->toString();
                }
                return "<دالة:مجهولة>"; // null function ref
            }

            case ::Sad::Types::SadTypeKind::Void:
                return "\u0644\u0627\u0634\u064a\u0621"; // لاشيء
            }
            return "";
        }

        bool Value::toBool() const
        {
            switch (type_)
            {
            case ::Sad::Types::SadTypeKind::Integer:
                return std::get<int64_t>(data_) != 0;

            case ::Sad::Types::SadTypeKind::Float:
                return std::get<double>(data_) != 0.0;

            case ::Sad::Types::SadTypeKind::String:
                return !std::get<std::string>(data_).empty();

            case ::Sad::Types::SadTypeKind::Boolean:
                return std::get<bool>(data_);

            case ::Sad::Types::SadTypeKind::Array:
                return !std::get<std::shared_ptr<ArrayType>>(data_)->empty();

            case ::Sad::Types::SadTypeKind::Map:
                return !std::get<std::shared_ptr<MapType>>(data_)->empty();

            case ::Sad::Types::SadTypeKind::Tuple:
                return !std::get<std::shared_ptr<TupleType>>(data_)->empty();

            case ::Sad::Types::SadTypeKind::Class:
            {
                // (AR) الكائن صحيح إذا كان المؤشر غير فارغ
                // (EN) Object is true if the pointer is not null
                const auto &objPtr = std::get<ObjectInstance *>(data_);
                return objPtr != nullptr;
            }

            case ::Sad::Types::SadTypeKind::Function:
                // (AR) مرجع الدالة دائماً صحيح (مثل Python — bool(func) = True)
                // (EN) Function reference is always truthy (like Python — bool(func) = True)
                return true;

            case ::Sad::Types::SadTypeKind::Void:
                return false;
            }
            return false;
        }

        Value::ArrayType Value::toArray() const
        {
            if (type_ == ::Sad::Types::SadTypeKind::Array)
            {
                return *std::get<std::shared_ptr<ArrayType>>(data_);
            }
            throwInvalidType("toArray - value is not an array");
            return ArrayType();
        }

        Value::MapType Value::toMap() const
        {
            if (type_ == ::Sad::Types::SadTypeKind::Map)
            {
                return *std::get<std::shared_ptr<MapType>>(data_);
            }
            // (AR) إذا كان كائناً، نحوله لقاموس من الحقول للتوافق مع الكود القديم
            // (EN) If it's an object, convert to map of fields for backward compatibility
            if (type_ == ::Sad::Types::SadTypeKind::Class)
            {
                const auto &objPtr = std::get<ObjectInstance *>(data_);
                if (objPtr)
                {
                    MapType result = objPtr->fields;
                    result["__class__"] = Value(objPtr->getClassName());
                    return result;
                }
            }
            throwInvalidType("toMap - value is not a map");
            return MapType();
        }

        // (AR) تحويل القيمة إلى صف / (EN) Convert value to tuple
        Value::TupleType Value::toTuple() const
        {
            if (type_ == ::Sad::Types::SadTypeKind::Tuple)
            {
                return *std::get<std::shared_ptr<TupleType>>(data_);
            }
            throwInvalidType("toTuple - value is not a tuple");
            return TupleType();
        }

        // ════════════════════════════════════════════════════════════════════════
        // (AR) إصدارات مرجعية ثابتة — تتجنب النسخ عندما نحتاج فقط للقراءة
        // (EN) Const reference versions — avoid copying when only reading
        // ════════════════════════════════════════════════════════════════════════

        const Value::ArrayType &Value::toArrayRef() const
        {
            if (type_ == ::Sad::Types::SadTypeKind::Array)
            {
                return *std::get<std::shared_ptr<ArrayType>>(data_);
            }
            throwInvalidType("toArrayRef - value is not an array");
            // (AR) لن يصل هنا أبداً بسبب الاستثناء أعلاه
            // (EN) Never reached due to exception above
            static const ArrayType empty;
            return empty;
        }

        const Value::MapType &Value::toMapRef() const
        {
            if (type_ == ::Sad::Types::SadTypeKind::Map)
            {
                return *std::get<std::shared_ptr<MapType>>(data_);
            }
            throwInvalidType("toMapRef - value is not a map");
            // (AR) لن يصل هنا أبداً بسبب الاستثناء أعلاه
            // (EN) Never reached due to exception above
            static const MapType empty;
            return empty;
        }

        // (AR) دالة toTupleRef — مرجع ثابت للصف بدون نسخ
        // (EN) toTupleRef — const reference to tuple without copying
        const Value::TupleType &Value::toTupleRef() const
        {
            if (type_ == ::Sad::Types::SadTypeKind::Tuple)
            {
                return *std::get<std::shared_ptr<TupleType>>(data_);
            }
            throwInvalidType("toTupleRef - value is not a tuple");
            static const TupleType empty;
            return empty;
        }

        const std::string &Value::toStringRef() const
        {
            if (type_ == ::Sad::Types::SadTypeKind::String)
            {
                return std::get<std::string>(data_);
            }
            // (AR) رمي خطأ إذا لم يكن النوع نصاً / (EN) Throw if not a string type
            throwInvalidType("toStringRef - value is not a string");
            // (AR) لن يصل هنا أبداً / (EN) Never reached
            static const std::string empty;
            return empty;
        }

        // ════════════════════════════════════════════════════════════════════════
        // (AR) إصدارات قابلة للتعديل — تعديل المصفوفة/الخريطة مباشرة بدون نسخ
        // (EN) Mutable reference versions — modify array/map in-place without copying
        // ════════════════════════════════════════════════════════════════════════

        Value::ArrayType &Value::toArrayMut()
        {
            if (type_ == ::Sad::Types::SadTypeKind::Array)
            {
                return *std::get<std::shared_ptr<ArrayType>>(data_);
            }
            throwInvalidType("toArrayMut - value is not an array");
            // (AR) لن يصل هنا أبداً بسبب الاستثناء أعلاه
            static ArrayType dummy;
            return dummy;
        }

        Value::MapType &Value::toMapMut()
        {
            if (type_ == ::Sad::Types::SadTypeKind::Map)
            {
                return *std::get<std::shared_ptr<MapType>>(data_);
            }
            throwInvalidType("toMapMut - value is not a map");
            // (AR) لن يصل هنا أبداً بسبب الاستثناء أعلاه
            static MapType dummy;
            return dummy;
        }

        // ════════════════════════════════════════════════════════════════════════
        // (AR) دالة toObject — الحصول على مؤشر الكائن
        //      يجب أن تكون القيمة من نوع OBJECT
        //
        // (EN) toObject function — get the object pointer
        //      Value must be of OBJECT type
        // ════════════════════════════════════════════════════════════════════════
        Value::ObjectPtr Value::toObject() const
        {
            if (type_ == ::Sad::Types::SadTypeKind::Class)
            {
                return std::get<ObjectInstance *>(data_);
            }
            throwInvalidType("toObject - القيمة ليست كائناً / value is not an object");
            return nullptr;
        }

        // ════════════════════════════════════════════════════════════════════════
        // (AR) forEachObjectRef — تعداد جميع مؤشرات ObjectInstance المرتبطة بهذه القيمة
        //   (B-step5b-iii) تُستدعى من موفّر جذور VariableManager أثناء mark phase.
        //   تُغطّي:
        //     OBJECT  → المؤشر مباشرة (إن لم يكن null)
        //     ARRAY   → استدعاء عميق على كل عنصر (للحاويات المتداخلة)
        //     MAP     → استدعاء عميق على كل قيمة (المفاتيح نصية فقط)
        //     غير ذلك → لا شيء (سكلر، نصوص، دوال، إلخ)
        //   التصميم تكراري عميق لتغطية المصفوفات المتداخلة من الكائنات.
        //
        // (EN) Enumerate all live ObjectInstance pointers reachable from this Value.
        //   Used by VariableManager root provider during GC mark phase.
        //   Recursive descent through ARRAY/MAP containers; emits OBJECT directly.
        // ════════════════════════════════════════════════════════════════════════
        void Value::forEachObjectRef(const std::function<void(ObjectInstance *)> &emit) const
        {
            if (!emit)
            {
                return;
            }
            switch (type_)
            {
            case ::Sad::Types::SadTypeKind::Class:
            {
                auto *objPtr = std::get<ObjectInstance *>(data_);
                if (objPtr != nullptr)
                {
                    emit(objPtr);
                }
                break;
            }
            case ::Sad::Types::SadTypeKind::Array:
            {
                // (AR) المصفوفة قد تحتوي قيماً من نوع OBJECT أو حاويات أخرى — نتعمّق.
                const auto &arrPtr = std::get<std::shared_ptr<ArrayType>>(data_);
                if (arrPtr)
                {
                    for (const auto &element : *arrPtr)
                    {
                        element.forEachObjectRef(emit);
                    }
                }
                break;
            }
            case ::Sad::Types::SadTypeKind::Map:
            {
                // (AR) الخريطة: المفاتيح نصية، القيم قد تكون كائنات أو حاويات.
                const auto &mpPtr = std::get<std::shared_ptr<MapType>>(data_);
                if (mpPtr)
                {
                    for (const auto &kv : *mpPtr)
                    {
                        kv.second.forEachObjectRef(emit);
                    }
                }
                break;
            }
            default:
                // (AR) سكلر/نص/دالة/منطقي/فراغ → لا توجد مراجع كائنات.
                break;
            }
        }

        // ════════════════════════════════════════════════════════════════════════
        // (AR) دالة getClassName — الحصول على اسم الصنف
        //      تعمل مع نوع OBJECT الجديد ومع MAP القديم الذي يحتوي على __class__
        //
        // (EN) getClassName function — get the class name
        //      Works with new OBJECT type and legacy MAP with __class__
        // ════════════════════════════════════════════════════════════════════════
        std::string Value::getClassName() const
        {
            // (AR) أولاً: التحقق من نوع OBJECT الحقيقي
            // (EN) First: check for real OBJECT type
            if (type_ == ::Sad::Types::SadTypeKind::Class)
            {
                const auto &objPtr = std::get<ObjectInstance *>(data_);
                if (objPtr)
                {
                    return objPtr->getClassName();
                }
                return "";
            }
            // (AR) ثانياً: التوافق مع MAP القديم
            // (EN) Second: backward compatibility with legacy MAP
            if (type_ == ::Sad::Types::SadTypeKind::Map)
            {
                const auto &map = *std::get<std::shared_ptr<MapType>>(data_);
                auto it = map.find("__class__");
                if (it != map.end())
                {
                    return it->second.toString();
                }
            }
            return "";
        }

        // ════════════════════════════════════════════════════════════════════════
        // (AR) دالة isObjectLike — هل القيمة تمثل كائناً بأي شكل؟
        //      تكشف كلاً من:
        //      1. نوع OBJECT الحقيقي (الجديد)
        //      2. MAP القديم الذي يحتوي على مفتاح __class__
        //
        // (EN) isObjectLike function — does the value represent an object?
        //      Detects both:
        //      1. Real OBJECT type (new)
        //      2. Legacy MAP containing __class__ key
        // ════════════════════════════════════════════════════════════════════════
        bool Value::isObjectLike() const
        {
            if (type_ == ::Sad::Types::SadTypeKind::Class)
                return true;
            if (type_ == ::Sad::Types::SadTypeKind::Map)
            {
                const auto &map = *std::get<std::shared_ptr<MapType>>(data_);
                return map.find("__class__") != map.end();
            }
            return false;
        }

        // ════════════════════════════════════════════════════════════════════════
        // (AR) دالة toFunction — الحصول على مرجع الدالة
        //      يجب أن تكون القيمة من نوع FUNCTION
        //
        // (EN) toFunction — get the function reference pointer
        //      Value must be of FUNCTION type
        // ════════════════════════════════════════════════════════════════════════
        Value::FunctionRefPtr Value::toFunction() const
        {
            if (type_ == ::Sad::Types::SadTypeKind::Function)
            {
                return std::get<std::shared_ptr<FunctionRef>>(data_);
            }
            throwInvalidType("toFunction - القيمة ليست دالة / value is not a function");
            return nullptr;
        }

        // ════════════════════════════════════════════════════════════════════════
        // (AR) دالة getFunctionName — الحصول على اسم الدالة المسجل
        //      تعمل مع FUNCTION (registeredName) وSTRING (النص نفسه)
        //
        // (EN) getFunctionName — get the registered function name
        //      Works with FUNCTION (registeredName) and STRING (the string itself)
        // ════════════════════════════════════════════════════════════════════════
        std::string Value::getFunctionName() const
        {
            if (type_ == ::Sad::Types::SadTypeKind::Function)
            {
                const auto &funcPtr = std::get<std::shared_ptr<FunctionRef>>(data_);
                if (funcPtr)
                {
                    return funcPtr->registeredName;
                }
                return "";
            }
            if (type_ == ::Sad::Types::SadTypeKind::String)
            {
                return std::get<std::string>(data_);
            }
            throwInvalidType("getFunctionName - القيمة ليست دالة أو نص / value is not a function or string");
            return "";
        }

        // ════════════════════════════════════════════════════════════════════════
        // (AR) دالة isCallable — هل القيمة قابلة للاستدعاء كدالة؟
        //      تشمل: FUNCTION + STRING (للتوافق) + OBJECT مع __call__
        //
        // (EN) isCallable — is the value callable as a function?
        //      Includes: FUNCTION + STRING (backward compat) + OBJECT with __call__
        // ════════════════════════════════════════════════════════════════════════
        bool Value::isCallable() const
        {
            if (type_ == ::Sad::Types::SadTypeKind::Function)
                return true;
            if (type_ == ::Sad::Types::SadTypeKind::String)
                return true; // (AR) للتوافق — قد يحمل اسم دالة
            if (type_ == ::Sad::Types::SadTypeKind::Class)
                return true; // (AR) قد يملك __call__
            return false;
        }

        // ========================================
        // Arithmetic Operators / العوامل الحسابية
        // ========================================

        Value Value::operator+(const Value &other) const
        {
            // ═══════════════════════════════════════════════════════════════════
            // (AR) دمج المصفوفات: [1,2] + [3,4] → [1,2,3,4]
            //      إضافة عنصر: [1,2] + 3 → [1,2,3]
            // (EN) Array concatenation: [1,2] + [3,4] → [1,2,3,4]
            //      Element append: [1,2] + 3 → [1,2,3]
            // ═══════════════════════════════════════════════════════════════════
            if (type_ == ::Sad::Types::SadTypeKind::Array)
            {
                // (AR) تحسين أداء: استخدام المرجع + reserve + move
                // (EN) Performance: use ref + reserve + move
                const auto &srcArr = toArrayRef();
                ArrayType result;
                if (other.type_ == ::Sad::Types::SadTypeKind::Array)
                {
                    const auto &otherArr = other.toArrayRef();
                    result.reserve(srcArr.size() + otherArr.size());
                    result.insert(result.end(), srcArr.begin(), srcArr.end());
                    result.insert(result.end(), otherArr.begin(), otherArr.end());
                }
                else
                {
                    result.reserve(srcArr.size() + 1);
                    result.insert(result.end(), srcArr.begin(), srcArr.end());
                    result.push_back(other);
                }
                return Value(std::move(result));
            }
            if (other.type_ == ::Sad::Types::SadTypeKind::Array)
            {
                // (AR) إضافة عنصر في بداية المصفوفة: 0 + [1,2] → [0,1,2]
                const auto &otherArr = other.toArrayRef();
                ArrayType result;
                result.reserve(1 + otherArr.size());
                result.push_back(*this);
                result.insert(result.end(), otherArr.begin(), otherArr.end());
                return Value(std::move(result));
            }

            // (AR) جمع النصوص / (EN) String concatenation
            if (type_ == ::Sad::Types::SadTypeKind::String || other.type_ == ::Sad::Types::SadTypeKind::String)
            {
                // (AR) تحسين أداء: استخدام reserve + append بدلاً من operator+
                // (EN) Performance: use reserve + append instead of operator+
                std::string s1 = toString();
                std::string s2 = other.toString();
                s1.reserve(s1.size() + s2.size());
                s1.append(s2);
                return Value(std::move(s1));
            }

            // (AR) جمع الأعداد مع حماية الطفحان / (EN) Numeric addition with overflow protection
            if (isNumeric() && other.isNumeric())
            {
                if (type_ == ::Sad::Types::SadTypeKind::Float || other.type_ == ::Sad::Types::SadTypeKind::Float)
                {
                    return Value(toDouble() + other.toDouble());
                }
                int a = toInt(), b = other.toInt();
                if (willAddOverflow(a, b))
                {
                    // (AR) ترقية تلقائية إلى عشري عند الطفحان
                    return Value(static_cast<double>(a) + static_cast<double>(b));
                }
                return Value(a + b);
            }

            throwTypeMismatch("addition (+)", other);
            return Value();
        }

        Value Value::operator-(const Value &other) const
        {
            if (isNumeric() && other.isNumeric())
            {
                if (type_ == ::Sad::Types::SadTypeKind::Float || other.type_ == ::Sad::Types::SadTypeKind::Float)
                {
                    return Value(toDouble() - other.toDouble());
                }
                int a = toInt(), b = other.toInt();
                if (willSubOverflow(a, b))
                {
                    return Value(static_cast<double>(a) - static_cast<double>(b));
                }
                return Value(a - b);
            }

            throwTypeMismatch("subtraction (-)", other);
            return Value();
        }

        Value Value::operator*(const Value &other) const
        {
            // ═══════════════════════════════════════════════════════════════════
            // (AR) تكرار المصفوفات: [1,2] * 3 → [1,2,1,2,1,2]
            // (EN) Array repetition: [1,2] * 3 → [1,2,1,2,1,2]
            // ═══════════════════════════════════════════════════════════════════
            if (type_ == ::Sad::Types::SadTypeKind::Array && other.isNumeric())
            {
                ArrayType arr = toArray();
                int count = other.toInt();
                if (count <= 0)
                    return Value(ArrayType{});
                if (count > MAX_REPETITION)
                {
                    throw std::runtime_error("(AR) خطأ: عدد التكرار كبير جداً (" + std::to_string(count) + "). الحد الأقصى: " + std::to_string(MAX_REPETITION) + "\n(EN) Error: Repetition count too large.");
                }
                ArrayType result;
                result.reserve(arr.size() * count);
                for (int i = 0; i < count; ++i)
                    result.insert(result.end(), arr.begin(), arr.end());
                return Value(result);
            }
            if (other.type_ == ::Sad::Types::SadTypeKind::Array && isNumeric())
            {
                ArrayType arr = other.toArray();
                int count = toInt();
                if (count <= 0)
                    return Value(ArrayType{});
                if (count > MAX_REPETITION)
                {
                    throw std::runtime_error("(AR) خطأ: عدد التكرار كبير جداً.\n(EN) Error: Repetition count too large.");
                }
                ArrayType result;
                result.reserve(arr.size() * count);
                for (int i = 0; i < count; ++i)
                    result.insert(result.end(), arr.begin(), arr.end());
                return Value(result);
            }
            // (AR) تكرار النصوص مع حماية الذاكرة
            if (type_ == ::Sad::Types::SadTypeKind::String && other.isNumeric())
            {
                std::string s = toString();
                int count = other.toInt();
                if (count <= 0)
                    return Value(std::string(""));
                if (count > MAX_REPETITION || s.size() * count > MAX_STRING_SIZE)
                {
                    throw std::runtime_error("(AR) خطأ: تكرار النص سينتج نصاً كبيراً جداً.\n(EN) Error: String repetition would produce too large a string.");
                }
                std::string result;
                result.reserve(s.size() * count);
                for (int i = 0; i < count; ++i)
                    result += s;
                return Value(result);
            }
            if (other.type_ == ::Sad::Types::SadTypeKind::String && isNumeric())
            {
                std::string s = other.toString();
                int count = toInt();
                if (count <= 0)
                    return Value(std::string(""));
                if (count > MAX_REPETITION || s.size() * count > MAX_STRING_SIZE)
                {
                    throw std::runtime_error("(AR) خطأ: تكرار النص سينتج نصاً كبيراً جداً.\n(EN) Error: String repetition would produce too large a string.");
                }
                std::string result;
                result.reserve(s.size() * count);
                for (int i = 0; i < count; ++i)
                    result += s;
                return Value(result);
            }

            if (isNumeric() && other.isNumeric())
            {
                if (type_ == ::Sad::Types::SadTypeKind::Float || other.type_ == ::Sad::Types::SadTypeKind::Float)
                {
                    return Value(toDouble() * other.toDouble());
                }
                int a = toInt(), b = other.toInt();
                if (willMulOverflow(a, b))
                {
                    return Value(static_cast<double>(a) * static_cast<double>(b));
                }
                return Value(a * b);
            }

            throwTypeMismatch("multiplication (*)", other);
            return Value();
        }

        Value Value::operator/(const Value &other) const
        {
            if (isNumeric() && other.isNumeric())
            {
                // (AR) التحقق من القسمة على صفر / (EN) Check for division by zero
                if ((other.isInteger() && other.toInt() == 0) ||
                    (other.isDouble() && other.toDouble() == 0.0))
                {
                    throw std::runtime_error(
                        "(AR) خطأ: القسمة على صفر. (EN) Error: Division by zero.");
                }

                if (type_ == ::Sad::Types::SadTypeKind::Float || other.type_ == ::Sad::Types::SadTypeKind::Float)
                {
                    return Value(toDouble() / other.toDouble());
                }
                return Value(toInt() / other.toInt());
            }

            throwTypeMismatch("division (/)", other);
            return Value();
        }

        Value Value::operator%(const Value &other) const
        {
            // (AR) باقي القسمة يعمل مع الأعداد الصحيحة والعشرية
            // (EN) Modulus works with integers and doubles
            if (isInteger() && other.isInteger())
            {
                int otherInt = other.toInt();
                if (otherInt == 0)
                {
                    throw std::runtime_error(
                        "(AR) خطأ: باقي القسمة على صفر. (EN) Error: Modulus by zero.");
                }
                return Value(toInt() % otherInt);
            }

            // (AR) دعم باقي القسمة للأعداد العشرية باستخدام fmod
            // (EN) Support modulus for doubles using fmod
            if (isNumeric() && other.isNumeric())
            {
                double otherD = other.toDouble();
                if (otherD == 0.0)
                {
                    throw std::runtime_error(
                        "(AR) خطأ: باقي القسمة على صفر. (EN) Error: Modulus by zero.");
                }
                return Value(std::fmod(toDouble(), otherD));
            }

            throwTypeMismatch("modulus (%)", other);
            return Value();
        }

        Value Value::operator-() const
        {
            // (AR) العكس الحسابي / (EN) Unary minus
            if (isInteger())
            {
                return Value(-toInt());
            }
            if (isDouble())
            {
                return Value(-toDouble());
            }

            throwInvalidType("unary minus (-)");
            return Value();
        }

        // ========================================
        // Comparison Operators / عوامل المقارنة
        // ========================================

        Value Value::operator==(const Value &other) const
        {
            // (AR) مقارنة الأنواع المختلفة / (EN) Compare different types
            if (type_ != other.type_)
            {
                // (AR) مقارنة رقمية بين int و double / (EN) Numeric comparison
                if (isNumeric() && other.isNumeric())
                {
                    return Value(toDouble() == other.toDouble());
                }
                return Value(false);
            }

            switch (type_)
            {
            case ::Sad::Types::SadTypeKind::Integer:
                return Value(std::get<int64_t>(data_) == std::get<int64_t>(other.data_));

            case ::Sad::Types::SadTypeKind::Float:
                return Value(std::abs(std::get<double>(data_) - std::get<double>(other.data_)) < 1e-10);

            case ::Sad::Types::SadTypeKind::String:
                return Value(std::get<std::string>(data_) == std::get<std::string>(other.data_));

            case ::Sad::Types::SadTypeKind::Boolean:
                return Value(std::get<bool>(data_) == std::get<bool>(other.data_));

            case ::Sad::Types::SadTypeKind::Void:
                return Value(true);

            // (AR) عدم == عدم ⇐ صحيح (قيمة وحيدة) — متمايزة عن فراغ لكن متساوية مع نفسها (S-TS-P1)
            // (EN) null == null ⇒ true (unit value) — distinct from void yet equal to itself (S-TS-P1)
            case Types::SadTypeKind::Null:
                return Value(true);

            case ::Sad::Types::SadTypeKind::Array:
            {
                // (AR) مقارنة عنصرية للمصفوفات — [1,2] == [1,2] ترجع صحيح
                // (EN) Element-wise array comparison — [1,2] == [1,2] returns true
                const auto &arr1 = *std::get<std::shared_ptr<ArrayType>>(data_);
                const auto &arr2 = *std::get<std::shared_ptr<ArrayType>>(other.data_);
                if (arr1.size() != arr2.size())
                    return Value(false);
                for (size_t i = 0; i < arr1.size(); ++i)
                {
                    if (!(arr1[i] == arr2[i]).toBool())
                        return Value(false);
                }
                return Value(true);
            }

            case ::Sad::Types::SadTypeKind::Map:
            {
                // (AR) مقارنة عنصرية للقواميس
                // (EN) Element-wise map comparison
                const auto &map1 = *std::get<std::shared_ptr<MapType>>(data_);
                const auto &map2 = *std::get<std::shared_ptr<MapType>>(other.data_);
                if (map1.size() != map2.size())
                    return Value(false);
                for (const auto &[key, val] : map1)
                {
                    auto it = map2.find(key);
                    if (it == map2.end())
                        return Value(false);
                    if (!(val == it->second).toBool())
                        return Value(false);
                }
                return Value(true);
            }

            case ::Sad::Types::SadTypeKind::Tuple:
            {
                // (AR) مقارنة عنصرية للصفوف — (1، 2) == (1، 2) ترجع صحيح
                // (EN) Element-wise tuple comparison — (1, 2) == (1, 2) returns true
                const auto &tup1 = *std::get<std::shared_ptr<TupleType>>(data_);
                const auto &tup2 = *std::get<std::shared_ptr<TupleType>>(other.data_);
                if (tup1.size() != tup2.size())
                    return Value(false);
                for (size_t i = 0; i < tup1.size(); ++i)
                {
                    if (!(tup1[i] == tup2[i]).toBool())
                        return Value(false);
                }
                return Value(true);
            }

            case ::Sad::Types::SadTypeKind::Class:
            {
                // (AR) مقارنة الكائنات: نقارن بالمرجع (هل هما نفس الكائن؟)
                // (EN) Object comparison: compare by reference (are they the same object?)
                const auto &obj1 = std::get<ObjectInstance *>(data_);
                const auto &obj2 = std::get<ObjectInstance *>(other.data_);
                return Value(obj1 == obj2);
            }

            case ::Sad::Types::SadTypeKind::Function:
            {
                // (AR) مقارنة مراجع الدوال: بالاسم المسجل ونوع الدالة
                // (EN) Function reference comparison: by registered name and kind
                const auto &f1 = std::get<std::shared_ptr<FunctionRef>>(data_);
                const auto &f2 = std::get<std::shared_ptr<FunctionRef>>(other.data_);
                if (f1 && f2)
                {
                    return Value(*f1 == *f2);
                }
                return Value(f1.get() == f2.get());
            }

            default:
                break;
            }

            return Value(false);
        }

        Value Value::operator!=(const Value &other) const
        {
            return Value(!(*this == other).toBool());
        }

        Value Value::operator<(const Value &other) const
        {
            if (isNumeric() && other.isNumeric())
            {
                return Value(toDouble() < other.toDouble());
            }

            if (isString() && other.isString())
            {
                return Value(std::get<std::string>(data_) < std::get<std::string>(other.data_));
            }

            throwTypeMismatch("less than (<)", other);
            return Value(false);
        }

        Value Value::operator>(const Value &other) const
        {
            if (isNumeric() && other.isNumeric())
            {
                return Value(toDouble() > other.toDouble());
            }

            if (isString() && other.isString())
            {
                return Value(std::get<std::string>(data_) > std::get<std::string>(other.data_));
            }

            throwTypeMismatch("greater than (>)", other);
            return Value(false);
        }

        Value Value::operator<=(const Value &other) const
        {
            // (AR) تحسين الأداء: تجنب التقييم المزدوج
            // (EN) Performance: avoid double evaluation
            if (isNumeric() && other.isNumeric())
            {
                return Value(toDouble() <= other.toDouble());
            }
            if (isString() && other.isString())
            {
                return Value(std::get<std::string>(data_) <= std::get<std::string>(other.data_));
            }
            return Value((*this < other).toBool() || (*this == other).toBool());
        }

        Value Value::operator>=(const Value &other) const
        {
            if (isNumeric() && other.isNumeric())
            {
                return Value(toDouble() >= other.toDouble());
            }
            if (isString() && other.isString())
            {
                return Value(std::get<std::string>(data_) >= std::get<std::string>(other.data_));
            }
            return Value((*this > other).toBool() || (*this == other).toBool());
        }

        // ========================================
        // Logical Operators / العوامل المنطقية
        // ========================================

        Value Value::operator&&(const Value &other) const
        {
            return Value(toBool() && other.toBool());
        }

        Value Value::operator||(const Value &other) const
        {
            return Value(toBool() || other.toBool());
        }

        Value Value::operator!() const
        {
            return Value(!toBool());
        }

        // ========================================
        // Increment/Decrement / الزيادة/النقصان
        // ========================================

        Value &Value::operator++()
        {
            // (AR) زيادة قبلية / (EN) Pre-increment
            if (isInteger())
            {
                data_ = std::get<int64_t>(data_) + 1;
            }
            else if (isDouble())
            {
                data_ = std::get<double>(data_) + 1.0;
            }
            else
            {
                throwInvalidType("pre-increment (++)");
            }
            return *this;
        }

        Value Value::operator++(int)
        {
            // (AR) زيادة بعدية / (EN) Post-increment
            Value temp = *this;
            ++(*this);
            return temp;
        }

        Value &Value::operator--()
        {
            // (AR) نقصان قبلي / (EN) Pre-decrement
            if (isInteger())
            {
                data_ = std::get<int64_t>(data_) - 1;
            }
            else if (isDouble())
            {
                data_ = std::get<double>(data_) - 1.0;
            }
            else
            {
                throwInvalidType("pre-decrement (--)");
            }
            return *this;
        }

        Value Value::operator--(int)
        {
            // (AR) نقصان بعدي / (EN) Post-decrement
            Value temp = *this;
            --(*this);
            return temp;
        }

        // ========================================
        // Compound Assignment / التعيين المركب
        // ========================================

        Value &Value::operator+=(const Value &other)
        {
            *this = *this + other;
            return *this;
        }

        Value &Value::operator-=(const Value &other)
        {
            *this = *this - other;
            return *this;
        }

        Value &Value::operator*=(const Value &other)
        {
            *this = *this * other;
            return *this;
        }

        Value &Value::operator/=(const Value &other)
        {
            *this = *this / other;
            return *this;
        }

        // ========================================
        // Debug Methods / دوال التصحيح
        // ========================================

        std::string Value::debugString() const
        {
            std::ostringstream oss;
            oss << "Type: " << getTypeName() << ", Value: " << toString();
            return oss.str();
        }

        std::string Value::getTypeName() const
        {
            switch (type_)
            {
            case ::Sad::Types::SadTypeKind::Void:
                return "VOID";
            case Types::SadTypeKind::Null:
                return "NULL";
            case ::Sad::Types::SadTypeKind::Integer:
                return "INTEGER";
            case ::Sad::Types::SadTypeKind::Float:
                return "DOUBLE";
            case ::Sad::Types::SadTypeKind::String:
                return "STRING";
            case ::Sad::Types::SadTypeKind::Boolean:
                return "BOOLEAN";
            case ::Sad::Types::SadTypeKind::Array:
                return "ARRAY";
            case ::Sad::Types::SadTypeKind::Map:
                return "MAP";
            case ::Sad::Types::SadTypeKind::Tuple:
                return "TUPLE";
            case ::Sad::Types::SadTypeKind::Class:
            {
                // (AR) للكائنات: نُرجع "OBJECT:اسم_الصنف" للتوضيح
                // (EN) For objects: return "OBJECT:ClassName" for clarity
                const auto &objPtr = std::get<ObjectInstance *>(data_);
                if (objPtr)
                {
                    return "OBJECT:" + objPtr->getClassName();
                }
                return "OBJECT";
            }
            case ::Sad::Types::SadTypeKind::Function:
            {
                // (AR) للدوال: نُرجع "FUNCTION:نوع_الدالة" للتوضيح
                // (EN) For functions: return "FUNCTION:kind" for clarity
                const auto &funcPtr = std::get<std::shared_ptr<FunctionRef>>(data_);
                if (funcPtr)
                {
                    return "FUNCTION:" + funcPtr->getKindNameEn();
                }
                return "FUNCTION";
            }
            }
            return "UNKNOWN";
        }

        // ========================================
        // Private Helper Methods / دوال مساعدة خاصة
        // ========================================

        // ========================================
        // Array/Map Operations / عمليات المصفوفات والقواميس
        // ========================================

        Value &Value::operator[](size_t index)
        {
            // (AR) الوصول للعنصر حسب الفهرس / (EN) Access element by index
            if (type_ != ::Sad::Types::SadTypeKind::Array)
            {
                throwInvalidType("array indexing [size_t]");
            }

            auto &arr = *std::get<std::shared_ptr<ArrayType>>(data_);
            if (index >= arr.size())
            {
                std::ostringstream oss;
                oss << "(AR) فهرس خارج النطاق: " << index << " >= " << arr.size() << ".\n"
                    << "(EN) Index out of range: " << index << " >= " << arr.size() << ".";
                throw std::out_of_range(oss.str());
            }

            return arr[index];
        }

        const Value &Value::operator[](size_t index) const
        {
            if (type_ != ::Sad::Types::SadTypeKind::Array)
            {
                throwInvalidType("array indexing [size_t] const");
            }

            const auto &arr = *std::get<std::shared_ptr<ArrayType>>(data_);
            if (index >= arr.size())
            {
                std::ostringstream oss;
                oss << "(AR) فهرس خارج النطاق: " << index << " >= " << arr.size() << ".\n"
                    << "(EN) Index out of range: " << index << " >= " << arr.size() << ".";
                throw std::out_of_range(oss.str());
            }

            return arr[index];
        }

        Value &Value::operator[](const std::string &key)
        {
            // (AR) الوصول للعنصر حسب المفتاح / (EN) Access element by key
            if (type_ != ::Sad::Types::SadTypeKind::Map)
            {
                throwInvalidType("map indexing [string]");
            }

            auto &map = *std::get<std::shared_ptr<MapType>>(data_);
            return map[key]; // Creates if doesn't exist
        }

        const Value &Value::operator[](const std::string &key) const
        {
            if (type_ != ::Sad::Types::SadTypeKind::Map)
            {
                throwInvalidType("map indexing [string] const");
            }

            const auto &map = *std::get<std::shared_ptr<MapType>>(data_);
            auto it = map.find(key);
            if (it == map.end())
            {
                std::ostringstream oss;
                oss << "(AR) المفتاح غير موجود: '" << key << "'.\n"
                    << "(EN) Key not found: '" << key << "'.";
                throw std::out_of_range(oss.str());
            }

            return it->second;
        }

        // (AR) قراءة حقل كائن باسمه دون رمي (إصلاح ISSUE-034).
        //      operator[] يخدم الخرائط فقط ويرمي على الكائنات؛ هذه الدالة تصل
        //      إلى ObjectInstance مباشرةً (متاح هنا، بخلاف رأس AST).
        // (EN) Non-throwing object field lookup; reaches ObjectInstance directly.
        const Value *Value::tryGetField(const std::string &fieldName) const
        {
            if (!isObject())
            {
                return nullptr;
            }
            ObjectPtr obj = toObject();
            if (obj == nullptr || !obj->hasField(fieldName))
            {
                return nullptr;
            }
            return obj->getField(fieldName);
        }

        size_t Value::size() const
        {
            // (AR) حجم المصفوفة أو القاموس / (EN) Size of array or map
            switch (type_)
            {
            case ::Sad::Types::SadTypeKind::Array:
                return std::get<std::shared_ptr<ArrayType>>(data_)->size();

            case ::Sad::Types::SadTypeKind::Map:
                return std::get<std::shared_ptr<MapType>>(data_)->size();

            case ::Sad::Types::SadTypeKind::Tuple:
                return std::get<std::shared_ptr<TupleType>>(data_)->size();

            case ::Sad::Types::SadTypeKind::String:
                // (AR) إرجاع عدد الأحرف وليس البايتات — مهم للنصوص العربية
                // (EN) Return character count not byte count — important for Arabic text
                return utf8CharCount(std::get<std::string>(data_));

            default:
                throwInvalidType("size()");
                return 0;
            }
        }

        bool Value::hasKey(const std::string &key) const
        {
            // (AR) التحقق من وجود مفتاح / (EN) Check key existence
            if (type_ != ::Sad::Types::SadTypeKind::Map)
            {
                throwInvalidType("hasKey()");
            }

            const auto &map = *std::get<std::shared_ptr<MapType>>(data_);
            return map.find(key) != map.end();
        }

        void Value::push(const Value &val)
        {
            // (AR) إضافة عنصر لنهاية المصفوفة / (EN) Append to array
            if (type_ != ::Sad::Types::SadTypeKind::Array)
            {
                throwInvalidType("push()");
            }

            auto &arr = *std::get<std::shared_ptr<ArrayType>>(data_);
            arr.push_back(val);
        }

        Value Value::pop()
        {
            // (AR) حذف وإرجاع آخر عنصر / (EN) Remove and return last element
            if (type_ != ::Sad::Types::SadTypeKind::Array)
            {
                throwInvalidType("pop()");
            }

            auto &arr = *std::get<std::shared_ptr<ArrayType>>(data_);
            if (arr.empty())
            {
                std::ostringstream oss;
                oss << "(AR) لا يمكن pop من مصفوفة فارغة.\n"
                    << "(EN) Cannot pop from empty array.";
                throw std::runtime_error(oss.str());
            }

            Value val = arr.back();
            arr.pop_back();
            return val;
        }

        bool Value::remove(const std::string &key)
        {
            // (AR) حذف عنصر من القاموس / (EN) Remove element from map
            if (type_ != ::Sad::Types::SadTypeKind::Map)
            {
                throwInvalidType("remove()");
            }

            auto &map = *std::get<std::shared_ptr<MapType>>(data_);
            return map.erase(key) > 0;
        }

        void Value::remove(size_t index)
        {
            // (AR) حذف عنصر من المصفوفة بالفهرس / (EN) Remove element from array by index
            if (type_ != ::Sad::Types::SadTypeKind::Array)
            {
                throwInvalidType("remove()");
            }

            auto &arr = *std::get<std::shared_ptr<ArrayType>>(data_);
            if (index >= arr.size())
            {
                std::ostringstream oss;
                oss << "(AR) الفهرس " << index << " خارج حدود المصفوفة (الحجم: " << arr.size() << ").\n"
                    << "(EN) Index " << index << " out of bounds (size: " << arr.size() << ").";
                throw std::runtime_error(oss.str());
            }
            arr.erase(arr.begin() + static_cast<std::ptrdiff_t>(index));
        }

        void Value::clear()
        {
            // (AR) مسح جميع العناصر / (EN) Clear all elements
            switch (type_)
            {
            case ::Sad::Types::SadTypeKind::Array:
                std::get<std::shared_ptr<ArrayType>>(data_)->clear();
                break;

            case ::Sad::Types::SadTypeKind::Map:
                std::get<std::shared_ptr<MapType>>(data_)->clear();
                break;

            default:
                throwInvalidType("clear()");
            }
        }

        bool Value::isEmpty() const
        {
            // (AR) التحقق من الفراغ / (EN) Check if empty
            switch (type_)
            {
            case ::Sad::Types::SadTypeKind::Array:
                return std::get<std::shared_ptr<ArrayType>>(data_)->empty();

            case ::Sad::Types::SadTypeKind::Map:
                return std::get<std::shared_ptr<MapType>>(data_)->empty();

            case ::Sad::Types::SadTypeKind::Tuple:
                return std::get<std::shared_ptr<TupleType>>(data_)->empty();

            case ::Sad::Types::SadTypeKind::String:
                return std::get<std::string>(data_).empty();

            default:
                throwInvalidType("isEmpty()");
                return true;
            }
        }

        std::vector<std::string> Value::keys() const
        {
            // (AR) الحصول على جميع المفاتيح / (EN) Get all keys
            if (type_ != ::Sad::Types::SadTypeKind::Map)
            {
                throwInvalidType("keys()");
            }

            const auto &map = *std::get<std::shared_ptr<MapType>>(data_);
            std::vector<std::string> result;
            result.reserve(map.size());

            for (const auto &pair : map)
            {
                result.push_back(pair.first);
            }

            return result;
        }

        std::vector<Value> Value::values() const
        {
            // (AR) الحصول على جميع القيم / (EN) Get all values
            if (type_ != ::Sad::Types::SadTypeKind::Map)
            {
                throwInvalidType("values()");
            }

            const auto &map = *std::get<std::shared_ptr<MapType>>(data_);
            std::vector<Value> result;
            result.reserve(map.size());

            for (const auto &pair : map)
            {
                result.push_back(pair.second);
            }

            return result;
        }

        // ========================================
        // Private Helper Methods / دوال مساعدة خاصة
        // ========================================

        void Value::throwTypeMismatch(const std::string &operation, const Value &other) const
        {
            std::ostringstream oss;
            oss << "(AR) خطأ في الأنواع: لا يمكن تطبيق " << operation
                << " على " << getTypeName() << " و " << other.getTypeName() << ".\n"
                << "(EN) Type mismatch: Cannot apply " << operation
                << " to " << getTypeName() << " and " << other.getTypeName() << ".";
            throw std::runtime_error(oss.str());
        }

        void Value::throwInvalidType(const std::string &operation) const
        {
            std::ostringstream oss;
            oss << "(AR) نوع غير صالح: لا يمكن تطبيق " << operation
                << " على " << getTypeName() << ".\n"
                << "(EN) Invalid type: Cannot apply " << operation
                << " to " << getTypeName() << ".";
            throw std::runtime_error(oss.str());
        }

        // ========================================
        // (AR) ADR-01 Phase 4 — اختصار getKind() / (EN) ADR-01 Phase 4 — getKind() shortcut
        // ========================================

        Types::SadTypeKind Value::getKind() const
        {
            // (AR) إذا كان النوع الغني موجوداً، نرجع نوعه مباشرة
            // (EN) If rich type exists, return its kind directly
            if (sadType_)
            {
                return sadType_->getKind();
            }
            return type_;
        }

        // ========================================
        // (AR) ADR-01 Phase 3 — تعيين النوع الغني / (EN) ADR-01 Phase 3 — Set rich type
        // ========================================

        void Value::setSadType(Types::SadTypePtr t)
        {
            sadType_ = std::move(t);
            // (AR) تحديث cache الـ type_ من sadType_
            // (EN) Update type_ cache from sadType_
            if (!sadType_)
            {
                type_ = ::Sad::Types::SadTypeKind::Void;
                return;
            }
            switch (sadType_->getKind())
            {
            case SadTypeKind::Void:
                type_ = ::Sad::Types::SadTypeKind::Void;
                break;
            case SadTypeKind::Integer:
                type_ = ::Sad::Types::SadTypeKind::Integer;
                break;
            case SadTypeKind::Float:
                type_ = ::Sad::Types::SadTypeKind::Float;
                break;
            case SadTypeKind::String:
                type_ = ::Sad::Types::SadTypeKind::String;
                break;
            case SadTypeKind::Boolean:
                type_ = ::Sad::Types::SadTypeKind::Boolean;
                break;
            case SadTypeKind::Array:
                type_ = ::Sad::Types::SadTypeKind::Array;
                break;
            case SadTypeKind::Map:
                type_ = ::Sad::Types::SadTypeKind::Map;
                break;
            case SadTypeKind::Tuple:
                type_ = ::Sad::Types::SadTypeKind::Tuple;
                break;
            case SadTypeKind::Class:
            case SadTypeKind::Struct:
                type_ = ::Sad::Types::SadTypeKind::Class;
                break;
            case SadTypeKind::Function:
            case SadTypeKind::Closure:
                type_ = ::Sad::Types::SadTypeKind::Function;
                break;
            // (AR) [S-TS-P1] عدم: نحفظ تمايزه (لا نُسقطه إلى فراغ).
            case SadTypeKind::Null:
                type_ = ::Sad::Types::SadTypeKind::Null;
                break;
            // (AR) [S-TS-P4] Future/Generator مُمثَّلان ككائن في الـruntime: نُبقي type_=OBJECT
            //      (ليعمل isObject() وفحص الطرق .احصل())، بينما sadType_ يحمل النوع الحقيقي
            //      فيُرجع getKind()=Future/Generator وتُعرّبه نوع() صحيحًا.
            // (EN) [S-TS-P4] Future/Generator are object-represented at runtime: keep
            //      type_=OBJECT so isObject()/method dispatch works, while sadType_ carries
            //      the real kind (getKind()=Future/Generator → نوع() reports it correctly).
            case SadTypeKind::Future:
            case SadTypeKind::Generator:
                type_ = ::Sad::Types::SadTypeKind::Class;
                break;
            default:
                type_ = ::Sad::Types::SadTypeKind::Void;
                break;
            }
        }

    } // namespace Data
} // namespace Sad
