# المرحلة 3: Bytecode Backend
# Phase 3: Bytecode Backend

**المدة / Duration:** 4-6 أسابيع / weeks  
**الأولوية / Priority:** P0 (حرجة / Critical)  
**الحالة / Status:** 🔴 لم تبدأ / Not Started

---

## 📋 نظرة عامة / Overview

### (AR) العربية

تطوير Bytecode backend كامل:
- تصميم Bytecode format
- Bytecode emitter (SIR → Bytecode)
- Virtual Machine implementation
- Runtime library (libsad)
- File format (.sbc)

**الهدف:** 5-10x أسرع من المفسّر الحالي

---

## 🎯 Bytecode Design / تصميم البايت كود

### (AR) العربية

**Stack-based VM مع register hints:**

```
# Instruction Format (variable length)
┌────────┬───────────┬───────────┬───────────┐
│ Opcode │ Operand 1 │ Operand 2 │ Operand 3 │
│ 1 byte │ 0-8 bytes │ 0-8 bytes │ 0-8 bytes │
└────────┴───────────┴───────────┴───────────┘

# Opcodes (0x00 - 0xFF)

## Stack Operations (0x00-0x0F)
0x00  NOP              # No operation
0x01  PUSH_I64  val    # Push 64-bit integer
0x02  PUSH_F64  val    # Push 64-bit float
0x03  PUSH_STR  idx    # Push string from pool
0x04  PUSH_NULL        # Push null
0x05  PUSH_TRUE        # Push true
0x06  PUSH_FALSE       # Push false
0x07  POP              # Pop and discard
0x08  DUP              # Duplicate top
0x09  SWAP             # Swap top two

## Arithmetic (0x10-0x1F)
0x10  ADD              # a + b
0x11  SUB              # a - b
0x12  MUL              # a * b
0x13  DIV              # a / b
0x14  MOD              # a % b
0x15  NEG              # -a

## Bitwise (0x20-0x2F)
0x20  AND              # a & b
0x21  OR               # a | b
0x22  XOR              # a ^ b
0x23  NOT              # ~a
0x24  SHL              # a << b
0x25  SHR              # a >> b

## Comparison (0x30-0x3F)
0x30  EQ               # a == b
0x31  NE               # a != b
0x32  LT               # a < b
0x33  LE               # a <= b
0x34  GT               # a > b
0x35  GE               # a >= b

## Control Flow (0x40-0x4F)
0x40  JMP      offset  # Unconditional jump
0x41  JMP_IF   offset  # Jump if true
0x42  JMP_NOT  offset  # Jump if false
0x43  CALL     idx     # Call function
0x44  RET              # Return
0x45  RET_VAL          # Return with value

## Variables (0x50-0x5F)
0x50  LOAD_LOCAL   idx # Load local variable
0x51  STORE_LOCAL  idx # Store local variable
0x52  LOAD_GLOBAL  idx # Load global variable
0x53  STORE_GLOBAL idx # Store global variable
0x54  LOAD_CAPTURE idx # Load captured variable

## Memory (0x60-0x6F)
0x60  ALLOC            # Allocate memory
0x61  FREE             # Free memory
0x62  LOAD_PTR         # Load from pointer
0x63  STORE_PTR        # Store to pointer
0x64  ADDR_OF          # Get address
0x65  DEREF            # Dereference pointer

## Arrays (0x70-0x7F)
0x70  ARRAY_NEW        # Create array
0x71  ARRAY_GET        # Get element
0x72  ARRAY_SET        # Set element
0x73  ARRAY_LEN        # Get length

## Objects (0x80-0x8F)
0x80  OBJ_NEW    idx   # Create object
0x81  OBJ_GET    idx   # Get field
0x82  OBJ_SET    idx   # Set field

## Builtins (0x90-0x9F)
0x90  PRINT            # Print to stdout
0x91  INPUT            # Read from stdin
0x92  LEN              # Length builtin
```

---

## 📝 المهام / Tasks

### 1. Bytecode Emitter (أسبوع واحد)

```cpp
// compiler/backends/bytecode/emitter.h
class BytecodeEmitter {
private:
    مصفوفة<بايت> code
    مصفوفة<نص> string_pool
    مصفوفة<رقم> constant_pool
    قاموس<نص، رقم> labels
    
public:
    دالة emit(مؤشر<SIRModule> module) -> مصفوفة<بايت>
    
    # Emit instructions
    دالة emit_instruction(Opcode op) -> فارغ
    دالة emit_with_operand(Opcode op, رقم operand) -> فارغ
    
    # Constants
    دالة add_string(نص str) -> رقم
    دالة add_constant(رقم val) -> رقم
    
    # Labels
    دالة create_label(نص name) -> رقم
    دالة patch_jump(رقم offset) -> فارغ
نهاية
```

**المهام:**
- [ ] تطبيق Bytecode format
- [ ] تطبيق Emitter
- [ ] SIR → Bytecode translation
- [ ] Constant pool management
- [ ] Label resolution
- [ ] اختبارات Emitter (40+ tests)

---

### 2. Virtual Machine (أسبوعان)

