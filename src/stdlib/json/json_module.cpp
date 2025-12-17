// بسم الله الرحمن الرحيم
/**
 * @file json_module.cpp
 * @brief JSON Module Implementation - تنفيذ وحدة JSON
 * 
 * الحمد لله رب العالمين
 */

#include "stdlib/json/json_module.h"
#include <sstream>
#include <iomanip>
#include <cctype>
#include <cmath>
#include <stack>
#include <algorithm>

namespace sad {
namespace stdlib {
namespace json {

// ============================================================================
// JsonValue Implementation - تنفيذ قيمة JSON
// ============================================================================

struct JsonValue::Impl {
    std::variant<std::monostate, bool, double, std::string, 
                 JsonArray, JsonObject> data;
    
    Impl() : data(std::monostate{}) {}
    
    Impl(const Impl& other) : data(other.data) {}
    
    Impl(Impl&& other) noexcept : data(std::move(other.data)) {}
    
    Impl& operator=(const Impl& other) {
        if (this != &other) {
            data = other.data;
        }
        return *this;
    }
    
    Impl& operator=(Impl&& other) noexcept {
        data = std::move(other.data);
        return *this;
    }
    
    template<typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, Impl>>>
    Impl(T&& value) : data(std::forward<T>(value)) {}
};

JsonValue::JsonValue() : pImpl(std::make_unique<Impl>()) {}

JsonValue::JsonValue(bool value) 
    : pImpl(std::make_unique<Impl>(value)) {}

JsonValue::JsonValue(double value)
    : pImpl(std::make_unique<Impl>(value)) {}

JsonValue::JsonValue(int value)
    : pImpl(std::make_unique<Impl>(static_cast<double>(value))) {}

JsonValue::JsonValue(const std::string& value)
    : pImpl(std::make_unique<Impl>(value)) {}

JsonValue::JsonValue(const char* value)
    : pImpl(std::make_unique<Impl>(std::string(value))) {}

JsonValue::JsonValue(const JsonArray& value)
    : pImpl(std::make_unique<Impl>(value)) {}

JsonValue::JsonValue(const JsonObject& value)
    : pImpl(std::make_unique<Impl>(value)) {}

JsonValue::~JsonValue() = default;

JsonValue::JsonValue(const JsonValue& other)
    : pImpl(std::make_unique<Impl>(*other.pImpl)) {}

JsonValue::JsonValue(JsonValue&& other) noexcept = default;

JsonValue& JsonValue::operator=(const JsonValue& other) {
    if (this != &other) {
        pImpl = std::make_unique<Impl>(*other.pImpl);
    }
    return *this;
}

JsonValue& JsonValue::operator=(JsonValue&& other) noexcept = default;

JsonType JsonValue::type() const {
    return std::visit([](auto&& arg) -> JsonType {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) return JsonType::NULL_TYPE;
        else if constexpr (std::is_same_v<T, bool>) return JsonType::BOOLEAN;
        else if constexpr (std::is_same_v<T, double>) return JsonType::NUMBER;
        else if constexpr (std::is_same_v<T, std::string>) return JsonType::STRING;
        else if constexpr (std::is_same_v<T, JsonArray>) return JsonType::ARRAY;
        else if constexpr (std::is_same_v<T, JsonObject>) return JsonType::OBJECT;
        else return JsonType::NULL_TYPE;
    }, pImpl->data);
}

bool JsonValue::is_null() const { return type() == JsonType::NULL_TYPE; }
bool JsonValue::is_boolean() const { return type() == JsonType::BOOLEAN; }
bool JsonValue::is_number() const { return type() == JsonType::NUMBER; }
bool JsonValue::is_string() const { return type() == JsonType::STRING; }
bool JsonValue::is_array() const { return type() == JsonType::ARRAY; }
bool JsonValue::is_object() const { return type() == JsonType::OBJECT; }

bool JsonValue::as_boolean() const {
    if (!is_boolean()) {
        throw JsonException("Value is not a boolean / القيمة ليست منطقية");
    }
    return std::get<bool>(pImpl->data);
}

double JsonValue::as_number() const {
    if (!is_number()) {
        throw JsonException("Value is not a number / القيمة ليست رقماً");
    }
    return std::get<double>(pImpl->data);
}

int JsonValue::as_int() const {
    return static_cast<int>(as_number());
}

std::string JsonValue::as_string() const {
    if (!is_string()) {
        throw JsonException("Value is not a string / القيمة ليست نصاً");
    }
    return std::get<std::string>(pImpl->data);
}

JsonArray& JsonValue::as_array() {
    if (!is_array()) {
        throw JsonException("Value is not an array / القيمة ليست مصفوفة");
    }
    return std::get<JsonArray>(pImpl->data);
}

const JsonArray& JsonValue::as_array() const {
    if (!is_array()) {
        throw JsonException("Value is not an array / القيمة ليست مصفوفة");
    }
    return std::get<JsonArray>(pImpl->data);
}

JsonObject& JsonValue::as_object() {
    if (!is_object()) {
        throw JsonException("Value is not an object / القيمة ليست كائناً");
    }
    return std::get<JsonObject>(pImpl->data);
}

const JsonObject& JsonValue::as_object() const {
    if (!is_object()) {
        throw JsonException("Value is not an object / القيمة ليست كائناً");
    }
    return std::get<JsonObject>(pImpl->data);
}

bool JsonValue::get_boolean(bool default_value) const {
    return is_boolean() ? as_boolean() : default_value;
}

double JsonValue::get_number(double default_value) const {
    return is_number() ? as_number() : default_value;
}

int JsonValue::get_int(int default_value) const {
    return is_number() ? as_int() : default_value;
}

std::string JsonValue::get_string(const std::string& default_value) const {
    return is_string() ? as_string() : default_value;
}

JsonValue& JsonValue::operator[](size_t index) {
    return as_array().at(index);
}

const JsonValue& JsonValue::operator[](size_t index) const {
    return as_array().at(index);
}

JsonValue& JsonValue::operator[](const std::string& key) {
    return as_object().get(key);
}

const JsonValue& JsonValue::operator[](const std::string& key) const {
    return as_object().get(key);
}

std::string JsonValue::to_string(const JsonStringifyOptions& options) const {
    return JsonGenerator::stringify(*this, options);
}

JsonValue JsonValue::parse(const std::string& json, const JsonParseOptions& options) {
    return JsonParser::parse(json, options);
}

bool JsonValue::operator==(const JsonValue& other) const {
    return json_equals(*this, other);
}

bool JsonValue::operator!=(const JsonValue& other) const {
    return !(*this == other);
}

// ============================================================================
// JsonArray Implementation - تنفيذ مصفوفة JSON
// ============================================================================

struct JsonArray::Impl {
    std::vector<JsonValue> values;
};

JsonArray::JsonArray() : pImpl(std::make_unique<Impl>()) {}
JsonArray::~JsonArray() = default;

JsonArray::JsonArray(const JsonArray& other)
    : pImpl(std::make_unique<Impl>(*other.pImpl)) {}

JsonArray::JsonArray(JsonArray&& other) noexcept = default;

JsonArray& JsonArray::operator=(const JsonArray& other) {
    if (this != &other) {
        pImpl = std::make_unique<Impl>(*other.pImpl);
    }
    return *this;
}

JsonArray& JsonArray::operator=(JsonArray&& other) noexcept = default;

void JsonArray::push(const JsonValue& value) {
    pImpl->values.push_back(value);
}

void JsonArray::push(JsonValue&& value) {
    pImpl->values.push_back(std::move(value));
}

void JsonArray::pop() {
    if (!pImpl->values.empty()) {
        pImpl->values.pop_back();
    }
}

JsonValue& JsonArray::at(size_t index) {
    if (index >= pImpl->values.size()) {
        throw JsonException("Array index out of range / فهرس المصفوفة خارج النطاق");
    }
    return pImpl->values[index];
}

const JsonValue& JsonArray::at(size_t index) const {
    if (index >= pImpl->values.size()) {
        throw JsonException("Array index out of range / فهرس المصفوفة خارج النطاق");
    }
    return pImpl->values[index];
}

size_t JsonArray::size() const {
    return pImpl->values.size();
}

bool JsonArray::empty() const {
    return pImpl->values.empty();
}

void JsonArray::clear() {
    pImpl->values.clear();
}

JsonValue& JsonArray::operator[](size_t index) {
    return at(index);
}

const JsonValue& JsonArray::operator[](size_t index) const {
    return at(index);
}

// ============================================================================
// JsonObject Implementation - تنفيذ كائن JSON
// ============================================================================

struct JsonObject::Impl {
    std::map<std::string, JsonValue> values;
};

JsonObject::JsonObject() : pImpl(std::make_unique<Impl>()) {}
JsonObject::~JsonObject() = default;

JsonObject::JsonObject(const JsonObject& other)
    : pImpl(std::make_unique<Impl>(*other.pImpl)) {}

JsonObject::JsonObject(JsonObject&& other) noexcept = default;

JsonObject& JsonObject::operator=(const JsonObject& other) {
    if (this != &other) {
        pImpl = std::make_unique<Impl>(*other.pImpl);
    }
    return *this;
}

JsonObject& JsonObject::operator=(JsonObject&& other) noexcept = default;

void JsonObject::set(const std::string& key, const JsonValue& value) {
    pImpl->values[key] = value;
}

void JsonObject::set(const std::string& key, JsonValue&& value) {
    pImpl->values[key] = std::move(value);
}

JsonValue& JsonObject::get(const std::string& key) {
    auto it = pImpl->values.find(key);
    if (it == pImpl->values.end()) {
        // Insert null value if key doesn't exist
        pImpl->values[key] = JsonValue();
        return pImpl->values[key];
    }
    return it->second;
}

const JsonValue& JsonObject::get(const std::string& key) const {
    auto it = pImpl->values.find(key);
    if (it == pImpl->values.end()) {
        throw JsonException("Key not found / المفتاح غير موجود: " + key);
    }
    return it->second;
}

bool JsonObject::has(const std::string& key) const {
    return pImpl->values.find(key) != pImpl->values.end();
}

void JsonObject::remove(const std::string& key) {
    pImpl->values.erase(key);
}

std::vector<std::string> JsonObject::keys() const {
    std::vector<std::string> result;
    result.reserve(pImpl->values.size());
    for (const auto& pair : pImpl->values) {
        result.push_back(pair.first);
    }
    return result;
}

size_t JsonObject::size() const {
    return pImpl->values.size();
}

bool JsonObject::empty() const {
    return pImpl->values.empty();
}

void JsonObject::clear() {
    pImpl->values.clear();
}

JsonValue& JsonObject::operator[](const std::string& key) {
    return get(key);
}

const JsonValue& JsonObject::operator[](const std::string& key) const {
    return get(key);
}

// ============================================================================
// JSON Parser - محلل JSON
// ============================================================================

namespace {
    class Lexer {
    public:
        enum class TokenType {
            LEFT_BRACE,    // {
            RIGHT_BRACE,   // }
            LEFT_BRACKET,  // [
            RIGHT_BRACKET, // ]
            COLON,         // :
            COMMA,         // ,
            STRING,
            NUMBER,
            TRUE,
            FALSE,
            NULL_TOKEN,
            END
        };
        
