# Phase 8: Exception Handling - خطة التنفيذ
# Phase 8: Exception Handling - Implementation Plan

**التاريخ / Date**: 2025-11-27  
**الحالة / Status**: ⏳ **قيد التنفيذ / In Progress 0%**

---

## 📋 نظرة عامة / Overview

Phase 8 focuses on implementing a comprehensive **Exception Handling** system for Sad Language, enabling developers to handle errors gracefully using `حاول/اصطد/أخيراً/ارمِ` (try/catch/finally/throw) syntax.

### 🎯 Goals

1. ✅ **Parser** (Already Done): Try-catch-finally-throw parsing complete
2. ⏳ **AST Nodes**: Verify TryStmt, CatchClause, ThrowStmt, FinallyBlock exist
3. ⏳ **Exception Runtime**: Exception propagation, stack unwinding, catch matching
4. ⏳ **Built-in Exceptions**: Standard exception types (RuntimeError, TypeError, etc.)
5. ⏳ **Testing**: Comprehensive exception handling tests

---

## 📊 الحالة الحالية / Current Status

### ✅ Completed (Parser)
```cpp
// src/parser/parser_core_impl.cpp (lines 1083-1133)
StmtPtr ParserCore::parseTryStmt() {
    // Parse try block
    auto tryBlock = parseBlockStmt();
    
    // Parse catch clauses
    std::vector<CatchClause> catchClauses;
    while (match(TT::KEYWORD_CATCH)) {
        Token exceptionVar = consume(TT::IDENTIFIER);
        auto catchBody = parseBlockStmt();
        catchClauses.push_back(CatchClause(...));
    }
    
    // Parse optional finally block
    StmtPtr finallyBlock = nullptr;
    if (match(TT::KEYWORD_FINALLY)) {
        finallyBlock = parseBlockStmt();
    }
    
    return std::make_unique<TryStmt>(...);
}

// Throw statement parser also exists
StmtPtr ParserCore::parseRaiseStmt() { ... }
```

**Status**: ✅ Parser complete, grammar working

### ⏳ To Be Implemented (Runtime)

**Required Components**:
1. Exception class hierarchy
2. Exception propagation mechanism
3. Stack unwinding logic
4. Catch clause matching
5. Finally block execution guarantee

---

## 🏗️ المراحل الفرعية / Sub-Phases

### Phase 8.1: Exception Infrastructure (⏳ 0%)

**Goal**: Build core exception system

#### 8.1.1: Exception Base Class
```cpp
// include/interpreter/exception.h
class SadException {
protected:
    std::string message_;
    std::string type_;
    std::vector<std::string> stackTrace_;
    Position position_;
    
public:
    SadException(const std::string& msg, const std::string& type);
    virtual ~SadException() = default;
    
    std::string getMessage() const { return message_; }
    std::string getType() const { return type_; }
    std::vector<std::string> getStackTrace() const { return stackTrace_; }
    Position getPosition() const { return position_; }
    
    void addStackFrame(const std::string& frame);
    virtual std::string toString() const;
};
```

#### 8.1.2: Standard Exception Types
```cpp
// Built-in exception types
class RuntimeError : public SadException {
public:
    RuntimeError(const std::string& msg) 
        : SadException(msg, "RuntimeError") {}
};

class TypeError : public SadException {
public:
    TypeError(const std::string& msg) 
        : SadException(msg, "TypeError") {}
};

class ValueError : public SadException {
public:
    ValueError(const std::string& msg) 
        : SadException(msg, "ValueError") {}
};

class DivisionByZeroError : public SadException {
public:
    DivisionByZeroError(const std::string& msg) 
        : SadException(msg, "DivisionByZeroError") {}
};

class IndexOutOfRangeError : public SadException {
public:
    IndexOutOfRangeError(const std::string& msg) 
        : SadException(msg, "IndexOutOfRangeError") {}
};

class NullReferenceError : public SadException {
public:
    NullReferenceError(const std::string& msg) 
        : SadException(msg, "NullReferenceError") {}
};
```

**Files to Create**:
- ✅ `include/interpreter/exception.h`
- ✅ `src/interpreter/exception.cpp`

**Tests**:
- Exception creation
- Message retrieval
- Stack trace formatting

**Estimated Time**: 2 hours

---

### Phase 8.2: Exception Propagation (⏳ 0%)

**Goal**: Implement exception throwing and catching

