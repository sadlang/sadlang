/*
 * ============================================================================
 * تنفيذ المخزن المؤقت للإطارات و GPU - لغة ص
 * Framebuffer & GPU Implementation - Sad Language
 * ============================================================================
 */

#include "framebuffer.h"
#include "io_ports.h"
#include <cstring>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace Sad {
namespace LowLevel {

// ============================================================================
// المثيل الوحيد / Singleton
// ============================================================================

FramebufferManager& FramebufferManager::getInstance() {
    static FramebufferManager instance;
    return instance;
}

FramebufferManager::FramebufferManager()
    : frontBuffer_(nullptr)
    , backBuffer_(nullptr)
    , bufferSize_(0)
    , doubleBuffered_(true)
    , clipEnabled_(false)
    , cursorWidth_(0)
    , cursorHeight_(0)
    , cursorX_(0)
    , cursorY_(0)
    , cursorHotX_(0)
    , cursorHotY_(0)
    , cursorVisible_(false)
    , vsyncEnabled_(true)
    , isInitialized_(false) {}

// ============================================================================
// تهيئة / Initialization
// ============================================================================

bool FramebufferManager::initFromVBE(const VBEModeInfo* modeInfo) {
    if (!modeInfo) return false;

    currentMode_.width = modeInfo->width;
    currentMode_.height = modeInfo->height;
    currentMode_.bitsPerPixel = modeInfo->bpp;
    currentMode_.pitch = modeInfo->pitch;
    currentMode_.framebufferAddr = modeInfo->framebuffer;
    currentMode_.framebufferSize = static_cast<uint64_t>(modeInfo->pitch) * modeInfo->height;

    // تحديد تنسيق البكسل / Determine pixel format
    if (modeInfo->bpp == 32) {
        if (modeInfo->redFieldPosition == 16)
            currentMode_.format = PixelFormat::BGRA_8888;
        else
            currentMode_.format = PixelFormat::RGBA_8888;
    } else if (modeInfo->bpp == 24) {
        if (modeInfo->redFieldPosition == 16)
            currentMode_.format = PixelFormat::BGR_888;
        else
            currentMode_.format = PixelFormat::RGB_888;
    } else if (modeInfo->bpp == 16) {
        currentMode_.format = PixelFormat::RGB_565;
    } else {
        currentMode_.format = PixelFormat::INDEXED_8;
    }

    return initManual(currentMode_.framebufferAddr, currentMode_.width,
                     currentMode_.height, currentMode_.pitch,
                     currentMode_.bitsPerPixel, currentMode_.format);
}

bool FramebufferManager::initFromGOP(const GOPModeInfo* modeInfo,
                                      uint64_t framebufferBase,
                                      uint64_t framebufferSize) {
    if (!modeInfo) return false;

    currentMode_.width = modeInfo->horizontalResolution;
    currentMode_.height = modeInfo->verticalResolution;
    currentMode_.bitsPerPixel = 32;
    currentMode_.pitch = modeInfo->pixelsPerScanLine * 4;
    currentMode_.framebufferAddr = framebufferBase;
    currentMode_.framebufferSize = framebufferSize;

    switch (modeInfo->pixelFormat) {
        case GOPPixelFormat::PixelRedGreenBlueReserved8BitPerColor:
            currentMode_.format = PixelFormat::RGBA_8888;
            break;
        case GOPPixelFormat::PixelBlueGreenRedReserved8BitPerColor:
            currentMode_.format = PixelFormat::BGRA_8888;
            break;
        default:
            currentMode_.format = PixelFormat::BGRA_8888;
            break;
    }

    return initManual(framebufferBase, currentMode_.width, currentMode_.height,
                     currentMode_.pitch, 32, currentMode_.format);
}

bool FramebufferManager::initManual(uint64_t framebufferAddr, uint32_t width,
                                     uint32_t height, uint32_t pitch,
                                     uint8_t bpp, PixelFormat format) {
    currentMode_.width = width;
    currentMode_.height = height;
    currentMode_.bitsPerPixel = bpp;
    currentMode_.pitch = pitch;
    currentMode_.format = format;
    currentMode_.framebufferAddr = framebufferAddr;
    currentMode_.framebufferSize = static_cast<uint64_t>(pitch) * height;

    frontBuffer_ = reinterpret_cast<volatile uint32_t*>(framebufferAddr);
    bufferSize_ = width * height;

    // تخصيص المخزن الخلفي / Allocate back buffer
    if (backBuffer_) {
        delete[] backBuffer_;
    }
    backBuffer_ = new uint32_t[bufferSize_];
    std::memset(backBuffer_, 0, bufferSize_ * sizeof(uint32_t));

    // إعداد القص / Setup clipping
    clipRect_ = Rect(0, 0, width, height);
    clipEnabled_ = false;

    isInitialized_ = true;
    return true;
}

bool FramebufferManager::initGPU(const GPUInfo& gpu) {
    gpuInfo_ = gpu;
    return true;
}

bool FramebufferManager::setVideoMode(uint32_t width, uint32_t height, uint8_t bpp) {
    // البحث عن وضع مطابق / Find matching mode
    for (const auto& mode : availableModes_) {
        if (mode.width == width && mode.height == height && mode.bitsPerPixel == bpp) {
            // في نظام حقيقي: استدعاء INT 10h AX=4F02h مع رقم الوضع
            // In real system: call INT 10h AX=4F02h with mode number
            return initManual(mode.framebufferAddr, mode.width, mode.height,
                            mode.pitch, mode.bitsPerPixel, mode.format);
        }
    }
    return false;
}

std::vector<VideoMode> FramebufferManager::getAvailableModes() const {
    return availableModes_;
}

// ============================================================================
// عمليات البكسل / Pixel Operations
// ============================================================================

bool FramebufferManager::inBounds(int32_t x, int32_t y) const {
    return x >= 0 && x < static_cast<int32_t>(currentMode_.width) &&
           y >= 0 && y < static_cast<int32_t>(currentMode_.height);
}

bool FramebufferManager::isClipped(int32_t x, int32_t y) const {
    if (!clipEnabled_) return false;
    return !clipRect_.contains(x, y);
}

uint32_t* FramebufferManager::pixelAt(int32_t x, int32_t y) {
    return &backBuffer_[y * static_cast<int32_t>(currentMode_.width) + x];
}

void FramebufferManager::putPixel(int32_t x, int32_t y, const Color& color) {
    if (!isInitialized_ || !inBounds(x, y) || isClipped(x, y)) return;
    *pixelAt(x, y) = color.toUint32();
}

Color FramebufferManager::getPixel(int32_t x, int32_t y) const {
    if (!isInitialized_ || !inBounds(x, y)) return Colors::Black;
    uint32_t val = backBuffer_[y * static_cast<int32_t>(currentMode_.width) + x];
    return Color::fromUint32(val);
}

void FramebufferManager::putPixelAlpha(int32_t x, int32_t y, const Color& color) {
    if (!isInitialized_ || !inBounds(x, y) || isClipped(x, y)) return;
    if (color.alpha == 255) {
        *pixelAt(x, y) = color.toUint32();
    } else if (color.alpha > 0) {
        Color existing = getPixel(x, y);
        *pixelAt(x, y) = existing.blend(color).toUint32();
    }
}

// ============================================================================
// خطوط سريعة / Fast Lines
// ============================================================================

void FramebufferManager::hLine(int32_t x1, int32_t x2, int32_t y, const Color& color) {
    if (!isInitialized_) return;
    if (y < 0 || y >= static_cast<int32_t>(currentMode_.height)) return;

    if (x1 > x2) { int32_t t = x1; x1 = x2; x2 = t; }
    if (x1 < 0) x1 = 0;
    if (x2 >= static_cast<int32_t>(currentMode_.width)) x2 = currentMode_.width - 1;

    if (clipEnabled_) {
        if (y < clipRect_.y || y >= clipRect_.y + static_cast<int32_t>(clipRect_.height)) return;
        if (x1 < clipRect_.x) x1 = clipRect_.x;
        if (x2 >= clipRect_.x + static_cast<int32_t>(clipRect_.width))
            x2 = clipRect_.x + static_cast<int32_t>(clipRect_.width) - 1;
    }

    uint32_t c = color.toUint32();
    uint32_t* ptr = pixelAt(x1, y);
    int32_t count = x2 - x1 + 1;

    // تعبئة سريعة بـ 4 بكسل / Fast fill 4 pixels at a time
    while (count >= 4) {
        ptr[0] = c; ptr[1] = c; ptr[2] = c; ptr[3] = c;
        ptr += 4; count -= 4;
    }
    while (count-- > 0) {
        *ptr++ = c;
    }
}

void FramebufferManager::vLine(int32_t x, int32_t y1, int32_t y2, const Color& color) {
    if (!isInitialized_) return;
    if (x < 0 || x >= static_cast<int32_t>(currentMode_.width)) return;

    if (y1 > y2) { int32_t t = y1; y1 = y2; y2 = t; }
    if (y1 < 0) y1 = 0;
    if (y2 >= static_cast<int32_t>(currentMode_.height)) y2 = currentMode_.height - 1;

    if (clipEnabled_) {
        if (x < clipRect_.x || x >= clipRect_.x + static_cast<int32_t>(clipRect_.width)) return;
        if (y1 < clipRect_.y) y1 = clipRect_.y;
        if (y2 >= clipRect_.y + static_cast<int32_t>(clipRect_.height))
            y2 = clipRect_.y + static_cast<int32_t>(clipRect_.height) - 1;
    }

    uint32_t c = color.toUint32();
    uint32_t w = currentMode_.width;
    uint32_t* ptr = pixelAt(x, y1);
    for (int32_t y = y1; y <= y2; y++) {
        *ptr = c;
        ptr += w;
    }
}

// ============================================================================
// عمليات الأشكال / Shape Operations
// ============================================================================

void FramebufferManager::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                                   const Color& color) {
    if (!isInitialized_) return;

    // حالة خط أفقي / Horizontal line case
    if (y1 == y2) { hLine(x1, x2, y1, color); return; }
    // حالة خط عمودي / Vertical line case
    if (x1 == x2) { vLine(x1, y1, y2, color); return; }

    // خوارزمية بريزنهام / Bresenham's algorithm
    int32_t dx = (x2 > x1) ? (x2 - x1) : (x1 - x2);
    int32_t dy = (y2 > y1) ? (y2 - y1) : (y1 - y2);
    int32_t sx = (x1 < x2) ? 1 : -1;
    int32_t sy = (y1 < y2) ? 1 : -1;
    int32_t err = dx - dy;

    while (true) {
        putPixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int32_t e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx)  { err += dx; y1 += sy; }
    }
}

