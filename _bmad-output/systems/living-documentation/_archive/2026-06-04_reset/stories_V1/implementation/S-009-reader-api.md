# S-009: Reader API + ReaderError Variant

> **Phase:** 3 | **Effort:** L | **Risk:** High (LSP integration critical)
> **Depends on:** S-008 | **Blocks:** S-010, S-011, S-012, S-013, S-014
> **Arch ref:** §3.4, §7, §11

---

## User Story

**As** LSP/Formatter/Validator/Web (consumers)،
**I want** API نظيف للقراءة من cache + error handling صريح بـvariant،
**So that** الـconsumers لا يربطون yaml-cpp/SQLite، ويتعاملون مع كل سيناريو فشل بوضوح.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | `Reader::open(cacheDir) → std::variant<Reader, ReaderError>` |
| AC-2 | `ReaderError` enum: `NotFound`, `Corrupted`, `VersionMismatch`, `LockOrphan`, `SQLiteError` |
| AC-3 | كل ReaderError يحوي `message` و `recovery_hint` |
| AC-4 | `reader.get(id) → std::optional<EntityView>` |
| AC-5 | `reader.list(category) → std::vector<std::string>` (IDs) |
| AC-6 | `reader.traverse(id, kind, depth) → std::vector<EntityRef>` |
| AC-7 | `reader.subscribe(callback)` للـwatch updates (placeholder حتى S-011) |
| AC-8 | API لا يكشف yaml-cpp أو SQLite في headers (pImpl) |
| AC-9 | thread-safe reads (multiple readers OK) |
| AC-10 | LSP smoke test محدَّد: `keyword_provider.cpp` يُعاد كتابته لاستخدام Reader → اختبار يستدعي `provideCompletions("دال")` ويتأكَّد من إرجاع `دالة` keyword + entity metadata + duration < 10ms |
| AC-11 | scope الـLSP integration: ملف واحد فقط (`keyword_provider.cpp`) — تكامل أوسع في epic منفصل |
| AC-12 | كل query تمرّ عبر security validators من S-009b (depth limit، query timeout 100ms) |

## Tasks

- [ ] T1: تعريف `Reader` class (pImpl)
- [ ] T2: تعريف `ReaderError` variant types
- [ ] T3: `Reader::open` يفحص cache integrity (Merkle root)
- [ ] T4: version compatibility check
- [ ] T5: get/list/traverse implementations
- [ ] T6: thread safety (shared_mutex للقراءة)
- [ ] T7: subscribe placeholder
- [ ] T8: LSP integration (`tools/lsp/CMakeLists.txt` يربط `sadinfo_core`)
- [ ] T9: LSP smoke test
- [ ] T10: Unit + Integration tests شاملة

## File List

**جديد:**
- `shared/sadinfo_core/include/reader.h`
- `shared/sadinfo_core/include/reader_error.h`
- `shared/sadinfo_core/include/entity_ref.h`
- `shared/sadinfo_core/src/reader/reader.cpp`
- `tests/sadinfo/unit/reader_tests.cpp`
- `tests/sadinfo/integration/lsp_smoke_test.cpp`

**معدَّل:**
- `tools/lsp/CMakeLists.txt`
- `tools/lsp/src/keyword_provider.cpp` (يستخدم Reader)

## Definition of Done

- [ ] LSP smoke test يمر
- [ ] 5 error scenarios مُختبَرة
- [ ] header يفحص بـscript أنه لا يحوي `yaml-cpp` أو `sqlite3` includes
- [ ] thread safety test (100 readers concurrent)

## Notes للمطوِّر

- `Reader` صنف opaque — كل implementation داخل pImpl
- `ReaderError` يستخدم `std::variant<NotFound, Corrupted, VersionMismatch, LockOrphan, SQLiteError>` لتوسعة سهلة
- LSP integration حرجة — اعمل smoke test مبكِّر
- اختبر بـscript:
  ```bash
  grep -E "yaml-cpp|sqlite3|nlohmann" shared/sadinfo_core/include/*.h && exit 1 || exit 0
  ```
