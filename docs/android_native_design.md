# وثيقة تصميم شاملة: دعم أندرويد المستقل بالكامل في لغة ص

> **المسار:** الاستقلال الكامل — بدون Java/Kotlin
> **الهدف النهائي:** تطبيق محادثة مكتوب بلغة ص فقط، مُترجَم إلى كود أصلي ARM64
> **التاريخ:** مارس 2026

---

## ١. الرؤية

تحويل لغة ص إلى **اللغة الأولى** لكتابة تطبيقات أندرويد بالكامل — بدون سطر واحد من Java أو Kotlin أو XML. المطور يكتب كوداً بالعربية فقط، والمترجم `sadc` يُنتج APK أصلي يعمل مباشرة على أي جهاز أندرويد.

```sad
استورد واجهة من "أندرويد/واجهة"
استورد شبكة من "أندرويد/شبكة"

دالة الرئيسية()
    متغير تطبيق = تطبيق_جديد("محادثة ص")

    تطبيق.واجهة(
        عمود(
            شريط_عنوان("💬 محادثة ص")
            قائمة_رسائل(تطبيق.رسائل)
            صف(
                حقل_إدخال(تلميح: "اكتب رسالة...")
                زر("إرسال"، عند_الضغط: تطبيق.أرسل)
            نهاية
        نهاية
    نهاية

    تطبيق.شغّل()
نهاية
```

---

## ٢. البنية المعمارية الكاملة

### ٢.١ خط الأنابيب من الكود إلى APK

```
┌─────────────────────────────────────────────────────────────────┐
│                    كود .ص (المطور يكتب بالعربية)                │
└──────────────────────────┬──────────────────────────────────────┘
                           ▼
┌──────────────────────────────────────────────────────────────────┐
│  المرحلة ١: التحليل (موجود)                                     │
│  LexerCore → Tokens → ParserCore → AST                          │
└──────────────────────────┬──────────────────────────────────────┘
                           ▼
┌──────────────────────────────────────────────────────────────────┐
│  المرحلة ٢: التمثيل الوسيط (موجود)                              │
│  SIRBuilder → SIR Module (12 عملية ملكية + ذاكرة + تحكم)        │
│  SIROptimizer → تحسينات (طي ثوابت، حذف كود ميت، إلخ)           │
└──────────────────────────┬──────────────────────────────────────┘
                           ▼
┌──────────────────────────────────────────────────────────────────┐
│  المرحلة ٣: توليد LLVM IR (موجود جزئياً)                        │
│  LLVMCodeGen → LLVM IR                                           │
│  LLVMOptimizer → تحسينات LLVM (O0-O3, Os, Oz)                   │
│  ⚠ مطلوب: تفعيل هدف aarch64-linux-android                       │
└──────────────────────────┬──────────────────────────────────────┘
                           ▼
┌──────────────────────────────────────────────────────────────────┐
│  المرحلة ٤: التجميع والربط (مطلوب بناء)                         │
│  LLVM Target Machine → .o (ARM64)                                │
│  NDK Linker (LLD) → libsad_app.so                                │
│  + libsad_runtime.so (Runtime أصلي)                              │
│  + libsad_ui.so (محرك واجهات)                                    │
└──────────────────────────┬──────────────────────────────────────┘
                           ▼
┌──────────────────────────────────────────────────────────────────┐
│  المرحلة ٥: تحزيم APK (مطلوب بناء)                              │
│  AndroidManifest.xml (NativeActivity)                             │
│  lib/arm64-v8a/libsad_app.so                                     │
│  lib/arm64-v8a/libsad_runtime.so                                 │
│  lib/arm64-v8a/libsad_ui.so                                      │
│  assets/fonts/arabic.ttf                                          │
│  → aapt2 → zipalign → apksigner → APK نهائي                     │
└──────────────────────────────────────────────────────────────────┘
```

### ٢.٢ البنية في وقت التشغيل (على الجهاز)

