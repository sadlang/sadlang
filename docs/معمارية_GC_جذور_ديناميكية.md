# معمارية محرك جامع المهملات (GC) — رحلة B-step5b بالكامل

> **آخر تحديث:** أبريل 2026
> **الفرع:** `graphic`
> **الكوميتات المرجعية:** `dc8f2e43` (i) → `4e9c4ff1` (ii) → `ccc1be4d` (iii)

---

## 1. المقدّمة والدافع

كان نظام إدارة الذاكرة للكائنات في لغة ص يعتمد على `std::shared_ptr<ObjectInstance>`
(عدّ مرجعي تلقائي). هذا الحل يعمل لكنه:

| المشكلة | الأثر |
|---------|------|
| **دورات مرجعية** | كائن أ يحوي مرجعاً لـ ب، وب يحوي مرجعاً لأ → لا يُحرَّر أبداً |
| **حمل عدّ مرجعي** | كل نسخة Value تزيد/تنقص العداد (atomic op) — مكلف |
| **انفصال عن مفهوم "الجذور"** | لا توجد طريقة لتعداد كل الكائنات الحية لتحليل اللحظة |
| **تعارض مع أهداف اللغة** | لغة ص تستهدف نظام تشغيل (ufuq OS) — `shared_ptr` غير ملائم |

**الحل الجذري:** بناء محرك GC حقيقي (Mark-and-Sweep تزايدي) يدير
حياة الكائنات عبر مؤشرات خام (`ObjectInstance*`) مُسجَّلة في المحرك.

---

## 2. المخطّط المعماري الكلّي

```
┌──────────────────────────────────────────────────────────────────────┐
│                       طبقات النظام (من الأعلى للأسفل)                 │
└──────────────────────────────────────────────────────────────────────┘

   ┌──────────────────────┐    ┌─────────────────────────┐
   │ المُفسّر (interpreter)│    │  الآلة الافتراضية (vm)   │
   │  ──────────          │    │  ──────────              │
   │  StatementExecutor   │    │  StackFrame              │
   │  ScopeManager        │    │  Operands stack          │
   │  VariableManager  ◄──┼────┼── (لاحقاً)                │
   │  callStack           │    │                          │
   └──────────┬───────────┘    └────────────┬────────────┘
              │                              │
              │  يُسجّل موفّر جذور           │  (سيُسجّل لاحقاً)
              ▼                              ▼
   ┌──────────────────────────────────────────────────────┐
   │              shared/types — Value                     │
   │   ┌────────────────────────────────────────────┐     │
   │   │ std::variant<                              │     │
   │   │   monostate, int64, double, string, bool,  │     │
   │   │   shared_ptr<ArrayType>,                   │     │
   │   │   shared_ptr<MapType>,                     │     │
   │   │   shared_ptr<TupleType>,                   │     │
   │   │   ObjectInstance*,        ◄── مؤشر خام    │     │
   │   │   shared_ptr<FunctionRef>                  │     │
   │   │ >                                          │     │
   │   └────────────────────────────────────────────┘     │
   │                                                       │
   │   forEachObjectRef(cb) → يتعمّق في ARRAY/MAP/OBJECT  │
   └──────────────────┬────────────────────────────────────┘
                      │
                      │ destroyer + visitor + emit
                      ▼
   ┌──────────────────────────────────────────────────────┐
   │       shared/memory_gc — GarbageCollector            │
   │                                                       │
   │   objects_:    [ObjectInstance*, ...]   ← مسجّل      │
   │   destroyers_: [void(*)(void*), ...]    ← لكل كائن  │
   │   visitors_:   [void(*)(void*, emit), ...]           │
   │   roots_:      [void*, ...]             ← جذور ثابتة│
   │   rootProviders_: [(id, fn), ...]       ← جذور دينام│
   │                                                       │
   │   ┌─────────────┐   ┌─────────────┐                 │
   │   │ register()  │   │ markAndSweep│                 │
   │   │ unregister()│   │ mark*Locked │                 │
   │   │ addRoot     │   │ sweepLocked │                 │
   │   │ addRootPro- │   │             │                 │
   │   │   vider     │   │             │                 │
   │   └─────────────┘   └─────────────┘                 │
   └──────────────────────────────────────────────────────┘
```

---

## 3. المراحل الثلاث — التسلسل الزمني

### 🟢 B-step5b-i — البنية التحتية للـ GC الدقيق

**الكوميت:** `dc8f2e43`

#### ما تم إنجازه
- إضافة `destroyer callbacks` لكل كائن مسجّل (تحديد كيف يُحرَّر).
- إضافة `visitChildren` (visitor) لكل كائن (تحديد ما يحويه من مؤشرات).
- آلية `Pin` (تثبيت مؤقت لمنع التحرير أثناء العمليات الحرجة).

