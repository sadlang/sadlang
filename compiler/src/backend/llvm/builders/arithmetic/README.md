# الفئة: arithmetic/ — الحساب وتحكم التدفق (تنفيذ)

> **الموقع:** `compiler/src/backend/llvm/builders/arithmetic/`
> **النوع:** ملفات تنفيذ (`.cpp`)
> **الـ headers المقابلة في:** `compiler/include/backend/llvm/builders/arithmetic/`

## الوصف
كل العمليات الحسابية، المقارنات، التحكم في التدفق (if/while/for/match)، والاستثناءات.

## ملفات التنفيذ في هذه الفئة

| Sub-CodeGen | ملفات Ops | الدور |
|---|---|---|
| `arithmetic_codegen.h` | `arith_{main,cmp,extras,resolve,type_conv}.cpp` | العمليات الحسابية (+, -, *, /, %, **) ومقارنات (==, !=, <, >, ...) — مقسّم لـ 5 ملفات لتقليل وقت التجميع |
| `controlflow_codegen.h` | `cf_{branch_call,return_switch}.cpp` | if/else, while, for, match/when, switch, return — يحفظ ترتيب الإدراج للكتل (BasicBlock ordering) |
| `exception_codegen.h` | `exception_ops.cpp` | try/catch/throw/finally باستخدام Itanium C++ ABI |

## نمط ملف ops

```cpp
#include "builders/arithmetic/X_codegen.h"
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
