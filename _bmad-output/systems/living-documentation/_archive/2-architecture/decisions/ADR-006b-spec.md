---
stepsCompleted: ["spec-draft"]
project: s-programming-language
feature: ADR-006b — pipeline توليد التوثيق
status: draft
created: 2026-07-09
author: Amelia (dev agent)
reviewedBy: []
---

# مواصفات ADR-006b — pipeline توليد التوثيق تلقائياً

**الحالة:** مسوَّدة — تحتاج مراجعة بشرية قبل الاعتماد  
**المرتبط بـ:** ADR-006a (مُعتمَد ✅ — UTM-6.0 → UTM-6.7 مكتملة)  
**شرط البدء:** ADR-006a مستقرة لمدة 3 أشهر + موافقة المستخدم saleh على هذه المواصفات عبر PR review  

> **تعريف Document-IR:** تمثيل وسيط (Python dataclasses في الذاكرة أثناء التوليد) يفصل بين بيانات YAML source وأي renderer مخرجات. LSP renderer يُنتج ملف `builtins.json` على القرص — هذا ليس DocIR بل **مخرج** renderer يستهلك DocIR.  

---

## 1. السياق والمشكلة

### 1.1. الوضع الحالي

لغة ص تملك **552 دالة مدمجة في 32 namespace** (مُولَّدة بـ ADR-006a) + **80 طريقة نوع** + **10 وحدات**. التوثيق الحالي:

| الملف/المجلد | الحالة | المشكلة |
|---|---|---|
| `docs/` (Markdown يدوي) | موجود، غير مكتمل | لا يُغطي إلا ~20% من الدوال |
| `وثائق/` (Markdown يدوي) | موجود، متقادم | تعريفات مختلفة عن YAML الرسمي |
| `mkdocs.yml` | موجود، أساسي | لا versioning، لا i18n، لا search optimization |
| توثيق LSP | غائب | المحرر لا يعرض hover docs |
| توثيق REPL | غائب | `:help دالة` لا يعمل |

### 1.2. المشكلات الجذرية

| المشكلة | التأثير |
|---|---|
| التوثيق اليدوي يتباعد عن YAML الرسمي مع كل تغيير | مطورو لغة ص يقرؤون توثيقاً خاطئاً |
| لا versioning — لا يعرف المطور متى ظهرت الدالة | تحديث لغة ص يكسر الكود الموجود بدون تحذير |
| قناة واحدة (MkDocs) — لا LSP ولا REPL ولا man pages | تجربة IDE رديئة مقارنة بـ Python/Rust |
| Jinja2 مباشر إلى Markdown = lock-in | لاحقاً لن يمكن توليد LSP/REPL/PDF بدون إعادة الكتابة |
| لا Diátaxis framework | خلط tutorial مع reference يُشوِّش المطور المبتدئ |

---

## 2. النطاق (Scope)

### 2.1. داخل النطاق ✅

- **Document-IR:** تمثيل وسيط (JSON/YAML) بين بيانات YAML وأي renderer
- **نظام Versioning:** حقول `since`, `deprecated_in`, `stability` في جميع YAML schemas
- **Diátaxis framework:** حقل `doc_type: reference|tutorial|how-to|explanation` في كل doc node
- **قنوات المخرجات الأربع:**
  1. VitePress — موقع الويب
  2. LSP MarkupContent — hover docs في المحررات
  3. REPL `:help` — مساعد تفاعلي داخل REPL
  4. man pages — لسطر الأوامر (Linux/macOS فقط — يتطلب `groff` غير متاح على Windows بشكل مدمج)
- **مصفوفة الترحيل:** خطة واضحة لكل ملف Markdown موجود (يبقى / يُحوَّل / يُحذف)
- **CI Validation:**
  - أمثلة `runnable: true` تُشغَّل بـ `sad.exe` وتُطابق `expected_output`
  - كل API عام له توثيق (enforce_docs_coverage)
  - لا تعارض بين الأسماء القانونية في YAML والتوثيق
