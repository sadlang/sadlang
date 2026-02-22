// بسم الله الرحمن الرحيم
// Compiler Driver - Main sadc Tool
// أداة المترجم الرئيسية
// Phase 7.2: Compiler Driver

#ifndef SAD_COMPILER_DRIVER_H
#define SAD_COMPILER_DRIVER_H

#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <iostream>

// AST types / أنواع شجرة AST
#include "ast_node.h"
#include "statements.h"

// Borrow Checker / فاحص الاستعارة
#include "semantic/borrow_checker.h"

// Type Checker / فاحص الأنواع
#include "semantic/type_checker.h"

// Forward declarations outside sad::driver namespace
namespace Sad {
    namespace Lexer { class LexerCore; }
    namespace Parser { class ParserCore; }
    namespace Compiler {
        namespace SIR { 
            class SIRBuilder; 
            class SIRModule;  // Forward declare SIRModule
        }
    }
    namespace LLVM { class LLVMCodeGen; }  // Correct namespace for LLVMCodeGen
}

// (AR) تصريح مسبق لوحدة LLVM - نحتاجها في دالة link_object_to_executable
// (EN) Forward declare llvm::Module - needed in link_object_to_executable
namespace llvm { class Module; }

namespace sad {
namespace driver {

// Type aliases for use in this namespace
using Lexer = Sad::Lexer::LexerCore;
using Parser = Sad::Parser::ParserCore;
using SIRBuilder = Sad::Compiler::SIR::SIRBuilder;
using SIRModule = Sad::Compiler::SIR::SIRModule;  // Add alias for SIRModule
using LLVMCodeGen = Sad::LLVM::LLVMCodeGen;  // Correct namespace

// Not implemented yet
class SIROptimizer;
class BytecodeEmitter;

// ============================================================================
// Compilation Options / خيارات الترجمة
// ============================================================================

/**
 * @brief مستوى التحسين / Optimization level
 * 
 * (AR) مستويات التحسين المختلفة
 * (EN) Different optimization levels
 */
enum class OptimizationLevel {
    O0,  // لا تحسين - للتطوير / No optimization - for development
    O1,  // تحسينات أساسية / Basic optimizations
    O2,  // تحسينات عادية (default) / Standard optimizations
    O3,  // تحسينات قصوى / Aggressive optimizations
    Os,  // تحسين الحجم / Size optimization
    Oz   // الحد الأدنى للحجم / Minimum size
};

/**
 * @brief نوع الإخراج / Output type
 * 
 * (AR) أنواع المخرجات المدعومة
 * (EN) Supported output types
 */
enum class OutputType {
    EXECUTABLE,      // برنامج تنفيذي / Executable program
    BYTECODE,        // bytecode (.sbc)
    LLVM_IR,         // LLVM IR text (.ll)
    LLVM_BC,         // LLVM bitcode (.bc)
    OBJECT_FILE,     // Object file (.o)
    ASSEMBLY,        // Assembly code (.asm)
    SHARED_LIBRARY,  // Shared library (.so/.dll)
    STATIC_LIBRARY   // Static library (.a/.lib)
};

/**
 * @brief Target Platform / المنصة المستهدفة
 * 
 * (AR) المنصات المدعومة
 * (EN) Supported target platforms
 */
struct TargetTriple {
    std::string architecture;  // x86_64, aarch64, wasm32, etc.
    std::string vendor;        // pc, apple, unknown, etc.
    std::string os;            // linux, windows, macos, none (freestanding)
    std::string environment;   // gnu, msvc, musl, etc.
    
    // Parse from string (e.g., "x86_64-linux-gnu")
    static std::optional<TargetTriple> parse(const std::string& triple);
    
    // Convert to string
    std::string to_string() const;
    
    // Get default target for current host
    static TargetTriple get_host_target();
};

/**
 * @brief CompilerOptions - خيارات المترجم
 * 
 * (AR) جميع الخيارات المتاحة للمترجم
 * (EN) All available compiler options
 */
struct CompilerOptions {
    // ========== Input/Output ==========
    std::vector<std::string> input_files;    // ملفات الإدخال / Input files
    std::string output_file;                 // ملف الإخراج / Output file
    OutputType output_type = OutputType::EXECUTABLE;
    
