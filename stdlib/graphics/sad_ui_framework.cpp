// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════
// sad_ui_framework.cpp — محرك واجهات المستخدم الرسومي (109 مكون)
// إطار عمل مشابه لـ Flutter مبني على SDL2 Renderer
//
// هذا الملف يحتوي على:
//   1) نظام خطوط TrueType عالي الجودة (HD) باستخدام stb_truetype
//   2) سجل عناصر (Widget Registry) لتخزين كل المكونات
//   3) حالة التطبيق (AppState) — النافذة + المحرك + الأحداث
//   4) نظام التخطيط التلقائي (Layout) — عمود/صف/شبكة/مكدّس
//   5) محرك الرسم (Renderer) — يرسم كل نوع عنصر بشكله الصحيح
//   6) معالج الأحداث (Events) — نقر/ماوس/لوحة مفاتيح
//   7) دوال الـ API العامة — إنشاء المكونات وتعديل خصائصها
//   8) نظام تعريف المكونات بالخصائص (declarative / Flutter-like)
// ═══════════════════════════════════════════════════════════════════

#include "sad_ui_framework.h"
#include "sad_animation.h"
#include <SDL.h>
#include <algorithm>
#include <cmath>
#include <set>
#include <map>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <sstream>

// ─── نظام الخطوط عالية الجودة (HD TrueType) ───
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "stb_truetype.h"

namespace sad {
namespace ui {

// ═══════════════════════════════════════════════════════════════════
// القسم 1: الخط النقطي المدمج 5×7
// ─────────────────────────────────────────────────────────────────
// كل حرف يتكون من 7 صفوف، كل صف بايت واحد (5 بتات سفلية = بكسلات)
// البت 4 = أقصى اليسار، البت 0 = أقصى اليمين
// يدعم: أرقام 0-9، حروف A-Z، علامات الترقيم، الرموز الحسابية
// لا يدعم: العربية (تحتاج نظام خطوط منفصل)
//
// مثال: حرف A يُرسم هكذا:
//   .###.  = 0x0E
//   #...#  = 0x11
//   #...#  = 0x11
//   #####  = 0x1F
//   #...#  = 0x11
//   #...#  = 0x11
//   #...#  = 0x11
// ═══════════════════════════════════════════════════════════════════

static const unsigned char FONT_DATA[96][7] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // 32 مسافة (space)
    {0x04,0x04,0x04,0x04,0x04,0x00,0x04}, // 33 !
    {0x0A,0x0A,0x0A,0x00,0x00,0x00,0x00}, // 34 "
    {0x0A,0x0A,0x1F,0x0A,0x1F,0x0A,0x0A}, // 35 #
    {0x04,0x0F,0x14,0x0E,0x05,0x1E,0x04}, // 36 $
    {0x18,0x19,0x02,0x04,0x08,0x13,0x03}, // 37 %
    {0x0C,0x12,0x14,0x08,0x15,0x12,0x0D}, // 38 &
    {0x04,0x04,0x08,0x00,0x00,0x00,0x00}, // 39 '
    {0x02,0x04,0x08,0x08,0x08,0x04,0x02}, // 40 (
    {0x08,0x04,0x02,0x02,0x02,0x04,0x08}, // 41 )
    {0x00,0x04,0x15,0x0E,0x15,0x04,0x00}, // 42 *
    {0x00,0x04,0x04,0x1F,0x04,0x04,0x00}, // 43 +
    {0x00,0x00,0x00,0x00,0x00,0x04,0x08}, // 44 ,
    {0x00,0x00,0x00,0x1F,0x00,0x00,0x00}, // 45 -
    {0x00,0x00,0x00,0x00,0x00,0x00,0x04}, // 46 .
    {0x01,0x01,0x02,0x04,0x08,0x10,0x10}, // 47 /
    {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E}, // 48 (0)
    {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E}, // 49 (1)
    {0x0E,0x11,0x01,0x02,0x04,0x08,0x1F}, // 50 (2)
    {0x0E,0x11,0x01,0x06,0x01,0x11,0x0E}, // 51 (3)
    {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02}, // 52 (4)
    {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E}, // 53 (5)
    {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E}, // 54 (6)
    {0x1F,0x01,0x02,0x04,0x04,0x04,0x04}, // 55 (7)
    {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E}, // 56 (8)
    {0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C}, // 57 (9)
    {0x00,0x00,0x04,0x00,0x04,0x00,0x00}, // 58 :
    {0x00,0x00,0x04,0x00,0x04,0x04,0x08}, // 59 ;
    {0x02,0x04,0x08,0x10,0x08,0x04,0x02}, // 60 <
    {0x00,0x00,0x1F,0x00,0x1F,0x00,0x00}, // 61 =
    {0x08,0x04,0x02,0x01,0x02,0x04,0x08}, // 62 >
    {0x0E,0x11,0x01,0x02,0x04,0x00,0x04}, // 63 ?
    {0x0E,0x11,0x17,0x15,0x17,0x10,0x0E}, // 64 @
    {0x0E,0x11,0x11,0x1F,0x11,0x11,0x11}, // 65 A
    {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E}, // 66 B
    {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E}, // 67 C
    {0x1C,0x12,0x11,0x11,0x11,0x12,0x1C}, // 68 D
    {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F}, // 69 E
    {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10}, // 70 F
    {0x0E,0x11,0x10,0x17,0x11,0x11,0x0F}, // 71 G
    {0x11,0x11,0x11,0x1F,0x11,0x11,0x11}, // 72 H
    {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E}, // 73 I
    {0x07,0x02,0x02,0x02,0x02,0x12,0x0C}, // 74 J
    {0x11,0x12,0x14,0x18,0x14,0x12,0x11}, // 75 K
    {0x10,0x10,0x10,0x10,0x10,0x10,0x1F}, // 76 L
    {0x11,0x1B,0x15,0x15,0x11,0x11,0x11}, // 77 M
    {0x11,0x19,0x15,0x13,0x11,0x11,0x11}, // 78 N
    {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}, // 79 O
    {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10}, // 80 P
    {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D}, // 81 Q
    {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11}, // 82 R
    {0x0E,0x11,0x10,0x0E,0x01,0x11,0x0E}, // 83 S
    {0x1F,0x04,0x04,0x04,0x04,0x04,0x04}, // 84 T
    {0x11,0x11,0x11,0x11,0x11,0x11,0x0E}, // 85 U
    {0x11,0x11,0x11,0x11,0x0A,0x0A,0x04}, // 86 V
    {0x11,0x11,0x11,0x15,0x15,0x1B,0x11}, // 87 W
    {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11}, // 88 X
    {0x11,0x11,0x0A,0x04,0x04,0x04,0x04}, // 89 Y
    {0x1F,0x01,0x02,0x04,0x08,0x10,0x1F}, // 90 Z
    {0x0E,0x08,0x08,0x08,0x08,0x08,0x0E}, // 91 [
    {0x10,0x10,0x08,0x04,0x02,0x01,0x01}, // 92 backslash
    {0x0E,0x02,0x02,0x02,0x02,0x02,0x0E}, // 93 ]
    {0x04,0x0A,0x11,0x00,0x00,0x00,0x00}, // 94 ^
    {0x00,0x00,0x00,0x00,0x00,0x00,0x1F}, // 95 _
    // الأحرف الصغيرة a-z (96-127) — نستخدم نفس الشكل الكبير مبسّط
    {0x00,0x00,0x0E,0x01,0x0F,0x11,0x0F}, // 96 ` → a
    {0x00,0x00,0x0E,0x01,0x0F,0x11,0x0F}, // 97 a
    {0x10,0x10,0x1E,0x11,0x11,0x11,0x1E}, // 98 b
    {0x00,0x00,0x0E,0x11,0x10,0x11,0x0E}, // 99 c
    {0x01,0x01,0x0F,0x11,0x11,0x11,0x0F}, // 100 d
    {0x00,0x00,0x0E,0x11,0x1F,0x10,0x0E}, // 101 e
    {0x06,0x08,0x1E,0x08,0x08,0x08,0x08}, // 102 f
    {0x00,0x00,0x0F,0x11,0x0F,0x01,0x0E}, // 103 g
    {0x10,0x10,0x1E,0x11,0x11,0x11,0x11}, // 104 h
    {0x04,0x00,0x0C,0x04,0x04,0x04,0x0E}, // 105 i
    {0x02,0x00,0x06,0x02,0x02,0x12,0x0C}, // 106 j
    {0x10,0x10,0x12,0x14,0x18,0x14,0x12}, // 107 k
    {0x0C,0x04,0x04,0x04,0x04,0x04,0x0E}, // 108 l
    {0x00,0x00,0x1A,0x15,0x15,0x11,0x11}, // 109 m
    {0x00,0x00,0x1E,0x11,0x11,0x11,0x11}, // 110 n
    {0x00,0x00,0x0E,0x11,0x11,0x11,0x0E}, // 111 o
    {0x00,0x00,0x1E,0x11,0x1E,0x10,0x10}, // 112 p
    {0x00,0x00,0x0F,0x11,0x0F,0x01,0x01}, // 113 q
    {0x00,0x00,0x16,0x19,0x10,0x10,0x10}, // 114 r
    {0x00,0x00,0x0F,0x10,0x0E,0x01,0x1E}, // 115 s
    {0x08,0x08,0x1E,0x08,0x08,0x09,0x06}, // 116 t
    {0x00,0x00,0x11,0x11,0x11,0x11,0x0F}, // 117 u
    {0x00,0x00,0x11,0x11,0x0A,0x0A,0x04}, // 118 v
    {0x00,0x00,0x11,0x11,0x15,0x15,0x0A}, // 119 w
    {0x00,0x00,0x11,0x0A,0x04,0x0A,0x11}, // 120 x
    {0x00,0x00,0x11,0x11,0x0F,0x01,0x0E}, // 121 y
    {0x00,0x00,0x1F,0x02,0x04,0x08,0x1F}, // 122 z
    {0x06,0x08,0x08,0x10,0x08,0x08,0x06}, // 123 {
    {0x04,0x04,0x04,0x04,0x04,0x04,0x04}, // 124 |
    {0x0C,0x02,0x02,0x01,0x02,0x02,0x0C}, // 125 }
    {0x00,0x00,0x08,0x15,0x02,0x00,0x00}, // 126 ~
    {0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F}, // 127 ■ (مربع ممتلئ)
};

// ═══════════════════════════════════════════════════════════════════
// القسم 2: سجل العناصر — يخزّن كل المكونات في خريطة عالمية
// ─────────────────────────────────────────────────────────────────
// كل عنصر (Widget) له معرّف فريد (int id)
// يتم الوصول إليه عبر getWidget(id)
// ═══════════════════════════════════════════════════════════════════

static std::unordered_map<int, Widget> g_widgets;  // سجل العناصر
static int g_nextWidgetId = 1;                      // المعرّف التالي

// ─── حالة التطبيق (نافذة واحدة لكل تطبيق) ───
struct AppState {
    int id = 0;
    SDL_Window* window = nullptr;       // نافذة SDL
    SDL_Renderer* renderer = nullptr;   // محرك الرسم
    int rootWidget = -1;                // العنصر الجذر (الرئيسي)
    bool running = true;                // هل التطبيق يعمل؟
    int width = 800, height = 600;      // أبعاد النافذة
    RGBA bgColor = {245, 245, 245, 255}; // لون الخلفية
    // حالة الماوس
    int mouseX = 0, mouseY = 0;
    bool mouseDown = false;
    bool prevMouseDown = false;
    bool mouseClicked = false;          // ضغطة جديدة هذا الإطار
    // حالة لوحة المفاتيح
    std::string keyInput;               // النص المدخل هذا الإطار
    bool backspace = false;             // تم ضغط مسح

    // ═══ نظام التوقيت (جديد) ═══
    // يتتبع الوقت بين الإطارات لدعم:
    //   - نظام التحريك (animations)
    //   - تحديد معدل الإطارات (frame rate limiting)
    //   - حساب FPS للتشخيص
    Uint32 lastTickMs = 0;              // وقت الإطار السابق (ملي ثانية)
    double deltaMs = 16.0;              // فارق الوقت بين الإطارين (ملي ثانية)
    int targetFps = 60;                 // معدل الإطارات المستهدف
    double fps = 60.0;                  // FPS الحالي (للتشخيص)
    double fpsAccum = 0;                // مجمّع الوقت لحساب FPS
    int fpsFrames = 0;                  // عداد الإطارات لحساب FPS
};

static std::unordered_map<int, AppState> g_apps;  // تطبيقات مفتوحة
static int g_nextAppId = 1;

// ═══════════════════════════════════════════════════════════════════
// القسم 3: دوال مساعدة داخلية
// ═══════════════════════════════════════════════════════════════════

// الحصول على مؤشر لعنصر بمعرّفه
Widget* getWidget(int id) {
    auto it = g_widgets.find(id);
    return (it != g_widgets.end()) ? &it->second : nullptr;
}

// الحصول على مؤشر لعنصر باسمه النصي (المعرّف)
Widget* getWidgetByName(const std::string& name) {
    if (name.empty()) return nullptr;
    for (auto& [id, w] : g_widgets) {
        if (w.widgetName == name) return &w;
    }
    return nullptr;
}

// الحصول على مؤشر لتطبيق بمعرّفه
static AppState* getApp(int id) {
    auto it = g_apps.find(id);
    return (it != g_apps.end()) ? &it->second : nullptr;
}

// إنشاء عنصر جديد وإرجاع معرّفه
static int createWidget(WidgetType type) {
    int id = g_nextWidgetId++;
    Widget w;
    w.id = id;
    w.type = type;
    g_widgets[id] = w;
    return id;
}

// ═══════════════════════════════════════════════════════════════════
// القسم 4: نظام الخطوط عالية الجودة (HD TrueType + نقطي احتياطي)
// ─────────────────────────────────────────────────────────────────
// يحاول تحميل خط TrueType من النظام (segoeui → arial → tahoma)
// إذا فشل يرجع للخط النقطي 5×7 كنظام احتياطي
// ═══════════════════════════════════════════════════════════════════

// ─── ذاكرة مؤقتة لحروف TrueType (Glyph Cache) ───
struct GlyphInfo {
    SDL_Texture* texture = nullptr;  // نسيج الحرف (alpha)
    int width = 0, height = 0;      // أبعاد الصورة
    int xoff = 0, yoff = 0;         // إزاحة الرسم
    int advance = 0;                 // المسافة للحرف التالي
};

struct FontData {
    std::vector<unsigned char> ttfBuffer;  // بيانات ملف الخط
    stbtt_fontinfo fontInfo;               // معلومات الخط
    bool loaded = false;                   // هل تم التحميل؟
    // ذاكرة مؤقتة: (codepoint * 1000 + pixelSize) → GlyphInfo
    std::unordered_map<uint64_t, GlyphInfo> glyphCache;
};

static FontData g_font;  // الخط الرئيسي (عالمي)

// تحميل ملف خط TrueType
static bool loadFontFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) return false;
    auto size = file.tellg();
    if (size <= 0) return false;
    file.seekg(0, std::ios::beg);
    g_font.ttfBuffer.resize((size_t)size);
    if (!file.read(reinterpret_cast<char*>(g_font.ttfBuffer.data()), size)) return false;
    if (!stbtt_InitFont(&g_font.fontInfo, g_font.ttfBuffer.data(), 0)) return false;
    g_font.loaded = true;
    return true;
}

// محاولة تحميل خط من مسارات النظام (Windows)
static void ensureFontLoaded() {
    if (g_font.loaded) return;
    // قائمة خطوط النظام — الأفضل أولاً
    const char* paths[] = {
        "C:\\Windows\\Fonts\\segoeui.ttf",   // Segoe UI — نظيف وحديث
        "C:\\Windows\\Fonts\\arial.ttf",     // Arial — متاح دائماً
        "C:\\Windows\\Fonts\\tahoma.ttf",    // Tahoma — يدعم العربية
        "C:\\Windows\\Fonts\\calibri.ttf",   // Calibri — بديل جيد
        nullptr
    };
    for (int i = 0; paths[i]; i++) {
        if (loadFontFile(paths[i])) {
            std::cout << "[SAD UI] HD Font loaded: " << paths[i] << std::endl;
            return;
        }
    }
    std::cout << "[SAD UI] Warning: No TrueType font found, using bitmap fallback" << std::endl;
}

