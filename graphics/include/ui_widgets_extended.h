/**
 * @file ui_widgets_extended.h
 * @brief 20 عنصر واجهة مستخدم متقدم
 * @brief (EN) 20 advanced UI widgets for desktop
 * 
 * @author كامل
 * @date أكتوبر 2026
 */

#ifndef SAD_UI_WIDGETS_EXTENDED_H
#define SAD_UI_WIDGETS_EXTENDED_H

#include &lt;string&gt;
#include &lt;vector&gt;
#include &lt;functional&gt;
#include &lt;memory&gt;

namespace Sad {
namespace UI {

//==============================================================================
// الأنواع الأساسية
//==============================================================================

struct Color {
    uint8_t r, g, b, a;
    Color() : r(0), g(0), b(0), a(255) {}
    Color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255) 
        : r(_r), g(_g), b(_b), a(_a) {}
    
    static Color أبيض() { return Color(255, 255, 255); }
    static Color أسود() { return Color(0, 0, 0); }
    static Color أحمر() { return Color(255, 0, 0); }
    static Color أخضر() { return Color(0, 255, 0); }
    static Color أزرق() { return Color(0, 0, 255); }
    static Color رمادي() { return Color(128, 128, 128); }
};

struct Rect {
    float x, y, width, height;
    Rect() : x(0), y(0), width(0), height(0) {}
    Rect(float _x, float _y, float _w, float _h) : x(_x), y(_y), width(_w), height(_h) {}
};

struct Padding {
    float top, right, bottom, left;
    Padding() : top(0), right(0), bottom(0), left(0) {}
    Padding(float all) : top(all), right(all), bottom(all), left(all) {}
    Padding(float v, float h) : top(v), right(h), bottom(v), left(h) {}
    Padding(float t, float r, float b, float l) : top(t), right(r), bottom(b), left(l) {}
};

enum class Alignment {
    START,      // بداية (يسار/أعلى)
    CENTER,     // وسط
    END,        // نهاية (يمين/أسفل)
    STRETCH     // تمدد
};

//==============================================================================
// الصنف الأساسي للعناصر
//==============================================================================

class Widget {
public:
    virtual ~Widget() = default;
    
    // الخصائص الأساسية
    std::string id;
    bool visible = true;
    bool enabled = true;
    Rect bounds;
    Padding padding;
    Color backgroundColor;
    
    // الأحداث
    std::function&lt;void()&gt; onClick;
    std::function&lt;void()&gt; onFocus;
    std::function&lt;void()&gt; onBlur;
    
    // الدوال الافتراضية
    virtual void render() = 0;
    virtual void update(float deltaTime) = 0;
    virtual bool handleEvent(const void* event) = 0;
    virtual Rect getMinSize() const = 0;
};

//==============================================================================
// 1. زر (Button)
//==============================================================================

class Button : public Widget {
public:
    std::string text;
    std::string icon;
    Color textColor;
    Color hoverColor;
    Color pressedColor;
    float cornerRadius = 4.0f;
    bool isPressed = false;
    bool isHovered = false;
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// 2. حقل نص (TextInput)
//==============================================================================

class TextInput : public Widget {
public:
    std::string text;
    std::string placeholder;
    std::string inputType; // "text", "password", "number", "email"
    int maxLength = -1;
    bool readOnly = false;
    bool multiline = false;
    int cursorPosition = 0;
    int selectionStart = -1;
    int selectionEnd = -1;
    
    std::function&lt;void(const std::string&amp;)&gt; onChange;
    std::function&lt;void()&gt; onSubmit;
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// 3. تسمية (Label)
//==============================================================================

class Label : public Widget {
public:
    std::string text;
    Color textColor;
    float fontSize = 14.0f;
    std::string fontFamily;
    bool bold = false;
    bool italic = false;
    bool underline = false;
    Alignment textAlign = Alignment::START;
    bool wrap = false;
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// 4. صندوق اختيار (Checkbox)
//==============================================================================

class Checkbox : public Widget {
public:
    std::string label;
    bool checked = false;
    bool indeterminate = false;
    Color checkColor;
    
    std::function&lt;void(bool)&gt; onToggle;
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// 5. زر راديو (RadioButton)
//==============================================================================

class RadioButton : public Widget {
public:
    std::string label;
    std::string groupName;
    bool selected = false;
    
    std::function&lt;void()&gt; onSelect;
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// 6. قائمة منسدلة (Dropdown)
//==============================================================================

class Dropdown : public Widget {
public:
    std::vector&lt;std::string&gt; items;
    int selectedIndex = -1;
    std::string placeholder;
    bool isOpen = false;
    int maxVisibleItems = 5;
    
