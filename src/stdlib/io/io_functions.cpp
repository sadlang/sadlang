/**
 * @file io_functions.cpp
 * @brief (AR) تطبيق دوال الإدخال والإخراج / (EN) Implementation of I/O Functions
 * 
 * Phase 1 of Standard Library Implementation:
 * Complete implementation of core I/O operations.
 * 
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 */

#include "stdlib/io/io_functions.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <vector>

namespace Sad {
namespace StdLib {
namespace IO {

// ====================================================================
// Helper Methods
// ====================================================================

std::string IOFunctions::processEscapeSequences(const std::string& input) {
    /**
     * (AR) معالجة تسلسلات الهروب في النصوص / (EN) Process escape sequences in strings
     * Converts \n, \t, \r, \\, \", etc. to their actual characters
     */
    std::string result;
    result.reserve(input.size());
    
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '\\' && i + 1 < input.size()) {
            char next = input[i + 1];
            switch (next) {
                case 'n':  result += '\n'; i++; break;  // Newline
                case 't':  result += '\t'; i++; break;  // Tab
                case 'r':  result += '\r'; i++; break;  // Carriage return
                case '\\': result += '\\'; i++; break;  // Backslash
                case '"':  result += '"';  i++; break;  // Quote
                case '\'': result += '\''; i++; break;  // Single quote
                case '0':  result += '\0'; i++; break;  // Null character
                default:   result += input[i]; break;   // Not an escape, keep backslash
            }
        } else {
            result += input[i];
        }
    }
    
    return result;
}

std::string IOFunctions::valueToString(const Data::Value& value) {
    using VT = Data::ValueType;
    
    switch (value.getType()) {
        case VT::VOID:
            return "(void)";
        
        case VT::INTEGER:
            return std::to_string(value.toInt());
        
        case VT::DOUBLE: {
            std::ostringstream oss;
            // Show enough precision for doubles, remove trailing zeros
            oss << std::fixed << std::setprecision(6) << value.toDouble();
            std::string result = oss.str();
            
            // Remove trailing zeros after decimal point
            if (result.find('.') != std::string::npos) {
                result.erase(result.find_last_not_of('0') + 1, std::string::npos);
                if (result.back() == '.') {
                    result.pop_back();
                }
            }
            return result;
        }
        
        case VT::STRING:
            return processEscapeSequences(value.toString());
        
        case VT::BOOLEAN:
            return value.toBool() ? "صحيح" : "خطأ";  // Arabic: "true" : "false"
        
        case VT::ARRAY: {
            std::ostringstream oss;
            oss << "[";
            try {
                auto arr = value.toArray();
                for (size_t i = 0; i < arr.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << valueToString(arr[i]);
                }
            } catch (...) {
                oss << "...";
            }
            oss << "]";
            return oss.str();
        }
        
        case VT::MAP: {
            std::ostringstream oss;
            oss << "{";
            try {
                auto map = value.toMap();
                bool first = true;
                for (const auto& [key, val] : map) {
                    if (!first) oss << ", ";
                    oss << "\"" << key << "\": " << valueToString(val);
                    first = false;
                }
            } catch (...) {
                oss << "...";
            }
            oss << "}";
            return oss.str();
        }
        
        default:
            return "(unknown)";
    }
}

std::string IOFunctions::formatValue(const Data::Value& value) {
    // For now, formatting is same as toString
    // Can be extended for more complex formatting in future
    return valueToString(value);
}

bool IOFunctions::validateArguments(const std::vector<Data::Value>& args,
                                     size_t minArgs, 
                                     int maxArgs) {
    if (args.size() < minArgs) {
        std::ostringstream error;
        error << "(AR) عدد معاملات غير صحيح: متوقع "
              << minArgs;
        if (maxArgs > 0) {
            error << " إلى " << maxArgs;
        }
        error << " لكن حصلنا على " << args.size()
              << " | (EN) Incorrect argument count: expected "
              << minArgs;
        if (maxArgs > 0) {
            error << " to " << maxArgs;
        }
        error << " but got " << args.size();
        throw std::invalid_argument(error.str());
    }
    
    if (maxArgs > 0 && args.size() > static_cast<size_t>(maxArgs)) {
        std::ostringstream error;
        error << "(AR) عدد معاملات غير صحيح: متوقع أقصى "
              << maxArgs << " لكن حصلنا على " << args.size()
              << " | (EN) Incorrect argument count: expected maximum "
              << maxArgs << " but got " << args.size();
        throw std::invalid_argument(error.str());
    }
    
    return true;
}

// ====================================================================
// Public I/O Functions
// ====================================================================

Data::Value IOFunctions::print(const std::vector<Data::Value>& args) {
    /**
     * (AR) وظيفة طبع () تطبع جميع المعاملات مفصولة بمسافة بدون إضافة سطر جديد
     * (EN) Function print() outputs all arguments space-separated without newline
     */
    
    try {
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) std::cout << " ";
            std::cout << valueToString(args[i]);
        }
        std::cout.flush();
    } catch (const std::exception& e) {
        throw std::runtime_error(
            std::string("(AR) خطأ في طبع() / (EN) Error in print(): ") + e.what()
        );
    }
    
    return Data::Value();  // Return VOID
}

