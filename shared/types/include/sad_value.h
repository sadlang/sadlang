/**
 * @file sad_value.h
 * @brief (AR) قيمة وقت التشغيل الجديدة — تستخدم SadType بدلاً من ValueType
 * @brief (EN) New runtime value — uses SadType instead of ValueType
 *
 * ═══════════════════════════════════════════════════════════════════════════════════
 * (AR) SadValue هو البديل الرسمي لـ Value (value.h) في ADR-01.
 *
 *      الفرق الجوهري:
 *        Value   يستخدم → ValueType (enum بسيط: 10 قيم فقط)
 *        SadValue يستخدم → SadTypePtr (نظام أنواع غني: 46 نوع، generics، OOP)
 *
 *      SadValue يحمل:
 *        1. SadTypePtr — معلومات النوع الكاملة (يمكن أن تكون مصفوفة<رقم> وليس مجرد "مصفوفة")
 *        2. std::variant — البيانات الفعلية (نفس هيكل Value القديم)
 *
 *      متى تستخدم SadValue بدلاً من Value:
 *        - كل كود جديد يُكتب من الآن (Phase 1)
 *        - كود يُعاد كتابته/ترحيله (Phase 2)
 *        - في Phase 3 يُحذف Value بالكامل ويبقى SadValue فقط
 *
 *      التحويل بين SadValue و Value:
 *        - SadValue::fromLegacy(Value) — استقبال من كود قديم
 *        - SadValue::toLegacy() — تمرير لكود قديم
 *        - الدوال في type_bridge.h للتحويلات الأكثر تخصصاً
 *
 * (EN) SadValue is the official replacement for Value (value.h) in ADR-01.
 *
 *      Key difference:
 *        Value    uses → ValueType (simple enum: 10 values only)
 *        SadValue uses → SadTypePtr (rich type system: 46 types, generics, OOP)
 *
 *      SadValue holds:
 *        1. SadTypePtr — full type information (can be Array<Integer> not just "Array")
 *        2. std::variant — actual data (same structure as legacy Value)
 * ═══════════════════════════════════════════════════════════════════════════════════
 *
 * @author فريق تطوير لغة ص / S Language Development Team
 * @date يناير 2026 / January 2026
 * @version 1.0 — ADR-01 Phase 1
 */

#pragma once

#include "sad_type_system.h"
#include <variant>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <sstream>
#include <cstdint>

// ═══════════════════════════════════════════════════════════════════════════════════
// (AR) إلغاء تعريفات ماكروز ويندوز المتعارضة
// ═══════════════════════════════════════════════════════════════════════════════════
#ifdef VOID
#undef VOID
#endif
#ifdef ERROR
#undef ERROR
#endif

// ═══════════════════════════════════════════════════════════════════════════════════
// (AR) إعلانات مسبقة
// ═══════════════════════════════════════════════════════════════════════════════════
namespace Sad
{
    namespace Data
    {
        class Value;
        class ObjectInstance;
        struct FunctionRef;
    } // namespace Data
} // namespace Sad

namespace Sad
{
    namespace Types
    {

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  SadValue — القيمة الجديدة مع نظام أنواع غني
        //  SadValue — New value with rich type system
        //
        // █████████████████████████████████████████████████████████████████████████████████

        /**
         * @brief (AR) قيمة وقت التشغيل الجديدة — النظام الموحد
         * @brief (EN) New runtime value — unified type system
         *
         * (AR) SadValue تجمع بين:
         *      - SadTypePtr: نوع غني يحمل كل معلومات النوع (بما فيها الأنواع الفرعية)
         *      - DataVariant: البيانات الفعلية (رقم، نص، مصفوفة، كائن...)
         *
         *      تصميم variant مطابق لـ Value القديم لضمان التوافق وسهولة الترحيل.
         */
        class SadValue
        {
        public:
            // ══════════════════════════════════════════════════════════════════
            // (AR) تعريفات الأنواع المركبة / (EN) Composite type definitions
            // ══════════════════════════════════════════════════════════════════
            using ArrayType = std::vector<SadValue>;
            using MapType = std::unordered_map<std::string, SadValue>;
            // (AR) B-step5b: ObjectPtr مؤشر خام مُدار بـGC (متوافق مع Data::Value::ObjectPtr).
            // (EN) B-step5b: raw GC-managed pointer (matches Data::Value::ObjectPtr).
            using ObjectPtr = Data::ObjectInstance *;
            using FunctionRefPtr = std::shared_ptr<Data::FunctionRef>;

            /// @brief (AR) بنية الصف — تغلف vector<SadValue> لتمييزها عن ArrayType في variant
            /// @brief (EN) Tuple struct — wraps vector<SadValue> to distinguish from ArrayType
            struct TupleType
            {
                std::vector<SadValue> elements;

