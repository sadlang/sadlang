/**
 * @file sad_type_system.h
 * @brief (AR) نظام الأنواع الموحد الوحيد في لغة ص — ملف واحد يحتوي كل شيء
 * @brief (EN) The ONE unified type system for Sad language — single file, everything included
 *
 * ═══════════════════════════════════════════════════════════════════════════════════
 *
 * (AR) هذا الملف هو المصدر الوحيد لنظام الأنواع في المشروع بأكمله.
 *      يُستخدم من:
 *        • المفسر (interpreter/)
 *        • المترجم (compiler/)
 *        • المحلل النحوي (shared/parser/)
 *        • شجرة AST (shared/ast/)
 *
 *      المحتويات:
 *        ① SadTypeKind — تعداد موحد (50 قيمة، مُولَّد من types.yaml) يغطي كل الأنظمة
 *        ② SadType — الصنف الأساسي المجرد + 15 صنف فرعي
 *        ③ SadTypeRegistry — Singleton لتخزين الأنواع وإنشائها
 *        ④ دوال مساعدة — أسماء عربية/إنجليزية، فحوصات، تحويلات
 *        ⑤ أسماء بديلة — SadTypeKind = SadTypeKind (للمترجم)
 *
 *      القاعدة الذهبية:
 *        include "sad_type_system.h" فقط — لا حاجة لأي ملف آخر!
 *
 * (EN) This file is the SINGLE source of truth for the type system.
 *      Used by interpreter, compiler, parser, and AST.
 *      Just #include "sad_type_system.h" — nothing else needed!
 *
 * ═══════════════════════════════════════════════════════════════════════════════════
 *
 * @author فريق تطوير لغة ص / Sad Language Team
 * @date أبريل 2026 / April 2026
 */

#pragma once

// ═══════════════════════════════════════════════════════════════════════════════════
// الاعتمادات / Dependencies
// ═══════════════════════════════════════════════════════════════════════════════════
#include "../generated/sad_type_kind_generated.h" // (AR) تعداد SadTypeKind — مُولَّد من language-truth/types.yaml (لا يُحرَّر يدويًّا)
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <mutex>

namespace Sad
{
    namespace Types
    {

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  الجزء ①: التعداد الموحد SadTypeKind — مُولَّد من types.yaml (50 قيمة)
        //  Part ①: Unified SadTypeKind enum — generated from types.yaml (50 values)
        //
        //  ⚠️ التعداد نفسه في: shared/types/generated/sad_type_kind_generated.h
        //     مصدر الحقيقة: language-truth/types.yaml — عدّل هناك ثم أعد البناء.
        //     The enum lives in the generated header above; SoT is types.yaml.
        //
        // █████████████████████████████████████████████████████████████████████████████████

        // ═══════════════════════════════════════════════════════════════════════════════════
        // دوال مساعدة للتعداد / Helper functions for the enum
        // ═══════════════════════════════════════════════════════════════════════════════════

        /** @brief (AR) الاسم العربي المختصر للنوع */
        inline std::string sadTypeKindToArabic(SadTypeKind kind)
        {
            switch (kind)
            {
            case SadTypeKind::Void:
                return "فراغ";
            case SadTypeKind::Null:
                return "عدم";
            case SadTypeKind::Integer:
                return "رقم";
            case SadTypeKind::Float:
                return "عشري";
            case SadTypeKind::Boolean:
                return "منطقي";
            case SadTypeKind::String:
                return "نص";
            case SadTypeKind::Byte:
                return "بايت";
            case SadTypeKind::Int8:
                return "عدد8";
            case SadTypeKind::Int16:
                return "عدد16";
            case SadTypeKind::Int32:
                return "عدد32";
            case SadTypeKind::Int64:
                return "عدد64";
            case SadTypeKind::UInt8:
                return "طبيعي8";
            case SadTypeKind::UInt16:
                return "طبيعي16";
            case SadTypeKind::UInt32:
                return "طبيعي32";
            case SadTypeKind::UInt64:
                return "طبيعي64";
            case SadTypeKind::Float32:
                return "عشري32";
            case SadTypeKind::Float64:
                return "عشري64";
            case SadTypeKind::Char:
                return "حرف";
            case SadTypeKind::Array:
                return "مصفوفة";
            case SadTypeKind::Map:
                return "خريطة";
            case SadTypeKind::Tuple:
                return "صف";
            case SadTypeKind::Slice:
                return "شريحة";
            case SadTypeKind::Class:
                return "صنف";
            case SadTypeKind::Struct:
                return "بنية";
            case SadTypeKind::Enum:
                return "تعداد";
            case SadTypeKind::Trait:
                return "سمة";
            case SadTypeKind::Function:
                return "دالة";
            case SadTypeKind::Closure:
                return "إغلاق";
            case SadTypeKind::Union:
                return "اتحاد";
            case SadTypeKind::Intersection:
                return "تقاطع";
            case SadTypeKind::Optional:
                return "اختياري";
            case SadTypeKind::Result:
                return "نتيجة";
            case SadTypeKind::Generic:
                return "عام";
            case SadTypeKind::TypeParameter:
                return "معامل_نوع";
            case SadTypeKind::TypeAlias:
                return "مستعار";
            case SadTypeKind::Pointer:
                return "مؤشر";
            case SadTypeKind::Reference:
                return "مرجع";
            case SadTypeKind::MutableRef:
                return "مرجع_متغير";
            case SadTypeKind::Any:
                return "أي";
            case SadTypeKind::Never:
                return "أبداً";
            case SadTypeKind::Unknown:
                return "مجهول";
            case SadTypeKind::Error:
                return "خطأ";
            case SadTypeKind::Future:
                return "مستقبل";
            case SadTypeKind::Generator:
                return "مولّد";
            case SadTypeKind::Comprehension:
                return "استيعاب";
            case SadTypeKind::Color:
                return "لون";
            case SadTypeKind::Widget:
                return "عنصر_واجهة";
            case SadTypeKind::Window:
                return "نافذة";
            case SadTypeKind::Event:
                return "حدث";
            case SadTypeKind::Vector:
                return "متجه";
            }
            return "مجهول";
        }

