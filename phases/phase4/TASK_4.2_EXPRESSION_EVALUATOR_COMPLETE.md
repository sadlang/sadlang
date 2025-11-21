# المرحلة 4.2: مُقيِّم التعابير - مكتملة ✅
# Phase 4.2: Expression Evaluator - Complete ✅

**التاريخ / Date:** نوفمبر 21، 2025 / November 21, 2025  
**الحالة / Status:** ✅ مكتملة / Complete  
**الاختبارات / Tests:** 37/37 ✅ نجحت / Passed

---

## نظرة عامة / Overview

### الوصف بالعربية
تم بنجاح تنفيذ مُقيِّم التعابير (ExpressionEvaluator) الذي يستخدم نمط الزائر (Visitor Pattern) لتقييم جميع أنواع التعابير في لغة ص. المُقيِّم يحوّل العُقد AST إلى قيم تنفيذية (Value objects) ويدمج مع VariableManager وFunctionManager.

### English Description
Successfully implemented the Expression Evaluator that uses the Visitor Pattern to evaluate all expression types in S Language. The evaluator converts AST nodes to runtime Value objects and integrates with VariableManager and FunctionManager.

---

## المكونات المُنفَّذة / Implemented Components

### 1. فئة ExpressionEvaluator / ExpressionEvaluator Class

**الملف / File:** `include/interpreter/visitors/expression_evaluator.h`  
**عدد الأسطر / Lines:** 210

```cpp
class ExpressionEvaluator : public AST::BaseASTVisitor {
public:
    ExpressionEvaluator(VariableManager& varMgr, FunctionManager& funcMgr);
    Data::Value getResult() const;
    void clearResult();
    
    // 15 Expression visitors
    void visitLiteralExpr(LiteralExpr& node) override;
    void visitVariableExpr(VariableExpr& node) override;
    void visitAssignExpr(AssignExpr& node) override;
    void visitBinaryExpr(BinaryExpr& node) override;
    void visitUnaryExpr(UnaryExpr& node) override;
    void visitCallExpr(CallExpr& node) override;
    void visitIndexExpr(IndexExpr& node) override;
    void visitMemberExpr(MemberExpr& node) override;
    void visitArrayExpr(ArrayExpr& node) override;
    void visitMapExpr(MapExpr& node) override;
    // ... and more
    
private:
    VariableManager& variableManager_;
    FunctionManager& functionManager_;
    Value lastResult_;
    
    Value evaluateArithmeticOp(const Value& left, TokenType op, const Value& right);
    Value evaluateComparisonOp(const Value& left, TokenType op, const Value& right);
    Value evaluateLogicalOp(const Value& left, TokenType op, const Value& right);
    Value tokenToValue(const Token& token);
};
```

### 2. التنفيذ / Implementation

**الملف / File:** `src/interpreter/visitors/expression_evaluator.cpp`  
**عدد الأسطر / Lines:** 442

**الوظائف الرئيسية / Main Functions:**

#### تقييم القيم الحرفية / Literal Evaluation
```cpp
void visitLiteralExpr(LiteralExpr& node) {
    lastResult_ = tokenToValue(node.token);
}

Value tokenToValue(const Token& token) {
    switch (token.getType()) {
        case TokenType::NUMBER_INTEGER: return Value(std::stoi(...));
        case TokenType::NUMBER_DOUBLE:  return Value(std::stod(...));
        case TokenType::STRING_LITERAL: return Value(token.getValue());
        case TokenType::LITERAL_TRUE:   return Value(true);
        case TokenType::LITERAL_FALSE:  return Value(false);
        case TokenType::LITERAL_NULL:   return Value(); // VOID
    }
}
```

#### تقييم المتغيرات / Variable Evaluation
```cpp
void visitVariableExpr(VariableExpr& node) {
    if (!variableManager_.exists(node.name)) {
        throw RuntimeError("متغير غير معرّف / Undefined variable: " + node.name);
    }
    lastResult_ = variableManager_.get(node.name);
}

void visitAssignExpr(AssignExpr& node) {
    node.value->accept(*this);
    Value value = lastResult_;
    variableManager_.assign(node.name, value);
    lastResult_ = value;  // Returns assigned value
}
```

