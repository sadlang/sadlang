/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: web_renderer.h
 * المسار: features/graphics/backends/web/include/sad_ui/web/web_renderer.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * مُصيّر الويب — يرسم مباشرة عبر HTML5 Canvas 2D API (Emscripten)
 *
 * بدلاً من توليد HTML كنص، يرسم مباشرة على Canvas باستخدام:
 *   - EM_ASM / EM_JS للوصول إلى JavaScript Canvas API
 *   - ctx.fillRect(), ctx.fillText(), ctx.drawImage()
 *   - ctx.beginPath(), ctx.arc(), ctx.lineTo()
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_WEB_RENDERER_H
#define SAD_UI_WEB_RENDERER_H

#include "sad_ui/platform_renderer.h"
#include "sad_ui/touch_processor.h"
#include "sad_ui/keyboard_processor.h"
#include "sad_ui/mouse_processor.h"
#include <string>
#include <unordered_map>

namespace sad
{
    namespace ui
    {
        namespace web
        {

            // ═══════════════════════════════════════════════════════════════════════════════

            struct WebRendererOptions : public PlatformRenderOptions
            {
                std::string canvasId = "sad-canvas"; ///< معرّف عنصر Canvas في HTML
                std::string fontFamily = "Cairo, Arial, sans-serif";
            };

            // ═══════════════════════════════════════════════════════════════════════════════

            class WebRenderer : public PlatformRenderer
            {
            public:
                WebRenderer();
                ~WebRenderer() override;

                // ── دورة الحياة ──
                bool initialize(void *nativeSurface,
                                const PlatformRenderOptions &options = {}) override;
                void shutdown() override;
                bool isReady() const override;

                bool loadFont(const std::string &fontPath, float fontSize = 16.0f) override;

                // ── حلقة الرسم ──
                void beginFrame(const Color &clearColor = {}) override;
                void endFrame() override;

                // ── دوال الرسم الأساسية ──
                void drawFilledRect(float x, float y, float w, float h,
                                    const Color &color) override;
                void drawRoundedRect(float x, float y, float w, float h,
                                     const Color &color, float radius) override;
                void drawRectOutline(float x, float y, float w, float h,
                                     const Color &color, float thickness) override;
                std::pair<float, float> drawText(const std::string &text,
                                                 float x, float y,
                                                 const Color &color,
                                                 float fontSize) override;
                std::pair<float, float> measureText(const std::string &text,
                                                    float fontSize) override;
                void drawImage(const std::string &path,
                               float x, float y, float w, float h) override;
                void drawCircle(float cx, float cy, float radius,
                                const Color &color) override;
                void drawCircleOutline(float cx, float cy, float radius,
                                       const Color &color, float thickness) override;
                void drawLine(float x1, float y1, float x2, float y2,
                              const Color &color, float thickness) override;
                void drawLinearGradient(float x, float y, float w, float h,
                                        const Color &startColor, const Color &endColor,
                                        bool vertical, float radius) override;
                void setClipRect(float x, float y, float w, float h) override;
                void clearClipRect() override;

                // ── دعم الدوران (Canvas2D: save/translate/rotate/restore) ──
                void pushRotation(float centerX, float centerY, float angleDegrees) override;
                void popRotation() override;

                // ── دعم التكبير (Canvas2D: save/translate/scale/restore) ──
                void pushScale(float centerX, float centerY, float scaleX, float scaleY) override;
                void popScale() override;

                /// تعيين خيارات خاصة بالويب
                void setWebOptions(const WebRendererOptions &opts) { webOpts_ = opts; }

            private:
                WebRendererOptions webOpts_;
                bool initialized_ = false;
                int canvasWidth_ = 800;
                int canvasHeight_ = 600;
                std::string defaultFontFamily_;

                // ─── تحسينات الأداء: كاش النصوص والصور ───
                struct TextMeasureKey
                {
                    std::string text;
                    float fontSize;
                    bool operator==(const TextMeasureKey &o) const
                    {
                        return text == o.text && fontSize == o.fontSize;
                    }
                };
                struct TextMeasureHash
                {
                    size_t operator()(const TextMeasureKey &k) const
                    {
                        return std::hash<std::string>()(k.text) ^ (std::hash<float>()(k.fontSize) << 16);
                    }
                };
                std::unordered_map<TextMeasureKey, std::pair<float, float>, TextMeasureHash> textMeasureCache_;
                static constexpr size_t MAX_TEXT_CACHE = 512;

                /// كاش الصور المحملة (لتجنب تحميل متكرر)
                std::unordered_map<std::string, bool> loadedImages_;
            };

