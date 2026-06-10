---
title: "خارطة الطريق لنظام التوثيق الحي"
type: roadmap
parent_system: living-documentation
date: 2026-06-01
status: SUPERSEDED
outOfDateSince: 2026-06-02
outOfDateReason: "دُمجت في IMPLEMENTATION_PLAN.md الجديد كوثيقة تَنفيذ قانونية واحدة."
supersededBy:
  - ../IMPLEMENTATION_PLAN.md
horizon: "Q3 2026 → Q1 2027 (تاريخي)"
related_adrs:
  - ADR-RESTRUCTURE-2026-06-01
  - ADR-SADINFO-ARCHITECTURE
  - ADR-DOCS-CANONICAL-2026-06-02
---

> ⚠️ **هذه الوثيقة مُؤرشَفة (SUPERSEDED)**. خَريطة الطريق المُعتمَدة الآن في [`IMPLEMENTATION_PLAN.md`](../IMPLEMENTATION_PLAN.md). محتوى هذه الوثيقة للرجوع التاريخي فقط.

# 🗺️ خارطة طريق نظام التوثيق الحي (LIVING_DOCS_ROADMAP)

> خارطة طريق شاملة عبر الطبقات الثلاث لإيصال `sadinfo` إلى الإنتاج، وربطه بالموقع والـLSP والامتداد.

---

## 🎯 الرؤية (1 سنة)

> **بحلول Q2 2027:** كل قطعة معلومة عن لغة ص (كلمات محجوزة، دوال مدمجة، رسائل أخطاء، أمثلة، أنماط) تَعيش في **YAML SoT واحد**، تَستهلكها 5 أدوات (الموقع، LSP، VS Code Extension، الذكاء الاصطناعي، CI) عبر بروتوكول `sadinfo` الذي يُولِّد JSON IR موَقَّع بـMerkle hash.

---

## 📊 الحالة الحالية (2026-06-01)

| المكوّن | الحالة | الأدلة |
|---|---|---|
| **الطبقة 1 (Strategy)** | ✅ كاملة | PRD + Architecture v2 + 13 ستوري معمارية + UX + Testing |
| **الطبقة 2 (Architecture)** | ✅ كاملة | Unified Schema + 6 ADRs + 12 ستوري معمارية |
| **الطبقة 3 (Implementation)** | 🟡 جزئية | 24 ستوري معرَّفة، **0 منفَّذة** فعلياً (S-000a..S-016) |
| **الـYAML SoT** | 🔴 غير موجود | لا ملفات في `data/_schemas/` بعد |
| **CLI `sadinfo`** | 🔴 غير موجود | لا برنامج تنفيذي |
| **تكامل الموقع** | 🔴 غير موجود | الموقع يَستخدم Markdown يَدوي |
| **تكامل LSP** | 🟡 جزئي | LSP يَستخدم runtime memory فقط |

> **الخلاصة:** التَخطيط مكتمل بنسبة عالية، التَنفيذ صفر.

---

## 🚀 المراحل (Milestones)

### **M1 — أساس البيانات (Foundation)** | 2026-06-02 → 2026-06-30 | 4 أسابيع

**الهدف:** إيجاد ملفات `data/_schemas/*.yaml` فعلية للكلمات الـ40 المحجوزة + 21 دالة مدمجة.

| الستوري | المسؤول | التَقدير |
|---|---|---|
| [S-000a — Foundation Schemas](3-implementation/stories/S-000a-foundation-schemas.md) | TBD | 5 أيام |
| [S-001 — Loader PoC](3-implementation/stories/S-001-loader-poc.md) | TBD | 3 أيام |
| [S-002 — Entity View](3-implementation/stories/S-002-entity-view.md) | TBD | 4 أيام |
| [S-015b — Migrate Keywords](3-implementation/stories/S-015b-migrate-keywords.md) | TBD | 3 أيام |
| [S-015a — Migrate Builtins](3-implementation/stories/S-015a-migrate-builtins.md) | TBD | 3 أيام |

**DoD (M1):**
- `data/_schemas/keywords.yaml` يَحتوي 40 كلمة محجوزة + 25 سياقية بصيغة canonical
- `data/_schemas/builtins.yaml` يَحتوي 21 دالة مدمجة
- `sadinfo load --check` يَنجح بدون أخطاء
- VERIFICATION_REPORT M1 يُؤكد ≥ 95% تَطابق مع `shared/lexer/src/lexer_keywords.cpp`

---

### **M2 — التَحقق والتجميع (Validation & Aggregation)** | 2026-07-01 → 2026-07-31 | 4 أسابيع

**الهدف:** بنية تَحققية ثلاثية الطبقات (Tier1/2/3) + Merkle hash + قفل الحالة.