#### المخطّط
```
┌──────────────────────────────────────────────┐
│ GarbageCollector::registerObject(ptr,        │
│                                  destroyer,  │
│                                  visitor)    │
└──────────────────────────────────────────────┘
         ▼
   objects_:     [ptr]
   destroyers_:  [delete-as-ObjectInstance]
   visitors_:    [walk-fields-and-emit]
```

---

### 🟢 B-step5b-ii — تحويل ObjectPtr إلى مؤشر خام

**الكوميت:** `4e9c4ff1` — **33 ملف، +2065 / −1851 سطر**

#### ما تم إنجازه
| الملف/المنطقة | التغيير |
|---|---|
| `shared/types/include/value.h` | `using ObjectPtr = ObjectInstance*` |
| `shared/types/src/value.cpp` | باني Value(ObjectPtr) يسجّل في GC مع destroyer/visitor |
| كل `std::get<shared_ptr<ObjectInstance>>` | → `std::get<ObjectInstance*>` |
| `interpreter/.../widget_builder` | `addChildBuilder(WidgetBuilder*)` خام |
| `ui_bridge` | `HandlerEntry::owner` مؤشر خام |
| `ui_state_manager` | `instance` مؤشر خام، إزالة `.lock()`/`.expired()` |
| OOP visitors | `new ObjectInstance(...)` بدلاً من `make_shared` |

#### المخطّط — قبل وبعد

```
   ◄ قبل ►                              ◄ بعد ►

  Value                              Value
   │                                   │
   ├── shared_ptr<ObjectInstance>     ├── ObjectInstance*  ──┐
   │      │                           │                       │
   │      ├── refCount: 3             │                       │
   │      └── ObjectInstance          │                       │
   │            ├── classType         │                       │
   │            ├── id                │                       │
   │            └── fields            │                       │
   │                                   │                       │
   │ نسخ Value → atomic++              │ نسخ Value → نسخة      │
   │ تدمير Value → atomic--            │ مؤشر فقط (رخيص)       │
   │                                   │                       │
   │ تحرير عند 0                       │ تحرير عبر GC sweep    │
                                       ▼
                                   GC.objects_[i] = ptr
                                   GC.destroyers_[i] = delete
                                   GC.visitors_[i] = walk-fields
```

#### نتائج التحقق
- ✅ بوابة hub: 47/47
- ✅ smoke في 4 أوضاع للمُفسّر: Exit=0
- ✅ smoke VM (سكلر بسيط): Exit=0
- ⚠️ VM لا يدعم OOP constructors (محدودية سابقة، ليست تراجعاً)

---

### 🟢 B-step5b-iii — جذور GC ديناميكية عبر VariableManager

**الكوميت:** `ccc1be4d` — **6 ملفات، +226 / −11 سطر**

#### المشكلة
بعد تحويل `ObjectPtr` إلى مؤشر خام، أصبح GC الوحيد المسؤول عن
تحرير الكائنات. لكن إذا لم يعرف GC ما هي **الجذور** (roots) الحيّة،
فإنه إما:
- **يحرّر كائناً ما زال يُستخدم** → segfault (الأسوأ)
- **لا يحرّر شيئاً أبداً** (fallback محافظ) → تسرّب ذاكرة

#### الحل — Root Provider Pattern

بدلاً من تسجيل/إلغاء كل متغير يدوياً، نُسجّل **callback واحد**
يُستدعى أثناء mark phase ويُصدر كل المؤشرات الحيّة.

#### المخطّط الكامل

```
        VariableManager (مدير المتغيرات)
        ──────────────────────────────
        scopeVariables_:
          ┌─────────────┬──────────────────┐
          │ Scope#1     │ {س: Value(obj1)} │
          │ Scope#2     │ {ن: Value(obj2),│
          │             │  ق: Value([     │
          │             │     Value(obj3),│
          │             │     Value(obj4) │
          │             │  ])}            │
          └─────────────┴──────────────────┘
                  │
                  │ في الباني:
                  │   id = GC.addRootProvider(provider)
                  │
                  │ provider = [this](emit) {
                  │   for scope in scopeVariables_:
                  │     for (name, value) in scope:
                  │       value.forEachObjectRef([&](obj){
                  │         emit(obj);
                  │       });
                  │ }
                  ▼
        GarbageCollector
        ────────────────
        rootProviders_:
          [(1, provider_من_VariableManager)]
                  │
                  │ عند markAndSweep():
                  ▼
        markFromRootsLocked(marked):
          worklist = []
          for r in roots_:           ← الجذور الثابتة
            worklist.push(r)
          for (id, fn) in rootProviders_:
            fn(emit=worklist.push)   ← الجذور الديناميكية

          while worklist not empty:
            ptr = worklist.pop()
            mark(ptr)
            visitor(ptr, emit_to_worklist)  ← أطفال

          if worklist empty AND roots empty:
            mark all (fallback محافظ)
```

