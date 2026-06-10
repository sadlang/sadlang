---
title: "وثيقة المعمارية الموحَّدة — نظام التوثيق الحي للغة ص"
type: canonical-architecture
system: living-documentation
date: 2026-06-02
status: ACTIVE
authority: SINGLE-SOURCE-OF-TRUTH
supersedes:
  - 2-architecture/planning/ARCHITECTURE_MAP.md (مُؤرشَف للرجوع)
  - 2-architecture/planning/YAML_UNIFIED_SCHEMA_DESIGN.md (مُؤرشَف للرجوع)
  - 2-architecture/planning/DOC_DISTRIBUTION_FLOWS.md (مُؤرشَف للرجوع)
  - 2-architecture/planning/DOC_FLOW_REALITY.md (مُؤرشَف للرجوع)
  - UNIFIED_DOCS_ARCHITECTURE.md (مُؤرشَف للرجوع)
related_canonical:
  - STRATEGY.md
  - IMPLEMENTATION_PLAN.md
adrs_referenced:
  - _archive/2-architecture/decisions/ADR-RESTRUCTURE-2026-06-01.md
  - _archive/2-architecture/decisions/ADR-SADINFO-ARCHITECTURE.md
  - _archive/2-architecture/decisions/ADR-006a_توحيد_codegen.md
  - _archive/2-architecture/decisions/ADR-006b-spec.md
  - _archive/2-architecture/decisions/ADR-006_توحيد_نظام_التوليد.md
  - _archive/2-architecture/decisions/ADR-007_منصة_التوثيق_Docusaurus.md
  - _archive/2-architecture/decisions/ADR-008_علاقة_الموقع_بالمشروع.md
governance_rule: GR-DOCS-CANONICAL
---

# 🏗️ وثيقة المعمارية الموحَّدة — نظام التوثيق الحي

> **هذه هي الوثيقة المعمارية الوحيدة المُعتمَدة**. أي تَصميم معماري سابق في `_archive/` هو **محتوى تاريخي للرجوع فقط** وغير مُعتمَد لقرارات جديدة.

---

## 1. المنظور المعماري الكلي

النظام مَبني على **3 طبقات تَتدفَّق البيانات بينها بِاتجاه واحد**:

```mermaid
graph LR
    subgraph SoT["1️⃣ مصدر الحقيقة (SoT)"]
        Y[data/_schemas/<br/>YAML Files]
    end

    subgraph Tool["2️⃣ أداة التَحويل"]
        S[sadinfo CLI<br/>tools/sadinfo/]
        SC[shared/sadinfo_core/<br/>مكتبة C++17]
    end

    subgraph Consumers["3️⃣ المُستهلكون"]
        W[website/<br/>VitePress]
        L[LSP Server]
        E[VS Code Ext]
        C[CI Gates]
        A[AI Drafts]
    end

    Y -->|yaml-cpp| SC
    SC --> S
    S -->|JSON IR<br/>Merkle Hashed| W
    S -->|JSON| L
    S -->|JSON| E
    S -->|exit code| C
    S -->|YAML| A

    style Y fill:#fff4e6,stroke:#ff9800,stroke-width:3px
    style S fill:#e3f2fd,stroke:#1976d2,stroke-width:2px
    style SC fill:#e3f2fd,stroke:#1976d2,stroke-width:2px
```

---

## 2. مصدر الحقيقة (Source of Truth)

### 2.1 البنية الجذرية لـ `data/`

```
data/
├── _schemas/              ← JSON Schemas + سياسات (Tier1 validators)
│   ├── builtin.schema.json
│   ├── keyword.schema.json
│   ├── error.schema.json
│   └── lesson.schema.json
├── _meta/
│   └── CODEOWNERS         ← ملكية الكيانات (YAML)
├── builtins/              ← 21 دالة مدمجة
│   └── builtin_<name>/
│       ├── _index.yaml    ← إلزامي
│       ├── docs.yaml      ← مستحسن
│       ├── examples/*.yaml
│       └── i18n/{en,fr,...}.yaml
├── keywords/              ← 40 محجوزة + 25 سياقية
├── errors/                ← رسائل الأخطاء
└── lessons/               ← دروس تَعليمية
```

### 2.2 قواعد التَسمية (إلزامية)

```
<folder_name> == <kind>_<id.last_segment>
```

| ✅ صحيح | ❌ خطأ |
|---|---|
| `data/builtins/builtin_print_line/` | `data/builtins/print_line/` |
| `data/keywords/keyword_if/` | `data/keywords/if/` |
| `data/errors/error_unexpected_token/` | `data/errors/E0001/` |

### 2.3 Whitelist صارم داخل كل كيان

