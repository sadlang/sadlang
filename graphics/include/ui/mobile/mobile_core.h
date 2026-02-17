// mobile_core.h - المركبات الأساسية للتطبيقات الهاتفية
// Mobile Core Components
//
// الوصف: يحتوي على المركبات الأساسية لبناء واجهات المستخدم للهاتف
// Description: Core components for building mobile user interfaces
// مشابه لـ Flutter widgets الأساسية
//
// المركبات: Container, Row, Column, Stack, Padding, Center, Align, SizedBox,
//           Expanded, Flexible, Spacer, AspectRatio, FractionallySizedBox,
//           ConstrainedBox, FittedBox, Wrap, LayoutBuilder

#ifndef SAD_GRAPHICS_MOBILE_CORE_H
#define SAD_GRAPHICS_MOBILE_CORE_H

#include "../../core/types.h"
#include <vector>
#include <memory>
#include <functional>
#include <optional>
#include <limits>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <string>
#include <SDL.h>

namespace sad {
namespace graphics {

// دالة مساعدة للحصول على أقصى قيمة Float32 / Helper for max Float32 value
inline Float32 Float32Max() { return std::numeric_limits<Float32>::max(); }

namespace ui {
namespace mobile {

// ============================================================================
// محرك رسم الهاتف / Mobile Renderer2D
// ============================================================================
/// واجهة رسم بسيطة للمركبات الهاتفية
/// Simple rendering interface for mobile widgets
class Renderer2D {
public:
    virtual ~Renderer2D() = default;
    
    /// رسم مستطيل ممتلئ / Draw filled rectangle
    virtual void fillRect(float x, float y, float w, float h, const Color& color) {
        (void)x; (void)y; (void)w; (void)h; (void)color;
    }
    
    /// رسم مستطيل بحدود / Draw outlined rectangle
    virtual void drawRect(float x, float y, float w, float h, const Color& color, float thickness = 1.0f) {
        (void)x; (void)y; (void)w; (void)h; (void)color; (void)thickness;
    }
    
    /// رسم نص / Draw text
    virtual void drawText(const std::string& text, float x, float y, const Color& color, float fontSize = 14.0f) {
        (void)text; (void)x; (void)y; (void)color; (void)fontSize;
    }
    
    /// رسم دائرة ممتلئة / Draw filled circle
    virtual void fillCircle(float cx, float cy, float radius, const Color& color) {
        (void)cx; (void)cy; (void)radius; (void)color;
    }
    
    /// رسم خط / Draw line
    virtual void drawLine(float x1, float y1, float x2, float y2, const Color& color, float thickness = 1.0f) {
        (void)x1; (void)y1; (void)x2; (void)y2; (void)color; (void)thickness;
    }
};

// ============================================================================
// الفئة الأساسية للمركبات الهاتفية / Mobile Widget Base Class
// ============================================================================
/// فئة أساسية بسيطة للمركبات الهاتفية (مستقلة عن نظام سطح المكتب)
/// Lightweight base class for mobile widgets (independent from desktop UI system)
class Widget {
public:
    virtual ~Widget() = default;
    
    /// الحصول على نوع المركب / Get widget type name
    virtual std::string GetTypeName() const { return "Widget"; }
    
    /// الرسم (اختياري) / Render (optional)
    virtual void render(Renderer2D& renderer) { (void)renderer; }
    
    /// التحديث / Update
    virtual void Update(Float32 deltaTime) { (void)deltaTime; }
    
    /// الرسم / Draw
    virtual void Draw() {}
    
    /// معالجة الأحداث / Handle events
    virtual void handleEvent(const SDL_Event& event) { (void)event; }
    
    /// اختبار الضغط / Hit test
    virtual bool hitTest(float x, float y) const {
        return x >= position.x && x <= position.x + size.x &&
               y >= position.y && y <= position.y + size.y;
    }
    
    /// هل المركب ظاهر؟ / Is widget visible?
    bool visible = true;
    
    /// مفتاح فريد (اختياري) / Unique key (optional)
    std::string key;
    
