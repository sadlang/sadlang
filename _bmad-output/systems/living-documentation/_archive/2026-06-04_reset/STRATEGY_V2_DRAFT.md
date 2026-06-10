---
title: "استراتيجية نظام التوثيق الحي — V2 (بعد إجماع الجولة 2)"
type: canonical-strategy-v2
system: living-documentation
date: 2026-06-04
status: PROPOSAL-AWAITING-OWNER-APPROVAL
authority: REPLACES-V1-IF-APPROVED
supersedes_candidate:
  - STRATEGY.md (V1 — 2026-06-02)
based_on:
  - _archive/PARTY_MODE_CRITIQUE_2026-06-02.md (Round 2 consensus)
  - _scratch/_owner_context_round2.md (سياق المالك)
governance_rule: GR-DOCS-CANONICAL
authors_consensus:
  - "Winston (🏗️ Architect) — Round 2"
  - "John (📋 PM) — Round 2"
  - "Amelia (💻 Senior Dev) — Round 2"
  - "Dr. Quinn (🔬 Problem Solver) — Round 2"
---

# 📋 استراتيجية نظام التوثيق الحي — V2

> **هذه الوثيقة مُقترَحة لاستبدال [STRATEGY.md](STRATEGY.md) (V1)** بعد إجماع وكلاء الجولة 2 على 9 تَحَوّلات جوهرية. تَتضمن **مخططات معمارية رسومية** + **توصيات قابلة للتنفيذ** + **مَقاييس قابلة للقياس**.
>
> **ليست SoT حتى يَعتمدها المالك صراحةً.**

---

## 0. ملخَّص تنفيذي (TL;DR)

> **التَحَوّل الأكبر:** المشكلة ليست "drift" — هي **"context loss" للوكلاء + المالك "يَتوه"** في مشروع كبير 100% مَكتوب بـ AI agents. الحل ليس فقط SoT واحد — هو **خريطة قابلة للقراءة آلياً تَضمن اتساق المعرفة عبر جميع المُنتجِين/المُستهلِكين (AI)**.

| البُعد | V1 (الأصلية) | V2 (المُقترَحة) |
|---|---|---|
| **المُستهلِك الأساسي** | "5 جهات استهلاك (موقع، LSP، …)" | **المالك + وكلاء AI** (لا بَشر آخرون) |
| **المشكلة الأساسية** | Drift بين الكود والتوثيق | **Context loss للوكلاء** + المالك يَتوه |
| **حجم MVP** | "≤100 entry تدريجي" | **130–150 entry شامل في M1–M3** |
| **`sadinfo`** | Binary مستقل | **مكتبة `shared/sadinfo_core/` + CLI رقيق** |
| **آلية التَوليد** | `cmake codegen` فقط | **`runtime YAML loader`** (أقل cmake errors) |
| **Quality Gates** | "هدف نوعي" | **CI workflows آلية قبل M1** (G1–G7) |
| **النشر** | gh-pages + سيرفر | **سيرفر خاص فقط** (استقلالية معمارية) |
| **مَكان النظام** | نظام مستقل | **جزء من معمارية متعددة الأنظمة** + ontology |
| **اختبار ACs** | "نص حر" | **Given/When/Then صارم** لكل AC |
| **DoD المُؤجَّلة** | مَسموح | **مَمنوع** (وكيل AI بلا ذاكرة بعد sprint) |

---

## 1. السياق الجديد — لماذا V2؟

### 1.1 الحقائق التي كَشَفها المالك (لم تَكن مَعروفة في V1)

```mermaid
mindmap
  root((سياق المالك الحقيقي))
    الكُتَّاب
      100% وكلاء AI
      لا مساهمون بشر
      حتى هذه المناقشات مع وكلاء
    المشكلة الفعلية
      وكلاء يفقدون السياق
      المشروع كبير جداً
      المالك يَتوه
      ما الموجود وما الغير موجود
    التَجربة السابقة
      جَرَّب بدون قواعد
      فشل في الاتساق
      المزامنة اليدوية مُتعِبة
    التَوازن الاقتصادي
      بناء النظام مرة قليل التكلفة
      شهر عمل ضائع غالي
      ROI موجب بأي خطأ كبير
    الرؤية الأشمل
      أنظمة متعددة
      types errors docs
      كل نظام SoT منفصل
      ontology موحَّد
```

### 1.2 ما تَغيَّر في تَقييم الوكلاء بين الجولتين؟

| الوكيل | الجولة 1 (قَبل السياق) | الجولة 2 (بعد السياق) |
|---|---|---|
| 🏗️ Winston | "gh-pages كـ fallback" | **❌ مرفوض** — السيرفر الخاص قرار صحيح معمارياً |
| 📋 John | "MVP ≤100 entry" | **❌ منهار** — شمول 130–150 مرة واحدة أفضل |
| 💻 Amelia | "ACs ناقصة" | **⬆️ حَرِجة 1000 مرة** — وكلاء AI بدون Given/When/Then = chaos |
| 🔬 Quinn | "حافز مالي لكُتَّاب التوثيق" | **❌ انهيار كامل** — لا بشر يَكتبون |
| 🔬 Quinn | "X-Macros أبسط من YAML" | **❌ خاطئ** — YAML أنسب لوكلاء AI |
| 🔬 Quinn | "sadinfo binary غير ضروري" | **🟡 تَعديل** — حدود واضحة للوكلاء مفيدة |

---

## 2. الرؤية المُحدَّثة (V2)

> **بحلول Q2 2027:** نظام التوثيق الحي يَخدم **المالك + وكلاء AI** كَخريطة قابلة للقراءة آلياً تَجيب على السؤالين الجوهريين:
>
> 1. **"ما هو موجود؟"** — entity واحد يُمثل كل keyword/builtin/error/directive
> 2. **"ما هو غير موجود؟"** — Quality Gate يَكشف الفجوات تلقائياً
>
> النظام جزء من **معمارية متعددة الأنظمة** (`systems/living-doc/` + `systems/type-system/` + `systems/error-system/` + …) تَتشارك ontology موحَّد عبر `_INTEROP_ONTOLOGY.md`.

---

## 3. المعمارية الجديدة — المَنظور العام

### 3.1 السياق النظامي (System Context)

