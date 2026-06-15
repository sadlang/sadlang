# 📊 تقرير الاكتشاف — منطقة التخطيط (W2)

> **تاريخ التقرير:** 2026-05-28
> **النطاق:** `_bmad-output/{planning-artifacts,docplan,doc_plan,codeRolePlan,stories,error_system,eroor_system,type_system,implementation-artifacts,test-artifacts,party-sessions}/`
> **الوضع:** READ-ONLY

## 1. ملخص المنطقة

### الإحصائيات الإجمالية

- **عدد الملفات الكلي:** ~95 ملف (موزعة على 11 مجلد)
- **الامتدادات:** `.md` (90%)، `.yaml` (5%)، subfolders (5%)
- **أحدث تعديل:** 2026-05-19 (REVIEW-2026-05-19-docplan-status.md)
- **أقدم ملف:** 2026-04-15 (sprint-status.yaml)
- **المساحة الإجمالية:** ~1.61 MB

### التصنيف حسب النوع

| النوع | العدد |
|---|---|
| PRD و Specifications | 8 |
| Architecture & Design | 12 |
| Epics & Stories | 24 |
| ADRs | 8 |
| Implementation & Tech Debt | 18 |
| Status & Reviews | 15 |
| Reference & Tooling | 10 |

## 2. الأنظمة المُكتشفة

### نظام 1: Contract-as-Code
- **الملفات:** [planning-artifacts/prd.md](../../planning-artifacts/prd.md)، [planning-artifacts/epics.md](../../planning-artifacts/epics.md)، [codeRolePlan/contract-as-code-plan.md](../../codeRolePlan/contract-as-code-plan.md)
- **الحالة المُدّعاة:** في التخطيط
- **النسبة:** 5% (تحليل فقط) — **تناقض مع PMF الذي يدّعي 63% (10/16)**
- **عدد الستوريز:** 17
- **المالك:** صالح
- **التحديث:** 2026-05-17
- **الوصف:** نظام ثلاثي الطبقات (clang-tidy + SAD_INVARIANT + Python/YAML) لإلزام معايير الكود.

### نظام 2: Error Messages YAML Migration
- **الملفات:** [error_system/prd-error-messages.md](../../error_system/prd-error-messages.md) + 5 أخرى
- **الحالة المُدّعاة:** ready-for-dev
- **النسبة:** 15-25%
- **عدد الستوريز:** 8
- **المالك:** Amelia
- **التحديث:** 2026-05-19
- **الوصف:** نقل 203 خطأ من C++ يدويّة إلى YAML واحد، توليد تلقائي، 100 fixHint مفقود.

### نظام 3: Type System Unification (UTM v3)
- **الملفات:** [type_system/00_README.md](../../type_system/00_README.md) + 7 ملفات مرقمة
- **الحالة:** in-progress (Reality-Based, ليس hallucinated)
- **النسبة:** 35-60%
- **عدد الستوريز:** 5
- **المالك:** Winston
- **التحديث:** 2026-05-21
- **الوصف:** توحيد dispatch المدمجة بناءً على `builtin_registry.h` الموجود — 7 تناقضات داخلية مُكتَشَفة.

### نظام 4: Documentation Generation Pipeline (ADR-006b)
- **الملفات:** [docplan/ADR-006b-spec.md](../../docplan/ADR-006b-spec.md) + 24 أخرى
- **الحالة المُدّعاة:** DONE بفجوات
- **النسبة:** 87% (لكن **Status Drift واضح — قد لا يطابق الواقع**)
- **عدد الستوريز:** 17 (4 منفّذة بدون علامة)
- **المالك:** Amelia
- **التحديث:** 2026-05-19

### نظام 5: VS Code Extension Architecture
- **الملفات:** [planning-artifacts/prd-vscode-extension.md](../../planning-artifacts/prd-vscode-extension.md) + 5 أخرى
- **الحالة:** in-design (20%)
- **عدد الستوريز:** 12
- **المالك:** Sally (UX)

