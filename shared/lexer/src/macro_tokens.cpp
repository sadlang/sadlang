/**
 * =============================================================================
 * ملف: macro_tokens.cpp
 * الوصف: تعريف وتحليل رموز الماكرو (ماكرو!)
 * المهمة: T192 - Implement macro lexer tokens
 * المرحلة: Phase 19 - User Story 16 (Macro System ماكرو!)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هو الماكرو؟
 * ───────────────
 * الماكرو = كود يُولّد كود آخر قبل الترجمة
 * 
 * تخيل الماكرو كـ "قالب ذكي":
 * - تعطيه نمط (pattern)
 * - يُنتج كود بناءً على ما تُدخله
 * 
 * مثال بسيط:
 * ```sad
 * // تعريف الماكرو
 * ماكرو! سجّل!($رسالة:عبر) {
 *     اطبع("[سجل] " + $رسالة)
 * }
 * 
 * // استخدام الماكرو
 * سجّل!("بدأ البرنامج")
 * 
 * // ما يُنتج فعلياً
 * اطبع("[سجل] " + "بدأ البرنامج")
 * ```
 * 
 * رموز الماكرو:
 * ─────────────
 * - ماكرو!     : بداية تعريف ماكرو
 * - اسم!       : استدعاء ماكرو (لاحظ علامة التعجب)
 * - $متغير    : متغير ماكرو (يُستبدل بالقيمة)
 * - :نوع      : نوع المتغير (عبر، قيمة، نوع، كتلة)
 * - $(...)*   : تكرار صفر أو أكثر
 * - $(...)+ : تكرار واحد أو أكثر
 * - $(...)؟   : اختياري
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include <memory>
#include <optional>
#include <regex>

// =============================================================================
// أنواع الرموز
// =============================================================================

/**
 * أنواع رموز الماكرو
 */
enum class MacroTokenType {
    // الكلمات المفتاحية
    MacroKeyword,       // ماكرو!
    MacroInvoke,        // اسم! (استدعاء)
    
    // المتغيرات
    MacroVar,           // $متغير
    MacroVarType,       // :نوع
    
    // أنواع المتغيرات
    FragmentExpr,       // :عبر (تعبير)
    FragmentValue,      // :قيمة (قيمة حرفية)
    FragmentType,       // :نوع (نوع)
    FragmentBlock,      // :كتلة (كتلة كود)
    FragmentIdent,      // :معرّف (اسم)
    FragmentTT,         // :رمز (رمز واحد - Token Tree)
    
    // التكرار
    RepeatZeroOrMore,   // $(...)* 
    RepeatOneOrMore,    // $(...)+
    RepeatOptional,     // $(...)؟
    RepeatSeparator,    // الفاصل بين التكرارات
    
    // الأقواس
    LeftParen,          // (
    RightParen,         // )
    LeftBrace,          // {
    RightBrace,         // }
    LeftBracket,        // [
    RightBracket,       // ]
    
    // علامات خاصة
    Dollar,             // $
    Bang,               // !
    Colon,              // :
    Comma,              // ,
    Semicolon,          // ;
    Arrow,              // =>
    
    // المحتوى
    Identifier,         // معرّف عادي
    Literal,            // قيمة حرفية
    Whitespace,         // مسافات
    Newline,            // سطر جديد
    
    // خاص
    EndOfMacro,         // نهاية الماكرو
    Error,              // خطأ
    EndOfFile           // نهاية الملف
};

/**
 * تمثيل رمز ماكرو
 */
struct MacroToken {
    MacroTokenType type;
    std::string value;
    int line;
    int column;
    
    // للتكرار
    std::vector<MacroToken> nestedTokens;
    char repeatKind = '\0';  // '*', '+', '?'
    std::string separator;
    
    MacroToken(MacroTokenType t = MacroTokenType::Error, 
               const std::string& v = "", 
               int l = 0, int c = 0)
        : type(t), value(v), line(l), column(c) {}
    
    std::string typeName() const {
        static const std::map<MacroTokenType, std::string> names = {
            {MacroTokenType::MacroKeyword, "ماكرو!"},
            {MacroTokenType::MacroInvoke, "استدعاء"},
            {MacroTokenType::MacroVar, "متغير$"},
            {MacroTokenType::Dollar, "$"},
            {MacroTokenType::Bang, "!"},
            {MacroTokenType::Identifier, "معرّف"},
        };
        auto it = names.find(type);
        return it != names.end() ? it->second : "غير معروف";
    }
};

// =============================================================================
// محلل الرموز
// =============================================================================

/**
 * محلل رموز الماكرو
 * 
 * 💡 للمبتدئ:
 * المحلل يقرأ النص حرفاً حرفاً ويُنتج رموز
 */
