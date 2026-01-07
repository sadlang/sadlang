/*
 * تنفيذ نظام كشف سلسلة الأدوات / Toolchain Detection System Implementation
 * ========================================================================
 */

#include "toolchain_detection.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <array>
#include <regex>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

namespace sad {

// ============================================================================
// ToolchainDetector Implementation
// ============================================================================

ToolchainDetector::ToolchainDetector()
    : detection_done_(false) {
}

ToolchainDetector::~ToolchainDetector() {
}

std::vector<ToolchainInfo> ToolchainDetector::detectAllToolchains() {
    if (detection_done_) {
        return detected_toolchains_;
    }
    
    detected_toolchains_.clear();
    
#ifdef _WIN32
    // Windows: كشف MSVC و MinGW و Clang
    if (auto msvc = detectMSVCToolchain()) {
        detected_toolchains_.push_back(*msvc);
    }
    if (auto mingw = detectMinGWToolchain()) {
        detected_toolchains_.push_back(*mingw);
    }
    if (auto clang = detectClangToolchain()) {
        detected_toolchains_.push_back(*clang);
    }
#else
    // Unix: كشف GCC و Clang
    if (auto gcc = detectGCCToolchain()) {
        detected_toolchains_.push_back(*gcc);
    }
    if (auto clang = detectClangToolchain()) {
        detected_toolchains_.push_back(*clang);
    }
#endif
    
    detection_done_ = true;
    return detected_toolchains_;
}

std::optional<ToolchainInfo> ToolchainDetector::detectToolchain(ToolchainType type) {
    switch (type) {
        case ToolchainType::MSVC:
            return detectMSVCToolchain();
        case ToolchainType::MinGW:
            return detectMinGWToolchain();
        case ToolchainType::Clang:
            return detectClangToolchain();
        case ToolchainType::GCC:
            return detectGCCToolchain();
        default:
            return std::nullopt;
    }
}

std::optional<ToolchainInfo> ToolchainDetector::getBestToolchain() {
    auto toolchains = detectAllToolchains();
    
    if (toolchains.empty()) {
        return std::nullopt;
    }
    
#ifdef _WIN32
    // Windows: تفضيل MSVC ثم Clang ثم MinGW
    for (const auto& tc : toolchains) {
        if (tc.type == ToolchainType::MSVC && tc.isValid()) {
            return tc;
        }
    }
    for (const auto& tc : toolchains) {
        if (tc.type == ToolchainType::Clang && tc.isValid()) {
            return tc;
        }
    }
    for (const auto& tc : toolchains) {
        if (tc.type == ToolchainType::MinGW && tc.isValid()) {
            return tc;
        }
    }
#else
    // Unix: تفضيل Clang ثم GCC
    for (const auto& tc : toolchains) {
        if (tc.type == ToolchainType::Clang && tc.isValid()) {
            return tc;
        }
    }
    for (const auto& tc : toolchains) {
        if (tc.type == ToolchainType::GCC && tc.isValid()) {
            return tc;
        }
    }
#endif
    
    // إرجاع أول سلسلة أدوات صالحة
    for (const auto& tc : toolchains) {
        if (tc.isValid()) {
            return tc;
        }
    }
    
    return std::nullopt;
}

std::optional<ToolchainInfo> ToolchainDetector::getToolchainForTarget(const std::string& target_triple) {
    bool is_windows = target_triple.find("windows") != std::string::npos;
    bool is_msvc = target_triple.find("msvc") != std::string::npos;
    bool is_mingw = target_triple.find("mingw") != std::string::npos;
    
    if (is_windows) {
        if (is_msvc) {
            return detectMSVCToolchain();
        } else if (is_mingw) {
            return detectMinGWToolchain();
        }
    }
    
    return getBestToolchain();
}

std::optional<MSVCInfo> ToolchainDetector::detectMSVC() {
#ifdef _WIN32
    MSVCInfo info;
    
    // البحث عن Visual Studio باستخدام vswhere
    std::string vswhere = findVSWhere();
    if (!vswhere.empty()) {
        std::string cmd = "\"" + vswhere + "\" -latest -property installationPath";
        std::string output = executeCommand(cmd);
        
        if (!output.empty()) {
            // إزالة الأسطر الجديدة
            output.erase(std::remove(output.begin(), output.end(), '\n'), output.end());
            output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());
            info.vs_install_path = output;
        }
    }
    
