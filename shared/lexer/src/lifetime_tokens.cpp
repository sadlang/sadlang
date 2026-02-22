/**
 * =============================================================================
 * ملف: lifetime_tokens.cpp
 * الوصف: تحليل رموز تعليقات العمر <'أ>
 * المهمة: T201 - Implement lifetime token lexing
 * المرحلة: Phase 20 - User Story 17 (Lifetime Annotations)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هي تعليقات العمر؟
 * ───────────────────
 * العمر (Lifetime) يصف المدة التي يظل فيها المرجع صالحاً.
 * 
 * في لغة سعد، نستخدم تعليقات العمر مثل Rust:
 * 
 *   دالة أطول<'أ>(س: &'أ نص, ص: &'أ نص) -> &'أ نص
 *   //       ^^^     ^^^        ^^^         ^^^
 *   //       عمر     عمر        عمر         عمر
 * 
 * هذا يقول: "المرجع المُرجَع يعيش بقدر ما يعيش أقصر مدخل"
 * 
 * لماذا نحتاج تعليقات العمر؟
 * ─────────────────────────
 * 1. **الأمان**: تمنع المراجع المتدلية (dangling references)
 * 2. **الوضوح**: توضح العلاقات بين المراجع
 * 3. **التحقق**: تمكّن المترجم من التحقق من صحة المراجع
 * 
 * صيغ تعليقات العمر:
 * ─────────────────
 * - `'أ` - عمر مسمى (مثل 'a في Rust)
 * - `'ثابت` - العمر الثابت (static lifetime)
 * - `'_` - عمر مجهول (anonymous lifetime)
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
#include <functional>

namespace sad::lexer::lifetime {

// =============================================================================
// أنواع رموز العمر
// =============================================================================

/**
 * نوع رمز العمر
 */
enum class LifetimeTokenType {
    // حرف الفتحة والإغلاق
    LeftAngle,          // <
    RightAngle,         // >
    
    // رمز العمر
    Apostrophe,         // '
    LifetimeName,       // أ، ب، حياة، الخ
    
    // أعمار خاصة
    StaticLifetime,     // 'ثابت
    AnonymousLifetime,  // '_
    
    // محددات
    Colon,              // :
    Comma,              // ,
    Plus,               // + (للحدود المتعددة)
    
    // مراجع
    Ampersand,          // &
    MutKeyword,         // متغير (mut)
    
    // عام
    Identifier,         // معرّف عام
    Whitespace,         // مسافة
    EndOfInput,         // نهاية
    Invalid,            // رمز غير صالح
};

/**
 * معلومات الموقع في الكود
 */
struct SourcePosition {
    size_t line;
    size_t column;
    size_t offset;
    
    std::string toString() const {
        std::ostringstream ss;
        ss << line << ":" << column;
        return ss.str();
    }
};

/**
 * رمز العمر
 */
struct LifetimeToken {
    LifetimeTokenType type;
    std::string value;
    SourcePosition start;
    SourcePosition end;
    
    bool isLifetime() const {
        return type == LifetimeTokenType::LifetimeName ||
               type == LifetimeTokenType::StaticLifetime ||
               type == LifetimeTokenType::AnonymousLifetime;
    }
    
    std::string typeName() const {
        switch (type) {
            case LifetimeTokenType::LeftAngle: return "LeftAngle";
            case LifetimeTokenType::RightAngle: return "RightAngle";
            case LifetimeTokenType::Apostrophe: return "Apostrophe";
            case LifetimeTokenType::LifetimeName: return "LifetimeName";
            case LifetimeTokenType::StaticLifetime: return "StaticLifetime";
            case LifetimeTokenType::AnonymousLifetime: return "AnonymousLifetime";
            case LifetimeTokenType::Colon: return "Colon";
            case LifetimeTokenType::Comma: return "Comma";
            case LifetimeTokenType::Plus: return "Plus";
            case LifetimeTokenType::Ampersand: return "Ampersand";
            case LifetimeTokenType::MutKeyword: return "MutKeyword";
            case LifetimeTokenType::Identifier: return "Identifier";
            case LifetimeTokenType::Whitespace: return "Whitespace";
            case LifetimeTokenType::EndOfInput: return "EndOfInput";
            case LifetimeTokenType::Invalid: return "Invalid";
        }
        return "Unknown";
    }
};

