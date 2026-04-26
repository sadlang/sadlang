// ============================================================================
// llvm_compiler_pipeline_emit.cpp — إصدار IR/Assembly/Object/Executable + Freestanding
// (AR) إخراج LLVM IR، تجميع، ربط، وضع Freestanding
// (EN) Emit LLVM IR, assembly, object files, executables, freestanding mode
// تم استخراج هذا الملف من llvm_compiler_pipeline.cpp وفقاً لقاعدة CW-05
// ============================================================================
//
// (AR) إصلاح الترميز: كان الملف يحتوي على نصوص عربية فاسدة (CP862 Hebrew DOS)
//      تم استعادة جميع المعرّفات والتعليقات العربية من ملفات الـ headers المرجعية:
//      - freestanding_codegen.h (نتيجة_توليد، FreestandingError، أسماء الدوال)
//      - no_std_mode.h (سمات::نقطة_دخول، سمات::معالج_ذعر)
//      - no_std_integration.h (NoStdScanResult، NoStdIntegration)
// ============================================================================

#include "llvm_compiler_pipeline.h"
#include "llvm_linker.h"
#include "../../../shared/utils/include/utf8_utils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

// (AR) استخدام فضاء الأسماء sad للرابط
namespace Sad
{
    namespace Compiler
    {
        namespace LLVM
        {
            bool LLVMCompilerPipeline::emitLLVMIR(const std::string &filename)
            {
                try
                {
                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] إصدار LLVM IR إلى / Emitting LLVM IR to: " << filename << "\n";
                    }

                    if (!llvmModule_)
                    {
                        logError("لا توجد وحدة LLVM / No LLVM module available");
                        return false;
                    }

                    return codeGen_->emitToFile(filename);
                }
                catch (const std::exception &e)
                {
                    logError(std::string("خطأ إصدار LLVM IR / LLVM IR emission error: ") + e.what());
                    return false;
                }
            }

            /**
             * (AR) إصدار Assembly
             * (EN) Emit assembly
             */
            bool LLVMCompilerPipeline::emitAssembly(const std::string &filename)
            {
                try
                {
                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] إصدار Assembly إلى / Emitting assembly to: " << filename << "\n";
                    }

                    if (!llvmModule_)
                    {
                        logError("لا توجد وحدة LLVM / No LLVM module available");
                        return false;
                    }

                    // (AR) استخدام الوحدة المحلية llvmModule_ بدلاً من codeGen_->module_
                    // Use local llvmModule_ instead of codeGen_->module_
                    return codeGen_->emitAssembly(filename, llvmModule_.get());
                }
                catch (const std::exception &e)
                {
                    logError(std::string("خطأ إصدار Assembly / Assembly emission error: ") + e.what());
                    return false;
                }
            }

            /**
             * (AR) إصدار Object file
             * (EN) Emit object file
             */
            bool LLVMCompilerPipeline::emitObjectFile(const std::string &filename)
            {
                try
                {
                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] إصدار Object file إلى / Emitting object file to: " << filename << "\n";
                    }

                    if (!llvmModule_)
                    {
                        logError("لا توجد وحدة LLVM / No LLVM module available");
                        return false;
                    }

                    // (AR) استخدام الوحدة المحلية llvmModule_ بدلاً من codeGen_->module_
                    //      لأن generate() نقلت الملكية بـ std::move
                    // Use local llvmModule_ instead of codeGen_->module_
                    // because generate() moved ownership with std::move
                    return codeGen_->emitObjectFile(filename, llvmModule_.get());
                }
                catch (const std::exception &e)
                {
                    logError(std::string("خطأ إصدار Object file / Object file emission error: ") + e.what());
                    return false;
                }
            }

            /**
             * (AR) إصدار Executable — محسّن بنظام كشف سلسلة الأدوات
             * (EN) Emit executable — Enhanced with Toolchain Detection
             */
            bool LLVMCompilerPipeline::emitExecutable(const std::string &filename)
            {
                try
                {
                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] إنشاء Executable: " << filename << "\n";
                    }

                    // أولاً: إنشاء object file / First: Create object file
                    std::string objFile = filename + ".o";
                    if (!emitObjectFile(objFile))
                    {
                        return false;
                    }

                    // ثانياً: استخدام LLVMLinker المحسّن / Second: Use enhanced LLVMLinker
                    LLVMLinker linker;
                    if (!linker.initialize(options_.target_triple))
                    {
                        logWarning("فشل تهيئة الرابط، محاولة الربط اليدوي / Linker initialization failed, trying manual link");
                    }

                    linker.addObjectFile(objFile);
                    linker.setOutputFile(filename);
                    linker.setEntryPoint("main");
                    linker.setVerbose(options_.verbose);

// (AR) إضافة المكتبات الافتراضية حسب النظام / Add default libraries based on system
#ifdef _WIN32
                    linker.addLinkerFlag("/SUBSYSTEM:CONSOLE");
                    linker.addLinkerFlag("/MACHINE:X64");
#else
                    linker.addLinkerFlag("-pie");