| الملف/المجلد | إلزامي؟ | الغرض |
|---|---|---|
| `_index.yaml` | ✅ دائماً | بيانات أساسية + ميتاداتا |
| `docs.yaml` | مستحسن | وصف تفصيلي + see_also |
| `examples/*.yaml` | اختياري | مثال لكل ملف |
| `exercises/*.yaml` | اختياري | تمرين لكل ملف |
| `i18n/{lang}.yaml` | اختياري | overlay ترجمات |

أي ملف خارج هذه القائمة → فشل validation.

### 2.4 بنية `_index.yaml` (نموذج Keyword)

```yaml
schema_version: 1
id: keyword.if
kind: keyword
name: إذا
category: control_flow
reserved: true                  # true = محجوزة | false = سياقية
since: 0.1.0
contextual_scope: null          # للسياقية: "after_function" مثلاً
owners: [@core-team]            # يُحقَّق ضد _meta/CODEOWNERS
```

### 2.5 بنية `_index.yaml` (نموذج Builtin)

```yaml
schema_version: 1
id: builtin.print_line
kind: builtin
name: اطبع_سطر
category: io
since: 0.1.0
signature:
  params:
    - name: قيمة
      type: any
      required: true
  returns: void
  is_variadic: false
tooling:
  lsp:
    completion: true
    snippet: "اطبع_سطر(${1:قيمة})"
  formatter:
    breaks_line: true
version_info:
  added: 0.1.0
  deprecated: null
  replaced_by: null
owners: [@core-team]
```

---

## 3. أداة `sadinfo` — التَصميم الداخلي

### 3.1 تَقسيم المكتبة/الأداة

```mermaid
graph TD
    subgraph Lib["shared/sadinfo_core/ — مكتبة C++17"]
        Loader[Loader<br/>yaml-cpp → Entity*]
        Aggregator[Aggregator<br/>merge overlays + i18n]
        Validator[Validator<br/>Tier1/Tier2/Tier3]
        Index[Index Builder<br/>graph + lookup]
        Exporter[Exporter<br/>JSON / MD / SQLite]
        Watcher[Watcher<br/>file events]
        Cache[Cache Manager<br/>Merkle hash store]
    end

    subgraph CLI["tools/sadinfo/ — CLI رقيق"]
        Cmd[Command Dispatcher]
        Out[stdout JSON/YAML]
    end

    Cmd --> Loader
    Loader --> Aggregator
    Aggregator --> Validator
    Validator --> Index
    Index --> Exporter
    Exporter --> Out
    Watcher -.->|trigger| Loader
    Cache -.->|skip if unchanged| Loader

    style Lib fill:#e3f2fd,stroke:#1976d2
    style CLI fill:#e8f5e9,stroke:#388e3c
```

### 3.2 الأوامر العامة (CLI Contract)

| الأمر | الغرض | المُخرَج |
|---|---|---|
| `sadinfo load --check` | تَحميل + Tier1 validation | exit 0/1 |
| `sadinfo validate --tier <1\|2\|3>` | تَحقق متعدد المستويات | تقرير stdout |
| `sadinfo aggregate --hash merkle` | بناء `index.merkle` | ملف hash |
| `sadinfo lock <acquire\|release>` | قفل الحالة (للـCI) | exit 0/1 |
| `sadinfo export --target json --out <dir>` | تَصدير JSON IR | ملفات JSON |
| `sadinfo watch` | مراقبة `data/_schemas/` | events log |
| `sadinfo dump --kind <keyword\|builtin\|error\|directive>` | dump سريع | JSON/YAML |
| `sadinfo --version` / `--help` | معياري | stdout |

### 3.3 طبقات التَحقق (Validation Tiers)

| المستوى | يَفحص | الأداة |
|---|---|---|
| **Tier 1** | JSON Schema syntax + حقول مطلوبة | ajv / yaml-cpp + manual |
| **Tier 2** | Cross-reference (owners ∈ CODEOWNERS، see_also موجود) | sadinfo داخلي |
| **Tier 3** | Snapshots (مقارنة merkle hash بين تَشغيلين) | sadinfo + git |

---

## 4. تَدفُّق البيانات الفعلي

```mermaid
sequenceDiagram
    actor Dev as المطوِّر
    participant YAML as data/_schemas/*.yaml
    participant SI as sadinfo
    participant Cache as .sadinfo/cache
    participant JSON as website/docs/_generated/
    participant Site as VitePress
    participant LSP as LSP Server
    participant CI as GitHub Action

    Dev->>YAML: تَعديل keyword جديد
    Dev->>SI: sadinfo load --check
    SI->>YAML: قراءة yaml-cpp
    SI->>Cache: hash check (Merkle)
    SI-->>Dev: exit 0 (Tier1 OK)

    Dev->>SI: sadinfo export --target json
    SI->>JSON: كتابة *.json (atomic)
    
    Site->>JSON: VitePress dataLoader
    Site-->>Dev: hot reload

    LSP->>JSON: قراءة (cached)
    LSP-->>Dev: hover docs

    Dev->>CI: git push
    CI->>SI: sadinfo validate --tier 3
    CI-->>Dev: ✅ merge أو ❌ fail
```

