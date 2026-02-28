/*
 * ═══════════════════════════════════════════════════════════════════════════
 *  BeeOS HD — واجهة الإعدادات الرسومية المتكاملة
 *  Settings UI — Graphical Settings Interface
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ
 *
 * هذا الملف يحتوي على واجهة إعدادات رسومية متكاملة لنظام BeeOS.
 * يتم الرسم مباشرة على الـ framebuffer باستخدام الدوال الأساسية:
 *   - fb_pixel()     : رسم بكسل واحد
 *   - fb_rect()      : مستطيل مملوء
 *   - fb_outline()   : إطار مستطيل
 *   - fb_hline()     : خط أفقي
 *   - fb_vline()     : خط عمودي
 *   - fb_char()      : حرف واحد (خط 5×7 مكبّر)
 *   - fb_string()    : سلسلة نصية
 *
 * ─── هيكل واجهة الإعدادات ───
 *
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │ [✕]  BeeOS Settings                              [_] [□]  │ ← عنوان
 *   ├────────────┬────────────────────────────────────────────────┤
 *   │ > Display  │                                               │
 *   │   Themes   │   [المحتوى يتغير حسب التبويب المحدد]          │
 *   │   Language │                                               │
 *   │   Sound    │   عرض/سمات/لغة/صوت/شبكة/أمان/نظام/حول       │
 *   │   Network  │                                               │
 *   │   Security │                                               │
 *   │   System   │                                               │
 *   │   About    │                                               │
 *   ├────────────┴────────────────────────────────────────────────┤
 *   │ [Apply]  [Reset]  [Close]                                  │ ← أزرار
 *   └─────────────────────────────────────────────────────────────┘
 *
 * ─── التبويبات (8 أقسام) ───
 *
 *   1. العرض (Display)     — الدقة، السطوع، معدل التحديث
 *   2. السمات (Themes)     — 5 سمات مدمجة + معاينة حية
 *   3. اللغة (Language)    — العربية/الإنجليزية، تنسيق الوقت
 *   4. الصوت (Sound)       — مستوى الصوت، التنبيهات
 *   5. الشبكة (Network)    — IP، اسم المضيف، DNS
 *   6. الأمان (Security)   — كلمة المرور، الجدار الناري
 *   7. النظام (System)     — اسم الجهاز، المنطقة الزمنية
 *   8. حول (About)         — معلومات النظام، الترخيص
 *
 * ─── عناصر التحكم المرسومة ───
 *
 *   ☐/☑  مربع اختيار (Checkbox)
 *   ○/●  زر راديو (Radio button)
 *   ─▓── شريط تمرير (Slider)
 *   [▼]  قائمة منسدلة (Dropdown)
 *   [زر]  زر عادي (Button)
 *   ═══  فاصل أفقي (Separator)
 *   📋  حقل نصي (Text field)
 *
 * الكود يعمل على الأجهزة الحقيقية — لا يعتمد على أي مكتبة خارجية.
 * جميع الرسومات بالبكسل مباشرة على الـ Linear Framebuffer.
 *
 * الترخيص: وقف إسلامي — BeeWaqf 1447هـ
 * التاريخ: 2025-06-28
 */

#include "beeos_kernel.h"

/* ═══════════════════════════════════════════════════════════════════
 *  الدوال الخارجية من fb_desktop.c
 *  External drawing primitives (defined in fb_desktop.c)
 * ═══════════════════════════════════════════════════════════════════
 *
 * هذه الدوال معرّفة في fb_desktop.c ونستخدمها هنا لرسم عناصر
 * واجهة الإعدادات. الدوال الأساسية:
 *   - fb_pixel: رسم بكسل واحد مع فحص الحدود
 *   - fb_rect: مستطيل مملوء (حلقتان متداخلتان)
 *   - fb_outline: إطار بسمك 1 بكسل
 *   - fb_hline/fb_vline: خطوط أفقية/عمودية
 *   - fb_char: حرف واحد بخط نقطي 5×7 مع تكبير
 *   - fb_string: سلسلة نصية (أحرف متتالية)
 */
extern void fb_pixel(int x, int y, unsigned int color);
extern void fb_rect(int x, int y, int w, int h, unsigned int color);
extern void fb_outline(int x, int y, int w, int h, unsigned int color);
extern void fb_hline(int x, int y, int w, unsigned int color);
extern void fb_vline(int x, int y, int h, unsigned int color);
extern void fb_char(int cx, int cy, char ch, unsigned int color, int scale);
extern void fb_string(int x, int y, const char *s, unsigned int color, int scale);

/* ═══════════════════════════════════════════════════════════════════
 *  ثوابت الألوان — لوحة ألوان واجهة الإعدادات
 * ═══════════════════════════════════════════════════════════════════
 *
 * لوحة الألوان مستوحاة من سمة "النحلة الذهبية":
 *   - خلفية النافذة: أبيض فاتح (لوضوح القراءة)
 *   - شريط العنوان: ذهبي داكن
 *   - الشريط الجانبي: رمادي فاتح
 *   - العنصر المحدد: ذهبي عسلي
 *   - النص الأساسي: أسود
 *   - النص الثانوي: رمادي
 */

/* ─── خلفية وإطار النافذة ─── */
#define SET_WIN_BG         0xF5F5F0   /* خلفية النافذة — أبيض عاجي */
#define SET_TITLE_BG       0x8B6914   /* خلفية العنوان — ذهبي داكن */
#define SET_TITLE_TEXT     0xFFD700   /* نص العنوان — ذهبي فاتح */
#define SET_BORDER         0x6B4F10   /* حدود النافذة — بني ذهبي */

/* ─── الشريط الجانبي (التبويبات) ─── */
#define SET_SIDEBAR_BG     0xE8E0D0   /* خلفية الشريط الجانبي */
#define SET_SIDEBAR_TEXT   0x3A3A3A   /* نص التبويب العادي */
#define SET_SIDEBAR_SEL    0xDAA520   /* خلفية التبويب المحدد */
#define SET_SIDEBAR_HOVER  0xEEDC82   /* خلفية التبويب عند المرور */

/* ─── منطقة المحتوى ─── */
#define SET_CONTENT_BG     0xFFFFF0   /* خلفية المحتوى — أبيض كريمي */
#define SET_CONTENT_TEXT   0x1A1A1A   /* النص الأساسي — أسود */
#define SET_CONTENT_SEC    0x808080   /* النص الثانوي — رمادي */
#define SET_CONTENT_LABEL  0x4A4A4A   /* تسمية الحقل — رمادي داكن */

/* ─── عناصر التحكم ─── */
#define SET_BTN_BG         0xDAA520   /* خلفية الزر — ذهبي */
#define SET_BTN_TEXT       0xFFFFFF   /* نص الزر — أبيض */
#define SET_BTN_BORDER     0x8B6914   /* حدود الزر */
#define SET_BTN_APPLY      0x2E8B57   /* زر تطبيق — أخضر */
#define SET_BTN_RESET      0xCC6600   /* زر إعادة ضبط — برتقالي */
#define SET_BTN_CLOSE      0xB84040   /* زر إغلاق — أحمر باهت */

/* ─── مربعات الاختيار والمؤشرات ─── */
#define SET_CHECK_BG       0xFFFFFF   /* خلفية مربع الاختيار */
#define SET_CHECK_MARK     0x2E8B57   /* علامة الاختيار — أخضر */
#define SET_CHECK_BORDER   0x808080   /* حدود مربع الاختيار */
#define SET_RADIO_FILL     0xDAA520   /* ملء زر الراديو */
#define SET_SLIDER_BG      0xD0D0D0   /* خلفية شريط التمرير */
#define SET_SLIDER_FILL    0xDAA520   /* ملء شريط التمرير */
#define SET_SLIDER_KNOB    0x8B6914   /* مقبض شريط التمرير */