void FramebufferManager::drawRect(const Rect& rect, const Color& color) {
    hLine(rect.x, rect.x + static_cast<int32_t>(rect.width) - 1, rect.y, color);
    hLine(rect.x, rect.x + static_cast<int32_t>(rect.width) - 1,
          rect.y + static_cast<int32_t>(rect.height) - 1, color);
    vLine(rect.x, rect.y, rect.y + static_cast<int32_t>(rect.height) - 1, color);
    vLine(rect.x + static_cast<int32_t>(rect.width) - 1, rect.y,
          rect.y + static_cast<int32_t>(rect.height) - 1, color);
}

void FramebufferManager::fillRect(const Rect& rect, const Color& color) {
    for (int32_t y = rect.y; y < rect.y + static_cast<int32_t>(rect.height); y++) {
        hLine(rect.x, rect.x + static_cast<int32_t>(rect.width) - 1, y, color);
    }
}

void FramebufferManager::drawCircle(int32_t cx, int32_t cy, int32_t radius,
                                     const Color& color) {
    if (!isInitialized_ || radius <= 0) return;

    // خوارزمية نقطة المنتصف / Midpoint circle algorithm
    int32_t x = radius;
    int32_t y = 0;
    int32_t d = 1 - radius;

    while (x >= y) {
        putPixel(cx + x, cy + y, color);
        putPixel(cx - x, cy + y, color);
        putPixel(cx + x, cy - y, color);
        putPixel(cx - x, cy - y, color);
        putPixel(cx + y, cy + x, color);
        putPixel(cx - y, cy + x, color);
        putPixel(cx + y, cy - x, color);
        putPixel(cx - y, cy - x, color);

        y++;
        if (d <= 0) {
            d += 2 * y + 1;
        } else {
            x--;
            d += 2 * (y - x) + 1;
        }
    }
}

