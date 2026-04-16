/**
 * @file stdlib_manager.cpp
 * @brief (AR) تطبيق مدير المكتبات القياسية / (EN) Implementation of Standard Library Manager
 * 
 * Phase 1-8 Implementation of Standard Library:
 * - Phase 1: I/O Functions (5 functions) - IMPLEMENTED
 * - Phase 2-5: Future phases (TBD)
 * - Phase 6-8: Advanced features (TBD)
 * 
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 */

#include "stdlib_manager.h"
#include "io/io_functions.h"
#include "string/string_functions.h"
#include "array_functions.h"
#include "math/math_functions.h"
#include "type_functions.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <sstream>

namespace Sad {
namespace StdLib {

using namespace Data;

// ====================================================================
// Constructor and Destructor
// ====================================================================

StandardLibraryManager::StandardLibraryManager(Data::FunctionManager& funcMgr)
    : functionManager_(funcMgr) {
    // Initialize tracking
    phase1_registered_ = false;
    phase2_registered_ = false;
    phase3_registered_ = false;
    phase4_registered_ = false;
    phase5_registered_ = false;
}

StandardLibraryManager::~StandardLibraryManager() {
    // Cleanup if needed
}

// ====================================================================
// Phase Registration Methods
// ====================================================================

bool StandardLibraryManager::registerPhase1_IOFunctions() {
    /**
     * (AR) تسجيل دوال المرحلة 1: الإدخال والإخراج
     * (EN) Register Phase 1: Input/Output Functions
     * 
     * Functions:
     * - طبع() / print() - Output without newline
     * - طبع_سطر() / println() - Output with newline
     * - أدخل() / input() - Read user input
     * - قراءة_سطر() / readLine() - Read line from input
     * - مسح_الشاشة() / clear() - Clear console screen
     */
    
    if (phase1_registered_) {
        std::cerr << "(AR) تحذير: دوال المرحلة 1 مسجلة بالفعل / "
                  << "(EN) Warning: Phase 1 functions already registered" << std::endl;
        return true;
    }

    try {
        // Register print() - طبع()
        registerBuiltin(
            "طبع",
            "print",
            [](const std::vector<Value>& args) -> Value {
                return IO::IOFunctions::print(args);
            }
        );

        // Register println() - طبع_سطر()
        registerBuiltin(
            "طبع_سطر",
            "println",
            [](const std::vector<Value>& args) -> Value {
                return IO::IOFunctions::println(args);
            }
        );

        // Register input() - أدخل()
        registerBuiltin(
            "أدخل",
            "input",
            [](const std::vector<Value>& args) -> Value {
                return IO::IOFunctions::input(args);
            }
        );

        // Register readLine() - قراءة_سطر()
        registerBuiltin(
            "قراءة_سطر",
            "readLine",
            [](const std::vector<Value>& args) -> Value {
                return IO::IOFunctions::readLine(args);
            }
        );

        // Register clear() - مسح_الشاشة()
        registerBuiltin(
            "مسح_الشاشة",
            "clear",
            [](const std::vector<Value>& args) -> Value {
                return IO::IOFunctions::clear(args);
            }
        );

        phase1_registered_ = true;
        return true;

    } catch (const std::exception& e) {
        std::cerr << "(AR) خطأ في تسجيل دوال المرحلة 1: " 
                  << "(EN) Error registering Phase 1 functions: "
                  << e.what() << std::endl;
        return false;
    }
}

bool StandardLibraryManager::registerPhase2_StringFunctions() {
    /**
     * (AR) تسجيل دوال المرحلة 2: معالجة النصوص
     * (EN) Register Phase 2: String Functions
     */
    if (phase2_registered_) return true;

    try {
        using namespace StdLib::String;

        registerBuiltin("بحث_نص", "find", [](const std::vector<Data::Value>& args) -> Data::Value {
            return StringFunctions::find(args);
        });
        registerBuiltin("استبدل_نص", "replace", [](const std::vector<Data::Value>& args) -> Data::Value {
            return StringFunctions::replace(args);
        });
        registerBuiltin("استخراج_نص", "substring", [](const std::vector<Data::Value>& args) -> Data::Value {
            return StringFunctions::substring(args);
        });
        registerBuiltin("قص_أطراف", "trim", [](const std::vector<Data::Value>& args) -> Data::Value {
            return StringFunctions::trim(args);
        });
        registerBuiltin("يبدأ_بـ", "startsWith", [](const std::vector<Data::Value>& args) -> Data::Value {
            return StringFunctions::startsWith(args);
        });
        registerBuiltin("ينتهي_بـ", "endsWith", [](const std::vector<Data::Value>& args) -> Data::Value {
            return StringFunctions::endsWith(args);
        });
        registerBuiltin("يحتوي_نص", "contains", [](const std::vector<Data::Value>& args) -> Data::Value {
            return StringFunctions::contains(args);
        });

        phase2_registered_ = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "(AR) خطأ في تسجيل دوال النصوص: " << e.what() << std::endl;
        return false;
    }
}

bool StandardLibraryManager::registerPhase3_ArrayFunctions() {
    /**
     * (AR) تسجيل دوال المرحلة 3: معالجة المصفوفات
     * (EN) Register Phase 3: Array Functions
     */
    if (phase3_registered_) return true;

    try {
        using namespace StdLib::Core;

        registerBuiltin("إضافة_عنصر", "array_append", [](const std::vector<Data::Value>& args) -> Data::Value {
            return ArrayFunctions::append(args);
        });
        registerBuiltin("إزالة_عنصر", "array_pop", [](const std::vector<Data::Value>& args) -> Data::Value {
            return ArrayFunctions::pop(args);
        });
        registerBuiltin("حجم_مصفوفة", "array_size", [](const std::vector<Data::Value>& args) -> Data::Value {
            return ArrayFunctions::size(args);
        });
        registerBuiltin("فهرس_عنصر", "indexOf", [](const std::vector<Data::Value>& args) -> Data::Value {
            return ArrayFunctions::indexOf(args);
        });
        registerBuiltin("قلب_مصفوفة", "reverse", [](const std::vector<Data::Value>& args) -> Data::Value {
            return ArrayFunctions::reverse(args);
        });
        registerBuiltin("فرز_مصفوفة", "sort", [](const std::vector<Data::Value>& args) -> Data::Value {
            return ArrayFunctions::sort(args);
        });
        registerBuiltin("شريحة", "slice", [](const std::vector<Data::Value>& args) -> Data::Value {
            return ArrayFunctions::slice(args);
        });

        phase3_registered_ = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "(AR) خطأ في تسجيل دوال المصفوفات: " << e.what() << std::endl;
        return false;
    }
}

bool StandardLibraryManager::registerPhase4_MathFunctions() {
    /**
     * (AR) تسجيل دوال المرحلة 4: الرياضيات
     * (EN) Register Phase 4: Math Functions
     */
    if (phase4_registered_) return true;

    try {
        using namespace StdLib::Math;

        registerBuiltin("جذر_تربيعي", "sqrt", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::sqrt(args);
        });
        registerBuiltin("أس", "power", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::power(args);
        });
        registerBuiltin("مطلق", "abs", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::abs(args);
        });
        registerBuiltin("تقريب", "round", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::round(args);
        });
        registerBuiltin("أرضية", "floor", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::floor(args);
        });
        registerBuiltin("سقف", "ceil", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::ceil(args);
        });

        // (AR) دوال مثلثية / (EN) Trigonometric functions
        registerBuiltin("جيب", "sin", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::sin(args);
        });
        registerBuiltin("جتا", "cos", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::cos(args);
        });
        registerBuiltin("ظل", "tan", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::tan(args);
        });
        registerBuiltin("معكوس_جيب", "asin", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::asin(args);
        });
        registerBuiltin("معكوس_جتا", "acos", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::acos(args);
        });
        registerBuiltin("معكوس_ظل", "atan", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::atan(args);
        });
        registerBuiltin("معكوس_ظل2", "atan2", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::atan2(args);
        });

        // (AR) دوال لوغاريتمية وأسية / (EN) Logarithmic & exponential
        registerBuiltin("لوغاريتم", "log", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::log(args);
        });
        registerBuiltin("لوغاريتم2", "log2", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::log2(args);
        });
        registerBuiltin("لوغاريتم10", "log10", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::log10(args);
        });
        registerBuiltin("أسي", "exp", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::exp(args);
        });

        // (AR) ثوابت رياضية / (EN) Math constants
        registerBuiltin("باي", "pi", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::pi(args);
        });
        registerBuiltin("هـ", "e", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::e(args);
        });

        // (AR) دوال عشوائية / (EN) Random functions
        registerBuiltin("عشوائي", "random", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::random(args);
        });
        registerBuiltin("عشوائي_صحيح", "randomInt", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::randomInt(args);
        });

        // (AR) دوال إضافية / (EN) Additional math functions
        registerBuiltin("مربع", "square", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::square(args);
        });
        registerBuiltin("أكبر", "max", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::max(args);
        });
        registerBuiltin("أصغر", "min", [](const std::vector<Data::Value>& args) -> Data::Value {
            return MathFunctions::min(args);
        });

        phase4_registered_ = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "(AR) خطأ في تسجيل دوال الرياضيات: " << e.what() << std::endl;
        return false;
    }
}

