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

**خاتمة الجزء الأول:** هذه المرحلة تمثّل الخطوة الجوهرية في بناء محرك
GC حقيقي يحلّ محل `shared_ptr<ObjectInstance>` ويُمهّد لجميع المسارات
استخدام نفس المحرك تحت سيطرة `MemoryPolicyManager`.

---

# الجزء الثاني — نظام إدارة الذاكرة الموحَّد عبر جميع المسارات

> هذا الجزء يصف **الرؤية المعمارية الكاملة** كما هي مُعرَّفة في
> [معمارية الذاكرة الموحَّدة](معمارية_الذاكرة_الموحدة.md) ومذكرة المستودع
> `architecture_refactor_status.md`. التركيز هنا على **التكامل** بين
> الأنظمة الفرعية بدلاً من تكرار التفاصيل.

## 10. المبدأ الأساسي — المطوِّر يختار، اللغة تنفّذ

لغة ص توفّر **نظامين لإدارة الذاكرة** يعملان جنباً إلى جنب:

| النظام | الموقع | الفائدة |
|--------|--------|---------|
| **نظام الملكية (Ownership)** | `shared/ownership/` + runtime في `shared/memory/ownership/runtime/` | أمان وقت ترجمة + صفر تكلفة في `--prod` |
| **جامع المهملات (GC)** | `shared/memory_gc/` (نواة) + `shared/memory/gc/` (سياسة) | سهولة التطوير + كشف دورات في `--dev` |

**الاختيار يقع على المطوِّر** عبر علامات سطر الأوامر:

```bash
sad ملف.ص --prod    # ملكية صارمة، لا GC، أخطاء فورية
sad ملف.ص --learn   # ملكية متساهلة (تحذيرات)، لا GC — للتعليم
sad ملف.ص --dev     # GC مُفعَّل + ملكية إرشادية (تحذيرات)

sadc ملف.ص --prod   # نفس الدلالة في الكود التنفيذي المولَّد
sadc ملف.ص --dev    # ربط runtime يحوي GC + enforcer
```

> ⚠️ **قاعدة دلالية صارمة:** نفس البرنامج بنفس الوضع يجب أن يُنتج
> **نفس النتيجة الدلالية** عبر المسارات الثلاثة (interpreter / vm / sadc).
> أي خرق لهذه القاعدة = خطأ معماري.

---

## 11. الطبقة الموحَّدة `shared/memory/` — مصدر الحقيقة

```
shared/memory/
├── policy/                                  ← السياسة (المرحلة A — مكتملة)
│   ├── memory_mode.h           : enum DEVELOPMENT/PRODUCTION/LEARN
│   ├── memory_mode_flag.h      : تحليل --dev/--prod/--learn من argv
│   └── memory_policy_manager.h : مصدر الحقيقة الوحيد للوضع
│
├── gc/                                      ← GC الموحَّد (المرحلة B — قيد التنفيذ)
│   ├── garbage_collector.{h,cpp}  ← يلتف حول shared/memory_gc/
│   ├── gc_roots.{h,cpp}            ← API للجذور المشتركة
│   └── cycle_detector.{h,cpp}      ← من compiler/src/memory/
│
└── ownership/runtime/                       ← تنفيذ الملكية وقت التشغيل
    ├── runtime_ownership_enforcer.{h,cpp}  ← يفوّض إلى Semantic::OwnershipTracker
    └── ownership_runtime_api.{h,cpp}        ← C-ABI لـ sadc-compiled binaries
```

### العلاقة مع `shared/memory_gc/`

`shared/memory_gc/` هو **نواة منخفضة المستوى** لـ GC (mark-and-sweep,
RootProvider, destroyer/visitor) — مستقلة عن السياسة. `shared/memory/gc/`
هو **الواجهة عالية المستوى** التي:
- تستشير `MemoryPolicyManager` قبل التفعيل.
- تختار: `--prod` → تجاوز كامل لـ GC. `--dev` → تفعيل + جمع دوري.
- تكشف API لجميع المسارات.

---

## 12. تكامل الأنظمة في كل مسار

### 12.1 المُفسّر (`sad ملف.ص [--mode]`)