```mermaid
flowchart TB
    subgraph PRODUCERS["🤖 المُنتِجون"]
        OWNER["👤 المالك<br/>قرارات معمارية"]
        AI_AGENTS["🤖 وكلاء AI<br/>كود + توثيق"]
    end

    subgraph SOT["📦 SoT الموحَّد"]
        YAML["📄 data/keywords/<br/>data/builtins/<br/>data/errors/<br/>data/directives/<br/>YAML files"]
    end

    subgraph CORE["⚙️ نواة النظام"]
        LIB["📚 shared/sadinfo_core/<br/>EntityLoader + Validator + Exporter"]
        CLI["🔧 tools/sadinfo/<br/>CLI رقيق فوق المكتبة"]
        QG["🛡️ Quality Gates G1-G7<br/>CI Workflows آلية"]
    end

    subgraph CONSUMERS["📥 المُستهلِكون"]
        OWNER_VIEW["👤 المالك<br/>ما الموجود؟ ما الناقص؟"]
        LSP["🔍 LSP<br/>API مُباشِر بدون subprocess"]
        WEBSITE["🌐 الموقع<br/>JSON IR"]
        AI_CONSUME["🤖 وكلاء AI<br/>قراءة قبل كل مهمة"]
        RUNTIME["⚡ Runtime<br/>KeywordTable::loadFromYAML"]
    end

    subgraph OTHER["🔗 أنظمة أخرى مُخطَّطة"]
        TYPE["systems/type-system/"]
        ERR["systems/error-system/"]
        ONTO["_INTEROP_ONTOLOGY.md<br/>(يَربط الأنظمة)"]
    end

    OWNER -->|"يُعرِّف schemas"| YAML
    AI_AGENTS -->|"يُضيفون entities"| YAML
    YAML -->|"تَقرأ"| LIB
    LIB -->|"يَستخدم"| CLI
    LIB -->|"يَفحص"| QG
    QG -->|"تَمنع merge"| YAML

    LIB -->|"API"| LSP
    LIB -->|"loadFromYAML"| RUNTIME
    CLI -->|"--export-json"| WEBSITE
    CLI -->|"--dump-all"| AI_CONSUME
    AI_CONSUME -->|"يَستخدم سياق"| AI_AGENTS
    OWNER_VIEW -.->|"يَستعرض"| CLI

    ONTO -.->|"يَربط"| SOT
    ONTO -.->|"يَربط"| TYPE
    ONTO -.->|"يَربط"| ERR

    style YAML fill:#f9f,stroke:#333,stroke-width:3px
    style LIB fill:#bbf,stroke:#333,stroke-width:2px
    style QG fill:#fbb,stroke:#333,stroke-width:2px
    style ONTO fill:#bfb,stroke:#333,stroke-width:2px
```

### 3.2 تَدفُّق البيانات (Data Flow)

```mermaid
sequenceDiagram
    autonumber
    participant Agent as 🤖 وكيل AI
    participant YAML as 📄 data/*.yaml
    participant Lib as 📚 sadinfo_core
    participant CI as 🛡️ CI (Quality Gates)
    participant Runtime as ⚡ Runtime
    participant Web as 🌐 الموقع
    participant OtherAgent as 🤖 وكيل آخر

    Note over Agent: مهمة جديدة: إضافة keyword
    Agent->>Lib: dump_all() قبل البَدء
    Lib->>YAML: قراءة كل entities
    Lib-->>Agent: JSON IR (سياق كامل)

    Note over Agent: الوكيل يَعرف السياق ✓
    Agent->>YAML: إضافة entity جديد (YAML)
    Agent->>CI: git push (PR)

    CI->>Lib: validate(جميع entities)
    Lib->>Lib: فحص G1-G7
    alt Quality Gates تَنجح
        CI-->>Agent: ✅ approved
        CI->>Web: نَشر JSON IR
        CI->>Runtime: تَحديث data/*.yaml
        Note over Runtime: KeywordTable::loadFromYAML<br/>عند startup
    else Quality Gates تَفشل
        CI-->>Agent: ❌ rejected with diagnostics
        Note over Agent: يَفهم الخطأ ويُصلح
    end

    Note over OtherAgent: لاحقاً: وكيل آخر يَبدأ مهمة
    OtherAgent->>Lib: dump_all()
    Lib-->>OtherAgent: JSON IR (بـ entity الجديد)
    Note over OtherAgent: لا context loss ✓
```

### 3.3 طبقات النظام (Layered Architecture)

```mermaid
flowchart TB
    subgraph L1["🟦 الطبقة 1 — Storage Layer"]
        L1A["data/keywords/*.yaml"]
        L1B["data/builtins/*.yaml"]
        L1C["data/errors/*.yaml"]
        L1D["data/directives/*.yaml"]
        L1S["data/_schemas/*.json"]
    end

    subgraph L2["🟩 الطبقة 2 — Core Library (shared/sadinfo_core/)"]
        L2A["EntityLoader<br/>YAML → Entity"]
        L2B["SchemaValidator<br/>Tier 1/2/3 checks"]
        L2C["EntityRepository<br/>in-memory store"]
        L2D["EntityExporter<br/>Entity → JSON IR"]
    end

    subgraph L3["🟨 الطبقة 3 — Consumers"]
        L3A["tools/sadinfo/<br/>(CLI)"]
        L3B["Runtime<br/>KeywordTable::loadFromYAML"]
        L3C["LSP<br/>(in-process)"]
        L3D["Quality Gates<br/>(CI scripts)"]
    end

    subgraph L4["🟥 الطبقة 4 — Outputs"]
        L4A["sadinfo.json<br/>(JSON IR)"]
        L4B["website/data/<br/>(JSON للموقع)"]
        L4C["CI reports<br/>(violations)"]
        L4D["AI context dumps"]
    end

    L1 --> L2A
    L1S --> L2B
    L2A --> L2C
    L2B --> L2C
    L2C --> L2D
    L2 --> L3A
    L2 --> L3B
    L2 --> L3C
    L2 --> L3D
    L3A --> L4A
    L3A --> L4B
    L3D --> L4C
    L3A --> L4D

    style L1 fill:#cce,stroke:#003,stroke-width:2px
    style L2 fill:#cec,stroke:#030,stroke-width:2px
    style L3 fill:#eec,stroke:#330,stroke-width:2px
    style L4 fill:#ecc,stroke:#300,stroke-width:2px
```

