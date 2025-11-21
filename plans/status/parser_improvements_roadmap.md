# خارطة طريق تحسينات المحلل النحوي
# Parser Improvements Roadmap

**التاريخ / Date:** 2025-11-11  
**الحالة / Status:** خطة تنفيذية / Action Plan  
**المرحلة / Phase:** Phase 3 - Parser Enhancement  
**المرجع / Reference:** `parser_grammar_analysis.md`

---

## 🎯 الملخص التنفيذي / Executive Summary

بناءً على تحليل المطابقة بين `merged_grammar.md` والتنفيذ الحالي، تم تحديد **3 فئات أولوية** و **18 تحسين مطلوب**.

### الإحصائيات / Statistics
```
🔴 أولوية حرجة / Critical:      5 تحسينات / 5 improvements
🟡 أولوية عالية / High:         6 تحسينات / 6 improvements
🟢 أولوية متوسطة / Medium:       7 تحسينات / 7 improvements
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
📊 المجموع / Total:              18 تحسين / 18 improvements
```

### الجدول الزمني المقترح / Proposed Timeline
```
Phase 3A (Week 1):  تحسينات حرجة / Critical improvements     (5 items)
Phase 3B (Week 2):  تحسينات عالية / High priority           (6 items)
Phase 3C (Week 3):  تحسينات متوسطة / Medium priority        (7 items)
```

---

## 🔴 المرحلة 3A: التحسينات الحرجة / Critical Improvements

### 1️⃣ Type Parser Implementation ⭐ PRIORITY #1

**الحالة / Status:** ❌ مفقود بالكامل / Completely Missing  
**التأثير / Impact:** 🔴 CRITICAL - يمنع Semantic Analysis  
**الوقت المقدر / Estimated Time:** 4-6 ساعات / 4-6 hours

#### المشكلة / Problem
```cpp
// ❌ Current State: No type parsing capability
StmtPtr parseVarDecl() {
    Token name = consume(TT::IDENTIFIER, "Expected variable name");
    // Type is always UNKNOWN!
    ExprPtr initializer = match(TT::OP_ASSIGN) ? parseExpression() : nullptr;
    return std::make_unique<VarDeclStmt>(name.getValue(), 
                                         std::move(initializer), ...);
}
```

#### الحل المقترح / Proposed Solution

**الخطوة 1: إنشاء parseType() الأساسية**
```cpp
/**
 * @brief (AR) يحلل اسم نوع البيانات.
 *        (EN) Parses data type name.
 * 
 * Grammar:
 *   type ::= 'رقم' | 'عشري' | 'نص' | 'منطقي' | 'مصفوفة' | 'قاموس'
 *          | 'int' | 'float' | 'string' | 'bool' | 'array' | 'dict'
 *          | generic_type
 */
Data::DataType ParserCore::parseType() {
    using DT = Data::DataType;
    
    // Arabic keywords
    if (match(TT::KEYWORD_INT_AR)) return DT::INTEGER;
    if (match(TT::KEYWORD_FLOAT_AR)) return DT::FLOAT;
    if (match(TT::KEYWORD_STRING_AR)) return DT::STRING;
    if (match(TT::KEYWORD_BOOL_AR)) return DT::BOOLEAN;
    if (match(TT::KEYWORD_ARRAY_AR)) return DT::ARRAY;
    if (match(TT::KEYWORD_DICT_AR)) return DT::MAP;
    
    // English keywords
    if (match(TT::KEYWORD_INT)) return DT::INTEGER;
    if (match(TT::KEYWORD_FLOAT)) return DT::FLOAT;
    if (match(TT::KEYWORD_STRING)) return DT::STRING;
    if (match(TT::KEYWORD_BOOL)) return DT::BOOLEAN;
    if (match(TT::KEYWORD_ARRAY)) return DT::ARRAY;
    if (match(TT::KEYWORD_DICT)) return DT::MAP;
    
    error("(AR) توقع اسم نوع البيانات. (EN) Expected type name.");
    return DT::UNKNOWN;
}
```

**الخطوة 2: دمج في parseVarDecl()**
```cpp
/**
 * @brief (AR) يحلل تصريح متغير مع نوع اختياري.
 *        (EN) Parses variable declaration with optional type.
 * 
 * Grammar:
 *   var_decl ::= [type] IDENTIFIER ['=' expression] '؛'?
 */
StmtPtr ParserCore::parseVarDecl() {
    Data::DataType type = Data::DataType::UNKNOWN;
    
    // ✅ Check for optional type
    if (isTypeKeyword(peek())) {
        type = parseType();
    }
    
    Token name = consume(TT::IDENTIFIER, "Expected variable name");
    
    ExprPtr initializer = nullptr;
    if (match(TT::OP_ASSIGN)) {
        initializer = parseExpression();
    }
    
    consume(TT::SEMICOLON, "Expected ';' after variable declaration");
    
    return std::make_unique<VarDeclStmt>(
        type,  // ✅ Now includes explicit type
        name.getValue(),
        std::move(initializer),
        name.getPosition()
    );
}

// ✅ Helper function
bool ParserCore::isTypeKeyword(const Token& token) const {
    switch (token.getType()) {
        case TT::KEYWORD_INT:
        case TT::KEYWORD_FLOAT:
        case TT::KEYWORD_STRING:
        case TT::KEYWORD_BOOL:
        case TT::KEYWORD_ARRAY:
        case TT::KEYWORD_DICT:
        case TT::KEYWORD_INT_AR:
        case TT::KEYWORD_FLOAT_AR:
        case TT::KEYWORD_STRING_AR:
        case TT::KEYWORD_BOOL_AR:
        case TT::KEYWORD_ARRAY_AR:
        case TT::KEYWORD_DICT_AR:
            return true;
        default:
            return false;
    }
}
```

