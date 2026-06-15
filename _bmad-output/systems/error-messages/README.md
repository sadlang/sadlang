# فهرس وثائق Epic — Error Messages YAML Migration

**Epic ID**: EPIC-EM
**التاريخ**: 2026-05-19
**المُعِد**: Amelia (bmad-agent-dev)

---

## الوثائق

| الوثيقة | الوصف |
|---|---|
| 📘 [prd-error-messages.md](prd-error-messages.md) | متطلبات المنتج — المشكلة، الحل، الأهداف، AC |
| 📗 [epic-error-messages.md](epic-error-messages.md) | الEpic — Stories، التبعيات، AC على مستوى Epic |
| 📙 [tech-spec-error-messages.md](tech-spec-error-messages.md) | المواصفات التقنية — معمارية، YAML، Schema، C++، CMake |
| 📕 [error_messages_yaml_migration.md](error_messages_yaml_migration.md) | الخطة الأولية + مراجعة Amelia CR (السياق الأصلي) |
| 🔗 [_alignment_with_docplan.md](_alignment_with_docplan.md) | علاقة هذا الEpic بنظام التوثيق الموحَّد |

## Stories

| Story | العنوان | SP | الأولوية |
|---|---|---|---|
| 🟥 [EM-1](stories/EM-1-extract-sot.md) | استخراج SOT (YAML + Schema + 203 entry) | 8 | حرجة |
| 🟥 [EM-2](stories/EM-2-generator.md) | المولِّد + CMake + Baseline | 5 | حرجة |
| 🟥 [EM-3](stories/EM-3-integration.md) | تكامل + تقليص C++ | 8 | حرجة |
| 🟥 [EM-4](stories/EM-4-verification.md) | اختبارات الإثبات + golden test | 3 | حرجة |
| 🟦 [EM-5](stories/EM-5-sadinfo.md) | sadinfo --errors (اختياري) | 3 | منخفضة |

**الإجمالي**: 24 SP حرجة + 3 SP اختيارية = 27 SP.
**التقدير الزمني**: 5-7 أيام عمل فعلية.

## ترتيب التنفيذ

```mermaid
graph LR
    EM1[EM-1: SOT] --> EM2[EM-2: Generator]
    EM2 --> EM3[EM-3: Integration]
    EM3 --> EM4[EM-4: Verification]
    EM4 -.optional.-> EM5[EM-5: sadinfo]
```

## ملخص الأهداف القابلة للقياس

| الهدف | قبل | بعد |
|---|---|---|
| حجم `error_codes.cpp` | 843 سطر | ≤ 250 سطر (-70%) |
| حجم `error_catalog_init.cpp` | 213 سطر | ≤ 80 سطر (-65%) |
| تغطية `fixHint` | 26 خطأ (12%) | ≥ 100 خطأ (50%+) |
| ملفات معدَّلة لإضافة خطأ | 3 (`.h` + 2× `.cpp`) | 2 (`.h` + YAML) |
| `run_tests.ps1` | PASS:137 / FAIL:20 | PASS:137 / FAIL:20 (بلا تراجع) |
