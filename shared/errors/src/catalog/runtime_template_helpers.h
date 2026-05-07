/**
 * @file runtime_template_helpers.h
 * @brief (AR) دوال مساعدة لتسجيل قوالب أخطاء وقت التشغيل (Phase 4).
 *             كل ملف فئة دلالية (arithmetic, oop, ...) يضمّن هذا الهيدر
 *             ويستخدم makeRT للإنشاء، ثم يُسجّل عبر ErrorCatalog::registerTemplate.
 *
 * @brief (EN) Helpers for registering Phase 4 runtime error templates.
 *             Each semantic-category file (arithmetic, oop, ...) includes
 *             this header, uses makeRT to construct, and registers through
 *             ErrorCatalog::registerTemplate.
 *
 * (AR) القاعدة الصارمة: المفسر لا يُمرر أي نص. القوالب هنا هي المصدر
 *      الوحيد لكل رسالة، بأربعة مستويات (brief/normal/detailed/teacher)
 *      وبثلاث لغات (ar/en/both) عبر ErrorCatalog::format.
 */

#pragma once

#include "error_catalog.h"
#include <string>
#include <utility>

namespace Sad
{
    namespace Errors
    {
        namespace Catalog
        {

            /**
             * @brief (AR) ينشئ ErrorTemplate لرسالة وقت تشغيل
             * @brief (EN) Constructs an ErrorTemplate for a runtime message
             */
            inline ErrorTemplate makeRT(std::string id,
                                        std::string titleAr, std::string titleEn,
                                        std::string briefAr, std::string briefEn,
                                        std::string fixAr = "", std::string fixEn = "",
                                        std::string detailedAr = "", std::string detailedEn = "",
                                        std::string codeExample = "")
            {
                ErrorTemplate t;
                t.id = std::move(id);
                t.titleAr = std::move(titleAr);
                t.titleEn = std::move(titleEn);
                t.briefAr = std::move(briefAr);
                t.briefEn = std::move(briefEn);
                t.fixHintAr = std::move(fixAr);
                t.fixHintEn = std::move(fixEn);
                t.detailedAr = std::move(detailedAr);
                t.detailedEn = std::move(detailedEn);
                t.codeExample = std::move(codeExample);
                return t;
            }

        } // namespace Catalog

        // ────────────────────────────────────────────────────────────
        // (AR) إعلانات دوال التسجيل لكل فئة دلالية
        // (EN) Registration function declarations for each semantic category
        // ────────────────────────────────────────────────────────────
        void registerArithmeticRuntimeTemplates(ErrorCatalog &catalog);
        void registerCollectionsRuntimeTemplates(ErrorCatalog &catalog);
        void registerOopRuntimeTemplates(ErrorCatalog &catalog);
        void registerFunctionsRuntimeTemplates(ErrorCatalog &catalog);
        void registerTypesRuntimeTemplates(ErrorCatalog &catalog);
        void registerBuiltinsRuntimeTemplates(ErrorCatalog &catalog);
        void registerContractsRuntimeTemplates(ErrorCatalog &catalog);
        void registerConcurrencyRuntimeTemplates(ErrorCatalog &catalog);
        void registerMiscRuntimeTemplates(ErrorCatalog &catalog);

    } // namespace Errors
} // namespace Sad
