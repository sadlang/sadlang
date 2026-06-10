---
status: done
slug: utm-6-3-gen-builtins-core
created: 2026-06-09
completed: 2026-06-09
scope: UTM-6.3 — ترحيل Core + TypeCtor + Concurrency (15 ثابت) + بناء gen_builtins.py
epic: UTM-6 (codegen unification — ADR-006a)
priority: high
---

# UTM-6.3 — gen_builtins.py + Core/TypeCtor/Concurrency

## السياق (Context)

الوضع الراهن:
- UTM-6.1 ✅: `_lib/` (4 ملفات) + gen_keywords + gen_type_methods + gen_all.py — 44/44 pytest
- UTM-6.2 ✅: gen_modules.py + modules.yaml + modules_generated.h — 3/3 generators، C++ build نظيف

الهدف: ترحيل أول **15 ثابت** من `builtin_registry.h` (Core + TypeCtor + Concurrency) إلى YAML مُولِّد،  
وبناء `gen_builtins.py` كأداة قادرة على ترحيل **555 ثابت إجمالية** في UTM-6.4 → 6.7.

**الملفات المرجعية:**
- `shared/builtins/include/builtin_registry.h` — المصدر الحالي (552 ثابت يدوي)
- `scripts/codegen/_lib/` — المكتبة المشتركة (loader, emit, validator, context)
- `docs/توثيقات_تطوير_اللغة/توثيقات_الخطط/ADR-006a_توحيد_codegen.md` — الخطة الكاملة

**الثوابت المستهدفة (15):**

```cpp
// Core (5)
Names::Core::PRINT    = "اطبع"
Names::Core::PRINTLN  = "اطبع_سطر"
Names::Core::READ     = "اقرأ"
Names::Core::LENGTH   = "طول"
Names::Core::TYPE     = "نوع"

// TypeCtor (4)
Names::TypeCtor::TO_INT    = "رقم"
Names::TypeCtor::TO_FLOAT  = "عشري"
Names::TypeCtor::TO_STRING = "نص"
Names::TypeCtor::TO_BOOL   = "منطقي"

// Concurrency (6)
Names::Concurrency::CHANNEL    = "قناة"
Names::Concurrency::WAIT_ALL   = "انتظر_الكل"
Names::Concurrency::TASK_COUNT = "عدد_المهام"
Names::Concurrency::WAIT_GROUP = "مجموعة_انتظار"
Names::Concurrency::MUTEX      = "قفل"
Names::Concurrency::FUTURE     = "مستقبل"
```

---

## معايير القبول (Acceptance Criteria)

**AC-1 (diff=0 — Core):**
Given `gen_builtins.py` يُشغَّل على `builtins/core.yaml`،
When أقارن `builtin_names_generated.h` بالثوابت المقابلة في `builtin_registry.h`،
Then القيم متطابقة حرفياً لكل ثابت في Core + TypeCtor + Concurrency.

**AC-2 (C++ build):**
Given `builtin_registry.h` بعد حذف Core + TypeCtor + Concurrency وإضافة `#include "builtin_names_generated.h"`،
When `cmake --build build --target sad`،
Then ينتهي البناء بدون أي خطأ أو تحذير جديد.

**AC-3 (gen_all.py):**
Given تنفيذ `python gen_all.py`،
Then الخرج: `[gen_builtins] OK: 15 builtins (header updated)` ومجموع `4/4 passed`.

**AC-4 (اختبارات وحدوية):**
Given تنفيذ `python -m pytest scripts/codegen/ -v`،
Then جميع الاختبارات تنجح (44 + اختبارات gen_builtins الجديدة).

**AC-5 (backward compatibility — أسبقية قصوى):**
Given أي ملف في المشروع يستخدم `Names::Core::PRINT` أو `Names::Concurrency::CHANNEL`،
When المشروع يُبنى،
Then لا يوجد أي خطأ تجميع — الـ API ثابت تماماً (CW-24).

**AC-6 (schema validation):**
Given ملف `core.yaml` يحتوي دالة بـ `cpp_id: "print_lowercase"` (حروف صغيرة)،
When يُشغَّل `gen_builtins.py`،
Then يرفض الإدخال بخطأ واضح (pattern يشترط UPPER_SNAKE_CASE).

---

## المهام (Tasks)