// الحصول على حرف TrueType مع تخزين مؤقت
static GlyphInfo* getGlyph(SDL_Renderer* renderer, int codepoint, int pixelSize) {
    if (!g_font.loaded) return nullptr;
    uint64_t key = (uint64_t)codepoint * 1000 + pixelSize;
    auto it = g_font.glyphCache.find(key);
    if (it != g_font.glyphCache.end()) return &it->second;

    float scale = stbtt_ScaleForPixelHeight(&g_font.fontInfo, (float)pixelSize);
    int w, h, xoff, yoff;
    unsigned char* bitmap = stbtt_GetCodepointBitmap(&g_font.fontInfo, 0, scale,
                                                      codepoint, &w, &h, &xoff, &yoff);
    GlyphInfo gi;
    gi.width = w; gi.height = h; gi.xoff = xoff; gi.yoff = yoff;

    int advW, lsb;
    stbtt_GetCodepointHMetrics(&g_font.fontInfo, codepoint, &advW, &lsb);
    gi.advance = (int)(advW * scale);

    if (bitmap && w > 0 && h > 0) {
        // إنشاء نسيج SDL من البتماب (alpha channel فقط)
        SDL_Surface* surface = SDL_CreateRGBSurface(0, w, h, 32,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        if (surface) {
            uint32_t* pixels = (uint32_t*)surface->pixels;
            for (int py = 0; py < h; py++) {
                for (int px = 0; px < w; px++) {
                    uint8_t alpha = bitmap[py * w + px];
                    // أبيض مع شفافية — اللون يتحدد بـ SDL_SetTextureColorMod
                    pixels[py * (surface->pitch / 4) + px] = (alpha << 24) | 0x00FFFFFF;
                }
            }
            gi.texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (gi.texture) {
                SDL_SetTextureBlendMode(gi.texture, SDL_BLENDMODE_BLEND);
            }
            SDL_FreeSurface(surface);
        }
    }
    if (bitmap) stbtt_FreeBitmap(bitmap, nullptr);

    g_font.glyphCache[key] = gi;
    return &g_font.glyphCache[key];
}

// ═══════════════════════════════════════════════════════════════════
// القسم 2.5: دعم اللغة العربية — اتجاه من اليمين لليسار + تشكيل الحروف
// ─────────────────────────────────────────────────────────────────
// يوفر هذا القسم:
//   1) جدول أشكال الحروف العربية الأربعة (منفصل/نهائي/بدائي/وسطي)
//   2) خوارزمية تشكيل الحروف — اختيار الشكل المناسب حسب موضع الحرف في الكلمة
//   3) معالجة لام-ألف الإجبارية (أربعة أشكال)
//   4) خوارزمية ثنائية الاتجاه المبسطة (BiDi) — دعم النصوص العربية-الإنجليزية المختلطة
//   5) فك ترميز وترميز UTF-8
//
// آلية العمل:
//   - يُحلَّل النص إلى نقاط كود (codepoints)
//   - تُفحص النقاط لمعرفة إن كان النص يحتوي عربية
//   - إن وُجدت عربية: تُشكَّل الحروف ثم تُعكس للعرض البصري
//   - النصوص المختلطة: تُعكس المقاطع العربية مع الحفاظ على اتجاه الأرقام والإنجليزية
// ═══════════════════════════════════════════════════════════════════

// ─── جدول أشكال الحروف العربية ───
// كل حرف عربي أساسي (U+0621 إلى U+064A) له حتى 4 أشكال عرض:
//   - منفصل (isolated): الحرف وحده، غير متصل بشيء
//   - نهائي (final): الحرف في آخر الكلمة، متصل من اليمين فقط
//   - بدائي (initial): الحرف في أول الكلمة، متصل من اليسار فقط
//   - وسطي (medial): الحرف في وسط الكلمة، متصل من الجانبين
// القيمة 0 تعني أن هذا الشكل غير موجود للحرف
struct ArabicLetterForm {
    int isolated;   // الشكل المنفصل
    int final_;     // الشكل النهائي
    int initial;    // الشكل البدائي (0 = لا يوجد — الحرف لا يتصل يساراً)
    int medial;     // الشكل الوسطي (0 = لا يوجد)
};

// الفهرس = الحرف - 0x0621 (42 مدخلة من ء إلى ي)
// الحروف التي لها شكلان فقط (منفصل + نهائي) هي التي لا تتصل بالحرف التالي:
//   ء آ أ ؤ إ ا ة د ذ ر ز و ى
static const ArabicLetterForm ARABIC_FORMS[] = {
    /* 0x0621 ء همزة          */ {0xFE80, 0,      0,      0     },
    /* 0x0622 آ ألف مد         */ {0xFE81, 0xFE82, 0,      0     },
    /* 0x0623 أ ألف همزة فوق   */ {0xFE83, 0xFE84, 0,      0     },
    /* 0x0624 ؤ واو همزة       */ {0xFE85, 0xFE86, 0,      0     },
    /* 0x0625 إ ألف همزة تحت   */ {0xFE87, 0xFE88, 0,      0     },
    /* 0x0626 ئ ياء همزة       */ {0xFE89, 0xFE8A, 0xFE8B, 0xFE8C},
    /* 0x0627 ا ألف            */ {0xFE8D, 0xFE8E, 0,      0     },
    /* 0x0628 ب باء            */ {0xFE8F, 0xFE90, 0xFE91, 0xFE92},
    /* 0x0629 ة تاء مربوطة     */ {0xFE93, 0xFE94, 0,      0     },
    /* 0x062A ت تاء            */ {0xFE95, 0xFE96, 0xFE97, 0xFE98},
    /* 0x062B ث ثاء            */ {0xFE99, 0xFE9A, 0xFE9B, 0xFE9C},
    /* 0x062C ج جيم            */ {0xFE9D, 0xFE9E, 0xFE9F, 0xFEA0},
    /* 0x062D ح حاء            */ {0xFEA1, 0xFEA2, 0xFEA3, 0xFEA4},
    /* 0x062E خ خاء            */ {0xFEA5, 0xFEA6, 0xFEA7, 0xFEA8},
    /* 0x062F د دال            */ {0xFEA9, 0xFEAA, 0,      0     },
    /* 0x0630 ذ ذال            */ {0xFEAB, 0xFEAC, 0,      0     },
    /* 0x0631 ر راء            */ {0xFEAD, 0xFEAE, 0,      0     },
    /* 0x0632 ز زاي            */ {0xFEAF, 0xFEB0, 0,      0     },
    /* 0x0633 س سين            */ {0xFEB1, 0xFEB2, 0xFEB3, 0xFEB4},
    /* 0x0634 ش شين            */ {0xFEB5, 0xFEB6, 0xFEB7, 0xFEB8},
    /* 0x0635 ص صاد            */ {0xFEB9, 0xFEBA, 0xFEBB, 0xFEBC},
    /* 0x0636 ض ضاد            */ {0xFEBD, 0xFEBE, 0xFEBF, 0xFEC0},
    /* 0x0637 ط طاء            */ {0xFEC1, 0xFEC2, 0xFEC3, 0xFEC4},
    /* 0x0638 ظ ظاء            */ {0xFEC5, 0xFEC6, 0xFEC7, 0xFEC8},
    /* 0x0639 ع عين            */ {0xFEC9, 0xFECA, 0xFECB, 0xFECC},
    /* 0x063A غ غين            */ {0xFECD, 0xFECE, 0xFECF, 0xFED0},
    /* 0x063B فارغ             */ {0,      0,      0,      0     },
    /* 0x063C فارغ             */ {0,      0,      0,      0     },
    /* 0x063D فارغ             */ {0,      0,      0,      0     },
    /* 0x063E فارغ             */ {0,      0,      0,      0     },
    /* 0x063F فارغ             */ {0,      0,      0,      0     },
    /* 0x0640 تطويل (كشيدة)    */ {0x0640, 0x0640, 0x0640, 0x0640},
    /* 0x0641 ف فاء            */ {0xFED1, 0xFED2, 0xFED3, 0xFED4},
    /* 0x0642 ق قاف            */ {0xFED5, 0xFED6, 0xFED7, 0xFED8},
    /* 0x0643 ك كاف            */ {0xFED9, 0xFEDA, 0xFEDB, 0xFEDC},
    /* 0x0644 ل لام            */ {0xFEDD, 0xFEDE, 0xFEDF, 0xFEE0},
    /* 0x0645 م ميم            */ {0xFEE1, 0xFEE2, 0xFEE3, 0xFEE4},
    /* 0x0646 ن نون            */ {0xFEE5, 0xFEE6, 0xFEE7, 0xFEE8},
    /* 0x0647 ه هاء            */ {0xFEE9, 0xFEEA, 0xFEEB, 0xFEEC},
    /* 0x0648 و واو            */ {0xFEED, 0xFEEE, 0,      0     },
    /* 0x0649 ى ألف مقصورة     */ {0xFEEF, 0xFEF0, 0,      0     },
    /* 0x064A ي ياء            */ {0xFEF1, 0xFEF2, 0xFEF3, 0xFEF4},
};
static const int ARABIC_FORMS_COUNT = 42; // 0x064A - 0x0621 + 1

// ─── دوال مساعدة للتعرف على نوع الحرف ───

// هل هذا حرف عربي أساسي؟ (من ء إلى ي)
static bool isArabicBase(int cp) {
    return cp >= 0x0621 && cp <= 0x064A;
}

// هل هذا حرف عربي بأي شكل؟ (يشمل أشكال العرض والتشكيل)
static bool isArabicAny(int cp) {
    return (cp >= 0x0600 && cp <= 0x06FF) ||   // الكتلة العربية الأساسية
           (cp >= 0x0750 && cp <= 0x077F) ||   // الملحق العربي
           (cp >= 0xFB50 && cp <= 0xFDFF) ||   // أشكال العرض العربية-أ
           (cp >= 0xFE70 && cp <= 0xFEFF);     // أشكال العرض العربية-ب
}

// هل هذا حرف تشكيل عربي؟ (فتحة، ضمة، كسرة، شدة...)
static bool isArabicDiacritic(int cp) {
    return cp >= 0x064B && cp <= 0x065F;
}

// هل هذا الحرف العربي يتصل بالحرف الذي بعده (يساراً في الكتابة)؟
// الحروف التي لا تتصل يساراً: ء آ أ ؤ إ ا ة د ذ ر ز و ى
static bool arabicJoinsLeft(int cp) {
    if (!isArabicBase(cp)) return (cp == 0x0640); // التطويل يتصل
    int idx = cp - 0x0621;
    if (idx < 0 || idx >= ARABIC_FORMS_COUNT) return false;
    // إذا كان له شكل بدائي فهو يتصل يساراً
    return ARABIC_FORMS[idx].initial != 0;
}

// هل هذا الحرف العربي يتصل بالحرف الذي قبله (يميناً في الكتابة)؟
// كل الحروف تتصل يميناً ما عدا الهمزة المنفردة
static bool arabicJoinsRight(int cp) {
    if (cp == 0x0621) return false; // الهمزة لا تتصل
    if (cp == 0x0640) return true;  // التطويل يتصل
    return isArabicBase(cp);
}

// ─── فك ترميز UTF-8 إلى مصفوفة نقاط كود ───
static std::vector<int> decodeUTF8ToCodepoints(const std::string& text) {
    std::vector<int> result;
    const unsigned char* str = (const unsigned char*)text.c_str();
    int i = 0, len = (int)text.size();
    while (i < len) {
        int cp;
        unsigned char c = str[i];
        if (c < 0x80) {
            cp = c; i += 1;
        } else if ((c & 0xE0) == 0xC0 && i + 1 < len) {
            cp = ((c & 0x1F) << 6) | (str[i+1] & 0x3F);
            i += 2;
        } else if ((c & 0xF0) == 0xE0 && i + 2 < len) {
            cp = ((c & 0x0F) << 12) | ((str[i+1] & 0x3F) << 6) | (str[i+2] & 0x3F);
            i += 3;
        } else if ((c & 0xF8) == 0xF0 && i + 3 < len) {
            cp = ((c & 0x07) << 18) | ((str[i+1] & 0x3F) << 12) |
                 ((str[i+2] & 0x3F) << 6) | (str[i+3] & 0x3F);
            i += 4;
        } else {
            i += 1;
            continue;
        }
        result.push_back(cp);
    }
    return result;
}

// ─── ترميز نقطة كود واحدة إلى UTF-8 ───
static std::string encodeCodepointUTF8(int cp) {
    std::string result;
    if (cp < 0x80) {
        result += (char)cp;
    } else if (cp < 0x800) {
        result += (char)(0xC0 | (cp >> 6));
        result += (char)(0x80 | (cp & 0x3F));
    } else if (cp < 0x10000) {
        result += (char)(0xE0 | (cp >> 12));
        result += (char)(0x80 | ((cp >> 6) & 0x3F));
        result += (char)(0x80 | (cp & 0x3F));
    } else {
        result += (char)(0xF0 | (cp >> 18));
        result += (char)(0x80 | ((cp >> 12) & 0x3F));
        result += (char)(0x80 | ((cp >> 6) & 0x3F));
        result += (char)(0x80 | (cp & 0x3F));
    }
    return result;
}

// ─── ترميز مصفوفة نقاط كود إلى سلسلة UTF-8 ───
static std::string encodeCodepointsUTF8(const std::vector<int>& codepoints) {
    std::string result;
    for (int cp : codepoints) {
        result += encodeCodepointUTF8(cp);
    }
    return result;
}

// هل النص يحتوي حروف عربية؟
static bool textHasArabic(const std::vector<int>& cps) {
    for (int cp : cps) {
        if (isArabicBase(cp)) return true;
    }
    return false;
}

// ─── تشكيل الحروف العربية ───
// تحليل كل حرف عربي واختيار شكله المناسب حسب جيرانه
// مع معالجة لام-ألف الإجبارية
static std::vector<int> shapeArabicText(const std::vector<int>& codepoints) {
    int n = (int)codepoints.size();
    if (n == 0) return codepoints;

    std::vector<int> result;
    result.reserve(n);

    for (int i = 0; i < n; i++) {
        int cp = codepoints[i];

        // تخطي الحروف غير العربية — تبقى كما هي
        if (!isArabicBase(cp) && cp != 0x0640) {
            result.push_back(cp);
            continue;
        }

        // ─── معالجة لام-ألف الإجبارية ───
        // إذا كان الحرف الحالي لام وبعده ألف (بأشكالها) → نُدمجهما في حرف واحد
        if (cp == 0x0644 && i + 1 < n) {
            int next = codepoints[i + 1];
            int ligature = 0;
            // تحديد نوع الألف ← شكل الدمج
            if      (next == 0x0622) ligature = 0xFEF5; // لآ
            else if (next == 0x0623) ligature = 0xFEF7; // لأ
            else if (next == 0x0625) ligature = 0xFEF9; // لإ
            else if (next == 0x0627) ligature = 0xFEFB; // لا

            if (ligature != 0) {
                // هل اللام متصلة من اليمين (بحرف سابق)؟
                bool joinedRight = false;
                // ابحث عن آخر حرف غير تشكيلي قبل هذا
                for (int j = i - 1; j >= 0; j--) {
                    if (!isArabicDiacritic(codepoints[j])) {
                        joinedRight = arabicJoinsLeft(codepoints[j]);
                        break;
                    }
                }
                // الشكل النهائي = الأساسي + 1
                result.push_back(joinedRight ? ligature + 1 : ligature);
                i++; // تخطي الألف (تم دمجها)
                continue;
            }
        }

        // ─── التطويل (كشيدة) ───
        if (cp == 0x0640) {
            result.push_back(0x0640);
            continue;
        }

        // ─── الحروف العادية ───
        int idx = cp - 0x0621;
        if (idx < 0 || idx >= ARABIC_FORMS_COUNT) {
            result.push_back(cp);
            continue;
        }

        const ArabicLetterForm& form = ARABIC_FORMS[idx];
        if (form.isolated == 0) {
            result.push_back(cp); // حرف بدون أشكال عرض
            continue;
        }

        // تحديد السياق: هل الحرف متصل من اليمين؟ هل متصل من اليسار؟

        // البحث عن الحرف السابق (تخطي التشكيل)
        bool prevJoinsToMe = false;
        for (int j = i - 1; j >= 0; j--) {
            if (!isArabicDiacritic(codepoints[j])) {
                prevJoinsToMe = arabicJoinsLeft(codepoints[j]);
                break;
            }
        }

        // البحث عن الحرف التالي (تخطي التشكيل)
        bool nextJoinsFromMe = false;
        for (int j = i + 1; j < n; j++) {
            if (!isArabicDiacritic(codepoints[j])) {
                nextJoinsFromMe = arabicJoinsRight(codepoints[j]);
                break;
            }
        }

        // هل هذا الحرف يمكنه الاتصال يساراً؟
        bool canJoinLeft = (form.initial != 0);

        // اختيار الشكل المناسب:
        //   وسطي = متصل يميناً + متصل يساراً
        //   نهائي = متصل يميناً فقط
        //   بدائي = متصل يساراً فقط
        //   منفصل = غير متصل
        if (prevJoinsToMe && canJoinLeft && nextJoinsFromMe) {
            result.push_back(form.medial);    // وسطي
        } else if (prevJoinsToMe) {
            result.push_back(form.final_);    // نهائي
        } else if (canJoinLeft && nextJoinsFromMe) {
            result.push_back(form.initial);   // بدائي
        } else {
            result.push_back(form.isolated);  // منفصل
        }
    }

    return result;
}

// ─── خوارزمية ثنائية الاتجاه المبسطة (Simplified BiDi) ───
// تعكس ترتيب الحروف للعرض من اليمين لليسار مع الحفاظ على اتجاه الأرقام والإنجليزية
//
// الخوارزمية:
//   1) عكس كل المصفوفة (الحرف الأول يصبح الأخير)
//   2) البحث عن مقاطع LTR (أرقام وإنجليزية) وعكسها مرة ثانية
//      (العكس المزدوج يُبقيها بترتيبها الأصلي)
static std::vector<int> bidiReorderRTL(const std::vector<int>& shaped) {
    // الخطوة 1: عكس كامل
    std::vector<int> visual(shaped.rbegin(), shaped.rend());

    // الخطوة 2: البحث عن مقاطع LTR وعكسها مرة ثانية
    int n = (int)visual.size();
    int i = 0;
    while (i < n) {
        // هل هذا حرف LTR (لاتيني أو رقم)؟
        auto isLTRChar = [](int cp) -> bool {
            return (cp >= '0' && cp <= '9') ||           // أرقام غربية
                   (cp >= 'A' && cp <= 'Z') ||           // حروف إنجليزية كبيرة
                   (cp >= 'a' && cp <= 'z') ||           // حروف إنجليزية صغيرة
                   (cp >= 0x0660 && cp <= 0x0669) ||     // أرقام عربية-هندية
                   (cp >= 0x06F0 && cp <= 0x06F9) ||     // أرقام فارسية
                   cp == '.' || cp == ',' ||             // فواصل عشرية
                   cp == '+' || cp == '-' ||             // إشارات
                   cp == '/' || cp == ':' ||             // فواصل تاريخ/وقت
                   cp == '@' || cp == '#' ||             // رموز
                   cp == '$' || cp == '%';               // رموز مالية
        };

        if (isLTRChar(visual[i])) {
            // وجدنا بداية مقطع LTR — نبحث عن نهايته
            int start = i;
            while (i < n && isLTRChar(visual[i])) {
                i++;
            }
            // عكس المقطع (عكس العكس = الترتيب الأصلي)
            std::reverse(visual.begin() + start, visual.begin() + i);
        } else {
            i++;
        }
    }

    return visual;
}

// ─── الدالة الرئيسية: معالجة النص العربي للعرض ───
// تأخذ النص الأصلي (UTF-8) وتُرجع نصاً مُشكَّلاً ومُرتباً بصرياً
// إذا لم يحتوِ النص على عربية، يُعاد كما هو بدون تغيير
struct ProcessedText {
    std::string text;       // النص المُعالج (UTF-8)
    bool isRTL;             // هل النص من اليمين لليسار؟
};

static ProcessedText processArabicText(const std::string& input) {
    ProcessedText result;
    result.isRTL = false;

    // فك ترميز UTF-8
    std::vector<int> codepoints = decodeUTF8ToCodepoints(input);

    // فحص وجود حروف عربية
    if (!textHasArabic(codepoints)) {
        result.text = input;
        return result;
    }

    result.isRTL = true;

    // الخطوة 1: تشكيل الحروف (اختيار الأشكال السياقية)
    std::vector<int> shaped = shapeArabicText(codepoints);

    // الخطوة 2: إعادة ترتيب ثنائية الاتجاه (عكس + حفظ LTR)
    std::vector<int> visual = bidiReorderRTL(shaped);

    // الخطوة 3: ترميز النتيجة إلى UTF-8
    result.text = encodeCodepointsUTF8(visual);

    return result;
}

// قياس عرض النص بـ TrueType
// يُشكّل النص العربي أولاً لقياس عرض الأشكال السياقية الصحيحة
static int measureTextWidthTTF(const std::string& text, int pixelSize) {
    if (!g_font.loaded || text.empty()) return 0;
    
    // معالجة النص العربي — التشكيل يغيّر الحروف فيتغير العرض
    ProcessedText processed = processArabicText(text);
    const std::string& processedStr = processed.text;
    
    float scale = stbtt_ScaleForPixelHeight(&g_font.fontInfo, (float)pixelSize);
    int totalWidth = 0;
    const unsigned char* str = (const unsigned char*)processedStr.c_str();
    int i = 0, len = (int)processedStr.size();
    while (i < len) {
        int codepoint;
        unsigned char c = str[i];
        if (c < 0x80) { codepoint = c; i += 1; }
        else if ((c & 0xE0) == 0xC0) { codepoint = (c & 0x1F) << 6 | (str[i+1] & 0x3F); i += 2; }
        else if ((c & 0xF0) == 0xE0) { codepoint = (c & 0x0F) << 12 | (str[i+1] & 0x3F) << 6 | (str[i+2] & 0x3F); i += 3; }
        else if ((c & 0xF8) == 0xF0) { codepoint = (c & 0x07) << 18 | (str[i+1] & 0x3F) << 12 | (str[i+2] & 0x3F) << 6 | (str[i+3] & 0x3F); i += 4; }
        else { i += 1; continue; }

        int advW, lsb;
        stbtt_GetCodepointHMetrics(&g_font.fontInfo, codepoint, &advW, &lsb);
        totalWidth += (int)(advW * scale);
    }
    return totalWidth;
}

// ─── الدوال العامة (تستخدم TTF إن توفر، وإلا النقطي) ───

// رسم حرف واحد بالخط النقطي (احتياطي)
static void drawCharBitmap(SDL_Renderer* r, char c, int x, int y, int scale, RGBA color) {
    int idx = (int)(unsigned char)c - 32;
    if (idx < 0 || idx >= 96) return;
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    const unsigned char* glyph = FONT_DATA[idx];
    for (int row = 0; row < 7; row++) {
        unsigned char bits = glyph[row];
        for (int col = 0; col < 5; col++) {
            if (bits & (1 << (4 - col))) {
                SDL_Rect px = {x + col * scale, y + row * scale, scale, scale};
                SDL_RenderFillRect(r, &px);
            }
        }
    }
}

// قياس عرض النص (HD أو نقطي)
static int measureTextWidth(const std::string& text, int scaleOrPixelSize) {
    if (text.empty()) return 0;
    if (g_font.loaded) {
        return measureTextWidthTTF(text, scaleOrPixelSize);
    }
    // النظام النقطي الاحتياطي
    return (int)text.size() * 6 * scaleOrPixelSize;
}

// ارتفاع النص
static int measureTextHeight(int scaleOrPixelSize) {
    if (g_font.loaded) return scaleOrPixelSize; // TTF: الارتفاع = حجم البكسل
    return 7 * scaleOrPixelSize; // نقطي: 7 صفوف × المقياس
}

// رسم نص كامل بنظام HD TrueType
// يعالج النص العربي تلقائياً: تشكيل + عكس + محاذاة يمينية
static void drawTextTTF(SDL_Renderer* renderer, const std::string& text,
                        int x, int y, int pixelSize, RGBA color,
                        const std::string& align = "left", int containerWidth = 0) {
    // معالجة النص العربي — تشكيل الحروف وعكس الاتجاه
    ProcessedText processed = processArabicText(text);
    const std::string& renderText = processed.text;
    
    int textW = 0;
    // حساب عرض النص المُعالج
    {
        float sc = stbtt_ScaleForPixelHeight(&g_font.fontInfo, (float)pixelSize);
        const unsigned char* s = (const unsigned char*)renderText.c_str();
        int ii = 0, ll = (int)renderText.size();
        while (ii < ll) {
            int cp2;
            unsigned char cc = s[ii];
            if (cc < 0x80) { cp2 = cc; ii += 1; }
            else if ((cc & 0xE0) == 0xC0) { cp2 = (cc & 0x1F) << 6 | (s[ii+1] & 0x3F); ii += 2; }
            else if ((cc & 0xF0) == 0xE0) { cp2 = (cc & 0x0F) << 12 | (s[ii+1] & 0x3F) << 6 | (s[ii+2] & 0x3F); ii += 3; }
            else if ((cc & 0xF8) == 0xF0) { cp2 = (cc & 0x07) << 18 | (s[ii+1] & 0x3F) << 12 | (s[ii+2] & 0x3F) << 6 | (s[ii+3] & 0x3F); ii += 4; }
            else { ii += 1; continue; }
            int advW2, lsb2;
            stbtt_GetCodepointHMetrics(&g_font.fontInfo, cp2, &advW2, &lsb2);
            textW += (int)(advW2 * sc);
        }
    }
    
    // حساب موضع البداية مع المحاذاة
    // للنص العربي: المحاذاة الافتراضية تصبح يميناً بدل اليسار
    std::string effectiveAlign = align;
    if (processed.isRTL && align == "left") {
        effectiveAlign = "right"; // النص العربي يبدأ من اليمين
    }
    
    int startX = x;
    if (effectiveAlign == "center" && containerWidth > 0)
        startX = x + (containerWidth - textW) / 2;
    else if (effectiveAlign == "right" && containerWidth > 0)
        startX = x + containerWidth - textW;

    // حساب خط الأساس (baseline)
    float scale = stbtt_ScaleForPixelHeight(&g_font.fontInfo, (float)pixelSize);
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&g_font.fontInfo, &ascent, &descent, &lineGap);
    int baseline = (int)(ascent * scale);

    // رسم الحروف — النص المُعالج بترتيبه البصري الصحيح
    int cursorX = startX;
    const unsigned char* str = (const unsigned char*)renderText.c_str();
    int i = 0, len = (int)renderText.size();
    while (i < len) {
        int codepoint;
        unsigned char c = str[i];
        if (c < 0x80) { codepoint = c; i += 1; }
        else if ((c & 0xE0) == 0xC0) { codepoint = (c & 0x1F) << 6 | (str[i+1] & 0x3F); i += 2; }
        else if ((c & 0xF0) == 0xE0) { codepoint = (c & 0x0F) << 12 | (str[i+1] & 0x3F) << 6 | (str[i+2] & 0x3F); i += 3; }
        else if ((c & 0xF8) == 0xF0) { codepoint = (c & 0x07) << 18 | (str[i+1] & 0x3F) << 12 | (str[i+2] & 0x3F) << 6 | (str[i+3] & 0x3F); i += 4; }
        else { i += 1; continue; }

        GlyphInfo* gi = getGlyph(renderer, codepoint, pixelSize);
        if (gi && gi->texture) {
            SDL_SetTextureColorMod(gi->texture, color.r, color.g, color.b);
            SDL_SetTextureAlphaMod(gi->texture, color.a);
            SDL_Rect dst = {cursorX + gi->xoff, y + baseline + gi->yoff, gi->width, gi->height};
            SDL_RenderCopy(renderer, gi->texture, nullptr, &dst);
            cursorX += gi->advance;
        } else if (gi) {
            cursorX += gi->advance;
        }
    }
}

