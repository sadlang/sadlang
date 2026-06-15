# 📁 مجلد الإدارة — فهرس وخريطة

> **الغرض:** المصدر الموحَّد (Single Source of Truth) لكل ما يخص حوكمة وتنفيذ وتطوير لغة ص.
> **آخر تحديث:** 2026-05-22 — PMF v1.9.1 + إعادة تنظيم المجلد.

هذا المجلد يحتوي وثائق الحوكمة والتنفيذ والمراجعات. الملفات في الجذر تبقى في الجذر لتفادي كسر السكريبتات (`agent_lock.py`, `verify_thresholds_consistency.py`, `scan_layers.py`, `check_arabic_ratio.py`, `thresholds_loader.py`) والـ workflow (`monthly-pmf-check.yml`) التي تعتمد على المسارات الحالية. مجلد `execution/` الجديد يحتوي ملفات طبقة التنفيذ التي أُضيفت مع PMF v1.9.1.

---

## 1) الوثيقة الحاكمة (Constitutional)

> نقطة الدخول الأولى لأي عضو فريق جديد. اقرأها أولاً.

| الملف | الوصف |
|-------|-------|
| [PROJECT_MANAGEMENT_FRAMEWORK.md](PROJECT_MANAGEMENT_FRAMEWORK.md) | إطار الحوكمة الكامل (PMF v1.9.1) — 15 قسماً يغطي القرارات والعتبات والطوارئ وطبقة التنفيذ. |

## 2) طبقة التنفيذ — مجلد فرعي جديد

> الجواب على سؤال "من أين نبدأ يوم الإثنين؟" — يطبّق نظرية القيود (TOC) ومبدأ **Policy ≠ Daily Work**.

| الملف | الوصف |
|-------|-------|
| [execution/ROADMAP.md](execution/ROADMAP.md) | معالم استراتيجية (M1 Beta، M2 Stable، M3 Production). |
| [execution/BACKLOG.md](execution/BACKLOG.md) | كل الستوريات (15 ستوري) مع الأولويات والحالات. |
| [execution/SPRINT_CURRENT.md](execution/SPRINT_CURRENT.md) | الأسبوع النشط — 3 ستوريات كحد أقصى، تُغلق وتُفتح كل جمعة. |

## 3) وثائق المرجع (Reference Specs)

> مواصفات ثابتة نسبياً — تتغيّر فقط عبر مراجعة رسمية.

| الملف | الوصف |
|-------|-------|
| [PRD.md](PRD.md) | متطلبات المنتج (Product Requirements Document). |
| [ARCHITECTURE.md](ARCHITECTURE.md) | البنية المعمارية للمشروع. |
| [LAYERS.json](LAYERS.json) | تعريف طبقات المشروع وقواعد الاعتمادية. |
| [THRESHOLDS.json](THRESHOLDS.json) | العتبات الرقمية الموحَّدة (Single Source of Truth). |
| [EDGE_CASE_GUARDS.md](EDGE_CASE_GUARDS.md) | حماية الحالات الحدّية الموثَّقة. |

## 4) الستوريات المستقلة (Standalone Stories)

> سجلات تفصيلية لستوريات معقدة تستحق وثيقة مستقلة عن BACKLOG.

| الملف | الوصف |
|-------|-------|
| [STORY-PMF-V17-ENFORCE-GPG.md](STORY-PMF-V17-ENFORCE-GPG.md) | ستوري فرض توقيع GPG على الأوامر الحساسة. |
| [STORY_ZERO_REPORT.md](STORY_ZERO_REPORT.md) | تقرير الستوري الصفري — قاعدة بدء التتبع. |

## 5) المراجعات العدائية (Adversarial Reviews)

> سجل دائم لكل نقد عدائي طُبِّق على PMF.

| الملف | الناقد | الناتج |
|-------|--------|--------|
| [CRITIQUE_QUINN_2025-11-21.md](CRITIQUE_QUINN_2025-11-21.md) | د. كوين | تأسيس مبدأ Constraints over Conventions. |
| [CRITIQUE_MURAD_2026-05-22.md](CRITIQUE_MURAD_2026-05-22.md) | مراد | تشديد عتبات v1.6/v1.7. |
| [CRITIQUE_PENTESTER_2026-05-22.md](CRITIQUE_PENTESTER_2026-05-22.md) | Pen-Tester | إصلاح 17 ثغرة في v1.8/v1.9. |
| [CRITIQUE_QUINN_2026-05-22.md](CRITIQUE_QUINN_2026-05-22.md) | د. كوين (الثانية) | تشخيص فجوة التنفيذ ← طبقة التنفيذ v1.9.1. |

## 6) ملفات وقت التشغيل (Runtime State)

> حالة متغيرة باستمرار — تُكتب وتُقرأ آلياً بواسطة السكريبتات.

| الملف | الكاتب | الوصف |
|-------|--------|-------|
| [AGENT_LOCK.json](AGENT_LOCK.json) | `scripts/agent_lock.py` | الأقفال النشطة على الستوريات (TTL). |
| [AUDIT_LOG.jsonl](AUDIT_LOG.jsonl) | السكريبتات + الـ workflow | سجل تدقيق كل عملية حوكمة. |
| [EMERGENCY_OVERRIDES.jsonl](EMERGENCY_OVERRIDES.jsonl) | تسجيل يدوي عبر النائب | تجاوزات الطوارئ مع post-mortem. |

