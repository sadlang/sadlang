/**
 * @file stdlib_manager.h
 * @brief (AR) مدير المكتبات القياسية / (EN) Standard Library Manager
 * 
 * Phase 1-8 Implementation:
 * Central manager for registering and managing all standard library functions.
 * 
 * 8 Phases of Implementation:
 * Phase 1: I/O Functions (5 functions) - print, input, println, readLine, clear
 * Phase 2: String Functions (12 functions) - length, find, replace, substring, etc.
 * Phase 3: Array Functions (10 functions) - append, pop, size, indexOf, etc.
 * Phase 4: Math Functions (12 functions) - abs, min, max, sqrt, sin, cos, etc.
 * Phase 5: Type Functions (10 functions) - type, isNumber, isString, etc.
 * Phase 6: Advanced Features - Lambda, Map, Reduce, Filter, etc.
 * Phase 7: Optimization - Memoization, Lazy Evaluation, Caching
 * Phase 8: Documentation - Examples, API Docs, User Guide
 * 
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "data/types/value.h"
#include "data/managers/function_manager.h"

namespace Sad {
namespace StdLib {

// Forward declarations
class IOFunctions;

/**
 * @class StandardLibraryManager
 * @brief (AR) مدير المكتبة القياسية / (EN) Standard Library Manager
 * 
 * Central point for:
 * - Registering all built-in functions
 * - Managing function initialization
 * - Phase-based implementation tracking
 * - Documentation and examples
 */
class StandardLibraryManager {
public:
    /**
     * @brief (AR) إنشاء مدير المكتبة / (EN) Create library manager
     * 
     * @param funcMgr (AR) مرجع لمدير الدوال / (EN) Reference to function manager
     */
    explicit StandardLibraryManager(Data::FunctionManager& funcMgr);
    
    /**
     * @brief (AR) المُدمر / (EN) Destructor
     */
    ~StandardLibraryManager();
    
    // ========================================
    // (AR) تسجيل الدوال المرحلية
    // (EN) Phase-based Function Registration
    // ========================================
    
    /**
     * @brief (AR) تسجيل دوال المرحلة 1 (I/O Functions)
     * @brief (EN) Register Phase 1 functions (I/O)
     * 
     * Functions: print(), input(), println(), readLine(), clear()
     * 
     * @return (AR) صحيح إذا نجح التسجيل / (EN) True if registration successful
     */
    bool registerPhase1_IOFunctions();
    
    /**
     * @brief (AR) تسجيل دوال المرحلة 2 (String Functions)
     * @brief (EN) Register Phase 2 functions (String)
     * 
     * Status: Not yet implemented
     */
    bool registerPhase2_StringFunctions();
    
    /**
     * @brief (AR) تسجيل دوال المرحلة 3 (Array Functions)
     * @brief (EN) Register Phase 3 functions (Array)
     * 
     * Status: Not yet implemented
     */
    bool registerPhase3_ArrayFunctions();
    
    /**
     * @brief (AR) تسجيل دوال المرحلة 4 (Math Functions)
     * @brief (EN) Register Phase 4 functions (Math)
     * 
     * Status: Not yet implemented
     */
    bool registerPhase4_MathFunctions();
    
    /**
     * @brief (AR) تسجيل دوال المرحلة 5 (Type Functions)
     * @brief (EN) Register Phase 5 functions (Type)
     * 
     * Status: Not yet implemented
     */
    bool registerPhase5_TypeFunctions();
    
    /**
     * @brief (AR) تسجيل جميع دوال المكتبة القياسية
     * @brief (EN) Register all standard library functions
     * 
     * Registers all currently implemented phases.
     * Can be called during interpreter initialization.
     * 
     * @return (AR) صحيح إذا نجح التسجيل / (EN) True if all successful
     */
    bool registerAllFunctions();
    
    /**
     * @brief (AR) الحصول على معلومات الدوال المسجلة
     * @brief (EN) Get information about registered functions
     * 
     * @return (AR) نص يحتوي على معلومات جميع الدوال / (EN) String with all functions info
     */
    std::string getRegistrationStatus() const;

private:
    Data::FunctionManager& functionManager_;    ///< (AR) مرجع مدير الدوال / (EN) Reference to function manager
    
    // Phase tracking
    bool phase1_registered_{false};
    bool phase2_registered_{false};
    bool phase3_registered_{false};
    bool phase4_registered_{false};
    bool phase5_registered_{false};
    
    /**
     * @brief (AR) مساعد داخلي لتسجيل دالة مضمنة
     * @brief (EN) Internal helper to register a built-in function
     * 
     * @param arabicName (AR) الاسم بالعربية / (EN) Arabic name
     * @param englishName (AR) الاسم بالإنجليزية / (EN) English name
     * @param implementation (AR) التنفيذ / (EN) Implementation function
     * @return (AR) صحيح إذا نجح / (EN) True if successful
     */
    bool registerBuiltin(
        const std::string& arabicName,
        const std::string& englishName,
        std::function<Data::Value(const std::vector<Data::Value>&)> implementation
    );
};

} // namespace StdLib
} // namespace Sad

