/*
 * ============================================================================
 * Type Errors - تنفيذ نظام إدارة أخطاء الأنواع
 * Type Errors - Type Error Management System Implementation
 * ============================================================================
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 31, 2025
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#include "../include/type_errors.h"
#include <iostream>
#include <sstream>
#include <iomanip>

namespace Sad {
namespace TypeChecker {

// ============================================================================
// رموز الألوان ANSI / ANSI Color Codes
// ============================================================================

// رموز الألوان للطباعة في Terminal / Color codes for terminal printing
namespace Colors {
    const std::string RESET   = "\033[0m";     // إعادة تعيين / Reset
    const std::string RED     = "\033[31m";    // أحمر / Red
    const std::string YELLOW  = "\033[33m";    // أصفر / Yellow
    const std::string BLUE    = "\033[34m";    // أزرق / Blue
    const std::string GREEN   = "\033[32m";    // أخضر / Green
    const std::string CYAN    = "\033[36m";    // سماوي / Cyan
    const std::string BOLD    = "\033[1m";     // عريض / Bold
    const std::string DIM     = "\033[2m";     // خافت / Dim
}

// ============================================================================
// TypeErrorReporter Implementation / تنفيذ TypeErrorReporter
// ============================================================================

/**
 * الحصول على رمز اللون حسب الخطورة / Get color code based on severity
 */
std::string TypeErrorReporter::getColorCode(Severity severity) const {
    // إذا كانت الألوان معطلة، أرجع سلسلة فارغة / If colors disabled, return empty
    if (!useColors_) {
        return "";
    }
    
    // اختيار اللون حسب الخطورة / Choose color based on severity
    switch (severity) {
        case Severity::ERROR:
            return Colors::RED + Colors::BOLD; // أحمر عريض للأخطاء / Bold red for errors
        case Severity::WARNING:
            return Colors::YELLOW + Colors::BOLD; // أصفر عريض للتحذيرات / Bold yellow for warnings
        case Severity::NOTE:
            return Colors::BLUE; // أزرق للملاحظات / Blue for notes
        case Severity::HINT:
            return Colors::GREEN; // أخضر للإشارات / Green for hints
        default:
            return "";
    }
}

/**
 * إعادة تعيين اللون / Reset color
 */
std::string TypeErrorReporter::resetColor() const {
    // إذا كانت الألوان معطلة، أرجع سلسلة فارغة / If colors disabled, return empty
    return useColors_ ? Colors::RESET : "";
}

/**
 * طباعة سطر من الكود / Print a line of code
 */
void TypeErrorReporter::printCodeLine(const Lexer::Position& location) const {
    // التحقق من صحة رقم السطر / Check if line number is valid
    if (location.line < 1 || 
        static_cast<size_t>(location.line) > sourceLines_.size()) {
        return; // خارج النطاق / Out of range
    }
    
    // الحصول على السطر / Get the line
    const std::string& line = sourceLines_[location.line - 1];
    
    // طباعة رقم السطر والسطر نفسه / Print line number and the line
    std::cout << getColorCode(Severity::NOTE) << " " 
              << std::setw(4) << location.line << " | " 
              << resetColor() << line << std::endl;
}

/**
 * طباعة مؤشر للموقع / Print pointer to location
 */
void TypeErrorReporter::printLocationPointer(const Lexer::Position& location) const {
    // التحقق من صحة الموقع / Check if location is valid
    if (location.column < 1) {
        return;
    }
    
    // طباعة مسافات حتى العمود المحدد / Print spaces until the column
    std::cout << "      | "; // محاذاة مع رقم السطر / Align with line number
    
    // طباعة مسافات قبل المؤشر / Print spaces before pointer
    for (int i = 1; i < location.column; ++i) {
        std::cout << " ";
    }
    
    // طباعة المؤشر / Print pointer
    std::cout << getColorCode(Severity::ERROR) << "^" << resetColor() << std::endl;
}

/**
 * طباعة الاقتراحات / Print suggestions
 */
