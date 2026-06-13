# rules_matrix/ — مصفوفة اختبارات مطابقة قواعد لغة ص

> تُثبت بالأدلة أن كل قاعدة إنتاج في [`language-truth/grammar/`](../../../language-truth/grammar/)
> حقيقية وعاملة في **المفسر والمترجم معاً**. تُدار عبر
> [`scripts/codegen/check_grammar_conformance.py`](../../../scripts/codegen/check_grammar_conformance.py).

---

## البنية (قابلة للتوسّع لآلاف الاختبارات)

```
rules_matrix/
├── <NN>_<area>/                  ← منطقة تطابق طبقة القواعد: 00_program، 10_statements، 40_expressions ...
│   └── <rule_id>/                ← اسم المجلد = معرّف القاعدة بالضبط (gr.stmt.if)
│       ├── basic/    NNN_*.ص     ← الأشكال الأساسية الصحيحة
│       ├── edge/     NNN_*.ص     ← حالات حدّية: فراغ، تعشيش عميق، حدود، تداخل بسيط
│       └── negative/ NNN_*.ص     ← خطأ متوقَّع (@expect_error)
└── _interactions/                ← تداخل عبر عدّة قواعد (لا تخصّ قاعدة واحدة)
    └── <theme>/      NNN_*.ص     ← مثل: شرط_متغير_نطاق
```

**الفئة تُشتق من المسار آلياً:** أي مسار تحت `_interactions/` = `interaction`؛ وإلا
اسم مجلد `basic`/`edge`/`negative`. هذا يجعل العدّ والميزانيات آليّين بلا وسوم إضافية.

> لماذا التقسيم؟ كل قاعدة تحتاج ~20 اختباراً على الأقل، والقواعد المتقدمة المتداخلة (مثل `إذا`
> مع تعريف المتغيرات والنطاقات) قد تحتاج 150–200+. الفئات تمنع الفوضى وتجعل التغطية قابلة للقياس.

---

## وسم كل اختبار

أول 30 سطراً تُقرأ كبيانات وصفية (يقرؤها `runner.py` و الفاحص):

```sad
# @rule: gr.program.block            ← القاعدة (إلزامي). للتداخل: قائمة بفواصل
# @description: وصف موجز
# @priority: P0                       ← يدمجه في مستويات runner
# @requires: اطبع_سطر
# @expected: موجب\nانتهى             ← المخرج المتوقع (يُقارَن مع المفسر والمترجم)
# @expect_error                       ← للاختبارات السلبية بدل @expected
```

**قاعدة التماسك:** داخل مجلد قاعدة، يجب أن يحوي `@rule` اسمَ المجلد نفسه. اختبارات
`_interactions/` تسرد كل القواعد التي تمسّها في `@rule` (تُحسَب فئة `interaction` لكلٍّ منها).

---

## الميزانية (كم اختباراً لكل فئة؟)

تُعلَن في القاعدة نفسها في YAML — لا في الاختبارات:

```yaml
# language-truth/grammar/10_statements.yaml
conformance:
  test_budget: { basic: 6, edge: 8, negative: 4, interaction: 5 }   # = 23 اختباراً
```

بوّابة التغطية تفشل ما لم تُستوفَ ميزانية **كل فئة**. القواعد `status: planned` مُعفاة مؤقتاً.

---

## التشغيل

```bash
# تغطية + تماسك (لا يحتاج بناء):
python scripts/codegen/check_grammar_conformance.py

# + التنفيذ المزدوج (مفسر ≡ مترجم) + توليد الأدلة:
python scripts/codegen/check_grammar_conformance.py --run

# عبر نظام الاختبارات القائم مباشرةً:
python tests/runner.py --level rules            # كل المصفوفة
python tests/runner.py --level rules.program    # طبقة البرنامج + التداخل
python tests/runner.py --section قواعد_برنامج
```

## الأدلة

`--run` يفوّض التنفيذ لـ`runner.py` (مصدر النتائج الوحيد، `build/_dual_report.json`)، ثم يكتب
ملخصاً مفهرساً بالقاعدة في **`build/_grammar_conformance.json`**. هذا **أثر بناء** يُعامَل مثل
تقرير runner تماماً (داخل `build/` المتجاهَل في git) — لا يُتتبَّع ولا يُحرَّر يدوياً (GR-01).

الأحكام لكل قاعدة: `dual_ok` (مطلقة في الاثنين) · `compiler_gap` (مفسِّر فقط بلا إعفاء) ·
`interp_only` (فجوة مترجم مُعفاة بـ`compiler_optional`) · `broken` (فشل/تباعد في المفسر) ·
`no_tests` · `not_run`.
