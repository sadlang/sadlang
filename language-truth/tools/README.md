# نظام «مصدر حقيقة الأدوات» / Tools' Source-of-Truth (Tool SoT)

> بسم الله الرحمن الرحيم

## ما هذا؟ / What is this?

هذا المجلّد (`language-truth/tools/`) هو **مصدر الحقيقة الخاصّ بأدوات لغة ص** — لا
باللغة نفسها. كلّ أداة (REPL، وما يليها) تملك كتالوجًا مستقلًّا لِـ:

- **أخطائها** الخاصّة (فشل فتح ملفّ، أمر مجهول، خطأ داخليّ في الأداة…).
- **رسائل واجهتها** غير-الخطئيّة (ترويسات، تلميحات، عناوين أقسام…).
- **أوامرها** التفاعليّة (اسم/مرادف عربيّ/وصف/صيغة استعمال).

**لماذا منفصل عن مصدر حقيقة اللغة؟** لأنّ هذه المفردات **ليست جزءًا من لغة ص**؛
هي شأنٌ داخليّ لأداةٍ مساعِدة. مبدأٌ راسخ: *الأدوات لا توسّع لغة ص*. فبدل أن
تُلوِّث الأداةُ `keywords.yaml`/`errors.yaml` اللغويّة، تحصل على فضاء SoT خاصّ بها
هنا، متعامدٍ تمامًا على مصدر حقيقة اللغة.

This directory is the **Source of Truth for the Sad language _tools_** — not the
language. Each tool owns an independent catalog of its errors, UI messages, and
commands. It is intentionally **orthogonal** to the language SoT: tools must not
expand the language, so instead of polluting `language-truth/*.yaml`, a tool gets
its own namespace here.

## البنية / Layout

```
language-truth/tools/
  README.md                 ← هذا الملفّ / this file
  _schemas/                 ← مخطّطات JSON عامّة تتشاركها كلّ الأدوات
    tool_meta.schema.json       هوية أداة (فضاء C++، بادئة، إصدار، تنسيق)
    tool_errors.schema.json     كتالوج أخطاء أداة
    tool_messages.schema.json   رسائل واجهة أداة
    tool_commands.schema.json   أوامر أداة تفاعليّة
  <tool>/                   ← نسخةٌ لكلّ أداة (أوّلها: repl)
    _meta.yaml                  إلزاميّ
    errors.yaml                 اختياريّ
    messages.yaml               اختياريّ
    commands.yaml               اختياريّ (للأدوات التفاعليّة)
```

## خطّ التوليد / Codegen pipeline

المولِّد **العامّ** `scripts/codegen/gen_tool_sot.py` يقرأ مجلّد أداةٍ ويُنتج
كتالوج C++ (رأس + مصدر) مُلتزَمًا داخل شجرة الأداة (مثل `tools/repl/generated/`)،
تحت فضاء الأسماء المُعلَن في `_meta.cpp_namespace`. الاستدعاء عبر بوّابة البناء:

```bash
python x.py gen           # يُوَلِّد كلّ النطاقات (ومنها الأدوات)
python x.py gen --check   # حارس الانجراف في CI (يفشل لو حُرِّر المولَّد يدويًّا)
```

الملفّات المولَّدة **مُلتزَمة** وتُجمَّع مباشرةً (بلا هدف CMake خاصّ) — كنمط نطاقات
`ui_*`؛ حارس الانجراف يكفل بقاءها متزامنةً مع الـ YAML.

The **generic** generator `scripts/codegen/gen_tool_sot.py` reads a tool directory
and emits a committed C++ catalog (header + source) inside the tool's own tree,
under the namespace declared in `_meta.cpp_namespace`. Wired through `x.py gen`
(+ `gen --check` drift guard). Generated files are committed and compiled directly.

## واجهة C++ المولَّدة / Generated C++ API (لكلّ أداة)

ضمن `<cpp_namespace>` (مثل `Sad::REPL::SoT`):

| العنصر | الوصف |
|---|---|
| `kToolId`, `kVersion`, `kDisplayNameAr/En` | ثوابت هوية الأداة |
| `kBilingualSeparator`, `kDetailSeparator` | ثوابت تنسيق ثنائيّ اللغة |
| `enum class Error` + `findError` + `errorMessage(code, detail)` | كتالوج الأخطاء |
| `enum class Message` + `messageAr/En/Both` | رسائل الواجهة |
| `enum class CommandHandler` + `kCommands[]` | جدول الأوامر (يُربَط معالِجه في C++) |

## إضافة أداة جديدة / Adding a new tool

1. أنشئ `language-truth/tools/<tool>/_meta.yaml` (+ ما يلزم من errors/messages/commands).
2. أضِف نطاقًا في `CODEGEN_DOMAINS` داخل `x.py` (بمولِّد `gen_tool_sot.py` ومجلّد إخراج الأداة).
3. `python x.py gen` ثمّ اربط الرأس المولَّد وضمّ مصدره في `CMakeLists` الأداة.
4. استبدل الحرفيّات ثنائيّة اللغة في كود الأداة باستدعاءات الكتالوج.
