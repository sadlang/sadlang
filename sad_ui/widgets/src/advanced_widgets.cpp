/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: advanced_widgets.cpp
 * المسار: sad_ui/widgets/src/advanced_widgets.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ عناصر متقدمة: Icon، Canvas، List، Avatar، Chip، FAB
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/widgets/advanced_widgets.h"

namespace sad {
namespace ui {
namespace widgets {

// ═══════════════════════════════════════════════════════════════════════════════
// IconBuilder
// ═══════════════════════════════════════════════════════════════════════════════

IconBuilder::IconBuilder(const std::string& name) : name_(name) {}

IconBuilder& IconBuilder::size(float s) { size_ = s; return *this; }
IconBuilder& IconBuilder::color(Color c) { color_ = c; return *this; }
IconBuilder& IconBuilder::onTap(const std::string& expression) { onTapExpr_ = expression; return *this; }

std::shared_ptr<UINode> IconBuilder::build() const {
    auto node = UINode::create(UINodeType::Icon);
    node->setPrimaryArg(name_);
    if (size_) node->addModifier(Modifier::numeric(ModifierType::FontSize, *size_));
    if (color_) node->addModifier(Modifier::color(ModifierType::ForegroundColor, *color_));
    if (onTapExpr_) node->addModifier(Modifier::event(ModifierType::OnTap, *onTapExpr_));
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// CanvasBuilder
// ═══════════════════════════════════════════════════════════════════════════════

CanvasBuilder::CanvasBuilder() = default;

CanvasBuilder& CanvasBuilder::width(float w) { width_ = w; return *this; }
CanvasBuilder& CanvasBuilder::height(float h) { height_ = h; return *this; }
CanvasBuilder& CanvasBuilder::backgroundColor(Color c) { backgroundColor_ = c; return *this; }
CanvasBuilder& CanvasBuilder::onDraw(const std::string& expression) { onDrawExpr_ = expression; return *this; }

std::shared_ptr<UINode> CanvasBuilder::build() const {
    auto node = UINode::create(UINodeType::Canvas);
    if (width_) node->addModifier(Modifier::numeric(ModifierType::Width, *width_));
    if (height_) node->addModifier(Modifier::numeric(ModifierType::Height, *height_));
    if (backgroundColor_) node->addModifier(Modifier::color(ModifierType::BackgroundColor, *backgroundColor_));
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// ListBuilder
// ═══════════════════════════════════════════════════════════════════════════════

ListBuilder::ListBuilder() = default;

ListBuilder& ListBuilder::addItem(std::shared_ptr<UINode> item) { items_.push_back(std::move(item)); return *this; }
ListBuilder& ListBuilder::addSection(const std::string& title, std::vector<std::shared_ptr<UINode>> items) {
    sections_.push_back({title, std::move(items)});
    return *this;
}
ListBuilder& ListBuilder::onItemTap(const std::string& expression) { onItemTapExpr_ = expression; return *this; }

std::shared_ptr<UINode> ListBuilder::build() const {
    auto node = UINode::create(UINodeType::List);
    // إضافة العناصر المباشرة
    for (const auto& item : items_) node->addChild(item);
    // إضافة الأقسام
    for (const auto& section : sections_) {
        auto sectionNode = UINode::create(UINodeType::Section);
        sectionNode->setPrimaryArg(section.title);
        for (const auto& item : section.items) sectionNode->addChild(item);
        node->addChild(sectionNode);
    }
    if (onItemTapExpr_) node->addModifier(Modifier::event(ModifierType::OnTap, *onItemTapExpr_));
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// AvatarBuilder
// ═══════════════════════════════════════════════════════════════════════════════

AvatarBuilder::AvatarBuilder(const std::string& source) : source_(source) {}

AvatarBuilder& AvatarBuilder::source(const std::string& src) { source_ = src; return *this; }
AvatarBuilder& AvatarBuilder::size(float s) { size_ = s; return *this; }
AvatarBuilder& AvatarBuilder::initials(const std::string& text) { initials_ = text; return *this; }
AvatarBuilder& AvatarBuilder::backgroundColor(Color c) { backgroundColor_ = c; return *this; }

std::shared_ptr<UINode> AvatarBuilder::build() const {
    auto node = UINode::create(UINodeType::Avatar);
    if (!source_.empty()) node->setPrimaryArg(source_);
    else if (initials_) node->setPrimaryArg(*initials_);
    if (size_) {
        node->addModifier(Modifier::numeric(ModifierType::Width, *size_));
        node->addModifier(Modifier::numeric(ModifierType::Height, *size_));
    }
    if (backgroundColor_) node->addModifier(Modifier::color(ModifierType::BackgroundColor, *backgroundColor_));
    node->addModifier(Modifier::numeric(ModifierType::CornerRadius, 999.0f)); // دائري
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// ChipBuilder
// ═══════════════════════════════════════════════════════════════════════════════

ChipBuilder::ChipBuilder(const std::string& label) : label_(label) {}

ChipBuilder& ChipBuilder::selected(bool s) { selected_ = s; return *this; }
ChipBuilder& ChipBuilder::icon(const std::string& iconName) { icon_ = iconName; return *this; }
ChipBuilder& ChipBuilder::backgroundColor(Color c) { backgroundColor_ = c; return *this; }
ChipBuilder& ChipBuilder::onTap(const std::string& expression) { onTapExpr_ = expression; return *this; }
ChipBuilder& ChipBuilder::onDelete(const std::string& expression) { onDeleteExpr_ = expression; return *this; }

std::shared_ptr<UINode> ChipBuilder::build() const {
    auto node = UINode::create(UINodeType::Chip);
    node->setPrimaryArg(label_);
    if (icon_) {
        auto iconNode = UINode::create(UINodeType::Icon);
        iconNode->setPrimaryArg(*icon_);
        node->addChild(iconNode);
    }
    if (backgroundColor_) node->addModifier(Modifier::color(ModifierType::BackgroundColor, *backgroundColor_));
    if (onTapExpr_) node->addModifier(Modifier::event(ModifierType::OnTap, *onTapExpr_));
    node->addModifier(Modifier::numeric(ModifierType::CornerRadius, 16.0f));
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// FABBuilder
// ═══════════════════════════════════════════════════════════════════════════════

FABBuilder::FABBuilder() = default;

FABBuilder& FABBuilder::icon(const std::string& iconName) { icon_ = iconName; return *this; }
FABBuilder& FABBuilder::label(const std::string& text) { label_ = text; return *this; }
FABBuilder& FABBuilder::backgroundColor(Color c) { backgroundColor_ = c; return *this; }
FABBuilder& FABBuilder::onTap(const std::string& expression) { onTapExpr_ = expression; return *this; }
FABBuilder& FABBuilder::extended(bool ext) { extended_ = ext; return *this; }

std::shared_ptr<UINode> FABBuilder::build() const {
    auto node = UINode::create(UINodeType::FAB);
    if (icon_) {
        auto iconNode = UINode::create(UINodeType::Icon);
        iconNode->setPrimaryArg(*icon_);
        node->addChild(iconNode);
    }
    if (label_) node->setPrimaryArg(*label_);
    if (backgroundColor_) node->addModifier(Modifier::color(ModifierType::BackgroundColor, *backgroundColor_));
    if (onTapExpr_) node->addModifier(Modifier::event(ModifierType::OnTap, *onTapExpr_));
    node->addModifier(Modifier::numeric(ModifierType::CornerRadius, 28.0f)); // دائري كبير
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// دوال إنشاء سريعة
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<UINode> makeIcon(const std::string& name) { return IconBuilder(name).build(); }
std::shared_ptr<UINode> makeCanvas(float width, float height) { return CanvasBuilder().width(width).height(height).build(); }
std::shared_ptr<UINode> makeList() { return ListBuilder().build(); }
std::shared_ptr<UINode> makeAvatar(const std::string& source) { return AvatarBuilder(source).build(); }
std::shared_ptr<UINode> makeChip(const std::string& label) { return ChipBuilder(label).build(); }
std::shared_ptr<UINode> makeFAB(const std::string& icon) { return FABBuilder().icon(icon).build(); }

} // namespace widgets
} // namespace ui
} // namespace sad
