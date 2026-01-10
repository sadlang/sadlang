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
     * 
     * Status: TBD - Next phase
     */
    phase2_registered_ = false;
    return false;
}

bool StandardLibraryManager::registerPhase3_ArrayFunctions() {
    /**
     * (AR) تسجيل دوال المرحلة 3: معالجة الصفائف
     * (EN) Register Phase 3: Array Functions
     * 
     * Status: TBD - Future phase
     */
    phase3_registered_ = false;
    return false;
}

bool StandardLibraryManager::registerPhase4_MathFunctions() {
    /**
     * (AR) تسجيل دوال المرحلة 4: دوال الرياضيات
     * (EN) Register Phase 4: Math Functions
     * 
     * Status: TBD - Future phase
     */
    phase4_registered_ = false;
    return false;
}

bool StandardLibraryManager::registerPhase5_TypeFunctions() {
    /**
     * (AR) تسجيل دوال المرحلة 5: فحص النوع
     * (EN) Register Phase 5: Type Functions
     * 
     * Status: TBD - Future phase
     */
    phase5_registered_ = false;
    return false;
}

// ====================================================================
// Public Interface Methods
// ====================================================================

bool StandardLibraryManager::registerAllFunctions() {
    /**
     * (AR) تسجيل جميع دوال المكتبة المتاحة حالياً
     * (EN) Register all currently available library functions
     */
    
    std::cout << "(AR) بدء تسجيل دوال المكتبة القياسية... / "
              << "(EN) Starting Standard Library function registration..." << std::endl;
    
    bool allSuccess = true;
    
    // Register Phase 1 (I/O Functions) - Currently implemented
    if (!registerPhase1_IOFunctions()) {
        std::cerr << "(AR) فشل تسجيل دوال المرحلة 1 / "
                  << "(EN) Failed to register Phase 1 functions" << std::endl;
        allSuccess = false;
    } else {
        std::cout << "(AR) ✓ تم تسجيل دوال المرحلة 1 بنجاح / "
                  << "(EN) ✓ Phase 1 functions registered successfully" << std::endl;
    }
    
    // Phase 2-5 will be registered when implemented
    // registerPhase2_StringFunctions();
    // registerPhase3_ArrayFunctions();
    // registerPhase4_MathFunctions();
    // registerPhase5_TypeFunctions();
    
    if (allSuccess) {
        std::cout << "(AR) ✓ اكتمل تسجيل جميع دوال المكتبة المتاحة / "
                  << "(EN) ✓ All available functions registered successfully" << std::endl;
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
