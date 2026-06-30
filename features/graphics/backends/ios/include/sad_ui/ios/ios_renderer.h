/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: ios_renderer.h
 * المسار: features/graphics/backends/ios/include/sad_ui/ios/ios_renderer.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * مُصيّر iOS — يرسم مباشرة عبر CoreGraphics / Metal
 *
 * بدلاً من توليد كود SwiftUI، يرسم مباشرة على CGContext:
 *   - CGContextFillRect, CGContextFillEllipseInRect
 *   - CoreText للنصوص العربية
 *   - Metal للأداء العالي (اختياري)
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_IOS_RENDERER_H
#define SAD_UI_IOS_RENDERER_H

#include "sad_ui/platform_renderer.h"
#include "sad_ui/touch_processor.h"
#include "sad_ui/keyboard_processor.h"
#include "sad_ui/mouse_processor.h"
#include <string>

#if defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_IOS || TARGET_OS_SIMULATOR
#define SAD_IOS_AVAILABLE 1
#endif
#endif

namespace sad
{
    namespace ui
    {
        namespace ios
        {

            // ═══════════════════════════════════════════════════════════════════════════════

            struct IOSRendererOptions : public PlatformRenderOptions
            {
                bool useMetal = false;    ///< استخدام Metal بدلاً من CoreGraphics
                float screenScale = 2.0f; ///< Retina scale factor
            };

            // ═══════════════════════════════════════════════════════════════════════════════

            class IOSRenderer : public PlatformRenderer
            {
            public:
                IOSRenderer();
                ~IOSRenderer() override;

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

                // ── دعم الدوران (CoreGraphics: SaveGState/TranslateCTM/RotateCTM) ──
                void pushRotation(float centerX, float centerY, float angleDegrees) override;
                void popRotation() override;

                // ── دعم التكبير (CoreGraphics: SaveGState/TranslateCTM/ScaleCTM) ──
                void pushScale(float centerX, float centerY, float scaleX, float scaleY) override;
                void popScale() override;

                void setIOSOptions(const IOSRendererOptions &opts) { iosOpts_ = opts; }

                /// تعيين سياق CoreGraphics الحالي (يُمرر من UIView drawRect:)
                void setCGContext(void *cgContext) { cgContext_ = cgContext; }

            private:
                IOSRendererOptions iosOpts_;
                bool initialized_ = false;
                int viewWidth_ = 0;
                int viewHeight_ = 0;
                void *cgContext_ = nullptr; // CGContextRef — opaque for C++
            };

            // ═══════════════════════════════════════════════════════════════════════════════

            class IOSWindow : public PlatformWindow
            {
            public:
                IOSWindow();
                ~IOSWindow() override;

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
                 * يطلب إعادة رسم الإطار عبر CoreGraphics/Metal.
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
                std::unique_ptr<IOSRenderer> renderer_;
                std::unique_ptr<LayoutEngine> layoutEngine_;
                std::shared_ptr<IRNode> contentRoot_;
                std::shared_ptr<LayoutResult> layoutResult_;
                std::shared_ptr<StateStore> stateStore_;
                PlatformWindowOptions options_;
                bool isOpen_ = false;
                bool needsRedraw_ = true;
                int width_ = 0;
                int height_ = 0;

                // ─── معالج أحداث اللمس الموحد ───
                TouchEventProcessor touchProcessor_;

                // ─── معالج أحداث لوحة المفاتيح الموحد ───
                KeyboardEventProcessor kbProcessor_;

                // ─── معالج أحداث الفأرة/المؤشر الموحد (iPadOS pointer والفأرة المتصلة) ───
                MouseEventProcessor mouseProcessor_;

            public:
                /**
                 * @brief (AR) تمرير حدث لمس من UIKit (UITouch)
                 * يُستدعى من UIView touchesBegan/Moved/Ended عبر Objective-C++ bridge
                 * @param action نوع الحدث: 0=DOWN, 1=UP, 2=MOVE
                 * @param touchId معرّف جهاز اللمس
                 * @param fingerId معرّف الإصبع
                 * @param x إحداثي X بالبكسل
                 * @param y إحداثي Y بالبكسل
                 * @param pressure ضغط اللمس (3D Touch / Force Touch)
                 * @param dx التغيير الأفقي (للحركة فقط)
                 * @param dy التغيير الرأسي (للحركة فقط)
                 */
                void onNativeTouchEvent(int action, int64_t touchId, int64_t fingerId,
                                        float x, float y, float pressure = 1.0f,
                                        float dx = 0.0f, float dy = 0.0f);

                /**
                 * @brief (AR) تمرير حدث لوحة مفاتيح من UIKit
                 */
                void onNativeKeyEvent(int action, int keyCode,
                                      const std::string &keyName,
                                      bool shift = false, bool ctrl = false,
                                      bool alt = false);

                /**
                 * @brief (AR) تمرير إدخال نص من IME
                 */
                void onNativeTextInput(const std::string &text);

                /**
                 * @brief (AR) تمرير حدث زر فأرة من UIKit (iPadOS pointer / فأرة متصلة)
                 * @param action 0=DOWN, 1=UP
                 * @param button 0=أيسر, 1=أوسط, 2=أيمن
                 * @param x إحداثي X
                 * @param y إحداثي Y
                 */
                void onNativeMouseEvent(int action, int button, float x, float y);

                /**
                 * @brief (AR) تمرير حدث حركة مؤشر من UIKit
                 */
                void onNativeMouseMove(float x, float y,
                                       bool leftPressed = false,
                                       bool middlePressed = false,
                                       bool rightPressed = false);

                /**
                 * @brief (AR) تمرير حدث تمرير (scrollWheel) من UIKit
                 */
                void onNativeMouseWheel(float deltaX, float deltaY,
                                        float mouseX, float mouseY,
                                        bool ctrlPressed = false);
            };

        } // namespace ios
    } // namespace ui
} // namespace sad

#endif // SAD_UI_IOS_RENDERER_H
