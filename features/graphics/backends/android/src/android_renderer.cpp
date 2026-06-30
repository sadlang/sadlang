/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: android_renderer.cpp
 * المسار: features/graphics/backends/android/src/android_renderer.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ مُصيّر أندرويد — يرسم مباشرة عبر OpenGL ES 3.0
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/android/android_renderer.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

// stb_truetype و stb_image — مكتبات header-only لتحميل الخطوط والصور
#ifdef __ANDROID__
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#include "stb_image.h"

// بيانات الخط المحلية
static stbtt_fontinfo s_fontInfo;
static bool s_fontInfoValid = false;
#endif

namespace sad
{
    namespace ui
    {
        namespace android
        {

            // ═══════════════════════════════════════════════════════════════════════════════
            // شيدرات OpenGL ES  (Vertex + Fragment)
            // ═══════════════════════════════════════════════════════════════════════════════

#ifdef __ANDROID__

            static const char *kRectVertexShader = R"(#version 300 es
layout(location = 0) in vec2 aPos;
uniform vec2 uResolution;
uniform vec2 uPosition;
uniform vec2 uSize;
uniform float uRotation;   // زاوية الدوران بالراديان
uniform vec2 uRotCenter;   // مركز الدوران بالبكسل
void main() {
    vec2 pixel = uPosition + aPos * uSize;
    // تطبيق الدوران حول المركز
    if (uRotation != 0.0) {
        float c = cos(uRotation);
        float s = sin(uRotation);
        vec2 d = pixel - uRotCenter;
        pixel = uRotCenter + vec2(d.x * c - d.y * s, d.x * s + d.y * c);
    }
    vec2 ndc = (pixel / uResolution) * 2.0 - 1.0;
    ndc.y = -ndc.y;
    gl_Position = vec4(ndc, 0.0, 1.0);
}
)";

            static const char *kRectFragmentShader = R"(#version 300 es
precision mediump float;
uniform vec4 uColor;
uniform vec2 uSize;
uniform float uRadius;
out vec4 fragColor;
void main() {
    fragColor = uColor;
}
)";

            // شيدر رسم النصوص والصور (textured quad)
            static const char *kTexVertexShader = R"(#version 300 es
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;
uniform vec2 uResolution;
uniform vec2 uPosition;
uniform vec2 uSize;
uniform float uRotation;   // زاوية الدوران بالراديان
uniform vec2 uRotCenter;   // مركز الدوران بالبكسل
out vec2 vTexCoord;
void main() {
    vec2 pixel = uPosition + aPos * uSize;
    // تطبيق الدوران حول المركز
    if (uRotation != 0.0) {
        float c = cos(uRotation);
        float s = sin(uRotation);
        vec2 d = pixel - uRotCenter;
        pixel = uRotCenter + vec2(d.x * c - d.y * s, d.x * s + d.y * c);
    }
    vec2 ndc = (pixel / uResolution) * 2.0 - 1.0;
    ndc.y = -ndc.y;
    gl_Position = vec4(ndc, 0.0, 1.0);
    vTexCoord = aTexCoord;
}
)";

            static const char *kTexFragmentShader = R"(#version 300 es
precision mediump float;
uniform sampler2D uTexture;
uniform vec4 uColor;
in vec2 vTexCoord;
out vec4 fragColor;
void main() {
    vec4 texel = texture(uTexture, vTexCoord);
    fragColor = texel * uColor;
}
)";

            static const char *kRoundedRectFragmentShader = R"(#version 300 es
precision mediump float;
uniform vec4 uColor;
uniform vec2 uSize;
uniform float uRadius;
in vec2 vLocalPos; // relative position within the rect [0..1]
out vec4 fragColor;
void main() {
    vec2 halfSize = uSize * 0.5;
    vec2 pos = vLocalPos * uSize;
    vec2 d = abs(pos - halfSize) - (halfSize - uRadius);
    float dist = length(max(d, 0.0)) - uRadius;
    float alpha = 1.0 - smoothstep(-1.0, 1.0, dist);
    fragColor = vec4(uColor.rgb, uColor.a * alpha);
}
)";

            static GLuint compileShader(GLenum type, const char *source)
            {
                GLuint shader = glCreateShader(type);
                glShaderSource(shader, 1, &source, nullptr);
                glCompileShader(shader);
                GLint success;
                glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    char log[512];
                    glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
                    // Not calling std::cerr on Android; use __android_log_print in production
                }
                return shader;
            }

            static GLuint createProgram(const char *vertSrc, const char *fragSrc)
            {
                GLuint vert = compileShader(GL_VERTEX_SHADER, vertSrc);
                GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);
                GLuint prog = glCreateProgram();
                glAttachShader(prog, vert);
                glAttachShader(prog, frag);
                glLinkProgram(prog);
                glDeleteShader(vert);
                glDeleteShader(frag);
                return prog;
            }

