/*
 * ============================================================================
 * خيارات المترجم - CompilerOptions
 * Compiler Options
 * ============================================================================
 * 
 * الوصف (Description):
 *   هيكل خيارات موحد لجميع مكونات المترجم. يحتوي على خيارات التحسين،
 *   توليد الكود، فحص الأنواع، والإخراج.
 * 
 *   Unified options structure for all compiler components. Contains optimization,
 *   code generation, type checking, and output options.
 * 
 * الاستخدام (Usage):
 *   ```cpp
 *   CompilerOptions options;
 *   options.optimization_level = OptimizationLevel::O2;
 *   options.emit_debug_info = true;
 *   options.output_file = "program.exe";
 *   ```
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): January 4, 2026
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#ifndef SAD_COMPILER_OPTIONS_H
#define SAD_COMPILER_OPTIONS_H

#include <string>
#include <vector>
#include "../../backends/llvm/llvm_optimizer.h"  // لاستخدام OptimizationLevel

namespace Sad {
namespace Compiler {

// ============================================================================
// CompilerOptions - خيارات المترجم
// CompilerOptions - Compiler Options
// ============================================================================

/**
 * هيكل خيارات المترجم الموحد
 * Unified compiler options structure
 */
struct CompilerOptions {
    // ========================================================================
    // خيارات المحسّن / Optimizer Options
    // ========================================================================
    
    /**
     * مستوى التحسين / Optimization level
     * - O0: بدون تحسين (debug builds) / No optimization (debug builds)
     * - O1: تحسينات أساسية / Basic optimizations
     * - O2: تحسينات قياسية (موصى بها) / Standard optimizations (recommended)
     * - O3: تحسينات عدوانية / Aggressive optimizations
     * - Os: تحسين لتقليل الحجم / Optimize for size
     */
    sad::OptimizationLevel optimization_level = sad::OptimizationLevel::O2;
    
    /**
     * تمكين دمج الدوال / Enable function inlining
     */
    bool enable_inlining = true;
    
    /**
     * تمكين فك الحلقات / Enable loop unrolling
     */
    bool enable_loop_unrolling = true;
    
    /**
     * تمكين التوجيه الشعاعي / Enable vectorization
     */
    bool enable_vectorization = true;
    
    /**
     * تمكين حذف الكود الميت / Enable dead code elimination
     */
    bool enable_dce = true;
    
    /**
     * تمكين Global Value Numbering
     */
    bool enable_gvn = true;
    
    /**
     * تمكين التحسينات الخاصة باللغة العربية / Enable Arabic-specific optimizations
     */
    bool enable_arabic_optimizations = true;
    
    /**
     * تحسين تلقائي بعد Code Generation / Auto optimize after code generation
     */
    bool auto_optimize = true;
    
    // ========================================================================
    // خيارات توليد الكود / Code Generation Options
    // ========================================================================
    
    /**
     * سلسلة الهدف / Target triple
     * أمثلة (Examples):
     * - "x86_64-pc-windows-msvc" (Windows 64-bit)
     * - "x86_64-unknown-linux-gnu" (Linux 64-bit)
     * - "arm64-apple-darwin" (macOS ARM64)
     */
    std::string target_triple = "x86_64-pc-windows-msvc";
    
    /**
     * إصدار معلومات التنقيح / Emit debug information
     */
    bool emit_debug_info = false;
    
    /**
     * التحقق من LLVM IR بعد التوليد / Verify LLVM IR after generation
     */
    bool verify_ir = true;
    
    /**
     * تمكين تحليل الحالات الجانبية / Enable side effect analysis
     */
    bool analyze_side_effects = true;
    
    /**
     * استخدام المحدد / Use linker
     */
    bool use_linker = true;
    
    /**
     * نوع إصدار الكود / Code emission type
     * - "object": ملف object (.o)
     * - "assembly": assembly code (.s)
     * - "llvm-ir": LLVM IR (.ll)
     * - "bitcode": LLVM bitcode (.bc)
     */
    std::string emit_type = "object";
    
    // ========================================================================
    // خيارات فاحص الأنواع / Type Checker Options
    // ========================================================================
    
    /**
     * تمكين استنتاج الأنواع / Enable type inference
     */
    bool enable_type_inference = true;
    
    /**
     * الأنواع الصارمة (لا تحويلات ضمنية) / Strict types (no implicit conversions)
     */
    bool strict_types = false;
    
    /**
     * تمكين تضييق الأنواع / Enable type narrowing
     */
    bool enable_type_narrowing = true;
    
    /**
     * التحقق من الأنواع في وقت الترجمة / Type check at compile time
     */
    bool type_check_at_compile_time = true;
    
    /**
     * تحذيرات الأنواع كأخطاء / Treat type warnings as errors
     */
    bool type_warnings_as_errors = false;
    
    // ========================================================================
    // خيارات الإخراج / Output Options
    // ========================================================================
    
    /**
     * ملف الإخراج / Output file
     */
    std::string output_file = "a.exe";
    
