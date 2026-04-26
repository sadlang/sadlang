# LLVM CodeGen Structure Analysis / تحليل بنية مولد كود LLVM

## Summary / ملخص
The file `llvm_codegen.cpp` contains **8503 lines** with **258+ member functions**.

## Line Ranges by Category / نطاقات الأسطر حسب الفئة

| Category / الفئة | Start | End | ~Lines |
|-----------------|-------|-----|--------|
| **Core** (Constructor, Types, Initialize) | 1 | 590 | 590 |
| **Module** (emitModule, preprocessClasses) | 590 | 1060 | 470 |
| **Functions** (emitFunction, emitFunctionBody, emitInstruction) | 1060 | 1877 | 817 |
| **Arithmetic** (emitAdd, emitSub, emitMul... emitShr) | 1877 | 2307 | 430 |
| **Comparison** (emitCmpEq, emitCmpNe... emitCmpGe) | 2307 | 2697 | 390 |
| **Memory** (emitLoad, emitStore, emitAlloca, emitMove, emitGEP) | 2697 | 3270 | 573 |
| **Control Flow** (emitBranch, emitCondBranch, emitCall, emitReturn, emitSwitch) | 3270 | 3605 | 335 |
| **Output** (emitToFile, emitAssembly, emitObjectFile, dump) | 3605 | 3780 | 175 |
| **Operand Resolution & Strings** (resolveOperand, emitStringConcat) | 3780 | 4136 | 356 |
| **OS/Hardware** (emitInlineAsm, Port, Memory, Serial, GPIO, Timer, DMA) | 4136 | 4640 | 504 |
| **FFI** (emitFFIPrintf, emitFFIMalloc... emitFFISnprintf) | 4640 | 4920 | 280 |
| **Security** (emitBuiltinSecurityAssert... emitBuiltinSecurityBase64Encode) | 4920 | 5170 | 250 |
| **Core Builtins** (emitBuiltinPrint, emitBuiltinSqrt... emitBuiltinRandom) | 5170 | 5406 | 236 |
| **Async** (emitAsyncSpawn... emitAsyncSelect) | 5406 | 5891 | 485 |
| **More Bitwise** (emitSar, emitRol, emitCallIndirect) | 5891 | 5977 | 86 |
| **Memory/Pointer Ops** (emitAllocHeap, emitFreeMem, emitPtrAdd, emitPtrCast) | 5977 | 6089 | 112 |
| **OOP** (emitObjectNew, emitObjectGet, emitObjectSet, emitInstanceOf...) | 6089 | 6435 | 346 |
| **Type Conversions** (emitI64ToF64, emitF64ToI64, emitCast...) | 6435 | 6683 | 248 |
| **Arrays** (emitArrayNew, emitArrayGet, emitArraySet, emitArrayLen) | 6683 | 6874 | 191 |
| **More Builtins** (emitBuiltinMin, emitBuiltinMax, emitBuiltinAssert) | 6874 | 6998 | 124 |
| **PHI & TypeOf** (emitPhi, emitBuiltinTypeOf) | 6998 | 7083 | 85 |
| **String Builtins** (toUpper, toLower, find, replace, substring, trim, split, join...) | 7083 | 7647 | 564 |
| **Array Builtins** (append, remove, indexOf, contains, reverse, sort, slice) | 7647 | 8010 | 363 |
| **File Builtins** (read, write, append, delete, copy, move, createDir, listDir) | 8010 | 8290 | 280 |
| **Low-level Casts** (emitBitCast, emitIntToPtr, emitPtrToInt, emitTrunc, emitZExt...) | 8290 | 8503 | 213 |

## Key Functions / الدوال الرئيسية

### Entry Points / نقاط الدخول
- `generate()` - Main SIR to LLVM IR generation entry point (line ~664)
- `emitToFile()` - Write LLVM IR to file (line ~3605)
- `emitAssembly()` - Generate assembly output (line ~3649)
- `emitObjectFile()` - Generate object file (line ~3701)

### Core Dispatch / التوزيع الأساسي
- `emitInstruction()` - Main switch statement handling all SIR opcodes (line ~1392)
  - This is a ~485 line switch statement

## If You Want to Split / إذا أردت التقسيم

To properly split this file:

1. Create a new `.cpp` file in the same directory
2. Copy the includes from lines 1-45:
   ```cpp
   #include "llvm_codegen.h"
   #include "llvm_optimizer.h"
   // ... other includes ...
   using namespace Sad::Compiler::SIR;
   namespace Sad {
   namespace LLVM {
   ```
3. Extract the desired functions by line range
4. Close namespaces at the end:
   ```cpp
   } // namespace LLVM
   } // namespace Sad
   ```
5. CMake will automatically include it (uses `file(GLOB ...)`)

## Backup / النسخة الاحتياطية

A backup exists at:
- `llvm_codegen_original.cpp.bak` (373KB, 8503 lines)

## Build Command / أمر البناء

```bash
cmake --build build --config Debug --target sad_new_llvm_backend
```
