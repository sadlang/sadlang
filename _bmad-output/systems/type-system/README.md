# نظام الأنواع الموحَّد — type-system

> **النسخة:** v1.0 — مكتمل ومدموج
> **آخر تحديث:** 2026-06-15 (من الجهاز)
> **الحالة:** ✅ القلب المعماريّ مُنفَّذ ومُختبَر ومدموج في `graphic` (`3dc75af4`). 25 commit.

نظام الأنواع الموحَّد للغة ص: تعدادٌ واحد `SadTypeKind` مُولَّد من مصدر الحقيقة
(`language-truth/types.yaml`) يخدم **المفسّر والمترجم معًا**، بديلًا عن نظام `DataType`
القديم المحذوف. يضمّ أساس null-safety (تمايز `عدم`/`فراغ`، `Optional`, عوامل `?./??/؟`)،
وأنواعًا متقدّمة (`Result`, `Future`, `Generator`)، وأنواع الرسوميات (بنية تحتية لـSadUI).

---

## ⚡ ما الذي تغيّر (الجوهر)

| قبل | بعد |
|-----|-----|
| `DataType` (enum يدويّ، 14 قيمة) + `SadTypeKind` منفصل + جسور تحويل | **`SadTypeKind` واحد** (52 قيمة) مُولَّد من SoT، `DataType` **محذوف كليّاً** |
| `نوع()` سلاسل عربية مكرّرة في المفسّر والمترجم (تتباعد) | مصدر واحد: `typeof_ar` في `types.yaml` → تطابق مزدوج |
| `عدم` = `فراغ` (لا تمايز) | `Null` متمايز عن `Void` + `Optional` (`T?`/`T؟`) + `?./??/؟` |
| لا أنواع متقدّمة مربوطة | `Result<T,E>`, `Future`, `Generator` موسومة، `intern()` |

---

## 📁 محتوى المجلد (الفعليّ)

```
type-system/
├── README.md                         ← هذا (فهرس + نظرة عامة)
├── decisions/
│   ├── ADR-TYPESYSTEM-001-NULL-SAFETY.md   ← قرار تمايز Null + Optional
│   └── README.md
├── docs/
│   ├── ARCHITECTURE_TYPE_SYSTEM.md          ← المعمارية الكاملة (الطبقات + التدفّق)
│   └── TYPE_SYSTEM_COMPLETION_PLAN.md       ← الخطة الأصلية (P0–P11)
├── stories/
│   ├── STORY-TS-P0-consistency.md … P11-graphics-types-sot.md  ← 13 قصّة منفّذة
│   └── README.md
├── status/
│   ├── implementation_status.md             ← حالة التنفيذ (دليل من الكود — GR-01)
│   ├── FINAL_REPORT_2026-06-13.md           ← تقرير منتصف الطريق
│   ├── FINAL_REPORT_2026-06-15.md           ← ✅ التقرير النهائي (الدمج + codegen-null)
│   └── 05_quality_gates.md                  ← بوّابات الجودة
├── epics/ · planning/ · sprints/            ← حوكمة BMAD
```

---

## 🗺️ القصص المنفَّذة (P0–P11) — كلّها مدموجة

| القصّة | الموضوع | الأثر |
|--------|---------|-------|
| **P0** | اتّساق `SadTypeKind` | أساس موحَّد |
| **P0.5** | توليد `SadTypeKind` من `types.yaml` | `gen_types.py` → `sad_type_kind_generated.h` (52 قيمة) |
| **P1** | نوع `Null` متمايز عن `Void` | `عدم != فراغ`، `makeNull()` |
| **P2/P2.5** | توحيد دلاليّ + **حذف `DataType`** | grep=0 في كل المستودع |
| **P3** | `SadResultType` | `Result<T,E>` + تباين |
| **P4** | ربط `Future`/`Generator` | `نوع(مستقبل())`=«مستقبل» (وسم بـ`setSadType`) |
| **P5** | suite الأمان النوعيّ | تأكيدات `نوع()` حقيقية |
| **P6** | تنظيف التعدادات | إزالة المهجور |
| **P7** | `intern()` | تجميع مثيلات الأنواع |
| **P8** | عوامل `?./??/؟` | معجم + محلّل + `operators.yaml` |
| **P9** | حارس عدم-الـnull | تحذير «اجعله اختياريًّا» |
| **P11** | أنواع الرسوميات في SoT | `Color/Widget/Window/Event/Point/Rect` |
| **+** | **codegen-null + توحيد `نوع()`** | `نوع(لاشيء)`=«عدم» في المترجم؛ مصدر `typeof_ar` واحد |

> P10 (تضييق التدفّق/flow-narrowing) **مُرحَّل** إلى نظام null-safety المستقلّ — انظر
> `_bmad-output/systems/null-safety/`.

---

## 🔌 نقاط التكامل (لمن يطوّر النظام)

- **مصدر الحقيقة:** `language-truth/types.yaml` (حقول `kind`, `word`, **`typeof_ar`**, `category`, `surface`, `status`).
- **المولِّد:** `scripts/codegen/gen_types.py` → `shared/types/generated/sad_type_kind_generated.h` (لا يُحرَّر يدوياً).
- **الواجهة والمساعدات:** `shared/types/include/sad_type_system.h` (`isAssignableTo`, `makeOptional/Result/Future`, `intern`).
- **القيمة:** `shared/types/src/value.cpp` (`type_` مقابل `sadType_`, `getKind()`, `setSadType`, `makeNull`).
- **`نوع()`:** `shared/builtins/src/runtime/builtins.cpp` (`type_of` = `sadTypeKindArabicName(getKind())`).
- **المترجم:** `compiler/.../llvm_type_mapper.cpp` (`mapSIRType`), `expression_basic.cpp`, `string_ops_ops.cpp`, `arith_cmp.cpp`.
- **مرجع المهارة:** `.github/skills/sad-lang-dev/references/types-and-sir.md`.

---

## 🔗 أنظمة ذات صلة

- **null-safety** (مستقلّ): التضييق + `!!` + إعادة إرساء P9 عبر `dispatch` → `_bmad-output/systems/null-safety/`.
- **SadUI** (الرسوميات): العناصر **كائنات** (`نوع(زر())`=«كائن»)، لا نوع `Widget` مميَّز؛
  الكتالوج يتقلّص 70→15 (ADR-UI-02) → `_bmad-output/systems/ui_system/` و`docs/تقرير_تكامل_SadUI_مع_نظام_الأنواع_الموحد.md`.

## 📚 ترتيب القراءة

1. **نظرة عامة:** هذا README ثم [status/FINAL_REPORT_2026-06-15.md](status/FINAL_REPORT_2026-06-15.md).
2. **المعمارية:** [docs/ARCHITECTURE_TYPE_SYSTEM.md](docs/ARCHITECTURE_TYPE_SYSTEM.md).
3. **التفاصيل:** [stories/](stories/) (P0–P11) + [decisions/ADR-TYPESYSTEM-001-NULL-SAFETY.md](decisions/ADR-TYPESYSTEM-001-NULL-SAFETY.md).
4. **الحالة/الجودة:** [status/implementation_status.md](status/implementation_status.md) + [status/05_quality_gates.md](status/05_quality_gates.md).
