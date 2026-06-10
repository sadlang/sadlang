# 📋 Epics v2 — نظام التوثيق الحي للغة ص

> **النسخة:** 2.1 (مُحدَّث بعد UX Review من Sally)
> **يستبدل:** `epics-docs-system.md` (v1)
> **التاريخ:** 2026-05-09
> **الإجمالي:** 5 Epics × 19 Stories — **10.75 يوم عمل** (بدلاً من 11)

---

## Epic 0 — Sprint 0: Bootstrap (0.25 يوم)

> **التغيير عن v1:** website موجود + tests موجودة + docs_emitter موجود → 0.5 يوم → **0.25 يوم**

### Story 0.0 — Verify Existing Infrastructure

**هدف:** التحقق من أن website + tests + docs_emitter يعملون ويوثيق ما هو موجود.

**ACs:**
- ✅ AC-1: `cd C:\s_lang\website && npm install && npm run build` ينجح
- ✅ AC-2: `cd C:\s_lang\website && npm run dev` يفتح localhost ويعرض الموقع
- ✅ AC-3: `python tests/doc_gen_dual_execution/runner.py --skip-pdf` يمر 33/33
- ✅ AC-4: `cmake --build build --target sad --config Release` ينجح
- ✅ AC-5: ملف `_bmad-output/implementation-artifacts/infra-verified.md` يُنشأ يوثّق الإصدارات
- ✅ AC-6: GitHub secrets موثّقة في `secrets-checklist.md`

**التقدير:** 0.25 يوم (2 ساعات)

---

## Epic 1 — Wave 1: sadinfo Foundation (5 أيام)

### Story 1.1 — `tools/sadinfo/` Skeleton (0.5 يوم)

**هدف:** بناء binary فارغ يُربط بـ shared/lexer ويستجيب لـ --version و --help.

**ACs:**
- AC-1: `tools/sadinfo/CMakeLists.txt` ينشئ target `sadinfo`
- AC-2: `cmake --build build --target sadinfo` ينجح على Win+Linux+Mac
- AC-3: `sadinfo --version` يطبع `sadinfo v1.0.0`
- AC-4: `sadinfo --help` يطبع كل الـ commands
- AC-5: `sadinfo` بدون args يطبع help و exit 0
- AC-6: حجم binary < 5MB

### Story 1.2 — `--dump-keywords` (1 يوم)

**هدف:** استخراج 40 كلمة محجوزة + 25 سياقية إلى JSON.

**ACs:**
- AC-1: `sadinfo --dump-keywords` يطبع JSON صحيح
- AC-2: JSON يحوي `reserved` array بـ 40 entry exactly
- AC-3: JSON يحوي `contextual` array بـ ~25 entry
- AC-4: كل entry يحوي: id, ar, en, token_type, category, since
- AC-5: `--format=yaml` يُخرج YAML صحيح (مُختبر بـ pyyaml)
- AC-6: `--lang=ar` يحذف الحقل `en`، `--lang=en` العكس
- AC-7: `--output=file.json` يكتب لملف
- AC-8: idempotent: تشغيل مرتين ينتج byte-identical output
- AC-9: اختبار unit في `tests/sadinfo/test_dump_keywords.cpp`

### Story 1.3 — `--dump-builtins` (1 يوم)

**هدف:** استخراج ~21 دالة مدمجة + ~50 طريقة على الأنواع.

**ACs:**
- AC-1: JSON يحوي functions array (~21) و methods نظمت بالنوع
- AC-2: كل entry يحوي: name_ar, name_en, signature, params, return_type, examples
- AC-3: examples تُؤخذ من تعليقات doc في كود C++
- AC-4: راية `--filter=printing` تخرج الدوال المتعلقة بالطباعة فقط
- AC-5: نفس AC-5 إلى 9 من Story 1.2

### Story 1.4 — `--dump-errors` (1 يوم)

**هدف:** استخراج ~150 رسالة خطأ من `shared/errors/`.

