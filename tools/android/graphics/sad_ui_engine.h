/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  نظام سَد الرسومي الثوري لتطبيقات أندرويد
 *  SadUI Revolutionary Graphics Engine for Android
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 *  محرك رسومي احترافي يقدم:
 *  - رسم نصوص عربية بجودة عالية (RTL + Shaping)
 *  - مكونات واجهة حديثة (Material Design 3)
 *  - رسوم متحركة سلسة 60fps
 *  - نظام تخطيطات مرن (Flexbox)
 *  - دعم كامل للإيماءات
 *  - ثيمات ديناميكية
 * 
 *  الإصدار: 2.0
 *  المؤلف: فريق تطوير لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_ENGINE_H
#define SAD_UI_ENGINE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//  الثوابت والتعريفات
// ═══════════════════════════════════════════════════════════════════════════════

#define SADUI_VERSION_MAJOR 2
#define SADUI_VERSION_MINOR 0
#define SADUI_MAX_WIDGETS 1024
#define SADUI_MAX_ANIMATIONS 256
#define SADUI_MAX_TOUCH_POINTS 10
#define SADUI_FONT_ATLAS_SIZE 2048

// ═══════════════════════════════════════════════════════════════════════════════
//  أنواع البيانات الأساسية
// ═══════════════════════════════════════════════════════════════════════════════

typedef uint32_t SadColor;      // لون RGBA (0xRRGGBBAA)
typedef uint32_t SadWidgetId;   // معرّف العنصر
typedef uint32_t SadAnimId;     // معرّف الحركة
typedef int32_t  SadFontId;     // معرّف الخط
typedef int32_t  SadImageId;    // معرّف الصورة

/** اتجاه التمرير */
typedef enum {
    SAD_SCROLL_VERTICAL = 0,
    SAD_SCROLL_HORIZONTAL,
    SAD_SCROLL_BOTH
} SadScrollDirection;

// ألوان جاهزة
#define SAD_COLOR_TRANSPARENT   0x00000000
#define SAD_COLOR_BLACK         0x000000FF
#define SAD_COLOR_WHITE         0xFFFFFFFF
#define SAD_COLOR_RED           0xFF0000FF
#define SAD_COLOR_GREEN         0x00FF00FF
#define SAD_COLOR_BLUE          0x0066FFFF
#define SAD_COLOR_PRIMARY       0x6750A4FF  // Material 3 Primary
#define SAD_COLOR_SECONDARY     0x625B71FF
#define SAD_COLOR_SURFACE       0xFFFBFEFF
#define SAD_COLOR_ERROR         0xB3261EFF

// ═══════════════════════════════════════════════════════════════════════════════
//  هياكل البيانات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * نقطة ثنائية الأبعاد
 */
typedef struct {
    float x, y;
} SadVec2;

/**
 * مستطيل
 */
typedef struct {
    float x, y, width, height;
} SadRect;

/**
 * حواف (للهوامش والحشو)
 */
typedef struct {
    float top, right, bottom, left;
} SadEdges;

/**
 * زوايا مستديرة
 */
typedef struct {
    float topLeft, topRight, bottomRight, bottomLeft;
} SadCorners;

/**
 * تدرج لوني
 */
typedef struct {
    SadColor start;
    SadColor end;
    float angle;    // زاوية بالدرجات (0=أفقي، 90=عمودي)
} SadGradient;

/**
 * ظل
 */
typedef struct {
    float offsetX, offsetY;
    float blur;
    float spread;
    SadColor color;
} SadShadow;

// ═══════════════════════════════════════════════════════════════════════════════
//  أنواع المكونات
// ═══════════════════════════════════════════════════════════════════════════════