> **لا حاجة لـdrift check** — `KeywordTable::initialize()` و `BuiltinRegistry` مُولَّدان من نفس YAML عبر cmake codegen، فالتَناقُض مستحيل بنيوياً (TD-08).

---

## 5. تَكامل مع باقي اللغة (Layered Architecture)

```mermaid
graph TB
    subgraph SoT["مصدر الحقيقة"]
        Y1[data/keywords.yaml]
        Y2[data/builtins/*.yaml]
    end

    subgraph Gen["مولّدات (cmake codegen — عند كل بناء)"]
        G1[scripts/codegen/gen_keywords.py<br/>→ generated/keyword_table.cpp]
        G2[scripts/codegen/gen_builtins.py<br/>→ generated/builtin_registry.cpp]
        G3[scripts/codegen/gen_errors.py<br/>→ generated/error_catalog.cpp]
    end

    subgraph Core["نواة اللغة"]
        L[shared/lexer]
        P[shared/parser]
        I[interpreter]
        C[compiler sadc]
    end

    subgraph Tools["الأدوات"]
        SI[tools/sadinfo]
        LSP[tools/lsp]
        FMT[tools/formatter]
    end

    subgraph Web["الموقع"]
        W[website/]
    end

    Y1 -->|Python codegen| G1
    G1 --> L
    Y1 -->|yaml-cpp| SI
    Y2 -->|yaml-cpp| SI
    L --> P --> I --> C
    SI -->|JSON IR| LSP
    SI -->|JSON IR| FMT
    SI -->|JSON IR| W

    style SoT fill:#fff4e6,stroke:#ff9800,stroke-width:2px
    style Core fill:#e8f5e9,stroke:#388e3c,stroke-width:2px
    style SI fill:#e3f2fd,stroke:#1976d2,stroke-width:2px
```

**قاعدة صارمة:** كل اعتماد يَتدفَّق من أعلى لأسفل (CW-02). لا يَجوز للـlexer أن يَستهلك من `sadinfo` (دائرة).

---

## 6. التَوزيع (Distribution Architecture)

| القناة | المحتوى | الوجهة | آلية التَحديث |
|---|---|---|---|
| **سيرفر خاص (Nginx)** | الموقع كامل (VitePress static) | `185.47.174.39` → `/opt/sad-lang-website/` → `https://sad-lang.org` | على كل merge to main عبر `deployment/deploy.{ps1,sh}` (rsync/scp) |
| **npm `@sad-lang/docs-data`** | JSON IR فقط | npm registry | release tag |
| **pip `sad-doctest`** | validators Python | PyPI | release tag |
| **VS Code Marketplace** | الامتداد + snippets | VS Code Marketplace | manual release |

> **تَفاصيل البنية التَحتية:** SSL عبر Let's Encrypt (certbot)، gzip مُفعَّل، HTTP/2، إعادة توجيه HTTP→HTTPS و www→apex، ترويسات أمان (X-Frame-Options, CSP, Referrer-Policy)، WASM MIME صحيح. التَكوين الكامل في [`deployment/nginx/sad-lang.org.conf`](../../../../deployment/nginx/sad-lang.org.conf).

---

## 7. Quality Gates (G1–G7)

