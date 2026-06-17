---
task_id: T-0003
title: "إنشاء scripts/agent_orchestrator.ps1 — أداة CLI لإدارة المهام"
assigned_to: agent_delta
created_by: PM (John)
created_at: 2026-06-01T08:50:00Z
deadline: 2026-06-03
priority: P0
story_id: M0-INFRA
milestone: M0
depends_on: []
guarded_files: []
status: queued
estimated_hours: 6
---

# 📋 T-0003 — إنشاء `scripts/agent_orchestrator.ps1`

## السياق

أداة CLI أساسية لتَفعيل بروتوكول Task-File. حالياً PM يُكلف يَدوياً — هذا غير مُستدام لـ32 ستوري قادمة. هذه المهمة بنية تَحتية لكل ما بعدها.

## المُدخلات (اقرأها أولاً)

1. **البروتوكول الكامل:** [AGENT_ORCHESTRATION.md](../../AGENT_ORCHESTRATION.md) — كل ما تَحتاجه
2. **هيكل المجلدات:** [tasks/README.md](../README.md)
3. **مَثال على فحص حَوكمة موجود:** [bmad-governance-check/scripts/check_governance.ps1](../../../../../.github/skills/bmad-governance-check/scripts/check_governance.ps1) — نَفس النَمط

## المخرجات المطلوبة (DoD)

ملف `scripts/agent_orchestrator.ps1` بـ7 أوامر فرعية:

| الأمر | الوصف | المُخرج |
|---|---|---|
| `list` | عرض كل المهام | جدول |
| `list -Agent <name>` | مَهام وكيل واحد | جدول مُرشَّح |
| `list -Status <s>` | مَهام بحالة مُحددة | جدول مُرشَّح |
| `assign -Agent <a> -Story <s> -Priority <p>` | إنشاء `inbox/T-NNNN-*.md` جديد + تَحديث `index.yaml` | path |
| `status -Agent <name>` | حالة وكيل + WIP الحالي | جدول |
| `gc-locks -TtlHours 4` | حذف أقفال > 4 ساعات | تَقرير |
| `report` | تَوليد Markdown لـ `_bmad-output/governance/1-policy/status/AGENT_STATUS_{date}.md` | path |

### القواعد البرمجية

- استخدم `powershell-yaml` module للقراءة من `index.yaml` (إن لم يَكن مُثبَّتاً، اطلب من PM)
- أكواد الخروج: 0 (نجاح), 1 (خطأ مَستخدم), 2 (خطأ نظام)
- دعم `-Quiet` و `-Format json` كما في `check_governance.ps1`
- BOM-encoded (للدعم العربي في Windows)

## القواعد الحَوكمية

- **CW-05:** لا تَتجاوز 800 سطر — قسِّم إلى `agent_orchestrator.ps1` (الواجهة) + `agent_orchestrator_lib.ps1` (المنطق) إن لزم
- **CW-07:** أسماء دالَّة (`Get-AgentTasks`, `New-TaskAssignment` ...)
- **BF-12:** اكتب اختبار `tests/scripts/test_agent_orchestrator.ps1` يَختبر كل الأوامر السبعة

## كيفية الإبلاغ

نَفس [T-0001](T-0001-delta-keywords-yaml.md) — اقرأ القسم "كيفية الإبلاغ" هناك.

## معايير القبول الإضافية

- [ ] `pwsh -File scripts/agent_orchestrator.ps1 list` يَعمل من جذر المشروع
- [ ] `pwsh -File ... assign -Agent alpha -Story S-001 -Priority P0` يُنشئ ملفاً وكاملاً في `inbox/` ويُحدِّث `index.yaml`
- [ ] الاختبار `tests/scripts/test_agent_orchestrator.ps1` يَنجح بـ100%
- [ ] التَوثيق: README في `scripts/` يَشرح كل أمر بمَثال
