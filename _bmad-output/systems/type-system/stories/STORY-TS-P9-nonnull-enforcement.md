---
id: S-TS-P9
title: "فرض غير-قابل-لـnull افتراضيًّا (استنتاج لاشيء + areTypesCompatible)"
type: implementation-story
milestone: M3-AdvancedTypes
status: ready
estimate: L
depends_on: [S-TS-P1, S-TS-P2, S-TS-P4, S-TS-P8]
governed_by:
  - ../docs/TYPE_SYSTEM_COMPLETION_PLAN.md
  - ../decisions/ADR-TYPESYSTEM-001-NULL-SAFETY.md
  - language-truth/errors/semantic.yaml
date: 2026-06-12
---

# S-TS-P9 — فرض أمان null (المدقّق الدلالي)

> **إعادة تأطير جوهرية بعد نقد أميليا:** النسخة الأولى ادّعت أنها «تكسر كودًا قائمًا كثيرًا». **الواقع معكوس** ويجب أن يقود التصميم.

## القصة
بصفتي **مستخدم لغة ص**, أريد **المدقّق يمنع `لاشيء` من التسرّب لنوع غير اختياري** حتى **يصبح أمان null حقيقيًّا لا اسميًّا**.

## السياق + تحليل الأثر الحقيقي (مُتحقَّق منه)

**الفجوة ليست «الفرض مفقود»، بل «`لاشيء` يهرب من الفحص أصلًا»:**
- `LITERAL_NULL → getUnknownType()` ([type_checker.cpp:400](../../../../shared/semantic/src/semantic/type_checker.cpp#L400)) بتعليق صريح «لاشيء = nullable — متوافق مع أي نوع».
- `areTypesCompatible`: `if (expected->isUnknown() || actual->isUnknown()) return true;` ([type_checker.cpp:244](../../../../shared/semantic/src/semantic/type_checker.cpp#L244)) — فـ`لاشيء` (Unknown) **متوافق مع كل شيء**.

**الأرقام الفعلية (grep على `tests/`):**
- `رقم س = لاشيء` (نوع صريح غير اختياري): **0 موضع** — الصيغة التي يكسرها الفرض الساذج **لا أحد يكتبها**.
- `متغير/ثابت/هذا.X = لاشيء` (نوع مُستنتَج): **48 موضعًا** — النمط السائد، **يهرب كليًّا** عبر `Unknown`.

> **النتيجة:** فرض «`Null ⊀ T` عند التصريح الصريح» وحده = **عديم الأثر** (يمسك 0، يفوّت 48). الإصلاح الحقيقي هو **معالجة استنتاج `لاشيء`** أولًا.

**ملكية الدوال (حسم ازدواج كشفته أميليا):**
- `SadType::isAssignableTo:1501` (قاعدة `Void→Null` على مستوى النوع) — يملكها **S-TS-P4** (تُستدعى من الدلالات `:267` والمفسّر `:273` معًا).
- `TypeChecker::areTypesCompatible:244` + استنتاج `LITERAL_NULL:400` (بوّابة الدلالات) — تملكها **هذه القصّة (P9)**. لا ازدواج — دالّتان مختلفتان.

> تضييق التدفّق (smart narrowing) **فُصِل لقصّة مستقلّة** [S-TS-P10](./STORY-TS-P10-flow-narrowing.md) — لأنه بنية flow-analysis كاملة (L وحده)؛ `visitIfStmt:133` لا يقرأ الشرط للتضييق حاليًّا.

## معايير القبول
- **AC1 — استنتاج `لاشيء`** `LITERAL_NULL` يُستنتَج كنوع **Null** (لا `Unknown`) بعد P1، مع **حفظ التوسعة عند إعادة الإسناد** لنمط `متغير س = لاشيء` (السائد، 48 موضعًا) — أي `متغير` المُستنتَج من `لاشيء` يصير `T?` ضمنيًّا لا خطأ.
- **AC2 — الفحص** `areTypesCompatible:244` لا تُمرِّر Null إلى نوع غير اختياري: `رقم س = لاشيء` → **خطأ كتالوج**؛ `رقم؟ س = لاشيء` → مقبول.
- **AC3 — الوصول غير الآمن** `س.حقل` على `T؟` (حيث `T؟` كُتب صراحةً — يلزم **إنجاز P4-AC5**) = خطأ دلالي (استخدم `؟.` أو فحص).
- **AC4 — لا انحدار** الـ48 موضعًا القائمة (`متغير = لاشيء`) تبقى صالحة (AC1 يحفظها)؛ `runner.py --level P0+P1` أخضر.
- **AC5 — رموز كتالوج** في `language-truth/errors/semantic.yaml` (إسناد null لغير اختياري؛ وصول غير آمن) — SoT-First.
- **AC6 — DoD §0.5** اختبار `.ص` إيجابي+سلبي، بناء `sad-run` (Debug)+`sadc` (Release) نظيف، كلا المحرّكين.

## المهام
- [ ] تغيير استنتاج `LITERAL_NULL` (`type_checker.cpp:400`) من Unknown إلى Null + قاعدة توسعة `متغير` (حفظ الـ48).
- [ ] تعديل `areTypesCompatible` (`:244`) ليفرّق Null عن Unknown.
- [ ] رموز خطأ جديدة في `semantic.yaml` + إعادة توليد.
- [ ] فحص الوصول غير الآمن على `T؟` (يعتمد P4-AC5).
- [ ] اختبارات إيجابي/سلبي + انحدار الـ48 + تحديث `status/` بدليل (GR-01).

## المصدر في الكود
- `shared/semantic/src/semantic/type_checker.cpp:400` (استنتاج LITERAL_NULL)، `:244` (areTypesCompatible Unknown)، `:267` (تفويض isAssignableTo)
- `language-truth/errors/semantic.yaml`
- (ملكية P4) `shared/types/include/sad_type_system.h:1501`

## تعريف "تم"
`رقم س = لاشيء` يُرفض، `رقم؟ س = لاشيء` يُقبل، الـ48 موضعًا لا تنحدر، رسائل كتالوج، اختبارات خضراء في المحرّكين.