```
                  argv → MemoryModeFlag.parse()
                              │
                              ▼
                  MemoryPolicyManager.setMode()
                              │
       ┌──────────────────────┼──────────────────────┐
       │                      │                      │
       ▼                      ▼                      ▼
   --prod                  --learn                  --dev
       │                      │                      │
       ▼                      ▼                      ▼
 OwnershipMgr                OwnershipMgr           OwnershipMgr
  (صارم،                     (تحذيرات              (تحذيرات
   ارمي خطأ)                  فقط)                  + GC مُفعَّل)
       │                      │                      │
       │                      │                      ▼
       │                      │                  GC.enable()
       │                      │                  RootProviders
       │                      │                  مُسجَّلة لـ:
       │                      │                  - VariableManager ✅
       │                      │                  - ChannelManager 🟡
       │                      │                  - FutureManager 🟡
       │                      │                  - UI handlers 🟡
       ▼                      ▼                      ▼
   Drop آلي عند             لا تحرير               GC.collect()
   انتهاء النطاق             (يعتمد المستخدم        دورياً
   (مولَّد ساكناً               على نهاية البرنامج)    
    أو ديناميكياً)
```

### 12.2 الآلة الافتراضية (`sad --vm ملف.ص [--mode]`)

VM يستخدم **نفس** الطبقة الموحَّدة:
- نفس `Value` (نفس `SadType` cache).
- نفس `OwnershipManager` كما المُفسّر.
- نفس GC engine — لكن جذور الـ VM (operand stack, stack frames)
  تحتاج RootProvider مستقل (B-step5b-iv).

```
StackFrame                 OperandStack
  │ locals[]: Value          │ values[]: Value
  │                          │
  └────────┬─────────────────┘
           │  (مستقبلاً)
           ▼
   GC.addRootProvider([&](emit){
     for frame in callStack:
       for v in frame.locals:
         v.forEachObjectRef(emit);
     for v in operandStack:
       v.forEachObjectRef(emit);
   })
```

### 12.3 المترجم (`sadc ملف.ص [--mode]` → ملف تنفيذي)

```
ملف.ص
  │
  ├─ Lex → Parse → AST
  │
  ├─ MemoryPolicyManager.setMode(mode)
  │
  ├─ run_borrow_check(AST)          ← shared/ownership (دائماً)
  │     ├─ --prod  : فشل البناء عند أي خطأ
  │     ├─ --learn : تحذيرات + متابعة
  │     └─ --dev   : تحذيرات + اعتماد GC في runtime
  │
  ├─ AST → SIR (مع 12 ownership ops)
  │
  ├─ SIROptimizer
  │     ├─ ownership_analysis      ← فحص ثاني
  │     └─ drop_elaboration         ← إدراج Drop
  │           ├─ --prod : Drop → free() مباشر (لا GC linkage)
  │           └─ --dev  : Drop → GC_decRef()  (مع GC runtime)
  │
  └─ SIR → LLVM IR → ملف تنفيذي
        مع ربط:
        ├─ --prod : libsad_runtime_minimal.a  (بلا GC)
        └─ --dev  : libsad_runtime_full.a     (GC + cycle detector + enforcer)
```

---

## 13. نظام الأنواع الموحَّد — `SadType` وعلاقته بالذاكرة

> **مهم:** القسم 1 السابق ذكر `std::variant<...>` كتفصيل تنفيذي. الواقع
> أن `Value` يستخدم نظام الأنواع الموحَّد `SadType` المعتمد منذ ADR-01.

### 13.1 المكونات

| المكون | الدور |
|--------|------|
| `Sad::Types::SadType` | أصل كل الأنواع (مع `getKind()`, `isMutable_`, `lifetimeName_`) |
| `Sad::Types::SadTypeKind` (enum) | `Void/Integer/Float/String/Boolean/Array/Map/Tuple/Class/Function/...` |
| `Sad::Types::SadTypePtr` | `shared_ptr<const SadType>` — كل قيمة تحمل واحدة |
| `Sad::Types::SadTypeRegistry` | factory مركزي (`getInteger()`, `getString()`, `makeFunction(...)`, ...) |
| `Sad::Data::Value` | يحمل `SadTypePtr sadType_` + بيانات حقيقية |

### 13.2 لماذا يهم لإدارة الذاكرة؟

`SadType` يحمل معلومات حاسمة لكلا النظامين:

```
SadType
  ├─ getKind()           ← ما النوع؟ (Integer, Class, Array, ...)
  ├─ isCopyable()        ← هل يمكن نسخه (Copy) أم يجب نقله (Move)؟
  ├─ isMutable_          ← هل يقبل BorrowMut؟
  ├─ lifetimeName_       ← العمر (`'أ`, `'static`, ...)
  └─ getTypeParams()     ← للأنواع المركبة (Array<T>, Map<K,V>)
```

