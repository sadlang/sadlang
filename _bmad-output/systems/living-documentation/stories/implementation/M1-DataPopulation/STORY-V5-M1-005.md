---
id: S-V5-M1-005
title: "نطاق errors — رسائل الأخطاء (281KB حسب category)"
type: implementation-story
milestone: M1-DataPopulation
scope: errors
entity_count: 200
codegen: gen_error_messages.py (تحديث ⚠️)
status: ready
estimate: L
depends_on: [S-V5-M1-001]
date: 2026-06-05
---

# S-V5-M1-005 — نطاق errors

## القصة
بصفتي **فريق اللغة**، أريد **توثيق رسائل الأخطاء (~200) منظَّمة حسب `category`** حتى **تعرض الأدوات رسائل + اقتراحات إصلاح ثنائية اللغة**.

## السياق
`error_messages.yaml` (281KB) يُقسَّم حسب `category` field **داخل YAML**، لا ملفات منفصلة (ADR-003 §قَرار 8) — لتجنب كسر API.

## معايير القبول
- **Given** `error_messages.yaml` موجود (281KB) + Schema error جاهز
- **When** ننقله إلى `language-truth/` ونُضيف/نتحقق من `category` + `id` لكل خطأ
- **Then** كل خطأ مُتحقَّق + codegen يعمل + البناء أخضر

## المهام
- [ ] نقل `error_messages.yaml` → `language-truth/errors.yaml`
- [ ] تأكيد `category` (lexical/syntactic/semantic/runtime/ownership/import/io/internal)
- [ ] تأكيد `fix_suggestion_ar` و `fix_suggestion_en` لكل خطأ (تمهيد T4)
- [ ] تحديث مسار `gen_error_messages.py` في codegen
- [ ] T1 (Schema) + بناء أخضر

## المصدر في الكود
- `shared/errors/`
- `data/language/error_messages.yaml`، `scripts/codegen/gen_error_messages.py`

## تعريف "تم"
YAML يطابق Schema + كل خطأ له ID فريد + `category` + بناء أخضر + لا كسر API الأخطاء.
