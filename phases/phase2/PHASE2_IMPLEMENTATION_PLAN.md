# Phase 2: Python-Style Features Implementation Plan
# خطة تنفيذ المرحلة الثانية: ميزات Python

**تاريخ البدء / Start Date:** November 19, 2025  
**الحالة / Status:** ⏳ IN PROGRESS  
**الأولوية / Priority:** 🟡 HIGH

---

## 🎯 الأهداف / Objectives

تنفيذ 5 ميزات رئيسية من Python لتحسين قدرات اللغة:

1. ✅ **Lambda Expressions** - دعم كامل للـ lambda مع arrow syntax
2. **List Comprehensions** - تحسين وتكامل
3. **Dict Comprehensions** - تحسين وتكامل  
4. **Decorators** - دعم جديد (@decorator)
5. **Generators & Yield** - دعم جديد

---

## 📋 Task 1: Lambda Expression Integration ⏳ IN PROGRESS

### الحالة الحالية / Current State

✅ **موجود:** `parseLambda()` function  
❌ **مفقود:** Integration in `parsePrimary()`  
❌ **مفقود:** Arrow syntax support (=>)

### الخطة التفصيلية / Detailed Plan

#### Step 1.1: Add Lambda Detection in parsePrimary() ⏳

**الملف / File:** `src/parser/parser_core_impl.cpp`  
**الموقع / Location:** Inside `parsePrimary()` after literals

**التعديل المطلوب / Required Modification:**

```cpp
ExprPtr ParserCore::parsePrimary() {
    // ... existing literal handling ...
    
    // ✅ NEW: Lambda expression detection
    // Syntax: lambda x: x + 1  OR  (x) => x + 1
    if (match(TT::KEYWORD_LAMBDA)) {
        return parseLambda();
    }
    
    // ✅ NEW: Arrow function: (x, y) => x + y
    if (check(TT::PAREN_LEFT)) {
        // Lookahead to detect arrow syntax
        if (isArrowFunction()) {
            return parseArrowFunction();
        }
    }
    
    // ... rest of existing code ...
}
```

#### Step 1.2: Implement Arrow Function Support ⏳

**دالة جديدة / New Function:** `parseArrowFunction()`

```cpp
/**
 * @brief (AR) يحلل arrow function: (x, y) => x + y.
 *        (EN) Parses arrow function: (x, y) => x + y.
 * 
 * Grammar:
 *   arrow_function ::= '(' [param_list] ')' '=>' expression
 *                    | identifier '=>' expression
 * 
 * Examples:
 *   - (x, y) => x + y
 *   - x => x * 2
 *   - () => 42
 */
ExprPtr ParserCore::parseArrowFunction() {
    std::vector<Parameter> params;
    
    // Case 1: Single parameter without parentheses (x => ...)
    if (check(TT::IDENTIFIER)) {
        Token param = advance();
        params.emplace_back(param.getValue(), Data::DataType::UNKNOWN, nullptr);
    }
    // Case 2: Multiple parameters with parentheses
    else if (match(TT::PAREN_LEFT)) {
        params = parseTypedParameterList();
        consume(TT::PAREN_RIGHT, 
            "(AR) توقع ')' بعد معاملات arrow function. "
            "(EN) Expected ')' after arrow function parameters.");
    }
    
    // Arrow operator
    consume(TT::ARROW, 
        "(AR) توقع '=>' في arrow function. "
        "(EN) Expected '=>' in arrow function.");
    
    // Body expression
    auto body = parseExpression();
    
    return std::make_unique<LambdaExpr>(
        std::move(params),
        std::move(body),
        previous().getPosition()
    );
}
```

#### Step 1.3: Add Lookahead Helper ⏳

**دالة مساعدة / Helper Function:** `isArrowFunction()`

```cpp
/**
 * @brief (AR) يتحقق إذا كان التسلسل الحالي arrow function.
 *        (EN) Checks if current sequence is arrow function.
 * 
 * Lookahead pattern: ( ... ) =>  OR  identifier =>
 */
bool ParserCore::isArrowFunction() {
    // Case 1: Single identifier followed by =>
    if (check(TT::IDENTIFIER) && checkNext(TT::ARROW)) {
        return true;
    }
    
    // Case 2: ( ... ) =>
    if (!check(TT::PAREN_LEFT)) {
        return false;
    }
    
    size_t saved = current_;
    int depth = 0;
    
    // Scan for matching )
    while (!isAtEnd()) {
        if (match(TT::PAREN_LEFT)) depth++;
        if (match(TT::PAREN_RIGHT)) {
            depth--;
            if (depth == 0) {
                bool isArrow = check(TT::ARROW);
                current_ = saved;  // Restore position
                return isArrow;
            }
        }
        advance();
    }
    
    current_ = saved;  // Restore position
    return false;
}
```

