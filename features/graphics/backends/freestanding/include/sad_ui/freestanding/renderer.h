/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: renderer.h
 * المسار: features/graphics/backends/freestanding/include/sad_ui/freestanding/renderer.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @brief (AR) مُصيّر الوضع المستقل — يرسم مباشرة على الـ framebuffer
 * @brief (EN) Freestanding Renderer — draws directly to framebuffer memory
 *
 * الوصف:
 * ------
 * مُصيّر للعمل بدون نظام تشغيل (bare-metal) أو داخل نواة نظام تشغيل.
 * لا يعتمد على SDL2 أو أي مكتبة خارجية — يكتب مباشرة في ذاكرة الـ framebuffer.
 *
 * يُستخدم مع:
 *   - نظام أفق (Ufuq OS kernel)
 *   - أي بيئة freestanding تملك framebuffer
 *   - أنظمة UEFI GOP
 *   - محاكيات (QEMU, Bochs)
 *
 * سلسلة العرض:
 * ─────────────
 *   IRNode Tree + LayoutResult → FreestandingRenderer → Framebuffer Memory → الشاشة
 *
 * المميزات:
 *   ✓ رسم مباشر على الذاكرة (mem_write32/memset32)
 *   ✓ خطوط نقطية مدمجة (bitmap fonts) — لا حاجة لـ freetype
 *   ✓ دعم 32-بت ARGB (مع ألفا)
 *   ✓ تخزين مزدوج (double buffering)
 *   ✓ Alpha blending برمجي
 *   ✓ خطوط ودوائر ومستطيلات وتدرجات
 *   ✓ قص (clipping)
 *   ✓ دعم النصوص العربية: تشكيل سياقيّ + عكس مدًى بسيط (arabic_shaper) قبل الرسم
 *     والقياس — لا UAX#9 كاملًا (حدّ معلَن في arabic_shaper.h)
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_FREESTANDING_RENDERER_H
#define SAD_UI_FREESTANDING_RENDERER_H

#include "sad_ui/platform_renderer.h"

#include <string>
#include <memory>
#include <vector>
#include <cstdint>
#include <functional>

namespace sad
{
    namespace ui
    {
        namespace freestanding
        {

            // ═══════════════════════════════════════════════════════════════════════════════
            // إعدادات الـ Framebuffer
            // ═══════════════════════════════════════════════════════════════════════════════

            /**
             * @brief (AR) إعدادات مخزن الإطار
             * @brief (EN) Framebuffer configuration
             */
            struct FramebufferConfig
            {
                uint32_t *address = nullptr; ///< عنوان الـ framebuffer في الذاكرة
                uint32_t width = 0;          ///< عرض الشاشة بالبكسل
                uint32_t height = 0;         ///< ارتفاع الشاشة بالبكسل
                uint32_t pitch = 0;          ///< خطوة السطر بالبايتات (stride)
                uint8_t bpp = 32;            ///< عمق اللون (بت لكل بكسل)

                /// عنوان المخزن الخلفي (للتخزين المزدوج) — nullptr يعني بدون تخزين مزدوج
                uint32_t *backBuffer = nullptr;
            };

            // ═══════════════════════════════════════════════════════════════════════════════
            // بيانات الخط النقطي (Bitmap Font Glyph)
            // ═══════════════════════════════════════════════════════════════════════════════

            /**
             * @brief (AR) حرف نقطي — بيانات رسم حرف واحد
             * @brief (EN) Bitmap glyph data for a single character
             */
            struct BitmapGlyph
            {
                uint32_t codepoint = 0;          ///< نقطة Unicode
                uint8_t width = 0;               ///< عرض الحرف بالبكسل
                uint8_t height = 0;              ///< ارتفاع الحرف بالبكسل
                int8_t xOffset = 0;              ///< إزاحة أفقية
                int8_t yOffset = 0;              ///< إزاحة رأسية
                uint8_t advance = 0;             ///< التقدم الأفقي بعد الحرف
                uint8_t bpp = 1;                 ///< بتات لكلّ بكسل: 1 (نقطيّ) أو 8 (رماديّ HD)
                const uint8_t *bitmap = nullptr; ///< بيانات البكسل (1 أو 8 بت لكلّ بكسل)
            };

