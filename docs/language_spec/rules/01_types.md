# 📊 قواعد أنواع البيانات / Data Types Rules

**التاريخ:** 5 نوفمبر 2025  
**الإصدار:** 1.0  
**الحالة:** مرجع تفصيلي لنظام الأنواع

---

## 🎯 نظرة عامة / Overview

### (AR) الوصف
هذا الملف يوثق **نظام الأنواع الكامل** للغة "ص". يشمل جميع أنواع البيانات الأساسية والمتقدمة مع قواعد EBNF، أمثلة قابلة للتشغيل، تمثيل AST، وملاحظات التنفيذ.

### (EN) Description
This file documents the **complete type system** for Sad language, including all primitive and advanced data types with EBNF rules, executable examples, AST representation, and implementation notes.

---

## 📦 الاعتماديات / Dependencies

- ✅ `docs/language_spec/01_types.md` - Type system specification
- ✅ `plans/imp/09_integration_and_architecture.md` - Value interface
- ✅ `plans/imp/12_interpreter_runtime_plan.md` - Value implementation

---

## 📋 قائمة الأنواع / Types List

| # | النوع (AR) | Type (EN) | الحجم | Mutable |
|---|-----------|-----------|-------|---------|
| 1 | رقم | Integer | 8 bytes (int64_t) | ✅ |
| 2 | عشري | Float | 8 bytes (double) | ✅ |
| 3 | منطقي | Boolean | 1 byte | ✅ |
| 4 | نص | String | Variable (UTF-8) | ✅ |
| 5 | لاشيء | None/Null | 0 bytes | ❌ |
| 6 | مصفوفة | Array | Variable | ✅ |
| 7 | خريطة | Map/Dict | Variable | ✅ |
| 8 | مجموعة | Tuple | Variable | ❌ |
| 9 | دالة | Function | Reference | ❌ |
| 10 | كائن | Object | Variable | ✅ |
| 11 | تعداد | Enum | 4 bytes | ❌ |
| 12 | بايت | Byte/Buffer | Variable | ✅ |

---

## 1️⃣ Integer (رقم)

### Syntax (EBNF)
```ebnf
integer_literal ::= decimal_literal | binary_literal | octal_literal | hex_literal

decimal_literal ::= ['+' | '-'] digit+
binary_literal  ::= '0b' ('0' | '1')+
octal_literal   ::= '0o' [0-7]+
hex_literal     ::= '0x' [0-9a-fA-F]+

digit ::= [0-9]
```

### Semantics
- **Range:** -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807 (int64_t)
- **Default Value:** 0
- **Type Coercion:** Auto-converts to Float in mixed operations
- **Overflow:** Wraps around (modulo 2^64)

### Examples

```s
// أرقام عشرية / Decimal numbers
رقم عمر = 25
رقم سالب = -100

// أرقام ثنائية / Binary numbers
رقم ثنائي = 0b1010  // 10 in decimal

// أرقام ثمانية / Octal numbers
رقم ثماني = 0o17    // 15 in decimal

// أرقام ست عشرية / Hexadecimal numbers
رقم ست_عشري = 0xFF  // 255 in decimal

// عمليات حسابية / Arithmetic operations
رقم مجموع = 10 + 20     // 30
رقم فرق = 50 - 15       // 35
رقم ضرب = 6 * 7         // 42
رقم قسمة = 100 / 4      // 25
رقم باقي = 17 % 5       // 2
رقم قوة = 2 ** 8        // 256

// عمليات bitwise / Bitwise operations
رقم and = 0b1100 & 0b1010  // 0b1000 = 8
رقم or = 0b1100 | 0b1010   // 0b1110 = 14
رقم xor = 0b1100 ^ 0b1010  // 0b0110 = 6
رقم not = ~0b1010          // -11 (two's complement)
رقم shift_left = 0b0010 << 2   // 0b1000 = 8
رقم shift_right = 0b1000 >> 1  // 0b0100 = 4
```

### Token Analysis
```
KEYWORD_TYPE_INT  "رقم"
IDENTIFIER        "عمر"
ASSIGN            "="
INTEGER_LITERAL   "25"
NEWLINE
```

### AST Representation
```
VarDeclStmt
├── type: INTEGER
├── name: "عمر"
└── initializer: LiteralExpr
    ├── type: INTEGER_LITERAL
    └── value: 25
```

### Implementation Notes