#### Value::forEachObjectRef — التعداد العميق

```sad
# قيمة مثل:
متغير ق = [نقطة(1،2)، نقطة(3،4)، {أ: نقطة(5،6)}]

# forEachObjectRef يصدر:
emit(obj_للنقطة(1،2))   ← من العنصر [0]
emit(obj_للنقطة(3،4))   ← من العنصر [1]
emit(obj_للنقطة(5،6))   ← من قيمة "أ" في الخريطة
```

```
Value::forEachObjectRef(emit)
  │
  ├─ OBJECT  → emit(toObject())
  │
  ├─ ARRAY   → for element in *arrPtr:
  │              element.forEachObjectRef(emit)  ← تعمّق
  │
  ├─ MAP     → for (k, v) in *mapPtr:
  │              v.forEachObjectRef(emit)        ← تعمّق
  │
  └─ غير ذلك → لا شيء (سكلر/نص/دالة/منطقي/...)
```

#### نتائج التحقق
- ✅ بناء `sad` بدون أخطاء (Debug)
- ✅ بوابة hub: 47/47 (~2.7 ث)
- ✅ smoke (run/check) على ملف يحوي:
  - تمرير كائن بالمرجع لدالة
  - قائمة من 5 كائنات
  - خريطة بقيم كائنات
- ✅ analyze/fmt: نفس السلوك السابق (ليس تراجعاً)

---

## 4. تشريح كوميت `ccc1be4d` — الملفات المعدّلة

```
┌─────────────────────────────────────────────────────────────────┐
│ shared/memory_gc/include/memory/gc/engine/garbage_collector.h   │
│ ───────────────────────────────────────────────────────────────  │
│ + using RootEmitter  = std::function<void(void*)>;              │
│ + using RootProvider = std::function<void(const RootEmitter&)>; │
│ + int  addRootProvider(RootProvider provider);                  │
│ + void removeRootProvider(int id);                              │
│ + std::vector<std::pair<int, RootProvider>> rootProviders_;     │
│ + int nextProviderId_;                                          │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│ shared/memory_gc/src/engine/garbage_collector.cpp               │
│ ───────────────────────────────────────────────────────────────  │
│ • ctor: nextProviderId_(1) في init list                         │
│ • markFromRootsLocked: worklist يبني من roots_ + providers      │
│ • addRootProvider/removeRootProvider: تنفيذ مع mutex            │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│ shared/types/include/value.h                                    │
│ ───────────────────────────────────────────────────────────────  │
│ + #include <functional>                                         │
│ + void forEachObjectRef(const std::function<...>& emit) const;  │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│ shared/types/src/value.cpp                                      │
│ ───────────────────────────────────────────────────────────────  │
│ + Value::forEachObjectRef impl (OBJECT/ARRAY/MAP recurse)       │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│ interpreter/include/managers/variable_manager.h                 │
│ ───────────────────────────────────────────────────────────────  │
│ + int gcRootProviderId_ = 0;                                    │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│ interpreter/src/managers/variable_manager.cpp                   │
│ ───────────────────────────────────────────────────────────────  │
│ + #include "memory/gc/engine/garbage_collector.h"               │
│ • ctor: تسجيل provider يمشي على scopeVariables_                 │
│ • dtor: removeRootProvider قبل أي تنظيف                         │
└─────────────────────────────────────────────────────────────────┘
```

---

## 5. سيناريو حياة كائن — تتبّع كامل

```sad
دالة عدّل(ن)
    ن.س = ن.س + 100   ← (5) gc يرى obj1 حياً عبر "ن" المحلي
نهاية

متغير ن1 = نقطة(3، 7)  ← (1) إنشاء obj1
عدّل(ن1)               ← (2) push scope جديد، ربط "ن"→obj1
                       ← (4) لو حصل GC هنا، obj1 حي عبر:
                       ←      - VariableManager (ن1 في scope#1)
                       ←      - VariableManager (ن في scope#2)
                       ← (6) pop scope#2
اطبع_سطر(ن1.وصف())     ← (7) obj1 ما زال حي عبر "ن1"
```

```
الزمن  | scopeVariables_                              | obj1 حي؟
────────┼─────────────────────────────────────────────┼──────────
  (1)   | scope#1: {ن1: Value(obj1)}                  |   ✓
  (2)   | scope#1: {ن1: Value(obj1)}                  |   ✓
        | scope#2: {ن:  Value(obj1)}                  |
  (4)   |   ↑ provider يصدر obj1 مرتين (آمن)         |   ✓
  (6)   | scope#1: {ن1: Value(obj1)}                  |   ✓
  (7)   | scope#1: {ن1: Value(obj1)}                  |   ✓
  END   | scope#1 destroyed → variableManager dtor    |
        |   removeRootProvider                         |
        |   → في sweep التالي obj1 يُحرَّر            |   ✗
```

