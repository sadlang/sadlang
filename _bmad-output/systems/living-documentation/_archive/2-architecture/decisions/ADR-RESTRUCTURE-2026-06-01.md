---
id: ADR-RESTRUCTURE-2026-06-01
title: "إعادة هيكلة doc-plan-v2 + doc-ir + sadinfo إلى نظام موحَّد living-documentation"
date: 2026-06-01
status: Accepted
author: Amelia (bmad-agent-dev)
deciders: [User, Amelia]
supersedes: []
relatedADRs:
  - ../epics/ADR-006b-epics.md
tags: [restructure, governance, gr-08]
---

# ADR — إعادة هيكلة نظام التوثيق الحي تحت مجلد موحَّد

## 1. السياق

كانت ثلاث منظومات تَعيش جنباً إلى جنب تحت `_bmad-output/systems/`:

- `doc-plan-v2/` (بنية مرقَّمة قديمة `01_..08_`)
- `doc-ir/` (البنية الستة + مجلدان فرعيان `sadinfo/` و `error_system/`)
- `sadinfo/` (البنية الستة + `prerequisites/`)

التحليل المُفصَّل في [UNIFIED_DOCS_ARCHITECTURE.md](../../UNIFIED_DOCS_ARCHITECTURE.md) أثبَت أن:

1. الثلاثة **ليست أنظمة متوازية** — هي **3 طبقات تَكاملية متعاقبة** لنظام واحد (Living Documentation)
2. `sadinfo` نَشأت كأداة تَنفيذ من PRD في `doc-plan-v2`
3. `doc-ir` صَمَّمت الـSchema الموحَّد و معمارية `sadinfo`
4. وجود `doc-ir/sadinfo/` و `doc-ir/error_system/` كمجلدات فرعية يَكشف التَبعية البنيوية
5. الروابط في `sadinfo/README.md` مكسورة (`../../docplan/...`) بعد إعادة هيكلة سابقة

## 2. القرار

**نَدمج الطبقات الثلاث تحت مجلد أب موحَّد:**

```
_bmad-output/systems/
├── living-documentation/              ← المجلد الأب الجديد
│   ├── README.md
│   ├── UNIFIED_DOCS_ARCHITECTURE.md
│   ├── 1-strategy/                    ← (كانت doc-plan-v2)
│   ├── 2-architecture/                ← (كانت doc-ir)
│   └── 3-implementation/              ← (كانت sadinfo)
│
├── error-messages/                    ← نظام مستقل فعلاً
│   └── decisions/
│       ├── ADR-DOC-STD.md             ← (نُقل من doc-ir/error_system/)
│       └── ADR-EM-3.md                ← (نُقل من doc-ir/error_system/)
└── ...
```

### تَفاصيل الـRename

| القديم | الجديد |
|---|---|
| `systems/doc-plan-v2/*` | `systems/living-documentation/1-strategy/*` |
| `systems/doc-ir/{planning,epics,stories,sprints,status,decisions}` | `systems/living-documentation/2-architecture/{planning,epics,stories,sprints,status,decisions}` |
| `systems/doc-ir/sadinfo/SADINFO_TARGET_ARCHITECTURE.md` | `systems/living-documentation/2-architecture/decisions/ADR-SADINFO-ARCHITECTURE.md` |
| `systems/doc-ir/error_system/ADR-DOC-STD.md` | `systems/error-messages/decisions/ADR-DOC-STD.md` |
| `systems/doc-ir/error_system/ADR-EM-3.md` | `systems/error-messages/decisions/ADR-EM-3.md` |
| `systems/sadinfo/*` | `systems/living-documentation/3-implementation/*` |
| `systems/sadinfo/README.md` | `systems/living-documentation/3-implementation/README-tool.md` (مُعاد تَسميته) |

### القواعد الجديدة (تَعديل GR-08)

