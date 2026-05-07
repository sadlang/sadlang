/**
 * @file explanation_level.h
 * @brief (AR) مستوى الشرح + لغة الإخراج لرسائل الأخطاء
 *        (EN) Explanation level + output language for error messages
 *
 * (AR) تم استخراج هذا التعداد من teacher_mode.h ليُستخدم بشكل مستقل
 *      عن نظام TeacherMode الكامل (لتجنّب التضمينات الثقيلة).
 *      teacher_mode.h يستورد هذا الملف للحفاظ على التوافق الخلفي.
 *
 * (EN) Extracted from teacher_mode.h to be used independently of the
 *      full TeacherMode system (to avoid heavy includes).
 *      teacher_mode.h re-exports this header for backward compatibility.
 */

#pragma once

#include "error_codes.h" // Language enum
#include <string>

namespace Sad
{
    namespace Errors
    {

        /**
         * @enum ExplanationLevel
         * @brief (AR) مستوى تفصيل شرح الخطأ
         *        (EN) Detail level for error explanation
         */
        enum class ExplanationLevel
        {
            BRIEF,    ///< (AR) موجز - للخبراء (سطر واحد) / (EN) Brief - one line for experts
            NORMAL,   ///< (AR) عادي - رسالة + اقتراحات / (EN) Normal - message + hints
            DETAILED, ///< (AR) مفصل - رسالة + شرح + أمثلة / (EN) Detailed - message + explanation + examples
            TEACHER   ///< (AR) معلم - كل شيء + تشبيهات / (EN) Teacher - everything + analogies
        };

        /**
         * @brief (AR) يُحوّل نص العَلَم --explain إلى ExplanationLevel
         *        (EN) Parses --explain flag string into ExplanationLevel
         *
         * @param s (AR) النص (brief/normal/detailed/teacher) / (EN) String
         * @return (AR) المستوى (NORMAL إذا غير معروف) / (EN) Level (NORMAL if unknown)
         */
        ExplanationLevel parseExplanationLevel(const std::string &s);

        /**
         * @brief (AR) يُحوّل ExplanationLevel إلى نص
         *        (EN) Converts ExplanationLevel to string
         */
        std::string toString(ExplanationLevel level);

        /**
         * @brief (AR) يُحوّل نص العَلَم --lang إلى Language
         *        (EN) Parses --lang flag string into Language
         *
         * @param s (AR) ar/en/both / (EN) ar/en/both
         * @return (AR) اللغة (BOTH إذا غير معروف) / (EN) Language (BOTH if unknown)
         */
        Language parseLanguage(const std::string &s);

        /**
         * @brief (AR) يُحوّل Language إلى نص
         *        (EN) Converts Language to string
         */
        std::string toString(Language lang);

    } // namespace Errors
} // namespace Sad
