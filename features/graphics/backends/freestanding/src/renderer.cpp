/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: renderer.cpp
 * المسار: features/graphics/backends/freestanding/src/renderer.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @brief (AR) تطبيق مُصيّر الوضع المستقل — رسم مباشر على framebuffer
 * @brief (EN) Freestanding renderer implementation — direct framebuffer drawing
 *
 * يُطبق جميع دوال الرسم الأساسية (~20 دالة) من PlatformRenderer
 * باستخدام كتابات ذاكرة مباشرة فقط — بدون SDL2 أو أي مكتبة خارجية.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/freestanding/renderer.h"

#include <cstring>
#include <cmath>
#include <algorithm>

namespace sad
{
    namespace ui
    {
        namespace freestanding
        {

            // ═══════════════════════════════════════════════════════════════════════════════
            //                         البناء والهدم
            // ═══════════════════════════════════════════════════════════════════════════════

            FreestandingRenderer::FreestandingRenderer()
            {
                initDefaultMemOps();
            }

            FreestandingRenderer::~FreestandingRenderer()
            {
                shutdown();
            }

            // ═══════════════════════════════════════════════════════════════════════════════
            //                         عمليات الذاكرة الافتراضية
            // ═══════════════════════════════════════════════════════════════════════════════

            void FreestandingRenderer::initDefaultMemOps()
            {
                memOps_.fill32 = [](uint32_t *dst, uint32_t value, size_t count)
                {
                    for (size_t i = 0; i < count; ++i)
                        dst[i] = value;
                };
                memOps_.copy32 = [](uint32_t *dst, const uint32_t *src, size_t count)
                {
                    std::memcpy(dst, src, count * sizeof(uint32_t));
                };
                memOps_.zero32 = [](uint32_t *dst, size_t count)
                {
                    std::memset(dst, 0, count * sizeof(uint32_t));
                };
            }

            void FreestandingRenderer::setMemoryOps(const MemoryOps &ops)
            {
                if (ops.fill32)
                    memOps_.fill32 = ops.fill32;
                if (ops.copy32)
                    memOps_.copy32 = ops.copy32;
                if (ops.zero32)
                    memOps_.zero32 = ops.zero32;
            }

            // ═══════════════════════════════════════════════════════════════════════════════
            //                         التهيئة
            // ═══════════════════════════════════════════════════════════════════════════════

            bool FreestandingRenderer::initializeFramebuffer(
                const FramebufferConfig &config,
                const FreestandingRenderOptions &options)
            {
                if (!config.address || config.width == 0 || config.height == 0)
                    return false;

                fb_ = config;
                fbOptions_ = options;

                // تعيين pitch تلقائياً إذا لم يُحدد
                if (fb_.pitch == 0)
                    fb_.pitch = fb_.width * (fb_.bpp / 8);

                // هدف الرسم: المخزن الخلفي (إذا وُجد) أو الشاشة مباشرة
                drawTarget_ = fb_.backBuffer ? fb_.backBuffer : fb_.address;

                // أبعاد viewport للقص
                viewportWidth_ = static_cast<int>(fb_.width);
                viewportHeight_ = static_cast<int>(fb_.height);

                initialized_ = true;
                return true;
            }

            bool FreestandingRenderer::initialize(void *nativeSurface,
                                                  const PlatformRenderOptions &options)
            {
                // nativeSurface هو مؤشر لـ FramebufferConfig
                if (!nativeSurface)
                    return false;

                auto *fbConfig = static_cast<FramebufferConfig *>(nativeSurface);
                FreestandingRenderOptions fbOpts;
                fbOpts.debugBounds = options.debugBounds;
                fbOpts.scaleFactor = options.scaleFactor;

                return initializeFramebuffer(*fbConfig, fbOpts);
            }

            void FreestandingRenderer::shutdown()
            {
                initialized_ = false;
                drawTarget_ = nullptr;
                currentFont_.reset();
            }

            bool FreestandingRenderer::isReady() const
            {
                return initialized_ && drawTarget_ != nullptr;
            }

            // ═══════════════════════════════════════════════════════════════════════════════
            //                         حلقة الرسم
            // ═══════════════════════════════════════════════════════════════════════════════