---

## 4. الأهداف الجديدة — G1 إلى G7 (مُعاد صياغتها)

### 4.1 إعادة الصياغة الجوهرية

| الهدف V1 | V1 (نوعي) | الهدف V2 | V2 (قابل للقياس) |
|---|---|---|---|
| G1: SoT | "توحيد المصدر" | **G1: Consistency** | كل entity له ID وحيد عبر النظام بالكامل |
| G2: استحالة drift | "بنيوياً" | **G2: Auditability** | كل تَعديل في `data/*.yaml` يَنتج git log واضح + diff JSON IR |
| G3: ثنائية لغة | "AR + EN" | **G3: AI-Readability** | كل entity يَحوي 6 حقول إلزامية: `id`, `ar`, `en`, `category`, `since`, `examples` |
| G4: سرعة بناء | "<2s" | **G4: Automation** | بدون أي manual sync — كل sync يَتم عبر CI |
| G5: إمكانية وصول | "WCAG 2.1 AA" | **G5: Completeness** | 130–150 entry محدَّد + Quality Gate يَكشف الفجوات |
| G6: توزيع مستقر | "99.9% uptime" | **G6: Trust** | Quality Gates G1–G7 تَمنع merge للتَناقضات |
| G7: flake rate | "<0.1%" | **G7: Stability** | CI green على كل commit في `data/*.yaml` |

### 4.2 آلية القياس (Quality Gates Implementation)

```mermaid
flowchart LR
    PR[🔄 PR لـ data/*.yaml] --> G1{G1: Consistency<br/>ID فريد؟}
    G1 -->|✓| G2{G2: Auditability<br/>git log + diff؟}
    G1 -->|✗| FAIL[❌ Reject]
    G2 -->|✓| G3{G3: AI-Readable<br/>6 حقول إلزامية؟}
    G2 -->|✗| FAIL
    G3 -->|✓| G4{G4: Automation<br/>لا manual edit في .cpp؟}
    G3 -->|✗| FAIL
    G4 -->|✓| G5{G5: Completeness<br/>category في whitelist؟}
    G4 -->|✗| FAIL
    G5 -->|✓| G6{G6: Trust<br/>schema validation ✓؟}
    G5 -->|✗| FAIL
    G6 -->|✓| G7{G7: Stability<br/>CI green على main؟}
    G6 -->|✗| FAIL
    G7 -->|✓| PASS[✅ Approved + Auto-Deploy]
    G7 -->|✗| FAIL

    style PASS fill:#9f9
    style FAIL fill:#f99
```

---

## 5. نطاق MVP الجديد (M1–M3)

### 5.1 ما الذي يَدخل في MVP؟

```mermaid
pie title توزيع 130-150 entry في MVP
    "Keywords مَحجوزة (40)" : 40
    "Keywords سياقية (25)" : 25
    "Builtins (21)" : 21
    "Errors Lexer (10)" : 10
    "Errors Parser (15)" : 15
    "Errors Runtime (10)" : 10
    "Directives (~10)" : 10
    "Operators (~15)" : 15
```

**الإجمالي: ~146 entry محدَّد** (ليس مفتوح النهاية)

### 5.2 خريطة المعالم الجديدة

```mermaid
gantt
    title المعالم M1-M7 (8 أشهر)
    dateFormat YYYY-MM-DD
    section Pre-M1
    _INTEROP_ONTOLOGY.md         :crit, p1, 2026-06-05, 7d
    sadinfo_core library         :crit, p2, after p1, 14d
    Quality Gates G1-G7 CI       :crit, p3, after p2, 7d
    Baseline performance         :crit, p4, after p1, 14d

    section M1 (Foundation)
    Schema validator + YAML init :m1a, after p3, 14d
    Keywords + builtins (~86)    :m1b, after m1a, 21d

    section M2 (Errors)
    Errors lexer + parser (~25)  :m2a, after m1b, 14d
    Errors runtime (~10)         :m2b, after m2a, 7d

    section M3 (Directives + Operators)
    Directives + Operators (~25) :m3a, after m2b, 14d

    section M4 (Website)
    JSON IR exporter             :m4a, after m3a, 14d
    Website integration          :m4b, after m4a, 21d

    section M5 (LSP)
    LSP hover docs               :m5, after m4b, 14d

    section M6 (Editor)
    VS Code snippets             :m6, after m5, 14d

    section M7 (Polish)
    AI training format           :m7, after m6, 14d
```

---

## 6. الاستوريات (Story Format V2)

### 6.1 قالب AC جديد — Given/When/Then صارم

```yaml
# قبل (V1) - غامض
acceptance_criteria:
  - "الملفات تُرفع بنجاح"
  - "الفحص يَعمل"

# بعد (V2) - قابل للأتمتة
acceptance_criteria:
  - id: AC-1
    given: "ملف data/keywords/IF_01.yaml جديد بـ ID فريد"
    when: "تَشغيل sadinfo --validate data/keywords/"
    then:
      - "stdout يَحتوي 'VALIDATED: 1/1 entries'"
      - "exit code == 0"
    automated: true
    test_command: "scripts/test_ac.sh STORY-001 AC-1"
```

### 6.2 ما هو مَمنوع في V2؟

```mermaid
flowchart LR
    A[ستوري جديدة] --> B{هل DoD مُكتمل<br/>في هذه الستوري؟}
    B -->|لا — مُؤجَّل لـ Sprint قادم| C[❌ مَرفوض<br/>وكيل AI بلا ذاكرة]
    B -->|نعم — كل ACs تَنجح| D{هل ACs بصيغة<br/>Given/When/Then؟}
    D -->|لا — نص حر| E[❌ مَرفوض<br/>غير قابل للأتمتة]
    D -->|نعم| F{هل لكل AC<br/>test_command؟}
    F -->|لا| G[❌ مَرفوض<br/>لا CI verification]
    F -->|نعم| H[✅ مَقبول]

    style C fill:#f99
    style E fill:#f99
    style G fill:#f99
    style H fill:#9f9
```

---