        /** @brief (EN) English name for debugging */
        inline std::string sadTypeKindToEnglish(SadTypeKind kind)
        {
            switch (kind)
            {
            case SadTypeKind::Void:
                return "Void";
            case SadTypeKind::Null:
                return "Null";
            case SadTypeKind::Integer:
                return "Integer";
            case SadTypeKind::Float:
                return "Float";
            case SadTypeKind::Boolean:
                return "Boolean";
            case SadTypeKind::String:
                return "String";
            case SadTypeKind::Byte:
                return "Byte";
            case SadTypeKind::Int8:
                return "Int8";
            case SadTypeKind::Int16:
                return "Int16";
            case SadTypeKind::Int32:
                return "Int32";
            case SadTypeKind::Int64:
                return "Int64";
            case SadTypeKind::UInt8:
                return "UInt8";
            case SadTypeKind::UInt16:
                return "UInt16";
            case SadTypeKind::UInt32:
                return "UInt32";
            case SadTypeKind::UInt64:
                return "UInt64";
            case SadTypeKind::Float32:
                return "Float32";
            case SadTypeKind::Float64:
                return "Float64";
            case SadTypeKind::Char:
                return "Char";
            case SadTypeKind::Array:
                return "Array";
            case SadTypeKind::Map:
                return "Map";
            case SadTypeKind::Tuple:
                return "Tuple";
            case SadTypeKind::Slice:
                return "Slice";
            case SadTypeKind::Class:
                return "Class";
            case SadTypeKind::Struct:
                return "Struct";
            case SadTypeKind::Enum:
                return "Enum";
            case SadTypeKind::Trait:
                return "Trait";
            case SadTypeKind::Function:
                return "Function";
            case SadTypeKind::Closure:
                return "Closure";
            case SadTypeKind::Union:
                return "Union";
            case SadTypeKind::Intersection:
                return "Intersection";
            case SadTypeKind::Optional:
                return "Optional";
            case SadTypeKind::Result:
                return "Result";
            case SadTypeKind::Generic:
                return "Generic";
            case SadTypeKind::TypeParameter:
                return "TypeParameter";
            case SadTypeKind::TypeAlias:
                return "TypeAlias";
            case SadTypeKind::Pointer:
                return "Pointer";
            case SadTypeKind::Reference:
                return "Reference";
            case SadTypeKind::MutableRef:
                return "MutableRef";
            case SadTypeKind::Any:
                return "Any";
            case SadTypeKind::Never:
                return "Never";
            case SadTypeKind::Unknown:
                return "Unknown";
            case SadTypeKind::Error:
                return "Error";
            case SadTypeKind::Future:
                return "Future";
            case SadTypeKind::Generator:
                return "Generator";
            case SadTypeKind::Comprehension:
                return "Comprehension";
            case SadTypeKind::Color:
                return "Color";
            case SadTypeKind::Widget:
                return "Widget";
            case SadTypeKind::Window:
                return "Window";
            case SadTypeKind::Event:
                return "Event";
            case SadTypeKind::Vector:
                return "Vector";
            }
            return "Unknown";
        }

        /** @brief (AR) هل النوع بدائي؟ */
        inline bool isPrimitiveKind(SadTypeKind k)
        {
            return k == SadTypeKind::Void || k == SadTypeKind::Integer || k == SadTypeKind::Float || k == SadTypeKind::Boolean || k == SadTypeKind::String || k == SadTypeKind::Byte || (k >= SadTypeKind::Int8 && k <= SadTypeKind::Char);
        }

        /** @brief (AR) هل النوع رقمي؟ */
        inline bool isNumericKind(SadTypeKind k)
        {
            return k == SadTypeKind::Integer || k == SadTypeKind::Float || k == SadTypeKind::Byte || (k >= SadTypeKind::Int8 && k <= SadTypeKind::Float64);
        }

        /** @brief (AR) هل النوع مركب؟ */
        inline bool isCompositeKind(SadTypeKind k)
        {
            return k == SadTypeKind::Array || k == SadTypeKind::Map || k == SadTypeKind::Tuple || k == SadTypeKind::Slice;
        }

        /** @brief (AR) هل النوع قابل للاستدعاء؟ */
        inline bool isCallableKind(SadTypeKind k)
        {
            return k == SadTypeKind::Function || k == SadTypeKind::Closure;
        }

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  الجزء ②: SadType — الصنف الأساسي المجرد + جميع الأصناف الفرعية
        //  Part ②: SadType — abstract base + all subclasses
        //
        // █████████████████████████████████████████████████████████████████████████████████

        // ─── إعلانات مسبقة / Forward declarations ───
        class SadType;
        using SadTypePtr = std::shared_ptr<const SadType>;
        using SadTypePtrMut = std::shared_ptr<SadType>;

        // ─── إعلان مسبق للأنواع الفرعية (مطلوب لـ isAssignableTo) ───
        class SadOptionalType;
        class SadResultType; // (S-TS-P3)
        class SadUnionType;
        class SadTypeAlias;
        class SadArrayType;

        /**
         * @brief (AR) الصنف الأساسي المجرد لكل أنواع لغة ص
         * @brief (EN) Abstract base class for all Sad language types
         */
        class SadType : public std::enable_shared_from_this<SadType>
        {
        public:
            virtual ~SadType() = default;

            // ─── الهوية / Identity ───
            SadTypeKind getKind() const { return kind_; }

            // ─── الأسماء / Names ───
            virtual std::string arabicName() const = 0;
            virtual std::string englishName() const = 0;
            virtual std::string toString() const { return arabicName(); }

            // ─── المقارنة / Comparison ───
            virtual bool equals(const SadType *other) const
            {
                return other && kind_ == other->kind_;
            }

            // ─── التوافقية / Compatibility ───
            virtual bool isAssignableTo(const SadType *target) const;
            virtual bool isSubtypeOf(const SadType *parent) const { return equals(parent); }
            virtual bool coercesTo(const SadType *target) const;

            // ─── فحوصات سريعة / Quick checks ───
            bool isPrimitive() const { return isPrimitiveKind(kind_); }
            bool isNumeric() const { return isNumericKind(kind_); }
            bool isComposite() const { return isCompositeKind(kind_); }
            bool isCallable() const { return isCallableKind(kind_); }
            bool isNullable() const { return kind_ == SadTypeKind::Void || kind_ == SadTypeKind::Optional; }

            /**
             * @brief (AR) هل النوع قابل للنسخ؟ (الأنواع البدائية والمراجع قابلة للنسخ)
             * @brief (EN) Is the type copyable? (Primitive types and references are copyable)
             */
            bool isCopyable() const
            {
                switch (kind_)
                {
                case SadTypeKind::Boolean:
                case SadTypeKind::Int8:
                case SadTypeKind::Int16:
                case SadTypeKind::Int32:
                case SadTypeKind::Int64:
                case SadTypeKind::UInt8:
                case SadTypeKind::UInt16:
                case SadTypeKind::UInt32:
                case SadTypeKind::UInt64:
                case SadTypeKind::Float32:
                case SadTypeKind::Float64:
                case SadTypeKind::Char:
                case SadTypeKind::Reference:
                case SadTypeKind::Byte:
                    return true;
                default:
                    return false;
                }
            }

            // ─── الحجم والمحاذاة (للمترجم) / Size and alignment ───
            virtual size_t sizeInBytes() const { return 0; }
            virtual size_t alignment() const { return sizeInBytes(); }

