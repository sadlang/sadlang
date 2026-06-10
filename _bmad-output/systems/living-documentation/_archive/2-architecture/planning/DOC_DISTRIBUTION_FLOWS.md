# 🗺️ مخططات مسارات التوثيق — من اللغة إلى المستهلكين

> **الغرض:** توثيق بصري شامل لمسارات تدفق التوثيق من **مصادر الحقيقة (SoT)** في الكود واليامل، عبر **مولِّدات وسيطة** (`sadinfo`, `gen_keywords.py`)، وصولاً إلى **المستهلكين النهائيين** (الموقع، LSP، REPL، المفسر، الفورماتر، الـCLI).
>
> **التاريخ:** 2026-05-23  
> **النسخة:** v1.0 — يعكس الحالة الفعلية في فرع `graphic` (HEAD = `e9168386`)
>
> **مفاتيح الألوان في كل المخططات:**
> - ✅ **أخضر** = منفَّذ ويعمل
> - ⚠️ **أصفر** = جزئي / مبدئي
> - ❌ **أحمر** = مخطط لكن غير منفَّذ
> - 🔵 **أزرق** = مصدر حقيقة (SoT)

---

## 1️⃣ النظرة الكلية: جميع المسارات معاً

هذا المخطط يربط جميع المسارات الموثَّقة لاحقاً في وثيقة واحدة:

```mermaid
flowchart TB
    %% ═════════════════════════════════════════
    %% مصادر الحقيقة
    %% ═════════════════════════════════════════
    subgraph SoT["🔵 مصادر الحقيقة (Single Source of Truth)"]
        direction LR
        YAML_KW["📄 data/language/<br/>keywords.yaml<br/>✅ موجود"]
        YAML_ERR["📄 data/language/<br/>error_messages.yaml<br/>❌ غير موجود"]
        YAML_MOD["📄 data/stdlib/<br/>modules.yaml<br/>❌ غير موجود"]
        YAML_LES["📄 data/lessons/<br/>*.yaml<br/>❌ غير موجود"]
        CPP_BI["💻 tools/sadinfo/src/<br/>builtin_data.cpp<br/>✅ موجود (hardcoded)"]
        SAD_EX["📁 examples/*.ص<br/>+ ## docstrings<br/>✅ موجود"]
        SAD_STD["📁 stdlib/**/*.ص<br/>+ ## docstrings<br/>✅ موجود"]
        MD_GUIDES["📁 docs/*.md<br/>(يدوي)<br/>✅ موجود"]
    end

    %% ═════════════════════════════════════════
    %% المولِّدات
    %% ═════════════════════════════════════════
    subgraph GEN["⚙️ المولِّدات (Generators)"]
        direction LR
        GEN_PY["🐍 scripts/codegen/<br/>gen_keywords.py<br/>✅ منفَّذ"]
        SADINFO["🛠️ tools/sadinfo<br/>(CLI binary)<br/>✅ منفَّذ"]
        DOC_EXTRACTOR["📜 stdlib doc extractor<br/>❌ غير موجود"]
    end

    %% ═════════════════════════════════════════
    %% المخرجات الوسيطة
    %% ═════════════════════════════════════════
    subgraph INTER["📦 المخرجات الوسيطة"]
        direction LR
        H_KW["📄 shared/lexer/include/<br/>keywords_generated.h<br/>✅ موجود"]
        JSON_KW["📄 keywords.json<br/>(عبر sadinfo)<br/>⚠️ ينشأ عند الطلب"]
        JSON_BI["📄 builtins.json<br/>(عبر sadinfo)<br/>⚠️ ينشأ عند الطلب"]
        JSON_ERR["📄 errors.json<br/>❌ غير ممكن"]
    end

    %% ═════════════════════════════════════════
    %% المستهلكون
    %% ═════════════════════════════════════════
    subgraph CONSUMERS["🎯 المستهلكون النهائيون"]
        direction TB
        LEXER["🔤 Lexer (sad/sadc)<br/>✅ يستهلك .h"]
        LSP["💡 LSP Server<br/>⚠️ hardcoded حالياً"]
        FMT["🎨 Formatter<br/>⚠️ hardcoded"]
        REPL["💻 REPL<br/>⚠️ hardcoded"]
        WEB["🌐 Website (VitePress)<br/>⚠️ يدوي حالياً"]
        CI_TOOLS["🤖 CI / أدوات خارجية<br/>✅ يمكنها استدعاء sadinfo"]
        IDE_EXT["🔌 VS Code Extension<br/>⚠️ hardcoded"]
    end

    %% ═════════════════════════════════════════
    %% الروابط
    %% ═════════════════════════════════════════
    YAML_KW -->|"build time"| GEN_PY
    YAML_KW -->|"runtime read<br/>yaml-cpp"| SADINFO
    CPP_BI -->|"#include"| SADINFO
    YAML_ERR -.->|"مخطط"| SADINFO
    YAML_MOD -.->|"مخطط"| DOC_EXTRACTOR
    SAD_STD -.->|"مخطط"| DOC_EXTRACTOR

    GEN_PY -->|"يولِّد"| H_KW
    SADINFO -->|"--dump-keywords"| JSON_KW
    SADINFO -->|"--dump-builtins"| JSON_BI
    SADINFO -.->|"--dump-errors<br/>غير منفَّذ"| JSON_ERR

    H_KW --> LEXER
    JSON_KW -.->|"مستقبلاً"| WEB
    JSON_KW -.->|"مستقبلاً"| LSP
    JSON_BI -.->|"مستقبلاً"| WEB
    JSON_BI -.->|"مستقبلاً"| IDE_EXT
    SADINFO -->|"CLI مباشر"| CI_TOOLS
    MD_GUIDES -->|"copy-paste<br/>يدوي"| WEB

    %% الألوان
    classDef sot fill:#1e3a8a,stroke:#1e40af,color:#fff
    classDef gen fill:#7c3aed,stroke:#6d28d9,color:#fff
    classDef ok fill:#16a34a,stroke:#15803d,color:#fff
    classDef warn fill:#ca8a04,stroke:#a16207,color:#fff
    classDef missing fill:#dc2626,stroke:#b91c1c,color:#fff,stroke-dasharray:5 5
    
    class YAML_KW,CPP_BI,SAD_EX,SAD_STD,MD_GUIDES sot
    class YAML_ERR,YAML_MOD,YAML_LES missing
    class GEN_PY,SADINFO gen
    class DOC_EXTRACTOR missing
    class H_KW,JSON_KW,JSON_BI ok
    class JSON_ERR missing
    class LEXER,CI_TOOLS ok
    class LSP,FMT,REPL,WEB,IDE_EXT warn
```

---

## 2️⃣ مسار الموقع (VitePress) — الحالة الفعلية والمقترحة

### 2.1 الحالة الفعلية اليوم

```mermaid
flowchart LR
    subgraph CURRENT["⚠️ الحالة الفعلية (لا توجد أتمتة)"]
        MD["📝 docs/*.md<br/>محتوى مكتوب يدوياً"]
        DEV["👨‍💻 المطوِّر<br/>(نسخ يدوي)"]
        WEBDOCS["📁 website/docs/*.md<br/>❌ مفقود من فرع graphic!"]
        WORKFLOW["⚙️ .github/workflows/<br/>deploy-website.yml<br/>⚠️ يستهدف فرع 'main'<br/>غير الموجود"]
        VP["🔨 VitePress build<br/>npm run build"]
        GHPAGES["🌐 origin/gh-pages<br/>(آخر نشر 2026-03-09)"]
    end

    MD -->|"نسخ يدوي"| DEV
    DEV --> WEBDOCS
    WEBDOCS -->|"push to main<br/>❌ معطَّل"| WORKFLOW
    WORKFLOW --> VP
    VP --> GHPAGES

    classDef warn fill:#ca8a04,stroke:#a16207,color:#fff
    classDef missing fill:#dc2626,stroke:#b91c1c,color:#fff,stroke-dasharray:5 5
    classDef ok fill:#16a34a,stroke:#15803d,color:#fff
    
    class MD,DEV,VP ok
    class WORKFLOW warn
    class WEBDOCS missing
    class GHPAGES ok
```