```
┌─────────────────────────────────────────────────────────────────┐
│                        نظام أندرويد                              │
│  ┌───────────────────────────────────────────────────────────┐   │
│  │ NativeActivity (بدون Java)                                │   │
│  │  ┌─────────────────────────────────────────────────────┐  │   │
│  │  │ libsad_app.so (كود المستخدم المُترجَم)              │  │   │
│  │  │  ├── الرئيسية()                                     │  │   │
│  │  │  ├── واجهات مخصصة                                   │  │   │
│  │  │  └── منطق التطبيق                                   │  │   │
│  │  └──────────────┬──────────────────────────────────────┘  │   │
│  │                 │ يستدعي                                  │   │
│  │  ┌──────────────▼──────────────────────────────────────┐  │   │
│  │  │ libsad_runtime.so (Runtime أصلي)                    │  │   │
│  │  │  ├── إدارة ذاكرة (Arena + RC/GC مختلط)              │  │   │
│  │  │  ├── أنواع بيانات (SadString, SadArray, SadMap)     │  │   │
│  │  │  ├── نظام استثناءات                                 │  │   │
│  │  │  ├── خيوط (pthreads/bionic)                         │  │   │
│  │  │  ├── شبكة (BSD sockets + BoringSSL)                 │  │   │
│  │  │  └── نظام ملفات + أصول                              │  │   │
│  │  └──────────────┬──────────────────────────────────────┘  │   │
│  │                 │ يستدعي                                  │   │
│  │  ┌──────────────▼──────────────────────────────────────┐  │   │
│  │  │ libsad_ui.so (محرك واجهات أصلي)                     │  │   │
│  │  │  ├── EGL + OpenGL ES 3.0 (رسم)                      │  │   │
│  │  │  ├── عرض نصوص عربية (stb_truetype + HarfBuzz)       │  │   │
│  │  │  ├── نظام واجهات (عمود/صف/زر/نص/قائمة/...)         │  │   │
│  │  │  ├── نظام أحداث لمس + إيماءات                       │  │   │
│  │  │  ├── حركات وانتقالات                                │  │   │
│  │  │  └── تخطيط RTL تلقائي                               │  │   │
│  │  └────────────────────────────────────────────────────┘  │   │
│  └───────────────────────────────────────────────────────────┘   │
│                                                                  │
│  bionic libc │ EGL/GLES │ ALooper │ ASensor │ AAudio │ AHardware │
└──────────────────────────────────────────────────────────────────┘
```

---

## ٣. المكونات التقنية بالتفصيل

### ٣.١ Runtime أندرويد أصلي (`libsad_runtime.so`)

هذا هو القلب النابض — يوفر كل ما يحتاجه الكود المُترجَم لكي يعمل.

#### ٣.١.١ إدارة الذاكرة

```
┌─────────────────────────────────────────────────┐
│            مدير الذاكرة (SadMemoryManager)        │
│  ┌───────────────────────────────────────────┐   │
│  │ Arena Allocator (للكائنات قصيرة العمر)     │   │
│  │  - تخصيص أحادي الاتجاه (bump)            │   │
│  │  - تحرير جماعي عند نهاية النطاق          │   │
│  │  - مثالي لـ: سلاسل مؤقتة، حسابات إطار   │   │
│  └───────────────────────────────────────────┘   │
│  ┌───────────────────────────────────────────┐   │
│  │ Reference Counting (للكائنات طويلة العمر)  │   │
│  │  - عدّ مرجعي ذري (thread-safe)           │   │
│  │  - كشف دورات عبر Mark-Sweep اختياري      │   │
│  │  - مثالي لـ: كائنات UI، اتصالات شبكة     │   │
│  └───────────────────────────────────────────┘   │
│  ┌───────────────────────────────────────────┐   │
│  │ Region-based (لكل إطار رسم)               │   │
│  │  - ذاكرة مخصصة لكل إطار                  │   │
│  │  - تُحرَّر تلقائياً بعد الرسم             │   │
│  └───────────────────────────────────────────┘   │
└─────────────────────────────────────────────────┘
```

