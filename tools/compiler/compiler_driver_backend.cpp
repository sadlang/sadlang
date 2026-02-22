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
// extern declarations for color variables from compiler_driver_frontend.cpp
namespace colors {
    extern const char* RESET;
    extern const char* BOLD;
    extern const char* RED;
    extern const char* GREEN;
    extern const char* YELLOW;
    extern const char* BLUE;
    extern const char* MAGENTA;
    extern const char* CYAN;
    extern const char* WHITE;
    void disable_colors();
}


bool CompilerDriver::run_middleend() {
    if (options_.verbose) {
        std::cout << "  [5/5] Optimizing... / التحسين...\n";
    }
    
    // ========================================================================
    // (AR) مرحلة التحسين الوسطى — SIR Optimizer
    //      هذه المرحلة تُطبّق تمريرات التحسين على تمثيل SIR الوسيط قبل
    //      تحويله إلى LLVM IR. التمريرات المدعومة حسب مستوى التحسين:
    //
    //      O0: بدون تحسين — يُتخطّى بالكامل (للتطوير والتصحيح)
    //      O1: طي الثوابت + إزالة الكود الميت
    //           - طي الثوابت: يحسب العمليات الحسابية على الثوابت وقت التجميع
    //             مثال: 2 + 3 → 5 بدلاً من توليد تعليمة ADD
    //           - إزالة الكود الميت: يحذف التعليمات التي لا تُستخدم نتائجها
    //      O2: O1 + نشر النسخ + حذف التعابير الفرعية المشتركة (CSE)
    //           - نشر النسخ: يستبدل %b = %a بالاستخدام المباشر لـ %a
    //           - CSE: يكتشف التعابير المكررة ويعيد استخدام النتيجة السابقة
    //      O3: O2 + دمج السجلات
    //           - دمج السجلات: يقلل عدد السجلات الافتراضية المستخدمة
    //
    // (EN) Middle-end optimization phase — SIR Optimizer
    //      Applies optimization passes on SIR intermediate representation before
    //      converting to LLVM IR. Supported passes per optimization level:
    //
    //      O0: No optimization — skipped entirely (for development/debugging)
    //      O1: Constant folding + Dead code elimination
    //      O2: O1 + Copy propagation + CSE (Common Subexpression Elimination)
    //      O3: O2 + Register coalescing
    // ========================================================================
    
    // (AR) تخطّي التحسين في المستوى O0
    // (EN) Skip optimization at O0 level
    if (options_.opt_level == OptimizationLevel::O0) {
        if (options_.verbose) {
            std::cout << "  ✓ Optimization skipped (O0) / تم تخطي التحسين (O0)\n";
        }
        return true;
    }
    
    // (AR) التحقق من وجود وحدة SIR صالحة
    // (EN) Verify valid SIR module exists
    if (!sir_module_) {
        if (options_.verbose) {
            std::cout << "  ⚠ No SIR module to optimize / لا توجد وحدة SIR للتحسين\n";
        }
        return true;
    }
    
    // (AR) تحويل مستوى التحسين من صيغة المترجم إلى صيغة المحسّن
    //      Os و Oz يُعاملان كـ O2 (تحسينات عادية مع تركيز على الحجم في LLVM لاحقاً)
    // (EN) Convert optimization level from driver format to optimizer format
    //      Os and Oz are treated as O2 (standard optimizations, size focus at LLVM level later)
    Sad::Compiler::Optimizer::OptimizationLevel optLevel;
    switch (options_.opt_level) {
        case OptimizationLevel::O1:
            optLevel = Sad::Compiler::Optimizer::OptimizationLevel::O1;
            break;
        case OptimizationLevel::O3:
            optLevel = Sad::Compiler::Optimizer::OptimizationLevel::O3;
            break;
        case OptimizationLevel::O2:
        case OptimizationLevel::Os:
        case OptimizationLevel::Oz:
        default:
            optLevel = Sad::Compiler::Optimizer::OptimizationLevel::O2;
            break;
    }
    
    // (AR) إنشاء المحسّن وتشغيله على وحدة SIR
    // (EN) Create optimizer and run on SIR module
    try {
        auto optimizer = Sad::Compiler::Optimizer::createOptimizer(optLevel);
        
        if (options_.verbose) {
            optimizer->setDebugMode(true);
        }
        
        bool changed = optimizer->optimize(sir_module_);
        
        if (options_.verbose) {
            optimizer->printStats();
            if (changed) {
                std::cout << "  ✓ Optimization applied / تم تطبيق التحسينات\n";
            } else {
                std::cout << "  ✓ No optimizations needed / لا حاجة لتحسينات\n";
            }
        }
    } catch (const std::exception& e) {
        // (AR) خطأ في التحسين — نتابع بدون تحسين بدلاً من الفشل الكامل
        //      هذا أفضل من إيقاف عملية الترجمة بالكامل
        // (EN) Optimization error — continue without optimization instead of full failure
        //      This is better than stopping the entire compilation process
        if (options_.verbose) {
            std::cerr << "  ⚠ Optimization error (continuing): " << e.what() << "\n";
            std::cerr << "  ⚠ خطأ في التحسين (سنتابع): " << e.what() << "\n";
        }
    }
    
    return true;
}

