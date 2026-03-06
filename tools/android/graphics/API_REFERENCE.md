# مرجع API محرك الواجهة — SadUI Engine

مرجع شامل لواجهة برمجة التطبيقات العربية لمحرك الواجهة الأصلي (Native UI Engine).

---

## فهرس المحتويات

1. [التهيئة والتنظيف](#التهيئة-والتنظيف)
2. [إنشاء العناصر](#إنشاء-العناصر)
3. [التخطيط والأنماط](#التخطيط-والأنماط)
4. [معالجة الأحداث](#معالجة-الأحداث)
5. [الرسوم المتحركة](#الرسوم-المتحركة)
6. [التنقل والشاشات](#التنقل-والشاشات)
7. [الثيمات](#الثيمات)
8. [تحسين الأداء](#تحسين-الأداء)
9. [الشبكة](#الشبكة)
10. [الرسم المباشر (Canvas)](#الرسم-المباشر-canvas)
11. [الموارد](#الموارد)
12. [دوال مساعدة](#دوال-مساعدة)

---

## التهيئة والتنظيف

### `sadui_init`
```c
bool sadui_init(void* androidApp)
```
تهيئة محرك الواجهة.

| المعامل | النوع | الوصف |
|---------|-------|-------|
| `androidApp` | `void*` | مؤشر إلى `android_app` |

**الإرجاع:** `true` عند النجاح، `false` عند الفشل.

### `sadui_shutdown`
```c
void sadui_shutdown(void)
```
تنظيف الموارد وإيقاف المحرك.

### `sadui_render`
```c
void sadui_render(float deltaTime)
```
رسم إطار واحد.

| المعامل | النوع | الوصف |
|---------|-------|-------|
| `deltaTime` | `float` | الوقت منذ الإطار السابق (بالثواني) |

---

## إنشاء العناصر

### أنواع العناصر المتاحة

| النوع | الثابت | الوصف |
|-------|--------|-------|
| حاوية | `SAD_WIDGET_CONTAINER` | حاوية عامة |
| صف | `SAD_WIDGET_ROW` | ترتيب أفقي |
| عمود | `SAD_WIDGET_COLUMN` | ترتيب عمودي |
| مكدس | `SAD_WIDGET_STACK` | عناصر فوق بعضها |
| نص | `SAD_WIDGET_TEXT` | عرض نص |
| زر | `SAD_WIDGET_BUTTON` | زر قابل للنقر |
| صورة | `SAD_WIDGET_IMAGE` | عرض صورة |
| حقل نص | `SAD_WIDGET_TEXT_FIELD` | إدخال نص |
| مربع اختيار | `SAD_WIDGET_CHECKBOX` | مربع تحديد |
| منزلق | `SAD_WIDGET_SLIDER` | شريط تمرير |
| تقدم | `SAD_WIDGET_PROGRESS` | شريط تقدم |
| قابل للتمرير | `SAD_WIDGET_SCROLLABLE` | منطقة تمرير |
| قائمة | `SAD_WIDGET_LIST` | قائمة عناصر |
| شبكة | `SAD_WIDGET_GRID` | شبكة عناصر |

### `sadui_create`
```c
SadWidgetId sadui_create(SadWidgetType type)
```
إنشاء عنصر جديد.

**مثال:**
```c
SadWidgetId btn = sadui_create(SAD_WIDGET_BUTTON);
sadui_set_text(btn, "اضغطني");
```

### دوال الإنشاء السريعة

```c
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
```

### إدارة الشجرة

```c
void sadui_add_child(SadWidgetId parent, SadWidgetId child);
void sadui_set_root(SadWidgetId widget);
void sadui_destroy(SadWidgetId widget);
```

---

## التخطيط والأنماط

### بنية `SadLayout`

```c
typedef struct {
    SadLayoutType type;        // نوع التخطيط
    SadSize width;             // العرض
    SadSize height;            // الارتفاع
    SadEdges padding;          // الحشو
    SadEdges margin;           // الهامش
    SadAlignment alignment;    // المحاذاة
    float gap;                 // المسافة بين العناصر
    SadFlexWrap wrap;          // التفاف العناصر
} SadLayout;
```

### أنواع الأحجام

```c
SadSize sadui_fixed(float value);    // حجم ثابت (بكسل)
SadSize sadui_percent(float pct);    // نسبة مئوية
SadSize sadui_auto(void);            // حجم تلقائي
SadSize sadui_fill(void);            // ملء المساحة
```

**مثال:**
```c
SadLayout layout = {0};
layout.width = sadui_fill();
layout.height = sadui_fixed(50);
layout.padding = sadui_edges(16);
sadui_set_layout(widget, &layout);
```

### بنية `SadStyle`

```c
typedef struct {
    SadColor backgroundColor;
    SadColor textColor;
    float fontSize;
    SadFontWeight fontWeight;
    SadCorners corners;        // زوايا مستديرة
    SadBorder border;          // الإطار
    SadShadow shadow;          // الظل
    bool hasShadow;
    float opacity;             // الشفافية (0-1)
    SadGradient gradient;      // التدرج
    bool hasGradient;
} SadStyle;
```

### `sadui_set_layout` / `sadui_set_style`
```c
void sadui_set_layout(SadWidgetId id, const SadLayout* layout);
void sadui_set_style(SadWidgetId id, const SadStyle* style);
```

---

## معالجة الأحداث

### أنواع الأحداث

| الحدث | الدالة | الوصف |
|-------|--------|-------|
| النقر | `sadui_on_click` | عند النقر على العنصر |
| تغيير النص | `sadui_on_text_change` | تغيير نص الإدخال |
| تغيير القيمة | `sadui_on_value_change` | تغيير المنزلق/الاختيار |
| اللمس | `sadui_on_touch` | لمس يدوي مخصص |
| الإيماءات | `sadui_on_gesture` | سحب، تكبير، إلخ |

### `sadui_on_click`
```c
void sadui_on_click(SadWidgetId id, SadEventHandler handler, void* userData);

// نوع المعالج:
typedef void (*SadEventHandler)(SadWidgetId widget, void* userData);
```

**مثال:**
```c
void onClick(SadWidgetId widget, void* userData) {
    SADLOG("تم النقر!");
}

sadui_on_click(btn, onClick, NULL);
```

### `sadui_on_gesture`
```c
void sadui_on_gesture(SadWidgetId id, SadGestureHandler handler, void* userData);

typedef void (*SadGestureHandler)(SadWidgetId widget, SadGestureType type,
                                   SadVec2 delta, float scale, void* userData);
```

أنواع الإيماءات:
- `SAD_GESTURE_TAP` — نقرة واحدة
- `SAD_GESTURE_DOUBLE_TAP` — نقرة مزدوجة
- `SAD_GESTURE_LONG_PRESS` — ضغط مطول
- `SAD_GESTURE_PAN` — سحب
- `SAD_GESTURE_SWIPE_LEFT/RIGHT/UP/DOWN` — تمرير
- `SAD_GESTURE_PINCH` — تكبير/تصغير
- `SAD_GESTURE_ROTATE` — دوران

---

## الرسوم المتحركة

### بنية `SadAnimation`

```c
typedef struct {
    SadAnimationType type;     // نوع الرسم المتحرك
    float duration;            // المدة (مللي ثانية)
    float delay;               // التأخير (مللي ثانية)
    SadEasing easing;          // دالة التسهيل
    bool loop;                 // التكرار
    int repeatCount;           // عدد مرات التكرار (-1 = لانهائي)
    bool reverse;              // عكس الاتجاه
} SadAnimation;
```

### أنواع الرسوم المتحركة

| النوع | الثابت |
|-------|--------|
| تلاشي | `SAD_ANIM_FADE` |
| تحريك X | `SAD_ANIM_TRANSLATE_X` |
| تحريك Y | `SAD_ANIM_TRANSLATE_Y` |
| تكبير | `SAD_ANIM_SCALE` |
| دوران | `SAD_ANIM_ROTATE` |
| انزلاق للأعلى | `SAD_ANIM_SLIDE_UP` |
| انزلاق للأسفل | `SAD_ANIM_SLIDE_DOWN` |
| انزلاق لليسار | `SAD_ANIM_SLIDE_LEFT` |
| انزلاق لليمين | `SAD_ANIM_SLIDE_RIGHT` |
| ارتداد | `SAD_ANIM_BOUNCE` |
| اهتزاز | `SAD_ANIM_SHAKE` |
| نبض | `SAD_ANIM_PULSE` |

### دوال التسهيل (Easing)

| الدالة | الثابت | الوصف |
|--------|--------|-------|
| خطي | `SAD_EASE_LINEAR` | سرعة ثابتة |
| تسهيل للداخل | `SAD_EASE_IN` | بداية بطيئة |
| تسهيل للخارج | `SAD_EASE_OUT` | نهاية بطيئة |
| تسهيل مزدوج | `SAD_EASE_IN_OUT` | بداية ونهاية بطيئة |
| ارتداد | `SAD_EASE_BOUNCE` | ارتداد في النهاية |
| مرونة | `SAD_EASE_ELASTIC` | تأثير مرن |
| رجوع | `SAD_EASE_BACK` | تجاوز ثم رجوع |

### `sadui_animate`
```c
SadAnimId sadui_animate(SadWidgetId widget, const SadAnimation* anim);
```

### دوال الرسوم المتحركة السريعة

```c
SadAnimId sadui_fade_in(SadWidgetId widget, float durationMs);
SadAnimId sadui_fade_out(SadWidgetId widget, float durationMs);
SadAnimId sadui_slide_up(SadWidgetId widget, float durationMs);
SadAnimId sadui_slide_down(SadWidgetId widget, float durationMs);
SadAnimId sadui_bounce(SadWidgetId widget, float durationMs);
SadAnimId sadui_shake(SadWidgetId widget, float durationMs);
SadAnimId sadui_pulse(SadWidgetId widget, float durationMs);
```

**مثال:**
```c
// ظهور تدريجي خلال 300 مللي ثانية
sadui_fade_in(widget, 300);

// اهتزاز للتنبيه
sadui_shake(errorLabel, 500);
```

---

## التنقل والشاشات

### `sadnav_init`
```c
void sadnav_init(void)
```
تهيئة نظام التنقل.

### `sadnav_register`
```c
bool sadnav_register(const char* route, SadScreenBuilder builder, void* userData)
```
تسجيل شاشة جديدة.

| المعامل | النوع | الوصف |
|---------|-------|-------|
| `route` | `const char*` | اسم المسار (مثل: `/الرئيسية`) |
| `builder` | `SadScreenBuilder` | دالة بناء الواجهة |
| `userData` | `void*` | بيانات المستخدم |

```c
typedef SadWidgetId (*SadScreenBuilder)(void* userData);
```

### `sadnav_navigate`
```c
void sadnav_navigate(const char* route)
```
الانتقال إلى شاشة.

### `sadnav_back`
```c
bool sadnav_back(void)
```
الرجوع للشاشة السابقة.

**الإرجاع:** `true` إذا نجح الرجوع، `false` إذا كانت الكومة فارغة.

### `sadnav_current`
```c
const char* sadnav_current(void)
```
الحصول على المسار الحالي.

### `sadnav_can_go_back`
```c
bool sadnav_can_go_back(void)
```
هل يمكن الرجوع؟

**مثال كامل:**
```c
// بناء شاشة الإعدادات
SadWidgetId buildSettings(void* data) {
    SadWidgetId col = sadui_column();
    sadui_add_child(col, sadui_heading("الإعدادات"));
    
    SadWidgetId backBtn = sadui_button("رجوع");
    sadui_on_click(backBtn, onBack, NULL);
    sadui_add_child(col, backBtn);
    
    return col;
}

void onBack(SadWidgetId w, void* d) {
    sadnav_back();
}

// التسجيل
sadnav_register("/الإعدادات", buildSettings, NULL);

// الانتقال
sadnav_navigate("/الإعدادات");
```

---

## الثيمات

### بنية `SadTheme`

```c
typedef struct {
    SadColor primary;           // اللون الرئيسي
    SadColor primaryVariant;    // تنويع اللون الرئيسي
    SadColor secondary;         // اللون الثانوي
    SadColor secondaryVariant;
    SadColor background;        // لون الخلفية
    SadColor surface;           // لون الأسطح
    SadColor error;             // لون الخطأ
    SadColor onPrimary;         // نص على اللون الرئيسي
    SadColor onSecondary;
    SadColor onBackground;
    SadColor onSurface;
    SadColor onError;
    
    float cornerRadiusSmall;    // زوايا صغيرة
    float cornerRadiusMedium;   // زوايا متوسطة
    float cornerRadiusLarge;    // زوايا كبيرة
    
    float elevationSmall;       // ارتفاع صغير
    float elevationMedium;
    float elevationLarge;
    
    float fontSizeSmall;        // حجم خط صغير
    float fontSizeMedium;
    float fontSizeLarge;
    float fontSizeHeading;      // حجم عنوان
    
    bool rtl;                   // من اليمين لليسار
} SadTheme;
```

### دوال الثيمات

```c
void sadui_set_theme(const SadTheme* theme);
const SadTheme* sadui_get_theme(void);
const SadTheme* sadui_theme_light(void);
const SadTheme* sadui_theme_dark(void);
```

**مثال:**
```c
// استخدام الثيم الداكن
sadui_set_theme(sadui_theme_dark());

// ثيم مخصص
SadTheme myTheme = *sadui_theme_light();
myTheme.primary = sadui_hex(0x2196F3);  // أزرق
myTheme.rtl = true;
sadui_set_theme(&myTheme);
```

---

## تحسين الأداء

### Dirty Flags

نظام لتقليل إعادة الرسم غير الضرورية.

```c
void sadui_invalidate(void);              // تحديد الحاجة لإعادة رسم
void sadui_invalidate_widget(SadWidgetId);// تحديد عنصر معين
void sadui_invalidate_layout(void);       // إعادة حساب التخطيط

bool sadui_needs_redraw(void);            // هل نحتاج رسم؟
bool sadui_needs_layout(void);            // هل نحتاج تخطيط؟
```

### مراقبة الأداء

```c
int sadui_get_fps(void);           // عدد الإطارات في الثانية
uint32_t sadui_get_frame_count(void); // عداد الإطارات الكلي
```

**ملاحظة:** الدوال التي تغيّر الواجهة تستدعي `sadui_invalidate()` تلقائيًا:
- `sadui_set_text()`
- `sadui_set_style()`
- `sadui_set_visible()`
- `sadui_set_value()`
- `sadui_add_child()`

---

## الشبكة

### `sadnet_http_get`
```c
int sadnet_http_get(const char* url, char* responseBuffer, 
                    int bufferSize, int* responseSize)
```
طلب HTTP GET.

| المعامل | الوصف |
|---------|-------|
| `url` | الرابط (HTTP فقط حاليًا) |
| `responseBuffer` | مصفوفة لتخزين الاستجابة |
| `bufferSize` | حجم المصفوفة |
| `responseSize` | [خرج] الحجم الفعلي |

**الإرجاع:** `0` = نجاح، خلاف ذلك = خطأ

### `sadnet_http_post`
```c
int sadnet_http_post(const char* url, const char* body,
                     char* responseBuffer, int bufferSize, int* responseSize)
```
طلب HTTP POST.

### `sadnet_parse_json`
```c
int sadnet_parse_json(const char* json, SadJsonCallback callback, void* userData)
```
تحليل JSON واستدعاء callback لكل عنصر.

**ملاحظة:** HTTPS غير مدعوم حاليًا — سيُضاف في إصدار قادم.

---

## الرسم المباشر (Canvas)

### `sadui_get_canvas`
```c
SadCanvas sadui_get_canvas(SadWidgetId widget)
```
الحصول على سياق الرسم من عنصر Canvas.

### دوال الرسم

```c
void sadcanvas_draw_rect(SadCanvas ctx, SadRect rect, SadColor color);
void sadcanvas_draw_rounded_rect(SadCanvas ctx, SadRect rect, float radius, SadColor color);
void sadcanvas_draw_circle(SadCanvas ctx, float x, float y, float radius, SadColor color);
void sadcanvas_draw_arc(SadCanvas ctx, float x, float y, float radius, 
                        float startAngle, float endAngle, SadColor color);
void sadcanvas_draw_line(SadCanvas ctx, float x1, float y1, float x2, float y2,
                         float thickness, SadColor color);
void sadcanvas_draw_path(SadCanvas ctx, SadVec2* points, int count,
                         float thickness, SadColor color, bool closed);
void sadcanvas_draw_text(SadCanvas ctx, const char* text, float x, float y,
                         float fontSize, SadColor color);
void sadcanvas_draw_image(SadCanvas ctx, int imageId, SadRect destRect);
void sadcanvas_draw_gradient(SadCanvas ctx, SadRect rect, SadGradient gradient);
```

---

## الموارد

### تحميل الخطوط

```c
SadFontId sadui_load_font(const char* path, float size);
void sadui_unload_font(SadFontId fontId);
```

### تحميل الصور

```c
SadImageId sadui_load_image(const char* path);
SadImageId sadui_load_image_memory(const uint8_t* data, int size);
void sadui_unload_image(SadImageId imageId);
void sadui_get_image_size(SadImageId imageId, int* width, int* height);
```

---

## دوال مساعدة

### إنشاء الألوان

```c
// من RGBA
SadColor sadui_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

// من قيمة سداسية
SadColor sadui_hex(uint32_t hex);  // مثال: 0xFF5722
```

### الألوان الجاهزة

```c
#define SAD_COLOR_WHITE       0xFFFFFFFF
#define SAD_COLOR_BLACK       0x000000FF
#define SAD_COLOR_RED         0xFF0000FF
#define SAD_COLOR_GREEN       0x00FF00FF
#define SAD_COLOR_BLUE        0x0000FFFF
#define SAD_COLOR_TRANSPARENT 0x00000000
```

### إنشاء الحواف

```c
SadEdges sadui_edges(float all);
SadEdges sadui_edges_vh(float vertical, float horizontal);
SadEdges sadui_edges_ltrb(float left, float top, float right, float bottom);
```

### إنشاء الزوايا

```c
SadCorners sadui_corners(float all);
SadCorners sadui_corners_top(float top);
SadCorners sadui_corners_bottom(float bottom);
```

---

## مثال كامل

```c
#include "sad_ui_engine.h"

void onClick(SadWidgetId widget, void* data) {
    sadnav_navigate("/التفاصيل");
}

SadWidgetId buildHome(void* data) {
    SadWidgetId col = sadui_column();
    
    SadLayout layout = {0};
    layout.padding = sadui_edges(24);
    layout.gap = 16;
    sadui_set_layout(col, &layout);
    
    sadui_add_child(col, sadui_heading("مرحباً بك!"));
    sadui_add_child(col, sadui_text("هذا تطبيق بلغة ص"));
    
    SadWidgetId btn = sadui_button("انتقل للتفاصيل");
    sadui_on_click(btn, onClick, NULL);
    sadui_add_child(col, btn);
    
    // رسم متحرك عند الظهور
    sadui_fade_in(col, 300);
    
    return col;
}

int main() {
    sadui_init(app);
    sadnav_init();
    
    sadui_set_theme(sadui_theme_dark());
    
    sadnav_register("/", buildHome, NULL);
    sadnav_navigate("/");
    
    // حلقة الرسم
    while (running) {
        sadui_render(deltaTime);
    }
    
    sadui_shutdown();
    return 0;
}
```

---

## الإصدار

- **الإصدار:** 1.0.0
- **التاريخ:** 2025
- **المتطلبات:** Android API 21+، OpenGL ES 3.0

---

**ملاحظات:**
- جميع الدوال آمنة للاستدعاء من الخيط الرئيسي فقط
- معرّفات العناصر (`SadWidgetId`) صالحة حتى استدعاء `sadui_destroy()`
- الرسوم المتحركة تعمل تلقائيًا مع `sadui_render()`