- **pipeline AI لكتابة المحتوى:** مواصفات YAML → وكيل AI يكتب الدرس → مراجعة بشرية

### 2.2. خارج النطاق ❌

- تغيير ADR-006a أو gen_all.py (مستقرة بالفعل)
- playground تفاعلي في المتصفح (مرحلة لاحقة)
- i18n كامل للتوثيق إلى الإنجليزية — البنية التحتية (website/ + VitePress config) **موجودة بالفعل** (i18n + RTL + Shiki)، التنفيذ الكامل للترجمة مُؤجَّل لـUTM-6.11 (خارج نطاق UTM-6.8⁩13)
- مدونة أو أخبار (ليست توثيقاً)

---

## 3. المعمارية المقترحة

### 3.1. الطبقات الثلاث

> **ملاحظة:** `data/docs/` مجلد جديد يُنشأ في Story 1.1 مع تحديث `_lib/loader.py` لتضمين مساره. هذا التغيير لا يمس ADR-006a.

```
┌─────────────────────────────────────────────────────────────┐
│             طبقة البيانات (Data Layer)                      │
│  data/language/ (YAML sources — مُنشأة بـ ADR-006a)         │
│  data/docs/ (NEW يُنشأ في Story 1.1) — محتوى تعليمي         │
│    ├── lessons/     ← دروس YAML (tutorial/how-to)           │
│    └── examples/    ← أمثلة مختبَرة                         │
├─────────────────────────────────────────────────────────────┤
│             Document-IR (طبقة وسيطة)                       │
│  scripts/codegen/doc_ir/                                    │
│    ├── doc_ir_builder.py    ← YAML → DocIR (Python objects) │
│    ├── doc_ir_schema.json   ← JSON Schema للـ DocIR         │
│    └── doc_ir_validator.py  ← تحقق من DocIR                 │
├─────────────────────────────────────────────────────────────┤
│             Renderers (طبقة التحويل)                        │
│  scripts/codegen/renderers/                                 │
│    ├── render_vitepress.py ← DocIR → VitePress Markdown   │
│    ├── render_lsp.py        ← DocIR → LSP JSON              │
│    ├── render_repl.py       ← DocIR → REPL help strings     │
│    └── render_man.py        ← DocIR → man page              │
└─────────────────────────────────────────────────────────────┘
```

### 3.2. Document-IR Schema (مبسَّط)

```json
{
  "type": "object",
  "required": ["id", "doc_type", "canonical_name", "description_ar"],
  "properties": {
    "id": { "type": "string" },
    "doc_type": { "enum": ["reference", "tutorial", "how-to", "explanation"] },
    "canonical_name": { "type": "string" },
    "namespace": { "type": "string" },
    "description_ar": { "type": "string" },
    "description_en": { "type": "string" },
    "since": { "type": "string", "pattern": "^\\d+\\.\\d+\\.\\d+$" },
    "deprecated_in": { "type": ["string", "null"] },
    "stability": { "enum": ["stable", "experimental", "deprecated"] },
    "examples": {
      "type": "array",
      "items": {
        "type": "object",
        "required": ["code"],
        "properties": {
          "code": { "type": "string" },
          "runnable": { "type": "boolean" },
          "expected_output": { "type": "string" }
        }
      }
    },
    "see_also": { "type": "array", "items": { "type": "string" } }
  }
}
```

### 3.3. التكامل مع gen_all.py

```python
# يُضاف في _make_registry() لاحقاً كمولِّد 5:
{
    "name": "docs",
    "script": codegen / "gen_docs.py",
    "args": [
        "--data-dir", str(repo / "data"),
        "--out-dir", str(repo / "docs" / "generated"),
        "--channels", "vitepress,lsp,repl",
    ],
}
```

---

## 4. متطلبات وظيفية (Functional Requirements)

