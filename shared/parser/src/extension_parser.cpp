/**
 * =============================================================================
 * ملف: extension_parser.cpp
 * الوصف: محلل طرق الامتداد (Extension Methods)
 * المهمة: T180 - Implement extension methods (امتداد) parser
 * المرحلة: Phase 17 - User Story 15 (OOP صنف Sugar)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هي طرق الامتداد (Extension Methods)؟
 * ─────────────────────────────────────────────
 * تخيل أنك اشتريت سيارة جديدة 🚗:
 * - السيارة تأتي بميزات معينة (تشغيل، إطفاء، قيادة)
 * - لكنك تريد إضافة ميزة جديدة (مثل: تشغيل موسيقى)
 * - بدلاً من تعديل السيارة الأصلية، تضيف جهازاً جديداً
 * 
 * طرق الامتداد تفعل نفس الشيء للأنواع:
 * - تضيف دوال جديدة لنوع موجود
 * - بدون تعديل الكود الأصلي للنوع
 * 
 * مثال:
 * ```sad
 * // النوع الأصلي (لا نستطيع تعديله)
 * هيكل نص:
 *     // ...
 * نهاية
 * 
 * // نضيف دالة جديدة عبر امتداد
 * امتداد نص:
 *     دالة عدّ_الكلمات(&هذا) -> ع32:
 *         ارجع هذا.قسّم(" ").طول()
 *     نهاية
 * نهاية
 * 
 * // الاستخدام
 * متغير جملة = "مرحباً بالعالم العربي"
 * اطبع(جملة.عدّ_الكلمات())  // 3
 * ```
 * 
 * لماذا طرق الامتداد مفيدة؟
 * ──────────────────────────────
 * 1. إضافة وظائف لأنواع المكتبة القياسية
 * 2. تنظيم الكود في ملفات منفصلة
 * 3. إضافة وظائف خاصة بمشروعك
 * 4. الحفاظ على التغليف (لا تحتاج الوصول للكود الأصلي)
 * 
 * قيود طرق الامتداد:
 * ─────────────────────
 * 1. لا تستطيع الوصول للحقول الخاصة
 * 2. لا تستطيع تجاوز (override) دوال موجودة
 * 3. تحتاج استيراد الامتداد لاستخدامه
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <optional>
#include <sstream>
#include <iostream>
#include <stdexcept>

// =============================================================================
// هياكل البيانات
// =============================================================================

/**
 * دالة امتداد
 */
struct ExtensionMethod {
    std::string name;                       // اسم الدالة
    std::string targetType;                 // النوع المُمتد
    std::vector<std::string> typeParams;    // معاملات النوع
    bool takesMutableSelf;                  // هل تأخذ &متغير هذا؟
    std::string returnType;                 // نوع الإرجاع
    
    struct Parameter {
        std::string name;
        std::string type;
        bool hasDefault;
        std::string defaultValue;
    };
    std::vector<Parameter> parameters;
    
    std::vector<std::string> body;          // جسم الدالة
    std::string visibility;                 // عام/خاص
    
    int line = 0;
    int column = 0;
    
    ExtensionMethod() 
        : takesMutableSelf(false), visibility("public") {}
};

/**
 * كتلة امتداد
 */
struct ExtensionBlock {
    std::string targetType;                 // النوع المُمتد
    std::vector<std::string> typeParams;    // معاملات النوع
    std::string typeConstraint;             // قيود على النوع (حيث T: سمة)
    std::vector<ExtensionMethod> methods;   // الدوال
    
    int line = 0;
    int column = 0;
    
    /**
     * طباعة معلومات الكتلة
     */
    void print() const {
        std::cout << "امتداد " << targetType;
        if (!typeParams.empty()) {
            std::cout << "[";
            for (size_t i = 0; i < typeParams.size(); i++) {
                if (i > 0) std::cout << ", ";
                std::cout << typeParams[i];
            }
            std::cout << "]";
        }
        std::cout << ":\n";
        
        for (const auto& m : methods) {
            std::cout << "  - " << m.name << "(";
            std::cout << (m.takesMutableSelf ? "&متغير هذا" : "&هذا");
            for (const auto& p : m.parameters) {
                std::cout << ", " << p.name << ": " << p.type;
            }
            std::cout << ")";
            if (!m.returnType.empty()) {
                std::cout << " -> " << m.returnType;
            }
            std::cout << "\n";
        }
    }
};

