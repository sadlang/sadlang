---
id: DOCS-STRATEGY-V3
title: استراتيجية نظام التَوثيق — الحقيقة المُطلقة (Absolute Source of Truth)
version: V3-DRAFT
status: PROPOSAL-AWAITING-OWNER-APPROVAL
date: 2026-06-04
authority: SoT (يَستبدل كل الإصدارات السابقة)
supersedes:
  - _archive/2026-06-04_reset/STRATEGY_V1.md
  - _archive/2026-06-04_reset/STRATEGY_V2_DRAFT.md
authors:
  - 🏗️ Winston (Architect)
  - 📋 John (PM)
  - 💻 Amelia (Senior Dev)
  - 🔬 Dr. Quinn (Master Problem Solver)
based_on: مَلاحظة المالك 2026-06-04 — "نظام التَوثيق طبقة مُستقلة فوق الطبقة المُشتركة"
---

# 🎯 استراتيجية نظام التَوثيق V3 — الحقيقة المُطلقة

> **رؤية المالك (2026-06-04 — حرفياً):**
> "نظام التَوثيق هو الحقيقة الوحيدة المُطلقة. المشروع ضخم ولا يُمكن الاعتماد على التَوثيق البشري.
> طبقة التَوثيق يَجب أن تَكون طبقة مُستقلة تَكون فوق الطبقة المُشتركة بحيث كل مُكوِّنات الطبقة المُشتركة تَأخذ المعلومات من طبقة التَوثيق."

---

## 1. الفِكرة الجَوهرية — تَحوُّل جذري

### من النَموذج القديم إلى النَموذج الجديد

| البُعد | النَموذج القديم (V1/V2) | النَموذج الجَديد (V3) |
|---|---|---|
| **دور التَوثيق** | مَنتَج جانبي للكود | **مَصدر الحقيقة الذي يَتولَّد منه الكود** |
| **اتجاه التَدفُّق** | Code → Docs (يَدوي) | **Docs → Code** (آلي، استهلاكي) |
| **مَوقع التَوثيق** | مَلفات Markdown مَنفصلة | **طبقة هندسية فوق `shared/`** |
| **المُستهلِكون** | المُطوِّرون البشر فقط | **lsp، formatter، interpreter، compiler، tools، web** |
| **الإلزام** | اختياري — كل أداة تَكتفي بنفسها | **إلزامي — لا أداة تَحوي معرفة دون استشارة التَوثيق** |
| **التَكامل** | بَعد التَطوير (لاحقاً) | **قبل التَطوير (Docs-First Development)** |
| **التَحقُّق** | لا يوجد آلية تَحقُّق | **CI يَفشل إذا كانت أداة تَستخدم مَعرفة غير مُوثَّقة** |

### المعادلة الأساسية

```text
نظام التَوثيق ≠ مَلفات
نظام التَوثيق = طبقة هندسية + API + قاعدة بيانات + Quality Gates

كل مُكوِّن في المشروع:
   ➜ يَستهلك من نظام التَوثيق (read API)
   ➜ يَفشل في CI إذا لم يَستهلك (lint enforcement)
   ➜ لا يَحوي بيانات مَعرفية مَحلية (no hardcoded knowledge)
```

---

## 2. تَموقع طبقة التَوثيق في المعمارية

### مَخطَّط الطبقات الجَديد

```mermaid
flowchart TB
    subgraph L0["Layer 0: Runtime (Bare Metal)"]
        RT[runtime/]
    end
    subgraph L1["Layer 1: Shared (Core)"]
        LEX[shared/lexer/]
        PAR[shared/parser/]
        AST[shared/ast/]
        TYP[shared/types/]
        ERR[shared/errors/]
    end
    subgraph L2["⭐ Layer 2: Documentation (NEW — SoT)"]
        DOCS[documentation/<br/>الحقيقة المُطلقة]
        DOCS_API[documentation/api/<br/>read-only API]
        DOCS_DB[documentation/data/<br/>YAML registry]
        DOCS_LIB[documentation/lib/<br/>libsaddocs.a/dll]
    end
    subgraph L3["Layer 3: Engines"]
        INT[interpreter/]
        COMP[compiler/]
        VM[vm/]
    end
    subgraph L4["Layer 4: Tools"]
        LSP[tools/lsp/]
        FMT[tools/formatter/]
        PKG[tools/pkg/]
        REPL[tools/repl/]
    end
    subgraph L5["Layer 5: Distribution"]
        WEB[website/]
        CLI[CLI binaries]
        STD[stdlib/]
    end

    L0 --> L1
    L1 --> L2
    L2 -.->|"📖 read API<br/>(إلزامي)"| L3
    L2 -.->|"📖 read API<br/>(إلزامي)"| L4
    L2 -.->|"📖 read API<br/>(إلزامي)"| L5

    style L2 fill:#fc9,stroke:#f60,stroke-width:4px
    style DOCS fill:#fc9
    style DOCS_API fill:#fc9
    style DOCS_DB fill:#fc9
    style DOCS_LIB fill:#fc9
```

