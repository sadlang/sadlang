---
title: "كَشف الوكلاء النَشطين (Agent Roster)"
date: 2026-06-01
status: ACTIVE
version: 1.0
owner: PM (John)
---

# 👥 كَشف الوكلاء النَشطين

> 11 وكيل مَطلوب للعمل المتوازي على نظامَي الحوكمة والتوثيق. مُنظَّمون في 3 طبقات.

---

## 🏛️ الطبقة 1 — القيادة (Leadership)

| الوكيل | المهارة | الدور | الأداة الرئيسية |
|---|---|---|---|
| **PM** (John) | [bmad-agent-pm](../../../.github/skills/bmad-agent-pm/SKILL.md) | منسِّق المشروع، يُكلِّف ويُراجع | يَكتب `tasks/inbox/` |
| **Architect** (Winston) | [bmad-agent-architect](../../../.github/skills/bmad-agent-architect/SKILL.md) | مراجَعة معمارية + ADRs | RFC + ADR في `2-architecture/decisions/` |
| **TEA** (Murat) | [bmad-tea](../../../.github/skills/bmad-tea/SKILL.md) | استراتيجية اختبار، NFRs، gates | تَقارير في `2-architecture/status/` |

**العدد:** 3 وكلاء فَرديون.

---

## ⚙️ الطبقة 2 — التَنفيذ (Execution) — 5 وكلاء بنطاقات معزولة

| الوكيل | النَطاق | WIP | المهارة الأولية |
|---|---|---|---|
| **α Alpha** | `shared/` (Lexer, Parser, AST, Types) | 2 | [bmad-quick-dev](../../../.github/skills/bmad-quick-dev/SKILL.md) |
| **β Beta** | `interpreter/`, `vm/`, `runtime/` | 3 | [bmad-quick-dev](../../../.github/skills/bmad-quick-dev/SKILL.md) |
| **γ Gamma** | `compiler/` (SIR, LLVM codegen) | 2 | [bmad-quick-dev](../../../.github/skills/bmad-quick-dev/SKILL.md) |
| **δ Delta** | `stdlib/`, `tools/`, `tests/`, `scripts/` | 4 | [bmad-quick-dev](../../../.github/skills/bmad-quick-dev/SKILL.md) |
| **ε Epsilon** | `graphics/`, `sad_ui/`, `network/`, `platform/`, `cmake/` | 3 | [bmad-quick-dev](../../../.github/skills/bmad-quick-dev/SKILL.md) |

**العدد:** 5 وكلاء فَرديون. **التَوازي الأقصى:** WIP=14 مَهمة في وقت واحد.

> **ملاحظة:** يُمكن تَشغيل **عدة instances من نفس الوكيل** (مثل `δ_1`, `δ_2`, `δ_3`) إذا كانت المهام لا تَتقاطع في الملفات. PM يَفحص التَقاطع قبل التَكليف.

---

## 📚 الطبقة 3 — المساندة (Support)

| الوكيل | المهارة | متى يُستدعى |
|---|---|---|
| **Developer** (Amelia) | [bmad-agent-dev](../../../.github/skills/bmad-agent-dev/SKILL.md) | تَنفيذ ستوريات مُعقَّدة (≥ 8 ساعات) |
| **Tech Writer** (Paige) | [bmad-agent-tech-writer](../../../.github/skills/bmad-agent-tech-writer/SKILL.md) | كل README جديد، توثيق API |
| **Story Creator** | [bmad-create-story](../../../.github/skills/bmad-create-story/SKILL.md) | إنشاء ستوريات M2..M7 من ROADMAP |

**العدد:** 3 وكلاء.

---

## 📊 المجموع

**11 وكيل** عبر 3 طبقات. الذروة المتوازية: **~14 مَهمة في وقت واحد** (مَجموع WIP لوكلاء الطبقة 2).

---

## 🎯 التَكليف الأولي (Sprint #2 / 2026-06-02 → 2026-06-08)

### المهام الأولى (10 مَهام مُتوازية)

| Task | الوكيل | المهارة | الستوري/المعلم | الأولوية |
|---|---|---|---|---|
| T-0001 | δ Delta | bmad-quick-dev | S-015b — Migrate Keywords YAML | P0 |
| T-0002 | δ Delta | bmad-quick-dev | S-015a — Migrate Builtins YAML | P0 |
| T-0003 | δ Delta | bmad-quick-dev | **M0** — `scripts/agent_orchestrator.ps1` | P0 |
| T-0004 | δ Delta | bmad-quick-dev | **M0** — `scripts/agent_lock.py` | P0 |
| T-0005 | δ Delta | bmad-quick-dev | **M0** — `scripts/scan_layers.py` | P1 |
| T-0006 | α Alpha | bmad-quick-dev | S-000a — Foundation Schemas | P0 |
| T-0007 | α Alpha | bmad-quick-dev | S-001 — Loader PoC | P0 |
| T-0008 | Winston | bmad-agent-architect | مراجَعة YAML Unified Schema | P1 |
| T-0009 | Murat | bmad-tea | استراتيجية اختبار M1 | P1 |
| T-0010 | Paige | bmad-agent-tech-writer | README لكل ستوري في M1 | P2 |

### التَوازي المُمكن

```
بنفس الوقت:
├── δ (4 instances): T-0001, T-0002, T-0003, T-0004
├── α (2 instances): T-0006, T-0007
├── Winston: T-0008
├── Murat: T-0009
└── Paige: T-0010
                    ───────────────────────
الإجمالي: 9 مَهام مُتوازية في يوم واحد
```

**T-0005 (scan_layers)** مُؤجل ليوم 2 (يَعتمد على T-0003).

---

## 🔐 القيود

### وكلاء بتَخصص محدود

- **Alpha (α)** لا يَلمس `compiler/` (نطاق Gamma)
- **Beta (β)** لا يَلمس `shared/types/` (نطاق Alpha — هو يَنشئ الأنواع، Beta يَستهلكها)
- **Delta (δ)** لا يَلمس `compiler/include/frontend/` (نطاق Gamma)
- **Epsilon (ε)** لا يَلمس `stdlib/` (نطاق Delta)

### ملفات محروسة (Saleh فقط)

راجع [GUARDED_FILES.md](../../management/GUARDED_FILES.md) — هذه الملفات تَتطلب PR من Saleh ولا تُكلَّف للوكلاء:

- `shared/lexer/src/lexer_keywords.cpp` (قائمة الكلمات الـ40 المحجوزة)
- `_bmad/bmm/config.yaml`
- `.github/copilot-instructions.md` (للتَعديلات الكبيرة)

---

## 📡 آلية الاستدعاء

كل وكيل في Copilot Chat يُستدعى بـ:
```
/runSubagent agentName="<name>" prompt="اقرأ tasks/inbox/T-XXXX*.md ونفِّذ"
```

أو يَدوياً من سطر الأوامر (للوكلاء البشريين):
```powershell
cat _bmad-output/governance/2-agents/tasks/inbox/T-0001-delta-keywords-yaml.md
```

---

## 🔗 مَراجع

- [AGENT_ORCHESTRATION.md](AGENT_ORCHESTRATION.md) — البروتوكول الكامل
- [README.md](README.md) — مَواثيق الوكلاء α β γ δ ε
- [tasks/index.yaml](tasks/index.yaml) — فهرس المهام
- [LIVING_DOCS_ROADMAP.md](../../systems/living-documentation/LIVING_DOCS_ROADMAP.md) — مصدر ستوريات M1..M7

---

**التَحديث:** كلما تَم تَكليف وكيل جديد أو إضافة مهارة، يَنبغي تَحديث هذا الكَشف.