// =============================================================================
// رموز المحلل المعجمي
// =============================================================================

enum class ExtTokenType {
    // كلمات محجوزة
    Extension,      // امتداد
    Function,       // دالة
    End,            // نهاية
    This,           // هذا
    Mutable,        // متغير
    Where,          // حيث
    Public,         // عام
    Private,        // خاص
    Return,         // ارجع
    
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
    Arrow,          // ->
    Ampersand,      // &
    Equals,         // =
    
    // خاص
    EndOfFile,
    Error,
};

struct ExtToken {
    ExtTokenType type;
    std::string value;
    int line;
    int column;
    
    ExtToken(ExtTokenType t, const std::string& v, int l, int c)
        : type(t), value(v), line(l), column(c) {}
};

// =============================================================================
// المحلل المعجمي
// =============================================================================

class ExtensionLexer {
public:
    explicit ExtensionLexer(const std::string& source)
        : source_(source), pos_(0), line_(1), column_(1) {}
    
    std::vector<ExtToken> tokenize() {
        std::vector<ExtToken> tokens;
        
        while (!isAtEnd()) {
            skipWhitespace();
            if (isAtEnd()) break;
            tokens.push_back(nextToken());
        }
        
        tokens.emplace_back(ExtTokenType::EndOfFile, "", line_, column_);
        return tokens;
    }
    
private:
    std::string source_;
    size_t pos_;
    int line_;
    int column_;
    
    std::map<std::string, ExtTokenType> keywords_ = {
        {"امتداد", ExtTokenType::Extension},
        {"دالة", ExtTokenType::Function},
        {"نهاية", ExtTokenType::End},
        {"هذا", ExtTokenType::This},
        {"متغير", ExtTokenType::Mutable},
        {"حيث", ExtTokenType::Where},
        {"عام", ExtTokenType::Public},
        {"خاص", ExtTokenType::Private},
        {"ارجع", ExtTokenType::Return},
        // English
        {"extension", ExtTokenType::Extension},
        {"fn", ExtTokenType::Function},
        {"function", ExtTokenType::Function},
        {"end", ExtTokenType::End},
        {"self", ExtTokenType::This},
        {"this", ExtTokenType::This},
        {"mut", ExtTokenType::Mutable},
        {"where", ExtTokenType::Where},
        {"return", ExtTokenType::Return},
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
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
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
    
    ExtToken nextToken() {
        char c = peek();
        int sl = line_, sc = column_;
        
        if (c == ':') { advance(); return ExtToken(ExtTokenType::Colon, ":", sl, sc); }
        if (c == ',') { advance(); return ExtToken(ExtTokenType::Comma, ",", sl, sc); }
        if (c == '.') { advance(); return ExtToken(ExtTokenType::Dot, ".", sl, sc); }
        if (c == '(') { advance(); return ExtToken(ExtTokenType::LeftParen, "(", sl, sc); }
        if (c == ')') { advance(); return ExtToken(ExtTokenType::RightParen, ")", sl, sc); }
        if (c == '[') { advance(); return ExtToken(ExtTokenType::LeftBracket, "[", sl, sc); }
        if (c == ']') { advance(); return ExtToken(ExtTokenType::RightBracket, "]", sl, sc); }
        if (c == '&') { advance(); return ExtToken(ExtTokenType::Ampersand, "&", sl, sc); }
        if (c == '=') { advance(); return ExtToken(ExtTokenType::Equals, "=", sl, sc); }
        
        if (c == '-' && peekNext() == '>') {
            advance(); advance();
            return ExtToken(ExtTokenType::Arrow, "->", sl, sc);
        }
        
        if (isDigit(c)) return readNumber(sl, sc);
        if (c == '"' || c == '\'') return readString(sl, sc);
        if (isIdentifierStart(c)) return readIdentifier(sl, sc);
        
        advance();
        return ExtToken(ExtTokenType::Error, std::string(1, c), sl, sc);
    }
    
    ExtToken readNumber(int sl, int sc) {
        std::string num;
        while (!isAtEnd() && (isDigit(peek()) || peek() == '.')) {
            num += advance();
        }
        return ExtToken(ExtTokenType::Number, num, sl, sc);
    }
    
    ExtToken readString(int sl, int sc) {
        char quote = advance();
        std::string str;
        while (!isAtEnd() && peek() != quote) {
            if (peek() == '\\') { advance(); if (!isAtEnd()) str += advance(); }
            else str += advance();
        }
        if (!isAtEnd()) advance();
        return ExtToken(ExtTokenType::String, str, sl, sc);
    }
    
    ExtToken readIdentifier(int sl, int sc) {
        std::string id;
        while (!isAtEnd() && isIdentifierChar(peek())) {
            id += advance();
        }
        auto it = keywords_.find(id);
        if (it != keywords_.end()) {
            return ExtToken(it->second, id, sl, sc);
        }
        return ExtToken(ExtTokenType::Identifier, id, sl, sc);
    }
    
    bool isDigit(char c) const { return c >= '0' && c <= '9'; }
    bool isIdentifierStart(char c) const {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
               c == '_' || static_cast<unsigned char>(c) >= 0x80;
    }
    bool isIdentifierChar(char c) const { return isIdentifierStart(c) || isDigit(c); }
};

// =============================================================================
// محلل الامتداد
// =============================================================================

class ExtensionParseError : public std::runtime_error {
public:
    ExtensionParseError(const std::string& msg, int line, int col)
        : std::runtime_error(msg), line_(line), column_(col) {}
    
