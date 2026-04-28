# الفئة: directives/ — التوجيهات والتزامن

> **الموقع:** `compiler/include/backend/llvm/builders/directives/`
> **النوع:** ملفات headers فقط (`.h`)
> **الـ ops files المرافقة في:** `compiler/src/backend/llvm/builders/directives/`

## الوصف
توجيهات @، SIMD، goroutines/channels، و coroutines (async/await).

## الـ Sub-CodeGens في هذه الفئة

| Header | ملفات Ops المرافقة | الدور |
|---|---|---|
| `directives_codegen.h` | `directives_ops.cpp` | توجيهات @: @غير_آمن, @وقت_الترجمة, @متطاير, @حجم |
| `simd_codegen.h` | `simd_ops.cpp` | تعليمات SIMD (Phase 3) — vector types و operations |
| `concurrency_codegen.h` | `concurrency_ops.cpp + channels_ops.cpp` | goroutines, channels (SadChannel), mutex, futures, WaitGroup |
| `coroutines_codegen.h` | `coroutines_ops.cpp` | async/await باستخدام LLVM coroutine intrinsics (@llvm.coro.*) |

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
