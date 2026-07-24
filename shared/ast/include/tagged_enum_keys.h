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
        } // namespace TaggedEnumKeys
    } // namespace AST
} // namespace Sad