        struct Token {
            TokenType type;
            std::string value;
            size_t line;
            size_t column;
        };
        
        Lexer(const std::string& input, const JsonParseOptions& options)
            : input_(input), pos_(0), line_(1), column_(1), options_(options) {}
        
        Token next_token() {
            skip_whitespace();
            
            if (pos_ >= input_.size()) {
                return {TokenType::END, "", line_, column_};
            }
            
            char c = current();
            size_t token_line = line_;
            size_t token_column = column_;
            
            switch (c) {
                case '{': advance(); return {TokenType::LEFT_BRACE, "{", token_line, token_column};
                case '}': advance(); return {TokenType::RIGHT_BRACE, "}", token_line, token_column};
                case '[': advance(); return {TokenType::LEFT_BRACKET, "[", token_line, token_column};
                case ']': advance(); return {TokenType::RIGHT_BRACKET, "]", token_line, token_column};
                case ':': advance(); return {TokenType::COLON, ":", token_line, token_column};
                case ',': advance(); return {TokenType::COMMA, ",", token_line, token_column};
                case '"': return read_string(token_line, token_column);
                case 't': return read_keyword("true", TokenType::TRUE, token_line, token_column);
                case 'f': return read_keyword("false", TokenType::FALSE, token_line, token_column);
                case 'n': return read_keyword("null", TokenType::NULL_TOKEN, token_line, token_column);
                default:
                    if (c == '-' || std::isdigit(c)) {
                        return read_number(token_line, token_column);
                    }
                    throw JsonException("Unexpected character / حرف غير متوقع", line_, column_);
            }
        }
        
