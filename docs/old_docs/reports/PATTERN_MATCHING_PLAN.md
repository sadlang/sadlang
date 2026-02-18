# 🎯 خطة تنفيذ Pattern Matching
# Pattern Matching Implementation Plan

**التاريخ:** 19 ديسمبر 2025  
**الأولوية:** 🔴 P0 - التالي في المرحلة 2  
**التعقيد:** 🔴 عالي  
**الوقت المتوقع:** 10-15 ساعة

---

## 📋 نظرة عامة

Pattern Matching هي ميزة قوية تسمح بمطابقة القيم مع أنماط مختلفة وتنفيذ كود بناءً على النمط المطابق. مشابهة لـ `switch` لكن أقوى بكثير.

---

## 🎯 الصيغة المستهدفة

### 1. Basic Literal Matching
```sad
match x:
    case 0:
        print("صفر")
    case 1:
        print("واحد")
    case 2:
        print("اثنان")
    case _:
        print("أخرى")
```

### 2. Multiple Values (OR Pattern)
```sad
match status:
    case "success" | "ok" | "done":
        print("نجح")
    case "error" | "failed":
        print("فشل")
```

### 3. Guard Conditions
```sad
match x:
    case n if n < 0:
        print("سالب")
    case n if n == 0:
        print("صفر")
    case n if n > 0:
        print("موجب")
```

### 4. List Pattern Matching
```sad
match list:
    case []:
        print("فارغة")
    case [x]:
        print("عنصر واحد:", x)
    case [first, second]:
        print("عنصران:", first, second)
    case [first, *rest]:
        print("الأول:", first, "الباقي:", rest)
```

### 5. Variable Binding
```sad
match point:
    case [x, y]:
        print("نقطة ثنائية:", x, y)
    case [x, y, z]:
        print("نقطة ثلاثية:", x, y, z)
```

### 6. Nested Patterns
```sad
match data:
    case ["user", name, age] if age >= 18:
        print("مستخدم بالغ:", name)
    case ["user", name, age]:
        print("مستخدم قاصر:", name)
```

---

## 🏗️ البنية المعمارية

### 1. عقد AST الجديدة

#### أ. MatchStmt - الجملة الرئيسية
```cpp
class MatchStmt : public Statement {
public:
    ExprPtr value;                     // القيمة المُختبرة
    std::vector<CaseClause> cases;     // قائمة الحالات
    Lexer::Position position;
    
    MatchStmt(ExprPtr val, 
              std::vector<CaseClause> cs,
              const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), value(std::move(val)), cases(std::move(cs)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitMatchStmt(*this);
    }
};
```

#### ب. CaseClause - فرع case واحد
```cpp
struct CaseClause {
    std::unique_ptr<Pattern> pattern;  // النمط
    ExprPtr guard;                     // شرط اختياري (if condition)
    std::vector<StmtPtr> body;         // الكود المُنفذ
    
    CaseClause(std::unique_ptr<Pattern> p,
               ExprPtr g,
               std::vector<StmtPtr> b)
        : pattern(std::move(p)),
          guard(std::move(g)),
          body(std::move(b)) {}
};
```

#### ج. Pattern - النمط الأساسي (abstract)
```cpp
class Pattern {
public:
    virtual ~Pattern() = default;
    virtual bool matches(const Value& value, 
                        std::map<std::string, Value>& bindings) const = 0;
    virtual std::string toString() const = 0;
};
```

#### د. LiteralPattern - نمط قيمة حرفية
```cpp
class LiteralPattern : public Pattern {
public:
    Value literal;  // القيمة: عدد، نص، إلخ
    
    explicit LiteralPattern(Value val) : literal(std::move(val)) {}
    
    bool matches(const Value& value, 
                std::map<std::string, Value>& bindings) const override {
        return value.toString() == literal.toString();
    }
    
    std::string toString() const override {
        return literal.toString();
    }
};
```

#### هـ. WildcardPattern - نمط شامل (_)
```cpp
class WildcardPattern : public Pattern {
public:
    bool matches(const Value& value, 
                std::map<std::string, Value>& bindings) const override {
        return true;  // يطابق أي شيء
    }
    
    std::string toString() const override {
        return "_";
    }
};
```

