/**
 * =============================================================================
 * ملف: target_list.cpp
 * الوصف: قائمة المنصات المدعومة للترجمة المتعددة
 * المهمة: T235 - Target enumeration
 * المرحلة: Phase 24 - User Story 21 (Cross-Platform Build --كل-الأهداف)
 * =============================================================================
 * 
 * 🌍 دليل المبتدئ للترجمة المتعددة المنصات
 * ═════════════════════════════════════════
 * 
 * ما هي الترجمة المتعددة المنصات؟
 * ────────────────────────────────
 * Cross-compilation = ترجمة برنامج لمنصة مختلفة
 * 
 * مثال: أنت على Windows وتريد إنشاء برنامج يعمل على:
 * - Linux
 * - Mac
 * - Android
 * - iPhone
 * 
 * المنصات المدعومة:
 * ──────────────────
 * 
 * 1. x86_64 (معالجات Intel/AMD)
 *    - Windows 64-bit
 *    - Linux 64-bit
 *    - macOS Intel
 * 
 * 2. ARM64 (معالجات ARM)
 *    - macOS Apple Silicon (M1/M2/M3)
 *    - Linux ARM (Raspberry Pi 4)
 *    - Android حديث
 *    - iOS/iPadOS
 * 
 * 3. WebAssembly (متصفحات الويب)
 *    - Chrome, Firefox, Safari, Edge
 *    - Node.js
 * 
 * الأمر السحري:
 * ──────────────
 * 
 * ص بناء --كل-الأهداف main.ص
 * 
 * ينتج:
 * - main-windows-x64.exe
 * - main-linux-x64
 * - main-macos-x64
 * - main-macos-arm64
 * - main.wasm
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <cstdlib>

namespace sad::targets {

// =============================================================================
// تعداد البنية المعمارية
// =============================================================================

enum class Architecture {
    Unknown,
    X86,        // 32-bit Intel
    X86_64,     // 64-bit Intel/AMD
    ARM,        // 32-bit ARM
    ARM64,      // 64-bit ARM (AArch64)
    RISCV32,    // 32-bit RISC-V
    RISCV64,    // 64-bit RISC-V
    WASM32,     // WebAssembly 32-bit
    WASM64      // WebAssembly 64-bit (future)
};

// =============================================================================
// تعداد نظام التشغيل
// =============================================================================

enum class OperatingSystem {
    Unknown,
    Windows,
    Linux,
    MacOS,
    iOS,
    Android,
    FreeBSD,
    WebAssembly,  // Not really an OS, but...
    Bare          // No OS (embedded)
};

// =============================================================================
// تعداد بيئة التشغيل
// =============================================================================

enum class Environment {
    Unknown,
    GNU,        // GNU/Linux
    MSVC,       // Microsoft Visual C++
    Musl,       // musl libc
    Android,    // Android NDK
    None        // Freestanding
};

// =============================================================================
// الهدف الثلاثي (Triple)
// =============================================================================

struct TargetTriple {
    Architecture arch;
    OperatingSystem os;
    Environment env;
    std::string vendor;  // e.g., "apple", "pc", "unknown"
    
    /**
     * إنشاء من نص
     */
    static TargetTriple parse(const std::string& tripleStr);
    
    /**
     * تحويل لنص
     */
    std::string toString() const {
        return archToString() + "-" + vendorToString() + "-" + osToString() + "-" + envToString();
    }
    
    /**
     * اسم مختصر
     */
    std::string shortName() const {
        std::string name;
        
        switch (os) {
            case OperatingSystem::Windows: name = "windows"; break;
            case OperatingSystem::Linux: name = "linux"; break;
            case OperatingSystem::MacOS: name = "macos"; break;
            case OperatingSystem::iOS: name = "ios"; break;
            case OperatingSystem::Android: name = "android"; break;
            case OperatingSystem::WebAssembly: name = "wasm"; break;
            default: name = "unknown";
        }
        
        name += "-";
        
        switch (arch) {
            case Architecture::X86: name += "x86"; break;
            case Architecture::X86_64: name += "x64"; break;
            case Architecture::ARM: name += "arm"; break;
            case Architecture::ARM64: name += "arm64"; break;
            case Architecture::WASM32: name += "wasm32"; break;
            default: name += "unknown";
        }
        
        return name;
    }
    
    /**
     * هل 64-bit؟
     */
    bool is64Bit() const {
        return arch == Architecture::X86_64 || 
               arch == Architecture::ARM64 ||
               arch == Architecture::RISCV64 ||
               arch == Architecture::WASM64;
    }
    
    /**
     * هل محمول؟
     */
    bool isMobile() const {
        return os == OperatingSystem::iOS || 
               os == OperatingSystem::Android;
    }
    
    /**
     * هل ويب؟
     */
    bool isWeb() const {
        return arch == Architecture::WASM32 || 
               arch == Architecture::WASM64;
    }
    
    /**
     * امتداد الملف التنفيذي
     */
    std::string executableExtension() const {
        if (os == OperatingSystem::Windows) return ".exe";
        if (isWeb()) return ".wasm";
        return "";
    }
    
    /**
     * امتداد المكتبة الديناميكية
     */
    std::string sharedLibExtension() const {
        if (os == OperatingSystem::Windows) return ".dll";
        if (os == OperatingSystem::MacOS || os == OperatingSystem::iOS) return ".dylib";
        return ".so";
    }
    