void FramebufferManager::fillCircle(int32_t cx, int32_t cy, int32_t radius,
                                     const Color& color) {
    if (!isInitialized_ || radius <= 0) return;

    int32_t x = radius;
    int32_t y = 0;
    int32_t d = 1 - radius;

    while (x >= y) {
        hLine(cx - x, cx + x, cy + y, color);
        hLine(cx - x, cx + x, cy - y, color);
        hLine(cx - y, cx + y, cy + x, color);
        hLine(cx - y, cx + y, cy - x, color);

        y++;
        if (d <= 0) {
            d += 2 * y + 1;
        } else {
            x--;
            d += 2 * (y - x) + 1;
        }
    }
}

void FramebufferManager::drawTriangle(Point p1, Point p2, Point p3, const Color& color) {
    drawLine(p1.x, p1.y, p2.x, p2.y, color);
    drawLine(p2.x, p2.y, p3.x, p3.y, color);
    drawLine(p3.x, p3.y, p1.x, p1.y, color);
}

void FramebufferManager::fillTriangle(Point p1, Point p2, Point p3, const Color& color) {
    if (!isInitialized_) return;

    // ترتيب الرؤوس حسب Y / Sort vertices by Y
    if (p1.y > p2.y) { Point t = p1; p1 = p2; p2 = t; }
    if (p1.y > p3.y) { Point t = p1; p1 = p3; p3 = t; }
    if (p2.y > p3.y) { Point t = p2; p2 = p3; p3 = t; }

    auto interpolateX = [](Point a, Point b, int32_t y) -> int32_t {
        if (a.y == b.y) return a.x;
        return a.x + (b.x - a.x) * (y - a.y) / (b.y - a.y);
    };

    // النصف العلوي / Upper half
    for (int32_t y = p1.y; y <= p2.y; y++) {
        int32_t xa = interpolateX(p1, p2, y);
        int32_t xb = interpolateX(p1, p3, y);
        hLine(xa, xb, y, color);
    }

    // النصف السفلي / Lower half
    for (int32_t y = p2.y + 1; y <= p3.y; y++) {
        int32_t xa = interpolateX(p2, p3, y);
        int32_t xb = interpolateX(p1, p3, y);
        hLine(xa, xb, y, color);
    }
}

