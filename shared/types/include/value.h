/**
 * @file value.h
 * @brief (AR) فئة القيمة في وقت التشغيل - تمثل أي قيمة أثناء تنفيذ البرنامج
 * @brief (EN) Runtime value class - represents any value during program execution
 *
 * ═══════════════════════════════════════════════════════════════════════════
 * (AR) هذا الملف يعرّف فئة Value الأساسية في نظام أنواع لغة ص
 *      تدعم الأنواع البدائية (رقم صحيح، عشري، نص، منطقي)
 *      والأنواع المركبة (مصفوفة، قاموس، كائن)
 *
 *      نوع OBJECT يمثل كائنات الأصناف الحقيقية ويحمل مؤشراً مشتركاً
 *      إلى ObjectInstance، مما يسمح بتمرير الكائنات كمعاملات للدوال
 *      بالمرجع (shared_ptr) بدلاً من النسخ
 *
 * (EN) This file defines the core Value class in S language type system
 *      Supports primitive types (int, double, string, boolean)
 *      and composite types (array, map, object)
 *
 *      The OBJECT type represents real class objects and holds a shared_ptr
 *      to ObjectInstance, enabling passing objects as function parameters
 *      by reference (shared_ptr) instead of copying
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * @author S Language Development Team
 * @date November 21, 2025 - يناير 2026
 * @version 2.0 — إضافة دعم الكائنات كنوع أصلي
 */

#pragma once

#include <variant>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <sstream>
#include <functional>
#include "function_ref.h"
#include "sad_type_system.h"

namespace Sad
{
    namespace Data
    {

        // ═══════════════════════════════════════════════════════════════════════════
        // (AR) إعلان مسبق لـ ObjectInstance لتجنب التبعية الدائرية
        //      value.h لا يشمل object_instance.h مباشرة لأن object_instance.h
        //      يشمل value.h بالفعل. نستخدم shared_ptr مع إعلان مسبق وهذا
        //      يعمل لأن shared_ptr لا يحتاج التعريف الكامل عند الإعلان
        //
        // (EN) Forward declaration of ObjectInstance to avoid circular dependency
        //      value.h doesn't include object_instance.h directly because
        //      object_instance.h already includes value.h. We use shared_ptr
        //      with forward declaration which works because shared_ptr doesn't
        //      need the complete definition at declaration point
        // ═══════════════════════════════════════════════════════════════════════════
        class ObjectInstance;

/**
 * @brief (AR) أنواع القيم المدعومة في وقت التشغيل
 * @brief (EN) Supported runtime value types
 *
 * (AR) ملاحظات مهمة:
 *   - OBJECT هو النوع الجديد لتمثيل كائنات الأصناف
 *   - الكائنات تُخزّن كـ shared_ptr<ObjectInstance> مما يسمح بتمريرها بالمرجع
 *   - MAP لا يزال مدعوماً للتوافق مع الكود القديم
 *   - عند إنشاء كائن بـ 'جديد' أو باستدعاء اسم الصنف، يُنشأ OBJECT وليس MAP
 *
 * (EN) Important notes:
 *   - OBJECT is the new type for representing class instances
 *   - Objects are stored as shared_ptr<ObjectInstance> enabling pass-by-reference
 *   - MAP is still supported for backward compatibility
 *   - When creating an object with 'new' or class name call, OBJECT is created not MAP
 */
// Windows.h defines VOID as 'void' macro — undefine to avoid conflict
// ويندوز يعرّف VOID كماكرو؛ إلغاء التعريف لتجنب التعارض
#ifdef VOID
#undef VOID
#endif

