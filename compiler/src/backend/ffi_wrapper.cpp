// تعطيل تحذير Unicode للنصوص العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file ffi_wrapper.cpp
 * @brief (AR) تنفيذ مولد أغلفة FFI الآمنة
 *        (EN) Implementation of Safe FFI Wrapper Generator
 * 
 * @details
 * (AR) هذا الملف يحتوي على التنفيذ الكامل لنظام الأغلفة الآمنة.
 *      يتضمن:
 *      - توليد كود الأغلفة
 *      - تتبع الملكية
 *      - كشف التسرب
 * 
 * (EN) This file contains the complete implementation of the safe wrapper system.
 *      Includes:
 *      - Wrapper code generation
 *      - Ownership tracking
 *      - Leak detection
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2025-01-18
 * @version 1.0.0
 */

#include "ffi/ffi_wrapper.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <regex>
#include <algorithm>

namespace Sad {
namespace FFI {

// ============================================================================
// (AR) الكلمات العربية للتوليد
// (EN) Arabic keywords for generation
// ============================================================================

namespace ArabicWrapperKeywords {
    const char* FUNCTION = "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9";                      // دالة
    const char* END = "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9";                   // نهاية
    const char* IF = "\xD8\xA5\xD8\xB0\xD8\xA7";                                     // إذا
    const char* RETURN = "\xD8\xA5\xD8\xB1\xD8\xAC\xD8\xA7\xD8\xB9";                // إرجاع
    const char* CONST = "\xD8\xAB\xD8\xA7\xD8\xA8\xD8\xAA";                          // ثابت
    const char* NULL_VAL = "\xD9\x81\xD8\xA7\xD8\xB1\xD8\xBA";                       // فارغ
    const char* SUCCESS = "\xD9\x86\xD8\xAC\xD8\xA7\xD8\xAD";                        // نجاح
    const char* ERROR = "\xD8\xAE\xD8\xB7\xD8\xA3";                                  // خطأ
    const char* RESULT = "\xD9\x86\xD8\xAA\xD9\x8A\xD8\xAC\xD8\xA9";                // نتيجة
    const char* SAFE_WRAPPER = "\xD8\xBA\xD9\x84\xD8\xA7\xD9\x81_\xD8\xA2\xD9\x85\xD9\x86"; // غلاف_آمن
    const char* ALLOC_FAILED = "\xD9\x81\xD8\xB4\xD9\x84 \xD8\xAD\xD8\xAC\xD8\xB2 "
        "\xD8\xA7\xD9\x84\xD8\xB0\xD8\xA7\xD9\x83\xD8\xB1\xD8\xA9";                 // فشل حجز الذاكرة
    const char* OPEN_FAILED = "\xD9\x81\xD8\xB4\xD9\x84 \xD9\x81\xD8\xAA\xD8\xAD "
        "\xD8\xA7\xD9\x84\xD9\x85\xD9\x88\xD8\xB1\xD8\xAF";                         // فشل فتح المورد
    const char* SAFE_SUFFIX = "_\xD8\xA2\xD9\x85\xD9\x86";                           // _آمن
}

// ============================================================================
//                    (AR) تنفيذ WrapperGenerator
//                    (EN) WrapperGenerator Implementation
// ============================================================================

WrapperGenerator::WrapperGenerator()
    : config_() {
}

WrapperGenerator::WrapperGenerator(const Config& config)
    : config_(config) {
    /*
     * (AR) تهيئة مولد الأغلفة مع الإعدادات
     * (EN) Initialize wrapper generator with configuration
     */
}

/**
 * @brief (AR) توليد غلاف لدالة تحجز ذاكرة
 *        (EN) Generate wrapper for memory-allocating function
 * 
 * @details
 * (AR) خوارزمية التوليد:
 *      1. إنشاء تصريح الدالة الجديدة
 *      2. استدعاء الدالة الأصلية
 *      3. فحص null
 *      4. لف المؤشر في غلاف_آمن
 *      5. إرجاع نتيجة<غلاف_آمن<T>>
 * 
 * (EN) Generation algorithm:
 *      1. Create new function declaration
 *      2. Call original function
 *      3. Check for null
 *      4. Wrap pointer in غلاف_آمن
 *      5. Return نتيجة<غلاف_آمن<T>>
 */
std::string WrapperGenerator::generateAllocatorWrapper(const ExternFunctionDecl& funcDecl,
                                                       const std::string& destructor) {
    std::ostringstream output;
    
    // (AR) التوثيق
    if (config_.generateDocumentation) {
        output << generateDocumentation(funcDecl);
    }
    
    // (AR) اسم الغلاف
    std::string wrapperName = funcDecl.getName();
    if (config_.generateArabicNames) {
        wrapperName = convertToArabicName(wrapperName);
    }
    wrapperName += ArabicWrapperKeywords::SAFE_SUFFIX;
    
    // (AR) نوع الإرجاع
    std::string returnType = funcDecl.getReturnType()->getCName();
    std::string safeReturnType;
    
    if (config_.useResultType) {
        safeReturnType = std::string(ArabicWrapperKeywords::RESULT) + 
                        "<" + ArabicWrapperKeywords::SAFE_WRAPPER + 
                        "<" + returnType + ">>";
    } else {
        safeReturnType = std::string(ArabicWrapperKeywords::SAFE_WRAPPER) + 
                        "<" + returnType + ">";
    }
    
    // (AR) تصريح الدالة
    output << ArabicWrapperKeywords::FUNCTION << " " << wrapperName << "(";
    
    // (AR) المعاملات
    const auto& params = funcDecl.getParameters();
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) output << ", ";
        output << params[i].name << ": " << params[i].type->getCName();
    }
    
