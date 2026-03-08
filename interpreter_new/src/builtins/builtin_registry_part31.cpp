/**
 * @file builtin_registry_part31.cpp
 * @brief (AR) دوال التسلسل (JSON) ومدير الموارد
 * @brief (EN) Serialization (JSON) & Resource Manager functions
 */

#include "interpreter_core.h"
#include "value.h"
#include "graphics/sad_serialization_resources.h"
#include "json/json_module.h"
#include "xml/xml_module.h"

#include <vector>
#include <memory>
#include <string>

namespace Sad {
namespace Interpreter {

namespace {

using Sad::Data::Value;

// Convert interpreter runtime value -> stdlib JSON value.
sad::stdlib::json::JsonValue sadToJsonValue(const Value& value) {
    if (value.isVoid()) {
        return sad::stdlib::json::JsonValue();
    }
    if (value.isBoolean()) {
        return sad::stdlib::json::JsonValue(value.toBool());
    }
    if (value.isNumeric()) {
        return sad::stdlib::json::JsonValue(value.toDouble());
    }
    if (value.isString()) {
        return sad::stdlib::json::JsonValue(value.toString());
    }
    if (value.isArray()) {
        sad::stdlib::json::JsonArray arr;
        for (const auto& item : value.toArrayRef()) {
            arr.push(sadToJsonValue(item));
        }
        return sad::stdlib::json::JsonValue(std::move(arr));
    }
    if (value.isMap()) {
        sad::stdlib::json::JsonObject obj;
        for (const auto& kv : value.toMapRef()) {
            obj.set(kv.first, sadToJsonValue(kv.second));
        }
        return sad::stdlib::json::JsonValue(std::move(obj));
    }

    // Fallback for unsupported runtime types in JSON encoding.
    return sad::stdlib::json::JsonValue();
}

// Convert stdlib JSON value -> interpreter runtime value.
Value jsonToSadValue(const sad::stdlib::json::JsonValue& jsonVal) {
    using JT = sad::stdlib::json::JsonType;
    switch (jsonVal.type()) {
        case JT::NULL_TYPE:
            return Value();
        case JT::BOOLEAN:
            return Value(jsonVal.as_boolean());
        case JT::NUMBER:
            return Value(jsonVal.as_number());
        case JT::STRING:
            return Value(jsonVal.as_string());
        case JT::ARRAY: {
            Value::ArrayType out;
            const auto& arr = jsonVal.as_array();
            out.reserve(arr.size());
            for (size_t i = 0; i < arr.size(); ++i) {
                out.push_back(jsonToSadValue(arr[i]));
            }
            return Value(std::move(out));
        }
        case JT::OBJECT: {
            Value::MapType out;
            const auto& obj = jsonVal.as_object();
            for (const auto& key : obj.keys()) {
                out[key] = jsonToSadValue(obj.get(key));
            }
            return Value(std::move(out));
        }
    }
    return Value();
}

Value xmlElementToSadValue(const sad::stdlib::xml::XmlElement& element) {
    Value::MapType out;
    out["name"] = Value(element.name());

    if (!element.text().empty()) {
        out["text"] = Value(element.text());
    }

    if (element.attribute_count() > 0) {
        Value::MapType attrs;
        for (const auto& attr : element.attributes()) {
            attrs[attr.name()] = Value(attr.value());
        }
        out["attributes"] = Value(std::move(attrs));
    }

    if (element.child_count() > 0) {
        Value::ArrayType children;
        children.reserve(element.child_count());
        for (const auto& child : element.children()) {
            children.push_back(xmlElementToSadValue(child));
        }
        out["children"] = Value(std::move(children));
    }

    return Value(std::move(out));
}

sad::stdlib::xml::XmlElement sadValueToXmlElement(const Value& value) {
    if (!value.isMap()) {
        throw std::runtime_error("(AR) قيمة XML يجب أن تكون كائن/خريطة. (EN) XML value must be an object/map.");
    }

    const auto& obj = value.toMapRef();
    auto itName = obj.find("name");
    if (itName == obj.end()) {
        throw std::runtime_error("(AR) عنصر XML يحتاج الحقل 'name'. (EN) XML element requires 'name' field.");
    }

    sad::stdlib::xml::XmlElement element(itName->second.toString());

    auto itText = obj.find("text");
    if (itText != obj.end()) {
        element.set_text(itText->second.toString());
    }

    auto itAttrs = obj.find("attributes");
    if (itAttrs != obj.end() && itAttrs->second.isMap()) {
        for (const auto& kv : itAttrs->second.toMapRef()) {
            element.set_attribute(kv.first, kv.second.toString());
        }
    }

    auto itChildren = obj.find("children");
    if (itChildren != obj.end() && itChildren->second.isArray()) {
        for (const auto& childVal : itChildren->second.toArrayRef()) {
            element.append_child(sadValueToXmlElement(childVal));
        }
    }

    return element;
}

} // namespace

void registerBuiltinsPart31(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // =================================================================
    // التسلسل — JSON / Serialization
    // =================================================================

    // 1. json_create / جسن_جديد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::json_create_impl()));
        };
        fm.registerBuiltinFunction("json_create", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", f); // جسن_جديد
    }

