// ============================================================================
// AUTO-GENERATED FROM data/language/keywords.yaml — DO NOT EDIT MANUALLY
// (AR) ملف مولَّد تلقائياً. لإجراء تعديل، عدّل YAML وأعد البناء.
// (EN) Auto-generated file. To modify, edit YAML and rebuild.
// ============================================================================

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "token.h"

namespace Sad {
namespace Lexer {
namespace Generated {

/**
 * @brief (AR) فئات الكلمات في معجم لغة ص
 * @brief (EN) Keyword categories in the Sad lexicon
 */
enum class KeywordCategory : std::uint8_t {
    RESERVED     = 0,  ///< (AR) محجوزة دائماً، Lexer يُصدر KEYWORD_*
    OPERATOR     = 1,  ///< (AR) عوامل منطقية كلمات (و/أو/ليس)
    CONTEXTUAL   = 2,  ///< (AR) سياقية، Lexer يُصدر IDENTIFIER
    BUILTIN_TYPE = 3   ///< (AR) أسماء أنواع مدمجة، Lexer يُصدر IDENTIFIER
};

/**
 * @brief (AR) إدخال واحد في المعجم
 * @brief (EN) Single lexicon entry
 */
struct KeywordEntry {
    std::string                primaryWord;     ///< (AR) الكلمة الرئيسية
    TokenType                  type;            ///< (AR) نوع الرمز
    KeywordCategory            category;        ///< (AR) الفئة
    bool                       emittedByLexer;  ///< (AR) هل Lexer يُصدرها كرمز خاص؟
    std::vector<std::string>   aliases;         ///< (AR) أسماء بديلة (بدون تشكيل/همزة)
    std::vector<std::string>   roles;           ///< (AR) أدوار دلالية (block_opener, ...)
    std::string                english;         ///< (AR) المرادف الإنجليزي (للتوثيق)
};

/**
 * @brief (AR) كل إدخالات المعجم
 * @brief (EN) Full lexicon entries
 */
const std::vector<KeywordEntry>& allEntries();

/**
 * @brief (AR) عدد الإدخالات الإجمالي (compile-time)
 * @brief (EN) Total entry count (compile-time)
 */
inline constexpr std::size_t kEntryCount = 92;

} // namespace Generated
} // namespace Lexer
} // namespace Sad