---

## 6. الخريطة الكلّية — ما اكتمل وما تبقى

### ✅ مكتمل (مُلتزَم على `graphic`)

| المرحلة | الكوميت | الإنجاز |
|---------|---------|---------|
| B-step5b-i  | `dc8f2e43` | destroyers + visitors + Pin |
| B-step5b-ii | `4e9c4ff1` | ObjectPtr → مؤشر خام (33 ملف) |
| B-step5b-iii | `ccc1be4d` | RootProvider API + VariableManager hookup |

### 🟡 قيد التطوير / لاحقاً

| المرحلة | الوصف | الأولوية |
|---------|-------|----------|
| **B-step5b-iv** | تغطية مصادر الجذور المتبقّية: | عالية |
|  | - channels (`SadChannel` يحمل قيم محتجزة) | |
|  | - futures (`Future::set` يحفظ نتيجة) | |
|  | - UI handlers (`handlerOwners_`) | |
|  | - VM (`vm/` لم يُربط بعد) | |
| **B-step5b-v** | إزالة الـ fallback المحافظ من `markFromRootsLocked` | متوسطة |
|  | (يتطلّب الثقة بأن جميع المصادر مغطاة) | |
| **B-step5b-vi** | تفعيل دورة `collect()` دورية + عتبات تشغيل | متوسطة |
|  | (حالياً GC لا يجمع تلقائياً) | |
| **B-step5b-vii** | اختبار stress: ألف كائن متداخل + قياس الذاكرة | متوسطة |
| **B-step5c** | دعم VM للـ OOP constructors + ربطه بـ GC | منخفضة |
| **B-step5d** | GC تزايدي حقيقي (incremental marking) | منخفضة |
|  | البنية موجودة (`incPhase_`, `incIndex_`) لكن غير مُفعَّلة | |
| **B-step5e** | نقل GC إلى نواة ufuq OS (freestanding mode) | بحثية |

### 🔴 محدوديات معروفة

| المحدودية | الموقع | الحل المقترح |
|----------|--------|--------------|
| الـ fallback يعلّم كل شيء حياً | `markFromRootsLocked` | B-step5b-v |
| VM يرفض OOP `جديد` | `vm/bytecode_compiler` | B-step5c |
| لا يوجد `Sad::Memory::GC::stats()` معروض للمستخدم | API | لاحقاً |
| لا توجد آلية finalization للموارد (ملفات/مقابس) | `ObjectInstance` | اقتراح |

---

## 7. جدول API الجديد (ملخّص مرجعي)

### `Sad::Memory::GC::GarbageCollector`

| الدالة | التوقيع | الغرض |
|--------|---------|-------|
| `registerObject` | `int(void*, Destroyer, Visitor)` | تسجيل كائن في GC |
| `unregisterObject` | `void(int)` | إزالة (نادراً) |
| `addRoot` | `void(void*)` | جذر ثابت (غالباً للاختبارات) |
| `removeRoot` | `void(void*)` | إزالة جذر ثابت |
| **`addRootProvider`** ⭐ | `int(RootProvider)` | جذور ديناميكية (B-iii) |
| **`removeRootProvider`** ⭐ | `void(int)` | إلغاء التسجيل (B-iii) |
| `collect` | `size_t()` | تشغيل دورة كاملة |
| `pause/resume` | `void()` | إيقاف مؤقت |

### `Sad::Data::Value`

| الدالة | الغرض |
|--------|-------|
| `toObject()` | استخراج `ObjectInstance*` |
| **`forEachObjectRef(emit)`** ⭐ | تعداد عميق لمراجع الكائنات (B-iii) |

---

## 8. كيفية إضافة موفّر جذور جديد (دليل سريع)

```cpp
// 1) تضمين الترويسة
#include "memory/gc/engine/garbage_collector.h"

// 2) في الباني
auto& gc = Sad::Memory::GC::defaultEngine();
providerId_ = gc.addRootProvider(
    [this](const Sad::Memory::GC::GarbageCollector::RootEmitter& emit) {
        // اعرض كل ObjectInstance* الحيّ في حالتك:
        for (auto* obj : myLiveObjects_) {
            if (obj) emit(static_cast<void*>(obj));
        }
        // أو استخدم forEachObjectRef للقيم:
        for (const auto& v : myValues_) {
            v.forEachObjectRef([&emit](Sad::Data::ObjectInstance* o){
                emit(static_cast<void*>(o));
            });
        }
    });

// 3) في الهادم
if (providerId_ > 0) {
    Sad::Memory::GC::defaultEngine().removeRootProvider(providerId_);
    providerId_ = 0;
}
```