// رسم نص كامل مع محاذاة (يختار تلقائياً بين HD و نقطي)
// align: "left" (يسار)، "center" (وسط)، "right" (يمين)
static void drawText(SDL_Renderer* r, const std::string& text, int x, int y,
                     int scaleOrPixelSize, RGBA color, const std::string& align = "left",
                     int containerWidth = 0) {
    if (g_font.loaded) {
        // نظام HD TrueType
        drawTextTTF(r, text, x, y, scaleOrPixelSize, color, align, containerWidth);
    } else {
        // النظام النقطي الاحتياطي
        int textW = (int)text.size() * 6 * scaleOrPixelSize;
        int startX = x;
        if (align == "center" && containerWidth > 0)
            startX = x + (containerWidth - textW) / 2;
        else if (align == "right" && containerWidth > 0)
            startX = x + containerWidth - textW;
        for (size_t i = 0; i < text.size(); i++)
            drawCharBitmap(r, text[i], startX + (int)i * 6 * scaleOrPixelSize, y, scaleOrPixelSize, color);
    }
}

// ═══════════════════════════════════════════════════════════════════
// القسم 5: رسم الأشكال الأساسية
// ─────────────────────────────────────────────────────────────────
// مستطيل دائري الزوايا، حدود، دائرة، خطوط
// ═══════════════════════════════════════════════════════════════════

// رسم مستطيل دائري الزوايا — يُستخدم للأزرار والبطاقات
static void drawRoundedRect(SDL_Renderer* r, int x, int y, int w, int h,
                            int radius, RGBA color) {
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    if (radius <= 0 || radius > std::min(w/2, h/2)) {
        SDL_Rect rect = {x, y, w, h};
        SDL_RenderFillRect(r, &rect);
        return;
    }
    radius = std::min(radius, std::min(w/2, h/2));
    // ثلاث مستطيلات مركزية تشكّل الجسم الرئيسي
    SDL_Rect rects[3] = {
        {x + radius, y, w - 2*radius, h},           // الشريط العمودي الوسط
        {x, y + radius, radius, h - 2*radius},       // الشريط الأيسر
        {x + w - radius, y + radius, radius, h - 2*radius} // الشريط الأيمن
    };
    for (auto& rect : rects) SDL_RenderFillRect(r, &rect);
    // رسم الزوايا الأربع كأرباع دوائر
    int cx[4] = {x+radius, x+w-radius-1, x+radius, x+w-radius-1};
    int cy[4] = {y+radius, y+radius, y+h-radius-1, y+h-radius-1};
    for (int corner = 0; corner < 4; corner++) {
        for (int dy = -radius; dy <= radius; dy++) {
            for (int dx = -radius; dx <= radius; dx++) {
                if (dx*dx + dy*dy <= radius*radius) {
                    bool valid = false;
                    if (corner==0 && dx<=0 && dy<=0) valid=true;
                    if (corner==1 && dx>=0 && dy<=0) valid=true;
                    if (corner==2 && dx<=0 && dy>=0) valid=true;
                    if (corner==3 && dx>=0 && dy>=0) valid=true;
                    if (valid) SDL_RenderDrawPoint(r, cx[corner]+dx, cy[corner]+dy);
                }
            }
        }
    }
}

// رسم حدود مستطيل (بدون تعبئة)
static void drawRectBorder(SDL_Renderer* r, int x, int y, int w, int h,
                           int thickness, RGBA color) {
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    for (int i = 0; i < thickness; i++) {
        SDL_Rect rect = {x+i, y+i, w-2*i, h-2*i};
        SDL_RenderDrawRect(r, &rect);
    }
}

// رسم دائرة ممتلئة (للأيقونات والأفاتار ومؤشرات الحالة)
static void drawFilledCircle(SDL_Renderer* r, int cx, int cy, int radius, RGBA color) {
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    for (int dy = -radius; dy <= radius; dy++)
        for (int dx = -radius; dx <= radius; dx++)
            if (dx*dx + dy*dy <= radius*radius)
                SDL_RenderDrawPoint(r, cx+dx, cy+dy);
}

// رسم خط أفقي
static void drawHLine(SDL_Renderer* r, int x, int y, int w, RGBA color) {
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(r, x, y, x+w, y);
}

// ═══════════════════════════════════════════════════════════════════
// القسم 6: حساب التخطيط (Layout) — تخطيط تلقائي تكراري
// ─────────────────────────────────────────────────────────────────
// يحسب موضع وحجم كل عنصر بناءً على نوعه:
//   Column: يوزّع الأبناء عمودياً (من أعلى لأسفل)
//   Row: يوزّع الأبناء أفقياً (من يسار ليمين)
//   Stack: يضع الأبناء فوق بعض
//   GridView: شبكة بأعمدة محددة
//   Center: يضع الابن في المنتصف
// ═══════════════════════════════════════════════════════════════════

// تقدير ارتفاع عنصر بدون أبناء (أوراق الشجرة)
static float estimateLeafHeight(Widget* child) {
    int scale = g_font.loaded ? (int)child->fontSize : std::max(1, (int)(child->fontSize / 7));
    float th = (float)measureTextHeight(scale);
    switch (child->type) {
        case WidgetType::Text: case WidgetType::Label: case WidgetType::Heading:
        case WidgetType::Paragraph: case WidgetType::Link: case WidgetType::Quote:
        case WidgetType::Markdown: case WidgetType::CodeBlock:
            return th + 12;
        case WidgetType::Button: case WidgetType::OutlineButton:
        case WidgetType::TextButton: case WidgetType::ToggleButton:
        case WidgetType::IconButton: case WidgetType::FloatingButton:
            return th + 30;
        case WidgetType::TextField: case WidgetType::SearchField:
        case WidgetType::PasswordField: case WidgetType::TextArea:
        case WidgetType::NumberInput: case WidgetType::DateField:
            return (child->type == WidgetType::TextArea) ? th * 4 + 20 : th + 20;
        case WidgetType::Checkbox: case WidgetType::Radio:
        case WidgetType::Switch:
            return std::max(th + 8, 30.0f);
        case WidgetType::Slider: case WidgetType::RangeSlider:
            return 40;
        case WidgetType::Dropdown:
            return th + 24;
        case WidgetType::ProgressBar: return 24;
        case WidgetType::CircleProgress: return 60;
        case WidgetType::LoadingSpinner: return 40;
        case WidgetType::Skeleton: return 20;
        case WidgetType::Divider: case WidgetType::Separator: return 2;
        case WidgetType::Spacer: return (child->fixedHeight > 0) ? child->fixedHeight : 8;
        case WidgetType::ListTile: case WidgetType::DetailRow:
        case WidgetType::NavItem: case WidgetType::MenuItem:
            return th + 24;
        case WidgetType::Avatar: return 48;
        case WidgetType::Badge: return 24;
        case WidgetType::Chip: case WidgetType::Tag: return th + 12;
        case WidgetType::StatusDot: return 16;
        case WidgetType::ColorBox: case WidgetType::GradientBox: return 40;
        case WidgetType::RatingStars: return 30;
        case WidgetType::DotIndicator: return 20;
        case WidgetType::StepIndicator: return 40;
        case WidgetType::Tooltip: return th + 16;
        case WidgetType::Gauge: return 80;
        case WidgetType::SparkLine: return 40;
        default: return 40;
    }
}

// تقدير عرض عنصر ورقي
static float estimateLeafWidth(Widget* child) {
    int scale = g_font.loaded ? (int)child->fontSize : std::max(1, (int)(child->fontSize / 7));
    float tw = (float)measureTextWidth(child->text, scale);
    switch (child->type) {
        case WidgetType::Button: case WidgetType::OutlineButton:
        case WidgetType::TextButton: case WidgetType::ToggleButton:
        case WidgetType::Text: case WidgetType::Label:
            return tw + 24;
        default: return 40;
    }
}

// الدالة الرئيسية لحساب التخطيط — تُستدعى تكرارياً
static void layoutWidget(int widgetId, float x, float y, float availW, float availH) {
    Widget* w = getWidget(widgetId);
    if (!w || !w->visible) return;

    float myW = (w->fixedWidth > 0) ? w->fixedWidth : availW;
    float myH = (w->fixedHeight > 0) ? w->fixedHeight : availH;
    w->cx = x; w->cy = y; w->cw = myW; w->ch = myH;

    // حساب الحشوة (padding) — مسافة داخلية
    float pT = (w->padding > 0) ? w->padding : w->padTop;
    float pR = (w->padding > 0) ? w->padding : w->padRight;
    float pB = (w->padding > 0) ? w->padding : w->padBottom;
    float pL = (w->padding > 0) ? w->padding : w->padLeft;
    float iX = x+pL, iY = y+pT;
    float iW = myW-pL-pR, iH = myH-pT-pB;

    // إذا لم يكن له أبناء → حساب حجم تلقائي
    if (w->children.empty()) {
        if (w->fixedHeight < 0) w->ch = estimateLeafHeight(w) + pT + pB;
        return;
    }

    // ─── تخطيط عمودي (Column) ───
    if (w->type == WidgetType::Column || w->type == WidgetType::ListView ||
        w->type == WidgetType::ScrollView || w->type == WidgetType::Card ||
        w->type == WidgetType::Panel || w->type == WidgetType::GroupBox ||
        w->type == WidgetType::InfoCard || w->type == WidgetType::Accordion ||
        w->type == WidgetType::ExpansionPanel || w->type == WidgetType::Drawer ||
        w->type == WidgetType::SideMenu || w->type == WidgetType::Modal ||
        w->type == WidgetType::Dialog || w->type == WidgetType::ConfirmDialog) {
        float fixedH = 0, flexSum = 0;
        int cnt = 0;
        // المرور الأول: حساب المساحة الثابتة ومجموع flex
        for (int cid : w->children) {
            Widget* c = getWidget(cid);
            if (!c || !c->visible) continue;
            cnt++;
            if (c->flex > 0) flexSum += c->flex;
            else if (c->fixedHeight > 0) fixedH += c->fixedHeight;
            else fixedH += estimateLeafHeight(c);
        }
        float gaps = (cnt > 1) ? (cnt-1) * w->spacing : 0;
        float remain = iH - fixedH - gaps;
        if (remain < 0) remain = 0;
        // المرور الثاني: تخصيص المواضع والأحجام
        float curY = iY;
        for (int cid : w->children) {
            Widget* c = getWidget(cid);
            if (!c || !c->visible) continue;
            float ch;
            if (c->flex > 0 && flexSum > 0) ch = remain * (c->flex / flexSum);
            else if (c->fixedHeight > 0) ch = c->fixedHeight;
            else ch = estimateLeafHeight(c);
            layoutWidget(cid, iX, curY, iW, ch);
            curY += c->ch + w->spacing;
        }
        if (w->fixedHeight < 0 && !w->children.empty())
            w->ch = curY - iY - w->spacing + pT + pB;
    }
    // ─── تخطيط أفقي (Row) ───
    else if (w->type == WidgetType::Row || w->type == WidgetType::ButtonGroup ||
             w->type == WidgetType::ToggleGroup || w->type == WidgetType::Toolbar ||
             w->type == WidgetType::Breadcrumb) {
        float fixedW = 0, flexSum = 0;
        int cnt = 0;
        for (int cid : w->children) {
            Widget* c = getWidget(cid);
            if (!c || !c->visible) continue;
            cnt++;
            if (c->flex > 0) flexSum += c->flex;
            else if (c->fixedWidth > 0) fixedW += c->fixedWidth;
            else fixedW += estimateLeafWidth(c);
        }
        float gaps = (cnt > 1) ? (cnt-1) * w->spacing : 0;
        float remain = iW - fixedW - gaps;
        if (remain < 0) remain = 0;
        float curX = iX;
        for (int cid : w->children) {
            Widget* c = getWidget(cid);
            if (!c || !c->visible) continue;
            float cw;
            if (c->flex > 0 && flexSum > 0) cw = remain * (c->flex / flexSum);
            else if (c->fixedWidth > 0) cw = c->fixedWidth;
            else cw = estimateLeafWidth(c);
            layoutWidget(cid, curX, iY, cw, iH);
            curX += c->cw + w->spacing;
        }
    }
    // ─── تخطيط مكدّس (Stack) — الأبناء فوق بعض ───
    else if (w->type == WidgetType::Stack) {
        for (int cid : w->children) layoutWidget(cid, iX, iY, iW, iH);
    }
    // ─── توسيط (Center) ───
    else if (w->type == WidgetType::Center) {
        for (int cid : w->children) {
            Widget* c = getWidget(cid);
            if (!c || !c->visible) continue;
            float cw = (c->fixedWidth > 0) ? c->fixedWidth : iW * 0.5f;
            float ch = (c->fixedHeight > 0) ? c->fixedHeight : estimateLeafHeight(c);
            layoutWidget(cid, iX + (iW-cw)/2, iY + (iH-ch)/2, cw, ch);
        }
    }
    // ─── شبكة (GridView) ───
    else if (w->type == WidgetType::GridView || w->type == WidgetType::Wrap) {
        int cols = std::max(1, w->columns);
        float cellW = (iW - (cols-1)*w->spacing) / cols;
        float curX = iX, curY = iY;
        int col = 0;
        for (int cid : w->children) {
            Widget* c = getWidget(cid);
            if (!c || !c->visible) continue;
            float ch = (c->fixedHeight > 0) ? c->fixedHeight : estimateLeafHeight(c);
            layoutWidget(cid, curX, curY, cellW, ch);
            col++;
            if (col >= cols) { col = 0; curX = iX; curY += ch + w->spacing; }
            else curX += cellW + w->spacing;
        }
    }
    // ─── حاوية عامة — ابن واحد يملأ المساحة ───
    else {
        for (int cid : w->children)
            layoutWidget(cid, iX, iY, iW, iH);
    }
}

// ═══════════════════════════════════════════════════════════════════
// القسم 7: محرك الرسم (Renderer) — يرسم كل نوع عنصر
// ─────────────────────────────────────────────────────────────────
// يُستدعى تكرارياً لكل عنصر في الشجرة
// كل نوع عنصر له شكل مختلف (زر، حقل، مؤشر، رسم بياني...)
// ═══════════════════════════════════════════════════════════════════

