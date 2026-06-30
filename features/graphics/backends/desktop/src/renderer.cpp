/**
 * ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
 * …„: renderer.cpp
 * ״§„…״³״§״±: sad_ui/backends/desktop/src/renderer.cpp
 * ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
 *
 * ״×†״° …״µ‘״± ״³״·״­ ״§„…ƒ״×״¨ (Desktop Renderer).
 *
 * ״±״³… ״¹†״§״µ״± ״§„ˆ״§״¬‡״© ״¹״¨״± SDL2:
 * - SDL_RenderFillRect „„…״³״×״·„״§״×
 * - SDL_ttf „„†״µˆ״µ
 * - SDL_RenderCopy „„״µˆ״±
 *
 * ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
 * ״×… ״§„״×״¹„ ״§„ƒ״§…„ ג€” SDL2 …״±״¨ˆ״· ˆ״¹…„
 * ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
 *
 * ״­‚ˆ‚ ״§„†״´״± (c) 2024-2026 ״±‚ „״÷״© ״µ
 * ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
 */

#include "sad_ui/desktop/renderer.h"
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

// ג”€ג”€ג”€ ״§„״¯ˆ״§„ ״§„…״³״§״¹״¯״© ״§„…״´״×״±ƒ״© ג”€ג”€ג”€
#include "renderer_internal.h"
#include "text_editing.h"

namespace sad
{
    namespace ui
    {
        namespace desktop
        {

            // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
            // ״×†״° DesktopRenderer
            // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

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
                // ״×״¹„ ˆ״¶״¹ ״§„…״²״¬ „„״´״§״© ״¹״§„…״§‹
                SDL_SetRenderDrawBlendMode(sdlRenderer_, SDL_BLENDMODE_BLEND);
                // ״×״­״³† ״¬ˆ״¯״© ״§„‚״§״³ „„״µˆ״± ˆ״§„†״µˆ״µ
                SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); // ״×״µ״© ״®״·״©
#endif

#ifdef SAD_UI_HAS_SDL_TTF
                // ״×‡״¦״© SDL_ttf
                if (TTF_Init() == -1)
                {
                    std::cerr << "״®״·״£ ״×‡״¦״© SDL_ttf: " << TTF_GetError() << std::endl;
                    return false;
                }
#endif

                return true;
            }