**واجهة C (للربط مع LLVM IR):**
```c
// تخصيص وتحرير
void* sad_alloc(size_t size);
void* sad_alloc_aligned(size_t size, size_t alignment);
void  sad_free(void* ptr);
void  sad_retain(void* obj);    // زيادة عدد المراجع
void  sad_release(void* obj);   // إنقاص عدد المراجع → تحرير إذا = 0

// Arena
SadArena* sad_arena_new(size_t capacity);
void*     sad_arena_alloc(SadArena* arena, size_t size);
void      sad_arena_reset(SadArena* arena);
void      sad_arena_free(SadArena* arena);
```

#### ٣.١.٢ أنواع البيانات الأصلية

| النوع | البنية C | الحجم | الملاحظات |
|------|----------|-------|----------|
| `رقم` (عدد صحيح) | `int64_t` | 8 بايت | مباشر، بدون تغليف |
| `عشري` | `double` | 8 بايت | IEEE 754 |
| `منطقي` | `bool` / `uint8_t` | 1 بايت | صحيح/خطأ |
| `نص` | `SadString` | 24 بايت | UTF-8, COW, SSO |
| `مصفوفة` | `SadArray` | 24 بايت | مصفوفة ديناميكية |
| `خريطة` | `SadMap` | 32 بايت | جدول تجزئة |
| `كائن` | `SadObject` | متغير | بنية + vtable |

```c
// SadString — تحسين السلاسل النصية القصيرة (SSO)
typedef struct {
    union {
        struct {          // سلسلة طويلة
            char* data;
            size_t length;
            size_t capacity;
        } heap;
        struct {          // سلسلة قصيرة (≤ 22 بايت)
            char data[23];
            uint8_t length; // الطول في البايت الأخير
        } sso;
    };
    uint32_t ref_count;   // عدّ مرجعي ذري
    uint8_t flags;        // SSO? | COW? | interned?
} SadString;

// SadArray — مصفوفة ديناميكية
typedef struct {
    SadValue* data;
    size_t length;
    size_t capacity;
    uint32_t ref_count;
} SadArray;

// SadMap — جدول تجزئة بسلسلة مفتوحة
typedef struct {
    SadMapEntry* buckets;
    size_t bucket_count;
    size_t length;
    uint32_t ref_count;
} SadMap;

// SadValue — القيمة الموحّدة (tagged union)
typedef struct {
    uint8_t type;  // 0=void, 1=int, 2=double, 3=string, 4=bool, 5=array, 6=map, 7=object
    union {
        int64_t    integer;
        double     decimal;
        SadString* string;
        bool       boolean;
        SadArray*  array;
        SadMap*    map;
        SadObject* object;
    };
} SadValue;
```

#### ٣.١.٣ نظام الشبكة الأصلي

```c
// TCP Socket (عبر bionic libc)
SadSocket* sad_tcp_connect(const char* host, uint16_t port);
SadSocket* sad_tcp_listen(uint16_t port, int backlog);
SadSocket* sad_tcp_accept(SadSocket* server);
int        sad_tcp_send(SadSocket* sock, const void* data, size_t len);
int        sad_tcp_recv(SadSocket* sock, void* buf, size_t len);
void       sad_tcp_close(SadSocket* sock);

// TLS (عبر BoringSSL المُضمّن في أندرويد)
SadTlsSocket* sad_tls_connect(const char* host, uint16_t port);
int           sad_tls_send(SadTlsSocket* sock, const void* data, size_t len);
int           sad_tls_recv(SadTlsSocket* sock, void* buf, size_t len);
void          sad_tls_close(SadTlsSocket* sock);

// WebSocket (فوق TCP/TLS)
SadWebSocket* sad_ws_connect(const char* url);
void          sad_ws_send(SadWebSocket* ws, const char* msg, size_t len);
void          sad_ws_on_message(SadWebSocket* ws, SadWsCallback callback);
void          sad_ws_close(SadWebSocket* ws);
```

#### ٣.١.٤ نظام الخيوط

```c
// خيوط (عبر pthreads في bionic)
SadThread*    sad_thread_new(SadThreadFunc func, void* arg);
void          sad_thread_join(SadThread* t);
void          sad_thread_detach(SadThread* t);

// قفل
SadMutex*     sad_mutex_new();
void          sad_mutex_lock(SadMutex* m);
void          sad_mutex_unlock(SadMutex* m);

// قناة رسائل (للتطبيق المحادثة)
SadChannel*   sad_channel_new(size_t capacity);
void          sad_channel_send(SadChannel* ch, SadValue val);
SadValue      sad_channel_recv(SadChannel* ch);
```