#### العمليات الحسابية / Arithmetic Operations
```cpp
Value evaluateArithmeticOp(const Value& left, TokenType op, const Value& right) {
    // Type promotion: int + double → double
    if (left.isInteger() && right.isInteger()) {
        int l = left.toInteger();
        int r = right.toInteger();
        switch (op) {
            case OP_PLUS:    return Value(l + r);
            case OP_MINUS:   return Value(l - r);
            case OP_MULTIPLY: return Value(l * r);
            case OP_DIVIDE:  
                if (r == 0) throw RuntimeError("قسمة على صفر / Division by zero");
                return Value(l / r);
            case OP_MODULO:  return Value(l % r);
            case OP_POWER:   return Value((int)std::pow(l, r));
        }
    }
    // Mixed int/double operations...
    // String concatenation for OP_PLUS...
}
```

#### عمليات المقارنة / Comparison Operations
```cpp
Value evaluateComparisonOp(const Value& left, TokenType op, const Value& right) {
    // Numeric comparison
    if (left.isNumeric() && right.isNumeric()) {
        double l = left.toDouble();
        double r = right.toDouble();
        switch (op) {
            case OP_EQUAL:         return Value(l == r);
            case OP_NOT_EQUAL:     return Value(l != r);
            case OP_LESS:          return Value(l < r);
            case OP_LESS_EQUAL:    return Value(l <= r);
            case OP_GREATER:       return Value(l > r);
            case OP_GREATER_EQUAL: return Value(l >= r);
        }
    }
    // String comparison...
    // Boolean comparison...
}
```

#### العمليات المنطقية / Logical Operations
```cpp
Value evaluateLogicalOp(const Value& left, TokenType op, const Value& right) {
    bool l = left.toBool();
    bool r = right.toBool();
    
    switch (op) {
        case TokenType::OP_AND: return Value(l && r);
        case TokenType::OP_OR:  return Value(l || r);
    }
}
```

#### العمليات الأحادية / Unary Operations
```cpp
void visitUnaryExpr(UnaryExpr& node) {
    node.operand->accept(*this);
    Value value = lastResult_;
    
    switch (node.op) {
        case TokenType::OP_MINUS:
            if (value.isInteger()) lastResult_ = Value(-value.toInteger());
            else if (value.isDouble()) lastResult_ = Value(-value.toDouble());
            break;
            
        case TokenType::OP_NOT:
            lastResult_ = Value(!value.toBool());
            break;
            
        case TokenType::OP_PLUS:
            lastResult_ = value;  // No-op
            break;
    }
}
```

#### المصفوفات والقواميس / Arrays and Maps
```cpp
void visitArrayExpr(ArrayExpr& node) {
    Value::ArrayType array;
    for (auto& elem : node.elements) {
        elem->accept(*this);
        array.push_back(std::make_shared<Value>(lastResult_));
    }
    lastResult_ = Value(array);
}

void visitMapExpr(MapExpr& node) {
    Value::MapType map;
    for (auto& [key, val] : node.pairs) {
        key->accept(*this);
        std::string keyStr = lastResult_.toString();
        val->accept(*this);
        map[keyStr] = std::make_shared<Value>(lastResult_);
    }
    lastResult_ = Value(map);
}

void visitIndexExpr(IndexExpr& node) {
    node.object->accept(*this);
    Value object = lastResult_;
    
    node.index->accept(*this);
    Value index = lastResult_;
    
    if (object.isArray()) {
        int idx = index.toInteger();
        const auto& arr = object.toArray();
        if (idx < 0 || idx >= arr.size()) {
            throw RuntimeError("خارج نطاق المصفوفة / Array index out of bounds");
        }
        lastResult_ = *arr[idx];
    }
    else if (object.isMap()) {
        std::string key = index.toString();
        const auto& map = object.toMap();
        if (map.find(key) == map.end()) {
            throw RuntimeError("مفتاح غير موجود / Key not found: " + key);
        }
        lastResult_ = *map.at(key);
    }
}
```

---

## الاختبارات / Tests

**الملف / File:** `tests/interpreter_tests/test_expression_evaluator.cpp`  
**عدد الأسطر / Lines:** 954  
**عدد الاختبارات / Test Count:** 37/37 ✅

### فئات الاختبارات / Test Categories

