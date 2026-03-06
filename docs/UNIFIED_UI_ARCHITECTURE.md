# معمارية النظام الموحد للواجهات — SadUI Unified Architecture

## النظرة العامة

طبقة التجريد الموحدة (SadUI Unified) تحل مشكلة وجود **ثلاثة أنظمة واجهات منفصلة** لا تتحدث مع بعضها:

```
قبل:   كود ص للسطح → sad::ui (SDL2) ✓
        كود ص للأندرويد → sad_ui_engine (GLES3) ✓  (لكن بأسماء مختلفة!)
        كود ص للويب → لا يوجد ✗

بعد:    كود ص واحد → sad::unified → Backend المناسب تلقائياً
                                     ├── DesktopBackend (SDL2)
                                     ├── AndroidBackend (EGL+GLES3)
                                     └── WasmBackend (Canvas2D)
```

## مسار البيانات

```
┌─────────────────────────────────────────────────────────────────┐
│   كود لغة ص (.ص)                                               │
│   اطبع_سطر("المنصة: " + المنصة())                                │
│   متغير تطبيق = أنشئ_تطبيق("تطبيقي"، 800، 600)                  │
│   متغير زر1 = زر("اضغطني")                                      │
│   اضف(عمود_رئيسي، زر1)                                          │
└─────────────────────┬───────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────────┐
│   المفسر (Interpreter)                                          │
│   builtin_registry_part11-18 → sad::ui::*                       │
│   builtin_registry_part39    → sad::unified::*                  │
└─────────────────────┬───────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────────┐
│   SadUI Singleton (sad::unified::SadUI)                         │
│   ┌─────────────────────┐                                       │
│   │ SadUIBackend* active │ → يُختار تلقائياً عند initPlatformUI │
│   └─────────┬───────────┘                                       │
│             │                                                    │
│   ┌─────────┼──────────┬────────────────┐                       │
│   ▼         ▼          ▼                │                       │
│ Desktop   Android    WASM              │                       │
│ Backend   Backend    Backend            │                       │
│ (SDL2)    (GLES3)    (Canvas2D)         │                       │
└─────────────────────────────────────────────────────────────────┘
```

## هيكل الملفات

```
stdlib/ui/
├── sad_ui_unified.h          الواجهة المجرّدة الرئيسية
│                              - WidgetType enum (~140 نوع)
│                              - SadUIBackend (abstract class)
│                              - SadUI singleton
│                              - دوال مختصرة (ui_button, ui_column, ...)
│
├── sad_ui_unified.cpp         تنفيذ SadUI + الدوال المختصرة
│
├── sad_backend_desktop.h      تعريف DesktopBackend
├── sad_backend_desktop.cpp    محوّل: unified → sad::ui (SDL2)
│                              - toLegacy() يحوّل ~100 نوع
│                              - يستدعي widget_button(), widget_text(), ...
│
├── sad_backend_android.h      تعريف AndroidBackend
├── sad_backend_android.cpp    محوّل: unified → sadui_* (C API)
│                              - toAndroid() يحوّل ~90 نوع
│                              - يخزّن SadLayout/SadStyle لكل widget
│                              - يستدعي sadui_create(), sadui_set_style(), ...
│
├── sad_backend_wasm.h         تعريف WasmBackend
├── sad_backend_wasm.cpp       محوّل: unified → JavaScript Canvas2D
│                              - يرسم عبر EM_ASM macros
│                              - تخطيط Flexbox مبسط داخلي
│                              - النقر عبر addEventListener
│
├── sad_ui_platform.h          كشف المنصة تلقائياً
├── sad_ui_platform.cpp        initPlatformUI() → يختار Backend
│                              __ANDROID__ → AndroidBackend
│                              __EMSCRIPTEN__ → WasmBackend
│                              else → DesktopBackend
│
└── sad_ui_compat.h            طبقة التوافق (للانتقال التدريجي)
```

## كيفية إضافة Backend جديد

لإضافة منصة جديدة (مثلاً iOS أو Vulkan):

### 1. أنشئ الملفات
```
stdlib/ui/sad_backend_ios.h
stdlib/ui/sad_backend_ios.cpp
```

