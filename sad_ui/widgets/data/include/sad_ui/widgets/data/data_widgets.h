/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: data_widgets.h
 * المسار: sad_ui/widgets/data/include/sad_ui/widgets/data/data_widgets.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * مكونات البيانات: Tooltip، DataTable، TreeView، Breadcrumb، Pagination، Timeline
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_WIDGETS_DATA_H
#define SAD_UI_WIDGETS_DATA_H

#include "sad_ui/node.h"
#include "sad_ui/style.h"
#include <string>
#include <memory>
#include <optional>
#include <vector>

namespace sad {
namespace ui {
namespace widgets {

// ── Tooltip ─────────────────────────────────────────────────────────

class TooltipBuilder {
public:
    explicit TooltipBuilder(const std::string& message = "");
    TooltipBuilder& position(const std::string& pos);
    TooltipBuilder& bgColor(Color c);
    TooltipBuilder& textColor(Color c);
    std::shared_ptr<UINode> build() const;
private:
    std::string message_;
    std::string position_ = "top";
    std::optional<Color> bgColor_;
    std::optional<Color> textColor_;
};

// ── DataTable ───────────────────────────────────────────────────────

class DataTableBuilder {
public:
    DataTableBuilder();
    DataTableBuilder& columns(const std::vector<std::string>& cols);
    DataTableBuilder& sortable(bool s);
    DataTableBuilder& striped(bool s);
    DataTableBuilder& bordered(bool b);
    std::shared_ptr<UINode> build() const;
private:
    std::vector<std::string> columns_;
    bool sortable_ = false;
    bool striped_ = false;
    bool bordered_ = true;
};

// ── TreeView ────────────────────────────────────────────────────────

class TreeViewBuilder {
public:
    TreeViewBuilder();
    TreeViewBuilder& expandAll(bool e);
    TreeViewBuilder& showIcons(bool s);
    TreeViewBuilder& selectable(bool s);
    std::shared_ptr<UINode> build() const;
private:
    bool expandAll_ = false;
    bool showIcons_ = true;
    bool selectable_ = true;
};

// ── Breadcrumb ──────────────────────────────────────────────────────

class BreadcrumbBuilder {
public:
    BreadcrumbBuilder();
    BreadcrumbBuilder& items(const std::vector<std::string>& items);
    BreadcrumbBuilder& separator(const std::string& sep);
    std::shared_ptr<UINode> build() const;
private:
    std::vector<std::string> items_;
    std::string separator_ = ">";
};

// ── Pagination ──────────────────────────────────────────────────────

class PaginationBuilder {
public:
    PaginationBuilder();
    PaginationBuilder& totalPages(int n);
    PaginationBuilder& currentPage(int n);
    PaginationBuilder& maxVisible(int n);
    std::shared_ptr<UINode> build() const;
private:
    int totalPages_ = 1;
    int currentPage_ = 1;
    int maxVisible_ = 5;
};

// ── Timeline ────────────────────────────────────────────────────────

class TimelineBuilder {
public:
    TimelineBuilder();
    TimelineBuilder& orientation(const std::string& o);
    TimelineBuilder& lineColor(Color c);
    TimelineBuilder& dotSize(float s);
    std::shared_ptr<UINode> build() const;
private:
    std::string orientation_ = "vertical";
    std::optional<Color> lineColor_;
    float dotSize_ = 12.0f;
};

} // namespace widgets
} // namespace ui
} // namespace sad

#endif // SAD_UI_WIDGETS_DATA_H