**المشاكل المرصودة:**
1. `website/` غير مُتتبَّع في فرع `graphic` (الفرع الافتراضي الحالي)
2. workflow يستهدف فرع `main` غير الموجود → النشر التلقائي معطَّل
3. لا يوجد ربط بـ`sadinfo` — المحتوى المرجعي مكتوب يدوياً

### 2.2 الحالة المقترحة (الهدف)

```mermaid
flowchart TB
    subgraph SoT2["مصادر الحقيقة"]
        KW2["data/language/<br/>keywords.yaml ✅"]
        BI2["builtin_data.cpp ✅"]
        EM2["error_messages.yaml ❌"]
        LES2["data/lessons/*.yaml ❌"]
        STD2["stdlib/**/*.ص ✅"]
    end

    subgraph CI2["GitHub Actions: gen-docs.yml ❌"]
        STEP1["1) cmake --build sadinfo"]
        STEP2["2) sadinfo --dump-* --format json<br/>→ website/docs/.vitepress/data/"]
        STEP3["3) extractor stdlib → modules.json"]
        STEP4["4) cmake --build sad_wasm<br/>→ website/public/wasm/"]
        STEP5["5) cd website && npm run build"]
        STEP6["6) deploy-pages → gh-pages"]
    end

    subgraph VP2["VitePress Data Layer"]
        LOADER["⚡ .vitepress/data/*.data.ts<br/>(data loaders)"]
        VUE["🎨 Vue components<br/>(KeywordsTable, BuiltinCard...)"]
        MD2["📝 Markdown templates<br/>+ <script setup>"]
    end

    subgraph DIST["مخرجات نهائية في gh-pages"]
        P_KW["/reference/keywords/"]
        P_BI["/reference/builtins/"]
        P_ERR["/reference/errors/"]
        P_STD["/reference/stdlib/"]
        P_LES["/lessons/ (تفاعلية)"]
        P_PLAY["/playground/ (WASM)"]
        P_GUIDES["/guides/ (يدوي)"]
    end

    KW2 --> STEP2
    BI2 --> STEP1
    EM2 -.-> STEP2
    LES2 -.-> STEP2
    STD2 -.-> STEP3

    STEP1 --> STEP2
    STEP2 --> STEP5
    STEP3 -.-> STEP5
    STEP4 -.-> STEP5
    STEP5 --> STEP6

    STEP5 --> LOADER
    LOADER --> VUE --> MD2
    MD2 --> P_KW & P_BI & P_ERR & P_STD & P_LES & P_PLAY & P_GUIDES

    classDef ok fill:#16a34a,stroke:#15803d,color:#fff
    classDef missing fill:#dc2626,stroke:#b91c1c,color:#fff,stroke-dasharray:5 5
    classDef proc fill:#7c3aed,stroke:#6d28d9,color:#fff
    
    class KW2,BI2,STD2 ok
    class EM2,LES2,CI2 missing
    class STEP1,STEP2 ok
    class STEP3,STEP4 missing
    class STEP5,STEP6,LOADER,VUE,MD2 proc
    class P_KW,P_BI,P_STD,P_GUIDES ok
    class P_ERR,P_LES,P_PLAY missing
```

---

## 3️⃣ مسار LSP — الحالة الفعلية والمقترحة

### 3.1 الحالة الفعلية

