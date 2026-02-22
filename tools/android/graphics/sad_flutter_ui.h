/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  نظام واجهات ص - Sad Flutter-like UI System
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 *  نظام مشابه لـ Flutter يتيح للمطورين إنشاء واجهات رسومية حقيقية
 *  تُرسم على الشاشة مع دعم كامل للتفاعل
 * 
 *  الميزات:
 *  ✓ شجرة Widgets تفاعلية
 *  ✓ نظام تخطيط مرن (Row, Column, Stack, Grid)
 *  ✓ رسم OpenGL ES 3.0 حقيقي
 *  ✓ دعم كامل للعربية (RTL)
 *  ✓ أحداث اللمس والإيماءات
 *  ✓ رسوم متحركة سلسة
 *  ✓ مكونات Material Design
 * 
 *  الإصدار: 1.0
 *  المؤلف: فريق تطوير لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_FLUTTER_UI_H
#define SAD_FLUTTER_UI_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//  الثوابت والتعريفات الأساسية
// ═══════════════════════════════════════════════════════════════════════════════

#define SADUI_MAX_CHILDREN      256
#define SADUI_MAX_TEXT_LENGTH   4096
#define SADUI_MAX_WIDGETS       1024
#define SADUI_ANIMATION_FPS     60

// ═══════════════════════════════════════════════════════════════════════════════
//  الألوان
// ═══════════════════════════════════════════════════════════════════════════════

typedef struct {
    uint8_t r, g, b, a;
} SadColor;

// ألوان مسبقة التعريف
#define SADCOLOR_WHITE       ((SadColor){255, 255, 255, 255})
#define SADCOLOR_BLACK       ((SadColor){0, 0, 0, 255})
#define SADCOLOR_RED         ((SadColor){244, 67, 54, 255})
#define SADCOLOR_GREEN       ((SadColor){76, 175, 80, 255})
#define SADCOLOR_BLUE        ((SadColor){33, 150, 243, 255})
#define SADCOLOR_YELLOW      ((SadColor){255, 235, 59, 255})
#define SADCOLOR_ORANGE      ((SadColor){255, 152, 0, 255})
#define SADCOLOR_PURPLE      ((SadColor){156, 39, 176, 255})
#define SADCOLOR_CYAN        ((SadColor){0, 188, 212, 255})
#define SADCOLOR_GREY        ((SadColor){158, 158, 158, 255})
#define SADCOLOR_TRANSPARENT ((SadColor){0, 0, 0, 0})

// ألوان Material Design
#define SADCOLOR_PRIMARY     ((SadColor){25, 118, 210, 255})
#define SADCOLOR_SECONDARY   ((SadColor){255, 64, 129, 255})
#define SADCOLOR_SURFACE     ((SadColor){255, 255, 255, 255})
#define SADCOLOR_BACKGROUND  ((SadColor){250, 250, 250, 255})
#define SADCOLOR_ERROR       ((SadColor){211, 47, 47, 255})

// دالة إنشاء لون
static inline SadColor sadcolor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (SadColor){r, g, b, a};
}

static inline SadColor sadcolor_hex(uint32_t hex) {
    return (SadColor){
        (hex >> 16) & 0xFF,
        (hex >> 8) & 0xFF,
        hex & 0xFF,
        255
    };
}

// ═══════════════════════════════════════════════════════════════════════════════
//  الأبعاد والموقع
// ═══════════════════════════════════════════════════════════════════════════════

typedef struct {
    float x, y;
} SadPoint;

typedef struct {
    float width, height;
} SadSize;

typedef struct {
    float x, y, width, height;
} SadRect;

typedef struct {
    float top, right, bottom, left;
} SadEdgeInsets;

typedef struct {
    float topLeft, topRight, bottomRight, bottomLeft;
} SadBorderRadius;

// دوال مساعدة
static inline SadEdgeInsets sadpadding_all(float value) {
    return (SadEdgeInsets){value, value, value, value};
}

static inline SadEdgeInsets sadpadding_symmetric(float vertical, float horizontal) {
    return (SadEdgeInsets){vertical, horizontal, vertical, horizontal};
}

