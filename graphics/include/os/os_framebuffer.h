/**
 * @file os_framebuffer.h
 * @brief رسومات المخزن المؤقت للإطار لأنظمة التشغيل
 * 
 * يوفر واجهة منخفضة المستوى للرسم المباشر على ذاكرة الشاشة،
 * مناسبة لبرمجة أنظمة التشغيل والانظمة المدمجة.
 * 
 * المكونات:
 * - إدارة المخزن المؤقت للإطار (Framebuffer)
 * - رسم البكسل المباشر
 * - رسم الأشكال الأساسية بدون GPU
 * - خلط الألوان (Alpha Blending)
 * - نسخ المناطق (Blit)
 * - مخزن مزدوج (Double Buffering)
 * - تحجيم وتدوير الصور
 * - نمط ألوان متعددة (RGB565, RGB888, RGBA8888, BGR)
 * 
 * @note لا يعتمد على OpenGL أو أي مكتبة رسومات خارجية
 * @author فريق لغة ص
 * @date 2026
 */
#pragma once

#include <cstdint>
#include <cstring>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <cmath>
#include <string>

namespace sad {
namespace graphics {
namespace os {

// =============================================================================
// أنماط الألوان - Color Formats
// =============================================================================

/// نمط ترتيب البكسل
enum class PixelFormat {
    RGB565,     ///< 16 بت: 5 أحمر، 6 أخضر، 5 أزرق (للأنظمة المدمجة)
    RGB888,     ///< 24 بت: 8 لكل قناة
    RGBA8888,   ///< 32 بت: 8 لكل قناة مع شفافية
    BGR888,     ///< 24 بت: ترتيب معكوس
    BGRA8888,   ///< 32 بت: ترتيب معكوس مع شفافية
    Grayscale8, ///< 8 بت: تدرج رمادي
    Indexed8    ///< 8 بت: مفهرس (مع لوح ألوان)
};

/// لون 32 بت (RGBA)
struct PixelColor {
    uint8_t r = 0;  ///< الأحمر
    uint8_t g = 0;  ///< الأخضر
    uint8_t b = 0;  ///< الأزرق
    uint8_t a = 255; ///< الشفافية

    PixelColor() = default;
    PixelColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}

    /// إنشاء من قيمة 32 بت مجمعة
    static PixelColor fromPacked(uint32_t packed) {
        return {
            (uint8_t)((packed >> 16) & 0xFF),
            (uint8_t)((packed >> 8) & 0xFF),
            (uint8_t)(packed & 0xFF),
            (uint8_t)((packed >> 24) & 0xFF)
        };
    }

    /// تحويل لقيمة 32 بت مجمعة
    uint32_t toPacked() const {
        return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
    }

    /// تحويل إلى RGB565
    uint16_t toRGB565() const {
        return ((uint16_t)(r >> 3) << 11) | ((uint16_t)(g >> 2) << 5) | (b >> 3);
    }

    /// إنشاء من RGB565
    static PixelColor fromRGB565(uint16_t c) {
        return {
            (uint8_t)(((c >> 11) & 0x1F) << 3),
            (uint8_t)(((c >> 5) & 0x3F) << 2),
            (uint8_t)((c & 0x1F) << 3),
            255
        };
    }

    /// خلط لونين (Alpha Blending)
    static PixelColor blend(const PixelColor& src, const PixelColor& dst) {
        uint16_t sa = src.a;
        uint16_t da = 255 - sa;
        return {
            (uint8_t)((src.r * sa + dst.r * da) / 255),
            (uint8_t)((src.g * sa + dst.g * da) / 255),
            (uint8_t)((src.b * sa + dst.b * da) / 255),
            (uint8_t)(sa + (dst.a * da) / 255)
        };
    }

