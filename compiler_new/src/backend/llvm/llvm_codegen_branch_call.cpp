/*
 * ============================================================================
 * LLVM IR Code Generator - Branch and Call Operations
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
#include <iostream>
#include <fstream>

using namespace Sad::Compiler::SIR;
using namespace Sad::Compiler; // (AR) للوصول لثوابت sir_constants.h

namespace Sad
{
    namespace LLVM
    {
        llvm::Value *LLVMCodeGen::emitBranch(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            // (AR) لا نضيف branch إذا كان البلوك الحالي ينتهي بـ ret
            // (EN) Skip branch if current block already has a terminator (e.g. ret)
            llvm::BasicBlock *currentBB = builder_->GetInsertBlock();
            if (currentBB && currentBB->getTerminator())
            {
                return nullptr; // Block already terminated
            }

            if (inst->operands.empty())
            {
                reportError("Branch instruction requires target label");
                return nullptr;
            }

            // Source: SIROperand::name is PUBLIC member at sir_types.h:293
            std::string targetLabel = inst->operands[0].name;

            auto it = context_info_.basicBlocks.find(targetLabel);
            if (it == context_info_.basicBlocks.end())
            {
                reportError("Target block not found for branch: " + targetLabel);
                return nullptr;
            }

            return builder_->CreateBr(it->second);
        }

        /**
         * إصدار تعليمة فرع مشروط
         * Emit conditional branch instruction
         *
         * Source: llvm_codegen.h:454
         */
        llvm::Value *LLVMCodeGen::emitCondBranch(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            // (AR) لا نضيف branch إذا كان البلوك الحالي ينتهي بـ ret
            // (EN) Skip conditional branch if current block already has a terminator
            llvm::BasicBlock *currentBB = builder_->GetInsertBlock();
            if (currentBB && currentBB->getTerminator())
            {
                return nullptr; // Block already terminated
            }

            if (inst->operands.size() < 3)
            {
                reportError("Conditional branch requires 3 operands");
                return nullptr;
            }

            llvm::Value *condition = resolveOperand(inst->operands[0]);
            std::string trueLabel = inst->operands[1].name;
            std::string falseLabel = inst->operands[2].name;

#ifndef NDEBUG
            std::cout << "[DEBUG] emitCondBranch: looking for trueLabel='" << trueLabel
                      << "', falseLabel='" << falseLabel << "'" << std::endl;
            std::cout << "[DEBUG] emitCondBranch: registered blocks count=" << context_info_.basicBlocks.size() << std::endl;
            for (const auto &[name, bb] : context_info_.basicBlocks)
            {
                std::cout << "[DEBUG] emitCondBranch: registered block '" << name << "'" << std::endl;
            }
#endif

            if (!condition)
            {
                reportError("Condition not found for conditional branch");
                return nullptr;
            }

            // (AR) التأكد من أن الشرط من نوع i1 — LLVM CreateCondBr يتطلب i1
            //      إذا كان الشرط ptr، نقارنه بـ null لتحويله إلى i1
            //      إذا كان الشرط i64 أو غيره، نقارنه بالصفر لتحويله إلى i1
            //      [إصلاح BF-04] السبب الجذري: ptr لا يقبل ConstantInt — يجب ConstantPointerNull
            // (EN) Ensure condition is i1 — LLVM CreateCondBr requires i1
            //      If condition is ptr, compare != null to convert to i1
            //      If condition is i64 or other integer, compare != 0 to convert to i1
            //      [Fix BF-04] Root cause: ptr doesn't accept ConstantInt — must use ConstantPointerNull
            if (!condition->getType()->isIntegerTy(1))
            {
                if (condition->getType()->isPointerTy())
                {
                    // (AR) الشرط من نوع مؤشر — نقارنه بـ null
                    // (EN) Condition is pointer type — compare with null
                    condition = builder_->CreateICmpNE(
                        condition,
                        llvm::ConstantPointerNull::get(
                            llvm::cast<llvm::PointerType>(condition->getType())),
                        "tobool.ptr");
                }
                else
                {
                    // (AR) الشرط من نوع عدد صحيح أو آخر — نقارنه بالصفر
                    // (EN) Condition is integer or other type — compare with zero
                    condition = builder_->CreateICmpNE(
                        condition,
                        llvm::ConstantInt::get(condition->getType(), 0),
                        "tobool");
                }
            }

            auto trueIt = context_info_.basicBlocks.find(trueLabel);
            auto falseIt = context_info_.basicBlocks.find(falseLabel);

            if (trueIt == context_info_.basicBlocks.end() ||
                falseIt == context_info_.basicBlocks.end())
            {
                reportError("Target blocks not found for conditional branch");
                return nullptr;
            }

            return builder_->CreateCondBr(condition, trueIt->second, falseIt->second);
        }

        /**
         * إصدار تعليمة استدعاء دالة
         * Emit function call instruction
         *
         * Source: llvm_codegen.h:455
         */
        llvm::Value *LLVMCodeGen::emitCall(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.empty())
            {
                reportError("Call instruction requires function name");
                return nullptr;
            }

            std::string funcName = inst->operands[0].name;

            // جمع المعاملات أولاً (نحتاجها لاستنتاج التوقيع إذا كانت الدالة خارجية)
            // Collect arguments first (needed to infer signature for external functions)
            std::vector<llvm::Value *> args;
            for (size_t i = 1; i < inst->operands.size(); ++i)
            {
                llvm::Value *arg = resolveOperand(inst->operands[i]);
                if (arg)
                {
                    args.push_back(arg);
                }
            }

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

            // ================================================================
            // (AR) دوال الخرائط (Hash Map) المضمنة — Runtime Functions
            //      بنية الخريطة في الذاكرة: {count:i64, capacity:i64, keys:ptr*, values:i64*, types:i64*}
            //      keys: مصفوفة مؤشرات نصوص (C strings) — null يعني خانة فارغة
            //      values: مصفوفة i64 — القيم المخزنة (ptrtoint للنصوص)
            //      types: مصفوفة i64 — نوع كل قيمة (0=نص, 1=رقم, 2=عشري, 3=منطقي)
            //      البحث: linear scan مع strcmp — مناسب للخرائط الصغيرة (<100 عنصر)
            // (EN) Hash Map built-in runtime functions
            //      Memory layout: {count:i64, capacity:i64, keys:ptr*, values:i64*, types:i64*}
            //      Search: linear scan with strcmp — suitable for small maps (<100 elements)
            // ================================================================

            if (funcName == "__sad_map_create")
            {
                // (AR) إنشاء خريطة جديدة بسعة أولية — malloc(5*8)=40 bytes للبنية
                //      ثم malloc(cap * 8) لكل من keys, values, types
                //      وتصفير keys بـ memset(0) لتعليم الخانات الفارغة
                // (EN) Create new map with initial capacity — malloc header + 3 arrays
                auto *i64Ty = getInt64Type();
                auto *ptrTy = llvm::PointerType::getUnqual(*context_);
                auto *i8Ty = llvm::Type::getInt8Ty(*context_);

                // (AR) حساب السعة: max(حجم_المُعطى * 2, 8)
                llvm::Value *hint = args.empty() ? llvm::ConstantInt::get(i64Ty, 0) : args[0];
                llvm::Value *doubled = builder_->CreateMul(hint, llvm::ConstantInt::get(i64Ty, 2), "cap.hint");
                llvm::Value *minCap = llvm::ConstantInt::get(i64Ty, 8);
                llvm::Value *cmp = builder_->CreateICmpUGT(doubled, minCap, "cap.cmp");
                llvm::Value *capacity = builder_->CreateSelect(cmp, doubled, minCap, "cap.final");

                // (AR) تخصيص البنية الرئيسية: 5 حقول * 8 = 40 بايت
                auto *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
                auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
                llvm::Value *mapPtr = builder_->CreateCall(mallocFunc,
                                                           {llvm::ConstantInt::get(i64Ty, 40)}, "map.ptr");

                // (AR) تخصيص المصفوفات (keys, values, types)
                llvm::Value *arrBytes = builder_->CreateMul(capacity, llvm::ConstantInt::get(i64Ty, 8), "arr.bytes");
                llvm::Value *keysPtr = builder_->CreateCall(mallocFunc, {arrBytes}, "map.keys");
                llvm::Value *valsPtr = builder_->CreateCall(mallocFunc, {arrBytes}, "map.vals");
                llvm::Value *typesPtr = builder_->CreateCall(mallocFunc, {arrBytes}, "map.types");

                // (AR) تصفير keys بالكامل — null يعني خانة فارغة
                auto *memsetType = llvm::FunctionType::get(ptrTy, {ptrTy, llvm::Type::getInt32Ty(*context_), i64Ty}, false);
                auto memsetFunc = module_->getOrInsertFunction("memset", memsetType);
                builder_->CreateCall(memsetFunc, {keysPtr, builder_->getInt32(0), arrBytes});

                // (AR) تخزين الحقول في البنية: [0]=count, [1]=capacity, [2]=keys, [3]=values, [4]=types
                llvm::Value *countGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                            {llvm::ConstantInt::get(i64Ty, 0)}, "map.count.gep");
                builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), countGep);

                llvm::Value *capGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                          {llvm::ConstantInt::get(i64Ty, 1)}, "map.cap.gep");
                builder_->CreateStore(capacity, capGep);

                llvm::Value *keysGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                           {llvm::ConstantInt::get(i64Ty, 2)}, "map.keys.gep");
                builder_->CreateStore(builder_->CreatePtrToInt(keysPtr, i64Ty), keysGep);

                llvm::Value *valsGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                           {llvm::ConstantInt::get(i64Ty, 3)}, "map.vals.gep");
                builder_->CreateStore(builder_->CreatePtrToInt(valsPtr, i64Ty), valsGep);

                llvm::Value *typesGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                            {llvm::ConstantInt::get(i64Ty, 4)}, "map.types.gep");
                builder_->CreateStore(builder_->CreatePtrToInt(typesPtr, i64Ty), typesGep);

                if (inst->result.has_value())
                    context_info_.namedValues[inst->result->name] = mapPtr;
                return mapPtr;
            }

            if (funcName == "__sad_map_set_typed")
            {
                // (AR) إدراج/تحديث زوج (مفتاح، قيمة) في الخريطة
                //      args: [0]=map, [1]=key(ptr), [2]=value(i64/ptr), [3]=typeTag(i64)
                //      خوارزمية: linear scan — ابحث عن مفتاح موجود أو أول خانة فارغة
                // (EN) Insert/update key-value pair in map
                //      Linear scan: find existing key or first empty slot
                if (args.size() < 4)
                    return nullptr;

                auto *i64Ty = getInt64Type();
                auto *ptrTy = llvm::PointerType::getUnqual(*context_);

                // (AR) تحويل map من i64 إلى ptr — المتغيرات تُخزَن كـ i64
                llvm::Value *mapPtr = args[0];
                if (mapPtr->getType() == i64Ty)
                    mapPtr = builder_->CreateIntToPtr(mapPtr, ptrTy, "mset.map.ptr");
                llvm::Value *key = args[1];
                llvm::Value *value = args[2];
                llvm::Value *typeTag = args[3];

                // (AR) تحميل count, capacity, keys, values, types من البنية
                llvm::Value *countGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                            {llvm::ConstantInt::get(i64Ty, 0)}, "mset.count.gep");
                llvm::Value *count = builder_->CreateLoad(i64Ty, countGep, "mset.count");
                llvm::Value *capGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                          {llvm::ConstantInt::get(i64Ty, 1)}, "mset.cap.gep");
                llvm::Value *cap = builder_->CreateLoad(i64Ty, capGep, "mset.cap");

                llvm::Value *keysArrGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                              {llvm::ConstantInt::get(i64Ty, 2)}, "mset.keys.gep");
                llvm::Value *keysI64 = builder_->CreateLoad(i64Ty, keysArrGep, "mset.keys.i64");
                llvm::Value *keysArr = builder_->CreateIntToPtr(keysI64, ptrTy, "mset.keys.ptr");

                llvm::Value *valsArrGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                              {llvm::ConstantInt::get(i64Ty, 3)}, "mset.vals.gep");
                llvm::Value *valsI64 = builder_->CreateLoad(i64Ty, valsArrGep, "mset.vals.i64");
                llvm::Value *valsArr = builder_->CreateIntToPtr(valsI64, ptrTy, "mset.vals.ptr");

                llvm::Value *typesArrGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                               {llvm::ConstantInt::get(i64Ty, 4)}, "mset.types.gep");
                llvm::Value *typesI64 = builder_->CreateLoad(i64Ty, typesArrGep, "mset.types.i64");
                llvm::Value *typesArr = builder_->CreateIntToPtr(typesI64, ptrTy, "mset.types.ptr");

                // (AR) بحث خطي: استدعاء __sad_map_find_slot لإيجاد الخانة
                // (EN) Linear search: call __sad_map_find_slot helper
                llvm::Function *findSlotFn = getOrCreateMapFindSlot();
                llvm::Value *slotIdx = builder_->CreateCall(findSlotFn,
                                                            {keysArr, cap, key}, "mset.slot");

                // (AR) فحص: هل الخانة فارغة (مفتاح المفتاح == null)؟
                llvm::Value *slotKeyGep = builder_->CreateGEP(ptrTy, keysArr,
                                                              {slotIdx}, "mset.slot.key.gep");
                llvm::Value *existingKey = builder_->CreateLoad(ptrTy, slotKeyGep, "mset.existing.key");
                llvm::Value *isNull = builder_->CreateICmpEQ(existingKey,
                                                             llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "mset.is.new");

                // (AR) تخزين المفتاح (نسخة strdup)
                auto *strdupType = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
                auto strdupFunc = module_->getOrInsertFunction("_strdup", strdupType);
                llvm::Value *keyCopy = builder_->CreateCall(strdupFunc, {key}, "mset.key.copy");
                // (AR) إذا كان الخانة جديدة، استخدم النسخة — وإلا احتفظ بالمفتاح القديم
                llvm::Value *finalKey = builder_->CreateSelect(isNull, keyCopy, existingKey, "mset.final.key");
                builder_->CreateStore(finalKey, slotKeyGep);

                // (AR) تخزين القيمة بتحويل نوعها حسب typeTag
                // (EN) Store value — for strings (type 0), store ptr as i64; for ints, store directly
                llvm::Value *valAsI64;
                if (value->getType()->isPointerTy())
                    valAsI64 = builder_->CreatePtrToInt(value, i64Ty, "mset.val.i64");
                else if (value->getType() == i64Ty)
                    valAsI64 = value;
                else
                    valAsI64 = builder_->CreateZExtOrTrunc(value, i64Ty, "mset.val.ext");

                llvm::Value *valSlotGep = builder_->CreateGEP(i64Ty, valsArr,
                                                              {slotIdx}, "mset.val.gep");
                builder_->CreateStore(valAsI64, valSlotGep);

                // (AR) تخزين نوع القيمة
                llvm::Value *typeSlotGep = builder_->CreateGEP(i64Ty, typesArr,
                                                               {slotIdx}, "mset.type.gep");
                llvm::Value *typeAsI64;
                if (typeTag->getType() == i64Ty)
                    typeAsI64 = typeTag;
                else
                    typeAsI64 = builder_->CreateZExtOrTrunc(typeTag, i64Ty, "mset.type.ext");
                builder_->CreateStore(typeAsI64, typeSlotGep);

                // (AR) زيادة count فقط إذا كانت الخانة جديدة
                llvm::Value *newCount = builder_->CreateAdd(count, llvm::ConstantInt::get(i64Ty, 1), "mset.new.count");
                llvm::Value *finalCount = builder_->CreateSelect(isNull, newCount, count, "mset.final.count");
                builder_->CreateStore(finalCount, countGep);

                return llvm::ConstantInt::get(i64Ty, 0);
            }

            if (funcName == "__sad_map_get" || funcName == "__sad_map_get_i64")
            {
                // (AR) قراءة قيمة من الخريطة بالمفتاح — ذكية حسب type tag
                //      __sad_map_get: يُرجع ptr (نص) — إذا القيمة رقم يُحوّلها لنص عبر sprintf
                //      __sad_map_get_i64: يُرجع i64 مباشرة — إذا القيمة نص يُرجع 0
                //      args: [0]=map, [1]=key(ptr)
                // (EN) Read value from map by key — type-tag aware
                //      __sad_map_get: returns ptr — if value is int, converts via sprintf
                //      __sad_map_get_i64: returns i64 — if value is string, returns 0
                if (args.size() < 2)
                    return nullptr;

                auto *i64Ty = getInt64Type();
                auto *ptrTy = llvm::PointerType::getUnqual(*context_);

                // (AR) تحويل map من i64 إلى ptr — المتغيرات تُخزَن كـ i64
                llvm::Value *mapPtr = args[0];
                if (mapPtr->getType() == i64Ty)
                    mapPtr = builder_->CreateIntToPtr(mapPtr, ptrTy, "mget.map.ptr");
                llvm::Value *key = args[1];

                // (AR) تحميل capacity, keys, values, types من البنية
                llvm::Value *capGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                          {llvm::ConstantInt::get(i64Ty, 1)}, "mget.cap.gep");
                llvm::Value *cap = builder_->CreateLoad(i64Ty, capGep, "mget.cap");

                llvm::Value *keysArrGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                              {llvm::ConstantInt::get(i64Ty, 2)}, "mget.keys.gep");
                llvm::Value *keysI64 = builder_->CreateLoad(i64Ty, keysArrGep, "mget.keys.i64");
                llvm::Value *keysArr = builder_->CreateIntToPtr(keysI64, ptrTy, "mget.keys.ptr");

                llvm::Value *valsArrGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                              {llvm::ConstantInt::get(i64Ty, 3)}, "mget.vals.gep");
                llvm::Value *valsI64 = builder_->CreateLoad(i64Ty, valsArrGep, "mget.vals.i64");
                llvm::Value *valsArr = builder_->CreateIntToPtr(valsI64, ptrTy, "mget.vals.ptr");

                llvm::Value *typesArrGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                               {llvm::ConstantInt::get(i64Ty, 4)}, "mget.types.gep");
                llvm::Value *typesI64 = builder_->CreateLoad(i64Ty, typesArrGep, "mget.types.i64");
                llvm::Value *typesArr = builder_->CreateIntToPtr(typesI64, ptrTy, "mget.types.ptr");

                // (AR) بحث عن المفتاح
                llvm::Function *findSlotFn = getOrCreateMapFindSlot();
                llvm::Value *slotIdx = builder_->CreateCall(findSlotFn,
                                                            {keysArr, cap, key}, "mget.slot");

                // (AR) قراءة القيمة ونوعها
                llvm::Value *valGep = builder_->CreateGEP(i64Ty, valsArr,
                                                          {slotIdx}, "mget.val.gep");
                llvm::Value *valI64 = builder_->CreateLoad(i64Ty, valGep, "mget.val");

                llvm::Value *typeGep = builder_->CreateGEP(i64Ty, typesArr,
                                                           {slotIdx}, "mget.type.gep");
                llvm::Value *typeTag = builder_->CreateLoad(i64Ty, typeGep, "mget.type");

                if (funcName == "__sad_map_get")
                {
                    // (AR) إرجاع ptr — لكن نفحص: إذا type=0 (نص) نحول مباشرة
                    //      إذا type=1 (رقم) نحول بـ sprintf لنص
                    //      هذا يحل مشكلة الخرائط المختلطة الأنواع
                    // (EN) Return ptr — but check type: if string → inttoptr, if int → sprintf to string
                    llvm::Value *isString = builder_->CreateICmpEQ(typeTag,
                                                                   llvm::ConstantInt::get(i64Ty, 0), "mget.is.str");

                    // (AR) نستخدم select: إذا نص → inttoptr(val)
                    //      إذا رقم → sprintf(buf, "%lld", val) → buf
                    llvm::Value *strPtr = builder_->CreateIntToPtr(valI64, ptrTy, "mget.as.ptr");

                    // (AR) تخصيص ذاكرة مؤقتة 32 بايت لتحويل الرقم لنص
                    auto *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
                    auto mallocFn = module_->getOrInsertFunction("malloc", mallocType);
                    llvm::Value *buf = builder_->CreateCall(mallocFn,
                                                            {llvm::ConstantInt::get(i64Ty, 32)}, "mget.sprintf.buf");

                    // (AR) استدعاء sprintf لتحويل الرقم لنص
                    auto *sprintfType = llvm::FunctionType::get(
                        llvm::Type::getInt32Ty(*context_),
                        {ptrTy, ptrTy, i64Ty}, true);
                    auto sprintfFn = module_->getOrInsertFunction("sprintf", sprintfType);
                    llvm::Value *fmtStr = builder_->CreateGlobalStringPtr("%lld", "mget.fmt.lld");
                    builder_->CreateCall(sprintfFn, {buf, fmtStr, valI64});

                    // (AR) اختيار: نص أصلي أو الرقم المُحوّل
                    llvm::Value *result = builder_->CreateSelect(isString, strPtr, buf, "mget.result");

                    if (inst->result.has_value())
                        context_info_.namedValues[inst->result->name] = result;
                    return result;
                }
                else
                {
                    // __sad_map_get_i64: (AR) إرجاع i64 مباشرة — النوع المناسب للأرقام
                    // (EN) Return i64 directly — suitable for integer values
                    llvm::Value *result = valI64;

                    if (inst->result.has_value())
                        context_info_.namedValues[inst->result->name] = result;
                    return result;
                }
            }

            if (funcName == "__sad_map_size")
            {
                // (AR) حجم الخريطة — قراءة count من البنية
                // (EN) Map size — read count from header
                if (args.empty())
                    return nullptr;

                auto *i64Ty = getInt64Type();
                auto *ptrTy = llvm::PointerType::getUnqual(*context_);

                // (AR) تحويل map من i64 إلى ptr
                llvm::Value *mapArg = args[0];
                if (mapArg->getType() == i64Ty)
                    mapArg = builder_->CreateIntToPtr(mapArg, ptrTy, "msize.map.ptr");
                llvm::Value *countGep = builder_->CreateGEP(i64Ty, mapArg,
                                                            {llvm::ConstantInt::get(i64Ty, 0)}, "msize.count.gep");
                llvm::Value *result = builder_->CreateLoad(i64Ty, countGep, "msize.count");

                if (inst->result.has_value())
                    context_info_.namedValues[inst->result->name] = result;
                return result;
            }

            if (funcName == "__sad_map_has")
            {
                // (AR) فحص وجود مفتاح — ابحث ثم تحقق إذا الخانة مشغولة
                // (EN) Check if key exists — find slot, check if occupied
                if (args.size() < 2)
                    return nullptr;

                auto *i64Ty = getInt64Type();
                auto *ptrTy = llvm::PointerType::getUnqual(*context_);

                // (AR) تحويل map من i64 إلى ptr
                llvm::Value *mapPtr = args[0];
                if (mapPtr->getType() == i64Ty)
                    mapPtr = builder_->CreateIntToPtr(mapPtr, ptrTy, "mhas.map.ptr");
                llvm::Value *key = args[1];

                llvm::Value *capGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                          {llvm::ConstantInt::get(i64Ty, 1)}, "mhas.cap.gep");
                llvm::Value *cap = builder_->CreateLoad(i64Ty, capGep, "mhas.cap");

                llvm::Value *keysArrGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                              {llvm::ConstantInt::get(i64Ty, 2)}, "mhas.keys.gep");
                llvm::Value *keysI64 = builder_->CreateLoad(i64Ty, keysArrGep, "mhas.keys.i64");
                llvm::Value *keysArr = builder_->CreateIntToPtr(keysI64, ptrTy, "mhas.keys.ptr");

                llvm::Function *findSlotFn = getOrCreateMapFindSlot();
                llvm::Value *slotIdx = builder_->CreateCall(findSlotFn,
                                                            {keysArr, cap, key}, "mhas.slot");

                // (AR) قراءة المفتاح في الخانة — إذا != null → موجود
                llvm::Value *slotKeyGep = builder_->CreateGEP(ptrTy, keysArr,
                                                              {slotIdx}, "mhas.slot.key.gep");
                llvm::Value *slotKey = builder_->CreateLoad(ptrTy, slotKeyGep, "mhas.slot.key");
                llvm::Value *isNotNull = builder_->CreateICmpNE(slotKey,
                                                                llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "mhas.found");
                llvm::Value *result = builder_->CreateZExt(isNotNull, i64Ty, "mhas.result");

                if (inst->result.has_value())
                    context_info_.namedValues[inst->result->name] = result;
                return result;
            }

            if (funcName == "__sad_map_keys" || funcName == "__sad_map_values")
            {
                // (AR) إرجاع مصفوفة المفاتيح أو القيم من الخريطة
                //      مفاتيح: نسخ المفاتيح غير الفارغة إلى مصفوفة جديدة
                //      قيم: نسخ القيم المقابلة للمفاتيح غير الفارغة
                // (EN) Return array of keys or values from map
                if (args.empty())
                    return nullptr;

                auto *i64Ty = getInt64Type();
                auto *ptrTy = llvm::PointerType::getUnqual(*context_);

                // (AR) تحويل map من i64 إلى ptr
                llvm::Value *mapPtr = args[0];
                if (mapPtr->getType() == i64Ty)
                    mapPtr = builder_->CreateIntToPtr(mapPtr, ptrTy, "mkvs.map.ptr");
                bool isKeys = (funcName == "__sad_map_keys");

                // (AR) تحميل count, capacity, keys, values
                llvm::Value *countGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                            {llvm::ConstantInt::get(i64Ty, 0)}, "mkvs.count.gep");
                llvm::Value *count = builder_->CreateLoad(i64Ty, countGep, "mkvs.count");
                llvm::Value *capGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                          {llvm::ConstantInt::get(i64Ty, 1)}, "mkvs.cap.gep");
                llvm::Value *cap = builder_->CreateLoad(i64Ty, capGep, "mkvs.cap");

                llvm::Value *keysArrGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                              {llvm::ConstantInt::get(i64Ty, 2)}, "mkvs.keys.gep");
                llvm::Value *keysI64 = builder_->CreateLoad(i64Ty, keysArrGep, "mkvs.keys.i64");
                llvm::Value *keysArr = builder_->CreateIntToPtr(keysI64, ptrTy, "mkvs.keys.ptr");

                llvm::Value *srcArrGep;
                if (isKeys)
                {
                    srcArrGep = keysArrGep; // (AR) نسخ المفاتيح
                }
                else
                {
                    srcArrGep = builder_->CreateGEP(i64Ty, mapPtr,
                                                    {llvm::ConstantInt::get(i64Ty, 3)}, "mkvs.vals.gep"); // (AR) نسخ القيم
                }
                llvm::Value *srcI64 = builder_->CreateLoad(i64Ty, srcArrGep, "mkvs.src.i64");
                llvm::Value *srcArr = builder_->CreateIntToPtr(srcI64, ptrTy, "mkvs.src.ptr");

                // (AR) استدعاء دالة مساعدة لبناء المصفوفة من الخانات غير الفارغة
                llvm::Function *collectFn = getOrCreateMapCollect();
                llvm::Value *result = builder_->CreateCall(collectFn,
                                                           {keysArr, srcArr, cap, count}, "mkvs.result");

                if (inst->result.has_value())
                    context_info_.namedValues[inst->result->name] = result;
                return result;
            }

            if (funcName == "__sad_map_merge")
            {
                // (AR) دمج خريطة مصدر في خريطة هدف — غير مدعوم حالياً، يُتجاهل بهدوء
                // (EN) Merge source map into target — not supported yet, silently ignored
                return llvm::ConstantInt::get(getInt64Type(), 0);
            }

            llvm::Function *callee = nullptr;
            auto funcIt = context_info_.functions.find(funcName);
            if (funcIt != context_info_.functions.end())
            {
                callee = funcIt->second;
            }

            // ═══════════════════════════════════════════════════════════════
            // (AR) بحث سلسلة الوراثة: إذا لم نجد الدالة وكانت استدعاء طريقة (className.method)
            //      نبحث في الأصناف الأب حتى نجد التعريف
            // (EN) Inheritance chain lookup: if method not found and name contains '.'
            //      walk parent classes via classParentMap to find the definition
            // ═══════════════════════════════════════════════════════════════
            if (!callee)
            {
                auto dotPos = funcName.find('.');
                if (dotPos != std::string::npos)
                {
                    std::string className = funcName.substr(0, dotPos);
                    std::string methodName = funcName.substr(dotPos + 1);
                    std::string searchClass = className;
                    while (!searchClass.empty())
                    {
                        auto parentIt = context_info_.classParentMap.find(searchClass);
                        if (parentIt != context_info_.classParentMap.end() && !parentIt->second.empty())
                        {
                            searchClass = parentIt->second;
                            std::string parentMethodName = searchClass + "." + methodName;
                            auto parentFuncIt = context_info_.functions.find(parentMethodName);
                            if (parentFuncIt != context_info_.functions.end())
                            {
                                callee = parentFuncIt->second;
                                // (AR) حفظ في السياق لتسريع الاستدعاءات اللاحقة
                                // (EN) Cache for future calls
                                context_info_.functions[funcName] = callee;
                                break;
                            }
                            // (AR) محاولة البحث في LLVM module مباشرة
                            // (EN) Try searching LLVM module directly
                            callee = module_->getFunction(parentMethodName);
                            if (callee)
                            {
                                context_info_.functions[funcName] = callee;
                                break;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }

            if (!callee)
            {
                // =====================================================================
                // (AR) دعم الربط عبر الملفات: إنشاء تصريح خارجي للدالة
                // (EN) Cross-file linking support: create extern declaration for function
                // عندما تستدعي دالة من ملف آخر، ننشئ تصريحاً (declare) في الوحدة الحالية
                // When calling a function from another file, create a declaration in current module
                // الرابط (linker) سيحل الرمز لاحقاً
                // The linker will resolve the symbol later
                // =====================================================================

                // استنتاج أنواع المعاملات من القيم الفعلية
                // Infer parameter types from actual values
                std::vector<llvm::Type *> paramTypes;
                for (const auto &arg : args)
                {
                    paramTypes.push_back(arg->getType());
                }

                // استنتاج نوع الإرجاع من نتيجة التعليمة
                // Infer return type from instruction result
                llvm::Type *returnType = getVoidType();
                if (inst->result.has_value())
                {
                    switch (inst->result->dataType)
                    {
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
                    case SadTypeKind::String:
                        returnType = getInt8PtrType();
                        break;
                    case SadTypeKind::Void:
                        returnType = getVoidType();
                        break;
                    default:
                        returnType = getInt64Type();
                        break;
                    }
                }

                // إنشاء نوع الدالة والتصريح الخارجي
                // Create function type and extern declaration
                llvm::FunctionType *funcType = llvm::FunctionType::get(
                    returnType, paramTypes, false);

                llvm::FunctionCallee fc = module_->getOrInsertFunction(funcName, funcType);
                callee = llvm::dyn_cast<llvm::Function>(fc.getCallee());

                if (!callee)
                {
                    reportError("Failed to create extern declaration for: " + funcName);
                    return nullptr;
                }

                // حفظ في السياق لاستخدامات لاحقة
                // Cache in context for future calls
                context_info_.functions[funcName] = callee;
            }

            // إنشاء تعليمة الاستدعاء
            // Create call instruction

            // ================================================================
            // تحويل أنواع المعاملات إذا لزم الأمر (ptr↔i64)
            // Convert argument types if needed (ptr↔i64)
            // ================================================================
            llvm::FunctionType *funcType = callee->getFunctionType();

            // (AR) مطابقة عدد الوسائط مع عدد بارامترات الدالة
            //      إذا كان عدد الوسائط أكثر — اقتطع الزيادة
            //      إذا كان أقل — أضف قيم صفرية
            // (EN) Match arg count with function parameter count.
            //      If too many args — truncate. If too few — pad with zeros.
            unsigned expectedArgCount = funcType->getNumParams();
            if (args.size() > expectedArgCount)
            {
                args.resize(expectedArgCount);
            }
            else
            {
                while (args.size() < expectedArgCount)
                {
                    llvm::Type *paramType = funcType->getParamType(args.size());
                    args.push_back(llvm::Constant::getNullValue(paramType));
                }
            }

            for (size_t i = 0; i < args.size() && i < funcType->getNumParams(); ++i)
            {
                llvm::Type *expectedType = funcType->getParamType(i);
                llvm::Type *actualType = args[i]->getType();

                if (expectedType != actualType)
                {
                    if (expectedType->isIntegerTy(64) && actualType->isPointerTy())
                    {
                        // (AR) تحويل مؤشر → i64 (لتمرير كائنات للبناة)
                        // (EN) Convert ptr → i64 (for passing objects to constructors)
                        args[i] = builder_->CreatePtrToInt(args[i], getInt64Type(), "arg.ptrtoint");
                    }
                    else if (expectedType->isPointerTy() && actualType->isIntegerTy(64))
                    {
                        // (AR) تحويل i64 → مؤشر
                        // (EN) Convert i64 → ptr
                        args[i] = builder_->CreateIntToPtr(args[i],
                                                           llvm::PointerType::getUnqual(*context_), "arg.inttoptr");
                    }
                    else if (expectedType->isIntegerTy(64) && actualType->isIntegerTy(1))
                    {
                        // (AR) تحويل i1 (منطقي) → i64
                        // (EN) Convert i1 (bool) → i64
                        args[i] = builder_->CreateZExt(args[i], getInt64Type(), "arg.i1toi64");
                    }
                    else if (expectedType->isIntegerTy(1) && actualType->isIntegerTy(64))
                    {
                        // (AR) تحويل i64 → i1 (منطقي)
                        // (EN) Convert i64 → i1 (bool)
                        args[i] = builder_->CreateTrunc(args[i], getInt1Type(), "arg.i64toi1");
                    }
                    else if (expectedType->isPointerTy() && actualType->isIntegerTy(1))
                    {
                        // (AR) تحويل i1 → مؤشر (عبر i64)
                        // (EN) Convert i1 → ptr (via i64)
                        llvm::Value *ext = builder_->CreateZExt(args[i], getInt64Type(), "arg.i1toi64");
                        args[i] = builder_->CreateIntToPtr(ext,
                                                           llvm::PointerType::getUnqual(*context_), "arg.i1toptr");
                    }
                    else if (expectedType->isDoubleTy() && actualType->isIntegerTy())
                    {
                        // (AR) تحويل عدد صحيح → عشري
                        // (EN) Convert integer → double
                        args[i] = builder_->CreateSIToFP(args[i], getDoubleType(), "arg.itofp");
                    }
                    else if (expectedType->isIntegerTy() && actualType->isDoubleTy())
                    {
                        // (AR) تحويل عشري → عدد صحيح
                        // (EN) Convert double → integer
                        args[i] = builder_->CreateFPToSI(args[i], expectedType, "arg.fptoi");
                    }
                }
            }

            llvm::Value *result = nullptr;
            if (callee->getReturnType()->isVoidTy())
            {
                // (AR) إصلاح: إذا كانت الدالة void في LLVM لكن SIR تتوقع قيمة إرجاع (I64 مثلاً)
                //      هذا يحدث عند خارجي("inl") دالة اقرأ_منفذ_32(...) — الدالة الخارجية بدون جسم
                //      يُعيّن نوع إرجاعها VOID لكنها فعلياً تُرجع I64
                //      الحل: استدعاء inline assembly مباشرة بنوع الإرجاع الصحيح
                // (EN) Fix: If LLVM function is void but SIR expects a return value,
                //      create a new declaration with correct return type via getOrInsertFunction
                bool needsReturnValue = inst->result.has_value() &&
                                        inst->result->dataType != SadTypeKind::Void;
                if (needsReturnValue)
                {
                    // (AR) تحديد نوع الإرجاع المطلوب
                    llvm::Type *wantedRetType = getInt64Type();
                    switch (inst->result->dataType)
                    {
                    case SadTypeKind::Float:
                        wantedRetType = getDoubleType();
                        break;
                    case SadTypeKind::Boolean:
                        wantedRetType = getInt1Type();
                        break;
                    case SadTypeKind::Pointer:
                    case SadTypeKind::String:
                        wantedRetType = getInt8PtrType();
                        break;
                    default:
                        wantedRetType = getInt64Type();
                        break;
                    }
                    // (AR) إعادة بناء نوع الدالة مع نوع إرجاع صحيح
                    std::vector<llvm::Type *> pTypes;
                    for (unsigned i = 0; i < callee->getFunctionType()->getNumParams(); ++i)
                    {
                        pTypes.push_back(callee->getFunctionType()->getParamType(i));
                    }
                    llvm::FunctionType *newFuncType = llvm::FunctionType::get(wantedRetType, pTypes, false);

                    // (AR) إنشاء استدعاء مباشر بنوع الدالة الجديد عبر FunctionCallee
                    //      لا نحذف الدالة القديمة — نستخدم FunctionCallee مع نوع مختلف
                    // (EN) Create direct call with new function type via FunctionCallee
                    //      Don't delete old function — use FunctionCallee with different type
                    llvm::FunctionCallee fc(newFuncType, callee);
                    result = builder_->CreateCall(fc, args, "calltmp");
                }
                else
                {
                    builder_->CreateCall(callee, args);
                    result = llvm::ConstantInt::get(getInt64Type(), 0);
                }
            }
            else
            {
                result = builder_->CreateCall(callee, args, "calltmp");
            }

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة رجوع
         * Emit return instruction
         *
         * Source: llvm_codegen.h:456
         */

        // ================================================================
        // (AR) دوال مساعدة للخرائط — تُنشأ كدوال LLVM داخلية عند الحاجة
        // (EN) Map helper functions — created as internal LLVM functions on-demand
        // ================================================================

        /**
         * @brief (AR) إنشاء/استرجاع دالة __sad_map_find_slot — بحث خطي عن مفتاح أو أول خانة فارغة
         *        (EN) Get or create __sad_map_find_slot — linear scan for key or first empty slot
         *
         * @return llvm::Function* (AR) الدالة: (ptr keysArr, i64 capacity, ptr key) → i64 slotIndex
         *
         * (AR) الخوارزمية: لكل i من 0 إلى capacity-1:
         *      إذا keys[i] == null → ارجع i (خانة فارغة)
         *      إذا strcmp(keys[i], key) == 0 → ارجع i (مفتاح موجود)
         *      خلاف ذلك → استمر
         *      إذا وصلنا النهاية بدون إيجاد → ارجع 0 (fallback)
         */
        llvm::Function *LLVMCodeGen::getOrCreateMapFindSlot()
        {
            const char *fnName = "__sad_map_find_slot";
            llvm::Function *fn = module_->getFunction(fnName);
            if (fn)
                return fn;

            auto *i64Ty = getInt64Type();
            auto *ptrTy = llvm::PointerType::getUnqual(*context_);
            auto *i32Ty = llvm::Type::getInt32Ty(*context_);

            // (AR) التوقيع: (ptr keysArr, i64 capacity, ptr key) → i64
            auto *fnType = llvm::FunctionType::get(i64Ty, {ptrTy, i64Ty, ptrTy}, false);
            fn = llvm::Function::Create(fnType, llvm::Function::InternalLinkage, fnName, *module_);

            auto *entry = llvm::BasicBlock::Create(*context_, "entry", fn);
            auto *loop = llvm::BasicBlock::Create(*context_, "loop", fn);
            auto *body = llvm::BasicBlock::Create(*context_, "body", fn);
            auto *checkKey = llvm::BasicBlock::Create(*context_, "check.key", fn);
            auto *found = llvm::BasicBlock::Create(*context_, "found", fn);
            auto *next = llvm::BasicBlock::Create(*context_, "next", fn);
            auto *notFound = llvm::BasicBlock::Create(*context_, "not.found", fn);

            auto argIt = fn->arg_begin();
            llvm::Value *keysArr = &*argIt++;
            llvm::Value *capacity = &*argIt++;
            llvm::Value *searchKey = &*argIt++;

            llvm::IRBuilder<> b(*context_);

            // entry: → loop
            b.SetInsertPoint(entry);
            b.CreateBr(loop);

            // loop: PHI idx = 0, cmp idx < capacity
            b.SetInsertPoint(loop);
            auto *idx = b.CreatePHI(i64Ty, 2, "idx");
            idx->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
            auto *done = b.CreateICmpUGE(idx, capacity, "done");
            b.CreateCondBr(done, notFound, body);

            // body: load keys[idx]
            b.SetInsertPoint(body);
            auto *keyGep = b.CreateGEP(ptrTy, keysArr, {idx}, "key.gep");
            auto *slotKey = b.CreateLoad(ptrTy, keyGep, "slot.key");
            auto *isNull = b.CreateICmpEQ(slotKey,
                                          llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "is.null");
            b.CreateCondBr(isNull, found, checkKey);

            // checkKey: strcmp(keys[idx], searchKey) == 0?
            b.SetInsertPoint(checkKey);
            auto *strcmpType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
            auto strcmpFn = module_->getOrInsertFunction("strcmp", strcmpType);
            auto *cmpRes = b.CreateCall(strcmpFn, {slotKey, searchKey}, "cmp");
            auto *isMatch = b.CreateICmpEQ(cmpRes, b.getInt32(0), "match");
            b.CreateCondBr(isMatch, found, next);

            // found: return idx
            b.SetInsertPoint(found);
            b.CreateRet(idx);

            // next: idx++, → loop
            b.SetInsertPoint(next);
            auto *nextIdx = b.CreateAdd(idx, llvm::ConstantInt::get(i64Ty, 1), "next.idx");
            idx->addIncoming(nextIdx, next);
            b.CreateBr(loop);

            // notFound: return 0 (fallback — shouldn't happen with proper capacity)
            b.SetInsertPoint(notFound);
            b.CreateRet(llvm::ConstantInt::get(i64Ty, 0));

            return fn;
        }

        /**
         * @brief (AR) إنشاء/استرجاع دالة __sad_map_collect — تجميع العناصر غير الفارغة في مصفوفة SadArray
         *        (EN) Get or create __sad_map_collect — collect non-null map entries into SadArray
         *
         * @return llvm::Function* (AR) الدالة: (ptr keysArr, ptr srcArr, i64 capacity, i64 count) → ptr SadArray
         *
         * (AR) بنية SadArray: {i64 length, i64 capacity, ptr data}
         *      تخصص المصفوفة + بيانات، ثم تنسخ العناصر غير الفارغة
         */
        llvm::Function *LLVMCodeGen::getOrCreateMapCollect()
        {
            const char *fnName = "__sad_map_collect";
            llvm::Function *fn = module_->getFunction(fnName);
            if (fn)
                return fn;

            auto *i64Ty = getInt64Type();
            auto *ptrTy = llvm::PointerType::getUnqual(*context_);

            // (AR) التوقيع: (ptr keysArr, ptr srcArr, i64 capacity, i64 count) → ptr
            auto *fnType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, i64Ty, i64Ty}, false);
            fn = llvm::Function::Create(fnType, llvm::Function::InternalLinkage, fnName, *module_);

            auto *entry = llvm::BasicBlock::Create(*context_, "entry", fn);
            auto *loop = llvm::BasicBlock::Create(*context_, "loop", fn);
            auto *body = llvm::BasicBlock::Create(*context_, "body", fn);
            auto *copySlot = llvm::BasicBlock::Create(*context_, "copy.slot", fn);
            auto *skip = llvm::BasicBlock::Create(*context_, "skip", fn);
            auto *done = llvm::BasicBlock::Create(*context_, "done", fn);

            auto argIt = fn->arg_begin();
            llvm::Value *keysArr = &*argIt++;
            llvm::Value *srcArr = &*argIt++;
            llvm::Value *capacity = &*argIt++;
            llvm::Value *count = &*argIt++;

            llvm::IRBuilder<> b(*context_);

            // entry: allocate SadArray {len, cap, data}
            b.SetInsertPoint(entry);
            auto *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFn = module_->getOrInsertFunction("malloc", mallocType);

            // (AR) بنية SadArray: 3 حقول i64 = 24 bytes
            llvm::Value *arrPtr = b.CreateCall(mallocFn,
                                               {llvm::ConstantInt::get(i64Ty, 24)}, "arr.ptr");
            // (AR) مصفوفة البيانات: count * 8 bytes
            llvm::Value *dataBytes = b.CreateMul(count, llvm::ConstantInt::get(i64Ty, 8), "data.bytes");
            // (AR) ضمان عدم تخصيص 0 bytes (للخرائط الفارغة)
            llvm::Value *minBytes = llvm::ConstantInt::get(i64Ty, 8);
            llvm::Value *cmpZero = b.CreateICmpUGT(dataBytes, llvm::ConstantInt::get(i64Ty, 0), "cmp.zero");
            llvm::Value *safeBytes = b.CreateSelect(cmpZero, dataBytes, minBytes, "safe.bytes");
            llvm::Value *dataPtr = b.CreateCall(mallocFn, {safeBytes}, "data.ptr");

            // (AR) تخزين: [0]=length=0 (سنزيدها), [1]=cap=count, [2]=data
            auto *lenGep = b.CreateGEP(i64Ty, arrPtr, {llvm::ConstantInt::get(i64Ty, 0)}, "len.gep");
            b.CreateStore(llvm::ConstantInt::get(i64Ty, 0), lenGep);
            auto *capGep = b.CreateGEP(i64Ty, arrPtr, {llvm::ConstantInt::get(i64Ty, 1)}, "cap.gep");
            b.CreateStore(count, capGep);
            auto *datGep = b.CreateGEP(i64Ty, arrPtr, {llvm::ConstantInt::get(i64Ty, 2)}, "dat.gep");
            b.CreateStore(b.CreatePtrToInt(dataPtr, i64Ty), datGep);

            b.CreateBr(loop);

            // loop: PHI srcIdx=0, dstIdx=0
            b.SetInsertPoint(loop);
            auto *srcIdx = b.CreatePHI(i64Ty, 2, "src.idx");
            srcIdx->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
            auto *dstIdx = b.CreatePHI(i64Ty, 2, "dst.idx");
            dstIdx->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
            auto *isDone = b.CreateICmpUGE(srcIdx, capacity, "is.done");
            b.CreateCondBr(isDone, done, body);

            // body: check if keys[srcIdx] != null
            b.SetInsertPoint(body);
            auto *keyGep = b.CreateGEP(ptrTy, keysArr, {srcIdx}, "key.gep");
            auto *slotKey = b.CreateLoad(ptrTy, keyGep, "slot.key");
            auto *isNull = b.CreateICmpEQ(slotKey,
                                          llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "is.null");
            b.CreateCondBr(isNull, skip, copySlot);

            // copySlot: data[dstIdx] = src[srcIdx], dstIdx++
            b.SetInsertPoint(copySlot);
            auto *srcGep = b.CreateGEP(i64Ty, srcArr, {srcIdx}, "src.gep");
            auto *srcVal = b.CreateLoad(i64Ty, srcGep, "src.val");
            auto *dstGep = b.CreateGEP(i64Ty, dataPtr, {dstIdx}, "dst.gep");
            b.CreateStore(srcVal, dstGep);
            auto *nextDst = b.CreateAdd(dstIdx, llvm::ConstantInt::get(i64Ty, 1), "next.dst");
            auto *nextSrc1 = b.CreateAdd(srcIdx, llvm::ConstantInt::get(i64Ty, 1), "next.src1");
            srcIdx->addIncoming(nextSrc1, copySlot);
            dstIdx->addIncoming(nextDst, copySlot);
            b.CreateBr(loop);

            // skip: srcIdx++, dstIdx stays
            b.SetInsertPoint(skip);
            auto *nextSrc2 = b.CreateAdd(srcIdx, llvm::ConstantInt::get(i64Ty, 1), "next.src2");
            srcIdx->addIncoming(nextSrc2, skip);
            dstIdx->addIncoming(dstIdx, skip);
            b.CreateBr(loop);

            // done: store final length, return arrPtr
            b.SetInsertPoint(done);
            b.CreateStore(dstIdx, lenGep);
            b.CreateRet(arrPtr);

            return fn;
        }

    } // namespace LLVM
} // namespace Sad
