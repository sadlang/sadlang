/*
 * ============================================================================
 * (AR) منافذُ النصِّ التي تهبطُ نداءً إلى زمنِ التشغيل — رمزٌ واحدٌ لاسمٍ واحد
 * (EN) String ports that lower to a runtime call — one symbol, one name
 * ============================================================================
 *
 * (AR) لماذا هذا الملفُّ موجود (مقيسٌ لا مُخمَّن):
 *
 *      ثلاثُ طرائقَ نصّيّةٍ لا تهبطُ إلى أوپكودٍ في الخلفيّة، بل إلى **نداءِ**
 *      دالّةٍ في زمنِ التشغيل: «عكس» و«كرر» و«حرف_عند». وبابُ مؤشّرِ النصّ
 *      (`emitStringPtrOrRaise`) يحرسُ بواعثَ الأوپكودات، فلا يمرُّ به النداءُ —
 *      فكان «س.عكس()» على خانةٍ عدميّةٍ **ينهارُ انهيارَ تجزئة** بينما يرفعُ
 *      المفسّرُ RUN033. كشفه حارسُ تكافؤِ المستقبِلِ العدميّ من أوّلِ تشغيلٍ له.
 *
 * (AR) 🔑 والاسمُ يُعرَفُ في الأمامِ ويُنسى في الخلف: الأمامُ يبني النداءَ وهو
 *      يعرفُ «عكس»، ثمّ لا يبقى في SIR إلّا رمزُ زمنِ التشغيل. والبابُ يحتاجُ
 *      الاسمَ ليملأ فراغَ RUN033. فوُضِعت الترجمةُ هنا مرّةً واحدةً يقرأها
 *      الطرفانِ معًا — لا جدولانِ ينحرفان.
 *
 * (AR) والأسماءُ العربيّةُ ليست مكتوبةً هنا، بل مأخوذةٌ من سجلِّ المدمجاتِ
 *      المولَّدِ من مصدرِ الحقيقة: تغييرُ اللفظِ في المصدرِ يسري هنا من تلقائه.
 *
 * (EN) Three string methods lower to a runtime CALL rather than a backend opcode
 *      (reverse, repeat, char_at), so they bypass the string-pointer door and a null
 *      receiver segfaulted where the interpreter raises RUN033. The Arabic name is
 *      known in the frontend and lost in SIR, while the door needs it to fill the
 *      RUN033 placeholder — hence one shared translation, read by both sides. The
 *      Arabic names come from the generated SoT registry, never spelled here.
 * ============================================================================
 */
#ifndef SAD_FRONTEND_STRING_RUNTIME_PORTS_H
#define SAD_FRONTEND_STRING_RUNTIME_PORTS_H

#include <string_view>

#include "builtin_registry.h"

namespace Sad
{
    namespace Compiler
    {
        namespace StringRuntimePorts
        {
            // (AR) رموزُ زمنِ التشغيل — معرَّفةٌ هنا وحدَها، ويقرؤها الأمامُ والخلف.
            // (EN) Runtime symbols — defined here only; both frontend and backend read them.
            inline constexpr std::string_view kUtf8CharAt = "sad_llvm_string_utf8_char_at";
            inline constexpr std::string_view kReverse = "sad_llvm_string_reverse";
            inline constexpr std::string_view kRepeat = "sad_llvm_string_repeat";

            /**
             * (AR) اسمُ الطريقةِ العربيُّ لرمزِ زمنِ تشغيلٍ يكونُ **معاملُه الأوّلُ
             *      مستقبِلًا نصّيًّا**. الرجوعُ بفراغٍ يعني «ليس منفذَ مستقبِلٍ نصّيّ»،
             *      فلا يُلمَسُ النداءُ بحال.
             * (EN) The Arabic method name for a runtime symbol whose FIRST operand is a
             *      string receiver. An empty result means "not a string-receiver port".
             */
            inline constexpr std::string_view receiverMethodName(std::string_view symbol)
            {
                namespace SoT = Sad::Builtins::Names::TypeMethods::String;
                if (symbol == kUtf8CharAt)
                    return SoT::CHAR_AT;
                if (symbol == kReverse)
                    return SoT::REVERSE;
                if (symbol == kRepeat)
                    return SoT::REPEAT;
                return {};
            }

        } // namespace StringRuntimePorts
    } // namespace Compiler
} // namespace Sad

#endif // SAD_FRONTEND_STRING_RUNTIME_PORTS_H
