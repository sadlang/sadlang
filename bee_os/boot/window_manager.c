/*
 * ═══════════════════════════════════════════════════════════════════════════
 *  BeeOS HD — نظام نوافذ حقيقي (Window Manager)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ
 *
 * هذا الملف يحتوي على نظام إدارة نوافذ حقيقي لنظام BeeOS.
 * يدير النوافذ المفتوحة ويرسمها بترتيب صحيح (z-ordering).
 *
 * ─── المميزات ───
 *
 *   1. هيكل بيانات النافذة (Window struct)
 *   2. مصفوفة النوافذ المفتوحة (حتى 16 نافذة)
 *   3. ترتيب العمق (Z-order) — الأحدث فوق الأقدم
 *   4. رسم النوافذ بخوارزمية الرسام (Painter's Algorithm)
 *   5. أنماط نوافذ متعددة (عادية، حوار، أداة)
 *   6. شريط عنوان مع أزرار (إغلاق، تصغير، تكبير)
 *   7. ظلال وحدود مخصصة
 *   8. محتوى داخلي لكل نافذة
 *   9. مؤشرات بصرية للنافذة النشطة
 *  10. شريط المهام يعكس النوافذ المفتوحة
 *
 * ─── هيكل النافذة ───
 *
 *   ┌──────────────────────────────────────┐
 *   │ [✕] عنوان النافذة         [_] [□]   │ ← شريط العنوان (title_h)
 *   ├──────────────────────────────────────┤
 *   │                                      │
 *   │           منطقة المحتوى              │ ← body (h - title_h)
 *   │                                      │
 *   │                                      │
 *   └──────────────────────────────────────┘
 *       ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓  ← ظل (shadow_size)
 *
 * ─── ترتيب العمق (Z-Ordering) ───
 *
 *   z_order = 0  → أدنى طبقة (الأبعد خلفاً)
 *   z_order = N  → أعلى طبقة (الأقرب أماماً)
 *
 *   عند النقر على نافذة:
 *     1. ترفع z_order إلى أعلى قيمة
 *     2. تُعاد ترتيب بقية النوافذ
 *     3. تُعلّم كنافذة نشطة (focused)
 *
 * ─── أنماط النوافذ ───
 *
 *   STYLE_NORMAL  — نافذة عادية (كل الأزرار)
 *   STYLE_DIALOG  — حوار (بدون تكبير/تصغير)
 *   STYLE_TOOL    — أداة (بدون أزرار تكبير، عنوان أقصر)
 *   STYLE_SPLASH  — شاشة بدء (بدون عنوان)
 *
 * ─── الحالات الممكنة للنافذة ───
 *
 *   STATE_NORMAL    — الحجم العادي
 *   STATE_MAXIMIZED — ملء الشاشة (بدون شريط المهام)
 *   STATE_MINIMIZED — مخفية (تظهر فقط في شريط المهام)
 *   STATE_HIDDEN    — مخفية تماماً
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
 * ═══════════════════════════════════════════════════════════════════ */
extern void fb_pixel(int x, int y, unsigned int color);
extern void fb_rect(int x, int y, int w, int h, unsigned int color);
extern void fb_outline(int x, int y, int w, int h, unsigned int color);
extern void fb_hline(int x, int y, int w, unsigned int color);
extern void fb_vline(int x, int y, int h, unsigned int color);
extern void fb_char(int cx, int cy, char ch, unsigned int color, int scale);
extern void fb_string(int x, int y, const char *s, unsigned int color, int scale);

/* ═══════════════════════════════════════════════════════════════════
 *  ثوابت نظام النوافذ
 * ═══════════════════════════════════════════════════════════════════
 *
 * الحدود القصوى والأبعاد الافتراضية لنظام النوافذ.
 * مصمم لدقة 1024×768 (BGA mode):
 *   - أقصى عدد نوافذ: 16 (كافي لنظام مكتبي)
 *   - ارتفاع العنوان: 28 بكسل (يتغير حسب النمط)
 *   - حجم الظل: 4 بكسل
 *   - الحد الأدنى للنافذة: 160×100 بكسل
 */

#define WM_MAX_WINDOWS    16     /* أقصى عدد نوافذ متزامنة */
#define WM_TITLE_H        28    /* ارتفاع شريط العنوان (عادي) */
#define WM_TITLE_H_TOOL   20    /* ارتفاع شريط العنوان (أداة) */
#define WM_SHADOW_SIZE    4     /* حجم الظل */
#define WM_BORDER_WIDTH   1     /* عرض الحدود */
#define WM_MIN_W          160   /* الحد الأدنى للعرض */
#define WM_MIN_H          100   /* الحد الأدنى للارتفاع */
#define WM_BTN_SIZE       16    /* حجم أزرار العنوان */
#define WM_BTN_MARGIN     6     /* هامش الأزرار */
#define WM_TASKBAR_H      48    /* ارتفاع شريط المهام */
#define WM_SCREEN_W       1024  /* عرض الشاشة */
#define WM_SCREEN_H       768   /* ارتفاع الشاشة */

/* ═══════════════════════════════════════════════════════════════════
 *  ألوان نظام النوافذ — سمة النحلة الذهبية
 * ═══════════════════════════════════════════════════════════════════
 *
 * لوحة ألوان متناسقة مع سطح المكتب:
 *   - العنوان النشط: ذهبي داكن
 *   - العنوان غير النشط: رمادي
 *   - الجسم: أبيض
 *   - الظل: أسود شبه شفاف
 */

/* ─── شريط العنوان ─── */
#define WM_TITLE_ACTIVE    0x8B6914   /* عنوان نشط — ذهبي داكن */
#define WM_TITLE_INACTIVE  0x808080   /* عنوان غير نشط — رمادي */
#define WM_TITLE_TEXT_ACT  0xFFD700   /* نص عنوان نشط — ذهبي */
#define WM_TITLE_TEXT_INA  0xC0C0C0   /* نص عنوان غير نشط — فضي */

/* ─── أزرار العنوان ─── */
#define WM_BTN_CLOSE       0xE04040   /* إغلاق — أحمر */
#define WM_BTN_MAXIMIZE    0x40C040   /* تكبير — أخضر */
#define WM_BTN_MINIMIZE    0xFFCC00   /* تصغير — أصفر */
#define WM_BTN_CLOSE_HOV   0xFF5050   /* إغلاق عند المرور */
#define WM_BTN_MAX_HOV     0x50D050   /* تكبير عند المرور */
#define WM_BTN_MIN_HOV     0xFFDD33   /* تصغير عند المرور */

/* ─── جسم النافذة ─── */
#define WM_BODY_BG         0xFFFFFF   /* خلفية الجسم — أبيض */
#define WM_BODY_TEXT       0x1A1A1A   /* نص الجسم — أسود */
#define WM_BORDER_ACTIVE   0x6B4F10   /* حدود نشطة — بني ذهبي */
#define WM_BORDER_INACTIVE 0xAAAAAA   /* حدود غير نشطة — رمادي */

