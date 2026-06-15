---
storyId: TEST-002
title: "إنشاء هيكل الطبقات الثلاث ورفع runner/config مع وضع التطبيع"
status: done
priority: P0
estimate: 5h
createdAt: 2026-06-11
updatedAt: 2026-06-11
assignee: TBD
dependsOn: [TEST-001]
blocks: [TEST-003, TEST-004, TEST-007]
relatedStories: [TEST-001, TEST-003, TEST-007]
relatedDecisions: [ADR-001, ADR-002, ADR-004]
acceptanceCriteria:
  - AC-01: مجلدات unit/ behavior/ system/ منشأة بـ README لكل منها
  - AC-02: runner.py و config.yaml في جذر tests/ ويعملان من هناك
  - AC-03: tests/README.md يشرح الطبقات الثلاث وكيف تضيف اختباراً
  - AC-04: هيكل القسم الموحّد يشمل COVERAGE.md و RISK.md و _negative/
---

# TEST-002 — هيكل الطبقات الثلاث

## السياق

بناء الهيكل العظمي قبل نقل المحتوى. لا ترحيل محتوى هنا. عُدِّلت بـ [ADR-004](../decisions/ADR-004-determinism-and-risk.md):
الهيكل الموحّد للقسم يجب أن يحجز مكاناً لـ `RISK.md` ووثائق الحتمية منذ البداية.

## معايير القبول (AC)

| ID | الوصف | التحقق | معرّف التتبّع |
|---|---|---|---|
| AC-01 | `unit/ behavior/ system/` + README بكل منها | فحص بنية | T1.002-DOC-001 |
| AC-02 | `tests/runner.py --level P0` يعمل من الجذر | تشغيل ناجح | T1.002-INT-001 |
| AC-03 | `tests/README.md` يوثّق الطبقات + إضافة اختبار + الوسوم | مراجعة | T1.002-DOC-002 |
| AC-04 | قالب قسم موحّد (COVERAGE.md + RISK.md + _negative/) | فحص قالب | T1.002-DOC-003 |

## المهام (Tasks)

- [x] **T1: الهيكل العظمي** (AC-01)
  - [x] T1.1: إنشاء `tests/{unit,behavior,system}/` + المجلدات الفرعية + `.gitkeep` للفارغة
  - [x] T1.2: README موسّع (عربي) لكل طبقة (unit/behavior/system)
- [x] **T2: رفع المشغّل** (AC-02)
  - [x] T2.1: `git mv runner.py + config.yaml` للجذر (تتبّع محفوظ)
  - [x] T2.2: فصل موقع المشغّل عن المحتوى عبر `config.paths.tests_dir` (توافق مؤقت = dual_execution)؛ تصحيح `project_root` (السطر 861) و`tests_dir` (869)
  - [x] T2.3: `runner.py --level P0` = **100%** من الجذر الجديد · `--section متغيرات` يُحلّ صحيحاً
  - [x] T2.4 (إضافي — لا تراجع): تحديث مرجع CI (`ci.yml:356`) + وثائق المهارة (build-test/playbooks)
- [x] **T3: وثيقة الجذر** (AC-03)
  - [x] T3.1: `tests/README.md` — الطبقات + بنية القسم + الأوامر + وسوم الحتمية
- [x] **T4: قالب القسم الموحّد** (AC-04)
  - [x] T4.1: `behavior/sections/_TEMPLATE/` بـ `COVERAGE.md` + `RISK.md` + `_negative/` + README

## ملاحظات التنفيذ

- `config.yaml` الحالي يشير إلى `tests/dual_execution`؛ يبقى توافق مؤقت حتى TEST-003.
- وضع التطبيع نفسه (منطق `runner.py`) يُنفَّذ في TEST-007 — هنا فقط نحجز البنية والوثائق.

## Dev Agent Record

| التاريخ | الوكيل | الإجراء |
|---|---|---|
| 2026-06-11 | Amelia | أنشأتُ الطبقات الثلاث + READMEs + قالب القسم. رفعتُ runner.py/config.yaml للجذر بـ git mv، وفصلتُ موقع المشغّل عن المحتوى (config.paths.tests_dir). P0=100% بلا تراجع. صحّحتُ مرجع CI ووثائق المهارة لتفادي كسر المسار. |

## File List

- `tests/runner.py` (منقول من dual_execution + تصحيح project_root/tests_dir)
- `tests/config.yaml` (منقول)
- `tests/README.md` (جديد)
- `tests/unit/README.md`، `tests/behavior/README.md`، `tests/system/README.md` (جديد)
- `tests/behavior/sections/_TEMPLATE/{README,COVERAGE,RISK}.md` (جديد)
- `tests/{unit,behavior,system}/**` (هيكل + .gitkeep ×16)
- `.github/workflows/ci.yml` (تصحيح مسار runner — السطر 356)
- `.github/skills/sad-lang-dev/references/{build-test,playbooks}.md` (تصحيح مسار)

## ملاحظة عدم التراجع

- P0 = 100% من الموقع الجديد · `--section متغيرات` = 80% (= baseline لذلك القسم، بلا تراجع).
- المحتوى لم يُنقل بعد (يبقى في `tests/dual_execution/` حتى TEST-003)؛ المشغّل يصل إليه عبر config.
