/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: error_recovery.cpp                                                   ║
 * ║   الوصف: استرداد أخطاء المحلل النحوي                                        ║
 * ║   المهمة: T163 - Phase 15 (US13)                                            ║
 * ║   المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                         دليل المبتدئين | Beginner's Guide
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 ما هو استرداد الأخطاء؟ | What is Error Recovery?
 * ─────────────────────────────────────────────────────
 *
 * عندما يجد المترجم خطأً في الكود، لديه خياران:
 *
 *    1️⃣ التوقف فوراً: يُظهر خطأ واحداً ثم يتوقف
 *    2️⃣ الاستمرار: يُصلح نفسه ويكمل للعثور على أخطاء أخرى
 *
 * استرداد الأخطاء يعني الخيار الثاني! 🎯
 *
 * 💡 لماذا هذا مهم؟
 * ─────────────────────────────────────────────────────
 *
 * تخيل أنك تكتب مقالاً وفيه 10 أخطاء إملائية:
 *
 *    ❌ بدون استرداد: يُظهر الخطأ الأول فقط
 *       تصلحه، تشغّل مرة أخرى، يُظهر الثاني...
 *       وهكذا 10 مرات! 😫
 *
 *    ✅ مع استرداد: يُظهر كل الأخطاء العشرة مرة واحدة
 *       تصلحها كلها، تشغّل مرة واحدة فقط! 😊
 *
 * 🔧 كيف يعمل؟
 * ─────────────────────────────────────────────────────
 *
 *    الكود: دالة خطأ(
 *                    ^ توقع )
 *
 *    بدلاً من التوقف، المترجم:
 *    1. يسجل الخطأ
 *    2. يبحث عن "نقطة آمنة" (مثل نهاية السطر)
 *    3. يكمل من النقطة الآمنة
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <string>
#include <vector>
#include <unordered_set>
#include <functional>
#include <memory>
#include <sstream>

namespace sad {
namespace parser {

// ═══════════════════════════════════════════════════════════════════════════════
//                          أنواع الرموز | Token Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔤 TokenType - أنواع الرموز في لغة سعد
 */
enum class TokenType {
    // نهاية الملف
    END_OF_FILE,
    
    // الكلمات المحجوزة
    KEYWORD_FUNCTION,    // دالة
    KEYWORD_IF,          // إذا
    KEYWORD_ELSE,        // وإلا
    KEYWORD_WHILE,       // طالما
    KEYWORD_FOR,         // لكل
    KEYWORD_RETURN,      // أرجع
    KEYWORD_CLASS,       // صنف
    KEYWORD_IMPORT,      // استورد
    KEYWORD_VAR,         // متغير
    KEYWORD_CONST,       // ثابت
    KEYWORD_TRUE,        // صحيح
    KEYWORD_FALSE,       // خطأ
    KEYWORD_NULL,        // عدم
    KEYWORD_MATCH,       // طابق
    KEYWORD_ASYNC,       // متزامن
    KEYWORD_AWAIT,       // انتظر
    
    // المعرفات والقيم
    IDENTIFIER,
    NUMBER_LITERAL,
    STRING_LITERAL,
    
    // الرموز
    LPAREN,              // (
    RPAREN,              // )
    LBRACE,              // {
    RBRACE,              // }
    LBRACKET,            // [
    RBRACKET,            // ]
    COMMA,               // ,
    COLON,               // :
    SEMICOLON,           // ;
    DOT,                 // .
    ARROW,               // ->
    EQUALS,              // =
    
    // العمليات
    PLUS,                // +
    MINUS,               // -
    STAR,                // *
    SLASH,               // /
    PERCENT,             // %
    
    // المقارنات
    EQ_EQ,               // ==
    NOT_EQ,              // !=
    LESS,                // <
    GREATER,             // >
    LESS_EQ,             // <=
    GREATER_EQ,          // >=
    
    // المنطقية
    AND,                 // &&
    OR,                  // ||
    NOT,                 // !
    