            /**
             * @brief (AR) خط نقطي — مجموعة حروف بحجم ثابت أو متغير
             * @brief (EN) Bitmap font — collection of glyphs
             */
            struct BitmapFont
            {
                std::string name;        ///< اسم الخط
                uint8_t charWidth = 8;   ///< عرض الحرف الافتراضي
                uint8_t charHeight = 16; ///< ارتفاع الحرف الافتراضي
                uint8_t lineHeight = 18; ///< ارتفاع السطر
                bool monospace = true;   ///< عرض ثابت أم متغير

                /// الحروف (مفهرسة بنقطة Unicode)
                std::vector<BitmapGlyph> glyphs;

                /// (AR) ملكيّة بيانات الغليفات المحمَّلة من ملفّ (خطوط PSF):
                /// مؤشّرات BitmapGlyph::bitmap خام، والخطوط المدمجة تشير لمصفوفات
                /// ساكنة فلا تحتاج ملكيّة — أمّا المحمَّل من ملفّ فبياناته تعيش هنا،
                /// وshared_ptr يُبقي نسخ BitmapFont (‏loadBitmapFont) آمنًا رخيصًا.
                std::shared_ptr<const std::vector<uint8_t>> ownedGlyphData;

                /// البحث عن حرف بنقطة Unicode
                const BitmapGlyph *findGlyph(uint32_t codepoint) const;
            };

            // ═══════════════════════════════════════════════════════════════════════════════
            // دوال الذاكرة المباشرة (يمكن تجاوزها للنواة)
            // ═══════════════════════════════════════════════════════════════════════════════

            /**
             * @brief (AR) واجهة العمليات على الذاكرة — يمكن للنظام تقديم تطبيق محسّن
             * @brief (EN) Memory operations interface — OS can provide optimized impl
             */
            struct MemoryOps
            {
                /// ملء منطقة بقيمة 32-بت (مثل REP STOSD)
                std::function<void(uint32_t *dst, uint32_t value, size_t count)> fill32;

                /// نسخ ذاكرة (مثل REP MOVSQ)
                std::function<void(uint32_t *dst, const uint32_t *src, size_t count)> copy32;

                /// تصفير ذاكرة
                std::function<void(uint32_t *dst, size_t count)> zero32;
            };

            // ═══════════════════════════════════════════════════════════════════════════════
            // خيارات المُصيّر المستقل
            // ═══════════════════════════════════════════════════════════════════════════════

            struct FreestandingRenderOptions
            {
                bool doubleBuffer = true;  ///< تخزين مزدوج
                bool alphaBlending = true; ///< خلط ألفا
                bool debugBounds = false;  ///< رسم حدود العناصر (للتصحيح)
                float scaleFactor = 1.0f;  ///< معامل التكبير
            };

            // ═══════════════════════════════════════════════════════════════════════════════
            // المُصيّر المستقل (Freestanding Renderer)
            // ═══════════════════════════════════════════════════════════════════════════════

            /**
             * @brief (AR) مُصيّر واجهات المستخدم للوضع المستقل (bare-metal / kernel)
             * @brief (EN) Freestanding UI renderer for bare-metal / kernel environments
             *
             * يُطبق واجهة PlatformRenderer باستخدام كتابات ذاكرة مباشرة.
             * لا يحتاج SDL2 أو أي مكتبة خارجية.
             *
             * @code
             *   // في نواة النظام:
             *   FramebufferConfig fb;
             *   fb.address = (uint32_t*)0xFD000000;
             *   fb.width = 1920;
             *   fb.height = 1080;
             *   fb.pitch = 1920 * 4;
             *
             *   FreestandingRenderer renderer;
             *   renderer.initializeFramebuffer(fb);
             *   renderer.loadBuiltinFont();
             *
             *   // حلقة الرسم:
             *   renderer.beginFrame({0x1A, 0x1B, 0x2E, 0xFF});
             *   renderer.render(irRoot, layoutResult);
             *   renderer.endFrame();  // ينسخ المخزن الخلفي → الشاشة
             * @endcode
             */
            class FreestandingRenderer : public PlatformRenderer
            {
            public:
                FreestandingRenderer();
                ~FreestandingRenderer();

                // ════════════════════════════════════════════════════════════════════════
                // تهيئة الـ Framebuffer
                // ════════════════════════════════════════════════════════════════════════

                /**
                 * @brief (AR) تهيئة المُصيّر باستخدام إعدادات framebuffer
                 * @brief (EN) Initialize renderer with framebuffer configuration
                 *
                 * @param config إعدادات مخزن الإطار (عنوان، عرض، ارتفاع، خطوة)
                 * @param options خيارات إضافية
                 * @return true إذا نجحت التهيئة
                 */
                bool initializeFramebuffer(const FramebufferConfig &config,
                                           const FreestandingRenderOptions &options = {});

