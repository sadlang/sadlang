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

#include "sad_ui_internal.h"
#include "sad_animation.h"
#include <algorithm>
#include <cmath>
#include <set>
#include <map>
#include <cstring>
#include <iostream>
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

const unsigned char FONT_DATA[96][7] = {
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

std::unordered_map<int, Widget> g_widgets;  // سجل العناصر
int g_nextWidgetId = 1;                      // المعرّف التالي

// AppState struct is defined in sad_ui_internal.h

std::unordered_map<int, AppState> g_apps;  // تطبيقات مفتوحة
int g_nextAppId = 1;

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
AppState* getApp(int id) {
    auto it = g_apps.find(id);
    return (it != g_apps.end()) ? &it->second : nullptr;
}

// إنشاء عنصر جديد وإرجاع معرّفه
int createWidget(WidgetType type) {
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

FontData g_font;  // الخط الرئيسي (عالمي)

// تحميل ملف خط TrueType
bool loadFontFile(const std::string& path) {
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
void ensureFontLoaded() {
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

bool isFontLoaded() { return g_font.loaded; }

// الحصول على حرف TrueType مع تخزين مؤقت
GlyphInfo* getGlyph(SDL_Renderer* renderer, int codepoint, int pixelSize) {
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
void drawCharBitmap(SDL_Renderer* r, char c, int x, int y, int scale, RGBA color) {
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
int measureTextWidth(const std::string& text, int scaleOrPixelSize) {
    if (text.empty()) return 0;
    if (g_font.loaded) {
        return measureTextWidthTTF(text, scaleOrPixelSize);
    }
    // النظام النقطي الاحتياطي
    return (int)text.size() * 6 * scaleOrPixelSize;
}

// ارتفاع النص
int measureTextHeight(int scaleOrPixelSize) {
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
void drawText(SDL_Renderer* r, const std::string& text, int x, int y,
                     int scaleOrPixelSize, RGBA color, const std::string& align,
                     int containerWidth) {
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
}} // namespace sad::ui
