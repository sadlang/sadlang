# S-006: Aliases (Rename Support)

> **Phase:** 2 | **Effort:** M | **Risk:** Low
> **Depends on:** S-005 | **Blocks:** S-008
> **Arch ref:** §3.2 (aliases.yaml schema)

---

## User Story

**As** صاحب الـbuiltin اللي اتغيَّر اسمه (`اطبع_سطر` → `اطبع_خط` مثلاً)،
**I want** alias system يحافظ على backward compat بدون breaking existing code،
**So that** الـrename لا يكسر LSP/docs/examples.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | `data/_meta/aliases.yaml` schema مُعرَّفة |
| AC-2 | كل alias entry: `{old_id, new_id, deprecated_at, remove_after, auto_update_refs}` |
| AC-3 | Aggregator يقرأ aliases.yaml ويُعالج: rename = (deletion of old_id) + (addition of new_id) + alias entry |
| AC-4 | Validator يحذِّر إذا `remove_after` انتهى ولـold_id لا يزال مستخدماً |
| AC-5 | Reader API يُرجع entity من old_id أو new_id (transparent) |
| AC-6 | `sadinfo aliases list` و `sadinfo aliases add --from X --to Y` commands |
| AC-7 | إذا `auto_update_refs: true` → Exporter يولِّد ملف rename suggestions للـIDE |

## Tasks

- [ ] T1: schema `aliases.yaml` + validation
- [ ] T2: Aggregator يدمج aliases في `AggregationResult`
- [ ] T3: Reader API يفحص aliases عند `get(id)`
- [ ] T4: CLI command `sadinfo aliases list/add/remove`
- [ ] T5: Validator warning لـexpired aliases
- [ ] T6: Unit + Integration tests
- [ ] T7: Rename scenario fixture

## File List

**جديد:**
- `shared/sadinfo_core/include/alias_manager.h`
- `shared/sadinfo_core/src/aggregator/alias_manager.cpp`
- `tools/sadinfo/src/commands/aliases.cpp`
- `tests/sadinfo/unit/alias_tests.cpp`
- `tests/sadinfo/fixtures/rename_scenario/`

## Definition of Done

- [ ] rename scenario يمر بالكامل
- [ ] documentation في `docs/sadinfo/aliases.md`
- [ ] CLI help موثَّق

## Notes للمطوِّر

- اقترح format:
  ```yaml
  schema: 1
  aliases:
    - old_id: اطبع_سطر
      new_id: اطبع_خط
      deprecated_at: 2026-01-15
      remove_after: 2026-07-15
      auto_update_refs: true
      reason: "توحيد التسمية مع باقي الـbuiltins"
  ```
- `auto_update_refs` لا يُعدِّل الكود فعلياً — فقط يولِّد قائمة في `exports/rename_suggestions.json`
