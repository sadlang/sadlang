// بسم الله الرحمن الرحيم
/**
 * @file json_builtins.cpp
 * @brief JSON Builtin Functions - الدوال المدمجة لـ JSON
 * 
 * This file implements bilingual builtin functions for JSON operations.
 * يوفر هذا الملف دوال مدمجة ثنائية اللغة لعمليات JSON
 * 
 * الحمد لله رب العالمين
 */

#include "json/json_module.h"
#include "data/types/value.h"
#include <memory>
#include <stdexcept>

namespace sad {
namespace stdlib {
namespace json {

using namespace Sad::Data;

// ============================================================================
// Helper Functions - الدوال المساعدة
// ============================================================================

namespace {
    // Convert Sad Value to JsonValue
    JsonValue value_to_json(const Value& value) {
        switch (value.get_type()) {
            case ::Sad::Types::SadTypeKind::Void:
                return JsonValue();
            
            case ::Sad::Types::SadTypeKind::Boolean:
                return JsonValue(value.as_boolean());
            
            case ::Sad::Types::SadTypeKind::Float:
            case ::Sad::Types::SadTypeKind::Integer:
                return JsonValue(value.as_double());
            
            case ::Sad::Types::SadTypeKind::String:
                return JsonValue(value.as_string());
            
            case ::Sad::Types::SadTypeKind::Array: {
                JsonArray arr;
                for (const auto& elem : value.as_array()) {
                    arr.push(value_to_json(elem));
                }
                return JsonValue(arr);
            }
            
            case ::Sad::Types::SadTypeKind::Map: {
                JsonObject obj;
                for (const auto& [key, val] : value.as_map()) {
                    obj.set(key, value_to_json(val));
                }
                return JsonValue(obj);
            }
            
            default:
                return JsonValue();
        }
    }
    
    // Convert JsonValue to Sad Value
    Value json_to_value(const JsonValue& json) {
        switch (json.type()) {
            case JsonType::NULL_TYPE:
                return Value();
            
            case JsonType::BOOLEAN:
                return Value(json.as_boolean());
            
            case JsonType::NUMBER:
                return Value(json.as_number());
            
            case JsonType::STRING:
                return Value(json.as_string());
            
            case JsonType::ARRAY: {
                Value::ArrayType arr;
                const auto& json_arr = json.as_array();
                for (size_t i = 0; i < json_arr.size(); i++) {
                    arr.push_back(json_to_value(json_arr[i]));
                }
                return Value(arr);
            }
            
            case JsonType::OBJECT: {
                Value::MapType obj;
                for (const auto& key : json.as_object().keys()) {
                    obj[key] = json_to_value(json.as_object().get(key));
                }
                return Value(obj);
            }
            
            default:
                return Value();
        }
    }
}

// ============================================================================
// Builtin Functions - الدوال المدمجة
// ============================================================================

/**
 * @brief Parse JSON string / حلل نص JSON
 * @english json_parse
 * @arabic حلل_json
 */
Value builtin_json_parse(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("json_parse requires a string argument / حلل_json يتطلب معامل نصي");
    }
    
    std::string json_string = args[0].as_string();
    
    JsonParseOptions options;
    if (args.size() > 1 && args[1].is_object()) {
        const auto& opts = args[1].as_object();
        if (opts.count("allow_comments")) {
            options.allow_comments = opts.at("allow_comments").as_boolean();
        }
        if (opts.count("allow_trailing_commas")) {
            options.allow_trailing_commas = opts.at("allow_trailing_commas").as_boolean();
        }
        if (opts.count("strict_mode")) {
            options.strict_mode = opts.at("strict_mode").as_boolean();
        }
    }
    
    try {
        JsonValue result = JsonValue::parse(json_string, options);
        return json_to_value(result);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("JSON parse error / خطأ في تحليل JSON: ") + e.what());
    }
}

/**
 * @brief Convert value to JSON string / حول قيمة إلى نص JSON
 * @english json_stringify
 * @arabic حول_لـjson
 */
Value builtin_json_stringify(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("json_stringify requires a value argument / حول_لـjson يتطلب معامل قيمة");
    }
    