## 7. معمارية متعددة الأنظمة — Ontology الموحَّد

### 7.1 لماذا ontology؟

```mermaid
flowchart TB
    subgraph SYSTEMS["🌐 الأنظمة المُخطَّطة"]
        DOC[living-documentation/<br/>keywords + builtins + …]
        TYPE[type-system/<br/>أنواع + relationships]
        ERR[error-system/<br/>أكواد + رسائل]
        TEST[test-system/<br/>مُخطَّط]
    end

    subgraph PROBLEM["❌ بدون ontology"]
        P1[keyword 'إذا' في docs<br/>لا يَعرف عن error E001<br/>الذي يَذكره]
        P2[type 'رقم' في type-system<br/>لا يَعرف عن builtin 'رقم()'<br/>الذي يُحوِّل إليه]
        P3[تَغيير اسم keyword<br/>= drift في error messages]
    end

    subgraph SOLUTION["✅ مع _INTEROP_ONTOLOGY.md"]
        ONTO[Entity Schema موحَّد<br/>+ Cross-system References]
        S1["keyword.references[]: error_id[]"]
        S2["type.builtin_constructors[]: builtin_id[]"]
        S3["تَغيير = تَحديث آلي في كل النظام"]
    end

    SYSTEMS --> PROBLEM
    PROBLEM --> SOLUTION

    style PROBLEM fill:#fcc
    style SOLUTION fill:#cfc
```

### 7.2 بُنية `_INTEROP_ONTOLOGY.md` المُقترَحة

```yaml
# _bmad-output/systems/_INTEROP_ONTOLOGY.md (مُختصَر)

ontology_version: "1.0"
entity_types:
  - Keyword       # systems/living-documentation/data/keywords/
  - Builtin       # systems/living-documentation/data/builtins/
  - Error         # systems/error-system/data/errors/
  - Type          # systems/type-system/data/types/
  - Directive     # systems/living-documentation/data/directives/

cross_references:
  Keyword:
    referenced_by: [Error]      # keyword 'إذا' مَذكور في error E0042
    references: [Type]           # keyword 'رقم' يُشير لـ type 'رقم'
  Error:
    references: [Keyword, Type]
  Type:
    referenced_by: [Keyword, Builtin]
    references: [Builtin]        # type 'رقم' يَستخدم builtin 'رقم()'

validation_rules:
  - "كل reference يَجب أن يَكون لـ entity موجود (no dangling refs)"
  - "تَغيير entity.id = تَحديث آلي لكل referenced_by"
  - "حذف entity مَرفوض إذا له referenced_by"
```

---

## 8. خَط المعالجة (Build Pipeline)

```mermaid
flowchart LR
    subgraph DEV["💻 Development"]
        D1[وكيل يَكتب<br/>data/*.yaml] --> D2[git commit]
        D2 --> D3[git push PR]
    end

    subgraph CI["🛡️ CI Pipeline"]
        D3 --> C1[Lint YAML]
        C1 --> C2[Schema validation<br/>Tier 1/2/3]
        C2 --> C3[Cross-ref check<br/>via ontology]
        C3 --> C4[Quality Gates G1-G7]
        C4 --> C5[Build JSON IR]
        C5 --> C6[Test runtime<br/>loadFromYAML]
        C6 --> C7[Test sadinfo --dump-all]
    end

    subgraph DEPLOY["🚀 Deployment"]
        C7 --> P1[Push to main]
        P1 --> P2[Auto-deploy<br/>إلى السيرفر الخاص]
        P2 --> P3[JSON IR live<br/>sadlang.org/sadinfo.json]
        P2 --> P4[Website rebuild]
    end

    subgraph CONSUME["📥 Consumption"]
        P3 --> Q1[وكلاء AI<br/>يَستهلكون]
        P3 --> Q2[LSP يَستخدم<br/>كَ fallback]
        P4 --> Q3[Browsing UI]
        P2 --> Q4[Runtime يَستخدم<br/>YAML مباشرة]
    end

    style CI fill:#fcf
    style DEPLOY fill:#cff
    style CONSUME fill:#ffc
```

---

## 9. القرارات الـ 5 غير القابلة للتفاوض (قبل M1)

| # | القرار | مَن دعمه (Round 2) | الدافع |
|---|---|---|---|
| **1** | اكتب `_bmad-output/systems/_INTEROP_ONTOLOGY.md` | Winston | معمارية متعددة الأنظمة تَحتاج عقد موحَّد |
| **2** | حوِّل `sadinfo` → مكتبة `shared/sadinfo_core/` + CLI رقيق | Winston, John, Quinn | AI لا تَستدعي shell — تَستهلك API |
| **3** | استبدل `cmake codegen` بـ `runtime YAML loader` | Winston, Quinn | أقل cmake errors، أسهل debugging |
| **4** | اكتب G1–G7 كـ automated CI checks (ليس M2) | Winston, John, الكل | AI تَثق بـ QG، تَمنع drift |
| **5** | قِس baseline performance لـ `--dump-all` (target < 1s) | Winston, Amelia | وكلاء يَستدعون 50×/يوم |

---

## 10. القرارات المُؤجَّلة (تَحتاج قياس قبل الالتزام)

| # | القرار | مَن طَرحه | شرط القياس |
|---|---|---|---|
| **A** | Merkle hash للـ snapshots | Quinn | فقط إذا git diff لا يَكفي لـ drift detection |
| **B** | نطاق > 150 entry | المالك | فقط إذا 130-150 لا يَكفي للسياق |
| **C** | LSP migration إلى JSON IR | Quinn | فقط إذا runtime memory < 2x latency |
| **D** | AI training format | Winston | فقط بعد توفر AI training pipeline |

---

## 11. المخاطر والتَخفيف

```mermaid
quadrantChart
    title المخاطر — احتمال × أثر
    x-axis "احتمال منخفض" --> "احتمال عالي"
    y-axis "أثر منخفض" --> "أثر عالي"
    quadrant-1 "خَفِّف فوراً"
    quadrant-2 "راقِب"
    quadrant-3 "تَجاهَل"
    quadrant-4 "خَفِّف مع وقت"
    "Context loss يَستمر": [0.7, 0.9]
    "QG تَفشل في كَشف drift": [0.4, 0.85]
    "sadinfo lib تَنهار": [0.3, 0.9]
    "السيرفر الخاص down": [0.2, 0.6]
    "YAML schema يَتطوَّر بسرعة": [0.6, 0.5]
    "وكلاء يَتجاوزون QG": [0.5, 0.7]
    "Ontology معقَّد جداً": [0.4, 0.4]
    "اختبارات flaky": [0.5, 0.3]
```