static void renderWidget(SDL_Renderer* r, int widgetId) {
    Widget* w = getWidget(widgetId);
    if (!w || !w->visible) return;
    int x=(int)w->cx, y=(int)w->cy, ww=(int)w->cw, hh=(int)w->ch;
    // HD: fontSize يُستخدم مباشرة كحجم بكسلات TrueType
    // نقطي احتياطي: يُحوَّل لمقياس (scale = fontSize/7)
    int scale = g_font.loaded ? (int)w->fontSize : std::max(1, (int)(w->fontSize / 7));

    // رسم الخلفية (إن وجدت)
    if (w->bgColor.a > 0)
        drawRoundedRect(r, x, y, ww, hh, (int)w->borderRadius, w->bgColor);
    // رسم الحدود (إن وجدت)
    if (w->borderWidth > 0)
        drawRectBorder(r, x, y, ww, hh, (int)w->borderWidth, w->borderColor);

    // ─── رسم حسب نوع العنصر ───
    switch (w->type) {

    // ─── نص عادي ───
    case WidgetType::Text: case WidgetType::Label: case WidgetType::Paragraph:
    case WidgetType::Markdown: {
        int tH = measureTextHeight(scale);
        int tY = y + (hh - tH) / 2;
        float pL = (w->padding > 0) ? w->padding : w->padLeft;
        drawText(r, w->text, x+(int)pL, tY, scale, w->textColor, w->textAlign, ww-(int)pL*2);
        break;
    }
    // ─── عنوان (بحجم خط أكبر حسب المستوى) ───
    case WidgetType::Heading: {
        int hScale = std::max(1, scale + (4 - std::min(w->level, 4)));
        int tH = measureTextHeight(hScale);
        int tY = y + (hh - tH) / 2;
        drawText(r, w->text, x+4, tY, hScale, w->textColor, w->textAlign, ww-8);
        break;
    }
    // ─── رابط (نص بلون أزرق) ───
    case WidgetType::Link: {
        int tH = measureTextHeight(scale);
        int tY = y + (hh - tH) / 2;
        RGBA linkC = {30, 100, 220, 255};
        drawText(r, w->text, x+4, tY, scale, linkC, w->textAlign, ww-8);
        // خط تحت النص
        int tw = measureTextWidth(w->text, scale);
        int lx = x + 4;
        if (w->textAlign == "center") lx = x + (ww - tw) / 2;
        drawHLine(r, lx, tY + tH + 1, tw, linkC);
        break;
    }
    // ─── اقتباس (شريط أيسر ملون) ───
    case WidgetType::Quote: {
        RGBA bar = {66, 133, 244, 255};
        SDL_SetRenderDrawColor(r, bar.r, bar.g, bar.b, bar.a);
        SDL_Rect qr = {x, y, 4, hh};
        SDL_RenderFillRect(r, &qr);
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+12, y + (hh-tH)/2, scale, {100,100,100,255}, "left", ww-16);
        break;
    }
    // ─── كتلة كود (خلفية داكنة) ───
    case WidgetType::CodeBlock: {
        drawRoundedRect(r, x, y, ww, hh, 4, {40,44,52,255});
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+8, y + (hh-tH)/2, scale, {180,220,180,255}, "left", ww-16);
        break;
    }
    // ─── زر عادي ───
    case WidgetType::Button: {
        RGBA bg = w->pressed ? w->btnPress : (w->hovered ? w->btnHover : w->btnColor);
        drawRoundedRect(r, x, y, ww, hh, w->borderRadius > 0 ? (int)w->borderRadius : 8, bg);
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x, y+(hh-tH)/2, scale, w->textColor, "center", ww);
        break;
    }
    // ─── زر مخطط (بدون تعبئة) ───
    case WidgetType::OutlineButton: {
        int rad = w->borderRadius > 0 ? (int)w->borderRadius : 8;
        if (w->hovered) drawRoundedRect(r, x, y, ww, hh, rad, {230,240,255,255});
        drawRectBorder(r, x, y, ww, hh, 2, w->btnColor);
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x, y+(hh-tH)/2, scale, w->btnColor, "center", ww);
        break;
    }
    // ─── زر نصي (بدون خلفية) ───
    case WidgetType::TextButton: {
        RGBA tc = w->hovered ? w->btnHover : w->btnColor;
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x, y+(hh-tH)/2, scale, tc, "center", ww);
        break;
    }
    // ─── زر تبديل (toggle) ───
    case WidgetType::ToggleButton: {
        RGBA bg = w->checked ? w->btnColor : RGBA{200,200,200,255};
        if (w->hovered) bg = w->checked ? w->btnHover : RGBA{220,220,220,255};
        drawRoundedRect(r, x, y, ww, hh, 8, bg);
        RGBA tc = w->checked ? RGBA{255,255,255,255} : RGBA{60,60,60,255};
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x, y+(hh-tH)/2, scale, tc, "center", ww);
        break;
    }
    // ─── زر أيقونة (دائري) ───
    case WidgetType::IconButton: {
        RGBA bg = w->pressed ? w->btnPress : (w->hovered ? RGBA{230,230,230,255} : RGBA{0,0,0,0});
        int rad = std::min(ww, hh) / 2;
        if (bg.a > 0) drawFilledCircle(r, x+ww/2, y+hh/2, rad, bg);
        char ic[2] = {w->iconChar, 0};
        int tH = measureTextHeight(scale);
        drawText(r, ic, x, y+(hh-tH)/2, scale, w->textColor, "center", ww);
        break;
    }
    // ─── زر عائم (FAB) ─── دائري ملون
    case WidgetType::FloatingButton: {
        RGBA bg = w->pressed ? w->btnPress : (w->hovered ? w->btnHover : w->btnColor);
        int rad = std::min(ww, hh) / 2;
        drawFilledCircle(r, x+ww/2, y+hh/2, rad, bg);
        char ic[2] = {w->iconChar, 0};
        int tH = measureTextHeight(scale);
        drawText(r, ic, x, y+(hh-tH)/2, scale, {255,255,255,255}, "center", ww);
        break;
    }
    // ─── حقل إدخال نص ───
    case WidgetType::TextField: case WidgetType::SearchField:
    case WidgetType::NumberInput: case WidgetType::DateField: {
        RGBA fbg = (w->bgColor.a > 0) ? w->bgColor : RGBA{255,255,255,255};
        drawRoundedRect(r, x, y, ww, hh, 4, fbg);
        RGBA bc = w->inputFocused ? RGBA{66,133,244,255} : RGBA{200,200,200,255};
        drawRectBorder(r, x, y, ww, hh, w->inputFocused ? 2 : 1, bc);
        int tH = measureTextHeight(scale);
        int tY = y + (hh-tH)/2;
        if (w->inputText.empty()) {
            drawText(r, w->text, x+8, tY, scale, {160,160,160,255}, "left", ww-16);
        } else {
            drawText(r, w->inputText, x+8, tY, scale, {33,33,33,255}, "left", ww-16);
        }
        // مؤشر الكتابة (يومض)
        if (w->inputFocused && (SDL_GetTicks()/500)%2==0) {
            int cx = x+8+measureTextWidth(w->inputText, scale);
            SDL_SetRenderDrawColor(r, 33, 33, 33, 255);
            SDL_RenderDrawLine(r, cx, tY, cx, tY+tH);
        }
        // أيقونة بحث للحقل البحثي
        if (w->type == WidgetType::SearchField) {
            drawFilledCircle(r, x+ww-20, y+hh/2, 6, {180,180,180,255});
        }
        break;
    }
    // ─── حقل كلمة مرور (نجوم بدل النص) ───
    case WidgetType::PasswordField: {
        drawRoundedRect(r, x, y, ww, hh, 4, {255,255,255,255});
        RGBA bc = w->inputFocused ? RGBA{66,133,244,255} : RGBA{200,200,200,255};
        drawRectBorder(r, x, y, ww, hh, w->inputFocused ? 2 : 1, bc);
        int tH = measureTextHeight(scale);
        int tY = y + (hh-tH)/2;
        if (w->inputText.empty()) {
            drawText(r, w->text, x+8, tY, scale, {160,160,160,255}, "left", ww-16);
        } else {
            std::string masked(w->inputText.size(), '*');
            drawText(r, masked, x+8, tY, scale, {33,33,33,255}, "left", ww-16);
        }
        break;
    }
    // ─── حقل نص متعدد الأسطر ───
    case WidgetType::TextArea: {
        drawRoundedRect(r, x, y, ww, hh, 4, {255,255,255,255});
        drawRectBorder(r, x, y, ww, hh, w->inputFocused ? 2 : 1,
                       w->inputFocused ? RGBA{66,133,244,255} : RGBA{200,200,200,255});
        int tH = measureTextHeight(scale);
        if (w->inputText.empty()) {
            drawText(r, w->text, x+8, y+8, scale, {160,160,160,255}, "left", ww-16);
        } else {
            drawText(r, w->inputText, x+8, y+8, scale, {33,33,33,255}, "left", ww-16);
        }
        break;
    }
    // ─── مربع اختيار (Checkbox) ───
    case WidgetType::Checkbox: {
        // مربع 18×18
        int boxSz = 18, bx = x+4, by = y+(hh-boxSz)/2;
        RGBA boxBg = w->checked ? RGBA{66,133,244,255} : RGBA{255,255,255,255};
        drawRoundedRect(r, bx, by, boxSz, boxSz, 3, boxBg);
        drawRectBorder(r, bx, by, boxSz, boxSz, 1, {150,150,150,255});
        if (w->checked) { // علامة صح ✓
            SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
            SDL_RenderDrawLine(r, bx+4, by+9, bx+7, by+13);
            SDL_RenderDrawLine(r, bx+7, by+13, bx+14, by+5);
        }
        // النص بجانب المربع
        int tH = measureTextHeight(scale);
        drawText(r, w->text, bx+boxSz+8, y+(hh-tH)/2, scale, w->textColor, "left", ww-boxSz-16);
        break;
    }
    // ─── زر دائري (Radio) ───
    case WidgetType::Radio: {
        int rad = 9, cx0 = x+4+rad, cy0 = y+hh/2;
        drawFilledCircle(r, cx0, cy0, rad, {255,255,255,255});
        drawFilledCircle(r, cx0, cy0, rad,
                        w->checked ? RGBA{66,133,244,255} : RGBA{200,200,200,255});
        drawFilledCircle(r, cx0, cy0, rad-2, {255,255,255,255});
        if (w->checked) drawFilledCircle(r, cx0, cy0, 5, {66,133,244,255});
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+4+rad*2+8, y+(hh-tH)/2, scale, w->textColor, "left", ww-rad*2-16);
        break;
    }
    // ─── مفتاح تبديل (Switch) ───
    case WidgetType::Switch: {
        int sw=40, sh=22, sx=x+4, sy=y+(hh-sh)/2;
        RGBA track = w->checked ? RGBA{66,133,244,255} : RGBA{180,180,180,255};
        drawRoundedRect(r, sx, sy, sw, sh, sh/2, track);
        int knobX = w->checked ? sx+sw-sh+2 : sx+2;
        drawFilledCircle(r, knobX+sh/2-2, sy+sh/2, sh/2-3, {255,255,255,255});
        int tH = measureTextHeight(scale);
        drawText(r, w->text, sx+sw+8, y+(hh-tH)/2, scale, w->textColor, "left", ww-sw-16);
        break;
    }
    // ─── منزلق (Slider) ───
    case WidgetType::Slider: {
        int trackH = 6, trackY = y+hh/2 - trackH/2;
        drawRoundedRect(r, x+4, trackY, ww-8, trackH, 3, {210,210,210,255});
        float pct = (w->maxVal > w->minVal) ? (w->value - w->minVal)/(w->maxVal - w->minVal) : 0;
        pct = std::max(0.0f, std::min(1.0f, pct));
        int fillW = (int)((ww-8) * pct);
        drawRoundedRect(r, x+4, trackY, fillW, trackH, 3, w->accentColor);
        int knobX = x + 4 + fillW;
        drawFilledCircle(r, knobX, y+hh/2, 8, w->accentColor);
        drawFilledCircle(r, knobX, y+hh/2, 5, {255,255,255,255});
        break;
    }
    // ─── منزلق نطاق (RangeSlider) ───
    case WidgetType::RangeSlider: {
        int trackH = 6, trackY = y+hh/2 - trackH/2;
        drawRoundedRect(r, x+4, trackY, ww-8, trackH, 3, {210,210,210,255});
        float range = w->maxVal - w->minVal;
        if (range <= 0) range = 1;
        float p1 = (w->value - w->minVal)/range, p2 = (w->value2 - w->minVal)/range;
        p1 = std::max(0.f, std::min(1.f, p1)); p2 = std::max(0.f, std::min(1.f, p2));
        int x1 = x+4+(int)((ww-8)*p1), x2 = x+4+(int)((ww-8)*p2);
        SDL_SetRenderDrawColor(r, w->accentColor.r, w->accentColor.g, w->accentColor.b, 255);
        SDL_Rect fill = {std::min(x1,x2), trackY, abs(x2-x1), trackH};
        SDL_RenderFillRect(r, &fill);
        drawFilledCircle(r, x1, y+hh/2, 8, w->accentColor);
        drawFilledCircle(r, x2, y+hh/2, 8, w->accentColor);
        break;
    }
    // ─── قائمة منسدلة (Dropdown) ───
    case WidgetType::Dropdown: {
        drawRoundedRect(r, x, y, ww, hh, 4, {255,255,255,255});
        drawRectBorder(r, x, y, ww, hh, 1, {200,200,200,255});
        int tH = measureTextHeight(scale);
        int tY = y+(hh-tH)/2;
        std::string display = w->text; // النص التلميحي
        if (w->selectedIndex >= 0 && w->selectedIndex < (int)w->items.size())
            display = w->items[w->selectedIndex];
        drawText(r, display, x+8, tY, scale, {33,33,33,255}, "left", ww-30);
        // سهم ▼
        drawText(r, "v", x+ww-20, tY, scale, {120,120,120,255});
        break;
    }
    // ─── شريط تقدم (ProgressBar) ───
    case WidgetType::ProgressBar: {
        drawRoundedRect(r, x, y+hh/2-4, ww, 8, 4, {220,220,220,255});
        float pct = std::max(0.f, std::min(1.f, w->value / std::max(1.f, w->maxVal)));
        int fillW = (int)(ww * pct);
        if (fillW > 0) drawRoundedRect(r, x, y+hh/2-4, fillW, 8, 4, w->accentColor);
        break;
    }
    // ─── مؤشر تقدم دائري ───
    case WidgetType::CircleProgress: {
        int rad = std::min(ww, hh)/2 - 4;
        int cx0 = x+ww/2, cy0 = y+hh/2;
        drawFilledCircle(r, cx0, cy0, rad, {220,220,220,255});
        drawFilledCircle(r, cx0, cy0, rad-4, {255,255,255,255});
        // نسبة التقدم
        float pct = std::max(0.f, std::min(1.f, w->value / std::max(1.f, w->maxVal)));
        std::string pStr = std::to_string((int)(pct*100)) + "%";
        int tH = measureTextHeight(scale);
        drawText(r, pStr, x, cy0-tH/2, scale, w->textColor, "center", ww);
        // قوس التقدم (تقريبي — رسم نقاط على المحيط)
        SDL_SetRenderDrawColor(r, w->accentColor.r, w->accentColor.g, w->accentColor.b, 255);
        int totalPts = (int)(360 * pct);
        for (int deg = -90; deg < -90+totalPts; deg++) {
            float ra = deg * 3.14159f / 180.0f;
            for (int t = 0; t < 4; t++) {
                int px = cx0 + (int)((rad-t) * cosf(ra));
                int py = cy0 + (int)((rad-t) * sinf(ra));
                SDL_RenderDrawPoint(r, px, py);
            }
        }
        break;
    }
    // ─── مؤشر تحميل دوّار ───
    case WidgetType::LoadingSpinner: {
        int cx0 = x+ww/2, cy0 = y+hh/2, rad = std::min(ww,hh)/2 - 4;
        float angle = w->animProgress;
        w->animProgress += 5.0f; if (w->animProgress > 360) w->animProgress = 0;
        SDL_SetRenderDrawColor(r, w->accentColor.r, w->accentColor.g, w->accentColor.b, 255);
        for (int deg = 0; deg < 90; deg++) {
            float ra = (angle + deg) * 3.14159f / 180.0f;
            for (int t = 0; t < 3; t++) {
                int px = cx0 + (int)((rad-t)*cosf(ra));
                int py = cy0 + (int)((rad-t)*sinf(ra));
                SDL_RenderDrawPoint(r, px, py);
            }
        }
        break;
    }
    // ─── هيكل عظمي (Skeleton) — مستطيل رمادي يومض ───
    case WidgetType::Skeleton: {
        int alpha = 150 + (int)(50 * sinf(SDL_GetTicks() * 0.003f));
        drawRoundedRect(r, x, y, ww, hh, 4, {210,210,210,(int)std::min(255,std::max(0,alpha))});
        break;
    }
    // ─── أيقونة (حرف في دائرة أو بمفرده) ───
    case WidgetType::Icon: {
        char ic[2] = {w->iconChar, 0};
        int tH = measureTextHeight(scale);
        drawText(r, ic, x, y+(hh-tH)/2, scale, w->textColor, "center", ww);
        break;
    }
    // ─── أفاتار (دائرة ملونة بحرف) ───
    case WidgetType::Avatar: {
        int rad = std::min(ww, hh)/2 - 2;
        drawFilledCircle(r, x+ww/2, y+hh/2, rad, w->btnColor);
        int tH = measureTextHeight(scale);
        std::string letter = w->text.empty() ? "?" : w->text.substr(0,1);
        drawText(r, letter, x, y+(hh-tH)/2, scale, {255,255,255,255}, "center", ww);
        break;
    }
    // ─── شارة (Badge) — دائرة حمراء بعدد ───
    case WidgetType::Badge: {
        int rad = std::min(ww, hh)/2;
        RGBA bg = (w->bgColor.a > 0) ? w->bgColor : RGBA{220,53,69,255};
        drawFilledCircle(r, x+ww/2, y+hh/2, rad, bg);
        std::string num = std::to_string((int)w->value);
        int tH = measureTextHeight(scale);
        drawText(r, num, x, y+(hh-tH)/2, scale, {255,255,255,255}, "center", ww);
        break;
    }
    // ─── رقاقة (Chip) — مستطيل دائري بنص ───
    case WidgetType::Chip: {
        RGBA bg = (w->bgColor.a > 0) ? w->bgColor : RGBA{230,230,230,255};
        drawRoundedRect(r, x, y, ww, hh, hh/2, bg);
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x, y+(hh-tH)/2, scale, w->textColor, "center", ww);
        break;
    }
    // ─── وسم (Tag) — مثل Chip بلون ───
    case WidgetType::Tag: {
        RGBA bg = (w->bgColor.a > 0) ? w->bgColor : w->btnColor;
        drawRoundedRect(r, x, y, ww, hh, 4, bg);
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x, y+(hh-tH)/2, scale, {255,255,255,255}, "center", ww);
        break;
    }
    // ─── نقطة حالة (StatusDot) ───
    case WidgetType::StatusDot: {
        RGBA c = (w->bgColor.a > 0) ? w->bgColor : RGBA{76,175,80,255};
        drawFilledCircle(r, x+ww/2, y+hh/2, std::min(ww,hh)/2-1, c);
        break;
    }
    // ─── صندوق لون (ColorBox) ───
    case WidgetType::ColorBox: {
        RGBA c = (w->bgColor.a > 0) ? w->bgColor : RGBA{100,100,200,255};
        drawRoundedRect(r, x, y, ww, hh, (int)w->borderRadius, c);
        break;
    }
    // ─── صندوق متدرج (تدرج أفقي بسيط) ───
    case WidgetType::GradientBox: {
        RGBA c1 = w->btnColor, c2 = w->accentColor;
        for (int col = 0; col < ww; col++) {
            float t = (float)col / std::max(1, ww-1);
            int cr = c1.r + (int)(t*(c2.r - c1.r));
            int cg = c1.g + (int)(t*(c2.g - c1.g));
            int cb = c1.b + (int)(t*(c2.b - c1.b));
            SDL_SetRenderDrawColor(r, cr, cg, cb, 255);
            SDL_RenderDrawLine(r, x+col, y, x+col, y+hh);
        }
        break;
    }
    // ─── صورة (عنصر نائب) ───
    case WidgetType::ImageWidget: {
        drawRoundedRect(r, x, y, ww, hh, 4, {220,220,230,255});
        int tH = measureTextHeight(scale);
        drawText(r, w->text.empty() ? "[IMG]" : w->text, x, y+(hh-tH)/2, scale, {130,130,140,255}, "center", ww);
        break;
    }
    // ─── فاصل (خط أفقي رفيع) ───
    case WidgetType::Divider: case WidgetType::Separator: {
        drawHLine(r, x, y+hh/2, ww, {220,220,220,255});
        break;
    }
    // ─── نجوم تقييم (RatingStars) ───
    case WidgetType::RatingStars: {
        int maxS = std::max(1, (int)w->maxVal);
        int starW = std::min(24, ww/maxS);
        for (int i = 0; i < maxS; i++) {
            RGBA sc = (i < (int)w->value) ? RGBA{255, 193, 7, 255} : RGBA{200, 200, 200, 255};
            drawText(r, "*", x + i*starW, y+(hh-measureTextHeight(scale))/2, scale, sc);
        }
        break;
    }
    // ─── شريط تطبيق (AppBar) ───
    case WidgetType::AppBar: {
        RGBA bg = (w->bgColor.a > 0) ? w->bgColor : RGBA{25,80,160,255};
        drawRoundedRect(r, x, y, ww, hh, 0, bg);
        int hScale = std::max(1, scale + 1);
        int tH = measureTextHeight(hScale);
        drawText(r, w->text, x+16, y+(hh-tH)/2, hScale, {255,255,255,255}, "left", ww-32);
        break;
    }
    // ─── شريط سفلي (BottomBar) ───
    case WidgetType::BottomBar: case WidgetType::StatusBar: {
        RGBA bg = (w->bgColor.a > 0) ? w->bgColor : RGBA{250,250,250,255};
        drawRoundedRect(r, x, y, ww, hh, 0, bg);
        drawHLine(r, x, y, ww, {220,220,220,255});
        if (w->type == WidgetType::StatusBar) {
            int tH = measureTextHeight(scale);
            drawText(r, w->text, x+8, y+(hh-tH)/2, scale, {100,100,100,255}, "left", ww-16);
        }
        break;
    }
    // ─── شريط تبويبات (TabBar) ───
    case WidgetType::TabBar: {
        RGBA bg = (w->bgColor.a > 0) ? w->bgColor : RGBA{250,250,250,255};
        drawRoundedRect(r, x, y, ww, hh, 0, bg);
        if (!w->items.empty()) {
            int tabW = ww / (int)w->items.size();
            for (int i = 0; i < (int)w->items.size(); i++) {
                int tH = measureTextHeight(scale);
                bool sel = (i == w->selectedIndex);
                RGBA tc = sel ? w->accentColor : RGBA{120,120,120,255};
                drawText(r, w->items[i], x+i*tabW, y+(hh-tH)/2, scale, tc, "center", tabW);
                if (sel) {
                    SDL_SetRenderDrawColor(r, w->accentColor.r, w->accentColor.g, w->accentColor.b, 255);
                    SDL_Rect ind = {x+i*tabW+tabW/4, y+hh-3, tabW/2, 3};
                    SDL_RenderFillRect(r, &ind);
                }
            }
        }
        break;
    }
    // ─── عنصر ملاحة (NavItem) / عنصر قائمة (MenuItem) ───
    case WidgetType::NavItem: case WidgetType::MenuItem: {
        RGBA bg = w->active ? RGBA{230,240,255,255} : (w->hovered ? RGBA{245,245,245,255} : RGBA{0,0,0,0});
        if (bg.a > 0) drawRoundedRect(r, x, y, ww, hh, 4, bg);
        RGBA tc = w->active ? RGBA{25,80,160,255} : w->textColor;
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+12, y+(hh-tH)/2, scale, tc, "left", ww-24);
        break;
    }
    // ─── تنبيه (Alert) ───
    case WidgetType::Alert: {
        // نوع التنبيه يحدد اللون
        RGBA bg = {253,237,237,255}; RGBA bc = {220,53,69,255}; // error
        if (w->subtitle == "success") { bg = {237,253,237,255}; bc = {40,167,69,255}; }
        else if (w->subtitle == "warning") { bg = {255,248,225,255}; bc = {255,193,7,255}; }
        else if (w->subtitle == "info") { bg = {227,242,253,255}; bc = {33,150,243,255}; }
        drawRoundedRect(r, x, y, ww, hh, 8, bg);
        SDL_SetRenderDrawColor(r, bc.r, bc.g, bc.b, bc.a);
        SDL_Rect lb = {x, y, 4, hh}; SDL_RenderFillRect(r, &lb);
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+12, y+(hh-tH)/2, scale, bc, "left", ww-20);
        break;
    }
    // ─── إشعار منبثق (Toast/Snackbar/Banner) ───
    case WidgetType::Toast: case WidgetType::Snackbar: case WidgetType::Banner: {
        RGBA bg = (w->type == WidgetType::Banner) ? RGBA{255,248,225,255} : RGBA{50,50,50,240};
        RGBA tc = (w->type == WidgetType::Banner) ? RGBA{100,80,0,255} : RGBA{255,255,255,255};
        drawRoundedRect(r, x, y, ww, hh, 8, bg);
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+12, y+(hh-tH)/2, scale, tc, "left", ww-24);
        break;
    }
    // ─── معلومات تفصيلية (ListTile / DetailRow) ───
    case WidgetType::ListTile: {
        if (w->hovered) drawRoundedRect(r, x, y, ww, hh, 0, {245,245,245,255});
        int tH = measureTextHeight(scale);
        int sScale = std::max(1, scale - 1);
        int sH = measureTextHeight(sScale);
        int totalH = tH + sH + 2;
        int tY = y + (hh - totalH) / 2;
        drawText(r, w->text, x+12, tY, scale, w->textColor, "left", ww-24);
        drawText(r, w->subtitle, x+12, tY+tH+2, sScale, {130,130,130,255}, "left", ww-24);
        break;
    }
    case WidgetType::DetailRow: {
        int tH = measureTextHeight(scale);
        int tY = y + (hh-tH)/2;
        drawText(r, w->text, x+8, tY, scale, {100,100,100,255}, "left", ww/2-8);
        drawText(r, w->subtitle, x+ww/2, tY, scale, w->textColor, "right", ww/2-8);
        break;
    }
    // ─── بطاقة معلومات / بطاقة إحصائية ───
    case WidgetType::InfoCard: case WidgetType::StatCard: {
        drawRoundedRect(r, x+2, y+2, ww, hh, 12, {0,0,0,25}); // ظل
        RGBA bg = (w->bgColor.a > 0) ? w->bgColor : RGBA{255,255,255,255};
        drawRoundedRect(r, x, y, ww, hh, 12, bg);
        int tH = measureTextHeight(scale);
        int bigScale = std::max(1, scale + 1);
        int bH = measureTextHeight(bigScale);
        if (w->type == WidgetType::StatCard) {
            // القيمة كبيرة في الأعلى، العنوان صغير تحت
            drawText(r, w->subtitle, x+12, y+12, bigScale, w->accentColor, "left", ww-24);
            drawText(r, w->text, x+12, y+12+bH+4, scale, {100,100,100,255}, "left", ww-24);
        } else {
            drawText(r, w->text, x+12, y+12, scale+1 > scale ? scale : scale, w->textColor, "left", ww-24);
            drawText(r, w->subtitle, x+12, y+12+tH+4, std::max(1,scale-1), {130,130,130,255}, "left", ww-24);
        }
        break;
    }
    // ─── لوحة قابلة للتوسيع (ExpansionPanel) ───
    case WidgetType::ExpansionPanel: {
        drawRoundedRect(r, x, y, ww, hh, 4, {255,255,255,255});
        drawRectBorder(r, x, y, ww, hh, 1, {220,220,220,255});
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+12, y+8, scale, w->textColor, "left", ww-40);
        // سهم توسيع/طي
        drawText(r, w->expanded ? "^" : "v", x+ww-24, y+8, scale, {120,120,120,255});
        break;
    }
    // ─── مجموعة (GroupBox) ───
    case WidgetType::GroupBox: case WidgetType::Panel: {
        drawRectBorder(r, x, y+8, ww, hh-8, 1, {200,200,200,255});
        // عنوان المجموعة فوق الحد
        if (!w->text.empty()) {
            int tW = measureTextWidth(w->text, scale);
            int tH = measureTextHeight(scale);
            SDL_SetRenderDrawColor(r, 245, 245, 245, 255); // مسح الخط تحت العنوان
            SDL_Rect clear = {x+8, y, tW+8, tH+4};
            SDL_RenderFillRect(r, &clear);
            drawText(r, w->text, x+12, y, scale, w->textColor, "left");
        }
        break;
    }
    // ─── ملاحظة (Callout) ───
    case WidgetType::Callout: {
        RGBA bg = {227,242,253,255};
        if (w->subtitle == "warning") bg = {255,248,225,255};
        else if (w->subtitle == "error") bg = {253,237,237,255};
        else if (w->subtitle == "success") bg = {237,253,237,255};
        drawRoundedRect(r, x, y, ww, hh, 8, bg);
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+12, y+(hh-tH)/2, scale, {50,50,50,255}, "left", ww-24);
        break;
    }
    // ─── رسم بياني عمودي (BarChart) ───
    case WidgetType::BarChart: {
        if (w->dataValues.empty()) break;
        float maxV = *std::max_element(w->dataValues.begin(), w->dataValues.end());
        if (maxV <= 0) maxV = 1;
        int n = (int)w->dataValues.size();
        int barW = std::max(4, (ww - 8) / n - 4);
        RGBA defColors[] = {{66,133,244,255},{234,67,53,255},{52,168,83,255},
                            {251,188,4,255},{171,71,188,255},{0,172,193,255}};
        for (int i = 0; i < n; i++) {
            float pct = w->dataValues[i] / maxV;
            int bH = (int)((hh-16) * pct);
            RGBA c = (i < (int)w->dataColors.size()) ? w->dataColors[i] : defColors[i%6];
            drawRoundedRect(r, x+4+i*(barW+4), y+hh-8-bH, barW, bH, 2, c);
        }
        break;
    }
    // ─── رسم بياني أفقي (HBarChart) ───
    case WidgetType::HBarChart: {
        if (w->dataValues.empty()) break;
        float maxV = *std::max_element(w->dataValues.begin(), w->dataValues.end());
        if (maxV <= 0) maxV = 1;
        int n = (int)w->dataValues.size();
        int barH = std::max(4, (hh - 8) / n - 4);
        RGBA defColors[] = {{66,133,244,255},{234,67,53,255},{52,168,83,255},{251,188,4,255}};
        for (int i = 0; i < n; i++) {
            float pct = w->dataValues[i] / maxV;
            int bW = (int)((ww-16) * pct);
            RGBA c = (i < (int)w->dataColors.size()) ? w->dataColors[i] : defColors[i%4];
            drawRoundedRect(r, x+8, y+4+i*(barH+4), bW, barH, 2, c);
        }
        break;
    }
    // ─── رسم بياني دائري / كعكة (PieChart/DonutChart) ───
    case WidgetType::PieChart: case WidgetType::DonutChart: {
        if (w->dataValues.empty()) break;
        float total = 0;
        for (float v : w->dataValues) total += v;
        if (total <= 0) break;
        int cx0 = x+ww/2, cy0 = y+hh/2;
        int rad = std::min(ww, hh)/2 - 4;
        RGBA defColors[] = {{66,133,244,255},{234,67,53,255},{52,168,83,255},
                            {251,188,4,255},{171,71,188,255},{0,172,193,255}};
        float startAngle = -90;
        for (int i = 0; i < (int)w->dataValues.size(); i++) {
            float sweep = 360 * (w->dataValues[i] / total);
            RGBA c = (i < (int)w->dataColors.size()) ? w->dataColors[i] : defColors[i%6];
            SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
            for (float deg = startAngle; deg < startAngle + sweep; deg += 0.5f) {
                float ra = deg * 3.14159f / 180.0f;
                int startR = (w->type == WidgetType::DonutChart) ? rad/2 : 0;
                for (int rr = startR; rr <= rad; rr++) {
                    SDL_RenderDrawPoint(r, cx0+(int)(rr*cosf(ra)), cy0+(int)(rr*sinf(ra)));
                }
            }
            startAngle += sweep;
        }
        break;
    }
    // ─── خط بياني (LineChart) ───
    case WidgetType::LineChart: {
        if (w->dataValues.size() < 2) break;
        float maxV = *std::max_element(w->dataValues.begin(), w->dataValues.end());
        float minV = *std::min_element(w->dataValues.begin(), w->dataValues.end());
        float range = maxV - minV; if (range <= 0) range = 1;
        int n = (int)w->dataValues.size();
        SDL_SetRenderDrawColor(r, w->accentColor.r, w->accentColor.g, w->accentColor.b, 255);
        for (int i = 1; i < n; i++) {
            float t0 = (float)(i-1)/(n-1), t1 = (float)i/(n-1);
            float v0 = (w->dataValues[i-1]-minV)/range, v1 = (w->dataValues[i]-minV)/range;
            int x0 = x+4+(int)(t0*(ww-8)), y0 = y+hh-4-(int)(v0*(hh-8));
            int x1 = x+4+(int)(t1*(ww-8)), y1 = y+hh-4-(int)(v1*(hh-8));
            SDL_RenderDrawLine(r, x0, y0, x1, y1);
            SDL_RenderDrawLine(r, x0, y0+1, x1, y1+1); // سمك 2
        }
        break;
    }
    // ─── خط مصغّر (SparkLine) ───
    case WidgetType::SparkLine: {
        if (w->dataValues.size() < 2) break;
        float maxV = *std::max_element(w->dataValues.begin(), w->dataValues.end());
        float minV = *std::min_element(w->dataValues.begin(), w->dataValues.end());
        float range = maxV - minV; if (range <= 0) range = 1;
        int n = (int)w->dataValues.size();
        SDL_SetRenderDrawColor(r, w->accentColor.r, w->accentColor.g, w->accentColor.b, 255);
        for (int i = 1; i < n; i++) {
            float t0 = (float)(i-1)/(n-1), t1 = (float)i/(n-1);
            float v0 = (w->dataValues[i-1]-minV)/range, v1 = (w->dataValues[i]-minV)/range;
            SDL_RenderDrawLine(r, x+(int)(t0*ww), y+hh-(int)(v0*hh), x+(int)(t1*ww), y+hh-(int)(v1*hh));
        }
        break;
    }
    // ─── عدّاد (Gauge) — نصف دائرة ───
    case WidgetType::Gauge: {
        int cx0 = x+ww/2, cy0 = y+hh-8;
        int rad = std::min(ww/2, hh) - 8;
        // خلفية القوس
        SDL_SetRenderDrawColor(r, 220, 220, 220, 255);
        for (float deg = 180; deg <= 360; deg += 0.5f) {
            float ra = deg * 3.14159f / 180.0f;
            for (int t = 0; t < 6; t++)
                SDL_RenderDrawPoint(r, cx0+(int)((rad-t)*cosf(ra)), cy0+(int)((rad-t)*sinf(ra)));
        }
        // قوس القيمة
        float pct = std::max(0.f, std::min(1.f, w->value / std::max(1.f, w->maxVal)));
        SDL_SetRenderDrawColor(r, w->accentColor.r, w->accentColor.g, w->accentColor.b, 255);
        for (float deg = 180; deg <= 180 + 180*pct; deg += 0.5f) {
            float ra = deg * 3.14159f / 180.0f;
            for (int t = 0; t < 6; t++)
                SDL_RenderDrawPoint(r, cx0+(int)((rad-t)*cosf(ra)), cy0+(int)((rad-t)*sinf(ra)));
        }
        std::string val = std::to_string((int)w->value);
        int tH = measureTextHeight(scale);
        drawText(r, val, x, cy0-tH-4, scale, w->textColor, "center", ww);
        break;
    }
    // ─── مؤشر نقطي (DotIndicator) ───
    case WidgetType::DotIndicator: {
        int total = std::max(1, (int)w->maxVal);
        int current = std::max(0, std::min((int)w->value, total-1));
        int dotSz = 8, gap = 6;
        int totalW = total * dotSz + (total-1) * gap;
        int startX = x + (ww - totalW) / 2;
        for (int i = 0; i < total; i++) {
            RGBA dc = (i == current) ? w->accentColor : RGBA{200,200,200,255};
            drawFilledCircle(r, startX + i*(dotSz+gap) + dotSz/2, y+hh/2, dotSz/2, dc);
        }
        break;
    }
    // ─── مؤشر خطوات (StepIndicator) ───
    case WidgetType::StepIndicator: {
        int total = std::max(1, (int)w->maxVal);
        int current = std::max(0, std::min((int)w->value, total));
        int stepW = ww / total;
        for (int i = 0; i < total; i++) {
            RGBA c = (i < current) ? w->accentColor : RGBA{200,200,200,255};
            drawFilledCircle(r, x+i*stepW+stepW/2, y+hh/2, 10, c);
            std::string num = std::to_string(i+1);
            int tH = measureTextHeight(2);
            drawText(r, num, x+i*stepW, y+(hh-tH)/2, 2, {255,255,255,255}, "center", stepW);
            if (i < total-1) {
                RGBA lc = (i < current-1) ? w->accentColor : RGBA{200,200,200,255};
                drawHLine(r, x+i*stepW+stepW/2+12, y+hh/2, stepW-24, lc);
            }
        }
        break;
    }
    // ─── ترقيم صفحات (Pagination) ───
    case WidgetType::Pagination: {
        int total = std::max(1, w->totalPages);
        int btnW = 30, gap = 4;
        int totalW = total * btnW + (total-1) * gap;
        int startX = x + (ww - totalW) / 2;
        for (int i = 0; i < total; i++) {
            bool cur = (i == w->currentPage);
            RGBA bg = cur ? w->accentColor : RGBA{230,230,230,255};
            RGBA tc = cur ? RGBA{255,255,255,255} : RGBA{60,60,60,255};
            drawRoundedRect(r, startX+i*(btnW+gap), y+(hh-btnW)/2, btnW, btnW, 4, bg);
            std::string num = std::to_string(i+1);
            int tH = measureTextHeight(2);
            drawText(r, num, startX+i*(btnW+gap), y+(hh-tH)/2, 2, tc, "center", btnW);
        }
        break;
    }
    // ─── تسلسل زمني (Timeline) ───
    case WidgetType::Timeline: {
        // خط عمودي في اليسار
        SDL_SetRenderDrawColor(r, 200, 200, 200, 255);
        SDL_RenderDrawLine(r, x+12, y, x+12, y+hh);
        break;
    }
    case WidgetType::TimelineItem: {
        drawFilledCircle(r, x+12, y+12, 6, w->accentColor);
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+28, y+(hh-tH)/2, scale, w->textColor, "left", ww-36);
        SDL_SetRenderDrawColor(r, 200, 200, 200, 255);
        SDL_RenderDrawLine(r, x+12, y, x+12, y+hh);
        break;
    }
    // ─── عنصر شجرة (TreeItem) ───
    case WidgetType::TreeItem: {
        int indent = w->level * 20;
        if (w->hovered) drawRoundedRect(r, x, y, ww, hh, 0, {245,245,245,255});
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+indent+8, y+(hh-tH)/2, scale, w->textColor, "left", ww-indent-16);
        break;
    }
    // ─── تلميح (Tooltip) ───
    case WidgetType::Tooltip: {
        drawRoundedRect(r, x, y, ww, hh, 6, {50,50,50,230});
        int tH = measureTextHeight(std::max(1, scale-1));
        drawText(r, w->text, x+8, y+(hh-tH)/2, std::max(1,scale-1), {255,255,255,255}, "center", ww-16);
        break;
    }
    // ─── مُنتقي ألوان (مبسّط — شبكة ألوان) ───
    case WidgetType::ColorPicker: {
        int cols = 8, rows = 4;
        int cw_ = ww/cols, ch_ = hh/rows;
        for (int ry = 0; ry < rows; ry++) {
            for (int cx_ = 0; cx_ < cols; cx_++) {
                int r_ = (cx_*255)/(cols-1), g_ = (ry*255)/(rows-1), b_ = 128;
                SDL_SetRenderDrawColor(r, r_, g_, b_, 255);
                SDL_Rect cr = {x+cx_*cw_, y+ry*ch_, cw_-1, ch_-1};
                SDL_RenderFillRect(r, &cr);
            }
        }
        break;
    }

    default: break;
    } // end switch

    // ─── رسم الأبناء (مع دعم العرض الافتراضي للقوائم) ───
    //
    // العرض الافتراضي (Virtualization):
    // بدلاً من رسم جميع الأبناء (حتى غير المرئيين)،
    // نتحقق مما إذا كان العنصر الابن يقع داخل منطقة العرض المرئية.
    // هذا يوفر أداءً هائلاً مع القوائم الطويلة (١٠٠٠+ عنصر).
    //
    // للعناصر القابلة للتمرير (ListView, ScrollView):
    //   - نحسب منطقة القصاصة (clipping rectangle)
    //   - نتخطى العناصر الخارجة عن المنطقة المرئية
    //   - نحسب أول وآخر عنصر مرئي للاستعلام لاحقاً
    //
    bool isScrollable = (w->type == WidgetType::ListView || w->type == WidgetType::ScrollView);
    
    if (isScrollable && w->children.size() > 10) {
        // ═══ وضع القصاصة للعناصر القابلة للتمرير ═══
        // نحدد منطقة قصاصة (clip rect) لمنع رسم أي شيء خارج حدود العنصر
        SDL_Rect clipRect = {(int)w->cx, (int)w->cy, (int)w->cw, (int)w->ch};
        SDL_RenderSetClipRect(r, &clipRect);
        
        float viewTop = w->cy;
        float viewBottom = w->cy + w->ch;
        int firstVisible = -1, lastVisible = -1;
        
        for (int i = 0; i < (int)w->children.size(); i++) {
            int cid = w->children[i];
            Widget* child = getWidget(cid);
            if (!child || !child->visible) continue;
            
            float childTop = child->cy;
            float childBottom = child->cy + child->ch;
            
            // هل العنصر الابن ضمن المنطقة المرئية؟
            // العنصر مرئي إذا: أسفله أسفل أعلى المنطقة، وأعلاه فوق أسفل المنطقة
            if (childBottom >= viewTop && childTop <= viewBottom) {
                if (firstVisible < 0) firstVisible = i;
                lastVisible = i;
                renderWidget(r, cid);
            }
        }
        
        // حفظ معلومات العرض الافتراضي
        w->virtualFirstVisible = firstVisible;
        w->virtualLastVisible = lastVisible;
        
        // إعادة تعيين القصاصة
        SDL_RenderSetClipRect(r, nullptr);
    } else {
        // ═══ الوضع العادي: رسم جميع الأبناء ═══
        for (int cid : w->children) renderWidget(r, cid);
    }
}

