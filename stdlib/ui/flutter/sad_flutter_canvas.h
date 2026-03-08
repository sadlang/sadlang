// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_canvas.h — نظام الرسم المخصص
//  Canvas & CustomPaint System
// ═══════════════════════════════════════════════════════════════════════════════

#ifndef SAD_FLUTTER_CANVAS_H
#define SAD_FLUTTER_CANVAS_H

#include "sad_flutter_core.h"
#include "sad_flutter_widgets.h"
#include <algorithm>

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  Paint — فرشاة الرسم
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) نمط الرسم
 * @brief (EN) Paint style
 */
enum class PaintingStyle {
    Fill,
    Stroke
};

using نمط_رسم = PaintingStyle;

/**
 * @brief (AR) نهاية الخط
 * @brief (EN) Stroke cap
 */
enum class StrokeCap {
    Butt,
    Round,
    Square
};

using نهاية_خط = StrokeCap;

/**
 * @brief (AR) وصلة الخط
 * @brief (EN) Stroke join
 */
enum class StrokeJoin {
    Miter,
    Round,
    Bevel
};

using وصلة_خط = StrokeJoin;

/**
 * @brief (AR) وضع المزج
 * @brief (EN) Blend mode
 */
enum class BlendMode {
    Clear,
    Src,
    Dst,
    SrcOver,
    DstOver,
    SrcIn,
    DstIn,
    SrcOut,
    DstOut,
    SrcATop,
    DstATop,
    Xor,
    Plus,
    Modulate,
    Screen,
    Overlay,
    Darken,
    Lighten,
    ColorDodge,
    ColorBurn,
    HardLight,
    SoftLight,
    Difference,
    Exclusion,
    Multiply,
    Hue,
    Saturation,
    Color,
    Luminosity
};

using وضع_مزج = BlendMode;

/**
 * @brief (AR) فرشاة الرسم
 * @brief (EN) Paint object
 */
class Paint {
public:
    Color color = Color(0, 0, 0);
    PaintingStyle style = PaintingStyle::Fill;
    double strokeWidth = 1.0;
    StrokeCap strokeCap = StrokeCap::Butt;
    StrokeJoin strokeJoin = StrokeJoin::Miter;
    double strokeMiterLimit = 4.0;
    bool isAntiAlias = true;
    BlendMode blendMode = BlendMode::SrcOver;
    FilterQuality filterQuality = FilterQuality::None;
    bool invertColors = false;
    
    // Shader
    std::shared_ptr<class Shader> shader;
    
    // Color filter
    std::shared_ptr<class ColorFilter> colorFilter;
    
    // Mask filter (blur)
    std::shared_ptr<class MaskFilter> maskFilter;
    
    // Image filter
    std::shared_ptr<class ImageFilter> imageFilter;
    
    Paint() = default;
    
    Paint& setColor(Color c) { color = c; return *this; }
    Paint& setStyle(PaintingStyle s) { style = s; return *this; }
    Paint& setStrokeWidth(double w) { strokeWidth = w; return *this; }
    Paint& setStrokeCap(StrokeCap c) { strokeCap = c; return *this; }
    Paint& setStrokeJoin(StrokeJoin j) { strokeJoin = j; return *this; }
    Paint& setAntiAlias(bool aa) { isAntiAlias = aa; return *this; }
    Paint& setBlendMode(BlendMode m) { blendMode = m; return *this; }
};

using فرشاة = Paint;

/**
 * @brief (AR) جودة التصفية
 * @brief (EN) Filter quality
 */
enum class FilterQuality {
    None,
    Low,
    Medium,
    High
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Path — المسار
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) قاعدة تعبئة المسار
 * @brief (EN) Path fill type
 */
enum class PathFillType {
    NonZero,
    EvenOdd
};

using نوع_تعبئة_مسار = PathFillType;

/**
 * @brief (AR) عملية المسار
 * @brief (EN) Path operation
 */