**الخطوة 3: تحديث parseFunctionDecl() لـ return type**
```cpp
/**
 * @brief (AR) يحلل تصريح دالة مع نوع الإرجاع الاختياري.
 *        (EN) Parses function declaration with optional return type.
 * 
 * Grammar:
 *   function_decl ::= 'دالة' [type] IDENTIFIER '(' [param_list] ')' block
 */
StmtPtr ParserCore::parseFunctionDecl() {
    Data::DataType returnType = Data::DataType::UNKNOWN;
    
    // ✅ Check for optional return type
    if (isTypeKeyword(peek())) {
        returnType = parseType();
    }
    
    Token name = consume(TT::IDENTIFIER, "Expected function name");
    consume(TT::PAREN_LEFT, "Expected '(' after function name");
    auto params = parseParameterList();
    consume(TT::PAREN_RIGHT, "Expected ')' after parameters");
    consume(TT::BRACE_LEFT, "Expected '{' before function body");
    auto body = parseBlockStmt();
    
    return std::make_unique<FunctionDecl>(
        returnType,  // ✅ Now includes return type
        name.getValue(),
        params,
        std::move(body),
        name.getPosition()
    );
}
```

#### الملفات المطلوب تعديلها / Files to Modify
```
1. include/parser/parser_core.h
   ├── إضافة: Data::DataType parseType();
   ├── إضافة: bool isTypeKeyword(const Token&) const;
   └── تحديث: توقيعات parseVarDecl(), parseFunctionDecl()

2. src/parser/parser_core_impl.cpp
   ├── تنفيذ: parseType() (30 سطر)
   ├── تنفيذ: isTypeKeyword() (15 سطر)
   ├── تحديث: parseVarDecl() (5 أسطر إضافية)
   └── تحديث: parseFunctionDecl() (5 أسطر إضافية)

3. include/lexer/token.h
   ├── إضافة: KEYWORD_INT_AR = "رقم"
   ├── إضافة: KEYWORD_FLOAT_AR = "عشري"
   ├── إضافة: KEYWORD_STRING_AR = "نص"
   ├── إضافة: KEYWORD_BOOL_AR = "منطقي"
   ├── إضافة: KEYWORD_ARRAY_AR = "مصفوفة"
   └── إضافة: KEYWORD_DICT_AR = "قاموس"

4. src/lexer/lexer_keywords.cpp
   └── إضافة: تسجيل الكلمات المفتاحية الجديدة
```

#### اختبارات التحقق / Verification Tests
```cpp
// Test 1: Variable with explicit type
"int x = 5;"
"رقم س = 5؛"

// Test 2: Function with return type
"int add(x, y) { return x + y; }"
"رقم اجمع(س، ص) { ارجع س + ص؛ }"

// Test 3: Type inference (no type)
"var x = 5;"  // Should still work
```

---

### 2️⃣ Lambda Expression Integration ⭐ PRIORITY #2

**الحالة / Status:** ⚠️ موجود لكن غير مستدعى / Exists but not called  
**التأثير / Impact:** 🔴 HIGH - ميزة لغة أساسية  
**الوقت المقدر / Estimated Time:** 2-3 ساعات / 2-3 hours

#### المشكلة / Problem
```cpp
// ✅ Function exists in parser_core.h
ExprPtr parseLambda();

// ❌ But NOT called anywhere in parsePrimary()!
ExprPtr parsePrimary() {
    // ... handles literals, identifiers, arrays, maps
    // ❌ No lambda detection!
    error("Expected expression.");
    return nullptr;
}
```

#### الحل المقترح / Proposed Solution

**الخطوة 1: كشف Lambda في parsePrimary()**
```cpp
ExprPtr ParserCore::parsePrimary() {
    // ... existing literal handling ...
    
    // ✅ Add Lambda detection
    // Grammar: 'دالة' [type] '(' [param_list] ')' '=>' expression
    if (match(TT::KEYWORD_LAMBDA) || match(TT::KEYWORD_FUNCTION)) {
        return parseLambda();
    }
    
    // ✅ Alternative: Arrow function: (x, y) => x + y
    if (check(TT::PAREN_LEFT)) {
        // Lookahead for arrow syntax
        size_t savePos = getCurrentTokenIndex();
        if (lookaheadForArrowFunction()) {
            return parseLambda();
        }
        // Restore if not lambda (regular parenthesized expr)
        restoreTokenIndex(savePos);
    }
    
    // ... rest of code ...
}
```

