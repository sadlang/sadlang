# 🧪 دليل اختبارات التنفيذ المزدوج — للمساهمين
<!-- آخر تحديث: إضافة --section / --cpu / --workers / P0.x + توثيق المشاكل المعروفة -->

## نظرة سريعة

اختبارات **التنفيذ المزدوج** (dual execution) تُشغّل كل ملف `.ص` عبر المفسر (`sad-run.exe`) والمترجم (`sadc.exe`) وتقارن المخرجات.
الاختبار ينجح فقط إذا أنتج كلاهما نفس المخرج بالضبط — أو إذا تطابق مخرج المفسر مع `@expected` في حالة `@skip_compiler`.

---

## المتطلبات قبل التشغيل

| المتطلب | الأمر |
|---------|-------|
| بناء المفسر (Debug) | `cmake --build build --config Debug --target sad` |
| بناء المترجم (Release) | `cmake --build build --config Release --target sadc` |
| Python 3.8+ | مثبت في PATH |
| PyYAML | `pip install pyyaml` (لقراءة config.yaml) |

> ⚠️ **ملاحظة مهمة:** المفسر يُبنى في نمط `Debug` بينما المترجم يُبنى في `Release` لأن LLVM مربوط بنمط Release.

---

## أوامر المساهمة اليومية

```powershell
# ① بناء
cmake --build build --config Debug --target sad
cmake --build build --config Release --target sadc

# ② تشغيل سريع (P0 — لكل commit — 5 اختبارات)
python tests/dual_execution/runner.py --level P0

# ③ تشغيل قسم ميزة واحدة (أثناء التطوير)
python tests/dual_execution/runner.py --section متغيرات
python tests/dual_execution/runner.py --section أنماط
python tests/dual_execution/runner.py --section كائني

# ④ P0 + قسم محدد (دخان + ميزة = تحقق سريع)
python tests/dual_execution/runner.py --level P0.متغيرات
python tests/dual_execution/runner.py --level P0.أنواع
python tests/dual_execution/runner.py --level P0.أنماط

# ⑤ تحكم في قوة CPU
python tests/dual_execution/runner.py --level P1 --cpu sequential   # تسلسلي للتشخيص
python tests/dual_execution/runner.py --level P2 --cpu moderate     # افتراضي (4 أنوية)
python tests/dual_execution/runner.py --level P3 --cpu full         # 8 أنوية للسيرفرات
python tests/dual_execution/runner.py --level P3 --cpu max          # أقصى أنوية متاحة
python tests/dual_execution/runner.py --level P2 --workers 6        # تحديد مباشر

# ⑥ تشغيل كامل مع تقرير
python tests/dual_execution/runner.py --level P4 --report --html

# ⑦ Burn-in (اكتشاف flakiness)
python tests/dual_execution/runner.py --level P0 --repeat 20
```

---

## هرم مستويات الاختبار

```
full (807+ اختبار)         ← كل شيء بدون استثناء
 └── P4                    ← بنية جديدة + أرشيف قديم
      └── P3               ← كل الميزات المتقدمة
           └── P2          ← أساسيات + OOP + أنماط + أخطاء (Nightly)
                └── P1     ← أساسيات اللغة + الأنواع (كل PR)
                     └── P0.x  ← دخان + قسم واحد (أثناء التطوير)
                          └── P0  ← 5 دخان فقط (كل commit)
```

### جدول المستويات

| المستوى | الاختبارات | الوقت (4 أنوية) | متى تُشغّل |
|---------|-----------|----------------|-----------|
| **P0** | 5 | ~5 ثوانٍ | كل commit |
| **P0.متغيرات** | ~46 | ~45 ثانية | عند تعديل نظام المتغيرات |
| **P0.أنماط** | ~30 | ~30 ثانية | عند تعديل match |
| **P0.كائني** | ~50 | ~50 ثانية | عند تعديل OOP |
| **P1** | ~100 | ~60 ثانية | كل PR |
| **P2** | ~250 | ~120 ثانية | Nightly |
| **P3** | ~500 | ~200 ثانية | قبل الإصدار |
| **P4** | ~800 | ~250 ثانية | تكافؤ كامل |

