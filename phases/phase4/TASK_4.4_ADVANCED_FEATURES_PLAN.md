# المرحلة 4.4: الميزات المتقدمة - خطة التنفيذ
# Phase 4.4: Advanced Features - Implementation Plan

**تاريخ الإنشاء / Created:** نوفمبر 21، 2025  
**الحالة / Status:** 🔄 قيد التخطيط / In Planning  
**الأولوية / Priority:** 🟡 متوسطة / Medium

---

## 🎯 نظرة عامة / Overview

المرحلة 4.4 تركز على إضافة ميزات برمجية متقدمة تجعل لغة ص أكثر قوة وتعبيراً:

1. **Lambda Expressions** - دوال مجهولة (Anonymous Functions)
2. **List/Dict Comprehensions** - بناء المجموعات بشكل مختصر
3. **Generators & Yield** - دوال مولدة كسولة (Lazy Evaluation)

---

## 📋 الميزة 1: Lambda Expressions (دوال Lambda)

### الهدف / Goal
إضافة دعم لدوال Lambda المجهولة التي يمكن تعريفها inline وتمريرها كمعاملات.

### Syntax / البناء اللغوي

```s
// البناء الأساسي
var add = lambda(a, b) { return a + b; };
print(add(10, 20));  // 30

// دالة بدون معاملات
var sayHello = lambda() { print("مرحباً"); };
sayHello();

// دالة بمعامل واحد
var double = lambda(x) { return x * 2; };
print(double(5));  // 10

// Lambda كمعامل لدالة
func map(arr, func) {
    var result = [];
    for item in arr {
        result.append(func(item));
    }
    return result;
}

var numbers = [1, 2, 3, 4, 5];
var squared = map(numbers, lambda(x) { return x * x; });
// [1, 4, 9, 16, 25]
```

### التصميم الفني / Technical Design

#### 1. AST Node - LambdaExpr

```cpp
/**
 * @brief Lambda expression node / عقدة تعبير Lambda
 * 
 * Represents an anonymous function (lambda).
 * يمثل دالة مجهولة (lambda).
 * 
 * @example Examples / أمثلة:
 * - lambda(x) { return x * 2; }
 * - lambda(a, b) { return a + b; }
 * - lambda() { print("Hello"); }
 */
class LambdaExpr : public Expression {
public:
    std::vector<Parameter> parameters;  ///< Lambda parameters / معاملات Lambda
    StmtPtr body;                       ///< Lambda body / جسم Lambda
    
    /**
     * @brief Constructor / البناء
     * @param params Parameter list / قائمة المعاملات
     * @param body Function body / جسم الدالة
     * @param pos Source position / الموقع في الكود
     */
    LambdaExpr(std::vector<Parameter> params, StmtPtr body,
               const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), parameters(std::move(params)), body(std::move(body)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitLambdaExpr(*this);
    }
    
    std::string toString() const override;
    Data::DataType getType() const override { return Data::DataType::FUNCTION; }
};
```

#### 2. Visitor Method

```cpp
// في base_ast_visitor.h
virtual void visitLambdaExpr(AST::LambdaExpr& node) {
    // Default: do nothing
}
```

#### 3. Evaluation في ExpressionEvaluator

```cpp
void ExpressionEvaluator::visitLambdaExpr(LambdaExpr& node) {
    // إنشاء دالة مجهولة في FunctionManager
    // Generate unique name for lambda
    static int lambdaCounter = 0;
    std::string lambdaName = "__lambda_" + std::to_string(lambdaCounter++);
    
    // Convert parameters
    std::vector<Data::FunctionParameter> params;
    for (const auto& param : node.parameters) {
        params.push_back(Data::FunctionParameter(
            param.name,
            dataTypeToString(param.type)
        ));
    }
    
    // Store lambda as function
    std::shared_ptr<Parser::ASTNode> bodyNode(
        reinterpret_cast<Parser::ASTNode*>(node.body.get()),
        [](Parser::ASTNode*) {}
    );
    
    functionManager_.defineFunction(lambdaName, params, bodyNode);
    
    // Return function value (contains name reference)
    lastResult_ = Data::Value::createFunction(lambdaName, params.size());
}
```

### الاختبارات / Tests

