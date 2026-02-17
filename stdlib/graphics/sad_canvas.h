// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// sad_canvas.h — واجهة الرسم المخصص (Custom Painting API)
// ─────────────────────────────────────────────────────────────────────────────
// يوفر واجهة رسم كاملة مستوحاة من Flutter Canvas + CustomPainter:
//   - رسم الأشكال: خط، مستطيل، دائرة، قوس، بيضاوي
//   - المسارات: bezier curves, paths, polygons
//   - التدرجات: خطي، شعاعي، مخروطي
//   - الظلال والتأثيرات: shadow, blur, glow
//   - تحويلات: translate, rotate, scale, save/restore
//   - الصور: تحميل ورسم صور (SDL_Surface)
//   - النص: رسم نص بأحجام وألوان مخصصة
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once
#include <string>
#include <vector>
#include <functional>

// إعلان مسبق
struct SDL_Renderer;

namespace sad { namespace canvas {

// ═══════════════════════════════════════════════════════════════════
// أنماط الرسم
// ═══════════════════════════════════════════════════════════════════

// وضع الرسم
enum class PaintStyle {
    Fill,       // تعبئة
    Stroke,     // حدود فقط
    FillAndStroke  // تعبئة + حدود
};

// نهاية الخط
enum class StrokeCap {
    Butt,       // مقطوع — بدون امتداد
    Round,      // دائري
    Square      // مربع — يمتد بمقدار نصف العرض
};

// وصلة الخطوط
enum class StrokeJoin {
    Miter,      // حاد
    Round,      // دائري
    Bevel       // مقطوع
};

// نوع التدرج
enum class GradientType {
    Linear,     // خطي — من نقطة إلى نقطة
    Radial,     // شعاعي — من المركز للخارج
    Sweep       // مخروطي — يدور حول المركز
};

// ═══════════════════════════════════════════════════════════════════
// هياكل البيانات
// ═══════════════════════════════════════════════════════════════════

struct Color {
    int r = 0, g = 0, b = 0, a = 255;
};

struct Point {
    float x = 0, y = 0;
};

struct Rect {
    float x = 0, y = 0, w = 0, h = 0;
};

struct Paint {
    Color color = {0, 0, 0, 255};       // لون الرسم
    PaintStyle style = PaintStyle::Fill; // نمط الرسم
    float strokeWidth = 1.0f;           // عرض الحدود
    StrokeCap strokeCap = StrokeCap::Round;
    StrokeJoin strokeJoin = StrokeJoin::Round;
    bool antiAlias = true;              // تنعيم الحواف
    float opacity = 1.0f;              // شفافية إضافية (0-1)
    