        namespace ValueType
        {
            // (AR) ثوابت توافقية فقط: تُبقي الصيغة القديمة ValueType::X
            //      لكن النوع الفعلي في النظام أصبح SadTypeKind مباشرة.
            // (EN) Compatibility constants only: preserve legacy ValueType::X
            //      syntax while the actual runtime type is now SadTypeKind.
            static constexpr Types::SadTypeKind VOID = Types::SadTypeKind::Void;
            static constexpr Types::SadTypeKind INTEGER = Types::SadTypeKind::Integer;
            static constexpr Types::SadTypeKind DOUBLE = Types::SadTypeKind::Float;
            static constexpr Types::SadTypeKind STRING = Types::SadTypeKind::String;
            static constexpr Types::SadTypeKind BOOLEAN = Types::SadTypeKind::Boolean;
            static constexpr Types::SadTypeKind ARRAY = Types::SadTypeKind::Array;
            static constexpr Types::SadTypeKind MAP = Types::SadTypeKind::Map;
            static constexpr Types::SadTypeKind TUPLE = Types::SadTypeKind::Tuple;
            static constexpr Types::SadTypeKind OBJECT = Types::SadTypeKind::Class;
            static constexpr Types::SadTypeKind FUNCTION = Types::SadTypeKind::Function;
        };

        /**
         * @brief (AR) فئة تمثل قيمة في وقت التشغيل
         * @brief (EN) Runtime value class
         *
         * (AR) الفئة الأساسية لجميع القيم في لغة ص أثناء التنفيذ
         *      تدعم الأنواع البدائية والمركبة بما في ذلك الكائنات
         *
         *      الكائنات (OBJECT) تُخزّن كـ shared_ptr مما يعني:
         *      - تمرير الكائنات للدوال يكون بالمرجع وليس بالنسخ
         *      - عدة متغيرات يمكن أن تشير لنفس الكائن
         *      - عد المراجع التلقائي (reference counting) لإدارة الذاكرة
         *
         * (EN) Core class for all runtime values in S language
         *      Supports primitive and composite types including objects
         *
         *      Objects (OBJECT) are stored as shared_ptr which means:
         *      - Passing objects to functions is by reference not copy
         *      - Multiple variables can point to the same object
         *      - Automatic reference counting for memory management
         */
        class Value
        {
        public:
            // ══════════════════════════════════════════════════════════════════
            // (AR) تعريفات الأنواع المركبة / (EN) Composite type definitions
            // ══════════════════════════════════════════════════════════════════
            using ArrayType = std::vector<Value>;
            using MapType = std::unordered_map<std::string, Value>;

            /// (AR) بنية الصف — تغلف vector<Value> لجعلها نوعاً مميزاً عن ArrayType في variant
            /// (EN) Tuple struct — wraps vector<Value> to make it a distinct type from ArrayType in variant
            struct TupleType
            {
                std::vector<Value> elements;

                TupleType() = default;
                TupleType(const std::vector<Value> &elems) : elements(elems) {}
                TupleType(std::vector<Value> &&elems) : elements(std::move(elems)) {}

                size_t size() const { return elements.size(); }
                bool empty() const { return elements.empty(); }
                void reserve(size_t n) { elements.reserve(n); }
                void push_back(const Value &v) { elements.push_back(v); }
                void push_back(Value &&v) { elements.push_back(std::move(v)); }
                const Value &operator[](size_t i) const { return elements[i]; }
                Value &operator[](size_t i) { return elements[i]; }
                auto begin() const { return elements.begin(); }
                auto end() const { return elements.end(); }
                auto begin() { return elements.begin(); }
                auto end() { return elements.end(); }

                bool operator==(const TupleType &other) const;
            };

            // (AR) B-step5b: ObjectPtr هو الآن مؤشر خام تديره GC.
            //      دورة حياة ObjectInstance يديرها محرّك GC الموحَّد عبر
            //      destroyer-callback المسجَّل في Value::Value(ObjectPtr).
            //      Pin<ObjectInstance> يُستخدم لتثبيت الكائن كجذر عند الحاجة.
            // (EN) B-step5b: ObjectPtr is now a raw pointer managed by GC.
            //      ObjectInstance lifetime is owned by the unified GC engine via
            //      a destroyer-callback registered in Value::Value(ObjectPtr).
            //      Use Pin<ObjectInstance> to root an object explicitly.
            using ObjectPtr = ObjectInstance *;                  ///< (AR) مؤشر خام لكائن مُدار بـGC / (EN) Raw GC-managed pointer
            using FunctionRefPtr = std::shared_ptr<FunctionRef>; ///< (AR) مؤشر مشترك لمرجع دالة / (EN) Shared pointer to function reference