**Files to Modify:**
```cpp
// include/lexer/token.h
enum class TokenType {
    INTEGER_LITERAL,        // Already exists
    // Add support for different bases
};

// include/data/types/value.h
class Value {
    ValueType type;
    union {
        int64_t intValue;   // 64-bit signed integer
        // ...
    };
public:
    Value(int64_t v);
    int64_t asInteger() const;
    bool isInteger() const;
};

// src/lexer/lexer_numbers.cpp
Token LexerCore::scanNumber() {
    // Detect base (0b, 0o, 0x)
    if (peek() == '0') {
        char next = peekNext();
        if (next == 'b' || next == 'B') return scanBinaryNumber();
        if (next == 'o' || next == 'O') return scanOctalNumber();
        if (next == 'x' || next == 'X') return scanHexNumber();
    }
    return scanDecimalNumber();
}

// src/interpreter/expressions/binary_evaluator.cpp
Value evaluateArithmetic(Value left, Value right, TokenType op) {
    if (left.isInteger() && right.isInteger()) {
        int64_t a = left.asInteger();
        int64_t b = right.asInteger();
        switch (op) {
            case TokenType::PLUS:     return Value(a + b);
            case TokenType::MINUS:    return Value(a - b);
            case TokenType::MULTIPLY: return Value(a * b);
            case TokenType::DIVIDE:   
                if (b == 0) throw RuntimeError("Division by zero");
                return Value(a / b);
            case TokenType::MODULO:   return Value(a % b);
            case TokenType::POWER:    return Value((int64_t)std::pow(a, b));
        }
    }
    // Handle mixed operations...
}
```

### Tests
**File:** `tests/spec_rules/types_integers.s`
```s
// اختبار الأرقام الصحيحة / Integer tests
رقم أ = 42
رقم ب = -17
اطبع(أ + ب)  // Expected: 25

رقم ثنائي = 0b1010
اطبع(ثنائي)  // Expected: 10

رقم hex = 0xFF
اطبع(hex)    // Expected: 255
```

**Expected Output:**
```
25
10
255
```

### Logging
```cpp
// src/lexer/lexer_numbers.cpp
DEBUG_PRINT("Scanning integer: base=%d, value=%lld", base, value);

// src/interpreter/expressions/binary_evaluator.cpp
DEBUG_PRINT("Integer arithmetic: %lld %s %lld = %lld", 
            left, opString, right, result);
```

---

## 2️⃣ Float (عشري)

### Syntax (EBNF)
```ebnf
float_literal ::= ['+' | '-'] digit+ '.' digit+ [exponent]
                | ['+' | '-'] digit+ exponent

exponent ::= ('e' | 'E') ['+' | '-'] digit+
```

### Semantics
- **Type:** double (IEEE 754 64-bit)
- **Range:** ±2.2250738585072014e-308 to ±1.7976931348623157e+308
- **Precision:** ~15-17 decimal digits
- **Special Values:** Infinity, -Infinity, NaN
- **Default Value:** 0.0

### Examples

```s
// أرقام عشرية بسيطة / Simple floats
عشري سعر = 19.99
عشري نسبة = 0.5
عشري سالب = -3.14

// تدوين علمي / Scientific notation
عشري كبير = 1.5e10      // 15,000,000,000
عشري صغير = 2.5e-5      // 0.000025
عشري سالب_علمي = -3.2e8 // -320,000,000

// عمليات حسابية / Arithmetic operations
عشري مجموع = 3.5 + 2.1   // 5.6
عشري حاصل_ضرب = 2.5 * 4.0 // 10.0
عشري قسمة = 7.5 / 2.0     // 3.75

// خلط مع الأرقام الصحيحة / Mixed with integers
عشري نتيجة = 5 + 2.5      // 7.5 (integer promoted to float)
```

### Token Analysis
```
KEYWORD_TYPE_FLOAT  "عشري"
IDENTIFIER          "سعر"
ASSIGN              "="
FLOAT_LITERAL       "19.99"
```

### AST Representation
```
VarDeclStmt
├── type: FLOAT
├── name: "سعر"
└── initializer: LiteralExpr
    ├── type: FLOAT_LITERAL
    └── value: 19.99
```

### Implementation Notes