#endif // __ANDROID__

            // ═══════════════════════════════════════════════════════════════════════════════
            // AndroidRenderer
            // ═══════════════════════════════════════════════════════════════════════════════

            AndroidRenderer::AndroidRenderer() = default;

            AndroidRenderer::~AndroidRenderer()
            {
                shutdown();
            }

            bool AndroidRenderer::initialize(void *nativeSurface,
                                             const PlatformRenderOptions &options)
            {
                options_ = options;

#ifdef __ANDROID__
                nativeWindow_ = static_cast<ANativeWindow *>(nativeSurface);
                if (!nativeWindow_)
                    return false;

                if (!initEGL(nativeWindow_))
                    return false;
                if (!initShaders())
                    return false;

                surfaceWidth_ = ANativeWindow_getWidth(nativeWindow_);
                surfaceHeight_ = ANativeWindow_getHeight(nativeWindow_);

                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                initialized_ = true;
                return true;
#else
                (void)nativeSurface;
                initialized_ = true;
                return true;
#endif
            }

            void AndroidRenderer::shutdown()
            {
#ifdef __ANDROID__
                // تنظيف ذاكرة الصور المؤقتة
                for (auto &[path, tex] : imageCache_)
                {
                    if (tex)
                        glDeleteTextures(1, &tex);
                }
                imageCache_.clear();

                if (texProgram_)
                {
                    glDeleteProgram(texProgram_);
                    texProgram_ = 0;
                }
                if (rectProgram_)
                {
                    glDeleteProgram(rectProgram_);
                    rectProgram_ = 0;
                }
                destroyEGL();
#endif
                fontLoaded_ = false;
                fontData_.clear();
                initialized_ = false;
            }

            bool AndroidRenderer::isReady() const { return initialized_; }

            bool AndroidRenderer::loadFont(const std::string &fontPath, float fontSize)
            {
#ifdef __ANDROID__
                // تحميل ملف الخط
                FILE *f = fopen(fontPath.c_str(), "rb");
                if (!f)
                    return false;
                fseek(f, 0, SEEK_END);
                long size = ftell(f);
                fseek(f, 0, SEEK_SET);
                fontData_.resize(static_cast<size_t>(size));
                fread(fontData_.data(), 1, static_cast<size_t>(size), f);
                fclose(f);

                // تهيئة stb_truetype
                if (!stbtt_InitFont(&s_fontInfo, fontData_.data(), 0))
                {
                    fontData_.clear();
                    return false;
                }
                s_fontInfoValid = true;
                fontSizePixels_ = fontSize;
                fontLoaded_ = true;
                return true;
#else
                (void)fontPath;
                (void)fontSize;
                return true;
#endif
            }