```cpp
// Test 1: Simple lambda
void test_simple_lambda() {
    // var add = lambda(a, b) { return a + b; };
    // var result = add(10, 20);
    // assert result == 30
}

// Test 2: Lambda without parameters
void test_lambda_no_params() {
    // var getValue = lambda() { return 42; };
    // assert getValue() == 42
}

// Test 3: Lambda with one parameter
void test_lambda_one_param() {
    // var double = lambda(x) { return x * 2; };
    // assert double(5) == 10
}

// Test 4: Lambda as function argument
void test_lambda_as_argument() {
    // func apply(x, f) { return f(x); }
    // var result = apply(5, lambda(n) { return n * n; });
    // assert result == 25
}

// Test 5: Lambda with local variables
void test_lambda_with_locals() {
    // var calc = lambda(x) {
    //     var temp = x * 2;
    //     return temp + 10;
    // };
    // assert calc(5) == 20
}

// Test 6: Multiple lambdas
void test_multiple_lambdas() {
    // var add = lambda(a, b) { return a + b; };
    // var mul = lambda(a, b) { return a * b; };
    // assert add(2, 3) == 5
    // assert mul(2, 3) == 6
}

// Test 7: Nested lambda calls
void test_nested_lambda() {
    // var outer = lambda(x) {
    //     return lambda(y) { return x + y; };
    // };
    // // Note: Requires closures for full support
}

// Test 8: Lambda returning lambda
void test_lambda_return_lambda() {
    // var makeAdder = lambda(n) {
    //     return lambda(x) { return x + n; };
    // };
    // var add5 = makeAdder(5);
    // assert add5(10) == 15  // Requires closures
}
```

### التحديات / Challenges

1. **Closure Support** - Lambda حالياً لا يدعم capturing variables من outer scope
2. **Anonymous Function Naming** - نحتاج نظام تسمية فريد
3. **Lifetime Management** - Lambda body يجب أن يظل حياً

### الحل المقترح / Proposed Solution

**Phase 1 (Current):** Lambda بسيطة بدون closures
- ✅ Lambda كدالة عادية مع اسم مولد
- ✅ Parameters و body عادية
- ❌ لا يمكن الوصول لمتغيرات خارجية

**Phase 2 (Future):** Lambda مع Closures
- ✅ Capture outer scope variables
- ✅ Environment chain
- ✅ Full closure support

---

## 📋 الميزة 2: List/Dict Comprehensions

### الهدف / Goal
بناء المصفوفات والقواميس بشكل مختصر وواضح باستخدام syntax معبر.

### Syntax / البناء اللغوي

```s
// List comprehension أساسي
var numbers = [1, 2, 3, 4, 5];
var squares = [x * x for x in numbers];
// [1, 4, 9, 16, 25]

// مع شرط
var evens = [x for x in numbers if x % 2 == 0];
// [2, 4]

// مع تحويل
var names = ["أحمد", "محمد", "علي"];
var lengths = [len(name) for name in names];
// [4, 4, 3]

// Dictionary comprehension
var keys = ["a", "b", "c"];
var values = [1, 2, 3];
var dict = {k: v for k, v in zip(keys, values)};
// {"a": 1, "b": 2, "c": 3}

// Nested comprehension
var matrix = [[1, 2], [3, 4], [5, 6]];
var flattened = [item for row in matrix for item in row];
// [1, 2, 3, 4, 5, 6]
```

### التصميم الفني / Technical Design

#### AST Nodes

```cpp
/**
 * @brief List comprehension expression / تعبير بناء المصفوفة
 * 
 * Syntax: [expr for var in iterable if condition]
 */
class ListComprehensionExpr : public Expression {
public:
    ExprPtr element;           ///< Element expression / تعبير العنصر
    std::string variable;      ///< Loop variable / متغير الحلقة
    ExprPtr iterable;          ///< Iterable expression / التعبير القابل للتكرار
    ExprPtr condition;         ///< Optional condition / شرط اختياري (nullable)
    
    ListComprehensionExpr(ExprPtr elem, std::string var, ExprPtr iter,
                         ExprPtr cond = nullptr,
                         const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), element(std::move(elem)), variable(var),
          iterable(std::move(iter)), condition(std::move(cond)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitListComprehensionExpr(*this);
    }
    
    Data::DataType getType() const override { return Data::DataType::ARRAY; }
};

/**
 * @brief Dictionary comprehension expression / تعبير بناء القاموس
 * 
 * Syntax: {key_expr: value_expr for var in iterable if condition}
 */
class DictComprehensionExpr : public Expression {
public:
    ExprPtr keyExpr;           ///< Key expression / تعبير المفتاح
    ExprPtr valueExpr;         ///< Value expression / تعبير القيمة
    std::string variable;      ///< Loop variable / متغير الحلقة
    ExprPtr iterable;          ///< Iterable expression / التعبير القابل للتكرار
    ExprPtr condition;         ///< Optional condition / شرط اختياري
    
    DictComprehensionExpr(ExprPtr key, ExprPtr val, std::string var,
                         ExprPtr iter, ExprPtr cond = nullptr,
                         const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), keyExpr(std::move(key)), valueExpr(std::move(val)),
          variable(var), iterable(std::move(iter)), condition(std::move(cond)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitDictComprehensionExpr(*this);
    }
    
    Data::DataType getType() const override { return Data::DataType::MAP; }
};
```

#### Evaluation

