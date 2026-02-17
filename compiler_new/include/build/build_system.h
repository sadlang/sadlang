// ======================================================================
// build_system.h - نظام بناء لغة ص / Sad Language Build System
// ======================================================================
// الوصف بالعربية:
//   نظام بناء عربي يقرأ ملف بناء.ص (build.sad) ويدير:
//   - ترجمة المشاريع
//   - إدارة التبعيات
//   - أهداف البناء (debug, release, bare metal)
//   - الاختبارات
//
// English Description:
//   Arabic build system reading بناء.ص (build.sad) files
//   Manages: compilation, dependencies, targets, tests
// ======================================================================

#ifndef SAD_BUILD_SYSTEM_H
#define SAD_BUILD_SYSTEM_H

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

namespace Sad {
namespace Build {

/**
 * @brief Build target type / نوع هدف البناء
 */
enum class TargetType {
    Executable,     ///< تنفيذي / executable
    StaticLib,      ///< مكتبة_ساكنة / static library
    SharedLib,      ///< مكتبة_مشتركة / shared library
    KernelImage,    ///< صورة_نواة / kernel image
    Driver,         ///< مشغّل / device driver
    Test            ///< اختبار / test binary
};

/**
 * @brief Build profile / ملف البناء
 */
enum class BuildProfile {
    Debug,          ///< تطوير / debug — no optimizations, debug info
    Release,        ///< إصدار / release — full optimizations
    MinSize,        ///< أصغر_حجم / min-size — optimize for size
    Kernel          ///< نواة / kernel — no-std, bare metal
};

/**
 * @brief Dependency info / معلومات التبعية
 */
struct Dependency {
    std::string name;       ///< Dependency name / اسم التبعية
    std::string version;    ///< Version constraint / قيد الإصدار
    std::string path;       ///< Local path (optional) / المسار المحلي
    std::string gitUrl;     ///< Git URL (optional) / رابط Git
    bool isOptional;        ///< Optional? / اختيارية؟
    
    Dependency(const std::string& n, const std::string& v = "*")
        : name(n), version(v), isOptional(false) {}
};

/**
 * @brief Build target / هدف البناء
 */
struct BuildTarget {
    std::string name;                   ///< Target name / اسم الهدف
    TargetType type;                    ///< Target type / نوع الهدف
    std::string targetTriple;           ///< Target triple / ثلاثية الهدف (e.g., "x86_64-نحلة-bare")
    std::vector<std::string> sources;   ///< Source files / ملفات المصدر
    std::vector<Dependency> deps;       ///< Dependencies / التبعيات
    std::vector<std::string> features;  ///< Enabled features / الميزات المفعّلة
    bool noStd;                         ///< No standard library? / بدون مكتبة قياسية؟
    std::string linkerScript;           ///< Linker script path / مسار نص الربط
    
    BuildTarget(const std::string& n, TargetType t)
        : name(n), type(t), noStd(false) {}
};

/**
 * @brief Project configuration / تكوين المشروع
 * 
 * (AR) يمثل ملف بناء.ص الذي يعرّف المشروع
 * (EN) Represents the بناء.ص file that defines the project
 * 
 * @example ملف بناء.ص:
 * ```
 * [المشروع]
 * الاسم = "نواة_نحلة"
 * الإصدار = "0.1.0"
 * المؤلف = "فريق النحلة"
 * 
 * [التبعيات]
 * مكتبة_الذاكرة = { مسار = "../ذاكرة" }
 * 
 * [الهدف]
 * النوع = "صورة_نواة"
 * الهدف = "x86_64-نحلة-bare"
 * لا_مكتبة_قياسية = صحيح
 * نص_الربط = "ربط/نواة.ld"
 * ```
 */
struct ProjectConfig {
    // ========== معلومات المشروع / Project Info ==========
    std::string name;           ///< اسم المشروع / project name
    std::string version;        ///< الإصدار / version
    std::string author;         ///< المؤلف / author
    std::string description;    ///< الوصف / description
    std::string license;        ///< الرخصة / license
    
