/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: column.h
 * المسار: features/graphics/widgets/include/sad_ui/widgets/column.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الوصف:
 * ------
 * عنصر العمود (Column Widget) — حاوية ترتب أبناءها عمودياً.
 *
 * الاستخدام في لغة ص:
 * ────────────────────
 *   عمود
 *       .تباعد(8)
 *       .حشوة(16)
 *       نص("العنوان")
 *       نص("المحتوى")
 *   نهاية
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_WIDGETS_COLUMN_H
#define SAD_UI_WIDGETS_COLUMN_H

#include "sad_ui/node.h"

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <initializer_list>

namespace sad {
namespace ui {
namespace widgets {

/**
 * @brief بناء عقدة عمود بطريقة برمجية
 *
 * العمود يرتب أبناءه عمودياً (من أعلى لأسفل).
 * يدعم التباعد، المحاذاة، والتوزيع.
 */
class ColumnBuilder {
public:
    ColumnBuilder() = default;

    /// تعيين التباعد بين الأبناء (بالنقاط)
    ColumnBuilder& spacing(float s);

    /// تعيين محاذاة الأبناء أفقياً
    ColumnBuilder& alignment(AlignmentType align);

    /// تعيين الحشوة الداخلية
    ColumnBuilder& padding(EdgeInsets p);

    /// تعيين لون الخلفية
    ColumnBuilder& backgroundColor(Color c);

    /// إضافة ابن واحد
    ColumnBuilder& child(std::shared_ptr<UINode> node);

    /// إضافة عدة أبناء
    ColumnBuilder& children(std::vector<std::shared_ptr<UINode>> nodes);

    /// بناء العقدة النهائية
    std::shared_ptr<UINode> build() const;

private:
    std::optional<float> spacing_;                           ///< التباعد
    std::optional<AlignmentType> alignment_;                 ///< المحاذاة
    std::optional<EdgeInsets> padding_;                       ///< الحشوة
    std::optional<Color> backgroundColor_;                   ///< لون الخلفية
    std::vector<std::shared_ptr<UINode>> children_;          ///< الأبناء
};

/// دالة مساعدة لإنشاء عمود فارغ
std::shared_ptr<UINode> makeColumn();

/// دالة مساعدة لإنشاء عمود بأبناء
std::shared_ptr<UINode> makeColumn(std::vector<std::shared_ptr<UINode>> children);

} // namespace widgets
} // namespace ui
} // namespace sad

#endif // SAD_UI_WIDGETS_COLUMN_H
