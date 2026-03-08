// ============================================================================
// renderer2d_advanced.cpp — تنفيذ الرسم المتقدم لغة ص
// Advanced 2D Rendering Implementation for Sad Language
// ============================================================================
//
// الوصف (AR):
//   يُنفّذ هذا الملف الأشكال المتقدمة ومعالجة الصور:
//   - منحنيات بزيه (تربيعية وتكعيبية) + مسارات + B-Spline
//   - مصفوفات التحويل (ترجمة، دوران، تكبير، قص)
//   - الظلال المسقطة (Drop Shadows)
//   - التدرجات اللونية (خطي، شعاعي، مخروطي)
//   - مخزن البكسل (تحكم كامل بكل بكسل)
//   - الفلاتر: ضبابية Gaussian، توهج، تباين، سطوع، رمادي
//
// Description (EN):
//   Implements advanced shapes and image processing:
//   - Bézier curves (quadratic, cubic) + paths + B-Spline
//   - 2D transform matrices (translate, rotate, scale, shear)
//   - Drop shadows with blur
//   - Color gradients (linear, radial, conic)
//   - Pixel buffer (per-pixel control)
//   - Filters: Gaussian blur, glow, contrast, brightness, grayscale
//
// المهندس: كامل — أبريل 2026 / Engineer: Kamel — April 2026
// ============================================================================

#include "../include/rendering/renderer2d.h"
#include <cmath>
#include <algorithm>
#include <cstring>
#include <numeric>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace SadGraphics {

// ============================================================================
// Transform2D — مصفوفة التحويل / Transform Matrix
// ============================================================================

Transform2D::Transform2D() {
    // مصفوفة هوية / Identity matrix
    m[0][0] = 1; m[0][1] = 0; m[0][2] = 0;
    m[1][0] = 0; m[1][1] = 1; m[1][2] = 0;
    m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;
}

Transform2D Transform2D::ترجمة(Float32 tx, Float32 ty) {
    Transform2D t;
    t.m[0][2] = tx;
    t.m[1][2] = ty;
    return t;
}

Transform2D Transform2D::دوران(Float32 angle) {
    Transform2D t;
    Float32 c = std::cos(angle);
    Float32 s = std::sin(angle);
    t.m[0][0] = c;  t.m[0][1] = -s;
    t.m[1][0] = s;  t.m[1][1] = c;
    return t;
}

Transform2D Transform2D::تكبير(Float32 sx, Float32 sy) {
    Transform2D t;
    t.m[0][0] = sx;
    t.m[1][1] = sy;
    return t;
}

Transform2D Transform2D::قص(Float32 shx, Float32 shy) {
    Transform2D t;
    t.m[0][1] = shx;
    t.m[1][0] = shy;
    return t;
}

Transform2D Transform2D::operator*(const Transform2D& other) const {
    Transform2D result;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result.m[i][j] = 0;
            for (int k = 0; k < 3; ++k) {
                result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
    }
    return result;
}

void Transform2D::حوّل(Float32& x, Float32& y) const {
    Float32 nx = m[0][0] * x + m[0][1] * y + m[0][2];
    Float32 ny = m[1][0] * x + m[1][1] * y + m[1][2];
    x = nx;
    y = ny;
}

