---
id: ADR-DOCS-V4-002
title: قَرارات بنيوية موحَّدة لـ V4 — IDs والمسارات والـ Schema
status: Accepted
date: 2026-06-05
deciders:
  - المالك (صالح)
  - Amelia (Dev — مُنفِّذ)
authority: SoT
relatedStrategy: ../STRATEGY.md
relatedArchitecture: ../ARCHITECTURE.md
storyId: S-V4-M0-001
supersedes: []
context: تَعارضات داخلية بين STRATEGY و ARCHITECTURE اكتُشِفت في مُراجَعة 2026-06-05 — هذا ADR يَفصل القَرار النهائي.
---

# ADR-DOCS-V4-002 — قَرارات بنيوية موحَّدة لـ V4

## السياق (Context)

بعد إنشاء STRATEGY.md V4 + ARCHITECTURE.md V4 + README.md + ADR-DOCS-V4-001 في 2026-06-05، أَجرَت **Amelia** مُراجَعة ذاتية ناقدة كَشَفت **25 ملاحظة**. أكثرها خُطورة كانت **تَعارضات بنيوية داخلية** بين الوثائق (بنية مَجلَّدات مُختلفة، نَمط IDs مُختلف، Schema حقول مُختلفة، API بتَوقيعات مُختلفة).

هذا ADR يَختار **قَراراً واحداً حاسماً لكل تَعارُض** ويَكون المَرجع الأَوحد. ثم تُعدَّل بقية الوثائق لتُطابقه.

## القَرارات (Decisions)

### قَرار 1 — بنية مَجلَّد `language-truth/` (مُسطَّحة بلا `data/`)

**القَرار:** كل فِئات الكِيانات في مُجلَّدات مُباشرة تحت `language-truth/` — **بلا** مُجلَّد `data/` وَسيط.

```
language-truth/
├── keywords/
├── directives/
├── builtins/
├── operators/
├── types/
├── grammar_rules/
├── errors/                      # كل الأخطاء في مُجلَّد واحد مُسطَّح (لا parser/lexer/runtime subfolders)
├── stdlib_modules/
├── stdlib_functions/
├── _schemas/
├── _index.yaml                  # يُولَّد آلياً
├── include/                     # public API للـ library
├── src/                         # implementation
├── tools/                       # sadlang-info CLI
├── tests/
├── CMakeLists.txt
├── README.md
└── VERSIONING.md
```

**الأسباب:**
- بَساطة (CW-04: تَناسق بنيوي مع نَمط `shared/<x>/<y>` في المشروع الأم)
- اسم المُجلَّد = الفِئة → تَسهيل scripts وسريع
- لا حاجة لـ `data/` إذ كل ما تحت الجذر هو data
- مَجلَّد `errors/` مُسطَّح — التَصنيف (lexer/parser/...) يَعيش في حقل `subcategory` في YAML

### قَرار 2 — نَمط IDs الموحَّد

| الفِئة | البادئة | مثال |
|---|---|---|
| Keyword | `KW-` | `KW-FUNC-001` |
| Directive | `DIR-` (3 حرف) | `DIR-SIZE-001` |
| Builtin | `BI-` | `BI-PRINT-001` |
| Operator | `OP-` | `OP-PLUS-001` |
| Type | `TY-` | `TY-NUMBER-001` |
| Grammar Rule | `GR-` | `GR-FUNC-DECL-001` |
| Error | `E_<LAYER>_<CTX>_<NNN>` | `E_PAR_FUNC_001` |
| Stdlib Module | `MOD-` | `MOD-MATH-001` |
| Stdlib Function | `FN-<MOD>-<NAME>-NNN` | `FN-MATH-SQRT-001` |

**رَفض:** نَمط `DR-` (مُلتَبس مع Doctor)، `STD_MATH` (لا يَتبع بادئة-رقم)، `SF_SQRT` (نَفس السبب).
**تَأكيد:** ADR-DOCS-V4-001 §1 وَ ARCHITECTURE §1.2 — هذا القَرار يُلغي كل بَدائل أخرى في STRATEGY.