void FramebufferManager::drawArc(int32_t cx, int32_t cy, int32_t radius,
                                  float startAngle, float endAngle, const Color& color) {
    if (!isInitialized_ || radius <= 0) return;

    const float PI = 3.14159265358979323846f;
    float step = 1.0f / static_cast<float>(radius); // خطوة تتناسب مع نصف القطر

    for (float angle = startAngle; angle <= endAngle; angle += step) {
        int32_t x = cx + static_cast<int32_t>(radius * std::cos(angle * PI / 180.0f));
        int32_t y = cy + static_cast<int32_t>(radius * std::sin(angle * PI / 180.0f));
        putPixel(x, y, color);
    }
}

void FramebufferManager::drawRoundedRect(const Rect& rect, int32_t radius,
                                          const Color& color) {
    if (!isInitialized_) return;
    if (radius <= 0) { drawRect(rect, color); return; }

    int32_t x1 = rect.x;
    int32_t y1 = rect.y;
    int32_t x2 = rect.x + static_cast<int32_t>(rect.width) - 1;
    int32_t y2 = rect.y + static_cast<int32_t>(rect.height) - 1;

    // الأضلاع المستقيمة / Straight edges
    hLine(x1 + radius, x2 - radius, y1, color);           // أعلى / Top
    hLine(x1 + radius, x2 - radius, y2, color);           // أسفل / Bottom
    vLine(x1, y1 + radius, y2 - radius, color);           // يسار / Left
    vLine(x2, y1 + radius, y2 - radius, color);           // يمين / Right

    // الزوايا / Corners (using midpoint algorithm on quadrants)
    int32_t r = radius;
    int32_t px = r, py = 0;
    int32_t d = 1 - r;

    while (px >= py) {
        putPixel(x2 - radius + px, y1 + radius - py, color); // أعلى يمين / Top-right
        putPixel(x2 - radius + py, y1 + radius - px, color);
        putPixel(x1 + radius - px, y1 + radius - py, color); // أعلى يسار / Top-left
        putPixel(x1 + radius - py, y1 + radius - px, color);
        putPixel(x2 - radius + px, y2 - radius + py, color); // أسفل يمين / Bottom-right
        putPixel(x2 - radius + py, y2 - radius + px, color);
        putPixel(x1 + radius - px, y2 - radius + py, color); // أسفل يسار / Bottom-left
        putPixel(x1 + radius - py, y2 - radius + px, color);

        py++;
        if (d <= 0) { d += 2 * py + 1; }
        else { px--; d += 2 * (py - px) + 1; }
    }
}

