/**
 * @file sad_installer.h
 * @brief (AR) أداة إنشاء مثبتات Windows MSI/WiX
 * @brief (EN) Windows MSI/WiX installer generation tool
 * @date May 2026
 */

#ifndef SAD_INSTALLER_H
#define SAD_INSTALLER_H

#include <string>
#include <vector>

namespace Sad {
namespace Tools {
namespace Installer {

/**
 * @struct InstallerConfig
 * @brief (AR) إعدادات المثبت
 * @brief (EN) Installer configuration
 */
struct InstallerConfig {
    std::string appName;            ///< اسم التطبيق / App name
    std::string appNameAr;          ///< اسم التطبيق بالعربية / Arabic app name
    std::string version = "1.0.0";  ///< الإصدار / Version
    std::string manufacturer;       ///< المصنّع / Manufacturer
    std::string description;        ///< وصف التطبيق / Description
    std::string exeName;            ///< اسم الملف التنفيذي / EXE name
    std::string iconPath;           ///< مسار الأيقونة / Icon path
    std::string licensePath;        ///< مسار الرخصة / License path
    std::string outputDir = "dist"; ///< مجلد الإخراج / Output directory
    
    bool addToPath = true;          ///< إضافة لمتغير PATH / Add to PATH
    bool createDesktopShortcut = true; ///< اختصار سطح المكتب / Desktop shortcut
    bool createStartMenu = true;    ///< قائمة ابدأ / Start menu entry
    
    std::vector<std::string> files; ///< ملفات إضافية / Additional files
};

/**
 * @class MSIGenerator
 * @brief (AR) مولد ملفات WiX XML ومثبتات MSI
 * @brief (EN) WiX XML and MSI installer generator
 */
class MSIGenerator {
public:
    explicit MSIGenerator(const InstallerConfig& config);
    
    /// (AR) توليد ملف WiX XML / Generate WiX XML file
    bool generateWixXml(const std::string& outputPath) const;
    
    /// (AR) توليد سكريبت PowerShell للبناء / Generate PowerShell build script
    bool generateBuildScript(const std::string& outputPath) const;
    
    /// (AR) توليد ملف NSIS كبديل / Generate NSIS script as alternative
    bool generateNSIS(const std::string& outputPath) const;

private:
    InstallerConfig config_;
    
    std::string generateGuid() const;
    std::string escapeXml(const std::string& text) const;
};

} // namespace Installer
} // namespace Tools
} // namespace Sad

#endif // SAD_INSTALLER_H
