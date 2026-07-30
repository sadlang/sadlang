// ============================================================================
// AUTO-GENERATED FROM language-truth/types.yaml — DO NOT EDIT MANUALLY
// (AR) ملف مولَّد تلقائياً. لإجراء تعديل، عدّل types.yaml وأعد البناء.
// (EN) Auto-generated file. To modify, edit types.yaml and rebuild.
// ============================================================================

#pragma once

#include <array>
#include <string_view>

namespace Sad
{
    namespace Types
    {

        /**
         * @brief (AR) التعداد الموحد لكل أنواع لغة ص — مُولَّد من types.yaml
         * @brief (EN) Unified type kind enum — generated from types.yaml
         *
         * (AR) عدد القيم: 52 — الترتيب حرج (توافق ثنائي).
         * (EN) 52 values — order is critical (binary compat).
         */
        enum class SadTypeKind : int
        {
            // ─── أنواع بدائية / Primitive types ───
            Void,          ///< فراغ / void
            Integer,       ///< رقم / integer
            Float,         ///< عشري / float
            Boolean,       ///< منطقي / boolean
            String,        ///< نص / string
            Byte,          ///< بايت / byte
            // ─── أنواع بدائية بحجم محدد (للمترجم) / Sized primitives (for compiler) ───
            Int8,          ///< عدد8 / int8
            Int16,         ///< عدد16 / int16
            Int32,         ///< عدد32 / int32
            Int64,         ///< عدد64 / int64
            UInt8,         ///< طبيعي8 / uint8
            UInt16,        ///< طبيعي16 / uint16
            UInt32,        ///< طبيعي32 / uint32
            UInt64,        ///< طبيعي64 / uint64
            Float32,       ///< عشري32 / float32
            Float64,       ///< عشري64 / float64
            Char,          ///< حرف / char
            // ─── أنواع مركبة / Composite types ───
            Array,         ///< مصفوفة / array
            Map,           ///< خريطة / map
            Tuple,         ///< صف / tuple
            Slice,         ///< شريحة / slice
            // ─── أنواع كائنية / OOP types ───
            Class,         ///< صنف / class
            Struct,        ///< بنية / struct
            Enum,          ///< تعداد / enum
            Trait,         ///< سمة / trait
            // ─── أنواع وظيفية / Function types ───
            Function,      ///< دالة / function
            Closure,       ///< إغلاق / closure
            // ─── أنواع متقدمة / Advanced types ───
            Union,         ///< اتحاد / union
            Intersection,  ///< تقاطع / intersection
            Optional,      ///< اختياري / optional
            Result,        ///< نتيجة / result
            Generic,       ///< نوع_عام / generic
            TypeParameter, ///< معامل_نوع / type_parameter
            TypeAlias,     ///< اسم_مستعار / type_alias
            // ─── مؤشرات ومراجع / Pointers and references ───
            Pointer,       ///< مؤشر / pointer
            Reference,     ///< مرجع / reference
            MutableRef,    ///< مرجع_متغير / mutable_ref
            // ─── أنواع خاصة / Special types ───
            Any,           ///< أي / any
            Never,         ///< أبدا / never
            Unknown,       ///< مجهول / unknown
            Error,         ///< خطأ / error
            // ─── أنواع غير متزامنة / Async types ───
            Future,        ///< مستقبل / future
            Generator,     ///< مولد / generator
            Comprehension, ///< استيعاب / comprehension
            // ─── أنواع الرسومات / Graphics types ───
            Color,         ///< لون / color
            Widget,        ///< عنصر_واجهة / widget
            Window,        ///< نافذة / window
            Event,         ///< حدث / event
            // ─── متجهات SIMD / SIMD vectors ───
            Vector,        ///< متجه / vector
            // ─── أنواع خاصة / Special types ───
            Null,          ///< عدم / null
            // ─── أنواع الرسومات / Graphics types ───
            Point,         ///< نقطة / point
            Rect,          ///< مستطيل / rect
        };

