// ════════════════════════════════════════════════════════════════════════════════
// ملف: type.h
// File: type.h
//
// الوصف: تعريف النوع الأساسي المجرد لنظام الأنواع
// Description: Base abstract type definition for the type system
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 2 يناير 2026
// Date: January 2, 2026
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_COMPILER_TYPE_SYSTEM_TYPE_H
#define SAD_COMPILER_TYPE_SYSTEM_TYPE_H

#include <memory>            // لـ shared_ptr / For shared_ptr
#include <string>            // لـ string / For string
#include <vector>            // لـ vector / For vector
#include <unordered_map>     // لـ unordered_map / For unordered_map
#include "sad_type_system.h" // (AR) نظام الأنواع الموحد / (EN) Unified type system

namespace Sad
{
    namespace TypeSystem
    {

        // ════════════════════════════════════════════════════════════════════════════════
        // (AR) جلب SadTypeKind من نظام الأنواع الموحد
        // (EN) Bring SadTypeKind from the unified type system
        // ════════════════════════════════════════════════════════════════════════════════
        using Sad::Types::SadTypeKind;

        // ════════════════════════════════════════════════════════════════════════════════
        // الصنف الأساسي: Type
        // Base Class: Type
        //
        // الوصف: النوع الأساسي المجرد الذي ترث منه جميع الأنواع الأخرى
        // Description: Abstract base type from which all other types inherit
        // ════════════════════════════════════════════════════════════════════════════════

        class Type
        {
        public:
            // ═══════════════════════════════════════════════════════════════════════════
            // المُنشئ / Constructor
            // ═══════════════════════════════════════════════════════════════════════════

            // منشئ مع نوع محدد / Constructor with specific kind
            explicit Type(SadTypeKind kind);

            // المُنشئ الافتراضي محذوف / Default constructor deleted
            Type() = delete;

            // ═══════════════════════════════════════════════════════════════════════════
            // المُدمّر الافتراضي / Virtual Destructor
            // ═══════════════════════════════════════════════════════════════════════════

            virtual ~Type() = default;

            // ═══════════════════════════════════════════════════════════════════════════
            // الدوال النقية الافتراضية / Pure Virtual Functions
            // ═══════════════════════════════════════════════════════════════════════════

            // الحصول على اسم النوع بالعربية / Get type name in Arabic
            virtual std::string getArabicName() const = 0;

            // الحصول على اسم النوع بالإنجليزية / Get type name in English
            virtual std::string getEnglishName() const = 0;

            // تحويل النوع إلى نص تمثيلي / Convert type to string representation
            virtual std::string toString() const = 0;

            // التحقق من التساوي مع نوع آخر / Check equality with another type
            virtual bool equals(const Type *other) const = 0;

            // استنساخ النوع / Clone the type
            virtual std::shared_ptr<Type> clone() const = 0;

            // ═══════════════════════════════════════════════════════════════════════════
            // الدوال العامة / Public Methods
            // ═══════════════════════════════════════════════════════════════════════════

            // الحصول على نوع النوع / Get the type kind
            SadTypeKind getKind() const { return kind_; }

            // التحقق من نوع معين / Check if specific kind
            bool isVoid() const { return kind_ == SadTypeKind::Void; }
            bool isInteger() const { return kind_ == SadTypeKind::Integer; }
            bool isFloat() const { return kind_ == SadTypeKind::Float; }
            bool isBoolean() const { return kind_ == SadTypeKind::Boolean; }
            bool isString() const { return kind_ == SadTypeKind::String; }
            bool isArray() const { return kind_ == SadTypeKind::Array; }
            bool isDictionary() const { return kind_ == SadTypeKind::Map; }
            bool isTuple() const { return kind_ == SadTypeKind::Tuple; }
            bool isFunction() const { return kind_ == SadTypeKind::Function; }
            bool isClass() const { return kind_ == SadTypeKind::Class; }
            bool isInterface() const { return kind_ == SadTypeKind::Trait; }
            bool isGeneric() const { return kind_ == SadTypeKind::Generic; }
            bool isTypeParameter() const { return kind_ == SadTypeKind::TypeParameter; }
            bool isUnion() const { return kind_ == SadTypeKind::Union; }
            bool isIntersection() const { return kind_ == SadTypeKind::Intersection; }
            bool isOptional() const { return kind_ == SadTypeKind::Optional; }
            bool isAny() const { return kind_ == SadTypeKind::Any; }
            bool isNever() const { return kind_ == SadTypeKind::Never; }
            bool isUnknown() const { return kind_ == SadTypeKind::Unknown; }
            bool isError() const { return kind_ == SadTypeKind::Error; }

            // التحقق من كون النوع بدائياً / Check if primitive type
            // (AR) 🔑 وأسرةُ الأعراضِ الثمانيةِ بدائيّةٌ كإخوتِها — وقد أغفلتُها
            //      أوّلَ مرّةٍ فرمى منشئُ `PrimitiveType` استثناءً على أوّلِ
            //      `رقم`، فابتلعَه الفاحصُ وأخرجَ «خطأ داخليّ» **على كلِّ
            //      برنامجٍ فيه عرضٌ مُعلَنٌ** — أي أنّ توسيعَ الرؤيةِ بلا توسيعِ
            //      الإذنِ حوّلَ العمى إلى عطبٍ شامل. والدرسُ أنّ حارسًا يعتمدُ
            //      قائمةَ أسماءٍ يُعادُ فتحُه كلَّما اتّسعَ ما يمرُّ عليه.
            // (EN) The eight-width family is primitive too. Omitting it here made
            //      PrimitiveType's constructor throw on the first رقم, surfacing
            //      as "internal type-checker error" on every program with a declared
            //      width — widening what the checker sees without widening what it
            //      permits turned blindness into a total failure.
            bool isPrimitive() const
            {
                return kind_ == SadTypeKind::Integer ||
                       kind_ == SadTypeKind::Float ||
                       kind_ == SadTypeKind::Boolean ||
                       kind_ == SadTypeKind::String ||
                       ::Sad::Types::sadTypeKindIsNumeric(kind_);
            }

