/*
 * نظام كشف سلسلة الأدوات / Toolchain Detection System
 * ====================================================
 * 
 * هذا الملف يوفر نظاماً شاملاً لكشف وإدارة سلاسل الأدوات المتاحة.
 * This file provides a comprehensive system for detecting and managing available toolchains.
 * 
 * سلاسل الأدوات المدعومة / Supported Toolchains:
 * - MSVC (Microsoft Visual C++)
 * - MinGW-w64
 * - Clang/LLVM
 * - GCC
 * 
 * الوظائف الرئيسية / Main Functions:
 * - كشف تلقائي لسلاسل الأدوات المثبتة / Auto-detect installed toolchains
 * - اختيار أفضل سلسلة أدوات متاحة / Select best available toolchain
 * - إدارة مسارات الأدوات / Manage tool paths
 * - دعم متعدد المنصات / Cross-platform support
 */

#ifndef TOOLCHAIN_DETECTION_H
#define TOOLCHAIN_DETECTION_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>
#include <filesystem>

namespace sad {

/**
 * نوع سلسلة الأدوات / Toolchain Type
 */
enum class ToolchainType {
    Unknown,
    MSVC,           // Microsoft Visual C++
    MinGW,          // MinGW-w64
    Clang,          // Clang/LLVM
    GCC,            // GNU Compiler Collection
    AppleClang      // Apple Clang (macOS)
};

/**
 * معلومات الإصدار / Version Information
 */
struct VersionInfo {
    int major;
    int minor;
    int patch;
    std::string full_version;
    
    VersionInfo() : major(0), minor(0), patch(0) {}
    VersionInfo(int maj, int min, int pat) : major(maj), minor(min), patch(pat) {
        full_version = std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
    }
    
    bool operator>=(const VersionInfo& other) const {
        if (major != other.major) return major > other.major;
        if (minor != other.minor) return minor > other.minor;
        return patch >= other.patch;
    }
    
    std::string toString() const { return full_version; }
};

/**
 * معلومات أداة / Tool Information
 */
struct ToolInfo {
    std::string name;                   // اسم الأداة / Tool name
    std::string path;                   // المسار الكامل / Full path
    VersionInfo version;                // الإصدار / Version
    bool available;                     // متاحة؟ / Available?
    
    ToolInfo() : available(false) {}
};

/**
 * معلومات سلسلة الأدوات / Toolchain Information
 */
struct ToolchainInfo {
    ToolchainType type;                 // النوع / Type
    std::string name;                   // الاسم / Name
    VersionInfo version;                // الإصدار / Version
    std::string install_path;           // مسار التثبيت / Installation path
    
    // الأدوات / Tools
    ToolInfo compiler;                  // المترجم / Compiler
    ToolInfo linker;                    // الرابط / Linker
    ToolInfo assembler;                 // المجمّع / Assembler
    ToolInfo archiver;                  // الأرشيف / Archiver
    
    // المسارات / Paths
    std::vector<std::string> include_paths;     // مسارات التضمين / Include paths
    std::vector<std::string> library_paths;     // مسارات المكتبات / Library paths
    std::vector<std::string> bin_paths;         // مسارات الثنائيات / Binary paths
    
    // المكتبات الافتراضية / Default libraries
    std::vector<std::string> default_libs;
    std::vector<std::string> runtime_libs;
    
    // الأعلام الافتراضية / Default flags
    std::vector<std::string> default_compiler_flags;
    std::vector<std::string> default_linker_flags;
    
    // متغيرات البيئة المطلوبة / Required environment variables
    std::unordered_map<std::string, std::string> env_vars;
    
    bool isValid() const {
        return type != ToolchainType::Unknown && 
               compiler.available && 
               linker.available;
    }
    