| الستوري | التَقدير |
|---|---|
| [S-003 — Tier1 Validator](3-implementation/stories/S-003-tier1-validator.md) | 4 أيام |
| [S-004 — Hash Strategy](3-implementation/stories/S-004-hash-strategy.md) | 3 أيام |
| [S-005 — Aggregator + Merkle](3-implementation/stories/S-005-aggregator-merkle.md) | 5 أيام |
| [S-007 — State & Lock](3-implementation/stories/S-007-state-and-lock.md) | 3 أيام |
| [S-012 — Tier2 Validator](3-implementation/stories/S-012-tier2-validator.md) | 4 أيام |

**DoD (M2):**
- `sadinfo validate --tier 1` يَنجح
- `sadinfo aggregate --hash merkle` يُنتج `data/_schemas/index.merkle`
- `sadinfo lock acquire/release` تَعمل
- Tier2 (cross-reference) يَكشف ≥ 3 تَناقضات حقيقية

---

### **M3 — المصدِّر والمراقب (Exporter & Watcher)** | 2026-08-01 → 2026-08-31 | 4 أسابيع

**الهدف:** تَصدير JSON IR + مراقبة آنية للتَغييرات.

| الستوري | التَقدير |
|---|---|
| [S-009 — Reader API](3-implementation/stories/S-009-reader-api.md) | 4 أيام |
| [S-009b — Security Hardening](3-implementation/stories/S-009b-security-hardening.md) | 3 أيام |
| [S-010 — Exporter](3-implementation/stories/S-010-exporter.md) | 5 أيام |
| [S-011 — Watcher](3-implementation/stories/S-011-watcher.md) | 4 أيام |
| [S-011b — Watcher (macOS)](3-implementation/stories/S-011b-watcher-macos.md) | 2 أيام |
| [S-008 — SQLite Graph](3-implementation/stories/S-008-sqlite-graph.md) | 4 أيام |

**DoD (M3):**
- `sadinfo export --target json --out site/data/` يُنتج JSON صالحاً
- `sadinfo watch` يَكشف تَغييرات `data/_schemas/` خلال < 200ms
- API `Reader` مَستقر مع 0 CVE في Tier1

---

### **M4 — التَكامل مع الموقع** | 2026-09-01 → 2026-09-30 | 4 أسابيع

**الهدف:** الموقع يَستهلك JSON IR بدلاً من Markdown اليَدوي.

| المهمة | المسؤول |
|---|---|
| ربط `website/src/pages/reference/*` بـ JSON IR | TBD |
| استبدال Markdown اليَدوي لجميع الكلمات المحجوزة | TBD |
| إضافة CI gate يَفشل إن خرج توثيق الموقع عن SoT | TBD |
| [story-2.0-website-move](2-architecture/stories/story-2.0-website-move.md) | متعلَّق بطبقة 2 |

**DoD (M4):**
- صفحة `https://sad-lang.dev/reference/keywords` تَعرض الكلمات الـ40 من JSON IR
- `pnpm build` يَفشل إذا كان `data/_schemas/keywords.yaml` غير صالح

---

### **M5 — تَكامل LSP + VS Code Extension** | 2026-10-01 → 2026-10-31 | 4 أسابيع

**الهدف:** الـLSP يَقرأ من JSON IR (ليس runtime memory).

| المهمة |
|---|
| تَعديل `tools/lsp/` لتَحميل JSON IR عند البدء |
| استبدال runtime keyword list بـJSON load |
| إضافة hover docs من JSON IR |
| [story-3.1-render-lsp](2-architecture/stories/story-3.1-render-lsp.md) |
| إصدار VS Code Extension v0.2 يَستخدم JSON IR |

**DoD (M5):**
- LSP autocompletion يَعرض docs العربية من JSON IR
- إضافة دالة مدمجة جديدة في YAML تَظهر في LSP بدون إعادة بناء

---

### **M6 — الانتقال والإزالة (Migration & Cleanup)** | 2026-11-01 → 2026-11-30 | 4 أسابيع

**الهدف:** إزالة كل المصادر اليَدوية القديمة.

| الستوري | التَقدير |
|---|---|
| [S-014b — CI Pipeline](3-implementation/stories/S-014b-ci-pipeline.md) | 5 أيام |
| [S-015c — Migrate Errors](3-implementation/stories/S-015c-migrate-errors.md) | 4 أيام |
| [S-015d — Migrate Lessons](3-implementation/stories/S-015d-migrate-lessons.md) | 4 أيام |
| [S-015e — MD Generator](3-implementation/stories/S-015e-md-generator.md) | 5 أيام |
| [S-016 — Legacy Removal](3-implementation/stories/S-016-legacy-removal.md) | 5 أيام |