    int line() const { return line_; }
    int column() const { return column_; }
    
    std::string toArabicString() const {
        std::ostringstream ss;
        ss << "خطأ في تحليل الامتداد [سطر " << line_ << "]: " << what();
        return ss.str();
    }
    
private:
    int line_;
    int column_;
};

/**
 * محلل كتل الامتداد
 */
class ExtensionParser {
public:
    explicit ExtensionParser(const std::vector<ExtToken>& tokens)
        : tokens_(tokens), current_(0) {}
    
    /**
     * تحليل كتلة امتداد كاملة
     * 
     * الصيغة:
     * امتداد نوع[T]:
     *     دالة ...
     * نهاية
     */
    ExtensionBlock parse() {
        ExtensionBlock block;
        block.line = peek().line;
        block.column = peek().column;
        
        // التحقق من "امتداد"
        if (!check(ExtTokenType::Extension)) {
            throw ExtensionParseError("متوقع 'امتداد'", peek().line, peek().column);
        }
        advance();
        
        // اسم النوع
        if (!check(ExtTokenType::Identifier)) {
            throw ExtensionParseError("متوقع اسم النوع بعد 'امتداد'", 
                                     peek().line, peek().column);
        }
        block.targetType = advance().value;
        
        // معاملات النوع [T]
        if (check(ExtTokenType::LeftBracket)) {
            advance();
            while (!check(ExtTokenType::RightBracket) && !isAtEnd()) {
                if (check(ExtTokenType::Identifier)) {
                    block.typeParams.push_back(advance().value);
                }
                if (check(ExtTokenType::Comma)) advance();
                else break;
            }
            if (!check(ExtTokenType::RightBracket)) {
                throw ExtensionParseError("متوقع ']'", peek().line, peek().column);
            }
            advance();
        }
        
        // قيود النوع (حيث T: سمة)
        if (check(ExtTokenType::Where)) {
            advance();
            block.typeConstraint = parseTypeConstraint();
        }
        
        // :
        if (check(ExtTokenType::Colon)) advance();
        
        // الدوال
        while (!check(ExtTokenType::End) && !isAtEnd()) {
            if (check(ExtTokenType::Function) || 
                check(ExtTokenType::Public) || 
                check(ExtTokenType::Private)) {
                auto method = parseMethod(block.targetType);
                block.methods.push_back(method);
            } else {
                advance(); // تخطي رموز غير معروفة
            }
        }
        
        // نهاية
        if (!check(ExtTokenType::End)) {
            throw ExtensionParseError("متوقع 'نهاية'", peek().line, peek().column);
        }
        advance();
        
        return block;
    }
    
private:
    std::vector<ExtToken> tokens_;
    size_t current_;
    
