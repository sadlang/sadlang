# S-011b: macOS Watcher (FSEvents)

> **Phase:** 5 | **Effort:** M | **Risk:** Med
> **Depends on:** S-011 | **Blocks:** None
> **Arch ref:** §3.6

---

## User Story

**As** مطوِّر على macOS،
**I want** الـwatcher يعمل بنفس جودة Win/Linux،
**So that** sadinfo cross-platform فعلاً.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | `WatcherMac.cpp` بـ`FSEvents` API كاملاً |
| AC-2 | يستهلك نفس `Watcher` interface من S-011 (no breaking change) |
| AC-3 | latency budget < 250ms (p95) — نفس Win/Linux |
| AC-4 | يعالج FSEvents coalescing (macOS قد يدمج events من نفسه) |
| AC-5 | يعمل على APFS و HFS+ |
| AC-6 | CI runner على macOS ينفِّذ كل tests الـwatcher |
| AC-7 | Graceful shutdown على SIGINT/SIGTERM |

## Tasks

- [ ] T1: `WatcherMac.cpp` (FSEvents + CFRunLoop)
- [ ] T2: تكامل مع DebouncerFSM (لا تكرار للـcoalescing)
- [ ] T3: APFS vs HFS+ test
- [ ] T4: storm test على macOS
- [ ] T5: CI: إضافة macos-latest runner للـsadinfo workflow

## File List

**جديد:**
- `shared/sadinfo_core/src/watcher/watcher_mac.cpp` (يحلّ محل الـstub من S-011)
- `tests/sadinfo/integration/watcher_mac_tests.cpp`

**معدَّل:**
- `.github/workflows/sadinfo.yml` (إضافة macos runner)

## Definition of Done

- [ ] storm test يستقرّ < 100ms على macOS
- [ ] لا memory leaks (Instruments check)
- [ ] CI macOS ينجح

## Notes للمطوِّر

- FSEvents callback يجري على main thread → استخدم dispatch_queue لـthread-safety
- `kFSEventStreamCreateFlagFileEvents` للحصول على per-file events (لا per-directory)
- APFS clones قد تُولِّد events غير متوقَّعة — اختبر
