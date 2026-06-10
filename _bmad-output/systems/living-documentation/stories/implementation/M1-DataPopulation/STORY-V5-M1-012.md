---
id: S-V5-M1-012
title: "نطاق learning content — دروس/تمارين/أمثلة"
type: implementation-story
milestone: M1-DataPopulation
scope: learning
entity_count: variable
codegen: gen_docs.py (تحديث ⚠️)
status: ready
estimate: M
depends_on: [S-V5-M1-001]
date: 2026-06-05
---

# S-V5-M1-012 — نطاق learning content ⭐

## القصة
بصفتي **كاتب توثيق تعليمي**، أريد **ربط الدروس والتمارين والأمثلة بكيانات Truth الحقيقية** حتى **لا يوجد درس يشرح ميزة غير موجودة (GR-01)**.

## السياق
Schemas التعلّم (`lesson`/`exercise`/`example`) موجودة في `data/_schemas/`. كل درس يشير إلى كيانات لغوية حقيقية عبر `id`.

## معايير القبول
- **Given** Schemas lesson/exercise/example جاهزة (إعادة استخدام)
- **When** نكتب `language-truth/learning/{lessons,exercises}.yaml`
- **Then** كل درس/تمرين يشير إلى `id` كيان موجود + التحقق يفشل على مرجع وهمي

## المهام
- [ ] كتابة `learning/lessons.yaml` (بنية Diátaxis-tutorial)
- [ ] كتابة `learning/exercises.yaml` + أمثلة مرتبطة
- [ ] لكل درس: `references[]` إلى IDs كيانات Truth
- [ ] تحديث `gen_docs.py` ليستهلك المحتوى + اختبار مرجعية (كل ref موجود)
- [ ] T1 + بناء أخضر

## المصدر في الكود
- `data/_schemas/{lesson,exercise,example}.schema.json`
- `scripts/codegen/gen_docs.py`، `scripts/codegen/doc_ir/doc_ir_builder.py`

## تعريف "تم"
كل درس/تمرين يطابق Schema + كل مرجع `id` موجود في Truth + بناء أخضر.