#### 8.2.1: Throw Statement Execution
```cpp
// src/interpreter/visitors/statement_executor.cpp
void StatementExecutor::visitThrowStmt(const AST::ThrowStmt& stmt) {
    // Evaluate exception expression
    Value exceptionValue = evaluateExpression(stmt.getExpression());
    
    // Get exception message
    std::string message = exceptionValue.toString();
    
    // Create and throw exception
    throw RuntimeError(message);
}
```

#### 8.2.2: Try-Catch Execution
```cpp
void StatementExecutor::visitTryStmt(const AST::TryStmt& stmt) {
    try {
        // Execute try block
        execute(stmt.getTryBlock());
    } 
    catch (const SadException& e) {
        // Find matching catch clause
        bool caught = false;
        
        for (const auto& catchClause : stmt.getCatchClauses()) {
            // Match exception type (for now, catch all)
            if (true) {  // TODO: Type matching
                // Bind exception to variable
                variableManager_.define(catchClause.getVarName(), 
                    Value(e.getMessage(), Data::DataType::STRING));
                
                // Execute catch block
                execute(catchClause.getBody());
                caught = true;
                break;
            }
        }
        
        // Re-throw if no catch matched
        if (!caught) {
            throw;
        }
    }
    
    // Always execute finally block
    if (stmt.getFinallyBlock()) {
        execute(stmt.getFinallyBlock());
    }
}
```

**Files to Modify**:
- `src/interpreter/visitors/statement_executor.cpp`
- `include/interpreter/visitors/statement_executor.h`

**Tests**:
- Basic try-catch
- Multiple catch clauses
- Finally block execution
- Nested try-catch
- Re-throwing exceptions

**Estimated Time**: 3 hours

---

### Phase 8.3: Division by Zero Example (⏳ 0%)

**Goal**: Automatically throw exceptions for runtime errors

#### 8.3.1: Division Operator Enhancement
```cpp
// src/interpreter/visitors/expression_evaluator.cpp
Value ExpressionEvaluator::evaluateBinaryOp(const BinaryOpExpr& expr) {
    Value left = evaluate(expr.getLeft());
    Value right = evaluate(expr.getRight());
    
    if (expr.getOperator() == TokenType::OPERATOR_DIV) {
        // Check for division by zero
        if (right.getType() == DataType::INTEGER && right.getIntValue() == 0) {
            throw DivisionByZeroError(
                "(AR) لا يمكن القسمة على صفر / (EN) Cannot divide by zero"
            );
        }
        if (right.getType() == DataType::FLOAT && right.getFloatValue() == 0.0) {
            throw DivisionByZeroError(
                "(AR) لا يمكن القسمة على صفر / (EN) Cannot divide by zero"
            );
        }
    }
    
    // ... rest of operation
}
```

#### 8.3.2: Array Index Bounds Checking
```cpp
Value ExpressionEvaluator::evaluateIndexAccess(const IndexExpr& expr) {
    Value array = evaluate(expr.getArray());
    Value index = evaluate(expr.getIndex());
    
    if (array.getType() != DataType::ARRAY) {
        throw TypeError("(AR) القيمة ليست مصفوفة / (EN) Value is not an array");
    }
    
    int idx = index.getIntValue();
    const auto& arrayData = array.getArrayValue();
    
    if (idx < 0 || idx >= arrayData.size()) {
        throw IndexOutOfRangeError(
            "(AR) الفهرس خارج النطاق / (EN) Index out of range: " + 
            std::to_string(idx)
        );
    }
    
    return arrayData[idx];
}
```

**Files to Modify**:
- `src/interpreter/visitors/expression_evaluator.cpp`

**Tests**:
- Division by zero (integer)
- Division by zero (float)
- Array index out of bounds
- Null reference access

**Estimated Time**: 2 hours

---

### Phase 8.4: User-Defined Exceptions (⏳ 0%)

**Goal**: Allow users to create custom exception classes

#### 8.4.1: Exception Class Syntax
```sad
# Define custom exception
صنف خطأ_حسابي
    خاص نص الرسالة
    
    باني(نص رسالة)
        الرسالة = رسالة
    نهاية
    
    عام دالة نص احصل_رسالة()
        ارجع الرسالة
    نهاية
نهاية

# Use custom exception
دالة قسمة(رقم أ، رقم ب)
    إذا ب == 0
        ارمِ جديد خطأ_حسابي("لا يمكن القسمة على صفر")
    نهاية
    ارجع أ / ب
نهاية

# Handle custom exception
حاول
    رقم نتيجة = قسمة(10، 0)
اصطد خطأ
    اطبع("حدث خطأ: "، خطأ)
نهاية
```

