# هيكل مشروع المترجم
# Compiler Project Structure

**التاريخ / Date:** 2 ديسمبر 2025  
**الحالة / Status:** المرحلة 0 - الإنشاء / Phase 0 - Created  
**الإصدار / Version:** 1.0

---

## 📁 الهيكل الكامل / Complete Structure

```
c:\s\s_language\
│
├── compiler/                           # مجلد المترجم الرئيسي / Main compiler directory
│   │
│   ├── frontend/                       # Frontend (Lexer, Parser, AST → SIR)
│   │   ├── sir_types.h                # أنواع SIR / SIR types
│   │   ├── sir_instruction.h          # تعليمات SIR / SIR instructions
│   │   ├── sir_builder.h              # بناء SIR من AST / Build SIR from AST
│   │   ├── sir_builder.cpp
│   │   ├── type_checker.h             # فحص الأنواع / Type checking
│   │   ├── type_checker.cpp
│   │   └── semantic_analyzer.h        # التحليل الدلالي / Semantic analysis
│   │
│   ├── middleend/                     # Middle-end (SIR Optimization)
│   │   ├── sir_optimizer.h            # محسن SIR / SIR optimizer
│   │   ├── sir_optimizer.cpp
│   │   ├── passes/                    # ممرات التحسين / Optimization passes
│   │   │   ├── constant_folding.h
│   │   │   ├── dead_code_elimination.h
│   │   │   ├── common_subexpression.h
│   │   │   └── inlining.h
│   │   └── analysis/                  # تحليلات / Analysis
│   │       ├── control_flow_graph.h
│   │       ├── dominators.h
│   │       └── liveness_analysis.h
│   │
│   ├── backend/                       # Backends (Code Generation)
│   │   │
│   │   ├── bytecode/                  # Bytecode Backend
│   │   │   ├── bytecode_format.h      # تنسيق البايتكود / Bytecode format
│   │   │   ├── bytecode_emitter.h     # توليد البايتكود / Generate bytecode
│   │   │   ├── bytecode_emitter.cpp
│   │   │   ├── vm/                    # Virtual Machine
│   │   │   │   ├── vm.h               # محرك الـ VM / VM engine
│   │   │   │   ├── vm.cpp
│   │   │   │   ├── stack.h            # Stack management
│   │   │   │   ├── heap.h             # Heap management
│   │   │   │   └── gc.h               # Garbage collector
│   │   │   └── runtime/               # مكتبة Runtime للبايتكود
│   │   │       ├── builtins.h
│   │   │       └── stdlib.h
│   │   │
│   │   └── llvm/                      # LLVM Backend
│   │       ├── llvm_codegen.h         # توليد LLVM IR / Generate LLVM IR
│   │       ├── llvm_codegen.cpp
│   │       ├── llvm_types.h           # ترجمة الأنواع / Type mapping
│   │       ├── llvm_runtime.h         # Runtime support
│   │       └── aot_compiler.h         # تجميع AOT / AOT compilation
│   │
│   └── runtime/                       # مكتبة Runtime المشتركة / Shared runtime
│       ├── memory.h                   # إدارة الذاكرة / Memory management
│       ├── memory.cpp
│       ├── gc.h                       # Garbage collection
│       ├── gc.cpp
│       ├── ffi.h                      # Foreign Function Interface
│       └── syscalls.h                 # System calls
│
├── include/                           # ملفات الرأسية الحالية / Current headers
│   ├── lexer/
│   ├── parser/
│   ├── interpreter/
│   └── ...
│
├── src/                               # الكود المصدري الحالي / Current source
│   ├── lexer/
│   ├── parser/
│   ├── interpreter/
│   └── ...
│
├── tests/                             # الاختبارات / Tests
│   ├── compiler/                      # اختبارات المترجم / Compiler tests
│   │   ├── stage1/                    # Stage 1 tests (20 tests)
│   │   │   ├── test_pointers_01.s
│   │   │   ├── test_pointers_02.s
│   │   │   ├── test_bitwise_01.s
│   │   │   ├── test_memory_01.s
│   │   │   └── ...
│   │   ├── frontend/                  # Frontend tests
│   │   ├── middleend/                 # Middle-end tests
│   │   ├── bytecode/                  # Bytecode tests
│   │   └── llvm/                      # LLVM tests
│   │
│   ├── interpreter/                   # اختبارات المفسر الحالي / Current interpreter tests
│   └── ...
│
├── docs/                              # الوثائق / Documentation
│   ├── compiler_plan/                 # خطة المترجم / Compiler plan
│   │   ├── status/                    # حالة التقدم / Progress status
│   │   │   ├── phase0_architecture.md
│   │   │   ├── phase0_sir_spec.md
│   │   │   ├── phase0_structure.md
│   │   │   └── phase0_progress.md
│   │   ├── 00_INDEX.md
│   │   ├── 01_overview_goals.md
│   │   └── ...
│   └── ...
│
├── build/                             # مجلد البناء / Build directory
├── CMakeLists.txt                     # ملف CMake الرئيسي / Main CMake
└── README.md
```

