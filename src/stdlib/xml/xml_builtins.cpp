// بسم الله الرحمن الرحيم
/**
 * @file xml_builtins.cpp
 * @brief XML Builtin Functions - الدوال المدمجة لـ XML
 * 
 * This file implements bilingual builtin functions for XML operations.
 * يوفر هذا الملف دوال مدمجة ثنائية اللغة لعمليات XML
 * 
 * الحمد لله رب العالمين
 */

#include "stdlib/xml/xml_module.h"
#include "data/types/value.h"
#include <memory>
#include <stdexcept>

namespace sad {
namespace stdlib {
namespace xml {

using namespace Sad::Data;

// ============================================================================
// Helper Functions - الدوال المساعدة
// ============================================================================

namespace {
    // Convert XmlElement to Sad Value (as object)
    Value xml_element_to_value(const XmlElement& element) {
        std::map<std::string, Value> obj;
        
        // Add name
        obj["name"] = Value(element.name());
        
        // Add text content
        if (!element.text().empty()) {
            obj["text"] = Value(element.text());
        }
        
        // Add attributes
        if (element.attribute_count() > 0) {
            std::map<std::string, Value> attrs;
            for (const auto& attr : element.attributes()) {
                attrs[attr.name()] = Value(attr.value());
            }
            obj["attributes"] = Value(attrs);
        }
        
        // Add children
        if (element.child_count() > 0) {
            std::vector<Value> children;
            for (const auto& child : element.children()) {
                children.push_back(xml_element_to_value(child));
            }
            obj["children"] = Value(children);
        }
        
        return Value(obj);
    }
    
    // Convert Sad Value to XmlElement
    XmlElement value_to_xml_element(const Value& value) {
        if (!value.is_object()) {
            throw std::runtime_error("Value must be an object / القيمة يجب أن تكون كائناً");
        }
        
        const auto& obj = value.as_object();
        
        if (obj.count("name") == 0) {
            throw std::runtime_error("Element must have a name / العنصر يجب أن يحتوي على اسم");
        }
        
        std::string name = obj.at("name").as_string();
        XmlElement element(name);
        
        // Add text content
        if (obj.count("text") > 0) {
            element.set_text(obj.at("text").as_string());
        }
        
        // Add attributes
        if (obj.count("attributes") > 0 && obj.at("attributes").is_object()) {
            for (const auto& [key, val] : obj.at("attributes").as_object()) {
                element.set_attribute(key, val.as_string());
            }
        }
        
        // Add children
        if (obj.count("children") > 0 && obj.at("children").is_array()) {
            for (const auto& child_val : obj.at("children").as_array()) {
                XmlElement child = value_to_xml_element(child_val);
                element.append_child(std::move(child));
            }
        }
        
        return element;
    }
}

// ============================================================================
// Builtin Functions - الدوال المدمجة
// ============================================================================

/**
 * @brief Parse XML string / حلل نص XML
 * @english xml_parse
 * @arabic حلل_xml
 */
Value builtin_xml_parse(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("xml_parse requires a string argument / حلل_xml يتطلب معامل نصي");
    }
    
    std::string xml_string = args[0].as_string();
    
    XmlParseOptions options;
    if (args.size() > 1 && args[1].is_object()) {
        const auto& opts = args[1].as_object();
        if (opts.count("preserve_whitespace")) {
            options.preserve_whitespace = opts.at("preserve_whitespace").as_boolean();
        }
        if (opts.count("ignore_comments")) {
            options.ignore_comments = opts.at("ignore_comments").as_boolean();
        }
    }
    
    try {
        XmlDocument doc = XmlDocument::parse(xml_string, options);
        return xml_element_to_value(doc.root());
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("XML parse error / خطأ في تحليل XML: ") + e.what());
    }
}

/**
 * @brief Convert value to XML string / حول قيمة إلى نص XML
 * @english xml_stringify
 * @arabic حول_لـxml
 */
Value builtin_xml_stringify(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("xml_stringify requires a value argument / حول_لـxml يتطلب معامل قيمة");
    }
    
