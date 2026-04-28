# الفئة: core/ — الأساسيات

> **الموقع:** `compiler/include/backend/llvm/builders/core/`
> **النوع:** ملفات headers فقط (`.h`)
> **الـ ops files المرافقة في:** `compiler/src/backend/llvm/builders/core/`

## الوصف
الـ sub-codegens الأساسية التي يعتمد عليها الباقي: dispatcher، إخراج الملفات، تحويل الأنواع، ودعم freestanding/kernel.

## الـ Sub-CodeGens في هذه الفئة

| Header | ملفات Ops المرافقة | الدور |
|---|---|---|
| `instr_core_codegen.h` | `instr_core_ops.cpp` | dispatcher مركزي يوزّع تعليمات SIR إلى الـ sub-codegen المناسب |
| `output_codegen.h` | `output_ops.cpp` | كتابة LLVM IR إلى ملفات .ll و .o ومعالجة linker output |
| `types_codegen.h` | `types_ops.cpp` | تحويل أنواع SIR إلى LLVM (17 method): convertType, getInt*Type, getConstant* |
| `freestanding_codegen.h` | `freestanding_{conv,io,sprintf}_ops.cpp + freestanding_ops.cpp` | دعم وضع freestanding/kernel بدون C runtime — لـ Ufuq OS وUEFI |

## نمط الاستخدام

كل sub-codegen يتبع النمط القياسي:

```cpp
namespace Sad::LLVM {
class LLVMCodeGen;  // forward decl

class XCodeGen {
public:
    explicit XCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    // ... methods ...
private:
    LLVMCodeGen &cg_;
};
}
```

## مراجع
- [الوثيقة المعمارية الكاملة](../../../../../../docs/compiler_architecture_phase10.md)
- [مخططات Mermaid](../../../../../../docs/compiler_architecture_diagrams.md)