| المخاطر | احتمال | أثر | التَخفيف |
|---|:---:|:---:|---|
| Context loss يَستمر | عالي | عالي جداً | `sadinfo --dump-all` إلزامي قبل كل مهمة (في agent prompt) |
| QG تَفشل في كَشف drift | متوسط | عالي | Test fixtures عمدية بـ drift + assert detection |
| sadinfo library تَنهار | منخفض | عالي جداً | Unit tests > 90% + integration tests شاملة |
| وكلاء يَتجاوزون QG | متوسط | عالي | branch protection rules + auto-revert على main |
| السيرفر الخاص down | منخفض | متوسط | mirror احتياطي بدون coupling (read-only sync) |
| YAML schema يَتطوَّر | متوسط | متوسط | `schema_version` في كل ملف + migration scripts |
| Ontology معقَّد | متوسط | منخفض | ابدأ بسيطاً، توسَّع تَدريجياً بـ ADRs |

---

## 12. مَقاييس النَجاح (Success Metrics)

| مَقياس | Target M1 | Target M3 | Target M7 | كيف يُقاس |
|---|:---:|:---:|:---:|---|
| **# entities موثَّقة** | 65 | 146 | 200+ | `sadinfo --count` |
| **# QG violations في PR** | < 5 | < 2 | 0 | CI logs |
| **سرعة `--dump-all`** | < 2s | < 1s | < 500ms | benchmark |
| **# drift detections في main** | N/A | 0 | 0 | weekly audit |
| **# context loss incidents** | N/A | تَتبُّع | < 1/شهر | استبيان للوكلاء |
| **# Sprint stories بـ Given/When/Then** | 100% | 100% | 100% | lint script |
| **# DoDs مُؤجَّلة** | 0 | 0 | 0 | governance audit |

---

## 13. الفجوات الباقية — أسئلة للمالك

قبل اعتماد V2 رسمياً، نَحتاج إجابات على:

| # | السؤال | لماذا مُهم؟ |
|---|---|---|
| Q1 | هل تَوافق على إنشاء `_INTEROP_ONTOLOGY.md` كَخطوة pre-M1؟ | يُغيِّر تَنظيم الأنظمة بالكامل |
| Q2 | هل المخطط الزمني الجديد (8 أشهر) واقعي مع الموارد الحالية؟ | يُحدِّد سرعة التَنفيذ |
| Q3 | هل تَدعم قَرار "السيرفر الخاص فقط" (بدون gh-pages كـ fallback)؟ | يُحدِّد deployment strategy |
| Q4 | هل تَقبل بـ "0 deferred DoDs"؟ (يُلزِم كل ستوري بالاكتمال) | يُغيِّر sprint planning |
| Q5 | ما الأنظمة الأخرى المُخطَّطة؟ (لتَصميم ontology صحيح) | يُحدِّد عدد entity types |
| Q6 | هل سَتُعتمَد V2 رسمياً؟ متى؟ | يُحدِّد التَوقيت |

---

## 14. خَطوات التَنفيذ المُقترَحة (إذا اعتُمدت V2)

```mermaid
flowchart TB
    A[اعتماد المالك<br/>لـ V2] --> B[نَقل STRATEGY.md → _archive/<br/>كَ STRATEGY_V1_PRE_ROUND2.md]
    B --> C[إعادة تَسمية STRATEGY_V2 → STRATEGY.md<br/>وتَحديث authority: SoT]
    C --> D[تَحديث ARCHITECTURE.md و IMPLEMENTATION_PLAN.md<br/>وفق V2]
    D --> E[إعادة كتابة 36 ستوري<br/>بصيغة Given/When/Then]
    E --> F[إنشاء _bmad-output/systems/_INTEROP_ONTOLOGY.md]
    F --> G[كتابة 5 ADRs<br/>للقرارات غير القابلة للتفاوض]
    G --> H[بدء M1 - Pre-work<br/>sadinfo_core library]

    style A fill:#9f9
    style H fill:#9cf
```

### قائمة تَحقُّق التَنفيذ (Pre-M1 Checklist)

- [ ] **PRE-1**: اعتماد V2 من المالك (مستند صريح)
- [ ] **PRE-2**: نَقل V1 إلى `_archive/STRATEGY_V1_PRE_ROUND2.md`
- [ ] **PRE-3**: إعادة تَسمية V2 إلى `STRATEGY.md` (تَصبح SoT)
- [ ] **PRE-4**: كتابة `_bmad-output/systems/_INTEROP_ONTOLOGY.md`
- [ ] **PRE-5**: 5 ADRs للقرارات غير القابلة للتفاوض:
  - ADR-DOCS-V2-001: sadinfo as library + thin CLI
  - ADR-DOCS-V2-002: runtime YAML loader (no cmake codegen)
  - ADR-DOCS-V2-003: Quality Gates G1-G7 as CI workflows
  - ADR-DOCS-V2-004: Private server only (no gh-pages fallback)
  - ADR-DOCS-V2-005: Multi-system ontology architecture
- [ ] **PRE-6**: تَحديث `ARCHITECTURE.md` وفق المعمارية الجديدة (الأقسام 3.1، 3.2، 3.3)
- [ ] **PRE-7**: إعادة كتابة `IMPLEMENTATION_PLAN.md` بـ 146 entry محدَّد
- [ ] **PRE-8**: إعادة كتابة 36 ستوري بصيغة Given/When/Then
- [ ] **PRE-9**: حذف كل deferred DoDs من S-000a و S-001
- [ ] **PRE-10**: قياس baseline performance لـ `sadinfo --dump-all`

---

## 15. خَلاصة الـ Round 2 — الإجماع النهائي

