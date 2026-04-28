# الفئة: oop/ — البرمجة الكائنية

> **الموقع:** `compiler/include/backend/llvm/builders/oop/`
> **النوع:** ملفات headers فقط (`.h`)
> **الـ ops files المرافقة في:** `compiler/src/backend/llvm/builders/oop/`

## الوصف
كل ما يتعلق بالأصناف: vtables، إنشاء الكائنات، الحقول، الباني/الهادم، الدوال، والتعدادات.

## الـ Sub-CodeGens في هذه الفئة

| Header | ملفات Ops المرافقة | الدور |
|---|---|---|
| `classes_vtables_codegen.h` | `classes_vtables_ops.cpp` | بناء جداول الدوال الافتراضية (vtables) و dynamic dispatch |
| `oop_ops_codegen.h` | `oop_ops.cpp` | إنشاء الكائنات (`جديد`)، الوصول للحقول، التعامل مع `هذا` |
| `objects_arrays_codegen.h` | `objects_arrays_ops.cpp` | مصفوفات الكائنات وتحويلات النوع لـ `أي` |
| `enum_ops_codegen.h` | `enum_ops.cpp` | التعدادات (tagged unions) مع discriminant byte |
| `functions_codegen.h` | `functions_ops.cpp + functions2_ops.cpp` | تعريف الدوال، الباني/الهادم، lambda — مقسّم لملفين لتقليل وقت التجميع |

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