    /// الموقع والحجم / Position and size
    Vec2 position{0, 0};
    Vec2 size{0, 0};
};

// ============================================================================
// الثوابت والتعدادات / Constants and Enums
// ============================================================================

/// محاذاة الطفل الرئيسية / Main axis alignment
enum class MainAxisAlignment {
    Start,          // البداية / Start
    End,            // النهاية / End
    Center,         // الوسط / Center
    SpaceBetween,   // مسافة بين العناصر / Space between items
    SpaceAround,    // مسافة حول العناصر / Space around items
    SpaceEvenly     // مسافة متساوية / Even space
};

/// محاذاة الطفل الثانوية / Cross axis alignment
enum class CrossAxisAlignment {
    Start,      // البداية / Start
    End,        // النهاية / End
    Center,     // الوسط / Center
    Stretch,    // تمديد / Stretch
    Baseline    // خط الأساس / Baseline
};

/// حجم المحور الرئيسي / Main axis size
enum class MainAxisSize {
    Min,    // الحد الأدنى / Minimum
    Max     // الحد الأقصى / Maximum
};

/// محاذاة النص عموديًا / Vertical text alignment
enum class VerticalDirection {
    Up,     // للأعلى / Upward
    Down    // للأسفل / Downward
};

/// وضع التكديس / Stack fit
enum class StackFit {
    Loose,      // فضفاض / Loose
    Expand,     // توسيع / Expand
    Passthrough // تمرير / Pass through
};

/// محاذاة العنصر في Stack
enum class StackAlignment {
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

/// طريقة تناسب الصورة / Box fit
enum class BoxFit {
    Fill,       // ملء كامل / Fill completely
    Contain,    // احتواء / Contain
    Cover,      // تغطية / Cover
    FitWidth,   // عرض مناسب / Fit width
    FitHeight,  // ارتفاع مناسب / Fit height
    None,       // لا شيء / None
    ScaleDown   // تصغير فقط / Scale down only
};

/// اتجاه الالتفاف / Wrap direction
enum class WrapAlignment {
    Start,
    End,
    Center,
    SpaceBetween,
    SpaceAround,
    SpaceEvenly
};

// ============================================================================
// بنية الحواف / Edge Insets Structure
// ============================================================================

/// حواف (padding/margin) / Edge insets for padding/margin
struct EdgeInsets {
    Float32 left;
    Float32 top;
    Float32 right;
    Float32 bottom;
    
    /// الحواف الافتراضية / Default (zero)
    EdgeInsets() : left(0), top(0), right(0), bottom(0) {}
    
    /// كل الجوانب بقيمة واحدة / All sides with single value
    EdgeInsets(Float32 all) : left(all), top(all), right(all), bottom(all) {}
    
    /// كل الجوانب بقيم منفصلة / Each side with separate values
    EdgeInsets(Float32 l, Float32 t, Float32 r, Float32 b)
        : left(l), top(t), right(r), bottom(b) {}
    
    /// كل الجوانب متساوية / All sides equal
    static EdgeInsets All(Float32 value) {
        EdgeInsets e;
        e.left = e.top = e.right = e.bottom = value;
        return e;
    }
    
    /// أفقي وعمودي / Horizontal and vertical
    static EdgeInsets Symmetric(Float32 horizontal = 0, Float32 vertical = 0) {
        EdgeInsets e;
        e.left = e.right = horizontal;
        e.top = e.bottom = vertical;
        return e;
    }
    
    /// كل جانب منفصل / Each side separately
    static EdgeInsets Only(Float32 left = 0, Float32 top = 0, 
                          Float32 right = 0, Float32 bottom = 0) {
        EdgeInsets e;
        e.left = left;
        e.top = top;
        e.right = right;
        e.bottom = bottom;
        return e;
    }
    
    /// جمع حواف / Add insets
    EdgeInsets operator+(const EdgeInsets& other) const {
        EdgeInsets result;
        result.left = left + other.left;
        result.top = top + other.top;
        result.right = right + other.right;
        result.bottom = bottom + other.bottom;
        return result;
    }
    
    /// المجموع الأفقي / Horizontal total
    Float32 Horizontal() const { return left + right; }
    
    /// المجموع العمودي / Vertical total
    Float32 Vertical() const { return top + bottom; }
};

// ============================================================================
// بنية الحدود / Border Structure
// ============================================================================

/// نمط الحد / Border style
enum class BorderStyle {
    None,       // لا حد / No border
    Solid,      // صلب / Solid
    Dashed,     // متقطع / Dashed
    Dotted      // منقط / Dotted
};

/// جانب حد واحد / Single border side
struct BorderSide {
    Color color;
    Float32 width;
    BorderStyle style;
    
    BorderSide() : color(Color::Black), width(1.0f), style(BorderStyle::Solid) {}
    BorderSide(Color c, Float32 w = 1.0f, BorderStyle s = BorderStyle::Solid)
        : color(c), width(w), style(s) {}
    
    static BorderSide None() {
        BorderSide b;
        b.style = BorderStyle::None;
        b.width = 0;
        return b;
    }
};

/// حدود الصندوق / Box border
struct BoxBorder {
    BorderSide left;
    BorderSide top;
    BorderSide right;
    BorderSide bottom;
    
    BoxBorder() = default;
    
    /// كل الجوانب متساوية / All sides equal
    static BoxBorder All(const BorderSide& side) {
        BoxBorder b;
        b.left = b.top = b.right = b.bottom = side;
        return b;
    }
    
