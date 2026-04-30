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