**الخطوة 2: تنفيذ parseLambda() الكامل**
```cpp
/**
 * @brief (AR) يحلل دالة لامدا (lambda expression).
 *        (EN) Parses lambda function expression.
 * 
 * Grammar:
 *   lambda_expr ::= 'دالة' [type] '(' [param_list] ')' '=>' expression
 *                 | '(' [param_list] ')' '=>' expression
 * 
 * Examples:
 *   - دالة(س) => س * 2
 *   - (x, y) => x + y
 *   - دالة رقم(س: رقم) => س * س
 */
ExprPtr ParserCore::parseLambda() {
    Data::DataType returnType = Data::DataType::UNKNOWN;
    
    // ✅ Optional return type (for verbose syntax)
    if (previous().getType() == TT::KEYWORD_LAMBDA || 
        previous().getType() == TT::KEYWORD_FUNCTION) {
        if (isTypeKeyword(peek())) {
            returnType = parseType();
        }
    }
    
    // ✅ Parse parameters
    consume(TT::PAREN_LEFT, "Expected '(' in lambda");
    auto params = parseParameterList();  // Already supports types
    consume(TT::PAREN_RIGHT, "Expected ')' after lambda parameters");
    
    // ✅ Arrow operator
    consume(TT::OP_ARROW, "Expected '=>' in lambda");
    
    // ✅ Parse body expression
    auto body = parseExpression();
    
    return std::make_unique<LambdaExpr>(
        returnType,
        params,
        std::move(body),
        previous().getPosition()
    );
}

/**
 * @brief (AR) يتحقق إذا كان التسلسل الحالي arrow function.
 *        (EN) Checks if current sequence is arrow function.
 */
bool ParserCore::lookaheadForArrowFunction() {
    // Simple heuristic: (params) =>
    int parenDepth = 0;
    size_t offset = 0;
    
    if (peek().getType() != TT::PAREN_LEFT) return false;
    parenDepth++;
    offset++;
    
    // Scan for matching closing paren
    while (parenDepth > 0 && offset < 10) {  // Max lookahead
        Token tok = peekAhead(offset);
        if (tok.getType() == TT::PAREN_LEFT) parenDepth++;
        if (tok.getType() == TT::PAREN_RIGHT) parenDepth--;
        if (tok.getType() == TT::END_OF_FILE) return false;
        offset++;
    }
    
    // Check for arrow after closing paren
    if (parenDepth == 0) {
        Token next = peekAhead(offset);
        return next.getType() == TT::OP_ARROW;
    }
    
    return false;
}
```

**الخطوة 3: إضافة OP_ARROW للـ Lexer**
```cpp
// في token.h
enum class TokenType {
    // ... existing tokens ...
    OP_ARROW,      ///< => (arrow function)
    // ...
};

// في lexer_core.cpp
if (ch == '=' && peek() == '>') {
    advance();  // consume '>'
    return Token(TokenType::OP_ARROW, "=>", pos);
}
```

#### الملفات المطلوب تعديلها / Files to Modify
```
1. include/parser/parser_core.h
   ├── تحديث: ExprPtr parseLambda() توقيع
   ├── إضافة: bool lookaheadForArrowFunction()
   ├── إضافة: Token peekAhead(size_t offset)
   └── إضافة: size_t getCurrentTokenIndex()

2. src/parser/parser_core_impl.cpp
   ├── تنفيذ: parseLambda() كامل (40 سطر)
   ├── تنفيذ: lookaheadForArrowFunction() (30 سطر)
   └── تحديث: parsePrimary() (10 أسطر إضافية)

3. include/lexer/token.h
   └── إضافة: TokenType::OP_ARROW

4. src/lexer/lexer_core.cpp
   └── إضافة: كشف => operator (5 أسطر)
```

---

### 3️⃣ Enhanced Parameter List ⭐ PRIORITY #3

**الحالة / Status:** ⚠️ مبسط - لا يدعم types/defaults  
**التأثير / Impact:** 🔴 HIGH - ضروري للـ typed functions  
**الوقت المقدر / Estimated Time:** 3-4 ساعات / 3-4 hours

#### المشكلة / Problem
```cpp
// ❌ Current: Only names, no types, no defaults
std::vector<std::string> parseParameterList() {
    std::vector<std::string> parameters;
    if (!check(TT::PAREN_RIGHT)) {
        do {
            Token param = consume(TT::IDENTIFIER, "Expected parameter name");
            parameters.push_back(param.getValue());
        } while (match(TT::COMMA));
    }
    return parameters;
}

// Grammar requires:
// param ::= IDENTIFIER [':' type] ['=' expression]
```

