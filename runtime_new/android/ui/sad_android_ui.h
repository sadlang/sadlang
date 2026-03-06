/**
 * ==========================================================================
 * sad_android_ui.h - محرك واجهة المستخدم الرسومي لأندرويد
 * Android Native UI Engine Header
 * ==========================================================================
 * 
 * الوصف (عربي):
 * --------------
 * هذا الملف يُعرّف محرك واجهة المستخدم الأصلي لتطبيقات لغة ص على أندرويد.
 * المحرك مبني على OpenGL ES 3.0 ويعمل بدون Java/Kotlin — كل الواجهات
 * تُرسم مباشرة عبر OpenGL باستخدام NativeActivity.
 * 
 * الميزات الرئيسية:
 * - نظام ويدجت كامل (نص، زر، حقل إدخال، قائمة، ...)
 * - دعم كامل للنصوص العربية مع RTL
 * - نظام تخطيط مرن (عمود، صف، مكدس)
 * - معالجة أحداث اللمس والإيماءات
 * - رسم نصوص Unicode مع خطوط TrueType
 * - تحريكات سلسة (animations)
 * - نظام ألوان Material Design
 * 
 * البنية المعمارية:
 * -----------------
 *   SadApp (التطبيق)
 *      └── SadScreen (الشاشة)
 *             └── SadWidget (الواجهة)
 *                    ├── SadText
 *                    ├── SadButton
 *                    ├── SadInput
 *                    ├── SadList
 *                    ├── SadColumn
 *                    ├── SadRow
 *                    └── ...
 * 
 * Description (English):
 * ----------------------
 * This file defines the native UI engine for Sad language Android apps.
 * Built on OpenGL ES 3.0, works without Java/Kotlin — all UI is rendered
 * directly via OpenGL using NativeActivity.
 * 
 * Key Features:
 * - Complete widget system (text, button, input, list, ...)
 * - Full Arabic text support with RTL
 * - Flexible layout system (column, row, stack)
 * - Touch and gesture event handling
 * - Unicode text rendering with TrueType fonts
 * - Smooth animations
 * - Material Design color system
 * 
 * ==========================================================================
 * المؤلف: فريق تطوير لغة ص | Author: Sad Language Team
 * الرخصة: MIT | License: MIT
 * ==========================================================================
 */

#ifndef SAD_ANDROID_UI_H
#define SAD_ANDROID_UI_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================
 * الثوابت / Constants
 * ========================================================================= */

/** أقصى عدد أطفال لواجهة واحدة / Max children per widget */
#define SAD_UI_MAX_CHILDREN     64

/** أقصى طول نص في واجهة / Max text length in widget */
#define SAD_UI_MAX_TEXT         1024

/** أقصى عدد شاشات في التطبيق / Max screens in app */
#define SAD_UI_MAX_SCREENS      32

/** أقصى عدد رسوم متحركة نشطة / Max active animations */
#define SAD_UI_MAX_ANIMATIONS   128

/** أقصى عدد نقاط لمس متزامنة / Max simultaneous touch points */
#define SAD_UI_MAX_TOUCH_POINTS 10


/* =========================================================================
 * أنواع الواجهات / Widget Types
 * ========================================================================= */

/**
 * نوع الواجهة / Widget type enumeration
 * 
 * كل واجهة لها نوع يُحدد سلوكها ورسمها
 * Each widget has a type that determines its behavior and rendering
 */
typedef enum SadWidgetType {
    /* --- حاويات / Containers --- */
    SAD_WIDGET_CONTAINER = 0,   /**< حاوية عامة / Generic container */
    SAD_WIDGET_COLUMN,          /**< عمود (تخطيط عمودي) / Column (vertical layout) */
    SAD_WIDGET_ROW,             /**< صف (تخطيط أفقي) / Row (horizontal layout) */
    SAD_WIDGET_STACK,           /**< مكدس (تراكب) / Stack (overlay) */
    SAD_WIDGET_SCROLL,          /**< قابل للتمرير / Scrollable container */
    SAD_WIDGET_CARD,            /**< بطاقة Material / Material card */
    
    /* --- نصوص / Text --- */
    SAD_WIDGET_TEXT,            /**< نص ثابت / Static text */
    SAD_WIDGET_RICH_TEXT,       /**< نص منسق / Rich text */
    
    /* --- مدخلات / Inputs --- */
    SAD_WIDGET_BUTTON,          /**< زر / Button */
    SAD_WIDGET_TEXT_INPUT,      /**< حقل إدخال نص / Text input field */
    SAD_WIDGET_CHECKBOX,        /**< مربع اختيار / Checkbox */
    SAD_WIDGET_RADIO,           /**< زر راديو / Radio button */
    SAD_WIDGET_SWITCH,          /**< مفتاح تبديل / Toggle switch */
    SAD_WIDGET_SLIDER,          /**< منزلق / Slider */
    SAD_WIDGET_DROPDOWN,        /**< قائمة منسدلة / Dropdown */
    
    /* --- قوائم / Lists --- */
    SAD_WIDGET_LIST,            /**< قائمة / List view */
    SAD_WIDGET_GRID,            /**< شبكة / Grid view */
    
    /* --- وسائط / Media --- */
    SAD_WIDGET_IMAGE,           /**< صورة / Image */
    SAD_WIDGET_ICON,            /**< أيقونة / Icon */
    
    /* --- تنقل / Navigation --- */
    SAD_WIDGET_APP_BAR,         /**< شريط التطبيق / App bar */
    SAD_WIDGET_BOTTOM_NAV,      /**< تنقل سفلي / Bottom navigation */
    SAD_WIDGET_DRAWER,          /**< درج جانبي / Navigation drawer */
    SAD_WIDGET_TAB_BAR,         /**< شريط تبويبات / Tab bar */
    
    /* --- حوارات / Dialogs --- */
    SAD_WIDGET_DIALOG,          /**< حوار / Dialog */
    SAD_WIDGET_SNACKBAR,        /**< إشعار سفلي / Snackbar */
    SAD_WIDGET_TOOLTIP,         /**< تلميح / Tooltip */
    
    /* --- مخصص / Custom --- */
    SAD_WIDGET_CANVAS,          /**< لوحة رسم / Canvas for custom drawing */
    SAD_WIDGET_CUSTOM,          /**< واجهة مخصصة / Custom widget */
    
    SAD_WIDGET_TYPE_COUNT       /**< عدد الأنواع / Type count */
} SadWidgetType;