bool StandardLibraryManager::registerPhase5_TypeFunctions() {
    /**
     * (AR) تسجيل دوال المرحلة 5: فحص وتحويل الأنواع
     * (EN) Register Phase 5: Type Functions
     */
    if (phase5_registered_) return true;

    try {
        using namespace StdLib::Core;

        registerBuiltin("النوع", "type", [](const std::vector<Data::Value>& args) -> Data::Value {
            return TypeFunctions::type(args);
        });
        registerBuiltin("لرقم", "to_int", [](const std::vector<Data::Value>& args) -> Data::Value {
            return TypeFunctions::toInt(args);
        });
        registerBuiltin("لعشري", "to_float", [](const std::vector<Data::Value>& args) -> Data::Value {
            return TypeFunctions::toFloat(args);
        });
        registerBuiltin("لنص", "to_string", [](const std::vector<Data::Value>& args) -> Data::Value {
            return TypeFunctions::toString(args);
        });
        registerBuiltin("لمصفوفة", "to_array", [](const std::vector<Data::Value>& args) -> Data::Value {
            return TypeFunctions::toArray(args);
        });
        registerBuiltin("لمنطقي", "to_bool", [](const std::vector<Data::Value>& args) -> Data::Value {
            return TypeFunctions::toBool(args);
        });
        registerBuiltin("هو_رقم_صحيح", "isInt", [](const std::vector<Data::Value>& args) -> Data::Value {
            return TypeFunctions::isInt(args);
        });
        registerBuiltin("هو_رقم_عشري", "isFloat", [](const std::vector<Data::Value>& args) -> Data::Value {
            return TypeFunctions::isFloat(args);
        });
        registerBuiltin("هو_نص", "isString", [](const std::vector<Data::Value>& args) -> Data::Value {
            return TypeFunctions::isString(args);
        });
        registerBuiltin("هو_مصفوفة", "isArray", [](const std::vector<Data::Value>& args) -> Data::Value {
            return TypeFunctions::isArray(args);
        });

        phase5_registered_ = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "(AR) خطأ في تسجيل دوال الأنواع: " << e.what() << std::endl;
        return false;
    }
}

