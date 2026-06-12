# اختبارات لغة ص — النظام الموحّد

> نظام اختبارات من **ثلاث طبقات**. التصميم الكامل في
> [`_bmad-output/systems/testing-system/`](../_bmad-output/systems/testing-system/).
> الحالة: قيد الترحيل (TEST-002 منجَزة — الهيكل والمشغّل؛ المحتوى يُرحَّل في TEST-003+).

## الطبقات الثلاث

| الطبقة | المجلد | ماذا تتحقق | المشغّل |
|---|---|---|---|
| 1 — وحدة | [`unit/`](unit/) | مكوّن C++ معزول (Lexer/Parser/AST/SIR/Value) | CTest |
| 2 — سلوك | [`behavior/`](behavior/) | `.ص` عبر المفسر + المترجم (تكافؤ) | `runner.py` |
| 3 — نظام | [`system/`](system/) | أدوات (LSP/pkg/sadinfo) + أداء + توثيق | CTest / scripts |

> **مبدأ (ADR-004):** «فضّل المستويات الأدنى» — منطق قابل للعزل يُختبَر في `unit/`؛
> التنفيذ المزدوج البطيء في `behavior/` محجوز للدلالات التشغيلية.

## المشغّل الموحّد

`runner.py` و`config.yaml` يعيشان في جذر `tests/`. موقع المشغّل **مفصول** عن موقع
المحتوى عبر `config["paths"]["tests_dir"]` (يسمح بالترحيل التدريجي).

```
python tests/runner.py --level P0          # دخان (5 ملفات، كل commit)
python tests/runner.py --level P0.<قسم>    # دخان + قسم أثناء التطوير
python tests/runner.py --section <قسم>     # قسم واحد مباشرة
python tests/runner.py --level P1          # كل PR
python tests/runner.py --level P2          # Nightly
python tests/runner.py --level full        # كل شيء
python tests/runner.py --cpu moderate      # تحكم بقوة CPU
```

## المستويات الهرمية

`P0 ⊂ P1 ⊂ P2 ⊂ P3 ⊂ full`. الأولوية تُشتقّ من **درجة المخاطر** لا حجم القسم (ADR-004).

| المستوى | المحتوى | متى |
|---|---|---|
| P0 | 5 ملفات دخان | كل commit |
| P1 | أساسيات + أنواع | كل PR |
| P2 | + OOP + أنماط + أخطاء | Nightly |
| P3 | كل الميزات الـ12 | قبل الإصدار |
| full | كل شيء + unit + system | بوّابة الإصدار |

## بوّابة CI (TEST-006)

CI يربط الحدث بالمستوى عبر المشغّل الموحّد + بوّابة قرار (`.github/workflows/ci.yml`):

| حدث CI | المستوى | البوّابة |
|---|---|---|
| `push` | P0 | `--gate` (أي فشل دخان = FAIL) |
| `pull_request` | P1 | `--gate --gate-floor 86.0` (تراجع تحت 86% = FAIL) |
| `schedule` (ليلي 03:00 UTC) | P2 | `--gate --gate-floor 86.0` |

- **بوّابة القرار** `--gate`: تُصدر `PASS` (صفر فشل) / `CONCERNS` (فشل ضمن الأرضية، لا يكسر البناء) / `FAIL` (فشل دخان P0 أو تراجع تحت الأرضية أو رفرفة).
- **burn-in في PR**: كل ملف `.ص` جديد/معدَّل يُشغَّل `--repeat 5` لكشف الرفرفة قبل الدمج.

```bash
python tests/runner.py --level P1 --gate --gate-floor 86.0   # كما في PR
python tests/runner.py --section تزامن --classify 10          # تمييز الرفرفة من خطأ المترجم
python tests/runner.py --file X.ص --repeat 5                  # burn-in يدوي
```

## كيف تضيف اختباراً سلوكياً؟

1. انسخ [`behavior/sections/_TEMPLATE/`](behavior/sections/_TEMPLATE/) للقسم المناسب.
2. أضف `NNN_اسم.ص` (إيجابي) و/أو `_negative/NNN_اسم.ص` (سلبي).
3. حدّث `COVERAGE.md` (الميزة ↔ الاختبار) و`RISK.md` للقسم.
4. شغّل `runner.py --section <القسم>` وتأكد من تكافؤ المفسر/المترجم.

### وسوم الحتمية (ADR-004)

- `@unordered`: فرز الخرج قبل المقارنة (التزامن).
- `@nondeterministic`: اختبار خصائص — **يُضاف فقط بعد إثبات الرفرفة بالمفسر وحده**
  (لا تُخفِ به خطأ مترجم حقيقياً — راجع `behavior/README.md`).

## أساس مرجعي (baseline 2026-06-11)

full = **83.2%** (696/837) · P0 = 100%. تفاصيل في
[`status/INVENTORY_2026-06-11.md`](../_bmad-output/systems/testing-system/status/INVENTORY_2026-06-11.md).
