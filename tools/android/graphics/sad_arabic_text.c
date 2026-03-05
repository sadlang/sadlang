/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  تنفيذ نظام عرض النصوص العربية
 *  SadUI Arabic Text Renderer Implementation
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_arabic_text.h"
#include <android/log.h>
#include <GLES3/gl3.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define LOG_TAG "SadArabicText"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Include stb_truetype
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "../../../graphics/third_party/stb_truetype.h"

// ═══════════════════════════════════════════════════════════════════════════════
//  جدول أشكال الحروف العربية
// ═══════════════════════════════════════════════════════════════════════════════

// كل حرف عربي له 4 أشكال: منفصل، بداية، وسط، نهاية
// بعض الحروف لا تتصل من اليسار (مثل ا، د، ذ، ر، ز، و)

typedef struct {
    uint32_t base;       // الشكل الأساسي
    uint32_t isolated;   // منفصل
    uint32_t initial;    // بداية
    uint32_t medial;     // وسط
    uint32_t final;      // نهاية
    bool joinsLeft;      // يتصل من اليسار
} ArabicCharInfo;

// جدول الحروف العربية الأساسية
static const ArabicCharInfo ARABIC_CHARS[] = {
    // base    isolated  initial   medial    final     joinsLeft
    {0x0621, 0xFE80, 0xFE80, 0xFE80, 0xFE80, false},  // ء همزة
    {0x0622, 0xFE81, 0xFE81, 0xFE82, 0xFE82, false},  // آ ألف مد
    {0x0623, 0xFE83, 0xFE83, 0xFE84, 0xFE84, false},  // أ ألف همزة فوق
    {0x0624, 0xFE85, 0xFE85, 0xFE86, 0xFE86, false},  // ؤ واو همزة
    {0x0625, 0xFE87, 0xFE87, 0xFE88, 0xFE88, false},  // إ ألف همزة تحت
    {0x0626, 0xFE89, 0xFE8B, 0xFE8C, 0xFE8A, true},   // ئ ياء همزة
    {0x0627, 0xFE8D, 0xFE8D, 0xFE8E, 0xFE8E, false},  // ا ألف
    {0x0628, 0xFE8F, 0xFE91, 0xFE92, 0xFE90, true},   // ب باء
    {0x0629, 0xFE93, 0xFE93, 0xFE94, 0xFE94, false},  // ة تاء مربوطة
    {0x062A, 0xFE95, 0xFE97, 0xFE98, 0xFE96, true},   // ت تاء
    {0x062B, 0xFE99, 0xFE9B, 0xFE9C, 0xFE9A, true},   // ث ثاء
    {0x062C, 0xFE9D, 0xFE9F, 0xFEA0, 0xFE9E, true},   // ج جيم
    {0x062D, 0xFEA1, 0xFEA3, 0xFEA4, 0xFEA2, true},   // ح حاء
    {0x062E, 0xFEA5, 0xFEA7, 0xFEA8, 0xFEA6, true},   // خ خاء
    {0x062F, 0xFEA9, 0xFEA9, 0xFEAA, 0xFEAA, false},  // د دال
    {0x0630, 0xFEAB, 0xFEAB, 0xFEAC, 0xFEAC, false},  // ذ ذال
    {0x0631, 0xFEAD, 0xFEAD, 0xFEAE, 0xFEAE, false},  // ر راء
    {0x0632, 0xFEAF, 0xFEAF, 0xFEB0, 0xFEB0, false},  // ز زاي
    {0x0633, 0xFEB1, 0xFEB3, 0xFEB4, 0xFEB2, true},   // س سين
    {0x0634, 0xFEB5, 0xFEB7, 0xFEB8, 0xFEB6, true},   // ش شين
    {0x0635, 0xFEB9, 0xFEBB, 0xFEBC, 0xFEBA, true},   // ص صاد
    {0x0636, 0xFEBD, 0xFEBF, 0xFEC0, 0xFEBE, true},   // ض ضاد
    {0x0637, 0xFEC1, 0xFEC3, 0xFEC4, 0xFEC2, true},   // ط طاء
    {0x0638, 0xFEC5, 0xFEC7, 0xFEC8, 0xFEC6, true},   // ظ ظاء
    {0x0639, 0xFEC9, 0xFECB, 0xFECC, 0xFECA, true},   // ع عين
    {0x063A, 0xFECD, 0xFECF, 0xFED0, 0xFECE, true},   // غ غين
    {0x0640, 0x0640, 0x0640, 0x0640, 0x0640, true},   // ـ تطويل
    {0x0641, 0xFED1, 0xFED3, 0xFED4, 0xFED2, true},   // ف فاء
    {0x0642, 0xFED5, 0xFED7, 0xFED8, 0xFED6, true},   // ق قاف
    {0x0643, 0xFED9, 0xFEDB, 0xFEDC, 0xFEDA, true},   // ك كاف
    {0x0644, 0xFEDD, 0xFEDF, 0xFEE0, 0xFEDE, true},   // ل لام
    {0x0645, 0xFEE1, 0xFEE3, 0xFEE4, 0xFEE2, true},   // م ميم
    {0x0646, 0xFEE5, 0xFEE7, 0xFEE8, 0xFEE6, true},   // ن نون
    {0x0647, 0xFEE9, 0xFEEB, 0xFEEC, 0xFEEA, true},   // ه هاء
    {0x0648, 0xFEED, 0xFEED, 0xFEEE, 0xFEEE, false},  // و واو
    {0x0649, 0xFEEF, 0xFEEF, 0xFEF0, 0xFEF0, false},  // ى ألف مقصورة
    {0x064A, 0xFEF1, 0xFEF3, 0xFEF4, 0xFEF2, true},   // ي ياء
};

