// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_layout.h — مكونات التخطيط بأسلوب Flutter
//  Layout Widgets: Column, Row, Stack, Center, Flex, Expanded, Padding, etc.
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "sad_flutter_core.h"
#include "sad_flutter_widgets.h"

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  Enums — أنواع التخطيط
// ═══════════════════════════════════════════════════════════════════════════════

/// اتجاه المحور
enum class Axis {
    Horizontal,  // أفقي
    Vertical     // عمودي
};

/// محاذاة المحور الرئيسي
enum class MainAxisAlignment {
    Start,          // بداية
    End,            // نهاية
    Center,         // وسط
    SpaceBetween,   // توزيع بين العناصر
    SpaceAround,    // توزيع حول العناصر
    SpaceEvenly     // توزيع متساوي
};

/// حجم المحور الرئيسي
enum class MainAxisSize {
    Min,    // أصغر حجم ممكن
    Max     // أكبر حجم ممكن
};

/// محاذاة المحور العرضي
enum class CrossAxisAlignment {
    Start,      // بداية
    End,        // نهاية
    Center,     // وسط
    Stretch,    // تمدد
    Baseline    // خط الأساس
};

/// محاذاة عامة (للـ Align و Stack)
enum class Alignment {
    TopLeft, TopCenter, TopRight,
    CenterLeft, Center, CenterRight,
    BottomLeft, BottomCenter, BottomRight
};

/// كيفية تحديد حجم العناصر في Stack
enum class StackFit {
    Loose,      // حر
    Expand,     // تمدد
    Passthrough // نقل القيود
};

/// طريقة القص
enum class Clip {
    None,           // بدون
    HardEdge,       // قص حاد
    AntiAlias,      // مع تنعيم
    AntiAliasWithSaveLayer
};

// ═══════════════════════════════════════════════════════════════════════════════
//  AlignmentGeometry — هندسة المحاذاة
// ═══════════════════════════════════════════════════════════════════════════════

struct AlignmentGeometry {
    float x = 0;  // -1.0 to 1.0 (left to right)
    float y = 0;  // -1.0 to 1.0 (top to bottom)
    
    AlignmentGeometry() = default;
    AlignmentGeometry(float x, float y) : x(x), y(y) {}
    
    static AlignmentGeometry topLeft()      { return {-1, -1}; }
    static AlignmentGeometry topCenter()    { return {0, -1}; }
    static AlignmentGeometry topRight()     { return {1, -1}; }
    static AlignmentGeometry centerLeft()   { return {-1, 0}; }
    static AlignmentGeometry center()       { return {0, 0}; }
    static AlignmentGeometry centerRight()  { return {1, 0}; }
    static AlignmentGeometry bottomLeft()   { return {-1, 1}; }
    static AlignmentGeometry bottomCenter() { return {0, 1}; }
    static AlignmentGeometry bottomRight()  { return {1, 1}; }
    
