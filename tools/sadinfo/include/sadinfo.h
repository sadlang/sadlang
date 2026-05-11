// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: tools/sadinfo/include/sadinfo.h
// (AR) واجهات أداة sadinfo العامة. كل دالة dump تنتج JSON على stdout أو
//      في ملف محدد عبر -o، وفق هيكل ثابت موثَّق في docs/_generated/SCHEMA.md.
// (EN) Public API for sadinfo. Each dump function emits JSON to stdout or
//      to a file via -o, following a stable schema documented in
//      docs/_generated/SCHEMA.md.
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include <string>
#include <vector>

namespace Sad
{
    namespace Info
    {

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) إصدار schema الإخراج — يتغير عند أي كسر للتوافق
        // (EN) Output schema version — bumped on any breaking change
        //
        // (AR) v1.0.0 → v1.1.0 (Story 1.2): إضافة backward-compatible للحقلين
        //      id (مشتق من english) و deprecated في كل entry من keywords.
        // (EN) v1.0.0 → v1.1.0 (Story 1.2): backward-compatible addition of
        //      id (derived from english) and deprecated fields per entry.
        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) v1.1.0 → v1.2.0 (Story 1.3): إضافة قسم builtins (functions +
        //      methods) — backward-compatible (لا يمس keywords).
        // (EN) v1.1.0 → v1.2.0 (Story 1.3): added builtins section (functions
        //      and methods) — backward-compatible (does not touch keywords).
        inline constexpr const char *kSchemaVersion = "1.2.0";
        inline constexpr const char *kToolVersion = "0.3.0";

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) خيارات الأمر / (EN) Command options
        //
        // (AR) ملاحظة على lang:
        //      - "both" (افتراضي) = يحتفظ بكل الحقول.
        //      - "ar"             = يحذف حقل english فقط.
        //      - "en"             = يبقي word و english (word مفتاح أساسي).
        // (EN) Note on lang: "both" keeps all; "ar" drops english; "en" keeps both.
        // ───────────────────────────────────────────────────────────────────────────────
        struct DumpOptions
        {
            // ── إخراج عام / Common output ───────────────────────────────────────
            std::string outputPath;        ///< (AR) -o file (فارغ = stdout)
            bool        pretty   = true;   ///< (AR) تنسيق مقروء (افتراضي)
            std::string format   = "json"; ///< (AR) "json" (افتراضي) أو "yaml"

            // ── خاصة بـ dump-keywords / Keyword-specific ────────────────────────
            std::string lang              = "both"; ///< (AR) "both" | "ar" | "en"
            bool        minimal           = false;  ///< (AR) أبقِ word/tokenType/category فقط
            bool        byCategory        = false;  ///< (AR) جمّع تحت categories.{...}
            bool        includeDeprecated = false;  ///< (AR) ضمّن الكلمات المُهمَلة
            std::vector<std::string> filters;       ///< (AR) ["category=reserved", ...]
        };

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) أوامر dump — كل واحد ينتج JSON ويُرجع 0 عند النجاح
        // (EN) Dump commands — each emits JSON and returns 0 on success
        // ───────────────────────────────────────────────────────────────────────────────

        /**
         * @brief (AR) إخراج كل كلمات اللغة من keywords.yaml المولَّدة
         * @brief (EN) Dump all language keywords from generated keywords.yaml
         */
        int dumpKeywords(const DumpOptions &opts);

        /**
         * @brief (AR) إخراج كل الدوال المدمجة (stub في v0.1)
         * @brief (EN) Dump all built-in functions (stub in v0.1)
         */
        int dumpBuiltins(const DumpOptions &opts);

        /**
         * @brief (AR) إخراج كل أكواد الأخطاء (stub في v0.1)
         * @brief (EN) Dump all error codes (stub in v0.1)
         */
        int dumpErrors(const DumpOptions &opts);

        /**
         * @brief (AR) إخراج كل التوجيهات @ (stub في v0.1)
         * @brief (EN) Dump all @ directives (stub in v0.1)
         */
        int dumpDirectives(const DumpOptions &opts);

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) واجهة CLI / (EN) CLI entry
        // ───────────────────────────────────────────────────────────────────────────────

        /**
         * @brief (AR) نقطة الدخول الرئيسية بعد معالجة UTF-8
         * @brief (EN) Main entry after UTF-8 setup
         */
        int sadinfoMain(int argc, char **argv);

    } // namespace Info
} // namespace Sad
