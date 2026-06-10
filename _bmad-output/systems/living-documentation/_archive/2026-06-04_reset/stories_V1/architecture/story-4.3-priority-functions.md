---
status: done
slug: story-4.3-priority-functions
epic: "ADR-006b Epic 4 — مصفوفة الترحيل"
story_id: "4.3"
created: "2026-06-10"
author: "Amelia (bmad-agent-dev)"
priority: high
depends_on: ["4.1 — migration_matrix.yaml ✅ DONE", "4.2 — CI Guard ✅ DONE"]
estimated_size: "3–4 أيام"
utm: "UTM-6.11"
---

# Story 4.3 — ترحيل الأولوية العالية (50 دالة)

## البيان

بوصفي **مستخدم موقع لغة ص**،  
أريد **توثيق الدوال المدمجة الـ 50 الأكثر استخداماً مُولَّداً تلقائياً** (حقل `description_ar` مكتمل في YAML)،  
لكي **لا أقرأ توثيقاً متقادماً أو ناقصاً على الموقع الرسمي**.

---

## قائمة الـ 50 دالة ذات الأولوية (مرتبة بعدد الظهور في examples/ + tests/)

> **المنهجية:** حُدِّدت بعد مسح 59 ملف .ص في `examples/` و `tests/` — الترتيب تنازلي.

### الطبقة 1 — الأكثر استخداماً (ظهور > 500 مرة)

| # | الدالة | الـ namespace | الظهور | YAML |
|---|--------|---------------|--------|------|
| 1 | `اطبع` | Core | 2906 | core.yaml |
| 2 | `اطبع_سطر` | Core | 2590 | core.yaml |
| 3 | `نص` | Core | 751 | core.yaml |
| 4 | `رقم` | Core | 310 | core.yaml |
| 5 | `مجموع` | Core | 184 | core.yaml |
| 6 | `طول` | Core | 146 | core.yaml |

### الطبقة 2 — كثيفة الاستخدام (50–500 مرة)

| # | الدالة | الـ namespace | YAML |
|---|--------|---------------|------|
| 7 | `عشري` | Core | core.yaml |
| 8 | `منطقي` | Core | core.yaml |
| 9 | `نوع` | Core | core.yaml |
| 10 | `اقرأ` | Core | core.yaml |
| 11 | `مطلق` | Math | stdlib_math.yaml |
| 12 | `جذر` | Math | stdlib_math.yaml |
| 13 | `قوة` | Math | stdlib_math.yaml |
| 14 | `تقريب` | Math | stdlib_math.yaml |
| 15 | `بتر` | Math | stdlib_math.yaml |
| 16 | `سقف` | Math | stdlib_math.yaml |
| 17 | `أرضية` | Math | stdlib_math.yaml |
| 18 | `أقصى` | Math | stdlib_math.yaml |
| 19 | `أدنى` | Math | stdlib_math.yaml |
| 20 | `عشوائي` | Math | stdlib_math.yaml |

### الطبقة 3 — دوال نصوص

| # | الدالة | الـ namespace | YAML |
|---|--------|---------------|------|
| 21 | `تقسيم` | Strings | stdlib_strings.yaml |
| 22 | `استبدل` | Strings | stdlib_strings.yaml |
| 23 | `يحتوي` | Strings | stdlib_strings.yaml |
| 24 | `يبدأ_بـ` | Strings | stdlib_strings.yaml |
| 25 | `ينتهي_بـ` | Strings | stdlib_strings.yaml |
| 26 | `قص_أطراف` | Strings | stdlib_strings.yaml |
| 27 | `تحويل_كبير` | Strings | stdlib_strings.yaml |
| 28 | `تحويل_صغير` | Strings | stdlib_strings.yaml |
| 29 | `استخراج` | Strings | stdlib_strings.yaml |
| 30 | `نص_يحتوي` | Strings | stdlib_strings.yaml |
| 31 | `لوغاريتم` | Math | stdlib_math.yaml |
| 32 | `جيب` | Math | stdlib_math.yaml |
| 33 | `جيب_تمام` | Math | stdlib_math.yaml |

