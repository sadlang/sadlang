/**
 * =============================================================================
 * ملف: constructor_parser.cpp
 * الوصف: محلل الباني (constructor) للأصناف
 * المهمة: T178 - Implement constructor (باني) parser
 * المرحلة: Phase 17 - User Story 15 (OOP صنف Sugar)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هو الباني (Constructor)؟
 * ─────────────────────────────
 * تخيل أنك تبني بيتاً 🏠:
 * - قبل السكن، يجب تجهيز البيت (كهرباء، ماء، أثاث)
 * - الباني هو "عامل التجهيز" الذي يجهز الكائن قبل استخدامه
 * 
 * مثال من الحياة:
 * - عند شراء هاتف جديد 📱، يأتي مُعدّ مسبقاً (نظام التشغيل، الإعدادات)
 * - الباني هو من يقوم بهذا الإعداد
 * 
 * لماذا نحتاج الباني؟
 * ─────────────────────
 * 1. تهيئة القيم الأولية للحقول
 * 2. التحقق من صحة البيانات
 * 3. حجز الموارد (ملفات، اتصالات)
 * 4. ضمان أن الكائن جاهز للاستخدام
 * 
 * أنواع البناة:
 * ─────────────
 * 1. باني افتراضي - بدون معاملات
 * 2. باني بمعاملات - يستقبل قيم أولية
 * 3. باني نسخ - ينشئ نسخة من كائن آخر
 * 4. باني تحويل - يحول من نوع لآخر
 * 
 * الصيغة في لغة ص:
 * ────────────────
 * ```sad
 * صنف سيارة:
 *     لون: نص
 *     سرعة: ع32
 *     
 *     // باني بمعاملات
 *     باني(اللون: نص, السرعة: ع32 = 0):
 *         هذا.لون = اللون
 *         هذا.سرعة = السرعة
 *     نهاية
 *     
 *     // باني افتراضي
 *     باني():
 *         هذا.لون = "أبيض"
 *         هذا.سرعة = 0
 *     نهاية
 * نهاية
 * 
 * // الاستخدام
 * متغير سيارتي = جديد سيارة("أحمر", 100)
 * متغير سيارة_بيضاء = جديد سيارة()  // باني افتراضي
 * ```
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
#include <algorithm>

// =============================================================================
// هياكل البيانات
// =============================================================================

/**
 * نوع الباني
 */
enum class ConstructorType {
    Default,        // باني افتراضي (بدون معاملات)
    Parameterized,  // باني بمعاملات
    Copy,           // باني نسخ
    Move,           // باني نقل
    Conversion,     // باني تحويل
};

/**
 * معامل الباني
 * 
 * 💡 للمبتدئ:
 * المعامل هو "مدخل" للباني - قيمة يستقبلها عند الإنشاء
 */
struct ConstructorParameter {
    std::string name;           // اسم المعامل
    std::string type;           // نوع المعامل
    bool hasDefault;            // هل له قيمة افتراضية؟
    std::string defaultValue;   // القيمة الافتراضية
    bool isReference;           // هل هو مرجع؟
    bool isMutable;             // هل هو قابل للتعديل؟
    int position;               // موقعه في القائمة
    
    ConstructorParameter()
        : hasDefault(false), isReference(false), isMutable(false), position(0) {}
    
    std::string toString() const {
        std::ostringstream ss;
        ss << name << ": ";
        if (isReference) ss << "&";
        if (isMutable) ss << "متغير ";
        ss << type;
        if (hasDefault) ss << " = " << defaultValue;
        return ss.str();
    }
};

/**
 * تعليمة تهيئة حقل
 * 
 * 💡 للمبتدئ:
 * تهيئة الحقل = إعطاء الحقل قيمته الأولية
 * مثل: هذا.اسم = الاسم
 */
struct FieldInitializer {
    std::string fieldName;      // اسم الحقل
    std::string expression;     // التعبير المُسند
    bool usesSuperInit;         // هل يستخدم تهيئة من الصنف الأب؟
    int line;                   // رقم السطر
    
    FieldInitializer() : usesSuperInit(false), line(0) {}
};