typedef enum {
    // الحاويات
    SAD_WIDGET_CONTAINER,      // حاوية عامة
    SAD_WIDGET_COLUMN,         // عمود (ترتيب عمودي)
    SAD_WIDGET_ROW,            // صف (ترتيب أفقي)
    SAD_WIDGET_STACK,          // طبقات متراكبة
    SAD_WIDGET_SCROLL,         // قابل للتمرير
    SAD_WIDGET_CARD,           // بطاقة بظل
    
    // النصوص
    SAD_WIDGET_TEXT,           // نص
    SAD_WIDGET_LABEL,          // تسمية
    SAD_WIDGET_HEADING,        // عنوان
    SAD_WIDGET_PARAGRAPH,      // فقرة
    
    // الأزرار
    SAD_WIDGET_BUTTON,         // زر
    SAD_WIDGET_ICON_BUTTON,    // زر أيقونة
    SAD_WIDGET_FAB,            // زر عائم (FAB)
    SAD_WIDGET_OUTLINED_BUTTON,// زر بإطار
    
    // الإدخال
    SAD_WIDGET_TEXT_FIELD,     // حقل نصي
    SAD_WIDGET_PASSWORD,       // حقل كلمة مرور
    SAD_WIDGET_TEXTAREA,       // منطقة نص متعددة
    SAD_WIDGET_CHECKBOX,       // مربع اختيار
    SAD_WIDGET_SWITCH,         // مفتاح تبديل
    SAD_WIDGET_RADIO,          // زر راديو
    SAD_WIDGET_SLIDER,         // شريط منزلق
    SAD_WIDGET_DROPDOWN,       // قائمة منسدلة
    
    // العرض
    SAD_WIDGET_IMAGE,          // صورة
    SAD_WIDGET_ICON,           // أيقونة
    SAD_WIDGET_AVATAR,         // صورة رمزية
    SAD_WIDGET_BADGE,          // شارة
    SAD_WIDGET_CHIP,           // رقاقة
    SAD_WIDGET_PROGRESS,       // شريط تقدم
    SAD_WIDGET_CIRCULAR_PROGRESS, // تقدم دائري
    SAD_WIDGET_DIVIDER,        // خط فاصل
    SAD_WIDGET_SPACER,         // فراغ
    
    // الحوارات
    SAD_WIDGET_DIALOG,         // حوار
    SAD_WIDGET_BOTTOM_SHEET,   // ورقة سفلية
    SAD_WIDGET_SNACKBAR,       // شريط إشعار
    SAD_WIDGET_TOOLTIP,        // تلميح
    
    // التنقل
    SAD_WIDGET_APPBAR,         // شريط التطبيق
    SAD_WIDGET_BOTTOM_NAV,     // تنقل سفلي
    SAD_WIDGET_DRAWER,         // درج جانبي
    SAD_WIDGET_TABS,           // علامات تبويب
    
    // الرسم والتطبيقات المتقدمة
    SAD_WIDGET_CANVAS,         // لوحة رسم
    SAD_WIDGET_CHART,          // مخطط بياني
    
    // 40+ مكون إضافي (متقدمات وتخطيطات وشاشات تفاعلية)
    SAD_WIDGET_GRID,                 // شبكة (Grid)
    SAD_WIDGET_LIST_VIEW,            // قائمة متقدمة (ListView)
    SAD_WIDGET_MAP,                  // خريطة تفاعلية (Map)
    SAD_WIDGET_VIDEO_PLAYER,         // مشغل فيديو
    SAD_WIDGET_AUDIO_PLAYER,         // مشغل صوت
    SAD_WIDGET_WEB_VIEW,             // متصفح ويب مضمن
    SAD_WIDGET_CAMERA_PREVIEW,       // معاينة الكاميرا
    SAD_WIDGET_PDF_VIEWER,           // عارض ملفات PDF
    SAD_WIDGET_DATE_PICKER,          // منتقي التاريخ
    SAD_WIDGET_TIME_PICKER,          // منتقي الوقت
    SAD_WIDGET_CALENDAR,             // تقويم شامل
    SAD_WIDGET_COLOR_PICKER,         // منتقي الألوان
    SAD_WIDGET_ACCORDION,            // طيات متداخلة (Accordion)
    SAD_WIDGET_STEPPER,              // متتبع خطوات (Stepper)
    SAD_WIDGET_BREADCRUMB,           // فتات الخبز للتنقل (Breadcrumb)
    SAD_WIDGET_CAROUSEL,             // دائرة عرض الصور/المحتوى
    SAD_WIDGET_RATING_BAR,           // شريط تقييم النجوم
    SAD_WIDGET_KNOB,                 // مقبض دوار (Knob)
    SAD_WIDGET_GAUGE,                // مؤشر قياس (Gauge)
    SAD_WIDGET_TOGGLE_BUTTON_GROUP,  // مجموعة أزرار التبديل
    SAD_WIDGET_SEGMENTED_CONTROL,    // تحكم مجزأ (Segmented Control)
    SAD_WIDGET_SLIDING_PANEL,        // لوحة منزلقة
    SAD_WIDGET_SPLIT_PANE,           // لوحة مقسومة قابلة للسحب
    SAD_WIDGET_TREE_VIEW,            // عرض شجري (Tree View)
    SAD_WIDGET_TABLE,                // جدول بسيط
    SAD_WIDGET_DATA_GRID,            // شبكة بيانات متقدمة (Data Grid)
    SAD_WIDGET_RICH_TEXT,            // نص غني بالتنسيقات
    SAD_WIDGET_MARKDOWN_VIEWER,      // عارض ماركداون (Markdown)
    SAD_WIDGET_CODE_EDITOR,          // محرر نصوص برمجية مع تلوين
    SAD_WIDGET_SIGNATURE_PAD,        // لوحة توقيع إلكتروني
    SAD_WIDGET_QR_SCANNER,           // قارئ رموز QR
    SAD_WIDGET_BARCODE_SCANNER,      // قارئ باركود
    SAD_WIDGET_LOTTIE_ANIMATION,     // رسوم Lottie المتحركة
    SAD_WIDGET_VOICE_RECORDER,       // مسجل صوت مع واجهة
    SAD_WIDGET_WAVEFORM,             // موجة صوتية (Waveform)
    SAD_WIDGET_RADAR_CHART,          // مخطط راداري
    SAD_WIDGET_PIE_CHART,            // مخطط دائري
    SAD_WIDGET_HEATMAP,              // خريطة حرارية (Heatmap)
    SAD_WIDGET_PULL_TO_REFRESH,      // السحب للتحديث
    SAD_WIDGET_SPEED_DIAL,           // أزرار سريعة الطلب متفرعة
    SAD_WIDGET_CONTEXT_MENU,         // قائمة سياقية (كليك يمين/لمس مطول)
    SAD_WIDGET_EXPANDABLE_LIST,      // قائمة قابلة للتوسيع

    SAD_WIDGET_COUNT
} SadWidgetType;

