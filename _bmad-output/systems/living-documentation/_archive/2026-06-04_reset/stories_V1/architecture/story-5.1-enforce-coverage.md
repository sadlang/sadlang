---
status: done
slug: story-5.1-enforce-coverage
epic: "ADR-006b Epic 5 — enforce_docs_coverage + pipeline AI"
story_id: "5.1"
created: "2026-06-10"
author: "Amelia (bmad-agent-dev)"
priority: medium
depends_on:
  - "4.3 — توثيق 50 دالة أولوية ✅ (مطلوب قبل --enforce)"
  - "تغطية ≥ 90% قبل تفعيل CI enforcement"
estimated_size: "2–3 أيام"
utm: "UTM-6.12"
---

# Story 5.1 — enforce_docs_coverage (حارس تغطية التوثيق)

## البيان

بوصفي **مطور CI**،  
أريد **حارس يتحقق أن كل دالة YAML عامة لها `description_ar` مكتملة**،  
لكي **لا يُنشر توثيق ناقص على الموقع الرسمي**.

> **تحذير د. كوين:** هذا الحارس **لا يُفعَّل بوضع `--enforce`** حتى تبلغ التغطية ≥ 90%.
> قبل ذلك: وضع `--report` فقط (warning بدون إيقاف CI).

---

## تعريف "تغطية كاملة"

- **دالة عامة**: كل entry في YAML files تحت:
  - `data/language/builtins/*.yaml`
  - `data/language/type_methods/*.yaml`
  - `data/language/modules/*.yaml`
- **يُستثنى**: entries بـ `stability: internal` (إذا أُضيف لاحقاً)
- **اكتمال description_ar**: موجود + غير فارغ + ≥ 10 أحرف

---

## معايير القبول

### AC-5.1-01: وضع التقرير (--report)

**Given** 10 ملفات YAML في `data/language/builtins/`  
**When** يُشغَّل `python scripts/codegen/check_docs_coverage.py --report`  
**Then** يُبلِّغ عن:
```
تغطية description_ar:
  إجمالي الدوال : 552
  مُوثَّق       : 384
  ناقص         : 168
  النسبة        : 69.6%
  
أكثر namespaces نقصاً:
  Maps        :  73 / 83  ناقص
  Strings     :  11 / 21  ناقص
```
**And** exit code = 0 (لا يكسر CI في وضع --report)

### AC-5.1-02: وضع الفحص (--check)

**Given** التغطية < 90%  
**When** يُشغَّل `check_docs_coverage.py --check --threshold 90`  
**Then** يطبع تحذيراً ويُغيِّر exit code = 0 (لا يكسر CI بعد)

**Given** التغطية ≥ 90%  
**When** يُشغَّل بـ `--check --threshold 90`  
**Then** يطبع `✅ التغطية (92.3%) تتجاوز الحد الأدنى` + exit code = 0

### AC-5.1-03: وضع الإلزام (--enforce)

**Given** entry بـ `description_ar` فارغة أو ناقصة  
**When** يُشغَّل `check_docs_coverage.py --enforce`  
**Then** يطبع اسم الـ namespace + اسم الدالة + رقم السطر في YAML  
**And** exit code = 1

**Given** جميع entries لها `description_ar` مكتملة  
**When** يُشغَّل بـ `--enforce`  
**Then** يطبع `✅ تغطية كاملة (100%)` + exit code = 0

### AC-5.1-04: فلترة namespace

**Given** يُشغَّل بـ `--namespace Core`  
**When** التحقق  
**Then** يفحص فقط entries في Core namespace

### AC-5.1-05: تكامل pytest

**Given** اختبارات `test_docs_coverage.py`  
**When** تُشغَّل `pytest scripts/codegen/test_docs_coverage.py -v`  
**Then** ≥ 15 passed (اختبارات منطق check_docs_coverage.py)

### AC-5.1-06: لا تراجع pytest

**Given** إضافة check_docs_coverage.py + test_docs_coverage.py  
**When** يُشغَّل `pytest scripts/codegen/ -v`  
**Then** ≥ 202 passed (187 الحالية + 15 جديدة)

---

## المهام

### T1: إنشاء check_docs_coverage.py (اليوم 1)

**الملف:** `scripts/codegen/check_docs_coverage.py`

```
check_docs_coverage.py
├── collect_all_entries(yaml_dirs) → list[EntryInfo]
├── check_description_ar(entry) → bool
├── generate_report(entries) → CoverageReport
├── print_report(report, verbose=False)
└── main(argv) → int  # exit code
```

**واجهة سطر الأوامر:**
```
python check_docs_coverage.py [--report | --check | --enforce]
                               [--threshold 90]
                               [--namespace Core|Math|Strings|...]
                               [--yaml-dir path]
                               [--verbose]
```

