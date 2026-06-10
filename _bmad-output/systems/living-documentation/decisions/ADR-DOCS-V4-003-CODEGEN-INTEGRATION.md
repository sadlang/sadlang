---
adr: ADR-DOCS-V4-003-CODEGEN-INTEGRATION
title: تَكامل language-truth مع نِظام Codegen القائم (Python → C++ Generated)
status: Accepted
date: 2026-06-05
deciders: [Amelia (Dev Agent), المالك (صالح)]
supersedes: []
relatedAdrs:
  - ADR-DOCS-V4-001-APPROVAL
  - ADR-DOCS-V4-002-UNIFIED-STRUCTURE
tags: [architecture, codegen, integration, build-time]
---

# ADR-DOCS-V4-003: تَكامل مع نِظام Codegen القائم

## السياق (Context)

أَثناء كِتابة `ARCHITECTURE.md V4`، اقتُرِح نَموذج تَحميل YAML في **وقت التشغيل** عَبر `yaml-cpp` ثم تَوليد cache ثُنائي لاحِقاً (M2+). هذا النَموذج أُسِّس على افتِراض **خاطئ** بأن المَشروع لا يَملك بَنية تَحتية لمُعالَجة YAML.

### الواقع المُكتَشَف (Audit نَتيجة)

المَشروع يَملك بالفِعل بَنية ناضِجة وعامِلة:

| المُكَوِّن | الحالة | المَلف |
|-------|------|------|
| **YAML للكَلِمات** | ✅ كامِل (14.16 KB، 91 إدخال، v4.1) | [data/language/keywords.yaml](../../../../data/language/keywords.yaml) |
| **JSON Schema** | ✅ كامِل | [data/language/keywords.schema.json](../../../../data/language/keywords.schema.json) |
| **Python Codegen** | ✅ كامِل (273 سَطر) | [scripts/codegen/gen_keywords.py](../../../../scripts/codegen/gen_keywords.py) |
| **C++ Generated** | ✅ مُولَّد، يُبنى تِلقائياً | [shared/lexer/generated/keywords_generated.h](../../../../shared/lexer/generated/keywords_generated.h) |
| **Consumer (Lexer)** | ✅ يَستَهلِك `Generated::allEntries()` | [shared/lexer/src/lexer_keywords.cpp](../../../../shared/lexer/src/lexer_keywords.cpp) |
| **Consumer (Formatter)** | ✅ يَستَهلِك نَفس API | [tools/shared/src/formatter_keywords.cpp](../../../../tools/shared/src/formatter_keywords.cpp) |
| **CMake Integration** | ✅ `cmake/codegen.cmake` يُولِّد قَبل البَناء | [cmake/codegen.cmake](../../../../cmake/codegen.cmake) |
| **Tests** | ✅ `test_keywords_v41_comprehensive.cpp` يَختَبِر Snapshot | [tests/comprehensive/test_keywords_v41_comprehensive.cpp](../../../../tests/comprehensive/test_keywords_v41_comprehensive.cpp) |
| **YAML للأَخطاء** | ⚠️ جُزئي (281 KB، يَحتاج تَقسيم) | [data/language/error_messages.yaml](../../../../data/language/error_messages.yaml) |
| **Builtins Codegen** | ⚠️ tool فقط (521 سَطر، YAML غير مَخلوق) | [scripts/codegen/gen_builtins.py](../../../../scripts/codegen/gen_builtins.py) |

### عَناصِر إضافية للـ Codegen المَوجودة

تَحت `scripts/codegen/`:
- `gen_all.py` — orchestrator
- `gen_docs.py` — تَوليد توثيق
- `gen_error_messages.py` (417 سَطر) — تَوليد C++ من error_messages.yaml
- `_lib/` — مَكتبة مُشتَركة (loader, emit, validator, context)
- `renderers/` — render_lsp, render_man, render_repl, render_tutorial, render_vitepress
- `doc_ir/` — IR للتَوثيق
- `runners/run_examples.py` — تَشغيل الأَمثلة