static inline SadBorderRadius sadradius_all(float value) {
    return (SadBorderRadius){value, value, value, value};
}

// ═══════════════════════════════════════════════════════════════════════════════
//  أنواع الـ Widgets
// ═══════════════════════════════════════════════════════════════════════════════

typedef enum {
    // حاويات
    SADWIDGET_CONTAINER,        // حاوية بسيطة
    SADWIDGET_COLUMN,           // تخطيط عمودي
    SADWIDGET_ROW,              // تخطيط أفقي
    SADWIDGET_STACK,            // تخطيط مكدس
    SADWIDGET_GRID,             // شبكة
    SADWIDGET_LISTVIEW,         // قائمة قابلة للتمرير
    SADWIDGET_SCROLLVIEW,       // عرض قابل للتمرير
    SADWIDGET_WRAP,             // لف تلقائي
    
    // عناصر أساسية
    SADWIDGET_TEXT,             // نص
    SADWIDGET_ICON,             // أيقونة
    SADWIDGET_IMAGE,            // صورة
    SADWIDGET_DIVIDER,          // فاصل
    SADWIDGET_SPACER,           // مسافة
    
    // عناصر تفاعلية
    SADWIDGET_BUTTON,           // زر
    SADWIDGET_ICONBUTTON,       // زر أيقونة
    SADWIDGET_TEXTFIELD,        // حقل إدخال
    SADWIDGET_CHECKBOX,         // مربع اختيار
    SADWIDGET_RADIO,            // زر راديو
    SADWIDGET_SWITCH,           // مفتاح
    SADWIDGET_SLIDER,           // شريط تمرير
    SADWIDGET_DROPDOWN,         // قائمة منسدلة
    
    // بطاقات وأسطح
    SADWIDGET_CARD,             // بطاقة
    SADWIDGET_APPBAR,           // شريط التطبيق
    SADWIDGET_BOTTOMNAV,        // شريط التنقل السفلي
    SADWIDGET_DRAWER,           // درج جانبي
    SADWIDGET_DIALOG,           // حوار
    SADWIDGET_SNACKBAR,         // رسالة مؤقتة
    SADWIDGET_BOTTOMSHEET,      // ورقة سفلية
    
    // خاصة
    SADWIDGET_SCAFFOLD,         // هيكل الصفحة
    SADWIDGET_TABBAR,           // شريط تبويبات
    SADWIDGET_PAGEVIEW,         // عرض صفحات
    SADWIDGET_PROGRESSBAR,      // شريط تقدم
    SADWIDGET_CIRCLEPROGRESS,   // دائرة تقدم
    SADWIDGET_AVATAR,           // صورة رمزية
    SADWIDGET_BADGE,            // شارة
    SADWIDGET_CHIP,             // رقاقة
    SADWIDGET_FAB,              // زر عائم
    
    // رسم مخصص
    SADWIDGET_CANVAS,           // لوحة رسم مخصصة
    SADWIDGET_CUSTOM,           // مكون مخصص
    
    SADWIDGET_COUNT
} SadWidgetType;

// ═══════════════════════════════════════════════════════════════════════════════
//  محاذاة النص والعناصر
// ═══════════════════════════════════════════════════════════════════════════════

typedef enum {
    SADALIGN_START,             // البداية (يمين في RTL)
    SADALIGN_CENTER,            // المنتصف
    SADALIGN_END,               // النهاية (يسار في RTL)
    SADALIGN_STRETCH,           // تمدد
    SADALIGN_SPACE_BETWEEN,     // توزيع متساوي
    SADALIGN_SPACE_AROUND,      // توزيع مع هوامش
    SADALIGN_SPACE_EVENLY       // توزيع متساوي تماماً
} SadAlignment;

typedef enum {
    SADTEXT_LEFT,
    SADTEXT_CENTER,
    SADTEXT_RIGHT,
    SADTEXT_JUSTIFY
} SadTextAlign;