### FR-1: Document-IR (الأساس)
- **FR-1.1:** `doc_ir_builder.py` يحوِّل جميع YAML sources إلى DocIR objects
- **FR-1.2:** `doc_ir_validator.py` يرفض أي DocIR بمعرّف مكرر أو canonical_name مفقود
- **FR-1.3:** DocIR يُخزَّن في الذاكرة فقط **أثناء مرحلة التوليد** (لا ملفات وسيطة) — كل renderer يُنتج مخرجاته على القرص (مثال: `render_lsp.py` يكتب `docs/generated/lsp/builtins.json`) ولكن DocIR نفسه لا يُحفظ كملف

### FR-2: Versioning في schemas
- **FR-2.1:** حقل `since` إلزامي في جميع entries جديدة (مثال: `"1.0.0"`)
- **FR-2.2:** حقل `stability: stable|experimental|deprecated` إلزامي
- **FR-2.3:** `deprecated_in` اختياري — إذا وُجد يُضيف تحذيراً في جميع القنوات
- **FR-2.4:** الـ entries الموجودة تأخذ `since: "0.1.0"` و `stability: stable` كقيم افتراضية

### FR-3: Diátaxis
- **FR-3.1:** كل doc node له `doc_type` من `[reference, tutorial, how-to, explanation]`
- **FR-3.2:** CI يرفض node واحد يخلط أنواعاً مختلفة
- **FR-3.3:** VitePress renderer يُنشئ sections منفصلة لكل Diátaxis type

### FR-4: Renderers
- **FR-4.1:** `render_vitepress.py` يُنتج VitePress Markdown pages بـ frontmatter متوافق (لا MDX — Markdown عادي + YAML frontmatter)
- **FR-4.2:** `render_lsp.py` يُنتج JSON بحقول `hover_text`, `completion_detail`, `completion_doc`
- **FR-4.3:** `render_repl.py` يُنتج نصوص `:help` للـ REPL
- **FR-4.4:** كل renderer يُنتج فقط ما تغيَّر (write_if_changed pattern)

### FR-5: مصفوفة الترحيل
- **FR-5.1:** ملف `data/docs/migration_matrix.yaml` يحدد مصير كل ملف Markdown موجود
- **FR-5.2:** CI guard يمنع إنشاء ملفات Markdown يدوية جديدة في `docs/` إذا كانت مُغطاة بـ YAML
- **FR-5.3:** 100 ملف Markdown ذو أولوية عالية يُحَّوَل في UTM-6.11

### FR-6: CI Validation
- **FR-6.1:** كل مثال بـ `runnable: true` يُشغَّل ضد `sad.exe` ويُطابق `expected_output`
- **FR-6.2:** `enforce_docs_coverage` يتحقق أن كل دالة في YAML لها `description_ar`
- **FR-6.3:** CI يفشل إذا كانت الأسماء القانونية في التوثيق مختلفة عن YAML

---

## 5. متطلبات غير وظيفية (Non-Functional Requirements)

| المتطلب | الهدف |
|---------|-------|
| الأداء | gen_docs.py يكتمل خلال ≤ 30 ثانية على 600+ entry |
| التغطية | ≥ 95% من YAML entries لها `description_ar` قبل UTM-6.13 |
| الدقة | 0 تناقض بين YAML canonical names والتوثيق |
| عدم التراجع | gen_all.py يُبلِّغ 4/4 (ثم 5/5) passed بعد إضافة gen_docs — هذا AC في Story 2.1 وليس NFR |
| LSP | hover يظهر خلال ≤ 100ms في VS Code + Vim/Neovim |
| التوليد التزايدي | gen_docs.py يُعيد توليد namespace مُحدَّث فقط (file hash comparison) — لا يُعيد توليد جميع الـ 600 entry عند تغيير ملف واحد |

---

## 6. مصفوفة الترحيل (Migration Matrix) — جدول أولي

