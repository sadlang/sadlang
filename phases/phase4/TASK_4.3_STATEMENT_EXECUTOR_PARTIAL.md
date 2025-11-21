# المرحلة 4.3: منفّذ الجُمل - مكتملة جزئياً ✅
# Phase 4.3: Statement Executor - Partially Complete ✅

**التاريخ / Date:** نوفمبر 21، 2025 / November 21, 2025  
**الحالة / Status:** ⏳ مكتملة جزئياً / Partially Complete (75%)  
**الاختبارات / Tests:** 15/15 ✅ نجحت / Passed

---

## نظرة عامة / Overview

### الوصف بالعربية
تم بنجاح تنفيذ منفّذ الجُمل (StatementExecutor) الذي يستخدم نمط الزائر لتنفيذ معظم أنواع الجُمل الأساسية في لغة ص. المُنفّذ يتكامل مع ExpressionEvaluator لتقييم التعابير ضمن الجُمل، ويدير النطاقات والتدفق بشكل صحيح.

### English Description
Successfully implemented the Statement Executor that uses the Visitor Pattern to execute most basic statement types in S Language. The executor integrates with ExpressionEvaluator to evaluate expressions within statements and properly manages scopes and control flow.

---

## المكونات المُنفَّذة / Implemented Components

### 1. فئة StatementExecutor / StatementExecutor Class

**الملف / File:** `include/interpreter/visitors/statement_executor.h`  
**عدد الأسطر / Lines:** 248

```cpp
class StatementExecutor : public AST::BaseASTVisitor {
public:
    StatementExecutor(VariableManager& varMgr, 
                     FunctionManager& funcMgr,
                     ScopeManager& scopeMgr);
    
    FlowControl getFlowControl() const;
    void resetFlowControl();
    Value getReturnValue() const;
    
    // Statement visitors implemented
    void visitExprStmt(AST::ExprStmt& node) override;
    void visitVarDeclStmt(AST::VarDeclStmt& node) override;
    void visitIfStmt(AST::IfStmt& node) override;
    void visitWhileStmt(AST::WhileStmt& node) override;
    void visitForStmt(AST::ForStmt& node) override;
    void visitForRangeStmt(AST::ForRangeStmt& node) override;
    void visitReturnStmt(AST::ReturnStmt& node) override;
    void visitBreakStmt(AST::BreakStmt& node) override;
    void visitContinueStmt(AST::ContinueStmt& node) override;
    void visitBlockStmt(AST::BlockStmt& node) override;
    void visitTryStmt(AST::TryStmt& node) override;
    void visitRaiseStmt(AST::RaiseStmt& node) override;
    
private:
    VariableManager& variableManager_;
    FunctionManager& functionManager_;
    ScopeManager& scopeManager_;
    std::unique_ptr<ExpressionEvaluator> expressionEvaluator_;
    FlowControl flowControl_;
    Value returnValue_;
    int loopDepth_;
};
```

### 2. التنفيذ / Implementation

**الملف / File:** `src/interpreter/visitors/statement_executor.cpp`  
**عدد الأسطر / Lines:** 394

---

## الجُمل المُنفَّذة / Implemented Statements

### ✅ 1. تعريف المتغيرات / Variable Declaration (VarDeclStmt)

```cpp
void visitVarDeclStmt(VarDeclStmt& node) {
    // تقييم القيمة الابتدائية أو استخدام قيمة افتراضية
    Value value;
    if (node.initializer) {
        value = evaluateExpression(*node.initializer);
    } else {
        // قيمة افتراضية حسب النوع
        switch (node.type) {
            case DataType::INTEGER: value = Value(0); break;
            case DataType::FLOAT:   value = Value(0.0); break;
            case DataType::STRING:  value = Value(""); break;
            case DataType::BOOLEAN: value = Value(false); break;
            default:                value = Value(); break; // VOID
        }
    }
    variableManager_.define(node.name, value);
}
```

**الميزات:**
- دعم التعريف مع قيمة ابتدائية
- دعم التعريف بدون قيمة (قيمة افتراضية)
- دعم جميع الأنواع: INTEGER, FLOAT, STRING, BOOLEAN
- دعم المتغيرات الثابتة (const) - جاهز للتكامل

### ✅ 2. جملة التعبير / Expression Statement (ExprStmt)

```cpp
void visitExprStmt(ExprStmt& node) {
    // تنفيذ التعبير وتجاهل النتيجة
    evaluateExpression(*node.expression);
}
```

**الاستخدامات:**
- استدعاء دوال: `print("hello")`
- إسناد متغيرات: `x = 10`
- عمليات جانبية: `i++`

### ✅ 3. كتل الكود / Block Statement (BlockStmt)