    /// القراءة من ذاكرة بنمط معين
    static PixelColor readFromMemory(const uint8_t* ptr, PixelFormat format) {
        switch (format) {
            case PixelFormat::RGB565: {
                uint16_t val = *(const uint16_t*)ptr;
                return fromRGB565(val);
            }
            case PixelFormat::RGB888:
                return {ptr[0], ptr[1], ptr[2], 255};
            case PixelFormat::RGBA8888:
                return {ptr[0], ptr[1], ptr[2], ptr[3]};
            case PixelFormat::BGR888:
                return {ptr[2], ptr[1], ptr[0], 255};
            case PixelFormat::BGRA8888:
                return {ptr[2], ptr[1], ptr[0], ptr[3]};
            case PixelFormat::Grayscale8:
                return {ptr[0], ptr[0], ptr[0], 255};
            case PixelFormat::Indexed8:
                return {ptr[0], ptr[0], ptr[0], 255}; // يحتاج لوح ألوان
            default:
                return {};
        }
    }

    /// الكتابة في ذاكرة بنمط معين
    void writeToMemory(uint8_t* ptr, PixelFormat format) const {
        switch (format) {
            case PixelFormat::RGB565:
                *(uint16_t*)ptr = toRGB565();
                break;
            case PixelFormat::RGB888:
                ptr[0] = r; ptr[1] = g; ptr[2] = b;
                break;
            case PixelFormat::RGBA8888:
                ptr[0] = r; ptr[1] = g; ptr[2] = b; ptr[3] = a;
                break;
            case PixelFormat::BGR888:
                ptr[0] = b; ptr[1] = g; ptr[2] = r;
                break;
            case PixelFormat::BGRA8888:
                ptr[0] = b; ptr[1] = g; ptr[2] = r; ptr[3] = a;
                break;
            case PixelFormat::Grayscale8:
                ptr[0] = (uint8_t)((r * 77 + g * 150 + b * 29) >> 8);
                break;
            case PixelFormat::Indexed8:
                ptr[0] = (uint8_t)((r * 77 + g * 150 + b * 29) >> 8);
                break;
        }
    }

    /// ألوان مسماة
    static PixelColor Black()   { return {0, 0, 0}; }
    static PixelColor White()   { return {255, 255, 255}; }
    static PixelColor Red()     { return {255, 0, 0}; }
    static PixelColor Green()   { return {0, 255, 0}; }
    static PixelColor Blue()    { return {0, 0, 255}; }
    static PixelColor Yellow()  { return {255, 255, 0}; }
    static PixelColor Cyan()    { return {0, 255, 255}; }
    static PixelColor Magenta() { return {255, 0, 255}; }
    static PixelColor Gray()    { return {128, 128, 128}; }
    static PixelColor Transparent() { return {0, 0, 0, 0}; }
};

// =============================================================================
// منطقة القص - Clip Rectangle
// =============================================================================

/// مستطيل لتحديد منطقة القص
struct ClipRect {
    int x = 0, y = 0;
    int w = 0, h = 0;

    bool contains(int px, int py) const {
        return px >= x && px < x + w && py >= y && py < y + h;
    }

    /// تقاطع مستطيلين
    ClipRect intersect(const ClipRect& other) const {
        int ix = std::max(x, other.x);
        int iy = std::max(y, other.y);
        int ir = std::min(x + w, other.x + other.w);
        int ib = std::min(y + h, other.y + other.h);
        if (ir <= ix || ib <= iy) return {0, 0, 0, 0};
        return {ix, iy, ir - ix, ib - iy};
    }
};

// =============================================================================
// المخزن المؤقت للإطار - Framebuffer
// =============================================================================

/// لوح ألوان للأنماط المفهرسة (256 لون)
struct ColorPalette {
    PixelColor colors[256];
    int count = 256;

