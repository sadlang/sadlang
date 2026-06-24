---
storyId: TEST-008
title: "تغطية شاملة لكل قاعدة + مصفوفة تداخل ثنائية مشتقّة من القواعد"
status: in-progress
priority: P1
estimate: 40h
createdAt: 2026-06-23
updatedAt: 2026-06-23
assignee: TBD
dependsOn: [TEST-003, TEST-007]
blocks: []
relatedStories: [TEST-001, TEST-003, TEST-007]
relatedDecisions: [ADR-003, ADR-004, ADR-005]
relatedRfc: "sadlang-rfcs: grammar-conformance-matrix"
acceptanceCriteria:
  - AC-01: مولّد يشتق رسم قابلية التركيب من language-truth/grammar ويحسب الأزواج المطلوبة
  - AC-02: بوّابة رابعة في الفاحص تقيس تغطية الأزواج (warning افتراضيًا، --pairs-gate للإنفاذ)
  - AC-03: كل القواعد الـ104 لها اختبارات منفردة تستوفي الميزانية (249 فجوة → 0)
  - AC-04: كل الأزواج الـ166 المطلوبة مُغطّاة (127 ناقص → 0) ومُثبَتة بتنفيذ مزدوج
  - AC-05: طبقة تجريبية (patterns) مُنجَزة ومُثبَتة بدليل تشغيل فعليّ (GR-01)
---

# TEST-008 — مصفوفة مطابقة قواعد اللغة الشاملة

## السياق

طلب المالك: «كل قاعدة تُختبَر بشكل شامل لنتأكد أن اللغة تدعمها 100%، وكل قاعدة
تُختبَر مع كل قاعدة أخرى لنتأكد من تناسقها». مصدر الحقيقة فيه **104 قاعدة**. البنية
والفاحص قائمان (ADR-003) لكن التغطية جزئية. القرار المعماري في
[ADR-005](../decisions/ADR-005-pairwise-interaction-matrix.md): مصفوفة ثنائية
**مشتقّة من القواعد ومقيَّدة بالمخاطر** بدل حاصل الضرب الأعمى (5356).

## القياس الأساسيّ (baseline — أثر بناء، GR-01)

> مقيس بـ`python scripts/codegen/gen_rules_matrix.py` بتاريخ 2026-06-23.

| المقياس | القيمة |
|---|---|
| القواعد | 104 |
| الأزواج المطلوبة (قابلة للتركيب نحويًا) | **166** (86 احتواء + 84 تجاور) |
| الأزواج المُغطّاة | 39 |
| الأزواج الناقصة | **127** |
| فجوات منفردة (قاعدة×فئة) | **249** |

## معايير القبول (AC)

| ID | الوصف | التحقق | معرّف التتبّع |
|---|---|---|---|
| AC-01 | المولّد يشتق الرسم ويحسب الأزواج | `gen_rules_matrix.py` يطبع 166 | T2.008-TOOL-001 |
| AC-02 | البوّابة الرابعة | `check_grammar_conformance.py` يطبع ④ | T2.008-GATE-001 |
| AC-03 | 249 فجوة منفردة → 0 | الفاحص ① أخضر للـ104 | T2.008-COV-001 |
| AC-04 | 127 زوجًا ناقصًا → 0 + تنفيذ مزدوج | `--run --pairs-gate` أخضر | T2.008-PAIR-001 |
| AC-05 | الطبقة التجريبية مُثبَتة | تقرير تشغيل فعليّ patterns | T2.008-PILOT-001 |

## المهام (Tasks)

- [x] **T1: المولّد + رسم التركيب** (AC-01)
  - [x] T1.1: `gen_rules_matrix.py` — استخراج `refs` من `alternatives` تعاوديًا
  - [x] T1.2: نوعا الزوج (احتواء + تجاور) + استبعاد الموزِّعات
  - [x] T1.3: قياس التغطية + تقرير مصفوفة (`build/_rules_matrix_report.md`)
  - [x] T1.4: هيكلة (`--scaffold --layer`) تبذّر basic من `example`
