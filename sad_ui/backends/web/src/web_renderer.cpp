/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: web_renderer.cpp
 * المسار: sad_ui/backends/web/src/web_renderer.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ مُصيّر الويب — يرسم مباشرة عبر HTML5 Canvas 2D API.
 *
 * يستخدم Emscripten EM_ASM / EM_JS لاستدعاء JavaScript Canvas API:
 *   ctx.fillRect(), ctx.fillText(), ctx.drawImage(), ctx.arc(), etc.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/web/web_renderer.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include <cmath>
#include <iostream>
#include <sstream>

namespace sad
{
    namespace ui
    {
        namespace web
        {

            // ═══════════════════════════════════════════════════════════════════════════════
            // WebRenderer — تنفيذ
            // ═══════════════════════════════════════════════════════════════════════════════

            WebRenderer::WebRenderer()
                : defaultFontFamily_("Cairo, Arial, sans-serif")
            {
            }

            WebRenderer::~WebRenderer()
            {
                shutdown();
            }

            bool WebRenderer::initialize(void * /*nativeSurface*/,
                                         const PlatformRenderOptions &options)
            {
                options_ = options;

#ifdef __EMSCRIPTEN__
                // تهيئة Canvas في المتصفح
                EM_ASM({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) {
            canvas = document.createElement('canvas');
            canvas.id = UTF8ToString($0);
            canvas.width = $1;
            canvas.height = $2;
            canvas.style.direction = 'rtl';
            document.body.appendChild(canvas);
        }
        Module._sadCanvas = canvas;
        Module._sadCtx = canvas.getContext('2d'); }, webOpts_.canvasId.c_str(), canvasWidth_, canvasHeight_);

                initialized_ = true;
                return true;
#else
                // خارج Emscripten — وضع المحاكاة (للاختبار على سطح المكتب)
                initialized_ = true;
                return true;
#endif
            }

            void WebRenderer::shutdown()
            {
                initialized_ = false;
            }

            bool WebRenderer::isReady() const
            {
                return initialized_;
            }

            bool WebRenderer::loadFont(const std::string &fontPath, float /*fontSize*/)
            {
#ifdef __EMSCRIPTEN__
                // تحميل الخط عبر CSS @font-face
                EM_ASM({
        var style = document.createElement('style');
        style.textContent = '@font-face { font-family: "SadFont"; src: url("' +
                            UTF8ToString($0) + '"); }';
        document.head.appendChild(style); }, fontPath.c_str());
#endif
                return true;
            }

            // ── حلقة الرسم ───────────────────────────────────

            void WebRenderer::beginFrame(const Color &clearColor)
            {
                // كاش النصوص يبقى صالحاً عبر الإطارات (نفس الخط = نفس القياس)
#ifdef __EMSCRIPTEN__
                EM_ASM({
        var ctx = Module._sadCtx;
        var canvas = Module._sadCanvas;
        ctx.clearRect(0, 0, canvas.width, canvas.height);
        // لون الخلفية
        ctx.fillStyle = 'rgba(' + Math.floor($0*255) + ',' +
                        Math.floor($1*255) + ',' +
                        Math.floor($2*255) + ',' + $3 + ')';
        ctx.fillRect(0, 0, canvas.width, canvas.height);
        // إعداد الاتجاه العربي
        ctx.direction = 'rtl';
        ctx.textBaseline = 'top'; }, clearColor.r, clearColor.g, clearColor.b, clearColor.a);
#endif
            }

            void WebRenderer::endFrame()
            {
                // Canvas 2D لا يحتاج swap — يُعرض مباشرة
            }

            // ════════════════════════════════════════════════════════════════════════════════
            // دعم الدوران — Canvas 2D save/translate/rotate/translate/restore
            // ════════════════════════════════════════════════════════════════════════════════

            void WebRenderer::pushRotation(float centerX, float centerY, float angleDegrees)
            {
#ifdef __EMSCRIPTEN__
                float angleRad = angleDegrees * 3.14159265358979f / 180.0f;
                EM_ASM({
        var ctx = Module._sadCtx;
        ctx.save();
        ctx.translate($0, $1);
        ctx.rotate($2);
        ctx.translate(-$0, -$1); }, centerX, centerY, angleRad);
#else
                (void)centerX;
                (void)centerY;
                (void)angleDegrees;
#endif
            }

