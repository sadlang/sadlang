# المرحلة 6: تنظيم مجلد الاختبارات

## 🎯 الهدف

إعادة تنظيم مجلد tests/ ليتبع هيكلاً واضحاً ومتسقاً.

---

## 📍 الوضع الحالي

```
tests/
├── async/
├── backends/
├── benchmark.cpp
├── benchmark_arabic_opt.cpp
├── CMakeLists.txt
├── compiler/
│   ├── frontend/
│   ├── optimizer/
│   └── stage1/
├── comprehensive_test_suite.cpp
├── data_tests/
├── interpreter_tests/
├── jit/
├── lexer_tests/
├── low_level/
├── lsp/
├── modules/
├── network/
├── oop/
├── oop_tests/                   # ⚠️ تكرار مع oop/
├── optimizer/
├── parser/
├── parser_tests/                # ⚠️ تكرار مع parser/
├── pkg/
├── README.md
├── run_comprehensive_tests.ps1
├── simple_test.cpp
├── spec_rules/
├── stage1/
├── stdlib/
│   ├── test_crypto_module.cpp
│   ├── test_database_module.cpp
│   ├── test_filesystem_module.cpp
│   ├── test_http_module.cpp
│   ├── test_json_module.cpp
│   └── test_xml_module.cpp
├── stdlib_tests/                # ⚠️ تكرار مع stdlib/
├── temp_test1.s
├── test_*.cpp                   # ملفات متفرقة
└── toolchain_test.cpp
```

### المشاكل:

1. **تكرار**: `oop/` و `oop_tests/`، `parser/` و `parser_tests/`، `stdlib/` و `stdlib_tests/`
2. **ملفات متفرقة**: عدة ملفات `.cpp` في الجذر
3. **عدم وضوح**: الفرق بين `compiler/` و `stage1/`
4. **benchmarks مع tests**: يجب فصلها

---

## 📁 الهيكل الجديد المقترح

```
tests/
├── unit/                        # اختبارات الوحدات
│   ├── core/
│   │   ├── lexer/
│   │   │   └── lexer_tests.cpp
│   │   ├── parser/
│   │   │   └── parser_tests.cpp
│   │   ├── ast/
│   │   │   └── ast_tests.cpp
│   │   └── modules/
│   │       └── module_tests.cpp
│   │
│   ├── compiler/
│   │   ├── frontend/
│   │   │   ├── sir_tests.cpp
│   │   │   └── type_checker_tests.cpp
│   │   ├── optimizer/
│   │   │   └── optimizer_tests.cpp
│   │   ├── type_system/
│   │   │   └── type_system_tests.cpp
│   │   ├── llvm/
│   │   │   └── llvm_tests.cpp
│   │   └── jit/
│   │       └── jit_tests.cpp
│   │
│   ├── runtime/
│   │   ├── vm/
│   │   │   └── vm_tests.cpp
│   │   ├── memory/
│   │   │   └── gc_tests.cpp
│   │   └── thread/
│   │       └── thread_tests.cpp
│   │
│   ├── interpreter/
│   │   ├── expression_tests.cpp
│   │   ├── statement_tests.cpp
│   │   └── oop_tests.cpp
│   │
│   └── stdlib/
│       ├── core_tests.cpp
│       ├── io_tests.cpp
│       ├── math_tests.cpp
│       ├── string_tests.cpp
│       ├── filesystem_tests.cpp
│       ├── network_tests.cpp
│       ├── http_tests.cpp
│       ├── json_tests.cpp
│       ├── xml_tests.cpp
│       ├── database_tests.cpp
│       ├── crypto_tests.cpp
│       └── graphics_tests.cpp
│
├── integration/                 # اختبارات التكامل
│   ├── compiler_pipeline_tests.cpp
│   ├── interpreter_integration_tests.cpp
│   ├── module_loading_tests.cpp
│   └── network_integration_tests.cpp
│
├── e2e/                         # اختبارات شاملة
│   ├── comprehensive_test_suite.cpp
│   ├── spec_rules/
│   │   └── ...
│   └── scripts/
│       ├── test_basic.s
│       ├── test_oop.s
│       └── ...
│
├── benchmarks/                  # قياسات الأداء
│   ├── benchmark.cpp
│   ├── benchmark_arabic_opt.cpp
│   ├── compiler_benchmark.cpp
│   └── runtime_benchmark.cpp
│
├── tools/                       # اختبارات الأدوات
│   ├── lsp/
│   │   └── lsp_tests.cpp
│   ├── pkg/
│   │   └── pkg_tests.cpp
│   └── toolchain/
│       └── toolchain_test.cpp
│
├── fixtures/                    # بيانات الاختبار
│   ├── sample_programs/
│   ├── expected_outputs/
│   └── test_data/
│
├── helpers/                     # دوال مساعدة للاختبارات
│   ├── test_utils.h
│   ├── test_utils.cpp
│   └── mock_classes.h
│
├── CMakeLists.txt
├── README.md
└── run_tests.ps1
```

