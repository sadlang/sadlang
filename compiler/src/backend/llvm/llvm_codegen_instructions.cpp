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
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include <llvm/Support/TargetSelect.h>
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
#include "bounds_checker.h" // (AR) فحص حدود موحَّد / (EN) unified bounds checking
#include "builtin_registry.h" // (AR) أسماء المدمجات القانونيّة لبوّابة الوضع الحرّ

// Source: llvm_codegen.h:103-108 - using declarations
using namespace Sad::Compiler::SIR; // For SIRModule, SIRFunction, SIRBasicBlock, SIRInstruction, SadTypeKind

namespace Sad
{
    namespace LLVM
    {

        // ====================================================================
        // (AR) بوّابة سلامة الوضع الحرّ — تصنيف المدمجات غير الآمنة:
        //      مدمجات نظام الملفّات ودخل الطرفيّة والتحكّم الطرفيّ/الإخراج تُصدر رموز
        //      libc (fopen/scanf/system...) غائبة في --freestanding، فكانت تُترجَم
        //      بنجاح ثمّ تفشل زمن الربط برسالة غامضة. هذا التصنيف يسمّي الدالّة
        //      القانونيّة (من SoT: Names) ليُصدر التشخيص المبكّر SEM019 اسمًا واضحًا.
        //      ملاحظة: «اطبع» آمنة حرًّا (تُوجَّه لمنافذ) فلا تُبوَّب. سلسلة فارغة = آمنة.
        //      ⚠️ قائمة منع محافِظة (لا سماح): ما ليس هنا يُترك كما هو (لا انحدار).
        // (EN) Freestanding-safety classifier: filesystem/stdin and terminal-control
        //      /output builtins emit libc symbols (fopen/scanf/system...) absent in
        //      --freestanding. Returns the canonical (SoT) name for unsafe opcodes so
        //      SEM019 can name it. Note: «print» is freestanding-safe (routed to ports)
        //      and is not gated. Empty = safe; conservative deny-list.
        // ====================================================================
        static std::string freestandingUnsafeBuiltinName(SIROpcode op)
        {
            namespace Nb = Sad::Builtins::Names::Basics;
            namespace Nc = Sad::Builtins::Names::Core;
            namespace Nio = Sad::Builtins::Names::CompilerIo;
            switch (op)
            {
            case SIROpcode::BUILTIN_READ:            return std::string(Nc::READ);        // اقرأ (دخل قياسيّ)
            // (AR) قراءة_سطر: دخل قياسيّ كـ«اقرأ» تمامًا، يُصدر scanf/getchar/strdup
            //      (libc غائبة حرًّا) ⇒ يُبوَّب كي يعطي SEM019 نظيفًا لا فشل ربط غامضًا.
            // (EN) read-line: stdin like «اقرأ»; emits scanf/getchar/strdup (libc,
            //      absent freestanding) ⇒ gated for a clean SEM019, not an opaque link error.
            case SIROpcode::BUILTIN_READ_LINE:       return std::string(Nio::IO_1);       // قراءة_سطر (دخل قياسيّ)
            // (AR) مسح_الشاشة: تحكّم طرفيّة يُصدر نداء system("cls"/"clear") — رمز
            //      libc غائب حرًّا فيكسر الربط تمامًا كمدمجات الملفّات/الدخل. توسعةً
            //      لميثاق البوّابة نحو الإخراج/الطرفيّة، يُبوَّب كي يعطي SEM019 نظيفًا.
            // (EN) clear-screen: terminal control emitting system("cls"/"clear") — a
            //      libc symbol absent freestanding, breaking the link exactly like the
            //      file/stdin builtins. Extends the gate's charter to output/terminal.
            case SIROpcode::BUILTIN_CLEAR_SCREEN:    return std::string(Nio::IO_2);       // مسح_الشاشة (تحكّم طرفيّة)
            case SIROpcode::BUILTIN_FILE_READ:       return std::string(Nb::READ_FILE);
            case SIROpcode::BUILTIN_FILE_WRITE:      return std::string(Nb::WRITE_FILE);
            case SIROpcode::BUILTIN_FILE_APPEND:     return std::string(Nb::APPEND_FILE);
            case SIROpcode::BUILTIN_FILE_DELETE:     return std::string(Nb::DELETE_FILE);
            case SIROpcode::BUILTIN_FILE_COPY:       return std::string(Nb::COPY_FILE);
            case SIROpcode::BUILTIN_FILE_MOVE:       return std::string(Nb::MOVE_FILE);
            case SIROpcode::BUILTIN_FILE_CREATE_DIR: return std::string(Nb::MKDIR);
            case SIROpcode::BUILTIN_FILE_LIST_DIR:   return std::string(Nb::LIST_DIR);
            default:                                 return std::string();
            }
        }

