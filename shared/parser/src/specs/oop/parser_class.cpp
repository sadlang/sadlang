/**
 * =============================================================================
 * ملف: class_parser.cpp
 * الوصف: محلل كلمة صنف (class) للبرمجة الكائنية
 * المهمة: T177 - Implement class (صنف) keyword parser
 * المرحلة: Phase 17 - User Story 15 (OOP صنف Sugar)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هي البرمجة الكائنية (OOP)؟
 * ───────────────────────────────
 * تخيل أنك تصنع لعبة فيديو 🎮:
 * - بدلاً من كتابة كود منفصل لكل شخصية
 * - تصنع "قالب" يسمى "صنف" يحدد خصائص الشخصيات
 * - ثم تصنع شخصيات (كائنات) من هذا القالب
 * 
 * مثال من الحياة:
 * - الصنف = وصفة كيكة 🎂
 * - الكائن = الكيكة الفعلية التي تصنعها
 * - يمكنك صنع عدة كيكات من نفس الوصفة!
 * 
 * لماذا صنف بدلاً من هيكل + نفّذ؟
 * ──────────────────────────────────
 * في لغة ص، هناك طريقتان:
 * 
 * 1. الطريقة الأصلية (هيكل + نفّذ):
 *    ```sad
 *    هيكل شخص:
 *        اسم: نص
 *        عمر: ع32
 *    نهاية
 *    
 *    نفّذ شخص:
 *        دالة قدّم_نفسك():
 *            اطبع("أنا " + هذا.اسم)
 *        نهاية
 *    نهاية
 *    ```
 * 
 * 2. الطريقة المختصرة (صنف) - Sugar Syntax:
 *    ```sad
 *    صنف شخص:
 *        اسم: نص
 *        عمر: ع32
 *        
 *        باني(الاسم: نص, العمر: ع32):
 *            هذا.اسم = الاسم
 *            هذا.عمر = العمر
 *        نهاية
 *        
 *        دالة قدّم_نفسك():
 *            اطبع("أنا " + هذا.اسم)
 *        نهاية
 *    نهاية
 *    ```
 * 
 * كلا الطريقتين تنتج نفس النتيجة!
 * "صنف" يُترجم تلقائياً إلى "هيكل + نفّذ"
 * 
 * مصطلحات مهمة:
 * ─────────────
 * - صنف (class): قالب لإنشاء كائنات
 * - كائن (object): نسخة من الصنف
 * - حقل (field): متغير داخل الصنف (مثل: اسم)
 * - دالة/طريقة (method): فعل يقوم به الصنف (مثل: قدّم_نفسك)
 * - باني (constructor): دالة خاصة تُنشئ الكائن
 * - هذا (this/self): يشير للكائن الحالي
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <optional>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <functional>

// =============================================================================
// تعريف أنواع العُقد في شجرة البناء (AST)
// =============================================================================

/**
 * نوع العقدة في الشجرة
 * 
 * 💡 للمبتدئ:
 * كل جزء من الكود يُمثَّل بنوع عقدة مختلف
 */
enum class ASTNodeType {
    // تعريفات الأصناف
    ClassDefinition,        // صنف ...
    FieldDefinition,        // حقل داخل الصنف
    MethodDefinition,       // دالة داخل الصنف
    ConstructorDefinition,  // باني
    
    // تعابير
    Identifier,             // اسم متغير
    TypeAnnotation,         // تحديد النوع
    FunctionCall,           // استدعاء دالة
    MemberAccess,           // الوصول لحقل (هذا.اسم)
    
    // أنواع أخرى
    Parameter,              // معامل دالة
    Block,                  // مجموعة أوامر
    Statement,              // أمر واحد
};

/**
 * العقدة الأساسية في الشجرة
 * 
 * 💡 للمبتدئ:
 * كل عنصر في الكود يصبح عقدة في شجرة
 * مثل شجرة العائلة - كل شخص له أبناء (عُقد فرعية)
 */
struct ASTNode {
    ASTNodeType type;
    std::string value;
    int line = 0;
    int column = 0;
    std::vector<std::shared_ptr<ASTNode>> children;
    std::map<std::string, std::string> attributes;
    
    ASTNode(ASTNodeType t, const std::string& v = "") : type(t), value(v) {}
    
    void addChild(std::shared_ptr<ASTNode> child) {
        children.push_back(child);
    }
    
    void setAttribute(const std::string& key, const std::string& val) {
        attributes[key] = val;
    }
    
    std::string getAttribute(const std::string& key) const {
        auto it = attributes.find(key);
        return it != attributes.end() ? it->second : "";
    }
};

// =============================================================================
// أنواع الرموز (Tokens)
// =============================================================================