/**
 * استدعاء باني آخر
 * 
 * 💡 للمبتدئ:
 * أحياناً باني يستدعي باني آخر (من نفس الصنف أو الصنف الأب)
 */
struct DelegatingCall {
    bool isToThis;              // استدعاء باني آخر من نفس الصنف
    bool isToSuper;             // استدعاء باني الصنف الأب
    std::vector<std::string> arguments;  // المعاملات
    
    DelegatingCall() : isToThis(false), isToSuper(false) {}
};

/**
 * تعريف الباني الكامل
 */
struct ConstructorDefinition {
    ConstructorType type;
    std::string className;
    std::vector<ConstructorParameter> parameters;
    std::vector<FieldInitializer> initializers;
    std::optional<DelegatingCall> delegation;
    std::vector<std::string> bodyStatements;
    std::string visibility;     // عام، خاص، محمي
    bool isExplicit;            // لا يُستخدم للتحويل الضمني
    bool isNoexcept;            // لا يرمي استثناءات
    int line;
    int column;
    
    ConstructorDefinition()
        : type(ConstructorType::Parameterized)
        , visibility("public")
        , isExplicit(false)
        , isNoexcept(false)
        , line(0)
        , column(0) {}
    
    /**
     * هل هو باني افتراضي؟
     */
    bool isDefault() const {
        return parameters.empty() || 
               std::all_of(parameters.begin(), parameters.end(),
                   [](const ConstructorParameter& p) { return p.hasDefault; });
    }
    
    /**
     * طباعة معلومات الباني
     */
    void print() const {
#ifdef DEBUG_OOP
        std::cout << "باني " << className << "(";
#endif
        for (size_t i = 0; i < parameters.size(); i++) {
            if (i > 0) std::cout << ", ";
            std::cout << parameters[i].toString();
        }
        std::cout << ")\n";
        
        if (!initializers.empty()) {
            std::cout << "  التهيئة:\n";
            for (const auto& init : initializers) {
                std::cout << "    " << init.fieldName << " = " << init.expression << "\n";
            }
        }
        
        if (delegation) {
            if (delegation->isToSuper) {
                std::cout << "  يستدعي: الأصل(";
            } else {
                std::cout << "  يستدعي: هذا(";
            }
            for (size_t i = 0; i < delegation->arguments.size(); i++) {
                if (i > 0) std::cout << ", ";
                std::cout << delegation->arguments[i];
            }
            std::cout << ")\n";
        }
    }
};

// =============================================================================
// محلل الباني
// =============================================================================

/**
 * خطأ في تحليل الباني
 */
class ConstructorParseError : public std::runtime_error {
public:
    ConstructorParseError(const std::string& msg, int line, int col)
        : std::runtime_error(msg), line_(line), column_(col) {}
    
    int line() const { return line_; }
    int column() const { return column_; }
    
    std::string toArabicString() const {
        std::ostringstream ss;
        ss << "خطأ في تحليل الباني [سطر " << line_ << "]: " << what();
        return ss.str();
    }
    
private:
    int line_;
    int column_;
};

/**
 * رمز للمحلل المعجمي المصغر
 */
struct CtorToken {
    enum Type {
        Constructor,    // باني
        This,           // هذا
        Super,          // الأصل
        End,            // نهاية
        Identifier,     // اسم
        Number,         // رقم
        String,         // نص
        Colon,          // :
        Comma,          // ,
        Dot,            // .
        Equals,         // =
        LeftParen,      // (
        RightParen,     // )
        Arrow,          // ->
        Ampersand,      // &
        Mutable,        // متغير
        Public,         // عام
        Private,        // خاص
        Protected,      // محمي
        Explicit,       // صريح
        Newline,        // سطر جديد
        EndOfFile,
        Error
    };
    
    Type type;
    std::string value;
    int line;
    int column;
    
    CtorToken(Type t, const std::string& v, int l, int c)
        : type(t), value(v), line(l), column(c) {}
};

/**
 * المحلل المعجمي للباني
 */
