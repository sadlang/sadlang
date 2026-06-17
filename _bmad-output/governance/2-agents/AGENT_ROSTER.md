# 👥 كَشف الوكلاء v2 — Sprint-per-Dev

**التاريخ:** 2026-06-01  
**يَنسخ ويَحل محل:** [v1 (11 وكيلاً سياقياً)](_archive/2026-06-01_atomic-tasks-model/AGENT_ROSTER.md)

---

## 1. الفلسفة الجديدة

بدل **5 وكلاء سياقيين** (α β γ δ ε) يَأخذون مَهام ذرية، نَستخدم **مهارة `bmad-agent-dev` واحدة** بـ**عدة instances متوازية**، كل instance يَملك سبرنتاً كاملاً في zone حصري.

---

## 2. القائمة (8 أدوار، 3 طبقات)

### 🎯 الطبقة 1 — القيادة (تُخطط، لا تُنفذ)

| # | الدور | المهارة BMAD | الذي يَملكه | WIP |
|---|---|---|---|---|
| 1 | **PM** | `bmad-agent-pm` (John) | `_bmad-output/governance/1-policy/`, تَخطيط السبرنتات، PRDs | ∞ |
| 2 | **Architect** | `bmad-agent-architect` (Winston) | جميع `decisions/ADR-*.md` في كل الأنظمة | ∞ |
| 3 | **TEA** | `bmad-tea` (Murat) | `*/test-strategy/`, NFR audits | ∞ |

### ⚙️ الطبقة 2 — التَنفيذ (تُسلِّم سبرنتات كاملة)

**المهارة الواحدة:** `bmad-agent-dev` (Amelia)  
**Instances جارية مُحتملة:**

| # | Instance | Zone حَصري | السبرنت النَموذجي |
|---|---|---|---|
| 4 | `dev_M0` | `scripts/`, `_bmad-output/governance/2-agents/` | البنية التَحتية للحوكمة (orchestrator, locks, scan_layers, DEPUTY, PMF) |
| 5 | `dev_M1A` | `_bmad-output/systems/living-documentation/2-architecture/schemas/`, `tools/living-docs-loader/` | Foundation Schemas + Loader PoC |
| 6 | `dev_M1B` | `_bmad-output/systems/living-documentation/3-implementation/poc/`, `data/_meta/` | Living Docs MVP |
| 7 | `dev_kernel` | `compiler/`, `interpreter/`, `vm/`, `runtime/`, `shared/` | أي سبرنت في النَواة |
| 8 | `dev_stdlib` | `stdlib/`, `data/stdlib/`, `data/language/` | المكتبة القياسية + ملفات data |
| 9 | `dev_tools` | `tools/`, `data/_schemas/` | LSP, Formatter, Pkg Manager |
| 10 | `dev_graphics` | `graphics/`, `sad_ui/`, `network/`, `platform/` | الرسوميات + الواجهات |
| 11 | `dev_tests` | `tests/comprehensive/`, `tests/*/` | حُزم الاختبارات الموسَّعة |

**القاعدة:** يُمكن تَشغيل أي عدد من هذه instances **بشرط أن zones لا تَتقاطع**.

### 📚 الطبقة 3 — المساندة

| # | الدور | المهارة BMAD | الذي يَملكه | WIP |
|---|---|---|---|---|
| 12 | **Tech Writer** | `bmad-agent-tech-writer` (Paige) | كل `README.md`, `INDEX.md`, تَوثيق نهائي بعد كل sprint | ∞ |
| 13 | **Analyst** | `bmad-agent-analyst` (Mary) | research + متطلبات + market analysis | ∞ |

---

## 3. تَخصيصات Sprint #2 (يُنشَأ من PM)

| Sprint ID | المُكلَّف | Zone | الأولوية | الحالة |
|---|---|---|---|---|
| **M0-INFRA** | `dev_M0` | scripts + 2-agents | P0 🔴 | planned |
| M1A-SCHEMAS | `dev_M1A` | schemas + loader | P0 🔴 | blocked (waits Winston ADRs) |
| M1B-MVP | `dev_M1B` | poc + data/_meta | P1 🟡 | blocked (waits M1A) |
| KERNEL-S04 | `dev_kernel` | compiler + interpreter | P1 🟡 | planned (مستقل) |

**التَوازي الأقصى الواقعي:** 3 dev_instances في وقت واحد (M0 + KERNEL + M1A بعد ADRs).

---

## 4. مَن يَكتب ماذا؟ (مرجع سريع)

| النَوع | الكاتب | المكان |
|---|---|---|
| PRD | PM | `*/planning/PRD.md` |
| ADR | Architect | `*/decisions/ADR-*.md` |
| Test Strategy | TEA | `*/test-strategy/*.md` |
| SPRINT plan | PM | `*/sprints/SPRINT_*.md` |
| Story | PM أو bmad-create-story | `*/stories/STORY-*.md` |
| كود | Dev Agent | داخل zone |
| اختبارات | Dev Agent | `tests/` داخل zone |
| Completion Report | Dev Agent | `*/stories/STORY-*-COMPLETION-REPORT.md` |
| Sprint Result | Dev Agent | `*/sprints/SPRINT_*_RESULT.md` |
| Sprint RETRO | PM | `*/sprints/SPRINT_*_RETRO.md` |
| README/INDEX | Tech Writer | كل مجلد |
| Verification Report | PM (مع TEA) | `governance/1-policy/status/VERIFICATION_REPORT_*.md` |

---

## 5. قَواعد التَشغيل المتوازي

- **R-01:** قبل بَدء instance جديد، PM يَفحص `sprints_active.yaml` للتأكد من عدم تَداخل zones.
- **R-02:** عند الحاجة لمشاركة ملف بين zones (مثل `tasks/index.yaml`)، يَمر التَعديل عبر PM فقط.
- **R-03:** dev_instance لا يَلمس `_bmad-output/governance/` إلا zone المُخصَّص له (dev_M0).
- **R-04:** عند فشل dev_instance، السبرنت يَنتقل لحالة `blocked` ويَنتظر PM لإعادة التَخطيط.
- **R-05:** Tech Writer (Paige) يَعمل **بالتَوازي** مع dev_instances الأخرى — zone خاصة (READMEs فقط) لا تَتصادم.

---

## 6. الفَرق عن v1

| البُعد | v1 | v2 |
|---|---|---|
| عدد الوكلاء المُعرَّفين | 11 (5 منهم سياقيون مُعقَّدون) | 8 أدوار، لكن 8 instances من نفس المهارة Dev |
| نموذج التَنفيذ | مَهام ذرية | سبرنتات كاملة |
| التَوازي الواقعي | 2-3 | 3-6 |
| تَعقيد PM | عالي (10+ مَهام/سبرنت) | منخفض (3-4 سبرنتات/فترة) |
| استخدام BMAD skills | متَفرق | مُتَوافق مع تَصميم BMAD (dev = full sprint owner) |