/* ─── حقول الإدخال ─── */
#define SET_INPUT_BG       0xFFFFFF   /* خلفية حقل الإدخال */
#define SET_INPUT_BORDER   0xAAAAAA   /* حدود حقل الإدخال */
#define SET_INPUT_FOCUS    0xDAA520   /* حدود عند التركيز */
#define SET_INPUT_TEXT     0x1A1A1A   /* نص الإدخال */

/* ─── ألوان متنوعة ─── */
#define SET_SEPARATOR      0xC8C0B0   /* لون الفاصل الأفقي */
#define SET_SHADOW         0x2A2A2A   /* ظل النافذة */
#define SET_SECTION_TITLE  0x6B4F10   /* عنوان القسم */
#define SET_HIGHLIGHT      0xFFF8DC   /* تمييز — أصفر فاتح */
#define SET_INFO_BG        0xE8F4FD   /* خلفية معلومات — أزرق فاتح */
#define SET_INFO_BORDER    0x4A90D9   /* حدود معلومات — أزرق */
#define SET_WARN_BG        0xFFF3CD   /* خلفية تحذير — أصفر */
#define SET_WARN_BORDER    0xE0A800   /* حدود تحذير — برتقالي */
#define SET_SUCCESS_BG     0xD4EDDA   /* خلفية نجاح — أخضر فاتح */
#define SET_SUCCESS_BORDER 0x28A745   /* حدود نجاح — أخضر */
#define SET_WHITE          0xFFFFFF
#define SET_BLACK          0x000000

/* ═══════════════════════════════════════════════════════════════════
 *  أبعاد النافذة وعناصرها
 * ═══════════════════════════════════════════════════════════════════
 *
 * النافذة الرئيسية: 700×520 بكسل (كبيرة لاستيعاب الإعدادات)
 * شريط العنوان: 32 بكسل ارتفاع
 * الشريط الجانبي: 140 بكسل عرض
 * شريط الأزرار السفلي: 44 بكسل ارتفاع
 * منطقة المحتوى: 700 - 140 = 560 × (520 - 32 - 44) = 444 بكسل
 */
#define SET_WIN_X     150    /* موضع النافذة — أفقي */
#define SET_WIN_Y      60    /* موضع النافذة — عمودي */
#define SET_WIN_W     700    /* عرض النافذة */
#define SET_WIN_H     520    /* ارتفاع النافذة */
#define SET_TITLE_H    32    /* ارتفاع شريط العنوان */
#define SET_SIDEBAR_W  140   /* عرض الشريط الجانبي */
#define SET_BUTTON_H   44    /* ارتفاع شريط الأزرار */
#define SET_TAB_H      28    /* ارتفاع كل تبويب */

/* ─── حساب مناطق الرسم ─── */
#define SET_CONTENT_X  (SET_WIN_X + SET_SIDEBAR_W)          /* بداية المحتوى — أفقي */
#define SET_CONTENT_Y  (SET_WIN_Y + SET_TITLE_H)            /* بداية المحتوى — عمودي */
#define SET_CONTENT_W  (SET_WIN_W - SET_SIDEBAR_W)          /* عرض المحتوى */
#define SET_CONTENT_H  (SET_WIN_H - SET_TITLE_H - SET_BUTTON_H) /* ارتفاع المحتوى */

/* ═══════════════════════════════════════════════════════════════════
 *  حالة واجهة الإعدادات
 * ═══════════════════════════════════════════════════════════════════
 *
 * التبويب الحالي: يحدد أي قسم يُعرض في منطقة المحتوى
 * القيم: 0=عرض, 1=سمات, 2=لغة, 3=صوت, 4=شبكة, 5=أمان, 6=نظام, 7=حول
 */
static int g_active_tab = 0;   /* التبويب النشط حالياً */

/* ─── قيم الإعدادات الحالية ─── */
/* العرض */
static int g_resolution = 2;      /* 0=640×480, 1=800×600, 2=1024×768, 3=1280×1024 */
static int g_brightness = 7;      /* 0-10 */
static int g_refresh_rate = 1;    /* 0=60Hz, 1=75Hz, 2=85Hz */
static int g_vsync = 1;           /* 0=off, 1=on */
static int g_fullscreen = 0;      /* 0=off, 1=on */

/* السمات */
static int g_theme = 0;           /* 0-4 (5 سمات مدمجة) */
static int g_dark_mode = 0;       /* 0=off, 1=on */
static int g_custom_wallpaper = 0;/* 0=gradient, 1=solid, 2=image */
static int g_transparency = 1;    /* 0=off, 1=on */
static int g_animations = 1;      /* 0=off, 1=on */

/* اللغة */
static int g_language = 0;        /* 0=عربي, 1=English */
static int g_clock_format = 0;    /* 0=24h, 1=12h */
static int g_date_format = 0;     /* 0=hijri, 1=gregorian, 2=both */
static int g_keyboard = 0;        /* 0=arabic, 1=english, 2=bilingual */

/* الصوت */
static int g_volume = 7;          /* 0-10 */
static int g_sound_enabled = 1;   /* 0=off, 1=on */
static int g_alerts = 1;          /* 0=off, 1=on */
static int g_startup_sound = 1;   /* 0=off, 1=on */

/* الشبكة */
static int g_network_enabled = 1; /* 0=off, 1=on */
static int g_dhcp = 1;            /* 0=static, 1=dhcp */
static int g_firewall = 1;        /* 0=off, 1=on */

/* الأمان */
static int g_password_required = 1; /* 0=off, 1=on */
static int g_auto_lock = 1;        /* 0=off, 1=on */
static int g_lock_timeout = 2;     /* 0=1min, 1=5min, 2=10min, 3=30min */
static int g_encryption = 1;       /* 0=off, 1=on */

/* ═══════════════════════════════════════════════════════════════════
 *  أدوات الرسم المساعدة
 *  Helper drawing utilities
 * ═══════════════════════════════════════════════════════════════════
 *
 * دوال مساعدة لرسم عناصر واجهة المستخدم المتكررة:
 * أزرار، مربعات اختيار، أشرطة تمرير، قوائم منسدلة
 */

/*
 * رسم ظل ناعم (4 بكسل)
 * يُرسم أسفل النافذة وعلى يمينها لإعطاء تأثير العمق
 */
static void draw_soft_shadow(int x, int y, int w, int h) {
    /* ظل سفلي (تدرج من داكن إلى شفاف) */
    unsigned int shadow_colors[4] = { 0x3A3A3A, 0x5A5A5A, 0x8A8A8A, 0xB0B0B0 };
    for (int i = 0; i < 4; i++) {
        fb_hline(x + 4 + i, y + h + i, w - 4, shadow_colors[i]);
        fb_vline(x + w + i, y + 4 + i, h - 4, shadow_colors[i]);
    }
}

/*
 * رسم زر مستطيل مع نص
 *
 * بنية الزر:
 *   ┌──────────────┐
 *   │   [النص]     │  ← ارتفاع 26px افتراضي
 *   └──────────────┘
 *   مع حدود 1px وظل خفيف
 *
 * @param x, y    الموضع (أعلى يسار)
 * @param w, h    الأبعاد
 * @param label   النص (إنجليزي — الخط النقطي لا يدعم العربي)
 * @param bg      لون الخلفية
 * @param fg      لون النص
 * @param border  لون الحدود
 */
