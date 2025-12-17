# المرحلة 4: LLVM Backend
# Phase 4: LLVM Backend

**المدة / Duration:** 8-12 أسابيع / weeks  
**الأولوية / Priority:** P1 (عالية / High)  
**الحالة / Status:** 🔴 لم تبدأ / Not Started

---

## 📋 نظرة عامة / Overview

### (AR) العربية

تطوير LLVM backend كامل:
- SIR to LLVM IR translation
- LLVM optimization pipeline
- AOT compilation (native executables)
- Cross-compilation support
- Debug info generation (DWARF)

**الهدف:** 50-100x أسرع من المفسّر

---

## 🎯 LLVM Integration / تكامل LLVM

### (AR) العربية

**LLVM Pipeline:**
```
SIR → LLVM IR → LLVM Opts → Machine Code → Object File → Executable

# Commands:
sadc program.s -o program --llvm           # Native executable
sadc program.s -o program.ll --emit-llvm   # LLVM IR text
sadc program.s -o program.bc --emit-bc     # LLVM bitcode
sadc program.s -o program.o --emit-obj     # Object file
```

**Optimization Levels:**
- `-O0`: No optimization (debugging)
- `-O1`: Basic optimizations
- `-O2`: Standard optimizations (default)
- `-O3`: Aggressive optimizations
- `-Os`: Size optimizations
- `-Oz`: Minimum size

---

## 📝 المهام / Tasks

### 1. LLVM IR Generator (3-4 أسابيع)

```cpp
// compiler/backends/llvm/codegen.h
class LLVMCodeGen {
private:
    # LLVM Context
    مؤشر<llvm::LLVMContext> context
    مؤشر<llvm::Module> module
    مؤشر<llvm::IRBuilder<>> builder
    
    # Target info
    مؤشر<llvm::TargetMachine> target_machine
    
    # Symbol tables
    قاموس<نص، llvm::Value*> named_values
    قاموس<نص، llvm::Function*> functions
    
public:
    دالة generate(مؤشر<SIRModule> sir) -> مؤشر<llvm::Module>
    
    # Module
    دالة emit_module(مؤشر<SIRModule> sir) -> فارغ
    
    # Functions
    دالة emit_function(مؤشر<SIRFunction> func) -> llvm::Function*
    دالة emit_function_body(مؤشر<SIRFunction> func, llvm::Function* llvm_func) -> فارغ
    
    # Basic blocks
    دالة emit_basic_block(مؤشر<SIRBasicBlock> bb) -> llvm::BasicBlock*
    
    # Instructions
    دالة emit_instruction(مؤشر<SIRInstruction> inst) -> llvm::Value*
    
    # Arithmetic
    دالة emit_add(مؤشر<SIRInstruction> inst) -> llvm::Value*
    دالة emit_sub(مؤشر<SIRInstruction> inst) -> llvm::Value*
    دالة emit_mul(مؤشر<SIRInstruction> inst) -> llvm::Value*
    
    # Memory
    دالة emit_load(مؤشر<SIRInstruction> inst) -> llvm::Value*
    دالة emit_store(مؤشر<SIRInstruction> inst) -> llvm::Value*
    دالة emit_alloc(مؤشر<SIRInstruction> inst) -> llvm::Value*
    
    # Control flow
    دالة emit_branch(مؤشر<SIRInstruction> inst) -> فارغ
    دالة emit_call(مؤشر<SIRInstruction> inst) -> llvm::Value*
    دالة emit_return(مؤشر<SIRInstruction> inst) -> فارغ
    
    # Types
    دالة convert_type(مؤشر<Type> type) -> llvm::Type*
نهاية
```

**Type Mapping:**
```cpp
# Sad Type → LLVM Type
رقم       → i64
نص        → i8* (pointer to string)
منطقي    → i1
مصفوفة<T> → {i64, T*} (length + pointer)
مؤشر<T>   → T*
دالة      → function pointer
```

**مثال SIR → LLVM IR:**
```
# SIR:
function @جمع(%a: i64, %b: i64) -> i64 {
entry:
    %t0 = add %a, %b
    ret %t0
}

# LLVM IR:
define i64 @جمع(i64 %a, i64 %b) {
entry:
  %t0 = add i64 %a, %b
  ret i64 %t0
}
```

**المهام:**
- [ ] LLVM context setup
- [ ] Type conversion
- [ ] Function emission
- [ ] Instruction translation
- [ ] Control flow handling
- [ ] اختبارات CodeGen (100+ tests)

---

### 2. Runtime Integration (أسبوعان)

```cpp
// runtime/llvm_runtime.cpp

# Runtime functions للـ LLVM backend
extern "C" {
    # Memory
    void* sad_llvm_alloc(uint64_t size);
    void sad_llvm_free(void* ptr);
    
    # Arrays
    void* sad_llvm_array_new(uint64_t size);
    void* sad_llvm_array_get(void* arr, uint64_t index);
    void sad_llvm_array_set(void* arr, uint64_t index, void* value);
    
    # Strings
    void* sad_llvm_string_concat(void* a, void* b);
    uint64_t sad_llvm_string_len(void* str);
    
    # I/O
    void sad_llvm_print(void* value);
    void* sad_llvm_input();
    
    # GC
    void sad_llvm_gc_register(void* ptr);
    void sad_llvm_gc_collect();
}
```

**المهام:**
- [ ] Runtime function declarations
- [ ] LLVM → Runtime linking
- [ ] GC integration
- [ ] Exception handling
- [ ] اختبارات Runtime (40+ tests)

---

### 3. Optimization Pipeline (أسبوعان)

