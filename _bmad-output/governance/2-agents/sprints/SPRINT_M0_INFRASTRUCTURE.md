# SPRINT M0 — البنية التَحتية للحوكمة

**Sprint ID:** M0-INFRA  
**التاريخ:** 2026-06-01 → 2026-06-10 (9 أيام)  
**Dev Instance:** `dev_M0`  
**الأولوية:** P0 🔴 (حَرج — يَفتح بقية النَموذج)  
**ADRs المرجعية:** [ADR-ORCHESTRATION-2026-06-01](../../systems/living-documentation/2-architecture/decisions/) (سيُكتب من Winston)

---

## §1 الهدف

بناء البنية التَحتية الناقصة لنظام الحوكمة v2 (Sprint-per-Dev) كي تُمكِّن PM من تَكليف dev_instances أُخرى بدون عُقَد يَدوية.

**تَعريف النَجاح:** بعد هذا السبرنت، يُمكن لـPM إصدار أمر واحد (`agent_orchestrator.ps1 dispatch M1A-SCHEMAS`) لإطلاق سبرنت جديد تلقائياً، مع قَفل zone، وتَحديث `sprints_active.yaml`.

---

## §2 الستوريات (5 ستوريات مترابطة)

### S-M0-01: scripts/agent_orchestrator.ps1
**الإنتاج:** سكربت PowerShell بـ7 subcommands:
- `dispatch <sprint_id> <dev_instance>` — يُطلق dev_agent على سبرنت
- `status` — يَعرض كل السبرنتات النَشطة من `sprints_active.yaml`
- `lock <zone> <by>` — يَقفل zone
- `unlock <zone>` — يَفك القفل
- `list-zones` — يَعرض الـzones المَحجوزة
- `validate` — يَتحقق من اتساق `sprints_active.yaml`
- `archive <sprint_id>` — يَنقل سبرنت done إلى `_archive/`

**معايير القَبول:**
- يَقرأ/يَكتب `sprints_active.yaml` بصحة YAML
- يَفشل بـexit code ≠ 0 عند تَصادم zone
- يَدعم `--dry-run`

### S-M0-02: scripts/agent_lock.py
**الإنتاج:** مكتبة Python للأقفال الذَرية (لو احتاج dev_agent للقَفل):
- `acquire(zone, owner, timeout)` — يَستخدم filesystem lock
- `release(zone, owner)`
- `is_locked(zone) → owner|None`

**معايير القَبول:**
- اختبار pytest يَفشل عند محاولة قَفل zone مَحجوز
- يَعمل عبر processes (مش thread-only)

### S-M0-03: scripts/scan_layers.py
**الإنتاج:** سكربت Python يَفحص بنية الستة (planning/epics/stories/sprints/status/decisions) لكل نظام تحت `_bmad-output/governance/` و `_bmad-output/systems/`، ويُولِّد تقرير:
- أي نظام يَنقصه مجلد من الستة
- أي نظام parent-system يَنقصه README
- أي ADR بدون `status:` صريح

**معايير القَبول:**
- إخراج JSON قابل للقراءة من CI
- exit code = عدد المشاكل (لـCI gating)

### S-M0-04: _bmad-output/governance/2-agents/DEPUTY_OWNER.md
**الإنتاج:** وثيقة تُحدِّد:
- مَن يَنوب عن PM عند غيابه (deputy)
- صلاحيات الـdeputy (ماذا يَستطيع/لا يَستطيع)
- إجراءات التَسليم بين PM ↔ Deputy

**معايير القَبول:**
- مَرجع صريح في `AGENT_ROSTER.md`
- مُوافَق عليه (status: Accepted)

### S-M0-05: _bmad-output/governance/1-policy/PMF_EVOLUTION.md
**الإنتاج:** سجل تَطور PMF (Project Management Framework):
- نسخ سابقة (v1.0 → v2.0)
- ما تَغير في كل نسخة
- كيف نَنتقل بين النسخ

**معايير القَبول:**
- يَذكر التَحول من v1 (atomic) إلى v2 (sprint-per-dev) في 2026-06-01