#define ARABIC_CHARS_COUNT (sizeof(ARABIC_CHARS) / sizeof(ARABIC_CHARS[0]))

// ═══════════════════════════════════════════════════════════════════════════════
//  هياكل داخلية
// ═══════════════════════════════════════════════════════════════════════════════

typedef struct {
    stbtt_fontinfo stbFont;
    unsigned char* fontData;
    int fontDataSize;
    float fontSize;
    float scale;
    
    // Atlas
    GLuint atlasTexture;
    int atlasWidth;
    int atlasHeight;
    unsigned char* atlasBitmap;
    
    // Glyph cache
    SadGlyphInfo* glyphCache;
    int glyphCacheCount;
    int glyphCacheCapacity;
    
    // للـ packing
    int atlasX;
    int atlasY;
    int atlasRowHeight;
    
    // مقاييس الخط
    int ascent;
    int descent;
    int lineGap;
    
    SadFontHandle handle;
    bool valid;
    
} SadFontInternal;

// ═══════════════════════════════════════════════════════════════════════════════
//  المتغيرات العامة
// ═══════════════════════════════════════════════════════════════════════════════

static SadFontInternal g_fonts[SAD_MAX_LOADED_FONTS];
static int g_fontCount = 0;
static SadFontHandle g_nextFontHandle = 1;
static SadFontHandle g_defaultFont = 0;
static bool g_initialized = false;

// Shader للنص
static GLuint g_textShader = 0;
static GLuint g_textVAO = 0;
static GLuint g_textVBO = 0;

// أبعاد الشاشة لمصفوفة الإسقاط
static int g_screenWidth = 1080;
static int g_screenHeight = 2400;

// ═══════════════════════════════════════════════════════════════════════════════
//  الشيدرز
// ═══════════════════════════════════════════════════════════════════════════════

static const char* TEXT_VERTEX_SHADER = 
    "#version 300 es\n"
    "layout(location = 0) in vec4 vertex;\n"  // xy = position, zw = texcoord
    "out vec2 TexCoords;\n"
    "uniform mat4 projection;\n"
    "void main() {\n"
    "    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
    "    TexCoords = vertex.zw;\n"
    "}\n";

