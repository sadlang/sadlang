# واجهة SIR الأمامية — `compiler_new/src/frontend/`

> **الدور:** تحويل AST إلى SIR (Sad Intermediate Representation) — التمثيل الوسيط الملكي  
> **الفضاء:** `Sad::Compiler::SIR`  
> **المدخل:** `AST::StmtList`  
> **المخرج:** `SIRModule`  
> **نقطة الدخول:** `SIRBuilder::buildModule()`

---

## نظرة عامة

الواجهة الأمامية للمترجم تحوّل **AST → SIR**. الـ SIR هو تمثيل وسيط ملكي (proprietary IR)  
بين AST المُعقد و LLVM IR المنخفض. يبسّط عمل المترجم بتقليل تعقيد AST إلى **12 تعليمة أساسية**.

```
AST (20+ نوع عقدة)  →  SIR (12 تعليمة)  →  LLVM IR (مئات التعليمات)
         ↑                     ↑                      ↑
     عالي المستوى          وسيط مبسّط           منخفض المستوى
```

---

## جدول الملفات (41 ملف)

### الملفات الأساسية

| الملف | الدور | الدوال الرئيسية |
|-------|------|-----------------|
| `sir_builder_module.cpp` | ⚡ **نقطة الدخول** — بناء الوحدة الكاملة | `buildModule()`, `buildFunction()`, `buildClass()` |
| `sir_builder_helpers.cpp` | دوال مساعدة مشتركة | `makeTemp()`, `emitInstruction()`, `resolveType()` |
| `sir_builder_type_inference.cpp` | استنتاج الأنواع | `inferType()`, `resolveExprType()` |
| `sir_instruction.cpp` | تنفيذ `SIRInstruction` | constructors, toString() |
| `sir_module.cpp` | تنفيذ `SIRModule` | إدارة الدوال والأصناف والكتل |
| `sir_types.cpp` | أنواع SIR | `SIRType`, تحويلات |
| `sir_frontend_optimizer.cpp` | تحسينات أمامية (5 ممرات) | طي ثوابت، إزالة كود ميت |
| `sir_module_test.cpp` | اختبار وحدة SIR | tests |
| `type_info.cpp` | معلومات الأنواع | type metadata |

### بناء التعابير (8 ملفات `sir_builder_expr_*`)

| الملف | الدور |
|-------|------|
| `sir_builder_expressions_dispatch.cpp` | ⚡ موزّع التعابير — يوجّه لكل نوع |
| `sir_builder_expr_collections.cpp` | مصفوفات، خرائط، tuples |
| `sir_builder_expr_comprehensions.cpp` | list/map comprehensions |
| `sir_builder_expr_functional.cpp` | لامدا، closures, pipe |
| `sir_builder_expr_index.cpp` | فهرسة المصفوفات والخرائط |
| `sir_builder_expr_lowlevel.cpp` | تعابير منخفضة المستوى (asm, sizeof) |
| `sir_builder_expr_members.cpp` | وصول الأعضاء (`.حقل`, `.دالة()`) |
| `sir_builder_expr_nullsafety.cpp` | النول الآمن (`?.`, `??`) |

### بناء الجمل (4 ملفات `sir_builder_stmt_*`)

| الملف | الدور |
|-------|------|
| `sir_builder_statements.cpp` | الجمل الأساسية (متغير، إرجاع، إذا، بينما، لكل) |
| `sir_builder_stmt_advanced.cpp` | جمل متقدمة (أجّل، أطلق، اختر) |
| `sir_builder_stmt_exceptions.cpp` | حاول/امسك/أخيراً/ارمي |
| `sir_builder_stmt_generators.cpp` | مولّدات (أنتج) |
| `sir_builder_stmt_types.cpp` | تصريحات الأنواع (بنية، تعداد) |

### بناء OOP والأصناف

| الملف | الدور |
|-------|------|
| `sir_builder_classes.cpp` | ⚠️ أصناف + وراثة + خصائص (~1947 سطر) |
| `sir_builder_calls.cpp` | استدعاء الدوال العادية |
| `sir_builder_calls_objects.cpp` | ⚠️ استدعاء دوال الكائنات (~2115 سطر) |

### بناء التحكم

