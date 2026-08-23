// ============================================================================
// AUTO-GENERATED FROM language-truth/types.yaml — DO NOT EDIT MANUALLY
// (AR) ملف مولَّد تلقائياً. لإجراء تعديل، عدّل types.yaml وأعد البناء.
// (EN) Auto-generated file. To modify, edit types.yaml and rebuild.
// ============================================================================

#pragma once

#include <array>
#include <cstddef>
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
         * @brief (AR) عدد قيم SadTypeKind — تستهلكه static_assert في جسور
         *             الأنواع (مثل astTypeToSIRType) كي **يكسر البناءَ** عند
         *             إضافة نوعٍ جديدٍ إلى types.yaml، فلا يسقط النوعُ الجديد
         *             صامتًا في فرعٍ افتراضيٍّ يُخمّن تمثيلًا خاطئًا.
         * @brief (EN) SadTypeKind cardinality — consumed by static_assert in the
         *             type bridges (e.g. astTypeToSIRType) so that adding a kind
         *             to types.yaml BREAKS THE BUILD instead of letting the new
         *             kind fall silently into a guessing default branch.
         */
        inline constexpr int SAD_TYPE_KIND_COUNT = 52;

        /**
         * @brief (AR) مفرداتُ القيمةِ الافتراضيّةِ للتهيئة — مُولَّدة من types.yaml
         * @brief (EN) Default-initialisation vocabulary — generated from types.yaml
         *
         * (AR) رمزيّةٌ لا تمثيليّة: يترجمها كلُّ محرّكٍ إلى قيمتِه الخاصّة.
         *      «Unspecified» صفرٌ عمدًا: خانةٌ مُصفَّرةٌ تعني «غيرُ مُعلَن».
         * (EN) Symbolic, not representational — each engine lowers it itself.
         *      «Unspecified» is 0 on purpose: a zeroed slot reads as undeclared.
         */
        enum class SadDefaultInit : int
        {
            Unspecified, ///< لا افتراضيَّ مُعلَنًا — يُبقي كلُّ محرّكٍ احتياطَه / no declared default - each engine keeps its own fallback
            IntZero,     ///< صفرٌ صحيح / integer zero
            FloatZero,   ///< صفرٌ عشريّ / float zero
            BoolFalse,   ///< خطأ / boolean false
            EmptyString, ///< نصٌّ فارغ / empty string
            Null,        ///< لاشيء — العدمُ الصريح / the explicit null value
            Void,        ///< فراغٌ — لم تُسنَدْ بعدُ، متمايزٌ عن العدم / void - never assigned, distinct from null
            NotASlot,    ///< لا خانةَ تحمله — يرفضه المحلّل (SEM040) / no slot can hold it - rejected by the parser (SEM040)
        };

        /**
         * @brief (AR) عددُ مفرداتِ SadDefaultInit — تستهلكه static_assert في كلِّ
         *             مستهلكٍ يوزّع عليها، فإضافةُ مفردةٍ جديدةٍ **تكسر البناءَ**
         *             عند كلِّ مَن يقرّر تمثيلَها، بدل أن تسقط في احتياطٍ صامت.
         * @brief (EN) SadDefaultInit cardinality — consumed by static_assert at every
         *             consumer, so adding a vocabulary word BREAKS THE BUILD at each
         *             site that must decide its representation.
         */
        inline constexpr int SAD_DEFAULT_INIT_COUNT = 8;

        /**
         * @brief (AR) جدولُ القيمِ الافتراضيّةِ مفهرسًا بقيمةِ SadTypeKind — مُولَّد
         * @brief (EN) Default-init table indexed by SadTypeKind value — generated
         */
        inline constexpr std::array<SadDefaultInit, SAD_TYPE_KIND_COUNT> SAD_TYPE_DEFAULT_INIT_TABLE = {{
            SadDefaultInit::NotASlot,    // Void — فراغ
            SadDefaultInit::IntZero,     // Integer — رقم
            SadDefaultInit::FloatZero,   // Float — عشري
            SadDefaultInit::BoolFalse,   // Boolean — منطقي
            SadDefaultInit::EmptyString, // String — نص
            SadDefaultInit::IntZero,     // Byte — بايت
            SadDefaultInit::Unspecified, // Int8 — عدد8
            SadDefaultInit::Unspecified, // Int16 — عدد16
            SadDefaultInit::Unspecified, // Int32 — عدد32
            SadDefaultInit::Unspecified, // Int64 — عدد64
            SadDefaultInit::Unspecified, // UInt8 — طبيعي8
            SadDefaultInit::Unspecified, // UInt16 — طبيعي16
            SadDefaultInit::Unspecified, // UInt32 — طبيعي32
            SadDefaultInit::IntZero,     // UInt64 — طبيعي64
            SadDefaultInit::Unspecified, // Float32 — عشري32
            SadDefaultInit::Unspecified, // Float64 — عشري64
            SadDefaultInit::Unspecified, // Char — حرف
            SadDefaultInit::Unspecified, // Array — مصفوفة
            SadDefaultInit::Unspecified, // Map — خريطة
            SadDefaultInit::Unspecified, // Tuple — صف
            SadDefaultInit::Unspecified, // Slice — شريحة
            SadDefaultInit::Unspecified, // Class — صنف
            SadDefaultInit::Unspecified, // Struct — بنية
            SadDefaultInit::Unspecified, // Enum — تعداد
            SadDefaultInit::Unspecified, // Trait — سمة
            SadDefaultInit::Unspecified, // Function — دالة
            SadDefaultInit::Unspecified, // Closure — إغلاق
            SadDefaultInit::Unspecified, // Union — اتحاد
            SadDefaultInit::Unspecified, // Intersection — تقاطع
            SadDefaultInit::Null,        // Optional — اختياري
            SadDefaultInit::Unspecified, // Result — نتيجة
            SadDefaultInit::Unspecified, // Generic — نوع_عام
            SadDefaultInit::Unspecified, // TypeParameter — معامل_نوع
            SadDefaultInit::Unspecified, // TypeAlias — اسم_مستعار
            SadDefaultInit::Unspecified, // Pointer — مؤشر
            SadDefaultInit::Unspecified, // Reference — مرجع
            SadDefaultInit::Unspecified, // MutableRef — مرجع_متغير
            SadDefaultInit::Void,        // Any — أي
            SadDefaultInit::Unspecified, // Never — أبدا
            SadDefaultInit::Unspecified, // Unknown — مجهول
            SadDefaultInit::Unspecified, // Error — خطأ
            SadDefaultInit::Unspecified, // Future — مستقبل
            SadDefaultInit::Unspecified, // Generator — مولد
            SadDefaultInit::Unspecified, // Comprehension — استيعاب
            SadDefaultInit::Unspecified, // Color — لون
            SadDefaultInit::Unspecified, // Widget — عنصر_واجهة
            SadDefaultInit::Unspecified, // Window — نافذة
            SadDefaultInit::Unspecified, // Event — حدث
            SadDefaultInit::Unspecified, // Vector — متجه
            SadDefaultInit::Null,        // Null — عدم
            SadDefaultInit::Unspecified, // Point — نقطة
            SadDefaultInit::Unspecified, // Rect — مستطيل
        }};

        /**
         * @brief (AR) القيمةُ الافتراضيّةُ الرمزيّةُ لنوعٍ ما — مُولَّدة من types.yaml
         * @brief (EN) Symbolic default-init of a kind — generated from types.yaml
         *
         * (AR) دالّةٌ كلّيّةٌ بنيويًّا: الفهرسةُ بقيمةِ التعدادِ الكثيفة، وقيمةٌ
         *      خارجَ المدى تُرجِع «Unspecified» — أي «لا أعرف» لا قيمةً مُخترَعة.
         * (EN) Structurally total: dense enum indexing; an out-of-range value
         *      yields «Unspecified» - never an invented value.
         */
        inline constexpr SadDefaultInit sadTypeKindDefaultInit(SadTypeKind kind)
        {
            const int index = static_cast<int>(kind);
            if (index < 0 || index >= SAD_TYPE_KIND_COUNT)
            {
                return SadDefaultInit::Unspecified;
            }
            return SAD_TYPE_DEFAULT_INIT_TABLE[static_cast<std::size_t>(index)];
        }

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

        // ─── اللفظ ⇒ النوع / Word ⇒ kind ───
        /**
         * @brief (AR) نوعُ اللفظِ السطحيّ — مُولَّد من types.yaml
         * @brief (EN) Kind of a surface type word — generated from types.yaml
         *
         * (AR) يُرجِع Unknown لِما ليس لفظَ نوعٍ سطحيّ — فالمُنادي يميّز
         *      «ليس نوعًا» عن «نوعٌ مجهول» بموضعِه لا بهذه الدالّة.
         * (EN) Returns Unknown for anything that is not a surface type word.
         */
        inline SadTypeKind sadTypeKindFromArabicName(std::string_view word)
        {
            if (word == "\xd9\x81\xd8\xb1\xd8\xa7\xd8\xba") return SadTypeKind::Void; // فراغ
            if (word == "\xd8\xb1\xd9\x82\xd9\x85") return SadTypeKind::Integer; // رقم
            if (word == "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a") return SadTypeKind::Float; // عشري
            if (word == "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a") return SadTypeKind::Boolean; // منطقي
            if (word == "\xd9\x86\xd8\xb5") return SadTypeKind::String; // نص
            if (word == "\xd8\xa8\xd8\xa7\xd9\x8a\xd8\xaa") return SadTypeKind::Byte; // بايت
            if (word == "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x36\x34") return SadTypeKind::UInt64; // طبيعي64
            if (word == "\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9") return SadTypeKind::Array; // مصفوفة
            if (word == "\xd8\xae\xd8\xb1\xd9\x8a\xd8\xb7\xd8\xa9") return SadTypeKind::Map; // خريطة
            if (word == "\xd8\xa3\xd9\x8a") return SadTypeKind::Any; // أي
            if (word == "\xd8\xb9\xd8\xaf\xd9\x85") return SadTypeKind::Null; // عدم
            return SadTypeKind::Unknown;
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

        // ─── ألفاظ أنواعٍ مُزالة / Removed type words ───
        /**
         * @brief (AR) ألفاظ الأنواع المُزالة — مُولَّدة من types.yaml
         * @brief (EN) Removed type words — generated from types.yaml
         *
         * (AR) العدد: 1 — يستهلكها المحلّل ليُبقي تشخيص SYN014 حيًّا.
         * (EN) 1 — consumed by the parser to keep SYN014 alive.
         */
        inline constexpr std::array<std::string_view, 1> REMOVED_TYPE_NAMES = {{
            "\xd9\x85\xd8\xb6\xd8\xa7\xd8\xb9\xd9\x81", // مضاعف ⇒ عشري
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
            if (word == "\xd8\xb9\xd8\xaf\xd9\x85") return "\xd8\xa7\xd9\x84\xd8\xb9\xd8\xaf\xd9\x85\x20\xe2\x80\x94\x20\xd8\xba\xd9\x8a\xd8\xa7\xd8\xa8\x20\xd8\xa7\xd9\x84\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9\xd8\x8c\x20\xd9\x85\xd8\xaa\xd9\x85\xd8\xa7\xd9\x8a\xd8\xb2\x20\xd8\xb9\xd9\x86\x20\xd9\x81\xd8\xb1\xd8\xa7\xd8\xba\x20\x28\x76\x6f\x69\x64\x29\x2e\x20\xd9\x88\xd8\xaa\xd9\x85\xd8\xab\xd9\x8a\xd9\x84\xd9\x8f\xd9\x87\x20\xd8\xad\xd8\xa7\xd8\xb1\xd8\xb3\xd9\x8c\x20\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad\xd9\x8f\x20\xd8\xa7\xd9\x84\xd8\xa8\xd8\xaa\xd9\x91\xd8\xa7\xd8\xaa\xd8\x8c\x20\xd9\x81\xd8\xa7\xd9\x84\xd8\xad\xd9\x83\xd9\x85\xd9\x8f\x20\xd8\xb9\xd9\x84\xd9\x8a\xd9\x87\x20\xd9\x8a\xd9\x84\xd8\xb2\xd9\x85\xd9\x8f\x20\xd8\xa3\xd9\x86\x20\xd9\x8a\xd9\x82\xd9\x88\xd9\x85\xd9\x8e\x20\xd8\xb9\xd9\x84\xd9\x89\x20\xd9\x88\xd8\xb3\xd9\x85\xd9\x90\x20\xd8\xa7\xd9\x84\xd9\x86\xd9\x88\xd8\xb9\xd9\x90\x20\xd9\x84\xd8\xa7\x20\xd8\xb9\xd9\x84\xd9\x89\x20\xd8\xb4\xd9\x83\xd9\x84\xd9\x90\x20\xd8\xa7\xd9\x84\xd8\xaa\xd9\x85\xd8\xab\xd9\x8a\xd9\x84\x3a\x20\xd9\x85\xd9\x82\xd8\xa7\xd8\xb1\xd9\x86\xd8\xa9\xd9\x8f\x20\xd8\xa7\xd9\x84\xd8\xb9\xd8\xaf\xd9\x85\xd9\x90\x20\xd8\xa8\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9\xd9\x8d\x20\xd9\x85\xd9\x86\x20\xd9\x86\xd9\x88\xd8\xb9\xd9\x8d\x20\xd8\xa2\xd8\xae\xd8\xb1\x20\xd8\xaa\xd9\x8f\xd8\xad\xd8\xb3\xd9\x8e\xd9\x85\xd9\x8f\x20\xd8\xa8\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x90\x20\xd8\xa7\xd9\x84\xd8\xb7\xd8\xb1\xd9\x81\xd9\x90\x20\xd8\xa7\xd9\x84\xd8\xa2\xd8\xae\xd8\xb1\xd9\x90\x20\xd8\xa5\xd9\x84\xd9\x89\x20\xd8\xb9\xd8\xb1\xd8\xb6\xd9\x90\x20\xd8\xa7\xd9\x84\xd8\xad\xd8\xa7\xd8\xb1\xd8\xb3\xd9\x90\x20\xd9\x88\xd9\x85\xd9\x82\xd8\xa7\xd8\xb1\xd9\x86\xd8\xaa\xd9\x90\xd9\x87\x20\xd8\xa8\xd9\x87\x20\xd8\xb9\xd8\xaf\xd8\xaf\xd9\x8a\xd9\x8b\xd9\x91\xd8\xa7\xd8\x8c\x20\xd9\x81\xd9\x84\xd8\xa7\x20\xd8\xaa\xd9\x8f\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xb9\xd9\x89\x20\xd9\x85\xd9\x82\xd8\xa7\xd8\xb1\xd9\x86\xd8\xa9\xd9\x8f\x20\xd9\x86\xd8\xb5\xd9\x88\xd8\xb5\xd9\x8d\x20\xd8\xb9\xd9\x84\xd9\x89\x20\xd8\xa7\xd9\x84\xd8\xad\xd8\xa7\xd8\xb1\xd8\xb3\xd9\x90\x20\xd8\xa3\xd8\xa8\xd8\xaf\xd9\x8b\xd8\xa7\xd8\x9b\x20\xd9\x88\xc2\xab\xd8\xb9\xd8\xaf\xd9\x85\x20\x3d\x3d\x20\xd8\xb9\xd8\xaf\xd9\x85\xc2\xbb\x20\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad\x2e\x20\xd9\x88\xd8\xa7\xd9\x84\xd9\x88\xd8\xb3\xd9\x85\xd9\x8f\x20\xd8\xa7\xd9\x84\xd8\xb3\xd8\xa7\xd9\x83\xd9\x86\xd9\x8f\x20\xd9\x84\xd8\xa7\x20\xd9\x8a\xd9\x83\xd9\x81\xd9\x8a\x20\xd9\x88\xd8\xad\xd8\xaf\xd9\x8e\xd9\x87\x20\xd9\x88\xd9\x84\xd8\xa7\x20\xd9\x8a\xd9\x85\xd9\x83\xd9\x86\xd9\x8f\x20\xd8\xa3\xd9\x86\x20\xd9\x8a\xd9\x83\xd9\x81\xd9\x8a\x3a\x20\xd8\xae\xd8\xa7\xd9\x86\xd8\xa9\xd9\x8c\x20\xd9\x86\xd9\x88\xd8\xb9\xd9\x8f\xd9\x87\xd8\xa7\x20\xd8\xa7\xd9\x84\xd9\x85\xd9\x8f\xd8\xb5\xd8\xb1\xd9\x8e\xd9\x91\xd8\xad\xd9\x8f\x20\xc2\xab\xd9\x86\xd8\xb5\xd9\x91\xc2\xbb\x20\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84\xd9\x8f\x20\xd9\x86\xd8\xb5\xd9\x8b\xd9\x91\xd8\xa7\x20\xd9\x81\xd9\x8a\x20\xd9\x85\xd9\x88\xd9\x82\xd8\xb9\xd9\x90\x20\xd9\x86\xd8\xaf\xd8\xa7\xd8\xa1\xd9\x8d\x20\xd9\x88\xd8\xb9\xd8\xaf\xd9\x85\xd9\x8b\xd8\xa7\x20\xd9\x81\xd9\x8a\x20\xd8\xa2\xd8\xae\xd8\xb1\xd8\x8c\x20\xd9\x81\xd9\x84\xd8\xa7\x20\xd9\x88\xd8\xb3\xd9\x85\xd9\x8e\x20\xd8\xb3\xd8\xa7\xd9\x83\xd9\x86\xd9\x8b\xd8\xa7\x20\xd9\x88\xd8\xa7\xd8\xad\xd8\xaf\xd9\x8b\xd8\xa7\x20\xd9\x8a\xd8\xb5\xd8\xaf\xd9\x8f\xd9\x82\xd9\x8f\x20\xd8\xb9\xd9\x84\xd9\x89\x20\xd8\xa7\xd9\x84\xd8\xa7\xd8\xab\xd9\x86\xd9\x8e\xd9\x8a\xd9\x86\x20\xe2\x80\x94\x20\xd9\x88\xd8\xa7\xd9\x84\xd8\xb3\xd8\xa4\xd8\xa7\xd9\x84\xd9\x8f\x20\xd9\x86\xd9\x81\xd8\xb3\xd9\x8f\xd9\x87\x20\xd8\xb3\xd8\xa4\xd8\xa7\xd9\x84\xd9\x8f\x20\xd8\xb2\xd9\x85\xd9\x86\xd9\x90\x20\xd8\xaa\xd8\xb4\xd8\xba\xd9\x8a\xd9\x84\x2e\x20\xd9\x84\xd8\xb0\xd9\x84\xd9\x83\xd9\x8e\x20\xd9\x85\xd9\x82\xd8\xa7\xd8\xb1\xd9\x86\xd8\xa9\xd9\x8f\x20\xd8\xa7\xd9\x84\xd9\x86\xd8\xb5\xd9\x88\xd8\xb5\xd9\x90\x20\xd8\xa2\xd9\x85\xd9\x86\xd8\xa9\xd9\x8c\x20\xd9\x85\xd9\x86\x20\xd8\xa7\xd9\x84\xd8\xb9\xd8\xaf\xd9\x85\xd9\x90\x20\xd9\x88\xd9\x82\xd8\xaa\xd9\x8e\x20\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb4\xd8\xba\xd9\x8a\xd9\x84\x3a\x20\xd9\x8a\xd9\x8f\xd9\x81\xd8\xad\xd9\x8e\xd8\xb5\xd9\x8f\x20\xd8\xa7\xd9\x84\xd8\xb7\xd8\xb1\xd9\x81\xd8\xa7\xd9\x86\xd9\x90\x20\xd8\xb9\xd9\x84\xd9\x89\x20\xd8\xa7\xd9\x84\xd8\xad\xd8\xa7\xd8\xb1\xd8\xb3\xd9\x90\x20\xd9\x82\xd8\xa8\xd9\x84\xd9\x8e\x20\xd8\xa3\xd9\x8a\xd9\x90\xd9\x91\x20\xd9\x82\xd8\xb1\xd8\xa7\xd8\xa1\xd8\xa9\xd9\x90\x20\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9\xd8\x8c\x20\xd9\x81\xd8\xa7\xd9\x84\xd8\xb9\xd8\xaf\xd9\x85\xd9\x8f\x20\xd9\x8a\xd8\xb3\xd8\xa7\xd9\x88\xd9\x8a\x20\xd8\xa7\xd9\x84\xd8\xb9\xd8\xaf\xd9\x85\xd9\x8e\x20\xd9\x88\xd9\x84\xd8\xa7\x20\xd9\x8a\xd8\xb3\xd8\xa7\xd9\x88\xd9\x8a\x20\xd9\x86\xd8\xb5\xd9\x8b\xd9\x91\xd8\xa7\xd8\x8c\x20\xd9\x88\xd9\x84\xd8\xa7\x20\xd9\x8a\xd9\x8f\xd9\x85\xd8\xb1\xd9\x8e\xd9\x91\xd8\xb1\xd9\x8f\x20\xd8\xa7\xd9\x84\xd8\xad\xd8\xa7\xd8\xb1\xd8\xb3\xd9\x8f\x20\xd8\xa5\xd9\x84\xd9\x89\x20\xd9\x85\xd9\x82\xd8\xa7\xd8\xb1\xd9\x86\xd8\xa9\xd9\x90\x20\xd9\x86\xd8\xb5\xd9\x88\xd8\xb5\xd9\x8d\x20\xd8\xa3\xd8\xa8\xd8\xaf\xd9\x8b\xd8\xa7"; // عدم
            return "";
        }

    } // namespace Types
} // namespace Sad
