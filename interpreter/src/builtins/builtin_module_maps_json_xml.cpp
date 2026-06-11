/**
 * @file builtin_module_maps_json_xml.cpp
 * @brief (AR) وحدة JSON و XML — تحليل وإنتاج نصوص JSON و XML
 * @brief (EN) JSON & XML module — parsing and stringifying JSON and XML
 *
 * @details
 * (AR) الأقسام:
 *   12. JSON (parse / stringify) — محلل JSON كامل (دوال static مستقلة)
 *   12b. XML (parse / stringify) — محلل XML بسيط
 *
 * @note محلل JSON يستخدم دوال static بدلاً من std::function لأداء أفضل
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */
#include "builtins.h"
#include "interpreter_core.h"
#include "builtin_registry.h"
#include <sstream>
#include "builtin_error.h" // (AR) EM-CPP: حامل خطأ الطبقة الأدنى

// (AR) إلغاء ماكرو VOID الخاص بويندوز إن وُجد
#ifdef VOID
#undef VOID
#endif

// (AR) اختصار لفضاء أسماء ثوابت وحدة الخرائط
namespace Bmp = Sad::Builtins::Names::Maps;

namespace Sad
{
    namespace Interpreter
    {

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) دوال مساعدة / (EN) Helper Functions
        // ═══════════════════════════════════════════════════════════════════════