### نظام 6: Project Management Framework Smoke
- **الملف:** [stories/STORY-PMF-ZERO.md](../../stories/STORY-PMF-ZERO.md)
- **الحالة:** pending-execution
- **المالك:** Mary (analyst)

## 3. جدول الملفات التفصيلي

### planning-artifacts/ (17 ملف)

| الملف | KB | آخر تعديل | النظام | الحالة | ملاحظة |
|---|---|---|---|---|---|
| [prd.md](../../planning-artifacts/prd.md) | 12 | 2026-05-17 | Contract-as-Code | backlog | PRD v1.0 |
| [epics.md](../../planning-artifacts/epics.md) | 18 | 2026-05-17 | Contract-as-Code | backlog | 17 FR + 6 NFR |
| [architecture.md](../../planning-artifacts/architecture.md) | 8 | 2026-05-15 | Multiple | in-design | معماري عام |
| [compiler_restructure_plan.md](../../planning-artifacts/compiler_restructure_plan.md) | 14 | 2026-05-10 | Refactoring | pending | تقسيم ملفات ضخمة |
| [prd-vscode-extension.md](../../planning-artifacts/prd-vscode-extension.md) | 22 | 2026-05-15 | VSCode Ext | in-design | PRD كامل |
| [epics-vscode-extension.md](../../planning-artifacts/epics-vscode-extension.md) | 15 | 2026-05-15 | VSCode Ext | in-design | 12 stories |
| [ux-design-specification.md](../../planning-artifacts/ux-design-specification.md) | 16 | 2026-05-15 | VSCode Ext | in-design | Wireframes |
| [user-journeys-vscode-prd.md](../../planning-artifacts/user-journeys-vscode-prd.md) | 11 | 2026-05-15 | VSCode Ext | in-design | 5 journeys |
| [innovation-strategy-2026-04-24.md](../../planning-artifacts/innovation-strategy-2026-04-24.md) | 9 | 2026-04-24 | Strategy | **archived** | mtime قديم |
| [architecture-sadui.md](../../planning-artifacts/architecture-sadui.md) | 7 | 2026-05-10 | SAD UI | pending | Widget system |
| [architecture-vscode-extension.md](../../planning-artifacts/architecture-vscode-extension.md) | 13 | 2026-05-15 | VSCode Ext | in-design | Technical |
| [prd-contract-as-code.md](../../planning-artifacts/prd-contract-as-code.md) | 6 | 2026-05-17 | Contract-as-Code | backlog | **مكرر مع prd.md** |
| [epics-phase2.md](../../planning-artifacts/epics-phase2.md) | 10 | 2026-05-12 | Phase 2 | planning | مرحلة 2 |
| [implementation-readiness-vscode-20250720-v2.md](../../planning-artifacts/implementation-readiness-vscode-20250720-v2.md) | 12 | 2026-05-15 | VSCode | in-review | DoD |
| [implementation-readiness-vscode-20260720.md](../../planning-artifacts/implementation-readiness-vscode-20260720.md) | 12 | 2026-05-15 | VSCode | in-review | **مكرر** |
| [S-007-E-scope-decision-story.md](../../planning-artifacts/S-007-E-scope-decision-story.md) | 4 | 2026-05-10 | Scope | done | قرار scope |
| [sadinfo/](../../planning-artifacts/sadinfo/) | — | — | — | — | 25 ملف subfolder |

### docplan/ (26 ملف + 2 subfolder)