            void FreestandingRenderer::beginFrame(const Color &clearColor)
            {
                if (!isReady())
                    return;

                uint32_t color = colorToARGB(clearColor);
                uint32_t totalPixels = fb_.width * fb_.height;

                // مسح الشاشة باللون المحدد
                memOps_.fill32(drawTarget_, color, totalPixels);

                // إلغاء القص
                clipRect_.active = false;
            }

            void FreestandingRenderer::endFrame()
            {
                if (!isReady())
                    return;

                // نسخ المخزن الخلفي إلى الشاشة إذا كان التخزين المزدوج مفعّلاً
                if (fb_.backBuffer && fb_.backBuffer != fb_.address)
                {
                    swapBuffers();
                }
            }

            void FreestandingRenderer::swapBuffers()
            {
                if (!fb_.address || !fb_.backBuffer)
                    return;

                uint32_t totalPixels = fb_.width * fb_.height;
                memOps_.copy32(fb_.address, fb_.backBuffer, totalPixels);
            }

            void FreestandingRenderer::swapRegion(int x, int y, int w, int h)
            {
                if (!fb_.address || !fb_.backBuffer)
                    return;

                // حدود آمنة
                int x0 = std::max(0, x);
                int y0 = std::max(0, y);
                int x1 = std::min(static_cast<int>(fb_.width), x + w);
                int y1 = std::min(static_cast<int>(fb_.height), y + h);

                uint32_t pitchPixels = fb_.pitch / sizeof(uint32_t);

                for (int row = y0; row < y1; ++row)
                {
                    uint32_t *dst = fb_.address + row * pitchPixels + x0;
                    const uint32_t *src = fb_.backBuffer + row * pitchPixels + x0;
                    memOps_.copy32(dst, src, static_cast<size_t>(x1 - x0));
                }
            }

            // ═══════════════════════════════════════════════════════════════════════════════
            //                         دوال البكسل الأساسية
            // ═══════════════════════════════════════════════════════════════════════════════

            inline uint32_t FreestandingRenderer::colorToARGB(const Color &c) const
            {
                return (static_cast<uint32_t>(c.a) << 24) |
                       (static_cast<uint32_t>(c.r) << 16) |
                       (static_cast<uint32_t>(c.g) << 8) |
                       static_cast<uint32_t>(c.b);
            }

            inline uint32_t FreestandingRenderer::alphaBlend(uint32_t bg, uint32_t fg) const
            {
                uint32_t fgA = (fg >> 24) & 0xFF;
                if (fgA == 0xFF)
                    return fg;
                if (fgA == 0x00)
                    return bg;

                uint32_t invA = 255 - fgA;

                uint32_t fgR = (fg >> 16) & 0xFF;
                uint32_t fgG = (fg >> 8) & 0xFF;
                uint32_t fgB = fg & 0xFF;

                uint32_t bgR = (bg >> 16) & 0xFF;
                uint32_t bgG = (bg >> 8) & 0xFF;
                uint32_t bgB = bg & 0xFF;

                uint32_t rR = (fgR * fgA + bgR * invA) / 255;
                uint32_t rG = (fgG * fgA + bgG * invA) / 255;
                uint32_t rB = (fgB * fgA + bgB * invA) / 255;

                return 0xFF000000 | (rR << 16) | (rG << 8) | rB;
            }

            inline void FreestandingRenderer::putPixel(int x, int y, uint32_t color)
            {
                // فحص الحدود
                if (x < 0 || y < 0 ||
                    x >= static_cast<int>(fb_.width) ||
                    y >= static_cast<int>(fb_.height))
                    return;

                // فحص القص
                if (clipRect_.active)
                {
                    if (x < clipRect_.x || y < clipRect_.y ||
                        x >= clipRect_.x + clipRect_.w ||
                        y >= clipRect_.y + clipRect_.h)
                        return;
                }

                uint32_t pitchPixels = fb_.pitch / sizeof(uint32_t);
                uint32_t idx = y * pitchPixels + x;

                // خلط ألفا إذا مفعّل واللون شبه شفاف
                if (fbOptions_.alphaBlending && ((color >> 24) & 0xFF) < 0xFF)
                {
                    drawTarget_[idx] = alphaBlend(drawTarget_[idx], color);
                }
                else
                {
                    drawTarget_[idx] = color;
                }
            }