/**
 * نوع الرمز
 * 
 * 💡 للمبتدئ:
 * المحلل يقرأ الكود حرفاً حرفاً ويحوله إلى "رموز"
 * مثل تقطيع جملة إلى كلمات
 */
enum class TokenType {
    // كلمات محجوزة عربية
    Class,          // صنف
    Struct,         // هيكل (للتوافق)
    Constructor,    // باني
    Function,       // دالة
    End,            // نهاية
    This,           // هذا
    Implement,      // نفّذ
    Extension,      // امتداد
    Public,         // عام
    Private,        // خاص
    Protected,      // محمي
    Static,         // ثابت
    Virtual,        // افتراضي
    Override,       // تجاوز
    Super,          // الأصل
    New,            // جديد
    Return,         // ارجع
    If,             // إذا
    Else,           // وإلا
    While,          // طالما
    For,            // لكل
    
    // رموز
    Identifier,     // اسم
    Number,         // رقم
    String,         // نص
    Colon,          // :
    Comma,          // ,
    Dot,            // .
    LeftParen,      // (
    RightParen,     // )
    LeftBracket,    // [
    RightBracket,   // ]
    LeftBrace,      // {
    RightBrace,     // }
    Equals,         // =
    Arrow,          // ->
    DoubleColon,    // ::
    
    // خاص
    Newline,        // سطر جديد
    Indent,         // مسافة بادئة
    Dedent,         // نهاية مسافة بادئة
    EndOfFile,      // نهاية الملف
    Error,          // خطأ
};

/**
 * رمز واحد من الكود
 */
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t, const std::string& v, int l, int c)
        : type(t), value(v), line(l), column(c) {}
    
    std::string toString() const {
        std::ostringstream ss;
        ss << "Token(" << static_cast<int>(type) << ", \"" << value 
           << "\", line " << line << ")";
        return ss.str();
    }
};

// =============================================================================
// المحلل المعجمي للأصناف
// =============================================================================

/**
 * المحلل المعجمي
 * 
 * 💡 للمبتدئ:
 * يحول النص إلى رموز
 * مثل قراءة جملة وتحديد: هذه كلمة، هذه علامة ترقيم، إلخ
 */
class ClassLexer {
public:
    explicit ClassLexer(const std::string& source) 
        : source_(source), pos_(0), line_(1), column_(1) {}
    
    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        
        while (!isAtEnd()) {
            skipWhitespaceAndComments();
            if (isAtEnd()) break;
            
            Token token = nextToken();
            tokens.push_back(token);
        }
        
        tokens.emplace_back(TokenType::EndOfFile, "", line_, column_);
        return tokens;
    }
    