private:
    std::string archToString() const {
        switch (arch) {
            case Architecture::X86: return "i686";
            case Architecture::X86_64: return "x86_64";
            case Architecture::ARM: return "arm";
            case Architecture::ARM64: return "aarch64";
            case Architecture::RISCV32: return "riscv32";
            case Architecture::RISCV64: return "riscv64";
            case Architecture::WASM32: return "wasm32";
            case Architecture::WASM64: return "wasm64";
            default: return "unknown";
        }
    }
    
    std::string vendorToString() const {
        return vendor.empty() ? "unknown" : vendor;
    }
    
    std::string osToString() const {
        switch (os) {
            case OperatingSystem::Windows: return "windows";
            case OperatingSystem::Linux: return "linux";
            case OperatingSystem::MacOS: return "darwin";
            case OperatingSystem::iOS: return "ios";
            case OperatingSystem::Android: return "android";
            case OperatingSystem::FreeBSD: return "freebsd";
            case OperatingSystem::WebAssembly: return "wasm";
            case OperatingSystem::Bare: return "none";
            default: return "unknown";
        }
    }
    
    std::string envToString() const {
        switch (env) {
            case Environment::GNU: return "gnu";
            case Environment::MSVC: return "msvc";
            case Environment::Musl: return "musl";
            case Environment::Android: return "android";
            case Environment::None: return "none";
            default: return "unknown";
        }
    }
};

// =============================================================================
// معلومات الهدف
// =============================================================================

struct TargetInfo {
    TargetTriple triple;
    std::string displayName;        // الاسم المعروض
    std::string displayNameArabic;  // الاسم بالعربية
    bool available;                 // متاح للترجمة؟
    std::string toolchainPath;      // مسار الأدوات
    std::vector<std::string> requiredTools; // الأدوات المطلوبة
    std::map<std::string, std::string> defaultFlags; // الخيارات الافتراضية
    
    /**
     * طباعة المعلومات
     */
    void print() const {
        std::cout << (available ? "✅" : "❌") << " " << displayName;
        std::cout << " (" << displayNameArabic << ")\n";
        std::cout << "   Triple: " << triple.toString() << "\n";
        if (!toolchainPath.empty()) {
            std::cout << "   Toolchain: " << toolchainPath << "\n";
        }
    }
};

// =============================================================================
// قائمة الأهداف
// =============================================================================

class TargetList {
public:
    TargetList() {
        registerBuiltinTargets();
    }
    