/* ─── ظل وخلفيات ─── */
#define WM_SHADOW_COLOR    0x1A1A1A   /* لون الظل */
#define WM_SHADOW_LIGHT    0x3A3A3A   /* ظل فاتح */
#define WM_SHADOW_LIGHTER  0x6A6A6A   /* ظل أفتح */
#define WM_SHADOW_LIGHTEST 0x9A9A9A   /* أفتح ظل */
#define WM_DESKTOP_OVERLAY 0x000000   /* تعتيم خلفية (للحوارات) */

/* ─── شريط المهام ─── */
#define WM_TASKBAR_BG      0x2A1A08   /* خلفية شريط المهام */
#define WM_TASKBAR_ACCENT  0xFFD700   /* لمسة ذهبية */
#define WM_TASKBAR_BTN     0x3A2A1A   /* زر في الشريط */
#define WM_TASKBAR_BTN_ACT 0x5A4A2A   /* زر نشط في الشريط */
#define WM_TASKBAR_TEXT    0xFFFFFF   /* نص شريط المهام */

/* ─── أنماط متنوعة ─── */
#define WM_SCROLLBAR_BG    0xE0E0E0   /* خلفية شريط التمرير */
#define WM_SCROLLBAR_FILL  0xDAA520   /* ملء شريط التمرير */
#define WM_MENU_BG         0xF5F5F0   /* خلفية القائمة */
#define WM_MENU_HOVER      0xDAA520   /* عنصر القائمة عند المرور */
#define WM_MENU_TEXT       0x1A1A1A   /* نص القائمة */
#define WM_MENU_BORDER     0xC0C0C0   /* حدود القائمة */

/* ═══════════════════════════════════════════════════════════════════
 *  تعدادات أنماط وحالات النوافذ
 * ═══════════════════════════════════════════════════════════════════ */

/* أنماط النوافذ — تحدد المظهر والسلوك */
enum WindowStyle {
    STYLE_NORMAL  = 0,    /* نافذة عادية — كل الأزرار وشريط عنوان كامل */
    STYLE_DIALOG  = 1,    /* نافذة حوار — بدون تكبير/تصغير */
    STYLE_TOOL    = 2,    /* نافذة أداة — عنوان أصغر، بدون تكبير */
    STYLE_SPLASH  = 3,    /* شاشة بدء — بدون شريط عنوان */
};

/* حالات النافذة — تحدد الرؤية والحجم */
enum WindowState {
    STATE_NORMAL    = 0,  /* حجم عادي */
    STATE_MAXIMIZED = 1,  /* ملء الشاشة */
    STATE_MINIMIZED = 2,  /* مُصغّرة (في شريط المهام فقط) */
    STATE_HIDDEN    = 3,  /* مخفية تماماً */
};

/* ═══════════════════════════════════════════════════════════════════
 *  هيكل بيانات النافذة (Window Structure)
 * ═══════════════════════════════════════════════════════════════════
 *
 * كل نافذة تحتوي على:
 *   - معرّف فريد (id)
 *   - عنوان (title) — حتى 63 حرف
 *   - موضع وأبعاد (x, y, w, h)
 *   - الموضع والأبعاد قبل التكبير (لاستعادتها)
 *   - نمط (style) — عادي/حوار/أداة/بدء
 *   - حالة (state) — عادي/مكبّر/مصغّر/مخفي
 *   - ترتيب العمق (z_order)
 *   - علامة التفعيل (active)
 *   - لون العنوان المخصص (title_color)
 *   - محتوى — نوع ومؤشر دالة الرسم
 */
typedef struct {
    int id;                     /* معرّف فريد (0 = فارغ) */
    char title[64];             /* عنوان النافذة */
    int x, y, w, h;            /* الموضع والأبعاد الحالية */
    int saved_x, saved_y;      /* موضع محفوظ (قبل التكبير) */
    int saved_w, saved_h;      /* أبعاد محفوظة (قبل التكبير) */
    int style;                  /* نمط النافذة (WindowStyle) */
    int state;                  /* حالة النافذة (WindowState) */
    int z_order;                /* ترتيب العمق (0 = أدنى) */
    int active;                 /* 1 = النافذة النشطة (المحددة) */
    unsigned int title_color;   /* لون شريط العنوان */
    int content_type;           /* نوع المحتوى (للتعرف عليه) */
    int scrollbar_pos;          /* موضع شريط التمرير (0-100) */
    int has_scrollbar;          /* 1 = يوجد شريط تمرير */
    int resizable;              /* 1 = قابلة للتحجيم */
    int closable;               /* 1 = قابلة للإغلاق */
} Window;

/* ═══════════════════════════════════════════════════════════════════
 *  حالة مدير النوافذ العامة
 * ═══════════════════════════════════════════════════════════════════
 *
 * g_windows: مصفوفة النوافذ المفتوحة (16 خانة)
 * g_win_count: عدد النوافذ النشطة حالياً
 * g_next_id: المعرّف التالي للنافذة الجديدة
 * g_focused_id: معرّف النافذة المحددة (النشطة)
 * g_cursor_x/y: موضع مؤشر الماوس
 */
static Window g_windows[WM_MAX_WINDOWS];
static int g_win_count = 0;
static int g_next_id = 1;
static int g_focused_id = 0;
static int g_cursor_x = WM_SCREEN_W / 2;
static int g_cursor_y = WM_SCREEN_H / 2;

/* ═══════════════════════════════════════════════════════════════════
 *  دوال إدارة النوافذ
 * ═══════════════════════════════════════════════════════════════════ */

/*
 * wm_find_slot — إيجاد خانة فارغة في مصفوفة النوافذ
 *
 * يبحث عن أول خانة بمعرّف == 0 (فارغة)
 *
 * @return  الفهرس (0 — WM_MAX_WINDOWS-1) أو -1 إذا ممتلئة
 */
static int wm_find_slot(void) {
    for (int i = 0; i < WM_MAX_WINDOWS; i++) {
        if (g_windows[i].id == 0) return i;
    }
    return -1;  /* لا توجد خانات فارغة */
}

/*
 * wm_create_window — إنشاء نافذة جديدة
 *
 * تنشئ نافذة بالمعلومات المحددة وتضيفها إلى مصفوفة النوافذ.
 * تُعيّن كنافذة نشطة (أعلى z_order) تلقائياً.
 *
 * @param title       عنوان النافذة
 * @param x, y        الموضع الأولي
 * @param w, h        الأبعاد
 * @param style       نمط النافذة
 * @param title_color لون شريط العنوان (0 = افتراضي)
 *
 * @return  معرّف النافذة الجديدة، أو 0 عند الفشل
 */