static void draw_button(int x, int y, int w, int h,
                        const char *label, unsigned int bg,
                        unsigned int fg, unsigned int border) {
    /* ظل خفيف */
    fb_rect(x + 2, y + 2, w, h, 0xA0A0A0);
    /* خلفية الزر */
    fb_rect(x, y, w, h, bg);
    /* حدود الزر */
    fb_outline(x, y, w, h, border);
    /* حساب موضع النص (توسيط تقريبي) */
    int text_len = 0;
    for (const char *p = label; *p; p++) text_len++;
    int tx = x + (w - text_len * 8) / 2;
    int ty = y + (h - 10) / 2;
    fb_string(tx, ty, label, fg, 1);
}

/*
 * رسم مربع اختيار (Checkbox) مع تسمية
 *
 * الحالات:
 *   ☐  غير محدد (فارغ)
 *   ☑  محدد (علامة √ أخضر)
 *
 * @param x, y     الموضع
 * @param label    النص بجوار المربع
 * @param checked  1 = محدد, 0 = غير محدد
 */
static void draw_checkbox(int x, int y, const char *label, int checked) {
    int box_size = 14;

    /* خلفية المربع */
    fb_rect(x, y, box_size, box_size, SET_CHECK_BG);
    /* حدود المربع */
    fb_outline(x, y, box_size, box_size, SET_CHECK_BORDER);

    /* علامة الاختيار (✓) — خطان مائلان */
    if (checked) {
        /* ملء بلون أخضر فاتح */
        fb_rect(x + 1, y + 1, box_size - 2, box_size - 2, SET_SUCCESS_BG);
        /* رسم علامة ✓ */
        /* خط قصير نازل (\) */
        for (int i = 0; i < 4; i++) {
            fb_pixel(x + 3 + i, y + 6 + i, SET_CHECK_MARK);
            fb_pixel(x + 4 + i, y + 6 + i, SET_CHECK_MARK);
        }
        /* خط طويل صاعد (/) */
        for (int i = 0; i < 6; i++) {
            fb_pixel(x + 6 + i, y + 10 - i, SET_CHECK_MARK);
            fb_pixel(x + 7 + i, y + 10 - i, SET_CHECK_MARK);
        }
    }

    /* النص بجوار المربع */
    fb_string(x + box_size + 6, y + 2, label, SET_CONTENT_TEXT, 1);
}

/*
 * رسم زر راديو (Radio button) مع تسمية
 *
 * الحالات:
 *   ○  غير محدد (دائرة فارغة)
 *   ●  محدد (دائرة ممتلئة)
 *
 * @param x, y      الموضع
 * @param label     النص بجوار الزر
 * @param selected  1 = محدد, 0 = غير محدد
 */
static void draw_radio(int x, int y, const char *label, int selected) {
    int r = 6; /* نصف القطر */
    int cx_pos = x + r;
    int cy_pos = y + r;

    /* رسم دائرة (خوارزمية بسيطة — مربع مع قطع الزوايا) */
    /* الخلفية */
    fb_rect(x + 2, y, r * 2 - 4, r * 2, SET_CHECK_BG);
    fb_rect(x, y + 2, r * 2, r * 2 - 4, SET_CHECK_BG);
    fb_rect(x + 1, y + 1, r * 2 - 2, r * 2 - 2, SET_CHECK_BG);

    /* الحدود — 4 خطوط تشكل شكل دائري تقريبي */
    fb_hline(x + 3, y, r * 2 - 6, SET_CHECK_BORDER);
    fb_hline(x + 3, y + r * 2 - 1, r * 2 - 6, SET_CHECK_BORDER);
    fb_vline(x, y + 3, r * 2 - 6, SET_CHECK_BORDER);
    fb_vline(x + r * 2 - 1, y + 3, r * 2 - 6, SET_CHECK_BORDER);
    /* زوايا */
    fb_pixel(x + 1, y + 1, SET_CHECK_BORDER);
    fb_pixel(x + 1, y + 2, SET_CHECK_BORDER);
    fb_pixel(x + 2, y + 1, SET_CHECK_BORDER);
    fb_pixel(x + r * 2 - 2, y + 1, SET_CHECK_BORDER);
    fb_pixel(x + r * 2 - 2, y + 2, SET_CHECK_BORDER);
    fb_pixel(x + r * 2 - 3, y + 1, SET_CHECK_BORDER);
    fb_pixel(x + 1, y + r * 2 - 2, SET_CHECK_BORDER);
    fb_pixel(x + 1, y + r * 2 - 3, SET_CHECK_BORDER);
    fb_pixel(x + 2, y + r * 2 - 2, SET_CHECK_BORDER);
    fb_pixel(x + r * 2 - 2, y + r * 2 - 2, SET_CHECK_BORDER);
    fb_pixel(x + r * 2 - 2, y + r * 2 - 3, SET_CHECK_BORDER);
    fb_pixel(x + r * 2 - 3, y + r * 2 - 2, SET_CHECK_BORDER);

    /* ملء الدائرة الداخلية (إذا محدد) */
    if (selected) {
        fb_rect(cx_pos - 3, cy_pos - 3, 6, 6, SET_RADIO_FILL);
        fb_rect(cx_pos - 2, cy_pos - 4, 4, 8, SET_RADIO_FILL);
        fb_rect(cx_pos - 4, cy_pos - 2, 8, 4, SET_RADIO_FILL);
    }

    /* النص */
    fb_string(x + r * 2 + 6, y + 2, label, SET_CONTENT_TEXT, 1);
}

/*
 * رسم شريط تمرير أفقي (Slider)
 *
 * الهيكل:
 *   ─────▓─────  ← المسار (track) + المقبض (knob)
 *   0           10
 *
 * @param x, y    الموضع
 * @param w       العرض الكلي
 * @param value   القيمة الحالية (0 — max_val)
 * @param max_val القيمة القصوى
 * @param label   التسمية على اليسار
 */
static void draw_slider(int x, int y, int w, int value, int max_val,
                        const char *label) {
    int track_y = y + 6;
    int track_h = 6;
    int knob_w = 10;
    int knob_h = 16;

    /* التسمية */
    fb_string(x, y, label, SET_CONTENT_LABEL, 1);

    /* حساب موضع المقبض */
    int slider_x = x + 90;  /* بعد التسمية */
    int slider_w = w - 120;
    int fill_w = (max_val > 0) ? (value * slider_w / max_val) : 0;
    int knob_x = slider_x + fill_w - knob_w / 2;

    /* المسار (track) */
    fb_rect(slider_x, track_y, slider_w, track_h, SET_SLIDER_BG);
    fb_outline(slider_x, track_y, slider_w, track_h, SET_CHECK_BORDER);

    /* الجزء المملوء */
    if (fill_w > 0) {
        fb_rect(slider_x + 1, track_y + 1, fill_w, track_h - 2, SET_SLIDER_FILL);
    }

    /* المقبض (knob) */
    if (knob_x < slider_x) knob_x = slider_x;
    fb_rect(knob_x, y, knob_w, knob_h, SET_SLIDER_KNOB);
    fb_outline(knob_x, y, knob_w, knob_h, SET_BORDER);

    /* القيمة الرقمية */
    char val_buf[4];
    val_buf[0] = '0' + (value / 10);
    val_buf[1] = '0' + (value % 10);
    val_buf[2] = 0;
    if (value < 10) { val_buf[0] = '0' + value; val_buf[1] = 0; }
    fb_string(slider_x + slider_w + 8, y + 2, val_buf, SET_CONTENT_TEXT, 1);
}

