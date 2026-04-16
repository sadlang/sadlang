# 🧪 دليل اختبارات التنفيذ المزدوج — للمساهمين

## نظرة سريعة

اختبارات **التنفيذ المزدوج** (dual execution) تُشغّل كل ملف `.ص` عبر المفسر (`sad.exe`) والمترجم (`sadc.exe`) وتقارن المخرجات.
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