    private:
        std::string input_;
        size_t pos_;
        size_t line_;
        size_t column_;
        JsonParseOptions options_;
        
        char current() const {
            return pos_ < input_.size() ? input_[pos_] : '\0';
        }
        
        char peek(size_t offset = 1) const {
            size_t p = pos_ + offset;
            return p < input_.size() ? input_[p] : '\0';
        }
        
        void advance() {
            if (pos_ < input_.size()) {
                if (input_[pos_] == '\n') {
                    line_++;
                    column_ = 1;
                } else {
                    column_++;
                }
                pos_++;
            }
        }
        
        void skip_whitespace() {
            while (pos_ < input_.size() && std::isspace(current())) {
                advance();
            }
            
            // Skip comments if allowed
            if (options_.allow_comments) {
                if (current() == '/' && peek() == '/') {
                    // Line comment
                    while (current() != '\n' && current() != '\0') {
                        advance();
                    }
                    skip_whitespace();
                } else if (current() == '/' && peek() == '*') {
                    // Block comment
                    advance(); // skip /
                    advance(); // skip *
                    while (!(current() == '*' && peek() == '/') && current() != '\0') {
                        advance();
                    }
                    if (current() != '\0') {
                        advance(); // skip *
                        advance(); // skip /
                    }
                    skip_whitespace();
                }
            }
        }
        