    output << ") -> " << safeReturnType << "\n";
    
    // (AR) جسم الدالة
    output << "    " << ArabicWrapperKeywords::CONST << " ";
    output << "\xD9\x85\xD8\xA4\xD8\xB4\xD8\xB1 = ";  // مؤشر =
    output << funcDecl.getName() << "(";
    
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) output << ", ";
        output << params[i].name;
    }
    
    output << ")\n";
    
    // (AR) فحص null
    if (config_.generateNullChecks) {
        output << generateNullCheck("\xD9\x85\xD8\xA4\xD8\xB4\xD8\xB1");
    }
    
    // (AR) إنشاء الغلاف الآمن
    output << "    " << ArabicWrapperKeywords::CONST << " ";
    output << "\xD8\xBA\xD9\x84\xD8\xA7\xD9\x81 = ";  // غلاف =
    output << generateRAIIWrapper("\xD9\x85\xD8\xA4\xD8\xB4\xD8\xB1", destructor);
    output << "\n";
    
    // (AR) الإرجاع
    if (config_.useResultType) {
        output << generateSuccessReturn("\xD8\xBA\xD9\x84\xD8\xA7\xD9\x81");
    } else {
        output << "    " << ArabicWrapperKeywords::RETURN << " \xD8\xBA\xD9\x84\xD8\xA7\xD9\x81\n";
    }
    
    output << ArabicWrapperKeywords::END << "\n\n";
    
    return output.str();
}

/**
 * @brief (AR) توليد غلاف لدالة تفتح مورد
 *        (EN) Generate wrapper for resource-opening function
 */
