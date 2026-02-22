// ==============================================================================
// shapes.h - نظام رسم الأشكال المتقدمة / Advanced Shape Drawing System
// ==============================================================================
// الوصف: واجهة برمجية لرسم أشكال هندسية متنوعة
// Description: API for drawing various geometric shapes
// ==============================================================================
// المميزات / Features:
//   - أشكال أساسية (دوائر، مستطيلات، خطوط) / Basic shapes
//   - مضلعات مخصصة / Custom polygons
//   - منحنيات بيزييه / Bezier curves
//   - أشكال مملوءة ومخططة / Filled and stroked shapes
//   - تدرجات لونية / Gradients
//   - أنماط الخطوط / Line styles
// ==============================================================================

#ifndef SAD_GRAPHICS_SHAPES_H
#define SAD_GRAPHICS_SHAPES_H

#include "../core/types.h"
#include <vector>
#include <memory>
#include <functional>

namespace sad {
namespace graphics {

// ==============================================================================
// أنماط الخطوط / Line Styles
// ==============================================================================

/// نمط نهاية الخط / Line cap style
enum class LineCap {
    Butt,       // مقطوع / Flat end
    Round,      // دائري / Round end
    Square      // مربع / Square end
};

/// نمط وصل الخطوط / Line join style
enum class LineJoin {
    Miter,      // حاد / Sharp corner
    Round,      // دائري / Round corner
    Bevel       // مائل / Beveled corner
};

/// نمط الخط المتقطع / Dash pattern
struct DashPattern {
    std::vector<Float32> pattern;   // نمط التقطيع [خط، فراغ، خط، فراغ...] / Dash pattern
    Float32 offset = 0.0f;          // إزاحة البداية / Start offset
    
    /// بدون تقطيع / No dash
    static DashPattern Solid() { return {}; }
    
    /// متقطع / Dashed
    static DashPattern Dashed(Float32 dashLength = 10.0f, Float32 gapLength = 5.0f) {
        DashPattern p;
        p.pattern = {dashLength, gapLength};
        return p;
    }
    
    /// منقط / Dotted
    static DashPattern Dotted(Float32 dotSize = 2.0f, Float32 gap = 4.0f) {
        DashPattern p;
        p.pattern = {dotSize, gap};
        return p;
    }
    
    /// شرطة ونقطة / Dash-dot
    static DashPattern DashDot(Float32 dash = 10.0f, Float32 dot = 2.0f, Float32 gap = 4.0f) {
        DashPattern p;
        p.pattern = {dash, gap, dot, gap};
        return p;
    }
};

/// إعدادات الخط / Stroke settings
struct StrokeStyle {
    Float32 width = 1.0f;           // سماكة الخط / Line width
    Color color = Color::White;    // لون الخط / Stroke color
    LineCap cap = LineCap::Round;   // نمط النهاية / Cap style
    LineJoin join = LineJoin::Round;// نمط الوصل / Join style
    Float32 miterLimit = 10.0f;     // حد الزاوية الحادة / Miter limit
    DashPattern dash;               // نمط التقطيع / Dash pattern
    
    /// إنشاء نمط خط / Create stroke style
    static StrokeStyle Create(Float32 width, Color color) {
        StrokeStyle s;
        s.width = width;
        s.color = color;
        return s;
    }
};

// ==============================================================================
// أنماط الملء / Fill Styles
// ==============================================================================

/// نوع التدرج / Gradient type
enum class GradientType {
    Linear,     // خطي / Linear
    Radial,     // شعاعي / Radial
    Angular     // زاوي / Angular (conical)
};

/// نقطة توقف التدرج / Gradient stop
struct GradientStop {
    Float32 position;   // الموقع (0-1) / Position (0-1)
    Color color;        // اللون / Color
    