### مهمة T1: Schema + YAML data
- [ ] إنشاء مجلد `data/language/builtins/`
- [ ] إنشاء `data/language/builtin_function.schema.json`:
  ```json
  {
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "builtin_function",
    "type": "object",
    "required": ["cpp_id", "canonical", "namespace"],
    "properties": {
      "cpp_id": {
        "type": "string",
        "pattern": "^[A-Z][A-Z0-9_]*$",
        "description": "(AR) اسم ثابت C++ بصيغة UPPER_SNAKE_CASE"
      },
      "canonical": {
        "type": "string",
        "minLength": 1,
        "description": "(AR) الاسم القانوني العربي الوحيد للدالة"
      },
      "namespace": {
        "type": "string",
        "description": "(AR) اسم الـ namespace داخل Names:: (مثال: Core)"
      },
      "description_ar": {"type": "string"},
      "description_en": {"type": "string"}
    },
    "additionalProperties": false
  }
  ```
- [ ] إنشاء `data/language/builtins/core.yaml`:
  ```yaml
  version: 1
  namespace_group: core
  description_ar: "الدوال المدمجة الأساسية (بدون استيراد) — Core + TypeCtor + Concurrency"
  functions:
    # ─── Core (5) ───
    - cpp_id: PRINT
      canonical: "اطبع"
      namespace: Core
      description_ar: "طباعة بدون سطر جديد"
      description_en: "Print without newline"
    - cpp_id: PRINTLN
      canonical: "اطبع_سطر"
      namespace: Core
      description_ar: "طباعة مع سطر جديد"
      description_en: "Print with newline"
    - cpp_id: READ
      canonical: "اقرأ"
      namespace: Core
      description_ar: "قراءة مدخل من المستخدم"
      description_en: "Read user input"
    - cpp_id: LENGTH
      canonical: "طول"
      namespace: Core
      description_ar: "طول المجموعة أو النص"
      description_en: "Length of collection or string"
    - cpp_id: TYPE
      canonical: "نوع"
      namespace: Core
      description_ar: "نوع القيمة"
      description_en: "Type of value"
    # ─── TypeCtor (4) ───
    - cpp_id: TO_INT
      canonical: "رقم"
      namespace: TypeCtor
      description_ar: "تحويل إلى عدد صحيح"
      description_en: "Convert to integer"
    - cpp_id: TO_FLOAT
      canonical: "عشري"
      namespace: TypeCtor
      description_ar: "تحويل إلى عدد عشري"
      description_en: "Convert to float"
    - cpp_id: TO_STRING
      canonical: "نص"
      namespace: TypeCtor
      description_ar: "تحويل إلى نص"
      description_en: "Convert to string"
    - cpp_id: TO_BOOL
      canonical: "منطقي"
      namespace: TypeCtor
      description_ar: "تحويل إلى منطقي"
      description_en: "Convert to boolean"
    # ─── Concurrency (6) ───
    - cpp_id: CHANNEL
      canonical: "قناة"
      namespace: Concurrency
      description_ar: "إنشاء قناة للتواصل بين الخيوط"
      description_en: "Create channel for goroutine communication"
    - cpp_id: WAIT_ALL
      canonical: "انتظر_الكل"
      namespace: Concurrency
      description_ar: "انتظار جميع الخيوط الخفيفة"
      description_en: "Wait for all goroutines to complete"
    - cpp_id: TASK_COUNT
      canonical: "عدد_المهام"
      namespace: Concurrency
      description_ar: "عدد المهام النشطة حالياً"
      description_en: "Current active task count"
    - cpp_id: WAIT_GROUP
      canonical: "مجموعة_انتظار"
      namespace: Concurrency
      description_ar: "إنشاء مجموعة انتظار"
      description_en: "Create wait group"
    - cpp_id: MUTEX
      canonical: "قفل"
      namespace: Concurrency
      description_ar: "إنشاء قفل للتزامن"
      description_en: "Create mutex for synchronization"
    - cpp_id: FUTURE
      canonical: "مستقبل"
      namespace: Concurrency
      description_ar: "إنشاء مستقبل لنتيجة غير متزامنة"
      description_en: "Create future for async result"
  ```

### مهمة T2: gen_builtins.py
- [ ] إنشاء `scripts/codegen/gen_builtins.py`
- البنية: يقرأ `--yaml builtins/core.yaml` → يُجمِّع الدوال حسب namespace → يُنتج:
  ```cpp
  // namespace Sad::Builtins::Names
  namespace Core {
      inline constexpr std::string_view PRINT    = "اطبع";
      inline constexpr std::string_view PRINTLN  = "اطبع_سطر";
      // ...
  }
  namespace TypeCtor {
      // ...
  }
  namespace Concurrency {
      // ...
  }
  ```