    // إذا لم نجد عبر vswhere، نبحث في المسارات الافتراضية
    if (info.vs_install_path.empty()) {
        std::vector<std::string> default_paths = {
            "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community",
            "C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional",
            "C:\\Program Files\\Microsoft Visual Studio\\2022\\Enterprise",
            "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community",
            "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Professional",
            "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Enterprise"
        };
        
        for (const auto& path : default_paths) {
            if (fileExists(path + "\\VC\\Auxiliary\\Build\\vcvars64.bat")) {
                info.vs_install_path = path;
                break;
            }
        }
    }
    
    if (info.vs_install_path.empty()) {
        return std::nullopt;
    }
    
    // البحث عن أدوات VC
    std::string vc_path = info.vs_install_path + "\\VC\\Tools\\MSVC";
    if (std::filesystem::exists(vc_path)) {
        // البحث عن أحدث إصدار
        std::string latest_version;
        for (const auto& entry : std::filesystem::directory_iterator(vc_path)) {
            if (entry.is_directory()) {
                std::string version = entry.path().filename().string();
                if (latest_version.empty() || version > latest_version) {
                    latest_version = version;
                }
            }
        }
        if (!latest_version.empty()) {
            info.vc_tools_path = vc_path + "\\" + latest_version;
        }
    }
    
    // البحث عن Windows SDK
    info.windows_sdk_path = findWindowsSDK();
    
    // تحديد ملفات vcvars
    info.vcvars64_path = info.vs_install_path + "\\VC\\Auxiliary\\Build\\vcvars64.bat";
    info.vcvars32_path = info.vs_install_path + "\\VC\\Auxiliary\\Build\\vcvars32.bat";
    
    if (info.isValid()) {
        msvc_info_ = info;
        return info;
    }
#endif
    
    return std::nullopt;
}

std::string ToolchainDetector::getEnvironmentSetupCommand(const ToolchainInfo& toolchain) {
#ifdef _WIN32
    if (toolchain.type == ToolchainType::MSVC) {
        if (msvc_info_ && fileExists(msvc_info_->vcvars64_path)) {
            return "call \"" + msvc_info_->vcvars64_path + "\"";
        }
    }
#endif
    return "";
}

bool ToolchainDetector::setupEnvironment(const ToolchainInfo& toolchain) {
#ifdef _WIN32
    if (toolchain.type == ToolchainType::MSVC) {
        // إعداد متغيرات البيئة لـ MSVC
        for (const auto& [key, value] : toolchain.env_vars) {
            SetEnvironmentVariableA(key.c_str(), value.c_str());
        }
        
        // إضافة مسارات الأدوات إلى PATH
        std::string path;
        char* current_path = std::getenv("PATH");
        if (current_path) {
            path = current_path;
        }
        
        for (const auto& bin_path : toolchain.bin_paths) {
            path = bin_path + ";" + path;
        }
        
        SetEnvironmentVariableA("PATH", path.c_str());
        return true;
    }
#endif
    return false;
}

std::string ToolchainDetector::findTool(const std::string& tool_name, 
                                        const std::vector<std::string>& search_paths) {
    std::vector<std::string> paths = search_paths;
    
    // إضافة PATH من البيئة
    char* env_path = std::getenv("PATH");
    if (env_path) {
        std::string path_str = env_path;
        std::stringstream ss(path_str);
        std::string item;
#ifdef _WIN32
        char delimiter = ';';
#else
        char delimiter = ':';
#endif
        while (std::getline(ss, item, delimiter)) {
            paths.push_back(item);
        }
    }
    
    // البحث عن الأداة
    for (const auto& path : paths) {
        std::string full_path = path + "/" + tool_name;
#ifdef _WIN32
        // تجربة مع .exe
        if (fileExists(full_path + ".exe")) {
            return full_path + ".exe";
        }
        // تجربة مع backslash
        full_path = path + "\\" + tool_name;
        if (fileExists(full_path + ".exe")) {
            return full_path + ".exe";
        }
#else
        if (fileExists(full_path)) {
            return full_path;
        }
#endif
    }
    
    return "";
}

bool ToolchainDetector::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

VersionInfo ToolchainDetector::readToolVersion(const std::string& tool_path, 
                                               const std::string& version_flag) {
    VersionInfo info;
    
    std::string cmd = "\"" + tool_path + "\" " + version_flag + " 2>&1";
    std::string output = executeCommand(cmd);
    
    // محاولة استخراج الإصدار باستخدام regex
    std::regex version_regex("(\\d+)\\.(\\d+)(?:\\.(\\d+))?");
    std::smatch match;
    
    if (std::regex_search(output, match, version_regex)) {
        info.major = std::stoi(match[1].str());
        info.minor = std::stoi(match[2].str());
        if (match[3].matched) {
            info.patch = std::stoi(match[3].str());
        }
        info.full_version = match[0].str();
    }
    
    return info;
}