        /**
         * @brief (AR) الاسم العربي الذي تُرجعه نوع() لنوعٍ ما — مُولَّد من types.yaml
         * @brief (EN) Arabic name returned by نوع()/typeof for a kind — generated
         */
        inline const char *sadTypeKindArabicName(SadTypeKind kind)
        {
            switch (kind)
            {
            case SadTypeKind::Void: return "\xd9\x81\xd8\xb1\xd8\xa7\xd8\xba"; // فراغ
            case SadTypeKind::Integer: return "\xd8\xb1\xd9\x82\xd9\x85"; // رقم
            case SadTypeKind::Float: return "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a"; // عشري
            case SadTypeKind::Boolean: return "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a"; // منطقي
            case SadTypeKind::String: return "\xd9\x86\xd8\xb5"; // نص
            case SadTypeKind::Byte: return "\xd8\xa8\xd8\xa7\xd9\x8a\xd8\xaa"; // بايت
            case SadTypeKind::Int8: return "\xd8\xb9\xd8\xaf\xd8\xaf\x38"; // عدد8
            case SadTypeKind::Int16: return "\xd8\xb9\xd8\xaf\xd8\xaf\x31\x36"; // عدد16
            case SadTypeKind::Int32: return "\xd8\xb9\xd8\xaf\xd8\xaf\x33\x32"; // عدد32
            case SadTypeKind::Int64: return "\xd8\xb9\xd8\xaf\xd8\xaf\x36\x34"; // عدد64
            case SadTypeKind::UInt8: return "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x38"; // طبيعي8
            case SadTypeKind::UInt16: return "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x31\x36"; // طبيعي16
            case SadTypeKind::UInt32: return "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x33\x32"; // طبيعي32
            case SadTypeKind::UInt64: return "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x36\x34"; // طبيعي64
            case SadTypeKind::Float32: return "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a\x33\x32"; // عشري32
            case SadTypeKind::Float64: return "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a\x36\x34"; // عشري64
            case SadTypeKind::Char: return "\xd8\xad\xd8\xb1\xd9\x81"; // حرف
            case SadTypeKind::Array: return "\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9"; // مصفوفة
            case SadTypeKind::Map: return "\xd8\xae\xd8\xb1\xd9\x8a\xd8\xb7\xd8\xa9"; // خريطة
            case SadTypeKind::Tuple: return "\xd8\xb5\xd9\x81"; // صف
            case SadTypeKind::Slice: return "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xad\xd8\xa9"; // شريحة
            case SadTypeKind::Class: return "\xd9\x83\xd8\xa7\xd8\xa6\xd9\x86"; // كائن
            case SadTypeKind::Struct: return "\xd9\x83\xd8\xa7\xd8\xa6\xd9\x86"; // كائن
            case SadTypeKind::Enum: return "\xd8\xaa\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf"; // تعداد
            case SadTypeKind::Trait: return "\xd8\xb3\xd9\x85\xd8\xa9"; // سمة
            case SadTypeKind::Function: return "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9"; // دالة
            case SadTypeKind::Closure: return "\xd8\xa5\xd8\xba\xd9\x84\xd8\xa7\xd9\x82"; // إغلاق
            case SadTypeKind::Union: return "\xd8\xa7\xd8\xaa\xd8\xad\xd8\xa7\xd8\xaf"; // اتحاد
            case SadTypeKind::Intersection: return "\xd8\xaa\xd9\x82\xd8\xa7\xd8\xb7\xd8\xb9"; // تقاطع
            case SadTypeKind::Optional: return "\xd8\xa7\xd8\xae\xd8\xaa\xd9\x8a\xd8\xa7\xd8\xb1\xd9\x8a"; // اختياري
            case SadTypeKind::Result: return "\xd9\x86\xd8\xaa\xd9\x8a\xd8\xac\xd8\xa9"; // نتيجة
            case SadTypeKind::Generic: return "\xd9\x86\xd9\x88\xd8\xb9\x5f\xd8\xb9\xd8\xa7\xd9\x85"; // نوع_عام
            case SadTypeKind::TypeParameter: return "\xd9\x85\xd8\xb9\xd8\xa7\xd9\x85\xd9\x84\x5f\xd9\x86\xd9\x88\xd8\xb9"; // معامل_نوع
            case SadTypeKind::TypeAlias: return "\xd8\xa7\xd8\xb3\xd9\x85\x5f\xd9\x85\xd8\xb3\xd8\xaa\xd8\xb9\xd8\xa7\xd8\xb1"; // اسم_مستعار
            case SadTypeKind::Pointer: return "\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1"; // مؤشر
            case SadTypeKind::Reference: return "\xd9\x85\xd8\xb1\xd8\xac\xd8\xb9"; // مرجع
            case SadTypeKind::MutableRef: return "\xd9\x85\xd8\xb1\xd8\xac\xd8\xb9\x5f\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1"; // مرجع_متغير
            case SadTypeKind::Any: return "\xd8\xa3\xd9\x8a"; // أي
            case SadTypeKind::Never: return "\xd8\xa3\xd8\xa8\xd8\xaf\xd8\xa7"; // أبدا
            case SadTypeKind::Unknown: return "\xd9\x85\xd8\xac\xd9\x87\xd9\x88\xd9\x84"; // مجهول
            case SadTypeKind::Error: return "\xd8\xae\xd8\xb7\xd8\xa3"; // خطأ
            case SadTypeKind::Future: return "\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84"; // مستقبل
            case SadTypeKind::Generator: return "\xd9\x85\xd9\x88\xd9\x84\xd9\x91\xd8\xaf"; // مولّد
            case SadTypeKind::Comprehension: return "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x8a\xd8\xb9\xd8\xa7\xd8\xa8"; // استيعاب
            case SadTypeKind::Color: return "\xd9\x84\xd9\x88\xd9\x86"; // لون
            case SadTypeKind::Widget: return "\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1\x5f\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9"; // عنصر_واجهة
            case SadTypeKind::Window: return "\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9"; // نافذة
            case SadTypeKind::Event: return "\xd8\xad\xd8\xaf\xd8\xab"; // حدث
            case SadTypeKind::Vector: return "\xd9\x85\xd8\xaa\xd8\xac\xd9\x87"; // متجه
            case SadTypeKind::Null: return "\xd8\xb9\xd8\xaf\xd9\x85"; // عدم
            case SadTypeKind::Point: return "\xd9\x86\xd9\x82\xd8\xb7\xd8\xa9"; // نقطة
            case SadTypeKind::Rect: return "\xd9\x85\xd8\xb3\xd8\xaa\xd8\xb7\xd9\x8a\xd9\x84"; // مستطيل
            default: return "\xd9\x85\xd8\xac\xd9\x87\xd9\x88\xd9\x84"; // مجهول
            }
        }