private:
    std::string source_;
    size_t pos_;
    int line_;
    int column_;
    
    // الكلمات المحجوزة العربية
    std::map<std::string, TokenType> keywords_ = {
        {"صنف", TokenType::Class},
        {"هيكل", TokenType::Struct},
        {"باني", TokenType::Constructor},
        {"دالة", TokenType::Function},
        {"نهاية", TokenType::End},
        {"هذا", TokenType::This},
        {"نفّذ", TokenType::Implement},
        {"نفذ", TokenType::Implement},  // بدون تشكيل
        {"امتداد", TokenType::Extension},
        {"عام", TokenType::Public},
        {"خاص", TokenType::Private},
        {"محمي", TokenType::Protected},
        {"ثابت", TokenType::Static},
        {"افتراضي", TokenType::Virtual},
        {"تجاوز", TokenType::Override},
        {"الأصل", TokenType::Super},
        {"جديد", TokenType::New},
        {"ارجع", TokenType::Return},
        {"إذا", TokenType::If},
        {"اذا", TokenType::If},
        {"وإلا", TokenType::Else},
        {"والا", TokenType::Else},
        {"طالما", TokenType::While},
        {"لكل", TokenType::For},
        // English keywords for compatibility
        {"class", TokenType::Class},
        {"struct", TokenType::Struct},
        {"constructor", TokenType::Constructor},
        {"function", TokenType::Function},
        {"end", TokenType::End},
        {"this", TokenType::This},
        {"self", TokenType::This},
        {"impl", TokenType::Implement},
        {"public", TokenType::Public},
        {"private", TokenType::Private},
        {"return", TokenType::Return},
    };
    
    bool isAtEnd() const { return pos_ >= source_.size(); }
    
    char peek() const { return isAtEnd() ? '\0' : source_[pos_]; }
    
    char peekNext() const {
        return pos_ + 1 >= source_.size() ? '\0' : source_[pos_ + 1];
    }
    
    char advance() {
        char c = source_[pos_++];
        if (c == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        return c;
    }
    
    void skipWhitespaceAndComments() {
        while (!isAtEnd()) {
            char c = peek();
            
            if (c == ' ' || c == '\t' || c == '\r') {
                advance();
            } else if (c == '\n') {
                advance();
            } else if (c == '/' && peekNext() == '/') {
                // تعليق سطر واحد
                while (!isAtEnd() && peek() != '\n') advance();
            } else if (c == '#') {
                // تعليق بالعربية
                while (!isAtEnd() && peek() != '\n') advance();
            } else {
                break;
            }
        }
    }
    
    Token nextToken() {
        char c = peek();
        int startLine = line_;
        int startCol = column_;
        
        // رموز خاصة
        if (c == ':') {
            advance();
            if (peek() == ':') {
                advance();
                return Token(TokenType::DoubleColon, "::", startLine, startCol);
            }
            return Token(TokenType::Colon, ":", startLine, startCol);
        }
        
        if (c == ',') { advance(); return Token(TokenType::Comma, ",", startLine, startCol); }
        if (c == '.') { advance(); return Token(TokenType::Dot, ".", startLine, startCol); }
        if (c == '(') { advance(); return Token(TokenType::LeftParen, "(", startLine, startCol); }
        if (c == ')') { advance(); return Token(TokenType::RightParen, ")", startLine, startCol); }
        if (c == '[') { advance(); return Token(TokenType::LeftBracket, "[", startLine, startCol); }
        if (c == ']') { advance(); return Token(TokenType::RightBracket, "]", startLine, startCol); }
        if (c == '{') { advance(); return Token(TokenType::LeftBrace, "{", startLine, startCol); }
        if (c == '}') { advance(); return Token(TokenType::RightBrace, "}", startLine, startCol); }
        
        if (c == '=') {
            advance();
            return Token(TokenType::Equals, "=", startLine, startCol);
        }
        
        if (c == '-' && peekNext() == '>') {
            advance(); advance();
            return Token(TokenType::Arrow, "->", startLine, startCol);
        }
        
        // أرقام
        if (isDigit(c)) {
            return readNumber(startLine, startCol);
        }
        
        // نصوص
        if (c == '"' || c == '\'') {
            return readString(startLine, startCol);
        }
        
        // معرفات وكلمات محجوزة
        if (isIdentifierStart(c)) {
            return readIdentifier(startLine, startCol);
        }
        
        // حرف غير معروف
        advance();
        return Token(TokenType::Error, std::string(1, c), startLine, startCol);
    }
    
    Token readNumber(int startLine, int startCol) {
        std::string num;
        while (!isAtEnd() && (isDigit(peek()) || peek() == '.')) {
            num += advance();
        }
        return Token(TokenType::Number, num, startLine, startCol);
    }
    
    Token readString(int startLine, int startCol) {
        char quote = advance(); // تخطي علامة الاقتباس
        std::string str;
        while (!isAtEnd() && peek() != quote) {
            if (peek() == '\\') {
                advance();
                if (!isAtEnd()) str += advance();
            } else {
                str += advance();
            }
        }
        if (!isAtEnd()) advance(); // تخطي علامة الاقتباس الإغلاق
        return Token(TokenType::String, str, startLine, startCol);
    }
    
    Token readIdentifier(int startLine, int startCol) {
        std::string id;
        while (!isAtEnd() && isIdentifierChar(peek())) {
            id += advance();
        }
        
        // التحقق من الكلمات المحجوزة
        auto it = keywords_.find(id);
        if (it != keywords_.end()) {
            return Token(it->second, id, startLine, startCol);
        }
        
        return Token(TokenType::Identifier, id, startLine, startCol);
    }
    
    bool isDigit(char c) const {
        return c >= '0' && c <= '9';
    }
    
    bool isIdentifierStart(char c) const {
        // يدعم الحروف العربية واللاتينية
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
            return true;
        }
        // UTF-8 Arabic characters start with specific bytes
        return (static_cast<unsigned char>(c) >= 0x80);
    }
    
    bool isIdentifierChar(char c) const {
        return isIdentifierStart(c) || isDigit(c);
    }
};

// =============================================================================
// محلل الأصناف (Class Parser)
// =============================================================================

/**
 * خطأ في التحليل
 */
class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& msg, int line, int col)
        : std::runtime_error(msg), line_(line), column_(col) {}
    
    int line() const { return line_; }
    int column() const { return column_; }
    
    std::string toArabicString() const {
        std::ostringstream ss;
        ss << "خطأ في التحليل [سطر " << line_ << ", عمود " << column_ << "]: " 
           << what();
        return ss.str();
    }
    
private:
    int line_;
    int column_;
};

/**
 * محلل الأصناف
 * 
 * 💡 للمبتدئ:
 * يأخذ الرموز ويبني منها شجرة تمثل بنية الصنف
 * مثل تحويل قائمة كلمات إلى جملة مفهومة
 */
class ClassParser {
public:
    explicit ClassParser(const std::vector<Token>& tokens)
        : tokens_(tokens), current_(0) {}
    