---

### ٣.٢ محرك الواجهات الأصلي (`libsad_ui.so`)

#### ٣.٢.١ طبقة الرسم (OpenGL ES 3.0)

```
┌──────────────────────────────────────────────────┐
│              محرك الرسم (SadRenderer)              │
│                                                    │
│  ┌────────────┐  ┌────────────┐  ┌─────────────┐ │
│  │ EGL Context│  │ Shader Mgr │  │ Texture Mgr │ │
│  │ + Surface  │  │ (GLSL ES)  │  │ (Atlas+Font)│ │
│  └────────────┘  └────────────┘  └─────────────┘ │
│                                                    │
│  واجهات الرسم:                                     │
│  ├── رسم_مستطيل(x, y, w, h, لون, نصف_قطر)        │
│  ├── رسم_نص(نص, x, y, حجم, خط, لون)              │
│  ├── رسم_صورة(مسار, x, y, w, h)                   │
│  ├── رسم_خط(x1, y1, x2, y2, سمك, لون)            │
│  ├── رسم_دائرة(cx, cy, r, لون)                    │
│  └── قص(x, y, w, h)   // clipping region          │
│                                                    │
│  نظام الدفعات (Batch Rendering):                   │
│  - تجميع المستطيلات المتشابهة في draw call واحد    │
│  - نصوص عبر texture atlas                         │
│  - حد أقصى ~1000 عنصر/دفعة                       │
└──────────────────────────────────────────────────┘
```

#### ٣.٢.٢ عرض النص العربي

```
┌──────────────────────────────────────────────────┐
│           نظام عرض النص (SadTextRenderer)          │
│                                                    │
│  ١. تحميل الخط: stb_truetype (Noto Naskh Arabic) │
│  ٢. تشكيل النص: HarfBuzz (ربط الحروف العربية)    │
│  ٣. اتجاه النص: FriBidi (ثنائي الاتجاه BiDi)     │
│  ٤. تحويل → glyphs → texture atlas               │
│  ٥. رسم عبر OpenGL ES                             │
│                                                    │
│  المدخلات:  "مرحباً بالعالم" (Unicode)             │
│  → FriBidi: تحديد اتجاه RTL                       │
│  → HarfBuzz: ربط حروف (لام+ألف = لا)             │
│  → stb_truetype: رسم كل glyph                     │
│  → Texture Atlas: تخزين الـ glyphs                 │
│  → OpenGL: رسم quads بإحداثيات RTL                │
└──────────────────────────────────────────────────┘
```

#### ٣.٢.٣ نظام الواجهات (Widget System)

```
شجرة الواجهات (Widget Tree):
─────────────────────────────

تطبيق
 └── عمود (Column)
      ├── شريط_عنوان (AppBar)
      │    └── نص ("محادثة ص")
      ├── قائمة_تمرير (ScrollView)
      │    └── عمود
      │         ├── فقاعة_رسالة (مرسلة)
      │         ├── فقاعة_رسالة (مستقبلة)
      │         └── ...
      └── صف (Row)
           ├── حقل_إدخال (TextInput)
           └── زر ("إرسال")

كل واجهة (Widget) تحتوي:
┌─────────────────────────────────┐
│ SadWidget                        │
│  ├── الموقع: {x, y, w, h}      │
│  ├── الهوامش: {أعلى, يمين, ...} │
│  ├── الحشو: {أعلى, يمين, ...}   │
│  ├── اللون: خلفية + حدود + نص  │
│  ├── الأبناء: مصفوفة واجهات     │
│  ├── مرئي: صحيح/خطأ            │
│  ├── تخطيط(): حساب الأحجام      │
│  ├── رسم(): رسم على الشاشة     │
│  └── حدث(): معالجة اللمس        │
└─────────────────────────────────┘
```

**الواجهات المطلوبة للمحادثة:**

