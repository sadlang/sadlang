# مركز الأدوات الموحَّد للغة ص (Sad Tool Hub)

> **النسخة:** 1.1 (نهائية - مُعتمَدة)  
> **الحالة:** قيد التنفيذ — قرارات المستخدم محسومة  
> **التاريخ:** يناير 2026  
> **مرتبط بـ:** [architecture-tools-breakdown.md](architecture-tools-breakdown.md), [architecture-summary.md](architecture-summary.md)

> ⚠️ **سِجِلٌّ لا خارطةُ طريق — اقرأ هذا أوّلًا.**
>
> كُتِبَت هذه الوثيقةُ حينَ كان للّغةِ محرّكان: مفسّرٌ ومترجم. وقد **حُذِفَ
> المفسّرُ**، فما فيها من `sad-run.exe` و`sad-repl.exe` ومكوّنِ `sad-runtime`
> وفعلِ `sad run` **لا وجودَ له**، ومعاييرُ القبولِ في القسمِ ٩ التي تشترطُ
> `sad-run hello.ص` قد بطَلَ شرطُها.
>
> والوثيقةُ باقيةٌ كما كُتِبَت لأنّها **سِجلُّ قرارٍ**، ومحوُ ما قِيلَ يومَ قيلَ
> يُفقِدُ السجلَّ قيمتَه. وما بقيَ قائمًا منها هو المركزُ نفسُه (`sad.exe`
> يُنادي `sad build` و`sad check` و`sad fmt`) واكتشافُ الأدواتِ من `PATH`.
>
> وقد **زالت قاعدةُ تركيبِها في الحزم** (`cmake/install.cmake`): وثيقةُ تصميمٍ
> داخليّةٌ لا تُشحَنُ إلى المستخدم.

---

## ١. المشكلة (Problem Statement)

### الوضع الحالي
أدوات لغة ص موزَّعة عشوائياً كتنفيذيات منفصلة بلا تنظيم موحَّد:

| الأداة | التنفيذي | التسمية |
|---|---|---|
| المفسّر | `sad.exe` | بدون شرطة |
| المترجم | `sadc.exe` | بدون شرطة |
| REPL | `sad-repl.exe` | شرطة وسطى |
| Profiler | `sad-profiler.exe` / `sad_profiler` | اسمان مختلفان! |
| LSP | `sad-lsp.exe` | شرطة وسطى |
| Formatter | `sad-fmt.exe` | شرطة وسطى |
| APK | `sad-apk.exe` | شرطة وسطى |
| Analyzer | `sad-analizer.exe` | إملاء خاطئ (analizer ← analyzer) |
| Pkg | (مكتبة فقط) | لا تنفيذي |
| WASM | غير مكتمل | — |

### المشاكل الحرجة
1. **تسمية فوضوية:** `sadc` vs `sad-fmt` vs `sad_profiler` — لا نمط موحَّد
2. **لا واجهة مركزية:** المستخدم يحتاج تذكُّر 8+ أسماء تنفيذيات
3. **لا اكتشاف ذاتي:** `sad --help` لا يعرف بقية الأدوات
4. **تكرار CLI parsing:** كل أداة تُنفّذ تحليل معطياتها
5. **خلط أسماء بـإملاء خاطئ:** `analizer` بدلاً من `analyzer`
6. **لا حزم توزيع منفصلة:** المطور يضطر تحميل الكل أو لا شيء

### المتطلبات الأساسية (من قرار المستخدم)
> "المطور قد لا يحتاج للمترجم أو المفسّر، لذلك سنحتاج إلى تصميم يلبي كل الاحتياجات:
> - إمكانية تحميل المترجم فقط
> - إمكانية تحميل المفسر فقط
> - إمكانية تحميل أداة معينة فقط
> - إمكانية تحميل مركز الأدوات بحيث يحتوي الكل"

