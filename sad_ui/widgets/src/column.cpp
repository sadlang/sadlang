/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: column.cpp
 * المسار: sad_ui/widgets/src/column.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ عنصر العمود (Column Widget).
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/widgets/column.h"

namespace sad {
namespace ui {
namespace widgets {

ColumnBuilder& ColumnBuilder::spacing(float s) {
    spacing_ = s;
    return *this;
}

ColumnBuilder& ColumnBuilder::alignment(AlignmentType align) {
    alignment_ = align;
    return *this;
}

ColumnBuilder& ColumnBuilder::padding(EdgeInsets p) {
    padding_ = p;
    return *this;
}

ColumnBuilder& ColumnBuilder::backgroundColor(Color c) {
    backgroundColor_ = c;
    return *this;
}

ColumnBuilder& ColumnBuilder::child(std::shared_ptr<UINode> node) {
    children_.push_back(std::move(node));
    return *this;
}

ColumnBuilder& ColumnBuilder::children(std::vector<std::shared_ptr<UINode>> nodes) {
    for (auto& n : nodes) {
        children_.push_back(std::move(n));
    }
    return *this;
}

std::shared_ptr<UINode> ColumnBuilder::build() const {
    auto node = UINode::create(UINodeType::Column);

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

std::shared_ptr<UINode> makeColumn() {
    return UINode::create(UINodeType::Column);
}

std::shared_ptr<UINode> makeColumn(std::vector<std::shared_ptr<UINode>> children) {
    ColumnBuilder builder;
    builder.children(std::move(children));
    return builder.build();
}

} // namespace widgets
} // namespace ui
} // namespace sad