        Token read_string(size_t line, size_t col) {
            std::string value;
            advance(); // skip opening "
            
            while (current() != '"' && current() != '\0') {
                if (current() == '\\') {
                    advance();
                    switch (current()) {
                        case '"': value += '"'; break;
                        case '\\': value += '\\'; break;
                        case '/': value += '/'; break;
                        case 'b': value += '\b'; break;
                        case 'f': value += '\f'; break;
                        case 'n': value += '\n'; break;
                        case 'r': value += '\r'; break;
                        case 't': value += '\t'; break;
                        case 'u': {
                            // Unicode escape \uXXXX
                            std::string hex;
                            for (int i = 0; i < 4; i++) {
                                advance();
                                if (!std::isxdigit(current())) {
                                    throw JsonException("Invalid unicode escape / هروب يونيكود غير صالح", line_, column_);
                                }
                                hex += current();
                            }
                            int codepoint = std::stoi(hex, nullptr, 16);
                            // Simple UTF-8 encoding for BMP
                            if (codepoint < 0x80) {
                                value += static_cast<char>(codepoint);
                            } else if (codepoint < 0x800) {
                                value += static_cast<char>(0xC0 | (codepoint >> 6));
                                value += static_cast<char>(0x80 | (codepoint & 0x3F));
                            } else {
                                value += static_cast<char>(0xE0 | (codepoint >> 12));
                                value += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                                value += static_cast<char>(0x80 | (codepoint & 0x3F));
                            }
                            break;
                        }
                        default:
                            throw JsonException("Invalid escape sequence / تسلسل هروب غير صالح", line_, column_);
                    }
                } else {
                    value += current();
                }
                advance();
            }
            
            if (current() != '"') {
                throw JsonException("Unterminated string / نص غير مكتمل", line, col);
            }
            advance(); // skip closing "
            
            return {TokenType::STRING, value, line, col};
        }
        
