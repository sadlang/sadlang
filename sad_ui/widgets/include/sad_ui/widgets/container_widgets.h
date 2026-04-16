/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: container_widgets.h
 * المسار: sad_ui/widgets/include/sad_ui/widgets/container_widgets.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * عناصر الحاويات: Card، Scaffold، AppBar
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_WIDGETS_CONTAINER_WIDGETS_H
#define SAD_UI_WIDGETS_CONTAINER_WIDGETS_H

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
// CardBuilder — بطاقة مع ظل وزوايا مستديرة
// ═══════════════════════════════════════════════════════════════════════════════

class CardBuilder {
public:
    CardBuilder();

    CardBuilder& elevation(float e);
    CardBuilder& cornerRadius(float r);
    CardBuilder& backgroundColor(Color c);
    CardBuilder& padding(EdgeInsets p);
    CardBuilder& addChild(std::shared_ptr<UINode> child);

    std::shared_ptr<UINode> build() const;

private:
    std::optional<float> elevation_;
    std::optional<float> cornerRadius_;
    std::optional<Color> backgroundColor_;
    std::optional<EdgeInsets> padding_;
    std::vector<std::shared_ptr<UINode>> children_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// ScaffoldBuilder — هيكل الشاشة (شريط علوي + محتوى + سفلي)
// ═══════════════════════════════════════════════════════════════════════════════

class ScaffoldBuilder {
public:
    ScaffoldBuilder();

    ScaffoldBuilder& topBar(std::shared_ptr<UINode> bar);
    ScaffoldBuilder& bottomBar(std::shared_ptr<UINode> bar);
    ScaffoldBuilder& fab(std::shared_ptr<UINode> fabNode);
    ScaffoldBuilder& drawer(std::shared_ptr<UINode> drawerNode);
    ScaffoldBuilder& body(std::shared_ptr<UINode> content);
    ScaffoldBuilder& backgroundColor(Color c);

    std::shared_ptr<UINode> build() const;

private:
    std::shared_ptr<UINode> topBar_;
    std::shared_ptr<UINode> bottomBar_;
    std::shared_ptr<UINode> fab_;
    std::shared_ptr<UINode> drawer_;
    std::shared_ptr<UINode> body_;
    std::optional<Color> backgroundColor_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// AppBarBuilder — شريط التطبيق العلوي
// ═══════════════════════════════════════════════════════════════════════════════

class AppBarBuilder {
public:
    explicit AppBarBuilder(const std::string& title = "");

    AppBarBuilder& title(const std::string& t);
    AppBarBuilder& backgroundColor(Color c);
    AppBarBuilder& leading(std::shared_ptr<UINode> widget);
    AppBarBuilder& addAction(std::shared_ptr<UINode> action);

    std::shared_ptr<UINode> build() const;

private:
    std::string title_;
    std::optional<Color> backgroundColor_;
    std::shared_ptr<UINode> leading_;
    std::vector<std::shared_ptr<UINode>> actions_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// دوال إنشاء سريعة
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<UINode> makeCard();
std::shared_ptr<UINode> makeScaffold();
std::shared_ptr<UINode> makeAppBar(const std::string& title = "");

} // namespace widgets
} // namespace ui
} // namespace sad

#endif // SAD_UI_WIDGETS_CONTAINER_WIDGETS_H
