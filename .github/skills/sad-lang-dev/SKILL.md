---
name: sad-lang-dev
description: 'تطوير لغة ص نفسها — العمل على كود C++ للمفسر والمترجم وأنظمتها الداخلية. Use when: developing the Sad language implementation (not writing .ص code), modifying the lexer/parser/AST/interpreter/compiler (sadc), adding or editing builtin functions, working with the error/diagnostics system, editing language-truth YAML (Single Source of Truth), running the codegen pipeline (scripts/codegen/gen_*.py), adding a new keyword/directive/type, building a new internal subsystem, regenerating generated C++ from YAML, understanding the layered architecture (Lexer→Parser→AST→SIR→LLVM), understanding the BMAD governance system (_bmad-output), understanding how language subsystems interlock (a builtin touches docs/YAML + errors + module/section + tests). Covers: error system (shared/errors + language-truth/errors), builtins system (interpreter/src/builtins + language-truth/builtins + codegen), documentation/YAML SoT system, interconnected-systems map, governance onboarding, strict dev workflow + Definition of Done, task delivery conditions + impact analysis + final checklist (which compiler/interpreter/tooling paths a change affects, how to confirm nothing was forgotten), extensibility — adding entirely new language subsystems.'
---

# مهارة مبرمج لغة ص (تطوير اللغة نفسها)

> **هذه المهارة لتطوير اللغة، وليست لكتابة الكود بها.**
> إن كنت تكتب برنامجاً بلغة ص (ملف `.ص`) فاستخدم مهارة `sad-lang-coding`.
> هذه المهارة تخصّ تعديل كود C++ للمفسر/المترجم وأنظمة اللغة الداخلية.

## متى تُستخدم هذه المهارة

- تعديل أي مكوّن أساسي: المحلل المعجمي، النحوي، AST، المفسر، المترجم `sadc`، الـ VM
- إضافة/تعديل **دالة مضمنة** أو **وحدة** في المكتبة القياسية
- العمل على **نظام الأخطاء والتشخيص** (إضافة رمز خطأ، تعديل رسالة، تلميح إصلاح)
- تعديل **مصدر الحقيقة YAML** (`language-truth/`) وإعادة توليد كود C++
- إضافة **كلمة مفتاحية / توجيه / نوع / نمط مطابقة** جديد
- بناء **نظام داخلي جديد بالكامل** (ميزة لغوية أو نظام أدوات)
- تشغيل خط أنابيب **توليد الكود** (`scripts/codegen/gen_*.py`)

## ⚡ القاعدة الذهبية: مصدر الحقيقة أولاً (SoT-First)

لغة ص تتبنّى معمارية **مدفوعة بالبيانات (data-driven)**. مجلد `language-truth/` هو
المصدر الموحَّد لكل بيانات اللغة (كلمات مفتاحية، دوال مضمنة، أنواع، عوامل، أخطاء، توجيهات).
**أي تغيير في بيانات اللغة يبدأ من YAML — وليس من كود C++ المُولَّد.**

```
language-truth/*.yaml  →  scripts/codegen/gen_*.py  →  shared/*/generated/*.{h,cpp}  →  بناء C++
        (المصدر)              (المُولِّد)                    (مُولَّد — لا يُحرَّر يدوياً)
```

> ⚠️ **قاعدة صارمة:** الملفات تحت `*/generated/` **مُولَّدة آلياً** — لا تُحرَّرها يدوياً أبداً.
> عدّل ملف YAML المصدر ثم أعد التوليد. تحرير المُولَّد يدوياً = خرق سيُمحى عند البناء التالي.
>
> 📌 **لكنها متتبَّعة في git** (ليست build-only) — بعد إعادة التوليد، **ضمِّن المُولَّد في الـ commit**
> مع YAML في نفس الدفعة (يجب أن يتطابقا). راجع [./references/workflow.md](./references/workflow.md).

## إجراء العمل الإلزامي (قبل أي تعديل)

0. **افهم التشابك أولاً:** لا يوجد تغيير «معزول». أي تعديل يَمَسّ عدة أنظمة متشابكة (توثيق/YAML،
   أخطاء، أقسام/وحدات، اختبارات، أحياناً حوكمة). اقرأ [./references/interconnected-systems.md](./references/interconnected-systems.md).
