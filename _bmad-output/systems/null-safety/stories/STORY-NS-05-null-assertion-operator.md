---
id: NS-05
title: "عامل التأكيد (مؤكَّد): T؟ → T + خطأ كتالوج"
type: implementation-story
milestone: NS-M2-Operators
status: in-progress
estimate: M
depends_on: [NS-01, NS-02, NS-03, NS-04]
governed_by:
  - ../planning/ROADMAP.md
  - ../decisions/ADR-NS-001-flow-analysis-scope-and-strictness.md
  - ../decisions/ADR-NS-002-arabic-canonical-null-operators.md
date: 2026-06-21
---

# NS-05 — عامل التأكيد `مؤكَّد` (تأكيد عدم الفراغ)

> عامل **لاحقيّ كلمة** يحوّل `T؟ → T`: يَعِد المبرمج بأنّ القيمة حاضرة، فإن كانت
> عدمًا وقت التشغيل طُرِح خطأ كتالوج `RUN_NULL_ASSERTION`.

## القصة
بصفتي **مستخدم لغة ص**، أريد **عامل تأكيد يفكّ الاختياريّ `T؟` إلى `T`** حين أكون
**واثقًا أنّ القيمة حاضرة**، حتى **أتجنّب `؟؟`/الفحص المطوّل وأحصل على خطأ واضح إن
خُلِف الوعد**.

## قرار الرمز (محسوم)
- **عامل كلمة لاحقيّ** لا رمز ASCII (اتّساقًا مع روح ADR-NS-002 ووضوح العربية).
- **صيغتان مترادفتان** لنفس الرمز المعجميّ `OP_NULL_ASSERT`:
  - `مؤكَّد` (مذكّر، الصيغة القانونيّة) — `الرقم مؤكَّد`
  - `مؤكَّدة` (مؤنّث، مرادف عبر `aliases`) — `القيمة مؤكَّدة`
  - تُطبَّعان لنفس العامل تمامًا كما `؟؟`، فيكتب المبرمج ما يطابق جنس الاسم فيُقرأ
    نحويًّا سليمًا.
- **سياقيّ لا محجوز** (`emittedByLexer: false`، فئة `contextual`): `مؤكد`/`مؤكدة`
  كلمتان عربيّتان شائعتان، فاحتجازهما عامليْن دائمين (كـ`ليس`) يكسر كودًا قائمًا.
  المحلّل المعجميّ يُصدرهما `IDENTIFIER`، ويميّزهما `parsePostfix` عبر
  `matchContextual(OP_NULL_ASSERT)` فقط حين تَليان تعبيرًا لاحقيًّا. هكذا تبقيان
  صالحتين كأسماء/أعضاء تعداد/ماكرو. (الوصول لعضو `.مؤكد` يُعالَج في فرع `DOT`.)

## معايير القبول
- **AC1 — معجم:** `مؤكَّد`/`مؤكَّدة` ⇒ توكِن واحد `OP_NULL_ASSERT` (عبر `aliases`). ✅
- **AC2 — تحليل:** لاحقيّ في `parsePostfix` ⇒ `UnaryExpr(OP_NULL_ASSERT, operand)`. ✅
- **AC3 — مفسّر (سعيد):** قيمة حاضرة ⇒ تُمرَّر كما هي (`T؟ → T`). ✅
- **AC4 — مفسّر (عدم):** قيمة عدم ⇒ خطأ كتالوج `RUN_NULL_ASSERTION`. ✅
- **AC5 — مترجم (سعيد):** يفكّ القيمة الحاضرة بالنوع الداخليّ (سباكة Optional<T>). ✅
- **AC5ب — مترجم (عدم):** فخّ وقت تشغيل عبر `NULL_ASSERT` SIR ⇒ فحص الحارس ثمّ طباعة RUN056 + `exit(1)`. ✅
- **AC6 — كتالوج:** `RUN_NULL_ASSERTION` (RUN056) في `errors/runtime.yaml` + `error_codes.h`. ✅
- **AC7 — SoT:** `operators.yaml` (`op.null_assert`) + `keywords.yaml` (`KW-OPR-004`). ✅
- **AC8 — DoD:** اختبار `.ص` تكافؤ مزدوج للطريق السعيد. ✅ (`053`)

