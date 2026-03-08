/**
 * @file cross_compile.h
 * @brief دعم الترجمة المتقاطعة
 * @brief (EN) Cross-compilation support
 * 
 * @author سعيد
 * @date ديسمبر 2026
 */

#ifndef SAD_CROSS_COMPILE_H
#define SAD_CROSS_COMPILE_H

#include &lt;string&gt;
#include &lt;vector&gt;
#include &lt;map&gt;
#include &lt;memory&gt;

namespace Sad {
namespace Compiler {
namespace CrossCompile {

//==============================================================================
// الأهداف المدعومة
//==============================================================================

/**
 * @brief نظام التشغيل الهدف
 */
enum class TargetOS {
    WINDOWS,        // Windows 10/11
    MACOS,          // macOS 12+
    LINUX,          // Linux (glibc 2.17+)
    ANDROID,        // Android 8.0+
    IOS,            // iOS 14+
    WASM,           // WebAssembly
    FREESTANDING    // بدون نظام تشغيل
};

/**
 * @brief معمارية المعالج
 */
enum class TargetArch {
    X86_64,         // Intel/AMD 64-bit
    AARCH64,        // ARM 64-bit
    ARM32,          // ARM 32-bit
    RISCV64,        // RISC-V 64-bit
    WASM32,         // WebAssembly 32-bit
    WASM64          // WebAssembly 64-bit (مستقبلي)
};

/**
 * @brief صيغة الملف التنفيذي
 */
enum class OutputFormat {
    ELF,            // Linux/BSD/Android
    PE,             // Windows
    MACHO,          // macOS/iOS
    WASM,           // WebAssembly module
    LLVM_BC,        // LLVM bitcode
    OBJ             // Object file
};

//==============================================================================
// إعدادات الهدف
//==============================================================================

/**
 * @brief ثلاثية الهدف (target triple)
 */
struct TargetTriple {
    TargetArch arch;
    std::string vendor;     // "pc", "apple", "unknown"
    TargetOS os;
    std::string environment; // "gnu", "musl", "msvc"
    
    /**
     * @brief تحويل إلى نص LLVM
     */
    std::string toLLVMTriple() const {
        std::string result;
        
        // المعمارية
        switch (arch) {
            case TargetArch::X86_64:  result = "x86_64-"; break;
            case TargetArch::AARCH64: result = "aarch64-"; break;
            case TargetArch::ARM32:   result = "arm-"; break;
            case TargetArch::RISCV64: result = "riscv64-"; break;
            case TargetArch::WASM32:  result = "wasm32-"; break;
            case TargetArch::WASM64:  result = "wasm64-"; break;
        }
        
        // المورد
        result += vendor + "-";
        
        // نظام التشغيل
        switch (os) {
            case TargetOS::WINDOWS: result += "windows-"; break;
            case TargetOS::MACOS:   result += "darwin"; return result;
            case TargetOS::LINUX:   result += "linux-"; break;
            case TargetOS::ANDROID: result += "android"; return result;
            case TargetOS::IOS:     result += "ios"; return result;
            case TargetOS::WASM:    result += "wasi"; return result;
            case TargetOS::FREESTANDING: result += "unknown"; return result;
        }
        
        result += environment;
        return result;
    }
    
    /**
     * @brief ثلاثيات مسبقة التعريف
     */
    static TargetTriple Windows_x64() {
        return {TargetArch::X86_64, "pc", TargetOS::WINDOWS, "msvc"};
    }
    
    static TargetTriple Linux_x64() {
        return {TargetArch::X86_64, "unknown", TargetOS::LINUX, "gnu"};
    }
    
    static TargetTriple macOS_x64() {
        return {TargetArch::X86_64, "apple", TargetOS::MACOS, ""};
    }
    
    static TargetTriple macOS_ARM64() {
        return {TargetArch::AARCH64, "apple", TargetOS::MACOS, ""};
    }
    
    static TargetTriple Android_ARM64() {
        return {TargetArch::AARCH64, "unknown", TargetOS::ANDROID, ""};
    }
    
    static TargetTriple iOS_ARM64() {
        return {TargetArch::AARCH64, "apple", TargetOS::IOS, ""};
    }
    
    static TargetTriple WASM() {
        return {TargetArch::WASM32, "unknown", TargetOS::WASM, ""};
    }
};

/**
 * @brief إعدادات الترجمة المتقاطعة
 */
struct CrossCompileOptions {
    TargetTriple target;
    
    // مسارات SDK
    std::string sysroot;            // مسار sysroot
    std::string sdkPath;            // مسار SDK (iOS/Android)
    std::string ndkPath;            // Android NDK
    
    // إعدادات الربط
    bool staticLink = false;        // ربط ساكن
    std::vector&lt;std::string&gt; libs;  // مكتبات إضافية
    std::vector&lt;std::string&gt; libPaths;
    
    // إعدادات التحسين
    int optLevel = 2;               // 0-3
    bool debugInfo = false;
    bool lto = false;               // Link-Time Optimization
    
    // إعدادات خاصة
    std::map&lt;std::string, std::string&gt; extraFlags;
};

//==============================================================================
// اكتشاف سلاسل الأدوات
//==============================================================================

/**
 * @brief معلومات سلسلة الأدوات
 */
struct Toolchain {
    std::string name;
    std::string compilerPath;       // clang, gcc
    std::string linkerPath;         // ld, lld
    std::string arPath;             // ar, llvm-ar
    std::string sysroot;
    std::vector&lt;std::string&gt; defaultFlags;
    