#endif

                    bool linkSuccess = linker.link();

                    if (!linkSuccess)
                    {
                        logWarning("فشل الربط بـ LLVMLinker، Object file متاح / Linking with LLVMLinker failed, object file available: " + objFile);

                        // طباعة معلومات المساعدة / Print help information
                        const auto &info = linker.getLinkingInfo();
                        for (const auto &err : info.errors)
                        {
                            logWarning("Linker error: " + err);
                        }

                        // (AR) لا نرجع false لأن الـ object file متاح للاستخدام اليدوي
                        // Don't return false because object file is available for manual linking
                    }
                    else
                    {
                        if (options_.verbose)
                        {
                            linker.printLinkingInfo();
                        }
                    }

                    return true;
                }
                catch (const std::exception &e)
                {
                    logError(std::string("خطأ إنشاء Executable / Executable creation error: ") + e.what());
                    return false;
                }
            }

            /**
             * (AR) الحصول على الوحدة الحالية
             * (EN) Get current module
             */
            llvm::Module *LLVMCompilerPipeline::getCurrentModule()
            {
                return llvmModule_.get();
            }

            // ============================================================================
            // Helper Functions - الدوال المساعدة
            // ============================================================================

            /**
             * (AR) قراءة ملف
             * (EN) Read file
             */
            std::string LLVMCompilerPipeline::readFile(const std::string &filename)
            {
                auto file = sad::utf8::open_ifstream(filename);
                if (!file.is_open())
                {
                    logError("فشل فتح الملف / Failed to open file: " + filename);
                    return "";
                }

                std::stringstream buffer;
                buffer << file.rdbuf();
                return buffer.str();
            }

            /**
             * (AR) تنظيف الموارد
             * (EN) Cleanup resources
             */
            void LLVMCompilerPipeline::cleanup()
            {
                tokens_.clear();
                ast_.clear();
                sirModule_.reset();
                llvmModule_.reset();
                errors_.clear();
                warnings_.clear();
            }

            /**
             * (AR) قياس الوقت
             * (EN) Measure time
             */
            double LLVMCompilerPipeline::measureTime(const std::chrono::high_resolution_clock::time_point &start) const
            {
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                return duration.count() / 1000.0; // (AR) تحويل إلى ملي ثانية / Convert to milliseconds
            }

            /**
             * (AR) تسجيل خطأ
             * (EN) Log error
             */
            void LLVMCompilerPipeline::logError(const std::string &message)
            {
                errors_.push_back(message);
                hasErrors_ = true;

                if (options_.verbose)
                {
                    std::cerr << "[Pipeline Error] " << message << "\n";
                }
            }

            /**
             * (AR) تسجيل تحذير
             * (EN) Log warning
             */
            void LLVMCompilerPipeline::logWarning(const std::string &message)
            {
                warnings_.push_back(message);

                if (options_.verbose)
                {
                    std::cout << "[Pipeline Warning] " << message << "\n";
                }
            }

            // ============================================================================
            // Freestanding Mode Support — دعم وضع Freestanding (بلا مكتبة قياسية)
            // ============================================================================
            //
            // (AR) هذه الدوال تشكّل الطبقة التي تدمج وضع no_std في خط الترجمة.
            //      تُشغّل في نهاية مراحل التحليل لاكتشاف وتطبيق قيود freestanding:
            //
            //  1. scanForFreestandingAttributes()  — قبل التحليل النحوي
            //  2. initializeFreestandingMode()     — بعد الاكتشاف
            //  3. checkFreestandingSymbol()        — خلال بناء SIR (لكل رمز)
            //  4. finalizeFreestandingUnit()       — بعد بناء SIR
            //
            // This layer integrates no_std mode into the compilation pipeline.
            // Runs before/after parsing to detect and enforce freestanding constraints.
            // ============================================================================

            /**
             * (AR) فحص الكود المصدر لسمات #![بلا_مكتبة_قياسية]
             *
             *      يُشغّل مرة واحدة قبل التحليل النحوي.
             *      يُحدّث:
             *        - options_.no_std                  ← هل وُجد #![بلا_مكتبة_قياسية]؟
             *        - options_.no_main                 ← هل وُجد #![بلا_رئيسية]؟
             *        - options_.abort_on_panic          ← هل وُجد #![إيقاف_عند_ذعر]؟
             *        - options_.freestanding_entry_point ← اسم دالة #[نقطة_دخول]
             *        - options_.freestanding_panic_handler ← اسم دالة #[معالج_ذعر]
             *        - options_.freestanding_auto_detected ← true إذا اكتُشف تلقائياً
             *        - freestandingModeActive_          ← true إذا وجب تفعيل الوضع
             *
             * (EN) Scan source code for #![no_std] attributes.
             *      Runs once before parsing. Updates options and freestandingModeActive_.
             */
            bool LLVMCompilerPipeline::scanForFreestandingAttributes(const std::string &sourceCode)
            {
                // ——— فحص المصدر لجمع سمات no_std ———
                noStdScanResult_ = sad::compiler::pipeline::NoStdIntegration::scanSourceForNoStd(sourceCode);

                // ——— إذا وُجدت سمة بلا_مكتبة_قياسية → فعّل الوضع ———
                if (noStdScanResult_.hasNoStd || options_.no_std)
                {
                    // تحديث الخيارات التلقائية
                    if (noStdScanResult_.hasNoStd && !options_.no_std)
                    {
                        options_.no_std = true;
                        options_.freestanding_auto_detected = true;
                    }
                    if (noStdScanResult_.hasNoMain)
                    {
                        options_.no_main = true;
                    }
                    if (noStdScanResult_.hasAbortOnPanic)
                    {
                        options_.abort_on_panic = true;
                    }

                    // استخراج أسماء الدوال المعلّمة من نتيجة الفحص
                    for (const auto &[سمة, دالة] : noStdScanResult_.functionAttributes)
                    {
                        if ((سمة == "entry_point" || سمة == sad::compiler::pipeline::سمات::نقطة_دخول) && options_.freestanding_entry_point.empty())
                        {
                            options_.freestanding_entry_point = دالة;
                        }
                        else if ((سمة == "panic_handler" || سمة == sad::compiler::pipeline::سمات::معالج_ذعر) && options_.freestanding_panic_handler.empty())
                        {
                            options_.freestanding_panic_handler = دالة;
                        }
                    }

                    freestandingModeActive_ = true;

                    if (options_.verbose)
                    {
                        std::cout << "[Freestanding] تم اكتشاف وضع بلا_مكتبة_قياسية / no_std mode detected\n";
                        if (!options_.freestanding_entry_point.empty())
                        {
                            std::cout << "[Freestanding] نقطة الدخول / Entry point: "
                                      << options_.freestanding_entry_point << "\n";
                        }
                        if (!options_.freestanding_panic_handler.empty())
                        {
                            std::cout << "[Freestanding] معالج الذعر / Panic handler: "
                                      << options_.freestanding_panic_handler << "\n";
                        }
                    }
                }

                // الفحص دائماً ناجح — حتى لو لم يُكتشف no_std
                // Scan always succeeds — even if no_std was not detected
                return true;
            }

            /**
             * (AR) تهيئة وضع Freestanding بعد اكتشافه
             *
             *      يُنشئ NoStdConfig من الخيارات المجمّعة،
             *      ثم يُنشئ FreestandingCodeGen ويُعدّه.
             *      يفتح وحدة الترجمة لبدء التتبع.
             *
             *      يجب استدعاؤه بعد scanForFreestandingAttributes()
             *      وقبل أي مرحلة بناء SIR.
             *
             * (EN) Initialize freestanding mode after detection.
             *      Creates NoStdConfig from gathered options, then FreestandingCodeGen.
             */
            bool LLVMCompilerPipeline::initializeFreestandingMode(const std::string &filename)
            {
                if (!freestandingModeActive_)
                {
                    // وضع freestanding غير مفعّل — لا شيء للتهيئة
                    return true;
                }

                // ——— بناء إعداد NoStdConfig من خيارات المترجم ———
                noStdConfig_.noStdEnabled = true;
                noStdConfig_.noMainEnabled = options_.no_main;
                noStdConfig_.abortOnPanic = options_.abort_on_panic;
                noStdConfig_.entryPoint = options_.freestanding_entry_point;
                noStdConfig_.panicHandler = options_.freestanding_panic_handler;
                noStdConfig_.allowAlloc = options_.freestanding_allow_alloc;
                noStdConfig_.allowFloat = options_.freestanding_allow_float;
                noStdConfig_.allowAtomics = options_.freestanding_allow_atomics;
                noStdConfig_.targetTriple = options_.target_triple;
                noStdConfig_.linkerScript = options_.freestanding_linker_script;

                // ——— إنشاء مولّد الكود لوضع freestanding ———
                freestandingCodeGen_ = std::make_unique<
                    sad::compiler::freestanding::FreestandingCodeGen>(noStdConfig_);

                // ——— فتح وحدة الترجمة لبدء التتبع ———
                auto نتيجة = freestandingCodeGen_->ابدأ_وحدة(filename);
                if (!نتيجة.نجاح)
                {
                    logError("[Freestanding] فشل تهيئة الوحدة: " + نتيجة.رسالة_الخطأ_عربي);
                    return false;
                }

                if (options_.verbose)
                {
                    std::cout << "[Freestanding] تمت التهيئة — الوحدة: " << filename << "\n";
                    std::cout << "[Freestanding]   no_main:      " << (noStdConfig_.noMainEnabled ? "نعم" : "لا") << "\n";
                    std::cout << "[Freestanding]   abort_panic:  " << (noStdConfig_.abortOnPanic ? "نعم" : "لا") << "\n";
                    std::cout << "[Freestanding]   allow_alloc:  " << (noStdConfig_.allowAlloc ? "نعم" : "لا") << "\n";
                    std::cout << "[Freestanding]   هدف / target: " << noStdConfig_.targetTriple << "\n";
                }

                return true;
            }

            /**
             * (AR) التحقق النهائي من اكتمال وحدة freestanding
             *
             *      يُشغّل في نهاية مرحلة بناء SIR بعد معالجة جميع التعريفات.
             *      يتحقق من:
             *        — وجود نقطة_دخول   (مطلوب إذا no_main = true)
             *        — وجود معالج_ذعر   (مطلوب دائماً في freestanding)
             *        — طباعة الإحصائيات إذا verbose
             *        — طباعة التحذيرات
             *
             *      يتسامح مع غياب معالج_ذعر (سيستخدم الافتراضي).
             *      يفشل الترجمة إذا كانت no_main=true ولا توجد نقطة_دخول.
             *
             * (EN) Final validation of freestanding unit completeness.
             *      Runs at end of SIR building after all definitions are processed.
             */
            bool LLVMCompilerPipeline::finalizeFreestandingUnit()
            {
                if (!freestandingModeActive_ || !freestandingCodeGen_)
                {
                    return true; // وضع عادي — لا حاجة للتحقق
                }

                // ——— إغلاق الوحدة + التحقق ———
                auto نتيجة = freestandingCodeGen_->أنهِ_وحدة();

                // ——— طباعة التحذيرات ———
                for (const auto &تحذير : freestandingCodeGen_->احصل_على_التحذيرات())
                {
                    logWarning("[Freestanding] " + تحذير);
                }

                // ——— طباعة الأخطاء ———
                for (const auto &خطأ : freestandingCodeGen_->احصل_على_الأخطاء())
                {
                    logError("[Freestanding] " + خطأ.رسالة_الخطأ_عربي);
                }

                // ——— طباعة الإحصائيات والوضع المفصّل ———
                if (options_.verbose)
                {
                    std::cout << freestandingCodeGen_->احصل_على_الإحصائيات();
                }

                // ——— تقرير النتيجة النهائية ———
                if (!نتيجة.نجاح)
                {
                    // نقطة الدخول مفقودة في وضع no_main — خطأ فادح
                    if (نتيجة.رمز_الخطأ ==
                        sad::compiler::freestanding::FreestandingError::نقطة_دخول_مفقودة)
                    {
                        logError(
                            "[Freestanding] نقطة الدخول مفقودة!\n"
                            "  أضف السمة #[نقطة_دخول] قبل دالة البدء:\n"
                            "  #[نقطة_دخول]\n"
                            "  لن_ترجع دالة _start() { ... }\n"
                            "\n"
                            "  أو إذا كانت دالة main موجودة، أضف #![بلا_رئيسية] في أعلى الملف.");
                        return false;
                    }

                    // معالج الذعر مفقود — تحذير (سيستخدم الافتراضي)
                    if (نتيجة.رمز_الخطأ ==
                        sad::compiler::freestanding::FreestandingError::معالج_ذعر_مفقود)
                    {
                        logWarning(
                            "[Freestanding] معالج ذعر مخصص غير موجود، سيُستخدم الافتراضي.\n"
                            "  الافتراضي: حلقة لانهائية + hlt (مناسب للنواة).\n"
                            "  لتوفير معالج مخصص:\n"
                            "  #[معالج_ذعر]\n"
                            "  لن_ترجع دالة عند_الذعر(معلومات: &معلومات_ذعر) { ... }");
                        // ليس خطأ — نستمر
                    }
                }

                if (options_.verbose)
                {
                    std::cout << "[Freestanding] تم اكتمال التحقق النهائي من الوحدة\n";
                }

                return true;
            }

            /**
             * (AR) فحص رمز في وضع freestanding
             *
             *      يُستدعى خلال بناء SIR عند مصادفة دالة أو نوع.
             *      يتحقق من أن الرمز ليس من المكتبة القياسية.
             *
             *      أمثلة على رموز مرفوضة:
             *        "printf"      →  استخدم بار_نص + طباعة_تسلسل()
             *        "std::string" →  استخدم عرض_نص
             *        "malloc"      →  استخدم sad_alloc()
             *        "std::vector" →  استخدم مصفوفة ثابتة
             *
             * @param symbolName اسم الرمز
             * @return true إذا كان الرمز مقبولاً في وضع freestanding
             *
             * (EN) Check symbol in freestanding mode.
             *      Called during SIR building when encountering functions/types.
             */
            bool LLVMCompilerPipeline::checkFreestandingSymbol(const std::string &symbolName)
            {
                if (!freestandingModeActive_ || !freestandingCodeGen_)
                {
                    return true; // وضع عادي — كل الرموز مقبولة
                }

                // ——— فحص اسم الرمز ———
                auto نتيجة = freestandingCodeGen_->تحقق_من_الرمز(symbolName);
                if (!نتيجة.نجاح)
                {
                    logError("[Freestanding] " + نتيجة.رسالة_الخطأ_عربي);
                    return false;
                }

                return true;
            }

        } // namespace LLVM
    } // namespace Compiler
} // namespace Sad
// ============================================================================
// llvm_compiler_pipeline_emit.cpp — إصدار IR/Assembly/Object/Executable + Freestanding
// (AR) إخراج LLVM IR، تجميع، ربط، وضع Freestanding
// (EN) Emit LLVM IR, assembly, object files, executables, freestanding mode
// تم استخراج هذا الملف من llvm_compiler_pipeline.cpp وفقاً لقاعدة CW-05
// ============================================================================