#### و. VariablePattern - نمط متغير (x, name, etc)
```cpp
class VariablePattern : public Pattern {
public:
    std::string name;
    
    explicit VariablePattern(std::string n) : name(std::move(n)) {}
    
    bool matches(const Value& value, 
                std::map<std::string, Value>& bindings) const override {
        bindings[name] = value;  // ربط القيمة بالمتغير
        return true;
    }
    
    std::string toString() const override {
        return name;
    }
};
```

#### ز. ListPattern - نمط قائمة
```cpp
class ListPattern : public Pattern {
public:
    std::vector<std::unique_ptr<Pattern>> elements;
    bool has_rest;         // هل يوجد *rest؟
    std::string rest_name; // اسم متغير *rest
    
    ListPattern(std::vector<std::unique_ptr<Pattern>> elems,
                bool rest = false,
                std::string rest_n = "")
        : elements(std::move(elems)),
          has_rest(rest),
          rest_name(std::move(rest_n)) {}
    
    bool matches(const Value& value, 
                std::map<std::string, Value>& bindings) const override;
    
    std::string toString() const override;
};
```

#### ح. OrPattern - نمط OR (a | b | c)
```cpp
class OrPattern : public Pattern {
public:
    std::vector<std::unique_ptr<Pattern>> alternatives;
    
    explicit OrPattern(std::vector<std::unique_ptr<Pattern>> alts)
        : alternatives(std::move(alts)) {}
    
    bool matches(const Value& value, 
                std::map<std::string, Value>& bindings) const override {
        for (const auto& alt : alternatives) {
            if (alt->matches(value, bindings)) {
                return true;
            }
        }
        return false;
    }
    
    std::string toString() const override;
};
```

---

## 🔧 تعديلات Parser

### 1. إضافة parseMatchStmt()

```cpp
StmtPtr ParserCore::parseMatchStmt() {
    // استهلاك 'match' أو 'حالة'
    consume(TT::KEYWORD_MATCH, "Expected 'match' or 'حالة'");
    
    // تحليل القيمة المُختبرة
    auto value = parseExpression();
    
    // استهلاك ':'
    consume(TT::COLON, "Expected ':' after match expression");
    
    // تحليل حالات case
    std::vector<CaseClause> cases;
    
    while (check(TT::KEYWORD_CASE)) {
        cases.push_back(parseCaseClause());
    }
    
    if (cases.empty()) {
        error("Match statement must have at least one case");
    }
    
    return std::make_unique<MatchStmt>(
        std::move(value),
        std::move(cases)
    );
}
```

### 2. إضافة parseCaseClause()

```cpp
CaseClause ParserCore::parseCaseClause() {
    // استهلاك 'case' أو 'في_حالة'
    consume(TT::KEYWORD_CASE, "Expected 'case' or 'في_حالة'");
    
    // تحليل النمط
    auto pattern = parsePattern();
    
    // تحليل guard اختياري
    ExprPtr guard = nullptr;
    if (check(TT::KEYWORD_IF)) {
        advance();
        guard = parseExpression();
    }
    
    // استهلاك ':'
    consume(TT::COLON, "Expected ':' after case pattern");
    
    // تحليل جسم case
    std::vector<StmtPtr> body;
    
    // إذا كان هناك indentation أو بلوك
    if (check(TT::INDENT) || check(TT::BRACE_LEFT)) {
        body = parseBlock();
    } else {
        // جملة واحدة على نفس السطر
        body.push_back(parseStatement());
    }
    
    return CaseClause(
        std::move(pattern),
        std::move(guard),
        std::move(body)
    );
}
```

### 3. إضافة parsePattern()

```cpp
std::unique_ptr<Pattern> ParserCore::parsePattern() {
    // Wildcard: _
    if (check(TT::UNDERSCORE)) {
        advance();
        return std::make_unique<WildcardPattern>();
    }
    
    // List pattern: [...]
    if (check(TT::BRACKET_LEFT)) {
        return parseListPattern();
    }
    
    // Literal or Variable
    auto primary = parsePrimaryPattern();
    
    // Check for OR: a | b | c
    if (check(TT::PIPE)) {
        std::vector<std::unique_ptr<Pattern>> alternatives;
        alternatives.push_back(std::move(primary));
        
        while (match(TT::PIPE)) {
            alternatives.push_back(parsePrimaryPattern());
        }
        
        return std::make_unique<OrPattern>(std::move(alternatives));
    }
    
    return primary;
}
```