        void LLVMCodeGen::emitFunctionParameters(std::shared_ptr<SIRFunction> sirFunc, llvm::Function *llvmFunc)
        {
            if (!sirFunc || !llvmFunc)
            {
                return;
            }

            // Source: SIRFunction::getParameters() is at sir_module.h:293
            const auto &params = sirFunc->getParameters();

            unsigned idx = 0;
            for (auto &arg : llvmFunc->args())
            {
                if (Sad::Security::BoundsChecker::checkArrayIndex(idx, params.size()))
                {
                    // Source: SIRParameter::name is PUBLIC member at sir_module.h:229
                    std::string paramName = params[idx].name;

                    // إنشاء alloca للمعامل
                    // Create alloca for parameter
                    llvm::IRBuilder<> tmpBuilder(&llvmFunc->getEntryBlock(),
                                                 llvmFunc->getEntryBlock().begin());
                    llvm::AllocaInst *alloca = tmpBuilder.CreateAlloca(
                        arg.getType(),
                        nullptr,
                        paramName);

                    // تخزين قيمة المعامل
                    // Store parameter value
                    tmpBuilder.CreateStore(&arg, alloca);

                    // حفظ في القيم المسماة
                    // Save to named values
                    // Source: context_info_.namedValues is at llvm_codegen.h:617
                    // (AR) نحفظ بكلا الاسمين: بدون % وبادئة % لأن SIR builder يسجل
                    //      المعاملات كسجلات بادئة % (sir_builder.cpp:271)
                    // (EN) Store under both names: without % and with % prefix because
                    //      the SIR builder registers parameters as %param_name registers
                    //      (sir_builder.cpp:271)
                    context_info_.namedValues[paramName] = alloca;
                    context_info_.namedValues["%" + paramName] = alloca;
                }
                idx++;
            }
        }

        /**
         * إنشاء كتلة أساسية فارغة
         * Create empty basic block
         *
         * Source: llvm_codegen.h:400
         * @param name اسم الكتلة / Block name
         * @param function الدالة المحتوية / Containing function
         * @return كتلة LLVM الأساسية / LLVM basic block
         */
        llvm::BasicBlock *LLVMCodeGen::createBasicBlock(const std::string &name, llvm::Function *function)
        {
            if (!function)
            {
                reportError(::Sad::Errors::ErrorCode::INT_COMPILER_NULL_IR, {{"detail", "Null"}});
                return nullptr;
            }

            // Source: context_ is defined at llvm_codegen.h:631
            return llvm::BasicBlock::Create(*context_, name, function);
        }

        // ============================================================================
        // ============================================================================
        // Dispatcher — يوزع على ثلاث طبقات فرعية (Strangler Fig v3.1)
        // ============================================================================