#### Step 1.4: Update parseLambda() for Python Syntax ✅

**التعديل المطلوب / Required Update:**

Current `parseLambda()` uses Python-style `lambda x: ...`.  
Keep it as is, but ensure it's called from `parsePrimary()`.

#### Step 1.5: اختبارات / Tests

**ملف الاختبار / Test File:** `tests/parser_tests/test_lambda.cpp`

```cpp
// Test 1: Python-style lambda
"lambda x: x * 2"
"لامبدا س: س * 2"

// Test 2: Arrow function (single param)
"x => x * 2"

// Test 3: Arrow function (multiple params)
"(x, y) => x + y"

// Test 4: Arrow function (no params)
"() => 42"

// Test 5: Typed arrow function
"(x: int, y: int) => x + y"
```

---

## 📋 Task 2: List Comprehensions ⏳ PENDING

### الحالة الحالية / Current State

✅ **موجود:** `parseListComprehension()` function  
⚠️ **يحتاج تحسين:** Integration and testing

### الخطة المختصرة / Brief Plan

1. Verify `parseListComprehension()` is called correctly
2. Test with various patterns:
   - `[x for x in list]`
   - `[x * 2 for x in list]`
   - `[x for x in list if x > 0]`
3. Add Arabic syntax support

---

## 📋 Task 3: Dict Comprehensions ⏳ PENDING

Similar to List Comprehensions but for dictionaries:
- `{k: v for k, v in items}`
- `{x: x**2 for x in range(10)}`

---

## 📋 Task 4: Decorators ⏳ PENDING

**جديد تماماً / Completely New**

### المطلوب / Requirements

```python
@decorator
@decorator(arg1, arg2)
function myFunction() {
    // ...
}

@مُزخرِف
@مُزخرِف(معامل)
دالة دالتي() {
    // ...
}
```

### الخطوات / Steps

1. إنشاء `DecoratorExpr` AST node
2. تنفيذ `parseDecorator()`
3. تحديث `parseFunctionDecl()` للتعامل مع decorators
4. اختبارات

---

## 📋 Task 5: Generators & Yield ⏳ PENDING

**جديد تماماً / Completely New**

### المطلوب / Requirements

```python
function* generator() {
    yield 1;
    yield 2;
    yield 3;
}

دالة* مولّد() {
    اعطِ 1؛
    اعطِ 2؛
    اعطِ 3؛
}
```

### الخطوات / Steps

1. إنشاء `YieldStmt` AST node
2. تنفيذ `parseYieldStmt()`
3. إضافة `isGenerator` flag to `FunctionDecl`
4. اختبارات

---

## 📊 Timeline / الجدول الزمني

| Task | Estimated Time | Priority | Status |
|------|---------------|----------|--------|
| Lambda Integration | 2-3 hours | 🔴 CRITICAL | ⏳ IN PROGRESS |
| List Comprehensions | 1-2 hours | 🟡 HIGH | ⏳ PENDING |
| Dict Comprehensions | 1-2 hours | 🟡 HIGH | ⏳ PENDING |
| Decorators | 2-3 hours | 🟡 HIGH | ⏳ PENDING |
| Generators & Yield | 2-3 hours | 🟢 MEDIUM | ⏳ PENDING |
| **Total** | **8-13 hours** | | |

---

## ✅ Success Criteria / معايير النجاح

1. ✅ All features implemented and tested
2. ✅ 100% test pass rate
3. ✅ Bilingual support (Arabic + English)
4. ✅ Full Doxygen documentation
5. ✅ Backward compatibility maintained
6. ✅ No compilation errors or warnings

---

**Next Action:** Start implementing Lambda Integration (Step 1.1)

**الإجراء التالي:** بدء تنفيذ دمج Lambda (الخطوة 1.1)
