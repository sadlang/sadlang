// ==============================================================================
// ui_layout.h - نظام تخطيط واجهة المستخدم / UI Layout System
// ==============================================================================
// الوصف: نظام تخطيط مرن مستوحى من Flexbox و CSS Grid
// Description: Flexible layout system inspired by Flexbox and CSS Grid
// ==============================================================================
// المميزات / Features:
//   - تخطيط Flexbox كامل / Complete Flexbox layout
//   - تخطيط Grid / Grid layout
//   - عناصر واجهة أساسية / Basic UI widgets
//   - نظام أحداث / Event system
//   - تمرير / Scrolling
//   - تحجيم تلقائي / Auto-sizing
// ==============================================================================

#ifndef SAD_GRAPHICS_UI_LAYOUT_H
#define SAD_GRAPHICS_UI_LAYOUT_H

#include "../core/types.h"
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <unordered_map>

namespace sad {
namespace graphics {

// Renderer2D is defined in namespace SadGraphics (renderer2d.h)
// Bring it into sad::graphics so the rest of ui_layout can use it seamlessly.
} // close graphics temporarily
} // close sad temporarily

namespace SadGraphics { class Renderer2D; }

namespace sad {
namespace graphics {
using Renderer2D = ::SadGraphics::Renderer2D;

namespace ui {

// Global renderer for UI drawing (set before calling Draw on any element)
void SetUIRenderer(Renderer2D* renderer);
Renderer2D* GetUIRenderer();

// ==============================================================================
// التعدادات / Enumerations
// ==============================================================================

/// اتجاه التخطيط / Layout direction
enum class FlexDirection {
    Row,            // أفقي / Horizontal
    RowReverse,     // أفقي معكوس / Horizontal reversed
    Column,         // عمودي / Vertical
    ColumnReverse   // عمودي معكوس / Vertical reversed
};

/// التفاف العناصر / Item wrapping
enum class FlexWrap {
    NoWrap,     // بدون التفاف / No wrapping
    Wrap,       // التفاف / Wrap
    WrapReverse // التفاف معكوس / Wrap reversed
};

/// محاذاة المحور الرئيسي / Main axis alignment
enum class JustifyContent {
    FlexStart,      // البداية / Start
    FlexEnd,        // النهاية / End
    Center,         // المنتصف / Center
    SpaceBetween,   // مسافة بين / Space between
    SpaceAround,    // مسافة حول / Space around
    SpaceEvenly     // مسافة متساوية / Space evenly
};

/// محاذاة المحور الثانوي / Cross axis alignment
enum class AlignItems {
    FlexStart,  // البداية / Start
    FlexEnd,    // النهاية / End
    Center,     // المنتصف / Center
    Stretch,    // تمدد / Stretch
    Baseline    // خط الأساس / Baseline
};

/// محاذاة الذات / Self alignment
enum class AlignSelf {
    Auto,       // تلقائي / Auto
    FlexStart,
    FlexEnd,
    Center,
    Stretch,
    Baseline
};

/// محاذاة المحتوى / Content alignment (for wrapped items)
enum class AlignContent {
    FlexStart,
    FlexEnd,
    Center,
    Stretch,
    SpaceBetween,
    SpaceAround
};

/// وضع التموضع / Position mode
enum class PositionMode {
    Relative,   // نسبي / Relative
    Absolute    // مطلق / Absolute
};

/// وحدة القياس / Measurement unit
enum class Unit {
    Auto,       // تلقائي / Auto
    Pixels,     // بكسل / Pixels
    Percent,    // نسبة مئوية / Percentage
    Em,         // نسبة لحجم الخط / Relative to font size
    Vh,         // نسبة لارتفاع الشاشة / Viewport height
    Vw          // نسبة لعرض الشاشة / Viewport width
};

/// قيمة مع وحدة / Value with unit
struct StyleValue {
    Float32 value = 0.0f;
    Unit unit = Unit::Auto;
    