#include "llvm_compiler_pipeline.h"
#include "llvm_linker.h"
#include "../../../shared/utils/include/utf8_utils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

// (AR) استخدام ضاء الأسماء sad للرابط
namespace Sad
{
    namespace Compiler
    {
        namespace LLVM
        {
            bool LLVMCompilerPipeline::emitLLVMIR(const std::string &filename)
            {
                try
                {
                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ״¥״µ״¯״§״± LLVM IR ״¥„‰ / Emitting LLVM IR to: " << filename << "\n";
                    }

                    if (!llvmModule_)
                    {
                        logError("„״§ ״×ˆ״¬״¯ ˆ״­״¯״© LLVM / No LLVM module available");
                        return false;
                    }

                    return codeGen_->emitToFile(filename);
                }
                catch (const std::exception &e)
                {
                    logError(std::string("״®״·״£  ״¥״µ״¯״§״± LLVM IR / LLVM IR emission error: ") + e.what());
                    return false;
                }
            }

            /**
             * ״¥״µ״¯״§״± Assembly / Emit assembly
             */
            bool LLVMCompilerPipeline::emitAssembly(const std::string &filename)
            {
                try
                {
                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ״¥״µ״¯״§״± Assembly ״¥„‰ / Emitting assembly to: " << filename << "\n";
                    }

                    if (!llvmModule_)
                    {
                        logError("„״§ ״×ˆ״¬״¯ ˆ״­״¯״© LLVM / No LLVM module available");
                        return false;
                    }

                    // ״§״³״×״®״¯״§… ״§„ˆ״­״¯״© ״§„…״­„״© llvmModule_ ״¨״¯„״§‹ …† codeGen_->module_
                    // Use local llvmModule_ instead of codeGen_->module_
                    return codeGen_->emitAssembly(filename, llvmModule_.get());
                }
                catch (const std::exception &e)
                {
                    logError(std::string("״®״·״£  ״¥״µ״¯״§״± Assembly / Assembly emission error: ") + e.what());
                    return false;
                }
            }