/* =========================================================================
 * الألوان / Colors (Material Design 3)
 * ========================================================================= */

/**
 * هيكل اللون (RGBA)
 * Color structure (RGBA format)
 * 
 * القيم من 0.0 إلى 1.0
 * Values range from 0.0 to 1.0
 */
typedef struct SadColor {
    float r;    /**< الأحمر / Red component */
    float g;    /**< الأخضر / Green component */
    float b;    /**< الأزرق / Blue component */
    float a;    /**< الشفافية / Alpha (transparency) */
} SadColor;

/** إنشاء لون من RGB (ألفا = 1) / Create color from RGB (alpha = 1) */
#define SAD_RGB(r, g, b) ((SadColor){(r)/255.0f, (g)/255.0f, (b)/255.0f, 1.0f})

/** إنشاء لون من RGBA / Create color from RGBA */
#define SAD_RGBA(r, g, b, a) ((SadColor){(r)/255.0f, (g)/255.0f, (b)/255.0f, (a)/255.0f})

/** إنشاء لون من HEX / Create color from HEX */
#define SAD_HEX(hex) ((SadColor){ \
    ((hex >> 16) & 0xFF) / 255.0f, \
    ((hex >> 8) & 0xFF) / 255.0f, \
    (hex & 0xFF) / 255.0f, \
    1.0f})

/* --- ألوان Material Design 3 الأساسية / Material Design 3 Colors --- */
static const SadColor SAD_COLOR_PRIMARY       = {0.102f, 0.459f, 0.827f, 1.0f};  /* أزرق أساسي */
static const SadColor SAD_COLOR_SECONDARY     = {0.600f, 0.247f, 0.800f, 1.0f};  /* بنفسجي ثانوي */
static const SadColor SAD_COLOR_TERTIARY      = {0.490f, 0.718f, 0.208f, 1.0f};  /* أخضر ثالثي */
static const SadColor SAD_COLOR_ERROR         = {0.698f, 0.133f, 0.133f, 1.0f};  /* أحمر خطأ */
static const SadColor SAD_COLOR_SURFACE       = {1.000f, 1.000f, 1.000f, 1.0f};  /* سطح أبيض */
static const SadColor SAD_COLOR_BACKGROUND    = {0.976f, 0.976f, 0.976f, 1.0f};  /* خلفية رمادية */
static const SadColor SAD_COLOR_ON_PRIMARY    = {1.000f, 1.000f, 1.000f, 1.0f};  /* نص على أساسي */
static const SadColor SAD_COLOR_ON_SURFACE    = {0.118f, 0.118f, 0.118f, 1.0f};  /* نص على سطح */
static const SadColor SAD_COLOR_OUTLINE       = {0.467f, 0.467f, 0.510f, 1.0f};  /* حدود */
static const SadColor SAD_COLOR_TRANSPARENT   = {0.000f, 0.000f, 0.000f, 0.0f};  /* شفاف */


/* =========================================================================
 * الهندسة / Geometry
 * ========================================================================= */

/**
 * نقطة ثنائية الأبعاد
 * 2D Point
 */
typedef struct SadPoint {
    float x;    /**< الإحداثي الأفقي / X coordinate */
    float y;    /**< الإحداثي العمودي / Y coordinate */
} SadPoint;

/**
 * حجم (عرض × ارتفاع)
 * Size (width × height)
 */
typedef struct SadSize {
    float width;    /**< العرض / Width */
    float height;   /**< الارتفاع / Height */
} SadSize;

/**
 * مستطيل (موقع + حجم)
 * Rectangle (position + size)
 */
typedef struct SadRect {
    float x;        /**< الموقع الأفقي / X position */
    float y;        /**< الموقع العمودي / Y position */
    float width;    /**< العرض / Width */
    float height;   /**< الارتفاع / Height */
} SadRect;

/**
 * حواف (للهوامش والبطانة)
 * Edge insets (for margins and padding)
 */
typedef struct SadEdgeInsets {
    float top;      /**< الأعلى / Top */
    float right;    /**< اليمين / Right */
    float bottom;   /**< الأسفل / Bottom */
    float left;     /**< اليسار / Left */
} SadEdgeInsets;

/** حواف متساوية / Uniform edge insets */
#define SAD_INSETS_ALL(v)     ((SadEdgeInsets){(v), (v), (v), (v)})