### الانتهاك الذي وَقَع في V4

ARCHITECTURE V4 اقتَرَح **استبدال** هذه البَنية بـ `yaml-cpp` وقت التَشغيل، مما يَنتهك:

| القاعدة | الوَصف | التَفصيل |
|-------|------|------|
| **CW-19 (DRY)** | لا تَكرار | إعادة بَناء codegen مَوجود |
| **CW-24 (Backward Compat)** | حِفاظ على API | كَسر `Sad::Lexer::Generated::*` المُستَهلَك في 19 مَوضِع |
| **BF-09 (لا تَرقيع)** | حَل جَذري | V4 رَقعَة على نَموذج خاطئ |
| **BF-10 (الطَبقة الصَحيحة)** | إصلاح في طَبقة المَنشأ | V4 يَحُل في طَبقة Truth وليس طَبقة Integration |
| **CW-22 (نَمط مُوَحَّد)** | اتباع النَمط القائم | V4 يَخلِط `yaml-cpp` runtime مع `Generated::*` build-time |

## القَرار (Decision)

نَعتَمِد **8 قَرارات مُلزِمة** لِجَعل `language-truth/` V5 يَتَكامَل مع البَنية القائمة بدَل استبدالها.

### قَرار 1: نَموذج Codegen (Build-Time) هو الأَساس

| | البَديل المَرفوض (V4) | البَديل المُعتَمَد (V5) |
|---|---|---|
| **نَموذج** | yaml-cpp runtime | Python → C++ Generated (build-time) |
| **مَدى السُرعة** | تَحميل + parse في startup | صِفر تَكلِفة (مَدمَج في binary) |
| **Freestanding (kernel)** | ❌ لا يَعمَل | ✅ يَعمَل |
| **تَكامل مع الموجود** | يَكسِر `Generated::*` | يَستَخدِم `Generated::*` |
| **اعتمادات وقت التَشغيل** | yaml-cpp | صِفر |

**النَتيجة**: كل YAML في `language-truth/` يَمُر بـ Python codegen → C++ Generated → يَستَهلِكها API الجَديد.

### قَرار 2: مَوقع YAML — مُزَدوَج خِلال الانتِقال (Migration Bridge)

| المَرحَلة | المَوقِع |
|---|---|
| **حالياً** | `data/language/keywords.yaml` (يَعمَل) |
| **V5 M0 (تَوسعة)** | إضافة `_bmad-output/systems/living-documentation/language-truth/` كَ Living Doc + ربط symbolic مع `data/language/` |
| **V5 M1 (تَوحيد)** | نَقل `data/language/*.yaml` تَدريجياً إلى `language-truth/` مع تَحديث `cmake/codegen.cmake` |
| **V5 M2 (نِهائي)** | `data/language/` يُحذَف؛ `language-truth/` هو SoT الوَحيد |

**سَبَب الجِسر**: تَجَنُّب كَسر البَناء أَثناء الانتِقال (BF-12 لا إصلاح بدون اختبار).

### قَرار 3: بُنية YAML داخل language-truth/ (مُتَوافِقة مع gen_keywords.py القائم)

البُنية الـ V4 الأَصلية `keywords/{reserved,contextual,...}.yaml` لا تَعمَل مع `gen_keywords.py` الذي يَقرَأ مَلف YAML واحِد بِفِئات مُتَعدِّدة (`categories:`).

**القَرار**: نُحافِظ على **مَلف واحد لكُل نِطاق** ولا نُقَسِّم داخل النِطاق:

```
language-truth/
├── keywords.yaml          # مَلف واحِد بِكُل الفِئات (مُتَوافِق مع gen_keywords.py)
├── keywords.schema.json
├── builtins/
│   ├── core.yaml          # gen_builtins.py يَتَوقَع هذا المَكان
│   ├── concurrency.yaml
│   └── types.yaml
├── builtin_function.schema.json
├── errors.yaml            # تَقسيم 281KB إلى مَلفات حَسب category لاحِقاً (M2)
├── error.schema.json
├── operators.yaml         # جَديد — لا يُوجَد حالياً
├── operator.schema.json   # جَديد
├── directives.yaml        # جَديد
├── directive.schema.json  # جَديد
├── types.yaml             # جَديد — أَنواع مُدمَجة
├── type.schema.json       # جَديد
├── grammar.yaml           # جَديد — قَواعِد نَحوية
├── grammar.schema.json    # جَديد
├── stdlib/
│   ├── modules.yaml       # ~15 وَحدة
│   └── functions.yaml     # ~100 دالة
├── stdlib_module.schema.json
└── stdlib_function.schema.json
```

**لا يُوجَد** مُجَلَّد `keywords/` فَرعي يَحوي ملفات مُنفَصِلة لِكل subcategory.

### قَرار 4: التَقسيم الداخلي عَبر حُقول `category`/`subcategory` داخل YAML

بَدلاً من تَقسيم الملفات، نَستَخدِم حُقول داخِلية:

```yaml
# keywords.yaml — بُنية مُتَوافِقة مع gen_keywords.py
version: "5.0"
language: "sad"
categories:
  reserved:
    description: "كلمات محجوزة، Lexer يُصدر KEYWORD_*"
    emittedByLexer: true
    keywords:
      - word: "دالة"
        tokenType: KEYWORD_FUNCTION
        english: function
        subcategory: declarations  # ← جَديد للتَنظيم الدَلالي
        roles: [block_opener]
      # ...
```

`subcategory` حَقل جَديد اختياري — يُضاف للتَنظيم الدَلالي بدون كَسر التَوافق.

### قَرار 5: libsadlangtruth — Wrapper فَوق Generated::* وليس بَديل عَنها

`libsadlangtruth` (المَقتَرَح في V4 §2.2) لا يَستَخدِم `yaml-cpp` مُطلَقاً. بَدلاً من ذلك، هو **طَبَقة API نَظيفة** فَوق `Sad::*::Generated::*` المَوجودة:

```cpp
// shared/langtruth/include/langtruth.h
namespace Sad::LangTruth {

// (AR) واجهة API نَظيفة فَوق Generated::*
// (EN) Clean API layer over Generated::*

const KeywordView* getKeyword(std::string_view id);
const KeywordView* findKeywordByArabic(std::string_view ar);
const KeywordView* findKeywordByEnglish(std::string_view en);
bool isReservedKeyword(std::string_view word);
bool isContextualKeyword(std::string_view word);
std::vector<const KeywordView*> getKeywordsBySubcategory(std::string_view sub);

const BuiltinView* getBuiltin(std::string_view id);
const ErrorView* getError(std::string_view id);
const OperatorView* findOperatorBySymbol(std::string_view sym);
// ...
}
```

التَنفيذ يَستَهلِك `Sad::Lexer::Generated::allEntries()` و `Sad::Builtins::Generated::*` ويُغَلِّفها في API ثابِت.

**صِفر** اعتمادات وقت التَشغيل. **صِفر** I/O. **صِفر** parsing.

### قَرار 6: Binary Cache يُلغى نِهائياً (Superseding ADR-002 §قَرار 5)

ADR-DOCS-V4-002 أَجَّل Binary Cache لِـ M2. **ADR-003 يُلغيه نِهائياً** لِأنه لم يَعُد لَه مَعنى:

| | Binary Cache (V4) | Generated C++ (V5) |
|---|---|---|
| **النَموذج** | تَحويل YAML → cache binary وقت البَناء | تَحويل YAML → C++ source وقت البَناء |
| **التَحميل وقت التَشغيل** | mmap + parse | صِفر (مُدمَج في binary) |
| **السُرعة** | ~2ms (تَحميل cache) | 0ms (constexpr) |
| **اعتمادات** | Cache loader logic + version check | صِفر |

