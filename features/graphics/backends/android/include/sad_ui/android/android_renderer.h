/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: android_renderer.h
 * المسار: features/graphics/backends/android/include/sad_ui/android/android_renderer.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * مُصيّر أندرويد — يرسم مباشرة عبر OpenGL ES 3.0 + EGL (Android NDK)
 *
 * بدلاً من توليد كود Jetpack Compose، يرسم مباشرة على سطح OpenGL:
 *   - glClear, glDrawArrays, glDrawElements
 *   - Shader programs للأشكال الأساسية
 *   - FreeType للنصوص (iconv for Arabic)
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_ANDROID_RENDERER_H
#define SAD_UI_ANDROID_RENDERER_H

#include "sad_ui/platform_renderer.h"
#include "sad_ui/touch_processor.h"
#include "sad_ui/keyboard_processor.h"
#include "sad_ui/mouse_processor.h"
#include <string>
#include <unordered_map>

#ifdef __ANDROID__
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <android/native_activity.h>
#include <android/native_window.h>
#endif

namespace sad
{
    namespace ui
    {
        namespace android
        {

            // ═══════════════════════════════════════════════════════════════════════════════

            struct AndroidRendererOptions : public PlatformRenderOptions
            {
                float density = 1.0f; ///< كثافة شاشة الجهاز (dp scale)
            };

            // ═══════════════════════════════════════════════════════════════════════════════

            class AndroidRenderer : public PlatformRenderer
            {
            public:
                AndroidRenderer();
                ~AndroidRenderer() override;

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

                // ── دعم الدوران (OpenGL ES: rotation uniform في الشيدرات) ──
                void pushRotation(float centerX, float centerY, float angleDegrees) override;
                void popRotation() override;

                void setAndroidOptions(const AndroidRendererOptions &opts) { androidOpts_ = opts; }

            private:
                AndroidRendererOptions androidOpts_;
                bool initialized_ = false;
                int surfaceWidth_ = 0;
                int surfaceHeight_ = 0;

#ifdef __ANDROID__
                EGLDisplay eglDisplay_ = EGL_NO_DISPLAY;
                EGLSurface eglSurface_ = EGL_NO_SURFACE;
                EGLContext eglContext_ = EGL_NO_CONTEXT;
                ANativeWindow *nativeWindow_ = nullptr;

                // Shader programs
                GLuint rectProgram_ = 0;
                GLuint circleProgram_ = 0;
                GLuint textProgram_ = 0;
                GLuint texProgram_ = 0; ///< شيدر رسم النصوص والصور (textured quad)

                // Uniforms — rect shader
                GLint rectColorLoc_ = -1;
                GLint rectPosLoc_ = -1;
                GLint rectSizeLoc_ = -1;
                GLint rectResolutionLoc_ = -1;
                GLint rectRadiusLoc_ = -1;
                GLint rectRotationLoc_ = -1;  ///< زاوية الدوران بالراديان
                GLint rectRotCenterLoc_ = -1; ///< مركز الدوران بالبكسل

                // Uniforms — texture shader
                GLint texPosLoc_ = -1;
                GLint texSizeLoc_ = -1;
                GLint texResolutionLoc_ = -1;
                GLint texColorLoc_ = -1;
                GLint texSamplerLoc_ = -1;
                GLint texRotationLoc_ = -1;  ///< زاوية الدوران بالراديان
                GLint texRotCenterLoc_ = -1; ///< مركز الدوران بالبكسل

                // حالة الدوران الحالية (push/pop)
                float activeRotation_ = 0.0f; ///< زاوية الدوران النشطة بالراديان
                float activeRotCenterX_ = 0.0f;
                float activeRotCenterY_ = 0.0f;

                // Font data (stb_truetype)
                std::vector<unsigned char> fontData_;
                bool fontLoaded_ = false;
                float fontSizePixels_ = 16.0f;

                // Image cache
                std::unordered_map<std::string, GLuint> imageCache_;

                bool initEGL(ANativeWindow *window);
                bool initShaders();
                bool initTextureShader();
                void destroyEGL();

                GLuint uploadTexture(const unsigned char *data, int w, int h, int channels);
                void drawTexturedQuad(GLuint texture, float x, float y, float w, float h,
                                      const Color &tint = {1, 1, 1, 1});
#endif

                /// تحويل إحداثيات الشاشة إلى NDC
                void screenToNDC(float x, float y, float &ndcX, float &ndcY) const;
            };

            // ═══════════════════════════════════════════════════════════════════════════════