typedef enum {
    SADFONT_NORMAL,
    SADFONT_BOLD,
    SADFONT_ITALIC,
    SADFONT_BOLD_ITALIC
} SadFontStyle;

// ═══════════════════════════════════════════════════════════════════════════════
//  أنماط النص
// ═══════════════════════════════════════════════════════════════════════════════

typedef struct {
    float fontSize;
    SadFontStyle fontStyle;
    SadColor color;
    SadTextAlign textAlign;
    float letterSpacing;
    float lineHeight;
    bool underline;
    bool strikethrough;
    const char* fontFamily;
} SadTextStyle;

// أنماط نص مسبقة
#define SADTEXT_HEADLINE1    ((SadTextStyle){96, SADFONT_NORMAL, SADCOLOR_BLACK, SADTEXT_RIGHT, 0, 1.2f, false, false, NULL})
#define SADTEXT_HEADLINE2    ((SadTextStyle){60, SADFONT_NORMAL, SADCOLOR_BLACK, SADTEXT_RIGHT, 0, 1.2f, false, false, NULL})
#define SADTEXT_HEADLINE3    ((SadTextStyle){48, SADFONT_NORMAL, SADCOLOR_BLACK, SADTEXT_RIGHT, 0, 1.2f, false, false, NULL})
#define SADTEXT_HEADLINE4    ((SadTextStyle){34, SADFONT_NORMAL, SADCOLOR_BLACK, SADTEXT_RIGHT, 0, 1.2f, false, false, NULL})
#define SADTEXT_HEADLINE5    ((SadTextStyle){24, SADFONT_NORMAL, SADCOLOR_BLACK, SADTEXT_RIGHT, 0, 1.2f, false, false, NULL})
#define SADTEXT_HEADLINE6    ((SadTextStyle){20, SADFONT_BOLD, SADCOLOR_BLACK, SADTEXT_RIGHT, 0, 1.2f, false, false, NULL})
#define SADTEXT_BODY1        ((SadTextStyle){16, SADFONT_NORMAL, SADCOLOR_BLACK, SADTEXT_RIGHT, 0, 1.5f, false, false, NULL})
#define SADTEXT_BODY2        ((SadTextStyle){14, SADFONT_NORMAL, SADCOLOR_BLACK, SADTEXT_RIGHT, 0, 1.5f, false, false, NULL})
#define SADTEXT_CAPTION      ((SadTextStyle){12, SADFONT_NORMAL, SADCOLOR_GREY, SADTEXT_RIGHT, 0, 1.4f, false, false, NULL})
#define SADTEXT_BUTTON       ((SadTextStyle){14, SADFONT_BOLD, SADCOLOR_WHITE, SADTEXT_CENTER, 1.25f, 1.5f, false, false, NULL})

// ═══════════════════════════════════════════════════════════════════════════════
//  أنماط الحدود والظلال
// ═══════════════════════════════════════════════════════════════════════════════

typedef struct {
    float width;
    SadColor color;
} SadBorder;

typedef struct {
    SadColor color;
    float blurRadius;
    float spreadRadius;
    float offsetX;
    float offsetY;
} SadBoxShadow;

// ═══════════════════════════════════════════════════════════════════════════════
//  أنماط الزخرفة
// ═══════════════════════════════════════════════════════════════════════════════

typedef enum {
    SADDECOR_NONE,
    SADDECOR_COLOR,
    SADDECOR_GRADIENT,
    SADDECOR_IMAGE
} SadDecorationType;

typedef enum {
    SADGRADIENT_LINEAR,
    SADGRADIENT_RADIAL,
    SADGRADIENT_SWEEP
} SadGradientType;

typedef struct {
    SadDecorationType type;
    SadColor color;
    
    // للتدرج
    SadGradientType gradientType;
    SadColor gradientColors[4];
    int gradientColorCount;
    float gradientAngle;
    
    // للصورة
    const char* imagePath;
    
    // الحدود
    SadBorder border;
    SadBorderRadius borderRadius;
    
    // الظل
    SadBoxShadow shadow;
    bool hasShadow;
} SadBoxDecoration;