### 4. إضافة parsePrimaryPattern()

```cpp
std::unique_ptr<Pattern> ParserCore::parsePrimaryPattern() {
    // Number literal
    if (check(TT::NUMBER)) {
        auto token = advance();
        return std::make_unique<LiteralPattern>(
            Value(std::stod(token.getValue()))
        );
    }
    
    // String literal
    if (check(TT::STRING)) {
        auto token = advance();
        return std::make_unique<LiteralPattern>(
            Value(token.getValue())
        );
    }
    
    // Boolean literal
    if (check(TT::KEYWORD_TRUE) || check(TT::KEYWORD_FALSE)) {
        bool val = check(TT::KEYWORD_TRUE);
        advance();
        return std::make_unique<LiteralPattern>(Value(val));
    }
    
    // Variable pattern
    if (check(TT::IDENTIFIER)) {
        auto token = advance();
        return std::make_unique<VariablePattern>(token.getValue());
    }
    
    error("Expected pattern");
    return nullptr;
}
```

### 5. إضافة parseListPattern()

```cpp
std::unique_ptr<Pattern> ParserCore::parseListPattern() {
    consume(TT::BRACKET_LEFT, "Expected '['");
    
    std::vector<std::unique_ptr<Pattern>> elements;
    bool has_rest = false;
    std::string rest_name;
    
    // قائمة فارغة: []
    if (check(TT::BRACKET_RIGHT)) {
        advance();
        return std::make_unique<ListPattern>(std::move(elements));
    }
    
    // تحليل العناصر
    do {
        // Rest pattern: *rest
        if (check(TT::STAR)) {
            advance();
            
            if (!check(TT::IDENTIFIER)) {
                error("Expected variable name after '*'");
            }
            
            rest_name = advance().getValue();
            has_rest = true;
            
            // لا يمكن أن يكون هناك عناصر بعد *rest
            break;
        }
        
        elements.push_back(parsePattern());
        
    } while (match(TT::COMMA));
    
    consume(TT::BRACKET_RIGHT, "Expected ']'");
    
    return std::make_unique<ListPattern>(
        std::move(elements),
        has_rest,
        rest_name
    );
}
```

---

## 🎭 تعديلات Interpreter

### إضافة visitMatchStmt()

```cpp
void StatementExecutor::visitMatchStmt(MatchStmt& stmt) {
    // تقييم القيمة المُختبرة
    stmt.value->accept(exprEvaluator_);
    Value testValue = exprEvaluator_.getLastResult();
    
    // تجربة كل case
    for (auto& caseClause : stmt.cases) {
        // map لحفظ المتغيرات المربوطة
        std::map<std::string, Value> bindings;
        
        // محاولة مطابقة النمط
        if (caseClause.pattern->matches(testValue, bindings)) {
            // فحص guard إذا وُجد
            if (caseClause.guard) {
                // إضافة المتغيرات المربوطة للـ scope
                for (const auto& [name, value] : bindings) {
                    variableManager_.define(name, value);
                }
                
                caseClause.guard->accept(exprEvaluator_);
                Value guardResult = exprEvaluator_.getLastResult();
                
                if (!guardResult.toBool()) {
                    // Guard فشل، جرّب الـ case التالي
                    // حذف المتغيرات المؤقتة
                    for (const auto& [name, _] : bindings) {
                        variableManager_.remove(name);
                    }
                    continue;
                }
            } else {
                // لا يوجد guard، إضافة المتغيرات مباشرة
                for (const auto& [name, value] : bindings) {
                    variableManager_.define(name, value);
                }
            }
            
            // تنفيذ جسم case
            scopeManager_.pushScope(Data::ScopeType::BLOCK, "match_case");
            
            for (auto& stmt : caseClause.body) {
                stmt->accept(*this);
                
                // فحص break/return
                if (shouldReturn_ || shouldBreak_ || shouldContinue_) {
                    scopeManager_.popScope();
                    return;
                }
            }
            
            scopeManager_.popScope();
            
            // تم العثور على مطابقة، لا داعي لاختبار الباقي
            return;
        }
    }
    
    // لم يتم العثور على أي مطابقة
    throw RuntimeError("No matching case found in match statement");
}
```

