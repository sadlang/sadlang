# الفئة: memory/ — الذاكرة والإغلاقات (تنفيذ)

> **الموقع:** `compiler/src/backend/llvm/builders/memory/`
> **النوع:** ملفات تنفيذ (`.cpp`)
> **الـ headers المقابلة في:** `compiler/include/backend/llvm/builders/memory/`

## الوصف
إدارة ذاكرة المكدس (alloca/load/store)، الإغلاقات، والبنى المجمعة.

## ملفات التنفيذ في هذه الفئة

| Sub-CodeGen | ملفات Ops | الدور |
|---|---|---|
| `memory_codegen.h` | `mem_{alloca,load,store}.cpp` | إدارة ذاكرة المكدس + ownership tracking (Phase 6 SIR opcodes) |
| `closure_codegen.h` | `closure_ops.cpp` | بناء الإغلاقات (closures) ودعم capture-by-value و capture-by-reference |
| `aggregate_ops_codegen.h` | `agg_ops.cpp` | البنى المجمعة (struct literals, array literals) عبر insertvalue/extractvalue |

## نمط ملف ops

```cpp
#include "builders/memory/X_codegen.h"
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