            inline void FreestandingRenderer::putPixelFast(int x, int y, uint32_t color)
            {
                uint32_t pitchPixels = fb_.pitch / sizeof(uint32_t);
                drawTarget_[y * pitchPixels + x] = color;
            }

            void FreestandingRenderer::fillHLine(int x1, int x2, int y, uint32_t color)
            {
                if (y < 0 || y >= static_cast<int>(fb_.height))
                    return;

                // ترتيب
                if (x1 > x2)
                    std::swap(x1, x2);

                // حدود الشاشة
                x1 = std::max(0, x1);
                x2 = std::min(static_cast<int>(fb_.width) - 1, x2);

                // حدود القص
                if (clipRect_.active)
                {
                    x1 = std::max(x1, clipRect_.x);
                    x2 = std::min(x2, clipRect_.x + clipRect_.w - 1);
                    if (y < clipRect_.y || y >= clipRect_.y + clipRect_.h)
                        return;
                }

                if (x1 > x2)
                    return;

                uint32_t pitchPixels = fb_.pitch / sizeof(uint32_t);
                uint32_t *row = drawTarget_ + y * pitchPixels;

                // إذا اللون معتم → ملء مباشر (أسرع)
                if (((color >> 24) & 0xFF) == 0xFF || !fbOptions_.alphaBlending)
                {
                    memOps_.fill32(row + x1, color, static_cast<size_t>(x2 - x1 + 1));
                }
                else
                {
                    // خلط ألفا بكسل بكسل
                    for (int x = x1; x <= x2; ++x)
                    {
                        row[x] = alphaBlend(row[x], color);
                    }
                }
            }

            // ═══════════════════════════════════════════════════════════════════════════════
            //                         رسم المستطيلات
            // ═══════════════════════════════════════════════════════════════════════════════

            void FreestandingRenderer::drawFilledRect(float x, float y, float w, float h,
                                                      const Color &color)
            {
                if (!isReady() || w <= 0 || h <= 0)
                    return;
                auto c = applyOpacity(color);

                int ix = static_cast<int>(x);
                int iy = static_cast<int>(y);
                int iw = static_cast<int>(w);
                int ih = static_cast<int>(h);

                uint32_t argb = colorToARGB(c);

                for (int row = iy; row < iy + ih; ++row)
                {
                    fillHLine(ix, ix + iw - 1, row, argb);
                }
            }

            void FreestandingRenderer::drawRoundedRect(float x, float y, float w, float h,
                                                       const Color &color, float radius)
            {
                if (!isReady() || w <= 0 || h <= 0)
                    return;
                auto c = applyOpacity(color);

                int ix = static_cast<int>(x);
                int iy = static_cast<int>(y);
                int iw = static_cast<int>(w);
                int ih = static_cast<int>(h);
                int ir = std::min(static_cast<int>(radius), std::min(iw / 2, ih / 2));

                if (ir <= 0)
                {
                    drawFilledRect(x, y, w, h, c);
                    return;
                }

                uint32_t argb = colorToARGB(c);

                for (int row = 0; row < ih; ++row)
                {
                    int lineY = iy + row;
                    int x1 = ix;
                    int x2 = ix + iw - 1;

                    // حساب القص الدائري للزوايا
                    if (row < ir)
                    {
                        // الزاوية العلوية
                        int dy = ir - row - 1;
                        int dx = ir - static_cast<int>(std::sqrt(
                                          static_cast<float>(ir * ir - dy * dy)));
                        x1 += dx;
                        x2 -= dx;
                    }
                    else if (row >= ih - ir)
                    {
                        // الزاوية السفلية
                        int dy = row - (ih - ir);
                        int dx = ir - static_cast<int>(std::sqrt(
                                          static_cast<float>(ir * ir - dy * dy)));
                        x1 += dx;
                        x2 -= dx;
                    }

                    fillHLine(x1, x2, lineY, argb);
                }
            }