bool CompilerDriver::run_backend() {
    // ============================================================================
    // (AR) تشغيل الواجهة الخلفية - تحويل SIR إلى الإخراج المطلوب
    // ============================================================================
    // هذه الدالة هي قلب المترجم: تأخذ وحدة SIR (التمثيل الوسيط)
    // وتحولها إلى الشكل النهائي المطلوب:
    //   - LLVM IR  (.ll)  : تمثيل LLVM النصي - مفيد للتعلم والتصحيح
    //   - LLVM BC  (.bc)  : تمثيل LLVM الثنائي - أسرع للتحميل
    //   - Object   (.obj) : ملف كائن - كود آلة بدون ربط
    //   - Assembly (.asm)  : كود تجميع - مقروء بشرياً
    //   - Executable(.exe) : ملف تنفيذي - الهدف النهائي
    //   - مكتبة مشتركة/ثابتة : للمكتبات
    // ============================================================================
    // (EN) Run backend - convert SIR to requested output format
    // This function is the heart of the compiler: takes a SIR module
    // and transforms it into the final requested form.
    // ============================================================================
    
    if (!sir_module_) {
        diagnostics_.report_fatal("لا توجد وحدة SIR متاحة للواجهة الخلفية / No SIR module available for backend");
        return false;
    }
    
    // ============================================================================
    // (AR) اختيار الواجهة الخلفية بناءً على نوع الإخراج المطلوب
    // (EN) Select backend based on requested output type
    // ============================================================================
    switch (options_.output_type) {
        case OutputType::BYTECODE: {
            // (AR) واجهة Bytecode غير منفذة بعد - مخطط لها في مرحلة مستقبلية
            // (EN) Bytecode backend not implemented yet - planned for future phase
            diagnostics_.report_fatal("واجهة Bytecode غير منفذة بعد / Bytecode backend not implemented yet");
            return false;
        }
        
        // ============================================================================
        // (AR) جميع أنواع الإخراج المعتمدة على LLVM
        // (EN) All LLVM-based output types
        // ============================================================================
        case OutputType::LLVM_IR:
        case OutputType::LLVM_BC:
        case OutputType::OBJECT_FILE:
        case OutputType::ASSEMBLY:
        case OutputType::EXECUTABLE:
        case OutputType::SHARED_LIBRARY:
        case OutputType::STATIC_LIBRARY: {
            
            // ================================================================
            // (AR) الخطوة 1: إنشاء مولد LLVM وتهيئته
            // ================================================================
            // نقوم بإنشاء كائن LLVMCodeGen الذي يحتوي على:
            //   - سياق LLVM (LLVMContext) : البيئة العامة
            //   - باني التعليمات (IRBuilder) : لبناء تعليمات LLVM IR
            //   - آلة الهدف (TargetMachine) : معلومات المعالج المستهدف
            // ================================================================
            // (EN) Step 1: Create and initialize LLVM code generator
            // ================================================================
            llvm_codegen_ = std::make_unique<Sad::LLVM::LLVMCodeGen>();
            
            // (AR) نستخدم اسم الملف بدون المسار كـ module name
            //      لتجنب تعطل LLVM COFF writer مع الأحرف العربية في المسار
            // (EN) Use just the filename stem as module name to avoid
            //      LLVM COFF writer crash with Arabic chars in path
            std::string module_name = options_.input_files.empty() 
                ? "sad_module" 
                : sad::utf8::get_stem(options_.input_files[0]);
            if (!(*llvm_codegen_).initialize(module_name, options_.target.to_string())) {
                diagnostics_.report_fatal("فشل تهيئة الواجهة الخلفية LLVM / Failed to initialize LLVM backend");
                return false;
            }
            
            // ================================================================
            // (AR) الخطوة 2: توليد LLVM IR من SIR
            // ================================================================
            // تحويل التمثيل الوسيط (SIR) إلى تمثيل LLVM IR
            // هذه العملية تشمل:
            //   - تحويل كل تعليمة SIR إلى تعليمات LLVM مكافئة
            //   - إنشاء الدوال والمتغيرات العامة
            //   - التحقق من صحة الوحدة المُنتجة
            // ملاحظة: generate() تنقل ملكية الوحدة - لكن آلة الهدف تبقى
            // ================================================================
            // (EN) Step 2: Generate LLVM IR from SIR
            // Note: generate() moves module ownership but targetMachine_ persists
            // ================================================================
            auto llvm_module = (*llvm_codegen_).generate(sir_module_);
            if (!llvm_module) {
                diagnostics_.report_fatal("فشل توليد LLVM IR / Failed to generate LLVM IR");
                return false;
            }
            
            // ================================================================
            // (AR) الخطوة 2.5: تشغيل المحسّن العربي
            // ================================================================
            // تحسينات مخصصة للنصوص العربية وعمليات Unicode:
            //   - دمج النصوص الثابتة المتكررة (String pooling)
            //   - تحسين UTF-8 encoding/decoding
            //   - تحسين string concatenation
            //   - تحسين pattern matching
            // ================================================================
            // (EN) Step 2.5: Run Arabic optimizer
            // ================================================================
            {
                sad::ArabicOptimizationPass arabic_pass;
                llvm::ModuleAnalysisManager mam;
                arabic_pass.run(*llvm_module, mam);
                
                if (options_.verbose) {
                    arabic_pass.printStats();
                }
            }
            
            // ================================================================
            // (AR) الخطوة 3: إصدار الإخراج حسب النوع المطلوب
            // ================================================================
            // (EN) Step 3: Emit output based on requested type
            // ================================================================
            
            if (options_.output_type == OutputType::LLVM_IR) {
                // ==============================================================
                // (AR) إخراج LLVM IR نصي (.ll)
                // ==============================================================
                // هذا أبسط أنواع الإخراج: نكتب النص مباشرة للملف
                // مفيد جداً للتعلم والتصحيح - يمكنك قراءة الملف بأي محرر نصوص
                // ==============================================================
                // (EN) LLVM IR text output (.ll)
                // Simplest output type - human-readable LLVM IR
                // ==============================================================
                std::error_code EC;
                llvm::raw_fd_ostream dest(options_.output_file, EC, llvm::sys::fs::OF_Text);
                
                if (EC) {
                    diagnostics_.report_fatal("فشل فتح ملف الإخراج: " + EC.message() + 
                                            " / Failed to open output file: " + EC.message());
                    return false;
                }
                
                llvm_module->print(dest, nullptr);
                dest.close();
                
            } else if (options_.output_type == OutputType::LLVM_BC) {
                // ==============================================================
                // (AR) إخراج LLVM Bitcode ثنائي (.bc)
                // ==============================================================
                // تمثيل ثنائي لـ LLVM IR - أسرع للتحميل والمعالجة من النص
                // يمكن تحويله لاحقاً إلى object file أو executable
                // أداة llvm-dis تحوله مرة أخرى إلى نص مقروء
                // ==============================================================
                // (EN) LLVM Bitcode binary output (.bc)
                // Binary representation of LLVM IR - faster to load than text
                // ==============================================================
                std::error_code EC;
                llvm::raw_fd_ostream dest(options_.output_file, EC, llvm::sys::fs::OF_None);
                
                if (EC) {
                    diagnostics_.report_fatal("فشل فتح ملف الإخراج: " + EC.message() +
                                            " / Failed to open output file: " + EC.message());
                    return false;
                }
                
                llvm::WriteBitcodeToFile(*llvm_module, dest);
                dest.close();
                
            } else if (options_.output_type == OutputType::OBJECT_FILE) {
                // ==============================================================
                // (AR) إخراج ملف كائن (.obj / .o)
                // ==============================================================
                // ملف الكائن يحتوي على كود الآلة الفعلي (تعليمات المعالج)
                // لكنه غير قابل للتشغيل بمفرده - يحتاج للربط مع المكتبات
                // نستخدم دالة emitObjectFile التي تستدعي LLVM TargetMachine
                // لتحويل LLVM IR إلى تعليمات المعالج المستهدف (x86, ARM, ...)
                // ==============================================================
                // (EN) Object file output (.obj / .o)
                // Contains actual machine code but needs linking
                // ==============================================================
                if (!(*llvm_codegen_).emitObjectFile(options_.output_file, llvm_module.get())) {
                    diagnostics_.report_fatal("فشل إنتاج ملف الكائن / Failed to emit object file");
                    return false;
                }
                
            } else if (options_.output_type == OutputType::ASSEMBLY) {
                // ==============================================================
                // (AR) إخراج ملف تجميع (.asm / .s)
                // ==============================================================
                // كود التجميع هو تمثيل نصي مقروء لتعليمات المعالج
                // مفيد لفهم ما يفعله المترجم على مستوى الآلة
                // مثال: mov rax, 42  /  push rbp  /  call printf
                // ==============================================================
                // (EN) Assembly file output (.asm / .s)
                // Human-readable processor instructions
                // ==============================================================
                if (!(*llvm_codegen_).emitAssembly(options_.output_file, llvm_module.get())) {
                    diagnostics_.report_fatal("فشل إنتاج ملف التجميع / Failed to emit assembly file");
                    return false;
                }
                
            } else if (options_.output_type == OutputType::EXECUTABLE) {
                // ==============================================================
                // (AR) إخراج ملف تنفيذي (.exe على ويندوز)
                // ==============================================================
                // هذا هو الهدف النهائي والأهم: إنتاج برنامج قابل للتشغيل!
                // العملية تتم على مرحلتين:
                //   المرحلة 1: تحويل LLVM IR → ملف كائن مؤقت (.obj)
                //   المرحلة 2: ربط ملف الكائن مع المكتبات → ملف تنفيذي (.exe)
                //
                // الربط يحتاج إلى:
                //   - مكتبة وقت التشغيل (runtime) التي تحتوي على:
                //     * sad_llvm_print_int, sad_llvm_print_string, ...
                //     * sad_llvm_input, sad_llvm_input_int, ...
                //   - مكتبة C القياسية (printf, scanf, malloc, ...)
                //
                // على ويندوز نستخدم clang كمحرك للربط لأنه:
                //   - يعرف أين توجد مكتبات النظام
                //   - يتعامل مع MSVC و MinGW تلقائياً
                //   - أسهل من استدعاء link.exe مباشرة
                // ==============================================================
                // (EN) Executable output (.exe on Windows)
                // Two-phase process:
                //   Phase 1: LLVM IR → temporary object file (.obj)
                //   Phase 2: Link object file with runtime → executable (.exe)
                // ==============================================================
                
                // ------- المرحلة 1: إنتاج ملف كائن مؤقت -------
                // (EN) Phase 1: Generate temporary object file
                auto temp_obj = get_temp_file(".obj");
                temp_files_.push_back(temp_obj);
                
                std::string obj_path = temp_obj.string();
                
                if (options_.verbose) {
                    std::cerr << "  [الخطوة أ] إنتاج ملف كائن مؤقت: " << obj_path << "\n";
                    std::cerr << "  [Step A] Generating temp object file: " << obj_path << "\n";
                }
                
                if (!(*llvm_codegen_).emitObjectFile(obj_path, llvm_module.get())) {
                    diagnostics_.report_fatal("فشل إنتاج ملف الكائن المؤقت / Failed to emit temporary object file");
                    return false;
                }
                
                // ------- المرحلة 2: ربط ملف الكائن لإنتاج ملف تنفيذي -------
                // (EN) Phase 2: Link object file to produce executable
                if (options_.verbose) {
                    std::cerr << "  [الخطوة ب] ربط الملفات لإنتاج: " << options_.output_file << "\n";
                    std::cerr << "  [Step B] Linking to produce: " << options_.output_file << "\n";
                }
                
                if (!link_object_to_executable(obj_path, options_.output_file, llvm_module.get())) {
                    return false;
                }
                
            } else if (options_.output_type == OutputType::SHARED_LIBRARY) {
                // ==============================================================
                // (AR) إخراج مكتبة مشتركة (.dll على ويندوز / .so على لينكس)
                // ==============================================================
                // مكتبة مشتركة يمكن تحميلها في وقت التشغيل
                // نستخدم نفس آلية الملف التنفيذي مع إضافة -shared
                // ==============================================================
                // (EN) Shared library output (.dll / .so)
                // ==============================================================
                auto temp_obj = get_temp_file(".obj");
                temp_files_.push_back(temp_obj);
                
                std::string obj_path = temp_obj.string();
                
                if (!(*llvm_codegen_).emitObjectFile(obj_path, llvm_module.get())) {
                    diagnostics_.report_fatal("فشل إنتاج ملف الكائن المؤقت / Failed to emit temporary object file");
                    return false;
                }
                
                // (AR) ربط كمكتبة مشتركة
                // (EN) Link as shared library
                std::vector<std::string> objects = { obj_path };
                if (!invoke_linker(objects, options_.output_file)) {
                    diagnostics_.report_fatal("فشل ربط المكتبة المشتركة / Failed to link shared library");
                    return false;
                }
                
            } else if (options_.output_type == OutputType::STATIC_LIBRARY) {
                // ==============================================================
                // (AR) إخراج مكتبة ثابتة (.lib على ويندوز / .a على لينكس)
                // ==============================================================
                // مكتبة ثابتة هي مجموعة من ملفات الكائن مُحزّمة معاً
                // يتم دمجها في الملف التنفيذي وقت الترجمة
                // ==============================================================
                // (EN) Static library output (.lib / .a)
                // ==============================================================
                auto temp_obj = get_temp_file(".obj");
                temp_files_.push_back(temp_obj);
                
                std::string obj_path = temp_obj.string();
                
                if (!(*llvm_codegen_).emitObjectFile(obj_path, llvm_module.get())) {
                    diagnostics_.report_fatal("فشل إنتاج ملف الكائن المؤقت / Failed to emit temporary object file");
                    return false;
                }
                
                // (AR) إنشاء مكتبة ثابتة باستخدام أداة lib أو ar
                // (EN) Create static library using lib or ar tool
                std::string ar_command;
                #ifdef _WIN32
                ar_command = "lib /OUT:" + options_.output_file + " " + obj_path;
                #else
                ar_command = "ar rcs " + options_.output_file + " " + obj_path;
                #endif
                
                if (options_.verbose) {
                    std::cerr << "  تشغيل: " << ar_command << "\n";
                }
                
                if (std::system(ar_command.c_str()) != 0) {
                    diagnostics_.report_fatal("فشل إنشاء المكتبة الثابتة / Failed to create static library");
                    return false;
                }
            }
            
            break;
        }
        
        default:
            diagnostics_.report_fatal("نوع إخراج غير مدعوم / unsupported output type");
            return false;
    }
    
    return true;
}