### قَرار 3 — Schema حقول YAML الموحَّد

> **⚠️ SUPERSEDED BY [ADR-DOCS-V4-003](ADR-DOCS-V4-003-CODEGEN-INTEGRATION.md) §قَرار 3+4 (2026-06-05):**
>
> هذا القَرار يَفترض ملف YAML واحد لكل كيان (`KW-FUNC-001.yaml`). V5 يَفرض ملف واحد لكل **نطاق** (`keywords.yaml` يَحوي 91 كيان داخلياً)، والـ IDs **حُقول داخلية في YAML** وليست أسماء ملفات. السبب: `gen_keywords.py` القائم يَتَوَقَّع `keywords.yaml` واحداً.
>
> الحقول نَفسها (`id`, `arabic`, `english`, `subcategory`, إلخ) **لا تَزال صَحيحة** — لكن مَوقعها في YAML تَغيَّر.

#### قَواعد عامة

```yaml
id: <PREFIX>-<NAME>-<NNN>          # إلزامي
schema_version: "1.0.0"             # إلزامي (يُحدِّد إصدار Schema)
since: "0.1.0"                      # إلزامي (إصدار اللُغة الذي ظهر فيه)
status: stable | experimental | deprecated  # إلزامي
deprecated_since: "0.5.0"           # اختياري
deprecation_reason_ar: "..."        # إلزامي إن deprecated
deprecation_reason_en: "..."        # إلزامي إن deprecated
replacement_id: <other-id>          # اختياري إن deprecated
```

#### نَمط الأَسماء العربية/الإنجليزية (مُسطَّح، ليس مُتشعِّباً)

```yaml
# ✅ مُعتمَد
arabic: دالة
english: function

# ❌ مَرفوض
names:
  ar: دالة
  en: function
```

**السبب:** أسهل في الاستهلاك من C++ (`entity.arabic` بدلاً من `entity.names["ar"]`)، وأقل تَعقيداً في schema.

#### حقول الكَلمة المفتاحية الكاملة

```yaml
id: KW-FUNC-001
schema_version: "1.0.0"
arabic: دالة
english: function
since: "0.1.0"
status: stable
subcategory: reserved              # reserved | contextual | builtin_type
token_type: KEYWORD_FUNCTION        # إن وُجد في Lexer
description_short_ar: تَعريف دالة قابلة لإعادة الاستخدام
description_short_en: Define a reusable function
aliases: []                         # بدائل مَقبولة (إن وُجدت)
ends_with: نهاية                    # كَيف تَنتَهي الكُتلة (إن انطَبق)
related_errors:
  - E_PAR_FUNC_001
related_grammar:
  - GR-FUNC-DECL-001
```

#### حقول الخَطأ الكاملة

```yaml
id: E_PAR_FUNC_001
schema_version: "1.0.0"
since: "0.1.0"
status: stable
subcategory: parser                # lexer | parser | semantic | runtime | type
severity: error                    # error | warning | info
message_ar: 'يَجب أن تَنتهي الدالة بـ "نهاية"'
message_en: 'Function must end with "نهاية"'
fix_suggestion_ar: 'أَضِف كلمة "نهاية" في نهاية الدالة'   # إلزامي ND-V4-7
fix_suggestion_en: 'Add "نهاية" keyword at the end of the function'  # إلزامي ND-V4-7
example_bad: |
  دالة جمع(أ، ب)
      ارجع أ + ب
example_good: |
  دالة جمع(أ، ب)
      ارجع أ + ب
  نهاية
related_keyword: KW-FUNC-001
```

**رَفض:** حقل `code` المُكرَّر لـ `id`. الـ `id` نَفسه هو الـ code.

### قَرار 4 — API توحيد التَوقيعات

