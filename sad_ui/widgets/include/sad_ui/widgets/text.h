/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: text.h
 * المسار: sad_ui/widgets/include/sad_ui/widgets/text.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الوصف:
 * ------
 * عنصر النص (Text Widget) — أكثر عناصر الواجهة استخداماً.
 * يعرض نصاً على الشاشة مع دعم كامل للعربية والتنسيق.
 *
 * الاستخدام في لغة ص:
 * ────────────────────
 *   نص("مرحباً بالعالم")
 *       .حجم_خط(24)
 *       .لون(.أزرق)
 *       .خط_عريض()
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_WIDGETS_TEXT_H
#define SAD_UI_WIDGETS_TEXT_H

#include "sad_ui/node.h"
#include "sad_ui/style.h"

#include <string>
#include <memory>
#include <optional>

namespace sad {
namespace ui {
namespace widgets {

/**
 * @brief بناء عقدة نص بطريقة برمجية
 *
 * يُسهّل إنشاء عقد النص مع تطبيق التنسيقات الشائعة.
 * يُستخدم داخلياً من المحلل النحوي ويمكن استخدامه مباشرة.
 */
class TextBuilder {
public:
    /// إنشاء عنصر نص بمحتوى مبدئي
    explicit TextBuilder(const std::string& content);

    /// تعيين حجم الخط
    TextBuilder& fontSize(float size);

    /// تعيين لون النص
    TextBuilder& color(Color c);

    /// تعيين وزن الخط (عريض، عادي...)
    TextBuilder& fontWeight(FontWeight weight);

    /// تعيين عائلة الخط
    TextBuilder& fontFamily(const std::string& family);

    /// تعيين محاذاة النص
    TextBuilder& textAlign(AlignmentType align);

    /// تعيين أقصى عدد أسطر (0 = بلا حد)
    TextBuilder& maxLines(int lines);

    /// تعيين تباعد الأسطر
    TextBuilder& lineSpacing(float spacing);

    /// بناء العقدة النهائية
    std::shared_ptr<UINode> build() const;

private:
    std::string content_;                           ///< محتوى النص
    std::optional<float> fontSize_;                 ///< حجم الخط
    std::optional<Color> color_;                    ///< لون النص
    std::optional<FontWeight> fontWeight_;           ///< وزن الخط
    std::optional<std::string> fontFamily_;          ///< عائلة الخط
    std::optional<AlignmentType> textAlign_;         ///< المحاذاة
    std::optional<int> maxLines_;                    ///< أقصى عدد أسطر
    std::optional<float> lineSpacing_;               ///< تباعد الأسطر
};

/// دالة مساعدة لإنشاء عقدة نص بسرعة
std::shared_ptr<UINode> makeText(const std::string& content);

} // namespace widgets
} // namespace ui
} // namespace sad

#endif // SAD_UI_WIDGETS_TEXT_H
