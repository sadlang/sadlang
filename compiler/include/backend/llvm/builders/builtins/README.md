# الفئة: builtins/ — الدوال المضمنة

> **الموقع:** `compiler/include/backend/llvm/builders/builtins/`
> **النوع:** ملفات headers فقط (`.h`)
> **الـ ops files المرافقة في:** `compiler/src/backend/llvm/builders/builtins/`

## الوصف
الدوال الجاهزة في اللغة (مع/بدون استيراد): الإخراج/الإدخال، الرياضيات، الشبكة، الأمان.

## الـ Sub-CodeGens في هذه الفئة

| Header | ملفات Ops المرافقة | الدور |
|---|---|---|
| `builtin_funcs_codegen.h` | `builtin_funcs_ops.cpp + builtins_math_async_ops.cpp` | الدوال المضمنة بدون استيراد: اطبع, طول, نوع, رقم(), نص(), منطقي(), ... |
| `io_builtins_codegen.h` | `io_builtins_ops.cpp` | اقرأ, اطبع_سطر, file I/O أساسي |
| `math_builtins_codegen.h` | `math_builtins.cpp` | sin, cos, log, exp, pow, sqrt + ثوابت π و e |
| `network_builtins_codegen.h` | `builtins_network_ops.cpp` | TCP/UDP, HTTP بدائي — يربط مكتبة sad_network |
| `security_builtins_codegen.h` | `security_builtins_ops.cpp` | التشفير: SHA-256, AES-CTR |

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