### الطبقات P0.x المتاحة

```powershell
--level P0.متغيرات    # دخان + متغيرات/ثوابت/ساكن
--level P0.تحكم       # دخان + if/while/for/match
--level P0.دوال       # دخان + الدوال الأساسية
--level P0.أساسيات    # دخان + كل أساسيات اللغة
--level P0.أنواع      # دخان + الأنواع المدمجة
--level P0.كائني      # دخان + OOP
--level P0.أنماط      # دخان + مطابقة الأنماط
--level P0.أخطاء      # دخان + معالجة الأخطاء
--level P0.لامدا      # دخان + lambda/closures
--level P0.تزامن      # دخان + goroutines/channels
--level P0.متقدم      # دخان + macros/defer/generators
--level P0.مكتبة      # دخان + المكتبة القياسية
```

---

## الأقسام المختصرة (--section)

أسرع طريقة لتشغيل اختبارات ميزة واحدة:

```powershell
python runner.py --section متغيرات   # features/01_.../02_المتغيرات
python runner.py --section تحكم      # features/01_.../03_التحكم_في_التدفق
python runner.py --section دوال      # features/01_.../04_الدوال
python runner.py --section أساسيات   # features/01_أساسيات_اللغة
python runner.py --section أنواع     # features/02_الأنواع_المدمجة
python runner.py --section كائني     # features/03_البرمجة_الكائنية
python runner.py --section أنماط     # features/04_مطابقة_الأنماط
python runner.py --section أخطاء     # features/05_معالجة_الأخطاء
python runner.py --section لامدا     # features/06_الدوال_المتقدمة
python runner.py --section تزامن     # features/07_التزامن
python runner.py --section متقدم     # features/08_ميزات_متقدمة
python runner.py --section مكتبة     # features/09_المكتبة_القياسية
python runner.py --section استيراد   # features/10_الاستيراد
```

> إذا أدخلت اسم قسم غير موجود، يعرض البرنامج القائمة الكاملة تلقائياً.

---

## التحكم في قوة CPU

```powershell
# مُسبقات جاهزة
--cpu sequential   # 1 خيط  — للتشخيص الدقيق وعزل الأخطاء
--cpu light        # 2 خيوط — للأجهزة ذات الموارد المحدودة
--cpu moderate     # 4 خيوط — الافتراضي لمحطات العمل
--cpu full         # 8 خيوط — خوادم CI/CD
--cpu max          # كل أنوية CPU المتاحة (os.cpu_count())

# تحديد مباشر
--workers 6        # 6 خيوط بالضبط (يتجاوز --cpu)

# ترتيب الأولوية: --workers > --cpu > --parallel > config.yaml
```

> عند تشخيص خطأ متقطع (flaky test)، استخدم `--cpu sequential` لعزل المشكلة.

---

## كتابة اختبار جديد

### 1. البنية الهرمية للملفات

```
tests/dual_execution/
├── P0_smoke/                          ← 5 اختبارات حرجة فقط
├── features/
│   ├── 01_أساسيات_اللغة/
│   │   ├── 01_مدخل/
│   │   ├── 02_المتغيرات/              ← متغير/ثابت/ساكن (200-239)
│   │   ├── 03_التحكم_في_التدفق/
│   │   └── 04_الدوال/
│   ├── 02_الأنواع_المدمجة/
│   ├── 03_البرمجة_الكائنية/
│   ├── 04_مطابقة_الأنماط/
│   ├── 05_معالجة_الأخطاء/
│   ├── 06_الدوال_المتقدمة/
│   ├── 07_التزامن/
│   └── ...
└── [مجلدات قديمة: core/, oop/, match/, ...]
```

### 2. البيانات الوصفية (أول 30 سطر)

```sad
# @description وصف مختصر للاختبار
# @priority P1
# @expected مخرج_سطر_أول\nمخرج_سطر_ثاني
# @timeout 30
# @skip_compiler
# @skip_interpreter
# @requires feature_name
# @expect_error نص_الخطأ_المتوقع
# @stdin_data سطر1\nسطر2
```