| الواجهة | الوظيفة | الأولوية |
|---------|---------|---------|
| `نص` (Text) | عرض نص عربي/إنجليزي | P0 |
| `عمود` (Column) | تخطيط عمودي | P0 |
| `صف` (Row) | تخطيط أفقي | P0 |
| `زر` (Button) | زر قابل للنقر | P0 |
| `حقل_إدخال` (TextInput) | إدخال نص مع لوحة مفاتيح | P0 |
| `قائمة_تمرير` (ScrollView) | تمرير المحتوى | P0 |
| `صورة` (Image) | عرض صورة | P1 |
| `بطاقة` (Card) | حاوية بظل وحواف دائرية | P1 |
| `شريط_عنوان` (AppBar) | شريط أعلى التطبيق | P1 |
| `قائمة` (List) | عرض قائمة عناصر | P1 |
| `حوار` (Dialog) | نافذة حوار منبثقة | P2 |
| `شريط_سفلي` (BottomBar) | شريط تنقل سفلي | P2 |

#### ٣.٢.٤ نظام الأحداث والإدخال

```
┌──────────────────────────────────────────────────┐
│          نظام الأحداث (SadEventSystem)             │
│                                                    │
│  AInputEvent (من أندرويد)                          │
│   ↓                                                │
│  معالج الأحداث الخام:                              │
│   ├── لمس (AMOTION_EVENT_ACTION_*)                │
│   │    ├── بداية_لمس (DOWN)                        │
│   │    ├── تحريك (MOVE)                            │
│   │    └── رفع (UP)                                │
│   ├── لوحة مفاتيح (AKEY_EVENT_*)                  │
│   └── إيماءات مُركّبة:                             │
│        ├── نقرة (tap)                              │
│        ├── نقرة مزدوجة (double tap)                │
│        ├── ضغط طويل (long press)                   │
│        ├── سحب (drag/pan)                          │
│        ├── تمرير (fling/scroll)                    │
│        └── تكبير/تصغير (pinch zoom)                │
│   ↓                                                │
│  توزيع على شجرة الواجهات (Hit Testing):           │
│   - اختبار من الأبناء إلى الأباء                   │
│   - أول واجهة تطابق الإحداثيات تستقبل الحدث       │
│   - Bubbling: الحدث يصعد للأب إذا لم يُستهلك      │
└──────────────────────────────────────────────────┘
```

#### ٣.٢.٥ لوحة المفاتيح الافتراضية

```
NativeActivity لا يعرض لوحة المفاتيح تلقائياً.
الحل: استدعاء Java عبر JNI الحد الأدنى:

ANativeActivity* activity;
// عبر JNI المُضمّن في NativeActivity:
activity->callbacks->showSoftInput(activity, ANATIVEACTIVITY_SHOW_SOFT_INPUT_IMPLICIT);

أو بديل: لوحة مفاتيح مرسومة بالكامل بـ OpenGL ES (Arabic keyboard)
   ✓ استقلال تام
   ✗ جهد إضافي كبير
   → القرار: نستخدم JNI الحد الأدنى للوحة المفاتيح النظامية
```

---

### ٣.٣ طبقة NativeActivity

```c
// نقطة الدخول — بدون Java
void ANativeActivity_onCreate(ANativeActivity* activity,
                               void* savedState, size_t savedStateSize)
{
    // ١. تهيئة Runtime
    sad_runtime_init();

    // ٢. تهيئة EGL + OpenGL ES 3.0
    sad_renderer_init(activity->window);

    // ٣. تهيئة نظام الواجهات
    sad_ui_init(sad_renderer_get());

    // ٤. استدعاء الرئيسية() من كود المستخدم
    sad_user_main();

    // ٥. بدء حلقة الأحداث
    sad_event_loop(activity);
}

// حلقة الأحداث الرئيسية
void sad_event_loop(ANativeActivity* activity) {
    while (!should_quit) {
        // ١. معالجة أحداث أندرويد (ALooper)
        int events;
        struct android_poll_source* source;
        while (ALooper_pollAll(0, NULL, &events, (void**)&source) >= 0) {
            if (source) source->process(activity, source);
        }

        // ٢. تحديث حالة الواجهات
        sad_ui_update(delta_time);

        // ٣. رسم الإطار
        sad_renderer_begin_frame();
        sad_ui_render(sad_renderer_get());
        sad_renderer_end_frame();  // eglSwapBuffers
    }
}
```

