// بسم الله الرحمن الرحيم
// ============================================================================
// formatter_keywords.h — تصنيف الكلمات المفتاحية لأدوات لغة ص
// Keyword categorization for Sad language tools (formatter, LSP, etc.)
// ============================================================================
//
// (AR) طبقة مشتركة لأدوات لغة ص (المنسق، LSP، ...). تبني تصنيفاً سياقياً
//      للكلمات اعتماداً على الجدول الرسمي في
//      `shared/lexer/include/lexer_keywords.h` (مصدر الحقيقة الوحيد).
//
// (EN) Tools-shared layer. Builds a contextual classification on top of the
//      canonical keyword table in `shared/lexer/include/lexer_keywords.h`
//      (single source of truth).
//
// المبادئ المعمارية / Architectural Principles:
//   - أي كلمة محجوزة (40 كلمة) تُكتشف عبر `KeywordTable::isKeyword`.
//   - الكلمات السياقية (طابق، عندما، قالب،...) مُعرَّفة هنا لأن المفسر
//     يتعامل معها كمُعرّفات IDENTIFIER، لكن أدوات التحرير تحتاج معرفتها
//     لتلوينها وضبط المسافة البادئة بشكل صحيح.
//   - أي تغيير في `KeywordTable::initialize()` ينعكس تلقائياً على هذه
//     الطبقة بدون أي تعديل يدوي.
// ============================================================================

#ifndef SAD_TOOLS_SHARED_FORMATTER_KEYWORDS_H
#define SAD_TOOLS_SHARED_FORMATTER_KEYWORDS_H

#include <string>
#include <vector>

namespace Sad
{
    namespace ToolsShared
    {

        // ============================================================================
        // تصنيف الكلمات / Keyword Classification
        // ============================================================================

        /// (AR) هل الكلمة مفتاحية معروفة لأدوات لغة ص؟
        /// (EN) Is the word a keyword recognized by Sad tooling?
        ///
        /// تشمل الكلمات المحجوزة (40 من `KeywordTable`) والكلمات السياقية المهمة
        /// لأدوات التحرير (طابق، عندما، قالب، ...).
        bool isToolKeyword(const std::string &word);

        /// (AR) هل الكلمة من كلمات الاستيراد (استورد، من، صدّر، كـ)؟
        /// (EN) Is the word an import-related keyword?
        bool isImportKeyword(const std::string &word);

        /// (AR) هل الكلمة تفتح كتلة (دالة، صنف، إذا، بينما، ...)؟
        /// (EN) Does the word open a block?
        bool isBlockOpenerKeyword(const std::string &word);

        /// (AR) هل الكلمة تغلق كتلة (نهاية، نهاية_فضاء، ...)؟
        /// (EN) Does the word close a block?
        bool isBlockCloserKeyword(const std::string &word);

        /// (AR) هل الكلمة بين-كتلية (وإلا، امسك، أخيراً، عندما، ...)؟
        /// (EN) Is it an inter-block keyword?
        bool isInterBlockKeyword(const std::string &word);

        /// (AR) هل الكلمة اسم نوع مدمج (رقم، نص، عشري، ...)؟
        /// (EN) Is the word a built-in type name?
        bool isBuiltInTypeName(const std::string &word);

        /// (AR) قائمة بكل الكلمات السياقية الإضافية التي تعرفها هذه الطبقة
        ///      (للاختبار والتشخيص فقط).
        /// (EN) List of all extra contextual keywords known to this layer
        ///      (for testing/diagnostics only).
        const std::vector<std::string> &getContextualKeywords();

        /// (AR) قائمة بأسماء الأنواع المدمجة (للتلوين فقط، ليست محجوزة).
        /// (EN) Built-in type names (for highlighting only, not reserved).
        const std::vector<std::string> &getBuiltInTypeNames();

    } // namespace ToolsShared
} // namespace Sad

#endif // SAD_TOOLS_SHARED_FORMATTER_KEYWORDS_H