            /**
             * ״¥״µ״¯״§״± Object file / Emit object file
             */
            bool LLVMCompilerPipeline::emitObjectFile(const std::string &filename)
            {
                try
                {
                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ״¥״µ״¯״§״± Object file ״¥„‰ / Emitting object file to: " << filename << "\n";
                    }

                    if (!llvmModule_)
                    {
                        logError("„״§ ״×ˆ״¬״¯ ˆ״­״¯״© LLVM / No LLVM module available");
                        return false;
                    }

                    // ״§״³״×״®״¯״§… ״§„ˆ״­״¯״© ״§„…״­„״© llvmModule_ ״¨״¯„״§‹ …† codeGen_->module_
                    // „״£† generate() †‚„״× ״§„…„ƒ״© ״¨€ std::move
                    // Use local llvmModule_ instead of codeGen_->module_
                    // because generate() moved ownership with std::move
                    return codeGen_->emitObjectFile(filename, llvmModule_.get());
                }
                catch (const std::exception &e)
                {
                    logError(std::string("״®״·״£  ״¥״µ״¯״§״± Object file / Object file emission error: ") + e.what());
                    return false;
                }
            }

            /**
             * ״¥״µ״¯״§״± Executable / Emit executable
             * Enhanced with Toolchain Detection / …״­״³‘† ״¨†״¸״§… ƒ״´ ״³„״³„״© ״§„״£״¯ˆ״§״×
             */
            bool LLVMCompilerPipeline::emitExecutable(const std::string &filename)
            {
                try
                {
                    if (options_.verbose)
                    {
                        std::cout << "[Pipeline] ״¥†״´״§״¡ Executable: " << filename << "\n";
                    }

                    // ״£ˆ„״§‹: ״¥†״´״§״¡ object file / First: Create object file
                    std::string objFile = filename + ".o";
                    if (!emitObjectFile(objFile))
                    {
                        return false;
                    }

                    // ״«״§†״§‹: ״§״³״×״®״¯״§… LLVMLinker ״§„…״­״³‘† / Second: Use enhanced LLVMLinker
                    LLVMLinker linker;
                    if (!linker.initialize(options_.target_triple))
                    {
                        logWarning("״´„ ״×‡״¦״© ״§„״±״§״¨״·״ …״­״§ˆ„״© ״§„״±״¨״· ״§„״¯ˆ / Linker initialization failed, trying manual link");
                    }

                    linker.addObjectFile(objFile);
                    linker.setOutputFile(filename);
                    linker.setEntryPoint("main");
                    linker.setVerbose(options_.verbose);

// ״¥״¶״§״© ״§„…ƒ״×״¨״§״× ״§„״§״×״±״§״¶״© ״­״³״¨ ״§„†״¸״§… / Add default libraries based on system
#ifdef _WIN32
                    linker.addLinkerFlag("/SUBSYSTEM:CONSOLE");
                    linker.addLinkerFlag("/MACHINE:X64");
#else
                    linker.addLinkerFlag("-pie");
#endif

                    bool linkSuccess = linker.link();

                    if (!linkSuccess)
                    {
                        logWarning("״´„ ״§„״±״¨״· ״¨€ LLVMLinker״ Object file …״×״§״­ / Linking with LLVMLinker failed, object file available: " + objFile);

                        // ״·״¨״§״¹״© …״¹„ˆ…״§״× ״§„…״³״§״¹״¯״© / Print help information
                        const auto &info = linker.getLinkingInfo();
                        for (const auto &err : info.errors)
                        {
                            logWarning("Linker error: " + err);
                        }

                        // „״§ †״±״¬״¹ false „״£† ״§„€ object file …״×״§״­ „„״§״³״×״®״¯״§… ״§„״¯ˆ
                        // Don't return false because object file is available for manual linking
                    }
                    else
                    {
                        if (options_.verbose)
                        {
                            linker.printLinkingInfo();
                        }
                    }

                    return true;
                }
                catch (const std::exception &e)
                {
                    logError(std::string("״®״·״£  ״¥†״´״§״¡ Executable / Executable creation error: ") + e.what());
                    return false;
                }
            }

            /**
             * ״§„״­״µˆ„ ״¹„‰ ״§„ˆ״­״¯״© ״§„״­״§„״© / Get current module
             */
            llvm::Module *LLVMCompilerPipeline::getCurrentModule()
            {
                return llvmModule_.get();
            }

            // ============================================================================
            // Helper Functions - ״§„״¯ˆ״§„ ״§„…״³״§״¹״¯״©
            // ============================================================================

            /**
             * ‚״±״§״¡״© …„ / Read file
             */
            std::string LLVMCompilerPipeline::readFile(const std::string &filename)
            {
                auto file = sad::utf8::open_ifstream(filename);
                if (!file.is_open())
                {
                    logError("״´„ ״×״­ ״§„…„ / Failed to open file: " + filename);
                    return "";
                }

                std::stringstream buffer;
                buffer << file.rdbuf();
                return buffer.str();
            }

            /**
             * ״×†״¸ ״§„…ˆ״§״±״¯ / Cleanup resources
             */
            void LLVMCompilerPipeline::cleanup()
            {
                tokens_.clear();
                ast_.clear();
                sirModule_.reset();
                llvmModule_.reset();
                errors_.clear();
                warnings_.clear();
            }

            /**
             * ‚״§״³ ״§„ˆ‚״× / Measure time
             */
            double LLVMCompilerPipeline::measureTime(const std::chrono::high_resolution_clock::time_point &start) const
            {
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                return duration.count() / 1000.0; // ״×״­ˆ„ ״¥„‰ …„ ״«״§†״© / Convert to milliseconds
            }

            /**
             * ״×״³״¬„ ״®״·״£ / Log error
             */
            void LLVMCompilerPipeline::logError(const std::string &message)
            {
                errors_.push_back(message);
                hasErrors_ = true;

                if (options_.verbose)
                {
                    std::cerr << "[Pipeline Error] " << message << "\n";
                }
            }

            /**
             * ״×״³״¬„ ״×״­״°״± / Log warning
             */
            void LLVMCompilerPipeline::logWarning(const std::string &message)
            {
                warnings_.push_back(message);

                if (options_.verbose)
                {
                    std::cout << "[Pipeline Warning] " << message << "\n";
                }
            }

            // ============================================================================
            // Freestanding Mode Support ג€” ״¯״¹… ˆ״¶״¹ Freestanding (״¨„״§ …ƒ״×״¨״© ‚״§״³״©)
            // ============================================================================
            //
            // ‡״°‡ ״§„״¯ˆ״§„ ״×״´ƒ‘„ ״§„״·״¨‚״© ״§„״× ״×״¯…״¬ ˆ״¶״¹ no_std  ״®״· ״§„״×״±״¬…״©.
            // ״×״´״÷‘„ ‚״¨„ ˆ״¨״¹״¯ …״±״§״­„ ״§„״×״­„„ „״§ƒ״×״´״§ ˆ״×״·״¨‚ ‚ˆ״¯ freestanding:
            //
            //  1. scanForFreestandingAttributes()  ג€” ‚״¨„ ״§„״×״­„„ ״§„†״­ˆ
            //  2. initializeFreestandingMode()     ג€” ״¨״¹״¯ ״§„״§ƒ״×״´״§
            //  3. checkFreestandingSymbol()        ג€” ״®„״§„ ״¨†״§״¡ SIR („ƒ„ ״±…״²)
            //  4. finalizeFreestandingUnit()       ג€” ״¨״¹״¯ ״¨†״§״¡ SIR
            //
            // This layer integrates no_std mode into the compilation pipeline.
            // Runs before/after parsing to detect and enforce freestanding constraints.
            // ============================================================================

            /**
             * (AR) ״­״µ ״§„ƒˆ״¯ ״§„…״µ״¯״± „״³…״§״× #![״¨„״§_…ƒ״×״¨״©_‚״§״³״©]
             *
             * ״´״÷‘„ …״±״© ˆ״§״­״¯״© ‚״¨„ ״§„״×״­„„ ״§„†״­ˆ.
             * ״­״¯‘״«:
             *   - options_.no_std                  ג† ‡„ ˆ״¬״¯ #![״¨„״§_…ƒ״×״¨״©_‚״§״³״©]״
             *   - options_.no_main                 ג† ‡„ ˆ״¬״¯ #![״¨„״§_״±״¦״³״©]״
             *   - options_.abort_on_panic          ג† ‡„ ˆ״¬״¯ #![״¥‚״§_״¹†״¯_״°״¹״±]״
             *   - options_.freestanding_entry_point ג† ״§״³… ״¯״§„״© #[†‚״·״©_״¯״®ˆ„]
             *   - options_.freestanding_panic_handler ג† ״§״³… ״¯״§„״© #[…״¹״§„״¬_״°״¹״±]
             *   - options_.freestanding_auto_detected ג† true ״¥״°״§ ״§ƒ״×״´ ״×„‚״§״¦״§‹
             *   - freestandingModeActive_          ג† true ״¥״°״§ ˆ״¬״¨ ״×״¹„ ״§„ˆ״¶״¹
             *
             * (EN) Scan source code for #![no_std] attributes.
             * Runs once before parsing. Updates options and freestandingModeActive_.
             */
            bool LLVMCompilerPipeline::scanForFreestandingAttributes(const std::string &sourceCode)
            {
                // ג”€ג”€ג”€ ״­״µ ״§„…״µ״¯״± „״¬…״¹ ״³…״§״× no_std ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
                noStdScanResult_ = sad::compiler::pipeline::NoStdIntegration::scanSourceForNoStd(sourceCode);

                // ג”€ג”€ג”€ ״¥״°״§ ˆ״¬״¯״× ״³…״© ״¨„״§_…ƒ״×״¨״©_‚״§״³״© ג†’ ״¹‘„ ״§„ˆ״¶״¹ ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
                if (noStdScanResult_.hasNoStd || options_.no_std)
                {
                    // ״×״­״¯״« ״§„״®״§״±״§״× ״§„״×„‚״§״¦
                    if (noStdScanResult_.hasNoStd && !options_.no_std)
                    {
                        options_.no_std = true;
                        options_.freestanding_auto_detected = true;
                    }
                    if (noStdScanResult_.hasNoMain)
                    {
                        options_.no_main = true;
                    }
                    if (noStdScanResult_.hasAbortOnPanic)
                    {
                        options_.abort_on_panic = true;
                    }

                    // ״§״³״×״®״±״§״¬ ״£״³…״§״¡ ״§„״¯ˆ״§„ ״§„…״¹„‘…״© …† †״×״¬״© ״§„״­״µ
                    for (const auto &[״³…״©, ״¯״§„״©] : noStdScanResult_.functionAttributes)
                    {
                        if ((״³…״© == "entry_point" || ״³…״© == sad::compiler::pipeline::״³…״§״×::†‚״·״©_״¯״®ˆ„) && options_.freestanding_entry_point.empty())
                        {
                            options_.freestanding_entry_point = ״¯״§„״©;
                        }
                        else if ((״³…״© == "panic_handler" || ״³…״© == sad::compiler::pipeline::״³…״§״×::…״¹״§„״¬_״°״¹״±) && options_.freestanding_panic_handler.empty())
                        {
                            options_.freestanding_panic_handler = ״¯״§„״©;
                        }
                    }

                    freestandingModeActive_ = true;

                    if (options_.verbose)
                    {
                        std::cout << "[Freestanding] ג… ״§ƒ״×״´ ˆ״¶״¹ ״¨„״§_…ƒ״×״¨״©_‚״§״³״© / no_std mode detected\n";
                        if (!options_.freestanding_entry_point.empty())
                        {
                            std::cout << "[Freestanding] †‚״·״© ״§„״¯״®ˆ„ / Entry point: "
                                      << options_.freestanding_entry_point << "\n";
                        }
                        if (!options_.freestanding_panic_handler.empty())
                        {
                            std::cout << "[Freestanding] …״¹״§„״¬ ״§„״°״¹״± / Panic handler: "
                                      << options_.freestanding_panic_handler << "\n";
                        }
                    }
                }

                // ״§„״­״µ ״¯״§״¦…״§‹ †״§״¬״­ ג€” ״­״×‰ „ˆ „… ƒ״×״´ no_std
                // Scan always succeeds ג€” even if no_std was not detected
                return true;
            }

            /**
             * (AR) ״×‡״¦״© ˆ״¶״¹ Freestanding ״¨״¹״¯ ״§ƒ״×״´״§‡
             *
             * †״´״¦ NoStdConfig …† ״§„״®״§״±״§״× ״§„…״¬…‘״¹״©״
             * ״«… †״´״¦ FreestandingCodeGen ˆ״¹״¯‘‡.
             * ״×״­ ˆ״­״¯״© ״§„״×״±״¬…״© „״¨״¯״¡ ״§„״×״×״¨״¹.
             *
             * ״¬״¨ ״§״³״×״¯״¹״§״₪‡ ״¨״¹״¯ scanForFreestandingAttributes()
             * ˆ‚״¨„ ״£ …״±״­„״© ״¨†״§״¡ SIR.
             *
             * (EN) Initialize freestanding mode after detection.
             * Creates NoStdConfig from gathered options, then FreestandingCodeGen.
             */
            bool LLVMCompilerPipeline::initializeFreestandingMode(const std::string &filename)
            {
                if (!freestandingModeActive_)
                {
                    // ˆ״¶״¹ freestanding ״÷״± …״¹‘„ ג€” „״§ ״´״¡ „„״×‡״¦״©
                    return true;
                }

                // ג”€ג”€ג”€ ״¨†״§״¡ ״¥״¹״¯״§״¯ NoStdConfig …† ״®״§״±״§״× ״§„…״×״±״¬… ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
                noStdConfig_.noStdEnabled = true;
                noStdConfig_.noMainEnabled = options_.no_main;
                noStdConfig_.abortOnPanic = options_.abort_on_panic;
                noStdConfig_.entryPoint = options_.freestanding_entry_point;
                noStdConfig_.panicHandler = options_.freestanding_panic_handler;
                noStdConfig_.allowAlloc = options_.freestanding_allow_alloc;
                noStdConfig_.allowFloat = options_.freestanding_allow_float;
                noStdConfig_.allowAtomics = options_.freestanding_allow_atomics;
                noStdConfig_.targetTriple = options_.target_triple;
                noStdConfig_.linkerScript = options_.freestanding_linker_script;

                // ג”€ג”€ג”€ ״¥†״´״§״¡ …ˆ„‘״¯ ״§„ƒˆ״¯ „ˆ״¶״¹ freestanding ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
                freestandingCodeGen_ = std::make_unique<
                    sad::compiler::freestanding::FreestandingCodeGen>(noStdConfig_);

                // ג”€ג”€ג”€ ״×״­ ˆ״­״¯״© ״§„״×״±״¬…״© „״¨״¯״¡ ״§„״×״×״¨״¹ ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
                auto †״×״¬״© = freestandingCodeGen_->״§״¨״¯״£_ˆ״­״¯״©(filename);
                if (!†״×״¬״©.†״¬״§״­)
                {
                    logError("[Freestanding] ״´„ ״×‡״¦״© ״§„ˆ״­״¯״©: " + †״×״¬״©.״±״³״§„״©_״§„״®״·״£_״¹״±״¨);
                    return false;
                }

                if (options_.verbose)
                {
                    std::cout << "[Freestanding] ג… ״×…״× ״§„״×‡״¦״© ג€” ״§„ˆ״­״¯״©: " << filename << "\n";
                    std::cout << "[Freestanding]   no_main:      " << (noStdConfig_.noMainEnabled ? "†״¹…" : "„״§") << "\n";
                    std::cout << "[Freestanding]   abort_panic:  " << (noStdConfig_.abortOnPanic ? "†״¹…" : "„״§") << "\n";
                    std::cout << "[Freestanding]   allow_alloc:  " << (noStdConfig_.allowAlloc ? "†״¹…" : "„״§") << "\n";
                    std::cout << "[Freestanding]   ‡״¯ / target: " << noStdConfig_.targetTriple << "\n";
                }

                return true;
            }

            /**
             * (AR) ״§„״×״­‚‚ ״§„†‡״§״¦ …† ״§ƒ״×…״§„ ˆ״­״¯״© freestanding
             *
             * ״´״÷‘„  †‡״§״© …״±״­„״© ״¨†״§״¡ SIR ״¨״¹״¯ …״¹״§„״¬״© ״¬…״¹ ״§„״×״¹״±״§״×.
             * ״×״­‚‚ …†:
             *   ג“ ˆ״¬ˆ״¯ †‚״·״©_״¯״®ˆ„   (…״·„ˆ״¨ ״¥״°״§ no_main = true)
             *   ג“ ˆ״¬ˆ״¯ …״¹״§„״¬_״°״¹״±   (…״·„ˆ״¨ ״¯״§״¦…״§‹  freestanding)
             *   ג“ ״·״¨״§״¹״© ״§„״¥״­״µ״§״¦״§״× ״¥״°״§ verbose
             *   ג“ ״·״¨״§״¹״© ״§„״×״­״°״±״§״×
             *
             * ״×״³״§…״­ …״¹ ״÷״§״¨ …״¹״§„״¬_״°״¹״± (״³״×״®״¯… ״§„״§״×״±״§״¶).
             * ״´„ ״§„״×״±״¬…״© ״¥״°״§ ƒ״§†״× no_main=true ˆ„״§ ״×ˆ״¬״¯ †‚״·״©_״¯״®ˆ„.
             *
             * (EN) Final validation of freestanding unit completeness.
             * Runs at end of SIR building after all definitions are processed.
             */
            bool LLVMCompilerPipeline::finalizeFreestandingUnit()
            {
                if (!freestandingModeActive_ || !freestandingCodeGen_)
                {
                    return true; // ˆ״¶״¹ ״¹״§״¯ ג€” „״§ ״­״§״¬״© „„״×״­‚‚
                }

                // ג”€ג”€ג”€ ״¥״÷„״§‚ ״§„ˆ״­״¯״© + ״§„״×״­‚‚ ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
                auto †״×״¬״© = freestandingCodeGen_->״£†‡_ˆ״­״¯״©();

                // ג”€ג”€ג”€ ״·״¨״§״¹״© ״§„״×״­״°״±״§״× ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
                for (const auto &״×״­״°״± : freestandingCodeGen_->״§״­״µ„_״¹„‰_״§„״×״­״°״±״§״×())
                {
                    logWarning("[Freestanding] " + ״×״­״°״±);
                }

                // ג”€ג”€ג”€ ״·״¨״§״¹״© ״§„״£״®״·״§״¡ ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
                for (const auto &״®״·״£ : freestandingCodeGen_->״§״­״µ„_״¹„‰_״§„״£״®״·״§״¡())
                {
                    logError("[Freestanding] " + ״®״·״£.״±״³״§„״©_״§„״®״·״£_״¹״±״¨);
                }

                // ג”€ג”€ג”€ ״·״¨״§״¹״© ״§„״¥״­״µ״§״¦״§״×  ״§„ˆ״¶״¹ ״§„…״µ‘„ ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
                if (options_.verbose)
                {
                    std::cout << freestandingCodeGen_->״§״­״µ„_״¹„‰_״§„״¥״­״µ״§״¦״§״×();
                }

                // ג”€ג”€ג”€ ״×‚״±״± ״§„†״×״¬״© ״§„†‡״§״¦״© ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
                if (!†״×״¬״©.†״¬״§״­)
                {
                    // †‚״·״© ״§„״¯״®ˆ„ …‚ˆ״¯״©  ˆ״¶״¹ no_main ג€” ״®״·״£ ״§״¯״­
                    if (†״×״¬״©.״±…״²_״§„״®״·״£ ==
                        sad::compiler::freestanding::FreestandingError::†‚״·״©_״¯״®ˆ„_…‚ˆ״¯״©)
                    {
                        logError(
                            "[Freestanding] ג †‚״·״© ״§„״¯״®ˆ„ …‚ˆ״¯״©!\n"
                            "  ״£״¶ ״§„״³…״© #[†‚״·״©_״¯״®ˆ„] ‚״¨„ ״¯״§„״© ״§„״¨״¯״¡:\n"
                            "  #[†‚״·״©_״¯״®ˆ„]\n"
                            "  „†_״×״±״¬״¹ ״¯״§„״© _start() { ... }\n"
                            "\n"
                            "  ״£ˆ ״¥״°״§ ƒ״§†״× ״¯״§„״© main …ˆ״¬ˆ״¯״©״ ״£״¶ #![״¨„״§_״±״¦״³״©]  ״£״¹„‰ ״§„…„.");
                        return false;
                    }

                    // …״¹״§„״¬ ״§„״°״¹״± …‚ˆ״¯ ג€” ״×״­״°״± (״³״³״×״®״¯… ״§„״§״×״±״§״¶)
                    if (†״×״¬״©.״±…״²_״§„״®״·״£ ==
                        sad::compiler::freestanding::FreestandingError::…״¹״§„״¬_״°״¹״±_…‚ˆ״¯)
                    {
                        logWarning(
                            "[Freestanding] ג  …״¹״§„״¬ ״°״¹״± …״®״µ״µ ״÷״± …ˆ״¬ˆ״¯״ ״³״³״×״®״¯… ״§„״§״×״±״§״¶.\n"
                            "  ״§„״§״×״±״§״¶: ״­„‚״© „״§†‡״§״¦״© + hlt (…†״§״³״¨ „„†ˆ״§״©).\n"
                            "  „״×ˆ״± …״¹״§„״¬ …״®״µ״µ:\n"
                            "  #[…״¹״§„״¬_״°״¹״±]\n"
                            "  „†_״×״±״¬״¹ ״¯״§„״© ״¹†״¯_״§„״°״¹״±(…״¹„ˆ…״§״×: &…״¹„ˆ…״§״×_״°״¹״±) { ... }");
                        // „״³ ״®״·״£ ג€” †״³״×…״±
                    }
                }

                if (options_.verbose)
                {
                    std::cout << "[Freestanding] ג… ״§ƒ״×…„ ״§„״×״­‚‚ ״§„†‡״§״¦ …† ״§„ˆ״­״¯״©\n";
                }

                return true;
            }

            /**
             * (AR) ״­״µ ״±…״²  ˆ״¶״¹ freestanding
             *
             * ״³״×״¯״¹‰ ״®„״§„ ״¨†״§״¡ SIR ״¹†״¯ …״µ״§״¯״© ״¯״§„״© ״£ˆ †ˆ״¹.
             * ״×״­‚‚ …† ״£† ״§„״±…״² „״³ …† ״§„…ƒ״×״¨״© ״§„‚״§״³״©.
             *
             * ״£…״«„״© ״¹„‰ ״±…ˆ״² …״±ˆ״¶״©:
             *   "printf"     ג†’ ״§״³״×״®״¯… ״¨״§״±_†״µ + ״·״¨״§״¹״©_״×״³„״³„()
             *   "std::string" ג†’ ״§״³״×״®״¯… ״¹״±״¶_†״µ
             *   "malloc"     ג†’ ״§״³״×״®״¯… sad_alloc()
             *   "std::vector" ג†’ ״§״³״×״®״¯… …״µˆ״© ״«״§״¨״×״©
             *
             * @param symbolName ״§״³… ״§„״±…״²
             * @return true ״¥״°״§ ƒ״§† ״§„״±…״² …‚״¨ˆ„״§‹  ˆ״¶״¹ freestanding
             *
             * (EN) Check symbol in freestanding mode.
             * Called during SIR building when encountering functions/types.
             */
            bool LLVMCompilerPipeline::checkFreestandingSymbol(const std::string &symbolName)
            {
                if (!freestandingModeActive_ || !freestandingCodeGen_)
                {
                    return true; // ˆ״¶״¹ ״¹״§״¯ ג€” ƒ„ ״§„״±…ˆ״² …‚״¨ˆ„״©
                }

                // ג”€ג”€ג”€ ״­״µ ״§״³… ״§„״±…״² ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
                auto †״×״¬״© = freestandingCodeGen_->״×״­‚‚_…†_״§„״±…״²(symbolName);
                if (!†״×״¬״©.†״¬״§״­)
                {
                    logError("[Freestanding] " + †״×״¬״©.״±״³״§„״©_״§„״®״·״£_״¹״±״¨);
                    return false;
                }

                return true;
            }

        } // namespace LLVM
    } // namespace Compiler
} // namespace Sad