هذا يعني التصميم **يجب** أن يدعم:
- ✅ توزيع كل أداة كحزمة مستقلة قابلة للتشغيل وحدها
- ✅ توزيع المفسّر وحده
- ✅ توزيع المترجم وحده
- ✅ توزيع "Hub Bundle" يحوي الكل + موزِّع موحَّد
- ✅ المستخدم النهائي يمكنه تركيب أدوات إضافية لاحقاً (plug-in style)

---

## ٢. النموذج المعماري المختار: **Hub-as-Optional-Frontend** ⭐

### المبدأ الأساسي
> **كل أداة = تنفيذي مستقل قائم بذاته (standalone)** — لا يعتمد على أي أداة أخرى للعمل.  
> **مركز الأدوات (`sad-hub`) = واجهة اختيارية فوقية** تُوزَّع كحزمة منفصلة وتكتشف الأدوات وتوجّه إليها.

### مقارنة بالنماذج الأخرى

| النموذج | المرجع | لماذا رفضناه |
|---|---|---|
| **Monolith** (cargo) | كل شيء داخل تنفيذي واحد | ❌ يخالف متطلب "تحميل أداة وحدها" |
| **Dispatcher رفيع** (git) | git الأساسي + git-* subprocess | ✅ قريب جداً — لكن git يتطلب git الأساسي دائماً |
| **Hub اختياري** (مقترَحنا) | كل أداة قائمة بذاتها + hub اختياري | ✅ يلبي كل المتطلبات |

### الفرق الجوهري عن git
- في git: لا يمكن تشغيل `git-log` مباشرة بسهولة — git هو نقطة الدخول
- في تصميمنا: يمكن تشغيل `sad-fmt file.ص` مباشرة دون أي حاجة لـ `sad-hub`
- `sad-hub` فقط يُضيف **الراحة** (`sad fmt file.ص`) لكن لا يُضيف قدرة جديدة

---

## ٣. التسمية الموحَّدة (Naming Convention)

### قاعدة صارمة
كل التنفيذيات تتبع نمطاً واحداً: **`sad-<role>[.exe]`** (شرطة وسطى، حروف صغيرة)

| الأداة | الاسم القديم | الاسم الجديد | ملاحظة |
|---|---|---|---|
| المفسّر | `sad.exe` | `sad-run.exe` | + alias `sad.exe` للتوافقية ✅ |
| المترجم | `sadc.exe` | `sad-build.exe` | + alias `sadc.exe` للتوافقية ✅ |
| REPL | `sad-repl.exe` | `sad-repl.exe` | بدون تغيير |
| LSP | `sad-lsp.exe` | `sad-lsp.exe` | بدون تغيير |
| Formatter | `sad-fmt.exe` | `sad-fmt.exe` | بدون تغيير |
| Profiler | `sad-profiler.exe` | `sad-profile.exe` | يطابق الفعل |
| Analyzer | `sad-analizer.exe` | `sad-analyze.exe` | إصلاح إملاء + توحيد |
| Package | (لا يوجد) | `sad-pkg.exe` | جديد |
| APK | `sad-apk.exe` | `sad-apk.exe` | بدون تغيير |
| WASM | (غير مكتمل) | `sad-wasm.exe` | جديد |
| Check | (لا يوجد) | `sad-check.exe` | جديد — ownership/types بدون تنفيذ |
| Hub | (لا يوجد) | `sad-hub.exe` أو `sad.exe` | الموزِّع |

### قاعدة الـ aliases للتوافقية الخلفية
- `sad.exe` يبقى موجوداً ويسلك سلوكاً ذكياً:
  - إذا وُجد `sad-hub.exe` بجانبه ⇒ يفوّض إليه (يصبح Hub)
  - إذا لم يوجد ⇒ يسلك كالمفسّر القديم (يستدعي `sad-run.exe` داخلياً أو يكون نسخة منه)
- `sadc.exe` يبقى موجوداً كـ symlink/copy لـ `sad-build.exe`