                TupleType() = default;
                explicit TupleType(std::vector<SadValue> elems) : elements(std::move(elems)) {}

                size_t size() const { return elements.size(); }
                bool empty() const { return elements.empty(); }
                const SadValue &operator[](size_t i) const { return elements[i]; }
                SadValue &operator[](size_t i) { return elements[i]; }
                auto begin() const { return elements.begin(); }
                auto end() const { return elements.end(); }
                auto begin() { return elements.begin(); }
                auto end() { return elements.end(); }

                /// @brief (AR) مقارنة — مطلوبة لـ std::variant::operator==
                bool operator==(const TupleType &other) const
                {
                    return elements == other.elements;
                }
                bool operator!=(const TupleType &other) const { return !(*this == other); }
            };

            /// @brief (AR) نوع البيانات الداخلي / (EN) Internal data variant
            using DataVariant = std::variant<
                std::monostate, // Void / فراغ
                int64_t,        // Integer / رقم
                double,         // Float / عشري
                std::string,    // String / نص
                bool,           // Boolean / منطقي
                ArrayType,      // Array / مصفوفة
                MapType,        // Map / خريطة
                TupleType,      // Tuple / صف
                ObjectPtr,      // Object / كائن
                FunctionRefPtr  // Function / دالة
                >;

            // ══════════════════════════════════════════════════════════════════
            // (AR) المنشئات / (EN) Constructors
            // ══════════════════════════════════════════════════════════════════

            /// @brief (AR) إنشاء قيمة فارغة (فراغ) / (EN) Create Void value
            SadValue()
                : type_(SadTypeRegistry::instance().getVoid()), data_(std::monostate{}) {}

            /// @brief (AR) إنشاء قيمة رقم صحيح / (EN) Create integer value
            explicit SadValue(int64_t val)
                : type_(SadTypeRegistry::instance().getInteger()), data_(val) {}

            /// @brief (AR) إنشاء من int (ترقية تلقائية إلى int64_t) / (EN) Create from int (auto-promote)
            explicit SadValue(int val)
                : type_(SadTypeRegistry::instance().getInteger()), data_(static_cast<int64_t>(val)) {}

            /// @brief (AR) إنشاء قيمة عشرية / (EN) Create float value
            explicit SadValue(double val)
                : type_(SadTypeRegistry::instance().getFloat()), data_(val) {}

            /// @brief (AR) إنشاء قيمة نصية / (EN) Create string value
            SadValue(const std::string &val)
                : type_(SadTypeRegistry::instance().getString()), data_(val) {}

            /// @brief (AR) إنشاء قيمة نصية (نقل) / (EN) Create string value (move)
            SadValue(std::string &&val)
                : type_(SadTypeRegistry::instance().getString()), data_(std::move(val)) {}

            /// @brief (AR) إنشاء قيمة نصية من C-string / (EN) Create string from C-string
            SadValue(const char *val)
                : type_(SadTypeRegistry::instance().getString()), data_(std::string(val)) {}

            /// @brief (AR) إنشاء قيمة منطقية / (EN) Create boolean value
            explicit SadValue(bool val)
                : type_(SadTypeRegistry::instance().getBoolean()), data_(val) {}

            /// @brief (AR) إنشاء مصفوفة / (EN) Create array value
            explicit SadValue(ArrayType val)
                : type_(SadTypeRegistry::instance().makeArray()), data_(std::move(val)) {}

            /// @brief (AR) إنشاء مصفوفة مع نوع عنصر محدد / (EN) Create array with specific element type
            SadValue(ArrayType val, SadTypePtr elementType)
                : type_(SadTypeRegistry::instance().makeArray(std::move(elementType))),
                  data_(std::move(val)) {}

            /// @brief (AR) إنشاء خريطة / (EN) Create map value
            explicit SadValue(MapType val)
                : type_(SadTypeRegistry::instance().makeMap()), data_(std::move(val)) {}

            /// @brief (AR) إنشاء صف / (EN) Create tuple value
            explicit SadValue(TupleType val)
                : type_(SadTypeRegistry::instance().makeTuple({})), data_(std::move(val)) {}

            /// @brief (AR) إنشاء كائن / (EN) Create object value
            explicit SadValue(ObjectPtr obj)
                : type_(SadTypeRegistry::instance().getAny()), data_(std::move(obj)) {}

            /// @brief (AR) إنشاء كائن مع اسم الصنف / (EN) Create object with class name
            SadValue(ObjectPtr obj, const std::string &className)
                : type_(SadTypeRegistry::instance().getOrCreateClass(className)),
                  data_(std::move(obj)) {}

            /// @brief (AR) إنشاء مرجع دالة / (EN) Create function reference value
            explicit SadValue(FunctionRefPtr funcRef)
                : type_(SadTypeRegistry::instance().makeFunction({})), data_(std::move(funcRef)) {}