    // الظل
    bool hasShadow = false;
    Color shadowColor = {0, 0, 0, 80};
    float shadowOffsetX = 2;
    float shadowOffsetY = 2;
    float shadowBlur = 4;
};

struct Gradient {
    GradientType type = GradientType::Linear;
    // نقاط التدرج الخطي
    float startX = 0, startY = 0;
    float endX = 0, endY = 0;
    // مركز ونصف قطر التدرج الشعاعي
    float centerX = 0, centerY = 0;
    float radius = 100;
    // ألوان خطوات التدرج
    std::vector<Color> colors;
    std::vector<float> stops;  // مواضع الألوان (0-1)
};

// ═══════════════════════════════════════════════════════════════════
// معرّف القماش (Canvas ID) — كل قماش مرتبط بودجت Canvas
// ═══════════════════════════════════════════════════════════════════

// إنشاء سياق رسم لودجت Canvas
int createCanvas(int widgetId);

// تعيين دالة الرسم — تُستدعى كل إطار عند الحاجة لإعادة الرسم
using DrawCallback = std::function<void(int canvasId)>;
void setDrawCallback(int canvasId, DrawCallback callback);

// طلب إعادة رسم القماش
void invalidate(int canvasId);

// ═══════════════════════════════════════════════════════════════════
// رسم الأشكال الأساسية
// ═══════════════════════════════════════════════════════════════════

// رسم خط من نقطة إلى نقطة
void drawLine(int canvasId, float x1, float y1, float x2, float y2, const Paint& paint);

// رسم خط بإعدادات بسيطة (لون وعرض)
void drawLineSimple(int canvasId, float x1, float y1, float x2, float y2,
                    int r, int g, int b, float width = 1.0f);

// رسم مستطيل
void drawRect(int canvasId, float x, float y, float w, float h, const Paint& paint);

// رسم مستطيل بزوايا مدورة
void drawRoundRect(int canvasId, float x, float y, float w, float h,
                   float radius, const Paint& paint);

// رسم دائرة
void drawCircle(int canvasId, float cx, float cy, float radius, const Paint& paint);

// رسم بيضاوي (Ellipse)
void drawEllipse(int canvasId, float cx, float cy, float rx, float ry, const Paint& paint);

// رسم قوس
void drawArc(int canvasId, float cx, float cy, float radius,
             float startAngle, float sweepAngle, const Paint& paint);

// رسم مثلث
void drawTriangle(int canvasId, float x1, float y1, float x2, float y2,
                  float x3, float y3, const Paint& paint);

// رسم مضلع (Polygon)
void drawPolygon(int canvasId, const std::vector<Point>& points, const Paint& paint);

// رسم نقطة
void drawPoint(int canvasId, float x, float y, float size, const Paint& paint);

// ═══════════════════════════════════════════════════════════════════
// رسم التدرجات
// ═══════════════════════════════════════════════════════════════════

// رسم مستطيل بتدرج
void drawGradientRect(int canvasId, float x, float y, float w, float h,
                      const Gradient& gradient);

// رسم دائرة بتدرج شعاعي
void drawGradientCircle(int canvasId, float cx, float cy, float radius,
                        const Gradient& gradient);

// ═══════════════════════════════════════════════════════════════════
// المسارات (Paths) — لرسم أشكال معقدة
// ═══════════════════════════════════════════════════════════════════

// إنشاء مسار جديد وإرجاع معرّفه
int createPath();

// بدء المسار من نقطة
void pathMoveTo(int pathId, float x, float y);

// خط مستقيم إلى نقطة
void pathLineTo(int pathId, float x, float y);

// منحنى بيزييه تربيعي (نقطة تحكم + نقطة نهاية)
void pathQuadTo(int pathId, float cx, float cy, float x, float y);

// منحنى بيزييه تكعيبي (نقطتا تحكم + نقطة نهاية)
void pathCubicTo(int pathId, float cx1, float cy1, float cx2, float cy2, float x, float y);

// قوس
void pathArcTo(int pathId, float cx, float cy, float radius,
               float startAngle, float sweepAngle);

// إغلاق المسار (وصل النهاية بالبداية)
void pathClose(int pathId);

// رسم المسار
void drawPath(int canvasId, int pathId, const Paint& paint);

// حذف مسار
void deletePath(int pathId);

// ═══════════════════════════════════════════════════════════════════
// تحويلات (Transforms) — تحريك، تدوير، تكبير
// ═══════════════════════════════════════════════════════════════════

// حفظ حالة التحويل الحالية (للاستعادة لاحقاً)
void save(int canvasId);

// استعادة آخر حالة محفوظة
void restore(int canvasId);

// إزاحة نقطة الأصل
void translate(int canvasId, float dx, float dy);

// تدوير حول نقطة الأصل الحالية (بالدرجات)
void rotate(int canvasId, float degrees);

// تدوير حول نقطة محددة
void rotateAt(int canvasId, float degrees, float px, float py);

// تكبير/تصغير
void scale(int canvasId, float sx, float sy);

// إعادة تعيين التحويلات
void resetTransform(int canvasId);

// ═══════════════════════════════════════════════════════════════════
// القصاصة (Clipping) — تحديد منطقة الرسم
// ═══════════════════════════════════════════════════════════════════

// قص بمستطيل — فقط ما بداخله يظهر
void clipRect(int canvasId, float x, float y, float w, float h);

// قص بدائرة
void clipCircle(int canvasId, float cx, float cy, float radius);

// إزالة القص
void resetClip(int canvasId);

// ═══════════════════════════════════════════════════════════════════
// رسم النص
// ═══════════════════════════════════════════════════════════════════

// رسم نص في موضع
void drawText(int canvasId, const std::string& text, float x, float y,
              float fontSize, const Paint& paint);

// قياس عرض النص
float measureText(const std::string& text, float fontSize);

// ═══════════════════════════════════════════════════════════════════
// أدوات مساعدة
// ═══════════════════════════════════════════════════════════════════

// مسح القماش بلون
void clear(int canvasId, int r, int g, int b, int a = 255);

// رسم جميع أوامر القماش على SDL_Renderer
// يُستدعى من renderWidget() عندما يكون نوع الودجت Canvas
void renderCanvas(int canvasId, SDL_Renderer* renderer, float x, float y, float w, float h);

// تنظيف جميع القماشات
void clearAllCanvases();

}} // namespace sad::canvas