    /// لوح VGA الافتراضي (16 لون)
    static ColorPalette VGA16() {
        ColorPalette p;
        p.count = 16;
        p.colors[0]  = {0, 0, 0};       // أسود
        p.colors[1]  = {0, 0, 170};     // أزرق
        p.colors[2]  = {0, 170, 0};     // أخضر
        p.colors[3]  = {0, 170, 170};   // سماوي
        p.colors[4]  = {170, 0, 0};     // أحمر
        p.colors[5]  = {170, 0, 170};   // أرجواني
        p.colors[6]  = {170, 85, 0};    // بني
        p.colors[7]  = {170, 170, 170}; // رمادي فاتح
        p.colors[8]  = {85, 85, 85};    // رمادي غامق
        p.colors[9]  = {85, 85, 255};   // أزرق فاتح
        p.colors[10] = {85, 255, 85};   // أخضر فاتح
        p.colors[11] = {85, 255, 255};  // سماوي فاتح
        p.colors[12] = {255, 85, 85};   // أحمر فاتح
        p.colors[13] = {255, 85, 255};  // أرجواني فاتح
        p.colors[14] = {255, 255, 85};  // أصفر
        p.colors[15] = {255, 255, 255}; // أبيض
        return p;
    }
};

/// المخزن المؤقت للإطار الرئيسي
class OSFramebuffer {
public:
    /// إنشاء مخزن مؤقت
    OSFramebuffer() = default;
    OSFramebuffer(int width, int height, PixelFormat format = PixelFormat::RGBA8888) {
        init(width, height, format);
    }

    /// تهيئة المخزن
    void init(int width, int height, PixelFormat format = PixelFormat::RGBA8888) {
        m_width = width;
        m_height = height;
        m_format = format;
        m_bytesPerPixel = getBytesPerPixel(format);
        m_pitch = width * m_bytesPerPixel;
        m_buffer.resize(m_pitch * height, 0);
        m_clipRect = {0, 0, width, height};
        // إنشاء المخزن الخلفي للتخزين المزدوج
        m_backBuffer.resize(m_buffer.size(), 0);
    }

    /// تهيئة من عنوان ذاكرة مباشر (لأنظمة التشغيل)
    void initFromAddress(void* address, int width, int height, int pitch,
                         PixelFormat format = PixelFormat::RGBA8888) {
        m_width = width;
        m_height = height;
        m_format = format;
        m_bytesPerPixel = getBytesPerPixel(format);
        m_pitch = pitch;
        m_externalBuffer = (uint8_t*)address;
        m_clipRect = {0, 0, width, height};
        m_backBuffer.resize(pitch * height, 0);
    }

    // ---- عمليات البكسل ----

    /// رسم بكسل واحد
    void setPixel(int x, int y, const PixelColor& color) {
        if (!m_clipRect.contains(x, y)) return;
        uint8_t* ptr = getPixelPtr(x, y);
        if (!ptr) return;
        if (color.a == 255) {
            color.writeToMemory(ptr, m_format);
        } else if (color.a > 0) {
            // خلط ألوان
            PixelColor dst = PixelColor::readFromMemory(ptr, m_format);
            PixelColor blended = PixelColor::blend(color, dst);
            blended.writeToMemory(ptr, m_format);
        }
    }

    /// قراءة بكسل
    PixelColor getPixel(int x, int y) const {
        if (x < 0 || x >= m_width || y < 0 || y >= m_height) return {};
        const uint8_t* ptr = getPixelPtrConst(x, y);
        return PixelColor::readFromMemory(ptr, m_format);
    }

    /// مسح الشاشة بلون
    void clear(const PixelColor& color = PixelColor::Black()) {
        if (m_format == PixelFormat::RGBA8888) {
            uint32_t packed = color.toPacked();
            uint32_t* ptr = (uint32_t*)getBuffer();
            int count = m_width * m_height;
            for (int i = 0; i < count; i++) ptr[i] = packed;
        } else {
            for (int y = 0; y < m_height; y++) {
                for (int x = 0; x < m_width; x++) {
                    uint8_t* ptr = getPixelPtr(x, y);
                    color.writeToMemory(ptr, m_format);
                }
            }
        }
    }

    // ---- رسم الخطوط ----

    /// رسم خط أفقي (سريع)
    void drawHLine(int x, int y, int width, const PixelColor& color) {
        // قص
        int x1 = std::max(x, m_clipRect.x);
        int x2 = std::min(x + width, m_clipRect.x + m_clipRect.w);
        if (y < m_clipRect.y || y >= m_clipRect.y + m_clipRect.h || x1 >= x2) return;

        if (color.a == 255 && m_format == PixelFormat::RGBA8888) {
            uint32_t packed = color.toPacked();
            uint32_t* ptr = (uint32_t*)(getBuffer() + y * m_pitch + x1 * 4);
            for (int i = 0; i < x2 - x1; i++) ptr[i] = packed;
        } else {
            for (int px = x1; px < x2; px++) setPixel(px, y, color);
        }
    }