```cpp
void ExpressionEvaluator::visitListComprehensionExpr(ListComprehensionExpr& node) {
    // 1. Evaluate iterable
    node.iterable->accept(*this);
    Data::Value iterableValue = lastResult_;
    
    // 2. Create result array
    std::vector<Data::Value> result;
    
    // 3. Iterate over iterable
    if (iterableValue.isArray()) {
        for (size_t i = 0; i < iterableValue.size(); ++i) {
            Data::Value item = iterableValue.at(i);
            
            // Define loop variable
            variableManager_.define(node.variable, item);
            
            // Check condition if exists
            bool includeItem = true;
            if (node.condition) {
                node.condition->accept(*this);
                includeItem = lastResult_.toBool();
            }
            
            // Evaluate element expression and add to result
            if (includeItem) {
                node.element->accept(*this);
                result.push_back(lastResult_);
            }
        }
    }
    
    // 4. Return result
    lastResult_ = Data::Value(result);
}
```

### الاختبارات / Tests

8 اختبارات شاملة لكل نوع (List + Dict)

---

## 📋 الميزة 3: Generators & Yield

### الهدف / Goal
دوال مولدة (Generator Functions) تستخدم `yield` لإنتاج قيم كسولة (lazy evaluation).

### Syntax / البناء اللغوي

```s
// Generator بسيط
func countdown(n) {
    while n > 0 {
        yield n;
        n = n - 1;
    }
}

// استخدام Generator
for num in countdown(5) {
    print(num);  // 5, 4, 3, 2, 1
}

// Generator لا نهائي
func fibonacci() {
    var a = 0;
    var b = 1;
    while true {
        yield a;
        var temp = a;
        a = b;
        b = temp + b;
    }
}

// أخذ 10 قيم فقط
var fibs = [];
var gen = fibonacci();
for i in range(10) {
    fibs.append(gen.next());
}
```

### التصميم الفني / Technical Design

#### AST Node - YieldStmt

```cpp
/**
 * @brief Yield statement / عبارة yield
 * 
 * Used in generator functions to produce values.
 * تستخدم في الدوال المولدة لإنتاج القيم.
 */
class YieldStmt : public Statement {
public:
    ExprPtr value;  ///< Value to yield / القيمة المنتجة
    
    YieldStmt(ExprPtr val, const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), value(std::move(val)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitYieldStmt(*this);
    }
    
    std::string toString() const override;
};
```

#### Generator State Management

```cpp
// في FunctionManager أو Generator Manager جديد
class GeneratorState {
public:
    std::shared_ptr<AST::Statement> body;
    std::map<std::string, Data::Value> locals;
    size_t programCounter;  // Where we stopped
    bool isExhausted;
    
    Data::Value next();  // Resume and get next value
    bool hasNext() const;
};
```

### التحديات / Challenges

1. **State Preservation** - حفظ حالة التنفيذ بين calls
2. **Program Counter** - تتبع مكان التوقف
3. **Local Variables** - الحفاظ على المتغيرات المحلية

---

## 📊 جدول التنفيذ / Implementation Timeline

| الميزة / Feature | الوقت المتوقع / Time | الأولوية / Priority |
|-----------------|---------------------|-------------------|
| Lambda Expressions | 3-4 ساعات | 🔴 عالية / High |
| List Comprehensions | 2-3 ساعات | 🟡 متوسطة / Medium |
| Dict Comprehensions | 1-2 ساعة | 🟡 متوسطة / Medium |
| Generators & Yield | 4-5 ساعات | 🟢 منخفضة / Low |
| **الإجمالي / Total** | **10-14 ساعة** | |

---

## 📝 ترتيب التنفيذ / Implementation Order

### المرحلة الأولى (3-4 ساعات) - Lambda
1. ✅ إضافة `LambdaExpr` إلى expressions.h
2. ✅ إضافة `visitLambdaExpr` إلى visitors
3. ✅ تنفيذ evaluation في ExpressionEvaluator
4. ✅ إضافة support في FunctionManager
5. ✅ كتابة 8 اختبارات شاملة
6. ✅ تشغيل الاختبارات والتأكد من النجاح

### المرحلة الثانية (3-4 ساعات) - Comprehensions
1. ✅ إضافة `ListComprehensionExpr` و `DictComprehensionExpr`
2. ✅ تنفيذ evaluation
3. ✅ اختبارات (8 لكل نوع)

### المرحلة الثالثة (4-5 ساعات) - Generators
1. ✅ إضافة `YieldStmt`
2. ✅ إنشاء GeneratorState management
3. ✅ تنفيذ next() protocol
4. ✅ اختبارات

---

## ✅ معايير النجاح / Success Criteria

- ✅ جميع AST nodes مضافة بشكل صحيح
- ✅ جميع visitor methods منفذة
- ✅ 20+ اختبار (8 lambda + 8 comprehensions + 4 generators)
- ✅ معدل نجاح 100%
- ✅ توثيق ثنائي اللغة كامل
- ✅ تقرير إكمال شامل

---

**التحديث التالي / Next Update:** بعد إكمال Lambda Expressions  
**الحالة / Status:** 🟢 جاهز للبدء / Ready to Start