---

## ٤. حزم التوزيع (Distribution Packages)

### المستوى ١: حزم فردية (Individual Packages)
كل واحدة تعمل وحدها بشكل كامل دون الحاجة للأخرى:

| الحزمة | المحتوى | الحجم التقريبي | الجمهور |
|---|---|---|---|
| `sad-runtime` | `sad-run.exe` + stdlib مشتركة | ~15 MB | تشغيل سكربتات `.ص` فقط |
| `sad-compiler` | `sad-build.exe` + LLVM libs + stdlib | ~80 MB | بناء تنفيذيات أصلية |
| `sad-fmt` | `sad-fmt.exe` فقط | ~3 MB | المنسِّق وحده |
| `sad-lsp` | `sad-lsp.exe` فقط | ~5 MB | لمحرّرات الأكواد |
| `sad-check` | `sad-check.exe` (ownership/types) | ~4 MB | فحص ثابت بدون تنفيذ |
| `sad-tools-extra` | profile, analyze, apk, wasm | ~20 MB | أدوات متقدمة |

### المستوى ٢: حزم مُجمَّعة (Bundles)

| الحزمة | المحتوى | الجمهور |
|---|---|---|
| `sad-developer` | runtime + compiler + fmt + check + lsp + hub | المطوّر العادي |
| `sad-minimal` | runtime + fmt | المستخدم الخفيف |
| `sad-everything` | كل الحزم + hub | المطوّر المتقدم |

### المستوى ٣: التركيب الإضافي (Add-ons)
- المستخدم لديه `sad-runtime` → يُركّب `sad-fmt` لاحقاً → كلاهما يعمل
- إذا ركّب `sad-hub` بعدها → يكتشف كليهما تلقائياً ويوفّر `sad fmt`, `sad run`

---

## ٥. مركز الأدوات (Hub) — التصميم التفصيلي

### مسؤوليات الـHub
1. **اكتشاف الأدوات (Discovery):** فحص PATH + مجلدات قياسية للعثور على `sad-*.exe`
2. **التوزيع (Dispatch):** ترجمة `sad <cmd> args...` إلى `sad-<cmd>.exe args...`
3. **رسائل المساعدة الموحَّدة:** `sad --help` يعرض كل الأدوات المتوفرة
4. **اقتراحات ذكية:** `sad biuld` → "هل تقصد `sad build`؟"
5. **معلومات النسخة:** `sad --version` يعرض إصدارات كل الأدوات الموجودة
6. **التهيئة المشتركة:** متغيرات بيئة، ألوان، مخرجات JSON موحَّدة

### مسؤوليات Hub **لا** تشمل
- ❌ تنفيذ منطق فعلي (لا يحتوي مفسّراً ولا مترجماً)
- ❌ تكرار وظائف الأدوات
- ❌ فرض ترتيب تركيب معيّن

### مكان البحث عن الأدوات (Tool Discovery Order)
```
1. مجلد التنفيذي نفسه (PATH/to/sad-hub.exe/)
2. PATH/to/sad-hub.exe/tools/
3. $SAD_TOOLS_PATH (متغير بيئة)
4. PATH العام للنظام
```

### أمثلة استخدام
```bash
# مع Hub
sad run hello.ص          # → sad-run.exe hello.ص
sad build hello.ص         # → sad-build.exe hello.ص
sad fmt hello.ص           # → sad-fmt.exe hello.ص
sad check hello.ص         # → sad-check.exe hello.ص
sad --help                # قائمة كل الأدوات

# بدون Hub (مباشر)
sad-run hello.ص           # نفس الشيء بالضبط
sad-build hello.ص
sad-fmt hello.ص

# توافقية خلفية
sad hello.ص               # يُكتشف ملف .ص → يستدعي sad-run تلقائياً
sadc hello.ص              # alias قديم لـ sad-build
```

---

## ٦. البنية المعمارية الجديدة

