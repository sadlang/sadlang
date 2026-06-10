# S-015a: Migrate Builtins (Hardcoded → data/)

> **Phase:** 7 | **Effort:** L | **Risk:** Med
> **Depends on:** S-010, S-012, S-014b | **Blocks:** S-015b, S-016
> **Arch ref:** [DATA_SCHEMA_CONTRACTS.md](../DATA_SCHEMA_CONTRACTS.md) §2–9 + [YAML_UNIFIED_SCHEMA_DESIGN.md](../../../docplan/YAML_UNIFIED_SCHEMA_DESIGN.md) v1.0

> ⚠️ **البنية إلزامية:** كل builtin = مجلد `data/builtins/builtin_<id>/` يحوي:
> - `_index.yaml` (إلزامي) — بنية §4.1 في contracts
> - `docs.yaml` (مستحسن)
> - `examples/*.yaml` (ملف لكل مثال — §6)
> - `i18n/en.yaml` (overlay اختياري)

---

## User Story

**As** maintainer،
**I want** نقل كل 15 builtin من `tools/sadinfo/src/builtin_data.cpp` إلى `data/builtins/{id}/`،
**So that** lookup الـbuiltins يصبح من data files لا hardcoded.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | كل 15 builtin مُرحَّل إلى `data/builtins/builtin_<id>/` ببنية whitelist (§3 contracts) |
| AC-1b | كل `_index.yaml` يجتاز `builtin.schema.json` |
| AC-1c | كل مثال في `examples/` يجتاز `example.schema.json` (deterministic → expected_output; non-det → expected_pattern + allow_nondet) |
| AC-2 | `scripts/migrate_builtins.py` deterministic (نفس الـoutput لنفس الـinput) |
| AC-3 | كل entity يجتاز Tier 1 + Tier 2 |
| AC-4 | parity test: `sadinfo --dump-builtins` v1 vs v2 = identical JSON |
| AC-5 | الـbuiltins القديمة تبقى شغَّالة (parallel mode) خلف flag `SADINFO_LEGACY_BUILTINS=1` |
| AC-6 | smoke tests على LSP + Formatter بعد الـmigration |
| AC-7 | رولباك مُختبَر: `git revert` يُعيد العمل بدون كسر |

## Tasks

- [ ] T1: `scripts/migrate_builtins.py` (قراءة C++ struct → kتابة YAML)
- [ ] T2: تشغيل + مراجعة يدوية لكل 15 entity
- [ ] T3: تشغيل Tier 1 + Tier 2
- [ ] T4: إصلاح validation errors
- [ ] T5: parity test
- [ ] T6: feature flag للـlegacy fallback
- [ ] T7: rollback test (revert + rerun comprehensive_tests)
- [ ] T8: تحديث `/memories/repo/keywords_yaml_sot_v41.md` (إضافة ملاحظة)

## File List

**جديد:**
- `scripts/migrate_builtins.py`
- `data/builtins/builtin_print/{definition,examples,_meta}.yaml`
- `data/builtins/builtin_print_line/...`
- ... (×15)
- `tests/sadinfo/integration/builtin_parity_test.cpp`

**معدَّل:**
- `tools/sadinfo/src/cli.cpp` (feature flag)
- `/memories/repo/keywords_yaml_sot_v41.md`

## Definition of Done

- [ ] 15 builtin = 45 YAML file
- [ ] parity test يمر
- [ ] rollback مُختبَر
- [ ] LSP smoke OK

## Notes للمطوِّر

- ابدأ بـ`اطبع` و`اطبع_سطر` (الأبسط)
- مراجعة يدوية حرجة للترجمات
- احتفظ بـv1 شغَّالاً حتى smoke tests تمر 100%