            // ══════════════════════════════════════════════════════════════════
            // (AR) المنشئات — لكل نوع من أنواع القيم منشئ خاص
            // (EN) Constructors — each value type has its own constructor
            // ══════════════════════════════════════════════════════════════════
            Value();                              ///< (AR) إنشاء قيمة فارغة VOID / (EN) Create VOID value
            explicit Value(int val);              ///< (AR) إنشاء قيمة رقم صحيح / (EN) Create INTEGER value (32-bit, auto-promoted to 64-bit)
            explicit Value(int64_t val);          ///< (AR) إنشاء قيمة رقم صحيح 64-bit / (EN) Create INTEGER value (64-bit)
            explicit Value(double val);           ///< (AR) إنشاء قيمة رقم عشري / (EN) Create DOUBLE value
            Value(const std::string &val);        ///< (AR) إنشاء قيمة نصية / (EN) Create STRING value (non-explicit for literals)
            Value(std::string &&val);             ///< (AR) إنشاء قيمة نصية بنقل / (EN) Create STRING value (move)
            Value(const char *val);               ///< (AR) إنشاء قيمة نصية من C-string / (EN) Create STRING from C-string
            explicit Value(bool val);             ///< (AR) إنشاء قيمة منطقية / (EN) Create BOOLEAN value
            explicit Value(const ArrayType &val); ///< (AR) إنشاء مصفوفة / (EN) Create ARRAY value
            explicit Value(ArrayType &&val);      ///< (AR) إنشاء مصفوفة بنقل / (EN) Create ARRAY value (move)
            explicit Value(const MapType &val);   ///< (AR) إنشاء قاموس / (EN) Create MAP value
            explicit Value(MapType &&val);        ///< (AR) إنشاء قاموس بنقل / (EN) Create MAP value (move)

            // (AR) منشئات الصف — لإنشاء صف (tuple) من مصفوفة قيم
            // (EN) Tuple constructors — create tuple from value vector
            struct TupleTag
            {
            }; ///< (AR) وسم للتمييز بين منشئ الصف والمصفوفة / (EN) Tag to distinguish tuple from array constructor
            Value(TupleTag, const TupleType &val); ///< (AR) إنشاء صف / (EN) Create TUPLE value
            Value(TupleTag, TupleType &&val);      ///< (AR) إنشاء صف بنقل / (EN) Create TUPLE value (move)

            /**
             * @brief (AR) إنشاء قيمة كائن من مؤشر مشترك لـ ObjectInstance
             * @brief (EN) Create OBJECT value from shared_ptr to ObjectInstance
             *
             * (AR) هذا المنشئ يُستخدم عند إنشاء كائن جديد بكلمة 'جديد' أو باستدعاء
             *      اسم الصنف مباشرة. الكائن يُخزّن كمؤشر مشترك مما يعني أن
             *      التمرير للدوال يكون بالمرجع تلقائياً
             *
             * (EN) This constructor is used when creating a new object with 'new' keyword
             *      or by calling the class name directly. The object is stored as a
             *      shared_ptr meaning passing to functions is by reference automatically
             *
             * @param obj (ObjectPtr) — (AR) مؤشر مشترك للكائن / (EN) shared pointer to object
             */
            explicit Value(ObjectPtr obj);

