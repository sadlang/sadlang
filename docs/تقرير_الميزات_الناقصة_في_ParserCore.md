# تقرير فحص شامل: الميزات الموجودة في الكود وغير المدمجة

> **تاريخ الفحص:** 7 أبريل 2026
> **النطاق:** المحلل النحوي (Parser) · عقد AST · المفسر (Interpreter) · المترجم (Compiler)

---

## القسم 1: ميزات ذُكرت كـ "مفقودة" في ملف المقارنة لكنها أُضيفت فعلياً

ملف المقارنة: `docs/مقارنة_لغة_ص_مع_لغات_أخرى.md` (القسم 12)

| # | الميزة المذكورة مفقودة | الحالة الفعلية | الأدلة |
|---|----------------------|----------------|--------|
| 1 | **أنواع جبرية (ADT)** | ⚠️ **جزئياً** — `تعداد` يدعم بيانات مرفقة في المفسر | `interpreter_new/src/visitors/statement_executor_oop.cpp:1055` — `visitEnumDecl` مُنفَّذ بالكامل مع أعضاء بقيم |
| 2 | **Extension Methods** | ✅ **مدمج بالكامل** — تم دمج `امتداد` كإعلان سياقي في `parseDeclaration()` مع عقدة AST `ExtensionDecl` ودعم كامل في المفسر | تم تنفيذه واختباره بنجاح — `examples/test_extension.ص` |
| 3 | **Type Aliases** | ❌ لا تزال مفقودة | لا يوجد أي أثر في Parser أو Interpreter |
| 4 | **Tuples** | ❌ لا تزال مفقودة | لا عقدة AST ولا تحليل نحوي |
| 5 | **Sealed Classes** | ❌ لا تزال مفقودة | لا دعم |
| 6 | **Macros** | ✅ **مدمج بالكامل** — تم دمج `ماكرو` كإعلان سياقي مع عقدة AST `MacroDecl`، يدعم: معاملات متغيرة (`...اسم`)، قيم إرجاع (ماكرو تعبيري)، أمان النطاق (hygiene)، نسخ للخلف، تداخل ماكروز | تم تنفيذه واختباره بنجاح — `examples/test_macros.ص` و `examples/test_macros_advanced.ص` |
| 7 | **Reflection** | ❌ لا تزال مفقودة | لا دعم |
| ~~8~~ | ~~**defer**~~ | ✅ **مُنفّذة** | `أجّل` — كلمة سياقية، LIFO، جملة/كتلة، مفسر + مترجم |
| 9 | **Goroutines/Channels** | ❌ لا تزال مفقودة | لا دعم أصلي |
| 10 | **Result/Option** | ❌ لا تزال مفقودة | `??` و `?.` مدعومان لكن ليسا نوع Result |
| 11 | **عامل `?` لنشر الأخطاء** | ❌ لا يزال مفقوداً | لا دعم |

### الميزات المذكورة في الجدول كمدعومة وهي فعلياً مدعومة:
- ✅ **قوالب/Generics** — `قالب<نوع ت>` مدعوم في Parser + Interpreter + Compiler (semantic)
- ✅ **قيود القوالب (Where)** — `حيث` مدعومة في Parser + Interpreter
- ✅ **سمات/واجهات** — `سمة` و `نفّذ` مدعومة كاملاً في Parser + Interpreter
- ✅ **تحميل عوامل زائدة** — `عامل` مدعوم كاملاً في Parser + Interpreter
- ✅ **بنية (struct)** — `بنية` مدعومة كاملاً
- ✅ **Async/Await** — مدعوم كاملاً في Parser + Interpreter (map-based + object-based)
- ✅ **مولدات (Generators)** — `أنتج`/`أنتج من` مدعوم كاملاً في Parser + Interpreter
- ✅ **مطابقة أنماط** — `طابق`/`عندما` مع 12 نوع نمط مدعوم كاملاً
- ✅ **List/Dict/Set Comprehension** — مدعوم كاملاً
- ✅ **عامل الأنبوب `|>`** — مدعوم كاملاً مع desugaring

---

## القسم 2: أقسام المحلل النحوي الموجودة في الكود لكن غير مدمجة أو مُعطلة