            void WebRenderer::popRotation()
            {
#ifdef __EMSCRIPTEN__
                EM_ASM({
        var ctx = Module._sadCtx;
        ctx.restore(); });
#endif
            }

            void WebRenderer::pushScale(float centerX, float centerY, float scaleX, float scaleY)
            {
#ifdef __EMSCRIPTEN__
                EM_ASM({
        var ctx = Module._sadCtx;
        ctx.save();
        ctx.translate($0, $1);
        ctx.scale($2, $3);
        ctx.translate(-$0, -$1); }, centerX, centerY, scaleX, scaleY);
#else
                (void)centerX;
                (void)centerY;
                (void)scaleX;
                (void)scaleY;
#endif
            }

            void WebRenderer::popScale()
            {
#ifdef __EMSCRIPTEN__
                EM_ASM({
        var ctx = Module._sadCtx;
        ctx.restore(); });
#endif
            }

            // ════════════════════════════════════════════════════════════════════════════════
            // دوال الرسم الأساسية
            // ════════════════════════════════════════════════════════════════════════════════

            void WebRenderer::drawFilledRect(float x, float y, float w, float h,
                                             const Color &color)
            {
                auto c = applyOpacity(color);
#ifdef __EMSCRIPTEN__
                EM_ASM({
        var ctx = Module._sadCtx;
        ctx.fillStyle = 'rgba(' + Math.floor($4*255) + ',' +
                        Math.floor($5*255) + ',' +
                        Math.floor($6*255) + ',' + $7 + ')';
        ctx.fillRect($0, $1, $2, $3); }, x, y, w, h, c.r, c.g, c.b, c.a);
#else
                (void)x;
                (void)y;
                (void)w;
                (void)h;
                (void)color;
#endif
            }

            void WebRenderer::drawRoundedRect(float x, float y, float w, float h,
                                              const Color &color, float radius)
            {
                auto c = applyOpacity(color);
#ifdef __EMSCRIPTEN__
                EM_ASM({
        var ctx = Module._sadCtx;
        var r = Math.min($4, Math.min($2, $3) / 2);
        ctx.fillStyle = 'rgba(' + Math.floor($5*255) + ',' +
                        Math.floor($6*255) + ',' +
                        Math.floor($7*255) + ',' + $8 + ')';
        ctx.beginPath();
        ctx.moveTo($0 + r, $1);
        ctx.lineTo($0 + $2 - r, $1);
        ctx.quadraticCurveTo($0 + $2, $1, $0 + $2, $1 + r);
        ctx.lineTo($0 + $2, $1 + $3 - r);
        ctx.quadraticCurveTo($0 + $2, $1 + $3, $0 + $2 - r, $1 + $3);
        ctx.lineTo($0 + r, $1 + $3);
        ctx.quadraticCurveTo($0, $1 + $3, $0, $1 + $3 - r);
        ctx.lineTo($0, $1 + r);
        ctx.quadraticCurveTo($0, $1, $0 + r, $1);
        ctx.closePath();
        ctx.fill(); }, x, y, w, h, radius, c.r, c.g, c.b, c.a);
#else
                (void)x;
                (void)y;
                (void)w;
                (void)h;
                (void)color;
                (void)radius;
#endif
            }

            void WebRenderer::drawRectOutline(float x, float y, float w, float h,
                                              const Color &color, float thickness)
            {
                auto c = applyOpacity(color);
#ifdef __EMSCRIPTEN__
                EM_ASM({
        var ctx = Module._sadCtx;
        ctx.strokeStyle = 'rgba(' + Math.floor($4*255) + ',' +
                          Math.floor($5*255) + ',' +
                          Math.floor($6*255) + ',' + $7 + ')';
        ctx.lineWidth = $8;
        ctx.strokeRect($0, $1, $2, $3); }, x, y, w, h, c.r, c.g, c.b, c.a, thickness);
#else
                (void)x;
                (void)y;
                (void)w;
                (void)h;
                (void)color;
                (void)thickness;
#endif
            }

