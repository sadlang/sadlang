# خطة تنفيذ تحسينات المحلل النحوي - مرحلية
# Parser Improvements Implementation Plan - Phased

**التاريخ / Date:** 2025-11-11  
**المرحلة / Phase:** Phase 3B - Planned Enhancements  
**الحالة / Status:** 📋 خطة جاهزة للتنفيذ / Ready for Implementation  
**المرجع / Reference:** `parser_improvements_roadmap.md`

---

## 🎯 نظرة عامة / Overview

بعد إكمال إصلاحات المرحلة 3A بنجاح، نبدأ الآن بتنفيذ التحسينات المخططة على **3 مراحل**:

```
📦 Phase 3B-1: Type System & Parsing     (Week 1) - 5 items
📦 Phase 3B-2: Advanced Features         (Week 2) - 6 items  
📦 Phase 3B-3: Optimization & Polish     (Week 3) - 7 items
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
📊 Total:                                          18 improvements
```

---

## 📦 Phase 3B-1: Type System & Parsing (الأسبوع الأول)

### 🔴 Priority #1: Type Parser Implementation
**الوقت / Time:** 6 ساعات / 6 hours  
**التعقيد / Complexity:** 🔴 High

#### الخطوات / Steps

**Step 1: إنشاء parseType() الأساسية**
```cpp
/**
 * @file parser_core_types.cpp
 * @brief (AR) تنفيذ تحليل أنواع البيانات
 *        (EN) Implementation of data type parsing
 * @author Sad Language Development Team
 * @date 2025-11-11
 */

#include "../../include/parser/parser_core.h"

namespace Sad {
namespace Parser {

/**
 * @brief (AR) يحلل اسم نوع البيانات الأساسي.
 *        (EN) Parses basic data type name.
 * 
 * Grammar:
 *   basic_type ::= 'رقم' | 'int' | 'integer'
 *                | 'عشري' | 'float' | 'double'
 *                | 'نص' | 'string' | 'str'
 *                | 'منطقي' | 'bool' | 'boolean'
 *                | 'فراغ' | 'void'
 *                | 'لاشيء' | 'null' | 'none'
 * 
 * @return (AR) نوع البيانات المحلل. (EN) Parsed data type.
 */
Data::DataType ParserCore::parseType() {
    using TT = Lexer::TokenType;
    using DT = Data::DataType;
    
    // Integer types (Arabic + English)
    if (match(TT::TYPE_INTEGER)) return DT::INTEGER;
    
    // Float types (Arabic + English)
    if (match(TT::TYPE_DOUBLE)) return DT::FLOAT;
    
    // String types (Arabic + English)
    if (match(TT::TYPE_STRING)) return DT::STRING;
    
    // Boolean types (Arabic + English)
    if (match(TT::TYPE_BOOLEAN)) return DT::BOOLEAN;
    
    // Void type (Arabic + English)
    if (match(TT::TYPE_VOID)) return DT::NONE;
    
    // Null type (Arabic + English)
    if (match(TT::TYPE_NULL)) return DT::NONE;
    
    // Array type (requires generic parsing)
    if (match(TT::TYPE_ARRAY)) {
        // TODO: Parse generic type parameters
        return DT::ARRAY;
    }
    
    // Map/Dict type (requires generic parsing)
    if (match(TT::TYPE_MAP)) {
        // TODO: Parse generic type parameters
        return DT::MAP;
    }
    
    error("(AR) توقع اسم نوع بيانات. (EN) Expected data type name.");
    return DT::UNKNOWN;
}

/**
 * @brief (AR) يتحقق إذا كان الرمز الحالي هو كلمة مفتاحية لنوع بيانات.
 *        (EN) Checks if current token is a data type keyword.
 * 
 * @return (AR) صحيح إذا كان نوع بيانات. (EN) True if data type keyword.
 */
bool ParserCore::isTypeKeyword() const {
    using TT = Lexer::TokenType;
    
    TokenType type = peek().getType();
    return type == TT::TYPE_INTEGER ||
           type == TT::TYPE_DOUBLE ||
           type == TT::TYPE_STRING ||
           type == TT::TYPE_BOOLEAN ||
           type == TT::TYPE_VOID ||
           type == TT::TYPE_NULL ||
           type == TT::TYPE_ARRAY ||
           type == TT::TYPE_MAP;
}

} // namespace Parser
} // namespace Sad
```

