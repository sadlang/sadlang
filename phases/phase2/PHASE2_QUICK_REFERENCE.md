# Phase 2 Quick Reference Guide
# دليل المرجع السريع للمرحلة 2

**Version:** 1.0  
**Status:** ✅ Complete  
**Languages:** English + العربية

---

## 🚀 Quick Links

| Feature | English Syntax | Arabic Syntax | Tests | Report |
|---------|---------------|---------------|-------|--------|
| **Lambda** | `lambda x: x * 2` | `لامدا س: س * 2` | 10/10 ✅ | [Report](TASK1_LAMBDA_REPORT.md) |
| **List Comp** | `[x*2 for x in list]` | `[س*2 لكل س في قائمة]` | 6/6 ✅ | [Report](TASK2_LIST_COMP_REPORT.md) |
| **Dict Comp** | `{k:v for k,v in dict}` | `{م:ق لكل م،ق في قاموس}` | 8/8 ✅ | [Report](TASK3_DICT_COMP_REPORT.md) |
| **Decorators** | `@decorator` | `@مُزخرِف` | 14/14 ✅ | [Report](TASK4_DECORATORS_REPORT.md) |
| **Generators** | `yield value` | `اعطِ قيمة` | 13/13 ✅ | [Report](TASK5_GENERATORS_COMPLETION_REPORT.md) |

---

## 1. Lambda Expressions

### Basic Syntax
```python
# English
lambda param1, param2: expression

# Arabic
لامدا معامل1، معامل2: تعبير
```

### Examples
```python
# Simple lambda
add = lambda x, y: x + y
double = lambda x: x * 2

# No parameters
get_pi = lambda: 3.14159

# In higher-order functions
map(lambda x: x ** 2, numbers)
filter(lambda x: x > 0, values)

# Arabic
جمع = لامدا س، ص: س + ص
مضاعفة = لامدا س: س * 2
```

### AST Node
```cpp
class LambdaExpr : public Expression {
    std::vector<std::string> params;  // Parameter names
    ExprPtr body;                     // Lambda body expression
};
```

### Test File
```
tests/parser_tests/test_lambda_simple.exe
```

---

## 2. List Comprehensions

### Basic Syntax
```python
# English
[expression for variable in iterable]
[expression for variable in iterable if condition]

# Arabic
[تعبير لكل متغير في مكرر]
[تعبير لكل متغير في مكرر إذا شرط]
```

### Examples
```python
# Simple comprehension
squares = [x ** 2 for x in range(10)]

# With filter
evens = [x for x in numbers if x % 2 == 0]

# Multiple sources
pairs = [x + y for x in list1 for y in list2]

# Nested comprehension
matrix = [[y for y in range(5)] for x in range(5)]

# Arabic
مربعات = [س ** 2 لكل س في نطاق(10)]
أزواج = [س لكل س في أعداد إذا س % 2 == 0]
```

### AST Node
```cpp
class ListComprehensionExpr : public Expression {
    ExprPtr element;                      // Element expression
    std::string variable;                 // Loop variable
    ExprPtr iterable;                     // Iterable expression
    ExprPtr condition;                    // Optional filter
};
```

### Test File
```
tests/parser_tests/test_comprehensions.exe
```

---

## 3. Dict Comprehensions

### Basic Syntax
```python
# English
{key: value for variable in iterable}
{key: value for variable in iterable if condition}

# Arabic
{مفتاح: قيمة لكل متغير في مكرر}
{مفتاح: قيمة لكل متغير في مكرر إذا شرط}
```

### Examples
```python
# Simple dict comprehension
scores = {name: 0 for name in students}

# With transformation
uppercase = {k.upper(): v for k, v in dict.items()}

# With filter
positive = {k: v for k, v in numbers if v > 0}

# Multiple sources
product = {x: y for x in keys for y in values}

# Arabic
درجات = {اسم: 0 لكل اسم في طلاب}
كبير = {م.كبير(): ق لكل م، ق في قاموس.عناصر()}
```

### AST Node
```cpp
class DictComprehensionExpr : public Expression {
    ExprPtr keyExpr;                      // Key expression
    ExprPtr valueExpr;                    // Value expression
    std::string variable;                 // Loop variable
    ExprPtr iterable;                     // Iterable expression
    ExprPtr condition;                    // Optional filter
};
```

### Test Files
```
tests/parser_tests/test_dict_comp_full.exe
tests/parser_tests/test_dict_simple.exe
```

---

## 4. Decorators

### Basic Syntax
```python
# English
@decorator
function name() {
    // body
}

# Arabic
@مُزخرِف
دالة اسم() {
    // الجسم
}
```

