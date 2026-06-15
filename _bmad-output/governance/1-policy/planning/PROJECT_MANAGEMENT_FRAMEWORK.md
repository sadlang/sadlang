# إطار إدارة مشروع لغة ص — الوثيقة الحاكِمة

> **التاريخ:** 2026-05-22 (يُؤخَذ من ساعة الجهاز عبر `Get-Date` — انظر `date_policy.md`)
> **النسخة:** v1.9.2 (Folder Reorganization — مجلد فرعي `execution/` لملفات طبقة التنفيذ + `README.md` كفهرس للمجلد)
> **المالك المسؤول:** صالح
> **المُساعد المُعتمَد:** أي وكيل ذكاء صناعي مفوَّض من صالح
> **الحالة:** **سارية المفعول**
> **النطاق:** كيفية إدارة مشروع ضخم قائم (brownfield) بدأ بلا خطة، بفريق صغير + 3 وكلاء ذكاء صناعي + مسؤول واحد.
> **مُعرِّف الوثيقة:** PMF-001 (لا يتغيَّر)
> **التحديث القادم المُجدوَل:** 2026-06-21 (دوري شهري — يُفعَّل آلياً عبر [`.github/workflows/monthly-pmf-check.yml`](../../.github/workflows/monthly-pmf-check.yml))
> **مرفقات:** [EDGE_CASE_GUARDS.md](EDGE_CASE_GUARDS.md) — 18 حارساً تقنياً/سياسياً
> **مرجع الطبقات:** [LAYERS.json](LAYERS.json) — المصدر الموثوق الوحيد لقائمة الطبقات (يُحدَّث آلياً بـ [`scripts/scan_layers.py`](../../scripts/scan_layers.py) — v1.7)
> **مرجع الأرقام السحرية:** [THRESHOLDS.json](THRESHOLDS.json) — مصدر وحيد لكل الأرقام (نسب، حدود، جداول) في PMF (v1.7 — يحلّ عقبة #10 من نقد د. كوين). **v1.8:** السكريبتات تقرأ منه فعلياً عبر [`scripts/thresholds_loader.py`](../../scripts/thresholds_loader.py)، والاتساق يُفحص بـ [`scripts/verify_thresholds_consistency.py`](../../scripts/verify_thresholds_consistency.py) (يحلّ ثغرة #13 من نقد Pen-Tester).
> **سياسة التوقيع التقني:** [`.github/BRANCH_PROTECTION_POLICY.md`](../../.github/BRANCH_PROTECTION_POLICY.md) — GPG + Branch Protection (v1.7 — يحلّ عقبة #9). **v1.8:** التفعيل الفعلي يتتبَّع في [STORY-PMF-V17-ENFORCE-GPG.md](STORY-PMF-V17-ENFORCE-GPG.md) (حالة: `PENDING_SALEH_ACTION` — يحلّ ثغرة #11) + فحص آلي في monthly-pmf-check.yml (يحلّ ثغرة #14).
> **قوالب المرحلة 2:** [templates/](templates/) — قوالب جاهزة للملفات الحوكمية المفقودة

---

## 0. كيف تقرأ هذه الوثيقة (للوكلاء)

**أنت وكيل AI تقرأ هذه الوثيقة في بداية الجلسة.**

**الإلزامي (يطابق §4.1 تماماً):** هذا القسم (§0) + القسم 4 (بروتوكول الوكلاء). إجمالي ≤ 2K رمز.

**عند الحاجة فقط** (لا تُقرأ ابتداءً):
- §1: لمَّا تحتاج تبرير قاعدة قبل تجاوزها.
- §2: لمَّا تبدأ مرحلة جديدة من المنظومة.
- §9: لمَّا تشكُّ في تجاوز ميزانية الرموز.
- §3، §5، §6، §7، §8، §10، §11: مراجع موضوعية حسب السياق.

**القاعدة الذهبية:** اقرأ §0 + §4 فقط؛ ابحث في الباقي عند الحاجة. تجاوز هذا يستنزف ميزانية الرموز بلا طائل.

### 0.1 حالة ملفات الجلسة المطلوبة (v1.5)

> **مضاف لمعالجة عقبة #1 من [STORY_ZERO_REPORT.md](STORY_ZERO_REPORT.md).** قبل القراءة، تأكَّد من وجود الملفات:

| الملف | الحالة | إذا غاب |
|---|---|---|
| [.github/copilot-instructions.md](../../.github/copilot-instructions.md) | ✅ موجود | لا تتابع |
| [AGENT_CONTEXT.md](../../AGENT_CONTEXT.md) | ✅ موجود (v1.5) | لا تتابع — أبلِغ صالح |
| [PRD.md](PRD.md) | ✅ موجود (v1.5 stub) | لا تتابع — أبلِغ صالح |
| [ARCHITECTURE.md](ARCHITECTURE.md) | ✅ موجود (v1.5 stub) | لا تتابع — أبلِغ صالح |
| [STATUS.md](../../STATUS.md) | ✅ موجود (v1.5) | شغِّل `python scripts/governance_metrics.py` أولاً |

**FALLBACK (v1.6 — يحلّ عقبة #2 من [CRITIQUE_MURAD_2026-05-22.md](CRITIQUE_MURAD_2026-05-22.md)):** إذا أي ملف مفقود، أوقِف الجلسة فوراً ولا تخمِّن السياق من الكود. أبلِغ صالح وأنشئ الملف من القالب الموازي في [`templates/`](templates/):

| الملف المفقود | القالب الواجب استخدامه |
|---|---|
| `AGENT_CONTEXT.md` | [templates/AGENT_CONTEXT.template.md](templates/AGENT_CONTEXT.template.md) |
| `PRD.md` | [templates/PRD.template.md](templates/PRD.template.md) |
| `ARCHITECTURE.md` | [templates/ARCHITECTURE.template.md](templates/ARCHITECTURE.template.md) |
| `STATUS.md` | [templates/STATUS.template.md](templates/STATUS.template.md) |

قواعد استخدام القوالب موثَّقة في [templates/README.md](templates/README.md).

---

## 1. المشكلة (Problem Statement)

### 1.1 الأعراض المُلاحَظة
1. بدأ المشروع بلا خطة موثَّقة — العمل تراكم سنوات بقرارات شفهية وذاكرة فردية.
2. لا توجد آلية واضحة للعمل في مشروع بحجم 200K+ سطر كود.
3. 3 وكلاء ذكاء صناعي يعملون بالتوازي بلا سياق مشترك — كل وكيل يبدأ من الصفر.
4. المسؤول البشري ينسى الخطة لأنها غير مكتوبة في مكان موحَّد.
5. التوثيق متخلِّف عن الكود — تحديثه يدوياً مستحيل بفريق صغير.
6. خطط متناثرة في `_bmad-output/{codeRolePlan, docplan, error_system, ...}` بدون ربط أو لوحة موحَّدة.

### 1.2 الأسباب الجذرية (Root Causes — مبدأ BF-04)
| العَرَض | السبب الجذري |
|---|---|
| فوضى الخطط | لا يوجد Single Source of Truth (SoT) لحالة المشروع |
| نسيان المسؤول | الذاكرة البشرية لا تتسع — يجب تفريغها للنظام |
| تباعد الوكلاء | لا يوجد بروتوكول بدء جلسة يُجبر كل وكيل على قراءة السياق |
| تخلُّف التوثيق | لا توجد بنية تحتية لتوليد التوثيق من البيانات |
| غياب المنهجية | لا يوجد PRD/Architecture/Epics يحدِّدون "ماذا" و"كيف" و"متى" |

### 1.3 الحلول الجزئية المرفوضة
أربع مقاربات سطحية فشلت لغياب SoT: التوثيق المؤجَّل، الذاكرة الجماعية، قراءة الكود مباشرةً من كل وكيل، وخطط Markdown المتفرِّقة بلا لوحة موحَّدة.

---

## 2. الحل — منظومة 4 مراحل مع تعريف نهاية صارم

### 2.1 المبدأ الحاكِم
> **كل قرار، كل خطة، كل فجوة → مكتوبٌ في مكان واحد، بصيغة قابلة للقراءة آلياً، ومُحدَّث تلقائياً.**

### 2.2 المراحل الأربع

#### **المرحلة 1: التوثيق الواقعي للوضع الحالي (Brownfield Snapshot)**

| البند | التفاصيل |
|---|---|
| **المهارة** | [`bmad-document-project`](../../.github/skills/bmad-document-project/) (نمط `initial_scan`) |
| **المُدخَل** | بنية المشروع الحالية (كود + ملفات + إعدادات) |
| **المُخرَج المُتوقَّع** | `docs/index.md` + `docs/parts/01..NN-*.md` + `_bmad-output/project_knowledge/project-scan-report.json` |
| **تعريف النهاية (DoD)** | (1) `docs/index.md` يحوي روابط لكل الطبقات الرئيسية المُعرَّفة في [LAYERS.json](LAYERS.json) (الحقل `code_layers` — حالياً 15 طبقة، يُحدَّث بستوري منفصلة عند أي إضافة) — (2) كل ملف part يحوي ≥ 300 كلمة، **والنسبة المئوية للأحرف العربية ≥ 70%** (يُقاس بسكربت `scripts/check_arabic_ratio.py` — Story منفصلة) — (3) state file محفوظ — (4) مراجعة بشرية من صالح أو نائبه — (5) commit واحد مع الرسالة `docs(brownfield-scan): initial v1` |
| **القابل للقياس** | عدد الملفات المُولَّدة، إجمالي الكلمات، نسبة طبقات مُغطَّاة |
| **المسؤول التنفيذي** | الوكيل المُكلَّف من صالح + مراجع بشري (صالح أو من ينوب عنه) |
| **مدة التنفيذ** | 1-2 ساعة مسح + 30 دقيقة مراجعة |
| **خطر فشل** | المسح يستنزف رموز كثيرة → خفِّفه بـ `scan_level=normal` بدل `exhaustive` |

#### **المرحلة 2: السياق المُشارَك للوكلاء (Agent Shared Context)**

| البند | التفاصيل |
|---|---|
| **المهارة** | [`bmad-generate-project-context`](../../.github/skills/bmad-generate-project-context/) |
| **المُدخَل** | مخرج المرحلة 1 + STATUS.md + ADRs السابقة |
| **المُخرَج المُتوقَّع** | `_bmad-output/governance/1-policy/AGENT_CONTEXT.md` (≤ 2000 رمز) + تحديث `.github/copilot-instructions.md` |
| **تعريف النهاية (DoD)** | (1) AGENT_CONTEXT.md ≤ 2000 رمز (محسوبة) — (2) يحوي 5 أقسام إلزامية: ملخص الحالة + المخططات النشطة + القرارات المحجوزة + قائمة "لا تفعل" + مفاتيح الاختصارات — (3) `.github/copilot-instructions.md` يحوي قاعدة: "ابدأ بقراءة AGENT_CONTEXT.md" — (4) اختبار: وكيل جديد يفتح جلسة → يجد المرجع خلال ≤ 30 ثانية |
| **القابل للقياس** | حجم الملف بالرموز، عدد الأقسام، زمن اكتشاف الوكيل |
| **المسؤول التنفيذي** | الوكيل + مراجعة صالح |
| **مدة التنفيذ** | 30 دقيقة |
| **خطر فشل** | تضخُّم الملف فوق 2000 رمز → ينقض الغرض. ضع `hard limit` في CI. |

#### **المرحلة 3: تحديد الفجوات بـ PRD + Architecture + Epics**

| البند | التفاصيل |
|---|---|
| **المهارات** | [`bmad-create-prd`](../../.github/skills/bmad-create-prd/) ← [`bmad-create-architecture`](../../.github/skills/bmad-create-architecture/) ← [`bmad-create-epics-and-stories`](../../.github/skills/bmad-create-epics-and-stories/) |
| **المُدخَل** | مخرج المرحلة 1 + رؤية صالح للإصدار التالي |
| **المُخرَج المُتوقَّع** | `_bmad-output/governance/1-policy/{PRD-v1.md, ARCHITECTURE-v1.md, EPICS-v1.md}` |
| **تعريف النهاية (DoD)** | (1) PRD يحوي ≥ 5 مواصفات قابلة للقياس — (2) ARCHITECTURE يحوي ≥ 3 ADRs جديدة مرتبطة بقرارات قائمة — (3) EPICS يحوي 5-10 ملاحم مع 3-7 قصص لكل ملحمة — (4) **بروتوكول التعارض مع الواقع:** كل قصة تُفحص ضد المرحلة 1 — لو الميزة مُنفَّذة جزئياً → تُسجَّل القصة كـ "ترحيل/إكمال" لا "إنشاء" — (5) مراجعة صالح |
| **القابل للقياس** | عدد المواصفات، عدد ADRs، عدد القصص، عدد التعارضات المُحلولة |
| **المسؤول التنفيذي** | الوكيل + صالح (في الحلقة) — **البديل اللامركزي:** إذا صالح غير متاح ≥ 5 أيام → الوكيل يُنتج مسوَّدة مع علامة `PENDING_REVIEW` ويتابع المراحل الأخرى |
| **مدة التنفيذ** | 2-4 ساعات مع صالح، أو 6 ساعات لامركزياً (مع مراجعة لاحقة) |
| **خطر فشل** | تضارب PRD مع الواقع → استخدم [`bmad-correct-course`](../../.github/skills/bmad-correct-course/) لتسجيل التغييرات |

#### **المرحلة 4: التشغيل المُستمر (Continuous Operations)**

##### 4.A الإيقاعات الدورية

| الإيقاع | المهارة | المخرج | المسؤول التنفيذي |
|---|---|---|---|
| **أسبوعي (الأحد)** | [`bmad-sprint-status`](../../.github/skills/bmad-sprint-status/) | تحديث STATUS.md | وكيل مُعيَّن (موزَّع بالدور أسبوعياً) |
| **كل سبرنت (3 أسابيع — مبرَّر أدناه)** | [`bmad-sprint-planning`](../../.github/skills/bmad-sprint-planning/) | قصص السبرنت القادم | صالح + وكيل |
| **نهاية كل مرحلة كبرى** | [`bmad-retrospective`](../../.github/skills/bmad-retrospective/) | تعلُّمات مكتوبة | صالح + كل الوكلاء النشطين |
| **عند كل تغيير معماري** | [`bmad-correct-course`](../../.github/skills/bmad-correct-course/) | ADR جديد | الوكيل المُقترِح + صالح |

**لماذا 3 أسابيع للسبرنت؟** **تقدير أولي قابل للمراجعة** بعد أول سبرنت كامل. الافتراض: متوسط القصص المُغلَقة في المشروع الحالي ≈ 6 قصص/شهر (تقدير غير موثَّق بـ git log — Story منفصلة لقياسه)، أي ≈ 4 قصص في 3 أسابيع. أسبوعان أقصر → overhead عالٍ؛ أربعة أسابيع → نسيان السياق. **آلية المراجعة:** بعد السبرنت الأول، يقيس وكيل sprint-status العدد الفعلي ويُعدِّل في v1.x.    

### 2.3 الخطوة الفورية الموصى بها
**ابدأ بالمرحلة 1** لأنها الأقل خطورةً، والأعلى قيمةً، والأكثر قابليةً للاستئناف. **البديل:** المرحلة 2 إذا كان "نسيان الوكلاء" أكثر إلحاحاً (يمكن صياغة AGENT_CONTEXT.md من STATUS.md الموجود بدون انتظار المسح الكامل).

### 2.2.1 تفصيل المرحلة إلى ستوريات تنفيذية (v1.5 — يعالج عقبتَي #2 و #3)

كل مرحلة من المراحل الأربع تُقسَّم إلى ستوريات صغيرة (story = ملف واحد أو وظيفة واحدة). DoD للستوري الفرعية ≠ DoD للمرحلة.

#### قالب story فرعية للمرحلة 1 (نموذج)

| الحقل | القيمة |
|---|---|
| `STORY-PMF-PHASE1-LAYER-<N>` | اسم الستوري (مثال: STORY-PMF-PHASE1-LAYER-LEXER) |
| المُدخَل | طبقة واحدة من 12 (مثال: `shared/lexer/`) |
| المُخرَج | `docs/parts/<N>-<layer>.md` |
| DoD المُصغَّر (story) | (1) الملف موجود (2) ≥ 300 كلمة (3) ≥ 70% عربية (يُفحص بـ `check_arabic_ratio.py`) (4) commit واحد (5) ذِكر في STATUS.md |
| الحجز | `agent_lock.py acquire --story STORY-PMF-PHASE1-LAYER-<N>` |

#### تطوُّر DoD للمرحلة (تدريجي)

> **v1.7:** الرقم `M` يُقرأ من [LAYERS.json](LAYERS.json) (`totals.M_code_layers`) — لا تُضف أرقاماً ثابتة هنا. **LAYERS.json يُحدَّث آلياً** بواسطة [`scripts/scan_layers.py`](../../scripts/scan_layers.py) (يستدعى في [`monthly-pmf-check.yml`](../../.github/workflows/monthly-pmf-check.yml) ويمنع أي انحراف بين الواقع والملف). حالياً `M = 15`.

| الحالة | الشرط | الوصف |
|---|---|---|
| `NOT_STARTED` | 0/M ستوريات مغلقة | المرحلة لم تبدأ |
| `IN_PROGRESS_PARTIAL(N/M)` | 1 ≤ N ≤ M-1 ستوريات مغلقة | مرحلة جارية. STATUS.md يُحدَّث بـ `N/M` (مع ذكر قيمة M الحالية وتاريخ آخر تحديث لـ LAYERS.json) |
| `COMPLETE` | M/M ستوريات مغلقة + DoD المرحلة كاملاً مُحقَّق | جاهزة لـ retrospective |

**قاعدة:** لا تنتقل من ستوري فرعية إلى التالية قبل تحرير الحجز (`agent_lock.py release`). لا تنتقل من مرحلة إلى التالية قبل بلوغ `COMPLETE`.

##### 4.B التوثيق التلقائي (موجود ✅)
- ✅ Story 5.1 — `check_docs_coverage.py --enforce` في CI
- ✅ Story 4.3 — `--min-tier BASIC` يكسر CI عند انحدار الجودة
- ✅ Story 3.3 — `render_man.py` يُولِّد 34 صفحة troff تلقائياً
- ⏳ Story 5.2 — توسيع pipeline AI lessons

##### 4.C STATUS.md كلوحة موحَّدة (موجود ✅)
- يُحدَّث عند كل إغلاق story.
- قاعدة منع الانحراف: أي تغيير في حالة خطة → سطر جديد في changelog STATUS.md.

---

## 3. البنية التنظيمية للملفات

```
_bmad-output/
├── STATUS.md                              ← ⭐ SoT لكل الخطط
├── management/                            ← ⭐ الإطار الحاكم
│   ├── PROJECT_MANAGEMENT_FRAMEWORK.md   (هذه الوثيقة — PMF-001)
│   ├── AGENT_CONTEXT.md                  (المرحلة 2)
│   ├── PRD-v1.md                         (المرحلة 3)
│   ├── ARCHITECTURE-v1.md                (المرحلة 3)
│   ├── EPICS-v1.md                       (المرحلة 3)
│   ├── AGENT_LOCK.json                   (بروتوكول التضارب — قسم 4.3)
│   └── archive/                          (نسخ ≥ 90 يوم)
├── codeRolePlan/                          ← خطط فرعية
├── docplan/                               ← خطط فرعية
└── ...

docs/
├── index.md                               ← خريطة المشروع (المرحلة 1)
└── parts/
    ├── 01-architecture.md
    ├── 02-lexer-parser.md
    └── ...
```

---

## 4. بروتوكول الوكلاء الثلاثة (Agent Protocol)

### 4.1 قراءة بدء الجلسة الإلزامية (بالترتيب)

| # | الملف | الحد الأقصى المسموح بالرموز | الغرض |
|---|---|---|---|
| 1 | `.github/copilot-instructions.md` | 8K | الأسلوب + 60 قاعدة |
| 2 | `_bmad-output/STATUS.md` | 3K | الخطط النشطة + النِسَب |
| 3 | `_bmad-output/governance/1-policy/AGENT_CONTEXT.md` | 2K | السياق المشترك + "لا تفعل" |
| 4 | PMF-001 — أقسام 0 + 4 فقط | 2K | بروتوكول العمل |
| 5 | ملف الخطة الفرعي ذو الصلة (ADR/Epic/Story) | 5K | تفاصيل المهمة |

**إجمالي الميزانية:**
- **سقف صلب (hard cap):** ≤ 20K رمز — يكسر CI إن تُجووِز.
- **سقف لين (soft cap):** ≤ 18K رمز — تنبيه في CI (warning).
- **عند بلوغ 90% من السقف اللين (16.2K):** يجب فتح Story لإعادة التكثيف قبل التراكم.

> يترك ذلك ≥ 80% من نافذة 128K للعمل الفعلي.

### 4.1.1 القياس الفعلي (v1.6 — يصحِّح عقبة #5)

> **التشخيص:** قياس فعلي بتاريخ 2026-05-22 كشف أن الميزانية مُستنفَدة بل متجاوَزة:

| الملف | البايتات | الكلمات | الرموز التقديرية |
|---|---|---|---|
| `.github/copilot-instructions.md` | 54,945 | 5,629 | ~13,700 |
| `_bmad-output/STATUS.md` | 5,232 | 549 | ~1,300 |
| `AGENT_CONTEXT.md` | 4,202 | 380 | ~1,000 |
| PROJECT_MANAGEMENT_FRAMEWORK.md (كامل) | 36,791 | 3,849 | ~9,200 |
| **الإجمالي إذا قُرئ PMF كاملاً** | **101,170** | **10,407** | **~25,200** ❌ |
| الإجمالي إذا قُرئت أقسام 0+4 فقط من PMF (~25%) | ~70,000 | ~7,400 | **~18,000** ⚠️ |

**النتيجة:** الالتزام الصارم بـ §0 (اقرأ §0+§4 فقط) ضروري وغير اختياري للبقاء تحت السقف اللين. إذا قُرئ PMF كاملاً، يُخرَق السقف الصلب.

**الإجراء الفوري (ستوريات منفصلة):**
1. `STORY-PMF-COMPRESS-COPILOT`: تكثيف `.github/copilot-instructions.md` إلى ≤ 10K رمز (يحتاج اقتطاع ~30%).
2. `STORY-PMF-SPLIT-SECTIONS`: تقسيم PMF بحيث §0+§4 = ملف منفصل صغير (`PMF_BOOT.md` ≤ 2K) وبقية الأقسام في `PMF_DETAIL.md`.

حتى إنجاز هاتين الستوريين، يُطبَّق إجراء طوارئ: تذكير في `AGENT_CONTEXT.md` يقول صراحةً "اقرأ §0+§4 من PMF فقط — لا تقرأ الوثيقة كاملة في بدء الجلسة".    

### 4.2 قاعدة إنهاء الجلسة الإلزامية
عند إنجاز أي story أو إغلاق فجوة:
1. تحديث ملف ADR/Epic الخاص بالخطة (✅ DONE + تاريخ).
2. تحديث STATUS.md (التغيير + الإحصائية).
3. إضافة سطر في changelog STATUS.md.
4. **حذف القيد من `AGENT_LOCK.json`** (انظر 4.3).

### 4.3 بروتوكول التضارب بين الوكلاء (Locking)

**الآلية:** ملف `_bmad-output/governance/1-policy/AGENT_LOCK.json` بتنسيق:
```json
{
  "active_claims": [
    {
      "story_id": "STORY-4.3",
      "agent_name": "amelia",
      "claimed_at": "2025-11-21T10:30:00Z",
      "expires_at": "2025-11-21T14:30:00Z",
      "session_id": "a2805b9b"
    }
  ]
}
```

**شرح الحقول:**
- `story_id`: معرِّف القصة (مطابق لـ EPICS-v1.md).
- `agent_name`: اسم الوكيل (amelia, mary, john, ...).
- `claimed_at`: ISO 8601 UTC — وقت بدء الحجز.
- `expires_at`: ISO 8601 UTC — `claimed_at + 4h` (سياسة TTL).
- `session_id`: أول 8 أحرف من session id الذي يولّده الوكيل (`uuid4().hex[:8]`) — يُستخدم في AUDIT_LOG لربط الحجز بالعمل المُنفَّذ.

**القواعد:**
1. قبل بدء أي story، الوكيل يقرأ `AGENT_LOCK.json` (بقفل ملف نظامي — انظر [EDGE_CASE_GUARDS.md](EDGE_CASE_GUARDS.md) #A.3).
2. إذا الـ story محجوزة وغير منتهية الصلاحية → الوكيل يختار story أخرى.
3. الحجز ينتهي تلقائياً بعد 4 ساعات (تجنُّب الجلسات المنسية).
4. صالح يمكنه إلغاء أي حجز يدوياً (override) — يُسجَّل في `AUDIT_LOG.jsonl` (انظر EDGE_CASE_GUARDS #A.15).
5. **كشف الكتابة الفوقية المتزامنة:** قبل أي write يفحص الوكيل أن `mtime` الحالي للملف = الذي قرأه (read-modify-write مع compare-and-swap). إن لم يتطابق → `merge_required` — على الوكيل قراءة التغييرات الجديدة (`git diff HEAD~1`) ودمج عمل السابق قبل الكتابة. لا يجوز overwrite صامت.

---

## 5. الأمن والمراجعة (Security & Review)

> 📎 للحواجز التقنية التفصيلية (race conditions، JSON corruption، CODEOWNERS، escalation): [EDGE_CASE_GUARDS.md](EDGE_CASE_GUARDS.md)

### 5.1 مخاطر الوكلاء
| المخاطرة | التخفيف |
|---|---|
| تسرُّب أسرار في الكود المولَّد | `gitleaks` في CI + قاعدة "لا API keys في الكود" |
| **مسح وكيل يقرأ ملفات أسرار (.env, secrets/*, *.key, *.pem) ويُضمِّنها في docs/ أو AGENT_CONTEXT.md** | **قائمة exclusion إلزامية في كل سكربت مسح: `.env*`, `secrets/`, `*.key`, `*.pem`, `*.p12`, `*.pfx`, `id_rsa*`, `.aws/`, `.ssh/`. + فحص `gitleaks` على المخرجات قبل commit.** |
| حقن أوامر في الـ tool calls | استخدام `execution_subagent` بدلاً من `run_in_terminal` للأوامر المتعددة + مراجعة بشرية للأوامر التدميرية |
| كتابة كود ضار غير مقصود | مراجعة Pull Request إلزامية قبل merge — لا commits مباشرة على `main` |
| تعديل ملفات الحوكمة (هذه الوثيقة) | تتطلَّب موافقة صالح صراحةً في الـ PR description + [EDGE_CASE_GUARDS](EDGE_CASE_GUARDS.md) #D.13 (CODEOWNERS) |
| تنفيذ مهارات غير مُختبَرة على UTF-8 عربي | اختبار كل مهارة جديدة على عيِّنة عربية أولاً (mini-scan) |

### 5.2 المراجعة قبل الدمج
- كل PR من وكيل → يُراجَع بـ [`bmad-code-review`](../../.github/skills/bmad-code-review/) أو [`bmad-review-adversarial-general`](../../.github/skills/bmad-review-adversarial-general/).
- **شرط استقلالية المراجع:** المراجعة تُشغَّل من **وكيل مختلف** عن صاحب الـPR (الوكيل المُولِّد ≠ الوكيل المراجِع). صالح يُعيِّن المراجِع البديل. لا يجوز للوكيل مراجعة عمله الخاص.
- النتائج تُسجَّل في تعليق PR قبل الدمج.

### 5.2.2 إنفاذ استقلالية المراجِع — v1.7 (يحلّ عقبة #9 من نقد د. كوين)

> **التطور من v1.6:** الإصدار v1.6 اقترح `Agent-Signature: <agent_name>:<sha256(session_id + story_id)>`. كشف د. كوين أن هذا التصميم قابل للتزوير لأن `agent_name` يكتبه الوكيل نفسه، و`session_id` متقلِّب لا يوجد root of trust خلفه. v1.7 يستبدله بإنفاذ تقني حقيقي عبر GPG signing + GitHub Branch Protection.

**الحل المعتمد:**

1. **GPG signing إلزامي** على كل commit يُدمَج في `main`:
   - كل مساهم يُسجِّل GPG key في GitHub.
   - `git config commit.gpgsign true` على جهازه.
   - GitHub يعرض شارة "Verified" على الكوميتات الموقَّعة.

2. **Branch Protection Rule** على `main`:
   - ✅ Require signed commits ← الإنفاذ الجوهري (GitHub يرفض push لكوميت غير موقَّع).

> **تحديث v1.8 (Pen-Tester #11 + #14):** السياسة في `BRANCH_PROTECTION_POLICY.md` ليست كافية بذاتها — يجب أن تكون مُفعَّلة فعلياً على GitHub. الحالة الراهنة موثَّقة في [`STORY-PMF-V17-ENFORCE-GPG.md`](STORY-PMF-V17-ENFORCE-GPG.md) كـ `PENDING_SALEH_ACTION`. خلال الفترة الانتقالية، CODEOWNERS + label `awaiting-saleh-review` يوفِّران الحد الأدنى. كما أُضيف فحص آلي في [`monthly-pmf-check.yml`](../../.github/workflows/monthly-pmf-check.yml) يستخدم `gh api .../branches/main/protection` ويُصدر `::warning::` إن لم يكن GPG مُفعَّلاً.
   - ✅ Require pull request + Code Owners review.
   - ✅ Require status checks: `monthly-pmf-check`.
   - ❌ Allow force pushes / deletions: مُعطَّلتان.

3. **التحقق من استقلالية المراجِع** يصبح ممكناً تقنياً:
   - GitHub API يُعطي `commits[].author.name` و`commits[].verification.verified` لكل commit.
   - سكريبت CI يقارن: `author of all commits` vs `reviewer who approved` — يكسر البناء إذا تطابقا.
   - لا حاجة لاسم وكيل في الـ PR description — Git نفسه يحمل الهوية الموقَّعة.

**القواعد التفصيلية والأوامر:** انظر [`.github/BRANCH_PROTECTION_POLICY.md`](../../.github/BRANCH_PROTECTION_POLICY.md).

**حالة الإنفاذ:**
- **سياسي:** فور دمج PMF v1.7.
- **تقني كامل:** فور قيام صالح بتفعيل Branch Protection في GitHub UI (خطوة يدوية لا يُمكن للسكريبتات تنفيذها لأسباب أمنية).
- **ميزة جوهرية:** القاعدة لا تحتاج وكيلاً يقرأها أو يتذكَّرها — Git/GitHub يرفضان المخالفة تلقائياً. **القاعدة تختفي من Working Memory للوكيل** (تطبيق مبدأ "Constraints over Conventions" من نقد د. كوين).

### 5.2.1 خوارزمية تعيين المراجِع (v1.6 — يصحِّح عقبة #4)

> **التصحيح الجذري:** الإصدار v1.5 طلب القراءة من `AGENT_LOCK.json` "آخر 30 يوم" — لكن هذا الملف يحوي الادعاءات النشطة لحظياً فقط (تُحذف عند التحرير). المصدر الصحيح هو [`AUDIT_LOG.jsonl`](AUDIT_LOG.jsonl) (سجل مُلحَق append-only لا يُحذف منه شيء).

| الحالة | الإجراء |
|---|---|
| ستوري عادية (default) | round-robin بين الوكلاء النشطين ≠ صاحب PR. **مصدر القائمة:** `AUDIT_LOG.jsonl` مع تصفية الأحداث `lock_acquire` و `lock_release` خلال آخر 30 يوم. |
| ستوري حوكمة (PMF/EDGE_CASE_GUARDS/CODEOWNERS/LAYERS.json/THRESHOLDS.json) | صالح حصراً. |
| ستوري أمنية (secrets, crypto, network) | صالح + وكيل واحد إضافي. |
| لا يوجد وكيل بديل متاح | صالح يُعيِّن يدوياً، ويُسجَّل القرار في `_bmad-output/governance/1-policy/REVIEWER_ASSIGNMENTS.jsonl` (يُنشأ عند أول استخدام). |

**القرار:** يُسجَّل في تعليق PR كسطر `Reviewer: <agent> (auto|manual|saleh)`. التحقق من استقلالية المراجِع يتمّ تقنياً عبر GPG (انظر §5.2.2).

---

## 6. النسخ الاحتياطي والتراجع (Backup & Rollback)

### 6.1 النقاط الحرجة المحفوظة بـ Git tags
- `governance-snapshot-YYYY-MM-DD` يُنشأ شهرياً ويشمل كل ملفات `_bmad-output/governance/1-policy/` + `docs/index.md` + `STATUS.md`.
- **توضيح إيقاعات:** الـ"sprint" دورته 3 أسابيع (انظر §2.2 المرحلة 4). بشكل مستقل، توجد مهمة `weekly-status-check` كل أحد (تحديث STATUS.md فقط) — هي ليست سبرنت بل فحص دوري قصير. مسؤولية الـ tag الشهري تقع على وكيل `weekly-status-check` في أول أحد من كل شهر.

### 6.2 آلية التراجع
- لأي ملف حاكم تالف:
  ```powershell
  git restore --source=governance-snapshot-2025-10-21 -- _bmad-output/governance/1-policy/STATUS.md
  ```
- لأي خطة فرعية تالفة: استعادة من commit history مع توثيق السبب في changelog.

### 6.3 الأرشفة طويلة المدى
- الإصدارات الأقدم من 90 يوماً من ملفات الحوكمة → تُنقل إلى `_bmad-output/governance/1-policy/archive/`.
- المسؤولية: وكيل retrospective.

---

## 7. حل تعارض الخطة مع الواقع (Mismatch Resolution)

عندما المرحلة 1 (المسح) تكشف أن ميزة في PRD مُنفَّذة فعلاً لكن بطريقة مختلفة:
1. **لا تُحذف الميزة من PRD** — تُعاد صياغتها كـ "ترحيل" بدل "إنشاء".
2. تُسجَّل القصة بحالة `MIGRATION_NEEDED` في EPICS.
3. تُكتب `correct-course` ADR توضِّح الفرق بين الخطة والواقع وأيهما يفوز.
4. القاعدة الافتراضية: **الواقع يفوز** ما لم يُثبَت أن الخطة أفضل تقنياً.

**معيار "أفضل تقنياً" (rubric كمّي قدر الإمكان):** يُسجَّل القرار في ADR مع درجة لكل بُعد من 1–5:
- **الأداء:** benchmarks (latency, throughput) — قياس فعلي لا تخمين.
- **الأمن:** OWASP Top 10 + threat model — عدد الثغرات المُحتمَلة.
- **الصيانة:** cyclomatic complexity + lines of code + عدد الاعتماديات.
- **التوافقية الخلفية:** regression tests passing rate.
- **تكلفة الترحيل:** ساعات عمل تقديرية × مخاطر.

الخطة "أفضل تقنياً" إذا حصلت على ≥ 3 من 5 أبعاد بفارق ≥ 1 نقطة. وإلا الواقع يفوز.

---

## 8. سياسة التجديد الذاتي

> سجل التغييرات الكامل في القسم 12. سياسة v2.0: عند تراكم ≥ 10 تغييرات جوهرية في v1.x، تُؤرشَف v1 في `archive/PROJECT_MANAGEMENT_FRAMEWORK-v1.md`.

### 8.1 محفِّز تحديث هذه الوثيقة (Self-update Trigger)
الوثيقة تُراجَع/تُحدَّث **تلقائياً** عند أحد الأحداث:
1. **شهرياً** (يوم 21 من كل شهر) → **مُنفَّذ آلياً** عبر [`.github/workflows/monthly-pmf-check.yml`](../../.github/workflows/monthly-pmf-check.yml) (v1.6 — يحلّ عقبة #7). السير يفحص الانحراف الزمني ويفتح Issue تلقائياً إذا تجاوز فارق التواريخ 35 يوماً.
2. **عند تغيير في `.github/copilot-instructions.md`** → فحص اتساق بروتوكول الوكلاء (قسم 4).
3. **عند إضافة مهارة BMAD جديدة** → فحص ما إذا كانت تستحق الإضافة للإطار. **آلية الاكتشاف:** CI scheduled job أسبوعي (الأحد) يحسب `Get-ChildItem .github/skills/ -Directory | Measure-Object` ويقارنه بالقيمة المحفوظة في `_bmad-output/governance/1-policy/.skill_count`. إذا اختلف → يفتح Issue تلقائياً بعنوان `[GOVERNANCE] مهارات جديدة تتطلَّب مراجعة PMF` ويُسنده لصالح.
4. **عند فشل في تطبيق الإطار** (مثل تضارب وكلاء غير محلول) → تحديث القسم المعني فوراً.
5. **عند تغيير في [LAYERS.json](LAYERS.json)** → فحص أن §2.2 و §2.2.1 لا تكرّر الأسماء وتُحيل فقط إلى الملف الخارجي. **يُنفَّذ آلياً عبر [`scripts/scan_layers.py --check`](../../scripts/scan_layers.py)** في workflow الفحص الشهري.
6. **عند تغيير في [THRESHOLDS.json](THRESHOLDS.json)** → أي رقم سحري في PMF يجب أن يُستبدل بإحالة إلى THRESHOLDS.json (v1.7).

**المسؤول:** سير العمل [`monthly-pmf-check.yml`](../../.github/workflows/monthly-pmf-check.yml) ينفِّذ البند #1 آلياً في **اليوم 21 من كل شهر**. صالح يتابع Issues المفتوحة منه.

---

## 9. ميزانية الرموز (Token Budget)

### 9.1 السقوف الصارمة
- **ملفات بدء الجلسة (4 ملفات):** ≤ 20K رمز (انظر §4.1 — سقف صلب + سقف لين 18K). الإجمالي الحالي **غير مُقاس بعد** (يتطلَّب Story منفصلة لتشغيل tiktoken على الأربعة).
- **نافذة السياق الكلية:** 128K — يترك ≥ 80% للعمل الفعلي بعد بدء الجلسة.
- قياس الرموز للنصوص العربية: استخدم `tiktoken` وليس `len/4` — تفاصيل في [EDGE_CASE_GUARDS.md](EDGE_CASE_GUARDS.md) (الفئة B).

### 9.2 قواعد التكثيف
- ملفات بدء الجلسة **لا يجوز** أن تتجاوز 20K مجموعة.
- إذا تجاوزت → إعادة هيكلة (نقل المحتوى إلى ملفات مرجعية تُقرَأ عند الحاجة فقط).
- AGENT_CONTEXT.md يحوي **روابط** لا محتوى مفصَّل.

---

## 10. مقاييس النجاح القابلة للقياس (Measurable Success Metrics)

| المقياس | الوحدة | الهدف | الحالة الحالية | كيف يُقاس |
|---|---|---|---|---|
| نسبة طبقات مُغطَّاة في docs/index.md | % | 100% | 0% | عدد الأقسام / 12 |
| عدد الكلمات الإجمالي في docs/parts/ | كلمة | ≥ 3000 (250 لكل طبقة) | 0 | wc على الملفات |
| حجم AGENT_CONTEXT.md | رمز | ≤ 2000 | غير موجود | `python -c "print(len(open('...').read())//4)"` |
| عمر آخر تحديث لـ STATUS.md | أيام | ≤ 7 | 0 (اليوم) | `git log -1 STATUS.md` |
| تغطية description_ar | % BASIC | 100% | ✅ 100% | `check_docs_coverage.py --enforce` |
| توزيع جودة description_ar (STANDARD+) | % | ≥ 60% | 54% | `check_docs_coverage.py --quality` |
| Story 5.1 enforce فعّال في CI | true/false | true | ✅ true | فحص الـ workflow |
| Sprint planning في آخر 21 يوم | عدد | ≥ 1 | 0 | فحص _bmad-output/ |
| Retrospective في آخر 60 يوم | عدد | ≥ 1 | 0 | فحص _bmad-output/ |
| PRs مدمجة بدون مراجعة | عدد/شهر | 0 | غير مُقاس بعد (Story منفصلة) | gh CLI |
| Governance Git tags في آخر 30 يوم | عدد | ≥ 1 | 0 | `git tag -l "governance-*"` |
| تعارضات AGENT_LOCK المسجَّلة | عدد/شهر | ≤ 2 | 0 | فحص `AUDIT_LOG.jsonl` (حقل `event: lock_conflict`) |

---

## 11. المخاطر المُقَنَّنَة (Risks)

| المخاطرة | الاحتمال | التأثير | التخفيف |
|---|---|---|---|
| المسح الكامل في المرحلة 1 يستنزف رموز | عالٍ | متوسط | تقسيم deep_dive حسب الأولوية |
| الوكلاء يتجاهلون قاعدة قراءة AGENT_CONTEXT.md | متوسط | عالٍ | جعلها أول قاعدة في `.github/copilot-instructions.md` |
| PRD يتغيَّر بسرعة → ARCHITECTURE يتأخَّر | متوسط | عالٍ | `bmad-correct-course` لتسجيل التغييرات بدلاً من إعادة الكتابة |
| التوثيق التلقائي يفشل صامتاً | منخفض | عالٍ | حواجز CI صارمة `--enforce` (موجودة ✅) |
| تشتُّت صالح بين 3 وكلاء | عالٍ | عالٍ | AGENT_LOCK.json + إيقاع أسبوعي مكتوب |
| مهارات BMAD غير مُختبَرة على UTF-8 عربي | متوسط | عالٍ | اختبار mini-scan قبل الاعتماد الكامل |
| تضخُّم ميزانية رموز بدء الجلسة فوق 20K | متوسط | عالٍ | حدّ صارم + اختبار CI يقيس الحجم |
| غياب صالح لأسابيع → توقُّف المرحلة 3 | متوسط | متوسط | البديل اللامركزي (PENDING_REVIEW) في DoD |
| هذه الوثيقة تتقادم وتُهمَل | منخفض | عالٍ | محفِّز التحديث الذاتي (قسم 8.1) |

---

## 11.1 فهرس مهارات BMAD المُستخدمة في الإطار (v1.6)

> **الغرض:** فهرس صريح لكل مهارة مذكورة في PMF مع رابطها المباشر في مجلد المهارات. يُسهِّل على الوكيل العثور على ملف `SKILL.md` بدون بحث.
>
> **المسار الجذر:** `.github/skills/` — كل مهارة في مجلد مستقل يحوي `SKILL.md` + ملفات داعمة.

### مهارات المراحل الأربع

| المرحلة | المهارة | المسار | الغرض |
|---|---|---|---|
| 1 | bmad-document-project | [`.github/skills/bmad-document-project/`](../../.github/skills/bmad-document-project/) | المسح الأولي للمشروع وتوليد `docs/parts/` |
| 2 | bmad-generate-project-context | [`.github/skills/bmad-generate-project-context/`](../../.github/skills/bmad-generate-project-context/) | توليد `AGENT_CONTEXT.md` المكثَّف |
| 3 | bmad-create-prd | [`.github/skills/bmad-create-prd/`](../../.github/skills/bmad-create-prd/) | توليد `PRD.md` |
| 3 | bmad-create-architecture | [`.github/skills/bmad-create-architecture/`](../../.github/skills/bmad-create-architecture/) | توليد `ARCHITECTURE.md` |
| 3 | bmad-create-epics-and-stories | [`.github/skills/bmad-create-epics-and-stories/`](../../.github/skills/bmad-create-epics-and-stories/) | توليد `EPICS.md` |
| 4 | bmad-sprint-status | [`.github/skills/bmad-sprint-status/`](../../.github/skills/bmad-sprint-status/) | تحديث STATUS أسبوعياً |
| 4 | bmad-sprint-planning | [`.github/skills/bmad-sprint-planning/`](../../.github/skills/bmad-sprint-planning/) | تخطيط السبرنت كل 3 أسابيع |
| 4 | bmad-retrospective | [`.github/skills/bmad-retrospective/`](../../.github/skills/bmad-retrospective/) | استخلاص التعلُّمات نهاية كل مرحلة |
| 4 | bmad-correct-course | [`.github/skills/bmad-correct-course/`](../../.github/skills/bmad-correct-course/) | تسجيل تعارض الخطة مع الواقع |

### مهارات المراجعة والجودة

| الغرض | المهارة | المسار |
|---|---|---|
| مراجعة كود قبل دمج PR | bmad-code-review | [`.github/skills/bmad-code-review/`](../../.github/skills/bmad-code-review/) |
| مراجعة عدائية عامة (دور مراد) | bmad-review-adversarial-general | [`.github/skills/bmad-review-adversarial-general/`](../../.github/skills/bmad-review-adversarial-general/) |
| صيد الحالات الحدّية | bmad-review-edge-case-hunter | [`.github/skills/bmad-review-edge-case-hunter/`](../../.github/skills/bmad-review-edge-case-hunter/) |
| مراجعة نثرية | bmad-editorial-review-prose | [`.github/skills/bmad-editorial-review-prose/`](../../.github/skills/bmad-editorial-review-prose/) |
| مراجعة بنيوية | bmad-editorial-review-structure | [`.github/skills/bmad-editorial-review-structure/`](../../.github/skills/bmad-editorial-review-structure/) |

### مهارات التطوير والاختبار

| الغرض | المهارة | المسار |
|---|---|---|
| تنفيذ ستوري تطويرية | bmad-dev-story | [`.github/skills/bmad-dev-story/`](../../.github/skills/bmad-dev-story/) |
| إنشاء ستوري جديدة | bmad-create-story | [`.github/skills/bmad-create-story/`](../../.github/skills/bmad-create-story/) |
| تحرير PRD قائم | bmad-edit-prd | [`.github/skills/bmad-edit-prd/`](../../.github/skills/bmad-edit-prd/) |
| التحقق من جاهزية التنفيذ | bmad-check-implementation-readiness | [`.github/skills/bmad-check-implementation-readiness/`](../../.github/skills/bmad-check-implementation-readiness/) |
| توليد اختبارات شاملة | bmad-qa-generate-e2e-tests | [`.github/skills/bmad-qa-generate-e2e-tests/`](../../.github/skills/bmad-qa-generate-e2e-tests/) |
| فهرسة الوثائق | bmad-index-docs | [`.github/skills/bmad-index-docs/`](../../.github/skills/bmad-index-docs/) |
| تقطيع وثيقة كبيرة | bmad-shard-doc | [`.github/skills/bmad-shard-doc/`](../../.github/skills/bmad-shard-doc/) |

### مهارات شخصيات الوكلاء (BMAD Agent Personas)

> **القاعدة:** كل شخصية وكيل لها مهارتها التي تُعرِّف هويتها وأسلوبها.

| الشخصية | المهارة | المسار | الدور النموذجي في PMF |
|---|---|---|---|
| المحلِّل (mary) | bmad-agent-analyst | [`.github/skills/bmad-agent-analyst/`](../../.github/skills/bmad-agent-analyst/) | Story Zero (محاكاة تنفيذية) |
| المهندس المعماري | bmad-agent-architect | [`.github/skills/bmad-agent-architect/`](../../.github/skills/bmad-agent-architect/) | تأليف ARCHITECTURE.md |
| مدير المنتج (john) | bmad-agent-pm | [`.github/skills/bmad-agent-pm/`](../../.github/skills/bmad-agent-pm/) | تأليف PRD.md |
| المطوِّر (amelia) | bmad-agent-dev | [`.github/skills/bmad-agent-dev/`](../../.github/skills/bmad-agent-dev/) | تنفيذ الستوريات الإنشائية |
| الكاتب التقني | bmad-agent-tech-writer | [`.github/skills/bmad-agent-tech-writer/`](../../.github/skills/bmad-agent-tech-writer/) | تحرير docs/ |
| مصمم تجربة | bmad-agent-ux-designer | [`.github/skills/bmad-agent-ux-designer/`](../../.github/skills/bmad-agent-ux-designer/) | تصميم تدفقات المستخدم |
| البنّاء | bmad-agent-builder | [`.github/skills/bmad-agent-builder/`](../../.github/skills/bmad-agent-builder/) | بناء المهارات نفسها |

### مهارات مساعدة

| الغرض | المهارة | المسار |
|---|---|---|
| المساعدة العامة لـ BMAD | bmad-help | [`.github/skills/bmad-help/`](../../.github/skills/bmad-help/) |
| Brainstorming | bmad-brainstorming | [`.github/skills/bmad-brainstorming/`](../../.github/skills/bmad-brainstorming/) |
| Party mode (نقاش جماعي) | bmad-party-mode | [`.github/skills/bmad-party-mode/`](../../.github/skills/bmad-party-mode/) |
| تقطير معرفي | bmad-distillator | [`.github/skills/bmad-distillator/`](../../.github/skills/bmad-distillator/) |
| التحقق من PRD | bmad-validate-prd | [`.github/skills/bmad-validate-prd/`](../../.github/skills/bmad-validate-prd/) |
| Elicitation متقدم | bmad-advanced-elicitation | [`.github/skills/bmad-advanced-elicitation/`](../../.github/skills/bmad-advanced-elicitation/) |

> **ملاحظة:** المهارات الخاصة بلغة ص (`sad-builtins`، `sad-lang-coding`، `sad-os-coding`) موجودة أيضاً في `.github/skills/` لكنها خارج نطاق الحوكمة (PMF) — تُستخدم أثناء كتابة الكود لا أثناء إدارة المشروع.

---

## 13. بروتوكول الطوارئ — v1.9 (يحلّ ثغرة #16 من نقد Pen-Tester: Bus Factor=1)

> **المشكلة:** كل آليات الإنفاذ في v1.7/v1.8 (CODEOWNERS، Branch Protection، GPG، مراجعة PR) تعتمد على صالح حصراً. إذا كان غير متاح لأسبوع ووُجد CVE حرج → النظام متجمِّد بلا حل.
>
> **مبدأ الحل:** "Single Point of Failure ≠ Single Point of Control" — يجب فصل **التحكم اليومي** عن **الاستجابة الطارئة** عبر Deputy Owner بصلاحيات مُقيَّدة + بروتوكول استدعاء + إلزام post-mortem.

### 13.1 Deputy Owner (المالك النائب)

- **الدور:** شخص واحد يُعيِّنه صالح صراحة، يحمل GPG key مُسجَّل ومُعتمَد في GitHub.
- **الصلاحيات المُمنوحة:**
  - dismiss stale review على PRs (لمنع تجميد PR قديم).
  - merge PR في حالة Emergency فقط (مع override label).
  - تفعيل/تعطيل workflows في حالة CI broken.
- **الصلاحيات الممنوعة:**
  - تعديل CODEOWNERS أو BRANCH_PROTECTION_POLICY.
  - تعديل ملفات `_bmad-output/governance/1-policy/*` بدون موافقة صالح لاحقاً.
  - إضافة/إزالة admins على المستودع.
- **التعيين موثَّق في:** [`.github/DEPUTY_OWNER.md`](../../.github/DEPUTY_OWNER.md) (يُحدَّث ربع سنوياً).

### 13.2 شروط استدعاء بروتوكول الطوارئ

تفعيل البروتوكول مشروط بـ **واحد من** المعايير التالية (وليس "صالح متأخر في الرد"):

| الحالة | المعيار القابل للقياس |
|---|---|
| CVE حرج في تبعية | تصنيف CVSS ≥ 7.0 + استغلال نشط معروف (KEV catalog) |
| تعطل CI كامل | فشل آخر 5 builds متتالية على main لأكثر من 24 ساعة |
| صالح غير متاح | لا رد على mention في PR/Issue لأكثر من 72 ساعة (3 أيام عمل) |
| فقدان مفتاح GPG | إعلان صالح فقدان المفتاح في Issue رسمي |

### 13.3 إجراءات Deputy عند التفعيل

1. **فتح Issue طارئ** بـ label `emergency-protocol-active` يحوي:
   - السبب (أحد المعايير في §13.2).
   - الدليل القابل للتحقق (CVE link، CI run URL، آخر mention timestamp).
   - الإجراء المخطَّط (PR ID، action plan).
2. **PR الإصلاح** يحمل labels: `emergency-override` + `requires-saleh-post-review`.
3. **Merge** عبر Deputy باستخدام GPG key الخاص به (لا يستخدم GPG key صالح).
4. **post-mortem إلزامي** خلال 7 أيام من عودة صالح:
   - تحليل ما إذا كان التفعيل مبرَّراً.
   - مراجعة كل التغييرات (Saleh post-review).
   - تحديث AUDIT_LOG.jsonl + سطر في §12.

### 13.4 سجل الـ Overrides

كل تفعيل يُسجَّل في `_bmad-output/governance/1-policy/EMERGENCY_OVERRIDES.jsonl` (append-only):
```jsonl
{"ts":"2026-MM-DDTHH:MM:SSZ","deputy":"<name>","reason_code":"CVE|CI|UNAVAILABLE|GPG_LOST","reason_detail":"...","pr":"#NNN","post_mortem":"PENDING|<URL>"}
```

سير العمل [`monthly-pmf-check.yml`](../../.github/workflows/monthly-pmf-check.yml) يفحص هذا الملف ويفتح Issue إذا وُجد override بـ `post_mortem=PENDING` لأكثر من 14 يوم.

---

## 14. DoD للنسخة المستقرة — v1.9 (يحلّ ثغرة #17 من نقد Pen-Tester: Recursion Bomb)

> **المشكلة:** النمط v1.5→v1.6→v1.7→v1.8→v1.9 يكشف أن كل نقد جديد يولِّد ستوريات تنفيذ، والستوريات تتراكم نظرياً وقد لا تُنفَّذ فعلياً. لا يوجد معيار "متى نتوقف ونعتبر النسخة مستقرة؟".
>
> **مبدأ الحل:** Recursion needs a base case — يجب تعريف base case قابل للقياس لإيقاف دورة المراجعات وإعلان v2.0 مستقرة.

### 14.1 معايير v2.0 (Stable) — كلها إلزامية

| # | المعيار | القياس |
|---|---|---|
| 1 | تنفيذ كل ستوريات النسخ السابقة | كل ملف في `_bmad-output/governance/1-policy/STORY-PMF-V*.md` بحالة `COMPLETE` (وليس `PENDING_*` أو `IN_PROGRESS`) |
| 2 | استقرار زمني | 30 يوم تقويمي بدون اكتشاف ثغرة بخطورة 🔴 حرجة |
| 3 | مراجعتان عدائيتان نظيفتان | مراجعتان متعاقبتان بشخصيات مختلفة لا تكتشفان أي ثغرة بخطورة ≥ 🟡 متوسطة في **آليات الحوكمة** (الثغرات الإملائية/التحريرية لا تُحتسب) |
| 4 | كل verifier scripts تعمل في CI | `verify_thresholds_consistency.py` + `scan_layers.py --check` + branch protection check = جميعها exit 0 لـ آخر 10 runs متتالية على main |
| 5 | Deputy Owner مُعيَّن وموثَّق | `.github/DEPUTY_OWNER.md` موجود وحديث (≤ 90 يوم) |

### 14.2 منع Recursion Bomb

- بعد ترقية إلى v2.0: **freeze النقد الذاتي لـ 60 يوم** (لا مراجعة عدائية جديدة في هذه الفترة).
- استثناء freeze: ثغرة 🔴 حرجة مكتشفة عبر incident حقيقي (وليس review مُجدول).
- بعد freeze: تُستأنف دورة المراجعات بنفس النظام، لكن أي ترقية لاحقة تُسمَّى v2.1, v2.2, ... وليس v3.0 إلا بتغيير بنيوي جوهري.

### 14.3 ما لا يمنع الترقية إلى v2.0

لتجنُّب recursion bomb عكسي ("لا نصل أبداً لـ v2.0 لأن كل مراجعة تكتشف شيئاً"):

- ثغرات بخطورة 🟢 منخفضة أو تحسينية لا تمنع الترقية.
- ثغرات في وثائق ثانوية (templates, examples) لا تُحتسب.
- ثغرات تتطلَّب إجراء بشري مُعلَّق (مثل `PENDING_SALEH_ACTION`) تُحتسب **فقط إذا** تجاوزت 60 يوم بلا تنفيذ.

### 14.4 الحالة الراهنة (2026-05-22)

- ✅ #4 verifier scripts: مكتمل في v1.8.
- ⏳ #1 ستوريات pending: `STORY-PMF-V17-ENFORCE-GPG` بحالة `PENDING_SALEH_ACTION`.
- ⏳ #2 استقرار زمني: لم يبدأ العدّ بعد (آخر ثغرة حرجة #11 اليوم).
- ⏳ #3 مراجعتان نظيفتان: 0/2.
- ⏳ #5 Deputy Owner: §13 يُعرِّف الإطار، التعيين الفعلي pending.

**التقدير:** v2.0 لا يمكن أن تُعلن قبل 2026-07-22 على الأقل (30 يوم + Deputy + GPG + مراجعتان).

---

## 15. طبقة التنفيذ — Execution Cadence (v1.9 — يكسر فجوة "policy vs. daily work")

> **السبب:** PMF يصف **الحوكمة** (الموافقات، الأقفال، الحدود) لكنه لا يجيب على سؤال "ماذا أعمل اليوم؟". طبقة التنفيذ تربط الحوكمة بالكود اليومي عبر 3 ملفات تشغيلية + إيقاع زمني واضح.

### 15.1 الملفات التشغيلية الثلاثة

| الملف | الدور | تكرار التحديث | المالك |
|------|------|--------------|--------|
| [`ROADMAP.md`](execution/ROADMAP.md) | المعالم الكبرى (M1 v1.0 Beta, M2 v2.0 Stable, M3 v3.0 Production) | عند تغيير معلم — نادر | صالح |
| [`BACKLOG.md`](execution/BACKLOG.md) | كل الستوريات بترتيب أولوية + حالة | عند إنشاء/تغيير حالة ستوري | أي agent |
| [`SPRINT_CURRENT.md`](execution/SPRINT_CURRENT.md) | 3 ستوريات للأسبوع الحالي فقط | أسبوعياً (الاثنين) + Retro (الجمعة) | أي agent |

### 15.2 قواعد الإيقاع الأسبوعي

| اليوم | الإجراء |
|------|---------|
| الاثنين | كتابة `SPRINT_CURRENT.md` جديد (3 ستوريات READY من BACKLOG، P0/P1 فقط) |
| ثلاثاء-خميس | تنفيذ الستوريات مع `agent_lock` لكل واحدة |
| الجمعة | كتابة `SPRINT_RETRO_<التاريخ>.md` (ما أُنجز، ما تأخَّر، الدروس) + تحديث BACKLOG |
| 21 من الشهر | workflow `monthly-pmf-check.yml` يفحص كل شيء آلياً (موجود من v1.7) |

### 15.3 قواعد ROADMAP ↔ BACKLOG ↔ SPRINT

1. **التتبُّع الهرمي:** كل ستوري في BACKLOG مرتبطة بمعلم واحد فقط في ROADMAP.
2. **معيار السحب إلى Sprint:** P0 أو P1 + `READY` فقط. P2/P3 تنتظر.
3. **حد أقصى 3 ستوريات/Sprint:** لكسر "Analysis Paralysis" وتركيز التنفيذ.
4. **لا حذف بدون PR:** تعديل ROADMAP يتطلَّب مراجعة صالح (CODEOWNERS يغطي `_bmad-output/governance/1-policy/*`).
5. **تحديث الذاكرة:** عند إغلاق ستوري كبيرة، يُحدَّث ملف ذات صلة في `/memories/repo/`.

### 15.4 معايير "نقطة البداية" (لمن يدخل المشروع لأول مرة)

تسلسل القراءة المُوصى به:
1. [`ROADMAP.md`](execution/ROADMAP.md) — فهم الصورة الكبرى (5 دقائق)
2. [`SPRINT_CURRENT.md`](execution/SPRINT_CURRENT.md) — معرفة ما يحدث الآن (دقيقتان)
3. [`BACKLOG.md`](execution/BACKLOG.md) — استكشاف الستوريات المتاحة (10 دقائق)
4. [`PMF`](PROJECT_MANAGEMENT_FRAMEWORK.md) — قواعد الحوكمة عند الحاجة فقط (مرجع)

### 15.5 الحماية من فقدان السيطرة (مبدأ TOC — Theory of Constraints)

- **القيد قبل v1.9 (طبقة التنفيذ):** عدم وجود "نقطة دخول واحدة" تربط 60 ذاكرة + 17 ملف management + ستوريات متفرقة.
- **الحل:** ROADMAP (الإجابة الاستراتيجية) → BACKLOG (الإجابة التكتيكية) → SPRINT (الإجابة التشغيلية).
- **الفحص:** إذا فتح أي agent المشروع ولم يستطع الإجابة على "ماذا أعمل الآن؟" خلال 5 دقائق → فجوة في طبقة التنفيذ يجب علاجها.

---

## 12. سجل التغييرات

| التاريخ (ISO) | الإصدار | التغيير | الكاتب |
|---|---|---|---|
| 2025-11-21T10:00:00Z | v1.0 | إنشاء + معالجة 15 نتيجة مراجعة عدائية (DoD، مقاييس، AGENT_LOCK، أمن، نسخ احتياطي، تكلفة رموز، تعارض PRD، إصلاح إملاء `managment`، محفِّز تحديث ذاتي) | Amelia |
| 2025-11-21T13:00:00Z | v1.1 | إضافة 18 حالة حدية اكتشفتها edge-case-hunter (race conditions، UTF-8 tiktoken، CODEOWNERS، JSON corruption، escalation) | Amelia |
| 2025-11-21T15:00:00Z | v1.2 | تطبيق 7 توصيات بنيوية (editorial-review-structure): دمج سجل الإصدارات، استخراج Edge Case Guards لملف منفصل، تكثيف القسم 1.3 و9.1، نقل الخطوة التالية لـ §2، حذف تكرار في §4.3، توضيح مقياس PRs | Amelia |
| 2025-11-21T16:00:00Z | v1.3 | تطبيق 8 تعديلات نثرية (editorial-review-prose): إصلاح مرجع §8.3 → §8.1، حذف تكرار سياسة v2.0، تصحيح إملاء/توازي، توحيد المصطلحات، إزالة emoji تحذيري | Amelia |
| 2025-11-21T17:00:00Z | v1.4 | معالجة 14 إشكالاً من مراجعة عدائية ثانية (bmad-review-adversarial-general): اتساق §0/§4.1، معيار DoD قابل للقياس (≥70% أحرف عربية)، هامش ميزانية (soft 18K / hard 20K)، حذف رقم ~15K المزيَّف، توضيح مبرِّر السبرنت 3أ كتقدير قابل للمراجعة، شرح حقول AGENT_LOCK + كشف overwrite، حسم تضارب cadence (أسبوعي vs 3أ)، secrets exclusion، استقلالية المراجع، rubric كمّي لـ"أفضل تقنياً"، آلية اكتشاف skills، مقياس AGENT_LOCK، توحيد عتبة Sprint إلى 21 يوم، churn note، فجوة فرض تقني (CODEOWNERS Story) | Amelia |
| 2025-11-21T19:00:00Z | v1.5 | معالجة 5 عقبات تنفيذية اكتشفتها Story Zero (تنفيذ محاكاة كاملة لـ PMF v1.4 بشخصية mary): إضافة §0.1 (حالة ملفات الجلسة + FALLBACK)، §2.2.1 (تفصيل المرحلة إلى ستوريات + DoD تدريجي IN_PROGRESS_PARTIAL/COMPLETE)، §5.2.1 (خوارزمية تعيين المراجِع: round-robin + REVIEWER_ASSIGNMENTS.jsonl)، إنشاء stubs أوّلية (STATUS.md، AGENT_CONTEXT.md، PRD.md، ARCHITECTURE.md). المرجع: [STORY_ZERO_REPORT.md](STORY_ZERO_REPORT.md). | Amelia |
| 2026-05-22T07:15:00Z | v1.6 | معالجة 7 عقبات اكتشفتها المراجعة العدائية #7 بشخصية مراد. المرجع: [CRITIQUE_MURAD_2026-05-22.md](CRITIQUE_MURAD_2026-05-22.md). التغييرات: تصحيح زمني، §0.1 FALLBACK + 4 قوالب، استبدال "12" بـ LAYERS.json، §5.2.1 → AUDIT_LOG.jsonl، §4.1.1 قياس ميزانية، §5.2.2 Agent-Signature، monthly-pmf-check.yml. + ربط مهارات `.github/skills/` لكل مهارة مذكورة + فهرس §11.1. | Amelia (تنفيذ نقد مراد) |
| 2026-05-22T07:45:00Z | v1.7 | معالجة 3 عقبات نظمية اكتشفتها المراجعة #8 بشخصية د. كوين (Master Problem Solver). التغييرات الجذرية: **(عقبة #8)** إنشاء [`scripts/scan_layers.py`](../../scripts/scan_layers.py) لتحديث LAYERS.json آلياً ومنع أي تضارب مع مسح `bmad-document-project` + ربطه في monthly-pmf-check.yml. **(عقبة #9)** استبدال Agent-Signature (hash يدوي قابل للتزوير) بـ GPG signing + Branch Protection — إنشاء [`.github/BRANCH_PROTECTION_POLICY.md`](../../.github/BRANCH_PROTECTION_POLICY.md) وإعادة كتابة §5.2.2 بالكامل. **(عقبة #10)** إنشاء [`THRESHOLDS.json`](THRESHOLDS.json) كمصدر وحيد لكل الأرقام السحرية في PMF (نسب، ميزانية، جدولة، حوكمة، جودة) لمنع تكرار خطأ "12 طبقة" بأشكال أخرى. مبدأ التصميم: **Constraints over Conventions** — كل قاعدة تتحول إلى أداة أو فحص آلي بدل قاعدة نصية يحفظها الوكيل. | Quinn (تنفيذ نقد د. كوين) |
| 2026-05-22T08:30:00Z | v1.8 | معالجة 5 ثغرات اكتشفتها مراجعة الاختراق #9 بشخصية Pen-Tester. المرجع: [CRITIQUE_PENTESTER_2026-05-22.md](CRITIQUE_PENTESTER_2026-05-22.md). التغييرات: **(#11)** إنشاء [STORY-PMF-V17-ENFORCE-GPG.md](STORY-PMF-V17-ENFORCE-GPG.md) لتتبُّع التفعيل الفعلي لـ Branch Protection (PENDING_SALEH_ACTION) + سياسة احتياطية. **(#12)** إضافة `discover_stray_sources()` في scan_layers.py للتحذير من ملفات مصدر شاردة في `_scratch/` و`_proj_pdf/`. **(#13)** إنشاء [`scripts/thresholds_loader.py`](../../scripts/thresholds_loader.py) كقارئ موحَّد + ربطه في `check_arabic_ratio.py` و`agent_lock.py` + إنشاء [`scripts/verify_thresholds_consistency.py`](../../scripts/verify_thresholds_consistency.py) للتدقيق الآلي. **(#14)** إضافة step في monthly-pmf-check.yml يستخدم `gh api .../branches/main/protection` لفحص `required_signatures.enabled` + استدعاء verify_thresholds_consistency. **(#15)** إصلاح agent_lock.py: إضافة `--ttl-hours` flag + قراءة `schedule.lock_ttl_hours` من THRESHOLDS.json بدل hardcode. الدرس المستفاد: **Policy ≠ Reality** — أي وثيقة سياسة بلا أداة تنفيذ = نظرية. | PenTester-Fixer (تنفيذ نقد Pen-Tester) |
| 2026-05-22T11:00:00Z | v1.9.2 | **إعادة تنظيم المجلد (Folder Reorganization)** — استجابةً لطلب المالك بتنظيم `_bmad-output/governance/1-policy/` الفوضوي: **(1)** إنشاء مجلد فرعي `execution/` ونقل [`ROADMAP.md`](execution/ROADMAP.md) و [`BACKLOG.md`](execution/BACKLOG.md) و [`SPRINT_CURRENT.md`](execution/SPRINT_CURRENT.md) إليه. **(2)** إنشاء [`README.md`](README.md) كفهرس للمجلد يُصنِّف الـ 17 ملفاً منطقياً إلى 7 فئات (حاكِم، تنفيذ، مرجع، ستوريات مستقلة، مراجعات عدائية، حالة وقت تشغيل، قوالب). **(3)** لم تُنقل ملفات أخرى عمداً لتفادي كسر 119 مرجعاً مشفَّراً (hardcoded) في 5 سكريبتات Python + workflow شهري. **(4)** تحديث الروابط الداخلية في 3 ملفات منقولة + PMF نفسه لإضافة بادئات `../` و `execution/` حسب الاتجاه. السياسات والعتبات لم تتغيَّر. |
| 2026-05-22T10:00:00Z | v1.9.1 | **طبقة التنفيذ (Execution Layer)** — معالجة فجوة "Policy ≠ Daily Work" المكتشَفة من د. كوين عبر منهجية TOC. التغييرات: **(§15 جديد)** "Execution Cadence" يُعرِّف ROADMAP + BACKLOG + SPRINT_CURRENT كملفات تشغيلية إلزامية + إيقاع أسبوعي (الاثنين Sprint جديد، الجمعة Retro) + قواعد التتبُّع الهرمي. **ملفات جديدة:** [`ROADMAP.md`](execution/ROADMAP.md) (3 معالم M1/M2/M3) + [`BACKLOG.md`](execution/BACKLOG.md) (15 ستوري بأولويات P0-P3) + [`SPRINT_CURRENT.md`](execution/SPRINT_CURRENT.md) (3 ستوريات لأسبوع 2026-05-22). **المبدأ المُضاف:** **Policy ≠ Daily Work** — الحوكمة وحدها لا تُجيب على "ماذا أعمل اليوم؟". تطبيق Theory of Constraints لكسر القيد الأساسي (شتات المعرفة). | Execution-Architect (د. كوين) |
| 2026-05-22T09:00:00Z | v1.9 | معالجة الثغرتين المتبقيتين #16 و #17 من نقد Pen-Tester (Resilience Pass). **(#16 Bus Factor=1)** إضافة **§13 بروتوكول الطوارئ**: Deputy Owner بصلاحيات مُقيَّدة + 4 شروط استدعاء قابلة للقياس (CVSS≥7.0، 5 builds فاشلة، 72h بلا رد، فقدان GPG) + سجل append-only في [`EMERGENCY_OVERRIDES.jsonl`](EMERGENCY_OVERRIDES.jsonl) + post-mortem إلزامي خلال 7 أيام. إنشاء قالب [`.github/DEPUTY_OWNER.md`](../../.github/DEPUTY_OWNER.md) جاهز للتعبئة. **(#17 Recursion Bomb)** إضافة **§14 DoD النسخة المستقرة**: 5 معايير إلزامية لإعلان v2.0 (ستوريات COMPLETE + 30 يوم استقرار + مراجعتان نظيفتان + verifier scripts تعمل + Deputy مُعيَّن) + freeze النقد الذاتي لـ 60 يوم بعد v2.0 + استثناءات تمنع recursion عكسي. **(تعزيز CI)** إضافة step في workflow لفحص EMERGENCY_OVERRIDES المعلَّقة > 14 يوم + فحص حداثة DEPUTY_OWNER.md (≤ 90 يوم). المبدأ المُضاف: **Single Point of Failure ≠ Single Point of Control** + **Recursion needs a base case**. | Resilience-Fixer |

> **سياسة الإصدارات** موضَّحة في القسم 8.
>
> **ملاحظة على churn:** المراجعات المتعدِّدة في 2025-11-21 (v1.0→v1.5) كانت مرحلة **تأسيس** الوثيقة في يوم واحد، ولا تُحتسَب في عدّاد سياسة v2.0. الترقيات إلى v1.6, v1.7, v1.8, v1.9 بتاريخ 2026-05-22 هي **تعديلات تشغيلية** متتالية (3 نقود عدائية متعاقبة + resilience pass لمعالجة ثغرتين متبقيتين)، وتُحتسَب معاً كتعديل **مُركَّب** واحد في عدّاد v2.0.

---

**نهاية الوثيقة الحاكِمة PMF-001 v1.9.2. أي تعديل عليها يتطلَّب موافقة صالح في الـ Pull Request.**

> **الفرض التقني (v1.9.2):**
> - ملف [`.github/CODEOWNERS`](../../.github/CODEOWNERS) يضمن أن أي PR يلمس `_bmad-output/governance/1-policy/*` يتطلَّب مراجعة صالح إلزامياً.
> - [`.github/BRANCH_PROTECTION_POLICY.md`](../../.github/BRANCH_PROTECTION_POLICY.md) — GPG signing + Branch Protection يُلغيان الحاجة لـ Agent-Signature اليدوي.
> - التفعيل الفعلي لسياسة GPG/BP يُتتبَّع في [`STORY-PMF-V17-ENFORCE-GPG.md`](STORY-PMF-V17-ENFORCE-GPG.md) ويُفحص آلياً في monthly-pmf-check.yml.
> - **v1.9:** [§13 بروتوكول الطوارئ](#13-بروتوكول-الطوارئ--v19-يحل-ثغرة-16-من-نقد-pen-tester-bus-factor1) + قالب [`.github/DEPUTY_OWNER.md`](../../.github/DEPUTY_OWNER.md) + سجل [`EMERGENCY_OVERRIDES.jsonl`](EMERGENCY_OVERRIDES.jsonl) يكسرون Bus Factor=1.
> - **v1.9:** [§14 DoD النسخة المستقرة](#14-dod-للنسخة-المستقرة--v19-يحل-ثغرة-17-من-نقد-pen-tester-recursion-bomb) يُعرِّف base case واضح لإيقاف دورة المراجعات وإعلان v2.0.
> - **v1.9.1:** [§15 طبقة التنفيذ](#15-طبقة-التنفيذ--execution-cadence-v19--يكسر-فجوة-policy-vs-daily-work) + [`ROADMAP.md`](execution/ROADMAP.md) + [`BACKLOG.md`](execution/BACKLOG.md) + [`SPRINT_CURRENT.md`](execution/SPRINT_CURRENT.md) يكسرون فجوة "ماذا أعمل اليوم؟".
> - سير العمل [`.github/workflows/monthly-pmf-check.yml`](../../.github/workflows/monthly-pmf-check.yml) يفعِّل البند #1 من §8.1 آلياً يوم 21 من كل شهر، ويستدعي `scan_layers.py --check`، يتحقَّق من THRESHOLDS.json، يستدعي `verify_thresholds_consistency.py`، يفحص Branch Protection على main، يفحص EMERGENCY_OVERRIDES المعلَّقة > 14 يوم، ويفحص حداثة DEPUTY_OWNER.md.
> - [`scripts/scan_layers.py`](../../scripts/scan_layers.py) يُحدِّث LAYERS.json آلياً ويرفض أي انحراف عن الواقع + يُحذِّر من ملفات مصدر شاردة.
> - [`THRESHOLDS.json`](THRESHOLDS.json) مصدر وحيد للأرقام السحرية، السكريبتات تقرأ منه عبر [`scripts/thresholds_loader.py`](../../scripts/thresholds_loader.py) والاتساق يُفحص بـ [`scripts/verify_thresholds_consistency.py`](../../scripts/verify_thresholds_consistency.py).
> - [`scripts/agent_lock.py`](../../scripts/agent_lock.py) يقبل `--ttl-hours` ويقرأ القيمة الافتراضية من THRESHOLDS.json (`schedule.lock_ttl_hours`).
>
> **مبادئ التصميم الجوهرية:**
> - **Constraints over Conventions** (د. كوين) — كل قاعدة سياسية تُستبدل بأداة تقنية تجعل المخالفة مستحيلة.
> - **Policy ≠ Reality** (Pen-Tester v1.8) — لكل ملف سياسة يجب أن يوجد قارئ runtime + verifier + خطوة CI.
> - **Single Point of Failure ≠ Single Point of Control** (v1.9 #16) — الإنفاذ المركَّز لا يعني انعدام البديل في الطوارئ.
> - **Recursion needs a base case** (v1.9 #17) — كل دورة مراجعة ذاتية يجب أن تُعرَّف لها شروط توقف قابلة للقياس.
> - **Policy ≠ Daily Work** (v1.9.1, د. كوين) — الحوكمة وحدها لا تخبر أحداً ماذا يعمل اليوم. لا بد من طبقة تنفيذ (ROADMAP/BACKLOG/SPRINT) تربط السياسة بالكود.