### هيكل المجلدات المقترَح
```
tools/
├─ hub/                                    ← جديد
│  ├─ include/hub/
│  │  ├─ tool_registry.h                   ← اكتشاف وتسجيل
│  │  ├─ command_dispatcher.h              ← التوزيع
│  │  ├─ tool_descriptor.h                 ← وصف كل أداة (اسم، مسار، نسخة)
│  │  └─ help_renderer.h                   ← عرض المساعدة الموحَّد
│  ├─ src/
│  │  ├─ main.cpp                          ← entry point للـHub
│  │  ├─ tool_registry.cpp
│  │  ├─ command_dispatcher.cpp
│  │  ├─ help_renderer.cpp
│  │  └─ suggestions.cpp                   ← اقتراحات "هل تقصد"
│  ├─ tests/
│  │  ├─ test_discovery.cpp
│  │  ├─ test_dispatch.cpp
│  │  └─ test_suggestions.cpp
│  ├─ CMakeLists.txt                       ← مكتبة sad_hub_lib + تنفيذي sad-hub
│  └─ README.md
│
├─ check/                                  ← جديد (sad-check)
│  ├─ include/check/checker_main.h
│  ├─ src/main.cpp                         ← يستخدم sad_ownership مباشرة
│  └─ CMakeLists.txt
│
├─ run/                                    ← جديد (إعادة تنظيم sad.exe)
│  ├─ src/main.cpp                         ← entry للمفسّر
│  └─ CMakeLists.txt                       ← ينتج sad-run.exe + alias sad.exe
│
├─ build/                                  ← يُعاد تنظيمه (sad-build بدل sadc)
│  ├─ src/main.cpp                         ← entry للمترجم
│  └─ CMakeLists.txt                       ← ينتج sad-build.exe + alias sadc.exe
│
├─ compiler/                               ← يبقى كـ "مكتبة compiler" فقط (sad_compiler)
│  └─ src/                                 ← منطق الترجمة (يُستهلك من tools/build/)
│
├─ fmt/                                    ← rename من formatter/
│  └─ ... (نفس البنية)
│
├─ lsp/                                    ← يبقى
├─ profile/                                ← rename من profiler/
├─ analyze/                                ← rename من analizer/ (إصلاح إملاء)
├─ pkg/                                    ← يضاف entry point sad-pkg.exe
├─ apk_builder/                            ← يبقى
└─ wasm/                                   ← يكتمل ويضاف entry
```

### مخطط الاعتماديات

```
                    ┌─────────────────┐
                    │    sad-hub.exe  │  (اختياري)
                    └────────┬────────┘
                             │ يكتشف ويستدعي (subprocess)
        ┌────────────────────┼────────────────────┐
        │           │        │        │           │
        ▼           ▼        ▼        ▼           ▼
  sad-run.exe  sad-build  sad-fmt  sad-check  sad-lsp.exe
   (مستقل)    .exe       .exe     .exe       (مستقل)
        │       │           │        │
        ▼       ▼           ▼        ▼
   sad_core  sad_compiler sad_fmt sad_ownership   ← مكتبات shared
```

كل تنفيذي يربط فقط المكتبات التي يحتاجها — Hub لا يربط أيّاً منها.

### مكتبة `sad_hub_lib`
- API صغيرة جداً: `discoverTools()`, `dispatchCommand()`, `renderHelp()`
- لا تعتمد على `sad_core` ولا `sad_compiler`
- اختبارات معزولة كاملة

---

## ٧. خطة التنفيذ المرحلية (Rollback-Safe)

### المرحلة H1 — البنية الأساسية (لا يكسر شيئاً)
- إنشاء `tools/hub/` كهيكل فارغ
- إنشاء مكتبة `sad_hub_lib` مع `tool_registry` فقط
- إنشاء `sad-hub.exe` يطبع "Hub v0.1 — لم تُكتشف أدوات بعد"
- اختبار وحدة لـ `discoverTools()`
- **التحقق:** البناء ينجح، الأدوات القديمة لم تتأثر