            /**
             * @brief (AR) إنشاء قيمة مرجع دالة من مؤشر FunctionRef
             * @brief (EN) Create FUNCTION value from FunctionRef pointer
             *
             * (AR) يُستخدم لتمثيل الدوال كقيم من الدرجة الأولى.
             *      يسمح بتمرير الدوال كمعاملات وتخزينها في متغيرات
             *
             * (EN) Used to represent functions as first-class values.
             *      Allows passing functions as arguments and storing in variables
             *
             * @param funcRef (FunctionRefPtr) — (AR) مؤشر مشترك لمرجع الدالة / (EN) shared pointer to function reference
             */
            explicit Value(FunctionRefPtr funcRef);

            /**
             * @brief (AR) إنشاء قيمة مرجع دالة من FunctionRef مباشرة (ينسخ إلى shared_ptr)
             * @brief (EN) Create FUNCTION value from FunctionRef directly (copies to shared_ptr)
             */
            explicit Value(const FunctionRef &funcRef);

            // ══════════════════════════════════════════════════════════════════
            // (AR) النسخ العميق / (EN) Deep clone
            // ══════════════════════════════════════════════════════════════════
            Value clone() const;

            // ══════════════════════════════════════════════════════════════════
            // (AR) فحص النوع — دوال للتحقق من نوع القيمة الحالية
            // (EN) Type checking — functions to check the current value type
            // ══════════════════════════════════════════════════════════════════
            Types::SadTypeKind getType() const { return type_; }

            /**
             * @brief (AR) الحصول على النوع الغني الكامل (ADR-01 Phase 3)
             * @brief (EN) Get the rich full type (ADR-01 Phase 3)
             */
            const Types::SadTypePtr &getSadType() const { return sadType_; }

            /**
             * @brief (AR) الحصول على نوع SadTypeKind مباشرة — اختصار لـ getSadType()->getKind()
             * @brief (EN) Get SadTypeKind directly — shortcut for getSadType()->getKind()
             */
            Types::SadTypeKind getKind() const;

            /**
             * @brief (AR) تعيين النوع الغني مباشرة (يُحدّث type_ تلقائياً)
             * @brief (EN) Set the rich type directly (auto-updates type_ cache)
             */
            void setSadType(Types::SadTypePtr t);

            bool isVoid() const { return type_ == ValueType::VOID; }
            bool isInteger() const { return type_ == ValueType::INTEGER; }
            bool isDouble() const { return type_ == ValueType::DOUBLE; }
            bool isString() const { return type_ == ValueType::STRING; }
            bool isBoolean() const { return type_ == ValueType::BOOLEAN; }
            bool isArray() const { return type_ == ValueType::ARRAY; }
            bool isTuple() const { return type_ == ValueType::TUPLE; }
            bool isMap() const { return type_ == ValueType::MAP; }

            /**
             * @brief (AR) هل القيمة كائن (نسخة من صنف)؟
             * @brief (EN) Is the value an object (class instance)?
             *
             * (AR) يُرجع true إذا كانت القيمة كائناً حقيقياً من نوع OBJECT
             *      وليس مجرد MAP يحتوي على __class__
             * (EN) Returns true if the value is a real OBJECT type
             *      not just a MAP containing __class__
             */
            bool isObject() const { return type_ == ValueType::OBJECT; }

            /**
             * @brief (AR) هل القيمة مرجع دالة (دالة من الدرجة الأولى)؟
             * @brief (EN) Is the value a function reference (first-class function)?
             *
             * (AR) تشمل: دوال المستخدم، الدوال المدمجة، اللامبدا، الطرق
             * (EN) Includes: user functions, built-in functions, lambdas, methods
             */
            bool isFunction() const { return type_ == ValueType::FUNCTION; }

            /**
             * @brief (AR) هل القيمة قابلة للاستدعاء كدالة؟
             * @brief (EN) Is the value callable as a function?
             *
             * (AR) تشمل FUNCTION بالإضافة إلى STRING التي قد تحمل اسم دالة (توافق)
             *      وOBJECT الذي يملك __call__
             * (EN) Includes FUNCTION plus STRING that may hold function name (backward compat)
             *      and OBJECT with __call__
             */
            bool isCallable() const;