> **اعترافات الوكلاء (مَنقولة حرفياً من PARTY_MODE_CRITIQUE_2026-06-02.md):**
>
> - **Winston:** "كنت على 80% صحيح — لكن قاعدة السياق كانت ناقصة." (5 نقاط احتاجت تصحيح)
> - **John:** "كنت مخطئاً عن أن مستخدِم البشر هو الأولوية. كنت محقاً عن أن السؤال أساسي."
> - **Amelia:** "نقدي الأول كان 85% صحيح — السياق يَجعله أعمق وأكثر إلحاحاً."
> - **Quinn:** "6 من 8 افتراضاتي احتاجت تصحيح. اعترفت بالأخطاء المنطقية."

### القاعدة الذهبية الجديدة

```text
في نظام 100% AI-driven:
  - "روح الهدف" ≠ تَوثيق كافٍ (لا ذاكرة للوكيل)
  - كل شيء يَجب أن يَكون آلياً (Quality Gates، tests، deployment)
  - كل entity يَجب أن يَكون قابل للقراءة والكتابة من API (ليس shell فقط)
  - كل decision يَجب أن يَكون ADR قابل للقراءة من وكيل آخر
```

---

## 16. مراجع ووثائق ذات صلة

- [STRATEGY.md (V1 — الحالية)](STRATEGY.md) — تُستبدَل إذا اعتُمدت V2
- [ARCHITECTURE.md](ARCHITECTURE.md) — تَحتاج تَحديث وفق V2
- [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md) — تَحتاج تَحديث وفق V2
- [_archive/PARTY_MODE_CRITIQUE_2026-06-02.md](_archive/PARTY_MODE_CRITIQUE_2026-06-02.md) — مَصدر الإجماع
- [stories/](stories/) — تَحتاج إعادة كتابة بـ Given/When/Then
- [_bmad-output/governance/1-policy/](../../governance/1-policy/) — حوكمة المشروع

---

## 18. استراتيجية النَشر + الأمثلة + التَمارين (إجابة سؤال المالك)

> **سؤال المالك (2026-06-04):** "ما هي استراتيجية نشر التَوثيقات في الموقع مع الأمثلة والتَمارين في خطة الاستراتيجية V2؟"
>
> هذا القسم يُغطي ثلاث طبقات: **(1) النَشر، (2) الأمثلة، (3) التَمارين** — كل منها بـ pipeline آلي + Quality Gate.

---

### 18.1 استراتيجية النَشر (Deployment Strategy)

#### نَموذج "Pull-Based" بدلاً من "Push-Based"

السيرفر الخاص لا يَستقبل push من CI — بل يَسحب من Git tag مُوقَّع كل دورة:

```mermaid
sequenceDiagram
    participant Dev as 👤 Developer/Agent
    participant Git as 📦 Git Repo
    participant CI as 🤖 GitHub Actions
    participant Reg as 📋 Registry (sad-docs-registry)
    participant Srv as 🖥️ السيرفر الخاص<br/>(docs.sadlang.io)
    participant CDN as 🌐 CDN (Cloudflare)
    participant User as 👁️ User/Agent

    Dev->>Git: git push (PR merged)
    Git->>CI: trigger build pipeline
    CI->>CI: sadinfo --dump-all → docs.json
    CI->>CI: build static site (mkdocs/vitepress)
    CI->>CI: validate Quality Gates G1-G7
    CI->>Git: git tag v-docs-2026-06-04T12-00
    CI->>Reg: publish manifest (signed SHA-256)
    
    Note over Srv: every 5 minutes (cron)
    Srv->>Reg: pull latest manifest
    Reg-->>Srv: manifest.json + signature
    Srv->>Srv: verify signature
    Srv->>Git: git fetch tag v-docs-2026-06-04T12-00
    Srv->>Srv: atomic swap (blue/green)
    Srv->>CDN: invalidate cache
    
    User->>CDN: GET docs.sadlang.io/keyword/دالة
    CDN->>User: HTML + JSON (cached)
```

#### بُنية السيرفر الخاص

```mermaid
flowchart TB
    subgraph SRV["🖥️ السيرفر الخاص (Self-Hosted)"]
        subgraph FE["Frontend Layer"]
            NG[Nginx<br/>reverse proxy]
            ST[Static Site<br/>VitePress build]
        end
        subgraph API["API Layer"]
            REST[REST API<br/>/api/v1/entity/:id]
            GQL[GraphQL<br/>/api/v1/graphql]
            WS[WebSocket<br/>/api/v1/playground]
        end
        subgraph DATA["Data Layer"]
            JSON[(docs.json<br/>146 entities)]
            SEARCH[(Lunr index<br/>full-text)]
            EXAMPLES[(examples.db<br/>SQLite)]
        end
        subgraph EXEC["Execution Layer (Playground)"]
            DOCKER[Docker container<br/>sad runtime]
            QUEUE[Redis queue<br/>rate limit]
        end
    end

    NG --> ST
    NG --> REST
    NG --> GQL
    NG --> WS
    REST --> JSON
    REST --> SEARCH
    GQL --> JSON
    WS --> QUEUE
    QUEUE --> DOCKER
    DOCKER --> EXAMPLES

    style SRV fill:#e0f0ff
    style EXEC fill:#fff0e0
```

#### مَكونات النَشر بالتَفصيل

| المكون | الأداة | الدور |
|---|---|---|
| **Build** | GitHub Actions | `sadinfo --dump-all` + VitePress build |
| **Signing** | cosign (Sigstore) | تَوقيع manifest برقم SHA-256 |
| **Registry** | GitHub Releases | حِفظ tags + artifacts |
| **Pull agent** | systemd timer + bash | كل 5 دقائق على السيرفر |
| **Hot reload** | nginx + symlink swap | استبدال atomic بدون downtime |
| **CDN** | Cloudflare (free tier) | cache + DDoS protection |
| **Monitoring** | Prometheus + Grafana | uptime + latency + errors |
| **Backup** | rclone → S3 (daily) | استرداد كارثي |

#### استراتيجية الإصدار (Versioning)

```mermaid
flowchart LR
    A[main branch] -->|every PR merge| B[v-docs-2026-06-04T12-00]
    B --> C{Quality Gates<br/>G1-G7}
    C -->|✓ all pass| D[publish to /current]
    C -->|✗ fail| E[block + alert]
    D --> F[/v/current/keyword/دالة]
    D --> G[/v/2026-06-04/keyword/دالة]
    G -.->|preserved| H[immutable archive]

    style D fill:#9f9
    style E fill:#f99
    style H fill:#cf9
```

