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
namespace Sad
{
    namespace Lexer
    {
        class LexerCore;
    }
    namespace Parser
    {
        class ParserCore;
    }
    namespace Compiler
    {
        namespace SIR
        {
            class SIRBuilder;
            class SIRModule; // Forward declare SIRModule
        }
    }
    namespace LLVM
    {
        class LLVMCodeGen;
    } // Correct namespace for LLVMCodeGen
}

// (AR) تصريح مسبق لوحدة LLVM - نحتاجها في دالة link_object_to_executable
// (EN) Forward declare llvm::Module - needed in link_object_to_executable
namespace llvm
{
    class Module;
}

namespace sad
{
    namespace driver
    {

        // ============================================================================
        // ANSI Color Codes (extern) / أكواد الألوان
        // (AR) التعريفات الفعلية في compiler_driver_diagnostics.cpp
        // (EN) Actual definitions in compiler_driver_diagnostics.cpp
        // ============================================================================
        namespace colors
        {
            extern const char *RESET;
            extern const char *BOLD;
            extern const char *RED;
            extern const char *GREEN;
            extern const char *YELLOW;
            extern const char *BLUE;
            extern const char *MAGENTA;
            extern const char *CYAN;
            extern const char *WHITE;
            void disable_colors();
        }

        // Type aliases for use in this namespace
        using Lexer = Sad::Lexer::LexerCore;
        using Parser = Sad::Parser::ParserCore;
        using SIRBuilder = Sad::Compiler::SIR::SIRBuilder;
        using SIRModule = Sad::Compiler::SIR::SIRModule; // Add alias for SIRModule
        using LLVMCodeGen = Sad::LLVM::LLVMCodeGen;      // Correct namespace

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
        enum class OptimizationLevel
        {
            O0, // لا تحسين - للتطوير / No optimization - for development
            O1, // تحسينات أساسية / Basic optimizations
            O2, // تحسينات عادية (default) / Standard optimizations
            O3, // تحسينات قصوى / Aggressive optimizations
            Os, // تحسين الحجم / Size optimization
            Oz  // الحد الأدنى للحجم / Minimum size
        };

        /**
         * @brief نوع الإخراج / Output type
         *
         * (AR) أنواع المخرجات المدعومة
         * (EN) Supported output types
         */
        enum class OutputType
        {
            EXECUTABLE,     // برنامج تنفيذي / Executable program
            BYTECODE,       // bytecode (.sbc)
            LLVM_IR,        // LLVM IR text (.ll)
            LLVM_BC,        // LLVM bitcode (.bc)
            OBJECT_FILE,    // Object file (.o)
            ASSEMBLY,       // Assembly code (.asm)
            SHARED_LIBRARY, // Shared library (.so/.dll)
            STATIC_LIBRARY  // Static library (.a/.lib)
        };

        /**
         * @brief Target Platform / المنصة المستهدفة
         *
         * (AR) المنصات المدعومة
         * (EN) Supported target platforms
         */
        struct TargetTriple
        {
            std::string architecture; // x86_64, aarch64, wasm32, etc.
            std::string vendor;       // pc, apple, unknown, etc.
            std::string os;           // linux, windows, macos, none (freestanding)
            std::string environment;  // gnu, msvc, musl, etc.

            // Parse from string (e.g., "x86_64-linux-gnu")
            static std::optional<TargetTriple> parse(const std::string &triple);

            // Convert to string
            std::string to_string() const;

            // Get default target for current host
            static TargetTriple get_host_target();

            // ──────────────────────────────────────────────────────────────────────
            // (AR) هل الهدف هو أندرويد؟ يتحقق من وجود "android" في نظام التشغيل
            //      أو البيئة (مثل aarch64-linux-android24)
            // (EN) Is target Android? Checks for "android" in os or environment
            // ──────────────────────────────────────────────────────────────────────
            bool is_android() const
            {
                return os.find("android") != std::string::npos ||
                       environment.find("android") != std::string::npos;
            }