### 2.1 ملفات Parser مستقلة (غير مربوطة بـ `ParserCore.parseDeclaration()`)

| الملف | الوصف | الحالة |
|-------|-------|--------|
| `extension_parser.cpp` | محلل طرق الامتداد (Extension Methods) | ✅ **تم الدمج** — الكلمة السياقية `امتداد` مربوطة بـ `parseDeclaration()` مع عقدة `ExtensionDecl` والمفسر. الملف القديم لا يزال موجوداً كمرجع لكن النظام الجديد يعتمد على `parseExtensionDecl()` في `parser_declarations.cpp` |
| `lifetime_parser.cpp` | محلل تعليقات العمر (Lifetimes) `'أ` | ✅ **تم الدمج** — رمز `LIFETIME` في المحلل المعجمي، `parseLifetimeParams()` في `parseFunctionDecl()` و `parseStructDecl()`، دعم `&'أ` في `parseUnary()` مع `BorrowExpr`. الملف القديم لا يزال موجوداً كمرجع لكن النظام الجديد مدمج في `ParserCore` |
| `macro_pattern_parser.cpp` | محلل أنماط الماكرو `ماكرو!` | ✅ **تم الدمج** — الكلمة السياقية `ماكرو` مربوطة بـ `parseDeclaration()` مع عقدة `MacroDecl`، دعم `...` للمعاملات المتغيرة، استدعاء `اسم!(وسائط)` في `parsePostfix()`. الملف القديم لا يزال موجوداً كمرجع |
| `contract_parser.cpp` | محلل العقود البرمجية (يتطلب/يضمن) | ✅ **تم الدمج** — الكلمات السياقية `يتطلب`/`يضمن` مربوطة بـ `parseFunctionDecl()` مع `preconditions`/`postconditions` في `FunctionDecl`، المفسر يفحص الشروط في `expression_evaluator_calls.cpp`. الملف القديم لا يزال موجوداً كمرجع |
| `contract_syntax.cpp` | محلل العقود الذكية (Blockchain) `#[عقد_ذكي]` | ✅ **تم الدمج** — الكلمة السياقية `عقد` مربوطة بـ `parseDeclaration()` تستدعي `parseClassDecl()` مع `isContract=true` في `ClassDecl`. الملف القديم لا يزال موجوداً كمرجع |
| `property_test_attr.cpp` | محلل سمة اختبار الخصائص `#[اختبار_خاصية]` | ✅ **تم الدمج** — نظام `اختبر` في `parseTestDecl()` يدعم إعدادات اختبار الخصائص: `تكرارات N`، `بذرة S`، `حد_أقصى M`. المفسر يكرر جسم الاختبار N مرة مع متغيرات `_تكرار` و`_بذرة`. الملف القديم لا يزال موجوداً كمرجع |
| `ui_parser.cpp` | محلل واجهة المستخدم التصريحية | ⚠️ **مستقل جزئياً** — يملك `UIParser` class خاص، لا يُستدعى من `ParserCore` لكن SIR opcodes للـ UI موجودة في المترجم |
| `generics_parser.cpp` | محلل معمّمات منفصل | ✅ **مدمج بالفعل** — `parseTemplateParameters()` يدعم قيود السمات inline: `قالب<نوع ت: سمة>` مع `TypeParameter.constraints`. الملف القديم تطبيق بديل مستقل |
| `constraint_parser.cpp`/`.hpp` | محلل قيود الأنواع المتقدمة | ✅ **تم الدمج** — الكلمة السياقية `حيث` مربوطة بـ `parseFunctionDecl()` مع `WhereConstraint` في `FunctionDecl`. القيود تُخزَّن كمعلومات بنيوية (ت: سمة1 + سمة2). الملف القديم لا يزال موجوداً كمرجع |

### 2.2 ملفات Parser مدمجة ومرتبطة بـ `ParserCore`

