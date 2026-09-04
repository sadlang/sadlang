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
         * (AR) عدد القيم: 49 — الترتيب حرج (توافق ثنائي).
         * (EN) 49 values — order is critical (binary compat).
         */
        enum class SadTypeKind : int
        {
            // ─── أنواع بدائية / Primitive types ───
            Void,          ///< فراغ / void
            Integer,       ///< رقم / integer
            Float,         ///< عشري / float
            Boolean,       ///< منطقي / boolean
            String,        ///< نص / string
            // ─── أنواع بدائية بحجم محدد (للمترجم) / Sized primitives (for compiler) ───
            Int8,          ///< رقم8 / int8
            Int16,         ///< رقم16 / int16
            Int32,         ///< رقم32 / int32
            UInt8,         ///< طبيعي8 / uint8
            UInt16,        ///< طبيعي16 / uint16
            UInt32,        ///< طبيعي32 / uint32
            UInt64,        ///< طبيعي / uint64
            Float32,       ///< عشري32 / float32
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
        inline constexpr int SAD_TYPE_KIND_COUNT = 49;

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
            SadDefaultInit::IntZero,     // Int8 — رقم8
            SadDefaultInit::IntZero,     // Int16 — رقم16
            SadDefaultInit::IntZero,     // Int32 — رقم32
            SadDefaultInit::IntZero,     // UInt8 — طبيعي8
            SadDefaultInit::IntZero,     // UInt16 — طبيعي16
            SadDefaultInit::IntZero,     // UInt32 — طبيعي32
            SadDefaultInit::IntZero,     // UInt64 — طبيعي
            SadDefaultInit::Unspecified, // Float32 — عشري32
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
         * @brief (AR) قيمةُ «لا حجمَ ثابتَ لهذا النوع» — تُشخَّصُ ولا تُطبَع
         * @brief (EN) Sentinel: this kind has no fixed slot size — diagnose, never print
         */
        inline constexpr int kSadTypeSizeUnknown = -1;

        /**
         * @brief (AR) جدولُ حجمِ الخانةِ بالبايتات مفهرسًا بـSadTypeKind — مُولَّد
         * @brief (EN) Slot size in bytes indexed by SadTypeKind — generated
         */
        inline constexpr std::array<int, SAD_TYPE_KIND_COUNT> SAD_TYPE_SIZE_BYTES_TABLE = {{
            0,                    // Void — فراغ
            8,                    // Integer — رقم
            8,                    // Float — عشري
            1,                    // Boolean — منطقي
            8,                    // String — نص
            1,                    // Int8 — رقم8
            2,                    // Int16 — رقم16
            4,                    // Int32 — رقم32
            1,                    // UInt8 — طبيعي8
            2,                    // UInt16 — طبيعي16
            4,                    // UInt32 — طبيعي32
            8,                    // UInt64 — طبيعي
            4,                    // Float32 — عشري32
            kSadTypeSizeUnknown,  // Char — حرف
            8,                    // Array — مصفوفة
            8,                    // Map — خريطة
            kSadTypeSizeUnknown,  // Tuple — صف
            kSadTypeSizeUnknown,  // Slice — شريحة
            kSadTypeSizeUnknown,  // Class — صنف
            kSadTypeSizeUnknown,  // Struct — بنية
            kSadTypeSizeUnknown,  // Enum — تعداد
            kSadTypeSizeUnknown,  // Trait — سمة
            8,                    // Function — دالة
            8,                    // Closure — إغلاق
            kSadTypeSizeUnknown,  // Union — اتحاد
            kSadTypeSizeUnknown,  // Intersection — تقاطع
            kSadTypeSizeUnknown,  // Optional — اختياري
            kSadTypeSizeUnknown,  // Result — نتيجة
            kSadTypeSizeUnknown,  // Generic — نوع_عام
            kSadTypeSizeUnknown,  // TypeParameter — معامل_نوع
            kSadTypeSizeUnknown,  // TypeAlias — اسم_مستعار
            8,                    // Pointer — مؤشر
            8,                    // Reference — مرجع
            8,                    // MutableRef — مرجع_متغير
            16,                   // Any — أي
            kSadTypeSizeUnknown,  // Never — أبدا
            kSadTypeSizeUnknown,  // Unknown — مجهول
            8,                    // Error — خطأ
            kSadTypeSizeUnknown,  // Future — مستقبل
            kSadTypeSizeUnknown,  // Generator — مولد
            kSadTypeSizeUnknown,  // Comprehension — استيعاب
            kSadTypeSizeUnknown,  // Color — لون
            kSadTypeSizeUnknown,  // Widget — عنصر_واجهة
            kSadTypeSizeUnknown,  // Window — نافذة
            kSadTypeSizeUnknown,  // Event — حدث
            kSadTypeSizeUnknown,  // Vector — متجه
            8,                    // Null — عدم
            kSadTypeSizeUnknown,  // Point — نقطة
            kSadTypeSizeUnknown,  // Rect — مستطيل
        }};

        /**
         * @brief (AR) حجمُ خانةِ نوعٍ بالبايتات — مُولَّد من types.yaml
         * @brief (EN) Slot size of a kind in bytes — generated from types.yaml
         *
         * (AR) يُرجِعُ kSadTypeSizeUnknown لِما لا حجمَ ثابتَ له (صنفٌ، متّجهٌ،
         *      نوعٌ عامّ) ولِقيمةٍ خارجَ المدى — «لا أعرف» لا رقمًا مُخترَعًا.
         * (EN) Returns kSadTypeSizeUnknown for kinds without a fixed size and
         *      for out-of-range values — «unknown», never an invented number.
         */
        inline constexpr int sadTypeKindSizeBytes(SadTypeKind kind)
        {
            const int index = static_cast<int>(kind);
            if (index < 0 || index >= SAD_TYPE_KIND_COUNT)
            {
                return kSadTypeSizeUnknown;
            }
            return SAD_TYPE_SIZE_BYTES_TABLE[static_cast<std::size_t>(index)];
        }

        // ─── الصفةُ العدديّة / Numeric traits ───
        /**
         * @brief (AR) صنفُ النوعِ العدديّ — مُولَّد من حقلِ numeric في types.yaml
         * @brief (EN) Numeric class of a kind — generated from types.yaml `numeric`
         *
         * (AR) «NotNumeric» صفرٌ عمدًا: خانةٌ مُصفَّرةٌ تعني «ليس عددًا»،
         *      فالاحتياطُ الصامتُ يقعُ على الجوابِ الآمنِ لا على تخمينِ إشارة.
         * (EN) «NotNumeric» is 0 on purpose: a zeroed slot reads as «not a
         *      number», so a silent fallback lands on the safe answer.
         */
        enum class SadNumericClass : int
        {
            NotNumeric,      ///< ليس عددًا / not a number
            SignedInteger,   ///< عدد صحيح موقَّع / signed integer
            UnsignedInteger, ///< عدد صحيح لا-موقَّع / unsigned integer
            Float,           ///< عدد عائم / floating point
        };

        /**
         * @brief (AR) عددُ مفرداتِ SadNumericClass — تستهلكه static_assert عندَ كلِّ
         *             مَن يوزّع عليها، فإضافةُ صنفٍ عدديٍّ جديدٍ **تكسر البناءَ**
         *             بدلَ أن تسقطَ في ذراعٍ افتراضيّةٍ تُخمِّن الإشارة.
         * @brief (EN) SadNumericClass cardinality — consumed by static_assert at
         *             every dispatcher, so a new class BREAKS THE BUILD.
         */
        inline constexpr int SAD_NUMERIC_CLASS_COUNT = 4;

        /**
         * @brief (AR) جدولُ الصنفِ العدديِّ مفهرسًا بقيمةِ SadTypeKind — مُولَّد
         * @brief (EN) Numeric-class table indexed by SadTypeKind — generated
         */
        inline constexpr std::array<SadNumericClass, SAD_TYPE_KIND_COUNT> SAD_TYPE_NUMERIC_CLASS_TABLE = {{
            SadNumericClass::NotNumeric,      // Void — فراغ
            SadNumericClass::SignedInteger,   // Integer — رقم
            SadNumericClass::Float,           // Float — عشري
            SadNumericClass::NotNumeric,      // Boolean — منطقي
            SadNumericClass::NotNumeric,      // String — نص
            SadNumericClass::SignedInteger,   // Int8 — رقم8
            SadNumericClass::SignedInteger,   // Int16 — رقم16
            SadNumericClass::SignedInteger,   // Int32 — رقم32
            SadNumericClass::UnsignedInteger, // UInt8 — طبيعي8
            SadNumericClass::UnsignedInteger, // UInt16 — طبيعي16
            SadNumericClass::UnsignedInteger, // UInt32 — طبيعي32
            SadNumericClass::UnsignedInteger, // UInt64 — طبيعي
            SadNumericClass::Float,           // Float32 — عشري32
            SadNumericClass::NotNumeric,      // Char — حرف
            SadNumericClass::NotNumeric,      // Array — مصفوفة
            SadNumericClass::NotNumeric,      // Map — خريطة
            SadNumericClass::NotNumeric,      // Tuple — صف
            SadNumericClass::NotNumeric,      // Slice — شريحة
            SadNumericClass::NotNumeric,      // Class — صنف
            SadNumericClass::NotNumeric,      // Struct — بنية
            SadNumericClass::NotNumeric,      // Enum — تعداد
            SadNumericClass::NotNumeric,      // Trait — سمة
            SadNumericClass::NotNumeric,      // Function — دالة
            SadNumericClass::NotNumeric,      // Closure — إغلاق
            SadNumericClass::NotNumeric,      // Union — اتحاد
            SadNumericClass::NotNumeric,      // Intersection — تقاطع
            SadNumericClass::NotNumeric,      // Optional — اختياري
            SadNumericClass::NotNumeric,      // Result — نتيجة
            SadNumericClass::NotNumeric,      // Generic — نوع_عام
            SadNumericClass::NotNumeric,      // TypeParameter — معامل_نوع
            SadNumericClass::NotNumeric,      // TypeAlias — اسم_مستعار
            SadNumericClass::NotNumeric,      // Pointer — مؤشر
            SadNumericClass::NotNumeric,      // Reference — مرجع
            SadNumericClass::NotNumeric,      // MutableRef — مرجع_متغير
            SadNumericClass::NotNumeric,      // Any — أي
            SadNumericClass::NotNumeric,      // Never — أبدا
            SadNumericClass::NotNumeric,      // Unknown — مجهول
            SadNumericClass::NotNumeric,      // Error — خطأ
            SadNumericClass::NotNumeric,      // Future — مستقبل
            SadNumericClass::NotNumeric,      // Generator — مولد
            SadNumericClass::NotNumeric,      // Comprehension — استيعاب
            SadNumericClass::NotNumeric,      // Color — لون
            SadNumericClass::NotNumeric,      // Widget — عنصر_واجهة
            SadNumericClass::NotNumeric,      // Window — نافذة
            SadNumericClass::NotNumeric,      // Event — حدث
            SadNumericClass::NotNumeric,      // Vector — متجه
            SadNumericClass::NotNumeric,      // Null — عدم
            SadNumericClass::NotNumeric,      // Point — نقطة
            SadNumericClass::NotNumeric,      // Rect — مستطيل
        }};

        /**
         * @brief (AR) جدولُ «عرضٌ مُصرَّحٌ به» — مُشتقٌّ من category
         * @brief (EN) Declared-width table — derived from `category`
         */
        inline constexpr std::array<bool, SAD_TYPE_KIND_COUNT> SAD_TYPE_SIZED_PRIMITIVE_TABLE = {{
            false, // Void — فراغ
            false, // Integer — رقم
            false, // Float — عشري
            false, // Boolean — منطقي
            false, // String — نص
            true,  // Int8 — رقم8
            true,  // Int16 — رقم16
            true,  // Int32 — رقم32
            true,  // UInt8 — طبيعي8
            true,  // UInt16 — طبيعي16
            true,  // UInt32 — طبيعي32
            true,  // UInt64 — طبيعي
            true,  // Float32 — عشري32
            true,  // Char — حرف
            false, // Array — مصفوفة
            false, // Map — خريطة
            false, // Tuple — صف
            false, // Slice — شريحة
            false, // Class — صنف
            false, // Struct — بنية
            false, // Enum — تعداد
            false, // Trait — سمة
            false, // Function — دالة
            false, // Closure — إغلاق
            false, // Union — اتحاد
            false, // Intersection — تقاطع
            false, // Optional — اختياري
            false, // Result — نتيجة
            false, // Generic — نوع_عام
            false, // TypeParameter — معامل_نوع
            false, // TypeAlias — اسم_مستعار
            false, // Pointer — مؤشر
            false, // Reference — مرجع
            false, // MutableRef — مرجع_متغير
            false, // Any — أي
            false, // Never — أبدا
            false, // Unknown — مجهول
            false, // Error — خطأ
            false, // Future — مستقبل
            false, // Generator — مولد
            false, // Comprehension — استيعاب
            false, // Color — لون
            false, // Widget — عنصر_واجهة
            false, // Window — نافذة
            false, // Event — حدث
            false, // Vector — متجه
            false, // Null — عدم
            false, // Point — نقطة
            false, // Rect — مستطيل
        }};

        /**
         * @brief (AR) أهذا نوعٌ عرضُه مُصرَّحٌ به في الشيفرة؟
         * @brief (EN) Is this a kind whose width the author declared?
         *
         * (AR) كلّيّةٌ بنيويًّا: ما خرجَ عن المدى ليس مُصرَّحًا بعرضِه.
         * (EN) Structurally total: out-of-range is not declared-width.
         */
        inline constexpr bool sadTypeKindIsSizedPrimitive(SadTypeKind kind)
        {
            const int index = static_cast<int>(kind);
            if (index < 0 || index >= SAD_TYPE_KIND_COUNT)
            {
                return false;
            }
            return SAD_TYPE_SIZED_PRIMITIVE_TABLE[static_cast<std::size_t>(index)];
        }


        /**
         * @brief (AR) صنفُ نوعٍ عدديًّا — مُولَّد من types.yaml
         * @brief (EN) Numeric class of a kind — generated from types.yaml
         *
         * (AR) كلّيّةٌ بنيويًّا: قيمةٌ خارجَ المدى تُرجِع «NotNumeric» — أي
         *      «ليس عددًا» لا إشارةً مُخترَعة.
         * (EN) Structurally total: out-of-range yields «NotNumeric», never an
         *      invented signedness.
         */
        inline constexpr SadNumericClass sadTypeKindNumericClass(SadTypeKind kind)
        {
            const int index = static_cast<int>(kind);
            if (index < 0 || index >= SAD_TYPE_KIND_COUNT)
            {
                return SadNumericClass::NotNumeric;
            }
            return SAD_TYPE_NUMERIC_CLASS_TABLE[static_cast<std::size_t>(index)];
        }

        /** @brief (AR) هل النوعُ عددٌ أصلًا؟ / (EN) Is the kind numeric at all? */
        inline constexpr bool sadTypeKindIsNumeric(SadTypeKind kind)
        {
            return sadTypeKindNumericClass(kind) != SadNumericClass::NotNumeric;
        }

        /** @brief (AR) هل هو عددٌ صحيحٌ (موقَّعًا أو لا)؟ / (EN) Integer numeric? */
        inline constexpr bool sadTypeKindIsIntegerNumeric(SadTypeKind kind)
        {
            const SadNumericClass numericClass = sadTypeKindNumericClass(kind);
            return numericClass == SadNumericClass::SignedInteger ||
                   numericClass == SadNumericClass::UnsignedInteger;
        }

        /**
         * @brief (AR) هل هو عددٌ صحيحٌ **لا-موقَّع**؟ — المُسنِدُ الواحدُ الذي كان
         *             مكتوبًا باليدِ في المحرّكَين (`== UInt64` حرفيًّا)، فكان
         *             يعمى عن كلِّ نوعٍ لا-موقَّعٍ آخر.
         * @brief (EN) Unsigned-integer predicate — the one that used to be
         *             hand-written as a literal `== UInt64` in both engines, and
         *             was therefore blind to every other unsigned kind.
         */
        inline constexpr bool sadTypeKindIsUnsignedInteger(SadTypeKind kind)
        {
            return sadTypeKindNumericClass(kind) == SadNumericClass::UnsignedInteger;
        }

        /** @brief (AR) هل هو عددٌ عائم؟ / (EN) Floating-point numeric? */
        inline constexpr bool sadTypeKindIsFloatNumeric(SadTypeKind kind)
        {
            return sadTypeKindNumericClass(kind) == SadNumericClass::Float;
        }

        /**
         * @brief (AR) عرضُ النوعِ العدديِّ بالبتّات — مشتقٌّ من size_bytes المُعلَن
         * @brief (EN) Numeric width in bits — derived from the declared size_bytes
         *
         * (AR) يُرجِع kSadTypeSizeUnknown لِما ليس عددًا أو لا عرضَ مُعلَنَ له.
         * (EN) Yields kSadTypeSizeUnknown for non-numeric or width-less kinds.
         */
        inline constexpr int sadTypeKindNumericBits(SadTypeKind kind)
        {
            if (!sadTypeKindIsNumeric(kind))
            {
                return kSadTypeSizeUnknown;
            }
            const int bytes = sadTypeKindSizeBytes(kind);
            return bytes == kSadTypeSizeUnknown ? kSadTypeSizeUnknown : bytes * 8;
        }

        /**
         * @brief (AR) أصغرُ قيمةٍ يحملها نوعٌ صحيح — ٠ لِلّا-موقَّع
         * @brief (EN) Minimum value an integer kind holds — 0 when unsigned
         */
        inline constexpr long long sadTypeKindIntegerMin(SadTypeKind kind)
        {
            const int bits = sadTypeKindNumericBits(kind);
            if (!sadTypeKindIsIntegerNumeric(kind) || bits == kSadTypeSizeUnknown)
            {
                return 0;
            }
            if (sadTypeKindIsUnsignedInteger(kind))
            {
                return 0;
            }
            if (bits >= 64)
            {
                return (-9223372036854775807LL - 1);
            }
            return -(1LL << (bits - 1));
        }

        /**
         * @brief (AR) أكبرُ قيمةٍ يحملها نوعٌ صحيح — لا-موقَّعةٌ لتسعَ ٢^٦٤−١
         * @brief (EN) Maximum value an integer kind holds — unsigned so 2^64-1 fits
         */
        inline constexpr unsigned long long sadTypeKindIntegerMax(SadTypeKind kind)
        {
            const int bits = sadTypeKindNumericBits(kind);
            if (!sadTypeKindIsIntegerNumeric(kind) || bits == kSadTypeSizeUnknown)
            {
                return 0ULL;
            }
            if (sadTypeKindIsUnsignedInteger(kind))
            {
                return bits >= 64 ? ~0ULL : ((1ULL << bits) - 1ULL);
            }
            return bits >= 64 ? 9223372036854775807ULL : ((1ULL << (bits - 1)) - 1ULL);
        }

        /**
         * @brief (AR) بترُ قيمةٍ إلى العرضِ المُعلَنِ ثمّ تمديدُها بحسبِ الإشارة —
         *             العمليّةُ الواحدةُ التي تُطبَّقُ عندَ **كلِّ** معبر: التصريح،
         *             والمعامل، وحقلُ الصنف، وناتجُ الحساب.
         *
         *             🔑 وكانت مكتوبةً في معبرٍ دونَ معبر، فَسُدَّ التصريحُ وبقي
         *             حقلُ الصنفِ مفتوحًا: `بايت م = 300` تُعطي ٤٤ و`ك.ح = 300`
         *             تُعطي ٣٠٠. معبرٌ واحدٌ منسيٌّ يُبطِلُ عقدَ النوعِ كلَّه.
         *
         * @brief (EN) Truncate a value to the declared width, then extend by
         *             signedness — the single operation that must be applied at
         *             EVERY crossing: declaration, parameter, class field, and
         *             arithmetic result. It used to be written at one crossing
         *             and not another, which is why `بايت م = 300` yields 44 while
         *             `obj.field = 300` yields 300. One forgotten crossing voids
         *             the whole type contract.
         *
         * (AR) هُويّةٌ لِما ليس صحيحًا أو لا عرضَ له — لا تخترعُ قيمةً لِما تجهل.
         * (EN) Identity for non-integer or width-less kinds — never invents.
         */
        inline constexpr long long sadTypeKindNormalizeInteger(SadTypeKind kind,
                                                               long long value)
        {
            const int bits = sadTypeKindNumericBits(kind);
            if (!sadTypeKindIsIntegerNumeric(kind) || bits == kSadTypeSizeUnknown ||
                bits >= 64)
            {
                return value;
            }
            const unsigned long long mask = (1ULL << bits) - 1ULL;
            const unsigned long long masked =
                static_cast<unsigned long long>(value) & mask;
            if (sadTypeKindIsUnsignedInteger(kind))
            {
                return static_cast<long long>(masked);
            }
            const unsigned long long signBit = 1ULL << (bits - 1);
            if ((masked & signBit) != 0ULL)
            {
                return static_cast<long long>(masked | ~mask);
            }
            return static_cast<long long>(masked);
        }

        /**
         * @brief (AR) هل تسعُ القيمةُ في النوعِ بلا بتر؟ — للتشخيصِ لا للبتر
         * @brief (EN) Does the value fit the kind without truncation? — for
         *             diagnostics, not for truncating
         */
        inline constexpr bool sadTypeKindIntegerFits(SadTypeKind kind, long long value)
        {
            if (!sadTypeKindIsIntegerNumeric(kind))
            {
                return false;
            }
            if (sadTypeKindIsUnsignedInteger(kind))
            {
                return value >= 0 &&
                       static_cast<unsigned long long>(value) <=
                           sadTypeKindIntegerMax(kind);
            }
            return value >= sadTypeKindIntegerMin(kind) &&
                   value <= static_cast<long long>(sadTypeKindIntegerMax(kind));
        }

        /**
         * @brief (AR) نوعُ ناتجِ الخلطِ بين معامِلَين — قاعدةُ الهيمنةِ **الواحدة**
         *
         *             القاعدة: العائمُ يهيمن؛ فإن كانا صحيحَين فالأعرضُ يفوز،
         *             وعندَ تساوي العرضِ يفوزُ اللا-موقَّع.
         *
         *             🔑 وكانت القاعدةُ محصورةً في أسماءٍ بعينِها
         *             (`عشري ← طبيعي ← بايت ← رقم`) مكتوبةً مرّتَين — مرّةً في
         *             كلِّ محرّك — ويقولُ تعليقُ المفسّرِ إنّ على المترجِمِ تطبيقَها
         *             «بالمطابقة»، وهو شرطٌ **مُعلَنٌ لا يقيسُه أحد**. وفيها خللٌ
         *             كامنٌ يظهرُ لحظةَ تنفيذِ البتر: `بايت + رقم` تُوسَمُ «بايت»،
         *             فيصيرُ ٢٠٠ + ١٠٠ يساوي ٤٤ بدلَ ٣٠٠. والقاعدةُ العامّةُ ههنا
         *             تُعطي «رقم» فتحفظُ السلوكَ المقيس.
         *
         * @brief (EN) Result kind of mixing two operands — the SINGLE dominance
         *             rule. Float dominates; otherwise the wider integer wins, and
         *             on equal width the unsigned one wins.
         *
         *             It used to be a name-bound ladder written once per engine,
         *             with the interpreter's comment declaring that the compiler
         *             must apply it «identically» — a contract nobody measured.
         *             It also carried a latent defect that surfaces the moment
         *             truncation lands: `Byte + Integer` was tagged Byte, which
         *             would turn 200 + 100 into 44 instead of 300.
         *
         * (AR) وإن لم يكن أحدُ المعامِلَين عددًا رجعَ «رقم» — وهو الاحتياطُ الذي
         *      كان قائمًا قبلَ التوحيد، أُبقيَ كما هو كي لا يتغيّرَ سلوكٌ مقيس.
         * (EN) If either operand is non-numeric the result is Integer — the
         *      pre-unification fallback, preserved so no measured behaviour moves.
         */
        inline constexpr SadTypeKind sadNumericDominantKind(SadTypeKind left,
                                                            SadTypeKind right)
        {
            const bool leftFloat = sadTypeKindIsFloatNumeric(left);
            const bool rightFloat = sadTypeKindIsFloatNumeric(right);
            if (leftFloat || rightFloat)
            {
                if (leftFloat && rightFloat)
                {
                    return sadTypeKindNumericBits(right) > sadTypeKindNumericBits(left)
                               ? right
                               : left;
                }
                return leftFloat ? left : right;
            }
            if (!sadTypeKindIsIntegerNumeric(left) || !sadTypeKindIsIntegerNumeric(right))
            {
                return SadTypeKind::Integer;
            }
            const int leftBits = sadTypeKindNumericBits(left);
            const int rightBits = sadTypeKindNumericBits(right);
            if (leftBits != rightBits)
            {
                return leftBits > rightBits ? left : right;
            }
            if (sadTypeKindIsUnsignedInteger(left) != sadTypeKindIsUnsignedInteger(right))
            {
                return sadTypeKindIsUnsignedInteger(left) ? left : right;
            }
            return left;
        }

        // ─── برهانُ الاشتقاق / Derivation proof ───
        // (AR) قيمٌ حسبتها پايثون استقلالًا — التباعدُ يكسرُ البناء
        // (EN) Python-computed values — drift breaks the build
        // Integer — رقم (64 bit, signed)
        static_assert(sadTypeKindNumericBits(SadTypeKind::Integer) == 64);
        static_assert(sadTypeKindIsUnsignedInteger(SadTypeKind::Integer) == false);
        static_assert(sadTypeKindIntegerMin(SadTypeKind::Integer) == -9223372036854775808LL);
        static_assert(sadTypeKindIntegerMax(SadTypeKind::Integer) == 9223372036854775807ULL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Integer, 0LL) == 0LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Integer, 1LL) == 1LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Integer, 200LL) == 200LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Integer, 255LL) == 255LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Integer, 300LL) == 300LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Integer, -1LL) == -1LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Integer, 65536LL) == 65536LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Integer, 4294967296LL) == 4294967296LL);
        // Int8 — رقم8 (8 bit, signed)
        static_assert(sadTypeKindNumericBits(SadTypeKind::Int8) == 8);
        static_assert(sadTypeKindIsUnsignedInteger(SadTypeKind::Int8) == false);
        static_assert(sadTypeKindIntegerMin(SadTypeKind::Int8) == -128LL);
        static_assert(sadTypeKindIntegerMax(SadTypeKind::Int8) == 127ULL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int8, 0LL) == 0LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int8, 1LL) == 1LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int8, 200LL) == -56LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int8, 255LL) == -1LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int8, 300LL) == 44LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int8, -1LL) == -1LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int8, 65536LL) == 0LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int8, 4294967296LL) == 0LL);
        // Int16 — رقم16 (16 bit, signed)
        static_assert(sadTypeKindNumericBits(SadTypeKind::Int16) == 16);
        static_assert(sadTypeKindIsUnsignedInteger(SadTypeKind::Int16) == false);
        static_assert(sadTypeKindIntegerMin(SadTypeKind::Int16) == -32768LL);
        static_assert(sadTypeKindIntegerMax(SadTypeKind::Int16) == 32767ULL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int16, 0LL) == 0LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int16, 1LL) == 1LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int16, 200LL) == 200LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int16, 255LL) == 255LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int16, 300LL) == 300LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int16, -1LL) == -1LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int16, 65536LL) == 0LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int16, 4294967296LL) == 0LL);
        // Int32 — رقم32 (32 bit, signed)
        static_assert(sadTypeKindNumericBits(SadTypeKind::Int32) == 32);
        static_assert(sadTypeKindIsUnsignedInteger(SadTypeKind::Int32) == false);
        static_assert(sadTypeKindIntegerMin(SadTypeKind::Int32) == -2147483648LL);
        static_assert(sadTypeKindIntegerMax(SadTypeKind::Int32) == 2147483647ULL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int32, 0LL) == 0LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int32, 1LL) == 1LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int32, 200LL) == 200LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int32, 255LL) == 255LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int32, 300LL) == 300LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int32, -1LL) == -1LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int32, 65536LL) == 65536LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::Int32, 4294967296LL) == 0LL);
        // UInt8 — طبيعي8 (8 bit, unsigned)
        static_assert(sadTypeKindNumericBits(SadTypeKind::UInt8) == 8);
        static_assert(sadTypeKindIsUnsignedInteger(SadTypeKind::UInt8) == true);
        static_assert(sadTypeKindIntegerMin(SadTypeKind::UInt8) == 0LL);
        static_assert(sadTypeKindIntegerMax(SadTypeKind::UInt8) == 255ULL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt8, 0LL) == 0LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt8, 1LL) == 1LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt8, 200LL) == 200LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt8, 255LL) == 255LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt8, 300LL) == 44LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt8, -1LL) == 255LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt8, 65536LL) == 0LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt8, 4294967296LL) == 0LL);
        // UInt16 — طبيعي16 (16 bit, unsigned)
        static_assert(sadTypeKindNumericBits(SadTypeKind::UInt16) == 16);
        static_assert(sadTypeKindIsUnsignedInteger(SadTypeKind::UInt16) == true);
        static_assert(sadTypeKindIntegerMin(SadTypeKind::UInt16) == 0LL);
        static_assert(sadTypeKindIntegerMax(SadTypeKind::UInt16) == 65535ULL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt16, 0LL) == 0LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt16, 1LL) == 1LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt16, 200LL) == 200LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt16, 255LL) == 255LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt16, 300LL) == 300LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt16, -1LL) == 65535LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt16, 65536LL) == 0LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt16, 4294967296LL) == 0LL);
        // UInt32 — طبيعي32 (32 bit, unsigned)
        static_assert(sadTypeKindNumericBits(SadTypeKind::UInt32) == 32);
        static_assert(sadTypeKindIsUnsignedInteger(SadTypeKind::UInt32) == true);
        static_assert(sadTypeKindIntegerMin(SadTypeKind::UInt32) == 0LL);
        static_assert(sadTypeKindIntegerMax(SadTypeKind::UInt32) == 4294967295ULL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt32, 0LL) == 0LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt32, 1LL) == 1LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt32, 200LL) == 200LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt32, 255LL) == 255LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt32, 300LL) == 300LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt32, -1LL) == 4294967295LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt32, 65536LL) == 65536LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt32, 4294967296LL) == 0LL);
        // UInt64 — طبيعي (64 bit, unsigned)
        static_assert(sadTypeKindNumericBits(SadTypeKind::UInt64) == 64);
        static_assert(sadTypeKindIsUnsignedInteger(SadTypeKind::UInt64) == true);
        static_assert(sadTypeKindIntegerMin(SadTypeKind::UInt64) == 0LL);
        static_assert(sadTypeKindIntegerMax(SadTypeKind::UInt64) == 18446744073709551615ULL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt64, 0LL) == 0LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt64, 1LL) == 1LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt64, 200LL) == 200LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt64, 255LL) == 255LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt64, 300LL) == 300LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt64, -1LL) == -1LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt64, 65536LL) == 65536LL);
        static_assert(sadTypeKindNormalizeInteger(SadTypeKind::UInt64, 4294967296LL) == 4294967296LL);

        // ─── دلالةُ القيمة / Value semantics ───
        /**
         * @brief (AR) جدولُ دلالةِ القيمة — مُولَّدٌ من `value_semantics` في types.yaml
         * @brief (EN) Value-semantics table — generated from types.yaml `value_semantics`
         */
        inline constexpr bool SAD_TYPE_IS_COPY_TABLE[] = {
            true ,  // Void          — فراغ
            true ,  // Integer       — رقم
            true ,  // Float         — عشري
            true ,  // Boolean       — منطقي
            false,  // String        — نص
            true ,  // Int8          — رقم8
            true ,  // Int16         — رقم16
            true ,  // Int32         — رقم32
            true ,  // UInt8         — طبيعي8
            true ,  // UInt16        — طبيعي16
            true ,  // UInt32        — طبيعي32
            true ,  // UInt64        — طبيعي
            true ,  // Float32       — عشري32
            true ,  // Char          — حرف
            false,  // Array         — مصفوفة
            false,  // Map           — خريطة
            false,  // Tuple         — صف
            false,  // Slice         — شريحة
            false,  // Class         — صنف
            false,  // Struct        — بنية
            false,  // Enum          — تعداد
            false,  // Trait         — سمة
            true ,  // Function      — دالة
            false,  // Closure       — إغلاق
            false,  // Union         — اتحاد
            false,  // Intersection  — تقاطع
            false,  // Optional      — اختياري
            false,  // Result        — نتيجة
            true ,  // Generic       — نوع_عام
            true ,  // TypeParameter — معامل_نوع
            true ,  // TypeAlias     — اسم_مستعار
            true ,  // Pointer       — مؤشر
            true ,  // Reference     — مرجع
            true ,  // MutableRef    — مرجع_متغير
            false,  // Any           — أي
            true ,  // Never         — أبدا
            true ,  // Unknown       — مجهول
            false,  // Error         — خطأ
            false,  // Future        — مستقبل
            false,  // Generator     — مولد
            false,  // Comprehension — استيعاب
            true ,  // Color         — لون
            false,  // Widget        — عنصر_واجهة
            false,  // Window        — نافذة
            false,  // Event         — حدث
            true ,  // Vector        — متجه
            true ,  // Null          — عدم
            true ,  // Point         — نقطة
            true ,  // Rect          — مستطيل
        };

        /**
         * @brief (AR) هل يُنسَخُ النوعُ فيبقى المصدرُ صالحًا؟
         * @brief (EN) Is the kind Copy (source stays valid after use)?
         *
         * (AR) مفتاحٌ خارجَ المدى يُجابُ بـ«يُنسَخ» لا بـ«يُنقَل»: النقلُ
         *      يُحمِّرُ برنامجًا، فلا يُحكَمُ به على مفتاحٍ مجهول.
         * (EN) An out-of-range kind answers Copy, never Move: Move reddens a
         *      program, and an unknown kind must not redden one.
         */
        inline constexpr bool sadTypeKindIsCopy(SadTypeKind kind)
        {
            const int index = static_cast<int>(kind);
            if (index < 0 ||
                index >= static_cast<int>(sizeof(SAD_TYPE_IS_COPY_TABLE) /
                                          sizeof(SAD_TYPE_IS_COPY_TABLE[0])))
            {
                return true;
            }
            return SAD_TYPE_IS_COPY_TABLE[static_cast<std::size_t>(index)];
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
            case SadTypeKind::Int8: return "\xd8\xb1\xd9\x82\xd9\x85\x38"; // رقم8
            case SadTypeKind::Int16: return "\xd8\xb1\xd9\x82\xd9\x85\x31\x36"; // رقم16
            case SadTypeKind::Int32: return "\xd8\xb1\xd9\x82\xd9\x85\x33\x32"; // رقم32
            case SadTypeKind::UInt8: return "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x38"; // طبيعي8
            case SadTypeKind::UInt16: return "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x31\x36"; // طبيعي16
            case SadTypeKind::UInt32: return "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x33\x32"; // طبيعي32
            case SadTypeKind::UInt64: return "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a"; // طبيعي
            case SadTypeKind::Float32: return "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a\x33\x32"; // عشري32
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
            if (word == "\xd8\xb1\xd9\x82\xd9\x85\x38") return SadTypeKind::Int8; // رقم8
            if (word == "\xd8\xb1\xd9\x82\xd9\x85\x31\x36") return SadTypeKind::Int16; // رقم16
            if (word == "\xd8\xb1\xd9\x82\xd9\x85\x33\x32") return SadTypeKind::Int32; // رقم32
            if (word == "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x38") return SadTypeKind::UInt8; // طبيعي8
            if (word == "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x31\x36") return SadTypeKind::UInt16; // طبيعي16
            if (word == "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x33\x32") return SadTypeKind::UInt32; // طبيعي32
            if (word == "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a") return SadTypeKind::UInt64; // طبيعي
            if (word == "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a\x33\x32") return SadTypeKind::Float32; // عشري32
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
         * (AR) العدد: 17 — يستهلكها مزوّد الرموز الدلاليّة لتلوينها أنواعًا.
         * (EN) 17 names — consumed by the semantic-tokens provider.
         */
        inline constexpr std::array<std::string_view, 17> SURFACE_TYPE_NAMES = {{
            "\xd9\x81\xd8\xb1\xd8\xa7\xd8\xba", // فراغ
            "\xd8\xb1\xd9\x82\xd9\x85", // رقم
            "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a", // عشري
            "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a", // منطقي
            "\xd9\x86\xd8\xb5", // نص
            "\xd8\xb1\xd9\x82\xd9\x85\x38", // رقم8
            "\xd8\xb1\xd9\x82\xd9\x85\x31\x36", // رقم16
            "\xd8\xb1\xd9\x82\xd9\x85\x33\x32", // رقم32
            "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x38", // طبيعي8
            "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x31\x36", // طبيعي16
            "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x33\x32", // طبيعي32
            "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a", // طبيعي
            "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a\x33\x32", // عشري32
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
            if (word == "\xd8\xb1\xd9\x82\xd9\x85\x38") return "\xd8\xb9\xd8\xaf\xd8\xaf\x20\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad\x20\x38\x2d\xd8\xa8\xd8\xaa"; // رقم8
            if (word == "\xd8\xb1\xd9\x82\xd9\x85\x31\x36") return "\xd8\xb9\xd8\xaf\xd8\xaf\x20\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad\x20\x31\x36\x2d\xd8\xa8\xd8\xaa"; // رقم16
            if (word == "\xd8\xb1\xd9\x82\xd9\x85\x33\x32") return "\xd8\xb9\xd8\xaf\xd8\xaf\x20\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad\x20\x33\x32\x2d\xd8\xa8\xd8\xaa"; // رقم32
            if (word == "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x38") return "\xd8\xb9\xd8\xaf\xd8\xaf\x20\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x20\x38\x2d\xd8\xa8\xd8\xaa\x20\x28\x30\xe2\x80\x93\x32\x35\x35\x29"; // طبيعي8
            if (word == "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x31\x36") return "\xd8\xb9\xd8\xaf\xd8\xaf\x20\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x20\x31\x36\x2d\xd8\xa8\xd8\xaa"; // طبيعي16
            if (word == "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x33\x32") return "\xd8\xb9\xd8\xaf\xd8\xaf\x20\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x20\x33\x32\x2d\xd8\xa8\xd8\xaa"; // طبيعي32
            if (word == "\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a") return "\xd8\xb9\xd8\xaf\xd8\xaf\x20\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x8a\x20\x36\x34\x2d\xd8\xa8\xd8\xaa"; // طبيعي
            if (word == "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a\x33\x32") return "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a\x20\x33\x32\x2d\xd8\xa8\xd8\xaa"; // عشري32
            if (word == "\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9") return "\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9\x3c\x54\x3e\x20\xd8\xaf\xd9\x8a\xd9\x86\xd8\xa7\xd9\x85\xd9\x8a\xd9\x83\xd9\x8a\xd8\xa9"; // مصفوفة
            if (word == "\xd8\xae\xd8\xb1\xd9\x8a\xd8\xb7\xd8\xa9") return "\xd8\xae\xd8\xb1\xd9\x8a\xd8\xb7\xd8\xa9\x3c\x4b\x2c\x56\x3e"; // خريطة
            if (word == "\xd8\xa3\xd9\x8a") return "\xd8\xa3\xd9\x8a\x20\xd9\x86\xd9\x88\xd8\xb9\x20\x28\xd8\xaf\xd9\x8a\xd9\x86\xd8\xa7\xd9\x85\xd9\x8a\xd9\x83\xd9\x8a\x29"; // أي
            if (word == "\xd8\xb9\xd8\xaf\xd9\x85") return "\xd8\xa7\xd9\x84\xd8\xb9\xd8\xaf\xd9\x85\x20\xe2\x80\x94\x20\xd8\xba\xd9\x8a\xd8\xa7\xd8\xa8\x20\xd8\xa7\xd9\x84\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9\xd8\x8c\x20\xd9\x85\xd8\xaa\xd9\x85\xd8\xa7\xd9\x8a\xd8\xb2\x20\xd8\xb9\xd9\x86\x20\xd9\x81\xd8\xb1\xd8\xa7\xd8\xba\x20\x28\x76\x6f\x69\x64\x29\x2e\x20\xd9\x88\xd8\xaa\xd9\x85\xd8\xab\xd9\x8a\xd9\x84\xd9\x8f\xd9\x87\x20\xd8\xad\xd8\xa7\xd8\xb1\xd8\xb3\xd9\x8c\x20\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad\xd9\x8f\x20\xd8\xa7\xd9\x84\xd8\xa8\xd8\xaa\xd9\x91\xd8\xa7\xd8\xaa\xd8\x8c\x20\xd9\x81\xd8\xa7\xd9\x84\xd8\xad\xd9\x83\xd9\x85\xd9\x8f\x20\xd8\xb9\xd9\x84\xd9\x8a\xd9\x87\x20\xd9\x8a\xd9\x84\xd8\xb2\xd9\x85\xd9\x8f\x20\xd8\xa3\xd9\x86\x20\xd9\x8a\xd9\x82\xd9\x88\xd9\x85\xd9\x8e\x20\xd8\xb9\xd9\x84\xd9\x89\x20\xd9\x88\xd8\xb3\xd9\x85\xd9\x90\x20\xd8\xa7\xd9\x84\xd9\x86\xd9\x88\xd8\xb9\xd9\x90\x20\xd9\x84\xd8\xa7\x20\xd8\xb9\xd9\x84\xd9\x89\x20\xd8\xb4\xd9\x83\xd9\x84\xd9\x90\x20\xd8\xa7\xd9\x84\xd8\xaa\xd9\x85\xd8\xab\xd9\x8a\xd9\x84\x3a\x20\xd9\x85\xd9\x82\xd8\xa7\xd8\xb1\xd9\x86\xd8\xa9\xd9\x8f\x20\xd8\xa7\xd9\x84\xd8\xb9\xd8\xaf\xd9\x85\xd9\x90\x20\xd8\xa8\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9\xd9\x8d\x20\xd9\x85\xd9\x86\x20\xd9\x86\xd9\x88\xd8\xb9\xd9\x8d\x20\xd8\xa2\xd8\xae\xd8\xb1\x20\xd8\xaa\xd9\x8f\xd8\xad\xd8\xb3\xd9\x8e\xd9\x85\xd9\x8f\x20\xd8\xa8\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd8\xb9\xd9\x90\x20\xd8\xa7\xd9\x84\xd8\xb7\xd8\xb1\xd9\x81\xd9\x90\x20\xd8\xa7\xd9\x84\xd8\xa2\xd8\xae\xd8\xb1\xd9\x90\x20\xd8\xa5\xd9\x84\xd9\x89\x20\xd8\xb9\xd8\xb1\xd8\xb6\xd9\x90\x20\xd8\xa7\xd9\x84\xd8\xad\xd8\xa7\xd8\xb1\xd8\xb3\xd9\x90\x20\xd9\x88\xd9\x85\xd9\x82\xd8\xa7\xd8\xb1\xd9\x86\xd8\xaa\xd9\x90\xd9\x87\x20\xd8\xa8\xd9\x87\x20\xd8\xb9\xd8\xaf\xd8\xaf\xd9\x8a\xd9\x8b\xd9\x91\xd8\xa7\xd8\x8c\x20\xd9\x81\xd9\x84\xd8\xa7\x20\xd8\xaa\xd9\x8f\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xb9\xd9\x89\x20\xd9\x85\xd9\x82\xd8\xa7\xd8\xb1\xd9\x86\xd8\xa9\xd9\x8f\x20\xd9\x86\xd8\xb5\xd9\x88\xd8\xb5\xd9\x8d\x20\xd8\xb9\xd9\x84\xd9\x89\x20\xd8\xa7\xd9\x84\xd8\xad\xd8\xa7\xd8\xb1\xd8\xb3\xd9\x90\x20\xd8\xa3\xd8\xa8\xd8\xaf\xd9\x8b\xd8\xa7\xd8\x9b\x20\xd9\x88\xc2\xab\xd8\xb9\xd8\xaf\xd9\x85\x20\x3d\x3d\x20\xd8\xb9\xd8\xaf\xd9\x85\xc2\xbb\x20\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad\x2e\x20\xd9\x88\xd8\xa7\xd9\x84\xd9\x88\xd8\xb3\xd9\x85\xd9\x8f\x20\xd8\xa7\xd9\x84\xd8\xb3\xd8\xa7\xd9\x83\xd9\x86\xd9\x8f\x20\xd9\x84\xd8\xa7\x20\xd9\x8a\xd9\x83\xd9\x81\xd9\x8a\x20\xd9\x88\xd8\xad\xd8\xaf\xd9\x8e\xd9\x87\x20\xd9\x88\xd9\x84\xd8\xa7\x20\xd9\x8a\xd9\x85\xd9\x83\xd9\x86\xd9\x8f\x20\xd8\xa3\xd9\x86\x20\xd9\x8a\xd9\x83\xd9\x81\xd9\x8a\x3a\x20\xd8\xae\xd8\xa7\xd9\x86\xd8\xa9\xd9\x8c\x20\xd9\x86\xd9\x88\xd8\xb9\xd9\x8f\xd9\x87\xd8\xa7\x20\xd8\xa7\xd9\x84\xd9\x85\xd9\x8f\xd8\xb5\xd8\xb1\xd9\x8e\xd9\x91\xd8\xad\xd9\x8f\x20\xc2\xab\xd9\x86\xd8\xb5\xd9\x91\xc2\xbb\x20\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84\xd9\x8f\x20\xd9\x86\xd8\xb5\xd9\x8b\xd9\x91\xd8\xa7\x20\xd9\x81\xd9\x8a\x20\xd9\x85\xd9\x88\xd9\x82\xd8\xb9\xd9\x90\x20\xd9\x86\xd8\xaf\xd8\xa7\xd8\xa1\xd9\x8d\x20\xd9\x88\xd8\xb9\xd8\xaf\xd9\x85\xd9\x8b\xd8\xa7\x20\xd9\x81\xd9\x8a\x20\xd8\xa2\xd8\xae\xd8\xb1\xd8\x8c\x20\xd9\x81\xd9\x84\xd8\xa7\x20\xd9\x88\xd8\xb3\xd9\x85\xd9\x8e\x20\xd8\xb3\xd8\xa7\xd9\x83\xd9\x86\xd9\x8b\xd8\xa7\x20\xd9\x88\xd8\xa7\xd8\xad\xd8\xaf\xd9\x8b\xd8\xa7\x20\xd9\x8a\xd8\xb5\xd8\xaf\xd9\x8f\xd9\x82\xd9\x8f\x20\xd8\xb9\xd9\x84\xd9\x89\x20\xd8\xa7\xd9\x84\xd8\xa7\xd8\xab\xd9\x86\xd9\x8e\xd9\x8a\xd9\x86\x20\xe2\x80\x94\x20\xd9\x88\xd8\xa7\xd9\x84\xd8\xb3\xd8\xa4\xd8\xa7\xd9\x84\xd9\x8f\x20\xd9\x86\xd9\x81\xd8\xb3\xd9\x8f\xd9\x87\x20\xd8\xb3\xd8\xa4\xd8\xa7\xd9\x84\xd9\x8f\x20\xd8\xb2\xd9\x85\xd9\x86\xd9\x90\x20\xd8\xaa\xd8\xb4\xd8\xba\xd9\x8a\xd9\x84\x2e\x20\xd9\x84\xd8\xb0\xd9\x84\xd9\x83\xd9\x8e\x20\xd9\x85\xd9\x82\xd8\xa7\xd8\xb1\xd9\x86\xd8\xa9\xd9\x8f\x20\xd8\xa7\xd9\x84\xd9\x86\xd8\xb5\xd9\x88\xd8\xb5\xd9\x90\x20\xd8\xa2\xd9\x85\xd9\x86\xd8\xa9\xd9\x8c\x20\xd9\x85\xd9\x86\x20\xd8\xa7\xd9\x84\xd8\xb9\xd8\xaf\xd9\x85\xd9\x90\x20\xd9\x88\xd9\x82\xd8\xaa\xd9\x8e\x20\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb4\xd8\xba\xd9\x8a\xd9\x84\x3a\x20\xd9\x8a\xd9\x8f\xd9\x81\xd8\xad\xd9\x8e\xd8\xb5\xd9\x8f\x20\xd8\xa7\xd9\x84\xd8\xb7\xd8\xb1\xd9\x81\xd8\xa7\xd9\x86\xd9\x90\x20\xd8\xb9\xd9\x84\xd9\x89\x20\xd8\xa7\xd9\x84\xd8\xad\xd8\xa7\xd8\xb1\xd8\xb3\xd9\x90\x20\xd9\x82\xd8\xa8\xd9\x84\xd9\x8e\x20\xd8\xa3\xd9\x8a\xd9\x90\xd9\x91\x20\xd9\x82\xd8\xb1\xd8\xa7\xd8\xa1\xd8\xa9\xd9\x90\x20\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9\xd8\x8c\x20\xd9\x81\xd8\xa7\xd9\x84\xd8\xb9\xd8\xaf\xd9\x85\xd9\x8f\x20\xd9\x8a\xd8\xb3\xd8\xa7\xd9\x88\xd9\x8a\x20\xd8\xa7\xd9\x84\xd8\xb9\xd8\xaf\xd9\x85\xd9\x8e\x20\xd9\x88\xd9\x84\xd8\xa7\x20\xd9\x8a\xd8\xb3\xd8\xa7\xd9\x88\xd9\x8a\x20\xd9\x86\xd8\xb5\xd9\x8b\xd9\x91\xd8\xa7\xd8\x8c\x20\xd9\x88\xd9\x84\xd8\xa7\x20\xd9\x8a\xd9\x8f\xd9\x85\xd8\xb1\xd9\x8e\xd9\x91\xd8\xb1\xd9\x8f\x20\xd8\xa7\xd9\x84\xd8\xad\xd8\xa7\xd8\xb1\xd8\xb3\xd9\x8f\x20\xd8\xa5\xd9\x84\xd9\x89\x20\xd9\x85\xd9\x82\xd8\xa7\xd8\xb1\xd9\x86\xd8\xa9\xd9\x90\x20\xd9\x86\xd8\xb5\xd9\x88\xd8\xb5\xd9\x8d\x20\xd8\xa3\xd8\xa8\xd8\xaf\xd9\x8b\xd8\xa7"; // عدم
            return "";
        }

    } // namespace Types
} // namespace Sad