    /// كل جانب منفصل / Each side separately
    static BoxBorder Only(const BorderSide& left = BorderSide::None(),
                         const BorderSide& top = BorderSide::None(),
                         const BorderSide& right = BorderSide::None(),
                         const BorderSide& bottom = BorderSide::None()) {
        BoxBorder b;
        b.left = left;
        b.top = top;
        b.right = right;
        b.bottom = bottom;
        return b;
    }
};

/// نصف قطر الزوايا / Border radius
struct BorderRadius {
    Float32 topLeft;
    Float32 topRight;
    Float32 bottomLeft;
    Float32 bottomRight;
    
    BorderRadius() : topLeft(0), topRight(0), bottomLeft(0), bottomRight(0) {}
    
    /// كل الزوايا متساوية / All corners equal
    static BorderRadius Circular(Float32 radius) {
        BorderRadius br;
        br.topLeft = br.topRight = br.bottomLeft = br.bottomRight = radius;
        return br;
    }
    
    /// زوايا منفصلة / Separate corners
    static BorderRadius Only(Float32 topLeft = 0, Float32 topRight = 0,
                            Float32 bottomLeft = 0, Float32 bottomRight = 0) {
        BorderRadius br;
        br.topLeft = topLeft;
        br.topRight = topRight;
        br.bottomLeft = bottomLeft;
        br.bottomRight = bottomRight;
        return br;
    }
};

// ============================================================================
// بنية تنسيق الصندوق / Box Decoration
// ============================================================================

/// نوع التدرج / Gradient type
enum class GradientType {
    Linear,     // خطي / Linear
    Radial,     // شعاعي / Radial
    Sweep       // دائري / Sweep
};

/// التدرج اللوني / Gradient
struct Gradient {
    GradientType type;
    std::vector<Color> colors;
    std::vector<Float32> stops;
    Float32 angle;  // للتدرج الخطي / For linear gradient
    
    Gradient() : type(GradientType::Linear), angle(0) {}
    
    /// تدرج خطي / Linear gradient
    static Gradient Linear(const std::vector<Color>& colors, Float32 angle = 0) {
        Gradient g;
        g.type = GradientType::Linear;
        g.colors = colors;
        g.angle = angle;
        return g;
    }
    
    /// تدرج شعاعي / Radial gradient
    static Gradient Radial(const std::vector<Color>& colors) {
        Gradient g;
        g.type = GradientType::Radial;
        g.colors = colors;
        return g;
    }
};

/// ظل الصندوق / Box shadow
struct BoxShadow {
    Color color;
    Float32 offsetX;
    Float32 offsetY;
    Float32 blurRadius;
    Float32 spreadRadius;
    
    BoxShadow()
        : color(Color(0, 0, 0, 64)), offsetX(0), offsetY(2),
          blurRadius(4), spreadRadius(0) {}
          
    BoxShadow(Color c, Float32 ox, Float32 oy, Float32 blur, Float32 spread = 0)
        : color(c), offsetX(ox), offsetY(oy), blurRadius(blur), spreadRadius(spread) {}
};

/// تنسيق الصندوق / Box decoration
struct BoxDecoration {
    std::optional<Color> color;
    std::optional<Gradient> gradient;
    std::optional<BoxBorder> border;
    std::optional<BorderRadius> borderRadius;
    std::vector<BoxShadow> boxShadow;
    
    BoxDecoration() = default;
    
    /// لون فقط / Color only
    static BoxDecoration Color(const sad::graphics::Color& c) {
        BoxDecoration d;
        d.color = c;
        return d;
    }
    
    /// مع حدود دائرية / With rounded corners
    static BoxDecoration RoundedColor(const sad::graphics::Color& c, Float32 radius) {
        BoxDecoration d;
        d.color = c;
        d.borderRadius = BorderRadius::Circular(radius);
        return d;
    }
};

// ============================================================================
// القيود / Constraints
// ============================================================================

/// قيود الصندوق / Box constraints
struct BoxConstraints {
    Float32 minWidth;
    Float32 maxWidth;
    Float32 minHeight;
    Float32 maxHeight;
    
    BoxConstraints()
        : minWidth(0), maxWidth(Float32Max()), 
          minHeight(0), maxHeight(Float32Max()) {}
          
    BoxConstraints(Float32 minW, Float32 maxW, Float32 minH, Float32 maxH)
        : minWidth(minW), maxWidth(maxW), minHeight(minH), maxHeight(maxH) {}
    
    /// قيود ضيقة (حجم محدد) / Tight constraints (exact size)
    static BoxConstraints Tight(Float32 width, Float32 height) {
        return BoxConstraints(width, width, height, height);
    }
    