    /**
     * تحليل تعريف صنف كامل
     * 
     * الصيغة:
     * صنف اسم_الصنف:
     *     حقول...
     *     باني...
     *     دوال...
     * نهاية
     */
    std::shared_ptr<ASTNode> parseClassDefinition() {
        // التحقق من كلمة "صنف"
        if (!check(TokenType::Class)) {
            throw ParseError("متوقع 'صنف' في بداية تعريف الصنف", 
                           peek().line, peek().column);
        }
        advance(); // تخطي "صنف"
        
        // قراءة اسم الصنف
        if (!check(TokenType::Identifier)) {
            throw ParseError("متوقع اسم الصنف بعد 'صنف'", 
                           peek().line, peek().column);
        }
        Token nameToken = advance();
        
        auto classNode = std::make_shared<ASTNode>(ASTNodeType::ClassDefinition, nameToken.value);
        classNode->line = nameToken.line;
        classNode->column = nameToken.column;
        
        // قراءة معاملات النوع (إن وجدت) <T, U>
        if (check(TokenType::LeftBracket)) {
            parseTypeParameters(classNode);
        }
        
        // قراءة الوراثة (إن وجدت) : صنف_أب
        if (check(TokenType::Colon)) {
            advance(); // تخطي :
            if (check(TokenType::Identifier)) {
                Token parentToken = advance();
                classNode->setAttribute("parent", parentToken.value);
            }
        }
        
        // قراءة جسم الصنف
        parseClassBody(classNode);
        
        // التحقق من "نهاية"
        if (!check(TokenType::End)) {
            throw ParseError("متوقع 'نهاية' في نهاية تعريف الصنف", 
                           peek().line, peek().column);
        }
        advance(); // تخطي "نهاية"
        
        return classNode;
    }
    
    /**
     * تحليل الحقل
     * 
     * الصيغة:
     * اسم_الحقل: النوع
     * أو
     * عام اسم_الحقل: النوع = قيمة_افتراضية
     */
    std::shared_ptr<ASTNode> parseFieldDefinition() {
        auto fieldNode = std::make_shared<ASTNode>(ASTNodeType::FieldDefinition);
        fieldNode->line = peek().line;
        fieldNode->column = peek().column;
        
        // التحقق من مُحدد الوصول
        std::string visibility = "private"; // الافتراضي خاص
        if (check(TokenType::Public)) {
            visibility = "public";
            advance();
        } else if (check(TokenType::Private)) {
            visibility = "private";
            advance();
        } else if (check(TokenType::Protected)) {
            visibility = "protected";
            advance();
        }
        fieldNode->setAttribute("visibility", visibility);
        
        // التحقق من ثابت
        if (check(TokenType::Static)) {
            fieldNode->setAttribute("static", "true");
            advance();
        }
        
        // قراءة اسم الحقل
        if (!check(TokenType::Identifier)) {
            throw ParseError("متوقع اسم الحقل", peek().line, peek().column);
        }
        Token nameToken = advance();
        fieldNode->value = nameToken.value;
        
        // التحقق من :
        if (!check(TokenType::Colon)) {
            throw ParseError("متوقع ':' بعد اسم الحقل", peek().line, peek().column);
        }
        advance();
        
        // قراءة النوع
        auto typeNode = parseTypeAnnotation();
        fieldNode->addChild(typeNode);
        
        // قراءة القيمة الافتراضية (إن وجدت)
        if (check(TokenType::Equals)) {
            advance();
            auto defaultValue = parseExpression();
            fieldNode->addChild(defaultValue);
            fieldNode->setAttribute("hasDefault", "true");
        }
        
        return fieldNode;
    }
    
    /**
     * تحليل الباني (constructor)
     * 
     * الصيغة:
     * باني(معاملات):
     *     أوامر...
     * نهاية
     */
    std::shared_ptr<ASTNode> parseConstructor() {
        if (!check(TokenType::Constructor)) {
            throw ParseError("متوقع 'باني'", peek().line, peek().column);
        }
        Token ctorToken = advance();
        
        auto ctorNode = std::make_shared<ASTNode>(ASTNodeType::ConstructorDefinition, "باني");
        ctorNode->line = ctorToken.line;
        ctorNode->column = ctorToken.column;
        
        // قراءة المعاملات
        if (!check(TokenType::LeftParen)) {
            throw ParseError("متوقع '(' بعد 'باني'", peek().line, peek().column);
        }
        advance();
        
        parseParameterList(ctorNode);
        
        if (!check(TokenType::RightParen)) {
            throw ParseError("متوقع ')' بعد معاملات الباني", peek().line, peek().column);
        }
        advance();
        
        // قراءة : (إن وجدت)
        if (check(TokenType::Colon)) {
            advance();
        }
        
        // قراءة جسم الباني
        auto body = parseBlock();
        ctorNode->addChild(body);
        
        // التحقق من "نهاية"
        if (!check(TokenType::End)) {
            throw ParseError("متوقع 'نهاية' في نهاية الباني", peek().line, peek().column);
        }
        advance();
        
        return ctorNode;
    }
    