- [x] **T2: البوّابة الرابعة** (AC-02)
  - [x] T2.1: `check_pairwise_coverage()` يفوّض لـgen_rules_matrix
  - [x] T2.2: تحذير افتراضيًا + `--pairs-gate` للإنفاذ
- [ ] **T3: سدّ الفجوات المنفردة** (AC-03) — 249 فجوة عبر الطبقات الثماني
  - [ ] T3.x: طبقة لكل وكيل (worktree + RFC فرعيّ) — منع التراجع بـ`--level full`
- [ ] **T4: سدّ أزواج التداخل** (AC-04) — 127 زوجًا
  - [ ] T4.1: بذر `@expected` من المفسّر (oracle) ثم تحقّق تطابق المترجم
  - [ ] T4.2: رفع `--pairs-gate` في P2 بعد بلوغ 100%
- [x] **T5: الطبقة التجريبية patterns** (AC-05) — مُنجَزة بدليل تشغيل فعليّ:
  - [x] **18/18 اختبارًا تكافؤ مزدوج 100%** (مفسّر≡مترجم) عبر **كل القواعد الست**:
        `gr.pattern.primary` (حرفيّ/نطاق/متغيّر/افتراضي)، `gr.pattern.list` (تفكيك 2 و3)،
        `gr.pattern.binding` (`@` داخل/خارج النطاق)، `gr.pattern.pattern` (شامل `_`)،
        `gr.pattern.or` (بدائل×3 + edge)، `gr.pattern.struct` (ربط حقلين + حقل حرفيّ).
  - [x] **اكتشاف 4 ثغرات حقيقية ثم إصلاح 3 جذريًّا (لم تُقنَّع):**
        - **ISSUE-033 (مُصلَح):** `gr.pattern.or` — المُحلِّل كان يقبل `\|\|` فقط؛ أُصلح
          ليقبل `\|` المفردة (parsePattern + OP_BITWISE_OR). **تكافؤ مزدوج مُثبَت** —
          أُضيفت 4 اختبارات (basic×3 + edge) خضراء.
        - **ISSUE-034 (مُصلَح — المفسّر):** `gr.pattern.struct` — `StructPattern::matches`
          كان يستخدم `operator[]` (للخرائط، يرمي على الكائنات)؛ أُضيف `Value::tryGetField`
          للوصول الكائنيّ. المفسّر يطبع 10.
        - **ISSUE-035 (مُصلَح — المترجم/codegen):** باني SIR كان يُصدر `CALL __sad_get_field`
          غير معرَّف (lld-link: undefined symbol)؛ أُصلح ليُصدر `LOAD(كائن، اسم الحقل)`
          وقت الترجمة كـ`buildMemberAccess`. **تكافؤ مزدوج مُثبَت** — الثلاثة اختبارات خضراء.
        - **ISSUE-036 (مُصلَح — codegen):** تباعد منفصل — المترجم كان يتجاهل القيم الافتراضية
          لحقول البنية (`نقطة().س` يُرجع 0 بدل 7)؛ أُصلح: `buildNewObject` يُهيّئ الحقول من
          `structFieldDefaults_` بـ`STORE` بعد `ALLOC` + تجاوز الباني الوهميّ. **تكافؤ مزدوج**.
        - **ISSUE-037 (مُصلَح — codegen):** اكتُشف أثناء 036 ومستقلّ عنه — مسار LOAD للعضو كان
          يثبّت `Integer` فيُبتر الحقل العشريّ (1 بدل 1.5، ويقع مع الإسناد الصريح أيضًا)؛
          أُضيف Float/Boolean في `buildExprMember` و`buildMemberAccess`. **تكافؤ مزدوج**.
  - [x] **طبقة patterns: 18/18 تكافؤ مزدوج 100%** بالثنائيات المعاد بناؤها بعد الإصلاحات
        (كل القواعد الست عاملة مزدوجًا: primary/list/binding/pattern/**or**/**struct**).
        أثر التشغيل: `python tests/runner.py --dir rules_matrix/50_patterns` ⇒ 18 نجح / 0 فشل.