#### Lookup APIs

```cpp
// (AR) كل lookup يَقبل id فقط (ليس name/symbol).
// (EN) Every lookup accepts id only (not name/symbol).
std::optional<KeywordEntity>        getKeyword(std::string_view id);
std::optional<DirectiveEntity>      getDirective(std::string_view id);
std::optional<BuiltinEntity>        getBuiltin(std::string_view id);
std::optional<OperatorEntity>       getOperator(std::string_view id);
std::optional<TypeEntity>           getType(std::string_view id);
std::optional<GrammarRuleEntity>    getGrammarRule(std::string_view id);
std::optional<ErrorEntity>          getError(std::string_view id);
std::optional<StdlibModuleEntity>   getStdlibModule(std::string_view id);
std::optional<StdlibFunctionEntity> getStdlibFunction(std::string_view id);
```

#### Search/Lookup-by-Name APIs (مُفصَّلة، صَريحة)

```cpp
// (AR) بَحث بالاسم العربي/الإنجليزي/الرَمز — مُفصَّل، صَريح.
// (EN) Search by Arabic/English/symbol — explicit, separate.
std::optional<KeywordEntity>  findKeywordByArabic(std::string_view arabic);
std::optional<KeywordEntity>  findKeywordByEnglish(std::string_view english);
std::optional<OperatorEntity> findOperatorBySymbol(std::string_view symbol);
std::optional<BuiltinEntity>  findBuiltinByArabic(std::string_view arabic);

bool isReservedKeyword(std::string_view arabic);
bool isContextualKeyword(std::string_view arabic);
```

#### Bulk APIs (تَعود بـ `const&`)

```cpp
const std::vector<KeywordEntity>&        getAllKeywords();
const std::vector<DirectiveEntity>&      getAllDirectives();
const std::vector<BuiltinEntity>&        getAllBuiltins();
// ... (بقية الفئات)

std::vector<KeywordEntity> getKeywordsBySubcategory(std::string_view subcat);  // إعادة نَسخ — مُفلتر
```

**رَفض:** `getKeyword(id_or_name)` المُغلَّف (ambiguous). يُفضَّل دائماً explicit (`findKeywordByArabic` / `getKeyword`).

### قَرار 5 — Binary Cache مُؤجَّل إلى M2 (BF-30: Profile Before Optimize)

> **⚠️ FULLY SUPERSEDED BY [ADR-DOCS-V4-003](ADR-DOCS-V4-003-CODEGEN-INTEGRATION.md) §قَرار 6 (2026-06-05):**
>
> Binary Cache **مُلغى نِهائياً** (ليس مُؤجَّلاً). البَديل المُعتَمد: **Build-Time Codegen → C++ Generated::** الذي يَتَفوَّق على Binary Cache في كل المَقاييس (0ms vs ~2ms، 0 runtime deps vs cache loader، freestanding works vs needs fileio).
>
> هذا القَرار كان يَفترض runtime YAML loading عبر yaml-cpp — V5 يُلغي هذا النَموذج بِالكامل.

**القَرار:** في M0+M1 نَقرأ YAML مُباشرةً عبر `yaml-cpp`. **لا** نُنشئ Binary Cache (`.stbin`) إلا في M2 وفقط **إذا** أَظهرت benchmarks فِعلية أن YAML بطيء (>10ms للتَحميل الكامل).

**التَفاصيل:**
- M0-M1: `Loader::loadAll()` يَفتح كل YAML، يُحلِّله، يَخزن في `unordered_map<id, Entity>`.
- نُقيس عند نهاية M1: زَمن تَحميل، حجم RAM.
- في M2: نُقرِّر هل Binary Cache يَستحق التَعقيد.

**السبب:** BF-30 + CW-30. الـ Binary Cache كان over-engineering مُبكِّر. YAML مَع yaml-cpp غالباً <50ms لـ516 ملف على SSD حديث — كافٍ للأدوات.