enum class PathOperation {
    Difference,
    Intersect,
    Union,
    Xor,
    ReverseDifference
};

using عملية_مسار = PathOperation;

/**
 * @brief (AR) المسار الهندسي
 * @brief (EN) Path object
 */
class Path {
public:
    PathFillType fillType = PathFillType::NonZero;
    
    Path() = default;
    
    // حركة القلم
    Path& moveTo(double x, double y) {
        commands_.push_back({PathCommand::MoveTo, x, y, 0, 0, 0, 0});
        return *this;
    }
    
    Path& lineTo(double x, double y) {
        commands_.push_back({PathCommand::LineTo, x, y, 0, 0, 0, 0});
        return *this;
    }
    
    Path& relativeMoveTo(double dx, double dy) {
        commands_.push_back({PathCommand::RelativeMoveTo, dx, dy, 0, 0, 0, 0});
        return *this;
    }
    
    Path& relativeLineTo(double dx, double dy) {
        commands_.push_back({PathCommand::RelativeLineTo, dx, dy, 0, 0, 0, 0});
        return *this;
    }
    
    // منحنيات بيزير
    Path& quadraticBezierTo(double x1, double y1, double x2, double y2) {
        commands_.push_back({PathCommand::QuadraticBezierTo, x1, y1, x2, y2, 0, 0});
        return *this;
    }
    
    Path& cubicTo(double x1, double y1, double x2, double y2, double x3, double y3) {
        commands_.push_back({PathCommand::CubicTo, x1, y1, x2, y2, x3, y3});
        return *this;
    }
    
    Path& conicTo(double x1, double y1, double x2, double y2, double w) {
        commands_.push_back({PathCommand::ConicTo, x1, y1, x2, y2, w, 0});
        return *this;
    }
    
    // أقواس
    Path& arcTo(Rect rect, double startAngle, double sweepAngle, bool forceMoveTo) {
        // Simplified arc storage
        commands_.push_back({PathCommand::ArcTo, rect.left, rect.top, 
                            rect.width(), rect.height(), startAngle, sweepAngle});
        return *this;
    }
    
    Path& arcToPoint(Offset point, Radius radius, double rotation = 0,
                     bool largeArc = false, bool clockwise = true) {
        // Store arc to point
        commands_.push_back({PathCommand::ArcToPoint, point.dx, point.dy,
                            radius.x, radius.y, rotation, 
                            (largeArc ? 1.0 : 0.0) + (clockwise ? 2.0 : 0.0)});
        return *this;
    }
    
    // أشكال
    Path& addRect(Rect rect) {
        moveTo(rect.left, rect.top);
        lineTo(rect.right, rect.top);
        lineTo(rect.right, rect.bottom);
        lineTo(rect.left, rect.bottom);
        close();
        return *this;
    }
    
    Path& addRRect(RRect rrect) {
        // TODO: add rounded rect
        return *this;
    }
    
    Path& addOval(Rect rect) {
        commands_.push_back({PathCommand::AddOval, rect.left, rect.top,
                            rect.width(), rect.height(), 0, 0});
        return *this;
    }
    
    Path& addArc(Rect rect, double startAngle, double sweepAngle) {
        commands_.push_back({PathCommand::AddArc, rect.left, rect.top,
                            rect.width(), rect.height(), startAngle, sweepAngle});
        return *this;
    }
    
    Path& addPolygon(const std::vector<Offset>& points, bool close_) {
        if (points.empty()) return *this;
        moveTo(points[0].dx, points[0].dy);
        for (size_t i = 1; i < points.size(); ++i) {
            lineTo(points[i].dx, points[i].dy);
        }
        if (close_) close();
        return *this;
    }
    
    Path& addPath(const Path& path, Offset offset = Offset::zero()) {
        // TODO: add path commands with offset
        return *this;
    }
    
    Path& close() {
        commands_.push_back({PathCommand::Close, 0, 0, 0, 0, 0, 0});
        return *this;
    }
    
