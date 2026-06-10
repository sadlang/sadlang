---
title: "الستوريات النَشطة — نظام التوثيق الحي"
type: stories-index
date: 2026-06-02
status: ACTIVE
parent_canonical: ../IMPLEMENTATION_PLAN.md
---

# 📋 الستوريات النَشطة — نظام التوثيق الحي

> هذا المجلد يَحوي **جميع الستوريات النَشطة** للنظام. الستوريات الأصلية كانت موزَّعة عبر طبقتين (`2-architecture/stories/` و `3-implementation/stories/`) — تم تَوحيدها هنا في 2026-06-02.

## التَنظيم

| المجلد الفرعي | الغرض | عدد الستوريات |
|---|---|---|
| [`architecture/`](architecture/) | ستوريات معمارية (تَصميم البنية) | 12 |
| [`implementation/`](implementation/) | ستوريات تَنفيذية ذرّية (S-000a..S-016) | 24 |

## كيف تُستخدم

- خَريطة الطريق + جدول الحالة في [../IMPLEMENTATION_PLAN.md](../IMPLEMENTATION_PLAN.md)
- كل ستوري قابلة للتَنفيذ في Sprint
- عند اكتمال ستوري: حدِّث جدول الحالة في `IMPLEMENTATION_PLAN.md` + أنشئ VERIFICATION_REPORT في `governance/1-policy/status/`

## القواعد

1. **لا تُضاف ستوري جديدة** بدون ربطها بمعلم (M1–M7) في `IMPLEMENTATION_PLAN.md`
2. **لا تُحذف ستوري** — تُعلَّم `status: Cancelled` مع `cancelReason`
3. **التَسمية الجديدة:** ستوريات تَنفيذية = `S-NNN-<slug>.md`، معمارية = `story-N.N-<slug>.md`
