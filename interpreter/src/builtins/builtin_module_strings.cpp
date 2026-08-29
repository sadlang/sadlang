/**
 * @file builtin_module_strings.cpp
 * @brief (AR) وحدة النصوص — دوال المصفوفات والنصوص والتحقق من الأنواع والرياضيات الأساسية
 * @brief (EN) Strings module — array/string manipulation, type checking, basic math
 *
 * @details
 * (AR) الأقسام:
 *   1. دوال المصفوفات (أضف، أزل، فرز، عكس...)
 *   2. دوال النصوص (قسم، صل، استبدل، استخراج...)
 *   3. دوال التحقق من الأنواع (هو_رقم_صحيح، هو_نص...)
 *   4. دوال الرياضيات الأساسية (أكبر، أصغر، القيمة_المطلقة...)
 *
 * @note يتطلب: builtin_common.h (الـ headers المشتركة)
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "builtin_common.h"
#include "builtin_registry.h"
#include "builtins/math_min_max_sign_aware.h"
#include <algorithm>
#include <cmath>
#include <condition_variable>
#include <cstdlib>
#include <fstream>

namespace Sad
{
    namespace Interpreter
    {

        using namespace StdLib;

        // (AR) اختصارات لأسماء الدوال المركزية
        namespace Bs = Builtins::Names::Strings;
        namespace Ba = Builtins::Names::Arrays;
        namespace Bm = Builtins::Names::Math;

        // (AR) أصغر/أكبر بوعي الإشارة: وُحِّد في رأسٍ مشترك (مصدرٌ واحد؛ كان مكرَّرًا هنا وفي
        //      stdlib_manager المسارِ الحيّ). يُستدعى أدناه في تسجيلِ أكبر/أصغر.
        // (EN) Sign-aware min/max: unified into a shared header (single source; was duplicated here
        //      and in stdlib_manager, the live path). Used below when registering أكبر/أصغر.
        using Sad::Interpreter::mathMinMaxSignAware;

        void registerBuiltinsStrings(Interpreter &interpreter)
        {
            // ═══════════════════════════════════════════════════════════════
            // (AR) دوال الإدخال والإخراج / (EN) I/O Functions

            // ═══════════════════════════════════════════════════════════════

            // ═══════════════════════════════════════════════════════════════
            // (AR) دوال المصفوفات والقوائم / (EN) Array/List Functions

            // ═══════════════════════════════════════════════════════════════
            auto append_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                return BuiltinFunctions::append(args);
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::APPEND), append_func);

            // (AR) أزل — حذف عنصر من مصفوفة
            // (EN) remove — remove element from array
            auto remove_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                return BuiltinFunctions::remove(args);
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::REMOVE_ELEM), remove_func);

            // ═══════════════════════════════════════════════════════════════
            // (AR) دوال النصوص / (EN) String Functions

            // ═══════════════════════════════════════════════════════════════

            // ═══════════════════════════════════════════════════════════════
            // (AR) دوال النوع والتحويل / (EN) Type & Conversion Functions

            // ═══════════════════════════════════════════════════════════════
            auto to_int_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                return BuiltinFunctions::to_int(args);
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::CONVERT_TO_INT), to_int_func);

            // (AR) نص — تحويل قيمة إلى نص
            // (EN) to_string — convert value to string
            auto to_string_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                return BuiltinFunctions::to_string(args);
            };

            // (AR) تسجيل دالة التحويل إلى نص بجميع الأسماء المحتملة
            // (EN) Register string conversion function with all possible names

            // ═══════════════════════════════════════════════════════════════
            // دوال الأنواع المتقدمة / Advanced Type Functions

            // ═══════════════════════════════════════════════════════════════
            // TODO 1: toArray / لمصفوفة
            auto to_array_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::toArray(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::TO_ARRAY), to_array_func);

            // TODO 2: toBool / لمنطقي
            auto to_bool_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::toBool(plainArgs));
            };

            // TODO 3: isInt / هو_رقم_صحيح
            auto is_int_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::isInt(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::IS_INT), is_int_func);

            // TODO 4: isFloat / هو_رقم_عشري
            auto is_float_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::isFloat(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::IS_FLOAT), is_float_func);

            // TODO 5: isString / هو_نص
            auto is_string_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::isString(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::IS_STRING), is_string_func);

            // TODO 6: isArray / هو_مصفوفة
            auto is_array_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::isArray(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::IS_ARRAY), is_array_func);

            // ═══════════════════════════════════════════════════════════════
            // دوال النصوص المتقدمة / Advanced String Functions

            // ═══════════════════════════════════════════════════════════════
            // TODO 1: length / طول (String length - Unicode-aware)
            auto string_length_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::String::StringFunctions::length(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::STR_LENGTH), string_length_func);

            // TODO 2: find / بحث (Find substring)
            auto string_find_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::String::StringFunctions::find(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::FIND), string_find_func);
            // TODO 3: replace / استبدل (Replace substring)
            auto string_replace_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::String::StringFunctions::replace(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::REPLACE), string_replace_func);

            // TODO 4: substring / استخراج (Extract substring)
            auto string_substring_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::String::StringFunctions::substring(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::SUBSTRING), string_substring_func);
            // TODO 5: toLower / تحويل_صغير (Convert to lowercase)
            auto string_tolower_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::String::StringFunctions::toLower(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::TO_LOWER), string_tolower_func);
            // TODO 6: toUpper / تحويل_كبير (Convert to uppercase)
            auto string_toupper_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::String::StringFunctions::toUpper(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::TO_UPPER), string_toupper_func);
            // TODO 7: trim / قص_أطراف (Remove leading/trailing whitespace)
            auto string_trim_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::String::StringFunctions::trim(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::TRIM), string_trim_func);
            // TODO 8: split / تقسيم (Split string into array)
            auto string_split_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::String::StringFunctions::split(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::SPLIT), string_split_func);

            // TODO 9: join / دمج (Join array elements into string)
            auto string_join_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::String::StringFunctions::join(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::JOIN), string_join_func);
            // TODO 10: startsWith / يبدأ_ب (Check if starts with prefix)
            auto string_startswith_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::String::StringFunctions::startsWith(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::STARTS_WITH), string_startswith_func);
            // TODO 11: endsWith / ينتهي_ب (Check if ends with suffix)
            auto string_endswith_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::String::StringFunctions::endsWith(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::ENDS_WITH), string_endswith_func);
            // TODO 12: contains / يحتوي_على (Check if contains substring)
            auto string_contains_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::String::StringFunctions::contains(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bs::CONTAINS), string_contains_func);

            // ═══════════════════════════════════════════════════════════════
            // دوال المصفوفات المتقدمة / Advanced Array Functions

            // ═══════════════════════════════════════════════════════════════
            // TODO 1: append / إضافة (Add element to array)
            auto array_append_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::append(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Ba::ADD), array_append_func);

            // TODO 2: pop / إزالة (Remove last element)
            auto array_pop_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::pop(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Ba::REMOVE), array_pop_func);

            // TODO 3: size / حجم (Get array size)
            auto array_size_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::size(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Ba::SIZE), array_size_func);

            // TODO 4: indexOf / فهرس (Find element index)
            auto array_indexof_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::indexOf(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Ba::INDEX_OF), array_indexof_func);

            // TODO 5: contains / يحتوي (Check if contains element)
            auto array_contains_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::contains(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Ba::ARRAY_CONTAINS), array_contains_func);

            // TODO 6: reverse / قلب (Reverse array)
            auto array_reverse_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::reverse(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Ba::REVERSE), array_reverse_func);

            // TODO 7: sort / فرز (Sort array)
            auto array_sort_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::sort(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Ba::SORT), array_sort_func);

            // TODO 8: first / أول (Get first element)
            auto array_first_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::first(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Ba::FIRST), array_first_func);

            // TODO 9: last / آخر (Get last element)
            auto array_last_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::last(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Ba::LAST), array_last_func);

            // TODO 10: slice / شريحة (Extract subarray)
            auto array_slice_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::slice(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Ba::SLICE), array_slice_func);

            // ═══════════════════════════════════════════════════════════════
            // الدوال الرياضية المتقدمة / Advanced Math Functions

            // ═══════════════════════════════════════════════════════════════
            // TODO 1: sqrt / جذر (Square root)
            auto math_sqrt_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::sqrt(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::SQRT), math_sqrt_func);
            // TODO 2: power / أس (Power)
            auto math_power_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::power(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::POWER), math_power_func);

            // TODO 3: abs / مطلق (Absolute value) - Enhanced registration
            auto math_abs_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::abs(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::ABS), math_abs_func);

            // TODO 4: max / أكبر (Maximum) - Enhanced registration (sign-aware: طبيعي ⇒ لا-موقَّع)
            auto math_max_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                return mathMinMaxSignAware(ctx, /*isMax=*/true);
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::MAX), math_max_func);

            // TODO 5: min / أصغر (Minimum) - Enhanced registration (sign-aware: طبيعي ⇒ لا-موقَّع)
            auto math_min_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                return mathMinMaxSignAware(ctx, /*isMax=*/false);
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::MIN), math_min_func);

            // TODO 6: round / تقريب (Round)
            auto math_round_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::round(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::ROUND), math_round_func);

            // TODO 7: floor / أرضية (Floor)
            auto math_floor_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::floor(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::FLOOR), math_floor_func);

            // TODO 8: ceil / سقف (Ceiling)
            auto math_ceil_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::ceil(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::CEIL), math_ceil_func);

            // TODO 9: square / مربع (Square)
            auto math_square_func = [](Sad::Interpreter::BuiltinContext &ctx)
            {
                const auto &args = ctx.args(); (void)args;
                std::vector<Data::Value> plainArgs;
                for (const auto &arg : args)
                    plainArgs.push_back(*arg);
                return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::square(plainArgs));
            };

            interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bm::SQUARE), math_square_func);

            // TODO 10: sin / جيب (Sine)

            // TODO 11: cos / جيب_تمام (Cosine)

            // TODO 12: tan / ظل (Tangent)

            // ═══════════════════════════════════════════════════════════════
            // Additional I/O Functions (println, readLine, clear)

            // ═══════════════════════════════════════════════════════════════
            // println - طباعة مع سطر جديد

            // readLine - قراءة سطر

            // clear - مسح الشاشة

            // ═══════════════════════════════════════════════════════════════
            // Other Utility Functions (random, sleep, exit, assert)

            // ═══════════════════════════════════════════════════════════════
            // random - توليد رقم عشوائي

            // sleep - إيقاف التنفيذ مؤقتا
        }

    } // namespace Interpreter
} // namespace Sad