    /**
     * إصدار LLVM IR إلى ملف .ll / Emit LLVM IR to .ll file
     */
    bool emit_llvm_ir = false;
    
    /**
     * اسم ملف LLVM IR / LLVM IR file name
     */
    std::string llvm_ir_file = "";
    
    /**
     * إصدار assembly code إلى ملف .s / Emit assembly code to .s file
     */
    bool emit_assembly = false;
    
    /**
     * اسم ملف Assembly / Assembly file name
     */
    std::string assembly_file = "";
    
    /**
     * إصدار object file / Emit object file
     */
    bool emit_object = true;
    
    /**
     * اسم ملف Object / Object file name
     */
    std::string object_file = "";
    
    /**
     * وضع مطول (طباعة تفاصيل إضافية) / Verbose mode (print extra details)
     */
    bool verbose = false;
    
    /**
     * طباعة إحصائيات التحسين / Print optimization statistics
     */
    bool print_optimization_stats = false;
    
    /**
     * طباعة إحصائيات الترجمة / Print compilation statistics
     */
    bool print_compilation_stats = false;
    
    // ========================================================================
    // خيارات Parser و Lexer / Parser & Lexer Options
    // ========================================================================
    
    /**
     * تمكين دعم اللغة الإنجليزية للكلمات المفتاحية / Enable English keyword support
     */
    bool enable_english_keywords = false;
    
    /**
     * السماح بملفات متعددة / Allow multiple files
     */
    bool multi_file = false;
    
    /**
     * مسارات البحث عن الوحدات / Module search paths
     */
    std::vector<std::string> module_search_paths;
    
    /**
     * مكتبات للربط / Libraries to link
     */
    std::vector<std::string> link_libraries;
    
    // ========================================================================
    // خيارات التنقيح والاختبار / Debugging & Testing Options
    // ========================================================================
    
    /**
     * طباعة AST بعد Parsing / Print AST after parsing
     */
    bool print_ast = false;
    
    /**
     * طباعة SIR بعد التحويل / Print SIR after conversion
     */
    bool print_sir = false;
    
    /**
     * طباعة LLVM IR قبل التحسين / Print LLVM IR before optimization
     */
    bool print_ir_before_opt = false;
    
    /**
     * طباعة LLVM IR بعد التحسين / Print LLVM IR after optimization
     */
    bool print_ir_after_opt = false;
    
    /**
     * تمكين التوقيت (قياس وقت كل مرحلة) / Enable timing (measure each phase time)
     */
    bool enable_timing = false;
    
    /**
     * حفظ الملفات المؤقتة / Keep temporary files
     */
    bool keep_temp_files = false;
    
    // ========================================================================
    // الدوال المساعدة / Helper Functions
    // ========================================================================
    
    /**
     * تحديد ما إذا كان التحسين مفعلاً / Check if optimization is enabled
     * @return true إذا كان مستوى التحسين أكبر من O0
     */
    bool isOptimizationEnabled() const {
        return optimization_level != sad::OptimizationLevel::O0;
    }
    
    /**
     * تحديد ما إذا كان وضع Debug مفعلاً / Check if debug mode is enabled
     * @return true إذا كان emit_debug_info مفعلاً أو O0
     */
    bool isDebugMode() const {
        return emit_debug_info || optimization_level == sad::OptimizationLevel::O0;
    }
    
    /**
     * الحصول على اسم ملف LLVM IR (مع توليد تلقائي إذا لزم) / Get LLVM IR filename (with automatic generation if needed)
     * @return اسم ملف LLVM IR
     */
    std::string getLLVMIRFilename() const {
        if (!llvm_ir_file.empty()) {
            return llvm_ir_file;
        }
        // توليد اسم تلقائي من output_file / Auto-generate from output_file
        size_t lastDot = output_file.find_last_of('.');
        if (lastDot != std::string::npos) {
            return output_file.substr(0, lastDot) + ".ll";
        }
        return output_file + ".ll";
    }
    
    /**
     * الحصول على اسم ملف Assembly / Get assembly filename
     * @return اسم ملف Assembly
     */
    std::string getAssemblyFilename() const {
        if (!assembly_file.empty()) {
            return assembly_file;
        }
        size_t lastDot = output_file.find_last_of('.');
        if (lastDot != std::string::npos) {
            return output_file.substr(0, lastDot) + ".s";
        }
        return output_file + ".s";
    }
    
    /**
     * الحصول على اسم ملف Object / Get object filename
     * @return اسم ملف Object
     */
    std::string getObjectFilename() const {
        if (!object_file.empty()) {
            return object_file;
        }
        size_t lastDot = output_file.find_last_of('.');
        if (lastDot != std::string::npos) {
            return output_file.substr(0, lastDot) + ".o";
        }
        return output_file + ".o";
    }
    
    /**
     * طباعة الخيارات (للتنقيح) / Print options (for debugging)
     */
    void print() const;
};

} // namespace Compiler
} // namespace Sad

#endif // SAD_COMPILER_OPTIONS_H
