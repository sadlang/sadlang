# 🔗 المرحلة 2: التكامل مع Lexer و Parser
## Phase 2: Integration with Lexer and Parser

<div dir="rtl">

## 📋 نظرة عامة / Overview

**المدة المتوقعة**: 3-7 أيام  
**الأولوية**: عالية (P0)  
**التبعيات**: المرحلة 1 (هياكل البيانات)

### الأهداف
- تكامل ErrorManager مع المحلل المعجمي (Lexer)
- تكامل ErrorManager مع المحلل النحوي (Parser)
- إضافة تتبع المواقع في كل مرحلة
- الحفاظ على السلوك الحالي مع تحسينات
- استبدال `std::vector<std::string> errors_` بـ ErrorManager

---

## 🎯 المهام التفصيلية / Detailed Tasks

### المهمة 2.1: تعديل Lexer لاستخدام ErrorManager ✅

**الملفات المتأثرة**:
- `src/lexer/lexer.cpp`
- `include/lexer/lexer.h`

#### التغييرات المطلوبة:

**أ) إضافة تتبع المواقع في Token:**

```cpp
// في include/lexer/token.h
struct Token {
    TokenType type;
    std::string lexeme;
    std::any literal;
    size_t line;        // موجود
    size_t column;      // إضافة جديدة ✅
    size_t offset;      // إضافة جديدة ✅
    size_t length;      // إضافة جديدة ✅
    
    /**
     * @brief (AR) يحول Token إلى SourceLocation
     *        (EN) Converts Token to SourceLocation
     */
    SourceLocation toLocation(const std::string& filename) const {
        return SourceLocation(filename, line, column, offset, length);
    }
};
```

**ب) تعديل Lexer لتتبع العمود:**

```cpp
// في src/lexer/lexer.cpp

class Lexer {
private:
    std::string source_;
    std::string filename_;     // إضافة ✅
    size_t start_ = 0;
    size_t current_ = 0;
    size_t line_ = 1;
    size_t column_ = 1;        // إضافة ✅
    size_t lineStart_ = 0;     // إضافة ✅ - موقع بداية السطر
    
    /**
     * @brief (AR) يتقدم حرف واحد ويحدث العمود
     *        (EN) Advances one character and updates column
     */
    char advance() {
        column_++;
        return source_[current_++];
    }
    
    /**
     * @brief (AR) ينشئ Token مع معلومات الموقع الكاملة
     *        (EN) Creates Token with complete location info
     */
    Token makeToken(TokenType type) {
        Token token;
        token.type = type;
        token.lexeme = source_.substr(start_, current_ - start_);
        token.line = line_;
        token.column = start_ - lineStart_ + 1;  // ✅ حساب العمود
        token.offset = start_;                     // ✅ الإزاحة
        token.length = current_ - start_;          // ✅ الطول
        return token;
    }
    
    /**
     * @brief (AR) يتعامل مع سطر جديد
     *        (EN) Handles new line
     */
    void handleNewline() {
        line_++;
        column_ = 1;
        lineStart_ = current_;  // ✅ تحديث بداية السطر
    }
};
```

**ج) استبدال الاستثناءات بـ ErrorManager:**

**قبل (Old):**
```cpp
if (!isValidCharacter(c)) {
    throw std::runtime_error("Invalid character: " + std::string(1, c));
}
```

**بعد (New):**
```cpp
if (!isValidCharacter(c)) {
    SourceLocation loc = getCurrentLocation();
    ErrorManager::getInstance().reportError(
        ErrorCode::LEX_INVALID_CHARACTER,
        loc,
        "رمز غير صالح: '" + std::string(1, c) + "'",
        "Invalid character: '" + std::string(1, c) + "'"
    );
    advance();  // تخطي الحرف والمتابعة (Error Recovery)
}
```

**د) أمثلة على الأخطاء المعجمية:**

