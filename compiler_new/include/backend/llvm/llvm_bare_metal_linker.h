// ════════════════════════════════════════════════════════════════════════════════
// ملف: llvm_bare_metal_linker.h
// File: llvm_bare_metal_linker.h
//
// الوصف: رابط bare-metal للنواة — يدعم -nostdlib، سكربتات الربط، والأقسام المخصصة
// Description: Bare-metal linker for kernel — supports -nostdlib, linker scripts, custom sections
//
// المؤلف: Sad Compiler — Phase ز
// Author: Sad Compiler — Phase ز
//
// التاريخ: فبراير 2026
// Date: February 2026
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_COMPILER_LLVM_BARE_METAL_LINKER_H
#define SAD_COMPILER_LLVM_BARE_METAL_LINKER_H

#include <string>
#include <vector>
#include <map>

namespace sad {

// ════════════════════════════════════════════════════════════════════════════════
// تعداد صيغ الإخراج / Output Format Enumeration
// ════════════════════════════════════════════════════════════════════════════════

enum class BareMetalOutputFormat {
    ELF,           // ملف ELF قياسي / Standard ELF file
    FlatBinary,    // ثنائي خام (مثل objcopy -O binary) / Raw binary
    HEX,           // Intel HEX للمتحكمات / Intel HEX for microcontrollers
    UEFI           // تطبيق UEFI PE32+ / UEFI PE32+ application
};

// ════════════════════════════════════════════════════════════════════════════════
// تعداد أهداف Bare-Metal / Bare-Metal Target Enumeration
// ════════════════════════════════════════════════════════════════════════════════

enum class BareMetalTarget {
    X86_64_Bare,       // x86_64 بدون نظام / x86_64 freestanding
    X86_32_Bare,       // i686 بدون نظام / i686 freestanding
    ARM_CortexM,       // ARM Cortex-M (Thumb) / ARM Cortex-M
    ARM_CortexA,       // ARM Cortex-A / ARM Cortex-A
    RISCV32_Bare,      // RISC-V 32 بدون نظام / RISC-V 32 freestanding
    RISCV64_Bare,      // RISC-V 64 بدون نظام / RISC-V 64 freestanding
    AARCH64_Bare       // AArch64 بدون نظام / AArch64 freestanding
};

// ════════════════════════════════════════════════════════════════════════════════
// بنية خيارات الربط المعدني / Bare-Metal Linking Options
// ════════════════════════════════════════════════════════════════════════════════

struct BareMetalLinkOptions {
    // الهدف / Target
    BareMetalTarget target = BareMetalTarget::X86_64_Bare;
    std::string target_triple = "x86_64-unknown-none";
    
    // صيغة الإخراج / Output format
    BareMetalOutputFormat output_format = BareMetalOutputFormat::ELF;
    
    // ملفات / Files
    std::string output_file = "kernel.elf";
    std::vector<std::string> object_files;
    std::string linker_script;         // مسار سكربت ربط مخصص / Custom linker script path
    bool auto_generate_script = true;  // توليد سكربت تلقائي إذا لم يُوفَّر / Auto-generate if not provided
    
    // نقطة الدخول / Entry point
    std::string entry_point = "_start";
    
    // خيارات / Options
    bool strip_symbols = false;        // إزالة الرموز / Strip symbols
    bool gc_sections = true;           // حذف الأقسام غير المستخدمة / Remove unused sections
    bool generate_map = false;         // إنشاء ملف خريطة / Generate map file
    std::string map_file;              // مسار ملف الخريطة / Map file path
    bool verbose = false;              // طباعة تفصيلية / Verbose output
    bool relocatable = false;          // ملف قابل لإعادة التموضع / Relocatable output
    
    // علامات ربط إضافية / Extra linker flags
    std::vector<std::string> extra_flags;
    
