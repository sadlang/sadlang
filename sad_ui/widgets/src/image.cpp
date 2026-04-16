/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: image.cpp
 * المسار: sad_ui/widgets/src/image.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ عنصر الصورة (Image Widget).
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/widgets/image.h"

namespace sad {
namespace ui {
namespace widgets {

ImageBuilder::ImageBuilder(const std::string& source)
    : source_(source)
{
}

ImageBuilder& ImageBuilder::width(float w) {
    width_ = w;
    return *this;
}

ImageBuilder& ImageBuilder::height(float h) {
    height_ = h;
    return *this;
}

ImageBuilder& ImageBuilder::cornerRadius(float radius) {
    cornerRadius_ = radius;
    return *this;
}

ImageBuilder& ImageBuilder::contentMode(const std::string& mode) {
    contentMode_ = mode;
    return *this;
}

ImageBuilder& ImageBuilder::altText(const std::string& text) {
    altText_ = text;
    return *this;
}

ImageBuilder& ImageBuilder::sourceType(ImageSource type) {
    sourceType_ = type;
    return *this;
}

std::shared_ptr<UINode> ImageBuilder::build() const {
    auto node = UINode::create(UINodeType::Image);
    node->setPrimaryArg(source_);

    if (width_) {
        node->addModifier(Modifier::numeric(ModifierType::Width, *width_));
    }
    if (height_) {
        node->addModifier(Modifier::numeric(ModifierType::Height, *height_));
    }
    if (cornerRadius_) {
        node->addModifier(Modifier::numeric(ModifierType::CornerRadius, *cornerRadius_));
    }
    if (contentMode_) {
        node->addModifier({ModifierType::ContentMode, *contentMode_, {}});
    }

    return node;
}

std::shared_ptr<UINode> makeImage(const std::string& source) {
    return ImageBuilder(source).build();
}

} // namespace widgets
} // namespace ui
} // namespace sad