void TypeErrorReporter::printSuggestions(const std::vector<FixSuggestion>& suggestions) const {
    // إذا كانت الاقتراحات معطلة أو لا توجد اقتراحات / If suggestions disabled or none exist
    if (!showSuggestions_ || suggestions.empty()) {
        return;
    }
    
    // طباعة كل اقتراح / Print each suggestion
    for (const auto& suggestion : suggestions) {
        std::cout << getColorCode(Severity::HINT) << "  💡 اقتراح: " 
                  << resetColor() << suggestion.message << std::endl;
        
        // إذا كان هناك كود بديل / If there's replacement code
        if (suggestion.replacement.has_value()) {
            std::cout << "      " << getColorCode(Severity::HINT) << "→ " 
                      << resetColor() << suggestion.replacement.value() << std::endl;
        }
    }
}

/**
 * طباعة خطأ واحد / Print a single error
 */
void TypeErrorReporter::printError(const TypeError& error) const {
    // ========================================================================
    // 1. طباعة عنوان الخطأ / Print error header
    // ========================================================================
    
    // تحديد اسم النوع / Determine type name
    std::string typeName;
    switch (error.severity) {
        case Severity::ERROR:
            typeName = "خطأ"; // Error
            break;
        case Severity::WARNING:
            typeName = "تحذير"; // Warning
            break;
        case Severity::NOTE:
            typeName = "ملاحظة"; // Note
            break;
        case Severity::HINT:
            typeName = "إشارة"; // Hint
            break;
    }
    
    // طباعة العنوان / Print header
    std::cout << getColorCode(error.severity) << typeName << "[" << error.code << "]"
              << resetColor() << ": ";
    
    // طباعة الرسالة بالعربية / Print Arabic message
    std::cout << getColorCode(error.severity) << Colors::BOLD 
              << error.messageAr << resetColor() << std::endl;
    
    // طباعة الرسالة بالإنجليزية (إذا كانت موجودة) / Print English message if exists
    if (!error.messageEn.empty()) {
        std::cout << "  " << Colors::DIM << error.messageEn 
                  << resetColor() << std::endl;
    }
    
    // ========================================================================
    // 2. طباعة موقع الخطأ / Print error location
    // ========================================================================
    
    std::cout << "  " << getColorCode(Severity::NOTE) << "→ " << resetColor();
    
    // طباعة اسم الملف إذا كان موجوداً / Print file name if exists
    if (!currentFile_.empty()) {
        std::cout << currentFile_ << ":";
    }
    
    // طباعة رقم السطر والعمود / Print line and column
    std::cout << error.primaryLocation.line << ":" 
              << error.primaryLocation.column << std::endl;
    
    // ========================================================================
    // 3. طباعة سطر الكود / Print code line
    // ========================================================================
    
    std::cout << std::endl; // سطر فارغ / Empty line
    printCodeLine(error.primaryLocation);
    printLocationPointer(error.primaryLocation);
    std::cout << std::endl; // سطر فارغ / Empty line
    
    // ========================================================================
    // 4. طباعة معلومات الأنواع / Print type information
    // ========================================================================
    
    if (error.expectedType && error.actualType) {
        std::cout << "  " << getColorCode(Severity::NOTE) << "النوع المتوقع: " 
                  << resetColor() << error.expectedType->toString() << std::endl;
        std::cout << "  " << getColorCode(Severity::NOTE) << "النوع الفعلي:  " 
                  << resetColor() << error.actualType->toString() << std::endl;
        std::cout << std::endl; // سطر فارغ / Empty line
    }
    
    // ========================================================================
    // 5. طباعة المواقع الثانوية / Print secondary locations
    // ========================================================================
    
    if (!error.secondaryLocations.empty()) {
        std::cout << "  " << getColorCode(Severity::NOTE) << "مواقع أخرى ذات صلة:" 
                  << resetColor() << std::endl;
        
        for (const auto& loc : error.secondaryLocations) {
            std::cout << "    • ";
            
            // طباعة اسم الملف إذا كان موجوداً / Print file name if exists
            if (!currentFile_.empty()) {
                std::cout << currentFile_ << ":";
            }
            
            std::cout << loc.line << ":" << loc.column << std::endl;
        }
        
        std::cout << std::endl; // سطر فارغ / Empty line
    }
    
    // ========================================================================
    // 6. طباعة الملاحظات / Print notes
    // ========================================================================
    
    if (error.notes.has_value()) {
        std::cout << "  " << getColorCode(Severity::NOTE) << "ℹ️  ملاحظة: " 
                  << resetColor() << error.notes.value() << std::endl;
        std::cout << std::endl; // سطر فارغ / Empty line
    }
    
    // ========================================================================
    // 7. طباعة الاقتراحات / Print suggestions
    // ========================================================================
    
    printSuggestions(error.suggestions);
    
    // خط فاصل / Separator line
    std::cout << std::endl;
    std::cout << "────────────────────────────────────────────────────────" << std::endl;
    std::cout << std::endl;
}