    Path& reset() {
        commands_.clear();
        return *this;
    }
    
    // عمليات المسار
    bool contains(Offset point) const {
        // TODO: implement point-in-path test
        return false;
    }
    
    Rect getBounds() const {
        // TODO: calculate bounds
        return Rect::zero();
    }
    
    static Path combine(PathOperation op, const Path& path1, const Path& path2) {
        // TODO: implement path operations
        return Path();
    }
    
    Path transform(const Matrix4& matrix) const {
        // TODO: transform path
        return *this;
    }
    
    Path shift(Offset offset) const {
        // TODO: shift path
        return *this;
    }
    
private:
    enum class PathCommand {
        MoveTo, LineTo, RelativeMoveTo, RelativeLineTo,
        QuadraticBezierTo, CubicTo, ConicTo,
        ArcTo, ArcToPoint,
        AddOval, AddArc,
        Close
    };
    
    struct PathData {
        PathCommand cmd;
        double x1, y1, x2, y2, x3, y3;
    };
    
    std::vector<PathData> commands_;
};

using مسار = Path;

// ═══════════════════════════════════════════════════════════════════════════════
//  Canvas — اللوحة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) محاذاة النقطة
 * @brief (EN) Point mode
 */
enum class PointMode {
    Points,
    Lines,
    Polygon
};

/**
 * @brief (AR) محاذاة الفقرة
 * @brief (EN) Clip operation
 */
enum class ClipOp {
    Difference,
    Intersect
};

/**
 * @brief (AR) لوحة الرسم
 * @brief (EN) Canvas for drawing
 */
class Canvas {
public:
    Canvas() = default;
    
    // الحفظ والاستعادة
    void save() { saveCount_++; }
    void saveLayer(Rect bounds, Paint paint) { saveCount_++; }
    void restore() { if (saveCount_ > 0) saveCount_--; }
    int getSaveCount() const { return saveCount_; }
    void restoreToCount(int count) { saveCount_ = std::max(0, count); }
    
    // التحويلات
    void translate(double dx, double dy) {
        // TODO: apply translation
    }
    
    void scale(double sx, double sy = -1) {
        if (sy < 0) sy = sx;
        // TODO: apply scale
    }
    
    void rotate(double radians) {
        // TODO: apply rotation
    }
    
    void skew(double sx, double sy) {
        // TODO: apply skew
    }
    
    void transform(const Matrix4& matrix) {
        // TODO: apply matrix
    }
    
    // القص
    void clipRect(Rect rect, ClipOp op = ClipOp::Intersect, bool doAntiAlias = true) {
        // TODO: clip to rect
    }
    
    void clipRRect(RRect rrect, bool doAntiAlias = true) {
        // TODO: clip to rounded rect
    }
    
    void clipPath(const Path& path, bool doAntiAlias = true) {
        // TODO: clip to path
    }
    
    // رسم الأشكال الأساسية
    void drawColor(Color color, BlendMode blendMode = BlendMode::SrcOver) {
        // TODO: fill with color
    }
    
    void drawLine(Offset p1, Offset p2, Paint paint) {
        // TODO: draw line
    }
    
    void drawPaint(Paint paint) {
        // TODO: fill canvas with paint
    }
    
    void drawRect(Rect rect, Paint paint) {
        // TODO: draw rectangle
    }
    
    void drawRRect(RRect rrect, Paint paint) {
        // TODO: draw rounded rectangle
    }
    
    void drawDRRect(RRect outer, RRect inner, Paint paint) {
        // TODO: draw difference of rounded rects
    }
    
    void drawOval(Rect rect, Paint paint) {
        // TODO: draw oval
    }
    
    void drawCircle(Offset center, double radius, Paint paint) {
        // TODO: draw circle
    }
    
    void drawArc(Rect rect, double startAngle, double sweepAngle, 
                 bool useCenter, Paint paint) {
        // TODO: draw arc
    }
    