    /// رسم خط رأسي (سريع)
    void drawVLine(int x, int y, int height, const PixelColor& color) {
        int y1 = std::max(y, m_clipRect.y);
        int y2 = std::min(y + height, m_clipRect.y + m_clipRect.h);
        if (x < m_clipRect.x || x >= m_clipRect.x + m_clipRect.w || y1 >= y2) return;
        for (int py = y1; py < y2; py++) setPixel(x, py, color);
    }

    /// رسم خط عام (خوارزمية Bresenham)
    void drawLine(int x0, int y0, int x1, int y1, const PixelColor& color) {
        int dx = std::abs(x1 - x0);
        int dy = std::abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1;
        int sy = y0 < y1 ? 1 : -1;
        int err = dx - dy;

        while (true) {
            setPixel(x0, y0, color);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x0 += sx; }
            if (e2 < dx) { err += dx; y0 += sy; }
        }
    }

    /// رسم خط بسمك
    void drawThickLine(int x0, int y0, int x1, int y1, int thickness, const PixelColor& color) {
        float dx = (float)(x1 - x0);
        float dy = (float)(y1 - y0);
        float len = std::sqrt(dx * dx + dy * dy);
        if (len == 0) return;
        float nx = -dy / len * thickness * 0.5f;
        float ny = dx / len * thickness * 0.5f;
        // رسم مستطيل مائل عبر خطوط متوازية
        for (int t = -(thickness/2); t <= thickness/2; t++) {
            int ox = (int)(nx * t / (thickness * 0.5f));
            int oy = (int)(ny * t / (thickness * 0.5f));
            drawLine(x0 + ox, y0 + oy, x1 + ox, y1 + oy, color);
        }
    }

    // ---- رسم المستطيلات ----

    /// رسم مستطيل (حدود فقط)
    void drawRect(int x, int y, int w, int h, const PixelColor& color) {
        drawHLine(x, y, w, color);
        drawHLine(x, y + h - 1, w, color);
        drawVLine(x, y, h, color);
        drawVLine(x + w - 1, y, h, color);
    }

    /// رسم مستطيل معبأ
    void fillRect(int x, int y, int w, int h, const PixelColor& color) {
        for (int py = y; py < y + h; py++) {
            drawHLine(x, py, w, color);
        }
    }

    /// رسم مستطيل بزوايا مستديرة
    void drawRoundRect(int x, int y, int w, int h, int radius, const PixelColor& color) {
        // الحواف الأفقية
        drawHLine(x + radius, y, w - 2 * radius, color);
        drawHLine(x + radius, y + h - 1, w - 2 * radius, color);
        // الحواف الرأسية
        drawVLine(x, y + radius, h - 2 * radius, color);
        drawVLine(x + w - 1, y + radius, h - 2 * radius, color);
        // الزوايا
        drawCircleQuarter(x + radius, y + radius, radius, 1, color);
        drawCircleQuarter(x + w - 1 - radius, y + radius, radius, 0, color);
        drawCircleQuarter(x + radius, y + h - 1 - radius, radius, 2, color);
        drawCircleQuarter(x + w - 1 - radius, y + h - 1 - radius, radius, 3, color);
    }

    /// رسم مستطيل معبأ بزوايا مستديرة
    void fillRoundRect(int x, int y, int w, int h, int radius, const PixelColor& color) {
        // المنطقة المركزية
        fillRect(x + radius, y, w - 2 * radius, h, color);
        fillRect(x, y + radius, radius, h - 2 * radius, color);
        fillRect(x + w - radius, y + radius, radius, h - 2 * radius, color);
        // الزوايا المعبأة
        fillCircleQuarter(x + radius, y + radius, radius, 1, color);
        fillCircleQuarter(x + w - 1 - radius, y + radius, radius, 0, color);
        fillCircleQuarter(x + radius, y + h - 1 - radius, radius, 2, color);
        fillCircleQuarter(x + w - 1 - radius, y + h - 1 - radius, radius, 3, color);
    }

