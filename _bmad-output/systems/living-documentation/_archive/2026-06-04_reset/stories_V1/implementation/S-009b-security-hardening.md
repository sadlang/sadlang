# S-009b: Security Hardening (Path/Size/Sandbox)

> **Phase:** 3 | **Effort:** M | **Risk:** High
> **Depends on:** S-009 | **Blocks:** S-010, S-011, S-013
> **Arch ref:** §3.1, §3.6, §11 (Security)

---

## User Story

**As** مسؤول أمن،
**I want** sadinfo محمياً من path traversal، oversized files، malicious YAML، وأمثلة لها side effects خارج الـsandbox،
**So that** قراءة data من filesystem لا تكون vector هجوم.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | entity_id يطابق regex `^[a-zA-Z0-9_\u0600-\u06FF][a-zA-Z0-9_\u0600-\u06FF\-]{0,63}$` (لا `..`، لا `/`، لا control chars) |
| AC-2 | كل filesystem read يُحقَّق بـ`std::filesystem::canonical()` ويُفحص أنه ضمن `data/` root |
| AC-3 | حد أقصى لحجم ملف YAML: 1 MiB (configurable عبر `query_limits.yaml`) |
| AC-4 | حد أقصى لـnesting depth في YAML: 32 |
| AC-5 | حد أقصى لعدد entities في category: 10000 |
| AC-6 | يُرفض أي symlink (يُسجَّل warning + skip) |
| AC-7 | Windows long path: استخدم `\\?\` prefix داخلياً لكل path > 240 chars |
| AC-8 | عند تشغيل أمثلة (S-013)، الـsandbox يمنع: file I/O، network، subprocess (allowlist فقط للـbuiltins الحسابية) |
| AC-9 | Timeout للـexample execution: 5s (configurable) |
| AC-10 | كل violation تُسجَّل في logger كـ`security.violation` event |

## Tasks

- [ ] T1: `entity_id_validator.cpp` (regex + UTF-8 normalization NFC)
- [ ] T2: `path_canonicalizer.cpp` (canonical + root check)
- [ ] T3: تحديث Loader لـenforce size/depth limits
- [ ] T4: symlink detection + skip logic
- [ ] T5: Windows long path helper
- [ ] T6: `sandbox.cpp` (allowlist-based builtins for Tier 3)
- [ ] T7: timeout enforcement (Windows: Job Object، Linux: setrlimit/timer)
- [ ] T8: Security tests: 12 attack scenarios
- [ ] T9: Update existing stories' integration tests للـlong paths

## File List

**جديد:**
- `shared/sadinfo_core/include/security/entity_id_validator.h`
- `shared/sadinfo_core/include/security/path_canonicalizer.h`
- `shared/sadinfo_core/include/security/sandbox.h`
- `shared/sadinfo_core/src/security/entity_id_validator.cpp`
- `shared/sadinfo_core/src/security/path_canonicalizer.cpp`
- `shared/sadinfo_core/src/security/sandbox.cpp`
- `shared/sadinfo_core/src/security/long_path_win.cpp`
- `tests/sadinfo/security/path_traversal_tests.cpp`
- `tests/sadinfo/security/size_limits_tests.cpp`
- `tests/sadinfo/security/sandbox_tests.cpp`
- `tests/sadinfo/fixtures/malicious/` (12 attack vectors)

**معدَّل:**
- `shared/sadinfo_core/src/loader/loader.cpp` (size/depth checks)
- `data/_schemas/query_limits.yaml` (إضافة security limits)

## Definition of Done

- [ ] كل 12 attack scenario يُرفض بـvalidation error
- [ ] لا regression في الاختبارات السابقة
- [ ] long path test على Windows ينجح (300 char path)
- [ ] sandbox timeout يقتل example بعد 5s

## Notes للمطوِّر

- Attack vectors في `malicious/`:
  - `..\..\etc_passwd/` (path traversal)
  - `huge.yaml` (1 GiB)
  - `deep_nesting.yaml` (1000 levels)
  - `symlink_to_secret/`
  - `entity_id_with_null_byte/`
  - `unicode_rtl_override/`
  - 6 أخرى
- Sandbox: استخدم `Job Object` على Win لـCPU/memory limits + `JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE`
- على Linux: fork + `setrlimit(RLIMIT_AS, RLIMIT_CPU, RLIMIT_NOFILE)` + `prctl(PR_SET_PDEATHSIG)`
