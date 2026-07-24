// بسم الله الرحمن الرحيم
// ============================================================================
// Compiler Driver — Backend (Middle-end + Code Generation)
// مشغّل المترجم — الواجهة الخلفية (المرحلة الوسطى + توليد الكود)
// ============================================================================
// هذا الملف يحتوي على:
//   - run_middleend()         : تحسين SIR الوسيط (O0-O3)
//   - run_backend()           : توليد LLVM IR/BC/OBJ/ASM/EXE + الربط
//   - print_ir_if_requested() : طباعة SIR عند الطلب
// ============================================================================

#include "compiler_driver.h"
#include "utf8_utils.h"
#include "error_codes.h" // (AR) ErrorCode::INT_SIR_FIELD_LAYOUT لبوّابة التوليد المُنمَّطة / (EN) code-scoped codegen gate
#include "../../compiler/include/frontend/sir_module.h"
#include "../../compiler/include/backend/llvm/llvm_codegen.h"
#include "../../compiler/include/backend/llvm/arabic_optimizer.h"
#include "../../compiler/include/sir_optimizer/optimizer.h"
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
            // (AR) مرحلة التحسين الوسيطة — SIR Optimizer
            //      هذه المرحلة تطبّق ممرات التحسين على تمثيل SIR الوسيط قبل
            //      التحويل إلى LLVM IR. الممرات المدعومة حسب مستوى التحسين:
            //
            //      O0: بلا تحسين — تُتخطى بالكامل (للتطوير والتنقيح)
            //      O1: طي الثوابت + إزالة الكود الميت
            //           - طي الثوابت: حساب التعبيرات الثابتة أثناء الترجمة بدل التنفيذ
            //             مثال: 2 + 3 → 5 مباشرة في وقت الترجمة بدل تعليمة ADD
            //           - إزالة الكود الميت: حذف التعليمات التي لا تُستخدم نتائجها
            //      O2: O1 + نشر النسخ + حذف التعبيرات الجزئية المشتركة (CSE)
            //           - نشر النسخ: تعقّب %b = %a واستبدال الاستخدامات مباشرة بـ %a
            //           - CSE: اكتشاف التعبيرات المتكررة وحساب قيمتها مرة واحدة فقط
            //      O3: O2 + دمج السجلات
            //           - دمج السجلات: تقليل عدد السجلات الافتراضية المستخدمة
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

            // (AR) تخطي التحسين في المستوى O0
            // (EN) Skip optimization at O0 level
            if (options_.opt_level == OptimizationLevel::O0)
            {
                if (options_.verbose)
                {
                    std::cout << "  ✓ Optimization skipped (O0) / تمّ تخطي التحسين (O0)\n";
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

            // (AR) تحويل مستوى التحسين من صيغة المشغّل إلى صيغة المحسِّن
            //      Os و Oz تُعامَلان كـ O2 (تحسينات قياسية، والتركيز على الحجم يتم في مستوى LLVM لاحقاً)
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

            // (AR) إنشاء المحسِّن وتشغيله على وحدة SIR
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
                        std::cout << "  ✓ Optimization applied / تمّ تطبيق التحسينات\n";
                    }
                    else
                    {
                        std::cout << "  ✓ No optimizations needed / لا توجد تحسينات لازمة\n";
                    }
                }
            }
            catch (const std::exception &e)
            {
                // (AR) خطأ في التحسين — نتابع بدون تحسين بدلاً من الفشل الكامل
                //      هذا أفضل من إيقاف عملية الترجمة بأكملها
                // (EN) Optimization error — continue without optimization instead of full failure
                //      This is better than stopping the entire compilation process
                if (options_.verbose)
                {
                    std::cerr << "  ? Optimization error (continuing): " << e.what() << "\n";
                    std::cerr << "  ⚠ خطأ في التحسين (متابعة): " << e.what() << "\n";
                }
            }

            return true;
        }

        bool CompilerDriver::run_backend()
        {
            // ============================================================================
            // (AR) تشغيل الواجهة الخلفية - تحويل SIR إلى صيغة الإخراج المطلوبة
            // ============================================================================
            // هذه الدالة هي قلب المترجم: تأخذ وحدة SIR (التمثيل الوسيط)
            // وتحوّلها إلى الشكل النهائي المطلوب:
            //   - LLVM IR  (.ll)  : تمثيل LLVM نصي - قابل للقراءة البشرية
            //   - LLVM BC  (.bc)  : تمثيل LLVM ثنائي - أسرع تحميلاً
            //   - Object   (.obj) : ملف كائني - كود آلة غير مربوط
            //   - Assembly (.asm)  : ملف تجميع - تعليمات المعالج
            //   - Executable(.exe) : ملف تنفيذي - جاهز للتشغيل
            //   - مكتبة مشتركة/ساكنة : (.dll / .so / .lib / .a)
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
            // (AR) اختيار الواجهة الخلفية المناسبة حسب نوع الإخراج المطلوب
            // (EN) Select backend based on requested output type
            // ============================================================================
            switch (options_.output_type)
            {
            case OutputType::BYTECODE:
            {
                // (AR) خلفية Bytecode غير منفَّذة بعد - مخطط لها في مرحلة قادمة
                // (EN) Bytecode backend not implemented yet - planned for future phase
                diagnostics_.report_fatal("خلفية Bytecode غير منفَّذة بعد / Bytecode backend not implemented yet");
                return false;
            }

            // ============================================================================
            // (AR) جميع أنواع الإخراج المبنية على LLVM
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
                // (AR) الخطوة 1: إنشاء مولّد LLVM وتهيئته
                // ================================================================
                // ننشئ كائن المولّد LLVMCodeGen الذي يضم كلاً من:
                //   - سياق LLVM (LLVMContext) : الحاوية الرئيسية
                //   - باني التعليمات (IRBuilder) : لتوليد تعليمات LLVM IR
                //   - آلة الهدف (TargetMachine) : لمعمارية المنصة المستهدفة
                // ================================================================
                // (EN) Step 1: Create and initialize LLVM code generator
                // ================================================================
                llvm_codegen_ = std::make_unique<Sad::LLVM::LLVMCodeGen>();

                // (AR) تمرير وضع الوحدة لتخطي غلاف main
                // (EN) Pass module mode to skip main wrapper
                llvm_codegen_->setModuleMode(options_.module_mode);

                // (AR) تمرير الوضع المستقل لإصدار وقت تشغيل مدمج
                // (EN) Pass freestanding mode to emit built-in runtime
                llvm_codegen_->setFreestanding(options_.freestanding);

                // (AR) استخدام اسم الملف المجرّد فقط كـ module name
                //      لتجنب انهيار LLVM COFF writer مع المحارف العربية في المسار
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

                // (AR) تطبيق وضع LTO من خيارات سطر الأوامر
                // (EN) Apply LTO mode from CLI options
                llvm_codegen_->setLTOMode(options_.enable_lto_full, options_.enable_lto_thin);
                if (options_.verbose && (options_.enable_lto_full || options_.enable_lto_thin))
                {
                    std::cout << "  ? LTO enabled: "
                              << (options_.enable_lto_full ? "Full" : "Thin") << "\n";
                }

                // ================================================================
                // (AR) الخطوة 2: توليد LLVM IR من SIR
                // ================================================================
                // تحويل التمثيل الوسيط (SIR) إلى تمثيل LLVM IR
                // هذه العملية تشمل:
                //   - تحويل كل تعليمة SIR إلى تعليمات LLVM مكافئة
                //   - توليد الدوال والمتغيرات العامة
                //   - التحقق من صحة الوحدة المولّدة
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
                // (AR) بوّابة الوضع الحرّ فقط: إن ضبط التوليد hasErrors_ (مثلاً
                //      SEM019 لمدمجة غير آمنة حرًّا كـاقرأ_ملف) فأحبِط زمن الترجمة
                //      بخروجٍ غير صفريّ بدل إصدار IR مكسور يفشل زمن الربط برسالة
                //      غامضة (رمز غير معرَّف). مقصورة على --freestanding عمدًا كي لا
                //      تكشف علل codegen المستضافة الخمس المؤجَّلة لـISSUE-073 (غير
                //      حرّة). لا نؤلّف هنا نصًّا خامًّا: التشخيص القانونيّ (SEM019)
                //      مصدره الوحيد كتالوج SoT وقد صاغه reportError وطبعه فعلًا؛
                //      نُمرّره إلى محرّك تشخيص السائق ليكون عدّ «N error(s)» صادقًا
                //      (وإلّا طُبع «0 error(s) generated» مضلِّلًا لأنّ أخطاء التوليد
                //      في قناة مستقلّة عن error_count_).
                // (EN) Freestanding-only gate: if codegen set hasErrors_ (e.g. the
                //      SEM019 diagnostic for a freestanding-unsafe builtin) abort
                //      with a non-zero exit instead of emitting broken IR that
                //      fails opaquely at link. Scoped to --freestanding so it does
                //      not surface the 5 hosted codegen bugs deferred to ISSUE-073.
                //      No hand-written string here: the canonical SEM019 text's sole
                //      source is the SoT catalog (built + already printed by
                //      reportError). We forward those messages into the driver's
                //      DiagnosticEngine so the "N error(s)" tally is honest — codegen
                //      errors live in a channel separate from error_count_.
                if (options_.freestanding && llvm_codegen_->hasErrors())
                {
                    for (const auto &codegen_error : llvm_codegen_->getErrors())
                    {
                        diagnostics_.report_error(codegen_error);
                    }
                    return false;
                }
                // (AR) بوّابة مستضافة مُنمَّطة بالرمز (جولة أميليا ٢ لإصلاح اختطاف
                //      «حدث»): فشلُ ربط كائنٍ بصنفه (INT_SIR_FIELD_LAYOUT — «No class
                //      mapping») يعني GEPًا مستحيلًا وثنائيًّا مكسورًا حتمًا، والمفسّر
                //      يرفض نظيره زمنيًّا (RUN025) — فالإحباط الصريح هو التكافؤ الصادق،
                //      وكان exit 0 يُبقيه انحدارًا صامتًا لا يمسكه أيّ اختبار سلوكيّ.
                //      مقصورة على هذا الرمز عمدًا: البوّابة العامّة مؤجَّلة (ISSUE-073)
                //      كي لا تنكشف عللُ codegen الخمس المقنَّعة (yield/مولّدات + مدى).
                // (EN) Hosted code-scoped gate (Amelia round 2 of the «حدث» hijack fix):
                //      failing to map an object to its class (INT_SIR_FIELD_LAYOUT —
                //      "No class mapping") means an impossible GEP and a certainly
                //      broken binary; the interpreter rejects the counterpart at run
                //      time (RUN025), so an explicit abort is the honest parity —
                //      exit 0 kept this class of regression invisible to behavioral
                //      tests. Deliberately scoped to this code: the general gate is
                //      deferred (ISSUE-073) to avoid exposing the 5 masked codegen
                //      bugs (yield/generators + equality-range).
                if (llvm_codegen_->hasErrorCode(
                        ::Sad::Errors::ErrorCode::INT_SIR_FIELD_LAYOUT))
                {
                    for (const auto &codegen_error : llvm_codegen_->getErrors())
                    {
                        diagnostics_.report_error(codegen_error);
                    }
                    return false;
                }
                // (AR) ملاحظة [ISSUE-073]: `generate()` قد يُبلّغ خطأً داخليًّا عبر reportError()
                //      (يضبط hasErrors_) لكنّه يُرجع وحدةً غير فارغة، فيصير الخروج 0 زائفًا
                //      (نجاح صامت لبرنامجٍ مكسور). بوّابةٌ هنا `if (llvm_codegen_->hasErrors())`
                //      تُصلح هذا الصنف — **لكنّها كاشفة**: تُظهر علل codegen مُقنَّعة سابقة (5
                //      اختبارات: yield/مولّدات + equality-range) كانت تُعَدّ ناجحةً زورًا. لذا
                //      أُجِّلت لـPR مستقلّ (ISSUE-073) يُصلح تلك الخمس أيضًا، كي لا يُبعثَر نطاق
                //      إصلاح وصول حقل ADT (ISSUE-077 مُصلَح أصلًا بفرع buildExprMember دون حاجةٍ
                //      لهذه البوّابة — لا يُصدر خطأ codegen بعد الإصلاح).
                // (EN) NOTE [ISSUE-073]: generate() may report an internal error via reportError()
                //      (sets hasErrors_) yet return a non-null module ⇒ false exit 0 (silent
                //      success for a broken program). A gate here `if (llvm_codegen_->hasErrors())`
                //      fixes that class — but it is REVEALING: it surfaces 5 pre-existing masked
                //      codegen bugs (yield/generators + equality-range) previously counted as
                //      passing. Deferred to a dedicated PR (ISSUE-073) that also fixes those five,
                //      so the ADT field-access fix (ISSUE-077, already fixed by the buildExprMember
                //      branch — emits no codegen error post-fix) is not entangled with them.
                // ================================================================
                // (AR) الخطوة 2.5: تشغيل المحسِّن العربي
                // ================================================================
                // تحسينات مخصصة للنصوص العربية ومعالجة Unicode:
                //   - دمج السلاسل النصية المكررة (String pooling)
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
                // (AR) الخطوة 2.6: تشغيل ممرات الكوروتين (لدعم غير_متزامن/انتظر)
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
                    // هذا أبسط أنواع الإخراج: مجرد طباعة الوحدة نصياً
                    // قابل للقراءة البشرية - مفيد للتنقيح والفحص دون أدوات خاصة
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
                    // تمثيل ثنائي لـ LLVM IR - أسرع تحميلاً ومعالجة من النص
                    // يمكن تحويله لاحقاً إلى object file أو executable
                    // أداة llvm-dis تحوّله إلى نص مقروء عند الحاجة
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
                    // (AR) إخراج ملف كائني (.obj / .o)
                    // ==============================================================
                    if (!(*llvm_codegen_).emitObjectFile(options_.output_file, llvm_module.get()))
                    {
                        diagnostics_.report_fatal("فشل إصدار ملف الكائن / Failed to emit object file");
                        return false;
                    }
                }
                else if (options_.output_type == OutputType::ASSEMBLY)
                {
                    // ==============================================================
                    // (AR) إخراج ملف تجميع (.asm / .s)
                    // ==============================================================
                    // كود التجميع هو تمثيل نصي مقروء لتعليمات المعالج
                    // مفيد لفهم ما يولّده المترجم على المستوى الأدنى
                    // مثال: mov rax, 42  /  push rbp  /  call printf
                    // ==============================================================
                    // (EN) Assembly file output (.asm / .s)
                    // Human-readable processor instructions
                    // ==============================================================
                    if (!(*llvm_codegen_).emitAssembly(options_.output_file, llvm_module.get()))
                    {
                        diagnostics_.report_fatal("فشل إصدار ملف التجميع / Failed to emit assembly file");
                        return false;
                    }
                }
                else if (options_.output_type == OutputType::EXECUTABLE)
                {
                    // ==============================================================
                    // (AR) إخراج ملف تنفيذي (.exe على ويندوز)
                    // ==============================================================
                    // هذا هو أكثر أنواع الإخراج اكتمالاً: برنامج جاهز يعمل مباشرة!
                    // العملية تتم على مرحلتين:
                    //   المرحلة 1: تحويل LLVM IR → ملف كائني مؤقت (.obj)
                    //   المرحلة 2: ربط ملف الكائن مع المكتبات → ملف تنفيذي (.exe)
                    //
                    // عملية الربط تشمل:
                    //   - مكتبة وقت التشغيل (runtime) التي توفّر مثلاً:
                    //     * sad_llvm_print_int, sad_llvm_print_string, ...
                    //     * sad_llvm_input, sad_llvm_input_int, ...
                    //   - مكتبة C القياسية (printf, scanf, malloc, ...)
                    //
                    // نستخدم هنا الرابط عبر clang لعدة أسباب مهمة:
                    //   - يعمل على جميع الأنظمة الأساسية
                    //   - متوافق مع MSVC و MinGW تلقائياً
                    //   - أسهل من استدعاء link.exe مباشرة
                    // ==============================================================
                    // (EN) Executable output (.exe on Windows)
                    // Two-phase process:
                    //   Phase 1: LLVM IR → temporary object file (.obj)
                    //   Phase 2: Link object file with runtime → executable (.exe)
                    // ==============================================================

                    // ------- المرحلة 1: توليد ملف كائني مؤقت -------
                    // (EN) Phase 1: Generate temporary object file
                    auto temp_obj = get_temp_file(".obj");
                    temp_files_.push_back(temp_obj);

                    std::string obj_path = temp_obj.string();

                    if (options_.verbose)
                    {
                        std::cerr << "  [الخطوة أ] توليد ملف كائن مؤقّت: " << obj_path << "\n";
                        std::cerr << "  [Step A] Generating temp object file: " << obj_path << "\n";
                    }

                    if (!(*llvm_codegen_).emitObjectFile(obj_path, llvm_module.get()))
                    {
                        diagnostics_.report_fatal("فشل إصدار ملف الكائن المؤقّت / Failed to emit temporary object file");
                        return false;
                    }

                    // ------- المرحلة 2: ربط ملف الكائن لإنتاج الملف التنفيذي -------
                    // (EN) Phase 2: Link object file to produce executable
                    if (options_.verbose)
                    {
                        std::cerr << "  [الخطوة ب] الربط لإنتاج المخرَج: " << options_.output_file << "\n";
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
                    // مكتبة ديناميكية تُحمَّل أثناء وقت التشغيل
                    // تُربط عبر نفس مسار الملف التنفيذي مع خيار -shared
                    // ==============================================================
                    // (EN) Shared library output (.dll / .so)
                    // ==============================================================
                    auto temp_obj = get_temp_file(".obj");
                    temp_files_.push_back(temp_obj);

                    std::string obj_path = temp_obj.string();

                    if (!(*llvm_codegen_).emitObjectFile(obj_path, llvm_module.get()))
                    {
                        diagnostics_.report_fatal("فشل إصدار ملف الكائن المؤقّت / Failed to emit temporary object file");
                        return false;
                    }

                    // (AR) الربط كمكتبة مشتركة
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
                    // (AR) إخراج مكتبة ساكنة (.lib على ويندوز / .a على لينكس)
                    // ==============================================================
                    // أرشيف مكوَّن من ملفات كائنية يُدمج في البرنامج وقت الربط
                    // فلا حاجة إلى ملفات إضافية عند التشغيل
                    // ==============================================================
                    // (EN) Static library output (.lib / .a)
                    // ==============================================================
                    auto temp_obj = get_temp_file(".obj");
                    temp_files_.push_back(temp_obj);

                    std::string obj_path = temp_obj.string();

                    if (!(*llvm_codegen_).emitObjectFile(obj_path, llvm_module.get()))
                    {
                        diagnostics_.report_fatal("فشل إصدار ملف الكائن المؤقّت / Failed to emit temporary object file");
                        return false;
                    }

                    // (AR) إنشاء مكتبة ساكنة باستخدام أداة lib أو ar
                    // (EN) Create static library using lib or ar tool
                    std::string ar_command;
#ifdef _WIN32
                    ar_command = "lib /OUT:" + options_.output_file + " " + obj_path;
#else
                    ar_command = "ar rcs " + options_.output_file + " " + obj_path;
#endif

                    if (options_.verbose)
                    {
                        std::cerr << "  الأمر: " << ar_command << "\n";
                    }

                    if (std::system(ar_command.c_str()) != 0)
                    {
                        diagnostics_.report_fatal("فشل إنشاء المكتبة الساكنة / Failed to create static library");
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
// بسم الله الرحمن الرحيم
