# الفئة: platform/ — المنصة و FFI و UI (تنفيذ)

> **الموقع:** `compiler/src/backend/llvm/builders/platform/`
> **النوع:** ملفات تنفيذ (`.cpp`)
> **الـ headers المقابلة في:** `compiler/include/backend/llvm/builders/platform/`

## الوصف
كل ما يعتمد على المنصة: low-level ops، UEFI/kernel، C ABI، SDL2/OpenGL UI، وتحويلات المؤشرات.

## ملفات التنفيذ في هذه الفئة

| Sub-CodeGen | ملفات Ops | الدور |
|---|---|---|
| `instr_platform_codegen.h` | `instr_platform_ops.cpp` | تعليمات خاصة بالمنصة (platform-specific intrinsics) |
| `instr_lowlevel_codegen.h` | `instr_lowlevel_ops.cpp` | تعليمات منخفضة المستوى (asm inline, registers) |
| `lowlevel_codegen.h` | `lowlevel_ops.cpp + lowlevel_uefi_ops.cpp` | UEFI/kernel boot, MSR, CPU features — لـ Ufuq OS (header 183 سطر) |
| `ui_codegen.h` | `ui_ops.cpp` | SDL2 + OpenGL bindings للـ widgets — مرتبط بـ sad_ui و graphics/ |
| `file_casts_codegen.h` | `file_casts_ops.cpp` | ptrtoint / inttoptr / bitcast الآمنة |
| `hardware_ffi_codegen.h` | `hardware_ffi_ops.cpp + hardware_ffi_ops2.cpp` | استدعاءات C ABI، تمرير structs بقيمة عبر System V AMD64 ABI |
| `ffi_remain_codegen.h` | `ffi_remain_ops.cpp` | حالات FFI خاصة لم تدخل في hardware_ffi |

## نمط ملف ops

```cpp
#include "builders/platform/X_codegen.h"
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
