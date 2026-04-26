// ======================================================================
// kernel_mode.h - وضع النواة / Kernel Mode Support
// ======================================================================
// الوصف بالعربية:
//   دعم ترجمة كود لغة ص في وضع النواة (bare metal)
//   يشمل: لا مكتبة قياسية، نقطة دخول مخصصة، نص ربط
//
// English Description:
//   Support for compiling Sad code in kernel (bare metal) mode
//   Includes: no-std, custom entry point, linker scripts
// ======================================================================

#ifndef SAD_KERNEL_MODE_H
#define SAD_KERNEL_MODE_H

#include <string>
#include <vector>

namespace Sad {
namespace Kernel {

/**
 * @brief Target triple for BeeOS / ثلاثية هدف نظام النحلة
 * 
 * (AR) الأهداف المدعومة:
 * - x86_64-نحلة-bare    → نواة BeeOS على x86_64
 * - x86_64-نحلة-نظام    → تطبيق BeeOS على x86_64
 * - aarch64-نحلة-bare   → نواة BeeOS على ARM64
 */
struct BeeTarget {
    std::string arch;       ///< البنية / architecture (x86_64, aarch64)
    std::string os;         ///< النظام / OS (نحلة / bee)
    std::string env;        ///< البيئة / environment (bare, نظام / system)
    
    BeeTarget(const std::string& a, const std::string& o, const std::string& e)
        : arch(a), os(o), env(e) {}
    
    std::string toString() const {
        return arch + "-" + os + "-" + env;
    }
    
    bool isBareMetal() const {
        return env == "bare" || env == "عاري";
    }
    
    bool isBeeOS() const {
        return os == "نحلة" || os == "bee";
    }
    
    /**
     * @brief Parse target string / تحليل نص الهدف
     */
    static BeeTarget parse(const std::string& triple) {
        size_t pos1 = triple.find('-');
        size_t pos2 = triple.find('-', pos1 + 1);
        if (pos1 == std::string::npos || pos2 == std::string::npos) {
            return BeeTarget("x86_64", "نحلة", "bare");
        }
        return BeeTarget(
            triple.substr(0, pos1),
            triple.substr(pos1 + 1, pos2 - pos1 - 1),
            triple.substr(pos2 + 1)
        );
    }
};

/**
 * @brief Kernel compilation options / خيارات ترجمة النواة
 */
struct KernelOptions {
    bool noStd;             ///< لا مكتبة قياسية / no standard library
    bool noMain;            ///< لا main / no main function (custom entry point)
    bool freestanding;      ///< مستقل / freestanding environment
    bool noDefaultLibs;     ///< لا مكتبات افتراضية / no default libraries
    bool staticLink;        ///< ربط ساكن / static linking only
    std::string entryPoint; ///< نقطة الدخول / entry point name (e.g., "_start", "نقطة_الدخول")
    std::string linkerScript; ///< نص الربط / linker script path
    BeeTarget target;       ///< الهدف / compilation target
    
    KernelOptions()
        : noStd(true), noMain(true), freestanding(true),
          noDefaultLibs(true), staticLink(true),
          entryPoint("_start"),
          target("x86_64", "نحلة", "bare") {}
};

/**
 * @brief Kernel mode compiler / مترجم وضع النواة
 * 
 * (AR) يعدّل خيارات الترجمة لتناسب وضع النواة
 * (EN) Modifies compilation options for kernel mode
 * 
 * @example صيغة ص:
 * ```
 * #![لا_مكتبة_قياسية]
 * 
 * #[نقطة_دخول]
 * دالة _start() بلا_رجوع {
 *     // تهيئة النواة
 *     نواة::تهيئة()
 *     // حلقة النواة الرئيسية
 *     بينما صحيح {
 *         نواة::معالجة_المقاطعات()
 *     }
 * }
 * 
 * #[معالج_ذعر]
 * دالة ذعر(معلومات: &معلومات_ذعر) بلا_رجوع {
 *     // إظهار رسالة خطأ
 *     شاشة::اكتب("خطأ فادح: ")
 *     شاشة::اكتب(معلومات.رسالة)
 *     توقف()
 * }
 * ```
 */
class KernelModeCompiler {
public:
    KernelModeCompiler() = default;
    KernelModeCompiler(const KernelOptions& opts) : options_(opts) {}
    
    /**
     * @brief Get LLVM target triple / الحصول على ثلاثية LLVM
     */
    std::string getLLVMTarget() const {
        if (options_.target.arch == "x86_64" || options_.target.arch == "x86-64") {
            return "x86_64-unknown-none-elf";
        }
        if (options_.target.arch == "aarch64" || options_.target.arch == "arm64") {
            return "aarch64-unknown-none-elf";
        }
        return "x86_64-unknown-none-elf";
    }
    
    /**
     * @brief Get compiler flags / الحصول على أعلام المترجم
     */
    std::vector<std::string> getCompilerFlags() const {
        std::vector<std::string> flags;
        
        if (options_.freestanding) {
            flags.push_back("-ffreestanding");
        }
        if (options_.noStd) {
            flags.push_back("-nostdlib");
        }
        if (options_.noDefaultLibs) {
            flags.push_back("-nodefaultlibs");
        }
        if (options_.staticLink) {
            flags.push_back("-static");
        }
        
        // (AR) تعطيل ميزات تعتمد على نظام التشغيل
        flags.push_back("-fno-exceptions");
        flags.push_back("-fno-rtti");
        flags.push_back("-fno-stack-protector");
        flags.push_back("-mno-red-zone");
        
        return flags;
    }
    
    /**
     * @brief Get linker flags / الحصول على أعلام الرابط
     */
    std::vector<std::string> getLinkerFlags() const {
        std::vector<std::string> flags;
        
        flags.push_back("-nostdlib");
        flags.push_back("-static");
        
        if (!options_.linkerScript.empty()) {
            flags.push_back("-T");
            flags.push_back(options_.linkerScript);
        }
        
        if (!options_.entryPoint.empty()) {
            flags.push_back("-e");
            flags.push_back(options_.entryPoint);
        }
        
        return flags;
    }
    
    /**
     * @brief Generate default linker script for BeeOS x86_64
     * (AR) توليد نص ربط افتراضي لنواة النحلة على x86_64
     */
    static std::string generateDefaultLinkerScript() {
        return R"(/* نص ربط نواة النحلة — BeeOS Kernel Linker Script */
ENTRY(_start)

SECTIONS {
    /* بدء النواة عند 1MB — العنوان التقليدي بعد BIOS */
    . = 1M;
    
    /* قسم الكود */
    .text BLOCK(4K) : ALIGN(4K) {
        *(.multiboot)
        *(.text)
        *(.text.*)
    }
    
    /* قسم البيانات للقراءة فقط */
    .rodata BLOCK(4K) : ALIGN(4K) {
        *(.rodata)
        *(.rodata.*)
    }
    
    /* قسم البيانات المهيأة */
    .data BLOCK(4K) : ALIGN(4K) {
        *(.data)
        *(.data.*)
    }
    
    /* قسم البيانات غير المهيأة */
    .bss BLOCK(4K) : ALIGN(4K) {
        *(COMMON)
        *(.bss)
        *(.bss.*)
    }
    
    /* نهاية النواة */
    _kernel_end = .;
}
)";
    }
    
    /**
     * @brief Get options / الحصول على الخيارات
     */
    const KernelOptions& getOptions() const { return options_; }
    KernelOptions& getOptions() { return options_; }
    
private:
    KernelOptions options_;
};

} // namespace Kernel
} // namespace Sad

#endif // SAD_KERNEL_MODE_H