    /**
     * تحليل دالة/طريقة
     * 
     * الصيغة:
     * دالة اسم_الدالة(معاملات) -> نوع_الإرجاع:
     *     أوامر...
     * نهاية
     */
    std::shared_ptr<ASTNode> parseMethod() {
        auto methodNode = std::make_shared<ASTNode>(ASTNodeType::MethodDefinition);
        methodNode->line = peek().line;
        methodNode->column = peek().column;
        
        // التحقق من المُحددات
        std::string visibility = "public"; // الدوال عامة افتراضياً
        if (check(TokenType::Public)) {
            visibility = "public";
            advance();
        } else if (check(TokenType::Private)) {
            visibility = "private";
            advance();
        } else if (check(TokenType::Protected)) {
            visibility = "protected";
            advance();
        }
        methodNode->setAttribute("visibility", visibility);
        
        if (check(TokenType::Static)) {
            methodNode->setAttribute("static", "true");
            advance();
        }
        
        if (check(TokenType::Virtual)) {
            methodNode->setAttribute("virtual", "true");
            advance();
        }
        
        if (check(TokenType::Override)) {
            methodNode->setAttribute("override", "true");
            advance();
        }
        
        // التحقق من "دالة"
        if (!check(TokenType::Function)) {
            throw ParseError("متوقع 'دالة'", peek().line, peek().column);
        }
        advance();
        
        // قراءة اسم الدالة
        if (!check(TokenType::Identifier)) {
            throw ParseError("متوقع اسم الدالة", peek().line, peek().column);
        }
        Token nameToken = advance();
        methodNode->value = nameToken.value;
        
        // قراءة المعاملات
        if (!check(TokenType::LeftParen)) {
            throw ParseError("متوقع '(' بعد اسم الدالة", peek().line, peek().column);
        }
        advance();
        
        parseParameterList(methodNode);
        
        if (!check(TokenType::RightParen)) {
            throw ParseError("متوقع ')' بعد المعاملات", peek().line, peek().column);
        }
        advance();
        
        // قراءة نوع الإرجاع (إن وجد)
        if (check(TokenType::Arrow)) {
            advance();
            auto returnType = parseTypeAnnotation();
            methodNode->addChild(returnType);
            methodNode->setAttribute("hasReturnType", "true");
        }
        
        // قراءة : (إن وجدت)
        if (check(TokenType::Colon)) {
            advance();
        }
        
        // قراءة جسم الدالة
        auto body = parseBlock();
        methodNode->addChild(body);
        
        // التحقق من "نهاية"
        if (!check(TokenType::End)) {
            throw ParseError("متوقع 'نهاية' في نهاية الدالة", peek().line, peek().column);
        }
        advance();
        
        return methodNode;
    }
    
private:
    std::vector<Token> tokens_;
    size_t current_;
    
    bool isAtEnd() const { 
        return current_ >= tokens_.size() || peek().type == TokenType::EndOfFile; 
    }
    
    const Token& peek() const { 
        return tokens_[current_]; 
    }
    
    const Token& previous() const { 
        return tokens_[current_ - 1]; 
    }
    
    Token advance() {
        if (!isAtEnd()) current_++;
        return previous();
    }
    
    bool check(TokenType type) const {
        return !isAtEnd() && peek().type == type;
    }
    
    bool match(TokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }
    
    /**
     * تحليل معاملات النوع <T, U>
     */
    void parseTypeParameters(std::shared_ptr<ASTNode> node) {
        if (!check(TokenType::LeftBracket)) return;
        advance(); // تخطي <
        
        std::string params;
        while (!check(TokenType::RightBracket) && !isAtEnd()) {
            if (!params.empty()) params += ",";
            if (check(TokenType::Identifier)) {
                params += advance().value;
            }
            if (check(TokenType::Comma)) advance();
        }
        
        if (check(TokenType::RightBracket)) advance();
        
        if (!params.empty()) {
            node->setAttribute("typeParams", params);
        }
    }
    