#ifdef __ANDROID__
            bool AndroidRenderer::initEGL(ANativeWindow *window)
            {
                eglDisplay_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
                eglInitialize(eglDisplay_, nullptr, nullptr);

                const EGLint attribs[] = {
                    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                    EGL_BLUE_SIZE, 8,
                    EGL_GREEN_SIZE, 8,
                    EGL_RED_SIZE, 8,
                    EGL_ALPHA_SIZE, 8,
                    EGL_DEPTH_SIZE, 0,
                    EGL_STENCIL_SIZE, 0,
                    EGL_NONE};

                EGLConfig config;
                EGLint numConfigs;
                eglChooseConfig(eglDisplay_, attribs, &config, 1, &numConfigs);

                const EGLint contextAttribs[] = {
                    EGL_CONTEXT_CLIENT_VERSION, 3,
                    EGL_NONE};

                eglSurface_ = eglCreateWindowSurface(eglDisplay_, config, window, nullptr);
                eglContext_ = eglCreateContext(eglDisplay_, config, EGL_NO_CONTEXT, contextAttribs);
                eglMakeCurrent(eglDisplay_, eglSurface_, eglSurface_, eglContext_);
                return true;
            }

            bool AndroidRenderer::initShaders()
            {
                rectProgram_ = createProgram(kRectVertexShader, kRectFragmentShader);
                rectColorLoc_ = glGetUniformLocation(rectProgram_, "uColor");
                rectPosLoc_ = glGetUniformLocation(rectProgram_, "uPosition");
                rectSizeLoc_ = glGetUniformLocation(rectProgram_, "uSize");
                rectResolutionLoc_ = glGetUniformLocation(rectProgram_, "uResolution");
                rectRadiusLoc_ = glGetUniformLocation(rectProgram_, "uRadius");
                rectRotationLoc_ = glGetUniformLocation(rectProgram_, "uRotation");
                rectRotCenterLoc_ = glGetUniformLocation(rectProgram_, "uRotCenter");

                if (!initTextureShader())
                    return false;

                return rectProgram_ != 0;
            }

            bool AndroidRenderer::initTextureShader()
            {
                texProgram_ = createProgram(kTexVertexShader, kTexFragmentShader);
                if (!texProgram_)
                    return false;

                texPosLoc_ = glGetUniformLocation(texProgram_, "uPosition");
                texSizeLoc_ = glGetUniformLocation(texProgram_, "uSize");
                texResolutionLoc_ = glGetUniformLocation(texProgram_, "uResolution");
                texColorLoc_ = glGetUniformLocation(texProgram_, "uColor");
                texSamplerLoc_ = glGetUniformLocation(texProgram_, "uTexture");
                texRotationLoc_ = glGetUniformLocation(texProgram_, "uRotation");
                texRotCenterLoc_ = glGetUniformLocation(texProgram_, "uRotCenter");
                return true;
            }

            GLuint AndroidRenderer::uploadTexture(const unsigned char *data,
                                                  int w, int h, int channels)
            {
                GLuint tex = 0;
                glGenTextures(1, &tex);
                glBindTexture(GL_TEXTURE_2D, tex);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                GLenum fmt = (channels == 4) ? GL_RGBA : (channels == 3) ? GL_RGB
                                                     : (channels == 1)   ? GL_LUMINANCE
                                                                         : GL_RGBA;
                glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
                return tex;
            }

            void AndroidRenderer::drawTexturedQuad(GLuint texture,
                                                   float x, float y, float w, float h,
                                                   const Color &tint)
            {
                auto c = applyOpacity(tint);
                glUseProgram(texProgram_);
                glUniform2f(texPosLoc_, x, y);
                glUniform2f(texSizeLoc_, w, h);
                glUniform2f(texResolutionLoc_,
                            static_cast<float>(surfaceWidth_),
                            static_cast<float>(surfaceHeight_));
                glUniform4f(texColorLoc_, c.r, c.g, c.b, c.a);
                glUniform1f(texRotationLoc_, activeRotation_);
                glUniform2f(texRotCenterLoc_, activeRotCenterX_, activeRotCenterY_);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture);
                glUniform1i(texSamplerLoc_, 0);

                // مواقع + إحداثيات النسيج
                float verts[] = {
                    0, 0, 0, 0, // أعلى يسار
                    1, 0, 1, 0, // أعلى يمين
                    0, 1, 0, 1, // أسفل يسار
                    1, 1, 1, 1  // أسفل يمين
                };
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), verts);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), verts + 2);
                glEnableVertexAttribArray(1);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            }

            void AndroidRenderer::destroyEGL()
            {
                if (eglDisplay_ != EGL_NO_DISPLAY)
                {
                    eglMakeCurrent(eglDisplay_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
                    if (eglContext_ != EGL_NO_CONTEXT)
                        eglDestroyContext(eglDisplay_, eglContext_);
                    if (eglSurface_ != EGL_NO_SURFACE)
                        eglDestroySurface(eglDisplay_, eglSurface_);
                    eglTerminate(eglDisplay_);
                }
                eglDisplay_ = EGL_NO_DISPLAY;
                eglSurface_ = EGL_NO_SURFACE;
                eglContext_ = EGL_NO_CONTEXT;
            }
#endif

            void AndroidRenderer::screenToNDC(float x, float y,
                                              float &ndcX, float &ndcY) const
            {
                ndcX = (x / static_cast<float>(surfaceWidth_)) * 2.0f - 1.0f;
                ndcY = 1.0f - (y / static_cast<float>(surfaceHeight_)) * 2.0f;
            }

            // ── حلقة الرسم ───────────────────────────────

            void AndroidRenderer::beginFrame(const Color &clearColor)
            {
#ifdef __ANDROID__
                glViewport(0, 0, surfaceWidth_, surfaceHeight_);
                glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
                glClear(GL_COLOR_BUFFER_BIT);
#else
                (void)clearColor;
#endif
            }

            void AndroidRenderer::endFrame()
            {
#ifdef __ANDROID__
                eglSwapBuffers(eglDisplay_, eglSurface_);
#endif
            }

            // ════════════════════════════════════════════════════════════════════════════════
            // دوال الرسم
            // ════════════════════════════════════════════════════════════════════════════════

            void AndroidRenderer::drawFilledRect(float x, float y, float w, float h,
                                                 const Color &color)
            {
                auto c = applyOpacity(color);
#ifdef __ANDROID__
                glUseProgram(rectProgram_);
                glUniform4f(rectColorLoc_, c.r, c.g, c.b, c.a);
                glUniform2f(rectPosLoc_, x, y);
                glUniform2f(rectSizeLoc_, w, h);
                glUniform2f(rectResolutionLoc_,
                            static_cast<float>(surfaceWidth_),
                            static_cast<float>(surfaceHeight_));
                glUniform1f(rectRadiusLoc_, 0.0f);
                glUniform1f(rectRotationLoc_, activeRotation_);
                glUniform2f(rectRotCenterLoc_, activeRotCenterX_, activeRotCenterY_);

                float verts[] = {0, 0, 1, 0, 0, 1, 1, 1};
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, verts);
                glEnableVertexAttribArray(0);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#else
                (void)x;
                (void)y;
                (void)w;
                (void)h;
                (void)color;
#endif
            }

            void AndroidRenderer::drawRoundedRect(float x, float y, float w, float h,
                                                  const Color &color, float radius)
            {
                auto c = applyOpacity(color);
                // لمبدئياً نستخدم نفس الشيدر مع uniform radius
#ifdef __ANDROID__
                glUseProgram(rectProgram_);
                glUniform4f(rectColorLoc_, c.r, c.g, c.b, c.a);
                glUniform2f(rectPosLoc_, x, y);
                glUniform2f(rectSizeLoc_, w, h);
                glUniform2f(rectResolutionLoc_,
                            static_cast<float>(surfaceWidth_),
                            static_cast<float>(surfaceHeight_));
                glUniform1f(rectRadiusLoc_, radius);
                glUniform1f(rectRotationLoc_, activeRotation_);
                glUniform2f(rectRotCenterLoc_, activeRotCenterX_, activeRotCenterY_);

                float verts[] = {0, 0, 1, 0, 0, 1, 1, 1};
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, verts);
                glEnableVertexAttribArray(0);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#else
                (void)x;
                (void)y;
                (void)w;
                (void)h;
                (void)color;
                (void)radius;
