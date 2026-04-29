/**
 * @file no_std_integration.cpp
 * @brief (AR) تنفيذ تكامل وضع بلا مكتبة قياسية مع خط الترجمة
 *        (EN) Implementation of no_std mode integration with compilation pipeline
 * 
 * --- م-ب02: دمج NoStdModeManager في خط الأنابيب الرئيسي ---
 * 
 * (AR) يفحص الكود المصدري لسمات #![بلا_مكتبة_قياسية] ويُنشئ ويُعد
 *      NoStdModeManager تلقائياً. يُستدعى من خط الترجمة بعد مرحلة التحليل.
 * 
 * (EN) Scans source code for #![no_std] attributes and automatically creates
 *      and configures NoStdModeManager. Called from pipeline after parsing stage.
 */

#include "no_std_integration.h"
#include "no_std_mode.h"

#include <string>
#include <vector>
#include <regex>
#include <algorithm>

namespace sad {
namespace compiler {
namespace pipeline {

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) تنفيذ فحص المصدر / (EN) Source scanning implementation
// ═══════════════════════════════════════════════════════════════════════════════

NoStdScanResult NoStdIntegration::scanSourceForNoStd(const std::string& sourceCode) {
    NoStdScanResult result;
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) البحث عن سمات crate-level: #![سمة]
    // (EN) Search for crate-level attributes: #![attr]
    // ─────────────────────────────────────────────────────────────────────────
    
    // (AR) نمط سمة الحزمة: #![اسم_السمة]
    // (EN) Crate attribute pattern: #![attribute_name]
    
    // (AR) فحص بسيط بالبحث النصي (أسرع من regex للملفات الكبيرة)
    // (EN) Simple text search (faster than regex for large files)
    
    auto containsAttribute = [&sourceCode](const std::string& attr) -> bool {
        // (AR) البحث عن #![attr] أو #![ attr ]
        std::string pattern1 = "#![" + attr + "]";
        std::string pattern2 = "#![ " + attr + " ]";
        return sourceCode.find(pattern1) != std::string::npos ||
               sourceCode.find(pattern2) != std::string::npos;
    };
    
    // (AR) فحص سمات الحزمة العربية والإنجليزية
    // (EN) Check Arabic and English crate attributes
    result.hasNoStd = containsAttribute(سمات::بلا_مكتبة_قياسية) ||
                       containsAttribute(سمات::NO_STD);
    
    result.hasNoMain = containsAttribute(سمات::بلا_رئيسية) ||
                        containsAttribute(سمات::NO_MAIN);
    
    result.hasAbortOnPanic = containsAttribute(سمات::إيقاف_عند_ذعر) ||
                              containsAttribute(سمات::ABORT_ON_PANIC);
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) البحث عن سمات الدوال: #[سمة] دالة اسم()
    // (EN) Search for function attributes: #[attr] fn name()
    // ─────────────────────────────────────────────────────────────────────────
    
    // (AR) البحث عن #[نقطة_دخول] أو #[entry_point]
    auto findFunctionWithAttribute = [&](const std::string& arabicAttr,
                                          const std::string& englishAttr) 
        -> std::string 
    {
        for (const auto& attr : {arabicAttr, englishAttr}) {
            std::string marker = "#[" + attr + "]";
            size_t pos = sourceCode.find(marker);
            if (pos == std::string::npos) continue;
            
            // (AR) البحث عن اسم الدالة بعد السمة
            // (EN) Find function name after attribute
            size_t afterAttr = pos + marker.length();
            
            // (AR) تخطي المسافات والأسطر الجديدة
            while (afterAttr < sourceCode.length() && 
                   (sourceCode[afterAttr] == ' ' || sourceCode[afterAttr] == '\n' ||
                    sourceCode[afterAttr] == '\r' || sourceCode[afterAttr] == '\t')) {
                afterAttr++;
            }
            
            // (AR) البحث عن كلمة "دالة" أو "fn"
            std::string funcKeywordAr = "دالة";
            std::string funcKeywordEn = "fn";
            
            size_t funcStart = std::string::npos;
            if (sourceCode.compare(afterAttr, funcKeywordAr.length(), funcKeywordAr) == 0) {
                funcStart = afterAttr + funcKeywordAr.length();
            } else if (sourceCode.compare(afterAttr, funcKeywordEn.length(), funcKeywordEn) == 0) {
                funcStart = afterAttr + funcKeywordEn.length();
            }
            
            if (funcStart == std::string::npos) continue;
            
            // (AR) تخطي المسافات
            while (funcStart < sourceCode.length() && sourceCode[funcStart] == ' ') {
                funcStart++;
            }
            
            // (AR) قراءة اسم الدالة
            size_t funcEnd = funcStart;
            while (funcEnd < sourceCode.length() && 
                   sourceCode[funcEnd] != '(' && sourceCode[funcEnd] != ' ' &&
                   sourceCode[funcEnd] != '\n') {
                funcEnd++;
            }
            
            if (funcEnd > funcStart) {
                return sourceCode.substr(funcStart, funcEnd - funcStart);
            }
        }
        return "";
    };
    
