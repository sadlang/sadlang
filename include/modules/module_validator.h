/**
 * @file module_validator.h
 * @brief (AR) التحقق من صحة الوحدات - Phase 4
 *        (EN) Module validation and diagnostics - Phase 4
 * 
 * @details
 * يوفر التحقق من:
 * - بنية الوحدة الصحيحة
 * - التبعيات الدائرية
 * - الاستيراد والتصدير
 * - أخطاء التكرار
 * 
 * @author Sad Language Development Team
 * @date December 2025
 */

#pragma once

#include "module.h"
#include "module_loader.h"
#include <vector>
#include <string>
#include <set>
#include <memory>
#include <optional>

namespace Sad {
namespace Modules {

/**
 * نوع التشخيص / Diagnostic Type
 */
enum class DiagnosticSeverity {
    ERROR,      // خطأ / Error
    WARNING,    // تحذير / Warning
    INFO        // معلومة / Info
};

/**
 * تشخيص الوحدة / Module Diagnostic
 */
struct ModuleDiagnostic {
    DiagnosticSeverity severity;
    std::string moduleName;
    std::string message;
    std::string messageAr;      // الرسالة بالعربية
    std::string messageEn;      // الرسالة بالإنجليزية
    int line = -1;              // رقم السطر (إن وُجد)
    int column = -1;            // رقم العمود (إن وُجد)
    
    /**
     * تحويل إلى نص / Convert to string
     */
    std::string toString() const;
    
    /**
     * هل هو خطأ؟ / Is error?
     */
    bool isError() const { return severity == DiagnosticSeverity::ERROR; }
    
    /**
     * هل هو تحذير؟ / Is warning?
     */
    bool isWarning() const { return severity == DiagnosticSeverity::WARNING; }
};

/**
 * نتيجة التحقق / Validation Result
 */
struct ValidationResult {
    bool success = true;
    std::vector<ModuleDiagnostic> diagnostics;
    
    /**
     * إضافة خطأ / Add error
     */
    void addError(const std::string& moduleName, 
                  const std::string& messageEn,
                  const std::string& messageAr = "",
                  int line = -1, int column = -1);
    
    /**
     * إضافة تحذير / Add warning
     */
    void addWarning(const std::string& moduleName,
                    const std::string& messageEn,
                    const std::string& messageAr = "",
                    int line = -1, int column = -1);
    
    /**
     * إضافة معلومة / Add info
     */
    void addInfo(const std::string& moduleName,
                 const std::string& messageEn,
                 const std::string& messageAr = "");
    
    /**
     * هل يوجد أخطاء؟ / Has errors?
     */
    bool hasErrors() const;
    
    /**
     * عدد الأخطاء / Error count
     */
    size_t errorCount() const;
    
    /**
     * عدد التحذيرات / Warning count
     */
    size_t warningCount() const;
    
    /**
     * طباعة التشخيصات / Print diagnostics
     */
    void print() const;
};

/**
 * محقق صحة الوحدات / Module Validator
 * 
 * @details
 * يتحقق من:
 * - Structure validity (بنية صحيحة)
 * - Cyclic dependencies (تبعيات دائرية)
 * - Import/Export consistency (اتساق الاستيراد/التصدير)
 * - Duplicate definitions (تعريفات مكررة)
 * - Missing dependencies (تبعيات مفقودة)
 */
class ModuleValidator {
public:
    ModuleValidator();
    ~ModuleValidator();
    
    /**
     * التحقق من وحدة واحدة
     * Validate single module
     * 
     * @param module الوحدة المراد التحقق منها
     * @return نتيجة التحقق
     */
    ValidationResult validateModule(const Module* module);
    
    /**
     * التحقق من مجموعة وحدات
     * Validate module set
     * 
     * @param modules مجموعة الوحدات
     * @return نتيجة التحقق
     */
    ValidationResult validateModules(const std::vector<const Module*>& modules);
    
    /**
     * كشف التبعيات الدائرية
     * Detect cyclic dependencies
     * 
     * @param module الوحدة المراد الفحص منها
     * @param visited الوحدات المزارة
     * @param stack مكدس المسار الحالي
     * @return مسار الدورة إن وُجد
     */
    std::optional<std::vector<std::string>> detectCycle(
        const Module* module,
        std::set<std::string>& visited,
        std::set<std::string>& stack
    );
    
    /**
     * التحقق من الاستيرادات
     * Validate imports
     * 
     * @param module الوحدة
     * @return نتيجة التحقق
     */
    ValidationResult validateImports(const Module* module);
    
    /**
     * التحقق من الصادرات
     * Validate exports
     * 
     * @param module الوحدة
     * @return نتيجة التحقق
     */
    ValidationResult validateExports(const Module* module);
    
    /**
     * التحقق من عدم وجود تعريفات مكررة
     * Check for duplicate definitions
     * 
     * @param module الوحدة
     * @return نتيجة التحقق
     */
    ValidationResult checkDuplicates(const Module* module);
    
    /**
     * التحقق من بنية AST
     * Validate AST structure
     * 
     * @param module الوحدة
     * @return نتيجة التحقق
     */
    ValidationResult validateAST(const Module* module);
    
    /**
     * التحقق من التبعيات المفقودة
     * Check missing dependencies
     * 
     * @param module الوحدة
     * @param loader محمّل الوحدات
     * @return نتيجة التحقق
     */
    ValidationResult checkMissingDependencies(
        const Module* module,
        ModuleLoader& loader
    );
    
    /**
     * تمكين/تعطيل الفحوصات الصارمة
     * Enable/disable strict checks
     */
    void setStrictMode(bool strict) { strictMode_ = strict; }
    
    /**
     * الحصول على وضع الصرامة
     * Get strict mode
     */
    bool isStrictMode() const { return strictMode_; }
    
private:
    bool strictMode_ = false;
    
    /**
     * فحص تبعية دائرية باستخدام DFS
     * DFS-based cycle detection helper
     */
    bool detectCycleHelper(
        const Module* module,
        std::set<std::string>& visited,
        std::set<std::string>& stack,
        std::vector<std::string>& path
    );
    
    /**
     * التحقق من اسم الوحدة
     * Validate module name
     */
    bool isValidModuleName(const std::string& name) const;
    
    /**
     * التحقق من اسم الرمز
     * Validate symbol name
     */
    bool isValidSymbolName(const std::string& name) const;
};

} // namespace Modules
} // namespace Sad