            void FreestandingRenderer::drawRectOutline(float x, float y, float w, float h,
                                                       const Color &color, float thickness)
            {
                if (!isReady() || w <= 0 || h <= 0)
                    return;

                int t = std::max(1, static_cast<int>(thickness));

                // الحد العلوي
                drawFilledRect(x, y, w, static_cast<float>(t), color);
                // الحد السفلي
                drawFilledRect(x, y + h - t, w, static_cast<float>(t), color);
                // الحد الأيسر
                drawFilledRect(x, y + t, static_cast<float>(t), h - 2.0f * t, color);
                // الحد الأيمن
                drawFilledRect(x + w - t, y + t, static_cast<float>(t), h - 2.0f * t, color);
            }

            // ═══════════════════════════════════════════════════════════════════════════════
            //                         رسم الخطوط (Bresenham)
            // ═══════════════════════════════════════════════════════════════════════════════

            void FreestandingRenderer::drawLine(float x1, float y1, float x2, float y2,
                                                const Color &color, float thickness)
            {
                if (!isReady())
                    return;
                auto c = applyOpacity(color);

                uint32_t argb = colorToARGB(c);
                int ix1 = static_cast<int>(x1), iy1 = static_cast<int>(y1);
                int ix2 = static_cast<int>(x2), iy2 = static_cast<int>(y2);

                // خط أفقي — محسّن
                if (iy1 == iy2)
                {
                    int t = std::max(1, static_cast<int>(thickness));
                    for (int i = 0; i < t; ++i)
                    {
                        fillHLine(std::min(ix1, ix2), std::max(ix1, ix2), iy1 + i, argb);
                    }
                    return;
                }

                // خط رأسي — محسّن
                if (ix1 == ix2)
                {
                    int t = std::max(1, static_cast<int>(thickness));
                    int yStart = std::min(iy1, iy2);
                    int yEnd = std::max(iy1, iy2);
                    for (int y = yStart; y <= yEnd; ++y)
                    {
                        for (int i = 0; i < t; ++i)
                        {
                            putPixel(ix1 + i, y, argb);
                        }
                    }
                    return;
                }

                // Bresenham — خط مائل
                int dx = std::abs(ix2 - ix1);
                int dy = -std::abs(iy2 - iy1);
                int sx = ix1 < ix2 ? 1 : -1;
                int sy = iy1 < iy2 ? 1 : -1;
                int err = dx + dy;

                int cx = ix1, cy = iy1;
                while (true)
                {
                    putPixel(cx, cy, argb);
                    // سماكة > 1
                    int t = static_cast<int>(thickness);
                    for (int i = 1; i < t; ++i)
                    {
                        putPixel(cx + i, cy, argb);
                        putPixel(cx, cy + i, argb);
                    }

                    if (cx == ix2 && cy == iy2)
                        break;
                    int e2 = 2 * err;
                    if (e2 >= dy)
                    {
                        err += dy;
                        cx += sx;
                    }
                    if (e2 <= dx)
                    {
                        err += dx;
                        cy += sy;
                    }
                }
            }

            // ═══════════════════════════════════════════════════════════════════════════════
            //                         رسم الدوائر (Midpoint)
            // ═══════════════════════════════════════════════════════════════════════════════

            void FreestandingRenderer::drawCircle(float cx, float cy, float radius,
                                                  const Color &color)
            {
                if (!isReady() || radius <= 0)
                    return;
                auto c = applyOpacity(color);

                int icx = static_cast<int>(cx);
                int icy = static_cast<int>(cy);
                int ir = static_cast<int>(radius);
                uint32_t argb = colorToARGB(c);

                // ملء دائرة باستخدام خطوط أفقية
                for (int y = -ir; y <= ir; ++y)
                {
                    int dx = static_cast<int>(std::sqrt(
                        static_cast<float>(ir * ir - y * y)));
                    fillHLine(icx - dx, icx + dx, icy + y, argb);
                }
            }