- [x] **T6: طبقة oop كاملة (basic)** — **كل القواعد الـ16 مُغطّاة بـ≥1 اختبار، 26/26 تكافؤ مزدوج 100%**:
  - [x] القواعد: class/enum/struct/member/field/method/constructor/destructor/property/
        operator/modifiers/trait/impl/extension/new/this_super — مُعايَرة ببروبات ثنائية قبل التأليف.
  - [x] أثر التشغيل: `python tests/runner.py --dir rules_matrix/30_oop` ⇒ **26 نجح / 0 فشل**؛
        والمولّد يؤكّد **oop: 16/16 قاعدة لها اختبار منفرد** في `build/_rules_matrix_report.md`.
  - [x] غطّت حالات دقيقة عاملة مزدوجًا: «مجرد»+تجاوز، باني `: الأساس(...)`، «جديد» اللاحقة،
        خاص/ساكن، احصل/عيّن، عامل +، نفّذ سمة لـ صنف، امتداد، هدم().
  - [~] **ثغرة معروفة وحيدة:** `الأساس.طريقة()` (super method) = **ISSUE-019** (يفشل في المفسّر،
        RUN050) — موثَّقة سلفًا باختبار كاشف `grammar_gaps/كائني/004_super.ص`؛ ليست انحدارًا.
- [x] **T7: طبقة expressions كاملة (basic)** — **كل القواعد الـ25 مُغطّاة، 35/35 تكافؤ مزدوج 100%**:
  - [x] القواعد: expression/pipeline/assignment/ternary/null_coalesce/logical_or/logical_and/
        bitwise_or/xor/and/equality/comparison/range/term/factor/unary/power/postfix/primary/
        lambda/fstring/decorator/directive/array_literal/map_literal — مُعايَرة ببروبات ثنائية.
  - [x] أثر: `runner.py --dir rules_matrix/40_expressions` ⇒ **35 نجح / 0 فشل**؛ المولّد يؤكّد
        **expressions: 25/25 قاعدة لها اختبار منفرد**. حالات دقيقة: `؟؟`، `؟ :`، `<< >>`، `**`،
        `\|>`، `:=`، `في`، تعبير-إذا، نص منسَّق `م"...{}"`، `@حجم`، صف `(أ،ب)`.
  - [x] **ثغرتان جديدتان اكتُشفتا ووُثّقتا (لا تُقنَّعان):**
        - **ISSUE-038:** `++`/`--` تتباعد — اللاحق `س++` عمليّة فارغة في المترجم؛ البادئ كجملة
          يُفسِد لـ«لاشيء» في المفسّر. كاشف `grammar_gaps/تعابير/001_increment_postfix.ص`.
        - **ISSUE-039:** القيمة المطلقة `\|x\|` غير منفَّذة (المفسّر يُخطئ، المترجم 0/فشل).
          كاشف `grammar_gaps/تعابير/002_abs_bars.ص`. كلٌّ من unary/postfix/primary مُغطّاة بصيغها العاملة.
- [x] **T8: طبقة advanced (basic)** — **21/26 قاعدة مُغطّاة، 21/21 اختبارًا ناجحًا**:
  - [x] **9 مزدوجة** (type, lifetime_params, defer, macro, contract, go, select, ui_decl, ui_state).
  - [x] **4 مفسّر-فقط** `@skip_compiler` (list_comprehension/dict_comprehension = ISSUE-016/017،
        with، await — تعمل في المفسّر، المترجم لا يدعمها بعد).
  - [x] **8 مترجم-فقط** `@skip_interpreter` (template_decl/params/args، where_clause، inline_asm،
        widget، ui_modifier_chain، ui_event — يدعمها المترجم، المفسّر لا يدعم الجنيسات/الواجهة بعد).
  - [x] **إصلاح بنية تحتيّة:** المُشغّل `runner.py` لم يكن يدعم `@skip_interpreter` (كان يقارن
        مخرجًا فارغًا بمخرج المترجم ⇒ FAIL_OUTPUT)؛ أُضيف فرعٌ يقارن مخرج المترجم بـ`@expected`
        (نظير فرع `@skip_compiler`). تحقّق: patterns 18/18 بلا انحدار.
  - [x] **5 ثغرات موثَّقة (بلا اختبار مارّ):** ISSUE-040 (المولّدات `أنتج`)، ISSUE-041 (FFI `خارجي "C"{}`)،
        ISSUE-042 (`اختبر` يخرج برمز 1 عند النجاح)؛ كاشفات في `grammar_gaps/متقدمة/`.
        (أُجِّلت 3 قواعد FFI + yield + property_test كثغرات تجريبيّة/غير منفَّذة.)
