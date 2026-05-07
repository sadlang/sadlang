# طبقات المُحسِّن في مترجم لغة ص

> **الغرض:** توثيق صارم لطبقات تحسين الكود الثلاث في مترجم `sadc`، ولماذا توجد طبقة canonical واحدة فقط، ولماذا تم حذف التنفيذات المكررة.
>
> **القاعدة الذهبية:** أي pass جديد يجب أن يُكتب في **طبقة SIR** (`compiler/src/sir_optimizer/`). لا تُنشئ تنفيذات بديلة على مستوى AST أو في مجلدات أخرى.

---

## 1. خريطة الطبقات (Top-Level View)

```
┌─────────────────────────────────────────────────────────────────────┐
│                    مصدر .ص  (Sad Source File)                       │
└─────────────────────────────────────────────────────────────────────┘
                                  │
                                  ▼
┌─────────────────────────────────────────────────────────────────────┐
│  طبقة 1: Lexer  (shared/lexer)                                      │
│  ───────────────────────────────────────                            │
│  لا تحسينات هنا.                                                    │
└─────────────────────────────────────────────────────────────────────┘
                                  │  Tokens
                                  ▼
┌─────────────────────────────────────────────────────────────────────┐
│  طبقة 2: Parser → AST  (shared/parser, shared/ast)                  │
│  ───────────────────────────────────────                            │
│  ❌ لا تحسينات هنا (سياسة معمارية).                                 │
│  السبب: AST قريب جداً من النص؛ التحسين عليه يُكرر منطقاً سيُعاد     │
│  تنفيذه على SIR. كل pass على AST = ازدواج عمل.                      │
└─────────────────────────────────────────────────────────────────────┘
                                  │  AST
                                  ▼
┌─────────────────────────────────────────────────────────────────────┐
│  طبقة 3: SIR Builder  (compiler/src/sir)                            │
│  ───────────────────────────────────────                            │
│  AST → SIRModule (Sad Intermediate Representation).                 │
│  لا تحسينات هنا — مجرد ترجمة 1:1.                                   │
└─────────────────────────────────────────────────────────────────────┘
                                  │  SIRModule
                                  ▼
┌─────────────────────────────────────────────────────────────────────┐
│  ⭐ طبقة 4: SIR Optimizer  ← canonical layer                        │
│  ──────────────────────────────────────────                         │
│  المكان: compiler/src/sir_optimizer/  +  compiler/include/sir_optimizer/          │
│  المكتبة: sad_optimizer                                             │
│  نقطة الدخول: Sad::Compiler::Optimizer::createOptimizer()           │
│  الاستدعاء من: tools/compiler/compiler_driver_backend.cpp:134       │
│                                                                     │
│  ⬅️ كل التحسينات المستقلة عن LLVM تحدث هنا.                         │
│  ⬅️ هنا فقط تُضاف passes جديدة.                                     │
└─────────────────────────────────────────────────────────────────────┘
                                  │  SIRModule (محسَّن)
                                  ▼
┌─────────────────────────────────────────────────────────────────────┐
│  طبقة 5: LLVM CodeGen  (compiler/src/llvm_codegen)                  │
│  ───────────────────────────────────────                            │
│  SIRModule → llvm::Module                                           │
└─────────────────────────────────────────────────────────────────────┘
                                  │  llvm::Module
                                  ▼
┌─────────────────────────────────────────────────────────────────────┐
│  طبقة 6: LLVM Pass Pipeline  ← قابلة للتمديد بـ arabic_passes      │
│  ───────────────────────────────────────                            │
│  llvm::PassBuilder قياسي + (اختيارياً) SadArabicPasses              │
│    - ArabicStringPoolingPass                                        │
│    - TashkeelOptimizationPass                                       │
│    - BidiAnalysisPass                                               │
└─────────────────────────────────────────────────────────────────────┘
                                  │
                                  ▼
                         ملف تنفيذي أصلي (.exe)
```

---

## 2. تشريح طبقة SIR Optimizer (Canonical)

