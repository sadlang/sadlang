# 🔍 Gap Analysis v2 — الإصدار النهائي بعد فحص website + tools/check

> **التاريخ:** 2026-05-09 (تحديث بعد اكتشاف website)
> **يستبدل:** `gap-analysis-docs-system.md` (v1)
> **ملخص الصدمة:** **85%+ من PRD الأصلي موجود بالفعل** — بما في ذلك VitePress site منشور
> **التقدير الجديد:** 31 يوم → **15 يوم** (-52%)

---

## 🎯 الواقع الكامل المُكتشف

### ✅ المنطقة 1: نواة التوثيق (`compiler/include/meta/` + `shared/ast/`)

| المكوّن | الموقع | الحالة |
|---|---|---|
| `DocsExtractor` (AST-based) | `shared/ast/include/docs_extractor.h` | ✅ كامل |
| `DocsEmitter` (HTML/MD/JSON) | `compiler/include/meta/docs_emitter.h` | ✅ كامل |
| `DocCommentParser` | داخل `docs_emitter.h` | ✅ كامل |
| 17 وسم AR/EN | `shared/ast/include/doc_comment.h` | ✅ كامل |
| 4 themes (dark/light/ocean/desert) | `DocsConfig` | ✅ معرّف |
| Cross-references | `buildCrossReferences()` | ✅ |
| Search index generation | `generateSearchIndex()` | ✅ |
| C-compatible API | `sadc_emit_docs()` | ✅ |

### ✅ المنطقة 2: CLI Integration

| الراية | الحالة |
|---|---|
| `--docs file.ص` | ✅ في sad + sadc |
| `--docs-out=path` | ✅ |
| `--docs-format=md\|json\|html` | ✅ |
| `--docs-project=<dir>` (recursive) | ✅ |
| `--docs-project-name=<name>` | ✅ |
| `--docs-exclude=<pattern>` | ✅ |
| `--وثّق` (Arabic alias) | ✅ |
| **byte-for-byte sad↔sadc parity** | ✅ مُختبر |

### ✅ المنطقة 3: الاختبارات المزدوجة

| المقياس | القيمة |
|---|---|
| **الموقع** | `tests/doc_gen_dual_execution/` |
| **النتيجة** | **44/44 ✓** |
| **الصيغ المُختبرة** | markdown + json + html + pdf |
| **زمن التنفيذ** | ~2 ثانية بدون pdf |
| **CRLF/LF normalization** | ✅ مُعالج |
| **PDF metadata tolerance** | ✅ 5KB |

### ✅ المنطقة 4: الموقع الرسمي (`C:\s_lang\website`) ⭐ **اكتشاف ضخم**

| المكوّن | الحالة |
|---|---|
| **VitePress 1.6.3** | ✅ مُثبَّت ويعمل |
| **gh-pages deployment** | ✅ مُعد بالكامل (`npm run deploy`) |
| **Vue 3 + CodeMirror playground** | ✅ كامل |
| **Playwright** | ✅ مُثبَّت للاختبارات |
| **i18n عربي/إنجليزي** | ✅ مجلد `en/` موجود |
| **مجلد `book/`** | ✅ كتاب اللغة |
| **مجلد `learn/`** | ✅ تعلّم تفاعلي |
| **مجلد `playground/`** | ✅ playground v1+v2 |
| **مجلد `project-docs/`** | ✅ توثيق مشاريع |
| **مجلد `blog/`** | ✅ مدوّنة |
| **`compare.md`** | ✅ مقارنات لغات |
| **`api/server.js`** | ✅ Express endpoint |
| **WASM build** | ✅ `build_wasm/` + `build_wasm_new/` |
| **theme مخصص** | ✅ `.vitepress/theme/` |

### ✅ المنطقة 5: أدوات الفحص

| الأداة | الحالة | الفائدة لنا |
|---|---|---|
| `tools/check/sad-check` | ✅ فاحص ملكية فقط | غير مفيد لـ docs |
| `tools/lsp/` | ✅ LSP server كامل | يحتاج keywords (مثل docgen) |
| `tools/formatter/` | ✅ يستخدم نفس KeywordTable | يثبت أن KeywordTable قابل للوصول |

---

## ❌ Gap الحقيقي (الناقص فعلاً)

### 🔴 Gap A: استخراج البيانات الوصفية للغة (المحرك)

**المشكلة:** `DocsExtractor` يستخرج من **كود لغة ص** (.ص) فقط. **لا يستخرج من نواة المفسر/المترجم نفسه:**

