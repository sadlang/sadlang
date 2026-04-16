/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: container_widgets.cpp
 * المسار: sad_ui/widgets/src/container_widgets.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ عناصر الحاويات: Card، Scaffold، AppBar
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/widgets/container_widgets.h"

namespace sad {
namespace ui {
namespace widgets {

// ═══════════════════════════════════════════════════════════════════════════════
// CardBuilder
// ═══════════════════════════════════════════════════════════════════════════════

CardBuilder::CardBuilder() = default;

CardBuilder& CardBuilder::elevation(float e) { elevation_ = e; return *this; }
CardBuilder& CardBuilder::cornerRadius(float r) { cornerRadius_ = r; return *this; }
CardBuilder& CardBuilder::backgroundColor(Color c) { backgroundColor_ = c; return *this; }
CardBuilder& CardBuilder::padding(EdgeInsets p) { padding_ = p; return *this; }
CardBuilder& CardBuilder::addChild(std::shared_ptr<UINode> child) { children_.push_back(std::move(child)); return *this; }

std::shared_ptr<UINode> CardBuilder::build() const {
    auto node = UINode::create(UINodeType::Card);
    if (elevation_) node->addModifier(Modifier::numeric(ModifierType::Shadow, *elevation_));
    if (cornerRadius_) node->addModifier(Modifier::numeric(ModifierType::CornerRadius, *cornerRadius_));
    if (backgroundColor_) node->addModifier(Modifier::color(ModifierType::BackgroundColor, *backgroundColor_));
    if (padding_) node->addModifier({ModifierType::Padding, *padding_, {}});
    for (const auto& child : children_) node->addChild(child);
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// ScaffoldBuilder
// ═══════════════════════════════════════════════════════════════════════════════

ScaffoldBuilder::ScaffoldBuilder() = default;

ScaffoldBuilder& ScaffoldBuilder::topBar(std::shared_ptr<UINode> bar) { topBar_ = std::move(bar); return *this; }
ScaffoldBuilder& ScaffoldBuilder::bottomBar(std::shared_ptr<UINode> bar) { bottomBar_ = std::move(bar); return *this; }
ScaffoldBuilder& ScaffoldBuilder::fab(std::shared_ptr<UINode> fabNode) { fab_ = std::move(fabNode); return *this; }
ScaffoldBuilder& ScaffoldBuilder::drawer(std::shared_ptr<UINode> drawerNode) { drawer_ = std::move(drawerNode); return *this; }
ScaffoldBuilder& ScaffoldBuilder::body(std::shared_ptr<UINode> content) { body_ = std::move(content); return *this; }
ScaffoldBuilder& ScaffoldBuilder::backgroundColor(Color c) { backgroundColor_ = c; return *this; }

std::shared_ptr<UINode> ScaffoldBuilder::build() const {
    auto node = UINode::create(UINodeType::Scaffold);
    if (backgroundColor_) node->addModifier(Modifier::color(ModifierType::BackgroundColor, *backgroundColor_));
    if (topBar_) node->addChild(topBar_);
    if (body_) node->addChild(body_);
    if (bottomBar_) node->addChild(bottomBar_);
    if (fab_) node->addChild(fab_);
    if (drawer_) node->addChild(drawer_);
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// AppBarBuilder
// ═══════════════════════════════════════════════════════════════════════════════

AppBarBuilder::AppBarBuilder(const std::string& title) : title_(title) {}

AppBarBuilder& AppBarBuilder::title(const std::string& t) { title_ = t; return *this; }
AppBarBuilder& AppBarBuilder::backgroundColor(Color c) { backgroundColor_ = c; return *this; }
AppBarBuilder& AppBarBuilder::leading(std::shared_ptr<UINode> widget) { leading_ = std::move(widget); return *this; }
AppBarBuilder& AppBarBuilder::addAction(std::shared_ptr<UINode> action) { actions_.push_back(std::move(action)); return *this; }

std::shared_ptr<UINode> AppBarBuilder::build() const {
    auto node = UINode::create(UINodeType::AppBar);
    node->setPrimaryArg(title_);
    if (backgroundColor_) node->addModifier(Modifier::color(ModifierType::BackgroundColor, *backgroundColor_));
    if (leading_) node->addChild(leading_);
    for (const auto& action : actions_) node->addChild(action);
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// دوال إنشاء سريعة
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<UINode> makeCard() { return CardBuilder().build(); }
std::shared_ptr<UINode> makeScaffold() { return ScaffoldBuilder().build(); }
std::shared_ptr<UINode> makeAppBar(const std::string& title) { return AppBarBuilder(title).build(); }

} // namespace widgets
} // namespace ui
} // namespace sad
