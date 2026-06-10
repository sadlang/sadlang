---
title: مراجعة حالة نظام التوثيقات — ADR-006b
date: 2026-05-19
author: Amelia (dev agent) — bmad-agent-dev
scope: _bmad-output/systems/doc-ir/ + scripts/codegen/ + .github/workflows/docs-deploy.yml
status: review-final
related:
  - ADR-006b-spec.md
  - ADR-006b-epics.md
  - ADR-007_منصة_التوثيق_Docusaurus.md
  - ADR-008_علاقة_الموقع_بالمشروع.md
---

# مراجعة حالة نظام التوثيقات (ADR-006b)

> **الغرض:** فحص فعلي (تنفيذ + اختبارات) لمدى تطابق وثائق التخطيط في `_bmad-output/systems/doc-ir/` مع الكود
> الموجود في `scripts/codegen/` و `website/` و `.github/workflows/`، وتحديد نقاط الضعف وفرص التحسين.
> اتُّبع مبدأ Amelia: «التحقق التجريبي قبل الادعاء — كل تأكيد قابل للاستشهاد».

---

## 1. البنية العامة للوثائق

```
_bmad-output/systems/doc-ir/
├── ADR-006_توحيد_نظام_التوليد.md          (تاريخي — مُقسَّم إلى 006a + 006b)
├── ADR-006a_توحيد_codegen.md               (UTM-6.0→6.7 — مُعتمَد ومنفَّذ)
├── ADR-006b-spec.md                        (مواصفات pipeline التوثيق)
├── ADR-006b-epics.md                       (5 Epics — 17 Story)
├── ADR-006b_توليد_التوثيق_مؤجَّل.md         (مرجع)
├── ADR-007_منصة_التوثيق_Docusaurus.md       (قرار: VitePress فعلياً)
├── ADR-008_علاقة_الموقع_بالمشروع.md
├── session-002-round2-critique.md          (نقد الخبراء)
└── story-*.md                              (تفاصيل قصص فردية)
```

### الطبقات المعمارية (ADR-006b §3.1)

```
Data (YAML)  →  Document-IR (Python dataclasses)  →  Renderers (vitepress/lsp/repl/man)
```

---

## 2. التحقق التجريبي

| الفحص | الأمر | النتيجة |
|---|---|---|
| اختبارات pytest الكاملة | `pytest scripts/codegen/ --tb=no -q` | **413 passed, 2 skipped** ✅ |
| تغطية `description_ar` | `python scripts/codegen/check_docs_coverage.py --report` | **100.0% — 555/555** ✅ |
| صفحات stdlib المُولَّدة | `Get-ChildItem website/docs/docs/stdlib/*.md` | **20 ملفاً** ✅ |
| `doc_ir_builder.py` موجود ويعمل | `Test-Path scripts/codegen/doc_ir/doc_ir_builder.py` | ✅ |
| renderers موجودة | `Get-ChildItem scripts/codegen/renderers/render_*.py` | vitepress, lsp, repl, tutorial ✅ — **man مفقود** ❌ |
| workflows | `docs-deploy.yml` + `deploy-website.yml` | موجودان ويستدعيان `gen_docs.py` + `check_docs_coverage.py` ✅ |

---

## 3. حالة القصص — الموثَّقة مقابل الواقع

| Epic | Story | علامة في الوثيقة | حالة الكود الفعلية | فجوة |
|---|---|---|---|---|
| 1 | 1.1 DocIR builder | ❌ غير معلَّمة | ✅ منفَّذ (`doc_ir_builder.py` يعمل ضمن 413 اختبار) | **status drift** |
| 1 | 1.2 validator | ✅ DONE 2026-07-21 | ✅ مطابق | — |
| 1 | 1.3 versioning fields | ✅ DONE 2026-07-21 | ✅ مطابق | — |
| 1 | 1.4 Diátaxis (`doc_type`) | ❌ غير معلَّمة | ✅ منفَّذ (DocEntry.doc_type + V-03 + V-10 + 6 اختبارات) | ✅ أُغلقت 2025-11-21 |
| 2 | 2.0 / 2.1 / 2.2 / 2.3 | ✅ DONE | ✅ مطابق | — |
| 3 | 3.1 / 3.2 / 3.3 | ✅ DONE 2026-07-22 | ✅ مطابق | — |
| 3 | render_man.py | (مُدرج في `renderers/__init__.py`) | ✅ منفَّذ (370 سطر + 7 اختبارات + 34 صفحة troff) | ✅ أُغلقت 2025-11-21 |
| 4 | 4.1 migration_matrix | ❌ غير معلَّمة | جزئي (الملف موجود — لا تحقق آلي) | فجوة |
| 4 | 4.2 CI guard (لا Markdown يدوي) | ❌ غير معلَّمة | ❌ غير منفَّذة | فجوة حرجة |
| 4 | 4.3 priority migration | ❌ غير معلَّمة | جزئي (التغطية 100% بـ description قصير فقط) | فجوة |
| 5 | 5.1 enforce_docs_coverage | ❌ غير معلَّمة | الأداة جاهزة — `--enforce` **غير مفعَّل** في CI (يستخدم `--check`) | **status drift + فجوة قفل جودة** |
| 5 | 5.2 AI pipeline | ❌ غير معلَّمة | تمهيدي فقط (`ai_doc_prompt.md` + درس واحد) | فجوة |
| 5 | 5.3 full deployment | ❌ غير معلَّمة | منفَّذ بالعمل (workflows جاهزة) | status drift |

---

## 4. نقاط الضعف (مرتَّبة بحدّة الأثر)

### 4.1. حرجة