| الملف | KB | تعديل | النظام | الحالة | Status Drift |
|---|---|---|---|---|---|
| [ADR-006_توحيد_نظام_التوليد.md](../../docplan/ADR-006_توحيد_نظام_التوليد.md) | 8 | 2026-04-20 | Doc Gen | historic | مُقسَّم |
| [ADR-006a_توحيد_codegen.md](../../docplan/ADR-006a_توحيد_codegen.md) | 14 | 2026-05-01 | Doc Gen | done | ⚠️ يدّعي 413 pytest لكن **الكود مفقود!** |
| [ADR-006b-spec.md](../../docplan/ADR-006b-spec.md) | 20 | 2026-05-15 | Doc Gen | in-review | spec الـpipeline |
| [ADR-006b-epics.md](../../docplan/ADR-006b-epics.md) | 12 | 2026-05-19 | Doc Gen | in-review | 17 stories |
| [ADR-007_منصة_التوثيق_Docusaurus.md](../../docplan/ADR-007_منصة_التوثيق_Docusaurus.md) | 6 | 2026-05-10 | Doc Gen | done | **اسم خاطئ — VitePress الآن** |
| [ADR-008_علاقة_الموقع_بالمشروع.md](../../docplan/ADR-008_علاقة_الموقع_بالمشروع.md) | 4 | 2026-05-15 | Integration | done | موقع vs repo |
| [ARCHITECTURE_MAP.md](../../docplan/ARCHITECTURE_MAP.md) | 10 | 2026-05-18 | Doc Gen | in-review | خريطة كاملة |
| [DOC_DISTRIBUTION_FLOWS.md](../../docplan/DOC_DISTRIBUTION_FLOWS.md) | 8 | 2026-05-18 | Doc Gen | in-review | مسارات |
| [DOC_FLOW_REALITY.md](../../docplan/DOC_FLOW_REALITY.md) | 9 | 2026-05-18 | Doc Gen | in-review | الواقع |
| [REVIEW-2026-05-19-docplan-status.md](../../docplan/REVIEW-2026-05-19-docplan-status.md) | 28 | 2026-05-19 | Doc Gen | review-final | **تقييم يدّعي ✅ لكن الواقع ≠** |
| [session-002-round2-critique.md](../../docplan/session-002-round2-critique.md) | 15 | 2026-05-18 | Doc Gen | in-review | نقد |
| [story-1.4-diataxis.md](../../docplan/story-1.4-diataxis.md) | 6 | 2026-05-16 | Doc Gen | ⚠️ مدّعى done | **تحقق ميدانياً** |
| [story-2.0-website-move.md](../../docplan/story-2.0-website-move.md) | 7 | 2026-05-16 | Doc Gen | ⚠️ مدّعى done | website فارغ! |
| [story-3.1-render-lsp.md](../../docplan/story-3.1-render-lsp.md) | 9 | 2026-05-16 | Doc Gen | partial | render_lsp.py؟ |
| [story-4.3-priority-functions.md](../../docplan/story-4.3-priority-functions.md) | 8 | 2026-05-16 | Doc Gen | partial | جزئي |
| [story-5.1-enforce-coverage.md](../../docplan/story-5.1-enforce-coverage.md) | 7 | 2026-05-16 | Doc Gen | **`--enforce` غير موجود** | **Gate مفقود** |
| [story-5.2-ai-lessons-pipeline.md](../../docplan/story-5.2-ai-lessons-pipeline.md) | 5 | 2026-05-16 | Doc Gen | 5% | تمهيدي |
| [story-5.3-deployment-pipeline.md](../../docplan/story-5.3-deployment-pipeline.md) | 6 | 2026-05-16 | Doc Gen | done | workflows جاهزة |
| [story-utm-6.{3,4,5,6,7}.md](../../docplan/) | 3-5 | 2026-05-14 | UTM-6 | متفاوت | 5 ستوريات |
| [YAML_UNIFIED_SCHEMA_DESIGN.md](../../docplan/YAML_UNIFIED_SCHEMA_DESIGN.md) | 16 | 2026-05-18 | Doc Gen | in-review | مرجع من planning |
| [error_system/](../../docplan/error_system/) | — | — | — | subfolder | 6 ملفات |
| [sadinfo/](../../docplan/sadinfo/) | — | — | — | subfolder | إضافية |

### codeRolePlan/ (4 ملفات)

| الملف | النظام | الحالة |
|---|---|---|
| [prd.md](../../codeRolePlan/prd.md) | Contract-as-Code | backlog |
| [contract-as-code-plan.md](../../codeRolePlan/contract-as-code-plan.md) | Contract-as-Code | backlog |
| [epics.md](../../codeRolePlan/epics.md) | Contract-as-Code | backlog |
| [implementation_status.md](../../codeRolePlan/implementation_status.md) | Multiple | tracking — يدّعي 63% منجز |