    std::function&lt;void(int)&gt; onSelectionChanged;
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// 7. شريط تمرير (Slider)
//==============================================================================

class Slider : public Widget {
public:
    float value = 0.0f;
    float minValue = 0.0f;
    float maxValue = 100.0f;
    float step = 1.0f;
    bool showValue = true;
    bool vertical = false;
    Color trackColor;
    Color thumbColor;
    
    std::function&lt;void(float)&gt; onValueChanged;
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// 8. شريط تقدم (ProgressBar)
//==============================================================================

class ProgressBar : public Widget {
public:
    float value = 0.0f;
    float maxValue = 100.0f;
    bool indeterminate = false;
    bool showPercentage = true;
    Color progressColor;
    Color trackColor;
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// 9. قائمة (ListView)
//==============================================================================

class ListView : public Widget {
public:
    std::vector&lt;std::string&gt; items;
    int selectedIndex = -1;
    std::vector&lt;int&gt; selectedIndices; // للتحديد المتعدد
    bool multiSelect = false;
    float itemHeight = 32.0f;
    float scrollOffset = 0.0f;
    
    std::function&lt;void(int)&gt; onItemSelected;
    std::function&lt;void(int)&gt; onItemDoubleClicked;
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// 10. جدول (TableView)
//==============================================================================

struct TableColumn {
    std::string header;
    float width = 100.0f;
    bool sortable = true;
    bool resizable = true;
    Alignment align = Alignment::START;
};

class TableView : public Widget {
public:
    std::vector&lt;TableColumn&gt; columns;
    std::vector&lt;std::vector&lt;std::string&gt;&gt; rows;
    int selectedRow = -1;
    int sortColumn = -1;
    bool sortAscending = true;
    bool showHeaders = true;
    bool showGridLines = true;
    float rowHeight = 28.0f;
    
    std::function&lt;void(int, int)&gt; onCellClicked;
    std::function&lt;void(int)&gt; onRowSelected;
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// 11. شجرة (TreeView)
//==============================================================================

struct TreeNode {
    std::string text;
    std::string icon;
    bool expanded = false;
    bool selected = false;
    std::vector&lt;TreeNode&gt; children;
    void* userData = nullptr;
};

class TreeView : public Widget {
public:
    std::vector&lt;TreeNode&gt; nodes;
    float indentSize = 20.0f;
    bool showLines = true;
    bool showIcons = true;
    
    std::function&lt;void(TreeNode&amp;)&gt; onNodeSelected;
    std::function&lt;void(TreeNode&amp;)&gt; onNodeExpanded;
    std::function&lt;void(TreeNode&amp;)&gt; onNodeCollapsed;
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// 12. تبويبات (TabView)
//==============================================================================

struct Tab {
    std::string title;
    std::string icon;
    bool closable = false;
    std::shared_ptr&lt;Widget&gt; content;
};

class TabView : public Widget {
public:
    std::vector&lt;Tab&gt; tabs;
    int activeTab = 0;
    bool tabsOnTop = true;
    float tabHeight = 32.0f;
    
    std::function&lt;void(int)&gt; onTabChanged;
    std::function&lt;void(int)&gt; onTabClosed;
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// 13. شريط أدوات (Toolbar)
//==============================================================================

struct ToolbarItem {
    std::string id;
    std::string text;
    std::string icon;
    std::string tooltip;
    bool enabled = true;
    bool checked = false;
    bool separator = false;
};

class Toolbar : public Widget {
public:
    std::vector&lt;ToolbarItem&gt; items;
    float iconSize = 24.0f;
    bool showText = true;
    bool showIcons = true;
    bool vertical = false;
    
    std::function&lt;void(const std::string&amp;)&gt; onItemClicked;
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// 14. قائمة سياقية (ContextMenu)
//==============================================================================

struct MenuItem {
    std::string text;
    std::string shortcut;
    std::string icon;
    bool enabled = true;
    bool checked = false;
    bool separator = false;
    std::vector&lt;MenuItem&gt; submenu;
};

class ContextMenu : public Widget {
public:
    std::vector&lt;MenuItem&gt; items;
    float itemHeight = 24.0f;
    
    std::function&lt;void(const std::string&amp;)&gt; onItemSelected;
    
    void show(float x, float y);
    void hide();
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// 15. نافذة حوار (Dialog)
//==============================================================================

class Dialog : public Widget {
public:
    std::string title;
    bool modal = true;
    bool closable = true;
    bool resizable = false;
    bool draggable = true;
    std::shared_ptr&lt;Widget&gt; content;
    
