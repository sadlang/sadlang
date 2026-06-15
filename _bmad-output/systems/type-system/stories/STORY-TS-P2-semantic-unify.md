---
id: S-TS-P2
title: "توحيد المدقّق الدلالي على SadTypeKind"
type: implementation-story
milestone: M2-CoreSafety
status: ready
estimate: L
depends_on: [S-TS-P0]
governed_by:
  - ../docs/TYPE_SYSTEM_COMPLETION_PLAN.md
date: 2026-06-12
---

# S-TS-P2 — توحيد المدقّق الدلالي على SadTypeKind

## القصة
بصفتي **مشرف معمارية الأنواع**, أريد **المدقّق الدلالي يعمل على `SadTypeKind`** حتى **يكتمل ادّعاء «تحويل السيستم بالكامل» في قلب التحقّق**.

## السياق
المدقّق الدلالي يستعمل `DataType` القديم في **28 موضعًا** مقابل `SadTypeKind` القليلة:
- `type_checker.cpp`: **DataType ×18** مقابل `SadTypeKind ×3`.
- `type_checker_stmt_visitors.cpp`: **DataType ×10**.
- `type_checker_expr_visitors.cpp`: **DataType ×0** (لا عمل فيه — تصحيح بعد نقد أميليا ع-5؛ لا تبدأ منه).

## معايير القبول
- **AC1 — Given** **28 موضع** `DataType` (18 + 10) عبر `type_checker.cpp` و`_stmt_visitors.cpp`
  **When** نرحّلها بدفعات صغيرة إلى `SadTypeKind` عبر الجسر
  **Then** عدد مواضع `DataType` في هذين الملفّين ينخفض من 28 → **0** (حدّ رقمي قابل للقياس بـ`grep -c`).
- **AC2** اختبارات الدلالات خضراء بعد كل دفعة — لا انحدار سلوكي.
- **AC3** كل دفعة = PR مستقلّ صغير (لا دفعة واحدة كبيرة).

## المهام
- [ ] جرد 28 موضع `DataType` (18 في `type_checker.cpp` + 10 في `_stmt_visitors.cpp`).
- [ ] ترحيل دُفعيّ: ابدأ بـ`type_checker.cpp` ثم `type_checker_stmt_visitors.cpp` (تجاهل `expr_visitors` — صفر مواضع).
- [ ] اختبار دلالي بعد كل دفعة + `grep -c DataType` للتحقّق من الانخفاض.
- [ ] توثيق المتبقّي (إن وُجد) على حدود الجسر.

## المصدر في الكود
- `shared/semantic/src/semantic/type_checker.cpp` (DataType ×18، SadTypeKind ×3)
- `shared/semantic/src/semantic/type_checker_stmt_visitors.cpp` (DataType ×10)
- ~~`type_checker_expr_visitors.cpp`~~ (×0 — لا عمل)

## تعريف "تم"
`grep -c DataType` في `type_checker.cpp` + `_stmt_visitors.cpp` = **0** (أو محصور بحدود الجسر موثّقًا) + الدلالات خضراء. (تمهيد لـ S-TS-P2.5a/b.)