### المرحلة H2 — التوزيع (Dispatch)
- تنفيذ `command_dispatcher.cpp`
- `sad-hub run x.ص` يستدعي `sad.exe x.ص` (الاسم القديم) كـ subprocess
- `sad-hub --help` يعرض الأدوات المكتشفة
- **التحقق:** `sad-hub run examples/01_مرحبا.ص` ينتج نفس مخرج `sad.exe examples/01_مرحبا.ص`

### المرحلة H3 — `sad-check` (الأداة الجديدة المطلوبة)
- إنشاء `tools/check/`
- `main.cpp` يستخدم `Sad::Semantic::OwnershipTracker` و `Sad::Semantic::TypeChecker` مباشرة
- مخرجات JSON اختيارية (`--json`)
- اختبارات على عيّنات `.ص` تحتوي انتهاكات ملكية معروفة
- **التحقق:** `sad-check broken.ص` يُرجع رمز خطأ غير صفري + رسالة دقيقة، بدون تنفيذ الكود

### المرحلة H4 — إعادة التسمية (مع توافقية خلفية)
- إضافة `tools/run/` ينتج `sad-run.exe` + alias `sad.exe` (نفس المحتوى)
- إضافة `tools/build/` ينتج `sad-build.exe` + alias `sadc.exe`
- rename `tools/formatter/` → `tools/fmt/` (اسم المجلد فقط، الـtarget يبقى)
- rename `tools/profiler/` → `tools/profile/` + الإخراج `sad-profile.exe`
- rename `tools/analizer/` → `tools/analyze/` + إصلاح كل المراجع
- **التحقق:** كل الاستدعاءات القديمة تستمر بالعمل، الأسماء الجديدة متوفرة بالتوازي

### المرحلة H5 — Discovery الذكي + الاقتراحات
- Hub يكتشف أدوات في PATH + `$SAD_TOOLS_PATH` + مجلده
- اقتراحات Levenshtein لـtypos
- `sad --version` يعرض إصدار كل أداة مكتشفة
- **التحقق:** اختبارات unit + integration

### المرحلة H6 — حزم التوزيع
- CMake/CPack components: `sad-runtime`, `sad-compiler`, `sad-fmt`, `sad-check`, `sad-tools-extra`, `sad-developer`, `sad-everything`
- سكربتات بناء `scripts/build_packages.ps1`
- اختبار: تركيب `sad-fmt` وحده ⇒ يعمل، ثم تركيب `sad-hub` ⇒ يكتشفه

### المرحلة H7 — التوثيق والتنظيف
- تحديث README الجذر
- تحديث `docs/architecture-tools-breakdown.md`
- إعلان رسمي: الأسماء القديمة مدعومة لـ12 شهراً ثم deprecation warning
- إزالة أي تكرار متبقٍّ

---

## ٨. تقييم المخاطر

| الخطر | الاحتمال | التأثير | التخفيف |
|---|---|---|---|
| كسر سكربتات تستخدم `sad.exe` | عالي | عالي | الـaliases تبقى للأبد، ليس deprecation |
| تشتت الإصدارات (sad-fmt 1.0 + sad-run 2.0) | متوسط | متوسط | Hub يحذّر إذا اكتُشف عدم تطابق major version |
| زيادة وقت بدء التشغيل (subprocess overhead) | منخفض | منخفض | فقط 5-15ms — مقبول لـCLI |
| مستخدمون لا يجدون `sad-hub` | متوسط | منخفض | المثبّت يضيفه لـPATH افتراضياً |
| تعقيد البناء (10 تنفيذيات) | متوسط | متوسط | CMake components + سكربت بناء واحد |

---

## ٩. معايير القبول (Acceptance Criteria)