            void FreestandingRenderer::drawCircleOutline(float cx, float cy, float radius,
                                                         const Color &color,
                                                         float thickness)
            {
                if (!isReady() || radius <= 0)
                    return;
                auto c = applyOpacity(color);

                int icx = static_cast<int>(cx);
                int icy = static_cast<int>(cy);
                int ir = static_cast<int>(radius);
                uint32_t argb = colorToARGB(c);
                int t = std::max(1, static_cast<int>(thickness));

                // خوارزمية Midpoint Circle
                int x = ir, y = 0;
                int err = 1 - ir;

                while (x >= y)
                {
                    // رسم 8 أقواس متناظرة
                    for (int i = 0; i < t; ++i)
                    {
                        putPixel(icx + x + i, icy + y, argb);
                        putPixel(icx + y, icy + x + i, argb);
                        putPixel(icx - y, icy + x + i, argb);
                        putPixel(icx - x - i, icy + y, argb);
                        putPixel(icx - x - i, icy - y, argb);
                        putPixel(icx - y, icy - x - i, argb);
                        putPixel(icx + y, icy - x - i, argb);
                        putPixel(icx + x + i, icy - y, argb);
                    }

                    ++y;
                    if (err < 0)
                    {
                        err += 2 * y + 1;
                    }
                    else
                    {
                        --x;
                        err += 2 * (y - x) + 1;
                    }
                }
            }

            // ═══════════════════════════════════════════════════════════════════════════════
            //                         التدرجات
            // ═══════════════════════════════════════════════════════════════════════════════

            void FreestandingRenderer::drawLinearGradient(
                float x, float y, float w, float h,
                const Color &startColor, const Color &endColor,
                bool vertical, float radius)
            {
                if (!isReady() || w <= 0 || h <= 0)
                    return;
                auto sc = applyOpacity(startColor);
                auto ec = applyOpacity(endColor);

                int ix = static_cast<int>(x);
                int iy = static_cast<int>(y);
                int iw = static_cast<int>(w);
                int ih = static_cast<int>(h);

                float steps = vertical ? static_cast<float>(ih) : static_cast<float>(iw);
                if (steps < 1)
                    steps = 1;

                for (int row = 0; row < ih; ++row)
                {
                    for (int col = 0; col < iw; ++col)
                    {
                        float t = vertical
                                      ? static_cast<float>(row) / steps
                                      : static_cast<float>(col) / steps;

                        t = std::max(0.0f, std::min(1.0f, t));

                        uint8_t r = static_cast<uint8_t>(sc.r + (ec.r - sc.r) * t);
                        uint8_t g = static_cast<uint8_t>(sc.g + (ec.g - sc.g) * t);
                        uint8_t b = static_cast<uint8_t>(sc.b + (ec.b - sc.b) * t);
                        uint8_t a = static_cast<uint8_t>(sc.a + (ec.a - sc.a) * t);

                        uint32_t argb = (static_cast<uint32_t>(a) << 24) |
                                        (static_cast<uint32_t>(r) << 16) |
                                        (static_cast<uint32_t>(g) << 8) |
                                        static_cast<uint32_t>(b);

                        putPixel(ix + col, iy + row, argb);
                    }
                }
            }

            // ═══════════════════════════════════════════════════════════════════════════════
            //                         القص (Clipping)
            // ═══════════════════════════════════════════════════════════════════════════════

            void FreestandingRenderer::setClipRect(float x, float y, float w, float h)
            {
                clipRect_.x = static_cast<int>(x);
                clipRect_.y = static_cast<int>(y);
                clipRect_.w = static_cast<int>(w);
                clipRect_.h = static_cast<int>(h);
                clipRect_.active = true;
            }

            void FreestandingRenderer::clearClipRect()
            {
                clipRect_.active = false;
            }

            // ═══════════════════════════════════════════════════════════════════════════════
            //                         النصوص والخطوط
            // ═══════════════════════════════════════════════════════════════════════════════

            bool FreestandingRenderer::loadFont(const std::string & /*fontPath*/,
                                                float /*fontSize*/)
            {
                // في الوضع المستقل، نستخدم الخط النقطي المدمج فقط
                if (!currentFont_)
                {
                    loadBuiltinFont();
                }
                return currentFont_ != nullptr;
            }