- يستخدم `_lib/loader.py` + `_lib/emit.py` + `_lib/validator.py`
- يفحص: لا تكرار في `cpp_id` + لا تكرار في `canonical` + `cpp_id` بصيغة UPPER_SNAKE_CASE
- **المخرج:** `shared/builtins/generated/builtin_names_generated.h`
- **CLI args:** `--yaml`, `--schema`, `--out-h`, `--repo-root`, `--quiet`
- [ ] تسجيل في `gen_all.py` (المولِّد الرابع):
  ```python
  {
      "name": "builtins",
      "script": codegen / "gen_builtins.py",
      "args": [
          "--yaml",   str(repo / "data" / "language" / "builtins" / "core.yaml"),
          "--schema", str(repo / "data" / "language" / "builtin_function.schema.json"),
          "--out-h",  str(repo / "shared" / "builtins" / "generated" / "builtin_names_generated.h"),
      ],
  }
  ```
- [ ] إضافة `sad_builtins_codegen` في `cmake/codegen.cmake` + تحديث `sad_all_codegen` (4 targets)

### مهمة T3: تحديث builtin_registry.h
- [ ] حذف `namespace Core { ... }` الكامل (سطور 80-86)
- [ ] حذف `namespace TypeCtor { ... }` الكامل (سطور 89-94)
- [ ] حذف `namespace Concurrency { ... }` الكامل (سطور 97-104)
- [ ] إضافة في بداية `namespace Names {` (بعد السطر 77):
  ```cpp
  // (AR) ثوابت مُولَّدة تلقائياً من data/language/builtins/core.yaml
  // (AR) لا تُعدِّل هذا المقطع يدوياً — استخدم gen_builtins.py
  // (EN) Auto-generated constants from data/language/builtins/core.yaml
  // (EN) Do NOT edit this section manually — use gen_builtins.py
  ```
- [ ] إضافة `#include "../generated/builtin_names_generated.h"` بعد `#include "module_definitions.h"`
- [ ] التحقق: `cmake --build build --target sad` ينجح بدون أخطاء

### مهمة T4: اختبارات وحدوية
- [ ] إنشاء `scripts/codegen/test_gen_builtins.py`
- اختبارات إلزامية:
  - `test_diff_zero_core`: يُشغِّل gen_builtins → يتحقق من PRINT="اطبع" وكل ثابت مُرحَّل
  - `test_diff_zero_typector`: يتحقق من TO_INT="رقم" إلخ
  - `test_diff_zero_concurrency`: يتحقق من CHANNEL="قناة" إلخ
  - `test_schema_rejects_lowercase_cpp_id`: `cpp_id: "print"` يرفضه schema
  - `test_schema_rejects_empty_canonical`: `canonical: ""` يرفضه schema
  - `test_no_duplicate_cpp_id`: دالتان بنفس `cpp_id` يُوقف التوليد بخطأ
  - `test_no_duplicate_canonical`: دالتان بنفس `canonical` يُوقف التوليد بخطأ
- [ ] تشغيل: `python -m pytest scripts/codegen/ -v` — يجب أن تمر **44 + N اختبار**

---

## قرارات التصميم الصارمة

1. **namespace grouping:** يُجمَّع في ملف مُولَّد واحد (`builtin_names_generated.h`) — ليس ملف لكل namespace
2. **UTM-6.4+ شامل:** عند ترحيل Math + Strings + ... يُمرَّر ملف YAML إضافي لـ gen_builtins — لا تغيير في API
3. **لا تكسر API:** `Names::Core::PRINT` يعمل بعد الترحيل بنفس الطريقة — لا تغيير في namespace structure
4. **write_if_changed:** إذا لم يتغير YAML، لا يُعاد كتابة الـ header (بناء deterministic)

---

## سجل المطوِّر (Dev Agent Record)

### التنفيذ
- [ ] T1: schema + YAML
- [ ] T2: gen_builtins.py + gen_all.py + cmake
- [ ] T3: builtin_registry.h
- [ ] T4: test_gen_builtins.py

### الملفات المُغيَّرة
*(تُحدَّث بعد كل task)*
- [ ] `data/language/builtin_function.schema.json` — جديد
- [ ] `data/language/builtins/core.yaml` — جديد
- [ ] `scripts/codegen/gen_builtins.py` — جديد
- [ ] `scripts/codegen/test_gen_builtins.py` — جديد
- [ ] `scripts/codegen/gen_all.py` — تحديث (4/4)
- [ ] `cmake/codegen.cmake` — تحديث (4 targets)
- [ ] `shared/builtins/generated/builtin_names_generated.h` — جديد (مُولَّد)
- [ ] `shared/builtins/include/builtin_registry.h` — تحديث (حذف 15 ثابت)