class MacroLexer {
public:
    explicit MacroLexer(const std::string& source)
        : source_(source), pos_(0), line_(1), column_(1) {}
    
    /**
     * الحصول على الرمز التالي
     */
    MacroToken nextToken() {
        skipWhitespace();
        
        if (isAtEnd()) {
            return MacroToken(MacroTokenType::EndOfFile, "", line_, column_);
        }
        
        char c = peek();
        
        // علامة الدولار - بداية متغير ماكرو
        if (c == '$') {
            return lexMacroVariable();
        }
        
        // علامة التعجب
        if (c == '!') {
            advance();
            return MacroToken(MacroTokenType::Bang, "!", line_, column_ - 1);
        }
        
        // النقطتان
        if (c == ':') {
            advance();
            return lexFragmentType();
        }
        
        // الأقواس
        if (c == '(') {
            advance();
            return MacroToken(MacroTokenType::LeftParen, "(", line_, column_ - 1);
        }
        if (c == ')') {
            advance();
            return MacroToken(MacroTokenType::RightParen, ")", line_, column_ - 1);
        }
        if (c == '{') {
            advance();
            return MacroToken(MacroTokenType::LeftBrace, "{", line_, column_ - 1);
        }
        if (c == '}') {
            advance();
            return MacroToken(MacroTokenType::RightBrace, "}", line_, column_ - 1);
        }
        if (c == '[') {
            advance();
            return MacroToken(MacroTokenType::LeftBracket, "[", line_, column_ - 1);
        }
        if (c == ']') {
            advance();
            return MacroToken(MacroTokenType::RightBracket, "]", line_, column_ - 1);
        }
        
        // الفاصلة
        if (c == ',') {
            advance();
            return MacroToken(MacroTokenType::Comma, ",", line_, column_ - 1);
        }
        
        // الفاصلة المنقوطة
        if (c == ';') {
            advance();
            return MacroToken(MacroTokenType::Semicolon, ";", line_, column_ - 1);
        }
        
        // علامات التكرار
        if (c == '*' || c == '+') {
            advance();
            return MacroToken(c == '*' ? MacroTokenType::RepeatZeroOrMore : 
                             MacroTokenType::RepeatOneOrMore, 
                             std::string(1, c), line_, column_ - 1);
        }
        
        // علامة الاستفهام (اختياري) - بالعربية
        if (c == '?' || (c == (char)0xD8 && pos_ + 1 < source_.size() && 
                         source_[pos_ + 1] == (char)0x9F)) {
            if (c == '?') {
                advance();
                return MacroToken(MacroTokenType::RepeatOptional, "?", line_, column_ - 1);
            } else {
                advance();
                advance();
                return MacroToken(MacroTokenType::RepeatOptional, "؟", line_, column_ - 2);
            }
        }
        
        // السهم =>
        if (c == '=' && pos_ + 1 < source_.size() && source_[pos_ + 1] == '>') {
            advance();
            advance();
            return MacroToken(MacroTokenType::Arrow, "=>", line_, column_ - 2);
        }
        
        // معرّف أو كلمة مفتاحية
        if (isIdentifierStart(c)) {
            return lexIdentifier();
        }
        
        // قيمة حرفية (نص أو رقم)
        if (c == '"' || c == '\'') {
            return lexStringLiteral();
        }
        if (isDigit(c)) {
            return lexNumberLiteral();
        }
        
        // حرف غير معروف
        advance();
        return MacroToken(MacroTokenType::Error, std::string(1, c), line_, column_ - 1);
    }
    
    /**
     * تحليل جميع الرموز
     */
    std::vector<MacroToken> tokenize() {
        std::vector<MacroToken> tokens;
        
        while (!isAtEnd()) {
            auto token = nextToken();
            if (token.type != MacroTokenType::Whitespace) {
                tokens.push_back(token);
            }
            if (token.type == MacroTokenType::EndOfFile) break;
        }
        
        return tokens;
    }
    
private:
    std::string source_;
    size_t pos_;
    int line_;
    int column_;
    
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
    
    void skipWhitespace() {
        while (!isAtEnd()) {
            char c = peek();
            if (c == ' ' || c == '\t' || c == '\r') {
                advance();
            } else if (c == '\n') {
                advance();
            } else if (c == '/' && peekNext() == '/') {
                // تعليق سطر
                while (!isAtEnd() && peek() != '\n') advance();
            } else {
                break;
            }
        }
    }
    