| البيان | الوصف |
|--------|-------|
| `@description` | وصف الاختبار (اختياري) |
| `@priority` | `P0` / `P1` / `P2` / `P3` / `P4` |
| `@expected` | المخرج المتوقع. `\n` = سطر جديد |
| `@timeout` | ثوانٍ. الافتراضي: 30 |
| `@skip_compiler` | تخطي المترجم — مقارنة مع `@expected` فقط |
| `@skip_interpreter` | تخطي المفسر |
| `@requires` | ميزة مطلوبة (اختياري) |
| `@expect_error` | اختبار سلبي — يتوقع رسالة خطأ |
| `@stdin_data` | بيانات stdin للاختبارات التفاعلية |

> كلا الصيغتين مقبولتان: `@key: value` و `@key value`

### 3. متى تستخدم `@skip_compiler`

استخدمها عندما:
- الميزة غير مدعومة بعد في `sadc`
- الاختبار يعتمد على دوال مدمجة غير متاحة في المترجم
- تريد اختبار سلوك المفسر فقط مؤقتاً

---

## قراءة الفشل بسرعة

| الحالة | المعنى | ماذا تفعل |
|--------|--------|----------|
| `PASS` | المخرجات متطابقة | ✅ لا شيء |
| `FAIL_OUTPUT` | مخرجات مختلفة | قارن مخرج المفسر والمترجم (`--verbose`) |
| `FAIL_INTERP` | المفسر أعطى خطأ | راجع الكود أو المفسر |
| `FAIL_COMPILE` | المترجم فشل في الترجمة | راجع `sadc` أو أضف `@skip_compiler` |
| `FAIL_RUNTIME` | الملف المترجم تعطل | خطأ في codegen |
| `FAIL_TIMEOUT` | تجاوز المهلة | زد `@timeout` أو راجع حلقات لا نهائية |
| `SKIP` | تم التخطي | `@skip_compiler` + بدون `@expected` |

---

## مؤشرات التقرير

التقارير (JSON/HTML) تعرض مؤشرين منفصلين:

| المؤشر | المعنى |
|--------|--------|
| **تكافؤ مزدوج** (dual_parity_passed) | نجح عبر المفسر **والمترجم** — تكافؤ حقيقي |
| **مفسر فقط** (interp_only_passed) | نجح عبر المفسر فقط vs `@expected` |

---

## المشاكل المعروفة (Known Issues)

هذه مشاكل اكتُشفت أثناء كتابة الاختبارات وتُخطِّيتها بحل مؤقت — السبب الجذري لم يُعالَج بعد:

### 1. ~~كلمة `حالة` تتعارض مع parser~~ ✅ تم الإصلاح
- **السبب:** `حالة(99).method()` في بداية الجملة كان يُحلَّل كـ switch statement
- **الإصلاح:** إذا تلا `حالة` قوسٌ `(` أو نقطة `.` مباشرةً → يُعامَل كـ expression وليس switch
- **الملفات المُعدَّلة:** `shared/parser/src/core/parser_main.cpp` (موقعان)
- **حالة الاختبار:** P4 = 807/807 = 100% بدون تراجع

### 2. طباعة float بدون كسر عشري
- **التأثير:** `4.0 * 2.5 = 10.0` يُطبع `10` (بدون `.0`)
- **الحل المؤقت:** `@expected` يحتوي `10` لا `10.0`
- **السبب الجذري:** دالة الطباعة لا تُميّز العشري من الصحيح عند الطباعة
- **سؤال التصميم:** هل يجب طباعة `10.0` للأعداد العشرية ذات الكسر الصفري؟

### 3. Strength reduction في المُحسِّن (CSE)
- **التأثير:** `متغير شدّة = 5; متغير ن = شدّة * 3` — المحسِّن يحذف حساب `5 * 3`
- **الحل المؤقت:** تغيير اسم المتغير أو استخدام تعبير أكثر تعقيداً
- **السبب الجذري:** CSEPass يُبطل الجدول عند STORE (تم إصلاحه جزئياً) لكن قد تكون هناك حالات مماثلة
- **الملف المعني:** `compiler/src/middle/cse_pass.cpp`

