# Story 1.1 — إنشاء SPEC (code_standards.yaml)

**Epic:** 1 — نظام Contract-as-Code
**Story:** 1.1
**الحالة:** backlog
**الأولوية:** P0 (حرج)
**المرجع:** `prd-contract-as-code.md` § FR-01

---

## وصف القصة

**كمطوِّر في لغة ص،**
أريد أن يكون لديّ ملف YAML واحد يحتوي جميع قواعد معايير الكود،
**حتى** أتمكن من إضافة قاعدة جديدة بسطر YAML واحد دون لمس Python أو CMake.

---

## معايير القبول (AC)

### AC-1.1.1: بنية الملف
- [ ] الملف موجود في `scripts/lint/code_standards.yaml`
- [ ] يحتوي قسم `rules:` (قائمة) وقسم `settings:` (إعدادات عامة)
- [ ] كل قاعدة تحتوي على: `id`, `name`, `severity`, `pattern`, `file_extensions`, `message_ar`, `message_en`, `fix_ar`, `fix_en`
- [ ] الحقول الاختيارية: `exclude_dirs`, `enabled`

### AC-1.1.2: القواعد CS-01..CS-05 معرَّفة
- [ ] CS-01: `== *"(\\x[89a-f][0-9a-f]\\x[0-9a-f]{2})+"` مع severity=FATAL
- [ ] CS-02: `getValue\(\) *= *"` مع severity=FATAL
- [ ] CS-03: `\bKW_[A-Z_]+\b` مع severity=FATAL
- [ ] CS-04: `reinterpret_cast` مع severity=WARN
- [ ] CS-05: حجم ملف > 800 سطر مع severity=WARN

### AC-1.1.3: التحقق من الصحة
- [ ] الملف YAML صحيح (يُحمَّل بـ `yaml.safe_load` بدون استثناء)
- [ ] كل قاعدة FATAL تشمل `fix_ar` و `fix_en`
- [ ] المسارات المستثناة `exclude_dirs` تشمل: `archived/`, `third_party/`

---

## المهام التقنية

### Task 1: إنشاء الملف
```
scripts/lint/code_standards.yaml
```
- أنشئ بنية YAML الكاملة مع قسمي `settings` و `rules`
- أضف التعليقات التوضيحية بالعربية والإنجليزية

### Task 2: تعريف CS-01 (مقارنة النص العربي)
- Pattern: `'== *"(\\x[89a-f][0-9a-f]\\x[0-9a-f]{2})+"'`
- البديل: `'== *"[\\u0600-\\u06ff]+'` للنصوص العربية المباشرة
- يطبق على: `.cpp`, `.h`, `.cc`

### Task 3: تعريف CS-02..CS-05
- CS-02 خاص بملفات parser (`shared/parser/**`, `interpreter/**`)
- CS-03 يطبق على جميع الملفات
- CS-04, CS-05 بـ WARN (لا تمنع البناء بل تُنبّه)

---

## سياق التنفيذ

### الأنماط الموجودة في المشروع
- `cmake/orphan_sources_guard.cmake` — نمط مشابه للـ YAML-driven guard
- `scripts/codegen/*.yaml` — أمثلة على ملفات YAML في المشروع

### ملاحظات هامة
- regex في YAML يجب escape مضاعف (`\\x` وليس `\x`)
- CS-02 قد يُنتج false positives في ملفات lexer — أضف `exclude_context` إذا لزم
- CS-05 يستخدم عدد السطور (line count) وليس regex

---

## قائمة الملفات المُنشأة/المُعدَّلة
- [ ] `scripts/lint/code_standards.yaml` ← جديد

---

## Dev Agent Record
*(تُملأ من قبل وكيل التطوير أثناء التنفيذ)*

**ما تم تنفيذه:**

**الاختبارات التي أُنشئت:**

**القرارات المتخذة:**

---