            // ═══════════════════════════════════════════════════════════════════════════════

            class WebWindow : public PlatformWindow
            {
            public:
                WebWindow();
                ~WebWindow() override;

                bool create(const PlatformWindowOptions &options) override;
                void destroy() override;
                bool isOpen() const override;

                void setContent(std::shared_ptr<IRNode> root) override;
                void setStateStore(std::shared_ptr<StateStore> store) override;

                /**
                 * @brief (AR) تطبيق رقع جزئية — تحديث ذكي بدلاً من setContent الكامل
                 * @brief (EN) Apply partial patches — smart update instead of full setContent
                 *
                 * يُعيد تهيئة التحريكات فقط إذا تغيّرت الأبعاد أو الأبناء.
                 * يطلب إعادة رسم الإطار عبر Canvas 2D API.
                 *
                 * @param patchedNodes عدد العقد المتأثرة
                 * @param needsRelayout هل التغييرات تتطلب إعادة تخطيط
                 */
                void applyPatches(size_t patchedNodes, bool needsRelayout = true) override;

                void run() override;
                bool runOneFrame() override;
                void invalidate() override;

                int getWidth() const override;
                int getHeight() const override;
                PlatformRenderer *getRenderer() override;

            private:
                std::unique_ptr<WebRenderer> renderer_;
                std::unique_ptr<LayoutEngine> layoutEngine_;
                std::shared_ptr<IRNode> contentRoot_;
                std::shared_ptr<LayoutResult> layoutResult_;
                std::shared_ptr<StateStore> stateStore_;
                PlatformWindowOptions options_;
                bool isOpen_ = false;
                bool needsRedraw_ = true;
                int width_ = 800;
                int height_ = 600;

                void processEvents();
                void updateLayout();
                void renderFrame();

                // ─── معالج أحداث اللمس الموحد ───
                TouchEventProcessor touchProcessor_;

                // ─── معالج أحداث لوحة المفاتيح الموحد ───
                KeyboardEventProcessor kbProcessor_;

                // ─── معالج أحداث الفأرة/المؤشر الموحد (MouseEvent عبر Emscripten) ───
                MouseEventProcessor mouseProcessor_;

            public:
                /**
                 * @brief (AR) تمرير حدث لمس من JavaScript (Touch API / Pointer Events)
                 * يُستدعى من Emscripten touch callbacks
                 * (emscripten_set_touchstart/end/move_callback)
                 * @param action نوع الحدث: 0=touchstart, 1=touchend/cancel, 2=touchmove
                 * @param touchId معرّف جهاز اللمس
                 * @param fingerId معرّف الإصبع
                 * @param x إحداثي X بالبكسل (نسبي للـCanvas)
                 * @param y إحداثي Y بالبكسل (نسبي للـCanvas)
                 * @param pressure ضغط اللمس
                 * @param dx التغيير الأفقي (للحركة فقط)
                 * @param dy التغيير الرأسي (للحركة فقط)
                 */
                void onNativeTouchEvent(int action, int64_t touchId, int64_t fingerId,
                                        float x, float y, float pressure = 1.0f,
                                        float dx = 0.0f, float dy = 0.0f);

                /**
                 * @brief (AR) تمرير حدث لوحة مفاتيح من JavaScript (KeyboardEvent)
                 */
                void onNativeKeyEvent(int action, int keyCode,
                                      const std::string &keyName,
                                      bool shift = false, bool ctrl = false,
                                      bool alt = false);

                /**
                 * @brief (AR) تمرير إدخال نص من JavaScript (InputEvent / compositionend)
                 */
                void onNativeTextInput(const std::string &text);

                /**
                 * @brief (AR) تمرير حدث زر فأرة من JavaScript (MouseEvent)
                 * @param action 0=mousedown, 1=mouseup
                 * @param button 0=أيسر, 1=أوسط, 2=أيمن
                 * @param x إحداثي X
                 * @param y إحداثي Y
                 */
                void onNativeMouseEvent(int action, int button, float x, float y);

                /**
                 * @brief (AR) تمرير حدث حركة فأرة من JavaScript (mousemove)
                 */
                void onNativeMouseMove(float x, float y,
                                       bool leftPressed = false,
                                       bool middlePressed = false,
                                       bool rightPressed = false);

                /**
                 * @brief (AR) تمرير حدث تمرير من JavaScript (wheel event)
                 */
                void onNativeMouseWheel(float deltaX, float deltaY,
                                        float mouseX, float mouseY,
                                        bool ctrlPressed = false);
            };

        } // namespace web
    } // namespace ui
} // namespace sad

#endif // SAD_UI_WEB_RENDERER_H
