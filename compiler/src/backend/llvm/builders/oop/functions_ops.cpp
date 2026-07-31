/*
 * ============================================================================
 * مولد كود LLVM IR - ملف التنفيذ
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 *
 * هذا الملف يحتوي على تنفيذ مولد كود LLVM IR الذي يترجم SIR إلى LLVM IR
 * This file contains the implementation of LLVM IR code generator that
 * translates SIR to LLVM IR
 *
 * التزام ب STRICT_CODING_RULES.md:
 * - تم قراءة جميع الملفات الرأسية بالكامل (1746 سطر)
 * - تم توثيق كل استخدام API بموقع المصدر
 * - لا توجد دوال أو متغيرات مخترعة
 *
 * STRICT_CODING_RULES.md compliance:
 * - All header files read completely (1746 lines)
 * - Every API usage documented with source location
 * - No invented functions or variables
 *
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2024
 * الإصدار (Version): 1.1.4 Phase 1
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "builders/oop/functions_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include <llvm/Support/TargetSelect.h>
#include <set> // For pre-scan label discovery
// Source: LLVM 14+ API - llvm/MC/TargetRegistry.h بدلاً من llvm/Support/TargetRegistry.h
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/InlineAsm.h>
#include <iostream>
#include <fstream>

// Source: llvm_codegen.h:103-108 - using declarations
using namespace Sad::Compiler::SIR; // For SIRModule, SIRFunction, SIRBasicBlock, SIRInstruction, SadTypeKind

namespace Sad
{
    namespace LLVM
    {

        void FunctionsCodeGen::emitConstants(std::shared_ptr<SIRModule> sirModule)
        {
            if (!sirModule)
            {
                return;
            }

            // (AR) إصدار الثوابت النصية كمتغيرات عامة
            // (EN) Emit string constants as global variables
            const auto &stringConstants = sirModule->getConstants();

            for (size_t i = 0; i < stringConstants.size(); i++)
            {
                const std::string &str = stringConstants[i];
                std::string constName = "str.const." + std::to_string(i);

                // (AR) تحقق أن الثابت لم يُنشأ من قبل
                // (EN) Check constant hasn't been created already
                if (cg_.module_->getGlobalVariable(constName))
                {
                    continue;
                }

                // (AR) إنشاء مصفوفة ثابتة للنص
                // (EN) Create constant array for the string
                llvm::Constant *strConstant = llvm::ConstantDataArray::getString(*cg_.context_, str, true);

                // (AR) إنشاء متغير عام ثابت
                // (EN) Create constant global variable
                auto *gv = new llvm::GlobalVariable(
                    *cg_.module_,
                    strConstant->getType(),
                    true, // isConstant
                    llvm::GlobalValue::PrivateLinkage,
                    strConstant,
                    constName);
                gv->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
                gv->setAlignment(llvm::Align(1));
            }
        }

        /**
         * إصدار الدوال العامة
         * Emit global functions
         *
         * Source: llvm_codegen.h:323
         * @param sirModule وحدة SIR / SIR module
         */
        void FunctionsCodeGen::emitGlobalFunctions(std::shared_ptr<SIRModule> sirModule)
        {
            if (!sirModule)
            {
                return;
            }

            // Source: SIRModule::getFunctions() returns const std::vector<std::shared_ptr<SIRFunction>>&
            const auto &functions = sirModule->getFunctions();

            // المرحلة 1: إنشاء توقيعات جميع الدوال أولاً
            // Phase 1: Create all function signatures first
            for (const auto &sirFunc : functions)
            {
                if (!sirFunc)
                    continue;

                llvm::Function *llvmFunc = emitFunctionPrototype(sirFunc);

                if (llvmFunc)
                {
                    // حفظ في السياق
                    // Save to context
                    // Source: cg_.context_info_ is defined at llvm_codegen.h:643
                    // Source: CodeGenContext::functions is at llvm_codegen.h:618
                    // Source: SIRFunction::getName() is at sir_module.h:306
                    cg_.context_info_.functions[sirFunc->getName()] = llvmFunc;
                }
            }

            // المرحلة 2: إصدار أجسام الدوال
            // Phase 2: Emit function bodies
            for (const auto &sirFunc : functions)
            {
                if (!sirFunc)
                    continue;

                // Source: SIRFunction::getName() is at sir_module.h:306
                auto it = cg_.context_info_.functions.find(sirFunc->getName());
                if (it != cg_.context_info_.functions.end())
                {
                    emitFunctionBody(sirFunc, it->second);
                }
            }

            // إضافة دالة main كـ wrapper للدالة الرئيسية العربية
            // Add main function as wrapper for Arabic main function
            emitMainWrapper(sirModule);
        }

        // ====================================================================
        // (AR) تهيئةُ محليّةِ UTF-8 في مقدّمة نقطة الدخول
        //
        // العلّة المقيسة: الشيفرةُ المولَّدة تنادي دوالَّ CRT الضيّقة مباشرةً
        // (fopen · remove · rename · _mkdir · stat …)، وهذه تفسّر `char*` بترميز
        // **صفحةِ نظامٍ محلّيّة** لا UTF-8. فاسمُ ملفٍّ عربيّ — وهو الحالُ الغالبُ في
        // لغةٍ عربيّة — يصل إلى النواة مشوَّهًا فيفشل الفتحُ ويعود NULL. النتيجة:
        // `اقرأ_ملف("موجود.txt")` يعمل في المفسّر ويموت في المصرَّف، أي تباعدٌ بين
        // المحرّكين في أبسطِ عمليّةٍ في اللغة.
        //
        // لماذا هنا لا في كلّ مدمجةٍ على حدة: العيبُ ليس في `اكتب_ملف` بعينها بل في
        // **تأويلِ زمنِ التشغيل للمسارات كلِّها**. فتغليفُ تسعَ عشرةَ مدمجةً واحدةً
        // واحدةً يعالج ما اصطُدم به ويترك البقيّة، ويعيد إنتاجَ العيب مع كلّ مدمجةٍ
        // جديدة. نداءٌ واحدٌ في المقدّمة يُصلح العائلةَ كلَّها — الحاضرَ منها والآتي.
        //
        // لماذا `.UTF8` تحديدًا: زمنُ التشغيل UCRT (مُقاسٌ في وارداتِ الثنائيّ:
        // api-ms-win-crt-*) يدعم صفحةَ UTF-8 للدوالّ الضيّقة، فتصير `fopen` تقبل
        // UTF-8 بلا تحويلٍ يدويّ إلى UTF-16 ولا نسخةٍ ثانية من كلّ نداء. وعلى غير
        // وندوز هي المحليّةُ الصحيحة كذلك. وفشلُ النداء غيرُ قاتل: يعيد NULL
        // فيبقى السلوكُ كما كان، فلا تُدخِل التهيئةُ انحدارًا حيث لا تنفع.
        // (EN) Emit `setlocale(LC_ALL, ".UTF8")` in the entry prologue. Generated code
        // calls narrow CRT APIs directly; without a UTF-8 locale they interpret char*
        // in the local code page, so any Arabic path fails to open — working in the
        // interpreter and dying in the compiled binary. Fixing it once in the prologue
        // covers the whole narrow-CRT family instead of wrapping each builtin.
        // ====================================================================
        void FunctionsCodeGen::emitUtf8LocaleInit()
        {
            if (!cg_.module_ || !cg_.builder_)
                return;

            auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto *i32Ty = llvm::Type::getInt32Ty(*cg_.context_);

            // (AR) char *setlocale(int category, const char *locale)
            auto *setlocaleTy = llvm::FunctionType::get(ptrTy, {i32Ty, ptrTy}, false);
            auto setlocaleFn = cg_.module_->getOrInsertFunction("setlocale", setlocaleTy);

            // (AR) LC_ALL == 0 في UCRT وفي glibc معًا.
            // (EN) LC_ALL == 0 in both UCRT and glibc.
            llvm::Value *category = llvm::ConstantInt::get(i32Ty, 0);
            llvm::Value *locale = cg_.builder_->CreateGlobalStringPtr(".UTF8", "sad_locale_utf8");
            cg_.builder_->CreateCall(setlocaleFn, {category, locale});
        }

        /**
         * إضافة دالة main كـ wrapper
         * Emit main wrapper function
         */
        void FunctionsCodeGen::emitMainWrapper(std::shared_ptr<SIRModule> sirModule)
        {
            if (!sirModule || !cg_.module_)
                return;

            // (AR) في وضع الوحدة لا نحتاج لدالة main wrapper
            // (EN) In module mode, skip main wrapper entirely
            if (cg_.moduleMode_)
                return;

            // (AR) البحث عن الدالة الرئيسية
            // (EN) Find the main entry point function
            // Priority: 1) "رئيسية"  2) "main"  3) "__sad_main"
            llvm::Function *mainFunc = nullptr;
            std::string mainName;

            // (AR) أولا: البحث عن دالة "رئيسية" (UTF-8 hex escape)
            // (EN) First: look for Arabic "رئيسية" function
            std::string arabicMain = "\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9";
            auto it = cg_.context_info_.functions.find(arabicMain);
            if (it != cg_.context_info_.functions.end())
            {
                mainFunc = it->second;
                mainName = arabicMain;
            }

            // (AR) ثانيا: البحث عن "main"
            // (EN) Second: look for "main"
            if (!mainFunc)
            {
                it = cg_.context_info_.functions.find("main");
                if (it != cg_.context_info_.functions.end())
                {
                    mainFunc = it->second;
                    mainName = "main";
                }
            }

            // (AR) ثالثا: البحث عن "__sad_main" (الكود التنفيذي في المستوى الأعلى)
            // (EN) Third: look for "__sad_main" (top-level executable code wrapper)
            if (!mainFunc)
            {
                it = cg_.context_info_.functions.find("__sad_main");
                if (it != cg_.context_info_.functions.end())
                {
                    mainFunc = it->second;
                    mainName = "__sad_main";
                }
            }

            // (AR) إذا لم نجد أي دالة رئيسية لا حاجة لـ wrapper
            // (EN) If no main function found, no wrapper needed
            if (!mainFunc)
                return;

            // (AR) إذا كان اسم الدالة بالفعل "main" لا نحتاج wrapper — لكن تهيئةَ
            //      المحليّة تلزم على كلّ حال، فتُحقن في مقدّمة الدالّة نفسها.
            // (EN) Already named "main" ⇒ no wrapper — but the locale init is still
            //      required, so inject it into that function's own prologue.
            if (mainName == "main")
            {
                if (!mainFunc->empty())
                {
                    llvm::BasicBlock &bb = mainFunc->getEntryBlock();
                    cg_.builder_->SetInsertPoint(&bb, bb.getFirstInsertionPt());
                    emitUtf8LocaleInit();
                }
                return;
            }

            // (AR) إنشاء دالة main wrapper: int main() { call mainFunc(); return 0; }
            // (EN) Create main wrapper function
            llvm::FunctionType *mainType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(*cg_.context_), // int return type
                {},                                // no parameters
                false                              // not vararg
            );

            llvm::Function *wrapper = llvm::Function::Create(
                mainType,
                llvm::Function::ExternalLinkage,
                "main",
                cg_.module_.get());

            // (AR) إنشاء basic block
            // (EN) Create entry basic block
            llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(*cg_.context_, "entry", wrapper);
            cg_.builder_->SetInsertPoint(entryBB);

            // (AR) تهيئةُ المحليّة قبل أيّ عمل — انظر التعليل في emitUtf8LocaleInit.
            // (EN) Locale init before any work — see the rationale in emitUtf8LocaleInit.
            emitUtf8LocaleInit();

            // (AR) استدعاء الدالة الرئيسية
            // (EN) Call the main function
            llvm::Value *result = cg_.builder_->CreateCall(mainFunc);

            // (AR) تحويل النتيجة إلى i32 إذا لزم الأمر
            // (EN) Convert result to i32 if needed
            if (result->getType()->isIntegerTy(64))
            {
                result = cg_.builder_->CreateTrunc(result, llvm::Type::getInt32Ty(*cg_.context_));
            }
            else if (result->getType()->isVoidTy())
            {
                result = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*cg_.context_), 0);
            }

            // (AR) إرجاع النتيجة
            // (EN) Return the result
            cg_.builder_->CreateRet(result);
        }

        /**
         * التحقق من صحة الوحدة
         * Verify module correctness
         *
         * Source: llvm_codegen.h:257
         * @return true إذا كانت الوحدة صحيحة / true if module is valid
         */
        bool FunctionsCodeGen::verify() const
        {
            // Source: cg_.module_ is defined at llvm_codegen.h:634
            if (!cg_.module_)
            {
                std::cerr << "LLVM CodeGen Error: Module is null in verify()\n";
                return false;
            }

            std::string errorMsg;
            llvm::raw_string_ostream errorStream(errorMsg);

            if (llvm::verifyModule(*cg_.module_, &errorStream))
            {
                std::cerr << "Module verification failed:\n"
                          << errorStream.str() << std::endl;
                return false;
            }

            return true;
        }

        /**
         * تطبيق التحسينات على الوحدة
         * Apply optimizations to module
         *
         * Source: llvm_codegen.h:294
         * @return true إذا نجح التحسين / true if optimization succeeded
         */
        bool FunctionsCodeGen::optimize()
        {
            // Source: cg_.optimizer_ is defined at llvm_codegen.h:657
            if (!cg_.optimizer_)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_BACKEND_EMIT, {{"detail", "Optimizer"}});
                return false;
            }

            // Source: cg_.module_ is defined at llvm_codegen.h:634
            if (!cg_.module_)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_BACKEND_EMIT, {{"detail", "Module"}});
                return false;
            }

            // Source: llvm_optimizer.h:123 - bool optimize(llvm::Module* module)
            return cg_.optimizer_->optimize(cg_.module_.get());
        }

        /**
         * تعيين مستوى التحسين
         * Set optimization level
         *
         * Source: llvm_codegen.h:266
         * @param level مستوى التحسين / Optimization level
         */
        void FunctionsCodeGen::setOptimizationLevel(sad::OptimizationLevel level)
        {
            // Source: cg_.optimizationLevel_ is defined at llvm_codegen.h:660
            cg_.optimizationLevel_ = level;

            // Source: cg_.optimizer_ is defined at llvm_codegen.h:657
            if (cg_.optimizer_)
            {
                cg_.optimizer_->setOptimizationLevel(level);
            }
        }

        /**
         * (AR) ضبط وضع LTO عبر LLVMOptimizer
         * (EN) Set LTO mode through underlying LLVMOptimizer
         */
        void FunctionsCodeGen::setLTOMode(bool enable_full, bool enable_thin)
        {
            if (cg_.optimizer_)
            {
                cg_.optimizer_->setLTOMode(enable_full, enable_thin);
            }

            // (AR) تطبيق علامات LTO على الموديول مباشرة لضمان ظهورها
            //      حتى عند --emit-llvm بمستوى تحسين O0 (حيث optimize() لا يُستدعى)
            // (EN) Apply LTO module flags directly to ensure they appear
            //      even with --emit-llvm at O0 (when optimize() may be skipped)
            if (cg_.module_)
            {
                if (enable_thin)
                {
                    if (!cg_.module_->getModuleFlag("ThinLTO"))
                        cg_.module_->addModuleFlag(llvm::Module::Error, "ThinLTO", 1);
                    if (!cg_.module_->getModuleFlag("EnableSplitLTOUnit"))
                        cg_.module_->addModuleFlag(llvm::Module::Error, "EnableSplitLTOUnit", 1);
                }
                else if (enable_full)
                {
                    if (!cg_.module_->getModuleFlag("EnableSplitLTOUnit"))
                        cg_.module_->addModuleFlag(llvm::Module::Error, "EnableSplitLTOUnit", 1);
                }
            }
        }

        /**
         * الحصول على إحصائيات التحسين
         * Get optimization statistics
         *
         * Source: llvm_codegen.h:305
         * @return إحصائيات التحسين / Optimization statistics
         */
        const sad::OptimizationStats &FunctionsCodeGen::getOptimizationStats() const
        {
            // Source: cg_.optimizer_ is defined at llvm_codegen.h:657
            static sad::OptimizationStats emptyStats;

            if (!cg_.optimizer_)
            {
                return emptyStats;
            }

            return cg_.optimizer_->getStats();
        }

        /**
         * طباعة إحصائيات التحسين
         * Print optimization statistics
         *
         * Source: llvm_codegen.h:313
         */
        void FunctionsCodeGen::printOptimizationStats() const
        {
            // Source: cg_.optimizer_ is defined at llvm_codegen.h:657
            if (cg_.optimizer_)
            {
                cg_.optimizer_->printStats();
            }
        }

        // ============================================================================
        // المرحلة 3: إصدار الدوال
        // Phase 3: Function Emission
        // ============================================================================
        // Note: getModule() is defined inline at llvm_codegen.h:211 - لا حاجة لتنفيذها هنا

        /**
         * إصدار دالة LLVM من دالة SIR
         * Emit LLVM function from SIR function
         *
         * Source: llvm_codegen.h:347
         * @param sirFunc دالة SIR / SIR function
         * @return دالة LLVM / LLVM function
         */

    } // namespace LLVM
} // namespace Sad