```mermaid
flowchart LR
    subgraph SRC_LSP["مصادر LSP الحالية"]
        HC1["🔴 ai_suggestions.cpp<br/>سلاسل عربية hardcoded<br/>(سطور 232, 237, 243)"]
        HC2["🔴 hover_provider.cpp<br/>أوصاف ثابتة"]
        HC3["🔴 completion_provider.cpp<br/>قوائم ثابتة"]
        SEMTOK["⚠️ semantic_tokens_provider.cpp<br/>يستخدم enum من token.h"]
    end

    subgraph LSP_CORE["LSP Server (sad-lsp.exe)"]
        STDIO["stdio/JSON-RPC"]
        PROVIDERS["Providers Hub"]
    end

    subgraph EDITORS["محرِّرات"]
        VSC["VS Code Extension"]
        VIM["vim/neovim"]
        EMACS["emacs"]
    end

    HC1 & HC2 & HC3 & SEMTOK -->|"compile-time embed"| PROVIDERS
    PROVIDERS --> STDIO
    STDIO --> VSC & VIM & EMACS

    classDef bad fill:#dc2626,stroke:#b91c1c,color:#fff
    classDef warn fill:#ca8a04,stroke:#a16207,color:#fff
    classDef ok fill:#16a34a,stroke:#15803d,color:#fff
    
    class HC1,HC2,HC3 bad
    class SEMTOK warn
    class STDIO,PROVIDERS,VSC,VIM,EMACS ok
```

**المشاكل:**
- النصوص العربية مدفونة في الكود C++ → تعديل وصف دالة = إعادة بناء LSP
- لا مصدر حقيقة موحَّد بين LSP وsadinfo وuncomment
- ترجمة إنجليزية مستحيلة بدون refactor

### 3.2 الحالة المقترحة (sadinfo as backbone)

```mermaid
flowchart LR
    subgraph SOT3["مصادر الحقيقة"]
        KW3["keywords.yaml ✅"]
        BI3["builtin_data.cpp ✅"]
        EM3["error_messages.yaml ❌"]
    end

    subgraph BUILD3["وقت البناء"]
        SI["sadinfo --dump-*<br/>--format json --lang both"]
        EMBED["resources/<br/>lsp_data.json<br/>(يُحقن في binary أو<br/>يُقرأ وقت التشغيل)"]
    end

    subgraph LSP3["LSP Server المُعاد هيكلته"]
        LOADER3["🔄 DataRegistry<br/>(يقرأ lsp_data.json مرة واحدة)"]
        H_PROV["HoverProvider<br/>→ registry.getKeyword(name).description[lang]"]
        C_PROV["CompletionProvider<br/>→ registry.getAllBuiltins()"]
        D_PROV["DiagnosticProvider<br/>→ registry.getError(code).message[lang]"]
    end

    KW3 & BI3 --> SI
    EM3 -.-> SI
    SI --> EMBED
    EMBED --> LOADER3
    LOADER3 --> H_PROV & C_PROV & D_PROV

    H_PROV & C_PROV & D_PROV --> EDITORS3["VS Code / vim / emacs"]

    classDef ok fill:#16a34a,stroke:#15803d,color:#fff
    classDef missing fill:#dc2626,stroke:#b91c1c,color:#fff,stroke-dasharray:5 5
    classDef proc fill:#7c3aed,stroke:#6d28d9,color:#fff
    
    class KW3,BI3,SI,EMBED ok
    class EM3 missing
    class LOADER3,H_PROV,C_PROV,D_PROV,EDITORS3 proc
```

**المزايا:**
- مصدر حقيقة واحد لكل من LSP/الموقع/الـformatter
- دعم متعدد اللغات (ar/en/both) مجاناً
- تحديث الوصف = تعديل YAML واحد + إعادة بناء

---

## 4️⃣ مسارات الأدوات الأخرى

### 4.1 المفسر `sad.exe` والمترجم `sadc.exe`

