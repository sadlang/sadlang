# S-007: Unified `_state.json` + Lock File

> **Phase:** 2 | **Effort:** M | **Risk:** Med (PID safety)
> **Depends on:** S-003, S-000a | **Blocks:** S-005, S-008
> **Arch ref:** §3.2, §8, §6
>
> **ملاحظة:** هذه الـstory مُقدَّمة قبل S-005 لأن `_state.json` schema و lock primitives يجب أن تكون موجودة قبل أن يكتب Aggregator أي شيء فيها.

---

## User Story

**As** orchestrator،
**I want** ملف `_state.json` موحَّد + `.lock` آمن من PID reuse + atomic rename للتحديث،
**So that** متعدِّد العمليات لا يفسد cache.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | `_state.json` يحوي: cache_metadata + versions + dependency_graph + merkle_root + deletions |
| AC-2 | كتابة atomic: `_state.json.tmp` ثم `MoveFileExW(MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)` على Windows |
| AC-3 | على Linux/Mac: `rename(2)` (atomic بطبيعتها على نفس filesystem) |
| AC-4 | `.lock` يحوي `{pid, start_time, hostname}` JSON |
| AC-5 | acquire lock يفحص إذا lock موجود → يقرأ PID + start_time → يتحقَّق من العملية حية |
| AC-6 | على Windows: `OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION)` + `GetProcessTimes` للـstart_time |
| AC-7 | على Linux: `kill(pid, 0)` + `/proc/{pid}/stat` للـstart_time |
| AC-8 | إذا العملية ميتة أو start_time مختلف → lock يتيمة → احذف وأعد المحاولة |
| AC-9 | إذا حية → fail بـ`LockOrphan` error |
| AC-10 | FS غير مدعوم (FAT32/exFAT) → warning + slow path (rename normal) |

## Tasks

- [ ] T1: تعريف `CacheState` struct (الكامل)
- [ ] T2: JSON serializer/deserializer (nlohmann/json أو yaml-cpp)
- [ ] T3: `CacheLock::tryAcquire()` cross-platform
- [ ] T4: `CacheLock::release()` cross-platform
- [ ] T5: orphan detection logic (PID + start_time check)
- [ ] T6: atomic write helpers (`writeAtomic(path, content)`)
- [ ] T7: FS capability detection (NTFS/ext4/etc.)
- [ ] T8: Unit tests: serialize/deserialize roundtrip
- [ ] T9: Concurrency tests: 2 processes يحاولان acquire
- [ ] T10: Recovery tests: 4 سيناريوهات (alive, dead, PID_reuse, corrupt_lock)

## File List

**جديد:**
- `shared/sadinfo_core/include/cache_state.h`
- `shared/sadinfo_core/include/cache_lock.h`
- `shared/sadinfo_core/src/cache/cache_state.cpp`
- `shared/sadinfo_core/src/cache/cache_lock.cpp`
- `shared/sadinfo_core/src/cache/cache_lock_win.cpp`
- `shared/sadinfo_core/src/cache/cache_lock_posix.cpp`
- `shared/sadinfo_core/src/cache/atomic_write.cpp`
- `tests/sadinfo/concurrency/lock_tests.cpp`
- `tests/sadinfo/recovery/lock_recovery_tests.cpp`

## Definition of Done

- [ ] كل recovery scenarios تمر
- [ ] cross-platform tests على CI
- [ ] لا race conditions (تأكَّد بـTSan إن متوفر)

## Notes للمطوِّر

- اعتبر nlohmann/json لـ`_state.json` (دقيق ولا overhead على yaml-cpp)
- PID reuse: على Windows GetProcessTimes() يُرجع CreationTime — قارنه مع المُسجَّل
- على Linux: `cat /proc/PID/stat | awk '{print $22}'` للـstarttime
- atomic write: write to tmp same directory ثم rename (لا يجوز عبر filesystems)
- FS detection: `GetVolumeInformationW` على Windows لاسم FS