---

## 9. مراجع داخلية

- ملف ذاكرة المستودع: `/memories/repo/architecture_refactor_status.md`
- خطة إعادة الهيكلة الكاملة: [docs/خطة_إعادة_الهيكلة_المعمارية.md](خطة_إعادة_الهيكلة_المعمارية.md)
- معمارية الذاكرة العامة: [docs/معمارية_الذاكرة_الموحدة.md](معمارية_الذاكرة_الموحدة.md)
- ملف اختبار الدخان: `_scratch/b5biii_smoke.ص`

---

**خاتمة:** هذه المرحلة تمثّل الخطوة الجوهرية في تحويل لغة ص من نموذج
إدارة ذاكرة "ساذج" إلى نموذج محرك GC حقيقي قابل للتطوير نحو نظام
تشغيل مستقل. الأساس مكتمل؛ المراحل القادمة تتعلق بالتغطية والتحسين
والتفعيل الفعلي للجمع التلقائي.

---

# الجزء الثاني — نظام إدارة الذاكرة الكامل عبر جميع المسارات

> هذا الجزء أُضيف بناءً على طلب توسيع التغطية لتشمل **نظام الملكية في
> المترجم** (sadc) وإمكانية الانتقال نحوه، بالإضافة إلى رسم الصورة
> الكلية للتفاعل بين GC والملكية والمسارات الثلاثة (المُفسّر/الـVM/المترجم).

## 10. نظرة شاملة — لغة ص لديها **نظامان** للذاكرة، لا واحد

| النظام | الموقع | الدور | الحالة |
|--------|--------|------|---------|
| **GC (Mark-and-Sweep)** | `shared/memory_gc/` | إدارة كائنات وقت التشغيل في المُفسّر/الـVM | 🟢 يعمل (B-step5b-iii) |
| **نظام الملكية (Ownership)** | `shared/ownership/` + `compiler/src/sir/` | فحص ساكن وقت الترجمة + توليد `Drop`/`Free` تلقائياً في sadc | 🟡 الفحص يعمل، توليد التحرير الجزئي |

**هذا ليس تكراراً** بل تخصصاً متكاملاً:
- المُفسّر يحتاج مرونة → GC لأن الحياة الديناميكية صعبة التتبع ساكناً.
- المترجم يحتاج أداءً → نظام ملكية على غرار Rust (صفر تكلفة وقت تشغيل).

```
                  ┌──────────────────────────────┐
                  │      AST (shared/ast)         │
                  └──────────────┬───────────────┘
                                 │
        ┌────────────────────────┼────────────────────────┐
        │                        │                        │
        ▼                        ▼                        ▼
┌──────────────┐        ┌──────────────┐         ┌─────────────────┐
│  المُفسّر    │        │     VM       │         │     sadc        │
│ (interpreter)│        │  (bytecode)  │         │  (compiler)     │
│              │        │              │         │                 │
│ ┌──────────┐ │        │ ┌──────────┐ │         │ ┌─────────────┐ │
│ │   GC     │ │        │ │ (لاحقاً) │ │         │ │ Ownership   │ │
│ │ + Roots  │ │        │ │   GC     │ │         │ │ Borrow Chk  │ │
│ │ Provider │ │        │ │          │ │         │ │ Lifetime    │ │
│ └──────────┘ │        │ └──────────┘ │         │ └─────┬───────┘ │
│              │        │              │         │       ▼          │
│ Ownership    │        │              │         │  SIR (12 ops)   │
│  Manager     │        │              │         │       ▼          │
│ (ديناميكي،   │        │              │         │  LLVM IR        │
│  تحذيري)    │        │              │         │  (drop/free     │
│              │        │              │         │   مولّدة آلياً) │
└──────────────┘        └──────────────┘         └─────────────────┘
```

---

## 11. نظام الملكية في المترجم — التفاصيل

### 11.1 الطبقة المشتركة: `shared/ownership/`

نُقلت من `compiler/src/semantic/` إلى `shared/ownership/` ليُستفاد منها
من **جميع المسارات** (LSP، sadc، interpreter، formatter، analyzer).

**المكونات الأساسية:**

| الملف/الفئة | الدور |
|-------------|-------|
| `OwnershipTracker` | تتبع حالة كل متغير: Owned/Moved/Borrowed/Dropped/Uninit |
| `BorrowChecker` | فرض قواعد Rust-like (استعارة متغيرة حصرية، ثابتة متعددة) |
| `LifetimeAnalyzer` | تحليل أعمار المراجع `&'أ` |
| `MoveAnalyzer` | تتبع نقل الملكية (`انقل` كلمة سياقية) |
| `UnsafeChecker` | فحص كتل `@غير_آمن ... نهاية` |
| `PatternOwnership` | ملكية الأنماط في `طابق` |
| `ExhaustivenessChecker` | شمولية المطابقة |
| `ThisBinding` | ربط `هذا` في الأصناف |

