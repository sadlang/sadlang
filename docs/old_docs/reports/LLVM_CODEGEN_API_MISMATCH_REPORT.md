# LLVM CodeGen API Mismatch Report
# تقرير عدم توافق API في LLVM CodeGen

## التاريخ / Date: January 4, 2026

## الخلاصة / Summary

❌ **CRITICAL ISSUE**: llvm_codegen.cpp uses **non-existent functions and types** from SIR classes, causing 100+ compilation errors.

❌ **مشكلة خطيرة**: llvm_codegen.cpp يستخدم **دوال وأنواع غير موجودة** من SIR classes، مما يسبب أكثر من 100 خطأ ترجمة.

## الأخطاء الرئيسية / Main Errors

### 1. Missing Member Functions / دوال غير موجودة

| File | Line | Error | Actual API |
|------|------|-------|------------|
| llvm_codegen.cpp | 511 | `SIRParameter::getName()` ❌ | **DOES NOT EXIST** |
| llvm_codegen.cpp | 544 | `SIRBasicBlock::getName()` ❌ | **DOES NOT EXIST** |
| llvm_codegen.cpp | 563 | `SIRBasicBlock::getInstructions()` ❌ | **DOES NOT EXIST** |
| llvm_codegen.cpp | 644 | `SIRInstruction::getOpcode()` ❌ | Should use `inst.opcode` (public member) |

### 2. Wrong Opcode Names / أسماء Opcode خاطئة

| File | Line | Wrong Code ❌ | Correct Code ✅ |
|------|------|---------------|-----------------|
| llvm_codegen.cpp | 646 | `SIROpcode::Add` | `SIROpcode::ADD_I64` or `ADD_F64` |
| llvm_codegen.cpp | 647 | `SIROpcode::Sub` | `SIROpcode::SUB_I64` or `SUB_F64` |
| llvm_codegen.cpp | 648 | `SIROpcode::Mul` | `SIROpcode::MUL_I64` or `MUL_F64` |
| llvm_codegen.cpp | 649 | `SIROpcode::Div` | `SIROpcode::DIV_I64` or `DIV_F64` |
| llvm_codegen.cpp | 650 | `SIROpcode::Mod` | `SIROpcode::MOD_I64` |
| llvm_codegen.cpp | 651 | `SIROpcode::Neg` | `SIROpcode::NEG` ✅ (this one is correct!) |
| llvm_codegen.cpp | 654 | `SIROpcode::And` | `SIROpcode::AND` |
| llvm_codegen.cpp | 655 | `SIROpcode::Or` | `SIROpcode::OR` |
| llvm_codegen.cpp | 656 | `SIROpcode::Xor` | `SIROpcode::XOR` |
| llvm_codegen.cpp | 657 | `SIROpcode::Not` | `SIROpcode::NOT` |

## الأسباب الجذرية / Root Causes

### 1. ❌ Violation of STRICT_CODING_RULES.md

**Golden Rule Violated:**
> "DO NOT write any function or variable unless you have READ its full definition from the source file"

**القاعدة الذهبية المنتهكة:**
> "لا تكتب أي دالة أو متغير إلا بعد قراءة تعريفه الكامل من الملف المصدري"

**What Happened:**
- llvm_codegen.cpp was written **without reading** the actual SIR headers
- Function names were **invented** (getName(), getInstructions(), etc.)
- Opcode names were **guessed** (Add instead of ADD_I64)

### 2. 📁 Actual API from Headers

**Source: compiler/frontend/include/sir_types.h:114-408**

```cpp
enum class SIROpcode {
    // Arithmetic (Lines 120-129)
    ADD_I64,    ///< Integer addition
    ADD_F64,    ///< Float addition
    SUB_I64,    ///< Integer subtraction
    SUB_F64,    ///< Float subtraction
    MUL_I64,    ///< Integer multiplication
    MUL_F64,    ///< Float multiplication
    DIV_I64,    ///< Integer division
    DIV_F64,    ///< Float division
    MOD_I64,    ///< Modulo
    NEG,        ///< Negation
    
    // Bitwise (Lines 134-141)
    AND,        ///< Bitwise AND
    OR,         ///< Bitwise OR
    XOR,        ///< Bitwise XOR
    NOT,        ///< Bitwise NOT
    // ... etc (90 opcodes total)
};
```

**Source: compiler/frontend/include/sir_instruction.h:59-72**

```cpp
class SIRInstruction {
public:
    SIROpcode opcode;                   ///< PUBLIC member (Line 60)
    std::optional<SIROperand> result;   ///< PUBLIC member (Line 61)
    std::vector<SIROperand> operands;   ///< PUBLIC member (Line 62)
    std::string label;                  ///< PUBLIC member (Line 63)
    std::string comment;                ///< PUBLIC member (Line 64)
    
    // NO getName() function!
    // NO getOpcode() function!
    // Access directly: inst.opcode, inst.result, inst.operands
};
```