static int wm_create_window(const char *title, int x, int y, int w, int h,
                            int style, unsigned int title_color) {
    int slot = wm_find_slot();
    if (slot < 0) return 0;  /* مصفوفة النوافذ ممتلئة */

    /* فرض الحد الأدنى للأبعاد */
    if (w < WM_MIN_W) w = WM_MIN_W;
    if (h < WM_MIN_H) h = WM_MIN_H;

    /* تعبئة بيانات النافذة */
    Window *win = &g_windows[slot];
    win->id = g_next_id++;
    win->x = x;
    win->y = y;
    win->w = w;
    win->h = h;
    win->saved_x = x;
    win->saved_y = y;
    win->saved_w = w;
    win->saved_h = h;
    win->style = style;
    win->state = STATE_NORMAL;
    win->z_order = g_win_count;  /* أعلى طبقة */
    win->active = 1;
    win->title_color = title_color ? title_color : WM_TITLE_ACTIVE;
    win->content_type = 0;
    win->scrollbar_pos = 0;
    win->has_scrollbar = 0;
    win->resizable = (style != STYLE_DIALOG && style != STYLE_SPLASH);
    win->closable = (style != STYLE_SPLASH);

    /* نسخ العنوان */
    int i = 0;
    while (title[i] && i < 63) { win->title[i] = title[i]; i++; }
    win->title[i] = '\0';

    /* إلغاء تفعيل النوافذ الأخرى */
    for (int j = 0; j < WM_MAX_WINDOWS; j++) {
        if (j != slot && g_windows[j].id != 0) {
            g_windows[j].active = 0;
        }
    }

    g_focused_id = win->id;
    g_win_count++;

    serial_puts("[WM] Created window: ");
    serial_puts(win->title);
    serial_puts("\r\n");

    return win->id;
}

/*
 * wm_close_window — إغلاق نافذة
 *
 * يحذف النافذة من مصفوفة النوافذ ويعيد ترتيب العمق.
 *
 * @param id  معرّف النافذة المراد إغلاقها
 */
static void wm_close_window(int id) {
    for (int i = 0; i < WM_MAX_WINDOWS; i++) {
        if (g_windows[i].id == id) {
            serial_puts("[WM] Closed window: ");
            serial_puts(g_windows[i].title);
            serial_puts("\r\n");

            int closed_z = g_windows[i].z_order;
            g_windows[i].id = 0;
            g_windows[i].title[0] = '\0';
            g_win_count--;

            /* تعديل z_order للنوافذ فوقها */
            for (int j = 0; j < WM_MAX_WINDOWS; j++) {
                if (g_windows[j].id != 0 && g_windows[j].z_order > closed_z) {
                    g_windows[j].z_order--;
                }
            }

            /* تفعيل النافذة الأعلى المتبقية */
            int highest_z = -1;
            int highest_idx = -1;
            for (int j = 0; j < WM_MAX_WINDOWS; j++) {
                if (g_windows[j].id != 0 && g_windows[j].z_order > highest_z) {
                    highest_z = g_windows[j].z_order;
                    highest_idx = j;
                }
            }
            if (highest_idx >= 0) {
                g_windows[highest_idx].active = 1;
                g_focused_id = g_windows[highest_idx].id;
            } else {
                g_focused_id = 0;
            }
            break;
        }
    }
}

/*
 * wm_bring_to_front — رفع نافذة إلى الأمام
 *
 * ترفع النافذة المحددة لتكون فوق جميع النوافذ الأخرى.
 *
 * @param id  معرّف النافذة
 */
static void wm_bring_to_front(int id) {
    int target_z = -1;
    int target_idx = -1;

    /* إيجاد النافذة المطلوبة */
    for (int i = 0; i < WM_MAX_WINDOWS; i++) {
        if (g_windows[i].id == id) {
            target_z = g_windows[i].z_order;
            target_idx = i;
            break;
        }
    }
    if (target_idx < 0) return;

    /* تخفيض z_order للنوافذ التي كانت فوقها */
    for (int i = 0; i < WM_MAX_WINDOWS; i++) {
        if (g_windows[i].id != 0 && g_windows[i].z_order > target_z) {
            g_windows[i].z_order--;
        }
        g_windows[i].active = 0;
    }

    /* رفعها إلى الأعلى */
    g_windows[target_idx].z_order = g_win_count - 1;
    g_windows[target_idx].active = 1;
    g_focused_id = id;
}

/*
 * wm_toggle_maximize — تبديل حالة التكبير
 *
 * إذا كانت عادية → تكبير إلى ملء الشاشة (بدون شريط المهام)
 * إذا كانت مكبّرة → استعادة الحجم الأصلي
 *
 * @param id  معرّف النافذة
 */
static void wm_toggle_maximize(int id) {
    for (int i = 0; i < WM_MAX_WINDOWS; i++) {
        if (g_windows[i].id == id) {
            Window *win = &g_windows[i];
            if (win->state == STATE_MAXIMIZED) {
                /* استعادة */
                win->x = win->saved_x;
                win->y = win->saved_y;
                win->w = win->saved_w;
                win->h = win->saved_h;
                win->state = STATE_NORMAL;
            } else {
                /* تكبير — ملء الشاشة (بدون شريط المهام) */
                win->saved_x = win->x;
                win->saved_y = win->y;
                win->saved_w = win->w;
                win->saved_h = win->h;
                win->x = 0;
                win->y = 0;
                win->w = WM_SCREEN_W;
                win->h = WM_SCREEN_H - WM_TASKBAR_H;
                win->state = STATE_MAXIMIZED;
            }
            break;
        }
    }
}

/*
 * wm_minimize — تصغير نافذة
 *
 * @param id  معرّف النافذة
 */
static void wm_minimize(int id) {
    for (int i = 0; i < WM_MAX_WINDOWS; i++) {
        if (g_windows[i].id == id) {
            g_windows[i].state = STATE_MINIMIZED;
            g_windows[i].active = 0;
            break;
        }
    }
}

/* ═══════════════════════════════════════════════════════════════════
 *  رسم النافذة الواحدة
 * ═══════════════════════════════════════════════════════════════════
 *
 * ترسم نافذة واحدة بكل عناصرها:
 *   1. ظل ناعم (4 مستويات)
 *   2. جسم النافذة (خلفية بيضاء)
 *   3. شريط العنوان
 *   4. أزرار التحكم (إغلاق، تصغير، تكبير)
 *   5. عنوان النافذة (نص)
 *   6. حدود النافذة
 *   7. شريط التمرير (اختياري)
 *   8. المحتوى الداخلي
 */