/*
 * رسم قائمة منسدلة (Dropdown / Combobox)
 *
 * الهيكل:
 *   ┌────────────────┬──┐
 *   │  الخيار الحالي  │▼│
 *   └────────────────┴──┘
 *
 * @param x, y      الموضع
 * @param w         العرض
 * @param label     التسمية
 * @param options   مصفوفة الخيارات
 * @param count     عدد الخيارات
 * @param selected  الفهرس المحدد
 */
static void draw_dropdown(int x, int y, int w, const char *label,
                          const char **options, int count, int selected) {
    int h = 22;

    /* التسمية */
    fb_string(x, y + 4, label, SET_CONTENT_LABEL, 1);

    /* حقل القائمة */
    int dd_x = x + 90;
    int dd_w = w - 90;
    fb_rect(dd_x, y, dd_w, h, SET_INPUT_BG);
    fb_outline(dd_x, y, dd_w, h, SET_INPUT_BORDER);

    /* الخيار الحالي */
    if (selected >= 0 && selected < count) {
        fb_string(dd_x + 6, y + 6, options[selected], SET_INPUT_TEXT, 1);
    }

    /* سهم القائمة ▼ */
    int arrow_x = dd_x + dd_w - 18;
    fb_vline(dd_x + dd_w - 22, y + 1, h - 2, SET_INPUT_BORDER);
    /* رسم مثلث صغير */
    for (int row = 0; row < 5; row++) {
        fb_hline(arrow_x - row + 2, y + 7 + row, row * 2 + 1, SET_CONTENT_TEXT);
    }
}

/*
 * رسم حقل نصي (Text field / Input box)
 *
 * @param x, y    الموضع
 * @param w       العرض
 * @param label   التسمية
 * @param value   النص الحالي
 * @param focused 1 = حدود ذهبية (تركيز)
 */
static void draw_text_field(int x, int y, int w, const char *label,
                            const char *value, int focused) {
    int h = 22;

    /* التسمية */
    fb_string(x, y + 4, label, SET_CONTENT_LABEL, 1);

    /* حقل الإدخال */
    int field_x = x + 90;
    int field_w = w - 90;
    fb_rect(field_x, y, field_w, h, SET_INPUT_BG);
    fb_outline(field_x, y, field_w, h,
               focused ? SET_INPUT_FOCUS : SET_INPUT_BORDER);

    /* النص */
    fb_string(field_x + 6, y + 6, value, SET_INPUT_TEXT, 1);

    /* مؤشر الكتابة (إذا في وضع التركيز) */
    if (focused) {
        int cursor_x_pos = field_x + 6;
        for (const char *p = value; *p; p++) cursor_x_pos += 6;
        fb_vline(cursor_x_pos, y + 4, h - 8, SET_CONTENT_TEXT);
    }
}

/*
 * رسم فاصل أفقي مع عنوان اختياري
 *
 * الهيكل:
 *   ══════ العنوان ══════
 *
 * @param x, y    الموضع
 * @param w       العرض
 * @param title   العنوان (يمكن أن يكون NULL للفاصل فقط)
 */
static void draw_separator(int x, int y, int w, const char *title) {
    if (title) {
        /* حساب عرض النص */
        int text_len = 0;
        for (const char *p = title; *p; p++) text_len++;
        int text_w = text_len * 7;  /* حجم خط scale 1 */
        int left_w = (w - text_w - 12) / 2;
        int right_w = w - text_w - 12 - left_w;

        /* خط يسار */
        fb_hline(x, y + 4, left_w, SET_SEPARATOR);
        /* النص */
        fb_string(x + left_w + 6, y, title, SET_SECTION_TITLE, 1);
        /* خط يمين */
        fb_hline(x + left_w + text_w + 12, y + 4, right_w, SET_SEPARATOR);
    } else {
        fb_hline(x, y + 4, w, SET_SEPARATOR);
    }
}

/*
 * رسم صندوق معلومات (Info box)
 *
 * @param x, y    الموضع
 * @param w, h    الأبعاد
 * @param text    النص الداخلي
 * @param bg      لون الخلفية
 * @param border  لون الحدود
 * @param icon    رمز (حرف) في البداية
 */
static void draw_info_box(int x, int y, int w, int h,
                          const char *text, unsigned int bg,
                          unsigned int border, char icon) {
    fb_rect(x, y, w, h, bg);
    fb_outline(x, y, w, h, border);
    /* الرمز */
    fb_char(x + 8, y + (h - 7) / 2, icon, border, 1);
    /* النص */
    fb_string(x + 20, y + (h - 7) / 2, text, SET_CONTENT_TEXT, 1);
}

/* ═══════════════════════════════════════════════════════════════════
 *  رسم هيكل النافذة الرئيسية
 * ═══════════════════════════════════════════════════════════════════
 *
 * ترسم:
 *   1. الظل (4 بكسل)
 *   2. خلفية النافذة
 *   3. شريط العنوان مع أزرار التحكم
 *   4. الشريط الجانبي مع التبويبات
 *   5. فاصل عمودي بين الجانب والمحتوى
 *   6. شريط الأزرار السفلي
 */
