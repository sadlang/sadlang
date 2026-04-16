/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: animation_widgets.cpp
 * المسار: sad_ui/widgets/animation/src/animation_widgets.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ مكونات الحركة: Skeleton، Shimmer، AnimatedList، Expandable، Collapsible، Swipeable
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/widgets/animation/animation_widgets.h"

namespace sad {
namespace ui {
namespace widgets {

// ── Skeleton ────────────────────────────────────────────────────────

SkeletonBuilder::SkeletonBuilder() = default;
SkeletonBuilder& SkeletonBuilder::width(float w) { width_ = w; return *this; }
SkeletonBuilder& SkeletonBuilder::height(float h) { height_ = h; return *this; }
SkeletonBuilder& SkeletonBuilder::borderRadius(float r) { borderRadius_ = r; return *this; }
SkeletonBuilder& SkeletonBuilder::animated(bool a) { animated_ = a; return *this; }

std::shared_ptr<UINode> SkeletonBuilder::build() const {
    auto node = UINode::create(UINodeType::Skeleton);
    if (width_) node->addModifier(Modifier::numeric(ModifierType::Width, *width_));
    if (height_) node->addModifier(Modifier::numeric(ModifierType::Height, *height_));
    node->addModifier(Modifier::numeric(ModifierType::BorderRadius, borderRadius_));
    return node;
}

// ── Shimmer ─────────────────────────────────────────────────────────

ShimmerBuilder::ShimmerBuilder() = default;
ShimmerBuilder& ShimmerBuilder::baseColor(Color c) { baseColor_ = c; return *this; }
ShimmerBuilder& ShimmerBuilder::highlightColor(Color c) { highlightColor_ = c; return *this; }
ShimmerBuilder& ShimmerBuilder::duration(float seconds) { duration_ = seconds; return *this; }

std::shared_ptr<UINode> ShimmerBuilder::build() const {
    auto node = UINode::create(UINodeType::Shimmer);
    if (baseColor_) node->addModifier(Modifier::color(ModifierType::BackgroundColor, *baseColor_));
    if (highlightColor_) node->addModifier(Modifier::color(ModifierType::TintColor, *highlightColor_));
    return node;
}

// ── AnimatedList ────────────────────────────────────────────────────

AnimatedListBuilder::AnimatedListBuilder() = default;
AnimatedListBuilder& AnimatedListBuilder::animationType(const std::string& type) { animationType_ = type; return *this; }
AnimatedListBuilder& AnimatedListBuilder::duration(float seconds) { duration_ = seconds; return *this; }

std::shared_ptr<UINode> AnimatedListBuilder::build() const {
    auto node = UINode::create(UINodeType::AnimatedList);
    return node;
}

// ── Expandable ──────────────────────────────────────────────────────

ExpandableBuilder::ExpandableBuilder(const std::string& title) : title_(title) {}
ExpandableBuilder& ExpandableBuilder::expanded(bool e) { expanded_ = e; return *this; }
ExpandableBuilder& ExpandableBuilder::iconPosition(const std::string& pos) { iconPosition_ = pos; return *this; }

std::shared_ptr<UINode> ExpandableBuilder::build() const {
    auto node = UINode::create(UINodeType::Expandable);
    node->setPrimaryArg(title_);
    return node;
}

// ── Collapsible ─────────────────────────────────────────────────────

CollapsibleBuilder::CollapsibleBuilder(const std::string& title) : title_(title) {}
CollapsibleBuilder& CollapsibleBuilder::collapsed(bool c) { collapsed_ = c; return *this; }
CollapsibleBuilder& CollapsibleBuilder::headerColor(Color c) { headerColor_ = c; return *this; }

std::shared_ptr<UINode> CollapsibleBuilder::build() const {
    auto node = UINode::create(UINodeType::Collapsible);
    node->setPrimaryArg(title_);
    if (headerColor_) node->addModifier(Modifier::color(ModifierType::BackgroundColor, *headerColor_));
    return node;
}

// ── Swipeable ───────────────────────────────────────────────────────

SwipeableBuilder::SwipeableBuilder() = default;
SwipeableBuilder& SwipeableBuilder::leftAction(const std::string& action) { leftAction_ = action; return *this; }
SwipeableBuilder& SwipeableBuilder::rightAction(const std::string& action) { rightAction_ = action; return *this; }
SwipeableBuilder& SwipeableBuilder::threshold(float t) { threshold_ = t; return *this; }

std::shared_ptr<UINode> SwipeableBuilder::build() const {
    auto node = UINode::create(UINodeType::Swipeable);
    if (leftAction_) node->addModifier(Modifier::event(ModifierType::OnDrag, *leftAction_));
    return node;
}

} // namespace widgets
} // namespace ui
} // namespace sad
