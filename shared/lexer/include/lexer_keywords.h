// ======================================================================
// lexer_keywords.h - جدول الكلمات (v4.1) / Keyword Table (v4.1)
// ======================================================================
// (AR) واجهة جدول الكلمات المفتاحية. تُبنى البيانات من المعجم المولَّد
//      shared/lexer/generated/keywords_generated.{h,cpp} المنتج من
//      language-truth/keywords.yaml. لا توجد سلاسل عربية مكتوبة يدوياً
//      في هذا الملف ولا في تنفيذه.
// (EN) Keyword table API. Data is built from auto-generated
//      shared/lexer/generated/keywords_generated.{h,cpp} produced from
//      language-truth/keywords.yaml. No hand-written Arabic strings live
//      in this header or its implementation.
// ======================================================================

#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "token.h"
#include "../generated/keywords_generated.h"

namespace Sad {
namespace Lexer {

/**
 * @class KeywordTable
 * @brief (AR) جدول قراءة فقط مبني من المعجم المولَّد من YAML
 * @brief (EN) Read-only table built from the YAML-generated lexicon
 *
 * (AR) الفئة كلها static. التهيئة كسولة وآمنة لإعادة الاستدعاء.
 * (EN) Fully static. Initialization is lazy and re-entrant safe.
 */
class KeywordTable {
public:
    using KeywordCategory = Generated::KeywordCategory;
    using KeywordEntry    = Generated::KeywordEntry;

    // ── (AR) تهيئة وفحص حالة الجدول ──
    static void initialize();
    static bool isInitialized();

    // ──────────────────────────────────────────────────────────────────
    // (AR) واجهة الـ Lexer — الكلمات التي يُصدرها Lexer كرمز خاص
    //      (emittedByLexer = true: المحجوزة + العوامل المنطقية فقط).
    // (EN) Lexer-facing API — only entries with emittedByLexer = true
    //      (reserved + logical operators).
    // ──────────────────────────────────────────────────────────────────

    /**
     * @brief (AR) هل الكلمة كلمة مفتاحية يُصدرها Lexer؟
     * @brief (EN) Is this word emitted by the Lexer as a special token?
     */
    static bool isKeyword(std::string_view word);

    /**
     * @brief (AR) نوع الرمز للكلمة المُصدرة (محجوز/عامل)
     * @brief (EN) TokenType for an emitted keyword (reserved/operator)
     * @throws std::runtime_error إذا لم تكن الكلمة مُصدرة
     */
    static TokenType getKeywordType(std::string_view word);

    /**
     * @brief (AR) الكلمة الرئيسية لنوع رمز مُصدر
     * @brief (EN) Primary spelling for an emitted TokenType
     * @throws std::runtime_error إذا لم يكن النوع مُصدراً
     */
    static std::string getKeyword(TokenType type);

    // ──────────────────────────────────────────────────────────────────
    // (AR) واجهة الـ Parser — وصول كامل لكل المعجم بما في ذلك السياقي
    // (EN) Parser-facing API — full access including contextual entries
    // ──────────────────────────────────────────────────────────────────

    /**
     * @brief (AR) إيجاد إدخال عبر TokenType (يبحث في كل الفئات)
     * @brief (EN) Find entry by TokenType (searches all categories)
     */
    static const KeywordEntry* getEntry(TokenType type);

    /**
     * @brief (AR) إيجاد إدخال عبر النص (كل الفئات + الأسماء البديلة)
     * @brief (EN) Find entry by spelling (all categories + aliases)
     */
    static const KeywordEntry* getEntry(std::string_view word);

    /**
     * @brief (AR) فئة الكلمة أو nullopt إن لم تكن في المعجم
     * @brief (EN) Category for a word or nullopt if not in lexicon
     */
    static std::optional<KeywordCategory> getCategory(std::string_view word);

    /**
     * @brief (AR) كل الكلمات الرئيسية في فئة معينة (بترتيب YAML)
     * @brief (EN) All primary words in a given category (YAML order)
     */
    static const std::vector<std::string>& getKeywordsByCategory(KeywordCategory category);

    /**
     * @brief (AR) كل الكلمات الرئيسية التي تحمل دور دلالي معين
     * @brief (EN) All primary words carrying a given semantic role
     */
    static const std::vector<std::string>& getKeywordsByRole(std::string_view role);

    /**
     * @brief (AR) هل Lexer يجب أن يُصدر هذا النوع كرمز خاص؟
     * @brief (EN) Should the Lexer emit this TokenType as a special token?
     */
    static bool shouldEmitAsKeyword(TokenType type);

    /**
     * @brief (AR) كل الكلمات المُصدرة من Lexer (أبجدياً) — توافق خلفي
     * @brief (EN) All Lexer-emitted spellings (alphabetical) — backward compat
     */
    static std::vector<std::string> getAllKeywords();

private:
    static bool initialized_;
    static std::unordered_map<std::string, const KeywordEntry*> wordIndex_;
    static std::unordered_map<int, const KeywordEntry*>         typeIndex_;
    static std::vector<std::string>                             emittedWords_;
    static std::vector<std::vector<std::string>>                byCategory_;
    static std::unordered_map<std::string, std::vector<std::string>> byRole_;
    static const std::vector<std::string>                       kEmpty_;
};

} // namespace Lexer
} // namespace Sad
