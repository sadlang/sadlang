/**
 * @file expression_evaluator_oop_string_map_methods.cpp
 * @brief (AR) معالجة الطرق المدمجة على النصوص والخرائط
 * @brief (EN) Built-in string and map method call handling
 *
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include <string>
#include "expression_evaluator.h"
#include "statement_executor.h"
#include "ast_node.h"
#include "expressions.h"
#include "class_manager.h"
#include "error_manager.h"
#include "exception.h"
#include <algorithm>
#include <cctype>
#include <vector>

namespace Sad
{
    namespace Interpreter
    {

        using namespace Data;
        using namespace AST;
        using namespace Lexer;

        // =========================================================================
        // (AR) معالجة الطرق المدمجة على النصوص
        // (EN) Handle Built-in String Method Calls
        // =========================================================================

        void ExpressionEvaluator::handleStringMethodCall(MethodCallExpr &node, const Value &objectValue)
        {
            std::vector<Value> args;
            for (auto &arg : node.arguments)
            {
                arg->accept(*this);
                args.push_back(lastResult_);
            }

            std::string str = objectValue.toString();
            const std::string &m = node.methodName;

            if (m == "الطول" || m == "الحجم" || m == "طول" || m == "حجم" || m == "length" || m == "size")
            {
                // (AR) نحسب عدد أحرف UTF-8 (وليس البايتات)
                // (EN) Count UTF-8 characters, not bytes
                int charCount = 0;
                for (size_t i = 0; i < str.size();)
                {
                    unsigned char c = static_cast<unsigned char>(str[i]);
                    if (c >= 0xF0)
                        i += 4;
                    else if (c >= 0xE0)
                        i += 3;
                    else if (c >= 0xC0)
                        i += 2;
                    else
                        i += 1;
                    ++charCount;
                }
                lastResult_ = Value(charCount);
                return;
            }
            if (m == "يحتوي" || m == "contains" || m == "includes")
            {
                if (args.empty())
                    throw RuntimeError("(AR) يحتوي() يتطلب معاملاً. (EN) contains() requires argument.", node.position);
                lastResult_ = Value(str.find(args[0].toString()) != std::string::npos);
                return;
            }
            if (m == "قسّم" || m == "قسم" || m == "split")
            {
                std::string sep = args.empty() ? " " : args[0].toString();
                Value::ArrayType parts;
                size_t pos = 0, found;
                while ((found = str.find(sep, pos)) != std::string::npos)
                {
                    parts.push_back(Value(str.substr(pos, found - pos)));
                    pos = found + sep.size();
                }
                parts.push_back(Value(str.substr(pos)));
                lastResult_ = Value(parts);
                return;
            }
            if (m == "فارغ" || m == "فارغة" || m == "empty" || m == "isEmpty")
            {
                lastResult_ = Value(str.empty());
                return;
            }
            if (m == "استبدل" || m == "replace")
            {
                if (args.size() < 2)
                    throw RuntimeError("(AR) استبدل() يتطلب معاملين. (EN) replace() requires 2 arguments.", node.position);
                std::string from = args[0].toString(), to = args[1].toString();
                std::string result = str;
                size_t pos = 0;
                while ((pos = result.find(from, pos)) != std::string::npos)
                {
                    result.replace(pos, from.length(), to);
                    pos += to.length();
                }
                lastResult_ = Value(result);
                return;
            }
            if (m == "جزء" || m == "substr" || m == "substring")
            {
                // (AR) فهرسة بالحروف (UTF-8) وليس بالبايتات
                // (EN) Index by UTF-8 characters, not bytes
                // (AR) نحول النص إلى قائمة حروف UTF-8 أولاً
                std::vector<std::string> chars;
                {
                    size_t i = 0;
                    while (i < str.size())
                    {
                        unsigned char c = static_cast<unsigned char>(str[i]);
                        size_t charLen = 1;
                        if (c >= 0xF0)
                            charLen = 4;
                        else if (c >= 0xE0)
                            charLen = 3;
                        else if (c >= 0xC0)
                            charLen = 2;
                        if (i + charLen > str.size())
                            charLen = 1; // safety
                        chars.push_back(str.substr(i, charLen));
                        i += charLen;
                    }
                }
                int start = args.empty() ? 0 : args[0].toInt();
                int len = args.size() < 2 ? static_cast<int>(chars.size()) - start : args[1].toInt();
                if (start < 0)
                    start = std::max(0, static_cast<int>(chars.size()) + start);
                if (start > static_cast<int>(chars.size()))
                    start = static_cast<int>(chars.size());
                if (len < 0)
                    len = 0;
                if (start + len > static_cast<int>(chars.size()))
                    len = static_cast<int>(chars.size()) - start;
                std::string result;
                for (int ci = start; ci < start + len; ++ci)
                    result += chars[ci];
                lastResult_ = Value(result);
                return;
            }
            if (m == "حرف_عند" || m == "charAt" || m == "at")
            {
                if (args.empty())
                    throw RuntimeError("(AR) حرف_عند() يتطلب فهرساً. (EN) charAt() requires index.", node.position);
                // (AR) فهرسة بالحروف (UTF-8) وليس بالبايتات
                // (EN) Index by UTF-8 characters, not bytes
                std::vector<std::string> chars;
                {
                    size_t i = 0;
                    while (i < str.size())
                    {
                        unsigned char c = static_cast<unsigned char>(str[i]);
                        size_t charLen = 1;
                        if (c >= 0xF0)
                            charLen = 4;
                        else if (c >= 0xE0)
                            charLen = 3;
                        else if (c >= 0xC0)
                            charLen = 2;
                        if (i + charLen > str.size())
                            charLen = 1;
                        chars.push_back(str.substr(i, charLen));
                        i += charLen;
                    }
                }
                int idx = args[0].toInt();
                if (idx < 0)
                    idx = static_cast<int>(chars.size()) + idx;
                if (idx < 0 || idx >= static_cast<int>(chars.size()))
                    throw RuntimeError("(AR) الفهرس خارج النطاق. (EN) Index out of range.", node.position);
                lastResult_ = Value(chars[idx]);
                return;
            }
            if (m == "يبدأ_بـ" || m == "يبدأ" || m == "startsWith")
            {
                if (args.empty())
                    throw RuntimeError("(AR) يبدأ_بـ() يتطلب معاملاً. (EN) startsWith() requires argument.", node.position);
                std::string prefix = args[0].toString();
                lastResult_ = Value(str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix);
                return;
            }
            if (m == "ينتهي_بـ" || m == "ينتهي" || m == "endsWith")
            {
                if (args.empty())
                    throw RuntimeError("(AR) ينتهي_بـ() يتطلب معاملاً. (EN) endsWith() requires argument.", node.position);
                std::string suffix = args[0].toString();
                lastResult_ = Value(str.size() >= suffix.size() && str.substr(str.size() - suffix.size()) == suffix);
                return;
            }
            if (m == "قص" || m == "trim")
            {
                std::string result = str;
                result.erase(0, result.find_first_not_of(" \t\r\n"));
                result.erase(result.find_last_not_of(" \t\r\n") + 1);
                lastResult_ = Value(result);
                return;
            }
            if (m == "كرر" || m == "repeat")
            {
                if (args.empty())
                    throw RuntimeError("(AR) كرر() يتطلب عدداً. (EN) repeat() requires count.", node.position);
                int count = args[0].toInt();
                std::string result;
                for (int i = 0; i < count; ++i)
                    result += str;
                lastResult_ = Value(result);
                return;
            }
            if (m == "عكس" || m == "reverse")
            {
                // (AR) عكس بأحرف UTF-8 (وليس بايتات)
                // (EN) Reverse by UTF-8 codepoints, not bytes
                std::vector<std::string> chars;
                for (size_t i = 0; i < str.size();)
                {
                    unsigned char c = static_cast<unsigned char>(str[i]);
                    size_t charLen = 1;
                    if (c >= 0xF0)
                        charLen = 4;
                    else if (c >= 0xE0)
                        charLen = 3;
                    else if (c >= 0xC0)
                        charLen = 2;
                    if (i + charLen > str.size())
                        charLen = 1;
                    chars.push_back(str.substr(i, charLen));
                    i += charLen;
                }
                std::string result;
                for (int ci = static_cast<int>(chars.size()) - 1; ci >= 0; --ci)
                {
                    result += chars[ci];
                }
                lastResult_ = Value(result);
                return;
            }
            if (m == "تحويل_صغير" || m == "لأصغر" || m == "toLower" || m == "lowercase")
            {
                std::string result = str;
                std::transform(result.begin(), result.end(), result.begin(),
                               [](unsigned char c)
                               { return std::tolower(c); });
                lastResult_ = Value(result);
                return;
            }
            if (m == "تحويل_كبير" || m == "لأكبر" || m == "toUpper" || m == "uppercase")
            {
                std::string result = str;
                std::transform(result.begin(), result.end(), result.begin(),
                               [](unsigned char c)
                               { return std::toupper(c); });
                lastResult_ = Value(result);
                return;
            }
            if (m == "بحث" || m == "جد" || m == "find" || m == "indexOf")
            {
                if (args.empty())
                    throw RuntimeError("(AR) بحث() يتطلب معاملاً. (EN) find() requires argument.", node.position);
                std::string target = args[0].toString();
                auto pos = str.find(target);
                lastResult_ = Value(pos != std::string::npos ? static_cast<int>(pos) : -1);
                return;
            }

            // (AR) طريقة غير معروفة على النص
            throw RuntimeError(
                "(AR) الطريقة '" + m + "' غير موجودة على النص. (EN) Method '" + m + "' not found on string.",
                node.position);
        }

        // =========================================================================
        // (AR) معالجة الطرق المدمجة على الخرائط
        // (EN) Handle Built-in Map Method Calls
        // =========================================================================

        void ExpressionEvaluator::handleMapMethodCall(MethodCallExpr &node, const Value &objectValue)
        {
            auto mapData = objectValue.toMap();
            // (AR) ملاحظة: الخرائط التي تحتوي على __class__ تم تصفيتها مسبقاً في المرسل الرئيسي
            // (EN) Note: Maps with __class__ are pre-filtered in the main dispatcher

            // (AR) خريطة عادية — معالجة الطرق المدمجة
            std::vector<Value> args;
            for (auto &arg : node.arguments)
            {
                arg->accept(*this);
                args.push_back(lastResult_);
            }
            const std::string &m = node.methodName;

            // ─── احصل (get) — الحصول على قيمة بالمفتاح ───
            if (m == "احصل" || m == "get")
            {
                if (args.empty())
                    throw RuntimeError("(AR) احصل() يتطلب مفتاحاً. (EN) get() requires a key.", node.position);
                std::string key = args[0].toString();
                auto it = mapData.find(key);
                if (it != mapData.end())
                {
                    lastResult_ = it->second;
                }
                else
                {
                    // (AR) إرجاع القيمة الافتراضية إذا مُررت، وإلا لاشيء
                    // (EN) Return default value if provided, otherwise null
                    lastResult_ = args.size() > 1 ? args[1] : Value();
                }
                return;
            }
            // ─── عيّن / عين (set) — تعيين قيمة بالمفتاح ───
            if (m == "عيّن" || m == "عين" || m == "set")
            {
                if (args.size() < 2)
                    throw RuntimeError("(AR) عيّن() يتطلب مفتاحاً وقيمة. (EN) set() requires key and value.", node.position);
                std::string key = args[0].toString();
                mapData[key] = args[1];
                Value newMap(mapData);
                writeBackChain(node.object.get(), newMap);
                lastResult_ = newMap;
                return;
            }

            // ─── المفاتيح ───
            if (m == "مفاتيح" || m == "keys")
            {
                Value::ArrayType keys;
                for (const auto &[k, v] : mapData)
                {
                    keys.push_back(Value(k));
                }
                lastResult_ = Value(keys);
                return;
            }
            // ─── القيم ───
            if (m == "القيم" || m == "قيم" || m == "values")
            {
                Value::ArrayType vals;
                for (const auto &[k, v] : mapData)
                {
                    vals.push_back(v);
                }
                lastResult_ = Value(vals);
                return;
            }
            // ─── الطول / الحجم ───
            if (m == "الطول" || m == "الحجم" || m == "طول" || m == "حجم" || m == "length" || m == "size")
            {
                lastResult_ = Value(static_cast<int>(mapData.size()));
                return;
            }
            // ─── يحتوي (على مفتاح) ───
            if (m == "يحتوي" || m == "يحتوي_مفتاح" || m == "has" || m == "containsKey" || m == "contains")
            {
                if (args.empty())
                    throw RuntimeError("(AR) يحتوي() يتطلب معاملاً. (EN) has() requires argument.", node.position);
                std::string key = args[0].toString();
                lastResult_ = Value(mapData.find(key) != mapData.end());
                return;
            }
            // ─── احذف ───
            if (m == "احذف" || m == "أزل" || m == "remove" || m == "delete")
            {
                if (args.empty())
                    throw RuntimeError("(AR) احذف() يتطلب مفتاحاً. (EN) remove() requires a key.", node.position);
                std::string key = args[0].toString();
                mapData.erase(key);
                Value newMap(mapData);
                writeBackChain(node.object.get(), newMap);
                lastResult_ = newMap;
                return;
            }
            // ─── دمج ───
            if (m == "دمج" || m == "merge")
            {
                if (args.empty() || !args[0].isMap())
                    throw RuntimeError("(AR) دمج() يتطلب خريطة. (EN) merge() requires a map argument.", node.position);
                auto otherMap = args[0].toMap();
                for (const auto &[k, v] : otherMap)
                {
                    mapData[k] = v;
                }
                Value newMap(mapData);
                writeBackChain(node.object.get(), newMap);
                lastResult_ = newMap;
                return;
            }
            // ─── امسح / نظف ───
            if (m == "امسح" || m == "نظف" || m == "clear")
            {
                Value newMap(Value::MapType{});
                writeBackChain(node.object.get(), newMap);
                lastResult_ = newMap;
                return;
            }
            // ─── فارغ / فارغة ───
            if (m == "فارغ" || m == "فارغة" || m == "empty" || m == "isEmpty")
            {
                lastResult_ = Value(mapData.empty());
                return;
            }
            // ─── نسخ ───
            if (m == "نسخ" || m == "انسخ" || m == "استنسخ" || m == "clone" || m == "copy")
            {
                Value::MapType copy(mapData.begin(), mapData.end());
                lastResult_ = Value(copy);
                return;
            }
            // ─── عناصر (entries) ───
            if (m == "عناصر" || m == "entries")
            {
                Value::ArrayType entries;
                for (const auto &[k, v] : mapData)
                {
                    Value::ArrayType pair;
                    pair.push_back(Value(k));
                    pair.push_back(v);
                    entries.push_back(Value(pair));
                }
                lastResult_ = Value(entries);
                return;
            }

            // ─── (AR) دعم فضاء الأسماء: إذا كان العضو دالة مخزنة في الخريطة ───
            // ─── (EN) Namespace support: if member is a function stored in the map ───
            auto memberIt = mapData.find(m);
            if (memberIt != mapData.end())
            {
                // ═══════════════════════════════════════════════════════════
                // (AR) دعم باني نموذج تعداد جبري (ADT variant constructor)
                //      إذا كان العضو خريطة مع __باني_نموذج__، يُنشئ variant
                //      مثال: شكل.دائرة(5) → { __تعداد__: "شكل", __عضو__: "دائرة", نصف_القطر: 5 }
                // (EN) ADT variant constructor support
                //      If member is a map with __باني_نموذج__, creates variant
                // ═══════════════════════════════════════════════════════════
                if (memberIt->second.isMap())
                {
                    auto variantCtorMap = memberIt->second.toMap();
                    auto ctorIt = variantCtorMap.find("__باني_نموذج__");
                    if (ctorIt != variantCtorMap.end() && ctorIt->second.isBoolean() && ctorIt->second.toBool())
                    {
                        std::string enumName = variantCtorMap["__تعداد__"].toString();
                        std::string memberName = variantCtorMap["__عضو__"].toString();
                        auto fieldNamesArr = variantCtorMap["__حقول_أسماء__"].toArray();
                        int expectedCount = variantCtorMap["__عدد_حقول__"].toInt();

                        if (static_cast<int>(args.size()) != expectedCount)
                        {
                            throw RuntimeError(
                                "(AR) باني النموذج '" + enumName + "." + memberName +
                                    "' يتوقع " + std::to_string(expectedCount) +
                                    " وسيطة، لكن تم تمرير " + std::to_string(args.size()) + ".\n"
                                                                                            "(EN) Variant constructor '" +
                                    enumName + "." + memberName +
                                    "' expects " + std::to_string(expectedCount) +
                                    " argument(s), but got " + std::to_string(args.size()) + ".",
                                node.position);
                        }

                        Value::MapType variantMap;
                        variantMap["__تعداد__"] = Value(enumName);
                        variantMap["__عضو__"] = Value(memberName);
                        variantMap["__جبري__"] = Value(true);

                        Value::ArrayType fieldsArray;
                        for (size_t i = 0; i < args.size() && i < fieldNamesArr.size(); ++i)
                        {
                            fieldsArray.push_back(args[i]);
                            variantMap[fieldNamesArr[i].toString()] = args[i];
                        }
                        variantMap["__حقول__"] = Value(fieldsArray);

                        lastResult_ = Value(variantMap);
                        return;
                    }
                }

                if (memberIt->second.isFunction())
                {
                    // (AR) العضو دالة — البحث عنها في FunctionManager واستدعاؤها
                    // (EN) Member is a function — find it in FunctionManager and call it
                    auto funcRef = memberIt->second.toFunction();
                    if (funcRef)
                    {
                        const std::string &fName = funcRef->registeredName;
                        // (AR) بحث مرن مع دعم المعاملات الافتراضية
                        // (EN) Flexible search with default parameter support
                        auto allOverloads = functionManager_.getFunctionOverloads(fName);
                        std::shared_ptr<Data::FunctionDefinition> func = nullptr;
                        for (const auto &candidate : allOverloads)
                        {
                            if (candidate->acceptsArgumentCount(args.size()))
                            {
                                func = candidate;
                                break;
                            }
                        }
                        if (!func)
                            func = functionManager_.getFunction(fName, args.size());

                        if (func)
                        {
                            if (func->hasNativeImplementation())
                            {
                                // (AR) دالة مدمجة — استدعاء مباشر
                                // (EN) Built-in function — direct call
                                std::vector<std::shared_ptr<Data::Value>> valuePtrs;
                                for (const auto &arg : args)
                                {
                                    valuePtrs.push_back(std::make_shared<Data::Value>(arg));
                                }
                                auto result = func->callNative(valuePtrs);
                                lastResult_ = result ? *result : Value();
                            }
                            else if (func->hasBody())
                            {
                                // (AR) دالة مستخدم — إنشاء نطاق وتنفيذ الجسم
                                // (EN) User function — create scope and execute body
                                variableManager_.enterScope(Data::ScopeType::FUNCTION, fName);
                                const auto &params = func->getParameters();
                                for (size_t i = 0; i < params.size() && i < args.size(); ++i)
                                {
                                    variableManager_.define(params[i].name, args[i]);
                                }
                                try
                                {
                                    func->getBody()->accept(statementExecutor_);
                                    if (statementExecutor_.getFlowControl() == FlowControl::RETURN)
                                    {
                                        lastResult_ = statementExecutor_.getReturnValue();
                                        statementExecutor_.resetFlowControl();
                                    }
                                    else
                                    {
                                        lastResult_ = Value();
                                    }
                                }
                                catch (...)
                                {
                                    variableManager_.exitScope();
                                    throw;
                                }
                                variableManager_.exitScope();
                            }
                            return;
                        }
                    }
                }
            }

            // (AR) طريقة غير معروفة على الخريطة
            throw RuntimeError(
                "(AR) الطريقة '" + m + "' غير موجودة على الخريطة. (EN) Method '" + m + "' not found on map.",
                node.position);
        }

    } // namespace Interpreter
} // namespace Sad