    // ---- رسم الدوائر ----

    /// رسم دائرة (خوارزمية Midpoint)
    void drawCircle(int cx, int cy, int radius, const PixelColor& color) {
        int x = 0, y = radius;
        int d = 1 - radius;
        while (x <= y) {
            setPixel(cx + x, cy + y, color);
            setPixel(cx - x, cy + y, color);
            setPixel(cx + x, cy - y, color);
            setPixel(cx - x, cy - y, color);
            setPixel(cx + y, cy + x, color);
            setPixel(cx - y, cy + x, color);
            setPixel(cx + y, cy - x, color);
            setPixel(cx - y, cy - x, color);
            if (d < 0) {
                d += 2 * x + 3;
            } else {
                d += 2 * (x - y) + 5;
                y--;
            }
            x++;
        }
    }

    /// رسم دائرة معبأة
    void fillCircle(int cx, int cy, int radius, const PixelColor& color) {
        int x = 0, y = radius;
        int d = 1 - radius;
        while (x <= y) {
            drawHLine(cx - x, cy + y, 2 * x + 1, color);
            drawHLine(cx - x, cy - y, 2 * x + 1, color);
            drawHLine(cx - y, cy + x, 2 * y + 1, color);
            drawHLine(cx - y, cy - x, 2 * y + 1, color);
            if (d < 0) {
                d += 2 * x + 3;
            } else {
                d += 2 * (x - y) + 5;
                y--;
            }
            x++;
        }
    }

    /// رسم قوس دائري (ربع)
    void drawCircleQuarter(int cx, int cy, int radius, int quarter, const PixelColor& color) {
        int x = 0, y = radius;
        int d = 1 - radius;
        while (x <= y) {
            switch (quarter) {
                case 0: setPixel(cx+x, cy-y, color); setPixel(cx+y, cy-x, color); break;
                case 1: setPixel(cx-x, cy-y, color); setPixel(cx-y, cy-x, color); break;
                case 2: setPixel(cx-x, cy+y, color); setPixel(cx-y, cy+x, color); break;
                case 3: setPixel(cx+x, cy+y, color); setPixel(cx+y, cy+x, color); break;
            }
            if (d < 0) d += 2 * x + 3;
            else { d += 2 * (x - y) + 5; y--; }
            x++;
        }
    }

    /// ملء ربع دائرة
    void fillCircleQuarter(int cx, int cy, int radius, int quarter, const PixelColor& color) {
        int x = 0, y = radius;
        int d = 1 - radius;
        while (x <= y) {
            switch (quarter) {
                case 0:
                    drawHLine(cx, cy - y, x + 1, color);
                    drawHLine(cx, cy - x, y + 1, color);
                    break;
                case 1:
                    drawHLine(cx - x, cy - y, x + 1, color);
                    drawHLine(cx - y, cy - x, y + 1, color);
                    break;
                case 2:
                    drawHLine(cx - x, cy + y, x + 1, color);
                    drawHLine(cx - y, cy + x, y + 1, color);
                    break;
                case 3:
                    drawHLine(cx, cy + y, x + 1, color);
                    drawHLine(cx, cy + x, y + 1, color);
                    break;
            }
            if (d < 0) d += 2 * x + 3;
            else { d += 2 * (x - y) + 5; y--; }
            x++;
        }
    }