### 4. type mismatch في STORE للحقل الساكن المنطقي
- **التأثير:** `store i64 N, ptr @Class.boolField` — عند حقل ساكن نوعه `i1` لكن القيمة `i64`
- **الحل المؤقت:** إصلاح `sir_builder_expr_members.cpp` عالج الحالة الشائعة
- **السبب الجذري:** `emitStore` لـ GLOBAL قد لا يُعالج جميع حالات truncation
- **الملف المعني:** `compiler/src/backend/llvm/llvm_codegen_store.cpp`

---

## Burn-in (اختبار الاستقرار)

```powershell
# تشغيل 20 جولة لاكتشاف flakiness
python runner.py --level P0 --repeat 20

# مع CPU sequential لعزل الأخطاء المتقطعة
python runner.py --level P0 --repeat 10 --cpu sequential
```

---

## Performance Baselines

```powershell
# حفظ الأزمنة الحالية كمرجع
python runner.py --level P1 --save-baselines

# مقارنة الأداء مع المرجع (تحذير عند تجاوز 2×)
python runner.py --level P1 --check-baselines

# مقارنة بعتبة مخصصة (1.5×)
python runner.py --level P1 --check-baselines --baselines-threshold 1.5
```

---

*تم التحديث تلقائياً — اختبارات التنفيذ المزدوج لـ لغة ص*

## نظرة سريعة

اختبارات **التنفيذ المزدوج** (dual execution) تُشغّل كل ملف `.ص` عبر المفسر (`sad-run.exe`) والمترجم (`sadc.exe`) وتقارن المخرجات.
الاختبار ينجح فقط إذا أنتج كلاهما نفس المخرج بالضبط — أو إذا تطابق مخرج المفسر مع `@expected` في حالة `@skip_compiler`.

---

## المتطلبات قبل التشغيل

| المتطلب | الأمر |
|---------|-------|
| بناء المفسر (Debug) | `cmake --build build --config Debug --target sad` |
| بناء المترجم (Release) | `cmake --build build --config Release --target sadc` |
| Python 3.8+ | مثبت في PATH |

> ⚠️ **ملاحظة مهمة:** المفسر يُبنى في نمط `Debug` بينما المترجم يُبنى في `Release` لأن LLVM مربوط بنمط Release.

---

## أوامر المساهمة اليومية

```powershell
# ① بناء
cmake --build build --config Debug --target sad
cmake --build build --config Release --target sadc

# ② تشغيل سريع (P0 — لكل commit)
cd tests/dual_execution
python runner.py --level P0 --no-color

# ③ تشغيل كامل (P2 — مع تقرير)
python runner.py --level P2 --report --html --no-color

# ④ اختبارات حرجة فقط (5 ملفات)
python runner.py --file 027_concurrency_stress_suite.ص --no-color
python runner.py --file 028_concurrency_negative_suite.ص --no-color
python runner.py --file 035_exception_flow_control_suite.ص --no-color
python runner.py --file 036_exception_concurrency_cross.ص --no-color
python runner.py --file 037_compound_high_density.ص --no-color

# ⑤ Burn-in (اكتشاف flakiness)
python runner.py --file 027_concurrency_stress_suite.ص --repeat 50 --no-color
```

---

## مستويات الاختبار

| المستوى | المجلدات | الاستخدام | الوقت |
|---------|----------|----------|-------|
| **P0** | `core/` | كل commit محلي | ~10 ثوانٍ |
| **P1** | `core/`, `oop/` | قبل كل PR | ~30 ثانية |
| **P2** | `core/`, `oop/`, `concurrency/`, `advanced/` | Nightly / مراجعة | ~60 ثانية |
| **P3** | جميع المجلدات | إصدار / شامل | ~120 ثانية |
| **critical** | 5 ملفات (027, 028, 035, 036, 037) | بوابة أمان | ~30 ثانية |

---

## كتابة اختبار جديد

### 1. أين يوضع الملف

