---
title: نظام الحوكمة الموحَّد للغة ص
date: 2026-05-30
status: ACTIVE
scope: _bmad-output/governance/
---

# 🏛️ نظام الحوكمة الموحَّد للغة ص

> **نظام واحد، ثلاث طبقات** — كل طبقة تجيب على سؤال واحد بوضوح.
> هذا ليس ثلاثة منتجات منفصلة — بل **هيكل حوكمي متكامل** تم توحيده من ثلاثة مجلدات سابقة (`agents/`، `codeRolePlan/`، `management/`).

---

## 1) الطبقات الثلاث

| # | الطبقة | السؤال الذي تجيب عليه | الموقع |
|---|---|---|---|
| 1 | ⚖️ **Policy** | ما هي القواعد الحاكمة؟ | [1-policy/](1-policy/) |
| 2 | 👥 **Agents** | من ينفّذ ضمن أي نطاق؟ | [2-agents/](2-agents/) |
| 3 | 🔧 **Code Contract** | كيف نُلزم الكود تقنياً بالقواعد؟ | [3-code-contract/](3-code-contract/) |

---

## 2) التسلسل الصحيح للقراءة (للأعضاء الجدد)

1. **اقرأ** [ARCHITECTURE.md](ARCHITECTURE.md) — فهم العلاقة بين الطبقات الثلاث
2. **اقرأ** [1-policy/PROJECT_MANAGEMENT_FRAMEWORK.md](1-policy/PROJECT_MANAGEMENT_FRAMEWORK.md) — الدستور التشغيلي (PMF v1.9.1)
3. **اقرأ ميثاق وكيلك** في [2-agents/](2-agents/) — نطاقك ومسؤولياتك
4. **ارجع لـ** [3-code-contract/](3-code-contract/) عند كتابة كود يلامس عقداً معمارياً

---

## 3) البنية الموحَّدة داخل كل طبقة (Policy + Code-Contract)

كل من `1-policy/` و `3-code-contract/` يتبع **نفس البنية الست**:

```
<area>/
├── planning/      ← PRD, ARCHITECTURE, ROADMAP, frameworks
├── epics/         ← BACKLOG, EPIC-XXX-*.md
├── stories/       ← STORY-*.md (كل القصص بصيغة موحَّدة)
├── sprints/       ← SPRINT_CURRENT.md + SPRINT_<date>_RETRO.md
├── status/        ← implementation_status, VERIFICATION_REPORTs
└── decisions/     ← ADR-XXX-*.md
```

**القاعدة:** لا يجوز خلط أنواع الملفات. كل ملف يذهب لمجلده الصحيح. كل مجلد يحوي `README.md` يفسّر القاعدة.

## 4) نقاط الدخول السريعة

| تحتاج… | الموقع |
|---|---|
| ستوري لتأخذها؟ | [1-policy/epics/BACKLOG.md](1-policy/epics/BACKLOG.md) |
| السبرنت الحالي؟ | [1-policy/sprints/SPRINT_CURRENT.md](1-policy/sprints/SPRINT_CURRENT.md) |
| خارطة الطريق؟ | [1-policy/planning/ROADMAP.md](1-policy/planning/ROADMAP.md) |
| PRD؟ | [1-policy/planning/PRD.md](1-policy/planning/PRD.md) |
| إطار PMF؟ | [1-policy/planning/PROJECT_MANAGEMENT_FRAMEWORK.md](1-policy/planning/PROJECT_MANAGEMENT_FRAMEWORK.md) |
| عتبات رقمية؟ | [1-policy/planning/THRESHOLDS.json](1-policy/planning/THRESHOLDS.json) |
| تقرير التحقق الحالي؟ | [1-policy/status/VERIFICATION_REPORT_2026-05-30.md](1-policy/status/VERIFICATION_REPORT_2026-05-30.md) |
| Contract-as-Code خطة؟ | [3-code-contract/planning/contract-as-code-plan.md](3-code-contract/planning/contract-as-code-plan.md) |
| Contract-as-Code epics؟ | [3-code-contract/epics/epics.md](3-code-contract/epics/epics.md) |
| ميثاق وكيلك؟ | `2-agents/agent_<اسمك>.md` |
| قواعد كتابة C++؟ | [../../.github/copilot-instructions.md](../../.github/copilot-instructions.md) |

---

## 5) ملاحظات حرجة

- **`BACKLOG.md` الوحيد المُعتمد** هو [1-policy/epics/BACKLOG.md](1-policy/epics/BACKLOG.md).
- **ملفات حالة وقت التشغيل** (`AGENT_LOCK.json`, `AUDIT_LOG.jsonl`, `EMERGENCY_OVERRIDES.jsonl`) في [1-policy/runtime-state/](1-policy/runtime-state/) — منفصلة عن الوثائق الدستورية.
- **المراجعات العدائية** في [1-policy/critiques/](1-policy/critiques/).
- **🔴 تنبيه:** [3-code-contract/status/implementation_status.md](3-code-contract/status/implementation_status.md) **OUT-OF-DATE** منذ 2026-05-30 — يدَّعي 156 ثابتاً غير موجود فعلياً. لا تعتمد عليه. اعتمد بدلاً منه على [1-policy/status/VERIFICATION_REPORT_2026-05-30.md](1-policy/status/VERIFICATION_REPORT_2026-05-30.md).

---

## 6) سجل التغيير

| التاريخ | التغيير | المرجع |
|---|---|---|
| 2026-05-30 | **التوحيد:** نقل `agents/` + `codeRolePlan/` + `management/` تحت `governance/` | [../REORGANIZATION_PROPOSAL.md](../REORGANIZATION_PROPOSAL.md) |
| 2026-05-30 | **توحيد البنية:** كل من `1-policy/` و `3-code-contract/` يتبع الآن `planning/epics/stories/sprints/status/decisions/`. نُقلت 21 ملفاً إلى المجلدات الصحيحة. أُضيفت شارة `OUT-OF-DATE` على `implementation_status.md` و `STORY_ZERO_REPORT.md`. | [1-policy/status/VERIFICATION_REPORT_2026-05-30.md](1-policy/status/VERIFICATION_REPORT_2026-05-30.md) |

---

**المالك:** Saleh
**المساعد:** Amelia (bmad-agent-dev)