    bool isAtEnd() const { return current_ >= tokens_.size() || peek().type == ExtTokenType::EndOfFile; }
    const ExtToken& peek() const { return tokens_[current_]; }
    const ExtToken& previous() const { return tokens_[current_ - 1]; }
    ExtToken advance() { if (!isAtEnd()) current_++; return previous(); }
    bool check(ExtTokenType type) const { return !isAtEnd() && peek().type == type; }
    
    std::string parseTypeConstraint() {
        std::string constraint;
        while (!check(ExtTokenType::Colon) && !check(ExtTokenType::End) && !isAtEnd()) {
            constraint += peek().value + " ";
            advance();
        }
        return constraint;
    }
    
    ExtensionMethod parseMethod(const std::string& targetType) {
        ExtensionMethod method;
        method.targetType = targetType;
        method.line = peek().line;
        method.column = peek().column;
        
        // مُحدد الوصول
        if (check(ExtTokenType::Public)) {
            method.visibility = "public";
            advance();
        } else if (check(ExtTokenType::Private)) {
            method.visibility = "private";
            advance();
        }
        
        // دالة
        if (!check(ExtTokenType::Function)) {
            throw ExtensionParseError("متوقع 'دالة'", peek().line, peek().column);
        }
        advance();
        
        // اسم الدالة
        if (!check(ExtTokenType::Identifier)) {
            throw ExtensionParseError("متوقع اسم الدالة", peek().line, peek().column);
        }
        method.name = advance().value;
        
        // معاملات النوع للدالة
        if (check(ExtTokenType::LeftBracket)) {
            advance();
            while (!check(ExtTokenType::RightBracket) && !isAtEnd()) {
                if (check(ExtTokenType::Identifier)) {
                    method.typeParams.push_back(advance().value);
                }
                if (check(ExtTokenType::Comma)) advance();
                else break;
            }
            if (check(ExtTokenType::RightBracket)) advance();
        }
        
        // (
        if (!check(ExtTokenType::LeftParen)) {
            throw ExtensionParseError("متوقع '('", peek().line, peek().column);
        }
        advance();
        
        // المعاملات - أول معامل هو &هذا أو &متغير هذا
        if (check(ExtTokenType::Ampersand)) {
            advance();
            if (check(ExtTokenType::Mutable)) {
                method.takesMutableSelf = true;
                advance();
            }
            if (!check(ExtTokenType::This)) {
                throw ExtensionParseError("متوقع 'هذا' بعد '&'", peek().line, peek().column);
            }
            advance();
            
            if (check(ExtTokenType::Comma)) advance();
        }
        
        // باقي المعاملات
        while (!check(ExtTokenType::RightParen) && !isAtEnd()) {
            ExtensionMethod::Parameter param;
            
            if (!check(ExtTokenType::Identifier)) break;
            param.name = advance().value;
            
            if (check(ExtTokenType::Colon)) {
                advance();
                param.type = parseType();
            }
            
            if (check(ExtTokenType::Equals)) {
                advance();
                param.hasDefault = true;
                param.defaultValue = parseExpression();
            }
            
            method.parameters.push_back(param);
            
            if (check(ExtTokenType::Comma)) advance();
            else break;
        }
        
        // )
        if (!check(ExtTokenType::RightParen)) {
            throw ExtensionParseError("متوقع ')'", peek().line, peek().column);
        }
        advance();
        
        // نوع الإرجاع
        if (check(ExtTokenType::Arrow)) {
            advance();
            method.returnType = parseType();
        }
        
        // :
        if (check(ExtTokenType::Colon)) advance();
        
        // جسم الدالة
        while (!check(ExtTokenType::End) && !isAtEnd()) {
            // التحقق من بداية دالة جديدة
            if (check(ExtTokenType::Function) || 
                check(ExtTokenType::Public) || 
                check(ExtTokenType::Private)) {
                break;
            }
            
            std::string stmt = parseStatement();
            if (!stmt.empty()) {
                method.body.push_back(stmt);
            }
        }
        
        // نهاية
        if (check(ExtTokenType::End)) {
            advance();
        }
        
        return method;
    }
    
