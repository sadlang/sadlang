// بسم الله الرحمن الرحيم
// Compiler Driver Implementation
// تنفيذ المحرك الرئيسي للمترجم
// Phase 7.2: Compiler Driver

#include "compiler_driver.h"

// UTF-8 utilities for Arabic filename support
#include "../../shared/utils/include/utf8_utils.h"

// Lexer and Parser
#include "../../shared/lexer/include/lexer_core.h"
#include "../../shared/parser/include/parser_core.h"

// SIR Builder
#include "../../compiler_new/include/frontend/sir_builder.h"
#include "../../compiler_new/include/frontend/sir_module.h"

// LLVM Backend
#include "../../compiler_new/include/backend/llvm/llvm_codegen.h"

// Arabic Optimizer / المحسّن العربي
#include "../../compiler_new/include/backend/llvm/arabic_optimizer.h"

// SIR Optimizer / محسّن التمثيل الوسيط
// (AR) يحتوي على تمريرات التحسين: طي الثوابت، إزالة الكود الميت، CSE، نشر النسخ
// (EN) Contains optimization passes: constant folding, DCE, CSE, copy propagation
#include "../../compiler_new/include/middle/optimizer.h"

// Borrow Checker / فاحص الاستعارة
#include "../../compiler_new/include/semantic/borrow_checker.h"
#include "../../shared/ast/include/statements.h"

// LLVM headers for file output
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
// (AR) مكتبة كتابة Bitcode - لإخراج ملفات .bc
// (EN) Bitcode writer - for .bc file output
#include <llvm/Bitcode/BitcodeWriter.h>

// Windows API for paths
#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <cstdio>

// Undefine Windows macros that conflict with our code
#ifdef _WIN32
#undef CYAN
#undef YELLOW
#undef RED
#undef GREEN
#undef BLUE
#undef MAGENTA
#undef WHITE
#undef ERROR  // Windows defines this
#undef FATAL  // Windows might define this too
#endif

namespace sad {
namespace driver {

// ============================================================================
// ANSI Color Codes / أكواد الألوان
// ============================================================================

namespace colors {
    const char* RESET   = "\033[0m";
    const char* BOLD    = "\033[1m";
    const char* RED     = "\033[31m";
    const char* GREEN   = "\033[32m";
    const char* YELLOW  = "\033[33m";
    const char* BLUE    = "\033[34m";
    const char* MAGENTA = "\033[35m";
    const char* CYAN    = "\033[36m";
    const char* WHITE   = "\033[37m";
    