**DoD (M6):**
- 0 ملفات Markdown يَدوية لتوثيق الكلمات/الدوال/الأخطاء
- CI ينجح كلياً مع SoT الموحَّد
- `_bmad-output/systems/error-messages/legacy/` مَحذوف

---

### **M7 — التَوسع (AI + Editors)** | 2026-12-01 → 2027-01-31 | 8 أسابيع

**الهدف:** مَنافذ جديدة (AI training data, JetBrains plugin, شات GPT).

| المهمة |
|---|
| تَصدير JSON IR كـ embeddings للذكاء الاصطناعي |
| JetBrains plugin (مَنفذ ثانٍ) |
| Public REST API لـ `sadinfo` على api.sad-lang.dev |
| [story-5.2-ai-lessons-pipeline](2-architecture/stories/story-5.2-ai-lessons-pipeline.md) |
| [story-5.3-deployment-pipeline](2-architecture/stories/story-5.3-deployment-pipeline.md) |

**DoD (M7):**
- AI agent يُجيب أسئلة لغة ص بـ < 500ms عبر API
- JetBrains plugin v0.1 يَدعم autocompletion

---

## 📈 جدول مُلخَّص

| المعلم | البداية | النهاية | الستوريات | الحالة |
|---|---|---|---|---|
| M1 | 2026-06-02 | 2026-06-30 | 5 | ⏳ مُخطط |
| M2 | 2026-07-01 | 2026-07-31 | 5 | ⏳ مُخطط |
| M3 | 2026-08-01 | 2026-08-31 | 6 | ⏳ مُخطط |
| M4 | 2026-09-01 | 2026-09-30 | 3 | ⏳ مُخطط |
| M5 | 2026-10-01 | 2026-10-31 | 4 | ⏳ مُخطط |
| M6 | 2026-11-01 | 2026-11-30 | 5 | ⏳ مُخطط |
| M7 | 2026-12-01 | 2027-01-31 | 4 | ⏳ مُخطط |
| **المجموع** | **2026-06-02** | **2027-01-31** | **32** | **0% منفَّذ** |

---

## ⚠️ المخاطر المعروفة

| المخاطرة | الاحتمال | الأثر | التَخفيف |
|---|---|---|---|
| تَأخر S-000a يَكسر كل ما بعده | عالٍ | حرج | ابدأ بـS-000a حصراً في Sprint #2 |
| تَزامن مَع تَطوير LLVM/مترجم رئيسي | متوسط | متوسط | استخدام `agent_lock.py` (المطلوب أصلاً في B-001) |
| التَوافق العكسي مَع runtime keyword list | متوسط | عالٍ | اختبار CI يُقارن JSON IR ضد `lexer_keywords.cpp` |
| نقص المسؤولين البشريين | عالٍ | متوسط | اعتماد على Copilot agents مَع PR review |

---

## 🔗 الاعتمادات الحَرجة

```mermaid
graph LR
    M1[M1 Foundation] --> M2[M2 Validation]
    M2 --> M3[M3 Exporter]
    M3 --> M4[M4 Website]
    M3 --> M5[M5 LSP]
    M4 --> M6[M6 Migration]
    M5 --> M6
    M6 --> M7[M7 AI + Editors]
    
    style M1 fill:#ffeb3b,stroke:#f57f17
    style M6 fill:#f44336,stroke:#b71c1c,color:#fff
    style M7 fill:#4caf50,stroke:#1b5e20,color:#fff
```

---

## 📜 مَراجع

- [README.md](README.md) — نَظرة عامة على النظام-الأب
- [UNIFIED_DOCS_ARCHITECTURE.md](UNIFIED_DOCS_ARCHITECTURE.md) — لماذا 3 طبقات؟
- [ADR-RESTRUCTURE-2026-06-01](2-architecture/decisions/ADR-RESTRUCTURE-2026-06-01.md) — قرار الهيكلة
- [ADR-SADINFO-ARCHITECTURE](2-architecture/decisions/ADR-SADINFO-ARCHITECTURE.md) — معمارية `sadinfo`
- [3-implementation/stories/README.md](3-implementation/stories/README.md) — كل الـ24 ستوري تنفيذية
- [SPRINT_2026-05-29_RETRO.md](../../governance/1-policy/sprints/SPRINT_2026-05-29_RETRO.md) — دروس Sprint #1

---

**الإصدار:** v1.0  
**التاريخ:** 2026-06-01  
**التَحديث التالي:** نهاية M1 (2026-06-30) — تَقرير مَدى الالتزام بالخارطة
