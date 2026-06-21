---
id: NS-04
title: "الوصول غير الآمن على T؟ غير المُضيَّق"
type: implementation-story
milestone: NS-M2-Flow
status: done
estimate: M
depends_on: [NS-01, NS-02]
governed_by:
  - ../planning/ARCHITECTURE.md
  - ../decisions/ADR-NS-001-flow-analysis-scope-and-strictness.md
  - ../../type-system/decisions/ADR-TYPESYSTEM-001-NULL-SAFETY.md
date: 2026-06-20
---

# NS-04 — الوصول غير الآمن على T؟ غير المُضيَّق

> يُبنى فوق NS-02 (الصرامة المجرّدة + نقطة الحقيقة الواحدة). يسبق NS-03 (التضييق)
> عمدًا (ADR-NS-001): أساس رصد مستقرّ قبل تحليل التدفّق عالي المخاطرة.

## القصة
بصفتي **مستخدم لغة ص**, أريد **أن يُحذّرني المحلّل حين أصل لعضو (`.عضو`/`.طريقة()`)
على متغيّر اختياريّ `T؟` قد يكون عدمًا دون فحص أو وصول آمن** حتى **أتفادى انهيار
الوصول إلى عدم، ويوجّهني إلى `؟.` أو الحرس `إذا (س != لاشيء)`**.

## السياق
يُبنى في `NullSafetyAnalyzer` المشترك (NS-01)، نقطة حقيقة واحدة يستدعيها المحرّكان
(sad-run + sad-build). لا تضييق تدفّق بعد (NS-03 لاحقًا): كلّ وصول خام على اختياريّ
يُرصَد. الوصول الآمن `؟.` (OptionalChainExpr) مستثنًى.

## معايير القبول (مُتحقَّقة — GR-01)
- **AC1 — الرصد:** `متغير.حقل` (MemberExpr) و`متغير.طريقة()` (MethodCallExpr) على
  متغيّر مُعلَن `T؟` → تشخيص `UnsafeAccessOnOptional`. ✅
- **AC2 — استثناء الآمن:** `متغير؟.عضو` (OptionalChainExpr) **لا يُرصَد**. ✅
- **AC3 — النطاق:** تتبّع المتغيّرات الاختيارية بمكدّس نطاقات (تصريحات `T؟` + معاملات
  الدوال/الطرائق الاختيارية)؛ لا تسرّب بين الدوال (لا إيجابيّ كاذب عابر للنطاق). ✅
- **AC4 — الصرامة (D11):** جدول الصرامة كاملًا فورًا — `--gc` صامت، `--learn` تحذير،
  `--prod` قاتل — دون انتظار NS-03. ✅
- **AC5 — التكافؤ:** المحرّكان يفرضان بنفس الرسالة والصرامة. ✅
- **AC6 — DoD:** اختبار `.ص` ذهبيّ (تكافؤ مزدوج) + سكربت صرامة عبر المحرّكين. ✅

## المصدر في الكود
- `shared/null_safety/include/null_safety/null_safety_analyzer.h` —
  `NullSafetyErrorKind::UnsafeAccessOnOptional` + `analyzeExpr`/`checkOptionalAccess`
  + مكدّس النطاقات (`pushScope/popScope/declareOptional/isOptionalVar`).
- `shared/null_safety/src/null_safety_analyzer.cpp` — ماشي التعابير + تتبّع النطاق.
- الاستدعاء قائم من NS-01/NS-02 في المفسّر والمترجم (لا تغيير في نقاط الاستدعاء —
  التشخيصات الجديدة تمرّ عبر حلقة الإبلاغ نفسها).

## الاختبارات
- `tests/behavior/sections/02_الأنواع_المدمجة/05_لاشيء_والفراغ/055_ns04_unsafe_optional_access.ص`
- `tests/behavior/null_safety/ns04_unsafe_access_check.sh` (8/8)

## تعريف "تم"
`متغير.عضو` على `T؟` يُرصَد بالصرامة الثلاثية في المحرّكين، `؟.` لا يُرصَد، اختبارات خضراء. ✅
