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
// ═══════════════════════════════════════════════════════════════════════════
// أنظمة الرسومات المتقدمة — كل نظام يضيف قدرات غير موجودة في Flutter
// ═══════════════════════════════════════════════════════════════════════════
#include "graphics/sad_particles.h"      // نظام الجسيمات — نار، دخان، مطر، ثلج
#include "graphics/sad_effects.h"        // التأثيرات البصرية — ظل، توهج، زجاج
#include "graphics/sad_physics.h"        // محرك الفيزياء — تصادم، جاذبية، نوابض
#include "graphics/sad_scene3d.h"        // المشاهد ثلاثية الأبعاد — رسم 3D فوق SDL2
#include "graphics/sad_ai.h"             // الذكاء الاصطناعي — إيجاد المسار، آلة الحالات
#include "graphics/sad_camera2d.h"       // الكاميرا ثنائية الأبعاد — تتبع، اهتزاز، تكبير
#include "graphics/sad_charts.h"         // الرسوم البيانية — أعمدة، خطوط، دائري
#include "graphics/sad_audio.h"          // النظام الصوتي — نغمات، مؤثرات صوتية
#include "graphics/sad_tilemap.h"        // خرائط البلاط — ألعاب ثنائية الأبعاد
// ═══════════════════════════════════════════════════════════════════════════
// الأنظمة الإسلامية — أول مكتبة برمجة في العالم بدعم إسلامي مدمج
// ═══════════════════════════════════════════════════════════════════════════
#include "graphics/sad_islamic_art.h"    // الفن الإسلامي — أنماط هندسية، أرابيسك
#include "graphics/sad_dhikr.h"          // الأذكار والتسبيح — عداد، أذكار الصباح والمساء
#include "graphics/sad_qibla.h"          // اتجاه القبلة — بوصلة، حساب الزاوية
#include "graphics/sad_prayer.h"         // أوقات الصلاة — حساب فلكي دقيق
#include "graphics/sad_hijri.h"          // التقويم الهجري — تحويل، مناسبات
#include "graphics/sad_android.h"        // دعم أندرويد — إشعارات، موقع، بطارية
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
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <unordered_map>
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
    
    // ╔═══════════════════════════════════════════════════════════════════════╗
    // ║                                                                       ║
    // ║   الموجة الأولى: أنظمة الرسومات المتقدمة                              ║
    // ║   Wave 1: Advanced Graphics Systems                                   ║
    // ║                                                                       ║
    // ║   تتضمن أربعة أنظمة فرعية قوية:                                       ║
    // ║     1) نظام الجسيمات (Particles) — نار، دخان، مطر، ثلج، انفجار       ║
    // ║     2) نظام التأثيرات البصرية (Effects) — ظل، توهج، تدرج، زجاج       ║
    // ║     3) محرك الفيزياء (Physics) — تصادم، جاذبية، نوابض                 ║
    // ║     4) المشاهد ثلاثية الأبعاد (Scene3D) — مكعب، كرة، كاميرا، إضاءة   ║
    // ║                                                                       ║
    // ║   كل دالة مسجلة باسم عربي وإنجليزي لتحقيق ثنائية اللغة الكاملة      ║
    // ║                                                                       ║
    // ╚═══════════════════════════════════════════════════════════════════════╝

    // ===================================================================
    // النظام 1: الجسيمات — sad::particles
    // ===================================================================
    //
    // نظام الجسيمات يوفر القدرة على إنشاء تأثيرات بصرية مذهلة مثل
    // النار والدخان والمطر والثلج والانفجارات والمزيد. كل مُصدِر جسيمات
    // يمكن تخصيصه بالكامل من حيث اللون والسرعة والجاذبية والشكل.
    // هذا النظام لا يوجد نظيره في Flutter على الإطلاق!
    // ===================================================================

    // ─── أنشئ_مصدر_جسيمات ───
    // إنشاء مُصدِر جسيمات جديد في الموضع (س، ص) المحدد.
    // المُصدِر هو المصدر الذي تنطلق منه الجسيمات — يمكنك إنشاء عدة مُصدِرات
    // في المشهد الواحد لتأثيرات مختلفة (مثلاً: نار هنا ودخان هناك).
    // يُرجع معرّف المُصدِر الذي يُستخدم في باقي الدوال للتحكم به.
    auto particles_createEmitter_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        int id = sad::particles::createEmitter(x, y);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_مصدر_جسيمات", particles_createEmitter_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_create_emitter", particles_createEmitter_func);

    // ─── احذف_مصدر ───
    // حذف مُصدِر جسيمات بمعرّفه. عند الحذف تختفي جميع الجسيمات
    // المرتبطة به فوراً من المشهد ويُحرَّر الذاكرة المخصصة له.
    // يُستخدم عندما لا نعود بحاجة لتأثير معين (مثلاً: انتهاء الانفجار).
    auto particles_destroyEmitter_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        sad::particles::destroyEmitter(id);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_مصدر", particles_destroyEmitter_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_destroy_emitter", particles_destroyEmitter_func);

    // ─── احذف_كل_المصادر ───
    // حذف جميع مُصدِرات الجسيمات الموجودة في المشهد دفعة واحدة.
    // مفيد عند الانتقال بين المشاهد أو عند إعادة تعيين اللعبة بالكامل
    // لتنظيف كل التأثيرات البصرية القديمة.
    auto particles_destroyAllEmitters_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        sad::particles::destroyAllEmitters();
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_كل_المصادر", particles_destroyAllEmitters_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_destroy_all_emitters", particles_destroyAllEmitters_func);

    // ─── ابدأ_جسيمات ───
    // بدء إصدار الجسيمات من المُصدِر المحدد. بعد إنشاء المُصدِر يكون
    // متوقفاً بشكل افتراضي — يجب استدعاء هذه الدالة لبدء تدفق الجسيمات.
    // بمجرد البدء يستمر المُصدِر في إطلاق الجسيمات حسب معدل الإصدار المحدد.
    auto particles_startEmitter_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        sad::particles::startEmitter(id);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ابدأ_جسيمات", particles_startEmitter_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_start_emitter", particles_startEmitter_func);

    // ─── أوقف_جسيمات ───
    // إيقاف إصدار جسيمات جديدة من المُصدِر المحدد. الجسيمات الموجودة
    // حالياً تستمر في حركتها حتى تنتهي أعمارها وتختفي تدريجياً.
    // يمكن إعادة تشغيل المُصدِر لاحقاً بالدالة ابدأ_جسيمات.
    auto particles_stopEmitter_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        sad::particles::stopEmitter(id);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أوقف_جسيمات", particles_stopEmitter_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_stop_emitter", particles_stopEmitter_func);

    // ─── دفعة_جسيمات ───
    // إطلاق دفعة واحدة من الجسيمات بعدد محدد. على عكس الإصدار المستمر،
    // هذه الدالة تطلق كمية محددة مرة واحدة ثم تتوقف. مثالية لتأثيرات
    // مثل الانفجار أو ضربة السيف أو جمع عنصر في اللعبة.
    auto particles_burstEmitter_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        int count = (int)args[1]->toDouble();
        sad::particles::burstEmitter(id, count);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("دفعة_جسيمات", particles_burstEmitter_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_burst_emitter", particles_burstEmitter_func);

    // ─── عيّن_معدل_إصدار ───
    // تعيين عدد الجسيمات التي يُصدرها المُصدِر في الثانية الواحدة.
    // القيم الأعلى تعطي تأثيراً أكثف (مثل نار شديدة) والقيم الأقل
    // تعطي تأثيراً أخف (مثل بخار خفيف). القيمة الافتراضية عادةً 50.
    auto particles_setEmitRate_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float rate = (float)args[1]->toDouble();
        sad::particles::setEmitRate(id, rate);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_معدل_إصدار", particles_setEmitRate_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_set_emit_rate", particles_setEmitRate_func);

    // ─── عيّن_جاذبية_جسيمات ───
    // تعيين قوة الجاذبية المؤثرة على جسيمات مُصدِر معين.
    // الجاذبية تسحب الجسيمات في الاتجاه المحدد — مثلاً:
    // (0, 100) تسحب للأسفل (للنار التي تصعد استخدم 0, -100)
    // (50, 0) تسحب لليمين (لمحاكاة الرياح الجانبية).
    auto particles_setGravity_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float gx = (float)args[1]->toDouble();
        float gy = (float)args[2]->toDouble();
        sad::particles::setGravity(id, gx, gy);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_جاذبية_جسيمات", particles_setGravity_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_set_gravity", particles_setGravity_func);

    // ─── عيّن_رياح_جسيمات ───
    // تعيين قوة الرياح المؤثرة على الجسيمات. الرياح تضيف قوة ثابتة
    // في الاتجاه المحدد، مما يجعل الجسيمات تنحرف. مثلاً: رياح (20, 0)
    // تجعل الدخان ينحرف لليمين، ورياح (0, -10) تجعل الثلج يطير للأعلى قليلاً.
    auto particles_setWind_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float wx = (float)args[1]->toDouble();
        float wy = (float)args[2]->toDouble();
        sad::particles::setWind(id, wx, wy);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_رياح_جسيمات", particles_setWind_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_set_wind", particles_setWind_func);

    // ─── عيّن_لون_بداية ───
    // تعيين اللون الذي يبدأ به كل جسيم عند ولادته (RGBA).
    // الجسيمات تتدرج من لون البداية إلى لون النهاية خلال عمرها.
    // مثلاً: للنار نبدأ بأصفر مشرق (255, 200, 50) وننتهي بأحمر داكن.
    // المعامل الرابع (الشفافية) اختياري وقيمته الافتراضية 255 (معتم بالكامل).
    auto particles_setColorStart_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        uint8_t r = (uint8_t)args[1]->toDouble();
        uint8_t g = (uint8_t)args[2]->toDouble();
        uint8_t b = (uint8_t)args[3]->toDouble();
        uint8_t a = args.size() > 4 ? (uint8_t)args[4]->toDouble() : 255;
        sad::particles::setColorStart(id, r, g, b, a);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_لون_بداية", particles_setColorStart_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_set_color_start", particles_setColorStart_func);

    // ─── عيّن_لون_نهاية ───
    // تعيين اللون الذي ينتهي به كل جسيم عند اقتراب موته (RGBA).
    // الجسيمات تتدرج تلقائياً من لون البداية إلى هذا اللون.
    // المعامل الرابع (الشفافية) اختياري وقيمته الافتراضية 0 (شفاف بالكامل)
    // مما يجعل الجسيم يتلاشى قبل اختفائه.
    auto particles_setColorEnd_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        uint8_t r = (uint8_t)args[1]->toDouble();
        uint8_t g = (uint8_t)args[2]->toDouble();
        uint8_t b = (uint8_t)args[3]->toDouble();
        uint8_t a = args.size() > 4 ? (uint8_t)args[4]->toDouble() : 0;
        sad::particles::setColorEnd(id, r, g, b, a);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_لون_نهاية", particles_setColorEnd_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_set_color_end", particles_setColorEnd_func);

    // ─── عيّن_موضع_مصدر ───
    // تغيير موضع مُصدِر الجسيمات أثناء التشغيل. مفيد لجعل التأثير
    // يتبع كائناً متحركاً (مثل ذيل صاروخ أو شعلة في يد شخصية اللعبة).
    // الجسيمات الموجودة بالفعل لا تتأثر — فقط الجسيمات الجديدة تنطلق من الموضع الجديد.
    auto particles_setPosition_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float x = (float)args[1]->toDouble();
        float y = (float)args[2]->toDouble();
        sad::particles::setPosition(id, x, y);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_موضع_مصدر", particles_setPosition_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_set_position", particles_setPosition_func);

    // ─── عدد_الجسيمات ───
    // الحصول على عدد الجسيمات الحية حالياً في مُصدِر معين.
    // مفيد لمراقبة الأداء أو لتحديد متى انتهى تأثير معين
    // (عندما يصل العدد إلى صفر بعد إيقاف المُصدِر).
    auto particles_getParticleCount_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        int count = sad::particles::getParticleCount(id);
        return std::make_shared<Data::Value>((double)count);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عدد_الجسيمات", particles_getParticleCount_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_get_count", particles_getParticleCount_func);

    // ─── حدّث_جسيمات ───
    // تحديث فيزياء جميع الجسيمات ورسمها على المُصيِّر (renderer).
    // يجب استدعاء هذه الدالة في كل إطار (frame) من حلقة اللعبة الرئيسية.
    // المعامل الأول: مؤشر المُصيِّر (renderer) كقيمة رقمية.
    // المعامل الثاني: الزمن المنقضي منذ آخر إطار بالملي ثانية (مثلاً: 16.67 لـ 60 إطار/ثانية).
    // هذه الدالة تحدّث حركة الجسيمات وتطبّق الجاذبية والرياح ثم ترسمها.
    auto particles_updateAndRenderAll_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        double deltaMs = args[1]->toDouble();
        sad::particles::updateAndRenderAll(renderer, deltaMs);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حدّث_جسيمات", particles_updateAndRenderAll_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_update_and_render", particles_updateAndRenderAll_func);

    // ─── التأثيرات الجاهزة (Presets) ───
    // هذه مجموعة من التأثيرات المُعدَّة مسبقاً والتي يمكن استخدامها بسطر واحد.
    // كل تأثير يُنشئ مُصدِر جسيمات مُهيَّأ تلقائياً بالإعدادات المثالية.
    // تُرجع جميعها معرّف المُصدِر للتحكم به لاحقاً.

    // ─── نار 🔥 ───
    // إنشاء تأثير نار واقعي في الموضع المحدد. اللهب يصعد للأعلى مع
    // تدرج من الأصفر المشرق إلى الأحمر الداكن. المعامل الثالث يتحكم
    // بشدة النار (1.0 = عادية، 2.0 = مضاعفة، 0.5 = خفيفة).
    auto particles_presetFire_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float intensity = args.size() > 2 ? (float)args[2]->toDouble() : 1.0f;
        int id = sad::particles::presetFire(x, y, intensity);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("نار", particles_presetFire_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_fire", particles_presetFire_func);

    // ─── دخان 💨 ───
    // إنشاء تأثير دخان — سحابة رمادية تصعد ببطء وتتلاشى تدريجياً.
    // مثالي للبراكين والمداخن ومحركات السيارات والحرائق المنطفئة.
    // المعامل الثالث يتحكم بكثافة الدخان (عدد الجسيمات وحجمها).
    auto particles_presetSmoke_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float density = args.size() > 2 ? (float)args[2]->toDouble() : 1.0f;
        int id = sad::particles::presetSmoke(x, y, density);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("دخان", particles_presetSmoke_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_smoke", particles_presetSmoke_func);

    // ─── مطر 🌧️ ───
    // إنشاء تأثير مطر يغطي عرض الشاشة المحدد. القطرات تتساقط
    // من الأعلى بسرعة عالية مع ميلان خفيف يحاكي الرياح.
    // المعامل الأول: عرض منطقة المطر. المعامل الثاني: شدة المطر.
    auto particles_presetRain_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float width = (float)args[0]->toDouble();
        float intensity = args.size() > 1 ? (float)args[1]->toDouble() : 1.0f;
        int id = sad::particles::presetRain(width, intensity);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مطر", particles_presetRain_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_rain", particles_presetRain_func);

    // ─── ثلج ❄️ ───
    // إنشاء تأثير ثلج — رقائق بيضاء تتساقط ببطء مع تمايل أفقي طفيف.
    // يعطي شعوراً بالشتاء والبرد. المعامل الأول: عرض منطقة الثلج.
    // المعامل الثاني: شدة التساقط (كلما زادت كلما زاد عدد الرقائق).
    auto particles_presetSnow_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float width = (float)args[0]->toDouble();
        float intensity = args.size() > 1 ? (float)args[1]->toDouble() : 1.0f;
        int id = sad::particles::presetSnow(width, intensity);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ثلج", particles_presetSnow_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_snow", particles_presetSnow_func);

    // ─── كونفيتي 🎊 ───
    // إنشاء تأثير كونفيتي احتفالي — قصاصات ملونة تتطاير في كل الاتجاهات.
    // مثالي لشاشات الفوز والاحتفالات. المعامل الثالث يحدد عدد القصاصات
    // (الافتراضي 100 قصاصة).
    auto particles_presetConfetti_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        int count = args.size() > 2 ? (int)args[2]->toDouble() : 100;
        int id = sad::particles::presetConfetti(x, y, count);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("كونفيتي", particles_presetConfetti_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_confetti", particles_presetConfetti_func);

    // ─── شرارات ✨ ───
    // إنشاء تأثير شرارات متألقة — نقاط مضيئة تظهر وتختفي في دائرة.
    // مثالي للعناصر السحرية والنجوم والجوائز. المعامل الثالث يحدد
    // نصف قطر الدائرة التي تظهر فيها الشرارات (الافتراضي 50 بكسل).
    auto particles_presetSparkle_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float radius = args.size() > 2 ? (float)args[2]->toDouble() : 50.0f;
        int id = sad::particles::presetSparkle(x, y, radius);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("شرارات", particles_presetSparkle_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_sparkle", particles_presetSparkle_func);

    // ─── انفجار 💥 ───
    // إنشاء تأثير انفجار — دفعة واحدة من الجسيمات تتطاير في كل الاتجاهات.
    // مثالي لتدمير الأعداء والانفجارات الكبيرة. المعامل الثالث يتحكم بقوة
    // الانفجار (سرعة الجسيمات وعددها). ينتهي تلقائياً بعد انطلاق الجسيمات.
    auto particles_presetExplosion_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float power = args.size() > 2 ? (float)args[2]->toDouble() : 1.0f;
        int id = sad::particles::presetExplosion(x, y, power);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("انفجار", particles_presetExplosion_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_explosion", particles_presetExplosion_func);

    // ─── فقاعات 🌊 ───
    // إنشاء تأثير فقاعات صاعدة — دوائر شفافة تصعد ببطء وتتمايل.
    // مثالي للمشاهد المائية وأحواض السمك والمشروبات الغازية.
    // المعامل الثالث يحدد معدل إنتاج الفقاعات في الثانية (الافتراضي 10).
    auto particles_presetBubbles_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float rate = args.size() > 2 ? (float)args[2]->toDouble() : 10.0f;
        int id = sad::particles::presetBubbles(x, y, rate);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("فقاعات", particles_presetBubbles_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_bubbles", particles_presetBubbles_func);

    // ─── مسار_نجمي ⭐ ───
    // إنشاء تأثير مسار نجمي — جسيمات تتبع نقطة معينة (مثل مؤشر الفأرة).
    // كل جسيم يظهر خلف الحركة ويتلاشى تدريجياً مكوّناً ذيلاً لامعاً.
    // لتحريك المسار استخدم عيّن_موضع_مصدر لتحديث الموضع في كل إطار.
    auto particles_presetTrail_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        int id = sad::particles::presetTrail(x, y);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مسار_نجمي", particles_presetTrail_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_trail", particles_presetTrail_func);

    // ─── بتلات 🌸 ───
    // إنشاء تأثير بتلات أزهار متساقطة — أوراق زهرية تهبط ببطء مع
    // تمايل أنيق يحاكي سقوط أوراق الكرز اليابانية. يعطي إحساساً
    // رومانسياً وجميلاً. المعامل الأول: عرض المنطقة. الثاني: الشدة.
    auto particles_presetPetals_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float width = (float)args[0]->toDouble();
        float intensity = args.size() > 1 ? (float)args[1]->toDouble() : 1.0f;
        int id = sad::particles::presetPetals(width, intensity);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("بتلات", particles_presetPetals_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_petals", particles_presetPetals_func);

    // ===================================================================
    // النظام 2: التأثيرات البصرية — sad::effects
    // ===================================================================
    //
    // نظام التأثيرات البصرية يوفر مجموعة غنية من التأثيرات التي لا
    // تتوفر في Flutter افتراضياً. يشمل الظلال الناعمة والتوهج
    // والتدرجات اللونية والزجاج الشفاف والنيومورفيزم وفلاتر الألوان
    // وتأثيرات التموج والحدود المتدرجة — كلها بأداء عالٍ فوق SDL2.
    // ===================================================================

    // ─── ارسم_ظل ───
    // رسم ظل ناعم خلف عنصر مستطيل. الظل يعطي إحساساً بالعمق والارتفاع.
    // المعاملات: المُصيِّر، موضع وأبعاد المستطيل (x, y, w, h)،
    // إزاحة الظل (أفقي، عمودي)، مقدار الضبابية، لون الظل (أحمر، أخضر، أزرق)،
    // وشفافية الظل (0.0 = شفاف تماماً، 1.0 = معتم تماماً).
    auto effects_drawShadowSimple_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        float offX = (float)args[5]->toDouble();
        float offY = (float)args[6]->toDouble();
        float blur = (float)args[7]->toDouble();
        uint8_t r = (uint8_t)args[8]->toDouble();
        uint8_t g = (uint8_t)args[9]->toDouble();
        uint8_t b = (uint8_t)args[10]->toDouble();
        float alpha = (float)args[11]->toDouble();
        sad::effects::drawShadowSimple(renderer, x, y, w, h, offX, offY, blur, r, g, b, alpha);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_ظل", effects_drawShadowSimple_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_shadow", effects_drawShadowSimple_func);

    // ─── ارسم_توهج ───
    // رسم تأثير توهج (Glow) حول عنصر مستطيل. التوهج يعطي انطباعاً بأن
    // العنصر يشع ضوءاً. مثالي لإبراز الأزرار النشطة أو العناصر المهمة.
    // المعاملات: المُصيِّر، الموضع والأبعاد، نصف قطر التوهج، اللون (أحمر، أخضر، أزرق)،
    // وشدة التوهج (كلما زادت كلما كان التوهج أوضح).
    auto effects_drawGlowSimple_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        float radius = (float)args[5]->toDouble();
        uint8_t r = (uint8_t)args[6]->toDouble();
        uint8_t g = (uint8_t)args[7]->toDouble();
        uint8_t b = (uint8_t)args[8]->toDouble();
        float intensity = (float)args[9]->toDouble();
        sad::effects::drawGlowSimple(renderer, x, y, w, h, radius, r, g, b, intensity);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_توهج", effects_drawGlowSimple_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_glow", effects_drawGlowSimple_func);

    // ─── ارسم_تدرج_افقي ───
    // رسم تدرج لوني أفقي (من اليسار إلى اليمين) داخل مستطيل.
    // التدرج ينتقل سلساً بين لونين. مثالي لخلفيات الأزرار والعناوين
    // والشرائط الزخرفية. المعاملات: المُصيِّر، الموضع والأبعاد،
    // اللون الأول (بداية التدرج)، اللون الثاني (نهاية التدرج)، والشفافية.
    auto effects_drawGradientH_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        uint8_t r1 = (uint8_t)args[5]->toDouble();
        uint8_t g1 = (uint8_t)args[6]->toDouble();
        uint8_t b1 = (uint8_t)args[7]->toDouble();
        uint8_t r2 = (uint8_t)args[8]->toDouble();
        uint8_t g2 = (uint8_t)args[9]->toDouble();
        uint8_t b2 = (uint8_t)args[10]->toDouble();
        float alpha = (float)args[11]->toDouble();
        sad::effects::drawGradientH(renderer, x, y, w, h, r1, g1, b1, r2, g2, b2, alpha);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_تدرج_افقي", effects_drawGradientH_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_gradient_h", effects_drawGradientH_func);

    // ─── ارسم_تدرج_عمودي ───
    // رسم تدرج لوني عمودي (من الأعلى إلى الأسفل) داخل مستطيل.
    // مشابه للتدرج الأفقي لكن الاتجاه رأسي. مثالي لخلفيات السماء
    // (أزرق فاتح في الأعلى إلى أزرق غامق في الأسفل) أو لأشرطة التقدم.
    auto effects_drawGradientV_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        uint8_t r1 = (uint8_t)args[5]->toDouble();
        uint8_t g1 = (uint8_t)args[6]->toDouble();
        uint8_t b1 = (uint8_t)args[7]->toDouble();
        uint8_t r2 = (uint8_t)args[8]->toDouble();
        uint8_t g2 = (uint8_t)args[9]->toDouble();
        uint8_t b2 = (uint8_t)args[10]->toDouble();
        float alpha = (float)args[11]->toDouble();
        sad::effects::drawGradientV(renderer, x, y, w, h, r1, g1, b1, r2, g2, b2, alpha);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_تدرج_عمودي", effects_drawGradientV_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_gradient_v", effects_drawGradientV_func);

    // ─── ارسم_تدرج_دائري ───
    // رسم تدرج لوني دائري (شعاعي) — اللون يتغير من المركز إلى الحافة.
    // مثالي لتأثيرات الإضاءة المركزية (بقعة ضوء) أو خلفيات دائرية جميلة.
    // المعاملات: المُصيِّر، مركز الدائرة (cx, cy)، نصف القطر، لونا البداية والنهاية، والشفافية.
    auto effects_drawGradientRadial_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int cx = (int)args[1]->toDouble();
        int cy = (int)args[2]->toDouble();
        float radius = (float)args[3]->toDouble();
        uint8_t r1 = (uint8_t)args[4]->toDouble();
        uint8_t g1 = (uint8_t)args[5]->toDouble();
        uint8_t b1 = (uint8_t)args[6]->toDouble();
        uint8_t r2 = (uint8_t)args[7]->toDouble();
        uint8_t g2 = (uint8_t)args[8]->toDouble();
        uint8_t b2 = (uint8_t)args[9]->toDouble();
        float alpha = (float)args[10]->toDouble();
        sad::effects::drawGradientRadial(renderer, cx, cy, radius, r1, g1, b1, r2, g2, b2, alpha);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_تدرج_دائري", effects_drawGradientRadial_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_gradient_radial", effects_drawGradientRadial_func);

    // ─── ارسم_زجاج ───
    // رسم تأثير الزجاج الشفاف (Glass Morphism) — لوحة زجاجية مع ضبابية
    // وشفافية وصبغة لونية. هذا التأثير شائع في تصاميم واجهات iOS الحديثة
    // ويعطي مظهراً أنيقاً وعصرياً. المعاملات: المُصيِّر، الموضع والأبعاد،
    // مقدار الضبابية، الشفافية، ولون الصبغة (أحمر، أخضر، أزرق).
    auto effects_drawGlassPanelSimple_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        float blur = (float)args[5]->toDouble();
        float opacity = (float)args[6]->toDouble();
        uint8_t tR = (uint8_t)args[7]->toDouble();
        uint8_t tG = (uint8_t)args[8]->toDouble();
        uint8_t tB = (uint8_t)args[9]->toDouble();
        sad::effects::drawGlassPanelSimple(renderer, x, y, w, h, blur, opacity, tR, tG, tB);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_زجاج", effects_drawGlassPanelSimple_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_glass", effects_drawGlassPanelSimple_func);

    // ─── ارسم_نيومورف ───
    // رسم تأثير النيومورفيزم (Neumorphism) — تصميم ثلاثي الأبعاد ناعم
    // يبدو كأن العنصر مطبوع في السطح نفسه. يتضمن ظلاً فاتحاً وظلاً داكناً
    // ليعطي إحساساً بالبروز أو الغؤور. المعاملات: المُصيِّر، الموضع والأبعاد،
    // عمق التأثير، لون الخلفية، وهل هو بارز (true) أم غائر (false).
    auto effects_drawNeumorphSimple_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        float depth = (float)args[5]->toDouble();
        uint8_t bgR = (uint8_t)args[6]->toDouble();
        uint8_t bgG = (uint8_t)args[7]->toDouble();
        uint8_t bgB = (uint8_t)args[8]->toDouble();
        bool convex = args[9]->toDouble() != 0.0;
        sad::effects::drawNeumorphSimple(renderer, x, y, w, h, depth, bgR, bgG, bgB, convex);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_نيومورف", effects_drawNeumorphSimple_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_neumorph", effects_drawNeumorphSimple_func);

    // ─── فلتر_بني ───
    // تطبيق فلتر اللون البني القديم (Sepia) على منطقة مستطيلة من الشاشة.
    // يعطي الصورة مظهراً كلاسيكياً قديماً كصور القرن التاسع عشر.
    // المعامل الأخير يتحكم بشدة التأثير (0.0 = بلا تأثير، 1.0 = كامل).
    auto effects_applySepia_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        float amount = args.size() > 5 ? (float)args[5]->toDouble() : 1.0f;
        sad::effects::applySepia(renderer, x, y, w, h, amount);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("فلتر_بني", effects_applySepia_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_sepia", effects_applySepia_func);

    // ─── فلتر_رمادي ───
    // تطبيق فلتر التدرج الرمادي (Grayscale) على منطقة مستطيلة.
    // يحوّل كل الألوان إلى درجات الرمادي. مفيد لتأثيرات الموت في الألعاب
    // أو لإبراز عنصر ملون واحد بجعل باقي الشاشة رمادية.
    // المعامل الأخير يتحكم بشدة التأثير (0.0 = ألوان كاملة، 1.0 = رمادي تماماً).
    auto effects_applyGrayscale_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        float amount = args.size() > 5 ? (float)args[5]->toDouble() : 1.0f;
        sad::effects::applyGrayscale(renderer, x, y, w, h, amount);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("فلتر_رمادي", effects_applyGrayscale_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_grayscale", effects_applyGrayscale_func);

    // ─── فلتر_سطوع ───
    // تعديل سطوع منطقة مستطيلة من الشاشة. المعامل factor يحدد
    // مستوى السطوع: 1.0 = بلا تغيير، أكبر من 1.0 = أكثر سطوعاً،
    // أقل من 1.0 = أكثر عتمةً. مثلاً: 1.5 = أكثر سطوعاً بـ 50%، 0.5 = نصف السطوع.
    // مفيد لتأثيرات الوميض (flash) أو التعتيم عند فتح القوائم.
    auto effects_applyBrightness_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        float factor = (float)args[5]->toDouble();
        sad::effects::applyBrightness(renderer, x, y, w, h, factor);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("فلتر_سطوع", effects_applyBrightness_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_brightness", effects_applyBrightness_func);

    // ─── ارسم_تموج ───
    // رسم تأثير تموج دائري (Ripple) — موجات دائرية تنتشر من نقطة مركزية.
    // مثالي لتأثيرات سقوط الحجر في الماء أو موجات الصوت أو الضغط على الشاشة.
    // المعاملات: المُصيِّر، مركز التموج (cx, cy)، نصف القطر، السعة (ارتفاع الموجة)،
    // الطور (لتحريك الموجة بمرور الوقت)، اللون، والشفافية.
    auto effects_drawRipple_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        float cx = (float)args[1]->toDouble();
        float cy = (float)args[2]->toDouble();
        float radius = (float)args[3]->toDouble();
        float amplitude = (float)args[4]->toDouble();
        float phase = (float)args[5]->toDouble();
        uint8_t r = (uint8_t)args[6]->toDouble();
        uint8_t g = (uint8_t)args[7]->toDouble();
        uint8_t b = (uint8_t)args[8]->toDouble();
        float alpha = (float)args[9]->toDouble();
        sad::effects::drawRipple(renderer, cx, cy, radius, amplitude, phase, r, g, b, alpha);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_تموج", effects_drawRipple_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_ripple", effects_drawRipple_func);

    // ─── ارسم_حافة ───
    // رسم تأثير الحافة المعتمة (Vignette) — تعتيم تدريجي عند حواف الشاشة.
    // يوجّه انتباه المشاهد إلى وسط الشاشة ويعطي مظهراً سينمائياً.
    // المعاملات: المُصيِّر، عرض وارتفاع الشاشة، نصف قطر المنطقة المضيئة،
    // نعومة الانتقال، وشدة التعتيم.
    auto effects_drawVignette_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int width = (int)args[1]->toDouble();
        int height = (int)args[2]->toDouble();
        float radius = (float)args[3]->toDouble();
        float softness = (float)args[4]->toDouble();
        float alpha = (float)args[5]->toDouble();
        sad::effects::drawVignette(renderer, width, height, radius, softness, alpha);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_حافة", effects_drawVignette_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_vignette", effects_drawVignette_func);

    // ─── ارسم_حد_متدرج ───
    // رسم حد (Border) بتدرج لوني حول مستطيل. الحد ينتقل سلساً بين لونين
    // مما يعطي مظهراً أنيقاً وعصرياً. مثالي لتزيين البطاقات والنوافذ والأزرار.
    // المعاملات: المُصيِّر، الموضع والأبعاد، سُمك الحد، لونا البداية والنهاية.
    auto effects_drawBorderGradient_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        float thickness = (float)args[5]->toDouble();
        uint8_t r1 = (uint8_t)args[6]->toDouble();
        uint8_t g1 = (uint8_t)args[7]->toDouble();
        uint8_t b1 = (uint8_t)args[8]->toDouble();
        uint8_t r2 = (uint8_t)args[9]->toDouble();
        uint8_t g2 = (uint8_t)args[10]->toDouble();
        uint8_t b2 = (uint8_t)args[11]->toDouble();
        sad::effects::drawBorderGradient(renderer, x, y, w, h, thickness, r1, g1, b1, r2, g2, b2);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_حد_متدرج", effects_drawBorderGradient_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_border_gradient", effects_drawBorderGradient_func);

    // ===================================================================
    // النظام 3: محرك الفيزياء — sad::physics
    // ===================================================================
    //
    // محرك فيزياء ثنائي الأبعاد متكامل يوفر محاكاة فيزيائية واقعية.
    // يدعم الأجسام الصلبة (دوائر ومستطيلات)، كشف التصادمات،
    // الجاذبية والاحتكاك والارتداد، القوى والاندفاعات، القيود (النوابض)،
    // والرسم التصحيحي. يمكن محاكاة حتى 500 جسم بأداء ممتاز.
    // Flutter ليس لديه أي محرك فيزياء مدمج!
    // ===================================================================

    // ─── أنشئ_دائرة_فيزيائية ───
    // إنشاء جسم فيزيائي دائري الشكل في الموضع (x, y) مع نصف القطر والكتلة المحددين.
    // الجسم الدائري مثالي لمحاكاة الكرات والعملات والفقاعات والقذائف.
    // يتأثر بالجاذبية والتصادمات تلقائياً. يُرجع معرّف الجسم للتحكم به.
    auto physics_createCircle_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float radius = (float)args[2]->toDouble();
        float mass = args.size() > 3 ? (float)args[3]->toDouble() : 1.0f;
        int id = sad::physics::createCircle(x, y, radius, mass);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_دائرة_فيزيائية", physics_createCircle_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_create_circle", physics_createCircle_func);

    // ─── أنشئ_صندوق_فيزيائي ───
    // إنشاء جسم فيزيائي مستطيل الشكل (صندوق) في الموضع (x, y) مع العرض والارتفاع والكتلة.
    // الصندوق مثالي لمحاكاة الصناديق والطوب والبلوكات والمنصات المتحركة.
    // يتأثر بالجاذبية والتصادمات ويتفاعل مع الأجسام الأخرى. يُرجع معرّف الجسم.
    auto physics_createBox_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float w = (float)args[2]->toDouble();
        float h = (float)args[3]->toDouble();
        float mass = args.size() > 4 ? (float)args[4]->toDouble() : 1.0f;
        int id = sad::physics::createBox(x, y, w, h, mass);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_صندوق_فيزيائي", physics_createBox_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_create_box", physics_createBox_func);

    // ─── أنشئ_منصة ───
    // إنشاء منصة ثابتة (Static Platform) لا تتحرك ولا تتأثر بالقوى.
    // المنصات الثابتة تُستخدم كأرضيات وجدران وحواجز وسلالم.
    // الأجسام المتحركة ترتد عنها لكنها هي لا تتزحزح أبداً.
    auto physics_createStaticPlatform_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float w = (float)args[2]->toDouble();
        float h = (float)args[3]->toDouble();
        int id = sad::physics::createStaticPlatform(x, y, w, h);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_منصة", physics_createStaticPlatform_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_create_platform", physics_createStaticPlatform_func);

    // ─── احذف_جسم ───
    // حذف جسم فيزيائي من العالم بمعرّفه. يختفي الجسم فوراً
    // ولا يعود يؤثر في التصادمات أو الفيزياء.
    auto physics_destroyBody_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        sad::physics::destroyBody(id);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_جسم", physics_destroyBody_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_destroy_body", physics_destroyBody_func);

    // ─── عيّن_موضع_فيزيائي ───
    // نقل جسم فيزيائي مباشرة إلى موضع جديد (x, y).
    // تحذير: هذا يتجاوز الفيزياء — الجسم ينتقل فوراً بدون تصادمات.
    // يُستخدم عادةً لإعادة وضع الجسم بعد السقوط أو عند بدء مرحلة جديدة.
    auto physics_setPosition_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float x = (float)args[1]->toDouble();
        float y = (float)args[2]->toDouble();
        sad::physics::setPosition(id, x, y);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_موضع_فيزيائي", physics_setPosition_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_set_position", physics_setPosition_func);

    // ─── عيّن_سرعة ───
    // تعيين سرعة جسم فيزيائي مباشرة (vx, vy بالبكسل/ثانية).
    // مفيد لإطلاق قذيفة أو قفز شخصية أو تحريك جسم بسرعة محددة.
    // القيمة الموجبة في vy تعني الحركة للأسفل والسالبة للأعلى.
    auto physics_setVelocity_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float vx = (float)args[1]->toDouble();
        float vy = (float)args[2]->toDouble();
        sad::physics::setVelocity(id, vx, vy);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_سرعة", physics_setVelocity_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_set_velocity", physics_setVelocity_func);

    // ─── عيّن_ارتداد ───
    // تعيين معامل الارتداد (Restitution) لجسم فيزيائي.
    // القيمة 0 = لا ارتداد (الجسم يتوقف عند الاصطدام مثل الطين).
    // القيمة 1 = ارتداد مرن كامل (مثل كرة مطاطية).
    // القيم بين 0 و1 تعطي ارتداداً جزئياً (مثل كرة القدم ≈ 0.6).
    auto physics_setRestitution_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float rest = (float)args[1]->toDouble();
        sad::physics::setRestitution(id, rest);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_ارتداد", physics_setRestitution_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_set_restitution", physics_setRestitution_func);

    // ─── عيّن_احتكاك ───
    // تعيين معامل الاحتكاك (Friction) لجسم فيزيائي.
    // القيمة 0 = سطح زلق (مثل الجليد — لا يبطئ الحركة).
    // القيمة 1 = سطح خشن جداً (يوقف الحركة بسرعة).
    // مفيد لمحاكاة أنواع مختلفة من الأسطح في الألعاب.
    auto physics_setFriction_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float fric = (float)args[1]->toDouble();
        sad::physics::setFriction(id, fric);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_احتكاك", physics_setFriction_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_set_friction", physics_setFriction_func);

    // ─── عيّن_لون_جسم ───
    // تعيين لون عرض جسم فيزيائي عند رسمه بالدالة التصحيحية.
    // مفيد للتمييز بين أنواع الأجسام (مثلاً: أعداء بالأحمر، لاعب بالأزرق).
    auto physics_setBodyColor_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        uint8_t r = (uint8_t)args[1]->toDouble();
        uint8_t g = (uint8_t)args[2]->toDouble();
        uint8_t b = (uint8_t)args[3]->toDouble();
        sad::physics::setBodyColor(id, r, g, b);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_لون_جسم", physics_setBodyColor_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_set_body_color", physics_setBodyColor_func);

    // ─── موضع_س_فيزيائي ───
    // الحصول على الإحداثي الأفقي (X) لجسم فيزيائي.
    // يُستخدم لمعرفة أين يقع الجسم حالياً في العالم الفيزيائي
    // لرسمه في المكان الصحيح أو لاتخاذ قرارات بناءً على موضعه.
    auto physics_getX_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float x = sad::physics::getX(id);
        return std::make_shared<Data::Value>((double)x);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موضع_س_فيزيائي", physics_getX_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_get_x", physics_getX_func);

    // ─── موضع_ص_فيزيائي ───
    // الحصول على الإحداثي الرأسي (Y) لجسم فيزيائي.
    // مع موضع_س_فيزيائي يمكنك معرفة الموضع الكامل للجسم في العالم.
    // لاحظ أن المحور Y موجب للأسفل (كما هو معتاد في الرسوميات ثنائية الأبعاد).
    auto physics_getY_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float y = sad::physics::getY(id);
        return std::make_shared<Data::Value>((double)y);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موضع_ص_فيزيائي", physics_getY_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_get_y", physics_getY_func);

    // ─── طبّق_قوة ───
    // تطبيق قوة مستمرة على جسم فيزيائي (بالنيوتن تقريباً).
    // القوة تُضاف إلى القوى الأخرى وتُطبَّق في الإطار التالي.
    // مثل الرياح أو محرك صاروخ — قوة مستمرة ما دام يتم استدعاؤها.
    // لقفزة أو ضربة مفاجئة استخدم طبّق_اندفاع بدلاً من هذه الدالة.
    auto physics_applyForce_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float fx = (float)args[1]->toDouble();
        float fy = (float)args[2]->toDouble();
        sad::physics::applyForce(id, fx, fy);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("طبّق_قوة", physics_applyForce_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_apply_force", physics_applyForce_func);

    // ─── طبّق_اندفاع ───
    // تطبيق اندفاع فوري (Impulse) على جسم فيزيائي — تغيير مباشر وفوري في السرعة.
    // على عكس القوة المستمرة، الاندفاع يحدث مرة واحدة فقط.
    // مثالي للقفز والرمي والضربات والانفجارات والنطّات.
    // القيمة السالبة في iy تعني اندفاع للأعلى (قفزة).
    auto physics_applyImpulse_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float ix = (float)args[1]->toDouble();
        float iy = (float)args[2]->toDouble();
        sad::physics::applyImpulse(id, ix, iy);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("طبّق_اندفاع", physics_applyImpulse_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_apply_impulse", physics_applyImpulse_func);

    // ─── عيّن_جاذبية_العالم ───
    // تعيين الجاذبية العامة لعالم الفيزياء بأكمله.
    // الجاذبية تؤثر على جميع الأجسام المتحركة (Dynamic).
    // القيمة الافتراضية عادةً (0, 980) لمحاكاة جاذبية الأرض (9.8 م/ث² × 100 بكسل/متر).
    // يمكن تغييرها لمحاكاة القمر (0, 160) أو الفضاء (0, 0) أو حتى عكسها (0, -500).
    auto physics_setWorldGravity_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float gx = (float)args[0]->toDouble();
        float gy = (float)args[1]->toDouble();
        sad::physics::setWorldGravity(gx, gy);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_جاذبية_العالم", physics_setWorldGravity_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_set_world_gravity", physics_setWorldGravity_func);

    // ─── عيّن_حدود_العالم ───
    // تعيين حدود العالم الفيزيائي (المنطقة التي تحتوي الأجسام).
    // الأجسام التي تخرج من هذه الحدود تُعاد إلى الداخل (ترتد عن الجدران).
    // المعاملات: الحد الأيسر، العلوي، الأيمن، السفلي.
    // مفيد لمنع الأجسام من السقوط إلى ما لا نهاية.
    auto physics_setWorldBounds_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float left = (float)args[0]->toDouble();
        float top = (float)args[1]->toDouble();
        float right = (float)args[2]->toDouble();
        float bottom = (float)args[3]->toDouble();
        sad::physics::setWorldBounds(left, top, right, bottom);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_حدود_العالم", physics_setWorldBounds_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_set_world_bounds", physics_setWorldBounds_func);

    // ─── أنشئ_نابض ───
    // إنشاء قيد نابض (Spring Constraint) بين جسمين فيزيائيين.
    // النابض يحاول إبقاء المسافة بين الجسمين عند الطول المحدد —
    // إذا ابتعدا يسحبهما وإذا اقتربا يبعدهما. مثالي لمحاكاة الحبال
    // والجسور المعلقة والسلاسل وأجهزة الزنبرك.
    // المعاملات: معرّفا الجسمين، طول النابض، الصلابة (0-1)، التخميد (0-1).
    auto physics_createSpringConstraint_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int bodyA = (int)args[0]->toDouble();
        int bodyB = (int)args[1]->toDouble();
        float length = (float)args[2]->toDouble();
        float stiffness = args.size() > 3 ? (float)args[3]->toDouble() : 0.3f;
        float damping = args.size() > 4 ? (float)args[4]->toDouble() : 0.1f;
        int id = sad::physics::createSpringConstraint(bodyA, bodyB, length, stiffness, damping);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_نابض", physics_createSpringConstraint_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_create_spring", physics_createSpringConstraint_func);

    // ─── هل_يتصادم ───
    // فحص هل جسمان فيزيائيان يتصادمان (يتلامسان) حالياً.
    // يُرجع 1 (صحيح) إذا كان هناك تصادم، و0 (خطأ) إذا لم يكن.
    // مفيد لكشف وصول الكرة للهدف، أو ملامسة اللاعب لعدو، أو جمع عنصر.
    auto physics_isColliding_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int idA = (int)args[0]->toDouble();
        int idB = (int)args[1]->toDouble();
        bool colliding = sad::physics::isColliding(idA, idB);
        return std::make_shared<Data::Value>(colliding ? 1.0 : 0.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هل_يتصادم", physics_isColliding_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_is_colliding", physics_isColliding_func);

    // ─── عدد_الأجسام ───
    // الحصول على العدد الإجمالي للأجسام الفيزيائية الموجودة في العالم.
    // مفيد لمراقبة الأداء أو لعرض إحصائيات أو للتحقق من فراغ العالم.
    auto physics_getBodyCount_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int count = sad::physics::getBodyCount();
        return std::make_shared<Data::Value>((double)count);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عدد_الأجسام", physics_getBodyCount_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_get_body_count", physics_getBodyCount_func);

    // ─── خطوة_فيزيائية ───
    // تقديم المحاكاة الفيزيائية خطوة واحدة للأمام بالزمن المحدد (بالملي ثانية).
    // يجب استدعاء هذه الدالة في كل إطار من حلقة اللعبة الرئيسية.
    // هي التي تحرّك الأجسام وتكشف التصادمات وتطبّق الجاذبية والقوى.
    // مثال: خطوة_فيزيائية(16.67) لـ 60 إطار في الثانية.
    auto physics_step_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float dt = (float)args[0]->toDouble();
        sad::physics::step(dt);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("خطوة_فيزيائية", physics_step_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_step", physics_step_func);

    // ─── ارسم_فيزياء ───
    // رسم جميع الأجسام الفيزيائية بأشكالها الهندسية وألوانها على المُصيِّر.
    // هذه الدالة تصحيحية (Debug) — ترسم الدوائر والمستطيلات والقيود.
    // مفيدة أثناء التطوير لرؤية مواضع الأجسام وحدود التصادم.
    // المعامل: مؤشر المُصيِّر (renderer) كقيمة رقمية.
    auto physics_debugRender_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        sad::physics::debugRender(renderer);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_فيزياء", physics_debugRender_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_debug_render", physics_debugRender_func);

    // ─── أعد_تعيين_العالم ───
    // إعادة تعيين عالم الفيزياء بالكامل — حذف كل الأجسام والقيود
    // وإعادة كل الإعدادات إلى قيمها الافتراضية. يُستخدم عند بدء
    // مرحلة جديدة أو إعادة تشغيل اللعبة أو تحميل مشهد جديد.
    auto physics_resetWorld_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        sad::physics::resetWorld();
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أعد_تعيين_العالم", physics_resetWorld_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_reset_world", physics_resetWorld_func);

    // ===================================================================
    // النظام 4: المشاهد ثلاثية الأبعاد — sad::scene3d
    // ===================================================================
    //
    // نظام رسم ثلاثي الأبعاد كامل فوق SDL2 — Flutter لا يدعم 3D إطلاقاً!
    // يتضمن إسقاطاً منظورياً، إضاءة اتجاهية ومحيطية، أشكالاً أساسية
    // (مكعب، كرة، مخروط، أسطوانة، مستوى)، كاميرا قابلة للتحريك والتدوير،
    // ورسم شبكي ومحاور إحداثية. يمكن عرض حتى 1000 كائن ثلاثي الأبعاد.
    // ===================================================================

    // ─── أنشئ_مكعب ───
    // إنشاء مكعب ثلاثي الأبعاد في الموضع (x, y, z) بالحجم المحدد.
    // المكعب هو أبسط الأشكال ثلاثية الأبعاد — 6 وجوه متساوية.
    // مثالي لتمثيل المباني والصناديق والأرضيات والجدران في مشاهد 3D.
    // المعامل الرابع (الحجم) اختياري وقيمته الافتراضية 1.0.
    auto scene3d_createCube_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float z = (float)args[2]->toDouble();
        float size = args.size() > 3 ? (float)args[3]->toDouble() : 1.0f;
        int id = sad::scene3d::createCube(x, y, z, size);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_مكعب", scene3d_createCube_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_create_cube", scene3d_createCube_func);

    // ─── أنشئ_كرة ───
    // إنشاء كرة ثلاثية الأبعاد في الموضع (x, y, z) بنصف القطر وعدد الأقسام.
    // الكرة مكونة من مثلثات — كلما زاد عدد الأقسام (segments) كلما بدت أكثر نعومة
    // لكن تكلفة الرسم تزيد. القيمة الافتراضية 12 قسماً تعطي نتيجة جيدة.
    // مثالية لتمثيل الكواكب والكرات والجزيئات والنقاط المضيئة.
    auto scene3d_createSphere_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float z = (float)args[2]->toDouble();
        float radius = args.size() > 3 ? (float)args[3]->toDouble() : 0.5f;
        int segments = args.size() > 4 ? (int)args[4]->toDouble() : 12;
        int id = sad::scene3d::createSphere(x, y, z, radius, segments);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_كرة", scene3d_createSphere_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_create_sphere", scene3d_createSphere_func);

    // ─── أنشئ_مخروط ───
    // إنشاء مخروط ثلاثي الأبعاد في الموضع (x, y, z) بنصف القطر والارتفاع.
    // المخروط شكل هندسي قاعدته دائرة ورأسه نقطة. مثالي لتمثيل
    // الأشجار (أقماع خضراء) والأسهم والمؤشرات والقمع والمباني المخروطية.
    auto scene3d_createCone_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float z = (float)args[2]->toDouble();
        float radius = args.size() > 3 ? (float)args[3]->toDouble() : 0.5f;
        float height = args.size() > 4 ? (float)args[4]->toDouble() : 1.0f;
        int id = sad::scene3d::createCone(x, y, z, radius, height);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_مخروط", scene3d_createCone_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_create_cone", scene3d_createCone_func);

    // ─── أنشئ_أسطوانة ───
    // إنشاء أسطوانة ثلاثية الأبعاد في الموضع (x, y, z) بنصف القطر والارتفاع.
    // الأسطوانة شكل ذو قاعدتين دائريتين وجسم مستقيم. مثالية لتمثيل
    // الأعمدة والأنابيب وجذوع الأشجار والعملات المعدنية (مع ارتفاع صغير).
    auto scene3d_createCylinder_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float z = (float)args[2]->toDouble();
        float radius = args.size() > 3 ? (float)args[3]->toDouble() : 0.5f;
        float height = args.size() > 4 ? (float)args[4]->toDouble() : 1.0f;
        int id = sad::scene3d::createCylinder(x, y, z, radius, height);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_أسطوانة", scene3d_createCylinder_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_create_cylinder", scene3d_createCylinder_func);

    // ─── أنشئ_مستوى ───
    // إنشاء مستوى (Plane) ثلاثي الأبعاد — سطح مسطح أفقي.
    // يُستخدم كأرضية للمشهد أو كسطح ماء أو كقاعدة للكائنات.
    // المعامل الرابع يحدد حجم المستوى (الافتراضي 5.0 وحدات).
    auto scene3d_createPlane_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float z = (float)args[2]->toDouble();
        float size = args.size() > 3 ? (float)args[3]->toDouble() : 5.0f;
        int id = sad::scene3d::createPlane(x, y, z, size);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_مستوى", scene3d_createPlane_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_create_plane", scene3d_createPlane_func);

    // ─── احذف_كائن_3D ───
    // حذف كائن ثلاثي الأبعاد من المشهد بمعرّفه.
    // الكائن يختفي فوراً ولا يُرسم بعد ذلك.
    auto scene3d_destroyObject_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        sad::scene3d::destroyObject(id);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_كائن_3D", scene3d_destroyObject_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_destroy_object", scene3d_destroyObject_func);

    // ─── عيّن_موضع_3D ───
    // تعيين موضع كائن ثلاثي الأبعاد مباشرة إلى الإحداثيات (x, y, z).
    // المحور X أفقي (يمين/يسار)، المحور Y رأسي (أعلى/أسفل)،
    // المحور Z عمق (بعيد/قريب من الكاميرا). يُستخدم لتحريك الكائنات في المشهد.
    auto scene3d_setObjectPosition_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float x = (float)args[1]->toDouble();
        float y = (float)args[2]->toDouble();
        float z = (float)args[3]->toDouble();
        sad::scene3d::setObjectPosition(id, x, y, z);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_موضع_3D", scene3d_setObjectPosition_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_set_position", scene3d_setObjectPosition_func);

    // ─── عيّن_دوران_3D ───
    // تعيين دوران كائن ثلاثي الأبعاد حول المحاور الثلاثة (بالدرجات).
    // rx = الدوران حول المحور X (ميلان أمامي/خلفي).
    // ry = الدوران حول المحور Y (دوران يمين/يسار — الأكثر شيوعاً).
    // rz = الدوران حول المحور Z (إمالة جانبية).
    auto scene3d_setObjectRotation_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float rx = (float)args[1]->toDouble();
        float ry = (float)args[2]->toDouble();
        float rz = (float)args[3]->toDouble();
        sad::scene3d::setObjectRotation(id, rx, ry, rz);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_دوران_3D", scene3d_setObjectRotation_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_set_rotation", scene3d_setObjectRotation_func);

    // ─── عيّن_مقياس_3D ───
    // تعيين مقياس (حجم) كائن ثلاثي الأبعاد على المحاور الثلاثة.
    // (1, 1, 1) = الحجم الطبيعي. (2, 2, 2) = ضعف الحجم في كل الاتجاهات.
    // يمكن مط الكائن بتغيير محور واحد فقط مثل (1, 2, 1) لمضاعفة الارتفاع.
    auto scene3d_setObjectScale_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float sx = (float)args[1]->toDouble();
        float sy = (float)args[2]->toDouble();
        float sz = (float)args[3]->toDouble();
        sad::scene3d::setObjectScale(id, sx, sy, sz);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_مقياس_3D", scene3d_setObjectScale_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_set_scale", scene3d_setObjectScale_func);

    // ─── عيّن_لون_3D ───
    // تعيين لون كائن ثلاثي الأبعاد (RGBA). اللون يؤثر على كيفية ظهور
    // الكائن عند تطبيق الإضاءة عليه. المعامل الخامس (الشفافية) اختياري
    // وقيمته الافتراضية 255 (معتم بالكامل). قيم أقل تجعل الكائن شبه شفاف.
    auto scene3d_setObjectColor_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        uint8_t r = (uint8_t)args[1]->toDouble();
        uint8_t g = (uint8_t)args[2]->toDouble();
        uint8_t b = (uint8_t)args[3]->toDouble();
        uint8_t a = args.size() > 4 ? (uint8_t)args[4]->toDouble() : 255;
        sad::scene3d::setObjectColor(id, r, g, b, a);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_لون_3D", scene3d_setObjectColor_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_set_color", scene3d_setObjectColor_func);

    // ─── دوّر_كائن ───
    // إضافة دوران تدريجي لكائن ثلاثي الأبعاد (بالدرجات).
    // على عكس عيّن_دوران_3D الذي يحدد الدوران المطلق، هذه الدالة
    // تضيف الدوران المحدد للدوران الحالي. مثالية لتدوير الكائنات باستمرار
    // في كل إطار (مثل كوكب يدور حول نفسه أو عملة تتقلب).
    auto scene3d_rotateObject_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float dx = (float)args[1]->toDouble();
        float dy = (float)args[2]->toDouble();
        float dz = (float)args[3]->toDouble();
        sad::scene3d::rotateObject(id, dx, dy, dz);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("دوّر_كائن", scene3d_rotateObject_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_rotate_object", scene3d_rotateObject_func);

    // ─── عيّن_كاميرا ───
    // تعيين موضع الكاميرا في الفضاء ثلاثي الأبعاد.
    // الكاميرا هي "العين" التي ننظر من خلالها للمشهد.
    // الموضع الافتراضي (0, 2, -5) — مرتفعة قليلاً ومبتعدة عن المركز.
    // غيّر z لتقريب/إبعاد الكاميرا، وy لرفعها/خفضها.
    auto scene3d_setCameraPosition_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float z = (float)args[2]->toDouble();
        sad::scene3d::setCameraPosition(x, y, z);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_كاميرا", scene3d_setCameraPosition_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_set_camera_position", scene3d_setCameraPosition_func);

    // ─── عيّن_هدف_كاميرا ───
    // تعيين النقطة التي تنظر إليها الكاميرا في الفضاء ثلاثي الأبعاد.
    // الكاميرا دائماً موجهة نحو هذه النقطة. الهدف الافتراضي (0, 0, 0) — المركز.
    // غيّر الهدف لتوجيه الكاميرا نحو كائن معين أو منطقة محددة.
    auto scene3d_setCameraTarget_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float z = (float)args[2]->toDouble();
        sad::scene3d::setCameraTarget(x, y, z);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_هدف_كاميرا", scene3d_setCameraTarget_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_set_camera_target", scene3d_setCameraTarget_func);

    // ─── دوّر_كاميرا ───
    // تدوير الكاميرا حول هدفها (Orbit) — كأنك تدور حول كائن لرؤيته من كل الزوايا.
    // المعامل الأول: الزاوية الأفقية (يمين/يسار). الثاني: الزاوية الرأسية (أعلى/أسفل).
    // الثالث: المسافة من الهدف (القرب/البعد). مثالي للمعاينة ثلاثية الأبعاد.
    auto scene3d_orbitCamera_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float angleH = (float)args[0]->toDouble();
        float angleV = (float)args[1]->toDouble();
        float distance = (float)args[2]->toDouble();
        sad::scene3d::orbitCamera(angleH, angleV, distance);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("دوّر_كاميرا", scene3d_orbitCamera_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_orbit_camera", scene3d_orbitCamera_func);

    // ─── عيّن_إضاءة ───
    // تعيين اتجاه الضوء الرئيسي (الاتجاهي) في المشهد ثلاثي الأبعاد.
    // الضوء الاتجاهي يشبه ضوء الشمس — يأتي من اتجاه واحد ويؤثر على
    // كل الكائنات بنفس الطريقة. الاتجاه الافتراضي (-0.5, -1, 0.5)
    // يمثل ضوءاً يأتي من الأعلى اليسار. غيّره لتغيير مظهر الإضاءة.
    auto scene3d_setLightDirection_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float z = (float)args[2]->toDouble();
        sad::scene3d::setLightDirection(x, y, z);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_إضاءة", scene3d_setLightDirection_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_set_light_direction", scene3d_setLightDirection_func);

    // ─── عيّن_إضاءة_محيطية ───
    // تعيين مستوى الإضاءة المحيطية (Ambient Light) في المشهد.
    // الإضاءة المحيطية هي الضوء الذي يصل لكل مكان بالتساوي بدون اتجاه محدد.
    // القيمة 0 = ظلام تام (فقط الضوء الاتجاهي يعمل).
    // القيمة 1 = إضاءة كاملة (لا ظلال — كل الوجوه مضاءة بالتساوي).
    // القيمة الافتراضية 0.3 تعطي توازناً جيداً بين الإضاءة والظلال.
    auto scene3d_setAmbientLight_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float amount = (float)args[0]->toDouble();
        sad::scene3d::setAmbientLight(amount);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_إضاءة_محيطية", scene3d_setAmbientLight_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_set_ambient_light", scene3d_setAmbientLight_func);

    // ─── ارسم_3D ───
    // رسم المشهد ثلاثي الأبعاد بالكامل على المُصيِّر (renderer).
    // هذه هي الدالة الرئيسية التي تُستدعى في كل إطار لعرض المشهد.
    // تقوم بتطبيق الإسقاط المنظوري والإضاءة وترتيب العمق ورسم كل الكائنات.
    // المعاملات: المُصيِّر، عرض الشاشة، ارتفاع الشاشة.
    auto scene3d_render_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int w = (int)args[1]->toDouble();
        int h = (int)args[2]->toDouble();
        sad::scene3d::render(renderer, w, h);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_3D", scene3d_render_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_render", scene3d_render_func);

    // ─── ارسم_شبكة ───
    // رسم شبكة أرضية (Grid) في المشهد ثلاثي الأبعاد — خطوط متقاطعة على
    // المستوى الأفقي تُساعد في تقدير المسافات والمواضع. مفيدة جداً أثناء
    // التطوير لرؤية "أرضية" المشهد. المعاملات: المُصيِّر، أبعاد الشاشة،
    // حجم الشبكة الكلي، وعدد التقسيمات (كلما زاد كلما كانت الشبكة أدق).
    auto scene3d_drawGrid_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int w = (int)args[1]->toDouble();
        int h = (int)args[2]->toDouble();
        float size = (float)args[3]->toDouble();
        int divs = (int)args[4]->toDouble();
        sad::scene3d::drawGrid(renderer, w, h, size, divs);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_شبكة", scene3d_drawGrid_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_draw_grid", scene3d_drawGrid_func);

    // ─── ارسم_محاور ───
    // رسم المحاور الإحداثية الثلاثة (X أحمر، Y أخضر، Z أزرق) في المشهد.
    // تُساعد في فهم اتجاهات المحاور والتنقل في الفضاء ثلاثي الأبعاد.
    // المعامل الرابع يحدد طول المحاور (بالوحدات ثلاثية الأبعاد).
    auto scene3d_drawAxes_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int w = (int)args[1]->toDouble();
        int h = (int)args[2]->toDouble();
        float length = (float)args[3]->toDouble();
        sad::scene3d::drawAxes(renderer, w, h, length);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_محاور", scene3d_drawAxes_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_draw_axes", scene3d_drawAxes_func);

    // ─── أعد_تعيين_المشهد ───
    // إعادة تعيين المشهد ثلاثي الأبعاد بالكامل — حذف كل الكائنات وإعادة
    // الكاميرا والإضاءة إلى قيمها الافتراضية. يُستخدم عند الانتقال
    // لمشهد جديد أو إعادة بناء المشهد من الصفر.
    auto scene3d_resetScene_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        sad::scene3d::resetScene();
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أعد_تعيين_المشهد", scene3d_resetScene_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_reset_scene", scene3d_resetScene_func);

    // ═══════════════════════════════════════════════════════════════════════════
    // نظام الذكاء الاصطناعي للألعاب (Game AI System) — sad::ai
    // ═══════════════════════════════════════════════════════════════════════════
    // هذا النظام يقدم أدوات ذكاء اصطناعي متكاملة للألعاب تشمل:
    //   - إيجاد المسار (A* Pathfinding): إنشاء شبكة وتحديد المسار الأقصر
    //   - آلة الحالات المحدودة (FSM): إدارة حالات الكيانات والانتقالات بينها
    //   - سلوكيات التوجيه (Steering): مطاردة، هروب، تجوال، اقتراب
    //   - سلوك الأسراب (Flocking): محاكاة حركة مجموعات مثل الطيور والأسماك
    // كل هذه الميزات مدمجة بدون أي مكتبة خارجية!
    // ═══════════════════════════════════════════════════════════════════════════

    // ─── أنشئ_شبكة_مسار / ai_create_grid ───
    // إنشاء شبكة ثنائية الأبعاد لنظام إيجاد المسار (A* Pathfinding).
    // الشبكة هي أساس كل عمليات إيجاد المسار — كل خلية يمكن أن تكون
    // قابلة للمشي أو حاجزاً. المعاملات: العرض والارتفاع (عدد الخلايا).
    // تُرجع معرّف الشبكة لاستخدامه في الدوال الأخرى.
    auto ai_createGrid_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int w = (int)args[0]->toDouble();
        int h = (int)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::createGrid(w, h));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_شبكة_مسار", ai_createGrid_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_create_grid", ai_createGrid_func);

    // ─── احذف_شبكة_مسار / ai_destroy_grid ───
    // حذف شبكة المسار وتحرير ذاكرتها. يجب استدعاء هذه الدالة عند الانتهاء
    // من استخدام الشبكة لتجنب تسرب الذاكرة. المعامل: معرّف الشبكة.
    auto ai_destroyGrid_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int gridId = (int)args[0]->toDouble();
        sad::ai::destroyGrid(gridId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_شبكة_مسار", ai_destroyGrid_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_destroy_grid", ai_destroyGrid_func);

    // ─── عيّن_قابلية_المشي / ai_set_walkable ───
    // تعيين خلية معينة في الشبكة كقابلة للمشي أو كحاجز (جدار).
    // هذا يحدد أين يمكن للكيانات أن تمر وأين لا يمكنها.
    // المعاملات: معرّف الشبكة، إحداثيات الخلية (س، ص)، وقيمة منطقية (1=ممر، 0=حاجز).
    auto ai_setWalkable_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int gridId = (int)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        bool walkable = args[3]->toDouble() != 0.0;
        sad::ai::setWalkable(gridId, x, y, walkable);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_قابلية_المشي", ai_setWalkable_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_set_walkable", ai_setWalkable_func);

    // ─── عيّن_تكلفة / ai_set_cost ───
    // تعيين تكلفة المرور عبر خلية معينة. الخلايا ذات التكلفة الأعلى
    // يتم تجنبها في المسار — مثلاً: الرمال أبطأ من الطريق المعبد.
    // المعاملات: معرّف الشبكة، إحداثيات الخلية (س، ص)، والتكلفة (عدد عشري).
    auto ai_setCost_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int gridId = (int)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        float cost = (float)args[3]->toDouble();
        sad::ai::setCost(gridId, x, y, cost);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_تكلفة", ai_setCost_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_set_cost", ai_setCost_func);

    // ─── جد_مسار / ai_find_path ───
    // إيجاد أقصر مسار بين نقطتين على الشبكة باستخدام خوارزمية A*.
    // هذه هي الدالة الأساسية في نظام إيجاد المسار — تأخذ نقطة البداية
    // ونقطة النهاية وتُرجع معرّف المسار الذي يمكن قراءة نقاطه لاحقاً.
    // المعاملات: معرّف الشبكة، بداية (س، ص)، نهاية (س، ص).
    auto ai_findPath_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int gridId = (int)args[0]->toDouble();
        int sx = (int)args[1]->toDouble();
        int sy = (int)args[2]->toDouble();
        int ex = (int)args[3]->toDouble();
        int ey = (int)args[4]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::findPath(gridId, sx, sy, ex, ey));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("جد_مسار", ai_findPath_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_find_path", ai_findPath_func);

    // ─── طول_المسار / ai_get_path_length ───
    // الحصول على عدد النقاط في المسار المحسوب. يُستخدم للتكرار على
    // نقاط المسار واحدة تلو الأخرى (مع مسار_س ومسار_ص).
    // المعامل: معرّف المسار.
    auto ai_getPathLength_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int pathId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::getPathLength(pathId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("طول_المسار", ai_getPathLength_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_path_length", ai_getPathLength_func);

    // ─── مسار_س / ai_get_path_x ───
    // الحصول على الإحداثي الأفقي (س) لنقطة معينة في المسار.
    // المعاملات: معرّف المسار، ورقم النقطة (الفهرس).
    auto ai_getPathX_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int pathId = (int)args[0]->toDouble();
        int index = (int)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::getPathX(pathId, index));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مسار_س", ai_getPathX_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_path_x", ai_getPathX_func);

    // ─── مسار_ص / ai_get_path_y ───
    // الحصول على الإحداثي العمودي (ص) لنقطة معينة في المسار.
    // المعاملات: معرّف المسار، ورقم النقطة (الفهرس).
    auto ai_getPathY_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int pathId = (int)args[0]->toDouble();
        int index = (int)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::getPathY(pathId, index));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مسار_ص", ai_getPathY_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_path_y", ai_getPathY_func);

    // ─── هل_وجد_مسار / ai_is_path_found ───
    // التحقق من نجاح عملية إيجاد المسار. تُرجع 1 إذا تم إيجاد مسار
    // و0 إذا لم يكن هناك مسار ممكن (مثلاً: الهدف محاط بحواجز).
    // المعامل: معرّف المسار.
    auto ai_isPathFound_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int pathId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>(sad::ai::isPathFound(pathId) ? 1.0 : 0.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هل_وجد_مسار", ai_isPathFound_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_is_path_found", ai_isPathFound_func);

    // ─── احذف_مسار / ai_destroy_path ───
    // حذف المسار المحسوب وتحرير ذاكرته. يجب استدعاء هذه الدالة بعد
    // الانتهاء من استخدام المسار لتجنب تسرب الذاكرة.
    // المعامل: معرّف المسار.
    auto ai_destroyPath_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int pathId = (int)args[0]->toDouble();
        sad::ai::destroyPath(pathId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_مسار", ai_destroyPath_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_destroy_path", ai_destroyPath_func);

    // ─────────────────────────────────────────────────────────────────────
    // آلة الحالات المحدودة (Finite State Machine - FSM)
    // ─────────────────────────────────────────────────────────────────────
    // آلة الحالات تُستخدم لإدارة سلوك الكيانات في اللعبة. كل كيان
    // (عدو، NPC، إلخ) يكون في "حالة" واحدة في كل لحظة (مثل: خمول،
    // مطاردة، هجوم، هروب). الانتقال بين الحالات يحدث من خلال "محفزات".
    // ─────────────────────────────────────────────────────────────────────

    // ─── أنشئ_آلة_حالات / ai_create_fsm ───
    // إنشاء آلة حالات محدودة جديدة. تُرجع معرّف الآلة لاستخدامه
    // في إضافة الحالات والانتقالات وإطلاق المحفزات.
    auto ai_createFSM_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return std::make_shared<Data::Value>((double)sad::ai::createFSM());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_آلة_حالات", ai_createFSM_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_create_fsm", ai_createFSM_func);

    // ─── احذف_آلة_حالات / ai_destroy_fsm ───
    // حذف آلة الحالات وتحرير ذاكرتها. المعامل: معرّف الآلة.
    auto ai_destroyFSM_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int fsmId = (int)args[0]->toDouble();
        sad::ai::destroyFSM(fsmId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_آلة_حالات", ai_destroyFSM_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_destroy_fsm", ai_destroyFSM_func);

    // ─── أضف_حالة / ai_add_state ───
    // إضافة حالة جديدة إلى آلة الحالات. كل حالة لها معرّف رقمي
    // واسم نصي (مثلاً: "خمول"، "مطاردة"، "هجوم").
    // المعاملات: معرّف الآلة، معرّف الحالة (رقم)، اسم الحالة (نص).
    auto ai_addState_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int fsmId = (int)args[0]->toDouble();
        int stateId = (int)args[1]->toDouble();
        std::string name = args[2]->toString();
        sad::ai::addState(fsmId, stateId, name);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أضف_حالة", ai_addState_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_add_state", ai_addState_func);

    // ─── عيّن_حالة_أولية / ai_set_initial_state ───
    // تعيين الحالة التي تبدأ منها آلة الحالات. يجب استدعاء هذه الدالة
    // بعد إضافة الحالات وقبل البدء في إطلاق المحفزات.
    // المعاملات: معرّف الآلة، معرّف الحالة الأولية.
    auto ai_setInitialState_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int fsmId = (int)args[0]->toDouble();
        int stateId = (int)args[1]->toDouble();
        sad::ai::setInitialState(fsmId, stateId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_حالة_أولية", ai_setInitialState_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_set_initial_state", ai_setInitialState_func);

    // ─── أضف_انتقال / ai_add_transition ───
    // إضافة انتقال بين حالتين. عند إطلاق المحفز المحدد، إذا كانت الآلة
    // في الحالة "من"، تنتقل تلقائياً إلى الحالة "إلى".
    // المعاملات: معرّف الآلة، حالة المصدر، حالة الوجهة، اسم المحفز.
    auto ai_addTransition_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int fsmId = (int)args[0]->toDouble();
        int from = (int)args[1]->toDouble();
        int to = (int)args[2]->toDouble();
        std::string trigger = args[3]->toString();
        sad::ai::addTransition(fsmId, from, to, trigger);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أضف_انتقال", ai_addTransition_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_add_transition", ai_addTransition_func);

    // ─── أطلق_محفز / ai_fire_trigger ───
    // إطلاق محفز (trigger) في آلة الحالات. إذا كان هناك انتقال معرّف
    // من الحالة الحالية بهذا المحفز، يتم الانتقال وتُرجع 1 (نجاح).
    // إذا لم يكن هناك انتقال مناسب، تُرجع 0 (فشل).
    // المعاملات: معرّف الآلة، اسم المحفز.
    auto ai_fireTrigger_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int fsmId = (int)args[0]->toDouble();
        std::string trigger = args[1]->toString();
        return std::make_shared<Data::Value>(sad::ai::fireTrigger(fsmId, trigger) ? 1.0 : 0.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أطلق_محفز", ai_fireTrigger_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_fire_trigger", ai_fireTrigger_func);

    // ─── الحالة_الحالية / ai_get_current_state ───
    // الحصول على المعرّف الرقمي للحالة الحالية في آلة الحالات.
    // مفيد للتحقق من حالة الكيان برمجياً (مثلاً: إذا كان في حالة الهجوم).
    // المعامل: معرّف الآلة.
    auto ai_getCurrentState_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int fsmId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::getCurrentState(fsmId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("الحالة_الحالية", ai_getCurrentState_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_current_state", ai_getCurrentState_func);

    // ─── اسم_الحالة_الحالية / ai_get_current_state_name ───
    // الحصول على الاسم النصي للحالة الحالية (مثلاً: "مطاردة").
    // مفيد لعرض حالة الكيان على الشاشة أو لأغراض التصحيح.
    // المعامل: معرّف الآلة.
    auto ai_getCurrentStateName_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int fsmId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>(sad::ai::getCurrentStateName(fsmId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("اسم_الحالة_الحالية", ai_getCurrentStateName_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_current_state_name", ai_getCurrentStateName_func);

    // ─────────────────────────────────────────────────────────────────────
    // سلوكيات التوجيه (Steering Behaviors)
    // ─────────────────────────────────────────────────────────────────────
    // سلوكيات التوجيه تُحرك الكيانات (أعداء، NPCs) بطريقة طبيعية وذكية.
    // كل كيان له موضع وسرعة وسرعة قصوى، ويمكنه تنفيذ سلوكيات مختلفة:
    //   - المطاردة (Seek): التوجه نحو هدف بأقصى سرعة
    //   - الهروب (Flee): الابتعاد عن تهديد بأقصى سرعة
    //   - التجوال (Wander): حركة عشوائية طبيعية
    //   - الاقتراب (Arrive): مطاردة مع تباطؤ تدريجي عند الاقتراب
    // ─────────────────────────────────────────────────────────────────────

    // ─── أنشئ_كيان / ai_create_agent ───
    // إنشاء كيان ذكي جديد (عدو، NPC، وحش، إلخ) في الموضع المحدد
    // مع سرعة قصوى. الكيان يمكنه تنفيذ سلوكيات التوجيه المختلفة.
    // المعاملات: الموضع الأفقي (س)، الموضع العمودي (ص)، السرعة القصوى.
    // تُرجع معرّف الكيان.
    auto ai_createAgent_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float maxSpeed = (float)args[2]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::createAgent(x, y, maxSpeed));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_كيان", ai_createAgent_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_create_agent", ai_createAgent_func);

    // ─── احذف_كيان / ai_destroy_agent ───
    // حذف كيان وتحرير ذاكرته. المعامل: معرّف الكيان.
    auto ai_destroyAgent_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int agentId = (int)args[0]->toDouble();
        sad::ai::destroyAgent(agentId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_كيان", ai_destroyAgent_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_destroy_agent", ai_destroyAgent_func);

    // ─── عيّن_موضع_كيان / ai_set_agent_position ───
    // تعيين موضع الكيان يدوياً. مفيد لنقل الكيان فوراً إلى موضع جديد
    // (مثلاً: عند ظهوره لأول مرة أو إعادة تعيينه).
    // المعاملات: معرّف الكيان، الموضع الأفقي (س)، الموضع العمودي (ص).
    auto ai_setAgentPosition_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int agentId = (int)args[0]->toDouble();
        float x = (float)args[1]->toDouble();
        float y = (float)args[2]->toDouble();
        sad::ai::setAgentPosition(agentId, x, y);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_موضع_كيان", ai_setAgentPosition_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_set_agent_position", ai_setAgentPosition_func);

    // ─── موضع_كيان_س / ai_get_agent_x ───
    // الحصول على الموضع الأفقي (س) للكيان. مفيد لرسم الكيان أو
    // حساب المسافات. المعامل: معرّف الكيان.
    auto ai_getAgentX_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int agentId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::getAgentX(agentId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موضع_كيان_س", ai_getAgentX_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_agent_x", ai_getAgentX_func);

    // ─── موضع_كيان_ص / ai_get_agent_y ───
    // الحصول على الموضع العمودي (ص) للكيان. مفيد لرسم الكيان أو
    // حساب المسافات. المعامل: معرّف الكيان.
    auto ai_getAgentY_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int agentId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::getAgentY(agentId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موضع_كيان_ص", ai_getAgentY_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_agent_y", ai_getAgentY_func);

    // ─── طارد / ai_seek ───
    // سلوك المطاردة — يجعل الكيان يتجه نحو نقطة الهدف بأقصى سرعة.
    // هذا أبسط سلوك توجيه: الكيان يتحرك مباشرةً نحو الهدف بدون تباطؤ.
    // المعاملات: معرّف الكيان، موضع الهدف (س، ص)، الزمن المنقضي (dt).
    auto ai_seek_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int agentId = (int)args[0]->toDouble();
        float tx = (float)args[1]->toDouble();
        float ty = (float)args[2]->toDouble();
        float dt = (float)args[3]->toDouble();
        sad::ai::seek(agentId, tx, ty, dt);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("طارد", ai_seek_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_seek", ai_seek_func);

    // ─── اهرب / ai_flee ───
    // سلوك الهروب — يجعل الكيان يبتعد عن نقطة التهديد بأقصى سرعة.
    // عكس المطاردة تماماً — مفيد لسلوك الخوف أو التراجع.
    // المعاملات: معرّف الكيان، موضع التهديد (س، ص)، الزمن المنقضي (dt).
    auto ai_flee_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int agentId = (int)args[0]->toDouble();
        float tx = (float)args[1]->toDouble();
        float ty = (float)args[2]->toDouble();
        float dt = (float)args[3]->toDouble();
        sad::ai::flee(agentId, tx, ty, dt);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("اهرب", ai_flee_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_flee", ai_flee_func);

    // ─── تجوّل / ai_wander ───
    // سلوك التجوال العشوائي — يجعل الكيان يتحرك بحركة عشوائية طبيعية
    // تشبه حركة الحيوانات في الطبيعة. لا يحتاج هدفاً محدداً.
    // المعاملات: معرّف الكيان، الزمن المنقضي (dt).
    auto ai_wander_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int agentId = (int)args[0]->toDouble();
        float dt = (float)args[1]->toDouble();
        sad::ai::wander(agentId, dt);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تجوّل", ai_wander_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_wander", ai_wander_func);

    // ─── اقترب / ai_arrive ───
    // سلوك الاقتراب — مثل المطاردة ولكن مع تباطؤ تدريجي عند الاقتراب
    // من الهدف. يمنع الكيان من التجاوز ذهاباً وإياباً حول الهدف.
    // نصف قطر التباطؤ (slowRadius) يحدد متى يبدأ التباطؤ.
    // المعاملات: معرّف الكيان، موضع الهدف (س، ص)، نصف قطر التباطؤ، الزمن (dt).
    auto ai_arrive_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int agentId = (int)args[0]->toDouble();
        float tx = (float)args[1]->toDouble();
        float ty = (float)args[2]->toDouble();
        float slowR = (float)args[3]->toDouble();
        float dt = (float)args[4]->toDouble();
        sad::ai::arrive(agentId, tx, ty, slowR, dt);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("اقترب", ai_arrive_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_arrive", ai_arrive_func);

    // ─── هل_يرى / ai_can_see ───
    // التحقق هل الكيان يمكنه رؤية النقطة المحددة (ضمن مدى رؤيته).
    // تُرجع 1 إذا كانت النقطة ضمن مدى الرؤية، و0 إذا كانت خارجه.
    // مفيد لتحديد متى يبدأ العدو في مطاردة اللاعب.
    // المعاملات: معرّف الكيان، موضع الهدف (س، ص).
    auto ai_canSee_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int agentId = (int)args[0]->toDouble();
        float tx = (float)args[1]->toDouble();
        float ty = (float)args[2]->toDouble();
        return std::make_shared<Data::Value>(sad::ai::canSee(agentId, tx, ty) ? 1.0 : 0.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هل_يرى", ai_canSee_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_can_see", ai_canSee_func);

    // ─────────────────────────────────────────────────────────────────────
    // سلوك الأسراب (Flocking)
    // ─────────────────────────────────────────────────────────────────────
    // محاكاة حركة مجموعة من الكيانات (مثل سرب طيور أو مجموعة أسماك).
    // يعتمد على ثلاث قواعد بسيطة:
    //   1. الفصل (Separation): تجنب التصادم مع الجيران
    //   2. المحاذاة (Alignment): محاذاة الاتجاه مع الجيران
    //   3. التماسك (Cohesion): الانجذاب نحو مركز المجموعة
    // ─────────────────────────────────────────────────────────────────────

    // ─── أنشئ_سرب / ai_create_flock ───
    // إنشاء سرب جديد من الكيانات موزعين عشوائياً في المنطقة المحددة.
    // المعاملات: عدد أفراد السرب، منطقة التوزيع (س، ص، عرض، ارتفاع).
    // تُرجع معرّف السرب.
    auto ai_createFlock_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int count = (int)args[0]->toDouble();
        float ax = (float)args[1]->toDouble();
        float ay = (float)args[2]->toDouble();
        float aw = (float)args[3]->toDouble();
        float ah = (float)args[4]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::createFlock(count, ax, ay, aw, ah));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_سرب", ai_createFlock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_create_flock", ai_createFlock_func);

    // ─── احذف_سرب / ai_destroy_flock ───
    // حذف السرب وتحرير ذاكرته. المعامل: معرّف السرب.
    auto ai_destroyFlock_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int flockId = (int)args[0]->toDouble();
        sad::ai::destroyFlock(flockId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_سرب", ai_destroyFlock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_destroy_flock", ai_destroyFlock_func);

    // ─── حدّث_سرب / ai_update_flock ───
    // تحديث حركة جميع أفراد السرب — يُطبق قواعد الفصل والمحاذاة والتماسك.
    // يجب استدعاء هذه الدالة في كل إطار (frame) لتحريك السرب.
    // المعاملات: معرّف السرب، الزمن المنقضي (dt).
    auto ai_updateFlock_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int flockId = (int)args[0]->toDouble();
        float dt = (float)args[1]->toDouble();
        sad::ai::updateFlock(flockId, dt);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حدّث_سرب", ai_updateFlock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_update_flock", ai_updateFlock_func);

    // ─── موضع_عنصر_سرب_س / ai_get_flock_member_x ───
    // الحصول على الموضع الأفقي (س) لعنصر معين في السرب.
    // المعاملات: معرّف السرب، رقم العنصر (الفهرس يبدأ من 0).
    auto ai_getFlockMemberX_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int flockId = (int)args[0]->toDouble();
        int index = (int)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::getFlockMemberX(flockId, index));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موضع_عنصر_سرب_س", ai_getFlockMemberX_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_flock_member_x", ai_getFlockMemberX_func);

    // ─── موضع_عنصر_سرب_ص / ai_get_flock_member_y ───
    // الحصول على الموضع العمودي (ص) لعنصر معين في السرب.
    // المعاملات: معرّف السرب، رقم العنصر (الفهرس يبدأ من 0).
    auto ai_getFlockMemberY_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int flockId = (int)args[0]->toDouble();
        int index = (int)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::getFlockMemberY(flockId, index));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موضع_عنصر_سرب_ص", ai_getFlockMemberY_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_flock_member_y", ai_getFlockMemberY_func);

    // ─── حجم_السرب / ai_get_flock_size ───
    // الحصول على عدد أفراد السرب. مفيد للتكرار على جميع الأفراد لرسمهم.
    // المعامل: معرّف السرب.
    auto ai_getFlockSize_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int flockId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::getFlockSize(flockId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حجم_السرب", ai_getFlockSize_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_flock_size", ai_getFlockSize_func);

    // ─── عيّن_هدف_سرب / ai_set_flock_target ───
    // تعيين نقطة هدف للسرب — يتحرك أفراد السرب تدريجياً نحو هذه النقطة
    // مع الحفاظ على سلوك السرب (الفصل والمحاذاة والتماسك).
    // المعاملات: معرّف السرب، موضع الهدف (س، ص).
    auto ai_setFlockTarget_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int flockId = (int)args[0]->toDouble();
        float x = (float)args[1]->toDouble();
        float y = (float)args[2]->toDouble();
        sad::ai::setFlockTarget(flockId, x, y);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_هدف_سرب", ai_setFlockTarget_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_set_flock_target", ai_setFlockTarget_func);

    // ─── ارسم_سرب / ai_render_flock ───
    // رسم جميع أفراد السرب كنقاط ملونة على الشاشة. دالة مساعدة سريعة
    // لعرض السرب بدون كتابة حلقة رسم يدوية.
    // المعاملات: المُصيِّر (renderer)، معرّف السرب، اللون (أحمر، أخضر، أزرق).
    auto ai_renderFlock_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int flockId = (int)args[1]->toDouble();
        uint8_t r = (uint8_t)(int)args[2]->toDouble();
        uint8_t g = (uint8_t)(int)args[3]->toDouble();
        uint8_t b = (uint8_t)(int)args[4]->toDouble();
        sad::ai::renderFlock(renderer, flockId, r, g, b);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_سرب", ai_renderFlock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_render_flock", ai_renderFlock_func);

    // ═══════════════════════════════════════════════════════════════════════════
    // نظام الكاميرا ثنائية الأبعاد (2D Camera System) — sad::camera2d
    // ═══════════════════════════════════════════════════════════════════════════
    // نظام كاميرا متقدم يوفر تحكماً كاملاً بنافذة العرض في الألعاب ثنائية الأبعاد:
    //   - الموضع والتحريك: تعيين موضع الكاميرا أو تحريكها نسبياً
    //   - التكبير والتصغير (Zoom): مع حدود دنيا وقصوى
    //   - الدوران: تدوير الكاميرا حول مركزها
    //   - متابعة الهدف (Follow): مع تنعيم ومنطقة ميتة ونظر أمامي
    //   - حدود الكاميرا (Bounds): لمنع الخروج عن الخريطة
    //   - الاهتزاز (Screen Shake): تأثيرات اهتزاز بقوة ومدة محددة
    //   - الحركة السينمائية (Pan): تحريك سلس بين نقطتين
    //   - تحويل الإحداثيات: من العالم للشاشة والعكس
    // ═══════════════════════════════════════════════════════════════════════════

    // ─── أنشئ_كاميرا / cam2d_create ───
    // إنشاء كاميرا ثنائية أبعاد جديدة بأبعاد نافذة العرض المحددة.
    // عادةً تكون الأبعاد مساوية لحجم الشاشة (مثلاً: 800×600).
    // المعاملات: عرض نافذة العرض، ارتفاع نافذة العرض.
    // تُرجع معرّف الكاميرا لاستخدامه في الدوال الأخرى.
    auto cam2d_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int w = (int)args[0]->toDouble();
        int h = (int)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::createCamera(w, h));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_كاميرا", cam2d_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_create", cam2d_create_func);

    // ─── احذف_كاميرا / cam2d_destroy ───
    // حذف الكاميرا وتحرير ذاكرتها. المعامل: معرّف الكاميرا.
    auto cam2d_destroy_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        sad::camera2d::destroyCamera(camId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_كاميرا", cam2d_destroy_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_destroy", cam2d_destroy_func);

    // ─── عيّن_موضع_كاميرا_2D / cam2d_set_position ───
    // تعيين موضع مركز الكاميرا مباشرةً. الكاميرا تعرض المنطقة المحيطة
    // بهذا الموضع. مفيد لنقل الكاميرا فوراً (بدون تنعيم).
    // المعاملات: معرّف الكاميرا، الموضع الأفقي (س)، الموضع العمودي (ص).
    auto cam2d_setPosition_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float x = (float)args[1]->toDouble();
        float y = (float)args[2]->toDouble();
        sad::camera2d::setCamPosition(camId, x, y);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_موضع_كاميرا_2D", cam2d_setPosition_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_set_position", cam2d_setPosition_func);

    // ─── موضع_كاميرا_س / cam2d_get_x ───
    // الحصول على الموضع الأفقي (س) لمركز الكاميرا.
    // المعامل: معرّف الكاميرا.
    auto cam2d_getX_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::getCamX(camId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موضع_كاميرا_س", cam2d_getX_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_get_x", cam2d_getX_func);

    // ─── موضع_كاميرا_ص / cam2d_get_y ───
    // الحصول على الموضع العمودي (ص) لمركز الكاميرا.
    // المعامل: معرّف الكاميرا.
    auto cam2d_getY_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::getCamY(camId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موضع_كاميرا_ص", cam2d_getY_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_get_y", cam2d_getY_func);

    // ─── حرّك_كاميرا / cam2d_move ───
    // تحريك الكاميرا بمقدار نسبي من موضعها الحالي.
    // المعاملات: معرّف الكاميرا، الإزاحة الأفقية (dx)، الإزاحة العمودية (dy).
    auto cam2d_move_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float dx = (float)args[1]->toDouble();
        float dy = (float)args[2]->toDouble();
        sad::camera2d::moveCam(camId, dx, dy);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حرّك_كاميرا", cam2d_move_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_move", cam2d_move_func);

    // ─── عيّن_تكبير / cam2d_set_zoom ───
    // تعيين مستوى التكبير. القيمة 1.0 تعني العرض الطبيعي، 2.0 تعني
    // تكبير مضاعف (كل شيء يبدو أكبر)، 0.5 تعني تصغير (كل شيء أصغر).
    // المعاملات: معرّف الكاميرا، مستوى التكبير.
    auto cam2d_setZoom_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float zoom = (float)args[1]->toDouble();
        sad::camera2d::setCamZoom(camId, zoom);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_تكبير", cam2d_setZoom_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_set_zoom", cam2d_setZoom_func);

    // ─── تكبير_الكاميرا / cam2d_get_zoom ───
    // الحصول على مستوى التكبير الحالي للكاميرا. المعامل: معرّف الكاميرا.
    auto cam2d_getZoom_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::getCamZoom(camId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تكبير_الكاميرا", cam2d_getZoom_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_get_zoom", cam2d_getZoom_func);

    // ─── حدود_تكبير / cam2d_set_zoom_limits ───
    // تعيين الحدود الدنيا والقصوى للتكبير. يمنع المستخدم من التكبير
    // أكثر من اللازم أو التصغير بشكل مفرط.
    // المعاملات: معرّف الكاميرا، الحد الأدنى، الحد الأقصى.
    auto cam2d_setZoomLimits_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float minZ = (float)args[1]->toDouble();
        float maxZ = (float)args[2]->toDouble();
        sad::camera2d::setCamZoomLimits(camId, minZ, maxZ);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حدود_تكبير", cam2d_setZoomLimits_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_set_zoom_limits", cam2d_setZoomLimits_func);

    // ─── عيّن_دوران_كاميرا / cam2d_set_rotation ───
    // تعيين زاوية دوران الكاميرا (بالدرجات). الدوران يكون حول مركز الكاميرا.
    // مفيد لتأثيرات مثل ميلان الشاشة عند الضرر أو الانعطاف.
    // المعاملات: معرّف الكاميرا، الزاوية (بالدرجات).
    auto cam2d_setRotation_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float degrees = (float)args[1]->toDouble();
        sad::camera2d::setCamRotation(camId, degrees);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_دوران_كاميرا", cam2d_setRotation_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_set_rotation", cam2d_setRotation_func);

    // ─── دوران_الكاميرا / cam2d_get_rotation ───
    // الحصول على زاوية الدوران الحالية للكاميرا (بالدرجات).
    // المعامل: معرّف الكاميرا.
    auto cam2d_getRotation_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::getCamRotation(camId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("دوران_الكاميرا", cam2d_getRotation_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_get_rotation", cam2d_getRotation_func);

    // ─── تابع_هدف / cam2d_follow_target ───
    // جعل الكاميرا تتبع نقطة هدف تلقائياً مع تنعيم (smoothing).
    // قيمة التنعيم بين 0 و1: قريبة من 0 = بطيئة وسلسة، قريبة من 1 = سريعة.
    // يُستدعى كل إطار مع موضع اللاعب الحالي.
    // المعاملات: معرّف الكاميرا، موضع الهدف (س، ص)، معامل التنعيم.
    auto cam2d_followTarget_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float tx = (float)args[1]->toDouble();
        float ty = (float)args[2]->toDouble();
        float smoothing = (float)args[3]->toDouble();
        sad::camera2d::followTarget(camId, tx, ty, smoothing);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تابع_هدف", cam2d_followTarget_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_follow_target", cam2d_followTarget_func);

    // ─── منطقة_ميتة / cam2d_set_dead_zone ───
    // تعيين المنطقة الميتة (Dead Zone) في وسط الشاشة. عندما يتحرك الهدف
    // داخل هذه المنطقة، الكاميرا لا تتحرك — فقط عند خروجه منها.
    // يجعل حركة الكاميرا أقل اضطراباً مع الحركات الصغيرة.
    // المعاملات: معرّف الكاميرا، عرض المنطقة، ارتفاع المنطقة.
    auto cam2d_setDeadZone_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float w = (float)args[1]->toDouble();
        float h = (float)args[2]->toDouble();
        sad::camera2d::setDeadZone(camId, w, h);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("منطقة_ميتة", cam2d_setDeadZone_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_set_dead_zone", cam2d_setDeadZone_func);

    // ─── نظر_أمامي / cam2d_set_look_ahead ───
    // تعيين مقدار النظر الأمامي — الكاميرا تنظر قليلاً في اتجاه حركة
    // الهدف لإعطاء اللاعب رؤية أفضل لما أمامه.
    // المعاملات: معرّف الكاميرا، مقدار النظر الأمامي (بالبكسل).
    auto cam2d_setLookAhead_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float amount = (float)args[1]->toDouble();
        sad::camera2d::setLookAhead(camId, amount);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("نظر_أمامي", cam2d_setLookAhead_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_set_look_ahead", cam2d_setLookAhead_func);

    // ─── حدود_كاميرا / cam2d_set_bounds ───
    // تعيين حدود الكاميرا — تمنع الكاميرا من عرض مناطق خارج الخريطة.
    // الحدود تُعرَّف بالإحداثيات: يسار، أعلى، يمين، أسفل.
    // المعاملات: معرّف الكاميرا، الحد الأيسر، العلوي، الأيمن، السفلي.
    auto cam2d_setBounds_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float l = (float)args[1]->toDouble();
        float t = (float)args[2]->toDouble();
        float r = (float)args[3]->toDouble();
        float b = (float)args[4]->toDouble();
        sad::camera2d::setCamBounds(camId, l, t, r, b);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حدود_كاميرا", cam2d_setBounds_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_set_bounds", cam2d_setBounds_func);

    // ─── ألغ_حدود_كاميرا / cam2d_clear_bounds ───
    // إلغاء حدود الكاميرا — السماح لها بالتحرك بحرية بدون قيود.
    // المعامل: معرّف الكاميرا.
    auto cam2d_clearBounds_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        sad::camera2d::clearCamBounds(camId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ألغ_حدود_كاميرا", cam2d_clearBounds_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_clear_bounds", cam2d_clearBounds_func);

    // ─── هزّ_الكاميرا / cam2d_shake ───
    // بدء تأثير اهتزاز الشاشة — يُستخدم عند الضربات القوية والانفجارات.
    // الاهتزاز يضعف تدريجياً خلال المدة المحددة.
    // المعاملات: معرّف الكاميرا، المدة (بالثواني)، الشدة (بالبكسل).
    auto cam2d_shake_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float duration = (float)args[1]->toDouble();
        float intensity = (float)args[2]->toDouble();
        sad::camera2d::shakeCamera(camId, duration, intensity);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هزّ_الكاميرا", cam2d_shake_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_shake", cam2d_shake_func);

    // ─── أوقف_الاهتزاز / cam2d_stop_shake ───
    // إيقاف الاهتزاز فوراً (بدون انتظار انتهاء المدة).
    // المعامل: معرّف الكاميرا.
    auto cam2d_stopShake_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        sad::camera2d::stopShake(camId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أوقف_الاهتزاز", cam2d_stopShake_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_stop_shake", cam2d_stopShake_func);

    // ─── حرّك_نحو / cam2d_pan_to ───
    // تحريك الكاميرا بسلاسة من موضعها الحالي إلى نقطة الهدف خلال
    // المدة المحددة. مفيد للمشاهد السينمائية وعرض مناطق الخريطة.
    // المعاملات: معرّف الكاميرا، موضع الهدف (س، ص)، المدة (بالثواني).
    auto cam2d_panTo_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float tx = (float)args[1]->toDouble();
        float ty = (float)args[2]->toDouble();
        float duration = (float)args[3]->toDouble();
        sad::camera2d::panTo(camId, tx, ty, duration);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حرّك_نحو", cam2d_panTo_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_pan_to", cam2d_panTo_func);

    // ─── هل_تتحرك / cam2d_is_panning ───
    // التحقق هل الكاميرا في حالة حركة سينمائية (pan) حالياً.
    // تُرجع 1 إذا كانت تتحرك، و0 إذا انتهت الحركة.
    // المعامل: معرّف الكاميرا.
    auto cam2d_isPanning_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>(sad::camera2d::isPanning(camId) ? 1.0 : 0.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هل_تتحرك", cam2d_isPanning_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_is_panning", cam2d_isPanning_func);

    // ─── حدّث_كاميرا / cam2d_update ───
    // تحديث الكاميرا — يجب استدعاء هذه الدالة في كل إطار.
    // تُعالج: متابعة الهدف، الاهتزاز، الحركة السينمائية، تطبيق الحدود.
    // المعاملات: معرّف الكاميرا، الزمن المنقضي (dt).
    auto cam2d_update_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float dt = (float)args[1]->toDouble();
        sad::camera2d::updateCamera(camId, dt);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حدّث_كاميرا", cam2d_update_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_update", cam2d_update_func);

    // ─── عالم_إلى_شاشة_س / cam2d_world_to_screen_x ───
    // تحويل إحداثي أفقي من نظام العالم إلى نظام الشاشة.
    // مفيد لمعرفة أين سيظهر كائن معين على الشاشة بعد تطبيق الكاميرا.
    // المعاملات: معرّف الكاميرا، الإحداثي الأفقي في العالم.
    auto cam2d_worldToScreenX_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float wx = (float)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::worldToScreenX(camId, wx));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عالم_إلى_شاشة_س", cam2d_worldToScreenX_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_world_to_screen_x", cam2d_worldToScreenX_func);

    // ─── عالم_إلى_شاشة_ص / cam2d_world_to_screen_y ───
    // تحويل إحداثي عمودي من نظام العالم إلى نظام الشاشة.
    // المعاملات: معرّف الكاميرا، الإحداثي العمودي في العالم.
    auto cam2d_worldToScreenY_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float wy = (float)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::worldToScreenY(camId, wy));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عالم_إلى_شاشة_ص", cam2d_worldToScreenY_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_world_to_screen_y", cam2d_worldToScreenY_func);

    // ─── شاشة_إلى_عالم_س / cam2d_screen_to_world_x ───
    // تحويل إحداثي أفقي من نظام الشاشة إلى نظام العالم.
    // مفيد لتحديد النقطة التي نقر عليها المستخدم في عالم اللعبة.
    // المعاملات: معرّف الكاميرا، الإحداثي الأفقي على الشاشة.
    auto cam2d_screenToWorldX_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float sx = (float)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::screenToWorldX(camId, sx));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("شاشة_إلى_عالم_س", cam2d_screenToWorldX_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_screen_to_world_x", cam2d_screenToWorldX_func);

    // ─── شاشة_إلى_عالم_ص / cam2d_screen_to_world_y ───
    // تحويل إحداثي عمودي من نظام الشاشة إلى نظام العالم.
    // المعاملات: معرّف الكاميرا، الإحداثي العمودي على الشاشة.
    auto cam2d_screenToWorldY_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float sy = (float)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::screenToWorldY(camId, sy));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("شاشة_إلى_عالم_ص", cam2d_screenToWorldY_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_screen_to_world_y", cam2d_screenToWorldY_func);

    // ─── ابدأ_كاميرا / cam2d_begin ───
    // تفعيل الكاميرا — كل عمليات الرسم بعد هذا السطر ستُحوَّل حسب
    // موضع الكاميرا وتكبيرها ودورانها. يجب استدعاء أنهِ_كاميرا بعد الانتهاء.
    // المعامل: معرّف الكاميرا.
    auto cam2d_begin_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        sad::camera2d::beginCamera(camId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ابدأ_كاميرا", cam2d_begin_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_begin", cam2d_begin_func);

    // ─── أنهِ_كاميرا / cam2d_end ───
    // إنهاء تحويل الكاميرا — العودة لنظام الإحداثيات العادي (الشاشة).
    // كل الرسم بعد هذا السطر لن يتأثر بالكاميرا (مفيد لعناصر الواجهة HUD).
    auto cam2d_end_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        sad::camera2d::endCamera();
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنهِ_كاميرا", cam2d_end_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_end", cam2d_end_func);

    // ─── إزاحة_كاميرا_س / cam2d_get_offset_x ───
    // الحصول على الإزاحة الأفقية الحالية للكاميرا (تشمل التنعيم والاهتزاز).
    // مفيد للاستخدام اليدوي مع SDL بدلاً من beginCamera/endCamera.
    // المعامل: معرّف الكاميرا.
    auto cam2d_getOffsetX_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::getOffsetX(camId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("إزاحة_كاميرا_س", cam2d_getOffsetX_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_get_offset_x", cam2d_getOffsetX_func);

    // ─── إزاحة_كاميرا_ص / cam2d_get_offset_y ───
    // الحصول على الإزاحة العمودية الحالية للكاميرا (تشمل التنعيم والاهتزاز).
    // المعامل: معرّف الكاميرا.
    auto cam2d_getOffsetY_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::getOffsetY(camId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("إزاحة_كاميرا_ص", cam2d_getOffsetY_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_get_offset_y", cam2d_getOffsetY_func);

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

    // ========================================================================
    // Async/Await & Concurrency Builtins (23 functions)
    // Simulated using std::thread, std::mutex, std::condition_variable
    // ========================================================================

    // --- Shared async state ---
    static std::atomic<int64_t> async_task_counter{1};
    static std::mutex async_tasks_mutex;
    static std::unordered_map<int64_t, std::shared_ptr<std::thread>> async_tasks;
    static std::unordered_map<int64_t, std::shared_ptr<Data::Value>> async_results;
    
    static std::atomic<int64_t> async_future_counter{1};
    static std::mutex async_futures_mutex;
    struct AsyncFuture {
        std::mutex mtx;
        std::condition_variable cv;
        std::shared_ptr<Data::Value> value;
        bool resolved = false;
    };
    static std::unordered_map<int64_t, std::shared_ptr<AsyncFuture>> async_futures;
    
    static std::atomic<int64_t> async_channel_counter{1};
    static std::mutex async_channels_mutex;
    struct AsyncChannel {
        std::mutex mtx;
        std::condition_variable cv_send, cv_recv;
        std::queue<std::shared_ptr<Data::Value>> buffer;
        int64_t capacity = 1;
        bool closed = false;
    };
    static std::unordered_map<int64_t, std::shared_ptr<AsyncChannel>> async_channels;
    
    static std::atomic<int64_t> async_mutex_counter{1};
    static std::mutex async_mutexes_mutex;
    static std::unordered_map<int64_t, std::shared_ptr<std::mutex>> async_mutexes;
    
    static std::atomic<int64_t> async_atomic_counter{1};
    static std::mutex async_atomics_mutex;
    static std::unordered_map<int64_t, std::shared_ptr<std::atomic<int64_t>>> async_atomics;

    // 1. spawn / async_spawn
    auto async_spawn_func = [&interpreter](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int64_t taskId = async_task_counter.fetch_add(1);
        std::cout << "[ASYNC-SIM] Spawned task #" << taskId << std::endl;
        // In simulation, we just return the task ID
        return std::make_shared<Data::Value>(static_cast<int>(taskId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x85\xd9\x87\xd9\x85\xd8\xa9", async_spawn_func);
    interpreter.getFunctionManager().registerBuiltinFunction("spawn", async_spawn_func);
    interpreter.getFunctionManager().registerBuiltinFunction("async_spawn", async_spawn_func);

    // 2. await / async_await
    auto async_await_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("await requires 1 argument (task_id)");
        int64_t taskId = 0;
        if (args[0]->isInteger()) taskId = args[0]->toInt();
        else if (args[0]->isDouble()) taskId = static_cast<int64_t>(args[0]->toDouble());
        std::cout << "[ASYNC-SIM] Awaiting task #" << taskId << std::endl;
        // Check if there's a future with this ID
        {
            std::lock_guard<std::mutex> lock(async_futures_mutex);
            auto it = async_futures.find(taskId);
            if (it != async_futures.end()) {
                auto& future = it->second;
                std::unique_lock<std::mutex> flock(future->mtx);
                future->cv.wait(flock, [&]{ return future->resolved; });
                std::cout << "[ASYNC-SIM] Task #" << taskId << " completed" << std::endl;
                return future->value ? future->value : std::make_shared<Data::Value>(0);
            }
        }
        std::cout << "[ASYNC-SIM] Task #" << taskId << " completed (immediate)" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1_\xd9\x85\xd9\x87\xd9\x85\xd8\xa9", async_await_func);
    interpreter.getFunctionManager().registerBuiltinFunction("await", async_await_func);
    interpreter.getFunctionManager().registerBuiltinFunction("async_await", async_await_func);
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1", async_await_func);

    // 3. yield / async_yield
    auto async_yield_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[ASYNC-SIM] Yielding execution" << std::endl;
        std::this_thread::yield();
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x86\xd8\xaa\xd8\xac", async_yield_func);
    interpreter.getFunctionManager().registerBuiltinFunction("yield", async_yield_func);
    interpreter.getFunctionManager().registerBuiltinFunction("async_yield", async_yield_func);

    // 4. async_sleep
    auto async_sleep_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("async_sleep requires 1 argument (ms)");
        int64_t ms = 0;
        if (args[0]->isInteger()) ms = args[0]->toInt();
        else if (args[0]->isDouble()) ms = static_cast<int64_t>(args[0]->toDouble());
        std::cout << "[ASYNC-SIM] Sleeping for " << ms << "ms" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x86\xd9\x88\xd9\x85_\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86", async_sleep_func);
    interpreter.getFunctionManager().registerBuiltinFunction("async_sleep", async_sleep_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sleep_async", async_sleep_func);

    // 5. create_future
    auto create_future_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int64_t futureId = async_future_counter.fetch_add(1);
        auto future = std::make_shared<AsyncFuture>();
        {
            std::lock_guard<std::mutex> lock(async_futures_mutex);
            async_futures[futureId] = future;
        }
        std::cout << "[ASYNC-SIM] Created future #" << futureId << std::endl;
        return std::make_shared<Data::Value>(static_cast<int>(futureId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84", create_future_func);
    interpreter.getFunctionManager().registerBuiltinFunction("create_future", create_future_func);
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84", create_future_func);

    // 6. resolve_future
    auto resolve_future_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("resolve_future requires 2 args (future_id, value)");
        int64_t futureId = args[0]->toInt();
        std::lock_guard<std::mutex> lock(async_futures_mutex);
        auto it = async_futures.find(futureId);
        if (it != async_futures.end()) {
            auto& future = it->second;
            std::lock_guard<std::mutex> flock(future->mtx);
            future->value = args[1];
            future->resolved = true;
            future->cv.notify_all();
            std::cout << "[ASYNC-SIM] Resolved future #" << futureId << std::endl;
        } else {
            std::cout << "[ASYNC-SIM] Future #" << futureId << " not found" << std::endl;
        }
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x88\xd9\x81_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84", resolve_future_func);
    interpreter.getFunctionManager().registerBuiltinFunction("resolve_future", resolve_future_func);
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xad\xd9\x82\xd9\x82_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84", resolve_future_func);

    // 7. get_future
    auto get_future_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("get_future requires 1 argument (future_id)");
        int64_t futureId = args[0]->toInt();
        std::lock_guard<std::mutex> lock(async_futures_mutex);
        auto it = async_futures.find(futureId);
        if (it != async_futures.end()) {
            auto& future = it->second;
            if (future->resolved && future->value) {
                std::cout << "[ASYNC-SIM] Got future #" << futureId << " value" << std::endl;
                return future->value;
            }
        }
        std::cout << "[ASYNC-SIM] Future #" << futureId << " pending (returning 0)" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xad\xd8\xb5\xd9\x84_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84", get_future_func);
    interpreter.getFunctionManager().registerBuiltinFunction("get_future", get_future_func);

    // 8. create_channel
    auto create_channel_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int64_t chanId = async_channel_counter.fetch_add(1);
        auto chan = std::make_shared<AsyncChannel>();
        if (!args.empty()) {
            chan->capacity = args[0]->isInteger() ? args[0]->toInt() : 1;
        }
        {
            std::lock_guard<std::mutex> lock(async_channels_mutex);
            async_channels[chanId] = chan;
        }
        std::cout << "[ASYNC-SIM] Created channel #" << chanId << " (capacity=" << chan->capacity << ")" << std::endl;
        return std::make_shared<Data::Value>(static_cast<int>(chanId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9", create_channel_func);
    interpreter.getFunctionManager().registerBuiltinFunction("create_channel", create_channel_func);
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9", create_channel_func);

    // 9. channel_send
    auto channel_send_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("channel_send requires 2 args (channel_id, value)");
        int64_t chanId = args[0]->toInt();
        std::lock_guard<std::mutex> lock(async_channels_mutex);
        auto it = async_channels.find(chanId);
        if (it != async_channels.end()) {
            auto& chan = it->second;
            std::lock_guard<std::mutex> clock(chan->mtx);
            chan->buffer.push(args[1]);
            chan->cv_recv.notify_one();
            std::cout << "[ASYNC-SIM] Sent to channel #" << chanId << std::endl;
        } else {
            std::cout << "[ASYNC-SIM] Channel #" << chanId << " not found" << std::endl;
        }
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd8\xb1\xd8\xb3\xd9\x84_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9", channel_send_func);
    interpreter.getFunctionManager().registerBuiltinFunction("channel_send", channel_send_func);
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd8\xb1\xd8\xb3\xd9\x84", channel_send_func);

    // 10. channel_recv
    auto channel_recv_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("channel_recv requires 1 argument (channel_id)");
        int64_t chanId = args[0]->toInt();
        std::lock_guard<std::mutex> lock(async_channels_mutex);
        auto it = async_channels.find(chanId);
        if (it != async_channels.end()) {
            auto& chan = it->second;
            std::lock_guard<std::mutex> clock(chan->mtx);
            if (!chan->buffer.empty()) {
                auto val = chan->buffer.front();
                chan->buffer.pop();
                chan->cv_send.notify_one();
                std::cout << "[ASYNC-SIM] Received from channel #" << chanId << std::endl;
                return val;
            }
            std::cout << "[ASYNC-SIM] Channel #" << chanId << " empty" << std::endl;
        }
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9", channel_recv_func);
    interpreter.getFunctionManager().registerBuiltinFunction("channel_recv", channel_recv_func);
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84", channel_recv_func);

    // 11. channel_close
    auto channel_close_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("channel_close requires 1 argument (channel_id)");
        int64_t chanId = args[0]->toInt();
        std::lock_guard<std::mutex> lock(async_channels_mutex);
        auto it = async_channels.find(chanId);
        if (it != async_channels.end()) {
            it->second->closed = true;
            async_channels.erase(it);
            std::cout << "[ASYNC-SIM] Closed channel #" << chanId << std::endl;
        }
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd8\xba\xd9\x84\xd9\x82_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9", channel_close_func);
    interpreter.getFunctionManager().registerBuiltinFunction("channel_close", channel_close_func);

    // 12. create_mutex
    auto create_mutex_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int64_t mutexId = async_mutex_counter.fetch_add(1);
        auto mtx = std::make_shared<std::mutex>();
        {
            std::lock_guard<std::mutex> lock(async_mutexes_mutex);
            async_mutexes[mutexId] = mtx;
        }
        std::cout << "[ASYNC-SIM] Created mutex #" << mutexId << std::endl;
        return std::make_shared<Data::Value>(static_cast<int>(mutexId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x82\xd9\x81\xd9\x84", create_mutex_func);
    interpreter.getFunctionManager().registerBuiltinFunction("create_mutex", create_mutex_func);
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x82\xd9\x81\xd9\x84", create_mutex_func);

    // 13. mutex_lock
    auto mutex_lock_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("mutex_lock requires 1 argument (mutex_id)");
        int64_t mutexId = args[0]->toInt();
        std::lock_guard<std::mutex> lock(async_mutexes_mutex);
        auto it = async_mutexes.find(mutexId);
        if (it != async_mutexes.end()) {
            it->second->lock();
            std::cout << "[ASYNC-SIM] Locked mutex #" << mutexId << std::endl;
        }
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd9\x81\xd9\x84", mutex_lock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mutex_lock", mutex_lock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("lock", mutex_lock_func);

    // 14. mutex_unlock
    auto mutex_unlock_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("mutex_unlock requires 1 argument (mutex_id)");
        int64_t mutexId = args[0]->toInt();
        std::lock_guard<std::mutex> lock(async_mutexes_mutex);
        auto it = async_mutexes.find(mutexId);
        if (it != async_mutexes.end()) {
            it->second->unlock();
            std::cout << "[ASYNC-SIM] Unlocked mutex #" << mutexId << std::endl;
        }
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x81\xd8\xaa\xd8\xad_\xd9\x82\xd9\x81\xd9\x84", mutex_unlock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("mutex_unlock", mutex_unlock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("unlock", mutex_unlock_func);

    // 15. thread_spawn
    auto thread_spawn_func = [&interpreter](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int64_t threadId = async_task_counter.fetch_add(1);
        std::cout << "[ASYNC-SIM] Spawned thread #" << threadId << std::endl;
        return std::make_shared<Data::Value>(static_cast<int>(threadId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd8\xae\xd9\x8a\xd8\xb7", thread_spawn_func);
    interpreter.getFunctionManager().registerBuiltinFunction("thread_spawn", thread_spawn_func);
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xae\xd9\x8a\xd8\xb7", thread_spawn_func);

    // 16. thread_join
    auto thread_join_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("thread_join requires 1 argument (thread_id)");
        int64_t threadId = args[0]->toInt();
        std::cout << "[ASYNC-SIM] Joined thread #" << threadId << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x86\xd8\xb6\xd9\x85_\xd8\xae\xd9\x8a\xd8\xb7", thread_join_func);
    interpreter.getFunctionManager().registerBuiltinFunction("thread_join", thread_join_func);

    // 16b. create_atomic — إنشاء متغير ذري
    auto create_atomic_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int64_t initVal = 0;
        if (!args.empty()) initVal = args[0]->toInt();
        int64_t id = async_atomic_counter.fetch_add(1);
        auto atom = std::make_shared<std::atomic<int64_t>>(initVal);
        {
            std::lock_guard<std::mutex> lock(async_atomics_mutex);
            async_atomics[id] = atom;
        }
        std::cout << "[ASYNC-SIM] Created atomic #" << id << " (initial=" << initVal << ")" << std::endl;
        return std::make_shared<Data::Value>(static_cast<int>(id));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd8\xb0\xd8\xb1\xd9\x8a", create_atomic_func);
    interpreter.getFunctionManager().registerBuiltinFunction("create_atomic", create_atomic_func);

    // 17. atomic_load
    auto atomic_load_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("atomic_load requires 1 argument (atomic_id)");
        int64_t atomicId = args[0]->toInt();
        std::lock_guard<std::mutex> lock(async_atomics_mutex);
        auto it = async_atomics.find(atomicId);
        if (it != async_atomics.end()) {
            int64_t val = it->second->load();
            std::cout << "[ASYNC-SIM] Atomic load #" << atomicId << " = " << val << std::endl;
            return std::make_shared<Data::Value>(static_cast<int>(val));
        }
        std::cout << "[ASYNC-SIM] Atomic #" << atomicId << " not found, returning 0" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xad\xd9\x85\xd9\x84_\xd8\xb0\xd8\xb1\xd9\x8a", atomic_load_func);
    interpreter.getFunctionManager().registerBuiltinFunction("atomic_load", atomic_load_func);

    // 18. atomic_store
    auto atomic_store_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("atomic_store requires 2 args (atomic_id, value)");
        int64_t atomicId = args[0]->toInt();
        int64_t value = args[1]->toInt();
        std::lock_guard<std::mutex> lock(async_atomics_mutex);
        auto it = async_atomics.find(atomicId);
        if (it == async_atomics.end()) {
            async_atomics[atomicId] = std::make_shared<std::atomic<int64_t>>(value);
        } else {
            it->second->store(value);
        }
        std::cout << "[ASYNC-SIM] Atomic store #" << atomicId << " = " << value << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xae\xd8\xb2\xd9\x86_\xd8\xb0\xd8\xb1\xd9\x8a", atomic_store_func);
    interpreter.getFunctionManager().registerBuiltinFunction("atomic_store", atomic_store_func);

    // 19. atomic_add
    auto atomic_add_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("atomic_add requires 2 args (atomic_id, value)");
        int64_t atomicId = args[0]->toInt();
        int64_t addVal = args[1]->toInt();
        std::lock_guard<std::mutex> lock(async_atomics_mutex);
        auto it = async_atomics.find(atomicId);
        int64_t oldVal = 0;
        if (it == async_atomics.end()) {
            async_atomics[atomicId] = std::make_shared<std::atomic<int64_t>>(addVal);
        } else {
            oldVal = it->second->fetch_add(addVal);
        }
        std::cout << "[ASYNC-SIM] Atomic add #" << atomicId << " += " << addVal << " (old=" << oldVal << ")" << std::endl;
        return std::make_shared<Data::Value>(static_cast<int>(oldVal));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd8\xb6\xd9\x81_\xd8\xb0\xd8\xb1\xd9\x8a", atomic_add_func);
    interpreter.getFunctionManager().registerBuiltinFunction("atomic_add", atomic_add_func);

    // 20. compare_and_swap / CAS
    auto atomic_cas_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) throw std::runtime_error("CAS requires 3 args (atomic_id, expected, desired)");
        int64_t atomicId = args[0]->toInt();
        int64_t expected = args[1]->toInt();
        int64_t desired = args[2]->toInt();
        std::lock_guard<std::mutex> lock(async_atomics_mutex);
        auto it = async_atomics.find(atomicId);
        if (it != async_atomics.end()) {
            int64_t exp = expected;
            bool success = it->second->compare_exchange_strong(exp, desired);
            std::cout << "[ASYNC-SIM] CAS #" << atomicId << " expected=" << expected << " desired=" << desired << " success=" << success << std::endl;
            return std::make_shared<Data::Value>(success ? 1 : 0);
        }
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x82\xd8\xa7\xd8\xb1\xd9\x86_\xd9\x88\xd8\xa8\xd8\xaf\xd9\x84", atomic_cas_func);
    interpreter.getFunctionManager().registerBuiltinFunction("compare_and_swap", atomic_cas_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cas", atomic_cas_func);
    interpreter.getFunctionManager().registerBuiltinFunction("atomic_cas", atomic_cas_func);

    // 21. wait_all
    auto wait_all_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[ASYNC-SIM] Waiting for all " << args.size() << " tasks" << std::endl;
        for (size_t i = 0; i < args.size(); i++) {
            int64_t taskId = args[i]->isInteger() ? args[i]->toInt() : static_cast<int64_t>(args[i]->toDouble());
            std::cout << "[ASYNC-SIM] Task #" << taskId << " completed (simulated)" << std::endl;
        }
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1_\xd8\xa7\xd9\x84\xd9\x83\xd9\x84", wait_all_func);
    interpreter.getFunctionManager().registerBuiltinFunction("wait_all", wait_all_func);

    // 22. wait_any
    auto wait_any_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(-1);
        int64_t taskId = args[0]->toInt();
        std::cout << "[ASYNC-SIM] First task #" << taskId << " completed (wait_any)" << std::endl;
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1_\xd8\xa3\xd9\x8a", wait_any_func);
    interpreter.getFunctionManager().registerBuiltinFunction("wait_any", wait_any_func);

    // 23. select / channel_select
    auto channel_select_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::cout << "[ASYNC-SIM] Select on " << args.size() << " channels" << std::endl;
        if (!args.empty()) {
            int64_t chanId = args[0]->toInt();
            return std::make_shared<Data::Value>(static_cast<int>(chanId));
        }
        return std::make_shared<Data::Value>(0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xae\xd8\xaa\xd8\xb1_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9", channel_select_func);
    interpreter.getFunctionManager().registerBuiltinFunction("select", channel_select_func);
    interpreter.getFunctionManager().registerBuiltinFunction("channel_select", channel_select_func);

}

} // namespace Interpreter
} // namespace Sad
