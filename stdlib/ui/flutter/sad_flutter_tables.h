// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_tables.h — الجداول
//  Tables & DataTable System
// ═══════════════════════════════════════════════════════════════════════════════

#ifndef SAD_FLUTTER_TABLES_H
#define SAD_FLUTTER_TABLES_H

#include "sad_flutter_core.h"
#include "sad_flutter_widgets.h"

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  Table — جدول أساسي
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) حدود الجدول
 * @brief (EN) Table border
 */
struct TableBorder {
    BorderSide top;
    BorderSide right;
    BorderSide bottom;
    BorderSide left;
    BorderSide horizontalInside;
    BorderSide verticalInside;
    
    static TableBorder all(Color color = Color(0, 0, 0), double width = 1.0,
                           BorderStyle style = BorderStyle::Solid) {
        BorderSide side{color, width, style};
        return TableBorder{side, side, side, side, side, side};
    }
    
    static TableBorder symmetric(BorderSide inside = BorderSide{},
                                  BorderSide outside = BorderSide{}) {
        return TableBorder{outside, outside, outside, outside, inside, inside};
    }
};

using حدود_جدول = TableBorder;

/**
 * @brief (AR) عرض العمود
 * @brief (EN) Table column width
 */
class TableColumnWidth {
public:
    virtual ~TableColumnWidth() = default;
    virtual double minIntrinsicWidth(const std::vector<double>& cells, double maxWidth) const = 0;
    virtual double maxIntrinsicWidth(const std::vector<double>& cells, double maxWidth) const = 0;
};

using عرض_عمود_جدول = TableColumnWidth;

/**
 * @brief (AR) عرض ثابت
 * @brief (EN) Fixed column width
 */
class FixedColumnWidth : public TableColumnWidth {
public:
    double value;
    
    FixedColumnWidth(double v) : value(v) {}
    
    double minIntrinsicWidth(const std::vector<double>&, double) const override { return value; }
    double maxIntrinsicWidth(const std::vector<double>&, double) const override { return value; }
};

using عرض_عمود_ثابت = FixedColumnWidth;

/**
 * @brief (AR) عرض مرن
 * @brief (EN) Flex column width
 */
class FlexColumnWidth : public TableColumnWidth {
public:
    double flex;
    
    FlexColumnWidth(double f = 1.0) : flex(f) {}
    
    double minIntrinsicWidth(const std::vector<double>&, double) const override { return 0; }
    double maxIntrinsicWidth(const std::vector<double>&, double maxWidth) const override { 
        return maxWidth * flex; 
    }
};

using عرض_عمود_مرن = FlexColumnWidth;

/**
 * @brief (AR) عرض جوهري
 * @brief (EN) Intrinsic column width
 */
class IntrinsicColumnWidth : public TableColumnWidth {
public:
    double flex;
    
    IntrinsicColumnWidth(double f = 1.0) : flex(f) {}
    
    double minIntrinsicWidth(const std::vector<double>& cells, double) const override {
        double max = 0;
        for (double c : cells) max = std::max(max, c);
        return max;
    }
    
    double maxIntrinsicWidth(const std::vector<double>& cells, double) const override {
        double max = 0;
        for (double c : cells) max = std::max(max, c);
        return max;
    }
};

using عرض_عمود_جوهري = IntrinsicColumnWidth;

/**
 * @brief (AR) صف الجدول
 * @brief (EN) Table row
 */
struct TableRow {
    Key key;
    BoxDecoration decoration;
    std::vector<WidgetPtr> children;
    
    TableRow(std::vector<WidgetPtr> c, BoxDecoration d = BoxDecoration{})
        : children(std::move(c)), decoration(d) {}
};

using صف_جدول = TableRow;

/**
 * @brief (AR) محاذاة الخلية العمودية
 * @brief (EN) Table cell vertical alignment
 */
enum class TableCellVerticalAlignment {
    Top,
    Middle,
    Bottom,
    Baseline,
    Fill
};

using محاذاة_خلية_عمودية = TableCellVerticalAlignment;

/**
 * @brief (AR) الجدول الأساسي
 * @brief (EN) Table widget
 */
class Table : public StatelessWidget {
public:
    std::vector<TableRow> children;
    std::map<int, std::shared_ptr<TableColumnWidth>> columnWidths;
    std::shared_ptr<TableColumnWidth> defaultColumnWidth = std::make_shared<FlexColumnWidth>();
    TextDirection textDirection = TextDirection::RTL;
    TableBorder border;
    TableCellVerticalAlignment defaultVerticalAlignment = TableCellVerticalAlignment::Top;
    TextBaseline textBaseline = TextBaseline::Alphabetic;
    
    Table(std::vector<TableRow> rows = {}) : children(std::move(rows)) {}
    
    WidgetPtr build(BuildContext* ctx) override {
        // Build table layout
        return nullptr;  // Simplified
    }
};