```cpp
/**
 * @brief (AR) يتعامل مع نص غير مغلق
 *        (EN) Handles unterminated string
 */
Token Lexer::scanString() {
    SourceLocation start = getCurrentLocation();
    
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\n') handleNewline();
        advance();
    }
    
    if (isAtEnd()) {
        // (AR) خطأ: نص غير مغلق
        // (EN) Error: Unterminated string
        ErrorManager::getInstance().reportError(
            ErrorCode::LEX_UNTERMINATED_STRING,
            start,
            "نص غير مغلق - متوقع '\"' في النهاية",
            "Unterminated string - expected '\"' at the end"
        );
        return errorToken();
    }
    
    advance(); // Closing "
    return makeToken(TokenType::STRING);
}

/**
 * @brief (AR) يتعامل مع رقم غير صالح
 *        (EN) Handles invalid number
 */
Token Lexer::scanNumber() {
    while (isDigit(peek())) advance();
    
    // Look for decimal
    if (peek() == '.' && isDigit(peekNext())) {
        advance(); // Consume '.'
        while (isDigit(peek())) advance();
    }
    
    // (AR) تحقق من تنسيق صحيح
    // (EN) Verify valid format
    std::string numStr = source_.substr(start_, current_ - start_);
    
    try {
        double value = std::stod(numStr);
        
        // (AR) تحقق من حجم الرقم
        // (EN) Check number size
        if (value > MAX_NUMBER_SIZE) {
            ErrorManager::getInstance().reportError(
                ErrorCode::LEX_NUMBER_TOO_LARGE,
                getCurrentLocation(),
                "رقم كبير جداً: " + numStr,
                "Number too large: " + numStr
            );
        }
    } catch (...) {
        ErrorManager::getInstance().reportError(
            ErrorCode::LEX_INVALID_NUMBER,
            getCurrentLocation(),
            "تنسيق رقم غير صالح: " + numStr,
            "Invalid number format: " + numStr
        );
    }
    
    return makeToken(TokenType::NUMBER);
}
```

**الوقت المتوقع**: 4-6 ساعات  
**الاختبارات**: اختبار جميع أنواع الأخطاء المعجمية

---

### المهمة 2.2: تعديل Parser لاستخدام ErrorManager ✅

**الملفات المتأثرة**:
- `src/parser/parser_core_impl.cpp`
- `include/parser/parser_core.h`

#### التغييرات المطلوبة:

**أ) إزالة errors_ Vector واستبداله:**

**قبل (Old):**
```cpp
class ParserCore {
private:
    std::vector<std::string> errors_;  // ❌ إزالة
    bool panicMode_ = false;
    
public:
    void error(const std::string& message) {
        if (panicMode_) return;
        panicMode_ = true;
        
        std::stringstream ss;
        ss << "[السطر " << peek().line << "] خطأ";
        if (peek().type == TokenType::END_OF_FILE) {
            ss << " في النهاية";
        } else {
            ss << " عند '" << peek().lexeme << "'";
        }
        ss << ": " << message;
        errors_.push_back(ss.str());
    }
    
    bool hasErrors() const { return !errors_.empty(); }
    void printErrors() const;
    std::vector<std::string> getErrors() const { return errors_; }
};
```

**بعد (New):**
```cpp
class ParserCore {
private:
    bool panicMode_ = false;
    std::string filename_;  // ✅ إضافة
    
public:
    /**
     * @brief (AR) يبلغ عن خطأ نحوي
     *        (EN) Reports syntax error
     * 
     * @param code (AR) رمز الخطأ / (EN) Error code
     * @param location (AR) الموقع / (EN) Location
     * @param message_ar (AR) الرسالة بالعربية / (EN) Arabic message
     * @param message_en (AR) الرسالة بالإنجليزية / (EN) English message
     */
    void error(ErrorCode code,
               const SourceLocation& location,
               const std::string& message_ar,
               const std::string& message_en) {
        if (panicMode_) return;
        panicMode_ = true;
        
        ErrorManager::getInstance().reportError(
            code, location, message_ar, message_en
        );
    }
    
    /**
     * @brief (AR) يتحقق من وجود أخطاء
     *        (EN) Checks if errors exist
     */
    bool hasErrors() const {
        return ErrorManager::getInstance().hasErrors();
    }
    
    /**
     * @brief (AR) يطبع الأخطاء
     *        (EN) Prints errors
     */
    void printErrors() const {
        ErrorManager::getInstance().printAll();
    }
};
```

**ب) تحديث استدعاءات error():**

**قبل (Old):**
```cpp
if (!match(TokenType::SEMICOLON)) {
    error("متوقع ';' بعد الجملة");
}
```