    StyleValue() = default;
    StyleValue(Float32 v, Unit u = Unit::Pixels) : value(v), unit(u) {}
    
    static StyleValue Auto() { return StyleValue(0, Unit::Auto); }
    static StyleValue Px(Float32 v) { return StyleValue(v, Unit::Pixels); }
    static StyleValue Pct(Float32 v) { return StyleValue(v, Unit::Percent); }
    static StyleValue Em(Float32 v) { return StyleValue(v, Unit::Em); }
    static StyleValue Vh(Float32 v) { return StyleValue(v, Unit::Vh); }
    static StyleValue Vw(Float32 v) { return StyleValue(v, Unit::Vw); }
    
    bool IsAuto() const { return unit == Unit::Auto; }
};

// ==============================================================================
// الهوامش والحشوات / Margins and Padding
// ==============================================================================

/// أربعة قيم (أعلى، يمين، أسفل، يسار) / Four values (top, right, bottom, left)
struct EdgeInsets {
    StyleValue top;
    StyleValue right;
    StyleValue bottom;
    StyleValue left;
    
    EdgeInsets() = default;
    
    /// جميع الجوانب متساوية / All sides equal
    static EdgeInsets All(Float32 value) {
        EdgeInsets e;
        e.top = e.right = e.bottom = e.left = StyleValue::Px(value);
        return e;
    }
    
    /// أفقي وعمودي / Horizontal and vertical
    static EdgeInsets Symmetric(Float32 horizontal, Float32 vertical) {
        EdgeInsets e;
        e.left = e.right = StyleValue::Px(horizontal);
        e.top = e.bottom = StyleValue::Px(vertical);
        return e;
    }
    
    /// قيم مختلفة / Different values
    static EdgeInsets TRBL(Float32 top, Float32 right, Float32 bottom, Float32 left) {
        EdgeInsets e;
        e.top = StyleValue::Px(top);
        e.right = StyleValue::Px(right);
        e.bottom = StyleValue::Px(bottom);
        e.left = StyleValue::Px(left);
        return e;
    }
};

// ==============================================================================
// نمط العنصر / Element Style
// ==============================================================================

/// نمط العنصر الكامل / Complete element style
struct LayoutStyle {
    // ============================================================================
    // الأبعاد / Dimensions
    // ============================================================================
    StyleValue width = StyleValue::Auto();
    StyleValue height = StyleValue::Auto();
    StyleValue minWidth = StyleValue::Px(0);
    StyleValue minHeight = StyleValue::Px(0);
    StyleValue maxWidth = StyleValue::Auto();
    StyleValue maxHeight = StyleValue::Auto();
    
    // ============================================================================
    // Flexbox - الحاوية / Container
    // ============================================================================
    FlexDirection flexDirection = FlexDirection::Row;
    FlexWrap flexWrap = FlexWrap::NoWrap;
    JustifyContent justifyContent = JustifyContent::FlexStart;
    AlignItems alignItems = AlignItems::Stretch;
    AlignContent alignContent = AlignContent::Stretch;
    Float32 gap = 0.0f;          // المسافة بين العناصر / Gap between items
    Float32 rowGap = 0.0f;       // المسافة بين الصفوف / Row gap
    Float32 columnGap = 0.0f;    // المسافة بين الأعمدة / Column gap
    
    // ============================================================================
    // Flexbox - العنصر / Item
    // ============================================================================
    Float32 flexGrow = 0.0f;     // معامل النمو / Grow factor
    Float32 flexShrink = 1.0f;   // معامل الانكماش / Shrink factor
    StyleValue flexBasis = StyleValue::Auto(); // الحجم الأساسي / Base size
    AlignSelf alignSelf = AlignSelf::Auto;
    Int32 order = 0;             // ترتيب العرض / Display order
    
    // ============================================================================
    // الهوامش والحشوات / Margins and Padding
    // ============================================================================
    EdgeInsets margin;
    EdgeInsets padding;
    
