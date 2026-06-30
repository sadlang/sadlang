/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: row.cpp
 * المسار: features/graphics/widgets/src/row.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ عنصر الصف (Row Widget).
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/widgets/row.h"

namespace sad {
namespace ui {
namespace widgets {

RowBuilder& RowBuilder::spacing(float s) {
    spacing_ = s;
    return *this;
}

RowBuilder& RowBuilder::alignment(AlignmentType align) {
    alignment_ = align;
    return *this;
}

RowBuilder& RowBuilder::padding(EdgeInsets p) {
    padding_ = p;
    return *this;
}

RowBuilder& RowBuilder::backgroundColor(Color c) {
    backgroundColor_ = c;
    return *this;
}

RowBuilder& RowBuilder::child(std::shared_ptr<UINode> node) {
    children_.push_back(std::move(node));
    return *this;
}

RowBuilder& RowBuilder::children(std::vector<std::shared_ptr<UINode>> nodes) {
    for (auto& n : nodes) {
        children_.push_back(std::move(n));
    }
    return *this;
}

std::shared_ptr<UINode> RowBuilder::build() const {
    auto node = UINode::create(UINodeType::Row);

    if (spacing_) {
        node->addModifier(Modifier::numeric(ModifierType::Spacing, *spacing_));
    }
    if (alignment_) {
        node->addModifier({ModifierType::Alignment, *alignment_, {}});
    }
    if (padding_) {
        node->addModifier({ModifierType::Padding, *padding_, {}});
    }
    if (backgroundColor_) {
        node->addModifier(Modifier::color(ModifierType::BackgroundColor, *backgroundColor_));
    }

    for (const auto& child : children_) {
        node->addChild(child);
    }

    return node;
}

std::shared_ptr<UINode> makeRow() {
    return UINode::create(UINodeType::Row);
}

std::shared_ptr<UINode> makeRow(std::vector<std::shared_ptr<UINode>> children) {
    RowBuilder builder;
    builder.children(std::move(children));
    return builder.build();
}

} // namespace widgets
} // namespace ui
} // namespace sad
