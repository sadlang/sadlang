/**
 * @file builtins.cpp
 * @brief (AR) تنفيذ الدوال المضمنة / (EN) Implementation of Built-in Functions
 * 
 * This file provides adapters that connect the built-in functions interface
 * with the actual implementations in various stdlib modules.
 * 
 * @author S Language Development Team
 * @date November 22, 2025
 */

#include "../../../include/stdlib/core/builtins.h"
#include "../../../include/stdlib/io/io_functions.h"
#include <iostream>
#include <cmath>
#include <algorithm>

namespace Sad {
namespace StdLib {
namespace BuiltinFunctions {

using namespace IO;

// =========================================================================
// (AR) دوال الإدخال والإخراج / (EN) I/O Functions
// =========================================================================

std::shared_ptr<Data::Value> print(const std::vector<std::shared_ptr<Data::Value>>& args) {
    // Convert from ValuePtr to Value
    std::vector<Data::Value> convertedArgs;
    for (const auto& arg : args) {
        if (arg) convertedArgs.push_back(*arg);
    }
    
    // Call the actual implementation
    Data::Value result = IOFunctions::print(convertedArgs);
    
    // Return as shared_ptr
    return std::make_shared<Data::Value>(result);
}

std::shared_ptr<Data::Value> println(const std::vector<std::shared_ptr<Data::Value>>& args) {
    std::vector<Data::Value> convertedArgs;
    for (const auto& arg : args) {
        if (arg) convertedArgs.push_back(*arg);
    }
    
    Data::Value result = IOFunctions::println(convertedArgs);
    return std::make_shared<Data::Value>(result);
}

std::shared_ptr<Data::Value> input(const std::vector<std::shared_ptr<Data::Value>>& args) {
    std::vector<Data::Value> convertedArgs;
    for (const auto& arg : args) {
        if (arg) convertedArgs.push_back(*arg);
    }
    
    Data::Value result = IOFunctions::input(convertedArgs);
    return std::make_shared<Data::Value>(result);
}

std::shared_ptr<Data::Value> readLine(const std::vector<std::shared_ptr<Data::Value>>& args) {
    std::vector<Data::Value> convertedArgs;
    for (const auto& arg : args) {
        if (arg) convertedArgs.push_back(*arg);
    }
    
    Data::Value result = IOFunctions::readLine(convertedArgs);
    return std::make_shared<Data::Value>(result);
}

std::shared_ptr<Data::Value> clear(const std::vector<std::shared_ptr<Data::Value>>& args) {
    std::vector<Data::Value> convertedArgs;
    for (const auto& arg : args) {
        if (arg) convertedArgs.push_back(*arg);
    }
    
    Data::Value result = IOFunctions::clear(convertedArgs);
    return std::make_shared<Data::Value>(result);
}

// =========================================================================
// (AR) دوال المصفوفات والقوائم / (EN) Array/List Functions
// =========================================================================

std::shared_ptr<Data::Value> length(const std::vector<std::shared_ptr<Data::Value>>& args) {
    if (args.empty()) {
        throw std::runtime_error(
            "(AR) دالة الطول تحتاج معامل واحد على الأقل / "
            "(EN) length() requires at least one argument"
        );
    }
    
    auto& value = args[0];
    if (!value) {
        return std::make_shared<Data::Value>(0);
    }
    
    // Use size() method which works for array, string, and map
    return std::make_shared<Data::Value>((int)value->size());
}

std::shared_ptr<Data::Value> append(const std::vector<std::shared_ptr<Data::Value>>& args) {
    if (args.size() < 2) {
        throw std::runtime_error(
            "(AR) دالة أضف تحتاج معاملين / "
            "(EN) append() requires two arguments"
        );
    }
    
    auto& array = args[0];
    auto& value = args[1];
    
    if (!array || !array->isArray()) {
        throw std::runtime_error(
            "(AR) المعامل الأول يجب أن يكون مصفوفة / "
            "(EN) First argument must be an array"
        );
    }
    
    if (value) {
        array->append(*value);
    } else {
        array->append(Data::Value());
    }
    return std::make_shared<Data::Value>();  // VOID
}

std::shared_ptr<Data::Value> remove(const std::vector<std::shared_ptr<Data::Value>>& args) {
    if (args.size() < 2) {
        throw std::runtime_error(
            "(AR) دالة أزل تحتاج معاملين / "
            "(EN) remove() requires two arguments"
        );
    }
    
    auto& array = args[0];
    auto& index = args[1];
    
    if (!array || !array->isArray()) {
        throw std::runtime_error(
            "(AR) المعامل الأول يجب أن يكون مصفوفة / "
            "(EN) First argument must be an array"
        );
    }
    
    if (!index || !index->isInteger()) {
        throw std::runtime_error(
            "(AR) المعامل الثاني يجب أن يكون رقماً / "
            "(EN) Second argument must be a number"
        );
    }
    
    int idx = index->toInt();
    
    if (idx >= 0 && idx < (int)array->size()) {
        // Need to get internal array - use operator[]
        // For now, return void
    }
    
    return std::make_shared<Data::Value>();  // VOID
}

// =========================================================================
// (AR) دوال النصوص / (EN) String Functions
// =========================================================================

std::shared_ptr<Data::Value> str_len(const std::vector<std::shared_ptr<Data::Value>>& args) {
    if (args.empty() || !args[0]) {
        return std::make_shared<Data::Value>(0);
    }
    
    return std::make_shared<Data::Value>((int)args[0]->toString().length());
}

std::shared_ptr<Data::Value> upper(const std::vector<std::shared_ptr<Data::Value>>& args) {
    if (args.empty() || !args[0]) {
        return std::make_shared<Data::Value>(std::string(""));
    }
    
    std::string str = args[0]->toString();
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return std::make_shared<Data::Value>(str);
}

std::shared_ptr<Data::Value> lower(const std::vector<std::shared_ptr<Data::Value>>& args) {
    if (args.empty() || !args[0]) {
        return std::make_shared<Data::Value>(std::string(""));
    }
    
    std::string str = args[0]->toString();
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return std::make_shared<Data::Value>(str);
}

std::shared_ptr<Data::Value> split(const std::vector<std::shared_ptr<Data::Value>>& args) {
    // TODO: Implement split functionality
    return std::make_shared<Data::Value>();
}

std::shared_ptr<Data::Value> join(const std::vector<std::shared_ptr<Data::Value>>& args) {
    // TODO: Implement join functionality
    return std::make_shared<Data::Value>();
}

// =========================================================================
// (AR) الدوال الرياضية / (EN) Math Functions
// =========================================================================

std::shared_ptr<Data::Value> abs(const std::vector<std::shared_ptr<Data::Value>>& args) {
    if (args.empty() || !args[0]) {
        throw std::runtime_error(
            "(AR) دالة القيمة المطلقة تحتاج معامل واحد / "
            "(EN) abs() requires one argument"
        );
    }
    
    if (args[0]->isInteger()) {
        return std::make_shared<Data::Value>(std::abs(args[0]->toInt()));
    }
    
    if (args[0]->isDouble()) {
        return std::make_shared<Data::Value>(std::abs(args[0]->toDouble()));
    }
    
    throw std::runtime_error(
        "(AR) المعامل يجب أن يكون رقماً / "
        "(EN) Argument must be a number"
    );
}

std::shared_ptr<Data::Value> max(const std::vector<std::shared_ptr<Data::Value>>& args) {
    if (args.size() < 2) {
        throw std::runtime_error(
            "(AR) دالة أكبر تحتاج معاملين على الأقل / "
            "(EN) max() requires at least two arguments"
        );
    }
    
    Data::Value result = *args[0];
    
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i]) {
            if (args[i]->isInteger() && result.isInteger()) {
                if (args[i]->toInt() > result.toInt()) {
                    result = *args[i];
                }
            }
        }
    }
    