```mermaid
flowchart LR
    KW4["data/language/<br/>keywords.yaml ✅"]
    GEN4["scripts/codegen/<br/>gen_keywords.py ✅"]
    HDR4["shared/lexer/include/<br/>keywords_generated.h ✅"]
    KWTBL["shared/lexer/src/<br/>lexer_keywords.cpp<br/>(يستهلك الـ.h)"]
    LEXER4["LexerCore ✅"]
    SAD["sad.exe<br/>المفسر ✅"]
    SADC["sadc.exe<br/>المترجم ✅"]

    KW4 -->|"compile time<br/>cmake target"| GEN4
    GEN4 -->|"يولِّد"| HDR4
    HDR4 -->|"#include"| KWTBL
    KWTBL --> LEXER4
    LEXER4 --> SAD & SADC

    classDef ok fill:#16a34a,stroke:#15803d,color:#fff
    classDef proc fill:#7c3aed,stroke:#6d28d9,color:#fff
    
    class KW4 ok
    class GEN4,HDR4 proc
    class KWTBL,LEXER4,SAD,SADC ok
```

**الحالة:** ✅ **هذا المسار يعمل بشكل كامل ومُختبر** — هو المسار الوحيد المنفَّذ بدون فجوات.

### 4.2 الـ Formatter

```mermaid
flowchart LR
    KW5["keywords.yaml ✅"]
    HC5["🔴 formatter rules<br/>hardcoded في C++"]
    FMT["sad-fmt"]
    OUT[".ص formatted output"]

    KW5 -.->|"لا يستخدمه"| FMT
    HC5 -->|"compiled in"| FMT
    FMT --> OUT

    %% المسار المقترح
    SI5["sadinfo JSON<br/>(formatting hints)"]
    KW5 -.->|"مستقبلاً"| SI5
    SI5 -.->|"runtime/build"| FMT

    classDef ok fill:#16a34a,stroke:#15803d,color:#fff
    classDef bad fill:#dc2626,stroke:#b91c1c,color:#fff
    classDef plan fill:#6b7280,stroke:#4b5563,color:#fff,stroke-dasharray:5 5
    
    class KW5,OUT ok
    class HC5 bad
    class FMT ok
    class SI5 plan
```

### 4.3 VS Code Extension

```mermaid
flowchart TB
    subgraph CURR_EXT["الحالة الفعلية"]
        TM["⚠️ syntaxes/sad.tmLanguage.json<br/>قائمة كلمات يدوية"]
        SNIP["⚠️ snippets/sad.json<br/>قوالب يدوية"]
        EXT_LSP["✅ يستدعي sad-lsp"]
    end

    subgraph PROP_EXT["الحالة المقترحة"]
        GEN_TM["⚙️ سكريبت build<br/>sadinfo → tmLanguage"]
        GEN_SNIP["⚙️ سكريبت build<br/>sadinfo → snippets"]
    end

    KW6["keywords.yaml ✅"] -.->|"عبر sadinfo"| GEN_TM
    BI6["builtin_data.cpp ✅"] -.->|"عبر sadinfo"| GEN_SNIP
    GEN_TM -.-> TM
    GEN_SNIP -.-> SNIP

    classDef ok fill:#16a34a,stroke:#15803d,color:#fff
    classDef warn fill:#ca8a04,stroke:#a16207,color:#fff
    classDef plan fill:#6b7280,stroke:#4b5563,color:#fff,stroke-dasharray:5 5
    
    class TM,SNIP warn
    class EXT_LSP,KW6,BI6 ok
    class GEN_TM,GEN_SNIP plan
```

### 4.4 REPL ومدير الحزم