1. **هل المهمة تَمَسّ `_bmad-output/` أو ضمن ستوري محكوم؟** إن نعم → اتبع [./references/governance.md](./references/governance.md) (اقرأ السياسة + اكتب سطر الإقرار) أولاً.
2. **حدّد الطبقة** التي يقع فيها التغيير (راجع جدول المعمارية أدناه). أصلِح/أضِف في الطبقة الصحيحة فقط — يُمنع القفز بين الطبقات (CW-02، BF-10).
3. **هل التغيير مدفوع بالبيانات؟** إن كان كلمة/دالة/خطأ/نوع/عامل → ابدأ من `language-truth/` ثم أعد التوليد. لا تُعدّل `generated/` (لكن ضمّنه في الـ commit).
4. **اتبع قائمة المهام الجاهزة** لنوع مهمتك في [./references/playbooks.md](./references/playbooks.md) (دالة/خطأ/كلمة/إصلاح مترجم) — تدفّق مرقّم وكيفية تعديل الكود.
5. **اقرأ 3 ملفات مشابهة** في نفس المجلد قبل الكتابة لاتباع النمط (BF-25, CW-04).
6. **التزم بقواعد الكود الـ30 (CW-*)** وقواعد إصلاح الأخطاء الـ30 (BF-*)، واتبع [./references/workflow.md](./references/workflow.md) (التسلسل + معيار الإنجاز).
7. **اختبر في المفسر والمترجم معاً** عبر `runner.py` — إن عمل في `sad-run` وفشل في `sadc` فالمشكلة في SIR/LLVM (BF-08).
8. **قبل إعلان الإنجاز:** أجرِ تحليل الأثر واجتَز قائمة التسليم الموحّدة في [./references/delivery-checklist.md](./references/delivery-checklist.md). المهمة ليست «مُسلَّمة» حتى تمرّ كل بوّاباتها.

## شروط التسليم (متى تكون المهمة «منجَزة» فعلاً)

لا تُعلن الإنجاز قبل اجتياز **كل** بوّابة (التفصيل في [./references/delivery-checklist.md](./references/delivery-checklist.md)):

1. **تحليل الأثر مكتمل** — حدّدتَ كل مسار/نظام متأثّر (مفسر، مترجم SIR/LLVM، VM، وأدوات
   `language-truth`: LSP/المنسّق/الحزم) ولم تُهمل شيئاً بلا مبرّر.
2. **الاختبارات موجودة وتمرّ 100%** — `.ص` إيجابي+سلبي، و`runner.py --level P0` + القسم + `P1` خضراء، بلا تراجع.
3. **البناء نظيف** — `sad-run` (Debug) و`sadc` (Release) بلا أخطاء/تحذيرات جديدة.
4. **التوليد متسق** — YAML + `generated/` متطابقان في نفس الـ commit.
5. **التوثيق والأخطاء مدمجان** — `description_ar/en` مكتملان؛ الأخطاء عبر الكتالوج الموحَّد.
6. **سجل المطوّر + قائمة الملفات** موثّقان (ما نُفِّذ، الاختبارات، القرارات، كل ملف تغيّر).
7. **الحوكمة مُستوفاة** إن انطبقت (سطر إقرار + تحديث `status/` بدليل فعلي).

> سقوط أي شرط = المهمة **قيد التنفيذ** لا «مُسلَّمة». لا تدّعِ إنجازاً غير مُثبت (BF-12, GR-01).

## خريطة المعمارية (أين يعيش كل شيء)

```
مصدر .ص → LexerCore → ParserCore → AST → ┬→ InterpreterCore / VM        (تنفيذ فوري)
                                          └→ SIRBuilder → SIROptimizer → LLVMCodeGen → تنفيذي
```

| المكوّن | المجلد | الدور |
|---------|--------|------|
| النواة المشتركة | `shared/` | معجمي، نحوي، AST، نظام الأنواع `Value`، **نظام الأخطاء**، **سجل الدوال المُولَّد** |
| المفسر | `interpreter/` | مفسر شجري؛ **تنفيذ الدوال المضمنة** في `interpreter/src/builtins/` |
| المترجم | `compiler/` | AST → SIR → LLVM IR → تنفيذي؛ codegen للدوال المضمنة في `compiler/src/backend/llvm/builders/builtins/` |
| الآلة الافتراضية | `vm/` | بايت كود مرتبط بالمفسر |
| **مصدر الحقيقة** | `language-truth/` | **YAML SoT** لكل بيانات اللغة |
| مولّدات الكود | `scripts/codegen/` | `gen_*.py` تقرأ YAML وتُنتج C++ |
| الكود المُولَّد | `shared/*/generated/` | **لا يُحرَّر يدوياً** |
| أدوات | `tools/` | `lsp/`, `formatter/`, `pkg/`, `compiler/` |

## المراجع التفصيلية

