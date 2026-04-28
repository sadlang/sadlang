# الفئة: memory/ — الذاكرة والإغلاقات

> **الموقع:** `compiler/include/backend/llvm/builders/memory/`
> **النوع:** ملفات headers فقط (`.h`)
> **الـ ops files المرافقة في:** `compiler/src/backend/llvm/builders/memory/`

## الوصف
إدارة ذاكرة المكدس (alloca/load/store)، الإغلاقات، والبنى المجمعة.

## الـ Sub-CodeGens في هذه الفئة

| Header | ملفات Ops المرافقة | الدور |
|---|---|---|
| `memory_codegen.h` | `mem_{alloca,load,store}.cpp` | إدارة ذاكرة المكدس + ownership tracking (Phase 6 SIR opcodes) |
| `closure_codegen.h` | `closure_ops.cpp` | بناء الإغلاقات (closures) ودعم capture-by-value و capture-by-reference |
| `aggregate_ops_codegen.h` | `agg_ops.cpp` | البنى المجمعة (struct literals, array literals) عبر insertvalue/extractvalue |

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