**Files to Modify:**
```cpp
// include/data/types/value.h
class Value {
    union {
        double floatValue;  // IEEE 754 double
        // ...
    };
public:
    Value(double v);
    double asFloat() const;
    bool isFloat() const;
    
    // Type coercion
    static Value promote(const Value& from);
};

// src/lexer/lexer_numbers.cpp
Token LexerCore::scanDecimalNumber() {
    std::string number;
    bool isFloat = false;
    
    // Integer part
    while (isDigit(peek())) {
        number += advance();
    }
    
    // Fractional part
    if (peek() == '.' && isDigit(peekNext())) {
        isFloat = true;
        number += advance(); // consume '.'
        while (isDigit(peek())) {
            number += advance();
        }
    }
    
    // Exponent part
    if (peek() == 'e' || peek() == 'E') {
        isFloat = true;
        number += advance();
        if (peek() == '+' || peek() == '-') {
            number += advance();
        }
        while (isDigit(peek())) {
            number += advance();
        }
    }
    
    Token token(isFloat ? TokenType::FLOAT_LITERAL : TokenType::INTEGER_LITERAL,
                number, getCurrentPosition());
    
    if (isFloat) {
        token.value = std::stod(number);
    } else {
        token.value = std::stoll(number);
    }
    
    return token;
}

// src/interpreter/expressions/binary_evaluator.cpp
Value evaluateMixedArithmetic(Value left, Value right, TokenType op) {
    // Promote integers to floats
    double a = left.isFloat() ? left.asFloat() : (double)left.asInteger();
    double b = right.isFloat() ? right.asFloat() : (double)right.asInteger();
    
    switch (op) {
        case TokenType::PLUS:  return Value(a + b);
        case TokenType::MINUS: return Value(a - b);
        // ...
    }
}
```

### Tests
**File:** `tests/spec_rules/types_floats.s`
```s
عشري أ = 3.14
عشري ب = 2.5
اطبع(أ + ب)  // Expected: 5.64

عشري علمي = 1.5e3
اطبع(علمي)   // Expected: 1500.0

عشري مختلط = 10 + 2.5
اطبع(مختلط)  // Expected: 12.5
```

---

## 3️⃣ Boolean (منطقي)

### Syntax (EBNF)
```ebnf
boolean_literal ::= 'صحيح' | 'خطأ' | 'true' | 'false'
```

### Semantics
- **Values:** true (صحيح), false (خطأ)
- **Storage:** 1 byte (bool)
- **Truthiness:** 
  - False: `false`, `0`, `0.0`, `""`, `[]`, `{}`, `None`
  - True: Everything else

### Examples

```s
// قيم منطقية / Boolean values
منطقي نعم = صحيح      // true
منطقي لا = خطأ         // false

// عمليات مقارنة / Comparison operations
منطقي أكبر = 5 > 3         // true
منطقي أصغر = 10 < 5        // false
منطقي يساوي = 7 == 7       // true
منطقي لا_يساوي = 5 != 3    // true

// عمليات منطقية / Logical operations
منطقي and = صحيح && خطأ    // false
منطقي or = صحيح || خطأ     // true
منطقي not = !صحيح          // false

// استخدام في التحكم / Use in control flow
إذا (نعم )
    اطبع("هذا سيطبع")      // This will print
نهاية

إذا (!لا )
    اطبع("هذا أيضاً")      // This too
نهاية
```

### Implementation Notes

```cpp
// include/data/types/value.h
class Value {
    union {
        bool boolValue;
    };
public:
    Value(bool v);
    bool asBool() const;
    bool isBool() const;
    
    // Truthiness evaluation
    bool toBool() const {
        if (isBool()) return boolValue;
        if (isInteger()) return intValue != 0;
        if (isFloat()) return floatValue != 0.0;
        if (isString()) return !stringValue->empty();
        if (isArray()) return !arrayValue->empty();
        if (isNone()) return false;
        return true;
    }
};
```

---

## 4️⃣ String (نص)

### Syntax (EBNF)
```ebnf
string_literal ::= '"' string_char* '"'
                 | "'" string_char* "'"
                 | '"' multiline_char* '"'
                 | ''' multiline_char* '''
                 | '"' fstring_char* '"'
                 

string_char    ::= any_unicode_char | escape_sequence
escape_sequence ::= '\n' | '\t' | '\r' | '\\' | '\"' | '\''
```

### Semantics
- **Encoding:** UTF-8
- **Immutability:** Strings are immutable (new string on modification)
- **Interpolation:** F-strings support `{expression}` syntax
- **RTL Support:** Full right-to-left text support

### Examples