            // ─── معلومات الملكية (للمترجم SIR) / Ownership info (for SIR compiler) ───
            /**
             * @brief (AR) هل النوع قابل للتغيير؟ (يُستخدم في تحليل الملكية)
             * @brief (EN) Is the type mutable? (Used in ownership analysis)
             */
            bool isMutable() const { return isMutable_; }
            void setMutable(bool mut) { isMutable_ = mut; }

            /**
             * @brief (AR) اسم دورة الحياة (مثل 'أ) — اختياري
             * @brief (EN) Lifetime name (e.g. 'a) — optional
             */
            const std::optional<std::string> &lifetimeName() const { return lifetimeName_; }
            void setLifetimeName(const std::string &name) { lifetimeName_ = name; }
            void clearLifetimeName() { lifetimeName_.reset(); }

            // ─── معاملات النوع الفرعية (للأنواع المركبة) / Type parameters (for composite types) ───
            /**
             * @brief (AR) إرجاع المعاملات الفرعية للنوع (مثل نوع العنصر في المصفوفة)
             * @brief (EN) Return sub-type parameters (e.g. element type in array)
             * @return قائمة بمؤشرات الأنواع الفرعية / Vector of sub-type pointers
             */
            virtual std::vector<SadTypePtr> getTypeParams() const { return {}; }

            // ─── تحويل إلى تمثيل النوع التشغيلي المتوافق / Runtime compatibility kind ───
            SadTypeKind toValueType() const;
            // (AR) [S-TS-P2.5b] حُذف toDataType() — صفر مستهلك بعد توحيد المحور على SadTypeKind.
            // (EN) [S-TS-P2.5b] toDataType() removed — zero consumers after SadTypeKind unification.

            // ─── Factory Methods ثابتة / Static factories ───
            static SadTypePtr fromValueType(SadTypeKind vt);
            // (AR) [S-TS-P2.5b] حُذف fromDataType() — استُبدِل بـ fromValueType(SadTypeKind).
            static SadTypePtr fromArabicName(const std::string &name);

        protected:
            explicit SadType(SadTypeKind kind) : kind_(kind) {}
            SadTypeKind kind_;
            bool isMutable_ = false;                  ///< (AR) هل النوع قابل للتغيير / (EN) Is type mutable
            std::optional<std::string> lifetimeName_; ///< (AR) اسم دورة الحياة / (EN) Lifetime name
        };

        // ─────────────────────────────────────────────────────────────────────────────────
        //  نوع بدائي / Primitive type (Void, Integer, Float, Boolean, String, Byte)
        // ─────────────────────────────────────────────────────────────────────────────────
        class SadPrimitiveType : public SadType
        {
        public:
            explicit SadPrimitiveType(SadTypeKind kind) : SadType(kind) {}

            std::string arabicName() const override { return sadTypeKindToArabic(kind_); }
            std::string englishName() const override { return sadTypeKindToEnglish(kind_); }

            bool equals(const SadType *other) const override
            {
                return other && kind_ == other->getKind();
            }

            size_t sizeInBytes() const override
            {
                switch (kind_)
                {
                case SadTypeKind::Void:
                    return 0;
                case SadTypeKind::Boolean:
                    return 1;
                case SadTypeKind::Byte:
                    return 1;
                case SadTypeKind::Integer:
                    return 8;
                case SadTypeKind::Float:
                    return 8;
                case SadTypeKind::String:
                    return sizeof(void *);
                default:
                    return 0;
                }
            }
        };

        // ─────────────────────────────────────────────────────────────────────────────────
        //  نوع خاص / Special type (Any, Never, Unknown, Error)
        // ─────────────────────────────────────────────────────────────────────────────────
        class SadSpecialType : public SadType
        {
        public:
            explicit SadSpecialType(SadTypeKind kind) : SadType(kind) {}

            std::string arabicName() const override { return sadTypeKindToArabic(kind_); }
            std::string englishName() const override { return sadTypeKindToEnglish(kind_); }

            bool equals(const SadType *other) const override
            {
                return other && kind_ == other->getKind();
            }

            bool isAssignableTo(const SadType *target) const override
            {
                if (!target)
                    return false;
                if (kind_ == SadTypeKind::Any)
                    return true; // أي → يُعيَّن لكل شيء
                if (kind_ == SadTypeKind::Never)
                    return true; // أبداً → bottom type
                return SadType::isAssignableTo(target);
            }
        };

        // ─────────────────────────────────────────────────────────────────────────────────
        //  نوع مصفوفة / Array type: مصفوفة<T>
        // ─────────────────────────────────────────────────────────────────────────────────
        class SadArrayType : public SadType
        {
        public:
            explicit SadArrayType(SadTypePtr elementType = nullptr)
                : SadType(SadTypeKind::Array), elementType_(std::move(elementType)) {}

            SadTypePtr getElementType() const { return elementType_; }

            std::string arabicName() const override
            {
                return elementType_ ? "مصفوفة<" + elementType_->arabicName() + ">" : "مصفوفة";
            }
            std::string englishName() const override
            {
                return elementType_ ? "Array<" + elementType_->englishName() + ">" : "Array";
            }

            bool equals(const SadType *other) const override
            {
                if (!other || other->getKind() != SadTypeKind::Array)
                    return false;
                auto o = static_cast<const SadArrayType *>(other);
                if (!elementType_ && !o->elementType_)
                    return true;
                if (!elementType_ || !o->elementType_)
                    return false;
                return elementType_->equals(o->elementType_.get());
            }

            bool isAssignableTo(const SadType *target) const override;

            std::vector<SadTypePtr> getTypeParams() const override
            {
                if (elementType_)
                    return {elementType_};
                return {};
            }

            size_t sizeInBytes() const override { return sizeof(void *); }

        private:
            SadTypePtr elementType_;
        };

        // ─────────────────────────────────────────────────────────────────────────────────
        //  نوع خريطة / Map type: خريطة<K, V>
        // ─────────────────────────────────────────────────────────────────────────────────
        class SadMapType : public SadType
        {
        public:
            explicit SadMapType(SadTypePtr keyType = nullptr, SadTypePtr valueType = nullptr)
                : SadType(SadTypeKind::Map), keyType_(std::move(keyType)), valueType_(std::move(valueType)) {}

            SadTypePtr getKeyType() const { return keyType_; }
            SadTypePtr getValueType() const { return valueType_; }

            std::string arabicName() const override
            {
                if (keyType_ && valueType_)
                    return "خريطة<" + keyType_->arabicName() + "، " + valueType_->arabicName() + ">";
                return "خريطة";
            }
            std::string englishName() const override
            {
                if (keyType_ && valueType_)
                    return "Map<" + keyType_->englishName() + ", " + valueType_->englishName() + ">";
                return "Map";
            }

