/**
 * =============================================================================
 * ملف: cross_config.cpp
 * الوصف: تكوين الترجمة المتعددة المنصات
 * المهمة: T236 - Cross-compilation config
 * المرحلة: Phase 24 - User Story 21 (Cross-Platform Build --كل-الأهداف)
 * =============================================================================
 * 
 * ⚙️ دليل المبتدئ لتكوين الترجمة المتعددة
 * ══════════════════════════════════════════
 * 
 * ما هو ملف التكوين؟
 * ───────────────────
 * ملف يحدد إعدادات الترجمة لكل منصة.
 * يخبر المترجم أين يجد الأدوات والمكتبات.
 * 
 * مثال ملف sad-targets.toml:
 * ────────────────────────────
 * 
 * [windows-x64]
 * toolchain = "msvc"
 * sdk_path = "C:\\Program Files\\Windows Kits\\10"
 * 
 * [linux-x64]
 * toolchain = "gcc"
 * sysroot = "/usr/x86_64-linux-gnu"
 * 
 * [android-arm64]
 * toolchain = "ndk"
 * ndk_path = "C:\\Android\\ndk\\25.0.0"
 * api_level = 24
 * 
 * [wasm]
 * toolchain = "emscripten"
 * emsdk_path = "C:\\emsdk"
 * 
 * خيارات كل منصة:
 * ─────────────────
 * - toolchain: نوع سلسلة الأدوات
 * - sdk_path: مسار SDK
 * - sysroot: المجلد الجذر للنظام الهدف
 * - cc: مترجم C
 * - cxx: مترجم C++
 * - linker: الرابط
 * - ar: أرشيف
 * - strip: أداة التجريد
 * - flags: خيارات إضافية
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

namespace sad::targets {

// Forward declarations
enum class Architecture;
enum class OperatingSystem;
struct TargetTriple;

// =============================================================================
// نوع سلسلة الأدوات
// =============================================================================

enum class ToolchainType {
    Unknown,
    GCC,        // GNU Compiler Collection
    Clang,      // LLVM Clang
    MSVC,       // Microsoft Visual C++
    MinGW,      // MinGW-w64
    NDK,        // Android NDK
    XCode,      // Apple Xcode
    Emscripten  // Emscripten (WASM)
};

// =============================================================================
// تكوين سلسلة الأدوات
// =============================================================================

struct ToolchainConfig {
    ToolchainType type = ToolchainType::Unknown;
    std::string cc;          // مترجم C
    std::string cxx;         // مترجم C++
    std::string linker;      // الرابط
    std::string ar;          // أداة الأرشيف
    std::string strip;       // أداة التجريد
    std::string objcopy;     // نسخ الكائنات
    std::string sysroot;     // المجلد الجذر
    std::string sdkPath;     // مسار SDK
    
    std::vector<std::string> cflags;      // خيارات C
    std::vector<std::string> cxxflags;    // خيارات C++
    std::vector<std::string> ldflags;     // خيارات الربط
    std::vector<std::string> includePaths; // مسارات التضمين
    std::vector<std::string> libPaths;     // مسارات المكتبات
    
    /**
     * هل التكوين صالح؟
     */
    bool isValid() const {
        return !cc.empty() || !cxx.empty();
    }
    
    /**
     * الحصول على جميع خيارات C
     */
    std::vector<std::string> getAllCFlags() const {
        std::vector<std::string> all = cflags;
        
        if (!sysroot.empty()) {
            all.push_back("--sysroot=" + sysroot);
        }
        
        for (const auto& inc : includePaths) {
            all.push_back("-I" + inc);
        }
        
        return all;
    }
    
    /**
     * الحصول على جميع خيارات الربط
     */
    std::vector<std::string> getAllLDFlags() const {
        std::vector<std::string> all = ldflags;
        
        for (const auto& lib : libPaths) {
            all.push_back("-L" + lib);
        }
        
        return all;
    }
};

// =============================================================================
// تكوين هدف واحد
// =============================================================================