1. **حارس Markdown يدوي مفقود (Story 4.2)** — لا شيء يمنع مطوراً من إضافة ملف يدوي في `website/docs/docs/stdlib/` يكسر شرط «YAML هو SoT». هذا يضرب مباشرة المبرر المعماري لـADR-006b.
2. **`--enforce` غير مفعَّل** رغم تغطية 100% — الحارس موجود لكن لا يحرس. أي PR يحذف `description_ar` سيمر بصمت.

### 4.2. متوسطة

3. **Status drift منهجي**: 4 قصص منفَّذة فعلياً بدون علامة `✅ DONE` في `ADR-006b-epics.md`. يخالف مبدأ Amelia صراحةً (Critical Action #3).
4. ~~**توثيق كاذب**: `renderers/__init__.py` يُعلن `render_man.py` لكن الملف غير موجود.~~ ✅ أُغلق 2025-11-21 — الملف مُنفَّذ (370 سطر) + 7 اختبارات + 34 صفحة troff في `docs/generated/man/*.1`.
5. ~~**`doc_type` (Diátaxis) لم يُدرج في DocIR**~~ ✅ أُغلق 2025-11-21 — `DocEntry.doc_type` موجود + V-03 + V-10 + 6 اختبارات (T-12, T-21..T-25).
6. **مصفوفة الترحيل بدون اختبار آلي** — `data/docs/migration_matrix.yaml` موجودة لكن لا pytest يضمن أن `fate: generate` يقابله ملف فعلاً.
7. **"100% تغطية" مضلِّلة** — العتبة `MIN_DESCRIPTION_LENGTH = 10` فقط؛ لا فحص لجودة الجملة (CW-22 + BF-22).

### 4.3. منخفضة

8. **ازدواج workflows** (`docs-deploy.yml` و `deploy-website.yml`) بدون توثيق متى يُستخدم أيهما.
9. **لا اختبار E2E يبني VitePress** ضمن pytest — كسر `npm run build` يُكتشف في CI فقط.
10. **مسارات الإخراج في `ADR-006b-spec.md` متقادمة** — تذكر `docs/generated/` بينما الواقع `website/docs/docs/stdlib/` (بعد ADR-008).
11. **اسم ADR-007 مضلِّل** — اسم الملف `..._Docusaurus.md` لكن القرار صار VitePress.

---

## 5. التحسينات المقترحة (أولوية → جهد)

| # | التحسين | الأثر | الجهد |
|---|---|---|---|
| 1 | **تفعيل `--enforce` في `docs-deploy.yml`** + تحديث حالة Story 5.1 و 1.1 إلى DONE | عالٍ | دقائق |
| 2 | تنفيذ Story 4.2: حارس CI بسيط يفشل عند تعديل `website/docs/docs/stdlib/**` بدون تعديل YAML مقابل | عالٍ | ساعات |
| 3 | إزالة ذكر `render_man.py` من `renderers/__init__.py` أو تنفيذه فعلياً | متوسط | دقائق إلى يوم |
| 4 | إنجاز Story 1.4: إضافة `doc_type` في `DocEntry` + قاعدة V-10 في validator + 5 اختبارات | متوسط | يوم |
| 5 | رفع `MIN_DESCRIPTION_LENGTH` تدريجياً (10 → 30) + فحص بدائي (يبدأ بفعل، لا `TODO`) | متوسط | ساعات |
| 6 | ربط `migration_matrix.yaml` بـ `test_migration_guard.py` (فشل عند `fate: generate` بدون ملف) | متوسط | ساعات |
| 7 | إضافة اختبار E2E بطيء: `subprocess.run(["npm","run","build"], cwd="website")` خلف `pytest --slow` | متوسط | ساعات |
| 8 | توحيد workflows أو إضافة README في `.github/workflows/` يشرح التقسيم | منخفض | دقائق |
| 9 | تحديث `ADR-006b-spec.md §3.1` لتعكس `website/docs/docs/stdlib/` بدل `docs/generated/` | منخفض | دقائق |
| 10 | إعادة تسمية `ADR-007_منصة_التوثيق_Docusaurus.md` → `ADR-007_منصة_التوثيق_VitePress.md` | منخفض | دقائق |
| 11 | نقل `_bmad-output/systems/doc-ir/` إلى `docs/توثيقات_تطوير_اللغة/توثيقات_الخطط/` (CW-04 تناسق بنيوي) | منخفض | متوسط |

---

## 6. الإجراء المتَّفق عليه — هذه الجلسة

اخْتِير العنصر #1 من جدول التحسينات:
- تفعيل `--enforce` في `.github/workflows/docs-deploy.yml`.
- وضع علامة `✅ DONE` على Story 1.1 و Story 5.1 في `ADR-006b-epics.md` مع تاريخ 2026-05-19.

التحقق:
- `python scripts/codegen/check_docs_coverage.py --enforce` يُرجع exit 0 محلياً.
- `pytest scripts/codegen/` لا يتراجع.

---

## 7. الخلاصة

البنية التقنية لـADR-006b صلبة ومنفَّذة فعلياً (Epics 1–3 جوهرياً كاملة، 413 اختبار يمرّ،
20 صفحة stdlib مُولَّدة، workflow للنشر يعمل). الفجوة الحقيقية في:

1. **انضباط الحالة** — القصص الفعلية متقدمة على الوثيقة.
2. **قفل الجودة** — Epic 4 منفَّذ جزئياً وغير مفعَّل في CI.
3. **خلق محتوى تعليمي** — Epic 5 تمهيدي فقط.

تنفيذ التحسينات 1–4 يُغلق الفجوات الحرجة ويضع النظام في حالة «إنتاج كامل».