    // ========== Optimization ==========
    OptimizationLevel opt_level = OptimizationLevel::O2;
    bool optimize_size = false;              // تحسين الحجم / Size optimization
    bool inline_functions = true;            // inline الدوال / Inline functions
    bool dead_code_elimination = true;       // حذف الكود الميت / DCE
    bool constant_folding = true;            // طي الثوابت / Constant folding
    bool loop_optimization = true;           // تحسين الحلقات / Loop optimization
    
    // ========== Target ==========
    TargetTriple target = TargetTriple::get_host_target();
    bool freestanding = false;               // نظام مستقل / Freestanding (no OS)
    bool position_independent_code = false;  // PIC for shared libraries

    // ──────────────────────────────────────────────────────────────────────────
    // ========== Freestanding / No-std Options - خيارات وضع بلا مكتبة ==========
    // ──────────────────────────────────────────────────────────────────────────
    //
    // (AR) هذه الخيارات مخصصة لوضع Freestanding (bare-metal):
    //   المناسب لكتابة أنظمة التشغيل، المتحكمات الدقيقة، UEFI...
    //   تُفعَّل تلقائياً عند اكتشاف #![بلا_مكتبة_قياسية] في الكود أو
    //   يدوياً بعلامة --freestanding من سطر الأوامر.
    //
    // (EN) These options are for Freestanding mode (bare-metal):
    //   Suitable for OS kernels, microcontrollers, UEFI applications...
    //   Auto-activated when #![no_std] is detected, or manually via --freestanding.
    // ──────────────────────────────────────────────────────────────────────────

    /**
     * (AR) تعطيل دالة main الافتراضية
     *      يُستخدم مع freestanding لتعريف نقطة دخول مخصصة (_start)
     *      يُفعَّل تلقائياً عند اكتشاف #![بلا_رئيسية] في الكود
     * (EN) Disable default main() entry point
     */
    bool no_main = false;

    /**
     * (AR) إيقاف عند الذعر بدلاً من stack unwinding
     *      في وضع freestanding لا يمكن استخدام استثناءات C++
     *      يُفعَّل تلقائياً عند اكتشاف #![إيقاف_عند_ذعر] في الكود
     * (EN) Abort on panic instead of C++ exception stack unwinding
     */
    bool abort_on_panic = false;

    /**
     * (AR) السماح بالتخصيص الديناميكي في وضع freestanding
     *      يتطلب تسجيل مُخصّص مخصص (#[معالج_تخصيص] / #[alloc_error_handler])
     *      افتراضياً: محظور لتجنب الأخطاء غير المقصودة
     * (EN) Allow dynamic allocation in freestanding mode
     */
    bool allow_freestanding_alloc = false;

    /**
     * (AR) اسم دالة نقطة الدخول المخصصة
     *      مثال: "_start", "kernel_main", "uefi_main"
     *      يُمرَّر للرابط كـ --entry أو -e
     * (EN) Custom entry point function name passed to linker
     */
    std::string freestanding_entry;

    // ──────────────────────────────────────────────────────────────────────────
    
    // ========== Debug ==========
    bool debug_info = false;                 // معلومات التنقيح / Debug info (DWARF)
    bool emit_line_tables = false;           // جداول الأسطر / Line tables
    std::string debug_level = "full";        // full, line-tables-only, none
    
    // ========== Linking ==========
    bool link_static = false;                // ربط ثابت / Static linking
    std::vector<std::string> library_paths;  // مسارات المكتبات / Library paths (-L)
    std::vector<std::string> libraries;      // مكتبات / Libraries to link (-l)
    std::string linker_script;               // Linker script (.ld)
    
    // ========== Output ==========
    bool verbose = false;                    // إخراج مفصل / Verbose output
    bool emit_ast = false;                   // طباعة AST
    bool emit_sir = false;                   // طباعة SIR
    bool emit_llvm = false;                  // طباعة LLVM IR
    bool emit_asm = false;                   // طباعة assembly
    bool time_passes = false;                // توقيت المراحل / Time each pass
    