    /// رسم قطع ناقص
    void drawEllipse(int cx, int cy, int rx, int ry, const PixelColor& color) {
        int x = 0, y = ry;
        long rx2 = (long)rx * rx, ry2 = (long)ry * ry;
        long tworx2 = 2 * rx2, twory2 = 2 * ry2;
        long px = 0, py = tworx2 * y;
        long p = (long)(ry2 - rx2 * ry + 0.25 * rx2);
        while (px < py) {
            setPixel(cx+x, cy+y, color); setPixel(cx-x, cy+y, color);
            setPixel(cx+x, cy-y, color); setPixel(cx-x, cy-y, color);
            x++; px += twory2;
            if (p < 0) p += ry2 + px;
            else { y--; py -= tworx2; p += ry2 + px - py; }
        }
        p = (long)(ry2*(x+0.5)*(x+0.5) + rx2*(y-1)*(y-1) - rx2*ry2);
        while (y >= 0) {
            setPixel(cx+x, cy+y, color); setPixel(cx-x, cy+y, color);
            setPixel(cx+x, cy-y, color); setPixel(cx-x, cy-y, color);
            y--; py -= tworx2;
            if (p > 0) p += rx2 - py;
            else { x++; px += twory2; p += rx2 - py + px; }
        }
    }

    // ---- رسم المثلثات ----

    /// رسم مثلث (حدود)
    void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const PixelColor& color) {
        drawLine(x0, y0, x1, y1, color);
        drawLine(x1, y1, x2, y2, color);
        drawLine(x2, y2, x0, y0, color);
    }