            /**
             * @brief (AR) هل القيمة تمثل كائناً (سواء OBJECT أو MAP بـ __class__)؟
             * @brief (EN) Does the value represent an object (either OBJECT or MAP with __class__)?
             *
             * (AR) للتوافق مع الكود القديم: يكشف الكائنات سواء كانت OBJECT حقيقي
             *      أو MAP قديم يحتوي على مفتاح __class__
             * (EN) For backward compatibility: detects objects whether real OBJECT type
             *      or legacy MAP containing __class__ key
             */
            bool isObjectLike() const;

            bool isNumeric() const { return isInteger() || isDouble(); }
            bool isContainer() const { return isArray() || isMap() || isTuple(); }
            bool isFunctionOrString() const { return isFunction() || isString(); } ///< (AR) دالة أو نص — للتوافق الخلفي / (EN) Function or string — backward compat

            // ══════════════════════════════════════════════════════════════════
            // (AR) تحويل النوع — تحويل القيمة إلى نوع محدد
            // (EN) Type conversion — convert value to a specific type
            // ══════════════════════════════════════════════════════════════════
            int toInt() const;
            int64_t toInt64() const;
            double toDouble() const;
            std::string toString() const;
            bool toBool() const;
            ArrayType toArray() const;
            MapType toMap() const;
            TupleType toTuple() const; ///< (AR) تحويل إلى صف / (EN) Convert to tuple

            // (AR) إصدارات مرجعية ثابتة — تتجنب النسخ عند الحاجة للقراءة فقط
            // (EN) Const reference versions — avoid copying when only reading
            const ArrayType &toArrayRef() const;
            const MapType &toMapRef() const;
            const TupleType &toTupleRef() const; ///< (AR) مرجع ثابت للصف / (EN) Const reference to tuple

            /**
             * @brief (AR) إرجاع مرجع ثابت للنص — يتجنب نسخ النص عند القراءة فقط
             * @brief (EN) Return const reference to string — avoids string copy when only reading
             *
             * (AR) يعمل فقط مع قيم من نوع STRING. للأنواع الأخرى استخدم toString()
             * (EN) Only works with STRING values. For other types use toString()
             *
             * @throws std::bad_variant_access (AR) إذا لم يكن النوع STRING / (EN) if type is not STRING
             */
            const std::string &toStringRef() const;

            // (AR) إصدارات مرجعية قابلة للتعديل — تعديل المصفوفة/الخريطة مباشرة بدون نسخ
            // (EN) Mutable reference versions — modify array/map in-place without copying
            ArrayType &toArrayMut();
            MapType &toMapMut();

            /**
             * @brief (AR) الحصول على مؤشر الكائن
             * @brief (EN) Get object pointer
             *
             * (AR) يُرجع المؤشر المشترك للكائن. يجب أن تكون القيمة من نوع OBJECT
             *      وإلا سيُطلق استثناء. استخدم isObject() للتحقق أولاً
             *
             * (EN) Returns the shared pointer to the object. Value must be OBJECT type
             *      or an exception is thrown. Use isObject() to check first
             *
             * @return (ObjectPtr) — (AR) مؤشر مشترك للكائن / (EN) shared pointer to object
             * @throws std::runtime_error (AR) إذا لم تكن القيمة كائناً / (EN) if value is not an object
             */
            ObjectPtr toObject() const;

            // ─────────────────────────────────────────────────────────────────
            // (AR) تعداد جذور الكائنات (B-step5b-iii)
            //   تستخدم لإصدار جميع مؤشرات ObjectInstance المرتبطة بهذه القيمة
            //   إلى GC أثناء mark phase. تُغطّي:
            //     OBJECT  → المؤشر مباشرة
            //     ARRAY   → استدعاء عميق على كل عنصر
            //     MAP     → استدعاء عميق على كل قيمة
            //   الأنواع الأخرى (سكلر، نص، دالة...) → لا شيء.
            //
            // (EN) Enumerate all ObjectInstance roots reachable from this value:
            //     OBJECT → emit pointer; ARRAY/MAP → recurse into elements/values.
            // ─────────────────────────────────────────────────────────────────
            void forEachObjectRef(const std::function<void(ObjectInstance *)> &emit) const;