    // ============================================================================
    // التموضع / Positioning
    // ============================================================================
    PositionMode position = PositionMode::Relative;
    StyleValue top = StyleValue::Auto();
    StyleValue right = StyleValue::Auto();
    StyleValue bottom = StyleValue::Auto();
    StyleValue left = StyleValue::Auto();
    Int32 zIndex = 0;
    
    // ============================================================================
    // الحدود / Borders
    // ============================================================================
    Float32 borderWidth = 0.0f;
    Float32 borderRadius = 0.0f;
    Color borderColor = Color::Black;
    
    // ============================================================================
    // الخلفية / Background
    // ============================================================================
    Color backgroundColor = Color::Transparent;
    Float32 opacity = 1.0f;
    
    // ============================================================================
    // الرؤية / Visibility
    // ============================================================================
    bool visible = true;
    bool enabled = true;
    Float32 aspectRatio = 0.0f; // 0 = بدون نسبة / 0 = no aspect ratio
};

// ==============================================================================
// نتيجة التخطيط / Layout Result
// ==============================================================================

/// حدود العنصر المحسوبة / Computed element bounds
struct LayoutRect {
    Float32 x = 0.0f;
    Float32 y = 0.0f;
    Float32 width = 0.0f;
    Float32 height = 0.0f;
    
    Float32 Left() const { return x; }
    Float32 Top() const { return y; }
    Float32 Right() const { return x + width; }
    Float32 Bottom() const { return y + height; }
    Vec2 Position() const { return Vec2(x, y); }
    Vec2 Size() const { return Vec2(width, height); }
    Vec2 Center() const { return Vec2(x + width / 2, y + height / 2); }
    
    bool Contains(Float32 px, Float32 py) const {
        return px >= x && px < x + width && py >= y && py < y + height;
    }
    
    bool Contains(Vec2 point) const { return Contains(point.x, point.y); }
};

// ==============================================================================
// عنصر واجهة المستخدم / UI Element
// ==============================================================================

/// معرف العنصر / Element ID
using ElementId = UInt32;

/// عنصر الواجهة الأساسي / Base UI element
class Element : public std::enable_shared_from_this<Element> {
public:
    // ============================================================================
    // الإنشاء والتدمير / Construction and Destruction
    // ============================================================================
    
    Element();
    virtual ~Element() = default;
    
    // ============================================================================
    // الهوية / Identity
    // ============================================================================
    
    ElementId GetId() const { return m_id; }
    
    const std::string& GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
    
    const std::string& GetTag() const { return m_tag; }
    void SetTag(const std::string& tag) { m_tag = tag; }
    
    // ============================================================================
    // الهيكل / Structure
    // ============================================================================
    
    /// الأب / Parent
    Element* GetParent() const { return m_parent; }
    
    /// الأبناء / Children
    const std::vector<std::shared_ptr<Element>>& GetChildren() const { return m_children; }
    Int32 GetChildCount() const { return static_cast<Int32>(m_children.size()); }
    
    /// إضافة ابن / Add child
    void AddChild(std::shared_ptr<Element> child);
    
    /// إزالة ابن / Remove child
    void RemoveChild(Element* child);
    void RemoveChild(Int32 index);
    
    /// مسح الأبناء / Clear children
    void ClearChildren();
    
    /// البحث عن عنصر / Find element
    Element* FindById(ElementId id);
    Element* FindByName(const std::string& name);
    std::vector<Element*> FindByTag(const std::string& tag);
    
    // ============================================================================
    // النمط / Style
    // ============================================================================
    
    LayoutStyle& GetStyle() { return m_style; }
    const LayoutStyle& GetStyle() const { return m_style; }
    void SetStyle(const LayoutStyle& style) { m_style = style; MarkDirty(); }
    
