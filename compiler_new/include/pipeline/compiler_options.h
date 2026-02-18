/**
 * @file compiler_options.h
 * @brief (AR) خيارات المترجم — إعدادات خط أنابيب الترجمة
 *        (EN) Compiler Options — Compilation pipeline settings
 * 
 * (AR) يحتوي هذا الملف على جميع خيارات التحكم بعملية الترجمة:
 *      - مستوى التحسين (O0 - O3, Os, Oz)
 *      - أعلام الإصدار (LLVM IR، Assembly، Object)
 *      - أعلام طباعة AST/SIR/IR
 *      - إعدادات المنصة الهدف (target triple)
 *      - تحكمات FFI والربط
 * 
 * (EN) This file contains all compilation process control options:
 *      - Optimization level (O0 - O3, Os, Oz)
 *      - Emission flags (LLVM IR, Assembly, Object)
 *      - AST/SIR/IR print flags
 *      - Target platform settings (target triple)
 *      - FFI and linking controls
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2026-02-16
 * @version 2.0.0
 */

#ifndef SAD_PIPELINE_COMPILER_OPTIONS_H
#define SAD_PIPELINE_COMPILER_OPTIONS_H

#include <string>
#include <vector>

// ════════════════════════════════════════════════════════════════════════════════
//  (AR) تصريح مسبق لمستوى التحسين من llvm_optimizer.h
//  (EN) Forward declaration of OptimizationLevel from llvm_optimizer.h
// ════════════════════════════════════════════════════════════════════════════════
namespace sad {
    enum class OptimizationLevel : int;
}

namespace Sad {
namespace Compiler {

// ════════════════════════════════════════════════════════════════════════════════
//  (AR) الخيارات الرئيسية للمترجم / (EN) Main Compiler Options
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @struct CompilerOptions
 * @brief (AR) جميع خيارات المترجم في بنية واحدة — تتوافق مع ما يتوقعه
 *        LLVMCompilerPipeline من حقول وأساليب.
 *        (EN) All compiler options in a single structure — matches the fields
 *        and methods expected by LLVMCompilerPipeline.
 */
struct CompilerOptions {

    // ═══════════════════════════════════════════════════════════════════════
    //  (AR) الإعدادات الأساسية / (EN) Basic Settings
    // ═══════════════════════════════════════════════════════════════════════

    /** (AR) ملف الإدخال / (EN) Input source file */
    std::string inputFile;

    /** (AR) ملف الإخراج / (EN) Output file */
    std::string outputFile;

    /** (AR) هدف الآلة — مثل x86_64-pc-windows-msvc
     *  (EN) Target machine triple — e.g. x86_64-pc-windows-msvc */
    std::string target_triple;

    // ═══════════════════════════════════════════════════════════════════════
    //  (AR) مستوى التحسين / (EN) Optimization Level
    //  يستخدم sad::OptimizationLevel المُعرَّف في llvm_optimizer.h
    //  Uses sad::OptimizationLevel defined in llvm_optimizer.h
    // ═══════════════════════════════════════════════════════════════════════

    /** (AR) مستوى التحسين (O0–O3, Os, Oz)
     *  (EN) Optimization level (O0–O3, Os, Oz)
     *  القيمة الافتراضية O0 = 0 */
    sad::OptimizationLevel optimization_level =
        static_cast<sad::OptimizationLevel>(0);  // O0

    /** (AR) تفعيل التحسين التلقائي في CodeGen
     *  (EN) Enable automatic optimization in CodeGen */
    bool auto_optimize = false;

    // ═══════════════════════════════════════════════════════════════════════
    //  (AR) أعلام فحص الأنواع / (EN) Type Checking Flags
    // ═══════════════════════════════════════════════════════════════════════

    /** (AR) فحص الأنواع في وقت الترجمة (اختياري)
     *  (EN) Run type checker at compile time (optional) */
    bool type_check_at_compile_time = false;

    // ═══════════════════════════════════════════════════════════════════════
    //  (AR) أعلام الإصدار / (EN) Emission Flags
    // ═══════════════════════════════════════════════════════════════════════

    /** (AR) إصدار LLVM IR إلى ملف (.ll) / (EN) Emit LLVM IR to file */
    bool emit_llvm_ir = false;

    /** (AR) إصدار Assembly إلى ملف (.s/.asm) / (EN) Emit assembly to file */
    bool emit_assembly = false;

    /** (AR) إصدار Object file (.o/.obj) / (EN) Emit object file */
    bool emit_object = false;

    // ═══════════════════════════════════════════════════════════════════════
    //  (AR) أعلام طباعة التمثيلات / (EN) Representation Print Flags
    // ═══════════════════════════════════════════════════════════════════════