        // ─── أسماء الأنواع السطحية / Surface type names ───
        /**
         * @brief (AR) أسماء الأنواع السطحية (surface:true) — مُولَّدة من types.yaml
         * @brief (EN) Surface type names (surface:true) — generated from types.yaml
         *
         * (AR) العدد: 11 — يستهلكها مزوّد الرموز الدلاليّة لتلوينها أنواعًا.
         * (EN) 11 names — consumed by the semantic-tokens provider.
         */
        inline constexpr std::array<std::string_view, 11> SURFACE_TYPE_NAMES = {{
            "\xd9\x81\xd8\xb1\xd8\xa7\xd8\xba", // فراغ
            "\xd8\xb1\xd9\x82\xd9\x85", // رقم
            "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a", // عشري
            "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a", // منطقي
            "\xd9\x86\xd8\xb5", // نص
            "\xd8\xa8\xd8\xa7\xd9\x8a\xd8\xaa", // بايت
            "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x36\x34", // طبيعي64
            "\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9", // مصفوفة
            "\xd8\xae\xd8\xb1\xd9\x8a\xd8\xb7\xd8\xa9", // خريطة
            "\xd8\xa3\xd9\x8a", // أي
            "\xd8\xb9\xd8\xaf\xd9\x85", // عدم
        }};

