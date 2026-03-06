/* ============================================================================
 * sad_android_ui.h — محرك واجهة المستخدم لأندرويد
 * ============================================================================
 * محرك رسوميات أصلي يستخدم OpenGL ES 3.0 / EGL للرسم المباشر
 * بدون أي اعتماد على Java أو Kotlin.
 *
 * يدعم:
 *   - مكونات واجهة كاملة (أزرار، حقول نص، قوائم، تخطيطات)
 *   - نظام أحداث لمس (touch) وإيماءات
 *   - رسم نصوص UTF-8 (بما فيها العربية) عبر خرائط حروف
 *   - تخطيطات مرنة (عمودي/أفقي/شبكي) مع دعم RTL
 *   - تصيير مستطيلات دائرية/ظلال/تدرجات
 *   - حركات (animations) وتحولات
 *
 * Android native UI engine using OpenGL ES 3.0 / EGL.
 * No Java/Kotlin dependency — works with NativeActivity.
 * ============================================================================ */

#ifndef SAD_ANDROID_UI_H
#define SAD_ANDROID_UI_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * الثوابت / Constants
 * ============================================================================ */

#define SAD_UI_MAX_WIDGETS      1024    /* أقصى عدد للمكونات */
#define SAD_UI_MAX_CHILDREN     64      /* أقصى عدد أبناء لمكون */
#define SAD_UI_MAX_TEXT_LEN     512     /* أقصى طول نص */
#define SAD_UI_MAX_TOUCH_POINTS 10      /* أقصى نقاط لمس متزامنة */
#define SAD_UI_MAX_ANIMATIONS   64      /* أقصى عدد حركات متزامنة */

/* ============================================================================
 * الألوان / Colors
 * ============================================================================ */

typedef struct {
    float r, g, b, a;           /* 0.0 — 1.0 */
} SadColor;

/* ألوان مسبقة التعريف / Predefined colors */
#define SAD_COLOR_WHITE     ((SadColor){1.0f, 1.0f, 1.0f, 1.0f})
#define SAD_COLOR_BLACK     ((SadColor){0.0f, 0.0f, 0.0f, 1.0f})
#define SAD_COLOR_RED       ((SadColor){1.0f, 0.0f, 0.0f, 1.0f})
#define SAD_COLOR_GREEN     ((SadColor){0.0f, 0.8f, 0.0f, 1.0f})
#define SAD_COLOR_BLUE      ((SadColor){0.2f, 0.4f, 0.9f, 1.0f})
#define SAD_COLOR_GRAY      ((SadColor){0.5f, 0.5f, 0.5f, 1.0f})
#define SAD_COLOR_LIGHT_GRAY ((SadColor){0.9f, 0.9f, 0.9f, 1.0f})
#define SAD_COLOR_TRANSPARENT ((SadColor){0.0f, 0.0f, 0.0f, 0.0f})

/* ألوان Material Design */
#define SAD_COLOR_PRIMARY    ((SadColor){0.247f, 0.318f, 0.710f, 1.0f})
#define SAD_COLOR_ACCENT     ((SadColor){0.012f, 0.663f, 0.957f, 1.0f})
#define SAD_COLOR_SURFACE    ((SadColor){1.0f, 1.0f, 1.0f, 1.0f})
#define SAD_COLOR_ERROR      ((SadColor){0.698f, 0.133f, 0.133f, 1.0f})

SadColor sad_color_from_hex(uint32_t hex);
SadColor sad_color_lerp(SadColor a, SadColor b, float t);

/* ============================================================================
 * المستطيل والهندسة / Rect & Geometry
 * ============================================================================ */

typedef struct {
    float x, y, w, h;
} SadRect;

typedef struct {
    float x, y;
} SadPoint;

typedef struct {
    float w, h;
} SadSize;

/* هوامش وحشوة / Margin & Padding */
typedef struct {
    float top, right, bottom, left;
} SadEdgeInsets;

