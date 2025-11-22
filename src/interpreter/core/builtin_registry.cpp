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
#include <memory>
#include <iostream>

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
    
    interpreter.getFunctionManager().registerBuiltinFunction("لنص", to_string_func);
    interpreter.getFunctionManager().registerBuiltinFunction("to_string", to_string_func);
    interpreter.getFunctionManager().registerBuiltinFunction("str", to_string_func);
    
    // ===================================================================
    // (AR) دوال المدى والتكرار / (EN) Range & Iteration Functions
    // ===================================================================
    
    auto range_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::range(args);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("مدى", range_func);
    interpreter.getFunctionManager().registerBuiltinFunction("range", range_func);
}

} // namespace Interpreter
} // namespace Sad