    void drawPath(const Path& path, Paint paint) {
        // TODO: draw path
    }
    
    // رسم الصور
    void drawImage(std::shared_ptr<class Image> image, Offset offset, Paint paint) {
        // TODO: draw image
    }
    
    void drawImageRect(std::shared_ptr<class Image> image, 
                       Rect src, Rect dst, Paint paint) {
        // TODO: draw image rect
    }
    
    void drawImageNine(std::shared_ptr<class Image> image,
                       Rect center, Rect dst, Paint paint) {
        // TODO: draw nine-patch image
    }
    
    // رسم النقاط
    void drawPoints(PointMode mode, const std::vector<Offset>& points, Paint paint) {
        // TODO: draw points
    }
    
    // رسم الأطلس (الصور المجمعة)
    void drawAtlas(std::shared_ptr<class Image> atlas,
                   const std::vector<RSTransform>& transforms,
                   const std::vector<Rect>& rects,
                   const std::vector<Color>& colors,
                   BlendMode blendMode,
                   Rect cullRect,
                   Paint paint) {
        // TODO: draw atlas
    }
    
    // رسم الظلال
    void drawShadow(const Path& path, Color color, double elevation, 
                    bool transparentOccluder) {
        // TODO: draw shadow
    }
    
    // رسم الفقرات (النصوص)
    void drawParagraph(std::shared_ptr<class Paragraph> paragraph, Offset offset) {
        // TODO: draw paragraph
    }
    
    // رسم الرؤوس (للرسوميات المتقدمة)
    void drawVertices(std::shared_ptr<class Vertices> vertices, 
                      BlendMode blendMode, Paint paint) {
        // TODO: draw vertices
    }
    
private:
    int saveCount_ = 0;
};

using لوحة = Canvas;

/**
 * @brief (AR) تحويل RST
 * @brief (EN) RST transform
 */
struct RSTransform {
    double scos;  // scale * cos(rotation)
    double ssin;  // scale * sin(rotation)
    double tx;    // translation x
    double ty;    // translation y
    
    static RSTransform fromComponents(double rotation, double scale, 
                                       double anchorX, double anchorY,
                                       double translateX, double translateY) {
        double c = std::cos(rotation) * scale;
        double s = std::sin(rotation) * scale;
        return RSTransform{c, s, 
                          translateX + -c * anchorX + s * anchorY,
                          translateY + -s * anchorX - c * anchorY};
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//  CustomPainter — الرسام المخصص
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) الرسام المخصص (كلاس أساسي)
 * @brief (EN) Custom painter base class
 */
class CustomPainter {
public:
    virtual ~CustomPainter() = default;
    
    /**
     * @brief (AR) دالة الرسم - يجب تخطيها
     * @brief (EN) Paint method - must override
     */
    virtual void paint(Canvas& canvas, Size size) = 0;
    
    /**
     * @brief (AR) هل يجب إعادة الرسم؟
     * @brief (EN) Should repaint?
     */
    virtual bool shouldRepaint(const CustomPainter* oldDelegate) const {
        return true;
    }
    
    /**
     * @brief (AR) هل يجب إعادة بناء الدلالات؟
     * @brief (EN) Should rebuild semantics?
     */
    virtual bool shouldRebuildSemantics(const CustomPainter* oldDelegate) const {
        return shouldRepaint(oldDelegate);
    }
    
    /**
     * @brief (AR) اختبار الإصابة
     * @brief (EN) Hit test
     */
    virtual bool hitTest(Offset position) const {
        return true;
    }
    
    /**
     * @brief (AR) بناء الدلالات
     * @brief (EN) Build semantics
     */
    virtual void buildSemantics(class SemanticsBuilder& builder) {}
};

using رسام_مخصص = CustomPainter;

/**
 * @brief (AR) مكون الرسم المخصص
 * @brief (EN) CustomPaint widget
 */
class CustomPaint : public StatelessWidget {
public:
    std::shared_ptr<CustomPainter> painter;
    std::shared_ptr<CustomPainter> foregroundPainter;
    Size size = Size::zero();
    bool isComplex = false;
    bool willChange = false;
    WidgetPtr child;
    