bool sad_rect_contains(SadRect rect, float px, float py);
SadRect sad_rect_inset(SadRect rect, SadEdgeInsets insets);

/* ============================================================================
 * أنواع المكونات / Widget Types
 * ============================================================================ */

typedef enum {
    SAD_WIDGET_VIEW = 0,        /* مكون أساسي (حاوية) */
    SAD_WIDGET_TEXT,             /* نص */
    SAD_WIDGET_BUTTON,          /* زر */
    SAD_WIDGET_TEXT_INPUT,      /* حقل إدخال نص */
    SAD_WIDGET_IMAGE,           /* صورة */
    SAD_WIDGET_SCROLL_VIEW,     /* حاوية قابلة للتمرير */
    SAD_WIDGET_LIST_VIEW,       /* قائمة */
    SAD_WIDGET_SWITCH,          /* مفتاح تبديل */
    SAD_WIDGET_CHECKBOX,        /* خانة اختيار */
    SAD_WIDGET_SLIDER,          /* شريط تمرير */
    SAD_WIDGET_PROGRESS,        /* شريط تقدم */
    SAD_WIDGET_DIVIDER,         /* فاصل */
    SAD_WIDGET_DIALOG,          /* نافذة حوار */
    SAD_WIDGET_TOAST,           /* رسالة مؤقتة */
    SAD_WIDGET_CUSTOM,          /* مكون مخصص */
} SadWidgetType;

/* ============================================================================
 * حالة المكون / Widget State
 * ============================================================================ */

typedef enum {
    SAD_STATE_NORMAL = 0,
    SAD_STATE_PRESSED,
    SAD_STATE_FOCUSED,
    SAD_STATE_DISABLED,
    SAD_STATE_HOVERED,
} SadWidgetState;

/* ============================================================================
 * التخطيط / Layout
 * ============================================================================ */

typedef enum {
    SAD_LAYOUT_NONE = 0,        /* بدون تخطيط (موقع مطلق) */
    SAD_LAYOUT_COLUMN,          /* عمودي (أعلى لأسفل) */
    SAD_LAYOUT_ROW,             /* أفقي (يسار ليمين / يمين ليسار RTL) */
    SAD_LAYOUT_GRID,            /* شبكي */
    SAD_LAYOUT_STACK,           /* تكديس (فوق بعض) */
    SAD_LAYOUT_FLEX,            /* مرن */
} SadLayoutType;

typedef enum {
    SAD_ALIGN_START = 0,        /* بداية (يسار / يمين في RTL) */
    SAD_ALIGN_CENTER,           /* وسط */
    SAD_ALIGN_END,              /* نهاية */
    SAD_ALIGN_STRETCH,          /* تمدد */
} SadAlignment;

typedef enum {
    SAD_GRAVITY_TOP_LEFT = 0,
    SAD_GRAVITY_TOP_CENTER,
    SAD_GRAVITY_TOP_RIGHT,
    SAD_GRAVITY_CENTER_LEFT,
    SAD_GRAVITY_CENTER,
    SAD_GRAVITY_CENTER_RIGHT,
    SAD_GRAVITY_BOTTOM_LEFT,
    SAD_GRAVITY_BOTTOM_CENTER,
    SAD_GRAVITY_BOTTOM_RIGHT,
} SadGravity;

typedef struct {
    SadLayoutType type;
    SadAlignment  main_align;       /* محاذاة المحور الرئيسي */
    SadAlignment  cross_align;      /* محاذاة المحور الثانوي */
    float         spacing;          /* المسافة بين العناصر */
    bool          rtl;              /* اتجاه يمين-ليسار (عربي) */
    int           grid_columns;     /* عدد الأعمدة (للشبكي فقط) */
    bool          wrap;             /* التفاف العناصر */
} SadLayout;

/* ============================================================================
 * أنماط المكون / Widget Style
 * ============================================================================ */

