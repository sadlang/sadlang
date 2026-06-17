---
title: "آلية تنسيق الوكلاء — Task-File Protocol"
type: governance-mechanism
date: 2026-06-01
status: ACTIVE
version: 1.0
owner: PM (John)
governance_rules: ["GR-01", "GR-03", "GR-05", "GR-08", "GR-10"]
---

# 🤖 آلية تَنسيق الوكلاء (Agent Orchestration)

> بروتوكول رسمي يُمكِّن مدير المشروع (PM) من إصدار أوامر لوكلاء مُستقلين متوازين عبر ملفات نَصية، وتَلقي النتائج بنفس الآلية.

---

## 1. المبدأ

**PM لا يُنفذ — PM يُكلِّف ويُراجع.**

- المدير يَكتب "ملف أمر" (`task`) → وكيل مُتخصص يَقرأه → يُنفذ المهمة → يَكتب "ملف نتيجة" (`result`).
- التَنسيق غير متزامن: المدير لا يَنتظر، الوكلاء يَعملون متوازين.
- الحالة موحَّدة في `index.yaml` — مَصدر الحقيقة الوحيد.

---

## 2. بنية المجلدات

```
_bmad-output/governance/2-agents/
├── README.md                          ← الوكلاء الـ5 (α β γ δ ε)
├── AGENT_ORCHESTRATION.md             ← هذا الملف
├── AGENT_ROSTER.md                    ← الوكلاء النَشطون + خرائط النطاقات
├── agent_alpha.md … agent_epsilon.md  ← مَواثيق الوكلاء
│
├── tasks/                             ← 🆕 نظام المهام
│   ├── README.md                      ← دليل البروتوكول
│   ├── index.yaml                     ← فهرس كل المهام (SoT)
│   ├── inbox/                         ← أوامر للوكلاء (يَكتبها PM)
│   │   └── T-{NNNN}-{agent}-{slug}.md
│   ├── in-progress/                   ← المهام النَشطة (يَنقلها الوكيل عند البدء)
│   ├── outbox/                        ← نتائج (يَكتبها الوكلاء)
│   │   └── T-{NNNN}-{agent}-{slug}-RESULT.md
│   ├── done/                          ← مَهام مُنجزة + مُراجعة من PM
│   └── locks/                         ← قفل لكل مَهمة نَشطة
│       └── T-{NNNN}.lock
│
└── rfcs/                              ← RFCs (موجود حالياً)
```

---

## 3. صيغة ملف الأمر (Task File)

```markdown
---
task_id: T-0001
title: "إنشاء data/_schemas/keywords.yaml"
assigned_to: agent_alpha       # أو "agent_alpha,agent_beta" للمتوازي
created_by: PM (John)
created_at: 2026-06-01T08:45:00Z
deadline: 2026-06-08
priority: P0                   # P0/P1/P2
story_id: S-015b               # من LIVING_DOCS_ROADMAP
milestone: M1
depends_on: []                 # task_ids
guarded_files: []              # G-XX من GUARDED_FILES.md
status: queued                 # queued / in-progress / blocked / done / failed
estimated_hours: 3
---

# 📋 المهمة: <العنوان>

## السياق
<لماذا هذه المهمة، الرابط بالستوري>

## المُدخلات
- ملفات يَقرأها الوكيل
- مَراجع مهمة

## المخرجات المطلوبة (DoD)
- [ ] ملف X موجود
- [ ] اختبار Y ينجح
- [ ] grep يُؤكد Z

## القواعد
- اتبع CW-XX, BF-XX حسب المهمة
- إذا واجَهت ملف محروس → توقف واطلب PR review

## كيفية الإبلاغ
1. عند البدء: انقل الملف من `inbox/` إلى `in-progress/` + أنشئ `locks/T-XXXX.lock` بمحتوى `{agent_id, started_at, pid}`
2. عند الانتهاء: اكتب `outbox/T-XXXX-...-RESULT.md` (الصيغة في §4)
3. حدِّث `index.yaml` (status, completed_at)
4. احذف القفل
```

---

## 4. صيغة ملف النتيجة (Result File)

```markdown
---
task_id: T-0001
agent_id: agent_alpha
started_at: 2026-06-01T09:00:00Z
completed_at: 2026-06-01T11:30:00Z
hours_actual: 2.5
status: done                   # done / partial / failed / blocked
exit_evidence:
  - "Test-Path data/_schemas/keywords.yaml → True"
  - "yq eval '.keywords | length' → 65"
  - "comprehensive_tests → 945/945 pass"
---

# ✅ النتيجة: T-0001

## ما تَم إنجازه
<قائمة مرقَّمة بالتغييرات>

## الأدلة (GR-01 — لا ادعاءات بلا أدلة)
```powershell
PS> <أمر تَحقق>
<الخرج>
```

## القيود / المخاطر المُكتشفة
<أي شيء فاجَأنا>

## التَوصيات للـPM
<مَهام تالية مَقترحة>

## المراجعة المطلوبة
- [ ] CW-XX
- [ ] BF-XX
- [ ] الملفات المحروسة: <لا/نعم — أيها>
```

---

## 5. صيغة `index.yaml`

```yaml
version: 1
updated_at: 2026-06-01T08:45:00Z
tasks:
  T-0001:
    title: "إنشاء data/_schemas/keywords.yaml"
    agent: agent_alpha
    status: queued
    priority: P0
    milestone: M1
    story: S-015b
    created: 2026-06-01T08:45:00Z
    deadline: 2026-06-08
  T-0002:
    title: "إنشاء scripts/agent_lock.py"
    agent: agent_delta
    status: queued
    priority: P0
    milestone: M0          # بنية تَحتية قبل M1
    ...
```

