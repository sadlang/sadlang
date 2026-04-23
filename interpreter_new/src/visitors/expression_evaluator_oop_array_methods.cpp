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
#include "exception.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <vector>

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

                // ─── الطول / الحجم ───
                if (m == "الطول" || m == "الحجم" || m == "طول")
                {
                    lastResult_ = Value(static_cast<int>(arr.size()));
                    return;
                }
                // ─── إضافة عنصر (تعديل موضعي) ───
                if (m == "اضف" || m == "أضف" || m == "ادفع")
                {
                    if (args.empty())
                        throw RuntimeError("(AR) اضف() يتطلب معاملاً واحداً على الأقل. (EN) push() requires at least one argument.", node.position);
                    for (auto &a : args)
                        arr.push_back(a);
                    Value newArr(arr);
                    writeBackChain(node.object.get(), newArr);
                    lastResult_ = newArr;
                    return;
                }
                // ─── حذف وإرجاع آخر عنصر ───
                if (m == "احذف_اخير" || m == "انزع")
                {
                    if (arr.empty())
                        throw RuntimeError("(AR) لا يمكن الحذف من مصفوفة فارغة. (EN) Cannot pop from empty array.", node.position);
                    Value last = arr.back();
                    arr.pop_back();
                    Value newArr(arr);
                    writeBackChain(node.object.get(), newArr);
                    lastResult_ = last;
                    return;
                }
                // ─── حذف عنصر بالفهرس ───
                if (m == "احذف" || m == "ازل" || m == "أزل")
                {
                    if (args.empty())
                        throw RuntimeError("(AR) احذف() يتطلب فهرس العنصر. (EN) remove() requires an index.", node.position);
                    int idx = args[0].toInt();
                    if (idx < 0)
                        idx = static_cast<int>(arr.size()) + idx;
                    if (idx < 0 || idx >= static_cast<int>(arr.size()))
                        throw RuntimeError("(AR) الفهرس " + std::to_string(idx) + " خارج النطاق. (EN) Index out of range.", node.position);
                    Value removed = arr[idx];
                    arr.erase(arr.begin() + idx);
                    Value newArr(arr);
                    writeBackChain(node.object.get(), newArr);
                    lastResult_ = removed;
                    return;
                }
                // ─── إدخال عنصر في موقع محدد ───
                if (m == "ادخل" || m == "أدخل")
                {
                    if (args.size() < 2)
                        throw RuntimeError("(AR) ادخل() يتطلب فهرساً وعنصراً. (EN) insert() requires index and value.", node.position);
                    int idx = args[0].toInt();
                    if (idx < 0)
                        idx = static_cast<int>(arr.size()) + idx;
                    if (idx < 0)
                        idx = 0;
                    if (idx > static_cast<int>(arr.size()))
                        idx = static_cast<int>(arr.size());
                    arr.insert(arr.begin() + idx, args[1]);
                    Value newArr(arr);
                    writeBackChain(node.object.get(), newArr);
                    lastResult_ = newArr;
                    return;
                }
                // ─── أول عنصر ───
                if (m == "اول" || m == "أول")
                {
                    if (arr.empty())
                        throw RuntimeError("(AR) المصفوفة فارغة. (EN) Array is empty.", node.position);
                    lastResult_ = arr.front();
                    return;
                }
                // ─── آخر عنصر ───
                if (m == "اخر" || m == "آخر")
                {
                    if (arr.empty())
                        throw RuntimeError("(AR) المصفوفة فارغة. (EN) Array is empty.", node.position);
                    lastResult_ = arr.back();
                    return;
                }
                // ─── يحتوي ───
                if (m == "يحتوي")
                {
                    if (args.empty())
                        throw RuntimeError("(AR) يحتوي() يتطلب معاملاً. (EN) contains() requires argument.", node.position);
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
                // ─── فهرس العنصر ───
                if (m == "فهرس")
                {
                    if (args.empty())
                        throw RuntimeError("(AR) فهرس() يتطلب معاملاً. (EN) indexOf() requires argument.", node.position);
                    for (int i = 0; i < static_cast<int>(arr.size()); ++i)
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
                // ─── عكس ───
                if (m == "اقلب" || m == "قلب")
                {
                    std::reverse(arr.begin(), arr.end());
                    Value newArr(arr);
                    writeBackChain(node.object.get(), newArr);
                    lastResult_ = newArr;
                    return;
                }
                // ─── ترتيب ───
                if (m == "رتب" || m == "فرز")
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
                if (m == "شريحة")
                {
                    int start = args.empty() ? 0 : args[0].toInt();
                    int end = args.size() < 2 ? static_cast<int>(arr.size()) : args[1].toInt();
                    if (start < 0)
                        start = std::max(0, static_cast<int>(arr.size()) + start);
                    if (end < 0)
                        end = std::max(0, static_cast<int>(arr.size()) + end);
                    if (start > static_cast<int>(arr.size()))
                        start = static_cast<int>(arr.size());
                    if (end > static_cast<int>(arr.size()))
                        end = static_cast<int>(arr.size());
                    if (start >= end)
                    {
                        lastResult_ = Value(Value::ArrayType{});
                        return;
                    }
                    Value::ArrayType sliced(arr.begin() + start, arr.begin() + end);
                    lastResult_ = Value(sliced);
                    return;
                }
                // ─── فارغة ───
                if (m == "فارغ" || m == "فارغة")
                {
                    lastResult_ = Value(arr.empty());
                    return;
                }
                // ─── مسح ───
                if (m == "امسح" || m == "نظف")
                {
                    Value newArr(Value::ArrayType{});
                    writeBackChain(node.object.get(), newArr);
                    lastResult_ = newArr;
                    return;
                }
                // ─── صل / اربط (join) ───
                if (m == "صل" || m == "اربط")
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
                if (m == "نسخ" || m == "انسخ" || m == "استنسخ")
                {
                    Value::ArrayType copy(arr.begin(), arr.end());
                    lastResult_ = Value(copy);
                    return;
                }
                // ─── تسطيح (flatten) ───
                if (m == "مسطح" || m == "افرد")
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
                if (m == "فريد" || m == "مميز")
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
                if (m == "عدّ" || m == "عد")
                {
                    if (args.empty())
                    {
                        lastResult_ = Value(static_cast<int>(arr.size()));
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
                        throw RuntimeError("(AR) الدالة '" + funcName + "' غير موجودة. (EN) Function '" + funcName + "' not found.", node.position);
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

                // ─── لكل (forEach) ───
                if (m == "لكل")
                {
                    if (args.empty() || !args[0].isFunctionOrString())
                        throw RuntimeError("(AR) لكل() يتطلب دالة أو اسم دالة. (EN) forEach() requires a function or function name.", node.position);
                    std::string funcName = args[0].getFunctionName();
                    for (size_t i = 0; i < arr.size(); ++i)
                    {
                        callFunction(funcName, {arr[i], Value(static_cast<int>(i))});
                    }
                    lastResult_ = Value();
                    return;
                }
                // ─── خريطة / حوّل (map) ───
                if (m == "خريطة" || m == "حوّل" || m == "حول")
                {
                    if (args.empty() || !args[0].isFunctionOrString())
                        throw RuntimeError("(AR) خريطة() يتطلب دالة أو اسم دالة. (EN) map() requires a function or function name.", node.position);
                    std::string funcName = args[0].getFunctionName();
                    Value::ArrayType result;
                    for (size_t i = 0; i < arr.size(); ++i)
                    {
                        result.push_back(callFunction(funcName, {arr[i], Value(static_cast<int>(i))}));
                    }
                    lastResult_ = Value(result);
                    return;
                }
                // ─── رشح / صفّي (filter) ───
                if (m == "رشح" || m == "صفّي" || m == "صفي")
                {
                    if (args.empty() || !args[0].isFunctionOrString())
                        throw RuntimeError("(AR) رشح() يتطلب دالة أو اسم دالة. (EN) filter() requires a function or function name.", node.position);
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
                // ─── اختزل (reduce) ───
                if (m == "اختزل")
                {
                    if (args.empty() || !args[0].isFunctionOrString())
                        throw RuntimeError("(AR) اختزل() يتطلب دالة أو اسم دالة. (EN) reduce() requires a function or function name.", node.position);
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
                if (m == "أي" || m == "اي" || m == "بعض")
                {
                    if (args.empty() || !args[0].isFunctionOrString())
                        throw RuntimeError("(AR) أي() يتطلب دالة أو اسم دالة. (EN) some() requires a function or function name.", node.position);
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
                if (m == "كل" || m == "جميع")
                {
                    if (args.empty() || !args[0].isFunctionOrString())
                        throw RuntimeError("(AR) كل() يتطلب دالة أو اسم دالة. (EN) every() requires a function or function name.", node.position);
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
                if (m == "جد")
                {
                    if (args.empty() || !args[0].isFunctionOrString())
                        throw RuntimeError("(AR) جد() يتطلب دالة أو اسم دالة. (EN) find() requires a function or function name.", node.position);
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
                if (m == "جد_فهرس")
                {
                    if (args.empty() || !args[0].isFunctionOrString())
                        throw RuntimeError("(AR) جد_فهرس() يتطلب دالة أو اسم دالة. (EN) findIndex() requires a function or function name.", node.position);
                    std::string funcName = args[0].getFunctionName();
                    for (int i = 0; i < static_cast<int>(arr.size()); ++i)
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
                if (m == "ازدوج")
                {
                    if (args.empty() || !args[0].isArray())
                        throw RuntimeError("(AR) ازدوج() يتطلب مصفوفة ثانية. (EN) zip() requires another array.", node.position);
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
                if (m == "معكوس")
                {
                    Value::ArrayType rev(arr.rbegin(), arr.rend());
                    lastResult_ = Value(rev);
                    return;
                }
                // ─── حد_أقصى / max ───
                if (m == "حد_اقصى" || m == "أقصى")
                {
                    if (arr.empty())
                        throw RuntimeError("(AR) المصفوفة فارغة. (EN) Array is empty.", node.position);
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
                if (m == "حد_ادنى" || m == "أدنى")
                {
                    if (arr.empty())
                        throw RuntimeError("(AR) المصفوفة فارغة. (EN) Array is empty.", node.position);
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
                if (m == "مجموع")
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
                if (m == "متوسط")
                {
                    if (arr.empty())
                        throw RuntimeError("(AR) المصفوفة فارغة. (EN) Array is empty.", node.position);
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
                if (m == "املأ" || m == "املا")
                {
                    if (args.empty())
                        throw RuntimeError("(AR) املأ() يتطلب قيمة. (EN) fill() requires a value.", node.position);
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
                if (m == "أبعاد" || m == "ابعاد" || m == "شكل")
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
                if (m == "صفوف")
                {
                    lastResult_ = Value(static_cast<int64_t>(arr.size()));
                    return;
                }

                // ─── أعمدة / cols / columns ───
                // (AR) تُرجع عدد الأعمدة (البُعد الثاني) أو 0 إذا كانت المصفوفة أحادية
                // (EN) Returns the number of columns (second dimension) or 0 if 1D
                if (m == "أعمدة" || m == "اعمدة")
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
                if (m == "قلب_محوري" || m == "تبديل")
                {
                    if (arr.empty())
                    {
                        lastResult_ = Value(arr);
                        return;
                    }
                    if (!arr[0].isArray())
                    {
                        throw RuntimeError("(AR) قلب_محوري() تعمل فقط على مصفوفة ثنائية الأبعاد. (EN) transpose() only works on 2D arrays.", node.position);
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
                if (m == "أعد_تشكيل" || m == "اعد_تشكيل")
                {
                    if (args.size() < 2)
                    {
                        throw RuntimeError("(AR) أعد_تشكيل() يتطلب بُعدين على الأقل (صفوف، أعمدة). (EN) reshape() requires at least 2 dimensions (rows, cols).", node.position);
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
                        throw RuntimeError("(AR) أبعاد المصفوفة يجب أن تكون موجبة. (EN) Matrix dimensions must be positive.", node.position);
                    }
                    if (static_cast<int64_t>(flat.size()) < rows * cols)
                    {
                        throw RuntimeError("(AR) عدد العناصر (" + std::to_string(flat.size()) + ") لا يكفي للأبعاد المطلوبة (" + std::to_string(rows) + "×" + std::to_string(cols) + "). (EN) Not enough elements.", node.position);
                    }
                    if (args.size() >= 3)
                    {
                        // (AR) إعادة تشكيل ثلاثية الأبعاد
                        int64_t depth = args[2].toInt();
                        if (depth <= 0)
                        {
                            throw RuntimeError("(AR) العمق يجب أن يكون موجباً. (EN) Depth must be positive.", node.position);
                        }
                        if (static_cast<int64_t>(flat.size()) < rows * cols * depth)
                        {
                            throw RuntimeError("(AR) عدد العناصر لا يكفي للأبعاد الثلاثية. (EN) Not enough elements for 3D reshape.", node.position);
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
                if (m == "ضرب_مصفوفات" || m == "ضرب")
                {
                    if (args.empty() || !args[0].isArray())
                    {
                        throw RuntimeError("(AR) ضرب_مصفوفات() يتطلب مصفوفة ثانية. (EN) matmul() requires another matrix.", node.position);
                    }
                    if (arr.empty() || !arr[0].isArray())
                    {
                        throw RuntimeError("(AR) ضرب_مصفوفات() تعمل فقط على مصفوفات ثنائية الأبعاد. (EN) matmul() only works on 2D arrays.", node.position);
                    }
                    auto &b = args[0].toArrayRef();
                    if (b.empty() || !b[0].isArray())
                    {
                        throw RuntimeError("(AR) المصفوفة الثانية يجب أن تكون ثنائية الأبعاد. (EN) Second matrix must be 2D.", node.position);
                    }
                    size_t m_rows = arr.size();
                    size_t n_cols_a = arr[0].toArrayRef().size();
                    size_t n_rows_b = b.size();
                    size_t p_cols = b[0].toArrayRef().size();
                    if (n_cols_a != n_rows_b)
                    {
                        throw RuntimeError("(AR) أبعاد المصفوفتين غير متوافقة للضرب (" + std::to_string(n_cols_a) + " != " + std::to_string(n_rows_b) + "). (EN) Incompatible matrix dimensions.", node.position);
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
                if (m == "عنصر")
                {
                    Value current(arr);
                    for (size_t i = 0; i < args.size(); ++i)
                    {
                        if (!current.isArray())
                        {
                            throw RuntimeError("(AR) فهرس زائد عن أبعاد المصفوفة. (EN) Index exceeds array dimensions.", node.position);
                        }
                        int idx = args[i].toInt();
                        auto &currentArr = current.toArrayRef();
                        // (AR) تطبيع الفهرس السالب: -1 = آخر عنصر
                        // (EN) Normalize negative index: -1 = last element
                        if (idx < 0)
                            idx = static_cast<int>(currentArr.size()) + idx;
                        if (idx < 0 || idx >= static_cast<int>(currentArr.size()))
                        {
                            throw RuntimeError("(AR) فهرس خارج النطاق: " + std::to_string(idx) + ". (EN) Index out of bounds.", node.position);
                        }
                        current = currentArr[idx];
                    }
                    lastResult_ = current;
                    return;
                }

                // ─── عمود / column ───
                // (AR) تُرجع عموداً معيناً من مصفوفة ثنائية الأبعاد كمصفوفة أحادية
                // (EN) Returns a specific column from a 2D matrix as a 1D array
                if (m == "عمود")
                {
                    if (args.empty())
                        throw RuntimeError("(AR) عمود() يتطلب رقم العمود. (EN) column() requires column index.", node.position);
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
                                resolvedCol = static_cast<int>(row.size()) + resolvedCol;
                            if (resolvedCol >= 0 && resolvedCol < static_cast<int>(row.size()))
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
                if (m == "صف")
                {
                    if (args.empty())
                        throw RuntimeError("(AR) صف() يتطلب رقم الصف. (EN) row() requires row index.", node.position);
                    int rowIdx = args[0].toInt();
                    // (AR) تطبيع الفهرس السالب: -1 = آخر صف
                    // (EN) Normalize negative index: -1 = last row
                    if (rowIdx < 0)
                        rowIdx = static_cast<int>(arr.size()) + rowIdx;
                    if (rowIdx < 0 || rowIdx >= static_cast<int>(arr.size()))
                    {
                        throw RuntimeError("(AR) فهرس الصف خارج النطاق. (EN) Row index out of bounds.", node.position);
                    }
                    lastResult_ = arr[rowIdx];
                    return;
                }

                // ─── مصفوفة_وحدة / identity ───
                // (AR) إنشاء مصفوفة وحدة n×n (مصفوفة ساكنة — تُستدعى على أي مصفوفة)
                // (EN) Create an n×n identity matrix
                if (m == "مصفوفة_وحدة" || m == "وحدة")
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
                if (m == "البُعد" || m == "البعد" || m == "بعد")
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
                throw RuntimeError(
                    "(AR) الطريقة '" + m + "' غير موجودة على المصفوفة. (EN) Method '" + m + "' not found on array.",
                    node.position);
            }
        }

    } // namespace Interpreter
} // namespace Sad