    /// تطبيق نمط جزئي / Apply partial style
    Element& Width(Float32 value) { m_style.width = StyleValue::Px(value); MarkDirty(); return *this; }
    Element& Height(Float32 value) { m_style.height = StyleValue::Px(value); MarkDirty(); return *this; }
    Element& Size(Float32 w, Float32 h) { Width(w); Height(h); return *this; }
    Element& FlexGrow(Float32 value) { m_style.flexGrow = value; MarkDirty(); return *this; }
    Element& FlexShrink(Float32 value) { m_style.flexShrink = value; MarkDirty(); return *this; }
    Element& Margin(Float32 all) { m_style.margin = EdgeInsets::All(all); MarkDirty(); return *this; }
    Element& Padding(Float32 all) { m_style.padding = EdgeInsets::All(all); MarkDirty(); return *this; }
    Element& BackgroundColor(Color color) { m_style.backgroundColor = color; return *this; }
    Element& BorderRadius(Float32 radius) { m_style.borderRadius = radius; return *this; }
    
    // ============================================================================
    // التخطيط / Layout
    // ============================================================================
    
    /// الحدود المحسوبة / Computed bounds
    const LayoutRect& GetBounds() const { return m_bounds; }
    
    /// الحدود مع الحشوة / Content bounds (inside padding)
    LayoutRect GetContentBounds() const;
    
    /// تحديث التخطيط / Update layout
    void UpdateLayout();
    
    /// هل يحتاج تحديث / Needs update
    bool IsDirty() const { return m_dirty; }
    void MarkDirty();
    
    // ============================================================================
    // الرسم / Drawing
    // ============================================================================
    
    /// رسم العنصر / Draw element
    virtual void Draw();
    
    /// رسم الأبناء / Draw children
    void DrawChildren();
    
    // ============================================================================
    // الأحداث / Events
    // ============================================================================
    
    /// معالجات الأحداث / Event handlers
    std::function<void(Element*, Float32, Float32)> OnClick;
    std::function<void(Element*, Float32, Float32)> OnDoubleClick;
    std::function<void(Element*, Float32, Float32)> OnMouseEnter;
    std::function<void(Element*, Float32, Float32)> OnMouseLeave;
    std::function<void(Element*, Float32, Float32)> OnMouseMove;
    std::function<void(Element*, Float32, Float32)> OnMouseDown;
    std::function<void(Element*, Float32, Float32)> OnMouseUp;
    std::function<void(Element*, Float32, Float32)> OnDragStart;
    std::function<void(Element*, Float32, Float32)> OnDrag;
    std::function<void(Element*, Float32, Float32)> OnDragEnd;
    std::function<void(Element*)> OnFocus;
    std::function<void(Element*)> OnBlur;
    
    /// معالجة الأحداث / Process events
    virtual bool HandleMouseDown(Float32 x, Float32 y);
    virtual bool HandleMouseUp(Float32 x, Float32 y);
    virtual bool HandleMouseMove(Float32 x, Float32 y);
    virtual bool HandleClick(Float32 x, Float32 y);
    
    /// هل تحت المؤشر / Is hovered
    bool IsHovered() const { return m_hovered; }
    
    /// هل مضغوط / Is pressed
    bool IsPressed() const { return m_pressed; }
    
    /// هل له تركيز / Is focused
    bool IsFocused() const { return m_focused; }
    void SetFocused(bool focused);

protected:
    ElementId m_id;
    std::string m_name;
    std::string m_tag;
    
    Element* m_parent = nullptr;
    std::vector<std::shared_ptr<Element>> m_children;
    
    LayoutStyle m_style;
    LayoutRect m_bounds;
    bool m_dirty = true;
    
    bool m_hovered = false;
    bool m_pressed = false;
    bool m_focused = false;
    
    static ElementId s_nextId;
    
    friend class LayoutEngine;
};

// ==============================================================================
// الحاويات / Containers
// ==============================================================================

/// حاوية Flex / Flex container
class FlexContainer : public Element {
public:
    FlexContainer(FlexDirection direction = FlexDirection::Row);
    
    /// تعيين اتجاه Flex / Set flex direction
    FlexContainer& Direction(FlexDirection dir) { 
        m_style.flexDirection = dir; 
        MarkDirty(); 
        return *this; 
    }
    