                /**
                 * @brief (AR) تعيين عمليات الذاكرة المحسّنة (اختياري)
                 * @brief (EN) Set optimized memory operations (optional)
                 *
                 * إذا لم تُستدعَ، يُستخدم تطبيق افتراضي (حلقة عادية).
                 * النواة يمكنها تقديم تطبيق محسّن باستخدام REP STOSD / REP MOVSQ.
                 */
                void setMemoryOps(const MemoryOps &ops);

                // ════════════════════════════════════════════════════════════════════════
                // الخطوط النقطية
                // ════════════════════════════════════════════════════════════════════════

                /**
                 * @brief (AR) تحميل الخط النقطي المدمج (8×16 ASCII + عربي أساسي)
                 * @brief (EN) Load the built-in bitmap font (8x16 ASCII + basic Arabic)
                 */
                void loadBuiltinFont();

                /**
                 * @brief (AR) تحميل الخطّ الرماديّ عالي الدقّة المضمَّن (Amiri 32px، bpp=8)
                 * @brief (EN) Load the embedded HD grayscale font (Amiri 32px, bpp=8)
                 *
                 * أشكال العرض المتّصلة (FE70–FEFC) منقّطة مباشرةً ⇒ نصّ عربيّ متّصل
                 * وناعم. يعيد false إن كان الأطلس المولَّد فارغًا (فيقع النداء على
                 * loadBuiltinFont النقطيّ الأدنى).
                 */
                bool loadHDFont();

                /**
                 * @brief (AR) تحميل خط نقطي خارجي (PSF أو مخصص)
                 * @brief (EN) Load an external bitmap font
                 */
                bool loadBitmapFont(const BitmapFont &font);

                // ════════════════════════════════════════════════════════════════════════
                // تطبيق واجهة PlatformRenderer
                // ════════════════════════════════════════════════════════════════════════

                bool initialize(void *nativeSurface,
                                const PlatformRenderOptions &options = {}) override;
                void shutdown() override;
                bool isReady() const override;
                bool loadFont(const std::string &fontPath, float fontSize = 16.0f) override;

                void beginFrame(const Color &clearColor = {}) override;
                void endFrame() override;

                void drawFilledRect(float x, float y, float w, float h,
                                    const Color &color) override;
                void drawRoundedRect(float x, float y, float w, float h,
                                     const Color &color, float radius) override;
                void drawRectOutline(float x, float y, float w, float h,
                                     const Color &color, float thickness = 1.0f) override;
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
                                       const Color &color, float thickness = 1.0f) override;
                void drawLine(float x1, float y1, float x2, float y2,
                              const Color &color, float thickness = 1.0f) override;
                void drawLinearGradient(float x, float y, float w, float h,
                                        const Color &startColor, const Color &endColor,
                                        bool vertical = true, float radius = 0.0f) override;
                void setClipRect(float x, float y, float w, float h) override;
                void clearClipRect() override;

                // ════════════════════════════════════════════════════════════════════════
                // دوال خاصة بالوضع المستقل
                // ════════════════════════════════════════════════════════════════════════

                /**
                 * @brief (AR) اسمُ الخطِّ المُحمَّلِ فعلًا (فارغٌ إن لم يُحمَّل خطّ)
                 * @brief (EN) Name of the font actually loaded (empty if none)
                 *
                 * لا زخرفة: بوّابةُ لقطاتِ البكسلِ تُثبِّتُه في ترويسةِ اللقطة، فتبدُّلُ
                 * الخطِّ (الأطلسُ الرماديُّ خلفَ SAD_UI_HD_FONT مثلًا) يُنتِجُ إخفاقًا
                 * **مسمًّى** بدلَ فرقِ بكسلٍ غامضٍ يُطارَدُ في المُنقِّط. الاشتقاقُ من
                 * الحالةِ الفعليّةِ لا من ثابتٍ في الاختبار: الثابتُ يوافقُ نفسَه دائمًا.
                 */
                std::string loadedFontName() const
                {
                    return currentFont_ ? currentFont_->name : std::string();
                }

                /// الحصول على عنوان مخزن الإطار الحالي
                uint32_t *getFramebuffer() const { return drawTarget_; }

