# SPRINT AUDIT — Reality Reconciliation

**Sprint ID:** AUDIT-2026-06  
**التاريخ:** 2026-06-01 → 2026-06-05 (4 أيام)  
**الأولوية:** P0 🔴🔴 (حَرج للجَميع — يَسبق كل سبرنت تَنفيذي آخر)  
**السبب:** [خطأ T-0001 المُسجَّل](_archive/2026-06-01_atomic-tasks-model/WHY_ARCHIVED.md) — كَلَّفنا بإنشاء `keywords.yaml` بينما كان مُنفَّذاً منذ قَبل. نَخشى وجود **مفاجآت كَثيرة مثلها**.

---

## §1 الهدف

> **مقارنة كل خُطة/PRD/سبرنت في `_bmad-output/` مع الكود الفعلي في `compiler/`, `interpreter/`, `stdlib/`, `tools/`, `scripts/`, `data/`، وكشف كل التَناقضات.**

النَتيجة: تقرير `REALITY_GAP_2026-06-05.md` يَحوي 3 قَوائم:
1. ✅ **مُنفَّذ ومُوثَّق صحيحاً** — لا عمل
2. 🎁 **مُنفَّذ لكن غير مُوثَّق** ("مفاجآت") — يَحتاج تَوثيق فقط
3. ❌ **مُوثَّق لكن غير مُنفَّذ** (ادعاءات كاذبة) — يَحتاج إصلاح OUT-OF-DATE

---

## §2 الستوريات (4 ستوريات متَوازية على zones مستقلة)

### S-AUDIT-01: فحص النَواة
**Dev Instance:** `dev_audit_kernel`  
**Zone للقراءة فقط:** `compiler/`, `interpreter/`, `vm/`, `shared/`, `runtime/`  
**Zone للكتابة:** `_bmad-output/governance/3-code-contract/status/REALITY_GAP_KERNEL.md`

**المُسلَّمات:**
- جدول لكل ميزة مُدَّعى تَنفيذها في `prd.md` (عقد الكود): ✅/❌/🎁
- قائمة بكل ملف في `compiler/` و `interpreter/` غير مَذكور في أي PRD
- قائمة بكل دالة عامة (`export`) في headers غير مُوثَّقة
- فحص: هل الكلمات المفتاحية الـ40 المُحجوزة موجودة فعلاً في `shared/lexer/src/lexer_keywords.cpp`؟

**معيار القَبول:** التقرير يُغطي ≥ 95% من الملفات في الـzone.

### S-AUDIT-02: فحص المكتبة القياسية
**Dev Instance:** `dev_audit_stdlib`  
**Zone للقراءة فقط:** `stdlib/`, `data/language/`, `data/stdlib/`, `data/_schemas/`, `data/_meta/`  
**Zone للكتابة:** `_bmad-output/governance/3-code-contract/status/REALITY_GAP_STDLIB.md`

**المُسلَّمات:**
- مقارنة `data/language/keywords.yaml` مع `shared/lexer/src/lexer_keywords.cpp` (هل مُتطابقان؟)
- مقارنة `data/stdlib/*.yaml` مع الكود الفعلي في `stdlib/`
- قائمة بكل دالة في `stdlib/` غير مُسجَّلة في `data/stdlib/`
- قائمة بكل entry في `data/` غير مُنفَّذ
- **خاص:** التَحقق من ادعاءات RETRO السابق (B-002, B-009, B-010) عن مفقودات

### S-AUDIT-03: فحص الأدوات والسكربتات
**Dev Instance:** `dev_audit_tools`  
**Zone للقراءة فقط:** `tools/`, `scripts/`, `cmake/`  
**Zone للكتابة:** `_bmad-output/governance/3-code-contract/status/REALITY_GAP_TOOLS.md`

**المُسلَّمات:**
- مسح كل سكربت في `scripts/` (Python, PowerShell) → ما يَفعله، ومَن يَستخدمه
- فحص `tools/lsp/`, `tools/formatter/`, `tools/pkg/`, `tools/repl/`, `tools/compiler/` — حالة كل أداة
- قائمة بسكربتات `_*.py` و `_*.ps1` في الجذر (هل مُهملة؟)
- ادعاءات `scripts/scan_layers.py`, `scripts/agent_lock.py`, `scripts/agent_orchestrator.ps1` — مفقودة فعلاً؟

### S-AUDIT-04: فحص الحوكمة نَفسها (Self-Audit)
**Dev Instance:** `dev_audit_governance`  
**Zone للقراءة فقط:** `_bmad-output/governance/`, `_bmad-output/systems/`  
**Zone للكتابة:** `_bmad-output/governance/1-policy/status/REALITY_GAP_GOVERNANCE.md`