    // 2. json_destroy / جسن_تدمير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::json_destroy_impl(id);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("json_destroy", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xaa\xd8\xaf\xd9\x85\xd9\x8a\xd8\xb1", f); // جسن_تدمير
    }

    // 3. json_write_string / جسن_اكتب_نص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string key = args.size() > 1 ? args[1]->toString() : "";
            std::string val = args.size() > 2 ? args[2]->toString() : "";
            sad::stdlib::graphics::json_write_string_impl(id, key, val);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("json_write_string", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xa7\xd9\x83\xd8\xaa\xd8\xa8_\xd9\x86\xd8\xb5", f); // جسن_اكتب_نص
    }

    // 4. json_write_number / جسن_اكتب_رقم
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string key = args.size() > 1 ? args[1]->toString() : "";
            double val = args.size() > 2 ? args[2]->toDouble() : 0.0;
            sad::stdlib::graphics::json_write_number_impl(id, key, val);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("json_write_number", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xa7\xd9\x83\xd8\xaa\xd8\xa8_\xd8\xb1\xd9\x82\xd9\x85", f); // جسن_اكتب_رقم
    }

    // 5. json_write_bool / جسن_اكتب_منطقي
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string key = args.size() > 1 ? args[1]->toString() : "";
            bool val = args.size() > 2 ? args[2]->toBool() : false;
            sad::stdlib::graphics::json_write_bool_impl(id, key, val);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("json_write_bool", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xa7\xd9\x83\xd8\xaa\xd8\xa8_\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a", f); // جسن_اكتب_منطقي
    }