std::string WrapperGenerator::generateResourceWrapper(const ExternFunctionDecl& funcDecl,
                                                      const std::string& closer) {
    std::ostringstream output;
    
    // (AR) التوثيق
    if (config_.generateDocumentation) {
        output << "/// " << ArabicWrapperKeywords::SAFE_WRAPPER;
        output << " \xD9\x84\xD9\x80 " << funcDecl.getName() << "\n";
        output << "/// \xD9\x8A\xD8\xBA\xD9\x84\xD9\x82 \xD8\xA7\xD9\x84\xD9\x85\xD9\x88\xD8\xB1\xD8\xAF "
                  "\xD8\xAA\xD9\x84\xD9\x82\xD8\xA7\xD8\xA6\xD9\x8A\xD8\xA7\xD9\x8B "
                  "\xD8\xA8\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xAE\xD8\xAF\xD8\xA7\xD9\x85 "
               << closer << "\n";
    }
    
    // (AR) اسم الغلاف
    std::string wrapperName = funcDecl.getName();
    if (config_.generateArabicNames) {
        wrapperName = convertToArabicName(wrapperName);
    }
    wrapperName += ArabicWrapperKeywords::SAFE_SUFFIX;
    
    // (AR) نوع الإرجاع
    std::string returnType = funcDecl.getReturnType()->getCName();
    std::string safeReturnType;
    
    if (config_.useResultType) {
        safeReturnType = std::string(ArabicWrapperKeywords::RESULT) + 
                        "<" + ArabicWrapperKeywords::SAFE_WRAPPER + 
                        "<" + returnType + ">>";
    } else {
        safeReturnType = std::string(ArabicWrapperKeywords::SAFE_WRAPPER) + 
                        "<" + returnType + ">";
    }
    
    // (AR) تصريح الدالة
    output << ArabicWrapperKeywords::FUNCTION << " " << wrapperName << "(";
    
    const auto& params = funcDecl.getParameters();
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) output << ", ";
        output << params[i].name << ": " << params[i].type->getCName();
    }
    
    output << ") -> " << safeReturnType << "\n";
    
    // (AR) جسم الدالة
    output << "    " << ArabicWrapperKeywords::CONST << " ";
    output << "\xD9\x85\xD9\x88\xD8\xB1\xD8\xAF = ";  // مورد =
    output << funcDecl.getName() << "(";
    
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) output << ", ";
        output << params[i].name;
    }
    
    output << ")\n";
    
    // (AR) فحص null
    if (config_.generateNullChecks) {
        output << "    " << ArabicWrapperKeywords::IF << " ";
        output << "\xD9\x85\xD9\x88\xD8\xB1\xD8\xAF == " << ArabicWrapperKeywords::NULL_VAL << " {\n";
        
        if (config_.useResultType) {
            output << "        " << ArabicWrapperKeywords::RETURN << " ";
            output << ArabicWrapperKeywords::ERROR << "(\"";
            output << ArabicWrapperKeywords::OPEN_FAILED << "\")\n";
        } else {
            output << "        " << ArabicWrapperKeywords::RETURN << " ";
            output << ArabicWrapperKeywords::NULL_VAL << "\n";
        }
        
        output << "    }\n";
    }
    
    // (AR) إنشاء الغلاف الآمن
    output << "    " << ArabicWrapperKeywords::CONST << " ";
    output << "\xD8\xBA\xD9\x84\xD8\xA7\xD9\x81 = " << ArabicWrapperKeywords::SAFE_WRAPPER;
    output << "(\xD9\x85\xD9\x88\xD8\xB1\xD8\xAF, " << closer << ")\n";
    
    // (AR) الإرجاع
    if (config_.useResultType) {
        output << "    " << ArabicWrapperKeywords::RETURN << " ";
        output << ArabicWrapperKeywords::SUCCESS << "(\xD8\xBA\xD9\x84\xD8\xA7\xD9\x81)\n";
    } else {
        output << "    " << ArabicWrapperKeywords::RETURN << " \xD8\xBA\xD9\x84\xD8\xA7\xD9\x81\n";
    }
    
    output << ArabicWrapperKeywords::END << "\n\n";
    
    return output.str();
}

/**
 * @brief (AR) توليد غلاف عام
 *        (EN) Generate generic wrapper
 */