    /** (AR) طباعة شجرة AST / (EN) Print AST */
    bool print_ast = false;

    /** (AR) طباعة SIR / (EN) Print SIR */
    bool print_sir = false;

    /** (AR) طباعة LLVM IR قبل التحسين / (EN) Print LLVM IR before optimization */
    bool print_ir_before_opt = false;

    /** (AR) طباعة LLVM IR بعد التحسين / (EN) Print LLVM IR after optimization */
    bool print_ir_after_opt = false;

    /** (AR) طباعة إحصائيات الترجمة / (EN) Print compilation statistics */
    bool print_compilation_stats = false;

    /** (AR) طباعة إحصائيات التحسين / (EN) Print optimization statistics */
    bool print_optimization_stats = false;

    // ═══════════════════════════════════════════════════════════════════════
    //  (AR) أعلام عامة / (EN) General Flags
    // ═══════════════════════════════════════════════════════════════════════

    /** (AR) إخراج مفصّل / (EN) Verbose output */
    bool verbose = false;

    /** (AR) معاملة التحذيرات كأخطاء / (EN) Treat warnings as errors */
    bool warningsAsErrors = false;

    /** (AR) تفعيل FFI / (EN) Enable FFI */
    bool enableFFI = false;

    /** (AR) تفعيل async/await / (EN) Enable async/await */
    bool enableAsync = false;

    // ═══════════════════════════════════════════════════════════════════════
    //  (AR) خيارات وضع بلا مكتبة قياسية (Freestanding / no_std)
    //  (EN) No-standard-library mode options (Freestanding / no_std)
    //
    //  هذه الخيارات ضرورية لبرمجة:
    //    - أنظمة التشغيل (OS Kernels) مثل BeeOS
    //    - المتحكمات الدقيقة (Microcontrollers)
    //    - برامج UEFI والـ Bootloaders
    //    - الأنظمة المدمجة بدون نظام تشغيل
    //
    //  تُفعَّل تلقائياً عند اكتشاف #![بلا_مكتبة_قياسية] في المصدر،
    //  أو يمكن تمريرها يدوياً من سطر الأوامر.
    // ═══════════════════════════════════════════════════════════════════════

    /**
     * (AR) تفعيل وضع بلا مكتبة قياسية (no_std mode)
     *      عند تفعيله:
     *        - تُرفَض جميع أنواع المكتبة القياسية (std::string, std::vector...)
     *        - تُرفَض دوال C القياسية (printf, malloc, exit...)
     *        - يُتوقَّع وجود معالج ذعر مخصص (#[معالج_ذعر])
     *        - يُضمَّن sad_core.h تلقائياً كبديل
     * (EN) Enable no-std mode
     */
    bool no_std = false;

    /**
     * (AR) تعطيل دالة main الافتراضية (no_main mode)
     *      يُستخدم مع no_std لإزالة نقطة الدخول الافتراضية
     *      ويُتيح للمبرمج تعريف #[نقطة_دخول] مخصصة
     * (EN) Disable default main entry point
     */
    bool no_main = false;

    /**
     * (AR) إيقاف عند الذعر بدلاً من الـ unwind
     *      في وضع freestanding لا يمكن استخدام stack unwinding
     *      لذا يتوقف البرنامج فور حدوث ذعر غير معالج
     * (EN) Abort on panic instead of unwind
     */
    bool abort_on_panic = false;

    /**
     * (AR) السماح بالتخصيص الديناميكي في وضع freestanding
     *      يشترط تسجيل مُخصّص ذاكرة مخصص (#[معالج_تخصيص])
     *      افتراضياً: محظور في وضع freestanding الكامل
     * (EN) Allow dynamic allocation in freestanding mode (requires custom allocator)
     */
    bool freestanding_allow_alloc = false;

    /**
     * (AR) السماح بالأعداد العشرية (float/double) في freestanding
     *      بعض المعالجات لا تدعم الوحدة الحسابية للفاصلة العائمة (FPU)
     *      افتراضياً: مسموح
     * (EN) Allow floating-point in freestanding mode
     */
    bool freestanding_allow_float = true;

    /**
     * (AR) السماح بعمليات الـ atomics في freestanding
     *      يتطلب دعم أجهزة (LOCK prefix على x86)
     *      افتراضياً: مسموح
     * (EN) Allow atomic operations in freestanding mode
     */
    bool freestanding_allow_atomics = true;

    /**
     * (AR) نقطة الدخول المخصصة في وضع freestanding
     *      اسم الدالة المُعلَّمة بـ #[نقطة_دخول] أو #[entry_point]
     *      مثال: "_start", "kernel_main"
     * (EN) Custom entry point function name for freestanding
     */
    std::string freestanding_entry_point;

