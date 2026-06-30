/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: nav_v2_widgets.h
 * المسار: features/graphics/widgets/navigation_v2/include/sad_ui/widgets/navigation_v2/nav_v2_widgets.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * مكونات التنقل المتقدمة: BottomNav، SideNav، Toolbar، SplitView، Tabs، MegaMenu
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_WIDGETS_NAV_V2_H
#define SAD_UI_WIDGETS_NAV_V2_H

#include "sad_ui/node.h"
#include "sad_ui/style.h"
#include <string>
#include <memory>
#include <optional>
#include <vector>

namespace sad {
namespace ui {
namespace widgets {

// ── BottomNav ───────────────────────────────────────────────────────

class BottomNavBuilder {
public:
    BottomNavBuilder();
    BottomNavBuilder& items(const std::vector<std::string>& labels);
    BottomNavBuilder& selectedIndex(int i);
    BottomNavBuilder& showLabels(bool s);
    BottomNavBuilder& bgColor(Color c);
    std::shared_ptr<UINode> build() const;
private:
    std::vector<std::string> items_;
    int selectedIndex_ = 0;
    bool showLabels_ = true;
    std::optional<Color> bgColor_;
};

// ── SideNav ─────────────────────────────────────────────────────────

class SideNavBuilder {
public:
    SideNavBuilder();
    SideNavBuilder& items(const std::vector<std::string>& labels);
    SideNavBuilder& selectedIndex(int i);
    SideNavBuilder& compact(bool c);
    SideNavBuilder& bgColor(Color c);
    std::shared_ptr<UINode> build() const;
private:
    std::vector<std::string> items_;
    int selectedIndex_ = 0;
    bool compact_ = false;
    std::optional<Color> bgColor_;
};

// ── Toolbar ─────────────────────────────────────────────────────────

class ToolbarBuilder {
public:
    explicit ToolbarBuilder(const std::string& title = "");
    ToolbarBuilder& bgColor(Color c);
    ToolbarBuilder& elevation(float e);
    std::shared_ptr<UINode> build() const;
private:
    std::string title_;
    std::optional<Color> bgColor_;
    float elevation_ = 4.0f;
};

// ── SplitView ───────────────────────────────────────────────────────

class SplitViewBuilder {
public:
    SplitViewBuilder();
    SplitViewBuilder& ratio(float r);
    SplitViewBuilder& orientation(const std::string& o);
    SplitViewBuilder& resizable(bool r);
    std::shared_ptr<UINode> build() const;
private:
    float ratio_ = 0.3f;
    std::string orientation_ = "horizontal";
    bool resizable_ = true;
};

// ── Tabs ────────────────────────────────────────────────────────────

class TabsBuilder {
public:
    TabsBuilder();
    TabsBuilder& labels(const std::vector<std::string>& lbls);
    TabsBuilder& selectedIndex(int i);
    TabsBuilder& scrollable(bool s);
    TabsBuilder& indicatorColor(Color c);
    std::shared_ptr<UINode> build() const;
private:
    std::vector<std::string> labels_;
    int selectedIndex_ = 0;
    bool scrollable_ = false;
    std::optional<Color> indicatorColor_;
};

// ── MegaMenu ────────────────────────────────────────────────────────

class MegaMenuBuilder {
public:
    MegaMenuBuilder();
    MegaMenuBuilder& columns(int n);
    MegaMenuBuilder& showIcons(bool s);
    MegaMenuBuilder& bgColor(Color c);
    std::shared_ptr<UINode> build() const;
private:
    int columns_ = 3;
    bool showIcons_ = true;
    std::optional<Color> bgColor_;
};

} // namespace widgets
} // namespace ui
} // namespace sad

#endif // SAD_UI_WIDGETS_NAV_V2_H