            std::pair<float, float> WebRenderer::drawText(const std::string &text,
                                                          float x, float y,
                                                          const Color &color,
                                                          float fontSize)
            {
                auto c = applyOpacity(color);
#ifdef __EMSCRIPTEN__
                float measuredWidth = EM_ASM_DOUBLE({
        var ctx = Module._sadCtx;
        ctx.font = $4 + 'px ' + UTF8ToString($5);
        ctx.fillStyle = 'rgba(' + Math.floor($1*255) + ',' +
                        Math.floor($2*255) + ',' +
                        Math.floor($3*255) + ',' + $6 + ')';
        ctx.textBaseline = 'top';
        var txt = UTF8ToString($0);
        ctx.fillText(txt, $7, $8);
        return ctx.measureText(txt).width; }, text.c_str(), c.r, c.g, c.b, fontSize, defaultFontFamily_.c_str(), c.a, x, y);

                return {static_cast<float>(measuredWidth), fontSize * 1.2f};
#else
                (void)text;
                (void)x;
                (void)y;
                (void)color;
                (void)fontSize;
                return {fontSize * text.size() * 0.6f, fontSize * 1.2f};
#endif
            }

            std::pair<float, float> WebRenderer::measureText(const std::string &text,
                                                             float fontSize)
            {
                // ─── كاش قياس النص — تجنب استدعاء JS المكلف ───
                TextMeasureKey key{text, fontSize};
                auto it = textMeasureCache_.find(key);
                if (it != textMeasureCache_.end())
                {
                    return it->second;
                }

#ifdef __EMSCRIPTEN__
                float measuredWidth = EM_ASM_DOUBLE({
        var ctx = Module._sadCtx;
        ctx.font = $1 + 'px ' + UTF8ToString($2);
        return ctx.measureText(UTF8ToString($0)).width; }, text.c_str(), fontSize, defaultFontFamily_.c_str());

                auto result = std::make_pair(static_cast<float>(measuredWidth), fontSize * 1.2f);
#else
                (void)text;
                (void)fontSize;
                auto result = std::make_pair(fontSize * text.size() * 0.6f, fontSize * 1.2f);
#endif

                // إضافة للكاش (مع حد أقصى)
                if (textMeasureCache_.size() < MAX_TEXT_CACHE)
                {
                    textMeasureCache_[key] = result;
                }
                return result;
            }

            void WebRenderer::drawImage(const std::string &path,
                                        float x, float y, float w, float h)
            {
#ifdef __EMSCRIPTEN__
                // ─── كاش الصور — تجنب إنشاء Image() جديد كل إطار ───
                bool alreadyLoaded = loadedImages_.count(path) > 0;
                if (!alreadyLoaded)
                {
                    loadedImages_[path] = true;
                }
                EM_ASM({
        var ctx = Module._sadCtx;
        var key = UTF8ToString($0);
        if (!Module._sadImages) Module._sadImages = {};
        var img = Module._sadImages[key];
        if (img && img.complete) {
            ctx.drawImage(img, $1, $2, $3, $4);
        } else if (!img) {
            img = new Image();
            img.onload = function() {
                ctx.drawImage(img, $1, $2, $3, $4);
            };
            img.src = key;
            Module._sadImages[key] = img;
        } }, path.c_str(), x, y, w, h);
#else
                (void)path;
                (void)x;
                (void)y;
                (void)w;
                (void)h;
#endif
            }

            void WebRenderer::drawCircle(float cx, float cy, float radius,
                                         const Color &color)
            {
                auto c = applyOpacity(color);
#ifdef __EMSCRIPTEN__
                EM_ASM({
        var ctx = Module._sadCtx;
        ctx.fillStyle = 'rgba(' + Math.floor($3*255) + ',' +
                        Math.floor($4*255) + ',' +
                        Math.floor($5*255) + ',' + $6 + ')';
        ctx.beginPath();
        ctx.arc($0, $1, $2, 0, 2 * Math.PI);
        ctx.fill(); }, cx, cy, radius, c.r, c.g, c.b, c.a);
#else
                (void)cx;
                (void)cy;
                (void)radius;
                (void)color;
#endif
            }

            void WebRenderer::drawCircleOutline(float cx, float cy, float radius,
                                                const Color &color, float thickness)
            {
                auto c = applyOpacity(color);
#ifdef __EMSCRIPTEN__
                EM_ASM({
        var ctx = Module._sadCtx;
        ctx.strokeStyle = 'rgba(' + Math.floor($3*255) + ',' +
                          Math.floor($4*255) + ',' +
                          Math.floor($5*255) + ',' + $6 + ')';
        ctx.lineWidth = $7;
        ctx.beginPath();
        ctx.arc($0, $1, $2, 0, 2 * Math.PI);
        ctx.stroke(); }, cx, cy, radius, c.r, c.g, c.b, c.a, thickness);
#else
                (void)cx;
                (void)cy;
                (void)radius;
                (void)color;
                (void)thickness;
#endif
            }