    CustomPaint() = default;
    
    CustomPaint(std::shared_ptr<CustomPainter> p, Size s = Size::zero())
        : painter(p), size(s) {}
    
    WidgetPtr build(BuildContext* ctx) override {
        return child;
    }
};

using رسم_مخصص = CustomPaint;

// ═══════════════════════════════════════════════════════════════════════════════
//  RepaintBoundary — حدود إعادة الرسم
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) حدود إعادة الرسم
 * @brief (EN) Repaint boundary
 */
class RepaintBoundary : public StatelessWidget {
public:
    WidgetPtr child;
    
    RepaintBoundary(WidgetPtr c = nullptr) : child(c) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
    
    static std::shared_ptr<RepaintBoundary> wrap(WidgetPtr child, int childIndex) {
        auto boundary = std::make_shared<RepaintBoundary>(child);
        return boundary;
    }
};

using حدود_اعادة_رسم = RepaintBoundary;

// ═══════════════════════════════════════════════════════════════════════════════
//  Shaders — التظليل
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) المظلل الأساسي
 * @brief (EN) Base shader
 */
class Shader {
public:
    virtual ~Shader() = default;
};

using مظلل = Shader;

/**
 * @brief (AR) وضع البلاط
 * @brief (EN) Tile mode
 */
enum class TileMode {
    Clamp,
    Repeated,
    Mirror,
    Decal
};

using وضع_بلاط = TileMode;

/**
 * @brief (AR) تدرج خطي
 * @brief (EN) Linear gradient
 */
class LinearGradient : public Shader {
public:
    Offset from;
    Offset to;
    std::vector<Color> colors;
    std::vector<double> stops;
    TileMode tileMode = TileMode::Clamp;
    Matrix4 transform;
    
    LinearGradient(Offset f, Offset t, 
                   std::vector<Color> c,
                   std::vector<double> s = {},
                   TileMode tm = TileMode::Clamp)
        : from(f), to(t), colors(std::move(c)), stops(std::move(s)), tileMode(tm) {}
};

using تدرج_خطي = LinearGradient;

/**
 * @brief (AR) تدرج شعاعي
 * @brief (EN) Radial gradient
 */
class RadialGradient : public Shader {
public:
    Offset center;
    double radius;
    std::vector<Color> colors;
    std::vector<double> stops;
    TileMode tileMode = TileMode::Clamp;
    Offset focal;
    double focalRadius = 0.0;
    Matrix4 transform;
    
    RadialGradient(Offset c, double r,
                   std::vector<Color> cols,
                   std::vector<double> s = {},
                   TileMode tm = TileMode::Clamp)
        : center(c), radius(r), colors(std::move(cols)), stops(std::move(s)), tileMode(tm) {}
};

using تدرج_شعاعي = RadialGradient;

/**
 * @brief (AR) تدرج مخروطي
 * @brief (EN) Sweep gradient
 */
class SweepGradient : public Shader {
public:
    Offset center;
    std::vector<Color> colors;
    std::vector<double> stops;
    TileMode tileMode = TileMode::Clamp;
    double startAngle = 0.0;
    double endAngle = 6.28318530718;  // 2 * PI
    Matrix4 transform;
    
    SweepGradient(Offset c,
                  std::vector<Color> cols,
                  std::vector<double> s = {})
        : center(c), colors(std::move(cols)), stops(std::move(s)) {}
};

using تدرج_مخروطي = SweepGradient;

/**
 * @brief (AR) صورة كمظلل
 * @brief (EN) Image shader
 */
class ImageShader : public Shader {
public:
    std::shared_ptr<class Image> image;
    TileMode tmx = TileMode::Clamp;
    TileMode tmy = TileMode::Clamp;
    FilterQuality quality = FilterQuality::Low;
    Matrix4 transform;
    