    /**
     * الحصول على جميع الأهداف
     */
    std::vector<TargetInfo> getAll() const {
        std::vector<TargetInfo> all;
        for (const auto& [name, info] : targets_) {
            all.push_back(info);
        }
        return all;
    }
    
    /**
     * الحصول على الأهداف المتاحة
     */
    std::vector<TargetInfo> getAvailable() const {
        std::vector<TargetInfo> available;
        for (const auto& [name, info] : targets_) {
            if (info.available) {
                available.push_back(info);
            }
        }
        return available;
    }
    
    /**
     * البحث عن هدف
     */
    TargetInfo* find(const std::string& name) {
        auto it = targets_.find(name);
        if (it != targets_.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    /**
     * إضافة هدف مخصص
     */
    void addCustomTarget(const TargetInfo& info) {
        targets_[info.triple.shortName()] = info;
    }
    
    /**
     * الهدف الحالي للنظام
     */
    TargetInfo getCurrentHost() const {
        #if defined(_WIN32)
            #if defined(_M_X64) || defined(__x86_64__)
                return *const_cast<TargetList*>(this)->find("windows-x64");
            #else
                return *const_cast<TargetList*>(this)->find("windows-x86");
            #endif
        #elif defined(__linux__)
            #if defined(__x86_64__)
                return *const_cast<TargetList*>(this)->find("linux-x64");
            #elif defined(__aarch64__)
                return *const_cast<TargetList*>(this)->find("linux-arm64");
            #endif
        #elif defined(__APPLE__)
            #if defined(__arm64__)
                return *const_cast<TargetList*>(this)->find("macos-arm64");
            #else
                return *const_cast<TargetList*>(this)->find("macos-x64");
            #endif
        #endif
        
        // افتراضي
        TargetInfo unknown;
        unknown.displayName = "Unknown Host";
        return unknown;
    }
    
    /**
     * طباعة جميع الأهداف
     */
    void printAll() const {
        std::cout << "═══════════════════════════════════════════════\n";
        std::cout << "   🌍 المنصات المدعومة\n";
        std::cout << "═══════════════════════════════════════════════\n\n";
        
        std::cout << "📱 المحمول:\n";
        printCategory({OperatingSystem::iOS, OperatingSystem::Android});
        
        std::cout << "\n💻 سطح المكتب:\n";
        printCategory({OperatingSystem::Windows, OperatingSystem::Linux, OperatingSystem::MacOS});
        
        std::cout << "\n🌐 الويب:\n";
        printCategory({OperatingSystem::WebAssembly});
    }
    
private:
    std::map<std::string, TargetInfo> targets_;
    
    void registerBuiltinTargets() {
        // Windows x64
        registerTarget({
            .triple = {Architecture::X86_64, OperatingSystem::Windows, Environment::MSVC, "pc"},
            .displayName = "Windows x64",
            .displayNameArabic = "ويندوز ٦٤ بت",
            .available = true,
            .requiredTools = {"cl.exe", "link.exe"}
        });
        
        // Windows x86
        registerTarget({
            .triple = {Architecture::X86, OperatingSystem::Windows, Environment::MSVC, "pc"},
            .displayName = "Windows x86",
            .displayNameArabic = "ويندوز ٣٢ بت",
            .available = true,
            .requiredTools = {"cl.exe", "link.exe"}
        });
        
        // Linux x64
        registerTarget({
            .triple = {Architecture::X86_64, OperatingSystem::Linux, Environment::GNU, "unknown"},
            .displayName = "Linux x64",
            .displayNameArabic = "لينكس ٦٤ بت",
            .available = true,
            .requiredTools = {"gcc", "ld"}
        });
        
        // Linux ARM64
        registerTarget({
            .triple = {Architecture::ARM64, OperatingSystem::Linux, Environment::GNU, "unknown"},
            .displayName = "Linux ARM64",
            .displayNameArabic = "لينكس آرم ٦٤",
            .available = false,
            .requiredTools = {"aarch64-linux-gnu-gcc"}
        });
        
        // macOS x64
        registerTarget({
            .triple = {Architecture::X86_64, OperatingSystem::MacOS, Environment::None, "apple"},
            .displayName = "macOS x64",
            .displayNameArabic = "ماك إنتل",
            .available = false,
            .requiredTools = {"clang", "ld64"}
        });
        
        // macOS ARM64
        registerTarget({
            .triple = {Architecture::ARM64, OperatingSystem::MacOS, Environment::None, "apple"},
            .displayName = "macOS ARM64",
            .displayNameArabic = "ماك أبل سيليكون",
            .available = false,
            .requiredTools = {"clang", "ld64"}
        });
        
        // iOS
        registerTarget({
            .triple = {Architecture::ARM64, OperatingSystem::iOS, Environment::None, "apple"},
            .displayName = "iOS",
            .displayNameArabic = "آي أو إس",
            .available = false,
            .requiredTools = {"xcrun", "clang"}
        });
        
        // Android ARM64
        // ═══════════════════════════════════════════════════════════════
        // (AR) كشف NDK تلقائياً — إذا وُجد NDK صالح، يصبح الهدف متاحاً
        // (EN) Auto-detect NDK — if valid NDK found, target becomes available
        // ═══════════════════════════════════════════════════════════════
        {
            bool android_available = false;
            std::string ndk_toolchain_path;
            
            // البحث عن NDK في متغيرات البيئة والمسارات المعروفة
            std::vector<std::string> ndk_search_paths;
            
            if (auto* p = std::getenv("ANDROID_NDK_HOME")) ndk_search_paths.push_back(p);
            if (auto* p = std::getenv("ANDROID_NDK")) ndk_search_paths.push_back(p);
            if (auto* sdk = std::getenv("ANDROID_SDK_ROOT")) {
                ndk_search_paths.push_back(std::string(sdk) + "/ndk");
                ndk_search_paths.push_back(std::string(sdk) + "/ndk-bundle");
            }
            #ifdef _WIN32
            if (auto* local = std::getenv("LOCALAPPDATA")) {
                ndk_search_paths.push_back(std::string(local) + "\\Android\\Sdk\\ndk");
            }
            ndk_search_paths.push_back("C:\\Android\\ndk");
            #else
            if (auto* home = std::getenv("HOME")) {
                ndk_search_paths.push_back(std::string(home) + "/Android/Sdk/ndk");
                ndk_search_paths.push_back(std::string(home) + "/Library/Android/sdk/ndk");
            }
            ndk_search_paths.push_back("/opt/android-ndk");
            #endif
            
            for (const auto& search_path : ndk_search_paths) {
                try {
                    if (std::filesystem::exists(search_path)) {
                        // التحقق من وجود toolchains/llvm/prebuilt
                        auto prebuilt = std::filesystem::path(search_path) / "toolchains" / "llvm" / "prebuilt";
                        if (std::filesystem::exists(prebuilt)) {
                            android_available = true;
                            ndk_toolchain_path = search_path;
                            break;
                        }
                        // قد يكون مجلد يحتوي على إصدارات فرعية
                        if (std::filesystem::is_directory(search_path)) {
                            for (const auto& entry : std::filesystem::directory_iterator(search_path)) {
                                if (entry.is_directory()) {
                                    auto sub_prebuilt = entry.path() / "toolchains" / "llvm" / "prebuilt";
                                    if (std::filesystem::exists(sub_prebuilt)) {
                                        android_available = true;
                                        ndk_toolchain_path = entry.path().string();
                                        break;
                                    }
                                }
                            }
                        }
                        if (android_available) break;
                    }
                } catch (...) {
                    // تجاهل أخطاء نظام الملفات
                }
            }
            
            registerTarget({
                .triple = {Architecture::ARM64, OperatingSystem::Android, Environment::Android, "unknown"},
                .displayName = "Android ARM64",
                .displayNameArabic = "أندرويد",
                .available = android_available,
                .toolchainPath = ndk_toolchain_path,
                .requiredTools = {"aarch64-linux-android-clang"}
            });
        }
        
        // WebAssembly
        registerTarget({
            .triple = {Architecture::WASM32, OperatingSystem::WebAssembly, Environment::None, "unknown"},
            .displayName = "WebAssembly",
            .displayNameArabic = "ويب أسمبلي",
            .available = true,
            .requiredTools = {"clang", "wasm-ld"}
        });
    }
    
    void registerTarget(const TargetInfo& info) {
        targets_[info.triple.shortName()] = info;
    }
    
    void printCategory(const std::set<OperatingSystem>& osSet) const {
        for (const auto& [name, info] : targets_) {
            if (osSet.count(info.triple.os)) {
                std::cout << "   " << (info.available ? "✅" : "⬜") << " ";
                std::cout << info.displayName << " (" << info.displayNameArabic << ")\n";
            }
        }
    }
};

// =============================================================================
// كاشف الأدوات
// =============================================================================

class ToolchainDetector {
public:
    /**
     * فحص توفر الأدوات
     */
    bool checkAvailability(TargetInfo& target) {
        bool allFound = true;
        
        for (const auto& tool : target.requiredTools) {
            if (!findTool(tool)) {
                allFound = false;
            }
        }
        
        target.available = allFound;
        return allFound;
    }
    
    /**
     * البحث عن أداة
     */
    bool findTool(const std::string& name) {
        // تحقق بسيط - يمكن توسيعه
        #ifdef _WIN32
        std::string cmd = "where " + name + " >nul 2>&1";
        #else
        std::string cmd = "which " + name + " >/dev/null 2>&1";
        #endif
        
        return system(cmd.c_str()) == 0;
    }
    
    /**
     * الحصول على مسار الأداة
     */
    std::string getToolPath(const std::string& name) {
        // سيتم تنفيذه لاحقاً
        return "";
    }
};

} // namespace sad::targets

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadTargetList {
    sad::targets::TargetList* list;
};

SadTargetList* sad_target_list_new() {
    auto* ctx = new SadTargetList();
    ctx->list = new sad::targets::TargetList();
    return ctx;
}

int sad_target_list_count(SadTargetList* ctx) {
    return static_cast<int>(ctx->list->getAll().size());
}

int sad_target_list_available_count(SadTargetList* ctx) {
    return static_cast<int>(ctx->list->getAvailable().size());
}

void sad_target_list_print(SadTargetList* ctx) {
    ctx->list->printAll();
}

void sad_target_list_free(SadTargetList* ctx) {
    if (ctx) {
        delete ctx->list;
        delete ctx;
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef TARGET_LIST_TEST

#include <cassert>

void testTargetList() {
    std::cout << "🎯 اختبار قائمة الأهداف...\n";
    
    sad::targets::TargetList list;
    
    auto all = list.getAll();
    assert(!all.empty());
    
    auto available = list.getAvailable();
    // على الأقل واحد متاح (النظام الحالي)
    
    std::cout << "   عدد الأهداف: " << all.size() << "\n";
    std::cout << "   ✅ نجح\n";
}

void testTargetTriple() {
    std::cout << "🔤 اختبار الثلاثي...\n";
    
    sad::targets::TargetTriple triple;
    triple.arch = sad::targets::Architecture::X86_64;
    triple.os = sad::targets::OperatingSystem::Windows;
    triple.env = sad::targets::Environment::MSVC;
    triple.vendor = "pc";
    
    assert(triple.is64Bit());
    assert(!triple.isMobile());
    assert(!triple.isWeb());
    assert(triple.executableExtension() == ".exe");
    
    std::cout << "   Triple: " << triple.toString() << "\n";
    std::cout << "   ✅ نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات قائمة الأهداف\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testTargetList();
    testTargetTriple();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // TARGET_LIST_TEST