```
                  Sad::Compiler::Optimizer::createOptimizer()
                                  │
                                  ▼
                        ┌─────────────────────┐
                        │   PassManager       │  ← compiler/src/sir_optimizer/pass_manager.cpp
                        └─────────────────────┘
                                  │
                  ┌───────────────┼───────────────────┐
                  ▼               ▼                   ▼
            initializePasses()  runOnModule()   ترتيب التنفيذ
                  │
   ┌──────────────┴──────────────────────────────────┐
   │                  Passes المُسجَّلة                 │
   ├─────────────────────────────────────────────────┤
   │ 1. ConstantFoldingPass         ← طي الثوابت     │
   │ 2. DeadCodeEliminationPass     ← حذف الميت      │
   │ 3. CSEPass                     ← حذف التكرار    │
   │ 4. CopyPropagationPass         ← نشر النسخ      │
   │ 5. RegisterCoalescingPass      ← دمج السجلات    │
   │ 6. LICMPass                    ← رفع الثوابت    │
   │ 7. StrengthReductionPass       ← خفض القوة      │
   │ 8. PeepholePass                ← تحسينات نظرة   │
   │ 9. BranchFoldingPass           ← طي الفروع      │
   │ 10. InliningPass               ← استدخال الدوال │
   │ 11. TailCallPass               ← استدعاء ذيلي   │
   │ 12. SROAPass                   ← تفكيك الكائنات │
   └─────────────────────────────────────────────────┘
                                  │
                                  ▼
                   كل pass يرث من OptimizationPass
                  (compiler/src/sir_optimizer/pass.cpp)
```

### ملف بملف:

| Pass | ملف التنفيذ | header |
|------|-------------|--------|
| `Optimizer` (orchestrator) | `compiler/src/sir_optimizer/optimizer.cpp` | `compiler/include/sir_optimizer/optimizer.h` |
| `OptimizationPass` (base) | `compiler/src/sir_optimizer/pass.cpp` | `compiler/include/sir_optimizer/pass.h` |
| `PassManager` | `compiler/src/sir_optimizer/pass_manager.cpp` | `compiler/include/sir_optimizer/pass_manager.h` |
| `ConstantFoldingPass` | `compiler/src/sir_optimizer/constant_folding_pass.cpp` | `compiler/include/sir_optimizer/constant_folding_pass.h` |
| `DeadCodeEliminationPass` | `compiler/src/sir_optimizer/dead_code_elimination_pass.cpp` | `compiler/include/sir_optimizer/dead_code_elimination_pass.h` |
| `CSEPass` | `compiler/src/sir_optimizer/cse_pass.cpp` | `compiler/include/sir_optimizer/cse_pass.h` |
| `CopyPropagationPass` | `compiler/src/sir_optimizer/copy_propagation_pass.cpp` | `compiler/include/sir_optimizer/copy_propagation_pass.h` |
| `RegisterCoalescingPass` | `compiler/src/sir_optimizer/register_coalescing_pass.cpp` | `compiler/include/sir_optimizer/register_coalescing_pass.h` |
| `LICMPass` | `compiler/src/sir_optimizer/licm_pass.cpp` | `compiler/include/sir_optimizer/licm_pass.h` |
| `StrengthReductionPass` | `compiler/src/sir_optimizer/strength_reduction_pass.cpp` | `compiler/include/sir_optimizer/strength_reduction_pass.h` |
| `PeepholePass` | `compiler/src/sir_optimizer/peephole_pass.cpp` | (داخلي) |
| `BranchFoldingPass` | `compiler/src/sir_optimizer/branch_folding_pass.cpp` | (داخلي) |
| `InliningPass` | `compiler/src/sir_optimizer/inlining_pass.cpp` | (داخلي) |
| `TailCallPass` | `compiler/src/sir_optimizer/tail_call_pass.cpp` | (داخلي) |
| `SROAPass` | `compiler/src/sir_optimizer/sroa_pass.cpp` | (داخلي) |

### حالات خاصة في `middle/`:

- **`async_transform.cpp`** — مُجمَّع في `sad_optimizer` لكن **غير مُسجَّل** في `Optimizer::initializePasses()`. يستخدمه فقط اختبار وحدي. مرشح للتسجيل لاحقاً عند اكتمال نظام `async/await` في المترجم.
- **`class_desugar.cpp`** — ❌ **محذوف** (لم يكن له header، ولا مستهلكين). كان كوداً ميتاً منذ البداية.