| Gate | الفحص | الأداة | يَفشل عند |
|---|---|---|---|
| **G1** | Coverage: كل keyword له YAML entry | `check_keywords.py` | keyword بدون entry |
| **G2** | صحة codegen: `cmake --build` يُوَلّد جميع generated/*.cpp بدون أخطاء | cmake | فشل توليد من YAML |
| **G3** | Examples تَنفيذ مُطابق | dual_execution tests | output يَختلف |
| **G4** | كل entry له AR + EN | `bilingual_check.py` | حقل ناقص |
| **G5** | website build < 60s | benchmark | تَجاوُز عتبة |
| **G6** | axe-playwright accessibility | axe | critical violation |
| **G7** | Test flake rate < 1% | CI history | > 1% فشل عشوائي |

---

## 8. القرارات المعمارية (ADRs)

كل القرارات المعمارية تَعيش في `_archive/2-architecture/decisions/`. هذه الوثيقة تَستهلكها بِالمرجع:

| ADR | الموضوع | الحالة | الموقع |
|---|---|---|---|
| ADR-RESTRUCTURE-2026-06-01 | إعادة هيكلة الطبقات الثلاث | Active | [`_archive/2-architecture/decisions/ADR-RESTRUCTURE-2026-06-01.md`](_archive/2-architecture/decisions/ADR-RESTRUCTURE-2026-06-01.md) |
| ADR-SADINFO-ARCHITECTURE | بنية sadinfo (مكتبة + CLI) | Active | [`_archive/2-architecture/decisions/ADR-SADINFO-ARCHITECTURE.md`](_archive/2-architecture/decisions/ADR-SADINFO-ARCHITECTURE.md) |
| ADR-006 | تَوحيد نظام التَوليد | Active | [`_archive/2-architecture/decisions/ADR-006_توحيد_نظام_التوليد.md`](_archive/2-architecture/decisions/ADR-006_توحيد_نظام_التوليد.md) |
| ADR-006a | تَوحيد codegen | Active | [`_archive/2-architecture/decisions/ADR-006a_توحيد_codegen.md`](_archive/2-architecture/decisions/ADR-006a_توحيد_codegen.md) |
| ADR-006b | spec للتَوليد | Active | [`_archive/2-architecture/decisions/ADR-006b-spec.md`](_archive/2-architecture/decisions/ADR-006b-spec.md) |
| ADR-006b-2 | تَوليد التوثيق مُؤجَّل | Active | [`_archive/2-architecture/decisions/ADR-006b_توليد_التوثيق_مؤجَّل.md`](_archive/2-architecture/decisions/ADR-006b_توليد_التوثيق_مؤجَّل.md) |
| ADR-007 | منصة التوثيق (Docusaurus → VitePress) | Superseded | [`_archive/2-architecture/decisions/ADR-007_منصة_التوثيق_Docusaurus.md`](_archive/2-architecture/decisions/ADR-007_منصة_التوثيق_Docusaurus.md) |
| ADR-008 | علاقة الموقع بالمشروع | Active | [`_archive/2-architecture/decisions/ADR-008_علاقة_الموقع_بالمشروع.md`](_archive/2-architecture/decisions/ADR-008_علاقة_الموقع_بالمشروع.md) |

> **قاعدة GR-02:** لا تَحذف ADR. أي قرار يُلغى يُعلَّم `status: Superseded` ويُربط بـ`supersededBy`.
> **قاعدة جديدة (DOCS-CANONICAL):** أي ADR جديد يُضاف إلى نفس المجلد الأرشيفي + يُضاف صف هنا — لا يُنشأ مجلد جديد.

---

## 9. الحدود مع الأنظمة الأخرى

| النظام الخارجي | العلاقة | نوع التَكامل |
|---|---|---|
| **`shared/lexer`** | يَقرأ `data/keywords.yaml` عبر codegen | one-way (SoT → lexer) |
| **`error-messages` system** | نظام مستقل، sadinfo يَستهلك schemas منه | loose coupling |
| **`type-system`** | sadinfo يَستخرج معلومات الأنواع | read-only |
| **`website/`** | يَقرأ JSON من `_generated/` | read-only |
| **`tools/lsp/`** | يَستهلك JSON IR | read-only |

---

## 10. القيود غير الوظيفية (NFRs)

| # | الفئة | المتطلب |
|---|---|---|
| NFR-01 | الأداء | `sadinfo --dump-all` < 2 ثانية |
| NFR-02 | الأداء | `website build` < 60 ثانية |
| NFR-03 | الموثوقية | `sadinfo` idempotent (تَشغيل مرتين = نفس النتيجة) |
| NFR-04 | الموثوقية | mutation tests: حذف keyword → CI يَفشل |
| NFR-05 | الأمان | لا أسرار في JSON المُولَّد (trufflesecurity) |
| NFR-06 | الأمان | npm audit + pip-audit = 0 high/critical |
| NFR-07 | إمكانية الوصول | WCAG 2.1 AA |
| NFR-08 | i18n | كل entry له AR + EN (G4) |
| NFR-09 | الصيانة | `sadinfo` binary < 5MB |
| NFR-10 | الصيانة | لا تَكرار بين sadinfo وdocs_emitter |
| NFR-11 | التَوافقية | يَعمل Windows + Linux + macOS (CI matrix) |
| NFR-12 | التَتبُّع | كل JSON entry يَحوي source location (file:line) |

---

## 📎 المراجع

- **الاستراتيجية:** [STRATEGY.md](STRATEGY.md)
- **خطة التَنفيذ + 24 ستوري:** [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md)
- **ADRs (تَفصيلية):** [_archive/2-architecture/decisions/](_archive/2-architecture/decisions/)
- **المحتوى المعماري التاريخي:** [_archive/2-architecture/](_archive/2-architecture/) — للرجوع فقط
