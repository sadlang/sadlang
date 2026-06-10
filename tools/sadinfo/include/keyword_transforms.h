// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: tools/sadinfo/include/keyword_transforms.h
// (AR) دوال تحويل بيانات الكلمات المفتاحية — دوال نقية قابلة للاختبار.
//      تحوّل KeywordEntry المولَّدة إلى ProcessedKeyword الغنية بمعرّف مستقر،
//      كشف deprecated، وبنيات الفلترة.
//
// (EN) Keyword data transformation functions — pure, testable.
//      Converts generated KeywordEntry into ProcessedKeyword with stable id,
//      deprecated detection, and filtering structures.
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include <string>
#include <vector>
#include "keywords_generated.h"

namespace Sad
{
    namespace Info
    {

        // ─────────────────────────────────────────────────────────────────────
        // (AR) كلمة مفتاحية مُعالَجة — تحمل البيانات المشتقة إضافةً إلى الأصلية
        // (EN) Processed keyword — carries derived data on top of source entry
        // ─────────────────────────────────────────────────────────────────────
        struct ProcessedKeyword
        {
            std::string                id;             ///< (AR) معرّف مستقر مشتق
            std::string                word;           ///< (AR) الكلمة العربية
            std::string                english;        ///< (AR) الاسم الإنجليزي
            std::string                category;       ///< (AR) "reserved"|"operator"|"contextual"|"builtin_type"
            int                        tokenType;      ///< (AR) قيمة TokenType
            bool                       emittedByLexer; ///< (AR) هل Lexer يُصدرها مباشرة؟
            bool                       deprecated;     ///< (AR) هل مُهمَلة؟
            std::vector<std::string>   aliases;        ///< (AR) أسماء بديلة
            std::vector<std::string>   roles;          ///< (AR) الأدوار الدلالية
        };

        // ─────────────────────────────────────────────────────────────────────
        // (AR) فلتر مُحلَّل — يمثّل تعبير فلتر واحد (مثال: category=reserved)
        // (EN) Parsed filter — represents a single filter expression
        // ─────────────────────────────────────────────────────────────────────
        struct ParsedFilter
        {
            std::string key;      ///< (AR) المفتاح (category, role, token-type, has-aliases, ...)
            std::string value;    ///< (AR) القيمة (فارغ إذا لم يكن فيه =)
            bool        hasValue; ///< (AR) هل يحوي =value
        };

        // ─────────────────────────────────────────────────────────────────────
        // (AR) نتيجة التجميع بالفئة
        // (EN) Result of grouping by category
        // ─────────────────────────────────────────────────────────────────────
        struct GroupedKeywords
        {
            std::vector<ProcessedKeyword> reserved;
            std::vector<ProcessedKeyword> op;
            std::vector<ProcessedKeyword> contextual;
            std::vector<ProcessedKeyword> builtinType;
        };

        // ═════════════════════════════════════════════════════════════════════
        // (AR) الدوال النقية / (EN) Pure functions
        // ═════════════════════════════════════════════════════════════════════

        /**
         * @brief (AR) هل الكلمة مُهمَلة (تنتهي بـ _deprecated بأي حالة حروف)؟
         * @brief (EN) Is the English name deprecated (_deprecated suffix, any case)?
         */
        bool isDeprecated(const std::string &english);

        /**
         * @brief (AR) اشتقاق معرّف مستقر من الفئة والاسم الإنجليزي وnوع الرمز.
         * @brief (EN) Derive stable id from category, english name, and token type.
         *
         * (AR) الصيغ:
         *   RESERVED    + LITERAL_TRUE/FALSE/NULL → "literal.<name>"
         *   RESERVED                               → "keyword.<english>"
         *   OPERATOR                               → "operator.<english>"
         *   CONTEXTUAL  + _deprecated suffix       → "contextual.<stripped>"
         *   CONTEXTUAL                             → "contextual.<english>"
         *   BUILTIN_TYPE                           → "type.<english>"
         */
        std::string deriveStableId(
            Sad::Lexer::Generated::KeywordCategory cat,
            const std::string                     &english,
            Sad::Lexer::TokenType                  tt
        );

        /**
         * @brief (AR) تحويل KeywordEntry → ProcessedKeyword.
         * @brief (EN) Convert KeywordEntry to ProcessedKeyword.
         */
        ProcessedKeyword processEntry(const Sad::Lexer::Generated::KeywordEntry &e);

        /**
         * @brief (AR) إرجاع جميع الكلمات المعالَجة من المعجم المولَّد.
         * @brief (EN) Return all processed keywords from the generated lexicon.
         */
        std::vector<ProcessedKeyword> getAllProcessed();

        /**
         * @brief (AR) تطبيق قائمة فلاتر (AND منطقي) مع خيار استبعاد deprecated.
         * @brief (EN) Apply filter list (logical AND) with optional deprecated exclusion.
         */
        std::vector<ProcessedKeyword> applyFilters(
            const std::vector<ProcessedKeyword> &all,
            const std::vector<ParsedFilter>     &filters,
            bool                                 excludeDeprecated
        );

        /**
         * @brief (AR) تجميع الكلمات حسب الفئة.
         * @brief (EN) Group keywords by category.
         */
        GroupedKeywords groupByCategory(const std::vector<ProcessedKeyword> &all);

        /**
         * @brief (AR) تحليل تعبير فلتر ("category=reserved", "has-aliases", ...).
         * @brief (EN) Parse a filter expression.
         * @param expr   التعبير
         * @param out    الفلتر المُحلَّل
         * @param errMsg رسالة الخطأ إن كان التحليل فاشلاً
         * @return true إن نجح، false إن كان التعبير غير صالح
         */
        bool parseFilter(const std::string &expr, ParsedFilter &out, std::string &errMsg);

    } // namespace Info
} // namespace Sad