**Step 2: تحديث parseVarDecl() لاستخدام parseType()**
```cpp
/**
 * @brief (AR) يحلل تصريح متغير مع نوع اختياري.
 *        (EN) Parses variable declaration with optional type.
 * 
 * Grammar:
 *   var_decl ::= ['متغير' | 'var'] [type] IDENTIFIER ['=' expression] ';'
 * 
 * Examples:
 *   متغير x = 10                    // Type inferred
 *   متغير رقم x = 10                // Explicit Arabic type
 *   var int x = 10                  // Explicit English type
 *   int x = 10                      // Type-first style (C-like)
 * 
 * @return (AR) عقدة تصريح المتغير. (EN) Variable declaration node.
 */
StmtPtr ParserCore::parseVarDecl() {
    Data::DataType type = Data::DataType::UNKNOWN;
    
    // Check for optional type
    // (AR) التحقق من النوع الاختياري
    if (isTypeKeyword()) {
        type = parseType();
    }
    
    // Parse variable name
    // (AR) تحليل اسم المتغير
    Token name = consume(TT::IDENTIFIER, 
        "(AR) توقع اسم المتغير. (EN) Expected variable name.");
    
    // Parse optional initializer
    // (AR) تحليل المهيئ الاختياري
    ExprPtr initializer = nullptr;
    if (match(TT::OP_ASSIGN)) {
        initializer = parseExpression();
    }
    
    consume(TT::SEMICOLON, 
        "(AR) توقع ';' بعد تصريح المتغير. (EN) Expected ';' after variable declaration.");
    
    // Create variable declaration statement
    // (AR) إنشاء جملة تصريح المتغير
    return std::make_unique<VarDeclStmt>(
        name.getValue(),
        type,
        std::move(initializer),
        false,  // not constant
        name.getPosition()
    );
}
```

**Step 3: تحديث parseFunctionDecl() للـ return types**
```cpp
/**
 * @brief (AR) يحلل تصريح دالة مع نوع الإرجاع الاختياري.
 *        (EN) Parses function declaration with optional return type.
 * 
 * Grammar:
 *   func_decl ::= ['دالة' | 'function'] [return_type] IDENTIFIER 
 *                 '(' param_list ')' block
 * 
 * Examples:
 *   دالة احسب(س، ص) { ... }                // Return type inferred
 *   دالة رقم احسب(س، ص) { ... }            // Arabic return type
 *   function int calculate(x, y) { ... }   // English return type
 * 
 * @return (AR) عقدة تصريح الدالة. (EN) Function declaration node.
 */
StmtPtr ParserCore::parseFunctionDecl() {
    Data::DataType returnType = Data::DataType::UNKNOWN;
    
    // Check for optional return type
    // (AR) التحقق من نوع الإرجاع الاختياري
    if (isTypeKeyword()) {
        returnType = parseType();
    }
    
    // Parse function name
    // (AR) تحليل اسم الدالة
    Token name = consume(TT::IDENTIFIER, 
        "(AR) توقع اسم الدالة. (EN) Expected function name.");
    
    // Parse parameter list
    // (AR) تحليل قائمة المعاملات
    consume(TT::PAREN_LEFT, 
        "(AR) توقع '(' بعد اسم الدالة. (EN) Expected '(' after function name.");
    auto params = parseParameterListWithTypes();  // ✅ NEW: Parse types
    consume(TT::PAREN_RIGHT, 
        "(AR) توقع ')' بعد المعاملات. (EN) Expected ')' after parameters.");
    
    // Parse function body
    // (AR) تحليل جسم الدالة
    consume(TT::BRACE_LEFT, 
        "(AR) توقع '{' قبل جسم الدالة. (EN) Expected '{' before function body.");
    auto body = parseBlockStmt();
    
    // Create function declaration node
    // (AR) إنشاء عقدة تصريح الدالة
    return std::make_unique<FunctionDecl>(
        name.getValue(),
        std::move(params),
        returnType,  // ✅ Now includes return type
        std::move(body),
        false,  // not exported
        name.getPosition()
    );
}
```