            bool equals(const SadType *other) const override
            {
                if (!other || other->getKind() != SadTypeKind::Map)
                    return false;
                auto o = static_cast<const SadMapType *>(other);
                if (!keyType_ && !o->keyType_ && !valueType_ && !o->valueType_)
                    return true;
                if (!keyType_ || !o->keyType_ || !valueType_ || !o->valueType_)
                    return false;
                return keyType_->equals(o->keyType_.get()) && valueType_->equals(o->valueType_.get());
            }

            size_t sizeInBytes() const override { return sizeof(void *); }

            std::vector<SadTypePtr> getTypeParams() const override
            {
                std::vector<SadTypePtr> result;
                if (keyType_)
                    result.push_back(keyType_);
                if (valueType_)
                    result.push_back(valueType_);
                return result;
            }

        private:
            SadTypePtr keyType_;
            SadTypePtr valueType_;
        };

        // ─────────────────────────────────────────────────────────────────────────────────
        //  نوع صف / Tuple type: (T1, T2, ...)
        // ─────────────────────────────────────────────────────────────────────────────────
        class SadTupleType : public SadType
        {
        public:
            explicit SadTupleType(std::vector<SadTypePtr> elements = {})
                : SadType(SadTypeKind::Tuple), elements_(std::move(elements)) {}

            const std::vector<SadTypePtr> &getElements() const { return elements_; }
            size_t arity() const { return elements_.size(); }

            std::string arabicName() const override
            {
                std::string r = "(";
                for (size_t i = 0; i < elements_.size(); ++i)
                {
                    if (i > 0)
                        r += "، ";
                    r += elements_[i] ? elements_[i]->arabicName() : "أي";
                }
                return r + ")";
            }
            std::string englishName() const override
            {
                std::string r = "(";
                for (size_t i = 0; i < elements_.size(); ++i)
                {
                    if (i > 0)
                        r += ", ";
                    r += elements_[i] ? elements_[i]->englishName() : "Any";
                }
                return r + ")";
            }

            bool equals(const SadType *other) const override
            {
                if (!other || other->getKind() != SadTypeKind::Tuple)
                    return false;
                auto o = static_cast<const SadTupleType *>(other);
                if (elements_.size() != o->elements_.size())
                    return false;
                for (size_t i = 0; i < elements_.size(); ++i)
                {
                    if (!elements_[i] || !o->elements_[i])
                        continue;
                    if (!elements_[i]->equals(o->elements_[i].get()))
                        return false;
                }
                return true;
            }

            size_t sizeInBytes() const override { return sizeof(void *); }

            std::vector<SadTypePtr> getTypeParams() const override { return elements_; }

        private:
            std::vector<SadTypePtr> elements_;
        };

        // ─────────────────────────────────────────────────────────────────────────────────
        //  نوع دالة / Function type: (P1, P2, ...) -> Return
        // ─────────────────────────────────────────────────────────────────────────────────
        class SadFunctionType : public SadType
        {
        public:
            SadFunctionType(std::vector<SadTypePtr> paramTypes,
                            SadTypePtr returnType = nullptr,
                            bool isClosure = false)
                : SadType(isClosure ? SadTypeKind::Closure : SadTypeKind::Function),
                  paramTypes_(std::move(paramTypes)), returnType_(std::move(returnType)) {}

            const std::vector<SadTypePtr> &getParamTypes() const { return paramTypes_; }
            SadTypePtr getReturnType() const { return returnType_; }
            size_t paramCount() const { return paramTypes_.size(); }

            std::string arabicName() const override
            {
                std::string r = "(";
                for (size_t i = 0; i < paramTypes_.size(); ++i)
                {
                    if (i > 0)
                        r += "، ";
                    r += paramTypes_[i] ? paramTypes_[i]->arabicName() : "أي";
                }
                r += ") -> ";
                r += returnType_ ? returnType_->arabicName() : "فراغ";
                return r;
            }
            std::string englishName() const override
            {
                std::string r = "(";
                for (size_t i = 0; i < paramTypes_.size(); ++i)
                {
                    if (i > 0)
                        r += ", ";
                    r += paramTypes_[i] ? paramTypes_[i]->englishName() : "Any";
                }
                r += ") -> ";
                r += returnType_ ? returnType_->englishName() : "Void";
                return r;
            }

            bool equals(const SadType *other) const override
            {
                if (!other)
                    return false;
                if (other->getKind() != SadTypeKind::Function && other->getKind() != SadTypeKind::Closure)
                    return false;
                auto o = static_cast<const SadFunctionType *>(other);
                if (paramTypes_.size() != o->paramTypes_.size())
                    return false;
                for (size_t i = 0; i < paramTypes_.size(); ++i)
                {
                    if (paramTypes_[i] && o->paramTypes_[i])
                        if (!paramTypes_[i]->equals(o->paramTypes_[i].get()))
                            return false;
                }
                if (returnType_ && o->returnType_)
                    return returnType_->equals(o->returnType_.get());
                return !returnType_ && !o->returnType_;
            }

            size_t sizeInBytes() const override { return sizeof(void *); }

            std::vector<SadTypePtr> getTypeParams() const override
            {
                std::vector<SadTypePtr> result = paramTypes_;
                if (returnType_)
                    result.push_back(returnType_);
                return result;
            }

        private:
            std::vector<SadTypePtr> paramTypes_;
            SadTypePtr returnType_;
        };

        // ─────────────────────────────────────────────────────────────────────────────────
        //  معلومات حقل / Field info for class/struct
        // ─────────────────────────────────────────────────────────────────────────────────
        struct SadFieldInfo
        {
            std::string name;
            SadTypePtr type;
            bool isPublic = true;
        };

        // ─────────────────────────────────────────────────────────────────────────────────
        //  نوع صنف / Class type
        // ─────────────────────────────────────────────────────────────────────────────────
        class SadClassType : public SadType
        {
        public:
            SadClassType(const std::string &name, SadTypePtr parent = nullptr, bool isStruct = false)
                : SadType(isStruct ? SadTypeKind::Struct : SadTypeKind::Class),
                  name_(name), parent_(std::move(parent)) {}

            const std::string &getName() const { return name_; }
            SadTypePtr getParent() const { return parent_; }
            const std::vector<SadFieldInfo> &getFields() const { return fields_; }
            const std::vector<SadTypePtr> &getTraits() const { return traits_; }

            void addField(const std::string &name, SadTypePtr type, bool isPublic = true)
            {
                fields_.push_back({name, std::move(type), isPublic});
            }
            void addTrait(SadTypePtr trait)
            {
                traits_.push_back(std::move(trait));
            }

            std::string arabicName() const override { return name_; }
            std::string englishName() const override { return name_; }

            bool equals(const SadType *other) const override
            {
                if (!other)
                    return false;
                if (other->getKind() != SadTypeKind::Class && other->getKind() != SadTypeKind::Struct)
                    return false;
                return name_ == static_cast<const SadClassType *>(other)->name_;
            }