/** حواف أفقية وعمودية / Symmetric edge insets */
#define SAD_INSETS_SYM(h, v)  ((SadEdgeInsets){(v), (h), (v), (h)})

/** حواف صفرية / Zero edge insets */
#define SAD_INSETS_ZERO       ((SadEdgeInsets){0, 0, 0, 0})


/* =========================================================================
 * المحاذاة / Alignment
 * ========================================================================= */

/**
 * محاذاة المحور الرئيسي (للعمود = عمودي، للصف = أفقي)
 * Main axis alignment (column = vertical, row = horizontal)
 */
typedef enum SadMainAxisAlign {
    SAD_MAIN_START,         /**< البداية / Start */
    SAD_MAIN_CENTER,        /**< الوسط / Center */
    SAD_MAIN_END,           /**< النهاية / End */
    SAD_MAIN_SPACE_BETWEEN, /**< مسافات بين / Space between */
    SAD_MAIN_SPACE_AROUND,  /**< مسافات حول / Space around */
    SAD_MAIN_SPACE_EVENLY   /**< مسافات متساوية / Space evenly */
} SadMainAxisAlign;

/**
 * محاذاة المحور المتقاطع
 * Cross axis alignment
 */
typedef enum SadCrossAxisAlign {
    SAD_CROSS_START,    /**< البداية / Start */
    SAD_CROSS_CENTER,   /**< الوسط / Center */
    SAD_CROSS_END,      /**< النهاية / End */
    SAD_CROSS_STRETCH   /**< تمديد / Stretch */
} SadCrossAxisAlign;

/**
 * محاذاة النص
 * Text alignment
 */
typedef enum SadTextAlign {
    SAD_TEXT_ALIGN_START,   /**< البداية (يمين للعربية) / Start (right for Arabic) */
    SAD_TEXT_ALIGN_CENTER,  /**< وسط / Center */
    SAD_TEXT_ALIGN_END,     /**< النهاية (يسار للعربية) / End (left for Arabic) */
    SAD_TEXT_ALIGN_JUSTIFY  /**< ضبط / Justify */
} SadTextAlign;


/* =========================================================================
 * أحداث اللمس / Touch Events
 * ========================================================================= */

/**
 * نوع حدث اللمس
 * Touch event type
 */
typedef enum SadTouchType {
    SAD_TOUCH_DOWN,     /**< إصبع لمس الشاشة / Finger touched screen */
    SAD_TOUCH_MOVE,     /**< إصبع يتحرك / Finger moving */
    SAD_TOUCH_UP,       /**< إصبع رُفع / Finger lifted */
    SAD_TOUCH_CANCEL    /**< الحدث أُلغي / Event cancelled */
} SadTouchType;

/**
 * نقطة لمس واحدة
 * Single touch point
 */
typedef struct SadTouchPoint {
    int32_t id;         /**< معرّف الإصبع / Finger ID */
    float x;            /**< الموقع الأفقي / X position */
    float y;            /**< الموقع العمودي / Y position */
    float pressure;     /**< الضغط (0-1) / Pressure (0-1) */
} SadTouchPoint;

/**
 * حدث لمس كامل
 * Full touch event
 */
typedef struct SadTouchEvent {
    SadTouchType type;                          /**< نوع الحدث / Event type */
    int32_t pointer_count;                      /**< عدد نقاط اللمس / Touch point count */
    SadTouchPoint points[SAD_UI_MAX_TOUCH_POINTS]; /**< نقاط اللمس / Touch points */
    int64_t timestamp_ms;                       /**< الطابع الزمني / Timestamp in ms */
} SadTouchEvent;


/* =========================================================================
 * الإيماءات / Gestures
 * ========================================================================= */

/**
 * نوع الإيماءة
 * Gesture type
 */
typedef enum SadGestureType {
    SAD_GESTURE_TAP,            /**< نقرة / Tap */
    SAD_GESTURE_DOUBLE_TAP,     /**< نقرة مزدوجة / Double tap */
    SAD_GESTURE_LONG_PRESS,     /**< ضغط مطول / Long press */
    SAD_GESTURE_SWIPE_LEFT,     /**< سحب يسار / Swipe left */
    SAD_GESTURE_SWIPE_RIGHT,    /**< سحب يمين / Swipe right */
    SAD_GESTURE_SWIPE_UP,       /**< سحب أعلى / Swipe up */
    SAD_GESTURE_SWIPE_DOWN,     /**< سحب أسفل / Swipe down */
    SAD_GESTURE_PINCH,          /**< قرص (تكبير/تصغير) / Pinch (zoom) */
    SAD_GESTURE_ROTATE,         /**< دوران / Rotation */
    SAD_GESTURE_PAN             /**< سحب / Pan */
} SadGestureType;

/**
 * حدث إيماءة
 * Gesture event
 */
typedef struct SadGestureEvent {
    SadGestureType type;    /**< نوع الإيماءة / Gesture type */
    SadPoint location;      /**< موقع الحدث / Event location */
    SadPoint velocity;      /**< السرعة (للسحب) / Velocity (for swipe) */
    float scale;            /**< مقياس (للقرص) / Scale (for pinch) */
    float rotation;         /**< زاوية (للدوران) / Angle (for rotation) */
} SadGestureEvent;


/* =========================================================================
 * دوال الاستدعاء / Callbacks
 * ========================================================================= */