    JsonValue json_val = value_to_json(args[0]);
    
    JsonStringifyOptions options;
    if (args.size() > 1 && args[1].is_object()) {
        const auto& opts = args[1].as_object();
        if (opts.count("pretty")) {
            options.pretty = opts.at("pretty").as_boolean();
        }
        if (opts.count("indent")) {
            options.indent = static_cast<int>(opts.at("indent").as_number());
        }
        if (opts.count("sort_keys")) {
            options.sort_keys = opts.at("sort_keys").as_boolean();
        }
    }
    
    try {
        std::string result = json_val.to_string(options);
        return Value(result);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("JSON stringify error / خطأ في تحويل JSON: ") + e.what());
    }
}

/**
 * @brief Pretty print JSON / طباعة JSON بشكل منسق
 * @english json_pretty
 * @arabic json_منسق
 */
Value builtin_json_pretty(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("json_pretty requires a value argument / json_منسق يتطلب معامل قيمة");
    }
    
    JsonValue json_val = value_to_json(args[0]);
    int indent = 2;
    
    if (args.size() > 1) {
        indent = static_cast<int>(args[1].as_number());
    }
    
    try {
        std::string result = JsonGenerator::pretty(json_val, indent);
        return Value(result);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("JSON pretty error / خطأ في تنسيق JSON: ") + e.what());
    }
}

/**
 * @brief Minify JSON / ضغط JSON
 * @english json_minify
 * @arabic json_مصغر
 */
Value builtin_json_minify(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("json_minify requires a value argument / json_مصغر يتطلب معامل قيمة");
    }
    
    JsonValue json_val = value_to_json(args[0]);
    
    try {
        std::string result = JsonGenerator::minify(json_val);
        return Value(result);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("JSON minify error / خطأ في ضغط JSON: ") + e.what());
    }
}

/**
 * @brief Validate JSON syntax / التحقق من صحة صيغة JSON
 * @english json_validate
 * @arabic تحقق_من_json
 */
Value builtin_json_validate(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("json_validate requires a string argument / تحقق_من_json يتطلب معامل نصي");
    }
    
    std::string json_string = args[0].as_string();
    bool is_valid = JsonParser::validate(json_string);
    
    return Value(is_valid);
}

/**
 * @brief Get value by JSONPath / احصل على قيمة بمسار JSON
 * @english json_get
 * @arabic احصل_على_json
 */
Value builtin_json_get(const std::vector<Value>& args) {
    if (args.size() < 2) {
        throw std::runtime_error("json_get requires value and path arguments / احصل_على_json يتطلب قيمة ومسار");
    }
    
    JsonValue json_val = value_to_json(args[0]);
    std::string path = args[1].as_string();
    
    try {
        JsonValue result = JsonPath::get(json_val, path);
        return json_to_value(result);
    } catch (const std::exception& e) {
        if (args.size() > 2) {
            // Return default value
            return args[2];
        }
        return Value::null();
    }
}

/**
 * @brief Set value by JSONPath / عين قيمة بمسار JSON
 * @english json_set
 * @arabic عين_json
 */
Value builtin_json_set(const std::vector<Value>& args) {
    if (args.size() < 3) {
        throw std::runtime_error("json_set requires value, path, and new value / عين_json يتطلب قيمة ومسار وقيمة جديدة");
    }
    
    JsonValue json_val = value_to_json(args[0]);
    std::string path = args[1].as_string();
    JsonValue new_val = value_to_json(args[2]);
    
    try {
        JsonPath::set(json_val, path, new_val);
        return json_to_value(json_val);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("JSON set error / خطأ في تعيين JSON: ") + e.what());
    }
}

/**
 * @brief Check if path exists / تحقق من وجود مسار
 * @english json_has
 * @arabic يحتوي_json
 */
Value builtin_json_has(const std::vector<Value>& args) {
    if (args.size() < 2) {
        throw std::runtime_error("json_has requires value and path arguments / يحتوي_json يتطلب قيمة ومسار");
    }
    
    JsonValue json_val = value_to_json(args[0]);
    std::string path = args[1].as_string();
    
    bool exists = JsonPath::exists(json_val, path);
    return Value(exists);
}