#### الحل المقترح / Proposed Solution

**الخطوة 1: إنشاء Parameter structure**
```cpp
/**
 * @struct Parameter
 * @brief (AR) معامل دالة مع نوع اختياري وقيمة افتراضية.
 *        (EN) Function parameter with optional type and default value.
 */
struct Parameter {
    std::string name;                  ///< Parameter name
    Data::DataType type;               ///< Optional type annotation
    std::unique_ptr<Expression> defaultValue;  ///< Optional default value
    
    Parameter(const std::string& n, 
              Data::DataType t = Data::DataType::UNKNOWN,
              std::unique_ptr<Expression> def = nullptr)
        : name(n), type(t), defaultValue(std::move(def)) {}
};

using ParameterList = std::vector<Parameter>;
```

**الخطوة 2: تحديث parseParameterList()**
```cpp
/**
 * @brief (AR) يحلل قائمة معاملات دالة مع أنواع وقيم افتراضية.
 *        (EN) Parses function parameter list with types and defaults.
 * 
 * Grammar:
 *   param_list ::= param ((',' | '،') param)*
 *   param      ::= IDENTIFIER [':' type] ['=' expression]
 * 
 * Examples:
 *   - (x, y, z)              // Simple names
 *   - (x: int, y: string)    // With types
 *   - (x = 10, y = "hello")  // With defaults
 *   - (x: int = 0)           // Type + default
 */
ParameterList ParserCore::parseParameterList() {
    ParameterList parameters;
    
    if (!check(TT::PAREN_RIGHT)) {
        do {
            // Parse parameter name
            Token name = consume(TT::IDENTIFIER, 
                "(AR) توقع اسم معامل. (EN) Expected parameter name.");
            
            Data::DataType type = Data::DataType::UNKNOWN;
            ExprPtr defaultValue = nullptr;
            
            // ✅ Optional type annotation: x: int
            if (match(TT::COLON)) {
                type = parseType();
            }
            
            // ✅ Optional default value: x = 10
            if (match(TT::OP_ASSIGN)) {
                defaultValue = parseExpression();
            }
            
            parameters.push_back(Parameter(
                name.getValue(),
                type,
                std::move(defaultValue)
            ));
            
        } while (match(TT::COMMA) || match(TT::COMMA_ARABIC));
    }
    
    return parameters;
}
```

**الخطوة 3: تحديث FunctionDecl AST**
```cpp
// في declarations.h
class FunctionDecl : public Statement {
public:
    FunctionDecl(
        Data::DataType returnType,
        const std::string& name,
        const ParameterList& params,  // ✅ Changed from vector<string>
        StmtPtr body,
        const SourcePosition& pos
    ) : Statement(pos)
      , returnType_(returnType)
      , name_(name)
      , parameters_(params)  // ✅ Now stores full Parameter objects
      , body_(std::move(body))
    {}
    
    const ParameterList& getParameters() const { return parameters_; }
    
private:
    Data::DataType returnType_;
    std::string name_;
    ParameterList parameters_;  // ✅ Changed type
    StmtPtr body_;
};
```

#### الملفات المطلوب تعديلها / Files to Modify
```
1. include/parser/ast/declarations.h
   ├── إضافة: struct Parameter
   ├── إضافة: using ParameterList
   └── تحديث: FunctionDecl constructor/fields

2. include/parser/parser_core.h
   └── تحديث: ParameterList parseParameterList()

3. src/parser/parser_core_helpers.cpp
   └── تحديث: parseParameterList() implementation (40 سطر)

4. include/lexer/token.h
   └── إضافة: TokenType::COMMA_ARABIC = '،'

5. All AST consumers (visitor, printer)
   └── تحديث: للتعامل مع ParameterList بدلاً من vector<string>
```

---

### 4️⃣ C-Style For Loop Support ⭐ PRIORITY #4

**الحالة / Status:** ⚠️ دعم Python-style فقط  
**التأثير / Impact:** 🟡 MEDIUM - للتوافق مع C++  
**الوقت المقدر / Estimated Time:** 2-3 ساعات / 2-3 hours

#### المشكلة / Problem
```cpp
// ✅ Supported: for x in collection
for (x in list) { ... }

// ❌ Not supported: C-style for
for (int i = 0; i < 10; i++) { ... }
```

#### الحل المقترح / Proposed Solution