                /// أبعاد الشاشة
                uint32_t getWidth() const { return fb_.width; }
                uint32_t getHeight() const { return fb_.height; }

                /// هل التخزين المزدوج مفعّل؟
                bool isDoubleBuffered() const { return fb_.backBuffer != nullptr; }

                /// تبديل المخملين يدوياً (عادة تُستدعى من endFrame)
                void swapBuffers();

                /// تحديث منطقة فقط (بدلاً من الشاشة كلها)
                void swapRegion(int x, int y, int w, int h);

                /**
                 * @brief (AR) عدّاد غليفات أشكال العرض العربيّة (FE70–FEFF) المرسومة فعلًا
                 *
                 * يتقدّم داخل مسار الرسم الحقيقيّ (drawBitmapChar) فقط حين يوجد الغليف
                 * في الخطّ ويُرسم — لا عند مربّع الاستبدال. تعتمده اختبارات الإثبات
                 * الحيّة (fb_demo «نص») بدل إعادة حساب مستقلّة قد تنحرف عن الرسم.
                 */
                uint32_t presentationGlyphsDrawn() const { return presentationGlyphsDrawn_; }

                /// (AR) تصفير عدّاد غليفات أشكال العرض (قبل إطار القياس)
                void resetPresentationGlyphCount() { presentationGlyphsDrawn_ = 0; }

            private:
                // ─── الحالة الداخلية ─────────────────────────

                FramebufferConfig fb_;                ///< إعدادات الـ framebuffer
                FreestandingRenderOptions fbOptions_; ///< خيارات المُصيّر
                uint32_t *drawTarget_ = nullptr;      ///< الهدف الحالي للرسم
                bool initialized_ = false;

                // ─── القص ────────────────────────────────────

                struct ClipRect
                {
                    int x = 0, y = 0, w = 0, h = 0;
                    bool active = false;
                };
                ClipRect clipRect_;

                // ─── الخط ────────────────────────────────────

                std::unique_ptr<BitmapFont> currentFont_;

                /// (AR) غليفات FE70–FEFF المرسومة فعلًا (راجع presentationGlyphsDrawn)
                uint32_t presentationGlyphsDrawn_ = 0;

                // ─── عمليات الذاكرة ──────────────────────────

                MemoryOps memOps_;

                // ─── دوال الرسم الداخلية ─────────────────────

                /// كتابة بكسل واحد مع قص وألفا
                inline void putPixel(int x, int y, uint32_t color);

                /// كتابة بكسل بدون فحوصات (أسرع)
                inline void putPixelFast(int x, int y, uint32_t color);

                /// خلط ألفا بين لونين
                inline uint32_t alphaBlend(uint32_t bg, uint32_t fg) const;

                /// تحويل Color إلى ARGB uint32_t
                inline uint32_t colorToARGB(const Color &c) const;

                /// ملء سطر أفقي (محسّن)
                void fillHLine(int x1, int x2, int y, uint32_t color);

                /// رسم حرف نقطي واحد
                int drawBitmapChar(uint32_t codepoint, int x, int y,
                                   uint32_t color, float scale);

                /// فك Unicode من UTF-8
                static uint32_t decodeUTF8(const char *&ptr, const char *end);

                /// التهيئة الافتراضية لعمليات الذاكرة
                void initDefaultMemOps();
            };

            // ═══════════════════════════════════════════════════════════════════════════════
            // نافذة الوضع المستقل (Freestanding Window)
            // ═══════════════════════════════════════════════════════════════════════════════

            /**
             * @brief (AR) نافذة بسيطة للوضع المستقل (bare-metal / kernel)
             * @brief (EN) Simple window for freestanding / bare-metal environments
             *
             * تُغلّف FreestandingRenderer مع LayoutEngine وحلقة رسم بسيطة.
             * تدعم الانيميشن عبر AnimationEngine الموروث من PlatformWindow.
             *
             * الاستخدام النموذجي:
             * @code
             *   FreestandingWindow win;
             *   FramebufferConfig fb;
             *   fb.address = (uint32_t*)0xFD000000;
             *   fb.width = 1920; fb.height = 1080; fb.pitch = 1920 * 4;
             *
             *   win.initializeFramebuffer(fb);
             *   win.create({.width = 1920, .height = 1080});
             *   win.setContent(irRoot);
             *
             *   // حلقة الرسم (في النواة):
             *   while (win.isOpen()) {
             *       win.runOneFrame();
             *   }
             * @endcode
             */
            class FreestandingWindow : public PlatformWindow
            {
            public:
                FreestandingWindow();
                ~FreestandingWindow() override;

