# language-truth/grammar/ — مصدر الحقيقة لقواعد لغة ص (Grammar SoT)

> **الحالة:** مُؤسَّس (skeleton). الهيكل والمخطط والملف النموذجي جاهزة؛ بقية الطبقات قيد الاستخراج من المُحلِّل.
> **المخطط:** [`_schemas/grammar_production.schema.json`](../_schemas/grammar_production.schema.json)
> **الترميز:** [`_notation.yaml`](./_notation.yaml)

---

## المشكلة التي يحلّها هذا المجلد

`language-truth/` يكتلج **عناصر** اللغة (كلمات، عوامل، أنواع، قائمة تراكيب). لكنه — حتى الآن —
لا يملك **قواعد الإنتاج الصورية** (production rules) التي تصف كيف تتجمّع تلك العناصر في برنامج
صحيح: كيف يُحلَّل البرنامج، الجمل، التعبيرات بسلسلة أسبقيتها، البنيات الكائنية، الأنماط.

سبب الفجوة: **بُنيت اللغة قبل قرار توحيد مصدر الحقيقة**، فعاش وصف القواعد موزّعاً وضمنياً داخل
المُحلِّل المكتوب يدوياً (`shared/parser/`) وحقول `syntax:` السطرية غير الدقيقة في `*_constructs.yaml`.

هذا المجلد هو **المواصفة المعيارية الموحَّدة لقواعد لغة ص**.

---

## مبدأ التصميم: مواصفة + تتبُّع، لا توليد للمُحلِّل

المُحلِّل في لغة ص **مكتوب يدوياً** (recursive descent). هذا المجلد **لا يولّد المُحلِّل** — فذلك
إعادة كتابة ضخمة ومخاطرة. بدلاً من ذلك يقدّم:

1. **المواصفة المعيارية** — قواعد الإنتاج الكاملة، هي المرجِع عند أي خلاف.
2. **خريطة تتبُّع (`maps_to`)** — كل قاعدة تشير إلى ملف:دالة التحليل الفعلية، فيتحقّق CI من
   بقاء المواصفة والتنفيذ متزامنين (الدالة موجودة، ولاحقاً: فحص انجراف أعمق).
3. **روابط للمعجم (`references`)** — الطرفيات لا تُكرَّر هنا؛ تشير إلى `keywords.yaml`/`operators.yaml`.

> هكذا نعكس الواقع بصدق: المُحلِّل اليدوي هو مصدر الحقيقة التنفيذي اليوم، وهذا المجلد يصبح مصدر
> الحقيقة المعياري — والجسر `maps_to` يمنع تباعدهما (BF-08، CW-24).

---

## بنية الملفات

ترقيم البادئات يعكس ترتيب طبقات التحليل (CW-02) ويطابق بنية `shared/parser/src/`:

```
language-truth/grammar/
├── README.md            ← هذا الملف
├── _notation.yaml       ← الميتا-قواعد: ترميز EBNF، اصطلاحات التسمية، ربط الطرفيات
│
├── 00_program.yaml      ← ✅ نموذجي — البرنامج، التصريح، الجملة، الكتلة («نهاية»)
├── 10_statements.yaml   ← ⏳ if / while / for / match / try / return / break / continue / expr-stmt
├── 20_declarations.yaml ← ⏳ متغير/ثابت/ساكن/خارجي، تصريح دالة، المعاملات، الاستيراد/التصدير
├── 30_oop.yaml          ← ⏳ صنف/بنية/تعداد/سمة/نفّذ/وراثة/خاصية/عامل/قالب
├── 40_expressions.yaml  ← ⏳ سلسلة الأسبقية الكاملة (إسناد→ثلاثي→…→أوّلي) — تربط operators.yaml
├── 50_patterns.yaml     ← ⏳ أنماط المطابقة (حرفي/نطاق/بنية/ربط/قائمة/OR/شامل)
├── 60_advanced.yaml     ← ⏳ عقود/تزامن/ماكرو/امتداد/عمر/async/ffi/أجّل
└── 70_lexical.yaml      ← ⏳ القواعد المعجمية (معرّف/رقم/نص/f-string/تعليق) — جسر للمُحلِّل المعجمي
```

### خريطة كل ملف ← مصدر المُحلِّل (للتتبُّع)

| ملف القواعد | دوال/ملفات المُحلِّل المصدر |
|---|---|
| `00_program.yaml` | `core/parser_main.cpp` (`parseProgram`، `parseDeclaration`، `parseStatement`، `parseBlockStmt`) |
| `10_statements.yaml` | `statements/parser_statements.cpp`، `statements/parser_advanced.cpp` |
| `20_declarations.yaml` | `declarations/parser_declarations.cpp`، `declarations/parser_modules.cpp` |
| `30_oop.yaml` | `declarations/parser_oop.cpp` + `specs/oop/` |
| `40_expressions.yaml` | `core/parser_expressions.cpp` (تربط أسبقية `operators.yaml`) |
| `50_patterns.yaml` | `specs/patterns/` |
| `60_advanced.yaml` | `specs/{contracts,async,meta,ffi,flow}/` |
| `70_lexical.yaml` | `shared/lexer/` (جسر — الطرفيات) |

---

## شكل قاعدة الإنتاج (production)

كل قاعدة تُكتب بصيغتين متطابقتين دلالياً (انظر [`00_program.yaml`](./00_program.yaml)):