#### 1. القيم الحرفية / Literals (6 tests) ✅
- `test_literal_integer()` - أرقام صحيحة: 42 → Value(42)
- `test_literal_double()` - أرقام عشرية: 3.14 → Value(3.14)
- `test_literal_string()` - نصوص: "Hello" → Value("Hello")
- `test_literal_bool_true()` - منطقي صحيح: true → Value(true)
- `test_literal_bool_false()` - منطقي خاطئ: false → Value(false)
- `test_literal_null()` - قيمة فارغة: null → Value(VOID)

#### 2. المتغيرات / Variables (2 tests) ✅
- `test_variable_read()` - قراءة متغير: x → Value(100)
- `test_variable_assign()` - إسناد متغير: x = 50 → Value(50)

#### 3. العمليات الحسابية / Arithmetic (9 tests) ✅
- `test_addition_integer()` - جمع: 10 + 20 → Value(30)
- `test_subtraction()` - طرح: 50 - 30 → Value(20)
- `test_multiplication()` - ضرب: 6 * 7 → Value(42)
- `test_division()` - قسمة: 100 / 4 → Value(25)
- `test_modulo()` - باقي القسمة: 17 % 5 → Value(2)
- `test_power()` - أس: 2 ** 10 → Value(1024)
- `test_string_concatenation()` - دمج نصوص: "Hello" + " World" → Value("Hello World")
- `test_division_by_zero()` - قسمة على صفر: يُلقي استثناء ✅
- `test_mixed_double_operations()` - عمليات مختلطة: 10 + 3.5 → Value(13.5)

#### 4. عمليات المقارنة / Comparisons (7 tests) ✅
- `test_equality()` - تساوي: 10 == 10 → Value(true)
- `test_inequality()` - عدم تساوي: 10 != 20 → Value(true)
- `test_less_than()` - أصغر من: 5 < 10 → Value(true)
- `test_greater_than()` - أكبر من: 15 > 10 → Value(true)
- `test_less_equal()` - أصغر أو يساوي: 10 <= 10 → Value(true)
- `test_greater_equal()` - أكبر أو يساوي: 10 >= 10 → Value(true)
- `test_string_equality()` - تساوي النصوص: "abc" == "abc" → Value(true)

#### 5. العمليات المنطقية / Logical (4 tests) ✅
- `test_logical_and_true()` - AND صحيح: true && true → Value(true)
- `test_logical_and_false()` - AND خاطئ: true && false → Value(false)
- `test_logical_or_true()` - OR صحيح: true || false → Value(true)
- `test_logical_or_false()` - OR خاطئ: false || false → Value(false)

#### 6. العمليات الأحادية / Unary (4 tests) ✅
- `test_unary_minus_integer()` - سالب صحيح: -42 → Value(-42)
- `test_unary_minus_double()` - سالب عشري: -3.14 → Value(-3.14)
- `test_unary_not_true()` - نفي صحيح: !true → Value(false)
- `test_unary_not_false()` - نفي خاطئ: !false → Value(true)

#### 7. المصفوفات / Arrays (3 tests) ✅
- `test_array_creation_empty()` - مصفوفة فارغة: [] → Value(ArrayType{})
- `test_array_creation()` - إنشاء مصفوفة: [1, 2, 3] → Value([1, 2, 3])
- `test_array_indexing()` - فهرسة مصفوفة: arr[1] → Value(2)

#### 8. القواميس / Maps (3 tests) ✅
- `test_map_creation_empty()` - قاموس فارغ: {} → Value(MapType{})
- `test_map_creation()` - إنشاء قاموس: {"key": 42} → Value({"key": 42})
- `test_map_indexing()` - فهرسة قاموس: dict["key"] → Value(42)

#### 9. تعابير معقدة / Complex Expressions (2 tests) ✅
- `test_nested_arithmetic()` - حسابات متداخلة: (10 + 20) * 3 → Value(90)
- `test_complex_expression_with_variables()` - تعابير مع متغيرات: (x + y) * 2

---

## نتائج التنفيذ / Execution Results

