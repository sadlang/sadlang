# S-015c: Migrate Error Messages

> **Phase:** 7 | **Effort:** M | **Risk:** Low
> **Depends on:** S-015b | **Blocks:** S-015d, S-016
> **Arch ref:** [DATA_SCHEMA_CONTRACTS.md](../DATA_SCHEMA_CONTRACTS.md) §2–9 + [YAML_UNIFIED_SCHEMA_DESIGN.md](../../../docplan/YAML_UNIFIED_SCHEMA_DESIGN.md) v1.0

> ⚠️ **البنية إلزامية:** كل error = مجلد `data/errors/error_<id>/` بـ`_index.yaml` (§4.3 — code: ^E\d{4}$) + `docs.yaml` + `i18n/en.yaml`.

---

## User Story

**As** maintainer،
**I want** نقل كل error messages من `data/language/error_messages.yaml` إلى `data/errors/{id}/`،
**So that** error catalog يكون قابلاً للترجمة والتحديث منفصل عن الكود.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | كل error message ← `data/errors/{id}/definition.yaml` |
| AC-2 | كل error له `code` ثابت (E0001, E0002, ...) |
| AC-3 | i18n keys: `ar` (مطلوب)، `en` (مطلوب)، `fr` (اختياري) |
| AC-4 | `scripts/migrate_errors.py` deterministic |
| AC-5 | كل entity يجتاز Tier 1 + Tier 2 |
| AC-6 | parity test: error messages المُولَّدة من المفسر/الكومبايلر تطابق v2 |
| AC-7 | المفسر يقرأ من v2 عبر Reader API (لا hardcoded في error_handler.cpp) |

## Tasks

- [ ] T1: `scripts/migrate_errors.py`
- [ ] T2: error code assignment (sequential)
- [ ] T3: تحديث `error_handler.cpp` في المفسر ليستخدم Reader
- [ ] T4: i18n coverage check
- [ ] T5: parity test

## File List

**جديد:**
- `scripts/migrate_errors.py`
- `data/errors/{id}/definition.yaml` (×~100)
- `tests/sadinfo/integration/error_parity_test.cpp`

**معدَّل:**
- `shared/errors/error_handler.cpp` (يستخدم Reader)

## Definition of Done

- [ ] كل error له code ثابت
- [ ] i18n ar+en complete
- [ ] parity test يمر
- [ ] المفسر لا hardcoded messages

## Notes للمطوِّر

- error codes تصبح ABI — وثِّق breaking changes إذا أي code تغيَّر
- ar/en شرط، fr nice-to-have