std::string WrapperGenerator::generateWrapper(const ExternFunctionDecl& funcDecl,
                                              const WrapperInfo& info) {
    std::ostringstream output;
    
    // (AR) التوثيق
    if (config_.generateDocumentation) {
        output << generateDocumentation(funcDecl);
    }
    
    // (AR) تصريح الدالة
    output << ArabicWrapperKeywords::FUNCTION << " " << info.sadWrapperName << "(";
    
    const auto& params = funcDecl.getParameters();
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) output << ", ";
        
        // (AR) إضافة annotations للملكية
        if (i < info.paramPolicies.size()) {
            OwnershipPolicy policy = info.paramPolicies[i].second;
            if (policy == OwnershipPolicy::BORROWED) {
                output << "#[\xD9\x85\xD8\xB3\xD8\xAA\xD8\xB9\xD8\xA7\xD8\xB1] ";
            } else if (policy == OwnershipPolicy::OWNED) {
                output << "#[\xD9\x85\xD9\x85\xD9\x84\xD9\x88\xD9\x83] ";
            }
        }
        
        output << params[i].name << ": " << params[i].type->getCName();
    }
    
    output << ")";
    
    // (AR) نوع الإرجاع
    if (funcDecl.getReturnType()->getCName() != "void") {
        std::string returnType = funcDecl.getReturnType()->getCName();
        
        if (config_.useResultType && info.returnPolicy == OwnershipPolicy::OWNED) {
            output << " -> " << ArabicWrapperKeywords::RESULT << "<";
            output << ArabicWrapperKeywords::SAFE_WRAPPER << "<" << returnType << ">>";
        } else {
            output << " -> " << returnType;
        }
    }
    
    output << "\n";
    
    // (AR) جسم الدالة
    output << "    " << ArabicWrapperKeywords::CONST << " ";
    output << "\xD9\x86\xD8\xAA\xD9\x8A\xD8\xAC\xD8\xA9 = ";  // نتيجة =
    output << info.cFunctionName << "(";
    
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) output << ", ";
        output << params[i].name;
    }
    
    output << ")\n";
    
    // (AR) فحص null إذا طُلب
    if (info.checksNull && funcDecl.getReturnType()->isPointer()) {
        output << generateNullCheck("\xD9\x86\xD8\xAA\xD9\x8A\xD8\xAC\xD8\xA9");
    }
    
    // (AR) لف النتيجة إذا كانت مملوكة
    if (info.returnPolicy == OwnershipPolicy::OWNED && !info.destructorName.empty()) {
        output << "    " << ArabicWrapperKeywords::CONST << " ";
        output << "\xD8\xBA\xD9\x84\xD8\xA7\xD9\x81 = ";
        output << generateRAIIWrapper("\xD9\x86\xD8\xAA\xD9\x8A\xD8\xAC\xD8\xA9", info.destructorName);
        output << "\n";
        
        if (config_.useResultType) {
            output << generateSuccessReturn("\xD8\xBA\xD9\x84\xD8\xA7\xD9\x81");
        } else {
            output << "    " << ArabicWrapperKeywords::RETURN << " \xD8\xBA\xD9\x84\xD8\xA7\xD9\x81\n";
        }
    } else {
        output << "    " << ArabicWrapperKeywords::RETURN << " \xD9\x86\xD8\xAA\xD9\x8A\xD8\xAC\xD8\xA9\n";
    }
    
    output << ArabicWrapperKeywords::END << "\n\n";
    
    return output.str();
}

/**
 * @brief (AR) توليد أغلفة لملف كامل
 *        (EN) Generate wrappers for complete file
 */
std::string WrapperGenerator::generateFileWrappers(
    const std::vector<ExternFunctionDecl>& functions,
    const std::unordered_map<std::string, std::string>& destructorMap) {
    
    std::ostringstream output;
    
    // (AR) رأس الملف
    output << "// \xD8\xA3\xD8\xBA\xD9\x84\xD9\x81\xD8\xA9 \xD8\xA2\xD9\x85\xD9\x86\xD8\xA9 "
              "\xD9\x85\xD9\x88\xD9\x84\xD9\x91\xD8\xAF\xD8\xA9 \xD8\xAA\xD9\x84\xD9\x82\xD8\xA7\xD8\xA6\xD9\x8A\xD8\xA7\xD9\x8B\n";
    output << "// Safe wrappers generated automatically\n\n";
    
    for (const auto& func : functions) {
        // (AR) البحث عن دالة التحرير المناسبة
        auto it = destructorMap.find(func.getName());
        
        if (it != destructorMap.end()) {
            // (AR) دالة تحجز ذاكرة لها destructor معروف
            output << generateAllocatorWrapper(func, it->second);
        } else if (func.getReturnType()->isPointer()) {
            // (AR) دالة تُرجع مؤشر بدون destructor معروف
            WrapperInfo info;
            info.cFunctionName = func.getName();
            info.sadWrapperName = func.getName() + ArabicWrapperKeywords::SAFE_SUFFIX;
            info.returnPolicy = OwnershipPolicy::BORROWED;  // افتراض مستعار
            info.checksNull = true;
            
            output << generateWrapper(func, info);
        }
    }
    
    return output.str();
}