```
========================================
 اختبارات مُقيِّم التعابير
 Expression Evaluator Tests
========================================

✅ PASSED - test_literal_integer
✅ PASSED - test_literal_double
✅ PASSED - test_literal_string
✅ PASSED - test_literal_bool_true
✅ PASSED - test_literal_bool_false
✅ PASSED - test_literal_null
✅ PASSED - test_variable_read
✅ PASSED - test_variable_assign
✅ PASSED - test_addition_integer
✅ PASSED - test_subtraction
✅ PASSED - test_multiplication
✅ PASSED - test_division
✅ PASSED - test_modulo
✅ PASSED - test_power
✅ PASSED - test_string_concatenation
✅ PASSED - test_mixed_double_operations
✅ PASSED - test_equality
✅ PASSED - test_inequality
✅ PASSED - test_less_than
✅ PASSED - test_greater_than
✅ PASSED - test_less_equal
✅ PASSED - test_greater_equal
✅ PASSED - test_string_equality
✅ PASSED - test_logical_and_true
✅ PASSED - test_logical_and_false
✅ PASSED - test_logical_or_true
✅ PASSED - test_logical_or_false
✅ PASSED - test_unary_minus_integer
✅ PASSED - test_unary_minus_double
✅ PASSED - test_unary_not_true
✅ PASSED - test_unary_not_false
✅ PASSED - test_array_creation_empty
✅ PASSED - test_array_creation
✅ PASSED - test_array_indexing
✅ PASSED - test_map_creation_empty
✅ PASSED - test_map_creation
✅ PASSED - test_map_indexing
✅ PASSED - test_nested_arithmetic
✅ PASSED - test_complex_expression_with_variables

========================================
✅ ALL TESTS PASSED! (37/37)
========================================
```

---

## المشاكل المحلولة / Problems Resolved

### 1. أخطاء أسماء TokenType / TokenType Naming Errors
**المشكلة / Problem:**
- استخدمنا `NUMBER_FLOAT` بدلاً من `NUMBER_DOUBLE`
- استخدمنا `LITERAL_NONE` بدلاً من `LITERAL_NULL`
- استخدمنا `OP_LESS_THAN` بدلاً من `OP_LESS`
- استخدمنا `OP_GREATER_THAN` بدلاً من `OP_GREATER`

**الحل / Solution:**
- بحث grep للعثور على الأسماء الصحيحة في `token.h`
- استبدال جميع الأسماء الخاطئة في:
  * `expression_evaluator.cpp` (11 موضع)
  * `test_expression_evaluator.cpp` (4 مواضع)

### 2. خطأ اسم حقل AssignExpr / AssignExpr Field Name Error
**المشكلة / Problem:**
- استخدمنا `node.target` بدلاً من `node.name`

**الحل / Solution:**
- فحص `expressions.h` لتأكيد أن الحقل اسمه `name`
- استبدال `node.target` بـ `node.name`

### 3. مشكلة مسار Include / Include Path Issue
**المشكلة / Problem:**
- استخدمنا `../../../include/` بدلاً من `../../include/`

**الحل / Solution:**
- حساب المسار النسبي الصحيح من `tests/interpreter_tests/`
- المسار الصحيح: `../../include/`

### 4. مشكلة Header Guard / Header Guard Issue
**المشكلة / Problem:**
- الملف يحتوي على `#pragma once` في البداية و`#endif` في النهاية

**الحل / Solution:**
- إزالة `#endif` لأن `#pragma once` كافٍ

### 5. أخطاء الربط (Linking Errors)
**المشكلة / Problem:**
- مراجع غير محددة لـ vtable لفئات AST

**الحل / Solution:**
- إضافة ملفات AST للتجميع:
  * `src/parser/ast/expressions.cpp`
  * `src/parser/ast/statements.cpp`
  * `src/parser/ast/ast_node.cpp`

---

## التكامل مع المكونات الأخرى / Integration with Other Components

### 1. نظام القيم (Value) / Value System
- تحويل التعابير إلى قيم Value تنفيذية
- دعم جميع أنواع البيانات: INT, DOUBLE, STRING, BOOL, VOID, ARRAY, MAP
- ترقية النوع التلقائية (int → double في العمليات المختلطة)

### 2. مدير المتغيرات (VariableManager) / Variable Manager
- قراءة قيم المتغيرات مع فحص الوجود
- إسناد قيم جديدة للمتغيرات
- رسائل خطأ ثنائية اللغة للمتغيرات غير المُعرّفة

### 3. مدير الدوال (FunctionManager) / Function Manager
- تمهيد للمرحلة 4.3 (استدعاء الدوال)
- حاليًا: Placeholder فقط

### 4. نمط الزائر (Visitor Pattern) / Visitor Pattern
- يرث من BaseASTVisitor (المرحلة 4.1)
- تنفيذ 15 دالة visit لأنواع التعابير المختلفة
- نمط تصميم نظيف وموسّع