struct TargetConfig {
    std::string name;              // اسم الهدف
    ToolchainConfig toolchain;     // تكوين الأدوات
    
    // Android specific
    int androidApiLevel = 24;
    std::string ndkPath;
    
    // iOS specific
    std::string iosSDKVersion;
    bool iosSimulator = false;
    
    // WASM specific
    std::string emsdkPath;
    bool wasmThreads = false;
    bool wasmSIMD = false;
    
    // Windows specific
    std::string windowsSDKVersion;
    std::string vcToolsVersion;
    
    // تحسينات
    int optimizationLevel = 2;     // -O2
    bool debugSymbols = false;     // -g
    bool lto = false;              // Link-Time Optimization
    bool stripBinary = true;       // تجريد الرموز
    
    /**
     * تطبيق التكوين الافتراضي
     */
    void applyDefaults();
};

// =============================================================================
// قارئ التكوين
// =============================================================================

class ConfigReader {
public:
    /**
     * قراءة من ملف TOML
     */
    std::map<std::string, TargetConfig> readTOML(const std::string& path) {
        std::map<std::string, TargetConfig> configs;
        
        std::ifstream file(path);
        if (!file.is_open()) {
            return configs;
        }
        
        std::string line;
        std::string currentSection;
        TargetConfig currentConfig;
        
        while (std::getline(file, line)) {
            // تجاهل الفارغ والتعليقات
            if (line.empty() || line[0] == '#') continue;
            
            // تنظيف المسافات
            line = trim(line);
            
            // قسم جديد [target]
            if (line[0] == '[' && line.back() == ']') {
                // حفظ القسم السابق
                if (!currentSection.empty()) {
                    currentConfig.applyDefaults();
                    configs[currentSection] = currentConfig;
                }
                
                currentSection = line.substr(1, line.size() - 2);
                currentConfig = TargetConfig();
                currentConfig.name = currentSection;
            }
            // خاصية key = value
            else {
                size_t eq = line.find('=');
                if (eq != std::string::npos) {
                    std::string key = trim(line.substr(0, eq));
                    std::string value = trim(line.substr(eq + 1));
                    
                    // إزالة علامات الاقتباس
                    if (value.size() >= 2 && value[0] == '"' && value.back() == '"') {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    applyProperty(currentConfig, key, value);
                }
            }
        }
        
        // حفظ القسم الأخير
        if (!currentSection.empty()) {
            currentConfig.applyDefaults();
            configs[currentSection] = currentConfig;
        }
        
        return configs;
    }
    
    /**
     * قراءة من JSON
     */
    std::map<std::string, TargetConfig> readJSON(const std::string& path) {
        // سيتم تنفيذه
        return {};
    }
    
private:
    std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        size_t end = s.find_last_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        return s.substr(start, end - start + 1);
    }
    
    void applyProperty(TargetConfig& config, const std::string& key, const std::string& value) {
        if (key == "toolchain") {
            config.toolchain.type = parseToolchainType(value);
        }
        else if (key == "cc") {
            config.toolchain.cc = value;
        }
        else if (key == "cxx") {
            config.toolchain.cxx = value;
        }
        else if (key == "linker") {
            config.toolchain.linker = value;
        }
        else if (key == "ar") {
            config.toolchain.ar = value;
        }
        else if (key == "sysroot") {
            config.toolchain.sysroot = value;
        }
        else if (key == "sdk_path") {
            config.toolchain.sdkPath = value;
        }
        else if (key == "ndk_path") {
            config.ndkPath = value;
        }
        else if (key == "api_level") {
            config.androidApiLevel = std::stoi(value);
        }
        else if (key == "emsdk_path") {
            config.emsdkPath = value;
        }
        else if (key == "optimization") {
            config.optimizationLevel = std::stoi(value);
        }
        else if (key == "debug") {
            config.debugSymbols = (value == "true" || value == "1");
        }
        else if (key == "lto") {
            config.lto = (value == "true" || value == "1");
        }
        else if (key == "strip") {
            config.stripBinary = (value == "true" || value == "1");
        }
    }
    