// --- (AR) دوال مساعدة / (EN) Helper functions ---

std::string WrapperGenerator::generateNullCheck(const std::string& varName) {
    std::ostringstream output;
    
    output << "    " << ArabicWrapperKeywords::IF << " ";
    output << varName << " == " << ArabicWrapperKeywords::NULL_VAL << " {\n";
    
    if (config_.useResultType) {
        output << "        " << ArabicWrapperKeywords::RETURN << " ";
        output << ArabicWrapperKeywords::ERROR << "(\"";
        output << ArabicWrapperKeywords::ALLOC_FAILED << "\")\n";
    } else {
        output << "        " << ArabicWrapperKeywords::RETURN << " ";
        output << ArabicWrapperKeywords::NULL_VAL << "\n";
    }
    
    output << "    }\n";
    
    return output.str();
}

std::string WrapperGenerator::generateErrorReturn(const std::string& message) {
    std::ostringstream output;
    output << "    " << ArabicWrapperKeywords::RETURN << " ";
    output << ArabicWrapperKeywords::ERROR << "(\"" << message << "\")\n";
    return output.str();
}

std::string WrapperGenerator::generateSuccessReturn(const std::string& value) {
    std::ostringstream output;
    output << "    " << ArabicWrapperKeywords::RETURN << " ";
    output << ArabicWrapperKeywords::SUCCESS << "(" << value << ")\n";
    return output.str();
}

std::string WrapperGenerator::generateRAIIWrapper(const std::string& ptr, 
                                                   const std::string& destructor) {
    std::ostringstream output;
    output << ArabicWrapperKeywords::SAFE_WRAPPER << "(" << ptr << ", " << destructor << ")";
    return output.str();
}

std::string WrapperGenerator::generateDocumentation(const ExternFunctionDecl& funcDecl) {
    std::ostringstream output;
    
    output << "/// " << ArabicWrapperKeywords::SAFE_WRAPPER;
    output << " \xD9\x84\xD9\x80 " << funcDecl.getName() << "\n";
    output << "/// \xD9\x8A\xD8\xAF\xD9\x8A\xD8\xB1 \xD8\xA7\xD9\x84\xD8\xB0\xD8\xA7\xD9\x83\xD8\xB1\xD8\xA9 "
              "\xD8\xAA\xD9\x84\xD9\x82\xD8\xA7\xD8\xA6\xD9\x8A\xD8\xA7\xD9\x8B\n";  // يدير الذاكرة تلقائياً
    
    return output.str();
}