- **`ebnf`** — نص مقروء بشريّاً للتوثيق وللمخططات السككية (railroad).
- **`alternatives`** — تمثيل منظَّم آلياً (رموز: طرفي/لاطرفي/تكرار/اختياري/مجموعة/بديل) **هو المرجِع**.
- **`references`** — معرّفات الكلمات/العوامل المستهلَكة (لفحص التماسك).
- **`maps_to`** — ملف:دالة التحليل الفعلية (للتتبُّع).
- **`ast_node`** — عقدة AST المُنتَجة.
- مع `id`، `lhs` (اللاطرفي)، `description_ar/en`، `example`، `since`، `status`.

---

## العلاقة بملفات `*_constructs.yaml` القائمة

ليست تكراراً — بل طبقتان:

| | `*_constructs.yaml` (قائم) | `grammar/` (جديد) |
|---|---|---|
| الغرض | كتالوج ميزات موجّه للمستخدم/التوثيق | قواعد إنتاج معيارية موجّهة للمُحلِّل |
| الدقّة | `syntax:` سطر واحد غير صوري | قواعد كاملة منظَّمة وقابلة للتحقق |
| العلاقة | كل تركيب يشير لقواعده عبر `grammar_rules: [...]` | كل قاعدة قد تُشير للتركيب عبر `references.constructs` |

المسار طويل الأمد: حقول `syntax:` السطرية تُشتق من هذا المجلد بدل صيانتها يدوياً.

---

## فحص المطابقة: هل كل قاعدة مدعومة فعلاً في المفسر والمترجم؟

القرار المعماري (بدل توليد المُحلِّل): **نُبقي القواعد وصفاً، ونُثبت بالأدلة أن كل قاعدة في
YAML حقيقية وعاملة في المفسر والمترجم معاً.** لا قاعدة بلا برهان تنفيذي (GR-01).

### بنية الاختبارات + الربط

الاختبارات منظَّمة منطقة/قاعدة/فئة لتتوسّع لآلاف الملفات (كل قاعدة ≥ 20 اختباراً، والمتقدمة
المتداخلة 150–200+). التفاصيل الكاملة في
[`tests/behavior/rules_matrix/README.md`](../../tests/behavior/rules_matrix/README.md):

```
rules_matrix/<area>/<rule_id>/{basic,edge,negative}/NNN_*.ص   ← اختبارات قاعدة واحدة
rules_matrix/_interactions/<theme>/NNN_*.ص                    ← تداخل عبر قواعد (@rule متعدد)
```

الربط في ملف الاختبار فقط عبر وسم `@rule` (test → rule)، فلا قوائم في YAML تتباعد. الفئة
تُشتق من المسار آلياً. في القاعدة: `conformance.test_budget` يحدّد الحد الأدنى لكل فئة،
و`compiler_optional` يُعفي الميزات المفسِّر-فقط الموثَّقة.

### الفاحص الشامل — ثلاث بوّابات

[`scripts/codegen/check_grammar_conformance.py`](../../scripts/codegen/check_grammar_conformance.py):

| البوّابة | تتحقق من |
|---|---|
| **① التغطية** | كل قاعدة تستوفي `test_budget` لكل فئة (basic/edge/negative/interaction) |
| **② التماسك** | `@rule` يطابق مجلد القاعدة، يشير لقاعدة موجودة، ولا اختبار بلا وسم |
| **③ التنفيذ المزدوج** (`--run`) | كل اختبار يمرّ في المفسر **و** المترجم بنفس المخرج — يفوّض لـ`runner.py` |

```bash
python scripts/codegen/check_grammar_conformance.py          # ①②
python scripts/codegen/check_grammar_conformance.py --run    # ①②③ + أدلة
```

الأحكام: `dual_ok` (مطلقة) · `compiler_gap` (مفسِّر فقط بلا إعفاء) · `interp_only` (مُعفاة) ·
`broken` (فشل/تباعد في المفسر) · `no_tests` · `not_run`.

### الأدلة — أثر بناء مدمج في نظام الاختبارات

`--run` يفوّض النتائج لـ`runner.py` (المصدر الوحيد، `build/_dual_report.json`) ثم يكتب ملخصاً
مفهرساً بالقاعدة في **`build/_grammar_conformance.json`**. يُعامَل مثل تقرير `runner` تماماً
(داخل `build/` المتجاهَل في git) — لا يُتتبَّع ولا يُحرَّر يدوياً (GR-01). كما تُسجَّل المصفوفة في
نظام الاختبارات: `runner.py --level rules` / `--section قواعد_برنامج`.

## بقية المستهلِكين (Codegen / CI)

- **توثيق ومخططات سككية** من `ebnf`.
- **فحص انجراف في CI**: كل `maps_to.function` يجب أن توجد فعلاً في المُحلِّل؛ كل `references` صالح.
- **LSP/المنسّق**: طيّ الكتل ومطابقة «نهاية».
- التحقق من المخطط: ضدّ [`_schemas/grammar_production.schema.json`](../_schemas/grammar_production.schema.json).

---

## English Summary

`language-truth/grammar/` is the Single Source of Truth for the **formal grammar** (production
rules) of the Sad language — the layer missing from the rest of `language-truth/`, which only
catalogs lexical/feature data. Because the parser is hand-written (recursive descent), this
directory is a **normative specification plus a `maps_to` traceability map** to the actual parser
functions (not a parser generator). Each production is expressed both as human-readable `ebnf`
and a machine-structured `alternatives` form, with terminals referencing `keywords.yaml` /
`operators.yaml` rather than duplicating them. Files are layered (program → statements →
declarations → oop → expressions → patterns → advanced → lexical) to mirror `shared/parser/src/`.