**Step 4: تحديث parseParameterList() لدعم الأنواع**
```cpp
/**
 * @brief (AR) يحلل قائمة معاملات مع أنواع اختيارية.
 *        (EN) Parses parameter list with optional types.
 * 
 * Grammar:
 *   param_list ::= [typed_param (',' typed_param)*]
 *   typed_param ::= [type] IDENTIFIER ['=' expression]
 * 
 * Examples:
 *   (x, y)                    // No types
 *   (int x, float y)          // English types
 *   (رقم س، عشري ص)           // Arabic types
 *   (x = 10, y = 20)          // Default values
 *   (int x = 10, float y)     // Mixed
 * 
 * @return (AR) قائمة المعاملات. (EN) Parameter list.
 */
std::vector<Parameter> ParserCore::parseParameterListWithTypes() {
    std::vector<Parameter> parameters;
    
    // Parse parameters
    // (AR) تحليل المعاملات
    if (!check(TT::PAREN_RIGHT)) {
        do {
            Data::DataType paramType = Data::DataType::UNKNOWN;
            
            // Check for optional type
            // (AR) التحقق من النوع الاختياري
            if (isTypeKeyword()) {
                paramType = parseType();
            }
            
            // Parse parameter name
            // (AR) تحليل اسم المعامل
            Token param = consume(TT::IDENTIFIER, 
                "(AR) توقع اسم معامل. (EN) Expected parameter name.");
            
            // Parse optional default value
            // (AR) تحليل القيمة الافتراضية الاختيارية
            ExprPtr defaultValue = nullptr;
            if (match(TT::OP_ASSIGN)) {
                defaultValue = parseExpression();
            }
            
            // Add parameter to list
            // (AR) إضافة المعامل للقائمة
            parameters.emplace_back(
                param.getValue(),
                paramType,
                std::move(defaultValue)
            );
            
        } while (match(TT::COMMA));
    }
    
    return parameters;
}
```

#### الملفات المطلوبة / Required Files
```
src/parser/parser_core_types.cpp         (✅ NEW FILE)
include/parser/parser_core.h             (✅ UPDATE: Add parseType() declaration)
src/parser/parser_core_impl.cpp          (✅ UPDATE: parseVarDecl, parseFunctionDecl)
src/parser/parser_core_helpers.cpp       (✅ UPDATE: parseParameterListWithTypes)
```

#### الاختبارات / Tests
```cpp
// Test 1: Variable with explicit type
متغير رقم س = 10;
var int x = 10;

// Test 2: Function with return type
دالة رقم جمع(رقم أ، رقم ب) {
    إرجاع أ + ب;
}

function int add(int a, int b) {
    return a + b;
}

// Test 3: Type inference
متغير ص = 3.14;  // inferred as float
var y = 3.14;

// Test 4: Mixed parameters
دالة احسب(س، رقم ص = 5) {
    إرجاع س * ص;
}
```

---

### 🔴 Priority #2: Generic Types Support
**الوقت / Time:** 4 ساعات / 4 hours  
**التعقيد / Complexity:** 🔴 High

#### الخطوات / Steps

