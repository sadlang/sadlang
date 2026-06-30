/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: button.cpp
 * المسار: features/graphics/widgets/src/button.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ عنصر الزر (Button Widget).
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/widgets/button.h"

namespace sad {
namespace ui {
namespace widgets {

ButtonBuilder::ButtonBuilder(const std::string& label)
    : label_(label)
{
}

ButtonBuilder& ButtonBuilder::backgroundColor(Color c) {
    backgroundColor_ = c;
    return *this;
}

ButtonBuilder& ButtonBuilder::textColor(Color c) {
    textColor_ = c;
    return *this;
}

ButtonBuilder& ButtonBuilder::fontSize(float size) {
    fontSize_ = size;
    return *this;
}

ButtonBuilder& ButtonBuilder::cornerRadius(float radius) {
    cornerRadius_ = radius;
    return *this;
}

ButtonBuilder& ButtonBuilder::padding(EdgeInsets p) {
    padding_ = p;
    return *this;
}

ButtonBuilder& ButtonBuilder::onTap(const std::string& expression) {
    onTapExpr_ = expression;
    return *this;
}

ButtonBuilder& ButtonBuilder::onLongPress(const std::string& expression) {
    onLongPressExpr_ = expression;
    return *this;
}

ButtonBuilder& ButtonBuilder::disabled(bool d) {
    disabled_ = d;
    return *this;
}

std::shared_ptr<UINode> ButtonBuilder::build() const {
    auto node = UINode::create(UINodeType::Button);
    node->setPrimaryArg(label_);

    if (backgroundColor_) {
        node->addModifier(Modifier::color(ModifierType::BackgroundColor, *backgroundColor_));
    }
    if (textColor_) {
        node->addModifier(Modifier::color(ModifierType::ForegroundColor, *textColor_));
    }
    if (fontSize_) {
        node->addModifier(Modifier::numeric(ModifierType::FontSize, *fontSize_));
    }
    if (cornerRadius_) {
        node->addModifier(Modifier::numeric(ModifierType::CornerRadius, *cornerRadius_));
    }
    if (padding_) {
        node->addModifier({ModifierType::Padding, *padding_, {}});
    }
    if (onTapExpr_) {
        node->addModifier(Modifier::event(ModifierType::OnTap, *onTapExpr_));
    }
    if (onLongPressExpr_) {
        node->addModifier(Modifier::event(ModifierType::OnLongPress, *onLongPressExpr_));
    }
    if (disabled_) {
        node->addModifier({ModifierType::Disabled, true, {}});
    }

    return node;
}

std::shared_ptr<UINode> makeButton(const std::string& label) {
    return ButtonBuilder(label).build();
}

} // namespace widgets
} // namespace ui
} // namespace sad
