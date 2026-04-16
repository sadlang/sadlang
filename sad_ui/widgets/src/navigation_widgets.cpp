/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: navigation_widgets.cpp
 * المسار: sad_ui/widgets/src/navigation_widgets.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ عناصر التنقل: NavigationView، TabView، Drawer، BottomSheet
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/widgets/navigation_widgets.h"

namespace sad {
namespace ui {
namespace widgets {

// ═══════════════════════════════════════════════════════════════════════════════
// NavigationViewBuilder
// ═══════════════════════════════════════════════════════════════════════════════

NavigationViewBuilder::NavigationViewBuilder() = default;

NavigationViewBuilder& NavigationViewBuilder::title(const std::string& t) { title_ = t; return *this; }
NavigationViewBuilder& NavigationViewBuilder::addDestination(std::shared_ptr<UINode> dest) { destinations_.push_back(std::move(dest)); return *this; }

std::shared_ptr<UINode> NavigationViewBuilder::build() const {
    auto node = UINode::create(UINodeType::NavigationView);
    if (title_) node->setPrimaryArg(*title_);
    for (const auto& dest : destinations_) node->addChild(dest);
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// TabViewBuilder
// ═══════════════════════════════════════════════════════════════════════════════

TabViewBuilder::TabViewBuilder() = default;

TabViewBuilder& TabViewBuilder::selectedIndex(int index) { selectedIndex_ = index; return *this; }
TabViewBuilder& TabViewBuilder::addTab(const std::string& title, const std::string& icon,
                                       std::shared_ptr<UINode> content) {
    tabs_.push_back({title, icon, std::move(content)});
    return *this;
}
TabViewBuilder& TabViewBuilder::onTabChange(const std::string& expression) { onTabChangeExpr_ = expression; return *this; }

std::shared_ptr<UINode> TabViewBuilder::build() const {
    auto node = UINode::create(UINodeType::TabView);
    for (const auto& tab : tabs_) {
        auto tabItem = UINode::create(UINodeType::TabItem);
        tabItem->setPrimaryArg(tab.title);
        if (tab.content) tabItem->addChild(tab.content);
        node->addChild(tabItem);
    }
    if (onTabChangeExpr_) node->addModifier(Modifier::event(ModifierType::OnValueChange, *onTabChangeExpr_));
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// DrawerBuilder
// ═══════════════════════════════════════════════════════════════════════════════

DrawerBuilder::DrawerBuilder() = default;

DrawerBuilder& DrawerBuilder::header(std::shared_ptr<UINode> headerNode) { header_ = std::move(headerNode); return *this; }
DrawerBuilder& DrawerBuilder::addItem(const std::string& title, const std::string& icon,
                                      const std::string& onTap) {
    items_.push_back({title, icon, onTap});
    return *this;
}
DrawerBuilder& DrawerBuilder::backgroundColor(Color c) { backgroundColor_ = c; return *this; }

std::shared_ptr<UINode> DrawerBuilder::build() const {
    auto node = UINode::create(UINodeType::Drawer);
    if (backgroundColor_) node->addModifier(Modifier::color(ModifierType::BackgroundColor, *backgroundColor_));
    if (header_) node->addChild(header_);
    for (const auto& item : items_) {
        auto row = UINode::create(UINodeType::Row);
        auto icon = UINode::create(UINodeType::Icon);
        icon->setPrimaryArg(item.icon);
        row->addChild(icon);
        auto text = UINode::create(UINodeType::Text);
        text->setPrimaryArg(item.title);
        row->addChild(text);
        if (!item.onTap.empty()) row->addModifier(Modifier::event(ModifierType::OnTap, item.onTap));
        node->addChild(row);
    }
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// BottomSheetBuilder
// ═══════════════════════════════════════════════════════════════════════════════

BottomSheetBuilder::BottomSheetBuilder() = default;

BottomSheetBuilder& BottomSheetBuilder::content(std::shared_ptr<UINode> c) { content_ = std::move(c); return *this; }
BottomSheetBuilder& BottomSheetBuilder::cornerRadius(float r) { cornerRadius_ = r; return *this; }
BottomSheetBuilder& BottomSheetBuilder::backgroundColor(Color c) { backgroundColor_ = c; return *this; }
BottomSheetBuilder& BottomSheetBuilder::onDismiss(const std::string& expression) { onDismissExpr_ = expression; return *this; }

std::shared_ptr<UINode> BottomSheetBuilder::build() const {
    auto node = UINode::create(UINodeType::BottomSheet);
    if (cornerRadius_) node->addModifier(Modifier::numeric(ModifierType::CornerRadius, *cornerRadius_));
    if (backgroundColor_) node->addModifier(Modifier::color(ModifierType::BackgroundColor, *backgroundColor_));
    if (onDismissExpr_) node->addModifier(Modifier::event(ModifierType::OnDisappear, *onDismissExpr_));
    if (content_) node->addChild(content_);
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// دوال إنشاء سريعة
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<UINode> makeNavigationView() { return NavigationViewBuilder().build(); }
std::shared_ptr<UINode> makeTabView() { return TabViewBuilder().build(); }
std::shared_ptr<UINode> makeDrawer() { return DrawerBuilder().build(); }
std::shared_ptr<UINode> makeBottomSheet() { return BottomSheetBuilder().build(); }

} // namespace widgets
} // namespace ui
} // namespace sad