std::string WrapperGenerator::convertToArabicName(const std::string& cName) {
    /*
     * (AR) تحويل أسماء C الشائعة إلى أسماء عربية
     * (EN) Convert common C names to Arabic names
     */
    static std::unordered_map<std::string, std::string> translations = {
        {"malloc", "\xD8\xA7\xD8\xAD\xD8\xAC\xD8\xB2_\xD8\xB0\xD8\xA7\xD9\x83\xD8\xB1\xD8\xA9"},  // احجز_ذاكرة
        {"free", "\xD8\xAD\xD8\xB1\xD8\xB1_\xD8\xB0\xD8\xA7\xD9\x83\xD8\xB1\xD8\xA9"},           // حرر_ذاكرة
        {"realloc", "\xD8\xA3\xD8\xB9\xD8\xAF_\xD8\xAD\xD8\xAC\xD8\xB2"},                       // أعد_حجز
        {"calloc", "\xD8\xA7\xD8\xAD\xD8\xAC\xD8\xB2_\xD9\x85\xD8\xB5\xD9\x81\xD8\xB1"},        // احجز_مصفر
        {"fopen", "\xD8\xA7\xD9\x81\xD8\xAA\xD8\xAD_\xD9\x85\xD9\x84\xD9\x81"},                  // افتح_ملف
        {"fclose", "\xD8\xA3\xD8\xBA\xD9\x84\xD9\x82_\xD9\x85\xD9\x84\xD9\x81"},                 // أغلق_ملف
        {"fread", "\xD8\xA7\xD9\x82\xD8\xB1\xD8\xA3_\xD9\x85\xD9\x84\xD9\x81"},                  // اقرأ_ملف
        {"fwrite", "\xD8\xA7\xD9\x83\xD8\xAA\xD8\xA8_\xD9\x85\xD9\x84\xD9\x81"},                 // اكتب_ملف
        {"printf", "\xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9_\xD9\x85\xD9\x86\xD8\xB3\xD9\x82"},         // اطبع_منسق
        {"scanf", "\xD8\xA7\xD9\x82\xD8\xB1\xD8\xA3_\xD9\x85\xD9\x86\xD8\xB3\xD9\x82"},          // اقرأ_منسق
    };
    
    auto it = translations.find(cName);
    if (it != translations.end()) {
        return it->second;
    }
    
    return cName;
}

// ============================================================================
//                    (AR) تنفيذ OwnershipTracker — نُقل في DEF-001
//                    (EN) OwnershipTracker Implementation — moved in DEF-001
// ============================================================================
//
// (AR) كان التنفيذ هنا (~135 سطر). نُقل إلى shared/ownership_runtime/src/
//      ownership_tracker.cpp مع إضافة قفل خيوط (std::mutex) وطبقة C-ABI
//      موازية. الاسم القديم Sad::FFI::OwnershipTracker لا يزال يعمل عبر
//      using-alias في compiler/include/ffi/ffi_wrapper.h.
//
// (EN) Implementation was here (~135 lines). Moved by DEF-001 to
//      shared/ownership_runtime/src/ownership_tracker.cpp with added thread
//      safety and a parallel C-ABI surface. Legacy name still works via
//      a using-alias in ffi_wrapper.h.
// ============================================================================

// ============================================================================
//                    (AR) تنفيذ LeakDetector
//                    (EN) LeakDetector Implementation
// ============================================================================

std::vector<LeakDetector::LeakWarning> 
LeakDetector::analyzeFunction(const std::string& code) {
    std::vector<LeakWarning> warnings;
    
    /*
     * (AR) تحليل بسيط للكود:
     *      1. البحث عن استدعاءات malloc/new
     *      2. التأكد من وجود free/delete مقابل
     *      3. التحقق من مسارات الخروج
     * 
     * (EN) Simple code analysis:
     *      1. Find malloc/new calls
     *      2. Ensure matching free/delete
     *      3. Check exit paths
     */
    
    // (AR) البحث عن تخصيصات
    std::regex allocRegex(R"((malloc|calloc|realloc|new)\s*\()");
    std::smatch match;
    std::string::const_iterator searchStart(code.cbegin());
    
    std::vector<std::string> allocations;
    
    while (std::regex_search(searchStart, code.cend(), match, allocRegex)) {
        allocations.push_back(match[1].str());
        searchStart = match.suffix().first;
    }
    
    // (AR) البحث عن تحريرات
    std::regex freeRegex(R"((free|delete)\s*\()");
    searchStart = code.cbegin();
    
    size_t freeCount = 0;
    while (std::regex_search(searchStart, code.cend(), match, freeRegex)) {
        freeCount++;
        searchStart = match.suffix().first;
    }
    
    // (AR) تحذير إذا كان عدد التحريرات أقل من التخصيصات
    if (freeCount < allocations.size()) {
        LeakWarning warning;
        warning.type = LeakWarning::POSSIBLE_LEAK;
        warning.message = "Possible memory leak: " + std::to_string(allocations.size()) + 
                         " allocations but only " + std::to_string(freeCount) + " frees";
        warning.arabicMessage = "\xD8\xAA\xD8\xB3\xD8\xB1\xD8\xA8 \xD9\x85\xD8\xAD\xD8\xAA\xD9\x85\xD9\x84: "
                               + std::to_string(allocations.size()) + " \xD8\xAA\xD8\xAE\xD8\xB5\xD9\x8A\xD8\xB5 "
                               "\xD9\x88 " + std::to_string(freeCount) + " \xD8\xAA\xD8\xAD\xD8\xB1\xD9\x8A\xD8\xB1";
        warnings.push_back(warning);
    }
    
    return warnings;
}