// ═══════════════════════════════════════════════════════════════════
// القسم 8: معالجة الأحداث (Events)
// ─────────────────────────────────────────────────────────────────
// إعادة تعيين حالة النقر، تتبع الماوس، اختبار الإصابة
// ═══════════════════════════════════════════════════════════════════

// إعادة تعيين wasClicked لكل العناصر في الشجرة
static void resetClickStates(int wid) {
    Widget* w = getWidget(wid);
    if (!w) return;
    w->wasClicked = false;
    for (int c : w->children) resetClickStates(c);
}

// تحديث حالة التحويم (hover) بناءً على موضع الماوس
static void updateHoverState(int wid, int mx, int my) {
    Widget* w = getWidget(wid);
    if (!w || !w->visible) return;
    bool inside = mx >= w->cx && mx < w->cx+w->cw && my >= w->cy && my < w->cy+w->ch;
    // فقط العناصر التفاعلية تُظهر حالة hover
    w->hovered = inside && (
        w->type == WidgetType::Button || w->type == WidgetType::TextField ||
        w->type == WidgetType::OutlineButton || w->type == WidgetType::TextButton ||
        w->type == WidgetType::ToggleButton || w->type == WidgetType::IconButton ||
        w->type == WidgetType::FloatingButton || w->type == WidgetType::Checkbox ||
        w->type == WidgetType::Radio || w->type == WidgetType::Switch ||
        w->type == WidgetType::ListTile || w->type == WidgetType::NavItem ||
        w->type == WidgetType::MenuItem || w->type == WidgetType::TreeItem ||
        w->type == WidgetType::SearchField || w->type == WidgetType::PasswordField ||
        w->type == WidgetType::TextArea || w->type == WidgetType::NumberInput ||
        w->type == WidgetType::DateField || w->type == WidgetType::Dropdown ||
        w->type == WidgetType::Link
    );
    for (int c : w->children) updateHoverState(c, mx, my);
}

// معالجة النقر — اختبار إصابة تكراري من الأمام للخلف
static void handleClick(int wid, int mx, int my, bool& handled) {
    Widget* w = getWidget(wid);
    if (!w || !w->visible || handled) return;
    bool inside = mx >= w->cx && mx < w->cx+w->cw && my >= w->cy && my < w->cy+w->ch;
    // الأبناء أولاً (الأعلى في الترتيب)
    for (int i = (int)w->children.size()-1; i >= 0; i--) {
        handleClick(w->children[i], mx, my, handled);
        if (handled) return;
    }
    if (!inside) return;

    // ─── أنواع قابلة للنقر ───
    if (w->type == WidgetType::Button || w->type == WidgetType::OutlineButton ||
        w->type == WidgetType::TextButton || w->type == WidgetType::IconButton ||
        w->type == WidgetType::FloatingButton || w->type == WidgetType::Link) {
        w->wasClicked = true; w->pressed = true; handled = true;
    }
    // ─── مربع اختيار — تبديل ───
    else if (w->type == WidgetType::Checkbox || w->type == WidgetType::Switch) {
        w->checked = !w->checked; w->wasClicked = true; handled = true;
    }
    // ─── زر دائري — اختيار حصري ───
    else if (w->type == WidgetType::Radio) {
        w->checked = true; w->wasClicked = true; handled = true;
    }
    // ─── زر تبديل ───
    else if (w->type == WidgetType::ToggleButton) {
        w->checked = !w->checked; w->wasClicked = true; w->pressed = true; handled = true;
    }
    // ─── حقول الإدخال — تركيز ───
    else if (w->type == WidgetType::TextField || w->type == WidgetType::SearchField ||
             w->type == WidgetType::PasswordField || w->type == WidgetType::TextArea ||
             w->type == WidgetType::NumberInput || w->type == WidgetType::DateField) {
        // إلغاء تركيز الكل أولاً
        for (auto& p : g_widgets)
            if (p.second.inputFocused) p.second.inputFocused = false;
        w->inputFocused = true; handled = true;
    }
    // ─── لوحة توسيع ───
    else if (w->type == WidgetType::ExpansionPanel) {
        w->expanded = !w->expanded; w->wasClicked = true; handled = true;
    }
    // ─── قائمة منسدلة — تبديل الفهرس ───
    else if (w->type == WidgetType::Dropdown) {
        if (!w->items.empty()) {
            w->selectedIndex = (w->selectedIndex + 1) % (int)w->items.size();
        }
        w->wasClicked = true; handled = true;
    }
    // ─── نجوم التقييم ───
    else if (w->type == WidgetType::RatingStars) {
        int maxS = std::max(1, (int)w->maxVal);
        int starW = std::max(12, (int)w->cw / maxS);
        int clickedStar = (mx - (int)w->cx) / starW + 1;
        w->value = (float)std::min(clickedStar, maxS);
        w->wasClicked = true; handled = true;
    }
    // ─── شريط التبويبات ───
    else if (w->type == WidgetType::TabBar && !w->items.empty()) {
        int tabW = (int)w->cw / (int)w->items.size();
        w->selectedIndex = (mx - (int)w->cx) / tabW;
        w->wasClicked = true; handled = true;
    }
    // ─── ترقيم الصفحات ───
    else if (w->type == WidgetType::Pagination) {
        int total = std::max(1, w->totalPages);
        int btnW = 30, gap = 4;
        int totalW = total * btnW + (total-1) * gap;
        int startX = (int)w->cx + ((int)w->cw - totalW) / 2;
        for (int i = 0; i < total; i++) {
            if (mx >= startX+i*(btnW+gap) && mx < startX+i*(btnW+gap)+btnW) {
                w->currentPage = i; w->wasClicked = true; handled = true; break;
            }
        }
    }
    // ─── منزلق — تحديث القيمة ───
    else if (w->type == WidgetType::Slider) {
        float pct = (float)(mx - (int)w->cx - 4) / ((int)w->cw - 8);
        pct = std::max(0.f, std::min(1.f, pct));
        w->value = w->minVal + pct * (w->maxVal - w->minVal);
        w->wasClicked = true; handled = true;
    }
    // ─── NavItem / MenuItem ───
    else if (w->type == WidgetType::NavItem || w->type == WidgetType::MenuItem ||
             w->type == WidgetType::ListTile || w->type == WidgetType::TreeItem) {
        w->wasClicked = true; w->active = true; handled = true;
    }
}