---

## 6. قواعد التَوازي

### القواعد الصارمة

| # | القاعدة |
|---|---|
| **OR-01** | كل وكيل له **WIP محدد** (راجع [README.md](README.md)) — لا يَتجاوزه |
| **OR-02** | مَهمة في نطاق محروس (Guarded File) تَتوقف وتَطلب PR review من Saleh |
| **OR-03** | المَهمة بدون قفل صالح **لا تَبدأ** — `locks/T-XXXX.lock` إلزامي |
| **OR-04** | القفل عمره TTL = 4 ساعات. بعدها يُلغى تلقائياً ويُعاد التَكليف |
| **OR-05** | كل وكيل يَكتب فقط في نطاقه — تَجاوز النطاق = توقف + RFC |
| **OR-06** | المهام المُعتمدة (`depends_on`) لا تَبدأ إلا بعد انتهاء سابقاتها |
| **OR-07** | نتيجة `failed` = PM يُحلل ويُعيد التَكليف (قد يُغير الوكيل) |
| **OR-08** | كل مَهمة مُنجزة تُراجع من PM قبل النَقل إلى `done/` |
| **OR-09** | RFC إلزامي للستوريات Lead-Follow (وكيلان أو أكثر يَعملان معاً) |
| **OR-10** | الإقرار الحَوكمي إلزامي في كل ملف نتيجة (سطر `> قرأت...`) |

### تَشغيل وكلاء متعدِّدين بنفس المهارة

نعم — يَدعم البروتوكول وكلاء متعدِّدين بنفس "النَوع":

```
agent_delta_1   ← يَعمل على S-001 (Loader PoC)
agent_delta_2   ← يَعمل على S-015a (Migrate Builtins)
agent_delta_3   ← يَعمل على T-0002 (إنشاء agent_lock.py)
```

- نفس النَطاق (`stdlib/+tools/+tests/`)
- لكن **مَهام مُختلفة لا تَتقاطع في الملفات**
- PM يَفحص التَقاطع قبل التَكليف (يُمكن أتمَتته في PR check)

---

## 7. دورة الحياة (Lifecycle)

```mermaid
stateDiagram-v2
    [*] --> queued: PM يَكتب inbox/T-XXXX.md
    queued --> in-progress: الوكيل يَنقل + يَقفل
    in-progress --> done: الوكيل يَكتب outbox/ + يُلغي القفل
    in-progress --> blocked: ملف محروس / تَبعية مَفقودة
    in-progress --> failed: خطأ تَنفيذي
    blocked --> queued: PM يَحل العائق
    failed --> queued: PM يُعيد التَكليف
    done --> reviewed: PM يَفحص النَتيجة
    reviewed --> [*]: PM يَنقل إلى done/
```

---

## 8. أوامر CLI مساعدة (سيتم إنشاؤها كستوري M0)

```powershell
# قائمة المهام
.\scripts\agent_orchestrator.ps1 list                  # كل المهام
.\scripts\agent_orchestrator.ps1 list -Agent alpha     # مَهام وكيل واحد
.\scripts\agent_orchestrator.ps1 list -Status queued

# تَكليف مَهمة جديدة
.\scripts\agent_orchestrator.ps1 assign -Agent alpha -Story S-015b -Priority P0

# قراءة حالة وكيل
.\scripts\agent_orchestrator.ps1 status -Agent alpha

# تَنظيف الأقفال المُنتهية
.\scripts\agent_orchestrator.ps1 gc-locks --ttl-hours 4

# تَوليد تَقرير
.\scripts\agent_orchestrator.ps1 report > _bmad-output/governance/1-policy/status/AGENT_STATUS_$(Get-Date -Format yyyy-MM-dd).md
```

---

## 9. تَكامل مع الحوكمة الموجودة

| الموجود | يَتكامل مع |
|---|---|
| `1-policy/PRD.md` | يَحكم ما يَصلح للتَكليف |
| `1-policy/sprints/SPRINT_CURRENT.md` | المهام تُشتق من السبرنت |
| `3-code-contract/planning/prd.md` | الوكلاء يَتبعون عقد الكود |
| `2-agents/agent_alpha.md..agent_epsilon.md` | مَواثيق الوكلاء = حدود النَطاق |
| `bmad-governance-check` skill | يَفحص قبل كل مَهمة (OR-10) |
| `bmad-create-story` skill | يُولِّد ملفات `inbox/` لستوريات M1..M7 |

---

## 10. خطوات التَفعيل

| # | الخطوة | المسؤول | الموعد |
|---|---|---|---|
| 1 | إنشاء مَجلدات `tasks/{inbox,in-progress,outbox,done,locks}/` | PM (الآن) | 2026-06-01 |
| 2 | إنشاء `tasks/index.yaml` فارغ | PM (الآن) | 2026-06-01 |
| 3 | إنشاء [AGENT_ROSTER.md](AGENT_ROSTER.md) | PM (الآن) | 2026-06-01 |
| 4 | إنشاء 5 ملفات أوامر أولى في `inbox/` | PM (الآن) | 2026-06-01 |
| 5 | إنشاء `scripts/agent_orchestrator.ps1` (T-0003) | agent_delta | 2026-06-03 |
| 6 | إضافة فحص `index.yaml` إلى `bmad-governance-check` | agent_delta | 2026-06-04 |

---

**المرجع:** هذا المستند هو SoT (Source of Truth) لبروتوكول الوكلاء. أي تَعديل = ADR جديد + version bump.