        static std::shared_ptr<Data::Value> makeVal(int v) { return std::make_shared<Data::Value>(v); }
        static std::shared_ptr<Data::Value> makeVal(double v) { return std::make_shared<Data::Value>(v); }
        static std::shared_ptr<Data::Value> makeVal(const std::string &v) { return std::make_shared<Data::Value>(v); }
        static std::shared_ptr<Data::Value> makeVal(bool v) { return std::make_shared<Data::Value>(v); }
        static std::shared_ptr<Data::Value> makeVoidVal() { return std::make_shared<Data::Value>(); }
        static std::shared_ptr<Data::Value> makeArrayVal(const Data::Value::ArrayType &a) { return std::make_shared<Data::Value>(a); }
        static std::shared_ptr<Data::Value> makeMapVal(const Data::Value::MapType &m) { return std::make_shared<Data::Value>(m); }

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) محلل JSON — دوال static مستقلة (بدون std::function overhead)
        // (EN) JSON Parser — independent static functions (no std::function overhead)
        // ═══════════════════════════════════════════════════════════════════════

        // (AR) تخطي المسافات البيضاء في نص JSON
        static void jsonSkipWhitespace(const std::string &json, size_t &pos)
        {
            while (pos < json.size() && std::isspace(json[pos]))
                pos++;
        }

        // (AR) إعلان مسبق — parseValue تستدعي parseObject و parseArray والعكس
        static Data::Value jsonParseValue(const std::string &json, size_t &pos);

        // (AR) تحليل نص JSON مع معالجة تسلسلات الهروب واليونيكود
        static std::string jsonParseString(const std::string &json, size_t &pos)
        {
            if (pos >= json.size() || json[pos] != '"')
                throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
            pos++; // تخطي " الافتتاحية
            std::string result;
            while (pos < json.size() && json[pos] != '"')
            {
                if (json[pos] == '\\' && pos + 1 < json.size())
                {
                    pos++;
                    switch (json[pos])
                    {
                    case '"':
                        result += '"';
                        break;
                    case '\\':
                        result += '\\';
                        break;
                    case '/':
                        result += '/';
                        break;
                    case 'n':
                        result += '\n';
                        break;
                    case 't':
                        result += '\t';
                        break;
                    case 'r':
                        result += '\r';
                        break;
                    case 'b':
                        result += '\b';
                        break;
                    case 'f':
                        result += '\f';
                        break;
                    case 'u':
                    {
                        // (AR) تسلسل هروب يونيكود \uXXXX
                        if (pos + 4 >= json.size())
                            throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                        std::string hex = json.substr(pos + 1, 4);
                        unsigned int codepoint = 0;
                        for (char h : hex)
                        {
                            codepoint <<= 4;
                            if (h >= '0' && h <= '9')
                                codepoint |= (h - '0');
                            else if (h >= 'a' && h <= 'f')
                                codepoint |= (h - 'a' + 10);
                            else if (h >= 'A' && h <= 'F')
                                codepoint |= (h - 'A' + 10);
                            else
                                throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                        }
                        pos += 4; // تخطي 4 أرقام hex
                        // (AR) معالجة أزواج UTF-16 البديلة (surrogate pairs)
                        if (codepoint >= 0xD800 && codepoint <= 0xDBFF)
                        {
                            if (pos + 1 < json.size() && json[pos + 1] == '\\' &&
                                pos + 2 < json.size() && json[pos + 2] == 'u')
                            {
                                pos += 2; // تخطي \ و u
                                std::string hex2 = json.substr(pos + 1, 4);
                                unsigned int low = 0;
                                for (char h : hex2)
                                {
                                    low <<= 4;
                                    if (h >= '0' && h <= '9')
                                        low |= (h - '0');
                                    else if (h >= 'a' && h <= 'f')
                                        low |= (h - 'a' + 10);
                                    else if (h >= 'A' && h <= 'F')
                                        low |= (h - 'A' + 10);
                                    else
                                        throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                                }
                                pos += 4;
                                codepoint = 0x10000 + ((codepoint - 0xD800) << 10) + (low - 0xDC00);
                            }
                        }
                        // (AR) ترميز نقطة الرمز كـ UTF-8
                        if (codepoint <= 0x7F)
                        {
                            result += static_cast<char>(codepoint);
                        }
                        else if (codepoint <= 0x7FF)
                        {
                            result += static_cast<char>(0xC0 | (codepoint >> 6));
                            result += static_cast<char>(0x80 | (codepoint & 0x3F));
                        }
                        else if (codepoint <= 0xFFFF)
                        {
                            result += static_cast<char>(0xE0 | (codepoint >> 12));
                            result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                            result += static_cast<char>(0x80 | (codepoint & 0x3F));
                        }
                        else if (codepoint <= 0x10FFFF)
                        {
                            result += static_cast<char>(0xF0 | (codepoint >> 18));
                            result += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
                            result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                            result += static_cast<char>(0x80 | (codepoint & 0x3F));
                        }
                        break;
                    }
                    default:
                        result += json[pos];
                        break;
                    }
                }
                else
                {
                    result += json[pos];
                }
                pos++;
            }
            if (pos >= json.size())
                throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
            pos++; // تخطي " الختامية
            return result;
        }

        // (AR) تحليل رقم JSON (صحيح أو عشري مع أسية)
        static Data::Value jsonParseNumber(const std::string &json, size_t &pos)
        {
            size_t start = pos;
            if (pos < json.size() && json[pos] == '-')
                pos++;
            while (pos < json.size() && std::isdigit(json[pos]))
                pos++;
            bool isFloat = false;
            if (pos < json.size() && json[pos] == '.')
            {
                isFloat = true;
                pos++;
                while (pos < json.size() && std::isdigit(json[pos]))
                    pos++;
            }
            if (pos < json.size() && (json[pos] == 'e' || json[pos] == 'E'))
            {
                isFloat = true;
                pos++;
                if (pos < json.size() && (json[pos] == '+' || json[pos] == '-'))
                    pos++;
                while (pos < json.size() && std::isdigit(json[pos]))
                    pos++;
            }
            std::string numStr = json.substr(start, pos - start);
            if (isFloat)
                return Data::Value(std::stod(numStr));
            return Data::Value(std::stoi(numStr));
        }

        // (AR) تحليل كائن JSON {مفتاح: قيمة, ...}
        static Data::Value jsonParseObject(const std::string &json, size_t &pos)
        {
            pos++; // تخطي {
            Data::Value::MapType mp;
            jsonSkipWhitespace(json, pos);
            if (pos < json.size() && json[pos] == '}')
            {
                pos++;
                return Data::Value(mp);
            }
            while (true)
            {
                jsonSkipWhitespace(json, pos);
                std::string key = jsonParseString(json, pos);
                jsonSkipWhitespace(json, pos);
                if (pos >= json.size() || json[pos] != ':')
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                pos++; // تخطي :
                jsonSkipWhitespace(json, pos);
                mp[key] = jsonParseValue(json, pos);
                jsonSkipWhitespace(json, pos);
                if (pos < json.size() && json[pos] == ',')
                {
                    pos++;
                    continue;
                }
                if (pos < json.size() && json[pos] == '}')
                {
                    pos++;
                    break;
                }
                throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
            }
            return Data::Value(mp);
        }

        // (AR) تحليل مصفوفة JSON [قيمة, ...]
        static Data::Value jsonParseArray(const std::string &json, size_t &pos)
        {
            pos++; // تخطي [
            Data::Value::ArrayType arr;
            jsonSkipWhitespace(json, pos);
            if (pos < json.size() && json[pos] == ']')
            {
                pos++;
                return Data::Value(arr);
            }
            while (true)
            {
                jsonSkipWhitespace(json, pos);
                arr.push_back(jsonParseValue(json, pos));
                jsonSkipWhitespace(json, pos);
                if (pos < json.size() && json[pos] == ',')
                {
                    pos++;
                    continue;
                }
                if (pos < json.size() && json[pos] == ']')
                {
                    pos++;
                    break;
                }
                throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
            }
            return Data::Value(arr);
        }

        // (AR) تحليل أي قيمة JSON (كائن، مصفوفة، نص، رقم، true/false/null)
        static Data::Value jsonParseValue(const std::string &json, size_t &pos)
        {
            jsonSkipWhitespace(json, pos);
            if (pos >= json.size())
                throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
            char c = json[pos];
            if (c == '{')
                return jsonParseObject(json, pos);
            if (c == '[')
                return jsonParseArray(json, pos);
            if (c == '"')
                return Data::Value(jsonParseString(json, pos));
            if (c == '-' || std::isdigit(c))
                return jsonParseNumber(json, pos);
            if (json.substr(pos, 4) == "true")
            {
                pos += 4;
                return Data::Value(true);
            }
            if (json.substr(pos, 5) == "false")
            {
                pos += 5;
                return Data::Value(false);
            }
            if (json.substr(pos, 4) == "null")
            {
                pos += 4;
                return Data::Value();
            }
            throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
        }

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) تحويل Value إلى JSON — دالة static تكرارية
        // (EN) Value to JSON — recursive static function
        // ═══════════════════════════════════════════════════════════════════════

        // (AR) تحويل قيمة إلى نص JSON (تكرارية لدعم الكائنات والمصفوفات المتداخلة)
        static std::string jsonStringifyValue(const Data::Value &val)
        {
            if (val.isVoid())
                return "null";
            if (val.isInteger())
                return std::to_string(val.toInt());
            if (val.isDouble())
            {
                std::ostringstream oss;
                oss << val.toDouble();
                return oss.str();
            }
            if (val.isBoolean())
                return val.toBool() ? "true" : "false";
            if (val.isString())
            {
                std::string s = val.toString();
                // (AR) تهريب الأحرف الخاصة + ترميز non-ASCII كـ \uXXXX
                std::string escaped = "\"";
                size_t i = 0;
                while (i < s.size())
                {
                    unsigned char ch = static_cast<unsigned char>(s[i]);
                    if (ch == '"')
                    {
                        escaped += "\\\"";
                        i++;
                    }
                    else if (ch == '\\')
                    {
                        escaped += "\\\\";
                        i++;
                    }
                    else if (ch == '\n')
                    {
                        escaped += "\\n";
                        i++;
                    }
                    else if (ch == '\t')
                    {
                        escaped += "\\t";
                        i++;
                    }
                    else if (ch == '\r')
                    {
                        escaped += "\\r";
                        i++;
                    }
                    else if (ch == '\b')
                    {
                        escaped += "\\b";
                        i++;
                    }
                    else if (ch == '\f')
                    {
                        escaped += "\\f";
                        i++;
                    }
                    else if (ch < 0x20)
                    {
                        // (AR) أحرف تحكم → \u00XX
                        char buf[8];
                        std::snprintf(buf, sizeof(buf), "\\u%04X", ch);
                        escaped += buf;
                        i++;
                    }
                    else if (ch <= 0x7F)
                    {
                        // (AR) ASCII مطبوع — كما هو
                        escaped += static_cast<char>(ch);
                        i++;
                    }
                    else
                    {
                        // (AR) UTF-8 متعدد البايتات → فك الترميز إلى نقطة رمز → \uXXXX
                        uint32_t codepoint = 0;
                        int bytes = 0;
                        if ((ch & 0xE0) == 0xC0)
                        {
                            codepoint = ch & 0x1F;
                            bytes = 2;
                        }
                        else if ((ch & 0xF0) == 0xE0)
                        {
                            codepoint = ch & 0x0F;
                            bytes = 3;
                        }
                        else if ((ch & 0xF8) == 0xF0)
                        {
                            codepoint = ch & 0x07;
                            bytes = 4;
                        }
                        else
                        {
                            escaped += static_cast<char>(ch);
                            i++;
                            continue;
                        }

                        bool valid = true;
                        for (int b = 1; b < bytes && (i + b) < s.size(); b++)
                        {
                            unsigned char cont = static_cast<unsigned char>(s[i + b]);
                            if ((cont & 0xC0) != 0x80)
                            {
                                valid = false;
                                break;
                            }
                            codepoint = (codepoint << 6) | (cont & 0x3F);
                        }
                        if (!valid || i + bytes > s.size())
                        {
                            escaped += static_cast<char>(ch);
                            i++;
                            continue;
                        }
                        i += bytes;
                        if (codepoint <= 0xFFFF)
                        {
                            char buf[8];
                            std::snprintf(buf, sizeof(buf), "\\u%04X", codepoint);
                            escaped += buf;
                        }
                        else
                        {
                            // (AR) أزواج بديلة لنقاط الرمز > U+FFFF
                            codepoint -= 0x10000;
                            uint16_t high = 0xD800 + (codepoint >> 10);
                            uint16_t low = 0xDC00 + (codepoint & 0x3FF);
                            char buf[16];
                            std::snprintf(buf, sizeof(buf), "\\u%04X\\u%04X", high, low);
                            escaped += buf;
                        }
                    }
                }
                escaped += "\"";
                return escaped;
            }
            if (val.isArray())
            {
                const auto &arr = val.toArrayRef();
                std::string result = "[";
                for (size_t i = 0; i < arr.size(); i++)
                {
                    if (i > 0)
                        result += ",";
                    result += jsonStringifyValue(arr[i]);
                }
                result += "]";
                return result;
            }
            if (val.isMap())
            {
                const auto &mp = val.toMapRef();
                std::string result = "{";
                bool first = true;
                for (const auto &[k, v] : mp)
                {
                    if (!first)
                        result += ",";
                    result += "\"" + k + "\":" + jsonStringifyValue(v);
                    first = false;
                }
                result += "}";
                return result;
            }
            return "null";
        }

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) محلل XML — دوال static مستقلة (بدون std::function overhead)
        // (EN) XML Parser — independent static functions (no std::function overhead)
        // ═══════════════════════════════════════════════════════════════════════

        // (AR) بنية عقدة XML — تمثل عنصراً XML واحداً مع أبنائه
        struct XmlNode
        {
            std::string name;
            std::string text;
            std::vector<XmlNode> children;
        };

        // (AR) تحليل عنصر XML تكرارياً (يدعم عناصر متداخلة ونص)
        static XmlNode xmlParseElement(const std::string &s, size_t &pos)
        {
            XmlNode node;
            // (AR) تخطي المسافات
            while (pos < s.size() && std::isspace(s[pos]))
                pos++;
            if (pos >= s.size() || s[pos] != '<')
                throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
            pos++; // تخطي <
            // (AR) قراءة اسم العنصر
            size_t nameStart = pos;
            while (pos < s.size() && s[pos] != '>' && s[pos] != ' ' && s[pos] != '/')
                pos++;
            node.name = s.substr(nameStart, pos - nameStart);
            // (AR) تخطي الخواص حتى >
            while (pos < s.size() && s[pos] != '>')
                pos++;
            if (pos >= s.size())
                throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
            pos++; // تخطي >

            // (AR) قراءة المحتوى: نص أو عناصر فرعية
            std::string textContent;
            while (pos < s.size())
            {
                if (s[pos] == '<')
                {
                    if (pos + 1 < s.size() && s[pos + 1] == '/')
                    {
                        // (AR) وسم إغلاق
                        break;
                    }
                    else
                    {
                        // (AR) عنصر فرعي — استدعاء تكراري
                        node.children.push_back(xmlParseElement(s, pos));
                    }
                }
                else
                {
                    textContent += s[pos];
                    pos++;
                }
            }
            node.text = textContent;

            // (AR) تخطي وسم الإغلاق </name>
            if (pos < s.size() && s[pos] == '<')
            {
                std::string closeTag = "</" + node.name + ">";
                if (pos + closeTag.size() <= s.size() &&
                    s.substr(pos, closeTag.size()) == closeTag)
                {
                    pos += closeTag.size();
                }
                else
                {
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                }
            }
            return node;
        }

        // (AR) تحويل XmlNode إلى Value (خريطة) — تكراري لدعم الأبناء
        static std::shared_ptr<Data::Value> xmlNodeToValue(const XmlNode &node)
        {
            auto result = std::make_shared<Data::Value>(Data::Value::MapType{});
            auto &mp = const_cast<Data::Value::MapType &>(result->toMapRef());
            mp["name"] = Data::Value(node.name);
            mp["text"] = Data::Value(node.text);
            Data::Value::ArrayType childArr;
            for (const auto &child : node.children)
            {
                childArr.push_back(*xmlNodeToValue(child));
            }
            mp["children"] = Data::Value(childArr);
            return result;
        }

        // (AR) تحويل Value (خريطة) إلى نص XML — تكراري لدعم الأبناء
        static std::string xmlValueToString(const Data::Value &val)
        {
            if (!val.isMap())
                return val.toString();
            const auto &mp = val.toMapRef();
            std::string name = "element";
            std::string text;
            if (mp.count("name"))
                name = mp.at("name").toString();
            if (mp.count("text"))
                text = mp.at("text").toString();

            std::string result = "<" + name + ">";
            if (mp.count("children"))
            {
                const auto &children = mp.at("children");
                if (children.isArray())
                {
                    for (const auto &child : children.toArrayRef())
                    {
                        result += xmlValueToString(child);
                    }
                }
            }
            result += text;
            result += "</" + name + ">";
            return result;
        }

        // (AR) تحويل Value إلى XML منسق بمسافات بادئة — تكراري
        static std::string xmlValueToPretty(const Data::Value &val, int depth, int indentSize)
        {
            if (!val.isMap())
                return val.toString();
            const auto &mp = val.toMapRef();
            std::string name = "element";
            auto nameIt = mp.find("name");
            if (nameIt != mp.end())
                name = nameIt->second.toString();

            std::string pad(depth * indentSize, ' ');
            std::string result = pad + "<" + name + ">";

            auto textIt = mp.find("text");
            auto childrenIt = mp.find("children");
            bool hasChildren = childrenIt != mp.end() && childrenIt->second.isArray() && !childrenIt->second.toArrayRef().empty();

            if (hasChildren)
            {
                result += "\n";
                const auto &children = childrenIt->second.toArrayRef();
                for (const auto &child : children)
                {
                    result += xmlValueToPretty(child, depth + 1, indentSize) + "\n";
                }
                result += pad + "</" + name + ">";
            }
            else if (textIt != mp.end() && textIt->second.isString())
            {
                result += textIt->second.toString() + "</" + name + ">";
            }
            else
            {
                result += "</" + name + ">";
            }
            return result;
        }

        void registerBuiltinsMapsJsonXml(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // ═══════════════════════════════════════════════════════════════════
            // 12. JSON (parse / stringify)
            // (AR) دوال static مستقلة — بدون std::function أو shared_ptr overhead
            // ═══════════════════════════════════════════════════════════════════

            // (AR) تحليل_جيسون — تحليل نص JSON إلى قيمة (يستدعي الدوال الـ static)
            // (EN) json_parse — parse JSON string to value (calls static functions)
            auto json_parse_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string json = args[0]->toString();
                if (json.empty())
                    return makeVoidVal();
                size_t pos = 0;
                auto result = jsonParseValue(json, pos);
                return std::make_shared<Data::Value>(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::JSON_PARSE), json_parse_fn);

            // (AR) نص_جيسون — تحويل قيمة إلى نص JSON (يستدعي الدالة الـ static)
            // (EN) json_stringify — convert value to JSON string (calls static function)
            auto json_stringify_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    return makeVal(std::string("null"));
                return makeVal(jsonStringifyValue(*args[0]));
            };
            fm.registerBuiltinFunction(std::string(Bmp::JSON_STRINGIFY), json_stringify_fn);

            // (AR) json_منسق — تنسيق JSON بمسافات بادئة
            // (EN) json_pretty — format JSON with indentation
            auto json_pretty_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty())
                    return makeVal(std::string("null"));
                std::string raw = jsonStringifyValue(*args[0]);
                // (AR) تنسيق بسيط: نضيف سطر جديد بعد { و [ و , ومسافات بادئة
                std::string result;
                int indent = 0;
                int indentSize = (args.size() > 1 && args[1]->isInteger()) ? args[1]->toInt() : 2;
                bool inString = false;
                for (size_t i = 0; i < raw.size(); i++)
                {
                    char c = raw[i];
                    if (c == '"' && (i == 0 || raw[i - 1] != '\\'))
                    {
                        inString = !inString;
                        result += c;
                    }
                    else if (!inString)
                    {
                        if (c == '{' || c == '[')
                        {
                            result += c;
                            result += '\n';
                            indent++;
                            result += std::string(indent * indentSize, ' ');
                        }
                        else if (c == '}' || c == ']')
                        {
                            result += '\n';
                            indent--;
                            result += std::string(indent * indentSize, ' ');
                            result += c;
                        }
                        else if (c == ',')
                        {
                            result += c;
                            result += '\n';
                            result += std::string(indent * indentSize, ' ');
                        }
                        else if (c == ':')
                        {
                            result += ": ";
                        }
                        else
                        {
                            result += c;
                        }
                    }
                    else
                    {
                        result += c;
                    }
                }
                return makeVal(result);
            };
            // (AR) json_مصغر — نفس stringify العادي (بدون مسافات)
            // (AR) json_مصغر — اسم عربي بديل لـ json_minify
            // (AR) json_parse_value — اسم بديل لـ json_parse

            // ═══════════════════════════════════════════════════════════════════
            // (AR) تنسيق_جيسون — دالة تنسيق JSON بمسافات بادئة (دالة مستقلة)
            // (EN) JSON pretty-print — separate function (not an alias)
            // ═══════════════════════════════════════════════════════════════════
            fm.registerBuiltinFunction(std::string(Bmp::JSON_PRETTY), json_pretty_fn);

            // ═══════════════════════════════════════════════════════════════════
            // 12b. XML (parse / stringify) — محلل XML بسيط
            // (AR) دوال static مستقلة — بدون std::function overhead
            // ═══════════════════════════════════════════════════════════════════

            // (AR) حلل_xml — تحليل نص XML إلى خريطة (يستدعي الدوال الـ static)
            // (EN) xml_parse — parse XML string to map (calls static functions)
            auto xml_parse_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty() || !args[0]->isString())
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                std::string xml = args[0]->toString();
                size_t pos = 0;
                // (AR) تخطي BOM والمسافات
                while (pos < xml.size() && (std::isspace(xml[pos]) || xml[pos] == '\xef' || xml[pos] == '\xbb' || xml[pos] == '\xbf'))
                    pos++;
                // (AR) تخطي <?xml ...?>
                if (pos + 1 < xml.size() && xml[pos] == '<' && xml[pos + 1] == '?')
                {
                    while (pos < xml.size() && xml[pos] != '>')
                        pos++;
                    if (pos < xml.size())
                        pos++;
                }
                auto root = xmlParseElement(xml, pos);
                return xmlNodeToValue(root);
            };
            fm.registerBuiltinFunction(std::string(Bmp::XML_PARSE), xml_parse_fn);

            // (AR) حول_لـxml — تحويل خريطة إلى نص XML (يستدعي الدالة الـ static)
            // (EN) xml_stringify — convert map to XML string (calls static function)
            auto xml_stringify_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty() || !args[0]->isMap())
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                return makeVal(xmlValueToString(*args[0]));
            };
            fm.registerBuiltinFunction(std::string(Bmp::XML_STRINGIFY), xml_stringify_fn);

            // (AR) xml_منسق — XML بمسافات بادئة (يستدعي الدالة الـ static)
            // (EN) xml_pretty — XML with indentation (calls static function)
            auto xml_pretty_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                if (args.empty() || !args[0]->isMap())
                    throw ::Sad::Errors::BuiltinError(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                int indentSize = (args.size() > 1 && args[1]->isInteger()) ? args[1]->toInt() : 2;
                return makeVal(xmlValueToPretty(*args[0], 0, indentSize));
            };
            // (AR) BF-04: تسجيل xml_pretty_fn — كان معرّفاً وغير مسجل (bug)
            // (EN) BF-04: register xml_pretty_fn — was defined but not registered (bug)
            fm.registerBuiltinFunction(std::string("تنسيق_وسائم"), xml_pretty_fn);

            // ═══════════════════════════════════════════════════════════════════
            // (AR) أسماء بديلة عربية بحتة فقط — كل اسم خلفه دلالة واضحة
            // (EN) Pure Arabic aliases only (no English/mixed legacy)
            // ═══════════════════════════════════════════════════════════════════
            // (AR) JSON aliases
            fm.registerBuiltinFunction(std::string(Bmp::JSON_PARSE_ALT), json_parse_fn);       // = تحليل_جيسون
            fm.registerBuiltinFunction(std::string(Bmp::JSON_STRINGIFY_ALT), json_stringify_fn); // = نص_جيسون
            fm.registerBuiltinFunction(std::string(Bmp::JSON_PRETTY_ALT), json_pretty_fn);     // = تنسيق_جيسون
            fm.registerBuiltinFunction(std::string(Bmp::JSON_MINIFY), json_stringify_fn);    // مصغر = stringify

            // (AR) XML aliases
            fm.registerBuiltinFunction(std::string(Bmp::XML_PARSE_ALT), xml_parse_fn);    // = تحليل_وسائم
            fm.registerBuiltinFunction(std::string(Bmp::XML_PRETTY_ALT), xml_pretty_fn);  // = تنسيق_وسائم
            fm.registerBuiltinFunction(std::string(Bmp::XML_MINIFY), xml_stringify_fn); // مصغر = stringify

        } // registerBuiltinsMapsJsonXml

    } // namespace Interpreter
} // namespace Sad