    ToolchainType parseToolchainType(const std::string& s) {
        if (s == "gcc") return ToolchainType::GCC;
        if (s == "clang") return ToolchainType::Clang;
        if (s == "msvc") return ToolchainType::MSVC;
        if (s == "mingw") return ToolchainType::MinGW;
        if (s == "ndk") return ToolchainType::NDK;
        if (s == "xcode") return ToolchainType::XCode;
        if (s == "emscripten") return ToolchainType::Emscripten;
        return ToolchainType::Unknown;
    }
};

// =============================================================================
// كاتب التكوين
// =============================================================================

class ConfigWriter {
public:
    /**
     * كتابة ملف TOML
     */
    void writeTOML(
        const std::map<std::string, TargetConfig>& configs,
        const std::string& path
    ) {
        std::ofstream file(path);
        
        file << "# تكوين الترجمة المتعددة المنصات للغة ص\n";
        file << "# تم التوليد تلقائياً\n\n";
        
        for (const auto& [name, config] : configs) {
            file << "[" << name << "]\n";
            
            if (config.toolchain.type != ToolchainType::Unknown) {
                file << "toolchain = \"" << toolchainTypeToString(config.toolchain.type) << "\"\n";
            }
            
            if (!config.toolchain.cc.empty()) {
                file << "cc = \"" << config.toolchain.cc << "\"\n";
            }
            
            if (!config.toolchain.cxx.empty()) {
                file << "cxx = \"" << config.toolchain.cxx << "\"\n";
            }
            
            if (!config.toolchain.sysroot.empty()) {
                file << "sysroot = \"" << config.toolchain.sysroot << "\"\n";
            }
            
            if (!config.ndkPath.empty()) {
                file << "ndk_path = \"" << config.ndkPath << "\"\n";
                file << "api_level = " << config.androidApiLevel << "\n";
            }
            
            if (!config.emsdkPath.empty()) {
                file << "emsdk_path = \"" << config.emsdkPath << "\"\n";
            }
            
            file << "optimization = " << config.optimizationLevel << "\n";
            file << "debug = " << (config.debugSymbols ? "true" : "false") << "\n";
            file << "lto = " << (config.lto ? "true" : "false") << "\n";
            file << "strip = " << (config.stripBinary ? "true" : "false") << "\n";
            
            file << "\n";
        }
    }
    
    /**
     * توليد تكوين افتراضي
     */
    std::map<std::string, TargetConfig> generateDefaults() {
        std::map<std::string, TargetConfig> configs;
        
        // Windows x64
        TargetConfig winX64;
        winX64.name = "windows-x64";
        winX64.toolchain.type = ToolchainType::MSVC;
        winX64.toolchain.cc = "cl.exe";
        winX64.toolchain.cxx = "cl.exe";
        winX64.toolchain.linker = "link.exe";
        configs["windows-x64"] = winX64;
        
        // Linux x64
        TargetConfig linuxX64;
        linuxX64.name = "linux-x64";
        linuxX64.toolchain.type = ToolchainType::GCC;
        linuxX64.toolchain.cc = "gcc";
        linuxX64.toolchain.cxx = "g++";
        linuxX64.toolchain.linker = "ld";
        configs["linux-x64"] = linuxX64;
        
        // macOS x64
        TargetConfig macX64;
        macX64.name = "macos-x64";
        macX64.toolchain.type = ToolchainType::Clang;
        macX64.toolchain.cc = "clang";
        macX64.toolchain.cxx = "clang++";
        configs["macos-x64"] = macX64;
        
        // Android ARM64
        TargetConfig android;
        android.name = "android-arm64";
        android.toolchain.type = ToolchainType::NDK;
        android.androidApiLevel = 24;
        configs["android-arm64"] = android;
        
        // WASM
        TargetConfig wasm;
        wasm.name = "wasm";
        wasm.toolchain.type = ToolchainType::Emscripten;
        wasm.toolchain.cc = "emcc";
        wasm.toolchain.cxx = "em++";
        configs["wasm"] = wasm;
        
        return configs;
    }
    
private:
    std::string toolchainTypeToString(ToolchainType type) {
        switch (type) {
            case ToolchainType::GCC: return "gcc";
            case ToolchainType::Clang: return "clang";
            case ToolchainType::MSVC: return "msvc";
            case ToolchainType::MinGW: return "mingw";
            case ToolchainType::NDK: return "ndk";
            case ToolchainType::XCode: return "xcode";
            case ToolchainType::Emscripten: return "emscripten";
            default: return "unknown";
        }
    }
};