Data::Value IOFunctions::println(const std::vector<Data::Value>& args) {
    /**
     * (AR) وظيفة طبع_سطر () تطبع جميع المعاملات مفصولة بمسافة وتضيف سطر جديد
     * (EN) Function println() outputs all arguments space-separated with newline
     */
    
    try {
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) std::cout << " ";
            std::cout << valueToString(args[i]);
        }
        std::cout << std::endl;
    } catch (const std::exception& e) {
        throw std::runtime_error(
            std::string("(AR) خطأ في طبع_سطر() / (EN) Error in println(): ") + e.what()
        );
    }
    
    return Data::Value();  // Return VOID
}

Data::Value IOFunctions::input(const std::vector<Data::Value>& args) {
    /**
     * (AR) وظيفة أدخل () تقرأ إدخال المستخدم كنص
     * (EN) Function input() reads user input as string
     * 
     * إذا تم توفير معامل، يتم طباعته كرسالة موجهة قبل قراءة الإدخال
     * If an argument is provided, it's displayed as prompt before reading
     */
    
    try {
        // If prompt provided, display it
        if (!args.empty()) {
            std::cout << valueToString(args[0]);
            std::cout.flush();
        }
        
        std::string input_line;
        if (std::getline(std::cin, input_line)) {
            return Data::Value(input_line);
        } else {
            // EOF or error
            return Data::Value("");
        }
    } catch (const std::exception& e) {
        throw std::runtime_error(
            std::string("(AR) خطأ في أدخل() / (EN) Error in input(): ") + e.what()
        );
    }
}

Data::Value IOFunctions::readLine(const std::vector<Data::Value>& args) {
    /**
     * (AR) وظيفة قراءة_سطر () - نفس وظيفة أدخل () بشكل أساسي
     * (EN) Function readLine() - essentially the same as input()
     * 
     * توفر واجهة أكثر وضوحاً للقراءة من المدخلات
     * Provides a clearer interface for reading from input
     */
    
    try {
        // If prompt provided, display it
        if (!args.empty()) {
            std::cout << valueToString(args[0]);
            std::cout.flush();
        }
        
        std::string line;
        if (std::getline(std::cin, line)) {
            return Data::Value(line);
        } else {
            // EOF or error
            return Data::Value("");
        }
    } catch (const std::exception& e) {
        throw std::runtime_error(
            std::string("(AR) خطأ في قراءة_سطر() / (EN) Error in readLine(): ") + e.what()
        );
    }
}

Data::Value IOFunctions::clear(const std::vector<Data::Value>& args) {
    /**
     * (AR) وظيفة مسح_الشاشة () - مسح شاشة الكونسول
     * (EN) Function clear() - clear the console screen
     * 
     * تعمل على معظم الأنظمة (Windows, Linux, macOS)
     * Works on most systems (Windows, Linux, macOS)
     */
    
    try {
        #ifdef _WIN32
            // Windows system call
            std::system("cls");
        #else
            // Unix/Linux/macOS system call
            std::system("clear");
        #endif
        std::cout.flush();
    } catch (const std::exception& e) {
        throw std::runtime_error(
            std::string("(AR) خطأ في مسح_الشاشة() / (EN) Error in clear(): ") + e.what()
        );
    }
    
    return Data::Value();  // Return VOID
}

} // namespace IO
} // namespace StdLib
} // namespace Sad
