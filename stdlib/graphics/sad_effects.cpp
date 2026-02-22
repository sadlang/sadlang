// ═══════════════════════════════════════════════════════════════════════════════
// نظام التأثيرات البصرية — التنفيذ
// Visual Effects System — Implementation
// ═══════════════════════════════════════════════════════════════════════════════
#include "sad_effects.h"
#include <SDL.h>
#include <algorithm>
#include <cmath>
#include <vector>

namespace sad { namespace effects {

// ═══════════════════════════════════════════════════════════════════
// مساعدات داخلية
// ═══════════════════════════════════════════════════════════════════

static uint8_t clampByte(int v) { return (uint8_t)std::max(0, std::min(255, v)); }
static float clampF(float v, float lo, float hi) { return std::max(lo, std::min(hi, v)); }

// ═══════════════════════════════════════════════════════════════════
// ضبابية — Box Blur (محاكاة عبر رسم شبه شفاف)
// ═══════════════════════════════════════════════════════════════════

void applyBlur(void* renderer, int x, int y, int w, int h, float radius) {
    if (!renderer || radius <= 0) return;
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    
    // محاكاة الضبابية عبر رسم مستطيلات شبه شفافة متراكبة
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    int passes = std::max(1, (int)(radius / 2));
    int alphaPerPass = std::max(5, (int)(180.0f / passes));
    
    for (int i = 0; i < passes; i++) {
        float expand = (float)i * (radius / passes);
        SDL_Rect rect = {
            (int)(x - expand), (int)(y - expand),
            (int)(w + expand * 2), (int)(h + expand * 2)
        };
        SDL_SetRenderDrawColor(r, 200, 200, 210, (uint8_t)alphaPerPass);
        SDL_RenderFillRect(r, &rect);
    }
}

// ═══════════════════════════════════════════════════════════════════
// ظل — Shadow
// ═══════════════════════════════════════════════════════════════════

void drawShadow(void* renderer, int x, int y, int w, int h, const ShadowConfig& cfg) {
    drawShadowSimple(renderer, x, y, w, h, cfg.offsetX, cfg.offsetY, cfg.blurRadius,
                      cfg.r, cfg.g, cfg.b, cfg.alpha);
}

void drawShadowSimple(void* renderer, int x, int y, int w, int h,
                       float offsetX, float offsetY, float blur,
                       uint8_t cr, uint8_t cg, uint8_t cb, float alpha) {
    if (!renderer) return;
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    
    int layers = std::max(1, (int)(blur / 2));
    float baseAlpha = alpha * 255.0f / layers;
    
    for (int i = layers; i >= 0; i--) {
        float expand = (float)i * (blur / layers);
        SDL_Rect rect = {
            (int)(x + offsetX - expand),
            (int)(y + offsetY - expand),
            (int)(w + expand * 2),
            (int)(h + expand * 2)
        };
        uint8_t a = clampByte((int)(baseAlpha * (1.0f - (float)i / (layers + 1))));
        SDL_SetRenderDrawColor(r, cr, cg, cb, a);
        SDL_RenderFillRect(r, &rect);
    }
}

// ═══════════════════════════════════════════════════════════════════
// توهج — Glow
// ═══════════════════════════════════════════════════════════════════

void drawGlow(void* renderer, int x, int y, int w, int h, const GlowConfig& cfg) {
    drawGlowSimple(renderer, x, y, w, h, cfg.radius, cfg.r, cfg.g, cfg.b, cfg.intensity);
}

void drawGlowSimple(void* renderer, int x, int y, int w, int h,
                     float radius, uint8_t cr, uint8_t cg, uint8_t cb, float intensity) {
    if (!renderer) return;
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_ADD);
    
    int layers = std::max(2, (int)(radius / 2));
    