    GradientStop(Float32 pos, Color col) : position(pos), color(col) {}
};

/// التدرج اللوني / Gradient
struct Gradient {
    GradientType type = GradientType::Linear;
    Vec2 start;             // نقطة البداية / Start point
    Vec2 end;               // نقطة النهاية / End point
    Float32 radius = 0.0f;  // نصف القطر (للشعاعي) / Radius (for radial)
    std::vector<GradientStop> stops;
    
    /// تدرج خطي / Linear gradient
    static Gradient Linear(Vec2 from, Vec2 to, Color startColor, Color endColor) {
        Gradient g;
        g.type = GradientType::Linear;
        g.start = from;
        g.end = to;
        g.stops = {{0.0f, startColor}, {1.0f, endColor}};
        return g;
    }
    
    /// تدرج شعاعي / Radial gradient
    static Gradient Radial(Vec2 center, Float32 radius, Color innerColor, Color outerColor) {
        Gradient g;
        g.type = GradientType::Radial;
        g.start = center;
        g.radius = radius;
        g.stops = {{0.0f, innerColor}, {1.0f, outerColor}};
        return g;
    }
    
    /// إضافة نقطة توقف / Add stop
    Gradient& AddStop(Float32 position, Color color) {
        stops.push_back({position, color});
        return *this;
    }
};

/// إعدادات الملء / Fill settings
struct FillStyle {
    enum class Type {
        None,       // بدون ملء / No fill
        Solid,      // لون واحد / Solid color
        Gradient    // تدرج / Gradient
    };
    
    Type type = Type::Solid;
    Color color = Color::White;
    Gradient gradient;
    
    /// بدون ملء / No fill
    static FillStyle None() {
        FillStyle f;
        f.type = Type::None;
        return f;
    }
    
    /// ملء بلون واحد / Solid fill
    static FillStyle Solid(Color color) {
        FillStyle f;
        f.type = Type::Solid;
        f.color = color;
        return f;
    }
    
    /// ملء بتدرج / Gradient fill
    static FillStyle WithGradient(const Gradient& gradient) {
        FillStyle f;
        f.type = Type::Gradient;
        f.gradient = gradient;
        return f;
    }
};

// ==============================================================================
// الأشكال / Shapes
// ==============================================================================

/// القوس / Arc
struct Arc {
    Vec2 center;            // المركز / Center
    Float32 radius;         // نصف القطر / Radius
    Float32 startAngle;     // زاوية البداية (راديان) / Start angle (radians)
    Float32 endAngle;       // زاوية النهاية (راديان) / End angle (radians)
    bool clockwise = true;  // باتجاه عقارب الساعة / Clockwise
};

/// القطاع / Pie/Sector
struct Pie {
    Vec2 center;
    Float32 radius;
    Float32 startAngle;
    Float32 endAngle;
};

/// الحلقة / Ring/Annulus
struct Ring {
    Vec2 center;
    Float32 innerRadius;
    Float32 outerRadius;
    Float32 startAngle = 0.0f;
    Float32 endAngle = 6.28318f; // 2π
};

/// المستطيل المدور / Rounded rectangle
struct RoundedRect {
    Rect rect;
    Float32 cornerRadius = 10.0f;
    