    /// تعيين الالتفاف / Set wrap
    FlexContainer& Wrap(FlexWrap wrap) { 
        m_style.flexWrap = wrap; 
        MarkDirty(); 
        return *this; 
    }
    
    /// تعيين المحاذاة / Set alignment
    FlexContainer& Justify(JustifyContent justify) { 
        m_style.justifyContent = justify; 
        MarkDirty(); 
        return *this; 
    }
    
    FlexContainer& Align(AlignItems align) { 
        m_style.alignItems = align; 
        MarkDirty(); 
        return *this; 
    }
    
    FlexContainer& Gap(Float32 gap) { 
        m_style.gap = gap; 
        MarkDirty(); 
        return *this; 
    }
};

/// صف / Row (Flex with direction=Row)
class Row : public FlexContainer {
public:
    Row() : FlexContainer(FlexDirection::Row) {}
};

/// عمود / Column (Flex with direction=Column)
class Column : public FlexContainer {
public:
    Column() : FlexContainer(FlexDirection::Column) {}
};

/// حاوية قابلة للتمرير / Scrollable container
class ScrollView : public Element {
public:
    ScrollView();
    
    /// موقع التمرير / Scroll position
    Vec2 GetScrollOffset() const { return m_scrollOffset; }
    void SetScrollOffset(Vec2 offset);
    
    /// تمرير إلى / Scroll to
    void ScrollTo(Float32 x, Float32 y);
    void ScrollToElement(Element* element);
    
    /// إظهار أشرطة التمرير / Show scrollbars
    bool ShowHorizontalScrollbar = true;
    bool ShowVerticalScrollbar = true;
    
    void Draw() override;
    bool HandleMouseMove(Float32 x, Float32 y) override;

private:
    Vec2 m_scrollOffset = Vec2(0, 0);
    Vec2 m_contentSize = Vec2(0, 0);
};

/// مكدس (عناصر فوق بعض) / Stack (overlapping elements)
class Stack : public Element {
public:
    Stack();
};

// ==============================================================================
// عناصر واجهة المستخدم / UI Widgets
// ==============================================================================

/// نص / Text label
class Label : public Element {
public:
    Label(const std::string& text = "");
    
    const std::string& GetText() const { return m_text; }
    void SetText(const std::string& text) { m_text = text; }
    
    Color GetTextColor() const { return m_textColor; }
    void SetTextColor(Color color) { m_textColor = color; }
    
    Float32 GetFontSize() const { return m_fontSize; }
    void SetFontSize(Float32 size) { m_fontSize = size; MarkDirty(); }
    
    void Draw() override;

private:
    std::string m_text;
    Color m_textColor = Color::White;
    Float32 m_fontSize = 16.0f;
};

/// زر / Button
class Button : public Element {
public:
    Button(const std::string& text = "");
    
    const std::string& GetText() const { return m_text; }
    void SetText(const std::string& text) { m_text = text; }
    
    void Draw() override;
    bool HandleClick(Float32 x, Float32 y) override;
    
    /// ألوان الحالات / State colors
    Color NormalColor = Color(0.3f, 0.3f, 0.3f, 1.0f);
    Color HoverColor = Color(0.4f, 0.4f, 0.4f, 1.0f);
    Color PressedColor = Color(0.2f, 0.2f, 0.2f, 1.0f);
    Color DisabledColor = Color(0.2f, 0.2f, 0.2f, 0.5f);
    Color TextColor = Color::White;

private:
    std::string m_text;
};

/// صورة / Image
class Image : public Element {
public:
    Image();
    
    void SetTexture(UInt32 textureId) { m_textureId = textureId; }
    UInt32 GetTexture() const { return m_textureId; }
    
    void SetTint(Color tint) { m_tint = tint; }
    Color GetTint() const { return m_tint; }
    
