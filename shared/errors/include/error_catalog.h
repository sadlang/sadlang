/**
 * @file error_catalog.h
 * @brief (AR) سجل قوالب الأخطاء المركزي للغة "ص"
 *        (EN) Central error template catalog for Sad language
 *
 * (AR) يوفر هذا الملف ErrorCatalog: قاعدة بيانات قوالب الأخطاء التي
 *      تُستخدم لتوليد رسائل خطأ متسقة ومتعددة المستويات (موجز/عادي/مفصل/معلم)
 *      ومزدوجة اللغة (عربي/إنجليزي). يستهلكه ErrorManager.throwRuntime
 *      و reportError* بدل بناء النصوص يدوياً في كل موقع رمي.
 *
 * (EN) Provides ErrorCatalog: a database of error templates used to generate
 *      consistent multi-level (brief/normal/detailed/teacher) bilingual
 *      (AR/EN) error messages. Consumed by ErrorManager.throwRuntime and
 *      reportError* instead of hand-building strings at every throw site.
 *
 * @see error_manager.h
 * @see explanation_level.h
 */

#pragma once

#include "error_codes.h"
#include "explanation_level.h"
#include "source_location.h"

#include <map>
#include <optional>
#include <string>
#include <unordered_map>

namespace Sad
{
    namespace Errors
    {

        /**
         * @struct RenderContext
         * @brief (AR) سياق عرض الخطأ — يحوي placeholders ومعلومات الموقع الإضافية
         *        (EN) Error rendering context — contains placeholders and extra location info
         *
         * (AR) يُمرَّر إلى ErrorCatalog::render لتعبئة الفجوات في القوالب.
         *      مثال: قالب "متغير '{name}' غير معرَّف" + placeholders["name"]="س"
         *      ينتج "متغير 'س' غير معرَّف".
         *
         * (EN) Passed to ErrorCatalog::render to fill template placeholders.
         *      Example: template "Variable '{name}' is undefined" + placeholders["name"]="x"
         *      yields "Variable 'x' is undefined".
         */
        struct RenderContext
        {
            SourceLocation location;                         ///< (AR) موقع الخطأ / (EN) Error location
            std::map<std::string, std::string> placeholders; ///< (AR) قيم {key} / (EN) {key} values
            std::optional<std::string> exprText;             ///< (AR) نص التعبير المُتسبِّب (اختياري) / (EN) Triggering expression text (optional)

            RenderContext() = default;
            explicit RenderContext(SourceLocation loc) : location(std::move(loc)) {}
            RenderContext(SourceLocation loc,
                          std::map<std::string, std::string> ph)
                : location(std::move(loc)), placeholders(std::move(ph)) {}
        };

        /**
         * @struct ErrorTemplate
         * @brief (AR) قالب رسالة خطأ واحد (ar/en × مستويات الشرح)
         *        (EN) One error message template (ar/en × explanation levels)
         *
         * (AR) كل حقل اختياري — إن كان فارغاً يُستخدم الحقل ذو المستوى الأدنى كبديل.
         *      title* للعنوان القصير، brief/detailed/teacher* للنص حسب المستوى،
         *      fixHint* لاقتراح إصلاح، codeExample مثال كود (يُعرض في DETAILED+).
         *
         * (EN) Every field optional — empty falls back to lower level.
         *      title* = short headline; brief/detailed/teacher* = body per level;
         *      fixHint* = fix suggestion; codeExample shown at DETAILED+.
         */
        struct ErrorTemplate
        {
            std::string id;      ///< (AR) معرّف نصي مثل "RUN001" / (EN) String id
            std::string titleAr; ///< (AR) العنوان بالعربية / (EN) Arabic title
            std::string titleEn; ///< (AR) العنوان بالإنجليزية / (EN) English title

            std::string briefAr;    ///< BRIEF — Arabic
            std::string briefEn;    ///< BRIEF — English
            std::string detailedAr; ///< DETAILED — Arabic (شرح موسع)
            std::string detailedEn; ///< DETAILED — English
            std::string teacherAr;  ///< TEACHER — Arabic (تشبيهات + خطوات)
            std::string teacherEn;  ///< TEACHER — English

            std::string fixHintAr;   ///< (AR) اقتراح إصلاح / (EN) Arabic fix hint
            std::string fixHintEn;   ///< English fix hint
            std::string codeExample; ///< (AR) مثال كود (لغة ص) / (EN) Code example (Sad source)
        };

