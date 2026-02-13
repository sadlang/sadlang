/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة ص - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: arabic_toml_parser.cpp                                               ║
 * ║   الوصف: محلل TOML العربي لملفات ص.toml                                     ║
 * ║   المهمة: T169 - Phase 16 (US14)                                            ║
 * ║   المؤلف: فريق ص | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                         دليل المبتدئين | Beginner's Guide
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 ما هو TOML؟
 * ──────────────────────────────────────
 *
 * TOML هو صيغة لملفات الإعدادات، مثل:
 *
 *   [الحزمة]
 *   الاسم = "مشروعي"
 *   النسخة = "1.0.0"
 *   
 *   [المتطلبات]
 *   رسومات = "0.5"
 *   شبكة = "1.2"
 *
 * 🔹 لماذا TOML عربي؟
 *    - سهولة القراءة للمبرمجين العرب
 *    - لا حاجة لمعرفة الإنجليزية
 *    - متوافق مع معيار TOML الدولي
 *
 * 📦 مثال ص.toml كامل:
 *
 *   [الحزمة]
 *   الاسم = "تطبيقي"
 *   النسخة = "2.0.0"
 *   الوصف = "تطبيق رائع بلغة ص"
 *   المؤلف = "أحمد محمد"
 *   الرخصة = "MIT"
 *
 *   [المتطلبات]
 *   واجهة = "^1.0"
 *   بيانات = "~2.0"
 *
 *   [التطوير]
 *   اختبار = "0.3"
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <memory>
#include <sstream>
#include <optional>
#include <stdexcept>

namespace sad {
namespace pkg {

// ═══════════════════════════════════════════════════════════════════════════════
//                          أنواع قيم TOML | TOML Value Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📦 TomlValue - قيمة TOML
 * 
 * يمكن أن تكون: نص، عدد، منطقي، قائمة، أو جدول
 */
class TomlValue;
using TomlArray = std::vector<std::shared_ptr<TomlValue>>;
using TomlTable = std::unordered_map<std::string, std::shared_ptr<TomlValue>>;

class TomlValue {
public:
    enum class Type {
        STRING,     // نص
        INTEGER,    // عدد صحيح
        FLOAT,      // عدد عشري
        BOOLEAN,    // منطقي
        ARRAY,      // قائمة
        TABLE       // جدول
    };
    
    using Value = std::variant<
        std::string,
        int64_t,
        double,
        bool,
        TomlArray,
        TomlTable
    >;
    
    TomlValue() : type_(Type::TABLE), value_(TomlTable{}) {}
    explicit TomlValue(const std::string& s) : type_(Type::STRING), value_(s) {}
    explicit TomlValue(int64_t i) : type_(Type::INTEGER), value_(i) {}
    explicit TomlValue(double d) : type_(Type::FLOAT), value_(d) {}
    explicit TomlValue(bool b) : type_(Type::BOOLEAN), value_(b) {}
    explicit TomlValue(const TomlArray& arr) : type_(Type::ARRAY), value_(arr) {}
    explicit TomlValue(const TomlTable& tbl) : type_(Type::TABLE), value_(tbl) {}
    
    Type type() const { return type_; }
    
    // Getters
    const std::string& asString() const { return std::get<std::string>(value_); }
    int64_t asInteger() const { return std::get<int64_t>(value_); }
    double asFloat() const { return std::get<double>(value_); }
    bool asBoolean() const { return std::get<bool>(value_); }
    const TomlArray& asArray() const { return std::get<TomlArray>(value_); }
    TomlTable& asTable() { return std::get<TomlTable>(value_); }
    const TomlTable& asTable() const { return std::get<TomlTable>(value_); }
    