// إلغاء حالة الضغط عند رفع الماوس
static void handlePressState(int wid, bool down) {
    Widget* w = getWidget(wid);
    if (!w) return;
    if (!down) w->pressed = false;
    for (int c : w->children) handlePressState(c, down);
}

// ═══════════════════════════════════════════════════════════════════
// القسم 9: الدوال العامة (Public API) — إدارة التطبيقات
// ═══════════════════════════════════════════════════════════════════

int app_create(const std::string& title, int width, int height) {
    if (SDL_WasInit(SDL_INIT_VIDEO) == 0) SDL_Init(SDL_INIT_VIDEO);
    // تحميل خط HD عند أول تشغيل
    ensureFontLoaded();
    int id = g_nextAppId++;
    AppState app;
    app.id = id; app.width = width; app.height = height;
    app.window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (app.window) {
        app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!app.renderer) app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_SOFTWARE);
    }
    app.running = (app.window && app.renderer);
    if (app.running) SDL_StartTextInput(); // تفعيل إدخال النص
    g_apps[id] = app;
    return id;
}

bool app_is_running(int appId) {
    auto* a = getApp(appId); return a ? a->running : false;
}

void app_update(int appId) {
    auto* a = getApp(appId);
    if (!a || !a->running) return;

    // ═══════════════════════════════════════════════════════════════
    // الخطوة ٠: حساب فارق الوقت (deltaTime)
    // ─────────────────────────────────────────────────────────────
    // نحسب الوقت المنقضي منذ الإطار السابق بدقة عالية
    // هذا ضروري لنظام التحريك وتحديد معدل الإطارات
    // ═══════════════════════════════════════════════════════════════
    Uint32 nowTick = SDL_GetTicks();
    if (a->lastTickMs == 0) a->lastTickMs = nowTick;  // الإطار الأول
    a->deltaMs = (double)(nowTick - a->lastTickMs);
    a->lastTickMs = nowTick;
    // حماية من القفزات الكبيرة (مثل عند تحريك النافذة)
    if (a->deltaMs > 100.0) a->deltaMs = 16.0;
    // حساب FPS (يُحدَّث كل ثانية)
    a->fpsAccum += a->deltaMs;
    a->fpsFrames++;
    if (a->fpsAccum >= 1000.0) {
        a->fps = (double)a->fpsFrames * 1000.0 / a->fpsAccum;
        a->fpsAccum = 0;
        a->fpsFrames = 0;
    }

    // إعادة تعيين حالة النقر
    if (a->rootWidget > 0) resetClickStates(a->rootWidget);
    a->keyInput.clear(); a->backspace = false; a->mouseClicked = false;

    // معالجة أحداث SDL
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT: a->running = false; break;
        case SDL_MOUSEMOTION: a->mouseX = e.motion.x; a->mouseY = e.motion.y; break;
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT) { a->mouseDown = true; a->mouseClicked = true; }
            break;
        case SDL_MOUSEBUTTONUP:
            if (e.button.button == SDL_BUTTON_LEFT) a->mouseDown = false;
            break;
        case SDL_TEXTINPUT: a->keyInput += e.text.text; break;
        case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_BACKSPACE) a->backspace = true;
            else if (e.key.keysym.sym == SDLK_RETURN) a->keyInput += "\n";
            // معالجة التمرير بالأسهم (لدعم ListView)
            else if (e.key.keysym.sym == SDLK_DOWN || e.key.keysym.sym == SDLK_UP) {
                // يتم التعامل مع التمرير في updateScrollState
            }
            break;
        case SDL_MOUSEWHEEL:
            // ═══ تمرير عجلة الماوس (لـ ListView/ScrollView) ═══
            // نبحث عن أقرب عنصر قابل للتمرير تحت الماوس
            if (a->rootWidget > 0) {
                // تمرير بـ 40 بكسل لكل خطوة
                float scrollDelta = -e.wheel.y * 40.0f;
                // نطبق على أول عنصر تمرير نجده
                for (auto& [wid, w] : g_widgets) {
                    if ((w.type == WidgetType::ListView || w.type == WidgetType::ScrollView) &&
                        a->mouseX >= w.cx && a->mouseX <= w.cx + w.cw &&
                        a->mouseY >= w.cy && a->mouseY <= w.cy + w.ch) {
                        w.scrollY = std::max(0.0f, w.scrollY + scrollDelta);
                        break;
                    }
                }
            }
            break;
        }
    }

    // تحديث إدخال النص للحقل المركّز
    for (auto& p : g_widgets) {
        Widget& w = p.second;
        if (w.inputFocused && (w.type == WidgetType::TextField || w.type == WidgetType::SearchField ||
            w.type == WidgetType::PasswordField || w.type == WidgetType::TextArea ||
            w.type == WidgetType::NumberInput || w.type == WidgetType::DateField)) {
            if (!a->keyInput.empty()) w.inputText += a->keyInput;
            if (a->backspace && !w.inputText.empty()) w.inputText.pop_back();
        }
    }

    if (a->rootWidget > 0) {
        updateHoverState(a->rootWidget, a->mouseX, a->mouseY);
        if (a->mouseClicked) {
            bool handled = false;
            handleClick(a->rootWidget, a->mouseX, a->mouseY, handled);
            if (!handled) { // نقر على مكان فارغ → إلغاء التركيز
                for (auto& p : g_widgets)
                    if (p.second.inputFocused) p.second.inputFocused = false;
            }
        }
        if (!a->mouseDown) handlePressState(a->rootWidget, false);
        // حساب التخطيط
        layoutWidget(a->rootWidget, 0, 0, (float)a->width, (float)a->height);
    }

    // ═══════════════════════════════════════════════════════════════
    // تحديث نظام التحريك
    // ─────────────────────────────────────────────────────────────
    // نمرر فارق الوقت بالمللي ثانية لتحديث جميع التحريكات النشطة
    // هذا يجعل التحريكات مستقلة عن معدل الإطارات
    // ═══════════════════════════════════════════════════════════════
    sad::anim::updateAllAnimations(a->deltaMs);

    // الرسم
    SDL_SetRenderDrawColor(a->renderer, a->bgColor.r, a->bgColor.g, a->bgColor.b, a->bgColor.a);
    SDL_RenderClear(a->renderer);
    if (a->rootWidget > 0) renderWidget(a->renderer, a->rootWidget);
    SDL_RenderPresent(a->renderer);

    // ═══════════════════════════════════════════════════════════════
    // تحديد معدل الإطارات (Frame Rate Limiting)
    // ─────────────────────────────────────────────────────────────
    // نحدّد سرعة الحلقة لتوفير طاقة المعالج
    // بدون هذا، الحلقة تعمل بأقصى سرعة وتستهلك CPU ١٠٠٪
    // ═══════════════════════════════════════════════════════════════
    if (a->targetFps > 0) {
        Uint32 frameTime = SDL_GetTicks() - nowTick;
        Uint32 targetTime = 1000 / (Uint32)a->targetFps;
        if (frameTime < targetTime) {
            SDL_Delay(targetTime - frameTime);
        }
    }
}

void app_set_content(int appId, int widgetId) {
    auto* a = getApp(appId); if (a) a->rootWidget = widgetId;
}
void app_set_bg_color(int appId, int r, int g, int b) {
    auto* a = getApp(appId); if (a) a->bgColor = {r,g,b,255};
}
void app_close(int appId) {
    auto* a = getApp(appId); if (!a) return;
    a->running = false;
    if (a->renderer) { SDL_DestroyRenderer(a->renderer); a->renderer = nullptr; }
    if (a->window) { SDL_DestroyWindow(a->window); a->window = nullptr; }
}

// ═══════════════════════════════════════════════════════════════════
// القسم 10: إنشاء العناصر الـ 109
// ═══════════════════════════════════════════════════════════════════

// ─── العناصر الأصلية 9 ───
int widget_column()    { return createWidget(WidgetType::Column); }
int widget_row()       { return createWidget(WidgetType::Row); }
int widget_text(const std::string& c) {
    int id = createWidget(WidgetType::Text);
    auto* w = getWidget(id); if(w) { w->text = c; w->textColor = {33,33,33,255}; }
    return id;
}
int widget_button(const std::string& t) {
    int id = createWidget(WidgetType::Button);
    auto* w = getWidget(id); if(w) {
        w->text = t; w->textColor = {255,255,255,255};
        w->btnColor = {66,133,244,255}; w->btnHover = {90,155,255,255};
        w->btnPress = {40,100,200,255}; w->borderRadius = 8;
    } return id;
}
int widget_textfield(const std::string& h) {
    int id = createWidget(WidgetType::TextField);
    auto* w = getWidget(id); if(w) { w->text = h; w->bgColor = {255,255,255,255}; w->borderRadius = 4; }
    return id;
}
int widget_container() { return createWidget(WidgetType::Container); }
int widget_spacer(float s) {
    int id = createWidget(WidgetType::Spacer);
    auto* w = getWidget(id); if(w) w->fixedHeight = s; return id;
}
int widget_card() {
    int id = createWidget(WidgetType::Card);
    auto* w = getWidget(id); if(w) { w->bgColor = {255,255,255,255}; w->borderRadius = 12; w->padding = 16; }
    return id;
}
int widget_divider() { return createWidget(WidgetType::Divider); }

// ─── تخطيط (10) ───
int widget_stack()     { return createWidget(WidgetType::Stack); }
int widget_center()    { return createWidget(WidgetType::Center); }
int widget_wrap()      { auto id=createWidget(WidgetType::Wrap); auto*w=getWidget(id); if(w)w->columns=4; return id; }
int widget_gridview(int cols) {
    int id=createWidget(WidgetType::GridView); auto*w=getWidget(id); if(w) w->columns=cols; return id;
}
int widget_listview()  { return createWidget(WidgetType::ListView); }
int widget_scrollview(){ return createWidget(WidgetType::ScrollView); }
int widget_sizedbox(float w_, float h_) {
    int id=createWidget(WidgetType::SizedBox); auto*w=getWidget(id);
    if(w){w->fixedWidth=w_; w->fixedHeight=h_;} return id;
}
int widget_expanded()  { int id=createWidget(WidgetType::Expanded); auto*w=getWidget(id); if(w)w->flex=1; return id; }
int widget_pageview()  { return createWidget(WidgetType::PageView); }
int widget_table(int cols) {
    int id=createWidget(WidgetType::Table); auto*w=getWidget(id); if(w)w->columns=cols; return id;
}

// ─── أزرار (6) ───
int widget_icon_button(char icon) {
    int id=createWidget(WidgetType::IconButton); auto*w=getWidget(id);
    if(w){w->iconChar=icon; w->textColor={60,60,60,255}; w->fixedWidth=40; w->fixedHeight=40;}
    return id;
}
int widget_floating_button(char icon) {
    int id=createWidget(WidgetType::FloatingButton); auto*w=getWidget(id);
    if(w){w->iconChar=icon; w->btnColor={66,133,244,255}; w->fixedWidth=56; w->fixedHeight=56;}
    return id;
}
int widget_outline_button(const std::string& t) {
    int id=createWidget(WidgetType::OutlineButton); auto*w=getWidget(id);
    if(w){w->text=t; w->btnColor={66,133,244,255}; w->borderRadius=8;} return id;
}
int widget_text_button(const std::string& t) {
    int id=createWidget(WidgetType::TextButton); auto*w=getWidget(id);
    if(w){w->text=t; w->btnColor={66,133,244,255};} return id;
}
int widget_toggle_button(const std::string& t) {
    int id=createWidget(WidgetType::ToggleButton); auto*w=getWidget(id);
    if(w){w->text=t; w->btnColor={66,133,244,255}; w->borderRadius=8;} return id;
}
int widget_button_group() { return createWidget(WidgetType::ButtonGroup); }

// ─── عناصر الإدخال (14) ───
int widget_checkbox(const std::string& l) {
    int id=createWidget(WidgetType::Checkbox); auto*w=getWidget(id); if(w) w->text=l; return id;
}
int widget_radio(const std::string& l) {
    int id=createWidget(WidgetType::Radio); auto*w=getWidget(id); if(w) w->text=l; return id;
}
int widget_switch(const std::string& l) {
    int id=createWidget(WidgetType::Switch); auto*w=getWidget(id); if(w) w->text=l; return id;
}
int widget_slider(float mn, float mx) {
    int id=createWidget(WidgetType::Slider); auto*w=getWidget(id);
    if(w){w->minVal=mn; w->maxVal=mx; w->value=mn;} return id;
}
int widget_dropdown(const std::string& h) {
    int id=createWidget(WidgetType::Dropdown); auto*w=getWidget(id); if(w) w->text=h; return id;
}
int widget_number_input(float mn, float mx) {
    int id=createWidget(WidgetType::NumberInput); auto*w=getWidget(id);
    if(w){w->minVal=mn; w->maxVal=mx; w->text="0";} return id;
}
int widget_search_field(const std::string& h) {
    int id=createWidget(WidgetType::SearchField); auto*w=getWidget(id); if(w) w->text=h; return id;
}
int widget_password_field(const std::string& h) {
    int id=createWidget(WidgetType::PasswordField); auto*w=getWidget(id); if(w) w->text=h; return id;
}
int widget_textarea(const std::string& h) {
    int id=createWidget(WidgetType::TextArea); auto*w=getWidget(id); if(w) w->text=h; return id;
}
int widget_date_field() { return createWidget(WidgetType::DateField); }
int widget_range_slider(float mn, float mx) {
    int id=createWidget(WidgetType::RangeSlider); auto*w=getWidget(id);
    if(w){w->minVal=mn; w->maxVal=mx; w->value=mn; w->value2=mx;} return id;
}
int widget_toggle_group() { return createWidget(WidgetType::ToggleGroup); }
int widget_color_picker() { int id=createWidget(WidgetType::ColorPicker); auto*w=getWidget(id); if(w){w->fixedHeight=80;} return id; }
int widget_rating_stars(int mx) {
    int id=createWidget(WidgetType::RatingStars); auto*w=getWidget(id);
    if(w){w->maxVal=(float)mx; w->value=0;} return id;
}

// ─── عرض النصوص (7) ───
int widget_label(const std::string& t) {
    int id=createWidget(WidgetType::Label); auto*w=getWidget(id); if(w){w->text=t; w->textColor={33,33,33,255};} return id;
}
int widget_heading(const std::string& t, int lv) {
    int id=createWidget(WidgetType::Heading); auto*w=getWidget(id);
    if(w){w->text=t; w->level=lv; w->textColor={20,20,20,255}; w->fontSize=28;} return id;
}
int widget_paragraph(const std::string& t) {
    int id=createWidget(WidgetType::Paragraph); auto*w=getWidget(id); if(w){w->text=t; w->textColor={60,60,60,255}; w->textAlign="left";} return id;
}
int widget_link(const std::string& t) {
    int id=createWidget(WidgetType::Link); auto*w=getWidget(id); if(w) w->text=t; return id;
}
int widget_code_block(const std::string& c) {
    int id=createWidget(WidgetType::CodeBlock); auto*w=getWidget(id); if(w) w->text=c; return id;
}
int widget_quote(const std::string& t) {
    int id=createWidget(WidgetType::Quote); auto*w=getWidget(id); if(w) w->text=t; return id;
}
int widget_markdown(const std::string& t) {
    int id=createWidget(WidgetType::Markdown); auto*w=getWidget(id); if(w) w->text=t; return id;
}

// ─── عرض مرئي (10) ───
int widget_icon(char ic) {
    int id=createWidget(WidgetType::Icon); auto*w=getWidget(id); if(w) w->iconChar=ic; return id;
}
int widget_avatar(const std::string& l, int r_, int g_, int b_) {
    int id=createWidget(WidgetType::Avatar); auto*w=getWidget(id);
    if(w){w->text=l; w->btnColor={r_,g_,b_,255}; w->fixedWidth=48; w->fixedHeight=48;} return id;
}
int widget_badge(int count) {
    int id=createWidget(WidgetType::Badge); auto*w=getWidget(id);
    if(w){w->value=(float)count; w->fixedWidth=24; w->fixedHeight=24;} return id;
}
int widget_chip(const std::string& l) {
    int id=createWidget(WidgetType::Chip); auto*w=getWidget(id); if(w) w->text=l; return id;
}
int widget_tag(const std::string& l, int r_, int g_, int b_) {
    int id=createWidget(WidgetType::Tag); auto*w=getWidget(id);
    if(w){w->text=l; w->btnColor={r_,g_,b_,255};} return id;
}
int widget_status_dot(int r_, int g_, int b_) {
    int id=createWidget(WidgetType::StatusDot); auto*w=getWidget(id);
    if(w){w->bgColor={r_,g_,b_,255}; w->fixedWidth=16; w->fixedHeight=16;} return id;
}
int widget_color_box(int r_, int g_, int b_) {
    int id=createWidget(WidgetType::ColorBox); auto*w=getWidget(id);
    if(w){w->bgColor={r_,g_,b_,255};} return id;
}
int widget_gradient_box(int r1, int g1, int b1, int r2, int g2, int b2) {
    int id=createWidget(WidgetType::GradientBox); auto*w=getWidget(id);
    if(w){w->btnColor={r1,g1,b1,255}; w->accentColor={r2,g2,b2,255};} return id;
}
int widget_image(const std::string& p) {
    int id=createWidget(WidgetType::ImageWidget); auto*w=getWidget(id); if(w) w->text=p; return id;
}
int widget_separator() { return createWidget(WidgetType::Separator); }

// ─── تقدم (4) ───
int widget_progress_bar(float v) {
    int id=createWidget(WidgetType::ProgressBar); auto*w=getWidget(id);
    if(w){w->value=v; w->maxVal=100;} return id;
}
int widget_circle_progress(float v) {
    int id=createWidget(WidgetType::CircleProgress); auto*w=getWidget(id);
    if(w){w->value=v; w->maxVal=100; w->fixedWidth=60; w->fixedHeight=60;} return id;
}
int widget_loading_spinner() {
    int id=createWidget(WidgetType::LoadingSpinner); auto*w=getWidget(id);
    if(w){w->fixedWidth=40; w->fixedHeight=40;} return id;
}
int widget_skeleton() { return createWidget(WidgetType::Skeleton); }

// ─── تنقل (10) ───
int widget_app_bar(const std::string& t) {
    int id=createWidget(WidgetType::AppBar); auto*w=getWidget(id);
    if(w){w->text=t; w->fixedHeight=56; w->bgColor={25,80,160,255};} return id;
}
int widget_bottom_bar() {
    int id=createWidget(WidgetType::BottomBar); auto*w=getWidget(id);
    if(w) w->fixedHeight=56; return id;
}
int widget_tab_bar() {
    int id=createWidget(WidgetType::TabBar); auto*w=getWidget(id);
    if(w){w->fixedHeight=48; w->selectedIndex=0;} return id;
}
int widget_drawer() { return createWidget(WidgetType::Drawer); }
int widget_breadcrumb() { return createWidget(WidgetType::Breadcrumb); }
int widget_nav_item(const std::string& l) {
    int id=createWidget(WidgetType::NavItem); auto*w=getWidget(id); if(w) w->text=l; return id;
}
int widget_menu_item(const std::string& l) {
    int id=createWidget(WidgetType::MenuItem); auto*w=getWidget(id); if(w) w->text=l; return id;
}
int widget_toolbar() { return createWidget(WidgetType::Toolbar); }
int widget_status_bar(const std::string& t) {
    int id=createWidget(WidgetType::StatusBar); auto*w=getWidget(id);
    if(w){w->text=t; w->fixedHeight=28;} return id;
}
int widget_side_menu() { return createWidget(WidgetType::SideMenu); }

// ─── ردود الفعل (10) ───
int widget_dialog(const std::string& t, const std::string& m) {
    int id=createWidget(WidgetType::Dialog); auto*w=getWidget(id);
    if(w){w->text=t; w->subtitle=m; w->padding=16; w->bgColor={255,255,255,255}; w->borderRadius=12;}
    return id;
}
int widget_alert(const std::string& m, const std::string& type) {
    int id=createWidget(WidgetType::Alert); auto*w=getWidget(id);
    if(w){w->text=m; w->subtitle=type;} return id;
}
int widget_toast(const std::string& m) {
    int id=createWidget(WidgetType::Toast); auto*w=getWidget(id); if(w) w->text=m; return id;
}
int widget_snackbar(const std::string& m) {
    int id=createWidget(WidgetType::Snackbar); auto*w=getWidget(id); if(w) w->text=m; return id;
}
int widget_banner(const std::string& m) {
    int id=createWidget(WidgetType::Banner); auto*w=getWidget(id); if(w) w->text=m; return id;
}
int widget_modal() {
    int id=createWidget(WidgetType::Modal); auto*w=getWidget(id);
    if(w){w->padding=24; w->bgColor={255,255,255,255}; w->borderRadius=12;} return id;
}
int widget_popover() { return createWidget(WidgetType::Popover); }
int widget_notification(const std::string& t, const std::string& m) {
    int id=createWidget(WidgetType::Notification); auto*w=getWidget(id);
    if(w){w->text=t; w->subtitle=m;} return id;
}
int widget_confirm_dialog(const std::string& t, const std::string& m) {
    int id=createWidget(WidgetType::ConfirmDialog); auto*w=getWidget(id);
    if(w){w->text=t; w->subtitle=m; w->padding=16; w->bgColor={255,255,255,255}; w->borderRadius=12;}
    return id;
}
int widget_tooltip(const std::string& t) {
    int id=createWidget(WidgetType::Tooltip); auto*w=getWidget(id); if(w) w->text=t; return id;
}

