---
id: S-V5-M1-008
title: "نطاق types — أسماء الأنواع المدمجة (9)"
type: implementation-story
milestone: M1-DataPopulation
scope: types
entity_count: 9
codegen: gen_types.py (جديد ❌)
status: ready
estimate: S
depends_on: [S-V5-M1-001]
date: 2026-06-05
---

# S-V5-M1-008 — نطاق types

## القصة
بصفتي **فريق اللغة**، أريد **توثيق الأنواع المدمجة الـ9 (`رقم`/`عشري`/`نص`/`منطقي`/`فراغ`/`عدم`/`مصفوفة`/`خريطة`/`أي`)** حتى **تعرف الأدوات الأنواع وخصائصها (ليست محجوزة)**.

## معايير القبول
- **Given** Schema type جاهز (M0-003)
- **When** نكتب `language-truth/types.yaml` + ننشئ `gen_types.py`
- **Then** كل نوع له اسم عربي + إنجليزي + DataType + ملاحظة "مُعرّف عادي" + بناء أخضر

## المهام
- [ ] كتابة `types.yaml`: الأنواع الـ9 مع DataType المقابل
- [ ] توثيق أن أسماء الأنواع مُعرّفات عادية (قابلة للاستخدام كمتغيرات)
- [ ] إنشاء `gen_types.py` + ربط في `codegen.cmake`
- [ ] T1 + بناء أخضر

## المصدر في الكود
- `shared/parser/src/specs/types/`، `shared/types/include/value.h`

## تعريف "تم"
YAML يطابق Schema + الأنواع الـ9 موثَّقة + بناء أخضر.