### وظيفية
- [ ] `sad-run hello.ص` يعمل دون وجود أي أداة أخرى
- [ ] `sad-build hello.ص` يعمل دون وجود `sad-run` ولا `sad-hub`
- [ ] `sad-fmt hello.ص` يعمل وحده
- [ ] `sad-check broken.ص` يكتشف انتهاك ملكية بدون تنفيذ
- [ ] `sad-hub run hello.ص` ⇔ `sad-run hello.ص` (نفس المخرج بالضبط)
- [ ] `sad-hub --help` يعرض كل الأدوات المكتشفة
- [ ] `sad-hub --list` يعرض المسارات وإصدارات الأدوات
- [ ] `sad biuld` (typo) يقترح "هل تقصد build؟"
- [ ] `sad.exe hello.ص` (السلوك القديم) يبقى يعمل

### معمارية
- [ ] `sad-hub` لا يعتمد على `sad_core` ولا `sad_compiler` ولا أي مكتبة ضخمة
- [ ] كل تنفيذي ≤ 80 MB (سقف للضخامة)
- [ ] لا توجد عمليات استدعاء متبادلة بين الأدوات (DAG وحيد الاتجاه)
- [ ] حزمة `sad-runtime` لا تحتوي LLVM
- [ ] حزمة `sad-fmt` ≤ 5 MB

### اختبارية
- [ ] اختبارات unit لـ`tool_registry` (بحث، اكتشاف، تصفية)
- [ ] اختبارات unit لـ`command_dispatcher` (mock subprocess)
- [ ] اختبارات integration: `sad-hub` يستدعي `sad-run` فعلاً
- [ ] اختبار توافقية: `sad.exe x.ص` و `sadc.exe x.ص` يبقيان يعملان
- [ ] جميع الاختبارات الـ900+ الحالية تبقى ناجحة

---

## ١٠. القرارات المعتمدة (محسومة)

| القرار | الاختيار النهائي |
|---|---|
| اسم Hub | **`sad.exe` يصبح Hub** — المفسّر القديم يصير `sad-run.exe`، يُستدعى عبر `sad run x.ص` |
| توافقية sad.exe القديم | **لا توافقية** — `sad x.ص` لم يعد يُشغِّل تلقائياً؛ يجب `sad run x.ص` صراحةً (يُعرض خطأ واضح إذا لم يُحدَّد أمر) |
| حزم التوزيع | **المستويات الثلاثة كاملة** (فردية + bundles + add-ons) |
| ترتيب التنفيذ | **دفعة واحدة H1→H7** حتى النهاية |

### تأثير قرار "sad.exe = Hub"
- `sadc.exe` يبقى alias لـ`sad-build.exe` (لم يُمَس بقرار 2)
- السكربتات/CI الحالية التي تستخدم `sad.exe x.ص` **ستحتاج تحديثاً** إلى `sad-run x.ص` أو `sad run x.ص`
- تُحدَّث جميع الاختبارات (~900) واختبارات التماثل لاستخدام `sad-run.exe`
- تُحدَّث جميع وثائق `docs/` و README

---

## ١١. الخلاصة

التصميم المختار **Hub-as-Optional-Frontend** يحقق المتطلبات الأربعة المطلوبة:

| المتطلب | كيف يُلبَّى |
|---|---|
| تحميل المترجم فقط | حزمة `sad-compiler` (sad-build.exe + LLVM libs) |
| تحميل المفسر فقط | حزمة `sad-runtime` (sad-run.exe + stdlib) |
| تحميل أداة معينة فقط | كل أداة لها حزمة خاصة (`sad-fmt`, `sad-check`, ...) |
| تحميل مركز الأدوات | حزمة `sad-developer` أو `sad-everything` تحوي Hub + كل شيء |

**المبدأ الذهبي:** Hub لا يضيف قدرات — يضيف راحة. كل أداة قائمة بذاتها.

---

**جاهز للموافقة قبل المرحلة H1.**