        Token read_number(size_t line, size_t col) {
            std::string value;
            
            if (current() == '-') {
                value += current();
                advance();
            }
            
            if (!std::isdigit(current())) {
                throw JsonException("Invalid number / رقم غير صالح", line_, column_);
            }
            
            if (current() == '0') {
                value += current();
                advance();
            } else {
                while (std::isdigit(current())) {
                    value += current();
                    advance();
                }
            }
            
            if (current() == '.') {
                value += current();
                advance();
                if (!std::isdigit(current())) {
                    throw JsonException("Invalid number / رقم غير صالح", line_, column_);
                }
                while (std::isdigit(current())) {
                    value += current();
                    advance();
                }
            }
            
            if (current() == 'e' || current() == 'E') {
                value += current();
                advance();
                if (current() == '+' || current() == '-') {
                    value += current();
                    advance();
                }
                if (!std::isdigit(current())) {
                    throw JsonException("Invalid number / رقم غير صالح", line_, column_);
                }
                while (std::isdigit(current())) {
                    value += current();
                    advance();
                }
            }
            
            return {TokenType::NUMBER, value, line, col};
        }
        
        Token read_keyword(const std::string& keyword, TokenType type, size_t line, size_t col) {
            for (char c : keyword) {
                if (current() != c) {
                    throw JsonException("Invalid keyword / كلمة مفتاحية غير صالحة", line_, column_);
                }
                advance();
            }
            return {type, keyword, line, col};
        }
    };
    
    class Parser {
    public:
        Parser(const std::string& json, const JsonParseOptions& options)
            : lexer_(json, options), options_(options) {
            current_token_ = lexer_.next_token();
        }
        
        JsonValue parse() {
            JsonValue result = parse_value();
            if (current_token_.type != Lexer::TokenType::END) {
                throw JsonException("Expected end of input / متوقع نهاية الإدخال", 
                                  current_token_.line, current_token_.column);
            }
            return result;
        }
        
    private:
        Lexer lexer_;
        Lexer::Token current_token_;
        JsonParseOptions options_;
        size_t depth_ = 0;
        
        void advance() {
            current_token_ = lexer_.next_token();
        }
        
        JsonValue parse_value() {
            if (++depth_ > options_.max_depth) {
                throw JsonException("Maximum nesting depth exceeded / تجاوز الحد الأقصى للتعشيش",
                                  current_token_.line, current_token_.column);
            }
            
            JsonValue result;
            
            switch (current_token_.type) {
                case Lexer::TokenType::NULL_TOKEN:
                    result = JsonValue();
                    advance();
                    break;
                case Lexer::TokenType::TRUE:
                    result = JsonValue(true);
                    advance();
                    break;
                case Lexer::TokenType::FALSE:
                    result = JsonValue(false);
                    advance();
                    break;
                case Lexer::TokenType::NUMBER: {
                    double num = std::stod(current_token_.value);
                    if (!options_.allow_nan_inf && (std::isnan(num) || std::isinf(num))) {
                        throw JsonException("NaN and Infinity not allowed / NaN و Infinity غير مسموح",
                                          current_token_.line, current_token_.column);
                    }
                    result = JsonValue(num);
                    advance();
                    break;
                }
                case Lexer::TokenType::STRING:
                    result = JsonValue(current_token_.value);
                    advance();
                    break;
                case Lexer::TokenType::LEFT_BRACE:
                    result = parse_object();
                    break;
                case Lexer::TokenType::LEFT_BRACKET:
                    result = parse_array();
                    break;
                default:
                    throw JsonException("Unexpected token / رمز غير متوقع",
                                      current_token_.line, current_token_.column);
            }
            
            depth_--;
            return result;
        }
        