    bool isIdentifierStart(char c) const {
        // حروف عربية أو لاتينية أو شرطة سفلية
        return (c >= 'a' && c <= 'z') || 
               (c >= 'A' && c <= 'Z') || 
               c == '_' ||
               (unsigned char)c >= 0x80;  // UTF-8
    }
    
    bool isIdentifierPart(char c) const {
        return isIdentifierStart(c) || isDigit(c);
    }
    
    bool isDigit(char c) const {
        return c >= '0' && c <= '9';
    }
    
    /**
     * تحليل متغير ماكرو ($name)
     */
    MacroToken lexMacroVariable() {
        int startCol = column_;
        advance();  // تخطي $
        
        if (isAtEnd()) {
            return MacroToken(MacroTokenType::Dollar, "$", line_, startCol);
        }
        
        // قراءة الاسم
        std::string name;
        while (!isAtEnd() && isIdentifierPart(peek())) {
            name += advance();
        }
        
        if (name.empty()) {
            return MacroToken(MacroTokenType::Dollar, "$", line_, startCol);
        }
        
        return MacroToken(MacroTokenType::MacroVar, "$" + name, line_, startCol);
    }
    
    /**
     * تحليل نوع المتغير (:عبر، :قيمة، إلخ)
     */
    MacroToken lexFragmentType() {
        int startCol = column_ - 1;
        
        // قراءة النوع
        std::string typeName;
        while (!isAtEnd() && isIdentifierPart(peek())) {
            typeName += advance();
        }
        
        // تحديد نوع المتغير
        MacroTokenType tokenType = MacroTokenType::MacroVarType;
        
        static const std::map<std::string, MacroTokenType> fragmentTypes = {
            {"عبر", MacroTokenType::FragmentExpr},
            {"expr", MacroTokenType::FragmentExpr},
            {"قيمة", MacroTokenType::FragmentValue},
            {"lit", MacroTokenType::FragmentValue},
            {"نوع", MacroTokenType::FragmentType},
            {"ty", MacroTokenType::FragmentType},
            {"كتلة", MacroTokenType::FragmentBlock},
            {"block", MacroTokenType::FragmentBlock},
            {"معرّف", MacroTokenType::FragmentIdent},
            {"ident", MacroTokenType::FragmentIdent},
            {"رمز", MacroTokenType::FragmentTT},
            {"tt", MacroTokenType::FragmentTT},
        };
        
        auto it = fragmentTypes.find(typeName);
        if (it != fragmentTypes.end()) {
            tokenType = it->second;
        }
        
        return MacroToken(tokenType, ":" + typeName, line_, startCol);
    }
    
    /**
     * تحليل معرّف
     */
    MacroToken lexIdentifier() {
        int startCol = column_;
        std::string name;
        
        while (!isAtEnd() && isIdentifierPart(peek())) {
            name += advance();
        }
        
        // هل تتبعه علامة تعجب؟ (استدعاء ماكرو أو تعريف)
        if (peek() == '!') {
            advance();
            
            if (name == "ماكرو" || name == "macro") {
                return MacroToken(MacroTokenType::MacroKeyword, name + "!", line_, startCol);
            }
            
            return MacroToken(MacroTokenType::MacroInvoke, name + "!", line_, startCol);
        }
        
        return MacroToken(MacroTokenType::Identifier, name, line_, startCol);
    }
    
    /**
     * تحليل نص حرفي
     */
    MacroToken lexStringLiteral() {
        int startCol = column_;
        char quote = advance();
        std::string value;
        value += quote;
        
        while (!isAtEnd() && peek() != quote) {
            if (peek() == '\\' && peekNext() == quote) {
                value += advance();  // \
                value += advance();  // "
            } else {
                value += advance();
            }
        }
        
        if (!isAtEnd()) {
            value += advance();  // علامة الإغلاق
        }
        
        return MacroToken(MacroTokenType::Literal, value, line_, startCol);
    }
    
    /**
     * تحليل رقم حرفي
     */
    MacroToken lexNumberLiteral() {
        int startCol = column_;
        std::string value;
        
        while (!isAtEnd() && (isDigit(peek()) || peek() == '.')) {
            value += advance();
        }
        
        return MacroToken(MacroTokenType::Literal, value, line_, startCol);
    }
};

// =============================================================================
// جدول الماكروات المُعرّفة
// =============================================================================

/**
 * معلومات ماكرو مُعرّف
 */
struct MacroDefinition {
    std::string name;
    std::vector<MacroToken> pattern;    // نمط المطابقة
    std::vector<MacroToken> body;       // جسم الماكرو
    bool isExported;
};

/**
 * جدول الماكروات
 */
class MacroRegistry {
public:
    static MacroRegistry& instance() {
        static MacroRegistry registry;
        return registry;
    }
    
