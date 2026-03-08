/**
 * =============================================================================
 * ملف: advanced_widgets.h
 * الوصف: 10 عناصر واجهة متقدمة
 * (AR) @brief TabBar, Grid, Dialog, Menu, Dropdown, ScrollView, Image,
 *            Tooltip, ProgressBar, Table
 * (EN) @brief 10 advanced UI widgets for Sad graphics system
 * المهمة: T307 - كامل: 10 widgets
 * =============================================================================
 */

#ifndef GRAPHICS_UI_ADVANCED_WIDGETS_H
#define GRAPHICS_UI_ADVANCED_WIDGETS_H

#include "widget.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>

namespace sad {
namespace graphics {
namespace ui {

// ============================================================================
// 1. TabBar — شريط التبويبات
// ============================================================================

struct TabInfo {
    std::string title;           ///< عنوان التبويب
    std::string icon;            ///< أيقونة (اختياري)
    bool closeable = false;      ///< قابل للإغلاق
};

class TabBar : public Widget {
public:
    TabBar();
    ~TabBar() override;

    int addTab(const std::string& title, const std::string& icon = "");
    void removeTab(int index);
    void setActiveTab(int index);
    int getActiveTab() const { return activeTab_; }
    int getTabCount() const { return static_cast<int>(tabs_.size()); }
    void setTabCloseable(int index, bool closeable);

    using TabChangeCallback = std::function<void(int oldTab, int newTab)>;
    using TabCloseCallback = std::function<void(int tabIndex)>;
    void onTabChanged(TabChangeCallback cb) { onTabChanged_ = cb; }
    void onTabClose(TabCloseCallback cb) { onTabClose_ = cb; }

    void Draw(SadGraphics::Renderer2D* renderer) override;
    bool OnMouseDown(int x, int y, int button) override;

private:
    std::vector<TabInfo> tabs_;
    int activeTab_ = 0;
    float tabWidth_ = 120.0f;
    float tabHeight_ = 36.0f;
    TabChangeCallback onTabChanged_;
    TabCloseCallback onTabClose_;
};

// ============================================================================
// 2. Grid — شبكة تخطيط
// ============================================================================

class Grid : public Widget {
public:
    Grid(int rows = 1, int cols = 1);
    ~Grid() override;

    void setGridSize(int rows, int cols);
    int getRows() const { return rows_; }
    int getCols() const { return cols_; }
    
    void setCell(int row, int col, std::shared_ptr<Widget> widget);
    Widget* getCell(int row, int col) const;
    void clearCell(int row, int col);

    void setSpacing(float hSpacing, float vSpacing);
    void setColumnWidth(int col, float width);
    void setRowHeight(int row, float height);

    void Draw(SadGraphics::Renderer2D* renderer) override;
    void Update(float deltaTime) override;

private:
    int rows_, cols_;
    float hSpacing_ = 4.0f, vSpacing_ = 4.0f;
    std::vector<std::vector<std::shared_ptr<Widget>>> cells_;
    std::vector<float> colWidths_;
    std::vector<float> rowHeights_;

    void recalculateLayout();
};

// ============================================================================
// 3. Dialog — نافذة حوار
// ============================================================================

enum class DialogResult { None, OK, Cancel, Yes, No };

class Dialog : public Widget {
public:
    Dialog(const std::string& title = "", const std::string& message = "");
    ~Dialog() override;

    void setTitle(const std::string& t) { title_ = t; }
    void setMessage(const std::string& m) { message_ = m; }
    void setModal(bool modal) { modal_ = modal; }
    
    enum class DialogType { OK, OKCancel, YesNo, YesNoCancel };
    void setType(DialogType type) { type_ = type; }

    using DialogCallback = std::function<void(DialogResult)>;
    void show(DialogCallback cb = nullptr);
    void close(DialogResult result = DialogResult::Cancel);
    bool isOpen() const { return open_; }

    void Draw(SadGraphics::Renderer2D* renderer) override;
    bool OnMouseDown(int x, int y, int button) override;
    bool OnKeyDown(int keyCode) override;

private:
    std::string title_;
    std::string message_;
    bool modal_ = true;
    bool open_ = false;
    DialogType type_ = DialogType::OK;
    DialogCallback callback_;