---

## 📋 خطوات التنفيذ

### الخطوة 1: إنشاء الهيكل الجديد

```powershell
$base = "C:\s\s_language\tests_new"

# Unit tests
New-Item -ItemType Directory -Path "$base\unit\core\lexer" -Force
New-Item -ItemType Directory -Path "$base\unit\core\parser" -Force
New-Item -ItemType Directory -Path "$base\unit\core\ast" -Force
New-Item -ItemType Directory -Path "$base\unit\core\modules" -Force
New-Item -ItemType Directory -Path "$base\unit\compiler\frontend" -Force
New-Item -ItemType Directory -Path "$base\unit\compiler\optimizer" -Force
New-Item -ItemType Directory -Path "$base\unit\compiler\type_system" -Force
New-Item -ItemType Directory -Path "$base\unit\compiler\llvm" -Force
New-Item -ItemType Directory -Path "$base\unit\compiler\jit" -Force
New-Item -ItemType Directory -Path "$base\unit\runtime\vm" -Force
New-Item -ItemType Directory -Path "$base\unit\runtime\memory" -Force
New-Item -ItemType Directory -Path "$base\unit\runtime\thread" -Force
New-Item -ItemType Directory -Path "$base\unit\interpreter" -Force
New-Item -ItemType Directory -Path "$base\unit\stdlib" -Force

# Integration
New-Item -ItemType Directory -Path "$base\integration" -Force

# E2E
New-Item -ItemType Directory -Path "$base\e2e\scripts" -Force
New-Item -ItemType Directory -Path "$base\e2e\spec_rules" -Force

# Benchmarks
New-Item -ItemType Directory -Path "$base\benchmarks" -Force

# Tools
New-Item -ItemType Directory -Path "$base\tools\lsp" -Force
New-Item -ItemType Directory -Path "$base\tools\pkg" -Force
New-Item -ItemType Directory -Path "$base\tools\toolchain" -Force

# Fixtures & Helpers
New-Item -ItemType Directory -Path "$base\fixtures\sample_programs" -Force
New-Item -ItemType Directory -Path "$base\fixtures\expected_outputs" -Force
New-Item -ItemType Directory -Path "$base\helpers" -Force
```

### الخطوة 2: نقل اختبارات Lexer و Parser

```powershell
$oldTests = "C:\s\s_language\tests"
$newTests = "C:\s\s_language\tests_new"

# Lexer tests
Copy-Item "$oldTests\lexer_tests\*" -Destination "$newTests\unit\core\lexer" -Recurse

# Parser tests (دمج parser/ و parser_tests/)
Copy-Item "$oldTests\parser\*" -Destination "$newTests\unit\core\parser" -Recurse
Copy-Item "$oldTests\parser_tests\*" -Destination "$newTests\unit\core\parser" -Recurse -ErrorAction SilentlyContinue
```

### الخطوة 3: نقل اختبارات Compiler

```powershell
# Frontend
Copy-Item "$oldTests\compiler\frontend\*" -Destination "$newTests\unit\compiler\frontend" -Recurse

# Optimizer
Copy-Item "$oldTests\compiler\optimizer\*" -Destination "$newTests\unit\compiler\optimizer" -Recurse
Copy-Item "$oldTests\optimizer\*" -Destination "$newTests\unit\compiler\optimizer" -Recurse -ErrorAction SilentlyContinue

# JIT
Copy-Item "$oldTests\jit\*" -Destination "$newTests\unit\compiler\jit" -Recurse
```

### الخطوة 4: نقل اختبارات Interpreter

```powershell
# دمج interpreter_tests/ و oop/ و oop_tests/
Copy-Item "$oldTests\interpreter_tests\*" -Destination "$newTests\unit\interpreter" -Recurse
Copy-Item "$oldTests\oop\*" -Destination "$newTests\unit\interpreter" -Recurse -ErrorAction SilentlyContinue
Copy-Item "$oldTests\oop_tests\*" -Destination "$newTests\unit\interpreter" -Recurse -ErrorAction SilentlyContinue
```

### الخطوة 5: نقل اختبارات Stdlib

```powershell
# دمج stdlib/ و stdlib_tests/
Copy-Item "$oldTests\stdlib\*" -Destination "$newTests\unit\stdlib" -Recurse
Copy-Item "$oldTests\stdlib_tests\*" -Destination "$newTests\unit\stdlib" -Recurse -ErrorAction SilentlyContinue
```

### الخطوة 6: نقل Benchmarks