### الطبقة 4 — دوال المصفوفات والأساسيات

| # | الدالة | الـ namespace | YAML |
|---|--------|---------------|------|
| 34 | `اضف` | Arrays | stdlib_arrays.yaml |
| 35 | `احذف` | Arrays | stdlib_arrays.yaml |
| 36 | `رتب` | Arrays | stdlib_arrays.yaml |
| 37 | `عكس` | Arrays | stdlib_arrays.yaml |
| 38 | `شريحة` | Arrays | stdlib_arrays.yaml |
| 39 | `رشح` | Arrays | stdlib_arrays.yaml |
| 40 | `اختزل` | Arrays | stdlib_arrays.yaml |
| 41 | `خروج` | Basics | stdlib_basics.yaml |
| 42 | `مدى_نطاق` | Basics | stdlib_basics.yaml |
| 43 | `تأكد` | Assertions | stdlib_assertions.yaml |

### الطبقة 5 — دوال التزامن والشبكات

| # | الدالة | الـ namespace | YAML |
|---|--------|---------------|------|
| 44 | `قناة` | Core | core.yaml |
| 45 | `انتظر_الكل` | Core | core.yaml |
| 46 | `مجموعة_انتظار` | Core | core.yaml |
| 47 | `قفل` | Core | core.yaml |
| 48 | `مستقبل` | Core | core.yaml |
| 49 | `اقرأ_ملف` | Basics | stdlib_basics.yaml |
| 50 | `اكتب_ملف` | Basics | stdlib_basics.yaml |

---

## معايير القبول

### AC-4.3-01: تحديد القائمة النهائية

**Given** مصفوفة الترحيل (`data/docs/migration_matrix.yaml`) و قائمة الـ 50 أعلاه  
**When** يُشغَّل `python scripts/codegen/audit_priority_functions.py`  
**Then** يطبع قائمة الـ 50 دالة مع حالة `description_ar` لكل منها (موجود/ناقص)

### AC-4.3-02: إضافة `description_ar` للدوال الناقصة

**Given** دالة في القائمة بدون `description_ar` أو بوصف ناقص (< 10 أحرف)  
**When** تُضاف `description_ar` بوصف عربي دقيق (20-120 حرف)  
**Then** `doc_ir_validator.py` يمر على الـ entry بلا أخطاء

### AC-4.3-03: إضافة مثال واحد لكل دالة

**Given** دالة من القائمة بدون `examples` أو بأمثلة ناقصة  
**When** تُضاف مثال واحد على الأقل بـ `code` و `expected_output`  
**Then** المثال صالح نحوياً ويُنفَّذ بنجاح في `sad.exe`

### AC-4.3-04: تغطية 50/50 دالة

**Given** القائمة الكاملة بعد التحديث  
**When** يُشغَّل `audit_priority_functions.py --strict`  
**Then** يطبع `50/50 دوال مُوثَّقة ✅` — 0 ناقصة

### AC-4.3-05: pytest لا يتراجع

**Given** التعديلات على YAML files  
**When** يُشغَّل `pytest scripts/codegen/ -v`  
**Then** جميع الاختبارات تمر (≥ 187 passed)

### AC-4.3-06: أداة المراجعة (audit script)

**Given** يُشغَّل بـ `python scripts/codegen/audit_priority_functions.py`  
**When** التغطية < 100%  
**Then** يطبع:
```
ملخص التغطية:
  مُوثَّق    : 42/50
  ناقص      :  8/50
  ناقصة     : [قائمة أسماء الدوال الناقصة]
```

---

## المهام

### T1: إنشاء أداة audit (اليوم 1)

**الغرض:** تحديد الدوال الناقصة تلقائياً.

- [ ] T1.1: إنشاء `scripts/codegen/audit_priority_functions.py`
  - يقرأ YAML files من `data/language/builtins/`
  - يبحث عن كل دالة من قائمة الـ 50
  - يُبلِّغ عن حالة `description_ar` و`examples`
  - مخرج واضح بالعربية (مُوثَّق/ناقص)
  - `--strict` يُغيِّر exit code إلى 1 عند أي نقص