**Implementation**: Already supported via object system!

**Tests**:
- Create custom exception class
- Throw custom exception object
- Catch custom exception
- Exception inheritance

**Estimated Time**: 1 hour

---

### Phase 8.5: Stack Traces (⏳ 0%)

**Goal**: Provide detailed stack traces for debugging

#### 8.5.1: Stack Frame Tracking
```cpp
// include/interpreter/call_stack.h
class CallStack {
private:
    std::vector<StackFrame> frames_;
    
public:
    void push(const std::string& functionName, const Position& pos);
    void pop();
    
    std::vector<std::string> getStackTrace() const;
    size_t depth() const { return frames_.size(); }
};

struct StackFrame {
    std::string functionName;
    Position position;
    std::string toString() const {
        return "at " + functionName + " (" + position.toString() + ")";
    }
};
```

#### 8.5.2: Enhanced Exception Messages
```cpp
void throwWithStackTrace(const SadException& e, const CallStack& stack) {
    SadException copy = e;
    
    // Add stack frames to exception
    for (const auto& frame : stack.getStackTrace()) {
        copy.addStackFrame(frame);
    }
    
    throw copy;
}
```

**Output Example**:
```
RuntimeError: لا يمكن القسمة على صفر / Cannot divide by zero
  at قسمة (test.s:5:12)
  at حساب (test.s:10:20)
  at main (test.s:15:5)
```

**Files to Create**:
- `include/interpreter/call_stack.h`
- `src/interpreter/call_stack.cpp`

**Tests**:
- Stack frame push/pop
- Stack trace generation
- Multi-level call traces

**Estimated Time**: 2 hours

---

## 🧪 اختبارات شاملة / Comprehensive Tests

### Test 1: Basic Try-Catch
```sad
# test_basic_exception.s
حاول
    اطبع("محاولة العملية...")
    متغير نتيجة = 10 / 0  # Division by zero!
    اطبع("هذا لن يُطبع")
اصطد خطأ
    اطبع("تم اصطياد خطأ: "، خطأ)
نهاية
اطبع("البرنامج استمر")
```

**Expected Output**:
```
محاولة العملية...
تم اصطياد خطأ: لا يمكن القسمة على صفر
البرنامج استمر
```

---

### Test 2: Finally Block
```sad
# test_finally.s
متغير تم_التنظيف = خطأ

حاول
    اطبع("محاولة العملية...")
    ارمِ "خطأ اختباري"
اصطد خطأ
    اطبع("تم اصطياد: "، خطأ)
أخيراً
    اطبع("تنظيف الموارد...")
    تم_التنظيف = صحيح
نهاية

اطبع("تم التنظيف؟ "، تم_التنظيف)
```

**Expected Output**:
```
محاولة العملية...
تم اصطياد: خطأ اختباري
تنظيف الموارد...
تم التنظيف؟ true
```

---

### Test 3: Nested Try-Catch
```sad
# test_nested_exception.s
حاول
    اطبع("المستوى الخارجي")
    
    حاول
        اطبع("المستوى الداخلي")
        ارمِ "خطأ داخلي"
    اصطد خطأ_داخلي
        اطبع("اصطياد داخلي: "، خطأ_داخلي)
        ارمِ "خطأ خارجي"  # Re-throw new exception
    نهاية
    
اصطد خطأ_خارجي
    اطبع("اصطياد خارجي: "، خطأ_خارجي)
نهاية
```

**Expected Output**:
```
المستوى الخارجي
المستوى الداخلي
اصطياد داخلي: خطأ داخلي
اصطياد خارجي: خطأ خارجي
```

---

### Test 4: Custom Exception Class
```sad
# test_custom_exception.s
صنف خطأ_مخصص
    خاص نص الرسالة
    خاص رقم الكود
    
    باني(نص رسالة، رقم كود)
        الرسالة = رسالة
        الكود = كود
    نهاية
    
    عام دالة نص نص()
        ارجع "خطأ [" + نص(الكود) + "]: " + الرسالة
    نهاية
نهاية

دالة عملية_خطرة()
    ارمِ جديد خطأ_مخصص("عملية فشلت"، 404)
نهاية

حاول
    عملية_خطرة()
اصطد خطأ
    اطبع("تم الاصطياد: "، خطأ)
نهاية
```

