# 📋 GOVERNANCE_DIGEST — ملخّص الحوكمة (مولَّد آليًّا)

<!-- ⚠️ ملف مولَّد آليًّا — لا تُحرّره يدويًّا. -->
<!-- مولَّد-من: بصمة=1378323475eb721e | المولّد=scripts/governance/gen_governance_digest.py -->
<!-- إعادة التوليد: python scripts/governance/gen_governance_digest.py -->

> هذا الـDigest بوابة الحوكمة المتدرّجة (ADR-GOV-001 / RFC sadlang/rfcs#5).
> للمهام الخفيفة (تعديل داخل ملف قائم تحت `status/` أو `stories/`) يكفي قراءته.
> للمهام البنيويّة (إنشاء/حذف ملف، أو لمس `decisions/`|`planning/`|`sprints/`)
> تلزم القراءة الكاملة للملفات الخمسة (انظر `.github/copilot-instructions.md`).

## 🏃 السبرنت الحالي

- الأسبوع: 2026-05-22 → 2026-05-29 (Sprint #1 من حقبة Execution Layer)
- المصدر: `_bmad-output/governance/1-policy/sprints/SPRINT_CURRENT.md`

## ✅ آخر تقرير تحقّق (مصدر حقيقة الحالة)

- **التقرير:** `_bmad-output/governance/1-policy/status/VERIFICATION_REPORT_2026-05-30.md`
- **التاريخ:** 2026-05-30 — **الحالة:** ACTIVE
- **العنوان:** تقرير تحقّق شامل — مطابقة خطط الحوكمة للكود الفعلي

## 📜 القواعد الحوكميّة (GR) — العناوين

- **GR-01: لا ادعاء نسبة إنجاز بلا دليل من الكود الفعليّ (grep/build)**
- **GR-02: لا تَحذف ADRs أبدًا؛ المُلغى `status: Superseded` + `supersededBy`**
- **GR-03: السبرنت لا ينتهي بلا `SPRINT_<date>_RETRO.md`**
- **GR-04: كل ملف زائف يُعلَّم `status: OUT-OF-DATE` فورًا (بلا حذف تاريخيّ)**
- **GR-05: قبل نظام جديد انسخ `_bmad-output/systems/_TEMPLATE/` بالبنية السداسيّة**
- **GR-06: التواريخ من الجهاز فقط — `Get-Date -Format "yyyy-MM-dd"`**

## 🧩 حالة الأنظمة (`systems/`)

| النظام | الحالة |
|--------|--------|
| `builtin-functions` | ACTIVE — المرحلة 1: توليد builtin_registry.h من YAML ✅ (100%) |
| `error-messages` | ACTIVE — 1. القرارات (محسومة) |
| `null-safety` | قيد التنفيذ — NS-01 + NS-02 + NS-04 + NS-03 مُنجَزة ومُتحقَّقة بالبناء والاختبار |
| `testing-system` | الإنجازات حسب القصة |
| `type-system` | القصص المُنفَّذة |
| `ui_system` | ACTIVE — 2. تفصيل الباطن لكل منصة |

---
<!-- نهاية الـDigest — بصمة=1378323475eb721e -->
