# تعليمات Copilot — لغة ص / Sad Programming Language (النواة النحيلة)

> **حافظ على هذا الملف ≤ 200 سطر (يفرضه حارس CI — RFC 0004).**
> هذه «النواة الدائمة النحيلة». التفاصيل تُحمَّل عند الطلب من المراجع المُشار إليها أدناه،
> ولا تُكرَّر هنا. المصدر القانوني الكامل للقواعد والمرجع موجود في ملفّاته المخصّصة.

## نظرة عامة

لغة ص (Sad) لغة برمجة عربية حديثة بـ C++17، ملفات المصدر `.ص`. تشمل: مفسّرًا شجريًّا،
مترجمًا (`sadc`) عبر LLVM، آلة بايت كود (VM)، خادم LSP، مدير حزم، ومنسّقًا.

## المعمارية (المكوّنات الكبرى)

| المكون | المجلد | الدور |
|--------|--------|------|
| النواة المشتركة | `shared/` | lexer/parser/AST/أنواع `Value`/الأخطاء — مشترك |
| المفسّر | `interpreter/` | مفسّر شجري؛ `InterpreterCore` |
| المترجم | `compiler/` | AST → SIR → LLVM IR → تنفيذيّ |
| الآلة الافتراضية | `vm/` | بايت كود مرتبط بالمفسّر |
| المكتبة القياسية | `stdlib/` | `core/`, `io/`, `math/`, `string/`, `network/`, `graphics/` |
| الأدوات | `tools/` | `lsp/`, `formatter/`, `pkg/`, `repl/`, `compiler/` |

مسار البيانات: `مصدر .ص → LexerCore → ParserCore → AST → {InterpreterCore/VM} | {SIRBuilder → SIROptimizer → LLVMCodeGen → تنفيذيّ}`

## البناء والتشغيل

```powershell
cmake -S . -B build                                          # تهيئة أولى
cmake --build build --config Debug --target sad-run          # المفسّر (sad.exe)
cmake --build build --config Debug --target sad-build        # المترجم (sadc)
.\build\bin\Debug\sad.exe examples\test_simple.ص            # تشغيل
ctest --test-dir build -R Comprehensive                      # الاختبارات (فعّلها بـ -DBUILD_TESTS=ON)
```

## معايير الكود (C++) — الأساسيّات

- الأسماء الفرعية: `Sad::Lexer`, `Sad::Parser`, `Sad::Compiler::SIR`, `Sad::LLVM`, `Sad::Interpreter`, `Sad::Data`.
- كل API عام موثّق بـ `@brief (AR)` و `@brief (EN)`.
- تعداد `TokenType`: استخدم دائمًا `KEYWORD_*`، لا نصوص عربية في switch.
- الكلمات السياقية في Parser: تحقّق مزدوج `check(TT::KEYWORD_X) || (check(IDENTIFIER) && value=="العربي")`.
- `Value` (`shared/types/include/value.h`) نوع القيم الموحّد؛ OBJECT يحمل `shared_ptr<ObjectInstance>`.
- `Position` يبدأ من 1؛ `offset` من 0. `DataType` enum بلا `ANY` — استخدم `DT::OBJECT`.

## 🧭 جدول التوجيه — أين تذهب لكل نوع مهمّة

| نوع المهمّة | المرجع/المهارة |
|------------|----------------|
| تطوير اللغة نفسها (C++ للمفسّر/المترجم، الأنظمة الداخلية، نظام الأخطاء، `language-truth/`) | `.github/skills/sad-lang-dev/SKILL.md` |
| كتابة برامج بلغة ص (`.ص`) | `.github/skills/sad-lang-coding/SKILL.md` |
| الدوال المضمنة ونظام الاستيراد (منظور المستخدم) | `.github/skills/sad-builtins/SKILL.md` |
| بناء أنظمة تشغيل بلغة ص | `.github/skills/sad-os-coding/SKILL.md` |
| **مرجع اللغة الكامل** (النحو، الكلمات الـ40، الماكروز، التزامن، العقود، مطابقة الأنماط) | `language-truth/` (مصدر الحقيقة YAML) + `docs/parser_rule/_generated/` (قواعد المحلّل المولَّدة) |
| **قواعد الهندسة الـ60** (CW-01..30 كتابة + BF-01..30 إصلاح) | `docs/ENGINEERING_RULES.md` |
| أيّ عمل في `_bmad-output/` | قسم الحوكمة المتدرّجة أدناه |