typedef struct {
    SadColor     bg_color;          /* لون الخلفية */
    SadColor     fg_color;          /* لون النص / الأمامي */
    SadColor     border_color;      /* لون الحدود */
    float        border_width;      /* سمك الحدود */
    float        corner_radius;     /* نصف قطر الزوايا */
    float        shadow_radius;     /* نصف قطر الظل */
    SadColor     shadow_color;      /* لون الظل */
    float        shadow_offset_x;   /* إزاحة الظل أفقياً */
    float        shadow_offset_y;   /* إزاحة الظل عمودياً */
    float        opacity;           /* الشفافية (0.0-1.0) */
    SadEdgeInsets padding;          /* الحشوة الداخلية */
    SadEdgeInsets margin;           /* الهامش الخارجي */
    float        font_size;         /* حجم الخط */
    bool         bold;              /* خط عريض */
    bool         italic;            /* خط مائل */
} SadStyle;

SadStyle sad_style_default(void);
SadStyle sad_style_button(void);
SadStyle sad_style_text_input(void);

/* ============================================================================
 * أحداث اللمس / Touch Events
 * ============================================================================ */

typedef enum {
    SAD_TOUCH_DOWN = 0,         /* لمسة بدأت */
    SAD_TOUCH_UP,               /* لمسة انتهت */
    SAD_TOUCH_MOVE,             /* حركة لمسة */
    SAD_TOUCH_CANCEL,           /* إلغاء لمسة */
} SadTouchAction;

typedef struct {
    int32_t      pointer_id;    /* معرف الإصبع */
    SadTouchAction action;
    float        x, y;          /* موقع اللمسة */
    float        pressure;      /* ضغط اللمسة */
    int64_t      timestamp_ns;  /* الطابع الزمني (نانو ثانية) */
} SadTouchEvent;

/* ============================================================================
 * أحداث المكون / Widget Events
 * ============================================================================ */

typedef enum {
    SAD_EVENT_CLICK = 0,        /* نقرة */
    SAD_EVENT_LONG_PRESS,       /* ضغط مطول */
    SAD_EVENT_TEXT_CHANGED,     /* تغير النص */
    SAD_EVENT_VALUE_CHANGED,    /* تغير القيمة */
    SAD_EVENT_FOCUS_CHANGED,    /* تغير التركيز */
    SAD_EVENT_SCROLL,           /* تمرير */
    SAD_EVENT_SWIPE,            /* سحب */
} SadEventType;

/* دالة رد الاتصال / Callback function pointer */
typedef void (*SadWidgetCallback)(int32_t widget_id, SadEventType event, void* user_data);

/* ============================================================================
 * بنية المكون / Widget Structure
 * ============================================================================ */

typedef struct SadWidget {
    int32_t         id;                     /* معرف فريد */
    SadWidgetType   type;                   /* نوع المكون */
    SadWidgetState  state;                  /* الحالة الحالية */
    SadRect         frame;                  /* الإطار المحسوب */
    SadSize         requested_size;         /* الحجم المطلوب */
    SadStyle        style;                  /* الأنماط */
    SadLayout       layout;                 /* التخطيط (للحاويات) */
    bool            visible;                /* مرئي */
    bool            enabled;                /* مفعّل */
    bool            needs_layout;           /* يحتاج إعادة تخطيط */
    bool            needs_redraw;           /* يحتاج إعادة رسم */
    bool            clips_children;         /* قص الأبناء */

    /* النص / Text */
    char            text[SAD_UI_MAX_TEXT_LEN];
    SadAlignment    text_align;

    /* البيانات الخاصة بالنوع / Type-specific data */
    union {
        struct { bool checked; } toggle;                    /* مفتاح/خانة */
        struct { float value; float min; float max; } slider; /* شريط */
        struct { float progress; } progress;                /* تقدم */
        struct { float scroll_x, scroll_y; } scroll;       /* تمرير */
        struct { int cursor_pos; bool editing; } text_input; /* إدخال */
    } data;

    /* الشجرة / Tree hierarchy */
    int32_t         parent_id;              /* معرف الأب (-1 = جذر) */
    int32_t         children[SAD_UI_MAX_CHILDREN];
    int32_t         child_count;

    /* الأحداث / Events */
    SadWidgetCallback on_event;
    void*           user_data;
} SadWidget;

