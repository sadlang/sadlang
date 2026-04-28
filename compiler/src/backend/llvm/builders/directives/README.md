# الفئة: directives/ — التوجيهات والتزامن (تنفيذ)

> **الموقع:** `compiler/src/backend/llvm/builders/directives/`
> **النوع:** ملفات تنفيذ (`.cpp`)
> **الـ headers المقابلة في:** `compiler/include/backend/llvm/builders/directives/`

## الوصف
توجيهات @، SIMD، goroutines/channels، و coroutines (async/await).

## ملفات التنفيذ في هذه الفئة

| Sub-CodeGen | ملفات Ops | الدور |
|---|---|---|
| `directives_codegen.h` | `directives_ops.cpp` | توجيهات @: @غير_آمن, @وقت_الترجمة, @متطاير, @حجم |
| `simd_codegen.h` | `simd_ops.cpp` | تعليمات SIMD (Phase 3) — vector types و operations |
| `concurrency_codegen.h` | `concurrency_ops.cpp + channels_ops.cpp` | goroutines, channels (SadChannel), mutex, futures, WaitGroup |
| `coroutines_codegen.h` | `coroutines_ops.cpp` | async/await باستخدام LLVM coroutine intrinsics (@llvm.coro.*) |

## نمط ملف ops

```cpp
#include "builders/directives/X_codegen.h"
#include "backend/llvm/llvm_codegen.h"

using namespace Sad::LLVM;

llvm::Value *XCodeGen::generateY(const SIR::Instruction &inst) {
    auto *operand = cg_.getLLVMValue(inst.operands[0]);
    if (!operand) {
        cg_.reportError("Invalid operand");
        return nullptr;
    }
    return cg_.builder_->CreateY(operand, "y_result");
}
```

## مراجع
- [الوثيقة المعمارية الكاملة](../../../../../../docs/compiler_architecture_phase10.md)
- [مخططات Mermaid](../../../../../../docs/compiler_architecture_diagrams.md)
