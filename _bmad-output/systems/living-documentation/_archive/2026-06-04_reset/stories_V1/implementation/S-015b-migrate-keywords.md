# S-015b: Migrate Keywords

> **Phase:** 7 | **Effort:** L | **Risk:** Med
> **Depends on:** S-015a | **Blocks:** S-015c, S-016
> **Arch ref:** [DATA_SCHEMA_CONTRACTS.md](../DATA_SCHEMA_CONTRACTS.md) §2–9 + [YAML_UNIFIED_SCHEMA_DESIGN.md](../../../docplan/YAML_UNIFIED_SCHEMA_DESIGN.md) v1.0

> ⚠️ **البنية إلزامية:** كل keyword = مجلد `data/keywords/keyword_<id>/` بـ`_index.yaml` (§4.2) + `docs.yaml`.

---

## User Story

**As** maintainer،
**I want** نقل كل keywords من `data/language/keywords.yaml` إلى `data/keywords/{id}/`،
**So that** keywords lookup يصبح من format موحَّد مع باقي entities.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | كل keyword من `keywords.yaml` ← `data/keywords/{id}/definition.yaml` |
| AC-2 | الـcontextual keywords مُعلَّمة `context: contextual` |
| AC-3 | الـreserved keywords مُعلَّمة `context: reserved` |
| AC-4 | عدد keywords في v2 = عدد keywords في v1 (no loss) |
| AC-5 | `scripts/migrate_keywords.py` يُولِّد deterministic output |
| AC-6 | كل entity يجتاز Tier 1 + Tier 2 |
| AC-7 | parity test: `sadinfo --dump-keywords` v1 vs v2 |
| AC-8 | LSP semantic tokens لا تتراجع |

## Tasks

- [ ] T1: `scripts/migrate_keywords.py`
- [ ] T2: تشغيل + مراجعة
- [ ] T3: تصنيف reserved/contextual
- [ ] T4: parity test
- [ ] T5: LSP regression test

## File List

**جديد:**
- `scripts/migrate_keywords.py`
- `data/keywords/{id}/definition.yaml` (×~43)
- `tests/sadinfo/integration/keyword_parity_test.cpp`

## Definition of Done

- [ ] count matches v1 exactly
- [ ] parity test يمر
- [ ] LSP semantic tokens identical (golden)

## Notes للمطوِّر

- مرجع: `keywords.yaml` يحوي 40 reserved + ~30 contextual + 9 type names
- contextual يجب أن تكون قابلة للاستخدام كـidentifier — وثِّق في الـYAML