/* مقابض أمامية / Forward declarations */
struct SadWidget;
struct SadScreen;
struct SadApp;

/** دالة استدعاء للنقر / Click callback */
typedef void (*SadOnClickCallback)(struct SadWidget* widget, void* user_data);

/** دالة استدعاء لتغير النص / Text change callback */
typedef void (*SadOnTextChangeCallback)(struct SadWidget* widget, const char* text, void* user_data);

/** دالة استدعاء لتغير القيمة / Value change callback */
typedef void (*SadOnValueChangeCallback)(struct SadWidget* widget, float value, void* user_data);

/** دالة استدعاء للإيماءات / Gesture callback */
typedef void (*SadOnGestureCallback)(struct SadWidget* widget, SadGestureEvent* gesture, void* user_data);

/** دالة استدعاء للتمرير / Scroll callback */
typedef void (*SadOnScrollCallback)(struct SadWidget* widget, float scroll_x, float scroll_y, void* user_data);

/** دالة استدعاء للتحديث / Update callback (called every frame) */
typedef void (*SadOnUpdateCallback)(struct SadWidget* widget, float delta_time, void* user_data);

/** دالة استدعاء للرسم المخصص / Custom draw callback */
typedef void (*SadOnDrawCallback)(struct SadWidget* widget, void* user_data);


/* =========================================================================
 * حالة الواجهة / Widget State
 * ========================================================================= */

/**
 * حالة الواجهة التفاعلية
 * Interactive widget state
 */
typedef enum SadWidgetState {
    SAD_STATE_NORMAL,       /**< عادية / Normal */
    SAD_STATE_HOVERED,      /**< مُمرر فوقها / Hovered (not common on mobile) */
    SAD_STATE_PRESSED,      /**< مضغوطة / Pressed */
    SAD_STATE_FOCUSED,      /**< مُركّز عليها / Focused */
    SAD_STATE_DISABLED      /**< معطّلة / Disabled */
} SadWidgetState;


/* =========================================================================
 * هيكل الواجهة الرئيسي / Main Widget Structure
 * ========================================================================= */

/**
 * الواجهة (Widget)
 * 
 * الهيكل الرئيسي لكل عنصر واجهة مستخدم.
 * يحتوي على الخصائص المشتركة بين جميع أنواع الواجهات.
 * 
 * Main structure for all UI elements.
 * Contains properties shared by all widget types.
 */
typedef struct SadWidget {
    /* --- الهوية / Identity --- */
    uint32_t id;                    /**< معرّف فريد / Unique ID */
    SadWidgetType type;             /**< نوع الواجهة / Widget type */
    char tag[64];                   /**< وسم اختياري للتعريف / Optional tag for identification */
    
    /* --- الهندسة / Geometry --- */
    SadRect bounds;                 /**< الحدود المحسوبة / Computed bounds */
    SadRect requested_bounds;       /**< الحدود المطلوبة / Requested bounds */
    SadEdgeInsets margin;           /**< الهامش الخارجي / Outer margin */
    SadEdgeInsets padding;          /**< البطانة الداخلية / Inner padding */
    float flex;                     /**< معامل المرونة (0 = ثابت) / Flex factor (0 = fixed) */
    
    /* --- الحالة / State --- */
    SadWidgetState state;           /**< الحالة التفاعلية / Interactive state */
    bool visible;                   /**< هل ظاهرة؟ / Is visible? */
    bool enabled;                   /**< هل مفعّلة؟ / Is enabled? */
    bool focusable;                 /**< هل قابلة للتركيز؟ / Is focusable? */
    bool clip_children;             /**< هل تقص الأطفال؟ / Clip children? */
    
    /* --- المظهر / Appearance --- */
    SadColor background_color;      /**< لون الخلفية / Background color */
    SadColor foreground_color;      /**< لون المقدمة / Foreground color */
    SadColor border_color;          /**< لون الحدود / Border color */
    float border_width;             /**< سمك الحدود / Border width */
    float corner_radius;            /**< نصف قطر الزوايا / Corner radius */
    float opacity;                  /**< الشفافية (0-1) / Opacity (0-1) */
    float elevation;                /**< الارتفاع (للظل) / Elevation (for shadow) */
    
    /* --- النص (للواجهات النصية) / Text (for text widgets) --- */
    char text[SAD_UI_MAX_TEXT];     /**< النص / Text content */
    float font_size;                /**< حجم الخط / Font size */
    SadTextAlign text_align;        /**< محاذاة النص / Text alignment */
    bool is_rtl;                    /**< هل النص من اليمين؟ / Is text RTL? */
    bool is_multiline;              /**< هل متعدد الأسطر؟ / Is multiline? */
    
    /* --- التخطيط / Layout --- */
    SadMainAxisAlign main_align;    /**< محاذاة المحور الرئيسي / Main axis alignment */
    SadCrossAxisAlign cross_align;  /**< محاذاة المحور المتقاطع / Cross axis alignment */
    float spacing;                  /**< المسافة بين الأطفال / Spacing between children */
    
    /* --- الأطفال / Children --- */
    struct SadWidget* children[SAD_UI_MAX_CHILDREN]; /**< الأطفال / Children */
    int32_t child_count;            /**< عدد الأطفال / Child count */
    struct SadWidget* parent;       /**< الأب / Parent */
    
    /* --- دوال الاستدعاء / Callbacks --- */
    SadOnClickCallback on_click;        /**< عند النقر / On click */
    SadOnTextChangeCallback on_text_change; /**< عند تغير النص / On text change */
    SadOnValueChangeCallback on_value_change; /**< عند تغير القيمة / On value change */
    SadOnGestureCallback on_gesture;    /**< عند الإيماءة / On gesture */
    SadOnScrollCallback on_scroll;      /**< عند التمرير / On scroll */
    SadOnUpdateCallback on_update;      /**< عند التحديث / On update */
    SadOnDrawCallback on_draw;          /**< عند الرسم / On custom draw */
    void* callback_user_data;           /**< بيانات المستخدم / User data for callbacks */
    
    /* --- بيانات خاصة بالنوع / Type-specific data --- */
    union {
        struct {                        /* للزر / For button */
            bool is_filled;
            bool is_outlined;
        } button;
        
        struct {                        /* لحقل الإدخال / For text input */
            char placeholder[128];
            char hint[128];
            int32_t cursor_pos;
            int32_t selection_start;
            int32_t selection_end;
            bool is_password;
            bool is_numeric;
        } input;
        
        struct {                        /* للمنزلق / For slider */
            float min_value;
            float max_value;
            float current_value;
            float step;
        } slider;
        
        struct {                        /* لـ checkbox/switch / For checkbox/switch */
            bool is_checked;
        } toggle;
        
        struct {                        /* للقائمة / For list */
            int32_t item_count;
            int32_t selected_index;
            float item_height;
            float scroll_offset;
        } list;
        
        struct {                        /* للصورة / For image */
            uint32_t texture_id;
            float aspect_ratio;
        } image;
        
        struct {                        /* للتمرير / For scroll view */
            float content_width;
            float content_height;
            float scroll_x;
            float scroll_y;
            bool scroll_horizontal;
            bool scroll_vertical;
        } scroll;
    } data;
    
    /* --- الرسوم المتحركة / Animation --- */
    bool is_animating;              /**< هل تتحرك؟ / Is animating? */
    float animation_progress;       /**< تقدم الرسوم / Animation progress (0-1) */
    
} SadWidget;