### قاعدة الاعتمادية الجَديدة (Dependency Rule)

```text
Layer N يُمكنه أن يَعتمد على Layer (N-1) فقط (بَنية تَقليدية)
+
كل Layer ≥ 3 يَجب أن يَعتمد على Layer 2 (Documentation)
+
Layer 2 لا يَعتمد على ما فوقها — يَعتمد فقط على Layer 1 (lexer/types)
```

---

## 3. ما هو "نظام التَوثيق" بدِقَّة؟

### تَعريف رَسمي

```yaml
نظام_التَوثيق:
  ماهيته:
    - قاعدة بيانات YAML مَركزية (data/)
    - مكتبة C++ ثابتة (lib/libsaddocs.a)
    - REST API (داخلي + خارجي)
    - CLI: sadinfo
    - WebSocket للتَحديث المُباشر
  ليست:
    - مَلفات Markdown مُتفرِّقة
    - وثائق وصفية
    - تَعليقات في الكود
  يَحوي:
    - كل keyword (40 محجوزة + ~25 سياقية)
    - كل built-in function (~21)
    - كل operator (+, -, ==, ...)
    - كل error code (compiler/runtime)
    - كل directive (@حجم، @ذري، ...)
    - كل grammar rule
    - كل type (built-in + user-defined)
    - كل stdlib module/function
    - أمثلة مُختبَرة لكل entity
    - تَمارين لكل مَفهوم
    - دروس مُترابطة
```

### بُنية البيانات

```mermaid
erDiagram
    ENTITY ||--o{ EXAMPLE : has
    ENTITY ||--o{ EXERCISE : covers
    ENTITY ||--o{ LESSON : taught_in
    ENTITY ||--o{ ERROR_CODE : may_emit
    ENTITY ||--o{ RELATED : related_to
    LESSON ||--o{ EXERCISE : includes
    LESSON ||--o{ EXAMPLE : illustrates
    
    ENTITY {
        string id PK
        string ar
        string en
        enum category
        string since
        string status
    }
    EXAMPLE {
        string id PK
        string entity_id FK
        text code
        text expected_output
        enum runner
        bool verified
    }
    EXERCISE {
        string id PK
        text problem
        text starter_code
        text solution
        json test_cases
        enum difficulty
    }
    LESSON {
        string id PK
        string title
        text content
        json prerequisites
        json covers
    }
    ERROR_CODE {
        string code PK
        string message_ar
        string message_en
        text explanation
        text fix_suggestion
    }
```

---

## 4. كيف يَستهلك كل مُكوِّن من نظام التَوثيق؟

### 4.1 المُفسِّر والمُترجم (Interpreter & Compiler)

```mermaid
sequenceDiagram
    participant U as User Code (.ص)
    participant LEX as Lexer
    participant DOCS as documentation/lib/<br/>libsaddocs.a
    participant ERR as Error Handler

    U->>LEX: المصدر
    LEX->>DOCS: get_keyword("دالة")
    DOCS-->>LEX: KEYWORD_FUNCTION + metadata
    LEX->>LEX: tokenize
    LEX->>ERR: خطأ على token غير معروف
    ERR->>DOCS: get_error_message("E_LEX_001")
    DOCS-->>ERR: رسالة مُترجَمة + اقتراحات إصلاح
    ERR-->>U: خطأ مَوحَّد
```

**القاعدة:** المُفسِّر لا يَحوي:
- قائمة كلمات مُفتاحية مَحلية → يَسأل `docs.get_keywords()`
- رسائل أخطاء مَحلية → يَسأل `docs.get_error("E_xxx")`
- قائمة built-ins → يَسأل `docs.get_builtins()`

