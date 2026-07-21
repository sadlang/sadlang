/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: renderer.cpp
 * المسار: sad_ui/backends/desktop/src/renderer.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ مُصيّر سطح المكتب (Desktop Renderer).
 *
 * يرسم عناصر الواجهة عبر SDL2:
 * - SDL_RenderFillRect للمستطيلات
 * - SDL_ttf للنصوص
 * - SDL_RenderCopy للصور
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * تم التفعيل الكامل — SDL2 مربوط ويعمل
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/desktop/renderer.h"
#include "sad_ui/prop_keys.h" // مفاتيح الخصائص القانونيّة (SoT) — لا literals خام
#include "sad_ui/types.h" // arabicNameToColor

#ifdef SAD_UI_USE_SDL2
#include <SDL.h>
// SDL_ttf.h يأتي من renderer.h (يُحدِّد فيه TTF_Font الحقيقي)
#endif

#include <cmath>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <algorithm>

// ─── الدوال المساعدة المشتركة ───
#include "renderer_internal.h"
#include "text_editing.h"

namespace sad
{
    namespace ui
    {
        namespace desktop
        {

            // ═══════════════════════════════════════════════════════════════════════════════
            // تنفيذ DesktopRenderer
            // ═══════════════════════════════════════════════════════════════════════════════

            DesktopRenderer::DesktopRenderer() = default;

            DesktopRenderer::~DesktopRenderer()
            {
                shutdown();
            }

            bool DesktopRenderer::initialize(SDL_Renderer *sdlRenderer, const RenderOptions &options)
            {
                if (!sdlRenderer)
                    return false;

                sdlRenderer_ = sdlRenderer;
                desktopOptions_ = options;
                initialized_ = true;

#ifdef SAD_UI_USE_SDL2
                // تفعيل وضع المزج للشفافية عالمياً
                SDL_SetRenderDrawBlendMode(sdlRenderer_, SDL_BLENDMODE_BLEND);
                // تحسين جودة القياس للصور والنصوص
                SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); // تصفية خطية
#endif

#ifdef SAD_UI_HAS_SDL_TTF
                // تهيئة SDL_ttf
                if (TTF_Init() == -1)
                {
                    std::cerr << "خطأ تهيئة SDL_ttf: " << TTF_GetError() << std::endl;
                    return false;
                }
#endif

                return true;
            }

            void DesktopRenderer::shutdown()
            {
#ifdef SAD_UI_HAS_SDL_TTF
                // تنظيف ذاكرة التخزين المؤقت للنصوص
                for (auto &[key, cached] : g_textCache_)
                {
                    if (cached.texture)
                    {
                        SDL_DestroyTexture(cached.texture);
                    }
                }
                g_textCache_.clear();

                // تنظيف الخطوط
                for (auto &[size, font] : g_fontSizeCache_)
                {
                    if (font)
                    {
                        TTF_CloseFont(font);
                    }
                }
                g_fontSizeCache_.clear();

                // تنظيف خطوط الإيموجي
                for (auto &[size, font] : g_emojiFontCache_)
                {
                    if (font)
                    {
                        TTF_CloseFont(font);
                    }
                }
                g_emojiFontCache_.clear();
                g_emojiFont_ = nullptr;

                if (defaultFont_)
                {
                    TTF_CloseFont(defaultFont_);
                    defaultFont_ = nullptr;
                }

                TTF_Quit();
#endif

                sdlRenderer_ = nullptr;
                initialized_ = false;
            }

            bool DesktopRenderer::loadFont(const std::string &fontPath, float fontSize)
            {
#ifdef SAD_UI_HAS_SDL_TTF
                defaultFont_ = TTF_OpenFont(fontPath.c_str(), static_cast<int>(fontSize));
                if (!defaultFont_)
                {
                    std::cerr << "خطأ تحميل الخط: " << TTF_GetError() << std::endl;
                    // محاولة تحميل خط بديل حسب المنصة
                    const char *fallbackFonts[] = {
#ifdef _WIN32
                        "C:/Windows/Fonts/arial.ttf",
                        "C:/Windows/Fonts/tahoma.ttf",
                        "C:/Windows/Fonts/segoeui.ttf",
#elif defined(__APPLE__)
                        "/System/Library/Fonts/Helvetica.ttc",
                        "/Library/Fonts/Arial.ttf",
#else
                        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                        "/usr/share/fonts/TTF/DejaVuSans.ttf",
                        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
#endif
                        nullptr};
                    for (int i = 0; fallbackFonts[i] != nullptr; ++i)
                    {
                        defaultFont_ = TTF_OpenFont(fallbackFonts[i], static_cast<int>(fontSize));
                        if (defaultFont_)
                        {
                            std::cout << "تم تحميل الخط البديل: " << fallbackFonts[i] << std::endl;
                            break;
                        }
                    }
                }
                if (defaultFont_)
                {
                    // تفعيل HarfBuzz للخط الافتراضي — دعم العربية
                    TTF_SetFontDirection(defaultFont_, TTF_DIRECTION_RTL);
                    TTF_SetFontScriptName(defaultFont_, "Arab");
                }
                return defaultFont_ != nullptr;
#else
                (void)fontPath;
                (void)fontSize;
                return true;
#endif
            }

            void DesktopRenderer::beginFrame(const Color &clearColor)
            {
                if (!initialized_)
                    return;

#ifdef SAD_UI_USE_SDL2
                // تحديث أبعاد منفذ العرض لقص العناصر خارج الشاشة
                SDL_GetRendererOutputSize(sdlRenderer_, &viewportWidth_, &viewportHeight_);

                SDL_SetRenderDrawColor(sdlRenderer_,
                                       static_cast<uint8_t>(clearColor.r * 255),
                                       static_cast<uint8_t>(clearColor.g * 255),
                                       static_cast<uint8_t>(clearColor.b * 255),
                                       static_cast<uint8_t>(clearColor.a * 255));
                SDL_RenderClear(sdlRenderer_);
#else
                (void)clearColor;
#endif
            }

            void DesktopRenderer::render(
                const std::shared_ptr<IRNode> &root,
                const std::shared_ptr<LayoutResult> &layout)
            {
                if (!initialized_ || !root || !layout)
                    return;

                // رسم الشجرة بالتكرار
                renderNode(*root, *layout);
            }

            void DesktopRenderer::endFrame()
            {
                if (!initialized_)
                    return;

#ifdef SAD_UI_USE_SDL2
                SDL_RenderPresent(sdlRenderer_);
#endif
            }

            // ─── رسم العناصر الفردية ───────────────────

            void DesktopRenderer::renderNode(const IRNode &node, const LayoutResult &layout)
            {
                const auto &origRect = layout.rect;

                // ─── 0.A تطبيق تحويلات الانيميشن (إزاحة + مقياس + شفافية) ───
                float animOpacity = 1.0f;
                float animTranslateX = 0.0f;
                float animTranslateY = 0.0f;
                float animScale = 1.0f;
                float animRotation = 0.0f;

#ifdef SAD_UI_USE_SDL2
                if (animationEngine_ && !node.getId().empty())
                {
                    const std::string &nid = node.getId();
                    if (animationEngine_->hasAnimation(nid, "__opacity"))
                    {
                        animOpacity = static_cast<float>(animationEngine_->getValue(nid, "__opacity", 1.0));
                    }
                    if (animationEngine_->hasAnimation(nid, "__translateX"))
                    {
                        animTranslateX = static_cast<float>(animationEngine_->getValue(nid, "__translateX", 0.0));
                    }
                    if (animationEngine_->hasAnimation(nid, "__translateY"))
                    {
                        animTranslateY = static_cast<float>(animationEngine_->getValue(nid, "__translateY", 0.0));
                    }
                    if (animationEngine_->hasAnimation(nid, "__scale"))
                    {
                        animScale = static_cast<float>(animationEngine_->getValue(nid, "__scale", 1.0));
                    }
                    if (animationEngine_->hasAnimation(nid, "__rotation"))
                    {
                        animRotation = static_cast<float>(animationEngine_->getValue(nid, "__rotation", 0.0));
                    }
                }
#endif

                // حساب المستطيل النهائي بعد تطبيق التحويلات
                LayoutRect rect = origRect;
                rect.x += animTranslateX;
                rect.y += animTranslateY;

                // تطبيق المقياس حول مركز العنصر
                if (animScale != 1.0f && animScale > 0.0f)
                {
                    float cx = rect.x + rect.width / 2.0f;
                    float cy = rect.y + rect.height / 2.0f;
                    float newW = rect.width * animScale;
                    float newH = rect.height * animScale;
                    rect.x = cx - newW / 2.0f;
                    rect.y = cy - newH / 2.0f;
                    rect.width = newW;
                    rect.height = newH;
                }

                // إخفاء العنصر بالكامل إذا شفافيته 0
                if (animOpacity <= 0.001f)
                    return;

                // 0. قص العناصر خارج منفذ العرض (Viewport Culling)
                // تخطي العناصر التي تقع بالكامل خارج الشاشة لتحسين الأداء
                // مع مراعاة إزاحة التمرير (scrollTranslateY_)
#ifdef SAD_UI_USE_SDL2
                if (viewportWidth_ > 0 && viewportHeight_ > 0)
                {
                    float vpW = static_cast<float>(viewportWidth_);
                    float vpH = static_cast<float>(viewportHeight_);
                    float effectiveY = rect.y + scrollTranslateY_;
                    if (rect.x > vpW || effectiveY > vpH ||
                        (rect.x + rect.width) < 0 || (effectiveY + rect.height) < 0)
                    {
                        return; // العنصر خارج الشاشة بالكامل — تخطيه مع كل أبنائه
                    }
                }
#endif

                // 1. رسم الخلفية (إن وجدت)
                const auto *bgProp = node.findProperty(props::BG_COLOR);
                if (!bgProp)
                    bgProp = node.findProperty(props::BG);
                if (bgProp)
                {
                    if (auto *colorStr = std::get_if<std::string>(&bgProp->value))
                    {
                        auto namedColor = arabicNameToColor(*colorStr);
                        if (namedColor)
                        {
                            drawFilledRect(rect, Color::fromNamed(*namedColor));
                        }
                        else if (colorStr->size() > 0 && (*colorStr)[0] == '#')
                        {
                            // تحويل النص السداسي (#RRGGBB) إلى لون
                            unsigned int hex = 0;
                            if (colorStr->size() == 7)
                            {
                                hex = std::stoul(colorStr->substr(1), nullptr, 16);
                                Color c = {static_cast<float>((hex >> 16) & 0xFF) / 255.0f,
                                           static_cast<float>((hex >> 8) & 0xFF) / 255.0f,
                                           static_cast<float>(hex & 0xFF) / 255.0f, 1.0f};
                                drawFilledRect(rect, c);
                            }
                        }
                    }
                }

                // 1.5 رسم الظل (إن وجد)
                const auto *shadowProp = node.findProperty(props::SHADOW);
                if (shadowProp)
                {
                    if (auto *v = std::get_if<double>(&shadowProp->value))
                    {
                        float offset = static_cast<float>(*v);
                        Color shadowColor = {0, 0, 0, 0.2f};
                        LayoutRect shadowRect = {rect.x + offset / 2, rect.y + offset, rect.width, rect.height};
                        drawFilledRect(shadowRect, shadowColor);
                    }
                }

                // 1.6 دعم الشفافية العامة (opacity)
#ifdef SAD_UI_USE_SDL2
                float nodeOpacity = getNumericProp(node.findProperty(props::OPACITY), 1.0f);
                if (nodeOpacity < 0.0f)
                    nodeOpacity = 0.0f;
                if (nodeOpacity > 1.0f)
                    nodeOpacity = 1.0f;
                nodeOpacity *= animOpacity; // دمج شفافية الانيميشن مع الشفافية الأصلية
                bool hasOpacity = (nodeOpacity < 1.0f);
                if (hasOpacity)
                {
                    SDL_SetRenderDrawBlendMode(sdlRenderer_, SDL_BLENDMODE_BLEND);
                }
                currentNodeOpacity_ = nodeOpacity;   // حفظ للاستخدام في drawText/drawImage
                currentNodeRotation_ = animRotation; // حفظ زاوية الدوران
#endif

                // 2. رسم المحتوى حسب نوع العنصر
                switch (node.getType())
                {
                case UINodeType::Text:
                {
                    const auto *textProp = node.findProperty(props::TEXT_LATIN);
                    if (!textProp)
                        textProp = node.findProperty(props::CONTENT); // محتوى
                    if (!textProp)
                        textProp = node.findProperty(props::TEXT); // نص
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            // الحصول على حجم الخط
                            const auto *sizeProp = node.findProperty(props::FONT_SIZE);
                            if (!sizeProp)
                                sizeProp = node.findProperty(props::FONT_SIZE_ALT);
                            if (!sizeProp)
                                sizeProp = node.findProperty(props::SIZE);
                            float fontSize = getNumericProp(sizeProp, 16.0f);

                            // الحصول على لون النص
                            const auto *colorProp = node.findProperty(props::TEXT_COLOR);
                            if (!colorProp)
                                colorProp = node.findProperty(props::COLOR);
                            Color textColor = parseColorProp(colorProp, {0, 0, 0, 1});

                            // الحصول على محاذاة النص
                            const auto *alignProp = node.findProperty(props::ALIGN);
                            std::string alignment = "";
                            if (alignProp)
                            {
                                if (auto *s = std::get_if<std::string>(&alignProp->value))
                                    alignment = *s;
                            }

                            // حساب موقع النص مع دعم RTL
                            float textX = rect.x;
                            if (isArabicUTF8(*text) || alignment == "يمين" || alignment == "right")
                            {
                                // النص العربي: محاذاة لليمين
                                // TTF_SizeUTF8 يعطينا العرض الفعلي للنص
#ifdef SAD_UI_HAS_SDL_TTF
                                TTF_Font *font = getFontForSize(fontSize);
                                if (!font)
                                    font = defaultFont_;
                                if (font)
                                {
                                    int textW = 0, textH = 0;
                                    TTF_SizeUTF8(font, text->c_str(), &textW, &textH);
                                    textX = rect.x + rect.width - static_cast<float>(textW);
                                    if (textX < rect.x)
                                        textX = rect.x;
                                }
#endif
                            }
                            else if (alignment == "وسط" || alignment == "center")
                            {
#ifdef SAD_UI_HAS_SDL_TTF
                                TTF_Font *font = getFontForSize(fontSize);
                                if (!font)
                                    font = defaultFont_;
                                if (font)
                                {
                                    int textW = 0, textH = 0;
                                    TTF_SizeUTF8(font, text->c_str(), &textW, &textH);
                                    textX = rect.x + (rect.width - static_cast<float>(textW)) / 2.0f;
                                }
#endif
                            }

                            drawText(*text, textX, rect.y, textColor, fontSize);
                        }
                    }
                    break;
                }

