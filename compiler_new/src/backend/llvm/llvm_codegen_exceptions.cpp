/*
 * ============================================================================
 * LLVM IR Code Generator - Exception Call Handlers
 * (AR) معالجة دوال runtime الاستثناءات المستدعاة عبر emitCall
 *      مستخرجة من llvm_codegen_branch_call.cpp بواسطة نمط Strangler Fig (CW-05)
 * (EN) Exception runtime call handlers extracted from llvm_codegen_branch_call.cpp
 *      via Strangler Fig pattern to comply with CW-05 (max 800 lines per file)
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include "sir_constants.h"
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/InlineAsm.h>
#include <optional>
#include <iostream>
#include <fstream>

using namespace Sad::Compiler::SIR;
using namespace Sad::Compiler; // (AR) للوصول لثوابت sir_constants.h

namespace Sad
{
    namespace LLVM
    {
        /**
         * @brief (AR) معالجة دوال runtime الاستثناءات المضمنة في emitCall
         *             __sad_alloc_jmpbuf, __sad_push_handler, __sad_setjmp,
         *             __sad_pop_handler, __sad_raise, __sad_raise_current,
         *             __sad_get_exception, __sad_get_exception_type, __sad_str_equals
         *        (EN) Handle built-in exception runtime functions dispatched from emitCall
         *
         * @return std::nullopt اذا لم يكن funcName دالة استثناء (تابع البحث)
         *         std::optional(value) اذا جرى التعامل معها (قد تكون nullptr عند الخطأ)
         * @return std::nullopt if funcName is not an exception function (keep looking)
         *         std::optional(value) if handled (value may be nullptr on error)
         */
        std::optional<llvm::Value *> LLVMCodeGen::emitCallException(
            const std::string &funcName,
            std::vector<llvm::Value *> &args,
            std::shared_ptr<SIRInstruction> inst)
        {
            // ================================================================
            // (AR) معالجة دوال runtime الاستثناءات (setjmp/longjmp)
            // (EN) Handle exception runtime functions (setjmp/longjmp based)
            // ================================================================

            if (funcName == "__sad_alloc_jmpbuf")
            {
                // (AR) تخصيص jmpbuf في كتلة الدخول (256 بايت لـ Windows x64)
                // (EN) Allocate jmpbuf in entry block (256 bytes for Windows x64)
                auto *currentBB = builder_->GetInsertBlock();
                auto *currentFunc = currentBB->getParent();
                auto &entryBB = currentFunc->getEntryBlock();

                llvm::IRBuilder<> entryBuilder(&entryBB, entryBB.begin());
                auto *jmpbufType = llvm::ArrayType::get(llvm::Type::getInt8Ty(*context_), 256);
                auto *jmpbuf = entryBuilder.CreateAlloca(jmpbufType, nullptr, "jmpbuf");
                jmpbuf->setAlignment(llvm::Align(16));

                llvm::Value *result = jmpbuf;
                if (inst->result.has_value())
                {
                    context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            }

            if (funcName == "__sad_push_handler")
            {
                // (AR) دفع jmpbuf إلى مكدس المعالجات — نأخذ المؤشر مباشرة بدون تحميل
                // (EN) Push jmpbuf onto handler stack — get pointer directly without loading
                llvm::Value *jmpbufPtr = nullptr;
                if (inst->operands.size() > 1)
                {
                    auto it = context_info_.namedValues.find(inst->operands[1].name);
                    if (it != context_info_.namedValues.end())
                        jmpbufPtr = it->second;
                }
                if (!jmpbufPtr)
                    return nullptr;

                auto *ptrType = llvm::PointerType::getUnqual(*context_);
                auto *i32Type = llvm::Type::getInt32Ty(*context_);

                auto *handlerStack = module_->getNamedGlobal(kRuntimeHandlerStack);
                if (!handlerStack)
                {
                    auto *arrType = llvm::ArrayType::get(ptrType, 64);
                    handlerStack = new llvm::GlobalVariable(
                        *module_, arrType, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantAggregateZero::get(arrType), kRuntimeHandlerStack);
                }

                auto *handlerCount = module_->getNamedGlobal(kRuntimeHandlerCount);
                if (!handlerCount)
                {
                    handlerCount = new llvm::GlobalVariable(
                        *module_, i32Type, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantInt::get(i32Type, 0), kRuntimeHandlerCount);
                }

                llvm::Value *count = builder_->CreateLoad(i32Type, handlerCount, "handler_count");
                auto *arrType = llvm::ArrayType::get(ptrType, 64);
                llvm::Value *slot = builder_->CreateGEP(arrType, handlerStack,
                                                        {builder_->getInt32(0), count}, "handler_slot");
                builder_->CreateStore(jmpbufPtr, slot);
                llvm::Value *newCount = builder_->CreateAdd(count, builder_->getInt32(1), "new_count");
                builder_->CreateStore(newCount, handlerCount);

                llvm::Value *dummy = llvm::ConstantInt::get(getInt64Type(), 0);
                if (inst->result.has_value())
                {
                    context_info_.namedValues[inst->result->name] = dummy;
                }
                return dummy;
            }

            if (funcName == "__sad_setjmp")
            {
                // (AR) استدعاء _setjmp — نأخذ المؤشر مباشرة بدون تحميل
                // (EN) Call _setjmp — get pointer directly without loading
                llvm::Value *jmpbufPtr = nullptr;
                if (inst->operands.size() > 1)
                {
                    auto it = context_info_.namedValues.find(inst->operands[1].name);
                    if (it != context_info_.namedValues.end())
                        jmpbufPtr = it->second;
                }
                if (!jmpbufPtr)
                    return nullptr;

                auto *ptrType = llvm::PointerType::getUnqual(*context_);
                auto *i32Type = llvm::Type::getInt32Ty(*context_);

                auto *setjmpFuncType = llvm::FunctionType::get(i32Type, {ptrType, ptrType}, false);
                auto setjmpCallee = module_->getOrInsertFunction("_setjmp", setjmpFuncType);
                if (auto *setjmpFunc = llvm::dyn_cast<llvm::Function>(setjmpCallee.getCallee()))
                {
                    setjmpFunc->addFnAttr(llvm::Attribute::ReturnsTwice);
                }

                llvm::Value *nullPtr = llvm::ConstantPointerNull::get(ptrType);
                llvm::Value *result32 = builder_->CreateCall(setjmpCallee, {jmpbufPtr, nullPtr}, "setjmp_result");
                llvm::Value *result = builder_->CreateSExt(result32, getInt64Type(), "setjmp_result64");

                if (inst->result.has_value())
                {
                    context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            }

            if (funcName == "__sad_pop_handler")
            {
                // (AR) إزالة آخر معالج من المكدس
                // (EN) Pop last handler from handler stack
                auto *i32Type = llvm::Type::getInt32Ty(*context_);

                auto *handlerCount = module_->getNamedGlobal(kRuntimeHandlerCount);
                if (!handlerCount)
                {
                    handlerCount = new llvm::GlobalVariable(
                        *module_, i32Type, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantInt::get(i32Type, 0), kRuntimeHandlerCount);
                }

                llvm::Value *count = builder_->CreateLoad(i32Type, handlerCount, "handler_count");
                llvm::Value *newCount = builder_->CreateSub(count, builder_->getInt32(1), "new_count");
                builder_->CreateStore(newCount, handlerCount);

                llvm::Value *dummy = llvm::ConstantInt::get(getInt64Type(), 0);
                if (inst->result.has_value())
                {
                    context_info_.namedValues[inst->result->name] = dummy;
                }
                return dummy;
            }

            if (funcName == "__sad_raise")
            {
                // (AR) رفع استثناء: تخزين النوع والرسالة + longjmp إلى آخر معالج
                // (EN) Raise exception: store type+message + longjmp to last handler
                // (AR) الصيغة: __sad_raise(type, msg) — إذا وسيط واحد فقط: type = "خطأ"
                // (EN) Format: __sad_raise(type, msg) — if single arg: type defaults to "خطأ"
                llvm::Value *excType = nullptr;
                llvm::Value *msg = nullptr;

                if (args.size() >= 2)
                {
                    excType = args[0];
                    msg = args[1];
                }
                else if (args.size() == 1)
                {
                    excType = getConstantString("\xd8\xae\xd8\xb7\xd8\xa3"); // "خطأ"
                    msg = args[0];
                }

                auto *ptrType = llvm::PointerType::getUnqual(*context_);
                auto *i32Type = llvm::Type::getInt32Ty(*context_);

                // (AR) تخزين نوع الاستثناء في متغير عام
                // (EN) Store exception type in global
                auto *exceptionType = module_->getNamedGlobal(kRuntimeExceptionType);
                if (!exceptionType)
                {
                    exceptionType = new llvm::GlobalVariable(
                        *module_, ptrType, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantPointerNull::get(ptrType), kRuntimeExceptionType);
                }
                if (excType)
                {
                    builder_->CreateStore(excType, exceptionType);
                }

                // (AR) تخزين رسالة الاستثناء في متغير عام
                // (EN) Store exception message in global
                auto *exceptionMsg = module_->getNamedGlobal(kRuntimeExceptionMsg);
                if (!exceptionMsg)
                {
                    exceptionMsg = new llvm::GlobalVariable(
                        *module_, ptrType, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantPointerNull::get(ptrType), kRuntimeExceptionMsg);
                }
                if (msg)
                {
                    builder_->CreateStore(msg, exceptionMsg);
                }

                // (AR) تحميل jmpbuf من مكدس المعالجات
                // (EN) Load jmpbuf from handler stack
                auto *handlerStack = module_->getNamedGlobal(kRuntimeHandlerStack);
                if (!handlerStack)
                {
                    auto *arrType = llvm::ArrayType::get(ptrType, 64);
                    handlerStack = new llvm::GlobalVariable(
                        *module_, arrType, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantAggregateZero::get(arrType), kRuntimeHandlerStack);
                }

                auto *handlerCount = module_->getNamedGlobal(kRuntimeHandlerCount);
                if (!handlerCount)
                {
                    handlerCount = new llvm::GlobalVariable(
                        *module_, i32Type, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantInt::get(i32Type, 0), kRuntimeHandlerCount);
                }

                llvm::Value *count = builder_->CreateLoad(i32Type, handlerCount, "handler_count");
                llvm::Value *idx = builder_->CreateSub(count, builder_->getInt32(1), "handler_idx");

                auto *arrType = llvm::ArrayType::get(ptrType, 64);
                llvm::Value *slot = builder_->CreateGEP(arrType, handlerStack,
                                                        {builder_->getInt32(0), idx}, "handler_slot");
                llvm::Value *jmpbuf = builder_->CreateLoad(ptrType, slot, "jmpbuf");

                // (AR) استدعاء longjmp — لا يعود أبداً
                // (EN) Call longjmp — never returns
                auto *longjmpFuncType = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(*context_), {ptrType, i32Type}, false);
                auto longjmpCallee = module_->getOrInsertFunction("longjmp", longjmpFuncType);
                if (auto *longjmpFunc = llvm::dyn_cast<llvm::Function>(longjmpCallee.getCallee()))
                {
                    longjmpFunc->addFnAttr(llvm::Attribute::NoReturn);
                }

                builder_->CreateCall(longjmpCallee, {jmpbuf, builder_->getInt32(1)});
                builder_->CreateUnreachable();

                // (AR) كتلة ميتة مع unreachable كـ terminator
                // (EN) Dead block with unreachable as terminator
                auto *deadBlock = llvm::BasicBlock::Create(
                    *context_, "dead.after_raise", builder_->GetInsertBlock()->getParent());
                builder_->SetInsertPoint(deadBlock);
                builder_->CreateUnreachable();

                llvm::Value *dummy = llvm::ConstantInt::get(getInt64Type(), 0);
                if (inst->result.has_value())
                {
                    context_info_.namedValues[inst->result->name] = dummy;
                }
                return dummy;
            }

            if (funcName == "__sad_raise_current")
            {
                // (AR) إعادة رمي الاستثناء الحالي — يقرأ النوع والرسالة من المتغيرات العامة
                // (EN) Re-throw current exception — reads type/msg from globals, calls longjmp
                auto *ptrType = llvm::PointerType::getUnqual(*context_);
                auto *i32Type = llvm::Type::getInt32Ty(*context_);

                // (AR) لا نغيّر المتغيرات العامة — نستخدم ما هو مخزن فعلاً
                // (EN) Don't modify globals — use what's already stored

                auto *handlerStack = module_->getNamedGlobal(kRuntimeHandlerStack);
                if (!handlerStack)
                {
                    auto *arrType = llvm::ArrayType::get(ptrType, 64);
                    handlerStack = new llvm::GlobalVariable(
                        *module_, arrType, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantAggregateZero::get(arrType), kRuntimeHandlerStack);
                }

                auto *handlerCount = module_->getNamedGlobal(kRuntimeHandlerCount);
                if (!handlerCount)
                {
                    handlerCount = new llvm::GlobalVariable(
                        *module_, i32Type, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantInt::get(i32Type, 0), kRuntimeHandlerCount);
                }

                llvm::Value *count = builder_->CreateLoad(i32Type, handlerCount, "handler_count");
                llvm::Value *idx = builder_->CreateSub(count, builder_->getInt32(1), "handler_idx");

                auto *arrType = llvm::ArrayType::get(ptrType, 64);
                llvm::Value *slot = builder_->CreateGEP(arrType, handlerStack,
                                                        {builder_->getInt32(0), idx}, "handler_slot");
                llvm::Value *jmpbuf = builder_->CreateLoad(ptrType, slot, "jmpbuf");

                auto *longjmpFuncType = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(*context_), {ptrType, i32Type}, false);
                auto longjmpCallee = module_->getOrInsertFunction("longjmp", longjmpFuncType);
                if (auto *longjmpFunc = llvm::dyn_cast<llvm::Function>(longjmpCallee.getCallee()))
                {
                    longjmpFunc->addFnAttr(llvm::Attribute::NoReturn);
                }

                builder_->CreateCall(longjmpCallee, {jmpbuf, builder_->getInt32(1)});
                builder_->CreateUnreachable();

                auto *deadBlock = llvm::BasicBlock::Create(
                    *context_, "dead.after_rethrow", builder_->GetInsertBlock()->getParent());
                builder_->SetInsertPoint(deadBlock);
                builder_->CreateUnreachable();

                llvm::Value *dummy = llvm::ConstantInt::get(getInt64Type(), 0);
                if (inst->result.has_value())
                {
                    context_info_.namedValues[inst->result->name] = dummy;
                }
                return dummy;
            }

            if (funcName == "__sad_get_exception")
            {
                // (AR) تحميل رسالة الاستثناء المحفوظة
                // (EN) Load stored exception message
                auto *ptrType = llvm::PointerType::getUnqual(*context_);

                auto *exceptionMsg = module_->getNamedGlobal(kRuntimeExceptionMsg);
                if (!exceptionMsg)
                {
                    exceptionMsg = new llvm::GlobalVariable(
                        *module_, ptrType, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantPointerNull::get(ptrType), kRuntimeExceptionMsg);
                }

                llvm::Value *result = builder_->CreateLoad(ptrType, exceptionMsg, "exception_msg");
                if (inst->result.has_value())
                {
                    context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            }

            if (funcName == "__sad_get_exception_type")
            {
                // (AR) تحميل نوع الاستثناء المحفوظ
                // (EN) Load stored exception type
                auto *ptrType = llvm::PointerType::getUnqual(*context_);

                auto *exceptionType = module_->getNamedGlobal(kRuntimeExceptionType);
                if (!exceptionType)
                {
                    exceptionType = new llvm::GlobalVariable(
                        *module_, ptrType, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantPointerNull::get(ptrType), kRuntimeExceptionType);
                }

                llvm::Value *result = builder_->CreateLoad(ptrType, exceptionType, "exception_type");
                if (inst->result.has_value())
                {
                    context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            }

            if (funcName == "__sad_str_equals")
            {
                // (AR) مقارنة نصين — يستدعي strcmp ويعيد 1 إذا متساويين، 0 إذا لا
                // (EN) Compare two strings — calls strcmp, returns 1 if equal, 0 if not
                if (args.size() < 2)
                    return nullptr;

                auto *ptrType = llvm::PointerType::getUnqual(*context_);
                auto *i32Type = llvm::Type::getInt32Ty(*context_);

                auto *strcmpFuncType = llvm::FunctionType::get(i32Type, {ptrType, ptrType}, false);
                auto strcmpCallee = module_->getOrInsertFunction("strcmp", strcmpFuncType);

                llvm::Value *cmpResult = builder_->CreateCall(strcmpCallee, {args[0], args[1]}, "strcmp_result");
                llvm::Value *isEqual = builder_->CreateICmpEQ(cmpResult, builder_->getInt32(0), "str_eq");
                llvm::Value *result = builder_->CreateZExt(isEqual, getInt64Type(), "str_eq_i64");

                if (inst->result.has_value())
                {
                    context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            }

            return std::nullopt;
        }

    } // namespace LLVM
} // namespace Sad