                /// تهيئة الـ framebuffer (يجب استدعاؤها قبل create)
                bool initializeFramebuffer(const FramebufferConfig &config,
                                           const FreestandingRenderOptions &options = {});

                // ════════════════════════════════════════════════════════════════════════
                // تطبيق واجهة PlatformWindow
                // ════════════════════════════════════════════════════════════════════════

                bool create(const PlatformWindowOptions &options) override;
                void destroy() override;
                bool isOpen() const override;

                void setContent(std::shared_ptr<IRNode> root) override;
                void setStateStore(std::shared_ptr<StateStore> store) override;

                /// (AR) جذر المحتوى الحيّ — يحتاجه معالج الفأرة للتحقّق من بقاء
                ///      العقدة المُمسَكة بعد إعادة بناء الشجرة (منع استعمال-بعد-التحرير).
                const IRNode *getContentRoot() const { return contentRoot_.get(); }

                /**
                 * @brief (AR) تطبيق رقع جزئية — تحديث ذكي بدلاً من setContent الكامل
                 * @brief (EN) Apply partial patches — smart update instead of full setContent
                 *
                 * يُعيد تهيئة التحريكات فقط إذا تغيّرت الأبعاد أو الأبناء.
                 * يطلب إعادة رسم الإطار ونسخه للـ framebuffer.
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

                /// الحصول على المُصيّر المستقل مباشرة (للعمليات الخاصة)
                FreestandingRenderer *getFreestandingRenderer() { return renderer_.get(); }

                // ════════════════════════════════════════════════════════════════════════
                // التفاعل (evdev وغيره): اختبار الإصابة + مؤشّر فأرة مرسوم برمجيًّا
                // ════════════════════════════════════════════════════════════════════════

                /**
                 * @brief (AR) العنصر التفاعليّ تحت النقطة (x, y) — أو nullptr
                 * @brief (EN) Interactive element under (x, y) — or nullptr
                 *
                 * نفس دلالة hitTest في backends/desktop (الأبناء من الأعلى للأسفل،
                 * والعقدة تُعاد إن كان لها أحداث أو كانت من الأنواع التفاعليّة)
                 * عدا تعويض إزاحة التمرير: المُصيّر المستقلّ لا يتتبّع إزاحات تمرير
                 * بعد — عناصر التمرير خارج نطاق الوضع المستقلّ حاليًّا (حدّ مُعلَن).
                 */
                const IRNode *hitTest(float x, float y) const;

                /**
                 * @brief (AR) تعيين موقع مؤشّر الفأرة المرسوم فوق الإطار
                 * لا يطلب إعادة رسم بنفسه — المستهلك يستدعي invalidate() عند الحركة.
                 */
                void setCursorPosition(float x, float y);

                /// (AR) إظهار/إخفاء مؤشّر الفأرة المرسوم (مُطفأ افتراضيًّا — رسم ثابت بلا مؤشّر)
                void setCursorVisible(bool visible);

            private:
                std::unique_ptr<FreestandingRenderer> renderer_;
                std::unique_ptr<LayoutEngine> layoutEngine_;
                std::shared_ptr<IRNode> contentRoot_;
                std::shared_ptr<LayoutResult> layoutResult_;
                std::shared_ptr<StateStore> stateStore_;
                PlatformWindowOptions options_;
                bool isOpen_ = false;
                bool needsRedraw_ = true;
                int width_ = 0;
                int height_ = 0;

                // ─── مؤشّر الفأرة المرسوم (تفاعل evdev) ───
                float cursorX_ = 0.0f;
                float cursorY_ = 0.0f;
                bool cursorVisible_ = false;

                void updateLayout();
                void renderFrame();

                /// (AR) اختبار الإصابة العودي (نفس خوارزميّة backends/desktop بلا تمرير)
                static const IRNode *hitTestNode(const IRNode *node,
                                                 const LayoutResult *layout,
                                                 float x, float y);

                /// (AR) رسم مؤشّر الفأرة فوق الإطار (قبل endFrame كي يدخل المخزن المزدوج)
                void drawCursorOverlay();
            };

        } // namespace freestanding
    } // namespace ui
} // namespace sad

#endif // SAD_UI_FREESTANDING_RENDERER_H
