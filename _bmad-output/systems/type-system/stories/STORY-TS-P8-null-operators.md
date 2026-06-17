---
id: S-TS-P8
title: "عوامل أمان null: تسجيل ?./?? في SoT + !! الجديد + ؟ عربي"
type: implementation-story
milestone: M3-AdvancedTypes
status: ready
estimate: M
depends_on: [S-TS-P1, S-TS-P4]
governed_by:
  - ../docs/TYPE_SYSTEM_COMPLETION_PLAN.md
  - ../decisions/ADR-TYPESYSTEM-001-NULL-SAFETY.md
  - language-truth/operators.yaml
date: 2026-06-12
---

# S-TS-P8 — عوامل أمان null (تسجيل + إكمال)

> **تصحيح جوهري بعد نقد أميليا:** ادّعاء «سطح نظيف» في النسخة الأولى **باطل**. `?.` و`??` **منفّذان عبر كل الطبقات**: المعجم ([token.h:391-392](../../../../shared/lexer/include/token.h#L391))، عقد AST، المحلّل، المفسّر ([members_advanced.cpp:680,733](../../../../interpreter/src/visitors/expression_evaluator_members_advanced.cpp#L680))، والمترجم ([expression_nullsafety.cpp:21,111](../../../../compiler/src/frontend/builders/expression_nullsafety.cpp#L21)). فالعمل **ليس إضافة** بل **تسجيل + إعادة إرساء + إكمال**.

## القصة
بصفتي **مستخدم لغة ص**, أريد **عوامل أمان null مسجّلة في SoT ومبنيّة على Null حقيقي، مع `!!`** حتى **تكون متّسقة وموثّقة لا مبنيّة على خلط void**.

## السياق
العوامل القائمة `?.`/`??` (لاتينية) **غير مسجّلة في `operators.yaml`** (خرق SoT-First في كود قائم)، و**تفحص null عبر `isVoid()`** ([members_advanced.cpp:686,739](../../../../interpreter/src/visitors/expression_evaluator_members_advanced.cpp#L686)) — مبنيّة على الخلط الذي يصلحه S-TS-P1. ينفّذ [ADR-TYPESYSTEM-001 §4](../decisions/ADR-TYPESYSTEM-001-NULL-SAFETY.md).

## معايير القبول
- **AC1 — SoT-First** تسجيل `?.` و`??` في `language-truth/operators.yaml` (token + أسبقية + description_ar/en) — توثيق ما هو منفّذ أصلًا.
- **AC2 — `!!` جديد** token `BANG_BANG` (أو كلمة `مؤكَّد`): يفرض `T? → T`، يرمي خطأ **عبر الكتالوج** إن null. مع فضّ غموض `!!`=نفيين متتاليين.
- **AC3 — `؟` عربي** فرع جديد في الـlexer (`scanToken` عند U+061F → `QUESTION`) ليكون مرادفًا لـ`?` اللاتيني. **مهمّة صريحة لا افتراض.**
- **AC4 — فضّ الغموض** قاعدة `T?` (لاحقة نوع، موضع النوع) مقابل الثلاثي `? :` ([operators.yaml:20](../../../../language-truth/operators.yaml#L20)، موضع التعبير) — موثّقة ومُختبَرة.
- **AC5** اختبار `.ص` إيجابي+سلبي عبر `runner.py` (P0+P1)، بناء `sad-run`+`sadc` نظيف، كلا المحرّكين، **لا انحدار** في سلوك `?.`/`??` القائم (BF-29).

## المهام
- [ ] تسجيل `?.`/`??` في `operators.yaml` + إعادة التوليد (ضمّ المُولَّد).
- [ ] إضافة `!!` (token + معجم + محلّل + مفسّر + مترجم + خطأ كتالوج).
- [ ] فرع `؟` العربي في `lexer_core.cpp` `scanToken`.
- [ ] توثيق + اختبار قاعدة فضّ غموض الثلاثي.
- [ ] اختبارات انحدار لـ`?.`/`??` + تحديث `status/` بدليل.

## المصدر في الكود
- `language-truth/operators.yaml` (تسجيل + `!!`)
- `shared/lexer/src/lexer_core.cpp` (`scanToken` — `؟` عربي)
- `shared/lexer/include/token.h:391-392` (QUESTION_DOT/QUESTION_QUESTION موجودان)
- `interpreter/src/visitors/expression_evaluator_members_advanced.cpp:680,733`
- `compiler/src/frontend/builders/expression_nullsafety.cpp`
- `language-truth/errors/runtime.yaml` (خطأ `!!` على null)

## تعريف "تم"
`?.`/`??` مسجّلان في SoT، `!!` يعمل في المحرّكين بخطأ كتالوج، `؟` عربي مدعوم، فضّ الغموض موثّق+مُختبَر، لا انحدار.