static void draw_settings_frame(void) {
    /* ─── الظل ─── */
    draw_soft_shadow(SET_WIN_X, SET_WIN_Y, SET_WIN_W, SET_WIN_H);

    /* ─── خلفية النافذة الكاملة ─── */
    fb_rect(SET_WIN_X, SET_WIN_Y, SET_WIN_W, SET_WIN_H, SET_WIN_BG);

    /* ─── شريط العنوان ─── */
    fb_rect(SET_WIN_X, SET_WIN_Y, SET_WIN_W, SET_TITLE_H, SET_TITLE_BG);

    /* زر الإغلاق (✕) — أحمر */
    fb_rect(SET_WIN_X + 6, SET_WIN_Y + 6, 20, 20, 0xE04040);
    fb_char(SET_WIN_X + 10, SET_WIN_Y + 10, 'x', SET_WHITE, 1);

    /* زر التصغير (_) — أصفر */
    fb_rect(SET_WIN_X + SET_WIN_W - 52, SET_WIN_Y + 6, 20, 20, 0xFFCC00);
    fb_char(SET_WIN_X + SET_WIN_W - 48, SET_WIN_Y + 12, '-', SET_BLACK, 1);

    /* زر التكبير (□) — أخضر */
    fb_rect(SET_WIN_X + SET_WIN_W - 28, SET_WIN_Y + 6, 20, 20, 0x40C040);
    fb_char(SET_WIN_X + SET_WIN_W - 24, SET_WIN_Y + 10, '+', SET_WHITE, 1);

    /* عنوان النافذة */
    fb_string(SET_WIN_X + 34, SET_WIN_Y + 10, "BeeOS Settings", SET_TITLE_TEXT, 2);

    /* ─── الشريط الجانبي ─── */
    int sidebar_y = SET_WIN_Y + SET_TITLE_H;
    int sidebar_h = SET_WIN_H - SET_TITLE_H - SET_BUTTON_H;
    fb_rect(SET_WIN_X, sidebar_y, SET_SIDEBAR_W, sidebar_h, SET_SIDEBAR_BG);

    /* ─── التبويبات ─── */
    static const char *tab_labels[8] = {
        "Display",    /* 0 — العرض */
        "Themes",     /* 1 — السمات */
        "Language",   /* 2 — اللغة */
        "Sound",      /* 3 — الصوت */
        "Network",    /* 4 — الشبكة */
        "Security",   /* 5 — الأمان */
        "System",     /* 6 — النظام */
        "About",      /* 7 — حول */
    };

    /* أيقونات رمزية لكل تبويب (حرف واحد) */
    static const char tab_icons[8] = {
        'D',  /* Display — شاشة */
        'T',  /* Themes — لوحة ألوان */
        'L',  /* Language — لغة */
        'S',  /* Sound — مكبر صوت */
        'N',  /* Network — شبكة */
        'K',  /* Security — قفل */
        'Y',  /* System — ترس */
        '?',  /* About — معلومات */
    };

    for (int i = 0; i < 8; i++) {
        int ty = sidebar_y + 4 + i * SET_TAB_H;
        unsigned int bg_color = (i == g_active_tab) ? SET_SIDEBAR_SEL : SET_SIDEBAR_BG;
        unsigned int text_color = (i == g_active_tab) ? SET_WHITE : SET_SIDEBAR_TEXT;

        /* خلفية التبويب */
        fb_rect(SET_WIN_X + 2, ty, SET_SIDEBAR_W - 4, SET_TAB_H - 2, bg_color);

        /* مؤشر التبويب النشط (شريط ذهبي على اليسار) */
        if (i == g_active_tab) {
            fb_rect(SET_WIN_X, ty, 3, SET_TAB_H - 2, SET_TITLE_TEXT);
        }

        /* رمز التبويب */
        fb_char(SET_WIN_X + 10, ty + 8, tab_icons[i], text_color, 1);

        /* اسم التبويب */
        fb_string(SET_WIN_X + 22, ty + 8, tab_labels[i], text_color, 1);
    }

    /* ─── فاصل عمودي بين الشريط الجانبي والمحتوى ─── */
    fb_vline(SET_WIN_X + SET_SIDEBAR_W, sidebar_y, sidebar_h, SET_SEPARATOR);

    /* ─── خلفية منطقة المحتوى ─── */
    fb_rect(SET_CONTENT_X + 1, SET_CONTENT_Y, SET_CONTENT_W - 1,
            SET_CONTENT_H, SET_CONTENT_BG);

    /* ─── شريط الأزرار السفلي ─── */
    int btn_y = SET_WIN_Y + SET_WIN_H - SET_BUTTON_H;
    fb_rect(SET_WIN_X, btn_y, SET_WIN_W, SET_BUTTON_H, SET_SIDEBAR_BG);
    fb_hline(SET_WIN_X, btn_y, SET_WIN_W, SET_SEPARATOR);

    /* أزرار الإجراء */
    draw_button(SET_WIN_X + 12, btn_y + 8, 80, 28,
                "Apply", SET_BTN_APPLY, SET_WHITE, 0x1A6E3E);
    draw_button(SET_WIN_X + 100, btn_y + 8, 80, 28,
                "Reset", SET_BTN_RESET, SET_WHITE, 0xAA5500);
    draw_button(SET_WIN_X + SET_WIN_W - 92, btn_y + 8, 80, 28,
                "Close", SET_BTN_CLOSE, SET_WHITE, 0x883030);

    /* ─── حدود النافذة الخارجية ─── */
    fb_outline(SET_WIN_X, SET_WIN_Y, SET_WIN_W, SET_WIN_H, SET_BORDER);

    /* ─── رسم خط فاصل بين العنوان والمحتوى ─── */
    fb_hline(SET_WIN_X, SET_WIN_Y + SET_TITLE_H, SET_WIN_W, SET_BORDER);
}

/* ═══════════════════════════════════════════════════════════════════
 *  تبويب 1: العرض (Display Settings)
 * ═══════════════════════════════════════════════════════════════════
 *
 * إعدادات العرض والشاشة:
 *   - الدقة (Resolution): 640×480, 800×600, 1024×768, 1280×1024
 *   - السطوع (Brightness): 0-10
 *   - معدل التحديث (Refresh Rate): 60Hz, 75Hz, 85Hz
 *   - مزامنة عمودية (V-Sync): تشغيل/إيقاف
 *   - ملء الشاشة (Fullscreen): تشغيل/إيقاف
 */
static void draw_tab_display(void) {
    int cx = SET_CONTENT_X + 16;
    int cy = SET_CONTENT_Y + 12;
    int cw = SET_CONTENT_W - 32;

    /* ─── عنوان القسم ─── */
    fb_string(cx, cy, "Display Settings", SET_SECTION_TITLE, 2);
    cy += 28;

    /* ─── الدقة ─── */
    draw_separator(cx, cy, cw, "Resolution");
    cy += 18;

    static const char *resolutions[] = {
        "640x480", "800x600", "1024x768", "1280x1024"
    };
    for (int i = 0; i < 4; i++) {
        draw_radio(cx + 10 + i * 110, cy, resolutions[i], g_resolution == i);
    }
    cy += 24;

    /* ─── صندوق معلومات الدقة الحالية ─── */
    draw_info_box(cx, cy, cw, 22,
                  "Current: 1024x768 @32bpp (BGA mode)",
                  SET_INFO_BG, SET_INFO_BORDER, 'i');
    cy += 34;

    /* ─── السطوع ─── */
    draw_separator(cx, cy, cw, "Brightness");
    cy += 18;
    draw_slider(cx + 10, cy, cw - 20, g_brightness, 10, "Level:");
    cy += 28;

    /* ─── معدل التحديث ─── */
    draw_separator(cx, cy, cw, "Refresh Rate");
    cy += 18;
    static const char *rates[] = { "60 Hz", "75 Hz", "85 Hz" };
    draw_dropdown(cx + 10, cy, cw - 20, "Rate:", rates, 3, g_refresh_rate);
    cy += 32;

    /* ─── خيارات إضافية ─── */
    draw_separator(cx, cy, cw, "Options");
    cy += 18;
    draw_checkbox(cx + 10, cy, "Vertical Sync (V-Sync)", g_vsync);
    cy += 22;
    draw_checkbox(cx + 10, cy, "Fullscreen Mode", g_fullscreen);
    cy += 22;

    /* ─── صندوق تحذير ─── */
    draw_info_box(cx, cy + 8, cw, 22,
                  "Changes require restart to apply",
                  SET_WARN_BG, SET_WARN_BORDER, '!');
}

/* ═══════════════════════════════════════════════════════════════════
 *  تبويب 2: السمات (Theme Settings)
 * ═══════════════════════════════════════════════════════════════════
 *
 * إعدادات المظهر والسمات:
 *   - 5 سمات مدمجة مع معاينة لونية
 *   - الوضع الليلي (Dark Mode)
 *   - خلفية مخصصة
 *   - تأثيرات بصرية (شفافية، حركة)
 */