/* ============================================================================
 * محرك واجهة المستخدم / UI Engine
 * ============================================================================ */

typedef struct SadUIEngine SadUIEngine;

/* ── الإنشاء والتدمير / Creation & Destruction ── */
SadUIEngine*    sad_ui_create(void);
void            sad_ui_destroy(SadUIEngine* engine);

/* ── التهيئة مع نافذة أندرويد / Init with Android window ── */
/* يأخذ ANativeWindow* من NativeActivity */
bool            sad_ui_init(SadUIEngine* engine, void* native_window,
                            int32_t width, int32_t height, float density);
void            sad_ui_resize(SadUIEngine* engine, int32_t width, int32_t height);
void            sad_ui_set_density(SadUIEngine* engine, float density);

/* ── إدارة المكونات / Widget Management ── */
int32_t         sad_ui_create_widget(SadUIEngine* engine, SadWidgetType type);
void            sad_ui_remove_widget(SadUIEngine* engine, int32_t id);
SadWidget*      sad_ui_get_widget(SadUIEngine* engine, int32_t id);
void            sad_ui_set_root(SadUIEngine* engine, int32_t id);
void            sad_ui_add_child(SadUIEngine* engine, int32_t parent_id, int32_t child_id);
void            sad_ui_remove_child(SadUIEngine* engine, int32_t parent_id, int32_t child_id);

/* ── خصائص المكون / Widget Properties ── */
void            sad_ui_set_text(SadUIEngine* engine, int32_t id, const char* text);
void            sad_ui_set_position(SadUIEngine* engine, int32_t id, float x, float y);
void            sad_ui_set_size(SadUIEngine* engine, int32_t id, float w, float h);
void            sad_ui_set_style(SadUIEngine* engine, int32_t id, SadStyle style);
void            sad_ui_set_layout(SadUIEngine* engine, int32_t id, SadLayout layout);
void            sad_ui_set_visible(SadUIEngine* engine, int32_t id, bool visible);
void            sad_ui_set_enabled(SadUIEngine* engine, int32_t id, bool enabled);
void            sad_ui_set_callback(SadUIEngine* engine, int32_t id,
                                    SadWidgetCallback callback, void* user_data);
const char*     sad_ui_get_text(SadUIEngine* engine, int32_t id);

/* ── مكونات مختصرة / Shorthand widget creation ── */

/* زر / Button */
int32_t         sad_ui_button(SadUIEngine* engine, const char* text,
                              SadWidgetCallback on_click, void* user_data);
/* نص / Text label */
int32_t         sad_ui_text(SadUIEngine* engine, const char* text, float font_size);

/* حقل إدخال / Text input */
int32_t         sad_ui_text_input(SadUIEngine* engine, const char* placeholder);

/* تبديل / Switch */
int32_t         sad_ui_switch_(SadUIEngine* engine, bool initial_value,
                               SadWidgetCallback on_change, void* user_data);

/* شريط تقدم / Progress bar */
int32_t         sad_ui_progress(SadUIEngine* engine, float initial_value);

/* شريط تمرير / Slider */
int32_t         sad_ui_slider(SadUIEngine* engine, float min, float max, float value,
                              SadWidgetCallback on_change, void* user_data);

/* حاوية عمودية / Vertical container */
int32_t         sad_ui_column(SadUIEngine* engine, float spacing);

/* حاوية أفقية / Horizontal container */
int32_t         sad_ui_row(SadUIEngine* engine, float spacing);

/* حاوية قابلة للتمرير / Scroll view */
int32_t         sad_ui_scroll_view(SadUIEngine* engine);