            bool isSubtypeOf(const SadType *parent) const override
            {
                if (equals(parent))
                    return true;
                if (parent_ && parent_->isSubtypeOf(parent))
                    return true;
                for (const auto &t : traits_)
                    if (t && t->equals(parent))
                        return true;
                return false;
            }

            size_t sizeInBytes() const override { return sizeof(void *); }

        private:
            std::string name_;
            SadTypePtr parent_;
            std::vector<SadFieldInfo> fields_;
            std::vector<SadTypePtr> traits_;
        };

        // ─────────────────────────────────────────────────────────────────────────────────
        //  نوع تعداد / Enum type
        // ─────────────────────────────────────────────────────────────────────────────────
        class SadEnumType : public SadType
        {
        public:
            struct Variant
            {
                std::string name;
                int64_t value;
            };

            explicit SadEnumType(const std::string &name, std::vector<Variant> variants = {})
                : SadType(SadTypeKind::Enum), name_(name), variants_(std::move(variants)) {}

            const std::string &getName() const { return name_; }
            const std::vector<Variant> &getVariants() const { return variants_; }
            void addVariant(const std::string &name, int64_t value) { variants_.push_back({name, value}); }

            std::string arabicName() const override { return name_; }
            std::string englishName() const override { return name_; }

            bool equals(const SadType *other) const override
            {
                if (!other || other->getKind() != SadTypeKind::Enum)
                    return false;
                return name_ == static_cast<const SadEnumType *>(other)->name_;
            }

            size_t sizeInBytes() const override { return 8; }

        private:
            std::string name_;
            std::vector<Variant> variants_;
        };

        // ─────────────────────────────────────────────────────────────────────────────────
        //  نوع سمة / Trait type
        // ─────────────────────────────────────────────────────────────────────────────────
        class SadTraitType : public SadType
        {
        public:
            struct RequiredMethod
            {
                std::string name;
                SadTypePtr signature;
            };

            explicit SadTraitType(const std::string &name, std::vector<RequiredMethod> methods = {})
                : SadType(SadTypeKind::Trait), name_(name), methods_(std::move(methods)) {}

            const std::string &getName() const { return name_; }
            const std::vector<RequiredMethod> &getMethods() const { return methods_; }
            void addMethod(const std::string &name, SadTypePtr sig) { methods_.push_back({name, std::move(sig)}); }

            std::string arabicName() const override { return name_; }
            std::string englishName() const override { return name_; }

            bool equals(const SadType *other) const override
            {
                if (!other || other->getKind() != SadTypeKind::Trait)
                    return false;
                return name_ == static_cast<const SadTraitType *>(other)->name_;
            }

        private:
            std::string name_;
            std::vector<RequiredMethod> methods_;
        };

        // ─────────────────────────────────────────────────────────────────────────────────
        //  نوع اتحاد / Union type: T1 | T2 | ...
        // ─────────────────────────────────────────────────────────────────────────────────
        class SadUnionType : public SadType
        {
        public:
            explicit SadUnionType(std::vector<SadTypePtr> alternatives)
                : SadType(SadTypeKind::Union), alternatives_(std::move(alternatives)) {}

            const std::vector<SadTypePtr> &getAlternatives() const { return alternatives_; }

            bool contains(const SadType *type) const
            {
                for (const auto &a : alternatives_)
                    if (a && a->equals(type))
                        return true;
                return false;
            }

            std::string arabicName() const override
            {
                std::string r;
                for (size_t i = 0; i < alternatives_.size(); ++i)
                {
                    if (i > 0)
                        r += " | ";
                    r += alternatives_[i] ? alternatives_[i]->arabicName() : "أي";
                }
                return r;
            }
            std::string englishName() const override
            {
                std::string r;
                for (size_t i = 0; i < alternatives_.size(); ++i)
                {
                    if (i > 0)
                        r += " | ";
                    r += alternatives_[i] ? alternatives_[i]->englishName() : "Any";
                }
                return r;
            }

            bool equals(const SadType *other) const override
            {
                if (!other || other->getKind() != SadTypeKind::Union)
                    return false;
                auto o = static_cast<const SadUnionType *>(other);
                if (alternatives_.size() != o->alternatives_.size())
                    return false;
                for (const auto &a : alternatives_)
                    if (!o->contains(a.get()))
                        return false;
                return true;
            }

        private:
            std::vector<SadTypePtr> alternatives_;
        };

        // ─────────────────────────────────────────────────────────────────────────────────
        //  نوع اختياري / Optional type: T?
        // ─────────────────────────────────────────────────────────────────────────────────
        class SadOptionalType : public SadType
        {
        public:
            explicit SadOptionalType(SadTypePtr innerType)
                : SadType(SadTypeKind::Optional), innerType_(std::move(innerType)) {}

            SadTypePtr getInnerType() const { return innerType_; }

            std::string arabicName() const override
            {
                return (innerType_ ? innerType_->arabicName() : "أي") + "?";
            }
            std::string englishName() const override
            {
                return (innerType_ ? innerType_->englishName() : "Any") + "?";
            }

            bool equals(const SadType *other) const override
            {
                if (!other || other->getKind() != SadTypeKind::Optional)
                    return false;
                auto o = static_cast<const SadOptionalType *>(other);
                if (!innerType_ && !o->innerType_)
                    return true;
                if (!innerType_ || !o->innerType_)
                    return false;
                return innerType_->equals(o->innerType_.get());
            }

        private:
            SadTypePtr innerType_;

        public:
            std::vector<SadTypePtr> getTypeParams() const override
            {
                if (innerType_)
                    return {innerType_};
                return {};
            }
        };

        // ─────────────────────────────────────────────────────────────────────────────────
        //  نوع نتيجة / Result type: نتيجة<T, E> (نجاح T أو خطأ E)
        //  (AR) [S-TS-P3] نوع جبري يمثّل إمّا قيمة نجاح من النوع T أو خطأ من النوع E.
        //       على نسق SadOptionalType لكن بنوعين داخليين.
        //  (EN) [S-TS-P3] Algebraic type representing either a success value (T) or an
        //       error (E). Modeled on SadOptionalType but with two inner types.
        // ─────────────────────────────────────────────────────────────────────────────────
        class SadResultType : public SadType
        {
        public:
            SadResultType(SadTypePtr okType, SadTypePtr errType)
                : SadType(SadTypeKind::Result), okType_(std::move(okType)), errType_(std::move(errType)) {}

            SadTypePtr getOkType() const { return okType_; }
            SadTypePtr getErrType() const { return errType_; }

            std::string arabicName() const override
            {
                return "نتيجة<" + (okType_ ? okType_->arabicName() : "أي") + "، " +
                       (errType_ ? errType_->arabicName() : "خطأ") + ">";
            }
            std::string englishName() const override
            {
                return "Result<" + (okType_ ? okType_->englishName() : "Any") + ", " +
                       (errType_ ? errType_->englishName() : "Error") + ">";
            }