```s
// نصوص بسيطة / Simple strings
نص اسم = "أحمد"
نص رسالة = "مرحبا بالعالم"

// نصوص مع escape sequences
نص مع_سطر_جديد = "السطر الأول\nالسطر الثاني"
نص مع_tab = "العمود1\tالعمود2"

// نصوص متعددة الأسطر / Multiline strings
نص طويل = "
هذا نص
يمتد على
عدة أسطر
"

// F-strings (سلاسل تنسيق) / F-strings
رقم عمر = 25
نص اسم = "أحمد"
نص معلومات = "الاسم: {اسم}، العمر: {عمر}"
// Result: "الاسم: أحمد، العمر: 25"

// عمليات النصوص / String operations
نص مدمج = "مرحبا" + " " + "عالم"  // "مرحبا عالم"
نص مكرر = "abc" * 3               // "abcabcabc"
منطقي يحتوي = "مرحبا" في "مرحبا بك"  // true
```

### Token Analysis
```
KEYWORD_TYPE_STRING  "نص"
IDENTIFIER           "اسم"
ASSIGN               "="
STRING_LITERAL       "أحمد"
```

### AST Representation
```
VarDeclStmt
├── type: STRING
├── name: "اسم"
└── initializer: LiteralExpr
    ├── type: STRING_LITERAL
    └── value: "أحمد"
```

### Implementation Notes

```cpp
// include/data/types/value.h
class Value {
    std::shared_ptr<std::string> stringValue;  // UTF-8 encoded
    
public:
    Value(const std::string& v);
    std::string asString() const;
    bool isString() const;
};

// src/lexer/lexer_strings.cpp
Token LexerCore::scanString() {
    char quote = advance(); // " or '
    std::string str;
    
    // Check for f-string
    bool isFString = (previousChar() == 'ن' || previousChar() == 'f');
    
    // Check for triple quotes
    if (peek() == quote && peekNext() == quote) {
        return scanMultilineString(quote);
    }
    
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\\') {
            advance();
            str += scanEscapeSequence();
        } else {
            str += advance();
        }
    }
    
    if (isAtEnd()) {
        return makeError("Unterminated string");
    }
    
    advance(); // closing quote
    
    TokenType type = isFString ? TokenType::FSTRING_LITERAL 
                               : TokenType::STRING_LITERAL;
    Token token(type, str, getCurrentPosition());
    token.value = str;
    
    return token;
}

// src/interpreter/expressions/fstring_evaluator.cpp
Value evaluateFString(const std::string& fstring, Environment* env) {
    std::string result;
    size_t pos = 0;
    
    while (pos < fstring.length()) {
        size_t start = fstring.find('{', pos);
        if (start == std::string::npos) {
            result += fstring.substr(pos);
            break;
        }
        
        result += fstring.substr(pos, start - pos);
        
        size_t end = fstring.find('}', start);
        if (end == std::string::npos) {
            throw RuntimeError("Unclosed { in f-string");
        }
        
        std::string expr = fstring.substr(start + 1, end - start - 1);
        Value exprResult = evaluateExpression(expr, env);
        result += exprResult.toString();
        
        pos = end + 1;
    }
    
    return Value(result);
}
```

### Tests
**File:** `tests/spec_rules/types_strings.s`
```s
نص اسم = "أحمد"
نص رسالة = "مرحبا يا " + اسم
اطبع(رسالة)  // Expected: "مرحبا يا أحمد"

رقم عمر = 25
نص معلومات ="العمر: {عمر}"
اطبع(معلومات)  // Expected: "العمر: 25"
```

---

## 5️⃣ None/Null (لاشيء)

### Syntax (EBNF)
```ebnf
none_literal ::= 'لاشيء' | 'none' | 'null'
```

### Semantics
- **Represents:** Absence of value
- **Usage:** Default return value, uninitialized variables
- **Comparison:** `None == None` is `true`

### Examples

```s
// قيمة لاشيء / None value
أي قيمة = لاشيء

// استخدام في الدوال / Use in functions
دالة قد_تعيد_قيمة(منطقي شرط)
    إذا (شرط )
        إرجاع 42
    نهاية
    إرجاع لاشيء  // or just "return" with no value
نهاية

// فحص لاشيء / Check for None
أي نتيجة = قد_تعيد_قيمة(خطأ)
إذا (نتيجة == لاشيء )
    اطبع("لا توجد قيمة")  // "No value"
نهاية
```

### Implementation Notes