    /**
     * تحليل جسم الصنف
     */
    void parseClassBody(std::shared_ptr<ASTNode> classNode) {
        while (!check(TokenType::End) && !isAtEnd()) {
            // التحقق من نوع العنصر التالي
            if (check(TokenType::Constructor)) {
                auto ctor = parseConstructor();
                classNode->addChild(ctor);
            }
            else if (check(TokenType::Function) || 
                     check(TokenType::Public) || 
                     check(TokenType::Private) ||
                     check(TokenType::Protected) ||
                     check(TokenType::Static) ||
                     check(TokenType::Virtual) ||
                     check(TokenType::Override)) {
                // قد يكون حقل أو دالة - نحتاج للنظر للأمام
                if (isMethodDefinition()) {
                    auto method = parseMethod();
                    classNode->addChild(method);
                } else {
                    auto field = parseFieldDefinition();
                    classNode->addChild(field);
                }
            }
            else if (check(TokenType::Identifier)) {
                // حقل
                auto field = parseFieldDefinition();
                classNode->addChild(field);
            }
            else {
                // تخطي الرموز غير المعروفة
                advance();
            }
        }
    }
    
    /**
     * التحقق هل التعريف التالي هو دالة أم حقل
     */
    bool isMethodDefinition() {
        size_t saved = current_;
        
        // تخطي المُحددات
        while (check(TokenType::Public) || check(TokenType::Private) ||
               check(TokenType::Protected) || check(TokenType::Static) ||
               check(TokenType::Virtual) || check(TokenType::Override)) {
            advance();
        }
        
        bool isMethod = check(TokenType::Function);
        current_ = saved;
        return isMethod;
    }
    
    /**
     * تحليل قائمة المعاملات
     */
    void parseParameterList(std::shared_ptr<ASTNode> node) {
        while (!check(TokenType::RightParen) && !isAtEnd()) {
            auto param = std::make_shared<ASTNode>(ASTNodeType::Parameter);
            
            // اسم المعامل
            if (!check(TokenType::Identifier)) break;
            Token nameToken = advance();
            param->value = nameToken.value;
            
            // نوع المعامل
            if (check(TokenType::Colon)) {
                advance();
                auto typeNode = parseTypeAnnotation();
                param->addChild(typeNode);
            }
            
            // قيمة افتراضية
            if (check(TokenType::Equals)) {
                advance();
                auto defaultVal = parseExpression();
                param->addChild(defaultVal);
                param->setAttribute("hasDefault", "true");
            }
            
            node->addChild(param);
            
            if (check(TokenType::Comma)) {
                advance();
            } else {
                break;
            }
        }
    }
    
    /**
     * تحليل تحديد النوع
     */
    std::shared_ptr<ASTNode> parseTypeAnnotation() {
        auto typeNode = std::make_shared<ASTNode>(ASTNodeType::TypeAnnotation);
        
        if (check(TokenType::Identifier)) {
            typeNode->value = advance().value;
            
            // معاملات النوع [T]
            if (check(TokenType::LeftBracket)) {
                parseTypeParameters(typeNode);
            }
        }
        
        return typeNode;
    }
    
    /**
     * تحليل تعبير (مبسط)
     */
    std::shared_ptr<ASTNode> parseExpression() {
        auto exprNode = std::make_shared<ASTNode>(ASTNodeType::Statement);
        
        // قراءة التعبير حتى نهاية السطر أو فاصلة
        std::string expr;
        int parenDepth = 0;
        
        while (!isAtEnd() && !check(TokenType::End)) {
            if (check(TokenType::LeftParen)) parenDepth++;
            if (check(TokenType::RightParen)) {
                if (parenDepth == 0) break;
                parenDepth--;
            }
            if (check(TokenType::Comma) && parenDepth == 0) break;
            if (check(TokenType::Colon) && parenDepth == 0) break;
            
            expr += peek().value + " ";
            advance();
        }
        
        exprNode->value = expr;
        return exprNode;
    }
    
    /**
     * تحليل كتلة أوامر
     */
    std::shared_ptr<ASTNode> parseBlock() {
        auto blockNode = std::make_shared<ASTNode>(ASTNodeType::Block);
        
        // قراءة الأوامر حتى "نهاية"
        while (!check(TokenType::End) && !isAtEnd()) {
            auto stmt = parseStatement();
            if (stmt) {
                blockNode->addChild(stmt);
            }
        }
        
        return blockNode;
    }
    
    /**
     * تحليل أمر واحد (مبسط)
     */
    std::shared_ptr<ASTNode> parseStatement() {
        auto stmtNode = std::make_shared<ASTNode>(ASTNodeType::Statement);
        
        // قراءة الأمر
        std::string stmt;
        while (!isAtEnd() && !check(TokenType::End)) {
            // توقف عند بداية أمر جديد
            if (check(TokenType::Return) || check(TokenType::If) || 
                check(TokenType::While) || check(TokenType::For)) {
                if (!stmt.empty()) break;
            }
            
            stmt += peek().value + " ";
            advance();
            
            // التحقق من نهاية الأمر
            if (stmt.find("نهاية") != std::string::npos) {
                // أعد "نهاية" للمستوى الأعلى
                current_--;
                stmt = stmt.substr(0, stmt.find("نهاية"));
                break;
            }
        }
        
        stmtNode->value = stmt;
        return stmtNode;
    }
};