void ToolchainDetector::printDetectedToolchains() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "سلاسل الأدوات المكتشفة / Detected Toolchains" << std::endl;
    std::cout << "========================================" << std::endl;
    
    if (detected_toolchains_.empty()) {
        std::cout << "لم يتم العثور على سلاسل أدوات / No toolchains found" << std::endl;
    } else {
        for (const auto& tc : detected_toolchains_) {
            std::cout << "\n" << tc.getTypeString() << " " << tc.version.toString() << std::endl;
            std::cout << "  المسار / Path: " << tc.install_path << std::endl;
            std::cout << "  المترجم / Compiler: " << tc.compiler.path << std::endl;
            std::cout << "  الرابط / Linker: " << tc.linker.path << std::endl;
            std::cout << "  صالح / Valid: " << (tc.isValid() ? "نعم/Yes" : "لا/No") << std::endl;
        }
    }
    
    std::cout << "========================================\n" << std::endl;
}

// ============================================================================
// Private Detection Methods
// ============================================================================

std::optional<ToolchainInfo> ToolchainDetector::detectMSVCToolchain() {
#ifdef _WIN32
    auto msvc = detectMSVC();
    if (!msvc) {
        return std::nullopt;
    }
    
    ToolchainInfo info;
    info.type = ToolchainType::MSVC;
    info.name = "Microsoft Visual C++";
    info.install_path = msvc->vs_install_path;
    
    // البحث عن cl.exe
    std::string cl_path = msvc->vc_tools_path + "\\bin\\Hostx64\\x64\\cl.exe";
    if (fileExists(cl_path)) {
        info.compiler.name = "cl.exe";
        info.compiler.path = cl_path;
        info.compiler.available = true;
        info.compiler.version = readToolVersion(cl_path, "");
    }
    
    // البحث عن link.exe
    std::string link_path = msvc->vc_tools_path + "\\bin\\Hostx64\\x64\\link.exe";
    if (fileExists(link_path)) {
        info.linker.name = "link.exe";
        info.linker.path = link_path;
        info.linker.available = true;
    }
    
    // البحث عن ml64.exe
    std::string ml_path = msvc->vc_tools_path + "\\bin\\Hostx64\\x64\\ml64.exe";
    if (fileExists(ml_path)) {
        info.assembler.name = "ml64.exe";
        info.assembler.path = ml_path;
        info.assembler.available = true;
    }
    
    // البحث عن lib.exe
    std::string lib_path = msvc->vc_tools_path + "\\bin\\Hostx64\\x64\\lib.exe";
    if (fileExists(lib_path)) {
        info.archiver.name = "lib.exe";
        info.archiver.path = lib_path;
        info.archiver.available = true;
    }
    
    // إضافة مسارات التضمين
    info.include_paths.push_back(msvc->vc_tools_path + "\\include");
    
    // إضافة مسارات المكتبات
    info.library_paths.push_back(msvc->vc_tools_path + "\\lib\\x64");
    
    // إضافة مسارات الثنائيات
    info.bin_paths.push_back(msvc->vc_tools_path + "\\bin\\Hostx64\\x64");
    
    // إضافة Windows SDK
    if (!msvc->windows_sdk_path.empty()) {
        // البحث عن أحدث إصدار SDK
        std::string sdk_include = msvc->windows_sdk_path + "\\Include";
        std::string sdk_lib = msvc->windows_sdk_path + "\\Lib";
        
        if (std::filesystem::exists(sdk_include)) {
            std::string latest_version;
            for (const auto& entry : std::filesystem::directory_iterator(sdk_include)) {
                if (entry.is_directory()) {
                    std::string version = entry.path().filename().string();
                    if (version.find("10.") == 0 && (latest_version.empty() || version > latest_version)) {
                        latest_version = version;
                    }
                }
            }
            
            if (!latest_version.empty()) {
                msvc->windows_sdk_version = latest_version;
                info.include_paths.push_back(sdk_include + "\\" + latest_version + "\\ucrt");
                info.include_paths.push_back(sdk_include + "\\" + latest_version + "\\um");
                info.include_paths.push_back(sdk_include + "\\" + latest_version + "\\shared");
                
                info.library_paths.push_back(sdk_lib + "\\" + latest_version + "\\ucrt\\x64");
                info.library_paths.push_back(sdk_lib + "\\" + latest_version + "\\um\\x64");
            }
        }
    }
    
    // المكتبات الافتراضية
    info.default_libs = {"kernel32.lib", "user32.lib", "gdi32.lib", "advapi32.lib"};
    info.runtime_libs = {"msvcrt.lib", "ucrt.lib", "vcruntime.lib"};
    
    // الأعلام الافتراضية للمترجم
    info.default_compiler_flags = {"/nologo", "/MD", "/EHsc"};
    
    // الأعلام الافتراضية للرابط
    info.default_linker_flags = {"/NOLOGO", "/SUBSYSTEM:CONSOLE", "/MACHINE:X64"};
    
    // متغيرات البيئة
    info.env_vars["INCLUDE"] = "";
    info.env_vars["LIB"] = "";
    
    for (const auto& inc : info.include_paths) {
        info.env_vars["INCLUDE"] += inc + ";";
    }
    for (const auto& lib : info.library_paths) {
        info.env_vars["LIB"] += lib + ";";
    }
    
    info.version = info.compiler.version;
    
    return info;
#else
    return std::nullopt;
#endif
}