    XmlElement element = value_to_xml_element(args[0]);
    XmlDocument doc;
    doc.set_root(element);
    
    XmlStringifyOptions options;
    if (args.size() > 1 && args[1].is_object()) {
        const auto& opts = args[1].as_object();
        if (opts.count("pretty")) {
            options.pretty = opts.at("pretty").as_boolean();
        }
        if (opts.count("indent")) {
            options.indent = static_cast<int>(opts.at("indent").as_number());
        }
        if (opts.count("omit_declaration")) {
            options.omit_declaration = opts.at("omit_declaration").as_boolean();
        }
    }
    
    try {
        std::string result = doc.to_string(options);
        return Value(result);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("XML stringify error / خطأ في تحويل XML: ") + e.what());
    }
}

/**
 * @brief Pretty print XML / طباعة XML بشكل منسق
 * @english xml_pretty
 * @arabic xml_منسق
 */
Value builtin_xml_pretty(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("xml_pretty requires a value argument / xml_منسق يتطلب معامل قيمة");
    }
    
    XmlElement element = value_to_xml_element(args[0]);
    XmlDocument doc;
    doc.set_root(element);
    
    int indent = 2;
    if (args.size() > 1) {
        indent = static_cast<int>(args[1].as_number());
    }
    
    try {
        std::string result = XmlGenerator::pretty(doc, indent);
        return Value(result);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("XML pretty error / خطأ في تنسيق XML: ") + e.what());
    }
}

/**
 * @brief Minify XML / ضغط XML
 * @english xml_minify
 * @arabic xml_مصغر
 */
Value builtin_xml_minify(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("xml_minify requires a value argument / xml_مصغر يتطلب معامل قيمة");
    }
    
    XmlElement element = value_to_xml_element(args[0]);
    XmlDocument doc;
    doc.set_root(element);
    
    try {
        std::string result = XmlGenerator::minify(doc);
        return Value(result);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("XML minify error / خطأ في ضغط XML: ") + e.what());
    }
}

/**
 * @brief Validate XML syntax / التحقق من صحة صيغة XML
 * @english xml_validate
 * @arabic تحقق_من_xml
 */
Value builtin_xml_validate(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("xml_validate requires a string argument / تحقق_من_xml يتطلب معامل نصي");
    }
    
    std::string xml_string = args[0].as_string();
    bool is_valid = XmlParser::validate(xml_string);
    
    return Value(is_valid);
}

/**
 * @brief Create XML element / أنشئ عنصر XML
 * @english xml_element
 * @arabic عنصر_xml
 */
Value builtin_xml_element(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("xml_element requires a name argument / عنصر_xml يتطلب معامل اسم");
    }
    
    std::string name = args[0].as_string();
    XmlElement element(name);
    
    // Optional text content
    if (args.size() > 1) {
        element.set_text(args[1].as_string());
    }
    
    // Optional attributes
    if (args.size() > 2 && args[2].is_object()) {
        for (const auto& [key, val] : args[2].as_object()) {
            element.set_attribute(key, val.as_string());
        }
    }
    
    return xml_element_to_value(element);
}

/**
 * @brief Get element attribute / احصل على سمة العنصر
 * @english xml_get_attribute
 * @arabic احصل_على_سمة_xml
 */
Value builtin_xml_get_attribute(const std::vector<Value>& args) {
    if (args.size() < 2) {
        throw std::runtime_error("xml_get_attribute requires element and attribute name / احصل_على_سمة_xml يتطلب عنصر واسم سمة");
    }
    
    XmlElement element = value_to_xml_element(args[0]);
    std::string attr_name = args[1].as_string();
    
    if (!element.has_attribute(attr_name)) {
        if (args.size() > 2) {
            return args[2]; // Default value
        }
        return Value::null();
    }
    
    std::string value = element.get_attribute(attr_name);
    return Value(value);
}