            // ──────────────────────────────────────────────────────────────────────
            // (AR) الحصول على مستوى Android API من الثلاثي (مثل 24 من android24)
            // (EN) Get Android API level from triple (e.g., 24 from android24)
            // ──────────────────────────────────────────────────────────────────────
            int get_android_api_level() const
            {
                // (AR) ابحث عن "android" متبوعاً برقم في os أو environment
                auto extract_api = [](const std::string &s) -> int
                {
                    auto pos = s.find("android");
                    if (pos != std::string::npos)
                    {
                        std::string num = s.substr(pos + 7); // بعد "android"
                        if (!num.empty() && std::isdigit(num[0]))
                        {
                            return std::stoi(num);
                        }
                    }
                    return 0;
                };
                int level = extract_api(os);
                if (level == 0)
                    level = extract_api(environment);
                return level > 0 ? level : 24; // افتراضي API 24 (Android 7.0)
            }
        };

        /**
         * @brief CompilerOptions - خيارات المترجم
         *
         * (AR) جميع الخيارات المتاحة للمترجم
         * (EN) All available compiler options
         */
        struct CompilerOptions
        {
            // ========== Input/Output ==========
            std::vector<std::string> input_files; // ملفات الإدخال / Input files
            std::string output_file;              // ملف الإخراج / Output file
            OutputType output_type = OutputType::EXECUTABLE;

            // ========== Optimization ==========
            OptimizationLevel opt_level = OptimizationLevel::O2;
            bool optimize_size = false;        // تحسين الحجم / Size optimization
            bool inline_functions = true;      // inline الدوال / Inline functions
            bool dead_code_elimination = true; // حذف الكود الميت / DCE
            bool constant_folding = true;      // طي الثوابت / Constant folding
            bool loop_optimization = true;     // تحسين الحلقات / Loop optimization

            // ========== Target ==========
            TargetTriple target = TargetTriple::get_host_target();
            bool freestanding = false;              // نظام مستقل / Freestanding (no OS)
            bool position_independent_code = false; // PIC for shared libraries

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
             * (AR) وضع الوحدة: يُعامل الملف كمكتبة بدون نقطة دخول
             *      يتخطى إنشاء __sad_main ودالة main wrapper
             *      يُستخدم لتجميع ملفات مكتبة في مشاريع متعددة الملفات
             * (EN) Module mode: treat file as library without entry point
             *      Skips __sad_main generation and main wrapper
             *      Used for compiling library files in multi-file projects
             */
            bool module_mode = false;

            /**
             * (AR) اسم دالة نقطة الدخول المخصصة
             *      مثال: "_start", "kernel_main", "uefi_main"
             *      يُمرَّر للرابط كـ --entry أو -e
             * (EN) Custom entry point function name passed to linker
             */
            std::string freestanding_entry;

            // ──────────────────────────────────────────────────────────────────────────

            // ========== Debug ==========
            bool debug_info = false;          // معلومات التنقيح / Debug info (DWARF)
            bool emit_line_tables = false;    // جداول الأسطر / Line tables
            std::string debug_level = "full"; // full, line-tables-only, none

            // ========== Linking ==========
            bool link_static = false;               // ربط ثابت / Static linking
            std::vector<std::string> library_paths; // مسارات المكتبات / Library paths (-L)
            std::vector<std::string> libraries;     // مكتبات / Libraries to link (-l)
            std::string linker_script;              // Linker script (.ld)

            // ========== Output ==========
            bool verbose = false;     // إخراج مفصل / Verbose output
            bool emit_ast = false;    // طباعة AST
            bool emit_sir = false;    // طباعة SIR
            bool emit_llvm = false;   // طباعة LLVM IR
            bool emit_asm = false;    // طباعة assembly
            bool time_passes = false; // توقيت المراحل / Time each pass
            // (AR) استخراج التوثيق من شجرة AST (Markdown)
            // (EN) Extract documentation from AST (Markdown)
            bool emit_docs = false;
            std::string docs_output_path; // فارغ → stdout / empty → stdout
            // (AR) توثيق مشروع متعدد الملفات: مجلد جذر يُمسح تكرارياً
            // (EN) Multi-file project docs: root directory scanned recursively
            std::string docs_project_dir;
            std::string docs_project_name; // (AR) اسم المشروع (اختياري)
            // (AR) صيغة الإخراج: markdown (افتراضي)، json، html
            // (EN) Output format: markdown (default), json, html
            std::string docs_format = "markdown";
            // (AR) أنماط استبعاد ملفات (تطابق سلسلة فرعية في المسار)
            // (EN) Exclude patterns (substring match in path)
            std::vector<std::string> docs_excludes;

