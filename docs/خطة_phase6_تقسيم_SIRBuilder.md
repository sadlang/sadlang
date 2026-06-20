# خطة Phase 6 — تقسيم god-class `SIRBuilder` إلى مكوّنات (F-06)

> **المرجع:** `docs/خطة_إعادة_الهيكلة_المعمارية.md` § F-06
> **الذاكرة المرتبطة:** `/memories/repo/phase6_sirbuilder_analysis.md`
> **الحالة:** مُجدوَلة لجلسات لاحقة بعد إغلاق Phase 7
> **آخر تحديث:** 2025-12 (بعد دفع 5 commits للـremote)

---

## 1. الواقع الحالي (الأرقام)

| البند | القيمة |
|-------|--------|
| الـheader | `compiler/include/frontend/sir_builder.h` (2011 سطر) |
| الـcpp files | 67 ملف `compiler/src/frontend/sir_builder_*.cpp` |
| إجمالي السطور | 37,121 سطر |
| تعريفات methods | 129 موقع `SIRBuilder::` |
| حقول state | ~25 حقل private |
| اختبارات (baseline) | 793/837 (94.7%) |

## 2. لماذا التقسيم؟

- **CW-01 (SRP):** صنف واحد يحوي ~150 method = انتهاك صارخ
- **CW-05 (≤800 سطر):** الـheader 2011 سطر، عدة .cpp تتجاوز 1000 سطر
- صعوبة الفهم: أي ملف cpp يستطيع لمس أي حقل من الـ25
- صعوبة الاختبار الوحدي

## 3. لماذا لا يمكن التنفيذ في جلسة واحدة؟

1. **حجم التغيير:** يتطلب لمس 37K سطر
2. **دورات البناء:** كل تغيير = `cmake --build` (~5 د) + `python tests/dual_execution/runner.py` (~10 د)
3. **خطر الكسر:** أي تغيير ذري في 1000+ سطر = احتمال شبه مؤكد لتراجع 793/837
4. **حدود السياق:** المهمة تتجاوز قدرة جلسة واحدة لـsubagent
5. **BF-23 (Incremental Fix):** القاعدة نفسها تفرض تقسيم العمل

---

## 4. التصميم النهائي المستهدف

