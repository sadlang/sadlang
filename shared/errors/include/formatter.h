/**
 * @file formatter.h
 * @brief (AR) واجهة موحَّدة لطباعة التشخيصات (caret + colors + bilingual)
 *        (EN) Unified facade for printing diagnostics (caret + colors + bilingual)
 *
 * (AR) المرحلة 2 من خطة توحيد رسائل الأخطاء.
 *      Diagnostic::format يحوي بالفعل منطق caret + ANSI + fix-its + notes؛
 *      Formatter هنا واجهة رقيقة (thin facade) توحّد:
 *        1) كيفية الاستهلاك من sad و sadc (نفس الإعدادات).
 *        2) كيفية تجميع الإخراج إلى ostream قابل للتوجيه (cerr/file/stringstream للاختبار).
 *        3) المُلخّص الختامي (عدد الأخطاء/التحذيرات).
 *
 * (EN) Phase 2 of error-message unification plan.
 *      Diagnostic::format already contains caret + ANSI + fix-its + notes logic;
 *      Formatter here is a thin facade that unifies:
 *        1) How sad and sadc consume it (same settings).
 *        2) Routing output to any ostream (cerr/file/stringstream for tests).
 *        3) Final summary (error/warning counts).
 *
 * @see diagnostic.h
 * @see error_manager.h
 */

#pragma once

#include "diagnostic.h"
#include "error_codes.h"

#include <iosfwd>
#include <string>
#include <vector>

namespace Sad
{
    namespace Errors
    {

        /**
         * @struct FormatterOptions
         * @brief (AR) خيارات تنسيق التشخيصات / (EN) Diagnostic formatting options
         */
        struct FormatterOptions
        {
            Language language = Language::BOTH; ///< (AR) اللغة / (EN) Language
            bool colorize = true;               ///< (AR) ألوان ANSI / (EN) ANSI colors
            bool showSource = true;             ///< (AR) عرض سطر المصدر / (EN) Show source line
            bool showSummary = true;            ///< (AR) ملخّص ختامي / (EN) Final summary
            std::string sourceCode;             ///< (AR) كود المصدر للسطر المعروض / (EN) Source code for shown line
        };

        /**
         * @class Formatter
         * @brief (AR) واجهة طباعة موحَّدة (دوال ساكنة)
         *        (EN) Unified printing facade (static methods)
         *
         * (AR) لا تحتفظ بحالة — كلها static. ErrorManager::flush ينادِيها.
         * (EN) Stateless — all static. Called by ErrorManager::flush.
         */
        class Formatter
        {
        public:
            /**
             * @brief (AR) يطبع تشخيصاً واحداً
             *        (EN) Prints a single diagnostic
             */
            static void printDiagnostic(std::ostream &os,
                                        const Diagnostic &diag,
                                        const FormatterOptions &opts);

            /**
             * @brief (AR) يطبع قائمة تشخيصات + ملخّصاً اختيارياً
             *        (EN) Prints a list of diagnostics + optional summary
             */
            static void printAll(std::ostream &os,
                                 const std::vector<Diagnostic> &diags,
                                 const FormatterOptions &opts);

            /**
             * @brief (AR) يطبع ملخّصاً ختامياً (س أخطاء، ص تحذيرات)
             *        (EN) Prints final summary (X errors, Y warnings)
             */
            static void printSummary(std::ostream &os,
                                     size_t errorCount,
                                     size_t warningCount,
                                     const FormatterOptions &opts);
        };

    } // namespace Errors
} // namespace Sad
