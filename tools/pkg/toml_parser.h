// بسم الله الرحمن الرحيم
/**
 * @file toml_parser.h
 * @brief TOML Parser for sad.toml - محلل TOML لملف sad.toml
 * 
 * محلل TOML لقراءة وكتابة ملفات sad.toml
 * يدعم جميع أنواع البيانات الأساسية والجداول والمصفوفات
 * 
 * TOML Parser for reading and writing sad.toml files
 * Supports all basic data types, tables, and arrays
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <variant>
#include <optional>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace sad {
namespace pkg {

// ============================================================================
// TOML Value Types - أنواع قيم TOML
// ============================================================================

/**
 * @brief TOML value variant - متغير قيمة TOML
 * 
 * يمثل أي قيمة في TOML
 * Represents any value in TOML
 */
class TomlValue;

using TomlTable = std::map<std::string, TomlValue>;
using TomlArray = std::vector<TomlValue>;

/**
 * @brief TOML value - قيمة TOML
 */
class TomlValue {
public:
    using ValueType = std::variant<
        std::string,      // String
        int64_t,          // Integer
        double,           // Float
        bool,             // Boolean
        TomlTable,        // Table
        TomlArray         // Array
    >;

    // Constructors - منشئات
    TomlValue() : value_(std::string()) {}
    TomlValue(const std::string& str) : value_(str) {}
    TomlValue(const char* str) : value_(std::string(str)) {}
    TomlValue(int64_t i) : value_(i) {}
    TomlValue(int i) : value_(static_cast<int64_t>(i)) {}
    TomlValue(double d) : value_(d) {}
    TomlValue(bool b) : value_(b) {}
    TomlValue(const TomlTable& t) : value_(t) {}
    TomlValue(const TomlArray& a) : value_(a) {}

    // Type checkers - فاحصات الأنواع
    bool is_string() const { return std::holds_alternative<std::string>(value_); }
    bool is_integer() const { return std::holds_alternative<int64_t>(value_); }
    bool is_float() const { return std::holds_alternative<double>(value_); }
    bool is_bool() const { return std::holds_alternative<bool>(value_); }
    bool is_table() const { return std::holds_alternative<TomlTable>(value_); }
    bool is_array() const { return std::holds_alternative<TomlArray>(value_); }

    // Getters - جالبات
    const std::string& as_string() const { return std::get<std::string>(value_); }
    int64_t as_integer() const { return std::get<int64_t>(value_); }
    double as_float() const { return std::get<double>(value_); }
    bool as_bool() const { return std::get<bool>(value_); }
    const TomlTable& as_table() const { return std::get<TomlTable>(value_); }
    const TomlArray& as_array() const { return std::get<TomlArray>(value_); }

    // Mutable getters - جالبات قابلة للتعديل
    TomlTable& as_table() { return std::get<TomlTable>(value_); }
    TomlArray& as_array() { return std::get<TomlArray>(value_); }

    // Table access - الوصول للجدول
    TomlValue& operator[](const std::string& key) {
        if (!is_table()) {
            value_ = TomlTable();
        }
        return as_table()[key];
    }

    const TomlValue& operator[](const std::string& key) const {
        return as_table().at(key);
    }

    // Array access - الوصول للمصفوفة
    TomlValue& operator[](size_t index) {
        if (!is_array()) {
            value_ = TomlArray();
        }
        return as_array()[index];
    }

    const TomlValue& operator[](size_t index) const {
        return as_array().at(index);
    }

    // Size - الحجم
    size_t size() const {
        if (is_table()) return as_table().size();
        if (is_array()) return as_array().size();
        return 0;
    }

    // Contains key - يحتوي على مفتاح
    bool contains(const std::string& key) const {
        if (!is_table()) return false;
        return as_table().find(key) != as_table().end();
    }

private:
    ValueType value_;
};

// ============================================================================
// TOML Parser - محلل TOML
// ============================================================================

/**
 * @brief TOML parser - محلل TOML
 * 
 * يحلل ملفات TOML ويحولها إلى هيكل بيانات
 * Parses TOML files into data structure
 */
class TomlParser {
public:
    /**
     * @brief Parse TOML from string - تحليل TOML من نص
     */
    static TomlTable parse(const std::string& content) {
        TomlParser parser(content);
        return parser.parse_document();
    }