// ─── بطاقات ولوحات (7) ───
int widget_info_card(const std::string& t, const std::string& b) {
    int id=createWidget(WidgetType::InfoCard); auto*w=getWidget(id);
    if(w){w->text=t; w->subtitle=b; w->borderRadius=12; w->padding=12;} return id;
}
int widget_stat_card(const std::string& l, const std::string& v) {
    int id=createWidget(WidgetType::StatCard); auto*w=getWidget(id);
    if(w){w->text=l; w->subtitle=v; w->borderRadius=12; w->padding=12;} return id;
}
int widget_expansion_panel(const std::string& t) {
    int id=createWidget(WidgetType::ExpansionPanel); auto*w=getWidget(id);
    if(w) w->text=t; return id;
}
int widget_accordion() { return createWidget(WidgetType::Accordion); }
int widget_panel(const std::string& t) {
    int id=createWidget(WidgetType::Panel); auto*w=getWidget(id);
    if(w){w->text=t; w->padding=12;} return id;
}
int widget_group_box(const std::string& t) {
    int id=createWidget(WidgetType::GroupBox); auto*w=getWidget(id);
    if(w){w->text=t; w->padding=12;} return id;
}
int widget_callout(const std::string& t, const std::string& type) {
    int id=createWidget(WidgetType::Callout); auto*w=getWidget(id);
    if(w){w->text=t; w->subtitle=type;} return id;
}

// ─── بيانات (10) ───
int widget_list_tile(const std::string& t, const std::string& s) {
    int id=createWidget(WidgetType::ListTile); auto*w=getWidget(id);
    if(w){w->text=t; w->subtitle=s;} return id;
}
int widget_data_table(int cols) {
    int id=createWidget(WidgetType::DataTable); auto*w=getWidget(id);
    if(w) w->columns=cols; return id;
}
int widget_table_row() { return createWidget(WidgetType::TableRow); }
int widget_table_cell(const std::string& t) {
    int id=createWidget(WidgetType::TableCell); auto*w=getWidget(id); if(w) w->text=t; return id;
}
int widget_detail_row(const std::string& k, const std::string& v) {
    int id=createWidget(WidgetType::DetailRow); auto*w=getWidget(id);
    if(w){w->text=k; w->subtitle=v;} return id;
}
int widget_timeline() { return createWidget(WidgetType::Timeline); }
int widget_timeline_item(const std::string& t) {
    int id=createWidget(WidgetType::TimelineItem); auto*w=getWidget(id); if(w) w->text=t; return id;
}
int widget_tree_item(const std::string& l, int depth) {
    int id=createWidget(WidgetType::TreeItem); auto*w=getWidget(id);
    if(w){w->text=l; w->level=depth;} return id;
}
int widget_stepper(int steps) {
    int id=createWidget(WidgetType::Stepper); auto*w=getWidget(id);
    if(w){w->maxVal=(float)steps; w->value=0;} return id;
}
int widget_step_indicator(int total, int current) {
    int id=createWidget(WidgetType::StepIndicator); auto*w=getWidget(id);
    if(w){w->maxVal=(float)total; w->value=(float)current;} return id;
}

// ─── رسوم بيانية (7) ───
int widget_bar_chart()   { int id=createWidget(WidgetType::BarChart); auto*w=getWidget(id); if(w) w->fixedHeight=150; return id; }
int widget_hbar_chart()  { int id=createWidget(WidgetType::HBarChart); auto*w=getWidget(id); if(w) w->fixedHeight=150; return id; }
int widget_pie_chart()   { int id=createWidget(WidgetType::PieChart); auto*w=getWidget(id); if(w){w->fixedWidth=150; w->fixedHeight=150;} return id; }
int widget_line_chart()  { int id=createWidget(WidgetType::LineChart); auto*w=getWidget(id); if(w) w->fixedHeight=120; return id; }
int widget_spark_line()  { int id=createWidget(WidgetType::SparkLine); auto*w=getWidget(id); if(w) w->fixedHeight=40; return id; }
int widget_gauge(float v, float mx) {
    int id=createWidget(WidgetType::Gauge); auto*w=getWidget(id);
    if(w){w->value=v; w->maxVal=mx; w->fixedHeight=80;} return id;
}
int widget_donut_chart() { int id=createWidget(WidgetType::DonutChart); auto*w=getWidget(id); if(w){w->fixedWidth=150; w->fixedHeight=150;} return id; }

// ─── ترقيم صفحات (2) ───
int widget_pagination(int total) {
    int id=createWidget(WidgetType::Pagination); auto*w=getWidget(id);
    if(w){w->totalPages=total; w->currentPage=0; w->fixedHeight=40;} return id;
}
int widget_dot_indicator(int total, int current) {
    int id=createWidget(WidgetType::DotIndicator); auto*w=getWidget(id);
    if(w){w->maxVal=(float)total; w->value=(float)current;} return id;
}

// ─── متقدم (3) ───
int widget_canvas(float w_, float h_) {
    int id=createWidget(WidgetType::Canvas); auto*w=getWidget(id);
    if(w){w->fixedWidth=w_; w->fixedHeight=h_;} return id;
}
int widget_split_view() { return createWidget(WidgetType::SplitView); }
int widget_carousel()   { return createWidget(WidgetType::Carousel); }

// ═══════════════════════════════════════════════════════════════════
// القسم 11: إدارة شجرة العناصر وتعديل الخصائص
// ═══════════════════════════════════════════════════════════════════

void widget_add_child(int pid, int cid)   { auto*w=getWidget(pid); if(w) w->children.push_back(cid); }
void widget_clear_children(int pid)       { auto*w=getWidget(pid); if(w) w->children.clear(); }

void widget_set_bg(int id,int r,int g,int b)         { auto*w=getWidget(id); if(w) w->bgColor={r,g,b,255}; }
void widget_set_padding(int id,float a)              { auto*w=getWidget(id); if(w){w->padding=a; w->padTop=w->padRight=w->padBottom=w->padLeft=a;} }
void widget_set_padding4(int id,float t,float r,float b,float l) { auto*w=getWidget(id); if(w){w->padding=0; w->padTop=t; w->padRight=r; w->padBottom=b; w->padLeft=l;} }
void widget_set_height(int id,float h)               { auto*w=getWidget(id); if(w) w->fixedHeight=h; }
void widget_set_width(int id,float v)                { auto*w=getWidget(id); if(w) w->fixedWidth=v; }
void widget_set_flex(int id,float f)                 { auto*w=getWidget(id); if(w) w->flex=f; }
void widget_set_border_radius(int id,float r)        { auto*w=getWidget(id); if(w) w->borderRadius=r; }
void widget_set_spacing(int id,float g)              { auto*w=getWidget(id); if(w) w->spacing=g; }
void widget_set_visible(int id,bool v)               { auto*w=getWidget(id); if(w) w->visible=v; }
void widget_set_border(int id,float bw,int r,int g,int b) { auto*w=getWidget(id); if(w){w->borderWidth=bw; w->borderColor={r,g,b,255};} }

void text_set_size(int id,float s)                   { auto*w=getWidget(id); if(w) w->fontSize=s; }
void text_set_color(int id,int r,int g,int b)        { auto*w=getWidget(id); if(w) w->textColor={r,g,b,255}; }
void text_set_content(int id,const std::string& c)   { auto*w=getWidget(id); if(w) w->text=c; }
std::string text_get_content(int id)                 { auto*w=getWidget(id); return w ? w->text : ""; }
void text_set_align(int id,const std::string& a)     { auto*w=getWidget(id); if(w) w->textAlign=a; }

void button_set_color(int id,int r,int g,int b) {
    auto*w=getWidget(id); if(w){
        w->btnColor={r,g,b,255};
        w->btnHover={std::min(r+30,255),std::min(g+30,255),std::min(b+30,255),255};
        w->btnPress={std::max(r-30,0),std::max(g-30,0),std::max(b-30,0),255};
    }
}
bool button_was_clicked(int id)                      { auto*w=getWidget(id); return w ? w->wasClicked : false; }
bool button_was_clicked(const std::string& name)      { auto*w=getWidgetByName(name); return w ? w->wasClicked : false; }

std::string textfield_get_text(int id)               { auto*w=getWidget(id); return w ? w->inputText : ""; }
std::string textfield_get_text(const std::string& name){ auto*w=getWidgetByName(name); return w ? w->inputText : ""; }
void textfield_set_text(int id,const std::string& t) { auto*w=getWidget(id); if(w) w->inputText=t; }

// ─── الخصائص الموسّعة للمكونات الجديدة ───
void   widget_set_checked(int id,bool c)             { auto*w=getWidget(id); if(w) w->checked=c; }
bool   widget_get_checked(int id)                    { auto*w=getWidget(id); return w ? w->checked : false; }
bool   widget_get_checked(const std::string& name)   { auto*w=getWidgetByName(name); return w ? w->checked : false; }
void   widget_set_value(int id,float v)              { auto*w=getWidget(id); if(w) w->value=v; }
float  widget_get_value(int id)                      { auto*w=getWidget(id); return w ? w->value : 0; }
float  widget_get_value(const std::string& name)     { auto*w=getWidgetByName(name); return w ? w->value : 0; }
void   widget_set_selected(int id,int i)             { auto*w=getWidget(id); if(w) w->selectedIndex=i; }
int    widget_get_selected(int id)                   { auto*w=getWidget(id); return w ? w->selectedIndex : -1; }
void   widget_set_expanded(int id,bool e)            { auto*w=getWidget(id); if(w) w->expanded=e; }
bool   widget_get_expanded(int id)                   { auto*w=getWidget(id); return w ? w->expanded : false; }
void   widget_set_items(int id,const std::vector<std::string>& items) { auto*w=getWidget(id); if(w) w->items=items; }
void   widget_add_item(int id,const std::string& item){ auto*w=getWidget(id); if(w) w->items.push_back(item); }
void   widget_set_data(int id,const std::vector<float>& d) { auto*w=getWidget(id); if(w) w->dataValues=d; }
void   widget_add_data(int id,float v)               { auto*w=getWidget(id); if(w) w->dataValues.push_back(v); }
void   widget_set_icon(int id,char c)                { auto*w=getWidget(id); if(w) w->iconChar=c; }
void   widget_set_active(int id,bool a)              { auto*w=getWidget(id); if(w) w->active=a; }
void   widget_set_subtitle(int id,const std::string& s){ auto*w=getWidget(id); if(w) w->subtitle=s; }
void   widget_set_level(int id,int l)                { auto*w=getWidget(id); if(w) w->level=l; }
void   widget_set_columns(int id,int c)              { auto*w=getWidget(id); if(w) w->columns=c; }
void   widget_set_page(int id,int p)                 { auto*w=getWidget(id); if(w) w->currentPage=p; }
void   widget_set_accent_color(int id,int r,int g,int b) { auto*w=getWidget(id); if(w) w->accentColor={r,g,b,255}; }
void   widget_show_dialog(int id,bool s)             { auto*w=getWidget(id); if(w) w->dialogVisible=s; }
bool   widget_is_dialog_visible(int id)              { auto*w=getWidget(id); return w ? w->dialogVisible : false; }

// ═══════════════════════════════════════════════════════════════════
// القسم الجديد: نظام إنشاء المكونات بالخصائص (Declarative API)
// ─────────────────────────────────────────────────────────────────
// يتيح إنشاء المكونات بطريقة تعريفية شبيهة بـ Flutter
// باستخدام خريطة خصائص (Map) بأسماء عربية
// ═══════════════════════════════════════════════════════════════════

// تحويل لون هيكس "#RRGGBB" أو "#RGB" إلى RGBA
RGBA parseColor(const std::string& hex) {
    RGBA c = {0, 0, 0, 255};
    if (hex.empty() || hex[0] != '#') {
        // ألوان مسماة بالعربية
        if (hex == "أحمر" || hex == "احمر") return {244, 67, 54, 255};
        if (hex == "أخضر" || hex == "اخضر") return {76, 175, 80, 255};
        if (hex == "أزرق" || hex == "ازرق") return {33, 150, 243, 255};
        if (hex == "أبيض" || hex == "ابيض") return {255, 255, 255, 255};
        if (hex == "أسود" || hex == "اسود") return {0, 0, 0, 255};
        if (hex == "رمادي") return {158, 158, 158, 255};
        if (hex == "برتقالي") return {255, 152, 0, 255};
        if (hex == "بنفسجي") return {156, 39, 176, 255};
        if (hex == "وردي") return {233, 30, 99, 255};
        if (hex == "بني") return {121, 85, 72, 255};
        if (hex == "ذهبي") return {255, 193, 7, 255};
        if (hex == "شفاف") return {0, 0, 0, 0};
        return c;
    }
    std::string h = hex.substr(1);
    if (h.size() == 3) {
        // "#RGB" → "#RRGGBB"
        c.r = std::stoi(std::string(2, h[0]), nullptr, 16);
        c.g = std::stoi(std::string(2, h[1]), nullptr, 16);
        c.b = std::stoi(std::string(2, h[2]), nullptr, 16);
    } else if (h.size() >= 6) {
        c.r = std::stoi(h.substr(0, 2), nullptr, 16);
        c.g = std::stoi(h.substr(2, 2), nullptr, 16);
        c.b = std::stoi(h.substr(4, 2), nullptr, 16);
        if (h.size() >= 8) c.a = std::stoi(h.substr(6, 2), nullptr, 16);
    }
    return c;
}

// خريطة أسماء الأنواع بالعربية → WidgetType
static WidgetType resolveTypeName(const std::string& name) {
    // الأنواع الأصلية (9)
    if (name == "عمود" || name == "column") return WidgetType::Column;
    if (name == "صف" || name == "row") return WidgetType::Row;
    if (name == "نص" || name == "text") return WidgetType::Text;
    if (name == "زر" || name == "button") return WidgetType::Button;
    if (name == "حقل" || name == "textfield") return WidgetType::TextField;
    if (name == "حاوية" || name == "container") return WidgetType::Container;
    if (name == "فاصل" || name == "spacer") return WidgetType::Spacer;
    if (name == "بطاقة" || name == "card") return WidgetType::Card;
    if (name == "خط_فاصل" || name == "divider") return WidgetType::Divider;
    // التخطيط (10)
    if (name == "تكديس" || name == "stack") return WidgetType::Stack;
    if (name == "توسيط" || name == "center") return WidgetType::Center;
    if (name == "التفاف" || name == "wrap") return WidgetType::Wrap;
    if (name == "شبكة" || name == "grid") return WidgetType::GridView;
    if (name == "قائمة_عرض" || name == "listview") return WidgetType::ListView;
    if (name == "تمرير" || name == "scrollview") return WidgetType::ScrollView;
    if (name == "صندوق" || name == "sizedbox") return WidgetType::SizedBox;
    if (name == "توسيع" || name == "expanded") return WidgetType::Expanded;
    if (name == "صفحات" || name == "pageview") return WidgetType::PageView;
    if (name == "جدول" || name == "table") return WidgetType::Table;
    // أزرار (6)
    if (name == "زر_ايقونة" || name == "iconbutton") return WidgetType::IconButton;
    if (name == "زر_عائم" || name == "fab") return WidgetType::FloatingButton;
    if (name == "زر_محيط" || name == "outlinebutton") return WidgetType::OutlineButton;
    if (name == "زر_نصي" || name == "textbutton") return WidgetType::TextButton;
    if (name == "زر_تبديل" || name == "togglebutton") return WidgetType::ToggleButton;
    if (name == "مجموعة_ازرار" || name == "buttongroup") return WidgetType::ButtonGroup;
    // مدخلات (14)
    if (name == "مربع_اختيار" || name == "checkbox") return WidgetType::Checkbox;
    if (name == "راديو" || name == "radio") return WidgetType::Radio;
    if (name == "مفتاح" || name == "switch") return WidgetType::Switch;
    if (name == "منزلق" || name == "slider") return WidgetType::Slider;
    if (name == "قائمة_منسدلة" || name == "dropdown") return WidgetType::Dropdown;
    if (name == "ادخال_رقم" || name == "numberinput") return WidgetType::NumberInput;
    if (name == "حقل_بحث" || name == "searchfield") return WidgetType::SearchField;
    if (name == "حقل_سر" || name == "passwordfield") return WidgetType::PasswordField;
    if (name == "منطقة_نص" || name == "textarea") return WidgetType::TextArea;
    if (name == "حقل_تاريخ" || name == "datefield") return WidgetType::DateField;
    if (name == "منزلق_نطاق" || name == "rangeslider") return WidgetType::RangeSlider;
    if (name == "مجموعة_تبديل" || name == "togglegroup") return WidgetType::ToggleGroup;
    if (name == "منتقي_لون" || name == "colorpicker") return WidgetType::ColorPicker;
    if (name == "تقييم" || name == "rating") return WidgetType::RatingStars;
    // نصوص (7)
    if (name == "تسمية" || name == "label") return WidgetType::Label;
    if (name == "عنوان" || name == "heading") return WidgetType::Heading;
    if (name == "فقرة" || name == "paragraph") return WidgetType::Paragraph;
    if (name == "رابط" || name == "link") return WidgetType::Link;
    if (name == "كود" || name == "codeblock") return WidgetType::CodeBlock;
    if (name == "اقتباس" || name == "quote") return WidgetType::Quote;
    if (name == "ماركداون" || name == "markdown") return WidgetType::Markdown;
    // بصري (10)
    if (name == "ايقونة" || name == "icon") return WidgetType::Icon;
    if (name == "صورة_رمزية" || name == "avatar") return WidgetType::Avatar;
    if (name == "شارة" || name == "badge") return WidgetType::Badge;
    if (name == "رقاقة" || name == "chip") return WidgetType::Chip;
    if (name == "وسم" || name == "tag") return WidgetType::Tag;
    if (name == "نقطة_حالة" || name == "statusdot") return WidgetType::StatusDot;
    if (name == "صندوق_لون" || name == "colorbox") return WidgetType::ColorBox;
    if (name == "تدرج" || name == "gradientbox") return WidgetType::GradientBox;
    if (name == "صورة" || name == "image") return WidgetType::ImageWidget;
    if (name == "فاصل_خط" || name == "separator") return WidgetType::Separator;
    // تقدم (4)
    if (name == "شريط_تقدم" || name == "progressbar") return WidgetType::ProgressBar;
    if (name == "تقدم_دائري" || name == "circleprogress") return WidgetType::CircleProgress;
    if (name == "تحميل" || name == "spinner") return WidgetType::LoadingSpinner;
    if (name == "هيكل" || name == "skeleton") return WidgetType::Skeleton;
    // تنقل (10)
    if (name == "شريط_تطبيق" || name == "appbar") return WidgetType::AppBar;
    if (name == "شريط_سفلي" || name == "bottombar") return WidgetType::BottomBar;
    if (name == "شريط_تبويب" || name == "tabbar") return WidgetType::TabBar;
    if (name == "درج" || name == "drawer") return WidgetType::Drawer;
    if (name == "مسار" || name == "breadcrumb") return WidgetType::Breadcrumb;
    if (name == "عنصر_تنقل" || name == "navitem") return WidgetType::NavItem;
    if (name == "عنصر_قائمة" || name == "menuitem") return WidgetType::MenuItem;
    if (name == "شريط_ادوات" || name == "toolbar") return WidgetType::Toolbar;
    if (name == "شريط_حالة" || name == "statusbar") return WidgetType::StatusBar;
    if (name == "قائمة_جانبية" || name == "sidemenu") return WidgetType::SideMenu;
    // تنبيهات (10)
    if (name == "حوار" || name == "dialog") return WidgetType::Dialog;
    if (name == "تنبيه" || name == "alert") return WidgetType::Alert;
    if (name == "اشعار_مؤقت" || name == "toast") return WidgetType::Toast;
    if (name == "شريط_اشعار" || name == "snackbar") return WidgetType::Snackbar;
    if (name == "لافتة" || name == "banner") return WidgetType::Banner;
    if (name == "نافذة_مشروطة" || name == "modal") return WidgetType::Modal;
    if (name == "منبثقة" || name == "popover") return WidgetType::Popover;
    if (name == "اشعار" || name == "notification") return WidgetType::Notification;
    if (name == "تأكيد" || name == "confirm") return WidgetType::ConfirmDialog;
    if (name == "تلميح" || name == "tooltip") return WidgetType::Tooltip;
    // بطاقات (7)
    if (name == "بطاقة_معلومات" || name == "infocard") return WidgetType::InfoCard;
    if (name == "بطاقة_احصائيات" || name == "statcard") return WidgetType::StatCard;
    if (name == "لوحة_توسيع" || name == "expansion") return WidgetType::ExpansionPanel;
    if (name == "اكورديون" || name == "accordion") return WidgetType::Accordion;
    if (name == "لوحة" || name == "panel") return WidgetType::Panel;
    if (name == "صندوق_مجموعة" || name == "groupbox") return WidgetType::GroupBox;
    if (name == "تنويه" || name == "callout") return WidgetType::Callout;
    // بيانات (10)
    if (name == "عنصر_بيانات" || name == "listtile") return WidgetType::ListTile;
    if (name == "جدول_بيانات" || name == "datatable") return WidgetType::DataTable;
    if (name == "صف_جدول" || name == "tablerow") return WidgetType::TableRow;
    if (name == "خلية" || name == "tablecell") return WidgetType::TableCell;
    if (name == "صف_تفاصيل" || name == "detailrow") return WidgetType::DetailRow;
    if (name == "خط_زمني" || name == "timeline") return WidgetType::Timeline;
    if (name == "عنصر_زمني" || name == "timelineitem") return WidgetType::TimelineItem;
    if (name == "عنصر_شجرة" || name == "treeitem") return WidgetType::TreeItem;
    if (name == "خطوات" || name == "stepper") return WidgetType::Stepper;
    if (name == "مؤشر_خطوة" || name == "stepindicator") return WidgetType::StepIndicator;
    // رسوم بيانية (7)
    if (name == "رسم_اعمدة" || name == "barchart") return WidgetType::BarChart;
    if (name == "رسم_افقي" || name == "hbarchart") return WidgetType::HBarChart;
    if (name == "رسم_دائري" || name == "piechart") return WidgetType::PieChart;
    if (name == "رسم_خطي" || name == "linechart") return WidgetType::LineChart;
    if (name == "خط_مصغر" || name == "sparkline") return WidgetType::SparkLine;
    if (name == "مقياس" || name == "gauge") return WidgetType::Gauge;
    if (name == "رسم_حلقي" || name == "donutchart") return WidgetType::DonutChart;
    // ترقيم + متقدم (5)
    if (name == "ترقيم" || name == "pagination") return WidgetType::Pagination;
    if (name == "مؤشر_نقطي" || name == "dotindicator") return WidgetType::DotIndicator;
    if (name == "لوحة_رسم" || name == "canvas") return WidgetType::Canvas;
    if (name == "عرض_مقسم" || name == "splitview") return WidgetType::SplitView;
    if (name == "دوار" || name == "carousel") return WidgetType::Carousel;
    // افتراضي
    return WidgetType::Container;
}