| الملف | مدمج مع المفسر؟ | مدمج مع المترجم؟ |
|-------|-----------------|-----------------|
| `parser_core.cpp` | ✅ | ✅ |
| `parser_core_main.cpp` | ✅ | ✅ |
| `parser_core_helpers.cpp` | ✅ | ✅ |
| `parser_core_oop.cpp` | ✅ | ⚠️ semantic فقط |
| `parser_declarations.cpp` | ✅ | ⚠️ semantic فقط |
| `parser_expressions.cpp` | ✅ | ⚠️ semantic فقط |
| `parser_statements.cpp` | ✅ | ⚠️ semantic فقط |
| `parser_modules.cpp` | ✅ | ⚠️ SIR opcodes |
| `async_parser.cpp` | ✅ | ❌ |
| `await_parser.cpp` | ✅ | ❌ |
| `class_parser.cpp` | ✅ | ⚠️ semantic فقط |
| `constructor_parser.cpp` | ✅ | ⚠️ semantic فقط |
| `comprehension_parser.cpp` | ✅ | ❌ |
| `dict_comprehension.cpp` | ✅ | ❌ |
| `pipeline_parser.cpp` | ✅ | ❌ |
| `yield_parser.cpp` | ✅ | ❌ |
| `pattern_guard.cpp` | ✅ | ❌ |
| `doc_comment_parser.cpp` | ✅ | N/A |
| `error_recovery.cpp` | ✅ | ✅ |
| `extern_parser.cpp` | ✅ | ✅ (sadc) |
| `parser_advanced.cpp` | ✅ | ⚠️ |

---

## القسم 3: عقد AST يتم تحليلها لكن لا تُنفَّذ بالكامل

### 3.1 التعبيرات (Expressions)

| عقدة AST | Parser | Interpreter | Compiler (semantic) | Compiler (LLVM codegen) |
|----------|--------|-------------|--------------------|-----------------------|
| `BinaryExpr` | ✅ | ✅ | ✅ | ✅ |
| `UnaryExpr` | ✅ | ✅ | ✅ | ✅ |
| `TernaryExpr` | ✅ | ✅ | ⚠️ | ❌ |
| `LiteralExpr` | ✅ | ✅ | ✅ | ✅ |
| `VariableExpr` | ✅ | ✅ | ✅ | ✅ |
| `AssignExpr` | ✅ | ✅ | ✅ | ✅ |
| `BorrowExpr` | ✅ | ✅ | ✅ (borrow checker) | ⚠️ SIR only |
| `CallExpr` | ✅ | ✅ | ✅ | ✅ |
| `IndexExpr` | ✅ | ✅ | ✅ | ✅ |
| `MemberExpr` | ✅ | ✅ | ⚠️ | ⚠️ |
| `ArrayExpr` | ✅ | ✅ | ✅ | ✅ |
| `MapExpr` | ✅ | ✅ | ⚠️ | ❌ |
| `WalrusExpr` | ✅ | ✅ | ❌ | ❌ |
| `AwaitExpr` | ✅ | ✅ | ❌ | ❌ |
| `LambdaExpr` | ✅ | ✅ | ❌ | ❌ |
| `ListComprehensionExpr` | ✅ | ✅ | ❌ | ❌ |
| `DictComprehensionExpr` | ✅ | ✅ | ❌ | ❌ |
| `SetComprehensionExpr` | ✅ | ✅ | ❌ | ❌ |
| `GeneratorExpr` | ✅ | ✅ | ❌ | ❌ |
| `DecoratorExpr` | ✅ | ✅ | ❌ | ❌ |
| `InlineAsmExpr` | ✅ | ✅ (تحذير) | ✅ | ✅ (sadc) |
| `RangeExpr` | ✅ | ✅ | ⚠️ | ❌ |
| `SliceExpr` | ✅ | ✅ | ❌ | ❌ |
| `OptionalChainExpr` | ✅ | ✅ | ❌ | ❌ |
| `NullCoalesceExpr` | ✅ | ✅ | ❌ | ❌ |
| `NewExpr` | ✅ | ✅ | ⚠️ | ⚠️ |
| `MemberAccessExpr` | ✅ | ✅ | ⚠️ | ⚠️ |
| `MemberAssignExpr` | ✅ | ✅ | ⚠️ | ❌ |
| `IndexAssignExpr` | ✅ | ✅ | ❌ | ❌ |
| `MethodCallExpr` | ✅ | ✅ | ⚠️ | ⚠️ |
| `ThisExpr` | ✅ | ✅ | ⚠️ | ❌ |
| `SuperExpr` | ✅ | ✅ | ⚠️ | ❌ |
| `SizeofExpr` | ✅ | ✅ | ⚠️ | ✅ (sadc) |
| `AtomicExpr` | ✅ | ✅ | ⚠️ | ✅ (sadc) |

