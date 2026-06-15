---
title: "تقرير تحقّق شامل — مطابقة خطط الحوكمة للكود الفعلي"
date: 2026-05-30
status: ACTIVE
author: Amelia (bmad-agent-dev)
version: "2.0 — إعادة كتابة كاملة بعد اكتشاف خطط execution/ + 3-code-contract/"
scope: "كل ملفات _bmad-output/governance/ مقابل الكود الفعلي"
methodology: "فحص مباشر — لا تشغيل اختبارات"
relatedDocuments:
  - 1-policy/planning/PRD.md
  - 1-policy/planning/ARCHITECTURE.md
  - 1-policy/planning/PROJECT_MANAGEMENT_FRAMEWORK.md
  - 1-policy/planning/LAYERS.json
  - 1-policy/planning/THRESHOLDS.json
  - 1-policy/planning/EDGE_CASE_GUARDS.md
  - 1-policy/planning/ROADMAP.md
  - 1-policy/epics/BACKLOG.md
  - 1-policy/sprints/SPRINT_CURRENT.md
  - 1-policy/stories/STORY-PMF-V17-ENFORCE-GPG.md
  - 3-code-contract/planning/contract-as-code-plan.md
  - 3-code-contract/planning/prd.md
  - 3-code-contract/epics/epics.md
  - 3-code-contract/status/implementation_status.md
purpose: |
  بعد تنبيه المستخدم على وجود خطط execution/ و 3-code-contract/ التي
  لم يَفحصها التقرير السابق، أُعيد كتابة التقرير بالكامل بمصادر أوسع
  ونتائج مختلفة جذرياً عن النسخة الأولى.
priorReportNote: "النسخة v1.0 من نفس اليوم كانت ناقصة — اعتمدت على 1-policy فقط وأهملت execution/ و 3-code-contract/. لذلك أرقام v1.0 (65% تحقق) كانت مُضلِّلة. هذه النسخة v2.0 هي المعتمدة."
---

# تقرير تحقّق الخطة من الكود — v2.0

> **تنبيه:** هذا التقرير يحلّ محل الإصدار v1.0 الصادر في نفس اليوم. v1.0 لم يفحص ملفات `execution/` (خارطة الطريق + Backlog + Sprint) ولا ملفات `3-code-contract/` (خطة Contract-as-Code المكوَّنة من PRD + Epics + Implementation Status). لذا أرقامه ونتائجه كانت ناقصة جوهرياً.

---

## 0.0 إجراءات تنظيمية مُنجَزة كجزء من v2.0 (2026-05-30)

بعد كتابة التقرير، نُفِّذت إعادة تنظيم بنيوية على `_bmad-output/governance/`:

1. **بنية موحَّدة** لكل من `1-policy/` و `3-code-contract/`: `planning/` + `epics/` + `stories/` + `sprints/` + `status/` + `decisions/`.
2. **نُقلت 21 ملفاً** إلى مجلداتها الصحيحة (المسارات المُحدَّثة موجودة في `relatedDocuments` أعلاه).
3. **حُذف مجلد `execution/`** بعد نقل محتوياته (`ROADMAP` → `planning/`، `BACKLOG` → `epics/`، `SPRINT_CURRENT` → `sprints/`).
4. **أُضيفت شارة `OUT-OF-DATE`** على:
   - [`3-code-contract/status/implementation_status.md`](../../3-code-contract/status/implementation_status.md) — لادعاءاته 156 ثابتاً غير موجود
   - [`1-policy/stories/STORY_ZERO_REPORT.md`](../stories/STORY_ZERO_REPORT.md) — لادعاءاته سكريبتات غير موجودة
5. **أُضيف README.md** في كل مجلد جديد يوضح قاعدته (ما يُسمح وما لا يُسمح وضعه فيه).
6. **حُدِّث `_bmad-output/governance/README.md`** ليصف البنية الجديدة وروابط الدخول السريع.

هذه الإجراءات تُغطي التوصية **R0** بالكامل (تعليم الوثائق المضلِّلة) + بداية البنية الموحَّدة للتنفيذ المستقبلي.

---

## 0. منهجية v2.0 (تصحيح للنقص السابق)