    /// قيود فضفاضة (0 إلى max) / Loose constraints
    static BoxConstraints Loose(Float32 maxWidth, Float32 maxHeight) {
        return BoxConstraints(0, maxWidth, 0, maxHeight);
    }
    
    /// هل القيود ضيقة؟ / Are constraints tight?
    bool IsTight() const {
        return minWidth == maxWidth && minHeight == maxHeight;
    }
    
    /// تقييد الحجم / Constrain size
    Vec2 Constrain(Vec2 size) const {
        return Vec2(
            std::clamp(size.x, minWidth, maxWidth),
            std::clamp(size.y, minHeight, maxHeight)
        );
    }
};

// ============================================================================
// 1. Container - الحاوية / Container
// ============================================================================

/// حاوية متعددة الاستخدامات / Versatile container widget
/// مشابه لـ Container في Flutter
class Container : public Widget {
public:
    // خصائص / Properties
    std::optional<Float32> width;
    std::optional<Float32> height;
    EdgeInsets padding;
    EdgeInsets margin;
    BoxDecoration decoration;
    std::shared_ptr<Widget> child;
    StackAlignment alignment;
    BoxConstraints constraints;
    
    Container() : alignment(StackAlignment::Center) {}
    
    /// تعيين الأبعاد / Set dimensions
    Container& SetSize(Float32 w, Float32 h) {
        width = w;
        height = h;
        return *this;
    }
    
    /// تعيين الحشو / Set padding
    Container& SetPadding(const EdgeInsets& p) {
        padding = p;
        return *this;
    }
    
    /// تعيين الهامش / Set margin
    Container& SetMargin(const EdgeInsets& m) {
        margin = m;
        return *this;
    }
    
    /// تعيين التنسيق / Set decoration
    Container& SetDecoration(const BoxDecoration& d) {
        decoration = d;
        return *this;
    }
    
    /// تعيين الطفل / Set child
    Container& SetChild(std::shared_ptr<Widget> c) {
        child = c;
        return *this;
    }
    
    /// تعيين لون الخلفية / Set background color
    Container& SetBackgroundColor(const Color& c) {
        decoration.color = c;
        return *this;
    }
    
    /// تعيين نصف قطر الزوايا / Set border radius
    Container& SetBorderRadius(Float32 radius) {
        decoration.borderRadius = BorderRadius::Circular(radius);
        return *this;
    }
};

// ============================================================================
// 2. Row - صف أفقي / Horizontal Row
// ============================================================================

/// صف أفقي من العناصر / Horizontal row of widgets
/// مشابه لـ Row في Flutter
class Row : public Widget {
public:
    std::vector<std::shared_ptr<Widget>> children;
    MainAxisAlignment mainAxisAlignment;
    CrossAxisAlignment crossAxisAlignment;
    MainAxisSize mainAxisSize;
    VerticalDirection verticalDirection;
    Float32 spacing;  // مسافة بين العناصر / Spacing between items
    
    Row()
        : mainAxisAlignment(MainAxisAlignment::Start),
          crossAxisAlignment(CrossAxisAlignment::Center),
          mainAxisSize(MainAxisSize::Max),
          verticalDirection(VerticalDirection::Down),
          spacing(0) {}
    
    /// إضافة طفل / Add child
    Row& AddChild(std::shared_ptr<Widget> child) {
        children.push_back(child);
        return *this;
    }
    
    /// تعيين المحاذاة الرئيسية / Set main axis alignment
    Row& SetMainAxisAlignment(MainAxisAlignment a) {
        mainAxisAlignment = a;
        return *this;
    }
    
    /// تعيين المحاذاة الثانوية / Set cross axis alignment
    Row& SetCrossAxisAlignment(CrossAxisAlignment a) {
        crossAxisAlignment = a;
        return *this;
    }
    
    /// تعيين المسافة / Set spacing
    Row& SetSpacing(Float32 s) {
        spacing = s;
        return *this;
    }
};

// ============================================================================
// 3. Column - عمود عمودي / Vertical Column
// ============================================================================

/// عمود عمودي من العناصر / Vertical column of widgets
/// مشابه لـ Column في Flutter
class Column : public Widget {
public:
    std::vector<std::shared_ptr<Widget>> children;
    MainAxisAlignment mainAxisAlignment;
    CrossAxisAlignment crossAxisAlignment;
    MainAxisSize mainAxisSize;
    VerticalDirection verticalDirection;
    Float32 spacing;
    
    Column()
        : mainAxisAlignment(MainAxisAlignment::Start),
          crossAxisAlignment(CrossAxisAlignment::Center),
          mainAxisSize(MainAxisSize::Max),
          verticalDirection(VerticalDirection::Down),
          spacing(0) {}
    
