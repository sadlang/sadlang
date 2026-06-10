---
id: S-V5-M1-011
title: "نطاق stdlib — دوال المكتبة القياسية"
type: implementation-story
milestone: M1-DataPopulation
scope: stdlib_functions
entity_count: 119
codegen: gen_stdlib.py (جديد ❌)
status: ready
estimate: L
depends_on: [S-V5-M1-001]
date: 2026-06-05
---

# S-V5-M1-011 — نطاق stdlib

## القصة
بصفتي **فريق اللغة**، أريد **توثيق دوال المكتبة القياسية (~119) داخل كل وحدة** حتى **تعرف الأدوات الدوال المتاحة بعد الاستيراد**.

## السياق
يُعيد استخدام `data/stdlib/*.yaml` القائمة (core + 8 ملفات stdlib). يرتبط بـ M1-004 (modules) عبر `module_id`.

## معايير القبول
- **Given** Schema stdlib_function جاهز + `data/stdlib/*.yaml` موجودة
- **When** ننقل/نوحّد دوال المكتبة إلى `language-truth/stdlib/functions.yaml`
- **Then** كل دالة مُتحقَّقة + مربوطة بوحدتها + C++ Generated + بناء أخضر

## المهام
- [ ] توحيد `data/stdlib/{core,math,strings,arrays,basics,assertions,maps,http,io}.yaml`
- [ ] لكل دالة: الاسم + التوقيع + `module_id` + الوصف ثنائي اللغة
- [ ] إنشاء `gen_stdlib.py` + ربط في `codegen.cmake`
- [ ] T1 + بناء أخضر

## المصدر في الكود
- `stdlib/`، `data/stdlib/*.yaml`

## تعريف "تم"
YAML يطابق Schema + كل دالة لها ID + `module_id` صالح + بناء أخضر.
