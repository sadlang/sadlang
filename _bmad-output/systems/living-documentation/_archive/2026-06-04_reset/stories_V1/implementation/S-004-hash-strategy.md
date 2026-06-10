# S-004: Tiered Hash Strategy

> **Phase:** 2 (Cache) | **Effort:** S | **Risk:** Low
> **Depends on:** S-002 | **Blocks:** S-005
> **Arch ref:** §3.2 (Hashing strategy)

---

## User Story

**As** Aggregator، 
**I want** hash متعدِّد الطبقات (fast-path + full SHA-256)،
**So that** detection للتغيُّرات سريع جداً (99% case) مع دقَّة كاملة عند الحاجة.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | `HashStrategy::fastHash(path) → uint64_t` يستخدم size + first 4KB FNV-1a |
| AC-2 | `HashStrategy::fullHash(path) → string` يستخدم SHA-256 (hex) |
| AC-3 | `HashStrategy::contentEquals(pathA, pathB)` يستدعي fast أولاً ثم full |
| AC-4 | fast hash < 50µs لـ5KB |
| AC-5 | full SHA-256 < 0.5ms لـ5KB |
| AC-6 | streaming SHA لملفات > 64KB (لا تحمِّل كاملاً للذاكرة) |
| AC-7 | mmap عند توفُّره (Win: `MapViewOfFile`، Linux: `mmap`) |

## Tasks

- [ ] T1: تعريف `HashStrategy` namespace
- [ ] T2: implement fastHash (size << 32 | FNV-1a لأول 4KB)
- [ ] T3: implement fullHash (SHA-256 streaming)
- [ ] T4: mmap helper cross-platform
- [ ] T5: Unit tests: collision check، edge cases (empty، 1 byte، huge)
- [ ] T6: Benchmark suite

## File List

**جديد:**
- `shared/sadinfo_core/include/hash_strategy.h`
- `shared/sadinfo_core/src/aggregator/hash_strategy.cpp`
- `tests/sadinfo/unit/hash_tests.cpp`
- `tests/sadinfo/performance/hash_bench.cpp`

## Definition of Done

- [ ] Perf budgets مُحقَّقة
- [ ] لا dependency جديدة (استخدم OpenSSL إن متوفر، أو SHA-256 إيمبليمنتيشن صغيرة inline)
- [ ] mmap path مُختبَر على Windows + Linux

## Notes للمطوِّر

- OpenSSL متوفر في المشروع لكن اختياري — افحص بـ`#ifdef HAS_OPENSSL`
- fallback: SHA-256 inline (PD code من RFC 6234، ~150 سطر)
- fastHash تصادمات OK — fullHash يحلُّها