    return std::make_shared<Data::Value>(result);
}

std::shared_ptr<Data::Value> min(const std::vector<std::shared_ptr<Data::Value>>& args) {
    if (args.size() < 2) {
        throw std::runtime_error(
            "(AR) دالة أصغر تحتاج معاملين على الأقل / "
            "(EN) min() requires at least two arguments"
        );
    }
    
    Data::Value result = *args[0];
    
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i]) {
            if (args[i]->isInteger() && result.isInteger()) {
                if (args[i]->toInt() < result.toInt()) {
                    result = *args[i];
                }
            }
        }
    }
    
    return std::make_shared<Data::Value>(result);
}

std::shared_ptr<Data::Value> sum(const std::vector<std::shared_ptr<Data::Value>>& args) {
    int total = 0;
    
    for (const auto& arg : args) {
        if (arg && arg->isInteger()) {
            total += arg->toInt();
        }
    }
    
    return std::make_shared<Data::Value>(total);
}

std::shared_ptr<Data::Value> sqrt(const std::vector<std::shared_ptr<Data::Value>>& args) {
    if (args.empty() || !args[0]) {
        throw std::runtime_error(
            "(AR) دالة الجذر تحتاج معامل واحد / "
            "(EN) sqrt() requires one argument"
        );
    }
    
    double value = 0;
    if (args[0]->isInteger()) {
        value = args[0]->toInt();
    } else if (args[0]->isDouble()) {
        value = args[0]->toDouble();
    }
    
    return std::make_shared<Data::Value>(std::sqrt(value));
}