| الملف | الدور |
|-------|------|
| `sir_builder_control_flow.cpp` | إذا/وإلا، بينما، لكل — التحكم الأساسي |
| `sir_builder_control_branch.cpp` | تفريعات معقدة (أنماط، عقود) |
| `sir_builder_control_match.cpp` | طابق/عندما — مطابقة الأنماط |

### ميزات خاصة

| الملف | الدور |
|-------|------|
| `sir_builder_operators.cpp` | العوامل (حسابية، مقارنة، منطقية) |
| `sir_builder_builtins_core.cpp` | دوال مدمجة (اطبع، طول، نوع) |
| `sir_builder_builtins_async.cpp` | تزامن (قناة، أطلق) |
| `sir_builder_builtins_ffi.cpp` | FFI bindings |
| `sir_builder_builtins_security.cpp` | أمان (تشفير) |
| `sir_builder_builtins_system.cpp` | نظام (ملفات، بيئة) |
| `sir_builder_builtins_embedded.cpp` | مدمج (IoT) |
| `sir_builder_builtins_ui.cpp` | واجهات المستخدم |
| `sir_builder_builtins_os_core.cpp` | أنوية OS أساسية |
| `sir_builder_builtins_os_hardware.cpp` | عتاد OS |
| `sir_builder_builtins_os_system.cpp` | نظام OS |
| `sir_builder_builtins_uefi.cpp` | UEFI boot |
| `sir_builder_templates.cpp` | القوالب (generics) |
| `generics_codegen.cpp` | توليد كود القوالب |

---

## تعليمات SIR الأساسية (12 تعليمة)

مُعرَّفة في `compiler_new/src/sir/sir_opcodes.h`:

| التعليمة | الوصف |
|----------|------|
| `ALLOC` | تخصيص متغير |
| `STORE` | تخزين قيمة |
| `LOAD` | تحميل قيمة |
| `CALL` | استدعاء دالة |
| `RETURN` | إرجاع |
| `BRANCH` | تفريع شرطي |
| `JUMP` | قفز غير شرطي |
| `BINOP` | عملية ثنائية |
| `UNOP` | عملية أحادية |
| `PHI` | نقطة دمج (SSA) |
| `CAST` | تحويل نوع |
| `BUILTIN_PRINT` | طباعة مدمجة |

---

## مسار التنفيذ

```
SIRBuilder::buildModule(program)
    │
    ├── لكل statement:
    │   ├── FuncDeclStmt  → buildFunction()
    │   │   ├── buildStatements(body)
    │   │   │   ├── VarDeclStmt → ALLOC + STORE
    │   │   │   ├── IfStmt → BRANCH + JUMP + BasicBlocks
    │   │   │   ├── WhileStmt → BRANCH + JUMP + loop blocks
    │   │   │   └── ReturnStmt → RETURN
    │   │   └── buildExpressions(...)
    │   │       ├── BinaryExpr → BINOP
    │   │       ├── CallExpr → CALL
    │   │       └── LiteralExpr → STORE constant
    │   │
    │   ├── ClassDeclStmt → إنشاء بنية SIR للصنف
    │   │   ├── حقول → StructType
    │   │   ├── دوال → functions
    │   │   └── باني → constructor function
    │   │
    │   └── ImportStmt → معالجة الاستيراد
    │
    └── SIRModule (النتيجة)
```

---

## العلاقات

- **🔗 يستهلك:** `AST::StmtList` (من المحلل النحوي)
- **🔗 ينتج:** `SIRModule` → يُستهلك من `SIROptimizer` ثم `LLVMCodeGen`
- **🔗 يعتمد على:** `sir_opcodes.h`, `sir_types.h`

---

## ⚠️ ملاحظات مهمة

1. **`sir_builder_calls_objects.cpp` هو أكبر ملف** (~2115 سطر)
2. **SIR يدعم SSA** (Static Single Assignment) — كل متغير يُعيَّن مرة واحدة
3. **`BasicBlock`** هي وحدة التنفيذ — تعليمات متتالية تنتهي بـ `BRANCH` أو `JUMP` أو `RETURN`
4. **`PHI`** ضرورية عند دمج تفريعات (مثل بعد `إذا`/`وإلا`)