#endif
            }

            void AndroidRenderer::drawRectOutline(float x, float y, float w, float h,
                                                  const Color &color, float thickness)
            {
                // رسم 4 خطوط
                drawFilledRect(x, y, w, thickness, color);                 // أعلى
                drawFilledRect(x, y + h - thickness, w, thickness, color); // أسفل
                drawFilledRect(x, y, thickness, h, color);                 // يسار
                drawFilledRect(x + w - thickness, y, thickness, h, color); // يمين
            }

            std::pair<float, float> AndroidRenderer::drawText(const std::string &text,
                                                              float x, float y,
                                                              const Color &color,
                                                              float fontSize)
            {
#ifdef __ANDROID__
                if (!s_fontInfoValid || fontData_.empty())
                {
                    // fallback — مستطيل ملون تقريبي
                    float approxWidth = fontSize * 0.5f * static_cast<float>(text.size());
                    drawFilledRect(x, y, approxWidth, fontSize, color);
                    return {approxWidth, fontSize * 1.2f};
                }

                float scale = stbtt_ScaleForPixelHeight(&s_fontInfo, fontSize);
                int ascent, descent, lineGap;
                stbtt_GetFontVMetrics(&s_fontInfo, &ascent, &descent, &lineGap);
                float ascentPx = static_cast<float>(ascent) * scale;

                // رسم كل حرف (glyph) على نسيج ثم عرضه
                float penX = x;
                const unsigned char *textData = reinterpret_cast<const unsigned char *>(text.c_str());
                int textLen = static_cast<int>(text.size());
                int i = 0;
                while (i < textLen)
                {
                    int codepoint = 0;
                    int advance = 0;

                    // فك ترميز UTF-8
                    unsigned char c = textData[i];
                    if (c < 0x80)
                    {
                        codepoint = c;
                        advance = 1;
                    }
                    else if ((c & 0xE0) == 0xC0)
                    {
                        codepoint = (c & 0x1F) << 6;
                        if (i + 1 < textLen)
                            codepoint |= (textData[i + 1] & 0x3F);
                        advance = 2;
                    }
                    else if ((c & 0xF0) == 0xE0)
                    {
                        codepoint = (c & 0x0F) << 12;
                        if (i + 1 < textLen)
                            codepoint |= (textData[i + 1] & 0x3F) << 6;
                        if (i + 2 < textLen)
                            codepoint |= (textData[i + 2] & 0x3F);
                        advance = 3;
                    }
                    else if ((c & 0xF8) == 0xF0)
                    {
                        codepoint = (c & 0x07) << 18;
                        if (i + 1 < textLen)
                            codepoint |= (textData[i + 1] & 0x3F) << 12;
                        if (i + 2 < textLen)
                            codepoint |= (textData[i + 2] & 0x3F) << 6;
                        if (i + 3 < textLen)
                            codepoint |= (textData[i + 3] & 0x3F);
                        advance = 4;
                    }
                    else
                    {
                        advance = 1;
                        i += advance;
                        continue;
                    }
                    i += advance;

                    // الحصول على bitmap الحرف من stb_truetype
                    int glyphW = 0, glyphH = 0, xoff = 0, yoff = 0;
                    unsigned char *bitmap = stbtt_GetCodepointBitmap(
                        &s_fontInfo, 0, scale, codepoint, &glyphW, &glyphH, &xoff, &yoff);

                    if (bitmap && glyphW > 0 && glyphH > 0)
                    {
                        // تحويل bitmap أحادي القناة إلى RGBA
                        std::vector<unsigned char> rgba(glyphW * glyphH * 4);
                        for (int p = 0; p < glyphW * glyphH; ++p)
                        {
                            rgba[p * 4 + 0] = 255;
                            rgba[p * 4 + 1] = 255;
                            rgba[p * 4 + 2] = 255;
                            rgba[p * 4 + 3] = bitmap[p];
                        }

                        GLuint tex = uploadTexture(rgba.data(), glyphW, glyphH, 4);
                        float drawY = y + ascentPx + static_cast<float>(yoff);
                        drawTexturedQuad(tex, penX + static_cast<float>(xoff), drawY,
                                         static_cast<float>(glyphW), static_cast<float>(glyphH),
                                         color);
                        glDeleteTextures(1, &tex);
                    }
                    if (bitmap)
                        stbtt_FreeBitmap(bitmap, nullptr);

                    // تقدم القلم
                    int advanceWidth, lsb;
                    stbtt_GetCodepointHMetrics(&s_fontInfo, codepoint, &advanceWidth, &lsb);
                    penX += static_cast<float>(advanceWidth) * scale;

                    // kerning مع الحرف التالي
                    if (i < textLen)
                    {
                        unsigned char nc = textData[i];
                        int nextCP = 0;
                        if (nc < 0x80)
                            nextCP = nc;
                        else if ((nc & 0xE0) == 0xC0 && i + 1 < textLen)
                            nextCP = ((nc & 0x1F) << 6) | (textData[i + 1] & 0x3F);

                        int kern = stbtt_GetCodepointKernAdvance(&s_fontInfo, codepoint, nextCP);
                        penX += static_cast<float>(kern) * scale;
                    }
                }

                float totalWidth = penX - x;
                float totalHeight = fontSize * 1.2f;
                return {totalWidth, totalHeight};
#else
                (void)text;
                (void)x;
                (void)y;
                (void)color;
                (void)fontSize;
                // تقريب محسّن للنسبة العربية
                float totalWidth = 0;
                const unsigned char *p = reinterpret_cast<const unsigned char *>(text.c_str());
                int len = static_cast<int>(text.size());
                int charCount = 0;
                for (int j = 0; j < len;)
                {
                    unsigned char c = p[j];
                    if (c < 0x80)
                    {
                        j += 1;
                    }
                    else if ((c & 0xE0) == 0xC0)
                    {
                        j += 2;
                    }
                    else if ((c & 0xF0) == 0xE0)
                    {
                        j += 3;
                    }
                    else
                    {
                        j += 4;
                    }
                    charCount++;
                }
                totalWidth = fontSize * 0.55f * static_cast<float>(charCount);
                return {totalWidth, fontSize * 1.2f};
#endif
            }

            std::pair<float, float> AndroidRenderer::measureText(const std::string &text,
                                                                 float fontSize)
            {
#ifdef __ANDROID__
                if (!s_fontInfoValid || fontData_.empty())
                {
                    // تقريب
                    float approxWidth = fontSize * 0.5f * static_cast<float>(text.size());
                    return {approxWidth, fontSize * 1.2f};
                }

                float scale = stbtt_ScaleForPixelHeight(&s_fontInfo, fontSize);
                float width = 0;
                const unsigned char *textData = reinterpret_cast<const unsigned char *>(text.c_str());
                int textLen = static_cast<int>(text.size());
                int i = 0;
                int prevCP = 0;
                while (i < textLen)
                {
                    int codepoint = 0;
                    int advance = 0;
                    unsigned char c = textData[i];
                    if (c < 0x80)
                    {
                        codepoint = c;
                        advance = 1;
                    }
                    else if ((c & 0xE0) == 0xC0)
                    {
                        codepoint = (c & 0x1F) << 6;
                        if (i + 1 < textLen)
                            codepoint |= (textData[i + 1] & 0x3F);
                        advance = 2;
                    }
                    else if ((c & 0xF0) == 0xE0)
                    {
                        codepoint = (c & 0x0F) << 12;
                        if (i + 1 < textLen)
                            codepoint |= (textData[i + 1] & 0x3F) << 6;
                        if (i + 2 < textLen)
                            codepoint |= (textData[i + 2] & 0x3F);
                        advance = 3;
                    }
                    else
                    {
                        codepoint = (c & 0x07) << 18;
                        if (i + 1 < textLen)
                            codepoint |= (textData[i + 1] & 0x3F) << 12;
                        if (i + 2 < textLen)
                            codepoint |= (textData[i + 2] & 0x3F) << 6;
                        if (i + 3 < textLen)
                            codepoint |= (textData[i + 3] & 0x3F);
                        advance = 4;
                    }
                    i += advance;

                    int advanceWidth, lsb;
                    stbtt_GetCodepointHMetrics(&s_fontInfo, codepoint, &advanceWidth, &lsb);
                    width += static_cast<float>(advanceWidth) * scale;

                    if (prevCP)
                    {
                        int kern = stbtt_GetCodepointKernAdvance(&s_fontInfo, prevCP, codepoint);
                        width += static_cast<float>(kern) * scale;
                    }
                    prevCP = codepoint;
                }
                return {width, fontSize * 1.2f};
#else
                // تقريب محسّن بحساب عدد الأحرف UTF-8 الفعلي
                const unsigned char *p = reinterpret_cast<const unsigned char *>(text.c_str());
                int len = static_cast<int>(text.size());
                int charCount = 0;
                for (int j = 0; j < len;)
                {
                    unsigned char c = p[j];
                    if (c < 0x80)
                    {
                        j += 1;
                    }
                    else if ((c & 0xE0) == 0xC0)
                    {
                        j += 2;
                    }
                    else if ((c & 0xF0) == 0xE0)
                    {
                        j += 3;
                    }
                    else
                    {
                        j += 4;
                    }
                    charCount++;
                }
                float approxWidth = fontSize * 0.55f * static_cast<float>(charCount);
                return {approxWidth, fontSize * 1.2f};
#endif
            }

            void AndroidRenderer::drawImage(const std::string &path,
                                            float x, float y, float w, float h)
            {
#ifdef __ANDROID__
                // التحقق من الذاكرة المؤقتة
                auto it = imageCache_.find(path);
                if (it != imageCache_.end())
                {
                    drawTexturedQuad(it->second, x, y, w, h);
                    return;
                }

                // تحميل الصورة عبر stb_image
                int imgW, imgH, channels;
                unsigned char *data = stbi_load(path.c_str(), &imgW, &imgH, &channels, 4);
                if (!data)
                {
                    // fallback — مستطيل رمادي
                    drawFilledRect(x, y, w, h, {0.8f, 0.8f, 0.8f, 1.0f});
                    return;
                }

                GLuint tex = uploadTexture(data, imgW, imgH, 4);
                stbi_image_free(data);

                // حفظ في الذاكرة المؤقتة (مع حد أقصى)
                if (imageCache_.size() < 100)
                {
                    imageCache_[path] = tex;
                }

                drawTexturedQuad(tex, x, y, w, h);
#else
                (void)path;
                drawFilledRect(x, y, w, h, {0.8f, 0.8f, 0.8f, 1.0f});
#endif
            }

            void AndroidRenderer::drawCircle(float cx, float cy, float radius,
                                             const Color &color)
            {
                // رسم دائرة كمستطيل بزوايا دائرية بنصف القطر الكامل
                drawRoundedRect(cx - radius, cy - radius,
                                radius * 2, radius * 2,
                                color, radius);
            }

            void AndroidRenderer::drawCircleOutline(float cx, float cy, float radius,
                                                    const Color &color, float thickness)
            {
                drawCircle(cx, cy, radius, color);
                Color transparent = {0, 0, 0, 0};
                drawCircle(cx, cy, radius - thickness, transparent);
            }

            void AndroidRenderer::drawLine(float x1, float y1, float x2, float y2,
                                           const Color &color, float thickness)
            {
                float dx = x2 - x1;
                float dy = y2 - y1;
                float len = std::sqrt(dx * dx + dy * dy);
                if (len < 0.001f)
                    return;

                // تطبيع المتجه العمودي
                float nx = -dy / len * thickness * 0.5f;
                float ny = dx / len * thickness * 0.5f;

#ifdef __ANDROID__
                auto c = applyOpacity(color);
                glUseProgram(rectProgram_);
                glUniform4f(rectColorLoc_, c.r, c.g, c.b, c.a);
                glUniform2f(rectResolutionLoc_,
                            static_cast<float>(surfaceWidth_),
                            static_cast<float>(surfaceHeight_));

                // رسم كمثلثين
                float verts[] = {
                    x1 + nx, y1 + ny,
                    x1 - nx, y1 - ny,
                    x2 + nx, y2 + ny,
                    x2 - nx, y2 - ny};
                // بدون تحويل — استخدام إحداثيات بكسل مباشرة
                glUniform2f(rectPosLoc_, 0, 0);
                glUniform2f(rectSizeLoc_, 1, 1);
                glUniform1f(rectRadiusLoc_, 0);

                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, verts);
                glEnableVertexAttribArray(0);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#else
                (void)x1;
                (void)y1;
                (void)x2;
                (void)y2;
                (void)color;
                (void)thickness;