    // ========== البناء / Build ==========
    BuildProfile profile;       ///< الملف / build profile
    std::vector<BuildTarget> targets;   ///< الأهداف / targets
    std::vector<Dependency> dependencies; ///< التبعيات / dependencies
    
    // ========== الإعدادات / Settings ==========
    std::unordered_map<std::string, std::string> settings; ///< إعدادات مخصصة
    
    ProjectConfig() : profile(BuildProfile::Debug) {}
};

/**
 * @brief Build system / نظام البناء
 * 
 * (AR) يقرأ ملف بناء.ص ويدير عملية البناء
 * (EN) Reads بناء.ص and manages the build process
 */
class BuildSystem {
public:
    BuildSystem() = default;
    
    /**
     * @brief Load project config / تحميل تكوين المشروع
     * @param buildFilePath Path to بناء.ص / مسار ملف بناء.ص
     */
    bool loadConfig(const std::string& buildFilePath) {
        configPath_ = buildFilePath;
        return parseConfigFile();
    }
    
    /**
     * @brief Build all targets / بناء جميع الأهداف
     */
    bool buildAll(BuildProfile profile = BuildProfile::Debug);
    
    /**
     * @brief Build specific target / بناء هدف محدد
     */
    bool buildTarget(const std::string& targetName);
    
    /**
     * @brief Run tests / تشغيل الاختبارات
     */
    bool runTests();
    
    /**
     * @brief Get project config / الحصول على تكوين المشروع
     */
    const ProjectConfig& getConfig() const { return config_; }
    ProjectConfig& getConfig() { return config_; }
    
    /**
     * @brief Get build command for target / الحصول على أمر البناء
     */
    std::string getBuildCommand(const BuildTarget& target) const {
        std::string cmd = "sadc";
        
        if (target.noStd) cmd += " --لا-مكتبة-قياسية";
        if (!target.targetTriple.empty()) cmd += " --هدف=" + target.targetTriple;
        if (!target.linkerScript.empty()) cmd += " --نص-ربط=" + target.linkerScript;
        
        switch (config_.profile) {
            case BuildProfile::Debug:   cmd += " --تطوير"; break;
            case BuildProfile::Release: cmd += " --إصدار"; break;
            case BuildProfile::MinSize: cmd += " --أصغر-حجم"; break;
            case BuildProfile::Kernel:  cmd += " --نواة"; break;
        }
        
        for (const auto& src : target.sources) {
            cmd += " " + src;
        }
        
        return cmd;
    }
    
private:
    bool parseConfigFile();
    
    std::string configPath_;
    ProjectConfig config_;
};

/**
 * @brief Convert Arabic target type name to enum
 */
inline TargetType targetTypeFromArabic(const std::string& name) {
    if (name == "تنفيذي" || name == "exe") return TargetType::Executable;
    if (name == "مكتبة_ساكنة" || name == "static") return TargetType::StaticLib;
    if (name == "مكتبة_مشتركة" || name == "shared") return TargetType::SharedLib;
    if (name == "صورة_نواة" || name == "kernel") return TargetType::KernelImage;
    if (name == "مشغّل" || name == "driver") return TargetType::Driver;
    if (name == "اختبار" || name == "test") return TargetType::Test;
    return TargetType::Executable;
}

/**
 * @brief Convert Arabic profile name to enum
 */
inline BuildProfile profileFromArabic(const std::string& name) {
    if (name == "تطوير" || name == "debug") return BuildProfile::Debug;
    if (name == "إصدار" || name == "release") return BuildProfile::Release;
    if (name == "أصغر_حجم" || name == "min-size") return BuildProfile::MinSize;
    if (name == "نواة" || name == "kernel") return BuildProfile::Kernel;
    return BuildProfile::Debug;
}

} // namespace Build
} // namespace Sad

#endif // SAD_BUILD_SYSTEM_H