            class AndroidWindow : public PlatformWindow
            {
            public:
                AndroidWindow();
                ~AndroidWindow() override;

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
                 * يطلب إعادة رسم الإطار عبر OpenGL ES.
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

#ifdef __ANDROID__
                /// تعيين النافذة الأصلية من NativeActivity
                void setNativeWindow(ANativeWindow *window) { nativeWindow_ = window; }
#endif

            private:
                std::unique_ptr<AndroidRenderer> renderer_;
                std::unique_ptr<LayoutEngine> layoutEngine_;
                std::shared_ptr<IRNode> contentRoot_;
                std::shared_ptr<LayoutResult> layoutResult_;
                std::shared_ptr<StateStore> stateStore_;
                PlatformWindowOptions options_;
                bool isOpen_ = false;
                bool needsRedraw_ = true;
                int width_ = 0;
                int height_ = 0;

#ifdef __ANDROID__
                ANativeWindow *nativeWindow_ = nullptr;
#endif

                void processEvents();
                void updateLayout();
                void renderFrame();

                // ─── معالج أحداث اللمس الموحد ───
                TouchEventProcessor touchProcessor_;

                // ─── معالج أحداث لوحة المفاتيح الموحد ───
                KeyboardEventProcessor kbProcessor_;

                // ─── معالج أحداث الفأرة/المؤشر الموحد ───
                MouseEventProcessor mouseProcessor_;

            public:
                /**
                 * @brief (AR) تمرير حدث لمس من طبقة JNI/NativeActivity
                 * يُستدعى من كود Java/Kotlin أو NativeActivity عبر JNI
                 * @param action نوع الحدث: 0=DOWN, 1=UP, 2=MOVE
                 * @param touchId معرّف جهاز اللمس
                 * @param fingerId معرّف الإصبع
                 * @param x إحداثي X بالبكسل
                 * @param y إحداثي Y بالبكسل
                 * @param pressure ضغط اللمس [0.0, 1.0]
                 * @param dx التغيير الأفقي (للحركة فقط)
                 * @param dy التغيير الرأسي (للحركة فقط)
                 */
                void onNativeTouchEvent(int action, int64_t touchId, int64_t fingerId,
                                        float x, float y, float pressure = 1.0f,
                                        float dx = 0.0f, float dy = 0.0f);

                /**
                 * @brief (AR) تمرير حدث لوحة مفاتيح من طبقة JNI
                 * @param action 0=DOWN, 1=UP
                 * @param keyCode رمز المفتاح (UnifiedKeyCode)
                 * @param keyName اسم المفتاح
                 * @param shift هل Shift مضغوط
                 * @param ctrl هل Ctrl مضغوط
                 * @param alt هل Alt مضغوط
                 */
                void onNativeKeyEvent(int action, int keyCode,
                                      const std::string &keyName,
                                      bool shift = false, bool ctrl = false,
                                      bool alt = false);

                /**
                 * @brief (AR) تمرير إدخال نص من IME
                 * @param text النص المُدخل
                 */
                void onNativeTextInput(const std::string &text);

                /**
                 * @brief (AR) تمرير حدث زر فأرة من طبقة JNI (لأجهزة مع فأرة/قلم)
                 * @param action 0=DOWN, 1=UP
                 * @param button 0=أيسر, 1=أوسط, 2=أيمن
                 * @param x إحداثي X بالبكسل
                 * @param y إحداثي Y بالبكسل
                 */
                void onNativeMouseEvent(int action, int button, float x, float y);

                /**
                 * @brief (AR) تمرير حدث حركة فأرة من طبقة JNI
                 * @param x إحداثي X بالبكسل
                 * @param y إحداثي Y بالبكسل
                 * @param leftPressed هل الزر الأيسر مضغوط
                 * @param middlePressed هل الزر الأوسط مضغوط
                 * @param rightPressed هل الزر الأيمن مضغوط
                 */
                void onNativeMouseMove(float x, float y,
                                       bool leftPressed = false,
                                       bool middlePressed = false,
                                       bool rightPressed = false);

                /**
                 * @brief (AR) تمرير حدث عجلة تمرير من طبقة JNI
                 * @param deltaX إزاحة أفقية
                 * @param deltaY إزاحة رأسية
                 * @param mouseX إحداثي المؤشر X
                 * @param mouseY إحداثي المؤشر Y
                 * @param ctrlPressed هل Ctrl مضغوط
                 */
                void onNativeMouseWheel(float deltaX, float deltaY,
                                        float mouseX, float mouseY,
                                        bool ctrlPressed = false);
            };

        } // namespace android
    } // namespace ui
} // namespace sad

#endif // SAD_UI_ANDROID_RENDERER_H