void CompilerDriver::print_ir_if_requested() {
    if (options_.emit_sir && sir_module_) {
        std::cout << "\n========== SIR ==========\n";
        sir_module_->print();  // print() doesn't take ostream parameter
        std::cout << "=========================\n\n";
    }
}

bool CompilerDriver::invoke_linker(const std::vector<std::string>& objects,
                                  const std::string& output) {
    // ════════════════════════════════════════════════════════════════════════
    // (AR) دالة ربط الملفات الثنائية — تجمع ملفات الكائن في ملف واحد
    // ════════════════════════════════════════════════════════════════════════
    // هذه الدالة تُنفِّذ مرحلة الربط (Linking) التي تجمع:
    //   - ملفات الكائن (.obj/.o)   : الكود المُترجَم
    //   - مكتبات النظام (.lib/.a)  : وظائف قياسية
    //   - ملفات بدء التشغيل (CRT) : إعداد البيئة قبل main()
    //
    // في وضع Freestanding يتم تخطّي جميع المكتبات القياسية وملفات CRT،
    // لأن النواة/البرنامج المدمج يُعرِّف بيئته الخاصة من الصفر.
    // ════════════════════════════════════════════════════════════════════════
    // (EN) Linker invocation — combines object files into a single binary
    //
    // This function performs the Linking phase combining:
    //   - Object files (.obj/.o)   : compiled code
    //   - System libraries (.lib/.a): standard functions
    //   - C runtime files (CRT)    : environment setup before main()
    //
    // In Freestanding mode, all standard libraries and CRT files are
    // skipped — the kernel/embedded program defines its own environment.
    // ════════════════════════════════════════════════════════════════════════

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
    
    // (AR) إضافة علامات وضع Freestanding قبل أي مكتبات
    // ════════════════════════════════════════════════════════════════════════
    // (AR) وضع Freestanding — علامات الرابط الضرورية:
    //
    //   -nostdlib
    //       لا تضمّن مكتبة C القياسية (libc.a / msvcrt.lib)
    //       ولا مكتبة C++ القياسية (libstdc++.a / msvcprt.lib)
    //       هذا يضمن عدم وجود أي كود خارجي غير متوقع في الثنائي
    //
    //   -nostartfiles
    //       لا تضمّن ملفات بدء تشغيل C (crt0.o, crti.o, crtn.o)
    //       هذه الملفات مسؤولة عن استدعاء main() وإعداد مكدّس C
    //       في وضع freestanding، الكود يبدأ مباشرة من رمز _start
    //
    //   -nodefaultlibs
    //       لا تُضف أي مكتبات افتراضية أخرى (libgcc, compiler-rt, ...)
    //       هذا يُعطي تحكماً كاملاً على ما يُرتبط بالثنائي
    //
    //   -e <نقطة_الدخول>
    //       تحديد نقطة البداية الفعلية للتنفيذ
    //       افتراضياً: _start (معيار لينكس)
    //       قابل للتغيير: kernel_main, boot_entry, ...
    //
    //   -T <سكريبت_الرابط>
    //       سكريبت يُحدّد تخطيط الذاكرة (Memory Layout):
    //       - أين تبدأ قسم النص (.text) في الذاكرة
    //       - أين تُوضع البيانات (.data, .bss)
    //       - عنوان التحميل لأنظمة التشغيل والبرامج المدمجة
    // ════════════════════════════════════════════════════════════════════════
    // (EN) Freestanding mode — required linker flags:
    //
    //   -nostdlib     : no C/C++ standard library
    //   -nostartfiles : no C startup files (crt0.o etc.)
    //   -nodefaultlibs: no default libraries (libgcc, compiler-rt)
    //   -e <entry>    : specify actual execution entry point
    //   -T <script>   : linker script for memory layout
    // ════════════════════════════════════════════════════════════════════════
    if (options_.freestanding) {
        // (AR) العلامات الثلاثة الأساسية لوضع freestanding
        // (EN) Three essential freestanding flags
        command += " -nostdlib -nostartfiles -nodefaultlibs";

        // (AR) تحديد نقطة الدخول — دائماً في وضع freestanding
        //      حتى _start الافتراضية يجب تحديدها صراحةً
        //      لأن بعض الرابطات قد تبحث عن main() افتراضياً
        // (EN) Always specify entry point in freestanding mode
        //      Because some linkers default to looking for main()
        if (!options_.freestanding_entry.empty()) {
            command += " -e " + options_.freestanding_entry;
            if (options_.verbose) {
                std::cout << u8"  \u2699 [linker] \u0646\u0642\u0637\u0629 \u0627\u0644\u062f\u062e\u0648\u0644: "
                          << options_.freestanding_entry << "\n";
            }
        }

        // (AR) سكريبت الرابط — يُحدَّد هنا (لا يُكرَّر لاحقاً)
        //      يُوفِّر تحكماً كاملاً في تخطيط الذاكرة
        // (EN) Linker script — specified here (not duplicated below)
        if (!options_.linker_script.empty()) {
            command += " -T \"" + options_.linker_script + "\"";
            if (options_.verbose) {
                std::cout << u8"  \u2699 [linker] \u0633\u0643\u0631\u064a\u0628\u062a \u0627\u0644\u0631\u0627\u0628\u0637: "
                          << options_.linker_script << "\n";
            }
        }

        // (AR) تحذير للمطور إذا لم يوجد سكريبت ربط في وضع freestanding
        //      بدون سكريبت، الرابط يستخدم التخطيط الافتراضي للنظام المضيف
        //      وهذا قد لا يكون صحيحاً للنواة أو الأنظمة المدمجة
        // (EN) Warn developer if no linker script in freestanding mode
        if (options_.linker_script.empty() && options_.verbose) {
            std::cout << u8"  \u26a0 [freestanding] \u062a\u062d\u0630\u064a\u0631: \u0644\u0627 \u064a\u0648\u062c\u062f \u0633\u0643\u0631\u064a\u0628\u062a \u0631\u0627\u0628\u0637 (-T).\n"
                      << u8"     \u0633\u064a\u0633\u062a\u062e\u062f\u0645 \u0627\u0644\u0631\u0627\u0628\u0637 \u0627\u0644\u062a\u062e\u0637\u064a\u0637 \u0627\u0644\u0627\u0641\u062a\u0631\u0627\u0636\u064a.\n"
                      << u8"     \u0627\u0633\u062a\u062e\u062f\u0645 --linker-script=<\u0645\u0633\u0627\u0631> \u0644\u062a\u062d\u062f\u064a\u062f \u062a\u062e\u0637\u064a\u0637 \u0627\u0644\u0630\u0627\u0643\u0631\u0629.\n";
        }
    } else {
        // (AR) وضع عادي (غير freestanding) — إضافة الخيارات القياسية
        // (EN) Normal (non-freestanding) mode — add standard options

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
        
        // Add linker script (normal mode)
        if (!options_.linker_script.empty()) {
            command += " -T " + options_.linker_script;
        }
    }
    
    // (AR) في وضع freestanding نضيف أيضاً مسارات/مكتبات المستخدم
    //      (مثلاً مكتبة runtime خاصة بالنواة)
    // (EN) In freestanding mode also add user-specified paths/libs
    //      (e.g., custom kernel runtime library)
    if (options_.freestanding) {
        for (const auto& path : options_.library_paths) {
            command += " -L" + path;
        }
        for (const auto& lib : options_.libraries) {
            command += " -l" + lib;
        }
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

// ============================================================================
// (AR) البحث عن أداة Clang في النظام
// ============================================================================
// نبحث عن Clang في عدة أماكن:
//   1. المسارات المعروفة (C:\LLVM\bin على ويندوز)
//   2. متغير البيئة PATH
//   3. بجوار الملف التنفيذي sadc.exe
//   4. مسارات Visual Studio المعروفة
//
// Clang ضروري لربط ملفات الكائن وإنتاج ملفات تنفيذية
// لأنه يعرف تلقائياً أين توجد مكتبات النظام ومكتبات C
// ============================================================================
// (EN) Find Clang tool on the system
// Clang is needed to link object files into executables because it
// automatically knows where system libraries and C libraries are located.
// ============================================================================
std::optional<std::string> CompilerDriver::find_clang() {
    // (AR) قائمة المسارات المرشحة للبحث عن clang
    // (EN) Candidate paths to search for clang
    std::vector<std::string> candidates;
    
    #ifdef _WIN32
    // (AR) المسارات المعروفة على ويندوز
    // (EN) Known Windows paths
    candidates.push_back("C:\\LLVM\\bin\\clang.exe");
    candidates.push_back("C:\\Program Files\\LLVM\\bin\\clang.exe");
    candidates.push_back("C:\\Program Files (x86)\\LLVM\\bin\\clang.exe");
    
    // (AR) البحث بجوار sadc.exe - قد يكون clang مثبتاً في نفس المجلد
    // (EN) Search next to sadc.exe - clang might be installed alongside
    auto exe_dir = get_executable_dir();
    candidates.push_back((exe_dir / "clang.exe").string());
    candidates.push_back((exe_dir.parent_path() / "clang.exe").string());
    
    // (AR) البحث في مسار LLVM_DIR إن وُجد في البيئة
    // (EN) Search in LLVM_DIR environment variable if set
    const char* llvm_dir = std::getenv("LLVM_DIR");
    if (llvm_dir) {
        candidates.push_back(std::string(llvm_dir) + "\\bin\\clang.exe");
    }
    const char* llvm_path = std::getenv("LLVM_PATH");
    if (llvm_path) {
        candidates.push_back(std::string(llvm_path) + "\\bin\\clang.exe");
    }
    
    // (AR) فحص كل مرشح
    // (EN) Check each candidate
    for (const auto& path : candidates) {
        if (std::filesystem::exists(path)) {
            if (options_.verbose) {
                std::cerr << "  تم العثور على clang: " << path << "\n";
                std::cerr << "  Found clang at: " << path << "\n";
            }
            return path;
        }
    }
    
    // (AR) البحث في PATH باستخدام أمر where (ويندوز)
    // (EN) Search in PATH using 'where' command (Windows)
    if (std::system("where clang.exe > nul 2>&1") == 0) {
        // (AR) clang موجود في PATH - استخرج المسار الكامل
        // (EN) clang is in PATH - extract full path
        FILE* pipe = _popen("where clang.exe 2>nul", "r");
        if (pipe) {
            char buffer[512];
            if (fgets(buffer, sizeof(buffer), pipe)) {
                std::string result(buffer);
                // (AR) إزالة أحرف السطر الجديد
                // (EN) Remove trailing newline/whitespace
                while (!result.empty() && (result.back() == '\n' || result.back() == '\r' || result.back() == ' ')) {
                    result.pop_back();
                }
                _pclose(pipe);
                if (!result.empty() && std::filesystem::exists(result)) {
                    if (options_.verbose) {
                        std::cerr << "  تم العثور على clang في PATH: " << result << "\n";
                    }
                    return result;
                }
            }
            _pclose(pipe);
        }
    }
    
    #else
    // (AR) البحث على لينكس/ماك
    // (EN) Search on Linux/Mac
    candidates.push_back("/usr/bin/clang");
    candidates.push_back("/usr/local/bin/clang");
    candidates.push_back("/usr/lib/llvm-18/bin/clang");
    candidates.push_back("/usr/lib/llvm-17/bin/clang");
    candidates.push_back("/usr/lib/llvm-16/bin/clang");
    
    auto exe_dir = get_executable_dir();
    candidates.push_back((exe_dir / "clang").string());
    
    for (const auto& path : candidates) {
        if (std::filesystem::exists(path)) {
            if (options_.verbose) {
                std::cerr << "  Found clang at: " << path << "\n";
            }
            return path;
        }
    }
    
    // (AR) البحث في PATH على لينكس/ماك
    // (EN) Search in PATH on Linux/Mac
    if (std::system("which clang > /dev/null 2>&1") == 0) {
        FILE* pipe = popen("which clang 2>/dev/null", "r");
        if (pipe) {
            char buffer[512];
            if (fgets(buffer, sizeof(buffer), pipe)) {
                std::string result(buffer);
                while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
                    result.pop_back();
                }
                pclose(pipe);
                if (!result.empty()) return result;
            }
            pclose(pipe);
        }
    }
    #endif
    
    return std::nullopt;
}

// ============================================================================
// (AR) ربط ملف كائن مع مكتبة وقت التشغيل لإنتاج ملف تنفيذي
// ============================================================================
// هذه الدالة تنفذ المرحلة الأخيرة من الترجمة: أخذ ملف الكائن (.obj)
// وربطه مع المكتبات اللازمة لإنتاج ملف تنفيذي (.exe).
//
// الاستراتيجية:
//   1. نبحث عن clang أولاً (الطريقة المفضلة)
//   2. إذا لم نجد clang، نستخدم link.exe (MSVC)
//   3. نبحث عن مكتبة وقت التشغيل (runtime) التي تحتوي على
//      دوال مثل sad_llvm_input, sad_llvm_input_int
//   4. إذا لم نجد المكتبة، ننشئ ملف runtime مؤقت ونترجمه مع البرنامج
//
// ملاحظة مهمة: دوال الطباعة (اطبع) تستخدم printf مباشرة في LLVM IR
// لذا لا تحتاج لمكتبة runtime. لكن دوال الإدخال (اقرأ) تحتاج لها.
// ============================================================================
// (EN) Link object file with runtime library to produce executable
// 
// Strategy:
//   1. Try clang first (preferred - handles libraries automatically)
//   2. Fall back to link.exe (MSVC) if clang not found
//   3. Look for runtime library with functions like sad_llvm_input*
//   4. If no library found, create minimal runtime temp file & compile
//
// Note: Print functions use printf directly in LLVM IR, so no runtime 
// needed for basic output. But input functions (اقرأ) need runtime.
// ============================================================================
bool CompilerDriver::link_object_to_executable(const std::string& obj_path,
                                                const std::string& output_file,
                                                llvm::Module* module) {
    // ================================================================
    // (AR) الخطوة 1: البحث عن أداة الربط (clang أو link.exe)
    // (EN) Step 1: Find linker tool (clang or link.exe)
    // ================================================================
    auto clang_opt = find_clang();
    
    if (clang_opt) {
        // ============================================================
        // (AR) الطريقة المفضلة: استخدام Clang كمحرك للربط
        // ============================================================
        // Clang هو الخيار الأفضل لأنه:
        //   - يعرف أين توجد مكتبات C القياسية (printf, scanf, ...)
        //   - يتعامل مع اختلافات النظام تلقائياً (MSVC vs MinGW)
        //   - يضيف مكتبات بدء التشغيل (CRT startup) تلقائياً
        //   - يدعم إضافة ملفات مصدر مباشرة (للـ runtime)
        // ============================================================
        // (EN) Preferred method: Use Clang as linker driver
        // Clang automatically handles system libraries, CRT startup,
        // and platform differences.
        // ============================================================
        std::string clang = *clang_opt;
        
        // (AR) بناء أمر الربط الأساسي
        // (EN) Build basic link command
        // (AR) ملاحظة: على ويندوز، system() يستخدم cmd.exe الذي له قواعد
        //       اقتباس معقدة. نغلف الأمر بالكامل بعلامات اقتباس خارجية.
        // (EN) Note: On Windows, system() uses cmd.exe which has complex
        //       quoting rules. We wrap the entire command in outer quotes.
        std::string command = "\"" + clang + "\" \"" + obj_path + "\"";
        command += " -o \"" + output_file + "\"";
        // (AR) تعطيل تحذيرات الدوال المهملة (scanf, fopen, ...)
        // (EN) Suppress deprecation warnings (scanf, fopen, ...)
        command += " -w";
        
        // ============================================================
        // (AR) إنشاء ملف runtime مؤقت بلغة C خالصة
        // ============================================================
        // نستخدم ملف C بسيط (ليس C++) لتجنب مشاكل توافق إصدارات
        // Clang مع مكتبات MSVC القياسية (مثل <vector>).
        // هذا الملف يحتوي على الدوال الأساسية فقط:
        //   - دوال الإدخال (اقرأ): sad_llvm_input, sad_llvm_input_int, ...
        //   - دوال الذاكرة البسيطة: sad_llvm_alloc, sad_llvm_free
        // ملاحظة: دوال الطباعة (اطبع) تستخدم printf مباشرة في LLVM IR
        // ============================================================
        // (EN) Create minimal pure-C runtime temp file
        // We use pure C (not C++) to avoid Clang/MSVC STL version issues.
        // ============================================================
        auto temp_runtime = get_temp_file(".c");
        temp_files_.push_back(temp_runtime);
        
        std::ofstream rt_file(temp_runtime);
        if (rt_file.is_open()) {
            rt_file << R"(
/* ============================================================================
 * Sad Language - Minimal Runtime / مكتبة وقت التشغيل المصغرة
 * ============================================================================
 * هذا الملف يُنشأ تلقائياً بواسطة مترجم Sad لتوفير الدوال
 * الأساسية اللازمة لتشغيل البرنامج المترجم.
 * ============================================================================
 * This file is auto-generated by the Sad compiler to provide
 * basic runtime functions needed by the compiled program.
 * ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
  #include <windows.h>
#endif

/* ============================================================================
 * دوال الإدخال / Input Functions
 * ============================================================================ */

/* قراءة نص من المستخدم / Read string from user */
const char* sad_llvm_input(void) {
    // ملاحظة: المخزن المؤقت الثابت ليس آمناً للخيوط المتعددة
    // في الإصدارات المستقبلية، يجب استخدام thread_local أو تخصيص ديناميكي
    static char buffer[4096];
    buffer[0] = '\0';  // تهيئة بقيمة افتراضية
    
    if (fgets(buffer, sizeof(buffer), stdin)) {
        size_t len = strlen(buffer);
        // إزالة حرف السطر الجديد
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        return buffer;
    }
    return "";
}

/* قراءة عدد صحيح من المستخدم / Read integer from user */
long long sad_llvm_input_int(void) {
    long long val = 0;
    
    // التحقق من نجاح scanf
    if (scanf("%lld", &val) != 1) {
        // فشل القراءة - تنظيف المخزن المؤقت
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return 0;  // قيمة افتراضية
    }
    
    // تنظيف المخزن المؤقت بعد القراءة الناجحة
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    return val;
}

/* قراءة عدد عشري من المستخدم / Read float from user */
double sad_llvm_input_float(void) {
    double val = 0.0;
    
    // التحقق من نجاح scanf
    if (scanf("%lf", &val) != 1) {
        // فشل القراءة - تنظيف المخزن المؤقت
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return 0.0;  // قيمة افتراضية
    }
    
    // تنظيف المخزن المؤقت بعد القراءة الناجحة
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    return val;
}

/* ============================================================================
 * دوال الذاكرة / Memory Functions
 * ============================================================================ */

void* sad_llvm_alloc(unsigned long long size) {
    // التحقق من الحجم المعقول (حد أقصى 1 جيجابايت)
    const unsigned long long MAX_ALLOC = 1ULL << 30;
    if (size == 0 || size > MAX_ALLOC) {
        fprintf(stderr, "[sad] تحذير: طلب تخصيص غير صالح: %llu بايت\n", size);
        return NULL;
    }
    
    void* ptr = malloc((size_t)size);
    if (!ptr) {
        fprintf(stderr, "[sad] خطأ: فشل تخصيص %llu بايت\n", size);
    }
    return ptr;
}

void sad_llvm_free(void* ptr) {
    if (ptr) free(ptr);
}

/* ============================================================================
 * دوال النصوص / String Functions (basic)
 * ============================================================================ */

void* sad_llvm_string_new(const char* data, unsigned long long length) {
    // إصلاح: التحقق من المدخلات قبل التخصيص
    if (!data || length > (1ULL << 30)) {  // حد أقصى 1GB
        return NULL;
    }
    char* str = (char*)malloc((size_t)(length + 1));
    if (!str) {
        // فشل التخصيص - تسجيل خطأ
        fprintf(stderr, "[sad] خطأ: فشل تخصيص %llu بايت\n", length + 1);
        return NULL;
    }
    memcpy(str, data, (size_t)length);
    str[length] = '\0';
    return str;
}

void* sad_llvm_string_from_cstr(const char* cstr) {
    if (!cstr) return NULL;
    size_t len = strlen(cstr);
    // إصلاح: التحقق من الحجم المعقول
    if (len > (1ULL << 30)) {  // حد أقصى 1GB
        fprintf(stderr, "[sad] خطأ: نص طويل جداً\n");
        return NULL;
    }
    char* str = (char*)malloc(len + 1);
    if (!str) {
        fprintf(stderr, "[sad] خطأ: فشل تخصيص %zu بايت\n", len + 1);
        return NULL;
    }
    memcpy(str, cstr, len + 1);
    return str;
}

unsigned long long sad_llvm_string_length(void* str) {
    if (!str) return 0;
    return (unsigned long long)strlen((const char*)str);
}

void sad_llvm_print_string(void* str) {
    if (str) printf("%s", (const char*)str);
    else printf("null");
}

/* ============================================================================
 * دوال الأمان / Security Functions
 * ============================================================================ */

#include <time.h>

/* تأكد / Assert — abort on false */
void sad_security_assert(int condition, const char* msg) {
    if (!condition) {
        fprintf(stderr, "[ASSERT FAILED] %s\n", msg ? msg : "assertion failed");
        abort();
    }
}

/* ذعر / Panic — emergency halt */
void sad_security_panic(const char* msg) {
    fprintf(stderr, "[PANIC] %s\n", msg ? msg : "panic");
    abort();
}

/* هاش / Hash — FNV-1a */
long long sad_security_hash(const char* str) {
    if (!str) return 0;
    unsigned long long hash = 14695981039346656037ULL;
    while (*str) {
        hash ^= (unsigned char)*str++;
        hash *= 1099511628211ULL;
    }
    return (long long)hash;
}

/* شفّر / Encrypt — XOR cipher (للاستخدام البسيط فقط، ليس للأمان الحقيقي) */
const char* sad_security_encrypt(const char* text, const char* key) {
    if (!text || !key || !*key) return text;
    
    size_t tlen = strlen(text);
    size_t klen = strlen(key);
    
    // التحقق من الحد الأقصى للحجم لمنع overflow
    if (tlen > (SIZE_MAX / 4 - 1)) {
        fprintf(stderr, "[sad] خطأ: النص طويل جداً للتشفير\n");
        return text;
    }
    
    char* result = (char*)malloc(tlen * 2 + 1);  // hex encoding: 2 chars per byte
    if (!result) {
        fprintf(stderr, "[sad] خطأ: فشل تخصيص الذاكرة للتشفير\n");
        return text;
    }
    
    size_t pos = 0;
    for (size_t i = 0; i < tlen; i++) {
        unsigned char c = (unsigned char)text[i] ^ (unsigned char)key[i % klen];
        // استخدام snprintf بدلاً من sprintf للأمان
        int written = snprintf(result + pos, 3, "%02x", c);
        if (written != 2) {
            free(result);
            return text;
        }
        pos += 2;
    }
    result[pos] = '\0';
    
    return result;
}

/* فك_تشفير / Decrypt — XOR decipher */
const char* sad_security_decrypt(const char* hex, const char* key) {
    if (!hex || !key || !*key) return hex;
    
    size_t hlen = strlen(hex);
    if (hlen % 2 != 0) {
        fprintf(stderr, "[sad] خطأ: طول النص المشفر غير صالح\n");
        return hex;
    }
    
    size_t rlen = hlen / 2;
    size_t klen = strlen(key);
    
    char* result = (char*)malloc(rlen + 1);
    if (!result) {
        fprintf(stderr, "[sad] خطأ: فشل تخصيص الذاكرة لفك التشفير\n");
        return hex;
    }
    
    for (size_t i = 0; i < rlen; i++) {
        unsigned int byte = 0;
        // التحقق من نجاح sscanf
        if (sscanf(hex + i * 2, "%02x", &byte) != 1) {
            free(result);
            fprintf(stderr, "[sad] خطأ: تنسيق hex غير صالح عند الموقع %zu\n", i);
            return hex;
        }
        // التحقق من أن القيمة في النطاق الصحيح
        if (byte > 255) {
            free(result);
            return hex;
        }
        result[i] = (char)((unsigned char)byte ^ (unsigned char)key[i % klen]);
    }
    result[rlen] = '\0';
    
    return result;
}

/* تأكد_نوع / Assert type */
void sad_security_assert_type(const char* expected, const char* actual) {
    if (!expected || !actual || strcmp(expected, actual) != 0) {
        fprintf(stderr, "[TYPE ASSERT FAILED] expected=%s actual=%s\n",
                expected ? expected : "null", actual ? actual : "null");
        abort();
    }
}

/* تأكد_مساواة / Assert equal (strings) */
void sad_security_assert_equal_str(const char* a, const char* b) {
    if (!a || !b || strcmp(a, b) != 0) {
        fprintf(stderr, "[EQUAL ASSERT FAILED] '%s' != '%s'\n",
                a ? a : "null", b ? b : "null");
        abort();
    }
}

/* نظّف / Sanitize — HTML entity encoding */
const char* sad_security_sanitize(const char* text) {
    if (!text) return "";
    size_t len = strlen(text);
    char* result = (char*)malloc(len * 6 + 1);
    if (!result) return text;
    size_t pos = 0;
    for (size_t i = 0; i < len; i++) {
        switch (text[i]) {
            case '<':  memcpy(result+pos, "&lt;", 4); pos+=4; break;
            case '>':  memcpy(result+pos, "&gt;", 4); pos+=4; break;
            case '&':  memcpy(result+pos, "&amp;", 5); pos+=5; break;
            case '"':  memcpy(result+pos, "&quot;", 6); pos+=6; break;
            case '\'': memcpy(result+pos, "&#39;", 5); pos+=5; break;
            default:   result[pos++] = text[i]; break;
        }
    }
    result[pos] = '\0';
    return result;
}

/* وقت_الآن / Current timestamp */
long long sad_security_timestamp(void) {
    return (long long)time(NULL);
}

/* عشوائي_آمن / Secure random in range */
long long sad_security_secure_random(long long min_val, long long max_val) {
    static int seeded = 0;
    if (!seeded) { srand((unsigned int)time(NULL)); seeded = 1; }
    if (min_val >= max_val) return min_val;
    long long range = max_val - min_val + 1;
    return min_val + (long long)(rand() % (int)range);
}

/* ترميز_64 / Base64 encode */
const char* sad_security_base64_encode(const char* text) {
    static const char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    if (!text) return "";
    size_t len = strlen(text);
    size_t olen = 4 * ((len + 2) / 3);
    char* result = (char*)malloc(olen + 1);
    if (!result) return "";
    size_t j = 0;
    for (size_t i = 0; i < len; i += 3) {
        unsigned int a = (unsigned char)text[i];
        unsigned int b2 = (i+1 < len) ? (unsigned char)text[i+1] : 0;
        unsigned int c = (i+2 < len) ? (unsigned char)text[i+2] : 0;
        unsigned int triple = (a << 16) | (b2 << 8) | c;
        result[j++] = b64[(triple >> 18) & 0x3F];
        result[j++] = b64[(triple >> 12) & 0x3F];
        result[j++] = (i+1 < len) ? b64[(triple >> 6) & 0x3F] : '=';
        result[j++] = (i+2 < len) ? b64[triple & 0x3F] : '=';
    }
    result[j] = '\0';
    return result;
}
)";
            rt_file.close();
            command += " \"" + temp_runtime.string() + "\"";
            
            if (options_.verbose) {
                std::cerr << "  استخدام runtime مؤقت: " << temp_runtime.string() << "\n";
                std::cerr << "  Using embedded runtime: " << temp_runtime.string() << "\n";
            }
        }
        
        // (AR) إضافة مسارات المكتبات المحددة من المستخدم
        // (EN) Add user-specified library paths
        for (const auto& path : options_.library_paths) {
            command += " -L\"" + path + "\"";
        }
        
        // (AR) إضافة المكتبات المحددة من المستخدم
        // (EN) Add user-specified libraries
        for (const auto& lib : options_.libraries) {
            command += " -l" + lib;
        }
        
        // (AR) إضافة علم الربط الثابت إذا طُلب
        // (EN) Add static linking flag if requested
        if (options_.link_static) {
            command += " -static";
        }
        
        if (options_.verbose) {
            std::cerr << "\n  أمر الربط / Link command:\n  " << command << "\n\n";
        }
        
        // (AR) تنفيذ أمر الربط
        // (EN) Execute link command
        // (AR) على ويندوز، cmd.exe يحتاج لتغليف الأمر بالكامل بعلامات اقتباس
        //       إضافية عندما يحتوي المسار التنفيذي على علامات اقتباس
        // (EN) On Windows, cmd.exe needs the entire command wrapped in quotes
        //       when the executable path contains quotes
        #ifdef _WIN32
        int result = std::system(("\"" + command + "\"").c_str());
        #else
        int result = std::system(command.c_str());
        #endif
        
        if (result != 0) {
            diagnostics_.report_fatal(
                "فشل ربط الملف التنفيذي (رمز الخطأ: " + std::to_string(result) + ")\n"
                "Failed to link executable (error code: " + std::to_string(result) + ")\n"
                "تأكد من تثبيت clang وأنه يعمل بشكل صحيح\n"
                "Make sure clang is installed and working correctly"
            );
            return false;
        }
        
        return true;
        
    } else {
        // ============================================================
        // (AR) الطريقة البديلة: استخدام link.exe (MSVC) مباشرة
        // ============================================================
        // إذا لم نجد clang، نحاول استخدام link.exe من Visual Studio
        // هذا أعقد قليلاً لأننا نحتاج لتحديد مكتبات C يدوياً
        // ============================================================
        // (EN) Fallback: Use link.exe (MSVC) directly
        // More complex since we need to specify C libraries manually
        // ============================================================
        #ifdef _WIN32
        // (AR) حاول العثور على link.exe
        // (EN) Try to find link.exe
        if (std::system("where link.exe > nul 2>&1") == 0) {
            // (AR) بناء أمر link.exe
            // (EN) Build link.exe command
            std::string command = "link.exe /OUT:\"" + output_file + "\"";
            command += " \"" + obj_path + "\"";
            
            // (AR) مكتبات C القياسية اللازمة
            // (EN) Required C standard libraries  
            command += " libcmt.lib";     // C runtime
            command += " libucrt.lib";    // Universal CRT  
            command += " kernel32.lib";   // Windows kernel
            command += " legacy_stdio_definitions.lib"; // printf/scanf
            
            // (AR) إضافة مسارات المكتبات
            for (const auto& path : options_.library_paths) {
                command += " /LIBPATH:\"" + path + "\"";
            }
            
            for (const auto& lib : options_.libraries) {
                command += " " + lib + ".lib";
            }
            
            command += " /NOLOGO /SUBSYSTEM:CONSOLE";
            
            if (options_.verbose) {
                std::cerr << "  أمر الربط / Link command: " << command << "\n";
            }
            
            int result = std::system(command.c_str());
            if (result != 0) {
                diagnostics_.report_fatal(
                    "فشل الربط بـ link.exe (رمز الخطأ: " + std::to_string(result) + ")\n"
                    "Linking with link.exe failed (error code: " + std::to_string(result) + ")"
                );
                return false;
            }
            return true;
        }
        #endif
        
        // (AR) لم نجد أي أداة ربط
        // (EN) No linker found
        diagnostics_.report_fatal(
            "لم يتم العثور على أداة ربط (clang أو link.exe)\n"
            "No linker found (clang or link.exe)\n"
            "قم بتثبيت LLVM/Clang من https://llvm.org/\n"
            "Install LLVM/Clang from https://llvm.org/"
        );
        return false;
    }
}