    // أو أنصاف أقطار مختلفة لكل زاوية / Or different radii per corner
    Float32 topLeftRadius = 0.0f;
    Float32 topRightRadius = 0.0f;
    Float32 bottomLeftRadius = 0.0f;
    Float32 bottomRightRadius = 0.0f;
    bool useUniformRadius = true;
};

/// المضلع المنتظم / Regular polygon
struct RegularPolygon {
    Vec2 center;
    Int32 sides;            // عدد الأضلاع / Number of sides
    Float32 radius;         // نصف القطر / Radius
    Float32 rotation = 0.0f;// الدوران (راديان) / Rotation (radians)
};

/// النجمة / Star
struct Star {
    Vec2 center;
    Int32 points;           // عدد الرؤوس / Number of points
    Float32 outerRadius;    // نصف القطر الخارجي / Outer radius
    Float32 innerRadius;    // نصف القطر الداخلي / Inner radius
    Float32 rotation = 0.0f;
};

/// منحنى بيزييه التربيعي / Quadratic Bezier curve
struct QuadraticBezier {
    Vec2 start;     // نقطة البداية / Start point
    Vec2 control;   // نقطة التحكم / Control point
    Vec2 end;       // نقطة النهاية / End point
};

/// منحنى بيزييه التكعيبي / Cubic Bezier curve
struct CubicBezier {
    Vec2 start;     // نقطة البداية / Start point
    Vec2 control1;  // نقطة التحكم الأولى / First control point
    Vec2 control2;  // نقطة التحكم الثانية / Second control point
    Vec2 end;       // نقطة النهاية / End point
};

/// منحنى سبلاين / Spline curve
struct Spline {
    std::vector<Vec2> points;   // نقاط التحكم / Control points
    Float32 tension = 0.5f;     // التوتر / Tension
    bool closed = false;        // مغلق / Closed
};

// ==============================================================================
// المسار / Path
// ==============================================================================

/// نوع أمر المسار / Path command type
enum class PathCommandType {
    MoveTo,         // الانتقال إلى / Move to
    LineTo,         // خط إلى / Line to
    QuadraticTo,    // منحنى تربيعي إلى / Quadratic curve to
    CubicTo,        // منحنى تكعيبي إلى / Cubic curve to
    ArcTo,          // قوس إلى / Arc to
    Close           // إغلاق المسار / Close path
};

/// أمر المسار / Path command
struct PathCommand {
    PathCommandType type;
    Vec2 point;
    Vec2 control1;
    Vec2 control2;
    Float32 radius = 0.0f;  // لـ ArcTo
};

/// المسار / Path for complex shapes
class Path {
public:
    // ============================================================================
    // البناء / Construction
    // ============================================================================
    
    Path() = default;
    ~Path() = default;
    
    /// مسح المسار / Clear path
    Path& Clear();
    
    // ============================================================================
    // أوامر الرسم / Drawing Commands
    // ============================================================================
    
    /// الانتقال إلى نقطة (بدون رسم) / Move to point (no drawing)
    Path& MoveTo(Float32 x, Float32 y);
    Path& MoveTo(Vec2 point);
    
    /// رسم خط إلى نقطة / Draw line to point
    Path& LineTo(Float32 x, Float32 y);
    Path& LineTo(Vec2 point);
    
    /// رسم خط أفقي / Draw horizontal line
    Path& HorizontalTo(Float32 x);
    
    /// رسم خط عمودي / Draw vertical line
    Path& VerticalTo(Float32 y);
    
    /// منحنى بيزييه تربيعي / Quadratic Bezier curve
    Path& QuadraticTo(Vec2 control, Vec2 end);
    Path& QuadraticTo(Float32 cx, Float32 cy, Float32 ex, Float32 ey);
    
    /// منحنى بيزييه تكعيبي / Cubic Bezier curve
    Path& CubicTo(Vec2 control1, Vec2 control2, Vec2 end);
    Path& CubicTo(Float32 c1x, Float32 c1y, Float32 c2x, Float32 c2y, 
                  Float32 ex, Float32 ey);
    
    /// قوس إلى نقطة / Arc to point
    Path& ArcTo(Vec2 center, Float32 radius, Float32 startAngle, Float32 endAngle);
    
    /// إغلاق المسار (خط إلى نقطة البداية) / Close path (line to start)
    Path& Close();
    
    // ============================================================================
    // أشكال جاهزة / Built-in Shapes
    // ============================================================================
    
    /// مستطيل / Rectangle
    Path& AddRect(const Rect& rect);
    Path& AddRect(Float32 x, Float32 y, Float32 width, Float32 height);
    
    /// مستطيل مدور / Rounded rectangle
    Path& AddRoundedRect(const Rect& rect, Float32 radius);
    Path& AddRoundedRect(const RoundedRect& roundedRect);
    
