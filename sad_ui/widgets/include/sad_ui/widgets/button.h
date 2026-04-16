/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: button.h
 * المسار: sad_ui/widgets/include/sad_ui/widgets/button.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الوصف:
 * ------
 * عنصر الزر (Button Widget) — عنصر تفاعلي يستجيب للنقر.
 *
 * الاستخدام في لغة ص:
 * ────────────────────
 *   زر("اضغط هنا")
 *       .لون_خلفية(.أزرق)
 *       .نصف_قطر(12)
 *       .عند_النقر { اطبع("تم!") }
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_WIDGETS_BUTTON_H
#define SAD_UI_WIDGETS_BUTTON_H

#include "sad_ui/node.h"
#include "sad_ui/style.h"

#include <string>
#include <memory>
#include <optional>
#include <functional>

namespace sad {
namespace ui {
namespace widgets {

/**
 * @brief بناء عقدة زر بطريقة برمجية
 */
class ButtonBuilder {
public:
    /// إنشاء زر بنص
    explicit ButtonBuilder(const std::string& label);

    /// تعيين لون خلفية الزر
    ButtonBuilder& backgroundColor(Color c);

    /// تعيين لون نص الزر
    ButtonBuilder& textColor(Color c);

    /// تعيين حجم خط الزر
    ButtonBuilder& fontSize(float size);

    /// تعيين نصف قطر الزوايا
    ButtonBuilder& cornerRadius(float radius);

    /// تعيين الحشوة الداخلية
    ButtonBuilder& padding(EdgeInsets p);

    /// تعيين حدث النقر (تعبير بلغة ص)
    ButtonBuilder& onTap(const std::string& expression);

    /// تعيين حدث الضغط المطوّل
    ButtonBuilder& onLongPress(const std::string& expression);

    /// تعيين ما إذا كان الزر مُعطّلاً
    ButtonBuilder& disabled(bool d);

    /// بناء العقدة النهائية
    std::shared_ptr<UINode> build() const;

private:
    std::string label_;                              ///< نص الزر
    std::optional<Color> backgroundColor_;           ///< لون الخلفية
    std::optional<Color> textColor_;                 ///< لون النص
    std::optional<float> fontSize_;                  ///< حجم الخط
    std::optional<float> cornerRadius_;              ///< نصف قطر الزوايا
    std::optional<EdgeInsets> padding_;               ///< الحشوة
    std::optional<std::string> onTapExpr_;           ///< تعبير حدث النقر
    std::optional<std::string> onLongPressExpr_;     ///< تعبير الضغط المطوّل
    bool disabled_ = false;                          ///< هل الزر مُعطّل
};

/// دالة مساعدة لإنشاء عقدة زر بسرعة
std::shared_ptr<UINode> makeButton(const std::string& label);

} // namespace widgets
} // namespace ui
} // namespace sad

#endif // SAD_UI_WIDGETS_BUTTON_H