                case UINodeType::Button:
                {
                    bool isHovered = (hoveredNode_ == &node);
                    bool isPressed = (pressedNode_ == &node);

                    // رسم ظل ناعم للزر (Material Design)
                    float elevation = getNumericProp(node.findProperty(props::SHADOW), 2.0f);
                    float radius = getNumericProp(node.findProperty(props::CORNER_RADIUS), 8.0f);
                    if (!node.findProperty(props::CORNER_RADIUS))
                        radius = getNumericProp(node.findProperty(props::RADIUS), 8.0f);

                    // تأثير hover: رفع الظل
                    if (isHovered && !isPressed)
                        elevation += 2.0f;
                    if (isPressed)
                        elevation = std::max(0.5f, elevation - 1.0f);

                    if (elevation > 0)
                    {
                        drawSoftShadow(rect, radius, elevation * 2.0f,
                                       0.0f, elevation * 0.6f, {0, 0, 0, 0.2f});
                    }

                    // رسم خلفية الزر (مع دعم التدرج)
                    const auto *bgColorProp = node.findProperty(props::BG_COLOR);
                    if (!bgColorProp)
                        bgColorProp = node.findProperty(props::BG);
                    Color btnColor = parseColorProp(bgColorProp, Color::fromNamed(NamedColor::Primary));

                    // تأثير hover: إضاءة اللون
                    if (isHovered && !isPressed)
                    {
                        btnColor.r = std::min(1.0f, btnColor.r + 0.08f);
                        btnColor.g = std::min(1.0f, btnColor.g + 0.08f);
                        btnColor.b = std::min(1.0f, btnColor.b + 0.08f);
                    }
                    // تأثير الضغط: تعتيم اللون
                    if (isPressed)
                    {
                        btnColor.r = std::max(0.0f, btnColor.r - 0.1f);
                        btnColor.g = std::max(0.0f, btnColor.g - 0.1f);
                        btnColor.b = std::max(0.0f, btnColor.b - 0.1f);
                    }

                    const auto *gradProp = node.findProperty(props::GRADIENT);
                    const auto *gradEndProp = node.findProperty(props::GRADIENT_END);
                    if (gradProp && gradEndProp)
                    {
                        Color gradStart = parseColorProp(gradProp, btnColor);
                        Color gradEnd = parseColorProp(gradEndProp, btnColor);
                        drawLinearGradient(rect, gradStart, gradEnd, true, radius);
                    }
                    else
                    {
                        drawRoundedRect(rect, btnColor, radius);
                    }

                    // تأثير ripple عند الضغط (دائرة شبه شفافة)
                    if (isPressed)
                    {
                        Color ripple = {1.0f, 1.0f, 1.0f, 0.15f};
                        drawRoundedRect(rect, ripple, radius);
                    }

                    // رسم نص الزر — «عنوان» المفتاح القانونيّ من مصنع زر()، ثمّ
                    // «أيقونة» (زر_أيقونة)، ثمّ بدائل احتياطيّة. findTextProp يوحّد
                    // الترتيب ويحوّل العدد لنصّ (زر(5)) ويمنع انحراف المفاتيح.
                    std::string btnText;
                    if (findTextProp(node, {props::TITLE, props::ICON, props::TEXT_LATIN, props::TEXT, props::CONTENT}, btnText) && !btnText.empty())
                    {
                        {
                            // لون نص الزر — لون_نص/لون_النص، ثمّ «لون» الصريح، وإلّا
                            // OnPrimary من الثيم (لا أبيض ثابت غير مقروء على الفاتح).
                            const auto *textColorProp = node.findProperty(props::TEXT_COLOR_ALT);
                            if (!textColorProp)
                                textColorProp = node.findProperty(props::TEXT_COLOR);
                            if (!textColorProp)
                                textColorProp = node.findProperty(props::COLOR);
                            Color textColor = parseColorProp(textColorProp, Color::fromNamed(NamedColor::OnPrimary));

                            // حجم الخط: «حجم_الخط» ثمّ «حجم_خط» — بفحص وجود الخاصّيّة
                            // لا بعتبة قيمة هشّة.
                            const auto *fsProp = node.findProperty(props::FONT_SIZE_ALT);
                            if (!fsProp)
                                fsProp = node.findProperty(props::FONT_SIZE);
                            float btnFontSize = getNumericProp(fsProp, 16.0f);

                            // حساب موقع النص في وسط الزر (بدقة عبر TTF_SizeUTF8)
                            float textX = rect.x + rect.width / 2;
                            float textY = rect.y + rect.height / 2 - btnFontSize / 2;
#ifdef SAD_UI_HAS_SDL_TTF
                            TTF_Font *font = getFontForSize(btnFontSize);
                            if (!font)
                                font = defaultFont_;
                            if (font)
                            {
                                int textW = 0, textH = 0;
                                TTF_SizeUTF8(font, btnText.c_str(), &textW, &textH);
                                textX = rect.x + (rect.width - static_cast<float>(textW)) / 2.0f;
                                textY = rect.y + (rect.height - static_cast<float>(textH)) / 2.0f;
                            }
#endif
                            drawText(btnText, textX, textY, textColor, btnFontSize);
                        }
                    }
                    break;
                }

                case UINodeType::Divider:
                {
                    Color divColor = parseColorProp(node.findProperty(props::COLOR), Color::fromNamed(NamedColor::Grey));
                    float thickness = getNumericProp(node.findProperty(props::THICKNESS), rect.height);
                    LayoutRect divRect = {rect.x, rect.y, rect.width, thickness};
                    drawFilledRect(divRect, divColor);
                    break;
                }

                case UINodeType::Image:
                {
                    // تحميل وعرض الصورة
                    const auto *srcProp = node.findProperty(props::SOURCE); // مصدر
                    if (!srcProp)
                        srcProp = node.findProperty(props::SRC_LATIN);
                    if (!srcProp)
                        srcProp = node.findProperty(props::PATH); // مسار
                    bool loaded = false;
                    if (srcProp)
                    {
                        if (auto *path = std::get_if<std::string>(&srcProp->value))
                        {
                            // محاولة رسم الصورة
                            drawImage(*path, rect);
                            loaded = true;
                        }
                    }
                    if (!loaded)
                    {
                        // Placeholder إذا لا يوجد مصدر
                        Color placeholder = Color::fromNamed(NamedColor::LightGray);
                        float imgRadius = getNumericProp(node.findProperty(props::CORNER_RADIUS), 0.0f); // زوايا
                        if (imgRadius > 0)
                            drawRoundedRect(rect, placeholder, imgRadius);
                        else
                            drawFilledRect(rect, placeholder);
                        Color iconColor = Color::fromNamed(NamedColor::Gray);
                        drawText("\xf0\x9f\x96\xbc", rect.x + rect.width / 2 - 8, rect.y + rect.height / 2 - 8, iconColor, 16.0f);
                    }
                    break;
                }