---

## 📝 وصف المجلدات / Directory Descriptions

### (AR) العربية

#### 1. compiler/frontend/
**المسؤولية:** تحويل AST إلى SIR

**الملفات الأساسية:**
- `sir_types.h` - تعريف أنواع SIR (i64, f64, ptr, array, etc.)
- `sir_instruction.h` - تعريف تعليمات SIR (add, sub, mul, load, store, etc.)
- `sir_builder.h/cpp` - بناء SIR من AST
- `type_checker.h/cpp` - فحص الأنواع والتحقق من صحتها
- `semantic_analyzer.h` - تحليل دلالي (scope checking, name resolution)

**الوظائف:**
```cpp
class SIRBuilder {
public:
    SIRModule buildFromAST(AST::Program* ast);
    SIRFunction buildFunction(AST::FunctionDeclStmt* func);
    SIRInstruction buildExpression(AST::Expression* expr);
};
```

---

#### 2. compiler/middleend/
**المسؤولية:** تحسين SIR

**الملفات الأساسية:**
- `sir_optimizer.h/cpp` - محسن SIR الرئيسي
- `passes/` - ممرات التحسين:
  - `constant_folding.h` - طي الثوابت (2+3 → 5)
  - `dead_code_elimination.h` - إزالة الكود غير المستخدم
  - `common_subexpression.h` - إزالة التعابير المكررة
  - `inlining.h` - دمج الدوال الصغيرة
- `analysis/` - تحليلات:
  - `control_flow_graph.h` - رسم CFG
  - `dominators.h` - تحليل الهيمنة
  - `liveness_analysis.h` - تحليل حيوية المتغيرات

**الوظائف:**
```cpp
class SIROptimizer {
public:
    void optimize(SIRModule& module, OptimizationLevel level);
    void runPass(OptimizationPass& pass);
};

enum class OptimizationLevel {
    O0,  // بدون تحسين
    O1,  // تحسينات أساسية
    O2,  // تحسينات متوسطة
    O3   // تحسينات قصوى
};
```

---

#### 3. compiler/backend/bytecode/
**المسؤولية:** توليد Bytecode وتنفيذه

**الملفات الأساسية:**
- `bytecode_format.h` - تنسيق البايتكود (.sbc)
- `bytecode_emitter.h/cpp` - توليد البايتكود من SIR
- `vm/vm.h/cpp` - محرك الـ Virtual Machine
- `vm/stack.h` - إدارة الـ stack
- `vm/heap.h` - إدارة الـ heap
- `vm/gc.h` - Garbage collector
- `runtime/` - مكتبة runtime للبايتكود

**الوظائف:**
```cpp
class BytecodeEmitter {
public:
    BytecodeModule emit(const SIRModule& sir);
};

class VM {
public:
    Value execute(const BytecodeModule& bytecode);
    void push(Value val);
    Value pop();
};
```

---

#### 4. compiler/backend/llvm/
**المسؤولية:** توليد LLVM IR والتجميع الأصلي

**الملفات الأساسية:**
- `llvm_codegen.h/cpp` - توليد LLVM IR من SIR
- `llvm_types.h` - ترجمة أنواع SIR إلى LLVM
- `llvm_runtime.h` - دعم runtime لـ LLVM
- `aot_compiler.h` - تجميع Ahead-of-Time

**الوظائف:**
```cpp
class LLVMCodeGen {
public:
    llvm::Module* generate(const SIRModule& sir);
    llvm::Function* generateFunction(const SIRFunction& func);
    llvm::Value* generateInstruction(const SIRInstruction& inst);
};

class AOTCompiler {
public:
    bool compileToNative(const std::string& inputFile, 
                        const std::string& outputFile,
                        OptimizationLevel level);
};
```

---

#### 5. compiler/runtime/
**المسؤولية:** مكتبة Runtime المشتركة

**الملفات الأساسية:**
- `memory.h/cpp` - إدارة الذاكرة (malloc, free)
- `gc.h/cpp` - Garbage collection
- `ffi.h` - Foreign Function Interface (استدعاء C/C++)
- `syscalls.h` - System calls (file I/O, etc.)

