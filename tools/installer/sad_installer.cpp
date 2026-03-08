/**
 * @file sad_installer.cpp
 * @brief (AR) تنفيذ أداة إنشاء مثبتات Windows MSI/WiX
 * @brief (EN) Windows MSI/WiX installer generator implementation
 * @date May 2026
 */

#include "sad_installer.h"
#include <fstream>
#include <sstream>
#include <random>
#include <iomanip>
#include <iostream>
#include <filesystem>

namespace Sad {
namespace Tools {
namespace Installer {

namespace fs = std::filesystem;

MSIGenerator::MSIGenerator(const InstallerConfig& config) : config_(config) {}

std::string MSIGenerator::generateGuid() const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFF);
    
    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::setfill('0');
    oss << std::setw(8) << dist(gen) << "-";
    oss << std::setw(4) << (dist(gen) & 0xFFFF) << "-";
    oss << std::setw(4) << ((dist(gen) & 0x0FFF) | 0x4000) << "-";
    oss << std::setw(4) << ((dist(gen) & 0x3FFF) | 0x8000) << "-";
    oss << std::setw(4) << (dist(gen) & 0xFFFF);
    oss << std::setw(8) << dist(gen);
    return oss.str();
}

std::string MSIGenerator::escapeXml(const std::string& text) const {
    std::string result;
    result.reserve(text.size());
    for (char c : text) {
        switch (c) {
            case '&':  result += "&amp;"; break;
            case '<':  result += "&lt;"; break;
            case '>':  result += "&gt;"; break;
            case '"':  result += "&quot;"; break;
            case '\'': result += "&apos;"; break;
            default:   result += c; break;
        }
    }
    return result;
}

bool MSIGenerator::generateWixXml(const std::string& outputPath) const {
    std::ofstream file(outputPath, std::ios::binary);
    if (!file.is_open()) return false;
    
    std::string productGuid = generateGuid();
    std::string upgradeGuid = generateGuid();
    std::string componentGuid = generateGuid();
    
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<!-- مثبت لغة ص — تم التوليد تلقائياً / Sad Language Installer — Auto-generated -->\n";
    file << "<Wix xmlns=\"http://schemas.microsoft.com/wix/2006/wi\">\n\n";
    
    file << "  <Product Id=\"" << productGuid << "\"\n";
    file << "           Name=\"" << escapeXml(config_.appName) << "\"\n";
    file << "           Language=\"1025\"\n";
    file << "           Version=\"" << config_.version << "\"\n";
    file << "           Manufacturer=\"" << escapeXml(config_.manufacturer) << "\"\n";
    file << "           UpgradeCode=\"" << upgradeGuid << "\">\n\n";
    
    file << "    <Package InstallerVersion=\"500\" Compressed=\"yes\"\n";
    file << "             InstallScope=\"perMachine\"\n";
    file << "             Description=\"" << escapeXml(config_.description) << "\"\n";
    file << "             Comments=\"" << escapeXml(config_.appNameAr) << "\" />\n\n";
    
    file << "    <MediaTemplate EmbedCab=\"yes\" />\n\n";
    
    // (AR) مجلد التثبيت
    file << "    <Directory Id=\"TARGETDIR\" Name=\"SourceDir\">\n";
    file << "      <Directory Id=\"ProgramFiles64Folder\">\n";
    file << "        <Directory Id=\"INSTALLDIR\" Name=\"" << escapeXml(config_.appName) << "\">\n";
    file << "          <Component Id=\"MainComponent\" Guid=\"" << componentGuid << "\">\n";
    file << "            <File Id=\"MainExe\" Source=\"" << config_.exeName << "\"\n";
    file << "                  KeyPath=\"yes\" />\n";
    
    // (AR) اختصار سطح المكتب
    if (config_.createDesktopShortcut) {
        file << "          </Component>\n";
        file << "        </Directory>\n";
        file << "      </Directory>\n";
        file << "      <Directory Id=\"DesktopFolder\" Name=\"Desktop\">\n";
        file << "        <Component Id=\"DesktopShortcut\" Guid=\"" << generateGuid() << "\">\n";
        file << "          <Shortcut Id=\"DesktopLink\" Name=\"" << escapeXml(config_.appName) << "\"\n";
        file << "                    Description=\"" << escapeXml(config_.description) << "\"\n";
        file << "                    Target=\"[INSTALLDIR]" << config_.exeName << "\" />\n";
        file << "          <RegistryValue Root=\"HKCU\" Key=\"Software\\" << escapeXml(config_.manufacturer) << "\\" << escapeXml(config_.appName) << "\"\n";
        file << "                         Name=\"installed\" Type=\"integer\" Value=\"1\" KeyPath=\"yes\" />\n";
        file << "        </Component>\n";
        file << "      </Directory>\n";
    } else {
        file << "          </Component>\n";
        file << "        </Directory>\n";
        file << "      </Directory>\n";
    }
    
    // (AR) إضافة لمتغير PATH
    if (config_.addToPath) {
        file << "      <!-- إضافة لمتغير PATH / Add to PATH -->\n";
        file << "      <Component Id=\"PathComponent\" Guid=\"" << generateGuid() << "\" Directory=\"INSTALLDIR\">\n";
        file << "        <Environment Id=\"PATH\" Name=\"PATH\" Value=\"[INSTALLDIR]\"\n";
        file << "                     Permanent=\"no\" Part=\"last\" Action=\"set\" System=\"yes\" />\n";
        file << "      </Component>\n";
    }
    
    file << "    </Directory>\n\n";
    
    // (AR) الميزات
    file << "    <Feature Id=\"Complete\" Title=\"" << escapeXml(config_.appName) << "\" Level=\"1\">\n";
    file << "      <ComponentRef Id=\"MainComponent\" />\n";
    if (config_.createDesktopShortcut) {
        file << "      <ComponentRef Id=\"DesktopShortcut\" />\n";
    }
    if (config_.addToPath) {
        file << "      <ComponentRef Id=\"PathComponent\" />\n";
    }
    file << "    </Feature>\n\n";
    
    // (AR) واجهة المستخدم
    file << "    <UIRef Id=\"WixUI_InstallDir\" />\n";
    file << "    <Property Id=\"WIXUI_INSTALLDIR\" Value=\"INSTALLDIR\" />\n";
    
    if (!config_.licensePath.empty()) {
        file << "    <WixVariable Id=\"WixUILicenseRtf\" Value=\"" << config_.licensePath << "\" />\n";
    }
    
    file << "  </Product>\n";
    file << "</Wix>\n";
    
    file.close();
    return true;
}

bool MSIGenerator::generateBuildScript(const std::string& outputPath) const {
    std::ofstream file(outputPath);
    if (!file.is_open()) return false;
    
    file << "# سكريبت بناء MSI — لغة ص / MSI Build Script — Sad Language\n";
    file << "# يتطلب: WiX Toolset v3+ / Requires: WiX Toolset v3+\n\n";
    
    file << "$WixDir = \"${env:WIX}bin\"\n";
    file << "if (-not (Test-Path $WixDir)) {\n";
    file << "    Write-Error \"WiX Toolset غير مثبت / WiX Toolset not installed\"\n";
    file << "    Write-Host \"قم بتثبيته من: https://wixtoolset.org/\"\n";
    file << "    exit 1\n";
    file << "}\n\n";
    
    std::string wixFile = config_.appName + ".wxs";
    std::string wixObj = config_.appName + ".wixobj";
    std::string msiFile = config_.appName + "-" + config_.version + ".msi";
    
    file << "Write-Host \"[1/3] ترجمة WiX XML... / Compiling WiX XML...\"\n";
    file << "& \"$WixDir\\candle.exe\" \"" << wixFile << "\" -o \"" << wixObj << "\"\n";
    file << "if ($LASTEXITCODE -ne 0) { exit 1 }\n\n";
    
    file << "Write-Host \"[2/3] ربط MSI... / Linking MSI...\"\n";
    file << "& \"$WixDir\\light.exe\" \"" << wixObj << "\" -o \"" << msiFile << "\" -ext WixUIExtension\n";
    file << "if ($LASTEXITCODE -ne 0) { exit 1 }\n\n";
    
    file << "Write-Host \"[3/3] تم إنشاء المثبت بنجاح! / Installer created successfully!\"\n";
    file << "Write-Host \"الملف: " << msiFile << "\"\n";
    
    file.close();
    return true;
}

bool MSIGenerator::generateNSIS(const std::string& outputPath) const {
    std::ofstream file(outputPath);
    if (!file.is_open()) return false;
    
    file << "; مثبت NSIS — لغة ص / NSIS Installer — Sad Language\n";
    file << "; يتطلب: NSIS 3+ / Requires: NSIS 3+\n\n";
    
    file << "Unicode true\n";
    file << "!include \"MUI2.nsh\"\n\n";
    
    file << "Name \"" << config_.appName << "\"\n";
    file << "OutFile \"" << config_.appName << "-" << config_.version << "-setup.exe\"\n";
    file << "InstallDir \"$PROGRAMFILES64\\" << config_.appName << "\"\n";
    file << "RequestExecutionLevel admin\n\n";
    
    // (AR) صفحات التثبيت
    file << "; صفحات التثبيت / Installer pages\n";
    file << "!insertmacro MUI_PAGE_WELCOME\n";
    if (!config_.licensePath.empty()) {
        file << "!insertmacro MUI_PAGE_LICENSE \"" << config_.licensePath << "\"\n";
    }
    file << "!insertmacro MUI_PAGE_DIRECTORY\n";
    file << "!insertmacro MUI_PAGE_INSTFILES\n";
    file << "!insertmacro MUI_PAGE_FINISH\n\n";
    
    file << "!insertmacro MUI_LANGUAGE \"Arabic\"\n";
    file << "!insertmacro MUI_LANGUAGE \"English\"\n\n";
    
    // (AR) قسم التثبيت
    file << "Section \"Install\"\n";
    file << "  SetOutPath $INSTDIR\n";
    file << "  File \"" << config_.exeName << "\"\n";
    
    for (const auto& f : config_.files) {
        file << "  File \"" << f << "\"\n";
    }
    
    if (config_.createDesktopShortcut) {
        file << "  CreateShortCut \"$DESKTOP\\" << config_.appName << ".lnk\" \"$INSTDIR\\" << config_.exeName << "\"\n";
    }
    
    if (config_.createStartMenu) {
        file << "  CreateDirectory \"$SMPROGRAMS\\" << config_.appName << "\"\n";
        file << "  CreateShortCut \"$SMPROGRAMS\\" << config_.appName << "\\" << config_.appName << ".lnk\" \"$INSTDIR\\" << config_.exeName << "\"\n";
    }
    
    if (config_.addToPath) {
        file << "  ; إضافة لمتغير PATH / Add to PATH\n";
        file << "  EnVar::AddValue \"PATH\" \"$INSTDIR\"\n";
    }
    
    file << "  WriteUninstaller \"$INSTDIR\\uninstall.exe\"\n";
    file << "SectionEnd\n\n";
    
    // (AR) قسم إلغاء التثبيت
    file << "Section \"Uninstall\"\n";
    file << "  Delete \"$INSTDIR\\" << config_.exeName << "\"\n";
    if (config_.createDesktopShortcut) {
        file << "  Delete \"$DESKTOP\\" << config_.appName << ".lnk\"\n";
    }
    if (config_.createStartMenu) {
        file << "  RMDir /r \"$SMPROGRAMS\\" << config_.appName << "\"\n";
    }
    if (config_.addToPath) {
        file << "  EnVar::DeleteValue \"PATH\" \"$INSTDIR\"\n";
    }
    file << "  Delete \"$INSTDIR\\uninstall.exe\"\n";
    file << "  RMDir \"$INSTDIR\"\n";
    file << "SectionEnd\n";
    
    file.close();
    return true;
}

} // namespace Installer
} // namespace Tools
} // namespace Sad
