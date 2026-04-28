# الفئة: collections/ — المصفوفات والخرائط والنصوص

> **الموقع:** `compiler/include/backend/llvm/builders/collections/`
> **النوع:** ملفات headers فقط (`.h`)
> **الـ ops files المرافقة في:** `compiler/src/backend/llvm/builders/collections/`

## الوصف
الأنواع المركبة: المصفوفات الديناميكية، الخرائط (hash maps)، والنصوص بنوعيها (operators + library).

## الـ Sub-CodeGens في هذه الفئة

| Header | ملفات Ops المرافقة | الدور |
|---|---|---|
| `array_ops_codegen.h` | `array_ops.cpp` | العمليات الأساسية على المصفوفات الديناميكية (vec equiv) |
| `array_builtins_codegen.h` | `array_builtins.cpp` | الدوال الجاهزة على المصفوفات: .اضف(), .رتب(), .خريطة(), .رشح(), .اختزل()... |
| `map_ops_codegen.h` | `map_ops.cpp` | الخرائط (open addressing + linear probing) |
| `string_ops_codegen.h` | `string_ops.cpp` | عمليات النصوص الأساسية (concat, slice, indexing) — operators |
| `strings_codegen.h` | `strings_ops.cpp + string_ops{,2}_ops.cpp` | دوال مكتبة `نصوص` المتقدمة (split, replace, trim, ...) |

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
