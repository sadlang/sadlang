/**
 * @file builtins.h
 * @brief (AR) تعريفات جميع الدوال المضمنة / (EN) All Built-in Functions Definitions
 * 
 * This header file provides unified access to all built-in functions
 * available in the Sad Language standard library.
 * 
 * تم دمج جميع دوال المكتبة القياسية من مختلف الوحدات في ملف واحد
 * للتسهيل على registry والتطبيقات الأخرى.
 * 
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 */

#pragma once

#include "value.h"
#include <vector>
#include <memory>
#include <functional>

namespace Sad {
namespace StdLib {

/**
 * @namespace BuiltinFunctions
 * @brief All built-in functions for the Sad Language
 * 
 * (AR) تجميع جميع الدوال المضمنة المتاحة في لغة ص
 * (EN) Collection of all available built-in functions in Sad Language
 */
namespace BuiltinFunctions {

// =========================================================================
// (AR) دوال الإدخال والإخراج / (EN) I/O Functions (Phase 1)
// =========================================================================

/**
 * @brief (AR) طبع قيمة بدون سطر جديد / (EN) Print without newline
 * 
 * @param args (AR) قائمة القيم / (EN) Values to print
 * @return (AR) قيمة فارغة / (EN) Void value
 */
std::shared_ptr<Data::Value> print(const std::vector<std::shared_ptr<Data::Value>>& args);

/**
 * @brief (AR) طبع قيمة مع سطر جديد / (EN) Print with newline
 * 
 * @param args (AR) قائمة القيم / (EN) Values to print
 * @return (AR) قيمة فارغة / (EN) Void value
 */
std::shared_ptr<Data::Value> println(const std::vector<std::shared_ptr<Data::Value>>& args);

/**
 * @brief (AR) قراءة من المدخلات / (EN) Read from input
 * 
 * @param args (AR) المعاملات / (EN) Arguments
 * @return (AR) النص المقروء / (EN) Input string
 */
std::shared_ptr<Data::Value> input(const std::vector<std::shared_ptr<Data::Value>>& args);

/**
 * @brief (AR) قراءة سطر كامل / (EN) Read full line
 * 
 * @param args (AR) المعاملات / (EN) Arguments
 * @return (AR) النص المقروء / (EN) Input line
 */
std::shared_ptr<Data::Value> readLine(const std::vector<std::shared_ptr<Data::Value>>& args);

/**
 * @brief (AR) مسح الشاشة / (EN) Clear screen
 * 
 * @param args (AR) المعاملات / (EN) Arguments
 * @return (AR) قيمة فارغة / (EN) Void value
 */
std::shared_ptr<Data::Value> clear(const std::vector<std::shared_ptr<Data::Value>>& args);

// =========================================================================
// (AR) دوال المصفوفات والقوائم / (EN) Array/List Functions
// =========================================================================

/**
 * @brief (AR) الحصول على طول المصفوفة / (EN) Get array/string length
 */
std::shared_ptr<Data::Value> length(const std::vector<std::shared_ptr<Data::Value>>& args);

/**
 * @brief (AR) إضافة عنصر للمصفوفة / (EN) Append element to array
 */
std::shared_ptr<Data::Value> append(const std::vector<std::shared_ptr<Data::Value>>& args);

/**
 * @brief (AR) إزالة عنصر من المصفوفة / (EN) Remove element from array
 */
std::shared_ptr<Data::Value> remove(const std::vector<std::shared_ptr<Data::Value>>& args);

// =========================================================================
// (AR) دوال النصوص / (EN) String Functions
// =========================================================================

/**
 * @brief (AR) الحصول على طول النص / (EN) Get string length
 */
std::shared_ptr<Data::Value> str_len(const std::vector<std::shared_ptr<Data::Value>>& args);

/**
 * @brief (AR) تحويل لأحرف كبيرة / (EN) Convert to uppercase
 */
std::shared_ptr<Data::Value> upper(const std::vector<std::shared_ptr<Data::Value>>& args);

/**
 * @brief (AR) تحويل لأحرف صغيرة / (EN) Convert to lowercase
 */
std::shared_ptr<Data::Value> lower(const std::vector<std::shared_ptr<Data::Value>>& args);

/**
 * @brief (AR) تقسيم النص / (EN) Split string
 */
std::shared_ptr<Data::Value> split(const std::vector<std::shared_ptr<Data::Value>>& args);

/**
 * @brief (AR) دمج النصوص / (EN) Join strings
 */
std::shared_ptr<Data::Value> join(const std::vector<std::shared_ptr<Data::Value>>& args);

// =========================================================================
// (AR) الدوال الرياضية / (EN) Math Functions
// =========================================================================

/**
 * @brief (AR) القيمة المطلقة / (EN) Absolute value
 */
std::shared_ptr<Data::Value> abs(const std::vector<std::shared_ptr<Data::Value>>& args);

/**
 * @brief (AR) أكبر قيمة / (EN) Maximum value
 */
std::shared_ptr<Data::Value> max(const std::vector<std::shared_ptr<Data::Value>>& args);

/**
 * @brief (AR) أصغر قيمة / (EN) Minimum value
 */
std::shared_ptr<Data::Value> min(const std::vector<std::shared_ptr<Data::Value>>& args);

/**
 * @brief (AR) مجموع القيم / (EN) Sum of values
 */
std::shared_ptr<Data::Value> sum(const std::vector<std::shared_ptr<Data::Value>>& args);

/**
 * @brief (AR) الجذر التربيعي / (EN) Square root
 */
std::shared_ptr<Data::Value> sqrt(const std::vector<std::shared_ptr<Data::Value>>& args);

// =========================================================================
// (AR) دوال النوع والتحويل / (EN) Type & Conversion Functions
// =========================================================================

/**
 * @brief (AR) الحصول على نوع القيمة / (EN) Get value type
 */
std::shared_ptr<Data::Value> type_of(const std::vector<std::shared_ptr<Data::Value>>& args);

/**
 * @brief (AR) تحويل لعدد صحيح / (EN) Convert to integer
 */
std::shared_ptr<Data::Value> to_int(const std::vector<std::shared_ptr<Data::Value>>& args);

/**
 * @brief (AR) تحويل لعدد عشري / (EN) Convert to float
 */
std::shared_ptr<Data::Value> to_float(const std::vector<std::shared_ptr<Data::Value>>& args);

/**
 * @brief (AR) تحويل لنص / (EN) Convert to string
 */
std::shared_ptr<Data::Value> to_string(const std::vector<std::shared_ptr<Data::Value>>& args);

// =========================================================================
// (AR) دوال المدى والتكرار / (EN) Range & Iteration Functions
// =========================================================================

/**
 * @brief (AR) إنشاء مدى / (EN) Create range
 */
std::shared_ptr<Data::Value> range(const std::vector<std::shared_ptr<Data::Value>>& args);

} // namespace BuiltinFunctions

} // namespace StdLib
} // namespace Sad