**ACs:**
- AC-1: JSON يحوي errors array مرتبة بـ error code
- AC-2: كل entry: code, ar, en, recovery_hint, source_file
- AC-3: مجموعات: lexer/parser/semantic/runtime
- AC-4: recovery_hint يُؤخذ من `@recovery` tag في الكود (إن وُجد)
- AC-5: نفس AC-5 إلى 9

### Story 1.5 — `--dump-directives` + `--dump-all` (0.5 يوم)

**هدف:** استخراج التوجيهات `@حجم/@ذري/...` + راية موحَّدة.

**ACs:**
- AC-1: JSON يحوي ~6 توجيهات: حجم, ذري, غير_آمن, وقت_الترجمة, متطاير, تجميع
- AC-2: `--dump-all --output-dir=path/` يكتب 4 ملفات atomically
- AC-3: إذا فشل أي dump، لا يُكتب أي ملف (rollback)
- AC-4: زمن `--dump-all` < 2 ثانية

### Story 1.6 — YAML Schema + `--validate` (0.5 يوم)

**هدف:** schema موحَّد لملفات `_published/*.yaml` وراية تحقق.

**ACs:**
- AC-1: ملف `tools/sadinfo/schemas/published-keyword-v1.json` موجود
- AC-2: `sadinfo --validate file.yaml` يفحص ضد الـ schema
- AC-3: exit 0 للصحيح، exit 1 للخطأ مع رسالة واضحة
- AC-4: 3 ملفات yaml نموذجية في `tests/sadinfo/fixtures/`
- AC-5: اختبار للحالات: ناقص، نوع خاطئ، JSON بدلاً من YAML

### Story 1.7 — `check_keywords.py` (G1+G2) (0.5 يوم)

**هدف:** Quality Gates 1+2 في Python script.

**ACs:**
- AC-1: `python tests/doc_quality/check_keywords.py` يفحص coverage
- AC-2: يفشل إذا coverage < 95%
- AC-3: يفشل إذا keyword جديد بدون doc
- AC-4: يطبع تقرير ملون مع قائمة missing keywords
- AC-5: راية `--update-baseline` تحدّث `last_known.json`

### Story 1.8 — CI workflow `docs-quality.yml` (0.5 يوم)

**هدف:** GitHub Action يجمع كل الفحوصات على 3 منصات.

**ACs:**
- AC-1: workflow يبدأ على pull_request + push
- AC-2: matrix [windows, ubuntu, macos]
- AC-3: يبني sadinfo، يولّد JSON، يفحص G1+G2
- AC-4: artifacts محفوظة (JSON + report)
- AC-5: يفشل PR إذا أي gate فشل
- AC-6: زمن إجمالي < 8 دقائق

---

## Epic 2 — Wave 2: AI Drafts Pipeline (4 أيام)

### Story 2.1 — `tools/sad-ai-docgen/` Setup (1 يوم)

**هدف:** Python tool يستهلك Claude API لإنتاج مسوّدات.

**ACs:**
- AC-1: `pip install -e tools/sad-ai-docgen` ينجح
- AC-2: `sad-ai-docgen --keyword=دالة --lang=ar` يولّد مسودة
- AC-3: المسودة تُكتَب في `website/docs/_drafts/keyword_دالة.draft.yaml`
- AC-4: schema للـ draft مختلف عن published (يحوي `_status: draft`)
- AC-5: API key من env var `CLAUDE_API_KEY`
- AC-6: rate limiting + retry logic

### Story 2.2 — Draft → Published Pipeline (1.5 يوم)

**هدف:** workflow بشري لمراجعة الـ drafts.

**ACs:**
- AC-1: `sad-ai-docgen promote <draft-file>` ينقل من _drafts إلى _published
- AC-2: قبل النقل، يُجبر مراجعة YAML schema (`sadinfo --validate`)
- AC-3: pre-commit hook يحظر `*.draft.yaml` في PR للـ main
- AC-4: GitHub Action تشغّل validate على كل ملف yaml في _published
- AC-5: CONTRIBUTING.md يشرح workflow بالعربية والإنجليزية

### Story 2.3 — Bilingual Validator (G4) (1 يوم)

**هدف:** فحص آلي أن كل entry له AR + EN.