class ConstructorLexer {
public:
    explicit ConstructorLexer(const std::string& source)
        : source_(source), pos_(0), line_(1), column_(1) {}
    
    std::vector<CtorToken> tokenize() {
        std::vector<CtorToken> tokens;
        
        while (!isAtEnd()) {
            skipWhitespace();
            if (isAtEnd()) break;
            
            tokens.push_back(nextToken());
        }
        
        tokens.emplace_back(CtorToken::EndOfFile, "", line_, column_);
        return tokens;
    }
    
private:
    std::string source_;
    size_t pos_;
    int line_;
    int column_;
    
    std::map<std::string, CtorToken::Type> keywords_ = {
        {"باني", CtorToken::Constructor},
        {"هذا", CtorToken::This},
        {"الأصل", CtorToken::Super},
        {"نهاية", CtorToken::End},
        {"متغير", CtorToken::Mutable},
        {"عام", CtorToken::Public},
        {"خاص", CtorToken::Private},
        {"محمي", CtorToken::Protected},
        {"صريح", CtorToken::Explicit},
        // English
        {"constructor", CtorToken::Constructor},
        {"this", CtorToken::This},
        {"super", CtorToken::Super},
        {"end", CtorToken::End},
        {"mut", CtorToken::Mutable},
        {"public", CtorToken::Public},
        {"private", CtorToken::Private},
        {"explicit", CtorToken::Explicit},
    };
    
    bool isAtEnd() const { return pos_ >= source_.size(); }
    char peek() const { return isAtEnd() ? '\0' : source_[pos_]; }
    char peekNext() const { return pos_ + 1 >= source_.size() ? '\0' : source_[pos_ + 1]; }
    
    char advance() {
        char c = source_[pos_++];
        if (c == '\n') { line_++; column_ = 1; }
        else column_++;
        return c;
    }
    
    void skipWhitespace() {
        while (!isAtEnd()) {
            char c = peek();
            if (c == ' ' || c == '\t' || c == '\r') {
                advance();
            } else if (c == '\n') {
                advance();
            } else if (c == '/' && peekNext() == '/') {
                while (!isAtEnd() && peek() != '\n') advance();
            } else if (c == '#') {
                while (!isAtEnd() && peek() != '\n') advance();
            } else {
                break;
            }
        }
    }
    
    CtorToken nextToken() {
        char c = peek();
        int startLine = line_, startCol = column_;
        
        if (c == ':') { advance(); return CtorToken(CtorToken::Colon, ":", startLine, startCol); }
        if (c == ',') { advance(); return CtorToken(CtorToken::Comma, ",", startLine, startCol); }
        if (c == '.') { advance(); return CtorToken(CtorToken::Dot, ".", startLine, startCol); }
        if (c == '(') { advance(); return CtorToken(CtorToken::LeftParen, "(", startLine, startCol); }
        if (c == ')') { advance(); return CtorToken(CtorToken::RightParen, ")", startLine, startCol); }
        if (c == '=') { advance(); return CtorToken(CtorToken::Equals, "=", startLine, startCol); }
        if (c == '&') { advance(); return CtorToken(CtorToken::Ampersand, "&", startLine, startCol); }
        
        if (c == '-' && peekNext() == '>') {
            advance(); advance();
            return CtorToken(CtorToken::Arrow, "->", startLine, startCol);
        }
        
        if (c >= '0' && c <= '9') return readNumber(startLine, startCol);
        if (c == '"' || c == '\'') return readString(startLine, startCol);
        if (isIdentifierStart(c)) return readIdentifier(startLine, startCol);
        
        advance();
        return CtorToken(CtorToken::Error, std::string(1, c), startLine, startCol);
    }
    
    CtorToken readNumber(int startLine, int startCol) {
        std::string num;
        while (!isAtEnd() && (isDigit(peek()) || peek() == '.')) {
            num += advance();
        }
        return CtorToken(CtorToken::Number, num, startLine, startCol);
    }
    
