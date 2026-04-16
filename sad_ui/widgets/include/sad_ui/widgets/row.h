/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: row.h
 * المسار: sad_ui/widgets/include/sad_ui/widgets/row.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الوصف:
 * ------
 * عنصر الصف (Row Widget) — حاوية ترتب أبناءها أفقياً.
 *
 * ملاحظة مهمة: في الوضع RTL (الافتراضي في لغة ص)، الترتيب يكون من
 * اليمين لليسار. هذا يتم تلقائياً بواسطة محرك التخطيط.
 *
 * الاستخدام في لغة ص:
 * ────────────────────
 *   صف
 *       .تباعد(12)
 *       صورة("أيقونة.png").عرض(32)
 *       نص("اسم المستخدم")
 *   نهاية
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_WIDGETS_ROW_H
#define SAD_UI_WIDGETS_ROW_H

#include "sad_ui/node.h"

#include <string>
#include <memory>
#include <vector>
#include <optional>

namespace sad {
namespace ui {
namespace widgets {

/**
 * @brief بناء عقدة صف بطريقة برمجية
 *
 * الصف يرتب أبناءه أفقياً. في وضع RTL (الافتراضي)،
 * الترتيب يكون من اليمين لليسار تلقائياً.
 */
class RowBuilder {
public:
    RowBuilder() = default;

    /// تعيين التباعد بين الأبناء
    RowBuilder& spacing(float s);

    /// تعيين محاذاة الأبناء عمودياً
    RowBuilder& alignment(AlignmentType align);

    /// تعيين الحشوة الداخلية
    RowBuilder& padding(EdgeInsets p);

    /// تعيين لون الخلفية
    RowBuilder& backgroundColor(Color c);

    /// إضافة ابن واحد
    RowBuilder& child(std::shared_ptr<UINode> node);

    /// إضافة عدة أبناء
    RowBuilder& children(std::vector<std::shared_ptr<UINode>> nodes);

    /// بناء العقدة النهائية
    std::shared_ptr<UINode> build() const;

private:
    std::optional<float> spacing_;                           ///< التباعد
    std::optional<AlignmentType> alignment_;                 ///< المحاذاة
    std::optional<EdgeInsets> padding_;                       ///< الحشوة
    std::optional<Color> backgroundColor_;                   ///< لون الخلفية
    std::vector<std::shared_ptr<UINode>> children_;          ///< الأبناء
};

/// دالة مساعدة لإنشاء صف فارغ
std::shared_ptr<UINode> makeRow();

/// دالة مساعدة لإنشاء صف بأبناء
std::shared_ptr<UINode> makeRow(std::vector<std::shared_ptr<UINode>> children);

} // namespace widgets
} // namespace ui
} // namespace sad

#endif // SAD_UI_WIDGETS_ROW_H
