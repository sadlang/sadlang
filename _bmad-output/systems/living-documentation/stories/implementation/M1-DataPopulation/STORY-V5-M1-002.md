---
id: S-V5-M1-002
title: "نطاق builtins — دوال عامة + تزامن"
type: implementation-story
milestone: M1-DataPopulation
scope: builtins
entity_count: 21
codegen: gen_builtins.py (تحديث ⚠️)
status: ready
estimate: M
depends_on: [S-V5-M1-001]
date: 2026-06-05
---

# S-V5-M1-002 — نطاق builtins

## القصة
بصفتي **فريق اللغة**، أريد **توثيق الدوال المدمجة تلقائياً (اطبع/طول/نوع/تحويلات/تزامن)** حتى **تعرف الأدوات الدوال المتاحة بلا استيراد**.

## معايير القبول
- **Given** Schema builtin_function جاهز + `gen_builtins.py` موجود
- **When** نكتب `language-truth/builtins/*.yaml` (core/concurrency/types) ونُحاذيها مع توقعات الأداة
- **Then** الكيانات مُتحقَّقة + `builtin_names_generated.h` مُولَّد + البناء أخضر

## المهام
- [x] كتابة `builtins/core.yaml` (5 دوال: اطبع/اطبع_سطر/اقرأ/طول/نوع)
- [x] كتابة `builtins/types.yaml` (4 دوال: رقم/عشري/نص/منطقي)
- [x] كتابة `builtins/concurrency.yaml` (6 دوال: قناة/انتظر_الكل/عدد_المهام/مجموعة_انتظار/قفل/مستقبل)
- [x] كتابة `builtins/_index.yaml`
- [x] مواءمة بنية YAML مع `gen_builtins.py` (--yaml file1 file2 file3) — CODEGEN OK
- [x] تحديث `builtin_function.schema.json` — JSON صالح
- [x] المخرج مطابق للقديم (data/stdlib/core.yaml) باستثناء مسارات المصدر

## المصدر في الكود
- `interpreter/src/builtins/`، `shared/builtins/`
- `scripts/codegen/gen_builtins.py`، `data/stdlib/core.yaml`

## تعريف "تم"
YAML يطابق Schema + كل دالة لها ID فريد + `gen_builtins.py` يبني + لا hardcode في `builtin_registry.h`.