### 3.2 التوجيهات (@)

| عقدة AST | Parser | Interpreter | Compiler |
|----------|--------|-------------|----------|
| `UnsafeBlockStmt` | ✅ | ✅ (ينفذ كنطاق عادي) | ✅ (sadc) |
| `ComptimeBlockStmt` | ✅ | ✅ (ينفذ كنطاق عادي) | ✅ (sadc) |
| `VolatileVarDeclStmt` | ✅ | ✅ (يعلن متغير عادي) | ✅ (sadc) |

### 3.3 الجمل (Statements)

| عقدة AST | Parser | Interpreter | Compiler (LLVM codegen) |
|----------|--------|-------------|------------------------|
| `ExprStmt` | ✅ | ✅ | ✅ |
| `VarDeclStmt` | ✅ | ✅ | ✅ |
| `IfStmt` | ✅ | ✅ | ✅ |
| `WhileStmt` | ✅ | ✅ | ✅ |
| `ForStmt` | ✅ | ✅ | ⚠️ |
| `ForRangeStmt` | ✅ | ✅ | ❌ |
| `SwitchStmt` | ✅ | ✅ | ⚠️ SIR Switch |
| `ReturnStmt` | ✅ | ✅ | ✅ |
| `YieldStmt` | ✅ | ✅ | ❌ |
| `BreakStmt` | ✅ | ✅ | ✅ |
| `ContinueStmt` | ✅ | ✅ | ✅ |
| `BlockStmt` | ✅ | ✅ | ✅ |
| `TryStmt` | ✅ | ✅ | ❌ |
| `RaiseStmt` | ✅ | ✅ | ❌ |
| `WithStmt` | ✅ | ✅ (RAII كامل) | ❌ |
| `MatchStmt` | ✅ | ✅ | ❌ |

### 3.4 التصريحات (Declarations)

| عقدة AST | Parser | Interpreter | Compiler (semantic) | Compiler (LLVM) |
|----------|--------|-------------|--------------------|--------------  |
| `FunctionDecl` | ✅ | ✅ | ✅ | ✅ |
| `ClassDecl` | ✅ | ✅ | ✅ | ⚠️ |
| `FieldDecl` | ✅ | ✅ | ⚠️ | ❌ |
| `MethodDecl` | ✅ | ✅ | ⚠️ | ❌ |
| `PropertyDecl` | ✅ | ✅ | ❌ | ❌ |
| `ConstructorDecl` | ✅ | ✅ | ⚠️ | ❌ |
| `DestructorDecl` | ✅ | ✅ | ❌ | ❌ |
| `EnumDecl` | ✅ | ✅ | ❌ | ❌ |
| `StructDecl` | ✅ | ✅ | ❌ | ❌ |
| `TestDecl` | ✅ | ✅ | ❌ | ❌ |
| `ImportStmt` | ✅ | ✅ | ⚠️ SIR | ❌ |
| `FromImportStmt` | ✅ | ✅ | ⚠️ | ❌ |
| `ExportStmt`/`ExportDecl` | ✅ | ✅ | ⚠️ SIR | ❌ |
| `ReExportStmt` | ✅ | ✅ | ❌ | ❌ |
| `TemplateFunctionDecl` | ✅ | ✅ | ✅ type_checker | ❌ |
| `TemplateClassDecl` | ✅ | ✅ | ✅ type_checker | ❌ |
| `TemplateInstantiation` | ✅ | ✅ | ✅ type_checker | ❌ |
| `NamespaceDecl` | ✅ | ✅ | ⚠️ stub | ❌ |
| `OperatorDecl` | ✅ | ✅ | ⚠️ stub | ❌ |
| `TraitDecl` | ✅ | ✅ | ⚠️ stub | ❌ |
| `ImplDecl` | ✅ | ✅ | ⚠️ traverse | ❌ |

### 3.5 زوار فارغون في المترجم

ملف `compiler_new/src/semantic/borrow_checker.cpp` (أسطر 754-800) يحتوي stubs فارغة:

```
visitTemplateFunctionDecl  — فارغ تماماً
visitTemplateClassDecl     — فارغ تماماً
visitTemplateInstantiation — فارغ تماماً
visitNamespaceDecl         — فارغ تماماً
visitOperatorDecl          — فارغ تماماً
visitTraitDecl             — فارغ تماماً
visitImplDecl              — يتصفح methods فقط
visitStructDecl            — فارغ تماماً
```

ملف `compiler_new/src/sir/ast_to_sir.cpp` — **لا يحتوي على أي visit methods** للعقد المتقدمة.

---

## القسم 4: ملخص حالة كل ميزة

### 4.1 الميزات الأساسية (مكتملة)

| الميزة | Parser | Interpreter | Compiler (sadc) |
|--------|--------|-------------|-----------------|
| المتغيرات (`متغير`/`ثابت`) | ✅ | ✅ | ✅ |
| الدوال (`دالة`) | ✅ | ✅ | ✅ |
| الشروط (`إذا`/`وإلا`) | ✅ | ✅ | ✅ |
| الحلقات (`بينما`/`لكل...في`) | ✅ | ✅ | ⚠️ |
| العمليات الحسابية/المنطقية | ✅ | ✅ | ✅ |
| المصفوفات | ✅ | ✅ | ✅ |
| النصوص + F-strings | ✅ | ✅ | ✅ |
| `ارجع`/`توقف`/`استمر` | ✅ | ✅ | ✅ |
| FFI (`خارجي`) | ✅ | ✅ | ✅ |

### 4.2 البرمجة الكائنية

| الميزة | Parser | Interpreter | Compiler |
|--------|--------|-------------|----------|
| أصناف + وراثة | ✅ | ✅ | ⚠️ semantic |
| باني + هدم | ✅ | ✅ | ⚠️ semantic |
| خصائص (get/set) | ✅ | ✅ | ❌ |
| محددات وصول | ✅ | ✅ | ⚠️ |
| تحميل عوامل | ✅ | ✅ | ⚠️ stub |
| سمات (`سمة`/`نفّذ`) | ✅ | ✅ | ⚠️ stub |
| بنية (`بنية`) | ✅ | ✅ | ❌ |
| تعداد (`تعداد`) | ✅ | ✅ | ❌ |

### 4.3 البرمجة الوظيفية

| الميزة | Parser | Interpreter | Compiler |
|--------|--------|-------------|----------|
| لامدا | ✅ | ✅ | ❌ |
| List/Dict/Set Comprehension | ✅ | ✅ | ❌ |
| Generator Expression | ✅ | ✅ | ❌ |
| عامل الأنبوب (`\|>`) | ✅ | ✅ | ❌ |
| Walrus (`:=`) | ✅ | ✅ | ❌ |
| مولدات (`أنتج`) | ✅ | ✅ | ❌ |

### 4.4 القوالب والميزات المتقدمة

| الميزة | Parser | Interpreter | Compiler |
|--------|--------|-------------|----------|
| قوالب (`قالب<نوع ت>`) | ✅ | ✅ | ⚠️ type_checker |
| فضاء أسماء (`فضاء`) | ✅ | ✅ | ⚠️ stub |
| مطابقة أنماط (`طابق`) | ✅ | ✅ | ❌ |
| مُزخرِفات (`@اسم`) | ✅ | ✅ | ❌ |
| `باستخدام` (with) | ✅ | ✅ | ❌ |
| Async/Await | ✅ | ✅ | ❌ |

### 4.5 البرمجة المنخفضة المستوى

| الميزة | Parser | Interpreter | Compiler (sadc) |
|--------|--------|-------------|-----------------|
| `@غير_آمن` | ✅ | ✅ | ✅ |
| `@وقت_الترجمة` | ✅ | ✅ | ✅ |
| `@متطاير` | ✅ | ✅ | ✅ |
| `@تجميع` (inline asm) | ✅ | ⚠️ | ✅ |
| `@حجم`/`@ذري` | ✅ | ✅ | ✅ |

### 4.6 أنظمة مستقلة غير مدمجة

