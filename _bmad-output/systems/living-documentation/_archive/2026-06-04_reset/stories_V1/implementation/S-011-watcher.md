# S-011: Watcher Cross-Platform + Debouncer FSM + Coalescer

> **Phase:** 5 | **Effort:** XL | **Risk:** High
> **Depends on:** S-009 | **Blocks:** S-015
> **Arch ref:** §3.6

---

## User Story

**As** مطوِّر يعدِّل ملفات `data/` بشكل مستمر،
**I want** watcher يلتقط التغيُّرات + يدبضها + يُحدِّث cache تلقائياً،
**So that** LSP/Web يرى التغيُّرات في < 250ms بدون يدوية.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | Watcher: Windows (`ReadDirectoryChangesW`) + Linux (`inotify`). macOS مؤجَّل لـS-011b (stub يُرجع `NotSupported`) |
| AC-2 | DebouncerFSM بـ4 states: IDLE → SCHEDULED (10ms) → COALESCING (200ms if > 5 events) → FIRE → IDLE |
| AC-3 | Coalescer يدمج events للـsame file (modify + modify → modify واحد) |
| AC-4 | git checkout (500 events) لا يُغرق — يستقرّ خلال 100ms |
| AC-5 | watcher latency (event → cache update) < 250ms (p95) |
| AC-6 | `sadinfo watch` CLI command (يبقى running) |
| AC-7 | Ctrl+C يُغلق watcher بـclean (release lock، flush cache) |
| AC-8 | فحص missed events: periodic full scan كل 5 دقائق، configurable عبر CLI flag `--scan-interval=Ns` أو env `SADINFO_SCAN_INTERVAL` |
| AC-9 | Windows long paths مدعومة (يستخدم helper من S-009b) |

## Tasks

- [ ] T1: `Watcher` interface (cross-platform abstraction)
- [ ] T2: `WatcherWin.cpp` (`ReadDirectoryChangesW`)
- [ ] T3: `WatcherLinux.cpp` (`inotify`)
- [ ] T4: `WatcherMac.cpp` (`FSEvents`)
- [ ] T5: `DebouncerFSM` class مع state diagram إيمبليمنتيشن
- [ ] T6: `Coalescer` (event deduplication)
- [ ] T7: `sadinfo watch` CLI
- [ ] T8: graceful shutdown
- [ ] T9: periodic missed-event scan
- [ ] T10: Unit tests للـFSM
- [ ] T11: Integration tests على Windows + Linux
- [ ] T12: storm test (`git_checkout_storm.ps1` generator)

## File List

**جديد:**
- `shared/sadinfo_core/include/watcher.h`
- `shared/sadinfo_core/include/debouncer_fsm.h`
- `shared/sadinfo_core/include/coalescer.h`
- `shared/sadinfo_core/src/watcher/watcher_base.cpp`
- `shared/sadinfo_core/src/watcher/watcher_win.cpp`
- `shared/sadinfo_core/src/watcher/watcher_linux.cpp`
- `shared/sadinfo_core/src/watcher/watcher_mac.cpp`
- `shared/sadinfo_core/src/watcher/debouncer_fsm.cpp`
- `shared/sadinfo_core/src/watcher/coalescer.cpp`
- `tools/sadinfo/src/commands/watch.cpp`
- `tests/sadinfo/unit/debouncer_fsm_tests.cpp`
- `tests/sadinfo/integration/watcher_tests.cpp`
- `tests/sadinfo/fixtures/generators/git_checkout_storm.ps1`

## Definition of Done

- [ ] Storm test يستقرّ خلال 100ms
- [ ] Latency budget مُحقَّق على Win + Linux
- [ ] macOS implementation شغَّال (إن لم يكن high-priority، قد يُؤجَّل لـnext sprint)
- [ ] graceful shutdown اختبار

## Notes للمطوِّر

- DebouncerFSM transitions (من الوثيقة §3.6):
  ```
  IDLE --event--> SCHEDULED (timer 10ms)
  SCHEDULED --timer_expired & events<=5--> FIRE
  SCHEDULED --events>5--> COALESCING (timer 200ms)
  COALESCING --timer_expired--> FIRE
  COALESCING --events_continue--> COALESCING (reset timer)
  FIRE --done--> IDLE
  ```
- Coalescer: hash map من path → latest event
- macOS قد يُؤجَّل: ابدأ بـstub يُرجع `NotSupported` ومرَّر للـnext story