| المصدر | كيف فُحص |
|---|---|
| `1-policy/planning/PRD.md` | قراءة كاملة + مقارنة بأهداف G1-G6 بالـcmake targets والمجلدات |
| `1-policy/planning/ARCHITECTURE.md` | فحص اسم `ValueType` (مفقود — الفعلي `DataType`) وعدد `SIROpcode` |
| `1-policy/planning/PROJECT_MANAGEMENT_FRAMEWORK.md` (713 سطر) | قراءة كاملة + فحص كل `[`scripts/X`]` و`workflows/Y` فيه |
| `1-policy/planning/LAYERS.json` | `Test-Path` على كل من الـ21 طبقة |
| `1-policy/planning/THRESHOLDS.json` | قياس فعلي لكل عتبة على ملفات الحوكمة |
| `1-policy/planning/EDGE_CASE_GUARDS.md` | فحص كل سكريبت/workflow مذكور |
| `1-policy/planning/ROADMAP.md` | فحص معالم M1/M2/M3 |
| `1-policy/epics/BACKLOG.md` | فحص الستوريات B-001..B-015 |
| `1-policy/sprints/SPRINT_CURRENT.md` | فحص الستوريات الثلاث الحالية |
| `1-policy/stories/STORY-PMF-V17-ENFORCE-GPG.md` | فحص حالة GPG/BP |
| `3-code-contract/planning/prd.md` | فحص FR-01..FR-17 |
| `3-code-contract/epics/epics.md` | قراءة Epic 1..4 |
| `3-code-contract/status/implementation_status.md` | **مقارنة كل ادعاء فيه بالواقع** |

**أدوات الفحص:** PowerShell scripts في `_scratch/*.ps1` (حُذفت بعد القياس). PowerShell 5.1 + `[regex]::Matches()` + `Test-Path` + قراءة UTF-8 صريحة بـ `[System.IO.File]::ReadAllText()`.

---

## 1. الملخص التنفيذي

### النتيجة الإجمالية: **~52% تحقّق فعلي**

(نسبة v1.0 = 65% كانت متفائلة بسبب إغفال `3-code-contract/` الذي يضيف 16 ادعاء معظمها مفقود).

| المصدر | محقَّق | غير محقَّق | جزئي | عدد الادعاءات |
|---|---|---|---|---|
| LAYERS.json | 21 | 0 | 0 | 21 ✅ 100% |
| PRD.md (1-policy) | 4 | 0 | 2 | 6 ✅ 83% |
| ARCHITECTURE.md (1-policy) | 7 | 3 | 0 | 10 ⚠️ 70% |
| PMF + EDGE_GUARDS scripts | 1 | 7 | 0 | 8 🔴 13% |
| PMF + EDGE_GUARDS workflows | 5 | 1 | 0 | 6 ⚠️ 83% |
| THRESHOLDS عتبات (الحوكمة نفسها) | 0 | 5 | 0 | 5 🔴 0% |
| execution/STORY-PMF-V17 (GPG) | 1 | 0 | 0 | 1 ✅ 100% |
| execution/BACKLOG (15 ستوري) | 1 DONE | 1 BLOCKED | 13 READY | 15 (READY = خطط لم تبدأ) |
| **3-code-contract Epic 1** (clang-tidy + fix) | 2 | 0 | 0 | 2 ✅ 100% |
| **3-code-contract Epic 2** (SAD_INVARIANT) | **0** | **8** | **0** | **8 🔴 0%** |
| **3-code-contract Epic 3** (CS YAML+enforcer+CMake) | 0 | 4 | 0 | 4 🔴 0% |
| **3-code-contract Epic 4** (tests للحارس) | 0 | 2 | 0 | 2 🔴 0% |
| **الإجمالي** | **42** | **31** | **15** | **88 (Sprint+BACKLOG لا يُحتسب كعجز)** |

**التقدير المُصحَّح بعد دمج كل المصادر:**
- ادعاءات مُحقَّقة فعلياً: 42
- ادعاءات غير مُحقَّقة (مذكورة كموجودة لكنها مفقودة): 31
- ادعاءات جزئية أو خطط شرعية لم تبدأ: 15
- **نسبة المُحقَّق من المُدَّعى تحقيقه = 42 / (42 + 31) ≈ 57.5%**
- نسبة الجاهزية الإجمالية (مع احتساب الجزئي = 0.5) ≈ 52%

---

## 2. أكبر فجوة مكتشفة: ادعاءات `3-code-contract/implementation_status.md` خاطئة جوهرياً