    /// دائرة / Circle
    Path& AddCircle(Vec2 center, Float32 radius);
    Path& AddCircle(Float32 cx, Float32 cy, Float32 radius);
    
    /// بيضاوي / Ellipse
    Path& AddEllipse(Vec2 center, Float32 radiusX, Float32 radiusY);
    
    /// قوس / Arc
    Path& AddArc(const Arc& arc);
    
    /// قطاع / Pie
    Path& AddPie(const Pie& pie);
    
    /// حلقة / Ring
    Path& AddRing(const Ring& ring);
    
    /// مضلع / Polygon
    Path& AddPolygon(const std::vector<Vec2>& points);
    
    /// مضلع منتظم / Regular polygon
    Path& AddRegularPolygon(const RegularPolygon& polygon);
    
    /// نجمة / Star
    Path& AddStar(const Star& star);
    
    /// منحنى بيزييه تربيعي / Quadratic Bezier
    Path& AddQuadraticBezier(const QuadraticBezier& bezier);
    
    /// منحنى بيزييه تكعيبي / Cubic Bezier
    Path& AddCubicBezier(const CubicBezier& bezier);
    
    /// سبلاين / Spline
    Path& AddSpline(const Spline& spline);
    
    // ============================================================================
    // التحويلات / Transformations
    // ============================================================================
    
    /// إزاحة / Translate
    Path& Translate(Float32 x, Float32 y);
    Path& Translate(Vec2 offset);
    
    /// تحجيم / Scale
    Path& Scale(Float32 sx, Float32 sy);
    Path& Scale(Float32 s);
    
    /// دوران / Rotate
    Path& Rotate(Float32 angle, Vec2 center = Vec2(0, 0));
    
    // ============================================================================
    // الخصائص / Properties
    // ============================================================================
    
    /// هل المسار فارغ / Is path empty
    bool IsEmpty() const { return m_commands.empty(); }
    
    /// عدد الأوامر / Command count
    Int32 GetCommandCount() const { return static_cast<Int32>(m_commands.size()); }
    
    /// الأوامر / Commands
    const std::vector<PathCommand>& GetCommands() const { return m_commands; }
    
    /// النقطة الحالية / Current point
    Vec2 GetCurrentPoint() const { return m_currentPoint; }
    
    /// حدود المسار / Path bounds
    Rect GetBounds() const;

private:
    std::vector<PathCommand> m_commands;
    Vec2 m_currentPoint = Vec2(0, 0);
    Vec2 m_startPoint = Vec2(0, 0);
};

// ==============================================================================
// راسم الأشكال / Shape Renderer
// ==============================================================================

/// راسم الأشكال / Shape renderer
class ShapeRenderer {
public:
    // ============================================================================
    // الإنشاء والتدمير / Construction and Destruction
    // ============================================================================
    
    ShapeRenderer();
    ~ShapeRenderer();
    
    // ============================================================================
    // التهيئة / Initialization
    // ============================================================================
    
    /// تهيئة الراسم / Initialize renderer
    bool Initialize();
    
    /// تنظيف / Cleanup
    void Cleanup();
    
    // ============================================================================
    // بدء وإنهاء الدفعة / Batch Begin/End
    // ============================================================================
    
    /// بدء الرسم / Begin drawing
    void Begin(const Mat4& projection);
    
    /// إنهاء الرسم (تنفيذ الدفعة) / End drawing (flush batch)
    void End();
    
    /// تنفيذ الدفعة / Flush batch
    void Flush();
    
    // ============================================================================
    // رسم الأشكال الأساسية / Basic Shape Drawing
    // ============================================================================
    
    /// رسم خط / Draw line
    void DrawLine(Vec2 start, Vec2 end, const StrokeStyle& stroke = StrokeStyle());
    void DrawLine(Float32 x1, Float32 y1, Float32 x2, Float32 y2, 
                  const StrokeStyle& stroke = StrokeStyle());
    