// ====================================================================
// Public Interface Methods
// ====================================================================

bool StandardLibraryManager::registerAllFunctions() {
    /**
     * (AR) تسجيل جميع دوال المكتبة المتاحة حالياً
     * (EN) Register all currently available library functions
     */
    
    bool allSuccess = true;
    
    // Register Phase 1 (I/O Functions)
    if (!registerPhase1_IOFunctions()) {
        std::cerr << "(AR) فشل تسجيل دوال المرحلة 1 / "
                  << "(EN) Failed to register Phase 1 functions" << std::endl;
        allSuccess = false;
    }
    
    // Register Phase 2 (String Functions)
    if (!registerPhase2_StringFunctions()) {
        std::cerr << "(AR) فشل تسجيل دوال المرحلة 2 / "
                  << "(EN) Failed to register Phase 2 functions" << std::endl;
        allSuccess = false;
    }

    // Register Phase 3 (Array Functions)
    if (!registerPhase3_ArrayFunctions()) {
        std::cerr << "(AR) فشل تسجيل دوال المرحلة 3 / "
                  << "(EN) Failed to register Phase 3 functions" << std::endl;
        allSuccess = false;
    }

    // Register Phase 4 (Math Functions)
    if (!registerPhase4_MathFunctions()) {
        std::cerr << "(AR) فشل تسجيل دوال المرحلة 4 / "
                  << "(EN) Failed to register Phase 4 functions" << std::endl;
        allSuccess = false;
    }

    // Register Phase 5 (Type Functions)
    if (!registerPhase5_TypeFunctions()) {
        std::cerr << "(AR) فشل تسجيل دوال المرحلة 5 / "
                  << "(EN) Failed to register Phase 5 functions" << std::endl;
        allSuccess = false;
    }
    
    return allSuccess;
}