            bool equals(const SadType *other) const override
            {
                if (!other || other->getKind() != SadTypeKind::Result)
                    return false;
                auto o = static_cast<const SadResultType *>(other);
                auto eq = [](const SadTypePtr &a, const SadTypePtr &b) {
                    if (!a && !b) return true;
                    if (!a || !b) return false;
                    return a->equals(b.get());
                };
                return eq(okType_, o->okType_) && eq(errType_, o->errType_);
            }

            std::vector<SadTypePtr> getTypeParams() const override
            {
                std::vector<SadTypePtr> params;
                if (okType_) params.push_back(okType_);
                if (errType_) params.push_back(errType_);
                return params;
            }

        private:
            SadTypePtr okType_;  ///< (AR) نوع قيمة النجاح T / (EN) success value type
            SadTypePtr errType_; ///< (AR) نوع الخطأ E / (EN) error type
        };

        // ─────────────────────────────────────────────────────────────────────────────────
        //  نوع عام / Generic type parameter: T, U, K, V...
        // ─────────────────────────────────────────────────────────────────────────────────
        class SadGenericType : public SadType
        {
        public:
            explicit SadGenericType(const std::string &name, SadTypePtr constraint = nullptr)
                : SadType(SadTypeKind::Generic), name_(name), constraint_(std::move(constraint)) {}

            const std::string &getName() const { return name_; }
            SadTypePtr getConstraint() const { return constraint_; }

            std::string arabicName() const override { return name_; }
            std::string englishName() const override { return name_; }

            bool equals(const SadType *other) const override
            {
                if (!other || other->getKind() != SadTypeKind::Generic)
                    return false;
                return name_ == static_cast<const SadGenericType *>(other)->name_;
            }

        private:
            std::string name_;
            SadTypePtr constraint_;
        };

        // ─────────────────────────────────────────────────────────────────────────────────
        //  اسم مستعار / Type alias
        // ─────────────────────────────────────────────────────────────────────────────────
        class SadTypeAlias : public SadType
        {
        public:
            SadTypeAlias(const std::string &name, SadTypePtr underlying)
                : SadType(SadTypeKind::TypeAlias), name_(name), underlying_(std::move(underlying)) {}

            const std::string &getName() const { return name_; }
            SadTypePtr getUnderlying() const { return underlying_; }

            std::string arabicName() const override { return name_; }
            std::string englishName() const override { return name_; }

            bool equals(const SadType *other) const override
            {
                if (underlying_ && underlying_->equals(other))
                    return true;
                if (!other || other->getKind() != SadTypeKind::TypeAlias)
                    return false;
                return name_ == static_cast<const SadTypeAlias *>(other)->name_;
            }

            bool isAssignableTo(const SadType *target) const override
            {
                if (underlying_)
                    return underlying_->isAssignableTo(target);
                return SadType::isAssignableTo(target);
            }

        private:
            std::string name_;
            SadTypePtr underlying_;
        };

        // ─────────────────────────────────────────────────────────────────────────────────
        //  نوع مرجع / Reference type: &T, &mut T
        // ─────────────────────────────────────────────────────────────────────────────────
        class SadReferenceType : public SadType
        {
        public:
            SadReferenceType(SadTypePtr pointee, bool isMutable = false)
                : SadType(isMutable ? SadTypeKind::MutableRef : SadTypeKind::Reference),
                  pointee_(std::move(pointee)) {}

            SadTypePtr getPointee() const { return pointee_; }
            bool isMutable() const { return kind_ == SadTypeKind::MutableRef; }

            std::string arabicName() const override
            {
                std::string pfx = isMutable() ? "&متغير " : "&";
                return pfx + (pointee_ ? pointee_->arabicName() : "أي");
            }
            std::string englishName() const override
            {
                std::string pfx = isMutable() ? "&mut " : "&";
                return pfx + (pointee_ ? pointee_->englishName() : "Any");
            }

            bool equals(const SadType *other) const override
            {
                if (!other || other->getKind() != kind_)
                    return false;
                auto o = static_cast<const SadReferenceType *>(other);
                if (!pointee_ && !o->pointee_)
                    return true;
                if (!pointee_ || !o->pointee_)
                    return false;
                return pointee_->equals(o->pointee_.get());
            }

            size_t sizeInBytes() const override { return sizeof(void *); }

            std::vector<SadTypePtr> getTypeParams() const override
            {
                if (pointee_)
                    return {pointee_};
                return {};
            }

        private:
            SadTypePtr pointee_;
        };

        // ─────────────────────────────────────────────────────────────────────────────────
        //  نوع مستقبل / Future type: مستقبل<T>
        // ─────────────────────────────────────────────────────────────────────────────────
        class SadFutureType : public SadType
        {
        public:
            explicit SadFutureType(SadTypePtr valueType = nullptr)
                : SadType(SadTypeKind::Future), valueType_(std::move(valueType)) {}

            SadTypePtr getValueType() const { return valueType_; }

            std::string arabicName() const override
            {
                return valueType_ ? "مستقبل<" + valueType_->arabicName() + ">" : "مستقبل";
            }
            std::string englishName() const override
            {
                return valueType_ ? "Future<" + valueType_->englishName() + ">" : "Future";
            }

            bool equals(const SadType *other) const override
            {
                if (!other || other->getKind() != SadTypeKind::Future)
                    return false;
                auto o = static_cast<const SadFutureType *>(other);
                if (!valueType_ && !o->valueType_)
                    return true;
                if (!valueType_ || !o->valueType_)
                    return false;
                return valueType_->equals(o->valueType_.get());
            }

            std::vector<SadTypePtr> getTypeParams() const override
            {
                if (valueType_)
                    return {valueType_};
                return {};
            }

        private:
            SadTypePtr valueType_;
        };

        // ─────────────────────────────────────────────────────────────────────────────────
        //  نوع مولّد / Generator type: مولّد<T>
        // ─────────────────────────────────────────────────────────────────────────────────
        class SadGeneratorType : public SadType
        {
        public:
            explicit SadGeneratorType(SadTypePtr yieldType = nullptr)
                : SadType(SadTypeKind::Generator), yieldType_(std::move(yieldType)) {}

            SadTypePtr getYieldType() const { return yieldType_; }

            std::string arabicName() const override
            {
                return yieldType_ ? "مولّد<" + yieldType_->arabicName() + ">" : "مولّد";
            }
            std::string englishName() const override
            {
                return yieldType_ ? "Generator<" + yieldType_->englishName() + ">" : "Generator";
            }

            bool equals(const SadType *other) const override
            {
                if (!other || other->getKind() != SadTypeKind::Generator)
                    return false;
                auto o = static_cast<const SadGeneratorType *>(other);
                if (!yieldType_ && !o->yieldType_)
                    return true;
                if (!yieldType_ || !o->yieldType_)
                    return false;
                return yieldType_->equals(o->yieldType_.get());
            }