    /// إضافة طفل / Add child
    Column& AddChild(std::shared_ptr<Widget> child) {
        children.push_back(child);
        return *this;
    }
    
    /// تعيين المحاذاة الرئيسية / Set main axis alignment
    Column& SetMainAxisAlignment(MainAxisAlignment a) {
        mainAxisAlignment = a;
        return *this;
    }
    
    /// تعيين المحاذاة الثانوية / Set cross axis alignment
    Column& SetCrossAxisAlignment(CrossAxisAlignment a) {
        crossAxisAlignment = a;
        return *this;
    }
    
    /// تعيين المسافة / Set spacing
    Column& SetSpacing(Float32 s) {
        spacing = s;
        return *this;
    }
};

// ============================================================================
// 4. Stack - تكديس / Stack
// ============================================================================

/// تكديس عناصر فوق بعضها / Stack widgets on top of each other
/// مشابه لـ Stack في Flutter
class Stack : public Widget {
public:
    std::vector<std::shared_ptr<Widget>> children;
    StackAlignment alignment;
    StackFit fit;
    bool clipBehavior;  // قص الأطفال الخارجين / Clip overflow
    
    Stack()
        : alignment(StackAlignment::TopLeft),
          fit(StackFit::Loose),
          clipBehavior(true) {}
    
    /// إضافة طفل / Add child
    Stack& AddChild(std::shared_ptr<Widget> child) {
        children.push_back(child);
        return *this;
    }
    
    /// تعيين المحاذاة / Set alignment
    Stack& SetAlignment(StackAlignment a) {
        alignment = a;
        return *this;
    }
};

// ============================================================================
// 5. Padding - حشو / Padding
// ============================================================================

/// إضافة حشو حول طفل / Add padding around a child
/// مشابه لـ Padding في Flutter
class Padding : public Widget {
public:
    EdgeInsets padding;
    std::shared_ptr<Widget> child;
    
    Padding() = default;
    Padding(const EdgeInsets& p) : padding(p) {}
    
    /// تعيين الحشو / Set padding
    Padding& SetPadding(const EdgeInsets& p) {
        padding = p;
        return *this;
    }
    
    /// تعيين الطفل / Set child
    Padding& SetChild(std::shared_ptr<Widget> c) {
        child = c;
        return *this;
    }
};

// ============================================================================
// 6. Center - توسيط / Center
// ============================================================================

/// توسيط طفل / Center a child
/// مشابه لـ Center في Flutter
class Center : public Widget {
public:
    std::shared_ptr<Widget> child;
    std::optional<Float32> widthFactor;
    std::optional<Float32> heightFactor;
    
    Center() = default;
    
    /// تعيين الطفل / Set child
    Center& SetChild(std::shared_ptr<Widget> c) {
        child = c;
        return *this;
    }
};

// ============================================================================
// 7. Align - محاذاة / Align
// ============================================================================

/// محاذاة طفل / Align a child
/// مشابه لـ Align في Flutter
class Align : public Widget {
public:
    std::shared_ptr<Widget> child;
    StackAlignment alignment;
    std::optional<Float32> widthFactor;
    std::optional<Float32> heightFactor;
    
    Align() : alignment(StackAlignment::Center) {}
    
    /// تعيين المحاذاة / Set alignment
    Align& SetAlignment(StackAlignment a) {
        alignment = a;
        return *this;
    }
    
    /// تعيين الطفل / Set child
    Align& SetChild(std::shared_ptr<Widget> c) {
        child = c;
        return *this;
    }
};

// ============================================================================
// 8. SizedBox - صندوق بحجم محدد / Sized Box
// ============================================================================

/// صندوق بحجم محدد / Box with specific size
/// مشابه لـ SizedBox في Flutter
class SizedBox : public Widget {
public:
    std::optional<Float32> width;
    std::optional<Float32> height;
    std::shared_ptr<Widget> child;
    
    SizedBox() = default;
    SizedBox(Float32 w, Float32 h) : width(w), height(h) {}
    
    /// تعيين الأبعاد / Set dimensions
    SizedBox& SetSize(Float32 w, Float32 h) {
        width = w;
        height = h;
        return *this;
    }
    
    /// صندوق مربع / Square box
    static SizedBox Square(Float32 dimension) {
        return SizedBox(dimension, dimension);
    }
    
    /// صندوق توسيع / Expand box
    static SizedBox Expand() {
        SizedBox box;
        box.width = Float32Max();
        box.height = Float32Max();
        return box;
    }
    
    /// صندوق انكماش / Shrink box
    static SizedBox Shrink() {
        return SizedBox(0, 0);
    }
};

// ============================================================================
// 9. Expanded - توسيع / Expanded
// ============================================================================

/// توسيع طفل ليملأ المساحة المتاحة / Expand child to fill available space
/// مشابه لـ Expanded في Flutter
class Expanded : public Widget {
public:
    std::shared_ptr<Widget> child;
    Int32 flex;  // وزن التوسيع / Flex factor
    
