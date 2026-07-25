/**
 * @file tagged_enum_keys.h
 * @brief (AR) مفاتيح التمثيل الزمن-تشغيليّ للتعداد بحمولة (القيمة الموسومة)
 *        (EN) Runtime-representation keys for tagged enums (ADT tagged value)
 *
 * (AR) القيمة الموسومة لعضو تعداد بحمولة تُمثَّل زمن التشغيل كخريطة (Map) بمفاتيح
 *      محجوزة: اسم التعداد، اسم العضو، ومصفوفة الحمولة. تُجمَّع هنا كثوابت مسمّاة
 *      وحيدة المصدر (SoT) كي لا تتكرّر السلاسل الخام عبر المحلّل الدلاليّ/المفسّر/المترجم
 *      (يسدّ دَين أميليا 🔵٦ الموروث في pattern_nodes.h).
 * (EN) The tagged value of a payload-carrying enum variant is represented at runtime as a
 *      Map with reserved keys: enum name, variant name, and the payload array. Collected
 *      here as single-source-of-truth named constants so the raw strings are not duplicated
 *      across semantic/interpreter/compiler (sinks inherited Amelia debt 🔵6 in pattern_nodes.h).
 *
 * @author Sad Language Team
 * @version 1.0.0
 */

#pragma once

namespace Sad
{
    namespace AST
    {
        namespace TaggedEnumKeys
        {
            /// (AR) مفتاح اسم التعداد المالك / (EN) Owning enum name key
            inline constexpr const char *ENUM = "__تعداد__";

            /// (AR) مفتاح اسم العضو / (EN) Variant (member) name key
            inline constexpr const char *VARIANT = "__عضو__";

            /// (AR) مفتاح مصفوفة الحمولة الموضعيّة / (EN) Positional payload array key
            inline constexpr const char *FIELDS = "__حقول__";

            /// (AR) علامة قاطعة: القيمة موسومة لتعدادٍ جبريّ (يضبطها الباني `=true`).
            ///      مفتاحٌ داخليٌّ حصريّ لا يكتبه المستخدم عادةً — يميّز القيمةَ الموسومة عن
            ///      خريطةِ مستخدمٍ عاديّةٍ تصادف حملَ __تعداد__/__عضو__/__حقول__ (تفادي إيجابيّة
            ///      كاذبة في كاشف الطبع — عيب أميليا ع-٣).
            /// (EN) Decisive marker: the value is a tagged algebraic-enum value (the constructor
            ///      sets it `=true`). An exclusive internal key a user does not normally write —
            ///      distinguishes a tagged value from an ordinary user map that happens to carry
            ///      __تعداد__/__عضو__/__حقول__ (avoids a print false-positive — Amelia ع-٣).
            inline constexpr const char *ALGEBRAIC = "__جبري__";

            // ════════════════════════════════════════════════════════════════════
            // (AR) تنسيق طبع القيمة الموسومة — «تعداد.عضو(حقل، حقل)». مصدرٌ وحيد (SoT)
            //      يستهلكه المفسّر (value.cpp/io_functions.cpp) والمترجم (call_builder)
            //      كي يُنتج المحرّكان **تمثيلًا متطابقًا حرفًا بحرف** (بوّابة التطابق).
            // (EN) Tagged-value print format — «Enum.Variant(f, f)». Single source of truth
            //      consumed by the interpreter (value.cpp/io_functions.cpp) and the compiler
            //      (call_builder) so both engines emit a **byte-identical** representation
            //      (the parity gate).
            // ════════════════════════════════════════════════════════════════════

            /// (AR) الفاصل بين التعداد والعضو / (EN) Separator between enum and variant
            inline constexpr const char *DISPLAY_DOT = ".";

            /// (AR) فاتحة قائمة الحقول / (EN) Field-list opener
            inline constexpr const char *DISPLAY_OPEN = "(";

            /// (AR) الفاصل بين الحقول / (EN) Separator between fields
            inline constexpr const char *DISPLAY_SEP = ", ";

            /// (AR) خاتمة قائمة الحقول / (EN) Field-list closer
            inline constexpr const char *DISPLAY_CLOSE = ")";
        } // namespace TaggedEnumKeys
    } // namespace AST
} // namespace Sad