        JsonValue parse_object() {
            JsonObject obj;
            advance(); // skip {
            
            while (current_token_.type != Lexer::TokenType::RIGHT_BRACE) {
                if (current_token_.type != Lexer::TokenType::STRING) {
                    throw JsonException("Expected string key / متوقع مفتاح نصي",
                                      current_token_.line, current_token_.column);
                }
                
                std::string key = current_token_.value;
                advance();
                
                if (current_token_.type != Lexer::TokenType::COLON) {
                    throw JsonException("Expected colon / متوقع نقطتان رأسيتان",
                                      current_token_.line, current_token_.column);
                }
                advance();
                
                obj.set(key, parse_value());
                
                if (current_token_.type == Lexer::TokenType::COMMA) {
                    advance();
                    if (current_token_.type == Lexer::TokenType::RIGHT_BRACE) {
                        if (!options_.allow_trailing_commas) {
                            throw JsonException("Trailing comma not allowed / الفاصلة الزائدة غير مسموحة",
                                              current_token_.line, current_token_.column);
                        }
                        break;
                    }
                } else if (current_token_.type != Lexer::TokenType::RIGHT_BRACE) {
                    throw JsonException("Expected comma or } / متوقع فاصلة أو }",
                                      current_token_.line, current_token_.column);
                }
            }
            
            advance(); // skip }
            return JsonValue(obj);
        }
        
        JsonValue parse_array() {
            JsonArray arr;
            advance(); // skip [
            
            while (current_token_.type != Lexer::TokenType::RIGHT_BRACKET) {
                arr.push(parse_value());
                
                if (current_token_.type == Lexer::TokenType::COMMA) {
                    advance();
                    if (current_token_.type == Lexer::TokenType::RIGHT_BRACKET) {
                        if (!options_.allow_trailing_commas) {
                            throw JsonException("Trailing comma not allowed / الفاصلة الزائدة غير مسموحة",
                                              current_token_.line, current_token_.column);
                        }
                        break;
                    }
                } else if (current_token_.type != Lexer::TokenType::RIGHT_BRACKET) {
                    throw JsonException("Expected comma or ] / متوقع فاصلة أو ]",
                                      current_token_.line, current_token_.column);
                }
            }
            
            advance(); // skip ]
            return JsonValue(arr);
        }
    };
}

JsonValue JsonParser::parse(const std::string& json, const JsonParseOptions& options) {
    try {
        Parser parser(json, options);
        return parser.parse();
    } catch (const std::exception& e) {
        throw JsonException(std::string("Parse error / خطأ في التحليل: ") + e.what());
    }
}

bool JsonParser::validate(const std::string& json) {
    try {
        parse(json);
        return true;
    } catch (...) {
        return false;
    }
}

std::string JsonParser::get_validation_errors(const std::string& json) {
    try {
        parse(json);
        return "";
    } catch (const JsonException& e) {
        return e.what();
    } catch (const std::exception& e) {
        return e.what();
    }
}

// ============================================================================
// JSON Generator - مولد JSON
// ============================================================================

namespace {
    class Generator {
    public:
        Generator(const JsonStringifyOptions& options) : options_(options) {}
        
        std::string generate(const JsonValue& value, int indent_level = 0) {
            std::ostringstream oss;
            
            switch (value.type()) {
                case JsonType::NULL_TYPE:
                    oss << "null";
                    break;
                case JsonType::BOOLEAN:
                    oss << (value.as_boolean() ? "true" : "false");
                    break;
                case JsonType::NUMBER: {
                    double num = value.as_number();
                    if (std::floor(num) == num && num >= INT_MIN && num <= INT_MAX) {
                        oss << static_cast<int>(num);
                    } else {
                        oss << std::setprecision(15) << num;
                    }
                    break;
                }
                case JsonType::STRING:
                    oss << '"' << json_escape(value.as_string()) << '"';
                    break;
                case JsonType::ARRAY:
                    oss << generate_array(value.as_array(), indent_level);
                    break;
                case JsonType::OBJECT:
                    oss << generate_object(value.as_object(), indent_level);
                    break;
            }
            
            return oss.str();
        }
        
    private:
        JsonStringifyOptions options_;
        
        std::string indent(int level) const {
            if (!options_.pretty) return "";
            return std::string(level * options_.indent, ' ');
        }
        
        std::string newline() const {
            return options_.pretty ? "\n" : "";
        }
        
        std::string space() const {
            return options_.pretty ? " " : "";
        }
        