```mermaid
flowchart LR
    subgraph REPL_PATH["REPL (.sad)"]
        REPL_HC["⚠️ help messages hardcoded"]
        REPL_BIN["sad --repl"]
    end

    subgraph PKG_PATH["sad-pkg"]
        PKG_REG["📊 data/packages.db<br/>(SQLite, محذوف من git عمداً)"]
        PKG_MAN["📄 packages.json<br/>(manifest)"]
        PKG_BIN["sad-pkg"]
    end

    REPL_HC --> REPL_BIN
    PKG_REG --> PKG_BIN
    PKG_MAN --> PKG_BIN

    SI_FUTURE["sadinfo JSON"] -.->|"مستقبلاً"| REPL_BIN

    classDef ok fill:#16a34a,stroke:#15803d,color:#fff
    classDef warn fill:#ca8a04,stroke:#a16207,color:#fff
    classDef plan fill:#6b7280,stroke:#4b5563,color:#fff,stroke-dasharray:5 5
    
    class PKG_REG,PKG_MAN,PKG_BIN ok
    class REPL_HC,REPL_BIN warn
    class SI_FUTURE plan
```

---

## 5️⃣ المسار الموحَّد المستقبلي: sadinfo كـ"hub"

التصميم النهائي المقترح يجعل `sadinfo` نقطة التوزيع المركزية:

```mermaid
flowchart TB
    subgraph SoT_HUB["🔵 طبقة مصادر الحقيقة"]
        direction LR
        S1["keywords.yaml"]
        S2["builtin_data.cpp"]
        S3["error_messages.yaml"]
        S4["modules.yaml"]
        S5["lessons/*.yaml"]
    end

    subgraph HUB["🛠️ sadinfo — Documentation Hub"]
        direction TB
        CMD["sadinfo CLI:<br/>--dump-keywords<br/>--dump-builtins<br/>--dump-errors<br/>--dump-modules<br/>--dump-lessons<br/>--dump-all"]
        FMT_OPT["formats: json | yaml<br/>langs: ar | en | both<br/>filters / pretty / minimal"]
    end

    subgraph CONSUMERS_HUB["🎯 المستهلكون"]
        direction TB
        C_WEB["🌐 Website<br/>(VitePress data loaders)"]
        C_LSP["💡 LSP<br/>(DataRegistry)"]
        C_IDE["🔌 VS Code Ext<br/>(build-time gen)"]
        C_FMT["🎨 Formatter<br/>(formatting hints)"]
        C_REPL["💻 REPL<br/>(help system)"]
        C_CI["🤖 CI Pipelines<br/>(validation, diff)"]
        C_BOT["🧠 AI / Bots<br/>(training data)"]
        C_3RD["🔗 أطراف خارجية<br/>(JSON Schema users)"]
    end

    S1 & S2 & S3 & S4 & S5 --> CMD
    CMD --> FMT_OPT
    FMT_OPT --> C_WEB & C_LSP & C_IDE & C_FMT & C_REPL & C_CI & C_BOT & C_3RD

    classDef sot fill:#1e3a8a,stroke:#1e40af,color:#fff
    classDef hub fill:#7c3aed,stroke:#6d28d9,color:#fff
    classDef cons fill:#16a34a,stroke:#15803d,color:#fff
    
    class S1,S2,S3,S4,S5 sot
    class CMD,FMT_OPT hub
    class C_WEB,C_LSP,C_IDE,C_FMT,C_REPL,C_CI,C_BOT,C_3RD cons
```

**المبدأ:** أي مستهلك جديد لا يحتاج تعديل اللغة — فقط استدعاء `sadinfo` بالمعاملات المناسبة.

---

## 6️⃣ مخطط الأولويات والاعتماديات

ترتيب التنفيذ المنطقي للوصول إلى المعمارية الكاملة:

```mermaid
flowchart TB
    A["✅ keywords.yaml + gen_keywords.py<br/>(منفَّذ)"]
    B["✅ sadinfo --dump-keywords/builtins<br/>(منفَّذ)"]
    
    C1["🎯 إصلاح deploy-website.yml<br/>(main → graphic)"]
    C2["🎯 استعادة website/docs/ المفقود"]
    C3["🎯 POC: keywords.data.ts + صفحة VitePress"]
    
    D1["📋 إنشاء error_messages.yaml"]
    D2["📋 sadinfo --dump-errors"]
    
    E1["📋 ربط LSP بـsadinfo (DataRegistry)"]
    E2["📋 ربط VS Code Ext بـsadinfo"]
    
    F1["📋 stdlib doc extractor"]
    F2["📋 modules.yaml + --dump-modules"]
    
    G1["📋 lessons schema"]
    G2["📋 sad.wasm integration"]
    G3["📋 playground page"]
    
    H["🚀 نظام توثيق متكامل"]

    A --> B
    B --> C1 & C2
    C1 --> C3
    C2 --> C3
    C3 --> H

    A --> D1 --> D2
    D2 --> E1
    B --> E1
    E1 --> E2
    
    A --> F1 --> F2
    F2 --> H
    
    A --> G1 --> G2 --> G3
    G3 --> H

    D2 --> H
    E1 --> H
    E2 --> H

    classDef done fill:#16a34a,stroke:#15803d,color:#fff
    classDef next fill:#ca8a04,stroke:#a16207,color:#fff
    classDef future fill:#6b7280,stroke:#4b5563,color:#fff
    classDef goal fill:#7c3aed,stroke:#6d28d9,color:#fff
    
    class A,B done
    class C1,C2,C3 next
    class D1,D2,E1,E2,F1,F2,G1,G2,G3 future
    class H goal
```

---

## 7️⃣ مقارنة سريعة: قبل وبعد

| الجانب | اليوم | المستقبل |
|---|---|---|
| **مصدر أسماء الكلمات المفتاحية** | `keywords.yaml` ✅ | `keywords.yaml` ✅ |
| **مصدر الدوال المضمنة** | `builtin_data.cpp` (C++) | `builtins.yaml` (مقترح) |
| **مصدر رسائل الأخطاء** | مبعثرة في الكود ❌ | `error_messages.yaml` |
| **توثيق الموقع** | Markdown يدوي | متولِّد من sadinfo |
| **توثيق LSP (hover/completion)** | hardcoded في C++ | متولِّد من sadinfo |
| **VS Code syntax highlighting** | tmLanguage يدوي | متولِّد من sadinfo |
| **عدد مصادر "الحقيقة" لكلمة `إذا`** | 4+ (لكسر، LSP، Ext، docs) | 1 (`keywords.yaml`) |
| **تعديل وصف دالة `اطبع`** | تعديل في 5 ملفات | تعديل في `builtin_data.cpp` أو YAML واحد |

---

## 📚 ملفات مرجعية

- [ARCHITECTURE_MAP.md](_bmad-output/systems/doc-ir/ARCHITECTURE_MAP.md) — الخريطة المعمارية الشاملة
- [DOC_FLOW_REALITY.md](_bmad-output/systems/doc-ir/DOC_FLOW_REALITY.md) — تحليل الفجوة بين المخطط والواقع
- [tools/sadinfo/README.md](tools/sadinfo/README.md) — توثيق الأداة الفعلي
- [tools/sadinfo/CHANGELOG.md](tools/sadinfo/CHANGELOG.md) — تاريخ الإصدارات
- [.github/workflows/deploy-website.yml](.github/workflows/deploy-website.yml) — workflow النشر الحالي

---

**ملاحظة:** المخططات أعلاه يمكن تصديرها كصور PNG/SVG عبر:
- VS Code: امتداد **Markdown Preview Mermaid Support**
- CLI: `npx @mermaid-js/mermaid-cli -i DOC_DISTRIBUTION_FLOWS.md -o flows.png`
- الموقع نفسه (VitePress): يدعم Mermaid مدمجاً بعد تفعيل `markdown.config.mermaid`