**الوظائف:**
```cpp
namespace Sad::Runtime {
    void* allocate(size_t size);
    void deallocate(void* ptr);
    
    void gcCollect();
    void gcMark(Value* root);
    void gcSweep();
    
    Value callForeignFunction(const std::string& name, 
                             const std::vector<Value>& args);
}
```

---

#### 6. tests/compiler/stage1/
**المسؤولية:** اختبارات Stage 1 (20 test)

**التقسيم:**
- **Pointers (5 tests):**
  - `test_pointers_01.s` - أساسيات المؤشرات
  - `test_pointers_02.s` - pointer arithmetic
  - `test_pointers_03.s` - double pointers
  - `test_pointers_04.s` - null pointers
  - `test_pointers_05.s` - pointer casting

- **Bitwise (5 tests):**
  - `test_bitwise_01.s` - AND, OR, XOR
  - `test_bitwise_02.s` - NOT, shift
  - `test_bitwise_03.s` - flags manipulation
  - `test_bitwise_04.s` - bit masks
  - `test_bitwise_05.s` - rotate operations

- **Memory (5 tests):**
  - `test_memory_01.s` - stack allocation
  - `test_memory_02.s` - heap allocation
  - `test_memory_03.s` - memory copy
  - `test_memory_04.s` - memory leaks detection
  - `test_memory_05.s` - large allocations

- **Control Flow (3 tests):**
  - `test_control_01.s` - goto labels
  - `test_control_02.s` - switch/case
  - `test_control_03.s` - computed jumps

- **Functions (2 tests):**
  - `test_functions_01.s` - function pointers
  - `test_functions_02.s` - inline assembly (future)

---

## 🔧 تحديث CMakeLists.txt / Update CMakeLists.txt

سيتم إضافة القسم التالي:

```cmake
# ======================================================================
# المترجم / Compiler (Phase 0+)
# ======================================================================

option(BUILD_COMPILER "بناء المترجم / Build compiler" ON)

if(BUILD_COMPILER)
    # Frontend
    set(COMPILER_FRONTEND_SOURCES
        compiler/frontend/sir_builder.cpp
        compiler/frontend/type_checker.cpp
    )
    
    # Middle-end
    set(COMPILER_MIDDLEEND_SOURCES
        compiler/middleend/sir_optimizer.cpp
    )
    
    # Backend - Bytecode
    set(COMPILER_BYTECODE_SOURCES
        compiler/backend/bytecode/bytecode_emitter.cpp
        compiler/backend/bytecode/vm/vm.cpp
    )
    
    # Backend - LLVM
    set(COMPILER_LLVM_SOURCES
        compiler/backend/llvm/llvm_codegen.cpp
    )
    
    # Runtime
    set(COMPILER_RUNTIME_SOURCES
        compiler/runtime/memory.cpp
        compiler/runtime/gc.cpp
    )
    
    # مكتبة المترجم / Compiler library
    add_library(sad_compiler STATIC
        ${COMPILER_FRONTEND_SOURCES}
        ${COMPILER_MIDDLEEND_SOURCES}
        ${COMPILER_BYTECODE_SOURCES}
        ${COMPILER_RUNTIME_SOURCES}
    )
    
    # LLVM support (optional)
    find_package(LLVM QUIET)
    if(LLVM_FOUND)
        target_sources(sad_compiler PRIVATE ${COMPILER_LLVM_SOURCES})
        target_link_libraries(sad_compiler PRIVATE LLVM)
        message(STATUS "✓ دعم LLVM مفعل / LLVM support enabled")
    else()
        message(STATUS "⚠ LLVM غير موجود / LLVM not found")
    endif()
    
    # Compiler executable
    add_executable(sadc
        compiler/main.cpp
    )
    
    target_link_libraries(sadc PRIVATE sad_core sad_compiler)
    
    message(STATUS "✓ المترجم مفعل / Compiler enabled")
endif()
```

---

## 📊 التقدم / Progress

| المكون | الحالة |
|--------|--------|
| compiler/frontend/ | ✅ مُنشأ |
| compiler/middleend/ | ✅ مُنشأ |
| compiler/backend/bytecode/ | ✅ مُنشأ |
| compiler/backend/llvm/ | ✅ مُنشأ |
| compiler/runtime/ | ✅ مُنشأ |
| tests/compiler/stage1/ | ✅ مُنشأ |
| CMakeLists.txt | ⏳ يحتاج تحديث |

---

**السابق / Previous:** [SIR Specification](phase0_sir_spec.md)  
**التالي / Next:** [Stage 1 Tests](phase0_tests.md)