    std::string parseType() {
        std::string type;
        
        if (check(ExtTokenType::Ampersand)) {
            type += "&";
            advance();
        }
        
        if (check(ExtTokenType::Mutable)) {
            type += "متغير ";
            advance();
        }
        
        if (check(ExtTokenType::Identifier)) {
            type += advance().value;
        }
        
        // معاملات النوع
        if (check(ExtTokenType::LeftBracket)) {
            type += "[";
            advance();
            while (!check(ExtTokenType::RightBracket) && !isAtEnd()) {
                type += peek().value;
                advance();
                if (check(ExtTokenType::Comma)) {
                    type += ", ";
                    advance();
                }
            }
            type += "]";
            if (check(ExtTokenType::RightBracket)) advance();
        }
        
        return type;
    }
    
    std::string parseExpression() {
        std::string expr;
        int depth = 0;
        
        while (!isAtEnd()) {
            if (check(ExtTokenType::LeftParen)) depth++;
            if (check(ExtTokenType::RightParen)) {
                if (depth == 0) break;
                depth--;
            }
            if (check(ExtTokenType::Comma) && depth == 0) break;
            if (check(ExtTokenType::End)) break;
            
            expr += peek().value;
            advance();
        }
        
        return expr;
    }
    
    std::string parseStatement() {
        std::string stmt;
        
        while (!isAtEnd() && !check(ExtTokenType::End)) {
            if (check(ExtTokenType::Function) || 
                check(ExtTokenType::Public) || 
                check(ExtTokenType::Private)) {
                break;
            }
            
            stmt += peek().value + " ";
            advance();
            
            // نهاية السطر المنطقية
            if (stmt.find("نهاية") != std::string::npos) {
                current_--;
                stmt = stmt.substr(0, stmt.rfind("نهاية"));
                break;
            }
        }
        
        // تنظيف
        while (!stmt.empty() && stmt.back() == ' ') {
            stmt.pop_back();
        }
        
        return stmt;
    }
};

// =============================================================================
// مولد كود الامتداد
// =============================================================================

/**
 * يحول الامتداد إلى كتلة نفّذ
 */
class ExtensionCodeGenerator {
public:
    /**
     * توليد كود لغة ص
     */
    std::string generateSadCode(const ExtensionBlock& block) {
        std::ostringstream ss;
        
        ss << "// امتداد للنوع: " << block.targetType << "\n";
        ss << "نفّذ " << block.targetType;
        
        if (!block.typeParams.empty()) {
            ss << "[";
            for (size_t i = 0; i < block.typeParams.size(); i++) {
                if (i > 0) ss << ", ";
                ss << block.typeParams[i];
            }
            ss << "]";
        }
        
        ss << ":\n";
        
        for (const auto& method : block.methods) {
            ss << generateMethodCode(method);
        }
        
        ss << "نهاية\n";
        
        return ss.str();
    }
    
private:
    std::string generateMethodCode(const ExtensionMethod& method) {
        std::ostringstream ss;
        
        ss << "    دالة " << method.name;
        
        if (!method.typeParams.empty()) {
            ss << "[";
            for (size_t i = 0; i < method.typeParams.size(); i++) {
                if (i > 0) ss << ", ";
                ss << method.typeParams[i];
            }
            ss << "]";
        }
        
        ss << "(&";
        if (method.takesMutableSelf) ss << "متغير ";
        ss << "هذا";
        
        for (const auto& param : method.parameters) {
            ss << ", " << param.name << ": " << param.type;
            if (param.hasDefault) {
                ss << " = " << param.defaultValue;
            }
        }
        
        ss << ")";
        
        if (!method.returnType.empty()) {
            ss << " -> " << method.returnType;
        }
        
        ss << ":\n";
        
        for (const auto& stmt : method.body) {
            ss << "        " << stmt << "\n";
        }
        
        ss << "    نهاية\n\n";
        
        return ss.str();
    }
};

// =============================================================================
// مسجل الامتدادات
// =============================================================================

/**
 * يتتبع جميع الامتدادات المسجلة
 * 
 * 💡 للمبتدئ:
 * عند البحث عن دالة لنوع معين، نبحث أولاً في الامتدادات
 */
class ExtensionRegistry {
public:
    static ExtensionRegistry& instance() {
        static ExtensionRegistry registry;
        return registry;
    }
    