    ImageShader(std::shared_ptr<class Image> img,
                TileMode tx = TileMode::Clamp,
                TileMode ty = TileMode::Clamp)
        : image(img), tmx(tx), tmy(ty) {}
};

using مظلل_صورة = ImageShader;

// ═══════════════════════════════════════════════════════════════════════════════
//  Filters — المرشحات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) مرشح اللون
 * @brief (EN) Color filter
 */
class ColorFilter {
public:
    virtual ~ColorFilter() = default;
    
    static std::shared_ptr<ColorFilter> mode(Color color, BlendMode blendMode) {
        return std::make_shared<ColorFilter>();
    }
    
    static std::shared_ptr<ColorFilter> matrix(const std::vector<double>& matrix) {
        return std::make_shared<ColorFilter>();
    }
    
    static std::shared_ptr<ColorFilter> linearToSrgbGamma() {
        return std::make_shared<ColorFilter>();
    }
    
    static std::shared_ptr<ColorFilter> srgbToLinearGamma() {
        return std::make_shared<ColorFilter>();
    }
};

using مرشح_لون = ColorFilter;

/**
 * @brief (AR) مرشح القناع (الضبابية)
 * @brief (EN) Mask filter (blur)
 */
class MaskFilter {
public:
    enum class BlurStyle {
        Normal,
        Solid,
        Outer,
        Inner
    };
    
    BlurStyle style = BlurStyle::Normal;
    double sigma = 0.0;
    
    static std::shared_ptr<MaskFilter> blur(BlurStyle style, double sigma) {
        auto filter = std::make_shared<MaskFilter>();
        filter->style = style;
        filter->sigma = sigma;
        return filter;
    }
};

using مرشح_قناع = MaskFilter;

/**
 * @brief (AR) مرشح الصورة
 * @brief (EN) Image filter
 */
class ImageFilter {
public:
    virtual ~ImageFilter() = default;
    
    static std::shared_ptr<ImageFilter> blur(double sigmaX, double sigmaY,
                                              TileMode tm = TileMode::Clamp) {
        return std::make_shared<ImageFilter>();
    }
    
    static std::shared_ptr<ImageFilter> dilate(double radiusX, double radiusY) {
        return std::make_shared<ImageFilter>();
    }
    
    static std::shared_ptr<ImageFilter> erode(double radiusX, double radiusY) {
        return std::make_shared<ImageFilter>();
    }
    
    static std::shared_ptr<ImageFilter> matrix(const Matrix4& matrix,
                                                FilterQuality quality = FilterQuality::Low) {
        return std::make_shared<ImageFilter>();
    }
    
    static std::shared_ptr<ImageFilter> compose(std::shared_ptr<ImageFilter> outer,
                                                 std::shared_ptr<ImageFilter> inner) {
        return std::make_shared<ImageFilter>();
    }
};

using مرشح_صورة = ImageFilter;

// ═══════════════════════════════════════════════════════════════════════════════
//  ClipPath Widget — مكون القص بالمسار
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) مكون القص بالمسار
 * @brief (EN) ClipPath widget
 */
class ClipPath : public StatelessWidget {
public:
    std::function<Path(Size)> clipper;
    Clip clipBehavior = Clip::AntiAlias;
    WidgetPtr child;
    
    ClipPath(std::function<Path(Size)> c = nullptr, WidgetPtr ch = nullptr)
        : clipper(c), child(ch) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using قص_مسار = ClipPath;

/**
 * @brief (AR) مكون القص بمستطيل
 * @brief (EN) ClipRect widget
 */
class ClipRect : public StatelessWidget {
public:
    std::function<Rect(Size)> clipper;
    Clip clipBehavior = Clip::HardEdge;
    WidgetPtr child;
    