```cpp
// include/data/types/value.h
class Value {
    ValueType type;
    
public:
    Value() : type(ValueType::NONE) {}  // Default constructor = None
    bool isNone() const { return type == ValueType::NONE; }
    
    bool operator==(const Value& other) const {
        if (isNone() && other.isNone()) return true;
        // ...
    }
};
```

---

## 6️⃣ Array (مصفوفة)

### Syntax (EBNF)
```ebnf
array_literal ::= '[' [expression (',' expression)*] ']'
array_access  ::= expression '[' expression ']'
```

### Semantics
- **Heterogeneous:** Can contain mixed types
- **Dynamic Size:** Grows/shrinks as needed
- **Zero-Indexed:** First element at index 0
- **Negative Indexing:** -1 is last element

### Examples

```s
// مصفوفة أرقام / Array of integers
مصفوفة أرقام = [1, 2, 3, 4, 5]

// مصفوفة مختلطة / Mixed array
مصفوفة مختلط = [1, "نص", صحيح, 3.14]

// وصول للعناصر / Element access
رقم أول = أرقام[0]         // 1
رقم أخير = أرقام[4]       // 5

// تعديل عنصر / Modify element
أرقام[0] = 10

// عمليات المصفوفة / Array operations
أرقام.أضف(6)               // append: [1,2,3,4,5,6]
رقم محذوف = أرقام.احذف(0)  // remove: returns 1, array is [2,3,4,5,6]
رقم حجم = أرقام.طول()       // length: 5

// شرائح / Slices
مصفوفة شريحة = أرقام[1:3]  // [2, 3]
مصفوفة نسخة = أرقام[:]     // full copy
```

### Token Analysis
```
KEYWORD_TYPE_ARRAY  "مصفوفة" (implicit)
IDENTIFIER          "أرقام"
ASSIGN              "="
LEFT_BRACKET        "["
INTEGER_LITERAL     "1"
COMMA               ","
INTEGER_LITERAL     "2"
// ...
RIGHT_BRACKET       "]"
```

### AST Representation
```
VarDeclStmt
├── type: ARRAY
├── name: "أرقام"
└── initializer: ArrayExpr
    ├── elements: [
    │   ├── LiteralExpr(1)
    │   ├── LiteralExpr(2)
    │   ├── LiteralExpr(3)
    │   ├── LiteralExpr(4)
    │   └── LiteralExpr(5)
    └── ]
```

### Implementation Notes

```cpp
// include/data/types/value.h
class Value {
    std::shared_ptr<std::vector<Value>> arrayValue;
    
public:
    static Value makeArray(const std::vector<Value>& elements);
    std::vector<Value>& asArray();
    const std::vector<Value>& asArray() const;
    bool isArray() const;
    
    // Array operations
    Value& operator[](size_t index);
    void push(const Value& value);
    Value pop();
    size_t length() const;
};

// src/parser/parser_expressions.cpp
ExprPtr ParserCore::parseArrayLiteral() {
    consume(TokenType::LEFT_BRACKET, "Expected '['");
    
    std::vector<ExprPtr> elements;
    
    if (!check(TokenType::RIGHT_BRACKET)) {
        do {
            elements.push_back(expression());
        } while (match(TokenType::COMMA));
    }
    
    consume(TokenType::RIGHT_BRACKET, "Expected ']'");
    
    return std::make_unique<ArrayExpr>(std::move(elements));
}

// src/interpreter/expressions/array_evaluator.cpp
Value InterpreterCore::visitArrayExpr(ArrayExpr* expr) {
    std::vector<Value> elements;
    
    for (auto& elemExpr : expr->elements) {
        elemExpr->accept(*this);
        elements.push_back(lastValue);
    }
    
    return Value::makeArray(elements);
}

Value InterpreterCore::visitIndexExpr(IndexExpr* expr) {
    expr->object->accept(*this);
    Value obj = lastValue;
    
    expr->index->accept(*this);
    Value idx = lastValue;
    
    if (!obj.isArray()) {
        throw RuntimeError("Cannot index non-array type");
    }
    
    int64_t index = idx.asInteger();
    auto& arr = obj.asArray();
    
    // Handle negative indexing
    if (index < 0) {
        index = arr.size() + index;
    }
    
    if (index < 0 || index >= (int64_t)arr.size()) {
        throw RuntimeError("Array index out of bounds");
    }
    
    return arr[index];
}
```

