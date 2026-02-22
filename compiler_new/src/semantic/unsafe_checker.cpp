// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file unsafe_checker.cpp
 * @brief تنفيذ فاحص الكود غير الآمن / Unsafe Code Checker Implementation
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 * 
 * =============================================================================
 *                    تفاصيل التنفيذ / Implementation Details
 * =============================================================================
 * 
 *     هذا الملف يُنفِّذ فاحص الكود غير الآمن الذي يتحقق من:
 *     
 *     1. أن العمليات غير الآمنة موجودة داخل كتل `غير_آمن` فقط
 *     2. أن الدوال غير الآمنة موسومة بشكل صحيح
 *     3. أن استدعاءات FFI تتم بشكل آمن
 *     4. تتبع مستوى الكتل غير الآمنة المتداخلة
 */

#include "semantic/unsafe_checker.h"
#include <iostream>
#include <sstream>

namespace Sad {
namespace Semantic {

// ============================================================================
//                    تنفيذ UnsafeError
// ============================================================================

std::string UnsafeError::toArabicString() const {
    std::ostringstream oss;
    oss << "خطأ: " << arabicMessage << "\n";
    oss << "  الموقع: " << operation.location.toString() << "\n";
    if (!suggestion.empty()) {
        oss << "  اقتراح: " << suggestion << "\n";
    }
    return oss.str();
}

std::string UnsafeError::toEnglishString() const {
    std::ostringstream oss;
    oss << "error: " << englishMessage << "\n";
    oss << "  location: " << operation.location.toString() << "\n";
    if (!suggestion.empty()) {
        oss << "  suggestion: " << suggestion << "\n";
    }
    return oss.str();
}

// ============================================================================
//                    البناء والهدم / Construction & Destruction
// ============================================================================

UnsafeChecker::UnsafeChecker() {
    // البدء بدون كتل غير آمنة
    unsafeDepth_ = 0;
    
    if (debugMode_) {
        std::cout << "[فاحص_غير_آمن] تم إنشاء فاحص الكود غير الآمن\n";
    }
}

UnsafeChecker::~UnsafeChecker() = default;

// ============================================================================
//                    إدارة كتل غير_آمن / Unsafe Block Management
// ============================================================================

void UnsafeChecker::enterUnsafeBlock(const SourceLocation& location) {
    unsafeDepth_++;
    unsafeBlockLocations_.push_back(location);
    
    if (debugMode_) {
        std::cout << "[فاحص_غير_آمن] دخول كتلة غير_آمن (العمق: " 
                  << unsafeDepth_ << ") في " << location.toString() << "\n";
    }
}

void UnsafeChecker::exitUnsafeBlock() {
    if (unsafeDepth_ > 0) {
        unsafeDepth_--;
        if (!unsafeBlockLocations_.empty()) {
            unsafeBlockLocations_.pop_back();
        }
        
        if (debugMode_) {
            std::cout << "[فاحص_غير_آمن] خروج من كتلة غير_آمن (العمق: " 
                      << unsafeDepth_ << ")\n";
        }
    } else {
        if (debugMode_) {
            std::cerr << "[فاحص_غير_آمن] تحذير: محاولة الخروج من كتلة غير_آمن "
                      << "بدون دخول مسبق!\n";
        }
    }
}

// ============================================================================
//                    فحص العمليات / Operation Checking
// ============================================================================

bool UnsafeChecker::checkOperation(UnsafeOperationKind kind,
                                   const SourceLocation& location,
                                   const std::string& description) {
    // تسجيل العملية
    UnsafeOperation op;
    op.kind = kind;
    op.description = description;
    op.location = location;
    op.isInsideUnsafeBlock = isInsideUnsafeBlock();
    operations_.push_back(op);
    
    if (debugMode_) {
        std::cout << "[فاحص_غير_آمن] فحص عملية: " << op.getArabicName()
                  << " في " << location.toString();
        if (op.isInsideUnsafeBlock) {
            std::cout << " (داخل كتلة غير_آمن ✓)";
        } else {
            std::cout << " (خارج كتلة غير_آمن ✗)";
        }
        std::cout << "\n";
    }
    
    // إذا كانت العملية خارج كتلة غير_آمن، سجل خطأ
    if (!isInsideUnsafeBlock()) {
        recordError(kind, location, description);
        return false;
    }
    
    return true;
}

bool UnsafeChecker::checkUnsafeFunctionCall(const std::string& functionName,
                                            const SourceLocation& location) {
    // التحقق مما إذا كانت الدالة مسجلة كغير آمنة
    if (!isUnsafeFunction(functionName)) {
        if (debugMode_) {
            std::cout << "[فاحص_غير_آمن] الدالة '" << functionName 
                      << "' ليست غير آمنة - لا حاجة لكتلة غير_آمن\n";
        }
        return true;
    }
    
    if (debugMode_) {
        std::cout << "[فاحص_غير_آمن] استدعاء دالة غير آمنة: " << functionName
                  << " في " << location.toString() << "\n";
    }
    
    return checkOperation(UnsafeOperationKind::UnsafeFnCall, location, functionName);
}

bool UnsafeChecker::checkFFICall(const std::string& externName,
                                 const SourceLocation& location) {
    if (debugMode_) {
        std::cout << "[فاحص_غير_آمن] استدعاء دالة خارجية (FFI): " << externName
                  << " في " << location.toString() << "\n";
    }
    
    return checkOperation(UnsafeOperationKind::FFICall, location, externName);
}

// ============================================================================
//                    تسجيل الدوال / Function Registration
// ============================================================================

void UnsafeChecker::registerUnsafeFunction(const std::string& functionName) {
    unsafeFunctions_.insert(functionName);
    
    if (debugMode_) {
        std::cout << "[فاحص_غير_آمن] تسجيل دالة غير آمنة: " << functionName << "\n";
    }
}

bool UnsafeChecker::isUnsafeFunction(const std::string& functionName) const {
    return unsafeFunctions_.count(functionName) > 0;
}

void UnsafeChecker::registerFFIFunction(const std::string& functionName) {
    ffiFunctions_.insert(functionName);
    
    if (debugMode_) {
        std::cout << "[فاحص_غير_آمن] تسجيل دالة FFI: " << functionName << "\n";
    }
}

bool UnsafeChecker::isFFIFunction(const std::string& functionName) const {
    return ffiFunctions_.count(functionName) > 0;
}

// ============================================================================
//                    تنقيح / Debugging
// ============================================================================

void UnsafeChecker::dump() const {
    std::cout << "\n======= حالة فاحص الكود غير الآمن =======\n";
    std::cout << "عمق كتل غير_آمن: " << unsafeDepth_ << "\n";
    std::cout << "عدد الدوال غير الآمنة: " << unsafeFunctions_.size() << "\n";
    std::cout << "عدد دوال FFI: " << ffiFunctions_.size() << "\n";
    std::cout << "عدد العمليات المسجلة: " << operations_.size() << "\n";
    std::cout << "عدد الأخطاء: " << errors_.size() << "\n\n";
    
    if (!unsafeFunctions_.empty()) {
        std::cout << "الدوال غير الآمنة:\n";
        for (const auto& fn : unsafeFunctions_) {
            std::cout << "  - " << fn << "\n";
        }
    }
    
    if (!ffiFunctions_.empty()) {
        std::cout << "\nدوال FFI:\n";
        for (const auto& fn : ffiFunctions_) {
            std::cout << "  - " << fn << "\n";
        }
    }
    
    if (!errors_.empty()) {
        std::cout << "\nالأخطاء:\n";
        for (const auto& error : errors_) {
            std::cout << "  " << error.toArabicString();
        }
    }
    
    std::cout << "==========================================\n\n";
}

// ============================================================================
//                    دوال مساعدة / Helper Functions
// ============================================================================

void UnsafeChecker::recordError(UnsafeOperationKind kind,
                                const SourceLocation& location,
                                const std::string& description) {
    UnsafeError error;
    error.operation.kind = kind;
    error.operation.location = location;
    error.operation.description = description;
    error.operation.isInsideUnsafeBlock = false;
    
    error.arabicMessage = getArabicErrorMessage(kind);
    error.englishMessage = getEnglishErrorMessage(kind);
    error.suggestion = getSuggestion(kind);
    
    errors_.push_back(error);
    
    if (debugMode_) {
        std::cerr << "[فاحص_غير_آمن] خطأ: " << error.arabicMessage << "\n";
        std::cerr << "  الموقع: " << location.toString() << "\n";
        if (!error.suggestion.empty()) {
            std::cerr << "  اقتراح: " << error.suggestion << "\n";
        }
    }
}

std::string UnsafeChecker::getArabicErrorMessage(UnsafeOperationKind kind) const {
    switch (kind) {
        case UnsafeOperationKind::RawPointerDeref:
            return "إلغاء مرجعية مؤشر خام خارج كتلة غير_آمن";
            
        case UnsafeOperationKind::PointerArithmetic:
            return "حساب عناوين المؤشرات خارج كتلة غير_آمن";
            
        case UnsafeOperationKind::FFICall:
            return "استدعاء دالة خارجية (FFI) خارج كتلة غير_آمن";
            
        case UnsafeOperationKind::Transmute:
            return "تحويل الذاكرة الخام خارج كتلة غير_آمن";
            
        case UnsafeOperationKind::MutableStaticAccess:
            return "الوصول لمتغير ثابت قابل للتغيير خارج كتلة غير_آمن";
            
        case UnsafeOperationKind::UnionFieldAccess:
            return "الوصول لحقل اتحاد خارج كتلة غير_آمن";
            
        case UnsafeOperationKind::UnsafeTraitImpl:
            return "تنفيذ سمة غير آمنة خارج كتلة غير_آمن";
            
        case UnsafeOperationKind::UnsafeFnCall:
            return "استدعاء دالة غير آمنة خارج كتلة غير_آمن";
            
        case UnsafeOperationKind::RawMemoryOp:
            return "عملية ذاكرة خام خارج كتلة غير_آمن";
            
        case UnsafeOperationKind::PointerCast:
            return "تحويل نوع مؤشر خارج كتلة غير_آمن";
            
        default:
            return "عملية غير آمنة خارج كتلة غير_آمن";
    }
}

std::string UnsafeChecker::getEnglishErrorMessage(UnsafeOperationKind kind) const {
    switch (kind) {
        case UnsafeOperationKind::RawPointerDeref:
            return "raw pointer dereference outside unsafe block";
            
        case UnsafeOperationKind::PointerArithmetic:
            return "pointer arithmetic outside unsafe block";
            
        case UnsafeOperationKind::FFICall:
            return "FFI call outside unsafe block";
            
        case UnsafeOperationKind::Transmute:
            return "memory transmute outside unsafe block";
            
        case UnsafeOperationKind::MutableStaticAccess:
            return "mutable static access outside unsafe block";
            
        case UnsafeOperationKind::UnionFieldAccess:
            return "union field access outside unsafe block";
            
        case UnsafeOperationKind::UnsafeTraitImpl:
            return "unsafe trait implementation outside unsafe block";
            
        case UnsafeOperationKind::UnsafeFnCall:
            return "unsafe function call outside unsafe block";
            
        case UnsafeOperationKind::RawMemoryOp:
            return "raw memory operation outside unsafe block";
            
        case UnsafeOperationKind::PointerCast:
            return "pointer type cast outside unsafe block";
            
        default:
            return "unsafe operation outside unsafe block";
    }
}

std::string UnsafeChecker::getSuggestion(UnsafeOperationKind kind) const {
    // جميع الاقتراحات بالعربية عندما يكون وضع العربية مفعلاً
    if (useArabicMessages_) {
        switch (kind) {
            case UnsafeOperationKind::RawPointerDeref:
                return "ضع إلغاء المرجعية داخل كتلة `غير_آمن { ... }`";
                
            case UnsafeOperationKind::PointerArithmetic:
                return "ضع حساب العناوين داخل كتلة `غير_آمن { ... }`";
                
            case UnsafeOperationKind::FFICall:
                return "ضع استدعاء الدالة الخارجية داخل كتلة `غير_آمن { ... }`";
                
            case UnsafeOperationKind::Transmute:
                return "ضع تحويل الذاكرة داخل كتلة `غير_آمن { ... }`";
                
            case UnsafeOperationKind::MutableStaticAccess:
                return "ضع الوصول للمتغير الثابت داخل كتلة `غير_آمن { ... }`";
                
            case UnsafeOperationKind::UnionFieldAccess:
                return "ضع الوصول لحقل الاتحاد داخل كتلة `غير_آمن { ... }`";
                
            case UnsafeOperationKind::UnsafeTraitImpl:
                return "استخدم `غير_آمن تنفيذ` لتنفيذ السمة غير الآمنة";
                
            case UnsafeOperationKind::UnsafeFnCall:
                return "ضع استدعاء الدالة داخل كتلة `غير_آمن { ... }`";
                
            case UnsafeOperationKind::RawMemoryOp:
                return "ضع عملية الذاكرة داخل كتلة `غير_آمن { ... }`";
                
            case UnsafeOperationKind::PointerCast:
                return "ضع تحويل المؤشر داخل كتلة `غير_آمن { ... }`";
                
            default:
                return "ضع العملية داخل كتلة `غير_آمن { ... }`";
        }
    }
    
    // الرسائل بالإنجليزية
    switch (kind) {
        case UnsafeOperationKind::RawPointerDeref:
            return "wrap dereference in `unsafe { ... }` block";
            
        case UnsafeOperationKind::PointerArithmetic:
            return "wrap pointer arithmetic in `unsafe { ... }` block";
            
        case UnsafeOperationKind::FFICall:
            return "wrap FFI call in `unsafe { ... }` block";
            
        case UnsafeOperationKind::Transmute:
            return "wrap transmute in `unsafe { ... }` block";
            
        case UnsafeOperationKind::MutableStaticAccess:
            return "wrap static access in `unsafe { ... }` block";
            
        case UnsafeOperationKind::UnionFieldAccess:
            return "wrap union access in `unsafe { ... }` block";
            
        case UnsafeOperationKind::UnsafeTraitImpl:
            return "use `unsafe impl` for unsafe trait";
            
        case UnsafeOperationKind::UnsafeFnCall:
            return "wrap function call in `unsafe { ... }` block";
            
        case UnsafeOperationKind::RawMemoryOp:
            return "wrap memory operation in `unsafe { ... }` block";
            
        case UnsafeOperationKind::PointerCast:
            return "wrap pointer cast in `unsafe { ... }` block";
            
        default:
            return "wrap operation in `unsafe { ... }` block";
    }
}

} // namespace Semantic
} // namespace Sad
