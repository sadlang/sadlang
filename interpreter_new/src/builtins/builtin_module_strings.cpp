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
#include <algorithm>
#include <cmath>
#include <condition_variable>
#include <cstdlib>
#include <fstream>

namespace Sad {
namespace Interpreter {

using namespace StdLib;

void registerBuiltinsStrings(Interpreter& interpreter) {
    
    // ═══════════════════════════════════════════════════════════════
    // (AR) دوال الإدخال والإخراج / (EN) I/O Functions
    
    // ═══════════════════════════════════════════════════════════════
    
    // ═══════════════════════════════════════════════════════════════
    // (AR) دوال المصفوفات والقوائم / (EN) Array/List Functions
    
    // ═══════════════════════════════════════════════════════════════
    auto append_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::append(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أضف", append_func);
    interpreter.getFunctionManager().registerBuiltinFunction("append", append_func);
    
    // (AR) أزل — حذف عنصر من مصفوفة
    // (EN) remove — remove element from array
    auto remove_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::remove(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أزل", remove_func);
    interpreter.getFunctionManager().registerBuiltinFunction("remove", remove_func);
    
    
    // ═══════════════════════════════════════════════════════════════
    // (AR) دوال النصوص / (EN) String Functions
    
    // ═══════════════════════════════════════════════════════════════
    auto str_len_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::str_len(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نص_طول", str_len_func);
    interpreter.getFunctionManager().registerBuiltinFunction("str_len", str_len_func);
    // (AR) تحويل_كبير — تحويل نص إلى أحرف كبيرة
    // (EN) to_upper — convert text to uppercase
    auto upper_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::upper(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أحرف_كبيرة", upper_func);
    interpreter.getFunctionManager().registerBuiltinFunction("upper", upper_func);
    
    // (AR) تحويل_صغير — تحويل نص إلى أحرف صغيرة
    // (EN) to_lower — convert text to lowercase
    auto lower_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::lower(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أحرف_صغيرة", lower_func);
    interpreter.getFunctionManager().registerBuiltinFunction("lower", lower_func);
    
    // (AR) تقسيم — تقسيم نص إلى مصفوفة
    // (EN) split — split text into array
    auto split_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::split(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("قسم", split_func);
    interpreter.getFunctionManager().registerBuiltinFunction("split", split_func);
    
    // (AR) صل — دمج مصفوفة في نص واحد
    // (EN) join — join array elements into string
    auto join_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::join(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("صل", join_func);
    interpreter.getFunctionManager().registerBuiltinFunction("join", join_func);
    
    
    // ═══════════════════════════════════════════════════════════════
    // (AR) الدوال الرياضية / (EN) Math Functions
    
    // ═══════════════════════════════════════════════════════════════
    auto abs_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::abs(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("القيمة_المطلقة", abs_func);
    interpreter.getFunctionManager().registerBuiltinFunction("abs", abs_func);
    
    // (AR) أكبر — إرجاع أكبر قيمة من مصفوفة
    // (EN) max — return maximum value from array
    auto max_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::max(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أكبر", max_func);
    interpreter.getFunctionManager().registerBuiltinFunction("max", max_func);
    
    // (AR) أصغر — إرجاع أصغر قيمة من مصفوفة
    // (EN) min — return minimum value from array
    auto min_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::min(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أصغر", min_func);
    interpreter.getFunctionManager().registerBuiltinFunction("min", min_func);
    
    // (AR) مجموع — حساب مجموع عناصر مصفوفة
    // (EN) sum — calculate sum of array elements
    auto sum_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::sum(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("جمع", sum_func);
    
    // (AR) جذر — حساب الجذر التربيعي
    // (EN) sqrt — calculate square root
    auto sqrt_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::sqrt(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("الجذر", sqrt_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sqrt", sqrt_func);
    
    
    // ═══════════════════════════════════════════════════════════════
    // (AR) دوال النوع والتحويل / (EN) Type & Conversion Functions
    
    // ═══════════════════════════════════════════════════════════════
    auto to_int_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::to_int(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("حول_رقم", to_int_func);
    
    
    // (AR) نص — تحويل قيمة إلى نص
    // (EN) to_string — convert value to string
    auto to_string_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::to_string(args);
    };
    
    // (AR) تسجيل دالة التحويل إلى نص بجميع الأسماء المحتملة
    // (EN) Register string conversion function with all possible names
    interpreter.getFunctionManager().registerBuiltinFunction("string", to_string_func);
    
    
    // ═══════════════════════════════════════════════════════════════
    // دوال الأنواع المتقدمة / Advanced Type Functions
    
    // ═══════════════════════════════════════════════════════════════
    // TODO 1: toArray / لمصفوفة
    auto to_array_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::toArray(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("لمصفوفة", to_array_func);
    interpreter.getFunctionManager().registerBuiltinFunction("toArray", to_array_func);
    interpreter.getFunctionManager().registerBuiltinFunction("to_array", to_array_func);
    
    // TODO 2: toBool / لمنطقي
    auto to_bool_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::toBool(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("toBool", to_bool_func);
    interpreter.getFunctionManager().registerBuiltinFunction("boolean", to_bool_func);
    
    // TODO 3: isInt / هو_رقم_صحيح
    auto is_int_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::isInt(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("هو_رقم_صحيح", is_int_func);
    interpreter.getFunctionManager().registerBuiltinFunction("isInt", is_int_func);
    interpreter.getFunctionManager().registerBuiltinFunction("is_int", is_int_func);
    interpreter.getFunctionManager().registerBuiltinFunction("isInteger", is_int_func);
    
    // TODO 4: isFloat / هو_رقم_عشري
    auto is_float_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::isFloat(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("هو_رقم_عشري", is_float_func);
    interpreter.getFunctionManager().registerBuiltinFunction("isFloat", is_float_func);
    interpreter.getFunctionManager().registerBuiltinFunction("is_float", is_float_func);
    interpreter.getFunctionManager().registerBuiltinFunction("isDouble", is_float_func);
    
    // TODO 5: isString / هو_نص
    auto is_string_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::isString(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("هو_نص", is_string_func);
    interpreter.getFunctionManager().registerBuiltinFunction("isString", is_string_func);
    interpreter.getFunctionManager().registerBuiltinFunction("is_string", is_string_func);
    interpreter.getFunctionManager().registerBuiltinFunction("isStr", is_string_func);
    
    // TODO 6: isArray / هو_مصفوفة
    auto is_array_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::isArray(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("هو_مصفوفة", is_array_func);
    interpreter.getFunctionManager().registerBuiltinFunction("isArray", is_array_func);
    interpreter.getFunctionManager().registerBuiltinFunction("is_array", is_array_func);
    interpreter.getFunctionManager().registerBuiltinFunction("isList", is_array_func);
    
    
    // ═══════════════════════════════════════════════════════════════
    // دوال النصوص المتقدمة / Advanced String Functions
    
    // ═══════════════════════════════════════════════════════════════
    // TODO 1: length / طول (String length - Unicode-aware)
    auto string_length_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::length(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("طول_نص", string_length_func);
    interpreter.getFunctionManager().registerBuiltinFunction("string_length", string_length_func);
    interpreter.getFunctionManager().registerBuiltinFunction("str_length", string_length_func);
    
    // TODO 2: find / بحث (Find substring)
    auto string_find_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::find(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("بحث", string_find_func);
    interpreter.getFunctionManager().registerBuiltinFunction("find", string_find_func);
    interpreter.getFunctionManager().registerBuiltinFunction("indexOf", string_find_func);
    // TODO 3: replace / استبدل (Replace substring)
    auto string_replace_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::replace(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("استبدل", string_replace_func);
    interpreter.getFunctionManager().registerBuiltinFunction("replace", string_replace_func);
    interpreter.getFunctionManager().registerBuiltinFunction("str_replace", string_replace_func);
    
    // TODO 4: substring / استخراج (Extract substring)
    auto string_substring_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::substring(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("استخراج", string_substring_func);
    interpreter.getFunctionManager().registerBuiltinFunction("substring", string_substring_func);
    interpreter.getFunctionManager().registerBuiltinFunction("substr", string_substring_func);
    // TODO 5: toLower / تحويل_صغير (Convert to lowercase)
    auto string_tolower_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::toLower(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("تحويل_صغير", string_tolower_func);
    interpreter.getFunctionManager().registerBuiltinFunction("toLower", string_tolower_func);
    interpreter.getFunctionManager().registerBuiltinFunction("lowercase", string_tolower_func);
    // TODO 6: toUpper / تحويل_كبير (Convert to uppercase)
    auto string_toupper_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::toUpper(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("تحويل_كبير", string_toupper_func);
    interpreter.getFunctionManager().registerBuiltinFunction("toUpper", string_toupper_func);
    interpreter.getFunctionManager().registerBuiltinFunction("uppercase", string_toupper_func);
    // TODO 7: trim / قص_أطراف (Remove leading/trailing whitespace)
    auto string_trim_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::trim(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("قص_أطراف", string_trim_func);
    interpreter.getFunctionManager().registerBuiltinFunction("trim", string_trim_func);
    interpreter.getFunctionManager().registerBuiltinFunction("strip", string_trim_func);
    // TODO 8: split / تقسيم (Split string into array)
    auto string_split_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::split(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("تقسيم", string_split_func);
    interpreter.getFunctionManager().registerBuiltinFunction("split_string", string_split_func);
    interpreter.getFunctionManager().registerBuiltinFunction("str_split", string_split_func);
    
    // TODO 9: join / دمج (Join array elements into string)
    auto string_join_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::join(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("دمج", string_join_func);
    interpreter.getFunctionManager().registerBuiltinFunction("join_strings", string_join_func);
    interpreter.getFunctionManager().registerBuiltinFunction("str_join", string_join_func);
    // TODO 10: startsWith / يبدأ_ب (Check if starts with prefix)
    auto string_startswith_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::startsWith(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("يبدأ_ب", string_startswith_func);
    interpreter.getFunctionManager().registerBuiltinFunction("startsWith", string_startswith_func);
    interpreter.getFunctionManager().registerBuiltinFunction("starts_with", string_startswith_func);
    // TODO 11: endsWith / ينتهي_ب (Check if ends with suffix)
    auto string_endswith_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::endsWith(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("ينتهي_ب", string_endswith_func);
    interpreter.getFunctionManager().registerBuiltinFunction("endsWith", string_endswith_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ends_with", string_endswith_func);
    // TODO 12: contains / يحتوي_على (Check if contains substring)
    auto string_contains_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::contains(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("يحتوي_على", string_contains_func);
    interpreter.getFunctionManager().registerBuiltinFunction("contains", string_contains_func);
    interpreter.getFunctionManager().registerBuiltinFunction("str_contains", string_contains_func);
    
    // ═══════════════════════════════════════════════════════════════
    // دوال المصفوفات المتقدمة / Advanced Array Functions
    
    // ═══════════════════════════════════════════════════════════════
    // TODO 1: append / إضافة (Add element to array)
    auto array_append_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::append(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("إضافة_عنصر", array_append_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_append", array_append_func);
    interpreter.getFunctionManager().registerBuiltinFunction("arr_append", array_append_func);
    
    // TODO 2: pop / إزالة (Remove last element)
    auto array_pop_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::pop(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("إزالة_عنصر", array_pop_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_pop", array_pop_func);
    interpreter.getFunctionManager().registerBuiltinFunction("arr_pop", array_pop_func);
    
    // TODO 3: size / حجم (Get array size)
    auto array_size_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::size(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("حجم_مصفوفة", array_size_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_size", array_size_func);
    interpreter.getFunctionManager().registerBuiltinFunction("arr_size", array_size_func);
    
    // TODO 4: indexOf / فهرس (Find element index)
    auto array_indexof_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::indexOf(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("indexOf", array_indexof_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_indexOf", array_indexof_func);
    interpreter.getFunctionManager().registerBuiltinFunction("findIndex", array_indexof_func);
    
    // TODO 5: contains / يحتوي (Check if contains element)
    auto array_contains_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::contains(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("يحتوي_عنصر", array_contains_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_contains", array_contains_func);
    interpreter.getFunctionManager().registerBuiltinFunction("arr_contains", array_contains_func);
    
    // TODO 6: reverse / قلب (Reverse array)
    auto array_reverse_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::reverse(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("قلب", array_reverse_func);
    interpreter.getFunctionManager().registerBuiltinFunction("reverse", array_reverse_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_reverse", array_reverse_func);
    
    // TODO 7: sort / فرز (Sort array)
    auto array_sort_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::sort(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("فرز", array_sort_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_sort", array_sort_func);
    
    // TODO 8: first / أول (Get first element)
    auto array_first_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::first(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أول", array_first_func);
    interpreter.getFunctionManager().registerBuiltinFunction("first", array_first_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_first", array_first_func);
    
    // TODO 9: last / آخر (Get last element)
    auto array_last_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::last(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("آخر", array_last_func);
    interpreter.getFunctionManager().registerBuiltinFunction("last", array_last_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_last", array_last_func);
    
    // TODO 10: slice / شريحة (Extract subarray)
    auto array_slice_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::slice(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("شريحة", array_slice_func);
    interpreter.getFunctionManager().registerBuiltinFunction("slice", array_slice_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_slice", array_slice_func);
    
    
    // ═══════════════════════════════════════════════════════════════
    // الدوال الرياضية المتقدمة / Advanced Math Functions
    
    // ═══════════════════════════════════════════════════════════════
    // TODO 1: sqrt / جذر (Square root)
    auto math_sqrt_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::sqrt(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("جذر", math_sqrt_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sqrt", math_sqrt_func);
    // TODO 2: power / أس (Power)
    auto math_power_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::power(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أس", math_power_func);
    interpreter.getFunctionManager().registerBuiltinFunction("power", math_power_func);
    interpreter.getFunctionManager().registerBuiltinFunction("pow", math_power_func);
    
    // TODO 3: abs / مطلق (Absolute value) - Enhanced registration
    auto math_abs_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::abs(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("مطلق", math_abs_func);
    interpreter.getFunctionManager().registerBuiltinFunction("absolute", math_abs_func);
    
    // TODO 4: max / أكبر (Maximum) - Enhanced registration
    auto math_max_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::max(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("maximum", math_max_func);
    interpreter.getFunctionManager().registerBuiltinFunction("أقصى", math_max_func);
    interpreter.getFunctionManager().registerBuiltinFunction("max", math_max_func);
    
    // TODO 5: min / أصغر (Minimum) - Enhanced registration
    auto math_min_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::min(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("minimum", math_min_func);
    interpreter.getFunctionManager().registerBuiltinFunction("أدنى", math_min_func);
    interpreter.getFunctionManager().registerBuiltinFunction("min", math_min_func);
    
    // TODO 6: round / تقريب (Round)
    auto math_round_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::round(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("تقريب", math_round_func);
    interpreter.getFunctionManager().registerBuiltinFunction("round", math_round_func);
    
    // TODO 7: floor / أرضية (Floor)
    auto math_floor_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::floor(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أرضية", math_floor_func);
    interpreter.getFunctionManager().registerBuiltinFunction("floor", math_floor_func);
    
    // TODO 8: ceil / سقف (Ceiling)
    auto math_ceil_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::ceil(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("سقف", math_ceil_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ceil", math_ceil_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ceiling", math_ceil_func);
    
    // TODO 9: square / مربع (Square)
    auto math_square_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::square(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("مربع", math_square_func);
    interpreter.getFunctionManager().registerBuiltinFunction("square", math_square_func);
    
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
