/**
 * @file builtin_module_maps_arrays.cpp
 * @brief (AR) وحدة المصفوفات المتقدمة — دوال المصفوفات والمصفوفات متعددة الأبعاد (matrix)
 * @brief (EN) Advanced arrays module — array functions and multidimensional arrays (matrix)
 *
 * @details
 * (AR) الأقسام:
 *   13. دوال المصفوفات المتقدمة (ترتيب، تسطيح، تجميع، تقطيع...)
 *   16. دوال المصفوفات متعددة الأبعاد (matrix — إنشاء، هوية، ضرب، نقل...)
 *
 * @note يتطلب: Interpreter& لـ sort_array_fn (callUserFunction)
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */
#include "builtins.h"
#include "interpreter_core.h"
#include "builtin_registry.h"
#include <algorithm>
#include <cmath>
#include <random>

// (AR) إلغاء ماكرو VOID الخاص بويندوز إن وُجد
#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int
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

        void registerBuiltinsMapsArrays(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // ═══════════════════════════════════════════════════════════════════
            // 13. دوال المصفوفات المتقدمة / Advanced Array Functions
            // (AR) إصلاح نقطة ضعف رقم 13: نقص دوال المصفوفات المتقدمة
            // ═══════════════════════════════════════════════════════════════════

            // flatten / تسطيح — تسطيح مصفوفة متداخلة
            auto flatten_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                Data::Value::ArrayType result;
                std::function<void(const Data::Value::ArrayType &)> flattenHelper;
                flattenHelper = [&result, &flattenHelper](const Data::Value::ArrayType &arr)
                {
                    for (const auto &item : arr)
                    {
                        if (item.isArray())
                            flattenHelper(item.toArrayRef());
                        else
                            result.push_back(item);
                    }
                };
                flattenHelper(args[0]->toArrayRef());
                return makeArrayVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::FLATTEN), flatten_fn);

            // chunk / تقسيم — تقسيم مصفوفة إلى أجزاء
            auto chunk_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2 || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &arr = args[0]->toArrayRef();
                int chunkSize = args[1]->toInt();
                if (chunkSize <= 0)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                Data::Value::ArrayType result;
                for (size_t i = 0; i < arr.size(); i += chunkSize)
                {
                    Data::Value::ArrayType chunk;
                    for (size_t j = i; j < std::min(i + static_cast<size_t>(chunkSize), arr.size()); j++)
                    {
                        chunk.push_back(arr[j]);
                    }
                    result.push_back(Data::Value(chunk));
                }
                return makeArrayVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::CHUNK), chunk_fn);

            // take / خذ — أخذ أول n عنصر
            auto take_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2 || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &arr = args[0]->toArrayRef();
                int n = args[1]->toInt();
                Data::Value::ArrayType result;
                for (int i = 0; i < n && i < ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "builtin_module_maps_arrays_size"); i++)
                {
                    result.push_back(arr[i]);
                }
                return makeArrayVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::TAKE), take_fn);

            // drop / اترك — حذف أول n عنصر
            auto drop_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2 || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &arr = args[0]->toArrayRef();
                int n = args[1]->toInt();
                Data::Value::ArrayType result;
                for (int i = n; i < ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "builtin_module_maps_arrays_size"); i++)
                {
                    result.push_back(arr[i]);
                }
                return makeArrayVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::DROP), drop_fn);

            // enumerate / رقّم — ترقيم عناصر المصفوفة [فهرس، قيمة]
            auto enumerate_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &arr = args[0]->toArrayRef();
                Data::Value::ArrayType result;
                for (size_t i = 0; i < arr.size(); i++)
                {
                    Data::Value::ArrayType pair;
                    pair.push_back(Data::Value(static_cast<int>(i)));
                    pair.push_back(arr[i]);
                    result.push_back(Data::Value(pair));
                }
                return makeArrayVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::ENUMERATE), enumerate_fn);

            // sum_array / مجموع — مجموع عناصر مصفوفة عددية
            auto sum_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                double sum = 0;
                for (const auto &item : args[0]->toArrayRef())
                {
                    sum += item.toDouble();
                }
                return makeVal(sum);
            };

            // average / متوسط — متوسط عناصر مصفوفة عددية
            auto average_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &arr = args[0]->toArrayRef();
                if (arr.empty())
                    return makeVal(0.0);
                double sum = 0;
                for (const auto &item : arr)
                    sum += item.toDouble();
                return makeVal(sum / static_cast<double>(arr.size()));
            };
            fm.registerBuiltinFunction(std::string(Bmp::AVERAGE), average_fn);

            // min_array / أصغر_المصفوفة — أصغر قيمة في المصفوفة
            auto min_array_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &arr = args[0]->toArrayRef();
                if (arr.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_EMPTY_OPERATION);
                double minVal = arr[0].toDouble();
                for (size_t i = 1; i < arr.size(); i++)
                {
                    double v = arr[i].toDouble();
                    if (v < minVal)
                        minVal = v;
                }
                return makeVal(minVal);
            };
            fm.registerBuiltinFunction(std::string(Bmp::MIN_ARRAY), min_array_fn);

            // max_array / أكبر_المصفوفة — أكبر قيمة في المصفوفة
            auto max_array_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &arr = args[0]->toArrayRef();
                if (arr.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_EMPTY_OPERATION);
                double maxVal = arr[0].toDouble();
                for (size_t i = 1; i < arr.size(); i++)
                {
                    double v = arr[i].toDouble();
                    if (v > maxVal)
                        maxVal = v;
                }
                return makeVal(maxVal);
            };
            fm.registerBuiltinFunction(std::string(Bmp::MAX_ARRAY), max_array_fn);

            // sort_array / ترتيب — ترتيب مصفوفة عددية (مع دعم دالة مقارنة اختيارية)
            auto sort_array_fn = [&interpreter](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                auto arr = args[0]->toArray(); // نسخة

                // (AR) إذا تم تمرير دالة مقارنة كوسيط ثانٍ
                // (EN) If a comparator function name is passed as second argument
                if (args.size() >= 2 && args[1]->isString())
                {
                    std::string funcName = args[1]->toString();
                    // Check if it's actually a function name (not "تصاعدي"/"تنازلي")
                    if (funcName != "تصاعدي" && funcName != "تنازلي" &&
                        funcName != "ascending" && funcName != "descending" &&
                        funcName != "asc" && funcName != "desc")
                    {
                        std::sort(arr.begin(), arr.end(), [&](const Data::Value &a, const Data::Value &b)
                                  {
                    std::vector<Data::Value> callArgs = {a, b};
                    auto res = interpreter.callUserFunction(funcName, callArgs);
                    if (res.isInteger()) return res.toInt() < 0;
                    return res.toBool(); });
                        return makeArrayVal(arr);
                    }
                    // (AR) ترتيب تنازلي
                    if (funcName == "تنازلي" || funcName == "descending" || funcName == "desc")
                    {
                        std::sort(arr.begin(), arr.end(), [](const Data::Value &a, const Data::Value &b)
                                  {
                    if (a.isNumeric() && b.isNumeric()) return a.toDouble() > b.toDouble();
                    return a.toString() > b.toString(); });
                        return makeArrayVal(arr);
                    }
                }

                // (AR) ترتيب تصاعدي عادي (الافتراضي)
                std::sort(arr.begin(), arr.end(), [](const Data::Value &a, const Data::Value &b)
                          {
            if (a.isNumeric() && b.isNumeric()) return a.toDouble() < b.toDouble();
            return a.toString() < b.toString(); });
                return makeArrayVal(arr);
            };
            fm.registerBuiltinFunction(std::string(Bmp::SORT_ARRAY), sort_array_fn);

            // reverse_array / عكس_مصفوفة — عكس ترتيب مصفوفة
            auto reverse_arr_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                auto arr = args[0]->toArray(); // نسخة
                std::reverse(arr.begin(), arr.end());
                return makeArrayVal(arr);
            };
            fm.registerBuiltinFunction(std::string(Bmp::REVERSE_ARRAY), reverse_arr_fn);

            // range / نطاق — إنشاء مصفوفة أرقام من start إلى end

            // includes / يتضمن — هل تتضمن المصفوفة عنصراً

            // index_of / فهرس — الحصول على فهرس عنصر في مصفوفة

            // join_array / ربط — ربط عناصر مصفوفة بفاصل
            auto join_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &arr = args[0]->toArrayRef();
                std::string sep = args.size() >= 2 ? args[1]->toString() : ",";
                std::string result;
                for (size_t i = 0; i < arr.size(); i++)
                {
                    if (i > 0)
                        result += sep;
                    result += arr[i].toString();
                }
                return makeVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::JOIN), join_fn);

            // sleep / انتظر — إيقاف التنفيذ لعدد من المللي ثواني

            // to_string / إلى_نص — تحويل أي قيمة لنص

            // to_int / إلى_رقم — تحويل قيمة لرقم صحيح

            // to_float / إلى_عشري — تحويل قيمة لرقم عشري

            // ═══════════════════════════════════════════════════════════════════
            // (AR) 16. دوال المصفوفات متعددة الأبعاد (matrix / multidimensional)
            // (EN) 16. Multidimensional array (matrix) functions
            // ═══════════════════════════════════════════════════════════════════

            // مصفوفة_جديدة / matrix_new — إنشاء مصفوفة بأبعاد محددة مملوءة بقيمة افتراضية
            // Usage: مصفوفة_جديدة(صفوف، أعمدة) → zeros matrix
            //        مصفوفة_جديدة(صفوف، أعمدة، قيمة) → filled matrix
            //        مصفوفة_جديدة(صفوف، أعمدة، أعماق) → 3D zeros
            //        مصفوفة_جديدة(صفوف، أعمدة، أعماق، قيمة) → 3D filled
            auto matrix_new_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.size() < 2)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                int64_t rows = args[0]->toInt();
                int64_t cols = args[1]->toInt();
                if (rows <= 0 || cols <= 0)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);

                // (AR) تحقق من وجود بُعد ثالث (3D)
                if (args.size() >= 3 && args[2]->isNumeric() && args[2]->toInt() > 0 && (args.size() < 4 || args[3]->isNumeric()))
                {
                    // (AR) 3D mode — if args[2] is a positive int and args[3] isn't a non-numeric fill value
                    // Check: is args[2] an actual dimension or a fill value?
                    // If args.size() == 3, it could be either 3D-zeros or 2D-filled
                    // Convention: if args[2] > 0 and args.size() == 4, treat as 3D(rows, cols, depth, fill)
                    if (args.size() >= 4)
                    {
                        int64_t depth = args[2]->toInt();
                        Data::Value fillVal = *args[3];
                        Data::Value::ArrayType result;
                        for (int64_t i = 0; i < rows; ++i)
                        {
                            Data::Value::ArrayType plane;
                            for (int64_t j = 0; j < cols; ++j)
                            {
                                Data::Value::ArrayType row;
                                for (int64_t k = 0; k < depth; ++k)
                                {
                                    row.push_back(fillVal);
                                }
                                plane.push_back(Data::Value(row));
                            }
                            result.push_back(Data::Value(plane));
                        }
                        return makeArrayVal(result);
                    }
                    // args.size() == 3: treat it as 2D with fill value
                }

                // (AR) 2D mode
                Data::Value fillVal(static_cast<int64_t>(0));
                if (args.size() >= 3)
                {
                    fillVal = *args[2];
                }
                Data::Value::ArrayType result;
                for (int64_t i = 0; i < rows; ++i)
                {
                    Data::Value::ArrayType row;
                    for (int64_t j = 0; j < cols; ++j)
                    {
                        row.push_back(fillVal);
                    }
                    result.push_back(Data::Value(row));
                }
                return makeArrayVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::MATRIX_NEW), matrix_new_fn);

            // مصفوفة_وحدة / identity_matrix — إنشاء مصفوفة وحدة n×n
            auto identity_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isNumeric())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                int64_t n = args[0]->toInt();
                if (n <= 0)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                Data::Value::ArrayType result;
                for (int64_t i = 0; i < n; ++i)
                {
                    Data::Value::ArrayType row;
                    for (int64_t j = 0; j < n; ++j)
                    {
                        row.push_back(Data::Value(static_cast<int64_t>(i == j ? 1 : 0)));
                    }
                    result.push_back(Data::Value(row));
                }
                return makeArrayVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::MATRIX_IDENTITY), identity_fn);

            // نطاق_مصفوفة / arange — إنشاء مصفوفة أرقام متتالية [start, start+1, ..., end-1]
            auto arange_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                int64_t start = 0, end = 0, step = 1;
                if (args.size() == 1)
                {
                    end = args[0]->toInt();
                }
                else if (args.size() >= 2)
                {
                    start = args[0]->toInt();
                    end = args[1]->toInt();
                    if (args.size() >= 3)
                        step = args[2]->toInt();
                }
                if (step == 0)
                    ctx.error(::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED);
                Data::Value::ArrayType result;
                if (step > 0)
                {
                    for (int64_t i = start; i < end; i += step)
                        result.push_back(Data::Value(i));
                }
                else
                {
                    for (int64_t i = start; i > end; i += step)
                        result.push_back(Data::Value(i));
                }
                return makeArrayVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::ARANGE), arange_fn);

            // أبعاد / shape — إرجاع أبعاد مصفوفة كدالة مستقلة
            auto shape_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                Data::Value::ArrayType dims;
                const Data::Value::ArrayType *current = &args[0]->toArrayRef();
                while (true)
                {
                    dims.push_back(Data::Value(static_cast<int64_t>(current->size())));
                    if (!current->empty() && (*current)[0].isArray())
                    {
                        current = &((*current)[0].toArrayRef());
                    }
                    else
                    {
                        break;
                    }
                }
                return makeArrayVal(dims);
            };
            fm.registerBuiltinFunction(std::string(Bmp::SHAPE), shape_fn);

            // قلب_محوري / transpose — تبديل صفوف وأعمدة مصفوفة ثنائية الأبعاد
            auto transpose_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                if (args.empty() || !args[0]->isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                const auto &arr = args[0]->toArrayRef();
                if (arr.empty())
                    return makeArrayVal(Data::Value::ArrayType{});
                if (!arr[0].isArray())
                    ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                size_t rows = arr.size();
                size_t cols = arr[0].toArrayRef().size();
                Data::Value::ArrayType result;
                for (size_t c = 0; c < cols; ++c)
                {
                    Data::Value::ArrayType newRow;
                    for (size_t r = 0; r < rows; ++r)
                    {
                        const auto &row = arr[r].toArrayRef();
                        newRow.push_back(c < row.size() ? row[c] : Data::Value());
                    }
                    result.push_back(Data::Value(newRow));
                }
                return makeArrayVal(result);
            };
            fm.registerBuiltinFunction(std::string(Bmp::TRANSPOSE), transpose_fn);

        } // registerBuiltinsMapsArrays

    } // namespace Interpreter
} // namespace Sad
