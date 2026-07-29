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
#include "sad_dyn_repr.h" // (AR) DynKind لتهيئة الحقل ٤ homogKind / (EN) DynKind for field 4 homogKind init
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
#include <limits>
#include "builders/collections/array_builtins_codegen.h" // (Phase 7 Step 7)
#include "llvm_codegen.h"

// Source: llvm_codegen.h:103-108 - using declarations
using namespace Sad::Compiler::SIR; // For SIRModule, SIRFunction, SIRBasicBlock, SIRInstruction, SadTypeKind

namespace Sad
{
    namespace LLVM
    {

        // Helper function for array struct type
        static llvm::StructType *getArrayStructType(llvm::LLVMContext &ctx)
        {
            static llvm::StructType *arrTy = nullptr;
            if (!arrTy)
            {
                arrTy = llvm::StructType::create(ctx, {
                                                          llvm::Type::getInt64Ty(ctx),       // length
                                                          llvm::Type::getInt64Ty(ctx),       // capacity
                                                          llvm::PointerType::getUnqual(ctx), // data pointer
                                                          llvm::PointerType::getUnqual(ctx), // tags (i8*) or null [option A]
                                                          llvm::Type::getInt8Ty(ctx)         // homogKind (option A2): DynKind of a homogeneous array; read only when tags==null
                                                      },
                                                 "SadArray");
            }
            return arrTy;
        }

        llvm::Value *ArrayBuiltinsCodeGen::emitBuiltinArraySize(std::shared_ptr<SIRInstruction> inst)
        {
            return cg_.emitArrayLen(inst);
        }

        llvm::Value *ArrayBuiltinsCodeGen::emitBuiltinArrayIndexOf(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) بحث خطي في المصفوفة — يُرجع فهرس العنصر أو -1 إن لم يُوجد
            // (EN) Linear search through SadArray, returns index or -1
            // البنية: entry→loop→body→cont→loop (حلقة)، body→found، loop→notFound→merge
            if (!inst || inst->operands.size() < 2)
                return nullptr;

            llvm::Value *arrPtr = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *needle = cg_.resolveOperand(inst->operands[1]);
            if (!arrPtr || !needle)
                return nullptr;

            // (AR) تطبيع مؤشر المصفوفة عبر الدالة الموحّدة
            arrPtr = cg_.normalizeArrayPtr(arrPtr, "idxof");
            if (!arrPtr)
                return nullptr;

            auto i64Ty = cg_.getInt64Type();
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);