            void WebRenderer::drawLine(float x1, float y1, float x2, float y2,
                                       const Color &color, float thickness)
            {
                auto c = applyOpacity(color);
#ifdef __EMSCRIPTEN__
                EM_ASM({
        var ctx = Module._sadCtx;
        ctx.strokeStyle = 'rgba(' + Math.floor($4*255) + ',' +
                          Math.floor($5*255) + ',' +
                          Math.floor($6*255) + ',' + $7 + ')';
        ctx.lineWidth = $8;
        ctx.beginPath();
        ctx.moveTo($0, $1);
        ctx.lineTo($2, $3);
        ctx.stroke(); }, x1, y1, x2, y2, c.r, c.g, c.b, c.a, thickness);
#else
                (void)x1;
                (void)y1;
                (void)x2;
                (void)y2;
                (void)color;
                (void)thickness;
#endif
            }

            void WebRenderer::drawLinearGradient(float x, float y, float w, float h,
                                                 const Color &startColor, const Color &endColor,
                                                 bool vertical, float radius)
            {
                auto sc = applyOpacity(startColor);
                auto ec = applyOpacity(endColor);
#ifdef __EMSCRIPTEN__
                EM_ASM({
        var ctx = Module._sadCtx;
        var grd;
        if ($8) { // vertical
            grd = ctx.createLinearGradient($0, $1, $0, $1 + $3);
        } else {
            grd = ctx.createLinearGradient($0, $1, $0 + $2, $1);
        }
        grd.addColorStop(0, 'rgba(' + Math.floor($4*255) + ',' +
                         Math.floor($5*255) + ',' +
                         Math.floor($6*255) + ',' + $7 + ')');
        grd.addColorStop(1, 'rgba(' + Math.floor($10*255) + ',' +
                         Math.floor($11*255) + ',' +
                         Math.floor($12*255) + ',' + $13 + ')');
        ctx.fillStyle = grd;
        if ($9 > 0) {
            // rounded rect
            var r = $9;
            ctx.beginPath();
            ctx.moveTo($0 + r, $1);
            ctx.lineTo($0 + $2 - r, $1);
            ctx.quadraticCurveTo($0 + $2, $1, $0 + $2, $1 + r);
            ctx.lineTo($0 + $2, $1 + $3 - r);
            ctx.quadraticCurveTo($0 + $2, $1 + $3, $0 + $2 - r, $1 + $3);
            ctx.lineTo($0 + r, $1 + $3);
            ctx.quadraticCurveTo($0, $1 + $3, $0, $1 + $3 - r);
            ctx.lineTo($0, $1 + r);
            ctx.quadraticCurveTo($0, $1, $0 + r, $1);
            ctx.closePath();
            ctx.fill();
        } else {
            ctx.fillRect($0, $1, $2, $3);
        } }, x, y, w, h, sc.r, sc.g, sc.b, sc.a, vertical ? 1.0 : 0.0, radius, ec.r, ec.g, ec.b, ec.a);
#else
                (void)x;
                (void)y;
                (void)w;
                (void)h;
                (void)startColor;
                (void)endColor;
                (void)vertical;
                (void)radius;
#endif
            }

            void WebRenderer::setClipRect(float x, float y, float w, float h)
            {
#ifdef __EMSCRIPTEN__
                EM_ASM({
        var ctx = Module._sadCtx;
        ctx.save();
        ctx.beginPath();
        ctx.rect($0, $1, $2, $3);
        ctx.clip(); }, x, y, w, h);
#else
                (void)x;
                (void)y;
                (void)w;
                (void)h;
#endif
            }

            void WebRenderer::clearClipRect()
            {
#ifdef __EMSCRIPTEN__
                EM_ASM({
                    var ctx = Module._sadCtx;
                    ctx.restore();
                });
#endif
            }

            // ═══════════════════════════════════════════════════════════════════════════════
            // WebWindow — تنفيذ
            // ═══════════════════════════════════════════════════════════════════════════════

            WebWindow::WebWindow()
                : renderer_(std::make_unique<WebRenderer>()), layoutEngine_(std::make_unique<LayoutEngine>())
            {
            }

            WebWindow::~WebWindow()
            {
                destroy();
            }

