#pragma once
/**
 * @file sad_debug_log.h
 * @brief (AR) مرفقُ تسجيلِ التنقيحِ الموحَّدُ لمحرّكَي «ص» — قناةٌ واحدةٌ مُطفأةٌ افتراضيًّا.
 * @brief (EN) Unified debug-log facility for both Sad engines — one channel, off by default.
 *
 * (AR) 🔑 لماذا وُجِدَ هذا الملفّ: كانت ٣٩٢ نقطةَ تنقيحٍ في المترجّمِ والمفسّرِ تطبعُ
 *      مباشرةً على `std::cout` بـ`std::endl`، كلٌّ منها محروسةٌ بـ`#ifndef NDEBUG`
 *      وحدَه. فترتّبَ على ذلك ثلاثةُ أعطابٍ مقيسة:
 *
 *      ① **القناةُ خطأ.** `std::cout` هو مَجرى نتيجةِ الأداةِ لا مَجرى تشخيصِها،
 *         فآلافُ أسطرِ الضوضاءِ تسيلُ في الخرجِ القياسيِّ للمترجّم. ولم يكسرْ ذلك
 *         شيئًا حتّى الآنَ لأنّ عدّاءَ الاختباراتِ يقارنُ خرجَ *البرنامجِ* لا خرجَ
 *         *المترجّم* — أي أنّها **حادثةٌ لم تقعْ بعد** لا تصميمٌ سليم.
 *
 *      ② **`std::endl` يُفرِغُ المَجرى عندَ كلِّ سطر.** والإفراغُ نداءُ نظامٍ، فثمنُه
 *         يُضرَبُ في عددِ الأسطرِ لا في حجمِها.
 *
 *      ③ **الحراسةُ زمنَ الترجمةِ وحدَها لا تكفي.** بناءُ التنقيحِ هو البناءُ الذي
 *         يعملُ عليه المطوّرُ ويقيسُ به، فـ«مُطفأٌ في الإصدار» يعني عمليًّا
 *         «مُشتعِلٌ دائمًا حيثُ يهمّ». وقِيسَ الأثر: بذرةٌ من ٥٩ سطرًا تُترجَمُ في
 *         ١٦٫٦ ثانيةً ببناءِ التنقيحِ مقابلَ ٠٫٨٠ ثانيةً ببناءِ الإصدار — عشرونَ
 *         ضعفًا، وأغلبُها موضعٌ واحدٌ يطبعُ كلَّ الكتلِ عندَ كلِّ فرع.
 *
 *      فصارَ لكلِّ نقطةٍ **مِفتاحانِ**: `NDEBUG` زمنَ الترجمةِ كما كان، ومتغيّرُ
 *      البيئةِ `SAD_DEBUG_LOG` زمنَ التشغيلِ وهو **مُطفأٌ ما لم يُضبَطْ**. والتشخيصُ
 *      لم يُحذَفْ — بقيَ كلُّه ويُشتعَلُ بأمرٍ صريح، لأنّ حذفَ أداةِ تشخيصٍ أرخصُ
 *      اليومَ وأغلى يومَ تُحتاج.
 *
 * (AR) واسمُ المتغيّرِ لاتينيٌّ عمدًا: أسماءُ متغيّراتِ البيئةِ العربيّةُ تنكسرُ عبرَ
 *      الصَّدَفات، وهذا الاسمُ يُكتَبُ في سطرِ أمرٍ لا في شيفرةِ «ص».
 *
 * (EN) 🔑 392 debug points across both engines printed straight to `std::cout` with
 *      `std::endl`, guarded only by `#ifndef NDEBUG`. Three measured defects: wrong
 *      channel (stdout is the tool's result stream, not its diagnostic stream); a
 *      flush per line; and compile-time-only gating, which means "on always, exactly
 *      where it hurts" since Debug is the build developers measure with. Measured
 *      cost: a 59-line seed compiled in 16.6s under Debug vs 0.80s under Release.
 *      Now every point has two switches: `NDEBUG` at compile time and the
 *      `SAD_DEBUG_LOG` environment variable at run time, off unless set. Nothing was
 *      deleted — diagnostics are cheap to keep and expensive to miss.
 */

