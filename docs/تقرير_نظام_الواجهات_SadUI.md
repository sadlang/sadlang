# تقرير شامل: بنية نظام واجهات لغة ص (SadUI)

> **تاريخ التقرير**: 9 أبريل 2026  
> **الإصدار**: 1.0  
> **المؤلف**: تقرير تحليلي آلي

---

## الفهرس

1. [نظرة عامة](#1-نظرة-عامة)
2. [البنية الحالية](#2-البنية-الحالية)
3. [مسار البيانات](#3-مسار-البيانات)
4. [التمثيل الوسيط (IR)](#4-التمثيل-الوسيط-ir)
5. [العناصر (Widgets)](#5-العناصر-widgets)
6. [حالة التنفيذ](#6-حالة-التنفيذ)
7. [نظام الحالة](#7-نظام-الحالة)
8. [الخلفيات (Backends)](#8-الخلفيات-backends)
9. [UIParser — تحليل كود الواجهة](#9-uiparser--تحليل-كود-الواجهة)
10. [دمج الرسومات بأسلوب Flutter](#10-دمج-الرسومات-بأسلوب-flutter)
11. [المميزات والعيوب](#11-المميزات-والعيوب)
12. [جدول المقارنة مع Flutter](#12-جدول-المقارنة-مع-flutter)

---

## 1. نظرة عامة

نظام واجهات لغة ص (SadUI) هو نظام واجهات مستخدم متعدد المنصات مبني بـ C++17، مصمم بهندسة **الهجين الذكي (Smart Hybrid)**.

### الفكرة الأساسية
بدلاً من اختيار نهج واحد (كل شيء أصلي أو كل شيء Canvas)، النظام يختار تلقائياً أفضل طريقة لعرض كل عنصر:

| المنصة | العناصر القياسية | العناصر المخصصة |
|--------|------------------|-----------------|
| سطح المكتب | SDL2 + OpenGL | SDL2 Canvas |
| أندرويد | Jetpack Compose | Compose Canvas |
| iOS | SwiftUI | SwiftUI Canvas/Path |
| ويب | HTML5 + CSS3 | Canvas API |
| Bare Metal | Framebuffer | Framebuffer |

### حجم النظام
- **55+ ملف header** (.h)
- **59+ ملف تنفيذ** (.cpp)
- **114+ ملف C++** إجمالاً

### بنية المجلدات
```
sad_ui/
├── core/               النواة المشتركة (14 ملف .h + 14 ملف .cpp)
│   ├── types           الأنواع الأساسية (93 UINodeType + 50+ ModifierType)
│   ├── node            شجرة عناصر الواجهة
│   ├── ir              التمثيل الوسيط
│   ├── ir_builder      بنّاء IR مع التوجيه الذكي
│   ├── style           نظام الأنماط والسمات
│   ├── layout          محرك تخطيط Flexbox
│   ├── state           إدارة الحالة التفاعلية
│   ├── animation       نظام الرسوم المتحركة
│   ├── gesture         نظام الإيماءات
│   ├── scroll_physics  فيزياء التمرير
│   ├── form_validation التحقق من النماذج
│   ├── focus           إدارة التركيز
│   ├── image_cache     ذاكرة الصور
│   ├── notification    الإشعارات
│   ├── hybrid_routing  التوجيه الهجين الذكي
│   ├── theme           السمات
│   ├── system_bridge   جسر النظام المجرد
│   └── platform_renderer المُصيّر المجرد
│
├── backends/           مُصيّرات المنصات
│   ├── desktop/        SDL2 + OpenGL (مكتمل)
│   ├── android/        Jetpack Compose (مولّد كود)
│   ├── ios/            SwiftUI (مولّد كود)
│   ├── web/            HTML5/CSS3/JS (مولّد كود)
│   ├── macos/          AppKit (مولّد كود)
│   └── freestanding/   Framebuffer مباشر (مكتمل — لنظام أفق)
│
├── widgets/            عناصر واجهة جاهزة (11 ملف .cpp)
│   ├── text, button, image, column, row
│   ├── layout_widgets, input_widgets
│   ├── container_widgets, navigation_widgets
│   ├── feedback_widgets, advanced_widgets
│   ├── media/          عناصر وسائط (v3)
│   └── navigation_v2/  تنقل متقدم (v3)
│
└── pipeline/           خط الأنابيب الرئيسي
```

---

## 2. البنية الحالية

### 2.1 تعداد UINodeType — 93 نوع عنصر واجهة

مقسّمة إلى 16 فئة:

| # | الفئة | العدد | الأنواع |
|---|-------|-------|---------|
| 1 | عرض أساسي | 3 | `Text`, `Image`, `Icon` |
| 2 | إدخال | 10 | `Button`, `TextField`, `TextArea`, `Toggle`, `Slider`, `Picker`, `DatePicker`, `ColorPicker`, `Checkbox`, `Radio` |
| 3 | تخطيط | 10 | `Column`, `Row`, `Stack`, `Grid`, `LazyColumn`, `LazyRow`, `LazyGrid`, `Spacer`, `Divider`, `Wrap` |
| 4 | حاويات | 5 | `Container`, `Card`, `Surface`, `Scaffold`, `Box` |
| 5 | تنقل | 4 | `NavigationView`, `NavigationLink`, `TabView`, `TabItem` |
| 6 | قوائم | 3 | `List`, `Section`, `ForEach` |
| 7 | حوارات | 4 | `Alert`, `Sheet`, `Dialog`, `Menu` |
| 8 | متقدمة | 5 | `Canvas`, `MapView`, `WebView`, `VideoPlayer`, `ScrollView` |
| 9 | خاصة | 3 | `CustomWidget`, `Conditional`, `Group` |
| 10 | v2 — جديدة | 10 | `ProgressBar`, `Badge`, `Chip`, `Avatar`, `SearchBar`, `BottomSheet`, `FAB`, `SnackBar`, `Drawer`, `AppBar` |
| 11 | v3 — بيانات | 6 | `Tooltip`, `DataTable`, `TreeView`, `Breadcrumb`, `Pagination`, `Timeline` |
| 12 | v3 — وسائط | 6 | `Carousel`, `RichText`, `Markdown`, `CodeBlock`, `AudioPlayer`, `ImageGallery` |
| 13 | v3 — حركة | 6 | `Skeleton`, `Shimmer`, `AnimatedList`, `Expandable`, `Collapsible`, `Swipeable` |
| 14 | v3 — تحكم متقدم | 6 | `Stepper`, `SegmentedControl`, `RatingBar`, `TimePicker`, `Calendar`, `ColorWheel` |
| 15 | v3 — تنقل متقدم | 6 | `BottomNav`, `SideNav`, `Toolbar`, `SplitView`, `Tabs`, `MegaMenu` |
| 16 | v4 — تخطيط أساسي | 13 | `Center`, `Padding`, `SizedBox`, `Expanded`, `Flexible`, `Align`, `SafeArea`, `GestureDetector`, `InkWell`, `ListView`, `FractionallySizedBox`, `ConstrainedBox`, `AspectRatio` |

### 2.2 تعداد ModifierType — +50 معدّل

| الفئة | المعدّلات |
|-------|---------|
| ألوان (4) | `ForegroundColor`, `BackgroundColor`, `TintColor`, `GradientColor` |
| خط/نصوص (7) | `FontSize`, `FontWeight`, `FontFamily`, `FontStyle`, `TextAlign`, `LineSpacing`, `TextDecoration` |
| أبعاد (6) | `Width`, `Height`, `MinWidth`, `MinHeight`, `MaxWidth`, `MaxHeight` |
| تباعد (6) | `Padding`, `PaddingTop/Bottom/Leading/Trailing`, `Margin` |
| حدود (4) | `Border`, `BorderRadius`, `BorderWidth`, `BorderColor` |
| محاذاة (4) | `Alignment`, `Frame`, `Position`, `Offset` |
| تأثيرات (3) | `Shadow`, `Opacity`, `Blur` |
| تحويل (2) | `Rotation`, `Scale` |
| تفاعل (5) | `OnTap`, `OnLongPress`, `OnDrag`, `OnAppear`, `OnDisappear` |
| حالة (2) | `Disabled`, `Hidden` |
| رسوم (2) | `Animation`, `Transition` |
| متنوعة (4) | `ClipShape`, `ZIndex`, `Accessibility`, `Tag` |

---

## 3. مسار البيانات

```
كود ص (.ص)
     ↓
UIParser (shared/parser/src/ui_parser.cpp)
  - يبحث عن كلمة "واجهة" بـ regex
  - يستخرج @حالة/@مراقب/@بيئة/@ربط
  - يحلل دالة عرض()
  - يبني شجرة UINode
     ↓
UINode Tree (sad_ui/core/include/sad_ui/node.h)
  - شجرة عقد: نوع + وسائط + معدّلات + أبناء + ربط حالة
     ↓
IRBuilder (sad_ui/core/include/sad_ui/ir_builder.h)
  - تحويل Modifier → IRProperty/IREvent
  - تحديد RenderingHint (Native/Canvas/Hybrid/Auto)
  - تحليل روابط الحالة → IRStateRef
  - تحسين الشجرة (حذف فارغ، دمج معدّلات)
     ↓
IR Module (sad_ui/core/include/sad_ui/ir.h)
  - شجرة IRNode مبسطة
  - خصائص + أحداث + إشارات حالة
     ↓
StyleResolver (sad_ui/core/include/sad_ui/style.h)
  - حل المعدّلات → ResolvedStyle
  - تطبيق الوراثة (لون الخط يرث، الحشوة لا)
  - تتالي: معدّل مباشر > نمط مسمّى > سمة > افتراضي
     ↓
LayoutEngine (sad_ui/core/include/sad_ui/layout.h)
  - Flexbox مبسط بمرحلتين: قياس (↑) + توزيع (↓)
  - دعم RTL أصلي
     ↓
Backend حسب الوضع:
  ├── DirectRender: DesktopRenderer (SDL2+GL) → الشاشة
  ├── CodeGen:      ComposeCodegen → ملف .kt
  ├── CodeGen:      SwiftUICodegen → ملف .swift
  ├── CodeGen:      HtmlCodegen → ملف .html
  ├── CodeGen:      AppKitCodegen → ملف .swift
  └── Freestanding:  → framebuffer مباشرة
```

---

## 4. التمثيل الوسيط (IR)

### 4.1 بنية IRNode

| المكوّن | النوع | الوصف |
|---------|-------|-------|
| `type_` | `UINodeType` | نوع العنصر |
| `hint_` | `RenderingHint` | Auto / Native / Canvas / Hybrid |
| `platform_` | `TargetPlatform` | المنصة المستهدفة |
| `properties_` | `vector<IRProperty>` | خصائص (key-value) |
| `events_` | `vector<IREvent>` | أحداث (نوع + تعبير + متغيرات متأثرة) |
| `stateRefs_` | `vector<IRStateRef>` | إشارات حالة |
| `children_` | `vector<shared_ptr<IRNode>>` | أبناء |

### 4.2 IRProperty
```cpp
using Value = std::variant<int64_t, double, bool, std::string>;
struct IRProperty { string key; Value value; };
```

### 4.3 IREvent
```cpp
struct IREvent {
    string eventType;                    // "عند_النقر"
    string expression;                   // "عداد += 1"
    vector<string> modifiedStates;       // ["عداد"]
};
```

### 4.4 IRStateRef
```cpp
struct IRStateRef {
    string stateName;           // "عداد"
    string propertyBinding;     // "نص"
    StateBindingType bindingType;
};
```

### 4.5 RenderingHint
```cpp
enum class RenderingHint : uint8_t {
    Auto,    // المولّد يقرر تلقائياً
    Native,  // استخدم Widget أصلي
    Canvas,  // ارسم يدوياً
    Hybrid   // جزء أصلي + جزء Canvas
};
```

### 4.6 IRBuilder
- **المدخل**: `ViewDefinition` (اسم + حالات + شجرة UINode)
- **المخرج**: `IRModule` (شجرة IR + تعريفات حالة)
- يستخدم `HybridRouter` لتحديد هل العنصر أصلي/Canvas/هجين بناءً على المنصة
- **مثال التحويل**:
  ```
  UINode(Text, "مرحباً", [FontSize=24, Color=Red])
    ↓ IRBuilder
  IRNode(Text, props={text:"مرحباً", fontSize:24, color:"#FF0000"}, hint=Native)
  ```

---

## 5. العناصر (Widgets)

### 5.1 بانيون (Builders) برمجيّون

كل عنصر له Builder بنمط Method Chaining:

| الملف | البانيون | الخصائص |
|-------|---------|---------|
| text.h | `TextBuilder` | fontSize, color, fontWeight, fontFamily, textAlign, maxLines, lineSpacing |
| button.h | `ButtonBuilder` | backgroundColor, textColor, fontSize, cornerRadius, padding, onTap, onLongPress, disabled |
| column.h | `ColumnBuilder` | alignment, spacing, children |
| row.h | `RowBuilder` | alignment, spacing, children |
| image.h | `ImageBuilder` | src, fit, size |
| layout_widgets.h | `StackBuilder`, `GridBuilder`, `LazyColumnBuilder`, `LazyRowBuilder` | alignment, columns, spacing, items |
| input_widgets.h | `ToggleBuilder`, `SliderBuilder`, `PickerBuilder`, `CheckboxBuilder` | value, min/max, tintColor, onChange |
| container_widgets.h | حاويات متنوعة | padding, elevation, shape |
| navigation_widgets.h | تنقل | destination, title |
| feedback_widgets.h | ملاحظات | message, duration |
| advanced_widgets.h | متقدمة | متنوعة |

### 5.2 الكلمات العربية للعناصر في ui_parser.cpp

44 عنصر مسجّل مع أسماء عربية:
```
نص, صورة, أيقونة, زر, حقل_نص, منطقة_نص, تبديل, منزلق, منتقي,
منتقي_تاريخ, منتقي_لون, عمود, صف, رصة, شبكة, عمود_كسول,
صف_كسول, شبكة_كسولة, فاصل, فاصل_خط, حاوية, بطاقة, سطح,
هيكل, صندوق, عرض_تنقل, رابط_تنقل, عرض_أقسام, قسم, قائمة,
قسم_قائمة, لكل, تنبيه, ورقة, حوار, قائمة_خيارات, لوحة,
خريطة, عرض_ويب, مشغل_فيديو, منشئ_واجهة, واجهة_مخصصة, شرطي, مجموعة
```

### 5.3 المعدّلات العربية

45+ معدّل مسجّل مع أسماء عربية:
```
لون, لون_خلفية, لون_صبغة, حجم_خط, سمك_خط, عائلة_خط, نمط_خط,
تباعد_أسطر, عرض, ارتفاع, أدنى_عرض, أدنى_ارتفاع, أقصى_عرض,
أقصى_ارتفاع, حشوة, حشوة_علوية, حشوة_سفلية, حشوة_بداية,
حشوة_نهاية, هامش, حد, انحناء_حد, سمك_حد, لون_حد, محاذاة,
إطار, موقع, إزاحة, ظل, عتامة, ضبابية, دوران, مقياس,
عند_النقر, عند_الضغط_المطول, عند_السحب, عند_الظهور, عند_الاختفاء,
معطل, مخفي, رسوم_متحركة, انتقال, شكل_قص, مستوى_ع,
إمكانية_وصول, علامة
```

### 5.4 الألوان المسماة

22 لون عربي:
```
أحمر, أخضر, أزرق, أصفر, برتقالي, بنفسجي, وردي, بني,
أبيض, أسود, رمادي, رمادي_فاتح, رمادي_غامق,
أساسي (iOS Blue), ثانوي (iOS Gray), نجاح (iOS Green),
تحذير (iOS Yellow), خطر (iOS Red), شفاف
```

---

## 6. حالة التنفيذ

### 6.1 مكتمل (Headers + Implementation .cpp)

| المكوّن | الملفات | الوصف |
|---------|---------|-------|
| الأنواع الأساسية | types.h/.cpp | 93 UINodeType + 50+ ModifierType |
| شجرة UINode | node.h/.cpp | UINode كامل مع Builder |
| نظام IR | ir.h/.cpp | IRNode + IRModule + IRProperty + IREvent + IRStateRef |
| IRBuilder | ir_builder.h/.cpp | تحويل + توجيه ذكي |
| نظام الأنماط | style.h/.cpp | ResolvedStyle + StyleResolver + Theme |
| محرك التخطيط | layout.h/.cpp | Flexbox بمرحلتين + RTL |
| إدارة الحالة | state.h/.cpp | StateStore + Observer + Computed + Batching |
| خط الأنابيب | pipeline.h/.cpp | CodeGen + DirectRender |
| UIParser | ui_parser.cpp | تحليل regex كامل |
| Widgets (11 .cpp) | widgets/*.cpp | text, button, column, row, image, layout, input, container, navigation, feedback, advanced |
| 8 أنظمة مساعدة | core/*.cpp | animation, gesture, scroll_physics, form_validation, focus, image_cache, notification, hybrid_routing, theme |
| Desktop Backend | desktop/*.h/.cpp | SDL2+OpenGL renderer مع text_editing و window |
| Freestanding Backend | freestanding/*.h/.cpp | رسم مباشر على framebuffer |

### 6.2 هيكلي (Headers + مولّدات كود غير مكتملة)

| المكوّن | الحالة |
|---------|--------|
| Android Backend | compose_codegen.h — يولّد Kotlin/Compose |
| iOS Backend | swiftui_codegen.h — يولّد Swift/SwiftUI |
| Web Backend | html_codegen.h — يولّد HTML/CSS/JS |
| macOS Backend | appkit_codegen.h — يولّد AppKit |

---

## 7. نظام الحالة

### 7.1 أنواع الربط

| النوع | الكلمة في كود ص | الوصف |
|-------|----------------|-------|
| `State` | `@حالة` | متغير تفاعلي محلي |
| `Binding` | `@ربط` | إشارة لحالة واجهة أخرى |
| `Observed` | `@مراقب` | حالة خارجية مُراقبة |
| `Environment` | `@بيئة` | قيمة عامة مشتركة |
| `Computed` | `@محسوب` | قيمة مشتقة تلقائياً |

### 7.2 آلية العمل

1. **StateStore** — مخزن مركزي لكل واجهة
2. **StateEntry** — يحتوي: قيمة (`std::any`), مراقبين (`weak_ptr<StateObserver>`), دالة حساب, تبعيات
3. **نمط Observer**: عند `set()` → إخطار المراقبين → إعادة حساب المحسوبة → إعادة رسم المتأثرة فقط
4. **تجميع (Batching)**: `beginBatch()`/`endBatch()` — تجمع تغييرات متعددة وتُخطر مرة واحدة
5. **تنظيف تلقائي**: weak_ptr منتهية تُحذف تلقائياً

### 7.3 مثال
```cpp
StateStore store;
store.define("عداد", "رقم", StateBindingType::State, 0);
store.addObserver("عداد", myTextWidget);

store.beginBatch();
store.set("اسم", "أحمد");
store.set("عمر", 25);
store.endBatch();  // إخطار واحد لكل المراقبين
```

---

## 8. الخلفيات (Backends)

### 8.1 Desktop (SDL2 + OpenGL) — ✅ مكتمل

- `DesktopRenderer` يرث `PlatformRenderer` (الواجهة المجردة)
- رسم مباشر: خلفية → حدود → محتوى → أبناء
- يدعم: antialiasing, vsync, scaleFactor, debugBounds
- ملفات: renderer.h, window.h, text_editing.h, renderer_internal.h, system_bridge.h

### 8.2 Android (Compose) — 🔶 مولّد كود

- `ComposeCodegen` — يُنتج Kotlin مع:
  - `@Composable` functions
  - `mutableStateOf()` + `remember`
  - Material3 theme
  - `@Preview`
- يدعم التوجيه الذكي: Native → Composable أصلي, Canvas → `Canvas { drawRect(...) }`

### 8.3 iOS (SwiftUI) — 🔶 مولّد كود

- `SwiftUICodegen` — يُنتج Swift مع:
  - `struct: View` + `@State`
  - `VStack/HStack/ZStack`
  - `#Preview`

### 8.4 Web (HTML/CSS/JS) — 🔶 مولّد كود

- `HtmlCodegen` — يُنتج HTML5 مع:
  - CSS Flexbox
  - `<canvas>` API
  - JavaScript للحالة
  - `dir="rtl" lang="ar"` تلقائياً

### 8.5 macOS (AppKit) — 🔶 مولّد كود

- `AppKitCodegen` — يُنتج AppKit/Swift

### 8.6 Freestanding (Bare Metal) — ✅ مكتمل

- `FreestandingRenderer` — رسم مباشر على framebuffer
- لا يعتمد على SDL2 أو أي مكتبة خارجية
- يدعم: خطوط نقطية (bitmap), double buffering, alpha blending, RTL, clipping
- مُصمم لنظام أفق (Ufuq OS) و UEFI + QEMU

### 8.7 جسر النظام (System Bridge)

واجهة مجردة تربط 8 أنظمة بكل backend:

| # | النظام | الوصف |
|---|--------|-------|
| 1 | ThemeBridge | السمات (داكن/فاتح) |
| 2 | GestureBridge | الإيماءات (نقر، سحب) |
| 3 | ScrollPhysics | فيزياء التمرير |
| 4 | Accessibility | إمكانية الوصول |
| 5 | FormValidation | التحقق من النماذج |
| 6 | Focus | إدارة التركيز |
| 7 | ImageCache | ذاكرة الصور |
| 8 | Notification | الإشعارات |

### 8.8 المُصيّر المجرد (PlatformRenderer)

~20 دالة رسم أساسية يجب تطبيقها في كل منصة:
```
drawFilledRect, drawRoundedRect, drawRectOutline, drawText,
measureText, drawImage, drawCircle, drawCircleOutline, drawLine,
drawLinearGradient, setClipRect, clearClipRect, loadFont,
beginFrame, endFrame, initialize, shutdown, isReady
```

منطق رسم الـ widgets (30+ عنصر) **مشترك** عبر دالة `renderNode()`.

---

## 9. UIParser — تحليل كود الواجهة

### 9.1 المراحل الخمس

| المرحلة | الوصف | التقنية |
|---------|-------|---------|
| 1 | البحث عن "واجهة" | `regex: واجهة\s+(\S+)\s*\{` |
| 2 | استخراج المحتوى | عد أقواس `{}` |
| 3 | تحليل الحالة | `regex: @(حالة|مراقب|بيئة|ربط)\s+(\w+)\s*:\s*(\w+)` |
| 4 | البحث عن عرض() | `regex: عرض\s*\(\s*\)\s*\{` |
| 5 | تحليل الجسم | `parseNodeFromSource()` — يبني شجرة UINode |

### 9.2 الخرائط المُعرّفة
- 44 نوع عقدة مع أسماء عربية
- 45+ معدّل مع أسماء عربية
- 22 لون مسمّى عربي
- أنواع أبعاد: مالئ, ملائم, تلقائي, نقاط, نسبة مئوية
- محاذاة: 9 أنواع

### 9.3 التحقق من التوافق
- معدّلات الخط فقط مع Text/TextField/TextArea/Button
- أحداث النقر ممنوعة على Spacer/Divider

---

## 10. دمج الرسومات بأسلوب Flutter

### 10.1 الوضع الحالي: نهجان مختلفان

**النهج الحتمي (Imperative)** — الموجود في مهام.ص:
```sad
دالة صفحة_المهام()
    ارجع جديد صفحة(
        جديد عمود([
            جديد شريط_تطبيق("مهامي"),
            جديد نص("مرحبا", حجم_الخط: 18),
            جديد زر("أضف", عند_الضغط: لامدا() => ...)
        ])
    )
نهاية
```

**النهج التصريحي (Declarative)** — المُعرّف في UIParser:
```sad
واجهة عداد {
    @حالة عدد: رقم = 0
    عرض() {
        عمود {
            نص("العدد: \(عدد)").حجم_خط(24).لون(.أزرق)
            زر("زد").عند_النقر { عدد += 1 }
        }
    }
}
```

### 10.2 النهج المقترح — أسلوب Flutter بالعربية

```sad
واجهة تطبيق_المهام
    @حالة مهام: مصفوفة = []
    @حالة عداد: رقم = 0

    دالة بناء()
        ارجع هيكل(
            شريط_علوي: شريط_تطبيق(عنوان: "مهامي"),
            جسم: وسط(
                ابن: عمود(
                    محاذاة_رئيسية: .وسط،
                    تباعد: 12،
                    أبناء: [
                        نص("العدد: " + عداد،
                            نمط: نمط_نص(حجم: 32، سمك: .سميك)
                        )،
                        فاصل(ارتفاع: 20)،
                        صف(
                            تباعد: 16،
                            أبناء: [
                                زر_عائم(
                                    ابن: أيقونة("ناقص")،
                                    عند_الضغط: لامدا() => عداد -= 1
                                )،
                                زر_عائم(
                                    ابن: أيقونة("زائد")،
                                    عند_الضغط: لامدا() => عداد += 1
                                )
                            ]
                        )
                    ]
                )
            )
        )
    نهاية
نهاية
```

### 10.3 جدول التوجيه الهجين الذكي

| العنصر | Desktop | Android | iOS | Web |
|--------|---------|---------|-----|-----|
| Text | Canvas | **Compose** | **SwiftUI** | Canvas |
| Button | Canvas | **Compose** | **SwiftUI** | Canvas |
| TextField | SDL2+IME | **Compose** | **SwiftUI** | Canvas |
| Slider | Canvas | **Compose** | **SwiftUI** | Canvas |
| Image | Canvas | Canvas | Canvas | Canvas |
| Custom | Canvas | Canvas | Canvas | Canvas |

---

## 11. المميزات والعيوب

### 11.1 المميزات (نقاط القوة)

| # | الميزة | الوصف |
|---|--------|-------|
| 1 | **التوجيه الهجين الذكي** | يختار تلقائياً Native أم Canvas لكل عنصر حسب المنصة — ميزة فريدة لا تملكها Flutter |
| 2 | **93 عنصر واجهة** | أكثر من Flutter (~80 widget) |
| 3 | **RTL أصلي** | الاتجاه من اليمين لليسار هو الافتراضي — ليس إضافة لاحقة |
| 4 | **إدارة حالة مدمجة** | `@حالة` + تعديل مباشر + Batching + Computed — أبسط من Flutter |
| 5 | **توليد كود أصلي** | من كود واحد يُنتج: Kotlin + Swift + HTML — Flutter لا يولّد كود أصلي |
| 6 | **Backend Freestanding** | يعمل على bare-metal بدون نظام تشغيل — لا مثيل في Flutter |
| 7 | **Modifier Chaining** | نمط مألوف لمطوري SwiftUI |
| 8 | **8 أنظمة مساعدة** | Animation, Gesture, Scroll, Accessibility, Form, Focus, ImageCache, Notification — كلها مدمجة |

### 11.2 العيوب (نقاط الضعف)

| # | العيب | الخطورة | الحل المقترح |
|---|-------|---------|-------------|
| 1 | **غير مربوط بالمفسر** | ❌ حرجة | إضافة دوال مدمجة (`شغّل_واجهة`) تربط المفسر بـ UIPipeline |
| 2 | **UIParser يعتمد على Regex** | ⚠️ عالية | دمج تحليل الواجهة في ParserCore كعقد AST |
| 3 | **لا يوجد Hot Reload** | ⚠️ متوسطة | مراقبة ملفات + إعادة تحليل + Diffing |
| 4 | **DesktopRenderer بدون GPU** | ⚠️ متوسطة | الانتقال إلى OpenGL مباشر أو Vulkan |
| 5 | **مولّدات الكود هيكلية** | ⚠️ متوسطة | إكمال تنفيذ ComposeCodegen / HtmlCodegen |
| 6 | **لا يوجد Navigation Stack** | ⚠️ متوسطة | تنفيذ NavigationStack مع history |
| 7 | **لا يوجد DevTools** | ⚠️ منخفضة | بناء Widget Inspector مشابه لـ Flutter |
| 8 | **لا يوجد نظام حزم widgets** | ⚠️ منخفضة | دعم حزم UI في مدير الحزم |
| 9 | **Accessibility هيكلي** | ⚠️ منخفضة | إكمال تنفيذ Screen Readers |

---

## 12. جدول المقارنة مع Flutter

| الخاصية | Flutter | SadUI الحالي | SadUI المأمول |
|---------|---------|-------------|--------------|
| عدد Widgets | ~80 | **93** ✅ | 93+ |
| أسلوب الكتابة | Dart OOP | نهجان (حتمي + تصريحي) | Flutter-like عربي |
| Hot Reload | ✅ | ❌ | ممكن |
| GPU Rendering | Skia/Impeller | SDL2 (2D) | OpenGL/Vulkan |
| Native Widgets | ❌ (يرسم كل شيء) | ✅ Hybrid Routing | ✅ |
| توليد كود أصلي | ❌ | ✅ (Compose/SwiftUI/HTML) | ✅ |
| RTL أصلي | إضافة | ✅ افتراضي | ✅ |
| إدارة الحالة | خارجية (Riverpod/Bloc) | ✅ مدمجة | ✅ |
| مربوط بالمفسر | ✅ (Dart VM) | ❌ | يحتاج ربط |
| Bare Metal | ❌ | ✅ (Freestanding) | ✅ |
| DevTools | ✅ ممتاز | ❌ | ممكن |
| إمكانية الوصول | ✅ | 🔶 هيكلي | يحتاج تنفيذ |

---

## الخلاصة

نظام SadUI بنية **طموحة ومصممة بشكل ممتاز معمارياً** — 114+ ملف C++ مع فصل نظيف بين الطبقات. الميزة الفريدة التي لا تملكها أي أداة أخرى هي **التوجيه الهجين الذكي** + **RTL أصلي** + **backend مستقل (bare metal)**.

**الحلقة المفقودة الأساسية**: الربط بالمفسر — لا يمكن حالياً كتابة `واجهة { ... }` في ملف `.ص` وتشغيلها مباشرة من المفسر.
