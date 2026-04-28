# الفئة: oop/ — البرمجة الكائنية (تنفيذ)

> **الموقع:** `compiler/src/backend/llvm/builders/oop/`
> **النوع:** ملفات تنفيذ (`.cpp`)
> **الـ headers المقابلة في:** `compiler/include/backend/llvm/builders/oop/`

## الوصف
كل ما يتعلق بالأصناف: vtables، إنشاء الكائنات، الحقول، الباني/الهادم، الدوال، والتعدادات.

## ملفات التنفيذ في هذه الفئة

| Sub-CodeGen | ملفات Ops | الدور |
|---|---|---|
| `classes_vtables_codegen.h` | `classes_vtables_ops.cpp` | بناء جداول الدوال الافتراضية (vtables) و dynamic dispatch |
| `oop_ops_codegen.h` | `oop_ops.cpp` | إنشاء الكائنات (`جديد`)، الوصول للحقول، التعامل مع `هذا` |
| `objects_arrays_codegen.h` | `objects_arrays_ops.cpp` | مصفوفات الكائنات وتحويلات النوع لـ `أي` |
| `enum_ops_codegen.h` | `enum_ops.cpp` | التعدادات (tagged unions) مع discriminant byte |
| `functions_codegen.h` | `functions_ops.cpp + functions2_ops.cpp` | تعريف الدوال، الباني/الهادم، lambda — مقسّم لملفين لتقليل وقت التجميع |

## نمط ملف ops

```cpp
#include "builders/oop/X_codegen.h"
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
