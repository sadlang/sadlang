---
storyId: TEST-007
title: "بوّابة جودة الاختبار: حتمية الخرج + سجل مخاطر + burn-in"
status: done
priority: P0
estimate: 10h
createdAt: 2026-06-11
updatedAt: 2026-06-11
assignee: TBD
dependsOn: [TEST-001, TEST-002]
blocks: [TEST-003, TEST-006]
relatedStories: [TEST-001, TEST-002, TEST-003, TEST-006]
relatedDecisions: [ADR-002, ADR-004]
acceptanceCriteria:
  - AC-01: runner.py يدعم @unordered (فرز) + تساهل عائم + @nondeterministic (اختبار خصائص)
  - AC-02: RISK.md لكل قسم بدرجة probability×impact ومالك وتخفيف للدرجات ≥6
  - AC-03: المستويات P0/P1/P2/P3 مشتقّة من درجة المخاطر لا من اتساع القسم
  - AC-04: burn-in (تشغيل الاختبار الجديد 5×) + بوّابة قرار PASS/CONCERNS/FAIL
---

# TEST-007 — بوّابة الجودة والحتمية والمخاطر

## السياق

ناتج مراجعة معماري الاختبارات (Murat) — [ADR-004](../decisions/ADR-004-determinism-and-risk.md)
و [REVIEW](../status/REVIEW_2026-06-11.md). يعالج مخاطرة درجة 9 (رفرفة التزامن/العشري)
ويعيد بناء الأولوية على المخاطر. **قصة حاجزة — تَحجز TEST-003 (لا يمكن ترحيل قسم 07
بمقارنة حرفية) وTEST-006 (بوّابة القرار).**

## معايير القبول (AC)

| ID | الوصف | التحقق | معرّف التتبّع |
|---|---|---|---|
| AC-01 | وضع تطبيع (فرز/عائم/خصائص) في المقارنة | اختبار تزامن أخضر 20× بلا رفرفة | T1.007-INT-001 |
| AC-02 | `RISK.md` لكل قسم بدرجة ومالك وتخفيف | مراجعة + تدقيق GR-01 | T1.007-DOC-001 |
| AC-03 | اشتقاق المستويات من الدرجة | مراجعة `config.yaml` مقابل `RISK.md` | T1.007-DOC-002 |
| AC-04 | burn-in + بوّابة قرار | تشغيل `--burn-in 5` + تقرير PASS/CONCERNS/FAIL | T1.007-INT-002 |

## المهام (Tasks)

- [x] **T1: وضع التطبيع في runner.py** (AC-01)
  - [x] T1.1: تحليل وسوم `@unordered`/`@nondeterministic` (regex + parse_metadata)
  - [x] T1.2: فرز الأسطر عند `@unordered` قبل المقارنة (`compare_outputs`)
  - [x] T1.3: تساهل عائم بـ `epsilon` (`_lines_equal_with_float`) — **مقيس: 83.2%→86.6%**
  - [x] T1.4: `@nondeterministic` → مقارنة كمجموعة مرتّبة
  - [x] T1.5: اختبار وحدة `tests/unit/testing/test_runner_normalization.py` = **10/10**
  - [x] T1.6: مصنّف `--classify` — **مُتحقَّق:** يكشف 036 (لاحتمي) ويفصل 058 (خطأ مترجم حتمي)
- [x] **T2: سجل المخاطر** (AC-02)
  - [x] T2.1: قالب `_TEMPLATE/RISK.md`
  - [x] T2.2: 12 `RISK.md` لكل قسم — مُعاير بأدلة baseline (GR-01)
- [x] **T3: اشتقاق المستويات** (AC-03)
  - [x] T3.1: `config.yaml` → كتلة `risk_levels` (درجة↔مستوى لكل قسم)
  - [x] T3.2: جدول الاشتقاق موثّق في `tests/README.md` + RISK.md
- [x] **T4: burn-in وبوّابة القرار** (AC-04)
  - [x] T4.1: `--repeat N` (burn-in موجود) + `--gate-floor`
  - [x] T4.2: `--gate` → PASS/CONCERNS/FAIL (مُتحقَّق: P0→PASS، حد 100%→FAIL، فشل→CONCERNS)
  - [~] T4.3: تزامن أخضر 20× — يُنفَّذ على الأقسام بعد وسمها في TEST-003 (الوسوم تُطبَّق أثناء الترحيل)

## ملاحظات التنفيذ

- «فضّل المستويات الأدنى»: قبل اختبار سلوكي مزدوج لمنطق parser/type-check، تحقّق أنه
  ليس مُغطّى (أو ينبغي تغطيته) في `unit/` — التنسيق مع TEST-004.
- الحل جذري لا ترقيع: لا تُخفِ الرفرفة بـ retry؛ طبّع المصدر الحقيقي للاحتمية (BF-09).
- **تصحيح حرج ([BASELINE_FINDINGS](../status/BASELINE_FINDINGS_2026-06-11.md)):** إخفاقات
  تكافؤ التزامن **مزيج** — لا-حتمية فعلية (تُوسَم) + أخطاء مترجم حقيقية (تُسجَّل وتُحال
  لفريق المترجم، **لا تُقنَّع**). الوسم مشروط بدليل رفرفة بالمفسر وحده.

## Dev Agent Record

| التاريخ | الوكيل | الإجراء |
|---|---|---|
| 2026-06-11 | Amelia | نفّذتُ وضع التطبيع (`compare_outputs`/`_lines_equal_with_float`) + مصنّف `--classify` + بوّابة `--gate` في runner.py. اختبار وحدة 10/10. أثر مقيس: 83.2%→86.6%. 12 RISK.md + risk_levels في config. |
| 2026-06-12 | Amelia | **استرجاع بعد ارتداد خارجي:** تعديلات runner.py على ملف متتبَّع أُعيدت لـ HEAD بين الجلستين. أعدتُ تطبيق كل التعديلات (git mv + التطبيع + المصنّف + البوّابة) وتحقّقت (10/10، P0=100%، classify يكشف 036)، ثم **حفظتُ بـ commit `86cd03eb`** لمنع تكرار الضياع. |

## File List

- `tests/runner.py` (تطبيع + مصنّف + بوّابة + فصل الموقع)
- `tests/config.yaml` (كتلة `risk_levels`)
- `tests/unit/testing/test_runner_normalization.py` (اختبار وحدة، 10/10)
- `tests/behavior/sections/*/RISK.md` (12 ملف مُعاير)
- `tests/behavior/sections/_TEMPLATE/RISK.md`

## ملاحظة استرجاع (BF-30)

الدرس: تعديلات على ملفات **متتبَّعة** في git قابلة للارتداد إن لم تُحفَظ بـ commit
عبر حدود الجلسات. **الإجراء الوقائي:** حفظ العمل بـ commit فور التحقق منه (طُبِّق:
`86cd03eb`). الملفات الجديدة غير المتتبَّعة (وثائق `_bmad-output` المُتجاهَلة) لم تتأثر.