// =========================================================================
// (AR) دوال النوع والتحويل / (EN) Type & Conversion Functions
// =========================================================================

std::shared_ptr<Data::Value> type_of(const std::vector<std::shared_ptr<Data::Value>>& args) {
    if (args.empty() || !args[0]) {
        return std::make_shared<Data::Value>(std::string("void"));
    }
    
    switch (args[0]->getType()) {
        case Data::ValueType::INTEGER:
            return std::make_shared<Data::Value>(std::string("integer"));
        case Data::ValueType::DOUBLE:
            return std::make_shared<Data::Value>(std::string("double"));
        case Data::ValueType::STRING:
            return std::make_shared<Data::Value>(std::string("string"));
        case Data::ValueType::BOOLEAN:
            return std::make_shared<Data::Value>(std::string("boolean"));
        case Data::ValueType::ARRAY:
            return std::make_shared<Data::Value>(std::string("array"));
        case Data::ValueType::MAP:
            return std::make_shared<Data::Value>(std::string("map"));
        default:
            return std::make_shared<Data::Value>(std::string("unknown"));
    }
}

std::shared_ptr<Data::Value> to_int(const std::vector<std::shared_ptr<Data::Value>>& args) {
    if (args.empty() || !args[0]) {
        return std::make_shared<Data::Value>(0);
    }
    
    if (args[0]->isInteger()) {
        return std::make_shared<Data::Value>(args[0]->toInt());
    }
    
    if (args[0]->isDouble()) {
        return std::make_shared<Data::Value>((int)args[0]->toDouble());
    }
    
    if (args[0]->isString()) {
        try {
            return std::make_shared<Data::Value>(std::stoi(args[0]->toString()));
        } catch (...) {
            return std::make_shared<Data::Value>(0);
        }
    }
    
    return std::make_shared<Data::Value>(0);
}

std::shared_ptr<Data::Value> to_float(const std::vector<std::shared_ptr<Data::Value>>& args) {
    if (args.empty() || !args[0]) {
        return std::make_shared<Data::Value>(0.0);
    }
    
    if (args[0]->isDouble()) {
        return std::make_shared<Data::Value>(args[0]->toDouble());
    }
    
    if (args[0]->isInteger()) {
        return std::make_shared<Data::Value>((double)args[0]->toInt());
    }
    
    if (args[0]->isString()) {
        try {
            return std::make_shared<Data::Value>(std::stod(args[0]->toString()));
        } catch (...) {
            return std::make_shared<Data::Value>(0.0);
        }
    }
    
    return std::make_shared<Data::Value>(0.0);
}

std::shared_ptr<Data::Value> to_string(const std::vector<std::shared_ptr<Data::Value>>& args) {
    if (args.empty() || !args[0]) {
        return std::make_shared<Data::Value>(std::string(""));
    }
    
    return std::make_shared<Data::Value>(args[0]->toString());
}

// =========================================================================
// (AR) دوال المدى والتكرار / (EN) Range & Iteration Functions
// =========================================================================

std::shared_ptr<Data::Value> range(const std::vector<std::shared_ptr<Data::Value>>& args) {
    // TODO: Implement range functionality
    return std::make_shared<Data::Value>();
}

} // namespace BuiltinFunctions
} // namespace StdLib
} // namespace Sad