**المُسلَّمات:**
- لكل `implementation_status.md` في كل نظام: مقارنة الادعاءات بـTest-Path الفعلي
- قائمة بكل ملف في `governance/` يَحوي ادعاءات نسب إنجاز بدون أدلة
- فحص: كل ADR له `status:` صريح؟
- فحص: كل نظام تَحت `systems/` له بنية الستة المجلدات؟
- جرد كامل لـ`_recovered/`, `_scratch/`, `_proj_pdf/`, `_bmad-output/codeRolePlan/` — هل هذه مَواد فَعالة أم مُهملة؟

---

## §3 Zones (مَنع التَصادم)

| Dev Instance | يَقرأ من | يَكتب في |
|---|---|---|
| dev_audit_kernel | `compiler/`, `interpreter/`, `vm/`, `shared/`, `runtime/` | `governance/3-code-contract/status/REALITY_GAP_KERNEL.md` |
| dev_audit_stdlib | `stdlib/`, `data/` | `governance/3-code-contract/status/REALITY_GAP_STDLIB.md` |
| dev_audit_tools | `tools/`, `scripts/`, `cmake/` | `governance/3-code-contract/status/REALITY_GAP_TOOLS.md` |
| dev_audit_governance | `_bmad-output/` | `governance/1-policy/status/REALITY_GAP_GOVERNANCE.md` |

**جميع dev_audit_* للقراءة فقط** من zones الكود (لا تَعديل). تَكتب فقط تقارير في status/.

---

## §4 ADRs المرجعية

- لا يَحتاج ADR — هذا سبرنت قراءة وتَحليل فقط.

---

## §5 Test Strategy

- لا اختبارات كود (read-only audit)
- مَعيار الجَودة: كل ادعاء في التقرير يَحوي `file:line` كَدليل

---

## §6 Deadline + WIP

- **Deadline:** 2026-06-05 (4 أيام)
- **WIP:** 4 dev_audit_* instances **بالتَوازي** (zones منفصلة تماماً)
- **يَوم 1-2:** كل instance يَمسح ويُحلِّل
- **يَوم 3:** كل instance يَكتب تقريره
- **يَوم 4:** PM يَدمج التقارير الأربعة في `REALITY_GAP_2026-06-05.md` ويُحدِّث `VERIFICATION_REPORT_2026-06-05.md`

---

## §7 Definition of Done

- [ ] 4 ملفات `REALITY_GAP_*.md` مَكتوبة بأدلة file:line
- [ ] PM دمج → `_bmad-output/governance/1-policy/status/REALITY_GAP_2026-06-05.md`
- [ ] PM كَتب `VERIFICATION_REPORT_2026-06-05.md` بـ%إنجاز حقيقي (يَنسخ القديم 52%)
- [ ] كل ملف يُكتشف أنه "مُوثَّق لكن غير مُنفَّذ" يُعلَّم `status: OUT-OF-DATE` (GR-04)
- [ ] قائمة "مفاجآت" 🎁 تُمرَّر لـPaige (Tech Writer) كسبرنت تَوثيقي لاحق
- [ ] SPRINT_AUDIT_RETRO.md

---

## §8 لماذا هذا السبرنت أهم من M0-INFRA؟

| البُعد | M0-INFRA | AUDIT |
|---|---|---|
| المُخرج | كود + سكربتات جديدة | **معرفة حقيقية بالواقع** |
| الخَطر دون تَنفيذه | بنية تَحتية أَخف | **كل سبرنت لاحق يُبنى على وَهم** |
| التَوازي | 1 instance | 4 instances |
| المُدة | 9 أيام | 4 أيام |
| التَأثير على بقية الخُطة | اختياري | **يُعيد ترتيب أولويات كل M0..M7** |

**قرار PM:** نُؤجِّل M0-INFRA حتى يَكتمل AUDIT. السبب: قد يَكتشف AUDIT أن جزءاً من M0 مُنفَّذ فعلاً.

---

## §9 المُتطَلبات قبل البَدء

- [x] هذا الملف مَكتوب
- [ ] تَسجيل السبرنت في [sprints_active.yaml](../sprints_active.yaml)
- [ ] مُوافَقة المالك على إطلاق 4 instances متوازية

---

## §10 سجل التَنفيذ (يُحدَّث من dev_audit_*)

| التاريخ | Instance | الستوري | الحالة | ملاحظة |
|---|---|---|---|---|
| - | - | - | - | لم يَبدأ بعد |
