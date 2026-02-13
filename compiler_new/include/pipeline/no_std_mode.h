/**
 * @file no_std_mode.h
 * @brief (AR) إعلانات وضع بلا مكتبة قياسية (#![بلا_مكتبة_قياسية])
 *        (EN) No-std mode declarations (#![no_std])
 * 
 * @details
 * (AR) يُستخرج من no_std_mode.cpp لتمكين التكامل مع خط الأنابيب الرئيسي
 * (EN) Extracted from no_std_mode.cpp to enable integration with main pipeline
 */

#ifndef SAD_PIPELINE_NO_STD_MODE_H
#define SAD_PIPELINE_NO_STD_MODE_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace sad {
namespace compiler {
namespace pipeline {

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) ثوابت السمات / (EN) Attribute Constants
// ═══════════════════════════════════════════════════════════════════════════════

namespace سمات {
    extern const std::string بلا_مكتبة_قياسية;
    extern const std::string NO_STD;
    extern const std::string بلا_رئيسية;
    extern const std::string NO_MAIN;
    extern const std::string نقطة_دخول;
    extern const std::string ENTRY_POINT;
    extern const std::string معالج_ذعر;
    extern const std::string PANIC_HANDLER;
    extern const std::string معالج_تخصيص;
    extern const std::string ALLOC_ERROR_HANDLER;
    extern const std::string إيقاف_عند_ذعر;
    extern const std::string ABORT_ON_PANIC;
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) الهياكل / (EN) Structures
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct NoStdConfig
 * @brief (AR) إعدادات وضع بلا مكتبة قياسية
 */
struct NoStdConfig {
    bool noStdEnabled = false;
    bool noMainEnabled = false;
    bool abortOnPanic = false;
    
    std::string entryPoint;
    std::string panicHandler;
    std::string allocErrorHandler;
    
    bool allowAlloc = false;
    bool allowFloat = true;
    bool allowAtomics = true;
    
    std::string targetTriple;
    std::string linkerScript;
};

/**
 * @struct ValidationError
 * @brief (AR) خطأ تحقق
 */
struct ValidationError {
    std::string code;
    std::string arabicMessage;
    std::string englishMessage;
    std::string hint;
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) الأصناف المُعلنة مسبقاً (التنفيذ في .cpp)
// (EN) Forward-declared classes (implementation in .cpp)
// ═══════════════════════════════════════════════════════════════════════════════

class NoStdAttributeParser;
class NoStdValidator;
class NoStdCodeGenerator;

/**
 * @class NoStdModeManager
 * @brief (AR) مدير وضع بلا مكتبة قياسية
 *        (EN) No-std mode manager
 */
class NoStdModeManager {
public:
    NoStdModeManager();
    ~NoStdModeManager();
    
    // (AR) تهيئة من سمات الحزمة
    void initializeFromAttributes(const std::vector<std::string>& attributes);
    
    // (AR) تسجيل دالة بسمة
    void registerFunctionAttribute(const std::string& attribute,
                                   const std::string& functionName);
    
    // (AR) هل وضع no_std مفعّل؟
    bool isNoStdEnabled() const;
    
    // (AR) التحقق من صحة الإعدادات
    std::vector<ValidationError> validate();
    
    // (AR) الحصول على الإعدادات
    const NoStdConfig& getConfig() const;
    NoStdConfig& getConfig();
    
    // (AR) توليد كود البدء (كنصوص IR)
    std::string generateStartupCode();
    
    // (AR) توليد سكريبت الرابط
    std::string generateLinkerScript();
    
    // (AR) الحصول على المكتبات المستبعدة/المتاحة
    std::vector<std::string> getExcludedLibraries() const;
    std::vector<std::string> getAvailableLibraries() const;
    
    // (AR) طباعة معلومات التكوين
    void printConfigInfo(std::ostream& out) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) دوال مساعدة / (EN) Helper Functions
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) إنشاء مدير no_std من سطر الأوامر
 */
std::unique_ptr<NoStdModeManager> createNoStdManagerFromArgs(
    bool noStd, bool noMain, const std::string& target);

/**
 * @brief (AR) التحقق مما إذا كان الهدف يتطلب no_std
 */
bool targetRequiresNoStd(const std::string& target);

} // namespace pipeline
} // namespace compiler
} // namespace sad

#endif // SAD_PIPELINE_NO_STD_MODE_H