    std::function&lt;void()&gt; onClose;
    std::function&lt;void()&gt; onOpen;
    
    void show();
    void close();
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// 16. منزلق (Splitter)
//==============================================================================

class Splitter : public Widget {
public:
    std::shared_ptr&lt;Widget&gt; firstPanel;
    std::shared_ptr&lt;Widget&gt; secondPanel;
    bool vertical = false;
    float splitPosition = 0.5f;
    float minSize = 50.0f;
    float splitterSize = 4.0f;
    bool dragging = false;
    
    std::function&lt;void(float)&gt; onSplitChanged;
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// 17. شريط تمرير (ScrollView)
//==============================================================================

class ScrollView : public Widget {
public:
    std::shared_ptr&lt;Widget&gt; content;
    float scrollX = 0.0f;
    float scrollY = 0.0f;
    bool showHorizontal = true;
    bool showVertical = true;
    float scrollbarWidth = 12.0f;
    
    void scrollTo(float x, float y);
    void scrollToWidget(Widget* widget);
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// 18. صورة (Image)
//==============================================================================

class ImageWidget : public Widget {
public:
    std::string source;
    void* imageData = nullptr;
    int imageWidth = 0;
    int imageHeight = 0;
    enum class ScaleMode { FIT, FILL, STRETCH, NONE } scaleMode = ScaleMode::FIT;
    
    bool loadFromFile(const std::string&amp; path);
    bool loadFromMemory(const void* data, size_t size);
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// 19. منتقي اللون (ColorPicker)
//==============================================================================

class ColorPicker : public Widget {
public:
    Color selectedColor;
    bool showAlpha = true;
    bool showPreview = true;
    std::vector&lt;Color&gt; recentColors;
    
    std::function&lt;void(Color)&gt; onColorChanged;
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// 20. منتقي التاريخ (DatePicker)
//==============================================================================

struct Date {
    int year, month, day;
    Date() : year(2026), month(1), day(1) {}
    Date(int y, int m, int d) : year(y), month(m), day(d) {}
};

class DatePicker : public Widget {
public:
    Date selectedDate;
    Date minDate;
    Date maxDate;
    bool showTime = false;
    int hour = 0, minute = 0;
    std::string format = "yyyy/mm/dd";
    
    std::function&lt;void(Date)&gt; onDateChanged;
    
    void render() override;
    void update(float deltaTime) override;
    bool handleEvent(const void* event) override;
    Rect getMinSize() const override;
};

//==============================================================================
// مصنع العناصر
//==============================================================================

namespace WidgetFactory {

std::shared_ptr&lt;Button&gt; زر(const std::string&amp; text);
std::shared_ptr&lt;TextInput&gt; حقل_نص(const std::string&amp; placeholder = "");
std::shared_ptr&lt;Label&gt; تسمية(const std::string&amp; text);
std::shared_ptr&lt;Checkbox&gt; صندوق_اختيار(const std::string&amp; label);
std::shared_ptr&lt;RadioButton&gt; زر_راديو(const std::string&amp; label, const std::string&amp; group);
std::shared_ptr&lt;Dropdown&gt; قائمة_منسدلة(const std::vector&lt;std::string&gt;&amp; items);
std::shared_ptr&lt;Slider&gt; شريط_تمرير(float min, float max, float value);
std::shared_ptr&lt;ProgressBar&gt; شريط_تقدم(float max = 100.0f);
std::shared_ptr&lt;ListView&gt; قائمة(const std::vector&lt;std::string&gt;&amp; items);
std::shared_ptr&lt;TableView&gt; جدول(const std::vector&lt;TableColumn&gt;&amp; columns);
std::shared_ptr&lt;TreeView&gt; شجرة(const std::vector&lt;TreeNode&gt;&amp; nodes);
std::shared_ptr&lt;TabView&gt; تبويبات();
std::shared_ptr&lt;Toolbar&gt; شريط_أدوات();
std::shared_ptr&lt;Dialog&gt; نافذة_حوار(const std::string&amp; title);
std::shared_ptr&lt;ScrollView&gt; منطقة_تمرير();
std::shared_ptr&lt;ImageWidget&gt; صورة(const std::string&amp; path);
std::shared_ptr&lt;ColorPicker&gt; منتقي_لون();
std::shared_ptr&lt;DatePicker&gt; منتقي_تاريخ();

} // namespace WidgetFactory

} // namespace UI
} // namespace Sad

#endif // SAD_UI_WIDGETS_EXTENDED_H