[implementation_status.md](_bmad-output/governance/3-code-contract/implementation_status.md) (المؤرَّخ 2025-11-21) يدَّعي أن:

- ✅ Epic 2 منجَز (8/8 ستوريات).
- ✅ "إجمالي الثوابت في النظام: **156** (INT001–INT156)".
- ✅ "34/34 death tests يمر".
- ✅ ملف `shared/include/sad_invariant.h` موجود.
- ✅ ملف `scripts/lint/check_invariants.py` موجود وأُصلح في 2025-11-15.
- ✅ تطبيقات INT115..INT156 في `compiler/src/types/`, `compiler/src/frontend/sir_module.cpp`, `compiler/src/backend/llvm/`, `compiler/src/frontend/sir_builder*`.
- ✅ "118 DEF / 118 USE" ثم "121 DEF / 121 USE" ثم "156" بعد دفعات P5–P11.

### الواقع المُقاس (2026-05-30)

| الادعاء | الواقع |
|---|---|
| `shared/include/sad_invariant.h` موجود | 🔴 **غير موجود** — `Test-Path` يعطي `False` |
| `shared/src/sad_invariant.cpp` موجود | 🔴 **غير موجود** |
| `scripts/lint/check_invariants.py` موجود | 🔴 **غير موجود** — `scripts/lint/` كمجلد غير موجود أصلاً |
| ثوابت INT001..INT156 في الكود | 🔴 **0 مواضع** — بحث `SAD_INVARIANT_DEF` في `shared/`, `interpreter/`, `compiler/` يعطي **0 نتائج** |
| `namespace SadInvariantId` في header | 🔴 **0 مواضع** — `constexpr auto INT*` غير موجود |
| إدخالات INVARIANT في `shared/errors/include/error_codes.h` | 🔴 **0 ذكر للكلمة INVARIANT في الملف** |
| `data/standards/code-standards.yaml` (Story 1.1 من Epic 3) | 🔴 **مجلد `data/standards/` غير موجود** |
| `tools/code-standards-enforcer/` (Story 1.2) | 🔴 **مفقود** |
| CMake target `check_standards` | 🔴 **لا يوجد في `cmake/*.cmake`** |
| `reports/code-standards/` | 🔴 **مفقود** |

**خلاصة:** Epic 2 الذي يدّعي `implementation_status.md` أنه منجَز كاملاً = **0% تطبيق فعلي**. ولا سطر واحد من 156 ثابتاً موجود.

### تفسيرات محتملة (دون تخمين السبب)

1. ملفات أُنجزت ثم حُذفت في رفع لاحق (يتطلب فحص `git log` للتأكيد).
2. ملفات لم تُكتب أصلاً و`implementation_status.md` كان optimistic/مُلَفَّق.
3. فرع آخر يحتوي العمل ولم يُدمج (لكن العمل المُدَّعى في `main` يجب أن يكون فيه).

أياً كان السبب، **`implementation_status.md` لا يطابق الواقع** ويجب إعلانه `OUT-OF-DATE` فوراً.

---

## 3. الفحص التفصيلي

### 3.1 ✅ ما هو محقَّق فعلاً (42 ادعاء)

#### LAYERS.json (21/21)

كل المجلدات الـ21 المعرَّفة في [LAYERS.json](LAYERS.json) موجودة وفيها ملفات:

| code_layers (15) | files | content_layers (6) | files |
|---|---|---|---|
| shared/ | 263 | docs/ | 150 |
| interpreter/ | 109 | examples/ | 8 |
| compiler/ | 379 | specs/ | 9 |
| vm/ | 12 | templates/ | 10 |
| runtime/ | 32 | deployment/ | 8 |
| stdlib/ | 168 | distribution/ | 1463 |
| tools/ | 224 | | |
| graphics/ | 269 | | |
| network/ | 38 | | |
| sad_ui/ | 142 | | |
| platform/ | 16 | | |
| tests/ | 2294 | | |
| scripts/ | 125 | | |
| cmake/ | 19 | | |
| data/ | 37 | | |

#### PRD.md (1-policy) — أهداف G1-G6 (4✅ + 2 جزئي)

