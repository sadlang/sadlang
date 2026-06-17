# قواعد لغة ص — نظام موحَّد المصدر (Single Source of Truth)

> **الكود هو الحقيقة، وYAML هو المصدر الموحَّد للمواصفة.**
> القواعد النحويّة مُستخرَجة من المحلل (`shared/parser/`) ومُدوَّنة كقواعد إنتاج في
> **`language-truth/grammar/`**، ثم يُولَّد منها هذا التوثيق آليًّا.

---

## 1) خطّ المصدر الموحَّد (Pipeline)

```
shared/parser/  ──(استخراج: الكود هو الحقيقة)──►  language-truth/grammar/*.yaml   (المصدر الموحَّد)
                                                          │  scripts/codegen/gen_parser_grammar_docs.py
                                                          ▼
                                   docs/parser_rule/_generated/*.md   (مُولَّد — لا يُحرَّر يدويًّا)
```

- **التوثيق المُولَّد (المعتمَد):** [`_generated/INDEX.md`](_generated/INDEX.md) — **104 قاعدة عبر 8 طبقات**،
  لكل قاعدة: BNF + تفصيل البدائل + المسار إلى المحلل (maps_to) ⇒ AST + مخطّط مسار الدوال + مخطّط البنية + روابط «يستدعي/مُستدعى».
- **المخطّط:** [`language-truth/_schemas/grammar_production.schema.json`](../../language-truth/_schemas/grammar_production.schema.json).
- **الترميز:** [`language-truth/grammar/_notation.yaml`](../../language-truth/grammar/_notation.yaml).

## 2) الطبقات

| الطبقة | YAML | معرّفات | المحلل المصدر |
|--------|------|---------|----------------|
| النواة | `00_program.yaml` | `gr.program.*` | `core/parser_main.cpp` |
| الجمل | `10_statements.yaml` | `gr.stmt.*` | `statements/parser_statements.cpp` |
| التصريحات | `20_declarations.yaml` | `gr.decl.*` | `declarations/parser_declarations.cpp`, `parser_modules.cpp` |
| الكائنية | `30_oop.yaml` | `gr.oop.*` | `declarations/parser_oop.cpp` |
| التعابير | `40_expressions.yaml` | `gr.expr.*` | `core/parser_expressions.cpp` |
| الأنماط | `50_patterns.yaml` | `gr.pattern.*` | `statements/parser_advanced.cpp`, `specs/patterns/` |
| المتقدّمة | `60_advanced.yaml` | `gr.adv.*` | `core/parser_helpers.cpp`, `specs/{ffi,...}`, `ui/parser_ui.cpp` |
| المعجمية | `70_lexical.yaml` | `gr.lex.*` | `shared/lexer/src/lexer_core.cpp` |

## 3) الأوامر

```bash
python scripts/codegen/gen_parser_grammar_docs.py            # توليد التوثيق
python scripts/codegen/gen_parser_grammar_docs.py --check    # تحقّق CI (محدَّث؟)
python scripts/codegen/check_grammar_conformance.py          # مطابقة (تغطية + تماسك)
```

## 4) نظام الترقيم

- **المعرّف الموحَّد (المرجِع):** `gr.<area>.<name>` — فريد، في YAML، يُفحَص تماسكه في CI.
- **الرقم التسلسليّ:** `ق-NNN` (مُولَّد للعرض فقط).

## 5) ملاحظة دمج (2026-06-17)

دُمجت طبقات الاستخراج (30/40/50/60/70 + إضافات) مع نسخة فرع `dev` للطبقات 00/10/20
(مؤلَّفة بأسلوب ISSUE-tracking) دون مسّها — تستخدم طبقاتي معرّفات `dev` (`gr.decl.parameters`/
`gr.decl.extern`) لتفادي التكرار. الإجمالي 104 قاعدة، كلها صالحة بالمخطّط ومتماسكة.