    std::string getTypeString() const {
        switch (type) {
            case ToolchainType::MSVC: return "MSVC";
            case ToolchainType::MinGW: return "MinGW";
            case ToolchainType::Clang: return "Clang";
            case ToolchainType::GCC: return "GCC";
            case ToolchainType::AppleClang: return "AppleClang";
            default: return "Unknown";
        }
    }
};

/**
 * معلومات MSVC المحددة / MSVC-Specific Information
 */
struct MSVCInfo {
    std::string vs_install_path;        // مسار Visual Studio
    std::string vc_tools_path;          // مسار أدوات VC
    std::string windows_sdk_path;       // مسار Windows SDK
    std::string windows_sdk_version;    // إصدار Windows SDK
    std::string ucrt_path;              // مسار UCRT
    
    // ملفات vcvars
    std::string vcvars64_path;
    std::string vcvars32_path;
    
    bool isValid() const {
        return !vs_install_path.empty() && !vc_tools_path.empty();
    }
};

/**
 * نظام كشف سلسلة الأدوات / Toolchain Detection System
 */
class ToolchainDetector {
public:
    /**
     * المُنشئ / Constructor
     */
    ToolchainDetector();
    
    /**
     * المُدمر / Destructor
     */
    ~ToolchainDetector();
    
    /**
     * كشف جميع سلاسل الأدوات المتاحة / Detect all available toolchains
     * @return قائمة سلاسل الأدوات / List of toolchains
     */
    std::vector<ToolchainInfo> detectAllToolchains();
    
    /**
     * كشف سلسلة أدوات محددة / Detect specific toolchain
     * @param type نوع سلسلة الأدوات / Toolchain type
     * @return معلومات سلسلة الأدوات / Toolchain information
     */
    std::optional<ToolchainInfo> detectToolchain(ToolchainType type);
    
    /**
     * الحصول على أفضل سلسلة أدوات / Get best available toolchain
     * @return أفضل سلسلة أدوات متاحة / Best available toolchain
     */
    std::optional<ToolchainInfo> getBestToolchain();
    
    /**
     * الحصول على سلسلة أدوات للهدف / Get toolchain for target
     * @param target_triple ثلاثية الهدف / Target triple
     * @return سلسلة الأدوات المناسبة / Appropriate toolchain
     */
    std::optional<ToolchainInfo> getToolchainForTarget(const std::string& target_triple);
    
    /**
     * كشف MSVC / Detect MSVC
     * @return معلومات MSVC / MSVC information
     */
    std::optional<MSVCInfo> detectMSVC();
    
    /**
     * الحصول على أمر تهيئة البيئة / Get environment setup command
     * @param toolchain سلسلة الأدوات / Toolchain
     * @return أمر التهيئة / Setup command
     */
    std::string getEnvironmentSetupCommand(const ToolchainInfo& toolchain);
    
    /**
     * إعداد البيئة / Setup environment
     * @param toolchain سلسلة الأدوات / Toolchain
     * @return true إذا نجح الإعداد / true if setup succeeded
     */
    bool setupEnvironment(const ToolchainInfo& toolchain);
    
    /**
     * البحث عن أداة / Find tool
     * @param tool_name اسم الأداة / Tool name
     * @param search_paths مسارات البحث / Search paths
     * @return مسار الأداة / Tool path
     */
    static std::string findTool(const std::string& tool_name, 
                                const std::vector<std::string>& search_paths = {});
    
    /**
     * التحقق من وجود ملف / Check if file exists
     * @param path المسار / Path
     * @return true إذا كان الملف موجوداً / true if file exists
     */
    static bool fileExists(const std::string& path);
    
    /**
     * قراءة إصدار الأداة / Read tool version
     * @param tool_path مسار الأداة / Tool path
     * @param version_flag علم الإصدار / Version flag
     * @return معلومات الإصدار / Version info
     */
    static VersionInfo readToolVersion(const std::string& tool_path, 
                                       const std::string& version_flag = "--version");
    
    /**
     * طباعة سلاسل الأدوات المكتشفة / Print detected toolchains
     */
    void printDetectedToolchains() const;

private:
    /**
     * كشف MSVC / Detect MSVC
     */
    std::optional<ToolchainInfo> detectMSVCToolchain();
    
