# Story 1.2 — إنشاء ENFORCER (check_code_standards.py)

**Epic:** 1 — نظام Contract-as-Code
**Story:** 1.2
**الحالة:** backlog
**الأولوية:** P0 (حرج)
**الاعتمادية:** Story 1.1 مكتملة
**المرجع:** `prd-contract-as-code.md` § FR-02

---

## وصف القصة

**كمطوِّر في لغة ص،**
أريد سكريبت Python يفحص ملفات C++ وفق قواعد SPEC،
**حتى** يُكتشف أي مخالفة تلقائياً مع تقرير يشير إلى الملف + السطر + الحل.

---

## معايير القبول (AC)

### AC-1.2.1: واجهة CLI
- [ ] يقبل `--dirs DIR [DIR...]` (المسارات المُفحوصة)
- [ ] يقبل `--spec FILE` (مسار YAML، افتراضي: `scripts/lint/code_standards.yaml`)
- [ ] يقبل `--allowlist FILE` (مسار allowlist)
- [ ] يُخرج exit code 0 عند نجاح، 1 عند وجود مخالفة FATAL

### AC-1.2.2: اكتشاف المخالفات
- [ ] CS-01: يكتشف `== "\xNN\xNN..."` في ملفات `.cpp`/`.h`
- [ ] CS-02: يكتشف `getValue() == "..."` في ملفات parser
- [ ] CS-03: يكتشف `KW_SOMETHING` (بدل `KEYWORD_SOMETHING`)
- [ ] CS-04: يكتشف `reinterpret_cast` ويُنبّه (WARN فقط)
- [ ] CS-05: يكتشف ملفات > 800 سطر ويُنبّه

### AC-1.2.3: الاستثناءات
- [ ] الملفات في `code_standards_allowlist.txt` تُتجاهل
- [ ] السطور التي تحتوي `# SAD_LINT_DISABLE: CS-XX` تُتجاهل
- [ ] المجلدات في `exclude_dirs` في YAML تُتجاهل

### AC-1.2.4: تنسيق التقرير
- [ ] كل مخالفة: `[CS-XX] path/to/file.cpp:LINE`
- [ ] يطبع: المشكلة بالعربية + السطر المُخالف + الحل المقترح
- [ ] ملخص: "إجمالي المخالفات: N FATAL, M WARN"

---

## المهام التقنية

### Task 1: هيكل السكريبت
```python
# scripts/lint/check_code_standards.py
# argparse CLI
# load_spec(yaml_file) → list[Rule]
# load_allowlist(txt_file) → set[str]
# check_file(path, rules, allowlist) → list[Violation]
# check_dirs(dirs, rules, allowlist) → list[Violation]
# format_report(violations) → str
# main() → exit code
```

### Task 2: تنفيذ فاحص regex (CS-01..CS-04)
- قراءة كل ملف سطراً سطراً
- تطبيق regex من SPEC
- التحقق من inline disable

### Task 3: تنفيذ فاحص حجم الملف (CS-05)
- عدّ الأسطر لكل `.cpp`/`.h`
- مقارنة مع الحد في SPEC

### Task 4: كتابة pytest
```
scripts/codegen/test_code_standards.py
```
- إنشاء ملفات temp مخالفة → تحقق أن `check_file` يُعيد violations
- إنشاء ملفات temp صحيحة → تحقق أن `check_file` يُعيد []
- اختبار allowlist: ملف في allowlist → لا violations
- اختبار inline disable: `SAD_LINT_DISABLE` → تُتجاهل
- اختبار WARN لا يرفع exit code

---

## سياق التنفيذ

### المرجعيات في المشروع
- `scripts/codegen/test_migration_guard.py` — نمط pytest موجود
- `scripts/codegen/gen_all.py` — نمط تشغيل سكريبت Python
- YAML examples في `scripts/codegen/*.yaml`

### ملاحظات
- استخدم `pathlib.Path` وليس `os.path`
- encoding UTF-8 explicit عند قراءة الملفات
- الـ regex تُترجَم من YAML مباشرة بـ `re.compile(pattern)`
- CS-05 يستخدم `len(file.read_text().splitlines())`

---

## قائمة الملفات المُنشأة/المُعدَّلة
- [ ] `scripts/lint/check_code_standards.py` ← جديد
- [ ] `scripts/codegen/test_code_standards.py` ← جديد

---

## Dev Agent Record
*(تُملأ من قبل وكيل التطوير أثناء التنفيذ)*

**ما تم تنفيذه:**

**الاختبارات التي أُنشئت:**

**القرارات المتخذة:**

---