            std::vector<SadTypePtr> getTypeParams() const override
            {
                if (yieldType_)
                    return {yieldType_};
                return {};
            }

        private:
            SadTypePtr yieldType_;
        };

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  الجزء ③: SadTypeRegistry — Singleton لتخزين وإنشاء الأنواع
        //  Part ③: SadTypeRegistry — Singleton for type storage and creation
        //
        // █████████████████████████████████████████████████████████████████████████████████

        /**
         * @brief (AR) سجل الأنواع — المصدر الوحيد لإنشاء الأنواع
         * @brief (EN) Type registry — single source for type creation
         *
         * (AR) الأنواع البدائية والخاصة مُخزَّنة مسبقاً (interning) — مقارنة بالمؤشر ممكنة.
         *      الأنواع المركبة تُنشأ عند الطلب عبر factory methods.
         *      thread-safe عبر mutex داخلي.
         */
        class SadTypeRegistry
        {
        public:
            static SadTypeRegistry &instance()
            {
                static SadTypeRegistry inst;
                return inst;
            }

            // ─── أنواع بدائية مُخزَّنة مسبقاً / Pre-interned primitives ───
            SadTypePtr getVoid() const { return void_; }
            SadTypePtr getNull() const { return null_; } // (AR) عدم — S-TS-P1
            SadTypePtr getInteger() const { return integer_; }
            SadTypePtr getFloat() const { return float_; }
            SadTypePtr getBoolean() const { return boolean_; }
            SadTypePtr getString() const { return string_; }
            SadTypePtr getByte() const { return byte_; }

            // ─── أنواع خاصة مُخزَّنة مسبقاً / Pre-interned specials ───
            SadTypePtr getAny() const { return any_; }
            SadTypePtr getNever() const { return never_; }
            SadTypePtr getUnknown() const { return unknown_; }
            SadTypePtr getError() const { return error_; }

            // ─── الحصول على نوع بدائي/خاص بواسطة Kind / Get by Kind ───
            SadTypePtr getByKind(SadTypeKind kind) const
            {
                switch (kind)
                {
                case SadTypeKind::Void:
                    return void_;
                case SadTypeKind::Integer:
                    return integer_;
                case SadTypeKind::Float:
                    return float_;
                case SadTypeKind::Boolean:
                    return boolean_;
                case SadTypeKind::String:
                    return string_;
                case SadTypeKind::Byte:
                    return byte_;
                case SadTypeKind::Any:
                    return any_;
                case SadTypeKind::Never:
                    return never_;
                case SadTypeKind::Unknown:
                    return unknown_;
                case SadTypeKind::Error:
                    return error_;
                default:
                    return nullptr;
                }
            }

            // ─── أنواع مركبة / Composite types ───
            SadTypePtr makeArray(SadTypePtr elem = nullptr)
            {
                return std::make_shared<SadArrayType>(std::move(elem));
            }

            SadTypePtr makeMap(SadTypePtr key = nullptr, SadTypePtr val = nullptr)
            {
                return std::make_shared<SadMapType>(std::move(key), std::move(val));
            }

            SadTypePtr makeTuple(std::vector<SadTypePtr> elems)
            {
                return std::make_shared<SadTupleType>(std::move(elems));
            }

            // ─── أنواع وظيفية / Function types ───
            SadTypePtr makeFunction(std::vector<SadTypePtr> params, SadTypePtr ret = nullptr)
            {
                return std::make_shared<SadFunctionType>(std::move(params), std::move(ret));
            }

            SadTypePtr makeClosure(std::vector<SadTypePtr> params, SadTypePtr ret = nullptr)
            {
                return std::make_shared<SadFunctionType>(std::move(params), std::move(ret), true);
            }

            // ─── أنواع كائنية / OOP types ───
            SadTypePtr getOrCreateClass(const std::string &name, SadTypePtr parent = nullptr, bool isStruct = false)
            {
                std::lock_guard<std::mutex> lock(mutex_);
                auto it = classTypes_.find(name);
                if (it != classTypes_.end())
                    return it->second;
                auto cls = std::make_shared<SadClassType>(name, std::move(parent), isStruct);
                classTypes_[name] = cls;
                return cls;
            }

            SadTypePtr getClass(const std::string &name) const
            {
                std::lock_guard<std::mutex> lock(mutex_);
                auto it = classTypes_.find(name);
                return it != classTypes_.end() ? it->second : nullptr;
            }

            SadTypePtr makeEnum(const std::string &name)
            {
                return std::make_shared<SadEnumType>(name);
            }

            SadTypePtr makeTrait(const std::string &name)
            {
                return std::make_shared<SadTraitType>(name);
            }

            // ─── أنواع متقدمة / Advanced types ───
            SadTypePtr makeUnion(std::vector<SadTypePtr> alts)
            {
                return std::make_shared<SadUnionType>(std::move(alts));
            }

            SadTypePtr makeOptional(SadTypePtr inner)
            {
                return std::make_shared<SadOptionalType>(std::move(inner));
            }

            // (AR) [S-TS-P3] إنشاء نوع نتيجة نتيجة<T, E> (نجاح T أو خطأ E)
            // (EN) [S-TS-P3] Create a Result<T, E> type (success T or error E)
            SadTypePtr makeResult(SadTypePtr okType, SadTypePtr errType)
            {
                return std::make_shared<SadResultType>(std::move(okType), std::move(errType));
            }

            SadTypePtr makeGeneric(const std::string &name, SadTypePtr constraint = nullptr)
            {
                return std::make_shared<SadGenericType>(name, std::move(constraint));
            }

            SadTypePtr makeTypeAlias(const std::string &name, SadTypePtr underlying)
            {
                return std::make_shared<SadTypeAlias>(name, std::move(underlying));
            }

            SadTypePtr makeReference(SadTypePtr pointee, bool isMutable = false)
            {
                return std::make_shared<SadReferenceType>(std::move(pointee), isMutable);
            }

            SadTypePtr makeFuture(SadTypePtr val = nullptr)
            {
                return std::make_shared<SadFutureType>(std::move(val));
            }

            SadTypePtr makeGenerator(SadTypePtr yield = nullptr)
            {
                return std::make_shared<SadGeneratorType>(std::move(yield));
            }

            // ─── من اسم عربي / From Arabic name ───
            SadTypePtr fromArabicName(const std::string &name) const
            {
                if (name == "فراغ" || name == "عدم" || name == "لاشيء")
                    return void_;
                if (name == "رقم")
                    return integer_;
                if (name == "عشري")
                    return float_;
                if (name == "منطقي")
                    return boolean_;
                if (name == "نص")
                    return string_;
                if (name == "بايت")
                    return byte_;
                if (name == "أي")
                    return any_;
                if (name == "مصفوفة")
                    return std::make_shared<SadArrayType>();
                if (name == "خريطة")
                    return std::make_shared<SadMapType>();
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    auto it = classTypes_.find(name);
                    if (it != classTypes_.end())
                        return it->second;
                }
                return nullptr;
            }