    /// رسم مثلث معبأ (خوارزمية المسح الخطي)
    void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, const PixelColor& color) {
        // ترتيب النقاط حسب y
        if (y0 > y1) { std::swap(x0, x1); std::swap(y0, y1); }
        if (y0 > y2) { std::swap(x0, x2); std::swap(y0, y2); }
        if (y1 > y2) { std::swap(x1, x2); std::swap(y1, y2); }

        if (y0 == y2) return; // مثلث منحل

        for (int y = y0; y <= y2; y++) {
            bool secondHalf = y > y1 || y1 == y0;
            float alpha = (float)(y - y0) / (y2 - y0);
            float beta;
            if (secondHalf)
                beta = (y1 == y2) ? 1.0f : (float)(y - y1) / (y2 - y1);
            else
                beta = (y0 == y1) ? 0.0f : (float)(y - y0) / (y1 - y0);

            int ax = (int)(x0 + (x2 - x0) * alpha);
            int bx;
            if (secondHalf)
                bx = (int)(x1 + (x2 - x1) * beta);
            else
                bx = (int)(x0 + (x1 - x0) * beta);

            if (ax > bx) std::swap(ax, bx);
            drawHLine(ax, y, bx - ax + 1, color);
        }
    }

    // ---- نسخ المناطق (Blit) ----

    /// نسخ من مخزن آخر
    void blit(const OSFramebuffer& src, int dx, int dy) {
        blitRegion(src, 0, 0, src.m_width, src.m_height, dx, dy);
    }

    /// نسخ منطقة من مخزن آخر
    void blitRegion(const OSFramebuffer& src, int sx, int sy, int sw, int sh, int dx, int dy) {
        for (int y = 0; y < sh; y++) {
            for (int x = 0; x < sw; x++) {
                PixelColor c = src.getPixel(sx + x, sy + y);
                setPixel(dx + x, dy + y, c);
            }
        }
    }

    /// نسخ مع تحجيم
    void blitScaled(const OSFramebuffer& src, int dx, int dy, int dw, int dh) {
        if (dw <= 0 || dh <= 0) return;
        for (int y = 0; y < dh; y++) {
            for (int x = 0; x < dw; x++) {
                int sx = x * src.m_width / dw;
                int sy = y * src.m_height / dh;
                setPixel(dx + x, dy + y, src.getPixel(sx, sy));
            }
        }
    }

    /// نسخ مع شفافية (Per-pixel alpha)
    void blitAlpha(const OSFramebuffer& src, int dx, int dy) {
        for (int y = 0; y < src.m_height; y++) {
            for (int x = 0; x < src.m_width; x++) {
                PixelColor srcColor = src.getPixel(x, y);
                if (srcColor.a > 0)
                    setPixel(dx + x, dy + y, srcColor);
            }
        }
    }

    // ---- التخزين المزدوج ----

    /// تبديل المخازن المؤقتة
    void swapBuffers() {
        if (m_externalBuffer) {
            // نسخ من المخزن الخلفي إلى العنوان الخارجي
            std::memcpy(m_externalBuffer, m_backBuffer.data(), m_backBuffer.size());
        } else {
            m_buffer.swap(m_backBuffer);
        }
    }

    /// الرسم في المخزن الخلفي
    void setDrawToBackBuffer(bool back) { m_drawToBack = back; }

    // ---- القص ----

    /// ضبط منطقة القص
    void setClipRect(int x, int y, int w, int h) {
        m_clipRect = {x, y, w, h};
        // التأكد من أن منطقة القص ضمن حدود الشاشة
        m_clipRect = m_clipRect.intersect({0, 0, m_width, m_height});
    }

    /// إعادة منطقة القص للكامل
    void resetClipRect() {
        m_clipRect = {0, 0, m_width, m_height};
    }

    // ---- تأثيرات ----

    /// تعبئة متدرجة أفقياً
    void fillGradientH(int x, int y, int w, int h,
                       const PixelColor& startColor, const PixelColor& endColor) {
        for (int px = 0; px < w; px++) {
            float t = (float)px / (float)(w - 1);
            PixelColor c;
            c.r = (uint8_t)(startColor.r + (endColor.r - startColor.r) * t);
            c.g = (uint8_t)(startColor.g + (endColor.g - startColor.g) * t);
            c.b = (uint8_t)(startColor.b + (endColor.b - startColor.b) * t);
            c.a = (uint8_t)(startColor.a + (endColor.a - startColor.a) * t);
            drawVLine(x + px, y, h, c);
        }
    }

    /// تعبئة متدرجة رأسياً
    void fillGradientV(int x, int y, int w, int h,
                       const PixelColor& startColor, const PixelColor& endColor) {
        for (int py = 0; py < h; py++) {
            float t = (float)py / (float)(h - 1);
            PixelColor c;
            c.r = (uint8_t)(startColor.r + (endColor.r - startColor.r) * t);
            c.g = (uint8_t)(startColor.g + (endColor.g - startColor.g) * t);
            c.b = (uint8_t)(startColor.b + (endColor.b - startColor.b) * t);
            c.a = (uint8_t)(startColor.a + (endColor.a - startColor.a) * t);
            drawHLine(x, y + py, w, c);
        }
    }

    /// تمويه بسيط (Box Blur)
    void boxBlur(int radius) {
        if (radius <= 0) return;
        std::vector<uint8_t> temp(m_buffer.size());
        // تمرير أفقي
        for (int y = 0; y < m_height; y++) {
            for (int x = 0; x < m_width; x++) {
                int r = 0, g = 0, b = 0, a = 0, count = 0;
                for (int dx = -radius; dx <= radius; dx++) {
                    int px = x + dx;
                    if (px >= 0 && px < m_width) {
                        PixelColor c = getPixel(px, y);
                        r += c.r; g += c.g; b += c.b; a += c.a; count++;
                    }
                }
                PixelColor result((uint8_t)(r/count), (uint8_t)(g/count),
                                  (uint8_t)(b/count), (uint8_t)(a/count));
                result.writeToMemory(&temp[y * m_pitch + x * m_bytesPerPixel], m_format);
            }
        }
        std::memcpy(getBuffer(), temp.data(), temp.size());
    }

    // ---- معلومات ----

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    int getPitch() const { return m_pitch; }
    int getBpp() const { return m_bytesPerPixel; }
    PixelFormat getFormat() const { return m_format; }
    const ClipRect& getClipRect() const { return m_clipRect; }

    /// الحصول على مؤشر البيانات
    uint8_t* getData() { return getBuffer(); }
    const uint8_t* getData() const { return getBufferConst(); }

    /// الحصول على حجم البيانات بالبايت
    size_t getDataSize() const { return m_pitch * m_height; }

    /// الحصول على عدد البايتات لكل بكسل حسب النمط
    static int getBytesPerPixel(PixelFormat format) {
        switch (format) {
            case PixelFormat::RGB565:     return 2;
            case PixelFormat::RGB888:     return 3;
            case PixelFormat::BGR888:     return 3;
            case PixelFormat::RGBA8888:   return 4;
            case PixelFormat::BGRA8888:   return 4;
            case PixelFormat::Grayscale8: return 1;
            case PixelFormat::Indexed8:   return 1;
            default: return 4;
        }
    }