static void draw_tab_themes(void) {
    int cx = SET_CONTENT_X + 16;
    int cy = SET_CONTENT_Y + 12;
    int cw = SET_CONTENT_W - 32;

    fb_string(cx, cy, "Theme Settings", SET_SECTION_TITLE, 2);
    cy += 28;

    /* ─── اختيار السمة ─── */
    draw_separator(cx, cy, cw, "Select Theme");
    cy += 18;

    /* 5 سمات مع معاينة لونية */
    static const char *theme_names[] = {
        "Golden Bee",       /* النحلة الذهبية */
        "Islamic Night",    /* الليل الإسلامي */
        "Arab Desert",      /* الصحراء العربية */
        "Andalusia",        /* الأندلس */
        "Dawn (Fajr)",      /* الفجر */
    };

    /* ألوان مميزة لكل سمة (لون رئيسي) */
    static const unsigned int theme_colors[] = {
        0xDAA520,   /* ذهبي */
        0x2E8B57,   /* أخضر إسلامي */
        0xC4A35A,   /* رملي */
        0x40B4B4,   /* فيروزي */
        0xD4749D,   /* وردي */
    };

    for (int i = 0; i < 5; i++) {
        int ty = cy + i * 30;

        /* زر راديو */
        draw_radio(cx + 10, ty, "", g_theme == i);

        /* مربع لون المعاينة */
        fb_rect(cx + 32, ty, 20, 14, theme_colors[i]);
        fb_outline(cx + 32, ty, 20, 14, SET_CHECK_BORDER);

        /* اسم السمة */
        fb_string(cx + 58, ty + 2, theme_names[i], SET_CONTENT_TEXT, 1);
    }
    cy += 155;

    /* ─── خيارات المظهر ─── */
    draw_separator(cx, cy, cw, "Appearance");
    cy += 18;
    draw_checkbox(cx + 10, cy, "Dark Mode", g_dark_mode);
    cy += 22;
    draw_checkbox(cx + 10, cy, "Window Transparency", g_transparency);
    cy += 22;
    draw_checkbox(cx + 10, cy, "Enable Animations", g_animations);
    cy += 28;

    /* ─── خلفية سطح المكتب ─── */
    draw_separator(cx, cy, cw, "Wallpaper");
    cy += 18;
    static const char *wp_options[] = { "Gradient", "Solid Color", "Image" };
    for (int i = 0; i < 3; i++) {
        draw_radio(cx + 10 + i * 120, cy, wp_options[i], g_custom_wallpaper == i);
    }
}

/* ═══════════════════════════════════════════════════════════════════
 *  تبويب 3: اللغة (Language Settings)
 * ═══════════════════════════════════════════════════════════════════
 *
 * إعدادات اللغة والتنسيق:
 *   - لغة النظام: عربي / إنجليزي
 *   - تنسيق الساعة: 24 ساعة / 12 ساعة
 *   - التاريخ: هجري / ميلادي / مزدوج
 *   - تخطيط لوحة المفاتيح
 */
static void draw_tab_language(void) {
    int cx = SET_CONTENT_X + 16;
    int cy = SET_CONTENT_Y + 12;
    int cw = SET_CONTENT_W - 32;

    fb_string(cx, cy, "Language & Region", SET_SECTION_TITLE, 2);
    cy += 28;

    /* ─── لغة النظام ─── */
    draw_separator(cx, cy, cw, "System Language");
    cy += 18;
    draw_radio(cx + 10, cy, "Arabic", g_language == 0);
    draw_radio(cx + 150, cy, "English", g_language == 1);
    cy += 28;

    /* ─── تنسيق الوقت ─── */
    draw_separator(cx, cy, cw, "Time Format");
    cy += 18;
    draw_radio(cx + 10, cy, "24-Hour (14:30)", g_clock_format == 0);
    draw_radio(cx + 200, cy, "12-Hour (2:30 PM)", g_clock_format == 1);
    cy += 28;

    /* ─── التاريخ ─── */
    draw_separator(cx, cy, cw, "Date Format");
    cy += 18;
    draw_radio(cx + 10, cy, "Hijri", g_date_format == 0);
    draw_radio(cx + 120, cy, "Gregorian", g_date_format == 1);
    draw_radio(cx + 260, cy, "Both", g_date_format == 2);
    cy += 28;

    /* ─── لوحة المفاتيح ─── */
    draw_separator(cx, cy, cw, "Keyboard Layout");
    cy += 18;
    static const char *kbd_layouts[] = { "Arabic", "English", "Bilingual" };
    draw_dropdown(cx + 10, cy, cw - 20, "Layout:", kbd_layouts, 3, g_keyboard);
    cy += 34;

    /* ─── معلومات ─── */
    draw_info_box(cx, cy, cw, 22,
                  "BeeOS supports full Arabic/RTL UI",
                  SET_INFO_BG, SET_INFO_BORDER, 'i');
    cy += 32;
    draw_info_box(cx, cy, cw, 22,
                  "Date: 4 Muharram 1447 (Hijri)",
                  SET_SUCCESS_BG, SET_SUCCESS_BORDER, '*');
}

/* ═══════════════════════════════════════════════════════════════════
 *  تبويب 4: الصوت (Sound Settings)
 * ═══════════════════════════════════════════════════════════════════
 *
 * إعدادات الصوت:
 *   - مستوى الصوت: 0-10
 *   - تشغيل/إيقاف الصوت
 *   - تنبيهات النظام
 *   - صوت بدء التشغيل
 */
static void draw_tab_sound(void) {
    int cx = SET_CONTENT_X + 16;
    int cy = SET_CONTENT_Y + 12;
    int cw = SET_CONTENT_W - 32;

    fb_string(cx, cy, "Sound Settings", SET_SECTION_TITLE, 2);
    cy += 28;

    /* ─── تشغيل/إيقاف ─── */
    draw_separator(cx, cy, cw, "Master");
    cy += 18;
    draw_checkbox(cx + 10, cy, "Enable Sound", g_sound_enabled);
    cy += 28;

    /* ─── مستوى الصوت ─── */
    draw_separator(cx, cy, cw, "Volume");
    cy += 18;
    draw_slider(cx + 10, cy, cw - 20, g_volume, 10, "Master:");
    cy += 28;

    /* ─── رسم مؤشر بصري لمستوى الصوت ─── */
    /* 10 أشرطة عمودية (مثل المعادل الصوتي) */
    int bar_x = cx + 100;
    for (int i = 0; i < 10; i++) {
        int bar_h = 5 + i * 3;
        unsigned int bar_color;
        if (i <= g_volume) {
            if (i <= 3) bar_color = 0x40C040;      /* أخضر */
            else if (i <= 7) bar_color = 0xFFCC00;  /* أصفر */
            else bar_color = 0xE04040;               /* أحمر */
        } else {
            bar_color = SET_SLIDER_BG;  /* رمادي */
        }
        fb_rect(bar_x + i * 14, cy + 35 - bar_h, 10, bar_h, bar_color);
        fb_outline(bar_x + i * 14, cy + 35 - bar_h, 10, bar_h, SET_CHECK_BORDER);
    }
    cy += 46;

    /* ─── خيارات ─── */
    draw_separator(cx, cy, cw, "Alerts");
    cy += 18;
    draw_checkbox(cx + 10, cy, "System Alerts", g_alerts);
    cy += 22;
    draw_checkbox(cx + 10, cy, "Startup Sound (Bismillah)", g_startup_sound);
    cy += 28;

    /* ─── معلومات عن بطاقة الصوت ─── */
    draw_separator(cx, cy, cw, "Audio Device");
    cy += 18;
    draw_info_box(cx, cy, cw, 22,
                  "AC97 Audio Controller (emulated)",
                  SET_INFO_BG, SET_INFO_BORDER, 'i');
}

/* ═══════════════════════════════════════════════════════════════════
 *  تبويب 5: الشبكة (Network Settings)
 * ═══════════════════════════════════════════════════════════════════
 *
 * إعدادات الشبكة:
 *   - تشغيل/إيقاف
 *   - DHCP أو عنوان ثابت
 *   - عنوان IP، القناع، البوابة
 *   - DNS
 *   - اسم المضيف
 */