    CtorToken readString(int startLine, int startCol) {
        char quote = advance();
        std::string str;
        while (!isAtEnd() && peek() != quote) {
            if (peek() == '\\') { advance(); if (!isAtEnd()) str += advance(); }
            else str += advance();
        }
        if (!isAtEnd()) advance();
        return CtorToken(CtorToken::String, str, startLine, startCol);
    }
    
    CtorToken readIdentifier(int startLine, int startCol) {
        std::string id;
        while (!isAtEnd() && isIdentifierChar(peek())) {
            id += advance();
        }
        auto it = keywords_.find(id);
        if (it != keywords_.end()) {
            return CtorToken(it->second, id, startLine, startCol);
        }
        return CtorToken(CtorToken::Identifier, id, startLine, startCol);
    }
    
    bool isDigit(char c) const { return c >= '0' && c <= '9'; }
    bool isIdentifierStart(char c) const {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
               c == '_' || static_cast<unsigned char>(c) >= 0x80;
    }
    bool isIdentifierChar(char c) const { return isIdentifierStart(c) || isDigit(c); }
};

/**
 * محلل الباني الرئيسي
 * 
 * 💡 للمبتدئ:
 * يأخذ كود الباني ويحوله إلى بيانات منظمة
 */
class ConstructorParser {
public:
    explicit ConstructorParser(const std::vector<CtorToken>& tokens)
        : tokens_(tokens), current_(0) {}
    
    /**
     * تحليل تعريف باني كامل
     */
    ConstructorDefinition parse() {
        ConstructorDefinition ctor;
        ctor.line = peek().line;
        ctor.column = peek().column;
        
        // قراءة مُحددات الوصول
        parseModifiers(ctor);
        
        // التحقق من كلمة "باني"
        if (!check(CtorToken::Constructor)) {
            throw ConstructorParseError("متوقع 'باني'", peek().line, peek().column);
        }
        advance();
        
        // قراءة المعاملات
        if (!check(CtorToken::LeftParen)) {
            throw ConstructorParseError("متوقع '(' بعد 'باني'", peek().line, peek().column);
        }
        advance();
        
        parseParameters(ctor);
        
        if (!check(CtorToken::RightParen)) {
            throw ConstructorParseError("متوقع ')'", peek().line, peek().column);
        }
        advance();
        
        // قراءة : إن وجدت
        if (check(CtorToken::Colon)) {
            advance();
            
            // التحقق من استدعاء الأصل أو هذا
            if (check(CtorToken::Super) || check(CtorToken::This)) {
                parseDelegation(ctor);
            }
        }
        
        // قراءة جسم الباني
        parseBody(ctor);
        
        // تحديد نوع الباني
        determineType(ctor);
        
        return ctor;
    }
    
    /**
     * تحليل قائمة معاملات
     */
    std::vector<ConstructorParameter> parseParameterList() {
        std::vector<ConstructorParameter> params;
        int position = 0;
        
        while (!check(CtorToken::RightParen) && !isAtEnd()) {
            ConstructorParameter param;
            param.position = position++;
            
            // التحقق من مرجع
            if (check(CtorToken::Ampersand)) {
                param.isReference = true;
                advance();
            }
            
            // التحقق من متغير
            if (check(CtorToken::Mutable)) {
                param.isMutable = true;
                advance();
            }
            
            // اسم المعامل
            if (!check(CtorToken::Identifier)) break;
            param.name = advance().value;
            
            // نوع المعامل
            if (check(CtorToken::Colon)) {
                advance();
                param.type = parseType();
            }
            
            // قيمة افتراضية
            if (check(CtorToken::Equals)) {
                advance();
                param.hasDefault = true;
                param.defaultValue = parseExpression();
            }
            
            params.push_back(param);
            
            if (check(CtorToken::Comma)) {
                advance();
            } else {
                break;
            }
        }
        
        return params;
    }
    
private:
    std::vector<CtorToken> tokens_;
    size_t current_;
    
    bool isAtEnd() const { return current_ >= tokens_.size() || peek().type == CtorToken::EndOfFile; }
    const CtorToken& peek() const { return tokens_[current_]; }
    const CtorToken& previous() const { return tokens_[current_ - 1]; }
    