// =============================================================================
// محلل رموز العمر
// =============================================================================

/**
 * محلل رموز تعليقات العمر
 * 
 * مثال الاستخدام:
 * ```
 * LifetimeLexer lexer;
 * auto tokens = lexer.tokenize("<'أ>");
 * // tokens: [LeftAngle, Apostrophe, LifetimeName("أ"), RightAngle]
 * ```
 */
class LifetimeLexer {
public:
    /**
     * تحليل نص إلى رموز عمر
     */
    std::vector<LifetimeToken> tokenize(const std::string& source) {
        source_ = source;
        pos_ = 0;
        line_ = 1;
        column_ = 1;
        tokens_.clear();
        
        while (!isAtEnd()) {
            scanToken();
        }
        
        tokens_.push_back({
            LifetimeTokenType::EndOfInput,
            "",
            currentPosition(),
            currentPosition()
        });
        
        return tokens_;
    }
    
private:
    std::string source_;
    size_t pos_ = 0;
    size_t line_ = 1;
    size_t column_ = 1;
    std::vector<LifetimeToken> tokens_;
    
    bool isAtEnd() const {
        return pos_ >= source_.size();
    }
    
    char peek() const {
        if (isAtEnd()) return '\0';
        return source_[pos_];
    }
    
    char peekNext() const {
        if (pos_ + 1 >= source_.size()) return '\0';
        return source_[pos_ + 1];
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
    
    SourcePosition currentPosition() const {
        return {line_, column_, pos_};
    }
    
    void addToken(LifetimeTokenType type, const std::string& value,
                  SourcePosition start) {
        tokens_.push_back({type, value, start, currentPosition()});
    }
    
    /**
     * هل هذا حرف UTF-8 عربي؟
     */
    bool isArabicChar() const {
        if (isAtEnd()) return false;
        unsigned char c = static_cast<unsigned char>(source_[pos_]);
        // أحرف UTF-8 متعددة البايت
        return c >= 0xD8 && c <= 0xDB;
    }
    
    /**
     * قراءة كلمة عربية
     */
    std::string readArabicWord() {
        std::string word;
        while (!isAtEnd()) {
            if (isArabicChar()) {
                // قراءة تسلسل UTF-8
                unsigned char first = static_cast<unsigned char>(source_[pos_]);
                if ((first & 0xE0) == 0xC0) {
                    // 2 bytes
                    word += source_[pos_++];
                    if (!isAtEnd()) word += source_[pos_++];
                } else if ((first & 0xF0) == 0xE0) {
                    // 3 bytes (معظم الأحرف العربية)
                    word += source_[pos_++];
                    if (!isAtEnd()) word += source_[pos_++];
                    if (!isAtEnd()) word += source_[pos_++];
                } else if ((first & 0xF8) == 0xF0) {
                    // 4 bytes
                    word += source_[pos_++];
                    if (!isAtEnd()) word += source_[pos_++];
                    if (!isAtEnd()) word += source_[pos_++];
                    if (!isAtEnd()) word += source_[pos_++];
                }
            } else if (isalnum(peek()) || peek() == '_') {
                word += advance();
            } else {
                break;
            }
        }
        return word;
    }
    
    /**
     * فحص الرمز التالي
     */
    void scanToken() {
        auto start = currentPosition();
        char c = advance();
        
        switch (c) {
            case '<':
                addToken(LifetimeTokenType::LeftAngle, "<", start);
                break;
                
            case '>':
                addToken(LifetimeTokenType::RightAngle, ">", start);
                break;
                
            case '\'':
                scanLifetime(start);
                break;
                
            case ':':
                addToken(LifetimeTokenType::Colon, ":", start);
                break;
                
            case ',':
                addToken(LifetimeTokenType::Comma, ",", start);
                break;
                
            case '+':
                addToken(LifetimeTokenType::Plus, "+", start);
                break;
                
            case '&':
                addToken(LifetimeTokenType::Ampersand, "&", start);
                break;
                
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                // تخطي المسافات
                break;
                
            default:
                if (isArabicStart(c) || isalpha(c) || c == '_') {
                    pos_--;  // إعادة الحرف
                    scanIdentifier(start);
                } else {
                    addToken(LifetimeTokenType::Invalid, 
                            std::string(1, c), start);
                }
                break;
        }
    }
    
    bool isArabicStart(char c) const {
        unsigned char uc = static_cast<unsigned char>(c);
        return uc >= 0xD8;  // بداية UTF-8 عربي
    }
    
    /**
     * تحليل عمر بعد الفاصلة العليا
     */
    void scanLifetime(SourcePosition start) {
        if (isAtEnd()) {
            addToken(LifetimeTokenType::Invalid, "'", start);
            return;
        }
        
        // فحص العمر المجهول '_
        if (peek() == '_') {
            advance();
            addToken(LifetimeTokenType::AnonymousLifetime, "'_", start);
            return;
        }
        
        // قراءة اسم العمر
        std::string name = "'";
        if (isArabicStart(peek())) {
            name += readArabicWord();
        } else if (isalpha(peek()) || peek() == '_') {
            while (!isAtEnd() && (isalnum(peek()) || peek() == '_')) {
                name += advance();
            }
        } else {
            addToken(LifetimeTokenType::Invalid, "'", start);
            return;
        }
        
        // فحص العمر الثابت
        if (name == "'ثابت" || name == "'static") {
            addToken(LifetimeTokenType::StaticLifetime, name, start);
        } else {
            addToken(LifetimeTokenType::LifetimeName, name, start);
        }
    }
    
    /**
     * تحليل معرّف
     */
    void scanIdentifier(SourcePosition start) {
        std::string ident = readArabicWord();
        
        // كلمات مفتاحية
        if (ident == "متغير" || ident == "mut") {
            addToken(LifetimeTokenType::MutKeyword, ident, start);
        } else {
            addToken(LifetimeTokenType::Identifier, ident, start);
        }
    }
};

// =============================================================================
// هياكل بيانات العمر
// =============================================================================

/**
 * تعريف عمر واحد
 */
struct Lifetime {
    std::string name;        // اسم العمر (بدون ')
    bool isStatic;          // هل هو 'ثابت؟
    bool isAnonymous;       // هل هو '_؟
    SourcePosition position;
    