**ملاحظات:**
- `/v/current/` يُعاد توجيهه دائماً لأحدث إصدار صحيح
- `/v/YYYY-MM-DD/` immutable — وكيل قديم يَطلب إصدار محدَّد
- لا يُحذف أي إصدار قديم — drift detection يَعتمد عليه

---

### 18.2 الأمثلة (Examples Strategy)

> **مبدأ:** كل entity يَجب أن يَحوي ≥1 مثال صالح + ≥1 مثال خاطئ (anti-pattern).
> الأمثلة **تُختبَر تلقائياً** في CI — لا تُنشَر إلا إن نَجحت.

#### بُنية المثال في YAML

```yaml
# مَلف: docs/keywords/دالة.yaml
id: KW-FUNC-001
ar: دالة
en: function
category: keyword
since: "0.1.0"

examples:
  - id: EX-FUNC-001-basic
    title_ar: دالة بسيطة
    title_en: Basic function
    code: |
      دالة جمع(أ، ب)
          ارجع أ + ب
      نهاية
      اطبع_سطر(جمع(3، 5))
    expected_output: "8"
    expected_exit_code: 0
    tags: [beginner, math]
    test_runner: sad-interp  # أو sad-compile

  - id: EX-FUNC-001-recursive
    title_ar: دالة مُتكررة
    title_en: Recursive function
    code: |
      دالة عاملي(ن)
          إذا (ن <= 1)
              ارجع 1
          نهاية
          ارجع ن * عاملي(ن - 1)
      نهاية
      اطبع_سطر(عاملي(5))
    expected_output: "120"
    tags: [intermediate, recursion]

  - id: EX-FUNC-001-antipattern
    title_ar: ❌ نسيان كلمة نهاية
    title_en: ❌ Missing 'نهاية'
    code: |
      دالة جمع(أ، ب)
          ارجع أ + ب
      # نسيان نهاية!
    expected_error: "ParseError: expected 'نهاية' at line 4"
    expected_exit_code: 1
    tags: [anti-pattern, common-mistake]
    why_wrong_ar: |
      كل كتلة في لغة ص يَجب أن تَنتهي بـ `نهاية`.
      المُفسِّر يَنتظر إغلاق الدالة ولا يَجد.
```

#### Pipeline اختبار الأمثلة

```mermaid
flowchart TB
    A[YAML files<br/>docs/**/*.yaml] --> B[example-extractor<br/>Python script]
    B --> C[examples/<br/>EX-FUNC-001-basic.ص]
    C --> D{Test Runner}
    D -->|sad-interp| E[sad.exe example.ص]
    D -->|sad-compile| F[sadc.exe -o out + run]
    E --> G{Compare output}
    F --> G
    G -->|✓ match| H[✅ PASS]
    G -->|✗ mismatch| I[❌ FAIL + block PR]
    H --> J[mark verified: true<br/>in docs.json]
    I --> K[GitHub PR comment<br/>with diff]

    style H fill:#9f9
    style I fill:#f99
```

#### تَصنيف الأمثلة

| التَصنيف | العدد المُستهدف لكل entity | الغرض |
|---|:---:|---|
| `beginner` | 1+ | شرح أساسي |
| `intermediate` | 1+ | استخدام شائع |
| `advanced` | 0-1 | حالة قصوى |
| `anti-pattern` | 1+ | خطأ شائع + شرح |
| `interop` | 0-1 | تَفاعل مع entity آخر |

**المُجمل المُستهدَف:** 146 entity × ~4 examples = **~584 مثال مُختبَر آلياً**

#### عَرض المثال في الموقع

```mermaid
flowchart LR
    A[/keyword/دالة] --> B[VitePress page]
    B --> C[Code block<br/>syntax highlight]
    B --> D[▶ Run in Playground<br/>button]
    B --> E[📋 Copy to clipboard]
    B --> F[🔗 Permalink<br/>EX-FUNC-001-basic]
    D --> G[WebSocket → docker exec]
    G --> H[Output panel<br/>real-time]

    style D fill:#cff
    style H fill:#fc9
```

---

### 18.3 التَمارين (Exercises Strategy)

> **مبدأ:** التَمرين = "ستوري قابل للتَنفيذ" يَتعلَّمه الوكيل/المستخدِم بحل مَهمة محدَّدة مع شُروط قَبول قابلة للقياس.

#### نَموذج التَمرين

```yaml
# مَلف: docs/exercises/EX-FIBONACCI.yaml
id: EX-FIBONACCI
title_ar: فيبوناتشي
title_en: Fibonacci
difficulty: beginner  # beginner | intermediate | advanced
estimated_time_min: 10
covers_entities: [KW-FUNC-001, KW-IF-001, KW-RETURN-001]
prerequisites: []

problem_ar: |
  اكتب دالة `فيبوناتشي(ن)` تَرجع رقم فيبوناتشي رقم `ن`.
  - فيبوناتشي(0) = 0
  - فيبوناتشي(1) = 1
  - فيبوناتشي(ن) = فيبوناتشي(ن-1) + فيبوناتشي(ن-2)

starter_code: |
  دالة فيبوناتشي(ن)
      # اكتب الحل هنا
  نهاية

test_cases:
  - input: "اطبع_سطر(فيبوناتشي(0))"
    expected_output: "0"
  - input: "اطبع_سطر(فيبوناتشي(1))"
    expected_output: "1"
  - input: "اطبع_سطر(فيبوناتشي(10))"
    expected_output: "55"
  - input: "اطبع_سطر(فيبوناتشي(20))"
    expected_output: "6765"

solution: |
  دالة فيبوناتشي(ن)
      إذا (ن <= 1)
          ارجع ن
      نهاية
      ارجع فيبوناتشي(ن - 1) + فيبوناتشي(ن - 2)
  نهاية

hints:
  - level: 1
    text_ar: ابدأ بحالة التَوقُّف (الأساس) — ماذا يَحدث عندما تَكون ن صغيرة؟
  - level: 2
    text_ar: استخدم الاستدعاء الذاتي (recursion) لاستدعاء الدالة من داخلها.
  - level: 3
    text_ar: تَذكَّر إضافة شرط `إذا` لمنع الاستدعاء اللانهائي.

acceptance_criteria:
  - given: مَدخَلات صحيحة (0-30)
    when: استدعاء الدالة
    then: النَتيجة مُطابقة لـ test_cases
  - given: لا توجد infinite loop
    when: قياس وقت التَنفيذ
    then: < 5 ثوانٍ لـ ن=20
```