    // 6. json_read_string / جسن_اقرا_نص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string key = args.size() > 1 ? args[1]->toString() : "";
            return std::make_shared<Data::Value>(sad::stdlib::graphics::json_read_string_impl(id, key));
        };
        fm.registerBuiltinFunction("json_read_string", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa7_\xd9\x86\xd8\xb5", f); // جسن_اقرا_نص
    }

    // 7. json_read_number / جسن_اقرا_رقم
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string key = args.size() > 1 ? args[1]->toString() : "";
            return std::make_shared<Data::Value>(sad::stdlib::graphics::json_read_number_impl(id, key));
        };
        fm.registerBuiltinFunction("json_read_number", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa7_\xd8\xb1\xd9\x82\xd9\x85", f); // جسن_اقرا_رقم
    }

    // 8. json_read_bool / جسن_اقرا_منطقي
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string key = args.size() > 1 ? args[1]->toString() : "";
            return std::make_shared<Data::Value>(sad::stdlib::graphics::json_read_bool_impl(id, key));
        };
        fm.registerBuiltinFunction("json_read_bool", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa7_\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a", f); // جسن_اقرا_منطقي
    }

    // 9. json_to_string / جسن_الى_نص
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(sad::stdlib::graphics::json_to_string_impl(id));
        };
        fm.registerBuiltinFunction("json_to_string", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xa7\xd9\x84\xd9\x89_\xd9\x86\xd8\xb5", f); // جسن_الى_نص
    }

    // 10. json_parse / جسن_حلل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            std::string jsonStr = args.size() > 0 ? args[0]->toString() : "{}";
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::json_parse_impl(jsonStr)));
        };
        fm.registerBuiltinFunction("json_parse", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xad\xd9\x84\xd9\x84", f); // جسن_حلل
    }

    // 11. json_has_key / جسن_يحتوي
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string key = args.size() > 1 ? args[1]->toString() : "";
            return std::make_shared<Data::Value>(sad::stdlib::graphics::json_has_key_impl(id, key));
        };
        fm.registerBuiltinFunction("json_has_key", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd9\x8a\xd8\xad\xd8\xaa\xd9\x88\xd9\x8a", f); // جسن_يحتوي
    }

    // 12. json_remove / جسن_احذف
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string key = args.size() > 1 ? args[1]->toString() : "";
            sad::stdlib::graphics::json_remove_impl(id, key);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("json_remove", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xa7\xd8\xad\xd8\xb0\xd9\x81", f); // جسن_احذف
    }

    // 13. json_size / جسن_حجم
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::json_size_impl(id)));
        };
        fm.registerBuiltinFunction("json_size", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xad\xd8\xac\xd9\x85", f); // جسن_حجم
    }

    // 14. json_save_file / جسن_احفظ_ملف
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            std::string path = args.size() > 1 ? args[1]->toString() : "";
            return std::make_shared<Data::Value>(sad::stdlib::graphics::json_save_file_impl(id, path));
        };
        fm.registerBuiltinFunction("json_save_file", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xa7\xd8\xad\xd9\x81\xd8\xb8_\xd9\x85\xd9\x84\xd9\x81", f); // جسن_احفظ_ملف
    }

    // 15. json_load_file / جسن_حمل_ملف
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            std::string path = args.size() > 0 ? args[0]->toString() : "";
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::json_load_file_impl(path)));
        };
        fm.registerBuiltinFunction("json_load_file", f);
        fm.registerBuiltinFunction("\xd8\xac\xd8\xb3\xd9\x86_\xd8\xad\xd9\x85\xd9\x84_\xd9\x85\xd9\x84\xd9\x81", f); // جسن_حمل_ملف
    }

    // =================================================================
    // مدير الموارد / Resource Manager
    // =================================================================

    // =================================================================
    // JSON/XML (stdlib unified entry points) / نقاط دخول موحدة
    // =================================================================

    // json_parse_value / حلل_json
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            if (args.empty()) {
                throw std::runtime_error("(AR) حلل_json يحتاج نص JSON. (EN) json_parse_value requires JSON string.");
            }
            sad::stdlib::json::JsonParseOptions options;
            if (args.size() > 1 && args[1] && args[1]->isMap()) {
                const auto& opts = args[1]->toMapRef();
                auto itComments = opts.find("allow_comments");
                if (itComments != opts.end()) options.allow_comments = itComments->second.toBool();
                auto itTrailing = opts.find("allow_trailing_commas");
                if (itTrailing != opts.end()) options.allow_trailing_commas = itTrailing->second.toBool();
                auto itStrict = opts.find("strict_mode");
                if (itStrict != opts.end()) options.strict_mode = itStrict->second.toBool();
            }
            auto parsed = sad::stdlib::json::JsonValue::parse(args[0]->toString(), options);
            return std::make_shared<Data::Value>(jsonToSadValue(parsed));
        };
        fm.registerBuiltinFunction("json_parse_value", f);
        fm.registerBuiltinFunction("حلل_json", f);
    }

    // json_stringify_value / حول_لـjson
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            if (args.empty()) {
                throw std::runtime_error("(AR) حول_لـjson يحتاج قيمة. (EN) json_stringify_value requires a value.");
            }
            sad::stdlib::json::JsonStringifyOptions options;
            if (args.size() > 1 && args[1] && args[1]->isMap()) {
                const auto& opts = args[1]->toMapRef();
                auto itPretty = opts.find("pretty");
                if (itPretty != opts.end()) options.pretty = itPretty->second.toBool();
                auto itIndent = opts.find("indent");
                if (itIndent != opts.end()) options.indent = itIndent->second.toInt();
                auto itSort = opts.find("sort_keys");
                if (itSort != opts.end()) options.sort_keys = itSort->second.toBool();
            }
            auto asJson = sadToJsonValue(*args[0]);
            return std::make_shared<Data::Value>(asJson.to_string(options));
        };
        fm.registerBuiltinFunction("json_stringify_value", f);
        fm.registerBuiltinFunction("حول_لـjson", f);
    }

    // json_pretty_value / json_منسق
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            if (args.empty()) {
                throw std::runtime_error("(AR) json_منسق يحتاج قيمة. (EN) json_pretty_value requires a value.");
            }
            int indent = (args.size() > 1 && args[1]) ? args[1]->toInt() : 2;
            auto asJson = sadToJsonValue(*args[0]);
            return std::make_shared<Data::Value>(sad::stdlib::json::JsonGenerator::pretty(asJson, indent));
        };
        fm.registerBuiltinFunction("json_pretty_value", f);
        fm.registerBuiltinFunction("json_منسق", f);
    }

    // json_minify_value / json_مصغر
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            if (args.empty()) {
                throw std::runtime_error("(AR) json_مصغر يحتاج قيمة. (EN) json_minify_value requires a value.");
            }
            auto asJson = sadToJsonValue(*args[0]);
            return std::make_shared<Data::Value>(sad::stdlib::json::JsonGenerator::minify(asJson));
        };
        fm.registerBuiltinFunction("json_minify_value", f);
        fm.registerBuiltinFunction("json_مصغر", f);
    }

    // xml_parse / حلل_xml
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            if (args.empty()) {
                throw std::runtime_error("(AR) حلل_xml يحتاج نص XML. (EN) xml_parse requires XML string.");
            }
            sad::stdlib::xml::XmlParseOptions options;
            if (args.size() > 1 && args[1] && args[1]->isMap()) {
                const auto& opts = args[1]->toMapRef();
                auto itWhitespace = opts.find("preserve_whitespace");
                if (itWhitespace != opts.end()) options.preserve_whitespace = itWhitespace->second.toBool();
                auto itComments = opts.find("ignore_comments");
                if (itComments != opts.end()) options.ignore_comments = itComments->second.toBool();
            }
            auto doc = sad::stdlib::xml::XmlDocument::parse(args[0]->toString(), options);
            return std::make_shared<Data::Value>(xmlElementToSadValue(doc.root()));
        };
        fm.registerBuiltinFunction("xml_parse", f);
        fm.registerBuiltinFunction("حلل_xml", f);
    }

    // xml_stringify / حول_لـxml
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            if (args.empty()) {
                throw std::runtime_error("(AR) حول_لـxml يحتاج قيمة. (EN) xml_stringify requires a value.");
            }
            sad::stdlib::xml::XmlStringifyOptions options;
            if (args.size() > 1 && args[1] && args[1]->isMap()) {
                const auto& opts = args[1]->toMapRef();
                auto itPretty = opts.find("pretty");
                if (itPretty != opts.end()) options.pretty = itPretty->second.toBool();
                auto itIndent = opts.find("indent");
                if (itIndent != opts.end()) options.indent = itIndent->second.toInt();
                auto itDecl = opts.find("omit_declaration");
                if (itDecl != opts.end()) options.omit_declaration = itDecl->second.toBool();
            }

            sad::stdlib::xml::XmlDocument doc;
            doc.set_root(sadValueToXmlElement(*args[0]));
            return std::make_shared<Data::Value>(doc.to_string(options));
        };
        fm.registerBuiltinFunction("xml_stringify", f);
        fm.registerBuiltinFunction("حول_لـxml", f);
    }

    // xml_pretty / xml_منسق
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            if (args.empty()) {
                throw std::runtime_error("(AR) xml_منسق يحتاج قيمة. (EN) xml_pretty requires a value.");
            }
            int indent = (args.size() > 1 && args[1]) ? args[1]->toInt() : 2;
            sad::stdlib::xml::XmlDocument doc;
            doc.set_root(sadValueToXmlElement(*args[0]));
            return std::make_shared<Data::Value>(sad::stdlib::xml::XmlGenerator::pretty(doc, indent));
        };
        fm.registerBuiltinFunction("xml_pretty", f);
        fm.registerBuiltinFunction("xml_منسق", f);
    }

    // xml_minify / xml_مصغر
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            if (args.empty()) {
                throw std::runtime_error("(AR) xml_مصغر يحتاج قيمة. (EN) xml_minify requires a value.");
            }
            sad::stdlib::xml::XmlDocument doc;
            doc.set_root(sadValueToXmlElement(*args[0]));
            return std::make_shared<Data::Value>(sad::stdlib::xml::XmlGenerator::minify(doc));
        };
        fm.registerBuiltinFunction("xml_minify", f);
        fm.registerBuiltinFunction("xml_مصغر", f);
    }

    // 16. resource_load / مورد_تحميل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            std::string path = args.size() > 0 ? args[0]->toString() : "";
            std::string type = args.size() > 1 ? args[1]->toString() : "generic";
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::resource_load_impl(path, type)));
        };
        fm.registerBuiltinFunction("resource_load", f);
        fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb1\xd8\xaf_\xd8\xaa\xd8\xad\xd9\x85\xd9\x8a\xd9\x84", f); // مورد_تحميل
    }

    // 17. resource_unload / مورد_تفريغ
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            std::string path = args.size() > 0 ? args[0]->toString() : "";
            sad::stdlib::graphics::resource_unload_impl(path);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("resource_unload", f);
        fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb1\xd8\xaf_\xd8\xaa\xd9\x81\xd8\xb1\xd9\x8a\xd8\xba", f); // مورد_تفريغ
    }

    // 18. resource_loaded / مورد_محمل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            std::string path = args.size() > 0 ? args[0]->toString() : "";
            return std::make_shared<Data::Value>(sad::stdlib::graphics::resource_loaded_impl(path));
        };
        fm.registerBuiltinFunction("resource_loaded", f);
        fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb1\xd8\xaf_\xd9\x85\xd8\xad\xd9\x85\xd9\x84", f); // مورد_محمل
    }

    // 19. resource_count / مورد_عدد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::resource_count_impl()));
        };
        fm.registerBuiltinFunction("resource_count", f);
        fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb1\xd8\xaf_\xd8\xb9\xd8\xaf\xd8\xaf", f); // مورد_عدد
    }

    // 20. resource_memory / مورد_ذاكرة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::resource_memory_impl()));
        };
        fm.registerBuiltinFunction("resource_memory", f);
        fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb1\xd8\xaf_\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9", f); // مورد_ذاكرة
    }

    // 21. resource_clear_all / مورد_مسح_الكل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            sad::stdlib::graphics::resource_clear_all_impl();
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("resource_clear_all", f);
        fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb1\xd8\xaf_\xd9\x85\xd8\xb3\xd8\xad_\xd8\xa7\xd9\x84\xd9\x83\xd9\x84", f); // مورد_مسح_الكل
    }

    // 22. resource_cache_ratio / مورد_نسبة_كاش
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(sad::stdlib::graphics::resource_cache_ratio_impl());
        };
        fm.registerBuiltinFunction("resource_cache_ratio", f);
        fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb1\xd8\xaf_\xd9\x86\xd8\xb3\xd8\xa8\xd8\xa9_\xd9\x83\xd8\xa7\xd8\xb4", f); // مورد_نسبة_كاش
    }

    // 23. resource_load_count / مورد_عدد_تحميلات
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(static_cast<int64_t>(sad::stdlib::graphics::resource_load_count_impl()));
        };
        fm.registerBuiltinFunction("resource_load_count", f);
        fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb1\xd8\xaf_\xd8\xb9\xd8\xaf\xd8\xaf_\xd8\xaa\xd8\xad\xd9\x85\xd9\x8a\xd9\x84\xd8\xa7\xd8\xaa", f); // مورد_عدد_تحميلات
    }

    // 24. resource_set_limit / مورد_حد_ذاكرة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int bytes = args.size() > 0 ? args[0]->toInt() : 0;
            sad::stdlib::graphics::resource_set_limit_impl(bytes);
            return std::make_shared<Data::Value>();
        };
        fm.registerBuiltinFunction("resource_set_limit", f);
        fm.registerBuiltinFunction("\xd9\x85\xd9\x88\xd8\xb1\xd8\xaf_\xd8\xad\xd8\xaf_\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9", f); // مورد_حد_ذاكرة
    }

} // registerBuiltinsPart31

} // namespace Interpreter
} // namespace Sad