            // التحقق من كون النوع رقمياً / Check if numeric type
            // (AR) 🔑 كانت هذه نسخةً خامسةً مكتوبةً باليدِ لسؤالِ «أعدديٌّ هذا؟»
            //      تعرفُ اسمَين اثنَين ولا تعرفُ أسرةَ الأعراضِ الثمانية. وأثرُها
            //      أنّ المدقّقَ الدلاليَّ المشتركَ كان يرى `رقم32` **غيرَ عدديّ**،
            //      فلا يستطيعُ أن يحكمَ على عمليّةٍ حسابيّةٍ بين عرضَين أصلًا —
            //      وهذا هو الجدارُ الذي منعَ SEM048 من أن يُكتَبَ في طبقةٍ واحدة.
            //      والسلطةُ الآنَ واحدةٌ مشتقّةٌ من `language-truth/types.yaml`.
            // (EN) This was a fifth hand-written copy of "is this numeric?" that knew
            //      two names and not the eight-width family; the shared checker saw
            //      `رقم32` as non-numeric. Authority now derives from the SoT.
            bool isNumeric() const
            {
                return ::Sad::Types::sadTypeKindIsNumeric(kind_);
            }

            // (AR) أعدديٌّ صحيحٌ بأيِّ عرض / (EN) integer-numeric at any width
            bool isIntegerNumeric() const
            {
                return ::Sad::Types::sadTypeKindIsIntegerNumeric(kind_);
            }

            // (AR) أعدديٌّ عشريٌّ بأيِّ عرض — و`isFloat()` أعلاه تسألُ عن الاسمِ
            //      «عشري» وحدَه، فلا تُجيبُ عن `عشري32`. والفرقُ بينهما مقصود.
            // (EN) float-numeric at any width; isFloat() above is name-exact.
            bool isFloatNumeric() const
            {
                return ::Sad::Types::sadTypeKindIsFloatNumeric(kind_);
            }

            // التحقق من كون النوع مركباً / Check if composite type
            bool isComposite() const
            {
                return kind_ == SadTypeKind::Array ||
                       kind_ == SadTypeKind::Map ||
                       kind_ == SadTypeKind::Tuple;
            }

            // ═══════════════════════════════════════════════════════════════════════════
            // علاقات الأنواع / Type Relations
            // ═══════════════════════════════════════════════════════════════════════════

            // التحقق من إمكانية تحويل هذا النوع إلى نوع آخر / Check if this type can be converted to another
            // القيمة الافتراضية: تحقق من التساوي فقط / Default: check equality only
            virtual bool isAssignableTo(const Type *other) const;

            // التحقق من إمكانية استبدال هذا النوع بنوع آخر / Check if this type can substitute another
            // (Subtyping relation)
            virtual bool isSubtypeOf(const Type *other) const;

            // ═══════════════════════════════════════════════════════════════════════════
            // إدارة الحجم / Size Management
            // ═══════════════════════════════════════════════════════════════════════════

            // الحصول على حجم النوع بالبايتات / Get type size in bytes
            // القيمة الافتراضية: 0 (يجب تجاوزها للأنواع المحددة) / Default: 0 (should be overridden)
            virtual size_t getSizeInBytes() const { return 0; }

            // الحصول على محاذاة النوع / Get type alignment
            // القيمة الافتراضية: 1 / Default: 1
            virtual size_t getAlignment() const { return 1; }

        protected:
            // ═══════════════════════════════════════════════════════════════════════════
            // الأعضاء المحمية / Protected Members
            // ═══════════════════════════════════════════════════════════════════════════

            SadTypeKind kind_; // نوع النوع / Type kind
        };

        // ════════════════════════════════════════════════════════════════════════════════
        // Pointer Types للاستخدام المريح / Pointer Types for Convenience
        // ════════════════════════════════════════════════════════════════════════════════

        using TypePtr = std::shared_ptr<Type>;   // مؤشر ذكي لنوع / Smart pointer to Type
        using TypeWeakPtr = std::weak_ptr<Type>; // مؤشر ضعيف لنوع / Weak pointer to Type
        using TypeList = std::vector<TypePtr>;   // قائمة من الأنواع / List of types

        // ════════════════════════════════════════════════════════════════════════════════
        // دوال مساعدة / Helper Functions
        // ════════════════════════════════════════════════════════════════════════════════

        // تحويل SadTypeKind إلى نص عربي / Convert SadTypeKind to Arabic text
        std::string typeKindToArabic(SadTypeKind kind);

        // تحويل SadTypeKind إلى نص إنجليزي / Convert SadTypeKind to English text
        std::string typeKindToEnglish(SadTypeKind kind);

        // التحقق من تساوي نوعين / Check if two types are equal
        bool typesEqual(const TypePtr &a, const TypePtr &b);

        // التحقق من تساوي قوائم الأنواع / Check if two type lists are equal
        bool typeListsEqual(const TypeList &a, const TypeList &b);

    } // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPE_SYSTEM_TYPE_H