    // ========== Diagnostics ==========
    bool warnings_as_errors = false;         // اعتبار التحذيرات أخطاء / Warnings as errors
    bool all_warnings = true;                // جميع التحذيرات / All warnings
    bool color_diagnostics = true;           // رسائل ملونة / Colored messages
    
    // ========== Features ==========
    bool enable_gc = true;                   // Garbage collector
    bool enable_bounds_check = true;         // فحص الحدود / Bounds checking
    bool enable_overflow_check = true;       // فحص الفيض / Overflow checking
    bool enable_null_check = true;           // فحص null
    bool enable_borrow_check = true;         // فحص الاستعارة / Borrow checking
    bool debug_borrow_check = false;         // تنقيح فحص الاستعارة / Debug borrow checker
    bool arabic_borrow_messages = true;      // رسائل عربية / Arabic borrow messages
    bool enable_type_check = true;           // فحص الأنواع / Type checking
    bool debug_type_check = false;           // تنقيح فحص الأنواع / Debug type checker
    bool strict_type_check = false;          // فحص أنواع صارم / Strict type checking
    
    // ========== Language ==========
    std::string language_standard = "sad2024"; // إصدار اللغة / Language version
    bool allow_experimental = false;         // ميزات تجريبية / Experimental features
    
    // ========== UI Pipeline / خط أنابيب الواجهات ==========
    bool emit_ui = false;                    // توليد واجهات / Generate UI code
    std::string ui_platform = "desktop";     // المنصة المستهدفة للواجهات / UI target platform
};

// ============================================================================
// Diagnostic System / نظام التشخيص
// ============================================================================

/**
 * @brief DiagnosticSeverity - شدة التشخيص
 */
enum class DiagnosticSeverity {
    NOTE,     // ملاحظة
    WARNING,  // تحذير
    ERROR,    // خطأ
    FATAL     // خطأ فادح
};

/**
 * @brief Diagnostic - رسالة تشخيصية
 */
struct Diagnostic {
    DiagnosticSeverity severity;
    std::string message;          // الرسالة بالإنجليزية
    std::string message_ar;       // الرسالة بالعربية
    std::string file;             // اسم الملف
    int line = -1;                // رقم السطر
    int column = -1;              // رقم العمود
    std::string source_line;      // السطر الكامل
    std::vector<std::string> notes; // ملاحظات إضافية
    
    // Print diagnostic with colors
    void print(std::ostream& os, bool use_colors = true) const;
};

/**
 * @brief DiagnosticEngine - محرك التشخيص
 */
class DiagnosticEngine {
public:
    DiagnosticEngine() = default;
    
    // Report diagnostics
    void report_note(const std::string& msg, const std::string& file = "",
                     int line = -1, int column = -1);
    
    void report_warning(const std::string& msg, const std::string& file = "",
                       int line = -1, int column = -1);
    
    void report_error(const std::string& msg, const std::string& file = "",
                     int line = -1, int column = -1);
    
    void report_fatal(const std::string& msg, const std::string& file = "",
                     int line = -1, int column = -1);
    
    // Get statistics
    int get_error_count() const { return error_count_; }
    int get_warning_count() const { return warning_count_; }
    bool has_errors() const { return error_count_ > 0; }
    
    // Print all diagnostics
    void print_diagnostics(std::ostream& os, bool use_colors = true) const;
    
    // Clear diagnostics
    void clear() {
        diagnostics_.clear();
        error_count_ = 0;
        warning_count_ = 0;
    }
    
    // Options
    void set_warnings_as_errors(bool value) { warnings_as_errors_ = value; }
    void set_color_diagnostics(bool value) { color_diagnostics_ = value; }

private:
    std::vector<Diagnostic> diagnostics_;
    int error_count_ = 0;
    int warning_count_ = 0;
    bool warnings_as_errors_ = false;
    bool color_diagnostics_ = true;
    