            void DesktopRenderer::shutdown()
            {
#ifdef SAD_UI_HAS_SDL_TTF
                // ״×†״¸ ״°״§ƒ״±״© ״§„״×״®״²† ״§„…״₪‚״× „„†״µˆ״µ
                for (auto &[key, cached] : g_textCache_)
                {
                    if (cached.texture)
                    {
                        SDL_DestroyTexture(cached.texture);
                    }
                }
                g_textCache_.clear();

                // ״×†״¸ ״§„״®״·ˆ״·
                for (auto &[size, font] : g_fontSizeCache_)
                {
                    if (font)
                    {
                        TTF_CloseFont(font);
                    }
                }
                g_fontSizeCache_.clear();

                // ״×†״¸ ״®״·ˆ״· ״§„״¥…ˆ״¬
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
                    std::cerr << "״®״·״£ ״×״­…„ ״§„״®״·: " << TTF_GetError() << std::endl;
                    // …״­״§ˆ„״© ״×״­…„ ״®״· ״¨״¯„ ״­״³״¨ ״§„…†״µ״©
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
                            std::cout << "״×… ״×״­…„ ״§„״®״· ״§„״¨״¯„: " << fallbackFonts[i] << std::endl;
                            break;
                        }
                    }
                }
                if (defaultFont_)
                {
                    // ״×״¹„ HarfBuzz „„״®״· ״§„״§״×״±״§״¶ ג€” ״¯״¹… ״§„״¹״±״¨״©
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
                // ״×״­״¯״« ״£״¨״¹״§״¯ …†״° ״§„״¹״±״¶ „‚״µ ״§„״¹†״§״µ״± ״®״§״±״¬ ״§„״´״§״´״©
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

                // ״±״³… ״§„״´״¬״±״© ״¨״§„״×ƒ״±״§״±
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

            // ג”€ג”€ג”€ ״±״³… ״§„״¹†״§״µ״± ״§„״±״¯״© ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€

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

                // 0. ‚״µ ״§„״¹†״§״µ״± ״®״§״±״¬ …†״° ״§„״¹״±״¶ (Viewport Culling)
                // ״×״®״· ״§„״¹†״§״µ״± ״§„״× ״×‚״¹ ״¨״§„ƒ״§…„ ״®״§״±״¬ ״§„״´״§״´״© „״×״­״³† ״§„״£״¯״§״¡
                // …״¹ …״±״§״¹״§״© ״¥״²״§״­״© ״§„״×…״±״± (scrollTranslateY_)
#ifdef SAD_UI_USE_SDL2
                if (viewportWidth_ > 0 && viewportHeight_ > 0)
                {
                    float vpW = static_cast<float>(viewportWidth_);
                    float vpH = static_cast<float>(viewportHeight_);
                    float effectiveY = rect.y + scrollTranslateY_;
                    if (rect.x > vpW || effectiveY > vpH ||
                        (rect.x + rect.width) < 0 || (effectiveY + rect.height) < 0)
                    {
                        return; // ״§„״¹†״µ״± ״®״§״±״¬ ״§„״´״§״´״© ״¨״§„ƒ״§…„ ג€” ״×״®״·‡ …״¹ ƒ„ ״£״¨†״§״¦‡
                    }
                }
#endif

                // 1. ״±״³… ״§„״®„״© (״¥† ˆ״¬״¯״×)
                const auto *bgProp = node.findProperty("„ˆ†_״®„״©");
                if (!bgProp)
                    bgProp = node.findProperty("״®„״©");
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
                            // ״×״­ˆ„ ״§„†״µ ״§„״³״¯״§״³ (#RRGGBB) ״¥„‰ „ˆ†
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

                // 1.5 ״±״³… ״§„״¸„ (״¥† ˆ״¬״¯)
                const auto *shadowProp = node.findProperty("״¸„");
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

                // 1.6 ״¯״¹… ״§„״´״§״© ״§„״¹״§…״© (opacity)
#ifdef SAD_UI_USE_SDL2
                float nodeOpacity = getNumericProp(node.findProperty("״´״§״©"), 1.0f);
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

                // 2. ״±״³… ״§„…״­״×ˆ‰ ״­״³״¨ †ˆ״¹ ״§„״¹†״µ״±
                switch (node.getType())
                {
                case UINodeType::Text:
                {
                    const auto *textProp = node.findProperty("text");
                    if (!textProp)
                        textProp = node.findProperty("\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89"); // …״­״×ˆ‰
                    if (!textProp)
                        textProp = node.findProperty("\xd9\x86\xd8\xb5"); // †״µ
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            // ״§„״­״µˆ„ ״¹„‰ ״­״¬… ״§„״®״·
                            const auto *sizeProp = node.findProperty("״­״¬…_״®״·");
                            if (!sizeProp)
                                sizeProp = node.findProperty("״­״¬…_״§„״®״·");
                            if (!sizeProp)
                                sizeProp = node.findProperty("״­״¬…");
                            float fontSize = getNumericProp(sizeProp, 16.0f);

                            // ״§„״­״µˆ„ ״¹„‰ „ˆ† ״§„†״µ
                            const auto *colorProp = node.findProperty("„ˆ†_״§„†״µ");
                            if (!colorProp)
                                colorProp = node.findProperty("„ˆ†");
                            Color textColor = parseColorProp(colorProp, {0, 0, 0, 1});

                            // ״§„״­״µˆ„ ״¹„‰ …״­״§״°״§״© ״§„†״µ
                            const auto *alignProp = node.findProperty("…״­״§״°״§״©");
                            std::string alignment = "";
                            if (alignProp)
                            {
                                if (auto *s = std::get_if<std::string>(&alignProp->value))
                                    alignment = *s;
                            }

                            // ״­״³״§״¨ …ˆ‚״¹ ״§„†״µ …״¹ ״¯״¹… RTL
                            float textX = rect.x;
                            if (isArabicUTF8(*text) || alignment == "…†" || alignment == "right")
                            {
                                // ״§„†״µ ״§„״¹״±״¨: …״­״§״°״§״© „„…†
                                // TTF_SizeUTF8 ״¹״·†״§ ״§„״¹״±״¶ ״§„״¹„ „„†״µ
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
                            else if (alignment == "ˆ״³״·" || alignment == "center")
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

                    // ״±״³… ״¸„ †״§״¹… „„״²״± (Material Design)
                    float elevation = getNumericProp(node.findProperty("״¸„"), 2.0f);
                    float radius = getNumericProp(node.findProperty("״²ˆ״§״§"), 8.0f);
                    if (!node.findProperty("״²ˆ״§״§"))
                        radius = getNumericProp(node.findProperty("†״µ_‚״·״±"), 8.0f);

                    // ״×״£״«״± hover: ״±״¹ ״§„״¸„
                    if (isHovered && !isPressed)
                        elevation += 2.0f;
                    if (isPressed)
                        elevation = std::max(0.5f, elevation - 1.0f);

                    if (elevation > 0)
                    {
                        drawSoftShadow(rect, radius, elevation * 2.0f,
                                       0.0f, elevation * 0.6f, {0, 0, 0, 0.2f});
                    }

                    // ״±״³… ״®„״© ״§„״²״± (…״¹ ״¯״¹… ״§„״×״¯״±״¬)
                    const auto *bgColorProp = node.findProperty("„ˆ†_״®„״©");
                    if (!bgColorProp)
                        bgColorProp = node.findProperty("״®„״©");
                    Color btnColor = parseColorProp(bgColorProp, Color::fromNamed(NamedColor::Primary));

                    // ״×״£״«״± hover: ״¥״¶״§״¡״© ״§„„ˆ†
                    if (isHovered && !isPressed)
                    {
                        btnColor.r = std::min(1.0f, btnColor.r + 0.08f);
                        btnColor.g = std::min(1.0f, btnColor.g + 0.08f);
                        btnColor.b = std::min(1.0f, btnColor.b + 0.08f);
                    }
                    // ״×״£״«״± ״§„״¶״÷״·: ״×״¹״×… ״§„„ˆ†
                    if (isPressed)
                    {
                        btnColor.r = std::max(0.0f, btnColor.r - 0.1f);
                        btnColor.g = std::max(0.0f, btnColor.g - 0.1f);
                        btnColor.b = std::max(0.0f, btnColor.b - 0.1f);
                    }

                    const auto *gradProp = node.findProperty("״×״¯״±״¬");
                    const auto *gradEndProp = node.findProperty("״×״¯״±״¬_†‡״§״©");
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

                    // ״×״£״«״± ripple ״¹†״¯ ״§„״¶״÷״· (״¯״§״¦״±״© ״´״¨‡ ״´״§״©)
                    if (isPressed)
                    {
                        Color ripple = {1.0f, 1.0f, 1.0f, 0.15f};
                        drawRoundedRect(rect, ripple, radius);
                    }

                    // ״±״³… †״µ ״§„״²״±
                    const auto *textProp = node.findProperty("text");
                    if (!textProp)
                        textProp = node.findProperty("†״µ");
                    if (!textProp)
                        textProp = node.findProperty("…״­״×ˆ‰");
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            // „ˆ† †״µ ״§„״²״± ג€” ‚״±״£ …† „ˆ†_†״µ ״£ˆ ״£״¨״¶ ״§״×״±״§״¶״§‹
                            const auto *textColorProp = node.findProperty("„ˆ†_†״µ");
                            if (!textColorProp)
                                textColorProp = node.findProperty("„ˆ†_״§„†״µ");
                            Color textColor = parseColorProp(textColorProp, Color::fromNamed(NamedColor::White));

                            float btnFontSize = getNumericProp(node.findProperty("״­״¬…_״§„״®״·"), 16.0f);
                            if (btnFontSize <= 1.0f)
                                btnFontSize = getNumericProp(node.findProperty("״­״¬…_״®״·"), 16.0f);

                            // ״­״³״§״¨ …ˆ‚״¹ ״§„†״µ  ˆ״³״· ״§„״²״± (״¨״¯‚״© ״¹״¨״± TTF_SizeUTF8)
                            float textX = rect.x + rect.width / 2;
                            float textY = rect.y + rect.height / 2 - btnFontSize / 2;
#ifdef SAD_UI_HAS_SDL_TTF
                            TTF_Font *font = getFontForSize(btnFontSize);
                            if (!font)
                                font = defaultFont_;
                            if (font)
                            {
                                int textW = 0, textH = 0;
                                TTF_SizeUTF8(font, text->c_str(), &textW, &textH);
                                textX = rect.x + (rect.width - static_cast<float>(textW)) / 2.0f;
                                textY = rect.y + (rect.height - static_cast<float>(textH)) / 2.0f;
                            }
#endif
                            drawText(*text, textX, textY, textColor, btnFontSize);
                        }
                    }
                    break;
                }

                case UINodeType::Divider:
                {
                    Color divColor = parseColorProp(node.findProperty("„ˆ†"), Color::fromNamed(NamedColor::Grey));
                    float thickness = getNumericProp(node.findProperty("״³…״§ƒ״©"), rect.height);
                    LayoutRect divRect = {rect.x, rect.y, rect.width, thickness};
                    drawFilledRect(divRect, divColor);
                    break;
                }

                case UINodeType::Image:
                {
                    // ״×״­…„ ˆ״¹״±״¶ ״§„״µˆ״±״©
                    const auto *srcProp = node.findProperty("\xd9\x85\xd8\xb5\xd8\xaf\xd8\xb1"); // …״µ״¯״±
                    if (!srcProp)
                        srcProp = node.findProperty("src");
                    if (!srcProp)
                        srcProp = node.findProperty("\xd9\x85\xd8\xb3\xd8\xa7\xd8\xb1"); // …״³״§״±
                    bool loaded = false;
                    if (srcProp)
                    {
                        if (auto *path = std::get_if<std::string>(&srcProp->value))
                        {
                            // …״­״§ˆ„״© ״±״³… ״§„״µˆ״±״©
                            drawImage(*path, rect);
                            loaded = true;
                        }
                    }
                    if (!loaded)
                    {
                        // Placeholder ״¥״°״§ „״§ ˆ״¬״¯ …״µ״¯״±
                        Color placeholder = Color::fromNamed(NamedColor::LightGray);
                        float imgRadius = getNumericProp(node.findProperty("\xd8\xb2\xd9\x88\xd8\xa7\xd9\x8a\xd8\xa7"), 0.0f); // ״²ˆ״§״§
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

                    Color bgColor = parseColorProp(node.findProperty("\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9"), Color::fromNamed(NamedColor::White));
                    Color borderColor = parseColorProp(node.findProperty("\xd8\xad\xd8\xaf_\xd9\x84\xd9\x88\xd9\x86"), Color::fromNamed(NamedColor::Gray));
                    float radius = getNumericProp(node.findProperty("\xd8\xb2\xd9\x88\xd8\xa7\xd9\x8a\xd8\xa7"), 4.0f);
                    float fontSize = getNumericProp(node.findProperty("\xd8\xad\xd8\xac\xd9\x85_\xd8\xa7\xd9\x84\xd8\xae\xd8\xb7"), 14.0f);

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

                    const auto *valueProp = node.findProperty("\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9");
                    if (!valueProp)
                        valueProp = node.findProperty("\xd9\x86\xd8\xb5");
                    if (!valueProp)
                        valueProp = node.findProperty("value");
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
                        Color textColor = parseColorProp(node.findProperty("\xd9\x84\xd9\x88\xd9\x86_\xd8\xa7\xd9\x84\xd9\x86\xd8\xb5"), {0, 0, 0, 1});
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
                        const auto *hintProp = node.findProperty("\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad");
                        if (!hintProp)
                            hintProp = node.findProperty("placeholder");
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
                    // ״±״³… …״×״§״­ ״×״¨״¯„
                    bool isOn = getBoolProp(node.findProperty("…״¹„"), false);
                    Color activeColor = parseColorProp(node.findProperty("„ˆ†_†״´״·"), Color::fromNamed(NamedColor::Primary));
                    Color inactiveColor = parseColorProp(node.findProperty("„ˆ†"), Color::fromNamed(NamedColor::LightGray));
                    Color trackColor = isOn ? activeColor : inactiveColor;
                    // ״×״£״«״± hover
                    if (isHoveredT && !isPressedT)
                    {
                        trackColor.r = std::min(1.0f, trackColor.r + 0.06f);
                        trackColor.g = std::min(1.0f, trackColor.g + 0.06f);
                        trackColor.b = std::min(1.0f, trackColor.b + 0.06f);
                    }
                    LayoutRect track = {rect.x, rect.y + rect.height / 4, rect.width, rect.height / 2};
                    drawRoundedRect(track, trackColor, rect.height / 4);
                    // …‚״¨״¶ …״¹ ״¸„
                    Color knobColor = Color::fromNamed(NamedColor::White);
                    float knobSize = rect.height - 4;
                    float knobX = isOn ? rect.x + rect.width - knobSize - 2 : rect.x + 2;
                    LayoutRect knobShadow = {knobX - 1, rect.y + 3, knobSize + 2, knobSize + 2};
                    drawRoundedRect(knobShadow, {0, 0, 0, 0.15f}, (knobSize + 2) / 2);
                    LayoutRect knob = {knobX, rect.y + 2, knobSize, knobSize};
                    drawRoundedRect(knob, knobColor, knobSize / 2);
                    // †״µ ״§„״×״³…״©
                    const auto *labelProp = node.findProperty("†״µ");
                    if (!labelProp)
                        labelProp = node.findProperty("…״­״×ˆ‰");
                    if (labelProp)
                    {
                        if (auto *lbl = std::get_if<std::string>(&labelProp->value))
                        {
                            float fsize = getNumericProp(node.findProperty("״­״¬…_״§„״®״·"), 14.0f);
                            drawText(*lbl, rect.x + rect.width + 8, rect.y + rect.height / 2 - fsize / 2,
                                     {0, 0, 0, 1}, fsize);
                        }
                    }
                    break;
                }

                case UINodeType::Slider:
                {
                    // ״±״³… ״´״±״· …†״²„‚
                    float sliderVal = getNumericProp(node.findProperty("‚…״©"), 50.0f) / 100.0f;
                    sliderVal = std::max(0.0f, std::min(1.0f, sliderVal));
                    Color trackBg = parseColorProp(node.findProperty("״®„״©"), Color::fromNamed(NamedColor::LightGray));
                    Color trackFill = parseColorProp(node.findProperty("„ˆ†"), Color::fromNamed(NamedColor::Primary));
                    float trackY = rect.y + rect.height / 2 - 2;
                    LayoutRect track = {rect.x, trackY, rect.width, 4};
                    drawRoundedRect(track, trackBg, 2.0f);
                    // ״§„״¬״²״¡ ״§„……„ˆ״¡
                    float filledW = rect.width * sliderVal;
                    if (filledW > 0)
                    {
                        LayoutRect filled = {rect.x, trackY, filledW, 4};
                        drawRoundedRect(filled, trackFill, 2.0f);
                    }
                    // …‚״¨״¶
                    Color thumbColor = parseColorProp(node.findProperty("„ˆ†_״§„…‚״¨״¶"), trackFill);
                    float thumbX = rect.x + filledW;
                    LayoutRect thumb = {thumbX - 8, rect.y + rect.height / 2 - 8, 16, 16};
                    // ״¸„ ״§„…‚״¨״¶
                    LayoutRect thumbShadow = {thumbX - 9, rect.y + rect.height / 2 - 7, 18, 18};
                    drawRoundedRect(thumbShadow, {0, 0, 0, 0.15f}, 9.0f);
                    drawRoundedRect(thumb, thumbColor, 8.0f);
                    break;
                }

                case UINodeType::Checkbox:
                {
                    bool isHoveredCB = (hoveredNode_ == &node);
                    // ״±״³… …״±״¨״¹ ״§״®״×״§״±
                    bool checked = getBoolProp(node.findProperty("…״¹„"), false);
                    Color activeColor = parseColorProp(node.findProperty("„ˆ†"), Color::fromNamed(NamedColor::Primary));
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
                        drawText("ג“", rect.x + 3, rect.y + 1, Color::fromNamed(NamedColor::White), 15.0f);
                    }
                    else
                    {
                        LayoutRect inner = {rect.x + 1, rect.y + 1, 18, 18};
                        drawRoundedRect(inner, Color::fromNamed(NamedColor::White), 2.0f);
                    }
                    // †״µ
                    const auto *textProp = node.findProperty("text");
                    if (!textProp)
                        textProp = node.findProperty("†״µ");
                    if (!textProp)
                        textProp = node.findProperty("…״­״×ˆ‰");
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            Color textColor = parseColorProp(node.findProperty("„ˆ†_״§„†״µ"), {0, 0, 0, 1});
                            float fontSize = getNumericProp(node.findProperty("״­״¬…_״§„״®״·"), 14.0f);
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
                    // ״±״³… ״²״± ״±״§״¯ˆ (״¯״§״¦״±״©)
                    bool selected = getBoolProp(node.findProperty("\xd9\x85\xd9\x81\xd8\xb9\xd9\x84")); // …״¹„
                    bool isRadioHovered = (hoveredNode_ == &node);
                    Color activeColor = parseColorProp(node.findProperty("\xd9\x84\xd9\x88\xd9\x86"), Color::fromNamed(NamedColor::Primary));
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
                    // †״µ
                    const auto *textProp = node.findProperty("text");
                    if (!textProp)
                        textProp = node.findProperty("\xd9\x86\xd8\xb5"); // †״µ
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            Color textColor = parseColorProp(node.findProperty("\xd9\x84\xd9\x88\xd9\x86_\xd8\xa7\xd9\x84\xd9\x86\xd8\xb5"), {0, 0, 0, 1}); // „ˆ†_״§„†״µ
                            drawText(*text, rect.x + 28, rect.y + 2, textColor, 14.0f);
                        }
                    }
                    break;
                }

                case UINodeType::Card:
                {
                    bool isHovered = (hoveredNode_ == &node);
                    bool isPressed = (pressedNode_ == &node);

                    // ״¨״·״§‚״© …״¹ ״¸„ †״§״¹… ˆ״²ˆ״§״§ …״³״×״¯״±״© (Material Design 3)
                    float cardElevation = getNumericProp(node.findProperty("״±״¹"), 4.0f);
                    float cardRadius = getNumericProp(node.findProperty("״²ˆ״§״§"), 12.0f);

                    // ״×״£״«״±״§״× ״§„״×״§״¹„
                    if (isPressed)
                        cardElevation = std::max(1.0f, cardElevation - 2.0f);
                    else if (isHovered)
                        cardElevation += 4.0f;

                    // ״¸„ …״×״¹״¯״¯ ״§„״·״¨‚״§״× (3 ״·״¨‚״§״× „…״­״§ƒ״§״© Material elevation)
                    if (cardElevation > 0)
                    {
                        // ״·״¨‚״© 1: ״¸„ ƒ״¨״± ״® (ambient)
                        drawSoftShadow(rect, cardRadius, cardElevation * 3.5f,
                                       0.0f, cardElevation * 0.5f, {0, 0, 0, 0.10f});
                        // ״·״¨‚״© 2: ״¸„ …״×ˆ״³״· (umbra)
                        drawSoftShadow(rect, cardRadius, cardElevation * 2.0f,
                                       0.0f, cardElevation * 1.0f, {0, 0, 0, 0.18f});
                        // ״·״¨‚״© 3: ״¸„ ״­״§״¯ (penumbra)
                        drawSoftShadow(rect, cardRadius, cardElevation * 1.0f,
                                       0.0f, cardElevation * 0.4f, {0, 0, 0, 0.12f});
                    }

                    const auto *cardColorProp = node.findProperty("„ˆ†");
                    if (!cardColorProp)
                        cardColorProp = node.findProperty("״®„״©");
                    Color cardBg = parseColorProp(cardColorProp, Color::fromNamed(NamedColor::White));

                    // ״×״£״«״± hover: ״×״±״§ƒ״¨ ״´״§
                    if (isHovered && !isPressed)
                    {
                        cardBg.r = std::min(1.0f, cardBg.r + 0.03f);
                        cardBg.g = std::min(1.0f, cardBg.g + 0.03f);
                        cardBg.b = std::min(1.0f, cardBg.b + 0.03f);
                    }

                    // ״×״£״«״± pressed: ״×״¹״×…
                    if (isPressed)
                    {
                        cardBg.r *= 0.95f;
                        cardBg.g *= 0.95f;
                        cardBg.b *= 0.95f;
                    }

                    // ״¯״¹… ״§„״´״§״©
                    float opacity = getNumericProp(node.findProperty("״´״§״©"), 1.0f);
                    if (opacity < 1.0f)
                        cardBg.a *= opacity;

                    // ״¯״¹… ״§„״×״¯״±״¬  ״§„״¨״·״§‚״§״×
                    const auto *gradProp = node.findProperty("״×״¯״±״¬");
                    const auto *gradEndProp = node.findProperty("״×״¯״±״¬_†‡״§״©");
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

                    // ״­״¯ ״§„״¨״·״§‚״© (״¥† ˆ״¬״¯)
                    const auto *borderColorProp = node.findProperty("״­״¯_„ˆ†");
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
                    // ״´״±״· ״×‚״¯… …״¹ ״×״£״«״±״§״× HD ˆ״×״¯״±״¬ „ˆ†
                    float barRadius = getNumericProp(node.findProperty("״²ˆ״§״§"), rect.height / 2.0f);
                    Color trackColor = parseColorProp(node.findProperty("״®„״©"), {0.9f, 0.9f, 0.9f, 1.0f});
                    drawRoundedRect(rect, trackColor, barRadius);

                    // ‚״±״§״¡״© ״§„‚…״©
                    float progressValue = getNumericProp(node.findProperty("‚…״©"),
                                                         getNumericProp(node.findProperty("value"), 50.0f));
                    progressValue /= 100.0f;
                    progressValue = std::max(0.0f, std::min(1.0f, progressValue));

                    // „ˆ† ״§„״´״±״· (…״¹ ״¯״¹… ״×״¯״±״¬ ״×„‚״§״¦)
                    Color fillColor = parseColorProp(node.findProperty("„ˆ†"), Color::fromNamed(NamedColor::Primary));
                    float fillWidth = rect.width * progressValue;
                    if (fillWidth > 0)
                    {
                        LayoutRect fill = {rect.x, rect.y, fillWidth, rect.height};
                        // ״×״¯״±״¬ ״® „…״¸‡״± „״§…״¹
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
                    // ״´״±״· ״¨״­״«
                    Color bgColor = parseColorProp(node.findProperty("״®„״©"), Color::fromNamed(NamedColor::LightGray));
                    float radius = getNumericProp(node.findProperty("״²ˆ״§״§"), rect.height / 2);
                    drawRoundedRect(rect, bgColor, radius);
                    // ״£‚ˆ†״© ״¨״­״«
                    Color iconColor = parseColorProp(node.findProperty("„ˆ†_״§„״§‚ˆ†״©"), Color::fromNamed(NamedColor::Gray));
                    drawText("נ”", rect.x + 8, rect.y + rect.height / 2 - 8, iconColor, 14.0f);
                    // †״µ ״×„…״­
                    const auto *hintProp = node.findProperty("״×„…״­");
                    if (!hintProp)
                        hintProp = node.findProperty("placeholder");
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
                    // ״²״± ״¹״§״¦… ״¯״§״¦״± …״¹ ״¸„ †״§״¹… (Material Design)
                    Color fabColor = parseColorProp(node.findProperty("„ˆ†_״®„״©"), Color::fromNamed(NamedColor::Primary));
                    if (!node.findProperty("„ˆ†_״®„״©"))
                        fabColor = parseColorProp(node.findProperty("„ˆ†"), fabColor);
                    float radius = std::min(rect.width, rect.height) / 2;
                    // ״¸„ †״§״¹…
                    float elevation = getNumericProp(node.findProperty("״¸„"), 6.0f);
                    if (elevation > 0)
                    {
                        drawSoftShadow(rect, radius, elevation * 2.0f,
                                       0.0f, elevation * 0.7f, {0, 0, 0, 0.3f});
                    }
                    drawRoundedRect(rect, fabColor, radius);
                    const auto *textProp = node.findProperty("text");
                    if (!textProp)
                        textProp = node.findProperty("†״µ");
                    if (!textProp)
                        textProp = node.findProperty("״§‚ˆ†״©");
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            Color textColor = parseColorProp(node.findProperty("„ˆ†_״§„†״µ"), Color::fromNamed(NamedColor::White));
                            float fontSize = getNumericProp(node.findProperty("״­״¬…_״§„״®״·"), 20.0f);
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
                                drawText(*text, rect.x + rect.width / 2 - 8, rect.y + rect.height / 2 - 8, textColor, fontSize);
                        }
                    }
                    break;
                }

                case UINodeType::AppBar:
                {
                    // ״´״±״· ״×״·״¨‚ ״¹„ˆ …״¹ ״×״¯״±״¬ „ˆ† ˆ״¸„
                    Color barColor = parseColorProp(node.findProperty("„ˆ†_״®„״©"), Color::fromNamed(NamedColor::Primary));
                    if (!node.findProperty("„ˆ†_״®„״©"))
                        barColor = parseColorProp(node.findProperty("„ˆ†"), barColor);
                    // ״¸„ †״§״¹… ״£״³„ ״§„״´״±״·
                    float elevation = getNumericProp(node.findProperty("״¸„"), 3.0f);
                    if (elevation > 0)
                    {
                        drawSoftShadow(rect, 0.0f, elevation * 2.0f,
                                       0.0f, elevation, {0, 0, 0, 0.15f});
                    }
                    // ״®„״© ״¨״×״¯״±״¬ ״®
                    Color barLighter = {
                        std::min(1.0f, barColor.r * 1.15f),
                        std::min(1.0f, barColor.g * 1.15f),
                        std::min(1.0f, barColor.b * 1.15f),
                        barColor.a};
                    drawLinearGradient(rect, barLighter, barColor, true);
                    const auto *textProp = node.findProperty("text");
                    if (!textProp)
                        textProp = node.findProperty("״¹†ˆ״§†");
                    if (!textProp)
                        textProp = node.findProperty("†״µ");
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            Color textColor = parseColorProp(node.findProperty("„ˆ†_״§„†״µ"), Color::fromNamed(NamedColor::White));
                            float fontSize = getNumericProp(node.findProperty("״­״¬…_״§„״®״·"), 20.0f);
                            float textX = rect.x + 16;
                            // RTL: …״­״§״°״§״© „„…†
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
                    // ״´״§״±״© ״¥״´״¹״§״±
                    Color badgeColor = parseColorProp(node.findProperty("„ˆ†_״®„״©"), Color::fromNamed(NamedColor::Error));
                    if (!node.findProperty("„ˆ†_״®„״©"))
                        badgeColor = parseColorProp(node.findProperty("„ˆ†"), badgeColor);
                    float r = std::min(rect.width, rect.height) / 2;
                    drawRoundedRect(rect, badgeColor, r);
                    const auto *textProp = node.findProperty("text");
                    if (!textProp)
                        textProp = node.findProperty("†״µ");
                    if (!textProp)
                        textProp = node.findProperty("…״­״×ˆ‰");
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            Color textColor = parseColorProp(node.findProperty("„ˆ†_״§„†״µ"), Color::fromNamed(NamedColor::White));
                            float fontSize = getNumericProp(node.findProperty("״­״¬…_״§„״®״·"), 10.0f);
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
                    // ״±‚״§‚״©
                    Color chipBg = parseColorProp(node.findProperty("״®„״©"), Color::fromNamed(NamedColor::LightGray));
                    if (!node.findProperty("״®„״©"))
                        chipBg = parseColorProp(node.findProperty("„ˆ†"), chipBg);
                    float chipRadius = getNumericProp(node.findProperty("״²ˆ״§״§"), 16.0f);
                    drawRoundedRect(rect, chipBg, chipRadius);
                    // ״§„״­״¯
                    const auto *borderProp = node.findProperty("״­״¯_„ˆ†");
                    if (borderProp)
                    {
                        Color borderColor = parseColorProp(borderProp, Color::fromNamed(NamedColor::Gray));
                        drawRoundedRect(rect, borderColor, chipRadius);
                        LayoutRect inner = {rect.x + 1, rect.y + 1, rect.width - 2, rect.height - 2};
                        drawRoundedRect(inner, chipBg, std::max(0.0f, chipRadius - 1));
                    }
                    const auto *textProp = node.findProperty("text");
                    if (!textProp)
                        textProp = node.findProperty("†״µ");
                    if (!textProp)
                        textProp = node.findProperty("…״­״×ˆ‰");
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            Color textColor = parseColorProp(node.findProperty("„ˆ†_״§„†״µ"), {0, 0, 0, 1});
                            float fontSize = getNumericProp(node.findProperty("״­״¬…_״§„״®״·"), 13.0f);
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
                    // ״µˆ״±״© ״±…״²״© ״¯״§״¦״±״©
                    Color avatarBg = parseColorProp(node.findProperty("״®„״©"), Color::fromNamed(NamedColor::Primary));
                    if (!node.findProperty("״®„״©"))
                        avatarBg = parseColorProp(node.findProperty("„ˆ†"), avatarBg);
                    float r = std::min(rect.width, rect.height) / 2;
                    drawRoundedRect(rect, avatarBg, r);
                    const auto *textProp = node.findProperty("text");
                    if (!textProp)
                        textProp = node.findProperty("†״µ");
                    if (!textProp)
                        textProp = node.findProperty("״­״±");
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            Color textColor = parseColorProp(node.findProperty("„ˆ†_״§„†״µ"), Color::fromNamed(NamedColor::White));
                            float fontSize = getNumericProp(node.findProperty("״­״¬…_״§„״®״·"), 18.0f);
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
                    // ״´״±״· ״¥״´״¹״§״± ״£״³„ ״§„״´״§״´״©
                    Color snackBg = parseColorProp(node.findProperty("״®„״©"), {0.2f, 0.2f, 0.2f, 0.9f});
                    float radius = getNumericProp(node.findProperty("״²ˆ״§״§"), 4.0f);
                    drawRoundedRect(rect, snackBg, radius);
                    const auto *textProp = node.findProperty("text");
                    if (!textProp)
                        textProp = node.findProperty("†״µ");
                    if (!textProp)
                        textProp = node.findProperty("…״­״×ˆ‰");
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            Color textColor = parseColorProp(node.findProperty("„ˆ†_״§„†״µ"), Color::fromNamed(NamedColor::White));
                            float fontSize = getNumericProp(node.findProperty("״­״¬…_״§„״®״·"), 14.0f);
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
                    // ״§״µ„ ״´״§ ג€” ״³״×״®״¯… ‚״· „״­״¬״² …״³״§״­״©  ״§„״×״®״·״·
                    // …ƒ† ״±״³… ״®„״© ״§״®״×״§״±״© „„״×״µ״­״­
                    if (desktopOptions_.debugBounds)
                    {
                        Color spacerDebug = {0.0f, 0.8f, 0.0f, 0.1f};
                        drawFilledRect(rect, spacerDebug);
                    }
                    break;
                }

                case UINodeType::Icon:
                {
                    // ״£‚ˆ†״©
                    Color iconColor = parseColorProp(node.findProperty("„ˆ†"), {0, 0, 0, 1});
                    float fontSize = getNumericProp(node.findProperty("״­״¬…"), 24.0f);
                    const auto *textProp = node.findProperty("text");
                    if (!textProp)
                        textProp = node.findProperty("״§‚ˆ†״©");
                    if (!textProp)
                        textProp = node.findProperty("״±…״²");
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                            drawText(*text, rect.x, rect.y, iconColor, fontSize);
                    }
                    break;
                }

                case UINodeType::Drawer:
                {
                    // ג”€ג”€ג”€ ״¯״±״¬ ״¬״§†״¨ (Material Drawer) ג”€ג”€ג”€
                    // ״®„״© ״´״¨‡ ״´״§״© (overlay)
                    int vw = 0, vh = 0;
                    SDL_GetRendererOutputSize(sdlRenderer_, &vw, &vh);
                    LayoutRect overlay = {0, 0, static_cast<float>(vw), static_cast<float>(vh)};
                    drawFilledRect(overlay, {0, 0, 0, 0.4f});

                    // ״±״³… ״§„״¯״±״¬ †״³‡
                    float drawerW = getNumericProp(node.findProperty("\xd8\xb9\xd8\xb1\xd8\xb6"), 280.0f);
                    LayoutRect drawerRect = {0, 0, drawerW, static_cast<float>(vh)};
                    drawSoftShadow(drawerRect, 0.0f, 20.0f, 4.0f, 0.0f, {0, 0, 0, 0.3f});
                    Color drawerBg = parseColorProp(node.findProperty("\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9"), Color::fromNamed(NamedColor::White));
                    drawFilledRect(drawerRect, drawerBg);
                    break;
                }

                case UINodeType::BottomSheet:
                {
                    // ג”€ג”€ג”€ ˆ״±‚״© ״³„״© (Material Bottom Sheet) ג”€ג”€ג”€
                    int bsVW = 0, bsVH = 0;
                    SDL_GetRendererOutputSize(sdlRenderer_, &bsVW, &bsVH);
                    float sheetH = getNumericProp(node.findProperty("\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9"), static_cast<float>(bsVH) * 0.5f);
                    float sheetY = static_cast<float>(bsVH) - sheetH;

                    // ״®„״© ״´״¨‡ ״´״§״©
                    LayoutRect bsOverlay = {0, 0, static_cast<float>(bsVW), static_cast<float>(bsVH)};
                    drawFilledRect(bsOverlay, {0, 0, 0, 0.35f});

                    // ״§„ˆ״±‚״©
                    LayoutRect sheetRect = {0, sheetY, static_cast<float>(bsVW), sheetH};
                    drawSoftShadow(sheetRect, 20.0f, 16.0f, 0.0f, -4.0f, {0, 0, 0, 0.25f});
                    Color sheetBg = parseColorProp(node.findProperty("\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9"), Color::fromNamed(NamedColor::White));
                    drawRoundedRect({sheetRect.x, sheetRect.y, sheetRect.width, 24.0f}, sheetBg, 20.0f);
                    drawFilledRect({sheetRect.x, sheetRect.y + 12, sheetRect.width, sheetH - 12}, sheetBg);

                    // …‚״¨״¶ ״³״­״¨
                    float handleW = 40.0f;
                    LayoutRect handle = {sheetRect.x + sheetRect.width / 2 - handleW / 2, sheetRect.y + 10, handleW, 4.0f};
                    drawRoundedRect(handle, {0.7f, 0.7f, 0.7f, 1.0f}, 2.0f);
                    break;
                }

                case UINodeType::Dialog:
                case UINodeType::Alert:
                {
                    // ג”€ג”€ג”€ ״­ˆ״§״± / ״×†״¨‡ (Material Dialog) ג”€ג”€ג”€
                    int dlgVW = 0, dlgVH = 0;
                    SDL_GetRendererOutputSize(sdlRenderer_, &dlgVW, &dlgVH);

                    // ״®„״© ״´״¨‡ ״´״§״© (scrim)
                    LayoutRect dlgOverlay = {0, 0, static_cast<float>(dlgVW), static_cast<float>(dlgVH)};
                    drawFilledRect(dlgOverlay, {0, 0, 0, 0.5f});

                    // …ˆ‚״¹ ˆ…‚״§״³ ״§„״­ˆ״§״± (ˆ״³״· ״§„״´״§״´״©)
                    float dlgW = getNumericProp(node.findProperty("\xd8\xb9\xd8\xb1\xd8\xb6"), 350.0f);
                    float dlgH = getNumericProp(node.findProperty("\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9"), 220.0f);
                    float dlgX = (static_cast<float>(dlgVW) - dlgW) / 2.0f;
                    float dlgY = (static_cast<float>(dlgVH) - dlgH) / 2.0f;
                    LayoutRect dlgRect = {dlgX, dlgY, dlgW, dlgH};

                    // ״¸„ †״§״¹…
                    drawSoftShadow(dlgRect, 16.0f, 24.0f, 0.0f, 8.0f, {0, 0, 0, 0.3f});

                    // ״®„״© ״§„״­ˆ״§״±
                    Color dlgBg = parseColorProp(node.findProperty("\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9"), Color::fromNamed(NamedColor::White));
                    drawRoundedRect(dlgRect, dlgBg, 16.0f);

                    // ״§„״¹†ˆ״§† (״¥† ˆ״¬״¯)
                    const auto *titleProp = node.findProperty("\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86"); // ״¹†ˆ״§†
                    if (!titleProp)
                        titleProp = node.findProperty("title");
                    if (titleProp)
                    {
                        if (auto *t = std::get_if<std::string>(&titleProp->value))
                        {
                            drawText(*t, dlgX + 24, dlgY + 20, Color::fromNamed(NamedColor::OnSurface), 20.0f);
                        }
                    }

                    // ״§„…״­״×ˆ‰ (״¥† ˆ״¬״¯)
                    const auto *contentProp = node.findProperty("\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89"); // …״­״×ˆ‰
                    if (!contentProp)
                        contentProp = node.findProperty("content");
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
                    // ״­״§ˆ״§״× ג€” ״®„״© ״§״®״×״§״±״©״ ״§„״£״¨†״§״¡ ״±״³…ˆ† ״¨״§„״×ƒ״±״§״±
                    const auto *containerBgProp = node.findProperty("\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9"); // ״®„״©
                    if (!containerBgProp)
                        containerBgProp = node.findProperty("\xd9\x84\xd9\x88\xd9\x86_\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9"); // „ˆ†_״®„״©
                    if (containerBgProp)
                    {
                        Color containerBg = parseColorProp(containerBgProp, {0, 0, 0, 0});
                        if (containerBg.a > 0)
                        {
                            float containerRadius = getNumericProp(node.findProperty("\xd8\xb2\xd9\x88\xd8\xa7\xd9\x8a\xd8\xa7"), 0.0f); // ״²ˆ״§״§
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
                    drawText("ג–¼", rect.x + rect.width - 20, rect.y + 4, borderColor, 14.0f);
                    break;
                }

                case UINodeType::TabView:
                {
                    // ג”€ג”€ג”€ ״¹״±״¶ ״£‚״³״§… (TabView) ג€” ״±״₪ˆ״³ ״§„״£‚״³״§… + ״§„…״­״×ˆ‰ ג”€ג”€ג”€
                    Color tabBg = parseColorProp(node.findProperty("\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9"), Color::fromNamed(NamedColor::Primary)); // ״®„״©
                    float tabH = 48.0f;
                    LayoutRect tabBarRect = {rect.x, rect.y, rect.width, tabH};
                    drawFilledRect(tabBarRect, tabBg);

                    // ״±״³… ״£„״³†״© ״§„״×״¨ˆ״¨ …† ״£״³…״§״¡ ״§„״£״¨†״§״¡
                    size_t tabCount = node.childCount();
                    int selectedTab = static_cast<int>(getNumericProp(node.findProperty("\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf"), 0.0f)); // …״­״¯״¯
                    if (tabCount > 0)
                    {
                        float tabW = rect.width / static_cast<float>(tabCount);
                        for (size_t t = 0; t < tabCount; ++t)
                        {
                            float tx = rect.x + t * tabW;
                            // ״§״³… ״§„‚״³…
                            const auto &tabChild = node.getChildren()[t];
                            const auto *tabLabel = tabChild->findProperty("\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86"); // ״¹†ˆ״§†
                            if (!tabLabel)
                                tabLabel = tabChild->findProperty("\xd9\x86\xd8\xb5"); // †״µ
                            if (!tabLabel)
                                tabLabel = tabChild->findProperty("title");
                            std::string label = "‚״³… " + std::to_string(t + 1);
                            if (tabLabel)
                            {
                                if (auto *s = std::get_if<std::string>(&tabLabel->value))
                                    label = *s;
                            }
                            Color textCol = (static_cast<int>(t) == selectedTab) ? Color{1, 1, 1, 1} : Color{1, 1, 1, 0.6f};
                            drawText(label, tx + tabW / 2 - 20, rect.y + 14, textCol, 14.0f);

                            // ״®״· ״³„ „„‚״³… ״§„…״­״¯״¯
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
                    // ״×״®״·״·״§״× ג€” ״§„״£״¨†״§״¡ ״±״³…ˆ† ״¨״§„״×ƒ״±״§״±
                    // ״®„״© ״§״®״×״§״±״©
                    const auto *layoutBgProp = node.findProperty("\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9"); // ״®„״©
                    if (!layoutBgProp)
                        layoutBgProp = node.findProperty("\xd9\x84\xd9\x88\xd9\x86_\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9"); // „ˆ†_״®„״©
                    if (layoutBgProp)
                    {
                        Color bg = parseColorProp(layoutBgProp, {0, 0, 0, 0});
                        if (bg.a > 0)
                            drawFilledRect(rect, bg);
                    }
                    // ‚״µ ״§„…״­״×ˆ‰ „„״¹†״§״µ״± ״§„‚״§״¨„״© „„״×…״±״±
                    if (node.getType() == UINodeType::ScrollView ||
                        node.getType() == UINodeType::LazyColumn ||
                        node.getType() == UINodeType::LazyRow ||
                        node.getType() == UINodeType::List)
                    {
                        setClipRect(&rect);
                    }
                    break;
                }

                    // ג”€ג”€ ֲ§11 …ƒˆ†״§״× ״§„״¨״§†״§״× (Data Components ג€” v3) ג”€ג”€

                case UINodeType::Tooltip:
                {
                    // ״×„…״­ ״¸‡״± ״¹†״¯ ״§„״×…״±״±
                    bool isTooltipHovered = (hoveredNode_ == &node);
                    if (isTooltipHovered)
                    {
                        const auto *tipProp = node.findProperty("†״µ");
                        if (!tipProp)
                            tipProp = node.findProperty("text");
                        if (!tipProp)
                            tipProp = node.findProperty("…״­״×ˆ‰");
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
                    // ״±״³… ״®״·ˆ״· ״§„״´״¨ƒ״©
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
                    // …״³״§״± ״§„״×†‚„: ״¹†״µ״±1 > ״¹†״µ״±2 > ״¹†״µ״±3
                    Color crumbColor = parseColorProp(node.findProperty("„ˆ†"), Color::fromNamed(NamedColor::Blue));
                    Color sepColor = Color::fromNamed(NamedColor::Gray);
                    float crumbFontSize = getNumericProp(node.findProperty("״­״¬…_״§„״®״·"), 14.0f);
                    float xOff = rect.x + 4;
                    for (size_t ci = 0; ci < node.childCount(); ++ci)
                    {
                        if (ci > 0)
                        {
                            drawText(" ג€÷ ", xOff, rect.y + 4, sepColor, crumbFontSize);
                            xOff += crumbFontSize * 1.2f;
                        }
                        const auto *childText = node.getChildren()[ci]->findProperty("†״µ");
                        if (!childText)
                            childText = node.getChildren()[ci]->findProperty("text");
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
                    // ״´״±״· ״×†‚„ ״§„״µ״­״§״×:  ֲ« 1 2 3 4 ֲ»
                    float currentPage = getNumericProp(node.findProperty("״µ״­״©"), 1.0f);
                    float totalPages = getNumericProp(node.findProperty("״¥״¬…״§„"), 5.0f);
                    Color activeColor = parseColorProp(node.findProperty("„ˆ†"), Color::fromNamed(NamedColor::Primary));
                    Color inactiveColor = Color::fromNamed(NamedColor::LightGray);
                    float btnSize = 28.0f, gap = 4.0f;
                    float startX = rect.x + (rect.width - (totalPages * (btnSize + gap))) / 2;
                    // ״²״± ״§„״³״§״¨‚
                    drawText("ֲ«", startX - btnSize - gap, rect.y + 4, activeColor, 16.0f);
                    for (int p = 1; p <= static_cast<int>(totalPages); ++p)
                    {
                        float px = startX + (p - 1) * (btnSize + gap);
                        LayoutRect pageRect = {px, rect.y, btnSize, btnSize};
                        bool isCurrent = (p == static_cast<int>(currentPage));
                        drawRoundedRect(pageRect, isCurrent ? activeColor : inactiveColor, 4.0f);
                        Color txtColor = isCurrent ? Color::fromNamed(NamedColor::White) : Color::fromNamed(NamedColor::Black);
                        drawText(std::to_string(p), px + btnSize / 2 - 4, rect.y + 6, txtColor, 14.0f);
                    }
                    // ״²״± ״§„״×״§„
                    float endX = startX + totalPages * (btnSize + gap);
                    drawText("ֲ»", endX, rect.y + 4, activeColor, 16.0f);
                    break;
                }

                case UINodeType::Timeline:
                {
                    Color lineColor = Color::fromNamed(NamedColor::Blue);
                    // ״®״· ״¹…ˆ״¯
                    LayoutRect line = {rect.x + 6.0f, rect.y, 2.0f, rect.height};
                    drawFilledRect(line, lineColor);
                    break;
                }

                    // ג”€ג”€ ֲ§12 …ƒˆ†״§״× ״§„ˆ״³״§״¦״· (Media Components ג€” v3) ג”€ג”€

                case UINodeType::Carousel:
                {
                    Color bgColor = {0.95f, 0.95f, 0.95f, 1.0f};
                    drawFilledRect(rect, bgColor);
                    drawRoundedRect(rect, Color::fromNamed(NamedColor::Gray), 8.0f);
                    // ״£״³‡… ״§„״×†‚„
                    drawText("ג—€", rect.x + 4, rect.y + rect.height / 2 - 10, Color::fromNamed(NamedColor::Gray), 20.0f);
                    drawText("ג–¶", rect.x + rect.width - 20, rect.y + rect.height / 2 - 10, Color::fromNamed(NamedColor::Gray), 20.0f);
                    break;
                }

                case UINodeType::RichText:
                case UINodeType::Markdown:
                {
                    const auto *textProp = node.findProperty("text");
                    if (!textProp)
                        textProp = node.findProperty("†״µ");
                    if (!textProp)
                        textProp = node.findProperty("…״­״×ˆ‰");
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                        {
                            Color textColor = parseColorProp(node.findProperty("„ˆ†"), Color::fromNamed(NamedColor::Black));
                            float fontSize = getNumericProp(node.findProperty("״­״¬…_״§„״®״·"), 14.0f);
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
                    bool darkThemeCB = getBoolProp(node.findProperty("\xd8\xaf\xd8\xa7\xd9\x83\xd9\x86"), true);
                    float fontSizeCB = getNumericProp(node.findProperty("\xd8\xad\xd8\xac\xd9\x85_\xd8\xa7\xd9\x84\xd8\xae\xd8\xb7"), 13.0f);
                    bool showLineNums = getBoolProp(node.findProperty("\xd8\xa3\xd8\xb1\xd9\x82\xd8\xa7\xd9\x85_\xd8\xa3\xd8\xb3\xd8\xb7\xd8\xb1"), true);

                    // خلفية الكتلة
                    Color codeBg = darkThemeCB ? Color{0.12f, 0.12f, 0.14f, 1.0f}
                                               : Color{0.96f, 0.96f, 0.96f, 1.0f};
                    drawRoundedRect(rect, codeBg, 6.0f);

                    const auto *textProp = node.findProperty("\xd9\x86\xd8\xb5");
                    if (!textProp)
                        textProp = node.findProperty("text");
                    if (!textProp)
                        textProp = node.findProperty("\xd9\x83\xd9\x88\xd8\xaf");
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
                    drawText("ג–¶", rect.x + 8, rect.y + 6, Color::fromNamed(NamedColor::Blue), 18.0f);
                    // ״´״±״· ״§„״×‚״¯…
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

                    // ג”€ג”€ ֲ§13 …ƒˆ†״§״× ״§„״­״±ƒ״© (Animation Components ג€” v3) ג”€ג”€

                case UINodeType::Skeleton:
                {
                    // ‡ƒ„ ״×״­…„ …״¹ ״×״¯״±״¬ „״§…״¹ (shimmer-like)
                    Color placeholderDark = {0.82f, 0.82f, 0.82f, 1.0f};
                    Color placeholderLight = {0.92f, 0.92f, 0.92f, 1.0f};
                    drawLinearGradient(rect, placeholderDark, placeholderLight, false, 4.0f);
                    break;
                }

                case UINodeType::Shimmer:
                {
                    // ״×״£״«״± ˆ…״¶ …״¹ ״×״¯״±״¬
                    Color shimmerDark = {0.88f, 0.88f, 0.88f, 1.0f};
                    Color shimmerLight = {0.96f, 0.96f, 0.96f, 1.0f};
                    drawLinearGradient(rect, shimmerDark, shimmerLight, false);
                    break;
                }

                case UINodeType::AnimatedList:
                case UINodeType::ForEach:
                {
                    // ‚ˆ״§״¦… ״¯†״§…ƒ״© ג€” ״±״³… ״®„״© ״®״© + ״§„״£״¨†״§״¡ ״±״³…ˆ† ״¨״§„״×ƒ״±״§״± ״£״¯†״§‡
                    Color listBg = parseColorProp(node.findProperty("״®„״©"), {0.98f, 0.98f, 0.98f, 1.0f});
                    drawFilledRect(rect, listBg);
                    break;
                }

                case UINodeType::Expandable:
                case UINodeType::Collapsible:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string title = textProp ? (std::get_if<std::string>(&textProp->value) ? *std::get_if<std::string>(&textProp->value) : "ג–¶ ״¹†״µ״±") : "ג–¶ ״¹†״µ״±";
                    drawText(title, rect.x + 4, rect.y + 4, Color::fromNamed(NamedColor::Black), 14.0f);
                    break;
                }

                case UINodeType::Swipeable:
                {
                    // ״§„״£״¨†״§״¡ ״±״³…ˆ† ״¨״§„״×ƒ״±״§״±
                    break;
                }

                    // ג”€ג”€ ֲ§14 …ƒˆ†״§״× ״§„״×״­ƒ… ״§„…״×‚״¯…״© (Advanced Controls ג€” v3) ג”€ג”€

                case UINodeType::Stepper:
                {
                    Color bgColor = Color::fromNamed(NamedColor::White);
                    Color borderColor = Color::fromNamed(NamedColor::Gray);
                    drawFilledRect(rect, bgColor);
                    drawRoundedRect(rect, borderColor, 4.0f);
                    drawText("גˆ’", rect.x + 4, rect.y + 2, Color::fromNamed(NamedColor::Black), 18.0f);
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
                    // ״£ˆ„ ״´״±״­״© …״­״¯״¯״©
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
                    Color starColor = {1.0f, 0.84f, 0.0f, 1.0f}; // ״°‡״¨
                    Color emptyColor = Color::fromNamed(NamedColor::Gray);
                    for (int s = 0; s < 5; s++)
                    {
                        Color c = s < 3 ? starColor : emptyColor;
                        drawText("ג˜…", rect.x + s * 22.0f, rect.y, c, 20.0f);
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
                    drawText("נ“…", rect.x + 4, rect.y + 4, Color::fromNamed(NamedColor::Black), 14.0f);
                    break;
                }

                case UINodeType::ColorPicker:
                case UINodeType::ColorWheel:
                {
                    // ״· „ˆ† ״¨״§„״×״¯״±״¬ (״£״­…״± ג†’ ״£״®״¶״± ג†’ ״£״²״±‚)
                    float third = rect.width / 3.0f;
                    LayoutRect r1 = {rect.x, rect.y, third + 1, rect.height};
                    LayoutRect r2 = {rect.x + third, rect.y, third + 1, rect.height};
                    LayoutRect r3 = {rect.x + 2 * third, rect.y, third, rect.height};
                    drawLinearGradient(r1, Color::fromNamed(NamedColor::Red), Color::fromNamed(NamedColor::Green), false);
                    drawLinearGradient(r2, Color::fromNamed(NamedColor::Green), Color::fromNamed(NamedColor::Blue), false);
                    drawLinearGradient(r3, Color::fromNamed(NamedColor::Blue), {1.0f, 0.0f, 1.0f, 1.0f}, false);
                    drawRoundedRect(rect, {0, 0, 0, 0}, 4.0f); // ״­״¯ˆ״¯ ‚״·
                    break;
                }

                    // ג”€ג”€ ֲ§15 …ƒˆ†״§״× ״§„״×†‚„ ״§„…״×‚״¯…״© (Extended Navigation ג€” v3) ג”€ג”€

                case UINodeType::BottomNav:
                {
                    Color bgColor = Color::fromNamed(NamedColor::White);
                    drawFilledRect(rect, bgColor);
                    // ״®״· ״¹„ˆ
                    LayoutRect topLine = {rect.x, rect.y, rect.width, 1.0f};
                    drawFilledRect(topLine, Color::fromNamed(NamedColor::Gray));
                    break;
                }

                case UINodeType::SideNav:
                {
                    Color bgColor = {0.95f, 0.95f, 0.97f, 1.0f};
                    drawFilledRect(rect, bgColor);
                    // ״®״· ״¬״§†״¨
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
                    // ״®״· ״§״µ„  ״§„…†״×״µ
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

                    // ג”€ג”€ ״£†ˆ״§״¹ v1-v2 …‚ˆ״¯״© ג”€ג”€

                    // ─── TextArea handled above with TextField ───

                case UINodeType::LazyGrid:
                {
                    // ״´״¨ƒ״© ƒ״³ˆ„״© ג€” ״®„״© + ״¥״·״§״± + ״§„״£״¨†״§״¡ ״¹״±״¶ˆ† ״¨״§„״×ƒ״±״§״± ״£״¯†״§‡
                    Color gridBg = parseColorProp(node.findProperty("״®„״©"), {0.97f, 0.97f, 0.97f, 1.0f});
                    drawFilledRect(rect, gridBg);
                    // ״±״³… ״®״·ˆ״· ״´״¨ƒ״© ״®״©
                    float cellSize = getNumericProp(node.findProperty("״­״¬…_״®„״©"), 80.0f);
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
                    drawText("ג†’", rect.x + 4, rect.y + 4, linkColor, 14.0f);
                    break;
                }

                case UINodeType::TabItem:
                case UINodeType::Section:
                {
                    // ‚״³… ג€” ״¹†ˆ״§† + …״­״×ˆ‰
                    const auto *sectionTitle = node.findProperty("״¹†ˆ״§†");
                    if (!sectionTitle)
                        sectionTitle = node.findProperty("†״µ");
                    if (sectionTitle)
                    {
                        if (auto *t = std::get_if<std::string>(&sectionTitle->value))
                        {
                            Color titleColor = parseColorProp(node.findProperty("„ˆ†"), {0.3f, 0.3f, 0.3f, 1.0f});
                            drawText(*t, rect.x + 4, rect.y + 2, titleColor, 12.0f);
                            // ״®״· ״§״µ„ ״×״­״× ״§„״¹†ˆ״§†
                            LayoutRect sep = {rect.x, rect.y + 18, rect.width, 1.0f};
                            drawFilledRect(sep, {0.85f, 0.85f, 0.85f, 1.0f});
                        }
                    }
                    // ״§„״£״¨†״§״¡ ״¹״±״¶ˆ† ״¨״§„״×ƒ״±״§״± ״£״¯†״§‡
                    break;
                }

                case UINodeType::Sheet:
                {
                    // ״¸„ †״§״¹… „„ˆ״±‚״© ״§„…†״¨״«‚״©
                    drawSoftShadow(rect, 12.0f, 16.0f, 0.0f, -4.0f, {0, 0, 0, 0.2f});
                    Color bgColor = Color::fromNamed(NamedColor::White);
                    drawFilledRect(rect, bgColor);
                    drawRoundedRect(rect, Color::fromNamed(NamedColor::Gray), 12.0f);
                    // …‚״¨״¶ ״§„ˆ״±‚״©
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
                    drawText("נ—÷ן¸", rect.x + rect.width / 2 - 10, rect.y + rect.height / 2 - 10, Color::fromNamed(NamedColor::Black), 20.0f);
                    break;
                }

                case UINodeType::WebView:
                {
                    Color bgColor = Color::fromNamed(NamedColor::White);
                    drawFilledRect(rect, bgColor);
                    drawRoundedRect(rect, Color::fromNamed(NamedColor::Gray), 2.0f);
                    drawText("נ", rect.x + 4, rect.y + 4, Color::fromNamed(NamedColor::Blue), 16.0f);
                    break;
                }

                case UINodeType::VideoPlayer:
                {
                    Color bgColor = Color::fromNamed(NamedColor::Black);
                    drawFilledRect(rect, bgColor);
                    // ״²״± ״×״´״÷„ ˆ״³״·
                    drawText("ג–¶", rect.x + rect.width / 2 - 12, rect.y + rect.height / 2 - 12, Color::fromNamed(NamedColor::White), 24.0f);
                    break;
                }

                case UINodeType::Canvas:
                {
                    // …†״·‚״© ״±״³… …״®״µ״µ״© ג€” ״®„״© + ״¥״·״§״±
                    Color canvasBg = Color::fromNamed(NamedColor::White);
                    drawFilledRect(rect, canvasBg);
                    drawRoundedRect(rect, Color::fromNamed(NamedColor::Gray), 1.0f);
                    // ״§„״£״¨†״§״¡ (״£ˆ״§…״± ״§„״±״³…) ״¹״±״¶ˆ† ״¨״§„״×ƒ״±״§״± ״£״¯†״§‡
                    break;
                }

                case UINodeType::CustomWidget:
                case UINodeType::Conditional:
                {
                    // ״¹†״§״µ״± …״®״µ״µ״© ג€” ״§„״£״¨†״§״¡ ״¹״±״¶ˆ† ״¨״§„״×ƒ״±״§״±
                    break;
                }

                default:
                    break;
                }

                // 3. ״±״³… ״­״¯ˆ״¯ ״§„״×״µ״­״­ (״¥״°״§ …״¹‘„)
                if (desktopOptions_.debugBounds)
                {
                    Color debugColor = {1.0f, 0.0f, 0.0f, 0.3f}; // ״£״­…״± ״´״§
                    drawFilledRect(rect, debugColor);
                }

                // 4. ״±״³… ״§„״£״¨†״§״¡ (״×ƒ״±״§״±) ג€” …״¹ ״¯״¹… ״§„״×…״±״±
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

                // 5. ״¥״¹״§״¯״© ״×״¹† ״§„‚״µ ˆ״§„״×…״±״± ״¨״¹״¯ ״§„״¹†״§״µ״± ״§„‚״§״¨„״© „„״×…״±״±
                if (isScrollable)
                {
                    scrollTranslateY_ = prevScrollTranslateY;
                    setClipRect(nullptr);

                    // 5.1 ״±״³… ״´״±״· ״§„״×…״±״± (scrollbar)
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