/**
 * طباعة جميع الأخطاء / Print all errors
 */
void TypeErrorReporter::printAllErrors() const {
    // إذا لم تكن هناك أخطاء / If no errors
    if (errors_.empty()) {
        return;
    }
    
    // طباعة عنوان / Print header
    std::cout << std::endl;
    std::cout << getColorCode(Severity::ERROR) << "════════════════════════════════════════════════════════" << resetColor() << std::endl;
    std::cout << getColorCode(Severity::ERROR) << "  الأخطاء المكتشفة / Detected Errors" << resetColor() << std::endl;
    std::cout << getColorCode(Severity::ERROR) << "════════════════════════════════════════════════════════" << resetColor() << std::endl;
    std::cout << std::endl;
    
    // طباعة كل خطأ / Print each error
    for (size_t i = 0; i < errors_.size(); ++i) {
        std::cout << getColorCode(Severity::ERROR) << "خطأ " << (i + 1) << " من " 
                  << errors_.size() << resetColor() << std::endl;
        std::cout << std::endl;
        
        printError(errors_[i]);
    }
}

/**
 * طباعة جميع التحذيرات / Print all warnings
 */
void TypeErrorReporter::printAllWarnings() const {
    // إذا لم تكن هناك تحذيرات / If no warnings
    if (warnings_.empty()) {
        return;
    }
    
    // طباعة عنوان / Print header
    std::cout << std::endl;
    std::cout << getColorCode(Severity::WARNING) << "════════════════════════════════════════════════════════" << resetColor() << std::endl;
    std::cout << getColorCode(Severity::WARNING) << "  التحذيرات / Warnings" << resetColor() << std::endl;
    std::cout << getColorCode(Severity::WARNING) << "════════════════════════════════════════════════════════" << resetColor() << std::endl;
    std::cout << std::endl;
    
    // طباعة كل تحذير / Print each warning
    for (size_t i = 0; i < warnings_.size(); ++i) {
        std::cout << getColorCode(Severity::WARNING) << "تحذير " << (i + 1) << " من " 
                  << warnings_.size() << resetColor() << std::endl;
        std::cout << std::endl;
        
        printError(warnings_[i]);
    }
}

/**
 * طباعة ملخص / Print summary
 */
void TypeErrorReporter::printSummary() const {
    std::cout << std::endl;
    std::cout << "════════════════════════════════════════════════════════" << std::endl;
    std::cout << "  الملخص / Summary" << std::endl;
    std::cout << "════════════════════════════════════════════════════════" << std::endl;
    std::cout << std::endl;
    
    // طباعة عدد الأخطاء / Print error count
    if (errors_.empty()) {
        std::cout << getColorCode(Severity::NOTE) << "✓ لا توجد أخطاء" 
                  << resetColor() << " / No errors" << std::endl;
    } else {
        std::cout << getColorCode(Severity::ERROR) << "✗ " << errors_.size() 
                  << " خطأ" << resetColor();
        
        if (errors_.size() == 1) {
            std::cout << " / 1 error";
        } else {
            std::cout << " / " << errors_.size() << " errors";
        }
        std::cout << std::endl;
    }
    
    // طباعة عدد التحذيرات / Print warning count
    if (warnings_.empty()) {
        std::cout << getColorCode(Severity::NOTE) << "✓ لا توجد تحذيرات" 
                  << resetColor() << " / No warnings" << std::endl;
    } else {
        std::cout << getColorCode(Severity::WARNING) << "⚠ " << warnings_.size() 
                  << " تحذير" << resetColor();
        
        if (warnings_.size() == 1) {
            std::cout << " / 1 warning";
        } else {
            std::cout << " / " << warnings_.size() << " warnings";
        }
        std::cout << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "════════════════════════════════════════════════════════" << std::endl;
    std::cout << std::endl;
}

} // namespace TypeChecker
} // namespace Sad