**الخطوة 1: توسيع parseForStmt()**
```cpp
/**
 * @brief (AR) يحلل حلقة for (Python-style أو C-style).
 *        (EN) Parses for loop (Python-style or C-style).
 * 
 * Grammar:
 *   for_stmt ::= 'لكل' IDENTIFIER 'في' expression block
 *              | 'لكل' '(' [var_decl] '؛' [expression] '؛' [expression] ')' block
 */
StmtPtr ParserCore::parseForStmt() {
    consume(TT::PAREN_LEFT, "Expected '(' after 'for'");
    
    // ✅ Lookahead to determine style
    if (lookaheadForCStyleFor()) {
        return parseCStyleFor();
    } else {
        return parsePythonStyleFor();
    }
}

/**
 * @brief (AR) كشف C-style for loop.
 *        (EN) Detect C-style for loop.
 * 
 * Heuristic: Look for semicolon before closing paren
 */
bool ParserCore::lookaheadForCStyleFor() {
    size_t offset = 0;
    int parenDepth = 1;  // Already consumed opening paren
    
    while (offset < 20 && parenDepth > 0) {
        Token tok = peekAhead(offset);
        
        if (tok.getType() == TT::SEMICOLON && parenDepth == 1) {
            return true;  // Found semicolon at top level
        }
        
        if (tok.getType() == TT::PAREN_LEFT) parenDepth++;
        if (tok.getType() == TT::PAREN_RIGHT) parenDepth--;
        if (tok.getType() == TT::END_OF_FILE) break;
        
        offset++;
    }
    
    return false;
}

/**
 * @brief (AR) يحلل C-style for loop.
 *        (EN) Parses C-style for loop.
 * 
 * Grammar: for ( [init] ; [cond] ; [incr] ) body
 */
StmtPtr ParserCore::parseCStyleFor() {
    // Parse init (optional)
    StmtPtr init = nullptr;
    if (!check(TT::SEMICOLON)) {
        if (isTypeKeyword(peek()) || check(TT::KEYWORD_VAR)) {
            init = parseVarDecl();
        } else {
            auto expr = parseExpression();
            init = std::make_unique<ExprStmt>(std::move(expr), expr->getPosition());
        }
    }
    consume(TT::SEMICOLON, "Expected ';' after for init");
    
    // Parse condition (optional)
    ExprPtr condition = nullptr;
    if (!check(TT::SEMICOLON)) {
        condition = parseExpression();
    }
    consume(TT::SEMICOLON, "Expected ';' after for condition");
    
    // Parse increment (optional)
    ExprPtr increment = nullptr;
    if (!check(TT::PAREN_RIGHT)) {
        increment = parseExpression();
    }
    consume(TT::PAREN_RIGHT, "Expected ')' after for clauses");
    
    // Parse body
    auto body = parseStatement();
    
    return std::make_unique<CStyleForStmt>(
        std::move(init),
        std::move(condition),
        std::move(increment),
        std::move(body),
        previous().getPosition()
    );
}

/**
 * @brief (AR) يحلل Python-style for loop.
 *        (EN) Parses Python-style for loop.
 * 
 * Grammar: for var in collection body
 */
StmtPtr ParserCore::parsePythonStyleFor() {
    Token var = consume(TT::IDENTIFIER, "Expected loop variable");
    
    if (!match(TT::KEYWORD_IN)) {
        error("Expected 'in' in for loop");
    }
    
    auto collection = parseExpression();
    consume(TT::PAREN_RIGHT, "Expected ')' after collection");
    auto body = parseStatement();
    
    return std::make_unique<ForStmt>(
        var.getValue(),
        std::move(collection),
        std::move(body),
        var.getPosition()
    );
}
```

**الخطوة 2: إضافة CStyleForStmt AST**
```cpp
// في statements.h
class CStyleForStmt : public Statement {
public:
    CStyleForStmt(
        StmtPtr init,
        ExprPtr condition,
        ExprPtr increment,
        StmtPtr body,
        const SourcePosition& pos
    ) : Statement(pos)
      , init_(std::move(init))
      , condition_(std::move(condition))
      , increment_(std::move(increment))
      , body_(std::move(body))
    {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitCStyleForStmt(*this);
    }
    
    Statement* getInit() const { return init_.get(); }
    Expression* getCondition() const { return condition_.get(); }
    Expression* getIncrement() const { return increment_.get(); }
    Statement* getBody() const { return body_.get(); }
    
private:
    StmtPtr init_;
    ExprPtr condition_;
    ExprPtr increment_;
    StmtPtr body_;
};
```

---

### 5️⃣ With Statement 'as' Variable ⭐ PRIORITY #5

**الحالة / Status:** ⚠️ لا يدعم variable binding  
**التأثير / Impact:** 🟡 MEDIUM - للـ context managers  
**الوقت المقدر / Estimated Time:** 1-2 ساعة / 1-2 hours

#### المشكلة / Problem
```cpp
// ✅ Supported: with resource { body }
// ❌ Not supported: with resource as var { body }

// Grammar: 'مع' expression ['كـ' IDENTIFIER] block
```

#### الحل المقترح / Proposed Solution

