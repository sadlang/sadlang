# الفئة: arithmetic/ — الحساب وتحكم التدفق

> **الموقع:** `compiler/include/backend/llvm/builders/arithmetic/`
> **النوع:** ملفات headers فقط (`.h`)
> **الـ ops files المرافقة في:** `compiler/src/backend/llvm/builders/arithmetic/`

## الوصف
كل العمليات الحسابية، المقارنات، التحكم في التدفق (if/while/for/match)، والاستثناءات.

## الـ Sub-CodeGens في هذه الفئة

| Header | ملفات Ops المرافقة | الدور |
|---|---|---|
| `arithmetic_codegen.h` | `arith_{main,cmp,extras,resolve,type_conv}.cpp` | العمليات الحسابية (+, -, *, /, %, **) ومقارنات (==, !=, <, >, ...) — مقسّم لـ 5 ملفات لتقليل وقت التجميع |
| `controlflow_codegen.h` | `cf_{branch_call,return_switch}.cpp` | if/else, while, for, match/when, switch, return — يحفظ ترتيب الإدراج للكتل (BasicBlock ordering) |
| `exception_codegen.h` | `exception_ops.cpp` | try/catch/throw/finally باستخدام Itanium C++ ABI |

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