    /**
     * @brief Parse TOML from file - تحليل TOML من ملف
     */
    static TomlTable parse_file(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filepath);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return parse(buffer.str());
    }

    /**
     * @brief Serialize TOML to string - تسلسل TOML إلى نص
     */
    static std::string serialize(const TomlTable& table) {
        std::stringstream ss;
        serialize_table(ss, table, "");
        return ss.str();
    }

    /**
     * @brief Write TOML to file - كتابة TOML إلى ملف
     */
    static void write_file(const std::string& filepath, const TomlTable& table) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot write file: " + filepath);
        }
        file << serialize(table);
    }

private:
    std::string content_;
    size_t pos_;
    int line_;

    TomlParser(const std::string& content) 
        : content_(content), pos_(0), line_(1) {}

    // Parse document - تحليل المستند
    TomlTable parse_document() {
        TomlTable root;
        std::string current_table = "";

        while (pos_ < content_.length()) {
            skip_whitespace_and_comments();
            if (pos_ >= content_.length()) break;

            char c = content_[pos_];

            if (c == '[') {
                // Table header - رأس الجدول
                current_table = parse_table_header();
                if (root.find(current_table) == root.end()) {
                    root[current_table] = TomlTable();
                }
            } else if (std::isalnum(c) || c == '_' || c == '"' || c == '\'') {
                // Key-value pair - زوج مفتاح-قيمة
                auto [key, value] = parse_key_value();
                
                if (current_table.empty()) {
                    root[key] = value;
                } else {
                    root[current_table].as_table()[key] = value;
                }
            } else {
                error("Unexpected character: " + std::string(1, c));
            }

            skip_whitespace_and_comments();
        }

        return root;
    }

    // Parse table header - تحليل رأس الجدول
    std::string parse_table_header() {
        expect('[');
        skip_whitespace();

        std::string name;
        while (pos_ < content_.length() && content_[pos_] != ']') {
            name += content_[pos_++];
        }

        expect(']');
        skip_whitespace();
        
        // Trim whitespace - إزالة المسافات
        size_t start = name.find_first_not_of(" \t");
        size_t end = name.find_last_not_of(" \t");
        if (start != std::string::npos) {
            name = name.substr(start, end - start + 1);
        }

        return name;
    }

    // Parse key-value pair - تحليل زوج مفتاح-قيمة
    std::pair<std::string, TomlValue> parse_key_value() {
        std::string key = parse_key();
        skip_whitespace();
        expect('=');
        skip_whitespace();
        TomlValue value = parse_value();
        skip_whitespace();
        return {key, value};
    }

    // Parse key - تحليل المفتاح
    std::string parse_key() {
        std::string key;
        
        // Handle quoted keys - معالجة المفاتيح المقتبسة
        if (content_[pos_] == '"' || content_[pos_] == '\'') {
            return parse_string();
        }

        // Bare key - مفتاح عادي
        while (pos_ < content_.length() && 
               (std::isalnum(content_[pos_]) || content_[pos_] == '_' || content_[pos_] == '-')) {
            key += content_[pos_++];
        }

        return key;
    }

    // Parse value - تحليل القيمة
    TomlValue parse_value() {
        char c = content_[pos_];

        if (c == '"' || c == '\'') {
            return TomlValue(parse_string());
        }
        else if (c == '[') {
            return TomlValue(parse_array());
        }
        else if (c == '{') {
            return TomlValue(parse_inline_table());
        }
        else if (c == 't' || c == 'f') {
            return TomlValue(parse_boolean());
        }
        else if (std::isdigit(c) || c == '-' || c == '+') {
            return parse_number();
        }
        else {
            error("Invalid value");
            return TomlValue();
        }
    }

    // Parse string - تحليل النص
    std::string parse_string() {
        char quote = content_[pos_++];
        std::string str;

        while (pos_ < content_.length() && content_[pos_] != quote) {
            if (content_[pos_] == '\\' && pos_ + 1 < content_.length()) {
                pos_++;
                char escape = content_[pos_++];
                switch (escape) {
                    case 'n': str += '\n'; break;
                    case 't': str += '\t'; break;
                    case 'r': str += '\r'; break;
                    case '\\': str += '\\'; break;
                    case '"': str += '"'; break;
                    case '\'': str += '\''; break;
                    default: str += escape; break;
                }
            } else {
                str += content_[pos_++];
            }
        }

        if (pos_ >= content_.length()) {
            error("Unterminated string");
        }

        pos_++; // Skip closing quote
        return str;
    }

    // Parse array - تحليل المصفوفة
    TomlArray parse_array() {
        expect('[');
        TomlArray arr;
        skip_whitespace();

        while (pos_ < content_.length() && content_[pos_] != ']') {
            arr.push_back(parse_value());
            skip_whitespace();

            if (content_[pos_] == ',') {
                pos_++;
                skip_whitespace();
            }
        }

        expect(']');
        return arr;
    }

    // Parse inline table - تحليل جدول سطري
    TomlTable parse_inline_table() {
        expect('{');
        TomlTable table;
        skip_whitespace();

        while (pos_ < content_.length() && content_[pos_] != '}') {
            auto [key, value] = parse_key_value();
            table[key] = value;
            skip_whitespace();

            if (content_[pos_] == ',') {
                pos_++;
                skip_whitespace();
            }
        }

        expect('}');
        return table;
    }

    // Parse boolean - تحليل منطقي
    bool parse_boolean() {
        if (content_.substr(pos_, 4) == "true") {
            pos_ += 4;
            return true;
        }
        else if (content_.substr(pos_, 5) == "false") {
            pos_ += 5;
            return false;
        }
        error("Invalid boolean");
        return false;
    }

    // Parse number - تحليل رقم
    TomlValue parse_number() {
        std::string num;
        bool is_float = false;

        if (content_[pos_] == '-' || content_[pos_] == '+') {
            num += content_[pos_++];
        }

        while (pos_ < content_.length() && 
               (std::isdigit(content_[pos_]) || content_[pos_] == '.' || 
                content_[pos_] == 'e' || content_[pos_] == 'E' ||
                content_[pos_] == '-' || content_[pos_] == '+')) {
            if (content_[pos_] == '.') is_float = true;
            num += content_[pos_++];
        }

        if (is_float) {
            return TomlValue(std::stod(num));
        } else {
            return TomlValue(std::stoll(num));
        }
    }

    // Helper functions - دوال مساعدة
    void skip_whitespace() {
        while (pos_ < content_.length() && 
               (content_[pos_] == ' ' || content_[pos_] == '\t' || 
                content_[pos_] == '\n' || content_[pos_] == '\r')) {
            if (content_[pos_] == '\n') line_++;
            pos_++;
        }
    }

    void skip_whitespace_and_comments() {
        while (pos_ < content_.length()) {
            skip_whitespace();
            if (pos_ < content_.length() && content_[pos_] == '#') {
                // Skip comment - تخطي التعليق
                while (pos_ < content_.length() && content_[pos_] != '\n') {
                    pos_++;
                }
            } else {
                break;
            }
        }
    }

    void expect(char c) {
        if (pos_ >= content_.length() || content_[pos_] != c) {
            error("Expected '" + std::string(1, c) + "'");
        }
        pos_++;
    }

    void error(const std::string& msg) {
        throw std::runtime_error("TOML parse error at line " + 
                                std::to_string(line_) + ": " + msg);
    }

    // Serialization helpers - مساعدات التسلسل
    static void serialize_table(std::stringstream& ss, const TomlTable& table, 
                                const std::string& prefix) {
        // Write simple key-value pairs first - كتابة أزواج المفتاح-القيمة أولاً
        for (const auto& [key, value] : table) {
            if (!value.is_table()) {
                ss << key << " = ";
                serialize_value(ss, value);
                ss << "\n";
            }
        }

        // Write tables - كتابة الجداول
        for (const auto& [key, value] : table) {
            if (value.is_table()) {
                std::string full_key = prefix.empty() ? key : prefix + "." + key;
                ss << "\n[" << full_key << "]\n";
                serialize_table(ss, value.as_table(), full_key);
            }
        }
    }

    static void serialize_value(std::stringstream& ss, const TomlValue& value) {
        if (value.is_string()) {
            ss << "\"" << escape_string(value.as_string()) << "\"";
        }
        else if (value.is_integer()) {
            ss << value.as_integer();
        }
        else if (value.is_float()) {
            ss << value.as_float();
        }
        else if (value.is_bool()) {
            ss << (value.as_bool() ? "true" : "false");
        }
        else if (value.is_array()) {
            ss << "[";
            const auto& arr = value.as_array();
            for (size_t i = 0; i < arr.size(); i++) {
                serialize_value(ss, arr[i]);
                if (i < arr.size() - 1) ss << ", ";
            }
            ss << "]";
        }
        else if (value.is_table()) {
            ss << "{";
            const auto& table = value.as_table();
            size_t i = 0;
            for (const auto& [key, val] : table) {
                ss << key << " = ";
                serialize_value(ss, val);
                if (i < table.size() - 1) ss << ", ";
                i++;
            }
            ss << "}";
        }
    }

    static std::string escape_string(const std::string& str) {
        std::string result;
        for (char c : str) {
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

} // namespace pkg
} // namespace sad
