# النظام الرسومي الثوري للغة ص على أندرويد
# SadUI Revolutionary Graphics Engine for Android

## نظرة عامة

النظام الرسومي الثوري هو محرك واجهات مستخدم أصلي (Native) مبني على OpenGL ES 3.0، مصمم خصيصاً للغة ص. يوفر مجموعة شاملة من المكونات الرسومية مع دعم كامل للغة العربية واتجاه الكتابة من اليمين لليسار (RTL).

## الميزات الرئيسية

### 🎨 نظام المكونات (40+ مكون)
- **الأزرار**: مملوء، محدد، نصي، أيقونة، عائم (FAB)
- **النصوص**: عناوين، فقرات، تسميات
- **الإدخال**: حقول نص، كلمات مرور، مناطق نص
- **التبديل**: مربعات اختيار، مفاتيح، أزرار راديو
- **القوائم**: منسدلة، منبثقة، درج جانبي
- **التخطيط**: أعمدة، صفوف، شبكات، مكدسات
- **البطاقات**: بطاقات، حوارات، شرائح
- **الرسومات**: Canvas، رسوم متحركة

### 🎯 نظام التخطيط (Flexbox-style)
```c
SadLayout layout = {
    .mainAxis = SAD_ALIGN_CENTER,      // المحور الرئيسي
    .crossAxis = SAD_ALIGN_STRETCH,    // المحور العرضي
    .width = {SAD_SIZE_FILL, 0},       // عرض يملأ المتاح
    .height = {SAD_SIZE_WRAP, 0},      // ارتفاع يتكيف حسب المحتوى
    .padding = sadui_edges(16),        // حشوة متساوية
    .gap = 8,                          // مسافة بين العناصر
    .rtl = true                        // اتجاه عربي
};
```

### 🎭 نظام الأنماط
```c
SadStyle style = {
    .backgroundColor = 0x6750A4FF,     // لون الخلفية (RGBA)
    .textColor = 0xFFFFFFFF,           // لون النص
    .borderRadius = sadui_corners(12), // زوايا مستديرة
    .borderWidth = 2,                  // سمك الحد
    .borderColor = 0x000000FF,         // لون الحد
    .fontSize = 16.0f,                 // حجم الخط
    .fontWeight = 500,                 // وزن الخط
    .opacity = 1.0f,                   // الشفافية
    .hasShadow = true,                 // تفعيل الظل
    .shadow = {0, 4, 8, 0, 0x00000040} // إعدادات الظل
};
```

### 🌈 نظام الثيمات (Material Design 3)
```c
// الثيم الفاتح (الافتراضي)
sadui_set_theme(sadui_theme_light());

// الثيم الداكن
sadui_set_theme(sadui_theme_dark());

// ثيم مخصص
SadTheme myTheme = {
    .primary = 0x2196F3FF,        // اللون الأساسي
    .secondary = 0xFF9800FF,      // اللون الثانوي
    .background = 0xFFFAFAFF,     // لون الخلفية
    .surface = 0xFFFFFFFF,        // لون السطح
    .onPrimary = 0xFFFFFFFF,      // لون النص على الأساسي
    .cornerRadiusMedium = 12.0f,  // نصف قطر الزوايا
    .rtl = true                   // اتجاه عربي
};
sadui_set_theme(&myTheme);
```

### 💫 نظام الرسوم المتحركة
```c
SadAnimation fadeIn = {
    .type = SAD_ANIM_FADE,        // نوع الحركة
    .duration = 300,              // المدة بالمللي ثانية
    .delay = 0,                   // التأخير
    .easing = SAD_EASE_OUT_CUBIC, // منحنى السهولة
    .startValue = 0.0f,           // القيمة الابتدائية
    .endValue = 1.0f,             // القيمة النهائية
    .loop = false,                // تكرار
    .reverse = false              // عكس
};

sadui_animate(widgetId, &fadeIn);
```

### ✨ أنواع الرسوم المتحركة
- `SAD_ANIM_FADE` - تلاشي
- `SAD_ANIM_SCALE` - تكبير/تصغير
- `SAD_ANIM_TRANSLATE` - انتقال
- `SAD_ANIM_ROTATE` - دوران
- `SAD_ANIM_COLOR` - تغيير اللون
- `SAD_ANIM_CUSTOM` - مخصص