    CtorToken advance() {
        if (!isAtEnd()) current_++;
        return previous();
    }
    
    bool check(CtorToken::Type type) const {
        return !isAtEnd() && peek().type == type;
    }
    
    /**
     * تحليل المُحددات
     */
    void parseModifiers(ConstructorDefinition& ctor) {
        while (true) {
            if (check(CtorToken::Public)) {
                ctor.visibility = "public";
                advance();
            } else if (check(CtorToken::Private)) {
                ctor.visibility = "private";
                advance();
            } else if (check(CtorToken::Protected)) {
                ctor.visibility = "protected";
                advance();
            } else if (check(CtorToken::Explicit)) {
                ctor.isExplicit = true;
                advance();
            } else {
                break;
            }
        }
    }
    
    /**
     * تحليل المعاملات
     */
    void parseParameters(ConstructorDefinition& ctor) {
        ctor.parameters = parseParameterList();
    }
    
    /**
     * تحليل استدعاء باني آخر
     */
    void parseDelegation(ConstructorDefinition& ctor) {
        DelegatingCall call;
        
        if (check(CtorToken::Super)) {
            call.isToSuper = true;
            advance();
        } else if (check(CtorToken::This)) {
            call.isToThis = true;
            advance();
        }
        
        // قراءة المعاملات
        if (check(CtorToken::LeftParen)) {
            advance();
            
            while (!check(CtorToken::RightParen) && !isAtEnd()) {
                call.arguments.push_back(parseExpression());
                
                if (check(CtorToken::Comma)) {
                    advance();
                } else {
                    break;
                }
            }
            
            if (check(CtorToken::RightParen)) advance();
        }
        
        ctor.delegation = call;
    }
    
    /**
     * تحليل جسم الباني
     */
    void parseBody(ConstructorDefinition& ctor) {
        // قراءة الأوامر حتى "نهاية"
        while (!check(CtorToken::End) && !isAtEnd()) {
            // التحقق من تهيئة حقل (هذا.حقل = ...)
            if (check(CtorToken::This)) {
                auto init = parseFieldInitializer();
                if (init) {
                    ctor.initializers.push_back(*init);
                }
            } else {
                // أمر عادي
                std::string stmt = parseStatement();
                if (!stmt.empty()) {
                    ctor.bodyStatements.push_back(stmt);
                }
            }
        }
        
        // التحقق من "نهاية"
        if (!check(CtorToken::End)) {
            throw ConstructorParseError("متوقع 'نهاية'", peek().line, peek().column);
        }
        advance();
    }
    
    /**
     * تحليل تهيئة حقل
     */
    std::optional<FieldInitializer> parseFieldInitializer() {
        if (!check(CtorToken::This)) return std::nullopt;
        advance(); // هذا
        
        if (!check(CtorToken::Dot)) {
            // ليست تهيئة حقل
            current_--; // إرجاع "هذا"
            return std::nullopt;
        }
        advance(); // .
        
        FieldInitializer init;
        init.line = peek().line;
        
        if (!check(CtorToken::Identifier)) {
            throw ConstructorParseError("متوقع اسم الحقل بعد 'هذا.'", peek().line, peek().column);
        }
        init.fieldName = advance().value;
        
        if (!check(CtorToken::Equals)) {
            // قراءة كـ تعبير عادي
            current_ -= 3; // إرجاع كل شيء
            return std::nullopt;
        }
        advance(); // =
        
        init.expression = parseExpression();
        
        return init;
    }
    
    /**
     * تحليل نوع
     */
    std::string parseType() {
        std::string type;
        
        if (check(CtorToken::Ampersand)) {
            type += "&";
            advance();
        }
        
        if (check(CtorToken::Mutable)) {
            type += "متغير ";
            advance();
        }
        
        if (check(CtorToken::Identifier)) {
            type += advance().value;
            
            // معاملات النوع
            // لم نُنفذها هنا للبساطة
        }
        
        return type;
    }
    
