/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: navigation_widgets.h
 * المسار: sad_ui/widgets/include/sad_ui/widgets/navigation_widgets.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * عناصر التنقل: NavigationView، TabView، Drawer، BottomSheet
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_WIDGETS_NAVIGATION_WIDGETS_H
#define SAD_UI_WIDGETS_NAVIGATION_WIDGETS_H

#include "sad_ui/node.h"
#include "sad_ui/style.h"

#include <string>
#include <memory>
#include <optional>
#include <vector>

namespace sad {
namespace ui {
namespace widgets {

// ═══════════════════════════════════════════════════════════════════════════════
// NavigationViewBuilder — حاوية التنقل الرئيسية
// ═══════════════════════════════════════════════════════════════════════════════

class NavigationViewBuilder {
public:
    NavigationViewBuilder();

    NavigationViewBuilder& title(const std::string& t);
    NavigationViewBuilder& addDestination(std::shared_ptr<UINode> dest);

    std::shared_ptr<UINode> build() const;

private:
    std::optional<std::string> title_;
    std::vector<std::shared_ptr<UINode>> destinations_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// TabViewBuilder — واجهة تبويبات
// ═══════════════════════════════════════════════════════════════════════════════

class TabViewBuilder {
public:
    TabViewBuilder();

    TabViewBuilder& selectedIndex(int index);
    TabViewBuilder& addTab(const std::string& title, const std::string& icon,
                           std::shared_ptr<UINode> content);
    TabViewBuilder& onTabChange(const std::string& expression);

    std::shared_ptr<UINode> build() const;

private:
    int selectedIndex_ = 0;
    struct TabInfo {
        std::string title;
        std::string icon;
        std::shared_ptr<UINode> content;
    };
    std::vector<TabInfo> tabs_;
    std::optional<std::string> onTabChangeExpr_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// DrawerBuilder — لوحة جانبية
// ═══════════════════════════════════════════════════════════════════════════════

class DrawerBuilder {
public:
    DrawerBuilder();

    DrawerBuilder& header(std::shared_ptr<UINode> headerNode);
    DrawerBuilder& addItem(const std::string& title, const std::string& icon,
                           const std::string& onTap = "");
    DrawerBuilder& backgroundColor(Color c);

    std::shared_ptr<UINode> build() const;

private:
    std::shared_ptr<UINode> header_;
    struct DrawerItem {
        std::string title;
        std::string icon;
        std::string onTap;
    };
    std::vector<DrawerItem> items_;
    std::optional<Color> backgroundColor_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// BottomSheetBuilder — لوحة سفلية تنزلق
// ═══════════════════════════════════════════════════════════════════════════════

class BottomSheetBuilder {
public:
    BottomSheetBuilder();

    BottomSheetBuilder& content(std::shared_ptr<UINode> c);
    BottomSheetBuilder& cornerRadius(float r);
    BottomSheetBuilder& backgroundColor(Color c);
    BottomSheetBuilder& onDismiss(const std::string& expression);

    std::shared_ptr<UINode> build() const;

private:
    std::shared_ptr<UINode> content_;
    std::optional<float> cornerRadius_;
    std::optional<Color> backgroundColor_;
    std::optional<std::string> onDismissExpr_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// دوال إنشاء سريعة
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<UINode> makeNavigationView();
std::shared_ptr<UINode> makeTabView();
std::shared_ptr<UINode> makeDrawer();
std::shared_ptr<UINode> makeBottomSheet();

} // namespace widgets
} // namespace ui
} // namespace sad

#endif // SAD_UI_WIDGETS_NAVIGATION_WIDGETS_H