std::string StandardLibraryManager::getRegistrationStatus() const {
    /**
     * (AR) الحصول على حالة التسجيل الحالية
     * (EN) Get current registration status
     */
    
    std::ostringstream oss;
    
    oss << "========================================\n";
    oss << "(AR) حالة تسجيل المكتبة القياسية\n";
    oss << "(EN) Standard Library Registration Status\n";
    oss << "========================================\n\n";
    
    oss << "Phase 1 (I/O Functions):        " << (phase1_registered_ ? "✓ REGISTERED" : "✗ NOT REGISTERED") << "\n";
    oss << "Phase 2 (String Functions):    " << (phase2_registered_ ? "✓ REGISTERED" : "✗ NOT REGISTERED") << "\n";
    oss << "Phase 3 (Array Functions):     " << (phase3_registered_ ? "✓ REGISTERED" : "✗ NOT REGISTERED") << "\n";
    oss << "Phase 4 (Math Functions):      " << (phase4_registered_ ? "✓ REGISTERED" : "✗ NOT REGISTERED") << "\n";
    oss << "Phase 5 (Type Functions):      " << (phase5_registered_ ? "✓ REGISTERED" : "✗ NOT REGISTERED") << "\n";
    
    oss << "\n(AR) الملخص / (EN) Summary:\n";
    int registered = static_cast<int>(phase1_registered_) + 
                     static_cast<int>(phase2_registered_) +
                     static_cast<int>(phase3_registered_) +
                     static_cast<int>(phase4_registered_) +
                     static_cast<int>(phase5_registered_);
    oss << "Phases Registered: " << registered << "/5\n";
    
    return oss.str();
}

// ====================================================================
// Private Helper Methods
// ====================================================================

bool StandardLibraryManager::registerBuiltin(
    const std::string& arabicName,
    const std::string& englishName,
    std::function<Data::Value(const std::vector<Data::Value>&)> implementation) {
    
    /**
     * (AR) تسجيل دالة مضمنة جديدة باسمين (عربي وإنجليزي)
     * (EN) Register new built-in function with two names (Arabic and English)
     */
    
    try {
        // Create function that wraps the Value-based implementation
        std::function<std::shared_ptr<Value>(const std::vector<std::shared_ptr<Value>>&)> wrapper =
            [implementation](const std::vector<std::shared_ptr<Value>>& ptrArgs) -> std::shared_ptr<Value> {
                // Convert shared_ptr<Value> to Value for the implementation
                std::vector<Value> args;
                for (const auto& ptr : ptrArgs) {
                    if (ptr) {
                        args.push_back(*ptr);
                    }
                }
                
                // Call the actual implementation and return wrapped result
                Value result = implementation(args);
                return std::make_shared<Value>(result);
            };
        
        // Register with Arabic name
        functionManager_.registerBuiltinFunction(arabicName, wrapper);
        
        // Register with English name (if different)
        if (englishName != arabicName) {
            functionManager_.registerBuiltinFunction(englishName, wrapper);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "(AR) خطأ في تسجيل الدالة / (EN) Error registering function: "
                  << e.what() << std::endl;
        return false;
    }
}

} // namespace StdLib
} // namespace Sad