/**
 * @brief Set element attribute / عين سمة العنصر
 * @english xml_set_attribute
 * @arabic عين_سمة_xml
 */
Value builtin_xml_set_attribute(const std::vector<Value>& args) {
    if (args.size() < 3) {
        throw std::runtime_error("xml_set_attribute requires element, name, and value / عين_سمة_xml يتطلب عنصر واسم وقيمة");
    }
    
    XmlElement element = value_to_xml_element(args[0]);
    std::string attr_name = args[1].as_string();
    std::string attr_value = args[2].as_string();
    
    element.set_attribute(attr_name, attr_value);
    
    return xml_element_to_value(element);
}

/**
 * @brief Get element children / احصل على أبناء العنصر
 * @english xml_children
 * @arabic ابناء_xml
 */
Value builtin_xml_children(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("xml_children requires an element argument / ابناء_xml يتطلب معامل عنصر");
    }
    
    XmlElement element = value_to_xml_element(args[0]);
    
    std::vector<Value> children;
    
    if (args.size() > 1) {
        // Filter by name
        std::string name = args[1].as_string();
        for (const auto& child : element.children(name)) {
            children.push_back(xml_element_to_value(child));
        }
    } else {
        // All children
        for (const auto& child : element.children()) {
            children.push_back(xml_element_to_value(child));
        }
    }
    
    return Value(children);
}

/**
 * @brief Get first child / احصل على أول ابن
 * @english xml_first_child
 * @arabic اول_ابن_xml
 */
Value builtin_xml_first_child(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("xml_first_child requires an element argument / اول_ابن_xml يتطلب معامل عنصر");
    }
    
    XmlElement element = value_to_xml_element(args[0]);
    
    std::optional<XmlElement> child;
    
    if (args.size() > 1) {
        // Filter by name
        std::string name = args[1].as_string();
        child = element.first_child(name);
    } else {
        child = element.first_child();
    }
    
    if (child.has_value()) {
        return xml_element_to_value(child.value());
    }
    
    return Value::null();
}

/**
 * @brief Append child element / أضف عنصر ابن
 * @english xml_append_child
 * @arabic اضف_ابن_xml
 */
Value builtin_xml_append_child(const std::vector<Value>& args) {
    if (args.size() < 2) {
        throw std::runtime_error("xml_append_child requires parent and child / اضف_ابن_xml يتطلب أب وابن");
    }
    
    XmlElement parent = value_to_xml_element(args[0]);
    XmlElement child = value_to_xml_element(args[1]);
    
    parent.append_child(std::move(child));
    
    return xml_element_to_value(parent);
}

/**
 * @brief Get element text / احصل على نص العنصر
 * @english xml_text
 * @arabic نص_xml
 */
Value builtin_xml_text(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("xml_text requires an element argument / نص_xml يتطلب معامل عنصر");
    }
    
    XmlElement element = value_to_xml_element(args[0]);
    std::string text = element.text();
    
    return Value(text);
}

/**
 * @brief Set element text / عين نص العنصر
 * @english xml_set_text
 * @arabic عين_نص_xml
 */
Value builtin_xml_set_text(const std::vector<Value>& args) {
    if (args.size() < 2) {
        throw std::runtime_error("xml_set_text requires element and text / عين_نص_xml يتطلب عنصر ونص");
    }
    
    XmlElement element = value_to_xml_element(args[0]);
    std::string text = args[1].as_string();
    
    element.set_text(text);
    
    return xml_element_to_value(element);
}

/**
 * @brief Query with XPath / استعلام بمسار XPath
 * @english xml_query
 * @arabic استعلم_xml
 */
Value builtin_xml_query(const std::vector<Value>& args) {
    if (args.size() < 2) {
        throw std::runtime_error("xml_query requires element and path / استعلم_xml يتطلب عنصر ومسار");
    }
    
    XmlElement element = value_to_xml_element(args[0]);
    std::string path = args[1].as_string();
    
    XmlDocument doc;
    doc.set_root(element);
    
    try {
        std::vector<XmlElement> results = XPath::query(doc, path);
        std::vector<Value> result_array;
        for (const auto& result : results) {
            result_array.push_back(xml_element_to_value(result));
        }
        return Value(result_array);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("XPath query error / خطأ في استعلام XPath: ") + e.what());
    }
}