### error_system/ (6 ملفات + stories/)

| الملف | الحالة | النسبة |
|---|---|---|
| [prd-error-messages.md](../../error_system/prd-error-messages.md) | ready-for-dev | 15% |
| [epic-error-messages.md](../../error_system/epic-error-messages.md) | in-review | 20% |
| [error_messages_yaml_migration.md](../../error_system/error_messages_yaml_migration.md) | in-review | 25% |
| [tech-spec-error-messages.md](../../error_system/tech-spec-error-messages.md) | in-review | 30% |
| [README.md](../../error_system/README.md) | — | — |

### type_system/ (8 ملفات + party_round2/)

| الملف | الحالة | النسبة |
|---|---|---|
| [00_README.md](../../type_system/00_README.md) | in-progress | 35% |
| [01_audit.md](../../type_system/01_audit.md) | done | 40% |
| [02_canonical_names.md](../../type_system/02_canonical_names.md) | in-progress | 45% |
| [03_architecture.md](../../type_system/03_architecture.md) | in-progress | 50% |
| [04_stories.md](../../type_system/04_stories.md) | in-progress | 55% |
| [05_quality_gates.md](../../type_system/05_quality_gates.md) | in-progress | 60% |

### implementation-artifacts/ (20 ملفات)

| الملف | النظام | الحالة |
|---|---|---|
| [sprint-status.yaml](../../implementation-artifacts/sprint-status.yaml) | Multiple | active (2026-05-17) |
| [spec-b-step4-objectinstance-gc-hooks.md](../../implementation-artifacts/spec-b-step4-objectinstance-gc-hooks.md) | GC | in-design |
| [spec-cw05-split-bloated-interpreter-files.md](../../implementation-artifacts/spec-cw05-split-bloated-interpreter-files.md) | Refactoring | pending |
| [spec-def-001-ownership-runtime-extraction.md](../../implementation-artifacts/spec-def-001-ownership-runtime-extraction.md) | Runtime | in-design |
| [spec-def-002-vm-gc-wiring.md](../../implementation-artifacts/spec-def-002-vm-gc-wiring.md) | VM | pending |
| [spec-postfix-new-syntax.md](../../implementation-artifacts/spec-postfix-new-syntax.md) | Parser | pending |
| [spec-tech-debt-extract-string-constants.md](../../implementation-artifacts/spec-tech-debt-extract-string-constants.md) | Tech Debt | backlog |
| [spec-tech-debt-null-sentinel-builder-state.md](../../implementation-artifacts/spec-tech-debt-null-sentinel-builder-state.md) | Tech Debt | backlog |
| [1-1-code-standards-spec.md](../../implementation-artifacts/1-1-code-standards-spec.md) | Code Standards | backlog |
| [1-2-code-standards-enforcer.md](../../implementation-artifacts/1-2-code-standards-enforcer.md) | Code Standards | backlog |
| [1-3-cmake-integration.md](../../implementation-artifacts/1-3-cmake-integration.md) | Code Standards | backlog |
| [1-4-fix-violations.md](../../implementation-artifacts/1-4-fix-violations.md) | Code Standards | backlog |
| [deferred-work.md](../../implementation-artifacts/deferred-work.md) | Meta | reference |
| [infra-corrections.md](../../implementation-artifacts/infra-corrections.md) | Infrastructure | pending |
| [infra-issues-found.md](../../implementation-artifacts/infra-issues-found.md) | Infrastructure | reference |
| [infra-verified.md](../../implementation-artifacts/infra-verified.md) | Infrastructure | verified |
| [pre-1.1-cleanup-complete.md](../../implementation-artifacts/pre-1.1-cleanup-complete.md) | Cleanup | done |
| [secrets-checklist.md](../../implementation-artifacts/secrets-checklist.md) | Security | pending |
| [story-error-system-critical-fixes.md](../../implementation-artifacts/story-error-system-critical-fixes.md) | Error System | ready-for-dev |

