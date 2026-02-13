// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file gc_mode.cpp
 * @brief تنفيذ نظام وضع الذاكرة الذكي / Smart Memory Mode Implementation
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#include "memory/gc_mode.h"
#include <iostream>
#include <algorithm>
#include <regex>

namespace Sad {
namespace Memory {

// =============================================================================
//                    تنفيذ MemoryModeManager
// =============================================================================

MemoryModeManager::MemoryModeManager()
    : settings_(MemoryModeSettings::developmentDefaults()) {
    // الوضع الافتراضي: تطوير (للتسهيل على المبتدئين)
}

MemoryModeManager::MemoryModeManager(const MemoryModeSettings& settings)
    : settings_(settings) {
}

MemoryModeManager::~MemoryModeManager() = default;

// =============================================================================
//                    تهيئة الوضع
// =============================================================================

bool MemoryModeManager::setModeFromFlag(const std::string& flag) {
    auto mode = parseCommandLineFlag(flag);
    if (!mode) {
        return false;
    }
    
    // (AR) إذا كان وضع النواة مفعّلاً، يُسمح فقط بـ Production
    // (EN) If no_std mode is active, only Production is allowed
    if (noStdMode_ && *mode != MemoryMode::Production) {
        std::cerr << u8"تحذير: وضع بلا_مكتبة_قياسية يفرض وضع الإنتاج. "
                  << u8"تم تجاهل العلم '" << flag << u8"'.\n";
        return false;
    }
    
    switch (*mode) {
        case MemoryMode::Development:
            settings_ = MemoryModeSettings::developmentDefaults();
            break;
        case MemoryMode::Production:
            settings_ = MemoryModeSettings::productionDefaults();
            break;
        case MemoryMode::Hybrid:
            settings_.mode = MemoryMode::Hybrid;
            settings_.gcStrategy = GCStrategy::ReferenceCounting;
            settings_.ownershipMode = OwnershipMode::Warnings;
            break;
        case MemoryMode::Auto:
            // اكتشاف تلقائي لاحقاً
            settings_.mode = MemoryMode::Auto;
            break;
    }
    
    return true;
}

bool MemoryModeManager::setModeFromAttribute(const std::string& attribute) {
    auto mode = parseCodeAttribute(attribute);
    if (!mode) {
        return false;
    }
    
    settings_.mode = *mode;
    
    // تعديل الإعدادات بناءً على الوضع
    switch (*mode) {
        case MemoryMode::Development:
            settings_.gcStrategy = GCStrategy::ReferenceCounting;
            settings_.ownershipMode = OwnershipMode::Warnings;
            break;
        case MemoryMode::Production:
            settings_.gcStrategy = GCStrategy::None;
            settings_.ownershipMode = OwnershipMode::UltraStrict;
            break;
        default:
            break;
    }
    
    return true;
}

void MemoryModeManager::setSettings(const MemoryModeSettings& settings) {
    settings_ = settings;
}

const MemoryModeSettings& MemoryModeManager::getSettings() const {
    return settings_;
}

// =============================================================================
//                    الاستعلام
// =============================================================================

bool MemoryModeManager::isDevelopmentMode() const {
    return settings_.mode == MemoryMode::Development ||
           settings_.mode == MemoryMode::Hybrid;
}

bool MemoryModeManager::isProductionMode() const {
    return settings_.mode == MemoryMode::Production;
}

bool MemoryModeManager::isGCEnabled() const {
    return settings_.gcStrategy != GCStrategy::None;
}

bool MemoryModeManager::isOwnershipCheckingEnabled() const {
    return settings_.ownershipMode != OwnershipMode::Disabled;
}

bool MemoryModeManager::areSuggestionsEnabled() const {
    return settings_.enableOwnershipSuggestions;
}

bool MemoryModeManager::isNoStdMode() const {
    return noStdMode_;
}

// =============================================================================
//                    فرض وضع النواة / No-std Enforcement
// =============================================================================

void MemoryModeManager::enforceNoStdMode() {
    // (AR) فرض إعدادات النواة: بلا GC، ملكية صارمة جداً
    // (EN) Enforce kernel settings: no GC, ultra-strict ownership
    noStdMode_ = true;
    settings_ = MemoryModeSettings::kernelDefaults();
}

std::string MemoryModeManager::validateNoStdCompatibility() const {
    if (!noStdMode_) {
        return "";  // (AR) لا تعارض في الوضع العادي
    }
    
    // (AR) التحقق: لا يمكن استخدام GC في وضع النواة
    // (EN) Check: GC cannot be used in kernel mode
    if (settings_.gcStrategy != GCStrategy::None) {
        return u8"خطأ: لا يمكن استخدام جامع القمامة في وضع بلا مكتبة قياسية. "
               u8"استخدم نظام الملكية بدلاً من ذلك.\n"
               "Error: Cannot use garbage collector in no_std mode. "
               "Use ownership system instead.";
    }
    
    // (AR) التحقق: يجب أن يكون مستوى الملكية Strict أو UltraStrict
    // (EN) Check: ownership level must be Strict or UltraStrict
    if (settings_.ownershipMode == OwnershipMode::Disabled) {
        return u8"خطأ: لا يمكن تعطيل نظام الملكية في وضع بلا مكتبة قياسية. "
               u8"يجب استخدام ملكية صارمة.\n"
               "Error: Cannot disable ownership in no_std mode. "
               "Strict ownership is required.";
    }
    
    return "";  // (AR) لا تعارض
}

// =============================================================================
//                    الأسماء العربية
// =============================================================================

std::string MemoryModeManager::getModeNameArabic() const {
    return memoryModeToArabic(settings_.mode);
}

std::string MemoryModeManager::getModeDescriptionArabic() const {
    switch (settings_.mode) {
        case MemoryMode::Development:
            return "وضع التطوير: جامع قمامة تلقائي للتجريب السريع. "
                   "لا تحتاج للتفكير في الملكية أثناء كتابة الكود. "
                   "المترجم سيُقدِّم اقتراحات لتحويل الكود للملكية عند الجاهزية.";
                   
        case MemoryMode::Production:
            return "وضع الإنتاج: نظام ملكية صارم بدون GC. "
                   "أقصى أداء ممكن مع أمان كامل في وقت الترجمة. "
                   "للإصدارات النهائية والأنظمة الحرجة.";
                   
        case MemoryMode::Hybrid:
            return "وضع مختلط: GC للأجزاء العادية، ملكية للأجزاء الحرجة. "
                   "استخدم #[ملكية] للدوال التي تحتاج أداء عالي.";
                   
        case MemoryMode::Auto:
            return "وضع تلقائي: المترجم يختار أفضل وضع بناءً على السياق. "
                   "برامج صغيرة ← GC، برامج كبيرة ← ملكية.";
    }
    return "وضع غير معروف";
}

std::string MemoryModeManager::getGCStrategyNameArabic() const {
    return gcStrategyToArabic(settings_.gcStrategy);
}

// =============================================================================
//                    التنقيح
// =============================================================================

void MemoryModeManager::dump() const {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              إعدادات وضع الذاكرة الحالية                     ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "  الوضع الرئيسي:        " << getModeNameArabic() << "\n";
    std::cout << "  استراتيجية GC:        " << getGCStrategyNameArabic() << "\n";
    std::cout << "  مستوى الملكية:        " << ownershipModeToArabic(settings_.ownershipMode) << "\n";
    std::cout << "  اقتراحات التحويل:     " << (settings_.enableOwnershipSuggestions ? "مُفعَّل" : "مُعطَّل") << "\n";
    std::cout << "  كشف الدورات:          " << (settings_.enableCycleDetection ? "مُفعَّل" : "مُعطَّل") << "\n";
    std::cout << "  حد ذاكرة GC:          " << settings_.gcMemoryLimitMB << " ميغابايت\n";
    std::cout << "  وضع التصحيح:          " << (settings_.debugMode ? "مُفعَّل" : "مُعطَّل") << "\n";
    std::cout << "  وضع المعلم:           " << (settings_.teacherMode ? "مُفعَّل" : "مُعطَّل") << "\n";
    std::cout << "\n";
    std::cout << "  الوصف:\n";
    std::cout << "  " << getModeDescriptionArabic() << "\n";
    std::cout << "\n";
}

// =============================================================================
//                    الدوال الخاصة
// =============================================================================

std::optional<MemoryMode> MemoryModeManager::parseCommandLineFlag(const std::string& flag) const {
    // أعلام عربية
    if (flag == "--تطوير" || flag == "--dev" || flag == "--development") {
        return MemoryMode::Development;
    }
    if (flag == "--إنتاج" || flag == "--prod" || flag == "--production" || flag == "--release") {
        return MemoryMode::Production;
    }
    if (flag == "--مختلط" || flag == "--hybrid" || flag == "--mixed") {
        return MemoryMode::Hybrid;
    }
    if (flag == "--تلقائي" || flag == "--auto") {
        return MemoryMode::Auto;
    }
    if (flag == "--تعلم" || flag == "--learn" || flag == "--learning") {
        return MemoryMode::Development;  // وضع التعلم = تطوير مع رسائل إضافية
    }
    
    return std::nullopt;
}

std::optional<MemoryMode> MemoryModeManager::parseCodeAttribute(const std::string& attribute) const {
    // أنماط السمات
    // #[وضع_ذاكرة: تطوير]
    // #[memory_mode: development]
    
    std::regex arabicPattern(R"(#\[وضع_ذاكرة:\s*(\S+)\])");
    std::regex englishPattern(R"(#\[memory_mode:\s*(\S+)\])");
    
    std::smatch match;
    std::string mode;
    
    if (std::regex_search(attribute, match, arabicPattern) && match.size() > 1) {
        mode = match[1].str();
    } else if (std::regex_search(attribute, match, englishPattern) && match.size() > 1) {
        mode = match[1].str();
    } else {
        return std::nullopt;
    }
    
    // تحويل النص للوضع
    if (mode == "تطوير" || mode == "development" || mode == "dev") {
        return MemoryMode::Development;
    }
    if (mode == "إنتاج" || mode == "production" || mode == "prod") {
        return MemoryMode::Production;
    }
    if (mode == "مختلط" || mode == "hybrid") {
        return MemoryMode::Hybrid;
    }
    if (mode == "تلقائي" || mode == "auto") {
        return MemoryMode::Auto;
    }
    
    return std::nullopt;
}

// =============================================================================
//                    الدوال المساعدة
// =============================================================================

std::string memoryModeToArabic(MemoryMode mode) {
    switch (mode) {
        case MemoryMode::Development: return "تطوير";
        case MemoryMode::Production:  return "إنتاج";
        case MemoryMode::Hybrid:      return "مختلط";
        case MemoryMode::Auto:        return "تلقائي";
    }
    return "غير معروف";
}

std::string memoryModeToEnglish(MemoryMode mode) {
    switch (mode) {
        case MemoryMode::Development: return "Development";
        case MemoryMode::Production:  return "Production";
        case MemoryMode::Hybrid:      return "Hybrid";
        case MemoryMode::Auto:        return "Auto";
    }
    return "Unknown";
}

std::string gcStrategyToArabic(GCStrategy strategy) {
    switch (strategy) {
        case GCStrategy::None:                    return "بدون (ملكية صرفة)";
        case GCStrategy::ReferenceCounting:       return "عد المراجع";
        case GCStrategy::AtomicReferenceCounting: return "عد المراجع الذري";
        case GCStrategy::Tracing:                 return "تتبع (Mark & Sweep)";
        case GCStrategy::Incremental:             return "تدريجي";
    }
    return "غير معروف";
}

std::string ownershipModeToArabic(OwnershipMode mode) {
    switch (mode) {
        case OwnershipMode::Disabled:    return "مُعطَّل";
        case OwnershipMode::Warnings:    return "تحذيرات فقط";
        case OwnershipMode::Strict:      return "صارم";
        case OwnershipMode::UltraStrict: return "صارم جداً (كـ Rust)";
    }
    return "غير معروف";
}

} // namespace Memory
} // namespace Sad
