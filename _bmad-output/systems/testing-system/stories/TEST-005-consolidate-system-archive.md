---
storyId: TEST-005
title: "دمج اختبارات الأدوات في system/ مع عتبات NFR وأرشفة المجلدات القديمة"
status: done
priority: P1
estimate: 7h
createdAt: 2026-06-11
updatedAt: 2026-06-12
completedAt: 2026-06-12
assignee: Claude (Fable 5)
dependsOn: [TEST-003, TEST-004]
blocks: [TEST-006]
relatedStories: [TEST-003, TEST-004, TEST-006]
relatedDecisions: [ADR-001, ADR-004]
acceptanceCriteria:
  - AC-01: lsp/formatter/pkg/sadinfo/codegen/docs/benchmark/fuzzing تحت system/
  - AC-02: tests/_archive/ يحوي القديم + README يشير للموقع الجديد لكل مجلد
  - AC-03: عدد مجلدات tests/ الجذرية ≤ 6 (مقيس بدليل فعلي)
  - AC-04: عتبات NFR رقمية في system/benchmark (تراجع أداء = فشل)
---

# TEST-005 — طبقة النظام وعتبات NFR والأرشفة

## السياق

اختبارات الأدوات والأداء متفرّقة. تُدمج في `system/` وتُؤرشف البقايا. أضاف نقد Murat
([ADR-004](../decisions/ADR-004-determinism-and-risk.md)) AC-04: لا قيمة لاختبار أداء بلا عتبة رقمية تُفشِل
عند التراجع (R5، درجة 4 — NFR).

## معايير القبول (AC)

| ID | الوصف | التحقق | معرّف التتبّع |
|---|---|---|---|
| AC-01 | كل أداة في `system/<أداة>/` واحدة | فحص بنية | T1.005-DOC-001 |
| AC-02 | `tests/_archive/README.md` يخرّط كل قديم → جديد | مراجعة (GR-04) | T1.005-DOC-002 |
| AC-03 | جذر `tests/` ≤ 6 عناصر (مقيس) | `ls tests/` موثّق | T1.005-INT-001 |
| AC-04 | عتبة أداء تُفشِل عند تجاوز X% تراجع | تشغيل benchmark مع عتبة | T1.005-INT-002 |

## المهام (Tasks)

- [x] **T1: دمج الأدوات** (AC-01) ✅ 2026-06-12
  - [x] T1.1: hub، sadinfo، lsp (+ملفا الجذر الشاردان)، pkg، tools، dev_tools_test→dev_tools، docs_extraction+doc_gen_dual_execution+توثيق→`system/docs/`، builtin_errors — كله بـ`git mv` مع تسوية تداخل هياكل TEST-002 الفارغة (sadinfo/lsp/pkg/fuzzing كانت متداخلة وفُلطحت).
  - [x] T1.2: benchmark+benchmarks+performance+stress → `system/benchmark/` (60 ملفاً).
  - [x] T1.3: network → `system/network/` خلف `SAD_ENABLE_NETWORK_TESTS` (افتراضي OFF — لا نداءات خارجية في CI).
- [x] **T2: الأرشفة** (AC-02, AC-03) ✅ 2026-06-12
  - [x] T2.1: 22 مجلداً غير مصنّف → `tests/_archive/` + `tests/_archive/README.md` بخريطة كاملة قديم→جديد ووجهة الدمج المستقبلية لكلٍّ (GR-04). استثناءان حيّان: regression → `behavior/_regression` (مربوط ctest — حُدِّثت بادئتاه)، وملفا optional_null → `behavior/_regression` (كان الالتقاط يشير لـownership المؤرشف).
  - [x] T2.2: **جذر tests = 5 مجلدات** (behavior, framework, system, unit, _archive) + 4 ملفات مشغّل — مقيس بـ`Get-ChildItem` (الدليل في سجل التنفيذ). AC-03 (≤6) ✓
- [x] **T3: تكامل المشغّل** (AC-01) ✅ — طبقة system تعمل عبر ctest (وسوم System): hub/sadinfo/docgen/NFR. الراننر يغطي behavior (سلوك .ص) — توزيع الأدوار موثّق في `tests/_archive/README.md`.
- [x] **T4: عتبات NFR** (AC-04) ✅ 2026-06-12
  - [x] T4.1: `system/benchmark/nfr_gate.py` — وسيط 7 تشغيلات لثلاثة برامج مرجعية من P0_smoke؛ العتبة = أساس مقيس × (1+تسامح 50%) من `nfr_thresholds.yaml` المعتمد في git (لا أرقام سحرية — CW-10). المعايرة `--calibrate` قرار بشري لا خطوة CI.
  - [x] T4.2: مسجّلة كـ`NFR_Gate` في ctest (وسم System;nfr) وتفشل بكود 1 عند الخرق. **القياس المعتمد:** hello=41.6ms، arithmetic=43.7ms، functions=43.7ms (حدود 62.4/65.6/65.6ms). أول تشغيل: ✅ خضراء.

## ملاحظات التنفيذ

- العتبات تُعاير من baseline (TEST-001)؛ لا أرقام سحرية (CW-10).

## التحقق النهائي (GR-01)

- ctest: **138 مسجلاً (137 + NFR_Gate) — 53 ناجحاً** مقابل 52 بعد TEST-004 — **صفر تراجعات وصفر تحولات** (diff لكل اختبار بين `_ctest_after.txt` و`_ctest_after5.txt`).
- runner P0: **100%** بعد كل النقل.

## Dev Agent Record

| التاريخ | الوكيل | الإجراء |
|---|---|---|
| 2026-06-12 | Claude (Fable 5) | S1: دمج 10 مجلدات أدوات في system/ + تحديث tests.cmake وdoc_gen_dual_tests.cmake + فلطحة التداخل |
| 2026-06-12 | Claude (Fable 5) | S2: دمج الأداء الرباعي + عزل الشبكة خلف SAD_ENABLE_NETWORK_TESTS (OFF) |
| 2026-06-12 | Claude (Fable 5) | S3: أرشفة 22 مجلداً + README خريطة + regression وoptional_null إلى behavior/_regression + الجذر = 5 |
| 2026-06-12 | Claude (Fable 5) | S4: nfr_gate.py + nfr_thresholds.yaml معايَر + NFR_Gate في ctest — خضراء |
| 2026-06-12 | Claude (Fable 5) | S5: ctest 53/138 صفر تراجعات + P0 100% |

## File List

- `tests/system/{hub,sadinfo,lsp,pkg,tools,dev_tools,docs,builtin_errors,benchmark,network,fuzzing}/**` — منقول (git mv)
- `tests/system/benchmark/nfr_gate.py` + `nfr_thresholds.yaml` — جديدان (AC-04)
- `tests/behavior/_regression/**` — regression (72) + optional_null (2) منقولة
- `tests/_archive/{22 مجلداً}/**` + `tests/_archive/README.md` — خريطة الأرشيف (جديد)
- `cmake/tests.cmake` — مسارات system + علم الشبكة + NFR_Gate (معدَّل)
- `cmake/tests_comprehensive.cmake` — بادئتا regression + التقاط optional_null (معدَّل)
- `cmake/doc_gen_dual_tests.cmake` — مسار doc_gen الجديد (معدَّل)
- `CMakeLists.txt` — خيار SAD_ENABLE_NETWORK_TESTS (معدَّل)