**EntryInfo dataclass:**
```python
@dataclass
class EntryInfo:
    yaml_file: Path
    namespace: str
    canonical_name: str
    description_ar: str | None
    line_number: int
    is_covered: bool  # description_ar موجود + ≥ 10 أحرف
```

**CoverageReport dataclass:**
```python
@dataclass
class CoverageReport:
    total: int
    covered: int
    missing: list[EntryInfo]
    by_namespace: dict[str, tuple[int, int]]  # {ns: (covered, total)}
    percentage: float
```

**قواعد:**
- يستخدم `_lib/loader.py` لتحميل YAML (لا يكرر منطق التحميل)
- يدعم يُشغَّل كـ script مستقل + كـ module
- لا يعتمد على doc_ir_builder — يقرأ YAML مباشرةً

---

### T2: إنشاء test_docs_coverage.py (اليوم 1-2)

**الملف:** `scripts/codegen/test_docs_coverage.py`

**الاختبارات (15 اختبار — DC-01→DC-15):**

| الاختبار | الوصف |
|---------|-------|
| DC-01 | `collect_all_entries` يجد جميع entries في core.yaml |
| DC-02 | `check_description_ar` يرفض description_ar فارغة |
| DC-03 | `check_description_ar` يرفض < 10 أحرف |
| DC-04 | `check_description_ar` يقبل وصف ≥ 10 أحرف |
| DC-05 | `generate_report` يحسب النسبة بدقة |
| DC-06 | `--report` يُنتج exit code 0 دائماً |
| DC-07 | `--check --threshold 90` يُنتج exit code 0 عند تغطية ≥ 90% |
| DC-08 | `--check --threshold 90` يُنتج exit code 0 عند تغطية < 90% (لا يكسر) |
| DC-09 | `--enforce` يُنتج exit code 1 عند وجود entries ناقصة |
| DC-10 | `--enforce` يُنتج exit code 0 عند تغطية 100% |
| DC-11 | `--namespace Core` يُصفِّي النتائج بالـ namespace الصحيح |
| DC-12 | التقرير يحتوي اسم YAML file + رقم السطر للدالة الناقصة |
| DC-13 | لا exception عند YAML file فارغ |
| DC-14 | `by_namespace` يجمع النتائج بشكل صحيح |
| DC-15 | لا تراجع في pytest الكلي |

---

### T3: تكامل مع gen_all.py (اليوم 2)

**الملف:** `scripts/codegen/gen_all.py`

إضافة check_docs_coverage كـ "step" بعد جميع generators:
```python
# في gen_all.py — خطوة 6 (بعد gen_docs)
{
    "name": "docs_coverage",
    "script": "check_docs_coverage.py",
    "args": ["--report", "--yaml-dir", "data/language"],
    "condition": "always",  # يعمل دائماً، لكن لا يوقف البناء
}
```

**القاعدة:** في وضع `--report` فقط حتى تبلغ تغطية 90%+ ← ثم يُغيَّر لـ `--check`.

---

### T4: اختياري — إضافة تحليل منهجيات الوصف (اليوم 3)

إضافة `--suggest` للأداة:
- لكل دالة ناقصة، تقترح `description_ar` مبني على اسم الدالة + نوع المعاملات
- مثال: دالة `جذر(رقم)` → `"يحسب الجذر التربيعي للرقم المُعطى."`

---

## الملفات المتوقع إنشاؤها/تعديلها

| الملف | الإجراء |
|-------|--------|
| `scripts/codegen/check_docs_coverage.py` | إنشاء جديد |
| `scripts/codegen/test_docs_coverage.py` | إنشاء جديد |
| `scripts/codegen/gen_all.py` | تعديل — إضافة خطوة docs_coverage |

---

## ملاحظات تقنية

1. **المسار الصحيح لـ YAML**: `data/language/builtins/` + `data/language/type_methods/` + `data/language/modules/`
2. **تعريف "ناقص"**: `description_ar` غائب **أو** فارغ **أو** < 10 أحرف
3. **لا hard-coded threshold**: الـ threshold يُمرَّر كـ argument (default: 90)
4. **تحذير يبدأ الحد 90%**: ما دون 90% = تحذير فقط في CI بغض النظر عن الـ flag
5. **الحالة الراهنة** (تقريباً): ~69% تغطية → `--check --threshold 90` سيطبع تحذير + exit 0

## سجل المطور

| التاريخ | المهمة | الحالة | ملاحظات |
|---------|--------|--------|---------|
| — | T1 | ⬜ | — |
| — | T2 | ⬜ | — |
| — | T3 | ⬜ | — |
| — | T4 | ⬜ | اختياري |
