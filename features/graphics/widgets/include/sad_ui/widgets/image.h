/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: image.h
 * المسار: features/graphics/widgets/include/sad_ui/widgets/image.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الوصف:
 * ------
 * عنصر الصورة (Image Widget) — يعرض صورة من مصدر محلي أو URL.
 *
 * الاستخدام في لغة ص:
 * ────────────────────
 *   صورة("شعار.png")
 *       .عرض(200)
 *       .ارتفاع(150)
 *       .نصف_قطر(16)
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_WIDGETS_IMAGE_H
#define SAD_UI_WIDGETS_IMAGE_H

#include "sad_ui/node.h"

#include <string>
#include <memory>
#include <optional>

namespace sad {
namespace ui {
namespace widgets {

/// نوع مصدر الصورة
enum class ImageSource : uint8_t {
    File,       ///< ملف محلي
    Url,        ///< رابط إنترنت
    Asset,      ///< مورد داخل التطبيق
    Memory      ///< بيانات في الذاكرة (base64)
};

/**
 * @brief بناء عقدة صورة بطريقة برمجية
 */
class ImageBuilder {
public:
    /// إنشاء صورة من مسار أو رابط
    explicit ImageBuilder(const std::string& source);

    /// تعيين عرض الصورة
    ImageBuilder& width(float w);

    /// تعيين ارتفاع الصورة
    ImageBuilder& height(float h);

    /// تعيين نصف قطر الزوايا
    ImageBuilder& cornerRadius(float radius);

    /// تعيين طريقة ملء الصورة (تمدد، ملء، احتواء)
    ImageBuilder& contentMode(const std::string& mode);

    /// تعيين نص بديل (للوصول)
    ImageBuilder& altText(const std::string& text);

    /// تعيين نوع المصدر
    ImageBuilder& sourceType(ImageSource type);

    /// بناء العقدة النهائية
    std::shared_ptr<UINode> build() const;

private:
    std::string source_;                             ///< مسار أو رابط المصدر
    ImageSource sourceType_ = ImageSource::File;     ///< نوع المصدر
    std::optional<float> width_;                     ///< العرض
    std::optional<float> height_;                    ///< الارتفاع
    std::optional<float> cornerRadius_;              ///< نصف قطر الزوايا
    std::optional<std::string> contentMode_;         ///< طريقة الملء
    std::optional<std::string> altText_;             ///< النص البديل
};

/// دالة مساعدة لإنشاء عقدة صورة
std::shared_ptr<UINode> makeImage(const std::string& source);

} // namespace widgets
} // namespace ui
} // namespace sad

#endif // SAD_UI_WIDGETS_IMAGE_H