    /// تحويل من Alignment enum
    static AlignmentGeometry fromAlignment(Alignment a) {
        switch (a) {
            case Alignment::TopLeft:      return topLeft();
            case Alignment::TopCenter:    return topCenter();
            case Alignment::TopRight:     return topRight();
            case Alignment::CenterLeft:   return centerLeft();
            case Alignment::Center:       return center();
            case Alignment::CenterRight:  return centerRight();
            case Alignment::BottomLeft:   return bottomLeft();
            case Alignment::BottomCenter: return bottomCenter();
            case Alignment::BottomRight:  return bottomRight();
        }
        return center();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//  FlexRenderObject — RenderObject للـ Flex
// ═══════════════════════════════════════════════════════════════════════════════

class FlexRenderObject : public RenderObject {
public:
    Axis direction = Axis::Vertical;
    MainAxisAlignment mainAxisAlignment = MainAxisAlignment::Start;
    MainAxisSize mainAxisSize = MainAxisSize::Max;
    CrossAxisAlignment crossAxisAlignment = CrossAxisAlignment::Start;
    float spacing = 0;
    
    void layout(float maxWidth, float maxHeight) override;
    void paint() override;
    
private:
    void layoutVertical(float maxWidth, float maxHeight);
    void layoutHorizontal(float maxWidth, float maxHeight);
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Flex — مكون التخطيط المرن الأساسي
// ═══════════════════════════════════════════════════════════════════════════════

class Flex : public MultiChildRenderObjectWidget {
public:
    Flex(
        Axis direction,
        std::vector<WidgetPtr> children,
        MainAxisAlignment mainAxisAlignment = MainAxisAlignment::Start,
        MainAxisSize mainAxisSize = MainAxisSize::Max,
        CrossAxisAlignment crossAxisAlignment = CrossAxisAlignment::Start,
        float spacing = 0,
        Key key = Key()
    )
        : MultiChildRenderObjectWidget(std::move(children), std::move(key))
        , direction_(direction)
        , mainAxisAlignment_(mainAxisAlignment)
        , mainAxisSize_(mainAxisSize)
        , crossAxisAlignment_(crossAxisAlignment)
        , spacing_(spacing)
    {}
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override;
    void updateRenderObject(BuildContext* ctx, RenderObject* ro) override;
    std::string typeName() const override { return "Flex"; }
    
protected:
    Axis direction_;
    MainAxisAlignment mainAxisAlignment_;
    MainAxisSize mainAxisSize_;
    CrossAxisAlignment crossAxisAlignment_;
    float spacing_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Column — عمود (ترتيب عمودي)
// ═══════════════════════════════════════════════════════════════════════════════

class Column : public Flex {
public:
    Column(
        std::vector<WidgetPtr> children,
        MainAxisAlignment mainAxisAlignment = MainAxisAlignment::Start,
        MainAxisSize mainAxisSize = MainAxisSize::Max,
        CrossAxisAlignment crossAxisAlignment = CrossAxisAlignment::Start,
        float spacing = 0,
        Key key = Key()
    )
        : Flex(Axis::Vertical, std::move(children),
               mainAxisAlignment, mainAxisSize, crossAxisAlignment, spacing,
               std::move(key))
    {}
    
    std::string typeName() const override { return "Column"; }
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Row — صف (ترتيب أفقي)
// ═══════════════════════════════════════════════════════════════════════════════

class Row : public Flex {
public:
    Row(
        std::vector<WidgetPtr> children,
        MainAxisAlignment mainAxisAlignment = MainAxisAlignment::Start,
        MainAxisSize mainAxisSize = MainAxisSize::Max,
        CrossAxisAlignment crossAxisAlignment = CrossAxisAlignment::Start,
        float spacing = 0,
        Key key = Key()
    )
        : Flex(Axis::Horizontal, std::move(children),
               mainAxisAlignment, mainAxisSize, crossAxisAlignment, spacing,
               std::move(key))
    {}
    
    std::string typeName() const override { return "Row"; }
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Expanded — توسيع في Flex
// ═══════════════════════════════════════════════════════════════════════════════

class Expanded : public SingleChildRenderObjectWidget {
public:
    Expanded(WidgetPtr child, int flex = 1, Key key = Key())
        : SingleChildRenderObjectWidget(std::move(child), std::move(key))
        , flex_(flex)
    {}
    
    int flex() const { return flex_; }
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override;
    std::string typeName() const override { return "Expanded"; }
    
private:
    int flex_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Flexible — مرونة محدودة
// ═══════════════════════════════════════════════════════════════════════════════

enum class FlexFit {
    Tight,  // يأخذ كل المساحة المتاحة
    Loose   // يأخذ ما يحتاج فقط
};

class Flexible : public SingleChildRenderObjectWidget {
public:
    Flexible(WidgetPtr child, int flex = 1, FlexFit fit = FlexFit::Loose, Key key = Key())
        : SingleChildRenderObjectWidget(std::move(child), std::move(key))
        , flex_(flex)
        , fit_(fit)
    {}
    
    int flex() const { return flex_; }
    FlexFit fit() const { return fit_; }
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override;
    std::string typeName() const override { return "Flexible"; }
    
private:
    int flex_;
    FlexFit fit_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Stack — تكديس (layers)
// ═══════════════════════════════════════════════════════════════════════════════

class StackRenderObject : public RenderObject {
public:
    AlignmentGeometry alignment = AlignmentGeometry::topLeft();
    StackFit fit = StackFit::Loose;
    Clip clipBehavior = Clip::HardEdge;
    
    void layout(float maxWidth, float maxHeight) override;
    void paint() override;
};

class Stack : public MultiChildRenderObjectWidget {
public:
    Stack(
        std::vector<WidgetPtr> children,
        AlignmentGeometry alignment = AlignmentGeometry::topLeft(),
        StackFit fit = StackFit::Loose,
        Clip clipBehavior = Clip::HardEdge,
        Key key = Key()
    )
        : MultiChildRenderObjectWidget(std::move(children), std::move(key))
        , alignment_(alignment)
        , fit_(fit)
        , clipBehavior_(clipBehavior)
    {}
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override;
    std::string typeName() const override { return "Stack"; }
    
private:
    AlignmentGeometry alignment_;
    StackFit fit_;
    Clip clipBehavior_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Positioned — موضع محدد في Stack
// ═══════════════════════════════════════════════════════════════════════════════

class Positioned : public SingleChildRenderObjectWidget {
public:
    Positioned(
        WidgetPtr child,
        std::optional<float> left = std::nullopt,
        std::optional<float> top = std::nullopt,
        std::optional<float> right = std::nullopt,
        std::optional<float> bottom = std::nullopt,
        std::optional<float> width = std::nullopt,
        std::optional<float> height = std::nullopt,
        Key key = Key()
    )
        : SingleChildRenderObjectWidget(std::move(child), std::move(key))
        , left_(left), top_(top), right_(right), bottom_(bottom)
        , width_(width), height_(height)
    {}
    
    /// ملء كامل Stack
    static std::shared_ptr<Positioned> fill(WidgetPtr child) {
        return std::make_shared<Positioned>(std::move(child), 0.0f, 0.0f, 0.0f, 0.0f);
    }
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override;
    std::string typeName() const override { return "Positioned"; }
    
    std::optional<float> left() const { return left_; }
    std::optional<float> top() const { return top_; }
    std::optional<float> right() const { return right_; }
    std::optional<float> bottom() const { return bottom_; }
    
private:
    std::optional<float> left_, top_, right_, bottom_;
    std::optional<float> width_, height_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Center — توسيط
// ═══════════════════════════════════════════════════════════════════════════════

class Center : public SingleChildRenderObjectWidget {
public:
    Center(WidgetPtr child, Key key = Key())
        : SingleChildRenderObjectWidget(std::move(child), std::move(key))
    {}
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override;
    std::string typeName() const override { return "Center"; }
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Align — محاذاة
// ═══════════════════════════════════════════════════════════════════════════════

class Align : public SingleChildRenderObjectWidget {
public:
    Align(
        WidgetPtr child,
        AlignmentGeometry alignment = AlignmentGeometry::center(),
        std::optional<float> widthFactor = std::nullopt,
        std::optional<float> heightFactor = std::nullopt,
        Key key = Key()
    )
        : SingleChildRenderObjectWidget(std::move(child), std::move(key))
        , alignment_(alignment)
        , widthFactor_(widthFactor)
        , heightFactor_(heightFactor)
    {}
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override;
    std::string typeName() const override { return "Align"; }
    
private:
    AlignmentGeometry alignment_;
    std::optional<float> widthFactor_, heightFactor_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Padding — حشو
// ═══════════════════════════════════════════════════════════════════════════════

class Padding : public SingleChildRenderObjectWidget {
public:
    Padding(EdgeInsets padding, WidgetPtr child, Key key = Key())
        : SingleChildRenderObjectWidget(std::move(child), std::move(key))
        , padding_(padding)
    {}
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override;
    std::string typeName() const override { return "Padding"; }
    
private:
    EdgeInsets padding_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  ConstrainedBox — قيود الحجم
// ═══════════════════════════════════════════════════════════════════════════════

class ConstrainedBox : public SingleChildRenderObjectWidget {
public:
    ConstrainedBox(BoxConstraints constraints, WidgetPtr child, Key key = Key())
        : SingleChildRenderObjectWidget(std::move(child), std::move(key))
        , constraints_(constraints)
    {}
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override;
    std::string typeName() const override { return "ConstrainedBox"; }
    
private:
    BoxConstraints constraints_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  AspectRatio — نسبة العرض للارتفاع
// ═══════════════════════════════════════════════════════════════════════════════

class AspectRatio : public SingleChildRenderObjectWidget {
public:
    AspectRatio(float aspectRatio, WidgetPtr child = nullptr, Key key = Key())
        : SingleChildRenderObjectWidget(std::move(child), std::move(key))
        , aspectRatio_(aspectRatio)
    {}
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override;
    std::string typeName() const override { return "AspectRatio"; }
    
private:
    float aspectRatio_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  FractionallySizedBox — حجم نسبي
// ═══════════════════════════════════════════════════════════════════════════════

class FractionallySizedBox : public SingleChildRenderObjectWidget {
public:
    FractionallySizedBox(
        WidgetPtr child = nullptr,
        std::optional<float> widthFactor = std::nullopt,
        std::optional<float> heightFactor = std::nullopt,
        AlignmentGeometry alignment = AlignmentGeometry::center(),
        Key key = Key()
    )
        : SingleChildRenderObjectWidget(std::move(child), std::move(key))
        , widthFactor_(widthFactor)
        , heightFactor_(heightFactor)
        , alignment_(alignment)
    {}
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override;
    std::string typeName() const override { return "FractionallySizedBox"; }
    
private:
    std::optional<float> widthFactor_, heightFactor_;
    AlignmentGeometry alignment_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Wrap — التفاف النص
// ═══════════════════════════════════════════════════════════════════════════════

enum class WrapAlignment {
    Start,
    End,
    Center,
    SpaceBetween,
    SpaceAround,
    SpaceEvenly
};

class WrapRenderObject : public RenderObject {
public:
    Axis direction = Axis::Horizontal;
    WrapAlignment alignment = WrapAlignment::Start;
    float spacing = 0;
    float runSpacing = 0;
    WrapAlignment runAlignment = WrapAlignment::Start;
    CrossAxisAlignment crossAxisAlignment = CrossAxisAlignment::Start;
    
    void layout(float maxWidth, float maxHeight) override;
    void paint() override;
};

class Wrap : public MultiChildRenderObjectWidget {
public:
    Wrap(
        std::vector<WidgetPtr> children,
        Axis direction = Axis::Horizontal,
        WrapAlignment alignment = WrapAlignment::Start,
        float spacing = 0,
        float runSpacing = 0,
        Key key = Key()
    )
        : MultiChildRenderObjectWidget(std::move(children), std::move(key))
        , direction_(direction)
        , alignment_(alignment)
        , spacing_(spacing)
        , runSpacing_(runSpacing)
    {}
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override;
    std::string typeName() const override { return "Wrap"; }
    
private:
    Axis direction_;
    WrapAlignment alignment_;
    float spacing_, runSpacing_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  ListView — قائمة قابلة للتمرير
// ═══════════════════════════════════════════════════════════════════════════════

class ListView : public StatefulWidget {
public:
    ListView(
        std::vector<WidgetPtr> children,
        Axis scrollDirection = Axis::Vertical,
        EdgeInsets padding = {},
        bool reverse = false,
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , children_(std::move(children))
        , scrollDirection_(scrollDirection)
        , padding_(padding)
        , reverse_(reverse)
    {}
    
    /// ListView.builder - بناء العناصر عند الحاجة
    static std::shared_ptr<ListView> builder(
        int itemCount,
        std::function<WidgetPtr(int index)> itemBuilder,
        Axis scrollDirection = Axis::Vertical,
        EdgeInsets padding = {},
        Key key = Key()
    );
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "ListView"; }
    
    const std::vector<WidgetPtr>& children() const { return children_; }
    Axis scrollDirection() const { return scrollDirection_; }
    EdgeInsets padding() const { return padding_; }
    bool reverse() const { return reverse_; }
    
private:
    std::vector<WidgetPtr> children_;
    Axis scrollDirection_;
    EdgeInsets padding_;
    bool reverse_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  SingleChildScrollView — عرض قابل للتمرير بطفل واحد
// ═══════════════════════════════════════════════════════════════════════════════

class SingleChildScrollView : public StatefulWidget {
public:
    SingleChildScrollView(
        WidgetPtr child,
        Axis scrollDirection = Axis::Vertical,
        EdgeInsets padding = {},
        bool reverse = false,
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , child_(std::move(child))
        , scrollDirection_(scrollDirection)
        , padding_(padding)
        , reverse_(reverse)
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "SingleChildScrollView"; }
    
private:
    WidgetPtr child_;
    Axis scrollDirection_;
    EdgeInsets padding_;
    bool reverse_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  GridView — شبكة
// ═══════════════════════════════════════════════════════════════════════════════

class GridView : public StatefulWidget {
public:
    GridView(
        std::vector<WidgetPtr> children,
        int crossAxisCount,
        float mainAxisSpacing = 0,
        float crossAxisSpacing = 0,
        float childAspectRatio = 1.0f,
        EdgeInsets padding = {},
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , children_(std::move(children))
        , crossAxisCount_(crossAxisCount)
        , mainAxisSpacing_(mainAxisSpacing)
        , crossAxisSpacing_(crossAxisSpacing)
        , childAspectRatio_(childAspectRatio)
        , padding_(padding)
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "GridView"; }
    
private:
    std::vector<WidgetPtr> children_;
    int crossAxisCount_;
    float mainAxisSpacing_, crossAxisSpacing_;
    float childAspectRatio_;
    EdgeInsets padding_;
};

} // namespace flutter
} // namespace sad