```cpp
/**
 * @brief (AR) يحلل جملة with مع variable binding اختياري.
 *        (EN) Parses with statement with optional variable binding.
 * 
 * Grammar:
 *   with_statement ::= 'مع' expression ['كـ' IDENTIFIER] block
 *                    | 'with' expression ['as' IDENTIFIER] block
 */
StmtPtr ParserCore::parseWithStmt() {
    // Parse resource expression
    auto resource = parseExpression();
    
    // ✅ Optional variable binding: as var
    std::string varName;
    if (match(TT::KEYWORD_AS) || match(TT::KEYWORD_AS_ARABIC)) {
        Token var = consume(TT::IDENTIFIER, 
            "(AR) توقع اسم متغير بعد 'كـ'. (EN) Expected variable name after 'as'.");
        varName = var.getValue();
    }
    
    // Parse body
    consume(TT::BRACE_LEFT, "Expected '{' before with body");
    auto body = parseBlockStmt();
    
    return std::make_unique<WithStmt>(
        std::move(resource),
        varName,  // ✅ Now includes optional variable name
        std::move(body),
        previous().getPosition()
    );
}
```

**تحديث WithStmt AST:**
```cpp
class WithStmt : public Statement {
public:
    WithStmt(
        ExprPtr resource,
        const std::string& varName,  // ✅ Added
        StmtPtr body,
        const SourcePosition& pos
    ) : Statement(pos)
      , resource_(std::move(resource))
      , varName_(varName)  // ✅ Added
      , body_(std::move(body))
    {}
    
    const std::string& getVarName() const { return varName_; }
    
private:
    ExprPtr resource_;
    std::string varName_;  // ✅ Added
    StmtPtr body_;
};
```

---

## 🟡 المرحلة 3B: التحسينات ذات الأولوية العالية / High Priority

### 6️⃣ OOP: Constructor/Destructor Parsing

**الوقت المقدر / Estimated Time:** 4-5 ساعات / 4-5 hours

```cpp
/**
 * @brief (AR) يحلل constructor داخل class body.
 *        (EN) Parses constructor inside class body.
 * 
 * Grammar:
 *   constructor_decl ::= ('باني' | 'منشئ') '(' [param_list] ')' 
 *                        [':' initializer_list] block
 */
StmtPtr ParserCore::parseConstructorDecl() {
    Token keyword = previous();  // 'constructor' or 'باني'
    
    // Parse parameters
    consume(TT::PAREN_LEFT, "Expected '(' after constructor keyword");
    auto params = parseParameterList();
    consume(TT::PAREN_RIGHT, "Expected ')' after parameters");
    
    // ✅ Optional initializer list: : member(value), ...
    std::vector<MemberInitializer> initializers;
    if (match(TT::COLON)) {
        initializers = parseMemberInitializerList();
    }
    
    // Parse body
    consume(TT::BRACE_LEFT, "Expected '{' before constructor body");
    auto body = parseBlockStmt();
    
    return std::make_unique<ConstructorDecl>(
        params,
        initializers,
        std::move(body),
        keyword.getPosition()
    );
}

/**
 * @brief (AR) يحلل initializer list: member(value), ...
 *        (EN) Parses member initializer list.
 */
std::vector<MemberInitializer> ParserCore::parseMemberInitializerList() {
    std::vector<MemberInitializer> initializers;
    
    do {
        Token member = consume(TT::IDENTIFIER, "Expected member name");
        consume(TT::PAREN_LEFT, "Expected '(' after member name");
        auto value = parseExpression();
        consume(TT::PAREN_RIGHT, "Expected ')' after initializer value");
        
        initializers.push_back({member.getValue(), std::move(value)});
    } while (match(TT::COMMA));
    
    return initializers;
}
```

---

### 7️⃣ OOP: Visibility Modifiers

**الوقت المقدر / Estimated Time:** 2-3 ساعات / 2-3 hours

```cpp
/**
 * @brief (AR) يحلل visibility modifier للأعضاء.
 *        (EN) Parses visibility modifier for members.
 * 
 * Grammar:
 *   visibility ::= 'عام' | 'خاص' | 'محمي'
 *                | 'public' | 'private' | 'protected'
 */
Visibility ParserCore::parseVisibility() {
    if (match(TT::KEYWORD_PUBLIC) || match(TT::KEYWORD_PUBLIC_AR)) {
        return Visibility::PUBLIC;
    }
    if (match(TT::KEYWORD_PRIVATE) || match(TT::KEYWORD_PRIVATE_AR)) {
        return Visibility::PRIVATE;
    }
    if (match(TT::KEYWORD_PROTECTED) || match(TT::KEYWORD_PROTECTED_AR)) {
        return Visibility::PROTECTED;
    }
    return Visibility::PUBLIC;  // Default
}

/**
 * @brief (AR) يحلل field أو method declaration مع visibility.
 *        (EN) Parses field or method declaration with visibility.
 */
StmtPtr ParserCore::parseClassMember() {
    // ✅ Check for visibility modifier
    Visibility vis = Visibility::PUBLIC;
    if (isVisibilityKeyword(peek())) {
        vis = parseVisibility();
    }
    
    // ✅ Check for static modifier
    bool isStatic = false;
    if (match(TT::KEYWORD_STATIC)) {
        isStatic = true;
    }
    
    // Parse the actual member (field or method)
    if (match(TT::KEYWORD_FUNCTION)) {
        auto method = parseFunctionDecl();
        // Set visibility and static flags
        static_cast<FunctionDecl*>(method.get())->setVisibility(vis);
        static_cast<FunctionDecl*>(method.get())->setStatic(isStatic);
        return method;
    }
    
    // Field declaration
    auto field = parseVarDecl();
    static_cast<VarDeclStmt*>(field.get())->setVisibility(vis);
    static_cast<VarDeclStmt*>(field.get())->setStatic(isStatic);
    return field;
}
```

