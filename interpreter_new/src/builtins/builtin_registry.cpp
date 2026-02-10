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

#include "builtins.h"
#include "interpreter_core.h"
#include "io/io_functions.h"
#include "type_functions.h"
#include "array_functions.h"
#include "other_functions.h"
#include "string/string_functions.h"
#include "math/math_functions.h"
#include "math/advanced_math.h"
#include "system/system_functions.h"
#include "filesystem/filesystem_module.h"
#include "graphics/graphics_module.h"
#include "mobile/mobile_module.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <random>
#include <chrono>
#include <thread>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

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
    interpreter.getFunctionManager().registerBuiltinFunction("حول_رقم", to_int_func);
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
    
    // ===================================================================
    // Phase 23: Advanced Math Functions (log, exp, clamp, etc.)
    // ===================================================================
    
    // Natural logarithm / لوغاريتم طبيعي
    auto math_log_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::log(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("لوغ", math_log_func);
    interpreter.getFunctionManager().registerBuiltinFunction("log", math_log_func);
    interpreter.getFunctionManager().registerBuiltinFunction("لوغاريتم", math_log_func);
    
    // Base-10 logarithm / لوغاريتم عشري
    auto math_log10_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::log10(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("لوغ10", math_log10_func);
    interpreter.getFunctionManager().registerBuiltinFunction("log10", math_log10_func);
    interpreter.getFunctionManager().registerBuiltinFunction("لوغاريتم_عشري", math_log10_func);
    
    // Base-2 logarithm / لوغاريتم ثنائي
    auto math_log2_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::log2(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("لوغ2", math_log2_func);
    interpreter.getFunctionManager().registerBuiltinFunction("log2", math_log2_func);
    interpreter.getFunctionManager().registerBuiltinFunction("لوغاريتم_ثنائي", math_log2_func);
    
    // Exponential e^x / أسّي
    auto math_exp_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::exp(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("أسّي", math_exp_func);
    interpreter.getFunctionManager().registerBuiltinFunction("exp", math_exp_func);
    interpreter.getFunctionManager().registerBuiltinFunction("أُسّي", math_exp_func);
    
    // Clamp / تقييد
    auto math_clamp_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::clamp(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("قيّد", math_clamp_func);
    interpreter.getFunctionManager().registerBuiltinFunction("clamp", math_clamp_func);
    interpreter.getFunctionManager().registerBuiltinFunction("تقييد", math_clamp_func);
    
    // Truncate / اقتطاع
    auto math_trunc_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::trunc(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("اقتطاع", math_trunc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("trunc", math_trunc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("اقتطع", math_trunc_func);
    
    // Modulo / باقي القسمة
    auto math_mod_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::mod(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("باقي", math_mod_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mod", math_mod_func);
    interpreter.getFunctionManager().registerBuiltinFunction("باقي_القسمة", math_mod_func);
    
    // Sign / إشارة
    auto math_sign_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::sign(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("إشارة", math_sign_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sign", math_sign_func);
    
    // Pi constant / ثابت باي
    auto math_pi_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::pi());
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("باي", math_pi_func);
    interpreter.getFunctionManager().registerBuiltinFunction("pi", math_pi_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ط", math_pi_func);
    
    // e constant / ثابت إي
    auto math_e_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return std::make_shared<Data::Value>(StdLib::Math::AdvancedMath::e());
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("إي", math_e_func);
    interpreter.getFunctionManager().registerBuiltinFunction("e_const", math_e_func);
    interpreter.getFunctionManager().registerBuiltinFunction("أويلر", math_e_func);
    
    // ===================================================================
    // Phase 24: System Functions (environment, directory, execution)
    // ===================================================================
    
    // Get environment variable / الحصول على متغير بيئة
    auto sys_getenv_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::getEnv(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("احصل_بيئة", sys_getenv_func);
    interpreter.getFunctionManager().registerBuiltinFunction("getEnv", sys_getenv_func);
    interpreter.getFunctionManager().registerBuiltinFunction("get_env", sys_getenv_func);
    
    // Set environment variable / تعيين متغير بيئة
    auto sys_setenv_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::setEnv(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_بيئة", sys_setenv_func);
    interpreter.getFunctionManager().registerBuiltinFunction("setEnv", sys_setenv_func);
    interpreter.getFunctionManager().registerBuiltinFunction("set_env", sys_setenv_func);
    
    // Get current directory / الحصول على المجلد الحالي
    auto sys_curdir_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::currentDir(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("مجلد_حالي", sys_curdir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("currentDir", sys_curdir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("المجلد_الحالي", sys_curdir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("pwd", sys_curdir_func);
    
    // Change directory / تغيير المجلد
    auto sys_chdir_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::changeDir(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("غيّر_مجلد", sys_chdir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("changeDir", sys_chdir_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cd", sys_chdir_func);
    
    // Execute command / تنفيذ أمر
    auto sys_execute_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::execute(plainArgs));
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نفّذ", sys_execute_func);
    interpreter.getFunctionManager().registerBuiltinFunction("execute", sys_execute_func);
    interpreter.getFunctionManager().registerBuiltinFunction("تنفيذ", sys_execute_func);
    interpreter.getFunctionManager().registerBuiltinFunction("system", sys_execute_func);
    
    // Get platform / المنصة
    auto sys_platform_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::platform());
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("منصة", sys_platform_func);
    interpreter.getFunctionManager().registerBuiltinFunction("platform", sys_platform_func);
    interpreter.getFunctionManager().registerBuiltinFunction("المنصة", sys_platform_func);
    
    // Get OS name / اسم نظام التشغيل
    auto sys_osname_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::osName());
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("نظام", sys_osname_func);
    interpreter.getFunctionManager().registerBuiltinFunction("osName", sys_osname_func);
    interpreter.getFunctionManager().registerBuiltinFunction("os", sys_osname_func);
    
    // Get timestamp / الطابع الزمني
    auto sys_timestamp_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::timestamp());
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("طابع_زمني", sys_timestamp_func);
    interpreter.getFunctionManager().registerBuiltinFunction("timestamp", sys_timestamp_func);
    interpreter.getFunctionManager().registerBuiltinFunction("الطابع_الزمني", sys_timestamp_func);
    
    // Get clock time / وقت الساعة
    auto sys_clock_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return std::make_shared<Data::Value>(StdLib::System::SystemFunctions::clock_time());
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("ساعة", sys_clock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("clock", sys_clock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("وقت_الساعة", sys_clock_func);

    // ===================================================================
    // (AR) دوال برمجة أنظمة التشغيل — محاكاة في المفسر
    // (EN) OS Development Functions — Simulation in interpreter
    // ===================================================================
    // (AR) هذه الدوال تحاكي عمليات النظام المنخفض المستوى في المفسر.
    //      في المترجم (sadc)، تُولّد هذه العمليات كود أصلي (inline asm).
    //      في المفسر (sad)، تُحاكى بطباعة رسائل تشخيصية.
    // ===================================================================

    // (AR) محاكاة كتابة منفذ — outb(port, value)
    auto port_write_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("منفذ_اكتب: يحتاج معاملين (منفذ، قيمة)");
        int port = args[0]->toInt();
        int val = args[1]->toInt();
        std::cout << "[OS-SIM] outb(0x" << std::hex << port << ", 0x" << val << std::dec << ")" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("منفذ_اكتب", port_write_func);
    interpreter.getFunctionManager().registerBuiltinFunction("port_write", port_write_func);
    interpreter.getFunctionManager().registerBuiltinFunction("outb", port_write_func);

    // (AR) محاكاة قراءة منفذ — inb(port)
    auto port_read_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("منفذ_اقرأ: يحتاج معامل واحد (رقم المنفذ)");
        int port = args[0]->toInt();
        std::cout << "[OS-SIM] inb(0x" << std::hex << port << std::dec << ") -> 0x00" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("منفذ_اقرأ", port_read_func);
    interpreter.getFunctionManager().registerBuiltinFunction("port_read", port_read_func);
    interpreter.getFunctionManager().registerBuiltinFunction("inb", port_read_func);

    // (AR) محاكاة كتابة ذاكرة — poke(addr, value)
    auto mem_write_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("ذاكرة_اكتب: يحتاج معاملين (عنوان، قيمة)");
        int addr = args[0]->toInt();
        int val = args[1]->toInt();
        std::cout << "[OS-SIM] poke(0x" << std::hex << addr << ", 0x" << val << std::dec << ")" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ذاكرة_اكتب", mem_write_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mem_write", mem_write_func);
    interpreter.getFunctionManager().registerBuiltinFunction("poke", mem_write_func);

    // (AR) محاكاة قراءة ذاكرة — peek(addr)
    auto mem_read_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("ذاكرة_اقرأ: يحتاج معامل واحد (عنوان)");
        int addr = args[0]->toInt();
        std::cout << "[OS-SIM] peek(0x" << std::hex << addr << std::dec << ") -> 0x00" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ذاكرة_اقرأ", mem_read_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mem_read", mem_read_func);
    interpreter.getFunctionManager().registerBuiltinFunction("peek", mem_read_func);

    // (AR) محاكاة مقاطعة — interrupt(n)
    auto interrupt_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("مقاطعة: يحتاج معامل واحد (رقم المقاطعة)");
        int n = args[0]->toInt();
        std::cout << "[OS-SIM] int 0x" << std::hex << n << std::dec << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مقاطعة", interrupt_func);
    interpreter.getFunctionManager().registerBuiltinFunction("interrupt", interrupt_func);
    interpreter.getFunctionManager().registerBuiltinFunction("int_call", interrupt_func);

    // (AR) محاكاة إيقاف المعالج — hlt
    auto halt_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[OS-SIM] hlt — CPU halted" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("توقف", halt_func);
    interpreter.getFunctionManager().registerBuiltinFunction("halt", halt_func);
    interpreter.getFunctionManager().registerBuiltinFunction("hlt", halt_func);

    // (AR) محاكاة تعطيل المقاطعات — cli
    auto cli_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[OS-SIM] cli — interrupts disabled" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تعطيل_مقاطعات", cli_func);
    interpreter.getFunctionManager().registerBuiltinFunction("disable_interrupts", cli_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cli", cli_func);

    // (AR) محاكاة تفعيل المقاطعات — sti
    auto sti_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[OS-SIM] sti — interrupts enabled" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تفعيل_مقاطعات", sti_func);
    interpreter.getFunctionManager().registerBuiltinFunction("enable_interrupts", sti_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sti", sti_func);

    // (AR) محاكاة كتابة VGA — vga_write(row, col, char, color)
    auto vga_write_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 4) throw std::runtime_error("شاشة_اكتب: يحتاج 4 معاملات (صف، عمود، حرف، لون)");
        int row = args[0]->toInt();
        int col = args[1]->toInt();
        int ch  = args[2]->toInt();
        int color = args[3]->toInt();
        std::cout << "[OS-SIM] VGA[" << row << "," << col << "] = '" 
                  << (char)ch << "' (color=0x" << std::hex << color << std::dec << ")" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("شاشة_اكتب", vga_write_func);
    interpreter.getFunctionManager().registerBuiltinFunction("vga_write", vga_write_func);

    // (AR) محاكاة مسح الشاشة — vga_clear(color?)
    auto vga_clear_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int color = 0x0F;
        if (!args.empty()) color = args[0]->toInt();
        std::cout << "[OS-SIM] VGA clear (color=0x" << std::hex << color << std::dec << ")" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("شاشة_امسح", vga_clear_func);
    interpreter.getFunctionManager().registerBuiltinFunction("vga_clear", vga_clear_func);

    // (AR) محاكاة نسخ ذاكرة — memcpy(dest, src, size)
    auto memcpy_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) throw std::runtime_error("انسخ_ذاكرة: يحتاج 3 معاملات");
        std::cout << "[OS-SIM] memcpy(0x" << std::hex << args[0]->toInt() 
                  << ", 0x" << args[1]->toInt() << ", " << std::dec << args[2]->toInt() << ")" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("انسخ_ذاكرة", memcpy_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mem_copy", memcpy_func);
    interpreter.getFunctionManager().registerBuiltinFunction("memcpy", memcpy_func);

    // (AR) محاكاة ملء ذاكرة — memset(dest, value, size)
    auto memset_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) throw std::runtime_error("املأ_ذاكرة: يحتاج 3 معاملات");
        std::cout << "[OS-SIM] memset(0x" << std::hex << args[0]->toInt() 
                  << ", 0x" << args[1]->toInt() << ", " << std::dec << args[2]->toInt() << ")" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("املأ_ذاكرة", memset_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mem_set", memset_func);
    interpreter.getFunctionManager().registerBuiltinFunction("memset", memset_func);

    // ══════════════════════════════════════════════════════════════════════
    // ⚡ دوال Embedded المتقدمة / Advanced Embedded Functions (18)
    // ══════════════════════════════════════════════════════════════════════

    // ─── Serial I/O (4) ─────────────────────────────────────────────────

    // (AR) تهيئة منفذ تسلسلي — serial_init(port, baud_rate)
    auto serial_init_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("تسلسلي_هيئ: يحتاج 2 معاملات (منفذ، سرعة)");
        int port = args[0]->toInt();
        int baud = args[1]->toInt();
        std::cout << "[OS-SIM] Serial init: port=0x" << std::hex << port 
                  << " baud=" << std::dec << baud << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تسلسلي_هيئ", serial_init_func);
    interpreter.getFunctionManager().registerBuiltinFunction("serial_init", serial_init_func);

    // (AR) إرسال بايت عبر التسلسلي — serial_send(port, byte)
    auto serial_write_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("تسلسلي_ارسل: يحتاج 2 معاملات (منفذ، بايت)");
        int port = args[0]->toInt();
        int byte = args[1]->toInt();
        std::cout << "[OS-SIM] Serial send: port=0x" << std::hex << port 
                  << " byte=0x" << byte << " ('" << (char)byte << "')" << std::dec << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تسلسلي_ارسل", serial_write_func);
    interpreter.getFunctionManager().registerBuiltinFunction("serial_send", serial_write_func);
    interpreter.getFunctionManager().registerBuiltinFunction("serial_write", serial_write_func);

    // (AR) استقبال بايت من التسلسلي — serial_receive(port) → byte
    auto serial_read_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("تسلسلي_استقبل: يحتاج معامل (منفذ)");
        int port = args[0]->toInt();
        std::cout << "[OS-SIM] Serial receive: port=0x" << std::hex << port << std::dec 
                  << " → simulated byte 0x41 ('A')" << std::endl;
        return std::make_shared<Data::Value>(0x41); // simulated received 'A'
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تسلسلي_استقبل", serial_read_func);
    interpreter.getFunctionManager().registerBuiltinFunction("serial_receive", serial_read_func);
    interpreter.getFunctionManager().registerBuiltinFunction("serial_read", serial_read_func);

    // (AR) فحص جاهزية البيانات — serial_ready(port) → bool
    auto serial_ready_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("تسلسلي_جاهز: يحتاج معامل (منفذ)");
        int port = args[0]->toInt();
        std::cout << "[OS-SIM] Serial ready: port=0x" << std::hex << port << std::dec 
                  << " → true (simulated)" << std::endl;
        return std::make_shared<Data::Value>(1); // simulated: data available
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تسلسلي_جاهز", serial_ready_func);
    interpreter.getFunctionManager().registerBuiltinFunction("serial_ready", serial_ready_func);

    // ─── GPIO (3) ────────────────────────────────────────────────────────

    // (AR) كتابة GPIO — gpio_write(pin, value)
    auto gpio_write_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("منفذ_رقمي_اكتب: يحتاج 2 معاملات (رقم_المنفذ، قيمة)");
        int pin = args[0]->toInt();
        int val = args[1]->toInt();
        std::cout << "[OS-SIM] GPIO write: pin=" << pin << " value=" << val << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("منفذ_رقمي_اكتب", gpio_write_func);
    interpreter.getFunctionManager().registerBuiltinFunction("gpio_write", gpio_write_func);
    interpreter.getFunctionManager().registerBuiltinFunction("digital_write", gpio_write_func);

    // (AR) قراءة GPIO — gpio_read(pin) → value
    auto gpio_read_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("منفذ_رقمي_اقرأ: يحتاج معامل (رقم_المنفذ)");
        int pin = args[0]->toInt();
        std::cout << "[OS-SIM] GPIO read: pin=" << pin << " → 1 (simulated HIGH)" << std::endl;
        return std::make_shared<Data::Value>(1); // simulated HIGH
    };
    interpreter.getFunctionManager().registerBuiltinFunction("منفذ_رقمي_اقرأ", gpio_read_func);
    interpreter.getFunctionManager().registerBuiltinFunction("gpio_read", gpio_read_func);
    interpreter.getFunctionManager().registerBuiltinFunction("digital_read", gpio_read_func);

    // (AR) تحديد وضع GPIO — gpio_mode(pin, mode)
    auto gpio_mode_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("حدد_وضع_منفذ: يحتاج 2 معاملات (رقم_المنفذ، الوضع)");
        int pin = args[0]->toInt();
        int mode = args[1]->toInt();
        std::string modeName = (mode == 0) ? "INPUT" : "OUTPUT";
        std::cout << "[OS-SIM] GPIO mode: pin=" << pin << " mode=" << modeName << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حدد_وضع_منفذ", gpio_mode_func);
    interpreter.getFunctionManager().registerBuiltinFunction("gpio_mode", gpio_mode_func);
    interpreter.getFunctionManager().registerBuiltinFunction("pin_mode", gpio_mode_func);

    // ─── Timer (3) ───────────────────────────────────────────────────────

    // (AR) تهيئة مؤقت — timer_init(frequency)
    auto timer_init_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("مؤقت_هيئ: يحتاج معامل (التردد)");
        int freq = args[0]->toInt();
        std::cout << "[OS-SIM] Timer init: frequency=" << freq << " Hz" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مؤقت_هيئ", timer_init_func);
    interpreter.getFunctionManager().registerBuiltinFunction("timer_init", timer_init_func);

    // (AR) قراءة المؤقت — timer_read() → value
    auto timer_read_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto now = std::chrono::high_resolution_clock::now();
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
        std::cout << "[OS-SIM] Timer read: " << us << " us" << std::endl;
        return std::make_shared<Data::Value>(static_cast<int>(us & 0x7FFFFFFF));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مؤقت_قراءة", timer_read_func);
    interpreter.getFunctionManager().registerBuiltinFunction("timer_read", timer_read_func);

    // (AR) انتظار ميكروثوان — timer_wait(us)
    auto timer_wait_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("مؤقت_انتظر: يحتاج معامل (ميكروثوان)");
        int us = args[0]->toInt();
        std::cout << "[OS-SIM] Timer wait: " << us << " microseconds" << std::endl;
        // Actually wait (in simulation)
        std::this_thread::sleep_for(std::chrono::microseconds(us));
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مؤقت_انتظر", timer_wait_func);
    interpreter.getFunctionManager().registerBuiltinFunction("timer_wait", timer_wait_func);
    interpreter.getFunctionManager().registerBuiltinFunction("delay_us", timer_wait_func);

    // ─── System Control (3) ──────────────────────────────────────────────

    // (AR) إعادة تشغيل — reset()
    auto reset_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[OS-SIM] *** SYSTEM RESET ***" << std::endl;
        std::cout << "[OS-SIM] (in simulation: program continues)" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("اعد_تشغيل", reset_func);
    interpreter.getFunctionManager().registerBuiltinFunction("reset", reset_func);
    interpreter.getFunctionManager().registerBuiltinFunction("reboot", reset_func);

    // (AR) معرّف المعالج — cpu_id(leaf?) → value
    auto cpuid_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int leaf = 0;
        if (!args.empty()) leaf = args[0]->toInt();
        std::cout << "[OS-SIM] CPUID leaf=" << leaf << " → 0x000306C3 (simulated i7)" << std::endl;
        return std::make_shared<Data::Value>(0x000306C3); // simulated Haswell i7
    };
    interpreter.getFunctionManager().registerBuiltinFunction("معرف_المعالج", cpuid_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cpu_id", cpuid_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cpuid", cpuid_func);

    // (AR) عداد الدورات — rdtsc() → cycle_count
    auto rdtsc_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto now = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
        int simCycles = static_cast<int>(ns & 0x7FFFFFFF);
        std::cout << "[OS-SIM] RDTSC → " << simCycles << " (simulated cycles)" << std::endl;
        return std::make_shared<Data::Value>(simCycles);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عداد_الدورات", rdtsc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("rdtsc", rdtsc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cycle_count", rdtsc_func);

    // ─── Memory Barriers (3) ────────────────────────────────────────────

    // (AR) حاجز ذاكرة كامل — mfence()
    auto mfence_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[OS-SIM] MFENCE — full memory barrier" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حاجز_ذاكرة", mfence_func);
    interpreter.getFunctionManager().registerBuiltinFunction("memory_barrier", mfence_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mfence", mfence_func);

    // (AR) حاجز قراءة — lfence()
    auto lfence_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[OS-SIM] LFENCE — load fence" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حاجز_قراءة", lfence_func);
    interpreter.getFunctionManager().registerBuiltinFunction("read_barrier", lfence_func);
    interpreter.getFunctionManager().registerBuiltinFunction("lfence", lfence_func);

    // (AR) حاجز كتابة — sfence()
    auto sfence_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[OS-SIM] SFENCE — store fence" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حاجز_كتابة", sfence_func);
    interpreter.getFunctionManager().registerBuiltinFunction("write_barrier", sfence_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sfence", sfence_func);

    // ─── DMA (2) ─────────────────────────────────────────────────────────

    // (AR) تهيئة DMA — dma_init(channel, src, dest, size)
    auto dma_init_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 4) throw std::runtime_error("نقل_مباشر_هيئ: يحتاج 4 معاملات (قناة، مصدر، وجهة، حجم)");
        int ch   = args[0]->toInt();
        int src  = args[1]->toInt();
        int dest = args[2]->toInt();
        int size = args[3]->toInt();
        std::cout << "[OS-SIM] DMA init: ch=" << ch << " src=0x" << std::hex << src 
                  << " dest=0x" << dest << " size=" << std::dec << size << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("نقل_مباشر_هيئ", dma_init_func);
    interpreter.getFunctionManager().registerBuiltinFunction("dma_init", dma_init_func);

    // (AR) بدء نقل DMA — dma_start(channel)
    auto dma_start_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("نقل_مباشر_ابدأ: يحتاج معامل (رقم_القناة)");
        int ch = args[0]->toInt();
        std::cout << "[OS-SIM] DMA start: channel=" << ch << " — transfer in progress" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("نقل_مباشر_ابدأ", dma_start_func);
    interpreter.getFunctionManager().registerBuiltinFunction("dma_start", dma_start_func);

    // ══════════════════════════════════════════════════════════════════════
    // 📱 وحدة تطبيقات الهاتف / Mobile Applications Module
    // ══════════════════════════════════════════════════════════════════════

    // (AR) معلومات الجهاز — هاتف_معلومات()
    auto mobile_info_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto info = sad::stdlib::mobile::get_device_info();
        std::cout << "📱 معلومات الجهاز:" << std::endl;
        std::cout << "  المنصة: " << info.platform << std::endl;
        std::cout << "  إصدار النظام: " << info.os_version << std::endl;
        std::cout << "  اسم الجهاز: " << info.device_name << std::endl;
        std::cout << "  الشاشة: " << info.screen_width << "x" << info.screen_height << std::endl;
        std::cout << "  كثافة الشاشة: " << info.screen_density << std::endl;
        std::cout << "  كاميرا: " << (info.has_camera ? "✓ متوفرة" : "✗ غير متوفرة") << std::endl;
        std::cout << "  GPS: " << (info.has_gps ? "✓ متوفر" : "✗ غير متوفر") << std::endl;
        return std::make_shared<Data::Value>(info.platform);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_معلومات", mobile_info_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mobile_info", mobile_info_func);

    // (AR) اسم المنصة — هاتف_منصة()
    auto mobile_platform_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::stdlib::mobile::get_platform_name());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_منصة", mobile_platform_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mobile_platform", mobile_platform_func);

    // (AR) عرض تنبيه — هاتف_رسالة(عنوان، نص)
    auto mobile_alert_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = "تنبيه";
        std::string message = "";
        if (args.size() >= 2) {
            title = args[0]->toString();
            message = args[1]->toString();
        } else if (args.size() >= 1) {
            message = args[0]->toString();
        }
        bool result = sad::stdlib::mobile::show_alert(title, message);
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_رسالة", mobile_alert_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mobile_alert", mobile_alert_func);

    // (AR) إشعار — هاتف_إشعار(عنوان، نص)
    auto mobile_notify_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = "إشعار";
        std::string message = "";
        if (args.size() >= 2) {
            title = args[0]->toString();
            message = args[1]->toString();
        } else if (args.size() >= 1) {
            message = args[0]->toString();
        }
        bool result = sad::stdlib::mobile::show_notification(title, message);
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_إشعار", mobile_notify_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mobile_notify", mobile_notify_func);

    // (AR) رسالة سريعة — هاتف_رسالة_سريعة(نص)
    auto mobile_toast_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("هاتف_رسالة_سريعة: يحتاج معامل واحد على الأقل");
        bool result = sad::stdlib::mobile::show_toast(args[0]->toString());
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_رسالة_سريعة", mobile_toast_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mobile_toast", mobile_toast_func);

    // (AR) إنشاء نص واجهة — واجهة_نص(محتوى)
    auto ui_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("واجهة_نص: يحتاج معامل واحد على الأقل");
        auto elem = sad::stdlib::mobile::create_text(args[0]->toString());
        return std::make_shared<Data::Value>(sad::stdlib::mobile::render_ui_element(elem));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("واجهة_نص", ui_text_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ui_text", ui_text_func);

    // (AR) إنشاء زر واجهة — واجهة_زر(عنوان)
    auto ui_button_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("واجهة_زر: يحتاج معامل واحد على الأقل");
        auto elem = sad::stdlib::mobile::create_button(args[0]->toString());
        return std::make_shared<Data::Value>(sad::stdlib::mobile::render_ui_element(elem));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("واجهة_زر", ui_button_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ui_button", ui_button_func);

    // (AR) إنشاء صورة واجهة — واجهة_صورة(مصدر)
    auto ui_image_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("واجهة_صورة: يحتاج معامل واحد على الأقل");
        auto elem = sad::stdlib::mobile::create_image(args[0]->toString());
        return std::make_shared<Data::Value>(sad::stdlib::mobile::render_ui_element(elem));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("واجهة_صورة", ui_image_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ui_image", ui_image_func);

    // (AR) تخطيط عمودي — واجهة_عمود()
    auto ui_column_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto elem = sad::stdlib::mobile::create_column({});
        return std::make_shared<Data::Value>(sad::stdlib::mobile::render_ui_element(elem));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("واجهة_عمود", ui_column_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ui_column", ui_column_func);

    // (AR) تخطيط أفقي — واجهة_صف()
    auto ui_row_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto elem = sad::stdlib::mobile::create_row({});
        return std::make_shared<Data::Value>(sad::stdlib::mobile::render_ui_element(elem));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("واجهة_صف", ui_row_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ui_row", ui_row_func);

    // (AR) توليد كود Compose — واجهة_كومبوز(نوع، محتوى)
    auto ui_compose_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("واجهة_كومبوز: يحتاج معامل واحد على الأقل");
        auto elem = sad::stdlib::mobile::create_text(args[0]->toString());
        return std::make_shared<Data::Value>(sad::stdlib::mobile::render_to_compose(elem));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("واجهة_كومبوز", ui_compose_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ui_compose", ui_compose_func);

    // (AR) توليد كود SwiftUI — واجهة_سويفت(نوع، محتوى)
    auto ui_swiftui_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("واجهة_سويفت: يحتاج معامل واحد على الأقل");
        auto elem = sad::stdlib::mobile::create_text(args[0]->toString());
        return std::make_shared<Data::Value>(sad::stdlib::mobile::render_to_swiftui(elem));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("واجهة_سويفت", ui_swiftui_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ui_swiftui", ui_swiftui_func);

    // (AR) التقاط صورة — كاميرا_التقط()
    auto camera_capture_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::stdlib::mobile::capture_photo());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("كاميرا_التقط", camera_capture_func);
    interpreter.getFunctionManager().registerBuiltinFunction("camera_capture", camera_capture_func);

    // (AR) فحص الكاميرا — كاميرا_متوفرة()
    auto camera_available_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::stdlib::mobile::is_camera_available());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("كاميرا_متوفرة", camera_available_func);
    interpreter.getFunctionManager().registerBuiltinFunction("camera_available", camera_available_func);

    // (AR) الموقع الحالي — موقع_حالي()
    auto location_current_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto loc = sad::stdlib::mobile::get_current_location();
        // إرجاع نص يحتوي على إحداثيات الموقع
        std::string result = std::to_string(loc.latitude) + "," + std::to_string(loc.longitude);
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موقع_حالي", location_current_func);
    interpreter.getFunctionManager().registerBuiltinFunction("location_current", location_current_func);

    // (AR) حساب المسافة — موقع_مسافة(خ1، ط1، خ2، ط2)
    auto location_distance_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 4) throw std::runtime_error("موقع_مسافة: يحتاج 4 معاملات (خط_عرض1، خط_طول1، خط_عرض2، خط_طول2)");
        double lat1 = args[0]->toDouble();
        double lon1 = args[1]->toDouble();
        double lat2 = args[2]->toDouble();
        double lon2 = args[3]->toDouble();
        double distance = sad::stdlib::mobile::calculate_distance(lat1, lon1, lat2, lon2);
        return std::make_shared<Data::Value>(distance);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موقع_مسافة", location_distance_func);
    interpreter.getFunctionManager().registerBuiltinFunction("location_distance", location_distance_func);

    // (AR) مستشعر التسارع — مستشعر_تسارع()
    auto accelerometer_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto data = sad::stdlib::mobile::get_accelerometer();
        std::string result = "x:" + std::to_string(data.x) + ",y:" + std::to_string(data.y) + ",z:" + std::to_string(data.z);
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مستشعر_تسارع", accelerometer_func);
    interpreter.getFunctionManager().registerBuiltinFunction("accelerometer", accelerometer_func);

    // (AR) مستشعر الدوران — مستشعر_دوران()
    auto gyroscope_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto data = sad::stdlib::mobile::get_gyroscope();
        std::string result = "x:" + std::to_string(data.x) + ",y:" + std::to_string(data.y) + ",z:" + std::to_string(data.z);
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مستشعر_دوران", gyroscope_func);
    interpreter.getFunctionManager().registerBuiltinFunction("gyroscope", gyroscope_func);

    // (AR) إنشاء مشروع هاتف — هاتف_مشروع_جديد(اسم، منصة)
    auto mobile_create_project_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("هاتف_مشروع_جديد: يحتاج معاملين (اسم، منصة)");
        std::string name = args[0]->toString();
        std::string platform = args[1]->toString();
        bool result = sad::stdlib::mobile::create_mobile_project(name, platform);
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_مشروع_جديد", mobile_create_project_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mobile_create_project", mobile_create_project_func);

    // (AR) Android Manifest — هاتف_مانيفست(معرف_حزمة، اسم_تطبيق)
    auto android_manifest_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("هاتف_مانيفست: يحتاج معاملين (معرف_حزمة، اسم_تطبيق)");
        return std::make_shared<Data::Value>(
            sad::stdlib::mobile::generate_android_manifest(args[0]->toString(), args[1]->toString()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_مانيفست", android_manifest_func);
    interpreter.getFunctionManager().registerBuiltinFunction("android_manifest", android_manifest_func);

    // (AR) iOS Info.plist — هاتف_بلست(اسم_تطبيق، معرف_حزمة)
    auto ios_plist_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("هاتف_بلست: يحتاج معاملين (اسم_تطبيق، معرف_حزمة)");
        return std::make_shared<Data::Value>(
            sad::stdlib::mobile::generate_ios_plist(args[0]->toString(), args[1]->toString()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاتف_بلست", ios_plist_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ios_plist", ios_plist_func);

    // ===================================================================
    // (AR) دوال الأمان / (EN) Security Functions
    // ===================================================================

    // تأكد / assert — يتحقق من صحة شرط ويرمي خطأ إذا فشل
    auto assert_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) تأكد: يحتاج معامل واحد على الأقل / (EN) assert: needs at least 1 argument");
        bool condition = args[0]->toBool();
        if (!condition) {
            std::string message = "(AR) فشل التأكيد / (EN) Assertion failed";
            if (args.size() >= 2) {
                message = "(AR) فشل التأكيد: " + args[1]->toString() + " / (EN) Assertion failed: " + args[1]->toString();
            }
            throw std::runtime_error(message);
        }
        return std::make_shared<Data::Value>(true);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تأكد", assert_func);
    interpreter.getFunctionManager().registerBuiltinFunction("assert", assert_func);

    // تحقق / verify — مثل تأكد لكن يعيد منطقي بدل رمي خطأ
    auto verify_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) تحقق: يحتاج معامل واحد على الأقل / (EN) verify: needs at least 1 argument");
        return std::make_shared<Data::Value>(args[0]->toBool());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تحقق", verify_func);
    interpreter.getFunctionManager().registerBuiltinFunction("verify", verify_func);

    // آمن / is_safe — يتحقق إذا كانت القيمة آمنة (ليست null/فارغة)
    auto is_safe_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(false);
        auto& val = args[0];
        if (!val) return std::make_shared<Data::Value>(false);
        if (val->isVoid()) return std::make_shared<Data::Value>(false);
        return std::make_shared<Data::Value>(true);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("آمن", is_safe_func);
    interpreter.getFunctionManager().registerBuiltinFunction("is_safe", is_safe_func);

    // ذعر / panic — يرمي خطأ ذعر (غير قابل للتعافي عادة)
    auto panic_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string message = "(AR) ذعر! / (EN) Panic!";
        if (!args.empty()) {
            message = "❌ ذعر: " + args[0]->toString() + " / Panic: " + args[0]->toString();
        }
        throw std::runtime_error(message);
        return std::make_shared<Data::Value>(); // unreachable
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ذعر", panic_func);
    interpreter.getFunctionManager().registerBuiltinFunction("panic", panic_func);

    // هاش / hash — يحسب هاش SHA-256 بسيط لنص (Simplified for interpreter)
    auto hash_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) هاش: يحتاج نص / (EN) hash: needs a string");
        std::string input = args[0]->toString();
        // Simple FNV-1a hash (for demonstration — real crypto uses OpenSSL)
        uint64_t hash = 14695981039346656037ULL;
        for (char c : input) {
            hash ^= static_cast<uint64_t>(c);
            hash *= 1099511628211ULL;
        }
        std::stringstream ss;
        ss << std::hex << hash;
        return std::make_shared<Data::Value>(ss.str());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هاش", hash_func);
    interpreter.getFunctionManager().registerBuiltinFunction("hash", hash_func);

    // شفّر / encrypt — تشفير بسيط (XOR مع مفتاح — للتعليم)
    auto encrypt_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) شفّر: يحتاج نص ومفتاح / (EN) encrypt: needs text and key");
        std::string text = args[0]->toString();
        std::string key = args[1]->toString();
        if (key.empty()) throw std::runtime_error("(AR) المفتاح فارغ / (EN) Key is empty");
        std::string result;
        for (size_t i = 0; i < text.size(); ++i) {
            result += static_cast<char>(text[i] ^ key[i % key.size()]);
        }
        // Convert to hex for safe display
        std::stringstream ss;
        for (unsigned char c : result) {
            ss << std::hex << std::setfill('0') << std::setw(2) << (int)c;
        }
        return std::make_shared<Data::Value>(ss.str());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("شفّر", encrypt_func);
    interpreter.getFunctionManager().registerBuiltinFunction("encrypt", encrypt_func);

    // فك_تشفير / decrypt — فك التشفير (XOR — للتعليم)
    auto decrypt_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) فك_تشفير: يحتاج نص_مشفر ومفتاح / (EN) decrypt: needs encrypted_text and key");
        std::string hex_text = args[0]->toString();
        std::string key = args[1]->toString();
        if (key.empty()) throw std::runtime_error("(AR) المفتاح فارغ / (EN) Key is empty");
        // Convert hex back to bytes
        std::string bytes;
        for (size_t i = 0; i + 1 < hex_text.size(); i += 2) {
            int byte = std::stoi(hex_text.substr(i, 2), nullptr, 16);
            bytes += static_cast<char>(byte);
        }
        // XOR decrypt
        std::string result;
        for (size_t i = 0; i < bytes.size(); ++i) {
            result += static_cast<char>(bytes[i] ^ key[i % key.size()]);
        }
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("فك_تشفير", decrypt_func);
    interpreter.getFunctionManager().registerBuiltinFunction("decrypt", decrypt_func);

    // تأكد_نوع / assert_type — يتحقق من نوع القيمة
    auto assert_type_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) تأكد_نوع: يحتاج قيمة واسم_نوع / (EN) assert_type: needs value and type_name");
        std::string actual_type = args[0]->getTypeName();
        std::string expected_type = args[1]->toString();
        // خريطة ترجمة الأنواع العربية ↔ الإنجليزية
        static const std::unordered_map<std::string, std::string> type_map = {
            {"رقم", "INTEGER"}, {"عدد", "INTEGER"}, {"صحيح", "INTEGER"}, {"integer", "INTEGER"}, {"int", "INTEGER"},
            {"عشري", "DOUBLE"}, {"حقيقي", "DOUBLE"}, {"double", "DOUBLE"}, {"float", "DOUBLE"},
            {"نص", "STRING"}, {"سلسلة", "STRING"}, {"string", "STRING"}, {"str", "STRING"},
            {"منطقي", "BOOLEAN"}, {"bool", "BOOLEAN"}, {"boolean", "BOOLEAN"},
            {"مصفوفة", "ARRAY"}, {"array", "ARRAY"}, {"قائمة", "ARRAY"},
            {"قاموس", "MAP"}, {"خريطة", "MAP"}, {"map", "MAP"},
            {"فراغ", "VOID"}, {"لاشيء", "VOID"}, {"void", "VOID"}, {"none", "VOID"}
        };
        std::string normalized = expected_type;
        auto it = type_map.find(expected_type);
        if (it != type_map.end()) {
            normalized = it->second;
        }
        // أيضاً تحويل actual_type لكبيرة للمقارنة
        std::string actual_upper = actual_type;
        for (auto& c : actual_upper) c = toupper(c);
        if (actual_upper != normalized && actual_type != expected_type) {
            throw std::runtime_error("(AR) خطأ نوع: توقعت '" + expected_type + "' لكن وجدت '" + actual_type + "' / (EN) Type error: expected '" + expected_type + "' but got '" + actual_type + "'");
        }
        return std::make_shared<Data::Value>(true);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تأكد_نوع", assert_type_func);
    interpreter.getFunctionManager().registerBuiltinFunction("assert_type", assert_type_func);

    // تأكد_مساواة / assert_equal — يتحقق من مساواة قيمتين
    auto assert_equal_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) تأكد_مساواة: يحتاج قيمتين / (EN) assert_equal: needs 2 values");
        bool equal = (args[0]->toString() == args[1]->toString());
        if (!equal) {
            std::string msg = "(AR) فشل التأكيد: '" + args[0]->toString() + "' لا يساوي '" + args[1]->toString() + "' / (EN) Assertion failed: '" + args[0]->toString() + "' != '" + args[1]->toString() + "'";
            if (args.size() >= 3) {
                msg = "(AR) فشل: " + args[2]->toString() + " / (EN) Failed: " + args[2]->toString();
            }
            throw std::runtime_error(msg);
        }
        return std::make_shared<Data::Value>(true);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تأكد_مساواة", assert_equal_func);
    interpreter.getFunctionManager().registerBuiltinFunction("assert_equal", assert_equal_func);

    // تأكد_أكبر / assert_greater — يتحقق أن القيمة أكبر
    auto assert_greater_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) تأكد_أكبر: يحتاج قيمتين / (EN) assert_greater: needs 2 values");
        double a = args[0]->toDouble();
        double b = args[1]->toDouble();
        if (!(a > b)) {
            throw std::runtime_error("(AR) فشل التأكيد: " + args[0]->toString() + " ليس أكبر من " + args[1]->toString() + " / (EN) Assertion failed: " + args[0]->toString() + " not greater than " + args[1]->toString());
        }
        return std::make_shared<Data::Value>(true);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تأكد_أكبر", assert_greater_func);
    interpreter.getFunctionManager().registerBuiltinFunction("assert_greater", assert_greater_func);

    // نظّف / sanitize — تنظيف نص من الأحرف الخطيرة (XSS/SQL Injection)
    auto sanitize_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) نظّف: يحتاج نص / (EN) sanitize: needs a string");
        std::string input = args[0]->toString();
        std::string result;
        for (char c : input) {
            switch (c) {
                case '<': result += "&lt;"; break;
                case '>': result += "&gt;"; break;
                case '&': result += "&amp;"; break;
                case '"': result += "&quot;"; break;
                case '\'': result += "&#39;"; break;
                case ';': result += ""; break;  // Remove semicolons (SQL injection)
                case '-': 
                    if (!result.empty() && result.back() == '-') { result.pop_back(); break; } // -- comments
                    result += c; break;
                default: result += c; break;
            }
        }
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("نظّف", sanitize_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sanitize", sanitize_func);

    // وقت_الآن / timestamp — الوقت الحالي (Unix timestamp)
    auto timestamp_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto now = std::chrono::system_clock::now();
        auto epoch = now.time_since_epoch();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch).count();
        return std::make_shared<Data::Value>(static_cast<double>(seconds));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("وقت_الآن", timestamp_func);
    interpreter.getFunctionManager().registerBuiltinFunction("timestamp", timestamp_func);

    // عشوائي_آمن / secure_random — عدد عشوائي آمن (crypto-grade)
    auto secure_random_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int min_val = 0, max_val = 100;
        if (args.size() >= 1) min_val = static_cast<int>(args[0]->toDouble());
        if (args.size() >= 2) max_val = static_cast<int>(args[1]->toDouble());
        // Use random_device for crypto-grade randomness
        std::random_device rd;
        std::uniform_int_distribution<int> dist(min_val, max_val);
        return std::make_shared<Data::Value>(static_cast<double>(dist(rd)));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عشوائي_آمن", secure_random_func);
    interpreter.getFunctionManager().registerBuiltinFunction("secure_random", secure_random_func);

    // base64_encode / ترميز_64
    auto base64_encode_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) ترميز_64: يحتاج نص / (EN) base64_encode: needs a string");
        const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string input = args[0]->toString();
        std::string result;
        int val = 0, valb = -6;
        for (unsigned char c : input) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                result.push_back(chars[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6) result.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
        while (result.size() % 4) result.push_back('=');
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ترميز_64", base64_encode_func);
    interpreter.getFunctionManager().registerBuiltinFunction("base64_encode", base64_encode_func);

    // ===================================================================
    // القسم 13: دوال FFI (واجهة الدوال الخارجية C/C++)
    // Section 13: FFI Functions (C/C++ Foreign Function Interface)
    // ===================================================================

    // 1. printf — طباعة_تنسيق / formatted print
    auto ffi_printf_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) printf: يحتاج نص التنسيق / (EN) printf: needs format string");
        std::string fmt = args[0]->toString();
        // Simple printf simulation: replace %d, %s, %f, %p with argument values
        std::string result;
        size_t argIdx = 1;
        for (size_t i = 0; i < fmt.size(); i++) {
            if (fmt[i] == '%' && i + 1 < fmt.size()) {
                char spec = fmt[i + 1];
                if (spec == 'd' || spec == 'i') {
                    if (argIdx < args.size()) result += std::to_string(static_cast<int>(args[argIdx++]->toDouble()));
                    else result += "0";
                    i++;
                } else if (spec == 'f') {
                    if (argIdx < args.size()) {
                        char buf[64]; std::snprintf(buf, sizeof(buf), "%f", args[argIdx++]->toDouble());
                        result += buf;
                    } else result += "0.000000";
                    i++;
                } else if (spec == 's') {
                    if (argIdx < args.size()) result += args[argIdx++]->toString();
                    else result += "(null)";
                    i++;
                } else if (spec == 'p') {
                    if (argIdx < args.size()) {
                        char buf[32]; std::snprintf(buf, sizeof(buf), "%p", (void*)(uintptr_t)static_cast<int64_t>(args[argIdx++]->toDouble()));
                        result += buf;
                    } else result += "0x0";
                    i++;
                } else if (spec == '%') {
                    result += '%'; i++;
                } else {
                    result += '%';
                }
            } else if (fmt[i] == '\\' && i + 1 < fmt.size() && fmt[i + 1] == 'n') {
                result += '\n'; i++;
            } else {
                result += fmt[i];
            }
        }
        std::cout << result;
        return std::make_shared<Data::Value>(static_cast<int>(result.size()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("طباعة_تنسيق", ffi_printf_func);
    interpreter.getFunctionManager().registerBuiltinFunction("printf", ffi_printf_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_printf", ffi_printf_func);

    // 2. malloc — حجز / allocate memory (simulated with unique ID)
    auto ffi_malloc_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) malloc: يحتاج الحجم / (EN) malloc: needs size");
        size_t size = static_cast<size_t>(args[0]->toDouble());
        void* ptr = std::malloc(size);
        if (!ptr) return std::make_shared<Data::Value>(0);
        return std::make_shared<Data::Value>(static_cast<double>(reinterpret_cast<uintptr_t>(ptr)));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حجز", ffi_malloc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("malloc", ffi_malloc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_malloc", ffi_malloc_func);

    // 3. free — حرر / free memory
    auto ffi_free_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) free: يحتاج مؤشر / (EN) free: needs pointer");
        uintptr_t addr = static_cast<uintptr_t>(args[0]->toDouble());
        if (addr != 0) std::free(reinterpret_cast<void*>(addr));
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حرر", ffi_free_func);
    interpreter.getFunctionManager().registerBuiltinFunction("free", ffi_free_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_free", ffi_free_func);

    // 4. realloc — اعد_حجز
    auto ffi_realloc_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) realloc: يحتاج مؤشر وحجم / (EN) realloc: needs ptr and size");
        uintptr_t addr = static_cast<uintptr_t>(args[0]->toDouble());
        size_t size = static_cast<size_t>(args[1]->toDouble());
        void* ptr = std::realloc(reinterpret_cast<void*>(addr), size);
        return std::make_shared<Data::Value>(static_cast<double>(reinterpret_cast<uintptr_t>(ptr)));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("اعد_حجز", ffi_realloc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("realloc", ffi_realloc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_realloc", ffi_realloc_func);

    // 5. calloc — حجز_صفري
    auto ffi_calloc_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) calloc: يحتاج العدد والحجم / (EN) calloc: needs count and size");
        size_t count = static_cast<size_t>(args[0]->toDouble());
        size_t size = static_cast<size_t>(args[1]->toDouble());
        void* ptr = std::calloc(count, size);
        return std::make_shared<Data::Value>(static_cast<double>(reinterpret_cast<uintptr_t>(ptr)));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حجز_صفري", ffi_calloc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("calloc", ffi_calloc_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_calloc", ffi_calloc_func);

    // 6. strlen — طول_نص_س
    auto ffi_strlen_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) strlen: يحتاج نص / (EN) strlen: needs string");
        std::string s = args[0]->toString();
        return std::make_shared<Data::Value>(static_cast<double>(s.length()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("طول_نص_س", ffi_strlen_func);
    interpreter.getFunctionManager().registerBuiltinFunction("strlen", ffi_strlen_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_strlen", ffi_strlen_func);

    // 7. strcpy — انسخ_نص_س
    auto ffi_strcpy_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) strcpy: يحتاج نص / (EN) strcpy: needs string");
        // In interpreted mode, just return a copy of the string
        return std::make_shared<Data::Value>(args[0]->toString());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("انسخ_نص_س", ffi_strcpy_func);
    interpreter.getFunctionManager().registerBuiltinFunction("strcpy", ffi_strcpy_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_strcpy", ffi_strcpy_func);

    // 8. strcmp — قارن_نص_س
    auto ffi_strcmp_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) strcmp: يحتاج نصين / (EN) strcmp: needs two strings");
        int result = args[0]->toString().compare(args[1]->toString());
        return std::make_shared<Data::Value>(static_cast<double>(result));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("قارن_نص_س", ffi_strcmp_func);
    interpreter.getFunctionManager().registerBuiltinFunction("strcmp", ffi_strcmp_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_strcmp", ffi_strcmp_func);

    // 9. strcat — الحق_نص_س
    auto ffi_strcat_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) strcat: يحتاج نصين / (EN) strcat: needs two strings");
        return std::make_shared<Data::Value>(args[0]->toString() + args[1]->toString());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("الحق_نص_س", ffi_strcat_func);
    interpreter.getFunctionManager().registerBuiltinFunction("strcat", ffi_strcat_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_strcat", ffi_strcat_func);

    // 10. memcpy — انسخ_ذاكرة_س (simulated with string copy in interpreter)
    auto ffi_memcpy_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) memcpy: يحتاج مصدر وحجم / (EN) memcpy: needs source and size");
        // In interpreted mode, copy string bytes
        std::string src = args[0]->toString();
        size_t n = args.size() >= 3 ? static_cast<size_t>(args[2]->toDouble()) : src.size();
        return std::make_shared<Data::Value>(src.substr(0, n));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("انسخ_ذاكرة_س", ffi_memcpy_func);
    interpreter.getFunctionManager().registerBuiltinFunction("memcpy", ffi_memcpy_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_memcpy", ffi_memcpy_func);

    // 11. memset — عبئ_ذاكرة_س (simulated)
    auto ffi_memset_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) throw std::runtime_error("(AR) memset: يحتاج مؤشر وقيمة وحجم / (EN) memset: needs ptr, value, size");
        char ch = static_cast<char>(static_cast<int>(args[1]->toDouble()));
        size_t n = static_cast<size_t>(args[2]->toDouble());
        return std::make_shared<Data::Value>(std::string(n, ch));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عبئ_ذاكرة_س", ffi_memset_func);
    interpreter.getFunctionManager().registerBuiltinFunction("memset", ffi_memset_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_memset", ffi_memset_func);

    // 12. fopen — افتح_ملف_س
    auto ffi_fopen_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) fopen: يحتاج اسم الملف والوضع / (EN) fopen: needs filename and mode");
        std::string filename = args[0]->toString();
        std::string mode = args[1]->toString();
        FILE* fp = std::fopen(filename.c_str(), mode.c_str());
        if (!fp) return std::make_shared<Data::Value>(0);
        return std::make_shared<Data::Value>(static_cast<double>(reinterpret_cast<uintptr_t>(fp)));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("افتح_ملف_س", ffi_fopen_func);
    interpreter.getFunctionManager().registerBuiltinFunction("fopen", ffi_fopen_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_fopen", ffi_fopen_func);

    // 13. fclose — اغلق_ملف_س
    auto ffi_fclose_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) fclose: يحتاج مؤشر ملف / (EN) fclose: needs file pointer");
        uintptr_t addr = static_cast<uintptr_t>(args[0]->toDouble());
        if (addr == 0) return std::make_shared<Data::Value>(-1);
        int result = std::fclose(reinterpret_cast<FILE*>(addr));
        return std::make_shared<Data::Value>(static_cast<double>(result));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("اغلق_ملف_س", ffi_fclose_func);
    interpreter.getFunctionManager().registerBuiltinFunction("fclose", ffi_fclose_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_fclose", ffi_fclose_func);

    // 14. fputs — اكتب_ملف_س
    auto ffi_fputs_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) fputs: يحتاج نص ومؤشر ملف / (EN) fputs: needs string and file pointer");
        std::string text = args[0]->toString();
        uintptr_t addr = static_cast<uintptr_t>(args[1]->toDouble());
        if (addr == 0) return std::make_shared<Data::Value>(-1);
        int result = std::fputs(text.c_str(), reinterpret_cast<FILE*>(addr));
        return std::make_shared<Data::Value>(static_cast<double>(result));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("اكتب_ملف_س", ffi_fputs_func);
    interpreter.getFunctionManager().registerBuiltinFunction("fputs", ffi_fputs_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_fputs", ffi_fputs_func);

    // 15. fgets — اقرأ_ملف_س
    auto ffi_fgets_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("(AR) fgets: يحتاج حجم ومؤشر ملف / (EN) fgets: needs size and file pointer");
        int size = static_cast<int>(args[0]->toDouble());
        uintptr_t addr = static_cast<uintptr_t>(args[1]->toDouble());
        if (addr == 0 || size <= 0) return std::make_shared<Data::Value>(std::string(""));
        std::vector<char> buf(size);
        char* result = std::fgets(buf.data(), size, reinterpret_cast<FILE*>(addr));
        if (!result) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(std::string(buf.data()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("اقرأ_ملف_س", ffi_fgets_func);
    interpreter.getFunctionManager().registerBuiltinFunction("fgets", ffi_fgets_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_fgets", ffi_fgets_func);

    // 16. system — نفذ_امر
    auto ffi_system_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) system: يحتاج أمر / (EN) system: needs command");
        std::string cmd = args[0]->toString();
        int result = std::system(cmd.c_str());
        return std::make_shared<Data::Value>(static_cast<double>(result));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("نفذ_امر", ffi_system_func);
    interpreter.getFunctionManager().registerBuiltinFunction("system", ffi_system_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_system", ffi_system_func);

    // 17. getenv — قيمة_بيئة
    auto ffi_getenv_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) getenv: يحتاج اسم المتغير / (EN) getenv: needs variable name");
        const char* val = std::getenv(args[0]->toString().c_str());
        if (!val) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(std::string(val));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("قيمة_بيئة", ffi_getenv_func);
    interpreter.getFunctionManager().registerBuiltinFunction("getenv", ffi_getenv_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_getenv", ffi_getenv_func);

    // 18. atoi — نص_لعدد
    auto ffi_atoi_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) atoi: يحتاج نص / (EN) atoi: needs string");
        int result = std::atoi(args[0]->toString().c_str());
        return std::make_shared<Data::Value>(static_cast<double>(result));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("نص_لعدد", ffi_atoi_func);
    interpreter.getFunctionManager().registerBuiltinFunction("atoi", ffi_atoi_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_atoi", ffi_atoi_func);

    // 19. atof — نص_لعشري
    auto ffi_atof_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) atof: يحتاج نص / (EN) atof: needs string");
        double result = std::atof(args[0]->toString().c_str());
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("نص_لعشري", ffi_atof_func);
    interpreter.getFunctionManager().registerBuiltinFunction("atof", ffi_atof_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_atof", ffi_atof_func);

    // 20. snprintf — تنسيق_نص
    auto ffi_snprintf_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("(AR) snprintf: يحتاج نص التنسيق / (EN) snprintf: needs format string");
        std::string fmt = args[0]->toString();
        // Reuse printf-style formatting to produce a string
        std::string result;
        size_t argIdx = 1;
        for (size_t i = 0; i < fmt.size(); i++) {
            if (fmt[i] == '%' && i + 1 < fmt.size()) {
                char spec = fmt[i + 1];
                if (spec == 'd' || spec == 'i') {
                    if (argIdx < args.size()) result += std::to_string(static_cast<int>(args[argIdx++]->toDouble()));
                    else result += "0";
                    i++;
                } else if (spec == 'f') {
                    if (argIdx < args.size()) {
                        char buf[64]; std::snprintf(buf, sizeof(buf), "%f", args[argIdx++]->toDouble());
                        result += buf;
                    } else result += "0.000000";
                    i++;
                } else if (spec == 's') {
                    if (argIdx < args.size()) result += args[argIdx++]->toString();
                    else result += "(null)";
                    i++;
                } else if (spec == '%') {
                    result += '%'; i++;
                } else {
                    result += '%';
                }
            } else if (fmt[i] == '\\' && i + 1 < fmt.size() && fmt[i + 1] == 'n') {
                result += '\n'; i++;
            } else {
                result += fmt[i];
            }
        }
        return std::make_shared<Data::Value>(result);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تنسيق_نص", ffi_snprintf_func);
    interpreter.getFunctionManager().registerBuiltinFunction("snprintf", ffi_snprintf_func);
    interpreter.getFunctionManager().registerBuiltinFunction("c_snprintf", ffi_snprintf_func);
}

} // namespace Interpreter
} // namespace Sad