void FramebufferManager::fillRoundedRect(const Rect& rect, int32_t radius,
                                          const Color& color) {
    if (!isInitialized_) return;
    if (radius <= 0) { fillRect(rect, color); return; }

    int32_t x1 = rect.x;
    int32_t y1 = rect.y;
    int32_t x2 = rect.x + static_cast<int32_t>(rect.width) - 1;
    int32_t y2 = rect.y + static_cast<int32_t>(rect.height) - 1;

    // الجزء المستقيم / Straight section
    for (int32_t y = y1 + radius; y <= y2 - radius; y++) {
        hLine(x1, x2, y, color);
    }

    // الزوايا الدائرية مع تعبئة / Rounded corners with fill
    int32_t r = radius;
    int32_t px = r, py = 0;
    int32_t d = 1 - r;

    while (px >= py) {
        hLine(x1 + radius - px, x2 - radius + px, y1 + radius - py, color);
        hLine(x1 + radius - py, x2 - radius + py, y1 + radius - px, color);
        hLine(x1 + radius - px, x2 - radius + px, y2 - radius + py, color);
        hLine(x1 + radius - py, x2 - radius + py, y2 - radius + px, color);

        py++;
        if (d <= 0) { d += 2 * py + 1; }
        else { px--; d += 2 * (py - px) + 1; }
    }
}

void FramebufferManager::fillGradientH(const Rect& rect,
                                         const Color& startColor, const Color& endColor) {
    if (!isInitialized_ || rect.width == 0) return;

    for (uint32_t x = 0; x < rect.width; x++) {
        float t = static_cast<float>(x) / static_cast<float>(rect.width - 1);
        Color c(
            static_cast<uint8_t>(startColor.red + t * (endColor.red - startColor.red)),
            static_cast<uint8_t>(startColor.green + t * (endColor.green - startColor.green)),
            static_cast<uint8_t>(startColor.blue + t * (endColor.blue - startColor.blue)),
            static_cast<uint8_t>(startColor.alpha + t * (endColor.alpha - startColor.alpha))
        );
        vLine(rect.x + static_cast<int32_t>(x), rect.y,
              rect.y + static_cast<int32_t>(rect.height) - 1, c);
    }
}

void FramebufferManager::fillGradientV(const Rect& rect,
                                         const Color& startColor, const Color& endColor) {
    if (!isInitialized_ || rect.height == 0) return;

    for (uint32_t y = 0; y < rect.height; y++) {
        float t = static_cast<float>(y) / static_cast<float>(rect.height - 1);
        Color c(
            static_cast<uint8_t>(startColor.red + t * (endColor.red - startColor.red)),
            static_cast<uint8_t>(startColor.green + t * (endColor.green - startColor.green)),
            static_cast<uint8_t>(startColor.blue + t * (endColor.blue - startColor.blue)),
            static_cast<uint8_t>(startColor.alpha + t * (endColor.alpha - startColor.alpha))
        );
        hLine(rect.x, rect.x + static_cast<int32_t>(rect.width) - 1,
              rect.y + static_cast<int32_t>(y), c);
    }
}

// ============================================================================
// عمليات النص / Text Operations
// ============================================================================

void FramebufferManager::setFont(const BitmapFont& font) {
    currentFont_ = font;
}

void FramebufferManager::drawChar(int32_t x, int32_t y, char c, const Color& fg,
                                   const Color& bg) {
    if (!isInitialized_ || !currentFont_.data) return;

    uint32_t charIndex = static_cast<uint8_t>(c) - currentFont_.firstChar;
    if (charIndex >= currentFont_.numGlyphs) return;

    const uint8_t* glyph = currentFont_.data + charIndex * currentFont_.height;

    for (uint8_t row = 0; row < currentFont_.height; row++) {
        uint8_t bits = glyph[row];
        for (uint8_t col = 0; col < currentFont_.width; col++) {
            if (bits & (0x80 >> col)) {
                putPixel(x + col, y + row, fg);
            } else if (bg.alpha > 0) {
                putPixel(x + col, y + row, bg);
            }
        }
    }
}

void FramebufferManager::drawString(int32_t x, int32_t y, const char* str,
                                     const Color& fg, const Color& bg) {
    if (!str) return;

    int32_t cx = x;
    while (*str) {
        if (*str == '\n') {
            cx = x;
            y += currentFont_.height;
        } else {
            drawChar(cx, y, *str, fg, bg);
            cx += currentFont_.width;
        }
        str++;
    }
}