Transform2D Transform2D::معكوس() const {
    // حساب المحدد / Calculate determinant
    Float32 det = m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1])
                - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
                + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    
    if (std::abs(det) < 1e-9f) return Transform2D(); // مصفوفة هوية إذا غير قابلة للعكس
    
    Transform2D inv;
    Float32 invDet = 1.0f / det;
    
    inv.m[0][0] =  (m[1][1] * m[2][2] - m[1][2] * m[2][1]) * invDet;
    inv.m[0][1] = -(m[0][1] * m[2][2] - m[0][2] * m[2][1]) * invDet;
    inv.m[0][2] =  (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invDet;
    inv.m[1][0] = -(m[1][0] * m[2][2] - m[1][2] * m[2][0]) * invDet;
    inv.m[1][1] =  (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invDet;
    inv.m[1][2] = -(m[0][0] * m[1][2] - m[0][2] * m[1][0]) * invDet;
    inv.m[2][0] =  (m[1][0] * m[2][1] - m[1][1] * m[2][0]) * invDet;
    inv.m[2][1] = -(m[0][0] * m[2][1] - m[0][1] * m[2][0]) * invDet;
    inv.m[2][2] =  (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * invDet;
    
    return inv;
}

// ============================================================================
// مخزن_البكسل — تحكم كامل بالبكسل / Pixel Buffer
// ============================================================================

مخزن_البكسل::مخزن_البكسل(UInt32 عرض, UInt32 ارتفاع)
    : العرض_(عرض), الارتفاع_(ارتفاع)
    , بيانات_(عرض * ارتفاع * 4, 0)  // RGBA × حجم
    , معرّف_texture_(0), متسخ_(true)
{
}

مخزن_البكسل::~مخزن_البكسل() {
    // OpenGL texture cleanup will be handled by context
}

void مخزن_البكسل::عيّن_بكسل(UInt32 x, UInt32 y, const Color& color) {
    if (x >= العرض_ || y >= الارتفاع_) return;
    size_t i = (y * العرض_ + x) * 4;
    // تحويل Float32 (0.0-1.0) إلى uint8_t (0-255)
    بيانات_[i + 0] = static_cast<uint8_t>(std::clamp(color.r * 255.0f, 0.0f, 255.0f));
    بيانات_[i + 1] = static_cast<uint8_t>(std::clamp(color.g * 255.0f, 0.0f, 255.0f));
    بيانات_[i + 2] = static_cast<uint8_t>(std::clamp(color.b * 255.0f, 0.0f, 255.0f));
    بيانات_[i + 3] = static_cast<uint8_t>(std::clamp(color.a * 255.0f, 0.0f, 255.0f));
    متسخ_ = true;
}

Color مخزن_البكسل::اقرأ_بكسل(UInt32 x, UInt32 y) const {
    if (x >= العرض_ || y >= الارتفاع_) return Color(0.0f, 0.0f, 0.0f, 0.0f);
    size_t i = (y * العرض_ + x) * 4;
    // تحويل uint8_t (0-255) إلى Float32 (0.0-1.0)
    return Color(بيانات_[i] / 255.0f, بيانات_[i + 1] / 255.0f,
                 بيانات_[i + 2] / 255.0f, بيانات_[i + 3] / 255.0f);
}

void مخزن_البكسل::املأ(const Color& color) {
    uint8_t r = static_cast<uint8_t>(std::clamp(color.r * 255.0f, 0.0f, 255.0f));
    uint8_t g = static_cast<uint8_t>(std::clamp(color.g * 255.0f, 0.0f, 255.0f));
    uint8_t b = static_cast<uint8_t>(std::clamp(color.b * 255.0f, 0.0f, 255.0f));
    uint8_t a = static_cast<uint8_t>(std::clamp(color.a * 255.0f, 0.0f, 255.0f));
    for (size_t i = 0; i < بيانات_.size(); i += 4) {
        بيانات_[i + 0] = r;
        بيانات_[i + 1] = g;
        بيانات_[i + 2] = b;
        بيانات_[i + 3] = a;
    }
    متسخ_ = true;
}

void مخزن_البكسل::امسح() {
    std::fill(بيانات_.begin(), بيانات_.end(), 0);
    متسخ_ = true;
}

void مخزن_البكسل::امزج_بكسل(UInt32 x, UInt32 y, const Color& color) {
    if (x >= العرض_ || y >= الارتفاع_) return;
    
    size_t i = (y * العرض_ + x) * 4;
    // Color.a is already 0.0-1.0
    Float32 alpha = color.a;
    Float32 invAlpha = 1.0f - alpha;
    
    Float32 srcR = color.r * 255.0f;
    Float32 srcG = color.g * 255.0f;
    Float32 srcB = color.b * 255.0f;
    Float32 srcA = color.a * 255.0f;
    
    بيانات_[i + 0] = static_cast<uint8_t>(std::clamp(srcR * alpha + بيانات_[i + 0] * invAlpha, 0.0f, 255.0f));
    بيانات_[i + 1] = static_cast<uint8_t>(std::clamp(srcG * alpha + بيانات_[i + 1] * invAlpha, 0.0f, 255.0f));
    بيانات_[i + 2] = static_cast<uint8_t>(std::clamp(srcB * alpha + بيانات_[i + 2] * invAlpha, 0.0f, 255.0f));
    بيانات_[i + 3] = static_cast<uint8_t>(std::clamp(بيانات_[i + 3] + srcA * alpha, 0.0f, 255.0f));
    متسخ_ = true;
}

void مخزن_البكسل::انسخ_منطقة(const مخزن_البكسل& مصدر,
                               UInt32 مصدر_س, UInt32 مصدر_ص,
                               UInt32 عرض, UInt32 ارتفاع,
                               UInt32 هدف_س, UInt32 هدف_ص) {
    for (UInt32 y = 0; y < ارتفاع; ++y) {
        for (UInt32 x = 0; x < عرض; ++x) {
            UInt32 sx = مصدر_س + x;
            UInt32 sy = مصدر_ص + y;
            UInt32 dx = هدف_س + x;
            UInt32 dy = هدف_ص + y;
            
            if (sx < مصدر.العرض_ && sy < مصدر.الارتفاع_ &&
                dx < العرض_ && dy < الارتفاع_) {
                size_t si = (sy * مصدر.العرض_ + sx) * 4;
                size_t di = (dy * العرض_ + dx) * 4;
                std::memcpy(&بيانات_[di], &مصدر.بيانات_[si], 4);
            }
        }
    }
    متسخ_ = true;
}

void مخزن_البكسل::ارسم_إلى_الشاشة(Renderer2D& renderer, Float32 x, Float32 y) {
    // سيتم تحويل المخزن إلى OpenGL texture ورسمه
    // Will be converted to OpenGL texture and drawn
    // التنفيذ يعتمد على الـ texture system الموجود
    // Implementation depends on existing texture system
    (void)renderer; (void)x; (void)y;
    // TODO: ربط مع نظام الـ Textures الموجود
}

// ============================================================================
// Renderer2DAdvanced — منحنيات بزيه / Bézier Curves
// ============================================================================

Renderer2DAdvanced::Renderer2DAdvanced(Renderer2D* renderer)
    : المحرك_(renderer)
{
    الظل_الحالي_.مفعّل = false;
}

// ────────────────────────────────────────────────────────────
// منحنى بزيه تربيعي / Quadratic Bézier
// ────────────────────────────────────────────────────────────
void Renderer2DAdvanced::DrawBezierQuadratic(
    Float32 x0, Float32 y0,
    Float32 x1, Float32 y1,
    Float32 x2, Float32 y2,
    const Color& color, Float32 thickness, Int32 segments)
{
    if (!المحرك_ || segments < 2) return;
    
    Float32 prevX = x0, prevY = y0;
    
    for (Int32 i = 1; i <= segments; ++i) {
        Float32 t = static_cast<Float32>(i) / segments;
        Float32 u = 1.0f - t;
        
        // B(t) = (1-t)²·P0 + 2(1-t)t·P1 + t²·P2
        Float32 cx = u * u * x0 + 2 * u * t * x1 + t * t * x2;
        Float32 cy = u * u * y0 + 2 * u * t * y1 + t * t * y2;
        
        المحرك_->DrawLine(prevX, prevY, cx, cy, color, thickness);
        prevX = cx;
        prevY = cy;
    }
}

// ────────────────────────────────────────────────────────────
// منحنى بزيه تكعيبي / Cubic Bézier
// ────────────────────────────────────────────────────────────
void Renderer2DAdvanced::DrawBezierCubic(
    Float32 x0, Float32 y0,
    Float32 x1, Float32 y1,
    Float32 x2, Float32 y2,
    Float32 x3, Float32 y3,
    const Color& color, Float32 thickness, Int32 segments)
{
    if (!المحرك_ || segments < 2) return;
    
    Float32 prevX = x0, prevY = y0;
    
    for (Int32 i = 1; i <= segments; ++i) {
        Float32 t = static_cast<Float32>(i) / segments;
        Float32 u = 1.0f - t;
        Float32 u2 = u * u;
        Float32 u3 = u2 * u;
        Float32 t2 = t * t;
        Float32 t3 = t2 * t;
        
        // B(t) = (1-t)³·P0 + 3(1-t)²t·P1 + 3(1-t)t²·P2 + t³·P3
        Float32 cx = u3 * x0 + 3 * u2 * t * x1 + 3 * u * t2 * x2 + t3 * x3;
        Float32 cy = u3 * y0 + 3 * u2 * t * y1 + 3 * u * t2 * y2 + t3 * y3;
        
        المحرك_->DrawLine(prevX, prevY, cx, cy, color, thickness);
        prevX = cx;
        prevY = cy;
    }
}

// ────────────────────────────────────────────────────────────
// مسار بزيه / Bézier Path
// ────────────────────────────────────────────────────────────
void Renderer2DAdvanced::DrawBezierPath(
    const std::vector<Point2D>& controlPoints,
    const Color& color, Float32 thickness, Int32 segmentsPerCurve)
{
    if (!المحرك_ || controlPoints.size() < 4) return;
    
    // كل 4 نقاط = منحنى تكعيبي
    // Every 4 points = cubic curve
    for (size_t i = 0; i + 3 < controlPoints.size(); i += 3) {
        DrawBezierCubic(
            controlPoints[i].x, controlPoints[i].y,
            controlPoints[i + 1].x, controlPoints[i + 1].y,
            controlPoints[i + 2].x, controlPoints[i + 2].y,
            controlPoints[i + 3].x, controlPoints[i + 3].y,
            color, thickness, segmentsPerCurve);
    }
}

// ────────────────────────────────────────────────────────────
// B-Spline
// ────────────────────────────────────────────────────────────
void Renderer2DAdvanced::DrawBSpline(
    const std::vector<Point2D>& controlPoints,
    Int32 degree, const Color& color, Float32 thickness, Int32 segments)
{
    if (!المحرك_ || controlPoints.size() < 2) return;
    
    Int32 n = static_cast<Int32>(controlPoints.size()) - 1;
    Int32 p = std::min(degree, n);
    
    // عقد موحدة / Uniform knots
    std::vector<Float32> knots(n + p + 2);
    for (int i = 0; i < static_cast<int>(knots.size()); ++i) {
        knots[i] = static_cast<Float32>(i);
    }
    
    Float32 tMin = knots[p];
    Float32 tMax = knots[n + 1];
    
    Float32 prevX = 0, prevY = 0;
    bool first = true;
    
    for (Int32 i = 0; i <= segments; ++i) {
        Float32 t = tMin + (tMax - tMin) * static_cast<Float32>(i) / segments;
        
        // خوارزمية De Boor
        Float32 x = 0, y = 0;
        for (Int32 j = 0; j <= n; ++j) {
            // أساس B-Spline مبسط — الدرجة 0
            Float32 basis = 0;
            if (t >= knots[j] && t < knots[j + 1]) basis = 1.0f;
            
            // البناء التدريجي للدرجات الأعلى
            for (Int32 k = 1; k <= p; ++k) {
                Float32 left = 0, right = 0;
                Float32 d1 = knots[j + k] - knots[j];
                Float32 d2 = knots[j + k + 1] - knots[j + 1];
                if (d1 > 0) left = (t - knots[j]) / d1;
                if (d2 > 0) right = (knots[j + k + 1] - t) / d2;
                basis = left * basis + right * basis;
            }
            
            x += basis * controlPoints[j].x;
            y += basis * controlPoints[j].y;
        }
        
        if (!first) {
            المحرك_->DrawLine(prevX, prevY, x, y, color, thickness);
        }
        prevX = x;
        prevY = y;
        first = false;
    }
}

// ============================================================================
// مستطيل دائري الزوايا / Rounded Rectangle
// ============================================================================
void Renderer2DAdvanced::DrawRoundedRect(
    Float32 x, Float32 y, Float32 w, Float32 h,
    Float32 radius, const Color& color, bool filled)
{
    if (!المحرك_) return;
    
    radius = std::min(radius, std::min(w / 2, h / 2));
    const Int32 cornerSegments = 8;
    
    if (filled) {
        // المستطيل المركزي / Central rectangle
        المحرك_->DrawRect(x + radius, y, w - 2 * radius, h, color, true);
        المحرك_->DrawRect(x, y + radius, radius, h - 2 * radius, color, true);
        المحرك_->DrawRect(x + w - radius, y + radius, radius, h - 2 * radius, color, true);
        
        // الزوايا الأربع / Four corners
        auto drawCorner = [&](Float32 cx, Float32 cy, Float32 startAngle) {
            for (Int32 i = 0; i < cornerSegments; ++i) {
                Float32 a1 = startAngle + (static_cast<Float32>(M_PI) / 2) * i / cornerSegments;
                Float32 a2 = startAngle + (static_cast<Float32>(M_PI) / 2) * (i + 1) / cornerSegments;
                المحرك_->DrawTriangle(
                    cx, cy,
                    cx + radius * std::cos(a1), cy + radius * std::sin(a1),
                    cx + radius * std::cos(a2), cy + radius * std::sin(a2),
                    color, true);
            }
        };
        
        drawCorner(x + radius, y + radius, static_cast<Float32>(M_PI));
        drawCorner(x + w - radius, y + radius, static_cast<Float32>(M_PI) * 1.5f);
        drawCorner(x + w - radius, y + h - radius, 0.0f);
        drawCorner(x + radius, y + h - radius, static_cast<Float32>(M_PI) * 0.5f);
    } else {
        // خطوط مستقيمة / Straight edges
        المحرك_->DrawLine(x + radius, y, x + w - radius, y, color);
        المحرك_->DrawLine(x + radius, y + h, x + w - radius, y + h, color);
        المحرك_->DrawLine(x, y + radius, x, y + h - radius, color);
        المحرك_->DrawLine(x + w, y + radius, x + w, y + h - radius, color);
        
        // أقواس الزوايا / Corner arcs
        المحرك_->DrawArc(x + radius, y + radius, radius,
                          static_cast<Float32>(M_PI), static_cast<Float32>(M_PI) * 1.5f, color, cornerSegments);
        المحرك_->DrawArc(x + w - radius, y + radius, radius,
                          static_cast<Float32>(M_PI) * 1.5f, static_cast<Float32>(M_PI) * 2.0f, color, cornerSegments);
        المحرك_->DrawArc(x + w - radius, y + h - radius, radius,
                          0.0f, static_cast<Float32>(M_PI) * 0.5f, color, cornerSegments);
        المحرك_->DrawArc(x + radius, y + h - radius, radius,
                          static_cast<Float32>(M_PI) * 0.5f, static_cast<Float32>(M_PI), color, cornerSegments);
    }
}

// ============================================================================
// الظلال / Shadows
// ============================================================================

void Renderer2DAdvanced::SetShadow(const إعدادات_الظل& shadow) {
    الظل_الحالي_ = shadow;
}

void Renderer2DAdvanced::DisableShadow() {
    الظل_الحالي_.مفعّل = false;
}

void Renderer2DAdvanced::DrawRectWithShadow(
    Float32 x, Float32 y, Float32 w, Float32 h,
    const Color& color, const إعدادات_الظل& shadow)
{
    if (!المحرك_) return;
    
    // رسم الظل أولاً / Draw shadow first
    Color shadowColor = shadow.لون;
    Float32 blur = shadow.نصف_قطر_الضبابية;
    
    // محاكاة الضبابية بطبقات متعددة شبه شفافة
    // Simulate blur with multiple semi-transparent layers
    Int32 layers = std::max(1, static_cast<Int32>(blur / 2));
    for (Int32 i = layers; i >= 0; --i) {
        Float32 spread = blur * static_cast<Float32>(i) / layers;
        Float32 alpha = static_cast<Float32>(shadowColor.a) / (layers + 1);
        Color layerColor(shadowColor.r, shadowColor.g, shadowColor.b, alpha);
        
        المحرك_->DrawRect(
            x + shadow.إزاحة_س - spread,
            y + shadow.إزاحة_ص - spread,
            w + 2 * spread,
            h + 2 * spread,
            layerColor, true);
    }
    
    // رسم المستطيل الأصلي / Draw original rectangle
    المحرك_->DrawRect(x, y, w, h, color, true);
}

void Renderer2DAdvanced::DrawCircleWithShadow(
    Float32 x, Float32 y, Float32 radius,
    const Color& color, const إعدادات_الظل& shadow)
{
    if (!المحرك_) return;
    
    Int32 layers = std::max(1, static_cast<Int32>(shadow.نصف_قطر_الضبابية / 2));
    for (Int32 i = layers; i >= 0; --i) {
        Float32 spread = shadow.نصف_قطر_الضبابية * static_cast<Float32>(i) / layers;
        Float32 alpha = static_cast<Float32>(shadow.لون.a) / (layers + 1);
        Color layerColor(shadow.لون.r, shadow.لون.g, shadow.لون.b, alpha);
        
        المحرك_->DrawCircle(
            x + shadow.إزاحة_س,
            y + shadow.إزاحة_ص,
            radius + spread,
            layerColor, true, 48);
    }
    
    المحرك_->DrawCircle(x, y, radius, color, true, 48);
}

// ============================================================================
// التدرجات اللونية / Gradients
// ============================================================================

void Renderer2DAdvanced::DrawGradientRect(
    Float32 x, Float32 y, Float32 w, Float32 h,
    const إعدادات_التدرج& gradient)
{
    if (!المحرك_) return;
    
    // تقسيم المستطيل إلى شرائح مع ألوان متدرجة
    // Split rectangle into strips with interpolated colors
    const Int32 strips = 32;
    Float32 stripH = h / strips;
    
    for (Int32 i = 0; i < strips; ++i) {
        Float32 t = static_cast<Float32>(i) / (strips - 1);
        
        Color c;
        c.r = static_cast<uint8_t>(gradient.لون_البداية.r + t * (gradient.لون_النهاية.r - gradient.لون_البداية.r));
        c.g = static_cast<uint8_t>(gradient.لون_البداية.g + t * (gradient.لون_النهاية.g - gradient.لون_البداية.g));
        c.b = static_cast<uint8_t>(gradient.لون_البداية.b + t * (gradient.لون_النهاية.b - gradient.لون_البداية.b));
        c.a = static_cast<uint8_t>(gradient.لون_البداية.a + t * (gradient.لون_النهاية.a - gradient.لون_البداية.a));
        
        المحرك_->DrawRect(x, y + i * stripH, w, stripH + 1, c, true);
    }
}

void Renderer2DAdvanced::DrawGradientCircle(
    Float32 x, Float32 y, Float32 radius,
    const Color& center, const Color& edge, Int32 segments)
{
    if (!المحرك_) return;
    
    // حلقات متحدة المركز بألوان متدرجة
    // Concentric rings with interpolated colors
    const Int32 rings = 16;
    for (Int32 i = rings; i >= 0; --i) {
        Float32 t = static_cast<Float32>(i) / rings;
        Float32 r = radius * t;
        
        Color c;
        c.r = static_cast<uint8_t>(center.r + t * (edge.r - center.r));
        c.g = static_cast<uint8_t>(center.g + t * (edge.g - center.g));
        c.b = static_cast<uint8_t>(center.b + t * (edge.b - center.b));
        c.a = static_cast<uint8_t>(center.a + t * (edge.a - center.a));
        
        المحرك_->DrawCircle(x, y, r, c, true, segments);
    }
}

// ============================================================================
// الفلاتر / Filters
// ============================================================================

void Renderer2DAdvanced::ApplyGaussianBlur(مخزن_البكسل& buffer, Float32 radius) {
    if (radius <= 0) return;
    
    Int32 w = buffer.العرض();
    Int32 h = buffer.الارتفاع();
    
    // إنشاء kernel غاوسي
    Int32 kernelSize = static_cast<Int32>(radius * 2) + 1;
    std::vector<Float32> kernel(kernelSize);
    Float32 sigma = radius / 3.0f;
    Float32 sum = 0;
    
    for (Int32 i = 0; i < kernelSize; ++i) {
        Float32 x = static_cast<Float32>(i - kernelSize / 2);
        kernel[i] = std::exp(-(x * x) / (2 * sigma * sigma));
        sum += kernel[i];
    }
    for (auto& k : kernel) k /= sum;
    
    // مخزن مؤقت / Temporary buffer
    std::vector<uint8_t> temp(w * h * 4);
    
    // مرور أفقي / Horizontal pass
    for (Int32 y = 0; y < h; ++y) {
        for (Int32 x = 0; x < w; ++x) {
            Float32 r = 0, g = 0, b = 0, a = 0;
            for (Int32 k = 0; k < kernelSize; ++k) {
                Int32 sx = std::clamp(x + k - kernelSize / 2, 0, w - 1);
                auto pixel = buffer.اقرأ_بكسل(sx, y);
                r += pixel.r * kernel[k];
                g += pixel.g * kernel[k];
                b += pixel.b * kernel[k];
                a += pixel.a * kernel[k];
            }
            size_t i = (y * w + x) * 4;
            temp[i] = static_cast<uint8_t>(std::clamp(r, 0.0f, 255.0f));
            temp[i + 1] = static_cast<uint8_t>(std::clamp(g, 0.0f, 255.0f));
            temp[i + 2] = static_cast<uint8_t>(std::clamp(b, 0.0f, 255.0f));
            temp[i + 3] = static_cast<uint8_t>(std::clamp(a, 0.0f, 255.0f));
        }
    }
    
    // مرور عمودي / Vertical pass
    for (Int32 y = 0; y < h; ++y) {
        for (Int32 x = 0; x < w; ++x) {
            Float32 r = 0, g = 0, b = 0, a = 0;
            for (Int32 k = 0; k < kernelSize; ++k) {
                Int32 sy = std::clamp(y + k - kernelSize / 2, 0, h - 1);
                size_t i = (sy * w + x) * 4;
                r += temp[i] * kernel[k];
                g += temp[i + 1] * kernel[k];
                b += temp[i + 2] * kernel[k];
                a += temp[i + 3] * kernel[k];
            }
            buffer.عيّن_بكسل(x, y, Color(
                std::clamp(r / 255.0f, 0.0f, 1.0f),
                std::clamp(g / 255.0f, 0.0f, 1.0f),
                std::clamp(b / 255.0f, 0.0f, 1.0f),
                std::clamp(a / 255.0f, 0.0f, 1.0f)
            ));
        }
    }
}

void Renderer2DAdvanced::ApplyGlow(مخزن_البكسل& buffer, const Color& glowColor, Float32 radius) {
    // نسخ المخزن ← تطبيق Blur ← مزج مع الأصل
    مخزن_البكسل blurred(buffer.العرض(), buffer.الارتفاع());
    std::memcpy(blurred.البيانات(), buffer.البيانات(), buffer.العرض() * buffer.الارتفاع() * 4);
    
    ApplyGaussianBlur(blurred, radius);
    
    // مزج: الأصل + التوهج
    for (UInt32 y = 0; y < buffer.الارتفاع(); ++y) {
        for (UInt32 x = 0; x < buffer.العرض(); ++x) {
            Color orig = buffer.اقرأ_بكسل(x, y);
            Color blur = blurred.اقرأ_بكسل(x, y);
            
            Color result;
            result.r = std::min(1.0f, orig.r + blur.r * glowColor.r);
            result.g = std::min(1.0f, orig.g + blur.g * glowColor.g);
            result.b = std::min(1.0f, orig.b + blur.b * glowColor.b);
            result.a = std::max(orig.a, blur.a);
            
            buffer.عيّن_بكسل(x, y, result);
        }
    }
}

void Renderer2DAdvanced::ApplyColorFilter(مخزن_البكسل& buffer,
                                           Float32 r_scale, Float32 g_scale,
                                           Float32 b_scale, Float32 a_scale) {
    uint8_t* data = buffer.البيانات();
    size_t count = buffer.العرض() * buffer.الارتفاع();
    
    for (size_t i = 0; i < count; ++i) {
        size_t p = i * 4;
        data[p]     = static_cast<uint8_t>(std::clamp(data[p] * r_scale, 0.0f, 255.0f));
        data[p + 1] = static_cast<uint8_t>(std::clamp(data[p + 1] * g_scale, 0.0f, 255.0f));
        data[p + 2] = static_cast<uint8_t>(std::clamp(data[p + 2] * b_scale, 0.0f, 255.0f));
        data[p + 3] = static_cast<uint8_t>(std::clamp(data[p + 3] * a_scale, 0.0f, 255.0f));
    }
}

void Renderer2DAdvanced::ApplyContrast(مخزن_البكسل& buffer, Float32 factor) {
    uint8_t* data = buffer.البيانات();
    size_t count = buffer.العرض() * buffer.الارتفاع();
    
    for (size_t i = 0; i < count; ++i) {
        size_t p = i * 4;
        for (int c = 0; c < 3; ++c) {
            Float32 v = (data[p + c] / 255.0f - 0.5f) * factor + 0.5f;
            data[p + c] = static_cast<uint8_t>(std::clamp(v * 255.0f, 0.0f, 255.0f));
        }
    }
}

void Renderer2DAdvanced::ApplyBrightness(مخزن_البكسل& buffer, Float32 offset) {
    uint8_t* data = buffer.البيانات();
    size_t count = buffer.العرض() * buffer.الارتفاع();
    
    for (size_t i = 0; i < count; ++i) {
        size_t p = i * 4;
        for (int c = 0; c < 3; ++c) {
            data[p + c] = static_cast<uint8_t>(std::clamp(data[p + c] + offset, 0.0f, 255.0f));
        }
    }
}

void Renderer2DAdvanced::ApplyGrayscale(مخزن_البكسل& buffer) {
    uint8_t* data = buffer.البيانات();
    size_t count = buffer.العرض() * buffer.الارتفاع();
    
    for (size_t i = 0; i < count; ++i) {
        size_t p = i * 4;
        // ITU-R BT.709 coefficients
        uint8_t gray = static_cast<uint8_t>(
            0.2126f * data[p] + 0.7152f * data[p + 1] + 0.0722f * data[p + 2]);
        data[p] = gray;
        data[p + 1] = gray;
        data[p + 2] = gray;
    }
}

void Renderer2DAdvanced::ApplyInvert(مخزن_البكسل& buffer) {
    uint8_t* data = buffer.البيانات();
    size_t count = buffer.العرض() * buffer.الارتفاع();
    
    for (size_t i = 0; i < count; ++i) {
        size_t p = i * 4;
        data[p]     = 255 - data[p];
        data[p + 1] = 255 - data[p + 1];
        data[p + 2] = 255 - data[p + 2];
        // alpha لا يُقلب / Don't invert alpha
    }
}

// ============================================================================
// تحويلات / Transforms
// ============================================================================

void Renderer2DAdvanced::SetTransform(const Transform2D& transform) {
    التحويل_الحالي_ = transform;
}

void Renderer2DAdvanced::PushTransform() {
    كومة_التحويلات_.push_back(التحويل_الحالي_);
}

void Renderer2DAdvanced::PopTransform() {
    if (!كومة_التحويلات_.empty()) {
        التحويل_الحالي_ = كومة_التحويلات_.back();
        كومة_التحويلات_.pop_back();
    }
}

// ============================================================================
// نقطة بزيه المساعدة في Renderer2D / Helper in base Renderer2D
// ============================================================================
void Renderer2D::BezierPoint(Float32 t, Float32 x0, Float32 y0,
                              Float32 x1, Float32 y1,
                              Float32 x2, Float32 y2,
                              Float32 x3, Float32 y3,
                              Float32& outX, Float32& outY) {
    Float32 u = 1.0f - t;
    Float32 u2 = u * u;
    Float32 u3 = u2 * u;
    Float32 t2 = t * t;
    Float32 t3 = t2 * t;
    outX = u3 * x0 + 3 * u2 * t * x1 + 3 * u * t2 * x2 + t3 * x3;
    outY = u3 * y0 + 3 * u2 * t * y1 + 3 * u * t2 * y2 + t3 * y3;
}

} // namespace SadGraphics