            // (AR) تحميل الطول ومؤشر البيانات من بنية المصفوفة
            llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "idxof.len.gep");
            llvm::Value *arrLen = cg_.builder_->CreateLoad(i64Ty, lenGep, "idxof.len");
            llvm::Value *datGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "idxof.dat.gep");
            llvm::Value *dataPtr = cg_.builder_->CreateLoad(ptrTy, datGep, "idxof.data");

            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();

            // (AR) إنشاء جميع الكتل الأساسية مسبقاً لتجنب أي ترتيب خاطئ
            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*cg_.context_, "idxof.loop", curFunc);
            llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(*cg_.context_, "idxof.body", curFunc);
            llvm::BasicBlock *contBB = llvm::BasicBlock::Create(*cg_.context_, "idxof.cont", curFunc);
            llvm::BasicBlock *foundBB = llvm::BasicBlock::Create(*cg_.context_, "idxof.found", curFunc);
            llvm::BasicBlock *notFoundBB = llvm::BasicBlock::Create(*cg_.context_, "idxof.notfound", curFunc);
            llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(*cg_.context_, "idxof.merge", curFunc);

            llvm::BasicBlock *entryBB = cg_.builder_->GetInsertBlock();
            cg_.builder_->CreateBr(loopBB);

            // === كتلة الحلقة (loop): فحص هل وصلنا لنهاية المصفوفة ===
            cg_.builder_->SetInsertPoint(loopBB);
            llvm::PHINode *idx = cg_.builder_->CreatePHI(i64Ty, 2, "idxof.idx");
            idx->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entryBB);
            llvm::Value *isDone = cg_.builder_->CreateICmpUGE(idx, arrLen, "idxof.done");
            cg_.builder_->CreateCondBr(isDone, notFoundBB, bodyBB);

            // === كتلة الجسم (body): مقارنة العنصر الحالي مع القيمة المطلوبة ===
            cg_.builder_->SetInsertPoint(bodyBB);
            llvm::Value *elemGep = cg_.builder_->CreateGEP(i64Ty, dataPtr, {idx}, "idxof.elem.gep");
            llvm::Value *elem = cg_.builder_->CreateLoad(i64Ty, elemGep, "idxof.elem");
            // ================================================================
            // (AR) إصلاح: needle قد يكون ptr (نص) أو i64 (رقم/منطقي).
            //      عند المقارنة يجب أن يكون الطرفان من نفس النوع.
            //      إذا needle هو ptr → نُحوّل elem من i64 إلى ptr للمقارنة.
            //      إذا needle هو i64 → نُقارن مباشرة.
            // (EN) Fix: needle may be ptr (string) or i64 (integer/bool).
            //      Both operands of icmp must have the same type.
            //      If needle is ptr → cast elem from i64 to ptr for comparison.
            //      If needle is i64 → compare directly.
            // ================================================================
            llvm::Value *lhsForCmp = elem;
            llvm::Value *rhsForCmp = needle;
            if (needle->getType()->isPointerTy() && elem->getType() != needle->getType())
            {
                // (AR) نحوّل elem (i64) إلى ptr لمطابقة النص
                // (EN) Cast elem (i64) to ptr to match string pointer
                lhsForCmp = cg_.builder_->CreateIntToPtr(elem, ptrTy, "idxof.elem.ptr");
            }
            else if (!needle->getType()->isPointerTy() && elem->getType() != needle->getType())
            {
                // (AR) نحوّل needle إلى i64 للمقارنة الرقمية
                // (EN) Cast needle to i64 for numeric comparison
                rhsForCmp = cg_.builder_->CreatePtrToInt(needle, i64Ty, "idxof.needle.i64");
            }
            llvm::Value *isEq = cg_.builder_->CreateICmpEQ(lhsForCmp, rhsForCmp, "idxof.eq");
            cg_.builder_->CreateCondBr(isEq, foundBB, contBB);

            // === كتلة الاستمرار (cont): زيادة العداد والعودة للحلقة ===
            cg_.builder_->SetInsertPoint(contBB);
            llvm::Value *nextIdx = cg_.builder_->CreateAdd(idx, llvm::ConstantInt::get(i64Ty, 1), "idxof.next");
            idx->addIncoming(nextIdx, contBB);
            cg_.builder_->CreateBr(loopBB);

            // === كتلة الإيجاد (found): وُجد العنصر — انتقل للدمج ===
            cg_.builder_->SetInsertPoint(foundBB);
            cg_.builder_->CreateBr(mergeBB);

            // === كتلة عدم الإيجاد (notFound): لم يُوجد — انتقل للدمج ===
            cg_.builder_->SetInsertPoint(notFoundBB);
            cg_.builder_->CreateBr(mergeBB);

            // === كتلة الدمج (merge): PHI تُرجع الفهرس أو -1 ===
            cg_.builder_->SetInsertPoint(mergeBB);
            llvm::PHINode *result = cg_.builder_->CreatePHI(i64Ty, 2, "idxof.result");
            result->addIncoming(idx, foundBB);
            result->addIncoming(llvm::ConstantInt::get(i64Ty, -1), notFoundBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ArrayBuiltinsCodeGen::emitBuiltinArrayContains(std::shared_ptr<SIRInstruction> inst)
        {
            // Delegates to indexOf, checks result != -1
            llvm::Value *idxResult = emitBuiltinArrayIndexOf(inst);
            if (!idxResult)
            {
                llvm::Value *falseval = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*cg_.context_), 0);
                if (inst && inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = falseval;
                }
                return falseval;
            }
            llvm::Value *result = cg_.builder_->CreateICmpNE(idxResult,
                                                         llvm::ConstantInt::get(cg_.getInt64Type(), -1), "contains.result");
            if (inst && inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ArrayBuiltinsCodeGen::emitBuiltinArrayReverse(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *arrPtr = cg_.resolveOperand(inst->operands[0]);
            if (!arrPtr)
                return nullptr;

            // (AR) تطبيع مؤشر المصفوفة عبر الدالة الموحّدة
            // (EN) Normalize arrPtr via unified helper
            arrPtr = cg_.normalizeArrayPtr(arrPtr, "rev");

            auto i64Ty = cg_.getInt64Type();
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);

            // Load length and data pointer
            llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "rev.len.gep");
            llvm::Value *arrLen = cg_.builder_->CreateLoad(i64Ty, lenGep, "rev.len");
            llvm::Value *datGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "rev.dat.gep");
            llvm::Value *dataPtr = cg_.builder_->CreateLoad(ptrTy, datGep, "rev.data");

            // In-place swap: i=0, j=len-1, while i < j: swap data[i], data[j]
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*cg_.context_, "rev.loop", curFunc);
            llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(*cg_.context_, "rev.body", curFunc);
            llvm::BasicBlock *doneBB = llvm::BasicBlock::Create(*cg_.context_, "rev.done", curFunc);

            llvm::Value *initJ = cg_.builder_->CreateSub(arrLen, llvm::ConstantInt::get(i64Ty, 1), "rev.initj");
            llvm::BasicBlock *entryBB = cg_.builder_->GetInsertBlock();
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(loopBB);
            llvm::PHINode *iVal = cg_.builder_->CreatePHI(i64Ty, 2, "rev.i");
            iVal->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entryBB);
            llvm::PHINode *jVal = cg_.builder_->CreatePHI(i64Ty, 2, "rev.j");
            jVal->addIncoming(initJ, entryBB);

            llvm::Value *cond = cg_.builder_->CreateICmpSLT(iVal, jVal, "rev.cond");
            cg_.builder_->CreateCondBr(cond, bodyBB, doneBB);

            cg_.builder_->SetInsertPoint(bodyBB);
            // Load data[i] and data[j]
            llvm::Value *iPtr = cg_.builder_->CreateGEP(i64Ty, dataPtr, {iVal}, "rev.iptr");
            llvm::Value *jPtr = cg_.builder_->CreateGEP(i64Ty, dataPtr, {jVal}, "rev.jptr");
            llvm::Value *iElem = cg_.builder_->CreateLoad(i64Ty, iPtr, "rev.ielem");
            llvm::Value *jElem = cg_.builder_->CreateLoad(i64Ty, jPtr, "rev.jelem");
            cg_.builder_->CreateStore(jElem, iPtr);
            cg_.builder_->CreateStore(iElem, jPtr);

            llvm::Value *nextI = cg_.builder_->CreateAdd(iVal, llvm::ConstantInt::get(i64Ty, 1));
            llvm::Value *nextJ = cg_.builder_->CreateSub(jVal, llvm::ConstantInt::get(i64Ty, 1));
            iVal->addIncoming(nextI, bodyBB);
            jVal->addIncoming(nextJ, bodyBB);
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(doneBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = arrPtr;
            }
            return arrPtr;
        }

        llvm::Value *ArrayBuiltinsCodeGen::emitBuiltinArraySort(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *arrPtr = cg_.context_info_.namedValues[inst->operands[0].name];
            if (!arrPtr)
                arrPtr = cg_.resolveOperand(inst->operands[0]);

            if (arrPtr)
            {
                // (AR) تطبيع مؤشر المصفوفة عبر الدالة الموحّدة
                // (EN) Normalize arrPtr via unified helper
                arrPtr = cg_.normalizeArrayPtr(arrPtr, "sort");

                auto i64Ty = cg_.getInt64Type();
                auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
                llvm::StructType *arrTy = getArrayStructType(*cg_.context_);

                // Load data pointer and length
                llvm::Value *dataGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "arr.data.gep");
                llvm::Value *dataPtr = cg_.builder_->CreateLoad(ptrTy, dataGep, "arr.data");
                llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "arr.len.gep");
                llvm::Value *len = cg_.builder_->CreateLoad(i64Ty, lenGep, "arr.len");

                // (AR) نوع العنصر يُمرَّر من الـ frontend كمعامل ثانٍ لتعليمة الفرز.
                //      القيمة الافتراضية تبقى Integer للحفاظ على التوافق مع أي SIR قديم.
                SadTypeKind elementType = SadTypeKind::Integer;
                if (inst->operands.size() > 1 &&
                    inst->operands[1].type == SIROperandType::CONSTANT)
                {
                    elementType = static_cast<SadTypeKind>(inst->operands[1].intValue);
                }

                // Call qsort(data, len, sizeof(i64), comparator)
                // (AR) نختار المقارن المناسب حسب نوع العنصر.
                // (AR) ‎qsort(ptr, size_t nmemb, size_t size, ptr)‎ — بـ i64 ثابتًا على
                //      32-بت تُزاح خانتا ‎size‎ و‎compar‎ ⇒ نداء لمؤشّر قمامة ⇒ تعطّل.
                llvm::Type *szTy = cg_.getSizeType();
                auto *qsortType = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(*cg_.context_),
                    {ptrTy, szTy, szTy, ptrTy}, false);
                auto qsortFunc = cg_.module_->getOrInsertFunction("qsort", qsortType);

                llvm::Function *cmpFunc = nullptr;
                if (elementType == SadTypeKind::String)
                {
                    cmpFunc = cg_.module_->getFunction("__sad_str_cmp");
                    if (!cmpFunc)
                    {
                        auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
                        auto *cmpFType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
                        cmpFunc = llvm::Function::Create(cmpFType, llvm::Function::InternalLinkage,
                                                         "__sad_str_cmp", cg_.module_.get());

                        auto *entry = llvm::BasicBlock::Create(*cg_.context_, "entry", cmpFunc);
                        auto *nullA = llvm::BasicBlock::Create(*cg_.context_, "null_a", cmpFunc);
                        auto *compare = llvm::BasicBlock::Create(*cg_.context_, "compare", cmpFunc);
                        llvm::IRBuilder<> tmpBuilder(entry);

                        auto args = cmpFunc->arg_begin();
                        llvm::Value *aSlot = &*args++;
                        llvm::Value *bSlot = &*args;
                        llvm::Value *aVal = tmpBuilder.CreateLoad(ptrTy, aSlot, "a.ptr");
                        llvm::Value *bVal = tmpBuilder.CreateLoad(ptrTy, bSlot, "b.ptr");
                        llvm::Value *aIsNull = tmpBuilder.CreateICmpEQ(
                            aVal, llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "a.null");
                        llvm::Value *bIsNull = tmpBuilder.CreateICmpEQ(
                            bVal, llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "b.null");
                        llvm::Value *anyNull = tmpBuilder.CreateOr(aIsNull, bIsNull, "any.null");
                        tmpBuilder.CreateCondBr(anyNull, nullA, compare);

                        llvm::IRBuilder<> nullABuilder(nullA);
                        llvm::Value *bothNull = nullABuilder.CreateAnd(aIsNull, bIsNull, "both.null");
                        llvm::Value *aOnlyNull = nullABuilder.CreateAnd(
                            aIsNull, nullABuilder.CreateNot(bIsNull), "a.only.null");
                        llvm::Value *nullResult = nullABuilder.CreateSelect(
                            bothNull,
                            llvm::ConstantInt::get(i32Ty, 0),
                            nullABuilder.CreateSelect(
                                aOnlyNull,
                                llvm::ConstantInt::getSigned(i32Ty, -1),
                                llvm::ConstantInt::get(i32Ty, 1)));
                        nullABuilder.CreateRet(nullResult);

                        llvm::IRBuilder<> compareBuilder(compare);
                        llvm::FunctionType *strcmpType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
                        llvm::FunctionCallee strcmpFn = cg_.module_->getOrInsertFunction("strcmp", strcmpType);
                        llvm::Value *cmpResult = compareBuilder.CreateCall(strcmpFn, {aVal, bVal}, "strcmp.ret");
                        compareBuilder.CreateRet(cmpResult);
                    }
                }
                else
                {
                    cmpFunc = cg_.module_->getFunction("__sad_i64_cmp");
                    if (!cmpFunc)
                    {
                        auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
                        auto *cmpFType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
                        cmpFunc = llvm::Function::Create(cmpFType, llvm::Function::InternalLinkage,
                                                         "__sad_i64_cmp", cg_.module_.get());
                        auto *entry = llvm::BasicBlock::Create(*cg_.context_, "entry", cmpFunc);
                        llvm::IRBuilder<> tmpBuilder(entry);
                        auto args = cmpFunc->arg_begin();
                        llvm::Value *aPtr = &*args++;
                        llvm::Value *bPtr = &*args;
                        llvm::Value *aVal = tmpBuilder.CreateLoad(i64Ty, aPtr, "a");
                        llvm::Value *bVal = tmpBuilder.CreateLoad(i64Ty, bPtr, "b");
                        llvm::Value *diff = tmpBuilder.CreateSub(aVal, bVal, "diff");
                        llvm::Value *truncated = tmpBuilder.CreateTrunc(diff, i32Ty, "trunc");
                        tmpBuilder.CreateRet(truncated);
                    }
                }

                // (AR) ‎nmemb‎ و‎size‎ بنوع ‎size_t‎ الهدف؛ ‎len‎ داخليًّا i64.
                cg_.builder_->CreateCall(qsortFunc,
                    {dataPtr, cg_.coerceToSize(len, "qsort.nmemb"),
                     llvm::ConstantInt::get(szTy, 8), cmpFunc});
            }

            if (inst->result.has_value() && arrPtr)
            {
                cg_.context_info_.namedValues[inst->result->name] = arrPtr;
            }
            return arrPtr;
        }

        llvm::Value *ArrayBuiltinsCodeGen::emitBuiltinArrayFirst(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            // Equivalent to ARRAY_GET(arr, 0)
            llvm::Value *arrPtr = cg_.context_info_.namedValues[inst->operands[0].name];
            if (!arrPtr)
                arrPtr = cg_.resolveOperand(inst->operands[0]);
            if (!arrPtr)
                return nullptr;

            // (AR) تطبيع مؤشر المصفوفة عبر الدالة الموحّدة
            // (EN) Normalize arrPtr via unified helper
            arrPtr = cg_.normalizeArrayPtr(arrPtr, "first");

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);

            llvm::Value *dataGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "arr.data.gep");
            llvm::Value *dataPtr = cg_.builder_->CreateLoad(ptrTy, dataGep, "arr.data");
            llvm::Value *result = cg_.builder_->CreateLoad(cg_.getInt64Type(), dataPtr, "arr.first");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ArrayBuiltinsCodeGen::emitBuiltinArrayLast(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *arrPtr = cg_.context_info_.namedValues[inst->operands[0].name];
            if (!arrPtr)
                arrPtr = cg_.resolveOperand(inst->operands[0]);
            if (!arrPtr)
                return nullptr;

            // (AR) تطبيع مؤشر المصفوفة عبر الدالة الموحّدة
            // (EN) Normalize arrPtr via unified helper
            arrPtr = cg_.normalizeArrayPtr(arrPtr, "last");

            auto i64Ty = cg_.getInt64Type();
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);

            llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "arr.len.gep");
            llvm::Value *len = cg_.builder_->CreateLoad(i64Ty, lenGep, "arr.len");
            llvm::Value *lastIdx = cg_.builder_->CreateSub(len, llvm::ConstantInt::get(i64Ty, 1), "last.idx");

            llvm::Value *dataGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "arr.data.gep");
            llvm::Value *dataPtr = cg_.builder_->CreateLoad(ptrTy, dataGep, "arr.data");
            llvm::Value *elemPtr = cg_.builder_->CreateGEP(i64Ty, dataPtr, {lastIdx}, "arr.last.ptr");
            llvm::Value *result = cg_.builder_->CreateLoad(i64Ty, elemPtr, "arr.last");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ArrayBuiltinsCodeGen::emitBuiltinArraySlice(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ARRAY_SLICE"}});
                return nullptr;
            }

            llvm::Value *arrPtr = cg_.context_info_.namedValues[inst->operands[0].name];
            if (!arrPtr)
                arrPtr = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *start = cg_.resolveOperand(inst->operands[1]);
            llvm::Value *end = cg_.resolveOperand(inst->operands[2]);
            if (!arrPtr || !start || !end)
                return nullptr;

            // (AR) معامل الخطوة: اختياري، الافتراضي = 1
            // (EN) Step parameter: optional, default = 1
            llvm::Value *step = nullptr;
            if (inst->operands.size() >= 4)
            {
                step = cg_.resolveOperand(inst->operands[3]);
            }

            // (AR) تطبيع مؤشر المصفوفة عبر الدالة الموحّدة
            // (EN) Normalize arrPtr via unified helper
            arrPtr = cg_.normalizeArrayPtr(arrPtr, "slice.src");

            auto i64Ty = cg_.getInt64Type();
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);

            // (AR) تحميل الطول الأصلي — نحتاجه لتطبيع الفهارس السالبة وقيمة الحارس
            // (EN) Load source array length — needed for normalizing negative indices and sentinel
            llvm::Value *srcLenGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "src.len.gep");
            llvm::Value *srcLen = cg_.builder_->CreateLoad(i64Ty, srcLenGep, "src.len");

            // (AR) تطبيع البداية: تحويل السالب إلى موجب (مثل م[-3:] = آخر 3 عناصر)
            // (EN) Normalize start: convert negative to positive (e.g. arr[-3:] = last 3 elements)
            start = cg_.normalizeArrayIndex(start, arrPtr, "slice.start");

            // (AR) معالجة النهاية: ثلاث حالات:
            //      1) end == INT64_MIN (حارس = لم يُحدَّد نهاية) → end = srcLen (حتى آخر المصفوفة)
            //      2) end < 0 (فهرس سالب صريح مثل م[2:-1]) → end = srcLen + end
            //      3) end >= 0 → استخدمه كما هو
            // (EN) Handle end: three cases:
            //      1) end == INT64_MIN (sentinel = no end specified) → end = srcLen (to end of array)
            //      2) end < 0 (explicit negative index like arr[2:-1]) → end = srcLen + end
            //      3) end >= 0 → use as-is
            {
                int64_t sentinel = std::numeric_limits<int64_t>::min();
                llvm::Value *sentinelVal = llvm::ConstantInt::get(i64Ty, static_cast<uint64_t>(sentinel), true);
                llvm::Value *isSentinel = cg_.builder_->CreateICmpEQ(end, sentinelVal, "end.is.sentinel");
                // (AR) الحالة 2: فهرس سالب عادي → srcLen + end
                llvm::Value *endIsNeg = cg_.builder_->CreateICmpSLT(end, llvm::ConstantInt::get(i64Ty, 0), "end.is.neg");
                llvm::Value *normalizedEnd = cg_.builder_->CreateAdd(srcLen, end, "end.neg.resolved");
                // (AR) إذا سالب (وليس حارس) → normalizedEnd، وإلا → end الأصلي
                llvm::Value *negResolved = cg_.builder_->CreateSelect(endIsNeg, normalizedEnd, end, "end.neg.or.orig");
                // (AR) إذا حارس → srcLen، وإلا → نتيجة الخطوة السابقة
                end = cg_.builder_->CreateSelect(isSentinel, srcLen, negResolved, "end.final");
            }

            // (AR) إذا لم يُحدد step أو كان الافتراضي = 1
            // (EN) If step not specified or default = 1
            if (!step)
            {
                step = llvm::ConstantInt::get(i64Ty, 1);
            }

            // (AR) التحقق: هل الخطوة = 1؟ → مسار سريع بـ memcpy
            //      وإلا → مسار حلقي عنصر بعنصر
            // (EN) Check: is step = 1? → fast path with memcpy, else → loop path element by element
            llvm::Value *stepIsOne = cg_.builder_->CreateICmpEQ(step, llvm::ConstantInt::get(i64Ty, 1), "step.is.one");

            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *fastBB = llvm::BasicBlock::Create(*cg_.context_, "slice.fast", curFunc);
            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*cg_.context_, "slice.loop.setup", curFunc);
            llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(*cg_.context_, "slice.merge", curFunc);

            cg_.builder_->CreateCondBr(stepIsOne, fastBB, loopBB);

            // ============================================================
            // (AR) المسار السريع: step=1 → حساب الطول + malloc + memcpy
            // (EN) Fast path: step=1 → compute length + malloc + memcpy
            // ============================================================
            cg_.builder_->SetInsertPoint(fastBB);

            llvm::Value *fastLen = cg_.builder_->CreateSub(end, start, "fast.len");

            auto *arrSize = llvm::ConstantExpr::getSizeOf(arrTy);
            llvm::Value *fastArr = cg_.emitMalloc(arrSize, "fast.arr");

            llvm::Value *fLenGep = cg_.builder_->CreateStructGEP(arrTy, fastArr, 0, "fast.len.gep");
            cg_.builder_->CreateStore(fastLen, fLenGep);
            llvm::Value *fCapGep = cg_.builder_->CreateStructGEP(arrTy, fastArr, 1, "fast.cap.gep");
            cg_.builder_->CreateStore(fastLen, fCapGep);

            llvm::Value *fDataSize = cg_.builder_->CreateMul(fastLen, llvm::ConstantInt::get(i64Ty, 8), "fast.data.size");
            llvm::Value *fNewData = cg_.emitMalloc(fDataSize, "fast.data");
            llvm::Value *fDataGep = cg_.builder_->CreateStructGEP(arrTy, fastArr, 2, "fast.data.gep");
            cg_.builder_->CreateStore(fNewData, fDataGep);
            // (AR) الحقل ٤ (homogKind): الشريحةُ تحفظ نوعَ كلّ عنصر ⇒ انسخ homogKind المصدر
            //      (يُقرأ فقط حين tags==null؛ للمصدر المختلط تُنسخ الوسوم فلا يُقرأ). صحيحٌ دومًا.
            // (EN) Field 4 (homogKind): a slice preserves each element's kind ⇒ copy the source's
            //      homogKind (read only when tags==null; a mixed source copies tags so it's unread).
            {
                llvm::Value *fSrcHkGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 4, "fast.src.homogkind.gep");
                llvm::Value *fSrcHk = cg_.builder_->CreateLoad(cg_.getInt8Type(), fSrcHkGep, "fast.src.homogkind");
                cg_.builder_->CreateStore(fSrcHk, cg_.builder_->CreateStructGEP(arrTy, fastArr, 4, "fast.homogkind.gep"));
            }
            // (AR) [وسم زمن-تشغيل] الحقل ٣ يُضبط أدناه بنسخِ نطاقِ وسوم المصدر (أو null).
            // (EN) [runtime tag] field 3 is set below by copying the source's tag range (or null).
            llvm::Value *fTagsGep = cg_.builder_->CreateStructGEP(arrTy, fastArr, 3, "fast.tags.gep");

            llvm::Value *srcDataGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "src.data.gep");
            llvm::Value *srcData = cg_.builder_->CreateLoad(ptrTy, srcDataGep, "src.data");
            llvm::Value *srcStart = cg_.builder_->CreateGEP(i64Ty, srcData, {start}, "src.start");

            // (AR) طول ‎mem*‎ بنوع ‎size_t‎ الهدف (i32 على 32-بت): الخلفيّة تولّد
            //      النداء المكتبيّ بهذا العرض، وتعريف وقت التشغيل الحرّ يطابقه.
            llvm::Type *szTy = cg_.getSizeType();
            auto *memcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, szTy}, false);
            auto memcpyFunc = cg_.module_->getOrInsertFunction("memcpy", memcpyType);
            cg_.builder_->CreateCall(memcpyFunc, {fNewData, srcStart,
                cg_.builder_->CreateZExtOrTrunc(fDataSize, szTy, "fast.size.sz")});

            // (AR) [وسم زمن-تشغيل] انسخ نطاقَ الوسوم [start, start+fastLen) إن كان للمصدر وسوم؛
            //      وإلّا اضبط الوسوم=null (متجانسة). auto i8Ty محليّ.
            // (EN) [runtime tag] copy the tag range [start, start+fastLen) if the source has
            //      tags; else set tags=null (homogeneous).
            auto i8TyF = cg_.getInt8Type();
            llvm::Value *fSrcTagsGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 3, "fast.src.tags.gep");
            llvm::Value *fSrcTags = cg_.builder_->CreateLoad(ptrTy, fSrcTagsGep, "fast.src.tags");
            llvm::Value *fSrcTagsNull = cg_.builder_->CreateICmpEQ(
                fSrcTags, llvm::ConstantPointerNull::get(ptrTy), "fast.src.tags.null");
            llvm::BasicBlock *fTagCopyBB = llvm::BasicBlock::Create(*cg_.context_, "slice.fast.tagcopy", curFunc);
            llvm::BasicBlock *fTagNullBB = llvm::BasicBlock::Create(*cg_.context_, "slice.fast.tagnull", curFunc);
            llvm::BasicBlock *fTagContBB = llvm::BasicBlock::Create(*cg_.context_, "slice.fast.tagcont", curFunc);
            cg_.builder_->CreateCondBr(fSrcTagsNull, fTagNullBB, fTagCopyBB);

            cg_.builder_->SetInsertPoint(fTagCopyBB);
            llvm::Value *fNewTags = cg_.emitMalloc(fastLen, "fast.tags.buf");
            llvm::Value *fSrcTagStart = cg_.builder_->CreateGEP(i8TyF, fSrcTags, {start}, "fast.src.tag.start");
            cg_.builder_->CreateCall(memcpyFunc, {fNewTags, fSrcTagStart,
                cg_.builder_->CreateZExtOrTrunc(fastLen, szTy, "fast.tags.sz")});
            cg_.builder_->CreateStore(fNewTags, fTagsGep);
            cg_.builder_->CreateBr(fTagContBB);

            cg_.builder_->SetInsertPoint(fTagNullBB);
            cg_.builder_->CreateStore(llvm::ConstantPointerNull::get(ptrTy), fTagsGep);
            cg_.builder_->CreateBr(fTagContBB);

            cg_.builder_->SetInsertPoint(fTagContBB);
            cg_.builder_->CreateBr(mergeBB);

            // ============================================================
            // (AR) المسار الحلقي: step≠1 → حساب الطول + حلقة نسخ عنصر بعنصر
            //      الطول = ceil((end - start) / step) = (end - start + step - 1) / step
            // (EN) Loop path: step≠1 → compute length + copy loop element by element
            //      Length = ceil((end - start) / step) = (end - start + step - 1) / step
            // ============================================================
            cg_.builder_->SetInsertPoint(loopBB);

            llvm::Value *range = cg_.builder_->CreateSub(end, start, "loop.range");
            llvm::Value *stepM1 = cg_.builder_->CreateSub(step, llvm::ConstantInt::get(i64Ty, 1), "step.m1");
            llvm::Value *rangeAdj = cg_.builder_->CreateAdd(range, stepM1, "loop.range.adj");
            llvm::Value *loopLen = cg_.builder_->CreateSDiv(rangeAdj, step, "loop.len");

            // (AR) الحد الأدنى = 0 (في حال كانت النتيجة سالبة)
            // (EN) Minimum = 0 (in case result is negative)
            llvm::Value *lenIsNeg = cg_.builder_->CreateICmpSLT(loopLen, llvm::ConstantInt::get(i64Ty, 0), "len.neg");
            loopLen = cg_.builder_->CreateSelect(lenIsNeg, llvm::ConstantInt::get(i64Ty, 0), loopLen, "loop.len.safe");

            llvm::Value *loopArr = cg_.emitMalloc(arrSize, "loop.arr");

            llvm::Value *lLenGep = cg_.builder_->CreateStructGEP(arrTy, loopArr, 0, "loop.len.gep");
            cg_.builder_->CreateStore(loopLen, lLenGep);
            llvm::Value *lCapGep = cg_.builder_->CreateStructGEP(arrTy, loopArr, 1, "loop.cap.gep");
            cg_.builder_->CreateStore(loopLen, lCapGep);

            llvm::Value *lDataSize = cg_.builder_->CreateMul(loopLen, llvm::ConstantInt::get(i64Ty, 8), "loop.data.size");
            llvm::Value *lNewData = cg_.emitMalloc(lDataSize, "loop.data");
            llvm::Value *lDataGep = cg_.builder_->CreateStructGEP(arrTy, loopArr, 2, "loop.data.gep");
            cg_.builder_->CreateStore(lNewData, lDataGep);

            // (AR) الحقل ٤ (homogKind): كالمسار السريع — انسخ homogKind المصدر (الشريحة تحفظ النوع).
            // (EN) Field 4 (homogKind): like the fast path — copy the source's homogKind (slice preserves kind).
            {
                llvm::Value *lSrcHkGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 4, "loop.src.homogkind.gep");
                llvm::Value *lSrcHk = cg_.builder_->CreateLoad(cg_.getInt8Type(), lSrcHkGep, "loop.src.homogkind");
                cg_.builder_->CreateStore(lSrcHk, cg_.builder_->CreateStructGEP(arrTy, loopArr, 4, "loop.homogkind.gep"));
            }

            // (AR) [وسم زمن-تشغيل] الحقل ٣: إن كان للمصدر وسوم (مختلطة) خصّص مخزنَ وسومٍ
            //      بطول loopLen ننسخه بالخطوة داخل الحلقة؛ وإلّا اتركه null (متجانسة).
            //      كلّ فهرسٍ في [0, loopLen) يُكتب في الحلقة، فلا حاجة إلى memset.
            // (EN) [runtime tag] field 3: if the source has tags (mixed), allocate a
            //      loopLen tag buffer copied strided inside the loop; else leave null.
            //      Every index in [0, loopLen) is written in the loop, so no memset needed.
            auto i8TyL = cg_.getInt8Type();
            llvm::Value *lSrcTagsGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 3, "loop.src.tags.gep");
            llvm::Value *lSrcTags = cg_.builder_->CreateLoad(ptrTy, lSrcTagsGep, "loop.src.tags");
            llvm::Value *lSrcTagsNull = cg_.builder_->CreateICmpEQ(
                lSrcTags, llvm::ConstantPointerNull::get(ptrTy), "loop.src.tags.null");
            llvm::Value *lTagsFieldGep = cg_.builder_->CreateStructGEP(arrTy, loopArr, 3, "loop.tags.gep");
            llvm::BasicBlock *lTagAllocBB = llvm::BasicBlock::Create(*cg_.context_, "slice.loop.tag.alloc", curFunc);
            llvm::BasicBlock *lTagNullBB = llvm::BasicBlock::Create(*cg_.context_, "slice.loop.tag.null", curFunc);
            llvm::BasicBlock *lTagContBB = llvm::BasicBlock::Create(*cg_.context_, "slice.loop.tag.cont", curFunc);
            cg_.builder_->CreateCondBr(lSrcTagsNull, lTagNullBB, lTagAllocBB);

            cg_.builder_->SetInsertPoint(lTagAllocBB);
            llvm::Value *lNewTags = cg_.emitMalloc(loopLen, "loop.tags.buf");
            cg_.builder_->CreateStore(lNewTags, lTagsFieldGep);
            cg_.builder_->CreateBr(lTagContBB);

            cg_.builder_->SetInsertPoint(lTagNullBB);
            cg_.builder_->CreateStore(llvm::ConstantPointerNull::get(ptrTy), lTagsFieldGep);
            cg_.builder_->CreateBr(lTagContBB);

            cg_.builder_->SetInsertPoint(lTagContBB);
            llvm::PHINode *lTagsPtr = cg_.builder_->CreatePHI(ptrTy, 2, "loop.tags.ptr");
            lTagsPtr->addIncoming(lNewTags, lTagAllocBB);
            lTagsPtr->addIncoming(llvm::ConstantPointerNull::get(ptrTy), lTagNullBB);

            // (AR) تحميل مؤشر بيانات المصدر
            // (EN) Load source data pointer
            llvm::Value *srcDataGep2 = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "src.data.gep2");
            llvm::Value *srcData2 = cg_.builder_->CreateLoad(ptrTy, srcDataGep2, "src.data2");

            // (AR) حلقة النسخ: لكل i في [0, loopLen): newData[i] = srcData[start + i*step]
            // (EN) Copy loop: for i in [0, loopLen): newData[i] = srcData[start + i*step]
            llvm::BasicBlock *loopCondBB = llvm::BasicBlock::Create(*cg_.context_, "slice.loop.cond", curFunc);
            llvm::BasicBlock *loopBodyBB = llvm::BasicBlock::Create(*cg_.context_, "slice.loop.body", curFunc);
            llvm::BasicBlock *loopEndBB = llvm::BasicBlock::Create(*cg_.context_, "slice.loop.end", curFunc);

            cg_.builder_->CreateBr(loopCondBB);

            cg_.builder_->SetInsertPoint(loopCondBB);
            llvm::PHINode *idx = cg_.builder_->CreatePHI(i64Ty, 2, "slice.idx");
            idx->addIncoming(llvm::ConstantInt::get(i64Ty, 0), lTagContBB);
            llvm::Value *loopCond = cg_.builder_->CreateICmpSLT(idx, loopLen, "slice.cond");
            cg_.builder_->CreateCondBr(loopCond, loopBodyBB, loopEndBB);

            cg_.builder_->SetInsertPoint(loopBodyBB);
            // (AR) حساب فهرس المصدر: srcIdx = start + idx * step
            // (EN) Compute source index: srcIdx = start + idx * step
            llvm::Value *offset = cg_.builder_->CreateMul(idx, step, "slice.offset");
            llvm::Value *srcIdx = cg_.builder_->CreateAdd(start, offset, "slice.src.idx");

            // (AR) نسخ العنصر: newData[idx] = srcData[srcIdx]
            // (EN) Copy element: newData[idx] = srcData[srcIdx]
            llvm::Value *srcElemPtr = cg_.builder_->CreateGEP(i64Ty, srcData2, {srcIdx}, "slice.src.elem");
            llvm::Value *elem = cg_.builder_->CreateLoad(i64Ty, srcElemPtr, "slice.elem");
            llvm::Value *dstElemPtr = cg_.builder_->CreateGEP(i64Ty, lNewData, {idx}, "slice.dst.elem");
            cg_.builder_->CreateStore(elem, dstElemPtr);

            // (AR) [وسم زمن-تشغيل] انسخ وسمَ العنصر بالخطوة: newTags[idx] = srcTags[srcIdx]
            //      محروسًا بـlSrcTagsNull (مخزنُ الوسوم الناتج null حين يكون المصدر متجانسًا).
            // (EN) [runtime tag] copy the element's tag strided: newTags[idx] = srcTags[srcIdx]
            //      guarded on lSrcTagsNull (result tags buffer is null when source is homogeneous).
            llvm::BasicBlock *lTagCopyBB = llvm::BasicBlock::Create(*cg_.context_, "slice.loop.body.tagcopy", curFunc);
            llvm::BasicBlock *lTagSkipBB = llvm::BasicBlock::Create(*cg_.context_, "slice.loop.body.tagskip", curFunc);
            cg_.builder_->CreateCondBr(lSrcTagsNull, lTagSkipBB, lTagCopyBB);

            cg_.builder_->SetInsertPoint(lTagCopyBB);
            llvm::Value *lSrcTagPtr = cg_.builder_->CreateGEP(i8TyL, lSrcTags, {srcIdx}, "slice.src.tag");
            llvm::Value *lTagByte = cg_.builder_->CreateLoad(i8TyL, lSrcTagPtr, "slice.tag.byte");
            llvm::Value *lDstTagPtr = cg_.builder_->CreateGEP(i8TyL, lTagsPtr, {idx}, "slice.dst.tag");
            cg_.builder_->CreateStore(lTagByte, lDstTagPtr);
            cg_.builder_->CreateBr(lTagSkipBB);

            cg_.builder_->SetInsertPoint(lTagSkipBB);
            llvm::Value *nextIdx = cg_.builder_->CreateAdd(idx, llvm::ConstantInt::get(i64Ty, 1), "slice.next.idx");
            idx->addIncoming(nextIdx, lTagSkipBB);
            cg_.builder_->CreateBr(loopCondBB);

            cg_.builder_->SetInsertPoint(loopEndBB);
            cg_.builder_->CreateBr(mergeBB);

            // ============================================================
            // (AR) الدمج: PHI للحصول على المصفوفة الناتجة (من المسار السريع أو الحلقي)
            // (EN) Merge: PHI to get result array (from fast path or loop path)
            // ============================================================
            cg_.builder_->SetInsertPoint(mergeBB);
            llvm::PHINode *resultArr = cg_.builder_->CreatePHI(ptrTy, 2, "slice.result");
            resultArr->addIncoming(fastArr, fTagContBB); // (AR) آخر كتلة في المسار السريع بعد نسخ الوسوم
            resultArr->addIncoming(loopArr, loopEndBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = resultArr;
            }
            return resultArr;
        }

        // ============================================================================
        // Phase N: Builtin File I/O Functions / دوال الملفات المضمنة
        // ============================================================================

        // ============================================================================
        // (AR) تم نقل عمليات الملفات والتحويلات إلى llvm_codegen_file_casts.cpp
        //      تم نقل الكوروتينات والمولدات إلى llvm_codegen_coroutines.cpp (CW-05)
        // ============================================================================
    } // namespace LLVM
} // namespace Sad