            // ========== Diagnostics ==========
            bool warnings_as_errors = false; // اعتبار التحذيرات أخطاء / Warnings as errors
            bool all_warnings = true;        // جميع التحذيرات / All warnings
            bool color_diagnostics = true;   // رسائل ملونة / Colored messages

            // ========== Features ==========
            bool enable_gc = true;              // Garbage collector
            bool enable_bounds_check = true;    // فحص الحدود / Bounds checking
            bool enable_overflow_check = true;  // فحص الفيض / Overflow checking
            bool enable_null_check = true;      // فحص null
            bool enable_borrow_check = true;    // فحص الاستعارة / Borrow checking
            bool debug_borrow_check = false;    // تنقيح فحص الاستعارة / Debug borrow checker
            bool arabic_borrow_messages = true; // رسائل عربية / Arabic borrow messages
            bool enable_type_check = true;      // فحص الأنواع / Type checking
            bool debug_type_check = false;      // تنقيح فحص الأنواع / Debug type checker
            bool strict_type_check = false;     // فحص أنواع صارم / Strict type checking

            // ========== Language ==========
            std::string language_standard = "sad2024"; // إصدار اللغة / Language version
            bool allow_experimental = false;           // ميزات تجريبية / Experimental features

            // ========== UI Pipeline / خط أنابيب الواجهات ==========
            bool emit_ui = false;                // توليد واجهات / Generate UI code
            std::string ui_platform = "desktop"; // المنصة المستهدفة للواجهات / UI target platform
        };

        // ============================================================================
        // Diagnostic System / نظام التشخيص
        // ============================================================================

        /**
         * @brief DiagnosticSeverity - شدة التشخيص
         */
        enum class DiagnosticSeverity
        {
            NOTE,    // ملاحظة
            WARNING, // تحذير
            ERROR,   // خطأ
            FATAL    // خطأ فادح
        };

        /**
         * @brief Diagnostic - رسالة تشخيصية
         */
        struct Diagnostic
        {
            DiagnosticSeverity severity;
            std::string message;            // الرسالة بالإنجليزية
            std::string message_ar;         // الرسالة بالعربية
            std::string file;               // اسم الملف
            int line = -1;                  // رقم السطر
            int column = -1;                // رقم العمود
            std::string source_line;        // السطر الكامل
            std::vector<std::string> notes; // ملاحظات إضافية

            // Print diagnostic with colors
            void print(std::ostream &os, bool use_colors = true) const;
        };

        /**
         * @brief DiagnosticEngine - محرك التشخيص
         */
        class DiagnosticEngine
        {
        public:
            DiagnosticEngine() = default;

            // Report diagnostics
            void report_note(const std::string &msg, const std::string &file = "",
                             int line = -1, int column = -1);

            void report_warning(const std::string &msg, const std::string &file = "",
                                int line = -1, int column = -1);

            void report_error(const std::string &msg, const std::string &file = "",
                              int line = -1, int column = -1);

            void report_fatal(const std::string &msg, const std::string &file = "",
                              int line = -1, int column = -1);

            // Get statistics
            int get_error_count() const { return error_count_; }
            int get_warning_count() const { return warning_count_; }
            bool has_errors() const { return error_count_ > 0; }

            // Print all diagnostics
            void print_diagnostics(std::ostream &os, bool use_colors = true) const;