    Expanded() : flex(1) {}
    Expanded(std::shared_ptr<Widget> c, Int32 f = 1) : child(c), flex(f) {}
    
    /// تعيين الوزن / Set flex
    Expanded& SetFlex(Int32 f) {
        flex = f;
        return *this;
    }
    
    /// تعيين الطفل / Set child
    Expanded& SetChild(std::shared_ptr<Widget> c) {
        child = c;
        return *this;
    }
};

// ============================================================================
// 10. Flexible - مرن / Flexible
// ============================================================================

/// طفل مرن / Flexible child
/// مشابه لـ Flexible في Flutter
enum class FlexFit {
    Tight,  // ضيق / Tight
    Loose   // فضفاض / Loose
};

class Flexible : public Widget {
public:
    std::shared_ptr<Widget> child;
    Int32 flex;
    FlexFit fit;
    
    Flexible() : flex(1), fit(FlexFit::Loose) {}
    
    /// تعيين الوزن / Set flex
    Flexible& SetFlex(Int32 f) {
        flex = f;
        return *this;
    }
    
    /// تعيين نوع التناسب / Set fit
    Flexible& SetFit(FlexFit f) {
        fit = f;
        return *this;
    }
    
    /// تعيين الطفل / Set child
    Flexible& SetChild(std::shared_ptr<Widget> c) {
        child = c;
        return *this;
    }
};

// ============================================================================
// 11. Spacer - فاصل / Spacer
// ============================================================================

/// فاصل مرن / Flexible spacer
/// مشابه لـ Spacer في Flutter
class Spacer : public Widget {
public:
    Int32 flex;
    
    Spacer() : flex(1) {}
    Spacer(Int32 f) : flex(f) {}
};

// ============================================================================
// 12. AspectRatio - نسبة العرض للارتفاع / Aspect Ratio
// ============================================================================

/// الحفاظ على نسبة العرض للارتفاع / Maintain aspect ratio
/// مشابه لـ AspectRatio في Flutter
class AspectRatio : public Widget {
public:
    Float32 aspectRatio;  // width / height
    std::shared_ptr<Widget> child;
    
    AspectRatio() : aspectRatio(1.0f) {}
    AspectRatio(Float32 ratio) : aspectRatio(ratio) {}
    
    /// نسب شائعة / Common ratios
    static AspectRatio Square() { return AspectRatio(1.0f); }
    static AspectRatio HD() { return AspectRatio(16.0f / 9.0f); }
    static AspectRatio SD() { return AspectRatio(4.0f / 3.0f); }
    static AspectRatio Portrait() { return AspectRatio(9.0f / 16.0f); }
};

// ============================================================================
// 13. FractionallySizedBox - صندوق بحجم نسبي / Fractionally Sized Box
// ============================================================================

/// صندوق بحجم نسبي من الأب / Box sized as fraction of parent
/// مشابه لـ FractionallySizedBox في Flutter
class FractionallySizedBox : public Widget {
public:
    std::optional<Float32> widthFactor;   // 0.0 - 1.0
    std::optional<Float32> heightFactor;  // 0.0 - 1.0
    StackAlignment alignment;
    std::shared_ptr<Widget> child;
    
    FractionallySizedBox() : alignment(StackAlignment::Center) {}
    
    /// تعيين العوامل / Set factors
    FractionallySizedBox& SetFactors(Float32 w, Float32 h) {
        widthFactor = w;
        heightFactor = h;
        return *this;
    }
};

// ============================================================================
// 14. ConstrainedBox - صندوق مقيد / Constrained Box
// ============================================================================

/// صندوق بقيود إضافية / Box with additional constraints
/// مشابه لـ ConstrainedBox في Flutter
class ConstrainedBox : public Widget {
public:
    BoxConstraints constraints;
    std::shared_ptr<Widget> child;
    
    ConstrainedBox() = default;
    ConstrainedBox(const BoxConstraints& c) : constraints(c) {}
    
    /// تعيين القيود / Set constraints
    ConstrainedBox& SetConstraints(const BoxConstraints& c) {
        constraints = c;
        return *this;
    }
};

// ============================================================================
// 15. FittedBox - صندوق متناسب / Fitted Box
// ============================================================================

/// يحجم/يمد الطفل ليناسب / Scales/stretches child to fit
/// مشابه لـ FittedBox في Flutter
class FittedBox : public Widget {
public:
    BoxFit fit;
    StackAlignment alignment;
    std::shared_ptr<Widget> child;
    bool clipBehavior;
    