#include <cstdlib>
#include <iostream>
#include <ostream>

namespace Sad
{
    namespace Diagnostics
    {
        /**
         * @brief (AR) هل تسجيلُ التنقيحِ مُشتعِلٌ في هذا التشغيل؟
         * @brief (EN) Is debug logging enabled for this run?
         *
         * (AR) يُقرَأُ متغيّرُ البيئةِ مرّةً واحدةً ويُخزَّنُ، فثمنُ السؤالِ عندَ كلِّ
         *      نقطةِ تنقيحٍ مقارنةُ منطقيٍّ لا نداءُ نظام. وأيُّ قيمةٍ غيرِ فارغةٍ
         *      تُشعِله عدا "0" — كي لا يُفاجَأَ مَن كتبَ `SAD_DEBUG_LOG=0` ظانًّا
         *      أنّه أطفأه.
         * (EN) The environment is read once and cached, so each call site costs a
         *      boolean test, not a syscall. Any non-empty value enables it except
         *      "0", so `SAD_DEBUG_LOG=0` does what its author meant.
         */
        inline bool debugLogEnabled() noexcept
        {
            static const bool enabled = []() noexcept
            {
                const char *value = std::getenv("SAD_DEBUG_LOG");
                if (value == nullptr || value[0] == '\0')
                {
                    return false;
                }
                return !(value[0] == '0' && value[1] == '\0');
            }();
            return enabled;
        }

        /**
         * @brief (AR) مَجرى التشخيصِ — `std::cerr` لا `std::cout`.
         * @brief (EN) The diagnostic stream — `std::cerr`, never `std::cout`.
         */
        inline std::ostream &debugLogStream() noexcept
        {
            return std::cerr;
        }
    } // namespace Diagnostics
} // namespace Sad

/**
 * @def SAD_DEBUG_LOG_LINE
 * @brief (AR) يطبعُ سطرَ تنقيحٍ واحدًا إن كانَ المِفتاحانِ مرفوعَين.
 * @brief (EN) Emits one debug line when both switches are up.
 *
 * (AR) الاستعمال: `SAD_DEBUG_LOG_LINE("[DEBUG] اسم: " << قيمة);` — أي ما كانَ
 *      يُكتَبُ بعدَ `std::cout <<` حرفًا بحرف، بلا `std::endl` فمحرفُ السطرِ
 *      مُضافٌ هنا بلا إفراغ. وتُغلَّفُ بـ`do/while(0)` كي تُسلِمَ نفسَها في
 *      `if` بلا قوسَين دونَ أن تبتلعَ `else`.
 * (EN) Usage: `SAD_DEBUG_LOG_LINE("[DEBUG] name: " << value);` — exactly what
 *      followed `std::cout <<`, minus `std::endl`: the newline is appended here
 *      without a flush. Wrapped in do/while(0) so it behaves as one statement.
 */
#ifndef NDEBUG
#define SAD_DEBUG_LOG_LINE(streamExpression)                                    \
    do                                                                          \
    {                                                                           \
        if (::Sad::Diagnostics::debugLogEnabled())                              \
        {                                                                       \
            ::Sad::Diagnostics::debugLogStream() << streamExpression << '\n';   \
        }                                                                       \
    } while (false)
#else
/**
 * (AR) في بناءِ الإصدارِ تُمحى المقاديرُ تمامًا — لا تقييمَ ولا نصَّ في الثنائيّ.
 * (EN) In release builds the expression is not evaluated and no text is emitted.
 */
#define SAD_DEBUG_LOG_LINE(streamExpression) \
    do                                       \
    {                                        \
    } while (false)
#endif