private:
    int m_width = 0;                ///< العرض
    int m_height = 0;               ///< الارتفاع
    int m_pitch = 0;                ///< عرض السطر بالبايت
    int m_bytesPerPixel = 4;        ///< بايت لكل بكسل
    PixelFormat m_format = PixelFormat::RGBA8888;
    std::vector<uint8_t> m_buffer;      ///< المخزن الأمامي
    std::vector<uint8_t> m_backBuffer;  ///< المخزن الخلفي
    uint8_t* m_externalBuffer = nullptr; ///< مؤشر خارجي (للنواة)
    bool m_drawToBack = false;          ///< الرسم في المخزن الخلفي
    ClipRect m_clipRect;                ///< منطقة القص

    /// الحصول على مؤشر بكسل
    uint8_t* getPixelPtr(int x, int y) {
        return getBuffer() + y * m_pitch + x * m_bytesPerPixel;
    }

    const uint8_t* getPixelPtrConst(int x, int y) const {
        return getBufferConst() + y * m_pitch + x * m_bytesPerPixel;
    }

    uint8_t* getBuffer() {
        if (m_drawToBack) return m_backBuffer.data();
        return m_externalBuffer ? m_externalBuffer : m_buffer.data();
    }

    const uint8_t* getBufferConst() const {
        if (m_drawToBack) return m_backBuffer.data();
        return m_externalBuffer ? m_externalBuffer : m_buffer.data();
    }
};

// =============================================================================
// مدير الشاشة - Display Manager
// =============================================================================

/// معلومات وضع العرض
struct DisplayMode {
    int width = 0;          ///< العرض
    int height = 0;         ///< الارتفاع
    int bpp = 32;           ///< عمق الألوان
    int refreshRate = 60;   ///< معدل التحديث
    PixelFormat format = PixelFormat::RGBA8888;
};

/// مدير الشاشة لنظام التشغيل
class DisplayManager {
public:
    /// تهيئة المدير
    void init(int width, int height, PixelFormat format = PixelFormat::RGBA8888) {
        m_currentMode = {width, height, OSFramebuffer::getBytesPerPixel(format) * 8, 60, format};
        m_framebuffer.init(width, height, format);
    }

    /// تهيئة من عنوان ذاكرة (لنظام التشغيل/الإقلاع)
    void initFromAddress(void* addr, int width, int height, int pitch, PixelFormat format) {
        m_currentMode = {width, height, OSFramebuffer::getBytesPerPixel(format) * 8, 60, format};
        m_framebuffer.initFromAddress(addr, width, height, pitch, format);
    }

    /// تغيير وضع العرض
    void setMode(const DisplayMode& mode) {
        m_currentMode = mode;
        m_framebuffer.init(mode.width, mode.height, mode.format);
    }

    /// الحصول على المخزن المؤقت
    OSFramebuffer& getFramebuffer() { return m_framebuffer; }
    const OSFramebuffer& getFramebuffer() const { return m_framebuffer; }

    /// الحصول على وضع العرض الحالي
    const DisplayMode& getCurrentMode() const { return m_currentMode; }

    /// إضافة وضع عرض مدعوم
    void addSupportedMode(const DisplayMode& mode) {
        m_supportedModes.push_back(mode);
    }

    /// الحصول على الأوضاع المدعومة
    const std::vector<DisplayMode>& getSupportedModes() const { return m_supportedModes; }

    /// تبديل المخازن
    void swapBuffers() { m_framebuffer.swapBuffers(); }

private:
    OSFramebuffer m_framebuffer;                ///< المخزن المؤقت
    DisplayMode m_currentMode;                  ///< الوضع الحالي
    std::vector<DisplayMode> m_supportedModes;  ///< الأوضاع المدعومة
};

} // namespace os
} // namespace graphics
} // namespace sad
