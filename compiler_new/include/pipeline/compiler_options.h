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
    //  (AR) دوال مساعدة / (EN) Helper Functions
    // ═══════════════════════════════════════════════════════════════════════

    /** (AR) هل التحسين مُفعّل؟ أي مستوى أكبر من O0
     *  (EN) Is optimization enabled? i.e. level > O0 */
    bool isOptimizationEnabled() const {
        return static_cast<int>(optimization_level) > 0;
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