// ═══════════════════════════════════════════════════════════════════════════════
//  خصائص التخطيط (Flexbox-style)
// ═══════════════════════════════════════════════════════════════════════════════

typedef enum {
    SAD_ALIGN_START,           // بداية
    SAD_ALIGN_CENTER,          // وسط
    SAD_ALIGN_END,             // نهاية
    SAD_ALIGN_STRETCH,         // تمدد
    SAD_ALIGN_SPACE_BETWEEN,   // توزيع متساوي
    SAD_ALIGN_SPACE_AROUND,    // توزيع مع هوامش
    SAD_ALIGN_SPACE_EVENLY     // توزيع متساوي تماماً
} SadAlignment;

typedef enum {
    SAD_SIZE_AUTO,             // تلقائي (يتناسب مع المحتوى)
    SAD_SIZE_FIXED,            // ثابت (بالبكسل)
    SAD_SIZE_PERCENT,          // نسبة مئوية
    SAD_SIZE_FILL,             // ملء المساحة المتاحة
    SAD_SIZE_WRAP              // التفاف حول المحتوى
} SadSizeMode;

typedef struct {
    SadSizeMode mode;
    float value;
} SadSize;

typedef struct {
    // المحاذاة
    SadAlignment mainAxis;      // المحور الرئيسي
    SadAlignment crossAxis;     // المحور المتقاطع
    
    // الحجم
    SadSize width;
    SadSize height;
    SadSize minWidth;
    SadSize minHeight;
    SadSize maxWidth;
    SadSize maxHeight;
    
    // الهوامش والحشو
    SadEdges margin;
    SadEdges padding;
    
    // المرونة
    float flex;                 // معامل المرونة (0 = لا مرونة)
    
    // الفراغ بين العناصر
    float gap;
    
    // الاتجاه (للنصوص العربية)
    bool rtl;                   // من اليمين لليسار
    
    // الموقع
    bool absolute;              // موقع مطلق
    float posX, posY;           // إذا كان الموقع مطلقاً
    
} SadLayout;

