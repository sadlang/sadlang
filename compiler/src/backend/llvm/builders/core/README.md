# الفئة: core/ — الأساسيات (تنفيذ)

> **الموقع:** `compiler/src/backend/llvm/builders/core/`
> **النوع:** ملفات تنفيذ (`.cpp`)
> **الـ headers المقابلة في:** `compiler/include/backend/llvm/builders/core/`

## الوصف
الـ sub-codegens الأساسية التي يعتمد عليها الباقي: dispatcher، إخراج الملفات، تحويل الأنواع، ودعم freestanding/kernel.

## ملفات التنفيذ في هذه الفئة

| Sub-CodeGen | ملفات Ops | الدور |
|---|---|---|
| `instr_core_codegen.h` | `instr_core_ops.cpp` | dispatcher مركزي يوزّع تعليمات SIR إلى الـ sub-codegen المناسب |
| `output_codegen.h` | `output_ops.cpp` | كتابة LLVM IR إلى ملفات .ll و .o ومعالجة linker output |
| `types_codegen.h` | `types_ops.cpp` | تحويل أنواع SIR إلى LLVM (17 method): convertType, getInt*Type, getConstant* |
| `freestanding_codegen.h` | `freestanding_{conv,io,sprintf}_ops.cpp + freestanding_ops.cpp` | دعم وضع freestanding/kernel بدون C runtime — لـ Ufuq OS وUEFI |

## نمط ملف ops

```cpp
#include "builders/core/X_codegen.h"
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
