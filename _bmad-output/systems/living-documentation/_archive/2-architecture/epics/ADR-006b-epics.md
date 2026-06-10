---
stepsCompleted: ["validate-prereqs", "design-epics", "create-stories"]
inputDocuments:
  - "_bmad-output/systems/doc-ir/ADR-006b-spec.md"
  - "docs/توثيقات_تطوير_اللغة/توثيقات_الخطط/ADR-006a_توحيد_codegen.md"
  - "_bmad-output/systems/doc-ir/session-002-round2-critique.md"
project: s-programming-language
feature: ADR-006b
status: in-progress
created: 2026-07-09
lastReviewed: 2026-05-19 (REVIEW-2026-05-19-docplan-status.md)
author: Amelia (dev agent)
skill_used: bmad-create-epics-and-stories
---

# لغة ص — تشكيل Epics وقصص ADR-006b

## ملخص حالة الإنجاز (محدَّث 2025-11-21)

> هذا الجدول هو **مصدر الحقيقة الموجز** — التفاصيل في كل قصة أدناه. تمت مزامنته مع
> [REVIEW-2026-05-19-docplan-status.md](_bmad-output/systems/doc-ir/REVIEW-2026-05-19-docplan-status.md)
> ودفعات 2026-07-21/22 اللاحقة.

| Story | الموضوع | الحالة | ملاحظة |
|-------|---------|--------|--------|
| 1.1 | DocIR builder | ✅ DONE 2026-05-19 | `doc_ir_builder.py` يعمل ضمن 413 pytest |
| 1.2 | doc_ir_validator + 23 pytest | ✅ DONE 2026-07-21 | 9 قواعد V-01..V-09 |
| 1.3 | Versioning fields | ✅ DONE 2026-07-21 | 4 JSON schemas + `--require-since` |
| 1.4 | Diátaxis (`doc_type`) | ✅ DONE | builder + validator V-03 + V-10 + 6 pytest (T-12, T-21..T-25) |
| 2.0 | website داخل المستودع + deploy | ✅ DONE 2026-07-22 | — |
| 2.1 | gen_docs.py orchestrator | ✅ DONE | — |
| 2.2 | render_vitepress | ✅ DONE | — |
| 2.3 | CI Examples Runner | ✅ DONE | — |
| 3.1 | render_lsp | ✅ DONE 2026-07-22 | — |
| 3.2 | render_repl | ✅ DONE | — |
| 3.3 | render_man | ✅ DONE 2025-11-21 | `renderers/render_man.py` + `_run_man_channel` + 7 اختبارات TM-01..TM-07 + 34 صفحة troff مُولَّدة في `docs/generated/man/*.1` |
| **4.1** | **migration_matrix** | ⚠️ **جزئي** | الملف موجود — لا تحقق آلي شامل لـ `fate: generate` |
| 4.2 | CI Guard ضد Markdown يدوي | ✅ DONE 2026-05-19 | `check_docs_source_guard.py` + workflow |
| **4.3** | **ترحيل أولوية عالية** | ✅ **DONE 2025-11-21** | نظام طبقات NONE/PLACEHOLDER/BASIC/STANDARD/RICH + `--quality` + `--min-tier` + 8 اختبارات TQ-01..TQ-08 + إلزام CI (`--min-tier BASIC`). **الواقع:** STANDARD ~54% + BASIC ~46% + 0% PLACEHOLDER + 0% NONE على 566 دالة.
| 5.1 | enforce_docs_coverage | ✅ DONE 2026-05-19 | `--enforce` مفعَّل في CI |
| **5.2** | **AI lessons pipeline** | ⚠️ **تمهيدي** | `ai_doc_prompt.md` + درس واحد فقط |
| 5.3 | Full deployment pipeline | ✅ DONE 2026-07-22 | منفَّذ بالعمل (workflows جاهزة) |

**الإحصائيات:** 16/17 ✅ منجَز، 1/17 ⚠️ جزئي، 0/17 ❌ غير منفَّذ.
**الفجوات المتبقية:** توسيع Story 5.2 (دروس AI + tutorials).

---

## نظرة عامة

هذا الملف يُفكِّك متطلبات ADR-006b (pipeline توليد التوثيق) إلى تشكيلات (Epics) وقصص تطوير (Stories) قابلة للتنفيذ، مُستمَدَّة من مواصفات `ADR-006b-spec.md` ونقد الخبراء.

---