---

## §3 Zone (مجلدات حصرية)

- `scripts/` (إضافة فقط — لا حذف ملفات سابقة)
- `_bmad-output/governance/2-agents/` (DEPUTY_OWNER.md + لا تَلمس tasks/ أو AGENT_*)
- `_bmad-output/governance/1-policy/` (PMF_EVOLUTION.md فقط — لا تَلمس PRD أو STATUS)

**ممنوع على هذا السبرنت:**
- لمس أي ملف في `compiler/`, `interpreter/`, `stdlib/`, إلخ
- تَعديل `AGENT_ORCHESTRATION.md` أو `AGENT_ROSTER.md` أو `sprints_active.yaml` (PM فقط)
- حذف أي ملف موجود (GR-02)

---

## §4 ADRs المرجعية

- **ADR-ORCHESTRATION-2026-06-01** (يُكتب من Winston قبل البَدء) — يُؤطر اختيار Sprint-per-Dev بدل Atomic Tasks

---

## §5 Test Strategy

**مُكلَّف بكتابتها:** Murat (TEA) قبل بَدء dev_M0  
**المُتوقَّع:**
- pytest لـ `agent_lock.py` (≥ 5 test cases)
- Pester tests لـ `agent_orchestrator.ps1` (≥ 8 test cases)
- smoke test لـ `scan_layers.py` على الـrepo الحالي
- NFR: السكربتات تَعمل في < 5 ثوانٍ على المشروع الكامل

---

## §6 Deadline + WIP

- **Deadline:** 2026-06-10 (9 أيام)
- **WIP:** dev_M0 instance واحد فقط — لا تَوازي داخل السبرنت
- **يَوم 1-2:** S-M0-01 (orchestrator) — البنية الأم
- **يَوم 3-4:** S-M0-02 (lock) + S-M0-03 (scan)
- **يَوم 5-6:** S-M0-04 (DEPUTY) + S-M0-05 (PMF_EVOLUTION)
- **يَوم 7-8:** اختبارات + تَوثيق
- **يَوم 9:** SPRINT_RESULT.md + تَسليم

---

## §7 Definition of Done

السبرنت لا يُعتبر `done` إلا بعد:

- [ ] 5 ستوريات لها `STORY-*-COMPLETION-REPORT.md` بأدلة
- [ ] جميع الاختبارات الجديدة تَنجح
- [ ] `_bmad-output/governance/1-policy/status/implementation_status.md` مُحدَّث
- [ ] `SPRINT_M0_RESULT.md` مَكتوب بنتائج قابلة للتدقيق
- [ ] TEA approval (Murat يُؤكد NFRs)
- [ ] Tech Writer (Paige) كَتب README لأي مجلد جديد
- [ ] PM كَتب `SPRINT_M0_RETRO.md`
- [ ] `sprints_active.yaml` مُحدَّث: status=done
- [ ] `agent_orchestrator.ps1 archive M0-INFRA` نُفِّذ بنَجاح (يَختبر السكربت ذاته على نَفسه!)

---

## §8 المُتطَلبات قبل البَدء (Pre-Sprint Checklist)

PM لا يُطلق `dev_M0` حتى:

- [ ] Winston يُسلِّم ADR-ORCHESTRATION-2026-06-01
- [ ] Murat يُسلِّم test-strategy لهذا السبرنت
- [ ] هذا الملف (SPRINT_M0_INFRASTRUCTURE.md) مُراجَع
- [ ] `sprints_active.yaml` يَعرض M0-INFRA بحالة `planned`
- [ ] PM يَفحص zones لا تَتصادم مع سبرنت آخر جارٍ

---

## §9 سجل التَنفيذ (يُحدَّث من dev_M0)

| التاريخ | الستوري | الحالة | ملاحظة |
|---|---|---|---|
| - | - | لم يَبدأ بعد | - |

---

## §10 المَخرجات المُسلَّمة (يُملأ في النهاية)

| الملف | السطور | حالة الفحص |
|---|---|---|
| - | - | - |