### 11.2 حالات الملكية الستة

```
       Uninitialized
            │
            │  متغير س = ...
            ▼
        ┌──────┐    Borrow(&س)         ┌──────────┐
        │Owned │ ──────────────────►   │ Borrowed │
        │      │                        │ (مشترك)  │
        │      │ ◄──────────────────    │          │
        │      │    EndBorrow           └──────────┘
        │      │
        │      │   BorrowMut(&متغير س)  ┌──────────────┐
        │      │ ──────────────────►   │ BorrowedMut  │
        │      │                        │ (حصري)       │
        │      │ ◄──────────────────    │              │
        │      │    EndBorrow           └──────────────┘
        │      │
        │      │   انقل س → ت             ┌────────┐
        │      │ ─────────────────────►   │ Moved  │ (لا يستخدم)
        │      │                          └────────┘
        │      │
        │      │   نهاية النطاق           ┌──────────┐
        │      │ ─────────────────────►   │ Dropped  │ (Drop مولّد)
        └──────┘                          └──────────┘
```

### 11.3 أخطاء الفحص (compile-time)

```cpp
enum class OwnershipErrorKind {
    UseAfterMove,           // استخدام بعد النقل
    UseAfterDrop,           // استخدام بعد الإسقاط
    DoubleFree,             // تحرير مزدوج
    DoubleMove,             // نقل مزدوج
    BorrowOfMoved,          // استعارة من قيمة منقولة
    MutBorrowWhileBorrowed, // استعارة متغيرة أثناء وجود استعارات
    UseWhileMutBorrowed,    // استخدام أثناء استعارة متغيرة
    InvalidBorrow           // استعارة غير صالحة
};
```

### 11.4 تعليمات SIR للملكية (12)

طبقة وسيطة بين AST و LLVM IR — تجعل عمليات الملكية صريحة:

```
┌─────────────────────────────────────────────────────────┐
│  #  | Opcode      | الصياغة             | الوصف         │
├─────┼─────────────┼─────────────────────┼───────────────┤
│  1  │ Alloc       │ %r = Alloc(نوع)    │ تخصيص        │
│  2  │ Borrow      │ %r = Borrow(%v)    │ استعارة &     │
│  3  │ BorrowMut   │ %r = BorrowMut(%v) │ استعارة &mut  │
│  4  │ Move        │ %r = Move(%v)      │ نقل ملكية     │
│  5  │ Copy        │ %r = Copy(%v)      │ نسخ سكلر     │
│  6  │ Drop        │ Drop(%v)           │ تحرير صريح    │
│  7  │ Clone       │ %r = Clone(%v)     │ نسخ عميق     │
│  8  │ EndBorrow   │ EndBorrow(%ref)    │ إنهاء استعارة │
│  9  │ Reborrow    │ %r = Reborrow(%ref)│ إعادة استعارة │
│ 10  │ Project     │ %r = Project(%v,حق)│ وصول حقل     │
│ 11  │ Deref       │ %r = Deref(%ref)   │ فك مرجع       │
│ 12  │ Take        │ %r = Take(%c, idx) │ أخذ من حاوية  │
└─────────────────────────────────────────────────────────┘
```

### 11.5 مثال: من كود ص إلى SIR إلى LLVM

```sad
دالة جمع(أ، ب)
    متغير ن = أ + ب         # ن: مملوك
    ارجع ن                   # نقل الملكية للخارج
نهاية

دالة رئيسية()
    متغير س = جمع(3، 5)
    اطبع_سطر(س)             # س ينتهي عمره هنا → Drop
نهاية
```

**SIR المولَّد (مبسَّط):**
```
fn جمع(%أ: i64, %ب: i64) -> i64:
  %0  = Alloc(i64)            ; ن
  %1  = Add(%أ, %ب)
        Store(%0, %1)
  %2  = Move(%0)              ; نقل ن للإرجاع
        ret %2

fn رئيسية():
  %0  = Alloc(i64)            ; س
  %1  = Call(جمع, [3, 5])
        Store(%0, %1)
  %2  = Borrow(%0)            ; للطباعة (read-only)
        Call(اطبع_سطر, [%2])
        EndBorrow(%2)
        Drop(%0)              ; ◄── مولَّد آلياً عند نهاية النطاق
        ret void
```

**LLVM IR النهائي (مفهوم):** `Drop` يصبح `free()` أو destructor call،
بدون أي عبء وقت تشغيل لتتبع المراجع.

---

## 12. نموذج الملكية في المُفسّر (الديناميكي)

