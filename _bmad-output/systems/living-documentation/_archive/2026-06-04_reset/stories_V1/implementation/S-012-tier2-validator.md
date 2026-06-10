# S-012: Tier 2 Validator (Cross-refs + i18n)

> **Phase:** 6 | **Effort:** M | **Risk:** Low
> **Depends on:** S-009 | **Blocks:** S-015
> **Arch ref:** §3.3 (Tier 2)

---

## User Story

**As** maintainer للـdata،
**I want** فحوصات Tier 2 (medium cost) تجري بعد aggregate الكامل،
**So that** أكتشف cross-reference errors و i18n gaps قبل النشر.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | `Validator::validateTier2(aggregationResult) → ValidationReport` |
| AC-2 | يفحص: كل `replaces` reference يشير لـentity موجود |
| AC-3 | يفحص: كل `extends` reference يشير لـentity موجود |
| AC-4 | يفحص: كل link في lessons يشير لـentity صحيح |
| AC-5 | يفحص: i18n coverage حسب `data/_schemas/i18n_policy.yaml` |
| AC-6 | i18n modes: `strict` (كل اللغات إجبارية)، `lenient` (الإنجليزية fallback)، `report-only` (warning) |
| AC-7 | يولِّد `missing_translations.json` في `cache/exports/` |
| AC-8 | تكلفة < 100ms لـ500 entity |
| AC-9 | يجري عند `sadinfo validate --tier=2` أو `sadinfo aggregate --full --validate` |

## Tasks

- [ ] T1: `validator_tier2.cpp`
- [ ] T2: cross-ref validator
- [ ] T3: i18n coverage analyzer
- [ ] T4: `i18n_policy.yaml` schema
- [ ] T5: `missing_translations.json` writer
- [ ] T6: CLI integration
- [ ] T7: Unit tests
- [ ] T8: Integration test مع `i18n_partial/` fixture

## File List

**جديد:**
- `shared/sadinfo_core/src/validator/validator_tier2.cpp`
- `shared/sadinfo_core/src/validator/i18n_coverage.cpp`
- `data/_schemas/i18n_policy.yaml`
- `tests/sadinfo/unit/validator_tier2_tests.cpp`
- `tests/sadinfo/integration/i18n_tests.cpp`
- `tests/sadinfo/fixtures/i18n_partial/`

## Definition of Done

- [ ] 3 i18n modes اختبارها
- [ ] missing_translations.json format موثَّق
- [ ] perf budget مُحقَّق

## Notes للمطوِّر

- `i18n_policy.yaml` format:
  ```yaml
  schema: 1
  mode: lenient   # strict | lenient | report-only
  required_languages: [ar, en]
  optional_languages: [fr, es]
  fallback_language: en
  exempt_categories: [errors]  # errors قد تكون عربية فقط في البداية
  ```
- cross-ref: تجوَّل على كل entity → لكل field من نوع `EntityRef` → افحص الوجود في الـmap