static void draw_tab_network(void) {
    int cx = SET_CONTENT_X + 16;
    int cy = SET_CONTENT_Y + 12;
    int cw = SET_CONTENT_W - 32;

    fb_string(cx, cy, "Network Settings", SET_SECTION_TITLE, 2);
    cy += 28;

    /* ─── تشغيل/إيقاف ─── */
    draw_separator(cx, cy, cw, "Connection");
    cy += 18;
    draw_checkbox(cx + 10, cy, "Enable Network", g_network_enabled);
    cy += 28;

    /* ─── طريقة الحصول على العنوان ─── */
    draw_separator(cx, cy, cw, "IP Configuration");
    cy += 18;
    draw_radio(cx + 10, cy, "DHCP (Auto)", g_dhcp == 1);
    draw_radio(cx + 180, cy, "Static IP", g_dhcp == 0);
    cy += 25;

    /* ─── حقول العنوان ─── */
    draw_text_field(cx + 10, cy, cw - 20, "IP Addr:", "10.0.2.15", 0);
    cy += 28;
    draw_text_field(cx + 10, cy, cw - 20, "Netmask:", "255.255.255.0", 0);
    cy += 28;
    draw_text_field(cx + 10, cy, cw - 20, "Gateway:", "10.0.2.2", 0);
    cy += 28;
    draw_text_field(cx + 10, cy, cw - 20, "DNS:", "8.8.8.8", 0);
    cy += 28;
    draw_text_field(cx + 10, cy, cw - 20, "Hostname:", "bee-os", 1);
    cy += 34;

    /* ─── جدار الحماية ─── */
    draw_separator(cx, cy, cw, "Firewall");
    cy += 18;
    draw_checkbox(cx + 10, cy, "Enable Firewall", g_firewall);
    cy += 24;

    /* ─── معلومات ─── */
    draw_info_box(cx, cy, cw, 22,
                  "NIC: Intel e1000 (QEMU emulated)",
                  SET_INFO_BG, SET_INFO_BORDER, 'i');
}

/* ═══════════════════════════════════════════════════════════════════
 *  تبويب 6: الأمان (Security Settings)
 * ═══════════════════════════════════════════════════════════════════
 *
 * إعدادات الأمان:
 *   - كلمة المرور
 *   - القفل التلقائي
 *   - مهلة القفل
 *   - التشفير
 */
static void draw_tab_security(void) {
    int cx = SET_CONTENT_X + 16;
    int cy = SET_CONTENT_Y + 12;
    int cw = SET_CONTENT_W - 32;

    fb_string(cx, cy, "Security Settings", SET_SECTION_TITLE, 2);
    cy += 28;

    /* ─── كلمة المرور ─── */
    draw_separator(cx, cy, cw, "Password");
    cy += 18;
    draw_checkbox(cx + 10, cy, "Require Password on Login", g_password_required);
    cy += 28;

    /* حقل كلمة المرور (مخفي بنجوم) */
    draw_text_field(cx + 10, cy, cw - 20, "Password:", "********", 0);
    cy += 28;

    draw_button(cx + 100, cy, 130, 24,
                "Change Password", SET_BTN_BG, SET_WHITE, SET_BTN_BORDER);
    cy += 36;

    /* ─── القفل التلقائي ─── */
    draw_separator(cx, cy, cw, "Auto Lock");
    cy += 18;
    draw_checkbox(cx + 10, cy, "Enable Auto Lock", g_auto_lock);
    cy += 28;

    static const char *timeouts[] = { "1 min", "5 min", "10 min", "30 min" };
    draw_dropdown(cx + 10, cy, cw - 20, "Timeout:", timeouts, 4, g_lock_timeout);
    cy += 34;

    /* ─── التشفير ─── */
    draw_separator(cx, cy, cw, "Encryption");
    cy += 18;
    draw_checkbox(cx + 10, cy, "Encrypt User Data (AES-256)", g_encryption);
    cy += 28;

    /* ─── تحذير أمني ─── */
    draw_info_box(cx, cy, cw, 22,
                  "Keep your password safe!",
                  SET_WARN_BG, SET_WARN_BORDER, '!');
    cy += 28;

    /* درجة الأمان */
    fb_string(cx + 10, cy, "Security Level:", SET_CONTENT_LABEL, 1);
    int level = g_password_required + g_auto_lock + g_encryption + g_firewall;
    unsigned int level_color;
    const char *level_text;
    if (level >= 4) { level_color = 0x28A745; level_text = "HIGH"; }
    else if (level >= 2) { level_color = 0xE0A800; level_text = "MEDIUM"; }
    else { level_color = 0xE04040; level_text = "LOW"; }
    fb_string(cx + 110, cy, level_text, level_color, 2);

    /* شريط مرئي للأمان */
    cy += 22;
    int bar_w_total = cw - 20;
    int bar_w_fill = bar_w_total * level / 4;
    fb_rect(cx + 10, cy, bar_w_total, 8, SET_SLIDER_BG);
    fb_rect(cx + 10, cy, bar_w_fill, 8, level_color);
    fb_outline(cx + 10, cy, bar_w_total, 8, SET_CHECK_BORDER);
}

/* ═══════════════════════════════════════════════════════════════════
 *  تبويب 7: النظام (System Settings)
 * ═══════════════════════════════════════════════════════════════════
 *
 * إعدادات النظام:
 *   - اسم الجهاز
 *   - المنطقة الزمنية
 *   - الحفظ التلقائي
 *   - معلومات الذاكرة والمعالج
 */
static void draw_tab_system(void) {
    int cx = SET_CONTENT_X + 16;
    int cy = SET_CONTENT_Y + 12;
    int cw = SET_CONTENT_W - 32;

    fb_string(cx, cy, "System Settings", SET_SECTION_TITLE, 2);
    cy += 28;

    /* ─── معلومات الجهاز ─── */
    draw_separator(cx, cy, cw, "Device Info");
    cy += 18;
    draw_text_field(cx + 10, cy, cw - 20, "Hostname:", "bee-os", 0);
    cy += 28;
    draw_text_field(cx + 10, cy, cw - 20, "Username:", "bee_user", 0);
    cy += 34;

    /* ─── المنطقة الزمنية ─── */
    draw_separator(cx, cy, cw, "Time Zone");
    cy += 18;
    static const char *timezones[] = {
        "UTC+0", "UTC+2 (Egypt)", "UTC+3 (Riyadh)",
        "UTC+4 (Dubai)", "UTC+5 (Pak)"
    };
    draw_dropdown(cx + 10, cy, cw - 20, "Zone:", timezones, 5, 2);
    cy += 34;

    /* ─── خيارات النظام ─── */
    draw_separator(cx, cy, cw, "Options");
    cy += 18;
    draw_checkbox(cx + 10, cy, "Auto Save Settings", 1);
    cy += 22;
    draw_checkbox(cx + 10, cy, "Show Boot Messages", 1);
    cy += 22;
    draw_checkbox(cx + 10, cy, "Enable Debug Log", 0);
    cy += 32;

    /* ─── موارد النظام ─── */
    draw_separator(cx, cy, cw, "Resources");
    cy += 18;

    /* CPU */
    fb_string(cx + 10, cy, "CPU:", SET_CONTENT_LABEL, 1);
    fb_string(cx + 60, cy, "i686 (QEMU emulated)", SET_CONTENT_TEXT, 1);
    cy += 14;

    /* RAM */
    fb_string(cx + 10, cy, "RAM:", SET_CONTENT_LABEL, 1);
    fb_string(cx + 60, cy, "64 MB (16 MB kernel)", SET_CONTENT_TEXT, 1);
    cy += 14;

    /* VGA */
    fb_string(cx + 10, cy, "VGA:", SET_CONTENT_LABEL, 1);
    fb_string(cx + 60, cy, "Bochs BGA 1024x768 @32bpp", SET_CONTENT_TEXT, 1);
    cy += 14;

    /* Disk */
    fb_string(cx + 10, cy, "Disk:", SET_CONTENT_LABEL, 1);
    fb_string(cx + 60, cy, "BeeFS (in-memory)", SET_CONTENT_TEXT, 1);
}

