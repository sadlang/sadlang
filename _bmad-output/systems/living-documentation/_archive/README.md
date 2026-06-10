---
title: "أرشيف نظام التوثيق الحي — للرجوع التاريخي فقط"
type: archive-notice
date: 2026-06-02
status: ARCHIVED-REFERENCE
authority: NOT-A-SOURCE-OF-TRUTH
related_canonical:
  - ../STRATEGY.md
  - ../ARCHITECTURE.md
  - ../IMPLEMENTATION_PLAN.md
governance_rule: GR-DOCS-CANONICAL
---

# 🗄️ أرشيف نظام التوثيق الحي

> ⚠️ **تَنبيه:** هذا المجلد يَحوي **محتوى تاريخي للرجوع فقط**. لا يَجوز استخدامه كمصدر للقرارات الجديدة.

## ما هو المُعتمَد الآن

| السؤال | الوثيقة المُعتمَدة |
|---|---|
| ماذا نَبني ولماذا؟ | [../STRATEGY.md](../STRATEGY.md) |
| كيف نُصمم البنية؟ | [../ARCHITECTURE.md](../ARCHITECTURE.md) |
| كيف نُنفِّذ ومتى؟ | [../IMPLEMENTATION_PLAN.md](../IMPLEMENTATION_PLAN.md) |
| الستوريات النَشطة؟ | [../stories/](../stories/) |

## محتوى الأرشيف

```
_archive/
├── 1-strategy/        ← كانت الطبقة الاستراتيجية (PRD v2 + Architecture v2 + UX + Testing)
├── 2-architecture/    ← كانت الطبقة المعمارية (planning + epics + ADRs + sprints + status)
│                       (ستورياتها نُقلت إلى ../stories/architecture/)
└── 3-implementation/  ← كانت طبقة التَنفيذ (planning + epics + prerequisites + sprints + status)
                        (ستورياتها نُقلت إلى ../stories/implementation/)
```

## القرارات المعمارية (ADRs)

ADRs الأصلية باقية في [`2-architecture/decisions/`](2-architecture/decisions/). كل ADR لا يَزال **نافذاً** لأن GR-02 تَمنع حذف ADRs. الوثيقة [../ARCHITECTURE.md §8](../ARCHITECTURE.md) تَستهلكها بِالمرجع.

## لماذا الأرشفة؟

البنية الثلاثية الطبقات (`1-strategy/ + 2-architecture/ + 3-implementation/`) أَنتجت:
- 3 README + INDEX + UNIFIED_DOCS_ARCHITECTURE + LIVING_DOCS_ROADMAP
- عشرات الستوريات موَزَّعة عبر مجلدات متَداخلة
- تَكرار للمحتوى الاستراتيجي عبر الطبقات
- صعوبة على المطوِّر في تَحديد "ما المُعتمَد؟"

**القرار (2026-06-02):** تَوحيد المحتوى في **3 وثائق قانونية في الجذر** + `stories/` نَشط. التَفاصيل في الـ ADR الجديد.

## قواعد التَعامل مع الأرشيف

1. ✅ يَجوز قراءة الملفات للرجوع التاريخي
2. ❌ لا يَجوز تَعديل ملفات هنا (إلا لتَصحيح روابط مَكسورة)
3. ❌ لا يَجوز الإشارة إليها كـ "المصدر المُعتمَد"
4. ✅ يَجوز نَقل ADR من هنا إذا ظَهرت حاجة فعلية لإعادة تَفعيله (مَع تَوثيق السبب)