void FramebufferManager::drawStringUTF8(int32_t x, int32_t y, const char* str,
                                          const Color& fg, const Color& bg) {
    if (!str) return;

    int32_t cx = x;
    while (*str) {
        uint8_t byte = static_cast<uint8_t>(*str);

        if (byte == '\n') {
            cx = x;
            y += currentFont_.height;
            str++;
            continue;
        }

        // حساب طول محرف UTF-8 / Calculate UTF-8 character length
        int charLen = 1;
        uint32_t codePoint = byte;

        if ((byte & 0x80) == 0) {
            // ASCII: بايت واحد / 1 byte
            codePoint = byte;
            charLen = 1;
        } else if ((byte & 0xE0) == 0xC0) {
            codePoint = byte & 0x1F;
            charLen = 2;
        } else if ((byte & 0xF0) == 0xE0) {
            codePoint = byte & 0x0F;
            charLen = 3;
        } else if ((byte & 0xF8) == 0xF0) {
            codePoint = byte & 0x07;
            charLen = 4;
        }

        for (int i = 1; i < charLen && str[i]; i++) {
            codePoint = (codePoint << 6) | (static_cast<uint8_t>(str[i]) & 0x3F);
        }

        // رسم المحرف (إذا ضمن نطاق ASCII) / Draw if ASCII range
        if (codePoint < 128) {
            drawChar(cx, y, static_cast<char>(codePoint), fg, bg);
        } else {
            // محارف غير ASCII: رسم مستطيل بديل / Non-ASCII: draw replacement box
            Rect box(cx, y, currentFont_.width, currentFont_.height);
            drawRect(box, fg);
        }

        cx += currentFont_.width;
        str += charLen;
    }
}

// ============================================================================
// عمليات المخزن / Buffer Operations
// ============================================================================

void FramebufferManager::clear(const Color& color) {
    if (!isInitialized_) return;

    uint32_t c = color.toUint32();
    uint32_t total = bufferSize_;

    // تعبئة سريعة / Fast fill
    uint32_t* ptr = backBuffer_;
    uint32_t count = total;

    // تعبئة بـ 8 بكسل / Fill 8 pixels at a time
    while (count >= 8) {
        ptr[0] = c; ptr[1] = c; ptr[2] = c; ptr[3] = c;
        ptr[4] = c; ptr[5] = c; ptr[6] = c; ptr[7] = c;
        ptr += 8; count -= 8;
    }
    while (count-- > 0) {
        *ptr++ = c;
    }
}

void FramebufferManager::swapBuffers() {
    if (!isInitialized_) return;

    if (vsyncEnabled_) {
        waitVSync();
    }

    if (doubleBuffered_ && frontBuffer_ && backBuffer_) {
        // نسخ المخزن الخلفي إلى الأمامي / Copy back buffer to front buffer
        // في نظام حقيقي: استخدام memcpy المحسّن أو DMA
        std::memcpy(const_cast<uint32_t*>(const_cast<volatile uint32_t*>(frontBuffer_)),
                    backBuffer_, bufferSize_ * sizeof(uint32_t));
    }
}

void FramebufferManager::setDoubleBuffering(bool enabled) {
    doubleBuffered_ = enabled;
}

void FramebufferManager::blit(const uint32_t* src, int32_t srcX, int32_t srcY,
                               uint32_t srcWidth, uint32_t srcHeight,
                               int32_t destX, int32_t destY) {
    if (!isInitialized_ || !src) return;

    for (uint32_t y = 0; y < srcHeight; y++) {
        for (uint32_t x = 0; x < srcWidth; x++) {
            int32_t dx = destX + static_cast<int32_t>(x);
            int32_t dy = destY + static_cast<int32_t>(y);
            int32_t sx = srcX + static_cast<int32_t>(x);
            int32_t sy = srcY + static_cast<int32_t>(y);

            uint32_t pixel = src[sy * srcWidth + sx];
            putPixel(dx, dy, Color::fromUint32(pixel));
        }
    }
}

void FramebufferManager::blitAlpha(const uint32_t* src, int32_t srcX, int32_t srcY,
                                    uint32_t srcWidth, uint32_t srcHeight,
                                    int32_t destX, int32_t destY) {
    if (!isInitialized_ || !src) return;

    for (uint32_t y = 0; y < srcHeight; y++) {
        for (uint32_t x = 0; x < srcWidth; x++) {
            int32_t dx = destX + static_cast<int32_t>(x);
            int32_t dy = destY + static_cast<int32_t>(y);
            int32_t sx = srcX + static_cast<int32_t>(x);
            int32_t sy = srcY + static_cast<int32_t>(y);

            uint32_t pixel = src[sy * srcWidth + sx];
            putPixelAlpha(dx, dy, Color::fromUint32(pixel));
        }
    }
}

