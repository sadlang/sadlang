---
test_plan_key: docs-1.1-test-plan
story_ref: Story 1.1 — sadinfo skeleton
author: Murat (Test Architect)
created: 2026-05-09
status: ready-for-dev
---

# 🧪 Test Plan — Story 1.1: `tools/sadinfo/` Skeleton

## 🎯 نطاق الاختبار

Story 1.1 ينشئ binary فارغ يستجيب لـ `--version` و `--help` فقط — لا dump logic بعد.
خطة الاختبار تركّز على: **بناء صحيح، CLI آمن، cross-platform، الجاهزية للقصص اللاحقة**.

---

## 1️⃣ Test Pyramid

```
          ┌──────────────────────┐
          │  E2E (3 tests)       │  ← cross-platform binary execution
          ├──────────────────────┤
          │  Integration (5)     │  ← CMake + CLI parsing
          ├──────────────────────┤
          │  Unit (8 tests)      │  ← argparse + version + help
          └──────────────────────┘
```

---

## 2️⃣ Unit Tests (8) — `tests/sadinfo/unit/`

### `test_argparse.cpp`

| # | Test | Input | Expected |
|---|---|---|---|
| U-1 | `parseArgs_empty` | `{}` | print help, exit 0 |
| U-2 | `parseArgs_help` | `{"--help"}` | print help, exit 0 |
| U-3 | `parseArgs_version` | `{"--version"}` | print "sadinfo v1.0.0", exit 0 |
| U-4 | `parseArgs_unknown` | `{"--foo"}` | print error to stderr, exit 2 |
| U-5 | `parseArgs_short_h` | `{"-h"}` | print help, exit 0 |
| U-6 | `parseArgs_short_v` | `{"-v"}` | print version, exit 0 |
| U-7 | `parseArgs_combined` | `{"--help", "--version"}` | print help (first wins), exit 0 |
| U-8 | `parseArgs_negative_args` | `{"--", "--help"}` | treat `--help` as positional, error |

### Framework: GoogleTest (موجود في المشروع)

```cpp
TEST(SadinfoArgs, EmptyShowsHelpExitsZero) {
    int exitCode = sadinfo::main({});
    EXPECT_EQ(exitCode, 0);
    EXPECT_THAT(captured_stdout(), HasSubstr("Usage:"));
}
```

---

## 3️⃣ Integration Tests (5) — `tests/sadinfo/integration/`

### I-1: CMake builds target

```cmake
# tests/sadinfo/integration/CMakeLists.txt
add_test(NAME sadinfo_target_exists
    COMMAND ${CMAKE_COMMAND} -E echo "checking sadinfo binary"
    COMMAND test -f $<TARGET_FILE:sadinfo>
)
```

### I-2: Binary size check

```bash
# يجب أن يكون < 5 MB
test $(stat -f%z build/bin/Release/sadinfo) -lt 5242880
```

### I-3: Symbols correctly linked

```bash
# تأكد أن KeywordTable rمربوط (للقصص القادمة)
nm build/bin/Release/sadinfo | grep -q "KeywordTable"
```

### I-4: Output is UTF-8

```bash
./sadinfo --help | file -
# يجب: "UTF-8 Unicode text"
```

### I-5: No memory leaks (Linux+Mac)

```bash
valgrind --error-exitcode=1 ./sadinfo --version
valgrind --error-exitcode=1 ./sadinfo --help
```

---

## 4️⃣ E2E Tests (3) — Cross-Platform

GitHub Action matrix: `[windows-latest, ubuntu-latest, macos-latest]`.

### E-1: `--version` يعمل على 3 منصات

```yaml
- name: Test --version (all OS)
  run: |
    ./build/bin/Release/sadinfo --version
    ./build/bin/Release/sadinfo --version | grep -q "sadinfo v1.0.0"
```

### E-2: `--help` بـ exit code 0

```yaml
- name: Test --help exit 0
  run: |
    ./build/bin/Release/sadinfo --help
    echo $?  # يجب 0
```

### E-3: Unknown flag بـ exit code 2

```yaml
- name: Test unknown flag exits 2
  shell: bash
  run: |
    ./build/bin/Release/sadinfo --unknown-flag
    if [ $? -ne 2 ]; then exit 1; fi
```

