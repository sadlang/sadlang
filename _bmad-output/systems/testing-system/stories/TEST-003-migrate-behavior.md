---
storyId: TEST-003
title: "ترحيل dual_execution إلى behavior/ مع تطبيق الحتمية على التزامن"
status: done
priority: P0
estimate: 8h
createdAt: 2026-06-11
updatedAt: 2026-06-12
completedAt: 2026-06-12
assignee: Claude (Fable 5)
dependsOn: [TEST-002, TEST-007]
blocks: [TEST-005]
relatedStories: [TEST-002, TEST-005, TEST-007]
relatedDecisions: [ADR-001, ADR-002, ADR-004]
acceptanceCriteria:
  - AC-01: محتوى dual_execution/features → behavior/sections/ بنفس الترقيم
  - AC-02: كل قسم له COVERAGE.md + RISK.md + مجلد _negative/
  - AC-03: قسم 07 (تزامن) يستخدم @unordered/@nondeterministic ولا يرفرف
  - AC-04: runner.py --level full أخضر بعد الترحيل بلا تراجع عن baseline
---

# TEST-003 — ترحيل السلوك إلى behavior/

## السياق

`dual_execution` هو الأنضج وأقل المكوّنات مخاطرة، فيُرحَّل أولاً. **يعتمد على TEST-007**
لأن قسم 07 (تزامن) لا يمكن ترحيله بمقارنة حرفية (مخاطرة درجة 9 — [ADR-004](../decisions/ADR-004-determinism-and-risk.md))؛
يحتاج وضع التطبيع جاهزاً.

## معايير القبول (AC)

| ID | الوصف | التحقق | معرّف التتبّع |
|---|---|---|---|
| AC-01 | `features/NN_*` → `behavior/sections/NN_*` | فحص بنية | T1.003-DOC-001 |
| AC-02 | لكل قسم `COVERAGE.md` + `RISK.md` + `_negative/` | فحص + مراجعة | T1.003-DOC-002 |
| AC-03 | قسم 07 أخضر 20× متتالية بلا رفرفة | `runner.py --section تزامن` ×20 | T1.003-E2E-001 |
| AC-04 | `runner.py --level full` ≥ baseline (TEST-001 AC-03) | مقارنة تقارير | T1.003-INT-001 |

## المهام (Tasks)

- [x] **T1: نقل الأقسام** (AC-01) ✅ 2026-06-12
  - [x] T1.1: `git mv dual_execution/features/* → behavior/sections/*` — **452/452** (git رصدها renames = التاريخ محفوظ). تفصيل: 127 كانت منسوخة مسبقاً (تحقق hash ×127 متطابق) فحُوّلت لنقل بحذف الأصل؛ 9 ملفات قسم 03 + أقسام 04-12 نُقلت `git mv` مباشرة.
  - [x] T1.2: `P0_smoke` (5 ملفات) نُقلت — كانت بنية behavior/P0_smoke فارغة. `rules_matrix` كانت منقولة منذ TEST-002.
  - [x] T1.3: الأرشيف القديم (380 ملفاً: core/oop/match/advanced/concurrency/errors/negative/stdlib) → `tests/_archive/dual_execution_legacy/` بـ`git mv` + README (GR-04 أرشفة لا حذف). دمج الفريد منه → TEST-005.
- [x] **T2: وثائق القسم** (AC-02) ✅ 2026-06-12
  - [x] T2.1: `COVERAGE.md` ×12 من مسودة TEST-001 + بنية المجلدات الفعلية
  - [x] T2.2: `RISK.md` ×12 — كانت مثبتة من TEST-007 (تحقّق وجود)
- [x] **T3: حتمية التزامن** (AC-03) ✅ 2026-06-12
  - [x] T3.1: المصنّف (`--classify` ×5 بالمفسر) أظهر **0 لا-حتمي / 23 حتمي** → **لا وسوم مطلوبة** (وسمها كان سيخفي علل تكافؤ حقيقية — BF-09). أُزيلت نسختا تكرار مطابقتان hash (`01_أساسي/022`، `04_بمهلة/099`) → القسم 21 ملفاً.
  - [x] T3.2: 20 تشغيلاً متتالياً = **نمط فشل واحد ثابت (16/21) — صفر رفرفة**. الإخفاقات الخمسة الثابتة علل تكافؤ حقيقية (مفسر≠مترجم): 057, 058, 089, 091, 099 — موثقة في COVERAGE.md للقسم وتحتاج قصة إصلاح (BF-08).