    /**
     * تحليل تعبير (مبسط)
     */
    std::string parseExpression() {
        std::string expr;
        int parenDepth = 0;
        
        while (!isAtEnd()) {
            if (check(CtorToken::LeftParen)) parenDepth++;
            if (check(CtorToken::RightParen)) {
                if (parenDepth == 0) break;
                parenDepth--;
            }
            if (check(CtorToken::Comma) && parenDepth == 0) break;
            if (check(CtorToken::End)) break;
            
            // التحقق من نهاية السطر المنطقية
            if (peek().value == "هذا" && parenDepth == 0) break;
            
            expr += peek().value;
            advance();
            
            // إضافة مسافة بين الرموز
            if (!check(CtorToken::Dot) && !check(CtorToken::LeftParen) &&
                previous().type != CtorToken::Dot) {
                expr += " ";
            }
        }
        
        // إزالة المسافات الزائدة
        while (!expr.empty() && expr.back() == ' ') {
            expr.pop_back();
        }
        
        return expr;
    }
    
    /**
     * تحليل أمر (مبسط)
     */
    std::string parseStatement() {
        std::string stmt;
        
        while (!isAtEnd() && !check(CtorToken::End)) {
            if (check(CtorToken::This)) {
                // قد تكون بداية تهيئة جديدة
                break;
            }
            
            stmt += peek().value + " ";
            advance();
        }
        
        // إزالة المسافات الزائدة
        while (!stmt.empty() && stmt.back() == ' ') {
            stmt.pop_back();
        }
        
        return stmt;
    }
    
    /**
     * تحديد نوع الباني
     */
    void determineType(ConstructorDefinition& ctor) {
        if (ctor.parameters.empty()) {
            ctor.type = ConstructorType::Default;
        }
        else if (ctor.parameters.size() == 1) {
            const auto& param = ctor.parameters[0];
            
            // باني نسخ: باني(آخر: &الصنف)
            if (param.isReference && !param.isMutable && 
                param.type == ctor.className) {
                ctor.type = ConstructorType::Copy;
            }
            // باني نقل: باني(آخر: &&الصنف)
            // (غير مُنفذ بالكامل)
        }
        else {
            ctor.type = ConstructorType::Parameterized;
        }
    }
};

// =============================================================================
// مولد كود الباني
// =============================================================================

/**
 * مولد كود الباني
 * 
 * 💡 للمبتدئ:
 * يحول تعريف الباني إلى كود قابل للتنفيذ
 */
class ConstructorCodeGenerator {
public:
    /**
     * توليد كود C++ للباني
     */
    std::string generateCpp(const ConstructorDefinition& ctor) {
        std::ostringstream ss;
        
        // المُحددات
        if (ctor.isExplicit) ss << "explicit ";
        
        // اسم الباني والمعاملات
        ss << ctor.className << "(";
        for (size_t i = 0; i < ctor.parameters.size(); i++) {
            if (i > 0) ss << ", ";
            ss << generateParamCpp(ctor.parameters[i]);
        }
        ss << ")";
        
        // قائمة التهيئة
        if (!ctor.initializers.empty() || ctor.delegation) {
            ss << "\n    : ";
            bool first = true;
            
            // استدعاء الأصل أولاً
            if (ctor.delegation && ctor.delegation->isToSuper) {
                ss << "Parent(";
                for (size_t i = 0; i < ctor.delegation->arguments.size(); i++) {
                    if (i > 0) ss << ", ";
                    ss << ctor.delegation->arguments[i];
                }
                ss << ")";
                first = false;
            }
            
            // تهيئة الحقول
            for (const auto& init : ctor.initializers) {
                if (!first) ss << ", ";
                ss << init.fieldName << "(" << init.expression << ")";
                first = false;
            }
        }
        
        // الجسم
        ss << "\n{\n";
        for (const auto& stmt : ctor.bodyStatements) {
            ss << "    " << stmt << ";\n";
        }
        ss << "}\n";
        
        return ss.str();
    }
    