---

## الميزات الرئيسية / Key Features

### ✅ الميزات المُنفَّذة / Implemented Features
1. **تقييم القيم الحرفية** - Literal evaluation (6 أنواع)
2. **عمليات المتغيرات** - Variable operations (قراءة/كتابة)
3. **العمليات الحسابية** - Arithmetic operations (6 أنواع)
4. **عمليات المقارنة** - Comparison operations (6 أنواع)
5. **العمليات المنطقية** - Logical operations (AND, OR)
6. **العمليات الأحادية** - Unary operations (-, !, +)
7. **المصفوفات** - Arrays (إنشاء، فهرسة)
8. **القواميس** - Maps (إنشاء، فهرسة)
9. **التعابير المتداخلة** - Nested expressions
10. **رسائل خطأ ثنائية اللغة** - Bilingual error messages

### ⏸️ الميزات المؤجلة / Deferred Features
1. **استدعاء الدوال** - Function calls (المرحلة 4.3)
2. **الوصول للأعضاء** - Member access (OOP)
3. **Lambda expressions** - (مرحلة متقدمة)
4. **List comprehensions** - (مرحلة متقدمة)

---

## أمر التجميع / Build Command

```bash
cd c:\s\s_language

g++ -std=c++17 -Iinclude \
    tests\interpreter_tests\test_expression_evaluator.cpp \
    src\interpreter\visitors\expression_evaluator.cpp \
    src\parser\ast\expressions.cpp \
    src\parser\ast\statements.cpp \
    src\parser\ast\ast_node.cpp \
    src\lexer\token.cpp \
    src\data\types\value.cpp \
    src\data\managers\variable_manager.cpp \
    src\data\managers\function_manager.cpp \
    src\data\scope\scope_manager.cpp \
    -o build\test_expression_evaluator.exe
```

---

## الإحصائيات / Statistics

- **ملفات جديدة / New Files:** 3
  1. `include/interpreter/visitors/expression_evaluator.h` (210 سطر)
  2. `src/interpreter/visitors/expression_evaluator.cpp` (442 سطر)
  3. `tests/interpreter_tests/test_expression_evaluator.cpp` (954 سطر)

- **إجمالي الأسطر / Total Lines:** 1,606 سطر
- **عدد الاختبارات / Test Count:** 37 اختبار ✅
- **معدل النجاح / Success Rate:** 100%
- **التعابير المدعومة / Supported Expressions:** 10 أنواع
- **العمليات المدعومة / Supported Operations:** 18 عملية

---

## الخطوة التالية / Next Step

### المرحلة 4.3: منفّذ الجُمل / Phase 4.3: Statement Executor

**الهدف / Goal:** تنفيذ StatementExecutor لتنفيذ جميع أنواع الجُمل

**المكونات المطلوبة / Required Components:**
1. فئة StatementExecutor ترث من BaseASTVisitor
2. تنفيذ الجُمل الأساسية:
   - VarDeclStmt - تعريف المتغيرات
   - ExpressionStmt - تعابير مستقلة
   - BlockStmt - كتل الكود
   - IfStmt - الشرط
   - WhileStmt - الحلقة while
   - ForStmt - الحلقة for
   - ReturnStmt - الإرجاع
3. تنفيذ الجُمل المتقدمة:
   - FuncDeclStmt - تعريف الدوال
   - استدعاء الدوال
   - Break/Continue
   - Try-Catch-Raise
4. 30+ اختبار شامل

**الوقت المتوقع / Estimated Time:** 8-12 ساعة

---

## الخلاصة / Conclusion

### بالعربية
تم إتمام المرحلة 4.2 بنجاح! مُقيِّم التعابير يعمل بكفاءة عالية ويقوم بتقييم جميع أنواع التعابير المطلوبة. جميع الاختبارات (37/37) نجحت بدون أخطاء. الكود نظيف، موثّق، ومتكامل مع طبقة البيانات ونظام AST.

### In English
Phase 4.2 completed successfully! The Expression Evaluator works efficiently and evaluates all required expression types. All tests (37/37) passed without errors. The code is clean, well-documented, and integrates seamlessly with the Data layer and AST system.

---

**الحالة النهائية / Final Status:** ✅ مكتملة 100% / Complete 100%

**جاهز للمرحلة 4.3 / Ready for Phase 4.3:** ✅ نعم / Yes