// =============================================================================
// معلومات الصنف المستخرجة
// =============================================================================

/**
 * معلومات حقل
 */
struct FieldInfo {
    std::string name;
    std::string type;
    std::string visibility;
    bool isStatic;
    bool hasDefault;
    std::string defaultValue;
};

/**
 * معلومات معامل
 */
struct ParameterInfo {
    std::string name;
    std::string type;
    bool hasDefault;
    std::string defaultValue;
};

/**
 * معلومات دالة/طريقة
 */
struct MethodInfo {
    std::string name;
    std::string visibility;
    bool isStatic;
    bool isVirtual;
    bool isOverride;
    std::string returnType;
    std::vector<ParameterInfo> parameters;
};

/**
 * معلومات صنف كاملة
 */
struct ClassInfo {
    std::string name;
    std::string parentClass;
    std::vector<std::string> typeParameters;
    std::vector<FieldInfo> fields;
    std::vector<MethodInfo> methods;
    std::optional<MethodInfo> constructor;
    
    /**
     * طباعة معلومات الصنف (للتصحيح)
     */
    void print() const {
        std::cout << "صنف: " << name << "\n";
        if (!parentClass.empty()) {
#ifdef DEBUG_OOP
            std::cout << "  يرث من: " << parentClass << "\n";
#endif
        }
        if (!typeParameters.empty()) {
            std::cout << "  معاملات النوع: ";
            for (const auto& p : typeParameters) std::cout << p << " ";
            std::cout << "\n";
        }
        
        std::cout << "  الحقول:\n";
        for (const auto& f : fields) {
            std::cout << "    - " << f.name << ": " << f.type 
                      << " (" << f.visibility << ")\n";
        }
        
        if (constructor) {
            std::cout << "  الباني: " << constructor->parameters.size() << " معامل\n";
        }
        
        std::cout << "  الدوال:\n";
        for (const auto& m : methods) {
            std::cout << "    - " << m.name << "(" << m.parameters.size() << " معامل)";
            if (!m.returnType.empty()) std::cout << " -> " << m.returnType;
            std::cout << "\n";
        }
    }
};

/**
 * مستخرج معلومات الصنف
 * 
 * 💡 للمبتدئ:
 * يحول شجرة AST إلى بيانات منظمة سهلة الاستخدام
 */
class ClassInfoExtractor {
public:
    ClassInfo extract(const std::shared_ptr<ASTNode>& classNode) {
        ClassInfo info;
        
        if (classNode->type != ASTNodeType::ClassDefinition) {
            throw std::runtime_error("العقدة ليست تعريف صنف");
        }
        
        info.name = classNode->value;
        info.parentClass = classNode->getAttribute("parent");
        
        // استخراج معاملات النوع
        std::string typeParams = classNode->getAttribute("typeParams");
        if (!typeParams.empty()) {
            // تقسيم بالفاصلة
            std::istringstream ss(typeParams);
            std::string param;
            while (std::getline(ss, param, ',')) {
                info.typeParameters.push_back(param);
            }
        }
        
        // استخراج العناصر الفرعية
        for (const auto& child : classNode->children) {
            switch (child->type) {
                case ASTNodeType::FieldDefinition:
                    info.fields.push_back(extractField(child));
                    break;
                case ASTNodeType::ConstructorDefinition:
                    info.constructor = extractMethod(child);
                    info.constructor->name = "باني";
                    break;
                case ASTNodeType::MethodDefinition:
                    info.methods.push_back(extractMethod(child));
                    break;
                default:
                    break;
            }
        }
        
        return info;
    }
    
private:
    FieldInfo extractField(const std::shared_ptr<ASTNode>& node) {
        FieldInfo field;
        field.name = node->value;
        field.visibility = node->getAttribute("visibility");
        field.isStatic = node->getAttribute("static") == "true";
        field.hasDefault = node->getAttribute("hasDefault") == "true";
        
        // استخراج النوع
        for (const auto& child : node->children) {
            if (child->type == ASTNodeType::TypeAnnotation) {
                field.type = child->value;
            }
        }
        
        return field;
    }
    
    MethodInfo extractMethod(const std::shared_ptr<ASTNode>& node) {
        MethodInfo method;
        method.name = node->value;
        method.visibility = node->getAttribute("visibility");
        method.isStatic = node->getAttribute("static") == "true";
        method.isVirtual = node->getAttribute("virtual") == "true";
        method.isOverride = node->getAttribute("override") == "true";
        
        // استخراج المعاملات والنوع
        for (const auto& child : node->children) {
            if (child->type == ASTNodeType::Parameter) {
                ParameterInfo param;
                param.name = child->value;
                param.hasDefault = child->getAttribute("hasDefault") == "true";
                
                for (const auto& pc : child->children) {
                    if (pc->type == ASTNodeType::TypeAnnotation) {
                        param.type = pc->value;
                    }
                }
                
                method.parameters.push_back(param);
            }
            else if (child->type == ASTNodeType::TypeAnnotation && 
                     node->getAttribute("hasReturnType") == "true") {
                method.returnType = child->value;
            }
        }
        
        return method;
    }
};