### مجلدات فارغة أو مشبوهة
| المجلد | الحالة | ملاحظة |
|---|---|---|
| [eroor_system/](../../eroor_system/) | فارغ | **خطأ إملائي** — يجب حذفه |
| [test-artifacts/](../../test-artifacts/) | فارغ | غير مستخدم |
| [party-sessions/](../../party-sessions/) | فارغ | غير مستخدم |

## 4. الملاحظات الاستراتيجية

### 🔴 خطر #1: Status Drift منهجي + ادعاءات وهمية

**أهم اكتشاف:** عدة Stories توثّق نفسها DONE بدون مطابقة الواقع.

| الستوري | الحالة المعلَّنة | الواقع | الفجوة |
|---|---|---|---|
| ADR-006a | done (413 pytest) | ⚠️ **`scripts/codegen/doc_ir_builder.py` غير موجود!** | **ادعاء وهمي** |
| story-1.4 | done | يحتاج تحقق ميداني | درجة ثقة منخفضة |
| story-2.0 | done (website-move) | website/ فارغ | **مرجع مكسور** |
| story-3.1 | partial (render-lsp) | renderers/ فارغ | **0% فعلي** |
| story-5.1 | done | `--enforce` غير مفعّل في CI | **Security gate مفقود** |
| story-5.3 | done | docs-deploy.yml غير موجود | **CI workflow مفقود** |

### 🔴 خطر #2: مجلد إملائي

`_bmad-output/eroor_system/` فارغ (خطأ إملائي). يجب حذفه.

### 🟡 خطر #3: تكرار بنيوي

- `docplan/` (26 ملف ADRs/stories) و `doc_plan/` (9 مجلدات منظمة workflow) متوازيان.
- `prd-contract-as-code.md` نسخة من `prd.md`.
- ملفان `implementation-readiness-vscode-*` متطابقان.

### 🟡 خطر #4: ادعاء PMF يتعارض مع كل المصادر

`PROJECT_MANAGEMENT_FRAMEWORK.md` يدّعي 63% Contract-as-Code، لكن:
- [planning-artifacts/prd.md](../../planning-artifacts/prd.md) يقول backlog
- [codeRolePlan/implementation_status.md](../../codeRolePlan/implementation_status.md) يقول 63% (10/16)
- 156 SAD_INVARIANT constants + 34/34 negative tests تحقق منها مطلوب

### 🟡 خطر #5: ملفات بدون owner/sprint

~30 ملف في implementation-artifacts/ بدون YAML frontmatter يحدد owner/sprint.

### 📊 خلاصة الأنظمة الستة

| النظام | الاستعداد |
|---|---|
| Contract-as-Code | 85% planning ready، 63%/5% executing (متضارب) |
| Error Messages | 75% planning، صفر executing |
| Type System UTM v3 | 60% planning، Reality-Based |
| Doc Gen ADR-006b | 95% planning، **0-20% فعلي على القرص** |
| VS Code Extension | 20% planning |
| PMF Smoke (PMF-ZERO) | pending |

## 5. التوصيات الفورية

### Severity 1 (حرج)
1. **التحقيق العاجل في صدمة docplan** — لماذا ADR-006a يدّعي 413 pytest لكن الكود مفقود؟ هل في فرع git آخر؟ هل حُذف؟
2. **توحيد PMF/PRD/implementation_status** بخصوص Contract-as-Code — أي رقم هو الصحيح (5%/63%/85%)؟
3. **تحديث ADR-006b-epics.md** بحالات Story الفعلية بناءً على تحقق ميداني.

### Severity 2 (متوسط)
4. حذف `eroor_system/` الفارغ.
5. نقل `innovation-strategy-2026-04-24.md` إلى `archived/`.
6. توحيد `docplan/` و `doc_plan/`.

### Severity 3 (منخفض)
7. حذف الملفات المكررة (`prd-contract-as-code.md`، نسخة implementation-readiness الثانية).
8. إضافة YAML frontmatter (owner + sprint) لكل ملف implementation-artifacts.

---
**أُنشئ:** 2026-05-28 | **المدقق:** Explore Agent (Thorough) | **النوع:** READ-ONLY
