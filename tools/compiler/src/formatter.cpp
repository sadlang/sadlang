// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: formatter.cpp
 * الوصف: منسق الكود للغة ص - Code Formatter for Sad Language
 * المرحلة: Phase 8 - US6 - أدوات التطوير
 * المهمة: T097
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يحتوي على منسق الكود التلقائي للغة ص.
 * 
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║  ما هو منسق الكود؟                                                        ║
 * ╠═══════════════════════════════════════════════════════════════════════════╣
 * ║  أداة تعيد ترتيب الكود بشكل متسق ومقروء وفق قواعد محددة.                 ║
 * ║  يساعد على:                                                               ║
 * ║  - توحيد نمط الكتابة في الفريق                                           ║
 * ║  - تحسين قابلية القراءة                                                  ║
 * ║  - تقليل الخلافات حول التنسيق                                            ║
 * ║  - تسريع مراجعة الكود                                                    ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * قواعد التنسيق الافتراضية:
 * 
 * ┌─────────────────────────────────────────────────────────────────────────────┐
 * │ القاعدة                              │ القيمة الافتراضية                    │
 * ├─────────────────────────────────────────────────────────────────────────────┤
 * │ حجم المسافة البادئة                  │ 4 مسافات                             │
 * │ استخدام المسافات بدل التاب           │ نعم                                  │
 * │ الحد الأقصى لطول السطر               │ 100 حرف                              │
 * │ مسافة بعد الفاصلة                    │ نعم                                  │
 * │ مسافة حول العوامل                    │ نعم                                  │
 * │ مسافة بعد الكلمات المفتاحية          │ نعم                                  │
 * │ أسطر فارغة بين الدوال               │ سطر واحد                             │
 * │ ترتيب الاستيرادات                    │ أبجدياً                              │
 * │ حذف الفراغات الزائدة في نهاية السطر│ نعم                                  │
 * │ سطر جديد في نهاية الملف              │ نعم                                  │
 * └─────────────────────────────────────────────────────────────────────────────┘
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * مثال على التنسيق:
 * 
 * قبل:
 * ```ص
 * دالة حساب(أ:رقم,ب:رقم)->رقم
 * رقم نتيجة=أ+ب
 * إرجاع نتيجة
 * نهاية
 * ```
 * 
 * بعد:
 * ```ص
 * دالة حساب(أ: رقم, ب: رقم) -> رقم
 *     رقم نتيجة = أ + ب
 *     إرجاع نتيجة
 * نهاية
 * ```
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0.0
 * 
 * الحمد لله رب العالمين
 */

#include <string>
#include <vector>
#include <sstream>
#include <regex>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace sad {
namespace formatter {

namespace fs = std::filesystem;

// ═══════════════════════════════════════════════════════════════════════════════
// إعدادات التنسيق - Formatting Options
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct خيارات_التنسيق
 * @brief إعدادات قابلة للتخصيص لمنسق الكود
 */
struct FormatterOptions {
    // المسافة البادئة
    int indent_size = 4;                    // حجم المسافة البادئة
    bool use_spaces = true;                 // استخدام مسافات بدل تاب
    
    // طول السطر
    int max_line_length = 100;              // الحد الأقصى لطول السطر
    bool wrap_long_lines = true;            // التفاف الأسطر الطويلة
    
    // المسافات
    bool space_after_comma = true;          // مسافة بعد الفاصلة
    bool space_around_operators = true;     // مسافة حول العوامل
    bool space_after_keywords = true;       // مسافة بعد الكلمات المفتاحية
    bool space_after_colon = true;          // مسافة بعد النقطتين
    bool space_before_colon = false;        // مسافة قبل النقطتين
    bool space_inside_brackets = false;     // مسافة داخل الأقواس
    bool space_around_arrow = true;         // مسافة حول سهم الإرجاع
    
    // الأسطر الفارغة
    int blank_lines_after_imports = 1;      // أسطر فارغة بعد الاستيرادات
    int blank_lines_between_functions = 1;  // أسطر فارغة بين الدوال
    int blank_lines_between_classes = 2;    // أسطر فارغة بين الأصناف
    int max_consecutive_blank_lines = 2;    // الحد الأقصى للأسطر الفارغة المتتالية
    
    // التنظيف
    bool trim_trailing_whitespace = true;   // حذف الفراغات في نهاية السطر
    bool ensure_final_newline = true;       // ضمان سطر جديد في النهاية
    bool remove_trailing_semicolons = true; // حذف الفواصل المنقوطة الزائدة
    