## المصدر في الكود
- `shared/lexer/include/token.h` — `OP_NULL_ASSERT` (بعد `QUESTION_QUESTION`).
- `shared/lexer/src/token.cpp` — اسم التوكِن في `tokenTypeToString`.
- `language-truth/keywords.yaml` — `KW-OPR-004` (`مؤكد` + alias `مؤكدة`) ⇒ توليد.
- `shared/parser/src/core/parser_expressions.cpp` — حالة `OP_NULL_ASSERT` في `parsePostfix`.
- `interpreter/src/visitors/expression_evaluator_calls.cpp::visitUnaryExpr` — فحص
  `isNull()` ⇒ `RUN_NULL_ASSERTION`؛ وإلّا تمرير القيمة.
- `compiler/src/frontend/builders/expression_basic.cpp::buildUnaryOp` — تمرير المعامل
  (مُحمَّل بالنوع الداخليّ) عند `OP_NULL_ASSERT`.
- `language-truth/errors/runtime.yaml` — `RUN_NULL_ASSERTION` (RUN056).

## المتبقّي (موثَّق لا مُخفى — راجع report-failures-dont-avoid)
- **فخّ المترجم وقت التشغيل عند العدم: مُنجَز** عبر تعليمة SIR جديدة `NULL_ASSERT`
  (`sir_types.h`) يلوّنها الخلفية (`arith_main.cpp::emitNullAssert`) كفحص حارس ⇒
  طباعة `printf` RUN056 + `exit(1)` (نمط `emitBoundsCheck`). **فارق دقيق عن المفسّر:**
  المفسّر يُبلِغ ثمّ **يتابع** بقيمة عدم (سلوك ErrorManager)، بينما المترجم **يُجهِض**
  (`exit(1)`)؛ كلاهما يمنع نتيجة صامتة خاطئة، لكنّ مخرجات حالة-العدم ليست متطابقة
  بايتيًّا ⇒ لا تُختبَر تكافؤًا مزدوجًا (يُختبَر المفسّر وحده).
- **تضييق المحلّل الساكن:** لم يُدمَج بعدُ وعيُ `NullSafetyAnalyzer` بأنّ `س مؤكَّد`
  يضمن عدم الفراغ (لا أثر عمليّ على التكافؤ؛ تحسين تشخيصيّ لاحق).

## الإنجاز (2026-06-21)
- ✅ AC1–AC3 + AC5–AC8: المعجم/المحلّل/المفسّر (سعيد)/المترجم (سعيد)/الكتالوج/SoT.
- ✅ AC4: خطأ `RUN_NULL_ASSERTION` في المفسّر عند العدم.
- **شاهد ناجح:** `053_optional_assert.ص` **تكافؤ مزدوج 100%**
  (A1:7 / A2:مرحبا / A3:9 / A4:3) — يغطّي المذكّر `مؤكد` والمؤنّث `مؤكدة`.

## مراجعة الكود (bmad-code-review، وكيل فرعي، 2026-06-21)
- **🔴 عيب حرِج وُجِد وأُصلِح:** الإصدار الأوّل سجّل `مؤكد`/`مؤكدة` في فئة `operators`
  (`emittedByLexer: true`) فاحتجزهما عامليْن دائمين، فكسر كودًا عربيًّا قائمًا
  (`ملفات_التدريب/52_نظام_حجز_فندقي.ص:25` عضو تعداد `مؤكد`، والوصول `وضع_الحجز.مؤكد`،
  وماكرو `مؤكد` في الأرشيف). الرنر لم يلتقطه لأنّ `ملفات_التدريب` خارج مجموعة الاختبار.
  **الإصلاح:** نقل العامل إلى فئة `contextual` + `matchContextual` في المحلّل (انظر «قرار
  الرمز» أعلاه). تُحقِّق: الملفّ 52 صار يتجاوز السطر 25 دون خطأ `مؤكد`، والميزة سليمة.
- **🟡 موثَّق سلفًا:** تباعد مسار العدم مفسّر/مترجم (المترجم يمرّر، لا فخّ) — انظر «المتبقّي».
- **🟢 سليم:** الأسبقيّة في `parsePostfix`، ترتيب RUN056 في enum مقابل YAML.