هذا يسمح للمُفسّر/المترجم باتخاذ قرارات دقيقة:
- `Integer` → `isCopyable()` = true → نسخ بالقيمة، لا حاجة لـ GC.
- `Class` → `isCopyable()` = false → ينتقل عبر `Move` أو يُسجَّل في GC.
- `Array<Class>` → عناصرها OBJECT → `forEachObjectRef` يتعمّق.

### 13.3 خريطة `SadTypeKind` → سياسة الذاكرة

| Kind | Ownership | GC | كيف يُدار؟ |
|------|-----------|----|------------|
| `Void`, `Boolean`, `Integer`, `Float` | Copy (تلقائي) | غير مُسجَّل | على المكدس / في-Value |
| `String` | Move + Clone | غير مُسجَّل | shared_ptr داخلي (immutable) |
| `Array`, `Map`, `Tuple` | Move + ref | غير مُسجَّل (المحتوى ربما) | shared_ptr داخلي + تعمّق GC |
| `Class` (OBJECT) | Move + Borrow | **مُسجَّل في GC** ✅ | `ObjectInstance*` خام + RootProvider |
| `Function` | Copy (closure) | غير مُسجَّل | shared_ptr<FunctionRef> |

> **تصحيح:** القسم 1 من هذا المستند ذكر `std::variant<...>` كتفصيل
> داخلي. هذا صحيح كآلية تخزين، لكن **الواجهة المنطقية** هي `SadType`،
> وأي قرار حول الذاكرة يجب أن يستند إلى `value.getKind()` لا إلى
> `value.is<T>()` في الـ variant.

---

## 14. أنواع أخطاء الملكية المدعومة في جميع المسارات

عبر `Sad::Semantic::OwnershipErrorKind`:

| الخطأ | المُفسّر (`--learn`) | المُفسّر (`--prod`) | sadc (`--prod`) |
|------|---------------------|---------------------|-----------------|
| `UseAfterMove` | تحذير | استثناء `SadOwnershipError` | فشل بناء |
| `UseAfterDrop` | تحذير | استثناء | فشل بناء |
| `DoubleFree` | تحذير | استثناء | فشل بناء |
| `DoubleMove` | تحذير | استثناء | فشل بناء |
| `BorrowOfMoved` | تحذير | استثناء | فشل بناء |
| `MutBorrowWhileBorrowed` | تحذير | استثناء | فشل بناء |
| `UseWhileMutBorrowed` | تحذير | استثناء | فشل بناء |
| `InvalidBorrow` | تحذير | استثناء | فشل بناء |

في وضع `--dev`: نفس الفحوصات تجري لكن لا تُفشل البرنامج — تُلتقط
الأخطاء كتحذيرات قابلة للعرض في LSP/REPL، والذاكرة تُدار بـ GC.

---

## 15. تعليمات SIR للملكية (12 — موحَّدة)

طبقة وسيطة بين AST و LLVM IR — تجعل عمليات الملكية صريحة وقابلة للتحسين:

```
┌─────────────────────────────────────────────────────────┐
│  #  | Opcode      | الصياغة             | الوصف         │
├─────┼─────────────┼─────────────────────┼───────────────┤
│  1  │ Alloc       │ %r = Alloc(SadType)│ تخصيص        │
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

**ملاحظة:** كل opcode يحمل `SadTypePtr` (وليس `ValueType` القديم).
هذا يسمح بنفس opcode أن يُترجم إلى:
- `--prod` LLVM IR: `alloca` + `free` صريح من `Drop`.
- `--dev` LLVM IR: `GC_alloc` + `GC_decRef` من `Drop`.

---

## 16. مثال شامل — نفس الكود، ثلاثة أوضاع

```sad
دالة جمع(أ، ب)
    متغير ن = أ + ب
    ارجع ن
نهاية

دالة رئيسية()
    متغير س = جمع(3، 5)
    اطبع_سطر(س)