    /**
     * كشف MinGW / Detect MinGW
     */
    std::optional<ToolchainInfo> detectMinGWToolchain();
    
    /**
     * كشف Clang / Detect Clang
     */
    std::optional<ToolchainInfo> detectClangToolchain();
    
    /**
     * كشف GCC / Detect GCC
     */
    std::optional<ToolchainInfo> detectGCCToolchain();
    
    /**
     * البحث في Registry عن Visual Studio / Search Registry for VS
     */
    std::vector<std::string> findVSInstallations();
    
    /**
     * البحث عن vswhere.exe / Find vswhere.exe
     */
    std::string findVSWhere();
    
    /**
     * تنفيذ أمر والحصول على الإخراج / Execute command and get output
     */
    static std::string executeCommand(const std::string& command);
    
    /**
     * تحليل إخراج vswhere / Parse vswhere output
     */
    std::vector<std::string> parseVSWhereOutput(const std::string& output);
    
    /**
     * البحث عن Windows SDK / Find Windows SDK
     */
    std::string findWindowsSDK();
    
    /**
     * الحصول على مسارات البحث الافتراضية / Get default search paths
     */
    std::vector<std::string> getDefaultSearchPaths();
    
    /**
     * قراءة PATH من البيئة / Read PATH from environment
     */
    std::vector<std::string> getPathFromEnvironment();

private:
    std::vector<ToolchainInfo> detected_toolchains_;
    std::optional<MSVCInfo> msvc_info_;
    bool detection_done_;
};

/**
 * مدير سلسلة الأدوات / Toolchain Manager
 * 
 * يوفر واجهة موحدة لإدارة سلاسل الأدوات
 * Provides unified interface for toolchain management
 */
class ToolchainManager {
public:
    /**
     * الحصول على المثيل الوحيد / Get singleton instance
     */
    static ToolchainManager& getInstance();
    
    /**
     * تهيئة المدير / Initialize manager
     */
    bool initialize();
    
    /**
     * الحصول على سلسلة الأدوات النشطة / Get active toolchain
     */
    const ToolchainInfo* getActiveToolchain() const;
    
    /**
     * تعيين سلسلة الأدوات النشطة / Set active toolchain
     */
    bool setActiveToolchain(ToolchainType type);
    
    /**
     * الحصول على مسار الرابط / Get linker path
     */
    std::string getLinkerPath() const;
    
    /**
     * الحصول على أعلام الرابط / Get linker flags
     */
    std::vector<std::string> getLinkerFlags() const;
    
    /**
     * الحصول على المكتبات الافتراضية / Get default libraries
     */
    std::vector<std::string> getDefaultLibraries() const;
    
    /**
     * الحصول على مسارات المكتبات / Get library paths
     */
    std::vector<std::string> getLibraryPaths() const;
    
    /**
     * بناء أمر الربط / Build link command
     * @param object_files ملفات الكائنات / Object files
     * @param output_file ملف الإخراج / Output file
     * @param additional_libs مكتبات إضافية / Additional libraries
     * @return أمر الربط / Link command
     */
    std::string buildLinkCommand(const std::vector<std::string>& object_files,
                                 const std::string& output_file,
                                 const std::vector<std::string>& additional_libs = {});
    
    /**
     * تنفيذ الربط / Execute linking
     */
    bool executeLink(const std::vector<std::string>& object_files,
                     const std::string& output_file,
                     const std::vector<std::string>& additional_libs = {});

private:
    ToolchainManager();
    ~ToolchainManager();
    
    ToolchainManager(const ToolchainManager&) = delete;
    ToolchainManager& operator=(const ToolchainManager&) = delete;
    
private:
    std::unique_ptr<ToolchainDetector> detector_;
    std::optional<ToolchainInfo> active_toolchain_;
    bool initialized_;
};

} // namespace sad

#endif // TOOLCHAIN_DETECTION_H