    FittedBox()
        : fit(BoxFit::Contain),
          alignment(StackAlignment::Center),
          clipBehavior(true) {}
    
    /// تعيين نوع التناسب / Set fit
    FittedBox& SetFit(BoxFit f) {
        fit = f;
        return *this;
    }
};

// ============================================================================
// 16. Wrap - التفاف / Wrap
// ============================================================================

/// التفاف العناصر لصفوف متعددة / Wrap children to multiple lines
/// مشابه لـ Wrap في Flutter
enum class WrapDirection {
    Horizontal,  // أفقي / Horizontal
    Vertical     // عمودي / Vertical
};

class Wrap : public Widget {
public:
    std::vector<std::shared_ptr<Widget>> children;
    WrapDirection direction;
    WrapAlignment alignment;
    WrapAlignment runAlignment;
    CrossAxisAlignment crossAxisAlignment;
    Float32 spacing;
    Float32 runSpacing;
    
    Wrap()
        : direction(WrapDirection::Horizontal),
          alignment(WrapAlignment::Start),
          runAlignment(WrapAlignment::Start),
          crossAxisAlignment(CrossAxisAlignment::Start),
          spacing(0),
          runSpacing(0) {}
    
    /// إضافة طفل / Add child
    Wrap& AddChild(std::shared_ptr<Widget> child) {
        children.push_back(child);
        return *this;
    }
    
    /// تعيين المسافات / Set spacing
    Wrap& SetSpacing(Float32 s, Float32 rs = 0) {
        spacing = s;
        runSpacing = rs;
        return *this;
    }
};

// ============================================================================
// 17. LayoutBuilder - باني التخطيط / Layout Builder
// ============================================================================

/// بناء بناءً على القيود / Build based on constraints
/// مشابه لـ LayoutBuilder في Flutter
class LayoutBuilder : public Widget {
public:
    using BuilderFunc = std::function<std::shared_ptr<Widget>(const BoxConstraints&)>;
    BuilderFunc builder;
    
    LayoutBuilder() = default;
    LayoutBuilder(BuilderFunc b) : builder(b) {}
    
    /// تعيين الباني / Set builder
    LayoutBuilder& SetBuilder(BuilderFunc b) {
        builder = b;
        return *this;
    }
};

// ============================================================================
// 18. Positioned - موضع محدد / Positioned (for Stack)
// ============================================================================

/// عنصر بموضع محدد في Stack / Positioned element in Stack
/// مشابه لـ Positioned في Flutter
class Positioned : public Widget {
public:
    std::shared_ptr<Widget> child;
    std::optional<Float32> left;
    std::optional<Float32> top;
    std::optional<Float32> right;
    std::optional<Float32> bottom;
    std::optional<Float32> width;
    std::optional<Float32> height;
    
    Positioned() = default;
    
    /// ملء الكامل / Fill completely
    static Positioned Fill(std::shared_ptr<Widget> child) {
        Positioned p;
        p.child = child;
        p.left = 0;
        p.top = 0;
        p.right = 0;
        p.bottom = 0;
        return p;
    }
    
    /// من اليسار والأعلى / From left and top
    Positioned& FromLeftTop(Float32 l, Float32 t) {
        left = l;
        top = t;
        return *this;
    }
    
    /// من اليمين والأسفل / From right and bottom
    Positioned& FromRightBottom(Float32 r, Float32 b) {
        right = r;
        bottom = b;
        return *this;
    }
};

// ============================================================================
// 19. SafeArea - منطقة آمنة / Safe Area
// ============================================================================

/// تجنب مناطق النظام / Avoid system intrusions
/// مشابه لـ SafeArea في Flutter
class SafeArea : public Widget {
public:
    std::shared_ptr<Widget> child;
    bool left;
    bool top;
    bool right;
    bool bottom;
    EdgeInsets minimum;
    
    SafeArea()
        : left(true), top(true), right(true), bottom(true) {}
    
    /// تعيين الجوانب / Set sides
    SafeArea& SetSides(bool l, bool t, bool r, bool b) {
        left = l;
        top = t;
        right = r;
        bottom = b;
        return *this;
    }
    
    /// تعيين الطفل / Set child
    SafeArea& SetChild(std::shared_ptr<Widget> c) {
        child = c;
        return *this;
    }
};

// ============================================================================
// 20. IntrinsicWidth/Height - العرض/الارتفاع الجوهري / Intrinsic Size
// ============================================================================

/// حجم الطفل بناءً على حجمه الجوهري / Size child to its intrinsic size
class IntrinsicWidth : public Widget {
public:
    std::shared_ptr<Widget> child;
    std::optional<Float32> stepWidth;
    std::optional<Float32> stepHeight;
    