    // خاص
    NEWLINE,
    INDENT,
    DEDENT,
    ERROR
};

/**
 * 📍 Token - رمز واحد
 */
struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    
    Token() : type(TokenType::ERROR), line(0), column(0) {}
    Token(TokenType t, const std::string& v, size_t l, size_t c)
        : type(t), value(v), line(l), column(c) {}
};

// ═══════════════════════════════════════════════════════════════════════════════
//                         هيكل الخطأ | Error Structure
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📋 ParseError - خطأ تحليل
 */
struct ParseError {
    std::string message;
    std::string messageEn;
    size_t line;
    size_t column;
    std::string context;      // السطر المحتوي على الخطأ
    Token foundToken;         // الرمز الذي وجدناه
    std::vector<TokenType> expectedTokens;  // ما كنا نتوقعه
    bool recovered;           // هل تم الاسترداد؟
    
    ParseError()
        : line(0), column(0), recovered(false) {}
    
    /**
     * 📝 تنسيق الخطأ للعرض
     */
    std::string format() const {
        std::ostringstream oss;
        
        oss << "\n";
        oss << "❌ خطأ نحوي | Syntax Error\n";
        oss << "   ─────────────────────────────────────────\n";
        oss << "   السطر " << line << ", العمود " << column << "\n";
        oss << "   " << message << "\n";
        
        if (!context.empty()) {
            oss << "\n";
            oss << "   " << context << "\n";
            oss << "   ";
            for (size_t i = 1; i < column; i++) oss << " ";
            oss << "^\n";
        }
        
        if (recovered) {
            oss << "\n   💡 تم الاستمرار لإيجاد أخطاء أخرى\n";
        }
        
        return oss.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    استراتيجيات الاسترداد | Recovery Strategies
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📊 RecoveryStrategy - استراتيجية الاسترداد
 *
 * طرق مختلفة للاسترداد من الأخطاء:
 */
enum class RecoveryStrategy {
    /**
     * 🔄 PANIC_MODE - وضع الذعر
     * 
     * يتخطى الرموز حتى يجد "نقطة آمنة"
     * مثل: نهاية السطر، فاصلة منقوطة، قوس إغلاق
     * 
     * الأفضل لـ: الأخطاء داخل التعبيرات
     */
    PANIC_MODE,
    
    /**
     * 🔧 TOKEN_INSERTION - إدراج رمز
     * 
     * يفترض أن المبرمج نسي رمزاً ويدرجه
     * مثل: نسي ) فيدرجها
     * 
     * الأفضل لـ: الأقواس المفقودة
     */
    TOKEN_INSERTION,
    
    /**
     * 🗑️ TOKEN_DELETION - حذف رمز
     * 
     * يفترض أن هناك رمزاً زائداً ويحذفه
     * مثل: ;; بدلاً من ;
     * 
     * الأفضل لـ: الرموز المكررة
     */
    TOKEN_DELETION,
    
    /**
     * 🔀 TOKEN_REPLACEMENT - استبدال رمز
     * 
     * يفترض أن المبرمج كتب رمزاً خاطئاً ويستبدله
     * مثل: كتب { بدلاً من (
     * 
     * الأفضل لـ: الرموز المتشابهة
     */
    TOKEN_REPLACEMENT,
    
    /**
     * ⏭️ STATEMENT_SKIP - تخطي الجملة
     * 
     * يتخطى الجملة كاملة وينتقل للتالية
     * 
     * الأفضل لـ: الأخطاء الكبيرة
     */
    STATEMENT_SKIP,
    
    /**
     * 📐 INDENTATION_SYNC - مزامنة المسافات
     * 
     * يستخدم المسافات البادئة للعودة لمستوى آمن
     * 
     * الأفضل لـ: الأخطاء في الكتل
     */
    INDENTATION_SYNC
};

// ═══════════════════════════════════════════════════════════════════════════════
//                      نظام الاسترداد | Recovery System
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 ErrorRecoverySystem - نظام استرداد الأخطاء
 *
 * المسؤول عن:
 * 1. اكتشاف نقاط الاسترداد
 * 2. اختيار الاستراتيجية المناسبة
 * 3. تنفيذ الاسترداد
 * 4. تسجيل الأخطاء
 */
class ErrorRecoverySystem {
public:
    ErrorRecoverySystem()
        : maxErrors_(20)
        , errorsFound_(0)
        , inRecovery_(false)
    {
        initializeSyncTokens();
    }
    
    /**
     * 🚨 تسجيل خطأ والمحاولة للاسترداد
     */
    bool reportErrorAndRecover(
        const std::string& message,
        const std::string& messageEn,
        const Token& current,
        const std::vector<TokenType>& expected,
        const std::string& sourceLine,
        std::function<Token()> advance,
        std::function<Token()> peek
    ) {
        // تسجيل الخطأ
        ParseError error;
        error.message = message;
        error.messageEn = messageEn;
        error.line = current.line;
        error.column = current.column;
        error.context = sourceLine;
        error.foundToken = current;
        error.expectedTokens = expected;
        
        errors_.push_back(error);
        errorsFound_++;
        
        // فحص الحد الأقصى
        if (errorsFound_ >= maxErrors_) {
            return false;  // توقف عن المحاولة
        }
        
        // اختيار استراتيجية الاسترداد
        RecoveryStrategy strategy = selectStrategy(current, expected);
        
        // تنفيذ الاسترداد
        bool recovered = executeRecovery(strategy, current, expected, advance, peek);
        
        errors_.back().recovered = recovered;
        
        return recovered;
    }
    
    /**
     * 🔍 البحث عن نقطة مزامنة
     */
    bool synchronize(
        std::function<Token()> advance,
        std::function<Token()> peek
    ) {
        inRecovery_ = true;
        
        while (true) {
            Token current = peek();
            
            // نهاية الملف
            if (current.type == TokenType::END_OF_FILE) {
                inRecovery_ = false;
                return false;
            }
            
            // وصلنا لنقطة مزامنة
            if (isSyncToken(current.type)) {
                inRecovery_ = false;
                return true;
            }
            
            // تقدم للرمز التالي
            advance();
        }
    }
    
    /**
     * 📋 الحصول على كل الأخطاء
     */
    const std::vector<ParseError>& getErrors() const {
        return errors_;
    }
    
    /**
     * 📊 هل هناك أخطاء؟
     */
    bool hasErrors() const {
        return !errors_.empty();
    }
    
    /**
     * 🔢 عدد الأخطاء
     */
    size_t errorCount() const {
        return errors_.size();
    }
    
    /**
     * 🧹 مسح الأخطاء
     */
    void clear() {
        errors_.clear();
        errorsFound_ = 0;
        inRecovery_ = false;
    }
    
    /**
     * 📝 تنسيق كل الأخطاء
     */
    std::string formatAllErrors() const {
        std::ostringstream oss;
        
        oss << "\n";
        oss << "╔══════════════════════════════════════════════════════════════╗\n";
        oss << "║              أخطاء التحليل النحوي | Parse Errors              ║\n";
        oss << "╠══════════════════════════════════════════════════════════════╣\n";
        oss << "║   عدد الأخطاء: " << errors_.size() << "\n";
        oss << "╚══════════════════════════════════════════════════════════════╝\n";
        
        for (size_t i = 0; i < errors_.size(); i++) {
            oss << "\n[" << (i + 1) << "] " << errors_[i].format();
        }
        
        return oss.str();
    }
    
private:
    std::vector<ParseError> errors_;
    std::unordered_set<TokenType> syncTokens_;
    size_t maxErrors_;
    size_t errorsFound_;
    bool inRecovery_;
    
    /**
     * 🔧 تهيئة رموز المزامنة
     */
    void initializeSyncTokens() {
        // رموز بداية الجمل
        syncTokens_.insert(TokenType::KEYWORD_FUNCTION);
        syncTokens_.insert(TokenType::KEYWORD_IF);
        syncTokens_.insert(TokenType::KEYWORD_WHILE);
        syncTokens_.insert(TokenType::KEYWORD_FOR);
        syncTokens_.insert(TokenType::KEYWORD_RETURN);
        syncTokens_.insert(TokenType::KEYWORD_CLASS);
        syncTokens_.insert(TokenType::KEYWORD_IMPORT);
        syncTokens_.insert(TokenType::KEYWORD_VAR);
        syncTokens_.insert(TokenType::KEYWORD_CONST);
        
        // رموز نهاية الجمل
        syncTokens_.insert(TokenType::NEWLINE);
        syncTokens_.insert(TokenType::SEMICOLON);
        syncTokens_.insert(TokenType::RBRACE);
        syncTokens_.insert(TokenType::DEDENT);
    }
    
    /**
     * 🔍 هل هذا رمز مزامنة؟
     */
    bool isSyncToken(TokenType type) const {
        return syncTokens_.find(type) != syncTokens_.end();
    }
    
    /**
     * 🎯 اختيار استراتيجية الاسترداد
     */
    RecoveryStrategy selectStrategy(
        const Token& current,
        const std::vector<TokenType>& expected
    ) {
        // إذا كنا نتوقع قوس إغلاق، جرب الإدراج
        for (TokenType exp : expected) {
            if (exp == TokenType::RPAREN || 
                exp == TokenType::RBRACE || 
                exp == TokenType::RBRACKET) {
                return RecoveryStrategy::TOKEN_INSERTION;
            }
        }
        
        // إذا كان الرمز الحالي رمز بداية جملة، استخدم تخطي الجملة
        if (isSyncToken(current.type)) {
            return RecoveryStrategy::STATEMENT_SKIP;
        }
        
        // إذا كان الرمز فاصلة منقوطة زائدة، احذفه
        if (current.type == TokenType::SEMICOLON) {
            return RecoveryStrategy::TOKEN_DELETION;
        }
        
        // الافتراضي: وضع الذعر
        return RecoveryStrategy::PANIC_MODE;
    }
    
    /**
     * ⚙️ تنفيذ الاسترداد
     */
    bool executeRecovery(
        RecoveryStrategy strategy,
        const Token& current,
        const std::vector<TokenType>& expected,
        std::function<Token()> advance,
        std::function<Token()> peek
    ) {
        switch (strategy) {
            case RecoveryStrategy::PANIC_MODE:
                return synchronize(advance, peek);
                
            case RecoveryStrategy::TOKEN_INSERTION:
                // نفترض وجود الرمز المتوقع ونكمل
                return true;
                
            case RecoveryStrategy::TOKEN_DELETION:
                // نتخطى الرمز الحالي
                advance();
                return true;
                
            case RecoveryStrategy::TOKEN_REPLACEMENT:
                // نستبدل ونكمل
                advance();
                return true;
                
            case RecoveryStrategy::STATEMENT_SKIP:
                // نتخطى حتى نهاية السطر
                while (peek().type != TokenType::NEWLINE &&
                       peek().type != TokenType::END_OF_FILE) {
                    advance();
                }
                if (peek().type == TokenType::NEWLINE) {
                    advance();
                }
                return true;
                
            case RecoveryStrategy::INDENTATION_SYNC:
                return synchronize(advance, peek);
        }
        
        return false;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                 مساعد توقع الرموز | Token Expectation Helper
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔮 TokenExpectation - توقع الرموز المحتملة
 */
class TokenExpectation {
public:
    /**
     * 🔍 توقع الرموز التالية المحتملة بناءً على السياق
     */
    static std::vector<TokenType> predictNext(TokenType current) {
        std::vector<TokenType> predictions;
        
        switch (current) {
            case TokenType::KEYWORD_FUNCTION:
                predictions.push_back(TokenType::IDENTIFIER);
                break;
                
            case TokenType::IDENTIFIER:
                predictions.push_back(TokenType::LPAREN);
                predictions.push_back(TokenType::EQUALS);
                predictions.push_back(TokenType::DOT);
                predictions.push_back(TokenType::COLON);
                break;
                
            case TokenType::LPAREN:
                predictions.push_back(TokenType::IDENTIFIER);
                predictions.push_back(TokenType::NUMBER_LITERAL);
                predictions.push_back(TokenType::STRING_LITERAL);
                predictions.push_back(TokenType::RPAREN);
                break;
                
            case TokenType::KEYWORD_IF:
            case TokenType::KEYWORD_WHILE:
                predictions.push_back(TokenType::IDENTIFIER);
                predictions.push_back(TokenType::NUMBER_LITERAL);
                predictions.push_back(TokenType::NOT);
                predictions.push_back(TokenType::LPAREN);
                break;
                
            case TokenType::COLON:
                predictions.push_back(TokenType::NEWLINE);
                predictions.push_back(TokenType::IDENTIFIER);
                break;
                
            default:
                break;
        }
        
        return predictions;
    }
    
    /**
     * 📝 تحويل نوع الرمز إلى وصف عربي
     */
    static std::string tokenTypeToArabic(TokenType type) {
        static std::unordered_map<TokenType, std::string> names = {
            {TokenType::END_OF_FILE, "نهاية الملف"},
            {TokenType::KEYWORD_FUNCTION, "كلمة 'دالة'"},
            {TokenType::KEYWORD_IF, "كلمة 'إذا'"},
            {TokenType::KEYWORD_ELSE, "كلمة 'وإلا'"},
            {TokenType::KEYWORD_WHILE, "كلمة 'طالما'"},
            {TokenType::KEYWORD_FOR, "كلمة 'لكل'"},
            {TokenType::KEYWORD_RETURN, "كلمة 'أرجع'"},
            {TokenType::KEYWORD_CLASS, "كلمة 'صنف'"},
            {TokenType::KEYWORD_VAR, "كلمة 'متغير'"},
            {TokenType::IDENTIFIER, "معرّف (اسم)"},
            {TokenType::NUMBER_LITERAL, "رقم"},
            {TokenType::STRING_LITERAL, "نص"},
            {TokenType::LPAREN, "قوس فتح ("},
            {TokenType::RPAREN, "قوس إغلاق )"},
            {TokenType::LBRACE, "قوس معقوف {"},
            {TokenType::RBRACE, "قوس معقوف }"},
            {TokenType::LBRACKET, "قوس مربع ["},
            {TokenType::RBRACKET, "قوس مربع ]"},
            {TokenType::COMMA, "فاصلة ,"},
            {TokenType::COLON, "نقطتان :"},
            {TokenType::SEMICOLON, "فاصلة منقوطة ;"},
            {TokenType::DOT, "نقطة ."},
            {TokenType::EQUALS, "علامة يساوي ="},
            {TokenType::NEWLINE, "سطر جديد"}
        };
        
        auto it = names.find(type);
        return it != names.end() ? it->second : "رمز غير معروف";
    }
    
    /**
     * 📝 تكوين رسالة الخطأ
     */
    static std::string formatExpectation(
        const std::vector<TokenType>& expected,
        TokenType found
    ) {
        std::ostringstream oss;
        
        oss << "توقعت ";
        
        if (expected.size() == 1) {
            oss << tokenTypeToArabic(expected[0]);
        } else if (expected.size() == 2) {
            oss << tokenTypeToArabic(expected[0]);
            oss << " أو ";
            oss << tokenTypeToArabic(expected[1]);
        } else {
            for (size_t i = 0; i < expected.size(); i++) {
                if (i > 0) {
                    if (i == expected.size() - 1) {
                        oss << " أو ";
                    } else {
                        oss << "، ";
                    }
                }
                oss << tokenTypeToArabic(expected[i]);
            }
        }
        
        oss << "، لكن وجدت " << tokenTypeToArabic(found);
        
        return oss.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

static std::unique_ptr<ErrorRecoverySystem> globalRecovery;

extern "C" {
    /**
     * 🔧 إنشاء نظام الاسترداد
     */
    void* sad_error_recovery_create() {
        return new ErrorRecoverySystem();
    }
    
    /**
     * 🗑️ حذف نظام الاسترداد
     */
    void sad_error_recovery_destroy(void* system) {
        delete static_cast<ErrorRecoverySystem*>(system);
    }
    
    /**
     * 📊 عدد الأخطاء
     */
    size_t sad_error_recovery_count(void* system) {
        return static_cast<ErrorRecoverySystem*>(system)->errorCount();
    }
    
    /**
     * 🧹 مسح الأخطاء
     */
    void sad_error_recovery_clear(void* system) {
        static_cast<ErrorRecoverySystem*>(system)->clear();
    }
    
    /**
     * 📝 تنسيق الأخطاء
     */
    const char* sad_error_recovery_format(void* system) {
        static std::string result;
        result = static_cast<ErrorRecoverySystem*>(system)->formatAllErrors();
        return result.c_str();
    }
}

} // namespace parser
} // namespace sad