void FramebufferManager::scroll(int32_t dx, int32_t dy, const Color& fillColor) {
    if (!isInitialized_) return;

    uint32_t w = currentMode_.width;
    uint32_t h = currentMode_.height;

    // إنشاء نسخة مؤقتة / Create temporary copy
    std::vector<uint32_t> temp(bufferSize_);
    std::memcpy(temp.data(), backBuffer_, bufferSize_ * sizeof(uint32_t));

    // مسح المخزن / Clear buffer
    clear(fillColor);

    // نسخ مع الإزاحة / Copy with offset
    for (uint32_t y = 0; y < h; y++) {
        int32_t srcY = static_cast<int32_t>(y) - dy;
        if (srcY < 0 || srcY >= static_cast<int32_t>(h)) continue;

        for (uint32_t x = 0; x < w; x++) {
            int32_t srcX = static_cast<int32_t>(x) - dx;
            if (srcX < 0 || srcX >= static_cast<int32_t>(w)) continue;

            backBuffer_[y * w + x] = temp[srcY * w + srcX];
        }
    }
}

// ============================================================================
// عمليات الصورة / Image Operations
// ============================================================================

void FramebufferManager::drawBitmap(int32_t x, int32_t y, const uint32_t* pixels,
                                     uint32_t width, uint32_t height) {
    if (!isInitialized_ || !pixels) return;

    for (uint32_t py = 0; py < height; py++) {
        for (uint32_t px = 0; px < width; px++) {
            putPixel(x + static_cast<int32_t>(px), y + static_cast<int32_t>(py),
                    Color::fromUint32(pixels[py * width + px]));
        }
    }
}

void FramebufferManager::drawBitmapAlpha(int32_t x, int32_t y, const uint32_t* pixels,
                                          uint32_t width, uint32_t height) {
    if (!isInitialized_ || !pixels) return;

    for (uint32_t py = 0; py < height; py++) {
        for (uint32_t px = 0; px < width; px++) {
            putPixelAlpha(x + static_cast<int32_t>(px), y + static_cast<int32_t>(py),
                         Color::fromUint32(pixels[py * width + px]));
        }
    }
}

void FramebufferManager::drawBitmapScaled(int32_t x, int32_t y, const uint32_t* pixels,
                                           uint32_t srcW, uint32_t srcH,
                                           uint32_t destW, uint32_t destH) {
    if (!isInitialized_ || !pixels || destW == 0 || destH == 0) return;

    // أقرب جار / Nearest neighbor scaling
    for (uint32_t dy = 0; dy < destH; dy++) {
        uint32_t srcY = dy * srcH / destH;
        for (uint32_t dx = 0; dx < destW; dx++) {
            uint32_t srcX = dx * srcW / destW;
            putPixel(x + static_cast<int32_t>(dx), y + static_cast<int32_t>(dy),
                    Color::fromUint32(pixels[srcY * srcW + srcX]));
        }
    }
}

// ============================================================================
// المؤشر / Cursor
// ============================================================================

void FramebufferManager::setCursorImage(const uint32_t* pixels, uint32_t width,
                                         uint32_t height, int32_t hotspotX,
                                         int32_t hotspotY) {
    cursorWidth_ = width;
    cursorHeight_ = height;
    cursorHotX_ = hotspotX;
    cursorHotY_ = hotspotY;
    cursorImage_.assign(pixels, pixels + (width * height));
    cursorBackground_.resize(width * height);
}

void FramebufferManager::moveCursor(int32_t x, int32_t y) {
    if (!isInitialized_) return;

    // استعادة الخلفية القديمة / Restore old background
    if (cursorVisible_ && !cursorBackground_.empty()) {
        for (uint32_t cy = 0; cy < cursorHeight_; cy++) {
            for (uint32_t cx = 0; cx < cursorWidth_; cx++) {
                int32_t px = cursorX_ - cursorHotX_ + static_cast<int32_t>(cx);
                int32_t py = cursorY_ - cursorHotY_ + static_cast<int32_t>(cy);
                if (inBounds(px, py)) {
                    *pixelAt(px, py) = cursorBackground_[cy * cursorWidth_ + cx];
                }
            }
        }
    }

    cursorX_ = x;
    cursorY_ = y;

    // حفظ الخلفية الجديدة ورسم المؤشر / Save new background and draw cursor
    if (cursorVisible_ && !cursorImage_.empty()) {
        for (uint32_t cy = 0; cy < cursorHeight_; cy++) {
            for (uint32_t cx = 0; cx < cursorWidth_; cx++) {
                int32_t px = cursorX_ - cursorHotX_ + static_cast<int32_t>(cx);
                int32_t py = cursorY_ - cursorHotY_ + static_cast<int32_t>(cy);
                if (inBounds(px, py)) {
                    cursorBackground_[cy * cursorWidth_ + cx] = *pixelAt(px, py);
                    Color cursorPixel = Color::fromUint32(cursorImage_[cy * cursorWidth_ + cx]);
                    if (cursorPixel.alpha > 0) {
                        putPixelAlpha(px, py, cursorPixel);
                    }
                }
            }
        }
    }
}