### تنفيذ ListPattern::matches()

```cpp
bool ListPattern::matches(const Value& value, 
                         std::map<std::string, Value>& bindings) const {
    // التحقق أن القيمة هي array
    if (value.getType() != Data::DataType::ARRAY) {
        return false;
    }
    
    const auto& array = value.getArray();
    
    // إذا لم يكن هناك rest pattern
    if (!has_rest) {
        // يجب أن يكون الحجم مطابق تماماً
        if (array.size() != elements.size()) {
            return false;
        }
        
        // مطابقة كل عنصر
        for (size_t i = 0; i < elements.size(); ++i) {
            if (!elements[i]->matches(array[i], bindings)) {
                return false;
            }
        }
        
        return true;
    }
    
    // يوجد rest pattern
    // يجب أن يكون الحجم >= عدد العناصر الثابتة
    if (array.size() < elements.size()) {
        return false;
    }
    
    // مطابقة العناصر الثابتة
    for (size_t i = 0; i < elements.size(); ++i) {
        if (!elements[i]->matches(array[i], bindings)) {
            return false;
        }
    }
    
    // ربط الباقي
    std::vector<Value> rest_elements;
    for (size_t i = elements.size(); i < array.size(); ++i) {
        rest_elements.push_back(array[i]);
    }
    
    bindings[rest_name] = Value(rest_elements);
    
    return true;
}
```

---

## 🧪 خطة الاختبار

### 1. اختبارات أساسية
```sad
# test_match_basic.s

print("=== Basic Match Tests ===")

# Test 1: Simple literal matching
match 1:
    case 0:
        print("FAIL: matched 0")
    case 1:
        print("PASS: matched 1")
    case 2:
        print("FAIL: matched 2")

# Test 2: Wildcard
match 999:
    case 1:
        print("FAIL")
    case 2:
        print("FAIL")
    case _:
        print("PASS: wildcard works")

# Test 3: String matching
match "hello":
    case "world":
        print("FAIL")
    case "hello":
        print("PASS: string match")
    case _:
        print("FAIL")
```

### 2. اختبارات OR Pattern
```sad
# test_match_or.s

match status:
    case "ok" | "success" | "done":
        print("PASS: OR pattern works")
    case _:
        print("FAIL")
```

### 3. اختبارات Guard
```sad
# test_match_guard.s

match 15:
    case x if x < 10:
        print("FAIL: small")
    case x if x >= 10 and x < 20:
        print("PASS: medium")
    case x if x >= 20:
        print("FAIL: large")
```

### 4. اختبارات List Pattern
```sad
# test_match_list.s

match [1, 2, 3]:
    case []:
        print("FAIL: empty")
    case [x]:
        print("FAIL: one element")
    case [x, y]:
        print("FAIL: two elements")
    case [x, y, z]:
        print("PASS: three elements:", x, y, z)

# Test with rest
match [1, 2, 3, 4, 5]:
    case [first, *rest]:
        print("PASS: first =", first, "rest =", rest)
```

---

## 📋 قائمة المهام التفصيلية

### المرحلة 1: تصميم AST (يوم 1)
- [ ] إنشاء ملف `pattern_nodes.h`
- [ ] تعريف class `Pattern` الأساسي
- [ ] تعريف `LiteralPattern`
- [ ] تعريف `WildcardPattern`
- [ ] تعريف `VariablePattern`
- [ ] تعريف `ListPattern`
- [ ] تعريف `OrPattern`
- [ ] تعريف `CaseClause` struct
- [ ] تعريف `MatchStmt` class
- [ ] إضافة declarations في `ast_visitor.h`

