/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: layout_widgets.cpp
 * المسار: sad_ui/widgets/src/layout_widgets.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ عناصر التخطيط: Stack، Grid، LazyColumn، LazyRow، ScrollView، Spacer، Container
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/widgets/layout_widgets.h"

namespace sad {
namespace ui {
namespace widgets {

// ═══════════════════════════════════════════════════════════════════════════════
// StackBuilder
// ═══════════════════════════════════════════════════════════════════════════════

StackBuilder::StackBuilder() = default;

StackBuilder& StackBuilder::alignment(AlignmentType align) { alignment_ = align; return *this; }
StackBuilder& StackBuilder::addChild(std::shared_ptr<UINode> child) { children_.push_back(std::move(child)); return *this; }

std::shared_ptr<UINode> StackBuilder::build() const {
    auto node = UINode::create(UINodeType::Stack);
    if (alignment_) node->addModifier({ModifierType::Alignment, *alignment_, {}});
    for (const auto& child : children_) node->addChild(child);
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// GridBuilder
// ═══════════════════════════════════════════════════════════════════════════════

GridBuilder::GridBuilder() = default;

GridBuilder& GridBuilder::columns(int count) { columns_ = count; return *this; }
GridBuilder& GridBuilder::spacing(float s) { spacing_ = s; return *this; }
GridBuilder& GridBuilder::addChild(std::shared_ptr<UINode> child) { children_.push_back(std::move(child)); return *this; }

std::shared_ptr<UINode> GridBuilder::build() const {
    auto node = UINode::create(UINodeType::Grid);
    node->addModifier(Modifier::numeric(ModifierType::MaxLines, static_cast<float>(columns_)));
    if (spacing_) node->addModifier(Modifier::numeric(ModifierType::Spacing, *spacing_));
    for (const auto& child : children_) node->addChild(child);
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// LazyColumnBuilder
// ═══════════════════════════════════════════════════════════════════════════════

LazyColumnBuilder::LazyColumnBuilder() = default;

LazyColumnBuilder& LazyColumnBuilder::spacing(float s) { spacing_ = s; return *this; }
LazyColumnBuilder& LazyColumnBuilder::addItem(std::shared_ptr<UINode> item) { items_.push_back(std::move(item)); return *this; }

std::shared_ptr<UINode> LazyColumnBuilder::build() const {
    auto node = UINode::create(UINodeType::LazyColumn);
    if (spacing_) node->addModifier(Modifier::numeric(ModifierType::Spacing, *spacing_));
    for (const auto& item : items_) node->addChild(item);
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// LazyRowBuilder
// ═══════════════════════════════════════════════════════════════════════════════

LazyRowBuilder::LazyRowBuilder() = default;

LazyRowBuilder& LazyRowBuilder::spacing(float s) { spacing_ = s; return *this; }
LazyRowBuilder& LazyRowBuilder::addItem(std::shared_ptr<UINode> item) { items_.push_back(std::move(item)); return *this; }

std::shared_ptr<UINode> LazyRowBuilder::build() const {
    auto node = UINode::create(UINodeType::LazyRow);
    if (spacing_) node->addModifier(Modifier::numeric(ModifierType::Spacing, *spacing_));
    for (const auto& item : items_) node->addChild(item);
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// ScrollViewBuilder
// ═══════════════════════════════════════════════════════════════════════════════

ScrollViewBuilder::ScrollViewBuilder() = default;

ScrollViewBuilder& ScrollViewBuilder::horizontal(bool h) { horizontal_ = h; return *this; }
ScrollViewBuilder& ScrollViewBuilder::showsIndicators(bool show) { showsIndicators_ = show; return *this; }
ScrollViewBuilder& ScrollViewBuilder::addChild(std::shared_ptr<UINode> child) { children_.push_back(std::move(child)); return *this; }

std::shared_ptr<UINode> ScrollViewBuilder::build() const {
    auto node = UINode::create(UINodeType::ScrollView);
    for (const auto& child : children_) node->addChild(child);
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// SpacerBuilder
// ═══════════════════════════════════════════════════════════════════════════════

SpacerBuilder::SpacerBuilder() = default;

SpacerBuilder& SpacerBuilder::size(float s) { size_ = s; return *this; }

std::shared_ptr<UINode> SpacerBuilder::build() const {
    auto node = UINode::create(UINodeType::Spacer);
    if (size_) node->addModifier(Modifier::numeric(ModifierType::Height, *size_));
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// ContainerBuilder
// ═══════════════════════════════════════════════════════════════════════════════

ContainerBuilder::ContainerBuilder() = default;

ContainerBuilder& ContainerBuilder::backgroundColor(Color c) { backgroundColor_ = c; return *this; }
ContainerBuilder& ContainerBuilder::cornerRadius(float r) { cornerRadius_ = r; return *this; }
ContainerBuilder& ContainerBuilder::padding(EdgeInsets p) { padding_ = p; return *this; }
ContainerBuilder& ContainerBuilder::width(float w) { width_ = w; return *this; }
ContainerBuilder& ContainerBuilder::height(float h) { height_ = h; return *this; }
ContainerBuilder& ContainerBuilder::addChild(std::shared_ptr<UINode> child) { children_.push_back(std::move(child)); return *this; }

std::shared_ptr<UINode> ContainerBuilder::build() const {
    auto node = UINode::create(UINodeType::Container);
    if (backgroundColor_) node->addModifier(Modifier::color(ModifierType::BackgroundColor, *backgroundColor_));
    if (cornerRadius_) node->addModifier(Modifier::numeric(ModifierType::CornerRadius, *cornerRadius_));
    if (padding_) node->addModifier({ModifierType::Padding, *padding_, {}});
    if (width_) node->addModifier(Modifier::numeric(ModifierType::Width, *width_));
    if (height_) node->addModifier(Modifier::numeric(ModifierType::Height, *height_));
    for (const auto& child : children_) node->addChild(child);
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// دوال إنشاء سريعة
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<UINode> makeStack() { return StackBuilder().build(); }
std::shared_ptr<UINode> makeGrid(int columns) { return GridBuilder().columns(columns).build(); }
std::shared_ptr<UINode> makeLazyColumn() { return LazyColumnBuilder().build(); }
std::shared_ptr<UINode> makeLazyRow() { return LazyRowBuilder().build(); }
std::shared_ptr<UINode> makeScrollView(bool horizontal) { return ScrollViewBuilder().horizontal(horizontal).build(); }
std::shared_ptr<UINode> makeSpacer(float size) { return SpacerBuilder().size(size).build(); }
std::shared_ptr<UINode> makeContainer() { return ContainerBuilder().build(); }

} // namespace widgets
} // namespace ui
} // namespace sad
