---
title: "خطة السبرنتات — نظام التوثيق الحي V5"
type: sprint-plan
version: V5.1
status: Approved
date: 2026-06-05
owner: Amelia (Dev) + فريق اللغة
governed_by: [IMPLEMENTATION_PLAN.md, SPRINT_CURRENT (governance)]
scope: مبادرة Living Documentation V5 فقط (26 ستوري)
---

# 🏃 خطة السبرنتات — Living Documentation V5

> توزّع الستوريات الـ26 على **5 موجات** تحترم نموذج التوازي في [IMPLEMENTATION_PLAN §6](IMPLEMENTATION_PLAN.md).
> هذه خطة **مبادرة مستقلة** — لا تستبدل سبرنت الحوكمة العام (`governance/1-policy/sprints/SPRINT_CURRENT.md`).
> كل ستوري يُغلَق فقط ببناء أخضر + اختبارات خضراء (تعريف "تم" §7).

---

## 🌊 الموجة 1 — Foundation (متسلسلة)

| الستوري | الحالة | التقدير | يعتمد على |
|---|---|:---:|---|
| [S-V5-M0-001](stories/implementation/M0-Foundation/STORY-V5-M0-001.md) إنشاء البنية | ready | S | — |
| [S-V5-M0-002](stories/implementation/M0-Foundation/STORY-V5-M0-002.md) جسر keywords | ready | M | M0-001 |
| [S-V5-M0-003](stories/implementation/M0-Foundation/STORY-V5-M0-003.md) JSON Schemas | ready | M | M0-001 |
| [S-V5-M0-004](stories/implementation/M0-Foundation/STORY-V5-M0-004.md) T1 المبدئي | ready | S | M0-003 |

**بوابة الخروج:** البنية موجودة + الجسر أخضر + T1 يعمل على keywords.

---

## 🌊 الموجة 2 — Data (جاهزة البنية، أولوية مبكرة)

> بعد إنجاز **M1-001** (أساس Schema المشترك) — باقي النطاقات قابلة للتوازي.

| الستوري | النطاق | Codegen | التقدير |
|---|---|---|:---:|
| [S-V5-M1-001](stories/implementation/M1-DataPopulation/STORY-V5-M1-001.md) | keywords | ✅ موجود | M |
| [S-V5-M1-003](stories/implementation/M1-DataPopulation/STORY-V5-M1-003.md) | type_methods ⭐ | ✅ موجود (نقل) | S |
| [S-V5-M1-004](stories/implementation/M1-DataPopulation/STORY-V5-M1-004.md) | modules ⭐ | ✅ موجود | S |
| [S-V5-M1-002](stories/implementation/M1-DataPopulation/STORY-V5-M1-002.md) | builtins | ⚠️ تحديث | M |
| [S-V5-M1-005](stories/implementation/M1-DataPopulation/STORY-V5-M1-005.md) | errors | ⚠️ تحديث | L |

**بوابة الخروج:** النطاقات الخمسة مُتحقَّقة بـ T1 + Generated مُولَّد.

---

## 🌊 الموجة 3 — Data (نطاقات جديدة، توازٍ كامل)

| الستوري | النطاق | التقدير |
|---|---|:---:|
| [S-V5-M1-006](stories/implementation/M1-DataPopulation/STORY-V5-M1-006.md) operators | M |
| [S-V5-M1-007](stories/implementation/M1-DataPopulation/STORY-V5-M1-007.md) directives | S |
| [S-V5-M1-008](stories/implementation/M1-DataPopulation/STORY-V5-M1-008.md) types | S |
| [S-V5-M1-009](stories/implementation/M1-DataPopulation/STORY-V5-M1-009.md) patterns ⭐ | M |
| [S-V5-M1-010](stories/implementation/M1-DataPopulation/STORY-V5-M1-010.md) grammar_constructs ⭐ | L |
| [S-V5-M1-011](stories/implementation/M1-DataPopulation/STORY-V5-M1-011.md) stdlib | L |
| [S-V5-M1-012](stories/implementation/M1-DataPopulation/STORY-V5-M1-012.md) learning ⭐ | M |
| [S-V5-M1-013](stories/implementation/M1-DataPopulation/STORY-V5-M1-013.md) oop_constructs ⭐ | L |
| [S-V5-M1-014](stories/implementation/M1-DataPopulation/STORY-V5-M1-014.md) expr_constructs ⭐ | M |

**بوابة الخروج:** كل النطاقات الـ14 مكتملة ومُتحقَّقة (~600+ كيان).

---

## 🌊 الموجة 4 — Library (متسلسلة)

| الستوري | الحالة | التقدير |
|---|---|:---:|
| [S-V5-M2-001](stories/implementation/M2-Library/STORY-V5-M2-001.md) تصميم API | ready | M |
| [S-V5-M2-002](stories/implementation/M2-Library/STORY-V5-M2-002.md) تنفيذ Wrapper | ready | L |
| [S-V5-M2-003](stories/implementation/M2-Library/STORY-V5-M2-003.md) توسعة codegen.cmake | ready | M |
| [S-V5-M2-004](stories/implementation/M2-Library/STORY-V5-M2-004.md) حذف data/language ⚠️ | ready | S |

**بوابة الخروج:** `libsadlangtruth` يبني + بحث O(1) + صفر مرجع لـ `data/language/`.
**⚠️ تنبيه:** M2-004 يتطلب تأكيداً صريحاً من المستخدم قبل الحذف (operationalSafety).

---

## 🌊 الموجة 5 — Quality (متسلسلة)

| الستوري | الحالة | التقدير |
|---|---|:---:|
| [S-V5-M3-001](stories/implementation/M3-Quality/STORY-V5-M3-001.md) T1 شامل | ready | M |
| [S-V5-M3-002](stories/implementation/M3-Quality/STORY-V5-M3-002.md) T2 Language Match | ready | L |
| [S-V5-M3-003](stories/implementation/M3-Quality/STORY-V5-M3-003.md) T3+T4 | ready | M |
| [S-V5-M3-004](stories/implementation/M3-Quality/STORY-V5-M3-004.md) T5 Doc Channels | ready | L |

**بوابة الخروج:** 5 اختبارات إلزامية خضراء + تطابق Truth ↔ اللغة 100%.

---

## 📊 ملخص الموجات

| الموجة | الستوريات | النمط | المُخرَج |
|---|:---:|---|---|
| 1 Foundation | 4 | متسلسل | `language-truth/` يعمل بالتوازي |
| 2 Data (جاهزة) | 5 | M1-001 ثم توازٍ | 5 نطاقات مُتحقَّقة |
| 3 Data (جديدة) | 9 | توازٍ كامل | 14 نطاقاً مكتملة |
| 4 Library | 4 | متسلسل | `libsadlangtruth` |
| 5 Quality | 4 | متسلسل | بوابات الجودة الخمس |
| **الإجمالي** | **26** | — | نظام Living Docs V5 مكتمل |

---

**المسار الحرج:** M0-002 → M1-001 → M1-003 → M2-002 → M3-002.
