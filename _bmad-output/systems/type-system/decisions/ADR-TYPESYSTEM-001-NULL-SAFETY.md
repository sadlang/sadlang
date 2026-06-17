---
id: ADR-TYPESYSTEM-001
title: "نموذج أمان null للغة ص (Null Safety Model)"
status: Proposed
revision: 2
date: 2026-06-12
deciders: [فريق لغة ص]
governed_by:
  - language-truth/types.yaml
  - language-truth/keywords.yaml
  - language-truth/operators.yaml
related_stories: [S-TS-P1, S-TS-P4, S-TS-P8, S-TS-P9]
supersedes_notes: "النسخة 1 احتوت أدلّة كود معكوسة (نقد أميليا)؛ صُحِّحت بالكامل في النسخة 2."
---

# ADR-TYPESYSTEM-001: نموذج أمان null للغة ص (مُراجَع v2)

> **الحوكمة:** قرأت السياسة في `_bmad-output/governance/1-policy/`؛ آخر تقرير تحقّق: `VERIFICATION_REPORT_2026-05-30.md`؛ السبرنت: Sprint #1. **مقترَح** — لا يُعتمَد إلا بموافقة الفريق (GR-02).
>
> **سجل التصحيح (نقد أميليا):** النسخة 1 ادّعت `arabicName()` يستخدم `؟` عربيًّا (هو `?` لاتيني)، وأن العوامل «سطح نظيف» (هي منفّذة)، وأن `isAssignableTo` يعالج Null (يعالج Void). هذه النسخة مبنيّة على الكود الفعلي المُتحقَّق منه بـملف:سطر.

## السياق — الوضع الفعلي المُتحقَّق منه

**الفجوة جوهرية: السطح موجود، الأساس مخلوط.**