    Rect getTitleBarRect() const;
    Rect getButtonRect(int index) const;
    int getButtonCount() const;
    std::string getButtonLabel(int index) const;
    DialogResult getButtonResult(int index) const;
};

// ============================================================================
// 4. Menu — قائمة (سياقية أو شريطية)
// ============================================================================

struct MenuItem {
    std::string label;           ///< نص العنصر
    std::string shortcut;        ///< اختصار لوحة المفاتيح
    bool enabled = true;         ///< مفعّل
    bool isSeparator = false;    ///< فاصل
    std::function<void()> action; ///< الإجراء عند النقر
    std::vector<MenuItem> submenu; ///< قائمة فرعية
};

class Menu : public Widget {
public:
    Menu();
    ~Menu() override;

    void addItem(const std::string& label, std::function<void()> action,
                 const std::string& shortcut = "");
    void addSeparator();
    void addSubmenu(const std::string& label, const std::vector<MenuItem>& items);
    void clearItems();

    void showAt(float x, float y);
    void hide();
    bool isShown() const { return shown_; }

    void Draw(SadGraphics::Renderer2D* renderer) override;
    bool OnMouseDown(int x, int y, int button) override;
    bool OnMouseMove(int x, int y) override;

private:
    std::vector<MenuItem> items_;
    int hoveredIndex_ = -1;
    bool shown_ = false;
    float itemHeight_ = 28.0f;
};

// ============================================================================
// 5. Dropdown — قائمة منسدلة
// ============================================================================

class Dropdown : public Widget {
public:
    Dropdown();
    ~Dropdown() override;

    void addItem(const std::string& label);
    void addItems(const std::vector<std::string>& labels);
    void removeItem(int index);
    void clearItems();
    
    void setSelectedIndex(int index);
    int getSelectedIndex() const { return selectedIndex_; }
    std::string getSelectedText() const;
    int getItemCount() const { return static_cast<int>(items_.size()); }

    void setPlaceholder(const std::string& text) { placeholder_ = text; }

    using SelectCallback = std::function<void(int index, const std::string& text)>;
    void onSelect(SelectCallback cb) { onSelect_ = cb; }

    void Draw(SadGraphics::Renderer2D* renderer) override;
    bool OnMouseDown(int x, int y, int button) override;

private:
    std::vector<std::string> items_;
    int selectedIndex_ = -1;
    std::string placeholder_ = "اختر...";
    bool open_ = false;
    float itemHeight_ = 28.0f;
    int hoveredIndex_ = -1;
    SelectCallback onSelect_;
};

// ============================================================================
// 6. ScrollView — عرض قابل للتمرير
// ============================================================================

class ScrollView : public Widget {
public:
    ScrollView();
    ~ScrollView() override;

    void setContent(std::shared_ptr<Widget> content);
    Widget* getContent() const { return content_.get(); }
    
    void setScrollX(float x) { scrollX_ = x; }
    void setScrollY(float y) { scrollY_ = y; }
    float getScrollX() const { return scrollX_; }
    float getScrollY() const { return scrollY_; }
    
    void scrollTo(float x, float y);
    void scrollBy(float dx, float dy);
    
    void setShowHorizontalBar(bool show) { showHBar_ = show; }
    void setShowVerticalBar(bool show) { showVBar_ = show; }

    void Draw(SadGraphics::Renderer2D* renderer) override;
    bool OnMouseDown(int x, int y, int button) override;
    bool OnMouseMove(int x, int y) override;
    void Update(float deltaTime) override;

private:
    std::shared_ptr<Widget> content_;
    float scrollX_ = 0, scrollY_ = 0;
    float maxScrollX_ = 0, maxScrollY_ = 0;
    bool showHBar_ = false, showVBar_ = true;
    bool dragging_ = false;
    float dragStartY_ = 0;
    float scrollBarWidth_ = 12.0f;
};

// ============================================================================
// 7. ImageWidget — عنصر صورة
// ============================================================================

class ImageWidget : public Widget {
public:
    ImageWidget();
    ~ImageWidget() override;

    void setImagePath(const std::string& path) { imagePath_ = path; loadImage(); }
    const std::string& getImagePath() const { return imagePath_; }
    
    enum class ScaleMode { None, Fit, Fill, Stretch };
    void setScaleMode(ScaleMode mode) { scaleMode_ = mode; }
    
    void setTint(const Color& tint) { tint_ = tint; }