**ACs:**
- AC-1: `python tests/doc_quality/check_bilingual.py` يمر على _published
- AC-2: يفشل إذا أي entry ينقصه AR أو EN
- AC-3: يفشل إذا أي EN يبدأ بـ "TODO" أو فارغ
- AC-4: تقرير يلخّص: X من Y لها AR+EN كامل
- AC-5: مدمج في docs-quality.yml

### Story 2.4 — Examples Validator (G3) (0.5 يوم)

**هدف:** كل مثال في docs يُنفَّذ ويُقارن.

**ACs:**
- AC-1: استخراج examples من _published/*.yaml
- AC-2: تنفيذ كل مثال بـ sad.exe
- AC-3: مقارنة output مع `expected` في YAML
- AC-4: يفشل إذا أي مثال فشل
- AC-5: مدمج في docs-quality.yml

---

## Epic 3 — Wave 3: Website Integration (0.75 يوم) ⭐ مُحدَّث

> **التغيير عن v1:** website كامل موجود → 8 أيام → **0.75 يوم** (بعد UX Review لـ Sally، -25% إضافية من 1 يوم)

### Story 3.1 — VitePress dataLoaders (0.25 يوم)

**هدف:** website يقرأ JSON من _generated/.

**ACs:**
- AC-1: `website/.vitepress/config/data/keywords.data.ts` موجود
- AC-2: `website/.vitepress/config/data/builtins.data.ts` موجود
- AC-3: نفسه لـ errors و directives
- AC-4: `npm run dev` يعرض البيانات في console.log
- AC-5: hot reload يعمل عند تغيير JSON

### Story 3.2 — Vue Components (0.25 يوم) ⭐ مُحدَّث بعد UX Review

**هدف:** 2 components جديدة + 2 wrappers (بدل 4 جديدة).

**مرجع:** `_bmad-output/planning-artifacts/ux-review-vue-components-docs-system.md`

**التغيير عن النسخة الأصلية:** اكتشاف Sally أن `SadCodeRunner.vue` و `FeatureGrid.vue` موجودان وقابلان لإعادة الاستخدام → تخفيض من 0.5 يوم إلى 0.25 يوم (-50%).

**ACs (مُحدَّثة):**
- AC-1: `KeywordCard.vue` (جديد) يعرض ar/en/category/since/summary/example
- AC-2: `BuiltinPlayground.vue` (wrapper) يلتف حول `SadCodeRunner.vue` الموجود (لا CodeMirror مباشر)
- AC-3: `ErrorRecoveryDemo.vue` (جديد) يعرض broken→fixed مع color tokens (`--sad-red` / `--sad-green`)
- AC-4: `DirectiveExample.vue` (wrapper) يستخدم نمط `FeatureGrid.vue` الموجود
- AC-5: كلها تستخدم متغيرات CSS من `style.css` — **zero hardcoded colors** (grep check في CI)
- AC-6: كلها تجاوز axe a11y (zero critical violations) على dark + light + RTL + LTR
- AC-7: Playwright visual regression snapshots لكل component (4 snapshots)
- AC-8: Lighthouse score ≥ 95 على صفحة keyword نموذجية
- AC-9: tested على mobile viewport (375px) بدون layout breaks

**ملاحظات Sally (مُحدَّثة بعد فحص SadCodeRunner.vue):**
- ✅ `SadCodeRunner.vue` ناضج جداً: WASM + CodeMirror 6 + fallback مدمج إلى textarea + كشف RTL/LTR تلقائي + 15s timeout
- ✅ `composables/useSadWasm.ts` يحمّل `/sad.wasm` مع stdout/stderr capture
- ❌ **تحذير مُلغى:** لا حاجة fallback خارجي إلى CodePlaygroundV2 (مدمج بالفعل)
- ⚠️ خط `Cairo` للكلمات العربية الطويلة (`غير_متزامن`) يحتاج `min-width: 0; word-break: break-word`
- ⚠️ ألوان dark mode للأخطاء: اختبر contrast ratio بـ axe
- ⚠️ `aria-label` بالعربية إجباري على KeywordCard
- ⚠️ ErrorRecoveryDemo يحتاج icons (❌/💡) بجانب الألوان (color-only meaning ممنوع)

### Story 3.3 — Auto Sidebar Generation (0.25 يوم)

**هدف:** sidebar يولّد روابط لكل keyword/builtin/error.

**ACs:**
- AC-1: `sidebar.ts` يستهلك dataLoaders
- AC-2: قسم "الكلمات المحجوزة" يحوي 40 رابط
- AC-3: قسم "الدوال المدمجة" يحوي 21 رابط
- AC-4: i18n: العربية في root، الإنجليزية في /en/
- AC-5: search VitePress يفهرس الكل

---

## Epic 4 — Wave 4: Distribution (1 يوم)

### Story 4.1 — npm `@sad-lang/docs-data` (0.5 يوم)

**هدف:** نشر JSON المُولَّد كحزمة npm.

**ACs:**
- AC-1: `tools/npm-publish/` يحوي package.json
- AC-2: يحوي 4 ملفات JSON المُولَّدة
- AC-3: `npm publish --dry-run` ينجح
- AC-4: TypeScript types مُولَّدة من JSON
- AC-5: GitHub Action ينشر على tag

### Story 4.2 — PyPI `sad-doctest` (0.25 يوم)

**هدف:** نشر validators كحزمة Python.

**ACs:**
- AC-1: `tools/sad-doctest/pyproject.toml` موجود
- AC-2: `pip install sad-doctest` يثبّت
- AC-3: `sad-doctest --version` يعمل
- AC-4: subcommands: check-keywords, check-builtins, check-bilingual
- AC-5: PyPI publish عبر GitHub Action

### Story 4.3 — gh-pages Auto-Deploy (0.25 يوم)

**هدف:** نشر website تلقائي عند merge to main.

**ACs:**
- AC-1: `.github/workflows/deploy-docs.yml` في website/
- AC-2: يبدأ على push to main
- AC-3: يبني sadinfo → يولّد JSON → يبني website → ينشر
- AC-4: زمن إجمالي < 5 دقائق
- AC-5: rollback آلي إذا فشل health check

---

## 📊 Coverage Map (Requirements → Stories)

| FR# | Story |
|---|---|
| FR-A-01 إلى A-15 | Stories 1.1-1.6 |
| FR-B-01 إلى B-08 | Stories 1.7, 1.8, 2.3, 2.4 |
| FR-C-01 إلى C-06 | Stories 2.1, 2.2 |
| FR-D-01 إلى D-10 | Stories 3.1-3.3 |
| FR-E-01 إلى E-04 | Stories 4.1-4.3 |

**التغطية:** 100% (44 FR مُغطَّى).

---

## 📈 Sprint Plan (10.75 يوم)

| Day | Sprint/Wave | Stories |
|---|---|---|
| 0.25 | Sprint 0 | Story 0.0 |
| 1.0 | Wave 1 | Story 1.1 |
| 2.0 | Wave 1 | Story 1.2 |
| 3.0 | Wave 1 | Story 1.3 |
| 4.0 | Wave 1 | Story 1.4 |
| 4.5 | Wave 1 | Story 1.5 |
| 5.0 | Wave 1 | Story 1.6 |
| 5.5 | Wave 1 | Story 1.7 |
| 6.0 | Wave 1 | Story 1.8 |
| 7.0 | Wave 2 | Story 2.1 |
| 8.5 | Wave 2 | Story 2.2 |
| 9.5 | Wave 2 | Story 2.3 |
| 10.0 | Wave 2 | Story 2.4 |
| 10.25 | Wave 3 | Story 3.1 (dataLoaders) |
| 10.5 | Wave 3 | Story 3.2 (Vue components — 0.25 يوم بعد UX review) |
| 10.75 | Wave 3 + 4 | 3.3 + 4.1 + 4.2 + 4.3 |

**التوفير الإضافي بعد Sally's UX Review:** 0.25 يوم (-2.3%).

---

> **توقيع:** John (PM) + Amelia + Winston — *"19 stories، 11 يوم، 100% coverage. هذه خطة قابلة للتنفيذ."*
