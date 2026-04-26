// بسم الله الرحمن الرحيم
// ============================================================================
// Compiler Driver — Backend (Middle-end + Code Generation)
// المحرك — الواجهة الخلفية (التحسين الوسيط + توليد الكود)
// ============================================================================
// هذا الملف يحتوي على:
//   - run_middleend()         : تحسين SIR الوسيط (O0-O3)
//   - run_backend()           : توليد LLVM IR/BC/OBJ/ASM/EXE + كوروتين
//   - print_ir_if_requested() : طباعة SIR إذا طُلب
// ============================================================================

#include "compiler_driver.h"
#include "../../shared/utils/include/utf8_utils.h"
#include "../../compiler/include/frontend/sir_module.h"
#include "../../compiler/include/backend/llvm/llvm_codegen.h"
#include "../../compiler/include/backend/llvm/arabic_optimizer.h"
#include "../../compiler/include/middle/optimizer.h"
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Transforms/Coroutines/CoroEarly.h>
#include <llvm/Transforms/Coroutines/CoroSplit.h>
#include <llvm/Transforms/Coroutines/CoroElide.h>
#include <llvm/Transforms/Coroutines/CoroCleanup.h>

#ifdef _WIN32
#include <windows.h>
#undef CYAN
#undef YELLOW
#undef RED
#undef GREEN
#undef BLUE
#undef MAGENTA
#undef WHITE
#undef ERROR
#undef FATAL
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <cstdio>

namespace sad
{
    namespace driver
    {