        /**
         * @brief (AR) وصف النوع السطحيّ بالعربية بحسب كلمته — مُولَّد من types.yaml
         * @brief (EN) Arabic description of a surface type by its word — generated
         */
        inline const char *surfaceTypeDescriptionAr(std::string_view word)
        {
            if (word == "\xd9\x81\xd8\xb1\xd8\xa7\xd8\xba") return "\xd9\x86\xd9\x88\xd8\xb9\x20\xd8\xa7\xd9\x84\xd8\xa5\xd8\xb1\xd8\xac\xd8\xa7\xd8\xb9\x20\xd8\xa7\xd9\x84\xd9\x81\xd8\xa7\xd8\xb1\xd8\xba\x20\xe2\x80\x94\x20\xd9\x84\xd8\xa7\x20\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9\x2e\x20\xd9\x85\xd8\xaa\xd9\x85\xd8\xa7\xd9\x8a\xd8\xb2\x20\xd8\xb9\xd9\x86\x20\xc2\xab\xd8\xb9\xd8\xaf\xd9\x85\xc2\xbb\x20\x28\x74\x79\x70\x65\x2e\x6e\x75\x6c\x6c\x29\x3a\x20\xc2\xab\xd9\x81\xd8\xb1\xd8\xa7\xd8\xba\xc2\xbb\x20\xd9\x85\xd8\xa7\x20\xd9\x84\xd9\x85\x20\xd9\x8a\xd9\x8f\xd8\xb1\xd8\xac\xd9\x8e\xd8\xb9\x20\xd8\xa3\xd8\xb5\xd9\x84\xd8\xa7\xd9\x8b\x20\x28\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad\x20\xd8\xba\xd8\xa7\xd8\xa6\xd8\xa8\xd8\x8c\x20\xd8\xa8\xd8\xad\xd8\xab\xd9\x8c\x20\xd8\xa8\xd9\x84\xd8\xa7\x20\xd9\x85\xd8\xb7\xd8\xa7\xd8\xa8\xd9\x82\xd8\xa9\x29\xd8\x8c\x20\xd9\x88\xc2\xab\xd8\xb9\xd8\xaf\xd9\x85\xc2\xbb\x20\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9\xd9\x8f\x20\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xa1\x20\xd8\xa7\xd9\x84\xd8\xb5\xd8\xb1\xd9\x8a\xd8\xad\xd8\xa9\x2e\x20\xd9\x81\xd8\xad\xd8\xb5\xd9\x8f\x20\xd8\xa3\xd8\xad\xd8\xaf\xd9\x87\xd9\x85\xd8\xa7\x20\xd9\x88\xd8\xad\xd8\xaf\xd9\x87\x20\xd9\x8a\xd9\x85\xd8\xb1\xd9\x91\x20\xd9\x86\xd8\xa7\xd9\x82\xd8\xb5\xd9\x8b\xd8\xa7"; // فراغ
            if (word == "\xd8\xb1\xd9\x82\xd9\x85") return "\xd8\xb9\xd8\xaf\xd8\xaf\x20\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad\x20\x28\x69\x36\x34\x29"; // رقم
            if (word == "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a") return "\xd8\xb9\xd8\xaf\xd8\xaf\x20\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a\x20\x28\x66\x36\x34\x29"; // عشري
            if (word == "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a") return "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9\x20\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a\xd8\xa9\x20\x28\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad\x2f\xd8\xae\xd8\xb7\xd8\xa3\x29"; // منطقي
            if (word == "\xd9\x86\xd8\xb5") return "\xd9\x86\xd8\xb5\x20\x28\x55\x54\x46\x2d\x38\x29"; // نص
            if (word == "\xd8\xa8\xd8\xa7\xd9\x8a\xd8\xaa") return "\xd8\xa8\xd8\xa7\xd9\x8a\xd8\xaa\x20\x28\x75\x38\xd8\x8c\x20\x30\xe2\x80\x93\x32\x35\x35\x29"; // بايت
            if (word == "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x36\x34") return "\xd8\xb9\xd8\xaf\xd8\xaf\x20\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x20\x36\x34\x2d\xd8\xa8\xd8\xaa"; // طبيعي64
            if (word == "\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9") return "\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9\x3c\x54\x3e\x20\xd8\xaf\xd9\x8a\xd9\x86\xd8\xa7\xd9\x85\xd9\x8a\xd9\x83\xd9\x8a\xd8\xa9"; // مصفوفة
            if (word == "\xd8\xae\xd8\xb1\xd9\x8a\xd8\xb7\xd8\xa9") return "\xd8\xae\xd8\xb1\xd9\x8a\xd8\xb7\xd8\xa9\x3c\x4b\x2c\x56\x3e"; // خريطة
            if (word == "\xd8\xa3\xd9\x8a") return "\xd8\xa3\xd9\x8a\x20\xd9\x86\xd9\x88\xd8\xb9\x20\x28\xd8\xaf\xd9\x8a\xd9\x86\xd8\xa7\xd9\x85\xd9\x8a\xd9\x83\xd9\x8a\x29"; // أي
            if (word == "\xd8\xb9\xd8\xaf\xd9\x85") return "\xd8\xa7\xd9\x84\xd8\xb9\xd8\xaf\xd9\x85\x20\xe2\x80\x94\x20\xd8\xba\xd9\x8a\xd8\xa7\xd8\xa8\x20\xd8\xa7\xd9\x84\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9\xd8\x8c\x20\xd9\x85\xd8\xaa\xd9\x85\xd8\xa7\xd9\x8a\xd8\xb2\x20\xd8\xb9\xd9\x86\x20\xd9\x81\xd8\xb1\xd8\xa7\xd8\xba\x20\x28\x76\x6f\x69\x64\x29"; // عدم
            return "";
        }

    } // namespace Types
} // namespace Sad