    /**
     * توليد كود لغة ص (للتوثيق)
     */
    std::string generateSad(const ConstructorDefinition& ctor) {
        std::ostringstream ss;
        
        // المُحددات
        if (ctor.visibility != "public") {
            ss << ctor.visibility << " ";
        }
        if (ctor.isExplicit) ss << "صريح ";
        
        ss << "باني(";
        for (size_t i = 0; i < ctor.parameters.size(); i++) {
            if (i > 0) ss << ", ";
            ss << generateParamSad(ctor.parameters[i]);
        }
        ss << ")";
        
        // استدعاء الأصل
        if (ctor.delegation && ctor.delegation->isToSuper) {
            ss << ": الأصل(";
            for (size_t i = 0; i < ctor.delegation->arguments.size(); i++) {
                if (i > 0) ss << ", ";
                ss << ctor.delegation->arguments[i];
            }
            ss << ")";
        }
        
        ss << ":\n";
        
        // التهيئة والأوامر
        for (const auto& init : ctor.initializers) {
            ss << "    هذا." << init.fieldName << " = " << init.expression << "\n";
        }
        for (const auto& stmt : ctor.bodyStatements) {
            ss << "    " << stmt << "\n";
        }
        
        ss << "نهاية\n";
        
        return ss.str();
    }
    
private:
    std::string generateParamCpp(const ConstructorParameter& param) {
        std::ostringstream ss;
        
        // النوع
        ss << mapTypeToCpp(param.type);
        if (param.isReference) {
            ss << (param.isMutable ? "& " : " const& ");
        } else {
            ss << " ";
        }
        
        // الاسم
        ss << param.name;
        
        // القيمة الافتراضية
        if (param.hasDefault) {
            ss << " = " << param.defaultValue;
        }
        
        return ss.str();
    }
    
    std::string generateParamSad(const ConstructorParameter& param) {
        std::ostringstream ss;
        
        ss << param.name << ": ";
        if (param.isReference) ss << "&";
        if (param.isMutable) ss << "متغير ";
        ss << param.type;
        
        if (param.hasDefault) {
            ss << " = " << param.defaultValue;
        }
        
        return ss.str();
    }
    
    std::string mapTypeToCpp(const std::string& sadType) {
        static std::map<std::string, std::string> typeMap = {
            {"ع8", "int8_t"},
            {"ع16", "int16_t"},
            {"ع32", "int32_t"},
            {"ع64", "int64_t"},
            {"ح8", "uint8_t"},
            {"ح16", "uint16_t"},
            {"ح32", "uint32_t"},
            {"ح64", "uint64_t"},
            {"عش32", "float"},
            {"عش64", "double"},
            {"منطقي", "bool"},
            {"نص", "std::string"},
            {"فراغ", "void"},
        };
        
        auto it = typeMap.find(sadType);
        return it != typeMap.end() ? it->second : sadType;
    }
};

// =============================================================================
// مُحقق الباني
// =============================================================================

/**
 * مُحقق صحة الباني
 * 
 * 💡 للمبتدئ:
 * يتحقق من أن الباني مكتوب بشكل صحيح
 */
class ConstructorValidator {
public:
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    ValidationResult validate(const ConstructorDefinition& ctor,
                             const std::vector<std::string>& classFields) {
        ValidationResult result;
        result.valid = true;
        
        // التحقق من تهيئة جميع الحقول
        std::vector<std::string> initializedFields;
        for (const auto& init : ctor.initializers) {
            initializedFields.push_back(init.fieldName);
        }
        
        for (const auto& field : classFields) {
            auto it = std::find(initializedFields.begin(), initializedFields.end(), field);
            if (it == initializedFields.end()) {
                result.warnings.push_back("الحقل '" + field + "' غير مُهيّأ في الباني");
            }
        }
        
        // التحقق من عدم تكرار التهيئة
        std::map<std::string, int> initCount;
        for (const auto& init : ctor.initializers) {
            initCount[init.fieldName]++;
        }
        for (const auto& [field, count] : initCount) {
            if (count > 1) {
                result.errors.push_back("الحقل '" + field + "' مُهيّأ أكثر من مرة");
                result.valid = false;
            }
        }
        
        // التحقق من معاملات بقيم افتراضية
        bool foundDefault = false;
        for (const auto& param : ctor.parameters) {
            if (param.hasDefault) {
                foundDefault = true;
            } else if (foundDefault) {
                result.errors.push_back(
                    "المعامل '" + param.name + "' بدون قيمة افتراضية بعد معامل له قيمة افتراضية");
                result.valid = false;
            }
        }
        
        // التحقق من استدعاء الأصل
        if (ctor.delegation && ctor.delegation->isToSuper && 
            ctor.delegation->isToThis) {
            result.errors.push_back("لا يمكن استدعاء الأصل وهذا معاً");
            result.valid = false;
        }
        
        return result;
    }
};

