# استراتيجية الاختبار
# Testing Strategy

**التاريخ / Date:** 2 ديسمبر 2025  
**الإصدار / Version:** 1.0

---

## 📋 نظرة عامة / Overview

### (AR) العربية

استراتيجية اختبار شاملة:
- Unit Tests (اختبارات الوحدة)
- Integration Tests (اختبارات التكامل)
- End-to-End Tests (اختبارات شاملة)
- Performance Benchmarks (قياسات الأداء)
- Regression Tests (اختبارات انحدار)
- Fuzzing Tests (اختبارات عشوائية)

**الهدف:** Test Coverage > 80%

---

## 🎯 أنواع الاختبارات / Test Types

### 1. Unit Tests (اختبارات الوحدة)

#### (AR) العربية

**Frontend Tests (200+ tests):**

```cpp
// tests/frontend/test_lexer.cpp
TEST(Lexer, TokenizeNumbers) {
    Lexer lexer("123 3.14 0b1010 0xABCD")
    auto tokens = lexer.tokenize()
    
    EXPECT_EQ(tokens[0].type, TokenType::LIT_NUMBER)
    EXPECT_EQ(tokens[0].value, "123")
    
    EXPECT_EQ(tokens[1].type, TokenType::LIT_NUMBER)
    EXPECT_EQ(tokens[1].value, "3.14")
}

TEST(Lexer, TokenizeArabicKeywords) {
    Lexer lexer("دالة إذا بينما إرجاع نهاية")
    auto tokens = lexer.tokenize()
    
    EXPECT_EQ(tokens[0].type, TokenType::KW_دالة)
    EXPECT_EQ(tokens[1].type, TokenType::KW_إذا)
}

TEST(Parser, ParseFunctionDeclaration) {
    Parser parser(tokens)
    auto ast = parser.parse()
    
    EXPECT_EQ(ast->type, "Program")
    EXPECT_EQ(ast->statements.size(), 1)
    
    auto func = dynamic_cast<FunctionDecl*>(ast->statements[0].get())
    EXPECT_EQ(func->name, "جمع")
    EXPECT_EQ(func->parameters.size(), 2)
}

TEST(TypeChecker, CheckBinaryExpression) {
    TypeChecker checker
    auto expr = make_binary_expr(OP_ADD, make_number(5), make_number(3))
    
    auto type = checker.check_expression(expr.get())
    EXPECT_EQ(type->name, "رقم")
}
```

**Middle-end Tests (100+ tests):**

```cpp
// tests/ir/test_sir.cpp
TEST(SIRBuilder, GenerateArithmetic) {
    auto sir = build_sir("رقم x = 5 + 3")
    
    EXPECT_EQ(sir->instructions.size(), 2)
    EXPECT_EQ(sir->instructions[0]->opcode, "add")
    EXPECT_EQ(sir->instructions[1]->opcode, "store")
}

TEST(Optimizer, ConstantFolding) {
    auto sir = build_sir("رقم x = 2 + 3 * 4")
    optimizer.constant_folding(sir)
    
    # Should fold to: x = 14
    EXPECT_EQ(sir->instructions.size(), 1)
    EXPECT_EQ(sir->instructions[0]->operands[1], "14")
}
```

**Backend Tests (150+ tests):**

```cpp
// tests/backends/test_bytecode.cpp
TEST(BytecodeEmitter, EmitArithmetic) {
    auto bytecode = emit_bytecode(sir)
    
    EXPECT_EQ(bytecode[0], Opcode::PUSH_I64)
    EXPECT_EQ(bytecode[9], Opcode::ADD)
}

TEST(VM, ExecuteSimpleProgram) {
    VM vm
    auto result = vm.execute(bytecode)
    
    EXPECT_EQ(result.as_number, 42)
}
```

**Runtime Tests (120+ tests):**

```cpp
// tests/runtime/test_memory.cpp
TEST(MemoryAllocator, BasicAllocation) {
    auto ptr = sad_alloc(1024)
    EXPECT_NE(ptr, nullptr)
    
    sad_free(ptr)
    # Should not leak
}

TEST(GC, MarkAndSweep) {
    GC gc
    auto obj1 = gc.allocate(100)
    auto obj2 = gc.allocate(200)
    
    gc.mark()
    gc.sweep()
    
    EXPECT_EQ(gc.get_live_objects(), 2)
}
```

---

### 2. Integration Tests (اختبارات التكامل)

#### (AR) العربية

**Compiler Pipeline Tests (80+ tests):**

```cpp
// tests/integration/test_pipeline.cpp
TEST(Pipeline, SourceToBytecode) {
    # .s → AST → SIR → Bytecode
    auto bytecode = compile_to_bytecode("src/test.s")
    EXPECT_TRUE(bytecode.has_value())
    
    VM vm
    auto result = vm.execute(bytecode.value())
    EXPECT_EQ(result.as_number, 42)
}

TEST(Pipeline, SourceToNative) {
    # .s → AST → SIR → LLVM IR → Native
    auto success = compile_to_native("src/test.s", "test")
    EXPECT_TRUE(success)
    
    # Execute native
    int result = system("./test")
    EXPECT_EQ(WEXITSTATUS(result), 42)
}
```

**Stage 1 Feature Tests (50+ tests):**