| النظام | الملف | الحالة |
|--------|-------|--------|
| طرق الامتداد | `extension_parser.cpp` | ✅ **مدمج بالكامل** |
| Lifetimes | `lifetime_parser.cpp` | ✅ **مدمج بالكامل** |
| نظام الماكرو | `macro_pattern_parser.cpp` | ✅ **مدمج بالكامل** |
| العقود البرمجية | `contract_parser.cpp` | ✅ **مدمج بالكامل** |
| العقود الذكية | `contract_syntax.cpp` | ✅ **مدمج بالكامل** |
| قيود الأنواع | `constraint_parser.cpp` | ✅ **مدمج بالكامل** |
| اختبار الخصائص | `property_test_attr.cpp` | ✅ **مدمج بالكامل** |
| واجهة المستخدم DSL | `ui_parser.cpp` | 🔴 غير مدمج (SIR opcodes موجودة) |
| معمّمات بديلة | `generics_parser.cpp` | ✅ **مدمج بالفعل** (قيود inline في القوالب) |

### 4.7 رموز TokenType غير مُستخدمة (لا تُنتج من المحلل المعجمي)

| الفئة | الرموز | السبب |
|-------|--------|-------|
| سياقية (20) | `KEYWORD_ASYNC`, `KEYWORD_AWAIT`, `KEYWORD_LAMBDA`, `KEYWORD_YIELD`, `KEYWORD_GENERATOR`, `KEYWORD_WITH`, `KEYWORD_TEMPLATE`, `KEYWORD_NAMESPACE`, `KEYWORD_OPERATOR`, `KEYWORD_PROPERTY`, `KEYWORD_GET`, `KEYWORD_SET`, `KEYWORD_DESTRUCTOR`, `KEYWORD_MAIN`, `KEYWORD_CASE`, `KEYWORD_TEST`, `KEYWORD_TRAIT`, `KEYWORD_IMPL`, `KEYWORD_RETURNS` | **تصميم مقصود** — parser يتعرف عليها كـ IDENTIFIER |
| توجيهات (6) | `KEYWORD_ASM`, `KEYWORD_VOLATILE`, `KEYWORD_SIZEOF`, `KEYWORD_UNSAFE`, `KEYWORD_COMPTIME`, `KEYWORD_ATOMIC` | تُعالج كتوجيهات `@` |
| BeeOS (7) | `KEYWORD_LET`, `KEYWORD_MODULE`, `KEYWORD_USE`, `KEYWORD_MUT`, `KEYWORD_CONST_PTR`, `KEYWORD_MUT_PTR`, الأنواع | **غير مدمجة** |
| غير مُستخدمة (5) | `KEYWORD_PACKED`, `KEYWORD_NORETURN`, `KEYWORD_NAKED`, `KEYWORD_REGISTER`, `KEYWORD_SECTION` | **لا parsing مرتبط** |
| أنواع مدمجة (9) | `TYPE_INTEGER`..`TYPE_ANY` | لا تُنتج — parser يتعرف كـ IDENTIFIER |
| أنواع منخفضة (12) | `TYPE_U8`..`TYPE_PTR`, `TYPE_I64`..`TYPE_ISIZE` | لا تُنتج |
| Legacy (2) | `KEYWORD_ELSE_IF`, `KEYWORD_EXTENDS` | بدائل قديمة |

---

## الخلاصة

### إحصائيات عامة:

| المعيار | العدد |
|---------|-------|
| إجمالي عقد AST في `ast_visitor.h` | ~55 عقدة |
| عقد مدعومة في المفسر بالكامل | ~50 عقدة (**91%**) |
| عقد مدعومة في المترجم (LLVM codegen) | ~15 عقدة (**27%**) |
| ملفات Parser مستقلة (غير مربوطة) | **9 ملفات** |
| رموز TokenType غير مُنتجة | ~35 رمز |

### الفجوة الأكبر:
1. **المفسر** يدعم **كل** الميزات تقريباً — إنجاز ممتاز
2. **المترجم** (sadc/LLVM) يدعم الأساسيات + البرمجة المنخفضة لكن **يفتقر** لـ OOP والبرمجة الوظيفية والميزات المتقدمة
3. **9 أنظمة فرعية** في مجلد Parser لم تُدمج بعد مع `ParserCore`