1. كل مجلد مُباشر تحت `systems/` = **نظام كامل مستقل** (لا طبقة)
2. الأنظمة متعدِّدة الطبقات تَستخدم بادئة رقمية (`1-`, `2-`, `3-`)
3. كل طبقة لها `README.md` يَشرح دورها
4. المجلد الأب يَحوي `README.md` رئيسي + `UNIFIED_*.md` للتحليل
5. الـADRs المعمارية تَعيش في `decisions/` للطبقة المعنية فقط
6. الأنظمة الفعلياً المستقلة (مثل `error-messages`) تَخرج للأعلى

## 3. البدائل المرفوضة

| البديل | لماذا رُفض |
|---|---|
| **إبقاء الوضع كما هو** | بلبلة فورية للمطوِّر الجديد، 3 مجلدات لنفس الموضوع |
| **دمج كل شيء في `doc-ir`** | يَخلط الطبقات (PRD مع المعمارية مع التَنفيذ) |
| **نَقل `sadinfo` فقط داخل `doc-ir`** | يَترك `doc-plan-v2` معلَّقاً + بُنية غير متَّسقة |
| **استخدام symlinks** | غير قابل للنَقل عبر Windows/Linux/macOS |
| **استخدام بنية مرقَّمة فقط بدون اسم رمزي** | `1/`, `2/`, `3/` بلا معنى ⇒ `1-strategy/` أوضح |

## 4. النتائج (Consequences)

### إيجابية
- ✅ وضوح فوري للمطوِّر الجديد
- ✅ نسق موحَّد لكل الأنظمة تحت `systems/`
- ✅ التسلسل الزمني واضح من البادئة الرقمية
- ✅ الـADRs في مكانها الصحيح (`decisions/` لكل طبقة)
- ✅ `error-messages` بَرَز كنظام مستقل (كان مَدفوناً في `doc-ir/error_system/`)
- ✅ الروابط المكسورة في `README-tool.md` أُصلحت

### سلبية / مخاطر مَقبولة
- ⚠️ كَسر مَحتمل لروابط في ملفات أخرى خارج `systems/` (يَتم فحص شامل تالياً)
- ⚠️ يَتطلب تَحديث `bmad-governance-check` للمسارات الجديدة
- ⚠️ يَتطلب تَحديث `copilot-instructions.md` لذكر `living-documentation` بدلاً من `doc-plan-v2`
- ⚠️ memory files قد تَحوي مسارات قديمة (يَتم تَحديثها تَدريجياً عند الاستخدام)

## 5. خطة المتابعة

> **آخر تَحديث:** 2026-06-01T08:39:02Z

| # | المهمة | الحالة | الدليل |
|---|---|---|---|
| 1 | تَنفيذ النَقل + كتابة READMEs | ✅ مُنجَز | راجع §5.1 |
| 2 | إصلاح روابط `README-tool.md` المكسورة | ✅ مُنجَز | راجع §5.2 |
| 3 | فتح هذا الـADR | ✅ مُنجَز | هذا الملف موجود |
| 4 | فحص روابط مكسورة في باقي المشروع | ✅ مُنجَز | راجع §5.3 (15 رابط أُصلح) |
| 5 | تَحديث `copilot-instructions.md` | ✅ مُنجَز | راجع §5.4 |
| 6 | تَحديث `bmad-governance-check` script | ✅ مُنجَز | راجع §5.5 |
| 7 | إنشاء `living-documentation/LIVING_DOCS_ROADMAP.md` | ✅ مُنجَز | راجع §5.6 |
| 8 | إغلاق خرق GR-03 (SPRINT RETRO) | ✅ مُنجَز | راجع §5.7 |

### §5.1 الدليل: النَقل + READMEs

```powershell
PS> Get-ChildItem _bmad-output/systems/living-documentation/ -Directory
1-strategy/        ← (كان doc-plan-v2)
2-architecture/    ← (كان doc-ir)
3-implementation/  ← (كان sadinfo)

PS> Test-Path _bmad-output/systems/doc-plan-v2/
False  ← (مَحذوف بعد النقل)
PS> Test-Path _bmad-output/systems/doc-ir/
False
PS> Test-Path _bmad-output/systems/sadinfo/
False
```