void FramebufferManager::showCursor(bool visible) {
    cursorVisible_ = visible;
    if (visible) {
        moveCursor(cursorX_, cursorY_);
    }
}

// ============================================================================
// القص / Clipping
// ============================================================================

void FramebufferManager::setClipRect(const Rect& rect) {
    clipRect_ = rect;
    clipEnabled_ = true;
}

void FramebufferManager::resetClipRect() {
    clipRect_ = Rect(0, 0, currentMode_.width, currentMode_.height);
    clipEnabled_ = false;
}

// ============================================================================
// VSync
// ============================================================================

void FramebufferManager::waitVSync() {
    // انتظار نهاية الـ retrace الحالي / Wait for current retrace to end
    while (IOPorts::inb(FramebufferConstants::VGA_INPUT_STATUS) & 0x08) {}
    // انتظار بداية الـ retrace التالي / Wait for next retrace to start
    while (!(IOPorts::inb(FramebufferConstants::VGA_INPUT_STATUS) & 0x08)) {}
}

void FramebufferManager::setVSync(bool enabled) {
    vsyncEnabled_ = enabled;
}

// ============================================================================
// التقرير / Report
// ============================================================================

std::string FramebufferManager::generateReport() const {
    std::ostringstream report;

    report << "\n" << std::string(70, '=') << "\n";
    report << "تقرير الرسومات / Graphics Report\n";
    report << std::string(70, '=') << "\n\n";

    report << "الحالة / Status: " << (isInitialized_ ? "مهيأ / Initialized" : "غير مهيأ / Not initialized") << "\n\n";

    if (isInitialized_) {
        report << "--- وضع الفيديو / Video Mode ---\n";
        report << "الدقة / Resolution: " << currentMode_.width << "x" << currentMode_.height << "\n";
        report << "عمق اللون / Color Depth: " << static_cast<int>(currentMode_.bitsPerPixel) << " bpp\n";
        report << "Pitch: " << currentMode_.pitch << " bytes/line\n";
        report << "عنوان المخزن / FB Address: 0x" << std::hex << currentMode_.framebufferAddr << std::dec << "\n";
        report << "حجم المخزن / FB Size: " << currentMode_.framebufferSize << " bytes\n";
        report << "تخزين مزدوج / Double Buffer: " << (doubleBuffered_ ? "نعم / Yes" : "لا / No") << "\n";
        report << "VSync: " << (vsyncEnabled_ ? "مفعل / Enabled" : "معطل / Disabled") << "\n\n";
    }

    if (gpuInfo_.type != GPUType::UNKNOWN) {
        report << "--- معلومات GPU / GPU Info ---\n";
        report << "النوع / Type: ";
        switch (gpuInfo_.type) {
            case GPUType::NVIDIA:   report << "NVIDIA"; break;
            case GPUType::AMD:      report << "AMD"; break;
            case GPUType::INTEL:    report << "Intel"; break;
            case GPUType::VIRTUAL:  report << "افتراضي / Virtual"; break;
            case GPUType::SOFTWARE: report << "برمجي / Software"; break;
            default:                report << "غير معروف / Unknown"; break;
        }
        report << "\n";
        report << "المصنع / Vendor: " << gpuInfo_.vendorName << "\n";
        report << "الجهاز / Device: " << gpuInfo_.deviceName << "\n";
        report << "ذاكرة VRAM: " << (gpuInfo_.vramSize / (1024 * 1024)) << " MB\n";
        report << "MMIO: 0x" << std::hex << gpuInfo_.mmioBase << " (" << std::dec << (gpuInfo_.mmioSize / 1024) << " KB)\n\n";

        report << "--- القدرات / Capabilities ---\n";
        report << "2D: " << (gpuInfo_.capabilities.supports2D ? "✓" : "✗") << "\n";
        report << "3D: " << (gpuInfo_.capabilities.supports3D ? "✓" : "✗") << "\n";
        report << "مؤشر عتاد / HW Cursor: " << (gpuInfo_.capabilities.supportsHardwareCursor ? "✓" : "✗") << "\n";
        report << "DMA: " << (gpuInfo_.capabilities.supportsDMA ? "✓" : "✗") << "\n";
    }

    report << std::string(70, '=') << "\n\n";
    return report.str();
}

} // namespace LowLevel
} // namespace Sad