```powershell
Copy-Item "$oldTests\benchmark.cpp" -Destination "$newTests\benchmarks"
Copy-Item "$oldTests\benchmark_arabic_opt.cpp" -Destination "$newTests\benchmarks"
```

### الخطوة 7: نقل E2E

```powershell
Copy-Item "$oldTests\comprehensive_test_suite.cpp" -Destination "$newTests\e2e"
Copy-Item "$oldTests\spec_rules\*" -Destination "$newTests\e2e\spec_rules" -Recurse

# نقل ملفات .s للاختبار
Get-ChildItem "$oldTests\*.s" | Copy-Item -Destination "$newTests\e2e\scripts"
```

### الخطوة 8: نقل اختبارات الأدوات

```powershell
Copy-Item "$oldTests\lsp\*" -Destination "$newTests\tools\lsp" -Recurse
Copy-Item "$oldTests\pkg\*" -Destination "$newTests\tools\pkg" -Recurse
Copy-Item "$oldTests\toolchain_test.cpp" -Destination "$newTests\tools\toolchain"
```

### الخطوة 9: نقل الملفات العامة

```powershell
Copy-Item "$oldTests\CMakeLists.txt" -Destination "$newTests"
Copy-Item "$oldTests\README.md" -Destination "$newTests"
Copy-Item "$oldTests\run_comprehensive_tests.ps1" -Destination "$newTests\run_tests.ps1"
```

### الخطوة 10: استبدال المجلد القديم

```powershell
# حذف القديم
Remove-Item -Recurse -Force "C:\s\s_language\tests"

# إعادة التسمية
Rename-Item "C:\s\s_language\tests_new" "C:\s\s_language\tests"
```

---

## 📝 تحديث CMakeLists.txt للاختبارات

```cmake
# ======================================================================
# اختبارات الوحدات / Unit Tests
# ======================================================================

# Core tests
add_executable(core_tests
    tests/unit/core/lexer/lexer_tests.cpp
    tests/unit/core/parser/parser_tests.cpp
    tests/unit/core/ast/ast_tests.cpp
)
target_link_libraries(core_tests PRIVATE sad_core gtest_main)

# Compiler tests
add_executable(compiler_tests
    tests/unit/compiler/frontend/sir_tests.cpp
    tests/unit/compiler/optimizer/optimizer_tests.cpp
    tests/unit/compiler/type_system/type_system_tests.cpp
)
target_link_libraries(compiler_tests PRIVATE sad_core gtest_main)

# Stdlib tests
add_executable(stdlib_tests
    tests/unit/stdlib/core_tests.cpp
    tests/unit/stdlib/io_tests.cpp
    tests/unit/stdlib/math_tests.cpp
    tests/unit/stdlib/string_tests.cpp
    tests/unit/stdlib/filesystem_tests.cpp
    tests/unit/stdlib/network_tests.cpp
    tests/unit/stdlib/json_tests.cpp
    tests/unit/stdlib/crypto_tests.cpp
)
target_link_libraries(stdlib_tests PRIVATE sad_core sad_stdlib gtest_main)

# ======================================================================
# اختبارات التكامل / Integration Tests
# ======================================================================

add_executable(integration_tests
    tests/integration/compiler_pipeline_tests.cpp
    tests/integration/interpreter_integration_tests.cpp
)
target_link_libraries(integration_tests PRIVATE sad_core gtest_main)

# ======================================================================
# قياسات الأداء / Benchmarks
# ======================================================================

add_executable(benchmarks
    tests/benchmarks/benchmark.cpp
    tests/benchmarks/benchmark_arabic_opt.cpp
)
target_link_libraries(benchmarks PRIVATE sad_core)

# ======================================================================
# إضافة الاختبارات لـ CTest
# ======================================================================

add_test(NAME CoreTests COMMAND core_tests)
add_test(NAME CompilerTests COMMAND compiler_tests)
add_test(NAME StdlibTests COMMAND stdlib_tests)
add_test(NAME IntegrationTests COMMAND integration_tests)
```

---

## ✅ قائمة التحقق

- [ ] إنشاء الهيكل الجديد
- [ ] نقل اختبارات Lexer و Parser
- [ ] نقل اختبارات Compiler
- [ ] نقل اختبارات Interpreter
- [ ] نقل اختبارات Stdlib
- [ ] نقل Benchmarks
- [ ] نقل E2E tests
- [ ] نقل اختبارات الأدوات
- [ ] تحديث CMakeLists.txt
- [ ] بناء الاختبارات
- [ ] تشغيل جميع الاختبارات
- [ ] حذف الملفات المكررة

---

## 🔗 المرحلة التالية

[المرحلة 7: تحديث CMakeLists.txt](07_phase7_cmake_update.md)