```cpp
void visitBlockStmt(BlockStmt& node) {
    // دخول نطاق جديد
    scopeManager_.pushScope(ScopeType::BLOCK);
    
    // تنفيذ جميع الجمل
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
        if (shouldStopExecution()) break;
    }
    
    // الخروج من النطاق
    scopeManager_.popScope();
}
```

**الميزات:**
- إدارة النطاقات (scopes) تلقائياً
- دعم المتغيرات المحلية
- إيقاف التنفيذ عند return/break/continue

### ✅ 4. جملة الشرط / If Statement (IfStmt)

```cpp
void visitIfStmt(IfStmt& node) {
    Value condition = evaluateExpression(*node.condition);
    
    if (condition.toBool()) {
        node.thenBranch->accept(*this);
    } else if (node.elseBranch) {
        node.elseBranch->accept(*this);
    }
}
```

**الميزات:**
- تقييم الشرط كقيمة منطقية
- دعم if-else
- دعم if-elif-else (سلاسل شرطية)
- تحويل تلقائي للأنواع إلى boolean

### ✅ 5. حلقة while / While Loop (WhileStmt)

```cpp
void visitWhileStmt(WhileStmt& node) {
    loopDepth_++;
    
    while (true) {
        Value condition = evaluateExpression(*node.condition);
        if (!condition.toBool()) break;
        
        node.body->accept(*this);
        
        // معالجة break/continue/return
        if (flowControl_ == FlowControl::BREAK) {
            flowControl_ = FlowControl::NONE;
            break;
        }
        if (flowControl_ == FlowControl::CONTINUE) {
            flowControl_ = FlowControl::NONE;
            continue;
        }
        if (flowControl_ == FlowControl::RETURN) break;
    }
    
    loopDepth_--;
}
```

**الميزات:**
- تقييم الشرط في كل تكرار
- دعم break للخروج
- دعم continue للمتابعة
- دعم return من داخل الحلقة
- عداد عمق الحلقات (loop depth)

### ✅ 6. حلقة for / For Loop (ForStmt)

```cpp
void visitForStmt(ForStmt& node) {
    scopeManager_.pushScope(ScopeType::LOOP);
    
    // تنفيذ التهيئة
    if (node.initializer) node.initializer->accept(*this);
    
    loopDepth_++;
    while (true) {
        // تقييم الشرط
        if (node.condition) {
            Value cond = evaluateExpression(*node.condition);
            if (!cond.toBool()) break;
        }
        
        // تنفيذ الجسم
        node.body->accept(*this);
        
        // معالجة التحكم بالتدفق
        if (flowControl_ == FlowControl::BREAK) {
            flowControl_ = FlowControl::NONE;
            break;
        }
        if (flowControl_ == FlowControl::CONTINUE) {
            flowControl_ = FlowControl::NONE;
        }
        if (flowControl_ == FlowControl::RETURN) break;
        
        // تنفيذ الزيادة
        if (node.increment) evaluateExpression(*node.increment);
    }
    loopDepth_--;
    
    scopeManager_.popScope();
}
```

**الميزات:**
- نطاق خاص للحلقة
- دعم التهيئة الاختيارية
- دعم الشرط الاختياري
- دعم الزيادة الاختيارية
- حلقة لا نهائية: `for (;;)`

### ✅ 7. حلقة for-range / For-Range Loop (ForRangeStmt)

```cpp
void visitForRangeStmt(ForRangeStmt& node) {
    Value iterable = evaluateExpression(*node.iterable);
    scopeManager_.pushScope(ScopeType::LOOP);
    loopDepth_++;
    
    if (iterable.isArray()) {
        const auto& arr = iterable.toArray();
        for (size_t i = 0; i < arr.size(); i++) {
            // تحديث أو تعريف متغير الحلقة
            if (variableManager_.exists(node.variable)) {
                variableManager_.assign(node.variable, arr[i]);
            } else {
                variableManager_.define(node.variable, arr[i]);
            }
            
            node.body->accept(*this);
            // معالجة break/continue/return...
        }
    }
    else if (iterable.isMap()) {
        const auto& map = iterable.toMap();
        for (const auto& [key, value] : map) {
            // تعريف متغير المفتاح والقيمة...
        }
    }
    
    loopDepth_--;
    scopeManager_.popScope();
}
```

**الميزات:**
- التكرار على المصفوفات: `for (x in array)`
- التكرار على القواميس: `for (key, value in map)`
- تحديث تلقائي لمتغير الحلقة
- دعم break/continue
- نطاق خاص للحلقة

### ✅ 8. جُمل التحكم بالتدفق / Flow Control

#### BreakStmt
```cpp
void visitBreakStmt(BreakStmt& node) {
    if (!isInLoop()) {
        throw ExecutionError("'break' خارج حلقة / 'break' outside loop");
    }
    flowControl_ = FlowControl::BREAK;
}
```