            // Clear diagnostics
            void clear()
            {
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
        class CompilerDriver
        {
        public:
            CompilerDriver();
            ~CompilerDriver(); // Defined in cpp - needs complete LLVMCodeGen type

            // ========== Main Entry Point ==========
            /**
             * @brief تشغيل المترجم / Run compiler
             *
             * @param argc Number of arguments
             * @param argv Arguments array
             * @return Exit code (0 = success, 1 = error)
             */
            int run(int argc, char *argv[]);

            // ========== Compilation Pipeline ==========
            /**
             * @brief ترجمة ملف واحد / Compile single file
             *
             * @param input_file Input file path
             * @return true on success
             */
            bool compile_file(const std::string &input_file);

            /**
             * @brief ترجمة عدة ملفات / Compile multiple files
             *
             * @param input_files Input file paths
             * @return true on success
             */
            bool compile_files(const std::vector<std::string> &input_files);

            /**
             * @brief (AR) توليد توثيق Markdown لمشروع متعدد الملفات من مجلد
             *        (EN) Generate multi-file project Markdown documentation
             * @return (AR) صحيح عند النجاح (EN) true on success
             *
             * @details (AR) يقرأ docs_project_dir من الخيارات، يمسح المجلد
             *               تكرارياً عن ملفات .ص و.sad، يحلّل كل ملف، ثم يُولّد
             *               مستند Markdown موحّد ويكتبه إلى docs_output_path
             *               (أو stdout إذا فارغ).
             */
            bool emit_project_docs();

            /**
             * @brief ربط ملفات الكائن / Link object files
             *
             * @param object_files Object file paths
             * @param output_file Output executable path
             * @return true on success
             */
            bool link_files(const std::vector<std::string> &object_files,
                            const std::string &output_file);

            // ========== Options ==========
            void set_options(const CompilerOptions &options) { options_ = options; }
            const CompilerOptions &get_options() const { return options_; }

            // ========== Diagnostics ==========
            DiagnosticEngine &get_diagnostics() { return diagnostics_; }
            const DiagnosticEngine &get_diagnostics() const { return diagnostics_; }

            // ========== Version Info ==========
            static std::string get_version() { return "1.0.0"; }
            static std::string get_compiler_name() { return "sadc - Sad Compiler"; }
            static void print_version(std::ostream &os);
            static void print_help(std::ostream &os);

        private:
            // ========== Private Methods ==========
            /**
             * @brief تحليل خيارات سطر الأوامر / Parse command line
             */
            bool parse_command_line(int argc, char *argv[]);

            /**
             * @brief التحقق من الخيارات / Validate options
             */
            bool validate_options();

            /**
             * @brief Frontend: Lexing + Parsing / الواجهة الأمامية
             */
            bool run_frontend(const std::string &file);

            /**
             * @brief (AR) فحص الملكية والاستعارة على AST
             * @brief (EN) Run ownership/borrow checking on AST
             */
            bool run_borrow_check(const std::string &file);

            /**
             * @brief (AR) فحص الأنواع المتقدم على AST
             * @brief (EN) Run advanced type checking on AST
             */
            bool run_type_check(const std::string &file);

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
            bool run_ui_pipeline(const std::string &input_file,
                                 const std::string &ui_platform_str);

            /**
             * @brief Invoke system linker
             */
            bool invoke_linker(const std::vector<std::string> &objects,
                               const std::string &output);

            /**
             * @brief (AR) ربط ملف كائن مع مكتبة وقت التشغيل لإنتاج ملف تنفيذي
             * @brief (EN) Link object file with runtime library to produce executable
             *
             * @param obj_path مسار ملف الكائن / Object file path
             * @param output_file مسار الملف التنفيذي الناتج / Output executable path
             * @param module وحدة LLVM (للحصول على معلومات الهدف) / LLVM module (for target info)
             * @return true إذا نجح الربط / true if linking succeeded
             */
            bool link_object_to_executable(const std::string &obj_path,
                                           const std::string &output_file,
                                           llvm::Module *module);

            /**
             * @brief (AR) البحث عن أداة clang في النظام
             * @brief (EN) Find clang tool on the system
             *
             * @return مسار clang إن وُجد / Path to clang if found
             */
            std::optional<std::string> find_clang();

            /**
             * @brief (AR) البحث عن clang الخاص بـ Android NDK
             * @brief (EN) Find Android NDK's clang for cross-compilation
             *
             * @return مسار NDK clang إن وُجد / Path to NDK clang if found
             */
            std::optional<std::string> find_android_ndk_clang();

            /**
             * @brief (AR) الحصول على مسار sysroot لـ Android NDK
             * @brief (EN) Get Android NDK sysroot path from clang path
             *
             * @param ndk_clang مسار clang في NDK / Path to NDK clang
             * @return مسار sysroot / Sysroot path
             */
            std::string get_android_sysroot(const std::string &ndk_clang);

            /**
             * @brief (AR) ربط ملف كائن لأندرويد
             * @brief (EN) Link object file for Android target
             *
             * (AR) يستخدم NDK clang مع sysroot ومكتبات أندرويد المناسبة.
             *      ينتج ملف .so (مكتبة مشتركة) أو ملف تنفيذي حسب الإعدادات.
             * (EN) Uses NDK clang with proper sysroot and Android libraries.
             *      Produces .so (shared library) or executable based on settings.
             */
            bool link_android_executable(const std::string &obj_path,
                                         const std::string &output_file,
                                         llvm::Module *module);

            /**
             * @brief Get temporary file path
             */
            std::filesystem::path get_temp_file(const std::string &suffix);

            /**
             * @brief Clean up temporary files
             */
            void cleanup_temp_files();

#ifdef HAS_EMBEDDED_LLD
            /**
             * @brief (AR) ربط ملف كائن باستخدام LLD المدمج داخل sadc
             * @brief (EN) Link object file using the LLD linker embedded in sadc
             *
             * @param obj_path مسار ملف الكائن / Object file path
             * @param runtime_obj_path مسار ملف runtime.obj (فارغ إذا غير متوفر) / Runtime object path
             * @param output_file مسار الملف التنفيذي الناتج / Output executable path
             * @return true إذا نجح الربط / true if linking succeeded
             */
            bool link_with_embedded_lld(const std::string &obj_path,
                                        const std::string &runtime_obj_path,
                                        const std::string &output_file);

            /**
             * @brief (AR) البحث عن مسارات مكتبات MSVC و Windows SDK
             * @brief (EN) Find MSVC and Windows SDK library paths for linking
             */
            std::vector<std::string> find_msvc_lib_paths();

            /**
             * @brief (AR) البحث عن مترجم C في النظام
             * @brief (EN) Find a C compiler on the system (cl.exe, gcc, or clang)
             */
            std::optional<std::string> find_c_compiler();

            /**
             * @brief (AR) ترجمة ملف C إلى ملف كائن
             * @brief (EN) Compile a C source file to an object file
             */
            bool compile_c_to_obj(const std::string &c_source_path,
                                  const std::string &obj_output_path,
                                  const std::string &c_compiler);
#endif

            /**
             * @brief (AR) إضافة مكتبات الشبكة المضمّنة المكتشفة تلقائياً
             * @brief (EN) Append auto-discovered bundled network libraries
             */
            void append_bundled_network_libraries(std::vector<std::string> &library_paths,
                                                  std::vector<std::string> &libraries) const;

            /**
             * @brief (AR) علم توافق CRT الذي يجب تمريره إلى clang على ويندوز
             * @brief (EN) Windows CRT compatibility flag to pass to clang
             */
            std::string get_windows_clang_runtime_flag() const;

            /**
             * @brief (AR) إضافة مكتبات CRT وC++ runtime المناسبة للربط المستضاف على ويندوز
             * @brief (EN) Append the proper hosted CRT/C++ runtime libraries on Windows
             */
            void append_windows_hosted_runtime_libraries(std::vector<std::string> &libraries,
                                                         bool include_cpp_runtime) const;

        private:
            // ========== Member Variables ==========
            CompilerOptions options_;
            DiagnosticEngine diagnostics_;

            // Compilation state
            std::unique_ptr<Lexer> lexer_;
            std::unique_ptr<Parser> parser_;
            std::unique_ptr<SIRBuilder> sir_builder_;
            std::shared_ptr<SIRModule> sir_module_; // Store SIR module
            // SIR Frontend Optimizer integrated in compiler_driver_frontend.cpp
            // std::unique_ptr<BytecodeEmitter> bytecode_emitter_;  // Not implemented yet
            std::unique_ptr<Sad::LLVM::LLVMCodeGen> llvm_codegen_; // Correct namespace

            // نظام الملكية / Ownership System
            std::unique_ptr<Sad::Semantic::BorrowChecker> borrow_checker_;

            // نظام الأنواع المتقدم / Advanced Type System
            std::unique_ptr<Sad::Semantic::TypeChecker> type_checker_;

            Sad::AST::StmtList current_ast_; // (AR) تخزين AST بين المراحل / (EN) Store AST between passes

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
        class CommandLineParser
        {
        public:
            CommandLineParser(int argc, char *argv[]);

            // Parse command line
            bool parse(CompilerOptions &options, DiagnosticEngine &diag);

            // Check if help/version requested
            bool is_help_requested() const { return help_requested_; }
            bool is_version_requested() const { return version_requested_; }

        private:
            int argc_;
            char **argv_;
            bool help_requested_ = false;
            bool version_requested_ = false;

            // Helper methods
            bool parse_option(const std::string &arg, CompilerOptions &options,
                              DiagnosticEngine &diag);

            OptimizationLevel parse_opt_level(const std::string &arg);
            OutputType parse_output_type(const std::string &arg);
        };

        // ============================================================================
        // Utility Functions / دوال مساعدة
        // ============================================================================

        /**
         * @brief Get file extension
         */
        std::string get_file_extension(const std::string &path);

        /**
         * @brief Change file extension
         */
        std::string change_extension(const std::string &path, const std::string &new_ext);

        /**
         * @brief Check if file exists
         */
        bool file_exists(const std::string &path);

        /**
         * @brief Read entire file
         */
        std::optional<std::string> read_file(const std::string &path);

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
        std::string get_default_output_name(const std::string &input_file,
                                            OutputType output_type);

        // ============================================================================
        // Android Build Support / دعم بناء أندرويد
        // ============================================================================

        /**
         * @brief handleBuildAndroidCommand - معالجة أمر build android
         *
         * (AR) يعالج أمر: sadc build android <ملف.ص>
         * (EN) Handles command: sadc build android <file.sad>
         *
         * @param argc عدد الوسائط / Argument count
         * @param argv مصفوفة الوسائط / Argument array
         * @return كود الخروج (0 = نجاح) / Exit code (0 = success)
         */
        int handleBuildAndroidCommand(int argc, char *argv[]);

        // ============================================================================
        // UI Generation Support / دعم توليد الواجهات
        // ============================================================================

        /**
         * @brief handleUICommand - معالجة أمر ui generate
         *
         * (AR) يعالج أمر: sadc ui generate <منصة>
         * (EN) Handles command: sadc ui generate <platform>
         *
         * @param argc عدد الوسائط / Argument count
         * @param argv مصفوفة الوسائط / Argument array
         * @return كود الخروج (0 = نجاح) / Exit code (0 = success)
         */
        int handleUICommand(int argc, char *argv[]);

        /**
         * @brief handlePkgCommand - معالجة أوامر إدارة الحزم
         *
         * (AR) يعالج أوامر: sadc pkg <أمر>
         *      - sadc pkg install <حزمة>  -- تثبيت حزمة
         *      - sadc pkg list            -- سرد الحزم
         *      - sadc pkg sync            -- مزامنة من sad.toml
         *      - sadc pkg search <نص>     -- بحث
         *
         * (EN) Handles commands: sadc pkg <command>
         *      - sadc pkg install <pkg>   -- Install package
         *      - sadc pkg list            -- List packages
         *      - sadc pkg sync            -- Sync from sad.toml
         *      - sadc pkg search <query>  -- Search
         *
         * @param argc عدد الوسائط / Argument count
         * @param argv مصفوفة الوسائط / Argument array
         * @return كود الخروج (0 = نجاح) / Exit code (0 = success)
         */
        int handlePkgCommand(int argc, char *argv[]);

        /**
         * @brief getPackageKotlinBridges - الحصول على Kotlin Bridges لحزمة
         *
         * (AR) يُرجع قائمة مسارات ملفات Kotlin التي تربط الحزمة بنظام Android
         * (EN) Returns list of Kotlin file paths that bridge the package to Android
         *
         * @param pkgName اسم الحزمة / Package name
         * @return قائمة مسارات الملفات / List of file paths
         */
        std::vector<std::string> getPackageKotlinBridges(const std::string &pkgName);

        /**
         * @brief getPackageGradleDeps - الحصول على اعتماديات Gradle لحزمة
         *
         * (AR) يُرجع قائمة اعتماديات Gradle المطلوبة للحزمة (مثل: okhttp:4.12.0)
         * (EN) Returns list of Gradle dependencies required by the package
         *
         * @param pkgName اسم الحزمة / Package name
         * @return قائمة الاعتماديات / List of dependencies
         */
        std::vector<std::string> getPackageGradleDeps(const std::string &pkgName);

    } // namespace driver
} // namespace sad

#endif // SAD_COMPILER_DRIVER_H