---

### ٣.٤ التعديلات المطلوبة على المترجم (`sadc`)

#### ٣.٤.١ تفعيل هدف أندرويد

الملف [compiler_new/src/targets/target_list.cpp](compiler_new/src/targets/target_list.cpp) يسجل هدف Android ARM64 لكنه مُعَلَّم كـ "غير متاح". المطلوب:

```cpp
// تفعيل الهدف
TargetInfo android_arm64;
android_arm64.triple = "aarch64-linux-android24";
android_arm64.available = true;  // ← تغيير من false إلى true
android_arm64.toolchain = NDKDetector::findNDK();
register_target("android-arm64", android_arm64);
```

#### ٣.٤.٢ ربط Runtime في LLVM IR

عند ترجمة كود ص إلى LLVM IR، يجب إضافة تصريحات لدوال الـ Runtime:

```llvm
; دوال Runtime مُصرَّح عنها
declare i8*  @sad_alloc(i64)
declare void @sad_free(i8*)
declare void @sad_retain(i8*)
declare void @sad_release(i8*)

declare %SadString* @sad_string_new(i8*, i64)
declare %SadString* @sad_string_concat(%SadString*, %SadString*)
declare i64         @sad_string_length(%SadString*)

declare void @sad_print(%SadString*)
declare void @sad_println(%SadString*)

; دوال الشبكة
declare %SadWebSocket* @sad_ws_connect(i8*)
declare void           @sad_ws_send(%SadWebSocket*, i8*, i64)
```

#### ٣.٤.٣ إضافة أمر `sadc android`

```
sadc android new <اسم_المشروع>     # إنشاء مشروع جديد
sadc android build [--release]      # بناء APK
sadc android install                # تثبيت على الجهاز
sadc android run                    # تشغيل على الجهاز
sadc android devices                # عرض الأجهزة المتصلة
```

---

## ٤. هيكل المجلدات الجديد

```
compiler_new/
  src/
    targets/
      android_ndk.cpp          # ← موجود (674 سطر)
      android_native.cpp       # ← جديد: NativeActivity bridge
      android_apk_builder.cpp  # ← جديد: بناء APK بدون Gradle

runtime_new/
  android/                     # ← مجلد جديد بالكامل
    sad_android_runtime.c      # Runtime أصلي (ذاكرة + أنواع)
    sad_android_runtime.h
    sad_android_memory.c       # Arena + RC + GC
    sad_android_memory.h
    sad_android_string.c       # SadString (UTF-8 + SSO)
    sad_android_string.h
    sad_android_array.c        # SadArray
    sad_android_array.h
    sad_android_map.c          # SadMap (hash table)
    sad_android_map.h
    sad_android_value.c        # SadValue (tagged union)
    sad_android_value.h
    sad_android_thread.c       # خيوط (pthreads)
    sad_android_thread.h
    sad_android_network.c      # TCP/TLS/WebSocket
    sad_android_network.h
    sad_android_io.c           # ملفات + أصول
    sad_android_io.h
    CMakeLists.txt

graphics/
  android/                     # ← مجلد جديد
    sad_egl.c                  # تهيئة EGL
    sad_egl.h
    sad_gles_renderer.c        # محرك OpenGL ES 3.0
    sad_gles_renderer.h
    sad_text_renderer.c        # عرض نصوص عربية
    sad_text_renderer.h
    sad_ui_widgets.c           # نظام الواجهات
    sad_ui_widgets.h
    sad_ui_layout.c            # خوارزميات التخطيط (Flexbox)
    sad_ui_layout.h
    sad_input.c                # أحداث لمس + إيماءات
    sad_input.h
    sad_animation.c            # حركات وانتقالات
    sad_animation.h
    sad_native_activity.c      # نقطة الدخول
    sad_native_activity.h
    CMakeLists.txt

stdlib/
  android/                     # ← موجود (32 ملف .ص) — يحتاج ربط حقيقي

tools/
  android/                     # ← موجود — يحتاج تحديث
    apk_builder_native.cpp     # ← جديد: بناء APK بدون Gradle/Python
```