    void registerExtension(const ExtensionBlock& block) {
        extensions_[block.targetType].push_back(block);
    }
    
    std::vector<ExtensionMethod> getMethodsForType(const std::string& type) const {
        std::vector<ExtensionMethod> methods;
        
        auto it = extensions_.find(type);
        if (it != extensions_.end()) {
            for (const auto& block : it->second) {
                for (const auto& method : block.methods) {
                    methods.push_back(method);
                }
            }
        }
        
        return methods;
    }
    
    bool hasMethod(const std::string& type, const std::string& methodName) const {
        auto methods = getMethodsForType(type);
        for (const auto& m : methods) {
            if (m.name == methodName) return true;
        }
        return false;
    }
    
    void clear() {
        extensions_.clear();
    }
    
private:
    ExtensionRegistry() = default;
    std::map<std::string, std::vector<ExtensionBlock>> extensions_;
};

// =============================================================================
// واجهة C البرمجية
// =============================================================================

extern "C" {

void* sad_parse_extension(const char* source, char** error_out) {
    try {
        ExtensionLexer lexer(source);
        auto tokens = lexer.tokenize();
        
        ExtensionParser parser(tokens);
        auto* block = new ExtensionBlock(parser.parse());
        
        return block;
    }
    catch (const ExtensionParseError& e) {
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

const char* sad_extension_get_type(void* ext) {
    if (!ext) return "";
    return static_cast<ExtensionBlock*>(ext)->targetType.c_str();
}

int sad_extension_get_method_count(void* ext) {
    if (!ext) return 0;
    return static_cast<int>(static_cast<ExtensionBlock*>(ext)->methods.size());
}

void sad_extension_free(void* ext) {
    if (ext) delete static_cast<ExtensionBlock*>(ext);
}

} // extern "C"

// =============================================================================
// دالة الاختبار
// =============================================================================

#ifdef EXTENSION_PARSER_TEST

int main() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║     اختبار محلل الامتداد                                                    ║
║     Extension Parser Test                                                    ║
║                                                                              ║
║     المرحلة 17 - T180                                                        ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;

    std::string testCode = R"(
امتداد نص:
    دالة عدّ_الكلمات(&هذا) -> ع32:
        ارجع هذا.قسّم(" ").طول()
    نهاية
    
    دالة عكس(&هذا) -> نص:
        متغير نتيجة = ""
        لكل ح في هذا:
            نتيجة = ح + نتيجة
        نهاية
        ارجع نتيجة
    نهاية
    
    دالة كرر(&هذا, عدد: ع32) -> نص:
        متغير نتيجة = ""
        لكل _ في مدى(عدد):
            نتيجة = نتيجة + هذا
        نهاية
        ارجع نتيجة
    نهاية
نهاية
)";

    std::cout << "الكود المُدخل:\n" << testCode << "\n";
    std::cout << "─────────────────────────────────────────\n\n";

    try {
        ExtensionLexer lexer(testCode);
        auto tokens = lexer.tokenize();
        
        std::cout << "الرموز المستخرجة: " << tokens.size() << " رمز\n\n";
        
        ExtensionParser parser(tokens);
        auto block = parser.parse();
        
        std::cout << "معلومات الامتداد:\n";
        std::cout << "═══════════════════\n";
        block.print();
        
        // توليد الكود
        ExtensionCodeGenerator generator;
        std::string code = generator.generateSadCode(block);
        
        std::cout << "\nالكود المُحوَّل:\n";
        std::cout << "─────────────────\n";
        std::cout << code;
        
        std::cout << "\n✅ نجح تحليل الامتداد!\n";
        
    } catch (const std::exception& e) {
        std::cout << "\n❌ خطأ: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

#endif // EXTENSION_PARSER_TEST