### 4.2 LSP (Language Server Protocol)

```mermaid
sequenceDiagram
    participant IDE as VS Code
    participant LSP as sad-lsp
    participant DOCS as documentation API
    participant WS as WebSocket

    IDE->>LSP: hover on "دالة"
    LSP->>DOCS: GET /entity/KW-FUNC-001
    DOCS-->>LSP: full entity + examples
    LSP-->>IDE: hover content (ar/en)
    
    IDE->>LSP: completion request
    LSP->>DOCS: GET /completions?context=...
    DOCS-->>LSP: ranked suggestions
    LSP-->>IDE: items
    
    Note over DOCS,WS: عند تَحديث وثيقة
    DOCS->>WS: broadcast change
    WS->>LSP: invalidate cache
    LSP->>LSP: refetch lazily
```

**القاعدة:** LSP لا يَحوي:
- نصوص hover مَحلية → يَسأل `docs.get_hover(entity_id)`
- قواعد completion مَحلية → يَسأل `docs.get_completions(context)`
- signatures مَحلية → يَسأل `docs.get_signature(fn_id)`

### 4.3 المُنسِّق (Formatter)

```mermaid
flowchart LR
    A[ملف .ص] --> B[sad-fmt]
    B --> C{استشارة DOCS}
    C -->|قواعد التَنسيق| D[docs.get_format_rules]
    C -->|تَفضيلات لغوية| E[docs.get_style_guide]
    D --> F[تَطبيق التَنسيق]
    E --> F
    F --> G[ملف .ص مُنسَّق]

    style C fill:#fc9
```

**القاعدة:** المُنسِّق لا يَحوي:
- قواعد المسافات → `docs.get_format_rules("spacing")`
- قواعد ترتيب الكلمات → `docs.get_format_rules("ordering")`
- قواعد التَعليقات → `docs.get_format_rules("comments")`

### 4.4 الموقع (Website)

```mermaid
flowchart LR
    DOCS[(documentation/data/<br/>YAML)] --> SDK[sadinfo --dump-all]
    SDK --> JSON[docs.json]
    JSON --> SSG[VitePress build]
    SSG --> HTML[Static site]
    HTML --> CDN[CDN]
    
    DOCS -.->|live updates| API[REST API]
    API -.-> WEB_RT[Web Runtime<br/>playground]

    style DOCS fill:#fc9
```

**القاعدة:** الموقع لا يَحوي:
- محتوى مَكتوب يَدوياً → كل صفحة تُولَّد من entity
- أمثلة مُكرَّرة → الأمثلة تَأتي من YAML المُختبَر
- تَمارين خاصة → التَمارين entities من نفس النظام

### 4.5 الأدوات الأخرى (Package Manager, REPL, sad-doc, sad-test)

نفس القاعدة: لا أداة تَحوي مَعرفة عن اللغة دون استشارة `documentation/`.

---

## 5. مُكوِّنات طبقة `documentation/`

### البنية الكاملة

```text
documentation/                                  ⭐ طبقة جَديدة
├── CMakeLists.txt
├── data/                                       (قاعدة البيانات — YAML)
│   ├── keywords/
│   │   ├── KW-FUNC-001.yaml                    # دالة
│   │   ├── KW-CLASS-001.yaml                   # صنف
│   │   └── ... (65 ملف)
│   ├── builtins/
│   │   ├── BI-PRINT-001.yaml                   # اطبع
│   │   ├── BI-LEN-001.yaml                     # طول
│   │   └── ... (~21 ملف)
│   ├── operators/
│   │   ├── OP-PLUS-001.yaml
│   │   └── ... (~15 ملف)
│   ├── directives/
│   │   ├── DR-SIZE-001.yaml                    # @حجم
│   │   └── ... (~10 ملفات)
│   ├── errors/
│   │   ├── lexer/E_LEX_001.yaml
│   │   ├── parser/E_PAR_001.yaml
│   │   └── runtime/E_RT_001.yaml
│   ├── types/
│   │   └── TY-INT-001.yaml                     # رقم
│   ├── grammar/
│   │   └── G_EXPR.yaml                         # قواعد نحوية
│   ├── stdlib/
│   │   └── STD_MATH.yaml
│   ├── lessons/                                # دروس (lessons)
│   │   ├── L-001-hello-world.yaml
│   │   └── ...
│   └── exercises/                              # تَمارين (exercises)
│       ├── EX-001-fibonacci.yaml
│       └── ...
├── schema/                                     (JSON Schema للتَحقُّق)
│   ├── entity.schema.json
│   ├── example.schema.json
│   ├── exercise.schema.json
│   └── lesson.schema.json
├── lib/                                        (مكتبة C++)
│   ├── include/
│   │   ├── sad/docs/api.h                      # public API
│   │   ├── sad/docs/entity.h
│   │   └── sad/docs/loader.h
│   └── src/
│       ├── api.cpp
│       ├── loader.cpp
│       ├── yaml_parser.cpp
│       └── cache.cpp
├── api/                                        (REST/GraphQL server)
│   ├── server.cpp
│   ├── routes/
│   └── websocket.cpp
├── cli/                                        (sadinfo tool)
│   └── main.cpp
├── tests/
│   ├── unit/
│   ├── integration/
│   └── consumer_compliance/                    # تَأكيد أن كل أداة تَستهلك
└── README.md
```