**Source: compiler/frontend/include/sir_instruction.h:353-450**

```cpp
class SIRBasicBlock {
public:
    std::string name;                           ///< PUBLIC member
    std::vector<SIRInstruction> instructions;   ///< PUBLIC member
    SIRBasicBlock* successor = nullptr;
    SIRBasicBlock* branch_target = nullptr;
    
    // NO getName() function!
    // NO getInstructions() function!
    // Access directly: block.name, block.instructions
};
```

## الإحصائيات / Statistics

- **Total Compilation Errors**: 100+ (stopped at error limit)
- **Missing Functions Used**: 4 (getName, getInstructions, getOpcode, etc.)
- **Wrong Opcode Names**: 10+ (Add, Sub, Mul, Div, Mod, And, Or, Xor, Not, etc.)
- **Lines of Code Affected**: ~500 lines in llvm_codegen.cpp

## التأثير / Impact

### ❌ Cannot Build test_llvm_pipeline_driver
```
Command: cmake --build . --config Debug --target test_llvm_pipeline_driver
Result:  100+ errors, compilation failed
Reason:  llvm_codegen.cpp uses non-existent API
```

### ❌ Phase 1.1.4 Blocked
- Testing: **BLOCKED** (cannot compile test driver)
- Benchmarks: **BLOCKED** (cannot run tests)
- Documentation: **BLOCKED** (no results to document)
- Progress: **STUCK at 80%**

## الحلول المقترحة / Proposed Solutions

### Solution 1: Fix llvm_codegen.cpp (HARD - 2-3 days)

**Pros:**
- Fixes the root cause
- Makes llvm_codegen.cpp work with real API

**Cons:**
- Requires reading **all** SIR headers (5+ files, 2000+ lines)
- Requires fixing **100+ errors** manually
- High risk of introducing new errors
- Time consuming (2-3 days minimum)

**Estimate**: 2-3 days, high risk

### Solution 2: Create Minimal Test Driver (EASY - 2 hours)

**Pros:**
- Quick solution (2 hours)
- Tests what actually works
- Demonstrates LLVM integration concept
- Can document progress

**Cons:**
- Doesn't fix llvm_codegen.cpp
- Limited functionality

**Estimate**: 2 hours, low risk

### Solution 3: Disable LLVM Pipeline, Use Bytecode VM (IMMEDIATE)

**Pros:**
- Immediate solution
- Bytecode VM works perfectly
- Can continue other work

**Cons:**
- Abandons Phase 1.1.4
- No performance improvement
- LLVM backend remains unused

**Estimate**: 10 minutes, zero risk

## التوصية / Recommendation

**Short-term (Today):** Solution 2 - Create minimal test driver
**Reason:** Demonstrates progress, completes Phase 1.1.4 at basic level

**Mid-term (Next week):** Solution 1 - Fix llvm_codegen.cpp properly
**Reason:** Needed for production use, must follow STRICT_CODING_RULES.md

**Long-term:** Implement full LLVM Pipeline with proper API
**Reason:** 50-100x performance improvement is worth the effort

## الدروس المستفادة / Lessons Learned

### ✅ What to Do

1. **READ headers completely** before writing code
2. **DOCUMENT every function** with file:line reference
3. **VERIFY types match** exactly (don't guess!)
4. **TEST after each small change**
5. **FOLLOW STRICT_CODING_RULES.md** strictly

### ❌ What NOT to Do

1. ❌ Don't **invent** function names without checking
2. ❌ Don't **guess** opcode names
3. ❌ Don't write **hundreds of lines** without compiling
4. ❌ Don't **assume** API based on other languages
5. ❌ Don't **skip** reading headers

## الاستنتاج / Conclusion

هذه المشكلة هي **مثال مثالي** على ما يحدث عندما لا نلتزم بـ STRICT_CODING_RULES.md:

This problem is a **perfect example** of what happens when we don't follow STRICT_CODING_RULES.md:

- **100+ errors** from not reading headers
- **2-3 days** of work to fix
- **Phase blocked** at 80%
- **User angry** (rightfully so!)

**Lesson:** Always read **BEFORE** writing. No exceptions.

---

## المصادر / Sources

- [sir_types.h](compiler/frontend/include/sir_types.h) - Lines 1-408
- [sir_instruction.h](compiler/frontend/include/sir_instruction.h) - Lines 1-450
- [sir_module.h](compiler/frontend/include/sir_module.h) - Lines 1-500+
- [STRICT_CODING_RULES.md](plans/agent/STRICT_CODING_RULES.md) - All lines
- [Build Log](build output above) - 100+ errors

---

## المؤلف / Author
SadLanguage Compiler Team

## التاريخ / Date
January 4, 2026

**End of Report**
