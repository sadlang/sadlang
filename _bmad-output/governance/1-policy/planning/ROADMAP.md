# 🗺️ خارطة الطريق — لغة ص (Sad Language)

> **الإصدار:** v1.0 (إنشاء أولي) — 2026-05-22  
> **المالك:** صالح | **النائب:** غير مُعيَّن (انظر [DEPUTY_OWNER.md](../../.github/DEPUTY_OWNER.md))  
> **يُحكمه:** [PMF v1.9](../PROJECT_MANAGEMENT_FRAMEWORK.md) §3 + §14  
> **مرتبط بـ:** [BACKLOG.md](BACKLOG.md) (كل ستوري ↔ معلم واحد) + [SPRINT_CURRENT.md](SPRINT_CURRENT.md) (3 ستوريات/أسبوع)

---

## 🎯 الرؤية الاستراتيجية

لغة ص هي **عائلة من 3 منتجات** (راجع `/memories/repo/sad_three_products_strategy.md`):

| المنتج | الموقع | الدور | المنافس |
|--------|--------|------|---------|
| `sad` (المفسر) | `interpreter/` | Scripting + REPL + تعليم | Python, Ruby |
| `sadc` (المترجم) | `compiler/` + LLVM | Performance + Systems + Kernels | Rust, C++ |
| `sad_vm` (الآلة الافتراضية) | `vm/` | Portability + Bytecode + JIT | JVM, .NET CLR |

**مبدأ صارم:** لا يُحذف أي منتج. الكود المشترك يُستخرج إلى `shared/`.

---

## 📍 المعالم (Milestones)

### 🌱 M1: v1.0 Beta — "اللغة عاملة وموثَّقة"
**المعنى:** الميزات الأساسية كلها تعمل على المفسر والمترجم بنفس الدلالة. لا تراجع في الاختبارات الشاملة. الوثائق مكتملة.

**معايير القبول (DoD):**
- ✅ تنفيذ مزدوج (dual execution) ناجح لـ ≥ 95% من اختبارات `tests/comprehensive/` (~900 اختبار)
- ✅ `sad.exe` و `sadc.exe` يبنيان بدون تحذيرات في Release
- ✅ المرجع الشامل `docs/SAD_LANGUAGE_COMPLETE_REFERENCE.md` يغطي 100% من الكلمات المحجوزة (40) والسياقية
- ✅ المكتبة القياسية: `core/`, `io/`, `math/`, `string/` ثابتة (لا breaking changes)
- ⏳ الميزات قيد العمل: `union_types` (راجع `/memories/repo/union_types_plan.md`)، استكمال `compiler_phase5*` للاستدلال

**التقدير:** Q3-Q4 2026  
**الستوريات المرتبطة:** B-001 إلى B-010 في [BACKLOG.md](BACKLOG.md)

---

### 🌲 M2: v2.0 Stable — "النضوج التشغيلي" (مُعرَّف في [PMF §14](../PROJECT_MANAGEMENT_FRAMEWORK.md))
**المعنى:** اللغة جاهزة للاستخدام الإنتاجي على نطاق واسع، مع حوكمة مستقرة وعدم تراجع لفترة طويلة.

**معايير القبول (نقلاً من PMF §14):**
1. كل ستوريات M1 (v1.0 Beta) = `DONE`
2. 30 يوماً بلا ثغرات حرجة (CVSS ≥ 7.0) منذ آخر مراجعة عدائية
3. مراجعتان عدائيتان متتاليتان "نظيفتان" (لا ثغرة High أو Critical)
4. جميع verifiers (`check_arabic_ratio`, `verify_thresholds_consistency`, `scan_layers`, `monthly-pmf-check`) تعمل بدون فشل في 10 تشغيلات متتالية
5. Deputy Owner مُعيَّن فعلياً في [DEPUTY_OWNER.md](../../.github/DEPUTY_OWNER.md)

**قيد ما بعد v2.0:** freeze على النقد الذاتي 60 يوماً (PMF §14.3).

**التقدير الأدنى:** 2026-07-22 (وفق PMF §14.4)  
**التقدير الواقعي:** Q1 2027

---

### 🌳 M3: v3.0 Production — "الانتشار والربط"
**المعنى:** اللغة تُستخدم في مشاريع حقيقية خارج المؤسسة، مع نظام بيئي ناضج.

**معايير القبول (مقترحة — للنقاش):**
- مدير حزم (`sad pkg`) ينشر إلى مستودع رسمي
- LSP server يدعم 3 محررات على الأقل (VS Code ✅، Vim، Emacs)
- مكتبات `graphics/` و `network/` و `sad_ui/` كلها COMPLETE (راجع memories)
- 10 مشاريع خارجية موثَّقة تستخدم اللغة
- وثائق بـ 3 لغات على الأقل (عربية + إنجليزية + ثالثة)

**التقدير:** غير محدد — يعتمد على وصول v2.0 ثم تبني المجتمع.

---

## 🔄 السلسلة الزمنية المتوقعة

```
2026-05  ───┬─── الآن: PMF v1.9 + طبقة التنفيذ (هذا الملف)
            │
2026-Q3 ───┬─── M1: v1.0 Beta (ستوريات B-001..B-010)
            │
2026-Q4 ───┬─── تحضير v2.0 (verifiers + Deputy + 30 يوم استقرار)
            │
2027-Q1 ───┬─── M2: v2.0 Stable (حسب PMF §14)
            │
2027-Q3 ───┬─── بدء العد لـ M3 (بعد 60 يوم freeze + اعتماد المجتمع)
            │
????     ───┴─── M3: v3.0 Production
```

---

## 🚧 المخاطر والقيود

| الخطر | التأثير | التخفيف |
|------|--------|---------|
| Bus Factor = 1 (صالح) | 🔴 يوقف كل شيء | PMF §13 + DEPUTY_OWNER (v1.9) |
| Recursion Bomb (مراجعات لا تنتهي) | 🟡 يبطئ الإنجاز | PMF §14 (DoD صريح) |
| Drift بين المفسر والمترجم | 🔴 يكسر dual execution | tests/comprehensive + Phase E3 dispatch unification |
| تكدُّس الستوريات | 🟡 تشتت تركيز | SPRINT_CURRENT (3 فقط/أسبوع) |
| ملفات شاردة في `_scratch/`/`_proj_pdf/` | 🟢 ضوضاء | `scan_layers --check` ينبِّه شهرياً |

---

## 🔗 الروابط

- [BACKLOG.md](BACKLOG.md) — قائمة الستوريات الكاملة
- [SPRINT_CURRENT.md](SPRINT_CURRENT.md) — ستوريات الأسبوع
- [PROJECT_MANAGEMENT_FRAMEWORK.md](../PROJECT_MANAGEMENT_FRAMEWORK.md) — الحوكمة
- [PRD.md](../PRD.md) — متطلبات المنتج
- [ARCHITECTURE.md](../ARCHITECTURE.md) — البنية التقنية

---

**نهاية ROADMAP v1.0. أي تعديل على المعالم (M1/M2/M3) أو معايير قبولها يتطلَّب موافقة صالح في PR (وفق CODEOWNERS).**