static void wm_draw_single_window(Window *win) {
    if (!win || win->id == 0) return;
    if (win->state == STATE_MINIMIZED || win->state == STATE_HIDDEN) return;

    int x = win->x, y = win->y, w = win->w, h = win->h;
    int is_active = win->active;
    int title_h = (win->style == STYLE_TOOL) ? WM_TITLE_H_TOOL : WM_TITLE_H;

    /* ─── 1. الظل الناعم ─── */
    /*
     * ظل متدرج من 4 مستويات لإعطاء تأثير العمق:
     *   الطبقة 0 (الأقرب): داكن (0x1A1A1A)
     *   الطبقة 1: رمادي داكن (0x3A3A3A)
     *   الطبقة 2: رمادي (0x6A6A6A)
     *   الطبقة 3 (الأبعد): رمادي فاتح (0x9A9A9A)
     */
    if (win->style != STYLE_SPLASH) {
        unsigned int shadow_levels[4] = {
            WM_SHADOW_COLOR, WM_SHADOW_LIGHT,
            WM_SHADOW_LIGHTER, WM_SHADOW_LIGHTEST
        };
        for (int s = 0; s < WM_SHADOW_SIZE; s++) {
            fb_hline(x + WM_SHADOW_SIZE + s, y + h + s, w - WM_SHADOW_SIZE, shadow_levels[s]);
            fb_vline(x + w + s, y + WM_SHADOW_SIZE + s, h - WM_SHADOW_SIZE, shadow_levels[s]);
        }
    }

    /* ─── 2. جسم النافذة ─── */
    if (win->style == STYLE_SPLASH) {
        /* شاشة بدء — بدون عنوان */
        fb_rect(x, y, w, h, WM_BODY_BG);
    } else {
        fb_rect(x, y + title_h, w, h - title_h, WM_BODY_BG);
    }

    /* ─── 3. شريط العنوان ─── */
    if (win->style != STYLE_SPLASH) {
        unsigned int tb_color = is_active ? win->title_color : WM_TITLE_INACTIVE;
        fb_rect(x, y, w, title_h, tb_color);

        /* ─── 4. أزرار التحكم ─── */
        /* زر الإغلاق (✕) — دائماً موجود ما عدا SPLASH */
        if (win->closable) {
            fb_rect(x + WM_BTN_MARGIN, y + WM_BTN_MARGIN,
                    WM_BTN_SIZE, WM_BTN_SIZE, WM_BTN_CLOSE);
            fb_char(x + WM_BTN_MARGIN + 4, y + WM_BTN_MARGIN + 3,
                    'x', 0xFFFFFF, 1);
        }

        /* زر التصغير (_) — فقط في NORMAL */
        if (win->style == STYLE_NORMAL) {
            int min_x = x + w - WM_BTN_MARGIN - WM_BTN_SIZE * 2 - 4;
            fb_rect(min_x, y + WM_BTN_MARGIN, WM_BTN_SIZE, WM_BTN_SIZE, WM_BTN_MINIMIZE);
            fb_char(min_x + 4, y + WM_BTN_MARGIN + 5, '-', 0x000000, 1);
        }

        /* زر التكبير (□) — فقط في NORMAL */
        if (win->style == STYLE_NORMAL && win->resizable) {
            int max_x = x + w - WM_BTN_MARGIN - WM_BTN_SIZE;
            fb_rect(max_x, y + WM_BTN_MARGIN, WM_BTN_SIZE, WM_BTN_SIZE, WM_BTN_MAXIMIZE);
            fb_char(max_x + 4, y + WM_BTN_MARGIN + 3, '+', 0xFFFFFF, 1);
        }

        /* ─── 5. عنوان النافذة ─── */
        unsigned int title_text = is_active ? WM_TITLE_TEXT_ACT : WM_TITLE_TEXT_INA;
        int text_x = x + WM_BTN_MARGIN + WM_BTN_SIZE + 8;
        int text_y = y + (title_h - 10) / 2;
        fb_string(text_x, text_y, win->title, title_text, 1);

        /* خط فاصل بين العنوان والجسم */
        unsigned int sep_color = is_active ? WM_BORDER_ACTIVE : WM_BORDER_INACTIVE;
        fb_hline(x, y + title_h, w, sep_color);
    }

    /* ─── 6. حدود النافذة ─── */
    unsigned int border_color = is_active ? WM_BORDER_ACTIVE : WM_BORDER_INACTIVE;
    fb_outline(x, y, w, h, border_color);

    /* حدود إضافية للنافذة النشطة (تأثير توهج خفيف) */
    if (is_active && win->style != STYLE_SPLASH) {
        fb_outline(x - 1, y - 1, w + 2, h + 2, WM_BORDER_ACTIVE);
    }

    /* ─── 7. شريط التمرير (إذا موجود) ─── */
    if (win->has_scrollbar) {
        int sb_x = x + w - 14;
        int sb_y = y + title_h + 1;
        int sb_h = h - title_h - 2;

        /* خلفية المسار */
        fb_rect(sb_x, sb_y, 13, sb_h, WM_SCROLLBAR_BG);

        /* المقبض */
        int knob_h = sb_h / 4;
        if (knob_h < 20) knob_h = 20;
        int knob_y = sb_y + (win->scrollbar_pos * (sb_h - knob_h) / 100);
        fb_rect(sb_x + 1, knob_y, 11, knob_h, WM_SCROLLBAR_FILL);
        fb_outline(sb_x + 1, knob_y, 11, knob_h, WM_BORDER_ACTIVE);

        /* فاصل عمودي قبل الشريط */
        fb_vline(sb_x - 1, sb_y, sb_h, WM_BORDER_INACTIVE);
    }
}

/* ═══════════════════════════════════════════════════════════════════
 *  نوافذ المحتوى — رسم المحتوى الداخلي لكل نافذة
 * ═══════════════════════════════════════════════════════════════════
 *
 * كل نافذة لها محتوى مختلف حسب content_type:
 *   0 = مدير الملفات (File Manager)
 *   1 = محرر النصوص (Text Editor)
 *   2 = الحاسبة (Calculator)
 *   3 = المتصفح (Web Browser)
 *   4 = الطرفية (Terminal/Shell)
 *   5 = إعدادات (Settings) — يتم توجيهها لـ settings_draw()
 *   6 = معلومات النظام (System Info)
 */

/*
 * رسم محتوى مدير الملفات
 * يعرض قائمة ملفات ومجلدات مع أيقونات ملونة
 */