---

## 3. طبقة LLVM Passes الاختيارية (`arabic_passes`)

```
                  llvm::PassBuilder PB;
                          │
                          ▼
              SadArabicPasses::registerPasses(PB)
                          │
        ┌─────────────────┼──────────────────┐
        ▼                 ▼                  ▼
  ArabicStringPool   TashkeelOpt       BidiAnalysis
   ─────────────    ─────────────     ─────────────
   تجميع السلاسل   إزالة التشكيل      تحليل الاتجاه
   العربية المكررة  من السلاسل لتقليل   ثنائي الاتجاه
   في pool واحد     الحجم              للتعامل مع
                                       النصوص العربية
```

**المكان:** [compiler/src/optimizer/arabic_passes.cpp](../src/optimizer/arabic_passes.cpp)
**المستوى:** يعمل على `llvm::Module` مباشرة بعد `LLVMCodeGen`، وليس على `SIRModule`.
**الحالة:** الكود مكتمل ومُختبر داخلياً، لكن `SadArabicPasses::registerPasses(PB)` **غير مُستدعى** من `llvm_codegen` حالياً. مرشح للإحياء.

> **استثناء معماري:** هذا الملف يبقى في مجلد `optimizer/` رغم حذف باقي محتواه، لأنه LLVM-level وليس SIR-level. لو أُريد إخفاؤه، الأنسب نقله إلى `compiler/src/llvm_codegen/arabic_passes.cpp` لاحقاً.

---

## 4. الكود المحذوف ولماذا

### ❌ التنفيذ #2 (محذوف): `compiler/src/optimizer/optimizer_o2.h`

```
            optimizer_o2.h  ←── (حذف)
                  │
                  ▼  كان يحوي:
       ┌──────────────────────────────────┐
       │ SIRFunction, SIRBasicBlock,      │  ← تعريفات بديلة لأنواع
       │ SIRInstruction, SIRModule        │    موجودة فعلاً
       │ ConstantFoldingPass,              │  ← أسماء مكررة لكن
       │ DeadCodeEliminationPass, ...     │    declarations فارغة
       └──────────────────────────────────┘
                  │
                  ▼
           0 مستهلك خارجي
           ⇒ كود ميت بالكامل
```

**سبب الحذف:**
- header-only بدون `.cpp` مرافق ⇒ لا منطق فعلي.
- 0 ملف `#include` لهذا الـ header.
- التعريفات `SIR*` تتعارض مع تعريفات SIR الحقيقية.

### ❌ التنفيذ #3 (محذوف): `compiler/include/optimizer/optimization_pass.h` + `advanced_optimizer.h`

```
                AdvancedOptimizer (AST-level)
                         │
                         ▼
        ┌────────────────────────────────────┐
        │ يعمل على AST nodes بدلاً من SIR    │
        │ ──────────────────────────────────  │
        │ - ConstantFoldingPass (AST)        │ ← اسم مكرر، منطق مختلف
        │ - DeadCodeEliminationPass (AST)    │   لا يمكن دمجه مع SIR pass
        │ - ExpressionSimplificationPass     │ ← فريد لكن AST-level
        │ - LoopOptimizationPass             │ ← فريد لكن AST-level
        │ - FunctionInliningPass             │ ← اسم مكرر
        │ - RedundantAssignmentElimination   │ ← فريد لكن AST-level
        └────────────────────────────────────┘
                         │
                         ▼
           يُجمَّع لكن لا يُستدعى من sadc
           يستخدمه فقط tests/optimizer/
           (ليس فيها CMakeLists.txt → غير مُجمَّعة)
```

**سبب الحذف:**
- يعمل على AST بينما المسار الحي SIR-only.
- "الأصناف الفريدة" بدون قيمة لأن دمجها يتطلب إعادة كتابتها كـ SIR passes.
- المستهلك الوحيد (`tests/optimizer/`) لم يُجمَّع منذ كتابته.