std::vector<LeakDetector::LeakWarning> 
LeakDetector::analyzeFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return {};
    }
    
    std::ostringstream content;
    content << file.rdbuf();
    
    return analyzeFunction(content.str());
}

std::string LeakDetector::formatWarningsArabic(
    const std::vector<LeakWarning>& warnings) const {
    
    std::ostringstream output;
    
    if (warnings.empty()) {
        output << "\xD9\x84\xD8\xA7 \xD8\xAA\xD9\x88\xD8\xAC\xD8\xAF "
                  "\xD8\xAA\xD8\xAD\xD8\xB0\xD9\x8A\xD8\xB1\xD8\xA7\xD8\xAA\n";
        return output.str();
    }
    
    output << "=== \xD8\xAA\xD8\xAD\xD8\xB0\xD9\x8A\xD8\xB1\xD8\xA7\xD8\xAA "
              "\xD8\xA7\xD9\x84\xD8\xAA\xD8\xB3\xD8\xB1\xD8\xA8 ===\n\n";
    
    for (size_t i = 0; i < warnings.size(); ++i) {
        const auto& w = warnings[i];
        
        output << (i + 1) << ". ";
        
        switch (w.type) {
            case LeakWarning::POSSIBLE_LEAK:
                output << "\xD8\xAA\xD8\xB3\xD8\xB1\xD8\xA8 \xD9\x85\xD8\xAD\xD8\xAA\xD9\x85\xD9\x84";
                break;
            case LeakWarning::DEFINITE_LEAK:
                output << "\xD8\xAA\xD8\xB3\xD8\xB1\xD8\xA8 \xD9\x85\xD8\xA4\xD9\x83\xD8\xAF";
                break;
            case LeakWarning::USE_AFTER_FREE:
                output << "\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xAE\xD8\xAF\xD8\xA7\xD9\x85 \xD8\xA8\xD8\xB9\xD8\xAF "
                          "\xD8\xA7\xD9\x84\xD8\xAA\xD8\xAD\xD8\xB1\xD9\x8A\xD8\xB1";
                break;
            case LeakWarning::DOUBLE_FREE:
                output << "\xD8\xAA\xD8\xAD\xD8\xB1\xD9\x8A\xD8\xB1 \xD9\x85\xD8\xB2\xD8\xAF\xD9\x88\xD8\xAC";
                break;
            case LeakWarning::MISSING_CLEANUP:
                output << "\xD8\xAA\xD9\x86\xD8\xB8\xD9\x8A\xD9\x81 \xD9\x85\xD9\x81\xD9\x82\xD9\x88\xD8\xAF";
                break;
        }
        
        output << "\n";
        
        if (!w.file.empty()) {
            output << "   \xD8\xA7\xD9\x84\xD9\x85\xD9\x84\xD9\x81: " << w.file;
            if (w.line > 0) {
                output << ":" << w.line;
            }
            output << "\n";
        }
        
        if (!w.variableName.empty()) {
            output << "   \xD8\xA7\xD9\x84\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1: " << w.variableName << "\n";
        }
        
        output << "   " << w.arabicMessage << "\n\n";
    }
    
    return output.str();
}

} // namespace FFI
} // namespace Sad