---

### 8️⃣ Import/Export Full Syntax

**الوقت المقدر / Estimated Time:** 3-4 ساعات / 3-4 hours

```cpp
/**
 * @brief (AR) يحلل import statement كامل.
 *        (EN) Parses full import statement.
 * 
 * Grammar:
 *   import_stmt ::= 'استورد' module_name ['كـ' alias]
 *                 | 'من' module_name 'استورد' ('*' | identifier_list)
 */
StmtPtr ParserCore::parseImportStmt() {
    // ✅ Check for 'from' syntax
    if (match(TT::KEYWORD_FROM)) {
        return parseFromImport();
    }
    
    // ✅ Regular import: import module [as alias]
    std::vector<std::string> modulePath = parseModuleName();
    
    std::string alias;
    if (match(TT::KEYWORD_AS)) {
        Token aliasToken = consume(TT::IDENTIFIER, "Expected alias name");
        alias = aliasToken.getValue();
    }
    
    consume(TT::SEMICOLON, "Expected ';' after import");
    
    return std::make_unique<ImportStmt>(
        modulePath,
        alias,
        std::vector<std::string>(),  // No specific names
        false,  // Not wildcard
        previous().getPosition()
    );
}

/**
 * @brief (AR) يحلل 'from module import ...' statement.
 *        (EN) Parses 'from module import ...' statement.
 */
StmtPtr ParserCore::parseFromImport() {
    // Parse module path
    std::vector<std::string> modulePath = parseModuleName();
    
    // Expect 'import' keyword
    if (!match(TT::KEYWORD_IMPORT)) {
        error("Expected 'import' after module name");
    }
    
    // ✅ Check for wildcard: from module import *
    if (match(TT::OP_MULTIPLY)) {
        consume(TT::SEMICOLON, "Expected ';' after import *");
        return std::make_unique<ImportStmt>(
            modulePath,
            "",
            std::vector<std::string>(),
            true,  // Wildcard import
            previous().getPosition()
        );
    }
    
    // ✅ Parse identifier list: from module import a, b, c
    std::vector<std::string> names;
    do {
        Token name = consume(TT::IDENTIFIER, "Expected identifier");
        names.push_back(name.getValue());
    } while (match(TT::COMMA));
    
    consume(TT::SEMICOLON, "Expected ';' after import statement");
    
    return std::make_unique<ImportStmt>(
        modulePath,
        "",
        names,
        false,
        previous().getPosition()
    );
}

/**
 * @brief (AR) يحلل module name (مع dots).
 *        (EN) Parses module name (with dots).
 * 
 * Grammar: module_name ::= identifier ('.' identifier)*
 */
std::vector<std::string> ParserCore::parseModuleName() {
    std::vector<std::string> path;
    
    Token first = consume(TT::IDENTIFIER, "Expected module name");
    path.push_back(first.getValue());
    
    // ✅ Handle nested modules: package.submodule
    while (match(TT::DOT)) {
        Token part = consume(TT::IDENTIFIER, "Expected identifier after '.'");
        path.push_back(part.getValue());
    }
    
    return path;
}
```

---

### 9️⃣-1️⃣1️⃣ Remaining High Priority Items

**9️⃣ Generic Type Support** (3-4 hours)
**1️⃣0️⃣ Try-Catch Type Annotations** (2 hours)
**1️⃣1️⃣ Operator Overloading** (4-5 hours)

_(التفاصيل الكاملة متاحة عند الطلب)_

---

## 🟢 المرحلة 3C: التحسينات المتوسطة / Medium Priority

### 1️⃣2️⃣ Generator Functions (yield)

**الوقت المقدر / Estimated Time:** 4-5 ساعات / 4-5 hours

```cpp
/**
 * @brief (AR) يحلل yield statement.
 *        (EN) Parses yield statement.
 * 
 * Grammar:
 *   yield_statement ::= 'أنتج' expression
 *                     | 'yield' expression
 */
StmtPtr ParserCore::parseYieldStmt() {
    Token keyword = previous();
    
    // Parse expression to yield
    auto value = parseExpression();
    
    consume(TT::SEMICOLON, "Expected ';' after yield");
    
    return std::make_unique<YieldStmt>(
        std::move(value),
        keyword.getPosition()
    );
}
```

