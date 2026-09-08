// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: shared/types/include/builtin_enum_names.h
// الوصف: أسماءُ التعدادَينِ المدمَجَين — «اختياري» و«نتيجة» — بلغتَيهما.
//
// (AR) 🔑 **وكان هذا السؤالُ يُجابُ بسجلٍّ كاملٍ اسمُه `EnumRegistry`** في
//      `types/enum_types.h` (٤٢٩ سطرًا + ٤٠٨ تنفيذًا) يجرُّ معه `EnumType`
//      و`EnumVariant` و`EnumBuilder` و`GenericParameter` و`StructType`.
//      وقِيسَ أنّ للسجلِّ **مُستهلِكًا واحدًا** في المستودعِ كلِّه هو
//      `EnumRegistry::instance().isEnumType(name)` في `visitEnumDecl`، وأنّ
//      **مُعبِّئَه الوحيدَ** هو مُنشِئُه نفسُه (`initializeCoreEnums`) —
//      فمجموعةُ إجاباتِه أربعةُ أسماءٍ ثابتةٍ لا تزيد. أمرا القياس:
//        grep -rn "EnumRegistry::instance" shared compiler apps tools tests
//        grep -rn "registerEnum"           shared compiler apps tools tests
//      فصارَ الجوابُ أربعةَ أسماءٍ مُعلَنةً، وحُذِفَ الهرمُ الذي كان يحملُها.
// (EN) This question used to be answered by a whole EnumRegistry whose only
//      populator was its own constructor (Option/Result) and whose only
//      consumer was one isEnumType() call. Four names replace 837 lines.
// ═══════════════════════════════════════════════════════════════════════════════

#ifndef SAD_TYPES_BUILTIN_ENUM_NAMES_H
#define SAD_TYPES_BUILTIN_ENUM_NAMES_H

#include <string>

namespace Sad
{
    namespace Types
    {
        // (AR) «اختياري» بترميز UTF-8 — ولا سلسلةَ عربيّةً خامًّا في الشفرة.
        // (EN) "Option" in Arabic, UTF-8 escaped: no raw Arabic string literals.
        inline const char *builtinOptionEnumArabicName()
        {
            return "\xd8\xa7\xd8\xae\xd8\xaa\xd9\x8a\xd8\xa7\xd8\xb1\xd9\x8a";
        }

        // (AR) «نتيجة» بترميز UTF-8.
        // (EN) "Result" in Arabic, UTF-8 escaped.
        inline const char *builtinResultEnumArabicName()
        {
            return "\xd9\x86\xd8\xaa\xd9\x8a\xd8\xac\xd8\xa9";
        }

        /**
         * @brief (AR) هل الاسمُ اسمَ تعدادٍ مدمَجٍ (بأيِّ لغتَيه)؟
         * @brief (EN) Is the name one of the built-in enums (either language)?
         */
        inline bool isBuiltinEnumName(const std::string &name)
        {
            return name == builtinOptionEnumArabicName() || name == "Option" ||
                   name == builtinResultEnumArabicName() || name == "Result";
        }

    } // namespace Types
} // namespace Sad

#endif // SAD_TYPES_BUILTIN_ENUM_NAMES_H