    // الاستيرادات
    bool sort_imports = true;               // ترتيب الاستيرادات أبجدياً
    bool group_imports = true;              // تجميع الاستيرادات حسب النوع
    
    // الأقواس
    bool brace_same_line = false;           // القوس المعقوص في نفس السطر
    bool align_binary_operators = false;    // محاذاة العوامل الثنائية
};


// ═══════════════════════════════════════════════════════════════════════════════
// أنواع الرموز - Token Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @enum نوع_الرمز
 * @brief أنواع الرموز في الكود
 */
enum class TokenType {
    KEYWORD,            // كلمة مفتاحية
    IDENTIFIER,         // معرف
    NUMBER,             // رقم
    STRING,             // نص
    OPERATOR,           // عامل
    PUNCTUATION,        // علامة ترقيم
    COMMENT,            // تعليق
    WHITESPACE,         // مسافة
    NEWLINE,            // سطر جديد
    END_OF_FILE         // نهاية الملف
};

/**
 * @struct رمز
 * @brief رمز مفرد في الكود
 */
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};


// ═══════════════════════════════════════════════════════════════════════════════
// الكلمات المفتاحية - Keywords
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * قائمة الكلمات المفتاحية في لغة ص
 */
const std::vector<std::string> KEYWORDS = {
    // التحكم في التدفق
    "إذا", "وإلا", "وإلا_إذا", "طالما", "لكل", "كرر",
    // التعريفات
    "دالة", "صنف", "واجهة", "وحدة", "ثابت",
    // التحكم
    "إرجاع", "توقف", "استمر", "نهاية",
    // الاستيراد
    "استورد", "من",
    // الاستثناءات
    "حاول", "امسك", "ارمِ",
    // أخرى
    "جديد", "ذاتي", "فارغ", "صحيح", "خطأ", "في",
    // الأنواع
    "رقم", "نص", "منطقي", "قائمة", "خريطة", "مجموعة", "كائن",
    // المنطقية
    "و", "أو", "ليس"
};

/**
 * العوامل في لغة ص
 */
const std::vector<std::string> OPERATORS = {
    // مقارنة
    "==", "!=", ">=", "<=", ">", "<",
    // حسابية
    "+", "-", "*", "/", "%", "^",
    // تعيين
    "=", "+=", "-=", "*=", "/=",
    // منطقية
    "&&", "||", "!",
    // أخرى
    "->", ".", ":"
};


// ═══════════════════════════════════════════════════════════════════════════════
// صنف المحلل اللغوي المبسط - Simple Lexer
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class Lexer
 * @brief محلل لغوي مبسط لتقسيم الكود إلى رموز
 */
class Lexer {
public:
    /**
     * @brief تقسيم النص إلى رموز
     */
    std::vector<Token> tokenize(const std::string& source) {
        tokens_.clear();
        source_ = source;
        pos_ = 0;
        line_ = 1;
        column_ = 1;
        
        while (pos_ < source_.length()) {
            char c = current();
            
            // تعليق
            if (c == '/' && peek() == '/') {
                scan_comment();
            }
            // نص
            else if (c == '"' || c == '\'') {
                scan_string(c);
            }
            // رقم
            else if (is_digit(c)) {
                scan_number();
            }
            // معرف أو كلمة مفتاحية
            else if (is_identifier_start(c)) {
                scan_identifier();
            }
            // مسافة
            else if (c == ' ' || c == '\t') {
                scan_whitespace();
            }
            // سطر جديد
            else if (c == '\n' || c == '\r') {
                scan_newline();
            }
            // عامل أو علامة ترقيم
            else {
                scan_operator_or_punctuation();
            }
        }
        
        tokens_.push_back({TokenType::END_OF_FILE, "", line_, column_});
        return tokens_;
    }

private:
    std::string source_;
    size_t pos_ = 0;
    int line_ = 1;
    int column_ = 1;
    std::vector<Token> tokens_;
    
    char current() const {
        return pos_ < source_.length() ? source_[pos_] : '\0';
    }
    
    char peek() const {
        return pos_ + 1 < source_.length() ? source_[pos_ + 1] : '\0';
    }
    