static void wm_content_file_manager(Window *win) {
    int title_h = WM_TITLE_H;
    int cx = win->x + 8;
    int cy = win->y + title_h + 8;
    int cw = win->w - 16;

    /* شريط المسار */
    fb_rect(cx, cy, cw, 20, 0xF0F0F0);
    fb_outline(cx, cy, cw, 20, 0xC0C0C0);
    fb_string(cx + 6, cy + 5, "/home/bee/", 0x606060, 1);
    cy += 28;

    /* خط فاصل */
    fb_hline(cx, cy, cw, 0xE0E0E0);
    cy += 6;

    /* قائمة الملفات */
    static const char *files[] = {
        "Documents/", "Pictures/", "Music/", "Downloads/",
        "hello.sad", "config.cfg", "notes.txt", "beeos.log"
    };
    static const unsigned int file_colors[] = {
        0x2A7FFF, 0x2A7FFF, 0x2A7FFF, 0x2A7FFF,
        0x3A9F3A, 0x808080, 0x808080, 0xCC6600
    };

    for (int i = 0; i < 8; i++) {
        /* أيقونة ملونة */
        fb_rect(cx + 4, cy, 10, 10, file_colors[i]);
        /* اسم الملف */
        fb_string(cx + 20, cy, files[i], WM_BODY_TEXT, 1);
        /* حجم وهمي */
        if (i >= 4) {
            fb_string(cx + cw - 60, cy, "4.2 KB", 0x808080, 1);
        }
        cy += 16;
    }

    /* شريط الحالة */
    int status_y = win->y + win->h - 20;
    fb_rect(win->x + 1, status_y, win->w - 2, 19, 0xF0F0F0);
    fb_hline(win->x + 1, status_y, win->w - 2, 0xE0E0E0);
    fb_string(win->x + 8, status_y + 5, "8 items   |   Free: 48 MB", 0x808080, 1);
}

/*
 * رسم محتوى محرر النصوص
 * يعرض محرر نصوص بسيط مع أرقام الأسطر وتلوين الصياغة
 */
static void wm_content_text_editor(Window *win) {
    int title_h = WM_TITLE_H;
    int cx = win->x + 8;
    int cy = win->y + title_h + 8;

    /* شريط الأدوات */
    fb_rect(cx, cy, win->w - 16, 18, 0xF0F0F0);
    fb_string(cx + 4, cy + 4, "File  Edit  View  Help", 0x404040, 1);
    fb_hline(cx, cy + 18, win->w - 16, 0xD0D0D0);
    cy += 24;

    /* أرقام الأسطر + المحتوى */
    static const char *lines[] = {
        "# BeeOS Kernel Script",
        "# Bismillah",
        "",
        "fn main():",
        "  print(\"Salam Alaykum!\")",
        "  x = 42",
        "  if x > 10:",
        "    print(\"BeeOS rocks!\")",
        "  end",
        "end",
    };
    static const unsigned int line_colors[] = {
        0x008000, 0x008000, 0x000000, 0x0000CC,
        0x000000, 0x000000, 0x0000CC, 0x000000,
        0x0000CC, 0x0000CC,
    };

    int gutter_w = 30;
    for (int i = 0; i < 10; i++) {
        /* رقم السطر */
        char num[4];
        num[0] = '0' + ((i + 1) / 10);
        num[1] = '0' + ((i + 1) % 10);
        num[2] = 0;
        if (i < 9) { num[0] = ' '; num[1] = '0' + (i + 1); }
        fb_string(cx + 4, cy, num, 0xA0A0A0, 1);

        /* فاصل عمودي */
        fb_vline(cx + gutter_w, cy - 1, 12, 0xE0E0E0);

        /* محتوى السطر */
        fb_string(cx + gutter_w + 6, cy, lines[i], line_colors[i], 1);

        cy += 14;
    }

    /* مؤشر الكتابة (وميض) */
    fb_vline(cx + gutter_w + 6 + 6 * 3, cy - 14 + 2, 10, 0x000000);
}

/*
 * رسم محتوى الحاسبة
 * واجهة حاسبة بسيطة مع أزرار رقمية
 */
static void wm_content_calculator(Window *win) {
    int title_h = WM_TITLE_H;
    int cx = win->x + 10;
    int cy = win->y + title_h + 10;
    int cw = win->w - 20;

    /* شاشة العرض */
    fb_rect(cx, cy, cw, 36, 0x1A2A1A);
    fb_outline(cx, cy, cw, 36, 0x808080);
    fb_string(cx + cw - 70, cy + 8, "42", 0x40FF40, 3);
    cy += 48;

    /* أزرار الحاسبة (4×4) */
    static const char calc_buttons[4][4] = {
        {'7', '8', '9', '/'},
        {'4', '5', '6', '*'},
        {'1', '2', '3', '-'},
        {'0', '.', '=', '+'},
    };
    static const unsigned int btn_colors[4][4] = {
        {0xE8E0D0, 0xE8E0D0, 0xE8E0D0, 0xDAA520},
        {0xE8E0D0, 0xE8E0D0, 0xE8E0D0, 0xDAA520},
        {0xE8E0D0, 0xE8E0D0, 0xE8E0D0, 0xDAA520},
        {0xE8E0D0, 0xE8E0D0, 0x2E8B57, 0xDAA520},
    };

    int btn_w = (cw - 15) / 4;
    int btn_h = 32;
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            int bx = cx + col * (btn_w + 5);
            int by = cy + row * (btn_h + 5);

            fb_rect(bx + 1, by + 1, btn_w, btn_h, 0xA0A0A0); /* ظل */
            fb_rect(bx, by, btn_w, btn_h, btn_colors[row][col]);
            fb_outline(bx, by, btn_w, btn_h, 0x808080);

            /* الحرف في المنتصف */
            unsigned int text_color = (row == 3 && col == 2) ? 0xFFFFFF : 0x1A1A1A;
            fb_char(bx + btn_w / 2 - 4, by + btn_h / 2 - 5, calc_buttons[row][col],
                    text_color, 1);
        }
    }

    /* زر C (مسح) */
    cy += 4 * (btn_h + 5) + 8;
    fb_rect(cx, cy, cw, 28, 0xCC4444);
    fb_outline(cx, cy, cw, 28, 0x993333);
    fb_string(cx + cw / 2 - 12, cy + 8, "CLEAR", 0xFFFFFF, 1);
}

/*
 * رسم محتوى الطرفية (Terminal/Shell)
 * واجهة سطر أوامر مع موجّه أوامر
 */
static void wm_content_terminal(Window *win) {
    int title_h = WM_TITLE_H;
    int cx = win->x + 1;
    int cy = win->y + title_h + 1;
    int cw = win->w - 2;
    int ch = win->h - title_h - 2;

    /* خلفية سوداء */
    fb_rect(cx, cy, cw, ch, 0x0A0A0A);

    /* محتوى الطرفية */
    int line_y = cy + 6;
    fb_string(cx + 6, line_y, "BeeOS Shell v0.2.0", 0x40FF40, 1);
    line_y += 12;
    fb_string(cx + 6, line_y, "Bismillah ar-Rahman ar-Rahim", 0x808080, 1);
    line_y += 16;

    fb_string(cx + 6, line_y, "bee@beeos:~$ ", 0x40FF40, 1);
    fb_string(cx + 6 + 13 * 6, line_y, "ls -la", 0xFFFFFF, 1);
    line_y += 12;

    fb_string(cx + 6, line_y, "drwxr-xr-x  Documents/", 0x40BBFF, 1);
    line_y += 12;
    fb_string(cx + 6, line_y, "drwxr-xr-x  Pictures/", 0x40BBFF, 1);
    line_y += 12;
    fb_string(cx + 6, line_y, "-rw-r--r--  hello.sad  256B", 0xFFFFFF, 1);
    line_y += 12;
    fb_string(cx + 6, line_y, "-rw-r--r--  config.cfg  128B", 0xFFFFFF, 1);
    line_y += 16;

    fb_string(cx + 6, line_y, "bee@beeos:~$ ", 0x40FF40, 1);
    /* مؤشر الكتابة */
    fb_rect(cx + 6 + 13 * 6, line_y, 7, 10, 0x40FF40);
}