// ═══════════════════════════════════════════════════════════════════════════════
//  نمط المكون
// ═══════════════════════════════════════════════════════════════════════════════

typedef struct {
    // الخلفية
    SadColor backgroundColor;
    SadGradient* gradient;       // NULL = لا تدرج
    
    // الحدود
    SadColor borderColor;
    float borderWidth;
    SadCorners borderRadius;
    
    // الظل
    SadShadow shadow;
    bool hasShadow;
    
    // النص
    SadColor textColor;
    float fontSize;
    int fontWeight;              // 100-900
    bool italic;
    bool underline;
    SadAlignment textAlign;
    float lineHeight;
    float letterSpacing;
    
    // الشفافية
    float opacity;
    
    // التحويل
    float scaleX, scaleY;
    float rotation;              // بالدرجات
    float translateX, translateY;
    
} SadStyle;

// ═══════════════════════════════════════════════════════════════════════════════
//  الرسوم المتحركة
// ═══════════════════════════════════════════════════════════════════════════════

typedef enum {
    SAD_EASE_LINEAR,
    SAD_EASE_IN,
    SAD_EASE_OUT,
    SAD_EASE_IN_OUT,
    SAD_EASE_BOUNCE,
    SAD_EASE_ELASTIC,
    SAD_EASE_BACK,
    SAD_EASE_SPRING
} SadEasing;

typedef enum {
    SAD_ANIM_FADE,              // تلاشي
    SAD_ANIM_SLIDE_UP,          // انزلاق للأعلى
    SAD_ANIM_SLIDE_DOWN,        // انزلاق للأسفل
    SAD_ANIM_SLIDE_LEFT,        // انزلاق لليسار
    SAD_ANIM_SLIDE_RIGHT,       // انزلاق لليمين
    SAD_ANIM_SCALE,             // تكبير/تصغير
    SAD_ANIM_ROTATE,            // دوران
    SAD_ANIM_BOUNCE,            // ارتداد
    SAD_ANIM_SHAKE,             // اهتزاز
    SAD_ANIM_PULSE,             // نبض
    SAD_ANIM_CUSTOM             // مخصص
} SadAnimationType;

typedef struct {
    SadAnimationType type;
    float duration;             // بالمللي ثانية
    float delay;                // تأخير البدء
    SadEasing easing;
    bool loop;                  // تكرار
    bool reverse;               // عكس الاتجاه
    int repeatCount;            // عدد التكرارات (-1 = لا نهائي)
    
    // قيم التحريك (للنوع المخصص)
    float fromValue;
    float toValue;
    
} SadAnimation;

// ═══════════════════════════════════════════════════════════════════════════════
//  أحداث اللمس والإيماءات
// ═══════════════════════════════════════════════════════════════════════════════

typedef enum {
    SAD_TOUCH_DOWN,             // بدء اللمس
    SAD_TOUCH_MOVE,             // تحريك
    SAD_TOUCH_UP,               // رفع الإصبع
    SAD_TOUCH_CANCEL            // إلغاء
} SadTouchAction;

typedef enum {
    SAD_GESTURE_TAP,            // نقر
    SAD_GESTURE_DOUBLE_TAP,     // نقر مزدوج
    SAD_GESTURE_LONG_PRESS,     // ضغط مطول
    SAD_GESTURE_SWIPE_LEFT,     // سحب يسار
    SAD_GESTURE_SWIPE_RIGHT,    // سحب يمين
    SAD_GESTURE_SWIPE_UP,       // سحب أعلى
    SAD_GESTURE_SWIPE_DOWN,     // سحب أسفل
    SAD_GESTURE_PINCH,          // قرص (تكبير/تصغير)
    SAD_GESTURE_ROTATE          // دوران بإصبعين
} SadGesture;