            bool WebWindow::create(const PlatformWindowOptions &options)
            {
                options_ = options;
                width_ = options.width;
                height_ = options.height;

                layoutEngine_->setDirection(options.direction);
                layoutEngine_->setViewportSize(
                    static_cast<float>(width_),
                    static_cast<float>(height_));

                if (!renderer_->initialize(nullptr))
                {
                    return false;
                }

                isOpen_ = true;
                needsRedraw_ = true;

                // ─── تهيئة معالج أحداث اللمس الموحد ───
                touchProcessor_.setViewportSize(width_, height_);
                touchProcessor_.setHitTestCallback(
                    [this](float x, float y) -> const IRNode *
                    { return hitTest(x, y); });
                touchProcessor_.setFireEventCallback(
                    [this](IREventType type, const std::string &expr,
                           const IRNode *node, const EventData &data)
                    { fireEvent(type, expr, node, data); });
                touchProcessor_.setGetTimeMsCallback(
                    [this]() -> uint32_t
                    { return getCurrentTimeMs(); });

#ifdef __EMSCRIPTEN__
                // تسجيل أحداث Canvas — نقر الفأرة
                emscripten_set_click_callback("#sad-canvas", this, true,
                                              [](int /*eventType*/, const EmscriptenMouseEvent *e, void *userData) -> EM_BOOL
                                              {
                                                  auto *self = static_cast<WebWindow *>(userData);
                                                  PlatformEvent evt;
                                                  evt.type = PlatformEventType::Tap;
                                                  evt.x = static_cast<float>(e->targetX);
                                                  evt.y = static_cast<float>(e->targetY);
                                                  if (self->eventHandler_)
                                                      self->eventHandler_(evt);
                                                  self->needsRedraw_ = true;
                                                  return true;
                                              });

                emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, true,
                                               [](int /*eventType*/, const EmscriptenUiEvent *e, void *userData) -> EM_BOOL
                                               {
                                                   auto *self = static_cast<WebWindow *>(userData);
                                                   self->width_ = e->windowInnerWidth;
                                                   self->height_ = e->windowInnerHeight;
                                                   self->layoutEngine_->setViewportSize(
                                                       static_cast<float>(self->width_),
                                                       static_cast<float>(self->height_));
                                                   // (AR) تحديث حجم معالج اللمس عند تغيير حجم النافذة
                                                   self->touchProcessor_.setViewportSize(
                                                       self->width_, self->height_);
                                                   self->needsRedraw_ = true;
                                                   return true;
                                               });

                // ─── تسجيل أحداث اللمس الأصلية (Touch API) ───
                emscripten_set_touchstart_callback("#sad-canvas", this, true,
                                                   [](int /*eventType*/, const EmscriptenTouchEvent *e, void *userData) -> EM_BOOL
                                                   {
                                                       auto *self = static_cast<WebWindow *>(userData);
                                                       for (int i = 0; i < e->numTouches; ++i)
                                                       {
                                                           if (e->touches[i].isChanged)
                                                           {
                                                               self->touchProcessor_.onFingerDown(
                                                                   0, // touchId — الويب لا يوفر device ID
                                                                   static_cast<int64_t>(e->touches[i].identifier),
                                                                   static_cast<float>(e->touches[i].targetX),
                                                                   static_cast<float>(e->touches[i].targetY),
                                                                   1.0f // pressure — غير متاح عبر Touch API القياسي
                                                               );
                                                           }
                                                       }
                                                       return true;
                                                   });

                emscripten_set_touchend_callback("#sad-canvas", this, true,
                                                 [](int /*eventType*/, const EmscriptenTouchEvent *e, void *userData) -> EM_BOOL
                                                 {
                                                     auto *self = static_cast<WebWindow *>(userData);
                                                     for (int i = 0; i < e->numTouches; ++i)
                                                     {
                                                         if (e->touches[i].isChanged)
                                                         {
                                                             self->touchProcessor_.onFingerUp(
                                                                 0,
                                                                 static_cast<int64_t>(e->touches[i].identifier),
                                                                 static_cast<float>(e->touches[i].targetX),
                                                                 static_cast<float>(e->touches[i].targetY));
                                                         }
                                                     }
                                                     return true;
                                                 });