Generated C++ هو **بِالفِعل** Binary Cache بِصُورة أَفضَل (مَدمَج في binary نَفسه).

### قَرار 7: `cmake/codegen.cmake` يَتَوسَّع، لا يُستَبدَل

نَموذج CMake الحالي:

```cmake
# cmake/codegen.cmake (مَوجود)
find_package(Python3 3.9 COMPONENTS Interpreter)
add_custom_command(
    OUTPUT  ${SAD_KW_GEN_H} ${SAD_KW_GEN_CPP}
    COMMAND ${Python3_EXECUTABLE} ${SAD_KW_GEN_SCRIPT}
            --yaml ${SAD_KW_YAML}
            --schema ${SAD_KW_SCHEMA}
            --header ${SAD_KW_GEN_H}
            --source ${SAD_KW_GEN_CPP}
    DEPENDS ${SAD_KW_YAML} ${SAD_KW_SCHEMA} ${SAD_KW_GEN_SCRIPT}
)
add_custom_target(sad_keywords_codegen DEPENDS ${SAD_KW_GEN_H} ${SAD_KW_GEN_CPP})
```

التَوسعة V5 تُضيف:

```cmake
# cmake/codegen.cmake (V5 — تَوسعة)
# 1. كَلِمات (مَوجود)
# 2. مُدمَجة (تَفعيل gen_builtins.py القائم)
# 3. أَخطاء (تَفعيل gen_error_messages.py القائم)
# 4. عَوامِل (جَديد — gen_operators.py)
# 5. أَنواع (جَديد — gen_types.py)
# 6. قَواعِد نَحوية (جَديد — gen_grammar.py)
# 7. توجيهات (جَديد — gen_directives.py)
# 8. وَحدات مَكتبة قِياسية (جَديد — gen_stdlib_modules.py)

# هَدف عام: يَجمَع كُل الأَهداف الفَرعية
add_custom_target(sad_langtruth_codegen_all
    DEPENDS
        sad_keywords_codegen
        sad_builtins_codegen
        sad_errors_codegen
        sad_operators_codegen
        sad_types_codegen
        sad_grammar_codegen
        sad_directives_codegen
        sad_stdlib_codegen
)
```

**لا يُستَخدَم** `add_executable(langtruth-build)` (كما اقتُرِح في ADR-002). البَناء يَتَمّ بـ Python tools القائمة + CMake `add_custom_command`.

### قَرار 8: تَجاوز ADR-002 الجُزئي (Partial Supersession)

ADR-003 يَتَجاوَز جُزئياً قَرارات ADR-002 التالية:

| ADR-002 قَرار | الحالة | السَبَب |
|--------|------|------|
| 1. Flat `language-truth/` | ✅ مُحَفَظ | لا تَغيير |
| 2. IDs (KW-, DIR-, BI-, ...) | ✅ مُحَفَظ | لا تَغيير |
| 3. YAML schema (flat arabic/english) | ⚠️ مُعَدَّل | نَستَخدِم بُنية gen_keywords.py القائمة (categories: → keywords: → word/tokenType/english) |
| 4. API (findKeywordByArabic, ...) | ✅ مُحَفَظ | يُنفَّذ كَ wrapper فَوق Generated:: |
| 5. **Binary Cache مُؤجَّل لـ M2** | ❌ مُلغى | استُبدِل بِـ Generated C++ (أَفضَل) |
| 6. Count ~516 | ✅ مُحَفَظ | لا تَغيير |
| 7. **CMake langtruth-build** | ❌ مُلغى | استُبدِل بِـ Python + add_custom_command القائم |
| 8. errors/ flat + subcategory | ⚠️ مُعَدَّل | errors.yaml واحِد بِـ `category:` field (مُتَوافِق مع gen_error_messages.py) |

ADR-002 يُحَدَّث بِـ `supersededBy: ADR-DOCS-V4-003` لِلقَرارات 3, 5, 7, 8.

