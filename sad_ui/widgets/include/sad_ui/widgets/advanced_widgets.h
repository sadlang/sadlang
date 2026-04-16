/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: advanced_widgets.h
 * المسار: sad_ui/widgets/include/sad_ui/widgets/advanced_widgets.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * عناصر متقدمة: Icon، Canvas، List، Avatar، Chip، FAB
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_WIDGETS_ADVANCED_WIDGETS_H
#define SAD_UI_WIDGETS_ADVANCED_WIDGETS_H

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
// IconBuilder — أيقونة متجهة
// ═══════════════════════════════════════════════════════════════════════════════

class IconBuilder {
public:
    explicit IconBuilder(const std::string& name);

    IconBuilder& size(float s);
    IconBuilder& color(Color c);
    IconBuilder& onTap(const std::string& expression);

    std::shared_ptr<UINode> build() const;

private:
    std::string name_;
    std::optional<float> size_;
    std::optional<Color> color_;
    std::optional<std::string> onTapExpr_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// CanvasBuilder — لوحة رسم حر
// ═══════════════════════════════════════════════════════════════════════════════

class CanvasBuilder {
public:
    CanvasBuilder();

    CanvasBuilder& width(float w);
    CanvasBuilder& height(float h);
    CanvasBuilder& backgroundColor(Color c);
    CanvasBuilder& onDraw(const std::string& expression);

    std::shared_ptr<UINode> build() const;

private:
    std::optional<float> width_;
    std::optional<float> height_;
    std::optional<Color> backgroundColor_;
    std::optional<std::string> onDrawExpr_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// ListBuilder — قائمة عناصر قابلة للتمرير
// ═══════════════════════════════════════════════════════════════════════════════

class ListBuilder {
public:
    ListBuilder();

    ListBuilder& addItem(std::shared_ptr<UINode> item);
    ListBuilder& addSection(const std::string& title, std::vector<std::shared_ptr<UINode>> items);
    ListBuilder& onItemTap(const std::string& expression);

    std::shared_ptr<UINode> build() const;

private:
    std::vector<std::shared_ptr<UINode>> items_;
    struct SectionInfo {
        std::string title;
        std::vector<std::shared_ptr<UINode>> items;
    };
    std::vector<SectionInfo> sections_;
    std::optional<std::string> onItemTapExpr_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// AvatarBuilder — صورة رمزية دائرية
// ═══════════════════════════════════════════════════════════════════════════════

class AvatarBuilder {
public:
    explicit AvatarBuilder(const std::string& source = "");

    AvatarBuilder& source(const std::string& src);
    AvatarBuilder& size(float s);
    AvatarBuilder& initials(const std::string& text);
    AvatarBuilder& backgroundColor(Color c);

    std::shared_ptr<UINode> build() const;

private:
    std::string source_;
    std::optional<float> size_;
    std::optional<std::string> initials_;
    std::optional<Color> backgroundColor_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// ChipBuilder — رقاقة اختيار
// ═══════════════════════════════════════════════════════════════════════════════

class ChipBuilder {
public:
    explicit ChipBuilder(const std::string& label);

    ChipBuilder& selected(bool s);
    ChipBuilder& icon(const std::string& iconName);
    ChipBuilder& backgroundColor(Color c);
    ChipBuilder& onTap(const std::string& expression);
    ChipBuilder& onDelete(const std::string& expression);

    std::shared_ptr<UINode> build() const;

private:
    std::string label_;
    bool selected_ = false;
    std::optional<std::string> icon_;
    std::optional<Color> backgroundColor_;
    std::optional<std::string> onTapExpr_;
    std::optional<std::string> onDeleteExpr_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// FABBuilder — زر عائم (Floating Action Button)
// ═══════════════════════════════════════════════════════════════════════════════

class FABBuilder {
public:
    FABBuilder();

    FABBuilder& icon(const std::string& iconName);
    FABBuilder& label(const std::string& text);
    FABBuilder& backgroundColor(Color c);
    FABBuilder& onTap(const std::string& expression);
    FABBuilder& extended(bool ext);

    std::shared_ptr<UINode> build() const;

private:
    std::optional<std::string> icon_;
    std::optional<std::string> label_;
    std::optional<Color> backgroundColor_;
    std::optional<std::string> onTapExpr_;
    bool extended_ = false;
};

// ═══════════════════════════════════════════════════════════════════════════════
// دوال إنشاء سريعة
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<UINode> makeIcon(const std::string& name);
std::shared_ptr<UINode> makeCanvas(float width = 300, float height = 300);
std::shared_ptr<UINode> makeList();
std::shared_ptr<UINode> makeAvatar(const std::string& source = "");
std::shared_ptr<UINode> makeChip(const std::string& label);
std::shared_ptr<UINode> makeFAB(const std::string& icon = "");

} // namespace widgets
} // namespace ui
} // namespace sad

#endif // SAD_UI_WIDGETS_ADVANCED_WIDGETS_H
