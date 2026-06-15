# Epic — ترحيل رسائل الأخطاء إلى YAML

**Epic ID**: EPIC-EM
**العنوان**: Error Messages YAML Migration
**الإصدار**: 1.0.0
**التاريخ**: 2026-05-19
**المُعِد**: Amelia (bmad-agent-dev)
**المرجع PRD**: [prd-error-messages.md](prd-error-messages.md)

---

## الهدف

تحويل نظام رسائل الأخطاء من 1056 سطر C++ يدوياً (843 + 213) إلى ملف YAML واحد + توليد C++ تلقائي، مع توسيع تغطية `fixHint` من 26 إلى 100+ خطأ.

---

## القيمة التجارية

| القيمة | القياس |
|---|---|
| تجربة المطوّر | إضافة خطأ = سطر YAML بدلاً من 3 ملفات C++ |
| تجربة المستخدم النهائي | 4× زيادة في `fixHint` المعروضة |
| القابلية للصيانة | تقليص 70% من حجم `error_codes.cpp` |
| التكامل المستقبلي | استخراج الكتالوج للـLSP بـأمر واحد |

---

## Stories

| Story ID | العنوان | Story Points | الأولوية | الحالة |
|---|---|---|---|---|
| [EM-1](stories/EM-1-extract-sot.md) | استخراج SOT (YAML + Schema + 203 entry) | 8 | حرجة | جاهزة |
| [EM-2](stories/EM-2-generator.md) | المولِّد + CMake + Baseline | 5 | حرجة | جاهزة |
| [EM-3](stories/EM-3-integration.md) | تكامل + تقليص C++ | 8 | حرجة | جاهزة |
| [EM-4](stories/EM-4-verification.md) | golden test + run_tests | 3 | حرجة | جاهزة |
| [EM-5](stories/EM-5-sadinfo.md) | sadinfo --errors (اختياري) | 3 | منخفضة | مؤجَّلة |

**الإجمالي**: 27 SP (24 حرجة + 3 اختيارية).

---

## التبعيات بين Stories

```
EM-1 (SOT)
  └─→ EM-2 (Generator) — يحتاج YAML + Schema جاهزَين
        └─→ EM-3 (Integration) — يحتاج kErrorMessages مولَّداً
              └─→ EM-4 (Verification) — يحتاج التكامل مكتملاً
                    └─→ EM-5 (sadinfo) — اختياري
```

---

## معايير القبول للEpic

- ✅ **AC-E1**: كل الـ203 ErrorCode موجودة في YAML.
- ✅ **AC-E2**: لا تراجع في `run_tests.ps1` (PASS:137 / FAIL:20).
- ✅ **AC-E3**: golden round-trip = 0 diffs.
- ✅ **AC-E4**: `error_codes.cpp` ≤ 250 سطر.
- ✅ **AC-E5**: `error_catalog_init.cpp` ≤ 80 سطر.
- ✅ **AC-E6**: المولِّد + Schema يُختبران بـpytest.
- ✅ **AC-E7**: baseline files ملتزمة في git.
- ✅ **AC-E8**: تحديث memory في [/memories/repo/yaml_cpp_integration.md](../../../memories/repo/yaml_cpp_integration.md).

---

## المخاطر (مستوى Epic)

1. **فقدان نص** أثناء الاستخراج التلقائي — مخفَّف بـmigration script + مراجعة بشرية + golden test.
2. **تأخير الإصدار** بسبب 203 رسالة تحتاج مراجعة — مخفَّف بتقسيم EM-1 إلى يومين.
3. **كسر LSP/IDE** المعتمد على API الحالي — مخفَّف بـBF-15 (لا تغيير في توقيع الدوال العامة).

---

## ملاحظات معمارية

- النمط المرجعي: [data/language/keywords.yaml](../../../data/language/keywords.yaml).
- البنية التحتية الجاهزة: [scripts/codegen/_lib/](../../../scripts/codegen/_lib/) + [cmake/codegen.cmake](../../../cmake/codegen.cmake).
- `ErrorCode` enum يبقى يدوياً (مؤجَّل لـPhase 2).

---

**حالة الEpic**: جاهزة للتنفيذ.