        /**
         * (AR) تنفيذ تعليمة SIR — dispatcher رئيسي يوزع على ثلاث طبقات:
         *      1. emitInstructionCore      — الجوهر (حساب، async، كائنات، إلخ)
         *      2. emitInstructionLowlevel  — مستوى منخفض (CPU، UEFI، إلخ)
         *      3. emitInstructionPlatform  — منصات (Android، UI، توجيهات)
         * (EN) Emit SIR instruction — main dispatcher that delegates to three tiers:
         *      1. emitInstructionCore      — core (arithmetic, async, objects, etc.)
         *      2. emitInstructionLowlevel  — low-level (CPU, UEFI, etc.)
         *      3. emitInstructionPlatform  — platform (Android, UI, directives)
         */
        llvm::Value *LLVMCodeGen::emitInstruction(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
            {
                reportError(::Sad::Errors::ErrorCode::INT_COMPILER_NULL_IR, {{"detail", "Null"}});
                return nullptr;
            }

            // (AR) بوّابة الوضع الحرّ: أوقِف مدمجة غير آمنة (ملفّات/دخل قياسيّ) زمن
            //      الترجمة بتشخيص واضح يسمّيها (SEM019) بدل تركها تُصدر رمز libc
            //      غائبًا فيفشل الربط لاحقًا برسالة غامضة. توضع هنا (قبل الطبقات) كي
            //      لا يُفسَّر إيقافها «opcode غير مدعوم» (سقوط عبر كلّ الطبقات).
            //      نربط السجلّ الناتج بمؤشّر باطل فلا تتتالى «سجلّ غير معرَّف» على
            //      مستهلكيه؛ والبناء يُحبَط عبر بوّابة hasErrors الحرّة في السائق.
            // (EN) Freestanding gate: reject an unsafe (filesystem/stdin) builtin
            //      at compile time with a named diagnostic (SEM019) instead of
            //      emitting an absent libc symbol. Placed before the tiers so the
            //      early-out isn't mistaken for "unsupported opcode". The result
            //      register is bound to null to avoid cascading "undefined
            //      register" errors; the build aborts via the driver's gate.
            if (freestanding_)
            {
                std::string unsafeName = freestandingUnsafeBuiltinName(inst->opcode);
                if (!unsafeName.empty())
                {
                    reportError(::Sad::Errors::ErrorCode::SEM_FREESTANDING_BUILTIN,
                                {{"name", unsafeName}});
                    if (inst->result.has_value())
                    {
                        auto *ptrTy = llvm::PointerType::getUnqual(*context_);
                        context_info_.namedValues[inst->result->name] =
                            llvm::ConstantPointerNull::get(
                                llvm::cast<llvm::PointerType>(ptrTy));
                    }
                    return nullptr;
                }
            }

            // (AR) الطبقة الأولى: التعليمات الجوهرية
            // (EN) Tier 1: Core instructions
            if (llvm::Value *result = emitInstructionCore(inst))
                return result;

            // (AR) الطبقة الثانية: تعليمات المستوى المنخفض
            // (EN) Tier 2: Low-level instructions
            if (llvm::Value *result = emitInstructionLowlevel(inst))
                return result;

            // (AR) الطبقة الثالثة: تعليمات المنصات والتوجيهات
            // (EN) Tier 3: Platform and directive instructions
            if (llvm::Value *result = emitInstructionPlatform(inst))
                return result;

            // (AR) الطبقة الرابعة: تعليمات الشبكة (TCP/UDP، HTTP، عناوين)
            // (EN) Tier 4: Network instructions (TCP/UDP, HTTP, addresses)
            if (llvm::Value *result = emitNetworkBuiltin(inst))
                return result;

            // (AR) الطبقة الخامسة: تعليمات SIMD متجهات (Phase 3)
            //      تنتج <N x T> أصلية + intrinsics (fma, sqrt, vector.reduce)
            // (EN) Tier 5: SIMD vector instructions (Phase 3)
            //      Emits native <N x T> + intrinsics (fma, sqrt, vector.reduce)
            if (llvm::Value *result = emitInstructionSimd(inst))
                return result;

            // (AR) لا توجد طبقة تعرف هذا الـ opcode
            // (EN) No tier recognized this opcode
            reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", std::string("Unsupported opcode:") + std::to_string(static_cast<int>(inst->opcode))}});
            return nullptr;
        }

    } // namespace LLVM
} // namespace Sad