نهاية
```

| الوضع | الفحص الساكن | كيف تُحرَّر `س`؟ |
|------|---------------|-------------------|
| `--prod` (المُفسّر) | OwnershipMgr يفحص ويرمي عند الخطأ | عند نهاية scope (Drop ديناميكي) |
| `--prod` (sadc) | borrow checker يفشل البناء عند الخطأ | `free()` مولَّد في الـ exe |
| `--learn` | تحذيرات فقط | عند نهاية البرنامج (لا تحرير فوري) |
| `--dev` (المُفسّر) | تحذيرات + GC نشط | GC.collect() دورياً |
| `--dev` (sadc) | تحذيرات + ربط runtime GC | `GC_decRef()` في الـ exe |

> **النتيجة:** نفس البرنامج، نفس الدلالة في كل الأوضاع — يختلف فقط
> **متى وكيف** تتحرر الذاكرة، وفق اختيار المطوِّر.

---

## 17. خريطة الحالة الكلية لإدارة الذاكرة

| المكون | الموقع | الحالة | يستخدم في |
|--------|--------|--------|-----------|
| `MemoryMode` enum | `shared/memory/policy/` | 🟢 مكتمل (Phase A1) | الجميع |
| `MemoryModeFlag` parser | `shared/memory/policy/` | 🟢 مكتمل (Phase A2) | الجميع |
| `MemoryPolicyManager` | `shared/memory/policy/` | 🟢 مكتمل (Phase A2) | الجميع |
| `GarbageCollector` (نواة) | `shared/memory_gc/` | 🟢 يعمل | interp / (vm جزئي) |
| `RootProvider` API | `shared/memory_gc/` | 🟢 جديد B-iii | interp |
| `Value::forEachObjectRef` | `shared/types/` | 🟢 جديد B-iii | الجميع |
| `shared/memory/gc/` (واجهة) | `shared/memory/gc/` | 🟡 قيد النقل | — |
| `OwnershipTracker` | `shared/ownership/` | 🟢 يعمل | الجميع (مشترك) |
| `BorrowChecker` | `shared/ownership/` | 🟢 يعمل | sadc / interp |
| `RuntimeOwnershipEnforcer` | `shared/memory/ownership/runtime/` | 🟡 قيد التصميم | runtime |
| `OwnershipManager` (interp) | `interpreter/src/managers/` | 🟢 wrapper | interp |
| SIR Ownership Ops (12) | `compiler/src/sir/` | 🟢 يعمل | sadc |
| Drop Elaboration | `compiler/src/sir/` | 🟡 جزئي | sadc |
| Mode-aware runtime linking | `compiler/CMakeLists.txt` | 🔴 لاحقاً | sadc |
| VM RootProvider | `vm/` | 🔴 B-step5b-iv | vm |
| Channels/Futures RootProvider | `interpreter/` | 🔴 B-step5b-iv | interp/vm |

---

## 18. نقاط مهمة للمطوِّر

### للمستخدم النهائي (كاتب كود ص)
- **افتراضياً = `--dev`**: استخدم اللغة كأي لغة GC عادية.
- **إذا أردت أداءً أقصى**: استخدم `--prod` وتعلَّم الملكية.
- **إذا تتعلَّم**: `--learn` يعطي تحذيرات بدلاً من إفشال البرنامج.

### للمساهم في اللغة
- لا تكتب كوداً يفترض GC أو Ownership — استشر `MemoryPolicyManager`.
- لا تستخدم `value.is<X>()` للقرار حول الذاكرة — استخدم `value.getKind()`.
- أي مكون يحوي `ObjectInstance*` خام يجب أن يُسجِّل RootProvider.
- لا تعتمد على `shared_ptr<ObjectInstance>` — تم حذفه (B-step5b-ii).

### للمطوِّر على النواة (ufuq OS)
- استخدم `--prod` دائماً — لا runtime GC في kernel space.
- جميع الملكية تُفحص ساكناً → ملف `.elf` لا يحوي GC.
- إذا احتجت GC في حقل معيّن (مثل user-space helpers) — استخدم `--dev`
  مع `libsad_runtime_full.a` المربوط بنواة منفصلة.

---

## 19. مراجع داخلية

- وثيقة الذاكرة الموحَّدة الكاملة: [docs/معمارية_الذاكرة_الموحدة.md](معمارية_الذاكرة_الموحدة.md)
- نظام الأنواع: مذكرة المستودع `unified_type_system.md` (ADR-01 Phase 1-5)
- وثيقة الأمان الموحَّد: [docs/الأمان_الموحد.md](الأمان_الموحد.md)
- خطة إعادة الهيكلة: [docs/خطة_إعادة_الهيكلة_المعمارية.md](خطة_إعادة_الهيكلة_المعمارية.md)
- مرجع SIR opcodes: `compiler/src/sir/sir_opcodes.h`
- مرجع OwnershipTracker: `shared/ownership/include/ownership/ownership_tracker.h`

---

> **آخر تحديث:** بعد مراجعة الرؤية الموحَّدة (نظامان متعايشان عبر
> `MemoryMode` + نظام أنواع موحَّد `SadType`).