    static Lifetime named(const std::string& name, SourcePosition pos = {}) {
        return {name, false, false, pos};
    }
    
    static Lifetime staticLifetime(SourcePosition pos = {}) {
        return {"ثابت", true, false, pos};
    }
    
    static Lifetime anonymous(SourcePosition pos = {}) {
        return {"_", false, true, pos};
    }
    
    std::string toString() const {
        if (isStatic) return "'ثابت";
        if (isAnonymous) return "'_";
        return "'" + name;
    }
    
    bool operator==(const Lifetime& other) const {
        return name == other.name;
    }
};

/**
 * قائمة أعمار (مثل <'أ, 'ب>)
 */
struct LifetimeList {
    std::vector<Lifetime> lifetimes;
    
    void add(const Lifetime& lt) {
        lifetimes.push_back(lt);
    }
    
    bool contains(const std::string& name) const {
        for (const auto& lt : lifetimes) {
            if (lt.name == name) return true;
        }
        return false;
    }
    
    size_t size() const { return lifetimes.size(); }
    bool empty() const { return lifetimes.empty(); }
    
    std::string toString() const {
        if (lifetimes.empty()) return "";
        std::ostringstream ss;
        ss << "<";
        for (size_t i = 0; i < lifetimes.size(); i++) {
            if (i > 0) ss << ", ";
            ss << lifetimes[i].toString();
        }
        ss << ">";
        return ss.str();
    }
};

/**
 * مرجع مع عمر
 */
struct LifetimeRef {
    Lifetime lifetime;
    bool isMutable;
    std::string typeName;
    
    std::string toString() const {
        std::ostringstream ss;
        ss << "&" << lifetime.toString() << " ";
        if (isMutable) ss << "متغير ";
        ss << typeName;
        return ss.str();
    }
};

// =============================================================================
// سجل تعريفات الأعمار
// =============================================================================

/**
 * سجل يتتبع تعريفات الأعمار في النطاق الحالي
 */
class LifetimeRegistry {
public:
    /**
     * تسجيل عمر جديد
     */
    void define(const std::string& name, const Lifetime& lifetime) {
        definitions_[name] = lifetime;
    }
    