/* =========================================================================
 * الشاشة / Screen
 * ========================================================================= */

/**
 * الشاشة (Screen)
 * 
 * تمثل شاشة كاملة في التطبيق (مثل صفحة).
 * التطبيق يحتوي على عدة شاشات يتنقل بينها المستخدم.
 * 
 * Represents a full screen in the app (like a page).
 * The app contains multiple screens the user navigates between.
 */
typedef struct SadScreen {
    char name[64];                  /**< اسم الشاشة / Screen name */
    SadWidget* root;                /**< الواجهة الجذر / Root widget */
    SadColor background_color;      /**< لون الخلفية / Background color */
    bool is_modal;                  /**< هل هي نافذة منبثقة؟ / Is modal? */
    void* user_data;                /**< بيانات المستخدم / User data */
    
    /* --- دوال دورة الحياة / Lifecycle callbacks --- */
    void (*on_enter)(struct SadScreen* screen);     /**< عند الدخول / On enter */
    void (*on_exit)(struct SadScreen* screen);      /**< عند الخروج / On exit */
    void (*on_pause)(struct SadScreen* screen);     /**< عند الإيقاف المؤقت / On pause */
    void (*on_resume)(struct SadScreen* screen);    /**< عند الاستئناف / On resume */
    
} SadScreen;


/* =========================================================================
 * التطبيق / Application
 * ========================================================================= */

/**
 * التطبيق (App)
 * 
 * الهيكل الرئيسي للتطبيق. يُدير:
 * - الشاشات والتنقل
 * - حلقة الأحداث
 * - الرسم
 * - الموارد
 * 
 * Main application structure. Manages:
 * - Screens and navigation
 * - Event loop
 * - Rendering
 * - Resources
 */
typedef struct SadApp {
    /* --- النوافذ / Windows --- */
    int32_t screen_width;           /**< عرض الشاشة / Screen width */
    int32_t screen_height;          /**< ارتفاع الشاشة / Screen height */
    float screen_density;           /**< كثافة الشاشة (DPI/160) / Screen density */
    
    /* --- الشاشات / Screens --- */
    SadScreen* screens[SAD_UI_MAX_SCREENS]; /**< الشاشات / Screens */
    int32_t screen_count;           /**< عدد الشاشات / Screen count */
    SadScreen* current_screen;      /**< الشاشة الحالية / Current screen */
    SadScreen* modal_screen;        /**< شاشة منبثقة (إن وجدت) / Modal screen (if any) */
    
    /* --- التركيز / Focus --- */
    SadWidget* focused_widget;      /**< الواجهة المُركّز عليها / Focused widget */
    
    /* --- الحالة / State --- */
    bool is_running;                /**< هل التطبيق يعمل؟ / Is app running? */
    bool is_paused;                 /**< هل التطبيق موقف؟ / Is app paused? */
    uint64_t frame_count;           /**< عدد الإطارات / Frame count */
    float delta_time;               /**< الوقت منذ آخر إطار / Time since last frame */
    
    /* --- الموارد / Resources --- */
    void* font_atlas;               /**< أطلس الخطوط / Font atlas */
    void* texture_cache;            /**< ذاكرة التخزين المؤقت للأنسجة / Texture cache */
    
    /* --- الموضوع / Theme --- */
    SadColor theme_primary;         /**< اللون الأساسي / Primary color */
    SadColor theme_secondary;       /**< اللون الثانوي / Secondary color */
    SadColor theme_background;      /**< لون الخلفية / Background color */
    SadColor theme_surface;         /**< لون السطح / Surface color */
    SadColor theme_error;           /**< لون الخطأ / Error color */
    bool is_dark_mode;              /**< هل الوضع المظلم؟ / Is dark mode? */
    
    /* --- NativeActivity --- */
    void* native_activity;          /**< مؤشر NativeActivity / NativeActivity pointer */
    void* native_window;            /**< مؤشر ANativeWindow / ANativeWindow pointer */
    void* egl_display;              /**< مؤشر EGLDisplay / EGLDisplay pointer */
    void* egl_surface;              /**< مؤشر EGLSurface / EGLSurface pointer */
    void* egl_context;              /**< مؤشر EGLContext / EGLContext pointer */
    
} SadApp;