    /// رسم خطوط متصلة / Draw polyline
    void DrawPolyline(const std::vector<Vec2>& points, const StrokeStyle& stroke = StrokeStyle());
    
    /// رسم مستطيل / Draw rectangle
    void DrawRect(const Rect& rect, const FillStyle& fill = FillStyle::None(),
                  const StrokeStyle& stroke = StrokeStyle());
    void DrawRect(Float32 x, Float32 y, Float32 width, Float32 height,
                  const FillStyle& fill = FillStyle::None(),
                  const StrokeStyle& stroke = StrokeStyle());
    
    /// رسم مستطيل مدور / Draw rounded rectangle
    void DrawRoundedRect(const Rect& rect, Float32 radius,
                          const FillStyle& fill = FillStyle::None(),
                          const StrokeStyle& stroke = StrokeStyle());
    void DrawRoundedRect(const RoundedRect& roundedRect,
                          const FillStyle& fill = FillStyle::None(),
                          const StrokeStyle& stroke = StrokeStyle());
    
    /// رسم دائرة / Draw circle
    void DrawCircle(Vec2 center, Float32 radius,
                    const FillStyle& fill = FillStyle::None(),
                    const StrokeStyle& stroke = StrokeStyle());
    void DrawCircle(Float32 cx, Float32 cy, Float32 radius,
                    const FillStyle& fill = FillStyle::None(),
                    const StrokeStyle& stroke = StrokeStyle());
    
    /// رسم بيضاوي / Draw ellipse
    void DrawEllipse(Vec2 center, Float32 radiusX, Float32 radiusY,
                     const FillStyle& fill = FillStyle::None(),
                     const StrokeStyle& stroke = StrokeStyle());
    
    /// رسم قوس / Draw arc
    void DrawArc(const Arc& arc, const StrokeStyle& stroke = StrokeStyle());
    
    /// رسم قطاع / Draw pie/sector
    void DrawPie(const Pie& pie, const FillStyle& fill = FillStyle::None(),
                 const StrokeStyle& stroke = StrokeStyle());
    
    /// رسم حلقة / Draw ring
    void DrawRing(const Ring& ring, const FillStyle& fill = FillStyle::None(),
                  const StrokeStyle& stroke = StrokeStyle());
    
    /// رسم مضلع / Draw polygon
    void DrawPolygon(const std::vector<Vec2>& points,
                     const FillStyle& fill = FillStyle::None(),
                     const StrokeStyle& stroke = StrokeStyle());
    
    /// رسم مضلع منتظم / Draw regular polygon
    void DrawRegularPolygon(const RegularPolygon& polygon,
                             const FillStyle& fill = FillStyle::None(),
                             const StrokeStyle& stroke = StrokeStyle());
    
    /// رسم نجمة / Draw star
    void DrawStar(const Star& star, const FillStyle& fill = FillStyle::None(),
                  const StrokeStyle& stroke = StrokeStyle());
    
    /// رسم مثلث / Draw triangle
    void DrawTriangle(Vec2 p1, Vec2 p2, Vec2 p3,
                       const FillStyle& fill = FillStyle::None(),
                       const StrokeStyle& stroke = StrokeStyle());
    
    // ============================================================================
    // رسم المنحنيات / Curve Drawing
    // ============================================================================
    
    /// رسم منحنى بيزييه تربيعي / Draw quadratic Bezier
    void DrawQuadraticBezier(const QuadraticBezier& bezier, 
                              const StrokeStyle& stroke = StrokeStyle());
    
    /// رسم منحنى بيزييه تكعيبي / Draw cubic Bezier
    void DrawCubicBezier(const CubicBezier& bezier, 
                          const StrokeStyle& stroke = StrokeStyle());
    
    /// رسم سبلاين / Draw spline
    void DrawSpline(const Spline& spline, const StrokeStyle& stroke = StrokeStyle());
    