    void advance() {
        if (current() == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        pos_++;
    }
    
    bool is_digit(char c) const {
        return c >= '0' && c <= '9';
    }
    
    bool is_identifier_start(char c) const {
        return (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z') ||
               c == '_' ||
               (static_cast<unsigned char>(c) >= 0x80); // UTF-8 للعربية
    }
    
    bool is_identifier_char(char c) const {
        return is_identifier_start(c) || is_digit(c);
    }
    
    void scan_comment() {
        int start_col = column_;
        std::string value;
        
        while (current() != '\n' && current() != '\0') {
            value += current();
            advance();
        }
        
        tokens_.push_back({TokenType::COMMENT, value, line_, start_col});
    }
    
    void scan_string(char quote) {
        int start_col = column_;
        std::string value;
        value += current(); // القوس الافتتاحي
        advance();
        
        while (current() != quote && current() != '\0') {
            if (current() == '\\' && peek() == quote) {
                value += current();
                advance();
            }
            value += current();
            advance();
        }
        
        if (current() == quote) {
            value += current();
            advance();
        }
        
        tokens_.push_back({TokenType::STRING, value, line_, start_col});
    }
    
    void scan_number() {
        int start_col = column_;
        std::string value;
        
        while (is_digit(current()) || current() == '.') {
            value += current();
            advance();
        }
        
        tokens_.push_back({TokenType::NUMBER, value, line_, start_col});
    }
    
    void scan_identifier() {
        int start_col = column_;
        std::string value;
        
        while (is_identifier_char(current())) {
            value += current();
            advance();
        }
        
        // التحقق من أنها كلمة مفتاحية
        TokenType type = TokenType::IDENTIFIER;
        for (const auto& kw : KEYWORDS) {
            if (value == kw) {
                type = TokenType::KEYWORD;
                break;
            }
        }
        
        tokens_.push_back({type, value, line_, start_col});
    }
    
    void scan_whitespace() {
        int start_col = column_;
        std::string value;
        
        while (current() == ' ' || current() == '\t') {
            value += current();
            advance();
        }
        
        tokens_.push_back({TokenType::WHITESPACE, value, line_, start_col});
    }
    
    void scan_newline() {
        int start_col = column_;
        std::string value;
        
        if (current() == '\r' && peek() == '\n') {
            value = "\r\n";
            advance();
            advance();
        } else {
            value = std::string(1, current());
            advance();
        }
        
        tokens_.push_back({TokenType::NEWLINE, value, line_, start_col});
    }
    
    void scan_operator_or_punctuation() {
        int start_col = column_;
        std::string value;
        
        // محاولة إيجاد أطول عامل مطابق
        for (const auto& op : OPERATORS) {
            if (source_.substr(pos_, op.length()) == op) {
                if (op.length() > value.length()) {
                    value = op;
                }
            }
        }
        
        if (!value.empty()) {
            for (size_t i = 0; i < value.length(); ++i) {
                advance();
            }
            tokens_.push_back({TokenType::OPERATOR, value, line_, start_col});
        } else {
            // علامة ترقيم مفردة
            value = std::string(1, current());
            advance();
            tokens_.push_back({TokenType::PUNCTUATION, value, line_, start_col});
        }
    }
};


// ═══════════════════════════════════════════════════════════════════════════════
// صنف المنسق - Formatter Class
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class CodeFormatter
 * @brief منسق الكود الرئيسي
 */
class CodeFormatter {
public:
    /**
     * @brief المنشئ
     */
    explicit CodeFormatter(const FormatterOptions& options = FormatterOptions())
        : options_(options) {}
    
    /**
     * @brief تنسيق كود
     * @param source الكود المصدري
     * @return الكود المنسق
     */
    std::string format(const std::string& source) {
        // تقسيم إلى رموز
        Lexer lexer;
        auto tokens = lexer.tokenize(source);
        
        // إعادة بناء مع التنسيق
        std::ostringstream output;
        int indent_level = 0;
        bool start_of_line = true;
        bool after_newline = false;
        int consecutive_blank_lines = 0;
        Token* prev_token = nullptr;
        
        for (size_t i = 0; i < tokens.size(); ++i) {
            auto& token = tokens[i];
            Token* next_token = (i + 1 < tokens.size()) ? &tokens[i + 1] : nullptr;
            
            switch (token.type) {
                case TokenType::NEWLINE:
                    // التعامل مع الأسطر الفارغة
                    if (after_newline) {
                        consecutive_blank_lines++;
                        if (consecutive_blank_lines > options_.max_consecutive_blank_lines) {
                            continue; // تخطي الأسطر الفارغة الزائدة
                        }
                    } else {
                        consecutive_blank_lines = 0;
                    }
                    
                    // حذف الفراغات في نهاية السطر
                    if (options_.trim_trailing_whitespace) {
                        trim_trailing_whitespace(output);
                    }
                    
                    output << "\n";
                    start_of_line = true;
                    after_newline = true;
                    break;
                    
                case TokenType::WHITESPACE:
                    // تجاهل المسافات في بداية السطر (سنضيف المسافة البادئة)
                    if (!start_of_line) {
                        output << " "; // مسافة واحدة فقط
                    }
                    break;
                    
                case TokenType::KEYWORD:
                    after_newline = false;
                    consecutive_blank_lines = 0;
                    
                    // المسافة البادئة
                    if (start_of_line) {
                        output << get_indent(indent_level);
                        start_of_line = false;
                    }
                    
                    // تعديل مستوى المسافة البادئة
                    if (token.value == "دالة" || token.value == "صنف" || 
                        token.value == "إذا" || token.value == "طالما" ||
                        token.value == "لكل" || token.value == "كرر" ||
                        token.value == "حاول" || token.value == "واجهة") {
                        output << token.value;
                        indent_level++;
                    }
                    else if (token.value == "وإلا" || token.value == "وإلا_إذا" ||
                             token.value == "امسك") {
                        // تراجع مؤقت
                        output.seekp(-options_.indent_size, std::ios_base::cur);
                        output << get_indent(indent_level - 1) << token.value;
                    }
                    else if (token.value == "نهاية") {
                        indent_level = std::max(0, indent_level - 1);
                        output.seekp(-options_.indent_size, std::ios_base::cur);
                        output << get_indent(indent_level) << token.value;
                    }
                    else {
                        output << token.value;
                    }
                    
                    // مسافة بعد الكلمة المفتاحية
                    if (options_.space_after_keywords && next_token &&
                        next_token->type != TokenType::PUNCTUATION &&
                        next_token->type != TokenType::NEWLINE) {
                        output << " ";
                    }
                    break;
                    
                case TokenType::IDENTIFIER:
                case TokenType::NUMBER:
                    after_newline = false;
                    consecutive_blank_lines = 0;
                    
                    if (start_of_line) {
                        output << get_indent(indent_level);
                        start_of_line = false;
                    }
                    
                    output << token.value;
                    break;
                    
                case TokenType::STRING:
                    after_newline = false;
                    consecutive_blank_lines = 0;
                    
                    if (start_of_line) {
                        output << get_indent(indent_level);
                        start_of_line = false;
                    }
                    
                    output << token.value;
                    break;
                    
                case TokenType::OPERATOR:
                    after_newline = false;
                    consecutive_blank_lines = 0;
                    
                    if (start_of_line) {
                        output << get_indent(indent_level);
                        start_of_line = false;
                    }
                    
                    format_operator(output, token, prev_token, next_token);
                    break;
                    
                case TokenType::PUNCTUATION:
                    after_newline = false;
                    consecutive_blank_lines = 0;
                    
                    if (start_of_line) {
                        output << get_indent(indent_level);
                        start_of_line = false;
                    }
                    
                    format_punctuation(output, token, prev_token, next_token);
                    break;
                    
                case TokenType::COMMENT:
                    after_newline = false;
                    consecutive_blank_lines = 0;
                    
                    if (start_of_line) {
                        output << get_indent(indent_level);
                        start_of_line = false;
                    }
                    
                    output << token.value;
                    break;
                    
                case TokenType::END_OF_FILE:
                    // ضمان سطر جديد في النهاية
                    if (options_.ensure_final_newline) {
                        trim_trailing_whitespace(output);
                        output << "\n";
                    }
                    break;
            }
            
            prev_token = &token;
        }
        
        return output.str();
    }
    
    /**
     * @brief تنسيق ملف
     * @param path مسار الملف
     * @return هل نجح التنسيق؟
     */
    bool format_file(const std::string& path) {
        // قراءة الملف
        std::ifstream input(path);
        if (!input.is_open()) {
            std::cerr << "خطأ: لا يمكن فتح الملف: " << path << std::endl;
            return false;
        }
        
        std::ostringstream buffer;
        buffer << input.rdbuf();
        input.close();
        
        std::string source = buffer.str();
        
        // تنسيق
        std::string formatted = format(source);
        
        // كتابة الناتج
        std::ofstream output(path);
        if (!output.is_open()) {
            std::cerr << "خطأ: لا يمكن كتابة الملف: " << path << std::endl;
            return false;
        }
        
        output << formatted;
        output.close();
        
        return true;
    }
    
    /**
     * @brief تنسيق مجلد
     * @param dir_path مسار المجلد
     * @param recursive تنسيق المجلدات الفرعية؟
     * @return عدد الملفات المنسقة
     */
    int format_directory(const std::string& dir_path, bool recursive = true) {
        int count = 0;
        
        auto iterator = recursive 
            ? fs::recursive_directory_iterator(dir_path)
            : fs::recursive_directory_iterator(dir_path, fs::directory_options::none);
        
        for (const auto& entry : fs::directory_iterator(dir_path)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                
                // التحقق من امتداد ملف ص
                if (ext == ".ص") {
                    if (format_file(entry.path().string())) {
                        count++;
                        std::cout << "تم تنسيق: " << entry.path().string() << std::endl;
                    }
                }
            }
            else if (recursive && entry.is_directory()) {
                count += format_directory(entry.path().string(), true);
            }
        }
        
        return count;
    }
    
    /**
     * @brief الحصول على الخيارات
     */
    const FormatterOptions& options() const {
        return options_;
    }
    
    /**
     * @brief تعديل الخيارات
     */
    void set_options(const FormatterOptions& options) {
        options_ = options;
    }

private:
    FormatterOptions options_;
    
    /**
     * @brief الحصول على المسافة البادئة
     */
    std::string get_indent(int level) {
        if (level <= 0) return "";
        
        int spaces = level * options_.indent_size;
        if (options_.use_spaces) {
            return std::string(spaces, ' ');
        } else {
            return std::string(level, '\t');
        }
    }
    
    /**
     * @brief تنسيق عامل
     */
    void format_operator(std::ostringstream& output, 
                        const Token& token,
                        const Token* prev,
                        const Token* next) {
        bool space_before = options_.space_around_operators;
        bool space_after = options_.space_around_operators;
        
        // حالات خاصة
        if (token.value == ":") {
            space_before = options_.space_before_colon;
            space_after = options_.space_after_colon;
        }
        else if (token.value == "->") {
            space_before = options_.space_around_arrow;
            space_after = options_.space_around_arrow;
        }
        else if (token.value == ".") {
            space_before = false;
            space_after = false;
        }
        else if (token.value == "-" && prev && 
                 (prev->type == TokenType::OPERATOR || 
                  prev->value == "(" || prev->value == "," ||
                  prev->type == TokenType::KEYWORD)) {
            // السالب الأحادي
            space_before = true;
            space_after = false;
        }
        
        if (space_before && prev && 
            prev->type != TokenType::WHITESPACE &&
            prev->type != TokenType::NEWLINE) {
            output << " ";
        }
        
        output << token.value;
        
        if (space_after && next && 
            next->type != TokenType::WHITESPACE &&
            next->type != TokenType::NEWLINE) {
            output << " ";
        }
    }
    
    /**
     * @brief تنسيق علامة ترقيم
     */
    void format_punctuation(std::ostringstream& output,
                           const Token& token,
                           const Token* prev,
                           const Token* next) {
        if (token.value == ",") {
            output << token.value;
            if (options_.space_after_comma && next &&
                next->type != TokenType::NEWLINE) {
                output << " ";
            }
        }
        else if (token.value == "(") {
            output << token.value;
            if (options_.space_inside_brackets && next &&
                next->value != ")") {
                output << " ";
            }
        }
        else if (token.value == ")") {
            if (options_.space_inside_brackets && prev &&
                prev->value != "(") {
                output << " ";
            }
            output << token.value;
        }
        else if (token.value == "[") {
            output << token.value;
            if (options_.space_inside_brackets && next &&
                next->value != "]") {
                output << " ";
            }
        }
        else if (token.value == "]") {
            if (options_.space_inside_brackets && prev &&
                prev->value != "[") {
                output << " ";
            }
            output << token.value;
        }
        else if (token.value == "{") {
            output << token.value;
            if (options_.space_inside_brackets && next &&
                next->value != "}") {
                output << " ";
            }
        }
        else if (token.value == "}") {
            if (options_.space_inside_brackets && prev &&
                prev->value != "{") {
                output << " ";
            }
            output << token.value;
        }
        else {
            output << token.value;
        }
    }
    
    /**
     * @brief حذف الفراغات في نهاية الناتج
     */
    void trim_trailing_whitespace(std::ostringstream& output) {
        std::string str = output.str();
        while (!str.empty() && (str.back() == ' ' || str.back() == '\t')) {
            str.pop_back();
        }
        output.str(str);
        output.seekp(0, std::ios_base::end);
    }
};


// ═══════════════════════════════════════════════════════════════════════════════
// دوال المساعدة - Helper Functions
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief قراءة إعدادات التنسيق من ملف
 */
FormatterOptions load_options_from_file(const std::string& path) {
    FormatterOptions options;
    
    std::ifstream file(path);
    if (!file.is_open()) {
        return options; // إرجاع الافتراضي
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // تجاهل التعليقات والأسطر الفارغة
        if (line.empty() || line[0] == '#') continue;
        
        // تحليل المفتاح والقيمة
        size_t eq_pos = line.find('=');
        if (eq_pos == std::string::npos) continue;
        
        std::string key = line.substr(0, eq_pos);
        std::string value = line.substr(eq_pos + 1);
        
        // إزالة الفراغات
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // تطبيق الإعداد
        if (key == "indent_size") options.indent_size = std::stoi(value);
        else if (key == "use_spaces") options.use_spaces = (value == "true");
        else if (key == "max_line_length") options.max_line_length = std::stoi(value);
        else if (key == "space_after_comma") options.space_after_comma = (value == "true");
        else if (key == "space_around_operators") options.space_around_operators = (value == "true");
        else if (key == "trim_trailing_whitespace") options.trim_trailing_whitespace = (value == "true");
        else if (key == "ensure_final_newline") options.ensure_final_newline = (value == "true");
        else if (key == "sort_imports") options.sort_imports = (value == "true");
    }
    
    return options;
}


// ═══════════════════════════════════════════════════════════════════════════════
// نقطة الدخول لسطر الأوامر - CLI Entry Point
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief معالجة سطر الأوامر
 */
int formatter_main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "منسق كود لغة ص\n\n";
        std::cout << "الاستخدام:\n";
        std::cout << "  sad-fmt <ملف.ص>              تنسيق ملف\n";
        std::cout << "  sad-fmt <مجلد>               تنسيق مجلد\n";
        std::cout << "  sad-fmt --check <ملف>        التحقق بدون تعديل\n";
        std::cout << "  sad-fmt --stdin              القراءة من stdin\n";
        std::cout << "  sad-fmt --config <ملف>       استخدام ملف إعدادات\n";
        return 1;
    }
    
    FormatterOptions options;
    std::string path;
    bool check_only = false;
    bool from_stdin = false;
    
    // تحليل الخيارات
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--check") {
            check_only = true;
        }
        else if (arg == "--stdin") {
            from_stdin = true;
        }
        else if (arg == "--config" && i + 1 < argc) {
            options = load_options_from_file(argv[++i]);
        }
        else {
            path = arg;
        }
    }
    
    CodeFormatter formatter(options);
    
    if (from_stdin) {
        // القراءة من stdin
        std::ostringstream buffer;
        buffer << std::cin.rdbuf();
        std::string formatted = formatter.format(buffer.str());
        std::cout << formatted;
        return 0;
    }
    
    if (path.empty()) {
        std::cerr << "خطأ: يجب تحديد ملف أو مجلد\n";
        return 1;
    }
    
    if (fs::is_directory(path)) {
        int count = formatter.format_directory(path);
        std::cout << "تم تنسيق " << count << " ملف\n";
    }
    else if (fs::is_regular_file(path)) {
        if (check_only) {
            // التحقق فقط
            std::ifstream file(path);
            std::ostringstream buffer;
            buffer << file.rdbuf();
            std::string original = buffer.str();
            std::string formatted = formatter.format(original);
            
            if (original != formatted) {
                std::cout << "الملف يحتاج تنسيق: " << path << "\n";
                return 1;
            }
            std::cout << "الملف منسق بشكل صحيح\n";
        }
        else {
            if (formatter.format_file(path)) {
                std::cout << "تم تنسيق: " << path << "\n";
            } else {
                return 1;
            }
        }
    }
    else {
        std::cerr << "خطأ: الملف غير موجود: " << path << "\n";
        return 1;
    }
    
    return 0;
}

} // namespace formatter
} // namespace sad