/*
 * رسم محتوى معلومات النظام
 */
static void wm_content_system_info(Window *win) {
    int title_h = WM_TITLE_H;
    int cx = win->x + 12;
    int cy = win->y + title_h + 12;

    /* شعار */
    fb_rect(cx + 60, cy, 30, 6, 0xFFD700);
    fb_rect(cx + 55, cy + 6, 40, 6, 0xFFD700);
    fb_rect(cx + 50, cy + 12, 50, 6, 0xDAA520);
    fb_rect(cx + 55, cy + 18, 40, 6, 0xDAA520);
    fb_rect(cx + 60, cy + 24, 30, 6, 0xDAA520);
    cy += 38;

    fb_string(cx, cy, "BeeOS HD v0.2.0", 0x8B6914, 2);
    cy += 22;
    fb_hline(cx, cy, win->w - 24, 0xE0E0E0);
    cy += 10;

    /* معلومات */
    fb_string(cx, cy, "Kernel:", 0x808080, 1);
    fb_string(cx + 60, cy, "BeeOS HD (Sad lang)", 0x1A1A1A, 1);
    cy += 14;
    fb_string(cx, cy, "Arch:", 0x808080, 1);
    fb_string(cx + 60, cy, "i686 (32-bit x86)", 0x1A1A1A, 1);
    cy += 14;
    fb_string(cx, cy, "RAM:", 0x808080, 1);
    fb_string(cx + 60, cy, "64 MB", 0x1A1A1A, 1);
    cy += 14;
    fb_string(cx, cy, "Video:", 0x808080, 1);
    fb_string(cx + 60, cy, "Bochs BGA 1024x768", 0x1A1A1A, 1);
    cy += 14;
    fb_string(cx, cy, "FS:", 0x808080, 1);
    fb_string(cx + 60, cy, "BeeFS (in-memory)", 0x1A1A1A, 1);
    cy += 14;
    fb_string(cx, cy, "License:", 0x808080, 1);
    fb_string(cx + 60, cy, "BeeWaqf 1447", 0x1A1A1A, 1);
}

/*
 * رسم المحتوى الداخلي لنافذة واحدة
 * يستدعي الدالة المناسبة حسب content_type
 */
static void wm_draw_content(Window *win) {
    if (!win || win->id == 0) return;
    if (win->state == STATE_MINIMIZED || win->state == STATE_HIDDEN) return;
    if (win->style == STYLE_SPLASH) return;

    switch (win->content_type) {
        case 0: wm_content_file_manager(win);  break;
        case 1: wm_content_text_editor(win);   break;
        case 2: wm_content_calculator(win);    break;
        case 4: wm_content_terminal(win);      break;
        case 6: wm_content_system_info(win);   break;
        default: break;
    }
}

/* ═══════════════════════════════════════════════════════════════════
 *  رسم شريط المهام بنوافذ مدير النوافذ
 * ═══════════════════════════════════════════════════════════════════
 *
 * شريط المهام يعكس النوافذ المفتوحة:
 *   [BeeOS] [ملفات] [محرر] [حاسبة] ─── [ساعة]
 *
 * النافذة النشطة تُظهر بلون مختلف
 */
static void wm_draw_taskbar(void) {
    int ty = WM_SCREEN_H - WM_TASKBAR_H;
    int tw = WM_SCREEN_W;

    /* ─── خلفية شريط المهام ─── */
    fb_rect(0, ty, tw, WM_TASKBAR_H, WM_TASKBAR_BG);
    /* خط ذهبي علوي */
    fb_hline(0, ty, tw, WM_TASKBAR_ACCENT);

    /* ─── زر BeeOS (القائمة الرئيسية) ─── */
    fb_rect(4, ty + 8, 80, 32, 0x8B6914);
    fb_outline(4, ty + 8, 80, 32, WM_TASKBAR_ACCENT);
    fb_string(14, ty + 16, "BeeOS", WM_TASKBAR_ACCENT, 2);

    /* ─── أزرار النوافذ المفتوحة ─── */
    int btn_x = 92;
    for (int z = 0; z < g_win_count; z++) {
        /* إيجاد النافذة بهذا z_order */
        for (int i = 0; i < WM_MAX_WINDOWS; i++) {
            if (g_windows[i].id != 0 && g_windows[i].z_order == z) {
                Window *win = &g_windows[i];

                /* حساب عرض النص */
                int text_len = 0;
                for (const char *p = win->title; *p; p++) text_len++;
                int btn_w = text_len * 6 + 16;
                if (btn_w < 70) btn_w = 70;
                if (btn_w > 120) btn_w = 120;

                /* لون الزر (نشط أو عادي) */
                unsigned int btn_bg = win->active ? WM_TASKBAR_BTN_ACT : WM_TASKBAR_BTN;
                unsigned int btn_border = win->active ? WM_TASKBAR_ACCENT : 0x5A4A3A;

                fb_rect(btn_x, ty + 8, btn_w, 32, btn_bg);
                fb_outline(btn_x, ty + 8, btn_w, 32, btn_border);

                /* خط علوي ذهبي للنافذة النشطة */
                if (win->active) {
                    fb_hline(btn_x + 1, ty + 8, btn_w - 2, WM_TASKBAR_ACCENT);
                    fb_hline(btn_x + 1, ty + 9, btn_w - 2, WM_TASKBAR_ACCENT);
                }

                /* نص العنوان (مقطوع إذا طويل) */
                fb_string(btn_x + 8, ty + 17, win->title, WM_TASKBAR_TEXT, 1);

                /* مؤشر التصغير */
                if (win->state == STATE_MINIMIZED) {
                    fb_char(btn_x + btn_w - 12, ty + 17, '_', 0xFFCC00, 1);
                }

                btn_x += btn_w + 4;
                break;
            }
        }
    }

    /* ─── منطقة النظام (يمين) ─── */
    int rx = tw - 180;

    /* مؤشر الشبكة */
    fb_rect(rx, ty + 10, 26, 28, 0x204020);
    fb_char(rx + 6, ty + 14, 'N', 0x40FF40, 2);

    /* مستوى الصوت */
    fb_rect(rx + 34, ty + 10, 26, 28, 0x202040);
    fb_char(rx + 40, ty + 14, 'V', 0x8080FF, 2);

    /* الساعة */
    fb_rect(rx + 80, ty + 8, 90, 32, 0x1A1A1A);
    fb_string(rx + 90, ty + 17, "14:47", WM_TASKBAR_ACCENT, 2);
}

