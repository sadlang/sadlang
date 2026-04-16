/**
 * =============================================================================
 * ملف: arm64_codegen.cpp
 * الوصف: مولد كود ARM64 المتكامل مع LLVM
 * (AR) @brief مولد الكود الأصلي لمعمارية ARM64/AArch64 عبر LLVM
 * (EN) @brief Native code generator for ARM64/AArch64 via LLVM cross-compilation
 * المهمة: T301 - ARM64 cross-compilation via LLVM backend
 * =============================================================================
 */

#ifdef HAS_LLVM

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/TargetParser/Triple.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/TargetParser/Host.h>

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace sad::compiler {

// =============================================================================
// (AR) دالة مساعدة: تحقق من سلامة المسار قبل تمريره لشل
// (EN) Helper: validate path safety before passing to shell
// =============================================================================
static bool isPathSafeForShell(const std::string& path) {
    // (AR) رفض المسارات التي تحتوي أحرف خطيرة (حقن أوامر)
    // (EN) Reject paths with dangerous characters (command injection)
    for (char c : path) {
        if (c == ';' || c == '|' || c == '&' || c == '`' ||
            c == '$' || c == '(' || c == ')' || c == '\n' || c == '\r') {
            return false;
        }
    }
    return !path.empty();
}

static std::string quotePathForShell(const std::string& path) {
    // (AR) لف المسار بعلامات اقتباس مع هروب الأقواس الداخلية
    // (EN) Quote path, escaping inner quotes
    std::string result = "\"";
    for (char c : path) {
        if (c == '"') result += "\\\"";
        else result += c;
    }
    result += "\"";
    return result;
}

// =============================================================================
// تكوينات ARM64 المدعومة
// =============================================================================

/**
 * (AR) أنواع منصات ARM64 المدعومة
 * (EN) Supported ARM64 platform types
 */
enum class ARM64Platform {
    Linux_Generic,      // Linux عام (RPi, Graviton)
    Linux_RPi4,         // Raspberry Pi 4/5
    Linux_Graviton,     // AWS Graviton 2/3
    MacOS_AppleSilicon, // Apple M1/M2/M3
    Android_ARM64,      // Android NDK aarch64
    iOS_ARM64,          // iOS/iPadOS
    BareMetal,          // بدون نظام تشغيل
    Windows_ARM64       // Windows on ARM
};

/**
 * (AR) تكوين الهدف ARM64
 * (EN) ARM64 target configuration
 */
struct ARM64TargetConfig {
    ARM64Platform platform;
    std::string triple;
    std::string cpu;
    std::vector<std::string> features;
    std::string linker;
    std::string sysroot;
    bool pic = true;
    bool lto = false;
    int opt_level = 2;  // O0-O3
    
    /**
     * (AR) إنشاء تكوين لمنصة محددة
     * (EN) Create config for a specific platform
     */
    static ARM64TargetConfig forPlatform(ARM64Platform platform) {
        ARM64TargetConfig config;
        config.platform = platform;
        
        switch (platform) {
            case ARM64Platform::Linux_Generic:
                config.triple = "aarch64-unknown-linux-gnu";
                config.cpu = "generic";
                config.features = {"+neon", "+crypto"};
                config.linker = "aarch64-linux-gnu-gcc";
                break;
                
            case ARM64Platform::Linux_RPi4:
                config.triple = "aarch64-unknown-linux-gnu";
                config.cpu = "cortex-a72";
                config.features = {"+neon", "+crypto", "+crc"};
                config.linker = "aarch64-linux-gnu-gcc";
                break;
                
            case ARM64Platform::Linux_Graviton:
                config.triple = "aarch64-unknown-linux-gnu";
                config.cpu = "neoverse-n1";
                config.features = {"+neon", "+crypto", "+lse", "+sve"};
                config.linker = "aarch64-linux-gnu-gcc";
                break;
                
            case ARM64Platform::MacOS_AppleSilicon:
                config.triple = "arm64-apple-macosx13.0.0";
                config.cpu = "apple-m1";
                config.features = {"+neon", "+crypto", "+fp-armv8"};
                config.linker = "clang";
                break;
                
            case ARM64Platform::Android_ARM64:
                config.triple = "aarch64-linux-android24";
                config.cpu = "generic";
                config.features = {"+neon"};
                config.linker = "aarch64-linux-android24-clang";
                break;
                
            case ARM64Platform::iOS_ARM64:
                config.triple = "arm64-apple-ios15.0.0";
                config.cpu = "apple-a15";
                config.features = {"+neon", "+crypto", "+fp-armv8"};
                config.linker = "clang";
                break;
                
            case ARM64Platform::BareMetal:
                config.triple = "aarch64-unknown-none-elf";
                config.cpu = "cortex-a53";
                config.features = {"+neon"};
                config.linker = "aarch64-none-elf-gcc";
                config.pic = false;
                break;
                
            case ARM64Platform::Windows_ARM64:
                config.triple = "aarch64-pc-windows-msvc";
                config.cpu = "generic";
                config.features = {"+neon"};
                config.linker = "lld-link";
                break;
        }
        
        return config;
    }
    
    /**
     * (AR) الحصول على اسم المنصة
     * (EN) Get platform display name
     */
    std::string platformName() const {
        switch (platform) {
            case ARM64Platform::Linux_Generic:      return "Linux ARM64 (عام)";
            case ARM64Platform::Linux_RPi4:         return "Raspberry Pi 4/5";
            case ARM64Platform::Linux_Graviton:     return "AWS Graviton";
            case ARM64Platform::MacOS_AppleSilicon: return "macOS Apple Silicon";
            case ARM64Platform::Android_ARM64:      return "Android ARM64";
            case ARM64Platform::iOS_ARM64:          return "iOS ARM64";
            case ARM64Platform::BareMetal:          return "Bare Metal ARM64";
            case ARM64Platform::Windows_ARM64:      return "Windows ARM64";
        }
        return "مجهول";
    }
};

// =============================================================================
// مولد كود ARM64
// =============================================================================

/**
 * (AR) مولد كود ARM64 عبر LLVM
 * (EN) ARM64 code generator via LLVM backend
 */
class ARM64CodeGenerator {
public:
    ARM64CodeGenerator() = default;
    
    /**
     * (AR) تهيئة المولد لمنصة محددة
     * (EN) Initialize generator for specific platform
     */
    bool initialize(ARM64Platform platform) {
        config_ = ARM64TargetConfig::forPlatform(platform);
        return initializeLLVM();
    }
    
    /**
     * (AR) تهيئة المولد بتكوين مخصص
     * (EN) Initialize with custom config
     */
    bool initialize(const ARM64TargetConfig& config) {
        config_ = config;
        return initializeLLVM();
    }
    
    /**
     * (AR) ترجمة وحدة LLVM IR إلى ملف كائن
     * (EN) Compile LLVM IR module to object file
     */
    bool compileToObject(llvm::Module& module, const std::string& outputPath) {
        if (!target_machine_) {
            std::cerr << "خطأ: لم يتم تهيئة آلة الهدف" << std::endl;
            return false;
        }
        
        // تحديث ثلاثية الهدف في الوحدة
        module.setTargetTriple(config_.triple);
        module.setDataLayout(target_machine_->createDataLayout());
        
        // التحقق من صحة الوحدة
        std::string verify_errors;
        llvm::raw_string_ostream verify_os(verify_errors);
        if (llvm::verifyModule(module, &verify_os)) {
            std::cerr << "خطأ في التحقق من الوحدة: " << verify_errors << std::endl;
            return false;
        }
        
        // فتح ملف الإخراج
        std::error_code ec;
        llvm::raw_fd_ostream dest(outputPath, ec, llvm::sys::fs::OF_None);
        if (ec) {
            std::cerr << "خطأ في فتح ملف الإخراج: " << ec.message() << std::endl;
            return false;
        }
        
        // تمرير مولد الكود
        llvm::legacy::PassManager pass;
        if (target_machine_->addPassesToEmitFile(
                pass, dest, nullptr, llvm::CodeGenFileType::ObjectFile)) {
            std::cerr << "خطأ: لا يمكن إصدار ملف كائن لهذا الهدف" << std::endl;
            return false;
        }
        
        pass.run(module);
        dest.flush();
        
        std::cout << "✅ تم ترجمة " << outputPath << " لـ " 
                  << config_.platformName() << std::endl;
        return true;
    }
    
    /**
     * (AR) ترجمة إلى تجميع ARM64
     * (EN) Compile to ARM64 assembly
     */
    bool compileToAssembly(llvm::Module& module, const std::string& outputPath) {
        if (!target_machine_) return false;
        
        module.setTargetTriple(config_.triple);
        module.setDataLayout(target_machine_->createDataLayout());
        
        std::error_code ec;
        llvm::raw_fd_ostream dest(outputPath, ec, llvm::sys::fs::OF_None);
        if (ec) return false;
        
        llvm::legacy::PassManager pass;
        if (target_machine_->addPassesToEmitFile(
                pass, dest, nullptr, llvm::CodeGenFileType::AssemblyFile)) {
            return false;
        }
        
        pass.run(module);
        dest.flush();
        
        std::cout << "✅ تم إنشاء " << outputPath << " (تجميع ARM64)" << std::endl;
        return true;
    }
    
    /**
     * (AR) ربط ملفات الكائنات إلى ملف تنفيذي
     * (EN) Link object files into executable
     */
    bool linkExecutable(const std::vector<std::string>& objectFiles,
                       const std::string& outputPath,
                       const std::vector<std::string>& libraries = {}) {
        std::string cmd = config_.linker;
        
        // إضافة sysroot إن وجد
        if (!config_.sysroot.empty()) {
            cmd += " --sysroot=" + config_.sysroot;
        }
        
        // إضافة ملفات الكائنات
        for (const auto& obj : objectFiles) {
            cmd += " " + obj;
        }
        
        // إضافة المكتبات
        for (const auto& lib : libraries) {
            cmd += " -l" + lib;
        }
        
        // ملف الإخراج
        cmd += " -o " + outputPath;
        
        // خيارات خاصة بالمنصة
        switch (config_.platform) {
            case ARM64Platform::MacOS_AppleSilicon:
                cmd += " -arch arm64 -target " + config_.triple;
                break;
            case ARM64Platform::iOS_ARM64:
                cmd += " -arch arm64 -target " + config_.triple;
                cmd += " -isysroot $(xcrun --sdk iphoneos --show-sdk-path)";
                break;
            case ARM64Platform::Android_ARM64:
                cmd += " -target " + config_.triple;
                if (!config_.sysroot.empty()) {
                    cmd += " --sysroot=" + config_.sysroot;
                }
                break;
            default:
                if (config_.pic) {
                    cmd += " -pie";
                }
                break;
        }
        
        std::cout << "🔗 ربط: " << cmd << std::endl;
        
        // (AR) تحقق من سلامة المسارات قبل التنفيذ
        // (EN) Validate path safety before execution
        if (!isPathSafeForShell(outputPath)) {
            std::cerr << "❌ مسار الإخراج يحتوي أحرف غير آمنة: " << outputPath << std::endl;
            return false;
        }
        for (const auto& obj : objectFiles) {
            if (!isPathSafeForShell(obj)) {
                std::cerr << "❌ مسار ملف كائن يحتوي أحرف غير آمنة: " << obj << std::endl;
                return false;
            }
        }
        
        int result = std::system(cmd.c_str());
        
        if (result == 0) {
            std::cout << "✅ تم إنشاء " << outputPath << std::endl;
            return true;
        } else {
            std::cerr << "❌ فشل الربط (الكود: " << result << ")" << std::endl;
            return false;
        }
    }
    
    /**
     * (AR) ترجمة وربط مباشرة
     * (EN) Compile and link in one step
     */
    bool compileAndLink(llvm::Module& module,
                       const std::string& outputPath,
                       const std::vector<std::string>& libraries = {}) {
        std::string objPath = outputPath + ".o";
        
        if (!compileToObject(module, objPath)) {
            return false;
        }
        
        bool result = linkExecutable({objPath}, outputPath, libraries);
        
        // حذف ملف الكائن المؤقت
        std::filesystem::remove(objPath);
        
        return result;
    }
    
    /**
     * (AR) الحصول على التكوين الحالي
     * (EN) Get current configuration
     */
    const ARM64TargetConfig& getConfig() const { return config_; }
    
    /**
     * (AR) طباعة معلومات المنصة
     * (EN) Print platform info
     */
    void printInfo() const {
        std::cout << "═══════════════════════════════════════\n";
        std::cout << "  هدف ARM64: " << config_.platformName() << "\n";
        std::cout << "  الثلاثية: " << config_.triple << "\n";
        std::cout << "  المعالج: " << config_.cpu << "\n";
        std::cout << "  الميزات: ";
        for (size_t i = 0; i < config_.features.size(); i++) {
            if (i > 0) std::cout << ", ";
            std::cout << config_.features[i];
        }
        std::cout << "\n";
        std::cout << "  الرابط: " << config_.linker << "\n";
        std::cout << "═══════════════════════════════════════\n";
    }

private:
    ARM64TargetConfig config_;
    std::unique_ptr<llvm::TargetMachine> target_machine_;
    
    /**
     * (AR) تهيئة LLVM للهدف ARM64
     * (EN) Initialize LLVM for ARM64 target
     */
    bool initializeLLVM() {
        // تهيئة جميع الأهداف
        llvm::InitializeAllTargetInfos();
        llvm::InitializeAllTargets();
        llvm::InitializeAllTargetMCs();
        llvm::InitializeAllAsmParsers();
        llvm::InitializeAllAsmPrinters();
        
        // البحث عن الهدف
        std::string error;
        const llvm::Target* target = llvm::TargetRegistry::lookupTarget(
            config_.triple, error);
        
        if (!target) {
            std::cerr << "خطأ: هدف ARM64 غير متوفر: " << error << std::endl;
            return false;
        }
        
        // تجميع الميزات
        std::string features_str;
        for (size_t i = 0; i < config_.features.size(); i++) {
            if (i > 0) features_str += ",";
            features_str += config_.features[i];
        }
        
        // تحديد مستوى التحسين
        llvm::CodeGenOptLevel opt;
        switch (config_.opt_level) {
            case 0:  opt = llvm::CodeGenOptLevel::None; break;
            case 1:  opt = llvm::CodeGenOptLevel::Less; break;
            case 3:  opt = llvm::CodeGenOptLevel::Aggressive; break;
            default: opt = llvm::CodeGenOptLevel::Default; break;
        }
        
        // نموذج إعادة التوطين
        auto reloc = config_.pic ? llvm::Reloc::PIC_ : llvm::Reloc::Static;
        
        // إنشاء آلة الهدف
        llvm::TargetOptions target_opts;
        target_machine_.reset(target->createTargetMachine(
            config_.triple,
            config_.cpu,
            features_str,
            target_opts,
            reloc,
            llvm::CodeModel::Small,
            opt
        ));
        
        if (!target_machine_) {
            std::cerr << "خطأ: فشل إنشاء آلة الهدف ARM64" << std::endl;
            return false;
        }
        
        return true;
    }
};

// =============================================================================
// واجهة CLI لترجمة ARM64
// =============================================================================

/**
 * (AR) معالجة أوامر ARM64 من سطر الأوامر
 * (EN) Handle ARM64 commands from CLI
 */
struct ARM64CLIHandler {
    /**
     * (AR) تحليل اسم المنصة من نص
     * (EN) Parse platform name from string
     */
    static ARM64Platform parsePlatform(const std::string& name) {
        if (name == "linux" || name == "لينكس")
            return ARM64Platform::Linux_Generic;
        if (name == "rpi" || name == "rpi4" || name == "راسبيري")
            return ARM64Platform::Linux_RPi4;
        if (name == "graviton" || name == "aws")
            return ARM64Platform::Linux_Graviton;
        if (name == "macos" || name == "mac" || name == "ماك")
            return ARM64Platform::MacOS_AppleSilicon;
        if (name == "android" || name == "أندرويد")
            return ARM64Platform::Android_ARM64;
        if (name == "ios" || name == "آيفون")
            return ARM64Platform::iOS_ARM64;
        if (name == "baremetal" || name == "مدمج")
            return ARM64Platform::BareMetal;
        if (name == "windows" || name == "ويندوز")
            return ARM64Platform::Windows_ARM64;
        
        return ARM64Platform::Linux_Generic;  // الافتراضي
    }
    
    /**
     * (AR) طباعة المنصات المدعومة
     * (EN) Print supported platforms
     */
    static void printSupportedPlatforms() {
        std::cout << "منصات ARM64 المدعومة:\n";
        std::cout << "  linux    | لينكس     — Linux ARM64 عام\n";
        std::cout << "  rpi4     | راسبيري   — Raspberry Pi 4/5\n";
        std::cout << "  graviton | aws       — AWS Graviton\n";
        std::cout << "  macos    | ماك       — macOS Apple Silicon\n";
        std::cout << "  android  | أندرويد   — Android ARM64\n";
        std::cout << "  ios      | آيفون     — iOS ARM64\n";
        std::cout << "  baremetal| مدمج      — بدون نظام تشغيل\n";
        std::cout << "  windows  | ويندوز    — Windows ARM64\n";
    }
};

} // namespace sad::compiler

#endif // HAS_LLVM