## 7) القوالب (Templates)

| المجلد | الوصف |
|--------|-------|
| [templates/](templates/) | قوالب جاهزة لإنشاء ستوريات ومراجعات جديدة + قوالب بروتوكول الوكلاء v1.2. |

### قوالب بروتوكول الوكلاء v1.2 (2026-05-27)

| القالب | الاستخدام | المؤلف |
|---|---|---|
| [templates/TASK_TEMPLATE.md](templates/TASK_TEMPLATE.md) | تعريف مهمة جديدة في `tasks/active/` | PM فقط |
| [templates/STATE_TEMPLATE.md](templates/STATE_TEMPLATE.md) | تتبّع حي للمهمة + heartbeat ≤ 12h | الوكيل المُكلَّف |
| [templates/ADR_TEMPLATE.md](templates/ADR_TEMPLATE.md) | اقتراح قرار معماري إلزامي | الوكيل (PM يوافق) |
| [templates/DAILY_TEMPLATE.md](templates/DAILY_TEMPLATE.md) | ملف يومي للتنسيق `daily/YYYY-MM-DD.md` | PM + جميع الوكلاء |

## 8) بروتوكول الوكلاء — v1.2 (الجديد، مُعتمد 2026-05-27)

> **الدستور التشغيلي لإدارة ثلاثة وكلاء AI مستقلين تحت إمرة PM.**

| الملف/المجلد | الدور | المالك |
|---|---|---|
| [PM_REPORT_AND_AGENT_PROTOCOL.md](PM_REPORT_AND_AGENT_PROTOCOL.md) | **الدستور** — التقييم + 8 فجوات + سياسات صالح المعتمدة | PM |
| [tasks/active/](tasks/active/) | المهام النشطة (TASK-NNN.md + state.md) | PM يكتب، الوكلاء يحدّثون state فقط |
| [tasks/done/](tasks/done/) | أرشيف المهام المنجزة + Lessons Learned | PM فقط |
| [proposals/](proposals/) | ADRs — قرارات معمارية | الوكلاء يفتحون، PM يقرر |
| [memory-drafts/](memory-drafts/) | مسودات ذاكرة قبل الدمج في `/memories/repo/` | الوكلاء يكتبون، PM يدمج |
| [daily/](daily/) | التنسيق اليومي — مركز التواصل الوحيد | جميع الفريق |

### القرارات السيادية لصالح (2026-05-27)

1. **Git للوكلاء:** مرفوع — كل وكيل push على `agent/<name>/TASK-NNN` (PM وحده يدمج لـ `main`).
2. **CI Auto-Trigger:** معطّل — اختبارات محلية قبل push إلزامية.
3. **Lost Agent:** 48 ساعة بلا heartbeat → PM يعيد التكليف.

---

## 🔗 روابط خارجية مهمة

- [`.github/DEPUTY_OWNER.md`](../../.github/DEPUTY_OWNER.md) — هوية النائب (للقرارات الحساسة).
- [`.github/workflows/monthly-pmf-check.yml`](../../.github/workflows/monthly-pmf-check.yml) — التحقق الشهري الآلي.
- [`scripts/agent_lock.py`](../../scripts/agent_lock.py) — إدارة أقفال الستوريات.
- [`scripts/verify_thresholds_consistency.py`](../../scripts/verify_thresholds_consistency.py) — تحقق اتساق العتبات.

---

## 📜 تاريخ التنظيم

- **في تاريخ الثاني والعشرين من مايو لعام ألفين وستة وعشرين، ومع إصدار النسخة التاسعة من الإطار، تقرر إنشاء مجلد فرعي مخصص لطبقة التنفيذ ونقل ملفات خارطة الطريق والسجل الكامل للستوريات وستوريات الأسبوع الحالي إليه. كما أُضيف هذا الفهرس كخريطة وصفية للمجلد. لم تُنقل بقية الملفات لتفادي كسر السكريبتات والمسارات المرجعية المعتمدة في أدوات التحقق الآلي وسير العمل الشهري.

## 🧭 فلسفة التنظيم

اتبع التنظيم الحالي مبدأ التعطيل الأدنى للتغيير: نقلنا فقط الملفات التي طلبها المالك صراحة، ووضعنا بقية الملفات في فئات منطقية افتراضية يصفها هذا الفهرس دون نقل فعلي. الهدف من ذلك هو الجمع بين وضوح التنظيم للقارئ البشري واستقرار المسارات للسكريبتات الآلية. كل ملف موجود في هذا المجلد يخدم غرضاً واحداً محدداً وفق مبدأ المسؤولية الواحدة، ولا يجوز خلط مهام طبقات مختلفة في ملف واحد. عند الحاجة لإضافة ملف جديد، يجب أولاً تحديد فئته من الفئات السبع المذكورة أعلاه ثم إضافة سطر له في الجدول المناسب من هذا الفهرس مع وصف عربي واضح يبين دوره وعلاقته بالملفات الأخرى.
