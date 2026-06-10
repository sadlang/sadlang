---
id: S-V5-M1-003
title: "نطاق type_methods — طرق الأنواع (80 method)"
type: implementation-story
milestone: M1-DataPopulation
scope: type_methods
entity_count: 80
codegen: gen_type_methods.py (موجود ✅ — نقل فقط)
status: ready
estimate: S
depends_on: [S-V5-M1-001]
priority: high
date: 2026-06-05
---

# S-V5-M1-003 — نطاق type_methods ⭐

## القصة
بصفتي **فريق اللغة**، أريد **نقل طرق الأنواع الـ80 (مصفوفة/نص/خريطة/قناة/قفل/مستقبل/مجموعة_انتظار)** حتى **توثَّق `.اضف`/`.رتب`/`.تقسيم`/`.أرسل` كحقيقة لغوية**.

## السياق
**أولوية عالية — البنية التحتية موجودة وملتزمة في git:** `data/language/type_methods.yaml` (80 method, 7 targets, v1.0) + `gen_type_methods.py` + `type_methods_generated.{h,cpp}`. هذا **نقل + تحقق فقط** لا بناء من صفر.

## معايير القبول
- **Given** `data/language/type_methods.yaml` موجود (80 method) + codegen يعمل
- **When** ننقله إلى `language-truth/type_methods.yaml` + نُحدِّث مسار codegen
- **Then** `type_methods_generated` مُطابق للسابق (`--diff` = صفر) + البناء أخضر

## المهام
- [x] كتابة `language-truth/type_methods.yaml` من الكود الفعلي (83 طريقة، 7 أهداف)
- [x] المصدر: expression_evaluator_oop_*.cpp (الكود الفعلي لا copilot-instructions)
- [x] الحفاظ على الـ aliases (عيّن/عين، أضف/اضف، إلخ) — موثَّقة
- [ ] gen_type_methods.py — غير موجود في المشروع (مُرجَأ لمرحلة M2)
- [ ] cmake/codegen.cmake — مُرجَأ حتى توفر gen_type_methods.py

## المصدر في الكود
- `data/language/type_methods.yaml` (80 method, 7 targets)
- `scripts/codegen/gen_type_methods.py`، `shared/builtins/generated/type_methods_generated.{h,cpp}`
- `parser/specs/types/`، `interpreter/.../expression_evaluator_oop_*_methods.cpp`

## تعريف "تم"
`type_methods_generated` بلا تغيير (دليل `git diff`) + Schema أخضر + `kTypeMethodCount = 80`.