### 📝 دعم النص العربي
```c
// تحميل خط عربي
SadFontHandle arabicFont = sad_font_load("/system/fonts/NotoNaskhArabic.ttf", 24);

// خيارات النص العربي
SadTextOptions opts = sad_text_options_arabic(20.0f);

// رسم نص عربي
sad_text_draw(arabicFont, "مرحباً بالعالم", x, y, &opts);
```

### 🎯 Canvas API للرسم المخصص
```c
// بدء الرسم
sadcanvas_begin(clipRect);

// رسم أشكال
sadcanvas_draw_rect(rect, color);
sadcanvas_draw_rect_rounded(rect, color, radius);
sadcanvas_draw_circle(cx, cy, radius, color);
sadcanvas_draw_line(x1, y1, x2, y2, color, width);
sadcanvas_draw_arc(cx, cy, r, startAngle, endAngle, color, width);

// رسم نص
sadcanvas_draw_text("نص", x, y, color, fontSize);

// رسم صورة
sadcanvas_draw_image(imageId, destRect);

// إنهاء الرسم
sadcanvas_end();
```

## الهيكل الملفي

```
tools/android/graphics/
├── sad_ui_engine.h           # API الرئيسي للواجهات
├── sad_ui_engine.c           # تنفيذ محرك OpenGL ES
├── sad_arabic_text.h         # API النصوص العربية
├── sad_arabic_text.c         # تنفيذ عرض النص العربي
├── sad_android_bridge_v2.h   # جسر Android المحسّن
└── sad_android_bridge_v2.c   # تنفيذ الجسر
```

## البناء والتكامل

### المتطلبات
- Android NDK r27c أو أحدث
- OpenGL ES 3.0
- CMake 3.18+

### البناء
```bash
# بناء المكتبة
cmake -B build -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24
cmake --build build --target sad_ui_android
```

### التكامل مع تطبيقات لغة ص
```c
// في ملف C الخاص بالتطبيق
#include "tools/android/graphics/sad_ui_engine.h"

// تهيئة
sadui_init(nativeWindow);

// إنشاء واجهة
SadWidgetId root = sadui_column();
SadWidgetId card = sadui_card();
sadui_add_child(root, card);

SadWidgetId title = sadui_heading("مرحباً");
sadui_add_child(card, title);

SadWidgetId btn = sadui_button("اضغطني");
sadui_on_click(btn, onClick, NULL);
sadui_add_child(card, btn);

sadui_set_root(root);

// حلقة الرسم
while (running) {
    sadui_handle_touch(&touchEvent);
    sadui_render(deltaTime);
}

// إغلاق
sadui_shutdown();
```

## المكونات المتاحة

| المكون | الوصف | الاستخدام |
|--------|-------|----------|
| `SAD_WIDGET_BUTTON` | زر مملوء | `sadui_button("نص")` |
| `SAD_WIDGET_OUTLINED_BUTTON` | زر محدد | `sadui_create(SAD_WIDGET_OUTLINED_BUTTON)` |
| `SAD_WIDGET_TEXT` | نص عادي | `sadui_text("نص")` |
| `SAD_WIDGET_HEADING` | عنوان | `sadui_heading("عنوان")` |
| `SAD_WIDGET_CARD` | بطاقة | `sadui_card()` |
| `SAD_WIDGET_COLUMN` | عمود | `sadui_column()` |
| `SAD_WIDGET_ROW` | صف | `sadui_row()` |
| `SAD_WIDGET_TEXT_FIELD` | حقل نص | `sadui_textfield("placeholder")` |
| `SAD_WIDGET_CHECKBOX` | مربع اختيار | `sadui_checkbox("تسمية", checked)` |
| `SAD_WIDGET_SLIDER` | منزلق | `sadui_slider(min, max, value)` |
| `SAD_WIDGET_PROGRESS` | شريط تقدم | `sadui_progress(value)` |
| `SAD_WIDGET_SCROLL_VIEW` | عرض قابل للتمرير | `sadui_scrollable(direction)` |

## معالجة الأحداث

```c
// النقر
sadui_on_click(widgetId, myClickHandler, userData);

// تغيير القيمة
sadui_on_value_change(sliderId, myValueHandler, userData);

// تغيير النص
sadui_on_text_change(textFieldId, myTextHandler, userData);

// اللمس المخصص
sadui_on_touch(widgetId, myTouchHandler, userData);

// الإيماءات
sadui_on_gesture(widgetId, myGestureHandler, userData);
```

## الترخيص

هذا النظام جزء من مشروع لغة ص مفتوح المصدر.

---

**الإصدار**: 1.0.0  
**التاريخ**: 2025  
**المؤلف**: فريق تطوير لغة ص