/**
 * @brief Query with JSONPath / استعلام بمسار JSON
 * @english json_query
 * @arabic استعلم_json
 */
Value builtin_json_query(const std::vector<Value>& args) {
    if (args.size() < 2) {
        throw std::runtime_error("json_query requires value and path arguments / استعلم_json يتطلب قيمة ومسار");
    }
    
    JsonValue json_val = value_to_json(args[0]);
    std::string path = args[1].as_string();
    
    try {
        std::vector<JsonValue> results = JsonPath::query(json_val, path);
        std::vector<Value> result_array;
        for (const auto& result : results) {
            result_array.push_back(json_to_value(result));
        }
        return Value(result_array);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("JSON query error / خطأ في استعلام JSON: ") + e.what());
    }
}

/**
 * @brief Merge JSON objects / دمج كائنات JSON
 * @english json_merge
 * @arabic ادمج_json
 */
Value builtin_json_merge(const std::vector<Value>& args) {
    if (args.size() < 2) {
        throw std::runtime_error("json_merge requires two objects / ادمج_json يتطلب كائنين");
    }
    
    JsonValue obj1 = value_to_json(args[0]);
    JsonValue obj2 = value_to_json(args[1]);
    
    try {
        JsonValue result = json_merge(obj1, obj2);
        return json_to_value(result);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("JSON merge error / خطأ في دمج JSON: ") + e.what());
    }
}

/**
 * @brief Deep clone JSON value / نسخ عميق لقيمة JSON
 * @english json_clone
 * @arabic انسخ_json
 */
Value builtin_json_clone(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("json_clone requires a value argument / انسخ_json يتطلب معامل قيمة");
    }
    
    JsonValue json_val = value_to_json(args[0]);
    JsonValue cloned = json_clone(json_val);
    
    return json_to_value(cloned);
}

/**
 * @brief Get object keys / احصل على مفاتيح الكائن
 * @english json_keys
 * @arabic مفاتيح_json
 */
Value builtin_json_keys(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("json_keys requires an object argument / مفاتيح_json يتطلب معامل كائن");
    }
    
    JsonValue json_val = value_to_json(args[0]);
    
    if (!json_val.is_object()) {
        throw std::runtime_error("json_keys requires an object / مفاتيح_json يتطلب كائناً");
    }
    
    std::vector<std::string> keys = json_val.as_object().keys();
    std::vector<Value> result;
    for (const auto& key : keys) {
        result.push_back(Value(key));
    }
    
    return Value(result);
}

/**
 * @brief Get object values / احصل على قيم الكائن
 * @english json_values
 * @arabic قيم_json
 */
Value builtin_json_values(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("json_values requires an object argument / قيم_json يتطلب معامل كائن");
    }
    
    JsonValue json_val = value_to_json(args[0]);
    
    if (!json_val.is_object()) {
        throw std::runtime_error("json_values requires an object / قيم_json يتطلب كائناً");
    }
    
    std::vector<Value> result;
    for (const auto& key : json_val.as_object().keys()) {
        result.push_back(json_to_value(json_val.as_object().get(key)));
    }
    
    return Value(result);
}

/**
 * @brief Get array/object length / احصل على طول المصفوفة/الكائن
 * @english json_length
 * @arabic طول_json
 */
Value builtin_json_length(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("json_length requires a value argument / طول_json يتطلب معامل قيمة");
    }
    
    JsonValue json_val = value_to_json(args[0]);
    
    size_t length = 0;
    if (json_val.is_array()) {
        length = json_val.as_array().size();
    } else if (json_val.is_object()) {
        length = json_val.as_object().size();
    }
    
    return Value(static_cast<double>(length));
}

/**
 * @brief Get value type / احصل على نوع القيمة
 * @english json_type
 * @arabic نوع_json
 */
Value builtin_json_type(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("json_type requires a value argument / نوع_json يتطلب معامل قيمة");
    }
    
    JsonValue json_val = value_to_json(args[0]);
    std::string type_str = json_type_to_string(json_val.type());
    
    return Value(type_str);
}

