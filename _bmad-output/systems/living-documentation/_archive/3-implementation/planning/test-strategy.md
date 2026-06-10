# Test Strategy: sadinfo v2

> **Coverage target:** > 80% lines, > 70% branches
> **Perf regression gate:** ≤ 10% vs baseline.json

---

## Test Pyramid

```
                 ╱╲
                ╱E2╲           ~30 tests (CLI workflows)
               ╱────╲
              ╱ Intg ╲          ~120 tests (Loader↔Aggregator↔Reader)
             ╱────────╲
            ╱   Unit   ╲         ~500 tests (per component)
           ────────────
```

## Layers

### 1. Unit Tests (`tests/sadinfo/unit/`)
- لكل component public method
- mocks محدودة (prefer real fixtures)
- runtime < 5s إجمالي

### 2. Integration Tests (`tests/sadinfo/integration/`)
- end-to-end داخل المكتبة: Loader → Validator → Aggregator → Cache → Reader
- fixtures حقيقية من `tests/sadinfo/fixtures/`
- runtime < 30s

### 3. E2E Tests (`tests/sadinfo/e2e/`)
- CLI binary فعلي
- لكل exit code اختبار
- Python harness

### 4. Concurrency Tests (`tests/sadinfo/concurrency/`)
- **ASAN + UBSAN** على Linux (clang)
- **AddressSanitizer + ConcurrencyVisualizer** على Windows (MSVC) — TSan لا يعمل بـMSVC
- 8 readers + 1 writer × 10000 iterations
- lock contention metrics

### 5. Performance Tests (`tests/sadinfo/performance/`)
- baseline في `tests/sadinfo/baselines/baseline.json` (محدَّث في S-000a)
- Google Benchmark
- Gate: regression > 10% يكسر CI

### 6. Security Tests (`tests/sadinfo/security/`)
- 12 attack vector (path traversal, symlink, zip bomb YAML, billion laughs, ...)
- يجب أن جميعها تُرفض بـexit code 75

### 7. Fuzzing (`tests/sadinfo/fuzz/`)
- libFuzzer على YAML parser entry
- 1M iterations أسبوعياً في CI

## Coverage Tools

| Platform | Tool | Output |
|----------|------|--------|
| Linux | gcov + lcov | lcov.info → Codecov |
| Windows | OpenCppCoverage | cobertura → Codecov |

## CI Gates (S-014b)

```yaml
gates:
  - name: build
    required: true
  - name: unit + integration + e2e
    required: true
  - name: concurrency (ASAN)
    required: true
  - name: security suite (12/12 vectors)
    required: true
  - name: performance regression
    threshold: 10%
    required: true
  - name: coverage
    line_threshold: 80%
    branch_threshold: 70%
    required: true
  - name: CodeQL
    required: true
```

## Test Data Strategy

- **Fixtures:** YAML مُختار يدوياً، committed، minimal
- **Generated:** scripts/gen_fixtures.py للـedge cases
- **Property-based:** rapidcheck للـschema invariants
- **Golden:** snapshots في `tests/sadinfo/golden/` (regenerate بـ`--update-snapshots`)

## Coverage Per Story

| Story | Unit | Intg | E2E | Concur | Perf | Sec |
|-------|------|------|-----|--------|------|-----|
| S-000a | ✓ | — | — | — | baseline | — |
| S-001 | ✓ | ✓ | — | — | — | — |
| S-002/S-003 | ✓ | ✓ | — | — | — | — |
| S-004 | ✓ | ✓ | — | — | ✓ | — |
| S-005 | ✓ | ✓ | — | — | ✓ | — |
| S-006 | ✓ | ✓ | — | ✓ | ✓ | — |
| S-007 | ✓ | ✓ | — | ✓ | — | — |
| S-008 | ✓ | ✓ | — | ✓ | ✓ | — |
| S-009 | ✓ | ✓ | ✓ | — | ✓ | — |
| S-009b | ✓ | — | ✓ | — | — | ✓ (12/12) |
| S-010 | — | ✓ | ✓ | — | — | — |
| S-011/b | ✓ | ✓ | — | ✓ | — | — |
| S-012 | — | ✓ | ✓ | — | — | — |
| S-013 | ✓ | ✓ | ✓ | ✓ | — | ✓ (sandbox) |
| S-014/b | — | — | ✓ | — | ✓ | — |
| S-015a-d | ✓ (parity) | ✓ | ✓ | — | ✓ | — |
| S-016 | — | — | ✓ (golden) | — | ✓ | — |

## Definition of Done (Test Quality)

- [ ] كل AC له test مُرتبط (traceability matrix)
- [ ] flaky tests = 0 (3 reruns متتالية بدون فشل)
- [ ] coverage gate يمر في CI
- [ ] perf gate يمر مع 5 runs (median)
- [ ] security suite 12/12