using جدول = Table;

/**
 * @brief (AR) خلية الجدول
 * @brief (EN) Table cell
 */
class TableCell : public StatelessWidget {
public:
    TableCellVerticalAlignment verticalAlignment = TableCellVerticalAlignment::Middle;
    WidgetPtr child;
    
    TableCell(WidgetPtr c, TableCellVerticalAlignment va = TableCellVerticalAlignment::Middle)
        : child(c), verticalAlignment(va) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using خلية_جدول = TableCell;

// ═══════════════════════════════════════════════════════════════════════════════
//  DataTable — جدول البيانات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) عمود جدول البيانات
 * @brief (EN) Data column
 */
struct DataColumn {
    WidgetPtr label;
    std::string tooltip;
    bool numeric = false;
    std::function<void(int, bool)> onSort;
    
    DataColumn(WidgetPtr lbl, bool num = false, std::string tip = "")
        : label(lbl), numeric(num), tooltip(std::move(tip)) {}
};

using عمود_بيانات = DataColumn;

/**
 * @brief (AR) خلية جدول البيانات
 * @brief (EN) Data cell
 */
struct DataCell {
    WidgetPtr child;
    bool placeholder = false;
    bool showEditIcon = false;
    std::function<void()> onTap;
    std::function<void()> onLongPress;
    std::function<void(bool)> onTapDown;
    std::function<void()> onDoubleTap;
    std::function<void()> onTapCancel;
    
    DataCell(WidgetPtr c, bool p = false) : child(c), placeholder(p) {}
    
    static DataCell empty() {
        return DataCell(nullptr, true);
    }
};

using خلية_بيانات = DataCell;

/**
 * @brief (AR) صف جدول البيانات
 * @brief (EN) Data row
 */
struct DataRow {
    Key key;
    bool selected = false;
    std::function<void(bool?)> onSelectChanged;
    std::function<void()> onLongPress;
    Color color;
    std::vector<DataCell> cells;
    
    DataRow(std::vector<DataCell> c, bool sel = false)
        : cells(std::move(c)), selected(sel) {}
    
    DataRow& setSelected(bool s) { selected = s; return *this; }
    DataRow& setOnSelectChanged(std::function<void(bool?)> cb) { onSelectChanged = cb; return *this; }
    DataRow& setColor(Color c) { color = c; return *this; }
};

using صف_بيانات = DataRow;

/**
 * @brief (AR) جدول البيانات
 * @brief (EN) Data table widget
 */
class DataTable : public StatelessWidget {
public:
    std::vector<DataColumn> columns;
    std::vector<DataRow> rows;
    int sortColumnIndex = -1;
    bool sortAscending = true;
    std::function<void(bool?)> onSelectAll;
    BoxDecoration decoration;
    Color dataRowColor;
    double dataRowMinHeight = 48.0;
    double dataRowMaxHeight = 48.0;
    TextStyle dataTextStyle;
    Color headingRowColor;
    double headingRowHeight = 56.0;
    TextStyle headingTextStyle;
    double horizontalMargin = 24.0;
    double columnSpacing = 56.0;
    bool showCheckboxColumn = true;
    bool showBottomBorder = true;
    double dividerThickness = 1.0;
    double checkboxHorizontalMargin = 0.0;
    BorderRadius border;
    Clip clipBehavior = Clip::None;
    
    DataTable(std::vector<DataColumn> cols, std::vector<DataRow> r)
        : columns(std::move(cols)), rows(std::move(r)) {}
    
    WidgetPtr build(BuildContext* ctx) override {
        // Build data table
        return nullptr;  // Simplified
    }
};

using جدول_بيانات = DataTable;

// ═══════════════════════════════════════════════════════════════════════════════
//  PaginatedDataTable — جدول بيانات مع ترقيم
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) مصدر بيانات الجدول
 * @brief (EN) Data table source
 */
class DataTableSource {
public:
    virtual ~DataTableSource() = default;
    
    virtual DataRow getRow(int index) = 0;
    virtual int rowCount() const = 0;
    virtual bool isRowCountApproximate() const { return false; }
    virtual int selectedRowCount() const { return 0; }
    
    // Notifier
    std::function<void()> onChanged;
    void notifyListeners() { if (onChanged) onChanged(); }
};

using مصدر_جدول_بيانات = DataTableSource;

/**
 * @brief (AR) جدول بيانات مرقّم
 * @brief (EN) Paginated data table
 */