## النَتائج (Consequences)

### إيجابية

- ✅ **حِفاظ على البَناء الحالي** — لا كَسر لـ `Sad::Lexer::Generated::*` (19 مَوضِع استهلاك)
- ✅ **سُرعة قُصوى** — 0ms في وقت التَشغيل (constexpr)
- ✅ **Freestanding ready** — يَعمَل في kernel/embedded
- ✅ **صِفر اعتمادات runtime** — لا yaml-cpp، لا parsing
- ✅ **اتباع نَمط النِظام** — كل codegen يَستَخدِم نَفس البُنية (CW-22)
- ✅ **اقتِصاد جُهد** — `gen_keywords.py`, `gen_builtins.py`, `gen_error_messages.py` جاهِزة (≈1200 سَطر Python)
- ✅ **اختبار قائم** — `test_keywords_v41_comprehensive.cpp` يَختَبِر Snapshot

### سَلبية / تَكاليف

- ⚠️ **إعادة بَناء عِند تَغيير YAML** — لكِنها سَريعة (CMake incremental)
- ⚠️ **اعتماد Python 3.9 + pyyaml + jsonschema** — مَوجود بالفِعل
- ⚠️ **تَعقيد بُنية CMake** — تَوسعة 8 codegens (يُحَل بـ orchestrator `sad_langtruth_codegen_all`)
- ⚠️ **migration_gradual_M0_M1_M2** — يَحتاج جَدوَل تَنفيذ دَقيق

### المَخاطِر

| المَخطَر | الاحتِمال | الشِدَّة | المُعالَجة |
|--------|------|------|------|
| كَسر `Generated::allEntries()` أَثناء تَوسعة gen_keywords.py | مُتَوسِّط | عالية | اختبار Snapshot قَبل/بَعد + CI gate |
| تَعارُض بَين `data/language/` و `language-truth/` خِلال الانتِقال | عالي | مُتَوسِّطة | symbolic link + Single Source per Fact (ND-V4 Rule 2) |
| Python tools تُصبِح dependency hell | مُنخَفِض | مُنخَفِضة | requirements.txt مُغَلَّق + Docker build env |

## بَدائل مَدروسة (Alternatives Considered)

### A. تَجاهُل البَنية القائمة (V4 الأَصلي) — مَرفوض

- يَنتَهِك CW-19, CW-22, CW-24, BF-09, BF-10
- يُنشِئ نِظامَين مُتَوازيَين (`Generated::*` و `yaml-cpp runtime`)
- يَكسِر 19 مَوضِع استهلاك في lexer/formatter/tests

### B. Hybrid (codegen للكَلِمات + yaml-cpp لِلباقي) — مَرفوض

- يَخلِط نَموذجَين (CW-22 violation)
- لا فائدة من yaml-cpp إذا كان النَموذج الأَول يَعمَل لـ ~74 كَلِمة

### C. Generated C++ Only (V5 — هذا ADR) — ✅ مُعتَمَد

- اتساق كامِل
- استِفادة قُصوى من المَوجود
- صِفر اعتمادات runtime

## مَراجِع (References)

- ARCHITECTURE V4: `_bmad-output/systems/living-documentation/ARCHITECTURE.md` (سَيُعاد كِتابَتها V5)
- ADR-002: `_bmad-output/systems/living-documentation/decisions/ADR-DOCS-V4-002-UNIFIED-STRUCTURE.md`
- Source: `cmake/codegen.cmake`
- Source: `scripts/codegen/gen_keywords.py`
- Source: `shared/lexer/generated/keywords_generated.h`
- copilot-instructions: CW-19, CW-22, CW-24, BF-09, BF-10

## التَطبيق (Application Plan)