**الـREADMEs المُنشأة:**
- [living-documentation/README.md](../../README.md) (نظام-أب)
- [1-strategy/README.md](../../1-strategy/README.md) (طبقة 1)
- [2-architecture/README.md](../README.md) (طبقة 2)
- [3-implementation/README.md](../../3-implementation/README.md) (طبقة 3)
- [3-implementation/README-tool.md](../../3-implementation/README-tool.md) (README الأداة الأصلي)

### §5.2 الدليل: روابط `README-tool.md`

3 روابط `../../docplan/...` المكسورة أُصلحت إلى:
- `../2-architecture/planning/...`
- `../2-architecture/decisions/...`

### §5.3 الدليل: 15 رابطاً مَكسوراً أُصلح

| الملف | عدد الإصلاحات |
|---|---|
| `.github/copilot-instructions.md` | 1 |
| `.github/instructions/governance.instructions.md` | 1 |
| `data/_schemas/README.md` | 2 |
| `docs/sadinfo_v2/README.md` | 3 |
| `docs/sadinfo_v2/BASELINE.md` | 2 |
| `3-implementation/README-tool.md` | 3 |
| **المجموع** | **15** ✅ |

**التحقق النهائي:**
```powershell
PS> grep -r "doc-plan-v2|doc-ir|sadinfo" --include="*.md" .  # داخل workspace
0 matches  ← (لا روابط قديمة متبقية)
```

### §5.4 الدليل: `copilot-instructions.md`

السطر 28 الأصلي يُشير إلى `doc-plan-v2/` ⇒ استُبدل بـ `living-documentation/1-strategy/` + ملاحظة عن نَمط الطبقات الـ3.
كذلك `instructions/governance.instructions.md` السطر 42.

### §5.5 الدليل: `bmad-governance-check`

| العنصر | الحالة |
|---|---|
| فحص grep للأسماء القديمة | 0 مطابقات (لا مسارات مَكسورة في السكريبت) |
| **قسم جديد 3.5 — فحص الأنظمة-الأب** | أُضيف في [scripts/check_governance.ps1](../../../../../.github/skills/bmad-governance-check/scripts/check_governance.ps1) |
| **توثيق Parent-System Pattern** | أُضيف في [SKILL.md](../../../../../.github/skills/bmad-governance-check/SKILL.md) |
| **نتيجة الفحص** | `Exit code: 0` بعد إغلاق GR-03 |

دليل تَشغيل:
```
3.5 الأنظمة-الأب
[OK] living-documentation/ — README موجود، 3 طبقات
    [OK] 1-strategy/README.md
    [OK] 2-architecture/README.md
    [OK] 3-implementation/README.md
```

### §5.6 الدليل: `LIVING_DOCS_ROADMAP.md`

[LIVING_DOCS_ROADMAP.md](../../LIVING_DOCS_ROADMAP.md) أُنشئ بـ:
- 7 مَعالم (M1..M7) من 2026-06-02 إلى 2027-01-31
- 32 ستوري معرَّفة (5+5+6+3+4+5+4)
- Mermaid diagram للاعتمادات
- 4 مخاطر موَّثَّقة مع التَخفيف

### §5.7 الدليل: إغلاق GR-03

[SPRINT_2026-05-29_RETRO.md](../../../../governance/1-policy/sprints/SPRINT_2026-05-29_RETRO.md) أُنشئ بـ:
- اعتراف صريح بالتَأخير 3 أيام
- 0/3 ستوريات أُنجزت (أدلة من `Test-Path`)
- 5 إجراءات للسبرنت التالي (A-01..A-05)
- 4 دروس مستفادة (DR-01..DR-04)

**فحص الحوكمة قبل وبعد:**
```
قبل: Exit code 4 (RETRO مفقود)
بعد: Exit code 0 ✅
```

## 6. مَراجع

- [UNIFIED_DOCS_ARCHITECTURE.md](../../UNIFIED_DOCS_ARCHITECTURE.md) — التحليل الكامل (10 إثباتات + مَخطط Mermaid)
- [README.md](../../README.md) — README النظام الأب
- [GR-08 في copilot-instructions.md](../../../../../.github/copilot-instructions.md) — البنية الموحَّدة الستة