| البيانات | المصدر | الحالة |
|---|---|---|
| 40 كلمة محجوزة عربية + EN | `shared/lexer/src/lexer_keywords.cpp` | ❌ runtime-only |
| 25 كلمة سياقية | نفس الملف | ❌ runtime-only |
| ~21 دالة مدمجة + توقيعاتها | `interpreter/builtins/` | ❌ مبعثر |
| رسائل الأخطاء (lexer/parser/sema) | `shared/errors/` | ❌ غير مُنظَّم |
| تعليمات SIR | `compiler_new/src/sir/sir_opcodes.h` | ❌ |
| التوجيهات `@حجم/@ذري/...` | لا مكان مركزي | ❌ مبعثر |

**الحل:** أداة جديدة `sadinfo.exe` (binary مستقل) تستخدم `shared/lexer` + `shared/parser` + `shared/types` لاستخراج هذه البيانات وإصدارها كـ JSON/YAML.

### 🔴 Gap B: Quality Gates (G1-G7)

| Gate | الموجود | الناقص |
|---|---|---|
| G1: Coverage (≥95%) | ❌ | تحتاج keyword checker |
| G2: Drift (no new keyword without doc) | ❌ | pre-commit + CI |
| G3: Examples executability | جزئي (dual tests) | يحتاج توسعة |
| G4: Bilingual (AR+EN) | ❌ | validator |
| G5: Performance (<60s build) | ❌ | benchmarks |
| G6: A11y (WCAG 2.1 AA) | ❌ | axe-playwright في website |
| G7: Flake (<1%) | ❌ | retry + monitoring |

### 🟡 Gap C: AI Drafts Pipeline

| المطلوب | الحالة |
|---|---|
| YAML schema للمحرَّر يدوياً | ❌ |
| `*.draft.yaml` نظام لمسوّدات AI | ❌ |
| Claude API integration | ❌ |
| pre-commit hook يحظر `.draft.yaml` في النشر | ❌ |
| Human review workflow | ❌ |

### 🟡 Gap D: Distribution كحزم

| الهدف | الحالة |
|---|---|
| `@sad-lang/docs-data` على npm | ❌ |
| Python `sad-doctest` على PyPI | ❌ (الموجود `runner.py` فقط) |
| استهلاك من website (`api/server.js` يقدم JSON) | جزئي ⚠️ |

### 🟢 Gap E: تكامل website ↔ sadinfo (Wiring)

**المشكلة:** website موجود + sadinfo سيُبنى — لكن لا اتصال بينهما حالياً.

| المطلوب | الحل |
|---|---|
| website يقرأ `keywords.json` من sadinfo | dataLoader في `.vitepress/config.mts` |
| website يقرأ `builtins.json` | نفسه |
| website يعرض في `book/keywords/` | Vue component |
| نشر تلقائي عند تحديث keywords | GitHub Action |

---

## 📊 إعادة تقدير الجهد (نهائي)

| Sprint | الأصلي v1 | بعد v1 | بعد v2 (الآن) | السبب |
|---|---|---|---|---|
| Sprint 0 | 5 أيام | 2 يوم | **0.5 يوم** | website + tests + docgen كلها موجودة |
| Wave 1 (sadinfo + Gap A) | 8 أيام | 5 أيام | **5 أيام** | بناء أداة جديدة (لا اختصار) |
| Wave 2 (AI YAML) | 6 أيام | 6 أيام | **5 أيام** | استخدام نمط dual tests الموجود |
| Wave 3 (Site integration) | 8 أيام | 8 أيام | **2 يوم** | website موجود! فقط dataLoaders + components |
| Wave 4 (Distribution) | 4 أيام | 4 أيام | **2.5 يوم** | gh-pages موجود، نضيف npm + PyPI |
| **المجموع** | **31 يوم** | **25 يوم** | **15 يوم** | **توفير 16 يوم (-52%)** |

---

## 🏗️ القرارات المعمارية النهائية

### قرار 1: بنية sadinfo.exe (binary مستقل)

```
tools/sadinfo/
├── CMakeLists.txt
├── src/
│   ├── main.cpp              # CLI dispatcher
│   ├── dump_keywords.cpp     # KeywordTable → JSON
│   ├── dump_builtins.cpp     # BuiltinRegistry → JSON
│   ├── dump_errors.cpp       # ErrorMessages → JSON
│   ├── dump_directives.cpp   # @حجم/@ذري/... → JSON
│   └── output_format.cpp     # JSON/YAML emitters
└── include/
    └── sadinfo/
        └── version.h
```

**يلتزم بـ:**
- `target_link_libraries(sadinfo PRIVATE sad_lexer_core sad_shared)` — يستخدم نفس بنية sad/sadc
- لا يكرر الكود
- binary صغير (لا LLVM، لا backend)
- نفس KeywordTable التي يستخدمها sad/sadc/lsp/formatter