1. ✅ كِتابة ADR-003 (هذا المَلف)
2. ✅ تَحديث ADR-002 بـ `supersededBy: ADR-DOCS-V4-003` لِلقَرارات 3, 5, 7, 8
3. ✅ إعادة كِتابة ARCHITECTURE.md V5 بالكامِل
4. ✅ تَحديث STRATEGY.md ليُشير لـ V5
5. ✅ تَحديث README.md ليُوضِّح نَموذج Codegen + إزالة CLI ([ADR-004](ADR-DOCS-V4-004-NO-CLI.md))
6. ⏳ تَحديث ADR-001 ليُذكِّر بِـ ADR-003
7. ✅ تَحقيق نِهائي لِكُل الملفات (تَم في 2026-06-05)

---

## سِجِل التَطبيق (Application Log)

| التاريخ | المَلف | القِسم | التَغيير |
|---------|--------|--------|----------|
| 2026-06-05 | `ARCHITECTURE.md` | frontmatter | `version: V5`, `integrationAdr: V4-003`, `noCliAdr: V4-004` |
| 2026-06-05 | `ARCHITECTURE.md` | §0 Mermaid | عُقدة CLI أُزيلت، Wrapper فَوق Generated::* مُضافة |
| 2026-06-05 | `ARCHITECTURE.md` | §1 بِنية | `language-truth/keywords.yaml` (مَلف واحِد لِكُل نِطاق)، `keyword_adapter.cpp` بَدلاً من `yaml_loader.cpp` |
| 2026-06-05 | `ARCHITECTURE.md` | §2 API | `KeywordView` + `Registry` فَوق `Generated::allEntries()` |
| 2026-06-05 | `ARCHITECTURE.md` | §3 Codegen | تَوسعة `scripts/codegen/*.py` القائمة بَدلاً من إنشاء جَديد |
| 2026-06-05 | `ARCHITECTURE.md` | §4 (بَدلاً من CLI) | "طُرُق الوصول بِلا CLI" — جَدول مُقارنة + Python/Node/Bash |
| 2026-06-05 | `ARCHITECTURE.md` | §6 BUILD | `cmake/codegen.cmake` مَوسَّع لِكُل النِطاقات |
| 2026-06-05 | `ARCHITECTURE.md` | §9.2 Security | تَنظيف yaml-cpp غَير المُستَخدَم |
| 2026-06-05 | `ARCHITECTURE.md` | §12 Changelog | إدخالات V5 لِـ ADR-003 و ADR-004 |
| 2026-06-05 | `STRATEGY.md` | frontmatter | `version: V5`, `integrationAdr: V4-003` |
| 2026-06-05 | `STRATEGY.md` | §0 Mermaid | S3=Wrapper، S4=codegen.cmake |
| 2026-06-05 | `STRATEGY.md` | §1 قاعدة ذَهَبية | "libsadlangtruth Wrapper + YAML مَفتوحة للجَميع" |
| 2026-06-05 | `STRATEGY.md` | §3 شَجَرة | إزالة `sadlang_info/`، إضافة `keyword_adapter.cpp` |
| 2026-06-05 | `STRATEGY.md` | §4 (بَدلاً من CLI) | "قِراءة YAML مُباشَرَة" (Python/bash/Node) |
| 2026-06-05 | `STRATEGY.md` | §8 Gantt M2 | إزالة "sadlang-info CLI"، إضافة "تَوسعة codegen" |
| 2026-06-05 | `STRATEGY.md` | §11 Changelog | إدخالات V5 لِـ ADR-003 و ADR-004 |
| 2026-06-05 | `README.md` | جَدول الحالة | تَحديث V5 + ADR-003 + ADR-004 |
| 2026-06-05 | `README.md` | الطَريقة 2 | حَذف CLI، استِبدالها بِـ "YAML مُباشَرَة" (Python/Node/Bash/CI) |
| 2026-06-05 | `ADR-DOCS-V4-002` | §قَرار 3, 5, 7, 8 | `supersededBy: ADR-DOCS-V4-003` |
| 2026-06-05 | `ADR-DOCS-V4-004-NO-CLI.md` | كامِل | مَلف جَديد — إقرار حَذف CLI |