    bool isValid() const {
        return !compilerPath.empty();
    }
};

/**
 * @brief مكتشف سلاسل الأدوات
 */
class ToolchainDiscovery {
public:
    /**
     * @brief اكتشاف سلسلة أدوات للهدف
     */
    static Toolchain discoverFor(const TargetTriple&amp; target) {
        switch (target.os) {
            case TargetOS::WINDOWS:
                return discoverWindows(target);
            case TargetOS::MACOS:
            case TargetOS::IOS:
                return discoverApple(target);
            case TargetOS::LINUX:
                return discoverLinux(target);
            case TargetOS::ANDROID:
                return discoverAndroid(target);
            case TargetOS::WASM:
                return discoverWASM();
            default:
                return {};
        }
    }

private:
    static Toolchain discoverWindows(const TargetTriple&amp; target);
    static Toolchain discoverApple(const TargetTriple&amp; target);
    static Toolchain discoverLinux(const TargetTriple&amp; target);
    static Toolchain discoverAndroid(const TargetTriple&amp; target);
    static Toolchain discoverWASM();
};

//==============================================================================
// مترجم متقاطع
//==============================================================================

/**
 * @brief المترجم المتقاطع
 */
class CrossCompiler {
public:
    CrossCompiler(const CrossCompileOptions&amp; options)
        : options_(options) {
        toolchain_ = ToolchainDiscovery::discoverFor(options.target);
    }
    
    /**
     * @brief ترجمة ملف مصدري
     */
    bool compileFile(const std::string&amp; inputPath, 
                     const std::string&amp; outputPath) {
        if (!toolchain_.isValid()) {
            lastError_ = "سلسلة الأدوات غير متوفرة للهدف: " + 
                        options_.target.toLLVMTriple();
            return false;
        }
        
        // بناء أمر الترجمة
        std::vector&lt;std::string&gt; args;
        args.push_back(toolchain_.compilerPath);
        
        // target triple
        args.push_back("--target=" + options_.target.toLLVMTriple());
        
        // sysroot
        if (!options_.sysroot.empty()) {
            args.push_back("--sysroot=" + options_.sysroot);
        }
        
        // optimization
        args.push_back("-O" + std::to_string(options_.optLevel));
        
        // debug
        if (options_.debugInfo) {
            args.push_back("-g");
        }
        
        // output
        args.push_back("-o");
        args.push_back(outputPath);
        args.push_back(inputPath);
        
        // TODO: Execute command
        return true;
    }
    
    /**
     * @brief ربط ملفات object
     */
    bool link(const std::vector&lt;std::string&gt;&amp; objects,
              const std::string&amp; outputPath);
    
    /**
     * @brief الحصول على آخر خطأ
     */
    std::string getLastError() const { return lastError_; }

private:
    CrossCompileOptions options_;
    Toolchain toolchain_;
    std::string lastError_;
};

//==============================================================================
// واجهة CLI
//==============================================================================

/**
 * @brief أمر الترجمة المتقاطعة
 */
inline void printCrossCompileHelp() {
    std::cout &lt;&lt; R"(
استخدام الترجمة المتقاطعة:
  sadc --target=<target> [options] input.ص -o output

الأهداف المدعومة:
  x86_64-pc-windows-msvc     Windows 64-bit
  x86_64-unknown-linux-gnu   Linux 64-bit (glibc)
  x86_64-unknown-linux-musl  Linux 64-bit (musl)
  aarch64-apple-darwin       macOS ARM64
  x86_64-apple-darwin        macOS x64
  aarch64-linux-android      Android ARM64
  aarch64-apple-ios          iOS ARM64
  wasm32-unknown-wasi        WebAssembly

أمثلة:
  # ترجمة لـ Windows من Linux
  sadc --target=x86_64-pc-windows-msvc app.ص -o app.exe
  
  # ترجمة لـ Android
  sadc --target=aarch64-linux-android --ndk=/path/to/ndk app.ص -o app
  
  # ترجمة لـ WebAssembly
  sadc --target=wasm32-unknown-wasi app.ص -o app.wasm

خيارات إضافية:
  --sysroot=<path>   مسار sysroot
  --sdk=<path>       مسار SDK
  --ndk=<path>       مسار Android NDK
  --static           ربط ساكن
  --lto              تحسين وقت الربط
)" &lt;&lt; std::endl;
}

/**
 * @brief تحليل target triple من نص
 */
inline TargetTriple parseTargetTriple(const std::string&amp; triple) {
    // قائمة الأهداف المعروفة
    static const std::map&lt;std::string, TargetTriple&gt; knownTargets = {
        {"x86_64-pc-windows-msvc", TargetTriple::Windows_x64()},
        {"x86_64-unknown-linux-gnu", TargetTriple::Linux_x64()},
        {"x86_64-apple-darwin", TargetTriple::macOS_x64()},
        {"aarch64-apple-darwin", TargetTriple::macOS_ARM64()},
        {"aarch64-linux-android", TargetTriple::Android_ARM64()},
        {"aarch64-apple-ios", TargetTriple::iOS_ARM64()},
        {"wasm32-unknown-wasi", TargetTriple::WASM()}
    };
    
    auto it = knownTargets.find(triple);
    if (it != knownTargets.end()) {
        return it-&gt;second;
    }
    
    // محاولة تحليل يدوي
    // TODO: Parse arbitrary triples
    return TargetTriple::Linux_x64(); // افتراضي
}

} // namespace CrossCompile
} // namespace Compiler
} // namespace Sad

#endif // SAD_CROSS_COMPILE_H