typedef struct {
    int pointerId;
    float x, y;
    float pressure;
    SadTouchAction action;
    int64_t timestamp;
} SadTouchEvent;

// معالج الأحداث
typedef void (*SadEventHandler)(SadWidgetId widget, void* userData);
typedef void (*SadGestureHandler)(SadWidgetId widget, SadGesture gesture, void* userData);
typedef void (*SadTouchHandler)(SadWidgetId widget, SadTouchEvent* event, void* userData);
typedef void (*SadValueHandler)(SadWidgetId widget, float value, void* userData);
typedef void (*SadTextHandler)(SadWidgetId widget, const char* text, void* userData);

// ═══════════════════════════════════════════════════════════════════════════════
//  واجهة برمجة التطبيقات (API)
// ═══════════════════════════════════════════════════════════════════════════════

// ─────────────────────────────────────────────────────────────────────────────
//  التهيئة والإدارة
// ─────────────────────────────────────────────────────────────────────────────

/**
 * تهيئة المحرك الرسومي
 * @param window نافذة أندرويد الأصلية
 * @return true عند النجاح
 */
bool sadui_init(void* window);

/**
 * إنهاء المحرك وتحرير الموارد
 */
void sadui_shutdown(void);

/**
 * رسم إطار واحد
 * @param deltaTime الوقت منذ الإطار السابق (بالثواني)
 */
void sadui_render(float deltaTime);

/**
 * معالجة حدث لمس
 */
void sadui_handle_touch(SadTouchEvent* event);

/**
 * تعيين حجم الشاشة (عند التغيير)
 */
void sadui_set_screen_size(int width, int height);

/**
 * الحصول على حجم الشاشة
 */
void sadui_get_screen_size(int* width, int* height);

// ─────────────────────────────────────────────────────────────────────────────
//  إنشاء المكونات
// ─────────────────────────────────────────────────────────────────────────────

/**
 * إنشاء مكون جديد
 */
SadWidgetId sadui_create(SadWidgetType type);

/**
 * حذف مكون
 */
void sadui_destroy(SadWidgetId widget);

/**
 * إضافة مكون كابن لمكون آخر
 */
void sadui_add_child(SadWidgetId parent, SadWidgetId child);

/**
 * إزالة مكون من أبيه
 */
void sadui_remove_child(SadWidgetId parent, SadWidgetId child);

/**
 * تعيين المكون الجذر
 */
void sadui_set_root(SadWidgetId widget);

// ─────────────────────────────────────────────────────────────────────────────
//  تعيين الخصائص
// ─────────────────────────────────────────────────────────────────────────────

/**
 * تعيين النص
 */
void sadui_set_text(SadWidgetId widget, const char* text);

/**
 * الحصول على النص
 */
const char* sadui_get_text(SadWidgetId widget);

/**
 * تعيين التخطيط
 */
void sadui_set_layout(SadWidgetId widget, const SadLayout* layout);

/**
 * تعيين النمط
 */
void sadui_set_style(SadWidgetId widget, const SadStyle* style);

/**
 * تعيين قيمة (للمنزلق، التقدم، إلخ)
 */
void sadui_set_value(SadWidgetId widget, float value);

/**
 * الحصول على القيمة
 */
float sadui_get_value(SadWidgetId widget);

/**
 * تعيين حالة التفعيل
 */
void sadui_set_enabled(SadWidgetId widget, bool enabled);

/**
 * تعيين الرؤية
 */
void sadui_set_visible(SadWidgetId widget, bool visible);

/**
 * تعيين حالة التحديد (للمربعات والمفاتيح)
 */
void sadui_set_checked(SadWidgetId widget, bool checked);

/**
 * الحصول على حالة التحديد
 */
bool sadui_get_checked(SadWidgetId widget);

// ─────────────────────────────────────────────────────────────────────────────
//  معالجات الأحداث
// ─────────────────────────────────────────────────────────────────────────────

/**
 * تعيين معالج النقر
 */
