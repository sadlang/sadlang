# الفئة: builtins/ — الدوال المضمنة (تنفيذ)

> **الموقع:** `compiler/src/backend/llvm/builders/builtins/`
> **النوع:** ملفات تنفيذ (`.cpp`)
> **الـ headers المقابلة في:** `compiler/include/backend/llvm/builders/builtins/`

## الوصف
الدوال الجاهزة في اللغة (مع/بدون استيراد): الإخراج/الإدخال، الرياضيات، الشبكة، الأمان.

## ملفات التنفيذ في هذه الفئة

| Sub-CodeGen | ملفات Ops | الدور |
|---|---|---|
| `builtin_funcs_codegen.h` | `builtin_funcs_ops.cpp + builtins_math_async_ops.cpp` | الدوال المضمنة بدون استيراد: اطبع, طول, نوع, رقم(), نص(), منطقي(), ... |
| `io_builtins_codegen.h` | `io_builtins_ops.cpp` | اقرأ, اطبع_سطر, file I/O أساسي |
| `math_builtins_codegen.h` | `math_builtins.cpp` | sin, cos, log, exp, pow, sqrt + ثوابت π و e |
| `network_builtins_codegen.h` | `builtins_network_ops.cpp` | TCP/UDP, HTTP بدائي — يربط مكتبة sad_network |
| `security_builtins_codegen.h` | `security_builtins_ops.cpp` | التشفير: SHA-256, AES-CTR |

## نمط ملف ops

```cpp
#include "builders/builtins/X_codegen.h"
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