                case UINodeType::TextField:
                case UINodeType::TextArea:
                {
                    bool isFocused = (focusedNode_ == &node);
                    bool isHovered = (hoveredNode_ == &node);

                    Color bgColor = parseColorProp(node.findProperty(props::BG), Color::fromNamed(NamedColor::White));
                    Color borderColor = parseColorProp(node.findProperty(props::BORDER_COLOR), Color::fromNamed(NamedColor::Gray));
                    float radius = getNumericProp(node.findProperty(props::CORNER_RADIUS), 4.0f);
                    float fontSize = getNumericProp(node.findProperty(props::FONT_SIZE_ALT), 14.0f);

                    if (isFocused)
                    {
                        borderColor = Color::fromNamed(NamedColor::Primary);
                        drawSoftShadow(rect, radius, 4.0f, 0.0f, 0.0f,
                                       {borderColor.r, borderColor.g, borderColor.b, 0.2f});
                    }
                    else if (isHovered)
                    {
                        borderColor.r = std::max(0.0f, borderColor.r - 0.15f);
                        borderColor.g = std::max(0.0f, borderColor.g - 0.15f);
                        borderColor.b = std::max(0.0f, borderColor.b - 0.15f);
                    }

                    drawRoundedRect(rect, bgColor, radius);
                    float borderW = isFocused ? 2.0f : 1.0f;
                    drawRoundedRect(rect, borderColor, radius);
                    LayoutRect inner = {rect.x + borderW, rect.y + borderW,
                                        rect.width - borderW * 2, rect.height - borderW * 2};
                    drawRoundedRect(inner, bgColor, std::max(0.0f, radius - borderW));

                    const auto *valueProp = node.findProperty(props::VALUE);
                    if (!valueProp)
                        valueProp = node.findProperty(props::TEXT);
                    if (!valueProp)
                        valueProp = node.findProperty(props::VALUE_LATIN);
                    std::string inputText;
                    if (valueProp)
                    {
                        if (auto *s = std::get_if<std::string>(&valueProp->value))
                            inputText = *s;
                    }

                    float textPad = 8.0f;
                    float textY = rect.y + rect.height / 2 - fontSize / 2;

                    if (!inputText.empty())
                    {
                        Color textColor = parseColorProp(node.findProperty(props::TEXT_COLOR), {0, 0, 0, 1});
                        float textX = rect.x + textPad;
                        bool isArabic = isArabicUTF8(inputText);
#ifdef SAD_UI_HAS_SDL_TTF
                        TTF_Font *font = getFontForSize(fontSize);
                        if (!font)
                            font = defaultFont_;
                        int fullTW = 0, fullTH = 0;
                        if (font)
                            TTF_SizeUTF8(font, inputText.c_str(), &fullTW, &fullTH);
                        if (isArabic && font)
                        {
                            textX = rect.x + rect.width - static_cast<float>(fullTW) - textPad;
                        }
#endif

                        // ─── رسم تحديد النص (Selection Highlight) ───
                        if (isFocused && textEditInteraction_.selectionStart != textEditInteraction_.selectionEnd)
                        {
                            // استخدام حالة التحديد المخزنة في interactionState
                            size_t selS = std::min(textEditInteraction_.selectionStart, textEditInteraction_.selectionEnd);
                            size_t selE = std::max(textEditInteraction_.selectionStart, textEditInteraction_.selectionEnd);
                            if (selS < inputText.size() && selE <= inputText.size())
                            {
#ifdef SAD_UI_HAS_SDL_TTF
                                if (font)
                                {
                                    std::string beforeSel = inputText.substr(0, selS);
                                    std::string selText = inputText.substr(selS, selE - selS);
                                    int selStartW = 0, selW = 0, h = 0;
                                    TTF_SizeUTF8(font, beforeSel.c_str(), &selStartW, &h);
                                    TTF_SizeUTF8(font, selText.c_str(), &selW, &h);
                                    float selX = textX + static_cast<float>(selStartW);
                                    Color selColor = {0.26f, 0.52f, 0.96f, 0.35f}; // أزرق شفاف
                                    LayoutRect selRect = {selX, rect.y + 3, static_cast<float>(selW), rect.height - 6};
                                    drawFilledRect(selRect, selColor);
                                }
#endif
                            }
                        }

                        drawText(inputText, textX, textY, textColor, fontSize);

                        // ─── رسم المؤشر الوامض عند موقع cursorPos ───
                        if (isFocused)
                        {
#ifdef SAD_UI_USE_SDL2
                            Uint32 ticks = SDL_GetTicks();
                            if ((ticks / 500) % 2 == 0)
                            {
                                float cursorX = textX;
#ifdef SAD_UI_HAS_SDL_TTF
                                if (font)
                                {
                                    size_t cPos = textEditInteraction_.cursorPos;
                                    if (cPos > inputText.size())
                                        cPos = inputText.size();
                                    if (cPos > 0 && cPos <= inputText.size())
                                    {
                                        std::string beforeCursor = inputText.substr(0, cPos);
                                        int cw = 0, ch = 0;
                                        TTF_SizeUTF8(font, beforeCursor.c_str(), &cw, &ch);
                                        cursorX = textX + static_cast<float>(cw);
                                    }
                                }
#endif
                                LayoutRect cursor = {cursorX, rect.y + 4, 2, rect.height - 8};
                                drawFilledRect(cursor, borderColor);
                            }
#endif
                        }
                    }
                    else
                    {
                        const auto *hintProp = node.findProperty(props::HINT);
                        if (!hintProp)
                            hintProp = node.findProperty(props::PLACEHOLDER_LATIN);
                        if (hintProp)
                        {
                            if (auto *hint = std::get_if<std::string>(&hintProp->value))
                            {
                                Color hintColor = {0.6f, 0.6f, 0.6f, 1.0f};
                                float textX = rect.x + textPad;
                                if (isArabicUTF8(*hint))
                                {
#ifdef SAD_UI_HAS_SDL_TTF
                                    TTF_Font *font = getFontForSize(fontSize);
                                    if (!font)
                                        font = defaultFont_;
                                    if (font)
                                    {
                                        int tw = 0, th = 0;
                                        TTF_SizeUTF8(font, hint->c_str(), &tw, &th);
                                        textX = rect.x + rect.width - static_cast<float>(tw) - textPad;
                                    }
#endif
                                }
                                drawText(*hint, textX, textY, hintColor, fontSize);
                            }
                        }

                        if (isFocused)
                        {
#ifdef SAD_UI_USE_SDL2
                            Uint32 ticks = SDL_GetTicks();
                            if ((ticks / 500) % 2 == 0)
                            {
                                float cursorX = rect.x + textPad;
                                LayoutRect cursor = {cursorX, rect.y + 4, 2, rect.height - 8};
                                drawFilledRect(cursor, borderColor);
                            }
#endif
                        }
                    }
                    break;
                }

                case UINodeType::Toggle:
                {
                    bool isHoveredT = (hoveredNode_ == &node);
                    bool isPressedT = (pressedNode_ == &node);
                    // رسم مفتاح تبديل
                    bool isOn = getBoolProp(node.findProperty(props::ENABLED), false);
                    Color activeColor = parseColorProp(node.findProperty(props::ACTIVE_COLOR), Color::fromNamed(NamedColor::Primary));
                    Color inactiveColor = parseColorProp(node.findProperty(props::COLOR), Color::fromNamed(NamedColor::LightGray));
                    Color trackColor = isOn ? activeColor : inactiveColor;
                    // تأثير hover
                    if (isHoveredT && !isPressedT)
                    {
                        trackColor.r = std::min(1.0f, trackColor.r + 0.06f);
                        trackColor.g = std::min(1.0f, trackColor.g + 0.06f);
                        trackColor.b = std::min(1.0f, trackColor.b + 0.06f);
                    }
                    LayoutRect track = {rect.x, rect.y + rect.height / 4, rect.width, rect.height / 2};
                    drawRoundedRect(track, trackColor, rect.height / 4);
                    // مقبض مع ظل
                    Color knobColor = Color::fromNamed(NamedColor::White);
                    float knobSize = rect.height - 4;
                    float knobX = isOn ? rect.x + rect.width - knobSize - 2 : rect.x + 2;
                    LayoutRect knobShadow = {knobX - 1, rect.y + 3, knobSize + 2, knobSize + 2};
                    drawRoundedRect(knobShadow, {0, 0, 0, 0.15f}, (knobSize + 2) / 2);
                    LayoutRect knob = {knobX, rect.y + 2, knobSize, knobSize};
                    drawRoundedRect(knob, knobColor, knobSize / 2);
                    // نص التسمية — «عنوان» أوّلًا اتّساقًا مع بقيّة العناصر.
                    const auto *labelProp = node.findProperty(props::TITLE);
                    if (!labelProp)
                        labelProp = node.findProperty(props::TEXT);
                    if (!labelProp)
                        labelProp = node.findProperty(props::CONTENT);
                    if (labelProp)
                    {
                        if (auto *lbl = std::get_if<std::string>(&labelProp->value))
                        {
                            float fsize = getNumericProp(node.findProperty(props::FONT_SIZE_ALT), 14.0f);
                            drawText(*lbl, rect.x + rect.width + 8, rect.y + rect.height / 2 - fsize / 2,
                                     {0, 0, 0, 1}, fsize);
                        }
                    }
                    break;
                }

                case UINodeType::Slider:
                {
                    // رسم شريط منزلق
                    float sliderVal = getNumericProp(node.findProperty(props::VALUE), 50.0f) / 100.0f;
                    sliderVal = std::max(0.0f, std::min(1.0f, sliderVal));
                    Color trackBg = parseColorProp(node.findProperty(props::BG), Color::fromNamed(NamedColor::LightGray));
                    Color trackFill = parseColorProp(node.findProperty(props::COLOR), Color::fromNamed(NamedColor::Primary));
                    float trackY = rect.y + rect.height / 2 - 2;
                    LayoutRect track = {rect.x, trackY, rect.width, 4};
                    drawRoundedRect(track, trackBg, 2.0f);
                    // الجزء المملوء
                    float filledW = rect.width * sliderVal;
                    if (filledW > 0)
                    {
                        LayoutRect filled = {rect.x, trackY, filledW, 4};
                        drawRoundedRect(filled, trackFill, 2.0f);
                    }
                    // مقبض
                    Color thumbColor = parseColorProp(node.findProperty(props::HANDLE_COLOR), trackFill);
                    float thumbX = rect.x + filledW;
                    LayoutRect thumb = {thumbX - 8, rect.y + rect.height / 2 - 8, 16, 16};
                    // ظل المقبض
                    LayoutRect thumbShadow = {thumbX - 9, rect.y + rect.height / 2 - 7, 18, 18};
                    drawRoundedRect(thumbShadow, {0, 0, 0, 0.15f}, 9.0f);
                    drawRoundedRect(thumb, thumbColor, 8.0f);
                    break;
                }

