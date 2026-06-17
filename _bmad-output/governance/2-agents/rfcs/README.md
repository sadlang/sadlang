# 📋 RFCs — مقترحات تقنية بقيادة الوكلاء

> هذا المجلد يحوي **مقترحات تقنية رسمية (RFCs)** يكتبها الوكلاء قبل تنفيذ ستوريات معينة من [../../1-policy/execution/BACKLOG.md](../../1-policy/execution/BACKLOG.md).

---

## 1) متى تكتب RFC؟

اكتب RFC إذا كانت الستوري التي تنفّذها:

1. تلمس **ملفاً محروساً** (انظر [`docs/governance/GUARDED_FILES.md`](../../../../docs/governance/GUARDED_FILES.md))
2. تُضيف **عقداً معمارياً جديداً** (`SAD_INVARIANT`) — انسّق مع [3-code-contract/](../../3-code-contract/)
3. تتجاوز **WIP** أو **عتبة من THRESHOLDS** بشكل مؤقت — يستلزم موافقة PM
4. تُعدِّل **PMF** أو أي وثيقة في [1-policy/](../../1-policy/) — يستلزم ADR في `1-policy/proposals/`

---

## 2) صيغة الاسم

```
B-XXX-<short-slug>.md
```

- `B-XXX` = معرّف الستوري من BACKLOG
- `short-slug` = وصف موجز بأحرف صغيرة وشُرَط

**أمثلة:**

- `B-003-typed-thresholds-loader.md`
- `B-007-pmf-monthly-check-workflow.md`

---

## 3) القالب المختصر

```markdown
---
rfc: B-XXX
author: agent_<اسمك>
date: YYYY-MM-DD
status: DRAFT | UNDER_REVIEW | APPROVED | REJECTED
---

# B-XXX — <العنوان>

## السياق
## المشكلة
## الحل المقترح
## البدائل المرفوضة
## التأثير على عقود `SAD_INVARIANT`
## خطة الاختبار
## مخاطر التراجع
```

---

## 4) حالياً

**لا توجد RFCs مفتوحة.** أول RFC متوقع: `B-003` (Typed Thresholds Loader) — وكيل `gamma`.