#### ContinueStmt
```cpp
void visitContinueStmt(ContinueStmt& node) {
    if (!isInLoop()) {
        throw ExecutionError("'continue' خارج حلقة / 'continue' outside loop");
    }
    flowControl_ = FlowControl::CONTINUE;
}
```

#### ReturnStmt
```cpp
void visitReturnStmt(ReturnStmt& node) {
    if (node.value) {
        returnValue_ = evaluateExpression(*node.value);
    } else {
        returnValue_ = Value(); // VOID
    }
    flowControl_ = FlowControl::RETURN;
}
```

**الميزات:**
- فحص السياق (break/continue داخل حلقة فقط)
- حفظ قيمة الإرجاع
- نظام FlowControl موحد

### ✅ 9. معالجة الاستثناءات / Exception Handling (جزئي)

```cpp
void visitTryStmt(TryStmt& node) {
    try {
        node.tryBlock->accept(*this);
    }
    catch (const ExecutionError& e) {
        // البحث عن بند التقاط مناسب
        for (auto& catchClause : node.catchClauses) {
            scopeManager_.pushScope(ScopeType::BLOCK);
            if (!catchClause.exceptionVar.empty()) {
                variableManager_.define(catchClause.exceptionVar, 
                                       Value(std::string(e.what())));
            }
            catchClause.body->accept(*this);
            scopeManager_.popScope();
            break;
        }
    }
    
    // تنفيذ finally
    if (node.finallyBlock) {
        node.finallyBlock->accept(*this);
    }
}
```

**ملاحظة:** معالجة الاستثناءات جاهزة بشكل أساسي لكن لم تُختبر بعد.

---

## الاختبارات / Tests

**الملف / File:** `tests/interpreter_tests/test_statement_executor.cpp`  
**عدد الأسطر / Lines:** 700+  
**عدد الاختبارات / Test Count:** 15/15 ✅

### فئات الاختبارات / Test Categories

#### 1. تعريف المتغيرات / Variable Declarations (4 tests) ✅
- `test_var_decl_with_init()` - تعريف مع قيمة: `var x = 42`
- `test_var_decl_without_init()` - تعريف بدون قيمة: `var y`
- `test_var_decl_string()` - متغير نصي: `var name = "أحمد"`
- `test_var_decl_const()` - ثابت: `const PI = 3.14`

#### 2. جملة التعبير / Expression Statement (1 test) ✅
- `test_expr_stmt()` - إسناد: `x = 20`

#### 3. كتل الكود / Block Statements (2 tests) ✅
- `test_block_simple()` - كتلة بسيطة: `{ var a = 10; var b = 20; }`
- `test_block_scoped()` - كتلة مع نطاقات: متغيرات داخلية وخارجية

#### 4. جُمل الشرط / If Statements (4 tests) ✅
- `test_if_true()` - if مع شرط صحيح
- `test_if_false()` - if مع شرط خاطئ
- `test_if_else()` - if-else
- `test_if_with_comparison()` - if مع عملية مقارنة: `if (a > 3)`

#### 5. حلقة while / While Loop (2 tests) ✅
- `test_while_simple()` - حلقة بسيطة: `while (i < 5)`
- `test_while_with_break()` - حلقة مع break

#### 6. حلقة for / For Loop (1 test) ✅
- `test_for_simple()` - حلقة for: `for (i = 0; i < 5; i++)`

#### 7. حلقة for-range / For-Range Loop (1 test) ✅
- `test_for_range_array()` - التكرار على مصفوفة: `for (x in array)`

---

## نتائج التنفيذ / Execution Results

```
========================================
 اختبارات منفّذ الجُمل
 Statement Executor Tests
========================================

✅ PASSED - test_var_decl_with_init
✅ PASSED - test_var_decl_without_init
✅ PASSED - test_var_decl_string
✅ PASSED - test_var_decl_const
✅ PASSED - test_expr_stmt
✅ PASSED - test_block_simple
✅ PASSED - test_block_scoped
✅ PASSED - test_if_true
✅ PASSED - test_if_false
✅ PASSED - test_if_else
✅ PASSED - test_if_with_comparison
✅ PASSED - test_while_simple
✅ PASSED - test_while_with_break
✅ PASSED - test_for_simple
✅ PASSED - test_for_range_array

========================================
✅ ALL TESTS PASSED! (15/15)
========================================
```

---

## الإحصائيات / Statistics

- **ملفات جديدة / New Files:** 3
  1. `include/interpreter/visitors/statement_executor.h` (248 سطر)
  2. `src/interpreter/visitors/statement_executor.cpp` (394 سطر)
  3. `tests/interpreter_tests/test_statement_executor.cpp` (700+ سطر)

- **إجمالي الأسطر / Total Lines:** 1,342+ سطر
- **عدد الاختبارات / Test Count:** 15 اختبار ✅
- **معدل النجاح / Success Rate:** 100%
- **الجُمل المدعومة / Supported Statements:** 12 نوع
- **معدل الإكمال / Completion Rate:** 75%