/**
 * @brief Escape string for XML / هروب نص لـ XML
 * @english xml_escape
 * @arabic اهرب_xml
 */
Value builtin_xml_escape(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("xml_escape requires a string argument / اهرب_xml يتطلب معامل نصي");
    }
    
    std::string str = args[0].as_string();
    std::string escaped = xml_escape(str);
    
    return Value(escaped);
}

/**
 * @brief Unescape XML string / إلغاء هروب نص XML
 * @english xml_unescape
 * @arabic الغ_هروب_xml
 */
Value builtin_xml_unescape(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("xml_unescape requires a string argument / الغ_هروب_xml يتطلب معامل نصي");
    }
    
    std::string str = args[0].as_string();
    std::string unescaped = xml_unescape(str);
    
    return Value(unescaped);
}

// ============================================================================
// Registration Function - دالة التسجيل
// ============================================================================

/**
 * @brief Register all XML builtin functions
 * تسجيل جميع الدوال المدمجة لـ XML
 */
void register_xml_builtins(runtime::Runtime& runtime) {
    // English names
    runtime.register_builtin("xml_parse", builtin_xml_parse);
    runtime.register_builtin("xml_stringify", builtin_xml_stringify);
    runtime.register_builtin("xml_pretty", builtin_xml_pretty);
    runtime.register_builtin("xml_minify", builtin_xml_minify);
    runtime.register_builtin("xml_validate", builtin_xml_validate);
    runtime.register_builtin("xml_element", builtin_xml_element);
    runtime.register_builtin("xml_get_attribute", builtin_xml_get_attribute);
    runtime.register_builtin("xml_set_attribute", builtin_xml_set_attribute);
    runtime.register_builtin("xml_children", builtin_xml_children);
    runtime.register_builtin("xml_first_child", builtin_xml_first_child);
    runtime.register_builtin("xml_append_child", builtin_xml_append_child);
    runtime.register_builtin("xml_text", builtin_xml_text);
    runtime.register_builtin("xml_set_text", builtin_xml_set_text);
    runtime.register_builtin("xml_query", builtin_xml_query);
    runtime.register_builtin("xml_escape", builtin_xml_escape);
    runtime.register_builtin("xml_unescape", builtin_xml_unescape);
    
    // Arabic names - الأسماء العربية
    runtime.register_builtin("حلل_xml", builtin_xml_parse);
    runtime.register_builtin("حول_لـxml", builtin_xml_stringify);
    runtime.register_builtin("xml_منسق", builtin_xml_pretty);
    runtime.register_builtin("xml_مصغر", builtin_xml_minify);
    runtime.register_builtin("تحقق_من_xml", builtin_xml_validate);
    runtime.register_builtin("عنصر_xml", builtin_xml_element);
    runtime.register_builtin("احصل_على_سمة_xml", builtin_xml_get_attribute);
    runtime.register_builtin("عين_سمة_xml", builtin_xml_set_attribute);
    runtime.register_builtin("ابناء_xml", builtin_xml_children);
    runtime.register_builtin("اول_ابن_xml", builtin_xml_first_child);
    runtime.register_builtin("اضف_ابن_xml", builtin_xml_append_child);
    runtime.register_builtin("نص_xml", builtin_xml_text);
    runtime.register_builtin("عين_نص_xml", builtin_xml_set_text);
    runtime.register_builtin("استعلم_xml", builtin_xml_query);
    runtime.register_builtin("اهرب_xml", builtin_xml_escape);
    runtime.register_builtin("الغ_هروب_xml", builtin_xml_unescape);
}

} // namespace xml
} // namespace stdlib
} // namespace sad

// الحمد لله رب العالمين - All praise is due to Allah, Lord of the worlds