السوريّة (SoT) والنحو يميّزان null عن void:
- `لاشيء` = `LITERAL_NULL` (قيمة) ([keywords.yaml:87](../../../../language-truth/keywords.yaml#L87))؛ `عدم` = `TYPE_NULL` (نوع) ([keywords.yaml:186](../../../../language-truth/keywords.yaml#L186))؛ `فراغ` = void ([types.yaml:15-16](../../../../language-truth/types.yaml#L15)).

**سطح أمان null منفّذ بالفعل عبر المحرّكين** (لكن غير مُسجَّل في SoT، ومبنيّ على الخلط):
- العوامل `?.` و`??` (لاتينية): رموز `QUESTION_DOT`/`QUESTION_QUESTION` ([token.h:391-392](../../../../shared/lexer/include/token.h#L391))، عقد AST `OptionalChainExpr`/`NullCoalesceExpr` ([expressions.h:1336](../../../../shared/ast/include/expressions.h#L1336))، محلّل ([parser_expressions.cpp:420,950](../../../../shared/parser/src/core/parser_expressions.cpp#L420))، **مفسّر** ([members_advanced.cpp:680,733](../../../../interpreter/src/visitors/expression_evaluator_members_advanced.cpp#L680))، **ومترجم** ([expression_nullsafety.cpp:21,111](../../../../compiler/src/frontend/builders/expression_nullsafety.cpp#L21)).
- `SadOptionalType` يُرجع `<inner>?` **لاتيني** ([sad_type_system.h:1028](../../../../shared/types/include/sad_type_system.h#L1028)).

**لكن كل هذا السطح يفحص null عبر `isVoid()`** — الخلط مكتوب حرفيًّا في الكود:
- `visitOptionalChainExpr`: `if (lastResult_.isVoid()) ... // VOID = لاشيء` ([members_advanced.cpp:686-688](../../../../interpreter/src/visitors/expression_evaluator_members_advanced.cpp#L686)).
- `visitNullCoalesceExpr`: `if (!lastResult_.isVoid()) return; // إذا لم يكن لاشيء (فراغ)` ([:738-739](../../../../interpreter/src/visitors/expression_evaluator_members_advanced.cpp#L738)).
- `isAssignableTo`: الهدف Optional يقبل `kind_ == SadTypeKind::Void` ([sad_type_system.h:1501](../../../../shared/types/include/sad_type_system.h#L1501)) — أي `Void <: T?` لا `Null <: T?`.
- المترجم: `LITERAL_NULL` يُمثَّل كـ`Integer`+حارس سالب ([expression_basic.cpp:109](../../../../compiler/src/frontend/builders/expression_basic.cpp#L109)).

> **الخلاصة:** المشكلة ليست «لا أمان null»، بل **«أمان null مبنيّ على رمل»**: العوامل تعمل لكنها تكتشف null بفحص void، فلا تميّز بين «null حقيقي» و«دالة لم تُرجع شيئًا». أمان null = 0% **رغم وجود السطح**، لأن الأساس (`SadTypeKind::Null`) مفقود.

## القرار

اعتماد **نموذج Kotlin/Swift: غير-قابل-لـnull افتراضيًّا + `T?` اختياري**، عبر **إرساء السطح القائم على أساس Null حقيقي** (لا بناء سطح جديد).

### 1. `Null` نوع أوّليّ متمايز (الأساس المفقود)
- إضافة `SadTypeKind::Null` + حالة null مستقلّة في `Value` + `bool isNull()` (متمايزة عن `isVoid()`).
- `LITERAL_NULL` (`لاشيء`) يبني Null لا `Value()` الفارغ.
- مُسقِط `نوع()` يُرجع **«عدم»** لـ Null (مطابقة `type.null`)؛ «فراغ» تبقى لـ void.
- **إعادة إرساء السطح القائم**: `visitOptionalChainExpr`/`visitNullCoalesceExpr` + codegen المترجم تفحص `isNull()` بدل `isVoid()`.

### 2. الصيغة: `?` لاتيني أساسٌ، `؟` عربي مهمّة lexer لاحقة
- `T?` (لاتيني، مثل `رقم?`) = سكّر لـ`Optional<T>`.
- > ⚠️ **تصحيح بعد نقد أميليا v2 (تمييز العرض عن التحليل):** لاحقة `?` على نوع **غير مُحلَّلة حاليًّا**. `parseType()` ([parser_helpers.cpp:1610-1691](../../../../shared/parser/src/core/parser_helpers.cpp#L1683)) **لا يستهلك** `QUESTION` بعد اسم النوع، و`OptionalTypeNode` ([type_nodes.h:239](../../../../shared/ast/include/type_nodes.h#L239)) **ميت** (لا يُبنى في أي مكان — grep=صفر). **المنفّذ فعلًا** هو: عرض `arabicName()` (يطبع `?` على `SadOptionalType` مبنيّ برمجيًّا) + العوامل `?.`/`??`. أمّا **كتابة** `رقم?` في المصدر فمهمّة جديدة (انظر §التنفيذ، أُسندت لـ S-TS-P4).
- `?.`/`??` لاتينية = العوامل القائمة (منفّذة عبر المحرّكين).
- `؟` العربي (U+061F) **غير مدعوم حاليًّا** في الـlexer (لا فرع في `scanToken`). دعمه كمرادف = **مهمّة صريحة** (S-TS-P8)، لا افتراض.
- **فضّ الغموض** مع الثلاثي القائم (`? :` ([operators.yaml:20](../../../../language-truth/operators.yaml#L20))): عند تنفيذ تحليل `T?`، تمييز موضع النوع عن التعبير قد يحتاج lookahead — يُصمَّم ويُختبَر في S-TS-P4 (لا يُفترَض «موضع النوع كافٍ» بلا إثبات).

### 3. علاقة الأنواع (تصحيح `isAssignableTo`)
- إضافة: `Null <: T?` لكل `T`؛ `Null ⊀ T` (خطأ)؛ `T <: T?` (ترقية).
- **تغيير صريح** في [sad_type_system.h:1501](../../../../shared/types/include/sad_type_system.h#L1501): الشرط `kind_ == Void` يُستبدَل/يُكمَّل بـ`kind_ == Null` (Void لم يعد يساوي null).

### 4. العامل المفقود الوحيد: `!!` (تأكيد عدم-null)
- `?.` و`??` **موجودان** — العمل: تسجيلهما في `operators.yaml` (SoT) + إعادة إرسائهما على `isNull`.
- `!!` (أو كلمة `مؤكَّد`) **جديد فعلًا** (لا token `BANG_BANG`): يفرض `T? → T`، يرمي خطأ كتالوج إن null. يلزم فضّ غموض `!!`=نفيين.

### 5. تضييق النوع بالتدفّق (Smart Narrowing)
داخل `إذا (س != لاشيء)`: `س` يُعامَل كـ`T` غير اختياري. يُنفّذه المدقّق الدلالي (S-TS-P9).

### 6. كلا المحرّكين
- المفسّر: حالة Null + إعادة إرساء العوامل القائمة على `isNull`.
- **المترجم sadc**: استبدال تمثيل `Integer`+حارس بتمثيل null/Optional حقيقي في LLVM — **قصّة codegen مستقلّة** (لا «مخاطرة تُرصد»).

## خريطة التنفيذ (ربط بالقصص)

| المرحلة | القصّة | المحتوى |
|---------|--------|---------|
| 1. الأساس | **S-TS-P1** | `SadTypeKind::Null` + حالة `Value` + `isNull` + `نوع()`→«عدم» + **إعادة إرساء العوامل القائمة على isNull** |
| 2. الاختياري | **S-TS-P4** | **تحليل لاحقة `رقم?` في `parseType` + بناء `OptionalTypeNode` (ميت حاليًّا)** + قواعد الإسناد (تصحيح `isAssignableTo` Void→Null) + فضّ غموض الثلاثي |
| 3. العوامل | **S-TS-P8** | تسجيل `?.`/`??` في `operators.yaml` (SoT) + `!!` الجديد + `؟` العربي (مهمّة lexer) + فضّ الغموض |
| 4. الفرض | **S-TS-P9** | استنتاج `لاشيء`→Null + `areTypesCompatible` (لا تُمرِّر Null لغير اختياري) + خطأ كتالوج |
| 5. التضييق | **S-TS-P10** | تضييق التدفّق (smart narrowing) — بنية flow-analysis (فُصلت عن P9) |

> **حسم ملكية الدوال (نقد أميليا P9):** `SadType::isAssignableTo:1501` يملكها **P4** (قاعدة النوع Void→Null، تُستدعى من الدلالات+المفسّر). `TypeChecker::areTypesCompatible:244` + استنتاج `LITERAL_NULL:400` يملكها **P9** (بوّابة الدلالات). دالّتان مختلفتان — لا ازدواج.
>
> **تحليل أثر التوافق الخلفي (مُتحقَّق):** `رقم س = لاشيء` (صريح) = **0 موضع**؛ `متغير/هذا = لاشيء` (مُستنتَج) = **48 موضعًا** تهرب عبر `LITERAL_NULL→Unknown`. فالفرض الفعّال يتطلّب معالجة الاستنتاج (P9-AC1) مع حفظ توسعة `متغير` للـ48.

> أمان null **لا يكتمل** قبل P9. P1 يعطي «null متمايز»؛ P1+P4+P8+P9 يعطي «أمان».

## البدائل المرفوضة

- **null-قابل افتراضيًّا (Java/C#)**: «خطأ المليار دولار». مرفوض.
- **لا null إطلاقًا (Rust/Haskell)**: يكسر `لاشيء` القائم في SoT والعوامل المنفّذة (CW-24). مرفوض.
- **إصلاح لفظي** (تغيير «فراغ»→«عدم» في الاسم فقط): يكذب — لا يمنح null هويّة. مرفوض.
- **بناء عوامل جديدة بـ`؟` عربي من الصفر** (النسخة 1): تجاهل أن `?.`/`??` اللاتينية منفّذة عبر المحرّكين. مرفوض.

## النتائج

- **إيجابي**: يستثمر سطحًا منفّذًا أصلًا (عوامل + Optional + codegen)؛ الإصلاح يُرسيه على أساس صحيح بدل إعادة بنائه.
- **سلبي**: يَمَسّ نقاطًا حسّاسة (`isVoid`→`isNull` في مفسّر+مترجم؛ تصحيح `isAssignableTo`)؛ خطر انحدار في سلوك `?.`/`??` القائم.
- **مخاطرة موثّقة**: تمثيل المترجم (Integer-sentinel→Optional حقيقي) قصّة مستقلّة؛ التوافق الخلفي لفرض P9 يحتاج تحليل أثر على إسنادات `لاشيء` القائمة.