            // ─── مسح أنواع المستخدم (للاختبارات) / Clear user types ───
            void clearUserTypes()
            {
                std::lock_guard<std::mutex> lock(mutex_);
                classTypes_.clear();
            }

        private:
            SadTypeRegistry()
            {
                void_ = std::make_shared<SadPrimitiveType>(SadTypeKind::Void);
                null_ = std::make_shared<SadPrimitiveType>(SadTypeKind::Null); // (AR) عدم — S-TS-P1
                integer_ = std::make_shared<SadPrimitiveType>(SadTypeKind::Integer);
                float_ = std::make_shared<SadPrimitiveType>(SadTypeKind::Float);
                boolean_ = std::make_shared<SadPrimitiveType>(SadTypeKind::Boolean);
                string_ = std::make_shared<SadPrimitiveType>(SadTypeKind::String);
                byte_ = std::make_shared<SadPrimitiveType>(SadTypeKind::Byte);
                any_ = std::make_shared<SadSpecialType>(SadTypeKind::Any);
                never_ = std::make_shared<SadSpecialType>(SadTypeKind::Never);
                unknown_ = std::make_shared<SadSpecialType>(SadTypeKind::Unknown);
                error_ = std::make_shared<SadSpecialType>(SadTypeKind::Error);
            }

            SadTypeRegistry(const SadTypeRegistry &) = delete;
            SadTypeRegistry &operator=(const SadTypeRegistry &) = delete;

            SadTypePtr void_, null_, integer_, float_, boolean_, string_, byte_;
            SadTypePtr any_, never_, unknown_, error_;
            mutable std::mutex mutex_;
            std::unordered_map<std::string, SadTypePtr> classTypes_;
        };

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  الجزء ④: التحويلات والتوافقية (inline implementations)
        //  Part ④: Conversions and compatibility (inline implementations)
        //
        // █████████████████████████████████████████████████████████████████████████████████

        // ─── SadType::isAssignableTo ───
        inline bool SadType::isAssignableTo(const SadType *target) const
        {
            if (!target)
                return false;
            if (equals(target))
                return true;
            if (target->getKind() == SadTypeKind::Any)
                return true;
            if (kind_ == SadTypeKind::Never)
                return true;

            // (AR) الهدف Optional<T> → يقبل عدم (Null) أو T — S-TS-P4 (AC7)
            //      (Null <: T?‎ و T <: T?‎). Void يبقى مقبولًا للتوافق الخلفي مؤقتًا.
            // (EN) Target Optional<T> → accepts Null or T (Null <: T?, T <: T?) — S-TS-P4.
            if (target->getKind() == SadTypeKind::Optional)
            {
                auto opt = static_cast<const SadOptionalType *>(target);
                if (kind_ == SadTypeKind::Null || kind_ == SadTypeKind::Void)
                    return true;
                if (opt->getInnerType() && isAssignableTo(opt->getInnerType().get()))
                    return true;
            }

            // (AR) الهدف Result<T,E> → المصدر Result بأنواع نجاح/خطأ متوافقة (تغايُر) — S-TS-P3
            // (EN) Target Result<T,E> → source Result with compatible ok/err types (covariant)
            if (target->getKind() == SadTypeKind::Result && kind_ == SadTypeKind::Result)
            {
                auto tr = static_cast<const SadResultType *>(target);
                auto sr = static_cast<const SadResultType *>(this);
                auto compat = [](const SadTypePtr &s, const SadTypePtr &t) {
                    if (!s || !t)
                        return true; // (AR) نوع مفتوح يُقبل
                    return s->isAssignableTo(t.get());
                };
                if (compat(sr->getOkType(), tr->getOkType()) && compat(sr->getErrType(), tr->getErrType()))
                    return true;
            }

            // (AR) الهدف Union → إذا كان المصدر أحد البدائل
            if (target->getKind() == SadTypeKind::Union)
            {
                auto u = static_cast<const SadUnionType *>(target);
                for (const auto &a : u->getAlternatives())
                    if (a && isAssignableTo(a.get()))
                        return true;
            }

            if (coercesTo(target))
                return true;
            if (isSubtypeOf(target))
                return true;

            // (AR) TypeAlias → فحص النوع الأصلي
            if (target->getKind() == SadTypeKind::TypeAlias)
            {
                auto alias = static_cast<const SadTypeAlias *>(target);
                if (alias->getUnderlying())
                    return isAssignableTo(alias->getUnderlying().get());
            }

            return false;
        }

        // ─── SadType::coercesTo ───
        inline bool SadType::coercesTo(const SadType *target) const
        {
            if (!target)
                return false;
            auto src = kind_, dst = target->getKind();
            if (src == SadTypeKind::Integer && dst == SadTypeKind::Float)
                return true; // رقم → عشري
            if (src == SadTypeKind::Byte && dst == SadTypeKind::Integer)
                return true; // بايت → رقم
            if (src == SadTypeKind::Byte && dst == SadTypeKind::Float)
                return true; // بايت → عشري
            return false;
        }

        // ─── SadArrayType::isAssignableTo ───
        inline bool SadArrayType::isAssignableTo(const SadType *target) const
        {
            if (!target)
                return false;
            if (target->getKind() == SadTypeKind::Array)
            {
                auto t = static_cast<const SadArrayType *>(target);
                if (!t->getElementType())
                    return true; // الهدف بدون نوع عنصر
                if (!getElementType())
                    return true; // المصدر بدون نوع عنصر
                return getElementType()->isAssignableTo(t->getElementType().get());
            }
            return SadType::isAssignableTo(target);
        }

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  الجزء ⑤: أسماء بديلة للتوافق الخلفي
        //  Part ⑤: Backward-compatibility aliases
        //
        // █████████████████████████████████████████████████████████████████████████████████

        // (AR) لا حاجة لاستيراد ملفات أخرى — هذا الملف هو كل شيء
        // (EN) No need to import other files — this file IS everything

    } // namespace Types
} // namespace Sad

// ═══════════════════════════════════════════════════════════════════════════════════
// (AR) أسماء بديلة قديمة — تحافظ على التوافق مع الكود الموجود
//      بـ include "sad_type_system.h" يُصبح:
//        - sad_type_kinds.h → غير مطلوب (SadTypeKind موجود هنا)
//        - sad_type.h → غير مطلوب (SadType وأصنافه موجودة هنا)
//        - sad_type_registry.h → غير مطلوب (SadTypeRegistry موجود هنا)
// ═══════════════════════════════════════════════════════════════════════════════════