        bool CompilerDriver::run_middleend()
        {
            if (options_.verbose)
            {
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
            if (options_.opt_level == OptimizationLevel::O0)
            {
                if (options_.verbose)
                {
                    std::cout << "  ✓ Optimization skipped (O0) / تم تخطي التحسين (O0)\n";
                }
                return true;
            }

            // (AR) التحقق من وجود وحدة SIR صالحة
            // (EN) Verify valid SIR module exists
            if (!sir_module_)
            {
                if (options_.verbose)
                {
                    std::cout << "  ⚠ No SIR module to optimize / لا توجد وحدة SIR للتحسين\n";
                }
                return true;
            }

            // (AR) تحويل مستوى التحسين من صيغة المترجم إلى صيغة المحسّن
            //      Os و Oz يُعاملان كـ O2 (تحسينات عادية مع تركيز على الحجم في LLVM لاحقاً)
            // (EN) Convert optimization level from driver format to optimizer format
            //      Os and Oz are treated as O2 (standard optimizations, size focus at LLVM level later)
            Sad::Compiler::Optimizer::OptimizationLevel optLevel;
            switch (options_.opt_level)
            {
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
            try
            {
                auto optimizer = Sad::Compiler::Optimizer::createOptimizer(optLevel);

                if (options_.verbose)
                {
                    optimizer->setDebugMode(true);
                }

                bool changed = optimizer->optimize(sir_module_);

                if (options_.verbose)
                {
                    optimizer->printStats();
                    if (changed)
                    {
                        std::cout << "  ✓ Optimization applied / تم تطبيق التحسينات\n";
                    }
                    else
                    {
                        std::cout << "  ✓ No optimizations needed / لا حاجة لتحسينات\n";
                    }
                }
            }
            catch (const std::exception &e)
            {
                // (AR) خطأ في التحسين — نتابع بدون تحسين بدلاً من الفشل الكامل
                //      هذا أفضل من إيقاف عملية الترجمة بالكامل
                // (EN) Optimization error — continue without optimization instead of full failure
                //      This is better than stopping the entire compilation process
                if (options_.verbose)
                {
                    std::cerr << "  ⚠ Optimization error (continuing): " << e.what() << "\n";
                    std::cerr << "  ⚠ خطأ في التحسين (سنتابع): " << e.what() << "\n";
                }
            }

            return true;
        }

        bool CompilerDriver::run_backend()
        {
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

            if (!sir_module_)
            {
                diagnostics_.report_fatal("لا توجد وحدة SIR متاحة للواجهة الخلفية / No SIR module available for backend");
                return false;
            }

            // ============================================================================
            // (AR) اختيار الواجهة الخلفية بناءً على نوع الإخراج المطلوب
            // (EN) Select backend based on requested output type
            // ============================================================================
            switch (options_.output_type)
            {
            case OutputType::BYTECODE:
            {
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
            case OutputType::STATIC_LIBRARY:
            {

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

                // (AR) تمرير وضع الوحدة لتخطي main wrapper
                // (EN) Pass module mode to skip main wrapper
                llvm_codegen_->setModuleMode(options_.module_mode);

                // (AR) تمرير وضع مستقل لتوليد وقت تشغيل مدمج
                // (EN) Pass freestanding mode to emit built-in runtime
                llvm_codegen_->setFreestanding(options_.freestanding);

                // (AR) نستخدم اسم الملف بدون المسار كـ module name
                //      لتجنب تعطل LLVM COFF writer مع الأحرف العربية في المسار
                // (EN) Use just the filename stem as module name to avoid
                //      LLVM COFF writer crash with Arabic chars in path
                std::string module_name = options_.input_files.empty()
                                              ? "sad_module"
                                              : sad::utf8::get_stem(options_.input_files[0]);
                if (!(*llvm_codegen_).initialize(module_name, options_.target.to_string()))
                {
                    diagnostics_.report_fatal("فشل تهيئة الواجهة الخلفية LLVM / Failed to initialize LLVM backend");
                    return false;
                }

                // (AR) تطبيق وضع LTO حسب خيارات سطر الأوامر
                // (EN) Apply LTO mode from CLI options
                llvm_codegen_->setLTOMode(options_.enable_lto_full, options_.enable_lto_thin);
                if (options_.verbose && (options_.enable_lto_full || options_.enable_lto_thin))
                {
                    std::cout << "  ✓ LTO enabled: "
                              << (options_.enable_lto_full ? "Full" : "Thin") << "\n";
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
                if (!llvm_module)
                {
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

                    if (options_.verbose)
                    {
                        arabic_pass.printStats();
                    }
                }
                // ================================================================
                // (AR) الخطوة 2.6: تشغيل تمريرات الكوروتين (لدعم غير_متزامن/انتظر)
                // ================================================================
                // (EN) Step 2.6: Run coroutine passes (for async/await support)
                // This transforms coroutine intrinsics into state machine code
                // Required before lli can execute the IR
                // ================================================================
                {
                    // Check if module has any coroutines (presplitcoroutine attribute)
                    bool hasCoroutines = false;
                    for (auto &F : *llvm_module)
                    {
                        if (F.hasFnAttribute(llvm::Attribute::PresplitCoroutine))
                        {
                            hasCoroutines = true;
                            break;
                        }
                    }

                    if (hasCoroutines)
                    {
                        llvm::LoopAnalysisManager LAM;
                        llvm::FunctionAnalysisManager FAM;
                        llvm::CGSCCAnalysisManager CGAM;
                        llvm::ModuleAnalysisManager MAM;

                        llvm::PassBuilder PB;
                        PB.registerModuleAnalyses(MAM);
                        PB.registerCGSCCAnalyses(CGAM);
                        PB.registerFunctionAnalyses(FAM);
                        PB.registerLoopAnalyses(LAM);
                        PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

                        llvm::ModulePassManager MPM;
                        MPM.addPass(llvm::CoroEarlyPass());

                        // CoroSplit requires CGSCC pass manager
                        llvm::CGSCCPassManager CGPM;
                        CGPM.addPass(llvm::CoroSplitPass());
                        MPM.addPass(llvm::createModuleToPostOrderCGSCCPassAdaptor(std::move(CGPM)));

                        MPM.addPass(llvm::CoroCleanupPass());

                        // Also run CoroElide at function level
                        llvm::FunctionPassManager FPM;
                        FPM.addPass(llvm::CoroElidePass());
                        MPM.addPass(llvm::createModuleToFunctionPassAdaptor(std::move(FPM)));

                        MPM.run(*llvm_module, MAM);

                        if (options_.verbose)
                        {
                            std::cerr << "[CORO] Coroutine passes completed successfully" << std::endl;
                        }
                    }
                }

                // ================================================================
                // (AR) الخطوة 3: إصدار الإخراج حسب النوع المطلوب
                // ================================================================
                // (EN) Step 3: Emit output based on requested type
                // ================================================================

                if (options_.output_type == OutputType::LLVM_IR)
                {
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

                    if (EC)
                    {
                        diagnostics_.report_fatal("فشل فتح ملف الإخراج: " + EC.message() +
                                                  " / Failed to open output file: " + EC.message());
                        return false;
                    }

                    llvm_module->print(dest, nullptr);
                    dest.close();
                }
                else if (options_.output_type == OutputType::LLVM_BC)
                {
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

                    if (EC)
                    {
                        diagnostics_.report_fatal("فشل فتح ملف الإخراج: " + EC.message() +
                                                  " / Failed to open output file: " + EC.message());
                        return false;
                    }

                    llvm::WriteBitcodeToFile(*llvm_module, dest);
                    dest.close();
                }
                else if (options_.output_type == OutputType::OBJECT_FILE)
                {
                    // ==============================================================
                    // (AR) إخراج ملف كائن (.obj / .o)
                    // ==============================================================
                    if (!(*llvm_codegen_).emitObjectFile(options_.output_file, llvm_module.get()))
                    {
                        diagnostics_.report_fatal("فشل إنتاج ملف الكائن / Failed to emit object file");
                        return false;
                    }
                }
                else if (options_.output_type == OutputType::ASSEMBLY)
                {
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
                    if (!(*llvm_codegen_).emitAssembly(options_.output_file, llvm_module.get()))
                    {
                        diagnostics_.report_fatal("فشل إنتاج ملف التجميع / Failed to emit assembly file");
                        return false;
                    }
                }
                else if (options_.output_type == OutputType::EXECUTABLE)
                {
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

                    if (options_.verbose)
                    {
                        std::cerr << "  [الخطوة أ] إنتاج ملف كائن مؤقت: " << obj_path << "\n";
                        std::cerr << "  [Step A] Generating temp object file: " << obj_path << "\n";
                    }

                    if (!(*llvm_codegen_).emitObjectFile(obj_path, llvm_module.get()))
                    {
                        diagnostics_.report_fatal("فشل إنتاج ملف الكائن المؤقت / Failed to emit temporary object file");
                        return false;
                    }

                    // ------- المرحلة 2: ربط ملف الكائن لإنتاج ملف تنفيذي -------
                    // (EN) Phase 2: Link object file to produce executable
                    if (options_.verbose)
                    {
                        std::cerr << "  [الخطوة ب] ربط الملفات لإنتاج: " << options_.output_file << "\n";
                        std::cerr << "  [Step B] Linking to produce: " << options_.output_file << "\n";
                    }
                    if (!link_object_to_executable(obj_path, options_.output_file, llvm_module.get()))
                    {
                        return false;
                    }
                }
                else if (options_.output_type == OutputType::SHARED_LIBRARY)
                {
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

                    if (!(*llvm_codegen_).emitObjectFile(obj_path, llvm_module.get()))
                    {
                        diagnostics_.report_fatal("فشل إنتاج ملف الكائن المؤقت / Failed to emit temporary object file");
                        return false;
                    }

                    // (AR) ربط كمكتبة مشتركة
                    // (EN) Link as shared library
                    std::vector<std::string> objects = {obj_path};
                    if (!invoke_linker(objects, options_.output_file))
                    {
                        diagnostics_.report_fatal("فشل ربط المكتبة المشتركة / Failed to link shared library");
                        return false;
                    }
                }
                else if (options_.output_type == OutputType::STATIC_LIBRARY)
                {
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

                    if (!(*llvm_codegen_).emitObjectFile(obj_path, llvm_module.get()))
                    {
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

                    if (options_.verbose)
                    {
                        std::cerr << "  تشغيل: " << ar_command << "\n";
                    }

                    if (std::system(ar_command.c_str()) != 0)
                    {
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

        void CompilerDriver::print_ir_if_requested()
        {
            if (options_.emit_sir && sir_module_)
            {
                std::cout << "\n========== SIR ==========\n";
                sir_module_->print(); // print() doesn't take ostream parameter
                std::cout << "=========================\n\n";
            }
        }

    } // namespace driver
} // namespace sad
// ״¨״³… ״§„„‡ ״§„״±״­…† ״§„״±״­