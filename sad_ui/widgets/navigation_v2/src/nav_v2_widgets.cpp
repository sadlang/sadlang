/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: nav_v2_widgets.cpp
 * المسار: sad_ui/widgets/navigation_v2/src/nav_v2_widgets.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ مكونات التنقل المتقدمة: BottomNav، SideNav، Toolbar، SplitView، Tabs، MegaMenu
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/widgets/navigation_v2/nav_v2_widgets.h"

namespace sad {
namespace ui {
namespace widgets {

// ── BottomNav ───────────────────────────────────────────────────────

BottomNavBuilder::BottomNavBuilder() = default;
BottomNavBuilder& BottomNavBuilder::items(const std::vector<std::string>& labels) { items_ = labels; return *this; }
BottomNavBuilder& BottomNavBuilder::selectedIndex(int i) { selectedIndex_ = i; return *this; }
BottomNavBuilder& BottomNavBuilder::showLabels(bool s) { showLabels_ = s; return *this; }
BottomNavBuilder& BottomNavBuilder::bgColor(Color c) { bgColor_ = c; return *this; }

std::shared_ptr<UINode> BottomNavBuilder::build() const {
    auto node = UINode::create(UINodeType::BottomNav);
    for (const auto& item : items_) {
        auto child = UINode::create(UINodeType::Text);
        child->setPrimaryArg(item);
        node->addChild(child);
    }
    if (bgColor_) node->addModifier(Modifier::color(ModifierType::BackgroundColor, *bgColor_));
    return node;
}

// ── SideNav ─────────────────────────────────────────────────────────

SideNavBuilder::SideNavBuilder() = default;
SideNavBuilder& SideNavBuilder::items(const std::vector<std::string>& labels) { items_ = labels; return *this; }
SideNavBuilder& SideNavBuilder::selectedIndex(int i) { selectedIndex_ = i; return *this; }
SideNavBuilder& SideNavBuilder::compact(bool c) { compact_ = c; return *this; }
SideNavBuilder& SideNavBuilder::bgColor(Color c) { bgColor_ = c; return *this; }

std::shared_ptr<UINode> SideNavBuilder::build() const {
    auto node = UINode::create(UINodeType::SideNav);
    for (const auto& item : items_) {
        auto child = UINode::create(UINodeType::Text);
        child->setPrimaryArg(item);
        node->addChild(child);
    }
    if (bgColor_) node->addModifier(Modifier::color(ModifierType::BackgroundColor, *bgColor_));
    return node;
}

// ── Toolbar ─────────────────────────────────────────────────────────

ToolbarBuilder::ToolbarBuilder(const std::string& title) : title_(title) {}
ToolbarBuilder& ToolbarBuilder::bgColor(Color c) { bgColor_ = c; return *this; }
ToolbarBuilder& ToolbarBuilder::elevation(float e) { elevation_ = e; return *this; }

std::shared_ptr<UINode> ToolbarBuilder::build() const {
    auto node = UINode::create(UINodeType::Toolbar);
    node->setPrimaryArg(title_);
    if (bgColor_) node->addModifier(Modifier::color(ModifierType::BackgroundColor, *bgColor_));
    return node;
}

// ── SplitView ───────────────────────────────────────────────────────

SplitViewBuilder::SplitViewBuilder() = default;
SplitViewBuilder& SplitViewBuilder::ratio(float r) { ratio_ = r; return *this; }
SplitViewBuilder& SplitViewBuilder::orientation(const std::string& o) { orientation_ = o; return *this; }
SplitViewBuilder& SplitViewBuilder::resizable(bool r) { resizable_ = r; return *this; }

std::shared_ptr<UINode> SplitViewBuilder::build() const {
    auto node = UINode::create(UINodeType::SplitView);
    return node;
}

// ── Tabs ────────────────────────────────────────────────────────────

TabsBuilder::TabsBuilder() = default;
TabsBuilder& TabsBuilder::labels(const std::vector<std::string>& lbls) { labels_ = lbls; return *this; }
TabsBuilder& TabsBuilder::selectedIndex(int i) { selectedIndex_ = i; return *this; }
TabsBuilder& TabsBuilder::scrollable(bool s) { scrollable_ = s; return *this; }
TabsBuilder& TabsBuilder::indicatorColor(Color c) { indicatorColor_ = c; return *this; }

std::shared_ptr<UINode> TabsBuilder::build() const {
    auto node = UINode::create(UINodeType::Tabs);
    for (const auto& lbl : labels_) {
        auto child = UINode::create(UINodeType::Text);
        child->setPrimaryArg(lbl);
        node->addChild(child);
    }
    if (indicatorColor_) node->addModifier(Modifier::color(ModifierType::TintColor, *indicatorColor_));
    return node;
}

// ── MegaMenu ────────────────────────────────────────────────────────

MegaMenuBuilder::MegaMenuBuilder() = default;
MegaMenuBuilder& MegaMenuBuilder::columns(int n) { columns_ = n; return *this; }
MegaMenuBuilder& MegaMenuBuilder::showIcons(bool s) { showIcons_ = s; return *this; }
MegaMenuBuilder& MegaMenuBuilder::bgColor(Color c) { bgColor_ = c; return *this; }

std::shared_ptr<UINode> MegaMenuBuilder::build() const {
    auto node = UINode::create(UINodeType::MegaMenu);
    if (bgColor_) node->addModifier(Modifier::color(ModifierType::BackgroundColor, *bgColor_));
    return node;
}

} // namespace widgets
} // namespace ui
} // namespace sad