    // مكتبات ثابتة إضافية (مثل libgcc) / Extra static libraries (e.g., libgcc)
    std::vector<std::string> static_libraries;
    std::vector<std::string> library_paths;
};

// ════════════════════════════════════════════════════════════════════════════════
// بنية نتيجة الربط / Linking Result
// ════════════════════════════════════════════════════════════════════════════════

struct BareMetalLinkResult {
    bool success = false;
    std::string output_file;
    size_t output_size = 0;           // حجم الملف الناتج بالبايت / Output file size in bytes
    double linking_time_ms = 0.0;     // وقت الربط بالملي ثانية / Linking time in ms
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    std::string generated_script;     // السكربت المُولد (إن وجد) / Generated script (if any)
};

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: LLVMBareMetalLinker
// Class: LLVMBareMetalLinker
//
// الوصف: رابط bare-metal يدعم أهداف النواة بدون نظام تشغيل
// Description: Bare-metal linker supporting kernel targets without OS
// ════════════════════════════════════════════════════════════════════════════════

class LLVMBareMetalLinker {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئ والمُدمّر / Constructor and Destructor
    // ═══════════════════════════════════════════════════════════════════════════
    
    LLVMBareMetalLinker();
    ~LLVMBareMetalLinker();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // التهيئة / Initialization
    // ═══════════════════════════════════════════════════════════════════════════
    
    // تهيئة لهدف محدد / Initialize for a specific target
    bool initialize(BareMetalTarget target);
    bool initialize(const std::string& target_triple);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الإعدادات / Configuration
    // ═══════════════════════════════════════════════════════════════════════════
    
    void setOutputFile(const std::string& path);
    void setOutputFormat(BareMetalOutputFormat format);
    void setLinkerScript(const std::string& path);
    void setEntryPoint(const std::string& entry);
    void setAutoGenerateScript(bool enable);
    void setStripSymbols(bool strip);
    void setGCSections(bool gc);
    void setGenerateMap(bool enable, const std::string& mapFile = "");
    void setVerbose(bool verbose);
    
    // إضافة ملفات / Add files
    void addObjectFile(const std::string& path);
    void addObjectFiles(const std::vector<std::string>& paths);
    void addStaticLibrary(const std::string& path);
    void addLibraryPath(const std::string& path);
    void addLinkerFlag(const std::string& flag);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الربط / Linking
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الربط الرئيسي / Main linking
    BareMetalLinkResult link();
    BareMetalLinkResult link(const BareMetalLinkOptions& options);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // المعلومات / Information
    // ═══════════════════════════════════════════════════════════════════════════
    
    const BareMetalLinkResult& getLastResult() const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال مساعدة ثابتة / Static Helper Functions
    // ═══════════════════════════════════════════════════════════════════════════
    
    // تحويل triple إلى هدف / Convert triple to target
    static BareMetalTarget targetFromTriple(const std::string& triple);
    
    // التحقق من أن الهدف bare-metal / Check if target is bare-metal
    static bool isBareMetalTriple(const std::string& triple);
    
    // الحصول على triple الافتراضي لهدف / Get default triple for target
    static std::string getDefaultTriple(BareMetalTarget target);
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // التنفيذ الداخلي / Internal Implementation
    // ═══════════════════════════════════════════════════════════════════════════
    
    // بناء سطر أوامر الربط / Build linker command line
    std::string buildCommandLine(const BareMetalLinkOptions& opts);
    
    // تنفيذ الرابط / Execute linker
    bool executeLinker(const std::string& command);
    
    // توليد سكربت ربط تلقائي / Auto-generate linker script
    std::string generateLinkerScript(const BareMetalLinkOptions& opts);
    
    // تحويل ELF إلى ثنائي خام / Convert ELF to raw binary
    bool convertToFlatBinary(const std::string& elfPath, const std::string& binPath);
    
    // تحويل ELF إلى Intel HEX / Convert ELF to Intel HEX
    bool convertToHex(const std::string& elfPath, const std::string& hexPath);
    
    // التحقق من وجود الملفات / Verify files exist
    bool verifyFiles(const BareMetalLinkOptions& opts);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء / Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    BareMetalLinkOptions options_;
    BareMetalLinkResult lastResult_;
    bool initialized_;
};

} // namespace sad

#endif // SAD_COMPILER_LLVM_BARE_METAL_LINKER_H