    // (AR) البحث عن نقطة الدخول
    std::string entryPoint = findFunctionWithAttribute(
        سمات::نقطة_دخول, سمات::ENTRY_POINT);
    if (!entryPoint.empty()) {
        result.functionAttributes.push_back({سمات::ENTRY_POINT, entryPoint});
    }
    
    // (AR) البحث عن معالج الذعر
    std::string panicHandler = findFunctionWithAttribute(
        سمات::معالج_ذعر, سمات::PANIC_HANDLER);
    if (!panicHandler.empty()) {
        result.functionAttributes.push_back({سمات::PANIC_HANDLER, panicHandler});
    }
    
    // (AR) البحث عن معالج التخصيص
    std::string allocHandler = findFunctionWithAttribute(
        سمات::معالج_تخصيص, سمات::ALLOC_ERROR_HANDLER);
    if (!allocHandler.empty()) {
        result.functionAttributes.push_back({سمات::ALLOC_ERROR_HANDLER, allocHandler});
    }
    
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) إنشاء المدير / (EN) Create manager
// ═══════════════════════════════════════════════════════════════════════════════

std::unique_ptr<NoStdModeManager> NoStdIntegration::createManagerFromScan(
    const NoStdScanResult& scanResult,
    const std::string& targetTriple
) {
    // (AR) لا حاجة لمدير إذا لم يكن no_std مطلوباً
    // (EN) No need for manager if no_std not requested
    bool needsNoStd = scanResult.hasNoStd || 
                      (!targetTriple.empty() && targetRequiresNoStd(targetTriple));
    
    if (!needsNoStd) {
        return nullptr;
    }
    
    // (AR) إنشاء المدير وتهيئته
    // (EN) Create and initialize manager
    auto manager = std::make_unique<NoStdModeManager>();
    auto& config = manager->getConfig();
    
    // (AR) تطبيق سمات الحزمة
    config.noStdEnabled = true;
    config.noMainEnabled = scanResult.hasNoMain;
    config.abortOnPanic = scanResult.hasAbortOnPanic;
    config.targetTriple = targetTriple;
    
    // (AR) تسجيل سمات الدوال
    for (const auto& [attr, funcName] : scanResult.functionAttributes) {
        manager->registerFunctionAttribute(attr, funcName);
    }
    
    return manager;
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) التحقق / (EN) Validation
// ═══════════════════════════════════════════════════════════════════════════════

std::vector<ValidationError> NoStdIntegration::validateNoStdRequirements(
    NoStdModeManager& manager
) {
    return manager.validate();
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) إعداد خيارات البناء / (EN) Build options setup
// ═══════════════════════════════════════════════════════════════════════════════

void NoStdIntegration::applyBuildOptions(
    const NoStdModeManager& manager,
    std::vector<std::string>& excludedLibs,
    std::vector<std::string>& linkerFlags
) {
    if (!manager.isNoStdEnabled()) return;
    
    // (AR) إضافة المكتبات المستبعدة
    auto libs = manager.getExcludedLibraries();
    excludedLibs.insert(excludedLibs.end(), libs.begin(), libs.end());
    
    // (AR) إضافة إعلام الرابط لوضع bare-metal
    // (EN) Add linker flags for bare-metal mode
    linkerFlags.push_back("-nostdlib");
    linkerFlags.push_back("-nostartfiles");
    linkerFlags.push_back("-nodefaultlibs");
    
    // (AR) إذا كان يوجد سكريبت رابط مخصص
    const auto& config = manager.getConfig();
    if (!config.linkerScript.empty()) {
        linkerFlags.push_back("-T");
        linkerFlags.push_back(config.linkerScript);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) فحص الهدف / (EN) Target check
// ═══════════════════════════════════════════════════════════════════════════════

bool NoStdIntegration::isBaremetalTarget(const std::string& targetTriple) {
    return targetRequiresNoStd(targetTriple);
}

} // namespace pipeline
} // namespace compiler
} // namespace sad