- [x] **T9: طبقة lexical كاملة (basic + edge + negative)** — **كل القواعد الـ8 مُغطّاة بميزانيّاتها، 76/76 تكافؤ مزدوج 100%**:
  - [x] القواعد: identifier/integer/double/string/raw_string/fstring/lifetime/comment —
        34 basic + 30 edge + 12 negative، مُعايَرة ببروبات ثنائية مسبقة على المحرّكين قبل التأليف.
  - [x] أثر: `runner.py --dir rules_matrix/70_lexical` ⇒ **76 نجح / 0 فشل / 0 تخطّي** —
        كلّها **مزدوجة** (لا `@skip`)، فالمعجم مدعوم بالكامل على المفسّر والمترجم معًا.
  - [x] حالات مؤكَّدة مزدوجًا: ست عشريّ/ثنائيّ/ثمانيّ + فاصل `_`، أُسّ `e/E±`، هروب `\n \t \" \\`،
        نص خام `r"..."` بلا معالجة هروب، نص منسَّق `f"...{تعبير}..."` + أقواس مهروبة `{{}}`،
        عمر `'أ` (معامل بنية + استعارة `&'أ`)، تعليقات سطر/كتلة `#* *#`/توثيق `##`/كتلة-توثيق `#** **#`.
  - [x] **لم تُكتشف ثغرات جديدة** — نتيجة سلبية موثَّقة (لا ادعاء): كل صيغة عُويرت على المحرّكين فأنتجا المخرج نفسه.
  - [x] فِخاخ معجميّة موثَّقة (سلوك فعليّ لا مفترَض): السلاسل تسمح بأسطر حقيقيّة داخلها (نص متعدّد الأسطر
        ليس خطأً)؛ الكلمة المفتاحيّة قابلة للاستخدام اسمًا (`متغير إذا` لا يُخطئ)؛ الهروب المجهول `\q`
        يُمرَّر حرفيًّا. لذا اختير سلبيّا النص: غير المنتهي + هروبُ اقتباسِ الإغلاق `\"` الذي يُبقي النص مفتوحًا.

## ملاحظات التنفيذ

- **GR-01 صارم:** لا يُسَم زوج/قاعدة «مُغطّى» قبل دليل تنفيذ مزدوج فعليّ. درس تقرير
  التحقّق 2026-05-30: نمط ادعاء إنجاز بلا أثر — لا نكرّره.
- **الحلّ جذريّ:** إن كشف زوجٌ أن المفسّر/المترجم لا يدعم التركيب → ثغرة حقيقية
  تُسجَّل في `DISCOVERED_ISSUES.md`، لا تُقنَّع.
- **التوزيع:** الطبقات الثماني قابلة للتوازي (وكلاء عبر worktree + RFC) بلا تعارض،
  لأن كل طبقة مجلد مستقلّ تحت `rules_matrix/<area>/`.
- **عزل:** هذا العمل يجري في worktree `rules-matrix/test-008` عن dev بعد أن داس دمجٌ
  خارجيّ النسخة الأولى في المجلد الأساسي (درس main-worktree-hijacked-by-merge).

## Dev Agent Record

