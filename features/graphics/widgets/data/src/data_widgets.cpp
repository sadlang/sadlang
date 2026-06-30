/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: data_widgets.cpp
 * المسار: features/graphics/widgets/data/src/data_widgets.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ مكونات البيانات: Tooltip، DataTable، TreeView، Breadcrumb، Pagination، Timeline
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/widgets/data/data_widgets.h"

namespace sad {
namespace ui {
namespace widgets {

// ── Tooltip ─────────────────────────────────────────────────────────

TooltipBuilder::TooltipBuilder(const std::string& message) : message_(message) {}
TooltipBuilder& TooltipBuilder::position(const std::string& pos) { position_ = pos; return *this; }
TooltipBuilder& TooltipBuilder::bgColor(Color c) { bgColor_ = c; return *this; }
TooltipBuilder& TooltipBuilder::textColor(Color c) { textColor_ = c; return *this; }

std::shared_ptr<UINode> TooltipBuilder::build() const {
    auto node = UINode::create(UINodeType::Tooltip);
    node->setPrimaryArg(message_);
    if (bgColor_) node->addModifier(Modifier::color(ModifierType::BackgroundColor, *bgColor_));
    if (textColor_) node->addModifier(Modifier::color(ModifierType::ForegroundColor, *textColor_));
    return node;
}

// ── DataTable ───────────────────────────────────────────────────────

DataTableBuilder::DataTableBuilder() = default;
DataTableBuilder& DataTableBuilder::columns(const std::vector<std::string>& cols) { columns_ = cols; return *this; }
DataTableBuilder& DataTableBuilder::sortable(bool s) { sortable_ = s; return *this; }
DataTableBuilder& DataTableBuilder::striped(bool s) { striped_ = s; return *this; }
DataTableBuilder& DataTableBuilder::bordered(bool b) { bordered_ = b; return *this; }

std::shared_ptr<UINode> DataTableBuilder::build() const {
    auto node = UINode::create(UINodeType::DataTable);
    for (const auto& col : columns_) {
        auto header = UINode::create(UINodeType::Text);
        header->setPrimaryArg(col);
        node->addChild(header);
    }
    return node;
}

// ── TreeView ────────────────────────────────────────────────────────

TreeViewBuilder::TreeViewBuilder() = default;
TreeViewBuilder& TreeViewBuilder::expandAll(bool e) { expandAll_ = e; return *this; }
TreeViewBuilder& TreeViewBuilder::showIcons(bool s) { showIcons_ = s; return *this; }
TreeViewBuilder& TreeViewBuilder::selectable(bool s) { selectable_ = s; return *this; }

std::shared_ptr<UINode> TreeViewBuilder::build() const {
    auto node = UINode::create(UINodeType::TreeView);
    return node;
}

// ── Breadcrumb ──────────────────────────────────────────────────────

BreadcrumbBuilder::BreadcrumbBuilder() = default;
BreadcrumbBuilder& BreadcrumbBuilder::items(const std::vector<std::string>& items) { items_ = items; return *this; }
BreadcrumbBuilder& BreadcrumbBuilder::separator(const std::string& sep) { separator_ = sep; return *this; }

std::shared_ptr<UINode> BreadcrumbBuilder::build() const {
    auto node = UINode::create(UINodeType::Breadcrumb);
    for (const auto& item : items_) {
        auto child = UINode::create(UINodeType::Text);
        child->setPrimaryArg(item);
        node->addChild(child);
    }
    return node;
}

// ── Pagination ──────────────────────────────────────────────────────

PaginationBuilder::PaginationBuilder() = default;
PaginationBuilder& PaginationBuilder::totalPages(int n) { totalPages_ = n; return *this; }
PaginationBuilder& PaginationBuilder::currentPage(int n) { currentPage_ = n; return *this; }
PaginationBuilder& PaginationBuilder::maxVisible(int n) { maxVisible_ = n; return *this; }

std::shared_ptr<UINode> PaginationBuilder::build() const {
    auto node = UINode::create(UINodeType::Pagination);
    return node;
}

// ── Timeline ────────────────────────────────────────────────────────

TimelineBuilder::TimelineBuilder() = default;
TimelineBuilder& TimelineBuilder::orientation(const std::string& o) { orientation_ = o; return *this; }
TimelineBuilder& TimelineBuilder::lineColor(Color c) { lineColor_ = c; return *this; }
TimelineBuilder& TimelineBuilder::dotSize(float s) { dotSize_ = s; return *this; }

std::shared_ptr<UINode> TimelineBuilder::build() const {
    auto node = UINode::create(UINodeType::Timeline);
    if (lineColor_) node->addModifier(Modifier::color(ModifierType::ForegroundColor, *lineColor_));
    return node;
}

} // namespace widgets
} // namespace ui
} // namespace sad