    IntrinsicWidth() = default;
};

class IntrinsicHeight : public Widget {
public:
    std::shared_ptr<Widget> child;
    
    IntrinsicHeight() = default;
};

// ============================================================================
// 21. Visibility - الرؤية / Visibility
// ============================================================================

/// التحكم في رؤية العنصر / Control widget visibility
class Visibility : public Widget {
public:
    std::shared_ptr<Widget> child;
    std::shared_ptr<Widget> replacement;  // بديل عند الإخفاء / Replacement when hidden
    bool visible;
    bool maintainState;
    bool maintainAnimation;
    bool maintainSize;
    bool maintainSemantics;
    bool maintainInteractivity;
    
    Visibility()
        : visible(true),
          maintainState(false),
          maintainAnimation(false),
          maintainSize(false),
          maintainSemantics(false),
          maintainInteractivity(false) {}
    
    /// تعيين الرؤية / Set visibility
    Visibility& SetVisible(bool v) {
        visible = v;
        return *this;
    }
};

// ============================================================================
// 22. Opacity - الشفافية / Opacity
// ============================================================================

/// التحكم في شفافية العنصر / Control widget opacity
class Opacity : public Widget {
public:
    std::shared_ptr<Widget> child;
    Float32 opacity;  // 0.0 - 1.0
    bool alwaysIncludeSemantics;
    
    Opacity() : opacity(1.0f), alwaysIncludeSemantics(false) {}
    Opacity(Float32 o) : opacity(o), alwaysIncludeSemantics(false) {}
    
    /// تعيين الشفافية / Set opacity
    Opacity& SetOpacity(Float32 o) {
        opacity = std::clamp(o, 0.0f, 1.0f);
        return *this;
    }
};

// ============================================================================
// 23. Transform - التحويل / Transform
// ============================================================================

/// تحويل هندسي للعنصر / Geometric transform on widget
class Transform : public Widget {
public:
    std::shared_ptr<Widget> child;
    Mat4 transform;
    Vec3 origin;
    StackAlignment alignment;
    bool transformHitTests;
    
    Transform()
        : transform(Mat4::Identity()),
          origin(0, 0, 0),
          alignment(StackAlignment::Center),
          transformHitTests(true) {}
    
    /// دوران / Rotation
    static Transform Rotate(Float32 angle, std::shared_ptr<Widget> child = nullptr) {
        Transform t;
        t.transform = Mat4::RotateZ(angle);
        t.child = child;
        return t;
    }
    
    /// تحجيم / Scale
    static Transform Scale(Float32 scaleX, Float32 scaleY = 1.0f,
                          std::shared_ptr<Widget> child = nullptr) {
        Transform t;
        t.transform = Mat4::Scale(Vec3(scaleX, scaleY, 1.0f));
        t.child = child;
        return t;
    }
    
    /// إزاحة / Translate
    static Transform Translate(Float32 x, Float32 y,
                              std::shared_ptr<Widget> child = nullptr) {
        Transform t;
        t.transform = Mat4::Translate(Vec3(x, y, 0));
        t.child = child;
        return t;
    }
};

// ============================================================================
// 24. ClipRect / ClipRRect / ClipOval - القص / Clipping
// ============================================================================

/// قص مستطيل / Rectangular clip
class ClipRect : public Widget {
public:
    std::shared_ptr<Widget> child;
    bool clipBehavior;
    
    ClipRect() : clipBehavior(true) {}
};

/// قص مستطيل دائري / Rounded rectangular clip
class ClipRRect : public Widget {
public:
    std::shared_ptr<Widget> child;
    BorderRadius borderRadius;
    bool clipBehavior;
    
    ClipRRect() : borderRadius(BorderRadius::Circular(0)), clipBehavior(true) {}
    ClipRRect(Float32 radius) : borderRadius(BorderRadius::Circular(radius)), clipBehavior(true) {}
};

/// قص بيضاوي / Oval clip
class ClipOval : public Widget {
public:
    std::shared_ptr<Widget> child;
    bool clipBehavior;
    
    ClipOval() : clipBehavior(true) {}
};

// ============================================================================
// 25. DecoratedBox - صندوق منسق / Decorated Box
// ============================================================================

/// صندوق بتنسيق / Box with decoration
class DecoratedBox : public Widget {
public:
    std::shared_ptr<Widget> child;
    BoxDecoration decoration;
    bool decorateBeforeChild;  // الرسم قبل الطفل / Draw before child
    
    DecoratedBox() : decorateBeforeChild(true) {}
    
    /// تعيين التنسيق / Set decoration
    DecoratedBox& SetDecoration(const BoxDecoration& d) {
        decoration = d;
        return *this;
    }
};

} // namespace mobile
} // namespace ui
} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_MOBILE_CORE_H