class PaginatedDataTable : public StatefulWidget {
public:
    WidgetPtr header;
    std::vector<WidgetPtr> actions;
    std::vector<DataColumn> columns;
    int sortColumnIndex = -1;
    bool sortAscending = true;
    std::function<void(bool?)> onSelectAll;
    double dataRowMinHeight = 48.0;
    double dataRowMaxHeight = 48.0;
    int initialFirstRowIndex = 0;
    std::function<void(int)> onPageChanged;
    int rowsPerPage = 10;
    std::vector<int> availableRowsPerPage = {10, 20, 50, 100};
    std::function<void(int?)> onRowsPerPageChanged;
    DragStartBehavior dragStartBehavior = DragStartBehavior::Start;
    Color arrowHeadColor;
    bool showCheckboxColumn = true;
    bool showFirstLastButtons = false;
    int checkboxHorizontalMargin = 0;
    std::shared_ptr<DataTableSource> source;
    
    std::unique_ptr<State_Base> createState() override;
};

using جدول_بيانات_مرقم = PaginatedDataTable;

class PaginatedDataTableState : public State<PaginatedDataTable> {
public:
    int currentPage_ = 0;
    int rowsPerPage_ = 10;
    
    void initState() override {
        auto* w = widget();
        rowsPerPage_ = w->rowsPerPage;
        currentPage_ = w->initialFirstRowIndex / rowsPerPage_;
    }
    
    void goToPage(int page) {
        currentPage_ = page;
        if (widget()->onPageChanged) {
            widget()->onPageChanged(currentPage_ * rowsPerPage_);
        }
        setState([]{});
    }
    
    void nextPage() {
        auto* w = widget();
        if (w->source && (currentPage_ + 1) * rowsPerPage_ < w->source->rowCount()) {
            goToPage(currentPage_ + 1);
        }
    }
    
    void previousPage() {
        if (currentPage_ > 0) {
            goToPage(currentPage_ - 1);
        }
    }
    
    void setRowsPerPage(int count) {
        rowsPerPage_ = count;
        currentPage_ = 0;
        if (widget()->onRowsPerPageChanged) {
            widget()->onRowsPerPageChanged(count);
        }
        setState([]{});
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        // Build paginated data table
        return nullptr;  // Simplified
    }
    
private:
    PaginatedDataTable* widget() const { return static_cast<PaginatedDataTable*>(widget_); }
};

inline std::unique_ptr<State_Base> PaginatedDataTable::createState() {
    return std::make_unique<PaginatedDataTableState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  DataGrid — شبكة بيانات (بديل محسن)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) تعريف عمود الشبكة
 * @brief (EN) Grid column definition
 */
struct GridColumn {
    std::string field;
    std::string headerText;
    double width = 100.0;
    double minWidth = 50.0;
    double maxWidth = 500.0;
    bool visible = true;
    bool sortable = true;
    bool resizable = true;
    bool editable = false;
    TextAlign textAlign = TextAlign::Start;
    std::function<WidgetPtr(BuildContext*, std::any)> cellBuilder;
    std::function<WidgetPtr(BuildContext*)> headerBuilder;
};

using عمود_شبكة = GridColumn;

/**
 * @brief (AR) شبكة البيانات
 * @brief (EN) Data grid
 */
class DataGrid : public StatefulWidget {
public:
    std::vector<GridColumn> columns;
    std::vector<std::map<std::string, std::any>> rows;
    bool showHeaderRow = true;
    bool showGridLines = true;
    Color gridLineColor = Color(200, 200, 200);
    double gridLineWidth = 1.0;
    double rowHeight = 48.0;
    double headerHeight = 56.0;
    bool allowSelection = true;
    bool allowMultiSelection = false;
    std::function<void(int)> onRowTap;
    std::function<void(int)> onRowDoubleTap;
    std::function<void(std::vector<int>)> onSelectionChanged;
    bool allowSorting = true;
    bool allowFiltering = true;
    bool allowReordering = true;
    bool allowResizing = true;
    std::shared_ptr<ScrollController> horizontalController;
    std::shared_ptr<ScrollController> verticalController;
    
    std::unique_ptr<State_Base> createState() override {
        return nullptr;  // Simplified
    }
};

using شبكة_بيانات = DataGrid;

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) إنشاء عمود نصي
 * @brief (EN) Create text column
 */
inline DataColumn انشئ_عمود_نصي(const std::string& نص, bool رقمي = false) {
    return DataColumn(std::make_shared<Text>(نص), رقمي);
}

/**
 * @brief (AR) إنشاء خلية نصية
 * @brief (EN) Create text cell
 */
inline DataCell انشئ_خلية_نصية(const std::string& نص) {
    return DataCell(std::make_shared<Text>(نص));
}

/**
 * @brief (AR) إنشاء صف بيانات
 * @brief (EN) Create data row
 */
inline DataRow انشئ_صف_بيانات(std::initializer_list<std::string> خلايا) {
    std::vector<DataCell> cells;
    for (const auto& text : خلايا) {
        cells.push_back(انشئ_خلية_نصية(text));
    }
    return DataRow(cells);
}

} // namespace flutter
} // namespace sad

#endif // SAD_FLUTTER_TABLES_H