```
┌─────────────────────────────────────────────────────────┐
│  SIRBuilder (facade) — public API ثابت                  │
│  ├── unique_ptr<SIRBuilderContext> ctx_                 │
│  ├── unique_ptr<ExpressionBuilder> expressions_         │
│  ├── unique_ptr<StatementBuilder>  statements_          │
│  ├── unique_ptr<CallBuilder>       calls_               │
│  ├── unique_ptr<BuiltinBuilder>    builtins_            │
│  ├── unique_ptr<MethodCallBuilder> methodCalls_         │
│  ├── unique_ptr<ClassBuilder>      classes_             │
│  └── unique_ptr<TemplateBuilder>   templates_           │
└─────────────────────────────────────────────────────────┘

SIRBuilderContext (struct — كل الحالة المشتركة):
  - module_, currentFunction_, currentBlock_
  - scopeStack_, functionTable_, classTable_
  - staticFields_, staticMethods_, funcRefNames_
  - namespaceMembers_, adtEnumTable_
  - loopStack_, lambdaAliases_, channelTypeMap_
  - closureCaptures_, genericScopeStack_
  - templateFunctions_, templateClasses_
  - traitDefaultImpls_, instantiatedTemplates_
  - macros_, errors_, moduleResolver_, moduleCache_
  - deferredStatements_, finallyStack_
  - classInstanceTypes_, paramClassTypes_
  - currentClassName_, currentScanFuncName_
  - nextTempRegister_, nextLabel_, currentScopeLevel_
  - moduleMode_, processedModules_, currentFilePath_
  + helpers: newTempRegister(), newLabel(), enter/exitScope(),
             addVariable(), lookupVariable(), reportError(),
             enter/exitLoop(), getCurrentLoop(),
             enter/exitGenericScope(), setTypeSubstitution(),
             resolveType(), addInstruction(), createBasicBlock(),
             saveContext(), restoreContext()

ExpressionBuilder (حوالي 28 method):
  buildExpression, buildBinaryOp, buildUnaryOp, buildShortCircuitLogical,
  buildVariableAccess, buildLiteral, buildStringMethod, buildNewObject,
  buildMemberAccess, buildExprMember, buildExprMemberAssign,
  buildExprTernary, buildExprIndex, buildExprIndexAssign, buildExprArray,
  buildExprTuple, buildExprMap, buildExprWalrus, buildExprLambda,
  buildExprRange, buildExprListComp, buildExprDictComp, buildExprSetComp,
  buildExprGenerator, buildExprInlineAsm, buildExprSizeof,
  buildExprAtomic, buildExprOptionalChain, buildExprNullCoalesce,
  buildExprSlice, buildExprErrorPropagate

StatementBuilder (حوالي 18 method):
  buildStatement, buildIfStatement, buildMatchStatement, buildWhileLoop,
  buildForLoop, buildForRangeLoop, buildReturnStatement,
  buildBreakStatement, buildContinueStatement, buildAssignment,
  buildLocalVariable, buildDeferredClosure, emitRunDeferredClosures,
  emitPopFunctionCleanupHandler, buildStatement_Exceptions,
  buildStatement_Generators, buildStatement_Types,
  buildStatement_Advanced, buildStatement_Test,
  buildStatement_Property, buildStatement_Go, buildStatement_Select,
  buildStatement_Extension, buildStatement_TypesAndReExport,
  buildMatchPatternCondition

CallBuilder (حوالي 8 method):
  buildFunctionCall, buildMacroCallExpansion, buildCallableObjectInvoke,
  buildSuperConstructorCall, buildCallArgumentsList,
  fillDefaultCallArguments, buildMethodCall

BuiltinBuilder (حوالي 16 method):
  buildBuiltinCallCore, buildBuiltinCallSimd, buildBuiltinCallSystem,
  buildBuiltinCallNetwork, buildBuiltinSystem_{Embedded, Security, FFI,
  Async, OsCore, OsHardware, OsSystem, Uefi, UI},
  buildBuiltinMathCall, buildBuiltinStringArrayCall,
  buildBuiltinIOUtilsCall, buildNetworkBuiltinInstruction,
  isStdlibModuleImported, getRequiredModuleForBuiltin

MethodCallBuilder (حوالي 8 method):
  buildChannelMethodCall, buildMutexMethodCall, buildFutureMethodCall,
  buildWaitGroupMethodCall, buildArrayBasicMethodCall,
  buildArrayHigherOrderMethodCall, buildStringBuiltinMethodCall,
  buildMapBuiltinMethodCall

ClassBuilder (حوالي 6 method):
  buildClass, buildClassConstructor, buildClassOperator,
  buildTrait, buildImpl, buildGlobalVariable

TemplateBuilder (حوالي 14 method):
  buildTemplateFunction, instantiateTemplate (×2),
  buildModule (entry point — يبقى في facade فعلياً),
  buildFunction (يبقى في facade ويفوّض للbuilders),
  buildImportStmt, buildFromImportStmt, setCurrentFilePath,
  + private helpers:
    astTypeToSIRType, astTypeToSadType, inferReturnTypeFromBody,
    hasReturnWithValue, inferExprType, inferParamTypesFromCallSites,
    inferLambdaParamTypes, inferLambdaParamFromExpr,
    inferLambdaParamFromStmt, scanCallSitesInStmt, scanCallSitesInExpr,
    binaryOpToOpcode, unaryOpToOpcode, areTypesCompatible,
    collectFreeVarsExpr, collectFreeVarsStmt, convertType,
    compileAndCacheModule, mergeCachedArtifacts
```

---

## 5. خطة الجلسات (8 جلسات منفصلة)

### **جلسة 1 — البنية التحتية (`SIRBuilderContext`)** ⏱ ~2 ساعة
**الهدف:** إنشاء `SIRBuilderContext` يحوي كل الحالة، **مع إبقاء SIRBuilder يستخدمها داخلياً عبر inheritance أو composition**.

**الخطوات:**
1. إنشاء `compiler/include/frontend/sir_builder_context.h` يحوي:
   - struct `SIRBuilderContext` يحوي كل 25 حقل (نقل حرفي)
   - struct `VariableInfo`, `FunctionInfo`, `LoopContext`, `GenericScope`, `ADTEnumInfo`, `FinallyContext`, `CaptureInfo`, `NamespaceMemberInfo`, `ModuleSIRArtifacts` (نقل من sir_builder.h)
   - methods helper: `newTempRegister`, `newLabel`, `enter/exitScope`, `addVariable`, `lookupVariable`, `addInstruction`, `createBasicBlock`, `setCurrentBlock`, `getCurrentBlock`, `enter/exitLoop`, `getCurrentLoop`, `enter/exitGenericScope`, `setTypeSubstitution`, `resolveType`, `validateTypeParameters`, `reportError`, `saveContext`, `restoreContext`
2. إنشاء `compiler/src/frontend/sir_builder_context.cpp` بالتعريفات
3. تعديل `class SIRBuilder` ليرث من `SIRBuilderContext` **مؤقتاً** (`class SIRBuilder : public SIRBuilderContext`)
4. حذف الحقول والـhelpers المكررة من `sir_builder.h`
5. بناء + اختبار → يجب أن يبقى 793/837

**Commit:** `refactor(phase-6-step-1): استخراج SIRBuilderContext من SIRBuilder (inheritance مؤقت)`

**معايير القبول:**
- ✅ `cmake --build build --config Debug --target sad-run` ينجح
- ✅ `cmake --build build --config Release --target sad-build` ينجح
- ✅ `python tests/dual_execution/runner.py` = 793/837

---