static const char* TEXT_FRAGMENT_SHADER = 
    "#version 300 es\n"
    "precision highp float;\n"
    "in vec2 TexCoords;\n"
    "out vec4 color;\n"
    "uniform sampler2D text;\n"
    "uniform vec4 textColor;\n"
    "void main() {\n"
    "    float alpha = texture(text, TexCoords).r;\n"
    "    color = vec4(textColor.rgb, textColor.a * alpha);\n"
    "}\n";

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

static GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        LOGE("Shader error: %s", log);
    }
    return shader;
}

static void create_shaders(void) {
    GLuint vs = compile_shader(GL_VERTEX_SHADER, TEXT_VERTEX_SHADER);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, TEXT_FRAGMENT_SHADER);
    
    g_textShader = glCreateProgram();
    glAttachShader(g_textShader, vs);
    glAttachShader(g_textShader, fs);
    glLinkProgram(g_textShader);
    
    GLint linkOk;
    glGetProgramiv(g_textShader, GL_LINK_STATUS, &linkOk);
    if (!linkOk) {
        char log[512];
        glGetProgramInfoLog(g_textShader, 512, NULL, log);
        LOGE("Text shader link error: %s", log);
    } else {
        LOGI("Text shader linked OK, program=%u", g_textShader);
    }
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    // إنشاء VAO/VBO
    glGenVertexArrays(1, &g_textVAO);
    glGenBuffers(1, &g_textVBO);
    
    glBindVertexArray(g_textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static SadFontInternal* get_font(SadFontHandle handle) {
    for (int i = 0; i < g_fontCount; i++) {
        if (g_fonts[i].handle == handle && g_fonts[i].valid) {
            return &g_fonts[i];
        }
    }
    return NULL;
}

// البحث عن معلومات الحرف العربي
static const ArabicCharInfo* find_arabic_char(uint32_t codepoint) {
    for (int i = 0; i < (int)ARABIC_CHARS_COUNT; i++) {
        if (ARABIC_CHARS[i].base == codepoint) {
            return &ARABIC_CHARS[i];
        }
    }
    return NULL;
}

// فك ترميز UTF-8
static int decode_utf8(const char* str, uint32_t* out) {
    unsigned char c = (unsigned char)str[0];
    
    if (c < 0x80) {
        *out = c;
        return 1;
    } else if ((c & 0xE0) == 0xC0) {
        *out = ((c & 0x1F) << 6) | (str[1] & 0x3F);
        return 2;
    } else if ((c & 0xF0) == 0xE0) {
        *out = ((c & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F);
        return 3;
    } else if ((c & 0xF8) == 0xF0) {
        *out = ((c & 0x07) << 18) | ((str[1] & 0x3F) << 12) | 
               ((str[2] & 0x3F) << 6) | (str[3] & 0x3F);
        return 4;
    }
    
    *out = '?';
    return 1;
}

// الحصول على glyph من الـ cache أو إنشاؤه
static SadGlyphInfo* get_glyph(SadFontInternal* font, uint32_t codepoint) {
    // البحث في cache
    for (int i = 0; i < font->glyphCacheCount; i++) {
        if (font->glyphCache[i].codepoint == codepoint) {
            return &font->glyphCache[i];
        }
    }
    
    // إنشاء glyph جديد
    if (font->glyphCacheCount >= font->glyphCacheCapacity) {
        // توسيع cache
        int newCap = font->glyphCacheCapacity * 2;
        font->glyphCache = realloc(font->glyphCache, newCap * sizeof(SadGlyphInfo));
        font->glyphCacheCapacity = newCap;
    }
    
    SadGlyphInfo* glyph = &font->glyphCache[font->glyphCacheCount++];
    memset(glyph, 0, sizeof(SadGlyphInfo));
    glyph->codepoint = codepoint;
    
    // الحصول على فهرس الـ glyph
    int glyphIndex = stbtt_FindGlyphIndex(&font->stbFont, codepoint);
    glyph->glyphIndex = glyphIndex;
    
    if (glyphIndex == 0) {
        // حرف غير موجود
        glyph->xadvance = font->fontSize * 0.5f;
        return glyph;
    }
    
    // الحصول على مقاييس الـ glyph
    int advance, lsb;
    stbtt_GetGlyphHMetrics(&font->stbFont, glyphIndex, &advance, &lsb);
    glyph->xadvance = advance * font->scale;
    
    // رسم الـ glyph إلى الـ atlas
    int x0, y0, x1, y1;
    stbtt_GetGlyphBitmapBox(&font->stbFont, glyphIndex, font->scale, font->scale, 
                            &x0, &y0, &x1, &y1);
    
    int w = x1 - x0;
    int h = y1 - y0;
    
    if (w <= 0 || h <= 0) {
        return glyph;
    }
    
    // التحقق من المساحة في atlas
    if (font->atlasX + w >= font->atlasWidth) {
        font->atlasX = 1;
        font->atlasY += font->atlasRowHeight + 1;
        font->atlasRowHeight = 0;
    }
    
    if (font->atlasY + h >= font->atlasHeight) {
        // Atlas ممتلئ - نحتاج إلى توسيعه أو إنشاء جديد
        LOGE("Font atlas full!");
        return glyph;
    }
    
    // رسم الـ glyph
    unsigned char* dest = font->atlasBitmap + font->atlasY * font->atlasWidth + font->atlasX;
    stbtt_MakeGlyphBitmap(&font->stbFont, dest, w, h, font->atlasWidth, 
                          font->scale, font->scale, glyphIndex);
    
    // تحديث texture
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, font->atlasWidth);
    glBindTexture(GL_TEXTURE_2D, font->atlasTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, font->atlasX, font->atlasY, w, h,
                    GL_RED, GL_UNSIGNED_BYTE, dest);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    
    // حفظ الإحداثيات
    glyph->x0 = (float)font->atlasX / font->atlasWidth;
    glyph->y0 = (float)font->atlasY / font->atlasHeight;
    glyph->x1 = (float)(font->atlasX + w) / font->atlasWidth;
    glyph->y1 = (float)(font->atlasY + h) / font->atlasHeight;
    glyph->width = w;
    glyph->height = h;
    glyph->xoff = x0;
    glyph->yoff = y0;
    
    // تحديث موقع atlas
    font->atlasX += w + 1;
    if (h > font->atlasRowHeight) {
        font->atlasRowHeight = h;
    }
    
    return glyph;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  API العام
// ═══════════════════════════════════════════════════════════════════════════════

bool sad_font_init(void) {
    if (g_initialized) return true;
    
    LOGI("Initializing Arabic text system");
    
    memset(g_fonts, 0, sizeof(g_fonts));
    g_fontCount = 0;
    g_nextFontHandle = 1;
    g_defaultFont = 0;
    
    create_shaders();
    
    g_initialized = true;
    return true;
}

void sad_font_shutdown(void) {
    if (!g_initialized) return;
    
    for (int i = 0; i < g_fontCount; i++) {
        if (g_fonts[i].valid) {
            if (g_fonts[i].fontData) free(g_fonts[i].fontData);
            if (g_fonts[i].atlasBitmap) free(g_fonts[i].atlasBitmap);
            if (g_fonts[i].glyphCache) free(g_fonts[i].glyphCache);
            if (g_fonts[i].atlasTexture) glDeleteTextures(1, &g_fonts[i].atlasTexture);
        }
    }
    
    if (g_textShader) glDeleteProgram(g_textShader);
    if (g_textVAO) glDeleteVertexArrays(1, &g_textVAO);
    if (g_textVBO) glDeleteBuffers(1, &g_textVBO);
    
    g_initialized = false;
}

SadFontHandle sad_font_load_memory(const void* data, int dataSize, float size) {
    if (!g_initialized || !data || dataSize <= 0) return 0;
    if (g_fontCount >= SAD_MAX_LOADED_FONTS) return 0;
    
    SadFontInternal* font = &g_fonts[g_fontCount];
    memset(font, 0, sizeof(SadFontInternal));
    
    // نسخ بيانات الخط
    font->fontData = malloc(dataSize);
    memcpy(font->fontData, data, dataSize);
    font->fontDataSize = dataSize;
    font->fontSize = size;
    
    // تهيئة stb_truetype
    if (!stbtt_InitFont(&font->stbFont, font->fontData, 0)) {
        LOGE("Failed to initialize font");
        free(font->fontData);
        return 0;
    }
    
    font->scale = stbtt_ScaleForPixelHeight(&font->stbFont, size);
    
    // الحصول على مقاييس الخط
    stbtt_GetFontVMetrics(&font->stbFont, &font->ascent, &font->descent, &font->lineGap);
    
    // إنشاء atlas
    font->atlasWidth = SAD_FONT_ATLAS_SIZE;
    font->atlasHeight = SAD_FONT_ATLAS_SIZE;
    font->atlasBitmap = calloc(font->atlasWidth * font->atlasHeight, 1);
    font->atlasX = 1;
    font->atlasY = 1;
    font->atlasRowHeight = 0;
    
    // إنشاء texture
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &font->atlasTexture);
    glBindTexture(GL_TEXTURE_2D, font->atlasTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, font->atlasWidth, font->atlasHeight, 
                 0, GL_RED, GL_UNSIGNED_BYTE, font->atlasBitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // تهيئة glyph cache
    font->glyphCacheCapacity = 256;
    font->glyphCache = malloc(font->glyphCacheCapacity * sizeof(SadGlyphInfo));
    font->glyphCacheCount = 0;
    
    font->handle = g_nextFontHandle++;
    font->valid = true;
    g_fontCount++;
    
    if (g_defaultFont == 0) {
        g_defaultFont = font->handle;
    }
    
    LOGI("Font loaded: handle=%d, size=%.1f", font->handle, size);
    return font->handle;
}

SadFontHandle sad_font_load(const char* path, float size) {
    // قراءة الملف
    FILE* f = fopen(path, "rb");
    if (!f) {
        LOGE("Cannot open font file: %s", path);
        return 0;
    }
    
    fseek(f, 0, SEEK_END);
    long fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    unsigned char* data = malloc(fileSize);
    fread(data, 1, fileSize, f);
    fclose(f);
    
    SadFontHandle handle = sad_font_load_memory(data, fileSize, size);
    free(data);
    
    return handle;
}

void sad_font_unload(SadFontHandle handle) {
    SadFontInternal* font = get_font(handle);
    if (!font) return;
    
    if (font->fontData) free(font->fontData);
    if (font->atlasBitmap) free(font->atlasBitmap);
    if (font->glyphCache) free(font->glyphCache);
    if (font->atlasTexture) glDeleteTextures(1, &font->atlasTexture);
    
    font->valid = false;
    
    if (g_defaultFont == handle) {
        g_defaultFont = 0;
    }
}

SadFontMetrics sad_font_get_metrics(SadFontHandle handle) {
    SadFontMetrics metrics = {0};
    SadFontInternal* font = get_font(handle);
    
    if (font) {
        metrics.ascent = font->ascent * font->scale;
        metrics.descent = -font->descent * font->scale;
        metrics.lineGap = font->lineGap * font->scale;
        metrics.lineHeight = metrics.ascent + metrics.descent + metrics.lineGap;
        metrics.unitsPerEm = 1.0f / font->scale;
    }
    
    return metrics;
}

void sad_font_set_default(SadFontHandle handle) {
    g_defaultFont = handle;
}

SadFontHandle sad_font_get_default(void) {
    return g_defaultFont;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  تشكيل النص العربي
// ═══════════════════════════════════════════════════════════════════════════════

bool sad_is_arabic_char(uint32_t cp) {
    return (cp >= SAD_ARABIC_START && cp <= SAD_ARABIC_END) ||
           (cp >= SAD_ARABIC_PRESENTATION_A && cp <= 0xFDFF) ||
           (cp >= SAD_ARABIC_PRESENTATION_B && cp <= 0xFEFF);
}

bool sad_arabic_joins_right(uint32_t cp) {
    // كل الحروف العربية تتصل من اليمين
    return sad_is_arabic_char(cp) && cp != 0x0621;  // ما عدا الهمزة المنفردة
}

bool sad_arabic_joins_left(uint32_t cp) {
    const ArabicCharInfo* info = find_arabic_char(cp);
    return info ? info->joinsLeft : false;
}

uint32_t sad_arabic_shape(uint32_t codepoint, bool prevJoins, bool nextJoins) {
    const ArabicCharInfo* info = find_arabic_char(codepoint);
    if (!info) return codepoint;
    
    if (prevJoins && nextJoins && info->joinsLeft) {
        return info->medial;      // وسط
    } else if (prevJoins && !nextJoins) {
        return info->final;       // نهاية
    } else if (!prevJoins && nextJoins && info->joinsLeft) {
        return info->initial;     // بداية
    } else {
        return info->isolated;    // منفصل
    }
}

int sad_arabic_shape_text(const char* input, uint32_t* output, int outputSize) {
    if (!input || !output || outputSize <= 0) return 0;
    
    // أولاً: تحويل UTF-8 إلى codepoints
    uint32_t* codepoints = malloc(outputSize * sizeof(uint32_t));
    int len = 0;
    
    const char* p = input;
    while (*p && len < outputSize - 1) {
        uint32_t cp;
        int bytes = decode_utf8(p, &cp);
        codepoints[len] = cp;
        len++;
        p += bytes;
    }
    
    // ثانياً: التشكيل
    for (int i = 0; i < len; i++) {
        uint32_t cp = codepoints[i];
        
        if (sad_is_arabic_char(cp) && cp >= 0x0621 && cp <= 0x064A) {
            bool prevJoins = (i > 0) && sad_arabic_joins_left(codepoints[i-1]);
            bool nextJoins = (i < len-1) && sad_arabic_joins_right(codepoints[i+1]);
            
            output[i] = sad_arabic_shape(cp, prevJoins, nextJoins);
        } else {
            output[i] = cp;
        }
    }
    
    free(codepoints);
    return len;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  رسم النص
// ═══════════════════════════════════════════════════════════════════════════════

SadTextMetrics sad_text_measure(SadFontHandle fontHandle, const char* text, 
                                 const SadTextOptions* options, float maxWidth) {
    SadTextMetrics metrics = {0};
    if (!text) return metrics;
    
    SadFontInternal* font = get_font(fontHandle);
    if (!font) font = get_font(g_defaultFont);
    if (!font) return metrics;
    
    float scale = options ? options->fontSize / font->fontSize : 1.0f;
    float x = 0;
    float lineHeight = (font->ascent - font->descent + font->lineGap) * font->scale * scale;
    
    metrics.lineCount = 1;
    metrics.height = lineHeight;
    
    const char* p = text;
    while (*p) {
        uint32_t cp;
        int bytes = decode_utf8(p, &cp);
        p += bytes;
        
        SadGlyphInfo* glyph = get_glyph(font, cp);
        x += glyph->xadvance * scale;
        
        if (options && options->letterSpacing > 0) {
            x += options->letterSpacing;
        }
        
        if (maxWidth > 0 && x > maxWidth) {
            metrics.lineCount++;
            x = 0;
        }
    }
    
    metrics.width = x;
    metrics.height = lineHeight * metrics.lineCount;
    
    return metrics;
}

void sad_text_draw(SadFontHandle fontHandle, const char* text, 
                   float x, float y, const SadTextOptions* options) {
    if (!text || !*text) return;
    
    SadFontInternal* font = get_font(fontHandle);
    if (!font) font = get_font(g_defaultFont);
    if (!font) { LOGE("sad_text_draw: no font!"); return; }
    
    float scale = options ? options->fontSize / font->fontSize : 1.0f;
    float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
    
    if (options && options->textColor) {
        r = ((options->textColor >> 24) & 0xFF) / 255.0f;
        g = ((options->textColor >> 16) & 0xFF) / 255.0f;
        b = ((options->textColor >> 8) & 0xFF) / 255.0f;
        a = (options->textColor & 0xFF) / 255.0f;
    }
    
    // تفعيل الـ blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUseProgram(g_textShader);
    
    // تعيين مصفوفة الإسقاط (orthographic)
    float projection[16];
    memset(projection, 0, sizeof(projection));
    projection[0]  =  2.0f / g_screenWidth;
    projection[5]  = -2.0f / g_screenHeight;  // Y مقلوب
    projection[10] = -1.0f;
    projection[12] = -1.0f;
    projection[13] =  1.0f;
    projection[15] =  1.0f;
    glUniformMatrix4fv(glGetUniformLocation(g_textShader, "projection"), 1, GL_FALSE, projection);
    
    glUniform4f(glGetUniformLocation(g_textShader, "textColor"), r, g, b, a);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font->atlasTexture);
    glUniform1i(glGetUniformLocation(g_textShader, "text"), 0);
    
    glBindVertexArray(g_textVAO);
    
    // تشكيل النص إذا كان عربياً
    uint32_t shaped[1024];
    int len = sad_arabic_shape_text(text, shaped, 1024);
    
    float curX = x;
    float curY = y + font->ascent * font->scale * scale;
    
    // للنص العربي (RTL)، نبدأ من اليمين
    if (options && options->rtl) {
        SadTextMetrics metrics = sad_text_measure(fontHandle, text, options, 0);
        curX = x + metrics.width;
    }
    
    for (int i = 0; i < len; i++) {
        uint32_t cp = shaped[i];
        SadGlyphInfo* glyph = get_glyph(font, cp);
        
        if (glyph->width > 0 && glyph->height > 0) {
            float xpos = curX + glyph->xoff * scale;
            float ypos = curY + glyph->yoff * scale;
            float w = glyph->width * scale;
            float h = glyph->height * scale;
            
            if (options && options->rtl) {
                xpos = curX - glyph->width * scale - glyph->xoff * scale;
            }
            
            float vertices[6][4] = {
                { xpos,     ypos + h,   glyph->x0, glyph->y1 },
                { xpos,     ypos,       glyph->x0, glyph->y0 },
                { xpos + w, ypos,       glyph->x1, glyph->y0 },
                { xpos,     ypos + h,   glyph->x0, glyph->y1 },
                { xpos + w, ypos,       glyph->x1, glyph->y0 },
                { xpos + w, ypos + h,   glyph->x1, glyph->y1 }
            };
            
            glBindBuffer(GL_ARRAY_BUFFER, g_textVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        
        if (options && options->rtl) {
            curX -= glyph->xadvance * scale;
        } else {
            curX += glyph->xadvance * scale;
        }
        
        if (options && options->letterSpacing != 0) {
            curX += (options->rtl ? -1 : 1) * options->letterSpacing;
        }
    }
    
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void sad_text_draw_wrapped(SadFontHandle fontHandle, const char* text,
                           float x, float y, float maxWidth,
                           const SadTextOptions* options) {
    // TODO: تنفيذ التفاف النص
    sad_text_draw(fontHandle, text, x, y, options);
}

SadTextOptions sad_text_options_arabic(float fontSize) {
    SadTextOptions opts = {0};
    opts.fontSize = fontSize;
    opts.lineSpacing = 1.5f;
    opts.letterSpacing = 0;
    opts.rtl = true;
    opts.enableShaping = true;
    opts.enableKerning = true;
    opts.textColor = 0x000000FF;  // أسود
    return opts;
}

SadTextOptions sad_text_options_latin(float fontSize) {
    SadTextOptions opts = {0};
    opts.fontSize = fontSize;
    opts.lineSpacing = 1.2f;
    opts.letterSpacing = 0;
    opts.rtl = false;
    opts.enableShaping = false;
    opts.enableKerning = true;
    opts.textColor = 0x000000FF;
    return opts;
}

void sad_text_set_screen_size(int width, int height) {
    g_screenWidth = width;
    g_screenHeight = height;
}