---
storyId: TEST-006
title: "ربط CI بالمستويات الهرمية مع burn-in وبوّابة قرار PASS/CONCERNS/FAIL"
status: done
priority: P1
estimate: 6h
createdAt: 2026-06-11
updatedAt: 2026-06-12
completedAt: 2026-06-12
assignee: Amelia
dependsOn: [TEST-005, TEST-007]
blocks: []
relatedStories: [TEST-005, TEST-007]
relatedDecisions: [ADR-002, ADR-004]
acceptanceCriteria:
  - AC-01: pre-commit يشغّل P0؛ PR يشغّل P1؛ nightly يشغّل P2؛ release يشغّل full
  - AC-02: أمر واحد موحّد لكل مستوى (لا استدعاء مجلدات متفرقة)
  - AC-03: burn-in للاختبارات الجديدة في PR (تشغيل 5×)
  - AC-04: بوّابة CI تصدر قرار PASS/CONCERNS/FAIL حسب درجات المخاطر
---

# TEST-006 — ربط CI

## السياق

بعد استقرار البنية، تُربط دورة CI بالمستويات الهرمية عبر المشغّل الموحّد. آخر قصة.
أضاف نقد Murat ([ADR-004](../decisions/ADR-004-determinism-and-risk.md)) AC-03/AC-04: burn-in لكشف الرفرفة
(R6) وبوّابة قرار صريحة (R4).

## معايير القبول (AC)

| ID | الوصف | التحقق | معرّف التتبّع |
|---|---|---|---|
| AC-01 | كل بوّابة CI تستدعي `runner.py --level <L>` المناسب | فحص workflow | T1.006-CI-001 |
| AC-02 | لا استدعاءات مجلدات قديمة في CI/cmake | `grep` نظيف | T1.006-CI-002 |
| AC-03 | الاختبار الجديد في PR يُشغَّل 5× (burn-in) | تشغيل PR تجريبي | T1.006-CI-003 |
| AC-04 | البوّابة تُخرِج PASS/CONCERNS/FAIL | تشغيل مع مخاطرة مفتوحة | T1.006-CI-004 |

## المهام (Tasks)

- [x] **T1: ربط المستويات** (AC-01, AC-02) ✅ 2026-06-12
  - [x] T1.1: خطوة CI واعية بالحدث: `push→P0`، `pull_request→P1`، `schedule→P2` (مُطلق nightly cron مُضاف) — `ci.yml`
  - [x] T1.2: `cmake/dual_tests.cmake` يستدعي `tests/runner.py --level` (موحّد، كان سليماً وظيفياً)
  - [x] T1.3: تنظيف تعليقات المسارات القديمة (`dual_execution/core` → `behavior/...`)؛ المتبقّي = اسم المنهجية + وسوم CTest (لا مسارات)
- [x] **T2: burn-in** (AC-03) ✅ 2026-06-12
  - [x] T2.1: خطوة CI تكشف ملفات `.ص` الجديدة/المعدَّلة في PR (`git diff`) وتشغّلها `--repeat 5`
  - [x] T2.2: تفشل الخطوة (exit 1) إن رفرف/فشل أي اختبار جديد
- [x] **T3: بوّابة القرار** (AC-04) ✅ 2026-06-12
  - [x] T3.1: `--gate` (+`--gate-floor 86.0` لـ P1/P2) → PASS/CONCERNS/FAIL؛ تراجع تحت الأساس أو فشل دخان = FAIL (exit 1)
  - [x] T3.2: توثيق بوّابة CI والأوامر في `tests/README.md`
- [x] **T4: إغلاق السبرنت** (GR-03) ✅ 2026-06-12
  - [x] T4.1: `sprints/SPRINT_2026-06-12_RETRO.md`

## ملاحظات التنفيذ

- تأكّد من توفّر `sadc.exe` (Release) في CI لمستويات التنفيذ المزدوج (ADR-002).

## Dev Agent Record

| التاريخ | الوكيل | الإجراء |
|---|---|---|
| 2026-06-12 | Amelia | فحص جودة كود العمل المُنجَز (TEST-003/004/005) + التحقق من سلامة runner.py (10/10، P0=100%). ربط CI بالمستويات الهرمية (push→P0/PR→P1/nightly→P2) + بوّابة `--gate` + burn-in للملفات الجديدة في PR + مُطلق schedule. تنظيف تعليقات cmake القديمة. توثيق README. تحقّقت YAML سليم. كتبتُ RETRO. **لم أُجرِ أي عملية git (التزاماً بطلب المستخدم).** |

## File List

- `.github/workflows/ci.yml` — خطوة تنفيذ مزدوج واعية بالمستوى + بوّابة + خطوة burn-in + مُطلق schedule
- `cmake/dual_tests.cmake` — تنظيف تعليقات المسارات القديمة
- `tests/README.md` — قسم «بوّابة CI» + الأوامر
- `_bmad-output/systems/testing-system/sprints/SPRINT_2026-06-12_RETRO.md` — مراجعة السبرنت (جديد)

## ملاحظة تحقق (GR-01)

محاكاة بوّابة CI محلياً: `--level P0 --gate` → **PASS (exit 0)** · `--gate --gate-floor 70`
على قسم فيه فشل → **CONCERNS (exit 0، لا يكسر البناء)**. YAML لـ ci.yml مُتحقَّق ببرنامج (7 jobs، 3 مُطلقات).
