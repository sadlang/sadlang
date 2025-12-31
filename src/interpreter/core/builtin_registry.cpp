/**
 * @file builtin_registry.cpp
 * @brief (AR) تسجيل الدوال المضمنة في بيئة المفسر
 * @brief (EN) Register built-in functions in interpreter environment
 * 
 * هذا الملف يسجل جميع الدوال المضمنة المتاحة بنسختيهما
 * العربية والإنجليزية.
 * 
 * This file registers all available built-in functions in both
 * Arabic and English versions.
 * 
 * @author S Language Development Team
 * @date November 22, 2025
 * @license MIT License
 */

#include "../../../include/stdlib/core/builtins.h"
#include "../../../include/interpreter/core/interpreter_core.h"
#include "../../../include/stdlib/io/io_functions.h"
#include "../../../include/stdlib/core/type_functions.h"
#include "../../../include/stdlib/core/array_functions.h"
#include "../../../include/stdlib/core/other_functions.h"
#include "../../../include/stdlib/string/string_functions.h"
#include "../../../include/stdlib/math/math_functions.h"
#include "../../../include/stdlib/filesystem_module.h"
#include "../../stdlib/graphics/graphics_module.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace Sad {
namespace Interpreter {

using namespace StdLib;

/**
 * @brief (AR) تسجيل جميع الدوال المضمنة في المفسر
 * @brief (EN) Register all built-in functions in interpreter
 * 
 * @param interpreter (AR) مرجع للمفسر / (EN) Reference to interpreter
 */
void registerBuiltinFunctions(Interpreter& interpreter) {
    // ===================================================================
    // (AR) دوال الإدخال والإخراج / (EN) I/O Functions
    // ===================================================================
    
   
    auto print_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        
        return BuiltinFunctions::print(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("اطبع", print_func);
    interpreter.getFunctionManager().registerBuiltinFunction("print", print_func);
    interpreter.getFunctionManager().registerBuiltinFunction("إطبع", print_func);
 
    
    auto input_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::input(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("اقرأ", input_func);
    interpreter.getFunctionManager().registerBuiltinFunction("input", input_func);
    
    // ===================================================================
    // (AR) دوال المصفوفات والقوائم / (EN) Array/List Functions
    // ===================================================================
    
    auto len_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::length(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("الطول", len_func);
    interpreter.getFunctionManager().registerBuiltinFunction("طول", len_func);
    interpreter.getFunctionManager().registerBuiltinFunction("length", len_func);
    interpreter.getFunctionManager().registerBuiltinFunction("len", len_func);
    
    auto append_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::append(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أضف", append_func);
    interpreter.getFunctionManager().registerBuiltinFunction("append", append_func);
    
    auto remove_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::remove(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أزل", remove_func);
    interpreter.getFunctionManager().registerBuiltinFunction("remove", remove_func);
    
    // ===================================================================
    // (AR) دوال النصوص / (EN) String Functions
    // ===================================================================
    
    auto str_len_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::str_len(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نص_طول", str_len_func);
    interpreter.getFunctionManager().registerBuiltinFunction("str_len", str_len_func);
    interpreter.getFunctionManager().registerBuiltinFunction("نص_الطول", str_len_func);
    
    auto upper_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::upper(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أحرف_كبيرة", upper_func);
    interpreter.getFunctionManager().registerBuiltinFunction("upper", upper_func);
    
    auto lower_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::lower(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أحرف_صغيرة", lower_func);
    interpreter.getFunctionManager().registerBuiltinFunction("lower", lower_func);
    
    auto split_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::split(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("قسّم", split_func);
    interpreter.getFunctionManager().registerBuiltinFunction("split", split_func);
    
    auto join_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::join(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("صل", join_func);
    interpreter.getFunctionManager().registerBuiltinFunction("join", join_func);
    
    // ===================================================================
    // (AR) الدوال الرياضية / (EN) Math Functions
    // ===================================================================
    
    auto abs_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::abs(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("القيمة_المطلقة", abs_func);
    interpreter.getFunctionManager().registerBuiltinFunction("abs", abs_func);
    
    auto max_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::max(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أكبر", max_func);
    interpreter.getFunctionManager().registerBuiltinFunction("max", max_func);
    
    auto min_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::min(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أصغر", min_func);
    interpreter.getFunctionManager().registerBuiltinFunction("min", min_func);
    
    auto sum_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::sum(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("جمع", sum_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sum", sum_func);
    
    auto sqrt_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::sqrt(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("الجذر", sqrt_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sqrt", sqrt_func);
    
    // ===================================================================
    // (AR) دوال النوع والتحويل / (EN) Type & Conversion Functions
    // ===================================================================
    
    auto type_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::type_of(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("النوع", type_func);
    interpreter.getFunctionManager().registerBuiltinFunction("type", type_func);
    
    auto to_int_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::to_int(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("لرقم", to_int_func);
    interpreter.getFunctionManager().registerBuiltinFunction("to_int", to_int_func);
    interpreter.getFunctionManager().registerBuiltinFunction("int", to_int_func);
    
    auto to_float_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::to_float(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("لعشري", to_float_func);
    interpreter.getFunctionManager().registerBuiltinFunction("to_float", to_float_func);
    interpreter.getFunctionManager().registerBuiltinFunction("float", to_float_func);
    
    auto to_string_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::to_string(args);
    };
    
    // (AR) تسجيل دالة التحويل إلى نص بجميع الأسماء المحتملة
    // (EN) Register string conversion function with all possible names
    interpreter.getFunctionManager().registerBuiltinFunction("لنص", to_string_func);
    interpreter.getFunctionManager().registerBuiltinFunction("نص", to_string_func);      // Most common: نص()
    interpreter.getFunctionManager().registerBuiltinFunction("to_string", to_string_func);
    interpreter.getFunctionManager().registerBuiltinFunction("str", to_string_func);
    interpreter.getFunctionManager().registerBuiltinFunction("string", to_string_func);
    
    // ===================================================================
    // ✅ المرحلة 16: دوال الأنواع المتقدمة / Phase 16: Advanced Type Functions
    // ===================================================================
    
    // ✅ TODO 1: toArray / لمصفوفة
    auto to_array_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::toArray(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("لمصفوفة", to_array_func);
    interpreter.getFunctionManager().registerBuiltinFunction("toArray", to_array_func);
    interpreter.getFunctionManager().registerBuiltinFunction("to_array", to_array_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array", to_array_func);
    
    // ✅ TODO 2: toBool / لمنطقي
    auto to_bool_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::toBool(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("لمنطقي", to_bool_func);
    interpreter.getFunctionManager().registerBuiltinFunction("toBool", to_bool_func);
    interpreter.getFunctionManager().registerBuiltinFunction("to_bool", to_bool_func);
    interpreter.getFunctionManager().registerBuiltinFunction("bool", to_bool_func);
    interpreter.getFunctionManager().registerBuiltinFunction("boolean", to_bool_func);
    
    // ✅ TODO 3: isInt / هو_رقم_صحيح
    auto is_int_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::isInt(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("هو_رقم_صحيح", is_int_func);
    interpreter.getFunctionManager().registerBuiltinFunction("هو_رقم", is_int_func);
    interpreter.getFunctionManager().registerBuiltinFunction("هل_عدد_صحيح", is_int_func);
    interpreter.getFunctionManager().registerBuiltinFunction("isInt", is_int_func);
    interpreter.getFunctionManager().registerBuiltinFunction("is_int", is_int_func);
    interpreter.getFunctionManager().registerBuiltinFunction("isInteger", is_int_func);
    
    // ✅ TODO 4: isFloat / هو_رقم_عشري
    auto is_float_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::isFloat(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("هو_رقم_عشري", is_float_func);
    interpreter.getFunctionManager().registerBuiltinFunction("هو_عشري", is_float_func);
    interpreter.getFunctionManager().registerBuiltinFunction("هل_عدد_عشري", is_float_func);
    interpreter.getFunctionManager().registerBuiltinFunction("isFloat", is_float_func);
    interpreter.getFunctionManager().registerBuiltinFunction("is_float", is_float_func);
    interpreter.getFunctionManager().registerBuiltinFunction("isDouble", is_float_func);
    
    // ✅ TODO 5: isString / هو_نص
    auto is_string_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::isString(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("هو_نص", is_string_func);
    interpreter.getFunctionManager().registerBuiltinFunction("هل_نص", is_string_func);
    interpreter.getFunctionManager().registerBuiltinFunction("isString", is_string_func);
    interpreter.getFunctionManager().registerBuiltinFunction("is_string", is_string_func);
    interpreter.getFunctionManager().registerBuiltinFunction("isStr", is_string_func);
    
    // ✅ TODO 6: isArray / هو_مصفوفة
    auto is_array_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::TypeFunctions::isArray(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("هو_مصفوفة", is_array_func);
    interpreter.getFunctionManager().registerBuiltinFunction("هل_مصفوفة", is_array_func);
    interpreter.getFunctionManager().registerBuiltinFunction("isArray", is_array_func);
    interpreter.getFunctionManager().registerBuiltinFunction("is_array", is_array_func);
    interpreter.getFunctionManager().registerBuiltinFunction("isList", is_array_func);
    
    // ===================================================================
    // ✅ المرحلة 17: دوال النصوص المتقدمة / Phase 17: Advanced String Functions
    // ===================================================================
    
    // ✅ TODO 1: length / طول (String length - Unicode-aware)
    auto string_length_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::length(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("طول_نص", string_length_func);
    interpreter.getFunctionManager().registerBuiltinFunction("string_length", string_length_func);
    interpreter.getFunctionManager().registerBuiltinFunction("str_length", string_length_func);
    
    // ✅ TODO 2: find / بحث (Find substring)
    auto string_find_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::find(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("بحث", string_find_func);
    interpreter.getFunctionManager().registerBuiltinFunction("find", string_find_func);
    interpreter.getFunctionManager().registerBuiltinFunction("indexOf", string_find_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ابحث", string_find_func);
    
    // ✅ TODO 3: replace / استبدل (Replace substring)
    auto string_replace_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::replace(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("استبدل", string_replace_func);
    interpreter.getFunctionManager().registerBuiltinFunction("replace", string_replace_func);
    interpreter.getFunctionManager().registerBuiltinFunction("str_replace", string_replace_func);
    
    // ✅ TODO 4: substring / استخراج (Extract substring)
    auto string_substring_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::substring(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("استخراج", string_substring_func);
    interpreter.getFunctionManager().registerBuiltinFunction("substring", string_substring_func);
    interpreter.getFunctionManager().registerBuiltinFunction("substr", string_substring_func);
    interpreter.getFunctionManager().registerBuiltinFunction("قطعة", string_substring_func);
    
    // ✅ TODO 5: toLower / تحويل_صغير (Convert to lowercase)
    auto string_tolower_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::toLower(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("تحويل_صغير", string_tolower_func);
    interpreter.getFunctionManager().registerBuiltinFunction("toLower", string_tolower_func);
    interpreter.getFunctionManager().registerBuiltinFunction("lowercase", string_tolower_func);
    interpreter.getFunctionManager().registerBuiltinFunction("لأصغر", string_tolower_func);
    
    // ✅ TODO 6: toUpper / تحويل_كبير (Convert to uppercase)
    auto string_toupper_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::toUpper(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("تحويل_كبير", string_toupper_func);
    interpreter.getFunctionManager().registerBuiltinFunction("toUpper", string_toupper_func);
    interpreter.getFunctionManager().registerBuiltinFunction("uppercase", string_toupper_func);
    interpreter.getFunctionManager().registerBuiltinFunction("لأكبر", string_toupper_func);
    
    // ✅ TODO 7: trim / قص_أطراف (Remove leading/trailing whitespace)
    auto string_trim_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::trim(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("قص_أطراف", string_trim_func);
    interpreter.getFunctionManager().registerBuiltinFunction("trim", string_trim_func);
    interpreter.getFunctionManager().registerBuiltinFunction("strip", string_trim_func);
    interpreter.getFunctionManager().registerBuiltinFunction("قلّم", string_trim_func);
    
    // ✅ TODO 8: split / تقسيم (Split string into array)
    auto string_split_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::split(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("تقسيم", string_split_func);
    interpreter.getFunctionManager().registerBuiltinFunction("split_string", string_split_func);
    interpreter.getFunctionManager().registerBuiltinFunction("str_split", string_split_func);
    
    // ✅ TODO 9: join / دمج (Join array elements into string)
    auto string_join_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::join(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("دمج", string_join_func);
    interpreter.getFunctionManager().registerBuiltinFunction("join_strings", string_join_func);
    interpreter.getFunctionManager().registerBuiltinFunction("str_join", string_join_func);
    interpreter.getFunctionManager().registerBuiltinFunction("اربط", string_join_func);
    
    // ✅ TODO 10: startsWith / يبدأ_ب (Check if starts with prefix)
    auto string_startswith_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::startsWith(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("يبدأ_ب", string_startswith_func);
    interpreter.getFunctionManager().registerBuiltinFunction("startsWith", string_startswith_func);
    interpreter.getFunctionManager().registerBuiltinFunction("starts_with", string_startswith_func);
    interpreter.getFunctionManager().registerBuiltinFunction("يبدأ_بـ", string_startswith_func);
    
    // ✅ TODO 11: endsWith / ينتهي_ب (Check if ends with suffix)
    auto string_endswith_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::endsWith(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("ينتهي_ب", string_endswith_func);
    interpreter.getFunctionManager().registerBuiltinFunction("endsWith", string_endswith_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ends_with", string_endswith_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ينتهي_بـ", string_endswith_func);
    
    // ✅ TODO 12: contains / يحتوي_على (Check if contains substring)
    auto string_contains_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::String::StringFunctions::contains(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("يحتوي_على", string_contains_func);
    interpreter.getFunctionManager().registerBuiltinFunction("contains", string_contains_func);
    interpreter.getFunctionManager().registerBuiltinFunction("str_contains", string_contains_func);
    interpreter.getFunctionManager().registerBuiltinFunction("يحتوي", string_contains_func);
    
    // ===================================================================
    // ✅ المرحلة 18: دوال المصفوفات المتقدمة / Phase 18: Advanced Array Functions
    // ===================================================================
    
    // ✅ TODO 1: append / إضافة (Add element to array)
    auto array_append_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::append(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("إضافة_عنصر", array_append_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_append", array_append_func);
    interpreter.getFunctionManager().registerBuiltinFunction("arr_append", array_append_func);
    
    // ✅ TODO 2: pop / إزالة (Remove last element)
    auto array_pop_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::pop(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("إزالة_عنصر", array_pop_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_pop", array_pop_func);
    interpreter.getFunctionManager().registerBuiltinFunction("arr_pop", array_pop_func);
    
    // ✅ TODO 3: size / حجم (Get array size)
    auto array_size_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::size(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("حجم_مصفوفة", array_size_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_size", array_size_func);
    interpreter.getFunctionManager().registerBuiltinFunction("arr_size", array_size_func);
    
    // ✅ TODO 4: indexOf / فهرس (Find element index)
    auto array_indexof_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::indexOf(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("فهرس", array_indexof_func);
    interpreter.getFunctionManager().registerBuiltinFunction("indexOf", array_indexof_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_indexOf", array_indexof_func);
    interpreter.getFunctionManager().registerBuiltinFunction("findIndex", array_indexof_func);
    
    // ✅ TODO 5: contains / يحتوي (Check if contains element)
    auto array_contains_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::contains(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("يحتوي_عنصر", array_contains_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_contains", array_contains_func);
    interpreter.getFunctionManager().registerBuiltinFunction("arr_contains", array_contains_func);
    interpreter.getFunctionManager().registerBuiltinFunction("includes", array_contains_func);
    
    // ✅ TODO 6: reverse / قلب (Reverse array)
    auto array_reverse_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::reverse(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("قلب", array_reverse_func);
    interpreter.getFunctionManager().registerBuiltinFunction("reverse", array_reverse_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_reverse", array_reverse_func);
    
    // ✅ TODO 7: sort / فرز (Sort array)
    auto array_sort_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::sort(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("فرز", array_sort_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sort", array_sort_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_sort", array_sort_func);
    
    // ✅ TODO 8: first / أول (Get first element)
    auto array_first_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::first(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أول", array_first_func);
    interpreter.getFunctionManager().registerBuiltinFunction("first", array_first_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_first", array_first_func);
    
    // ✅ TODO 9: last / آخر (Get last element)
    auto array_last_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::last(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("آخر", array_last_func);
    interpreter.getFunctionManager().registerBuiltinFunction("last", array_last_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_last", array_last_func);
    
    // ✅ TODO 10: slice / شريحة (Extract subarray)
    auto array_slice_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::ArrayFunctions::slice(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("شريحة", array_slice_func);
    interpreter.getFunctionManager().registerBuiltinFunction("slice", array_slice_func);
    interpreter.getFunctionManager().registerBuiltinFunction("array_slice", array_slice_func);
    
    // ===================================================================
    // ✅ المرحلة 19: الدوال الرياضية المتقدمة / Phase 19: Advanced Math Functions
    // ===================================================================
    
    // ✅ TODO 1: sqrt / جذر (Square root)
    auto math_sqrt_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::sqrt(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("جذر", math_sqrt_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sqrt", math_sqrt_func);
    interpreter.getFunctionManager().registerBuiltinFunction("الجذر_التربيعي", math_sqrt_func);
    
    // ✅ TODO 2: power / أس (Power)
    auto math_power_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::power(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أس", math_power_func);
    interpreter.getFunctionManager().registerBuiltinFunction("power", math_power_func);
    interpreter.getFunctionManager().registerBuiltinFunction("pow", math_power_func);
    
    // ✅ TODO 3: abs / مطلق (Absolute value) - Enhanced registration
    auto math_abs_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::abs(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("مطلق", math_abs_func);
    interpreter.getFunctionManager().registerBuiltinFunction("absolute", math_abs_func);
    
    // ✅ TODO 4: max / أكبر (Maximum) - Enhanced registration
    auto math_max_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::max(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("maximum", math_max_func);
    
    // ✅ TODO 5: min / أصغر (Minimum) - Enhanced registration
    auto math_min_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::min(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("minimum", math_min_func);
    
    // ✅ TODO 6: round / تقريب (Round)
    auto math_round_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::round(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("تقريب", math_round_func);
    interpreter.getFunctionManager().registerBuiltinFunction("round", math_round_func);
    
    // ✅ TODO 7: floor / أرضية (Floor)
    auto math_floor_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::floor(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أرضية", math_floor_func);
    interpreter.getFunctionManager().registerBuiltinFunction("floor", math_floor_func);
    
    // ✅ TODO 8: ceil / سقف (Ceiling)
    auto math_ceil_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::ceil(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("سقف", math_ceil_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ceil", math_ceil_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ceiling", math_ceil_func);
    
    // ✅ TODO 9: square / مربع (Square)
    auto math_square_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::square(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("مربع", math_square_func);
    interpreter.getFunctionManager().registerBuiltinFunction("square", math_square_func);
    
    // ✅ TODO 10: sin / جيب (Sine)
    auto math_sin_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::sin(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("جيب", math_sin_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sin", math_sin_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sine", math_sin_func);
    
    // ✅ TODO 11: cos / جيب_تمام (Cosine)
    auto math_cos_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::cos(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("جيب_تمام", math_cos_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cos", math_cos_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cosine", math_cos_func);
    
    // ✅ TODO 12: tan / ظل (Tangent)
    auto math_tan_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::MathFunctions::tan(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("ظل", math_tan_func);
    interpreter.getFunctionManager().registerBuiltinFunction("tan", math_tan_func);
    interpreter.getFunctionManager().registerBuiltinFunction("tangent", math_tan_func);
    
    // ===================================================================
    // Phase 20: Additional I/O Functions (println, readLine, clear)
    // ===================================================================
    
    // println - طباعة مع سطر جديد
    auto io_println_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::IO::IOFunctions::println(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("اطبع_سطر", io_println_func);
    interpreter.getFunctionManager().registerBuiltinFunction("println", io_println_func);
    interpreter.getFunctionManager().registerBuiltinFunction("طبع_سطر", io_println_func);
    
    // readLine - قراءة سطر
    auto io_readline_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::IO::IOFunctions::readLine(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("قراءة_سطر", io_readline_func);
    interpreter.getFunctionManager().registerBuiltinFunction("readLine", io_readline_func);
    interpreter.getFunctionManager().registerBuiltinFunction("اقرأ_سطر", io_readline_func);
    interpreter.getFunctionManager().registerBuiltinFunction("readline", io_readline_func);
    
    // clear - مسح الشاشة
    auto io_clear_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::IO::IOFunctions::clear(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("مسح_الشاشة", io_clear_func);
    interpreter.getFunctionManager().registerBuiltinFunction("clear", io_clear_func);
    interpreter.getFunctionManager().registerBuiltinFunction("مسح", io_clear_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cls", io_clear_func);
    
    // ===================================================================
    // Phase 21: Other Utility Functions (random, sleep, exit, assert)
    // ===================================================================
    
    // random - توليد رقم عشوائي
    auto other_random_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::random(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("عشوائي", other_random_func);
    interpreter.getFunctionManager().registerBuiltinFunction("random", other_random_func);
    interpreter.getFunctionManager().registerBuiltinFunction("رقم_عشوائي", other_random_func);
    
    // sleep - إيقاف التنفيذ مؤقتاً
    auto other_sleep_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::sleep(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نم", other_sleep_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sleep", other_sleep_func);
    interpreter.getFunctionManager().registerBuiltinFunction("انتظر", other_sleep_func);
    interpreter.getFunctionManager().registerBuiltinFunction("wait", other_sleep_func);
    
    // exit - إنهاء البرنامج
    auto other_exit_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::exit(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("اخرج", other_exit_func);
    interpreter.getFunctionManager().registerBuiltinFunction("exit", other_exit_func);
    interpreter.getFunctionManager().registerBuiltinFunction("أنه", other_exit_func);
    interpreter.getFunctionManager().registerBuiltinFunction("quit", other_exit_func);
    
    // assert - التحقق من شرط
    auto other_assert_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Core::assert(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("تأكد", other_assert_func);
    interpreter.getFunctionManager().registerBuiltinFunction("assert", other_assert_func);
    interpreter.getFunctionManager().registerBuiltinFunction("تحقق", other_assert_func);
    
    // ===================================================================
    // Phase 22: Filesystem Module Functions (Part 1 - File I/O & Directories)
    // ===================================================================
    
    // read_lines - قراءة أسطر الملف
    auto fs_read_lines_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) throw std::runtime_error("read_lines requires file path");
        std::string path = args[0]->toString();
        auto lines = sad::stdlib::filesystem::read_lines(path);
        std::vector<Data::Value> result;
        for (const auto& line : lines) {
            result.push_back(Data::Value(line));
        }
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("اقرأ_أسطر", fs_read_lines_func);
    interpreter.getFunctionManager().registerBuiltinFunction("read_lines", fs_read_lines_func);
    interpreter.getFunctionManager().registerBuiltinFunction("قراءة_أسطر", fs_read_lines_func);
    
    // append_to_file - إضافة إلى ملف
    auto fs_append_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2) throw std::runtime_error("append_to_file requires path and content");
        std::string path = args[0]->toString();
        std::string content = args[1]->toString();
        sad::stdlib::filesystem::append_to_file(path, content);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أضف_إلى_ملف", fs_append_func);
    interpreter.getFunctionManager().registerBuiltinFunction("append_to_file", fs_append_func);
    interpreter.getFunctionManager().registerBuiltinFunction("أضف_لملف", fs_append_func);
    
    // copy_file - نسخ ملف
    auto fs_copy_file_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2) throw std::runtime_error("copy_file requires source and destination");
        std::string source = args[0]->toString();
        std::string dest = args[1]->toString();
        bool overwrite = args.size() > 2 ? args[2]->toBool() : false;
        sad::stdlib::filesystem::copy_file(source, dest, overwrite);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("انسخ_ملف", fs_copy_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("copy_file", fs_copy_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("نسخ_ملف", fs_copy_file_func);
    
    // move_file - نقل ملف
    auto fs_move_file_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2) throw std::runtime_error("move_file requires source and destination");
        std::string source = args[0]->toString();
        std::string dest = args[1]->toString();
        sad::stdlib::filesystem::move_file(source, dest);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("انقل_ملف", fs_move_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("move_file", fs_move_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("نقل_ملف", fs_move_file_func);
    
    // delete_file - حذف ملف
    auto fs_delete_file_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) throw std::runtime_error("delete_file requires file path");
        std::string path = args[0]->toString();
        bool result = sad::stdlib::filesystem::delete_file(path);
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_ملف", fs_delete_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("delete_file", fs_delete_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("حذف_ملف", fs_delete_file_func);
    
    // create_directory - إنشاء مجلد
    auto fs_create_dir_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) throw std::runtime_error("create_directory requires path");
        std::string path = args[0]->toString();
        bool recursive = args.size() > 1 ? args[1]->toBool() : true;
        bool result = sad::stdlib::filesystem::create_directory(path, recursive);
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_مجلد", fs_create_dir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("create_directory", fs_create_dir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mkdir", fs_create_dir_func);
    
    // list_directory - سرد محتويات مجلد
    auto fs_list_dir_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) throw std::runtime_error("list_directory requires path");
        std::string path = args[0]->toString();
        auto entries = sad::stdlib::filesystem::list_directory(path);
        std::vector<Data::Value> result;
        for (const auto& entry : entries) {
            result.push_back(Data::Value(entry));
        }
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("اسرد_مجلد", fs_list_dir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("list_directory", fs_list_dir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ls", fs_list_dir_func);
    
    // remove_directory - حذف مجلد
    auto fs_remove_dir_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) throw std::runtime_error("remove_directory requires path");
        std::string path = args[0]->toString();
        bool recursive = args.size() > 1 ? args[1]->toBool() : false;
        bool result = sad::stdlib::filesystem::remove_directory(path, recursive);
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_مجلد", fs_remove_dir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("remove_directory", fs_remove_dir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("rmdir", fs_remove_dir_func);
    
    // is_file - هل هو ملف
    auto fs_is_file_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) throw std::runtime_error("is_file requires path");
        std::string path = args[0]->toString();
        bool result = sad::stdlib::filesystem::is_file(path);
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("هل_ملف", fs_is_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("is_file", fs_is_file_func);
    
    // is_directory - هل هو مجلد
    auto fs_is_dir_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) throw std::runtime_error("is_directory requires path");
        std::string path = args[0]->toString();
        bool result = sad::stdlib::filesystem::is_directory(path);
        return std::make_shared<Data::Value>(result);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("هل_مجلد", fs_is_dir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("is_directory", fs_is_dir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("is_dir", fs_is_dir_func);
    
    // ===================================================================
    // (AR) دوال المدى والتكرار / (EN) Range & Iteration Functions
    // ===================================================================
    
    auto range_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::range(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("مدى", range_func);
    interpreter.getFunctionManager().registerBuiltinFunction("range", range_func);
    
    // ===================================================================
    // (AR) دوال نظام الملفات / (EN) Filesystem Functions
    // ===================================================================
    
    // دالة قراءة ملف / Read file function
    auto read_file_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty() || !args[0]) {
            throw std::runtime_error("(AR) اقرأ_ملف: المعامل الأول مطلوب (مسار الملف) / (EN) read_file: First argument required (file path)");
        }
        std::string path = args[0]->toString();
        
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("(AR) اقرأ_ملف: فشل فتح الملف '" + path + "' / (EN) read_file: Failed to open file '" + path + "'");
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        return std::make_shared<Data::Value>(content);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("اقرأ_ملف", read_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("read_file", read_file_func);
    
    // دالة كتابة ملف / Write file function
    auto write_file_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2 || !args[0] || !args[1]) {
            throw std::runtime_error("(AR) اكتب_ملف: معاملان مطلوبان (مسار، محتوى) / (EN) write_file: Two arguments required (path, content)");
        }
        
        std::string path = args[0]->toString();
        std::string content = args[1]->toString();
        
        std::ofstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("(AR) اكتب_ملف: فشل فتح الملف '" + path + "' للكتابة / (EN) write_file: Failed to open file '" + path + "' for writing");
        }
        
        file << content;
        file.close();
        
        return std::make_shared<Data::Value>();  // void
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("اكتب_ملف", write_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("write_file", write_file_func);
    
    // دالة إضافة لملف / Append to file function
    auto append_file_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2 || !args[0] || !args[1]) {
            throw std::runtime_error("(AR) أضف_إلى_ملف: معاملان مطلوبان / (EN) append_to_file: Two arguments required");
        }
        
        std::string path = args[0]->toString();
        std::string content = args[1]->toString();
        
        std::ofstream file(path, std::ios::app);
        if (!file.is_open()) {
            throw std::runtime_error("(AR) أضف_إلى_ملف: فشل فتح الملف / (EN) append_to_file: Failed to open file");
        }
        
        file << content;
        file.close();
        
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أضف_إلى_ملف", append_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("append_to_file", append_file_func);
    
    // دالة حذف ملف / Delete file function
    auto delete_file_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty() || !args[0]) {
            throw std::runtime_error("(AR) احذف_ملف: المعامل الأول مطلوب / (EN) delete_file: First argument required");
        }
        
        std::string path = args[0]->toString();
        
        if (std::filesystem::exists(path)) {
            std::filesystem::remove(path);
        } else {
            throw std::runtime_error("(AR) احذف_ملف: الملف غير موجود / (EN) delete_file: File not found");
        }
        
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_ملف", delete_file_func);
    interpreter.getFunctionManager().registerBuiltinFunction("delete_file", delete_file_func);
    
    // دالة التحقق من وجود ملف / Check if file exists
    auto file_exists_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty() || !args[0]) {
            throw std::runtime_error("(AR) هل_موجود: المعامل الأول مطلوب / (EN) exists: First argument required");
        }
        
        std::string path = args[0]->toString();
        bool exists = std::filesystem::exists(path);
        
        return std::make_shared<Data::Value>(exists);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("هل_موجود", file_exists_func);
    interpreter.getFunctionManager().registerBuiltinFunction("exists", file_exists_func);
    
    // ===================================================================
    // (AR) دوال الرسومات / (EN) Graphics Functions
    // ===================================================================
    
    // ===== Window Management / إدارة النوافذ =====
    
    // Create window / إنشاء نافذة
    auto window_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 3) {
            throw std::runtime_error("(AR) نافذة_جديد: 3 معاملات مطلوبة (عنوان، عرض، ارتفاع) / (EN) window_new: 3 arguments required (title, width, height)");
        }
        
        std::string title = args[0]->toString();
        int width = args[1]->toInt();
        int height = args[2]->toInt();
        
        int windowId = sad::stdlib::graphics::window_create_impl(title, width, height);
        return std::make_shared<Data::Value>(windowId);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نافذة_جديد", window_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_new", window_create_func);
    
    // Show window / عرض النافذة
    auto window_show_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) نافذة_عرض: معامل النافذة مطلوب / (EN) window_show: Window argument required");
        }
        
        int windowId = args[0]->toInt();
        sad::stdlib::graphics::window_show_impl(windowId);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نافذة_عرض", window_show_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_show", window_show_func);
    
    // Hide window / إخفاء النافذة
    auto window_hide_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) نافذة_إخفاء: معامل النافذة مطلوب / (EN) window_hide: Window argument required");
        }
        
        int windowId = args[0]->toInt();
        sad::stdlib::graphics::window_hide_impl(windowId);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نافذة_إخفاء", window_hide_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_hide", window_hide_func);
    
    // Close window / إغلاق النافذة
    auto window_close_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) نافذة_إغلاق: معامل النافذة مطلوب / (EN) window_close: Window argument required");
        }
        
        int windowId = args[0]->toInt();
        sad::stdlib::graphics::window_close_impl(windowId);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نافذة_إغلاق", window_close_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_close", window_close_func);
    
    // Should close window / هل يجب إغلاق النافذة
    auto window_should_close_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) نافذة_يجب_الإغلاق: معامل النافذة مطلوب / (EN) window_should_close: Window argument required");
        }
        
        int windowId = args[0]->toInt();
        bool shouldClose = sad::stdlib::graphics::window_should_close_impl(windowId);
        return std::make_shared<Data::Value>(shouldClose);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نافذة_يجب_الإغلاق", window_should_close_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_should_close", window_should_close_func);
    
    // Poll events / استقبال الأحداث
    auto window_poll_events_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) نافذة_استقبال_أحداث: معامل النافذة مطلوب / (EN) window_poll_events: Window argument required");
        }
        
        int windowId = args[0]->toInt();
        sad::stdlib::graphics::window_poll_events_impl(windowId);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نافذة_استقبال_أحداث", window_poll_events_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_poll_events", window_poll_events_func);
    
    // Swap buffers / تبديل buffers
    auto window_swap_buffers_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) نافذة_تبديل_buffers: معامل النافذة مطلوب / (EN) window_swap_buffers: Window argument required");
        }
        
        int windowId = args[0]->toInt();
        sad::stdlib::graphics::window_swap_buffers_impl(windowId);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نافذة_تبديل_buffers", window_swap_buffers_func);
    interpreter.getFunctionManager().registerBuiltinFunction("window_swap_buffers", window_swap_buffers_func);
    
    // ===== Renderer Management / إدارة الرسام =====
    
    // Create renderer / إنشاء رسام
    auto renderer_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) رسام_ثنائي_جديد: معامل النافذة مطلوب / (EN) renderer_new: Window argument required");
        }
        
        int windowId = args[0]->toInt();
        int rendererId = sad::stdlib::graphics::renderer_create_impl(windowId);
        return std::make_shared<Data::Value>(rendererId);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("رسام_ثنائي_جديد", renderer_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_new", renderer_create_func);
    
    // Begin frame / بدء إطار
    auto renderer_begin_frame_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) رسام_بدء_إطار: معامل الرسام مطلوب / (EN) renderer_begin_frame: Renderer argument required");
        }
        
        int rendererId = args[0]->toInt();
        sad::stdlib::graphics::renderer_begin_frame_impl(rendererId);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("رسام_بدء_إطار", renderer_begin_frame_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_begin_frame", renderer_begin_frame_func);
    
    // End frame / إنهاء إطار
    auto renderer_end_frame_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) رسام_إنهاء_إطار: معامل الرسام مطلوب / (EN) renderer_end_frame: Renderer argument required");
        }
        
        int rendererId = args[0]->toInt();
        sad::stdlib::graphics::renderer_end_frame_impl(rendererId);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("رسام_إنهاء_إطار", renderer_end_frame_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_end_frame", renderer_end_frame_func);
    
    // Clear screen / مسح الشاشة
    auto renderer_clear_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 5) {
            throw std::runtime_error("(AR) رسام_مسح: 5 معاملات مطلوبة (رسام، أحمر، أخضر، أزرق، شفاف) / (EN) renderer_clear: 5 arguments required (renderer, r, g, b, a)");
        }
        
        int rendererId = args[0]->toInt();
        int r = args[1]->toInt();
        int g = args[2]->toInt();
        int b = args[3]->toInt();
        int a = args[4]->toInt();
        
        sad::stdlib::graphics::renderer_clear_impl(rendererId, r, g, b, a);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("رسام_مسح", renderer_clear_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_clear", renderer_clear_func);
    
    // Draw line / رسم خط
    auto renderer_draw_line_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 9) {
            throw std::runtime_error("(AR) رسام_خط: 9 معاملات مطلوبة / (EN) renderer_draw_line: 9 arguments required");
        }
        
        int rendererId = args[0]->toInt();
        float x1 = static_cast<float>(args[1]->toDouble());
        float y1 = static_cast<float>(args[2]->toDouble());
        float x2 = static_cast<float>(args[3]->toDouble());
        float y2 = static_cast<float>(args[4]->toDouble());
        int r = args[5]->toInt();
        int g = args[6]->toInt();
        int b = args[7]->toInt();
        int a = args[8]->toInt();
        
        sad::stdlib::graphics::renderer_draw_line_impl(rendererId, x1, y1, x2, y2, r, g, b, a);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("رسام_خط", renderer_draw_line_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_draw_line", renderer_draw_line_func);
    
    // Draw rectangle / رسم مستطيل
    auto renderer_draw_rect_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 10) {
            throw std::runtime_error("(AR) رسام_مستطيل: 10 معاملات مطلوبة / (EN) renderer_draw_rect: 10 arguments required");
        }
        
        int rendererId = args[0]->toInt();
        float x = static_cast<float>(args[1]->toDouble());
        float y = static_cast<float>(args[2]->toDouble());
        float width = static_cast<float>(args[3]->toDouble());
        float height = static_cast<float>(args[4]->toDouble());
        int r = args[5]->toInt();
        int g = args[6]->toInt();
        int b = args[7]->toInt();
        int a = args[8]->toInt();
        bool filled = args[9]->toBool();
        
        sad::stdlib::graphics::renderer_draw_rect_impl(rendererId, x, y, width, height, r, g, b, a, filled);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("رسام_مستطيل", renderer_draw_rect_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_draw_rect", renderer_draw_rect_func);
    
    // Draw circle / رسم دائرة
    auto renderer_draw_circle_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 9) {
            throw std::runtime_error("(AR) رسام_دائرة: 9 معاملات مطلوبة / (EN) renderer_draw_circle: 9 arguments required");
        }
        
        int rendererId = args[0]->toInt();
        float x = static_cast<float>(args[1]->toDouble());
        float y = static_cast<float>(args[2]->toDouble());
        float radius = static_cast<float>(args[3]->toDouble());
        int r = args[4]->toInt();
        int g = args[5]->toInt();
        int b = args[6]->toInt();
        int a = args[7]->toInt();
        bool filled = args[8]->toBool();
        
        sad::stdlib::graphics::renderer_draw_circle_impl(rendererId, x, y, radius, r, g, b, a, filled);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("رسام_دائرة", renderer_draw_circle_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_draw_circle", renderer_draw_circle_func);
    
    // Draw text / رسم نص
    auto renderer_draw_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 9) {
            throw std::runtime_error("(AR) رسام_نص: 9 معاملات مطلوبة / (EN) renderer_draw_text: 9 arguments required");
        }
        
        int rendererId = args[0]->toInt();
        std::string text = args[1]->toString();
        float x = static_cast<float>(args[2]->toDouble());
        float y = static_cast<float>(args[3]->toDouble());
        float size = static_cast<float>(args[4]->toDouble());
        int r = args[5]->toInt();
        int g = args[6]->toInt();
        int b = args[7]->toInt();
        int a = args[8]->toInt();
        
        sad::stdlib::graphics::renderer_draw_text_impl(rendererId, text, x, y, size, r, g, b, a);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("رسام_نص", renderer_draw_text_func);
    interpreter.getFunctionManager().registerBuiltinFunction("renderer_draw_text", renderer_draw_text_func);
    
    // ===== UI Widgets / عناصر الواجهة =====
    
    // Create label / إنشاء تسمية
    auto label_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 5) {
            throw std::runtime_error("(AR) تسمية_جديدة: 5 معاملات مطلوبة (نص، x، y، عرض، ارتفاع) / (EN) label_new: 5 arguments required (text, x, y, width, height)");
        }
        
        std::string text = args[0]->toString();
        float x = static_cast<float>(args[1]->toDouble());
        float y = static_cast<float>(args[2]->toDouble());
        float width = static_cast<float>(args[3]->toDouble());
        float height = static_cast<float>(args[4]->toDouble());
        
        int labelId = sad::stdlib::graphics::label_create_impl(text, x, y, width, height);
        return std::make_shared<Data::Value>(labelId);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("تسمية_جديدة", label_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("label_new", label_create_func);
    
    // Set label text / تعيين نص التسمية
    auto label_set_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2) {
            throw std::runtime_error("(AR) تسمية_تعيين_نص: معاملان مطلوبان / (EN) label_set_text: 2 arguments required");
        }
        
        int labelId = args[0]->toInt();
        std::string text = args[1]->toString();
        sad::stdlib::graphics::label_set_text_impl(labelId, text);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("تسمية_تعيين_نص", label_set_text_func);
    interpreter.getFunctionManager().registerBuiltinFunction("label_set_text", label_set_text_func);
    
    // Get label text / الحصول على نص التسمية
    auto label_get_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) تسمية_الحصول_على_نص: معامل التسمية مطلوب / (EN) label_get_text: Label argument required");
        }
        
        int labelId = args[0]->toInt();
        std::string text = sad::stdlib::graphics::label_get_text_impl(labelId);
        return std::make_shared<Data::Value>(text);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("تسمية_الحصول_على_نص", label_get_text_func);
    interpreter.getFunctionManager().registerBuiltinFunction("label_get_text", label_get_text_func);
    
    // Create button / إنشاء زر
    auto button_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 5) {
            throw std::runtime_error("(AR) زر_جديد: 5 معاملات مطلوبة / (EN) button_new: 5 arguments required");
        }
        
        std::string text = args[0]->toString();
        float x = static_cast<float>(args[1]->toDouble());
        float y = static_cast<float>(args[2]->toDouble());
        float width = static_cast<float>(args[3]->toDouble());
        float height = static_cast<float>(args[4]->toDouble());
        
        int buttonId = sad::stdlib::graphics::button_create_impl(text, x, y, width, height);
        return std::make_shared<Data::Value>(buttonId);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("زر_جديد", button_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("button_new", button_create_func);
    
    // Check button clicked / فحص النقر على الزر
    auto button_is_clicked_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) زر_تم_النقر: معامل الزر مطلوب / (EN) button_is_clicked: Button argument required");
        }
        
        int buttonId = args[0]->toInt();
        bool clicked = sad::stdlib::graphics::button_is_clicked_impl(buttonId);
        return std::make_shared<Data::Value>(clicked);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("زر_تم_النقر", button_is_clicked_func);
    interpreter.getFunctionManager().registerBuiltinFunction("button_is_clicked", button_is_clicked_func);
    
    // Set button text / تعيين نص الزر
    auto button_set_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2) {
            throw std::runtime_error("(AR) زر_تعيين_نص: معاملان مطلوبان / (EN) button_set_text: 2 arguments required");
        }
        
        int buttonId = args[0]->toInt();
        std::string text = args[1]->toString();
        sad::stdlib::graphics::button_set_text_impl(buttonId, text);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("زر_تعيين_نص", button_set_text_func);
    interpreter.getFunctionManager().registerBuiltinFunction("button_set_text", button_set_text_func);
    
    // Create text input / إنشاء حقل نص
    auto textinput_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 5) {
            throw std::runtime_error("(AR) حقل_نص_جديد: 5 معاملات مطلوبة / (EN) textinput_new: 5 arguments required");
        }
        
        std::string placeholder = args[0]->toString();
        float x = static_cast<float>(args[1]->toDouble());
        float y = static_cast<float>(args[2]->toDouble());
        float width = static_cast<float>(args[3]->toDouble());
        float height = static_cast<float>(args[4]->toDouble());
        
        int inputId = sad::stdlib::graphics::textinput_create_impl(placeholder, x, y, width, height);
        return std::make_shared<Data::Value>(inputId);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("حقل_نص_جديد", textinput_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("textinput_new", textinput_create_func);
    
    // Get text input value / الحصول على قيمة حقل النص
    auto textinput_get_value_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) حقل_نص_الحصول_على_قيمة: معامل حقل النص مطلوب / (EN) textinput_get_value: TextInput argument required");
        }
        
        int inputId = args[0]->toInt();
        std::string value = sad::stdlib::graphics::textinput_get_value_impl(inputId);
        return std::make_shared<Data::Value>(value);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("حقل_نص_الحصول_على_قيمة", textinput_get_value_func);
    interpreter.getFunctionManager().registerBuiltinFunction("textinput_get_value", textinput_get_value_func);
    
    // Set text input value / تعيين قيمة حقل النص
    auto textinput_set_value_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2) {
            throw std::runtime_error("(AR) حقل_نص_تعيين_قيمة: معاملان مطلوبان / (EN) textinput_set_value: 2 arguments required");
        }
        
        int inputId = args[0]->toInt();
        std::string value = args[1]->toString();
        sad::stdlib::graphics::textinput_set_value_impl(inputId, value);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("حقل_نص_تعيين_قيمة", textinput_set_value_func);
    interpreter.getFunctionManager().registerBuiltinFunction("textinput_set_value", textinput_set_value_func);
    
    // Create checkbox / إنشاء مربع اختيار
    auto checkbox_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 5) {
            throw std::runtime_error("(AR) مربع_اختيار_جديد: 5 معاملات مطلوبة / (EN) checkbox_new: 5 arguments required");
        }
        
        std::string label = args[0]->toString();
        float x = static_cast<float>(args[1]->toDouble());
        float y = static_cast<float>(args[2]->toDouble());
        float width = static_cast<float>(args[3]->toDouble());
        float height = static_cast<float>(args[4]->toDouble());
        
        int checkboxId = sad::stdlib::graphics::checkbox_create_impl(label, x, y, width, height);
        return std::make_shared<Data::Value>(checkboxId);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("مربع_اختيار_جديد", checkbox_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("checkbox_new", checkbox_create_func);
    
    // Check checkbox state / فحص حالة مربع الاختيار
    auto checkbox_is_checked_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) مربع_اختيار_محدد: معامل مربع الاختيار مطلوب / (EN) checkbox_is_checked: Checkbox argument required");
        }
        
        int checkboxId = args[0]->toInt();
        bool checked = sad::stdlib::graphics::checkbox_is_checked_impl(checkboxId);
        return std::make_shared<Data::Value>(checked);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("مربع_اختيار_محدد", checkbox_is_checked_func);
    interpreter.getFunctionManager().registerBuiltinFunction("checkbox_is_checked", checkbox_is_checked_func);
    
    // Set checkbox state / تعيين حالة مربع الاختيار
    auto checkbox_set_checked_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2) {
            throw std::runtime_error("(AR) مربع_اختيار_تعيين_حالة: معاملان مطلوبان / (EN) checkbox_set_checked: 2 arguments required");
        }
        
        int checkboxId = args[0]->toInt();
        bool checked = args[1]->toBool();
        sad::stdlib::graphics::checkbox_set_checked_impl(checkboxId, checked);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("مربع_اختيار_تعيين_حالة", checkbox_set_checked_func);
    interpreter.getFunctionManager().registerBuiltinFunction("checkbox_set_checked", checkbox_set_checked_func);
    
    // Draw widget / رسم عنصر
    auto widget_draw_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2) {
            throw std::runtime_error("(AR) عنصر_رسم: معاملان مطلوبان (عنصر، رسام) / (EN) widget_draw: 2 arguments required (widget, renderer)");
        }
        
        int widgetId = args[0]->toInt();
        int rendererId = args[1]->toInt();
        sad::stdlib::graphics::widget_draw_impl(widgetId, rendererId);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("عنصر_رسم", widget_draw_func);
    interpreter.getFunctionManager().registerBuiltinFunction("widget_draw", widget_draw_func);
    
    // Update widget / تحديث عنصر
    auto widget_update_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2) {
            throw std::runtime_error("(AR) عنصر_تحديث: معاملان مطلوبان (عنصر، زمن) / (EN) widget_update: 2 arguments required (widget, deltaTime)");
        }
        
        int widgetId = args[0]->toInt();
        float deltaTime = static_cast<float>(args[1]->toDouble());
        sad::stdlib::graphics::widget_update_impl(widgetId, deltaTime);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("عنصر_تحديث", widget_update_func);
    interpreter.getFunctionManager().registerBuiltinFunction("widget_update", widget_update_func);
    
    // Handle mouse event / معالجة حدث الفأرة
    auto widget_mouse_event_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 4) {
            throw std::runtime_error("(AR) عنصر_حدث_فأرة: 4 معاملات مطلوبة (عنصر، x، y، مضغوط) / (EN) widget_mouse_event: 4 arguments required (widget, x, y, pressed)");
        }
        
        int widgetId = args[0]->toInt();
        int x = args[1]->toInt();
        int y = args[2]->toInt();
        bool pressed = args[3]->toBool();
        
        sad::stdlib::graphics::widget_mouse_event_impl(widgetId, x, y, pressed);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("عنصر_حدث_فأرة", widget_mouse_event_func);
    interpreter.getFunctionManager().registerBuiltinFunction("widget_mouse_event", widget_mouse_event_func);
}

} // namespace Interpreter
} // namespace Sad
