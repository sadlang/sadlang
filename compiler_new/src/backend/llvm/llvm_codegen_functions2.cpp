// ============================================================================
// llvm_codegen_functions2.cpp — إصدار الدوال وجسمها
// (AR) إصدار دالة، نموذجها الأولي، وجسمها في LLVM IR
// (EN) Emit function, prototype, and body to LLVM IR
// تم استخراج هذا الملف من llvm_codegen_functions.cpp وفقاً لقاعدة CW-05
// ============================================================================

#include "llvm_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <iostream>
#include <fstream>

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {
        llvm::Function *LLVMCodeGen::emitFunction(std::shared_ptr<SIRFunction> sirFunc)
        {
            if (!sirFunc)
            {
                reportError("SIR function is null in emitFunction");
                return nullptr;
            }

            // إنشاء توقيع الدالة
            // Create function prototype
            llvm::Function *llvmFunc = emitFunctionPrototype(sirFunc);

            if (!llvmFunc)
            {
                return nullptr;
            }

            // إصدار جسم الدالة
            // Emit function body
            emitFunctionBody(sirFunc, llvmFunc);

            return llvmFunc;
        }

        /**
         * إصدار توقيع الدالة فقط (بدون جسم)
         * Emit function signature only (without body)
         *
         * Source: llvm_codegen.h:368
         * @param sirFunc دالة SIR / SIR function
         * @return دالة LLVM / LLVM function
         */
        llvm::Function *LLVMCodeGen::emitFunctionPrototype(std::shared_ptr<SIRFunction> sirFunc)
        {
            if (!sirFunc)
            {
                reportError("SIR function is null in emitFunctionPrototype");
                return nullptr;
            }

            // Source: SIRFunction::getName() is at sir_module.h:306
            std::string funcName = sirFunc->getName();

            // (AR) استخدام اسم الربط الخارجي (FFI) إذا كان محدداً
            // (EN) Use FFI link name for the LLVM symbol if specified
            std::string llvmSymbolName = sirFunc->getLinkName();

            // Source: SIRFunction::returnType is PUBLIC member at sir_module.h:251
            SadTypeKind returnSIRType = sirFunc->returnType;

            // تحويل نوع الإرجاع
            // Convert return type
            llvm::Type *returnType = nullptr;
            switch (returnSIRType)
            {
            case SadTypeKind::Void:
                returnType = getVoidType();
                break;
            case SadTypeKind::Integer:
                returnType = getInt64Type();
                break;
            case SadTypeKind::Float:
                returnType = getDoubleType();
                break;
            case SadTypeKind::Boolean:
                returnType = getInt1Type();
                break;
            case SadTypeKind::Pointer:
                returnType = getInt8PtrType();
                break;
            case SadTypeKind::String:
                returnType = getInt8PtrType();
                break;
            // (AR) الكائنات والبنى تُرجع كمؤشرات — ptr في LLVM opaque pointer mode
            // (EN) Objects and structs returned as pointers — ptr in LLVM opaque pointer mode
            case SadTypeKind::Struct:
                returnType = llvm::PointerType::getUnqual(*context_);
                break;
            case SadTypeKind::Array:
                returnType = llvm::PointerType::getUnqual(*context_);
                break;
            // (AR) الصفوف تُرجع كمؤشرات — نفس بنية SadArray
            // (EN) Tuples returned as pointers — same SadArray structure
            case SadTypeKind::Tuple:
                returnType = llvm::PointerType::getUnqual(*context_);
                break;
            // ================================================================
            // (AR) [إصلاح الإغلاقات المتداخلة] الدوال التي تُرجع إغلاقاً (closure)
            //      نوعها في SIR هو Function. الإغلاقات تُخزَّن كـ i64 (مؤشر معبأ).
            //      بدون هذه الحالة: يقع في default → void → الناتج لا يُخزَّن
            //      مما يسبب خطأ "Undefined register" عند استدعاء الإغلاق المُرجَع.
            // (EN) [Fix nested closures] Functions returning a closure (Function type).
            //      Closures are stored as packed i64 (pointer-as-integer).
            //      Without this case: falls to default → void → result not stored
            //      causing "Undefined register" when using the returned closure.
            // ================================================================
            case SadTypeKind::Function:
                returnType = getInt64Type();
                break;
            default:
                returnType = getVoidType();
                break;
            }

            // (AR) إصلاح أنواع الإرجاع لدوال C المعروفة — خاصة دوال تخصيص الذاكرة.
            //      عند كتابة دالة بلغة ص باسم "malloc" أو "realloc" أو "calloc"،
            //      نوع الإرجاع الافتراضي يكون i64 لكن يجب أن يكون ptr لتوافق C ABI.
            //      emitReturn يتعامل تلقائياً مع تحويل i64 → ptr عبر inttoptr.
            // (EN) Fix return types for well-known C allocation functions.
            //      When a .ص file defines a function named "malloc"/"realloc"/"calloc",
            //      the default return type is i64 but must be ptr for C ABI compatibility.
            //      emitReturn already handles i64 → ptr conversion via inttoptr.
            {
                const std::string &effectiveName = llvmSymbolName;
                if (effectiveName == "malloc" || effectiveName == "calloc" ||
                    effectiveName == "realloc" || effectiveName == "memcpy" ||
                    effectiveName == "memset" || effectiveName == "memmove" ||
                    effectiveName == "strcpy" || effectiveName == "strncpy" ||
                    effectiveName == "strcat" || effectiveName == "strncat")
                {
                    returnType = llvm::PointerType::getUnqual(*context_);
                }
                else if (effectiveName == "free")
                {
                    returnType = llvm::Type::getVoidTy(*context_);
                }
            }

            // تحويل أنواع المعاملات
            // Convert parameter types
            std::vector<llvm::Type *> paramTypes;

            // Source: SIRFunction::getParameters() is at sir_module.h:293
            const auto &params = sirFunc->getParameters();

            for (const auto &param : params)
            {
                // Source: SIRParameter::type is PUBLIC member at sir_module.h:230
                llvm::Type *paramType = nullptr;

                switch (param.type)
                {
                case SadTypeKind::Integer:
                    paramType = getInt64Type();
                    break;
                case SadTypeKind::Float:
                    paramType = getDoubleType();
                    break;
                case SadTypeKind::Boolean:
                    paramType = getInt1Type();
                    break;
                case SadTypeKind::Pointer:
                    paramType = getInt8PtrType();
                    break;
                case SadTypeKind::String:
                    paramType = getInt8PtrType();
                    break;
                default:
                    paramType = getInt64Type();
                    break;
                }

                paramTypes.push_back(paramType);
            }

            // إنشاء نوع الدالة
            // Create function type
            llvm::FunctionType *funcType = llvm::FunctionType::get(
                returnType,
                paramTypes,
                false // isVarArg
            );

            // إنشاء الدالة
            // Create function
            // Source: module_ is defined at llvm_codegen.h:634
            // (AR) في وضع الوحدة: كل الدوال المحلية InternalLinkage لتجنب تعارضات الربط.
            //      الدوال المعلنة بـ خارجي (FFI) تبقى ExternalLinkage لأنها declarations.
            //      في وضع غير الوحدة: __sad_main يحتاج ExternalLinkage كنقطة دخول.
            // (EN) In module mode: all locally-defined functions get InternalLinkage to avoid
            //      duplicate symbol conflicts. FFI-declared functions stay ExternalLinkage.
            //      In non-module mode: __sad_main needs ExternalLinkage as entry point.
            llvm::GlobalValue::LinkageTypes linkage = llvm::Function::ExternalLinkage;
            if (moduleMode_)
            {
                // (AR) تحقق: هل هذه دالة FFI (لديها linkName مختلف عن name)؟
                // (EN) Check: is this an FFI function (has linkName different from name)?
                bool isFFI = !sirFunc->linkName.empty() && sirFunc->linkName != sirFunc->getName();
                // (AR) تحقق: هل لهذه الدالة جسم (تعريف)؟ التصريحات (declare) بدون جسم يجب أن تبقى ExternalLinkage
                // (EN) Check: does this function have a body (definition)? Declarations without body must stay ExternalLinkage
                bool hasBody = !sirFunc->basicBlocks.empty();
                if (!isFFI && hasBody && !sirFunc->isExported)
                {
                    // (AR) دالة محلية غير مُصدّرة في وضع الوحدة → InternalLinkage
                    // (EN) Locally-defined non-exported function in module mode → InternalLinkage
                    linkage = llvm::Function::InternalLinkage;
                }
            }

            // (AR) إصلاح: إذا كانت الدالة موجودة مسبقاً في الوحدة (من freestanding أو تصريح سابق)
            //      أعد استخدامها بدلاً من إنشاء نسخة جديدة بلاحقة ".1"
            // (EN) Fix: If function already exists in the module (from freestanding or prior declaration),
            //      reuse it instead of creating a new one with ".1" suffix
            llvm::Function *llvmFunc = nullptr;
            llvm::Function *existingFunc = module_->getFunction(llvmSymbolName);
            if (existingFunc)
            {
                if (existingFunc->getFunctionType() == funcType)
                {
                    // (AR) نفس النوع — أعد استخدام الدالة الموجودة
                    // (EN) Same type — reuse existing function
                    llvmFunc = existingFunc;
                    // (AR) إذا كانت الدالة الموجودة ضعيفة (freestanding) والجديدة قوية، حدّث الربط
                    // (EN) If existing is weak (freestanding) and new is strong, update linkage
                    if (existingFunc->getLinkage() == llvm::Function::WeakODRLinkage ||
                        existingFunc->isDeclaration())
                    {
                        llvmFunc->setLinkage(linkage);
                        // (AR) إزالة أجسام الدوال الضعيفة حتى يُستبدل بالجسم الجديد
                        // (EN) Remove weak function bodies so they can be replaced by new body
                        if (!existingFunc->isDeclaration() && !sirFunc->basicBlocks.empty())
                        {
                            existingFunc->deleteBody();
                        }
                    }
                }
                else
                {
                    // (AR) نوع مختلف — احذف القديم وأنشئ جديد
                    // (EN) Different type — remove old and create new
                    existingFunc->replaceAllUsesWith(
                        llvm::UndefValue::get(existingFunc->getType()));
                    existingFunc->eraseFromParent();
                    llvmFunc = llvm::Function::Create(
                        funcType, linkage, llvmSymbolName, module_.get());
                }
            }
            else
            {
                llvmFunc = llvm::Function::Create(
                    funcType, linkage, llvmSymbolName, module_.get());
            }

            // تسمية المعاملات
            // Name parameters
            unsigned idx = 0;
            for (auto &arg : llvmFunc->args())
            {
                if (idx < params.size())
                {
                    // Source: SIRParameter::name is PUBLIC member at sir_module.h:229
                    arg.setName(params[idx].name);
                }
                idx++;
            }

            // (AR) إذا كانت الدالة كوروتين، أضف سمة presplitcoroutine
            // (EN) If function is coroutine, add presplitcoroutine attribute
            // MUST use enum attribute, not string attribute, for CoroSplit to recognize
            if (sirFunc->isCoroutine)
            {
                llvmFunc->addFnAttr(llvm::Attribute::PresplitCoroutine);
                std::cerr << "[CORO] Added presplitcoroutine to '" << funcName << "'" << std::endl;
            }

            return llvmFunc;
        }

        /**
         * إصدار جسم الدالة
         * Emit function body
         *
         * Source: llvm_codegen.h:356
         * @param sirFunc دالة SIR / SIR function
         * @param llvmFunc دالة LLVM / LLVM function
         */
        void LLVMCodeGen::emitFunctionBody(std::shared_ptr<SIRFunction> sirFunc, llvm::Function *llvmFunc)
        {
            if (!sirFunc || !llvmFunc)
            {
                reportError("Null function in emitFunctionBody");
                return;
            }

            // حفظ الدالة الحالية في السياق
            // Save current function in context
            // Source: context_info_ is defined at llvm_codegen.h:643
            // Source: CodeGenContext::currentFunction is at llvm_codegen.h:615
            context_info_.currentFunction = llvmFunc;

            // مسح القيم المسماة للدالة الجديدة
            // Clear named values for new function
            // Source: CodeGenContext::namedValues is at llvm_codegen.h:617
            context_info_.namedValues.clear();

            // (AR) تنظيف خرائط الأصناف المحلية — تمنع تلوث بين الدوال
            // (EN) Clear per-function class maps — prevents cross-function pollution
            context_info_.objectClassMap.clear();
            context_info_.objectFieldsAccessed.clear();

            // ================================================================
            // كشف الباني: إذا كان اسم الدالة يحتوي ".بناء"
            // Detect constructor: if function name contains ".بناء"
            // ================================================================
            context_info_.currentConstructorClass.clear();
            context_info_.currentMethodClass.clear();
            std::string funcName = sirFunc->getName();
            // بناء = \xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1
            std::string ctorSuffix = ".\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1";
            size_t ctorPos = funcName.find(ctorSuffix);
            if (ctorPos != std::string::npos)
            {
                context_info_.currentConstructorClass = funcName.substr(0, ctorPos);
#ifndef NDEBUG
                std::cout << "[DEBUG] emitFunctionBody: detected constructor for class '"
                          << context_info_.currentConstructorClass << "'" << std::endl;
#endif
                // (AR) تسجيل %self في objectClassMap للباني
                // (EN) Register %self in objectClassMap for constructor
                context_info_.objectClassMap["%self"] = context_info_.currentConstructorClass;
                context_info_.objectClassMap["self"] = context_info_.currentConstructorClass;
            }
            else
            {
                // ================================================================
                // كشف الدوال (Methods): إذا كان الاسم يحتوي على "." والبادئة صنف معروف
                // Detect methods: if name contains "." and prefix is a known class
                // ================================================================
                size_t dotPos = funcName.find('.');
                if (dotPos != std::string::npos)
                {
                    std::string prefix = funcName.substr(0, dotPos);
                    if (context_info_.classStructTypes.find(prefix) != context_info_.classStructTypes.end())
                    {
                        context_info_.currentMethodClass = prefix;
#ifndef NDEBUG
                        std::cout << "[DEBUG] emitFunctionBody: detected method for class '"
                                  << context_info_.currentMethodClass << "'" << std::endl;
#endif
                        // (AR) تسجيل %self في objectClassMap للدالة
                        // (EN) Register %self in objectClassMap for method
                        context_info_.objectClassMap["%self"] = context_info_.currentMethodClass;
                        context_info_.objectClassMap["self"] = context_info_.currentMethodClass;
                    }
                }
            }

            // Source: SIRFunction::getBasicBlocks() is at sir_module.h:299
            const auto &basicBlocks = sirFunc->getBasicBlocks();

            if (basicBlocks.empty())
            {
                // دالة بدون جسم (تصريح فقط)
                // Function without body (declaration only)
                return;
            }

            // المرحلة 1: إنشاء جميع الكتل الأساسية أولاً (قبل المعاملات)
            // Phase 1: Create all basic blocks FIRST (before parameters)
            // Source: context_info_.basicBlocks is at llvm_codegen.h:619
            context_info_.basicBlocks.clear();

            // ========================================================================
            // FIX: Pre-scan all instructions to discover referenced labels
            // This fixes the bug where buildIfStatement creates blocks that aren't
            // registered in SIRFunction (currentFunction_ issue)
            // ========================================================================
            std::set<std::string> allLabels;
            for (const auto &sirBlock : basicBlocks)
            {
                if (!sirBlock)
                    continue;
                allLabels.insert(sirBlock->name);
                // Scan instructions for BR_COND and BR labels
                for (const auto &inst : sirBlock->instructions)
                {
                    if (inst.opcode == SIROpcode::BR_COND)
                    {
                        // BR_COND has: condition, trueLabel, falseLabel
                        if (inst.operands.size() >= 3)
                        {
                            if (inst.operands[1].type == SIROperandType::LABEL)
                            {
                                allLabels.insert(inst.operands[1].name);
                            }
                            if (inst.operands[2].type == SIROperandType::LABEL)
                            {
                                allLabels.insert(inst.operands[2].name);
                            }
                        }
                    }
                    else if (inst.opcode == SIROpcode::BR)
                    {
                        // BR has: targetLabel
                        if (!inst.operands.empty() && inst.operands[0].type == SIROperandType::LABEL)
                        {
                            allLabels.insert(inst.operands[0].name);
                        }
                    }
                }
            }

            // Create LLVM blocks for ALL discovered labels
            // FIX: Create the FIRST block in basicBlocks first — it's always the entry point
            //      Previously only "entry" was handled, but lambda entry blocks are named
            //      "lambda_entry" and were created after alphabetically-earlier blocks like
            //      "else_2", causing LLVM to treat the wrong block as entry.
            // (AR) إنشاء أول كتلة في basicBlocks أولاً — هي دائماً نقطة الدخول للدالة
            //      سابقاً كان يُعامَل فقط "entry" بشكل خاص، لكن اللامدا تستخدم "lambda_entry"
            //      وكانت تُنشأ بعد "else_2" (أبجدياً) مما يجعل LLVM يعتبر الكتلة الخطأ كنقطة دخول
            std::string entryBlockName = basicBlocks[0] ? basicBlocks[0]->name : "entry";
            {
                llvm::BasicBlock *entryBlock = llvm::BasicBlock::Create(
                    *context_, entryBlockName, llvmFunc);
                context_info_.basicBlocks[entryBlockName] = entryBlock;
            }
            for (const auto &labelName : allLabels)
            {
                if (labelName == entryBlockName)
                    continue; // already created as entry
#ifndef NDEBUG
                std::cout << "[DEBUG] emitFunctionBody: creating block '" << labelName << "'" << std::endl;
#endif

                llvm::BasicBlock *llvmBlock = llvm::BasicBlock::Create(
                    *context_, // Source: context_ is at llvm_codegen.h:631
                    labelName,
                    llvmFunc);

                context_info_.basicBlocks[labelName] = llvmBlock;
            }

            // Phase 2: Add parameters to named values (after blocks are created)
            emitFunctionParameters(sirFunc, llvmFunc);

            // (AR) إذا كانت كوروتين، أضف مقدمة الكوروتين (coro.id, coro.begin...)
            // (EN) If coroutine, emit coroutine preamble
            if (sirFunc->isCoroutine)
            {
                context_info_.isCoroutineFunction = true;
                context_info_.isGeneratorFunction = sirFunc->isGenerator;
                emitCoroutinePreamble(sirFunc, llvmFunc);
            }
            else
            {
                context_info_.isCoroutineFunction = false;
                context_info_.isGeneratorFunction = false;
            }

            // Phase 2: Emit instructions for each block
            for (size_t blockIdx = 0; blockIdx < basicBlocks.size(); ++blockIdx)
            {
                const auto &sirBlock = basicBlocks[blockIdx];
                if (!sirBlock)
                    continue;

                // Source: SIRBasicBlock::name is PUBLIC member at sir_instruction.h:355
                std::string blockName = sirBlock->name;

                auto it = context_info_.basicBlocks.find(blockName);
                if (it == context_info_.basicBlocks.end())
                {
                    reportError("Basic block not found: " + blockName);
                    continue;
                }

                llvm::BasicBlock *llvmBlock = it->second;

                // (AR) للكوروتينات: تعليمات الكتلة entry تُوضع في coro.init.resume
                // (EN) For coroutines: entry block instructions go into coro.init.resume
                // The entry block already has preamble + initial suspend (terminated by switch).
                // Body instructions must go into the init resume block instead.
                if (context_info_.isCoroutineFunction && blockName == "entry" && context_info_.currentBlock)
                {
                    llvmBlock = context_info_.currentBlock; // coro.init.resume
                }

                // تعيين نقطة الإدراج
                // Set insertion point
                // Source: builder_ is defined at llvm_codegen.h:637
                builder_->SetInsertPoint(llvmBlock);

                // Source: CodeGenContext::currentBlock is at llvm_codegen.h:616
                context_info_.currentBlock = llvmBlock;

                // إصدار تعليمات الكتلة
                // Emit block instructions
                // Source: SIRBasicBlock::instructions is PUBLIC member at sir_instruction.h:356
                // ================================================================
                // (AR) شبكة أمان: تخطي أي تعليمات بعد terminator في نفس الكتلة
                //      هذا يمنع خطأ LLVM "Terminator in middle of basic block"
                //      حتى لو كان SIR يحتوي على تعليمات بعد RET/BR عن طريق الخطأ
                // (EN) Safety net: skip any instructions after a terminator in same block
                //      This prevents LLVM "Terminator in middle of basic block" error
                //      even if SIR erroneously has instructions after RET/BR
                // ================================================================
                for (const auto &inst : sirBlock->instructions)
                {
                    // (AR) إذا كانت الكتلة الحالية تحتوي بالفعل على terminator، نتوقف
                    // (EN) If current block already has a terminator, stop emitting
                    if (builder_->GetInsertBlock() && builder_->GetInsertBlock()->getTerminator())
                    {
                        break;
                    }

                    auto instPtr = std::make_shared<SIRInstruction>(inst);
                    emitInstruction(instPtr);
                }
            }

            // ========================================================================
            // Phase 3: Ensure ALL basic blocks have terminators
            // FIX: Iterate ALL function blocks (not just SIR-named ones) to catch
            // dynamically-created blocks (gen.yield.X.resume, await.X.cont, etc.)
            // IMPORTANT: Skip coroutine infrastructure blocks (coro.final, coro.cleanup,
            // coro.suspend) — those get their terminators from emitCoroutineEpilogue().
            // ========================================================================
            for (auto &llvmBlock : *llvmFunc)
            {
                if (!llvmBlock.getTerminator())
                {
                    // (AR) تخطي كتل البنية التحتية للكوروتين — الخاتمة تعالجها
                    // (EN) Skip coroutine infrastructure blocks — epilogue handles them
                    if (sirFunc->isCoroutine)
                    {
                        std::string bname = llvmBlock.getName().str();
                        if (bname == "coro.final" || bname == "coro.cleanup" || bname == "coro.suspend")
                        {
                            continue; // Epilogue will add terminators to these
                        }
                    }

                    builder_->SetInsertPoint(&llvmBlock);
                    // (AR) للكوروتين/المولد: الكتل بدون terminator تقفز لـ coroFinalBB
                    // (EN) For coroutine/generator: unterminated blocks branch to coroFinalBB
                    if (sirFunc->isCoroutine && context_info_.coroFinalBB)
                    {
                        builder_->CreateBr(context_info_.coroFinalBB);
                    }
                    else if (llvmFunc->getReturnType()->isVoidTy())
                    {
                        builder_->CreateRetVoid();
                    }
                    else
                    {
                        // Return default value for non-void functions
                        builder_->CreateRet(llvm::Constant::getNullValue(llvmFunc->getReturnType()));
                    }
                }
            }

            // (AR) إذا كانت كوروتين، أضف خاتمة الكوروتين (تنظيف + تعليق)
            // (EN) If coroutine, emit epilogue (cleanup + suspend blocks)
            if (sirFunc->isCoroutine)
            {
                emitCoroutineEpilogue();
                context_info_.isCoroutineFunction = false;
                context_info_.isGeneratorFunction = false;
            }
        }

        /**
         * إصدار معاملات الدالة
         * Emit function parameters
         *
         * Source: llvm_codegen.h:376
         * @param sirFunc دالة SIR / SIR function
         * @param llvmFunc دالة LLVM / LLVM function
         */

    } // namespace LLVM
} // namespace Sad