    // Type checks
    bool isString() const { return type_ == Type::STRING; }
    bool isInteger() const { return type_ == Type::INTEGER; }
    bool isFloat() const { return type_ == Type::FLOAT; }
    bool isBoolean() const { return type_ == Type::BOOLEAN; }
    bool isArray() const { return type_ == Type::ARRAY; }
    bool isTable() const { return type_ == Type::TABLE; }
    
private:
    Type type_;
    Value value_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                          أخطاء التحليل | Parse Errors
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ❌ TomlParseError - خطأ في تحليل TOML
 */
class TomlParseError : public std::runtime_error {
public:
    TomlParseError(int line, int column, const std::string& messageAr, const std::string& messageEn)
        : std::runtime_error(formatMessage(line, column, messageAr, messageEn))
        , line_(line)
        , column_(column)
        , messageAr_(messageAr)
        , messageEn_(messageEn)
    {}
    
    int line() const { return line_; }
    int column() const { return column_; }
    const std::string& messageArabic() const { return messageAr_; }
    const std::string& messageEnglish() const { return messageEn_; }
    
private:
    int line_;
    int column_;
    std::string messageAr_;
    std::string messageEn_;
    
    static std::string formatMessage(int line, int column, const std::string& ar, const std::string& en) {
        std::ostringstream oss;
        oss << "سطر " << line << ", عمود " << column << ": " << ar;
        oss << " | Line " << line << ", Column " << column << ": " << en;
        return oss.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                          محلل TOML العربي | Arabic TOML Parser
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📖 ArabicTomlParser - محلل TOML بدعم عربي كامل
 */
class ArabicTomlParser {
public:
    ArabicTomlParser() = default;
    
    /**
     * 📄 تحليل نص TOML
     */
    std::shared_ptr<TomlValue> parse(const std::string& input) {
        input_ = input;
        pos_ = 0;
        line_ = 1;
        column_ = 1;
        
        auto root = std::make_shared<TomlValue>(TomlTable{});
        currentTable_ = root.get();
        
        while (!isEof()) {
            skipWhitespaceAndComments();
            
            if (isEof()) break;
            
            if (peek() == '[') {
                parseTableHeader(root.get());
            }
            else if (isKeyChar(peek())) {
                parseKeyValue();
            }
            else {
                throw TomlParseError(line_, column_,
                    "حرف غير متوقع",
                    "Unexpected character");
            }
        }
        
        return root;
    }
    
    /**
     * 🔄 تحويل المفاتيح العربية للإنجليزية
     */
    static std::string arabicToEnglishKey(const std::string& key) {
        static const std::unordered_map<std::string, std::string> keyMap = {
            // حقول الحزمة
            {"الاسم", "name"},
            {"النسخة", "version"},
            {"الوصف", "description"},
            {"المؤلف", "author"},
            {"المؤلفون", "authors"},
            {"الرخصة", "license"},
            {"المستودع", "repository"},
            {"الكلمات_المفتاحية", "keywords"},
            {"الفئات", "categories"},
            {"الصفحة", "homepage"},
            {"التوثيق", "documentation"},
            {"الحد_الأدنى_ص", "sad_minimum"},
            
            // أقسام الملف
            {"الحزمة", "package"},
            {"المتطلبات", "dependencies"},
            {"التطوير", "dev-dependencies"},
            {"البناء", "build"},
            {"الميزات", "features"},
            {"الملفات_الشخصية", "profile"},
            {"الفحص", "test"},
            {"الأهداف", "targets"},
            {"الإعدادات", "settings"}
        };
        
        auto it = keyMap.find(key);
        return it != keyMap.end() ? it->second : key;
    }
    
    /**
     * 🔄 تحويل المفاتيح الإنجليزية للعربية
     */
    static std::string englishToArabicKey(const std::string& key) {
        static const std::unordered_map<std::string, std::string> keyMap = {
            // Package fields
            {"name", "الاسم"},
            {"version", "النسخة"},
            {"description", "الوصف"},
            {"author", "المؤلف"},
            {"authors", "المؤلفون"},
            {"license", "الرخصة"},
            {"repository", "المستودع"},
            {"keywords", "الكلمات_المفتاحية"},
            {"categories", "الفئات"},
            {"homepage", "الصفحة"},
            {"documentation", "التوثيق"},
            {"sad_minimum", "الحد_الأدنى_ص"},
            
            // File sections
            {"package", "الحزمة"},
            {"dependencies", "المتطلبات"},
            {"dev-dependencies", "التطوير"},
            {"build", "البناء"},
            {"features", "الميزات"},
            {"profile", "الملفات_الشخصية"},
            {"test", "الفحص"},
            {"targets", "الأهداف"},
            {"settings", "الإعدادات"}
        };
        
        auto it = keyMap.find(key);
        return it != keyMap.end() ? it->second : key;
    }
    
private:
    std::string input_;
    size_t pos_ = 0;
    int line_ = 1;
    int column_ = 1;
    TomlValue* currentTable_ = nullptr;
    
    // ═══════════════════════════════════════════════════════════════
    //                     أدوات التحليل | Parsing Utilities
    // ═══════════════════════════════════════════════════════════════
    
    bool isEof() const {
        return pos_ >= input_.size();
    }
    
    char peek() const {
        return isEof() ? '\0' : input_[pos_];
    }
    
    char peekNext() const {
        return pos_ + 1 >= input_.size() ? '\0' : input_[pos_ + 1];
    }
    
    char advance() {
        char c = peek();
        pos_++;
        if (c == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        return c;
    }
    
    bool match(char expected) {
        if (peek() == expected) {
            advance();
            return true;
        }
        return false;
    }
    
    void expect(char expected, const std::string& contextAr, const std::string& contextEn) {
        if (!match(expected)) {
            throw TomlParseError(line_, column_,
                "متوقع '" + std::string(1, expected) + "' " + contextAr,
                "Expected '" + std::string(1, expected) + "' " + contextEn);
        }
    }
    
    bool isWhitespace(char c) const {
        return c == ' ' || c == '\t';
    }
    
    bool isNewline(char c) const {
        return c == '\n' || c == '\r';
    }
    
    bool isKeyChar(char c) const {
        // ASCII alphanumeric, underscore, hyphen
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '_' || c == '-') {
            return true;
        }
        // Arabic characters (UTF-8 encoded)
        // Arabic starts at U+0600 (0xD8 0x80 in UTF-8)
        if ((unsigned char)c >= 0xD8) {
            return true;
        }
        return false;
    }
    
    void skipWhitespace() {
        while (!isEof() && isWhitespace(peek())) {
            advance();
        }
    }
    
    void skipWhitespaceAndNewlines() {
        while (!isEof() && (isWhitespace(peek()) || isNewline(peek()))) {
            advance();
        }
    }
    
    void skipWhitespaceAndComments() {
        while (!isEof()) {
            skipWhitespace();
            
            if (peek() == '#') {
                // تعليق - تجاهل حتى نهاية السطر
                while (!isEof() && !isNewline(peek())) {
                    advance();
                }
            }
            
            if (isNewline(peek())) {
                advance();
                continue;
            }
            
            break;
        }
    }
    
    void skipToEndOfLine() {
        while (!isEof() && !isNewline(peek())) {
            advance();
        }
        if (!isEof()) advance(); // consume newline
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                     تحليل العناصر | Element Parsing
    // ═══════════════════════════════════════════════════════════════
    
    std::string parseKey() {
        std::string key;
        
        if (peek() == '"') {
            // مفتاح منصوص
            return parseString();
        }
        
        // مفتاح عادي
        while (!isEof() && isKeyChar(peek())) {
            key += advance();
        }
        
        if (key.empty()) {
            throw TomlParseError(line_, column_,
                "مفتاح فارغ",
                "Empty key");
        }
        
        return key;
    }
    
    std::string parseString() {
        expect('"', "في بداية النص", "at start of string");
        
        std::string result;
        
        while (!isEof() && peek() != '"') {
            if (peek() == '\\') {
                advance();
                if (isEof()) {
                    throw TomlParseError(line_, column_,
                        "تسلسل هروب غير مكتمل",
                        "Incomplete escape sequence");
                }
                char escaped = advance();
                switch (escaped) {
                    case 'n': result += '\n'; break;
                    case 't': result += '\t'; break;
                    case 'r': result += '\r'; break;
                    case '\\': result += '\\'; break;
                    case '"': result += '"'; break;
                    default:
                        throw TomlParseError(line_, column_,
                            "تسلسل هروب غير صالح",
                            "Invalid escape sequence");
                }
            }
            else if (isNewline(peek())) {
                throw TomlParseError(line_, column_,
                    "سطر جديد في نص غير متعدد الأسطر",
                    "Newline in single-line string");
            }
            else {
                result += advance();
            }
        }
        
        expect('"', "في نهاية النص", "at end of string");
        return result;
    }
    
    std::shared_ptr<TomlValue> parseValue() {
        skipWhitespace();
        
        char c = peek();
        
        // نص
        if (c == '"') {
            return std::make_shared<TomlValue>(parseString());
        }
        
        // قائمة
        if (c == '[') {
            return parseArray();
        }
        
        // جدول مضمن
        if (c == '{') {
            return parseInlineTable();
        }
        
        // منطقي أو رقم
        if (c == 't' || c == 'f' || (c >= '0' && c <= '9') || c == '-' || c == '+') {
            return parseNumberOrBoolean();
        }
        
        // كلمات عربية للقيم المنطقية
        if ((unsigned char)c >= 0xD8) {
            return parseArabicBoolean();
        }
        
        throw TomlParseError(line_, column_,
            "قيمة غير صالحة",
            "Invalid value");
    }
    
    std::shared_ptr<TomlValue> parseNumberOrBoolean() {
        std::string token;
        
        while (!isEof() && !isWhitespace(peek()) && !isNewline(peek()) &&
               peek() != ',' && peek() != ']' && peek() != '}' && peek() != '#') {
            token += advance();
        }
        
        // Boolean
        if (token == "true") return std::make_shared<TomlValue>(true);
        if (token == "false") return std::make_shared<TomlValue>(false);
        
        // Integer or Float
        bool isFloat = token.find('.') != std::string::npos;
        
        try {
            if (isFloat) {
                return std::make_shared<TomlValue>(std::stod(token));
            } else {
                return std::make_shared<TomlValue>(static_cast<int64_t>(std::stoll(token)));
            }
        }
        catch (...) {
            throw TomlParseError(line_, column_,
                "رقم غير صالح: " + token,
                "Invalid number: " + token);
        }
    }
    
    std::shared_ptr<TomlValue> parseArabicBoolean() {
        std::string token;
        
        while (!isEof() && !isWhitespace(peek()) && !isNewline(peek()) &&
               peek() != ',' && peek() != ']' && peek() != '}' && peek() != '#') {
            token += advance();
        }
        
        if (token == "صحيح" || token == "نعم") {
            return std::make_shared<TomlValue>(true);
        }
        if (token == "خطأ" || token == "لا") {
            return std::make_shared<TomlValue>(false);
        }
        
        throw TomlParseError(line_, column_,
            "قيمة منطقية غير صالحة: " + token,
            "Invalid boolean value: " + token);
    }
    
    std::shared_ptr<TomlValue> parseArray() {
        expect('[', "في بداية القائمة", "at start of array");
        
        TomlArray arr;
        
        skipWhitespaceAndComments();
        
        while (!isEof() && peek() != ']') {
            arr.push_back(parseValue());
            
            skipWhitespaceAndComments();
            
            if (peek() == ',') {
                advance();
                skipWhitespaceAndComments();
            }
        }
        
        expect(']', "في نهاية القائمة", "at end of array");
        
        return std::make_shared<TomlValue>(arr);
    }
    
    std::shared_ptr<TomlValue> parseInlineTable() {
        expect('{', "في بداية الجدول", "at start of inline table");
        
        TomlTable table;
        
        skipWhitespace();
        
        while (!isEof() && peek() != '}') {
            std::string key = parseKey();
            
            skipWhitespace();
            expect('=', "بعد المفتاح", "after key");
            
            auto value = parseValue();
            table[key] = value;
            
            skipWhitespace();
            
            if (peek() == ',') {
                advance();
                skipWhitespace();
            }
        }
        
        expect('}', "في نهاية الجدول", "at end of inline table");
        
        return std::make_shared<TomlValue>(table);
    }
    
    void parseTableHeader(TomlValue* root) {
        expect('[', "في بداية رأس الجدول", "at start of table header");
        
        bool isArrayOfTables = false;
        if (peek() == '[') {
            advance();
            isArrayOfTables = true;
        }
        
        skipWhitespace();
        
        // تحليل مسار الجدول (مثل: الحزمة.الإعدادات)
        std::vector<std::string> path;
        path.push_back(parseKey());
        
        while (peek() == '.') {
            advance();
            skipWhitespace();
            path.push_back(parseKey());
            skipWhitespace();
        }
        
        expect(']', "في نهاية رأس الجدول", "at end of table header");
        if (isArrayOfTables) {
            expect(']', "في نهاية رأس مصفوفة الجداول", "at end of array of tables header");
        }
        
        skipToEndOfLine();
        
        // التنقل إلى الجدول المطلوب
        TomlValue* current = root;
        for (size_t i = 0; i < path.size(); ++i) {
            const std::string& key = path[i];
            auto& table = current->asTable();
            
            if (table.find(key) == table.end()) {
                if (i == path.size() - 1 && isArrayOfTables) {
                    table[key] = std::make_shared<TomlValue>(TomlArray{});
                } else {
                    table[key] = std::make_shared<TomlValue>(TomlTable{});
                }
            }
            
            if (i == path.size() - 1 && isArrayOfTables) {
                auto& arr = std::get<TomlArray>(table[key]->asArray());
                arr.push_back(std::make_shared<TomlValue>(TomlTable{}));
                current = arr.back().get();
            } else {
                current = table[key].get();
            }
        }
        
        currentTable_ = current;
    }
    
    void parseKeyValue() {
        std::string key = parseKey();
        
        skipWhitespace();
        expect('=', "بعد المفتاح", "after key");
        
        auto value = parseValue();
        
        currentTable_->asTable()[key] = value;
        
        skipToEndOfLine();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        كاتب TOML | TOML Writer
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📝 TomlWriter - كتابة ملفات TOML
 */
class TomlWriter {
public:
    /**
     * 📄 تحويل جدول إلى نص TOML
     */
    std::string write(const TomlValue& root, bool useArabicKeys = true) {
        if (!root.isTable()) {
            throw std::runtime_error("Root must be a table");
        }
        
        std::ostringstream oss;
        writeTable(oss, root.asTable(), "", useArabicKeys);
        return oss.str();
    }
    
private:
    void writeValue(std::ostream& os, const TomlValue& value) {
        switch (value.type()) {
            case TomlValue::Type::STRING:
                os << '"' << escapeString(value.asString()) << '"';
                break;
                
            case TomlValue::Type::INTEGER:
                os << value.asInteger();
                break;
                
            case TomlValue::Type::FLOAT:
                os << value.asFloat();
                break;
                
            case TomlValue::Type::BOOLEAN:
                os << (value.asBoolean() ? "true" : "false");
                break;
                
            case TomlValue::Type::ARRAY:
                writeArray(os, value.asArray());
                break;
                
            case TomlValue::Type::TABLE:
                writeInlineTable(os, value.asTable());
                break;
        }
    }
    
    void writeArray(std::ostream& os, const TomlArray& arr) {
        os << "[";
        for (size_t i = 0; i < arr.size(); ++i) {
            if (i > 0) os << ", ";
            writeValue(os, *arr[i]);
        }
        os << "]";
    }
    
    void writeInlineTable(std::ostream& os, const TomlTable& table) {
        os << "{ ";
        bool first = true;
        for (const auto& [key, value] : table) {
            if (!first) os << ", ";
            first = false;
            os << key << " = ";
            writeValue(os, *value);
        }
        os << " }";
    }
    
    void writeTable(std::ostream& os, const TomlTable& table, const std::string& prefix, bool useArabic) {
        // كتابة القيم البسيطة أولاً
        for (const auto& [key, value] : table) {
            if (!value->isTable() && !value->isArray()) {
                std::string displayKey = useArabic ? ArabicTomlParser::englishToArabicKey(key) : key;
                os << displayKey << " = ";
                writeValue(os, *value);
                os << "\n";
            }
        }
        
        // ثم كتابة الجداول الفرعية
        for (const auto& [key, value] : table) {
            if (value->isTable()) {
                std::string fullPath = prefix.empty() ? key : prefix + "." + key;
                std::string displayKey = useArabic ? ArabicTomlParser::englishToArabicKey(key) : key;
                std::string displayPath = prefix.empty() ? displayKey : prefix + "." + displayKey;
                
                os << "\n[" << displayPath << "]\n";
                writeTable(os, value->asTable(), fullPath, useArabic);
            }
        }
    }
    
    std::string escapeString(const std::string& s) {
        std::string result;
        for (char c : s) {
            switch (c) {
                case '\n': result += "\\n"; break;
                case '\t': result += "\\t"; break;
                case '\r': result += "\\r"; break;
                case '\\': result += "\\\\"; break;
                case '"': result += "\\\""; break;
                default: result += c; break;
            }
        }
        return result;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

static std::shared_ptr<TomlValue> lastParsedToml;
static std::string lastError;

extern "C" {
    int sad_toml_parse(const char* input) {
        try {
            ArabicTomlParser parser;
            lastParsedToml = parser.parse(input ? input : "");
            lastError.clear();
            return 1; // success
        }
        catch (const TomlParseError& e) {
            lastError = e.what();
            return 0; // failure
        }
    }
    
    const char* sad_toml_get_error() {
        return lastError.c_str();
    }
    
    const char* sad_toml_get_string(const char* path) {
        if (!lastParsedToml || !path) return nullptr;
        
        // Simple path lookup (e.g., "package.name")
        std::string pathStr(path);
        TomlValue* current = lastParsedToml.get();
        
        size_t start = 0;
        while (start < pathStr.size()) {
            size_t dot = pathStr.find('.', start);
            std::string key = (dot == std::string::npos) 
                ? pathStr.substr(start)
                : pathStr.substr(start, dot - start);
            
            if (!current->isTable()) return nullptr;
            
            auto& table = current->asTable();
            auto it = table.find(key);
            if (it == table.end()) {
                // Try Arabic key
                it = table.find(ArabicTomlParser::englishToArabicKey(key));
            }
            if (it == table.end()) return nullptr;
            
            current = it->second.get();
            start = (dot == std::string::npos) ? pathStr.size() : dot + 1;
        }
        
        static std::string result;
        if (current->isString()) {
            result = current->asString();
            return result.c_str();
        }
        return nullptr;
    }
    
    const char* sad_toml_arabic_to_english(const char* key) {
        if (!key) return nullptr;
        static std::string result;
        result = ArabicTomlParser::arabicToEnglishKey(key);
        return result.c_str();
    }
    
    const char* sad_toml_english_to_arabic(const char* key) {
        if (!key) return nullptr;
        static std::string result;
        result = ArabicTomlParser::englishToArabicKey(key);
        return result.c_str();
    }
}

} // namespace pkg
} // namespace sad