#### مَسار حل التَمرين للوكيل

```mermaid
sequenceDiagram
    participant A as 🤖 Agent
    participant API as 🌐 docs.sadlang.io/api
    participant PG as 🎮 Playground
    participant DB as 📊 Progress DB

    A->>API: GET /exercise/EX-FIBONACCI
    API-->>A: problem + starter_code + tests
    A->>A: تَحليل المسألة
    A->>A: كتابة الحل
    A->>PG: POST /run<br/>{code, test_cases}
    PG->>PG: docker exec sad
    PG-->>A: results (pass/fail per test)
    
    alt كل الاختبارات نَجحت
        A->>API: POST /submit<br/>{solution, agent_id}
        API->>DB: record success
        API-->>A: ✅ Completed + next exercise suggestion
    else فشل
        A->>API: GET /hint?level=1
        API-->>A: hint
        A->>A: إعادة المحاولة
    end
```

#### تَصنيف التَمارين

```mermaid
mindmap
  root((التَمارين<br/>~200 مُستهدَف))
    Beginner~60
      Syntax basics
      Variables
      Simple functions
      Conditionals
      Loops
    Intermediate~80
      Classes/Structs
      Pattern matching
      Error handling
      Modules
      File I/O
    Advanced~40
      Concurrency goroutines
      Macros
      FFI
      Generics
      Async
    Domain-specific~20
      OS kernel
      Network
      Graphics
      Crypto
```

#### Quality Gate للتَمارين (G8 جديد مَقترح)

| فحص | شرط النَجاح |
|---|---|
| **G8.1 starter_code** | يُحلِّل lex/parse بنجاح (يُعطي خطأ تنفيذ متوقَّع) |
| **G8.2 solution** | يَجتاز 100% من test_cases |
| **G8.3 hints** | 3 مستويات على الأقل (تَدرُّج من غامض إلى صريح) |
| **G8.4 acceptance** | Given/When/Then قابل للقياس آلياً |
| **G8.5 difficulty match** | covers_entities ⊆ entities لـ difficulty المعلَن |

---

### 18.4 ربط النَشر + الأمثلة + التَمارين معاً

```mermaid
flowchart TB
    subgraph SRC["📁 Source (Git)"]
        Y1[docs/keywords/*.yaml<br/>+ examples]
        Y2[docs/exercises/*.yaml]
        Y3[docs/builtins/*.yaml<br/>+ examples]
    end

    subgraph CI["🤖 CI Pipeline"]
        EX[extract examples<br/>→ .ص files]
        TE[test all examples<br/>sad + sadc]
        TX[test all exercises<br/>starter + solution]
        QG[Quality Gates<br/>G1-G8]
        BD[build static site<br/>VitePress]
    end

    subgraph SRV["🖥️ Server"]
        ST[Static pages<br/>HTML + JSON]
        API[REST + GraphQL API]
        PG[Playground<br/>Docker exec]
        TR[Progress Tracker<br/>per agent]
    end

    subgraph USR["👥 Users"]
        AG[🤖 AI Agents<br/>via API]
        HU[👤 Humans<br/>via Web]
    end

    Y1 --> EX
    Y2 --> TX
    Y3 --> EX
    EX --> TE
    TE --> QG
    TX --> QG
    QG -->|✓| BD
    QG -->|✗| BLOCK[❌ block PR]
    BD --> ST
    BD --> API
    BD --> PG
    PG --> TR
    AG -->|GET/POST| API
    AG -->|exercises| TR
    HU -->|browse| ST
    HU -->|try| PG

    style QG fill:#fc9
    style BLOCK fill:#f99
    style AG fill:#9cf
```

#### المُجمل النهائي (M7 — 8 أشهر)

| المُكوِّن | الكَم | المَصدر |
|---|:---:|---|
| Entities موثَّقة | 146 | YAML files |
| Examples مُختبَرة | ~584 | extracted + tested |
| Exercises قابلة للحل | ~200 | dedicated YAML |
| API endpoints | 8+ | REST + GraphQL |
| Playground sessions/day | 1000+ | Docker queue |
| Quality Gates | G1-G8 | CI workflows |
| Test coverage | 100% | كل example + exercise |

---

### 18.5 ستوريات جديدة مُقترَحة (للسبرنت القادم)

| Story ID | العنوان | الأولوية | Effort |
|---|---|:---:|:---:|
| **S-DEPLOY-001** | إنشاء `_INFRA_DEPLOY.md` بتَفاصيل السيرفر الخاص | 🔴 | M |
| **S-EXAMPLES-001** | تَوسيع YAML schema لدعم `examples[]` | 🔴 | S |
| **S-EXAMPLES-002** | كتابة `example-extractor` (YAML → .ص files) | 🔴 | M |
| **S-EXAMPLES-003** | CI workflow لاختبار جميع الأمثلة | 🔴 | M |
| **S-EXERCISES-001** | تَصميم YAML schema للتَمارين | 🟡 | S |
| **S-EXERCISES-002** | كتابة 20 تَمرين beginner أولاً | 🟡 | L |
| **S-PLAYGROUND-001** | Docker setup + REST API | 🟢 | L |
| **S-PLAYGROUND-002** | WebSocket integration للنَتائج المُباشرة | 🟢 | M |

---

## 19. سِجل التَغييرات

| التاريخ | الإصدار | الكاتب | التَغيير |
|---|---|---|---|
| 2026-06-04 | V2-DRAFT | إجماع 4 وكلاء (Round 2) | إنشاء أولي مَبني على Round 2 critique |
| 2026-06-04 | V2-DRAFT.1 | إجابة سؤال المالك | إضافة قسم 18: النَشر + الأمثلة + التَمارين |

---

**انتهت V2-DRAFT.1 — تَنتظر اعتماد المالك.**