```cpp
// compiler/backends/llvm/optimizer.h
class LLVMOptimizer {
private:
    مؤشر<llvm::PassManager> pass_manager
    
public:
    دالة setup_pipeline(رقم opt_level) -> فارغ
    دالة optimize(مؤشر<llvm::Module> module) -> فارغ
    
    # Optimization levels
    دالة add_O0_passes() -> فارغ   # No optimization
    دالة add_O1_passes() -> فارغ   # Basic
    دالة add_O2_passes() -> فارغ   # Standard
    دالة add_O3_passes() -> فارغ   # Aggressive
نهاية
```

**LLVM Optimization Passes:**

**-O1 (Basic):**
- mem2reg (promote memory to registers)
- simplifycfg (simplify control flow)
- instcombine (combine instructions)
- reassociate (reassociate expressions)

**-O2 (Standard):**
- All -O1 passes
- loop-unroll (unroll small loops)
- gvn (global value numbering)
- sccp (sparse conditional constant propagation)
- adce (aggressive dead code elimination)
- inline (function inlining)

**-O3 (Aggressive):**
- All -O2 passes
- aggressive inline
- vectorization
- loop-unroll (more aggressive)
- polly (polyhedral optimizations)

**المهام:**
- [ ] Pass manager setup
- [ ] Optimization level configs
- [ ] Custom passes (optional)
- [ ] Performance benchmarks
- [ ] اختبارات Optimization (50+ tests)

---

### 4. Cross-Compilation (أسبوعان)

```cpp
// compiler/backends/llvm/target.h
class TargetManager {
public:
    دالة setup_target(نص triple) -> مؤشر<llvm::TargetMachine>
    دالة get_supported_targets() -> مصفوفة<نص>
    دالة emit_object_file(مؤشر<llvm::Module> module, نص output) -> منطقي
نهاية
```

**Supported Targets:**
```bash
# x86_64
sadc program.s -o program --target=x86_64-linux-gnu
sadc program.s -o program --target=x86_64-windows-msvc
sadc program.s -o program --target=x86_64-macos

# ARM64
sadc program.s -o program --target=aarch64-linux-gnu
sadc program.s -o program --target=aarch64-macos

# WASM
sadc program.s -o program.wasm --target=wasm32-unknown-unknown

# RISC-V
sadc program.s -o program --target=riscv64-linux-gnu
```

**المهام:**
- [ ] Target triple parsing
- [ ] Target machine setup
- [ ] Object file emission
- [ ] Linker integration
- [ ] Testing on multiple targets
- [ ] اختبارات Cross-compilation (30+ tests)

---

### 5. Debug Info Generation (أسبوع واحد)

```cpp
// compiler/backends/llvm/debug_info.h
class DebugInfoGenerator {
private:
    مؤشر<llvm::DIBuilder> di_builder
    مؤشر<llvm::DICompileUnit> compile_unit
    
public:
    دالة generate_debug_info(مؤشر<SIRModule> sir) -> فارغ
    
    # Debug info for functions
    دالة emit_function_debug_info(مؤشر<SIRFunction> func) -> مؤشر<llvm::DISubprogram>
    
    # Debug info for variables
    دالة emit_variable_debug_info(نص name, مؤشر<Type> type) -> مؤشر<llvm::DILocalVariable>
    
    # Source locations
    دالة emit_location(رقم line, رقم column) -> مؤشر<llvm::DILocation>
نهاية
```

**DWARF Debug Info:**
- Function names and types
- Variable names and locations
- Source line mapping
- Call stack information

**Usage:**
```bash
# Compile with debug info
sadc program.s -o program -g

# Debug with GDB
gdb program
(gdb) break main
(gdb) run
(gdb) print variable
```

**المهام:**
- [ ] DIBuilder setup
- [ ] Function debug info
- [ ] Variable debug info
- [ ] Line number mapping
- [ ] GDB/LLDB testing
- [ ] اختبارات Debug (20+ tests)

---

### 6. Linker Integration (أسبوع واحد)

```cpp
// compiler/backends/llvm/linker.h
class Linker {
public:
    دالة link_objects(مصفوفة<نص> object_files, نص output) -> منطقي
    دالة link_with_runtime(نص object_file, نص output) -> منطقي
    دالة create_shared_library(مصفوفة<نص> objects, نص output) -> منطقي
نهاية
```

**Linking:**
```bash
# Static linking
sadc program.s -o program --static

# Dynamic linking
sadc program.s -o program -lsad

# Shared library
sadc lib.s -o libsad.so --shared
```

**المهام:**
- [ ] LLD integration
- [ ] Static linking
- [ ] Dynamic linking
- [ ] Shared library creation
- [ ] اختبارات Linking (20+ tests)

---

## 📊 معايير النجاح / Success Criteria

✅ **المرحلة مكتملة عندما:**
- [ ] LLVM IR generator يعمل 100%
- [ ] Native executables تعمل
- [ ] Performance 50-100x من المفسّر
- [ ] Cross-compilation يعمل
- [ ] Debug info كامل
- [ ] Test coverage > 80%
- [ ] جميع Stage 1 tests تمر

---

## 🎯 Performance Targets

| Benchmark | المفسّر | Bytecode | LLVM -O0 | LLVM -O2 | C (gcc -O2) |
|-----------|---------|----------|----------|----------|-------------|
| Fibonacci(35) | 5000ms | 500ms | 200ms | 50ms | 45ms |
| Array Sort | 2000ms | 200ms | 80ms | 20ms | 18ms |
| String Ops | 3000ms | 300ms | 120ms | 30ms | 28ms |

---

**السابق / Previous:** [المرحلة 3: Bytecode Backend](06_phase3_bytecode.md)  
**التالي / Next:** [المرحلة 5: Runtime & ABI](08_phase5_runtime.md)
