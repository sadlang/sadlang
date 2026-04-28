# الفئة: collections/ — المصفوفات والخرائط والنصوص (تنفيذ)

> **الموقع:** `compiler/src/backend/llvm/builders/collections/`
> **النوع:** ملفات تنفيذ (`.cpp`)
> **الـ headers المقابلة في:** `compiler/include/backend/llvm/builders/collections/`

## الوصف
الأنواع المركبة: المصفوفات الديناميكية، الخرائط (hash maps)، والنصوص بنوعيها (operators + library).

## ملفات التنفيذ في هذه الفئة

| Sub-CodeGen | ملفات Ops | الدور |
|---|---|---|
| `array_ops_codegen.h` | `array_ops.cpp` | العمليات الأساسية على المصفوفات الديناميكية (vec equiv) |
| `array_builtins_codegen.h` | `array_builtins.cpp` | الدوال الجاهزة على المصفوفات: .اضف(), .رتب(), .خريطة(), .رشح(), .اختزل()... |
| `map_ops_codegen.h` | `map_ops.cpp` | الخرائط (open addressing + linear probing) |
| `string_ops_codegen.h` | `string_ops.cpp` | عمليات النصوص الأساسية (concat, slice, indexing) — operators |
| `strings_codegen.h` | `strings_ops.cpp + string_ops{,2}_ops.cpp` | دوال مكتبة `نصوص` المتقدمة (split, replace, trim, ...) |

## نمط ملف ops

```cpp
#include "builders/collections/X_codegen.h"
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