### **جلسة 2 — `MethodCallBuilder` (الأصغر، اختبار النمط)** ⏱ ~3 ساعات
**الهدف:** أول builder حقيقي مفصول. اختيار MethodCallBuilder لأنه:
- صغير (8 methods)
- مرتبط بملفات قليلة (sir_builder_method_calls_*.cpp)
- methods موجودة وقائمة بذاتها

**الخطوات:**
1. إنشاء `compiler/include/frontend/builders/method_call_builder.h`
2. إنشاء `compiler/src/frontend/builders/method_call_builder.cpp` (+ نقل المحتوى من sir_builder_method_*.cpp)
3. `MethodCallBuilder` يحمل `SIRBuilderContext& ctx_`
4. تحديث استدعاءات `buildChannelMethodCall(...)` في sir_builder_calls.cpp إلى `methodCalls_->buildChannelMethodCall(...)`
5. حذف التعريفات القديمة من sir_builder.h
6. تحديث `cmake/sources.cmake` لإضافة الملف الجديد
7. بناء + اختبار

**Commit:** `refactor(phase-6-step-2): فصل MethodCallBuilder (8 methods)`

---

### **جلسة 3 — `BuiltinBuilder` (متوسط)** ⏱ ~4 ساعات
- 16 method
- 13 ملف cpp مرتبط (sir_builder_builtins_*.cpp)
- نقل + تحديث استدعاءات + اختبار

**Commit:** `refactor(phase-6-step-3): فصل BuiltinBuilder (16 methods, 13 files)`

---

### **جلسة 4 — `CallBuilder`** ⏱ ~3 ساعات
- 8 methods
- مرتبط بـsir_builder_calls.cpp
- الاستدعاءات الداخلية كثيرة → حذر

**Commit:** `refactor(phase-6-step-4): فصل CallBuilder`

---

### **جلسة 5 — `ClassBuilder`** ⏱ ~3 ساعات
- 6 methods
- sir_builder_class*.cpp + sir_builder_constructor*.cpp + sir_builder_operator*.cpp

**Commit:** `refactor(phase-6-step-5): فصل ClassBuilder`

---

### **جلسة 6 — `StatementBuilder` (كبير)** ⏱ ~5 ساعات
- ~25 method
- sir_builder_assign_if.cpp + sir_builder_loops*.cpp + sir_builder_match*.cpp + sir_builder_stmt_*.cpp
- الأكثر تعقيداً

**Commit:** `refactor(phase-6-step-6): فصل StatementBuilder`

---

### **جلسة 7 — `ExpressionBuilder` (الأكبر)** ⏱ ~6 ساعات
- ~28 method
- sir_builder_expr_*.cpp + sir_builder_operators.cpp
- يستدعي كل builders الأخرى

**Commit:** `refactor(phase-6-step-7): فصل ExpressionBuilder`

---

### **جلسة 8 — `TemplateBuilder` + إغلاق** ⏱ ~4 ساعات
- 14 method + private helpers
- نقل آخر helpers
- إزالة `inheritance` المؤقتة من Step 1
- `SIRBuilder` يصبح facade نظيف بـcomposition فقط
- توثيق نهائي + تحديث `architecture_refactor_status.md`

**Commit:** `refactor(phase-6-step-8): إغلاق التقسيم - SIRBuilder facade خالص`

---

## 6. قواعد صارمة لكل جلسة

| القاعدة | التفصيل |
|---------|---------|
| **BF-12** | اختبار يعيد إنتاج، اختبار ينجح بعد، 793/837 يبقى |
| **BF-23** | كل جلسة = commit مستقل |
| **CW-05** | كل ملف جديد ≤800 سطر — قسّم البـbuilders إذا تجاوزوا |
| **CW-19 (DRY)** | لا تكرار للحالة — فقط في Context |
| **BF-29** | تشغيل اختبار شامل بعد كل جلسة |
| **CW-08** | تعليقات ثنائية اللغة لكل API عام جديد |

## 7. خطر التراجع وخطة الطوارئ

**خطر:** أي جلسة قد تكسر 793/837.
**خطة الطوارئ:**
1. إذا فشل البناء بعد commit → `git revert` للـcommit الأخير
2. إذا تراجعت الاختبارات → تشخيص ملف بملف
3. الـbranch `sadlang` يبقى مستقراً — العمل في branches فرعية مقترحة:
   - `phase-6/step-1-context`
   - `phase-6/step-2-method-calls`
   - ...

## 8. ما بعد Phase 6

بعد إكمال 8 الجلسات:
- `sir_builder.h` يصبح ~300 سطر (facade فقط)
- 7 ملفات header للbuilders بأحجام معقولة
- اختبار وحدة لكل builder
- توثيق المعمارية الجديدة في `docs/`

---

**ملاحظة نهائية:** هذه الخطة محافظة عمداً. كل جلسة قابلة للإنجاز في 3-6 ساعات بـcommit واحد آمن. المجموع المتوقع: ~30 ساعة عمل موزّعة على 8 جلسات.
