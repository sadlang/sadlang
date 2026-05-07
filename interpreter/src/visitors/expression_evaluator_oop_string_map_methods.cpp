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
#include "runtime_throw.h"
#include "user_thrown.h"
#include "runtime_throw.h"
#include <algorithm>
#include <cctype>
#include <vector>

#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int
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

            if (m == "الطول" || m == "الحجم" || m == "طول" || m == "حجم")
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
            if (m == "يحتوي")
            {
                if (args.empty())
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                        node.position,
                        {{"function", "يحتوي/contains"}, {"argument", "value"}});
                lastResult_ = Value(str.find(args[0].toString()) != std::string::npos);
                return;
            }
            if (m == "قسّم" || m == "قسم")
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
            if (m == "فارغ" || m == "فارغة")
            {
                lastResult_ = Value(str.empty());
                return;
            }
            if (m == "استبدل")
            {
                if (args.size() < 2)
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                        node.position,
                        {{"function", "استبدل/replace"}, {"argument", "from, to"}});
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
            if (m == "جزء")
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
                int len = args.size() < 2 ? ::Sad::Security::SafeArithmetic::assertSafeCast<int>(chars.size(), "expression_evaluator_oop_string_map_methods_size") - start : args[1].toInt();
                if (start < 0)
                    start = std::max(0, ::Sad::Security::SafeArithmetic::assertSafeCast<int>(chars.size(), "expression_evaluator_oop_string_map_methods_size") + start);
                if (start > ::Sad::Security::SafeArithmetic::assertSafeCast<int>(chars.size(), "expression_evaluator_oop_string_map_methods_size"))
                    start = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(chars.size(), "expression_evaluator_oop_string_map_methods_size");
                if (len < 0)
                    len = 0;
                if (start + len > ::Sad::Security::SafeArithmetic::assertSafeCast<int>(chars.size(), "expression_evaluator_oop_string_map_methods_size"))
                    len = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(chars.size(), "expression_evaluator_oop_string_map_methods_size") - start;
                std::string result;
                for (int ci = start; ci < start + len; ++ci)
                    result += chars[ci];
                lastResult_ = Value(result);
                return;
            }
            if (m == "حرف_عند")
            {
                if (args.empty())
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                        node.position,
                        {{"function", "حرف_عند/charAt"}, {"argument", "index"}});
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
                    idx = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(chars.size(), "expression_evaluator_oop_string_map_methods_size") + idx;
                if (idx < 0 || idx >= ::Sad::Security::SafeArithmetic::assertSafeCast<int>(chars.size(), "expression_evaluator_oop_string_map_methods_size"))
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_STRING_INDEX_OUT_OF_RANGE,
                        node.position,
                        {{"index", std::to_string(args[0].toInt())}, {"length", std::to_string(chars.size())}});
                lastResult_ = Value(chars[idx]);
                return;
            }
            if (m == "يبدأ_بـ" || m == "يبدأ")
            {
                if (args.empty())
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                        node.position,
                        {{"function", "يبدأ_بـ/startsWith"}, {"argument", "prefix"}});
                std::string prefix = args[0].toString();
                lastResult_ = Value(str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix);
                return;
            }
            if (m == "ينتهي_بـ" || m == "ينتهي")
            {
                if (args.empty())
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                        node.position,
                        {{"function", "ينتهي_بـ/endsWith"}, {"argument", "suffix"}});
                std::string suffix = args[0].toString();
                lastResult_ = Value(str.size() >= suffix.size() && str.substr(str.size() - suffix.size()) == suffix);
                return;
            }
            if (m == "قص")
            {
                std::string result = str;
                result.erase(0, result.find_first_not_of(" \t\r\n"));
                result.erase(result.find_last_not_of(" \t\r\n") + 1);
                lastResult_ = Value(result);
                return;
            }
            if (m == "كرر")
            {
                if (args.empty())
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                        node.position,
                        {{"function", "كرر/repeat"}, {"argument", "count"}});
                int count = args[0].toInt();
                std::string result;
                for (int i = 0; i < count; ++i)
                    result += str;
                lastResult_ = Value(result);
                return;
            }
            if (m == "عكس")
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
                for (int ci = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(chars.size(), "expression_evaluator_oop_string_map_methods_size") - 1; ci >= 0; --ci)
                {
                    result += chars[ci];
                }
                lastResult_ = Value(result);
                return;
            }
            if (m == "تحويل_صغير" || m == "لأصغر")
            {
                std::string result = str;
                std::transform(result.begin(), result.end(), result.begin(),
                               [](unsigned char c)
                               { return std::tolower(c); });
                lastResult_ = Value(result);
                return;
            }
            if (m == "تحويل_كبير" || m == "لأكبر")
            {
                std::string result = str;
                std::transform(result.begin(), result.end(), result.begin(),
                               [](unsigned char c)
                               { return std::toupper(c); });
                lastResult_ = Value(result);
                return;
            }
            if (m == "بحث" || m == "جد")
            {
                if (args.empty())
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                        node.position,
                        {{"function", "بحث/find"}, {"argument", "needle"}});
                std::string target = args[0].toString();
                auto pos = str.find(target);
                lastResult_ = Value(pos != std::string::npos ? static_cast<int>(pos) : -1);
                return;
            }

            // (AR) طريقة غير معروفة على النص
            ::Sad::Errors::throwRuntime(
                ::Sad::Errors::ErrorCode::RUN_METHOD_NOT_FOUND,
                node.position,
                {{"method", m}, {"class", "string"}});
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
            if (m == "احصل")
            {
                if (args.empty())
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                        node.position,
                        {{"function", "احصل/get"}, {"argument", "key"}});
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
            if (m == "عيّن" || m == "عين")
            {
                if (args.size() < 2)
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                        node.position,
                        {{"function", "عيّن/set"}, {"argument", "key, value"}});
                std::string key = args[0].toString();
                mapData[key] = args[1];
                Value newMap(mapData);
                writeBackChain(node.object.get(), newMap);
                lastResult_ = newMap;
                return;
            }

            // ─── المفاتيح ───
            if (m == "مفاتيح")
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
            if (m == "القيم" || m == "قيم")
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
            if (m == "الطول" || m == "الحجم" || m == "طول" || m == "حجم")
            {
                lastResult_ = Value(::Sad::Security::SafeArithmetic::assertSafeCast<int>(mapData.size(), "expression_evaluator_oop_string_map_methods_size"));
                return;
            }
            // ─── يحتوي (على مفتاح) ───
            if (m == "يحتوي" || m == "يحتوي_مفتاح")
            {
                if (args.empty())
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                        node.position,
                        {{"function", "يحتوي/has"}, {"argument", "key"}});
                std::string key = args[0].toString();
                lastResult_ = Value(mapData.find(key) != mapData.end());
                return;
            }
            // ─── احذف ───
            if (m == "احذف" || m == "أزل")
            {
                if (args.empty())
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                        node.position,
                        {{"function", "احذف/remove"}, {"argument", "key"}});
                std::string key = args[0].toString();
                mapData.erase(key);
                Value newMap(mapData);
                writeBackChain(node.object.get(), newMap);
                lastResult_ = newMap;
                return;
            }
            // ─── دمج ───
            if (m == "دمج")
            {
                if (args.empty() || !args[0].isMap())
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                        node.position,
                        {{"function", "دمج/merge"}, {"argument", "map"}});
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
            if (m == "امسح" || m == "نظف")
            {
                Value newMap(Value::MapType{});
                writeBackChain(node.object.get(), newMap);
                lastResult_ = newMap;
                return;
            }
            // ─── فارغ / فارغة ───
            if (m == "فارغ" || m == "فارغة")
            {
                lastResult_ = Value(mapData.empty());
                return;
            }
            // ─── نسخ ───
            if (m == "نسخ" || m == "انسخ" || m == "استنسخ")
            {
                Value::MapType copy(mapData.begin(), mapData.end());
                lastResult_ = Value(copy);
                return;
            }
            // ─── عناصر (entries) ───
            if (m == "عناصر")
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

                        if (::Sad::Security::SafeArithmetic::assertSafeCast<int>(args.size(), "expression_evaluator_oop_string_map_methods_size") != expectedCount)
                        {
                            ::Sad::Errors::throwRuntime(
                                ::Sad::Errors::ErrorCode::RUN_TOO_MANY_ARGS,
                                node.position,
                                {{"function", enumName + "." + memberName},
                                 {"expected", std::to_string(expectedCount)},
                                 {"actual", std::to_string(args.size())}});
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
            ::Sad::Errors::throwRuntime(
                ::Sad::Errors::ErrorCode::RUN_METHOD_NOT_FOUND,
                node.position,
                {{"method", m}, {"class", "map"}});
        }

    } // namespace Interpreter
} // namespace Sad