                emscripten_set_touchcancel_callback("#sad-canvas", this, true,
                                                    [](int /*eventType*/, const EmscriptenTouchEvent *e, void *userData) -> EM_BOOL
                                                    {
                                                        auto *self = static_cast<WebWindow *>(userData);
                                                        for (int i = 0; i < e->numTouches; ++i)
                                                        {
                                                            if (e->touches[i].isChanged)
                                                            {
                                                                self->touchProcessor_.onFingerUp(
                                                                    0,
                                                                    static_cast<int64_t>(e->touches[i].identifier),
                                                                    static_cast<float>(e->touches[i].targetX),
                                                                    static_cast<float>(e->touches[i].targetY));
                                                            }
                                                        }
                                                        return true;
                                                    });

                emscripten_set_touchmove_callback("#sad-canvas", this, true,
                                                  [](int /*eventType*/, const EmscriptenTouchEvent *e, void *userData) -> EM_BOOL
                                                  {
                                                      auto *self = static_cast<WebWindow *>(userData);
                                                      for (int i = 0; i < e->numTouches; ++i)
                                                      {
                                                          if (e->touches[i].isChanged)
                                                          {
                                                              // (AR) الويب لا يوفر dx/dy مباشرة — TouchEventProcessor
                                                              //      يحسبها من الفرق بين المواقع الحالية والسابقة
                                                              self->touchProcessor_.onFingerMotion(
                                                                  0,
                                                                  static_cast<int64_t>(e->touches[i].identifier),
                                                                  static_cast<float>(e->touches[i].targetX),
                                                                  static_cast<float>(e->touches[i].targetY),
                                                                  0.0f, 0.0f, // dx, dy — سيُحسب في المعالج
                                                                  1.0f);
                                                          }
                                                      }
                                                      return true;
                                                  });

                // ─── تسجيل أحداث لوحة المفاتيح (Emscripten Keyboard API) ───
                emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, this, true,
                                                [](int /*eventType*/, const EmscriptenKeyboardEvent *e, void *userData) -> EM_BOOL
                                                {
                                                    auto *self = static_cast<WebWindow *>(userData);
                                                    // (AR) تحويل DOM keyCode إلى UnifiedKeyCode
                                                    //      e->which يُطابق USB HID Usage codes لمعظم المفاتيح
                                                    UnifiedKeyCode uKey = static_cast<UnifiedKeyCode>(e->which);
                                                    KeyModifiers mods;
                                                    mods.shift = e->shiftKey;
                                                    mods.ctrl = e->ctrlKey;
                                                    mods.alt = e->altKey;
                                                    mods.meta = e->metaKey;
                                                    self->kbProcessor_.onKeyDown(uKey, e->key, mods);
                                                    return true;
                                                });

                emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, this, true,
                                              [](int /*eventType*/, const EmscriptenKeyboardEvent *e, void *userData) -> EM_BOOL
                                              {
                                                  auto *self = static_cast<WebWindow *>(userData);
                                                  UnifiedKeyCode uKey = static_cast<UnifiedKeyCode>(e->which);
                                                  KeyModifiers mods;
                                                  mods.shift = e->shiftKey;
                                                  mods.ctrl = e->ctrlKey;
                                                  mods.alt = e->altKey;
                                                  mods.meta = e->metaKey;
                                                  self->kbProcessor_.onKeyUp(uKey, e->key, mods);
                                                  return true;
                                              });