    void Draw() override;

private:
    UInt32 m_textureId = 0;
    Color m_tint = Color::White;
};

/// شريط تقدم / Progress bar
class ProgressBar : public Element {
public:
    ProgressBar();
    
    Float32 GetProgress() const { return m_progress; }
    void SetProgress(Float32 value) { m_progress = std::max(0.0f, std::min(1.0f, value)); }
    
    Color GetFillColor() const { return m_fillColor; }
    void SetFillColor(Color color) { m_fillColor = color; }
    
    Color GetBackgroundColor() const { return m_backgroundColor; }
    void SetBackgroundColor(Color color) { m_backgroundColor = color; }
    
    void Draw() override;

private:
    Float32 m_progress = 0.0f;
    Color m_fillColor = Color(0.2f, 0.6f, 1.0f, 1.0f);
    Color m_backgroundColor = Color(0.2f, 0.2f, 0.2f, 1.0f);
};

/// منزلق / Slider
class Slider : public Element {
public:
    Slider();
    
    Float32 GetValue() const { return m_value; }
    void SetValue(Float32 value);
    
    Float32 GetMin() const { return m_min; }
    void SetMin(Float32 min) { m_min = min; }
    
    Float32 GetMax() const { return m_max; }
    void SetMax(Float32 max) { m_max = max; }
    
    Float32 GetStep() const { return m_step; }
    void SetStep(Float32 step) { m_step = step; }
    
    std::function<void(Float32)> OnValueChanged;
    
    void Draw() override;
    bool HandleMouseDown(Float32 x, Float32 y) override;
    bool HandleMouseMove(Float32 x, Float32 y) override;

private:
    Float32 m_value = 0.5f;
    Float32 m_min = 0.0f;
    Float32 m_max = 1.0f;
    Float32 m_step = 0.0f;
    bool m_dragging = false;
    
    void UpdateValueFromMouse(Float32 x);
};

/// مربع اختيار / Checkbox
class Checkbox : public Element {
public:
    Checkbox(const std::string& label = "");
    
    bool IsChecked() const { return m_checked; }
    void SetChecked(bool checked);
    
    const std::string& GetLabel() const { return m_label; }
    void SetLabel(const std::string& label) { m_label = label; MarkDirty(); }
    
    std::function<void(bool)> OnValueChanged;
    
    void Draw() override;
    bool HandleClick(Float32 x, Float32 y) override;

private:
    bool m_checked = false;
    std::string m_label;
};

/// زر اختيار / Radio button
class RadioButton : public Element {
public:
    RadioButton(const std::string& label = "", const std::string& group = "");
    
    bool IsSelected() const { return m_selected; }
    void SetSelected(bool selected);
    
    const std::string& GetLabel() const { return m_label; }
    const std::string& GetGroup() const { return m_group; }
    
    std::function<void()> OnSelected;
    
    void Draw() override;
    bool HandleClick(Float32 x, Float32 y) override;

private:
    bool m_selected = false;
    std::string m_label;
    std::string m_group;
    
    static std::unordered_map<std::string, std::vector<RadioButton*>> s_groups;
};

/// حقل نص / Text input
class TextInput : public Element {
public:
    TextInput();
    
    const std::string& GetText() const { return m_text; }
    void SetText(const std::string& text);
    
    const std::string& GetPlaceholder() const { return m_placeholder; }
    void SetPlaceholder(const std::string& placeholder) { m_placeholder = placeholder; }
    
    std::function<void(const std::string&)> OnTextChanged;
    std::function<void(const std::string&)> OnSubmit;
    
    void Draw() override;
    bool HandleClick(Float32 x, Float32 y) override;

private:
    std::string m_text;
    std::string m_placeholder;
    Int32 m_cursorPosition = 0;
    bool m_showCursor = true;
    Float32 m_cursorBlinkTimer = 0.0f;
};

// ==============================================================================
// محرك التخطيط / Layout Engine
// ==============================================================================

/// محرك التخطيط / Layout engine
class LayoutEngine {
public:
    // ============================================================================
    // التهيئة / Initialization
    // ============================================================================
    