`interpreter/src/managers/ownership_manager.cpp` هو **wrapper رفيع**
فوق `Sad::Semantic::OwnershipTracker` نفسه — لكن دوره **تحذيري** لا قاتل:

| الجانب | المترجم (sadc) | المُفسّر |
|--------|----------------|---------|
| **التوقيت** | وقت الترجمة | وقت التشغيل |
| **الصرامة** | فشل البناء عند الخطأ | تحذير + متابعة |
| **التحرير** | `Drop` مولّد في IR | GC يُحرّر |
| **الأداء** | صفر تكلفة | بسيط (hashmap lookup) |

**لماذا الازدواجية؟**
- المُفسّر يفيد من **اكتشاف الأخطاء قبل الترجمة** للتطوير السريع.
- المترجم يفيد من نفس المحرك ليضمن **توافق الدلالة** بين الوضعين.

---

## 13. مسارات التنفيذ الثلاثة — تفصيل إدارة الذاكرة

### 13.1 مسار المُفسّر (`sad.exe ملف.ص`)

```
ملف.ص
  │
  ├─ Lex → Parse → AST
  │
  ├─ [اختياري] OwnershipManager.check(AST)  ← تحذيرات فقط
  │
  ▼
InterpreterCore.run(AST)
  │
  ├─ كل تعبير ينشئ Value
  │     ├─ سكلر (int/double/bool/string) → في-Value مباشرة
  │     ├─ ARRAY/MAP/TUPLE → shared_ptr داخلي (refcount C++)
  │     └─ OBJECT (صنف) → ObjectInstance* خام
  │              │
  │              └─► GC.registerObject(ptr, destroyer, visitor)
  │
  ├─ VariableManager يحفظ Value في scopeVariables_
  │     └─► RootProvider يصدر كل OBJECT حي عند mark
  │
  ├─ نهاية scope → eraseScope()
  │     └─► OBJECT يصبح unreachable من الجذور
  │
  └─ GC.collect() دورياً (لاحقاً B-step5b-vi)
        ├─ mark: من roots_ + rootProviders_
        └─ sweep: destroyer لكل غير مُعلَّم
```

### 13.2 مسار الـ VM (`sad --vm ملف.ص`)

```
ملف.ص → AST → BytecodeCompiler → Bytecode → VM.execute()

الحالة الراهنة:
  ├─ سكلر يعمل (int/double/bool/string)
  ├─ مصفوفات/خرائط: محدودة
  └─ OOP constructors: ❌ غير مدعومة (B-step5c)

نموذج الذاكرة:
  ├─ Operand stack: قيم Value على المكدس (نفس النظام)
  ├─ Locals: مصفوفة Value في StackFrame
  └─ Heap: نفس GC المشترك (لكن لا يوجد provider بعد)

العمل المتبقّي:
  ├─ B-step5b-iv: ربط RootProvider لـ StackFrames و operand stacks
  └─ B-step5c: دعم تعليمات NEW_OBJECT, CALL_METHOD, ...
```

### 13.3 مسار المترجم (`sadc ملف.ص → ملف.exe`)

```
ملف.ص
  │
  ├─ Lex → Parse → AST
  │
  ├─ run_borrow_check(AST)            ← shared/ownership
  │     ├─ OwnershipTracker
  │     ├─ BorrowChecker
  │     ├─ LifetimeAnalyzer
  │     └─ ❌ فشل → خطأ ترجمة
  │
  ├─ AST → SIRBuilder → SIR (مع 12 ownership ops)
  │
  ├─ SIROptimizer
  │     ├─ ownership_analysis.cpp ← فحص ثاني على SIR
  │     ├─ drop_elaboration       ← إدراج Drop التلقائي
  │     └─ borrow_inlining
  │
  ├─ SIR → LLVMCodeGen → LLVM IR
  │     ├─ Alloc/StackAlloc → alloca
  │     ├─ HeapAlloc → call malloc
  │     ├─ Drop → call free (أو destructor)
  │     ├─ Borrow/Deref → load/store
  │     └─ Move → memcpy + invalidate (debug)
  │
  └─ LLVM → ملف تنفيذي أصلي (لا GC، لا runtime overhead)
```

**النتيجة:** ملف `.exe` لا يحوي محرك GC ولا refcounting — فقط
`malloc`/`free` صريحة مولَّدة من تحليل الملكية.

---

## 14. خريطة القرار — متى يُستخدم كل نظام؟