std::optional<ToolchainInfo> ToolchainDetector::detectMinGWToolchain() {
#ifdef _WIN32
    ToolchainInfo info;
    info.type = ToolchainType::MinGW;
    info.name = "MinGW-w64";
    
    // مسارات البحث الشائعة لـ MinGW
    std::vector<std::string> search_paths = {
        "C:\\mingw-w64\\mingw64\\bin",
        "C:\\msys64\\mingw64\\bin",
        "C:\\w64devkit\\bin",
        "C:\\mingw64\\bin",
        "C:\\TDM-GCC-64\\bin"
    };
    
    std::string gcc_path = findTool("gcc", search_paths);
    if (gcc_path.empty()) {
        gcc_path = findTool("x86_64-w64-mingw32-gcc", search_paths);
    }
    
    if (gcc_path.empty()) {
        return std::nullopt;
    }
    
    // استخراج مسار التثبيت
    std::filesystem::path p(gcc_path);
    info.install_path = p.parent_path().parent_path().string();
    
    info.compiler.name = "gcc";
    info.compiler.path = gcc_path;
    info.compiler.available = true;
    info.compiler.version = readToolVersion(gcc_path, "--version");
    
    // البحث عن ld
    std::string ld_path = findTool("ld", {p.parent_path().string()});
    if (ld_path.empty()) {
        ld_path = findTool("x86_64-w64-mingw32-ld", {p.parent_path().string()});
    }
    if (!ld_path.empty()) {
        info.linker.name = "ld";
        info.linker.path = ld_path;
        info.linker.available = true;
    }
    
    // البحث عن ar
    std::string ar_path = findTool("ar", {p.parent_path().string()});
    if (!ar_path.empty()) {
        info.archiver.name = "ar";
        info.archiver.path = ar_path;
        info.archiver.available = true;
    }
    
    // المسارات
    std::string base = p.parent_path().parent_path().string();
    info.include_paths = {base + "\\include", base + "\\x86_64-w64-mingw32\\include"};
    info.library_paths = {base + "\\lib", base + "\\x86_64-w64-mingw32\\lib"};
    info.bin_paths = {p.parent_path().string()};
    
    // المكتبات الافتراضية
    info.default_libs = {"mingw32", "gcc", "moldname", "mingwex", "msvcrt", "kernel32"};
    
    // الأعلام الافتراضية
    info.default_compiler_flags = {};
    info.default_linker_flags = {"-Wl,--subsystem,console"};
    
    info.version = info.compiler.version;
    
    return info;
#else
    return std::nullopt;
#endif
}