**Step 1: توسيع parseType() لدعم Generics**
```cpp
/**
 * @brief (AR) يحلل نوع مع معاملات نوعية (generic).
 *        (EN) Parses type with generic parameters.
 * 
 * Grammar:
 *   generic_type ::= base_type ['<' type_list '>']
 *   type_list ::= type (',' type)*
 * 
 * Examples:
 *   مصفوفة<رقم>                // Array<int>
 *   قاموس<نص، رقم>             // Map<string, int>
 *   مصفوفة<مصفوفة<رقم>>        // Array<Array<int>>
 * 
 * @return (AR) نوع البيانات مع المعاملات. (EN) Data type with parameters.
 */
Data::DataType ParserCore::parseGenericType() {
    Data::DataType baseType = parseType();
    
    // Check for generic parameters
    // (AR) التحقق من المعاملات النوعية
    if (match(TT::OP_LESS)) {
        std::vector<Data::DataType> typeParams;
        
        do {
            typeParams.push_back(parseGenericType());  // Recursive for nested
        } while (match(TT::COMMA));
        
        consume(TT::OP_GREATER,
            "(AR) توقع '>' بعد معاملات النوع. (EN) Expected '>' after type parameters.");
        
        // TODO: Store type parameters in DataType structure
        // For now, just return the base type
    }
    
    return baseType;
}
```

---

### 🟡 Priority #3: Operator Precedence Refinement
**الوقت / Time:** 3 ساعات / 3 hours  
**التعقيد / Complexity:** 🟡 Medium

#### المهمة / Task
مراجعة وتحسين ترتيب الأولويات لجميع العوامل (15 مستوى).

---

### 🟡 Priority #4: List/Dict Comprehensions
**الوقت / Time:** 5 ساعات / 5 hours  
**التعقيد / Complexity:** 🟡 Medium

#### Grammar المطلوبة / Required Grammar
```
list_comp ::= '[' expression 'for' IDENTIFIER 'in' expression ['if' expression] ']'
dict_comp ::= '{' expression ':' expression 'for' IDENTIFIER 'in' expression ['if' expression] '}'
```

---

### 🟢 Priority #5: Match Statement
**الوقت / Time:** 6 ساعات / 6 hours  
**التعقيد / Complexity:** 🟡 Medium

#### Grammar المطلوبة / Required Grammar
```
match_stmt ::= 'match' expression '{' case_clause+ '}'
case_clause ::= 'case' pattern ':' block
pattern ::= literal | IDENTIFIER | '_' | list_pattern | dict_pattern
```

---

## 📦 Phase 3B-2: Advanced Features (الأسبوع الثاني)

### المهام / Tasks
1. Async/Await Implementation
2. Decorator Support
3. With Statement Enhancement
4. Import Statement Enhancement
5. Error Recovery Improvements
6. Unicode Identifier Support

---

## 📦 Phase 3B-3: Optimization & Polish (الأسبوع الثالث)

### المهام / Tasks
1. Performance Optimization
2. Memory Management
3. Documentation Completion
4. Test Coverage to 90%
5. Code Quality Review
6. Benchmarking
7. Final Polish

---

## 📊 جدول زمني تفصيلي / Detailed Timeline

| الأسبوع | المهام | الوقت المقدر | الحالة |
|---------|--------|--------------|--------|
| Week 1 | Type System (5 items) | 24 hours | 📋 Planned |
| Week 2 | Advanced Features (6 items) | 30 hours | 📋 Planned |
| Week 3 | Optimization (7 items) | 26 hours | 📋 Planned |
| **Total** | **18 improvements** | **80 hours** | **~2 weeks** |

---

## ✅ معايير الإكمال / Completion Criteria

- [ ] جميع الـ18 تحسين منفذة
- [ ] Test coverage ≥ 90%
- [ ] Zero compilation warnings
- [ ] Documentation complete
- [ ] Performance benchmarks met
- [ ] Code review passed

---

**تم إعداد هذه الخطة في:** 2025-11-11  
**الحالة:** ✅ جاهزة للتنفيذ / Ready for Implementation
