/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: text.cpp
 * المسار: sad_ui/widgets/src/text.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ عنصر النص (Text Widget).
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/widgets/text.h"

namespace sad {
namespace ui {
namespace widgets {

TextBuilder::TextBuilder(const std::string& content)
    : content_(content)
{
}

TextBuilder& TextBuilder::fontSize(float size) {
    fontSize_ = size;
    return *this;
}

TextBuilder& TextBuilder::color(Color c) {
    color_ = c;
    return *this;
}

TextBuilder& TextBuilder::fontWeight(FontWeight weight) {
    fontWeight_ = weight;
    return *this;
}

TextBuilder& TextBuilder::fontFamily(const std::string& family) {
    fontFamily_ = family;
    return *this;
}

TextBuilder& TextBuilder::textAlign(AlignmentType align) {
    textAlign_ = align;
    return *this;
}

TextBuilder& TextBuilder::maxLines(int lines) {
    maxLines_ = lines;
    return *this;
}

TextBuilder& TextBuilder::lineSpacing(float spacing) {
    lineSpacing_ = spacing;
    return *this;
}

std::shared_ptr<UINode> TextBuilder::build() const {
    auto node = UINode::create(UINodeType::Text);
    node->setPrimaryArg(content_);

    // تطبيق المعدّلات المحددة
    if (fontSize_) {
        node->addModifier(Modifier::numeric(ModifierType::FontSize, *fontSize_));
    }
    if (color_) {
        node->addModifier(Modifier::color(ModifierType::ForegroundColor, *color_));
    }
    if (fontWeight_) {
        node->addModifier(Modifier::numeric(
            ModifierType::FontWeight,
            static_cast<float>(static_cast<int>(*fontWeight_))
        ));
    }
    if (fontFamily_) {
        node->addModifier({ModifierType::FontFamily, *fontFamily_, {}});
    }
    if (textAlign_) {
        node->addModifier({ModifierType::TextAlign, *textAlign_, {}});
    }
    if (maxLines_) {
        node->addModifier(Modifier::numeric(ModifierType::MaxLines, static_cast<float>(*maxLines_)));
    }
    if (lineSpacing_) {
        node->addModifier(Modifier::numeric(ModifierType::LineSpacing, *lineSpacing_));
    }

    return node;
}

std::shared_ptr<UINode> makeText(const std::string& content) {
    return TextBuilder(content).build();
}

} // namespace widgets
} // namespace ui
} // namespace sad