```
                    ┌──────────────────────┐
                    │ هل النوع سكلر بسيط؟   │
                    │ (int/double/bool/str)│
                    └──────────┬───────────┘
                               │
                    ┌──────────┴──────────┐
                   نعم                    لا
                    │                     │
                    ▼                     ▼
          ┌──────────────────┐  ┌──────────────────────┐
          │ نسخة بالقيمة     │  │ هل المسار مُفسّر/VM؟  │
          │ (لا تتبع لازم)   │  └──────────┬───────────┘
          └──────────────────┘             │
                                ┌──────────┴──────────┐
                               نعم                    لا (sadc)
                                │                     │
                                ▼                     ▼
                      ┌─────────────────┐  ┌──────────────────────┐
                      │ ARRAY/MAP/TUPLE │  │ تحليل ملكية ساكن    │
                      │  → shared_ptr   │  │ + توليد Drop آلي    │
                      │ OBJECT          │  │ (صفر تكلفة تشغيل)   │
                      │  → GC + raw ptr │  └──────────────────────┘
                      └─────────────────┘
```

---

## 15. التفاعل بين GC والملكية — المستقبل

**حالياً:** النظامان منفصلان تماماً (المُفسّر يستخدم GC، المترجم يستخدم Ownership).

**الرؤية بعيدة المدى (مرحلة B-step6+):**

```
نوع الكائن              | سياسة الإدارة المثلى
────────────────────────┼────────────────────────────────
كائن بمالك واحد واضح    │ Drop مولّد ساكناً (لا GC)
كائن بمشاركة محدودة      │ Rc/Arc (refcount)
كائن بمشاركة معقدة/دورية │ GC mark-and-sweep
كائن قصير العمر (scope)  │ Stack allocation
```

هذا "نموذج هجين" يستخدمه Swift و Nim بنجاح. لغة ص في موقع جيد لتبنيه
لاحقاً لأن:
1. شجرة الـ AST تحوي معلومات كافية (الـ AST الموحد).
2. SIR يدعم 12 opcode ملكية بالفعل.
3. GC جاهز للحالات الديناميكية المعقدة.

---

## 16. ملخص الحالة الكلية لإدارة الذاكرة

| المسار | السكلر | المجموعات (Array/Map) | الكائنات (OOP) | الفحص الساكن |
|--------|--------|----------------------|----------------|--------------|
| **interpreter** | by-value | shared_ptr داخلي | 🟢 GC + RootProvider (B-iii) | تحذيري |
| **vm** | by-value | shared_ptr داخلي | 🟡 GC مسجَّل لكن بلا provider | لا |
| **sadc** | stack/reg | malloc/free مع Drop | malloc/free مع Drop | 🟢 صارم |

| المكوّن | الموقع | الحالة | يعمل في |
|---------|--------|--------|---------|
| GarbageCollector | `shared/memory_gc/` | 🟢 يعمل | interp / (vm جزئياً) |
| RootProvider API | `shared/memory_gc/` | 🟢 جديد (B-iii) | interp |
| Value::forEachObjectRef | `shared/types/` | 🟢 جديد (B-iii) | الجميع |
| OwnershipTracker | `shared/ownership/` | 🟢 يعمل | sadc / (interp تحذيري) |
| BorrowChecker | `shared/ownership/` | 🟢 يعمل | sadc |
| SIR Ownership Ops (12) | `compiler/src/sir/` | 🟢 يعمل | sadc |
| Drop Elaboration | `compiler/src/sir/` | 🟡 جزئي | sadc |
| Lifetime Analysis | `shared/ownership/` | 🟡 أساسي | sadc |
| Hybrid GC+Ownership | — | 🔴 رؤية مستقبلية | — |

---

## 17. الأسئلة المتكررة

**س: لماذا لا نستخدم نظام الملكية في المُفسّر بشكل صارم بدلاً من GC؟**
ج: المُفسّر يجب أن يدعم REPL وكوداً تجريبياً غير مكتمل. الفشل الصارم
يعطّل التجربة. التحذيرات + GC يعطيان أفضل توازن.

**س: هل GC سيوجد في النواة (ufuq OS)؟**
ج: نعم — GC مصمم ليكون freestanding. لكن النواة ستعتمد بشكل أكبر
على الملكية الساكنة (sadc) وستستخدم GC للكائنات الديناميكية فقط.

**س: ما الفرق بين `Drop` في SIR و `destroyer` في GC؟**
ج: `Drop` يُولَّد ساكناً ويصبح `free()` في الـ exe. `destroyer` يُسجَّل
ديناميكياً مع كل كائن ويُستدعى أثناء `sweep()`. كلاهما يحرر الذاكرة
نفسها لكن في زمن مختلف.

**س: هل يمكن أن يحدث leak في المسار المُفسّر؟**
ج: حالياً نعم بسبب الـ fallback المحافظ في `markFromRootsLocked`
(عند عدم وجود جذور، يعتبر كل شيء حياً). يُحلّ في B-step5b-v.

---

> **آخر تحديث:** 2025-11-20 — كوميت `20ca8991` (هذا التوثيق نفسه)