### Examples
```python
# Simple decorator
@staticmethod
function helper() {
    return 42;
}

# Multiple decorators (applied bottom-up)
@decorator1
@decorator2
@decorator3
function process() {
    // ...
}

# Decorator with arguments
@route("/api/users")
@auth_required
function get_users() {
    // ...
}

# Arabic
@ثابت
دالة مساعد() {
    أرجع 42؛
}

@طريق("/api/users")
@مصادقة_مطلوبة
دالة احصل_على_مستخدمين() {
    // ...
}
```

### AST Node
```cpp
class DecoratorExpr : public Expression {
    std::string name;                     // Decorator name
    std::vector<ExprPtr> arguments;       // Optional arguments
    bool hasArguments;                    // Has argument list?
};

class FunctionDecl : public Declaration {
    std::vector<ExprPtr> decorators;      // Applied decorators
    // ... other fields
};
```

### Test Files
```
tests/parser_tests/test_decorator_token.exe
tests/parser_tests/test_decorator_parser.exe
tests/parser_tests/test_decorator_integration.exe
```

---

## 5. Generators & Yield

### Basic Syntax
```python
# English
yield expression;
yield from iterable;
yield;  // yields None

# Arabic
اعطِ تعبير؛
اعطِ من مكرر؛
اعطِ؛  // يعطي لاشيء
```

### Examples
```python
# Simple generator
function count_up_to(n) {
    for (i in range(n)) {
        yield i;
    }
}

# Fibonacci generator
function fibonacci() {
    a = 0;
    b = 1;
    while (true) {
        yield a;
        temp = a;
        a = b;
        b = temp + b;
    }
}

# Generator delegation
function all_numbers() {
    yield from range(0, 10);
    yield from range(100, 110);
}

# Yield without value
function ticker() {
    while (true) {
        yield;  // Just signal, no value
    }
}

# Arabic
دالة عد_حتى(ن) {
    لكل (س في نطاق(ن)) {
        اعطِ س؛
    }
}

دالة فيبوناتشي() {
    أ = 0؛
    ب = 1؛
    بينما (صحيح) {
        اعطِ أ؛
        مؤقت = أ؛
        أ = ب؛
        ب = مؤقت + ب؛
    }
}
```

### AST Node
```cpp
class YieldStmt : public Statement {
    ExprPtr value;                        // Yielded value (optional)
    bool isYieldFrom;                     // Is 'yield from'?
};
```

### Test Files
```
tests/lexer_tests/test_yield_token.exe         (5 tests)
tests/parser_tests/test_yield_integration.exe  (8 tests)
```

---

## 📊 Build Commands

### Individual Tests
```bash
# Lambda tests
g++ tests/parser_tests/test_lambda_simple.cpp ... -o build/test_lambda_simple.exe

# List comprehension tests
g++ tests/parser_tests/test_comprehensions.cpp ... -o build/test_comprehensions.exe

# Dict comprehension tests
g++ tests/parser_tests/test_dict_comp_full.cpp ... -o build/test_dict_comp_full.exe

# Decorator tests
g++ tests/parser_tests/test_decorator_integration.cpp ... -o build/test_decorator_integration.exe

# Generator tests
g++ tests/parser_tests/test_yield_integration.cpp ... -o build/test_yield_integration.exe
```

### Run All Phase 2 Tests
```bash
cd s_language
./build/test_lambda_simple.exe
./build/test_comprehensions.exe
./build/test_dict_comp_full.exe
./build/test_decorator_integration.exe
./build/test_yield_integration.exe
```

### Complete Build Script
```powershell
# PowerShell script
cd c:\s\s_language

# Compile all features
$sources = "src/lexer/*.cpp src/parser/*.cpp src/parser/ast/*.cpp src/errors/*.cpp src/utils/*.cpp"
$flags = "-Iinclude -std=c++17 -g"

# Lambda
g++ tests/parser_tests/test_lambda_simple.cpp $sources -o build/test_lambda_simple.exe $flags

# List Comp
g++ tests/parser_tests/test_comprehensions.cpp $sources -o build/test_comprehensions.exe $flags

# Dict Comp
g++ tests/parser_tests/test_dict_comp_full.cpp $sources -o build/test_dict_comp_full.exe $flags

# Decorators
g++ tests/parser_tests/test_decorator_integration.cpp $sources -o build/test_decorator_integration.exe $flags

# Generators
g++ tests/parser_tests/test_yield_integration.cpp $sources -o build/test_yield_integration.exe $flags

# Run all
Write-Host "`n=== Running Phase 2 Tests ===`n"
./build/test_lambda_simple.exe
./build/test_comprehensions.exe
./build/test_dict_comp_full.exe
./build/test_decorator_integration.exe
./build/test_yield_integration.exe
```

---

## 🔍 Common Patterns