/* فاصل / Divider */
int32_t         sad_ui_divider(SadUIEngine* engine);

/* حوار / Dialog */
int32_t         sad_ui_dialog(SadUIEngine* engine, const char* title, const char* message);

/* رسالة مؤقتة / Toast */
void            sad_ui_show_toast(SadUIEngine* engine, const char* message, int duration_ms);

/* ── التصيير والأحداث / Rendering & Events ── */
void            sad_ui_layout(SadUIEngine* engine);      /* إعادة حساب التخطيط */
void            sad_ui_render(SadUIEngine* engine);      /* رسم الإطار */
bool            sad_ui_handle_touch(SadUIEngine* engine, SadTouchEvent event);

/* ── خريطة الحروف / Font Atlas ── */
/* تحميل خط TTF لرسم النصوص (مطلوب قبل الرسم) */
bool            sad_ui_load_font(SadUIEngine* engine, const void* ttf_data,
                                 int32_t ttf_size, float default_size);

/* ── الحركات / Animations ── */

typedef enum {
    SAD_ANIM_FADE_IN = 0,
    SAD_ANIM_FADE_OUT,
    SAD_ANIM_SLIDE_LEFT,
    SAD_ANIM_SLIDE_RIGHT,
    SAD_ANIM_SLIDE_UP,
    SAD_ANIM_SLIDE_DOWN,
    SAD_ANIM_SCALE,
    SAD_ANIM_COLOR,
} SadAnimationType;

typedef enum {
    SAD_EASE_LINEAR = 0,
    SAD_EASE_IN,
    SAD_EASE_OUT,
    SAD_EASE_IN_OUT,
    SAD_EASE_BOUNCE,
} SadEasing;

typedef void (*SadAnimationDone)(int32_t widget_id, void* user_data);

void            sad_ui_animate(SadUIEngine* engine, int32_t widget_id,
                               SadAnimationType type, float duration_ms,
                               SadEasing easing, SadAnimationDone on_done,
                               void* user_data);

void            sad_ui_animate_to(SadUIEngine* engine, int32_t widget_id,
                                  SadRect target_frame, float duration_ms,
                                  SadEasing easing);

/* ── المظهر / Theme ── */

typedef struct {
    SadColor    primary;
    SadColor    on_primary;         /* لون النص على Primary */
    SadColor    secondary;
    SadColor    on_secondary;
    SadColor    background;
    SadColor    surface;
    SadColor    error;
    SadColor    on_error;
    SadColor    text_primary;
    SadColor    text_secondary;
    SadColor    divider;
    float       corner_radius;
    float       default_font_size;
    float       button_height;
    float       input_height;
    bool        rtl;                /* اتجاه يمين-ليسار */
} SadTheme;

SadTheme        sad_theme_default(void);
SadTheme        sad_theme_dark(void);
SadTheme        sad_theme_arabic(void);       /* مظهر عربي مع RTL */
void            sad_ui_set_theme(SadUIEngine* engine, SadTheme theme);

/* ── حلقة التطبيق / Application Loop ── */
/* التكامل مع NativeActivity — looper-based event handling */

typedef struct {
    SadUIEngine*    ui;
    void*           native_activity;    /* ANativeActivity* */
    void*           native_window;      /* ANativeWindow* */
    bool            running;
    bool            has_focus;
    int32_t         width, height;
    float           density;
} SadApp;

SadApp*     sad_app_create(void* native_activity);
void        sad_app_destroy(SadApp* app);
void        sad_app_set_window(SadApp* app, void* native_window, int32_t w, int32_t h);
void        sad_app_clear_window(SadApp* app);
void        sad_app_main_loop(SadApp* app);

/* دالة نقطة الدخول التي يُعرّفها المستخدم بلغة ص */
/* Entry point defined by user in Sad language */
extern void sad_app_on_create(SadApp* app);

#ifdef __cplusplus
}
#endif

#endif /* SAD_ANDROID_UI_H */