            bool FreestandingRenderer::loadBitmapFont(const BitmapFont &font)
            {
                currentFont_ = std::make_unique<BitmapFont>(font);
                return true;
            }

            uint32_t FreestandingRenderer::decodeUTF8(const char *&ptr, const char *end)
            {
                if (ptr >= end)
                    return 0;

                unsigned char c = static_cast<unsigned char>(*ptr);

                if (c < 0x80)
                {
                    ++ptr;
                    return c;
                }
                if ((c & 0xE0) == 0xC0)
                {
                    if (ptr + 1 >= end)
                    {
                        ++ptr;
                        return 0xFFFD;
                    }
                    uint32_t cp = (c & 0x1F) << 6;
                    cp |= (static_cast<unsigned char>(ptr[1]) & 0x3F);
                    ptr += 2;
                    return cp;
                }
                if ((c & 0xF0) == 0xE0)
                {
                    if (ptr + 2 >= end)
                    {
                        ptr = end;
                        return 0xFFFD;
                    }
                    uint32_t cp = (c & 0x0F) << 12;
                    cp |= (static_cast<unsigned char>(ptr[1]) & 0x3F) << 6;
                    cp |= (static_cast<unsigned char>(ptr[2]) & 0x3F);
                    ptr += 3;
                    return cp;
                }
                if ((c & 0xF8) == 0xF0)
                {
                    if (ptr + 3 >= end)
                    {
                        ptr = end;
                        return 0xFFFD;
                    }
                    uint32_t cp = (c & 0x07) << 18;
                    cp |= (static_cast<unsigned char>(ptr[1]) & 0x3F) << 12;
                    cp |= (static_cast<unsigned char>(ptr[2]) & 0x3F) << 6;
                    cp |= (static_cast<unsigned char>(ptr[3]) & 0x3F);
                    ptr += 4;
                    return cp;
                }

                ++ptr;
                return 0xFFFD;
            }

            int FreestandingRenderer::drawBitmapChar(uint32_t codepoint, int x, int y,
                                                     uint32_t color, float scale)
            {
                if (!currentFont_)
                    return 0;

                const BitmapGlyph *glyph = currentFont_->findGlyph(codepoint);
                if (!glyph || !glyph->bitmap)
                {
                    // حرف غير موجود — ارسم مربع استبدال
                    int cw = static_cast<int>(currentFont_->charWidth * scale);
                    int ch = static_cast<int>(currentFont_->charHeight * scale);
                    Color replColor;
                    replColor.r = (color >> 16) & 0xFF;
                    replColor.g = (color >> 8) & 0xFF;
                    replColor.b = color & 0xFF;
                    replColor.a = 128;
                    drawRectOutline(static_cast<float>(x), static_cast<float>(y),
                                    static_cast<float>(cw), static_cast<float>(ch),
                                    replColor, 1.0f);
                    return cw;
                }

                int gw = glyph->width;
                int gh = glyph->height;
                int sx = static_cast<int>(scale);
                if (sx < 1)
                    sx = 1;

                for (int gy = 0; gy < gh; ++gy)
                {
                    for (int gx = 0; gx < gw; ++gx)
                    {
                        // بت واحد لكل بكسل (packed row)
                        int byteIdx = gy * ((gw + 7) / 8) + gx / 8;
                        int bitIdx = 7 - (gx % 8);
                        bool on = (glyph->bitmap[byteIdx] >> bitIdx) & 1;

                        if (on)
                        {
                            int px = x + (gx + glyph->xOffset) * sx;
                            int py = y + (gy + glyph->yOffset) * sx;

                            for (int dy = 0; dy < sx; ++dy)
                                for (int dx = 0; dx < sx; ++dx)
                                    putPixel(px + dx, py + dy, color);
                        }
                    }
                }

                return static_cast<int>(glyph->advance * scale);
            }