/* =========================================================================
 * دوال التطبيق / Application Functions
 * ========================================================================= */

/**
 * إنشاء تطبيق جديد
 * Create a new application
 * 
 * @return مؤشر التطبيق أو NULL عند الفشل / App pointer or NULL on failure
 */
SadApp* sad_app_create(void);

/**
 * تدمير التطبيق
 * Destroy the application
 * 
 * @param app التطبيق / The app
 */
void sad_app_destroy(SadApp* app);

/**
 * تهيئة التطبيق مع NativeActivity
 * Initialize app with NativeActivity
 * 
 * @param app التطبيق / The app
 * @param activity مؤشر NativeActivity / NativeActivity pointer
 * @return true عند النجاح / true on success
 */
bool sad_app_init_native(SadApp* app, void* activity);

/**
 * تشغيل حلقة التطبيق
 * Run the application loop
 * 
 * هذه الدالة لا تعود حتى ينتهي التطبيق
 * This function doesn't return until the app ends
 * 
 * @param app التطبيق / The app
 */
void sad_app_run(SadApp* app);

/**
 * طلب إنهاء التطبيق
 * Request app termination
 * 
 * @param app التطبيق / The app
 */
void sad_app_quit(SadApp* app);

/**
 * إضافة شاشة للتطبيق
 * Add a screen to the app
 * 
 * @param app التطبيق / The app
 * @param screen الشاشة / The screen
 * @return true عند النجاح / true on success
 */
bool sad_app_add_screen(SadApp* app, SadScreen* screen);

/**
 * الانتقال إلى شاشة
 * Navigate to a screen
 * 
 * @param app التطبيق / The app
 * @param screen_name اسم الشاشة / Screen name
 * @return true عند النجاح / true on success
 */
bool sad_app_navigate_to(SadApp* app, const char* screen_name);

/**
 * العودة للشاشة السابقة
 * Go back to previous screen
 * 
 * @param app التطبيق / The app
 * @return true إذا كان هناك شاشة للعودة إليها / true if there was a screen to go back to
 */
bool sad_app_go_back(SadApp* app);

/**
 * عرض شاشة منبثقة (modal)
 * Show a modal screen
 * 
 * @param app التطبيق / The app
 * @param screen الشاشة المنبثقة / The modal screen
 */
void sad_app_show_modal(SadApp* app, SadScreen* screen);

/**
 * إغلاق الشاشة المنبثقة
 * Dismiss the modal screen
 * 
 * @param app التطبيق / The app
 */
void sad_app_dismiss_modal(SadApp* app);


/* =========================================================================
 * دوال إنشاء الواجهات / Widget Creation Functions
 * ========================================================================= */

/**
 * إنشاء واجهة جديدة
 * Create a new widget
 * 
 * @param type نوع الواجهة / Widget type
 * @return مؤشر الواجهة أو NULL عند الفشل / Widget pointer or NULL on failure
 */
SadWidget* sad_widget_create(SadWidgetType type);

/**
 * تدمير واجهة (مع أطفالها)
 * Destroy a widget (including children)
 * 
 * @param widget الواجهة / The widget
 */
void sad_widget_destroy(SadWidget* widget);

/**
 * إضافة طفل لواجهة
 * Add a child to a widget
 * 
 * @param parent الأب / Parent widget
 * @param child الطفل / Child widget
 * @return true عند النجاح / true on success
 */
bool sad_widget_add_child(SadWidget* parent, SadWidget* child);

/**
 * إزالة طفل من واجهة
 * Remove a child from a widget
 * 
 * @param parent الأب / Parent widget
 * @param child الطفل / Child widget
 */
void sad_widget_remove_child(SadWidget* parent, SadWidget* child);

/**
 * إزالة كل الأطفال
 * Remove all children
 * 
 * @param parent الأب / Parent widget
 */
void sad_widget_clear_children(SadWidget* parent);


/* =========================================================================
 * دوال بناء الواجهات السريعة / Quick Widget Builders
 * ========================================================================= */

/**
 * إنشاء نص
 * Create a text widget
 * 
 * @param text النص / Text content
 * @param font_size حجم الخط / Font size (0 for default)
 * @param color اللون / Color (NULL for default)
 * @return الواجهة / The widget
 */
SadWidget* sad_text(const char* text, float font_size, const SadColor* color);

/**
 * إنشاء زر
 * Create a button widget
 * 
 * @param text نص الزر / Button text
 * @param on_click دالة النقر / Click callback
 * @param user_data بيانات المستخدم / User data
 * @return الواجهة / The widget
 */
