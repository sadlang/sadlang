---
id: S-TS-P2.5a
title: "ترحيل حقول DataType في الـAST + المحلّل إلى SadTypeKind"
type: implementation-story
milestone: M2-CoreSafety
status: ready
estimate: L
depends_on: [S-TS-P2]
governed_by:
  - ../docs/TYPE_SYSTEM_COMPLETION_PLAN.md
date: 2026-06-12
---

# S-TS-P2.5a — ترحيل DataType في الـAST/المحلّل

> **تقسيم بعد نقد أميليا (ع-6):** قُسِّمت P2.5 الأصلية إلى **P2.5a (هذه — الترحيل)** ثم **S-TS-P2.5b (حذف دوال DataType من الجسر)**؛ لأن 23 ملفًّا + حذف + إثبات حيّ في قصّة L واحدة غير واقعي.

## القصة
بصفتي **مشرف معمارية الأنواع**, أريد **تحويل حقول النوع في عقد الـAST والمحلّل من `Data::DataType` إلى `Types::SadTypeKind`** حتى **يصبح حذف دوال DataType من الجسر ممكنًا (S-TS-P2.5b)**.

## السياق
الجسر سقالة انتقالية. فقدانه للأنواع المتقدّمة (`SadTypeKind`→**ValueType**: `Optional→Void`, `Result→Void` في `shared/types/src/type_bridge.cpp:118-121`) عَرَضٌ لكون النوع الهدف أفقر تعبيرًا. `DataType` مغروس في عقد الـAST (`shared/ast/include/ast_node.h:191` `getDataType()`، `declarations.h:57,237,280,716,911,964`).

**نطاق مؤكّد (grep):** `DataType` في **23 ملفًّا** = 12 `shared/ast` + 8 `shared/parser` + 3 `shared/semantic` (الأخيرة يغطّيها S-TS-P2). (أميليا قالت 24؛ التحقّق الفعلي 23.)

## معايير القبول
- **AC1 — Given** حقول النوع في عقد الـAST من `Data::DataType`
  **When** نحوّلها إلى `Types::SadTypeKind`
  **Then** البناء أخضر بعد كل دفعة.
- **AC2** المحلّل (`shared/parser`، 8 ملفّات) يُنتج `SadTypeKind` مباشرة لا `DataType`.
- **AC3** `grep -c DataType` في `shared/ast` + `shared/parser` = **0** (semantic يكمّله S-TS-P2).
- **AC4** المستهلكون (المفسّر) يقرؤون `SadTypeKind` دون عبور دوال DataType في الجسر.
- **AC5** بناء `sad-run` + المترجم = 0، كل الاختبارات خضراء، لا انحدار سلوكي.

## المهام (بالترتيب، دفعات صغيرة)
- [ ] تحويل حقول النوع في `declarations.h` (السطور 57,237,280,716,911,964), `ast_node.h:191`, `class_nodes`/`type_nodes`.
- [ ] تحويل بقية ملفّات `shared/ast` الـ12.
- [ ] جعل `shared/parser` (8 ملفّات) يُنتج `SadTypeKind`.
- [ ] تحديث المستهلكين في المفسّر.
- [ ] بناء + اختبار بعد كل دفعة + `grep -c DataType`.

## المصدر في الكود
- `shared/types/include/data_types.h:26`
- `shared/ast/include/ast_node.h:191`, `shared/ast/include/declarations.h:57,237,280,716,911,964`
- `shared/ast/*` (12 ملفًّا), `shared/parser/*` (8)

## تعريف "تم"
`grep -c DataType` في `shared/ast`+`shared/parser` = 0 + بناء+اختبارات خضراء. (تمهيد لـ S-TS-P2.5b.)