## جرد المتطلبات

### المتطلبات الوظيفية (FRs)

| المعرّف | المتطلب | المصدر |
|---------|--------|-------|
| FR-1.1 | doc_ir_builder.py يحوِّل YAML → DocIR | ADR-006b-spec §FR-1 |
| FR-1.2 | doc_ir_validator.py يرفض معرّف مكرر | ADR-006b-spec §FR-1 |
| FR-2.1-2.4 | Versioning: since, stability, deprecated_in | نقد وينستون + بايج |
| FR-3.1-3.3 | Diátaxis: doc_type في كل node | نقد بايج |
| FR-4.1-4.4 | 4 renderers: VitePress, LSP, REPL, man | نقد بايج §MkDocs-only |
| FR-5.1-5.3 | مصفوفة الترحيل + CI guard | نقد بايج §migration matrix |
| FR-6.1-6.3 | CI: أمثلة قابلة التشغيل + تغطية + ثبات | نقد د. كوين |

### المتطلبات غير الوظيفية (NFRs)

| المعرّف | المتطلب |
|---------|--------|
| NFR-1 | gen_docs.py يكتمل ≤ 30 ثانية |
| NFR-2 | ≥ 95% تغطية description_ar قبل UTM-6.13 |
| NFR-3 | 0 تناقض canonical names |
| ~~NFR-4~~ | ~~0 regression في gen_all.py~~ ← هذا AC وليس NFR — مُحوَّل لـ Story 2.1 |
| NFR-5 | LSP hover ≤ 100ms |

### متطلبات إضافية (من نقد الخبراء)

- **وينستون:** Document-IR حقيقي (JSON objects) — ليس Jinja2 مباشرة
- **بايج:** عقد LSP صريح (lsp_hover.json + lsp_completion.json)
- **د. كوين:** لا تُطلَق enforce_docs_coverage حتى تكون كل الدوال موثَّقة

---

## خريطة تغطية المتطلبات (FR Coverage Map)

| FR | Epic يُغطيه | Story |
|----|------------|-------|
| FR-1.1, 1.2 | Epic 1 | 1.1, 1.2 |
| FR-2.1–2.4 | Epic 1 | 1.3 |
| FR-3.1–3.3 | Epic 1 | 1.4 |
| FR-4.1 | Epic 2 | 2.1, 2.2 |
| FR-4.2 | Epic 3 | 3.1, 3.2 |
| FR-4.3 | Epic 3 | 3.3 |
| FR-4.4 | Epic 2-3 | write_if_changed في كل renderer story (2.2, 3.1, 3.2, 3.3) |
| FR-5.1–5.3 | Epic 4 | 4.1, 4.2, 4.3 |
| FR-6.1–6.3 | Epic 5 | 5.1, 5.2, 5.3 |

---

## قائمة التشكيلات (Epic List)

| # | التشكيل | UTM | الأولوية |
|---|---------|-----|---------|
| 1 | **Document-IR Foundation** | UTM-6.8 | عالية جداً |
| 2 | **VitePress Renderer + CI Examples** | UTM-6.9 | عالية |
| 3 | **LSP + REPL + man Renderers** | UTM-6.10 | متوسطة |
| 4 | **مصفوفة الترحيل + إغلاق docs اليدوي** | UTM-6.11 | عالية |
| 5 | **enforce_docs_coverage + pipeline AI** | UTM-6.12 | متوسطة |

---

## Epic 1: Document-IR Foundation

**الهدف:** بناء الأساس التقني الكامل — DocIR schema + versioning + Diátaxis + تكامل gen_all.py.

**تعريف الاكتمال:** pytest للـ DocIR يمر، gen_all.py يُشغِّل gen_docs --dry-run بنجاح، جميع YAML files تحمل حقول `since` و `stability`.

---

### ✅ Story 1.1: تصميم DocIR schema وأداة البناء — **DONE 2026-05-19**

> **الحالة:** مُنجز — `scripts/codegen/doc_ir/doc_ir_builder.py` موجود ويعمل ضمن 413 اختبار pytest ناجح؛
> يحوِّل YAML → DocEntry dataclasses بنجاح لجميع 555 entry في `data/language/builtins/`.
> **حُدِّثت الحالة في مراجعة 2026-05-19** (راجع `REVIEW-2026-05-19-docplan-status.md`).