### واجهة C++ المُوحَّدة (Public API)

```cpp
// documentation/lib/include/sad/docs/api.h
namespace Sad::Docs {

class DocsAPI {
public:
    // Singleton — مَصدر وحيد للحقيقة في process
    static DocsAPI& instance();

    // === Keywords ===
    std::optional<KeywordEntity> getKeyword(std::string_view ar_name);
    std::vector<KeywordEntity> getAllKeywords();
    bool isReservedKeyword(std::string_view name);

    // === Built-ins ===
    std::optional<BuiltinEntity> getBuiltin(std::string_view ar_name);
    std::vector<BuiltinEntity> getAllBuiltins();

    // === Errors ===
    ErrorMessage getError(std::string_view code, Locale = Locale::AR);
    std::vector<std::string> getFixSuggestions(std::string_view code);

    // === Formatting rules ===
    FormatRules getFormatRules();
    StyleGuide getStyleGuide();

    // === Examples ===
    std::vector<Example> getExamples(std::string_view entity_id);

    // === Generic ===
    std::optional<Entity> getEntity(std::string_view id);
    std::vector<Entity> queryEntities(const Query& q);

    // === Hot reload ===
    void reloadFromDisk();
    void subscribeToChanges(std::function<void(EntityId)> cb);
};

} // namespace Sad::Docs
```

### نَموذج الاستخدام (في lexer مثلاً)

```cpp
// shared/lexer/src/lexer_keywords.cpp — قَبل V3
void KeywordTable::initialize() {
    table_["دالة"] = TokenType::KEYWORD_FUNCTION;
    table_["صنف"] = TokenType::KEYWORD_CLASS;
    // ... 38 سطراً يَدوياً
}

// shared/lexer/src/lexer_keywords.cpp — بَعد V3
#include "sad/docs/api.h"

void KeywordTable::initialize() {
    auto& docs = Sad::Docs::DocsAPI::instance();
    for (const auto& kw : docs.getAllKeywords()) {
        if (kw.is_reserved) {
            table_[kw.ar] = mapToTokenType(kw.id);
        }
    }
}
// النَتيجة: إضافة كلمة مُفتاحية جَديدة = إضافة ملف YAML واحد فقط
// لا تَعديل لـ lexer_keywords.cpp مَطلقاً
```

---

## 6. Quality Gates للنَموذج الجَديد

### G1-G7 (مَوروثة من V2) + G8-G12 (جَديدة)

| Gate | الفحص | كيف يُنفَّذ |
|---|---|---|
| **G8: Consumer Compliance** | لا أداة تَحوي knowledge مَحلية | `lint-no-hardcoded-knowledge.py` يَبحث عن hardcoded keywords/errors |
| **G9: Bidirectional Trace** | كل entity في docs مُستهلَك من ≥1 أداة | `trace-consumers.py` يَتَحقَّق |
| **G10: API Stability** | كسر API يُكتشَف قبل merge | semver check + binary compatibility |
| **G11: Schema Conformance** | كل YAML يُطابق schema | `ajv validate` أو `yamale` |
| **G12: Example Verification** | كل مثال يُختبَر | `sad` + `sadc` على .ص المُستخرَج |