**Expected Output**:
```
تم الاصطياد: خطأ [404]: عملية فشلت
```

---

### Test 5: Array Index Exception
```sad
# test_array_exception.s
مصفوفة أرقام = [1، 2، 3، 4، 5]

حاول
    اطبع("الوصول للفهرس 2: "، أرقام[2])  # OK
    اطبع("الوصول للفهرس 10: "، أرقام[10])  # Out of bounds!
اصطد خطأ
    اطبع("خطأ: "، خطأ)
نهاية
```

**Expected Output**:
```
الوصول للفهرس 2: 3
خطأ: الفهرس خارج النطاق: 10
```

---

## 📁 الملفات المطلوبة / Required Files

### New Files to Create:
1. ✅ `include/interpreter/exception.h`
2. ✅ `src/interpreter/exception.cpp`
3. ✅ `include/interpreter/call_stack.h`
4. ✅ `src/interpreter/call_stack.cpp`
5. ✅ `tests/exception_tests/test_basic_exception.s`
6. ✅ `tests/exception_tests/test_finally.s`
7. ✅ `tests/exception_tests/test_nested.s`
8. ✅ `tests/exception_tests/test_custom_exception.s`
9. ✅ `tests/exception_tests/test_array_exception.s`

### Files to Modify:
1. ✅ `src/interpreter/visitors/statement_executor.cpp`
2. ✅ `src/interpreter/visitors/expression_evaluator.cpp`
3. ✅ `include/interpreter/visitors/statement_executor.h`
4. ✅ `src/interpreter/core/interpreter.cpp` (integrate call stack)
5. ✅ `CMakeLists.txt` (add new source files)

---

## 📊 ملخص التقدم / Progress Summary

| Sub-Phase | Feature | Status | Tests | Priority |
|-----------|---------|--------|-------|----------|
| 8.1 | Exception Infrastructure | ⏳ 0% | 0/3 | P0 |
| 8.2 | Try-Catch-Finally Execution | ⏳ 0% | 0/5 | P0 |
| 8.3 | Auto Exception Throwing | ⏳ 0% | 0/4 | P0 |
| 8.4 | User-Defined Exceptions | ⏳ 0% | 0/4 | P1 |
| 8.5 | Stack Traces | ⏳ 0% | 0/3 | P2 |

**Overall Phase 8 Progress**: ⏳ **0%** (0/19 tests)

---

## ⏱️ الجدول الزمني / Timeline

**Total Estimated Time**: 10-12 hours

### Day 1 (4-5 hours):
- ✅ Phase 8.1: Exception infrastructure (2 hours)
- ✅ Phase 8.2: Basic try-catch (3 hours)

### Day 2 (3-4 hours):
- ✅ Phase 8.3: Auto exceptions (2 hours)
- ✅ Phase 8.4: Custom exceptions (1 hour)
- ✅ Testing (1 hour)

### Day 3 (3 hours):
- ✅ Phase 8.5: Stack traces (2 hours)
- ✅ Final testing and documentation (1 hour)

---

## ✅ تعريف النجاح / Success Criteria

Phase 8 is considered **100% complete** when:

1. ✅ All exception types defined and working
2. ✅ Try-catch-finally execution correct
3. ✅ Finally blocks always execute (even with return/break)
4. ✅ Division by zero throws exception automatically
5. ✅ Array bounds checking throws exception
6. ✅ Custom exception classes work
7. ✅ Nested try-catch works correctly
8. ✅ Stack traces show call hierarchy
9. ✅ All 19 tests passing (100%)
10. ✅ Documentation complete with examples

---

## 🚀 الخطوة التالية / Next Steps

**Immediate Action**: Start Phase 8.1

1. Create `exception.h` and `exception.cpp`
2. Define `SadException` base class
3. Create standard exception types
4. Write basic tests
5. Move to Phase 8.2 (try-catch execution)

**Command to Start**:
```bash
# Create exception infrastructure
mkdir -p include/interpreter
mkdir -p src/interpreter
mkdir -p tests/exception_tests
```

---

**Document Version**: 1.0  
**Last Updated**: 2025-11-27  
**Status**: Ready to begin implementation  
**Priority**: P0 - Critical for production use

**Let's complete Phase 8 to 100%! 🚀**