// ═══════════════════════════════════════════════════════════════════════════════
//  بنية الـ Widget الأساسية
// ═══════════════════════════════════════════════════════════════════════════════

typedef struct SadWidget SadWidget;
typedef void (*SadTapCallback)(SadWidget* widget, void* userData);
typedef void (*SadValueCallback)(SadWidget* widget, void* value, void* userData);
typedef void (*SadDrawCallback)(SadWidget* widget, void* canvas, void* userData);

struct SadWidget {
    // النوع والهوية
    SadWidgetType type;
    int id;
    const char* key;
    
    // الأبعاد
    float width;                // -1 = auto
    float height;               // -1 = auto
    float minWidth;
    float maxWidth;
    float minHeight;
    float maxHeight;
    float flex;                 // للتخطيط المرن
    
    // التخطيط المحسوب
    SadRect bounds;             // الحدود المحسوبة
    SadRect contentBounds;      // حدود المحتوى
    
    // الهوامش والحشو
    SadEdgeInsets margin;
    SadEdgeInsets padding;
    
    // الزخرفة
    SadBoxDecoration decoration;
    
    // الرؤية والتفاعل
    bool visible;
    bool enabled;
    float opacity;
    
    // الشجرة
    SadWidget* parent;
    SadWidget** children;
    int childCount;
    int childCapacity;
    
    // بيانات خاصة بالنوع (union لتوفير الذاكرة)
    union {
        // للنص
        struct {
            char* text;
            SadTextStyle textStyle;
            int maxLines;
            bool selectable;
        } textData;
        
        // للزر
        struct {
            char* label;
            SadTextStyle labelStyle;
            SadColor buttonColor;
            SadColor pressedColor;
            SadColor disabledColor;
            bool isPressed;
            bool isElevated;
            float elevation;
        } buttonData;
        
        // لحقل الإدخال
        struct {
            char* value;
            char* placeholder;
            char* label;
            bool obscureText;
            bool multiline;
            int maxLength;
            bool focused;
        } textFieldData;
        
        // لمربع الاختيار والمفتاح
        struct {
            bool checked;
            SadColor activeColor;
        } toggleData;
        
        // للشريط
        struct {
            float value;
            float min;
            float max;
            SadColor activeColor;
            SadColor trackColor;
        } sliderData;
        
        // للقائمة
        struct {
            const char** items;
            int itemCount;
            int selectedIndex;
            bool expanded;
        } dropdownData;
        
        // للصورة
        struct {
            const char* src;
            unsigned char* pixels;
            int imgWidth;
            int imgHeight;
            uint32_t textureId;
        } imageData;
        
        // للأيقونة
        struct {
            int iconCode;
            float iconSize;
            SadColor iconColor;
        } iconData;
        
        // لشريط التقدم
        struct {
            float progress;
            bool indeterminate;
            SadColor progressColor;
            SadColor backgroundColor;
        } progressData;
        
        // للتخطيط
        struct {
            SadAlignment mainAxisAlignment;
            SadAlignment crossAxisAlignment;
            float spacing;
            bool scrollable;
            float scrollOffset;
        } layoutData;
        
        // للرسم المخصص
        struct {
            SadDrawCallback onDraw;
            void* drawUserData;
        } canvasData;
    };
    
    // الأحداث
    SadTapCallback onTap;
    SadTapCallback onLongPress;
    SadTapCallback onDoubleTap;
    SadValueCallback onChanged;
    void* userData;
    