            /// @brief (AR) إنشاء من نوع محدد وبيانات / (EN) Create from explicit type and data
            SadValue(SadTypePtr type, DataVariant data)
                : type_(std::move(type)), data_(std::move(data))
            {
                if (!type_)
                    type_ = SadTypeRegistry::instance().getVoid();
            }

            // ══════════════════════════════════════════════════════════════════
            // (AR) فحص النوع / (EN) Type checking
            // ══════════════════════════════════════════════════════════════════

            /// @brief (AR) الحصول على النوع الكامل / (EN) Get full type
            const SadTypePtr &getType() const { return type_; }

            /// @brief (AR) الحصول على نوع kind (مختصر) / (EN) Get type kind (shortcut)
            SadTypeKind getKind() const { return type_ ? type_->getKind() : SadTypeKind::Void; }

            // ─── فحوصات سريعة / Quick checks ───
            bool isVoid() const { return getKind() == SadTypeKind::Void; }
            bool isInteger() const { return getKind() == SadTypeKind::Integer; }
            bool isFloat() const { return getKind() == SadTypeKind::Float; }
            bool isBoolean() const { return getKind() == SadTypeKind::Boolean; }
            bool isString() const { return getKind() == SadTypeKind::String; }
            bool isArray() const { return getKind() == SadTypeKind::Array; }
            bool isMap() const { return getKind() == SadTypeKind::Map; }
            bool isTuple() const { return getKind() == SadTypeKind::Tuple; }
            bool isFunction() const
            {
                auto k = getKind();
                return k == SadTypeKind::Function || k == SadTypeKind::Closure;
            }
            bool isObject() const
            {
                auto k = getKind();
                return k == SadTypeKind::Class || k == SadTypeKind::Struct;
            }
            bool isNumeric() const { return type_ && type_->isNumeric(); }
            bool isPrimitive() const { return type_ && type_->isPrimitive(); }
            bool isComposite() const { return type_ && type_->isComposite(); }
            bool isCallable() const { return type_ && type_->isCallable(); }

            // ══════════════════════════════════════════════════════════════════
            // (AR) الوصول للبيانات / (EN) Data access
            // ══════════════════════════════════════════════════════════════════

            /// @brief (AR) الحصول على الرقم الصحيح / (EN) Get integer value
            /// @throws std::bad_variant_access (AR) إذا لم يكن النوع رقماً صحيحاً
            int64_t toInt64() const { return std::get<int64_t>(data_); }

            /// @brief (AR) الحصول على الرقم كـ int / (EN) Get as int
            int toInt() const { return static_cast<int>(toInt64()); }

            /// @brief (AR) الحصول على العشري / (EN) Get double value
            double toDouble() const
            {
                if (std::holds_alternative<double>(data_))
                    return std::get<double>(data_);
                if (std::holds_alternative<int64_t>(data_))
                    return static_cast<double>(std::get<int64_t>(data_));
                throw std::runtime_error("SadValue::toDouble(): النوع ليس عشرياً");
            }

            /// @brief (AR) الحصول على المنطقي / (EN) Get boolean value
            bool toBool() const
            {
                if (std::holds_alternative<bool>(data_))
                    return std::get<bool>(data_);
                // (AR) تحويل ضمني: 0=خطأ، ""=خطأ، null=خطأ
                if (std::holds_alternative<std::monostate>(data_))
                    return false;
                if (std::holds_alternative<int64_t>(data_))
                    return std::get<int64_t>(data_) != 0;
                if (std::holds_alternative<double>(data_))
                    return std::get<double>(data_) != 0.0;
                if (std::holds_alternative<std::string>(data_))
                    return !std::get<std::string>(data_).empty();
                return true; // (AR) المصفوفات والكائنات والدوال = صحيح / (EN) arrays, objects, functions = true
            }

            /// @brief (AR) تحويل إلى نص / (EN) Convert to string
            std::string toString() const
            {
                if (std::holds_alternative<std::string>(data_))
                    return std::get<std::string>(data_);
                if (std::holds_alternative<int64_t>(data_))
                    return std::to_string(std::get<int64_t>(data_));
                if (std::holds_alternative<double>(data_))
                {
                    std::ostringstream oss;
                    oss << std::get<double>(data_);
                    return oss.str();
                }
                if (std::holds_alternative<bool>(data_))
                    return std::get<bool>(data_) ? "صحيح" : "خطأ";
                if (std::holds_alternative<std::monostate>(data_))
                    return "لاشيء";
                return "<" + (type_ ? type_->arabicName() : "مجهول") + ">";
            }

            /// @brief (AR) مرجع ثابت للنص / (EN) Const reference to string
            const std::string &toStringRef() const { return std::get<std::string>(data_); }

