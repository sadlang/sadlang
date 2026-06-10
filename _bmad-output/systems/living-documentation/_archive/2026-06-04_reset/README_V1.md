---
title: "نظام التوثيق الحي (Living Documentation System)"
type: parent-system
date: 2026-06-02
status: ACTIVE
structure_version: 2.0
restructured: 2026-06-02
adr: ADR-DOCS-CANONICAL-2026-06-02
previous_structure: "3 layers (1-strategy + 2-architecture + 3-implementation) — \u0623\u064f\u0631\u0634\u0641 \u0641\u064a _archive/"
---

# 📚 نظام التوثيق الحي للغة ص

> نظام مُوحَّد لِتَوليد ونَشر توثيق لغة ص تلقائياً من مصدر حقيقة واحد (YAML SoT).

---

## ✨ البنية الجديدة (2026-06-02) — 3 وثائق قانونية فقط

| # | الوثيقة | الإجابة عن |
|---|---|---|
| 1️⃣ | **[STRATEGY.md](STRATEGY.md)** | لماذا نَبنيه؟ ما الرؤية والأهداف والمخاطر؟ |
| 2️⃣ | **[ARCHITECTURE.md](ARCHITECTURE.md)** | كيف نُصمم البنية؟ مصدر الحقيقة، sadinfo، التَدفُّق، Quality Gates |
| 3️⃣ | **[IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md)** | متى وكيف نُنفِّذ؟ المعالم M1–M7، الستوريات، الاعتمادات |

**+** الستوريات النَشطة في [`stories/`](stories/) (36 ستوري موَزَّعة على `architecture/` و `implementation/`)

---

## 🎯 الهدف

تَحويل بيانات اللغة (40 كلمة محجوزة + 25 سياقية + 21 دالة مدمجة + رسائل أخطاء + توجيهات `@`) من **runtime memory مُغلقة** إلى **YAML/JSON قابل للقراءة** يَستهلكه:

- 🌐 **الموقع** (`website/`) — صفحات مَرجعية تلقائية
- 🧠 **LSP** (`tools/lsp/`) — autocompletion + hover docs
- 🎨 **VS Code Extension** — syntax highlighting + snippets
- 🤖 **الذكاء الاصطناعي** — تَدريب على API مُحدَّث
- ✅ **CI** — اختبارات تَطابق التوثيق مع الكود

---

## 📂 بنية المجلد

```
living-documentation/
├── README.md                     ← أنت هنا (نَظرة عامة فقط)
├── STRATEGY.md                   ← ✅ المُعتمَد — الاستراتيجية
├── ARCHITECTURE.md               ← ✅ المُعتمَد — المعمارية
├── IMPLEMENTATION_PLAN.md        ← ✅ المُعتمَد — خطة التَنفيذ
│
├── stories/                      ← الستوريات النَشطة
│   ├── README.md
│   ├── architecture/             ← 12 ستوري معمارية (story-N.N-*)
│   └── implementation/           ← 24 ستوري تَنفيذية (S-NNN-*)
│
└── _archive/                     ← 🗄️ محتوى تاريخي للرجوع فقط (NOT SoT)
    ├── README.md                 ← يَشرح الأرشيف
    ├── UNIFIED_DOCS_ARCHITECTURE.md  ← SUPERSEDED
    ├── LIVING_DOCS_ROADMAP.md        ← SUPERSEDED
    ├── 1-strategy/               ← الطبقة الاستراتيجية القديمة
    ├── 2-architecture/           ← الطبقة المعمارية القديمة (تَحوي ADRs نَشطة)
    └── 3-implementation/         ← طبقة التَنفيذ القديمة
```

---

## 🚦 قواعد الحوكمة (GR-DOCS-CANONICAL)

1. **3 وثائق فقط في الجذر** هي المصدر المُعتمَد: `STRATEGY` + `ARCHITECTURE` + `IMPLEMENTATION_PLAN`
2. **أي معلومة جديدة** تَدخل واحدة من الثلاث — لا تُنشأ وثائق مَوازية في الجذر
3. **الستوريات النَشطة** فقط في [`stories/`](stories/)
4. **ADRs** تَبقى في [`_archive/2-architecture/decisions/`](_archive/2-architecture/decisions/) ويُشار إليها من [ARCHITECTURE.md §8](ARCHITECTURE.md)
5. **`_archive/`** للرجوع فقط — يُمنع استخدامه كمصدر للقرارات الجديدة
6. **عند إضافة ADR جديد:** يُضاف إلى `_archive/2-architecture/decisions/` + يُسجَّل صف جديد في [ARCHITECTURE.md §8](ARCHITECTURE.md)

---

## 📜 لماذا الإعادة الهيكلة؟

البنية السابقة (3 طبقات منفصلة) أَنتجت:
- 3 README + INDEX + UNIFIED_DOCS_ARCHITECTURE + LIVING_DOCS_ROADMAP → 7+ وثائق تَنظيمية
- ستوريات موَزَّعة عبر مجلدين
- تَكرار للمحتوى الاستراتيجي عبر الطبقات
- صعوبة على المطوِّر في تَحديد "ما المُعتمَد؟"

**القرار (2026-06-02):** تَوحيد إلى **3 وثائق قانونية + stories/ موَّحَّد + _archive/**. التَفاصيل في [ADR-DOCS-CANONICAL-2026-06-02](_archive/2-architecture/decisions/ADR-DOCS-CANONICAL-2026-06-02.md).

---

## 🔗 أنظمة ذات صلة (مستقلة)

| النظام | الموقع | العلاقة |
|---|---|---|
| **error-messages** | [`../error-messages/`](../error-messages/) | يَستخدم schemas من ARCHITECTURE، لكنه نظام مستقل بدورة حياة خاصة |
| **error-recovery** | [`../error-recovery/`](../error-recovery/) | مستقل |
| **type-system** | [`../type-system/`](../type-system/) | مستقل، لكن sadinfo يَستخرج معلومات الأنواع منه |

---

## 🚀 ابدأ من هنا

1. اقرأ [STRATEGY.md](STRATEGY.md) لفَهم **لماذا**
2. اقرأ [ARCHITECTURE.md](ARCHITECTURE.md) لفَهم **كيف نُصمم**
3. اقرأ [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md) لفَهم **متى ومن أين**
4. افتح أول ستوري P0: [stories/implementation/S-000a-foundation-schemas.md](stories/implementation/S-000a-foundation-schemas.md)