void sadui_on_click(SadWidgetId widget, SadEventHandler handler, void* userData);

/**
 * تعيين معالج الإيماءات
 */
void sadui_on_gesture(SadWidgetId widget, SadGestureHandler handler, void* userData);

/**
 * تعيين معالج اللمس
 */
void sadui_on_touch(SadWidgetId widget, SadTouchHandler handler, void* userData);

/**
 * تعيين معالج تغيير القيمة
 */
void sadui_on_value_change(SadWidgetId widget, SadValueHandler handler, void* userData);

/**
 * تعيين معالج تغيير النص
 */
void sadui_on_text_change(SadWidgetId widget, SadTextHandler handler, void* userData);

// ─────────────────────────────────────────────────────────────────────────────
//  الرسوم المتحركة
// ─────────────────────────────────────────────────────────────────────────────

/**
 * بدء رسم متحرك
 */
SadAnimId sadui_animate(SadWidgetId widget, const SadAnimation* anim);

/**
 * إيقاف رسم متحرك
 */
void sadui_stop_animation(SadAnimId anim);

/**
 * إيقاف جميع الرسوم المتحركة لمكون
 */
void sadui_stop_all_animations(SadWidgetId widget);

// ─────────────────────────────────────────────────────────────────────────────
//  دوال مساعدة للرسوم المتحركة
// ─────────────────────────────────────────────────────────────────────────────

/** رسم متحرك: ظهور تدريجي */
static inline SadAnimId sadui_fade_in(SadWidgetId widget, float durationMs) {
    SadAnimation anim = {0};
    anim.type = SAD_ANIM_FADE;
    anim.duration = durationMs;
    anim.easing = SAD_EASE_OUT;
    return sadui_animate(widget, &anim);
}

/** رسم متحرك: اختفاء تدريجي */
static inline SadAnimId sadui_fade_out(SadWidgetId widget, float durationMs) {
    SadAnimation anim = {0};
    anim.type = SAD_ANIM_FADE;
    anim.duration = durationMs;
    anim.easing = SAD_EASE_IN;
    anim.reverse = true;
    return sadui_animate(widget, &anim);
}

/** رسم متحرك: انزلاق للأعلى */
static inline SadAnimId sadui_slide_up(SadWidgetId widget, float durationMs) {
    SadAnimation anim = {0};
    anim.type = SAD_ANIM_SLIDE_UP;
    anim.duration = durationMs;
    anim.easing = SAD_EASE_OUT;
    return sadui_animate(widget, &anim);
}

/** رسم متحرك: انزلاق للأسفل */
static inline SadAnimId sadui_slide_down(SadWidgetId widget, float durationMs) {
    SadAnimation anim = {0};
    anim.type = SAD_ANIM_SLIDE_DOWN;
    anim.duration = durationMs;
    anim.easing = SAD_EASE_OUT;
    return sadui_animate(widget, &anim);
}

/** رسم متحرك: ارتداد */
static inline SadAnimId sadui_bounce(SadWidgetId widget, float durationMs) {
    SadAnimation anim = {0};
    anim.type = SAD_ANIM_BOUNCE;
    anim.duration = durationMs;
    anim.easing = SAD_EASE_BOUNCE;
    return sadui_animate(widget, &anim);
}

/** رسم متحرك: اهتزاز */
static inline SadAnimId sadui_shake(SadWidgetId widget, float durationMs) {
    SadAnimation anim = {0};
    anim.type = SAD_ANIM_SHAKE;
    anim.duration = durationMs;
    anim.easing = SAD_EASE_LINEAR;
    return sadui_animate(widget, &anim);
}

/** رسم متحرك: نبض */
static inline SadAnimId sadui_pulse(SadWidgetId widget, float durationMs) {
    SadAnimation anim = {0};
    anim.type = SAD_ANIM_PULSE;
    anim.duration = durationMs;
    anim.easing = SAD_EASE_OUT;
    anim.loop = true;
    anim.repeatCount = -1;
    return sadui_animate(widget, &anim);
}

