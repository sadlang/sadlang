---
title: "ADR-DOCS-CANONICAL: تَوحيد نظام التوثيق الحي إلى 3 وثائق قانونية"
id: ADR-DOCS-CANONICAL-2026-06-02
date: 2026-06-02
status: Accepted
supersedes:
  - ADR-RESTRUCTURE-2026-06-01 (جزئياً — البنية الثلاثية الطبقات)
deciders:
  - صلاح (Owner)
  - Claude Code Agent (تَنفيذ)
context_files:
  - ../../README.md
  - ../../STRATEGY.md
  - ../../ARCHITECTURE.md
  - ../../IMPLEMENTATION_PLAN.md
governance_rule_introduced: GR-DOCS-CANONICAL
---

# ADR-DOCS-CANONICAL: تَوحيد نظام التوثيق الحي إلى 3 وثائق قانونية

## 1. السياق

في 2026-06-01، طُبِّق ADR-RESTRUCTURE-2026-06-01 الذي وَحَّد 3 أنظمة منفصلة (`doc-plan-v2`, `doc-ir`, `sadinfo`) تحت نظام-أب واحد `living-documentation/` ذي **ثلاث طبقات**:
- `1-strategy/` (9 مجلدات فرعية مرقَّمة)
- `2-architecture/` (البنية الستة كاملة)
- `3-implementation/` (البنية الستة + prerequisites)

**النتيجة العملية بعد 1 يوم:** المطوِّر يَتوه بين:
- 3 ملفات README
- INDEX.md في 1-strategy
- UNIFIED_DOCS_ARCHITECTURE.md (تَحليل لماذا 3 طبقات)
- LIVING_DOCS_ROADMAP.md (خَريطة طريق عبر الطبقات)
- ستوريات موَزَّعة على مجلدين (`2-architecture/stories/` + `3-implementation/stories/`)
- planning/ في كل طبقة (3 مرات!)
- ADRs في `2-architecture/decisions/` فقط لكن المرجع من كل الطبقات

**عدد الوثائق التَنظيمية = 7+**، والمطوِّر لا يَعرف أيهم المُعتمَد للقرار.

## 2. القرار

**تَوحيد النظام إلى 3 وثائق قانونية في الجذر + مجلد ستوريات نَشط واحد + أرشيف:**

```
living-documentation/
├── README.md
├── STRATEGY.md             ← الوثيقة الاستراتيجية الوحيدة
├── ARCHITECTURE.md         ← الوثيقة المعمارية الوحيدة
├── IMPLEMENTATION_PLAN.md  ← خطة التَنفيذ الوحيدة
├── stories/
│   ├── architecture/       ← ستوريات معمارية نَشطة
│   └── implementation/     ← ستوريات تَنفيذية نَشطة
└── _archive/               ← المحتوى التاريخي (NOT SoT)
    ├── 1-strategy/
    ├── 2-architecture/
    │   └── decisions/      ← ADRs نَشطة (مَرجعية من ARCHITECTURE.md §8)
    ├── 3-implementation/
    ├── UNIFIED_DOCS_ARCHITECTURE.md   (SUPERSEDED)
    └── LIVING_DOCS_ROADMAP.md         (SUPERSEDED)
```

## 3. الأسباب

1. **مبدأ "المصدر الواحد" (Single Source of Truth):** المطوِّر يَفتح وثيقة واحدة لكل سؤال، لا 3 طبقات
2. **تَقليل الكَوغنيتف لود:** 3 وثائق + ستوريات = أبسط بكثير من 7 وثائق تَنظيمية + طبقتين متَداخلتين
3. **سرعة الـonboarding:** المطوِّر الجديد يَقرأ STRATEGY → ARCHITECTURE → IMPLEMENTATION_PLAN بالترتيب
4. **مَنع الازدواجية:** كل معلومة لها مكان واحد، لا 3
5. **حماية التَاريخ:** GR-02 (لا حذف ADRs) و GR-04 (لا حذف ملفات) محفوظتان عبر `_archive/`

## 4. البدائل المرفوضة

| البديل | السبب |
|---|---|
| إبقاء البنية الثلاثية الطبقات وتَحسين الـREADMEs | المشكلة الجذرية بنيوية، لا تَحلها README أفضل |
| دمج كل المحتوى في وثيقة واحدة ضخمة | يَخلط بين "لماذا/كيف/متى" — يَصعب التَنقُّل |
| حذف المحتوى القديم بدلاً من أرشفته | يَخرق GR-02 و GR-04 ويَفقد السياق التاريخي |

## 5. التَأثير

### إيجابي
- المطوِّر الجديد يَفهم النظام في < 15 دقيقة
- لا تَكرار للمحتوى الاستراتيجي
- ADRs محفوظة وقابلة للمَرجعية
- مَرجع واحد لكل سؤال

### سلبي / مَخاطر
- روابط قديمة في وثائق أخرى قد تَكسر (مثل copilot-instructions.md) — **تم تَحديثها مع هذا الـADR**
- المطوِّرون المعتادون على البنية القديمة يَحتاجون تَكيُّفاً مرة واحدة

### مَيزات تَنفيذية
- ADRs بَقيت في `_archive/2-architecture/decisions/` بدون نقل (تَجنُّب كسر روابط داخلية)
- الستوريات الـ36 موَّحَّدة في `stories/{architecture,implementation}/` (38 ملف بما في ذلك README)

## 6. قاعدة الحوكمة الجديدة (GR-DOCS-CANONICAL)

> **GR-DOCS-CANONICAL:** نظام `living-documentation/` يَتبع بنية 3 وثائق قانونية في الجذر (`STRATEGY.md` + `ARCHITECTURE.md` + `IMPLEMENTATION_PLAN.md`) + `stories/` موَّحَّد + `_archive/`.
>
> - أي معلومة جديدة تَدخل **واحدة من الثلاث** — لا تُنشأ وثائق مَوازية في الجذر.
> - ADRs جديدة تَدخل [`_archive/2-architecture/decisions/`](../decisions/) ويُسجَّل صف لها في [ARCHITECTURE.md §8](../../ARCHITECTURE.md).
> - الستوريات النَشطة في `stories/` فقط — لا تُنشأ مجلدات stories بديلة.
> - `_archive/` للرجوع التاريخي فقط — **يُمنع** استخدامه كمصدر للقرارات.

## 7. التَحقق

- [x] STRATEGY.md موجود ومُعتمَد
- [x] ARCHITECTURE.md موجود ومُعتمَد
- [x] IMPLEMENTATION_PLAN.md موجود ومُعتمَد
- [x] stories/architecture/ + stories/implementation/ موجودان مَع 36 ستوري
- [x] _archive/ يَحوي 1-strategy + 2-architecture + 3-implementation + الوثيقتان القديمتان مع status=SUPERSEDED
- [x] _archive/README.md يَشرح طبيعة الأرشيف
- [x] `.github/copilot-instructions.md` مُحدَّث ليَعكس البنية الجديدة (نفس ADR)

## 8. التَاريخ

| التاريخ | الحدث |
|---|---|
| 2026-06-01 | تَطبيق ADR-RESTRUCTURE-2026-06-01 (البنية الثلاثية) |
| 2026-06-02 | تَفعيل ADR-DOCS-CANONICAL (هذه الوثيقة) — تَوحيد إلى 3 وثائق |