```cpp
// vm/vm.h
class SadVM {
private:
    # Stack
    مصفوفة<Value> stack
    رقم stack_top
    
    # Call frames
    مصفوفة<CallFrame> frames
    رقم frame_count
    
    # Globals
    قاموس<نص، Value> globals
    
    # Bytecode
    مصفوفة<بايت> code
    رقم ip  # instruction pointer
    
    # Runtime
    مؤشر<Heap> heap
    مؤشر<GC> gc
    
public:
    دالة execute(مصفوفة<بايت> bytecode) -> Value
    
    # Stack operations
    دالة push(Value value) -> فارغ
    دالة pop() -> Value
    دالة peek(رقم distance) -> Value
    
    # Instruction dispatch
    دالة run() -> InterpretResult
    دالة dispatch() -> فارغ
    
    # Memory
    دالة allocate(رقم size) -> مؤشر<فارغ>
    دالة free(مؤشر<فارغ> ptr) -> فارغ
نهاية

class CallFrame {
    مؤشر<Function> function
    رقم ip
    مؤشر<Value> slots
نهاية

class Value {
    ValueType type
    union {
        رقم number
        منطقي boolean
        مؤشر<نص> string
        مؤشر<فارغ> pointer
    } as
نهاية
```

**VM Features:**
- Stack-based execution
- Function call frames
- Global variables
- Heap allocation
- Simple GC (mark & sweep)

**المهام:**
- [ ] تطبيق Value system
- [ ] تطبيق Stack
- [ ] تطبيق Call frames
- [ ] Instruction dispatch loop
- [ ] جميع opcodes (90+)
- [ ] اختبارات VM (80+ tests)

---

### 3. Runtime Library (أسبوعان)

```cpp
// runtime/libsad.h

# Memory Management
دالة sad_alloc(رقم size) -> مؤشر<فارغ>
دالة sad_free(مؤشر<فارغ> ptr) -> فارغ
دالة sad_realloc(مؤشر<فارغ> ptr, رقم size) -> مؤشر<فارغ>

# I/O
دالة sad_print(Value value) -> فارغ
دالة sad_input() -> نص
دالة sad_read_file(نص path) -> نص
دالة sad_write_file(نص path, نص content) -> منطقي

# Stage 1 Support
دالة sad_malloc(رقم size) -> مؤشر<فارغ>
دالة sad_mmap(رقم addr, رقم size) -> مؤشر<فارغ>
دالة sad_io_read(رقم port) -> رقم
دالة sad_io_write(رقم port, رقم value) -> فارغ

# Arrays
دالة sad_array_new(رقم size) -> مؤشر<Array>
دالة sad_array_get(مؤشر<Array> arr, رقم index) -> Value
دالة sad_array_set(مؤشر<Array> arr, رقم index, Value val) -> فارغ
دالة sad_array_len(مؤشر<Array> arr) -> رقم

# Strings
دالة sad_string_concat(نص a, نص b) -> نص
دالة sad_string_substr(نص str, رقم start, رقم len) -> نص
دالة sad_string_len(نص str) -> رقم

# GC
دالة sad_gc_collect() -> فارغ
دالة sad_gc_enable() -> فارغ
دالة sad_gc_disable() -> فارغ
```

**المهام:**
- [ ] Memory management
- [ ] I/O functions
- [ ] Array operations
- [ ] String operations
- [ ] Stage 1 builtins
- [ ] Simple GC
- [ ] اختبارات Runtime (60+ tests)

---

### 4. File Format (.sbc)

```
┌─────────────────────────────────────────┐
│ Header (32 bytes)                       │
├─────────────────────────────────────────┤
│ Magic Number:    "SADC" (4 bytes)      │
│ Version:         0x0001 (2 bytes)      │
│ Flags:           0x0000 (2 bytes)      │
│ Entry Point:     offset (8 bytes)      │
│ Checksum:        CRC32 (4 bytes)       │
│ Reserved:        (12 bytes)            │
├─────────────────────────────────────────┤
│ String Pool (variable)                  │
├─────────────────────────────────────────┤
│ Count:           N (4 bytes)           │
│ String 0:        length + data         │
│ String 1:        length + data         │
│ ...                                     │
├─────────────────────────────────────────┤
│ Constant Pool (variable)                │
├─────────────────────────────────────────┤
│ Count:           N (4 bytes)           │
│ Constant 0:      type + value          │
│ Constant 1:      type + value          │
│ ...                                     │
├─────────────────────────────────────────┤
│ Function Table (variable)               │
├─────────────────────────────────────────┤
│ Count:           N (4 bytes)           │
│ Function 0:                             │
│   Name:          string_idx (4 bytes)  │
│   Parameters:    count (2 bytes)       │
│   Locals:        count (2 bytes)       │
│   Code Size:     size (4 bytes)        │
│   Code:          bytecode              │
│ Function 1:      ...                    │
├─────────────────────────────────────────┤
│ Global Variables (variable)             │
├─────────────────────────────────────────┤
│ Count:           N (4 bytes)           │
│ Variable 0:      name + type + init    │
│ ...                                     │
└─────────────────────────────────────────┘
```

**المهام:**
- [ ] تصميم File format
- [ ] Writer (emit .sbc)
- [ ] Loader (read .sbc)
- [ ] Verification
- [ ] اختبارات Format (20+ tests)

---

## 📊 معايير النجاح / Success Criteria

✅ **المرحلة مكتملة عندما:**
- [ ] Bytecode emitter يعمل 100%
- [ ] VM يشغّل جميع Stage 1 tests
- [ ] Performance 5-10x من المفسّر
- [ ] Runtime library كاملة
- [ ] File format stable
- [ ] Test coverage > 85%
- [ ] Memory leaks = 0

---

**السابق / Previous:** [المرحلة 2: Middle-end](05_phase2_middleend.md)  
**التالي / Next:** [المرحلة 4: LLVM Backend](07_phase4_llvm.md)