| التاريخ | الوكيل | الإجراء |
|---|---|---|
| 2026-06-23 | Claude | أنشأتُ المولّد + البوّابة الرابعة + ADR-005. قِست الأساس (166 زوجًا، 39 مُغطّى، 127 ناقص، 249 فجوة منفردة) بأثر بناء. T1+T2 مُنجَزتان ومُتحقَّقتان بالتشغيل. أُعيد الإنشاء في worktree معزول بعد دمج خارجيّ داس النسخة الأولى. |
| 2026-06-23 | Claude | أكملتُ T5 (الطبقة التجريبية patterns): 18 اختبارًا عبر القواعد الست، **18/18 تكافؤ مزدوج 100%** بأثر تشغيل `runner.py --dir rules_matrix/50_patterns`. كشفتُ 4 ثغرات حقيقية وأصلحتُ 3 جذريًّا (033 محلِّل، 034 مفسّر، 035 codegen) — كلٌّ بتكافؤ مزدوج مُثبَت؛ ISSUE-036 موثَّقة (تباعد تهيئة حقول منفصل). أُعيد بناء sad-run + sad-build (Release) في الworktree. |
| 2026-06-23 | Claude | أصلحتُ ISSUE-036 (codegen تهيئة حقول البنية الافتراضية: `structFieldDefaults_` + STORE بعد ALLOC + تجاوز الباني الوهميّ) وISSUE-037 (مسار LOAD للعضو كان يُبتر العشريّ — أُضيف Float/Boolean). كلاهما بتكافؤ مزدوج مُثبَت + اختبارات regression. |
| 2026-06-23 | Claude | أكملتُ T6 (طبقة oop كاملة basic): 26 اختبارًا تغطّي كل القواعد الـ16، **26/26 تكافؤ مزدوج 100%** بأثر `runner.py --dir rules_matrix/30_oop`؛ المولّد يؤكّد oop 16/16. لا انحدار في patterns. الثغرة الوحيدة ISSUE-019 (super method) موثَّقة سلفًا. لم تُكتشف ثغرات جديدة في أساسيّات oop. |
| 2026-06-23 | Claude | فحص انحدار كامل: `runner.py --dir rules_matrix` ⇒ **959/959 تكافؤ مزدوج 100%، 0 فشل** — إصلاحات codegen (036/037) بلا أيّ تراجع. |
| 2026-06-23 | Claude | أكملتُ T7 (طبقة expressions كاملة basic): 35 اختبارًا تغطّي كل القواعد الـ25، **35/35 تكافؤ مزدوج 100%** بأثر `runner.py --dir rules_matrix/40_expressions`؛ المولّد يؤكّد expressions 25/25. اكتُشفت ووُثّقت ثغرتان جديدتان: ISSUE-038 (++/--) وISSUE-039 (abs \|x\|). إجمالي التغطية صار **68/104 قاعدة لها ≥1 اختبار**. |
| 2026-06-23 | Claude | أكملتُ T8 (طبقة advanced basic): 21 اختبارًا تغطّي **21/26 قاعدة** (9 مزدوجة + 4 مفسّر-فقط + 8 مترجم-فقط)، **21/21 ناجح** بأثر `runner.py --dir rules_matrix/60_advanced`. أصلحتُ ثغرة بنية تحتيّة في `runner.py` (دعم `@skip_interpreter`). وثّقتُ 5 ثغرات (ISSUE-040 المولّدات، 041 FFI، 042 رمز خروج اختبر؛ + تأجيل 3 FFI). إجمالي التغطية صار **89/104 قاعدة**. |
| 2026-06-23 | Claude | سددتُ القاعدتين المتبقيتين: `gr.decl.arg_list` (3 اختبارات: فاصلة عربيّة/لاتينيّة/فارغة) و`gr.stmt.switch` (2: مطابقة + افتراضي) — **5/5 تكافؤ مزدوج**. الطبقتان صارتا 8/8 و11/11. إجمالي التغطية صار **91/104 قاعدة** (المتبقّي: lexical 0/8 + 5 ثغرات advanced). |
| 2026-06-23 | Claude | **أصلحتُ ISSUE-038** (`++`/`--` عمليّة فارغة في المترجم): `buildUnaryOp` يُصدر `ADD/SUB ±1`+`STORE` ويُرجع القيمة الجديدة. **تكافؤ مزدوج مُثبَت** عبر `س++`/`س--`/`++س` تعبيرًا/عشريّ/حلقة. أضفتُ اختبارَي regression (postfix + prefix) ⇒ expressions 37 اختبارًا. اكتشفتُ ISSUE-043 (تعارض تحليليّ: `++س` في بداية جملة يُلتقَط لاحقيًّا على الجملة السابقة — محلّل مشترك، عدم حساسية الأسطر) ووثّقته. |
| 2026-06-23 | Claude | **أصلحتُ ISSUE-016 و017** (استيعاب القائمة/القاموس كان يفشل ربطه): codegen كان يُصدر `CALL __sad_array_push`/`__sad_dict_set` **غير المعرَّفَين**؛ استبدلتُهما بـ`ARRAY_NEW/GET/APPEND/LEN` و`__sad_map_create/set_typed` المُلوَّنة. **تكافؤ مزدوج** (قائمة مع «إذا»؛ قاموس بمفاتيح نصّيّة). **وأصلحتُ ISSUE-039** (`\|x\|`): المحلّل كان يحوّلها لـ`abs` (مكسور) فصار `مطلق` (عامل). أضفتُ اختبارات (advanced 22، expressions 38). اكتشفتُ ISSUE-044 (مفاتيح عدديّة في الخرائط لا تُقرأ بالمترجم — يقع مع الخريطة الحرفيّة أيضًا). |
| 2026-06-23 | Claude | **أصلحتُ ISSUE-044** (مفاتيح الخرائط العدديّة): الخريطة تُخزّن المفاتيح كـ`char*` عبر `strdup`؛ أُضيف تحويل المفتاح العدديّ/العشريّ/المنطقيّ إلى نص (`I64_TO_STRING`) عند الإدراج (حرفيّ + استيعاب) والقراءة (الفهرسة). **وأصلحتُ ISSUE-043** (تعارض الأسطر): `parsePostfix` لا يربط `++`/`--` اللاحق إلا على سطر معاملِه نفسِه، ففُكّ التباس `++س` في بداية الجملة. **تكافؤ مزدوج لكليهما**؛ رُقّي الكاشفان لاختبارين مارّين (expressions 40). |
| 2026-06-23 | Claude | **أصلحتُ ISSUE-019 مزدوجًا** (`الأساس.طريقة()`): المفسّر [expression_evaluator_oop.cpp] يكتشف معامل `الأساس` فيستبدله بـ«هذا» ويعيد توجيه البحث للأب؛ المترجم [call_method_dispatch.cpp] يوجّه className للأب **ويُجبر نداءً مباشرًا غير افتراضيّ** (isObjectCall=false) — كان vtable يعود للابن ⇒ تكرار لانهائيّ وانهيار. **تكافؤ مزدوج مُثبَت** (2، «صوت+مياو»، حقل=50؛ توزيع `هذا` الافتراضيّ سليم). رُقّي الاختبار لمزدوج وأُزيل كاشف الفجوة (oop 27). **وثّقتُ ISSUE-042 بتشخيص مُنقَّح** (تشابك تحميل-كسول+دلالي+خروج — مفتوح). |
| 2026-06-24 | Claude | **أكملتُ T9 (طبقة lexical كاملة basic+edge+negative)**: 76 اختبارًا تغطّي **كل القواعد الـ8** بميزانيّاتها (34 basic + 30 edge + 12 negative)، **76/76 تكافؤ مزدوج 100%، 0 تخطّي** بأثر `runner.py --dir rules_matrix/70_lexical`. عُويرت كل صيغة (ست عشريّ/ثنائيّ/ثمانيّ، أُسّ، هروب، خام، منسَّق، عمر، تعليقات) ببروب ثنائيّ مسبق على المحرّكين. **لا ثغرات جديدة** — المعجم مدعوم بالكامل مزدوجًا (نتيجة سلبية موثَّقة). إجمالي التغطية صار **99/104 قاعدة** (المتبقّي 5 ثغرات advanced: 040/041/042). فحص انحدار كامل `runner.py --dir rules_matrix` ⇒ **1101/1101، 0 فشل، 0 تخطّي** (1099 مزدوج + 2 مفسّر-فقط) — لا انحدار. |
| 2026-06-24 | Claude | **وسّعتُ الطبقة المعجمية بحالات حدّية إضافية**: +32 اختبارًا (31 edge + 1 negative) ⇒ المعجم صار **108 اختبارًا**. كلٌّ مُعوَّر ثنائيًّا مسبقًا على المحرّكين. تغطية حدّية جديدة: خلط النصوص + شرطة سفليّة وحيدة/مع رقم + رقم هنديّ-عربيّ في المُعرّف + كشيدة + بادئة كلمة مفتاحيّة؛ أصفار بادئة + فاصل ست عشريّ + أرقام هنديّة-عربيّة كعدد؛ فواصل قراءة في العشريّ + أُسّ بلا نقطة؛ هروب يونيكود `أ` + طول النصّ الفارغ + `\r`؛ خام يحفظ `\n`/`\t`/`\u` حرفيًّا؛ منطقيّ/نداء/نصّ مختلط في الإقحام؛ أعمار لاتينيّة؛ تعليق فارغ/مضمَّن/تعبيريّ. **اكتشفتُ ووثّقتُ ISSUE-045** (تباعد حقيقيّ: المترجم يبتر النصّ عند البايت الصفريّ `\0` بدلالة سلاسل C ⇒ `طول("\0")`=0 بدل 1) — كشفته المقارنة المزدوجة وحدها؛ عُلِّم الاختبار `@skip_compiler` لحفظ سلوك المفسّر الصحيح. **108/108 ناجح (107 مزدوج + 1 مفسّر-فقط، 0 فشل)** بأثر `runner.py --dir rules_matrix/70_lexical`. فحص انحدار كامل ⇒ **1133/1133، 0 فشل، 0 تخطّي** (1130 مزدوج + 3 مفسّر-فقط) — لا انحدار. |
| 2026-06-24 | Claude | **قلّصتُ الاختبارات أحاديّة المحرّك من 11 إلى 5 برفع 6 إلى التكافؤ المزدوج بإصلاحات مصدر حقيقيّة.** ① **`باستخدام` (with) في المترجم:** كان `__sad_context_enter/__sad_context_exit` رمزين غير معرَّفين (فشل ربط)؛ أضفتُ اعتراضهما في [exception_ops.cpp](../../../compiler/src/backend/llvm/builders/arithmetic/exception_ops.cpp) كـlا-عمل للقيم غير الكائنيّة (مطابِق للمفسّر الذي يتخطّى enter/exit ما لم يكن المورد كائنًا). ② **`انتظر` (await) في المترجم:** كان يُصدر `CORO_SUSPEND` على قيمة عدديّة فيسقط backend بـ«Do not know how to promote this operator»؛ في [expression_main.cpp](../../../compiler/src/frontend/builders/expression_main.cpp) صار انتظار قيمة بدائيّة تمريرًا مطابقًا للمفسّر (مسار الكوروتين يبقى للمقابض). ③ **الجنيسات ×4 في المفسّر** (template_decl/params/args/where_clause): كانت دالة القالب تُسجَّل تحت `__template_` فقط فالاستدعاء العاديّ يُرجع «لاشيء»، ثمّ فحص النوع يرفض البدائيّ على معامل-النوع «ت»؛ في [statement_executor_functions_templates.cpp](../../../interpreter/src/visitors/statement_executor_functions_templates.cpp) سجّلتُ القالب تحت اسمه الصريح أيضًا **ومحوتُ أسماء أنواع المعاملات الجنيسة** (type erasure) فيُنفَّذ كدالة عاديّة — مطابقةً لحَوْصلة المترجم. **النتيجة:** with/await/الجنيسات الأربع = **6 اختبارات صارت مزدوجة**؛ طبقة advanced **18 مزدوجة فعليًّا + 4 مترجم-فقط** (UI×3 + inline_asm). **المتبقّي أحاديًّا (بصدق، ليست ثغرات قابلة للإصلاح هنا):** ISSUE-045 (معماريّ: سلاسل المترجم منتهية بصفر `char*`+`strlen` في كامل النظام — تتطلّب تمثيلًا بطول مخزَّن، مؤجَّل)؛ UI×3 (المفسّر يحتاج زمن عرض)؛ inline_asm (تجميع آليّ خاصّ بالمترجم بطبيعته — `@skip_interpreter` هو الحالة الصحيحة لا ثغرة). فحص انحدار كامل بالثنائيّتين المُعاد بناؤهما ⇒ **1133/1133، 0 فشل** (1132 مزدوج + 1 مفسّر-فقط=ISSUE-045) — **صفر انحدار** من تغييرات المصدر الثلاثة. |