    // Disable colors if not supported
    void disable_colors() {
        RESET = "";
        BOLD = "";
        RED = "";
        GREEN = "";
        YELLOW = "";
        BLUE = "";
        MAGENTA = "";
        CYAN = "";
        WHITE = "";
    }
}

// ============================================================================
// TargetTriple Implementation / تنفيذ TargetTriple
// ============================================================================

std::optional<TargetTriple> TargetTriple::parse(const std::string& triple) {
    // Parse format: <arch>-<vendor>-<os>-<env>
    // Examples: x86_64-pc-linux-gnu, aarch64-apple-macos
    
    TargetTriple result;
    std::istringstream ss(triple);
    std::string part;
    std::vector<std::string> parts;
    
    while (std::getline(ss, part, '-')) {
        parts.push_back(part);
    }
    
    if (parts.size() < 2) {
        return std::nullopt;  // Invalid format
    }
    
    result.architecture = parts[0];
    
    if (parts.size() == 2) {
        // <arch>-<os>
        result.vendor = "unknown";
        result.os = parts[1];
        result.environment = "";
    } else if (parts.size() == 3) {
        // <arch>-<vendor>-<os>
        result.vendor = parts[1];
        result.os = parts[2];
        result.environment = "";
    } else if (parts.size() >= 4) {
        // <arch>-<vendor>-<os>-<env>
        result.vendor = parts[1];
        result.os = parts[2];
        result.environment = parts[3];
    }
    
    return result;
}

std::string TargetTriple::to_string() const {
    std::string result = architecture;
    
    if (!vendor.empty()) {
        result += "-" + vendor;
    }
    
    if (!os.empty()) {
        result += "-" + os;
    }
    
    if (!environment.empty()) {
        result += "-" + environment;
    }
    
    return result;
}

TargetTriple TargetTriple::get_host_target() {
    TargetTriple result;
    
    #if defined(_WIN32) || defined(_WIN64)
        result.architecture = "x86_64";
        result.vendor = "pc";
        result.os = "windows";
        result.environment = "msvc";
    #elif defined(__linux__)
        result.architecture = "x86_64";
        result.vendor = "pc";
        result.os = "linux";
        result.environment = "gnu";
    #elif defined(__APPLE__)
        result.architecture = "x86_64";
        result.vendor = "apple";
        result.os = "macos";
        result.environment = "";
    #else
        result.architecture = "unknown";
        result.vendor = "unknown";
        result.os = "unknown";
        result.environment = "";
    #endif
    
    return result;
}

// ============================================================================
// Diagnostic Implementation / تنفيذ التشخيص
// ============================================================================

void Diagnostic::print(std::ostream& os, bool use_colors) const {
    // Color codes
    const char* color_start = "";
    const char* color_end = use_colors ? colors::RESET : "";
    const char* bold_start = use_colors ? colors::BOLD : "";
    
    // Select color based on severity
    switch (severity) {
        case DiagnosticSeverity::NOTE:
            color_start = use_colors ? colors::CYAN : "";
            break;
        case DiagnosticSeverity::WARNING:
            color_start = use_colors ? colors::YELLOW : "";
            break;
        case DiagnosticSeverity::ERROR:
        case DiagnosticSeverity::FATAL:
            color_start = use_colors ? colors::RED : "";
            break;
    }
    
    // Severity label
    std::string severity_label;
    std::string severity_label_ar;
    
    switch (severity) {
        case DiagnosticSeverity::NOTE:
            severity_label = "note";
            severity_label_ar = "ملاحظة";
            break;
        case DiagnosticSeverity::WARNING:
            severity_label = "warning";
            severity_label_ar = "تحذير";
            break;
        case DiagnosticSeverity::ERROR:
            severity_label = "error";
            severity_label_ar = "خطأ";
            break;
        case DiagnosticSeverity::FATAL:
            severity_label = "fatal error";
            severity_label_ar = "خطأ فادح";
            break;
    }
    
    // Print location if available
    if (!file.empty()) {
        os << bold_start << file;
        if (line > 0) {
            os << ":" << line;
            if (column > 0) {
                os << ":" << column;
            }
        }
        os << ": ";
    }
    
    // Print severity
    os << color_start << bold_start << severity_label << ": " << color_end;
    
    // Print message (bilingual)
    os << bold_start << message << color_end;
    
    if (!message_ar.empty()) {
        os << " / " << color_start << message_ar << color_end;
    }
    
    os << "\n";
    
    // Print source line with caret
    if (!source_line.empty() && line > 0 && column > 0) {
        os << source_line << "\n";
        
        // Print caret (^) at error position
        for (int i = 1; i < column; ++i) {
            os << " ";
        }
        os << color_start << "^" << color_end << "\n";
    }
    
    // Print notes
    for (const auto& note : notes) {
        os << colors::CYAN << "note: " << colors::RESET << note << "\n";
    }
}

void DiagnosticEngine::report_note(const std::string& msg, const std::string& file,
                                   int line, int column) {
    Diagnostic diag;
    diag.severity = DiagnosticSeverity::NOTE;
    diag.message = msg;
    diag.file = file;
    diag.line = line;
    diag.column = column;
    add_diagnostic(diag);
}

void DiagnosticEngine::report_warning(const std::string& msg, const std::string& file,
                                      int line, int column) {
    Diagnostic diag;
    diag.severity = DiagnosticSeverity::WARNING;
    diag.message = msg;
    diag.file = file;
    diag.line = line;
    diag.column = column;
    add_diagnostic(diag);
    ++warning_count_;
    
    if (warnings_as_errors_) {
        ++error_count_;
    }
}

void DiagnosticEngine::report_error(const std::string& msg, const std::string& file,
                                    int line, int column) {
    Diagnostic diag;
    diag.severity = DiagnosticSeverity::ERROR;
    diag.message = msg;
    diag.file = file;
    diag.line = line;
    diag.column = column;
    add_diagnostic(diag);
    ++error_count_;
}

void DiagnosticEngine::report_fatal(const std::string& msg, const std::string& file,
                                    int line, int column) {
    Diagnostic diag;
    diag.severity = DiagnosticSeverity::FATAL;
    diag.message = msg;
    diag.file = file;
    diag.line = line;
    diag.column = column;
    add_diagnostic(diag);
    ++error_count_;
}

void DiagnosticEngine::print_diagnostics(std::ostream& os, bool use_colors) const {
    for (const auto& diag : diagnostics_) {
        diag.print(os, use_colors);
    }
}

void DiagnosticEngine::add_diagnostic(Diagnostic diag) {
    diagnostics_.push_back(std::move(diag));
}

// ============================================================================
// CompilerDriver Implementation / تنفيذ CompilerDriver
// ============================================================================

CompilerDriver::CompilerDriver() {
    // LLVM is always available, no conditional initialization needed
}

// Destructor - needs to be defined in cpp where LLVMCodeGen is complete
CompilerDriver::~CompilerDriver() = default;

int CompilerDriver::run(int argc, char* argv[]) {
    // Parse command line
    if (!parse_command_line(argc, argv)) {
        return 1;
    }
    
    // Validate options
    if (!validate_options()) {
        return 1;
    }
    
    // Compile files
    if (!compile_files(options_.input_files)) {
        // Print diagnostics
        diagnostics_.print_diagnostics(std::cerr, options_.color_diagnostics);
        
        // Print error summary
        std::cerr << colors::RED << colors::BOLD 
                  << diagnostics_.get_error_count() << " error(s) generated.\n"
                  << colors::RESET;
        
        return 1;
    }
    
    // Success!
    if (options_.verbose) {
        std::cout << colors::GREEN << colors::BOLD 
                  << "✓ Compilation successful! / الترجمة نجحت!\n"
                  << colors::RESET;
        
        // Print timing info
        if (options_.time_passes) {
            std::cout << "\nTiming Statistics:\n";
            std::cout << std::fixed << std::setprecision(3);
            std::cout << "  Frontend:   " << frontend_time_ << "s\n";
            std::cout << "  Middle-end: " << middleend_time_ << "s\n";
            std::cout << "  Backend:    " << backend_time_ << "s\n";
            if (link_time_ > 0.0) {
                std::cout << "  Linking:    " << link_time_ << "s\n";
            }
            std::cout << "  Total:      " 
                      << (frontend_time_ + middleend_time_ + backend_time_ + link_time_) 
                      << "s\n";
        }
    }
    
    return 0;
}

bool CompilerDriver::compile_file(const std::string& input_file) {
    // ════════════════════════════════════════════════════════════════════════
    // (AR) وضع توليد الواجهات — يُستخدم خط أنابيب UI بدلاً من LLVM
    // (EN) UI generation mode — uses UI pipeline instead of LLVM
    // ════════════════════════════════════════════════════════════════════════
    if (options_.emit_ui) {
        return run_ui_pipeline(input_file, options_.ui_platform);
    }

    if (options_.verbose) {
        std::cout << colors::CYAN << "Compiling " << input_file << "...\n" 
                  << colors::RESET;
    }
    
    // Time frontend
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Run frontend
    if (!run_frontend(input_file)) {
        return false;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    frontend_time_ += std::chrono::duration<double>(end_time - start_time).count();
    
    // Time middle-end
    start_time = std::chrono::high_resolution_clock::now();
    
    // Run middle-end
    if (!run_middleend()) {
        return false;
    }
    
    end_time = std::chrono::high_resolution_clock::now();
    middleend_time_ += std::chrono::duration<double>(end_time - start_time).count();
    
    // Print IR if requested
    print_ir_if_requested();
    
    // Time backend
    start_time = std::chrono::high_resolution_clock::now();
    
    // Run backend
    if (!run_backend()) {
        return false;
    }
    
    end_time = std::chrono::high_resolution_clock::now();
    backend_time_ += std::chrono::duration<double>(end_time - start_time).count();
    
    return true;
}

bool CompilerDriver::compile_files(const std::vector<std::string>& input_files) {
    // Check if we have input files
    if (input_files.empty()) {
        diagnostics_.report_fatal("no input files / لا يوجد ملفات إدخال");
        return false;
    }
    
    // (AR) تحديد ما إذا كنا نحتاج لمرحلة ربط منفصلة
    // ============================================================
    // (AR) عند وجود عدة ملفات مصدرية وليس في وضع -c (كائن فقط)،
    // نحتاج لمرحلة ربط لجمع ملفات الكائن المُنتجة.
    // أيضاً عند وجود ملفات .o/.obj مباشرة كمدخلات.
    // ============================================================
    // (EN) When multiple source files exist and not in -c (object-only) mode,
    // we need a linking phase to combine produced object files.
    // Also when .o/.obj files are given directly as inputs.
    // ============================================================
    bool has_object_inputs = false;
    for (const auto& f : input_files) {
        std::string ext = get_file_extension(f);
        if (ext == ".o" || ext == ".obj") {
            has_object_inputs = true;
            break;
        }
    }
    bool need_linking = has_object_inputs || 
                        (input_files.size() > 1 && 
                         options_.output_type != OutputType::OBJECT_FILE &&
                         options_.output_type != OutputType::LLVM_IR &&
                         options_.output_type != OutputType::LLVM_BC &&
                         options_.output_type != OutputType::ASSEMBLY);
    
    std::vector<std::string> object_files;
    
    // Compile each file
    for (const auto& input_file : input_files) {
        // Check if file exists
        if (!file_exists(input_file)) {
            diagnostics_.report_error("file not found: " + input_file + 
                                    " / الملف غير موجود");
            return false;
        }
        
        // Get file extension
        std::string ext = get_file_extension(input_file);
        
        if (ext == ".\xd8\xb5" || ext == ".sad") {
            // Source file - compile it (supports .ص and .sad)
            
            // (AR) عند الحاجة للربط، نجمع كل ملف مصدري إلى ملف كائن (.o) أولاً
            // (EN) When linking is needed, compile each source to object file first
            OutputType saved_output_type = options_.output_type;
            std::string saved_output_file = options_.output_file;
            std::string obj_file;
            
            if (need_linking) {
                options_.output_type = OutputType::OBJECT_FILE;
                obj_file = change_extension(input_file, ".o");
                options_.output_file = obj_file;
            }
            
            if (!compile_file(input_file)) {
                if (need_linking) {
                    options_.output_type = saved_output_type;
                    options_.output_file = saved_output_file;
                }
                return false;
            }
            
            if (need_linking) {
                options_.output_type = saved_output_type;
                options_.output_file = saved_output_file;
                object_files.push_back(obj_file);
                temp_files_.push_back(obj_file);
            }
        } else if (ext == ".o" || ext == ".obj") {
            // Object file - add to linking list
            if (need_linking) {
                object_files.push_back(input_file);
            } else {
                diagnostics_.report_error("cannot process object file without linking: " + 
                                        input_file);
                return false;
            }
        } else {
            diagnostics_.report_error("unknown file type: " + input_file + 
                                    " / نوع ملف غير معروف");
            return false;
        }
    }
    
    // Link if needed
    if (need_linking && !object_files.empty()) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        if (!link_files(object_files, options_.output_file)) {
            return false;
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        link_time_ = std::chrono::duration<double>(end_time - start_time).count();
    }
    
    // Cleanup temporary files
    cleanup_temp_files();
    
    return true;
}

bool CompilerDriver::link_files(const std::vector<std::string>& object_files,
                               const std::string& output_file) {
    if (options_.verbose) {
        std::cout << colors::CYAN << "Linking " << output_file << "...\n" 
                  << colors::RESET;
    }
    
    // Invoke system linker
    return invoke_linker(object_files, output_file);
}

bool CompilerDriver::parse_command_line(int argc, char* argv[]) {
    CommandLineParser parser(argc, argv);
    
    if (!parser.parse(options_, diagnostics_)) {
        return false;
    }
    
    // Check if help/version requested
    if (parser.is_help_requested()) {
        print_help(std::cout);
        std::exit(0);
    }
    
    if (parser.is_version_requested()) {
        print_version(std::cout);
        std::exit(0);
    }
    
    return true;
}

bool CompilerDriver::validate_options() {
    // ════════════════════════════════════════════════════════════════════════
    // (AR) التحقق من خيارات المترجم قبل بدء الترجمة
    // ════════════════════════════════════════════════════════════════════════
    // هذه الدالة تُجري سلسلة من التحقق على خيارات المترجم لضمان:
    //   1. وجود ملفات إدخال صالحة
    //   2. إعداد ملف الإخراج الافتراضي إذا لم يُحدَّد
    //   3. ضبط خيارات وضع Freestanding تلقائياً إذا كانت مُفعَّلة
    //   4. التعامل مع خصوصيات نظام ويندوز (مثل دعم الألوان)
    // ════════════════════════════════════════════════════════════════════════
    // (EN) Validate compiler options before starting compilation
    // ════════════════════════════════════════════════════════════════════════

    // Check if we have input files
    if (options_.input_files.empty()) {
        diagnostics_.report_fatal("no input files / لا يوجد ملفات إدخال");
        return false;
    }
    
    // Set default output file if not specified
    if (options_.output_file.empty()) {
        options_.output_file = get_default_output_name(
            options_.input_files[0],
            options_.output_type
        );
    }

    // ════════════════════════════════════════════════════════════════════════
    // (AR) ضبط خيارات وضع Freestanding التلقائية
    // ════════════════════════════════════════════════════════════════════════
    // وضع Freestanding يعني البرمجة بدون مكتبة قياسية — يُستخدم في:
    //   - تطوير أنوية أنظمة التشغيل (kernel)
    //   - البرمجة المدمجة (embedded systems)
    //   - البرامج ذاتية الاكتفاء (self-contained executables)
    //   - بيئات بدء التشغيل (boot loaders)
    //   - البرمجة المعدنية (bare-metal programming)
    //
    // عندما يُفعَّل هذا الوضع (سواء عبر --freestanding أو #![بلا_مكتبة_قياسية]):
    //   1. no_main = true   → لا يوجد دالة main() القياسية
    //   2. abort_on_panic   → لا استثناءات C++ في بيئة بدون مكتبة
    //   3. freestanding_entry = "_start" → نقطة الدخول الافتراضية للنواة
    //
    // ملاحظة: يمكن تجاوز اسم نقطة الدخول بـ --entry=kernel_main
    // ════════════════════════════════════════════════════════════════════════
    // (EN) Configure freestanding mode defaults
    //
    // Freestanding mode = programming without standard library, used in:
    //   - OS kernel development
    //   - Embedded systems
    //   - Self-contained executables
    //   - Boot loaders
    //   - Bare-metal programming
    //
    // When activated (via --freestanding or #![بلا_مكتبة_قياسية]):
    //   1. no_main = true   → no standard main() entry point
    //   2. abort_on_panic   → no C++ exceptions without runtime
    //   3. freestanding_entry = "_start" → default kernel entry point
    // ════════════════════════════════════════════════════════════════════════
    if (options_.freestanding) {
        // (AR) تفعيل no_main تلقائياً — في بيئة freestanding لا مكتبة C
        //      لذا لا توجد دالة __start المسؤولة عن استدعاء main()
        // (EN) Auto-enable no_main — freestanding has no C lib startup
        if (!options_.no_main) {
            options_.no_main = true;
            if (options_.verbose) {
                std::cout << u8"  \u2699 [freestanding] no_main \u0645\u0641\u0639\u0651\u0644 \u062a\u0644\u0642\u0627\u0626\u064a\u0627\u064b\n";
            }
        }

        // (AR) تعيين نقطة الدخول الافتراضية _start إذا لم تُحدَّد
        //      _start هي نقطة الدخول القياسية لأنوية لينكس/نظم التشغيل
        //      يمكن تغييرها لـ kernel_main أو أي اسم آخر بـ --entry=...
        // (EN) Set default entry point _start if not specified
        //      _start is the standard entry for Linux kernels/OS
        if (options_.freestanding_entry.empty()) {
            options_.freestanding_entry = "_start";
            if (options_.verbose) {
                std::cout << u8"  \u2699 [freestanding] \u0646\u0642\u0637\u0629 \u0627\u0644\u062f\u062e\u0648\u0644: _start\n";
            }
        }

        // (AR) تفعيل abort_on_panic — لا استثناءات C++ في بيئة freestanding
        //      الذعر (panic) يجب أن يوقف النواة مباشرة بدلاً من رمي استثناء
        // (EN) Enable abort_on_panic — no C++ exceptions in freestanding
        //      Panic must halt the kernel directly, not throw an exception
        if (!options_.abort_on_panic) {
            options_.abort_on_panic = true;
            if (options_.verbose) {
                std::cout << u8"  \u2699 [freestanding] abort_on_panic \u0645\u0641\u0639\u0651\u0644 \u062a\u0644\u0642\u0627\u0626\u064a\u0627\u064b\n";
            }
        }

        // (AR) طباعة ملخص وضع freestanding في الوضع المفصل
        // (EN) Print freestanding mode summary in verbose mode
        if (options_.verbose) {
            std::cout << u8"\n  \u2550\u2550 \u0648\u0636\u0639 Freestanding \u0645\u064f\u0641\u0639\u064e\u0651\u0644 \u2550\u2550\n";
            std::cout << u8"     \u2022 \u0628\u062f\u0648\u0646 \u0645\u0643\u062a\u0628\u0629 \u0642\u064a\u0627\u0633\u064a\u0629 (no stdlib)\n";
            std::cout << u8"     \u2022 \u0646\u0642\u0637\u0629 \u0627\u0644\u062f\u062e\u0648\u0644: " << options_.freestanding_entry << "\n";
            std::cout << u8"     \u2022 abort_on_panic: \u0645\u0641\u0639\u0651\u0644\n";
            std::cout << u8"     \u2022 \u0644\u0627 \u062f\u0627\u0644\u0629 main() \u0642\u064a\u0627\u0633\u064a\u0629\n\n";
        }
    }
    
    // Disable colors on Windows if not supported
    #ifdef _WIN32
    // Check if ANSI colors are supported
    // For now, disable colors on Windows by default
    if (options_.color_diagnostics) {
        // You can add Windows console color support here
        // For now, disable:
        options_.color_diagnostics = false;
        colors::disable_colors();
    }
    #endif
    
    return true;
}

bool CompilerDriver::run_frontend(const std::string& file) {
    // Read file
    auto source_opt = read_file(file);
    if (!source_opt) {
        diagnostics_.report_fatal("failed to read file: " + file + 
                                " / فشل قراءة الملف");
        return false;
    }
    
    std::string source = *source_opt;

    // ════════════════════════════════════════════════════════════════════════
    // (AR) المرحلة 0: فحص سمات Freestanding قبل التحليل المعجمي
    // ════════════════════════════════════════════════════════════════════════
    // نبحث في النص المصدري عن سمة #![بلا_مكتبة_قياسية] قبل أي مرحلة تحليل.
    // هذا يسمح للمبرمج باستخدام:
    //
    //   #![بلا_مكتبة_قياسية]      ← السمة العربية (المتوقع في لغة ص)
    //   #![no_std]                ← السمة الإنجليزية (للتوافق)
    //   #![بلا_رئيسية]            ← تعطيل main() بشكل صريح
    //   #![نقطة_دخول="_start"]    ← تحديد نقطة الدخول
    //
    // هذا الفحص المبكر يضمن أن كل مراحل التحليل اللاحقة (المعجمي، النحوي،
    // الدلالي، بناء SIR) تعرف أنها في وضع freestanding وتُطبّق القواعد
    // المناسبة (عدم استخدام malloc, printf, أنواع STL، ...)
    //
    // الخوارزمية:
    //   1. البحث السريع بـ string::find (O(n) مرة واحدة فقط)
    //   2. إذا وُجدت السمة: تفعيل options_.freestanding
    //   3. إعادة تشغيل validate_freestanding_options لضبط باقي الخيارات
    //   4. طباعة رسالة تشخيصية في الوضع المفصل
    // ════════════════════════════════════════════════════════════════════════
    // (EN) Phase 0: Detect freestanding attributes before lexing
    //
    // Search source text for #![بلا_مكتبة_قياسية] before any parsing.
    // This allows programmers to use Arabic and English no_std annotations.
    //
    // Early detection ensures all analysis phases operate in freestanding
    // mode: no malloc, printf, STL types, etc.
    //
    // Algorithm:
    //   1. Fast O(n) string search (done once only)
    //   2. If attribute found: activate options_.freestanding
    //   3. Re-run freestanding defaults setup
    //   4. Print diagnostic in verbose mode
    // ════════════════════════════════════════════════════════════════════════
    {
        // (AR) البحث عن السمات بالعربية والإنجليزية
        // (EN) Search for attributes in both Arabic and English
        const bool has_no_std_arabic  = (source.find(u8"#![\u0628\u0644\u0627_\u0645\u0643\u062a\u0628\u0629_\u0642\u064a\u0627\u0633\u064a\u0629]") != std::string::npos);
        const bool has_no_std_english = (source.find("#![no_std]") != std::string::npos);
        const bool has_no_main_arabic = (source.find(u8"#![\u0628\u0644\u0627_\u0631\u0626\u064a\u0633\u064a\u0629]") != std::string::npos);
        const bool has_no_std = has_no_std_arabic || has_no_std_english;

        if (has_no_std && !options_.freestanding) {
            // (AR) تفعيل وضع freestanding تلقائياً
            // (EN) Auto-activate freestanding mode
            options_.freestanding = true;

            // (AR) تفعيل no_main — بلا مكتبة قياسية = بلا نقطة بدء C
            // (EN) Enable no_main — no stdlib means no C startup entry
            if (!options_.no_main) {
                options_.no_main = true;
            }

            // (AR) تعيين نقطة الدخول الافتراضية
            // (EN) Set default entry point
            if (options_.freestanding_entry.empty()) {
                options_.freestanding_entry = "_start";
            }

            // (AR) تفعيل abort_on_panic — لا استثناءات بدون مكتبة
            // (EN) Enable abort on panic — no exceptions without stdlib
            options_.abort_on_panic = true;

            if (options_.verbose) {
                std::cout << u8"  \u26a1 [مرحلة 0] اكتُشف ";
                if (has_no_std_arabic) {
                    std::cout << u8"#![\u0628\u0644\u0627_\u0645\u0643\u062a\u0628\u0629_\u0642\u064a\u0627\u0633\u064a\u0629]";
                } else {
                    std::cout << "#![no_std]";
                }
                std::cout << u8" — وضع Freestanding مُفعَّل تلقائياً\n";
                std::cout << u8"     \u2022 \u0646\u0642\u0637\u0629 \u0627\u0644\u062f\u062e\u0648\u0644: " << options_.freestanding_entry << "\n";
                std::cout << u8"     \u2022 no_main: \u0645\u0641\u0639\u064e\u0651\u0644 | abort_on_panic: \u0645\u0641\u0639\u064e\u0651\u0644\n";
            }
        }

        // (AR) تفعيل no_main إذا وُجدت السمة #![بلا_رئيسية] بشكل مستقل
        // (EN) Enable no_main if #![بلا_رئيسية] attribute found independently
        if (has_no_main_arabic && !options_.no_main) {
            options_.no_main = true;
            if (options_.verbose) {
                std::cout << u8"  \u26a1 [مرحلة 0] اكتُشف #![\u0628\u0644\u0627_\u0631\u0626\u064a\u0633\u064a\u0629] — no_main \u0645\u0641\u0639\u064e\u0651\u0644\n";
            }
        }

        // (AR) البحث عن سمة نقطة الدخول المُخصَّصة: #![نقطة_دخول="..."]
        //      مثال: #![نقطة_دخول="kernel_main"] أو #![entry="my_start"]
        // (EN) Search for custom entry point attribute: #![نقطة_دخول="..."]
        {
            // (AR) نمط السمة العربية
            const std::string ar_entry_prefix = u8"#![\u0646\u0642\u0637\u0629_\u062f\u062e\u0648\u0644=\"";
            auto ar_pos = source.find(ar_entry_prefix);
            if (ar_pos != std::string::npos) {
                auto start = ar_pos + ar_entry_prefix.size();
                auto end   = source.find('"', start);
                if (end != std::string::npos && end > start) {
                    options_.freestanding_entry = source.substr(start, end - start);
                    if (options_.verbose) {
                        std::cout << u8"  \u26a1 [مرحلة 0] نقطة دخول مُخصَّصة: "
                                  << options_.freestanding_entry << "\n";
                    }
                }
            }

            // (AR) نمط السمة الإنجليزية للتوافق
            const std::string en_entry_prefix = "#![entry=\"";
            auto en_pos = source.find(en_entry_prefix);
            if (en_pos != std::string::npos && options_.freestanding_entry.empty()) {
                auto start = en_pos + en_entry_prefix.size();
                auto end   = source.find('"', start);
                if (end != std::string::npos && end > start) {
                    options_.freestanding_entry = source.substr(start, end - start);
                    if (options_.verbose) {
                        std::cout << u8"  \u26a1 [مرحلة 0] entry point (EN): "
                                  << options_.freestanding_entry << "\n";
                    }
                }
            }
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) المرحلة 0.1: اكتشاف سمات الدوال الخاصة بـ freestanding
        // ════════════════════════════════════════════════════════════════════
        // نبحث عن السمات التالية على الدوال (function attributes) لتسجيلها:
        //
        //   #[معالج_ذعر]       → يجب وجودها في كل وحدة freestanding
        //                         تُعالج الذعر بدلاً من throw/std::terminate
        //
        //   #[معالج_تخصيص]    → تُوجَد عند استخدام التخصيص الديناميكي
        //                         مطلوبة مع --allow-alloc
        //
        //   #[معالج_مقاطعة]   → معالجات المقاطعات (ISR)
        //                         تُولَّد مع اتفاقية interrupt تلقائياً
        //
        //   #[نقطة_دخول]      → دالة بدء التشغيل (بدلاً من main)
        //
        // هذه المعلومات تُستخدم لاحقاً في:
        //   - التحقق من اكتمال الوحدة (FreestandingCodeGen::أنهِ_وحدة)
        //   - توليد الاتفاقيات الصحيحة للدوال
        //   - الإبلاغ عن السمات المفقودة
        // ════════════════════════════════════════════════════════════════════
        if (options_.freestanding) {
            // (AR) فحص وجود #[معالج_ذعر] أو #[panic_handler]
            const bool له_معالج_ذعر_عربي    = (source.find(u8"#[\u0645\u0639\u0627\u0644\u062c_\u0630\u0639\u0631]") != std::string::npos);
            const bool له_معالج_ذعر_إنجليزي = (source.find("#[panic_handler]") != std::string::npos);

            // (AR) فحص وجود #[معالج_تخصيص] أو #[alloc_error_handler]
            const bool له_معالج_تخصيص_عربي    = (source.find(u8"#[\u0645\u0639\u0627\u0644\u062c_\u062a\u062e\u0635\u064a\u0635]") != std::string::npos);
            const bool له_معالج_تخصيص_إنجليزي = (source.find("#[alloc_error_handler]") != std::string::npos);

            // (AR) فحص وجود #[معالج_مقاطعة] أو #[interrupt_handler]
            const bool له_معالج_مقاطعة_عربي    = (source.find(u8"#[\u0645\u0639\u0627\u0644\u062c_\u0645\u0642\u0627\u0637\u0639\u0629") != std::string::npos);
            const bool له_معالج_مقاطعة_إنجليزي = (source.find("#[interrupt_handler") != std::string::npos);

            // (AR) فحص وجود #[نقطة_دخول] أو #[entry_point]
            const bool له_نقطة_دخول_عربي    = (source.find(u8"#[\u0646\u0642\u0637\u0629_\u062f\u062e\u0648\u0644]") != std::string::npos);
            const bool له_نقطة_دخول_إنجليزي = (source.find("#[entry_point]") != std::string::npos);

            if (options_.verbose) {
                // (AR) طباعة ملخص السمات المكتشفة
                std::cout << u8"\n  \u2550\u2550 \u0633\u0645\u0627\u062a freestanding \u0627\u0644\u0645\u0643\u062a\u0634\u0641\u0629 \u2550\u2550\n";
                // نقطة الدخول
                std::cout << (له_نقطة_دخول_عربي || له_نقطة_دخول_إنجليزي
                    ? u8"  \u2705 " : u8"  \u274c ")
                    << u8"#[\u0646\u0642\u0637\u0629_\u062f\u062e\u0648\u0644]\n";
                // معالج الذعر
                std::cout << (له_معالج_ذعر_عربي || له_معالج_ذعر_إنجليزي
                    ? u8"  \u2705 " : u8"  \u26a0  ")
                    << u8"#[\u0645\u0639\u0627\u0644\u062c_\u0630\u0639\u0631]\n";
                // معالج التخصيص
                if (options_.allow_freestanding_alloc) {
                    std::cout << (له_معالج_تخصيص_عربي || له_معالج_تخصيص_إنجليزي
                        ? u8"  \u2705 " : u8"  \u274c ")
                        << u8"#[\u0645\u0639\u0627\u0644\u062c_\u062a\u062e\u0635\u064a\u0635] (\u0645\u0637\u0644\u0648\u0628 \u0645\u0639 --allow-alloc)\n";
                }
                // معالج المقاطعة
                if (له_معالج_مقاطعة_عربي || له_معالج_مقاطعة_إنجليزي) {
                    std::cout << u8"  \u2705 #[\u0645\u0639\u0627\u0644\u062c_\u0645\u0642\u0627\u0637\u0639\u0629] (\u0633\u064a\u062a\u0645 \u062a\u0648\u0644\u064a\u062f \u0627\u062a\u0641\u0627\u0642\u064a\u0629 interrupt)\n";
                }

                // (AR) تحذير: وضع freestanding بدون معالج ذعر
                if (!له_معالج_ذعر_عربي && !له_معالج_ذعر_إنجليزي) {
                    std::cout << u8"\n  \u26a0 [freestanding] \u062a\u062d\u0630\u064a\u0631: \u0644\u0627 \u064a\u0648\u062c\u062f #[\u0645\u0639\u0627\u0644\u062c_\u0630\u0639\u0631]\n"
                              << u8"     \u0633\u064a\u0633\u062a\u062e\u062f\u0645 \u0627\u0644\u0645\u0639\u0627\u0644\u062c \u0627\u0644\u0627\u0641\u062a\u0631\u0627\u0636\u064a (happy loop + hlt).\n"
                              << u8"     \u0623\u0636\u0641:\n"
                              << u8"       #[\u0645\u0639\u0627\u0644\u062c_\u0630\u0639\u0631]\n"
                              << u8"       \u062f\u0627\u0644\u0629 \u0639\u0646\u062f_\u0627\u0644\u0630\u0639\u0631() \u062a \u062d\u0644\u0642\u0629 {}\n\n";
                }

                // (AR) تحذير: --allow-alloc بدون معالج تخصيص
                if (options_.allow_freestanding_alloc &&
                    !له_معالج_تخصيص_عربي && !له_معالج_تخصيص_إنجليزي) {
                    std::cout << u8"  \u26a0 [freestanding] \u062a\u062d\u0630\u064a\u0631: --allow-alloc \u0645\u0641\u0639\u0651\u0644 \u0644\u0643\u0646 \u0644\u0627 \u064a\u0648\u062c\u062f #[\u0645\u0639\u0627\u0644\u062c_\u062a\u062e\u0635\u064a\u0635]\n"
                              << u8"     \u0627\u0644\u0644\u062d\u0627\u0638\u0629: \u0627\u0635\u0641 \u062f\u0627\u0644\u0629 \u0628\u0640 #[\u0645\u0639\u0627\u0644\u062c_\u062a\u062e\u0635\u064a\u0635] \u0644\u0645\u0639\u0627\u0644\u062c\u0629 \u0641\u0634\u0644 \u0627\u0644\u062a\u062e\u0635\u064a\u0635.\n";
                }

                std::cout << u8"\n";
            }
        }
    }
    // ════════════════════════════════════════════════════════════════════════
    
    // Lex source
    if (options_.verbose) {
        std::cout << "  [1/5] Lexing... / التحليل المعجمي...\n";
    }
    
    lexer_ = std::make_unique<Lexer>(source);
    
    // Parse tokens
    if (options_.verbose) {
        std::cout << "  [2/5] Parsing... / التحليل النحوي...\n";
    }
    
    parser_ = std::make_unique<Parser>(*lexer_);
    current_ast_ = parser_->parseProgram();
    
    // Check for parser errors
    if (parser_->hasErrors()) {
        for (const auto& error : parser_->getErrors()) {
            diagnostics_.report_error(error, file);
        }
        return false;
    }
    
    // Print AST if requested
    if (options_.emit_ast) {
        std::cout << "\n========== AST ==========\n";
        // TODO: implement AST printing
        std::cout << "=========================\n\n";
    }
    
    // ================================================================
    // (AR) فحص الملكية والاستعارة - بين التحليل وبناء SIR
    // (EN) Borrow check - between parsing and SIR building
    // ================================================================
    if (options_.enable_borrow_check) {
        if (options_.verbose) {
            std::cout << "  [3/5] Borrow checking... / فحص الاستعارة...\n";
        }
        
        if (!run_borrow_check(file)) {
            return false;
        }
    }
    
    // ================================================================
    // (AR) فحص الأنواع المتقدم - بعد فحص الاستعارة وقبل بناء SIR
    // (EN) Advanced type check - after borrow check, before SIR building
    // ================================================================
    if (options_.enable_type_check) {
        if (options_.verbose) {
            std::cout << "  [3.5/5] Type checking... / فحص الأنواع...\n";
        }
        
        if (!run_type_check(file)) {
            return false;
        }
    }
    
    // Build SIR
    if (options_.verbose) {
        std::cout << "  [4/5] Building SIR... / بناء SIR...\n";
    }
    
    sir_builder_ = std::make_unique<SIRBuilder>();
    sir_builder_->setCurrentFilePath(file);
    sir_module_ = sir_builder_->buildModule(&current_ast_);
    
    // Check for semantic errors
    if (!sir_module_) {
        diagnostics_.report_error("Failed to build SIR module", file);
        return false;
    }
    
    return true;
}

// ============================================================================
// (AR) فحص الملكية والاستعارة / Borrow Check Phase
// ============================================================================

bool CompilerDriver::run_borrow_check(const std::string& file) {
    // (AR) إنشاء فاحص الاستعارة
    // (EN) Create borrow checker
    borrow_checker_ = std::make_unique<Sad::Semantic::BorrowChecker>();
    
    // (AR) إعداد الخيارات
    // (EN) Configure options
    borrow_checker_->setArabicMessages(options_.arabic_borrow_messages);
    borrow_checker_->setDebugMode(options_.debug_borrow_check);
    borrow_checker_->setNLLMode(true);  // (AR) تفعيل NLL دائماً / (EN) Always enable NLL
    
    // (AR) لف العبارات في BlockStmt لتمريرها للفاحص
    // (EN) Wrap statements in BlockStmt to pass to checker
    Sad::AST::BlockStmt programBlock(std::move(current_ast_));
    
    // (AR) تشغيل الفحص
    // (EN) Run the check
    auto result = borrow_checker_->check(&programBlock);
    
    // (AR) استعادة AST بعد الفحص (نحتاجها لبناء SIR)
    // (EN) Restore AST after check (needed for SIR building)
    current_ast_ = std::move(programBlock.statements);
    
    // (AR) عرض ملخص الفحص في الوضع المفصل
    // (EN) Show check summary in verbose mode
    if (options_.verbose) {
        if (options_.arabic_borrow_messages) {
            std::cout << result.getArabicSummary();
        } else {
            std::cout << result.getSummary();
        }
    }
    
    // (AR) الإبلاغ عن التحذيرات
    // (EN) Report warnings
    for (const auto& warning : result.warnings) {
        diagnostics_.report_warning(warning, file);
    }
    
    // (AR) الإبلاغ عن الأخطاء
    // (EN) Report errors
    if (!result.success) {
        for (const auto& error : result.errors) {
            std::string msg;
            if (options_.arabic_borrow_messages) {
                msg = error.toArabicString();
            } else {
                msg = error.toEnglishString();
            }
            diagnostics_.report_error(msg, file, error.errorLocation.line, error.errorLocation.column);
        }
        
        // (AR) ملخص الأخطاء
        // (EN) Error summary
        std::cerr << "\n";
        if (options_.arabic_borrow_messages) {
            std::cerr << "✗ فحص الاستعارة فشل: " << result.errors.size() 
                      << " خطأ في الملكية\n";
        } else {
            std::cerr << "✗ Borrow check failed: " << result.errors.size() 
                      << " ownership error(s)\n";
        }
        
        return false;
    }
    
    // (AR) نجاح الفحص
    // (EN) Check passed
    if (options_.verbose) {
        if (options_.arabic_borrow_messages) {
            std::cout << "  ✓ فحص الاستعارة نجح\n";
        } else {
            std::cout << "  ✓ Borrow check passed\n";
        }
    }
    
    return true;
}

// ============================================================================
// (AR) فحص الأنواع المتقدم / Advanced Type Check Phase
// ============================================================================

bool CompilerDriver::run_type_check(const std::string& file) {
    // (AR) إنشاء فاحص الأنواع
    // (EN) Create type checker
    type_checker_ = std::make_unique<Sad::Semantic::TypeChecker>();
    
    // (AR) إعداد الخيارات
    // (EN) Configure options
    type_checker_->setArabicMessages(options_.arabic_borrow_messages);
    type_checker_->setDebugMode(options_.debug_type_check);
    type_checker_->setStrictMode(options_.strict_type_check);
    
    // (AR) لف العبارات في BlockStmt لتمريرها للفاحص
    // (EN) Wrap statements in BlockStmt to pass to checker
    Sad::AST::BlockStmt programBlock(std::move(current_ast_));
    
    // (AR) تشغيل الفحص
    // (EN) Run the check
    auto result = type_checker_->check(&programBlock);
    
    // (AR) استعادة AST بعد الفحص (نحتاجها لبناء SIR)
    // (EN) Restore AST after check (needed for SIR building)
    current_ast_ = std::move(programBlock.statements);
    
    // (AR) عرض ملخص الفحص في الوضع المفصل
    // (EN) Show check summary in verbose mode
    if (options_.verbose) {
        if (options_.arabic_borrow_messages) {
            std::cout << result.getArabicSummary();
        } else {
            std::cout << result.getSummary();
        }
    }
    
    // (AR) الإبلاغ عن التحذيرات
    // (EN) Report warnings
    for (const auto& warning : result.warnings) {
        diagnostics_.report_warning(warning, file);
    }
    
    // (AR) الإبلاغ عن الأخطاء
    // (EN) Report errors
    if (!result.success) {
        for (const auto& error : result.errors) {
            std::string msg;
            if (options_.arabic_borrow_messages) {
                msg = error.toArabicString();
            } else {
                msg = error.toEnglishString();
            }
            diagnostics_.report_error(msg, file, 
                static_cast<int>(error.line), 
                static_cast<int>(error.column));
        }
        
        // (AR) ملخص الأخطاء
        // (EN) Error summary
        std::cerr << "\n";
        if (options_.arabic_borrow_messages) {
            std::cerr << "✗ فحص الأنواع فشل: " << result.errors.size() 
                      << " خطأ في الأنواع\n";
        } else {
            std::cerr << "✗ Type check failed: " << result.errors.size() 
                      << " type error(s)\n";
        }
        
        return false;
    }
    
    // (AR) نجاح الفحص
    // (EN) Check passed
    if (options_.verbose) {
        if (options_.arabic_borrow_messages) {
            std::cout << "  ✓ فحص الأنواع نجح\n";
        } else {
            std::cout << "  ✓ Type check passed\n";
        }
    }
    
    return true;
}

} // namespace driver
} // namespace sad