#endif
            }

            void AndroidRenderer::drawLinearGradient(float x, float y, float w, float h,
                                                     const Color &startColor,
                                                     const Color &endColor,
                                                     bool vertical, float radius)
            {
                // تقريب التدرج بشرائح
                const int steps = 16;
                for (int i = 0; i < steps; ++i)
                {
                    float t = static_cast<float>(i) / static_cast<float>(steps);
                    Color c = {
                        startColor.r + (endColor.r - startColor.r) * t,
                        startColor.g + (endColor.g - startColor.g) * t,
                        startColor.b + (endColor.b - startColor.b) * t,
                        startColor.a + (endColor.a - startColor.a) * t};
                    if (vertical)
                    {
                        float sliceH = h / static_cast<float>(steps);
                        float sliceY = y + i * sliceH;
                        if (radius > 0 && (i == 0 || i == steps - 1))
                            drawRoundedRect(x, sliceY, w, sliceH, c, radius);
                        else
                            drawFilledRect(x, sliceY, w, sliceH, c);
                    }
                    else
                    {
                        float sliceW = w / static_cast<float>(steps);
                        float sliceX = x + i * sliceW;
                        drawFilledRect(sliceX, y, sliceW, h, c);
                    }
                }
            }

            void AndroidRenderer::setClipRect(float x, float y, float w, float h)
            {
#ifdef __ANDROID__
                glEnable(GL_SCISSOR_TEST);
                // OpenGL scissor rect: bottom-left origin
                glScissor(static_cast<int>(x),
                          surfaceHeight_ - static_cast<int>(y + h),
                          static_cast<int>(w),
                          static_cast<int>(h));
#else
                (void)x;
                (void)y;
                (void)w;
                (void)h;
#endif
            }

            void AndroidRenderer::clearClipRect()
            {
#ifdef __ANDROID__
                glDisable(GL_SCISSOR_TEST);
#endif
            }

            // ════════════════════════════════════════════════════════════════════════════════
            // دعم الدوران — OpenGL ES: تخزين حالة الدوران وتمريرها عبر uniforms
            // ════════════════════════════════════════════════════════════════════════════════

            void AndroidRenderer::pushRotation(float centerX, float centerY, float angleDegrees)
            {
                activeRotation_ = angleDegrees * 3.14159265358979f / 180.0f;
                activeRotCenterX_ = centerX;
                activeRotCenterY_ = centerY;
            }

            void AndroidRenderer::popRotation()
            {
                activeRotation_ = 0.0f;
                activeRotCenterX_ = 0.0f;
                activeRotCenterY_ = 0.0f;
            }

            // ═══════════════════════════════════════════════════════════════════════════════
            // AndroidWindow
            // ═══════════════════════════════════════════════════════════════════════════════

            AndroidWindow::AndroidWindow()
                : renderer_(std::make_unique<AndroidRenderer>()), layoutEngine_(std::make_unique<LayoutEngine>())
            {
            }

            AndroidWindow::~AndroidWindow()
            {
                destroy();
            }

            bool AndroidWindow::create(const PlatformWindowOptions &options)
            {
                options_ = options;
                width_ = options.width;
                height_ = options.height;

                layoutEngine_->setDirection(options.direction);

#ifdef __ANDROID__
                if (nativeWindow_)
                {
                    width_ = ANativeWindow_getWidth(nativeWindow_);
                    height_ = ANativeWindow_getHeight(nativeWindow_);
                    if (!renderer_->initialize(nativeWindow_))
                        return false;
                }
#endif

                layoutEngine_->setViewportSize(
                    static_cast<float>(width_),
                    static_cast<float>(height_));

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

                // ─── تهيئة معالج أحداث الفأرة/المؤشر الموحد ───
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
                    [](int /*cursorType*/)
                    {
                        // (AR) أندرويد لا يدعم تغيير شكل مؤشر الفأرة مبدئياً
                        //      يمكن استخدام PointerIcon API مستقبلاً
                    });

                return true;
            }

            void AndroidWindow::destroy()
            {
                renderer_->shutdown();
                isOpen_ = false;
            }

            bool AndroidWindow::isOpen() const { return isOpen_; }

            void AndroidWindow::setContent(std::shared_ptr<IRNode> root)
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

            void AndroidWindow::setStateStore(std::shared_ptr<StateStore> store)
            {
                stateStore_ = std::move(store);
            }

            void AndroidWindow::applyPatches(size_t patchedNodes, bool needsRelayout)
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

            void AndroidWindow::run()
            {
                // على أندرويد، حلقة الأحداث تُدار من NativeActivity
                while (isOpen_)
                {
                    runOneFrame();
                }
            }

            bool AndroidWindow::runOneFrame()
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

            void AndroidWindow::invalidate() { needsRedraw_ = true; }
            int AndroidWindow::getWidth() const { return width_; }
            int AndroidWindow::getHeight() const { return height_; }
            PlatformRenderer *AndroidWindow::getRenderer() { return renderer_.get(); }

            void AndroidWindow::processEvents()
            {
                // أحداث Android تُسلّم من NativeActivity عبر callbacks
                // وتُمرَّر عبر onNativeTouchEvent() إلى TouchEventProcessor
            }

            void AndroidWindow::onNativeTouchEvent(int action, int64_t touchId, int64_t fingerId,
                                                   float x, float y, float pressure,
                                                   float dx, float dy)
            {
                // (AR) تحويل أحداث اللمس الأصلية من JNI/NativeActivity إلى معالج موحد
                //      action: 0=AMOTION_EVENT_ACTION_DOWN, 1=UP, 2=MOVE
                switch (action)
                {
                case 0: // DOWN
                    touchProcessor_.onFingerDown(touchId, fingerId, x, y, pressure);
                    break;
                case 1: // UP
                    touchProcessor_.onFingerUp(touchId, fingerId, x, y);
                    break;
                case 2: // MOVE
                    touchProcessor_.onFingerMotion(touchId, fingerId, x, y, dx, dy, pressure);
                    break;
                }
                needsRedraw_ = true;
            }

            void AndroidWindow::onNativeKeyEvent(int action, int keyCode,
                                                 const std::string &keyName,
                                                 bool shift, bool ctrl, bool alt)
            {
                // (AR) تحويل حدث لوحة المفاتيح من JNI إلى معالج موحد
                //      action: 0=DOWN, 1=UP
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

            void AndroidWindow::onNativeTextInput(const std::string &text)
            {
                // (AR) تمرير إدخال نص من IME الأصلي إلى معالج موحد
                kbProcessor_.onTextInput(text);
                needsRedraw_ = true;
            }

            // ─── أحداث الفأرة/المؤشر (للأجهزة مع فأرة أو قلم رقمي) ───

            void AndroidWindow::onNativeMouseEvent(int action, int button,
                                                   float x, float y)
            {
                // (AR) تحويل حدث زر الفأرة من JNI إلى معالج موحد
                //      action: 0=DOWN, 1=UP
                //      button: 0=Left, 1=Middle, 2=Right
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

            void AndroidWindow::onNativeMouseMove(float x, float y,
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

            void AndroidWindow::onNativeMouseWheel(float deltaX, float deltaY,
                                                   float mouseX, float mouseY,
                                                   bool ctrlPressed)
            {
                // (AR) تمرير حدث عجلة التمرير إلى معالج موحد
                mouseProcessor_.onMouseWheel(deltaX, deltaY, mouseX, mouseY, ctrlPressed);
                needsRedraw_ = true;
            }

            void AndroidWindow::updateLayout()
            {
                if (contentRoot_)
                {
                    LayoutConstraints constraints;
                    constraints.maxWidth = static_cast<float>(width_);
                    constraints.maxHeight = static_cast<float>(height_);
                    layoutResult_ = layoutEngine_->layout(contentRoot_, constraints);
                }
            }

            void AndroidWindow::renderFrame()
            {
                Color bg = {0.98f, 0.98f, 0.98f, 1.0f};
                renderer_->beginFrame(bg);
                if (contentRoot_ && layoutResult_)
                {
                    renderer_->render(contentRoot_, layoutResult_);
                }
                renderer_->endFrame();
            }

        } // namespace android
    } // namespace ui
} // namespace sad