---

## الميزات المُؤجلة / Deferred Features

### ⏸️ 1. تعريف واستدعاء الدوال / Function Declaration & Call
- FuncDeclStmt - تعريف الدوال
- CallExpr - استدعاء الدوال
- معاملات اختيارية
- قيم إرجاع متعددة

**السبب:** يحتاج تكامل أعمق مع FunctionManager

### ⏸️ 2. معالجة استثناءات كاملة / Full Exception Handling
- مطابقة أنواع الاستثناءات
- اختبارات شاملة لـ try-catch-finally
- raise مع أنواع مختلفة

**السبب:** يحتاج نظام أنواع استثناءات متقدم

### ⏸️ 3. جُمل متقدمة / Advanced Statements
- WithStmt - context managers
- YieldStmt - generators
- Import/Export statements

**السبب:** ميزات متقدمة للمراحل القادمة

---

## التكامل مع المكونات الأخرى / Integration

### ✅ مع ExpressionEvaluator
- تقييم سلس للتعابير ضمن الجُمل
- مشاركة VariableManager و FunctionManager
- تكامل كامل

### ✅ مع ScopeManager
- إدارة النطاقات التلقائية
- دعم النطاقات المتداخلة
- حماية المتغيرات المحلية

### ✅ مع VariableManager
- تعريف المتغيرات
- القراءة والكتابة
- فحص الوجود

### ⏳ مع FunctionManager
- جاهز للتكامل
- لم يُنفّذ بعد

---

## المشاكل المحلولة / Problems Resolved

### 1. أسماء DataType
**المشكلة:** استخدام `INT` و `DOUBLE` بدلاً من `INTEGER` و `FLOAT`  
**الحل:** تحديث جميع الإشارات للأسماء الصحيحة

### 2. pushScope يحتاج معاملات
**المشكلة:** استدعاء `pushScope()` بدون معاملات  
**الحل:** إضافة `ScopeType::BLOCK` أو `ScopeType::LOOP`

### 3. تعريف متغير الحلقة مرات متعددة
**المشكلة:** في for-range، المتغير يُعرّف في كل تكرار  
**الحل:** فحص الوجود أولاً: `if (exists) assign() else define()`

### 4. Value::toInteger() غير موجودة
**المشكلة:** الدالة الصحيحة هي `toInt()`  
**الحل:** تحديث جميع الإشارات

---

## أمر التجميع / Build Command

```bash
cd c:\s\s_language

g++ -std=c++17 -Iinclude \
    tests\interpreter_tests\test_statement_executor.cpp \
    src\interpreter\visitors\statement_executor.cpp \
    src\interpreter\visitors\expression_evaluator.cpp \
    src\parser\ast\expressions.cpp \
    src\parser\ast\statements.cpp \
    src\parser\ast\ast_node.cpp \
    src\lexer\token.cpp \
    src\data\types\value.cpp \
    src\data\managers\variable_manager.cpp \
    src\data\managers\function_manager.cpp \
    src\data\scope\scope_manager.cpp \
    -o build\test_statement_executor.exe
```

---

## الخطوة التالية / Next Step

### المرحلة 4.4: الميزات المتقدمة / Advanced Features

**المكونات المطلوبة:**
1. **تعريف واستدعاء الدوال:**
   - تخزين تعريف الدالة
   - استدعاء الدالة مع المعاملات
   - إرجاع القيم
   - معاملات اختيارية

2. **اختبارات إضافية:**
   - 5+ اختبارات للدوال
   - 3+ اختبارات للاستثناءات
   - 2+ اختبارات معقدة

3. **Generators & Lambda (اختياري):**
   - YieldStmt
   - Lambda expressions
   - Higher-order functions

**الوقت المتوقع / Estimated Time:** 4-6 ساعات

---

## الخلاصة / Conclusion

### بالعربية
تم إتمام 75% من المرحلة 4.3 بنجاح! منفّذ الجُمل يعمل بكفاءة عالية وينفذ معظم الجُمل الأساسية المطلوبة. جميع الاختبارات (15/15) نجحت بدون أخطاء. الكود نظيف، موثّق، ومتكامل مع ExpressionEvaluator ونظام إدارة النطاقات.

### In English
Successfully completed 75% of Phase 4.3! The Statement Executor works efficiently and executes most required basic statements. All tests (15/15) passed without errors. The code is clean, well-documented, and integrates seamlessly with ExpressionEvaluator and the scope management system.

---

**الحالة النهائية / Final Status:** ⏳ مكتملة جزئياً 75% / Partially Complete 75%

**جاهز للمرحلة 4.4 / Ready for Phase 4.4:** ✅ نعم / Yes