    void Draw(SadGraphics::Renderer2D* renderer) override;

private:
    std::string imagePath_;
    ScaleMode scaleMode_ = ScaleMode::Fit;
    Color tint_ = Color::White;
    unsigned int textureId_ = 0;
    int imageWidth_ = 0, imageHeight_ = 0;

    void loadImage();
};

// ============================================================================
// 8. Tooltip — تلميح
// ============================================================================

class Tooltip : public Widget {
public:
    Tooltip();
    ~Tooltip() override;

    void setText(const std::string& text) { text_ = text; }
    const std::string& getText() const { return text_; }
    
    void setDelay(float seconds) { delay_ = seconds; }
    void setTarget(Widget* target) { target_ = target; }

    void showAt(float x, float y);
    void hide();

    void Draw(SadGraphics::Renderer2D* renderer) override;
    void Update(float deltaTime) override;

private:
    std::string text_;
    float delay_ = 0.5f;
    float timer_ = 0;
    bool visible_ = false;
    Widget* target_ = nullptr;
};

// ============================================================================
// 9. ProgressBar — شريط تقدم
// ============================================================================

class ProgressBar : public Widget {
public:
    ProgressBar();
    ~ProgressBar() override;

    void setValue(float value);        ///< 0.0 - 1.0
    float getValue() const { return value_; }
    
    void setMinValue(float min) { minValue_ = min; }
    void setMaxValue(float max) { maxValue_ = max; }
    
    void setShowText(bool show) { showText_ = show; }
    void setIndeterminate(bool indeterminate) { indeterminate_ = indeterminate; }
    
    enum class Orientation { Horizontal, Vertical };
    void setOrientation(Orientation o) { orientation_ = o; }
    
    void setBarColor(const Color& c) { barColor_ = c; }

    void Draw(SadGraphics::Renderer2D* renderer) override;
    void Update(float deltaTime) override;

private:
    float value_ = 0.0f;
    float minValue_ = 0.0f, maxValue_ = 1.0f;
    bool showText_ = true;
    bool indeterminate_ = false;
    float animPhase_ = 0.0f;
    Orientation orientation_ = Orientation::Horizontal;
    Color barColor_ = Color(0.2f, 0.6f, 1.0f, 1.0f);
};

// ============================================================================
// 10. Table — جدول بيانات
// ============================================================================

struct TableColumn {
    std::string header;          ///< عنوان العمود
    float width = 100.0f;       ///< عرض العمود
    bool sortable = true;       ///< قابل للترتيب
    bool resizable = true;      ///< قابل لتغيير الحجم
};

class Table : public Widget {
public:
    Table();
    ~Table() override;

    // أعمدة
    void addColumn(const std::string& header, float width = 100.0f);
    void setColumns(const std::vector<TableColumn>& columns);
    int getColumnCount() const { return static_cast<int>(columns_.size()); }
    
    // صفوف
    void addRow(const std::vector<std::string>& cells);
    void setRow(int row, const std::vector<std::string>& cells);
    void removeRow(int row);
    void clearRows();
    int getRowCount() const { return static_cast<int>(rows_.size()); }
    
    std::string getCell(int row, int col) const;
    void setCell(int row, int col, const std::string& value);

    // تحديد
    void setSelectedRow(int row) { selectedRow_ = row; }
    int getSelectedRow() const { return selectedRow_; }

    // ترتيب
    void sortByColumn(int col, bool ascending = true);

    // أحداث
    using RowSelectCallback = std::function<void(int row)>;
    using CellClickCallback = std::function<void(int row, int col)>;
    void onRowSelect(RowSelectCallback cb) { onRowSelect_ = cb; }
    void onCellClick(CellClickCallback cb) { onCellClick_ = cb; }

    // رسم
    void Draw(SadGraphics::Renderer2D* renderer) override;
    bool OnMouseDown(int x, int y, int button) override;
    bool OnMouseMove(int x, int y) override;

private:
    std::vector<TableColumn> columns_;
    std::vector<std::vector<std::string>> rows_;
    int selectedRow_ = -1;
    int hoveredRow_ = -1;
    float headerHeight_ = 32.0f;
    float rowHeight_ = 28.0f;
    float scrollY_ = 0;
    RowSelectCallback onRowSelect_;
    CellClickCallback onCellClick_;
};

} // namespace ui
} // namespace graphics
} // namespace sad

#endif // GRAPHICS_UI_ADVANCED_WIDGETS_H
