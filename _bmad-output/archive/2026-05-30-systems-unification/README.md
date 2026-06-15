---
title: "أرشيف توحيد الأنظمة — 2026-05-30"
date: 2026-05-30
type: migration-log
status: completed
performedBy: Amelia (Senior Software Engineer)
relatedDocuments:
  - ../../SYSTEMS_ARCHITECTURE_PROPOSAL.md
  - ../../systems/README.md
  - ../../STATUS.md
purpose: |
  سجل تاريخي كامل لعملية توحيد أنظمة لغة ص تحت `_bmad-output/systems/`.
  يُحفظ هنا لتتبّع التحوّلات وتفسير الروابط القديمة إن وُجدت في تاريخ git.
---

# أرشيف توحيد الأنظمة — 2026-05-30

## ملخص

نُفِّذت أربع مراحل من [SYSTEMS_ARCHITECTURE_PROPOSAL.md](../../SYSTEMS_ARCHITECTURE_PROPOSAL.md)
في يوم واحد بقيادة Amelia.

| المرحلة | الإجراء | الحالة |
|---|---|---|
| Phase 1 | إنشاء `systems/` + `_TEMPLATE/` + `README.md` فهرس + حذف `eroor_system/` | ✅ |
| Phase 2 | نقل `error_system/` → `systems/error-messages/` + تحديث الروابط | ✅ |
| Phase 3 | نقل `type_system/` + `docplan/` + `doc_plan/` + `planning-artifacts/sadinfo/` كما هي | ✅ |
| Phase 4 | تحديث `STATUS.md` + `README.md` + إنشاء هذا السجل | ✅ |

## خريطة النقل التفصيلية

| المصدر القديم | الوجهة الجديدة | عدد الملفات | ملاحظات |
|---|---|---|---|
| `_bmad-output/error_system/` | `_bmad-output/systems/error-messages/` | 11 | نقل + تحديث الروابط في README/STATUS |
| `_bmad-output/eroor_system/` | 🗑️ سلة المهملات | 1 | مكرر مُتحقَّق منه بـ SHA-256 = `899AA071F44D61330B4B2FDFE0EF3F3CCB1B7CECD2804C08561F0AC73E205B8B` |
| `_bmad-output/type_system/` | `_bmad-output/systems/type-system/` | 12 | نقل كما هو (UTM v3) |
| `_bmad-output/docplan/` | `_bmad-output/systems/doc-ir/` | 28 | نقل كما هو (DocIR + Renderers، ADR-006b) |
| `_bmad-output/doc_plan/` | `_bmad-output/systems/doc-plan-v2/` | 14 | نقل كما هو (Diátaxis، لا دمج — مؤجَّل) |
| `_bmad-output/planning-artifacts/sadinfo/` | `_bmad-output/systems/sadinfo/` | 30 | نقل كما هو (Sprint 2) |
| **المجموع** | **95 ملف** | + 6 ملفات قالب جديدة في `_TEMPLATE/` |

## سياسة عدم الدمج (Deferred Merge)

`doc_plan/` و `docplan/` نُقلا منفصلَين بقرار صريح من المستخدم (2026-05-30):

> "لا تدمج ملفات التوثيق docplan, doc_plan لأنه بحاجة إلى جلسة منفردة مع تفاصيل دقيقة."

السبب: هما طبقتان مكمّلتان (تنفيذ IR vs تخطيط منتج/UX) وليستا تكراراً.
الدمج المستقبلي يتطلب جلسة مخصَّصة مع PM + Architect + UX Designer + TEA.
انظر [SYSTEMS_ARCHITECTURE_PROPOSAL.md §4.5](../../SYSTEMS_ARCHITECTURE_PROPOSAL.md).

## ملفات تأثرت بتحديث الروابط

| الملف | عدد الروابط المُحدَّثة |
|---|---|
| `_bmad-output/README.md` | ~15 (جدول الجذر + قسم 2.5 + قسم 2.6 + قسم 2.12 + مخطط Mermaid) |
| `_bmad-output/STATUS.md` | ~5 (relatedDocuments + قسم 1.3 + قسم 1.5 + callout) |
| `data/_schemas/README.md` | 2 |
| `docs/sadinfo_v2/README.md` | 3 |
| `docs/sadinfo_v2/BASELINE.md` | 2 |

## التحقق النهائي

- ✅ `Test-Path _bmad-output/{eroor_system,error_system,type_system,docplan,doc_plan,planning-artifacts/sadinfo}` = **6× False**.
- ✅ `Get-ChildItem _bmad-output/systems -Directory | Measure-Object` = **6** (_TEMPLATE + 5 أنظمة).
- ✅ بحث شامل عن الروابط القديمة في الـworkspace = **0 رابط مكسور**.
- ✅ النصوص التاريخية في `discovery/` و `governance/PROJECT_MANAGEMENT_FRAMEWORK.md` و
  `SYSTEMS_ARCHITECTURE_PROPOSAL.md` لم تُمَس (محفوظة كسياق تاريخي).

## استرداد (Rollback) — في حالة الطوارئ

```powershell
# استعادة eroor_system/ من سلة المهملات (تتطلب أداة Recycle Bin restore)
# استعادة باقي المجلدات عبر git (إن كانت متتبَّعة) أو من backup

# لـ git untracked: استعادة بـ Move-Item عكسي
Move-Item _bmad-output/systems/error-messages _bmad-output/error_system
Move-Item _bmad-output/systems/type-system    _bmad-output/type_system
Move-Item _bmad-output/systems/doc-ir         _bmad-output/docplan
Move-Item _bmad-output/systems/doc-plan-v2    _bmad-output/doc_plan
Move-Item _bmad-output/systems/sadinfo        _bmad-output/planning-artifacts/sadinfo
# ثم استرجاع نسخة git السابقة لـ README.md + STATUS.md + 3 ملفات في data/ + docs/sadinfo_v2/
```