            std::pair<float, float> FreestandingRenderer::drawText(
                const std::string &text, float x, float y,
                const Color &color, float fontSize)
            {
                if (!isReady() || text.empty())
                    return {0, 0};

                if (!currentFont_)
                    loadBuiltinFont();
                if (!currentFont_)
                    return {0, 0};

                float scale = fontSize / static_cast<float>(currentFont_->charHeight);
                if (scale < 0.5f)
                    scale = 0.5f;

                auto c = applyOpacity(color);
                uint32_t argb = colorToARGB(c);
                float curX = x;
                float totalWidth = 0;
                float lineHeight = currentFont_->lineHeight * scale;

                const char *ptr = text.c_str();
                const char *end = ptr + text.size();

                while (ptr < end)
                {
                    uint32_t cp = decodeUTF8(ptr, end);
                    if (cp == 0)
                        break;

                    if (cp == '\n')
                    {
                        y += lineHeight;
                        curX = x;
                        continue;
                    }

                    int advance = drawBitmapChar(cp, static_cast<int>(curX),
                                                 static_cast<int>(y), argb, scale);
                    curX += advance;
                    totalWidth = std::max(totalWidth, curX - x);
                }

                return {totalWidth, lineHeight};
            }

            std::pair<float, float> FreestandingRenderer::measureText(
                const std::string &text, float fontSize)
            {
                if (text.empty())
                    return {0, 0};

                if (!currentFont_)
                    loadBuiltinFont();
                if (!currentFont_)
                    return {0, 0};

                float scale = fontSize / static_cast<float>(currentFont_->charHeight);
                if (scale < 0.5f)
                    scale = 0.5f;

                float width = 0;
                float maxWidth = 0;
                int lines = 1;
                float lineHeight = currentFont_->lineHeight * scale;

                const char *ptr = text.c_str();
                const char *end = ptr + text.size();

                while (ptr < end)
                {
                    uint32_t cp = decodeUTF8(ptr, end);
                    if (cp == 0)
                        break;

                    if (cp == '\n')
                    {
                        maxWidth = std::max(maxWidth, width);
                        width = 0;
                        ++lines;
                        continue;
                    }

                    const BitmapGlyph *glyph = currentFont_ ? currentFont_->findGlyph(cp) : nullptr;
                    float advance = glyph ? glyph->advance * scale
                                          : currentFont_->charWidth * scale;
                    width += advance;
                }

                maxWidth = std::max(maxWidth, width);
                return {maxWidth, lines * lineHeight};
            }

            // ═══════════════════════════════════════════════════════════════════════════════
            //                         رسم الصور (stub في الوضع المستقل)
            // ═══════════════════════════════════════════════════════════════════════════════

            void FreestandingRenderer::drawImage(const std::string & /*path*/,
                                                 float x, float y, float w, float h)
            {
                // في الوضع المستقل، لا يمكن تحميل صور من ملفات بدون نظام ملفات
                // نرسم مستطيل رمادي كبديل
                Color placeholder;
                placeholder.r = 128;
                placeholder.g = 128;
                placeholder.b = 128;
                placeholder.a = 200;
                drawFilledRect(x, y, w, h, placeholder);

                // رسم علامة × عليه
                Color cross;
                cross.r = 200;
                cross.g = 200;
                cross.b = 200;
                cross.a = 255;
                drawLine(x, y, x + w, y + h, cross, 1.0f);
                drawLine(x + w, y, x, y + h, cross, 1.0f);
            }

            // ═══════════════════════════════════════════════════════════════════════════════
            //                         BitmapFont::findGlyph
            // ═══════════════════════════════════════════════════════════════════════════════

            const BitmapGlyph *BitmapFont::findGlyph(uint32_t codepoint) const
            {
                // بحث خطي (بسيط — يمكن تحسينه بـ hash map لاحقاً)
                for (const auto &g : glyphs)
                {
                    if (g.codepoint == codepoint)
                        return &g;
                }
                return nullptr;
            }

            // ═══════════════════════════════════════════════════════════════════════════════
            // FreestandingWindow — تنفيذ
            // ═══════════════════════════════════════════════════════════════════════════════

            FreestandingWindow::FreestandingWindow()
                : renderer_(std::make_unique<FreestandingRenderer>()),
                  layoutEngine_(std::make_unique<LayoutEngine>())
            {
            }

            FreestandingWindow::~FreestandingWindow()
            {
                destroy();
            }

