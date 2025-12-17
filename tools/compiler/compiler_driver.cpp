// بسم الله الرحمن الرحيم
// Compiler Driver Implementation
// تنفيذ المحرك الرئيسي للمترجم
// Phase 7.2: Compiler Driver

#include "compiler_driver.h"
#include "../../lexer/lexer.h"
#include "../../parser/parser.h"
#include "../../compiler/frontend/sir_builder.h"
#include "../../compiler/middleend/sir_optimizer.h"
#include "../../compiler/backend/bytecode/bytecode_emitter.h"

// LLVM headers (if available)
#ifdef ENABLE_LLVM
#include "../../compiler/backend/llvm/llvm_codegen.h"
#include <llvm/Support/TargetSelect.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <iomanip>

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
    // Initialize LLVM if available
    #ifdef ENABLE_LLVM
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();
    llvm::InitializeAllAsmParsers();
    #endif
}

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
    
    // Determine if we need linking
    bool need_linking = (options_.output_type == OutputType::EXECUTABLE ||
                        options_.output_type == OutputType::SHARED_LIBRARY ||
                        options_.output_type == OutputType::STATIC_LIBRARY);
    
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
        
        if (ext == ".s" || ext == ".sad") {
            // Source file - compile it
            if (!compile_file(input_file)) {
                return false;
            }
            
            // If we need linking, add object file to list
            if (need_linking) {
                std::string obj_file;
                if (input_files.size() == 1) {
                    obj_file = options_.output_file;
                } else {
                    obj_file = change_extension(input_file, ".o");
                    temp_files_.push_back(obj_file);
                }
                object_files.push_back(obj_file);
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
    
    // Lex source
    if (options_.verbose) {
        std::cout << "  [1/4] Lexing...\n";
    }
    
    lexer_ = std::make_unique<Lexer>(source, file);
    auto tokens = lexer_->tokenize();
    
    // Check for lexer errors
    if (lexer_->has_errors()) {
        // Report lexer errors
        for (const auto& error : lexer_->get_errors()) {
            diagnostics_.report_error(error.message, file, error.line, error.column);
        }
        return false;
    }
    
    // Parse tokens
    if (options_.verbose) {
        std::cout << "  [2/4] Parsing...\n";
    }
    
    parser_ = std::make_unique<Parser>(tokens);
    auto ast = parser_->parse();
    
    // Check for parser errors
    if (parser_->has_errors()) {
        for (const auto& error : parser_->get_errors()) {
            diagnostics_.report_error(error.message, file, error.line, error.column);
        }
        return false;
    }
    
    // Print AST if requested
    if (options_.emit_ast) {
        std::cout << "\n========== AST ==========\n";
        ast->print(std::cout);
        std::cout << "=========================\n\n";
    }
    
    // Build SIR
    if (options_.verbose) {
        std::cout << "  [3/4] Building SIR...\n";
    }
    
    sir_builder_ = std::make_unique<SIRBuilder>();
    auto sir_module = sir_builder_->build(ast);
    
    // Check for semantic errors
    if (sir_builder_->has_errors()) {
        for (const auto& error : sir_builder_->get_errors()) {
            diagnostics_.report_error(error.message, file, error.line, error.column);
        }
        return false;
    }
    
    return true;
}

bool CompilerDriver::run_middleend() {
    if (options_.verbose) {
        std::cout << "  [4/4] Optimizing...\n";
    }
    
    // Create optimizer
    optimizer_ = std::make_unique<SIROptimizer>(options_.opt_level);
    
    // Run optimization passes based on options
    if (options_.dead_code_elimination) {
        optimizer_->run_dead_code_elimination();
    }
    
    if (options_.constant_folding) {
        optimizer_->run_constant_folding();
    }
    
    if (options_.loop_optimization) {
        optimizer_->run_loop_optimization();
    }
    
    if (options_.inline_functions) {
        optimizer_->run_function_inlining();
    }
    
    return true;
}

bool CompilerDriver::run_backend() {
    // Select backend based on output type
    switch (options_.output_type) {
        case OutputType::BYTECODE: {
            // Bytecode emitter
            bytecode_emitter_ = std::make_unique<BytecodeEmitter>();
            auto bytecode = bytecode_emitter_->emit(sir_builder_->get_module());
            
            // Write to file
            std::ofstream out(options_.output_file, std::ios::binary);
            if (!out) {
                diagnostics_.report_fatal("failed to write output file: " + 
                                        options_.output_file);
                return false;
            }
            
            out.write(reinterpret_cast<const char*>(bytecode.data()), bytecode.size());
            break;
        }
        
        case OutputType::LLVM_IR:
        case OutputType::LLVM_BC:
        case OutputType::OBJECT_FILE:
        case OutputType::ASSEMBLY:
        case OutputType::EXECUTABLE:
        case OutputType::SHARED_LIBRARY:
        case OutputType::STATIC_LIBRARY: {
            #ifdef ENABLE_LLVM
            // LLVM code generation
            llvm_codegen_ = std::make_unique<LLVMCodeGen>(options_.target);
            auto llvm_module = llvm_codegen_->generate(sir_builder_->get_module());
            
            // Emit based on output type
            if (options_.output_type == OutputType::LLVM_IR) {
                // Emit LLVM IR text
                llvm_codegen_->emit_ir(llvm_module, options_.output_file);
            } else if (options_.output_type == OutputType::LLVM_BC) {
                // Emit LLVM bitcode
                llvm_codegen_->emit_bitcode(llvm_module, options_.output_file);
            } else if (options_.output_type == OutputType::ASSEMBLY) {
                // Emit assembly
                llvm_codegen_->emit_assembly(llvm_module, options_.output_file);
            } else {
                // Emit object file
                llvm_codegen_->emit_object(llvm_module, options_.output_file);
            }
            #else
            diagnostics_.report_fatal("LLVM backend not available / LLVM غير متاح");
            return false;
            #endif
            break;
        }
        
        default:
            diagnostics_.report_fatal("unsupported output type / نوع إخراج غير مدعوم");
            return false;
    }
    
    return true;
}

void CompilerDriver::print_ir_if_requested() {
    if (options_.emit_sir) {
        std::cout << "\n========== SIR ==========\n";
        sir_builder_->get_module()->print(std::cout);
        std::cout << "=========================\n\n";
    }
}

bool CompilerDriver::invoke_linker(const std::vector<std::string>& objects,
                                  const std::string& output) {
    // Find system linker
    auto linker_opt = find_system_linker();
    if (!linker_opt) {
        diagnostics_.report_fatal("linker not found / الرابط غير موجود");
        return false;
    }
    
    std::string linker = *linker_opt;
    
    // Build linker command
    std::string command = linker;
    
    // Add output file
    command += " -o " + output;
    
    // Add object files
    for (const auto& obj : objects) {
        command += " " + obj;
    }
    
    // Add library paths
    for (const auto& path : options_.library_paths) {
        command += " -L" + path;
    }
    
    // Add libraries
    for (const auto& lib : options_.libraries) {
        command += " -l" + lib;
    }
    
    // Add static linking flag
    if (options_.link_static) {
        command += " -static";
    }
    
    // Add linker script
    if (!options_.linker_script.empty()) {
        command += " -T " + options_.linker_script;
    }
    
    // Add freestanding flag
    if (options_.freestanding) {
        command += " -nostdlib";
    }
    
    // Execute linker
    if (options_.verbose) {
        std::cout << colors::CYAN << "Running: " << command << "\n" 
                  << colors::RESET;
    }
    
    int result = std::system(command.c_str());
    
    if (result != 0) {
        diagnostics_.report_fatal("linker failed / فشل الربط");
        return false;
    }
    
    return true;
}

std::filesystem::path CompilerDriver::get_temp_file(const std::string& suffix) {
    // Create temp file in system temp directory
    auto temp_dir = std::filesystem::temp_directory_path();
    auto temp_file = temp_dir / ("sadc_" + std::to_string(std::rand()) + suffix);
    return temp_file;
}

void CompilerDriver::cleanup_temp_files() {
    for (const auto& file : temp_files_) {
        try {
            std::filesystem::remove(file);
        } catch (...) {
            // Ignore errors during cleanup
        }
    }
    temp_files_.clear();
}

void CompilerDriver::print_version(std::ostream& os) {
    os << colors::BOLD << get_compiler_name() << " " << get_version() << colors::RESET << "\n";
    os << "Copyright (C) 2024 Sad Language Project\n";
    os << "This is free software; see the source for copying conditions.\n";
    
    #ifdef ENABLE_LLVM
    os << "\nLLVM support: " << colors::GREEN << "enabled" << colors::RESET << "\n";
    #else
    os << "\nLLVM support: " << colors::YELLOW << "disabled" << colors::RESET << "\n";
    #endif
    
    os << "\nTarget: " << TargetTriple::get_host_target().to_string() << "\n";
}

void CompilerDriver::print_help(std::ostream& os) {
    os << colors::BOLD << get_compiler_name() << " - Sad Language Compiler\n" 
       << colors::RESET;
    os << "مترجم لغة Sad / Sad Language Compiler\n\n";
    
    os << "Usage / الاستخدام:\n";
    os << "  sadc [options] <input files...>\n\n";
    
    os << "Examples / أمثلة:\n";
    os << "  sadc program.s -o program           " 
       << colors::CYAN << "# Compile to executable\n" << colors::RESET;
    os << "  sadc program.s -O3 -o program       " 
       << colors::CYAN << "# With optimizations\n" << colors::RESET;
    os << "  sadc program.s -c -o program.o      " 
       << colors::CYAN << "# Compile to object file\n" << colors::RESET;
    os << "  sadc program.s --emit-llvm -o prog.ll " 
       << colors::CYAN << "# Emit LLVM IR\n" << colors::RESET;
    os << "  sadc *.o -o program                 " 
       << colors::CYAN << "# Link object files\n\n" << colors::RESET;
    
    os << "Options / الخيارات:\n";
    os << "  -o <file>              " << "Output file / ملف الإخراج\n";
    os << "  -c                     " << "Compile only (no linking) / ترجمة فقط\n";
    os << "  -S                     " << "Generate assembly / إنتاج assembly\n";
    os << "  --emit-llvm            " << "Emit LLVM IR / إخراج LLVM IR\n";
    os << "  --emit-bc              " << "Emit LLVM bitcode / إخراج LLVM bitcode\n";
    os << "\n";
    
    os << "Optimization / التحسين:\n";
    os << "  -O0                    " << "No optimization / بدون تحسين\n";
    os << "  -O1                    " << "Basic optimization / تحسين أساسي\n";
    os << "  -O2                    " << "Standard optimization (default) / تحسين عادي\n";
    os << "  -O3                    " << "Aggressive optimization / تحسين قصوى\n";
    os << "  -Os                    " << "Size optimization / تحسين الحجم\n";
    os << "\n";
    
    os << "Debug / التنقيح:\n";
    os << "  -g                     " << "Generate debug info / معلومات التنقيح\n";
    os << "  --emit-ast             " << "Print AST / طباعة AST\n";
    os << "  --emit-sir             " << "Print SIR / طباعة SIR\n";
    os << "  --time-passes          " << "Time each compilation pass / توقيت المراحل\n";
    os << "\n";
    
    os << "Target / الهدف:\n";
    os << "  --target=<triple>      " << "Target platform / المنصة المستهدفة\n";
    os << "  --freestanding         " << "Freestanding (no OS) / مستقل\n";
    os << "\n";
    
    os << "Linking / الربط:\n";
    os << "  -L<path>               " << "Add library search path / مسار المكتبات\n";
    os << "  -l<lib>                " << "Link with library / ربط مكتبة\n";
    os << "  --static               " << "Static linking / ربط ثابت\n";
    os << "  -T<script>             " << "Use linker script / استخدام linker script\n";
    os << "\n";
    
    os << "Other / أخرى:\n";
    os << "  -v, --verbose          " << "Verbose output / إخراج مفصل\n";
    os << "  -h, --help             " << "Show this help / عرض المساعدة\n";
    os << "  --version              " << "Show version / عرض الإصدار\n";
    os << "  -Werror                " << "Treat warnings as errors / التحذيرات كأخطاء\n";
    os << "  --color                " << "Use colored output / استخدام ألوان\n";
    os << "  --no-color             " << "Disable colored output / تعطيل الألوان\n";
}

// ============================================================================
// CommandLineParser Implementation / تنفيذ CommandLineParser
// ============================================================================

CommandLineParser::CommandLineParser(int argc, char* argv[])
    : argc_(argc), argv_(argv) {}

bool CommandLineParser::parse(CompilerOptions& options, DiagnosticEngine& diag) {
    // Parse each argument
    for (int i = 1; i < argc_; ++i) {
        std::string arg = argv_[i];
        
        // Check for help/version
        if (arg == "-h" || arg == "--help") {
            help_requested_ = true;
            return true;
        }
        
        if (arg == "--version") {
            version_requested_ = true;
            return true;
        }
        
        // Parse option or add as input file
        if (arg[0] == '-') {
            if (!parse_option(arg, options, diag)) {
                return false;
            }
            
            // Some options take a value (next argument)
            if (arg == "-o" || arg == "-T") {
                if (i + 1 < argc_) {
                    if (arg == "-o") {
                        options.output_file = argv_[++i];
                    } else if (arg == "-T") {
                        options.linker_script = argv_[++i];
                    }
                } else {
                    diag.report_error("option '" + arg + "' requires an argument");
                    return false;
                }
            }
        } else {
            // Input file
            options.input_files.push_back(arg);
        }
    }
    
    return true;
}

bool CommandLineParser::parse_option(const std::string& arg, CompilerOptions& options,
                                     DiagnosticEngine& diag) {
    // Optimization levels
    if (arg == "-O0") {
        options.opt_level = OptimizationLevel::O0;
    } else if (arg == "-O1") {
        options.opt_level = OptimizationLevel::O1;
    } else if (arg == "-O2") {
        options.opt_level = OptimizationLevel::O2;
    } else if (arg == "-O3") {
        options.opt_level = OptimizationLevel::O3;
    } else if (arg == "-Os") {
        options.opt_level = OptimizationLevel::Os;
    } else if (arg == "-Oz") {
        options.opt_level = OptimizationLevel::Oz;
    }
    
    // Output types
    else if (arg == "-c") {
        options.output_type = OutputType::OBJECT_FILE;
    } else if (arg == "-S") {
        options.output_type = OutputType::ASSEMBLY;
    } else if (arg == "--emit-llvm") {
        options.output_type = OutputType::LLVM_IR;
    } else if (arg == "--emit-bc") {
        options.output_type = OutputType::LLVM_BC;
    } else if (arg == "--shared") {
        options.output_type = OutputType::SHARED_LIBRARY;
    }
    
    // Debug
    else if (arg == "-g") {
        options.debug_info = true;
    } else if (arg == "--emit-ast") {
        options.emit_ast = true;
    } else if (arg == "--emit-sir") {
        options.emit_sir = true;
    } else if (arg == "--time-passes") {
        options.time_passes = true;
    }
    
    // Target
    else if (arg.starts_with("--target=")) {
        std::string triple_str = arg.substr(9);
        auto triple_opt = TargetTriple::parse(triple_str);
        if (triple_opt) {
            options.target = *triple_opt;
        } else {
            diag.report_error("invalid target triple: " + triple_str);
            return false;
        }
    } else if (arg == "--freestanding") {
        options.freestanding = true;
    }
    
    // Linking
    else if (arg.starts_with("-L")) {
        options.library_paths.push_back(arg.substr(2));
    } else if (arg.starts_with("-l")) {
        options.libraries.push_back(arg.substr(2));
    } else if (arg == "--static") {
        options.link_static = true;
    }
    
    // Other
    else if (arg == "-v" || arg == "--verbose") {
        options.verbose = true;
    } else if (arg == "-Werror") {
        options.warnings_as_errors = true;
    } else if (arg == "--color") {
        options.color_diagnostics = true;
    } else if (arg == "--no-color") {
        options.color_diagnostics = false;
        colors::disable_colors();
    }
    
    // Unknown option
    else if (arg != "-o" && arg != "-T") {
        diag.report_warning("unknown option: " + arg + " / خيار غير معروف");
    }
    
    return true;
}

// ============================================================================
// Utility Functions / دوال مساعدة
// ============================================================================

std::string get_file_extension(const std::string& path) {
    std::filesystem::path p(path);
    return p.extension().string();
}

std::string change_extension(const std::string& path, const std::string& new_ext) {
    std::filesystem::path p(path);
    p.replace_extension(new_ext);
    return p.string();
}

bool file_exists(const std::string& path) {
    return std::filesystem::exists(path);
}

std::optional<std::string> read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        return std::nullopt;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::filesystem::path get_executable_dir() {
    // Get directory of current executable
    #ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    std::filesystem::path exe_path(buffer);
    return exe_path.parent_path();
    #else
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        std::filesystem::path exe_path(buffer);
        return exe_path.parent_path();
    }
    return std::filesystem::current_path();
    #endif
}