**بعد (New):**
```cpp
if (!match(TokenType::SEMICOLON)) {
    Token current = peek();
    SourceLocation loc = current.toLocation(filename_);
    
    error(
        ErrorCode::SYN_MISSING_SEMICOLON,
        loc,
        "متوقع ';' بعد الجملة",
        "Expected ';' after statement"
    );
}
```

**ج) إضافة Fix-it Hints:**

```cpp
/**
 * @brief (AR) خطأ مع اقتراح إصلاح
 *        (EN) Error with fix-it suggestion
 */
void ParserCore::expectSemicolon() {
    if (!match(TokenType::SEMICOLON)) {
        Token current = previous();
        SourceLocation loc = current.toLocation(filename_);
        
        // (AR) إنشاء موقع للإدراج بعد Token الحالي
        // (EN) Create location for insertion after current token
        SourceLocation insertLoc(
            filename_,
            current.line,
            current.column + current.length,
            current.offset + current.length,
            0
        );
        
        DiagnosticBuilder(
            ErrorCode::SYN_MISSING_SEMICOLON,
            DiagnosticSeverity::ERROR,
            loc
        )
        .withMessage(
            "متوقع ';' بعد الجملة",
            "Expected ';' after statement"
        )
        .withFixItInsert(insertLoc, ";")
        .report();
        
        panicMode_ = true;
    }
}
```

**د) أمثلة على أخطاء Parser مع Fix-its:**

```cpp
/**
 * @brief (AR) يتوقع معرّف
 *        (EN) Expects identifier
 */
std::shared_ptr<Expression> ParserCore::parseIdentifier() {
    if (!check(TokenType::IDENTIFIER)) {
        Token current = peek();
        SourceLocation loc = current.toLocation(filename_);
        
        error(
            ErrorCode::SYN_MISSING_IDENTIFIER,
            loc,
            "متوقع معرّف هنا",
            "Expected identifier here"
        );
        return nullptr;
    }
    
    return std::make_shared<IdentifierExpr>(advance());
}

/**
 * @brief (AR) يتحقق من تطابق الأقواس
 *        (EN) Checks bracket matching
 */
void ParserCore::checkBracketMatch(Token opening, TokenType expected) {
    if (!match(expected)) {
        Token current = peek();
        SourceLocation loc = current.toLocation(filename_);
        
        // (AR) إنشاء ملاحظة لموقع القوس الافتتاحي
        // (EN) Create note for opening bracket location
        SourceLocation openLoc = opening.toLocation(filename_);
        Diagnostic note(
            ErrorCode::SYN_UNCLOSED_BRACKET,
            DiagnosticSeverity::NOTE,
            openLoc,
            "القوس الافتتاحي هنا",
            "Opening bracket here"
        );
        
        DiagnosticBuilder(
            ErrorCode::SYN_UNCLOSED_BRACKET,
            DiagnosticSeverity::ERROR,
            loc
        )
        .withMessage(
            "قوس غير مغلق - متوقع '" + tokenTypeToString(expected) + "'",
            "Unclosed bracket - expected '" + tokenTypeToString(expected) + "'"
        )
        .withNote(note)
        .report();
    }
}
```

**الوقت المتوقع**: 6-8 ساعات  
**الاختبارات**: اختبار جميع أنواع الأخطاء النحوية

---

### المهمة 2.3: Error Recovery في Parser ✅

**الهدف**: تحسين قدرة Parser على التعافي من الأخطاء ومواصلة التحليل

#### استراتيجيات التعافي:

**أ) Panic Mode Recovery:**

```cpp
/**
 * @brief (AR) يتزامن في نقاط آمنة بعد خطأ
 *        (EN) Synchronizes at safe points after error
 */
void ParserCore::synchronize() {
    panicMode_ = false;
    
    // (AR) تخطي حتى بداية الجملة التالية
    // (EN) Skip until next statement start
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        
        switch (peek().type) {
            case TokenType::CLASS:
            case TokenType::FUNCTION:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
            default:
                advance();
        }
    }
}
```

**ب) Error Productions:**