---

### 1️⃣3️⃣ Decorator Support

**الوقت المقدر / Estimated Time:** 5-6 ساعات / 5-6 hours

```cpp
/**
 * @brief (AR) يحلل decorator.
 *        (EN) Parses decorator.
 * 
 * Grammar:
 *   decorator ::= '@' decorator_name ['(' [arg_list] ')'] NEWLINE
 */
Decorator ParserCore::parseDecorator() {
    consume(TT::AT, "Expected '@' for decorator");
    
    Token name = consume(TT::IDENTIFIER, "Expected decorator name");
    
    // Optional arguments
    ExprList args;
    if (match(TT::PAREN_LEFT)) {
        args = parseArgumentList();
        consume(TT::PAREN_RIGHT, "Expected ')' after decorator args");
    }
    
    // Expect newline after decorator
    consume(TT::NEWLINE, "Expected newline after decorator");
    
    return Decorator{name.getValue(), std::move(args)};
}

/**
 * @brief (AR) يحلل decorated function.
 *        (EN) Parses decorated function.
 */
StmtPtr ParserCore::parseDecoratedFunction() {
    std::vector<Decorator> decorators;
    
    // Parse all decorators
    while (check(TT::AT)) {
        decorators.push_back(parseDecorator());
    }
    
    // Parse the function
    if (!match(TT::KEYWORD_FUNCTION)) {
        error("Expected function after decorators");
    }
    
    auto func = parseFunctionDecl();
    
    // Attach decorators to function
    static_cast<FunctionDecl*>(func.get())->setDecorators(decorators);
    
    return func;
}
```

---

### 1️⃣4️⃣-1️⃣8️⃣ Remaining Medium Priority Items

**1️⃣4️⃣ Generator Expressions** (2-3 hours)
**1️⃣5️⃣ Multiple Inheritance** (3 hours)
**1️⃣6️⃣ Ternary Operator** (1-2 hours)
**1️⃣7️⃣ Slice Notation** (2-3 hours)
**1️⃣8️⃣ Pattern Matching** (6-8 hours)

---

## 📊 ملخص الجدول الزمني / Timeline Summary

```
Week 1 - Phase 3A (Critical): 16-21 hours
├── Type Parser:              4-6 hours
├── Lambda Integration:       2-3 hours
├── Enhanced Parameters:      3-4 hours
├── C-Style For Loop:         2-3 hours
├── With Statement 'as':      1-2 hours
└── Testing & Integration:    4-5 hours

Week 2 - Phase 3B (High): 20-25 hours
├── Constructor/Destructor:   4-5 hours
├── Visibility Modifiers:     2-3 hours
├── Import/Export Full:       3-4 hours
├── Generic Types:            3-4 hours
├── Try-Catch Types:          2 hours
├── Operator Overloading:     4-5 hours
└── Testing & Integration:    2-3 hours

Week 3 - Phase 3C (Medium): 23-30 hours
├── Generators (yield):       4-5 hours
├── Decorators:               5-6 hours
├── Generator Expressions:    2-3 hours
├── Multiple Inheritance:     3 hours
├── Ternary Operator:         1-2 hours
├── Slice Notation:           2-3 hours
├── Pattern Matching:         6-8 hours
└── Testing & Integration:    0 hours (handled by pattern matching tests)

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total Estimated Time:         59-76 hours
                              (~2-3 weeks full-time)
                              (~4-6 weeks part-time)
```

---

## ✅ معايير الإنجاز / Completion Criteria

### لكل تحسين / Per Improvement
- [ ] التنفيذ مكتمل / Implementation complete
- [ ] الاختبارات تمر بنجاح / Tests passing
- [ ] التوثيق محدث / Documentation updated
- [ ] لا أخطاء تصريف / No compilation errors
- [ ] مُلتزم في Git / Committed to git

### للمرحلة الكاملة / For Full Phase
- [ ] جميع التحسينات الحرجة مكتملة / All critical improvements done
- [ ] 100 اختبار على الأقل / At least 100 tests
- [ ] تغطية كود > 80% / Code coverage > 80%
- [ ] تحديث merged_grammar.md / Update merged_grammar.md
- [ ] تقرير مطابقة نهائي / Final compliance report

---

## 🎯 الخطوة التالية / Next Step

**الآن: إنشاء الاختبارات للميزات الحالية**
1. اختبارات Parser الأساسية
2. اختبارات التعبيرات والأولويات
3. اختبارات الجمل والتحكم
4. اختبارات معالجة الأخطاء

**بعد ذلك: تنفيذ Phase 3A (Critical)**
- البدء بـ Type Parser (الأهم)
- ثم Lambda Integration
- ثم Enhanced Parameters

---

**التاريخ / Date:** 2025-11-11  
**الحالة / Status:** خطة جاهزة للتنفيذ / Plan Ready for Execution ✅  
**المحلل / Prepared by:** GitHub Copilot