// ─────────────────────────────────────────────────────────────────────────────
//  الثيمات
// ─────────────────────────────────────────────────────────────────────────────

typedef struct {
    SadColor primary;
    SadColor primaryVariant;
    SadColor secondary;
    SadColor secondaryVariant;
    SadColor background;
    SadColor surface;
    SadColor error;
    SadColor onPrimary;
    SadColor onSecondary;
    SadColor onBackground;
    SadColor onSurface;
    SadColor onError;
    
    float cornerRadiusSmall;
    float cornerRadiusMedium;
    float cornerRadiusLarge;
    
    float elevationSmall;
    float elevationMedium;
    float elevationLarge;
    
    float fontSizeSmall;
    float fontSizeMedium;
    float fontSizeLarge;
    float fontSizeHeading;
    
    bool rtl;                   // اتجاه النص من اليمين لليسار
} SadTheme;

/**
 * تعيين الثيم العام
 */
void sadui_set_theme(const SadTheme* theme);

/**
 * الحصول على الثيم الحالي
 */
const SadTheme* sadui_get_theme(void);

/**
 * ثيم فاتح افتراضي
 */
const SadTheme* sadui_theme_light(void);

/**
 * ثيم داكن افتراضي
 */
const SadTheme* sadui_theme_dark(void);

// ─────────────────────────────────────────────────────────────────────────────
//  الرسم المباشر (Canvas)
// ─────────────────────────────────────────────────────────────────────────────

typedef void* SadCanvas;

/**
 * الحصول على سياق الرسم من مكون Canvas
 */
SadCanvas sadui_get_canvas(SadWidgetId widget);

/**
 * رسم مستطيل
 */
void sadcanvas_draw_rect(SadCanvas ctx, SadRect rect, SadColor color);

/**
 * رسم مستطيل بزوايا مستديرة
 */
void sadcanvas_draw_rounded_rect(SadCanvas ctx, SadRect rect, float radius, SadColor color);

/**
 * رسم دائرة
 */
void sadcanvas_draw_circle(SadCanvas ctx, float x, float y, float radius, SadColor color);

/**
 * رسم قوس
 */
void sadcanvas_draw_arc(SadCanvas ctx, float x, float y, float radius, 
                        float startAngle, float endAngle, SadColor color);

/**
 * رسم خط
 */
void sadcanvas_draw_line(SadCanvas ctx, float x1, float y1, float x2, float y2,
                         float thickness, SadColor color);

/**
 * رسم مسار متعدد النقاط
 */
void sadcanvas_draw_path(SadCanvas ctx, SadVec2* points, int count,
                         float thickness, SadColor color, bool closed);

/**
 * رسم نص
 */
void sadcanvas_draw_text(SadCanvas ctx, const char* text, float x, float y,
                         float fontSize, SadColor color);

/**
 * رسم صورة
 */
void sadcanvas_draw_image(SadCanvas ctx, int imageId, SadRect destRect);

/**
 * رسم تدرج
 */
void sadcanvas_draw_gradient(SadCanvas ctx, SadRect rect, SadGradient gradient);

// ─────────────────────────────────────────────────────────────────────────────
//  إدارة الموارد
// ─────────────────────────────────────────────────────────────────────────────

/**
 * تحميل خط
 * @param path مسار ملف الخط (TTF)
 * @return معرّف الخط أو -1 عند الفشل
 */
SadFontId sadui_load_font(const char* path, float size);

/**
 * تحميل صورة
 * @param path مسار ملف الصورة
 * @return معرّف الصورة أو -1 عند الفشل
 */
SadImageId sadui_load_image(const char* path);

/**
 * تحميل صورة من الذاكرة
 */
SadImageId sadui_load_image_memory(const uint8_t* data, int size);

/**
 * تحرير خط
 */
void sadui_unload_font(SadFontId fontId);

/**
 * تحرير صورة
 */
void sadui_unload_image(SadImageId imageId);

/**
 * الحصول على أبعاد صورة محمّلة
 */
void sadui_get_image_size(SadImageId imageId, int* width, int* height);