/**
 * @brief Escape string for JSON / هروب نص لـ JSON
 * @english json_escape
 * @arabic اهرب_json
 */
Value builtin_json_escape(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("json_escape requires a string argument / اهرب_json يتطلب معامل نصي");
    }
    
    std::string str = args[0].as_string();
    std::string escaped = json_escape(str);
    
    return Value(escaped);
}

/**
 * @brief Unescape JSON string / إلغاء هروب نص JSON
 * @english json_unescape
 * @arabic الغ_هروب_json
 */
Value builtin_json_unescape(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("json_unescape requires a string argument / الغ_هروب_json يتطلب معامل نصي");
    }
    
    std::string str = args[0].as_string();
    std::string unescaped = json_unescape(str);
    
    return Value(unescaped);
}

/**
 * @brief Compare two JSON values / قارن قيمتي JSON
 * @english json_equals
 * @arabic تساوي_json
 */
Value builtin_json_equals(const std::vector<Value>& args) {
    if (args.size() < 2) {
        throw std::runtime_error("json_equals requires two values / تساوي_json يتطلب قيمتين");
    }
    
    JsonValue val1 = value_to_json(args[0]);
    JsonValue val2 = value_to_json(args[1]);
    
    bool equals = json_equals(val1, val2);
    return Value(equals);
}

// ============================================================================
// Registration Function - دالة التسجيل
// ============================================================================

/**
 * @brief Register all JSON builtin functions
 * تسجيل جميع الدوال المدمجة لـ JSON
 */
void register_json_builtins(runtime::Runtime& runtime) {
    // English names
    runtime.register_builtin("json_parse", builtin_json_parse);
    runtime.register_builtin("json_stringify", builtin_json_stringify);
    runtime.register_builtin("json_pretty", builtin_json_pretty);
    runtime.register_builtin("json_minify", builtin_json_minify);
    runtime.register_builtin("json_validate", builtin_json_validate);
    runtime.register_builtin("json_get", builtin_json_get);
    runtime.register_builtin("json_set", builtin_json_set);
    runtime.register_builtin("json_has", builtin_json_has);
    runtime.register_builtin("json_query", builtin_json_query);
    runtime.register_builtin("json_merge", builtin_json_merge);
    runtime.register_builtin("json_clone", builtin_json_clone);
    runtime.register_builtin("json_keys", builtin_json_keys);
    runtime.register_builtin("json_values", builtin_json_values);
    runtime.register_builtin("json_length", builtin_json_length);
    runtime.register_builtin("json_type", builtin_json_type);
    runtime.register_builtin("json_escape", builtin_json_escape);
    runtime.register_builtin("json_unescape", builtin_json_unescape);
    runtime.register_builtin("json_equals", builtin_json_equals);
    
    // Arabic names - الأسماء العربية
    runtime.register_builtin("حلل_json", builtin_json_parse);
    runtime.register_builtin("حول_لـjson", builtin_json_stringify);
    runtime.register_builtin("json_منسق", builtin_json_pretty);
    runtime.register_builtin("json_مصغر", builtin_json_minify);
    runtime.register_builtin("تحقق_من_json", builtin_json_validate);
    runtime.register_builtin("احصل_على_json", builtin_json_get);
    runtime.register_builtin("عين_json", builtin_json_set);
    runtime.register_builtin("يحتوي_json", builtin_json_has);
    runtime.register_builtin("استعلم_json", builtin_json_query);
    runtime.register_builtin("ادمج_json", builtin_json_merge);
    runtime.register_builtin("انسخ_json", builtin_json_clone);
    runtime.register_builtin("مفاتيح_json", builtin_json_keys);
    runtime.register_builtin("قيم_json", builtin_json_values);
    runtime.register_builtin("طول_json", builtin_json_length);
    runtime.register_builtin("نوع_json", builtin_json_type);
    runtime.register_builtin("اهرب_json", builtin_json_escape);
    runtime.register_builtin("الغ_هروب_json", builtin_json_unescape);
    runtime.register_builtin("تساوي_json", builtin_json_equals);
}

} // namespace json
} // namespace stdlib
} // namespace sad

// الحمد لله رب العالمين - All praise is due to Allah, Lord of the worlds