**أَثر هذا القَرار:**
- ARCHITECTURE §2.4 (Binary Cache) يُصبح "خَيار مُؤجَّل لـ M2 — انظر §10".
- CMakeLists لا يَحوي `langtruth-build` كَأداة خارجية في M0/M1.
- ARCHITECTURE §3 (Build Pipeline) يَصبح Loader-time pipeline بدلاً من build-time.

### قَرار 6 — أَرقام التَقدير الموحَّدة

| الفِئة | العَدد المُتَوقَّع |
|---|:---:|
| keywords (40 محجوزة + 25 سياقية + 9 أنواع) | **74** |
| directives | **10** |
| builtins | **25** |
| operators | **30** |
| types (مُختصَر — types مُدمجة في keywords أساساً) | **12** |
| grammar_rules | **50** |
| errors | **200** |
| stdlib_modules | **15** |
| stdlib_functions | **100** |
| **الإجمالي** | **~516** |

**رَفض:** أي أَرقام أُخرى. كل وثيقة تَستخدم 516 (لا 510، لا 500).

### قَرار 7 — CMake `langtruth-build` ليس أداة خارجية

> **⚠️ FULLY SUPERSEDED BY [ADR-DOCS-V4-003](ADR-DOCS-V4-003-CODEGEN-INTEGRATION.md) §قَرار 7 (2026-06-05):**
>
> `add_executable(langtruth-build)` **مُلغى نِهائياً** — لا حاجة لِأَداة C++ مُخَصَّصة. البَديل: استِخدام `cmake/codegen.cmake` القائم الذي يَستَدعي `${Python3_EXECUTABLE}` لِتَشغيل `scripts/codegen/gen_*.py`.
>
> السبب: نِظام codegen Python ناضج ومُختَبَر (273 سطر `gen_keywords.py`، 521 سطر `gen_builtins.py`، 417 سطر `gen_error_messages.py`) ولا حاجة لِإنشاء بَديل C++ مُوازِ (CW-19 DRY).

**القَرار:** أَداة build (إن وُجدت في M2) تَكون **target** في نفس البناء، تُستَدعى عبر `$<TARGET_FILE:langtruth-build>`. **لا** نَستخدم `find_program` كأنها أداة نِظام مُسبقة.

```cmake
# ✅ مُعتمَد
add_executable(langtruth-build tools/build_cache/main.cpp)
add_custom_command(
    OUTPUT ${LT_OUTPUT}
    COMMAND $<TARGET_FILE:langtruth-build> ...
    DEPENDS langtruth-build ${LT_INPUT}/keywords
)

# ❌ مَرفوض
find_program(LANGTRUTH_BUILD langtruth-build REQUIRED)
```

### قَرار 8 — `errors/` مُسطَّح، subcategory في YAML

> **⚠️ PARTIALLY SUPERSEDED BY [ADR-DOCS-V4-003](ADR-DOCS-V4-003-CODEGEN-INTEGRATION.md) §قَرار 3 (2026-06-05):**
>
> الفِكرة (subcategory في YAML) **صَحيحة وتَبقى نافذة**. لكن البُنية الفِعلية تَختَلِف:
> - V4 (ADR-002): `errors/<id>.yaml` لكل خَطأ مُنفَرِد
> - V5 (ADR-003): `errors/lexical.yaml`, `errors/syntactic.yaml`, إلخ — ملف لكل subcategory يَحوي عدة أخطاء داخلياً
>
> السبب: `error_messages.yaml` القائم (281.92 KB!) يُقَسَّم لـ ~8 ملفات منطقية وليس آلاف ملفات منفَرِدة.

**القَرار:** كل ملفات الأخطاء في `errors/` مُباشرةً (بلا `lexer/`, `parser/` subfolders). التَصنيف عبر حقل `subcategory: parser|lexer|...` في YAML.