// =============================================================================
// واجهة C البرمجية
// =============================================================================

extern "C" {

/**
 * تحليل باني من نص
 */
void* sad_parse_constructor(const char* source, char** error_out) {
    try {
        std::string src(source);
        
        ConstructorLexer lexer(src);
        auto tokens = lexer.tokenize();
        
        ConstructorParser parser(tokens);
        auto* ctor = new ConstructorDefinition(parser.parse());
        
        return ctor;
    }
    catch (const ConstructorParseError& e) {
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
 * الحصول على عدد معاملات الباني
 */
int sad_constructor_param_count(void* ctor) {
    if (!ctor) return 0;
    return static_cast<int>(static_cast<ConstructorDefinition*>(ctor)->parameters.size());
}

/**
 * هل الباني افتراضي؟
 */
int sad_constructor_is_default(void* ctor) {
    if (!ctor) return 0;
    return static_cast<ConstructorDefinition*>(ctor)->isDefault() ? 1 : 0;
}

/**
 * تحرير ذاكرة الباني
 */
void sad_constructor_free(void* ctor) {
    if (ctor) delete static_cast<ConstructorDefinition*>(ctor);
}

} // extern "C"

// =============================================================================
// دالة الاختبار
// =============================================================================

#ifdef CONSTRUCTOR_PARSER_TEST

int main() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║     اختبار محلل الباني                                                      ║
║     Constructor Parser Test                                                  ║
║                                                                              ║
║     المرحلة 17 - T178                                                        ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;

    // كود باني للاختبار
    std::string testCode = R"(
باني(الاسم: نص, العمر: ع32 = 0):
    هذا.اسم = الاسم
    هذا.عمر = العمر
    اطبع("تم إنشاء شخص جديد")
نهاية
)";

    std::cout << "الكود المُدخل:\n" << testCode << "\n";
    std::cout << "─────────────────────────────────────────\n\n";

    try {
        ConstructorLexer lexer(testCode);
        auto tokens = lexer.tokenize();
        
        std::cout << "الرموز المستخرجة: " << tokens.size() << " رمز\n";
        
        ConstructorParser parser(tokens);
        auto ctor = parser.parse();
        
#ifdef DEBUG_OOP
        std::cout << "\nمعلومات الباني المستخرجة:\n";
#endif
        std::cout << "═══════════════════════════\n";
        ctor.print();
        
        // توليد الكود
        ConstructorCodeGenerator generator;
        
        std::cout << "\nكود C++ المولّد:\n";
        std::cout << "─────────────────\n";
        std::cout << generator.generateCpp(ctor);
        
        std::cout << "\nكود ص المولّد:\n";
        std::cout << "───────────────\n";
        std::cout << generator.generateSad(ctor);
        
        // التحقق
        ConstructorValidator validator;
        auto result = validator.validate(ctor, {"اسم", "عمر"});
        
        std::cout << "\nنتيجة التحقق:\n";
        std::cout << "─────────────\n";
        std::cout << (result.valid ? "✅ صالح\n" : "❌ غير صالح\n");
        
        for (const auto& err : result.errors) {
            std::cout << "  ❌ " << err << "\n";
        }
        for (const auto& warn : result.warnings) {
            std::cout << "  ⚠️ " << warn << "\n";
        }
        
        std::cout << "\n✅ نجح تحليل الباني!\n";
        
    } catch (const std::exception& e) {
        std::cout << "\n❌ خطأ: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

#endif // CONSTRUCTOR_PARSER_TEST