// =============================================================================
// واجهة C البرمجية
// =============================================================================

extern "C" {

/**
 * تحليل تعريف صنف من نص
 * 
 * @param source الكود المصدري
 * @param error_out مؤشر لتخزين رسالة الخطأ
 * @return معرف الصنف المُحلل أو nullptr
 */
void* sad_parse_class(const char* source, char** error_out) {
    try {
        std::string src(source);
        
        ClassLexer lexer(src);
        auto tokens = lexer.tokenize();
        
        ClassParser parser(tokens);
        auto ast = parser.parseClassDefinition();
        
        ClassInfoExtractor extractor;
        auto* info = new ClassInfo(extractor.extract(ast));
        
        return info;
    }
    catch (const ParseError& e) {
        if (error_out) {
            std::string msg = e.toArabicString();
            *error_out = new char[msg.size() + 1];
            std::strcpy(*error_out, msg.c_str());
        }
        return nullptr;
    }
    catch (const std::exception& e) {
        if (error_out) {
            std::string msg = std::string("خطأ: ") + e.what();
            *error_out = new char[msg.size() + 1];
            std::strcpy(*error_out, msg.c_str());
        }
        return nullptr;
    }
}

/**
 * الحصول على اسم الصنف
 */
const char* sad_class_get_name(void* class_info) {
    if (!class_info) return "";
    auto* info = static_cast<ClassInfo*>(class_info);
    return info->name.c_str();
}

/**
 * الحصول على عدد الحقول
 */
int sad_class_get_field_count(void* class_info) {
    if (!class_info) return 0;
    auto* info = static_cast<ClassInfo*>(class_info);
    return static_cast<int>(info->fields.size());
}

/**
 * الحصول على عدد الدوال
 */
int sad_class_get_method_count(void* class_info) {
    if (!class_info) return 0;
    auto* info = static_cast<ClassInfo*>(class_info);
    return static_cast<int>(info->methods.size());
}

/**
 * التحقق من وجود باني
 */
int sad_class_has_constructor(void* class_info) {
    if (!class_info) return 0;
    auto* info = static_cast<ClassInfo*>(class_info);
    return info->constructor.has_value() ? 1 : 0;
}

/**
 * تحرير ذاكرة معلومات الصنف
 */
void sad_class_free(void* class_info) {
    if (class_info) {
        delete static_cast<ClassInfo*>(class_info);
    }
}

/**
 * تحرير رسالة الخطأ
 */
void sad_class_error_free(char* error) {
    if (error) {
        delete[] error;
    }
}

} // extern "C"

// =============================================================================
// دالة الاختبار
// =============================================================================

#ifdef CLASS_PARSER_TEST

int main() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║     اختبار محلل الأصناف (صنف)                                               ║
║     Class Parser Test                                                        ║
║                                                                              ║
║     المرحلة 17 - T177                                                        ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;

    // كود صنف للاختبار
    std::string testCode = R"(
صنف شخص:
    اسم: نص
    عمر: ع32
    
    باني(الاسم: نص, العمر: ع32):
        هذا.اسم = الاسم
        هذا.عمر = العمر
    نهاية
    
    دالة قدّم_نفسك() -> نص:
        ارجع "أنا " + هذا.اسم
    نهاية
    
    دالة عيد_الميلاد():
        هذا.عمر = هذا.عمر + 1
    نهاية
نهاية
)";

    std::cout << "الكود المُدخل:\n" << testCode << "\n";
    std::cout << "─────────────────────────────────────────\n\n";

    try {
        // تحليل الكود
        ClassLexer lexer(testCode);
        auto tokens = lexer.tokenize();
        
        std::cout << "الرموز المستخرجة: " << tokens.size() << " رمز\n";
        
        ClassParser parser(tokens);
        auto ast = parser.parseClassDefinition();
        
        std::cout << "تم بناء شجرة AST بنجاح!\n\n";
        
        // استخراج المعلومات
        ClassInfoExtractor extractor;
        ClassInfo info = extractor.extract(ast);
        
        std::cout << "معلومات الصنف المستخرجة:\n";
        std::cout << "═══════════════════════════\n";
        info.print();
        
        std::cout << "\n✅ نجح تحليل الصنف!\n";
        
    } catch (const ParseError& e) {
        std::cout << "\n❌ " << e.toArabicString() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cout << "\n❌ خطأ: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

#endif // CLASS_PARSER_TEST