| النظام | متى تقرأه | الملف |
|--------|-----------|-------|
| **كتيّب المهام (قوائم تنفيذ)** | **عند تنفيذ مهمة شائعة** — تدفّق مرقّم + كيفية تعديل الكود | [./references/playbooks.md](./references/playbooks.md) |
| **فهرس كل الأنظمة** | لمعرفة كل أنظمة اللغة وأين تقع (لتحليل الأثر) | [./references/systems-catalog.md](./references/systems-catalog.md) |
| **الأنظمة متشابكة (اقرأه أولاً)** | **قبل أي تغيير** — لماذا لا يوجد تغيير معزول | [./references/interconnected-systems.md](./references/interconnected-systems.md) |
| **نظام الحوكمة الإلزامي** | إن مسّت `_bmad-output/` أو عملت ضمن ستوري محكوم | [./references/governance.md](./references/governance.md) |
| **سير العمل الصارم + معيار الإنجاز + سير عمل الفروع** | قبل البدء بأي تغيير — التسلسل، قائمة الملفات، DoD، **والعمل على فرع `dev` عبر worktree + PR** (§5) | [./references/workflow.md](./references/workflow.md) |
| **شروط التسليم + تحليل الأثر + قائمة ختامية** | **قبل إعلان الإنجاز** — هل أثّرت على كل المسارات؟ هل نسيتُ شيئاً؟ | [./references/delivery-checklist.md](./references/delivery-checklist.md) |
| المعمارية وخط الأنابيب الكامل | قبل أي تعديل بنيوي | [./references/architecture.md](./references/architecture.md) |
| نظام الأخطاء والتشخيص | إضافة/تعديل رمز خطأ أو رسالة | [./references/error-system.md](./references/error-system.md) |
| بنى YAML للأخطاء الثلاث + الهجرة V4→V5 | قبل لمس أي ملف أخطاء YAML | [./references/error-yaml-structures.md](./references/error-yaml-structures.md) |
| نظام الدوال المضمنة | إضافة/تعديل دالة أو وحدة | [./references/builtins-system.md](./references/builtins-system.md) |
| نظام الأنواع و SIR | العمل على `Value`/`SadTypeKind`/opcodes المترجم | [./references/types-and-sir.md](./references/types-and-sir.md) |
| نظام التوثيق و YAML (SoT) | تعديل بيانات اللغة + التوليد | [./references/docs-yaml-system.md](./references/docs-yaml-system.md) |
| إضافة نظام/ميزة جديدة | بناء نظام داخلي جديد | [./references/adding-systems.md](./references/adding-systems.md) |
| البناء والاختبار | تشغيل/اختبار التغييرات | [./references/build-test.md](./references/build-test.md) |

## ملفات يجب قراءتها أولاً (للسياق)

- `shared/lexer/include/token.h` — أنواع الرموز و `Position`
- `shared/types/include/value.h` — نوع القيم في وقت التشغيل (`Value`)
- `shared/errors/include/error_codes.h` — تعداد `ErrorCode`
- `interpreter/include/core/interpreter_core.h` — نقطة دخول المفسر
- `interpreter/src/builtins/BUILTIN_CODING_STANDARDS.md` — معايير كتابة الدوال المضمنة
- `language-truth/README.md` — مصدر الحقيقة
- `compiler/include/frontend/sir_types.h` — تعليمات وأنواع SIR

## قواعد لا يُتهاون فيها

- **لا تغيير معزول:** أي تعديل يَمَسّ أنظمة متشابكة (توثيق + أخطاء + أقسام + اختبارات). الكود
  «المتكامل» يعبرها كلها — وإلا فشل CI أو انكسرت تجربة المستخدم ([./references/interconnected-systems.md](./references/interconnected-systems.md)).
- **الحوكمة فوق الكل:** عند مسّ `_bmad-output/` أو العمل ضمن ستوري محكوم — اقرأ السياسة واكتب سطر الإقرار أولاً ([./references/governance.md](./references/governance.md)).
- **مدفوع بالبيانات:** بيانات اللغة من `language-truth/` ثم توليد — لا تحرير يدوي للمُولَّد (لكن ضمّنه في الـ commit — متتبَّع في git).
- **ثوابت مُولَّدة لا سلاسل حرفية:** سجّل الدوال بـ `Bn::<Group>::<CPP_ID>`، وطرق الأنواع بـ `TM::<Group>::<NAME>`، وأطلِق الأخطاء بـ `ErrorCode::<NAME>` + placeholders. **يُمنع** نص خطأ حر (`runtime_throw.h`) والألقاب اليدوية (`remove_aliases.py`). `throw std::runtime_error` في ملفات قديمة = نمط مهجور.
- **الطبقة الصحيحة:** أصلِح الخطأ حيث يحدث (BF-10). lexer→lexer، ترتيب حقول→SIR builder، تحويل أنواع→codegen.
- **حل جذري لا ترقيع:** يُمنع `if` خاص لحالة واحدة، أو تحويل نوع لإخفاء عدم تطابق (BF-09).
- **مزدوج اللغة:** كل API عام موثّق بـ `@brief (AR)` و `@brief (EN)` (CW-08).
- **التوافق الخلفي:** إضافة opcode/token/AST node مسموحة — تغيير معنى موجود ممنوع (CW-24).
- **اختبار المفسر + المترجم:** لا تكتفِ بأحدهما (BF-08, BF-29).
- **فرع `dev` عبر PR فقط:** العمل يتكامل في فرع **`dev`** (لا `sadlang`). كلاهما محميّ على GitHub
  (Rulesets، توقيع GPG + PR إلزاميّان). أنشئ فرع `agent/<مهمة>` من `dev` في **worktree** بمجلد
  `C:/s_lang/temp-brunch/`، أودِع بـcommits **موقّعة GPG**، وادمج في `dev` **عبر PR** — لا دفع مباشر.
  المستودع: `sadlang/s-programming-language`. التفصيل في [./references/workflow.md](./references/workflow.md) §5.
- **كل تعليق بالعربية وموسّع** — وكل ملف يبدأ بكتلة تشرح وظيفته.