SadWidget* sad_button(const char* text, SadOnClickCallback on_click, void* user_data);

/**
 * إنشاء حقل إدخال
 * Create a text input widget
 * 
 * @param placeholder نص التلميح / Placeholder text
 * @param on_change دالة التغيير / Change callback
 * @param user_data بيانات المستخدم / User data
 * @return الواجهة / The widget
 */
SadWidget* sad_input(const char* placeholder, SadOnTextChangeCallback on_change, void* user_data);

/**
 * إنشاء عمود (تخطيط عمودي)
 * Create a column (vertical layout)
 * 
 * @param main_align محاذاة المحور الرئيسي / Main axis alignment
 * @param cross_align محاذاة المحور المتقاطع / Cross axis alignment
 * @param spacing المسافة بين الأطفال / Spacing between children
 * @return الواجهة / The widget
 */
SadWidget* sad_column(SadMainAxisAlign main_align, SadCrossAxisAlign cross_align, float spacing);

/**
 * إنشاء صف (تخطيط أفقي)
 * Create a row (horizontal layout)
 * 
 * @param main_align محاذاة المحور الرئيسي / Main axis alignment
 * @param cross_align محاذاة المحور المتقاطع / Cross axis alignment
 * @param spacing المسافة بين الأطفال / Spacing between children
 * @return الواجهة / The widget
 */
SadWidget* sad_row(SadMainAxisAlign main_align, SadCrossAxisAlign cross_align, float spacing);

/**
 * إنشاء مكدس (تراكب)
 * Create a stack (overlay layout)
 * 
 * @return الواجهة / The widget
 */
SadWidget* sad_stack(void);

/**
 * إنشاء حاوية قابلة للتمرير
 * Create a scrollable container
 * 
 * @param horizontal تمرير أفقي / Horizontal scroll
 * @param vertical تمرير عمودي / Vertical scroll
 * @return الواجهة / The widget
 */
SadWidget* sad_scroll(bool horizontal, bool vertical);

/**
 * إنشاء قائمة
 * Create a list widget
 * 
 * @param item_height ارتفاع العنصر / Item height
 * @return الواجهة / The widget
 */
SadWidget* sad_list(float item_height);

/**
 * إنشاء بطاقة Material
 * Create a Material card
 * 
 * @param elevation الارتفاع (للظل) / Elevation (for shadow)
 * @return الواجهة / The widget
 */
SadWidget* sad_card(float elevation);

/**
 * إنشاء صورة
 * Create an image widget
 * 
 * @param path مسار الصورة / Image path
 * @return الواجهة / The widget
 */
SadWidget* sad_image(const char* path);

/**
 * إنشاء مربع اختيار
 * Create a checkbox widget
 * 
 * @param label الملصق / Label text
 * @param checked حالة التحديد / Checked state
 * @param on_change دالة التغيير / Change callback
 * @param user_data بيانات المستخدم / User data
 * @return الواجهة / The widget
 */
SadWidget* sad_checkbox(const char* label, bool checked, SadOnValueChangeCallback on_change, void* user_data);

/**
 * إنشاء مفتاح تبديل
 * Create a switch widget
 * 
 * @param is_on حالة التشغيل / On state
 * @param on_change دالة التغيير / Change callback
 * @param user_data بيانات المستخدم / User data
 * @return الواجهة / The widget
 */
SadWidget* sad_switch(bool is_on, SadOnValueChangeCallback on_change, void* user_data);

/**
 * إنشاء منزلق
 * Create a slider widget
 * 
 * @param min الحد الأدنى / Minimum value
 * @param max الحد الأقصى / Maximum value
 * @param value القيمة الحالية / Current value
 * @param on_change دالة التغيير / Change callback
 * @param user_data بيانات المستخدم / User data
 * @return الواجهة / The widget
 */
SadWidget* sad_slider(float min, float max, float value, SadOnValueChangeCallback on_change, void* user_data);


/* =========================================================================
 * دوال تعديل الواجهات / Widget Modification Functions
 * ========================================================================= */

/**
 * تعيين النص
 * Set widget text
 */
void sad_widget_set_text(SadWidget* widget, const char* text);

/**
 * تعيين حجم الخط
 * Set font size
 */
void sad_widget_set_font_size(SadWidget* widget, float size);

/**
 * تعيين لون الخلفية
 * Set background color
 */
void sad_widget_set_background(SadWidget* widget, SadColor color);

/**
 * تعيين لون المقدمة
 * Set foreground color
 */
void sad_widget_set_foreground(SadWidget* widget, SadColor color);

/**
 * تعيين الحدود
 * Set border
 */
void sad_widget_set_border(SadWidget* widget, SadColor color, float width);

/**
 * تعيين نصف قطر الزوايا
 * Set corner radius
 */
void sad_widget_set_corner_radius(SadWidget* widget, float radius);

/**
 * تعيين الهامش
 * Set margin
 */
void sad_widget_set_margin(SadWidget* widget, SadEdgeInsets margin);

/**
 * تعيين البطانة
 * Set padding
 */
void sad_widget_set_padding(SadWidget* widget, SadEdgeInsets padding);

/**
 * تعيين الحجم
 * Set size
 */
void sad_widget_set_size(SadWidget* widget, float width, float height);

/**
 * تعيين الشفافية
 * Set opacity
 */