    void add_diagnostic(Diagnostic diag);
};

// ============================================================================
// Compilation Pipeline / خط الترجمة
// ============================================================================

/**
 * @brief CompilerDriver - المحرك الرئيسي للمترجم
 * 
 * (AR) يدير عملية الترجمة من البداية للنهاية
 * (EN) Manages compilation from start to finish
 * 
 * Pipeline:
 * 1. Parse command line → CompilerOptions
 * 2. Lex source files → Tokens
 * 3. Parse tokens → AST
 * 4. Build SIR → SIR Module
 * 5. Optimize SIR → Optimized SIR
 * 6. Generate code → Bytecode/LLVM IR/Object
 * 7. Link (if needed) → Executable
 */
class CompilerDriver {
public:
    CompilerDriver();
    ~CompilerDriver();  // Defined in cpp - needs complete LLVMCodeGen type
    
    // ========== Main Entry Point ==========
    /**
     * @brief تشغيل المترجم / Run compiler
     * 
     * @param argc Number of arguments
     * @param argv Arguments array
     * @return Exit code (0 = success, 1 = error)
     */
    int run(int argc, char* argv[]);
    
    // ========== Compilation Pipeline ==========
    /**
     * @brief ترجمة ملف واحد / Compile single file
     * 
     * @param input_file Input file path
     * @return true on success
     */
    bool compile_file(const std::string& input_file);
    
    /**
     * @brief ترجمة عدة ملفات / Compile multiple files
     * 
     * @param input_files Input file paths
     * @return true on success
     */
    bool compile_files(const std::vector<std::string>& input_files);
    
    /**
     * @brief ربط ملفات الكائن / Link object files
     * 
     * @param object_files Object file paths
     * @param output_file Output executable path
     * @return true on success
     */
    bool link_files(const std::vector<std::string>& object_files,
                   const std::string& output_file);
    
    // ========== Options ==========
    void set_options(const CompilerOptions& options) { options_ = options; }
    const CompilerOptions& get_options() const { return options_; }
    
    // ========== Diagnostics ==========
    DiagnosticEngine& get_diagnostics() { return diagnostics_; }
    const DiagnosticEngine& get_diagnostics() const { return diagnostics_; }
    
    // ========== Version Info ==========
    static std::string get_version() { return "1.0.0"; }
    static std::string get_compiler_name() { return "sadc - Sad Compiler"; }
    static void print_version(std::ostream& os);
    static void print_help(std::ostream& os);

private:
    // ========== Private Methods ==========
    /**
     * @brief تحليل خيارات سطر الأوامر / Parse command line
     */
    bool parse_command_line(int argc, char* argv[]);
    
    /**
     * @brief التحقق من الخيارات / Validate options
     */
    bool validate_options();
    
    /**
     * @brief Frontend: Lexing + Parsing / الواجهة الأمامية
     */
    bool run_frontend(const std::string& file);
    
    /**
     * @brief (AR) فحص الملكية والاستعارة على AST
     * @brief (EN) Run ownership/borrow checking on AST
     */
    bool run_borrow_check(const std::string& file);
    
    /**
     * @brief (AR) فحص الأنواع المتقدم على AST
     * @brief (EN) Run advanced type checking on AST
     */
    bool run_type_check(const std::string& file);
    
    /**
     * @brief Middle-end: SIR Building + Optimization / الطبقة الوسطى
     */
    bool run_middleend();
    
    /**
     * @brief Backend: Code Generation / الواجهة الخلفية
     */
    bool run_backend();
    
    /**
     * @brief Print intermediate representations
     */
    void print_ir_if_requested();
    
    /**
     * @brief (AR) تشغيل خط أنابيب الواجهات الرسومية
     * @brief (EN) Run UI IR pipeline for cross-platform code generation
     * 
     * @param input_file ملف المصدر / Source file
     * @param ui_platform_str المنصة المستهدفة / Target platform string
     * @return true إذا نجح التوليد / true if generation succeeded
     */
    bool run_ui_pipeline(const std::string& input_file,
                          const std::string& ui_platform_str);
    
    /**
     * @brief Invoke system linker
     */
    bool invoke_linker(const std::vector<std::string>& objects,
                      const std::string& output);
    
