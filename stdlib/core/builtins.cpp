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

#include "builtins.h"
#include "io/io_functions.h"
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

/**
 * @brief (AR) تقسيم نص إلى مصفوفة من النصوص
 *        (EN) Split string into array of strings
 * 
 * @param args (AR) المعاملات: [النص, الفاصل]
 *             (EN) Arguments: [text, delimiter]
 * @return (AR) مصفوفة من النصوص المقسّمة
 *         (EN) Array of split strings
 * 
 * @example Examples / أمثلة:
 * split("مرحبا بكم", " ") -> ["مرحبا", "بكم"]
 * split("a,b,c", ",") -> ["a", "b", "c"]
 * split("hello", "") -> ["h", "e", "l", "l", "o"]
 */
std::shared_ptr<Data::Value> split(const std::vector<std::shared_ptr<Data::Value>>& args) {
    // (AR) التحقق من عدد المعاملات
    // (EN) Validate argument count
    if (args.size() != 2) {
        throw std::runtime_error(
            "(AR) خطأ: دالة split تحتاج معاملين (النص، الفاصل).\n"
            "(EN) Error: split function requires 2 arguments (text, delimiter)."
        );
    }
    
    // (AR) التحقق من أن المعامل الأول نص
    // (EN) Validate first argument is string
    if (!args[0]->isString()) {
        throw std::runtime_error(
            "(AR) خطأ: المعامل الأول لدالة split يجب أن يكون نصاً.\n"
            "(EN) Error: First argument to split must be a string."
        );
    }
    
    // (AR) التحقق من أن المعامل الثاني نص
    // (EN) Validate second argument is string
    if (!args[1]->isString()) {
        throw std::runtime_error(
            "(AR) خطأ: المعامل الثاني لدالة split يجب أن يكون نصاً.\n"
            "(EN) Error: Second argument to split must be a string."
        );
    }
    
    std::string text = args[0]->toString();
    std::string delimiter = args[1]->toString();
    std::vector<Data::Value> result;
    
    // (AR) حالة خاصة: فاصل فارغ - تقسيم إلى أحرف
    // (EN) Special case: empty delimiter - split into characters
    if (delimiter.empty()) {
        for (char c : text) {
            result.push_back(Data::Value(std::string(1, c)));
        }
        return std::make_shared<Data::Value>(result);
    }
    
    // (AR) تقسيم النص باستخدام الفاصل
    // (EN) Split text using delimiter
    size_t start = 0;
    size_t end = text.find(delimiter);
    
    while (end != std::string::npos) {
        result.push_back(Data::Value(text.substr(start, end - start)));
        start = end + delimiter.length();
        end = text.find(delimiter, start);
    }
    
    // (AR) إضافة الجزء الأخير
    // (EN) Add last part
    result.push_back(Data::Value(text.substr(start)));
    
    return std::make_shared<Data::Value>(result);
}

/**
 * @brief (AR) دمج مصفوفة إلى نص واحد
 *        (EN) Join array elements into a single string
 * 
 * @param args (AR) المعاملات: [المصفوفة, الفاصل]
 *             (EN) Arguments: [array, delimiter]
 * @return (AR) نص مدموج
 *         (EN) Joined string
 * 
 * @example Examples / أمثلة:
 * join(["مرحبا", "بكم"], " ") -> "مرحبا بكم"
 * join(["a", "b", "c"], ",") -> "a,b,c"
 * join([1, 2, 3], "-") -> "1-2-3"
 */