void sad_widget_set_opacity(SadWidget* widget, float opacity);

/**
 * تعيين قابلية التفاعل
 * Set enabled state
 */
void sad_widget_set_enabled(SadWidget* widget, bool enabled);

/**
 * تعيين الرؤية
 * Set visibility
 */
void sad_widget_set_visible(SadWidget* widget, bool visible);

/**
 * تعيين معامل المرونة
 * Set flex factor (for flexible layouts)
 */
void sad_widget_set_flex(SadWidget* widget, float flex);


/* =========================================================================
 * دوال الشاشة / Screen Functions
 * ========================================================================= */

/**
 * إنشاء شاشة جديدة
 * Create a new screen
 * 
 * @param name اسم الشاشة / Screen name
 * @return مؤشر الشاشة أو NULL عند الفشل / Screen pointer or NULL on failure
 */
SadScreen* sad_screen_create(const char* name);

/**
 * تدمير شاشة
 * Destroy a screen
 * 
 * @param screen الشاشة / The screen
 */
void sad_screen_destroy(SadScreen* screen);

/**
 * تعيين واجهة جذر الشاشة
 * Set screen's root widget
 * 
 * @param screen الشاشة / The screen
 * @param root الواجهة الجذر / Root widget
 */
void sad_screen_set_root(SadScreen* screen, SadWidget* root);


/* =========================================================================
 * دوال الرسم / Drawing Functions
 * ========================================================================= */

/**
 * تهيئة نظام الرسم
 * Initialize the rendering system
 * 
 * @param app التطبيق / The app
 * @return true عند النجاح / true on success
 */
bool sad_render_init(SadApp* app);

/**
 * تنظيف نظام الرسم
 * Cleanup the rendering system
 * 
 * @param app التطبيق / The app
 */
void sad_render_cleanup(SadApp* app);

/**
 * بدء إطار رسم جديد
 * Begin a new render frame
 * 
 * @param app التطبيق / The app
 */
void sad_render_begin_frame(SadApp* app);

/**
 * إنهاء إطار الرسم
 * End the render frame
 * 
 * @param app التطبيق / The app
 */
void sad_render_end_frame(SadApp* app);

/**
 * رسم واجهة
 * Render a widget
 * 
 * @param widget الواجهة / The widget
 */
void sad_render_widget(SadWidget* widget);


/* =========================================================================
 * دوال التخطيط / Layout Functions
 * ========================================================================= */

/**
 * حساب تخطيط واجهة
 * Calculate widget layout
 * 
 * @param widget الواجهة / The widget
 * @param available_width العرض المتاح / Available width
 * @param available_height الارتفاع المتاح / Available height
 */
void sad_layout_widget(SadWidget* widget, float available_width, float available_height);

/**
 * إعادة حساب التخطيط لكل الواجهات
 * Recalculate layout for all widgets
 * 
 * @param app التطبيق / The app
 */
void sad_layout_update(SadApp* app);


/* =========================================================================
 * دوال الأحداث / Event Functions
 * ========================================================================= */

/**
 * معالجة حدث لمس
 * Handle a touch event
 * 
 * @param app التطبيق / The app
 * @param event حدث اللمس / Touch event
 */
void sad_handle_touch(SadApp* app, SadTouchEvent* event);

/**
 * معالجة حدث لوحة المفاتيح
 * Handle a keyboard event
 * 
 * @param app التطبيق / The app
 * @param key_code رمز المفتاح / Key code
 * @param is_down هل ضُغط؟ / Is pressed down?
 */
void sad_handle_key(SadApp* app, int32_t key_code, bool is_down);


/* =========================================================================
 * دوال النصوص والخطوط / Text and Font Functions
 * ========================================================================= */

/**
 * تحميل خط
 * Load a font
 * 
 * @param app التطبيق / The app
 * @param path مسار الخط / Font path
 * @param size حجم الخط / Font size
 * @return معرّف الخط أو 0 عند الفشل / Font ID or 0 on failure
 */
uint32_t sad_font_load(SadApp* app, const char* path, float size);

/**
 * قياس حجم نص
 * Measure text size
 * 
 * @param text النص / Text
 * @param font_size حجم الخط / Font size
 * @param out_width عرض النص (مخرج) / Text width (output)
 * @param out_height ارتفاع النص (مخرج) / Text height (output)
 */
void sad_text_measure(const char* text, float font_size, float* out_width, float* out_height);


/* =========================================================================
 * دوال مساعدة / Utility Functions
 * ========================================================================= */

/**
 * الحصول على الوقت الحالي بالمللي ثانية
 * Get current time in milliseconds
 * 
 * @return الوقت بالمللي ثانية / Time in milliseconds
 */
int64_t sad_get_time_ms(void);

/**
 * كشف إذا كان النص عربي (RTL)
 * Detect if text is Arabic (RTL)
 * 
 * @param text النص / Text
 * @return true إذا كان النص عربي / true if text is Arabic
 */
bool sad_text_is_rtl(const char* text);

/**
 * البحث عن واجهة بالوسم
 * Find widget by tag
 * 
 * @param root الجذر / Root widget
 * @param tag الوسم / Tag
 * @return الواجهة أو NULL / Widget or NULL
 */
SadWidget* sad_widget_find_by_tag(SadWidget* root, const char* tag);


#ifdef __cplusplus
}
#endif

#endif /* SAD_ANDROID_UI_H */
