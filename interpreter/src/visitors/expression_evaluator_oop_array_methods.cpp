/**
 * @file expression_evaluator_oop_array_methods.cpp
 * @brief (AR) معالجة الطرق المدمجة على المصفوفات — handleArrayMethodCall
 * @brief (EN) Built-in array method call handling — handleArrayMethodCall
 *
 * (AR) يحتوي على جميع الطرق المدمجة للمصفوفات:
 *      الطول، اضف، احذف، ادخل، رتب، شريحة، خريطة، رشح، اختزل،
 *      ازدوج، أعد_تشكيل، ضرب_مصفوفات، وغيرها.
 *
 * (EN) Contains all built-in array methods:
 *      length, push, pop, remove, insert, sort, slice, map, filter, reduce,
 *      zip, reshape, matmul, and more.
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
// (AR) ثوابت أسماء الطرق المُولَّدة من language-truth/type_methods.yaml
// (EN) Generated type method name constants from language-truth/type_methods.yaml
#include "builtin_registry.h"
namespace TM = Sad::Builtins::Names::TypeMethods;
#include <algorithm>
#include <cmath>
#include <functional>
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
        // (AR) معالجة الطرق المدمجة على المصفوفات
        // (EN) Handle Built-in Array Method Calls
        // =========================================================================

        void ExpressionEvaluator::handleArrayMethodCall(MethodCallExpr &node, const Value &objectValue)
        {
            // ═══════════════════════════════════════════════════════════════════
            if (objectValue.isArray())
            {
                // (AR) تقييم جميع المعاملات أولاً
                std::vector<Value> args;
                for (auto &arg : node.arguments)
                {
                    arg->accept(*this);
                    args.push_back(lastResult_);
                }

                Value::ArrayType arr = objectValue.toArray();
                const std::string &m = node.methodName;

                // ─── الطول / الحجم — TM::Array::LENGTH ───
                if (m == TM::Array::LENGTH)
                {
                    lastResult_ = Value(::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_oop_array_methods_size"));
                    return;
                }
                // ─── إضافة عنصر — TM::Array::PUSH ───
                if (m == TM::Array::PUSH)
                {
                    if (args.empty())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                            node.position,
                            {{"function", "اضف/push"}, {"argument", "value"}});
                    for (auto &a : args)
                        arr.push_back(a);
                    Value newArr(arr);
                    writeBackChain(node.object.get(), newArr);
                    lastResult_ = newArr;
                    return;
                }
                // ─── حذف وإرجاع آخر عنصر — TM::Array::POP ───
                if (m == TM::Array::POP)
                {
                    if (arr.empty())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_EMPTY_COLLECTION,
                            node.position,
                            {{"operation", "احذف_اخير/pop"}});
                    Value last = arr.back();
                    arr.pop_back();
                    Value newArr(arr);
                    writeBackChain(node.object.get(), newArr);
                    lastResult_ = last;
                    return;
                }
                // ─── حذف عنصر بالفهرس — TM::Array::DELETE ───
                if (m == TM::Array::DELETE)
                {
                    if (args.empty())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                            node.position,
                            {{"function", "احذف/remove"}, {"argument", "index"}});
                    int idx = args[0].toInt();
                    if (idx < 0)
                        idx = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_oop_array_methods_size") + idx;
                    if (idx < 0 || idx >= ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_oop_array_methods_size"))
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_INDEX_OUT_OF_RANGE,
                            node.position,
                            {{"index", std::to_string(idx)}, {"length", std::to_string(arr.size())}, {"container", "array"}});
                    Value removed = arr[idx];
                    arr.erase(arr.begin() + idx);
                    Value newArr(arr);
                    writeBackChain(node.object.get(), newArr);
                    lastResult_ = removed;
                    return;
                }
                // ─── إدخال عنصر في موقع محدد ───
                if (m == TM::Array::INSERT_ALT)
                {
                    if (args.size() < 2)
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                            node.position,
                            {{"function", "ادخل/insert"}, {"argument", "index, value"}});
                    int idx = args[0].toInt();
                    if (idx < 0)
                        idx = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_oop_array_methods_size") + idx;
                    if (idx < 0)
                        idx = 0;
                    if (idx > ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_oop_array_methods_size"))
                        idx = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_oop_array_methods_size");
                    arr.insert(arr.begin() + idx, args[1]);
                    Value newArr(arr);
                    writeBackChain(node.object.get(), newArr);
                    lastResult_ = newArr;
                    return;
                }
                // ─── أول عنصر — TM::Array::FIRST ───
                if (m == TM::Array::FIRST)
                {
                    if (arr.empty())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_EMPTY_COLLECTION,
                            node.position,
                            {{"operation", "اول/first"}});
                    lastResult_ = arr.front();
                    return;
                }
                // ─── آخر عنصر — TM::Array::LAST ───
                if (m == TM::Array::LAST)
                {
                    if (arr.empty())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_EMPTY_COLLECTION,
                            node.position,
                            {{"operation", "اخر/last"}});
                    lastResult_ = arr.back();
                    return;
                }
                // ─── يحتوي — TM::Array::CONTAINS ───
                if (m == TM::Array::CONTAINS)
                {
                    if (args.empty())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                            node.position,
                            {{"function", "يحتوي/contains"}, {"argument", "value"}});
                    bool found = false;
                    for (auto &el : arr)
                    {
                        if ((el == args[0]).toBool())
                        {
                            found = true;
                            break;
                        }
                    }
                    lastResult_ = Value(found);
                    return;
                }
                // ─── فهرس العنصر — TM::Array::INDEX_OF ───
                if (m == TM::Array::INDEX_OF)
                {
                    if (args.empty())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                            node.position,
                            {{"function", "فهرس/indexOf"}, {"argument", "value"}});
                    for (int i = 0; i < ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_oop_array_methods_size"); ++i)
                    {
                        if ((arr[i] == args[0]).toBool())
                        {
                            lastResult_ = Value(i);
                            return;
                        }
                    }
                    lastResult_ = Value(-1);
                    return;
                }
                // ─── عكس — TM::Array::REVERSE ───
                if (m == TM::Array::REVERSE)
                {
                    std::reverse(arr.begin(), arr.end());
                    Value newArr(arr);
                    writeBackChain(node.object.get(), newArr);
                    lastResult_ = newArr;
                    return;
                }
                // ─── ترتيب — TM::Array::SORT ───
                if (m == TM::Array::SORT)
                {
                    bool ascending = true;
                    if (!args.empty() && args[0].isBoolean())
                        ascending = args[0].toBool();
                    if (!args.empty() && args[0].isString() && (args[0].toString() == "تنازلي" || args[0].toString() == "desc"))
                        ascending = false;
                    auto cmp = [](const Value &a, const Value &b) -> bool
                    {
                        if (a.isNumeric() && b.isNumeric())
                            return a.toDouble() < b.toDouble();
                        return a.toString() < b.toString();
                    };
                    if (ascending)
                        std::sort(arr.begin(), arr.end(), cmp);
                    else
                        std::sort(arr.begin(), arr.end(), [&cmp](const Value &a, const Value &b)
                                  { return cmp(b, a); });
                    Value newArr(arr);
                    writeBackChain(node.object.get(), newArr);
                    lastResult_ = newArr;
                    return;
                }
                // ─── شريحة ───
                // TM::Array::SLICE
                if (m == TM::Array::SLICE)
                {
                    int start = args.empty() ? 0 : args[0].toInt();
                    int end = args.size() < 2 ? ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_oop_array_methods_size") : args[1].toInt();
                    if (start < 0)
                        start = std::max(0, ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_oop_array_methods_size") + start);
                    if (end < 0)
                        end = std::max(0, ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_oop_array_methods_size") + end);
                    if (start > ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_oop_array_methods_size"))
                        start = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_oop_array_methods_size");
                    if (end > ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_oop_array_methods_size"))
                        end = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_oop_array_methods_size");
                    if (start >= end)
                    {
                        lastResult_ = Value(Value::ArrayType{});
                        return;
                    }
                    Value::ArrayType sliced(arr.begin() + start, arr.begin() + end);
                    lastResult_ = Value(sliced);
                    return;
                }
                // ─── فارغة — TM::Array::IS_EMPTY ───
                if (m == TM::Array::IS_EMPTY)
                {
                    lastResult_ = Value(arr.empty());
                    return;
                }
                // ─── مسح ───
                if (m == TM::Array::CLEAR)
                {
                    Value newArr(Value::ArrayType{});
                    writeBackChain(node.object.get(), newArr);
                    lastResult_ = newArr;
                    return;
                }
                // ─── صل / اربط (join) ───
                if (m == TM::Array::JOIN)
                {
                    std::string sep = args.empty() ? "" : args[0].toString();
                    std::string result;
                    for (size_t i = 0; i < arr.size(); ++i)
                    {
                        if (i > 0)
                            result += sep;
                        result += arr[i].toString();
                    }
                    lastResult_ = Value(result);
                    return;
                }
                // ─── نسخ ───
                if (m == TM::Array::COPY_ALT)
                {
                    Value::ArrayType copy(arr.begin(), arr.end());
                    lastResult_ = Value(copy);
                    return;
                }
                // ─── تسطيح (flatten) ───
                // TM::Array::FLATTEN
                if (m == TM::Array::FLATTEN)
                {
                    // (AR) حد العمق الأقصى لمنع التكرار اللانهائي
                    // (EN) Max depth limit to prevent infinite recursion
                    int maxDepth = 100;
                    if (!args.empty() && args[0].isNumeric())
                    {
                        maxDepth = args[0].toInt();
                    }
                    Value::ArrayType flat;
                    std::function<void(const Value::ArrayType &, int)> doFlatten;
                    doFlatten = [&flat, &doFlatten](const Value::ArrayType &a, int depth)
                    {
                        for (auto &el : a)
                        {
                            if (el.isArray() && depth > 0)
                                doFlatten(el.toArrayRef(), depth - 1);
                            else
                                flat.push_back(el);
                        }
                    };
                    doFlatten(arr, maxDepth);
                    lastResult_ = Value(flat);
                    return;
                }
                // ─── فريد (unique) ───
                if (m == TM::Array::UNIQUE)
                {
                    Value::ArrayType unique;
                    for (auto &el : arr)
                    {
                        bool alreadySeen = false;
                        for (auto &u : unique)
                        {
                            if ((u == el).toBool())
                            {
                                alreadySeen = true;
                                break;
                            }
                        }
                        if (!alreadySeen)
                        {
                            unique.push_back(el);
                        }
                    }
                    lastResult_ = Value(unique);
                    return;
                }
                // ─── عدّ (count) ───
                if (m == TM::Array::COUNT_ALT)
                {
                    if (args.empty())
                    {
                        lastResult_ = Value(::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_oop_array_methods_size"));
                        return;
                    }
                    int cnt = 0;
                    for (auto &el : arr)
                    {
                        if ((el == args[0]).toBool())
                            ++cnt;
                    }
                    lastResult_ = Value(cnt);
                    return;
                }

                // ═══════════════════════════════════════════════════════════════
                // (AR) العمليات الدالية (Functional) — تقبل دوال كمعاملات
                // ═══════════════════════════════════════════════════════════════

                // ─── دالة مساعدة لتنفيذ lambda/function على عنصر ───
                // (AR) تبحث عن الدالة بعدد الوسائط الممررة أولاً، ثم بعدد أقل (1) للتوافق مع اللامبدا
                // (EN) Tries exact arg count first, then fewer args (1) for lambda compat
                auto callFunction = [this, &node](const std::string &funcName, const std::vector<Value> &callArgs) -> Value
                {
                    // (AR) البحث عن الدالة — نحاول عدد الوسائط الكامل أولاً
                    auto funcDef = functionManager_.getFunction(funcName, callArgs.size());
                    if (!funcDef)
                    {
                        funcDef = functionManager_.getFunction("__template_" + funcName, callArgs.size());
                    }
                    // (AR) إذا لم نجد بالعدد الكامل، نحاول بوسيط واحد (العنصر فقط)
                    //      هذا يسمح للامبدا ذات معامل واحد بالعمل مع دوال تمرر (عنصر، فهرس)
                    // (EN) If not found with full args, try with 1 arg (element only)
                    //      Allows single-param lambdas to work with (element, index) callers
                    if (!funcDef && callArgs.size() > 1)
                    {
                        funcDef = functionManager_.getFunction(funcName, 1);
                        if (!funcDef)
                        {
                            funcDef = functionManager_.getFunction("__template_" + funcName, 1);
                        }
                    }
                    if (!funcDef)
                    {
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_FUNCTION_NOT_FOUND,
                            node.position,
                            {{"function", funcName}});
                    }

                    variableManager_.enterScope(Data::ScopeType::FUNCTION, funcName);
                    // (AR) حقن المتغيرات الملتقطة (للإغلاقات/لامدا)
                    // (EN) Inject captured variables (for closures/lambda)
                    if (funcDef->hasCaptures())
                    {
                        for (const auto &[capName, capVal] : funcDef->getCaptures())
                        {
                            variableManager_.define(capName, capVal);
                        }
                    }
                    // (AR) ربط المعاملات باستخدام getParameters()
                    const auto &params = funcDef->getParameters();
                    for (size_t i = 0; i < params.size() && i < callArgs.size(); ++i)
                    {
                        variableManager_.define(params[i].name, callArgs[i]);
                    }

                    Value result;
                    try
                    {
                        if (funcDef->hasBody())
                        {
                            // (AR) الحصول على جسم الدالة واستدعاء accept — تحويل آمن مباشر
                            // (EN) Get function body and call accept — direct safe access
                            auto bodyPtr = funcDef->getBody();
                            bodyPtr->accept(statementExecutor_);
                            if (statementExecutor_.getFlowControl() == FlowControl::RETURN)
                            {
                                result = statementExecutor_.getReturnValue();
                                statementExecutor_.resetFlowControl();
                            }
                        }
                    }
                    catch (...)
                    {
                        variableManager_.exitScope();
                        throw;
                    }
                    variableManager_.exitScope();
                    return result;
                };

                // ─── لكل (forEach) — TM::Array::FOR_EACH ───
                if (m == TM::Array::FOR_EACH)
                {
                    if (args.empty() || !args[0].isFunctionOrString())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_FUNCTION,
                            node.position,
                            {{"builtin", "لكل/forEach"}});
                    std::string funcName = args[0].getFunctionName();
                    for (size_t i = 0; i < arr.size(); ++i)
                    {
                        callFunction(funcName, {arr[i], Value(static_cast<int>(i))});
                    }
                    lastResult_ = Value();
                    return;
                }
                // ─── خريطة / map — TM::Array::MAP ───
                if (m == TM::Array::MAP)
                {
                    if (args.empty() || !args[0].isFunctionOrString())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_FUNCTION,
                            node.position,
                            {{"builtin", "خريطة/map"}});
                    std::string funcName = args[0].getFunctionName();
                    Value::ArrayType result;
                    for (size_t i = 0; i < arr.size(); ++i)
                    {
                        result.push_back(callFunction(funcName, {arr[i], Value(static_cast<int>(i))}));
                    }
                    lastResult_ = Value(result);
                    return;
                }
                // ─── رشح / filter — TM::Array::FILTER ───
                if (m == TM::Array::FILTER)
                {
                    if (args.empty() || !args[0].isFunctionOrString())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_FUNCTION,
                            node.position,
                            {{"builtin", "رشح/filter"}});
                    std::string funcName = args[0].getFunctionName();
                    Value::ArrayType result;
                    for (size_t i = 0; i < arr.size(); ++i)
                    {
                        Value cond = callFunction(funcName, {arr[i], Value(static_cast<int>(i))});
                        if (cond.toBool())
                            result.push_back(arr[i]);
                    }
                    lastResult_ = Value(result);
                    return;
                }
                // ─── اختزل / reduce — TM::Array::REDUCE ───
                if (m == TM::Array::REDUCE)
                {
                    if (args.empty() || !args[0].isFunctionOrString())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_FUNCTION,
                            node.position,
                            {{"builtin", "اختزل/reduce"}});
                    std::string funcName = args[0].getFunctionName();
                    if (arr.empty())
                    {
                        lastResult_ = args.size() > 1 ? args[1] : Value();
                        return;
                    }
                    Value accumulator = args.size() > 1 ? args[1] : arr[0];
                    size_t startIdx = args.size() > 1 ? 0 : 1;
                    for (size_t i = startIdx; i < arr.size(); ++i)
                    {
                        accumulator = callFunction(funcName, {accumulator, arr[i]});
                    }
                    lastResult_ = accumulator;
                    return;
                }
                // ─── أي / بعض (some/any) ───
                if (m == TM::Array::SOME)
                {
                    if (args.empty() || !args[0].isFunctionOrString())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_FUNCTION,
                            node.position,
                            {{"builtin", "أي/some"}});
                    std::string funcName = args[0].getFunctionName();
                    for (auto &el : arr)
                    {
                        if (callFunction(funcName, {el}).toBool())
                        {
                            lastResult_ = Value(true);
                            return;
                        }
                    }
                    lastResult_ = Value(false);
                    return;
                }
                // ─── كل / جميع (every) ───
                if (m == TM::Array::EVERY)
                {
                    if (args.empty() || !args[0].isFunctionOrString())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_FUNCTION,
                            node.position,
                            {{"builtin", "كل/every"}});
                    std::string funcName = args[0].getFunctionName();
                    for (auto &el : arr)
                    {
                        if (!callFunction(funcName, {el}).toBool())
                        {
                            lastResult_ = Value(false);
                            return;
                        }
                    }
                    lastResult_ = Value(true);
                    return;
                }
                // ─── جد (find) ───
                if (m == TM::Array::FIND)
                {
                    if (args.empty() || !args[0].isFunctionOrString())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_FUNCTION,
                            node.position,
                            {{"builtin", "جد/find"}});
                    std::string funcName = args[0].getFunctionName();
                    for (auto &el : arr)
                    {
                        if (callFunction(funcName, {el}).toBool())
                        {
                            lastResult_ = el;
                            return;
                        }
                    }
                    lastResult_ = Value();
                    return;
                }
                // ─── جد_فهرس (findIndex) ───
                if (m == TM::Array::FIND_INDEX)
                {
                    if (args.empty() || !args[0].isFunctionOrString())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_FUNCTION,
                            node.position,
                            {{"builtin", "جد_فهرس/findIndex"}});
                    std::string funcName = args[0].getFunctionName();
                    for (int i = 0; i < ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_oop_array_methods_size"); ++i)
                    {
                        if (callFunction(funcName, {arr[i]}).toBool())
                        {
                            lastResult_ = Value(i);
                            return;
                        }
                    }
                    lastResult_ = Value(-1);
                    return;
                }
                // ─── ازدوج / zip ───
                if (m == TM::Array::ZIP)
                {
                    if (args.empty() || !args[0].isArray())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                            node.position,
                            {{"function", "ازدوج/zip"}, {"argument", "second array"}});
                    Value::ArrayType other = args[0].toArray();
                    Value::ArrayType result;
                    size_t minLen = std::min(arr.size(), other.size());
                    for (size_t i = 0; i < minLen; ++i)
                    {
                        Value::ArrayType pair;
                        pair.push_back(arr[i]);
                        pair.push_back(other[i]);
                        result.push_back(Value(pair));
                    }
                    lastResult_ = Value(result);
                    return;
                }
                // ─── عكس المصفوفة بدون تعديل (reversed) ───
                if (m == TM::Array::INVERSE)
                {
                    Value::ArrayType rev(arr.rbegin(), arr.rend());
                    lastResult_ = Value(rev);
                    return;
                }
                // ─── حد_أقصى / max ───
                if (m == TM::Array::MAX)
                {
                    if (arr.empty())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_BUILTIN_EMPTY_OPERATION,
                            node.position,
                            {{"builtin", "حد_اقصى/max"}});
                    Value mx = arr[0];
                    for (size_t i = 1; i < arr.size(); ++i)
                    {
                        if (arr[i].isNumeric() && mx.isNumeric())
                        {
                            if (arr[i].toDouble() > mx.toDouble())
                                mx = arr[i];
                        }
                        else if (arr[i].toString() > mx.toString())
                            mx = arr[i];
                    }
                    lastResult_ = mx;
                    return;
                }
                // ─── حد_أدنى / min ───
                if (m == TM::Array::MIN)
                {
                    if (arr.empty())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_BUILTIN_EMPTY_OPERATION,
                            node.position,
                            {{"builtin", "حد_ادنى/min"}});
                    Value mn = arr[0];
                    for (size_t i = 1; i < arr.size(); ++i)
                    {
                        if (arr[i].isNumeric() && mn.isNumeric())
                        {
                            if (arr[i].toDouble() < mn.toDouble())
                                mn = arr[i];
                        }
                        else if (arr[i].toString() < mn.toString())
                            mn = arr[i];
                    }
                    lastResult_ = mn;
                    return;
                }
                // ─── مجموع / sum ───
                if (m == TM::Array::SUM)
                {
                    double sum = 0;
                    for (auto &el : arr)
                    {
                        if (el.isNumeric())
                            sum += el.toDouble();
                    }
                    if (sum == static_cast<int>(sum))
                        lastResult_ = Value(static_cast<int>(sum));
                    else
                        lastResult_ = Value(sum);
                    return;
                }
                // ─── متوسط / average ───
                if (m == TM::Array::AVERAGE)
                {
                    if (arr.empty())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_BUILTIN_EMPTY_OPERATION,
                            node.position,
                            {{"builtin", "متوسط/average"}});
                    double sum = 0;
                    int count = 0;
                    for (auto &el : arr)
                    {
                        if (el.isNumeric())
                        {
                            sum += el.toDouble();
                            ++count;
                        }
                    }
                    if (count == 0)
                    {
                        lastResult_ = Value(0);
                        return;
                    }
                    lastResult_ = Value(sum / count);
                    return;
                }
                // ─── ملء / fill ───
                if (m == TM::Array::FILL)
                {
                    if (args.empty())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                            node.position,
                            {{"function", "املأ/fill"}, {"argument", "value"}});
                    for (auto &el : arr)
                        el = args[0];
                    Value newArr(arr);
                    writeBackChain(node.object.get(), newArr);
                    lastResult_ = newArr;
                    return;
                }

                // ═══════════════════════════════════════════════════════════════
                // (AR) دوال المصفوفات متعددة الأبعاد (matrix / multidimensional)
                // (EN) Multidimensional array (matrix) methods
                // ═══════════════════════════════════════════════════════════════

                // ─── أبعاد / شكل / dimensions / shape ───
                // (AR) تُرجع مصفوفة بأبعاد المصفوفة، مثلاً [3, 4] لمصفوفة 3×4
                // (EN) Returns an array with the dimensions, e.g. [3, 4] for a 3×4 matrix
                if (m == TM::Array::SHAPE)
                {
                    Value::ArrayType dims;
                    std::function<void(const Value::ArrayType &)> getDims;
                    getDims = [&dims, &getDims](const Value::ArrayType &a)
                    {
                        dims.push_back(Value(static_cast<int64_t>(a.size())));
                        if (!a.empty() && a[0].isArray())
                        {
                            getDims(a[0].toArrayRef());
                        }
                    };
                    getDims(arr);
                    lastResult_ = Value(dims);
                    return;
                }

                // ─── صفوف / rows ───
                // (AR) تُرجع عدد الصفوف (البُعد الأول)
                // (EN) Returns the number of rows (first dimension)
                if (m == TM::Array::ROWS)
                {
                    lastResult_ = Value(static_cast<int64_t>(arr.size()));
                    return;
                }

                // ─── أعمدة / cols / columns ───
                // (AR) تُرجع عدد الأعمدة (البُعد الثاني) أو 0 إذا كانت المصفوفة أحادية
                // (EN) Returns the number of columns (second dimension) or 0 if 1D
                if (m == TM::Array::COLUMNS)
                {
                    if (!arr.empty() && arr[0].isArray())
                    {
                        lastResult_ = Value(static_cast<int64_t>(arr[0].toArrayRef().size()));
                    }
                    else
                    {
                        lastResult_ = Value(static_cast<int64_t>(0));
                    }
                    return;
                }

                // ─── قلب_محوري / transpose ───
                // (AR) تقلب المصفوفة ثنائية الأبعاد (تبادل الصفوف والأعمدة)
                // (EN) Transpose a 2D matrix (swap rows and columns)
                if (m == TM::Array::TRANSPOSE)
                {
                    if (arr.empty())
                    {
                        lastResult_ = Value(arr);
                        return;
                    }
                    if (!arr[0].isArray())
                    {
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_OPERAND_TYPE_INVALID,
                            node.position,
                            {{"operation", "قلب_محوري/transpose"}, {"type", "non-2D array"}});
                    }
                    size_t rows = arr.size();
                    size_t cols = arr[0].toArrayRef().size();
                    Value::ArrayType result;
                    for (size_t c = 0; c < cols; ++c)
                    {
                        Value::ArrayType newRow;
                        for (size_t r = 0; r < rows; ++r)
                        {
                            auto &row = arr[r].toArrayRef();
                            if (c < row.size())
                            {
                                newRow.push_back(row[c]);
                            }
                            else
                            {
                                newRow.push_back(Value());
                            }
                        }
                        result.push_back(Value(newRow));
                    }
                    lastResult_ = Value(result);
                    return;
                }

                // ─── أعد_تشكيل / reshape ───
                // (AR) تُعيد تشكيل مصفوفة مسطحة إلى أبعاد جديدة، مثال: [1,2,3,4,5,6].أعد_تشكيل(2, 3) → [[1,2,3],[4,5,6]]
                // (EN) Reshape a flat array into new dimensions, e.g. [1,2,3,4,5,6].reshape(2, 3) → [[1,2,3],[4,5,6]]
                if (m == TM::Array::RESHAPE)
                {
                    if (args.size() < 2)
                    {
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                            node.position,
                            {{"function", "أعد_تشكيل/reshape"}, {"argument", "rows, cols"}});
                    }
                    // (AR) تسطيح المصفوفة أولاً إن كانت متداخلة
                    Value::ArrayType flat;
                    std::function<void(const Value::ArrayType &)> flattenAll;
                    flattenAll = [&flat, &flattenAll](const Value::ArrayType &a)
                    {
                        for (auto &el : a)
                        {
                            if (el.isArray())
                                flattenAll(el.toArrayRef());
                            else
                                flat.push_back(el);
                        }
                    };
                    flattenAll(arr);

                    int64_t rows = args[0].toInt();
                    int64_t cols = args[1].toInt();
                    if (rows <= 0 || cols <= 0)
                    {
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_OPERAND_TYPE_INVALID,
                            node.position,
                            {{"operation", "أعد_تشكيل/reshape"}, {"type", "non-positive dimensions"}});
                    }
                    if (static_cast<int64_t>(flat.size()) < rows * cols)
                    {
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_INDEX_OUT_OF_RANGE,
                            node.position,
                            {{"index", std::to_string(rows * cols)}, {"length", std::to_string(flat.size())}, {"container", "array"}});
                    }
                    if (args.size() >= 3)
                    {
                        // (AR) إعادة تشكيل ثلاثية الأبعاد
                        int64_t depth = args[2].toInt();
                        if (depth <= 0)
                        {
                            ::Sad::Errors::throwRuntime(
                                ::Sad::Errors::ErrorCode::RUN_OPERAND_TYPE_INVALID,
                                node.position,
                                {{"operation", "أعد_تشكيل/reshape"}, {"type", "non-positive depth"}});
                        }
                        if (static_cast<int64_t>(flat.size()) < rows * cols * depth)
                        {
                            ::Sad::Errors::throwRuntime(
                                ::Sad::Errors::ErrorCode::RUN_INDEX_OUT_OF_RANGE,
                                node.position,
                                {{"index", std::to_string(rows * cols * depth)}, {"length", std::to_string(flat.size())}, {"container", "array"}});
                        }
                        Value::ArrayType result3d;
                        size_t idx = 0;
                        for (int64_t i = 0; i < rows; ++i)
                        {
                            Value::ArrayType plane;
                            for (int64_t j = 0; j < cols; ++j)
                            {
                                Value::ArrayType row;
                                for (int64_t k = 0; k < depth; ++k)
                                {
                                    row.push_back(flat[idx++]);
                                }
                                plane.push_back(Value(row));
                            }
                            result3d.push_back(Value(plane));
                        }
                        lastResult_ = Value(result3d);
                    }
                    else
                    {
                        Value::ArrayType result2d;
                        size_t idx = 0;
                        for (int64_t i = 0; i < rows; ++i)
                        {
                            Value::ArrayType row;
                            for (int64_t j = 0; j < cols; ++j)
                            {
                                row.push_back(flat[idx++]);
                            }
                            result2d.push_back(Value(row));
                        }
                        lastResult_ = Value(result2d);
                    }
                    return;
                }

                // ─── ضرب_مصفوفات / matmul / dot ───
                // (AR) ضرب مصفوفتين ثنائيتي الأبعاد: أ(m×n) · ب(n×p) = ج(m×p)
                // (EN) Matrix multiplication: A(m×n) · B(n×p) = C(m×p)
                if (m == TM::Array::MATMUL)
                {
                    if (args.empty() || !args[0].isArray())
                    {
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                            node.position,
                            {{"function", "ضرب_مصفوفات/matmul"}, {"argument", "second matrix"}});
                    }
                    if (arr.empty() || !arr[0].isArray())
                    {
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_OPERAND_TYPE_INVALID,
                            node.position,
                            {{"operation", "ضرب_مصفوفات/matmul"}, {"type", "non-2D first matrix"}});
                    }
                    auto &b = args[0].toArrayRef();
                    if (b.empty() || !b[0].isArray())
                    {
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_OPERAND_TYPE_INVALID,
                            node.position,
                            {{"operation", "ضرب_مصفوفات/matmul"}, {"type", "non-2D second matrix"}});
                    }
                    size_t m_rows = arr.size();
                    size_t n_cols_a = arr[0].toArrayRef().size();
                    size_t n_rows_b = b.size();
                    size_t p_cols = b[0].toArrayRef().size();
                    if (n_cols_a != n_rows_b)
                    {
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_OPERAND_TYPE_INVALID,
                            node.position,
                            {{"operation", "ضرب_مصفوفات/matmul"}, {"type", "incompatible dimensions"}});
                    }
                    Value::ArrayType result;
                    for (size_t i = 0; i < m_rows; ++i)
                    {
                        Value::ArrayType row;
                        auto &aRow = arr[i].toArrayRef();
                        for (size_t j = 0; j < p_cols; ++j)
                        {
                            double sum = 0.0;
                            for (size_t k = 0; k < n_cols_a; ++k)
                            {
                                sum += aRow[k].toDouble() * b[k].toArrayRef()[j].toDouble();
                            }
                            if (sum == static_cast<int64_t>(sum))
                                row.push_back(Value(static_cast<int64_t>(sum)));
                            else
                                row.push_back(Value(sum));
                        }
                        result.push_back(Value(row));
                    }
                    lastResult_ = Value(result);
                    return;
                }

                // ─── عنصر / element / at ───
                // (AR) الوصول إلى عنصر بإحداثيات متعددة: م.عنصر(ص، ع) = م[ص][ع]
                // (EN) Access element by multiple indices: m.element(r, c) = m[r][c]
                if (m == TM::Array::ELEMENT)
                {
                    Value current(arr);
                    for (size_t i = 0; i < args.size(); ++i)
                    {
                        if (!current.isArray())
                        {
                            ::Sad::Errors::throwRuntime(
                                ::Sad::Errors::ErrorCode::RUN_INDEX_OUT_OF_RANGE,
                                node.position,
                                {{"index", std::to_string(i)}, {"length", "0"}, {"container", "array"}});
                        }
                        int idx = args[i].toInt();
                        auto &currentArr = current.toArrayRef();
                        // (AR) تطبيع الفهرس السالب: -1 = آخر عنصر
                        // (EN) Normalize negative index: -1 = last element
                        if (idx < 0)
                            idx = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(currentArr.size(), "expression_evaluator_oop_array_methods_size") + idx;
                        if (idx < 0 || idx >= ::Sad::Security::SafeArithmetic::assertSafeCast<int>(currentArr.size(), "expression_evaluator_oop_array_methods_size"))
                        {
                            ::Sad::Errors::throwRuntime(
                                ::Sad::Errors::ErrorCode::RUN_INDEX_OUT_OF_RANGE,
                                node.position,
                                {{"index", std::to_string(idx)}, {"length", std::to_string(currentArr.size())}, {"container", "array"}});
                        }
                        current = currentArr[idx];
                    }
                    lastResult_ = current;
                    return;
                }

                // ─── عمود / column ───
                // (AR) تُرجع عموداً معيناً من مصفوفة ثنائية الأبعاد كمصفوفة أحادية
                // (EN) Returns a specific column from a 2D matrix as a 1D array
                if (m == TM::Array::COLUMN)
                {
                    if (args.empty())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                            node.position,
                            {{"function", "عمود/column"}, {"argument", "column index"}});
                    int colIdx = args[0].toInt();
                    Value::ArrayType col;
                    for (auto &rowVal : arr)
                    {
                        if (rowVal.isArray())
                        {
                            auto &row = rowVal.toArrayRef();
                            // (AR) تطبيع الفهرس السالب: -1 = آخر عمود
                            // (EN) Normalize negative index: -1 = last column
                            int resolvedCol = colIdx;
                            if (resolvedCol < 0)
                                resolvedCol = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(row.size(), "expression_evaluator_oop_array_methods_size") + resolvedCol;
                            if (resolvedCol >= 0 && resolvedCol < ::Sad::Security::SafeArithmetic::assertSafeCast<int>(row.size(), "expression_evaluator_oop_array_methods_size"))
                            {
                                col.push_back(row[resolvedCol]);
                            }
                            else
                            {
                                col.push_back(Value());
                            }
                        }
                    }
                    lastResult_ = Value(col);
                    return;
                }

                // ─── صف / row ───
                // (AR) تُرجع صفاً معيناً من المصفوفة (مكافئ لـ م[ف])
                // (EN) Returns a specific row (equivalent to m[r])
                if (m == TM::Array::ROW)
                {
                    if (args.empty())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                            node.position,
                            {{"function", "صف/row"}, {"argument", "row index"}});
                    int rowIdx = args[0].toInt();
                    // (AR) تطبيع الفهرس السالب: -1 = آخر صف
                    // (EN) Normalize negative index: -1 = last row
                    if (rowIdx < 0)
                        rowIdx = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_oop_array_methods_size") + rowIdx;
                    if (rowIdx < 0 || rowIdx >= ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_oop_array_methods_size"))
                    {
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_INDEX_OUT_OF_RANGE,
                            node.position,
                            {{"index", std::to_string(rowIdx)}, {"length", std::to_string(arr.size())}, {"container", "array"}});
                    }
                    lastResult_ = arr[rowIdx];
                    return;
                }

                // ─── مصفوفة_وحدة / identity ───
                // (AR) إنشاء مصفوفة وحدة n×n (مصفوفة ساكنة — تُستدعى على أي مصفوفة)
                // (EN) Create an n×n identity matrix
                if (m == TM::Array::IDENTITY)
                {
                    int64_t n = args.empty() ? static_cast<int64_t>(arr.size()) : args[0].toInt();
                    Value::ArrayType result;
                    for (int64_t i = 0; i < n; ++i)
                    {
                        Value::ArrayType row;
                        for (int64_t j = 0; j < n; ++j)
                        {
                            row.push_back(Value(static_cast<int64_t>(i == j ? 1 : 0)));
                        }
                        result.push_back(Value(row));
                    }
                    lastResult_ = Value(result);
                    return;
                }

                // ─── البُعد / dimension / ndim ───
                // (AR) تُرجع عدد الأبعاد (عمق التداخل)
                // (EN) Returns the number of dimensions (nesting depth)
                if (m == TM::Array::DIMENSION)
                {
                    int depth = 1;
                    const Value::ArrayType *current = &arr;
                    while (!current->empty() && (*current)[0].isArray())
                    {
                        depth++;
                        current = &((*current)[0].toArrayRef());
                    }
                    lastResult_ = Value(static_cast<int64_t>(depth));
                    return;
                }

                // (AR) طريقة غير معروفة على المصفوفة
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_METHOD_NOT_FOUND,
                    node.position,
                    {{"method", m}, {"class", "array"}});
            }
        }

    } // namespace Interpreter
} // namespace Sad