    ClipRect(WidgetPtr ch = nullptr) : child(ch) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using قص_مستطيل = ClipRect;

/**
 * @brief (AR) مكون القص بمستطيل دائري
 * @brief (EN) ClipRRect widget
 */
class ClipRRect : public StatelessWidget {
public:
    BorderRadius borderRadius = BorderRadius::zero();
    std::function<RRect(Size)> clipper;
    Clip clipBehavior = Clip::AntiAlias;
    WidgetPtr child;
    
    ClipRRect(BorderRadius br = BorderRadius::zero(), WidgetPtr ch = nullptr)
        : borderRadius(br), child(ch) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using قص_مستطيل_دائري = ClipRRect;

/**
 * @brief (AR) مكون القص بشكل بيضاوي
 * @brief (EN) ClipOval widget
 */
class ClipOval : public StatelessWidget {
public:
    std::function<Rect(Size)> clipper;
    Clip clipBehavior = Clip::AntiAlias;
    WidgetPtr child;
    
    ClipOval(WidgetPtr ch = nullptr) : child(ch) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using قص_بيضاوي = ClipOval;

// ═══════════════════════════════════════════════════════════════════════════════
//  BackdropFilter — مرشح الخلفية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) مرشح الخلفية
 * @brief (EN) Backdrop filter
 */
class BackdropFilter : public StatelessWidget {
public:
    std::shared_ptr<ImageFilter> filter;
    BlendMode blendMode = BlendMode::SrcOver;
    WidgetPtr child;
    
    BackdropFilter(std::shared_ptr<ImageFilter> f = nullptr, WidgetPtr ch = nullptr)
        : filter(f), child(ch) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using مرشح_خلفية = BackdropFilter;

// ═══════════════════════════════════════════════════════════════════════════════
//  Transform — التحويلات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) مكون التحويل
 * @brief (EN) Transform widget
 */
class Transform : public StatelessWidget {
public:
    Matrix4 transform_;
    Offset origin = Offset::zero();
    AlignmentGeometry alignment = Alignment::center();
    bool transformHitTests = true;
    FilterQuality filterQuality;
    WidgetPtr child;
    
    Transform(Matrix4 t = Matrix4::identity(), WidgetPtr ch = nullptr)
        : transform_(t), child(ch) {}
    
    static std::shared_ptr<Transform> rotate(double angle, 
                                              Offset origin = Offset::zero(),
                                              AlignmentGeometry alignment = Alignment::center(),
                                              WidgetPtr child = nullptr) {
        auto t = std::make_shared<Transform>();
        t->transform_ = Matrix4::rotationZ(angle);
        t->origin = origin;
        t->alignment = alignment;
        t->child = child;
        return t;
    }
    
    static std::shared_ptr<Transform> translate(Offset offset, WidgetPtr child = nullptr) {
        auto t = std::make_shared<Transform>();
        t->transform_ = Matrix4::translationValues(offset.dx, offset.dy, 0);
        t->child = child;
        return t;
    }
    
    static std::shared_ptr<Transform> scale(double scaleX, double scaleY = -1,
                                            Offset origin = Offset::zero(),
                                            AlignmentGeometry alignment = Alignment::center(),
                                            WidgetPtr child = nullptr) {
        if (scaleY < 0) scaleY = scaleX;
        auto t = std::make_shared<Transform>();
        t->transform_ = Matrix4::diagonal3Values(scaleX, scaleY, 1.0);
        t->origin = origin;
        t->alignment = alignment;
        t->child = child;
        return t;
    }
    
    static std::shared_ptr<Transform> flip(bool x = false, bool y = false,
                                           Offset origin = Offset::zero(),
                                           WidgetPtr child = nullptr) {
        return scale(x ? -1.0 : 1.0, y ? -1.0 : 1.0, origin, Alignment::center(), child);
    }
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using تحويل = Transform;

// ═══════════════════════════════════════════════════════════════════════════════
//  DecoratedBox — صندوق مزخرف
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) صندوق مزخرف
 * @brief (EN) Decorated box
 */
class DecoratedBox : public StatelessWidget {
public:
    BoxDecoration decoration;
    DecorationPosition position = DecorationPosition::Background;
    WidgetPtr child;
    