/* ═══════════════════════════════════════════════════════════════════
 *  تبويب 8: حول (About)
 * ═══════════════════════════════════════════════════════════════════
 *
 * معلومات عن النظام:
 *   - الاسم والإصدار
 *   - الوصف
 *   - سلسلة الأدوات (Toolchain)
 *   - الترخيص
 *   - المساهمون
 */
static void draw_tab_about(void) {
    int cx = SET_CONTENT_X + 16;
    int cy = SET_CONTENT_Y + 12;
    int cw = SET_CONTENT_W - 32;

    /* ─── شعار BeeOS مكبّر ─── */
    /* شكل سداسي مبسط (خلية نحل) */
    int logo_x = cx + cw / 2 - 30;
    fb_rect(logo_x + 15, cy, 30, 6, 0xFFD700);
    fb_rect(logo_x + 10, cy + 6, 40, 6, 0xFFD700);
    fb_rect(logo_x + 5, cy + 12, 50, 6, 0xFFD700);
    fb_rect(logo_x + 5, cy + 18, 50, 6, 0xDAA520);
    fb_rect(logo_x + 10, cy + 24, 40, 6, 0xDAA520);
    fb_rect(logo_x + 15, cy + 30, 30, 6, 0xDAA520);
    cy += 42;

    /* اسم النظام */
    fb_string(cx + cw / 2 - 60, cy, "BeeOS HD v0.2.0", SET_SECTION_TITLE, 2);
    cy += 22;

    /* الوصف */
    fb_string(cx + 10, cy, "Islamic/Arabic Operating System", SET_CONTENT_TEXT, 1);
    cy += 12;
    fb_string(cx + 10, cy, "Written in Sad Language (S lang)", SET_CONTENT_TEXT, 1);
    cy += 12;
    fb_string(cx + 10, cy, "Compiled via LLVM 18 to native x86", SET_CONTENT_TEXT, 1);
    cy += 22;

    /* ─── سلسلة الأدوات ─── */
    draw_separator(cx, cy, cw, "Toolchain");
    cy += 18;
    fb_string(cx + 10, cy, "sadc (Sad Compiler) -> LLVM IR", SET_CONTENT_SEC, 1);
    cy += 12;
    fb_string(cx + 10, cy, "-> llc (x86 codegen) -> ld.lld", SET_CONTENT_SEC, 1);
    cy += 12;
    fb_string(cx + 10, cy, "-> ELF32 i686 (Multiboot1 boot)", SET_CONTENT_SEC, 1);
    cy += 22;

    /* ─── الترخيص ─── */
    draw_separator(cx, cy, cw, "License");
    cy += 18;
    draw_info_box(cx, cy, cw, 36,
                  "Islamic Waqf License (BeeWaqf 1447)\n"
                  "Free for all Muslims & humanity",
                  SET_SUCCESS_BG, SET_SUCCESS_BORDER, '*');
    cy += 46;

    /* ─── الحقوق ─── */
    draw_separator(cx, cy, cw, "Credits");
    cy += 18;
    fb_string(cx + 10, cy, "Bismillah - In the Name of Allah",
              SET_CONTENT_TEXT, 1);
    cy += 14;
    fb_string(cx + 10, cy, "All praise due to Allah (SWT)", SET_CONTENT_SEC, 1);
    cy += 14;
    fb_string(cx + 10, cy, "Built for the Muslim Ummah", SET_CONTENT_SEC, 1);
}

/* ═══════════════════════════════════════════════════════════════════
 *  الدالة الرئيسية: رسم واجهة الإعدادات الكاملة
 * ═══════════════════════════════════════════════════════════════════
 *
 * ترسم كامل واجهة الإعدادات فوق سطح المكتب:
 *   1. إطار النافذة (شريط العنوان + جانبي + أزرار)
 *   2. محتوى التبويب النشط
 *   3. شريط الحالة
 *
 * يتم استدعاء هذه الدالة من fb_draw_desktop() كطبقة إضافية.
 */
void settings_draw(int tab) {
    /* تعيين التبويب النشط */
    g_active_tab = tab;

    serial_puts("[SETTINGS] Drawing settings UI (tab=");
    serial_putc('0' + tab);
    serial_puts(")\r\n");

    /* ─── رسم إطار النافذة ─── */
    draw_settings_frame();

    /* ─── رسم محتوى التبويب النشط ─── */
    switch (tab) {
        case 0: draw_tab_display();  break;
        case 1: draw_tab_themes();   break;
        case 2: draw_tab_language(); break;
        case 3: draw_tab_sound();    break;
        case 4: draw_tab_network();  break;
        case 5: draw_tab_security(); break;
        case 6: draw_tab_system();   break;
        case 7: draw_tab_about();    break;
        default: draw_tab_display(); break;
    }

    serial_puts("[SETTINGS] Settings UI rendered OK\r\n");
}

/*
 * رسم واجهة الإعدادات في الوضع الافتراضي (تبويب العرض)
 * هذه هي نقطة الدخول الرئيسية التي تستدعى من fb_draw_desktop()
 */
void settings_draw_default(void) {
    settings_draw(0);
}

/*
 * ═══════════════════════════════════════════════════════════════════
 *  ملاحظات التصميم:
 * ═══════════════════════════════════════════════════════════════════
 *
 * 1. الخط الحالي (5×7 bitmap) يدعم ASCII فقط (الإنجليزية).
 *    لدعم العربية في المستقبل، يلزم خط عربي نقطي (مثلاً 8×16).
 *
 * 2. عناصر التحكم (checkbox, radio, slider, dropdown) مرسومة بالكامل
 *    بالبكسل — لا تعتمد على أي مكتبة واجهة.
 *
 * 3. حالياً الواجهة ثابتة (static rendering). لجعلها تفاعلية:
 *    - يلزم مدخلات ماوس (IRQ12 + PS/2 بروتوكول)
 *    - يلزم مدخلات لوحة مفاتيح (IRQ1 + scancode translation)
 *    - يلزم حلقة أحداث (event loop) في النواة
 *
 * 4. الأبعاد مصممة لدقة 1024×768 (BGA mode):
 *    - النافذة: 700×520 بكسل في وسط الشاشة تقريباً
 *    - الشريط الجانبي: 140 بكسل
 *    - منطقة المحتوى: 560×444 بكسل
 *
 * 5. الألوان مستوحاة من سمة "النحلة الذهبية" (الافتراضية).
 *    يمكن تعديلها لتتوافق مع السمة النشطة من سمات.ص.
 *
 * 6. كل عنصر يدعم الحالات:
 *    - عادي (normal): اللون الافتراضي
 *    - تحويم (hover): يمكن إضافته مع event loop
 *    - ضغط (pressed): يمكن إضافته مع mouse input
 *    - معطّل (disabled): رمادي باهت
 *
 * بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ
 * الحمد لله رب العالمين
 * ═══════════════════════════════════════════════════════════════════
 */