/* ═══════════════════════════════════════════════════════════════════
 *  رسم مؤشر الماوس
 * ═══════════════════════════════════════════════════════════════════
 *
 * مؤشر سهم بسيط (12×18 بكسل):
 *   █
 *   ██
 *   ███
 *   ████
 *   █████
 *   ██████
 *   ███████
 *   ████████
 *   ████████
 *   ██████
 *   █████
 *   ██ ██
 *   █   █
 *      ██
 *      ██
 *       ██
 *       ██
 */
static void wm_draw_cursor(void) {
    static const unsigned char arrow[18] = {
        0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF,
        0xFF, 0xFC, 0xF8, 0xD8, 0x88, 0x0C, 0x0C, 0x06,
        0x06, 0x00,
    };
    for (int row = 0; row < 18; row++) {
        unsigned char bits = arrow[row];
        for (int col = 0; col < 8; col++) {
            if (bits & (0x80 >> col)) {
                fb_pixel(g_cursor_x + col, g_cursor_y + row, 0x000000);
            }
        }
    }
    /* حدود بيضاء على اليسار والأعلى */
    for (int row = 0; row < 16; row++) {
        unsigned char bits = arrow[row];
        for (int col = 0; col < 8; col++) {
            if (bits & (0x80 >> col)) {
                int left  = (col == 0) || !(bits & (0x80 >> (col-1)));
                int above = (row == 0) || !(arrow[row-1] & (0x80 >> col));
                if (left || above) fb_pixel(g_cursor_x + col, g_cursor_y + row, 0xFFFFFF);
                break;
            }
        }
    }
}

/* ═══════════════════════════════════════════════════════════════════
 *  رسم خلفية سطح المكتب — تدرج ذهبي/أزرق
 * ═══════════════════════════════════════════════════════════════════
 *
 * خلفية سطح المكتب: تدرج عمودي من أزرق داكن (أعلى)
 * إلى ذهبي (أسفل)، مثل أفق الصحراء عند المغرب.
 */
static unsigned int wm_lerp(unsigned int c1, unsigned int c2, int t, int max) {
    if (max == 0) return c1;
    int r1 = (c1 >> 16) & 0xFF, g1 = (c1 >> 8) & 0xFF, b1 = c1 & 0xFF;
    int r2 = (c2 >> 16) & 0xFF, g2 = (c2 >> 8) & 0xFF, b2 = c2 & 0xFF;
    int r = r1 + (r2 - r1) * t / max;
    int g = g1 + (g2 - g1) * t / max;
    int b = b1 + (b2 - b1) * t / max;
    return ((unsigned int)r << 16) | ((unsigned int)g << 8) | (unsigned int)b;
}

static void wm_draw_desktop_bg(void) {
    unsigned int top = 0x0A1628;   /* أزرق داكن — سماء الليل */
    unsigned int mid = 0x1A3A5C;   /* أزرق متوسط — أفق */
    unsigned int bot = 0x8B6914;   /* ذهبي داكن — رمال */

    int h = WM_SCREEN_H - WM_TASKBAR_H;
    int half = h / 2;

    for (int y = 0; y < half; y++) {
        unsigned int c = wm_lerp(top, mid, y, half);
        fb_hline(0, y, WM_SCREEN_W, c);
    }
    for (int y = half; y < h; y++) {
        unsigned int c = wm_lerp(mid, bot, y - half, h - half);
        fb_hline(0, y, WM_SCREEN_W, c);
    }
}

/* ═══════════════════════════════════════════════════════════════════
 *  رسم أيقونات سطح المكتب
 * ═══════════════════════════════════════════════════════════════════ */
static void wm_draw_desktop_icons(void) {
    static const struct {
        int x, y;
        unsigned int color;
        char letter;
        const char *label;
    } icons[] = {
        { 24,  30, 0x2A7FFF, 'F', "Files"  },
        { 24, 110, 0x555555, 'S', "Setup"  },
        { 24, 190, 0x3A9F3A, 'E', "Edit"   },
        { 24, 270, 0xE07020, 'C', "Calc"   },
        { 24, 350, 0x2060D0, 'W', "Web"    },
        { 24, 430, 0x1A1A1A, '>', "Shell"  },
        { 24, 510, 0x8040A0, 'U', "USB"    },
    };

    for (int i = 0; i < 7; i++) {
        /* ظل */
        fb_rect(icons[i].x + 3, icons[i].y + 3, 48, 48, 0x1A1A1A);
        /* خلفية الأيقونة */
        fb_rect(icons[i].x, icons[i].y, 48, 48, icons[i].color);
        /* حدود */
        fb_outline(icons[i].x, icons[i].y, 48, 48, 0xFFFFFF);
        /* حرف */
        fb_char(icons[i].x + 17, icons[i].y + 14, icons[i].letter, 0xFFFFFF, 3);
        /* تسمية */
        fb_string(icons[i].x - 4, icons[i].y + 54, icons[i].label, 0xFFFFFF, 1);
    }
}

/* ═══════════════════════════════════════════════════════════════════
 *  رسم شعار BeeOS في الزاوية العليا اليمنى
 * ═══════════════════════════════════════════════════════════════════ */
static void wm_draw_logo(void) {
    int lx = WM_SCREEN_W - 200;
    int ly = 20;

    fb_rect(lx + 30, ly, 40, 8, 0xFFD700);
    fb_rect(lx + 20, ly + 8, 60, 8, 0xFFD700);
    fb_rect(lx + 10, ly + 16, 80, 8, 0xFFD700);
    fb_rect(lx + 10, ly + 24, 80, 8, 0xDAA520);
    fb_rect(lx + 20, ly + 32, 60, 8, 0xDAA520);
    fb_rect(lx + 30, ly + 40, 40, 8, 0xDAA520);
    fb_string(lx + 4, ly + 56, "BeeOS HD", 0xFFD700, 2);
    fb_string(lx + 10, ly + 78, "v0.2.0", 0xC0C0C0, 1);
}

/* ═══════════════════════════════════════════════════════════════════
 *  الدالة الرئيسية: رسم سطح المكتب بنظام النوافذ
 * ═══════════════════════════════════════════════════════════════════
 *
 * خوارزمية الرسم (Painter's Algorithm):
 *   1. خلفية سطح المكتب (تدرج عمودي)
 *   2. أيقونات سطح المكتب (7 أيقونات)
 *   3. شعار BeeOS
 *   4. النوافذ المفتوحة (بترتيب z_order من الخلف للأمام)
 *   5. شريط المهام
 *   6. مؤشر الماوس (أعلى طبقة)
 *
 * كل نافذة تُرسم بترتيب z_order:
 *   z=0 أولاً (خلف الجميع) → z=N أخيراً (أمام الجميع)
 */