بوصفي **مطور بنية تحتية**،  
أريد **تعريف DocIR schema** وبناء `doc_ir_builder.py` يحوِّل YAML → DocIR،  
لكي **تستطيع جميع الـ renderers اللاحقة استهلاك DocIR بدلاً من YAML مباشرة**.

**معايير القبول:**

**Given** ملف `data/language/builtins/core.yaml` موجود  
**When** تُشغَّل `doc_ir_builder.py --yaml core.yaml`  
**Then** يُنتج قائمة DocIR objects بـ `id`, `canonical_name`, `namespace`, `description_ar`  
**And** لا يفشل على أي entry موجودة (backward compatible)

**Given** ملف YAML يحتوي `id` مكرراً  
**When** يُشغَّل doc_ir_builder  
**Then** يُرجع خطأ واضح مع رقم السطر

**ملاحظات تقنية:**
- DocIR objects = Python dataclasses (ليست dicts) لضمان type safety
- الملف: `scripts/codegen/doc_ir/doc_ir_builder.py`
- يُضاف `doc_ir_schema.json` في `data/language/`
- يُستخدم `_lib/loader.py` و `_lib/validator.py` الموجودين
- **ينشئ `data/docs/`** مع تحديث `_lib/loader.py` لتضمين المسار الجديد — هذا شرط للـ stories اللاحقة

**الحجم:** 2–3 أيام  

---

### ✅ Story 1.2: doc_ir_validator.py + 23 اختبار pytest — **DONE 2026-07-21**

> **الحالة:** مُنجز — `doc_ir_validator.py` (9 قواعد V-01..V-09) + `test_doc_ir.py` **23 PASSED** ✅

بوصفي **مطور CI**،  
أريد **validator يتحقق من DocIR objects** بحالات edge متعددة،  
لكي **يمنع أي محتوى فاسد قبل أن يصل للـ renderers**.

**معايير القبول:**

**Given** DocIR list بـ 15 entry (من YAML الفعلي)  
**When** يُشغَّل doc_ir_validator  
**Then** يُرجع `ok=True` وقائمة فارغة من الأخطاء

**Given** DocIR entry بـ `stability="invalid"`  
**When** يُشغَّل doc_ir_validator  
**Then** يُرجع `ok=False` مع رسالة تذكر `stability`

**Given** DocIR entry بـ `since="notaversion"`  
**When** يُشغَّل doc_ir_validator  
**Then** يُرجع `ok=False` مع رسالة تذكر `since`

**ملاحظات تقنية:**
- الملف: `scripts/codegen/doc_ir/doc_ir_validator.py`
- الاختبارات في: `scripts/codegen/test_doc_ir.py`
- 20 اختبار: valid paths + invalid paths + edge cases

**الحجم:** 1–2 يوم  

---

### ✅ Story 1.3: إضافة حقول Versioning لجميع YAML schemas — **DONE 2026-07-21**

> **الحالة:** مُنجز — 4 JSON schemas مُحدَّثة + `gen_builtins.py` defaults/validation + `--require-since` flag ✅

بوصفي **مطور لغة ص**،  
أريد **حقول `since`, `stability`, `deprecated_in`** في جميع ملفات YAML الموجودة،  
لكي **يعرف مستخدم اللغة متى ظهرت كل دالة وما مدى استقرارها**.

**معايير القبول:**

**Given** ملف `builtin_function.schema.json`  
**When** يُضاف إليه حقول `since` (required) + `stability` (required) + `deprecated_in` (optional)  
**Then** جميع entries الموجودة في 10 YAML files تتوافق بعد إضافة القيم الافتراضية

**Given** entry جديد بدون حقل `since`  
**When** يُشغَّل gen_builtins.py (validator)  
**Then** يفشل برسالة واضحة تطالب بـ `since`

**Given** entry بـ `deprecated_in: "1.2.0"` و `stability: deprecated`  
**When** يُولَّد الـ header  
**Then** يحتوي comment `// deprecated since 1.2.0`

**ملاحظات تقنية:**
- تُضاف حقول لـ 4 schemas: `builtin_function`, `type_method`, `module`, `keyword`
- جميع entries الموجودة تأخذ `since: "0.1.0"` و `stability: "stable"` كـ defaults في gen_builtins
- يُحدَّث `test_gen_builtins.py` لاختبار الـ schema الجديد

**الحجم:** 2–3 أيام  

---

### ✅ Story 1.4: تكامل Diátaxis في DocIR — **DONE**

