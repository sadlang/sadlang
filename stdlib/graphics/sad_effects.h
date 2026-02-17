// ═══════════════════════════════════════════════════════════════════════════════
// نظام التأثيرات البصرية — لغة ص
// Visual Effects System — SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
//
// تأثيرات بصرية متقدمة غير موجودة في Flutter أصلاً:
//   - ضبابية (Blur) — Box blur على أي منطقة
//   - توهج (Glow) — حافة متوهجة حول العناصر
//   - ظل (Shadow) — ظل ناعم بألوان مخصصة
//   - تأثير الزجاج (Glass Morphism) — خلفية زجاجية شفافة
//   - فلاتر ألوان: بني قديم، رمادي، عكسي، سطوع، تباين
//   - تأثير الانعكاس (Reflection)
//   - تأثير النيومورفيزم (Neumorphism)
//   - تدرج بصري (Vignette)
//   - تشويه / تموج (Ripple/Distortion)
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

namespace sad { namespace effects {

// ═══════════════════════════════════════════════════════════════════
// أنواع التأثيرات
// ═══════════════════════════════════════════════════════════════════

enum class FilterType {
    Sepia,      // بني قديم
    Grayscale,  // رمادي
    Invert,     // عكسي
    Brightness, // سطوع
    Contrast,   // تباين
    Saturate,   // تشبع
    HueRotate,  // دوران اللون
    Tint        // صبغة لون
};

// هيكل نقطة لون
struct PixelColor {
    uint8_t r, g, b, a;
};

// إعدادات الظل
struct ShadowConfig {
    float offsetX = 4.0f;
    float offsetY = 4.0f;
    float blurRadius = 8.0f;
    uint8_t r = 0, g = 0, b = 0;
    float alpha = 0.3f;
};

// إعدادات التوهج
struct GlowConfig {
    float radius = 10.0f;
    float intensity = 1.0f;
    uint8_t r = 255, g = 255, b = 100;
    float alpha = 0.8f;
};

// إعدادات الزجاج
struct GlassConfig {
    float blurAmount = 10.0f;
    float opacity = 0.3f;
    uint8_t tintR = 255, tintG = 255, tintB = 255;
    float borderRadius = 12.0f;
    bool hasBorder = true;
    float borderOpacity = 0.2f;
};

// إعدادات النيومورفيزم
struct NeumorphConfig {
    float depth = 5.0f;        // عمق التأثير
    float blurRadius = 10.0f;  // نصف قطر الضبابية
    uint8_t bgR = 230, bgG = 230, bgB = 235;  // لون الخلفية
    bool isConvex = true;      // بارز أم غائر
    float borderRadius = 12.0f;
};

// ═══════════════════════════════════════════════════════════════════
// واجهة برمجة التأثيرات البصرية
// ═══════════════════════════════════════════════════════════════════

// ─── ضبابية (Blur) ───
// تطبيق ضبابية على منطقة مستطيلة
void applyBlur(void* renderer, int x, int y, int w, int h, float radius);

// ─── ظل (Shadow) ───
// رسم ظل مستطيل
void drawShadow(void* renderer, int x, int y, int w, int h, const ShadowConfig& cfg);
// رسم ظل بالقيم المباشرة
void drawShadowSimple(void* renderer, int x, int y, int w, int h,
                       float offsetX, float offsetY, float blur,
                       uint8_t r, uint8_t g, uint8_t b, float alpha);

// ─── توهج (Glow) ───
void drawGlow(void* renderer, int x, int y, int w, int h, const GlowConfig& cfg);
void drawGlowSimple(void* renderer, int x, int y, int w, int h,
                     float radius, uint8_t r, uint8_t g, uint8_t b, float intensity);

// ─── تدرج بصري (Gradient) ───
void drawGradientH(void* renderer, int x, int y, int w, int h,
                    uint8_t r1, uint8_t g1, uint8_t b1,
                    uint8_t r2, uint8_t g2, uint8_t b2, float alpha);
void drawGradientV(void* renderer, int x, int y, int w, int h,
                    uint8_t r1, uint8_t g1, uint8_t b1,
                    uint8_t r2, uint8_t g2, uint8_t b2, float alpha);
void drawGradientRadial(void* renderer, int cx, int cy, float radius,
                         uint8_t r1, uint8_t g1, uint8_t b1,
                         uint8_t r2, uint8_t g2, uint8_t b2, float alpha);

// ─── تأثير الزجاج (Glass Morphism) ───
void drawGlassPanel(void* renderer, int x, int y, int w, int h, const GlassConfig& cfg);
void drawGlassPanelSimple(void* renderer, int x, int y, int w, int h,
                           float blur, float opacity,
                           uint8_t tintR, uint8_t tintG, uint8_t tintB);

// ─── نيومورفيزم (Neumorphism) ───
void drawNeumorph(void* renderer, int x, int y, int w, int h, const NeumorphConfig& cfg);
void drawNeumorphSimple(void* renderer, int x, int y, int w, int h,
                         float depth, uint8_t bgR, uint8_t bgG, uint8_t bgB, bool convex);

// ─── فلاتر الألوان ───
void applyColorFilter(void* renderer, int x, int y, int w, int h,
                       FilterType filter, float amount = 1.0f);
void applySepia(void* renderer, int x, int y, int w, int h, float amount = 1.0f);
void applyGrayscale(void* renderer, int x, int y, int w, int h, float amount = 1.0f);
void applyInvert(void* renderer, int x, int y, int w, int h);
void applyBrightness(void* renderer, int x, int y, int w, int h, float factor);
void applyContrast(void* renderer, int x, int y, int w, int h, float factor);
void applyTint(void* renderer, int x, int y, int w, int h,
               uint8_t r, uint8_t g, uint8_t b, float amount);

// ─── تأثير Vignette ───
void drawVignette(void* renderer, int width, int height,
                   float radius, float softness, float alpha);

// ─── تأثير التموج (Ripple) ───
void drawRipple(void* renderer, float cx, float cy,
                float radius, float amplitude, float phase,
                uint8_t r, uint8_t g, uint8_t b, float alpha);

// ─── حد مزخرف (Decorated Border) ───
void drawBorderGradient(void* renderer, int x, int y, int w, int h,
                         float thickness,
                         uint8_t r1, uint8_t g1, uint8_t b1,
                         uint8_t r2, uint8_t g2, uint8_t b2);

// ─── مسح التأثيرات المؤقتة ───
void clearEffects();

}} // namespace sad::effects