        /**
         * @class ErrorCatalog
         * @brief (AR) سجل القوالب المركزي (Singleton)
         *        (EN) Central template registry (Singleton)
         *
         * (AR) يخزّن قوالب الأخطاء ويوفّر دالة render لاختيار النص المناسب حسب
         *      المستوى واللغة وتعبئة الـplaceholders. registerDefaults يسجّل
         *      مجموعة افتراضية شاملة (تُستدعى من ErrorManager.initializeDefaults).
         *
         * (EN) Stores error templates and provides render() to pick the right text
         *      per level/language and fill placeholders. registerDefaults() seeds a
         *      comprehensive default set (called from ErrorManager.initializeDefaults).
         */
        class ErrorCatalog
        {
        public:
            /**
             * @struct Rendered
             * @brief (AR) نتيجة العرض — نصوص ar/en + ملاحظة تعليمية اختيارية
             *        (EN) Render result — ar/en texts + optional teaching note
             */
            struct Rendered
            {
                std::string messageAr;                  ///< (AR) النص العربي / (EN) Arabic text
                std::string messageEn;                  ///< (AR) النص الإنجليزي / (EN) English text
                std::optional<std::string> fixHintAr;   ///< (AR) اقتراح إصلاح ar (إن وُجد) / (EN) AR fix hint
                std::optional<std::string> fixHintEn;   ///< (AR) اقتراح إصلاح en (إن وُجد) / (EN) EN fix hint
                std::optional<std::string> codeExample; ///< (AR) مثال كود (إن وُجد ومستوى ≥ DETAILED) / (EN) Code example
            };

            /**
             * @brief (AR) يرجع النسخة الوحيدة (Singleton)
             *        (EN) Returns singleton instance
             */
            static ErrorCatalog &instance();

            /**
             * @brief (AR) يسجّل قالباً جديداً (يستبدل أي قالب سابق بنفس الكود)
             *        (EN) Registers a new template (replaces any prior template with same code)
             */
            void registerTemplate(ErrorCode code, ErrorTemplate tmpl);

            /**
             * @brief (AR) يجلب قالباً مسجَّلاً (nullptr إن لم يوجد)
             *        (EN) Fetches a registered template (nullptr if absent)
             */
            const ErrorTemplate *getTemplate(ErrorCode code) const;

            /**
             * @brief (AR) يبني الرسالة من القالب حسب المستوى واللغة وسياق العرض
             *        (EN) Renders message from template per level/language/context
             *
             * (AR) إذا لم يوجد قالب للكود، تُنتَج رسالة عامة
             *      "خطأ غير موصوف (CODE)" حتى لا ينهار النظام.
             * (EN) If no template registered for code, a generic
             *      "Unspecified error (CODE)" message is produced so the system
             *      never collapses on missing templates.
             *
             * @param code (AR) رمز الخطأ / (EN) Error code
             * @param level (AR) مستوى الشرح / (EN) Explanation level
             * @param lang (AR) لغة الإخراج (تُحدّد أي حقل يُملأ) / (EN) Output language
             * @param ctx (AR) سياق + placeholders / (EN) Context + placeholders
             */
            Rendered render(ErrorCode code,
                            ExplanationLevel level,
                            Language lang,
                            const RenderContext &ctx) const;

            /**
             * @brief (AR) يُسجّل القوالب الافتراضية الأساسية (~50 قالباً)
             *        (EN) Registers default core templates (~50 templates)
             *
             * (AR) يُستدعى مرة واحدة من ErrorManager.initializeDefaults
             *      عند بدء تشغيل sad أو sadc.
             * (EN) Called once from ErrorManager.initializeDefaults at startup
             *      of sad or sadc.
             */
            void registerDefaults();

            /**
             * @brief (AR) يمسح كل القوالب (للاختبارات أساساً)
             *        (EN) Clears all templates (mainly for tests)
             */
            void clear();

            /**
             * @brief (AR) عدد القوالب المسجَّلة
             *        (EN) Number of registered templates
             */
            size_t size() const { return templates_.size(); }

        private:
            ErrorCatalog() = default;
            ErrorCatalog(const ErrorCatalog &) = delete;
            ErrorCatalog &operator=(const ErrorCatalog &) = delete;

            // (AR) استبدال {key} بالقيم من ctx.placeholders
            // (EN) Substitute {key} placeholders from ctx.placeholders
            static std::string substitute(const std::string &tmpl,
                                          const RenderContext &ctx);

            // (AR) يختار حقل النص المناسب حسب المستوى مع fallback تنازلي
            // (EN) Picks the right text field per level with downward fallback
            static const std::string &pickAr(const ErrorTemplate &t, ExplanationLevel lvl);
            static const std::string &pickEn(const ErrorTemplate &t, ExplanationLevel lvl);

            std::unordered_map<int, ErrorTemplate> templates_; ///< key = static_cast<int>(ErrorCode)
        };

    } // namespace Errors
} // namespace Sad