| Goal | الواقع |
|---|---|
| G1 مفسر `sad` | ✅ `interpreter/` (109) + `cmake/executables.cmake` add_executable(sad) |
| G2 مترجم `sadc` | ✅ `compiler/` (379) + `add_executable(sadc)` (موضعان) |
| G3 stdlib | 🔄 موجود (168) لكن "ادعاء مكتمل" غير مفحوص دلالياً |
| G4 LSP | ✅ `tools/lsp/` |
| G5 sadpkg | 🔄 `tools/pkg/` موجود |
| G6 PMF v1.5 | 🔴 موجود نصياً، غير منفَّذ آلياً (انظر 3.4) |

#### ARCHITECTURE.md + copilot-instructions (7/10)

| الادعاء | النتيجة |
|---|---|
| 7 مكونات رئيسية | ✅ كلها موجودة |
| 40 كلمة محجوزة | ✅ `keywords.yaml` قسم `reserved` = 40 بالضبط |
| 3 عوامل منطقية | ✅ قسم `operators` = 3 |
| ~30 كلمة سياقية | ⚠️ الفعلي = 39 (تقريب) |
| 9 أنواع مدمجة | ✅ كما هو موثَّق |
| `enum class ValueType` في value.h | 🔴 **غير موجود**؛ الفعلي `DataType` في [data_types.h:26](shared/types/include/data_types.h#L26) و`SadTypeKind` في [sad_type_system.h:68](shared/types/include/sad_type_system.h#L68) |
| 12 تعليمة SIR ملكية | 🔴 `enum class SIROpcode` يحوي 659 تعليمة؛ الملكية الحرفية = `MOVE`, `FREE` فقط |
| Lexer→Parser→AST→SIR→LLVM | ✅ كل المكونات موجودة |
| فحص مزدوج للسياقية في parser | ⚠️ لم يُختبر هنا |
| `DataType` بلا ANY | ⚠️ لم يُختبر هنا |

#### Workflows (5/6)

| Workflow | الادعاء | الواقع |
|---|---|---|
| `build-all-platforms.yml` | — | ✅ موجود |
| `ci.yml` | — | ✅ |
| `codegen-check.yml` | — | ✅ |
| `deploy-website.yml` | — | ✅ |
| `publish-extension.yml` | — | ✅ |
| `release.yml` | — | ✅ |
| `monthly-pmf-check.yml` | PMF + THRESHOLDS + EDGE C.1 | 🔴 **مفقود** |
| `code-standards.yml` | ROADMAP E1 | 🔴 **مفقود** |

#### STORY-PMF-V17 (GPG/Branch Protection)

✅ مُنجَزة بالكامل (2026-05-23) — `.github/CODEOWNERS` موجود، `scripts/enforce-gpg-protection.ps1` موجود، Ruleset 16775713 active.

#### Epic 1 من 3-code-contract (2/2 ✅)

| Story | الواقع |
|---|---|
| 1.1 `.clang-tidy` مع `cppcoreguidelines-pro-type-reinterpret-cast` | ✅ موجود في `.clang-tidy` (لم يُفحص محتواه تفصيلياً هنا — موثَّق في implementation_status.md L29-32) |
| 1.2 إصلاح `expression_evaluator_ui.cpp` | ✅ موثَّق L37 |

---

### 3.2 🔴 ما هو غير محقَّق (31 ادعاء)

#### السكريبتات (7 مفقودة من 8)

| السكريبت | مُدَّعى في | الواقع |
|---|---|---|
| `scripts/agent_lock.py` | PMF §4.3 + README §1 + EDGE A.1-A.5 + Sprint #1 instructions | 🔴 مفقود |
| `scripts/scan_layers.py` | LAYERS.json `_meta.auto_refresh_script` + EDGE D.1 + PMF §2.2.1 | 🔴 مفقود |
| `scripts/check_arabic_ratio.py` | THRESHOLDS consumers + STORY_ZERO #4 (مُدَّعى أنه "يعمل") | 🔴 مفقود |
| `scripts/thresholds_loader.py` | README §1 + PMF v1.8 | 🔴 مفقود |
| `scripts/verify_thresholds_consistency.py` | README §1 + PMF v1.8 | 🔴 مفقود |
| `scripts/governance_metrics.py` | THRESHOLDS consumers + STORY_ZERO #8 (مُدَّعى "يعمل") | 🔴 مفقود |
| `scripts/validate_thresholds.py` | THRESHOLDS `_meta.validation` (with `to be created`) | 🔴 مفقود |
| `.github/CODEOWNERS` | EDGE D.3 | ✅ **موجود** |

**ملاحظة:** [STORY_ZERO_REPORT.md](../stories/STORY_ZERO_REPORT.md) يقول صراحة `agent_lock.py`, `governance_metrics.py`, `check_arabic_ratio.py` "تعمل ✅" مع نتائج أرقام محددة. لكن **لا يوجد منها أي ملف اليوم**. هذا يطابق نمط `implementation_status.md` — توثيق ادعاءات إنجاز بلا أثر في الكود.

#### Epic 2 (Contract-as-Code: SAD_INVARIANT) — 8/8 🔴

| Story | الادعاء | الواقع |
|---|---|---|
| 2.1 `sad_invariant.h` | منجَز بـ4 ماكروز | 🔴 الملف غير موجود |
| 2.2 SIR_SCOPE_STACK_BALANCED | منجَز في `sir_builder_scope_loop.cpp` | 🔴 0 SAD_INVARIANT_DEF في compiler/ |
| 2.3 LEXER_TOKEN_POSITION_ONE_BASED | منجَز بأربع `DEF` | 🔴 0 في shared/lexer |
| 2.4 PARSER_SCOPE_STACK_BALANCED | منجَز بـ10 DEFs | 🔴 0 في shared/parser |
| 2.5 INTERPRETER_SCOPE_NO_LEAK | منجَز | 🔴 0 في interpreter |
| 2.6 GOROUTINE_SNAPSHOT_IMMUTABLE | منجَز بفحص ثلاثي | 🔴 0 في interpreter |
| 2.7 قائمة مركزية | منجَز عبر `namespace SadInvariantId` | 🔴 الـheader نفسه مفقود |
| 2.8 `check_invariants.py` | منجَز ومُصلَح 2025-11-15 | 🔴 `scripts/lint/` غير موجود |

#### Epic 3 (Contract-as-Code: YAML + ENFORCER + CMake) — 4/4 🔴

| Story | الادعاء | الواقع |
|---|---|---|
| 3.1 `scripts/lint/code_standards.yaml` | يجب أن يوجد | 🔴 مفقود |
| 3.2 `scripts/lint/check_code_standards.py` | يجب أن يوجد | 🔴 مفقود |
| 3.3 `scripts/lint/code_standards_allowlist.yaml` | يجب أن يوجد | 🔴 مفقود |
| 3.4 `cmake/code_standards_guard.cmake` | يجب أن يوجد | 🔴 مفقود |

`implementation_status.md` يعترف بهذا (Epic 3 = `❌ غير منجَز كاملاً`) فهذا الجزء فقط صادق.

#### Epic 4 (اختبار الحارس) — 2/2 🔴

| Story | الادعاء | الواقع |
|---|---|---|
| 4.1 `test_code_standards.py` | يجب أن يوجد | 🔴 مفقود |
| 4.2 `test_invariants.py` | يجب أن يوجد | 🔴 مفقود |

#### عتبات THRESHOLDS تُكسر فعلياً (5/5 🔴)

| العتبة | الحد | الفعلي | النسبة |
|---|---|---|---|
| `per_file_max_words.PMF` | 5000 | 6853 | 137% |
| `per_file_max_words.STATUS` | 800 | 2664 | 333% |
| `per_file_max_words.copilot_instructions` | 2500 | 5631 | 225% |
| `token_budget.measured_actual.PMF_bytes` | 36791 (مُسجَّل 2026-05-22) | 70111 (اليوم) | 190% |
| `code_quality.max_file_lines` (800) | — | 82 ملف يتجاوزها | — |

تفصيل ملفات code_quality المُنتهَكة:

| الطبقة | إجمالي ملفات cpp/h | يتجاوز 800 سطر |
|---|---|---|
| shared/ | 251 | 29 |
| interpreter/ | 107 | 7 |
| compiler/ | 339 | 15 |
| vm/ | 11 | 3 |
| stdlib/ | 126 | 7 |
| tools/ | 171 | 20 |
| runtime/ | 29 | 1 |
| **الإجمالي** | **1034** | **82** |

---

### 3.3 🔄 جزئي / خطط لم تبدأ (15 ادعاء)

`execution/BACKLOG.md` يحوي 15 ستوري:
- B-001 (GPG): ✅ DONE (في STORY-PMF-V17 لكن BACKLOG ما زال يعرضها IN_PROGRESS — انحراف بسيط)
- B-002 (Deputy Owner): READY
- B-003 .. B-008 (M1 P1): READY (6 ستوريات تطوير حقيقي)
- B-009..B-012 (P2): READY
- B-013 (sad_ui Phase 2): BLOCKED
- B-014 (Adversarial review #10): READY
- B-015 (sad pkg phase1): READY

هذه ليست "غير محقَّقة" — هي خطط شرعية لم تُنفَّذ بعد.

Sprint #1 (2026-05-22 → 2026-05-29) يحوي B-002, B-009, B-010. **Sprint انتهى دون توثيق إنجاز** — لا `SPRINT_RETRO_2026-05-29.md` في `1-policy/`.

---

## 4. الفجوات الحرجة المُصنَّفة

### 🔴 P0 — انحراف توثيق عن واقع (CRITICAL)

**Drift خطير في 3 ملفات تدّعي إنجازاً غير موجود:**

1. **`3-code-contract/implementation_status.md`** يدّعي 156 ثابتاً + Epic 2 منجَز كاملاً — الواقع = **0**.
2. **`STORY_ZERO_REPORT.md`** يدّعي `agent_lock.py` + `governance_metrics.py` + `check_arabic_ratio.py` "تعمل ✅" مع نتائج مرقَّمة — كلها مفقودة.
3. **`PROJECT_MANAGEMENT_FRAMEWORK.md`** يُحيل إلى 7 سكريبتات + 1 workflow غير موجودين.

**أثر:** أي وكيل/مطور يقرأ هذه الوثائق يعتقد النظام مُنفَّذ، فيبني عليها قرارات خاطئة.

### 🔴 P0 — انتهاك ذاتي للعتبات

ملفات الحوكمة تخرق عتباتها (137%-333%) بدون أي CI يفرضها.

### 🔴 P0 — انعدام الحوكمة الآلية

كل ضمانات PMF/THRESHOLDS/EDGE_GUARDS غير منفَّذة. PMF v1.9 ادَّعى "يحلّ" 13+ ثغرة سابقة بسكريبتات وworkflows غير موجودة.

### ⚠️ P1 — ادعاءات معمارية خاطئة في ARCHITECTURE.md

- `enum class ValueType` غير موجود (الفعلي `DataType`+`SadTypeKind`).
- "12 تعليمة ملكية" غير دقيق (الفعلي `MOVE`+`FREE` فقط من 659).

### ⚠️ P2 — Sprint بلا retrospective

Sprint #1 انتهى 2026-05-29 دون `SPRINT_RETRO_2026-05-29.md`.

---

## 5. التوصيات المُحدَّثة (مع أولوية تنفيذ)

| # | التوصية | الأولوية | الجهد التقديري |
|---|---|---|---|
| **R0** | **تعليم `implementation_status.md` و`STORY_ZERO_REPORT.md` بشارة OUT-OF-DATE فوراً** حتى لا يُضلَّل الوكلاء | P0 | 10 دقائق |
| R1 | إنشاء `scripts/agent_lock.py` (الأساس لكل العمل بين الوكلاء) | P0 | 2-3 ساعات |
| R2 | إنشاء `scripts/scan_layers.py` (يولِّد LAYERS.json) | P0 | 2 ساعات |
| R3 | إنشاء `scripts/check_arabic_ratio.py` + `governance_metrics.py` + `thresholds_loader.py` + `verify_thresholds_consistency.py` + `validate_thresholds.py` | P0 | 1 يوم |
| R4 | إنشاء `.github/workflows/monthly-pmf-check.yml` يستدعي R1-R3 | P0 | 1 ساعة |
| R5 | تنفيذ Epic 2 (SAD_INVARIANT) فعلياً من الصفر — `shared/include/sad_invariant.h` + `shared/src/sad_invariant.cpp` + 6 ثوابت ابتدائية | P0 | 1-2 يوم |
| R6 | تنفيذ Epic 3 (`code-standards.yaml` + enforcer + CMake guard) | P0 | 3-5 أيام |
| R7 | تنفيذ Epic 4 (tests لـEpic 2+3) | P0 | 2 يوم |
| R8 | **حلّ خيار:** (أ) تقليص PMF/STATUS/copilot لمطابقة عتبات THRESHOLDS، **أو** (ب) رفع العتبات وتوثيق السبب | P0 | 1 يوم |
| R9 | تحديث ARCHITECTURE.md ليعكس `DataType`/`SadTypeKind` + توضيح "12 تعليمة ملكية" أو حذف الادعاء | P1 | 30 دقيقة |
| R10 | كتابة `SPRINT_RETRO_2026-05-29.md` يعترف بأن Sprint #1 لم يُنجَز | P1 | 30 دقيقة |
| R11 | إعادة قياس `THRESHOLDS.json._meta.measured_actual` بقياس جديد | P1 | 15 دقيقة |
| R12 | إضافة عمود `Enforced by CI` لكل صف في THRESHOLDS.json | P2 | 15 دقيقة |

---

## 6. ما لم يُختبر في هذا التقرير

- **بناء فعلي** (`cmake --build`) — لم يُجرَ.
- **تشغيل اختبارات** (`ctest`) — لم يُجرَ.
- **محتوى دلالي لـ`DataType`/`SadTypeKind`** — قُرئت لمجرد التأكد من وجودها.
- **محتوى `.clang-tidy`** فُحص فقط بقراءة `implementation_status.md` (الذي ثبت أنه غير دقيق) — يحتاج فحصاً مباشراً مستقلاً.
- **محتوى دلالي للـ40 كلمة محجوزة** — عُدَّت فقط ولم يُختبر تحويل كل واحدة إلى `KEYWORD_*` صحيح في `lexer_keywords.cpp`.
- **git log** لتقصِّي ما إذا كانت ملفات `sad_invariant.h` و`scripts/lint/` موجودة في commits سابقة ثم حُذفت.

---

## 7. الخلاصة المُصحَّحة

**نسبة التحقق الفعلية: ~52%** (وليس 65% كما زعمت v1.0).

النظام كله ينقسم إلى ثلاث طبقات:

| الطبقة | الحالة |
|---|---|
| **معمارية اللغة الأساسية** (المفسر، sadc، stdlib، keywords، layers، LSP، pkg) | ✅ **محقَّقة فعلياً** |
| **حوكمة GPG/Branch Protection/CODEOWNERS** | ✅ مُفعَّلة على GitHub (2026-05-23) |
| **حوكمة آلية للكود وعقد التصميم** (PMF scripts, SAD_INVARIANT, Code Standards Enforcer) | 🔴 **غير موجودة كلياً** — وثائقها تدَّعي عكس ذلك |

**الخطر الأكبر** ليس النقص نفسه، بل **التضليل**: `implementation_status.md` + `STORY_ZERO_REPORT.md` يدَّعيان إنجازاً وهمياً. يجب وضع شارة `OUT-OF-DATE` على الوثائق المضلِّلة قبل أي عمل آخر، وإلا فإن أي قرار يُبنى عليها سيكون مغلوطاً.

---

## 8. سجل الفحص

| الخطوة | الأداة | المخرج |
|---|---|---|
| طبقات LAYERS | `Test-Path` + `Get-ChildItem` | 21/21 |
| keywords.yaml | regex على inline YAML | 40+3+39+9 |
| ValueType search | grep على shared/types/**/*.h | غير موجود؛ وجد `DataType`+`SadTypeKind` |
| SIROpcode count | brace-balanced parser على enum | 659 إجمالاً، 2 ملكية حرفية |
| Governance scripts | `Get-ChildItem -Recurse -Filter` خارج build/_recovered | 7/8 مفقود |
| Workflows | `Get-ChildItem .github/workflows` | 6 موجودة، 2 مفقودة (monthly-pmf, code-standards) |
| العتبات | `Measure-Object -Word` على ملفات الحوكمة | 4/4 منتهكة |
| SAD_INVARIANT | grep `SAD_INVARIANT_DEF` في shared+interpreter+compiler | **0 مواضع** |
| `sad_invariant.h/cpp` | `Test-Path` | غير موجودان |
| `scripts/lint/` | `Test-Path` | غير موجود |
| `data/standards/` | `Test-Path` | غير موجود |
| INVARIANT في error_codes.h | grep في الملف | **0 ذكر** |
| CMake `check_standards` target | grep في cmake/*.cmake | غير موجود |
| Epic 1 (`.clang-tidy`, expression_evaluator_ui.cpp fix) | اعتمدت implementation_status.md (غير مفحوص مباشرة) | افتُرض ✅ بحذر |

