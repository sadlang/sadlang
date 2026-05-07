# الطبقة المشتركة `shared/` في لغة ص

> **الغرض:** خريطة معمارية تفصيلية لمحتوى مجلد [shared/](../../shared/) — النواة المشتركة بين المُفسِّر، المُترجم، الـ VM، وكل الأدوات (LSP، Formatter، REPL). تشرح **ما** يوجد، **لماذا** فُصل بهذا الشكل، وبالأخص **لماذا** توجد **أربعة** مجلدات منفصلة لإدارة الذاكرة بدلاً من مجلد واحد.
>
> **الجمهور:** المطورون الذين يضيفون ميزات جديدة، يُعيدون هيكلة الطبقة المشتركة، أو يحاولون فهم الفروق بين `memory_gc/` و`memory_policy/` و`ownership/` و`ownership_runtime/`.

---

## 📚 فهرس سريع

| ما تبحث عنه | اذهب إلى |
|---|---|
| نظرة كلية على `shared/` | [§1 خريطة المجلدات](#1-خريطة-shared-كاملة) |
| الـ16 مكوّناً المشتركاً | [§2 المكونات](#2-المكونات-الستة-عشر-بالتفصيل) |
| لماذا 4 مجلدات للذاكرة؟ | [§3 لغز الذاكرة](#3-لغز-الذاكرة-لماذا-أربعة-مجلدات) |
| المقارنة بين الطبقات الأربع | [§3.4 جدول المقارنة](#34-جدول-مقارنة-المجلدات-الأربعة) |
| تدفق طلب GC من المستخدم إلى الكائن | [§3.5 تدفق التطبيق](#35-تدفق-طلب-gc-من-المستخدم-إلى-كائن-فعلي) |
| لماذا لم يتم الدمج؟ | [§4 قواعد منع الدمج](#4-القواعد-السبع-التي-تمنع-الدمج) |
| المكتبات المُولَّدة CMake | [§5 المكتبات](#5-مكتبات-cmake-المُولَّدة) |
| ملخص بصري نهائي | [§6 ملخص بصري](#6-ملخص-بصري-نهائي) |
| مراجع | [§7 مراجع](#7-مراجع-داخلية) |

---

## 1. خريطة `shared/` كاملة

```
shared/
│
├─ 🔤 lexer/              ← المحلل المعجمي (10 ملفات)
├─ 🌳 parser/             ← المحلل النحوي (38 ملف)
├─ 📜 ast/                ← شجرة بناء جملة مجردة (31 ملف)
├─ 🔢 types/              ← نظام الأنواع وقت التشغيل Sad::Data (19 ملف)
├─ 🛡️ semantic/           ← فاحص الأنواع المشترك (4 ملفات)
├─ ⚠️ errors/             ← نظام الأخطاء + Smart Errors (54 ملف)
│
├─ 💾 memory_policy/      ← سياسات وضع الذاكرة (--dev/--prod/--learn) (4 ملفات)
├─ ♻️ memory_gc/          ← محرك جامع القمامة + كاشف الدورات (7 ملفات)
├─ 🔒 ownership/          ← فاحص الملكية وقت الترجمة (15 ملف)
├─ 🏃 ownership_runtime/  ← متتبع الملكية وقت التشغيل (4 ملفات)
│
├─ 🛡️ security/           ← BoundsChecker, SafeArithmetic, ... (9 ملفات)
├─ 📦 modules/            ← نظام الاستيراد import/export (16 ملف)
├─ 🔌 builtins/           ← السجل المركزي للدوال المضمنة (13 ملف)
├─ 🔧 utils/              ← أدوات نصية مساعدة (4 ملفات)
├─ 📊 profiler/           ← قياس الأداء (3 ملفات)
└─ 🔄 hot_reload/         ← إعادة التحميل الفوري (4 ملفات)
```

### المخطط المعماري العام

```
                    ┌──────────────────────────────────────┐
                    │     جميع المسارات التنفيذية           │
                    │  ┌──────┐  ┌────────┐  ┌─────┐      │
                    │  │ sad  │  │ sadc   │  │ vm  │ ...  │
                    │  └──┬───┘  └────┬───┘  └──┬──┘      │
                    └─────┼───────────┼─────────┼─────────┘
                          │           │         │
                          ▼           ▼         ▼
   ╔══════════════════════════════════════════════════════════╗
   ║                    shared/  (النواة)                       ║
   ║                                                            ║
   ║   ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐     ║
   ║   │ lexer   │→ │ parser  │→ │  ast    │  │ types   │     ║
   ║   └─────────┘  └─────────┘  └─────────┘  └─────────┘     ║
   ║                                                            ║
   ║   ┌─────────┐  ┌─────────┐  ┌─────────────────────────┐  ║
   ║   │semantic │  │ errors  │  │ 💾 طبقة الذاكرة (4 مجلدات)│  ║
   ║   └─────────┘  └─────────┘  │ memory_policy/           │  ║
   ║                              │ memory_gc/               │  ║
   ║   ┌─────────┐  ┌─────────┐  │ ownership/               │  ║
   ║   │security │  │ modules │  │ ownership_runtime/       │  ║
   ║   └─────────┘  └─────────┘  └─────────────────────────┘  ║
   ║                                                            ║
   ║   ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌──────────┐   ║
   ║   │builtins │  │  utils  │  │profiler │  │hot_reload│   ║
   ║   └─────────┘  └─────────┘  └─────────┘  └──────────┘   ║
   ╚══════════════════════════════════════════════════════════╝
```

---

## 2. المكونات الستة عشر بالتفصيل

### 2.1 الطبقات الأمامية (Frontend Core)

| المجلد | الملفات | المكتبة | الدور |
|---|---|---|---|
| **[lexer/](../../shared/lexer/)** | 10 | داخل `sad_shared` | تحويل النص إلى Tokens. يشمل [lexer_keywords.cpp](../../shared/lexer/src/lexer_keywords.cpp) و`keywords_generated.cpp` (مولّد من [data/language/keywords.yaml](../../data/language/keywords.yaml) — Codegen v4.1) |
| **[parser/](../../shared/parser/)** | 38 | داخل `sad_shared` | بناء AST من Tokens. مقسّم إلى: `core/`، `declarations/`، `statements/`، `ui/` |
| **[ast/](../../shared/ast/)** | 31 | داخل `sad_shared` | عقد AST + clone عميق + printer |
| **[types/](../../shared/types/)** | 19 | داخل `sad_shared` | نظام الأنواع وقت التشغيل: `Value`, `ObjectInstance`, `ClassType`, `TypeBridge` (للجسر مع `compiler/src/types/`) |

> **ملاحظة:** الطبقات الأربع أعلاه **لا تملك `CMakeLists.txt` خاصاً** — كلها مدمجة في مكتبة واحدة `sad_shared` للسرعة وتجنب اعتمادات دائرية بين الـ tokens والـ AST.

### 2.2 الطبقات التحليلية (Analysis)

| المجلد | الملفات | المكتبة | الدور |
|---|---|---|---|
| **[semantic/](../../shared/semantic/)** | 4 | `sad_semantic_shared` | فاحص الأنواع المشترك (نُقل من `compiler/` في Phase 3 لإصلاح اعتماد المُفسِّر على المُترجم) |
| **[errors/](../../shared/errors/)** | 54 | داخل `sad_shared` | كتالوج الأخطاء (Phase 1) + Runtime templates (Phase 4، 9 ملفات حسب الفئة الدلالية) + Smart Errors (Phase 15، 8 ملفات) + Error builders dispatch |

### 2.3 طبقة الذاكرة (Memory) — **محور الوثيقة**

| المجلد | الملفات | المكتبة | الدور المختصر |
|---|---|---|---|
| **[memory_policy/](../../shared/memory_policy/)** | 4 | `sad_memory_policy` | يقرأ أعلام `--dev/--prod/--learn` ويوحّد الإعدادات |
| **[memory_gc/](../../shared/memory_gc/)** | 7 | `sad_memory_gc` | محرك GC + كاشف الدورات + `policy_bridge` |
| **[ownership/](../../shared/ownership/)** | 15 | `sad_ownership` | فحص الملكية وقت الترجمة (borrow checker، lifetime، move) |
| **[ownership_runtime/](../../shared/ownership_runtime/)** | 4 | `sad_ownership_runtime` | متتبع الملكية وقت التشغيل + C-ABI |

**المجموع: 30 ملف موزعة على 4 مكتبات.** القسم §3 يشرح بالتفصيل **لماذا**.

### 2.4 طبقات الخدمة (Services)

| المجلد | الملفات | المكتبة | الدور |
|---|---|---|---|
| **[security/](../../shared/security/)** | 9 | `sad_security_core` | `BoundsChecker`, `SafeArithmetic`, `InputSanitizer`, `SafeAllocator`, `TaintTracker` تحت `Sad::Security` |
| **[modules/](../../shared/modules/)** | 16 | داخل `sad_shared` | استيراد/تصدير الوحدات: dependency_graph، module_loader، resolver، validator، cache |
| **[builtins/](../../shared/builtins/)** | 13 | داخل `sad_shared` | السجل المركزي الموحّد للدوال المضمنة (ADR-003) |
| **[utils/](../../shared/utils/)** | 4 | داخل `sad_shared` | أدوات نصية مساعدة (string_utils) |
| **[profiler/](../../shared/profiler/)** | 3 | منفصلة | قياس الأداء |
| **[hot_reload/](../../shared/hot_reload/)** | 4 | منفصلة | إعادة التحميل الفوري للملفات `.ص` |

---

## 3. لغز الذاكرة: لماذا أربعة مجلدات؟

> **السؤال المحوري:** إذا كانت الأربعة كلها "ذاكرة"، **لماذا لم تُوضع تحت مجلد واحد** مثل `shared/memory/{policy,gc,ownership,runtime}/`؟

الإجابة المختصرة: **ليست كلها "ذاكرة" بالمعنى نفسه.** كل مجلد يمثل **مرحلة مختلفة** من دورة حياة البرنامج، **وعمر مختلف** للكود، **وعملاء مختلفين** يستهلكون كل واحدة. الدمج كان سيُنشئ **اعتمادات دائرية** ويكسر **CW-02 (تسلسل الطبقات)**.

دعنا نفصّل كل واحدة:

### 3.1 `memory_policy/` — *النوايا*

```
shared/memory_policy/
├─ include/memory/policy/
│  ├─ gc_mode.h               ← هل GC مفعّل؟ ما استراتيجيته؟
│  └─ memory_mode_flag.h      ← --dev | --prod | --learn
└─ src/
   ├─ gc_mode.cpp
   └─ memory_mode_flag.cpp
```

**ما يفعله:**
- يقرأ سطر الأوامر: `--dev`, `--prod`, `--learn`, `--gc=on/off/cycle-only`
- يُحوّلها إلى كائن `MemoryModeSettings` موحّد
- لا يُنفّذ أي شيء — مجرد **مُعرِّفات** و **enums**

**عمر الكود:** بداية البرنامج فقط (مرة واحدة).

**العملاء:** المُفسِّر (CLI sad)، المُترجم (sadc)، الـ VM، REPL، LSP — كلهم يحتاجون قراءة نفس الإعدادات.

> **لماذا منفصل عن `memory_gc/`؟** لأن **القرار** (الـ policy) أبسط بكثير من **التنفيذ** (الـ engine). LSP مثلاً يحتاج معرفة الإعدادات لتحذيرات ملائمة، **لكنه لا يحتاج** ربط محرك GC كامل (5 آلاف سطر).

### 3.2 `memory_gc/` — *المحرك*

```
shared/memory_gc/
├─ include/memory/gc/
│  ├─ engine/garbage_collector.h     ← المحرك الفعلي
│  ├─ cycles/cycle_detector.h        ← كشف الإسناد الدوري
│  └─ policy_bridge.h                ← الجسر مع memory_policy
└─ src/
   ├─ engine/garbage_collector.cpp
   ├─ cycles/cycle_detector.cpp
   ├─ cycles/cycle_detector_impl.cpp
   └─ policy_bridge.cpp              ← يربّب hooks على ObjectInstance
```

**ما يفعله:**
- يدير **حلقة جمع القمامة** الفعلية (Mark & Sweep + Cycle Detection)
- يُركّب **lifecycle hooks** على `Sad::Data::ObjectInstance` (إنشاء/هدم)
- يستهلك `MemoryModeSettings` من `memory_policy/` ويُطبّقها

**عمر الكود:** طوال تنفيذ البرنامج (heap-bound).

**العملاء:** المُفسِّر، الـ VM، الكود المُولَّد من sadc — **لكن ليس** LSP أو Formatter.

> **لماذا منفصل عن `memory_policy/`؟** لأن المحرك يعتمد على `Sad::Data::ObjectInstance` (من `types/`) ويحتاج runtime كامل. سياسة الـ policy تحتاج فقط enums بسيطة. **اعتماد عكسي محظور (CW-02):** policy لا يجب أن يعرف عن GC.

### 3.3 `ownership/` — *المُصحِّح وقت الترجمة*

```
shared/ownership/
├─ include/ownership/
│  ├─ borrow_checker.h          ← فحص الاستعارات (& و &mut)
│  ├─ lifetime_analyzer.h       ← تحليل عمر القيم
│  ├─ move_analyzer.h           ← تتبع نقل الملكية
│  ├─ ownership_tracker.h       ← متتبع الحالة
│  └─ unsafe_checker.h          ← فحص كتل @غير_آمن
└─ src/
   ├─ borrow_checker.cpp + borrow_checker_visitors.cpp
   ├─ cascade_prevention.cpp    ← منع تتالي أخطاء الملكية
   ├─ exhaustiveness.cpp        ← شمولية مطابقة الأنماط
   ├─ lifetime_analyzer.cpp
   ├─ move_analyzer.cpp
   ├─ ownership_tracker.cpp
   ├─ pattern_ownership.cpp     ← ملكية في destructuring
   ├─ this_binding.cpp          ← قواعد this
   └─ unsafe_checker.cpp
```

**ما يفعله:**
- يفحص الـ AST **قبل** التنفيذ/التوليد
- يكشف: `use-after-move`, `borrow-conflict`, `dangling-reference`
- يُنتج **أخطاء وقت ترجمة** فقط — لا كود تنفيذي

**عمر الكود:** مرحلة الترجمة/التحليل (لا يعمل في وقت التشغيل).

**العملاء:** المُفسِّر (للوضع `--prod`)، المُترجم (دائماً)، LSP (تحذيرات)، أداة الفحص.

> **لماذا منفصل عن `memory_gc/`؟** لأن GC يعمل وقت التشغيل، أما borrow checker فيعمل **قبل** التنفيذ تماماً. خلطهما يعني أن LSP (الذي يحتاج فقط الفحص) سيُجبر على ربط محرك GC كامل. **انتهاك مباشر لـ CW-21 (Clear Interfaces)**.

### 3.4 `ownership_runtime/` — *المتتبع وقت التشغيل*

```
shared/ownership_runtime/
├─ include/ownership/runtime/
│  ├─ ownership_tracker.h          ← Singleton آمن للخيوط
│  └─ sad_ownership_capi.h         ← C-ABI (sad_ownership_*)
└─ src/
   ├─ ownership_tracker.cpp
   └─ sad_ownership_capi.cpp
```

**ما يفعله:**
- يتتبع المؤشرات الخام (`raw pointers`) في **وقت التشغيل** للوضع `--prod`
- يعرض **C-ABI** (`extern "C"`) ليتمكن مخرج sadc من استدعائه **بدون ربط رموز C++**
- مُنشئ DEF-001 لحل مشكلة: المُترجم يُنتج .exe يحتاج تتبع ملكية، لكن لا يستطيع ربط الـ C++ من المُترجم نفسه

**عمر الكود:** وقت تشغيل البرامج المُترجَمة فقط.

**العملاء:** الكود المُولَّد من sadc حصراً (في الوضع `--prod`).

> **لماذا منفصل عن `ownership/`؟** لأن `ownership/` يحتوي C++ كاملاً مع AST visitors و templates و RTTI. الكود المُترجَم لـ bare-metal لا يستطيع ربط هذا (لا libc، لا STL). فُصل ABI مستقل بـ `extern "C"` ليكون **freestanding-safe**.

### 3.4 جدول مقارنة المجلدات الأربعة

| المعيار | `memory_policy/` | `memory_gc/` | `ownership/` | `ownership_runtime/` |
|---|---|---|---|---|
| **العمر** | بداية البرنامج | طوال التنفيذ | قبل التنفيذ (compile) | طوال التنفيذ |
| **التكلفة** | بضع KB | ~MB (heap) | يعمل مرة | بضع KB |
| **يعتمد على STL/RTTI** | لا | نعم | نعم | **لا** (freestanding) |
| **يعتمد على AST** | لا | لا | **نعم** | لا |
| **يعتمد على ObjectInstance** | لا | **نعم** | لا | لا |
| **يُطلق exceptions C++** | لا | نعم | نعم | **لا** (يُرجع رموز خطأ) |
| **يُستخدم من LSP** | نعم | لا | نعم (تحذيرات) | لا |
| **يُستخدم من sadc output** | لا (مُدمج) | لا | لا | **نعم** |
| **يُستخدم من VM** | نعم | نعم | لا | لا |
| **مكتبة مستقلة** | `sad_memory_policy` | `sad_memory_gc` | `sad_ownership` | `sad_ownership_runtime` |

### 3.5 تدفق طلب GC من المستخدم إلى كائن فعلي

هذا المخطط يُظهر **كيف تتعاون الأربعة معاً** بدون اعتماد دائري:

```
┌──────────────────────────────────────────────────────────────────┐
│ 1. المستخدم: sad-run app.ص --gc=on --learn                       │
└─────────────────────────────┬────────────────────────────────────┘
                              ▼
┌──────────────────────────────────────────────────────────────────┐
│ 2. CLI Parser → memory_policy/                                   │
│    MemoryModeSettings { gc: ON, mode: LEARN, ownership: SOFT }   │
└─────────────────────────────┬────────────────────────────────────┘
                              ▼
┌──────────────────────────────────────────────────────────────────┐
│ 3. ownership/ — يفحص AST قبل التنفيذ                              │
│    LEARN mode → يُحوّل الأخطاء إلى تحذيرات + اقتراحات            │
└─────────────────────────────┬────────────────────────────────────┘
                              ▼
┌──────────────────────────────────────────────────────────────────┐
│ 4. memory_gc/policy_bridge.cpp                                   │
│    applyMemoryModeSettings(settings):                            │
│      → garbage_collector.start()                                 │
│      → ObjectInstance::installLifecycleHooks(onCreate, onDelete) │
└─────────────────────────────┬────────────────────────────────────┘
                              ▼
┌──────────────────────────────────────────────────────────────────┐
│ 5. التنفيذ يبدأ — الكائنات تُسجّل تلقائياً عبر Hooks                │
│    cycle_detector يفحص دورات الإسناد دورياً                        │
└──────────────────────────────────────────────────────────────────┘

      🔁 ملاحظة: لو كان sadc بدلاً من sad-run:
              ownership_runtime/ بديلاً عن خطوات 3+4
              عبر C-ABI (sad_ownership_track / sad_ownership_release)
```

---

## 4. القواعد السبع التي تمنع الدمج

| # | القاعدة | الذي يكسرها الدمج |
|---|---|---|
| **1** | **CW-01 (SRP)**: كل مكتبة لها مسؤولية واحدة | دمج policy+gc يجمع "اتخاذ القرار" مع "التنفيذ" |
| **2** | **CW-02 (Layered)**: الطبقات لا تتقاطع | gc يحتاج types/، ownership يحتاج ast/ — اعتمادات مختلفة |
| **3** | **CW-21 (Clear Interfaces)**: واجهة لكل عميل | LSP يحتاج policy فقط، sadc-output يحتاج runtime فقط |
| **4** | **Freestanding Safety**: ownership_runtime بدون STL | الدمج يجبره على ربط C++ كامل (يكسر bare-metal) |
| **5** | **Compile-time vs Runtime**: ownership/ ≠ ownership_runtime/ | الأول لا يوجد في binary المُترجم، الثاني فقط هو الموجود |
| **6** | **Header isolation**: ASTNode.h ضخم — لا يجب أن يصل لـ runtime | دمج ownership+ownership_runtime يجبر runtime على تضمينه |
| **7** | **Build time**: مكتبة GC وحدها = ~2 ثانية build، مدمجة = ~30 ثانية | LSP/Formatter لا يحتاجان GC أصلاً |

### مثال على الكسر المحتمل لو دُمجت

```cpp
// لو كانت كل الذاكرة في shared/memory/ موحَّدة:
// LSP server يحتاج فقط فحص الملكية لتحذيرات الكود

#include "memory/all.h"   // ← يجلب: GC engine + heap + AST visitors + ABI

// النتيجة:
// 1. LSP يُربط بـ ~5MB من كود GC غير ضروري
// 2. ompile time لـ LSP × 10
// 3. binary حجم LSP يتضخم
// 4. أي تغيير في GC engine يُعيد compile كل LSP
```

---

## 5. مكتبات CMake المُولَّدة

```
sad_shared (مكتبة جامعة كبيرة)
    │
    ├─ lexer/    (ضمنياً)
    ├─ parser/   (ضمنياً)
    ├─ ast/      (ضمنياً)
    ├─ types/    (ضمنياً)
    ├─ errors/   (ضمنياً)
    ├─ modules/  (ضمنياً)
    ├─ builtins/ (ضمنياً)
    └─ utils/    (ضمنياً)

مكتبات منفصلة (للسماح بالاستهلاك المستقل):
    │
    ├─ sad_semantic_shared       (shared/semantic/)
    ├─ sad_security_core         (shared/security/)
    │
    ├─ sad_memory_policy         (shared/memory_policy/)
    ├─ sad_memory_gc             (shared/memory_gc/)
    ├─ sad_ownership             (shared/ownership/)
    │   └─ alias: sad_semantic   (للتوافق الخلفي)
    ├─ sad_ownership_runtime     (shared/ownership_runtime/)
    │
    └─ (profiler, hot_reload — مكتبات منفصلة)
```

**لماذا `sad_shared` مكتبة عملاقة بدلاً من تقسيم Lexer/Parser/AST/Types؟**

- اعتمادات دائرية حقيقية: `Token` يحتاج `Position`، `AST` يحتاج `Token`، `Parser` يحتاج `AST`، الـ Types يحتاج `ObjectInstance` الذي يحتاج `Value` الذي يحتاج Tokens للـ literals.
- تقسيمها لمكتبات منفصلة يتطلب forward declarations ضخمة وعقد templates.
- التقسيم الحالي = **توازن عملي**: المكونات شديدة الاقتران معاً، والمكونات قابلة للفصل (security، memory_*، ownership_*) منفصلة.

---

## 6. ملخص بصري نهائي

```
╔══════════════════════════════════════════════════════════════════════╗
║                      shared/  — النواة المشتركة                       ║
╠══════════════════════════════════════════════════════════════════════╣
║                                                                      ║
║  ┌─────────────────── CORE (sad_shared) ────────────────────────┐   ║
║  │                                                              │   ║
║  │  lexer → parser → ast → types → errors → modules → builtins  │   ║
║  │         (مدمجة في مكتبة واحدة بسبب الاقتران الشديد)              │   ║
║  └──────────────────────────────────────────────────────────────┘   ║
║                                                                      ║
║  ┌──────────────── ANALYSIS (مكتبات مستقلة) ─────────────────────┐   ║
║  │                                                              │   ║
║  │  sad_semantic_shared    sad_security_core                    │   ║
║  └──────────────────────────────────────────────────────────────┘   ║
║                                                                      ║
║  ┌─────────── 💾 MEMORY LAYER (4 مكتبات منفصلة) ─────────────────┐   ║
║  │                                                              │   ║
║  │  ┌──────────────────┐         ┌────────────────────┐         │   ║
║  │  │ memory_policy    │ ◄────── │  memory_gc         │         │   ║
║  │  │ (نوايا، enums)    │ يقرأ   │  (محرك GC + cycles)│         │   ║
║  │  │ خفيف، LSP-safe   │         │  ثقيل، runtime    │         │   ║
║  │  └──────────────────┘         └────────────────────┘         │   ║
║  │                                                              │   ║
║  │  ┌──────────────────┐         ┌────────────────────┐         │   ║
║  │  │ ownership        │         │ ownership_runtime  │         │   ║
║  │  │ (compile-time)   │         │ (C-ABI، freestanding)│        │   ║
║  │  │ borrow check     │         │ للـ sadc output    │         │   ║
║  │  └──────────────────┘         └────────────────────┘         │   ║
║  │                                                              │   ║
║  │  📌 أربعة مجلدات لأن: عمر مختلف + عملاء مختلفون +              │   ║
║  │     compile-time vs runtime + freestanding-safety              │   ║
║  └──────────────────────────────────────────────────────────────┘   ║
║                                                                      ║
║  ┌──────────────── SERVICES (مكتبات صغيرة) ──────────────────────┐   ║
║  │                                                              │   ║
║  │  profiler    hot_reload                                      │   ║
║  └──────────────────────────────────────────────────────────────┘   ║
║                                                                      ║
╠══════════════════════════════════════════════════════════════════════╣
║   يستهلكها: sad-run, sad-build, vm, lsp, formatter, repl, tests    ║
╚══════════════════════════════════════════════════════════════════════╝
```

---

## 7. مراجع داخلية

- طبقات المُفسِّر/المُترجم: [docs/architecture/interpreter_compiler_layers.md](./interpreter_compiler_layers.md)
- نظام الأنواع المزدوج: [compiler/docs/type_system_layers.md](../../compiler/docs/type_system_layers.md)
- طبقات المحسن: [compiler/docs/optimization_layers.md](../../compiler/docs/optimization_layers.md)
- معايير builtins: [interpreter/src/builtins/BUILTIN_CODING_STANDARDS.md](../../interpreter/src/builtins/BUILTIN_CODING_STANDARDS.md)
- ملاحظات مرحلة Memory Unification: `/memories/repo/architecture_refactor_status.md`
- ملاحظات DEF-001 (ownership_runtime): راجع `shared/CMakeLists.txt:217`

---

> **خلاصة معمارية:** الفصل الرباعي لطبقة الذاكرة ليس "تشتيت" — بل **استثمار في العزل** يسمح لـ LSP أن يبقى خفيفاً، ولـ sadc-output أن يبقى freestanding-safe، ولـ GC engine أن يتطور بدون كسر فاحص الملكية، ولـ borrow checker أن يعمل بدون runtime. الدمج يكسر كل ذلك.