    /**
     * @brief (AR) ربط ملف كائن مع مكتبة وقت التشغيل لإنتاج ملف تنفيذي
     * @brief (EN) Link object file with runtime library to produce executable
     * 
     * @param obj_path مسار ملف الكائن / Object file path
     * @param output_file مسار الملف التنفيذي الناتج / Output executable path
     * @param module وحدة LLVM (للحصول على معلومات الهدف) / LLVM module (for target info)
     * @return true إذا نجح الربط / true if linking succeeded
     */
    bool link_object_to_executable(const std::string& obj_path,
                                   const std::string& output_file,
                                   llvm::Module* module);
    
    /**
     * @brief (AR) البحث عن أداة clang في النظام
     * @brief (EN) Find clang tool on the system
     * 
     * @return مسار clang إن وُجد / Path to clang if found
     */
    std::optional<std::string> find_clang();
    
    /**
     * @brief Get temporary file path
     */
    std::filesystem::path get_temp_file(const std::string& suffix);
    
    /**
     * @brief Clean up temporary files
     */
    void cleanup_temp_files();

private:
    // ========== Member Variables ==========
    CompilerOptions options_;
    DiagnosticEngine diagnostics_;
    
    // Compilation state
    std::unique_ptr<Lexer> lexer_;
    std::unique_ptr<Parser> parser_;
    std::unique_ptr<SIRBuilder> sir_builder_;
    std::shared_ptr<SIRModule> sir_module_;  // Store SIR module
    // std::unique_ptr<SIROptimizer> optimizer_;  // Not implemented yet
    // std::unique_ptr<BytecodeEmitter> bytecode_emitter_;  // Not implemented yet
    std::unique_ptr<Sad::LLVM::LLVMCodeGen> llvm_codegen_;  // Correct namespace
    
    // نظام الملكية / Ownership System
    std::unique_ptr<Sad::Semantic::BorrowChecker> borrow_checker_;
    
    // نظام الأنواع المتقدم / Advanced Type System
    std::unique_ptr<Sad::Semantic::TypeChecker> type_checker_;
    
    Sad::AST::StmtList current_ast_;  // (AR) تخزين AST بين المراحل / (EN) Store AST between passes
    
    // Temporary files to clean up
    std::vector<std::filesystem::path> temp_files_;
    
    // Statistics
    double frontend_time_ = 0.0;
    double middleend_time_ = 0.0;
    double backend_time_ = 0.0;
    double link_time_ = 0.0;
};

// ============================================================================
// Command Line Parser / محلل سطر الأوامر
// ============================================================================

/**
 * @brief CommandLineParser - محلل خيارات سطر الأوامر
 */
class CommandLineParser {
public:
    CommandLineParser(int argc, char* argv[]);
    
    // Parse command line
    bool parse(CompilerOptions& options, DiagnosticEngine& diag);
    
    // Check if help/version requested
    bool is_help_requested() const { return help_requested_; }
    bool is_version_requested() const { return version_requested_; }

private:
    int argc_;
    char** argv_;
    bool help_requested_ = false;
    bool version_requested_ = false;
    
    // Helper methods
    bool parse_option(const std::string& arg, CompilerOptions& options,
                     DiagnosticEngine& diag);
    
    OptimizationLevel parse_opt_level(const std::string& arg);
    OutputType parse_output_type(const std::string& arg);
};

// ============================================================================
// Utility Functions / دوال مساعدة
// ============================================================================

/**
 * @brief Get file extension
 */
std::string get_file_extension(const std::string& path);

/**
 * @brief Change file extension
 */
std::string change_extension(const std::string& path, const std::string& new_ext);

/**
 * @brief Check if file exists
 */
bool file_exists(const std::string& path);

/**
 * @brief Read entire file
 */
std::optional<std::string> read_file(const std::string& path);

/**
 * @brief Get executable directory
 */
std::filesystem::path get_executable_dir();

/**
 * @brief Find system linker (ld, lld, link.exe)
 */
std::optional<std::string> find_system_linker();

/**
 * @brief Get default output filename
 */
std::string get_default_output_name(const std::string& input_file,
                                   OutputType output_type);

} // namespace driver
} // namespace sad

#endif // SAD_COMPILER_DRIVER_H