// ─────────────────────────────────────────────────────────────────────────────
//  دوال مساعدة
// ─────────────────────────────────────────────────────────────────────────────

/**
 * إنشاء لون من قيم RGBA
 */
static inline SadColor sadui_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | a;
}

/**
 * إنشاء لون من قيمة سداسية
 */
static inline SadColor sadui_hex(uint32_t hex) {
    return (hex << 8) | 0xFF;
}

/**
 * إنشاء حجم ثابت
 */
static inline SadSize sadui_fixed(float value) {
    return (SadSize){SAD_SIZE_FIXED, value};
}

/**
 * إنشاء حجم بنسبة مئوية
 */
static inline SadSize sadui_percent(float percent) {
    return (SadSize){SAD_SIZE_PERCENT, percent};
}

/**
 * حجم تلقائي
 */
static inline SadSize sadui_auto(void) {
    return (SadSize){SAD_SIZE_AUTO, 0};
}

/**
 * ملء المساحة
 */
static inline SadSize sadui_fill(void) {
    return (SadSize){SAD_SIZE_FILL, 0};
}

/**
 * إنشاء حواف متساوية
 */
static inline SadEdges sadui_edges(float all) {
    return (SadEdges){all, all, all, all};
}

/**
 * إنشاء زوايا متساوية
 */
static inline SadCorners sadui_corners(float all) {
    return (SadCorners){all, all, all, all};
}

// ─────────────────────────────────────────────────────────────────────────────
//  دوال إنشاء مكونات سريعة (Convenience Widgets)
// ─────────────────────────────────────────────────────────────────────────────

// ─────────────────────────────────────────────────────────────────────────────
//  تحسين الأداء — نظام Dirty Flags
// ─────────────────────────────────────────────────────────────────────────────

/**
 * تحديد أن الواجهة تحتاج إعادة رسم
 * استدعِ هذه عند تغيير أي عنصر مرئي
 */
void sadui_invalidate(void);

/**
 * تحديد أن عنصر معين يحتاج إعادة رسم
 */
void sadui_invalidate_widget(SadWidgetId widget);

/**
 * تحديد أن التخطيط يحتاج إعادة حساب
 * استدعِ هذه عند تغيير أحجام أو مواقع
 */
void sadui_invalidate_layout(void);

/**
 * التحقق من الحاجة لإعادة الرسم
 * @return true إذا كانت الواجهة بحاجة لإعادة رسم
 */
bool sadui_needs_redraw(void);

/**
 * التحقق من الحاجة لإعادة حساب التخطيط
 */
bool sadui_needs_layout(void);

/**
 * الحصول على عدد الإطارات في الثانية (FPS)
 */
int sadui_get_fps(void);

/**
 * الحصول على عداد الإطارات الكلي
 */
uint32_t sadui_get_frame_count(void);

// ─────────────────────────────────────────────────────────────────────────────
//  دوال إنشاء مكونات سريعة (Convenience Widgets)
// ─────────────────────────────────────────────────────────────────────────────

SadWidgetId sadui_button(const char* text);
SadWidgetId sadui_text(const char* text);
SadWidgetId sadui_heading(const char* text);
SadWidgetId sadui_image(int imageId);
SadWidgetId sadui_card(void);
SadWidgetId sadui_column(void);
SadWidgetId sadui_row(void);
SadWidgetId sadui_container(void);
SadWidgetId sadui_scrollable(SadScrollDirection direction);
SadWidgetId sadui_textfield(const char* placeholder);
SadWidgetId sadui_checkbox(const char* label, bool checked);
SadWidgetId sadui_slider(float min, float max, float value);
SadWidgetId sadui_progress(float value);
SadWidgetId sadui_divider(void);
SadWidgetId sadui_chip(const char* text);
SadWidgetId sadui_fab(const char* text);
SadEdges    sadui_edges_vh(float vertical, float horizontal);
SadEdges    sadui_edges_ltrb(float left, float top, float right, float bottom);
SadCorners  sadui_corners_top(float top);
SadCorners  sadui_corners_bottom(float bottom);

#ifdef __cplusplus
}
#endif

#endif // SAD_UI_ENGINE_H
