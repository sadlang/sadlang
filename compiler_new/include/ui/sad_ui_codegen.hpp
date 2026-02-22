/**
 * ==========================================================================
 * ملف: sad_ui_codegen.hpp
 * الوصف: واجهة موحدة لمولدات الكود — SadUI IR → كود أصلي
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 *
 * كل مولد كود (Desktop/Android/iOS/Web) يُنفّذ هذه الواجهة.
 */

#ifndef SAD_UI_CODEGEN_HPP
#define SAD_UI_CODEGEN_HPP

#include "sad_ui_ir.hpp"
#include <string>
#include <vector>
#include <unordered_map>

namespace sad {
namespace ui {
namespace ir {

// ═══════════════════════════════════════════════════════════════════════════
// ملف مُولّد
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief ملف ناتج من مولد الكود
 */
struct GeneratedFile {
    std::string path;           ///< مسار الملف النسبي (مثل "app/src/main/java/Activity.kt")
    std::string content;        ///< محتوى الملف
    bool isBinary = false;      ///< هل هو ملف ثنائي؟
    std::string language;       ///< لغة الملف (cpp, kt, swift, html)
};

/**
 * @brief نتيجة توليد الكود
 */
struct CodeGenResult {
    bool success = false;
    std::vector<GeneratedFile> files;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::string buildCommand;   ///< أمر البناء المقترح
};


// ═══════════════════════════════════════════════════════════════════════════
// واجهة مولد الكود
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief واجهة موحدة لمولدات الكود
 */
class ICodeGenerator {
public:
    virtual ~ICodeGenerator() = default;

    /// توليد كود لتطبيق كامل
    virtual CodeGenResult generate(const AppIR& app) = 0;

    /// توليد كود لشاشة واحدة
    virtual CodeGenResult generateScreen(const ScreenIR& screen) = 0;

    /// المنصة المستهدفة
    virtual TargetPlatform targetPlatform() const = 0;

    /// اسم المنصة (للعرض)
    virtual std::string platformName() const = 0;
};


} // namespace ir
} // namespace ui
} // namespace sad

#endif // SAD_UI_CODEGEN_HPP