---

## 5️⃣ Negative Tests (Edge Cases)

| # | Scenario | Expected Behavior |
|---|---|---|
| N-1 | Argument بـ NULL byte | rejected with clear error |
| N-2 | Argument > 4096 chars | rejected (overflow guard) |
| N-3 | `--help=value` (option ياخذ قيمة) | error: --help لا يأخذ قيمة |
| N-4 | UTF-8 BOM في args | محذوف بصمت أو rejected |
| N-5 | Args بأحرف عربية مكسورة | rejected with utf-8 error |
| N-6 | تشغيل من path يحوي مسافات/عربية | يعمل بدون مشاكل |

---

## 6️⃣ Performance Benchmarks

| Metric | Target | Measurement |
|---|---|---|
| Cold start (`--version`) | < 50 ms | `time ./sadinfo --version` |
| Help print | < 100 ms | `time ./sadinfo --help` |
| Binary size | < 5 MB | `ls -la build/bin/Release/sadinfo` |
| Build time | < 30 sec | `time cmake --build build --target sadinfo` |

---

## 7️⃣ Quality Gates لـ Story 1.1

| Gate | Threshold | Tool |
|---|---|---|
| Unit coverage | ≥ 90% | gcov / OpenCppCoverage |
| All 16 tests pass | 100% | CTest |
| Cross-platform CI | 3/3 OS green | GitHub Actions |
| No new warnings | 0 | MSVC /W4 + clang -Wall |
| Binary < 5 MB | strict | size check in CI |

---

## 8️⃣ المخاطر التي اكتشفتها أثناء المراجعة

| المخاطر | احتمال × تأثير | تخفيف |
|---|---|---|
| `KeywordTable::instance()` يفشل عند static init order | 2×4=8 🟡 | اختبار I-3 يكشف linking مبكراً |
| Args parsing مكتوب يدوياً (لا std library صحيحة) | 3×3=9 🟡 | استخدم CLI11 (header-only) أو cxxopts |
| MSVC `/utf-8` flag مفقود | 2×4=8 🟡 | تأكد في CMakeLists.txt — موثَّق في Architecture |
| Help text لا يدعم RTL terminals | 1×2=2 🟢 | مقبول، خارج scope |

---

## 9️⃣ ملف الاختبار التلقائي

```bash
# tests/sadinfo/run_all.sh
#!/bin/bash
set -e

echo "[Unit] Running 8 unit tests..."
ctest --test-dir build -R "Sadinfo.*" --output-on-failure

echo "[Integration] Running 5 integration tests..."
ctest --test-dir build -R "sadinfo_(target_exists|binary_size|symbols|utf8|valgrind)"

echo "[E2E] Running 3 E2E tests..."
./build/bin/Release/sadinfo --version | grep -q "sadinfo v1.0.0"
./build/bin/Release/sadinfo --help | grep -q "Usage:"
./build/bin/Release/sadinfo --unknown 2>&1 | grep -q "unknown" && [ $? -eq 0 ]

echo "✅ Story 1.1 — all 16 tests passed!"
```

---

## 🔟 Definition of Done (Test Perspective)

- [ ] 8 unit tests مكتوبة وتمر
- [ ] 5 integration tests مكتوبة وتمر
- [ ] 3 E2E tests على 3 منصات (CI)
- [ ] negative tests N-1 إلى N-6 مكتوبة وتمر
- [ ] code coverage ≥ 90%
- [ ] no new warnings (MSVC /W4)
- [ ] valgrind clean على Linux+Mac
- [ ] performance benchmarks موثَّقة
- [ ] `tests/sadinfo/run_all.sh` يعمل في < 60 ثانية

---

## ⚠️ ما لا يُختبر في Story 1.1 (مؤجَّل)

- ❌ `--dump-keywords` (Story 1.2)
- ❌ `--dump-builtins` (Story 1.3)
- ❌ JSON/YAML output (Stories 1.2-1.5)
- ❌ `--validate` (Story 1.6)

---

> **توقيع Murat:** "16 اختبار لـ skeleton يبدو مبالغاً به. لكن: لو فشل skeleton، كل القصص الـ7 بعده تفشل cascading. الاستثمار هنا = تأمين 5 أيام تطوير قادمة."