std::optional<ToolchainInfo> ToolchainDetector::detectClangToolchain() {
    ToolchainInfo info;
    info.type = ToolchainType::Clang;
    info.name = "Clang/LLVM";
    
    std::vector<std::string> search_paths;
    
#ifdef _WIN32
    search_paths = {
        "C:\\LLVM\\bin",
        "C:\\Program Files\\LLVM\\bin",
        "C:\\Program Files (x86)\\LLVM\\bin"
    };
#else
    search_paths = {
        "/usr/bin",
        "/usr/local/bin",
        "/opt/homebrew/bin"
    };
#endif
    
    std::string clang_path = findTool("clang", search_paths);
    if (clang_path.empty()) {
        return std::nullopt;
    }
    
    std::filesystem::path p(clang_path);
    info.install_path = p.parent_path().parent_path().string();
    
    info.compiler.name = "clang";
    info.compiler.path = clang_path;
    info.compiler.available = true;
    info.compiler.version = readToolVersion(clang_path, "--version");
    
    // البحث عن lld-link (Windows) أو ld.lld (Unix)
#ifdef _WIN32
    std::string lld_path = findTool("lld-link", {p.parent_path().string()});
#else
    std::string lld_path = findTool("ld.lld", {p.parent_path().string()});
#endif
    
    if (!lld_path.empty()) {
        info.linker.name = "lld";
        info.linker.path = lld_path;
        info.linker.available = true;
    }
    
    // البحث عن llvm-ar
    std::string ar_path = findTool("llvm-ar", {p.parent_path().string()});
    if (!ar_path.empty()) {
        info.archiver.name = "llvm-ar";
        info.archiver.path = ar_path;
        info.archiver.available = true;
    }
    
    info.bin_paths = {p.parent_path().string()};
    
#ifdef _WIN32
    info.default_libs = {"kernel32", "user32", "msvcrt"};
    info.default_linker_flags = {"/SUBSYSTEM:CONSOLE"};
#else
    info.default_libs = {"c", "m"};
    info.default_linker_flags = {};
#endif
    
    info.version = info.compiler.version;
    
    return info;
}

std::optional<ToolchainInfo> ToolchainDetector::detectGCCToolchain() {
#ifndef _WIN32
    ToolchainInfo info;
    info.type = ToolchainType::GCC;
    info.name = "GNU Compiler Collection";
    
    std::string gcc_path = findTool("gcc", {"/usr/bin", "/usr/local/bin"});
    if (gcc_path.empty()) {
        return std::nullopt;
    }
    
    info.install_path = "/usr";
    
    info.compiler.name = "gcc";
    info.compiler.path = gcc_path;
    info.compiler.available = true;
    info.compiler.version = readToolVersion(gcc_path, "--version");
    
    std::string ld_path = findTool("ld", {"/usr/bin"});
    if (!ld_path.empty()) {
        info.linker.name = "ld";
        info.linker.path = ld_path;
        info.linker.available = true;
    }
    
    std::string ar_path = findTool("ar", {"/usr/bin"});
    if (!ar_path.empty()) {
        info.archiver.name = "ar";
        info.archiver.path = ar_path;
        info.archiver.available = true;
    }
    
    info.include_paths = {"/usr/include", "/usr/local/include"};
    info.library_paths = {"/usr/lib", "/usr/local/lib", "/lib/x86_64-linux-gnu"};
    info.bin_paths = {"/usr/bin"};
    
    info.default_libs = {"c", "m", "pthread", "dl"};
    info.default_linker_flags = {"-pie"};
    
    info.version = info.compiler.version;
    
    return info;
#else
    return std::nullopt;
#endif
}

std::string ToolchainDetector::findVSWhere() {
#ifdef _WIN32
    // البحث في المسار الافتراضي
    std::string vswhere = "C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe";
    if (fileExists(vswhere)) {
        return vswhere;
    }
    
    // البحث في PATH
    return findTool("vswhere", {});
#else
    return "";
#endif
}

std::string ToolchainDetector::executeCommand(const std::string& command) {
    std::string result;
    
#ifdef _WIN32
    FILE* pipe = _popen(command.c_str(), "r");
#else
    FILE* pipe = popen(command.c_str(), "r");
#endif
    
    if (pipe) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        
#ifdef _WIN32
        _pclose(pipe);
#else
        pclose(pipe);
#endif
    }
    
    return result;
}