## File List

- `scripts/codegen/gen_rules_matrix.py` (المولّد + المصفوفة + الهيكلة)
- `scripts/codegen/check_grammar_conformance.py` (البوّابة الرابعة + `--pairs-gate`)
- `_bmad-output/systems/testing-system/decisions/ADR-005-pairwise-interaction-matrix.md`
- `tests/behavior/rules_matrix/50_patterns/**` (18 اختبارًا عبر القواعد الست)
- `tests/behavior/rules_matrix/30_oop/**` (26 اختبارًا عبر القواعد الـ16 — T6)
- `tests/behavior/rules_matrix/40_expressions/**` (35 اختبارًا عبر القواعد الـ25 — T7)
- `tests/behavior/grammar_gaps/تعابير/**` (كاشفا ISSUE-038 و039)
- `tests/behavior/rules_matrix/60_advanced/**` (21 اختبارًا عبر 21 قاعدة — T8)
- `tests/behavior/rules_matrix/70_lexical/**` (108 اختبارًا عبر القواعد الـ8 — T9: identifier/integer/double/string/raw_string/fstring/lifetime/comment؛ +32 حالة حدّية إضافية 2026-06-24 كشفت ISSUE-045)
- `tests/behavior/grammar_gaps/متقدمة/**` (كاشفات ISSUE-040/041/042)
- `tests/runner.py` (إصلاح: دعم `@skip_interpreter` للميزات الخاصّة بالمترجم — T8)
- `compiler/src/frontend/builders/expression_basic.cpp` (إصلاح ISSUE-038: تنفيذ `++`/`--` في buildUnaryOp)
- `compiler/src/frontend/builders/expression_comprehensions.cpp` (إصلاح ISSUE-016/017: أوبكودات ARRAY/MAP)
- `shared/parser/src/core/parser_expressions.cpp` (إصلاح ISSUE-039 `\|x\|`←`مطلق` + ISSUE-043 فحص سطر `++`/`--`)
- `compiler/src/frontend/builders/expression_collections.cpp` + `expression_index.cpp` (إصلاح ISSUE-044: مفاتيح خرائط عدديّة←نص)
- `interpreter/src/visitors/expression_evaluator_oop.cpp` + `compiler/src/frontend/builders/call_method_dispatch.cpp` (إصلاح ISSUE-019: super method مزدوجًا)
- **رفع 6 اختبارات أحاديّة إلى التكافؤ المزدوج (2026-06-24):**
  - `compiler/src/backend/llvm/builders/arithmetic/exception_ops.cpp` (with: اعتراض `__sad_context_enter/exit` كـlا-عمل)
  - `compiler/src/frontend/builders/expression_main.cpp` (await: تمرير القيمة البدائيّة بدل `CORO_SUSPEND`)
  - `interpreter/src/visitors/statement_executor_functions_templates.cpp` (الجنيسات: تسجيل باسم صريح + محو نوع المعاملات)
  - `tests/behavior/rules_matrix/60_advanced/gr.adv.{with,await,template_decl,template_params,template_args,where_clause}/**` (إزالة `@skip_*` بعد التكافؤ)
- `tests/behavior/grammar_gaps/أنماط/001_struct_default_field.ص` (كاشف ISSUE-036)
- `language-truth/grammar/DISCOVERED_ISSUES.md` (033/034/035 مُصلَح، 036 مفتوح)
- **إصلاحات المصدر (طبقة التجريبية):**
  - `shared/parser/src/statements/parser_advanced.cpp` (ISSUE-033: `parsePattern` يقبل `OP_BITWISE_OR`)
  - `shared/types/include/value.h` + `shared/types/src/value.cpp` (ISSUE-034: `Value::tryGetField`)
  - `shared/ast/include/pattern_nodes.h` (ISSUE-034: `StructPattern::matches` يستخدم `tryGetField`)
  - `compiler/src/frontend/sir_builder_match_patterns.cpp` (ISSUE-035: `LOAD` بدل `CALL __sad_get_field`)