---

## ٥. خطة التنفيذ (المراحل)

### المرحلة ١: Runtime أصلي (4-6 أسابيع)

| الأسبوع | المهمة | المخرج |
|---------|--------|--------|
| 1-2 | إدارة ذاكرة (Arena + RC) | `sad_android_memory.c/h` |
| 2-3 | أنواع بيانات (String + Array + Map + Value) | `sad_android_string/array/map/value.c/h` |
| 3-4 | نظام خيوط + قنوات رسائل | `sad_android_thread.c/h` |
| 4-5 | شبكة TCP + TLS | `sad_android_network.c/h` |
| 5-6 | ملفات + أصول + استثناءات | `sad_android_io.c/h` |

### المرحلة ٢: محرك رسومي (4-6 أسابيع)

| الأسبوع | المهمة | المخرج |
|---------|--------|--------|
| 1-2 | EGL + OpenGL ES 3.0 + NativeActivity | `sad_egl.c`, `sad_native_activity.c` |
| 2-3 | رسم أشكال 2D + دفعات | `sad_gles_renderer.c` |
| 3-4 | عرض نصوص عربية (stb_truetype + HarfBuzz) | `sad_text_renderer.c` |
| 4-5 | نظام إدخال + إيماءات | `sad_input.c` |
| 5-6 | حركات وانتقالات بسيطة | `sad_animation.c` |

### المرحلة ٣: نظام واجهات (4-6 أسابيع)

| الأسبوع | المهمة | المخرج |
|---------|--------|--------|
| 1-2 | بنية Widget + تخطيط Flexbox | `sad_ui_widgets.c`, `sad_ui_layout.c` |
| 2-3 | واجهات P0: نص، عمود، صف، زر | Widget implementations |
| 3-4 | حقل_إدخال + لوحة مفاتيح | TextInput widget |
| 4-5 | قائمة_تمرير + قائمة | ScrollView + List widgets |
| 5-6 | ثيم Material-like + RTL | Theming system |

### المرحلة ٤: دمج المترجم + APK (2-3 أسابيع)

| الأسبوع | المهمة | المخرج |
|---------|--------|--------|
| 1 | تفعيل هدف Android في sadc | Cross-compilation works |
| 1-2 | ربط Runtime بـ LLVM IR | Runtime calls in generated code |
| 2-3 | باني APK بدون Gradle | `sadc android build` works |

### المرحلة ٥: تطبيق المحادثة (2-3 أسابيع)

| الأسبوع | المهمة | المخرج |
|---------|--------|--------|
| 1 | بروتوكول WebSocket + خادم اختبار | Chat protocol |
| 1-2 | واجهة المحادثة (فقاعات رسائل + إدخال) | Chat UI |
| 2-3 | اختبار على جهاز حقيقي + تنقيح | Working APK |

---

## ٦. مؤشرات النجاح

| المؤشر | الهدف |
|--------|------|
| حجم APK | < 5 MB |
| زمن بدء التطبيق | < 500ms |
| معدل الإطارات | 60 FPS مستقر |
| استهلاك الذاكرة | < 50 MB |
| أقل نسخة أندرويد | API 24 (Android 7.0) |
| معماريات مدعومة | arm64-v8a (أساسي), armeabi-v7a (اختياري) |

---

## ٧. المخاطر والتحديات

| المخاطر | الاحتمال | التأثير | التخفيف |
|---------|---------|---------|---------|
| عرض النص العربي معقد (تشكيل + ربط) | عالي | عالي | استخدام HarfBuzz المُجرَّب |
| NativeActivity لا يدعم لوحة المفاتيح | عالي | متوسط | JNI حد أدنى لـ showSoftInput |
| الترجمة المتقاطعة Windows→Android | متوسط | عالي | اختبار مبكر مع NDK |
| أداء OpenGL ES على أجهزة ضعيفة | متوسط | متوسط | Batching + Atlas + LOD |
| توافق مع إصدارات أندرويد المختلفة | متوسط | متوسط | اختبار على API 24+ فقط |