> **تحذير:** هذا الجدول **أولي فقط** — يغطي أبرز الملفات. القائمة الكاملة تُبنى في Story 4.1 بعد تعداد آلي لجميع ملفات `docs/` و `وثائق/`. لا تُحذف أي ملف بناءً على هذا الجدول وحده.

| الملف | المصير | السبب | المالك |
|-------|--------|-------|-------|
| `docs/SAD_LANGUAGE_COMPLETE_REFERENCE.md` | يُولَّد جزئياً من YAML | الأقسام المُولَّدة: جداول الدوال + طرق الأنواع. يبقى يدوياً: الدروس وأمثلة OOP والتعديلات السردية. الحد الفاصل يُحدَّد في Story 4.1 | gen_docs |
| `وثائق/مرجع_الدوال.md` (إن وجد) | يُحذف / يُحوَّل | مغطى بـ builtin YAML | gen_docs |
| `docs/07_البرمجة_الكائنية.md` | يبقى يدوياً | محتوى تعليمي تفسيري | Amelia AI |
| `docs/توثيقات_تطوير_اللغة/` | يبقى يدوياً | توثيق داخلي للمطورين | فريق |
| جميع ملفات `docs/` الأخرى | **تحتاج تصنيف في Story 4.1** | غير مُحدَّد بعد | — |

---

## 7. مخاطر معلومة (Known Risks)

| الخطر | الاحتمال | التأثير | التخفيف |
|-------|---------|---------|---------|
| drift بين DocIR وYAML sources | متوسط | عالٍ | CI يُعيد التوليد في كل commit |
| LSP MarkupContent يحتاج تنسيقاً خاصاً لكل لغة محرر | عالٍ | متوسط | ابدأ بـ VS Code فقط |
| AI-generated content جودة متفاوتة | متوسط | عالٍ | مراجعة بشرية إلزامية لكل درس |
| فشل npm build / VitePress breaking change | منخفض | عالٍ | pin VitePress بإصدار محدد في package.json |

---

## 8. شروط البدء الإلزامية

- [x] ADR-006a مُنفَّذة (UTM-6.0 → UTM-6.7) ✅
- [ ] ADR-006a مستقرة لمدة 3 أشهر بدون regression
- [ ] موافقة على هذه المواصفات من المستخدم (saleh) — عبر تعليق على PR أو رسالة صريحة
- [ ] DocIR schema مُصمَّم ومُعتمَد
- [ ] مصفوفة ترحيل مكتوبة بالكامل

---

## 9.0. قرار المنصة الوثائقية — مُعتمَد ✅

> ✅ **القرار مُتَّخذ:** **VitePress** (مُعدَّل من Docusaurus) — مُوثَّق في [ADR-007](../../docs/توثيقات_تطوير_اللغة/توثيقات_الخطط/ADR-007_منصة_التوثيق_Docusaurus.md) + [ADR-008](../../docs/توثيقات_تطوير_اللغة/توثيقات_الخطط/ADR-008_علاقة_الموقع_بالمشروع.md). السبب: الموقع مبني بالفعل على VitePress مع i18n+RTL+Shiki. Epic 2 تبدأ بنقل `website/` داخل المشروع ثم بناء `render_vitepress.py`.

## 9. المراحل المقترحة (UTM-6.8 → UTM-6.12)

| المرحلة | الهدف | المُخرَج |
|---------|-------|---------|
| **UTM-6.8** | Document-IR + Versioning في schemas | `doc_ir_schema.json` + حقول `since/stability` في كل YAML |
| **UTM-6.9** | Renderer 1 (VitePress) + CI examples runner | موقع VitePress مُولَّد من YAML |
| **UTM-6.10** | Renderers 2-4 (LSP + REPL + man) | hover docs في VS Code + `:help` في REPL |
| **UTM-6.11** | مصفوفة ترحيل + migrate أولوية | 100 ملف Markdown مُحوَّل |
| **UTM-6.12** | enforce_docs_coverage + pipeline AI | 100% تغطية + pipeline محتوى AI |
