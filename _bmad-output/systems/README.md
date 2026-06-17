---
title: "أنظمة لغة ص — الفهرس الموحَّد"
date: 2026-05-30
lastUpdated: 2026-05-31
status: ACTIVE
owner: Amelia (Senior Software Engineer)
purpose: |
  فهرس موحَّد لكل أنظمة لغة ص الفرعية الموجودة تحت `_bmad-output/systems/`.
  يُستبدل به التشتيت السابق بين المجلدات المختلفة (`error_system/`,
  `type_system/`, `docplan/`, `doc_plan/`, `planning-artifacts/sadinfo/` …).
relatedDocuments:
  - ../SYSTEMS_ARCHITECTURE_PROPOSAL.md
  - ../STATUS.md
  - ../README.md
  - _TEMPLATE/README.md
---

# أنظمة لغة ص — الفهرس الموحَّد

> هذا الفهرس مرجع موحَّد لكل نظام فرعي.
> **كل نظام له مجلد مستقل بنفس البنية القياسية الموحَّدة الستة:**
>
> ```
> <system>/
> ├── planning/    ← PRD, ARCHITECTURE, ROADMAP, مواصفات
> ├── epics/       ← BACKLOG + EPIC-XXX-*.md
> ├── stories/     ← STORY-*.md (قصص فردية)
> ├── sprints/     ← SPRINT_CURRENT.md + SPRINT_<date>_RETRO.md
> ├── status/      ← implementation_status.md + VERIFICATION_REPORT_*
> ├── decisions/   ← ADR-XXX-*.md
> └── README.md    ← مرجع النظام
> ```
>
> القالب الكامل في [`_TEMPLATE/`](_TEMPLATE/README.md).
> نفس البنية مطبَّقة على `governance/1-policy/` و `governance/3-code-contract/`.

## جدول الأنظمة

| # | النظام | المجلد | الغرض | الحالة |
|---|---|---|---|---|
| 1 | رسائل الأخطاء | [`error-messages/`](error-messages/) | منظومة رسائل أخطاء اللغة (E000–E999) | 🟢 ACTIVE |
| 2 | نظام الأنواع | [`type-system/`](type-system/) | محرك الاستدلال + الفحص الثابت + قواعد التحويل | 🟢 ACTIVE |
| 3 | بنية وثائق IR | [`doc-ir/`](doc-ir/) | خطة توثيق طبقات IR (SIR/LLVM/Codegen) | 🟢 ACTIVE |
| 4 | بنية وثائق v2 | [`doc-plan-v2/`](doc-plan-v2/) | خطة توثيق متكاملة (PRD/UX/Architecture/Stories) — طبقة منتج 📚 مرجع | 📚 REFERENCE |
| 5 | معلومات اللغة | [`sadinfo/`](sadinfo/) | أداة `sad-info` لاستعراض الكلمات المفتاحية + الدوال المضمنة | 🟢 ACTIVE |
| 6 | استرداد الأخطاء | [`error-recovery/`](error-recovery/) | استراتيجيات استرداد parser + suggestions | 🟡 IN-DEVELOPMENT |
| 7 | نظام الاختبارات | [`testing-system/`](testing-system/) | توحيد اختبارات اللغة في 3 طبقات (وحدة/سلوك/نظام) + تغطية لكل أقسام اللغة | 🟢 ACTIVE |

## كيفية إضافة نظام جديد

1. انسخ `_TEMPLATE/` إلى `<system-kebab-case>/`.
2. عبّئ `planning/prd.md` + `planning/architecture.md` + `README.md` للنظام.
3. أضف ملاحم في `epics/` وقصص في `stories/`.
4. أضف صف جديد إلى جدول الأنظمة أعلاه.
5. حدّث `../STATUS.md` للإشارة إلى النظام الجديد.

## مبادئ التنظيم

- **مجلد واحد لكل نظام** — لا تشتيت، لا تكرار.
- **بنية قياسية ستة مجلدات داخلية** — كل نظام يتبع `_TEMPLATE/` بالضبط.
- **القرارات بـ ADRs** — كل قرار جوهري في `<system>/decisions/ADR-NNN-*.md`.
- **القصص في `stories/`** — مع ربط واضح بـ AC.
- **التخطيط في `planning/`** — PRD/Architecture/Roadmap لا تُخلط مع التنفيذ.
- **الحالة في `status/`** — `implementation_status.md` + تقارير تحقق دورية.
- **استثناء `doc-plan-v2/`**: يستخدم بنية مرقَّمة قديمة (`01_prd/` … `08_implementation_artifacts/`) ويُترك كما هو لأنه مرجع تاريخي مستقر؛ لا تُطبَّق عليه إعادة الهيكلة لتجنب كسر الروابط الداخلية الكثيرة.

## سجل التحديثات

| التاريخ | التغيير | المرجع |
|---|---|---|
| 2026-05-30 | إنشاء الفهرس الموحَّد + بداية المرحلة 1 من التوحيد | [`SYSTEMS_ARCHITECTURE_PROPOSAL.md`](../SYSTEMS_ARCHITECTURE_PROPOSAL.md) |
| 2026-05-31 | **توحيد البنية الداخلية لكل نظام**: إضافة `planning/`, `epics/`, `stories/`, `sprints/`, `status/`, `decisions/` داخل كل من `_TEMPLATE/`, `doc-ir/`, `type-system/`, `sadinfo/`, `error-messages/`, `error-recovery/`. نُقل 31 ملفاً إلى مواضعها الصحيحة + 42 README فرعي. مطابق لما طُبِّق على `governance/`. | هذا الإصدار |
| 2026-06-11 | إنشاء نظام **testing-system** (PROPOSAL) — توثيق التصميم الكامل ببنية الستة المجلدات: README + PRD + Architecture + 3 ADRs + BACKLOG + 6 stories (TEST-001..006). | [`testing-system/`](testing-system/) |