- [ ] T1.2: اختبار الأداة على الـ YAML الحالي

**ملاحظة:** لا تعتمد على gen_docs.py — أداة مستقلة للمراجعة.

---

### T2: إضافة description_ar للطبقات 1+2 (اليوم 1-2)

> **الترتيب:** ابدأ بالأعلى أولويةً (Core namespace).

**القاعدة:** كل `description_ar` يجب أن:
- يكون بالعربية الفصحى (ليس دارجة)
- يبدأ بفعل مضارع (يطبع/يُرجع/يُحسب...)
- يذكر نوع المدخل والمخرج
- 20-120 حرف

**مثال على description_ar صالح:**
```yaml
description_ar: "يطبع قيمة ويضيف سطراً جديداً. يقبل أي نوع ويحوّله نصاً تلقائياً."
```

**مثال خاطئ:**
```yaml
description_ar: "طباعة"  # < 20 حرف — مرفوض
```

**الملفات المُعدَّلة:**
- `data/language/builtins/core.yaml` — 10 دوال (الطبقة 1+2)
- `data/language/builtins/stdlib_math.yaml` — 10 دوال (الطبقة 2)

---

### T3: إضافة description_ar للطبقات 3+4+5 (اليوم 2-3)

**الملفات المُعدَّلة:**
- `data/language/builtins/stdlib_strings.yaml` — 11 دالة (الطبقة 3)
- `data/language/builtins/stdlib_arrays.yaml` — 7 دوال (الطبقة 4)
- `data/language/builtins/stdlib_basics.yaml` — 3 دوال (الطبقة 4)
- `data/language/builtins/stdlib_assertions.yaml` — 1 دالة (الطبقة 4)

---

### T4: إضافة أمثلة runnable (اليوم 3)

لكل دالة من الـ 50 تفتقر لمثال:
```yaml
examples:
  - code: |
      اطبع(جذر(16))
    expected_output: "4"
    runnable: true
    doc_type: reference
```

**قاعدة:** مثال واحد كافٍ — لكن يجب أن يكون `runnable: true`.

---

### T5: audit --strict + pytest (اليوم 4)

- [ ] T5.1: تشغيل `audit_priority_functions.py --strict` — يجب `50/50 ✅`
- [ ] T5.2: تشغيل `pytest scripts/codegen/ -v` — يجب ≥ 187 passed
- [ ] T5.3: اختياري — تشغيل `render_lsp.py` للتحقق أن hover docs تعرض description_ar الجديد

---

## ملاحظات تقنية

1. **YAML files الحالية**: بعض الدوال قد تحتوي `description_ar` جزئياً — تحقق قبل الإضافة
2. **الترجمة**: المصطلحات التقنية العربية الموحَّدة في `.github/copilot-instructions.md` مصدر الحقيقة
3. **صيغة YAML**: الوصف الطويل يستخدم `|` (block scalar) لتفادي مشكلات الأحرف العربية
4. **نظام الأنواع**: الدوال التي تقبل `أي` نوع تُوثَّق بـ "يقبل أي نوع"

## قائمة الملفات المتوقع إنشاؤها/تعديلها

| الملف | الإجراء |
|-------|--------|
| `scripts/codegen/audit_priority_functions.py` | إنشاء جديد |
| `data/language/builtins/core.yaml` | تعديل (description_ar + examples) |
| `data/language/builtins/stdlib_math.yaml` | تعديل |
| `data/language/builtins/stdlib_strings.yaml` | تعديل |
| `data/language/builtins/stdlib_arrays.yaml` | تعديل |
| `data/language/builtins/stdlib_basics.yaml` | تعديل |
| `data/language/builtins/stdlib_assertions.yaml` | تعديل |

## سجل المطور (يُملأ أثناء التنفيذ)

| التاريخ | المهمة | الحالة | ملاحظات |
|---------|--------|--------|---------|
| — | T1 | ⬜ | — |
| — | T2 | ⬜ | — |
| — | T3 | ⬜ | — |
| — | T4 | ⬜ | — |
| — | T5 | ⬜ | — |