### قرار 2: لا حاجة لـ tools/docgen المنفصل

**المبرر:** `compiler/include/meta/docs_emitter.h` يفعل ما يفعله docgen — أكثر وأفضل.

### قرار 3: sadinfo CLI

```bash
sadinfo --dump-keywords [--format=json|yaml] [--lang=ar|en|both] [--output=file]
sadinfo --dump-builtins [--format=...]
sadinfo --dump-errors [--format=...]
sadinfo --dump-directives [--format=...]
sadinfo --dump-all [--output-dir=docs/_generated/]   # كل البيانات
sadinfo --validate <yaml-file>                       # تحقق YAML schema
sadinfo --version
sadinfo --help
```

### قرار 4: تكامل مع website

```
C:\s_lang\website\
├── docs/_generated/         # ← sadinfo --dump-all يكتب هنا
│   ├── keywords.json
│   ├── builtins.json
│   ├── errors.json
│   └── directives.json
└── .vitepress/
    └── config.mts           # ← يقرأ JSON عبر dataLoaders
```

GitHub Action: عند تغيير `KeywordTable` في sad → run sadinfo → commit JSON → website يبني تلقائياً.

### قرار 5: AI YAML pipeline

```
website/docs/_drafts/        # ← مسودات AI (gitignored من النشر)
website/docs/_published/     # ← مراجَعة بشرياً + موقّعة
tools/sad-ai-docgen/         # ← Python script يستهلك Claude API
```

---

## 📋 Mapping النهائي: Stories الجديدة

| # | Story | يبني على | جديد؟ |
|---|---|---|---|
| 0.0 | Bootstrap (website check + secrets) | website موجود | ⚠️ مبسّطة جداً |
| 0.1 | Test framework توسعة dual_execution | tests/doc_gen_dual_execution موجود | ⚠️ توسعة |
| 1.1 | sadinfo.exe skeleton | shared/lexer | ✅ جديد |
| 1.2 | sadinfo --dump-keywords | KeywordTable | ✅ جديد |
| 1.3 | sadinfo --dump-builtins | BuiltinRegistry | ✅ جديد |
| 1.4 | sadinfo --dump-errors | shared/errors | ✅ جديد |
| 1.5 | sadinfo --dump-directives | scattered → unified | ✅ جديد |
| 1.6 | YAML schema + sadinfo --validate | جديد | ✅ جديد |
| 1.7 | check_keywords.py (G1+G2) | sadinfo output | ✅ جديد |
| 1.8 | CI workflow docs-quality.yml | جديد | ✅ جديد |
| 2.1 | Claude API integration | جديد | ✅ جديد |
| 2.2 | AI draft → human review pipeline | جديد | ✅ جديد |
| 2.3 | Bilingual validator | جديد | ✅ جديد |
| 3.1 | website dataLoaders للـ JSON | website/.vitepress | ⚠️ توسعة |
| 3.2 | Vue components (KeywordCard, BuiltinPlayground) | website موجود | ⚠️ توسعة |
| 3.3 | i18n switching للـ generated content | website/en موجود | ⚠️ توسعة |
| 3.4 | A11y axe-playwright tests | website + Playwright موجودان | ⚠️ توسعة |
| 4.1 | npm package @sad-lang/docs-data | جديد | ✅ جديد |
| 4.2 | PyPI sad-doctest | runner.py موجود | ⚠️ توسعة |
| 4.3 | gh-pages action تلقائي | website/.github موجود | ⚠️ توسعة |

**الإجمالي:** 19 story (بدلاً من 28). 9 منها توسعات لأشياء موجودة.

---

## ✅ Approvals مطلوبة

من إجابات صلاح:
- ✅ **Q1:** أداة مستقلة (sadinfo) تستخدم الطبقة المشتركة — **مُوافَق**
- ✅ **Q2:** sadinfo.exe منفصل (لا --dump-* في sad/sadc) — **مُوافَق**
- ✅ **Q3:** إعادة كتابة كل الـ4 وثائق — **مُوافَق**

**موافقة إضافية مطلوبة (تأكيد):**
- ❓ هل نستخدم `C:\s_lang\website` كـ deployment target (موصى به) أم ننشئ موقع مستقل في `s-programming-language/docs/`؟

> **توصية Winston:** استخدم website الموجود — كل البنية التحتية جاهزة، مع subfolder `docs/_generated/` للمحتوى المُولَّد من sadinfo.

---

> **توقيع:** Amelia (Dev) + Winston (Architect)
> **حالة:** جاهز لإعادة كتابة PRD v2 + Architecture v2 + Epics v2 + Story 0.0 v2