```cpp
// tests/integration/test_stage1.cpp
TEST(Stage1, Pointers) {
    auto result = execute_file("tests/stage1/test_pointers.s")
    EXPECT_EQ(result.stdout, "42\n")
}

TEST(Stage1, BitwiseOperations) {
    auto result = execute_file("tests/stage1/test_bitwise.s")
    EXPECT_EQ(result.stdout, "8\n14\n6\n")
}

TEST(Stage1, MemoryManagement) {
    auto result = execute_file("tests/stage1/test_memory.s")
    EXPECT_EQ(result.exit_code, 0)
    EXPECT_EQ(result.memory_leaks, 0)
}
```

---

### 3. End-to-End Tests (اختبارات شاملة)

#### (AR) العربية

**Real Program Tests (30+ tests):**

```s
# tests/e2e/fibonacci.s
دالة فيبوناتشي(رقم ن) -> رقم
    إذا (ن <= 1)
        إرجاع ن
    نهاية
    إرجاع فيبوناتشي(ن - 1) + فيبوناتشي(ن - 2)
نهاية

دالة main()
    اطبع(فيبوناتشي(10))  # Expected: 55
نهاية
```

```cpp
TEST(E2E, Fibonacci) {
    auto result = execute_file("tests/e2e/fibonacci.s")
    EXPECT_EQ(result.stdout, "55\n")
}
```

---

### 4. Performance Benchmarks (قياسات الأداء)

#### (AR) العربية

**Benchmark Suite:**

```cpp
// tests/benchmarks/benchmark_suite.cpp
BENCHMARK(Fibonacci_Interpreter) {
    Interpreter interp
    interp.execute_file("benchmarks/fibonacci.s")
}

BENCHMARK(Fibonacci_Bytecode) {
    VM vm
    auto bytecode = load_bytecode("benchmarks/fibonacci.sbc")
    vm.execute(bytecode)
}

BENCHMARK(Fibonacci_Native_O0) {
    system("./benchmarks/fibonacci_O0")
}

BENCHMARK(Fibonacci_Native_O2) {
    system("./benchmarks/fibonacci_O2")
}

# Results:
# Interpreter:   5000ms
# Bytecode:       500ms  (10x faster)
# Native -O0:     200ms  (25x faster)
# Native -O2:      50ms  (100x faster)
```

**Benchmark Categories:**
- Arithmetic operations
- Array operations
- String operations
- Memory allocation
- Function calls
- Object creation
- I/O operations

---

### 5. Regression Tests (اختبارات انحدار)

#### (AR) العربية

**Golden Output Tests:**

```bash
# tests/regression/run_regression.sh
for test in tests/regression/*.s; do
    sadc $test -o $test.out
    ./$test.out > $test.actual
    
    if diff $test.golden $test.actual; then
        echo "✓ $test PASSED"
    else
        echo "✗ $test FAILED"
    fi
done
```

**Regression Database:**
```
tests/regression/
├── golden/          # Expected outputs
│   ├── test1.golden
│   └── test2.golden
├── inputs/          # Test programs
│   ├── test1.s
│   └── test2.s
└── reports/         # Failure reports
```

---

### 6. Fuzzing Tests (اختبارات عشوائية)

#### (AR) العربية

**Fuzzer:**

```cpp
// tests/fuzzing/fuzzer.cpp
class Fuzzer {
public:
    دالة generate_random_program() -> نص
        # Generate random valid Sad code
        
    دالة fuzz_compiler(رقم iterations) -> فارغ
        لكل ع في مدى(iterations)
            auto code = generate_random_program()
            
            try {
                compile(code)
            } catch (...) {
                # Log crash
                save_crash_input(code)
            }
        نهاية
نهاية
```

**استخدام:**
```bash
# Run fuzzer
sad-fuzz --iterations 10000 --timeout 10s

# AFL fuzzing
afl-fuzz -i input/ -o output/ -- sadc @@
```

---

## 📊 Test Coverage Goals

### (AR) العربية

| المكون | الهدف | الحالي |
|--------|--------|--------|
| Lexer | > 95% | 0% |
| Parser | > 90% | 0% |
| Type Checker | > 90% | 0% |
| SIR Generator | > 85% | 0% |
| Optimizer | > 80% | 0% |
| Bytecode Emitter | > 90% | 0% |
| VM | > 90% | 0% |
| LLVM Backend | > 80% | 0% |
| Runtime | > 85% | 0% |
| **Overall** | **> 80%** | **0%** |

---

## 🔧 Testing Tools

### (AR) العربية

**Frameworks:**
- Google Test (C++ unit tests)
- pytest (Python test runner)
- Valgrind (memory leak detection)
- AddressSanitizer (memory errors)
- UndefinedBehaviorSanitizer
- ThreadSanitizer (race conditions)

**CI/CD:**
```yaml
# .github/workflows/test.yml
name: Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Build
        run: cmake --build build
      - name: Run Tests
        run: ctest --test-dir build
      - name: Coverage
        run: lcov --capture --output coverage.info
```

---

## 📈 Test Automation

### (AR) العربية

**Continuous Testing:**
```bash
# Watch mode
sad-test --watch

# Pre-commit hook
#!/bin/bash
sad-test --fast || exit 1

# Nightly full test
sad-test --all --benchmarks --report
```

---

**السابق / Previous:** [الجدول الزمني](11_timeline.md)  
**التالي / Next:** [المراجع التقنية](13_technical_refs.md)