**السبب:** يُسهِّل الـ globbing وlookup، ويَتفق مع نَمط بقية الفِئات. التَصنيف يَتم في الذاكرة بعد التَحميل.

## أَثر القَرارات (Impact)

كل من الوثائق التالية يَجب تَحديثها لتُطابق هذا ADR:

| الوثيقة | الأَقسام المُتَأثِّرة | الإجراء |
|---|---|---|
| [STRATEGY.md](../STRATEGY.md) | §2 (بنية), §3 (مسارات), §4 (API), §9 (أرقام), §11 (نَص نهاية مُتَناقض) | تَوحيد |
| [ARCHITECTURE.md](../ARCHITECTURE.md) | §1.1 (بنية), §2.2 (API), §2.4 (Binary Cache), §3 (Pipeline), §6 (CMake) | تَوحيد |
| [README.md](../README.md) | "الحالة الحالية" (ARCH صار مَوجوداً)، أمثلة الـ API | تَحديث |

## النَتائج (Consequences)

### إيجابية ✅

- ثَبات تَسمية IDs عبر كل الوثائق
- API نَظيف وَصَريح، بلا lookups مُلتَبسة
- BF-30 مُحتَرَم — لا over-engineering لـ Binary Cache
- بنية مُسطَّحة → سَهلة الـ globbing والـ tooling

### سَلبية / تَكلفة ⚠️

- يَتطلَّب تَعديل 3 ملفات (STRATEGY, ARCHITECTURE, README)
- Schema الكَلمة المفتاحية صار أَطول (أَكثر حقولاً) — لكن صَريح

### مَخاطر (Risks)

| المُخاطرة | الاحتمال | التَخفيف |
|---|:---:|---|
| schema حقول جديدة تُكسر YAML سابقة | منخفض (لم نَكتب YAML بعد) | لا شيء — لا توجد بيانات بعد |
| ARCH §2.4 يُبقي خَيار Binary Cache فيُربك | متوسط | إعادة كتابة §2.4 لتَصبح "تَأجيل وأَسباب" |

## التَتبُّع (Trace)

- يَخدُم: STRATEGY V4 (§2-§4, §9-§11), ARCHITECTURE V4 (§1-§3, §6)
- يَخدُم ستوري: S-V4-M0-001 (تَوحيد الـ Foundation)
- يَستبدل: لا شيء (تَكميلي)
- يَتقدَّم على: ADR-DOCS-V4-001 (يُكمِّله، لا يُلغيه)

## التَطبيق (Application Log)

| التاريخ | الملف | التَطبيق |
|---|---|---|
| 2026-06-05 | STRATEGY.md | تَطبيق قرارات 1, 2, 3, 4, 6 — بنية مُسطَّحة، IDs مُوحَّدة، schema (`arabic`/`english` مُسطَّحة)، API صَريح، 516 كَيان، إصلاح تَناقُض الخاتمة |
| 2026-06-05 | ARCHITECTURE.md | تَطبيق قرارات 3, 4, 5, 7, 8 — `CommonFields` + `subcategory` + `token_type`، API صَريح (`findKeywordByArabic/English/Symbol`, `getKeywordsBySubcategory`)، Binary Cache مُؤجَّل لـ M2 (§2.4 + §3 مُعَلَّمَين)، CMake `add_executable` + `$<TARGET_FILE:>` |
| 2026-06-05 | README.md | تَحديث ARCHITECTURE من "قَيد البناء" → "✅ Active"، إضافة سَطر ADR-002، تَحديث مثال YAML ليُطابق Schema الجديد، تَصحيح عدد الكَلمات (~74) |
| 2026-06-05 | ADR-DOCS-V4-001-APPROVAL.md | تَصحيح "الكلمات المفتاحية (40)" → "(~74)"، تَصحيح خَطأ مَطبَعي "ساعَتا الإطلاق" → "إصدار الإطلاق"، إضافة رَبط ADR-002 في Follow-Up |
