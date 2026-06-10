# معمارية لغة ص — خط الأنابيب الكامل والطبقات

> اقرأ هذا الملف قبل أي تعديل بنيوي. الهدف: تعرف **أين** يقع تغييرك و**على ماذا** يؤثر.

## 1. خط الأنابيب (Data Flow)

```
مصدر .ص
   │
   ▼
LexerCore (shared/lexer)              → سلسلة Tokens
   │
   ▼
ParserCore (shared/parser)           → AST (shared/ast)
   │
   ├──────────────────────────────┬───────────────────────────────┐
   ▼                              ▼                               
InterpreterCore (interpreter/)   VM (vm/)        SIRBuilder (compiler/src/frontend)
  تنفيذ شجري فوري                بايت كود          → SIR (~90 opcode، منها عمليات الملكية)
                                                       │
                                                       ▼
                                                  SIROptimizer (compiler/src/sir_optimizer)
                                                       │
                                                       ▼
                                                  LLVMCodeGen (compiler/src/backend)
                                                       │
                                                       ▼
                                                  ملف تنفيذي أصلي (عبر LLVM 18)
```

## 2. الطبقات والاعتماد (CW-02)

كل طبقة تعتمد **فقط** على ما تحتها. يُمنع الاعتماد العكسي أو القفز:

```
Lexer  →  Parser  →  AST  →  SIR  →  LLVM
```

- خطأ في الرموز → أصلِح في `shared/lexer/`
- خطأ نحوي → أصلِح في `shared/parser/`
- خطأ في بنية الشجرة → أصلِح في `shared/ast/`
- خطأ ترتيب حقول/ملكية → أصلِح في SIR builder (`compiler/src/frontend/`)
- خطأ تحويل أنواع في الإخراج → أصلِح في codegen (`compiler/src/backend/`)

## 3. المكوّنات الرئيسية

| المكوّن | المجلد | نقطة الدخول / ملف مفتاح |
|--------|--------|------------------------|
| المعجمي | `shared/lexer/` | `include/token.h`, `src/lexer_keywords.cpp`, `generated/keywords_generated.*` |
| النحوي | `shared/parser/` | `include/parser_core.h` + `src/{core,declarations,statements,specs,ui}/` |
| AST | `shared/ast/` | `include/` — كل عقدة ترث `ASTNode` |
| نظام الأنواع | `shared/types/` | `include/value.h` — `Value` = `std::variant` على `ValueType` |
| نظام الأخطاء | `shared/errors/` | `include/error_codes.h`, `include/error_catalog.h` |
| سجل الدوال المُولَّد | `shared/builtins/generated/` | `builtin_registry_generated.h` (مُولَّد من YAML) |
| المفسر | `interpreter/` | `include/core/interpreter_core.h` |
| تنفيذ الدوال المضمنة | `interpreter/src/builtins/` | `builtin_registry.cpp` + `builtin_*.cpp` |
| الزوّار (Visitors) | `interpreter/include/visitors/`, `interpreter/src/visitors/` | `expression_evaluator.h` (تقييم التعابير) + `statement_executor.h` (تنفيذ الجمل) |
| المترجم frontend | `compiler/src/frontend/` | `SIRBuilder` |
| SIR types | `compiler/include/frontend/sir_types.h` | `enum class SIROpcode` (~90 opcode، منها تعليمات الملكية والتعداد الجبري) |
| المترجم backend | `compiler/src/backend/llvm/` | `LLVMCodeGen` + `builders/builtins/` (codegen الدوال المضمنة) |
| الـ VM | `vm/` | بايت كود مربوط بالمفسر |
| وقت التشغيل | `runtime/` | ABI/FFI مستقل + ربط VM |

### تنظيم الزوّار في المفسر (مرجع سريع)

التقييم والتنفيذ مقسّمان إلى ملفات صغيرة حسب المسؤولية (SRP — CW-01):

- **تقييم التعابير** `expression_evaluator_*.cpp`: `_core`, `_binary_ops`, `_binary_logic`,
  `_calls*` (الاستدعاء/الإرسال/الماكرو/دوال المستخدم), `_members*` (الوصول/الإسناد),
  `_oop*` (الكائنات/الإنشاء/طرق المصفوفات/النصوص/الخرائط/التزامن), `_overloads`, `_ui`.
- **تنفيذ الجمل** `statement_executor_*.cpp`: `_control` (+`_exceptions`), `_functions`
  (+`_templates`), `_modules` (الاستيراد), `_oop`.

> عند تعديل سلوك تعبير/جملة، حدّد الملف الأدق بالاسم — لا تضع المنطق في الملف الخطأ (CW-03).

## 4. القاعدة الذهبية: مدفوع بالبيانات

بيانات اللغة **ليست** مكتوبة يدوياً في C++. هي في `language-truth/*.yaml` وتُولَّد:

```
language-truth/keywords.yaml  → gen_keywords.py        → shared/lexer/generated/keywords_generated.{h,cpp}
language-truth/builtins/*.yaml → gen_builtins_registry.py → shared/builtins/generated/builtin_registry_generated.h
language-truth/errors/*.yaml  → gen_error_messages.py   → (كتالوج الأخطاء)
```

التوليد يحدث **وقت البناء** عبر `cmake/codegen.cmake` (custom commands تعتمد على ملفات YAML).
تفاصيل كاملة في [./docs-yaml-system.md](./docs-yaml-system.md).

## 5. نوع القيم الموحّد `Value`

- `shared/types/include/value.h` — يحمل `ValueType` داخلي + القيمة
- `OBJECT` يُدار بـ GC (`ObjectInstance*`)، يُمرَّر بالمرجع تلقائياً (`Pin` للتثبيت كجذر)
- تجنّب تعارض ماكرو `VOID` مع Windows (الملف يلغي تعريفه قبل `ValueType::VOID`)
- `DataType` enum **لا يحتوي** `ANY` — استخدم `DT::OBJECT` بدلاً منه
- نظام الأنواع الثابت `SadTypeKind` (للمترجم/الدلالي) **مختلف** عن `ValueType` (لوقت التشغيل)

> تفصيل كامل لـ `Value`/`SadTypeKind`/SIR opcodes في [./types-and-sir.md](./types-and-sir.md).

## 6. أمان الخيوط (مهم عند العمل على المفسر)

كل goroutine يعمل بـ `StatementExecutor` مستقل (مع `ScopeManager`, `VariableManager`,
`OwnershipManager` خاصة). يُشارك `FunctionManager` فقط (للقراءة). المتغيرات تُلتقط كـ
snapshot عبر `captureVisibleVariables()`. القنوات آمنة عبر mutex داخلي في `SadChannel`.

## 7. أين أبدأ حسب نوع المهمة

| المهمة | ابدأ من |
|--------|---------|
| كلمة مفتاحية جديدة | `language-truth/keywords.yaml` → توليد → parser → AST → visitors |
| دالة مضمنة جديدة | `language-truth/builtins/*.yaml` → توليد → `interpreter/src/builtins/` → codegen |
| رمز خطأ جديد | `error_codes.h` + `language-truth/errors/<cat>.yaml` (المصدر الموحَّد، مربوط CMake) → توليد — راجع [./error-system.md](./error-system.md) |
| توجيه `@` جديد | `language-truth/directives.yaml` → parser → AST → visitors |
| تحسين أداء المترجم | `compiler/src/sir_optimizer/` (بعد قياس — CW-30) |
| ميزة لغوية كاملة | راجع [./adding-systems.md](./adding-systems.md) |
