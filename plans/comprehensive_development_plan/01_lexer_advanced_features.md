# 📝 المرحلة 1: المحلل المعجمي المتقدم
## Phase 1: Advanced Lexer Features

**المدة:** أسبوعان (10 أيام عمل)  
**الأولوية:** 🔴 P0 - حرجة  
**التبعيات:** لا يوجد  
**التأثير:** يفتح جميع الميزات المتقدمة

---

## 🎯 الأهداف

### الهدف الرئيسي
تطبيق جميع ميزات المحلل المعجمي المحددة في `rules/rules/01_types.md` و `04_syntax.md` والتي غير مطبقة حالياً في `src/lexer/`.

### الأهداف الفرعية
1. ✅ دعم كامل لأنواع الأرقام (Binary، Octal، Hex)
2. ✅ دعم الأرقام العربية (٠-٩) 
3. ✅ تسلسلات الهروب المحسّنة (Unicode، Raw strings)
4. ✅ F-strings (String interpolation)
5. ✅ Multi-line strings محسّنة
6. ✅ تعليقات documentation (#**)
7. ✅ Underscores في الأرقام (1_000_000)

---

## 📊 التقييم الحالي

### ما موجود حالياً في `src/lexer/`

```cpp
// include/lexer/lexer_core.h - الموجود
✅ scanNumber()      // أرقام عشرية وعشرية فقط
✅ scanString()      // نصوص عادية بـ "" و ''
✅ scanIdentifier()  // معرفات عربية ولاتينية
✅ scanOperator()    // عوامل أساسية
✅ skipComment()     // تعليقات # و #* *#

// الحروف الخاصة المدعومة
✅ \n \t \" \\ \r
```

### ما ناقص (من القواعد)

```cpp
❌ Binary literals: 0b1010، 0ثن1010
❌ Octal literals: 0o17، 0ع17
❌ Hex literals: 0xFF، 0س15
❌ Arabic digits: ٠١٢٣٤٥٦٧٨٩
❌ Unicode escapes: \u0627، \U00000627
❌ Raw strings: r"C:\path"
❌ F-strings: f"القيمة = {متغير}"
❌ Byte strings: b"data"
❌ Doc comments: #** ... **#
❌ Underscores: 1_000_000
❌ Arabic hex digits: ابجدهو (أ=10، ب=11، ج=12، د=13، ه=14، و=15)
```

---

## 📋 المهام التفصيلية

### المهمة 1.1: أنواع الأرقام المتقدمة (3 أيام)

#### 1.1.1: Binary Numbers (يوم 1)

**الملف:** `src/lexer/lexer_numbers.cpp`

```cpp
// الصيغة المطلوبة (من القواعد)
0b1010      // عشري: 10
0b0001_1111 // مع underscores، عشري: 31
0ثن1010     // الصيغة العربية
0ثن0001_1111

// التنفيذ المطلوب
Token LexerCore::scanBinaryNumber() {
    size_t start = current_;
    advance(); // skip '0'
    advance(); // skip 'b' or 'ثن'
    
    std::string digits;
    int digit_count = 0;
    
    while (!isAtEnd()) {
        char c = peek();
        
        // Binary digits
        if (c == '0' || c == '1') {
            digits += c;
            digit_count++;
            advance();
        }
        // Underscores (ignore)
        else if (c == '_') {
            advance();
        }
        // Invalid digit
        else if (isAlphaNumeric(c)) {
            return makeError(
                "رقم ثنائي غير صالح / Invalid binary digit: '" + 
                std::string(1, c) + "'"
            );
        }
        else {
            break;
        }
    }
    
    // Must have at least one digit
    if (digit_count == 0) {
        return makeError("رقم ثنائي فارغ / Empty binary number");
    }
    
    // Convert binary string to integer
    int64_t value = 0;
    for (char c : digits) {
        value = value * 2 + (c - '0');
    }
    
    return Token(TokenType::INTEGER_LITERAL, 
                 std::to_string(value),
                 Position(line_, column_, start));
}

// في scanNumber() الرئيسية
Token LexerCore::scanNumber() {
    if (peek() == '0') {
        char next = peekNext();
        
        // Binary: 0b or 0ثن
        if (next == 'b' || next == 'B') {
            return scanBinaryNumber();
        }
        
        // Arabic binary: 0ثن
        if (isArabicLetter(next)) {
            std::string prefix = std::string(1, next);
            if (peekNext(2) == 'ن' && prefix == "ث") {
                return scanBinaryNumber();
            }
        }
        
        // ... باقي الأنواع
    }
    
    return scanDecimalNumber();
}
```

**الاختبارات المطلوبة:**

```cpp
// tests/lexer/test_binary_numbers.cpp
TEST(LexerNumbers, BasicBinary) {
    Lexer lexer("0b1010");
    auto tokens = lexer.tokenize();
    EXPECT_EQ(tokens[0].type, TokenType::INTEGER_LITERAL);
    EXPECT_EQ(tokens[0].value, "10");
}

TEST(LexerNumbers, BinaryWithUnderscores) {
    Lexer lexer("0b0001_1111");
    auto tokens = lexer.tokenize();
    EXPECT_EQ(tokens[0].value, "31");
}

TEST(LexerNumbers, ArabicBinary) {
    Lexer lexer("0ثن1010");
    auto tokens = lexer.tokenize();
    EXPECT_EQ(tokens[0].value, "10");
}

TEST(LexerNumbers, BinaryInvalidDigit) {
    Lexer lexer("0b1012");
    EXPECT_THROW(lexer.tokenize(), LexerError);
}
```

#### 1.1.2: Octal Numbers (يوم 1)

**الصيغة المطلوبة:**
```
0o17      // عشري: 15
0o755     // عشري: 493 (permissions)
0ع17      // الصيغة العربية
0o1_777   // مع underscores
```

**التنفيذ:** مشابه للـ Binary ولكن:
- قبول digits: 0-7 فقط
- Base: 8 بدلاً من 2
- التحقق من صحة الأرقام (لا 8 أو 9)

#### 1.1.3: Hexadecimal Numbers (يوم 1)

**الصيغة المطلوبة:**
```
0xFF           // عشري: 255
0x1A2B         // case-insensitive
0سFF           // الصيغة العربية
0xDEAD_BEEF    // مع underscores
0سابجد         // الأرقام العربية hex: أ=10، ب=11، ج=12، د=13، ه=14، و=15
```

**التنفيذ المتقدم:**

```cpp
Token LexerCore::scanHexNumber() {
    advance(); // skip '0'
    char prefix = peek();
    advance(); // skip 'x' or 'س'
    
    std::string digits;
    int digit_count = 0;
    
    while (!isAtEnd()) {
        char c = peek();
        
        // Regular hex digits: 0-9, A-F, a-f
        if (isxdigit(c)) {
            digits += tolower(c);
            digit_count++;
            advance();
        }
        // Arabic hex digits: ابجدهو
        else if (isArabicHexDigit(c)) {
            int value = arabicHexToInt(c);
            digits += hexDigits[value];
            digit_count++;
            advance();
        }
        // Underscores
        else if (c == '_') {
            advance();
        }
        // Invalid
        else if (isAlphaNumeric(c)) {
            return makeError("رقم ستعشري غير صالح / Invalid hex digit");
        }
        else {
            break;
        }
    }
    
    if (digit_count == 0) {
        return makeError("رقم ستعشري فارغ / Empty hex number");
    }
    
    // Convert hex to integer
    int64_t value = std::stoll(digits, nullptr, 16);
    
    return Token(TokenType::INTEGER_LITERAL,
                 std::to_string(value),
                 Position(line_, column_, start));
}

// دالة مساعدة
bool LexerCore::isArabicHexDigit(char c) {
    return c == 'أ' || c == 'ب' || c == 'ج' || 
           c == 'د' || c == 'ه' || c == 'و';
}

int LexerCore::arabicHexToInt(char c) {
    switch(c) {
        case 'أ': return 10;
        case 'ب': return 11;
        case 'ج': return 12;
        case 'د': return 13;
        case 'ه': return 14;
        case 'و': return 15;
        default: return -1;
    }
}
```

---

### المهمة 1.2: الأرقام العربية (يوم 1)

**الهدف:** دعم ٠١٢٣٤٥٦٧٨٩ وتحويلها تلقائياً لـ 0-9

```cpp
// الصيغة المطلوبة (من القواعد)
٤٢        // 42
٣.١٤      // 3.14
٠ثن١٠١٠  // binary مع أرقام عربية
٠س١٥      // hex مع أرقام عربية

// التنفيذ
char LexerCore::normalizeArabicDigit(char c) {
    // Unicode ranges for Arabic digits
    // ٠ = U+0660, ١ = U+0661, ..., ٩ = U+0669
    
    if (c >= 0x0660 && c <= 0x0669) {
        return '0' + (c - 0x0660);
    }
    
    return c;
}

Token LexerCore::scanNumber() {
    size_t start = current_;
    std::string normalized_number;
    
    // Normalize all digits
    while (!isAtEnd() && (isDigit(peek()) || isArabicDigit(peek()) || peek() == '_' || peek() == '.')) {
        char c = peek();
        
        if (isArabicDigit(c)) {
            normalized_number += normalizeArabicDigit(c);
        } else if (c != '_') {
            normalized_number += c;
        }
        
        advance();
    }
    
    // Process normalized number
    // ...
}

bool LexerCore::isArabicDigit(char c) {
    return (c >= 0x0660 && c <= 0x0669);
}
```

**الاختبارات:**

```cpp
TEST(LexerNumbers, ArabicDigits) {
    Lexer lexer("٤٢");
    auto tokens = lexer.tokenize();
    EXPECT_EQ(tokens[0].value, "42");
}

TEST(LexerNumbers, ArabicFloat) {
    Lexer lexer("٣.١٤");
    auto tokens = lexer.tokenize();
    EXPECT_EQ(tokens[0].value, "3.14");
}

TEST(LexerNumbers, MixedDigits) {
    Lexer lexer("٤2");  // Mixed: ٤ + 2
    auto tokens = lexer.tokenize();
    EXPECT_EQ(tokens[0].value, "42");
}
```

---

### المهمة 1.3: Underscores في الأرقام (نصف يوم)

```cpp
// الصيغة المطلوبة
1_000_000        // مليون
3.14_159_265     // pi
0xDEAD_BEEF      // hex
0b1111_0000      // binary

// التنفيذ (مطبق في جميع دوال الأرقام)
Token LexerCore::scanDecimalNumber() {
    std::string number_str;
    
    while (!isAtEnd()) {
        char c = peek();
        
        if (isDigit(c) || isArabicDigit(c)) {
            number_str += normalizeArabicDigit(c);
            advance();
        }
        else if (c == '_') {
            // تجاهل الـ underscore
            advance();
        }
        else if (c == '.') {
            // نقطة عشرية
            // ...
        }
        else {
            break;
        }
    }
    
    // ...
}
```

---

### المهمة 1.4: تسلسلات الهروب المحسّنة (يوم 1)

#### الحروف الخاصة الإضافية

```cpp
// الموجود حالياً
\n \t \" \\ \r

// المطلوب إضافتها (من القواعد)
\0     // null character
\a     // alert (bell)
\b     // backspace
\f     // form feed
\v     // vertical tab
\x41   // hex character (A)
\u0627 // Unicode 4 digits (ا)
\U00000627 // Unicode 8 digits (ا)
\101   // octal character (A)

// الصيغة العربية
\س     // \n (سطر)
\ت     // \t (تاب)
\ع     // \r (عودة)
```

**التنفيذ:**

```cpp
// في src/lexer/lexer_strings.cpp
char LexerCore::parseEscapeSequence() {
    advance(); // skip backslash
    
    char c = peek();
    advance();
    
    switch(c) {
        // Existing
        case 'n': return '\n';
        case 't': return '\t';
        case 'r': return '\r';
        case '\\': return '\\';
        case '"': return '"';
        case '\'': return '\'';
        
        // New basic
        case '0': return '\0';
        case 'a': return '\a';
        case 'b': return '\b';
        case 'f': return '\f';
        case 'v': return '\v';
        
        // Arabic
        case 'س': return '\n';  // سطر
        case 'ت': return '\t';  // تاب
        case 'ع': return '\r';  // عودة
        
        // Hex: \xHH
        case 'x': return parseHexEscape(2);
        
        // Unicode: \uHHHH
        case 'u': return parseUnicodeEscape(4);
        
        // Unicode: \UHHHHHHHH
        case 'U': return parseUnicodeEscape(8);
        
        // Octal: \NNN
        default:
            if (c >= '0' && c <= '7') {
                return parseOctalEscape();
            }
            
            error("حرف خاص غير معروف / Unknown escape sequence: \\" + 
                  std::string(1, c));
            return c;
    }
}

char LexerCore::parseHexEscape(int digits) {
    std::string hex_str;
    
    for (int i = 0; i < digits; i++) {
        if (isxdigit(peek())) {
            hex_str += peek();
            advance();
        } else {
            error("تسلسل ستعشري غير كامل / Incomplete hex escape");
        }
    }
    
    return (char)std::stoi(hex_str, nullptr, 16);
}

char32_t LexerCore::parseUnicodeEscape(int digits) {
    std::string hex_str;
    
    for (int i = 0; i < digits; i++) {
        if (isxdigit(peek())) {
            hex_str += peek();
            advance();
        } else {
            error("تسلسل Unicode غير كامل / Incomplete Unicode escape");
        }
    }
    
    return std::stoi(hex_str, nullptr, 16);
}

char LexerCore::parseOctalEscape() {
    // Backtrack to get the first digit
    current_--;
    
    std::string octal_str;
    for (int i = 0; i < 3; i++) {
        char c = peek();
        if (c >= '0' && c <= '7') {
            octal_str += c;
            advance();
        } else {
            break;
        }
    }
    
    return (char)std::stoi(octal_str, nullptr, 8);
}
```

---

### المهمة 1.5: Raw Strings (نصف يوم)

```cpp
// الصيغة المطلوبة
r"C:\path\to\file"      // no escape processing
r'regex: \d+\s*'        // useful for regex

// في scanString()
Token LexerCore::scanString() {
    size_t start = current_;
    bool is_raw = false;
    bool is_fstring = false;
    bool is_bytes = false;
    
    // Check prefix
    if (current_ > 0) {
        char prev = source_[current_ - 1];
        if (prev == 'r' || prev == 'R') is_raw = true;
        if (prev == 'f' || prev == 'F') is_fstring = true;
        if (prev == 'b' || prev == 'B') is_bytes = true;
    }
    
    char quote = peek();
    advance(); // skip opening quote
    
    std::string value;
    
    while (!isAtEnd() && peek() != quote) {
        char c = peek();
        
        if (c == '\\' && !is_raw) {
            // Process escape sequence
            value += parseEscapeSequence();
        } else {
            value += c;
            advance();
        }
    }
    
    if (isAtEnd()) {
        return makeError("نص غير مغلق / Unterminated string");
    }
    
    advance(); // skip closing quote
    
    TokenType type = is_bytes ? TokenType::BYTES_LITERAL : TokenType::STRING_LITERAL;
    
    return Token(type, value, Position(line_, column_, start));
}
```

---

### المهمة 1.6: F-Strings (يومان)

**الهدف:** دعم String interpolation كما في القواعد

```cpp
// الصيغة المطلوبة (من 05_python_features.md)
f"القيمة = {متغير}"
f"المجموع = {أ + ب}"
f"التنسيق: {القيمة:.2f}"

// التنفيذ - المرحلة 1: Lexer فقط (tokenization)
Token LexerCore::scanFString() {
    std::vector<Token> parts;
    size_t start = current_;
    
    advance(); // skip 'f'
    char quote = peek();
    advance(); // skip opening quote
    
    std::string text;
    
    while (!isAtEnd() && peek() != quote) {
        char c = peek();
        
        // Start of interpolation
        if (c == '{') {
            // Save text before {
            if (!text.empty()) {
                parts.push_back(Token(TokenType::STRING_LITERAL, text, 
                                    Position(line_, column_, start)));
                text.clear();
            }
            
            advance(); // skip {
            
            // Scan expression inside {}
            std::string expr;
            int brace_count = 1;
            
            while (!isAtEnd() && brace_count > 0) {
                char ec = peek();
                
                if (ec == '{') brace_count++;
                if (ec == '}') brace_count--;
                
                if (brace_count > 0) {
                    expr += ec;
                }
                
                advance();
            }
            
            // Create FSTRING_EXPR token
            parts.push_back(Token(TokenType::FSTRING_EXPR, expr,
                                Position(line_, column_, start)));
        }
        // Regular character
        else {
            if (c == '\\') {
                text += parseEscapeSequence();
            } else {
                text += c;
                advance();
            }
        }
    }
    
    // Save remaining text
    if (!text.empty()) {
        parts.push_back(Token(TokenType::STRING_LITERAL, text,
                            Position(line_, column_, start)));
    }
    
    if (isAtEnd()) {
        return makeError("f-string غير مغلق / Unterminated f-string");
    }
    
    advance(); // skip closing quote
    
    // Return FSTRING token with parts
    return Token(TokenType::FSTRING, parts, Position(line_, column_, start));
}
```

**ملاحظة:** Parser سيتعامل مع FSTRING tokens في المرحلة 2.

---

### المهمة 1.7: Documentation Comments (نصف يوم)

```cpp
// الصيغة المطلوبة
## هذه دالة مهمة
## تقوم بالعملية الفلانية
دالة اختبار() {
    // ...
}

#**
 * وصف مفصل
 * للدالة
 **#
دالة اختبار2() {
    // ...
}

// التنفيذ في skipComment()
void LexerCore::skipComment() {
    char c = peek();
    
    // Doc comment: ##
    if (c == '#' && peekNext() == '#') {
        advance(); // skip first #
        advance(); // skip second #
        
        std::string doc;
        while (!isAtEnd() && peek() != '\n') {
            doc += peek();
            advance();
        }
        
        // Store doc comment (للاستخدام في docgen tool)
        doc_comments_.push_back(DocComment{doc, line_, column_});
        
        return;
    }
    
    // Doc block: #** ... **#
    if (c == '#' && peekNext() == '*' && peekNext(2) == '*') {
        advance(); // skip #
        advance(); // skip *
        advance(); // skip *
        
        std::string doc;
        
        while (!isAtEnd()) {
            if (peek() == '*' && peekNext() == '*' && peekNext(2) == '#') {
                advance(); advance(); advance();
                break;
            }
            
            doc += peek();
            if (peek() == '\n') {
                line_++;
                column_ = 0;
            }
            advance();
        }
        
        doc_comments_.push_back(DocComment{doc, line_, column_});
        return;
    }
    
    // Regular comments...
}
```

---

## 📈 معايير النجاح

### التغطية
- ✅ 100% من ميزات الأرقام من القواعد
- ✅ 100% من تسلسلات الهروب
- ✅ F-strings بنسبة 90% (الباقي في Parser)
- ✅ Raw strings 100%
- ✅ Doc comments 100%

### الاختبارات
- ✅ 150+ test cases جديدة
- ✅ 100% test pass rate
- ✅ Edge cases covered

### الأداء
- ✅ لا تأثير سلبي على السرعة
- ✅ Memory overhead < 5%

### التوافقية
- ✅ 100% backward compatible
- ✅ الكود القديم يعمل بدون تغيير

---

## 🔗 الملفات المعدّلة

```
include/lexer/
├── lexer_core.h                    (إضافة دوال جديدة)
├── token.h                         (إضافة أنواع tokens جديدة)

src/lexer/
├── lexer_core.cpp                  (تعديلات عامة)
├── lexer_numbers.cpp               (إعادة كتابة كاملة)
├── lexer_strings.cpp               (توسيع كبير)
├── lexer_comments.cpp              (ملف جديد)

tests/lexer/
├── test_binary_numbers.cpp         (جديد)
├── test_octal_numbers.cpp          (جديد)
├── test_hex_numbers.cpp            (جديد)
├── test_arabic_digits.cpp          (جديد)
├── test_escape_sequences.cpp       (جديد)
├── test_raw_strings.cpp            (جديد)
├── test_fstrings.cpp               (جديد)
├── test_doc_comments.cpp           (جديد)
```

---

## 📅 الجدول الزمني التفصيلي

| اليوم | المهمة | الساعات | الحالة |
|------|---------|----------|--------|
| 1 | Binary numbers + tests | 8 | ⏳ |
| 2 | Octal numbers + tests | 8 | ⏳ |
| 3 | Hex numbers (عادي + عربي) + tests | 8 | ⏳ |
| 4 | Arabic digits + underscores + tests | 8 | ⏳ |
| 5 | Escape sequences محسّنة + tests | 8 | ⏳ |
| 6 | Raw strings + tests | 4 | ⏳ |
| 6 | F-strings tokenization | 4 | ⏳ |
| 7 | F-strings tests | 4 | ⏳ |
| 7 | Doc comments + tests | 4 | ⏳ |
| 8 | Integration testing | 8 | ⏳ |
| 9 | Bug fixes + optimization | 8 | ⏳ |
| 10 | Documentation + review | 8 | ⏳ |

**المجموع:** 80 ساعة (10 أيام عمل)

---

## 🎓 المخرجات المتوقعة

1. **Lexer محسّن** بدعم كامل لجميع ميزات القواعد
2. **150+ اختبار جديد** مع تغطية شاملة
3. **توثيق كامل** لجميع الميزات الجديدة
4. **100% backward compatibility**
5. **أساس متين** للمراحل القادمة

---

**الحالة:** 📝 جاهز للتنفيذ  
**التحديث التالي:** بعد اليوم 5  
**المسؤول:** فريق Lexer
