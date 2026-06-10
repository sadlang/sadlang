---
id: ADR-DOCS-V4-004
title: حَذف sadlang-info CLI نِهائياً (V5)
status: Accepted
date: 2026-06-05
decidedBy: المالك (صالح)
decidedOn: 2026-06-05
relatedAdrs:
  - ADR-DOCS-V4-001  # الاعتماد الأَصلي
  - ADR-DOCS-V4-002  # القَرارات المُوَحَّدة (مَنطِق المَجال)
  - ADR-DOCS-V4-003  # تَكامل Codegen — هَذا ADR يَستَكمِله
supersedes:
  - ARCHITECTURE V4 §4 (CLI sadlang-info)
context: |
  بَعد تَطبيق ADR-003 (تَكامل Codegen)، أصبَح CLI sadlang-info زائداً —
  لأن جَميع طُرق الوصول المُتاحة تَكفي بِدونه. هَذا ADR يَحسِم حَذفه نِهائياً.
---

# ADR-DOCS-V4-004 — حَذف sadlang-info CLI نِهائياً (V5)

## السِياق

في V4 (ADR-002)، خُطِّط `sadlang-info` كَأَداة CLI مَكتوبة بـ C++ تَستَخدِم `libsadlangtruth` لِعَرض/البَحث/التَحَقُّق من YAML:

```bash
sadlang-info list keywords
sadlang-info show KW-FUNC-001
sadlang-info search "دالة"
sadlang-info stats
sadlang-info validate keywords/KW-NEW.yaml
sadlang-info export json
```

بَعد ADR-003 (V5 Codegen Integration)، النَموذج تَغَيَّر جَوهَرياً:
- المَكتبة C++ wrapper تَقرأ من `Sad::Lexer::Generated::*` (لا yaml-cpp، لا runtime YAML loading)
- YAML مَلفات نَصية بَسيطة في `language-truth/` (مَلف لِكُل نِطاق)
- نِظام `scripts/codegen/*.py` يَحوي بِالفِعل validation كامِل

السُؤال: **هَل CLI مَطلوب فِعلياً في V5؟**

## التَحليل

### مَن قَد يَستَخدِم CLI؟ وماذا يَفعل بَدلاً منه في V5؟

| المُستَخدِم | الاستِخدام المُتَوَقَّع | البَديل في V5 |
|---|---|---|
| **أداة C++ (lexer, parser, formatter)** | `getKeyword("KW-FUNC-001")` | `Registry::instance().find_by_arabic()` مُباشرة عبر Wrapper |
| **أداة Python (codegen, validators)** | `subprocess.run(["sadlang-info", "list"])` | `yaml.safe_load(open("language-truth/keywords.yaml"))` مُباشرة |
| **أداة Node.js (LSP server, Website)** | `child_process.exec("sadlang-info show")` | `require("js-yaml").load(fs.readFileSync("language-truth/keywords.yaml"))` مُباشرة |
| **مُطَوِّر بَشَري (CLI manual)** | `sadlang-info show KW-FUNC-001` | `cat language-truth/keywords.yaml \| grep -A 15 "KW-FUNC-001"` أو فَتح المَلف مُباشرة |
| **CI validation** | `sadlang-info validate file.yaml` | `python scripts/codegen/gen_keywords.py --validate-only` (مَوجود) |
| **Export JSON/CSV** | `sadlang-info export json` | `python -c "import yaml,json; print(json.dumps(yaml.safe_load(open('language-truth/keywords.yaml'))))"` |

### تَحليل التَكلِفة/الفائدة

| البُعد | تَكلِفة بِناء CLI | فائدة CLI |
|---|---|---|
| **كود C++** | ~500 LOC (main.cpp + arg parser + formatters) | لا قِيمة فَوق `cat` + `grep` |
| **تَوثيق** | ~200 LOC (help, man page, examples) | تَكرار لِما في README |
| **اختبارات** | ~300 LOC (E2E + unit) | تَكرار لِاختبارات Codegen |
| **CMake target** | `add_executable(sadlang-info)` + linkage | عَبء بِناء إضافي |
| **التَوزيع** | binary مَنفَصل لِكُل منصَّة | binary إضافي يَجِب صيانَته |
| **Discovery** | `--help`, command help | الـ YAML نَفسه ذاتي-التَوثيق |
| **Performance** | فَور — مَكتبة Wrapper تَعمَل بِـ 0ms | لا فَرق |

**النَتيجة:** CLI = ~1000 LOC + binary إضافي + صيانَة دائمة، لِفائدة = **صِفر** عَلى ما تُقَدِّمه YAML الخام + Wrapper.

### قَواعد الكود المُتَأَثِّرة

- **CW-19 (DRY):** CLI يُكَرِّر ما يَفعَله `cat`/`grep`/`yaml.safe_load`/Wrapper.
- **CW-01 (SRP):** CLI لَيس لَه مَهمَّة فَريدة — كُل وَظيفَة لَه تُؤَدَّى أَفضَل بِأَداة قائمة.
- **BF-09 (لا تَرقيع):** الاحتِفاظ بِـ CLI = تَرقيع نَمَط V4 على نَموذج V5 الجَديد.
- **BF-15 (لا كَسر API):** لا أَحَد يَستَخدِم CLI حالياً — لا API لِكَسره.

## القَرار

**حَذف `sadlang-info` CLI نِهائياً مِن نَطاق V5.**

طُرُق الوصول المُعتَمَدة بَدلاً منه:

```text
┌─────────────────────────────────────────────────────────────┐
│  مُستَخدِم C++ → #include <sad/langtruth.h>                  │
│                  Registry::instance().find_by_arabic("دالة") │
├─────────────────────────────────────────────────────────────┤
│  مُستَخدِم Python → yaml.safe_load(open("language-truth/...")) │
├─────────────────────────────────────────────────────────────┤
│  مُستَخدِم Node → require("js-yaml").load(fs.readFileSync()) │
├─────────────────────────────────────────────────────────────┤
│  مُطَوِّر بَشَري → فَتح المَلف في VS Code، أو cat/grep         │
├─────────────────────────────────────────────────────────────┤
│  CI/CD → python scripts/codegen/gen_keywords.py --validate  │
└─────────────────────────────────────────────────────────────┘
```

### ما يَتَغَيَّر

- ❌ حَذف §4 من ARCHITECTURE.md (CLI)
- ❌ حَذف `add_executable(sadlang-info)` من §6.1
- ❌ حَذف ذِكر CLI من `scope` في frontmatter
- ❌ حَذف ذِكر CLI من Mermaid diagram §0
- ❌ حَذف ذِكر CLI من STRATEGY.md
- ❌ حَذف ذِكر CLI من README.md (الطَريقة 2: CLI)
- ✅ إضافة فِقرة "كَيف تَقرأ YAML مُباشرةً" بَدلاً في README.md
- ✅ تَوثيق هَذا الإلغاء في كُل تَغيير

### ما يَبقى

- ✅ `libsadlangtruth` (Wrapper C++) — مَطلوب لِأَدوات C++
- ✅ `scripts/codegen/*.py` — توليد C++ Generated:: وَ validation
- ✅ YAML المَفتوحة في `language-truth/` — مَصدَر للجَميع
- ✅ JSON Schema في `_schemas/` — للتَحَقُّق

## البَدائل المَرفوضة

### A) الاحتِفاظ بـ CLI كامِلاً (الخِيار V4)

❌ مَرفوض. تَكرار + صيانَة بِلا فائدة. كُل أَمر لَه بَديل أَفضَل.

### B) تَقليص CLI إلى `sadlang-info validate` فَقَط

❌ مَرفوض. الـ validation مَوجود بِالفِعل في `scripts/codegen/_lib/validator.py` ويَعمَل ضِمن build pipeline. لا حاجَة لِنَسخَة C++ مُنفَصِلة.

### C) سكربت Python بَسيط بَدلاً من C++ binary

❌ مَرفوض. الـ scripts الموجودة (`gen_*.py`) تَكفي. سكربت إضافي = تَكرار + نُقطَة فَشَل جَديدة.

## الأَثَر (Impact)

### على ARCHITECTURE.md

- حَذف §4 كامِلاً (CLI sadlang-info)
- تَحديث Mermaid في §0 (إزالة عُقدَة CLI)
- تَحديث `scope` في frontmatter (إزالة "CLI")
- تَحديث §11 (CW compliance — إزالة سُطور CLI)

### على STRATEGY.md

- تَحديث الجَدول الزَمَني (إزالة "M2: CLI")
- تَحديث طُرُق الاستِخدام

### على README.md

- إزالة "الطَريقة 2: CLI"
- تَوسيع "الطَريقة 3: YAML مُباشرة" مَع أَمثِلة Python/Node/Bash

### على ADR-002

- تَحديث القَرار 7 (`langtruth-build`) — مُلغى مُسبَقاً بِـ ADR-003
- لا تَأثير إضافي

### على Stories/Sprints

- إلغاء أَي ستوري مُخَطَّطَة لِبَناء CLI (إن وُجِدت)
- تَحويل SP المُخَصَّصة لِـ CLI إلى توسعة `scripts/codegen/`

## القَواعد المَرجِعية

- **CW-01 (SRP):** كُل مُكَوِّن مَسؤولِيَّة واحدة — CLI لا يُضيف مَسؤولِيَّة.
- **CW-19 (DRY):** لا تَكرار — CLI سَيُكَرِّر cat/yaml/Wrapper/Codegen.
- **CW-30 (Profile Before Optimize):** لا حاجَة لِأَداة بِلا مَطلَب فِعلي.
- **BF-04 (Root Cause):** السَبَب الحَقيقي لِوجود CLI كان غِياب Wrapper — V5 يَحلُّه.
- **BF-09 (لا تَرقيع):** لا نُحافِظ على CLI لِمُجَرَّد أَنه كان في V4.

## الحالة بَعد التَطبيق

| البَعد | قَبل V5 | بَعد V5 |
|---|---|---|
| عَدَد binaries | 2 (`sad.exe` + `sadlang-info.exe`) | 1 (`sad.exe` فَقَط) |
| LOC للصِيانة | +1000 (CLI) | 0 |
| طُرُق الوصول | 3 (C++/CLI/YAML) | 3 (C++ Wrapper / Python yaml / مُحَرِّر نَصي) |
| Time-to-fact | ~5ms (CLI startup) | 0ms (`grep` فَوري، Wrapper مُدمَج) |
| تَعقيد البِناء | يَحتاج CMake target | لا |

## التَوقيع

| | |
|---|---|
| **المُقتَرَح** | Amelia (Dev Agent) |
| **المُعتَمَد** | المالك (صالح) |
| **التاريخ** | 2026-06-05 |
| **سَجل الحوكمة** | `VERIFICATION_REPORT_2026-05-30.md` |

---

**ADR-DOCS-V4-004 مُعتَمَد ونافِذ. CLI `sadlang-info` مَحذوف نِهائياً من نَطاق V5.**
