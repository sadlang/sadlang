/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: layout_widgets.h
 * المسار: features/graphics/widgets/include/sad_ui/widgets/layout_widgets.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * عناصر التخطيط: Stack، Grid، LazyColumn، LazyRow، ScrollView، Spacer
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_WIDGETS_LAYOUT_WIDGETS_H
#define SAD_UI_WIDGETS_LAYOUT_WIDGETS_H

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
// StackBuilder — رصة عناصر فوق بعضها (ZStack)
// ═══════════════════════════════════════════════════════════════════════════════

class StackBuilder {
public:
    StackBuilder();

    StackBuilder& alignment(AlignmentType align);
    StackBuilder& addChild(std::shared_ptr<UINode> child);

    std::shared_ptr<UINode> build() const;

private:
    std::optional<AlignmentType> alignment_;
    std::vector<std::shared_ptr<UINode>> children_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// GridBuilder — شبكة بصفوف وأعمدة
// ═══════════════════════════════════════════════════════════════════════════════

class GridBuilder {
public:
    GridBuilder();

    GridBuilder& columns(int count);
    GridBuilder& spacing(float s);
    GridBuilder& addChild(std::shared_ptr<UINode> child);

    std::shared_ptr<UINode> build() const;

private:
    int columns_ = 2;
    std::optional<float> spacing_;
    std::vector<std::shared_ptr<UINode>> children_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// LazyColumnBuilder — عمود كسول (تحميل عند الحاجة)
// ═══════════════════════════════════════════════════════════════════════════════

class LazyColumnBuilder {
public:
    LazyColumnBuilder();

    LazyColumnBuilder& spacing(float s);
    LazyColumnBuilder& addItem(std::shared_ptr<UINode> item);

    std::shared_ptr<UINode> build() const;

private:
    std::optional<float> spacing_;
    std::vector<std::shared_ptr<UINode>> items_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// LazyRowBuilder — صف كسول (تحميل عند الحاجة)
// ═══════════════════════════════════════════════════════════════════════════════

class LazyRowBuilder {
public:
    LazyRowBuilder();

    LazyRowBuilder& spacing(float s);
    LazyRowBuilder& addItem(std::shared_ptr<UINode> item);

    std::shared_ptr<UINode> build() const;

private:
    std::optional<float> spacing_;
    std::vector<std::shared_ptr<UINode>> items_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// ScrollViewBuilder — حاوية قابلة للتمرير
// ═══════════════════════════════════════════════════════════════════════════════

class ScrollViewBuilder {
public:
    ScrollViewBuilder();

    ScrollViewBuilder& horizontal(bool h);
    ScrollViewBuilder& showsIndicators(bool show);
    ScrollViewBuilder& addChild(std::shared_ptr<UINode> child);

    std::shared_ptr<UINode> build() const;

private:
    bool horizontal_ = false;
    bool showsIndicators_ = true;
    std::vector<std::shared_ptr<UINode>> children_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// SpacerBuilder — فاصل مرن
// ═══════════════════════════════════════════════════════════════════════════════

class SpacerBuilder {
public:
    SpacerBuilder();

    SpacerBuilder& size(float s);

    std::shared_ptr<UINode> build() const;

private:
    std::optional<float> size_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// ContainerBuilder — حاوية عامة
// ═══════════════════════════════════════════════════════════════════════════════

class ContainerBuilder {
public:
    ContainerBuilder();

    ContainerBuilder& backgroundColor(Color c);
    ContainerBuilder& cornerRadius(float r);
    ContainerBuilder& padding(EdgeInsets p);
    ContainerBuilder& width(float w);
    ContainerBuilder& height(float h);
    ContainerBuilder& addChild(std::shared_ptr<UINode> child);

    std::shared_ptr<UINode> build() const;

private:
    std::optional<Color> backgroundColor_;
    std::optional<float> cornerRadius_;
    std::optional<EdgeInsets> padding_;
    std::optional<float> width_;
    std::optional<float> height_;
    std::vector<std::shared_ptr<UINode>> children_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// دوال إنشاء سريعة
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<UINode> makeStack();
std::shared_ptr<UINode> makeGrid(int columns = 2);
std::shared_ptr<UINode> makeLazyColumn();
std::shared_ptr<UINode> makeLazyRow();
std::shared_ptr<UINode> makeScrollView(bool horizontal = false);
std::shared_ptr<UINode> makeSpacer(float size = 8.0f);
std::shared_ptr<UINode> makeContainer();

} // namespace widgets
} // namespace ui
} // namespace sad

#endif // SAD_UI_WIDGETS_LAYOUT_WIDGETS_H