std::shared_ptr<Data::Value> join(const std::vector<std::shared_ptr<Data::Value>>& args) {
    // (AR) التحقق من عدد المعاملات
    // (EN) Validate argument count
    if (args.size() != 2) {
        throw std::runtime_error(
            "(AR) خطأ: دالة join تحتاج معاملين (المصفوفة، الفاصل).\n"
            "(EN) Error: join function requires 2 arguments (array, delimiter)."
        );
    }
    
    // (AR) التحقق من أن المعامل الأول مصفوفة
    // (EN) Validate first argument is array
    if (!args[0]->isArray()) {
        throw std::runtime_error(
            "(AR) خطأ: المعامل الأول لدالة join يجب أن يكون مصفوفة.\n"
            "(EN) Error: First argument to join must be an array."
        );
    }
    
    // (AR) التحقق من أن المعامل الثاني نص
    // (EN) Validate second argument is string
    if (!args[1]->isString()) {
        throw std::runtime_error(
            "(AR) خطأ: المعامل الثاني لدالة join يجب أن يكون نصاً.\n"
            "(EN) Error: Second argument to join must be a string."
        );
    }
    
    const auto& array = args[0]->toArray();
    std::string delimiter = args[1]->toString();
    std::string result;
    
    // (AR) دمج عناصر المصفوفة
    // (EN) Join array elements
    for (size_t i = 0; i < array.size(); ++i) {
        result += array[i].toString();
        
        // (AR) إضافة الفاصل بين العناصر (ليس بعد العنصر الأخير)
        // (EN) Add delimiter between elements (not after last element)
        if (i < array.size() - 1) {
            result += delimiter;
        }
    }
    
    return std::make_shared<Data::Value>(result);
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

/**
 * @brief (AR) إنشاء مصفوفة من الأرقام
 *        (EN) Generate array of numbers
 * 
 * @param args (AR) المعاملات: [start, stop] أو [start, stop, step]
 *             (EN) Arguments: [start, stop] or [start, stop, step]
 *             (AR) أو [stop] فقط (يبدأ من 0)
 *             (EN) Or [stop] only (starts from 0)
 * @return (AR) مصفوفة من الأرقام
 *         (EN) Array of numbers
 * 
 * @example Examples / أمثلة:
 * range(5) -> [0, 1, 2, 3, 4]
 * range(2, 5) -> [2, 3, 4]
 * range(0, 10, 2) -> [0, 2, 4, 6, 8]
 * range(5, 0, -1) -> [5, 4, 3, 2, 1]
 */
std::shared_ptr<Data::Value> range(const std::vector<std::shared_ptr<Data::Value>>& args) {
    // (AR) التحقق من عدد المعاملات
    // (EN) Validate argument count
    if (args.empty() || args.size() > 3) {
        throw std::runtime_error(
            "(AR) خطأ: دالة range تحتاج 1-3 معاملات (stop) أو (start, stop) أو (start, stop, step).\n"
            "(EN) Error: range function requires 1-3 arguments (stop) or (start, stop) or (start, stop, step)."
        );
    }
    
    int start = 0;
    int stop = 0;
    int step = 1;
    
    // (AR) تحليل المعاملات
    // (EN) Parse arguments
    if (args.size() == 1) {
        // range(stop)
        stop = args[0]->toInt();
    } else if (args.size() == 2) {
        // range(start, stop)
        start = args[0]->toInt();
        stop = args[1]->toInt();
    } else {
        // range(start, stop, step)
        start = args[0]->toInt();
        stop = args[1]->toInt();
        step = args[2]->toInt();
        
        // (AR) التحقق من أن step ليس صفراً
        // (EN) Validate step is not zero
        if (step == 0) {
            throw std::runtime_error(
                "(AR) خطأ: قيمة step في range لا يمكن أن تكون صفراً.\n"
                "(EN) Error: step value in range cannot be zero."
            );
        }
    }
    
    std::vector<Data::Value> result;
    
    // (AR) إنشاء المصفوفة
    // (EN) Generate array
    if (step > 0) {
        // (AR) step موجب - من start إلى stop
        // (EN) Positive step - from start to stop
        for (int i = start; i < stop; i += step) {
            result.push_back(Data::Value(i));
        }
    } else {
        // (AR) step سالب - من start إلى stop بالعكس
        // (EN) Negative step - from start to stop in reverse
        for (int i = start; i > stop; i += step) {
            result.push_back(Data::Value(i));
        }
    }
    
    return std::make_shared<Data::Value>(result);
}

} // namespace BuiltinFunctions
} // namespace StdLib
} // namespace Sad