            /**
             * @brief (AR) الحصول على مرجع الدالة
             * @brief (EN) Get function reference
             *
             * (AR) يُرجع المؤشر المشترك لمرجع الدالة. يجب أن تكون القيمة من نوع FUNCTION
             * (EN) Returns the shared pointer to function reference. Value must be FUNCTION type
             *
             * @return (FunctionRefPtr) — (AR) مؤشر مشترك لمرجع الدالة / (EN) shared pointer to function reference
             * @throws std::runtime_error (AR) إذا لم تكن القيمة دالة / (EN) if value is not a function
             */
            FunctionRefPtr toFunction() const;

            /**
             * @brief (AR) الحصول على اسم الدالة المسجل (للاستدعاء)
             * @brief (EN) Get registered function name (for calling)
             *
             * (AR) يعمل مع FUNCTION (يرجع registeredName) ومع STRING (يرجع النص)
             * (EN) Works with FUNCTION (returns registeredName) and STRING (returns the string)
             */
            std::string getFunctionName() const;

            /**
             * @brief (AR) الحصول على اسم الصنف إذا كانت القيمة كائناً
             * @brief (EN) Get class name if value is an object
             *
             * (AR) يعمل مع نوع OBJECT الجديد ومع MAP القديم الذي يحتوي على __class__
             * (EN) Works with new OBJECT type and legacy MAP containing __class__
             *
             * @return (std::string) — (AR) اسم الصنف أو "" إذا ليست كائناً / (EN) class name or "" if not an object
             */
            std::string getClassName() const;

            // Array/Map operations
            // (AR) عمليات المصفوفات والقواميس / (EN) Array/Map operations

            /**
             * @brief (AR) الحصول على عنصر من مصفوفة أو قاموس
             * @brief (EN) Get element from array or map
             */
            Value &operator[](size_t index); // Array indexing
            const Value &operator[](size_t index) const;
            Value &operator[](const std::string &key); // Map indexing
            const Value &operator[](const std::string &key) const;

            /**
             * @brief (AR) الحصول على طول المصفوفة أو القاموس
             * @brief (EN) Get length of array or map
             */
            size_t size() const;
            size_t length() const { return size(); }

            /**
             * @brief (AR) التحقق من وجود مفتاح في القاموس
             * @brief (EN) Check if key exists in map
             */
            bool hasKey(const std::string &key) const;

            /**
             * @brief (AR) إضافة عنصر لنهاية المصفوفة
             * @brief (EN) Append element to end of array
             */
            void push(const Value &val);
            void append(const Value &val) { push(val); }

            /**
             * @brief (AR) حذف آخر عنصر من المصفوفة
             * @brief (EN) Remove last element from array
             */
            Value pop();

            /**
             * @brief (AR) حذف عنصر من القاموس
             * @brief (EN) Remove element from map
             */
            bool remove(const std::string &key);

            /**
             * @brief (AR) حذف عنصر من المصفوفة بالفهرس
             * @brief (EN) Remove element from array by index
             */
            void remove(size_t index);

            /**
             * @brief (AR) مسح جميع العناصر
             * @brief (EN) Clear all elements
             */
            void clear();

            /**
             * @brief (AR) التحقق من الفراغ
             * @brief (EN) Check if empty
             */
            bool isEmpty() const;

            /**
             * @brief (AR) الحصول على جميع المفاتيح (للقاموس)
             * @brief (EN) Get all keys (for map)
             */
            std::vector<std::string> keys() const;