    // الرسوم المتحركة
    bool animating;
    float animationProgress;
    int animationType;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  التطبيق والنافذة
// ═══════════════════════════════════════════════════════════════════════════════

typedef struct {
    // الشاشة
    int screenWidth;
    int screenHeight;
    float density;
    
    // الجذر
    SadWidget* rootWidget;
    
    // الحالة
    bool running;
    bool needsRebuild;
    bool needsLayout;
    bool needsPaint;
    
    // RTL
    bool isRTL;
    
    // السمة
    SadColor primaryColor;
    SadColor secondaryColor;
    SadColor backgroundColor;
    SadColor surfaceColor;
    SadColor textColor;
    
    // الخطوط
    void* arabicFont;
    void* latinFont;
    void* iconFont;
    
    // OpenGL
    uint32_t shaderProgram;
    uint32_t textShader;
    uint32_t roundedRectShader;
    
    // Widget قيد التركيز
    SadWidget* focusedWidget;
    SadWidget* pressedWidget;
    
    // قائمة الـ Widgets للتنظيف
    SadWidget** allWidgets;
    int widgetCount;
    int widgetCapacity;
    
} SadApp;

// ═══════════════════════════════════════════════════════════════════════════════
//  إنشاء وإدارة التطبيق
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تهيئة التطبيق
 */
SadApp* sadapp_create(int width, int height, float density);

/**
 * تدمير التطبيق
 */
void sadapp_destroy(SadApp* app);

/**
 * تعيين الـ Widget الجذر
 */
void sadapp_set_root(SadApp* app, SadWidget* root);

/**
 * تحديث التخطيط
 */
void sadapp_layout(SadApp* app);

/**
 * رسم الإطار
 */
void sadapp_render(SadApp* app);

/**
 * معالجة حدث لمس
 */
bool sadapp_touch(SadApp* app, int action, float x, float y);

/**
 * تشغيل حلقة الرسم
 */
void sadapp_run_frame(SadApp* app);

/**
 * هل يحتاج إعادة رسم
 */
bool sadapp_needs_redraw(SadApp* app);

/**
 * طلب إعادة بناء
 */
void sadapp_rebuild(SadApp* app);

// ═══════════════════════════════════════════════════════════════════════════════
//  إنشاء الـ Widgets
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * حاوية Container
 */
SadWidget* sad_container(void);

/**
 * تخطيط عمودي Column
 */
SadWidget* sad_column(void);

/**
 * تخطيط أفقي Row
 */
SadWidget* sad_row(void);

/**
 * تخطيط مكدس Stack
 */
SadWidget* sad_stack(void);

/**
 * نص Text
 */
SadWidget* sad_text(const char* text);

/**
 * نص بنمط
 */
SadWidget* sad_text_styled(const char* text, SadTextStyle style);

/**
 * أيقونة
 */
SadWidget* sad_icon(int iconCode, float size, SadColor color);

/**
 * صورة
 */
SadWidget* sad_image(const char* path);

/**
 * زر Button
 */
SadWidget* sad_button(const char* label, SadTapCallback onTap, void* userData);

/**
 * زر مرتفع ElevatedButton
 */
SadWidget* sad_elevated_button(const char* label, SadColor color, SadTapCallback onTap, void* userData);

/**
 * زر نص TextButton
 */
SadWidget* sad_text_button(const char* label, SadTapCallback onTap, void* userData);

/**
 * زر أيقونة IconButton
 */
SadWidget* sad_icon_button(int iconCode, SadTapCallback onTap, void* userData);

/**
 * زر عائم FAB
 */
SadWidget* sad_fab(int iconCode, SadColor color, SadTapCallback onTap, void* userData);

/**
 * حقل إدخال TextField
 */
SadWidget* sad_textfield(const char* label, const char* placeholder);

/**
 * مربع اختيار Checkbox
 */
SadWidget* sad_checkbox(bool checked, SadValueCallback onChanged, void* userData);

/**
 * مفتاح Switch
 */
SadWidget* sad_switch(bool value, SadValueCallback onChanged, void* userData);

/**
 * شريط تمرير Slider
 */
SadWidget* sad_slider(float value, float min, float max, SadValueCallback onChanged, void* userData);

/**
 * بطاقة Card
 */
SadWidget* sad_card(void);

/**
 * شريط التطبيق AppBar
 */
SadWidget* sad_appbar(const char* title);

/**
 * شريط التنقل السفلي BottomNavigationBar
 */
SadWidget* sad_bottomnav(void);

/**
 * عنصر تنقل
 */
void sad_bottomnav_add_item(SadWidget* nav, int iconCode, const char* label);

/**
 * هيكل الصفحة Scaffold
 */
SadWidget* sad_scaffold(void);

/**
 * قائمة ListView
 */
SadWidget* sad_listview(void);

/**
 * عرض التمرير ScrollView
 */
SadWidget* sad_scrollview(void);

/**
 * فاصل Divider
 */
SadWidget* sad_divider(void);

/**
 * مسافة Spacer
 */
SadWidget* sad_spacer(float size);

/**
 * شريط التقدم ProgressBar
 */
SadWidget* sad_progressbar(float value);

/**
 * دائرة التقدم CircularProgress
 */
SadWidget* sad_circular_progress(void);

/**
 * صورة رمزية Avatar
 */
SadWidget* sad_avatar(const char* imagePath, float radius);

/**
 * شارة Badge
 */
SadWidget* sad_badge(const char* text, SadColor color);

/**
 * رقاقة Chip
 */
SadWidget* sad_chip(const char* label);

/**
 * لوحة رسم مخصصة Canvas
 */
SadWidget* sad_canvas(SadDrawCallback onDraw, void* userData);

// ═══════════════════════════════════════════════════════════════════════════════
//  تعديل الـ Widgets
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * إضافة ابن
 */
void sad_add_child(SadWidget* parent, SadWidget* child);

/**
 * إضافة عدة أبناء
 */
void sad_add_children(SadWidget* parent, SadWidget** children, int count);

/**
 * تعيين الأبعاد
 */
void sad_set_size(SadWidget* widget, float width, float height);

/**
 * تعيين الهوامش
 */
void sad_set_margin(SadWidget* widget, SadEdgeInsets margin);

/**
 * تعيين الحشو
 */
void sad_set_padding(SadWidget* widget, SadEdgeInsets padding);

/**
 * تعيين لون الخلفية
 */
void sad_set_background(SadWidget* widget, SadColor color);

/**
 * تعيين التدرج
 */
void sad_set_gradient(SadWidget* widget, SadColor color1, SadColor color2, float angle);

/**
 * تعيين الحدود
 */
void sad_set_border(SadWidget* widget, float width, SadColor color);

/**
 * تعيين زوايا مستديرة
 */
void sad_set_border_radius(SadWidget* widget, SadBorderRadius radius);

/**
 * تعيين الظل
 */
void sad_set_shadow(SadWidget* widget, SadColor color, float blur, float offsetX, float offsetY);

/**
 * تعيين الشفافية
 */
void sad_set_opacity(SadWidget* widget, float opacity);

/**
 * تعيين المحاذاة
 */
void sad_set_alignment(SadWidget* widget, SadAlignment mainAxis, SadAlignment crossAxis);

/**
 * تعيين التباعد
 */
void sad_set_spacing(SadWidget* widget, float spacing);

/**
 * تعيين المرونة
 */
void sad_set_flex(SadWidget* widget, float flex);

/**
 * إخفاء/إظهار
 */
void sad_set_visible(SadWidget* widget, bool visible);

/**
 * تفعيل/تعطيل
 */
void sad_set_enabled(SadWidget* widget, bool enabled);

// ═══════════════════════════════════════════════════════════════════════════════
//  الأحداث
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تعيين حدث النقر
 */
void sad_on_tap(SadWidget* widget, SadTapCallback callback, void* userData);

/**
 * تعيين حدث الضغط المطول
 */
void sad_on_long_press(SadWidget* widget, SadTapCallback callback, void* userData);

/**
 * تعيين حدث التغيير
 */
void sad_on_changed(SadWidget* widget, SadValueCallback callback, void* userData);

// ═══════════════════════════════════════════════════════════════════════════════
//  الرسوم المتحركة
// ═══════════════════════════════════════════════════════════════════════════════

typedef enum {
    SADANIM_FADE_IN,
    SADANIM_FADE_OUT,
    SADANIM_SLIDE_IN_LEFT,
    SADANIM_SLIDE_IN_RIGHT,
    SADANIM_SLIDE_IN_UP,
    SADANIM_SLIDE_IN_DOWN,
    SADANIM_SCALE_IN,
    SADANIM_SCALE_OUT,
    SADANIM_BOUNCE,
    SADANIM_SHAKE
} SadAnimationType;

/**
 * تشغيل رسم متحرك
 */
void sad_animate(SadWidget* widget, SadAnimationType type, float durationMs);

/**
 * إيقاف الرسم المتحرك
 */
void sad_stop_animation(SadWidget* widget);

// ═══════════════════════════════════════════════════════════════════════════════
//  الحوارات والرسائل
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * عرض حوار
 */
void sad_show_dialog(SadApp* app, const char* title, const char* message, 
                     SadTapCallback onConfirm, SadTapCallback onCancel, void* userData);

/**
 * عرض رسالة مؤقتة
 */
void sad_show_snackbar(SadApp* app, const char* message, int durationMs);

/**
 * عرض ورقة سفلية
 */
void sad_show_bottom_sheet(SadApp* app, SadWidget* content);

/**
 * إغلاق الحوار/الورقة
 */
void sad_dismiss(SadApp* app);

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال الرسم المباشر (للـ Canvas)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * رسم مستطيل
 */
void sad_draw_rect(float x, float y, float width, float height, SadColor color);

/**
 * رسم مستطيل مستدير
 */
void sad_draw_rounded_rect(float x, float y, float width, float height, 
                           SadBorderRadius radius, SadColor color);

/**
 * رسم دائرة
 */
void sad_draw_circle(float cx, float cy, float radius, SadColor color);

/**
 * رسم خط
 */
void sad_draw_line(float x1, float y1, float x2, float y2, float width, SadColor color);

/**
 * رسم نص
 */
void sad_draw_text(const char* text, float x, float y, SadTextStyle style);

/**
 * رسم صورة
 */
void sad_draw_image(uint32_t textureId, float x, float y, float width, float height);

// ═══════════════════════════════════════════════════════════════════════════════
//  أيقونات Material Design (رموز Unicode)
// ═══════════════════════════════════════════════════════════════════════════════

#define SADICON_HOME            0xE88A
#define SADICON_SETTINGS        0xE8B8
#define SADICON_PERSON          0xE7FD
#define SADICON_SEARCH          0xE8B6
#define SADICON_MENU            0xE5D2
#define SADICON_CLOSE           0xE5CD
#define SADICON_ADD             0xE145
#define SADICON_REMOVE          0xE15B
#define SADICON_EDIT            0xE3C9
#define SADICON_DELETE          0xE872
#define SADICON_CHECK           0xE5CA
#define SADICON_ARROW_BACK      0xE5C4
#define SADICON_ARROW_FORWARD   0xE5C8
#define SADICON_ARROW_UP        0xE5D8
#define SADICON_ARROW_DOWN      0xE5DB
#define SADICON_REFRESH         0xE5D5
#define SADICON_SHARE           0xE80D
#define SADICON_FAVORITE        0xE87D
#define SADICON_STAR            0xE838
#define SADICON_NOTIFICATIONS   0xE7F4
#define SADICON_EMAIL           0xE0BE
#define SADICON_PHONE           0xE0CD
#define SADICON_LOCATION        0xE0C8
#define SADICON_CAMERA          0xE3AF
#define SADICON_IMAGE           0xE3F4
#define SADICON_LOCK            0xE897
#define SADICON_VISIBILITY      0xE8F4
#define SADICON_VISIBILITY_OFF  0xE8F5

// أيقونات بنكية
#define SADICON_ACCOUNT_BALANCE 0xE84F
#define SADICON_CREDIT_CARD     0xE870
#define SADICON_PAYMENT         0xE8A1
#define SADICON_TRANSFER        0xE8D4
#define SADICON_WALLET          0xE850
#define SADICON_ATM             0xE573
#define SADICON_RECEIPT         0xE8B0
#define SADICON_HISTORY         0xE889

#ifdef __cplusplus
}
#endif

#endif // SAD_FLUTTER_UI_H