            bool FreestandingWindow::initializeFramebuffer(
                const FramebufferConfig &config,
                const FreestandingRenderOptions &options)
            {
                return renderer_->initializeFramebuffer(config, options);
            }

            bool FreestandingWindow::create(const PlatformWindowOptions &options)
            {
                options_ = options;
                width_ = options.width;
                height_ = options.height;

                layoutEngine_->setDirection(options.direction);
                layoutEngine_->setViewportSize(
                    static_cast<float>(width_),
                    static_cast<float>(height_));

                isOpen_ = true;
                needsRedraw_ = true;
                return true;
            }

            void FreestandingWindow::destroy()
            {
                renderer_->shutdown();
                isOpen_ = false;
            }

            bool FreestandingWindow::isOpen() const { return isOpen_; }

            void FreestandingWindow::setContent(std::shared_ptr<IRNode> root)
            {
                contentRoot_ = std::move(root);
                // ربط محرك الانيميشن بالمُصيّر وتهيئة الحركات
                renderer_->setAnimationEngine(&animationEngine_);
                renderer_->resetAnimationState();
                if (contentRoot_)
                {
                    renderer_->initializeAnimations(contentRoot_, getCurrentTimeMs());
                }
                needsRedraw_ = true;
            }

            void FreestandingWindow::setStateStore(std::shared_ptr<StateStore> store)
            {
                stateStore_ = std::move(store);
            }

            void FreestandingWindow::applyPatches(size_t patchedNodes, bool needsRelayout)
            {
                // (AR) تحديث جزئي — الشجرة contentRoot_ تم تعديلها موضعياً بالفعل
                //      نحتاج فقط:
                //      1. إعادة تهيئة التحريكات للعقد الجديدة فقط (إذا تغيّر التخطيط)
                //      2. طلب إعادة رسم (ينسخ المخزن الخلفي → الشاشة في endFrame)
                // (EN) Partial update — contentRoot_ already patched in-place.
                //      Only reinit animations for new nodes if layout changed, and request redraw.

                if (!contentRoot_)
                    return;

                // (AR) تهيئة تحريكات العقد الجديدة فقط
                //      initializeAnimations تفحص التحريكات الموجودة ولا تُعيد تهيئة المُشغّلة
                if (needsRelayout && renderer_)
                {
                    renderer_->initializeAnimations(contentRoot_, getCurrentTimeMs());
                }

                needsRedraw_ = true;
            }

            void FreestandingWindow::run()
            {
                // في الوضع المستقل، حلقة الأحداث تُدار من النواة
                while (isOpen_)
                {
                    runOneFrame();
                }
            }

            bool FreestandingWindow::runOneFrame()
            {
                if (!isOpen_)
                    return false;

                // تحديث الانيميشن — إذا كانت حركات نشطة، نطلب إعادة رسم
                bool animating = animationEngine_.update(getCurrentTimeMs());
                if (animating)
                    needsRedraw_ = true;

                if (needsRedraw_)
                {
                    updateLayout();
                    renderFrame();
                    needsRedraw_ = false;
                }
                return isOpen_;
            }

            void FreestandingWindow::invalidate() { needsRedraw_ = true; }
            int FreestandingWindow::getWidth() const { return width_; }
            int FreestandingWindow::getHeight() const { return height_; }
            PlatformRenderer *FreestandingWindow::getRenderer() { return renderer_.get(); }

            void FreestandingWindow::updateLayout()
            {
                if (contentRoot_)
                {
                    LayoutConstraints constraints;
                    constraints.maxWidth = static_cast<float>(width_);
                    constraints.maxHeight = static_cast<float>(height_);
                    layoutResult_ = layoutEngine_->layout(contentRoot_, constraints);
                }
            }

            void FreestandingWindow::renderFrame()
            {
                Color bg = {0.1f, 0.1f, 0.15f, 1.0f};
                renderer_->beginFrame(bg);
                if (contentRoot_ && layoutResult_)
                {
                    renderer_->render(contentRoot_, layoutResult_);
                }
                renderer_->endFrame();
            }

        } // namespace freestanding
    } // namespace ui
} // namespace sad