        std::string generate_array(const JsonArray& arr, int indent_level) {
            if (arr.empty()) {
                return "[]";
            }
            
            std::ostringstream oss;
            oss << "[";
            
            if (options_.pretty) {
                oss << newline();
            }
            
            for (size_t i = 0; i < arr.size(); i++) {
                if (options_.pretty) {
                    oss << indent(indent_level + 1);
                }
                oss << generate(arr[i], indent_level + 1);
                if (i < arr.size() - 1) {
                    oss << ",";
                }
                if (options_.pretty) {
                    oss << newline();
                }
            }
            
            if (options_.pretty) {
                oss << indent(indent_level);
            }
            oss << "]";
            
            return oss.str();
        }
        
        std::string generate_object(const JsonObject& obj, int indent_level) {
            if (obj.empty()) {
                return "{}";
            }
            
            std::ostringstream oss;
            oss << "{";
            
            if (options_.pretty) {
                oss << newline();
            }
            
            auto keys = obj.keys();
            if (options_.sort_keys) {
                std::sort(keys.begin(), keys.end());
            }
            
            for (size_t i = 0; i < keys.size(); i++) {
                if (options_.pretty) {
                    oss << indent(indent_level + 1);
                }
                oss << '"' << json_escape(keys[i]) << '"' << ":" << space();
                oss << generate(obj[keys[i]], indent_level + 1);
                if (i < keys.size() - 1) {
                    oss << ",";
                }
                if (options_.pretty) {
                    oss << newline();
                }
            }
            
            if (options_.pretty) {
                oss << indent(indent_level);
            }
            oss << "}";
            
            return oss.str();
        }
    };
}

std::string JsonGenerator::stringify(const JsonValue& value, const JsonStringifyOptions& options) {
    Generator gen(options);
    return gen.generate(value);
}

std::string JsonGenerator::pretty(const JsonValue& value, int indent) {
    JsonStringifyOptions options;
    options.pretty = true;
    options.indent = indent;
    return stringify(value, options);
}

std::string JsonGenerator::minify(const JsonValue& value) {
    JsonStringifyOptions options;
    options.pretty = false;
    options.compact = true;
    return stringify(value, options);
}

// ============================================================================
// JSONPath - مسار JSON
// ============================================================================

std::vector<JsonValue> JsonPath::query(const JsonValue& root, const std::string& path) {
    // Simple JSONPath implementation
    std::vector<JsonValue> results;
    
    if (path.empty() || path == "$" || path == ".") {
        results.push_back(root);
        return results;
    }
    
    // Split path by dots
    std::vector<std::string> parts;
    std::string current;
    for (char c : path) {
        if (c == '.' || c == '[') {
            if (!current.empty()) {
                parts.push_back(current);
                current.clear();
            }
        } else if (c == ']') {
            if (!current.empty()) {
                parts.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        parts.push_back(current);
    }
    
    // Traverse path
    std::vector<JsonValue> current_values = {root};
    for (const auto& part : parts) {
        if (part == "$") continue;
        
        std::vector<JsonValue> next_values;
        for (const auto& val : current_values) {
            if (val.is_object() && val.as_object().has(part)) {
                next_values.push_back(val.as_object().get(part));
            } else if (val.is_array()) {
                // Try as array index
                try {
                    size_t index = std::stoull(part);
                    if (index < val.as_array().size()) {
                        next_values.push_back(val.as_array()[index]);
                    }
                } catch (...) {
                    // Not a valid index
                }
            }
        }
        current_values = next_values;
    }
    
    return current_values;
}

bool JsonPath::exists(const JsonValue& root, const std::string& path) {
    return !query(root, path).empty();
}

JsonValue JsonPath::get(const JsonValue& root, const std::string& path) {
    auto results = query(root, path);
    if (results.empty()) {
        throw JsonException("Path not found / المسار غير موجود: " + path);
    }
    return results[0];
}

void JsonPath::set(JsonValue& root, const std::string& path, const JsonValue& value) {
    // Simple implementation for direct paths
    if (path.empty() || path == "$") {
        root = value;
        return;
    }
    
    // Split path
    std::vector<std::string> parts;
    std::string current;
    for (char c : path) {
        if (c == '.' || c == '[') {
            if (!current.empty()) {
                parts.push_back(current);
                current.clear();
            }
        } else if (c == ']') {
            if (!current.empty()) {
                parts.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        parts.push_back(current);
    }
    
    // Navigate to parent and set value
    JsonValue* current_val = &root;
    for (size_t i = 0; i < parts.size() - 1; i++) {
        const auto& part = parts[i];
        if (part == "$") continue;
        
        if (current_val->is_object()) {
            current_val = &current_val->as_object().get(part);
        } else if (current_val->is_array()) {
            size_t index = std::stoull(part);
            current_val = &current_val->as_array()[index];
        }
    }
    
    const auto& last_part = parts.back();
    if (last_part != "$") {
        if (current_val->is_object()) {
            current_val->as_object().set(last_part, value);
        } else if (current_val->is_array()) {
            size_t index = std::stoull(last_part);
            current_val->as_array()[index] = value;
        }
    }
}

// ============================================================================
// Utility Functions - الدوال المساعدة
// ============================================================================

std::string json_escape(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (c < 32) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                } else {
                    oss << c;
                }
        }
    }
    return oss.str();
}

std::string json_unescape(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case '"': result += '"'; i++; break;
                case '\\': result += '\\'; i++; break;
                case '/': result += '/'; i++; break;
                case 'b': result += '\b'; i++; break;
                case 'f': result += '\f'; i++; break;
                case 'n': result += '\n'; i++; break;
                case 'r': result += '\r'; i++; break;
                case 't': result += '\t'; i++; break;
                default: result += str[i];
            }
        } else {
            result += str[i];
        }
    }
    return result;
}