    /**
     * (AR) اسم دالة معالج الذعر المخصصة
     *      الدالة المُعلَّمة بـ #[معالج_ذعر] أو #[panic_handler]
     *      إذا كانت فارغة يُستخدم المعالج الافتراضي (حلقة + hlt)
     * (EN) Custom panic handler function name
     */
    std::string freestanding_panic_handler;

    /**
     * (AR) سكريبت الرابط للنظام المستهدف (Linker Script)
     *      مسار ملف .ld يُحدِّد تخطيط الذاكرة للنواة
     *      مثال: "linker.ld", "kernel.ld"
     * (EN) Linker script for target system (path to .ld file)
     */
    std::string freestanding_linker_script;

    /**
     * (AR) المعمارية المستهدفة للـ freestanding
     *      مثال: "x86_64", "aarch64", "riscv64", "arm"
     *      تؤثر على كود التهيئة والتجميع المُضمَّن
     * (EN) Target architecture for freestanding
     */
    std::string freestanding_arch;

    /**
     * (AR) هل تم اكتشاف وضع freestanding تلقائياً من الكود المصدري؟
     *      true = وُجد #![بلا_مكتبة_قياسية] في الملف
     *      false = وضع عادي
     * (EN) Was no_std mode auto-detected from source code?
     */
    bool freestanding_auto_detected = false;

    // ═══════════════════════════════════════════════════════════════════════
    //  (AR) دوال مساعدة / (EN) Helper Functions
    // ═══════════════════════════════════════════════════════════════════════

    /** (AR) هل التحسين مُفعّل؟ أي مستوى أكبر من O0
     *  (EN) Is optimization enabled? i.e. level > O0 */
    bool isOptimizationEnabled() const {
        return static_cast<int>(optimization_level) > 0;
    }

    /**
     * (AR) هل نحن في وضع Freestanding (بلا مكتبة قياسية)؟
     *      يُعيد صحيح إذا كان no_std مُفعَّل (يدوياً أو تلقائياً)
     * (EN) Are we in freestanding (no_std) mode?
     */
    bool isFreestandingMode() const {
        return no_std || freestanding_auto_detected;
    }

    /**
     * (AR) هل يجب التحقق من الرموز المحظورة في وضع freestanding؟
     *      نشط فقط في وضع no_std
     * (EN) Should banned symbols be checked in freestanding mode?
     */
    bool shouldCheckFreestandingSymbols() const {
        return isFreestandingMode();
    }

    /**
     * (AR) الحصول على اسم ملف LLVM IR المشتق من ملف الإخراج
     * (EN) Get LLVM IR filename derived from output file
     */
    std::string getLLVMIRFilename() const {
        return replaceExtension(outputFile.empty() ? inputFile : outputFile, ".ll");
    }

    /**
     * (AR) الحصول على اسم ملف Assembly المشتق من ملف الإخراج
     * (EN) Get assembly filename derived from output file
     */
    std::string getAssemblyFilename() const {
        return replaceExtension(outputFile.empty() ? inputFile : outputFile, ".s");
    }

    /**
     * (AR) الحصول على اسم ملف Object المشتق من ملف الإخراج
     * (EN) Get object filename derived from output file
     */
    std::string getObjectFilename() const {
        return replaceExtension(outputFile.empty() ? inputFile : outputFile, ".o");
    }

    /** (AR) إنشاء خيارات تصحيح افتراضية / (EN) Create default debug options */
    static CompilerOptions createDebugOptions() {
        CompilerOptions opts;
        opts.optimization_level = static_cast<sad::OptimizationLevel>(0); // O0
        opts.verbose = true;
        return opts;
    }

    /** (AR) إنشاء خيارات إصدار افتراضية / (EN) Create default release options */
    static CompilerOptions createReleaseOptions() {
        CompilerOptions opts;
        opts.optimization_level = static_cast<sad::OptimizationLevel>(2); // O2
        opts.auto_optimize = true;
        return opts;
    }

private:
    /**
     * (AR) استبدال امتداد الملف — دالة مساعدة داخلية
     * (EN) Replace file extension — internal helper
     */
    static std::string replaceExtension(const std::string& path, const std::string& ext) {
        if (path.empty()) return "output" + ext;
        auto dot = path.rfind('.');
        if (dot != std::string::npos) {
            return path.substr(0, dot) + ext;
        }
        return path + ext;
    }
};

} // namespace Compiler
} // namespace Sad

#endif // SAD_PIPELINE_COMPILER_OPTIONS_H
