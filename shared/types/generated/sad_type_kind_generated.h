// ============================================================================
// AUTO-GENERATED FROM language-truth/types.yaml — DO NOT EDIT MANUALLY
// (AR) ملف مولَّد تلقائياً. لإجراء تعديل، عدّل types.yaml وأعد البناء.
// (EN) Auto-generated file. To modify, edit types.yaml and rebuild.
// ============================================================================

#pragma once

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

    } // namespace Types
} // namespace Sad