### CI Workflow

```mermaid
flowchart TB
    PR[Pull Request] --> G1[G1: Schema Conformance]
    G1 --> G2[G2: Example Verification]
    G2 --> G3[G3: Consumer Compliance Scan]
    G3 --> G4[G4: Build all tools]
    G4 --> G5[G5: Run all tests]
    G5 --> G6[G6: API Stability check]
    G6 --> G7[G7: Doc generation pipeline]
    G7 --> G8[G8: Deploy to staging]
    G8 --> MERGE[✅ Merge allowed]

    G1 -->|✗| FAIL[❌ Block PR]
    G2 -->|✗| FAIL
    G3 -->|✗| FAIL
    G4 -->|✗| FAIL
    G5 -->|✗| FAIL
    G6 -->|✗| FAIL

    style MERGE fill:#9f9
    style FAIL fill:#f99
```

---

## 7. خَطة التَنفيذ (Roadmap)

### المراحل (8 أشهر)

```mermaid
gantt
    title V3 Implementation Roadmap
    dateFormat YYYY-MM-DD
    section Foundation
    M0 Pre-work :m0, 2026-06-05, 2w
    M1 documentation/data + schema :m1, after m0, 4w
    M2 documentation/lib (C++ API) :m2, after m1, 4w
    section Integration
    M3 Refactor lexer (consumer #1) :m3, after m2, 3w
    M4 Refactor parser + errors :m4, after m3, 4w
    M5 Refactor LSP :m5, after m4, 4w
    M6 Refactor formatter + tools :m6, after m5, 3w
    section Distribution
    M7 Build pipeline + website :m7, after m6, 4w
    M8 Server + playground :m8, after m7, 4w
```

### مَهام كل مَرحلة

| Milestone | المُخرَجات الرئيسية | شُروط القَبول |
|---|---|---|
| **M0 Pre-work** | اعتماد V3، 5 ADRs، archive V1/V2 | المالك يُوقِّع |
| **M1 Data Layer** | YAML schemas + 65 keyword + 21 builtin | كل YAML يَجتاز G11 |
| **M2 Library** | libsaddocs.a + sadinfo CLI | unit tests 100% |
| **M3 Lexer Refactor** | lexer يَستهلك من docs | G8 يَنجح للـ lexer |
| **M4 Parser+Errors** | parser + error messages من docs | G8 يَنجح للـ parser |
| **M5 LSP** | sad-lsp يَستهلك من docs | hover/completion من docs |
| **M6 Tools** | formatter + repl + pkg | G8 يَنجح للكل |
| **M7 Web** | الموقع يَتَولَّد من docs | 100% pages من YAML |
| **M8 Playground** | server + docker + WS | end-to-end exercise |

---

## 8. القَرارات غير القابلة للتَفاوض (Non-Negotiable Decisions)

| # | القَرار | السبب |
|---|---|---|
| **ND-1** | طبقة `documentation/` تَأتي قبل كل Layer ≥ 3 | لا توجد مَعرفة خارج النظام |
| **ND-2** | YAML = صيغة وحيدة لـ source of truth | machine-readable + human-editable |
| **ND-3** | C++ API = الواجهة الوحيدة للقراءة في process | لا يَوجد parsing مُكرَّر |
| **ND-4** | REST API = الواجهة الوحيدة للقراءة عن بُعد | uniform interface |
| **ND-5** | CI يَفشل إذا أداة تَحوي knowledge مَحلية | enforce SoT |
| **ND-6** | كل entity يَحوي ≥1 مثال مُختبَر | لا توثيق نَظري |
| **ND-7** | كل error يَحوي fix_suggestion | تَجربة المُستخدِم |
| **ND-8** | hot-reload إلزامي (no restart) | تَجربة المُطوِّر |

---

## 9. القَرارات المُؤجَّلة (Deferred — لـ ADR لاحق)

| # | القَرار | لماذا مُؤجَّل |
|---|---|---|
| **DD-1** | اختيار YAML parser (yaml-cpp vs rapidyaml) | بَنشمارك في M1 |
| **DD-2** | GraphQL مع REST أم بدلاً منه | بَعد M7 بناءً على usage |
| **DD-3** | WebSocket vs SSE للـ live updates | بَعد M5 |
| **DD-4** | تَنسيق التَخزين الداخلي (LMDB vs SQLite vs in-memory) | بَعد قياس حجم البيانات |