    // ============================================================================
    // رسم المسار / Path Drawing
    // ============================================================================
    
    /// رسم مسار (خط فقط) / Draw path (stroke only)
    void StrokePath(const Path& path, const StrokeStyle& stroke = StrokeStyle());
    
    /// ملء مسار / Fill path
    void FillPath(const Path& path, const FillStyle& fill = FillStyle::Solid(Color::White));
    
    /// رسم وملء مسار / Draw and fill path
    void DrawPath(const Path& path, const FillStyle& fill = FillStyle::None(),
                  const StrokeStyle& stroke = StrokeStyle());
    
    // ============================================================================
    // أشكال خاصة / Special Shapes
    // ============================================================================
    
    /// رسم سهم / Draw arrow
    void DrawArrow(Vec2 start, Vec2 end, Float32 headSize = 10.0f,
                   const StrokeStyle& stroke = StrokeStyle());
    
    /// رسم سهم ثنائي الرأس / Draw double-headed arrow
    void DrawDoubleArrow(Vec2 start, Vec2 end, Float32 headSize = 10.0f,
                          const StrokeStyle& stroke = StrokeStyle());
    
    /// رسم شبكة / Draw grid
    void DrawGrid(const Rect& bounds, Float32 cellWidth, Float32 cellHeight,
                  const StrokeStyle& stroke = StrokeStyle());
    
    /// رسم علامة X / Draw X mark
    void DrawX(Vec2 center, Float32 size, const StrokeStyle& stroke = StrokeStyle());
    
    /// رسم علامة صح / Draw checkmark
    void DrawCheckmark(Vec2 center, Float32 size, const StrokeStyle& stroke = StrokeStyle());
    
    /// رسم قلب / Draw heart
    void DrawHeart(Vec2 center, Float32 size, const FillStyle& fill = FillStyle::None(),
                   const StrokeStyle& stroke = StrokeStyle());
    
    // ============================================================================
    // الإعدادات / Settings
    // ============================================================================
    
    /// جودة الدوائر والمنحنيات / Circle/curve quality (segments)
    void SetCurveQuality(Int32 segments) { m_curveSegments = segments; }
    Int32 GetCurveQuality() const { return m_curveSegments; }
    
    /// تفعيل/تعطيل مكافحة التعرج / Enable/disable anti-aliasing
    void SetAntiAliasing(bool enabled) { m_antiAliasing = enabled; }
    bool IsAntiAliasingEnabled() const { return m_antiAliasing; }

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    Int32 m_curveSegments = 32;
    bool m_antiAliasing = true;
    
    // دوال مساعدة / Helper functions
    std::vector<Vec2> TessellateCircle(Vec2 center, Float32 radius, Int32 segments);
    std::vector<Vec2> TessellateArc(Vec2 center, Float32 radius, 
                                     Float32 startAngle, Float32 endAngle, Int32 segments);
    std::vector<Vec2> TessellateQuadraticBezier(Vec2 p0, Vec2 p1, Vec2 p2, Int32 segments);
    std::vector<Vec2> TessellateCubicBezier(Vec2 p0, Vec2 p1, Vec2 p2, Vec2 p3, Int32 segments);
    std::vector<Vec2> TessellateRoundedRect(const RoundedRect& rect, Int32 cornerSegments);
    std::vector<Vec2> TessellatePath(const Path& path, Int32 curveSegments);
    
    void DrawFilledPolygon(const std::vector<Vec2>& vertices, const FillStyle& fill);
    void DrawStrokedPolyline(const std::vector<Vec2>& vertices, const StrokeStyle& stroke, 
                              bool closed = false);
};

// ==============================================================================
// الوصول العالمي / Global Access
// ==============================================================================

/// الحصول على راسم الأشكال / Get shape renderer instance
ShapeRenderer& GetShapeRenderer();

} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_SHAPES_H