#endif

                // ─── تهيئة معالج أحداث لوحة المفاتيح الموحد ───
                kbProcessor_.setFireEventCallback(
                    [this](IREventType type, const std::string &expr,
                           const IRNode *node, const EventData &data)
                    { fireEvent(type, expr, node, data); });
                kbProcessor_.setGetFocusedNodeCallback(
                    [this]() -> const IRNode *
                    { return focusedNode_; });
                kbProcessor_.setGetContentRootCallback(
                    [this]() -> const IRNode *
                    { return contentRoot_.get(); });
                kbProcessor_.setCollectFocusableNodesCallback(
                    [this](std::vector<const IRNode *> &nodes)
                    { collectFocusableNodes(contentRoot_.get(), nodes); });
                kbProcessor_.setSetFocusedNodeCallback(
                    [this](const IRNode *node)
                    { focusedNode_ = node; });
                kbProcessor_.setInvalidateCallback(
                    [this]()
                    { invalidate(); });
                kbProcessor_.setCloseWindowCallback(
                    [this]()
                    { isOpen_ = false; });

                // ─── تهيئة معالج أحداث الفأرة/المؤشر الموحد (Emscripten mouse events) ───
                mouseProcessor_.setHitTestCallback(
                    [this](float x, float y) -> const IRNode *
                    { return hitTest(x, y); });
                mouseProcessor_.setFireEventCallback(
                    [this](IREventType type, const std::string &expr,
                           const IRNode *node, const EventData &data)
                    { fireEvent(type, expr, node, data); });
                mouseProcessor_.setGetTimeMsCallback(
                    [this]() -> uint32_t
                    { return getCurrentTimeMs(); });
                mouseProcessor_.setInvalidateCallback(
                    [this]()
                    { invalidate(); });
                mouseProcessor_.setGetFocusedNodeCallback(
                    [this]() -> const IRNode *
                    { return focusedNode_; });
                mouseProcessor_.setSetFocusedNodeCallback(
                    [this](const IRNode *node)
                    { focusedNode_ = node; });
                mouseProcessor_.setFindNodeRectCallback(
                    [this](const IRNode *node) -> LayoutRect
                    { return findNodeRect(node); });
                mouseProcessor_.setFindScrollableAtCallback(
                    [this](float x, float y) -> ScrollableHitInfo
                    {
                        auto hit = findScrollableAt(x, y);
                        ScrollableHitInfo info;
                        if (hit.node && hit.layout)
                        {
                            info.node = hit.node;
                            info.viewHeight = hit.layout->rect.height;
                            info.contentHeight = hit.layout->contentHeight;
                            info.currentScrollOffset = renderer_->getScrollOffset(hit.node);
                        }
                        return info;
                    });
                mouseProcessor_.setSetScrollOffsetCallback(
                    [this](const IRNode *node, float offset)
                    { renderer_->setScrollOffset(node, offset); });
                mouseProcessor_.setGetContentRootCallback(
                    [this]() -> const IRNode *
                    { return contentRoot_.get(); });
                mouseProcessor_.setSetCursorCallback(
                    [](int cursorType)
                    {
#ifdef __EMSCRIPTEN__
                        // (AR) تغيير شكل المؤشر عبر JavaScript
                        if (cursorType == 1)
                        {
                            EM_ASM({ document.body.style.cursor = 'pointer'; });
                        }
                        else
                        {
                            EM_ASM({ document.body.style.cursor = 'default'; });
                        }
#else
                        (void)cursorType;
#endif
                    });

                return true;
            }

            void WebWindow::destroy()
            {
                renderer_->shutdown();
                isOpen_ = false;
            }

            bool WebWindow::isOpen() const { return isOpen_; }

            void WebWindow::setContent(std::shared_ptr<IRNode> root)
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

            void WebWindow::setStateStore(std::shared_ptr<StateStore> store)
            {
                stateStore_ = std::move(store);
            }

            void WebWindow::applyPatches(size_t patchedNodes, bool needsRelayout)
            {
                // (AR) تحديث جزئي — الشجرة contentRoot_ تم تعديلها موضعياً بالفعل
                //      نحتاج فقط:
                //      1. إعادة تهيئة التحريكات للعقد الجديدة فقط (إذا تغيّر التخطيط)
                //      2. طلب إعادة رسم
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

            void WebWindow::run()
            {
#ifdef __EMSCRIPTEN__
                // على WASM نستخدم emscripten_set_main_loop
                emscripten_set_main_loop_arg(
                    [](void *arg)
                    {
                        auto *self = static_cast<WebWindow *>(arg);
                        self->runOneFrame();
                    },
                    this, 0, true);
#else
                while (isOpen_)
                {
                    runOneFrame();
                }
#endif
            }

            bool WebWindow::runOneFrame()
            {
                if (!isOpen_)
                    return false;

                processEvents();

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

            void WebWindow::invalidate()
            {
                needsRedraw_ = true;
            }

            int WebWindow::getWidth() const { return width_; }
            int WebWindow::getHeight() const { return height_; }
            PlatformRenderer *WebWindow::getRenderer() { return renderer_.get(); }

            void WebWindow::processEvents()
            {
                // الأحداث تُعالج عبر callbacks المُسجّلة في create()
                // (touch callbacks تُمرر مباشرة إلى touchProcessor_)
            }

            void WebWindow::onNativeTouchEvent(int action, int64_t touchId, int64_t fingerId,
                                               float x, float y, float pressure,
                                               float dx, float dy)
            {
                // (AR) واجهة بديلة — في الويب، الأحداث تُسجّل مباشرة عبر
                //      emscripten_set_touchstart/end/move_callback في create().
                //      هذه الدالة توفر واجهة موحدة مع باقي المنصات
                //      في حال استُدعيت يدوياً من JavaScript عبر ccall.
                switch (action)
                {
                case 0: // touchstart
                    touchProcessor_.onFingerDown(touchId, fingerId, x, y, pressure);
                    break;
                case 1: // touchend / touchcancel
                    touchProcessor_.onFingerUp(touchId, fingerId, x, y);
                    break;
                case 2: // touchmove
                    touchProcessor_.onFingerMotion(touchId, fingerId, x, y, dx, dy, pressure);
                    break;
                }
                needsRedraw_ = true;
            }

            void WebWindow::onNativeKeyEvent(int action, int keyCode,
                                             const std::string &keyName,
                                             bool shift, bool ctrl, bool alt)
            {
                // (AR) واجهة بديلة — في الويب، الأحداث تُسجّل مباشرة عبر
                //      emscripten_set_keydown/keyup_callback في create().
                //      هذه الدالة توفر واجهة موحدة لاستدعاء من JavaScript عبر ccall.
                UnifiedKeyCode uKey = static_cast<UnifiedKeyCode>(keyCode);
                KeyModifiers mods;
                mods.shift = shift;
                mods.ctrl = ctrl;
                mods.alt = alt;

                if (action == 0)
                    kbProcessor_.onKeyDown(uKey, keyName, mods);
                else if (action == 1)
                    kbProcessor_.onKeyUp(uKey, keyName, mods);

                needsRedraw_ = true;
            }

            void WebWindow::onNativeTextInput(const std::string &text)
            {
                // (AR) تمرير إدخال نص من JavaScript (compositionend / input) إلى معالج موحد
                kbProcessor_.onTextInput(text);
                needsRedraw_ = true;
            }

            // ─── أحداث الفأرة (MouseEvent عبر JavaScript / Emscripten) ───

            void WebWindow::onNativeMouseEvent(int action, int button, float x, float y)
            {
                // (AR) تحويل حدث زر الفأرة من JavaScript إلى معالج موحد
                //      action: 0=mousedown, 1=mouseup
                //      button: 0=Left, 1=Middle, 2=Right (matches JS MouseEvent.button)
                MouseButton btn = MouseButton::Unknown;
                switch (button)
                {
                case 0:
                    btn = MouseButton::Left;
                    break;
                case 1:
                    btn = MouseButton::Middle;
                    break;
                case 2:
                    btn = MouseButton::Right;
                    break;
                }

                if (action == 0)
                    mouseProcessor_.onMouseDown(btn, x, y);
                else if (action == 1)
                    mouseProcessor_.onMouseUp(btn, x, y);

                needsRedraw_ = true;
            }

            void WebWindow::onNativeMouseMove(float x, float y,
                                              bool leftPressed,
                                              bool middlePressed,
                                              bool rightPressed)
            {
                // (AR) تمرير حركة الفأرة إلى معالج موحد
                MouseButtonState buttons;
                buttons.leftPressed = leftPressed;
                buttons.middlePressed = middlePressed;
                buttons.rightPressed = rightPressed;

                mouseProcessor_.onMouseMove(x, y, buttons);
                needsRedraw_ = true;
            }

            void WebWindow::onNativeMouseWheel(float deltaX, float deltaY,
                                               float mouseX, float mouseY,
                                               bool ctrlPressed)
            {
                // (AR) تمرير حدث عجلة التمرير إلى معالج موحد
                mouseProcessor_.onMouseWheel(deltaX, deltaY, mouseX, mouseY, ctrlPressed);
                needsRedraw_ = true;
            }

            void WebWindow::updateLayout()
            {
                if (contentRoot_)
                {
                    LayoutConstraints constraints;
                    constraints.maxWidth = static_cast<float>(width_);
                    constraints.maxHeight = static_cast<float>(height_);
                    layoutResult_ = layoutEngine_->layout(contentRoot_, constraints);
                }
            }

            void WebWindow::renderFrame()
            {
                Color bg = isDarkMode()
                               ? Color::fromNamed(NamedColor::Surface)
                               : Color{0.98f, 0.98f, 0.98f, 1.0f};

                renderer_->beginFrame(bg);
                if (contentRoot_ && layoutResult_)
                {
                    renderer_->render(contentRoot_, layoutResult_);
                }
                renderer_->endFrame();
            }

        } // namespace web
    } // namespace ui
} // namespace sad