// =============================================================================
// مدير التكوين
// =============================================================================

class CrossConfigManager {
public:
    CrossConfigManager() {
        loadDefaults();
    }
    
    /**
     * تحميل من ملف
     */
    bool loadFromFile(const std::string& path) {
        ConfigReader reader;
        
        std::string ext = fs::path(path).extension().string();
        
        if (ext == ".toml") {
            configs_ = reader.readTOML(path);
        } else if (ext == ".json") {
            configs_ = reader.readJSON(path);
        } else {
            return false;
        }
        
        configPath_ = path;
        return !configs_.empty();
    }
    
    /**
     * حفظ إلى ملف
     */
    void saveToFile(const std::string& path) {
        ConfigWriter writer;
        writer.writeTOML(configs_, path);
    }
    
    /**
     * الحصول على تكوين هدف
     */
    TargetConfig* getConfig(const std::string& target) {
        auto it = configs_.find(target);
        if (it != configs_.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    /**
     * إضافة/تحديث تكوين
     */
    void setConfig(const std::string& target, const TargetConfig& config) {
        configs_[target] = config;
    }
    
    /**
     * قائمة الأهداف المُكونة
     */
    std::vector<std::string> getConfiguredTargets() const {
        std::vector<std::string> targets;
        for (const auto& [name, config] : configs_) {
            targets.push_back(name);
        }
        return targets;
    }
    
    /**
     * البحث عن ملف تكوين
     */
    std::string findConfigFile() {
        // البحث في المسارات المعتادة
        std::vector<std::string> searchPaths = {
            "./sad-targets.toml",
            "./.sad/targets.toml",
            "./build/targets.toml"
        };
        
        #ifdef _WIN32
        searchPaths.push_back(std::string(getenv("USERPROFILE")) + "/.sad/targets.toml");
        #else
        searchPaths.push_back(std::string(getenv("HOME")) + "/.sad/targets.toml");
        #endif
        
        for (const auto& path : searchPaths) {
            if (fs::exists(path)) {
                return path;
            }
        }
        
        return "";
    }
    
    /**
     * طباعة التكوين
     */
    void print() const {
        std::cout << "═══════════════════════════════════════════════\n";
        std::cout << "   ⚙️ تكوين الترجمة المتعددة\n";
        std::cout << "═══════════════════════════════════════════════\n\n";
        
        if (!configPath_.empty()) {
            std::cout << "📄 ملف التكوين: " << configPath_ << "\n\n";
        }
        
        for (const auto& [name, config] : configs_) {
            std::cout << "🎯 " << name << "\n";
            if (config.toolchain.isValid()) {
                std::cout << "   CC: " << config.toolchain.cc << "\n";
                std::cout << "   CXX: " << config.toolchain.cxx << "\n";
            }
            if (!config.toolchain.sysroot.empty()) {
                std::cout << "   Sysroot: " << config.toolchain.sysroot << "\n";
            }
            std::cout << "\n";
        }
    }
    
private:
    std::map<std::string, TargetConfig> configs_;
    std::string configPath_;
    
    void loadDefaults() {
        ConfigWriter writer;
        configs_ = writer.generateDefaults();
    }
};

// Implementation
void TargetConfig::applyDefaults() {
    // تطبيق الإعدادات الافتراضية بناءً على نوع سلسلة الأدوات
    switch (toolchain.type) {
        case ToolchainType::GCC:
            if (toolchain.cc.empty()) toolchain.cc = "gcc";
            if (toolchain.cxx.empty()) toolchain.cxx = "g++";
            if (toolchain.ar.empty()) toolchain.ar = "ar";
            if (toolchain.strip.empty()) toolchain.strip = "strip";
            break;
            
        case ToolchainType::Clang:
            if (toolchain.cc.empty()) toolchain.cc = "clang";
            if (toolchain.cxx.empty()) toolchain.cxx = "clang++";
            if (toolchain.ar.empty()) toolchain.ar = "llvm-ar";
            if (toolchain.strip.empty()) toolchain.strip = "llvm-strip";
            break;
            
        case ToolchainType::MSVC:
            if (toolchain.cc.empty()) toolchain.cc = "cl.exe";
            if (toolchain.cxx.empty()) toolchain.cxx = "cl.exe";
            if (toolchain.linker.empty()) toolchain.linker = "link.exe";
            if (toolchain.ar.empty()) toolchain.ar = "lib.exe";
            break;
            
        default:
            // (AR) نوع toolchain غير معروف — تحذير مع استخدام أسماء أدوات عامة
            // (EN) Unknown toolchain type — warn and use generic tool names
            std::cerr << "[sadc تحذير] نوع toolchain غير معالج: "
                      << static_cast<int>(toolchain.type)
                      << " — استخدام أسماء أدوات افتراضية" << std::endl;
            if (toolchain.cc.empty()) toolchain.cc = "cc";
            if (toolchain.cxx.empty()) toolchain.cxx = "c++";
            if (toolchain.ar.empty()) toolchain.ar = "ar";
            break;
    }
}

} // namespace sad::targets

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadCrossConfig {
    sad::targets::CrossConfigManager* manager;
};

SadCrossConfig* sad_cross_config_new() {
    auto* ctx = new (std::nothrow) SadCrossConfig();
    if (!ctx) return nullptr;
    ctx->manager = new (std::nothrow) sad::targets::CrossConfigManager();
    if (!ctx->manager) { delete ctx; return nullptr; }
    return ctx;
}

int sad_cross_config_load(SadCrossConfig* ctx, const char* path) {
    return ctx->manager->loadFromFile(path) ? 1 : 0;
}

void sad_cross_config_save(SadCrossConfig* ctx, const char* path) {
    ctx->manager->saveToFile(path);
}

void sad_cross_config_print(SadCrossConfig* ctx) {
    ctx->manager->print();
}

void sad_cross_config_free(SadCrossConfig* ctx) {
    if (ctx) {
        delete ctx->manager;
        delete ctx;
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef CROSS_CONFIG_TEST

#include <cassert>

void testConfigDefaults() {
    std::cout << "⚙️ اختبار الافتراضيات...\n";
    
    sad::targets::CrossConfigManager manager;
    
    auto* winConfig = manager.getConfig("windows-x64");
    assert(winConfig != nullptr);
    assert(winConfig->toolchain.type == sad::targets::ToolchainType::MSVC);
    
    auto targets = manager.getConfiguredTargets();
    assert(!targets.empty());
    
    std::cout << "   ✅ نجح\n";
}

void testConfigReadWrite() {
    std::cout << "📄 اختبار القراءة والكتابة...\n";
    
    sad::targets::ConfigWriter writer;
    auto configs = writer.generateDefaults();
    
    // كتابة
    writer.writeTOML(configs, "test_config.toml");
    
    // قراءة
    sad::targets::ConfigReader reader;
    auto loaded = reader.readTOML("test_config.toml");
    
    assert(loaded.size() == configs.size());
    
    // تنظيف
    fs::remove("test_config.toml");
    
    std::cout << "   ✅ نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات تكوين الترجمة المتعددة\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testConfigDefaults();
    testConfigReadWrite();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // CROSS_CONFIG_TEST