std::optional<std::string> find_system_linker() {
    // Try to find system linker
    const char* linkers[] = {
        "ld",        // GNU ld
        "lld",       // LLVM linker
        "ld.lld",    // LLVM linker (alternative name)
        "link.exe"   // MSVC linker (Windows)
    };
    
    for (const char* linker : linkers) {
        // Check if linker exists in PATH
        std::string command = std::string("which ") + linker + " > /dev/null 2>&1";
        
        #ifdef _WIN32
        command = std::string("where ") + linker + " > nul 2>&1";
        #endif
        
        if (std::system(command.c_str()) == 0) {
            return std::string(linker);
        }
    }
    
    return std::nullopt;
}

std::string get_default_output_name(const std::string& input_file,
                                   OutputType output_type) {
    std::filesystem::path input(input_file);
    std::string base = input.stem().string();
    
    switch (output_type) {
        case OutputType::EXECUTABLE:
            #ifdef _WIN32
            return base + ".exe";
            #else
            return base;
            #endif
        
        case OutputType::BYTECODE:
            return base + ".sbc";
        
        case OutputType::LLVM_IR:
            return base + ".ll";
        
        case OutputType::LLVM_BC:
            return base + ".bc";
        
        case OutputType::OBJECT_FILE:
            return base + ".o";
        
        case OutputType::ASSEMBLY:
            return base + ".asm";
        
        case OutputType::SHARED_LIBRARY:
            #ifdef _WIN32
            return base + ".dll";
            #elif defined(__APPLE__)
            return "lib" + base + ".dylib";
            #else
            return "lib" + base + ".so";
            #endif
        
        case OutputType::STATIC_LIBRARY:
            #ifdef _WIN32
            return base + ".lib";
            #else
            return "lib" + base + ".a";
            #endif
        
        default:
            return base;
    }
}

} // namespace driver
} // namespace sad