- [x] **T4: بوّابة عدم التراجع** (AC-04) ✅ 2026-06-12
  - [x] T4.1: `config.yaml`: `tests_dir → tests/behavior`، `features/ → sections/`، إزالة مراجع المجلدات المؤرشفة من كل المستويات؛ تحديث fallback في `runner.py` (موضعان). إصلاح إضافي: `report_html.py` نُقل لجوار الراننر (كان `--html` مكسوراً منذ TEST-002)، و3 مسارات مكسورة في `ci.yml` + 5 ملفات مرجعية بمهارة sad-lang-dev.
  - [x] T4.2: full قبل الترحيل (837): 86.5% — مجموعة الترحيل = **86.0% (394/458)**. full بعد الترحيل (457): **86.0% (393/457)** + **مقارنة لكل اختبار: 0 تراجع، 0 مفقود، 0 جديد**. خط الأساس محفوظ: `status/baseline_full_2026-06-12.json`.

## ملاحظات التنفيذ

- استخدم `git mv` للحفاظ على التتبّع (NFR-04). لا حذف — المكرّر يُؤرشف.
- إن فشل AC-03 (رفرفة باقية)، أوقف وارجع لـ TEST-007 — لا تُخفِ الرفرفة بـ retry (BF-09).

## Dev Agent Record

| التاريخ | الوكيل | الإجراء |
|---|---|---|
| 2026-06-12 | Claude (Fable 5) | T0: تحقق ثنائيات + P0 أخضر 100% (5/5) قبل أي نقل + full قبلي محفوظ كخط أساس |
| 2026-06-12 | Claude (Fable 5) | T1: ترحيل 452 ملفاً (git mv/rm — renames في git) + P0_smoke + أرشفة 380 ملفاً قديماً |
| 2026-06-12 | Claude (Fable 5) | T2: COVERAGE.md ×12 (RISK.md ×12 موجودة من TEST-007) |
| 2026-06-12 | Claude (Fable 5) | T3: مصنّف 0 لا-حتمي → لا وسوم؛ 20× = صفر رفرفة؛ 5 علل تكافؤ حقيقية موثّقة؛ إزالة تكرارَي 022/099 |
| 2026-06-12 | Claude (Fable 5) | T4: config/runner → behavior؛ إصلاح ci.yml (3 مسارات مكسورة) + 5 وثائق مهارة + نقل report_html.py؛ full = 86.0% بمقارنة لكل اختبار: 0 تراجع |

## File List

- `tests/behavior/sections/**` — 450 ملف .ص (452 منقولاً − تكراران) + COVERAGE.md ×12 (جديدة)
- `tests/behavior/P0_smoke/*.ص` — 5 ملفات (منقولة)
- `tests/_archive/dual_execution_legacy/**` — 380 ملفاً مؤرشفاً + `README.md` (جديد) + `TESTING_GUIDE.md`
- `tests/config.yaml` — tests_dir/sections/المستويات (معدَّل)
- `tests/runner.py` — fallback المسار + تعليق (معدَّل)
- `tests/report_html.py` — منقول من dual_execution (إصلاح `--html`)
- `.github/workflows/ci.yml` — 3 مسارات runner مصحَّحة
- `.github/skills/sad-lang-dev/references/{build-test,playbooks,interconnected-systems,delivery-checklist,systems-catalog}.md` — مسارات محدَّثة
- `_bmad-output/systems/testing-system/status/baseline_full_2026-06-12.json` — دليل خط الأساس (جديد)
- `_bmad-output/systems/testing-system/{stories/TEST-003-*.md, status/implementation_status.md, epics/BACKLOG.md}` — حوكمة