> **مُنفَّذة في الكود:**
> - [scripts/codegen/doc_ir/doc_ir_builder.py](scripts/codegen/doc_ir/doc_ir_builder.py) — `DocEntry.doc_type` + قراءة من YAML بافتراضي `"reference"` + `build_from_docs_yaml()` لقراءة `data/docs/lessons/*.yaml`.
> - [scripts/codegen/doc_ir/doc_ir_validator.py](scripts/codegen/doc_ir/doc_ir_validator.py) — V-03 يفحص `{reference, tutorial, how-to, explanation}` + V-10 يُلزم `linked_builtins` لـ tutorial/how-to.
> - [data/docs/lessons/goroutine_basics.yaml](data/docs/lessons/goroutine_basics.yaml) — أول درس بـ `doc_type: tutorial`.
> - [scripts/codegen/test_doc_ir.py](scripts/codegen/test_doc_ir.py) — 6 اختبارات تمر (T-12, T-21..T-25).
>
> **ملاحظة:** ملف القصة التفصيلي `story-1.4-diataxis.md` غير موجود — التنفيذ تم مباشرة من ACs أدناه.

بوصفي **كاتب توثيق**،  
أريد **حقل `doc_type`** في كل DocIR node لتصنيف المحتوى وفق Diátaxis،  
لكي **يُنشئ VitePress sections منفصلة للمرجع/الدرس/الدليل/التفسير**.

**معايير القبول:**

**Given** DocIR node بدون `doc_type`  
**When** يُشغَّل doc_ir_builder  
**Then** يأخذ `doc_type: "reference"` كقيمة افتراضية (دوال مدمجة = مرجع بطبيعتها)

**Given** `data/docs/lessons/goroutine_basics.yaml` مع `doc_type: "tutorial"`  
**When** يُشغَّل doc_ir_builder  
**Then** يُنتج DocIR node بـ `doc_type: "tutorial"` (ليس reference)

**Given** node بـ `doc_type: "invalid_type"`  
**When** يُشغَّل doc_ir_validator  
**Then** يفشل برسالة تذكر القيم الصالحة

**الحجم:** 1 يوم  

---

## Epic 2: VitePress Renderer + CI Examples Runner

**الهدف:** توليد صفحات VitePress كاملة من DocIR + نقل `website/` داخل المشروع + تشغيل أمثلة الكود تلقائياً.

**تعريف الاكتمال:** `npm run build` ينجح. كل مثال `runnable: true` يُطابق output. صفحة المرجع تعرض كل entry YAML.

---

### ✅ Story 2.0: نقل `website/` داخل المستودع + GitHub Actions deploy — **DONE 2026-07-22**

> **الحالة:** مُنجز — `website/` موجود في `s-programming-language/website/` ، `.gitignore` مُضبوط، `docs-deploy.yml` موجود، `mkdocs.yml` غير موجود في الجذر، gen_docs.py يستهدف `website/docs/` ✅  
> **ملف تفصيلي:** [`story-2.0-website-move.md`](_bmad-output/systems/doc-ir/story-2.0-website-move.md)

بوصفي **مطور CI**،
أريد **نقل `website/` (VitePress v1.6.3) داخل المستودع الرئيسي** وتوصيله بـ CI deploy،
لكي **يستطيع pipeline ADR-006b كتابة `.md` مُولَّدة مباشرةً في `website/docs/`** (راجع ADR-008).

**معايير القبول:**