    /**
     * البحث عن عمر
     */
    std::optional<Lifetime> lookup(const std::string& name) const {
        auto it = definitions_.find(name);
        if (it != definitions_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    /**
     * هل العمر معرّف؟
     */
    bool isDefined(const std::string& name) const {
        return definitions_.find(name) != definitions_.end();
    }
    
    /**
     * الحصول على جميع الأعمار المعرّفة
     */
    std::vector<std::string> allNames() const {
        std::vector<std::string> names;
        for (const auto& [name, _] : definitions_) {
            names.push_back(name);
        }
        return names;
    }
    
    /**
     * مسح السجل
     */
    void clear() {
        definitions_.clear();
    }
    
private:
    std::map<std::string, Lifetime> definitions_;
};

// =============================================================================
// محلل قائمة الأعمار
// =============================================================================

/**
 * محلل قوائم تعليقات العمر
 * 
 * يحلل: <'أ, 'ب, 'ثابت>
 */
class LifetimeListParser {
public:
    /**
     * تحليل قائمة أعمار من الرموز
     */
    LifetimeList parse(const std::vector<LifetimeToken>& tokens) {
        tokens_ = tokens;
        pos_ = 0;
        LifetimeList result;
        
        // توقع <
        if (!match(LifetimeTokenType::LeftAngle)) {
            return result;  // لا توجد أعمار
        }
        
        // قراءة الأعمار
        do {
            if (check(LifetimeTokenType::LifetimeName) ||
                check(LifetimeTokenType::StaticLifetime) ||
                check(LifetimeTokenType::AnonymousLifetime)) {
                
                auto token = advance();
                Lifetime lt;
                
                if (token.type == LifetimeTokenType::StaticLifetime) {
                    lt = Lifetime::staticLifetime(token.start);
                } else if (token.type == LifetimeTokenType::AnonymousLifetime) {
                    lt = Lifetime::anonymous(token.start);
                } else {
                    // إزالة ' من البداية
                    std::string name = token.value;
                    if (!name.empty() && name[0] == '\'') {
                        name = name.substr(1);
                    }
                    lt = Lifetime::named(name, token.start);
                }
                
                result.add(lt);
            }
        } while (match(LifetimeTokenType::Comma));
        
        // توقع >
        match(LifetimeTokenType::RightAngle);
        
        return result;
    }
    
private:
    std::vector<LifetimeToken> tokens_;
    size_t pos_ = 0;
    
    bool isAtEnd() const {
        return pos_ >= tokens_.size() ||
               tokens_[pos_].type == LifetimeTokenType::EndOfInput;
    }
    
    LifetimeToken peek() const {
        if (isAtEnd()) return {LifetimeTokenType::EndOfInput, "", {}, {}};
        return tokens_[pos_];
    }
    
    LifetimeToken advance() {
        if (!isAtEnd()) pos_++;
        return tokens_[pos_ - 1];
    }
    
    bool check(LifetimeTokenType type) const {
        if (isAtEnd()) return false;
        return tokens_[pos_].type == type;
    }
    
    bool match(LifetimeTokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }
};

} // namespace sad::lexer::lifetime

// =============================================================================
// واجهة C للتكامل
// =============================================================================

extern "C" {

using namespace sad::lexer::lifetime;

/**
 * تحليل نص إلى رموز عمر
 */
int sad_tokenize_lifetimes(const char* source, void** out_tokens, size_t* out_count) {
    if (!source || !out_tokens || !out_count) return -1;
    
    try {
        LifetimeLexer lexer;
        auto tokens = lexer.tokenize(source);
        
        auto* result = new std::vector<LifetimeToken>(std::move(tokens));
        *out_tokens = result;
        *out_count = result->size();
        
        return 0;
    } catch (...) {
        return -1;
    }
}

/**
 * الحصول على رمز من القائمة
 */
int sad_lifetime_token_at(void* tokens, size_t index, 
                          int* out_type, const char** out_value) {
    if (!tokens || !out_type || !out_value) return -1;
    
    auto* vec = static_cast<std::vector<LifetimeToken>*>(tokens);
    if (index >= vec->size()) return -1;
    
    const auto& token = (*vec)[index];
    *out_type = static_cast<int>(token.type);
    *out_value = token.value.c_str();
    
    return 0;
}

/**
 * تحرير قائمة الرموز
 */
void sad_free_lifetime_tokens(void* tokens) {
    delete static_cast<std::vector<LifetimeToken>*>(tokens);
}

/**
 * هل الرمز عمر؟
 */
int sad_is_lifetime_token(int type) {
    auto t = static_cast<LifetimeTokenType>(type);
    return (t == LifetimeTokenType::LifetimeName ||
            t == LifetimeTokenType::StaticLifetime ||
            t == LifetimeTokenType::AnonymousLifetime) ? 1 : 0;
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef LIFETIME_TOKENS_TEST

#include <cassert>

void testBasicLexing() {
    sad::lexer::lifetime::LifetimeLexer lexer;
    
    // اختبار عمر بسيط
    auto tokens = lexer.tokenize("<'أ>");
    
    assert(tokens.size() == 4);  // < 'أ > EOF
    assert(tokens[0].type == sad::lexer::lifetime::LifetimeTokenType::LeftAngle);
    assert(tokens[1].type == sad::lexer::lifetime::LifetimeTokenType::LifetimeName);
    assert(tokens[2].type == sad::lexer::lifetime::LifetimeTokenType::RightAngle);
    
    std::cout << "✅ اختبار التحليل الأساسي نجح!\n";
}

void testStaticLifetime() {
    sad::lexer::lifetime::LifetimeLexer lexer;
    
    auto tokens = lexer.tokenize("'ثابت");
    
    bool found = false;
    for (const auto& t : tokens) {
        if (t.type == sad::lexer::lifetime::LifetimeTokenType::StaticLifetime) {
            found = true;
            break;
        }
    }
    assert(found);
    
    std::cout << "✅ اختبار العمر الثابت نجح!\n";
}

void testAnonymousLifetime() {
    sad::lexer::lifetime::LifetimeLexer lexer;
    
    auto tokens = lexer.tokenize("'_");
    
    bool found = false;
    for (const auto& t : tokens) {
        if (t.type == sad::lexer::lifetime::LifetimeTokenType::AnonymousLifetime) {
            found = true;
            break;
        }
    }
    assert(found);
    
    std::cout << "✅ اختبار العمر المجهول نجح!\n";
}

void testLifetimeList() {
    sad::lexer::lifetime::LifetimeLexer lexer;
    sad::lexer::lifetime::LifetimeListParser parser;
    
    auto tokens = lexer.tokenize("<'أ, 'ب, 'ثابت>");
    auto list = parser.parse(tokens);
    
    assert(list.size() == 3);
    assert(list.lifetimes[0].name == "أ");
    assert(list.lifetimes[1].name == "ب");
    assert(list.lifetimes[2].isStatic);
    
    std::cout << "✅ اختبار قائمة الأعمار نجح!\n";
}

void testReference() {
    sad::lexer::lifetime::LifetimeLexer lexer;
    
    auto tokens = lexer.tokenize("&'أ متغير");
    
    bool hasAmpersand = false;
    bool hasLifetime = false;
    bool hasMut = false;
    
    for (const auto& t : tokens) {
        if (t.type == sad::lexer::lifetime::LifetimeTokenType::Ampersand) hasAmpersand = true;
        if (t.type == sad::lexer::lifetime::LifetimeTokenType::LifetimeName) hasLifetime = true;
        if (t.type == sad::lexer::lifetime::LifetimeTokenType::MutKeyword) hasMut = true;
    }
    
    assert(hasAmpersand);
    assert(hasLifetime);
    assert(hasMut);
    
    std::cout << "✅ اختبار المرجع مع العمر نجح!\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات محلل رموز العمر\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testBasicLexing();
    testStaticLifetime();
    testAnonymousLifetime();
    testLifetimeList();
    testReference();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // LIFETIME_TOKENS_TEST