    for (int i = layers; i >= 0; i--) {
        float expand = (float)i * (radius / layers);
        float layerAlpha = intensity * (1.0f - (float)i / layers) * 0.3f;
        
        SDL_Rect rect = {
            (int)(x - expand), (int)(y - expand),
            (int)(w + expand * 2), (int)(h + expand * 2)
        };
        SDL_SetRenderDrawColor(r, cr, cg, cb, clampByte((int)(layerAlpha * 255)));
        SDL_RenderFillRect(r, &rect);
    }
    
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
}

// ═══════════════════════════════════════════════════════════════════
// تدرج — Gradients
// ═══════════════════════════════════════════════════════════════════

void drawGradientH(void* renderer, int x, int y, int w, int h,
                    uint8_t r1, uint8_t g1, uint8_t b1,
                    uint8_t r2, uint8_t g2, uint8_t b2, float alpha) {
    if (!renderer || w <= 0) return;
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    uint8_t a = clampByte((int)(alpha * 255));
    
    for (int i = 0; i < w; i++) {
        float t = (float)i / (float)(w - 1);
        SDL_SetRenderDrawColor(r,
            clampByte((int)(r1 + (r2 - r1) * t)),
            clampByte((int)(g1 + (g2 - g1) * t)),
            clampByte((int)(b1 + (b2 - b1) * t)), a);
        SDL_RenderDrawLine(r, x + i, y, x + i, y + h - 1);
    }
}

void drawGradientV(void* renderer, int x, int y, int w, int h,
                    uint8_t r1, uint8_t g1, uint8_t b1,
                    uint8_t r2, uint8_t g2, uint8_t b2, float alpha) {
    if (!renderer || h <= 0) return;
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    uint8_t a = clampByte((int)(alpha * 255));
    
    for (int i = 0; i < h; i++) {
        float t = (float)i / (float)(h - 1);
        SDL_SetRenderDrawColor(r,
            clampByte((int)(r1 + (r2 - r1) * t)),
            clampByte((int)(g1 + (g2 - g1) * t)),
            clampByte((int)(b1 + (b2 - b1) * t)), a);
        SDL_RenderDrawLine(r, x, y + i, x + w - 1, y + i);
    }
}

void drawGradientRadial(void* renderer, int cx, int cy, float radius,
                         uint8_t r1, uint8_t g1, uint8_t b1,
                         uint8_t r2, uint8_t g2, uint8_t b2, float alpha) {
    if (!renderer || radius <= 0) return;
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    
    // رسم دوائر متحدة المركز من الخارج للداخل
    int iRad = (int)radius;
    for (int rad = iRad; rad >= 0; rad--) {
        float t = (float)rad / radius;
        uint8_t cr = clampByte((int)(r1 + (r2 - r1) * t));
        uint8_t cg = clampByte((int)(g1 + (g2 - g1) * t));
        uint8_t cb = clampByte((int)(b1 + (b2 - b1) * t));
        uint8_t a = clampByte((int)(alpha * 255));
        SDL_SetRenderDrawColor(r, cr, cg, cb, a);
        
        // رسم محيط الدائرة (Midpoint circle)
        int dx = rad, dy = 0;
        int err = 1 - dx;
        while (dx >= dy) {
            SDL_RenderDrawLine(r, cx - dx, cy + dy, cx + dx, cy + dy);
            SDL_RenderDrawLine(r, cx - dx, cy - dy, cx + dx, cy - dy);
            SDL_RenderDrawLine(r, cx - dy, cy + dx, cx + dy, cy + dx);
            SDL_RenderDrawLine(r, cx - dy, cy - dx, cx + dy, cy - dx);
            dy++;
            if (err < 0) { err += 2 * dy + 1; }
            else { dx--; err += 2 * (dy - dx) + 1; }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════
// تأثير الزجاج — Glass Morphism
// ═══════════════════════════════════════════════════════════════════

void drawGlassPanel(void* renderer, int x, int y, int w, int h, const GlassConfig& cfg) {
    drawGlassPanelSimple(renderer, x, y, w, h, cfg.blurAmount, cfg.opacity,
                          cfg.tintR, cfg.tintG, cfg.tintB);
    if (cfg.hasBorder) {
        SDL_Renderer* r = (SDL_Renderer*)renderer;
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(r, 255, 255, 255, clampByte((int)(cfg.borderOpacity * 255)));
        SDL_Rect border = {x, y, w, h};
        SDL_RenderDrawRect(r, &border);
    }
}

void drawGlassPanelSimple(void* renderer, int x, int y, int w, int h,
                           float blur, float opacity,
                           uint8_t tintR, uint8_t tintG, uint8_t tintB) {
    if (!renderer) return;
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    
    // الخطوة 1: رسم ضبابية خفيفة في الخلفية
    applyBlur(renderer, x, y, w, h, blur);
    
    // الخطوة 2: طبقة لونية شبه شفافة
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, tintR, tintG, tintB, clampByte((int)(opacity * 255)));
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(r, &rect);
    
    // الخطوة 3: وهج خفيف في الأعلى (لمعان زجاجي)
    SDL_SetRenderDrawColor(r, 255, 255, 255, 20);
    SDL_Rect topHighlight = {x + 1, y + 1, w - 2, h / 3};
    SDL_RenderFillRect(r, &topHighlight);
}

// ═══════════════════════════════════════════════════════════════════
// نيومورفيزم — Neumorphism (تأثير الظل الناعم)
// ═══════════════════════════════════════════════════════════════════

void drawNeumorph(void* renderer, int x, int y, int w, int h, const NeumorphConfig& cfg) {
    drawNeumorphSimple(renderer, x, y, w, h, cfg.depth, cfg.bgR, cfg.bgG, cfg.bgB, cfg.isConvex);
}

void drawNeumorphSimple(void* renderer, int x, int y, int w, int h,
                         float depth, uint8_t bgR, uint8_t bgG, uint8_t bgB, bool convex) {
    if (!renderer) return;
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    
    int d = (int)depth;
    
    if (convex) {
        // ظل داكن (أسفل-يمين)
        for (int i = d; i >= 1; i--) {
            float alpha = 0.15f * (1.0f - (float)i / (d + 1));
            SDL_SetRenderDrawColor(r, 0, 0, 0, clampByte((int)(alpha * 255)));
            SDL_Rect shadow = {x + i, y + i, w, h};
            SDL_RenderFillRect(r, &shadow);
        }
        
        // ظل فاتح (أعلى-يسار)
        for (int i = d; i >= 1; i--) {
            float alpha = 0.3f * (1.0f - (float)i / (d + 1));
            SDL_SetRenderDrawColor(r, 255, 255, 255, clampByte((int)(alpha * 255)));
            SDL_Rect light = {x - i, y - i, w, h};
            SDL_RenderFillRect(r, &light);
        }
    } else {
        // غائر — عكس الظلال
        for (int i = d; i >= 1; i--) {
            float alpha = 0.15f * (1.0f - (float)i / (d + 1));
            SDL_SetRenderDrawColor(r, 255, 255, 255, clampByte((int)(alpha * 255)));
            SDL_Rect light = {x + i, y + i, w, h};
            SDL_RenderFillRect(r, &light);
        }
        for (int i = d; i >= 1; i--) {
            float alpha = 0.3f * (1.0f - (float)i / (d + 1));
            SDL_SetRenderDrawColor(r, 0, 0, 0, clampByte((int)(alpha * 255)));
            SDL_Rect shadow = {x - i, y - i, w, h};
            SDL_RenderFillRect(r, &shadow);
        }
    }
    
    // الجسم الرئيسي
    SDL_SetRenderDrawColor(r, bgR, bgG, bgB, 255);
    SDL_Rect body = {x, y, w, h};
    SDL_RenderFillRect(r, &body);
}

// ═══════════════════════════════════════════════════════════════════
// فلاتر الألوان
// ═══════════════════════════════════════════════════════════════════

// ملاحظة: فلاتر الألوان الحقيقية تحتاج قراءة بكسلات من render target
// هنا نستخدم طبقات لونية فوقية كمحاكاة فعالة

void applyColorFilter(void* renderer, int x, int y, int w, int h,
                       FilterType filter, float amount) {
    switch (filter) {
        case FilterType::Sepia: applySepia(renderer, x, y, w, h, amount); break;
        case FilterType::Grayscale: applyGrayscale(renderer, x, y, w, h, amount); break;
        case FilterType::Invert: applyInvert(renderer, x, y, w, h); break;
        case FilterType::Brightness: applyBrightness(renderer, x, y, w, h, amount); break;
        case FilterType::Contrast: applyContrast(renderer, x, y, w, h, amount); break;
        case FilterType::Tint: applyTint(renderer, x, y, w, h, 255, 200, 100, amount); break;
        default: break;
    }
}

void applySepia(void* renderer, int x, int y, int w, int h, float amount) {
    if (!renderer) return;
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    // طبقة بنية شبه شفافة تحاكي تأثير بني قديم
    uint8_t a = clampByte((int)(amount * 80));
    SDL_SetRenderDrawColor(r, 112, 66, 20, a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(r, &rect);
}

void applyGrayscale(void* renderer, int x, int y, int w, int h, float amount) {
    if (!renderer) return;
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    // طبقة رمادية لتخفيف الألوان
    uint8_t a = clampByte((int)(amount * 120));
    SDL_SetRenderDrawColor(r, 128, 128, 128, a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(r, &rect);
}

void applyInvert(void* renderer, int x, int y, int w, int h) {
    if (!renderer) return;
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    // محاكاة العكس باستخدام XOR blend — مستطيل أبيض مع SDL_BLENDMODE_MOD
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_MOD);
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(r, &rect);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
}

void applyBrightness(void* renderer, int x, int y, int w, int h, float factor) {
    if (!renderer) return;
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    if (factor > 1.0f) {
        // تفتيح
        uint8_t a = clampByte((int)((factor - 1.0f) * 128));
        SDL_SetRenderDrawColor(r, 255, 255, 255, a);
    } else {
        // تعتيم
        uint8_t a = clampByte((int)((1.0f - factor) * 200));
        SDL_SetRenderDrawColor(r, 0, 0, 0, a);
    }
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(r, &rect);
}

void applyContrast(void* renderer, int x, int y, int w, int h, float factor) {
    if (!renderer) return;
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    if (factor > 1.0f) {
        // زيادة التباين — تعتيم خفيف
        uint8_t a = clampByte((int)((factor - 1.0f) * 40));
        SDL_SetRenderDrawColor(r, 0, 0, 0, a);
    } else {
        // تقليل التباين — رمادي خفيف
        uint8_t a = clampByte((int)((1.0f - factor) * 100));
        SDL_SetRenderDrawColor(r, 128, 128, 128, a);
    }
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(r, &rect);
}

void applyTint(void* renderer, int x, int y, int w, int h,
               uint8_t cr, uint8_t cg, uint8_t cb, float amount) {
    if (!renderer) return;
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    uint8_t a = clampByte((int)(amount * 100));
    SDL_SetRenderDrawColor(r, cr, cg, cb, a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(r, &rect);
}

// ═══════════════════════════════════════════════════════════════════
// تأثير Vignette — تعتيم الحواف
// ═══════════════════════════════════════════════════════════════════

void drawVignette(void* renderer, int width, int height,
                   float radius, float softness, float alpha) {
    if (!renderer) return;
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    
    float cx = width / 2.0f;
    float cy = height / 2.0f;
    float maxDist = std::sqrt(cx * cx + cy * cy);
    float innerR = radius * maxDist;
    float outerR = innerR + softness * maxDist;
    
    // رسم شرائط أفقية لتأثير الحافة
    int step = std::max(1, height / 60);
    for (int y = 0; y < height; y += step) {
        for (int x = 0; x < width; x += step) {
            float dx = x - cx;
            float dy = y - cy;
            float dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist > innerR) {
                float t = clampF((dist - innerR) / (outerR - innerR), 0.0f, 1.0f);
                uint8_t a = clampByte((int)(t * alpha * 255));
                SDL_SetRenderDrawColor(r, 0, 0, 0, a);
                SDL_Rect rect = {x, y, step, step};
                SDL_RenderFillRect(r, &rect);
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════
// تأثير التموج — Ripple
// ═══════════════════════════════════════════════════════════════════

void drawRipple(void* renderer, float cx, float cy,
                float radius, float amplitude, float phase,
                uint8_t cr, uint8_t cg, uint8_t cb, float alpha) {
    if (!renderer) return;
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    
    // رسم دوائر متموجة
    int rings = (int)(radius / 5);
    for (int i = 0; i < rings; i++) {
        float t = (float)i / rings;
        float ringR = radius * t;
        float wave = std::sin(t * 6.2832f * 3 + phase) * amplitude;
        float actualR = ringR + wave;
        if (actualR <= 0) continue;
        
        float ringAlpha = alpha * (1.0f - t) * 0.5f;
        SDL_SetRenderDrawColor(r, cr, cg, cb, clampByte((int)(ringAlpha * 255)));
        
        // رسم الدائرة بخوارزمية Midpoint
        int dx = (int)actualR, dy = 0;
        int err = 1 - dx;
        int icx = (int)cx, icy = (int)cy;
        while (dx >= dy) {
            SDL_RenderDrawPoint(r, icx + dx, icy + dy);
            SDL_RenderDrawPoint(r, icx - dx, icy + dy);
            SDL_RenderDrawPoint(r, icx + dx, icy - dy);
            SDL_RenderDrawPoint(r, icx - dx, icy - dy);
            SDL_RenderDrawPoint(r, icx + dy, icy + dx);
            SDL_RenderDrawPoint(r, icx - dy, icy + dx);
            SDL_RenderDrawPoint(r, icx + dy, icy - dx);
            SDL_RenderDrawPoint(r, icx - dy, icy - dx);
            dy++;
            if (err < 0) { err += 2 * dy + 1; }
            else { dx--; err += 2 * (dy - dx) + 1; }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════
// حد مزخرف — Gradient Border
// ═══════════════════════════════════════════════════════════════════

void drawBorderGradient(void* renderer, int x, int y, int w, int h,
                         float thickness,
                         uint8_t r1, uint8_t g1, uint8_t b1,
                         uint8_t r2, uint8_t g2, uint8_t b2) {
    if (!renderer) return;
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    
    int t = (int)thickness;
    int perimeter = 2 * (w + h);
    
    // رسم الحد العلوي
    for (int i = 0; i < w; i++) {
        float ratio = (float)i / perimeter;
        SDL_SetRenderDrawColor(r,
            clampByte((int)(r1 + (r2 - r1) * ratio)),
            clampByte((int)(g1 + (g2 - g1) * ratio)),
            clampByte((int)(b1 + (b2 - b1) * ratio)), 255);
        for (int j = 0; j < t; j++)
            SDL_RenderDrawPoint(r, x + i, y + j);
    }
    
    // رسم الحد الأيمن
    for (int i = 0; i < h; i++) {
        float ratio = (float)(w + i) / perimeter;
        SDL_SetRenderDrawColor(r,
            clampByte((int)(r1 + (r2 - r1) * ratio)),
            clampByte((int)(g1 + (g2 - g1) * ratio)),
            clampByte((int)(b1 + (b2 - b1) * ratio)), 255);
        for (int j = 0; j < t; j++)
            SDL_RenderDrawPoint(r, x + w - 1 - j, y + i);
    }
    
    // رسم الحد السفلي
    for (int i = w - 1; i >= 0; i--) {
        float ratio = (float)(w + h + (w - i)) / perimeter;
        SDL_SetRenderDrawColor(r,
            clampByte((int)(r1 + (r2 - r1) * ratio)),
            clampByte((int)(g1 + (g2 - g1) * ratio)),
            clampByte((int)(b1 + (b2 - b1) * ratio)), 255);
        for (int j = 0; j < t; j++)
            SDL_RenderDrawPoint(r, x + i, y + h - 1 - j);
    }
    
    // رسم الحد الأيسر
    for (int i = h - 1; i >= 0; i--) {
        float ratio = (float)(2 * w + h + (h - i)) / perimeter;
        if (ratio > 1.0f) ratio = 1.0f;
        SDL_SetRenderDrawColor(r,
            clampByte((int)(r1 + (r2 - r1) * ratio)),
            clampByte((int)(g1 + (g2 - g1) * ratio)),
            clampByte((int)(b1 + (b2 - b1) * ratio)), 255);
        for (int j = 0; j < t; j++)
            SDL_RenderDrawPoint(r, x + j, y + i);
    }
}

// ═══════════════════════════════════════════════════════════════════

void clearEffects() {
    // مسح أي حالة مؤقتة (حالياً لا يوجد شيء للمسح)
}

}} // namespace sad::effects
