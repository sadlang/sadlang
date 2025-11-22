/**
 * @file type_functions.cpp
 * @brief (AR) تنفيذ دوال الأنواع والتحويلات / (EN) Implementation of Type Conversion and Checking Functions
 * 
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 */

#include "stdlib/core/type_functions.h"
#include <stdexcept>
#include <sstream>
#include <cmath>

namespace Sad {
namespace StdLib {
namespace Core {

// ============================================================================
// (AR) دوال مساعدة خاصة / (EN) Private Helper Functions
// ============================================================================

void TypeFunctions::validateArgCount(const std::vector<Data::Value>& args,
                                     size_t expected,
                                     const std::string& funcName) {
    if (args.size() != expected) {
        throw std::runtime_error(funcName + " expects " + 
                               std::to_string(expected) + " argument(s), got " + 
                               std::to_string(args.size()));
    }
}

// ============================================================================
// (AR) دوال التحويل / (EN) Conversion Functions
// ============================================================================

/**
 * @brief (AR) تحويل أي قيمة إلى نص
 * @brief (EN) Convert any value to string
 */
Data::Value TypeFunctions::toString(const std::vector<Data::Value>& args) {
    validateArgCount(args, 1, "toString");
    
    const Data::Value& value = args[0];
    
    // استخدام دالة toString المدمجة في Value
    // Use built-in toString method in Value
    return Data::Value(value.toString());
}

/**
 * @brief (AR) تحويل قيمة إلى رقم صحيح
 * @brief (EN) Convert value to integer
 */
Data::Value TypeFunctions::toInt(const std::vector<Data::Value>& args) {
    validateArgCount(args, 1, "toInt");
    
    const Data::Value& value = args[0];
    
    try {
        // التحويل المباشر باستخدام toInt المدمج
        // Direct conversion using built-in toInt
        int result = value.toInt();
        return Data::Value(result);
    }
    catch (const std::exception&) {
        // في حالة الفشل، نرجع 0
        // On failure, return 0
        return Data::Value(0);
    }
}

/**
 * @brief (AR) تحويل قيمة إلى رقم عشري
 * @brief (EN) Convert value to floating-point number
 */
Data::Value TypeFunctions::toFloat(const std::vector<Data::Value>& args) {
    validateArgCount(args, 1, "toFloat");
    
    const Data::Value& value = args[0];
    
    try {
        // التحويل المباشر باستخدام toDouble المدمج
        // Direct conversion using built-in toDouble
        double result = value.toDouble();
        return Data::Value(result);
    }
    catch (const std::exception&) {
        // في حالة الفشل، نرجع 0.0
        // On failure, return 0.0
        return Data::Value(0.0);
    }
}

/**
 * @brief (AR) تحويل قيمة إلى مصفوفة
 * @brief (EN) Convert value to array
 */
Data::Value TypeFunctions::toArray(const std::vector<Data::Value>& args) {
    validateArgCount(args, 1, "toArray");
    
    const Data::Value& value = args[0];
    Data::Value::ArrayType result;
    
    // إذا كانت القيمة مصفوفة بالفعل، نرجعها كما هي
    // If already an array, return as-is
    if (value.isArray()) {
        return value;
    }
    
    // إذا كانت نصاً، نحوله إلى مصفوفة من الأحرف
    // If string, convert to array of characters
    if (value.isString()) {
        std::string str = value.toString();
        for (char c : str) {
            result.push_back(Data::Value(std::string(1, c)));
        }
        return Data::Value(result);
    }
    
    // لأي نوع آخر، نضعه في مصفوفة بعنصر واحد
    // For any other type, wrap in single-element array
    result.push_back(value);
    return Data::Value(result);
}

/**
 * @brief (AR) تحويل قيمة إلى قيمة منطقية
 * @brief (EN) Convert value to boolean
 */
Data::Value TypeFunctions::toBool(const std::vector<Data::Value>& args) {
    validateArgCount(args, 1, "toBool");
    
    const Data::Value& value = args[0];
    
    try {
        // استخدام دالة toBool المدمجة
        // Use built-in toBool method
        bool result = value.toBool();
        return Data::Value(result);
    }
    catch (const std::exception&) {
        // في حالة الفشل، نرجع false
        // On failure, return false
        return Data::Value(false);
    }
}

// ============================================================================
// (AR) دوال فحص الأنواع / (EN) Type Checking Functions
// ============================================================================

/**
 * @brief (AR) فحص ما إذا كانت القيمة رقماً صحيحاً
 * @brief (EN) Check if value is an integer
 */
Data::Value TypeFunctions::isInt(const std::vector<Data::Value>& args) {
    validateArgCount(args, 1, "isInt");
    
    const Data::Value& value = args[0];
    
    // فحص النوع باستخدام isInteger المدمج
    // Check type using built-in isInteger
    return Data::Value(value.isInteger());
}

/**
 * @brief (AR) فحص ما إذا كانت القيمة رقماً عشرياً
 * @brief (EN) Check if value is a floating-point number
 */
Data::Value TypeFunctions::isFloat(const std::vector<Data::Value>& args) {
    validateArgCount(args, 1, "isFloat");
    
    const Data::Value& value = args[0];
    
    // فحص النوع باستخدام isDouble المدمج
    // Check type using built-in isDouble
    return Data::Value(value.isDouble());
}

/**
 * @brief (AR) فحص ما إذا كانت القيمة نصاً
 * @brief (EN) Check if value is a string
 */
Data::Value TypeFunctions::isString(const std::vector<Data::Value>& args) {
    validateArgCount(args, 1, "isString");
    
    const Data::Value& value = args[0];
    
    // فحص النوع باستخدام isString المدمج
    // Check type using built-in isString
    return Data::Value(value.isString());
}

/**
 * @brief (AR) فحص ما إذا كانت القيمة مصفوفة
 * @brief (EN) Check if value is an array
 */
Data::Value TypeFunctions::isArray(const std::vector<Data::Value>& args) {
    validateArgCount(args, 1, "isArray");
    
    const Data::Value& value = args[0];
    
    // فحص النوع باستخدام isArray المدمج
    // Check type using built-in isArray
    return Data::Value(value.isArray());
}

// ============================================================================
// (AR) دالة معلومات النوع / (EN) Type Information Function
// ============================================================================

/**
 * @brief (AR) الحصول على اسم نوع القيمة
 * @brief (EN) Get the type name of a value
 */
Data::Value TypeFunctions::type(const std::vector<Data::Value>& args) {
    validateArgCount(args, 1, "type");
    
    const Data::Value& value = args[0];
    
    // الحصول على اسم النوع باستخدام getTypeName المدمج
    // Get type name using built-in getTypeName
    std::string typeName = value.getTypeName();
    
    // إضافة ترجمة عربية للنوع
    // Add Arabic translation for type
    if (typeName == "INTEGER") {
        return Data::Value("integer");  // يمكن أن يكون "رقم_صحيح" أيضاً
    } else if (typeName == "DOUBLE") {
        return Data::Value("double");   // يمكن أن يكون "رقم_عشري" أيضاً
    } else if (typeName == "STRING") {
        return Data::Value("string");   // يمكن أن يكون "نص" أيضاً
    } else if (typeName == "BOOLEAN") {
        return Data::Value("boolean");  // يمكن أن يكون "منطقي" أيضاً
    } else if (typeName == "ARRAY") {
        return Data::Value("array");    // يمكن أن يكون "مصفوفة" أيضاً
    } else if (typeName == "VOID") {
        return Data::Value("void");     // يمكن أن يكون "فراغ" أيضاً
    } else if (typeName == "MAP") {
        return Data::Value("map");      // يمكن أن يكون "خريطة" أيضاً
    }
    
    // نوع غير معروف
    // Unknown type
    return Data::Value("unknown");
}

} // namespace Core
} // namespace StdLib
} // namespace Sad