    LayoutEngine();
    ~LayoutEngine();
    
    /// تهيئة / Initialize
    bool Initialize(Int32 viewportWidth, Int32 viewportHeight);
    
    /// تغيير حجم العرض / Resize viewport
    void Resize(Int32 width, Int32 height);
    
    // ============================================================================
    // الجذر / Root
    // ============================================================================
    
    /// العنصر الجذر / Root element
    Element* GetRoot() const { return m_root.get(); }
    void SetRoot(std::shared_ptr<Element> root);
    
    // ============================================================================
    // التحديث والرسم / Update and Draw
    // ============================================================================
    
    /// حساب التخطيط / Calculate layout
    void CalculateLayout();
    
    /// رسم الواجهة / Draw UI
    void Draw();
    
    /// تحديث / Update
    void Update(Float32 deltaTime);
    
    // ============================================================================
    // الأحداث / Events
    // ============================================================================
    
    /// معالجة أحداث الفأرة / Handle mouse events
    bool HandleMouseMove(Float32 x, Float32 y);
    bool HandleMouseDown(Float32 x, Float32 y);
    bool HandleMouseUp(Float32 x, Float32 y);
    bool HandleMouseClick(Float32 x, Float32 y);
    
    /// معالجة أحداث لوحة المفاتيح / Handle keyboard events
    bool HandleKeyDown(Int32 keyCode);
    bool HandleKeyUp(Int32 keyCode);
    bool HandleTextInput(const std::string& text);
    
    // ============================================================================
    // البحث / Lookup
    // ============================================================================
    
    /// العنصر تحت النقطة / Element at point
    Element* ElementAtPoint(Float32 x, Float32 y);
    
    /// العنصر ذو التركيز / Focused element
    Element* GetFocusedElement() const { return m_focusedElement; }
    void SetFocusedElement(Element* element);

private:
    std::shared_ptr<Element> m_root;
    Int32 m_viewportWidth = 0;
    Int32 m_viewportHeight = 0;
    
    Element* m_focusedElement = nullptr;
    Element* m_hoveredElement = nullptr;
    Element* m_pressedElement = nullptr;
    
    // حساب التخطيط التكراري / Recursive layout calculation
    void CalculateFlexLayout(Element* element, Float32 availableWidth, Float32 availableHeight);
    Float32 ResolveValue(StyleValue value, Float32 parentSize, Float32 fontSize = 16.0f);
};

// ==============================================================================
// دوال مساعدة للبناء / Builder Helper Functions
// ==============================================================================

/// إنشاء صف / Create row
inline std::shared_ptr<Row> MakeRow() { return std::make_shared<Row>(); }

/// إنشاء عمود / Create column
inline std::shared_ptr<Column> MakeColumn() { return std::make_shared<Column>(); }

/// إنشاء نص / Create label
inline std::shared_ptr<Label> MakeLabel(const std::string& text) { 
    return std::make_shared<Label>(text); 
}

/// إنشاء زر / Create button
inline std::shared_ptr<Button> MakeButton(const std::string& text) { 
    return std::make_shared<Button>(text); 
}

/// إنشاء صورة / Create image
inline std::shared_ptr<Image> MakeImage() { return std::make_shared<Image>(); }

/// إنشاء شريط تقدم / Create progress bar
inline std::shared_ptr<ProgressBar> MakeProgressBar() { return std::make_shared<ProgressBar>(); }

/// إنشاء منزلق / Create slider
inline std::shared_ptr<Slider> MakeSlider() { return std::make_shared<Slider>(); }

/// إنشاء مربع اختيار / Create checkbox
inline std::shared_ptr<Checkbox> MakeCheckbox(const std::string& label = "") {
    return std::make_shared<Checkbox>(label);
}

/// إنشاء حقل نص / Create text input
inline std::shared_ptr<TextInput> MakeTextInput() { return std::make_shared<TextInput>(); }

} // namespace ui
} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_UI_LAYOUT_H