// تطبيق خصائص من خريطة على مكون موجود
static void applyPropsToWidget(int id, const std::unordered_map<std::string, PropValue>& props) {
    Widget* w = getWidget(id);
    if (!w) return;

    for (auto& [key, val] : props) {
        // ─── المعرّف (الاسم النصي للمكون) ───
        if (key == "\xd8\xa7\xd9\x84\xd9\x85\xd8\xb9\xd8\xb1\xd9\x81" || key == "id" || key == "name") {
            if (val.type == PropValue::STRING) w->widgetName = val.strVal;
        }
        // ─── خصائص النص والمحتوى ───
        else if (key == "محتوى" || key == "نص" || key == "text" || key == "content") {
            if (val.type == PropValue::STRING) w->text = val.strVal;
        }
        else if (key == "عنوان_فرعي" || key == "subtitle") {
            if (val.type == PropValue::STRING) w->subtitle = val.strVal;
        }
        // ─── الأحجام والأبعاد ───
        else if (key == "حجم" || key == "حجم_خط" || key == "fontSize" || key == "size") {
            if (val.type == PropValue::NUMBER) w->fontSize = (float)val.numVal;
        }
        else if (key == "عرض" || key == "width") {
            if (val.type == PropValue::NUMBER) w->fixedWidth = (float)val.numVal;
        }
        else if (key == "ارتفاع" || key == "height") {
            if (val.type == PropValue::NUMBER) w->fixedHeight = (float)val.numVal;
        }
        else if (key == "مرونة" || key == "flex") {
            if (val.type == PropValue::NUMBER) w->flex = (float)val.numVal;
        }
        // ─── التباعد والحشوة ───
        else if (key == "حشوة" || key == "padding") {
            if (val.type == PropValue::NUMBER) {
                float p = (float)val.numVal;
                w->padding = p; w->padTop = p; w->padRight = p; w->padBottom = p; w->padLeft = p;
            }
        }
        else if (key == "تباعد" || key == "spacing") {
            if (val.type == PropValue::NUMBER) w->spacing = (float)val.numVal;
        }
        // ─── الألوان ───
        else if (key == "لون" || key == "لون_نص" || key == "color" || key == "textColor") {
            if (val.type == PropValue::COLOR) w->textColor = val.colorVal;
            else if (val.type == PropValue::STRING) w->textColor = parseColor(val.strVal);
        }
        else if (key == "خلفية" || key == "لون_خلفية" || key == "bg" || key == "background") {
            if (val.type == PropValue::COLOR) w->bgColor = val.colorVal;
            else if (val.type == PropValue::STRING) w->bgColor = parseColor(val.strVal);
        }
        else if (key == "لون_زر" || key == "btnColor") {
            RGBA bc = (val.type == PropValue::COLOR) ? val.colorVal : parseColor(val.strVal);
            w->btnColor = bc;
            w->btnHover = {std::min(bc.r+30,255), std::min(bc.g+30,255), std::min(bc.b+30,255), 255};
            w->btnPress = {std::max(bc.r-30,0), std::max(bc.g-30,0), std::max(bc.b-30,0), 255};
        }
        else if (key == "لون_تمييز" || key == "accent") {
            if (val.type == PropValue::COLOR) w->accentColor = val.colorVal;
            else if (val.type == PropValue::STRING) w->accentColor = parseColor(val.strVal);
        }
        else if (key == "لون_حدود" || key == "borderColor") {
            if (val.type == PropValue::COLOR) w->borderColor = val.colorVal;
            else if (val.type == PropValue::STRING) w->borderColor = parseColor(val.strVal);
        }
        // ─── الحدود والزوايا ───
        else if (key == "حدود" || key == "زوايا" || key == "borderRadius" || key == "radius") {
            if (val.type == PropValue::NUMBER) w->borderRadius = (float)val.numVal;
        }
        else if (key == "سمك_حدود" || key == "borderWidth") {
            if (val.type == PropValue::NUMBER) w->borderWidth = (float)val.numVal;
        }
        // ─── المحاذاة ───
        else if (key == "محاذاة" || key == "align" || key == "textAlign") {
            if (val.type == PropValue::STRING) w->textAlign = val.strVal;
        }
        // ─── الحالة ───
        else if (key == "مرئي" || key == "visible") {
            if (val.type == PropValue::BOOL) w->visible = val.boolVal;
        }
        else if (key == "محدد" || key == "checked") {
            if (val.type == PropValue::BOOL) w->checked = val.boolVal;
        }
        else if (key == "قيمة" || key == "value") {
            if (val.type == PropValue::NUMBER) w->value = (float)val.numVal;
        }
        else if (key == "أدنى" || key == "ادنى" || key == "min") {
            if (val.type == PropValue::NUMBER) w->minVal = (float)val.numVal;
        }
        else if (key == "أقصى" || key == "اقصى" || key == "max") {
            if (val.type == PropValue::NUMBER) w->maxVal = (float)val.numVal;
        }
        else if (key == "مختار" || key == "selected") {
            if (val.type == PropValue::NUMBER) w->selectedIndex = (int)val.numVal;
        }
        else if (key == "موسع" || key == "expanded") {
            if (val.type == PropValue::BOOL) w->expanded = val.boolVal;
        }
        else if (key == "نشط" || key == "active") {
            if (val.type == PropValue::BOOL) w->active = val.boolVal;
        }
        else if (key == "مستوى" || key == "level") {
            if (val.type == PropValue::NUMBER) w->level = (int)val.numVal;
        }
        else if (key == "أعمدة" || key == "اعمدة" || key == "columns") {
            if (val.type == PropValue::NUMBER) w->columns = (int)val.numVal;
        }
        else if (key == "صفحة" || key == "page") {
            if (val.type == PropValue::NUMBER) w->currentPage = (int)val.numVal;
        }
        else if (key == "ايقونة" || key == "icon") {
            if (val.type == PropValue::STRING && !val.strVal.empty()) w->iconChar = val.strVal[0];
        }
        else if (key == "تلميح" || key == "hint") {
            if (val.type == PropValue::STRING) w->text = val.strVal;
        }
    }
}

// إنشاء مكون جديد من اسم النوع وخريطة الخصائص
int widget_create_from_props(const std::string& typeName,
                             const std::unordered_map<std::string, PropValue>& props) {
    WidgetType type = resolveTypeName(typeName);
    int id = createWidget(type);
    applyPropsToWidget(id, props);
    return id;
}

// تطبيق خصائص على مكون موجود — واجهة عامة
void apply_props_to_widget(int id, const std::unordered_map<std::string, PropValue>& props) {
    applyPropsToWidget(id, props);
}

// إنشاء تطبيق من خريطة خصائص
int app_create_from_props(const std::unordered_map<std::string, PropValue>& props) {
    std::string title = "SAD App";
    int width = 800, height = 600;
    RGBA bg = {245, 245, 245, 255};

    for (auto& [key, val] : props) {
        if ((key == "عنوان" || key == "title") && val.type == PropValue::STRING)
            title = val.strVal;
        else if ((key == "عرض" || key == "width") && val.type == PropValue::NUMBER)
            width = (int)val.numVal;
        else if ((key == "ارتفاع" || key == "height") && val.type == PropValue::NUMBER)
            height = (int)val.numVal;
        else if ((key == "خلفية" || key == "bg" || key == "background")) {
            if (val.type == PropValue::STRING) bg = parseColor(val.strVal);
            else if (val.type == PropValue::COLOR) bg = val.colorVal;
        }
    }

    int appId = app_create(title, width, height);
    app_set_bg_color(appId, bg.r, bg.g, bg.b);
    return appId;
}

// ═══════════════════════════════════════════════════════════════════
// القسم 12: النظام التفاعلي — الثيمات والأحداث والتنظيف
// ═══════════════════════════════════════════════════════════════════

// ───── الثيمة العالمية ─────
static Theme g_theme;
static std::map<int, std::string> g_prevTextStates; // لتتبع تغيّرات النصوص

const Theme& getTheme() { return g_theme; }

void setThemeProperty(const std::string& propName, const std::string& strValue) {
    RGBA color = parseColor(strValue);
    if (propName == "أساسي" || propName == "primary")          g_theme.primaryColor = color;
    else if (propName == "ثانوي" || propName == "secondary")    g_theme.secondaryColor = color;
    else if (propName == "خلفية" || propName == "background")   g_theme.backgroundColor = color;
    else if (propName == "سطح" || propName == "surface")        g_theme.surfaceColor = color;
    else if (propName == "نص" || propName == "text")            g_theme.textColor = color;
    else if (propName == "نص_ثانوي" || propName == "textSecondary") g_theme.textSecondary = color;
    else if (propName == "خطأ" || propName == "error")          g_theme.errorColor = color;
    else if (propName == "نجاح" || propName == "success")       g_theme.successColor = color;
    else if (propName == "تحذير" || propName == "warning")      g_theme.warningColor = color;
    else if (propName == "فاصل" || propName == "divider")       g_theme.dividerColor = color;
}

void setThemeProperty(const std::string& propName, double numValue) {
    float v = (float)numValue;
    if (propName == "خط_صغير" || propName == "fontSmall")       g_theme.fontSizeSmall = v;
    else if (propName == "خط" || propName == "fontSize")        g_theme.fontSizeMedium = v;
    else if (propName == "خط_كبير" || propName == "fontLarge")  g_theme.fontSizeLarge = v;
    else if (propName == "خط_عنوان" || propName == "fontTitle") g_theme.fontSizeTitle = v;
    else if (propName == "خط_رئيسي" || propName == "fontHeading") g_theme.fontSizeHeading = v;
    else if (propName == "حشوة" || propName == "padding")       g_theme.paddingMedium = v;
    else if (propName == "حشوة_صغيرة" || propName == "paddingSmall") g_theme.paddingSmall = v;
    else if (propName == "حشوة_كبيرة" || propName == "paddingLarge") g_theme.paddingLarge = v;
    else if (propName == "تباعد" || propName == "spacing")      g_theme.spacingDefault = v;
    else if (propName == "حدود" || propName == "radius")        g_theme.borderRadius = v;
    else if (propName == "سمك_حدود" || propName == "borderWidth") g_theme.borderWidth = v;
}

// ───── جمع الأحداث ─────
// يمسح جميع الودجات أسفل الجذر ويكتشف الأحداث التي حصلت

static void collectEventsRecursive(int id, std::vector<UIEvent>& events) {
    Widget* w = getWidget(id);
    if (!w || !w->visible) return;
    
    // اكتشاف ضغط الأزرار
    if (w->wasClicked) {
        UIEvent ev;
        ev.widgetId = id;
        std::string evName = w->widgetName.empty() ? w->text : w->widgetName;
        
        switch (w->type) {
            case WidgetType::Button:
            case WidgetType::IconButton:
            case WidgetType::FloatingButton:
            case WidgetType::OutlineButton:
            case WidgetType::TextButton:
            case WidgetType::ToggleButton: {
                ev.type = "\xd8\xb6\xd8\xba\xd8\xb7"; // "ضغط"
                ev.textData = evName;
                events.push_back(ev);
                break;
            }
            case WidgetType::Checkbox:
            case WidgetType::Switch: {
                ev.type = "\xd8\xaa\xd8\xa8\xd8\xaf\xd9\x8a\xd9\x84"; // "تبديل"
                ev.textData = evName;
                ev.boolData = w->checked;
                events.push_back(ev);
                break;
            }
            case WidgetType::NavItem:
            case WidgetType::MenuItem:
            case WidgetType::ListTile: {
                ev.type = "\xd8\xb6\xd8\xba\xd8\xb7"; // "ضغط"
                ev.textData = evName;
                events.push_back(ev);
                break;
            }
            default: {
                ev.type = "\xd8\xb6\xd8\xba\xd8\xb7"; // "ضغط"
                ev.textData = evName;
                events.push_back(ev);
                break;
            }
        }
        w->wasClicked = false; // مسح العلامة بعد الجمع
    }
    
    // اكتشاف تغيّرات حقول النص
    bool isTextField = (w->type == WidgetType::TextField || 
                        w->type == WidgetType::SearchField ||
                        w->type == WidgetType::PasswordField ||
                        w->type == WidgetType::TextArea ||
                        w->type == WidgetType::NumberInput);
    
    if (isTextField) {
        auto prevIt = g_prevTextStates.find(id);
        std::string prevText = (prevIt != g_prevTextStates.end()) ? prevIt->second : "";
        
        if (w->inputText != prevText) {
            UIEvent ev;
            ev.type = "\xd9\x86\xd8\xb5"; // "نص"
            ev.widgetId = id;
            ev.textData = w->inputText;
            events.push_back(ev);
            g_prevTextStates[id] = w->inputText;
        }
    }
    
    // اكتشاف تغيّرات المنزلقات
    if (w->type == WidgetType::Slider || w->type == WidgetType::RangeSlider) {
        // نستخدم animProgress مؤقتاً لتخزين القيمة السابقة
        // (هذا اختراق بسيط — يمكن تحسينه لاحقاً)
        UIEvent ev;
        ev.type = "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9"; // "قيمة"
        ev.widgetId = id;
        ev.numData = w->value;
        ev.textData = w->text;
        // لا نضيف الحدث إلا إذا تغيّرت القيمة فعلاً — يُترك للطبقة العليا
        events.push_back(ev);
    }
    
    // تكرار للأبناء
    for (int childId : w->children) {
        collectEventsRecursive(childId, events);
    }
}

std::vector<UIEvent> collectEvents(int rootWidget) {
    std::vector<UIEvent> events;
    collectEventsRecursive(rootWidget, events);
    return events;
}

// ───── حفظ واستعادة حالة الحقول ─────

static void saveTextStatesRecursive(int id, std::map<std::string, std::string>& states, int& counter) {
    Widget* w = getWidget(id);
    if (!w) return;
    
    bool isTextField = (w->type == WidgetType::TextField || 
                        w->type == WidgetType::SearchField ||
                        w->type == WidgetType::PasswordField ||
                        w->type == WidgetType::TextArea ||
                        w->type == WidgetType::NumberInput);
    
    if (isTextField) {
        // المفتاح يجمع بين النص التلميحي والترتيب لضمان التفرد
        std::string key = w->text + "##" + std::to_string(counter);
        states[key] = w->inputText;
        counter++;
    }
    
    for (int childId : w->children) {
        saveTextStatesRecursive(childId, states, counter);
    }
}

std::map<std::string, std::string> saveTextFieldStates(int rootWidget) {
    std::map<std::string, std::string> states;
    int counter = 0;
    saveTextStatesRecursive(rootWidget, states, counter);
    return states;
}

static void restoreTextStatesRecursive(int id, const std::map<std::string, std::string>& states, int& counter) {
    Widget* w = getWidget(id);
    if (!w) return;
    
    bool isTextField = (w->type == WidgetType::TextField || 
                        w->type == WidgetType::SearchField ||
                        w->type == WidgetType::PasswordField ||
                        w->type == WidgetType::TextArea ||
                        w->type == WidgetType::NumberInput);
    
    if (isTextField) {
        std::string key = w->text + "##" + std::to_string(counter);
        auto it = states.find(key);
        if (it != states.end()) {
            w->inputText = it->second;
        }
        counter++;
    }
    
    for (int childId : w->children) {
        restoreTextStatesRecursive(childId, states, counter);
    }
}

void restoreTextFieldStates(int rootWidget, const std::map<std::string, std::string>& states) {
    int counter = 0;
    restoreTextStatesRecursive(rootWidget, states, counter);
}

// ───── تنظيف الودجات ─────

void widgetCleanupAll() {
    g_widgets.clear();
    g_prevTextStates.clear();
    g_nextWidgetId = 1;
}

static void cleanupTreeRecursive(int id, std::set<int>& toDelete) {
    Widget* w = getWidget(id);
    if (!w) return;
    toDelete.insert(id);
    for (int childId : w->children) {
        cleanupTreeRecursive(childId, toDelete);
    }
}

void widgetCleanupTree(int rootId) {
    std::set<int> toDelete;
    cleanupTreeRecursive(rootId, toDelete);
    for (int id : toDelete) {
        g_widgets.erase(id);
        g_prevTextStates.erase(id);
    }
}

// ───── ضبط خاصية واحدة ─────

void widgetSetProp(int id, const std::string& propName, const std::string& strValue) {
    Widget* w = getWidget(id);
    if (!w) return;
    
    if (propName == "\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89" || propName == "محتوى" || propName == "نص" || propName == "content" || propName == "text")
        w->text = strValue;
    else if (propName == "عنوان_فرعي" || propName == "subtitle")
        w->subtitle = strValue;
    else if (propName == "محاذاة" || propName == "align" || propName == "textAlign")
        w->textAlign = strValue;
    else if (propName == "لون" || propName == "لون_نص" || propName == "color") 
        w->textColor = parseColor(strValue);
    else if (propName == "خلفية" || propName == "لون_خلفية" || propName == "bg" || propName == "background")
        w->bgColor = parseColor(strValue);
    else if (propName == "لون_زر" || propName == "btnColor") {
        RGBA bc = parseColor(strValue);
        w->btnColor = bc;
        w->btnHover = {std::min(bc.r+30,255), std::min(bc.g+30,255), std::min(bc.b+30,255), 255};
        w->btnPress = {std::max(bc.r-30,0), std::max(bc.g-30,0), std::max(bc.b-30,0), 255};
    }
    else if (propName == "لون_تمييز" || propName == "accent")
        w->accentColor = parseColor(strValue);
    else if (propName == "لون_حدود" || propName == "borderColor")
        w->borderColor = parseColor(strValue);
    else if (propName == "تلميح" || propName == "hint")
        w->text = strValue;
    else if (propName == "ايقونة" || propName == "icon") {
        if (!strValue.empty()) w->iconChar = strValue[0];
    }
}

void widgetSetProp(int id, const std::string& propName, double numValue) {
    Widget* w = getWidget(id);
    if (!w) return;
    float v = (float)numValue;
    
    if (propName == "حجم" || propName == "حجم_خط" || propName == "fontSize")
        w->fontSize = v;
    else if (propName == "عرض" || propName == "width")
        w->fixedWidth = v;
    else if (propName == "ارتفاع" || propName == "height")
        w->fixedHeight = v;
    else if (propName == "مرونة" || propName == "flex")
        w->flex = v;
    else if (propName == "حشوة" || propName == "padding") {
        w->padding = v; w->padTop = v; w->padRight = v; w->padBottom = v; w->padLeft = v;
    }
    else if (propName == "تباعد" || propName == "spacing")
        w->spacing = v;
    else if (propName == "حدود" || propName == "زوايا" || propName == "radius" || propName == "borderRadius")
        w->borderRadius = v;
    else if (propName == "سمك_حدود" || propName == "borderWidth")
        w->borderWidth = v;
    else if (propName == "قيمة" || propName == "value")
        w->value = v;
    else if (propName == "أدنى" || propName == "ادنى" || propName == "min")
        w->minVal = v;
    else if (propName == "أقصى" || propName == "اقصى" || propName == "max")
        w->maxVal = v;
    else if (propName == "مختار" || propName == "selected")
        w->selectedIndex = (int)numValue;
    else if (propName == "مستوى" || propName == "level")
        w->level = (int)numValue;
    else if (propName == "أعمدة" || propName == "اعمدة" || propName == "columns")
        w->columns = (int)numValue;
    else if (propName == "صفحة" || propName == "page")
        w->currentPage = (int)numValue;
}

void widgetSetProp(int id, const std::string& propName, bool boolValue) {
    Widget* w = getWidget(id);
    if (!w) return;
    
    if (propName == "مرئي" || propName == "visible") w->visible = boolValue;
    else if (propName == "محدد" || propName == "checked") w->checked = boolValue;
    else if (propName == "موسع" || propName == "expanded") w->expanded = boolValue;
    else if (propName == "نشط" || propName == "active") w->active = boolValue;
    else if (propName == "مقنع" || propName == "masked") w->masked = boolValue;
}

// ───── ضبط الأطفال من مصفوفة ─────

void widgetSetChildren(int parentId, const std::vector<int>& childIds) {
    Widget* w = getWidget(parentId);
    if (!w) return;
    w->children.clear();
    for (int childId : childIds) {
        w->children.push_back(childId);
    }
}

}} // namespace sad::ui