### Pattern 1: Map with Lambda
```python
# English
result = map(lambda x: x * 2, numbers)

# Arabic
نتيجة = خريطة(لامدا س: س * 2، أعداد)
```

### Pattern 2: Filter with Comprehension
```python
# English
filtered = [x for x in numbers if x > 0]

# Arabic
مصفى = [س لكل س في أعداد إذا س > 0]
```

### Pattern 3: Decorated Generator
```python
# English
@cached
function fibonacci_gen() {
    a = 0;
    b = 1;
    while (true) {
        yield a;
        temp = a;
        a = b;
        b = temp + b;
    }
}

# Arabic
@مخزن
دالة مولد_فيبوناتشي() {
    أ = 0؛
    ب = 1؛
    بينما (صحيح) {
        اعطِ أ؛
        مؤقت = أ؛
        أ = ب؛
        ب = مؤقت + ب؛
    }
}
```

### Pattern 4: Comprehension with Lambda
```python
# English
processed = [lambda x: x * factor for factor in [1, 2, 3]]

# Arabic
معالج = [لامدا س: س * عامل لكل عامل في [1، 2، 3]]
```

### Pattern 5: Multiple Decorators with Generator
```python
# English
@route("/api/stream")
@auth_required
@rate_limited
function data_stream() {
    for (item in database.items()) {
        yield item;
    }
}

# Arabic
@طريق("/api/stream")
@مصادقة_مطلوبة
@محدود_المعدل
دالة تدفق_بيانات() {
    لكل (عنصر في قاعدة_بيانات.عناصر()) {
        اعطِ عنصر؛
    }
}
```

---

## ⚡ Performance Notes

### Lambda Expressions
- Inline evaluation when possible
- Closure capture for external variables
- No separate function call overhead in simple cases

### Comprehensions
- Lazy evaluation recommended for large datasets
- Memory efficient compared to explicit loops
- List comprehensions eager, generators lazy

### Decorators
- Applied at function definition time
- Wrapping overhead minimal
- Can be optimized for common patterns (e.g., @staticmethod)

### Generators
- Memory efficient for large sequences
- State saved between yields
- Ideal for streaming data

---

## 🐛 Common Pitfalls / الأخطاء الشائعة

### Lambda
```python
# ❌ Wrong: Multi-line lambda (not supported)
lambda x: {
    result = x * 2;
    return result;
}

# ✅ Correct: Single expression
lambda x: x * 2
```

### List Comprehension
```python
# ❌ Wrong: Missing 'for'
[x * 2 in numbers]

# ✅ Correct
[x * 2 for x in numbers]
```

### Dict Comprehension
```python
# ❌ Wrong: Missing colon
{k v for k, v in dict}

# ✅ Correct
{k: v for k, v in dict}
```

### Decorators
```python
# ❌ Wrong: Decorator after function
function test() {}
@decorator

# ✅ Correct: Decorator before function
@decorator
function test() {}
```

### Generators
```python
# ❌ Wrong: Missing semicolon
yield x

# ✅ Correct
yield x;
```

---

## 📚 Further Reading

### Documentation
- [Phase 2 Complete Summary](PHASE2_COMPLETE_SUMMARY.md)
- [Language Specification](../../docs/language_spec/04_python_features.md)
- [API Documentation](../../docs/api/)

### Individual Reports
- [Lambda Report](TASK1_LAMBDA_REPORT.md)
- [List Comprehension Report](TASK2_LIST_COMP_REPORT.md)
- [Dict Comprehension Report](TASK3_DICT_COMP_REPORT.md)
- [Decorators Report](TASK4_DECORATORS_REPORT.md)
- [Generators Report](TASK5_GENERATORS_COMPLETION_REPORT.md)

### Test Files
- `tests/parser_tests/` - All integration tests
- `tests/lexer_tests/` - Token-level tests
- `examples/` - Usage examples

---

## 🎯 Quick Reference Table

| Feature | Keyword (EN) | Keyword (AR) | Symbol | Example |
|---------|--------------|--------------|--------|---------|
| Lambda | `lambda` | `لامدا` | `:` | `lambda x: x * 2` |
| List Comp | `for`, `in`, `if` | `لكل`, `في`, `إذا` | `[]` | `[x for x in list]` |
| Dict Comp | `for`, `in`, `if` | `لكل`, `في`, `إذا` | `{}`, `:` | `{k: v for k, v in dict}` |
| Decorator | - | - | `@` | `@decorator` |
| Yield | `yield` | `اعطِ` | `;` | `yield value;` |
| Yield From | `yield from` | `اعطِ من` | `;` | `yield from gen();` |

---

**Quick Reference Version:** 1.0  
**Last Updated:** 2024  
**Status:** ✅ Phase 2 Complete  

---

🎉 **All features ready to use!**  
🎉 **جميع الميزات جاهزة للاستخدام!**