### المرحلة 2: تنفيذ Parser (يوم 2-3)
- [ ] إضافة `KEYWORD_MATCH` و `KEYWORD_CASE` في Lexer
- [ ] تنفيذ `parseMatchStmt()`
- [ ] تنفيذ `parseCaseClause()`
- [ ] تنفيذ `parsePattern()`
- [ ] تنفيذ `parsePrimaryPattern()`
- [ ] تنفيذ `parseListPattern()`
- [ ] دمج في `parseStatement()`
- [ ] اختبار Parser منفصل

### المرحلة 3: تنفيذ Interpreter (يوم 4-5)
- [ ] تنفيذ `visitMatchStmt()` في `StatementExecutor`
- [ ] تنفيذ `LiteralPattern::matches()`
- [ ] تنفيذ `WildcardPattern::matches()`
- [ ] تنفيذ `VariablePattern::matches()`
- [ ] تنفيذ `ListPattern::matches()`
- [ ] تنفيذ `OrPattern::matches()`
- [ ] معالجة bindings والـ scope
- [ ] معالجة guards
- [ ] اختبار Interpreter

### المرحلة 4: تحديث Visitors (يوم 6)
- [ ] إضافة `visitMatchStmt()` في `ASTPrinter`
- [ ] إضافة stubs في `OptimizationPass`
- [ ] التأكد من كل visitors محدّث

### المرحلة 5: الاختبار الشامل (يوم 7)
- [ ] اختبارات literal matching
- [ ] اختبارات wildcard
- [ ] اختبارات OR pattern
- [ ] اختبارات guard conditions
- [ ] اختبارات list patterns
- [ ] اختبارات rest patterns
- [ ] اختبارات nested patterns
- [ ] اختبارات edge cases

### المرحلة 6: التوثيق (يوم 7)
- [ ] إنشاء `PATTERN_MATCHING_COMPLETION_REPORT.md`
- [ ] تحديث `phase2_report.md`
- [ ] تحديث `PHASE2_STATUS_SUMMARY.md`
- [ ] كتابة أمثلة شاملة
- [ ] توثيق الـ API

---

## 🎯 معايير النجاح

### وظيفية:
- ✅ جميع أنواع الأنماط تعمل
- ✅ Guards تعمل بشكل صحيح
- ✅ Variable binding يعمل
- ✅ Rest patterns تعمل
- ✅ معالجة أخطاء صحيحة

### أداء:
- ✅ مطابقة سريعة O(n) للأنماط البسيطة
- ✅ لا memory leaks
- ✅ استخدام معقول للذاكرة

### جودة:
- ✅ كود نظيف وموثق
- ✅ تعليقات ثنائية اللغة
- ✅ اختبارات شاملة
- ✅ معالجة أخطاء واضحة

---

## 📅 الجدول الزمني

| اليوم | المهمة | الساعات |
|------|---------|---------|
| 1 | تصميم AST nodes | 2-3 |
| 2 | Parser (أساسي) | 3-4 |
| 3 | Parser (متقدم) | 3-4 |
| 4 | Interpreter (أساسي) | 3-4 |
| 5 | Interpreter (متقدم) | 3-4 |
| 6 | Visitors + fixes | 2-3 |
| 7 | اختبارات + توثيق | 3-4 |

**المجموع:** 19-26 ساعة (~3 أسابيع بمعدل يوم واحد)

---

## 🚀 ملاحظات تنفيذية

### نقاط حرجة:
1. **Variable Binding:** يجب إدارة scope بعناية
2. **Guard Evaluation:** يجب تقييم guards بعد binding
3. **Rest Patterns:** معالجة دقيقة للعناصر المتبقية
4. **Error Messages:** رسائل خطأ واضحة للمطابقة الفاشلة

### تحسينات مستقبلية:
- دعم Dict patterns: `{key: value}`
- دعم Type patterns: `case int(x):`
- دعم As patterns: `case [x, y] as point:`
- Pattern exhaustiveness checking

---

**الحالة:** 📝 جاهز للبدء  
**الأولوية:** 🔴 عالية جداً - التالي في المرحلة 2  
**المتطلبات:** Lexer يجب أن يدعم KEYWORD_MATCH و KEYWORD_CASE