```cpp
/**
 * @brief (AR) يحلل جملة مع تعافي من الأخطاء
 *        (EN) Parses statement with error recovery
 */
std::shared_ptr<Statement> ParserCore::parseStatement() {
    try {
        if (match(TokenType::PRINT)) return parsePrintStatement();
        if (match(TokenType::VAR)) return parseVarDeclaration();
        if (match(TokenType::IF)) return parseIfStatement();
        // ... المزيد
        
        return parseExpressionStatement();
        
    } catch (const ParseError& e) {
        // (AR) تعافي من خطأ وحاول المتابعة
        // (EN) Recover from error and try to continue
        synchronize();
        return nullptr;  // يمكن إرجاع ErrorStmt خاص
    }
}
```

**ج) تعافي من أخطاء شائعة:**

```cpp
/**
 * @brief (AR) يحاول إصلاح أخطاء شائعة
 *        (EN) Attempts to fix common errors
 */
bool ParserCore::tryRecoverFromCommonErrors() {
    Token current = peek();
    
    // (AR) إذا كان '=' بدلاً من '=='
    // (EN) If '=' instead of '=='
    if (current.type == TokenType::EQUAL &&
        isInConditionContext()) {
        
        SourceLocation loc = current.toLocation(filename_);
        
        DiagnosticBuilder(
            ErrorCode::SYN_INVALID_ASSIGNMENT,
            DiagnosticSeverity::WARNING,
            loc
        )
        .withMessage(
            "ربما تقصد '==' للمقارنة بدلاً من '=' للإسناد",
            "Perhaps you meant '==' for comparison instead of '=' for assignment"
        )
        .withFixItReplace(loc, "==")
        .report();
        
        // (AR) استبدل ومتابعة
        // (EN) Replace and continue
        advance();
        return true;
    }
    
    return false;
}
```

**الوقت المتوقع**: 4-5 ساعات  
**الاختبارات**: اختبارات لحالات التعافي

---

### المهمة 2.4: تعديل AST Nodes لتخزين المواقع ✅

**الملفات المتأثرة**:
- `include/ast/expressions.h`
- `include/ast/statements.h`

#### التغييرات:

```cpp
/**
 * @brief (AR) فئة أساسية للتعبيرات مع الموقع
 *        (EN) Base class for expressions with location
 */
class Expression {
protected:
    SourceLocation location_;  // ✅ إضافة
    
public:
    Expression(const SourceLocation& loc) : location_(loc) {}
    virtual ~Expression() = default;
    
    /**
     * @brief (AR) يرجع الموقع
     *        (EN) Returns location
     */
    const SourceLocation& getLocation() const { return location_; }
    
    virtual std::any accept(ExpressionVisitor& visitor) = 0;
};

/**
 * @brief (AR) تعبير ثنائي مع الموقع
 *        (EN) Binary expression with location
 */
class BinaryExpr : public Expression {
private:
    std::shared_ptr<Expression> left_;
    Token operator_;
    std::shared_ptr<Expression> right_;
    
public:
    BinaryExpr(const SourceLocation& loc,
               std::shared_ptr<Expression> left,
               Token op,
               std::shared_ptr<Expression> right)
        : Expression(loc), left_(left), operator_(op), right_(right) {}
    
    // ... الباقي
};
```

**الوقت المتوقع**: 3-4 ساعات  
**الاختبارات**: التحقق من تخزين المواقع صحيحاً

---

### المهمة 2.5: تحديث Main وCommand Line ✅

**الملف**: `src/main.cpp`