> ملخّص اللغة (للاستئناس فقط؛ الحقيقة في المرجع): 40 كلمة محجوزة + 3 عوامل منطقية (`و`/`أو`/`ليس`)؛
> الكتل تُغلق بـ`نهاية`؛ الفاصلة `،`/`,` والمنقوطة `؛`/`;` متكافئتان؛ التعليقات `#`، `#* *#`، `##`؛
> أسماء الأنواع (`رقم`، `نص`، `عشري`...) **ليست محجوزة**. كل شيء UTF-8 عربيّ/إنجليزيّ.

## ⚠️ نظام الحوكمة المتدرّج — قبل أي عمل في `_bmad-output/`

**أعلى أولوية. تجاوزه = خرق سياسة.** الإلزام **متدرّج** (ADR-GOV-001، RFC sadlang/rfcs#5):

### أ) مهمّة خفيفة — يكفيها الـDigest
تعديل **داخل ملف قائم** تحت `status/` أو `stories/` (تحديث حالة/نصّ ستوري):
اقرأ فقط [`_bmad-output/GOVERNANCE_DIGEST.md`](../_bmad-output/GOVERNANCE_DIGEST.md) (مولَّد ومحروس طزاجةً بالـCI).
سطر الإقرار:
> "قرأت `GOVERNANCE_DIGEST.md` (مولَّد &lt;تاريخ&gt;)؛ السبرنت: `<اسم>`؛ المهمة تحت `status/`|`stories/` فلا تستلزم القراءة الكاملة (ADR-GOV-001)."

### ب) مهمّة بنيويّة — تلزم القراءة الكاملة
إنشاء/حذف أيّ ملف، أو لمس `decisions/` (ADR) أو `planning/` (PRD/معمارية/خارطة) أو `sprints/`.
اقرأ بالترتيب: `governance/1-policy/planning/PRD.md` ← `.../PROJECT_MANAGEMENT_FRAMEWORK.md`
← آخر `.../status/VERIFICATION_REPORT_<date>.md` ← `.../sprints/SPRINT_CURRENT.md`
← `governance/3-code-contract/planning/prd.md`. سطر الإقرار الكامل:
> "قرأت السياسة في `_bmad-output/governance/1-policy/`؛ آخر تقرير تحقق: `VERIFICATION_REPORT_<YYYY-MM-DD>.md`؛ السبرنت الحالي: `<اسم>`."

### القواعد الحوكميّة (GR) — العناوين (التفصيل في PRD/PMF)
- **GR-01:** لا ادعاء نسبة إنجاز بلا دليل من الكود الفعليّ (grep/build).
- **GR-02:** لا تَحذف ADRs أبدًا؛ المُلغى `status: Superseded` + `supersededBy`.
- **GR-03:** السبرنت لا ينتهي بلا `SPRINT_<date>_RETRO.md`.
- **GR-04:** كل ملف زائف يُعلَّم `status: OUT-OF-DATE` فورًا (بلا حذف تاريخيّ).
- **GR-05:** قبل نظام جديد انسخ `_bmad-output/systems/_TEMPLATE/` بالبنية السداسيّة.
- **GR-06:** التواريخ من الجهاز فقط — `Get-Date -Format "yyyy-MM-dd"`.

> البنية السداسيّة لكل نظام: `planning/ epics/ stories/ sprints/ status/ decisions/ + README.md`.
> استثناء `living-documentation/`: 3 وثائق جذر قانونية (`STRATEGY`/`ARCHITECTURE`/`IMPLEMENTATION_PLAN`)
> + `stories/` نشطة + `_archive/` تاريخيّ (NOT-A-SoT) — راجع ADR-DOCS-CANONICAL.

## اللغة والتواصل

- تكلّم مع المستخدم **بالعربية فقط**؛ والتفكير والتحليل بالعربية.
- كل تعليق/شرح داخل الملفات بالعربية وبشكل موسّع؛ كل ملف يبدأ بكتلة تعليق تشرح وظيفته.

## سلوك المهام

- **الحلّ جذريّ من الجذر** (وفق `docs/ENGINEERING_RULES.md`)، لا ترقيع ولا إخفاء أعراض.
- **لا تستخدم وكلاء فرعيين (subagents)** إلا بطلب صريح من المستخدم.
- أبلِغ عمّا يفشل بصدق؛ لا تستبدل حالة فاشلة بعامل يُخفيها.
- بادر بالخطوة التالية ولا تنتظر أمرًا من المستخدم.
- بعد إتمام أي مهمة، اختم بـ`AskUserQuestion` (الخيار الموصى به أولًا)، كل الخيارات بالعربية،
  مع خيار «شيء آخر...» + `allowFreeformInput: true` على مستوى السؤال (إلزاميّ معًا).
- إن ظهر «Recovered from a request error» مرّتين متتاليتين، غيّر الأمر (قسّمه/رشّح مخرجاته)
  ونفّذ البديل فورًا دون اعتذار أو انتظار تعليمات.