---

## 10. مَقاييس النَجاح (Success Metrics)

| مَقياس | M1 | M3 | M5 | M8 |
|---|:---:|:---:|:---:|:---:|
| # entities في docs | 65 | 100 | 146 | 200+ |
| # consumers مُتَكامِلين | 0 | 2 (lex/par) | 4 (+lsp/fmt) | 7+ |
| # hardcoded knowledge violations | N/A | < 10 | < 5 | 0 |
| latency: `getKeyword()` | N/A | < 1ms | < 100µs | < 50µs |
| latency: `--dump-all` | < 2s | < 1s | < 500ms | < 200ms |
| # examples verified | 65 | 200 | 400 | 584+ |
| # exercises | 0 | 20 | 100 | 200+ |
| CI Quality Gates | 5 | 8 | 10 | 12 |

---

## 11. المَخاطر الرئيسية

```mermaid
quadrantChart
    title مَخاطر V3 (تَأثير × احتمال)
    x-axis احتمال منخفض --> احتمال عالٍ
    y-axis تَأثير منخفض --> تَأثير عالٍ
    quadrant-1 مَراقبة دقيقة
    quadrant-2 مُعالجة فورية
    quadrant-3 قَبول
    quadrant-4 تَخطيط
    "Refactor lexer يَكسر اختبارات": [0.7, 0.8]
    "API stability في M1-M2": [0.6, 0.9]
    "أداء consumer-side": [0.5, 0.7]
    "حجم YAML database": [0.3, 0.5]
    "تَعقيد hot-reload": [0.4, 0.6]
    "مُقاومة الفريق للتَغيير": [0.3, 0.8]
    "كسر تَوافُق ABI": [0.2, 0.9]
```

### استراتيجيات التَخفيف

| المُخاطرة | استراتيجية |
|---|---|
| Refactor lexer يَكسر اختبارات | M3 لها 3 أسابيع + 100% test coverage قبل البدء |
| API stability | semver + binary compat tests من M2 |
| أداء consumer-side | benchmarks في M2، target < 100µs |
| تَعقيد hot-reload | البدء بـ cold reload في M2، hot في M8 |
| كسر ABI | freeze API بَعد M3 |

---

## 12. الستوريات الأولى (Sprint 1 المُقترَح)

| Story ID | العنوان | Effort |
|---|---|:---:|
| **S-V3-001** | اعتماد V3 + archive V1/V2 + تَوقيع المالك | XS |
| **S-V3-002** | إنشاء `_INTEROP_ONTOLOGY.md` لتَنظيم الأنظمة | M |
| **S-V3-003** | كتابة 5 ADRs (ND-1 إلى ND-5) | M |
| **S-V3-004** | تَصميم schema/entity.schema.json | S |
| **S-V3-005** | تَصميم schema/example.schema.json + exercise + lesson | S |
| **S-V3-006** | كتابة أول 10 entities (KW-FUNC، KW-CLASS، ...) | M |
| **S-V3-007** | إنشاء `documentation/lib/include/sad/docs/api.h` (interface فقط) | M |
| **S-V3-008** | كتابة `lint-no-hardcoded-knowledge.py` (G8 prototype) | S |

---

## 13. مَراجع

- [_archive/2026-06-04_reset/STRATEGY_V1.md](_archive/2026-06-04_reset/STRATEGY_V1.md) — مُؤرشَف
- [_archive/2026-06-04_reset/STRATEGY_V2_DRAFT.md](_archive/2026-06-04_reset/STRATEGY_V2_DRAFT.md) — مُؤرشَف
- [_archive/PARTY_MODE_CRITIQUE_2026-06-02.md](_archive/PARTY_MODE_CRITIQUE_2026-06-02.md) — مَصدر إجماع Round 2

---

## 14. سِجل التَغييرات

| التاريخ | الإصدار | الكاتب | التَغيير |
|---|---|---|---|
| 2026-06-04 | V3-DRAFT | مَلاحظة المالك + 4 وكلاء | إعادة بناء جَذرية — طبقة `documentation/` كَ SoT مُطلق |

---

**انتهت V3-DRAFT — تَنتظر اعتماد المالك.**