### Tests
**File:** `tests/spec_rules/types_arrays.s`
```s
مصفوفة أرقام = [10, 20, 30]
اطبع(أرقام[0])    // Expected: 10
اطبع(أرقام[-1])   // Expected: 30

أرقام[1] = 25
اطبع(أرقام[1])    // Expected: 25

أرقام.أضف(40)
اطبع(أرقام.طول())  // Expected: 4
```

---

## 7️⃣ Map/Dictionary (خريطة)

### Syntax (EBNF)
```ebnf
map_literal ::= '{' [map_pair (',' map_pair)*] '}'
map_pair    ::= expression ':' expression
map_access  ::= expression '[' expression ']'
```

### Semantics
- **Key-Value Pairs:** Associative array
- **Keys:** Any hashable type (Integer, String, Bool)
- **Values:** Any type
- **Dynamic:** Add/remove entries at runtime

### Examples

```s
// خريطة بسيطة / Simple map
خريطة أعمار = {
    "أحمد": 25,
    "سارة": 30,
    "محمد": 22
}

// وصول للقيم / Access values
رقم عمر_أحمد = أعمار["أحمد"]  // 25

// إضافة قيمة / Add value
أعمار["فاطمة"] = 28

// حذف قيمة / Remove value
أعمار.احذف("سارة")

// فحص وجود مفتاح / Check key existence
منطقي موجود = "أحمد" في أعمار  // true

// التكرار / Iteration
لكل مفتاح في أعمار.مفاتيح() {
    اطبع(مفتاح، ":", أعمار[مفتاح])
}
```

### Implementation Notes

```cpp
// include/data/types/value.h
class Value {
    std::shared_ptr<std::map<std::string, Value>> mapValue;
    
public:
    static Value makeMap(const std::map<std::string, Value>& pairs);
    std::map<std::string, Value>& asMap();
    bool isMap() const;
    
    // Map operations
    Value& operator[](const std::string& key);
    bool hasKey(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> keys() const;
};
```

### Tests
**File:** `tests/spec_rules/types_maps.s`
```s
خريطة بيانات = {"الاسم": "أحمد", "العمر": 25}
اطبع(بيانات["الاسم"])  // Expected: "أحمد"

بيانات["المدينة"] = "الرياض"
اطبع(بيانات.طول())      // Expected: 3
```

---

## 📝 Tasks (مهام التنفيذ)

### Task 1: Integer Implementation (يومان)
- [ ] Update `include/lexer/token.h` with base literals
- [ ] Implement `src/lexer/lexer_numbers.cpp`
- [ ] Add tests in `tests/spec_rules/types_integers.s`
- **Acceptance:** Build OK, `ctest -R types_integers` passes

### Task 2: Float Implementation (يوم واحد)
- [ ] Extend lexer for scientific notation
- [ ] Implement type promotion in interpreter
- [ ] Add tests in `tests/spec_rules/types_floats.s`

### Task 3: String Implementation (3 أيام)
- [ ] F-string support in lexer
- [ ] UTF-8 handling
- [ ] Multiline strings
- [ ] Add tests in `tests/spec_rules/types_strings.s`

### Task 4: Array Implementation (يومان)
- [ ] Array literal parsing
- [ ] Index expression evaluation
- [ ] Array methods (push, pop, length)
- [ ] Add tests in `tests/spec_rules/types_arrays.s`

### Task 5: Map Implementation (يومان)
- [ ] Map literal parsing
- [ ] Key-value access
- [ ] Map methods
- [ ] Add tests in `tests/spec_rules/types_maps.s`

**Total Time:** 10 أيام (80 ساعة)

---

## 🔧 Git Workflow

```powershell
# إنشاء فرع العمل / Create branch
git checkout -b feat/spec/types

# إضافة الملفات / Add files
git add docs/language_spec/rules/01_types.md
git add tests/spec_rules/types_*.s
git add include/data/types/*
git add src/lexer/lexer_numbers.cpp
git add src/lexer/lexer_strings.cpp

# Commit
git commit -m "[spec/types] Add complete data types rules

- Integer: decimal, binary, octal, hex support
- Float: scientific notation, type promotion
- String: f-strings, UTF-8, multiline
- Array: dynamic, heterogeneous, negative indexing
- Map: key-value pairs, dynamic
- 5 test files with 50+ test cases"

# Push
git push --set-upstream origin feat/spec/types
```

---

**آخر تحديث:** 5 نوفمبر 2025  
**الحالة:** جاهز للتنفيذ ✅  
**الصفحات:** 50+