void wm_draw_all(void) {
    serial_puts("[WM] Drawing windowed desktop...\r\n");

    /* 1. الخلفية */
    serial_puts("[WM] Layer 1: Background\r\n");
    wm_draw_desktop_bg();

    /* 2. الأيقونات */
    serial_puts("[WM] Layer 2: Desktop icons\r\n");
    wm_draw_desktop_icons();

    /* 3. الشعار */
    serial_puts("[WM] Layer 3: Logo\r\n");
    wm_draw_logo();

    /* 4. النوافذ (بترتيب z_order) */
    serial_puts("[WM] Layer 4: Windows (z-ordered)\r\n");
    for (int z = 0; z < g_win_count; z++) {
        for (int i = 0; i < WM_MAX_WINDOWS; i++) {
            if (g_windows[i].id != 0 && g_windows[i].z_order == z) {
                wm_draw_single_window(&g_windows[i]);
                wm_draw_content(&g_windows[i]);
                break;
            }
        }
    }
    serial_puts("[WM] Layer 4 done\r\n");

    /* 5. شريط المهام */
    serial_puts("[WM] Layer 5: Taskbar\r\n");
    wm_draw_taskbar();

    /* 6. المؤشر */
    serial_puts("[WM] Layer 6: Cursor\r\n");
    wm_draw_cursor();

    serial_puts("[WM] Desktop rendering complete!\r\n");
}

/* ═══════════════════════════════════════════════════════════════════
 *  تهيئة مدير النوافذ — إنشاء النوافذ الافتراضية
 * ═══════════════════════════════════════════════════════════════════
 *
 * ينشئ 5 نوافذ لعرض قدرات نظام النوافذ:
 *   1. مدير الملفات (Files) — خلفية ذهبية
 *   2. الحاسبة (Calculator) — خلفية خضراء داكنة
 *   3. الطرفية (Terminal) — خلفية سوداء
 *   4. محرر النصوص (Editor) — خلفية خضراء
 *   5. معلومات النظام (System Info) — خلفية زرقاء
 *
 * النوافذ متراكبة/متداخلة لإظهار ترتيب العمق.
 */
void wm_init(void) {
    serial_puts("[WM] Initializing Window Manager...\r\n");

    /* تصفير مصفوفة النوافذ */
    for (int i = 0; i < WM_MAX_WINDOWS; i++) {
        g_windows[i].id = 0;
    }
    g_win_count = 0;
    g_next_id = 1;

    /* ─── نافذة 1: مدير الملفات ─── */
    int fid = wm_create_window("Files", 140, 50, 380, 340,
                               STYLE_NORMAL, 0x8B6914);
    for (int i = 0; i < WM_MAX_WINDOWS; i++) {
        if (g_windows[i].id == fid) {
            g_windows[i].content_type = 0;
            g_windows[i].has_scrollbar = 1;
            g_windows[i].scrollbar_pos = 15;
            break;
        }
    }

    /* ─── نافذة 2: الحاسبة ─── */
    int cid = wm_create_window("Calculator", 560, 80, 200, 300,
                               STYLE_DIALOG, 0x1A5A3A);
    for (int i = 0; i < WM_MAX_WINDOWS; i++) {
        if (g_windows[i].id == cid) {
            g_windows[i].content_type = 2;
            break;
        }
    }

    /* ─── نافذة 3: الطرفية ─── */
    int tid = wm_create_window("Terminal", 300, 320, 500, 260,
                               STYLE_NORMAL, 0x1A1A1A);
    for (int i = 0; i < WM_MAX_WINDOWS; i++) {
        if (g_windows[i].id == tid) {
            g_windows[i].content_type = 4;
            break;
        }
    }

    /* ─── نافذة 4: محرر النصوص ─── */
    int eid = wm_create_window("Editor", 200, 140, 440, 320,
                               STYLE_NORMAL, 0x2A6A2A);
    for (int i = 0; i < WM_MAX_WINDOWS; i++) {
        if (g_windows[i].id == eid) {
            g_windows[i].content_type = 1;
            g_windows[i].has_scrollbar = 1;
            g_windows[i].scrollbar_pos = 30;
            break;
        }
    }

    /* ─── نافذة 5: معلومات النظام ─── */
    int sid = wm_create_window("System Info", 450, 200, 320, 280,
                               STYLE_NORMAL, 0x2060A0);
    for (int i = 0; i < WM_MAX_WINDOWS; i++) {
        if (g_windows[i].id == sid) {
            g_windows[i].content_type = 6;
            break;
        }
    }

    serial_puts("[WM] Window Manager initialized: ");
    char num[4];
    num[0] = '0' + g_win_count;
    num[1] = 0;
    serial_puts(num);
    serial_puts(" windows\r\n");
}

/*
 * ═══════════════════════════════════════════════════════════════════
 *  ملاحظات التصميم ونقاط التطوير المستقبلية
 * ═══════════════════════════════════════════════════════════════════
 *
 * 1. ترتيب العمق (Z-ordering):
 *    - النوافذ تُرسم بترتيب z_order (0 = خلف، N = أمام)
 *    - wm_bring_to_front() ترفع النافذة للأمام عند النقر
 *    - wm_close_window() تُعيد ترتيب البقية تلقائياً
 *
 * 2. أنماط النوافذ:
 *    - STYLE_NORMAL: كل الأزرار (إغلاق + تصغير + تكبير)
 *    - STYLE_DIALOG: إغلاق فقط
 *    - STYLE_TOOL: عنوان أصغر (20px بدلاً من 28px)
 *    - STYLE_SPLASH: بدون شريط عنوان
 *
 * 3. الحالات:
 *    - STATE_NORMAL: مرئية بالحجم العادي
 *    - STATE_MAXIMIZED: ملء الشاشة (بدون شريط المهام)
 *    - STATE_MINIMIZED: مخفية (تظهر فقط في الشريط)
 *    - STATE_HIDDEN: مخفية تماماً
 *
 * 4. لجعل النظام تفاعلياً (مستقبلاً):
 *    - إعداد IDT مع معالجات IRQ1 (لوحة مفاتيح) و IRQ12 (ماوس)
 *    - حلقة أحداث (event loop) تفحص خط انتظار الأحداث
 *    - hit-testing: فحص نقرة الماوس على أي نافذة/زر
 *    - سحب النوافذ: تغيير x,y عند السحب على شريط العنوان
 *    - تحجيم النوافذ: تغيير w,h عند السحب على الحواف
 *    - إعادة رسم المناطق المتأثرة فقط (damage rectangles)
 *
 * 5. الكود يعمل على الأجهزة الحقيقية:
 *    - port I/O مباشر (BGA/PCI)
 *    - لا اعتماديات خارجية
 *    - خط bitmap مدمج
 *    - ألوان 32-bit XRGB
 *
 * بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ
 * الحمد لله رب العالمين
 * ═══════════════════════════════════════════════════════════════════
 */