```cpp
/**
 * @brief (AR) نقطة الدخول الرئيسية
 *        (EN) Main entry point
 */
int main(int argc, char* argv[]) {
    // (AR) إعداد ErrorManager
    // (EN) Setup ErrorManager
    ErrorManager& errorMgr = ErrorManager::getInstance();
    errorMgr.setLanguage(Language::ARABIC);
    errorMgr.setColorize(true);
    
    // (AR) معالجة خيارات سطر الأوامر
    // (EN) Process command line options
    if (hasOption(argc, argv, "--lang=en")) {
        errorMgr.setLanguage(Language::ENGLISH);
    }
    if (hasOption(argc, argv, "--no-color")) {
        errorMgr.setColorize(false);
    }
    if (hasOption(argc, argv, "--json")) {
        // (AR) إخراج JSON للـ IDE
        // (EN) JSON output for IDE
        std::string jsonFile = getOptionValue(argc, argv, "--json");
        errorMgr.setOutputFile(jsonFile);
    }
    
    // (AR) تشغيل المفسّر
    // (EN) Run interpreter
    try {
        std::string source = readFile(argv[1]);
        
        Lexer lexer(source, argv[1]);
        auto tokens = lexer.scanTokens();
        
        if (errorMgr.hasErrors()) {
            errorMgr.printAll();
            return 65;  // EX_DATAERR
        }
        
        ParserCore parser(tokens, argv[1]);
        auto ast = parser.parse();
        
        if (errorMgr.hasErrors()) {
            errorMgr.printAll();
            return 65;
        }
        
        Interpreter interpreter;
        interpreter.interpret(ast);
        
        if (errorMgr.hasErrors()) {
            errorMgr.printAll();
            return 70;  // EX_SOFTWARE
        }
        
    } catch (const std::exception& e) {
        std::cerr << "(AR) خطأ غير متوقع: " << e.what() << std::endl;
        std::cerr << "(EN) Unexpected error: " << e.what() << std::endl;
        return 1;
    }
    
    // (AR) حفظ JSON إذا طُلب
    // (EN) Save JSON if requested
    if (!errorMgr.getAllDiagnostics().empty()) {
        errorMgr.saveToFile(errorMgr.getOutputFile());
    }
    
    return 0;
}
```

**الوقت المتوقع**: 2-3 ساعات

---

## 📊 التقدم والإنجاز / Progress Tracking

### قائمة المهام (Checklist):

- [ ] المهمة 2.1: تكامل Lexer (4-6 ساعات)
- [ ] المهمة 2.2: تكامل Parser (6-8 ساعات)
- [ ] المهمة 2.3: Error Recovery (4-5 ساعات)
- [ ] المهمة 2.4: تحديث AST Nodes (3-4 ساعات)
- [ ] المهمة 2.5: تحديث Main (2-3 ساعات)
- [ ] اختبارات التكامل (6-8 ساعات)
- [ ] تحديث التوثيق (2-3 ساعات)

**إجمالي الوقت المتوقع**: 27-37 ساعة (~5-7 أيام عمل)

---

## 🧪 استراتيجية الاختبار / Testing Strategy

### اختبارات التكامل:

```cpp
// test_lexer_integration.cpp
TEST(LexerIntegrationTest, InvalidCharacterReporting) {
    ErrorManager::getInstance().clear();
    
    Lexer lexer("متغير @ = 5;", "test.s");
    auto tokens = lexer.scanTokens();
    
    EXPECT_TRUE(ErrorManager::getInstance().hasErrors());
    auto diags = ErrorManager::getInstance().getAllDiagnostics();
    ASSERT_EQ(diags.size(), 1);
    EXPECT_EQ(diags[0].getCode(), ErrorCode::LEX_INVALID_CHARACTER);
}

// test_parser_integration.cpp
TEST(ParserIntegrationTest, MissingSemicolonWithFixIt) {
    ErrorManager::getInstance().clear();
    
    std::vector<Token> tokens = createTokens("عرض(5)");  // Missing ;
    ParserCore parser(tokens, "test.s");
    auto ast = parser.parse();
    
    EXPECT_TRUE(ErrorManager::getInstance().hasErrors());
    auto diags = ErrorManager::getInstance().getAllDiagnostics();
    ASSERT_EQ(diags.size(), 1);
    EXPECT_EQ(diags[0].getCode(), ErrorCode::SYN_MISSING_SEMICOLON);
    EXPECT_FALSE(diags[0].getFixIts().empty());
}
```

---

## 📈 معايير الإنجاز / Completion Criteria

### يعتبر المرحلة 2 مكتملة عندما:
- ✅ Lexer يستخدم ErrorManager بالكامل
- ✅ Parser يستخدم ErrorManager بدلاً من errors_ vector
- ✅ جميع AST nodes تحتوي على معلومات الموقع
- ✅ Error Recovery يعمل بشكل صحيح
- ✅ Fix-it hints تظهر في الأخطاء المناسبة
- ✅ اختبارات التكامل تمر بنجاح (> 95%)
- ✅ السلوك الحالي محفوظ (backward compatible)

---

**التاريخ**: 23 نوفمبر 2025  
**الحالة**: جاهز للتنفيذ  
**المرحلة التالية**: المرحلة 3 - التشخيصات والعرض

</div>