std::string json_type_to_string(JsonType type) {
    switch (type) {
        case JsonType::NULL_TYPE: return "null";
        case JsonType::BOOLEAN: return "boolean";
        case JsonType::NUMBER: return "number";
        case JsonType::STRING: return "string";
        case JsonType::ARRAY: return "array";
        case JsonType::OBJECT: return "object";
        default: return "unknown";
    }
}

JsonValue json_merge(const JsonValue& obj1, const JsonValue& obj2) {
    if (!obj1.is_object() || !obj2.is_object()) {
        return obj2;
    }
    
    JsonObject result = obj1.as_object();
    for (const auto& key : obj2.as_object().keys()) {
        const JsonValue& val = obj2.as_object().get(key);
        if (result.has(key) && result.get(key).is_object() && val.is_object()) {
            result.set(key, json_merge(result.get(key), val));
        } else {
            result.set(key, val);
        }
    }
    
    return JsonValue(result);
}

JsonValue json_clone(const JsonValue& value) {
    // Copy constructor handles deep clone
    return value;
}

bool json_equals(const JsonValue& v1, const JsonValue& v2) {
    if (v1.type() != v2.type()) {
        return false;
    }
    
    switch (v1.type()) {
        case JsonType::NULL_TYPE:
            return true;
        case JsonType::BOOLEAN:
            return v1.as_boolean() == v2.as_boolean();
        case JsonType::NUMBER:
            return v1.as_number() == v2.as_number();
        case JsonType::STRING:
            return v1.as_string() == v2.as_string();
        case JsonType::ARRAY: {
            const auto& arr1 = v1.as_array();
            const auto& arr2 = v2.as_array();
            if (arr1.size() != arr2.size()) return false;
            for (size_t i = 0; i < arr1.size(); i++) {
                if (!json_equals(arr1[i], arr2[i])) return false;
            }
            return true;
        }
        case JsonType::OBJECT: {
            const auto& obj1 = v1.as_object();
            const auto& obj2 = v2.as_object();
            if (obj1.size() != obj2.size()) return false;
            for (const auto& key : obj1.keys()) {
                if (!obj2.has(key)) return false;
                if (!json_equals(obj1.get(key), obj2.get(key))) return false;
            }
            return true;
        }
    }
    
    return false;
}

} // namespace json
} // namespace stdlib
} // namespace sad

// الحمد لله رب العالمين