### 2. وارث من SadUIBackend
```cpp
#include "sad_ui_unified.h"

namespace sad { namespace unified {

class iOSBackend : public SadUIBackend {
public:
    std::string backendName() const override { return "iOS-UIKit"; }
    
    int  appCreate(const std::string& title, int w, int h) override;
    bool appIsRunning(int appId) override;
    void appUpdate(int appId) override;
    // ... نفّذ جميع الدوال المجرّدة الـ 25+
    
    WidgetId createWidget(WidgetType type) override;
    void setText(WidgetId id, const std::string& text) override;
    // إلخ...
};

}} // namespace
```

### 3. أضف الكشف في sad_ui_platform.cpp
```cpp
#ifdef __APPLE__
  #include <TargetConditionals.h>
  #if TARGET_OS_IPHONE
    #include "sad_backend_ios.h"
    // في initPlatformUI():
    ui.setBackend(std::make_unique<iOSBackend>());
  #endif
#endif
```

### 4. أضف للـ CMakeLists.txt
```cmake
if(IOS)
    target_sources(sad_unified PRIVATE
        stdlib/ui/sad_backend_ios.cpp)
endif()
```

## أنواع الـ Widget الموحدة (140 نوع)

| الفئة | الأنواع |
|-------|---------|
| حاويات | Container, Column, Row, Stack, Center, Wrap, ScrollView, Grid, ListView, ... |
| نصوص | Text, Label, Heading, Paragraph, Link, CodeBlock, RichText, Markdown |
| أزرار | Button, IconButton, FloatingButton, OutlineButton, TextButton, SpeedDial |
| إدخال | TextField, PasswordField, TextArea, Checkbox, Radio, Switch, Slider, Dropdown, DatePicker, ColorPicker, SignaturePad, ... |
| عرض | Icon, Image, Avatar, Badge, Chip, Divider, Spacer, ProgressBar, CircleProgress, Gauge |
| تنقل | AppBar, BottomBar, TabBar, Drawer, Breadcrumb, Toolbar |
| حوارات | Dialog, Alert, Toast, Snackbar, Modal, BottomSheet, ContextMenu |
| بيانات | ListTile, DataTable, DataGrid, TreeView, Timeline, Stepper, Accordion |
| مخططات | BarChart, PieChart, LineChart, RadarChart, Heatmap, Waveform |
| وسائط | VideoPlayer, AudioPlayer, CameraPreview, VoiceRecorder, LottieAnimation |
| متقدم | Canvas, Carousel, WebView, PDFViewer, Map, QRScanner, CodeEditor |

## حالة التقدم

| المرحلة | الحالة | الوصف |
|---------|--------|-------|
| ✅ الواجهة المجرّدة | **مكتمل** | sad_ui_unified.h — 140 نوع + SadUIBackend |
| ✅ Desktop Backend | **مكتمل** | محوّل كامل يلف sad::ui (SDL2) |
| ✅ Android Backend | **مكتمل** | محوّل يلف sadui_* (C API) |
| ✅ WASM Backend | **مكتمل** | محرك رسم Canvas2D + تخطيط داخلي |
| ✅ كشف المنصة | **مكتمل** | initPlatformUI() تلقائي |
| ✅ Part39 في المفسر | **مكتمل** | المنصة(), هل_اندرويد(), اسم_المحرك() |
| ✅ مثال عملي | **مكتمل** | examples/unified/اختبار_موحد.ص |
| 🔄 ربط builtins الحالية | **قيد العمل** | تحويل part13-16 للاستخدام عبر الموحد |
| ⬜ دمج CMake | **لم يبدأ** | إضافة stdlib/ui/ لنظام البناء |
| ⬜ اختبار شامل | **لم يبدأ** | بناء وتشغيل على Desktop |
| ⬜ اختبار Android | **لم يبدأ** | بناء APK وتشغيل |
| ⬜ اختبار WASM | **لم يبدأ** | بناء Emscripten وتشغيل |

## المراحل التالية (الخريطة)

```
المرحلة 1 (الحالية): بناء الطبقة الموحدة ← مكتمل ✅
المرحلة 2: ربط builtins + CMake ← قيد العمل 🔄
المرحلة 3: اختبار سطح المكتب
المرحلة 4: بناء APK أندرويد
المرحلة 5: بناء WASM/Web
المرحلة 6: رسوم متحركة موحدة + ثيمات
المرحلة 7: Hot Reload عبر المنصات
```