std::string ToolchainDetector::findWindowsSDK() {
#ifdef _WIN32
    // البحث في المسار الافتراضي
    std::string sdk_path = "C:\\Program Files (x86)\\Windows Kits\\10";
    if (fileExists(sdk_path)) {
        return sdk_path;
    }
    
    // محاولة قراءة من Registry
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
                      "SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots",
                      0, KEY_READ | KEY_WOW64_32KEY, &hKey) == ERROR_SUCCESS) {
        char buffer[512];
        DWORD bufferSize = sizeof(buffer);
        if (RegQueryValueExA(hKey, "KitsRoot10", nullptr, nullptr, 
                             (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return std::string(buffer);
        }
        RegCloseKey(hKey);
    }
#endif
    
    return "";
}

// ============================================================================
// ToolchainManager Implementation
// ============================================================================

ToolchainManager& ToolchainManager::getInstance() {
    static ToolchainManager instance;
    return instance;
}

ToolchainManager::ToolchainManager()
    : initialized_(false) {
}

ToolchainManager::~ToolchainManager() {
}

bool ToolchainManager::initialize() {
    if (initialized_) {
        return true;
    }
    
    detector_ = std::make_unique<ToolchainDetector>();
    
    auto best = detector_->getBestToolchain();
    if (best) {
        active_toolchain_ = *best;
        initialized_ = true;
        return true;
    }
    
    return false;
}

const ToolchainInfo* ToolchainManager::getActiveToolchain() const {
    if (active_toolchain_) {
        return &(*active_toolchain_);
    }
    return nullptr;
}

bool ToolchainManager::setActiveToolchain(ToolchainType type) {
    if (!detector_) {
        return false;
    }
    
    auto tc = detector_->detectToolchain(type);
    if (tc && tc->isValid()) {
        active_toolchain_ = *tc;
        return true;
    }
    
    return false;
}

std::string ToolchainManager::getLinkerPath() const {
    if (active_toolchain_ && active_toolchain_->linker.available) {
        return active_toolchain_->linker.path;
    }
    return "";
}

std::vector<std::string> ToolchainManager::getLinkerFlags() const {
    if (active_toolchain_) {
        return active_toolchain_->default_linker_flags;
    }
    return {};
}

std::vector<std::string> ToolchainManager::getDefaultLibraries() const {
    if (active_toolchain_) {
        std::vector<std::string> libs = active_toolchain_->default_libs;
        libs.insert(libs.end(), active_toolchain_->runtime_libs.begin(), 
                    active_toolchain_->runtime_libs.end());
        return libs;
    }
    return {};
}

std::vector<std::string> ToolchainManager::getLibraryPaths() const {
    if (active_toolchain_) {
        return active_toolchain_->library_paths;
    }
    return {};
}

std::string ToolchainManager::buildLinkCommand(const std::vector<std::string>& object_files,
                                               const std::string& output_file,
                                               const std::vector<std::string>& additional_libs) {
    if (!active_toolchain_ || !active_toolchain_->linker.available) {
        return "";
    }
    
    std::ostringstream cmd;
    
    // المسار للرابط
    cmd << "\"" << active_toolchain_->linker.path << "\"";
    
    if (active_toolchain_->type == ToolchainType::MSVC) {
        // MSVC linker
        cmd << " /NOLOGO";
        cmd << " /OUT:\"" << output_file << "\"";
        
        // ملفات الكائنات
        for (const auto& obj : object_files) {
            cmd << " \"" << obj << "\"";
        }
        
        // مسارات المكتبات
        for (const auto& path : active_toolchain_->library_paths) {
            cmd << " /LIBPATH:\"" << path << "\"";
        }
        
        // المكتبات الافتراضية
        for (const auto& lib : active_toolchain_->default_libs) {
            cmd << " " << lib;
        }
        for (const auto& lib : active_toolchain_->runtime_libs) {
            cmd << " " << lib;
        }
        
        // المكتبات الإضافية
        for (const auto& lib : additional_libs) {
            cmd << " " << lib;
        }
        
        // الأعلام
        for (const auto& flag : active_toolchain_->default_linker_flags) {
            cmd << " " << flag;
        }
    } else {
        // Unix-like linker
        cmd << " -o \"" << output_file << "\"";
        
        // ملفات الكائنات
        for (const auto& obj : object_files) {
            cmd << " \"" << obj << "\"";
        }
        
        // مسارات المكتبات
        for (const auto& path : active_toolchain_->library_paths) {
            cmd << " -L\"" << path << "\"";
        }
        
        // المكتبات
        for (const auto& lib : active_toolchain_->default_libs) {
            cmd << " -l" << lib;
        }
        
        // المكتبات الإضافية
        for (const auto& lib : additional_libs) {
            cmd << " -l" << lib;
        }
        
        // الأعلام
        for (const auto& flag : active_toolchain_->default_linker_flags) {
            cmd << " " << flag;
        }
    }
    
    return cmd.str();
}

bool ToolchainManager::executeLink(const std::vector<std::string>& object_files,
                                   const std::string& output_file,
                                   const std::vector<std::string>& additional_libs) {
    std::string cmd = buildLinkCommand(object_files, output_file, additional_libs);
    if (cmd.empty()) {
        return false;
    }
    
    int result = std::system(cmd.c_str());
    return result == 0;
}

} // namespace sad