### ❌ ملفات يتيمة محذوفة

| ملف | السبب |
|---|---|
| `compiler/src/optimizer/optimization_pass_constant_folding.cpp` | غير مدرج في أي CMakeLists.txt |
| `compiler/src/optimizer/optimization_pass_expr_loop.cpp` | غير مدرج في أي CMakeLists.txt |
| `compiler/src/optimizer/optimization_pass_inline_assign.cpp` | غير مدرج في أي CMakeLists.txt |
| `compiler/src/optimizer/optimization_pass_loop.cpp` | غير مدرج في أي CMakeLists.txt |
| `compiler/src/sir_optimizer/class_desugar.cpp` | لا header، 0 مستهلك |
| `tests/optimizer/*.cpp` (8 ملفات) | لا CMakeLists.txt → لم تُجمَّع أبداً |

### ❌ مكتبة محذوفة من CMake

- `add_library(sad_advanced_optimizer ...)` ⇒ أُزيلت بالكامل من [compiler/CMakeLists.txt](../CMakeLists.txt).

---

## 5. مقارنة قبل/بعد التوحيد

```
┌─────────────────────────────────────────────────────────────┐
│                          قبل                                │
├─────────────────────────────────────────────────────────────┤
│ 3 تنفيذات لـ ConstantFoldingPass:                           │
│   - middle/constant_folding_pass.cpp     (SIR, حي)          │
│   - optimizer/optimizer_o2.h             (مكرر فارغ)        │
│   - include/optimizer/optimization_pass.h (AST، ميت)        │
│                                                             │
│ 2 مكتبات: sad_optimizer  +  sad_advanced_optimizer          │
│ 9 ملفات .cpp ميتة + 3 .h ميتة + 8 اختبارات يتيمة            │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼  توحيد
┌─────────────────────────────────────────────────────────────┐
│                          بعد                                │
├─────────────────────────────────────────────────────────────┤
│ 1 تنفيذ لكل pass — في compiler/src/sir_optimizer/                  │
│ 1 مكتبة: sad_optimizer                                      │
│ 0 ملفات ميتة                                                │
│ arabic_passes معزول كطبقة LLVM-level مستقلة                 │
└─────────────────────────────────────────────────────────────┘
```

---

## 6. سياسات صارمة للمستقبل

> **هذه السياسات تمنع تكرار المشكلة.**

1. **CW-19 (DRY):** أي pass جديد يُكتب **مرة واحدة** في `compiler/src/sir_optimizer/`.
2. **CW-02 (Layering):** يُمنع إضافة passes في `parser/` أو `ast/` أو `sir/` — هذه طبقات بناء وليست تحسين.
3. **AST passes ممنوعة:** لو احتجت تحسيناً، نفّذه على SIR. AST قريب من النص ولا يستحق الاستثمار التحسيني.
4. **مجلد `compiler/src/optimizer/` محجوز للـ LLVM-level فقط:** أي passes تعمل على `llvm::Module` بعد codegen.
5. **`compiler/include/optimizer/` محذوف بالكامل:** لا تعيد إنشاءه. لو احتجت header لـ `arabic_passes`، ضعه بجوار الـ `.cpp`.
6. **اختبارات الـ passes** تُكتب في `tests/comprehensive/` أو `tests/unit/` مع CMakeLists.txt صريح — لا تترك ملفات اختبار يتيمة.
7. **`async_transform.cpp` و`arabic_passes.cpp`:** مرشحان للإحياء. أي تعديل عليهما يجب أن يكون مصحوباً بتسجيل في الـ pipeline (وإلا يُحذفان).

---

## 7. مراجع داخلية

- خريطة المشروع الكاملة: [/memories/repo/project_full_map.md](/memories/repo/project_full_map.md)
- تدقيق الكود الميت في middle: [/memories/repo/middle_dead_code_audit.md](/memories/repo/middle_dead_code_audit.md)
- CMakeLists المترجم: [compiler/CMakeLists.txt](../CMakeLists.txt)
- نقطة دخول المُحسِّن من sadc: [tools/compiler/compiler_driver_backend.cpp](../../tools/compiler/compiler_driver_backend.cpp)