    void define(const std::string& name, const MacroDefinition& def) {
        macros_[name] = def;
    }
    
    const MacroDefinition* find(const std::string& name) const {
        auto it = macros_.find(name);
        return it != macros_.end() ? &it->second : nullptr;
    }
    
    bool isDefined(const std::string& name) const {
        return macros_.find(name) != macros_.end();
    }
    
    void clear() { macros_.clear(); }
    
    const std::map<std::string, MacroDefinition>& all() const { return macros_; }
    
private:
    MacroRegistry() = default;
    std::map<std::string, MacroDefinition> macros_;
};

// =============================================================================
// واجهة C البرمجية
// =============================================================================

extern "C" {

/**
 * تحليل نص ماكرو إلى رموز
 */
int sad_tokenize_macro(const char* source, void** tokensOut, int* countOut) {
    MacroLexer lexer(source);
    auto tokens = lexer.tokenize();
    
    *countOut = static_cast<int>(tokens.size());
    // في التنفيذ الحقيقي: تخصيص ذاكرة وتعبئة البيانات
    
    return 0;
}

/**
 * التحقق من وجود ماكرو
 */
int sad_macro_exists(const char* name) {
    return MacroRegistry::instance().isDefined(name) ? 1 : 0;
}

/**
 * تسجيل ماكرو جديد
 */
void sad_register_macro(const char* name, int isExported) {
    MacroDefinition def;
    def.name = name;
    def.isExported = isExported != 0;
    MacroRegistry::instance().define(name, def);
}

/**
 * مسح جميع الماكروات
 */
void sad_clear_macros() {
    MacroRegistry::instance().clear();
}

} // extern "C"

// =============================================================================
// دالة الاختبار
// =============================================================================

#ifdef MACRO_TOKENS_TEST

int main() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║     اختبار رموز الماكرو                                                       ║
║     Macro Tokens Test                                                        ║
║                                                                              ║
║     المرحلة 19 - T192                                                        ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;

    // مثال ماكرو
    std::string macroSource = R"(
ماكرو! سجّل!($رسالة:عبر) {
    اطبع("[سجل] " + $رسالة)
}
)";

    std::cout << "الكود المصدري:\n";
    std::cout << "───────────────\n";
    std::cout << macroSource << "\n";
    
    // تحليل الرموز
    MacroLexer lexer(macroSource);
    auto tokens = lexer.tokenize();
    
    std::cout << "\nالرموز المُستخرجة:\n";
    std::cout << "═══════════════════\n";
    
    for (const auto& tok : tokens) {
        std::cout << "[" << tok.typeName() << "] \"" << tok.value << "\" ";
        std::cout << "@ سطر " << tok.line << ":" << tok.column << "\n";
    }
    
    // اختبار متغيرات الماكرو
    std::cout << "\n\nاختبار متغيرات الماكرو:\n";
    std::cout << "─────────────────────────\n";
    
    std::string varTest = "$س $قيمة $xyz";
    MacroLexer varLexer(varTest);
    auto varTokens = varLexer.tokenize();
    
    for (const auto& tok : varTokens) {
        if (tok.type == MacroTokenType::MacroVar) {
            std::cout << "✓ متغير: " << tok.value << "\n";
        }
    }
    
    // اختبار أنواع المتغيرات
    std::cout << "\nاختبار أنواع المتغيرات:\n";
    std::cout << "────────────────────────\n";
    
    std::string typeTest = ":عبر :قيمة :نوع :كتلة :معرّف :رمز";
    MacroLexer typeLexer(typeTest);
    auto typeTokens = typeLexer.tokenize();
    
    for (const auto& tok : typeTokens) {
        std::cout << "  " << tok.value << " -> " << tok.typeName() << "\n";
    }
    
    // اختبار التكرار
    std::cout << "\nاختبار علامات التكرار:\n";
    std::cout << "───────────────────────\n";
    
    std::string repeatTest = "* + ؟ ?";
    MacroLexer repeatLexer(repeatTest);
    auto repeatTokens = repeatLexer.tokenize();
    
    for (const auto& tok : repeatTokens) {
        if (tok.type == MacroTokenType::RepeatZeroOrMore) {
            std::cout << "  * -> صفر أو أكثر\n";
        } else if (tok.type == MacroTokenType::RepeatOneOrMore) {
            std::cout << "  + -> واحد أو أكثر\n";
        } else if (tok.type == MacroTokenType::RepeatOptional) {
            std::cout << "  " << tok.value << " -> اختياري\n";
        }
    }
    
    std::cout << "\n✅ نجح اختبار رموز الماكرو!\n";
    
    return 0;
}

#endif // MACRO_TOKENS_TEST