            /**
             * @brief (AR) الحصول على جميع القيم (للقاموس)
             * @brief (EN) Get all values (for map)
             */
            std::vector<Value> values() const;

            // Arithmetic operators
            Value operator+(const Value &other) const;
            Value operator-(const Value &other) const;
            Value operator*(const Value &other) const;
            Value operator/(const Value &other) const;
            Value operator%(const Value &other) const;
            Value operator-() const; // Unary minus

            // Comparison operators
            Value operator==(const Value &other) const;
            Value operator!=(const Value &other) const;
            Value operator<(const Value &other) const;
            Value operator>(const Value &other) const;
            Value operator<=(const Value &other) const;
            Value operator>=(const Value &other) const;

            // Logical operators
            Value operator&&(const Value &other) const;
            Value operator||(const Value &other) const;
            Value operator!() const;

            // Increment/Decrement
            Value &operator++();   // ++val
            Value operator++(int); // val++
            Value &operator--();   // --val
            Value operator--(int); // val--

            // Compound assignment
            Value &operator+=(const Value &other);
            Value &operator-=(const Value &other);
            Value &operator*=(const Value &other);
            Value &operator/=(const Value &other);

            // Debug
            std::string debugString() const;
            std::string getTypeName() const;

        private:
            /**
             * @brief (AR) نوع القيمة الغني — يحمل كل معلومات النوع (ADR-01 Phase 3)
             * @brief (EN) Rich type — holds full type information (ADR-01 Phase 3)
             *
             * (AR) sadType_ هو المصدر الرئيسي لمعلومات النوع.
             *      type_ هو cache محسوب للتوافق الخلفي (يُحسب من sadType_ في المنشئ).
             *      بعد ترحيل كل الكود، يُزال type_ ويبقى sadType_ فقط.
             */
            Types::SadTypePtr sadType_;
            Types::SadTypeKind type_; // (AR) cache للتوافق الخلفي — يُحسب من sadType_ / (EN) Backward-compat cache

            /**
             * @brief (AR) التخزين الداخلي — يستخدم std::variant لتخزين القيمة الفعلية
             * @brief (EN) Internal storage — uses std::variant to store the actual value
             *
             * (AR) الأنواع المخزّنة:
             *   - monostate → VOID (فراغ)
             *   - int → INTEGER (رقم صحيح)
             *   - double → DOUBLE (رقم عشري)
             *   - string → STRING (نص)
             *   - bool → BOOLEAN (منطقي)
             *   - shared_ptr<ArrayType> → ARRAY (مصفوفة — مشاركة بالمرجع)
             *   - shared_ptr<MapType> → MAP (قاموس — مشاركة بالمرجع)
             *   - shared_ptr<ObjectInstance> → OBJECT (كائن — مشاركة بالمرجع)
             *
             * (EN) Stored types:
             *   - monostate → VOID
             *   - int → INTEGER
             *   - double → DOUBLE
             *   - string → STRING
             *   - bool → BOOLEAN
             *   - shared_ptr<ArrayType> → ARRAY (shared by reference)
             *   - shared_ptr<MapType> → MAP (shared by reference)
             *   - shared_ptr<ObjectInstance> → OBJECT (shared by reference)
             */
            std::variant<
                std::monostate,              // VOID
                int64_t,                     // INTEGER (64-bit)
                double,                      // DOUBLE
                std::string,                 // STRING
                bool,                        // BOOLEAN
                std::shared_ptr<ArrayType>,  // ARRAY
                std::shared_ptr<MapType>,    // MAP
                std::shared_ptr<TupleType>,  // TUPLE — صف مرتب
                ObjectInstance *,            // OBJECT — كائن مُدار بـGC (B-step5b)
                std::shared_ptr<FunctionRef> // FUNCTION — مرجع دالة من الدرجة الأولى
                >
                data_;

            void throwTypeMismatch(const std::string &operation, const Value &other) const;
            void throwInvalidType(const std::string &operation) const;
        };

    } // namespace Data
} // namespace Sad