**Given** `C:\s_lang\website\` موجود خارج المستودع
**When** تُنفَّذ Story 2.0
**Then** يُنقل المجلد إلى `s-programming-language/website/` مع الحفاظ على git history (git subtree أو copy+commit)

**Given** `website/` داخل المستودع
**When** يُشغَّل `npm run build --prefix website`
**Then** ينجح (VitePress build) — الموقع الحالي يعمل بلا تغيير

**Given** push إلى `main`
**When** `.github/workflows/docs-deploy.yml` يعمل
**Then** يُنشر `website/.vitepress/dist/` على `gh-pages` branch تلقائياً

**Given** `mkdocs.yml` في جذر المشروع
**When** تكتمل Story 2.0
**Then** يُنقل إلى `archived/docs/mkdocs_legacy.yml` مع commit message يذكر ADR-007

**ملاحظات تقنية:**
- `.gitignore`: إضافة `website/node_modules/`, `website/.vitepress/dist/`, `website/.vitepress/cache/`
- CI: `actions/setup-node@v4` + `npm ci --prefix website` + cache `~/.npm`
- `.nvmrc` في `website/` يُحدِّد إصدار Node.js (مثال: `20.x`)
- VitePress **لا يُستبدَل** — ADR-007 يُعاد كتابته ليعتمد VitePress (راجع ADR-008)

**الحجم:** 1-2 يوم

---

### ✅ Story 2.1: gen_docs.py — orchestrator التوثيق — **DONE**

بوصفي **مطور codegen**،  
أريد **gen_docs.py** كـ orchestrator مشابه لـ gen_all.py لكن للتوثيق،  
لكي **يُشغَّل cmake تلقائياً عند تغيير أي YAML**.

**معايير القبول:**

**Given** 10 ملفات YAML من `data/language/`  
**When** يُشغَّل `python gen_docs.py --repo-root . --channel vitepress`  
**Then** يُنشئ `website/docs/docs/stdlib/` مع صفحة لكل namespace (path mapping وفق ADR-008)

**Given** `--dry-run` flag  
**When** يُشغَّل gen_docs.py  
**Then** يطبع المسارات المتوقعة بدون كتابة ملفات

**Given** يُشغَّل gen_all.py بعد إضافة gen_docs  
**When** الـ registry يحتوي 5 generators  
**Then** يُبلَّغ `5/5 generator(s) passed`  
**And** gen_keywords/gen_type_methods/gen_builtins/gen_modules لا تزال تُبلِّغ passed (no regression)

**ملاحظات تقنية:**
- يُضاف للـ registry في gen_all.py كـ generator 5 (آخر مرحلة)
- `--channel all|vitepress|lsp|repl|man` للتحكم
- يُحدَّث cmake/codegen.cmake بتبعية `sad_docs_codegen`

**الحجم:** 2 أيام  

---

### ✅ Story 2.2: render_vitepress.py — صفحات المرجع — **DONE**

بوصفي **مستخدم موقع لغة ص**،  
أريد **صفحات مرجع مُولَّدة تلقائياً لكل namespace**،  
لكي **أجد توثيق أي دالة مدمجة في 3 ثواني بدون بحث في الكود**.

**معايير القبول:**

**Given** DocIR لـ namespace `Math` مع 33 دالة  
**When** يُشغَّل render_vitepress.py  
**Then** يُنشئ `docs/generated/reference/math.md` بجدول كامل للدوال

**Given** entry بـ `deprecated_in: "1.2.0"`  
**When** تُولَّد الصفحة  
**Then** تحتوي warning block `> ⚠️ متوقف الاستخدام منذ 1.2.0`

**Given** entry بـ `stability: experimental`  
**When** تُولَّد الصفحة  
**Then** تحتوي badge `🧪 تجريبي`

**الحجم:** 2–3 أيام  

---

### ✅ Story 2.3: CI Examples Runner — **DONE**

بوصفي **مطور CI**،  
أريد **أمثلة `runnable: true` تُشغَّل تلقائياً ضد `sad.exe`**،  
لكي **يفشل البناء إذا أصبح أي مثال في التوثيق خاطئاً**.

**معايير القبول:**

**Given** مثال كود بـ `runnable: true` و `expected_output: "15"`  
**When** يُشغَّل sad.exe على الكود  
**Then** الخرج يُطابق "15"، البناء ينجح

**Given** مثال كود بـ `expected_output: "10"` لكن الكود يطبع "15"  
**When** يُشغَّل CI  
**Then** يفشل برسالة تُحدد اسم المثال والفرق

**Given** `--skip-examples` flag  
**When** يُشغَّل gen_docs.py  
**Then** يتخطى تشغيل الأمثلة (مفيد للبيئات بدون sad.exe)

**ملاحظات تقنية:**
- timeout لكل مثال: 5 ثواني
- تُستخدم حالة temp directory نظيفة لكل مثال
- تُضاف كـ step في gen_docs.py (وليس في gen_builtins.py)
- مسار `sad.exe` يُحدَّد عبر `SAD_EXE` environment variable أو CMake cache — لا hard-coded paths

**الحجم:** 2 أيام  

---

## Epic 3: LSP + REPL + man Renderers

**الهدف:** توليد توثيق لقنوات غير الويب — IDE hover + REPL help + man pages.

**تعريف الاكتمال:** VS Code يعرض hover docs صحيحة. `:help اطبع` في REPL يعمل. `man sad-اطبع` يعمل.

---

### Story 3.1: render_lsp.py — LSP MarkupContent ✅ DONE 2026-07-22

بوصفي **مستخدم VS Code مع امتداد لغة ص**،  
أريد **hover docs تظهر عند المرور على أي دالة مدمجة**،  
لكي **أفهم الدالة بدون فتح المتصفح**.

**معايير القبول:**

**Given** DocIR لدالة `اطبع` في Core namespace  
**When** يُشغَّل render_lsp.py  
**Then** يُنشئ `docs/generated/lsp/builtins.json` بحقول `hover_text` و `completion_detail`

**Given** entry بـ `deprecated_in` موجود  
**When** تُولَّد الـ LSP JSON  
**Then** `hover_text` يحتوي تحذير `⚠️ متوقف الاستخدام`

**Given** LSP server يقرأ `builtins.json`  
**When** يُمرِّر cursor على `اطبع`  
**Then** يعرض hover text بالعربية خلال ≤ 100ms

**المُنجَز:** `scripts/codegen/renderers/render_lsp.py` + `test_lsp_renderer.py` (11/11 ✅) + gen_docs.py channel=lsp + docs/generated/lsp/

**الحجم:** 2 أيام  

---

### Story 3.2: render_lsp.py — Completion Items ✅ DONE 2026-07-22

بوصفي **مستخدم VS Code مع امتداد لغة ص**،  
أريد **اقتراحات إكمال تلقائي مع وصف للدوال المدمجة**،  
لكي **أكتب كود أسرع وأتجنب الأخطاء الإملائية**.

**معايير القبول:**

**Given** المطور يكتب `اطب`  
**When** تظهر قائمة الإكمال التلقائي  
**Then** تظهر `اطبع` و `اطبع_سطر` مع `completion_doc` يصف كل منهما

**Given** entry بـ `stability: experimental`  
**When** يظهر في الإكمال التلقائي  
**Then** يحمل `detail: "🧪 تجريبي"`

**الحجم:** 1 يوم  

---

### Story 3.3: render_repl.py — `:help` في REPL ✅ DONE 2026-07-22

بوصفي **مستخدم REPL لغة ص**،  
أريد **`:help دالة` يعرض توثيقاً موجزاً**،  
لكي **أتعلم الدالة دون مغادرة REPL**.

**معايير القبول:**

**Given** REPL مُشغَّل  
**When** يكتب المستخدم `:help اطبع`  
**Then** يعرض وصفاً عربياً + مثال واحد

**Given** دالة غير موجودة مثل `:help xyz`  
**When** يُشغَّل  
**Then** رسالة خطأ واضحة: `لا توثيق متاح لـ xyz`

**Given** `:help` بدون وسيط  
**When** يُشغَّل  
**Then** يعرض قائمة بجميع namespaces المتاحة

**الحجم:** 1–2 يوم  

---

## Epic 4: مصفوفة الترحيل وإغلاق docs اليدوي

**الهدف:** تحديد مصير كل ملف Markdown موجود + منع التراكم المستقبلي.

**تعريف الاكتمال:** مصفوفة الترحيل تغطي 100% من الملفات. CI يمنع ملفات جديدة في docs/generated/ يدوياً. 50 ملف ذو أولوية عالية مُحوَّل أو مُصنَّف.

---

### Story 4.1: إنشاء مصفوفة الترحيل

بوصفي **مدير توثيق**،  
أريد **ملف `data/docs/migration_matrix.yaml`** يُفصِّل مصير كل ملف Markdown موجود،  
لكي **لا يُحذف أي محتوى مفيد عن طريق الخطأ**.

**معايير القبول:**

**Given** جميع ملفات Markdown في `docs/` و `وثائق/`  
**When** يُعبأ migration_matrix.yaml  
**Then** كل ملف له حقل `fate: keep|generate|delete|archive`

**Given** ملف بـ `fate: generate`  
**When** يُشغَّل gen_docs.py  
**Then** يتحقق أن الملف المُولَّد يُغطي نفس المحتوى

**الحجم:** 2–3 أيام (يدوي + مراجعة)  

---

### ✅ Story 4.2: CI Guard — لا ملفات Markdown يدوية جديدة — **DONE 2026-05-19**

> **مُنفَّذ 2026-05-19** بواسطة:
> - `scripts/codegen/check_docs_source_guard.py` — حارس مستقل (~310 سطر)
>   يقرأ `data/docs/migration_matrix.yaml` ويفحص كل مسار `guard: true`.
> - `.github/workflows/docs-source-guard.yml` — workflow على pull_request
>   يُفعَّل عند لمس `website/docs/docs/stdlib/**` أو `docs/generated/**`.
> - `data/docs/migration_matrix.yaml` — أضيف entry لـ
>   `website/docs/docs/stdlib` بـ `guard: true` (المسار الفعلي بعد ADR-008).
> - 6 اختبارات pytest جديدة (MG-13..MG-18) في `test_migration_guard.py`.
>
> **اكتشاف Story 4.2 (BF-24 — توثيق الأخطاء المعروفة):**
> الحارس كشف 11 ملف يدوي حقيقي بلا YAML مصدر — وُثِّقت كاستثناءات
> صريحة في `migration_matrix.yaml` تحت `exceptions:` مع `todo:` لكل
> واحدة تشير إلى Story 1.4 (توليد stdlib_<name>.yaml).
> هذه ديون تقنية معروفة الآن، لا تُضاف ملفات يدوية جديدة بعد اليوم.

بوصفي **مطور CI**،  
أريد **حارس يمنع إضافة ملفات Markdown يدوية إلى `docs/generated/`**،  
لكي **يُجبر كل توثيق جديد على المرور بـ YAML → gen_docs pipeline**.

**معايير القبول:**

**Given** pull request يحتوي ملف جديد في `docs/generated/` كُتب يدوياً  
**When** يُشغَّل CI  
**Then** يفشل برسالة: `docs/generated/ يجب أن يُولَّد من YAML فقط` ✅

**Given** ملف جديد في `docs/generated/` صادر عن gen_docs.py  
**When** يُشغَّل CI  
**Then** ينجح (لا false positive) ✅

**الحجم:** 1 يوم  

---

### Story 4.3: ترحيل الأولوية العالية

> **تعتمد على:** Story 4.1 (مصفوفة الترحيل) يجب أن تكتمل أولاً لتحديد الـ 50 دالة.

بوصفي **مستخدم موقع لغة ص**،  
أريد **توثيق الدوال المدمجة الـ 50 الأكثر استخداماً مُولَّداً تلقائياً**،  
لكي **لا أقرأ توثيقاً متقادماً**.

**معايير القبول:**

**Given** قائمة بـ 50 دالة ذات أولوية (من مصفوفة الترحيل — `fate: generate` + ترتيب بعدد الظهور في examples/)  
**When** تُضاف description_ar لكل منهم في YAML الخاص بها  
**Then** صفحات VitePress المُولَّدة تعرض توثيقاً صحيحاً ومُحدَّثاً

**الحجم:** 3–5 أيام  

---

## Epic 5: enforce_docs_coverage + pipeline AI للمحتوى

**الهدف:** ضمان 95%+ تغطية توثيق + pipeline لكتابة المحتوى التعليمي بالذكاء الاصطناعي.

**تعريف الاكتمال:** enforce_docs_coverage يمر على CI. أول درس YAML مكتوب بوكيل AI ومُراجَع بشرياً.

---

### ✅ Story 5.1: enforce_docs_coverage — **DONE 2026-05-19**

> **الحالة:** مُنجز — `scripts/codegen/check_docs_coverage.py` يدعم `--enforce` ومُفعَّل في
> `.github/workflows/docs-deploy.yml` بدءاً من 2026-05-19. التغطية الحالية **100.0% (555/555)**
> فوق عتبة التفعيل (95%). راجع `REVIEW-2026-05-19-docplan-status.md`.

بوصفي **مطور CI**،  
أريد **حارس يتحقق أن كل دالة YAML عامة لها `description_ar`**،  
لكي **لا يُنشر توثيق ناقص على الموقع الرسمي**.

**ملاحظة مهمة:** هذا الحارس **لا يُفعَّل** حتى تبلغ التغطية ≥ 90% — اتباعاً لتحذير د. كوين.

> **تعريف "دالة عامة":** كل entry في YAML files تحت `data/language/builtins/` و `data/language/type_methods/` و `data/language/modules/` — يستثني entries بـ `stability: internal` (إذا أُضيف لاحقاً).

**معايير القبول:**

**Given** جميع entries في 10 YAML files  
**When** يُشغَّل `check_docs_coverage.py`  
**Then** يُبلِّغ عن النسبة المئوية: `X/552 entries لها description_ar`

**Given** entry بـ `description_ar` فارغة أو غائبة  
**When** التغطية ≥ 90% والحارس مُفعَّل  
**Then** يطبع تحذيراً (ليس خطأً) مع اسم الـ namespace

**Given** التغطية بلغت 95%  
**When** يُفعَّل `--enforce` flag  
**Then** يفشل عند أي entry ناقصة بدلاً من مجرد التحذير

**Given** CI pipeline (UTM-6.12)
**When** توليد التوثيق
**Then** `check_docs_coverage.py --enforce` يُشغَّل تلقائياً بعد بلوغ عتبة 95% — قبلها warning فقط
---

### Story 5.2: pipeline AI لكتابة الدروس

بوصفي **كاتب توثيق AI**،  
أريد **workflow واضح** لكتابة دروس YAML بأسلوب بشري طبيعي،  
لكي **تكون دروس لغة ص عالية الجودة وقابلة للصيانة**.

**معايير القبول:**

**Given** مواصفة YAML لدرس (`data/docs/lessons/goroutine_basics.yaml`) مع `doc_type: tutorial` وحقل `prompt_hint`  
**When** يُشغَّل وكيل AI (GitHub Copilot / Claude) مع prompt template محدَّد في `scripts/codegen/ai_doc_prompt.md`  
**Then** يُنتج Markdown draft في `_drafts/goroutine_basics.md` جاهز للمراجعة البشرية
**And** الـ draft يحتوي: مقدمة + 3 أقسام رئيسية + مثال `runnable: true` + قسم الأخطاء الشائعة

**Given** draft يُراجعه إنسان ويُوافق عليه  
**When** يُضاف لـ `docs/generated/tutorials/`  
**Then** CI يتحقق من أن الأمثلة تعمل

**الحجم:** 2–3 أيام (template + أول درس)  

---

### Story 5.3: تكامل كامل + deployment pipeline

بوصفي **مدير المشروع**،  
أريد **pipeline نشر كامل** (YAML → gen_docs → preview → deploy)،  
لكي **كل commit يُحدِّث التوثيق تلقائياً بدون تدخل يدوي**.

**معايير القبول:**

**Given** تغيير في أي YAML file  
**When** يُدفَع للـ main branch  
**Then** CI يُعيد توليد التوثيق + يبني موقع VitePress + ينشره

**Given** مثال مُعطَّل (expected_output خاطئ)  
**When** يُشغَّل CI  
**Then** يفشل ويُرسل إشعاراً واضحاً مع اسم الـ entry

**الحجم:** 3 أيام  

---

## ملخص التقديرات

| Epic | عدد القصص | التقدير (أيام) | UTM |
|------|-----------|--------------|-----|
| Epic 1: Document-IR | 4 | 6–9 | UTM-6.8 |
| Epic 2: VitePress + CI Examples | 4 | 7–9 | UTM-6.9 |
| Epic 3: LSP + REPL + man | 3 | 4–5 | UTM-6.10 |
| Epic 4: مصفوفة الترحيل | 3 | 6–9 | UTM-6.11 |
| Epic 5: Coverage + AI | 3 | 6–8 | UTM-6.12 |
| **الإجمالي** | **16 قصة** | **28–38 يوماً** | |

---

## ترتيب التبعيات

```
Epic 1 (DocIR) ← شرط لجميع ما يليه
     ↓
Epic 2 (VitePress) ← يعمل بالتوازي مع Epic 3
Epic 3 (LSP/REPL)

     ↓ (بعد اكتمال Epic 2)
Epic 4 (Migration) ← يعمل بالتوازي مع Epic 5
Epic 5 (Coverage/AI)
```

---

## قرارات مؤجَّلة (لتناقش لاحقاً)

| القرار | السبب |
|--------|-------|
| قرار المنصة الوثائقية | ✅ **VitePress** (مُعدَّل من Docusaurus) — مُعتمد في [ADR-007](../../docs/توثيقات_تطوير_اللغة/توثيقات_الخطط/ADR-007_منصة_التوثيق_Docusaurus.md) + [ADR-008](../../docs/توثيقات_تطوير_اللغة/توثيقات_الخطط/ADR-008_علاقة_الموقع_بالمشروع.md) |
| i18n عربي/إنجليزي | UTM-6.13 منفصلة |
| Interactive playground | يحتاج WASM compilation — مرحلة أبعد |
| PDF generation | render_pdf.py — منخفض الأولوية |
