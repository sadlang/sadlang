/**
 * @file error_hints.h
 * @brief (AR) نظام نصائح الأخطاء للمبتدئين — يوفر شروحات وإصلاحات مقترحة لكل خطأ شائع
 *        (EN) Beginner-friendly error hints system — provides explanations and suggested fixes
 *
 * (AR) هذا النظام مصمم خصيصاً لشخصية "أحمد" — طالب سنة أولى يتعلم البرمجة بالعربي.
 *      كل رسالة خطأ يجب أن:
 *      1. تشرح المشكلة بلغة بسيطة
 *      2. تعطي مثالاً على الكود الخاطئ
 *      3. تعطي مثالاً على الكود الصحيح
 *      4. توجّه المبتدئ للحل
 *
 * (EN) This system is designed for "Ahmed" persona — a first-year student learning
 *      programming in Arabic. Each error message should:
 *      1. Explain the problem in simple language
 *      2. Show an example of the wrong code
 *      3. Show an example of the correct code
 *      4. Guide the beginner to the fix
 *
 * @author فريق لغة ص
 * @date يوليو 2025
 * @version 1.0
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace Sad
{
    namespace Errors
    {

        /**
         * @struct ErrorHint
         * @brief (AR) نصيحة مرتبطة بخطأ معين — تشمل شرح بسيط وأمثلة كود
         *        (EN) Hint linked to a specific error — includes simple explanation and code examples
         */
        struct ErrorHint
        {
            std::string explanation;       ///< (AR) شرح بسيط للمشكلة بالعربية
            std::string wrongExample;      ///< (AR) مثال على الكود الخاطئ
            std::string correctExample;    ///< (AR) مثال على الكود الصحيح
            std::vector<std::string> tips; ///< (AR) نصائح إضافية
        };

        /**
         * @class ErrorHintEngine
         * @brief (AR) محرك نصائح الأخطاء — يحلل رسالة الخطأ ويعطي نصائح مناسبة
         *        (EN) Error hint engine — analyzes error message and provides appropriate hints
         *
         * (AR) يعمل بتحليل نص رسالة الخطأ ونوع الاستثناء للبحث عن أقرب نصيحة.
         *      لا يحتاج لتغيير بنية الاستثناءات الحالية — يعمل كطبقة عرض فقط.
         */
        class ErrorHintEngine
        {
        public:
            /**
             * @brief (AR) الحصول على النسخة الوحيدة
             *        (EN) Get singleton instance
             */
            static ErrorHintEngine &getInstance();

            /**
             * @brief (AR) البحث عن نصيحة لخطأ معين
             *        (EN) Find hint for a specific error
             *
             * @param errorType نوع الاستثناء (مثل "RuntimeError", "DivisionByZeroError")
             * @param errorMessage رسالة الخطأ الكاملة
             * @return نصيحة إذا وُجدت، أو nullptr
             */
            const ErrorHint *findHint(const std::string &errorType,
                                      const std::string &errorMessage) const;

            /**
             * @brief (AR) تنسيق النصيحة كنص للعرض في الطرفية
             *        (EN) Format hint as text for terminal display
             *
             * @param hint النصيحة المراد تنسيقها
             * @param colorize هل يُستخدم تلوين ANSI؟
             * @return النص المنسق
             */
            static std::string formatHint(const ErrorHint &hint, bool colorize = true);

        private:
            ErrorHintEngine();
            ErrorHintEngine(const ErrorHintEngine &) = delete;
            ErrorHintEngine &operator=(const ErrorHintEngine &) = delete;

            /// (AR) تهيئة قاعدة النصائح
            void initializeHints();

            /// (AR) نصائح مرتبطة بنوع الاستثناء مباشرة
            std::unordered_map<std::string, ErrorHint> typeHints_;

            /// (AR) نصائح مرتبطة بكلمات مفتاحية في رسالة الخطأ
            struct KeywordHint
            {
                std::string keyword; ///< كلمة مفتاحية للبحث عنها في الرسالة
                ErrorHint hint;
            };
            std::vector<KeywordHint> keywordHints_;
        };

    } // namespace Errors
} // namespace Sad