                case UINodeType::Checkbox:
                {
                    bool isHoveredCB = (hoveredNode_ == &node);
                    // رسم مربع اختيار
                    bool checked = getBoolProp(node.findProperty(props::ENABLED), false);
                    Color activeColor = parseColorProp(node.findProperty(props::COLOR), Color::fromNamed(NamedColor::Primary));
                    Color borderColor = checked ? activeColor : Color::fromNamed(NamedColor::Gray);
                    // hover effect
                    if (isHoveredCB && !checked)
                    {
                        borderColor.r = std::max(0.0f, borderColor.r - 0.15f);
                        borderColor.g = std::max(0.0f, borderColor.g - 0.15f);
                        borderColor.b = std::max(0.0f, borderColor.b - 0.15f);
                    }
                    LayoutRect box = {rect.x, rect.y, 20, 20};
                    drawRoundedRect(box, borderColor, 3.0f);
                    if (checked)
                    {
                        LayoutRect inner = {rect.x + 1, rect.y + 1, 18, 18};
                        drawRoundedRect(inner, activeColor, 2.0f);
                        drawText("✓", rect.x + 3, rect.y + 1, Color::fromNamed(NamedColor::White), 15.0f);
                    }
                    else
                    {
                        LayoutRect inner = {rect.x + 1, rect.y + 1, 18, 18};
                        drawRoundedRect(inner, Color::fromNamed(NamedColor::White), 2.0f);
                    }
                    // نص
                    const auto *textProp = node.findProperty(props::TEXT_LATIN);
                    if (!textProp)
                        textProp = node.findProperty(props::TEXT);
                    if (!textProp)
                        textProp = node.findProperty(props::CONTENT);
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            Color textColor = parseColorProp(node.findProperty(props::TEXT_COLOR), {0, 0, 0, 1});
                            float fontSize = getNumericProp(node.findProperty(props::FONT_SIZE_ALT), 14.0f);
                            float textX = rect.x + 28;
                            if (isArabicUTF8(*text))
                            {
                                textX = rect.x + rect.width - 28;
#ifdef SAD_UI_HAS_SDL_TTF
                                TTF_Font *font = getFontForSize(fontSize);
                                if (!font)
                                    font = defaultFont_;
                                if (font)
                                {
                                    int tw = 0, th = 0;
                                    TTF_SizeUTF8(font, text->c_str(), &tw, &th);
                                    textX = rect.x + rect.width - static_cast<float>(tw) - 28;
                                }
#endif
                            }
                            drawText(*text, textX, rect.y + 2, textColor, fontSize);
                        }
                    }
                    break;
                }

                case UINodeType::Radio:
                {
                    // رسم زر راديو (دائرة)
                    bool selected = getBoolProp(node.findProperty(props::ENABLED), false); // مفعل
                    bool isRadioHovered = (hoveredNode_ == &node);
                    Color activeColor = parseColorProp(node.findProperty(props::COLOR), Color::fromNamed(NamedColor::Primary));
                    Color borderColor = selected ? activeColor : Color::fromNamed(NamedColor::Gray);
                    if (isRadioHovered && !selected)
                    {
                        borderColor.r *= 0.7f;
                        borderColor.g *= 0.7f;
                        borderColor.b *= 0.7f;
                    }
                    LayoutRect circle = {rect.x, rect.y, 20, 20};
                    drawRoundedRect(circle, borderColor, 10.0f);
                    LayoutRect innerCircle = {rect.x + 1, rect.y + 1, 18, 18};
                    drawRoundedRect(innerCircle, Color::fromNamed(NamedColor::White), 9.0f);
                    if (selected)
                    {
                        LayoutRect dot = {rect.x + 5, rect.y + 5, 10, 10};
                        drawRoundedRect(dot, activeColor, 5.0f);
                    }
                    // نص
                    const auto *textProp = node.findProperty(props::TEXT_LATIN);
                    if (!textProp)
                        textProp = node.findProperty(props::TEXT); // نص
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            Color textColor = parseColorProp(node.findProperty(props::TEXT_COLOR), {0, 0, 0, 1}); // لون_النص
                            drawText(*text, rect.x + 28, rect.y + 2, textColor, 14.0f);
                        }
                    }
                    break;
                }

                case UINodeType::Card:
                {
                    bool isHovered = (hoveredNode_ == &node);
                    bool isPressed = (pressedNode_ == &node);

                    // بطاقة مع ظل ناعم وزوايا مستديرة (Material Design 3)
                    float cardElevation = getNumericProp(node.findProperty(props::ELEVATION), 4.0f);
                    float cardRadius = getNumericProp(node.findProperty(props::CORNER_RADIUS), 12.0f);

                    // تأثيرات التفاعل
                    if (isPressed)
                        cardElevation = std::max(1.0f, cardElevation - 2.0f);
                    else if (isHovered)
                        cardElevation += 4.0f;

                    // ظل متعدد الطبقات (3 طبقات لمحاكاة Material elevation)
                    if (cardElevation > 0)
                    {
                        // طبقة 1: ظل كبير خفيف (ambient)
                        drawSoftShadow(rect, cardRadius, cardElevation * 3.5f,
                                       0.0f, cardElevation * 0.5f, {0, 0, 0, 0.10f});
                        // طبقة 2: ظل متوسط (umbra)
                        drawSoftShadow(rect, cardRadius, cardElevation * 2.0f,
                                       0.0f, cardElevation * 1.0f, {0, 0, 0, 0.18f});
                        // طبقة 3: ظل حاد (penumbra)
                        drawSoftShadow(rect, cardRadius, cardElevation * 1.0f,
                                       0.0f, cardElevation * 0.4f, {0, 0, 0, 0.12f});
                    }

                    const auto *cardColorProp = node.findProperty(props::COLOR);
                    if (!cardColorProp)
                        cardColorProp = node.findProperty(props::BG);
                    Color cardBg = parseColorProp(cardColorProp, Color::fromNamed(NamedColor::White));

                    // تأثير hover: تراكب شفاف
                    if (isHovered && !isPressed)
                    {
                        cardBg.r = std::min(1.0f, cardBg.r + 0.03f);
                        cardBg.g = std::min(1.0f, cardBg.g + 0.03f);
                        cardBg.b = std::min(1.0f, cardBg.b + 0.03f);
                    }

                    // تأثير pressed: تعتيم
                    if (isPressed)
                    {
                        cardBg.r *= 0.95f;
                        cardBg.g *= 0.95f;
                        cardBg.b *= 0.95f;
                    }

                    // دعم الشفافية
                    float opacity = getNumericProp(node.findProperty(props::OPACITY), 1.0f);
                    if (opacity < 1.0f)
                        cardBg.a *= opacity;

                    // دعم التدرج في البطاقات
                    const auto *gradProp = node.findProperty(props::GRADIENT);
                    const auto *gradEndProp = node.findProperty(props::GRADIENT_END);
                    if (gradProp && gradEndProp)
                    {
                        Color gradStart = parseColorProp(gradProp, cardBg);
                        Color gradEnd = parseColorProp(gradEndProp, cardBg);
                        drawLinearGradient(rect, gradStart, gradEnd, true, cardRadius);
                    }
                    else
                    {
                        drawRoundedRect(rect, cardBg, cardRadius);
                    }

                    // حد البطاقة (إن وجد)
                    const auto *borderColorProp = node.findProperty(props::BORDER_COLOR);
                    if (borderColorProp)
                    {
                        Color borderColor = parseColorProp(borderColorProp, Color::fromNamed(NamedColor::Gray));
                        LayoutRect inner = {rect.x + 1, rect.y + 1, rect.width - 2, rect.height - 2};
                        drawRoundedRect(rect, borderColor, cardRadius);
                        drawRoundedRect(inner, cardBg, cardRadius - 1);
                    }
                    break;
                }

                case UINodeType::ProgressBar:
                {
                    // شريط تقدم مع تأثيرات HD وتدرج لوني
                    float barRadius = getNumericProp(node.findProperty(props::CORNER_RADIUS), rect.height / 2.0f);
                    Color trackColor = parseColorProp(node.findProperty(props::BG), {0.9f, 0.9f, 0.9f, 1.0f});
                    drawRoundedRect(rect, trackColor, barRadius);

                    // قراءة القيمة
                    float progressValue = getNumericProp(node.findProperty(props::VALUE),
                                                         getNumericProp(node.findProperty(props::VALUE_LATIN), 50.0f));
                    progressValue /= 100.0f;
                    progressValue = std::max(0.0f, std::min(1.0f, progressValue));

                    // لون الشريط (مع دعم تدرج تلقائي)
                    Color fillColor = parseColorProp(node.findProperty(props::COLOR), Color::fromNamed(NamedColor::Primary));
                    float fillWidth = rect.width * progressValue;
                    if (fillWidth > 0)
                    {
                        LayoutRect fill = {rect.x, rect.y, fillWidth, rect.height};
                        // تدرج خفيف لمظهر لامع
                        Color lighter = {
                            std::min(1.0f, fillColor.r * 1.2f),
                            std::min(1.0f, fillColor.g * 1.2f),
                            std::min(1.0f, fillColor.b * 1.2f),
                            fillColor.a};
                        drawLinearGradient(fill, lighter, fillColor, true, barRadius);
                    }
                    break;
                }

                case UINodeType::SearchBar:
                {
                    // شريط بحث
                    Color bgColor = parseColorProp(node.findProperty(props::BG), Color::fromNamed(NamedColor::LightGray));
                    float radius = getNumericProp(node.findProperty(props::CORNER_RADIUS), rect.height / 2);
                    drawRoundedRect(rect, bgColor, radius);
                    // أيقونة بحث
                    Color iconColor = parseColorProp(node.findProperty(props::ICON_COLOR), Color::fromNamed(NamedColor::Gray));
                    drawText("🔍", rect.x + 8, rect.y + rect.height / 2 - 8, iconColor, 14.0f);
                    // نص تلميحي
                    const auto *hintProp = node.findProperty(props::HINT);
                    if (!hintProp)
                        hintProp = node.findProperty(props::PLACEHOLDER_LATIN);
                    if (hintProp)
                    {
                        if (auto *hint = std::get_if<std::string>(&hintProp->value))
                        {
                            Color hintColor = {0.5f, 0.5f, 0.5f, 1.0f};
                            drawText(*hint, rect.x + 30, rect.y + rect.height / 2 - 7, hintColor, 14.0f);
                        }
                    }
                    break;
                }

                case UINodeType::FAB:
                {
                    // زر عائم دائري مع ظل ناعم (Material Design)
                    Color fabColor = parseColorProp(node.findProperty(props::BG_COLOR), Color::fromNamed(NamedColor::Primary));
                    if (!node.findProperty(props::BG_COLOR))
                        fabColor = parseColorProp(node.findProperty(props::COLOR), fabColor);
                    float radius = std::min(rect.width, rect.height) / 2;
                    // ظل ناعم
                    float elevation = getNumericProp(node.findProperty(props::SHADOW), 6.0f);
                    if (elevation > 0)
                    {
                        drawSoftShadow(rect, radius, elevation * 2.0f,
                                       0.0f, elevation * 0.7f, {0, 0, 0, 0.3f});
                    }
                    drawRoundedRect(rect, fabColor, radius);
                    // نصّ الزرّ العائم: «أيقونة» أوّلًا (ما يكتبه مصنع زر_عائم،
                    // بهمزة على الألف) ثمّ بدائل. كان المفتاح يُكتب بألف مجرَّدة
                    // خطأً فلا تُرسم الأيقونة أبدًا — findTextProp يوحّده ويصحّحه.
                    std::string fabText;
                    if (findTextProp(node, {props::ICON, props::TEXT_LATIN, props::TEXT, props::CONTENT}, fabText) && !fabText.empty())
                    {
                        {
                            Color textColor = parseColorProp(node.findProperty(props::TEXT_COLOR), Color::fromNamed(NamedColor::OnPrimary));
                            float fontSize = getNumericProp(node.findProperty(props::FONT_SIZE_ALT), 20.0f);
#ifdef SAD_UI_HAS_SDL_TTF
                            TTF_Font *font = getFontForSize(fontSize);
                            if (!font)
                                font = defaultFont_;
                            if (font)
                            {
                                int tw = 0, th = 0;
                                TTF_SizeUTF8(font, fabText.c_str(), &tw, &th);
                                drawText(fabText, rect.x + (rect.width - tw) / 2, rect.y + (rect.height - th) / 2, textColor, fontSize);
                            }
                            else
#endif
                                drawText(fabText, rect.x + rect.width / 2 - 8, rect.y + rect.height / 2 - 8, textColor, fontSize);
                        }
                    }
                    break;
                }

                case UINodeType::AppBar:
                {
                    // شريط تطبيق علوي مع تدرج لوني وظل
                    Color barColor = parseColorProp(node.findProperty(props::BG_COLOR), Color::fromNamed(NamedColor::Primary));
                    if (!node.findProperty(props::BG_COLOR))
                        barColor = parseColorProp(node.findProperty(props::COLOR), barColor);
                    // ظل ناعم أسفل الشريط
                    float elevation = getNumericProp(node.findProperty(props::SHADOW), 3.0f);
                    if (elevation > 0)
                    {
                        drawSoftShadow(rect, 0.0f, elevation * 2.0f,
                                       0.0f, elevation, {0, 0, 0, 0.15f});
                    }
                    // خلفية بتدرج خفيف
                    Color barLighter = {
                        std::min(1.0f, barColor.r * 1.15f),
                        std::min(1.0f, barColor.g * 1.15f),
                        std::min(1.0f, barColor.b * 1.15f),
                        barColor.a};
                    drawLinearGradient(rect, barLighter, barColor, true);
                    // «عنوان» أوّلًا (الترتيب القانونيّ الموحَّد عبر العناصر).
                    const auto *textProp = node.findProperty(props::TITLE);
                    if (!textProp)
                        textProp = node.findProperty(props::TEXT_LATIN);
                    if (!textProp)
                        textProp = node.findProperty(props::TEXT);
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            Color textColor = parseColorProp(node.findProperty(props::TEXT_COLOR), Color::fromNamed(NamedColor::White));
                            float fontSize = getNumericProp(node.findProperty(props::FONT_SIZE_ALT), 20.0f);
                            float textX = rect.x + 16;
                            // RTL: محاذاة لليمين
                            if (isArabicUTF8(*text))
                            {
#ifdef SAD_UI_HAS_SDL_TTF
                                TTF_Font *font = getFontForSize(fontSize);
                                if (!font)
                                    font = defaultFont_;
                                if (font)
                                {
                                    int tw = 0, th = 0;
                                    TTF_SizeUTF8(font, text->c_str(), &tw, &th);
                                    textX = rect.x + rect.width - static_cast<float>(tw) - 16;
                                }
#endif
                            }
                            drawText(*text, textX, rect.y + rect.height / 2 - fontSize / 2, textColor, fontSize);
                        }
                    }
                    break;
                }

                case UINodeType::Badge:
                {
                    // شارة إشعار
                    Color badgeColor = parseColorProp(node.findProperty(props::BG_COLOR), Color::fromNamed(NamedColor::Error));
                    if (!node.findProperty(props::BG_COLOR))
                        badgeColor = parseColorProp(node.findProperty(props::COLOR), badgeColor);
                    float r = std::min(rect.width, rect.height) / 2;
                    drawRoundedRect(rect, badgeColor, r);
                    const auto *textProp = node.findProperty(props::TEXT_LATIN);
                    if (!textProp)
                        textProp = node.findProperty(props::TEXT);
                    if (!textProp)
                        textProp = node.findProperty(props::CONTENT);
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            Color textColor = parseColorProp(node.findProperty(props::TEXT_COLOR), Color::fromNamed(NamedColor::White));
                            float fontSize = getNumericProp(node.findProperty(props::FONT_SIZE_ALT), 10.0f);
#ifdef SAD_UI_HAS_SDL_TTF
                            TTF_Font *font = getFontForSize(fontSize);
                            if (!font)
                                font = defaultFont_;
                            if (font)
                            {
                                int tw = 0, th = 0;
                                TTF_SizeUTF8(font, text->c_str(), &tw, &th);
                                drawText(*text, rect.x + (rect.width - tw) / 2, rect.y + (rect.height - th) / 2, textColor, fontSize);
                            }
                            else
#endif
                                drawText(*text, rect.x + 4, rect.y + 2, textColor, fontSize);
                        }
                    }
                    break;
                }

                case UINodeType::Chip:
                {
                    // رقاقة
                    Color chipBg = parseColorProp(node.findProperty(props::BG), Color::fromNamed(NamedColor::LightGray));
                    if (!node.findProperty(props::BG))
                        chipBg = parseColorProp(node.findProperty(props::COLOR), chipBg);
                    float chipRadius = getNumericProp(node.findProperty(props::CORNER_RADIUS), 16.0f);
                    drawRoundedRect(rect, chipBg, chipRadius);
                    // الحد
                    const auto *borderProp = node.findProperty(props::BORDER_COLOR);
                    if (borderProp)
                    {
                        Color borderColor = parseColorProp(borderProp, Color::fromNamed(NamedColor::Gray));
                        drawRoundedRect(rect, borderColor, chipRadius);
                        LayoutRect inner = {rect.x + 1, rect.y + 1, rect.width - 2, rect.height - 2};
                        drawRoundedRect(inner, chipBg, std::max(0.0f, chipRadius - 1));
                    }
                    const auto *textProp = node.findProperty(props::TEXT_LATIN);
                    if (!textProp)
                        textProp = node.findProperty(props::TEXT);
                    if (!textProp)
                        textProp = node.findProperty(props::CONTENT);
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            Color textColor = parseColorProp(node.findProperty(props::TEXT_COLOR), {0, 0, 0, 1});
                            float fontSize = getNumericProp(node.findProperty(props::FONT_SIZE_ALT), 13.0f);
#ifdef SAD_UI_HAS_SDL_TTF
                            TTF_Font *font = getFontForSize(fontSize);
                            if (!font)
                                font = defaultFont_;
                            if (font)
                            {
                                int tw = 0, th = 0;
                                TTF_SizeUTF8(font, text->c_str(), &tw, &th);
                                drawText(*text, rect.x + (rect.width - tw) / 2, rect.y + (rect.height - th) / 2, textColor, fontSize);
                            }
                            else
#endif
                                drawText(*text, rect.x + 12, rect.y + rect.height / 2 - 7, textColor, fontSize);
                        }
                    }
                    break;
                }

                case UINodeType::Avatar:
                {
                    // صورة رمزية دائرية
                    Color avatarBg = parseColorProp(node.findProperty(props::BG), Color::fromNamed(NamedColor::Primary));
                    if (!node.findProperty(props::BG))
                        avatarBg = parseColorProp(node.findProperty(props::COLOR), avatarBg);
                    float r = std::min(rect.width, rect.height) / 2;
                    drawRoundedRect(rect, avatarBg, r);
                    const auto *textProp = node.findProperty(props::TEXT_LATIN);
                    if (!textProp)
                        textProp = node.findProperty(props::TEXT);
                    if (!textProp)
                        textProp = node.findProperty(props::CHAR);
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            Color textColor = parseColorProp(node.findProperty(props::TEXT_COLOR), Color::fromNamed(NamedColor::White));
                            float fontSize = getNumericProp(node.findProperty(props::FONT_SIZE_ALT), 18.0f);
#ifdef SAD_UI_HAS_SDL_TTF
                            TTF_Font *font = getFontForSize(fontSize);
                            if (!font)
                                font = defaultFont_;
                            if (font)
                            {
                                int tw = 0, th = 0;
                                TTF_SizeUTF8(font, text->c_str(), &tw, &th);
                                drawText(*text, rect.x + (rect.width - tw) / 2, rect.y + (rect.height - th) / 2, textColor, fontSize);
                            }
                            else
#endif
                                drawText(*text, rect.x + r - 8, rect.y + r - 10, textColor, fontSize);
                        }
                    }
                    break;
                }

                case UINodeType::SnackBar:
                {
                    // شريط إشعار أسفل الشاشة
                    Color snackBg = parseColorProp(node.findProperty(props::BG), {0.2f, 0.2f, 0.2f, 0.9f});
                    float radius = getNumericProp(node.findProperty(props::CORNER_RADIUS), 4.0f);
                    drawRoundedRect(rect, snackBg, radius);
                    const auto *textProp = node.findProperty(props::TEXT_LATIN);
                    if (!textProp)
                        textProp = node.findProperty(props::TEXT);
                    if (!textProp)
                        textProp = node.findProperty(props::CONTENT);
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            Color textColor = parseColorProp(node.findProperty(props::TEXT_COLOR), Color::fromNamed(NamedColor::White));
                            float fontSize = getNumericProp(node.findProperty(props::FONT_SIZE_ALT), 14.0f);
                            float textX = rect.x + 16;
                            if (isArabicUTF8(*text))
                            {
#ifdef SAD_UI_HAS_SDL_TTF
                                TTF_Font *font = getFontForSize(fontSize);
                                if (!font)
                                    font = defaultFont_;
                                if (font)
                                {
                                    int tw = 0, th = 0;
                                    TTF_SizeUTF8(font, text->c_str(), &tw, &th);
                                    textX = rect.x + rect.width - static_cast<float>(tw) - 16;
                                }
#endif
                            }
                            drawText(*text, textX, rect.y + rect.height / 2 - fontSize / 2, textColor, fontSize);
                        }
                    }
                    break;
                }

                case UINodeType::Spacer:
                {
                    // فاصل شفاف — يُستخدم فقط لحجز مساحة في التخطيط
                    // يمكن رسم خلفية اختيارية للتصحيح
                    if (desktopOptions_.debugBounds)
                    {
                        Color spacerDebug = {0.0f, 0.8f, 0.0f, 0.1f};
                        drawFilledRect(rect, spacerDebug);
                    }
                    break;
                }

                case UINodeType::Icon:
                {
                    // أيقونة
                    Color iconColor = parseColorProp(node.findProperty(props::COLOR), {0, 0, 0, 1});
                    float fontSize = getNumericProp(node.findProperty(props::SIZE), 24.0f);
                    const auto *textProp = node.findProperty(props::TEXT_LATIN);
                    if (!textProp)
                        textProp = node.findProperty(props::ICON_ALT);
                    if (!textProp)
                        textProp = node.findProperty(props::SYMBOL);
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                            drawText(*text, rect.x, rect.y, iconColor, fontSize);
                    }
                    break;
                }

                case UINodeType::Drawer:
                {
                    // ─── درج جانبي (Material Drawer) ───
                    // خلفية شبه شفافة (overlay)
                    int vw = 0, vh = 0;
                    SDL_GetRendererOutputSize(sdlRenderer_, &vw, &vh);
                    LayoutRect overlay = {0, 0, static_cast<float>(vw), static_cast<float>(vh)};
                    drawFilledRect(overlay, {0, 0, 0, 0.4f});

                    // رسم الدرج نفسه
                    float drawerW = getNumericProp(node.findProperty(props::WIDTH), 280.0f);
                    LayoutRect drawerRect = {0, 0, drawerW, static_cast<float>(vh)};
                    drawSoftShadow(drawerRect, 0.0f, 20.0f, 4.0f, 0.0f, {0, 0, 0, 0.3f});
                    Color drawerBg = parseColorProp(node.findProperty(props::BG), Color::fromNamed(NamedColor::White));
                    drawFilledRect(drawerRect, drawerBg);
                    break;
                }

                case UINodeType::BottomSheet:
                {
                    // ─── ورقة سفلية (Material Bottom Sheet) ───
                    int bsVW = 0, bsVH = 0;
                    SDL_GetRendererOutputSize(sdlRenderer_, &bsVW, &bsVH);
                    float sheetH = getNumericProp(node.findProperty(props::HEIGHT), static_cast<float>(bsVH) * 0.5f);
                    float sheetY = static_cast<float>(bsVH) - sheetH;

                    // خلفية شبه شفافة
                    LayoutRect bsOverlay = {0, 0, static_cast<float>(bsVW), static_cast<float>(bsVH)};
                    drawFilledRect(bsOverlay, {0, 0, 0, 0.35f});

                    // الورقة
                    LayoutRect sheetRect = {0, sheetY, static_cast<float>(bsVW), sheetH};
                    drawSoftShadow(sheetRect, 20.0f, 16.0f, 0.0f, -4.0f, {0, 0, 0, 0.25f});
                    Color sheetBg = parseColorProp(node.findProperty(props::BG), Color::fromNamed(NamedColor::White));
                    drawRoundedRect({sheetRect.x, sheetRect.y, sheetRect.width, 24.0f}, sheetBg, 20.0f);
                    drawFilledRect({sheetRect.x, sheetRect.y + 12, sheetRect.width, sheetH - 12}, sheetBg);

                    // مقبض سحب
                    float handleW = 40.0f;
                    LayoutRect handle = {sheetRect.x + sheetRect.width / 2 - handleW / 2, sheetRect.y + 10, handleW, 4.0f};
                    drawRoundedRect(handle, {0.7f, 0.7f, 0.7f, 1.0f}, 2.0f);
                    break;
                }

                case UINodeType::Dialog:
                case UINodeType::Alert:
                {
                    // ─── حوار / تنبيه (Material Dialog) ───
                    int dlgVW = 0, dlgVH = 0;
                    SDL_GetRendererOutputSize(sdlRenderer_, &dlgVW, &dlgVH);

                    // خلفية شبه شفافة (scrim)
                    LayoutRect dlgOverlay = {0, 0, static_cast<float>(dlgVW), static_cast<float>(dlgVH)};
                    drawFilledRect(dlgOverlay, {0, 0, 0, 0.5f});

                    // موقع ومقاس الحوار (وسط الشاشة)
                    float dlgW = getNumericProp(node.findProperty(props::WIDTH), 350.0f);
                    float dlgH = getNumericProp(node.findProperty(props::HEIGHT), 220.0f);
                    float dlgX = (static_cast<float>(dlgVW) - dlgW) / 2.0f;
                    float dlgY = (static_cast<float>(dlgVH) - dlgH) / 2.0f;
                    LayoutRect dlgRect = {dlgX, dlgY, dlgW, dlgH};

                    // ظل ناعم
                    drawSoftShadow(dlgRect, 16.0f, 24.0f, 0.0f, 8.0f, {0, 0, 0, 0.3f});

                    // خلفية الحوار
                    Color dlgBg = parseColorProp(node.findProperty(props::BG), Color::fromNamed(NamedColor::White));
                    drawRoundedRect(dlgRect, dlgBg, 16.0f);

                    // العنوان (إن وجد)
                    const auto *titleProp = node.findProperty(props::TITLE); // عنوان
                    if (!titleProp)
                        titleProp = node.findProperty(props::TITLE_LATIN);
                    if (titleProp)
                    {
                        if (auto *t = std::get_if<std::string>(&titleProp->value))
                        {
                            drawText(*t, dlgX + 24, dlgY + 20, Color::fromNamed(NamedColor::OnSurface), 20.0f);
                        }
                    }

                    // المحتوى (إن وجد)
                    const auto *contentProp = node.findProperty(props::CONTENT); // محتوى
                    if (!contentProp)
                        contentProp = node.findProperty(props::CONTENT_LATIN);
                    if (contentProp)
                    {
                        if (auto *c = std::get_if<std::string>(&contentProp->value))
                        {
                            drawText(*c, dlgX + 24, dlgY + 56, {0.4f, 0.4f, 0.4f, 1.0f}, 14.0f);
                        }
                    }
                    break;
                }

                case UINodeType::Scaffold:
                case UINodeType::Container:
                case UINodeType::Box:
                case UINodeType::Surface:
                case UINodeType::NavigationView:
                case UINodeType::Stack:
                case UINodeType::Group:
                case UINodeType::Center:
                case UINodeType::Padding:
                case UINodeType::SizedBox:
                case UINodeType::Expanded:
                case UINodeType::Flexible:
                case UINodeType::Align:
                case UINodeType::SafeArea:
                case UINodeType::GestureDetector:
                case UINodeType::InkWell:
                case UINodeType::FractionallySizedBox:
                case UINodeType::ConstrainedBox:
                case UINodeType::AspectRatio:
                {
                    // حاويات — خلفية اختيارية، الأبناء يُرسمون بالتكرار
                    const auto *containerBgProp = node.findProperty(props::BG); // خلفية
                    if (!containerBgProp)
                        containerBgProp = node.findProperty(props::BG_COLOR); // لون_خلفية
                    if (containerBgProp)
                    {
                        Color containerBg = parseColorProp(containerBgProp, {0, 0, 0, 0});
                        if (containerBg.a > 0)
                        {
                            float containerRadius = getNumericProp(node.findProperty(props::CORNER_RADIUS), 0.0f); // زوايا
                            if (containerRadius > 0)
                                drawRoundedRect(rect, containerBg, containerRadius);
                            else
                                drawFilledRect(rect, containerBg);
                        }
                    }
                    break;
                }

                case UINodeType::Picker:
                {
                    Color bgColor = Color::fromNamed(NamedColor::White);
                    Color borderColor = Color::fromNamed(NamedColor::Gray);
                    drawFilledRect(rect, bgColor);
                    drawRoundedRect(rect, borderColor, 4.0f);
                    drawText("▼", rect.x + rect.width - 20, rect.y + 4, borderColor, 14.0f);
                    break;
                }

                case UINodeType::TabView:
                {
                    // ─── عرض أقسام (TabView) — رؤوس الأقسام + المحتوى ───
                    Color tabBg = parseColorProp(node.findProperty(props::BG), Color::fromNamed(NamedColor::Primary)); // خلفية
                    float tabH = 48.0f;
                    LayoutRect tabBarRect = {rect.x, rect.y, rect.width, tabH};
                    drawFilledRect(tabBarRect, tabBg);

                    // رسم ألسنة التبويب من أسماء الأبناء
                    size_t tabCount = node.childCount();
                    int selectedTab = static_cast<int>(getNumericProp(node.findProperty(props::SELECTED), 0.0f)); // محدد
                    if (tabCount > 0)
                    {
                        float tabW = rect.width / static_cast<float>(tabCount);
                        for (size_t t = 0; t < tabCount; ++t)
                        {
                            float tx = rect.x + t * tabW;
                            // اسم القسم
                            const auto &tabChild = node.getChildren()[t];
                            const auto *tabLabel = tabChild->findProperty(props::TITLE); // عنوان
                            if (!tabLabel)
                                tabLabel = tabChild->findProperty(props::TEXT); // نص
                            if (!tabLabel)
                                tabLabel = tabChild->findProperty(props::TITLE_LATIN);
                            std::string label = "قسم " + std::to_string(t + 1);
                            if (tabLabel)
                            {
                                if (auto *s = std::get_if<std::string>(&tabLabel->value))
                                    label = *s;
                            }
                            Color textCol = (static_cast<int>(t) == selectedTab) ? Color{1, 1, 1, 1} : Color{1, 1, 1, 0.6f};
                            drawText(label, tx + tabW / 2 - 20, rect.y + 14, textCol, 14.0f);

                            // خط سفلي للقسم المحدد
                            if (static_cast<int>(t) == selectedTab)
                            {
                                LayoutRect indicator = {tx + 8, rect.y + tabH - 3, tabW - 16, 3.0f};
                                drawFilledRect(indicator, {1, 1, 1, 1});
                            }
                        }
                    }
                    break;
                }

                case UINodeType::Grid:
                case UINodeType::LazyColumn:
                case UINodeType::LazyRow:
                case UINodeType::ScrollView:
                case UINodeType::List:
                case UINodeType::ListView:
                case UINodeType::Column:
                case UINodeType::Row:
                case UINodeType::Wrap:
                {
                    // تخطيطات — الأبناء يُرسمون بالتكرار
                    // خلفية اختيارية
                    const auto *layoutBgProp = node.findProperty(props::BG); // خلفية
                    if (!layoutBgProp)
                        layoutBgProp = node.findProperty(props::BG_COLOR); // لون_خلفية
                    if (layoutBgProp)
                    {
                        Color bg = parseColorProp(layoutBgProp, {0, 0, 0, 0});
                        if (bg.a > 0)
                            drawFilledRect(rect, bg);
                    }
                    // قص المحتوى للعناصر القابلة للتمرير
                    if (node.getType() == UINodeType::ScrollView ||
                        node.getType() == UINodeType::LazyColumn ||
                        node.getType() == UINodeType::LazyRow ||
                        node.getType() == UINodeType::List)
                    {
                        setClipRect(&rect);
                    }
                    break;
                }

                    // ── §11 مكونات البيانات (Data Components — v3) ──

                case UINodeType::Tooltip:
                {
                    // تلميح يظهر عند التمرير
                    bool isTooltipHovered = (hoveredNode_ == &node);
                    if (isTooltipHovered)
                    {
                        const auto *tipProp = node.findProperty(props::TEXT);
                        if (!tipProp)
                            tipProp = node.findProperty(props::TEXT_LATIN);
                        if (!tipProp)
                            tipProp = node.findProperty(props::CONTENT);
                        if (tipProp)
                        {
                            if (auto *tip = std::get_if<std::string>(&tipProp->value))
                            {
                                float tipFontSize = 12.0f;
                                float tipPadX = 8.0f, tipPadY = 4.0f;
                                float tipW = static_cast<float>(utf8CharCount(*tip)) * tipFontSize * 0.6f + tipPadX * 2;
                                float tipH = tipFontSize + tipPadY * 2;
                                LayoutRect tipRect = {rect.x, rect.y - tipH - 4, tipW, tipH};
                                drawSoftShadow(tipRect, 4.0f, 4.0f, 0.0f, 2.0f, {0, 0, 0, 0.15f});
                                Color tipBg = {0.2f, 0.2f, 0.2f, 0.9f};
                                drawRoundedRect(tipRect, tipBg, 4.0f);
                                drawText(*tip, tipRect.x + tipPadX, tipRect.y + tipPadY,
                                         Color::fromNamed(NamedColor::White), tipFontSize);
                            }
                        }
                    }
                    break;
                }

                case UINodeType::DataTable:
                {
                    Color bgColor = Color::fromNamed(NamedColor::White);
                    Color borderColor = Color::fromNamed(NamedColor::Gray);
                    drawFilledRect(rect, bgColor);
                    drawRoundedRect(rect, borderColor, 2.0f);
                    // رسم خطوط الشبكة
                    float rowH = 30.0f;
                    for (float y = rect.y + rowH; y < rect.y + rect.height; y += rowH)
                    {
                        LayoutRect line = {rect.x, y, rect.width, 1.0f};
                        drawFilledRect(line, borderColor);
                    }
                    break;
                }

                case UINodeType::TreeView:
                {
                    Color bgColor = Color::fromNamed(NamedColor::White);
                    drawFilledRect(rect, bgColor);
                    break;
                }

                case UINodeType::Breadcrumb:
                {
                    // مسار التنقل: عنصر1 > عنصر2 > عنصر3
                    Color crumbColor = parseColorProp(node.findProperty(props::COLOR), Color::fromNamed(NamedColor::Blue));
                    Color sepColor = Color::fromNamed(NamedColor::Gray);
                    float crumbFontSize = getNumericProp(node.findProperty(props::FONT_SIZE_ALT), 14.0f);
                    float xOff = rect.x + 4;
                    for (size_t ci = 0; ci < node.childCount(); ++ci)
                    {
                        if (ci > 0)
                        {
                            drawText(" › ", xOff, rect.y + 4, sepColor, crumbFontSize);
                            xOff += crumbFontSize * 1.2f;
                        }
                        const auto *childText = node.getChildren()[ci]->findProperty(props::TEXT);
                        if (!childText)
                            childText = node.getChildren()[ci]->findProperty(props::TEXT_LATIN);
                        if (childText)
                        {
                            if (auto *t = std::get_if<std::string>(&childText->value))
                            {
                                bool isLast = (ci == node.childCount() - 1);
                                Color c = isLast ? Color::fromNamed(NamedColor::Black) : crumbColor;
                                drawText(*t, xOff, rect.y + 4, c, crumbFontSize);
                                xOff += static_cast<float>(utf8CharCount(*t)) * crumbFontSize * 0.6f;
                            }
                        }
                    }
                    break;
                }

                case UINodeType::Pagination:
                {
                    // شريط تنقل الصفحات:  « 1 2 3 4 »
                    float currentPage = getNumericProp(node.findProperty(props::PAGE), 1.0f);
                    float totalPages = getNumericProp(node.findProperty(props::TOTAL_SUM), 5.0f);
                    Color activeColor = parseColorProp(node.findProperty(props::COLOR), Color::fromNamed(NamedColor::Primary));
                    Color inactiveColor = Color::fromNamed(NamedColor::LightGray);
                    float btnSize = 28.0f, gap = 4.0f;
                    float startX = rect.x + (rect.width - (totalPages * (btnSize + gap))) / 2;
                    // زر السابق
                    drawText("«", startX - btnSize - gap, rect.y + 4, activeColor, 16.0f);
                    for (int p = 1; p <= static_cast<int>(totalPages); ++p)
                    {
                        float px = startX + (p - 1) * (btnSize + gap);
                        LayoutRect pageRect = {px, rect.y, btnSize, btnSize};
                        bool isCurrent = (p == static_cast<int>(currentPage));
                        drawRoundedRect(pageRect, isCurrent ? activeColor : inactiveColor, 4.0f);
                        Color txtColor = isCurrent ? Color::fromNamed(NamedColor::White) : Color::fromNamed(NamedColor::Black);
                        drawText(std::to_string(p), px + btnSize / 2 - 4, rect.y + 6, txtColor, 14.0f);
                    }
                    // زر التالي
                    float endX = startX + totalPages * (btnSize + gap);
                    drawText("»", endX, rect.y + 4, activeColor, 16.0f);
                    break;
                }

                case UINodeType::Timeline:
                {
                    Color lineColor = Color::fromNamed(NamedColor::Blue);
                    // خط عمودي
                    LayoutRect line = {rect.x + 6.0f, rect.y, 2.0f, rect.height};
                    drawFilledRect(line, lineColor);
                    break;
                }

                    // ── §12 مكونات الوسائط (Media Components — v3) ──

                case UINodeType::Carousel:
                {
                    Color bgColor = {0.95f, 0.95f, 0.95f, 1.0f};
                    drawFilledRect(rect, bgColor);
                    drawRoundedRect(rect, Color::fromNamed(NamedColor::Gray), 8.0f);
                    // أسهم التنقل
                    drawText("◀", rect.x + 4, rect.y + rect.height / 2 - 10, Color::fromNamed(NamedColor::Gray), 20.0f);
                    drawText("▶", rect.x + rect.width - 20, rect.y + rect.height / 2 - 10, Color::fromNamed(NamedColor::Gray), 20.0f);
                    break;
                }

                case UINodeType::RichText:
                case UINodeType::Markdown:
                {
                    const auto *textProp = node.findProperty(props::TEXT_LATIN);
                    if (!textProp)
                        textProp = node.findProperty(props::TEXT);
                    if (!textProp)
                        textProp = node.findProperty(props::CONTENT);
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            Color textColor = parseColorProp(node.findProperty(props::COLOR), Color::fromNamed(NamedColor::Black));
                            float fontSize = getNumericProp(node.findProperty(props::FONT_SIZE_ALT), 14.0f);
                            float textX = rect.x;
                            if (isArabicUTF8(*text))
                            {
#ifdef SAD_UI_HAS_SDL_TTF
                                TTF_Font *font = getFontForSize(fontSize);
                                if (!font)
                                    font = defaultFont_;
                                if (font)
                                {
                                    int tw = 0, th = 0;
                                    TTF_SizeUTF8(font, text->c_str(), &tw, &th);
                                    textX = rect.x + rect.width - static_cast<float>(tw);
                                }
#endif
                            }
                            drawText(*text, textX, rect.y, textColor, fontSize);
                        }
                    }
                    break;
                }

                case UINodeType::CodeBlock:
                {
                    // ─── كتلة كود مع تلوين نحوي وأرقام أسطر ───
                    bool darkThemeCB = getBoolProp(node.findProperty(props::DARK), true);
                    float fontSizeCB = getNumericProp(node.findProperty(props::FONT_SIZE_ALT), 13.0f);
                    bool showLineNums = getBoolProp(node.findProperty(props::LINE_NUMBERS), true);

                    // خلفية الكتلة
                    Color codeBg = darkThemeCB ? Color{0.12f, 0.12f, 0.14f, 1.0f}
                                               : Color{0.96f, 0.96f, 0.96f, 1.0f};
                    drawRoundedRect(rect, codeBg, 6.0f);

                    const auto *textProp = node.findProperty(props::TEXT);
                    if (!textProp)
                        textProp = node.findProperty(props::TEXT_LATIN);
                    if (!textProp)
                        textProp = node.findProperty(props::CODE);
                    if (textProp)
                    {
                        if (auto *codeText = std::get_if<std::string>(&textProp->value))
                        {
                            float lineH = fontSizeCB + 4.0f;
                            float padX = 8.0f, padY = 8.0f;
                            float gutterW = 0.0f;

                            // حساب عدد الأسطر وعرض gutter
                            std::vector<std::string> codeLines;
                            {
                                std::string line;
                                for (size_t ci = 0; ci < codeText->size(); ++ci)
                                {
                                    if ((*codeText)[ci] == '\n')
                                    {
                                        codeLines.push_back(line);
                                        line.clear();
                                    }
                                    else
                                    {
                                        line += (*codeText)[ci];
                                    }
                                }
                                codeLines.push_back(line);
                            }

                            if (showLineNums)
                            {
                                int digits = 1;
                                int n = static_cast<int>(codeLines.size());
                                while (n >= 10)
                                {
                                    ++digits;
                                    n /= 10;
                                }
                                gutterW = static_cast<float>(digits) * fontSizeCB * 0.6f + 16.0f;

                                // خلفية gutter
                                Color gutterBg = darkThemeCB ? Color{0.10f, 0.10f, 0.12f, 1.0f}
                                                             : Color{0.92f, 0.92f, 0.92f, 1.0f};
                                LayoutRect gutterRect = {rect.x, rect.y, gutterW, rect.height};
                                drawRoundedRect(gutterRect, gutterBg, 6.0f);

                                Color lineNumColor = darkThemeCB ? Color{0.45f, 0.45f, 0.50f, 1.0f}
                                                                 : Color{0.55f, 0.55f, 0.55f, 1.0f};
                                for (size_t li = 0; li < codeLines.size(); ++li)
                                {
                                    float ly = rect.y + padY + static_cast<float>(li) * lineH;
                                    if (ly > rect.y + rect.height)
                                        break;
                                    drawText(std::to_string(li + 1),
                                             rect.x + gutterW - padX - 4.0f,
                                             ly, lineNumColor, fontSizeCB);
                                }
                            }

                            // تلوين نحوي
                            SadSyntaxHighlighter highlighter;
                            float codeStartX = rect.x + gutterW + padX;

                            for (size_t li = 0; li < codeLines.size(); ++li)
                            {
                                float ly = rect.y + padY + static_cast<float>(li) * lineH;
                                if (ly > rect.y + rect.height)
                                    break;

                                auto lineTokens = highlighter.tokenize(codeLines[li]);
                                if (lineTokens.empty())
                                    continue;

                                float cx = codeStartX;
                                size_t lastEnd = 0;

                                for (const auto &tok : lineTokens)
                                {
                                    // رسم المسافات قبل الرمز
                                    if (tok.start > lastEnd)
                                    {
                                        std::string gap = codeLines[li].substr(lastEnd, tok.start - lastEnd);
                                        auto gapSize = measureText(gap, fontSizeCB);
                                        cx += gapSize.first;
                                    }

                                    std::string tokText = codeLines[li].substr(tok.start, tok.length);
                                    auto color = SadSyntaxHighlighter::getColor(tok.type, darkThemeCB);
                                    Color tokColor = {color.r, color.g, color.b, color.a};
                                    drawText(tokText, cx, ly, tokColor, fontSizeCB);
                                    auto tokSize = measureText(tokText, fontSizeCB);
                                    cx += tokSize.first;
                                    lastEnd = tok.start + tok.length;
                                }
                            }
                        }
                    }
                    break;
                }

                case UINodeType::AudioPlayer:
                {
                    Color bgColor = {0.95f, 0.95f, 0.95f, 1.0f};
                    drawFilledRect(rect, bgColor);
                    drawRoundedRect(rect, Color::fromNamed(NamedColor::Gray), 8.0f);
                    drawText("▶", rect.x + 8, rect.y + 6, Color::fromNamed(NamedColor::Blue), 18.0f);
                    // شريط التقدم
                    LayoutRect bar = {rect.x + 40, rect.y + rect.height / 2 - 2, rect.width - 90, 4.0f};
                    drawFilledRect(bar, Color::fromNamed(NamedColor::Gray));
                    drawText("00:00", rect.x + rect.width - 45, rect.y + 6, Color::fromNamed(NamedColor::Gray), 12.0f);
                    break;
                }

                case UINodeType::ImageGallery:
                {
                    Color bgColor = {0.97f, 0.97f, 0.97f, 1.0f};
                    drawFilledRect(rect, bgColor);
                    break;
                }

                    // ── §13 مكونات الحركة (Animation Components — v3) ──

                case UINodeType::Skeleton:
                {
                    // هيكل تحميل مع تدرج لامع (shimmer-like)
                    Color placeholderDark = {0.82f, 0.82f, 0.82f, 1.0f};
                    Color placeholderLight = {0.92f, 0.92f, 0.92f, 1.0f};
                    drawLinearGradient(rect, placeholderDark, placeholderLight, false, 4.0f);
                    break;
                }

                case UINodeType::Shimmer:
                {
                    // تأثير وميض مع تدرج
                    Color shimmerDark = {0.88f, 0.88f, 0.88f, 1.0f};
                    Color shimmerLight = {0.96f, 0.96f, 0.96f, 1.0f};
                    drawLinearGradient(rect, shimmerDark, shimmerLight, false);
                    break;
                }

                case UINodeType::AnimatedList:
                case UINodeType::ForEach:
                {
                    // قوائم ديناميكية — رسم خلفية خفيفة + الأبناء يُرسمون بالتكرار أدناه
                    Color listBg = parseColorProp(node.findProperty(props::BG), {0.98f, 0.98f, 0.98f, 1.0f});
                    drawFilledRect(rect, listBg);
                    break;
                }

                case UINodeType::Expandable:
                case UINodeType::Collapsible:
                {
                    const auto *textProp = node.findProperty(props::TEXT_LATIN);
                    std::string title = textProp ? (std::get_if<std::string>(&textProp->value) ? *std::get_if<std::string>(&textProp->value) : "▶ عنصر") : "▶ عنصر";
                    drawText(title, rect.x + 4, rect.y + 4, Color::fromNamed(NamedColor::Black), 14.0f);
                    break;
                }

                case UINodeType::Swipeable:
                {
                    // الأبناء يُرسمون بالتكرار
                    break;
                }

                    // ── §14 مكونات التحكم المتقدمة (Advanced Controls — v3) ──

                case UINodeType::Stepper:
                {
                    Color bgColor = Color::fromNamed(NamedColor::White);
                    Color borderColor = Color::fromNamed(NamedColor::Gray);
                    drawFilledRect(rect, bgColor);
                    drawRoundedRect(rect, borderColor, 4.0f);
                    drawText("−", rect.x + 4, rect.y + 2, Color::fromNamed(NamedColor::Black), 18.0f);
                    drawText("0", rect.x + rect.width / 2 - 6, rect.y + 4, Color::fromNamed(NamedColor::Black), 14.0f);
                    drawText("+", rect.x + rect.width - 20, rect.y + 2, Color::fromNamed(NamedColor::Black), 18.0f);
                    break;
                }

                case UINodeType::SegmentedControl:
                {
                    Color bgColor = {0.93f, 0.93f, 0.93f, 1.0f};
                    Color selectedColor = Color::fromNamed(NamedColor::White);
                    drawFilledRect(rect, bgColor);
                    drawRoundedRect(rect, Color::fromNamed(NamedColor::Gray), 8.0f);
                    // أول شريحة محددة
                    if (rect.width > 20)
                    {
                        float segW = rect.width / 3.0f;
                        LayoutRect sel = {rect.x + 2, rect.y + 2, segW - 4, rect.height - 4};
                        drawFilledRect(sel, selectedColor);
                        drawRoundedRect(sel, Color::fromNamed(NamedColor::Gray), 6.0f);
                    }
                    break;
                }

                case UINodeType::RatingBar:
                {
                    Color starColor = {1.0f, 0.84f, 0.0f, 1.0f}; // ذهبي
                    Color emptyColor = Color::fromNamed(NamedColor::Gray);
                    for (int s = 0; s < 5; s++)
                    {
                        Color c = s < 3 ? starColor : emptyColor;
                        drawText("★", rect.x + s * 22.0f, rect.y, c, 20.0f);
                    }
                    break;
                }

                case UINodeType::DatePicker:
                case UINodeType::TimePicker:
                case UINodeType::Calendar:
                {
                    Color bgColor = Color::fromNamed(NamedColor::White);
                    Color borderColor = Color::fromNamed(NamedColor::Gray);
                    drawFilledRect(rect, bgColor);
                    drawRoundedRect(rect, borderColor, 4.0f);
                    drawText("📅", rect.x + 4, rect.y + 4, Color::fromNamed(NamedColor::Black), 14.0f);
                    break;
                }

                case UINodeType::ColorPicker:
                case UINodeType::ColorWheel:
                {
                    // طيف لوني بالتدرج (أحمر → أخضر → أزرق)
                    float third = rect.width / 3.0f;
                    LayoutRect r1 = {rect.x, rect.y, third + 1, rect.height};
                    LayoutRect r2 = {rect.x + third, rect.y, third + 1, rect.height};
                    LayoutRect r3 = {rect.x + 2 * third, rect.y, third, rect.height};
                    drawLinearGradient(r1, Color::fromNamed(NamedColor::Red), Color::fromNamed(NamedColor::Green), false);
                    drawLinearGradient(r2, Color::fromNamed(NamedColor::Green), Color::fromNamed(NamedColor::Blue), false);
                    drawLinearGradient(r3, Color::fromNamed(NamedColor::Blue), {1.0f, 0.0f, 1.0f, 1.0f}, false);
                    drawRoundedRect(rect, {0, 0, 0, 0}, 4.0f); // حدود فقط
                    break;
                }

                    // ── §15 مكونات التنقل المتقدمة (Extended Navigation — v3) ──

                case UINodeType::BottomNav:
                {
                    Color bgColor = Color::fromNamed(NamedColor::White);
                    drawFilledRect(rect, bgColor);
                    // خط علوي
                    LayoutRect topLine = {rect.x, rect.y, rect.width, 1.0f};
                    drawFilledRect(topLine, Color::fromNamed(NamedColor::Gray));
                    break;
                }

                case UINodeType::SideNav:
                {
                    Color bgColor = {0.95f, 0.95f, 0.97f, 1.0f};
                    drawFilledRect(rect, bgColor);
                    // خط جانبي
                    LayoutRect sideLine = {rect.x + rect.width - 1, rect.y, 1.0f, rect.height};
                    drawFilledRect(sideLine, Color::fromNamed(NamedColor::Gray));
                    break;
                }

                case UINodeType::Toolbar:
                {
                    Color bgColor = {0.96f, 0.96f, 0.96f, 1.0f};
                    drawFilledRect(rect, bgColor);
                    LayoutRect bottomLine = {rect.x, rect.y + rect.height - 1, rect.width, 1.0f};
                    drawFilledRect(bottomLine, Color::fromNamed(NamedColor::Gray));
                    break;
                }

                case UINodeType::SplitView:
                {
                    // خط فاصل في المنتصف
                    float mid = rect.x + rect.width / 2.0f;
                    LayoutRect divider = {mid - 1, rect.y, 2.0f, rect.height};
                    drawFilledRect(divider, Color::fromNamed(NamedColor::Gray));
                    break;
                }

                case UINodeType::Tabs:
                {
                    Color bgColor = {0.95f, 0.95f, 0.95f, 1.0f};
                    drawFilledRect(rect, bgColor);
                    LayoutRect bottomLine = {rect.x, rect.y + rect.height - 2, rect.width, 2.0f};
                    drawFilledRect(bottomLine, Color::fromNamed(NamedColor::Blue));
                    break;
                }

                case UINodeType::MegaMenu:
                {
                    Color bgColor = Color::fromNamed(NamedColor::White);
                    drawFilledRect(rect, bgColor);
                    drawRoundedRect(rect, Color::fromNamed(NamedColor::Gray), 4.0f);
                    break;
                }

                    // ── أنواع v1-v2 مفقودة ──

                    // ─── TextArea handled above with TextField ───

                case UINodeType::LazyGrid:
                {
                    // شبكة كسولة — خلفية + إطار + الأبناء يُعرضون بالتكرار أدناه
                    Color gridBg = parseColorProp(node.findProperty(props::BG), {0.97f, 0.97f, 0.97f, 1.0f});
                    drawFilledRect(rect, gridBg);
                    // رسم خطوط شبكة خفيفة
                    float cellSize = getNumericProp(node.findProperty(props::CELL_SIZE), 80.0f);
                    Color gridLine = {0.9f, 0.9f, 0.9f, 0.5f};
                    for (float gx = rect.x + cellSize; gx < rect.x + rect.width; gx += cellSize)
                    {
                        LayoutRect vLine = {gx, rect.y, 1.0f, rect.height};
                        drawFilledRect(vLine, gridLine);
                    }
                    for (float gy = rect.y + cellSize; gy < rect.y + rect.height; gy += cellSize)
                    {
                        LayoutRect hLine = {rect.x, gy, rect.width, 1.0f};
                        drawFilledRect(hLine, gridLine);
                    }
                    break;
                }

                case UINodeType::NavigationLink:
                {
                    Color linkColor = Color::fromNamed(NamedColor::Blue);
                    drawText("→", rect.x + 4, rect.y + 4, linkColor, 14.0f);
                    break;
                }

                case UINodeType::TabItem:
                case UINodeType::Section:
                {
                    // قسم — عنوان + محتوى
                    const auto *sectionTitle = node.findProperty(props::TITLE);
                    if (!sectionTitle)
                        sectionTitle = node.findProperty(props::TEXT);
                    if (sectionTitle)
                    {
                        if (auto *t = std::get_if<std::string>(&sectionTitle->value))
                        {
                            Color titleColor = parseColorProp(node.findProperty(props::COLOR), {0.3f, 0.3f, 0.3f, 1.0f});
                            drawText(*t, rect.x + 4, rect.y + 2, titleColor, 12.0f);
                            // خط فاصل تحت العنوان
                            LayoutRect sep = {rect.x, rect.y + 18, rect.width, 1.0f};
                            drawFilledRect(sep, {0.85f, 0.85f, 0.85f, 1.0f});
                        }
                    }
                    // الأبناء يُعرضون بالتكرار أدناه
                    break;
                }

                case UINodeType::Sheet:
                {
                    // ظل ناعم للورقة المنبثقة
                    drawSoftShadow(rect, 12.0f, 16.0f, 0.0f, -4.0f, {0, 0, 0, 0.2f});
                    Color bgColor = Color::fromNamed(NamedColor::White);
                    drawFilledRect(rect, bgColor);
                    drawRoundedRect(rect, Color::fromNamed(NamedColor::Gray), 12.0f);
                    // مقبض الورقة
                    float handleW = 40.0f;
                    LayoutRect handle = {rect.x + rect.width / 2 - handleW / 2, rect.y + 8, handleW, 4.0f};
                    drawRoundedRect(handle, Color::fromNamed(NamedColor::Gray), 2.0f);
                    break;
                }

                case UINodeType::Menu:
                {
                    Color bgColor = Color::fromNamed(NamedColor::White);
                    drawFilledRect(rect, bgColor);
                    drawRoundedRect(rect, Color::fromNamed(NamedColor::Gray), 4.0f);
                    break;
                }

                case UINodeType::MapView:
                {
                    Color mapColor = {0.85f, 0.92f, 0.85f, 1.0f};
                    drawFilledRect(rect, mapColor);
                    drawText("🗺️", rect.x + rect.width / 2 - 10, rect.y + rect.height / 2 - 10, Color::fromNamed(NamedColor::Black), 20.0f);
                    break;
                }

                case UINodeType::WebView:
                {
                    Color bgColor = Color::fromNamed(NamedColor::White);
                    drawFilledRect(rect, bgColor);
                    drawRoundedRect(rect, Color::fromNamed(NamedColor::Gray), 2.0f);
                    drawText("🌐", rect.x + 4, rect.y + 4, Color::fromNamed(NamedColor::Blue), 16.0f);
                    break;
                }

                case UINodeType::VideoPlayer:
                {
                    Color bgColor = Color::fromNamed(NamedColor::Black);
                    drawFilledRect(rect, bgColor);
                    // زر تشغيل وسط
                    drawText("▶", rect.x + rect.width / 2 - 12, rect.y + rect.height / 2 - 12, Color::fromNamed(NamedColor::White), 24.0f);
                    break;
                }

                case UINodeType::Canvas:
                {
                    // منطقة رسم مخصصة — خلفية + إطار
                    Color canvasBg = Color::fromNamed(NamedColor::White);
                    drawFilledRect(rect, canvasBg);
                    drawRoundedRect(rect, Color::fromNamed(NamedColor::Gray), 1.0f);
                    // الأبناء (أوامر الرسم) يُعرضون بالتكرار أدناه
                    break;
                }

                case UINodeType::CustomWidget:
                case UINodeType::Conditional:
                {
                    // عناصر مخصصة — الأبناء يُعرضون بالتكرار
                    break;
                }

                default:
                    break;
                }

                // 3. رسم حدود التصحيح (إذا مفعّل)
                if (desktopOptions_.debugBounds)
                {
                    Color debugColor = {1.0f, 0.0f, 0.0f, 0.3f}; // أحمر شفاف
                    drawFilledRect(rect, debugColor);
                }

                // 4. رسم الأبناء (تكرار) — مع دعم التمرير
                bool isScrollable = (node.getType() == UINodeType::ScrollView ||
                                     node.getType() == UINodeType::LazyColumn ||
                                     node.getType() == UINodeType::LazyRow ||
                                     node.getType() == UINodeType::List);

                float prevScrollTranslateY = scrollTranslateY_;
                if (isScrollable)
                {
                    float scrollOff = getScrollOffset(&node);
                    scrollTranslateY_ += scrollOff;
                }

                for (size_t i = 0; i < layout.children.size() && i < node.childCount(); ++i)
                {
                    renderNode(*node.getChildren()[i], *layout.children[i]);
                }

                // 5. إعادة تعيين القص والتمرير بعد العناصر القابلة للتمرير
                if (isScrollable)
                {
                    scrollTranslateY_ = prevScrollTranslateY;
                    setClipRect(nullptr);

                    // 5.1 رسم شريط التمرير (scrollbar)
                    float scrollOff = getScrollOffset(&node);
                    float contentH = layout.contentHeight;
                    float viewH = rect.height;
                    if (contentH > viewH && viewH > 0)
                    {
                        float scrollRatio = viewH / contentH;
                        float barH = std::max(20.0f, viewH * scrollRatio);
                        float maxScroll = contentH - viewH;
                        float barY = rect.y + (-scrollOff / maxScroll) * (viewH - barH);
                        barY = std::max(rect.y, std::min(barY, rect.y + viewH - barH));
                        LayoutRect barRect = {rect.x + rect.width - 6.0f, barY, 4.0f, barH};
                        Color barColor = {0.6f, 0.6f, 0.6f, 0.4f};
                        drawRoundedRect(barRect, barColor, 2.0f);
                    }
                }
            }

        } // namespace desktop
    } // namespace ui
} // namespace sad