---

## ٨. الاعتماديات الخارجية

| المكتبة | الغرض | الترخيص | الحجم |
|---------|-------|---------|------|
| stb_truetype | تحميل خطوط TTF | ملكية عامة | ~60 KB |
| HarfBuzz | تشكيل النص العربي | MIT | ~1.5 MB |
| FriBidi | اتجاه النص (BiDi) | LGPL 2.1 | ~200 KB |
| BoringSSL | TLS (مُضمّن في أندرويد) | ISC | 0 (نظام) |
| Noto Naskh Arabic | خط عربي | OFL | ~300 KB |

---

## ٩. مثال التطبيق النهائي (تطبيق المحادثة)

```sad
# تطبيق محادثة بلغة ص — بدون Java/Kotlin

استورد واجهة من "أندرويد/واجهة"
استورد شبكة من "أندرويد/شبكة"
استورد حالة من "أندرويد/حالة"

صنف تطبيق_المحادثة
    متغير الاسم: نص
    متغير الرسائل: مصفوفة
    متغير الاتصال: شبكة.ويب_سوكت
    متغير رسالة_جديدة: نص

    باني(اسم: نص)
        هذا.الاسم = اسم
        هذا.الرسائل = []
        هذا.رسالة_جديدة = ""
    نهاية

    دالة اتصل()
        هذا.الاتصال = شبكة.ويب_سوكت("wss://chat.example.com")

        هذا.الاتصال.عند_الاستقبال(دالة(رسالة)
            هذا.الرسائل.أضف({
                "المرسل": رسالة.مرسل،
                "النص": رسالة.محتوى،
                "الوقت": رسالة.وقت
            })
        نهاية)
    نهاية

    دالة أرسل()
        إذا (هذا.رسالة_جديدة != "")
            هذا.الاتصال.أرسل({
                "المرسل": هذا.الاسم،
                "النص": هذا.رسالة_جديدة
            })
            هذا.رسالة_جديدة = ""
        نهاية
    نهاية

    دالة الواجهة()
        ارجع عمود(
            # شريط العنوان
            شريط_عنوان(
                عنوان: "💬 " + هذا.الاسم
                لون_خلفية: "#1976D2"
            نهاية

            # قائمة الرسائل
            قائمة_تمرير(وزن: 1)
                لكل رسالة في هذا.الرسائل
                    فقاعة_رسالة(
                        نص: رسالة["النص"]
                        مرسل: رسالة["المرسل"]
                        مرسلة: رسالة["المرسل"] == هذا.الاسم
                    نهاية
                نهاية
            نهاية

            # شريط الإدخال
            صف(حشو: 8)
                حقل_إدخال(
                    قيمة: هذا.رسالة_جديدة
                    تلميح: "اكتب رسالة..."
                    عند_التغيير: دالة(نص) هذا.رسالة_جديدة = نص نهاية
                    وزن: 1
                نهاية

                زر(
                    نص: "📤"
                    عند_الضغط: هذا.أرسل
                    لون: "#1976D2"
                نهاية
            نهاية
        نهاية
    نهاية
نهاية

# نقطة الدخول
دالة الرئيسية()
    متغير تطبيق = جديد تطبيق_المحادثة("أحمد")
    تطبيق.اتصل()

    واجهة.شغّل(تطبيق.الواجهة)
نهاية
```

---

## ١٠. ملخص

هذه الوثيقة تُحدد المسار الكامل لجعل لغة ص مستقلة تماماً على أندرويد. المشروع يمتلك بالفعل بنية تحتية كبيرة (مترجم LLVM مكتمل، هدف ARM64 مُسجّل، أنواع SIR شاملة، runtime أساسي). المطلوب هو **ربط هذه القطع معاً** وبناء الطبقات الناقصة (محرك رسومي + واجهات + شبكة أصلية).

**المدة الإجمالية التقديرية: 16-24 أسبوع (4-6 أشهر)** للوصول إلى تطبيق محادثة يعمل على جهاز حقيقي.