    DecoratedBox(BoxDecoration dec, WidgetPtr ch = nullptr)
        : decoration(dec), child(ch) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using صندوق_مزخرف = DecoratedBox;

/**
 * @brief (AR) موضع الزخرفة
 * @brief (EN) Decoration position
 */
enum class DecorationPosition {
    Background,
    Foreground
};

// ═══════════════════════════════════════════════════════════════════════════════
//  رسامون جاهزون
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) رسام دائرة
 * @brief (EN) Circle painter
 */
class CirclePainter : public CustomPainter {
public:
    Color color;
    double radius;
    bool filled;
    double strokeWidth;
    
    CirclePainter(Color c = Color(0, 0, 255), double r = 50.0, 
                  bool f = true, double sw = 2.0)
        : color(c), radius(r), filled(f), strokeWidth(sw) {}
    
    void paint(Canvas& canvas, Size size) override {
        Paint paint;
        paint.color = color;
        paint.style = filled ? PaintingStyle::Fill : PaintingStyle::Stroke;
        paint.strokeWidth = strokeWidth;
        
        Offset center{size.width / 2, size.height / 2};
        canvas.drawCircle(center, radius, paint);
    }
    
    bool shouldRepaint(const CustomPainter* old) const override {
        auto* oldP = dynamic_cast<const CirclePainter*>(old);
        if (!oldP) return true;
        return color != oldP->color || radius != oldP->radius || 
               filled != oldP->filled || strokeWidth != oldP->strokeWidth;
    }
};

using رسام_دائرة = CirclePainter;

/**
 * @brief (AR) رسام مخطط
 * @brief (EN) Chart painter
 */
class ChartPainter : public CustomPainter {
public:
    std::vector<double> data;
    Color lineColor;
    Color fillColor;
    double strokeWidth;
    bool showFill;
    
    ChartPainter(std::vector<double> d = {}, 
                 Color lc = Color(0, 100, 255),
                 Color fc = Color(0, 100, 255, 50),
                 double sw = 2.0,
                 bool sf = true)
        : data(std::move(d)), lineColor(lc), fillColor(fc), 
          strokeWidth(sw), showFill(sf) {}
    
    void paint(Canvas& canvas, Size size) override {
        if (data.empty()) return;
        
        double maxVal = *std::max_element(data.begin(), data.end());
        if (maxVal == 0) maxVal = 1;
        
        double stepX = size.width / (data.size() - 1);
        
        Path linePath;
        Path fillPath;
        
        for (size_t i = 0; i < data.size(); ++i) {
            double x = i * stepX;
            double y = size.height - (data[i] / maxVal * size.height);
            
            if (i == 0) {
                linePath.moveTo(x, y);
                fillPath.moveTo(x, size.height);
                fillPath.lineTo(x, y);
            } else {
                linePath.lineTo(x, y);
                fillPath.lineTo(x, y);
            }
        }
        
        // Fill
        if (showFill) {
            fillPath.lineTo(size.width, size.height);
            fillPath.close();
            
            Paint fillPaint;
            fillPaint.color = fillColor;
            fillPaint.style = PaintingStyle::Fill;
            canvas.drawPath(fillPath, fillPaint);
        }
        
        // Line
        Paint linePaint;
        linePaint.color = lineColor;
        linePaint.style = PaintingStyle::Stroke;
        linePaint.strokeWidth = strokeWidth;
        linePaint.strokeCap = StrokeCap::Round;
        linePaint.strokeJoin = StrokeJoin::Round;
        canvas.drawPath(linePath, linePaint);
    }
    
    bool shouldRepaint(const CustomPainter* old) const override {
        return true;  // Always repaint for simplicity
    }
};

using رسام_مخطط = ChartPainter;

} // namespace flutter
} // namespace sad

#endif // SAD_FLUTTER_CANVAS_H