void CompilerDriver::print_version(std::ostream& os) {
    os << colors::BOLD << get_compiler_name() << " " << get_version() << colors::RESET << "\n";
    os << "Copyright (C) 2024 Sad Language Project\n";
    os << "This is free software; see the source for copying conditions.\n";
    
    #if defined(ENABLE_LLVM) || defined(ENABLE_LLVM_BACKEND)
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
    os << "  sadc program.ص -o program           " 
       << colors::CYAN << "# Compile to executable\n" << colors::RESET;
    os << "  sadc program.ص -O3 -o program       " 
       << colors::CYAN << "# With optimizations\n" << colors::RESET;
    os << "  sadc program.ص -c -o program.o      " 
       << colors::CYAN << "# Compile to object file\n" << colors::RESET;
    os << "  sadc program.ص --emit-llvm -o prog.ll " 
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
    os << "  --freestanding         " << "Freestanding (no OS) / مستقل بلا نظام تشغيل\n";
    os << "\n";

    os << "Freestanding / وضع بلا مكتبة قياسية:\n";
    os << "  --freestanding         " << "Full bare-metal mode / وضع bare-metal كامل\n";
    os << "                         " << "(مكافئ لـ --no-std)\n";
    os << "  --no-std               " << "Same as --freestanding / نفس --freestanding\n";
    os << "  --no-main              " << "No default main() entry / تعطيل main الافتراضية\n";
    os << "  --abort-on-panic       " << "Abort instead of unwind / إيقاف عند الذعر\n";
    os << "  --linker-script=<f>    " << "Use linker script (.ld) / سكريبت رابط\n";
    os << "  --entry-point=<name>   " << "Custom entry point name / اسم نقطة الدخول\n";
    os << "  --allow-alloc          " << "Allow heap allocation / السماح بالتخصيص الديناميكي\n";
    os << "\n";
    os << "  مثال نواة بسيطة / Minimal kernel example:\n";
    os << "    sadc --freestanding --no-main --abort-on-panic \\\n";
    os << "         --linker-script=kernel.ld kernel.ص -o kernel.elf\n";
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
    os << "\n";
    
    os << "Ownership & Borrow Checking / نظام الملكية والاستعارة:\n";
    os << "  --borrow-check         " << "Enable borrow checking (default) / تفعيل فحص الاستعارة\n";
    os << "  --فحص-الاستعارة         " << "فحص الاستعارة (عربي)\n";
    os << "  --no-borrow-check      " << "Disable borrow checking / تعطيل فحص الاستعارة\n";
    os << "  --debug-borrow         " << "Debug borrow checker output / تنقيح فحص الاستعارة\n";
    os << "  --arabic-borrow        " << "Arabic borrow error messages / رسائل عربية\n";
    os << "  --english-borrow       " << "English borrow error messages / رسائل إنجليزية\n";
    os << "\n";
    
    os << "Type Checking / فحص الأنواع المتقدم:\n";
    os << "  --type-check           " << "Enable type checking (default) / تفعيل فحص الأنواع\n";
    os << "  --فحص-الأنواع           " << "فحص الأنواع (عربي)\n";
    os << "  --no-type-check        " << "Disable type checking / تعطيل فحص الأنواع\n";
    os << "  --debug-types          " << "Debug type checker output / تنقيح فحص الأنواع\n";
    os << "  --strict-types         " << "Strict type checking mode / وضع صارم للأنواع\n";
    os << "  --أنواع-صارمة          " << "وضع صارم للأنواع (عربي)\n";
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
    else if (arg.size() >= 9 && arg.substr(0, 9) == "--target=") {
        std::string triple_str = arg.substr(9);
        auto triple_opt = TargetTriple::parse(triple_str);
        if (triple_opt) {
            options.target = *triple_opt;
        } else {
            diag.report_error("invalid target triple: " + triple_str);
            return false;
        }
    } else if (arg == "--freestanding") {
        // ────────────────────────────────────────────────────────────────────
        // (AR) تفعيل وضع bare-metal الكامل:
        //   - يُعطِّل جميع مكتبات C/C++ القياسية
        //   - يُلزم وجود معالج ذعر مخصص
        //   - يُلزم وجود نقطة دخول مخصصة
        //   - يُضيف -nostdlib -nostartfiles -nodefaultlibs للرابط
        //
        // (EN) Enable full bare-metal mode:
        //   - Disables all C/C++ standard libraries
        //   - Requires custom panic handler
        //   - Requires custom entry point
        //   - Adds -nostdlib -nostartfiles -nodefaultlibs to linker
        // ────────────────────────────────────────────────────────────────────
        options.freestanding = true;

    } else if (arg == "--no-std" || arg == "--nostd") {
        // (AR) مرادف لـ --freestanding
        // (EN) Alias for --freestanding
        options.freestanding = true;

    } else if (arg == "--no-main" || arg == "--nomain") {
        // (AR) تعطيل نقطة الدخول الافتراضية main()
        //      يُستخدم مع --freestanding لتعريف _start المخصص
        // (EN) Disable default main() entry point
        options.no_main = true;

    } else if (arg == "--abort-on-panic") {
        // (AR) إيقاف عند الذعر بدلاً من stack unwinding
        //      مناسب لأنظمة لا تدعم C++ exceptions
        // (EN) Abort on panic instead of stack unwinding
        options.abort_on_panic = true;

    } else if (arg.size() >= 16 && arg.substr(0, 16) == "--linker-script=") {
        // (AR) مسار سكريبت الرابط (.ld) لتعريف تخطيط الذاكرة
        //      مثال: --linker-script=kernel.ld
        // (EN) Linker script path (.ld) to define memory layout
        options.linker_script = arg.substr(16);

    } else if (arg.size() >= 14 && arg.substr(0, 14) == "--entry-point=") {
        // (AR) اسم دالة نقطة الدخول المخصصة
        //      مثال: --entry-point=kernel_main
        // (EN) Custom entry point function name
        options.freestanding_entry = arg.substr(14);

    } else if (arg == "--allow-alloc") {
        // (AR) السماح بالتخصيص الديناميكي في وضع freestanding
        //      يتطلب تسجيل مُخصّص مخصص (#[معالج_تخصيص])
        // (EN) Allow dynamic allocation in freestanding (requires custom allocator)
        options.allow_freestanding_alloc = true;

    // ─── ربط المكتبات ───────────────────────────────────────────────────────
    // (AR) خيارات الرابط (Linker): مسارات المكتبات وأسماؤها ونوع الربط
    // (EN) Linker options: library paths, library names, and link type
    } else if (arg.size() >= 2 && arg.substr(0, 2) == "-L") {
        options.library_paths.push_back(arg.substr(2));
    } else if (arg.size() >= 2 && arg.substr(0, 2) == "-l") {
        options.libraries.push_back(arg.substr(2));
    } else if (arg == "--static") {
        options.link_static = true;

    // ─── خيارات متنوعة أخرى ─────────────────────────────────────────────────
    // (AR) خيارات عامة: الإسهاب، التحذيرات كأخطاء، والألوان
    // (EN) General options: verbosity, warnings-as-errors, color output
    } else if (arg == "-v" || arg == "--verbose") {
        options.verbose = true;
    } else if (arg == "-Werror") {
        options.warnings_as_errors = true;
    } else if (arg == "--color") {
        options.color_diagnostics = true;
    } else if (arg == "--no-color") {
        options.color_diagnostics = false;
        colors::disable_colors();

    // ─── خيارات نظام الملكية والاستعارة ────────────────────────────────────
    // (AR) التحكم في فحص الملكية والاستعارة (Ownership & Borrow Checker)
    // (EN) Ownership & Borrow Check options
    } else if (arg == "--borrow-check" || arg == "--فحص-الاستعارة" || arg == "--ملكية") {
        options.enable_borrow_check = true;
    } else if (arg == "--no-borrow-check" || arg == "--بدون-فحص-استعارة") {
        options.enable_borrow_check = false;
    } else if (arg == "--debug-borrow" || arg == "--تنقيح-الاستعارة") {
        options.debug_borrow_check = true;
    } else if (arg == "--arabic-borrow" || arg == "--رسائل-عربية") {
        options.arabic_borrow_messages = true;
    } else if (arg == "--english-borrow") {
        options.arabic_borrow_messages = false;
    }
    
    // (AR) خيارات فحص الأنواع المتقدم / Type Check options
    else if (arg == "--type-check" || arg == "--فحص-الأنواع") {
        options.enable_type_check = true;
    } else if (arg == "--no-type-check" || arg == "--بدون-فحص-أنواع") {
        options.enable_type_check = false;
    } else if (arg == "--debug-types" || arg == "--تنقيح-الأنواع") {
        options.debug_type_check = true;
    } else if (arg == "--strict-types" || arg == "--أنواع-صارمة") {
        options.strict_type_check = true;
    }
    
    // ─── خيارات خط أنابيب الواجهات الرسومية / UI Pipeline options ───────
    // (AR) --ui/--واجهات: تفعيل وضع توليد الواجهات (سطح مكتب، اندرويد، iOS، ويب)
    // (EN) --ui: Enable UI code generation mode
    else if (arg == "--ui" || arg == "--واجهات") {
        options.emit_ui = true;
    }
    // --platform=<name> / --منصة=<name>
    else if (arg.size() >= 11 && arg.substr(0, 11) == "--platform=") {
        options.ui_platform = arg.substr(11);
        options.emit_ui = true;
    } else if (arg.size() > 2 && arg.find("--منصة=") == 0) {
        auto eqPos = arg.find('=');
        if (eqPos != std::string::npos) {
            options.ui_platform = arg.substr(eqPos + 1);
            options.emit_ui = true;
        }
    }
    // Shorthand platform flags
    else if (arg == "--desktop" || arg == "--سطح-المكتب") {
        options.emit_ui = true;
        options.ui_platform = "desktop";
    } else if (arg == "--android" || arg == "--اندرويد") {
        options.emit_ui = true;
        options.ui_platform = "android";
    } else if (arg == "--ios" || arg == "--ايفون") {
        options.emit_ui = true;
        options.ui_platform = "ios";
    } else if (arg == "--web" || arg == "--ويب") {
        options.emit_ui = true;
        options.ui_platform = "web";
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
    return sad::utf8::get_extension(path);
}

std::string change_extension(const std::string& path, const std::string& new_ext) {
    return sad::utf8::change_extension(path, new_ext);
}

bool file_exists(const std::string& path) {
    return sad::utf8::file_exists(path);
}

std::optional<std::string> read_file(const std::string& path) {
    return sad::utf8::read_file(path);
}

std::filesystem::path get_executable_dir() {
    return sad::utf8::get_executable_dir();
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
    std::string base = sad::utf8::get_stem(input_file);
    
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