| النوع | المجلد |
|-------|--------|
| أساسي (متغيرات، حلقات، شروط) | `core/` |
| OOP (أصناف، وراثة، سمات) | `oop/` |
| تزامن (قنوات، goroutines) | `concurrency/` |
| متقدم (defer، macros، contracts) | `advanced/` |

### 2. البيانات الوصفية (أول 30 سطر)

```sad
# @description وصف مختصر للاختبار
# @priority P1
# @expected مخرج_سطر_أول\nمخرج_سطر_ثاني
# @timeout 30
# @skip_compiler
# @skip_interpreter
# @requires feature_name
```

| البيان | الوصف |
|--------|-------|
| `@description` | وصف الاختبار (اختياري) |
| `@priority` | `P0` / `P1` / `P2` / `P3` |
| `@expected` | المخرج المتوقع. `\n` = سطر جديد |
| `@timeout` | ثوانٍ. الافتراضي: 30 |
| `@skip_compiler` | تخطي المترجم — مقارنة مع `@expected` فقط |
| `@skip_interpreter` | تخطي المفسر |
| `@requires` | ميزة مطلوبة (اختياري) |

> كلا الصيغتين مقبولتان: `@key: value` و `@key value`

### 3. متى تستخدم `@skip_compiler`

استخدمها عندما:
- الميزة غير مدعومة بعد في `sadc` (مثل: goroutines، defer)
- الاختبار يعتمد على دوال مدمجة غير متاحة في المترجم
- تريد اختبار سلوك المفسر فقط مؤقتاً

---

## قراءة الفشل بسرعة

| الحالة | المعنى | ماذا تفعل |
|--------|--------|----------|
| `PASS` | المخرجات متطابقة | ✅ لا شيء |
| `FAIL_OUTPUT` | مخرجات مختلفة | قارن مخرج المفسر والمترجم |
| `FAIL_INTERP` | المفسر أعطى خطأ | راجع الكود أو المفسر |
| `FAIL_COMPILE` | المترجم فشل في الترجمة | راجع `sadc` أو أضف `@skip_compiler` |
| `FAIL_RUNTIME` | الملف المترجم تعطل | خطأ في codegen |
| `FAIL_TIMEOUT` | تجاوز المهلة | زد `@timeout` أو راجع حلقات لا نهائية |
| `SKIP` | تم التخطي | `@skip_compiler` + بدون `@expected` |

---

## مؤشرات التقرير

التقارير (JSON/HTML) تعرض مؤشرين منفصلين:

| المؤشر | المعنى |
|--------|--------|
| **تكافؤ مزدوج** (dual_parity_passed) | نجح عبر المفسر **والمترجم** — تكافؤ حقيقي |
| **مفسر فقط** (interp_only_passed) | نجح عبر المفسر فقط vs `@expected` |

> هذا التفصيل مهم لأن `PASS` مجمّع لا يكشف مدى تغطية التكافؤ الفعلي.

---

## ملاحظات Windows و UTF-8

- الـ runner يفرض UTF-8 داخلياً (عبر `configure_utf8_console()`)
- لا حاجة لتغيير إعدادات الطرفية يدوياً
- إذا ظهرت أحرف مشوّهة: تأكد أن Python 3.8+ مثبت وأن الطرفية تدعم UTF-8
- CMake يفرض `PYTHONIOENCODING=utf-8` و `PYTHONUTF8=1` عند تشغيل runner

---

## مخرجات التقارير

| الملف | المسار | يُنشأ عند |
|-------|--------|----------|
| JSON | `build/_dual_report.json` | `--report` |
| HTML | `build/_dual_report.html` | `--html` |

---

## Burn-in (اختبار الاستقرار)

```powershell
# تشغيل 50 جولة متتالية لاكتشاف flakiness
python runner.py --file 027_concurrency_stress_suite.ص --repeat 50 --no-color

# عبر CMake target
cmake --build build --config Debug --target dual_tests_burnin
```

البرنامج يطبع ملخص كل جولة ويبلّغ عن أي فشل متقطع (flaky test).

---

*تم الإنشاء تلقائياً — اختبارات التنفيذ المزدوج لـ لغة ص*