            /// @brief (AR) الحصول على المصفوفة / (EN) Get array
            const ArrayType &toArrayRef() const { return std::get<ArrayType>(data_); }
            ArrayType &toArrayMut() { return std::get<ArrayType>(data_); }

            /// @brief (AR) الحصول على الخريطة / (EN) Get map
            const MapType &toMapRef() const { return std::get<MapType>(data_); }
            MapType &toMapMut() { return std::get<MapType>(data_); }

            /// @brief (AR) الحصول على الصف / (EN) Get tuple
            const TupleType &toTupleRef() const { return std::get<TupleType>(data_); }

            /// @brief (AR) الحصول على مؤشر الكائن / (EN) Get object pointer
            ObjectPtr toObject() const
            {
                if (!std::holds_alternative<ObjectPtr>(data_))
                    throw std::runtime_error("SadValue::toObject(): القيمة ليست كائناً");
                return std::get<ObjectPtr>(data_);
            }

            /// @brief (AR) الحصول على مرجع الدالة / (EN) Get function reference
            FunctionRefPtr toFunction() const
            {
                if (!std::holds_alternative<FunctionRefPtr>(data_))
                    throw std::runtime_error("SadValue::toFunction(): القيمة ليست دالة");
                return std::get<FunctionRefPtr>(data_);
            }

            // ══════════════════════════════════════════════════════════════════
            // (AR) الوصول المباشر لـ variant / (EN) Direct variant access
            // ══════════════════════════════════════════════════════════════════

            /// @brief (AR) الحصول على variant البيانات الداخلي / (EN) Get internal data variant
            const DataVariant &getData() const { return data_; }
            DataVariant &getDataMut() { return data_; }

            // ══════════════════════════════════════════════════════════════════
            // (AR) التحويل من/إلى Value القديم / (EN) Conversion from/to legacy Value
            // ══════════════════════════════════════════════════════════════════

            /**
             * @brief (AR) إنشاء SadValue من Value قديم
             * @brief (EN) Create SadValue from legacy Value
             *
             * (AR) يُستخدم عند استقبال قيم من كود قديم يستخدم Value.
             *      يستنتج SadTypePtr من ValueType ثم ينسخ البيانات.
             *      معلن هنا — التنفيذ في type_bridge.cpp (يحتاج value.h الكامل).
             *
             * @param legacy قيمة Value القديمة
             * @return SadValue جديد
             */
            static SadValue fromLegacy(const Data::Value &legacy);

            /**
             * @brief (AR) تحويل إلى Value قديم
             * @brief (EN) Convert to legacy Value
             *
             * (AR) يُستخدم عند تمرير قيم لكود قديم يستخدم Value.
             *      معلن هنا — التنفيذ في type_bridge.cpp (يحتاج value.h الكامل).
             *
             * @return Data::Value قيمة قديمة
             */
            Data::Value toLegacy() const;

            // ══════════════════════════════════════════════════════════════════
            // (AR) معلومات النوع الغنية / (EN) Rich type information
            // ══════════════════════════════════════════════════════════════════

            /// @brief (AR) اسم النوع بالعربية / (EN) Arabic type name
            std::string typeName() const { return type_ ? type_->arabicName() : "مجهول"; }

            /// @brief (AR) اسم النوع بالإنجليزية / (EN) English type name
            std::string typeNameEn() const { return type_ ? type_->englishName() : "Unknown"; }

            /// @brief (AR) هل النوع متوافق مع نوع آخر؟ / (EN) Is type compatible with another?
            bool isCompatibleWith(const SadTypePtr &target) const
            {
                if (!type_ || !target)
                    return true;
                return type_->isAssignableTo(target.get());
            }

            /// @brief (AR) حجم النوع بالبايتات / (EN) Type size in bytes
            size_t typeSizeInBytes() const { return type_ ? type_->sizeInBytes() : 0; }

            // ══════════════════════════════════════════════════════════════════
            // (AR) المقارنة / (EN) Comparison
            // ══════════════════════════════════════════════════════════════════

            bool operator==(const SadValue &other) const
            {
                if (getKind() != other.getKind())
                    return false;
                return data_ == other.data_;
            }

            bool operator!=(const SadValue &other) const { return !(*this == other); }

        private:
            SadTypePtr type_;  ///< (AR) النوع الكامل / (EN) Full type information
            DataVariant data_; ///< (AR) البيانات الفعلية / (EN) Actual data
        };

        // ═══════════════════════════════════════════════════════════════════════════════════
        // (AR) أسماء مختصرة مفيدة / (EN) Useful short aliases
        // ═══════════════════════════════════════════════════════════════════════════════════
        using SadValuePtr = std::shared_ptr<SadValue>;

    } // namespace Types
} // namespace Sad
