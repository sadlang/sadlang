// ============================================================================
// llvm_codegen_string_ops2.cpp — عمليات النصوص المتقدمة والمصفوفات
// (AR) تقسيم، ربط، بداية/نهاية، احتواء، إضافة/حذف مصفوفة
// (EN) Split, join, startsWith/endsWith, contains, array append/remove
// تم استخراج هذا الملف من llvm_codegen_string_ops.cpp وفقاً لقاعدة CW-05
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
#include <iostream>

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {

        // (AR) دالة مساعدة لإنشاء نوع بنية المصفوفة — منسوخة من llvm_codegen_string_ops.cpp
        // (EN) Helper function for array struct type — copied from llvm_codegen_string_ops.cpp
        static llvm::StructType *getArrayStructType(llvm::LLVMContext &ctx)
        {
            static llvm::StructType *arrTy = nullptr;
            if (!arrTy)
            {
                arrTy = llvm::StructType::create(ctx, {
                                                          llvm::Type::getInt64Ty(ctx),      // length
                                                          llvm::Type::getInt64Ty(ctx),      // capacity
                                                          llvm::PointerType::getUnqual(ctx) // data pointer
                                                      },
                                                 "SadArray");
            }
            return arrTy;
        }

        // ============================================================================
        // Phase N: Builtin Array Functions / دوال المصفوفات المضمنة
        // ============================================================================

        llvm::Value *LLVMCodeGen::emitBuiltinArrayAppend(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                reportError("ARRAY_APPEND requires 2 operands (array, value)");
                return nullptr;
            }

            llvm::Value *arrPtr = context_info_.namedValues[inst->operands[0].name];
            if (!arrPtr)
                arrPtr = resolveOperand(inst->operands[0]);
            llvm::Value *value = resolveOperand(inst->operands[1]);
            if (!arrPtr || !value)
                return nullptr;

            // (AR) تطبيع مؤشر المصفوفة عبر الدالة الموحّدة
            // (EN) Normalize arrPtr via unified helper
            arrPtr = normalizeArrayPtr(arrPtr, "append");

            auto i64Ty = getInt64Type();
            auto ptrTy = llvm::PointerType::getUnqual(*context_);
            llvm::StructType *arrTy = getArrayStructType(*context_);

            // (AR) تحميل الطول الحالي والسعة ومؤشر البيانات
            // (EN) Load current length, capacity, and data pointer
            llvm::Value *lenGep = builder_->CreateStructGEP(arrTy, arrPtr, 0, "app.len.gep");
            llvm::Value *len = builder_->CreateLoad(i64Ty, lenGep, "app.len");

            llvm::Value *capGep = builder_->CreateStructGEP(arrTy, arrPtr, 1, "app.cap.gep");
            llvm::Value *cap = builder_->CreateLoad(i64Ty, capGep, "app.cap");

            llvm::Value *dataGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, "app.data.gep");
            llvm::Value *dataPtr = builder_->CreateLoad(ptrTy, dataGep, "app.data");

            // (AR) التحقق: هل الطول >= السعة؟ إذا نعم → نحتاج إعادة تخصيص
            // (EN) Check: is length >= capacity? If yes → need reallocation
            llvm::Value *needGrow = builder_->CreateICmpUGE(len, cap, "app.need.grow");

            llvm::Function *curFunc = builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *growBB = llvm::BasicBlock::Create(*context_, "app.grow", curFunc);
            llvm::BasicBlock *storeBB = llvm::BasicBlock::Create(*context_, "app.store", curFunc);
            llvm::BasicBlock *entryBB = builder_->GetInsertBlock();

            builder_->CreateCondBr(needGrow, growBB, storeBB);

            // === كتلة إعادة التخصيص (grow) ===
            // (AR) مضاعفة السعة + تخصيص مخزن جديد + نسخ البيانات القديمة + تحرير القديم
            // (EN) Double capacity + allocate new buffer + copy old data + free old buffer
            builder_->SetInsertPoint(growBB);

            llvm::Value *newCap = builder_->CreateMul(cap, llvm::ConstantInt::get(i64Ty, 2), "app.new.cap");
            // (AR) الحد الأدنى للسعة الجديدة = 8 (في حال كانت السعة القديمة 0)
            // (EN) Minimum new capacity = 8 (in case old capacity is 0)
            llvm::Value *minCap = llvm::ConstantInt::get(i64Ty, 8);
            llvm::Value *capTooSmall = builder_->CreateICmpULT(newCap, minCap, "app.cap.small");
            newCap = builder_->CreateSelect(capTooSmall, minCap, newCap, "app.cap.final");

            // (AR) تخصيص مخزن بيانات جديد: السعة_الجديدة × 8 بايت
            // (EN) Allocate new data buffer: new_capacity * 8 bytes
            auto *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *newDataSize = builder_->CreateMul(newCap, llvm::ConstantInt::get(i64Ty, 8), "app.new.size");
            llvm::Value *newData = builder_->CreateCall(mallocFunc, {newDataSize}, "app.new.data");

            // (AR) نسخ البيانات القديمة: memcpy(newData, oldData, len * 8)
            // (EN) Copy old data: memcpy(newData, oldData, len * 8)
            llvm::Value *copySize = builder_->CreateMul(len, llvm::ConstantInt::get(i64Ty, 8), "app.copy.size");
            auto *memcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, i64Ty}, false);
            auto memcpyFunc = module_->getOrInsertFunction("memcpy", memcpyType);
            builder_->CreateCall(memcpyFunc, {newData, dataPtr, copySize});

            // (AR) تحرير المخزن القديم
            // (EN) Free old buffer
            auto *freeType = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {ptrTy}, false);
            auto freeFunc = module_->getOrInsertFunction("free", freeType);
            builder_->CreateCall(freeFunc, {dataPtr});

            // (AR) تحديث السعة ومؤشر البيانات في البنية
            // (EN) Update capacity and data pointer in struct
            builder_->CreateStore(newCap, capGep);
            builder_->CreateStore(newData, dataGep);

            builder_->CreateBr(storeBB);

            // === كتلة التخزين (store) ===
            // (AR) نستخدم PHI للحصول على مؤشر البيانات الصحيح (القديم أو الجديد)
            // (EN) Use PHI to get correct data pointer (old or new)
            builder_->SetInsertPoint(storeBB);
            llvm::PHINode *finalData = builder_->CreatePHI(ptrTy, 2, "app.final.data");
            finalData->addIncoming(dataPtr, entryBB); // (AR) لم نغيّر — المخزن القديم
            finalData->addIncoming(newData, growBB);  // (AR) بعد إعادة التخصيص — المخزن الجديد

            // (AR) تخزين العنصر الجديد في data[len]
            // (EN) Store new element at data[len]
            llvm::Value *elemPtr = builder_->CreateGEP(i64Ty, finalData, {len}, "app.elem");
            builder_->CreateStore(value, elemPtr);

            // (AR) زيادة الطول بمقدار 1
            // (EN) Increment length by 1
            llvm::Value *newLen = builder_->CreateAdd(len, llvm::ConstantInt::get(i64Ty, 1), "app.new.len");
            builder_->CreateStore(newLen, lenGep);

            return arrPtr;
        }

        llvm::Value *LLVMCodeGen::emitBuiltinArrayRemove(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                reportError("ARRAY_REMOVE requires 2 operands (array, index)");
                return nullptr;
            }

            llvm::Value *arrPtr = context_info_.namedValues[inst->operands[0].name];
            if (!arrPtr)
                arrPtr = resolveOperand(inst->operands[0]);
            llvm::Value *removeIdx = resolveOperand(inst->operands[1]);
            if (!arrPtr || !removeIdx)
                return nullptr;

            // (AR) تطبيع مؤشر المصفوفة عبر الدالة الموحّدة
            // (EN) Normalize arrPtr via unified helper
            arrPtr = normalizeArrayPtr(arrPtr, "remove");

            // (AR) تطبيع الفهرس: تحويل السالب إلى موجب (مثل احذف(م، -1) = حذف آخر عنصر)
            // (EN) Normalize index: convert negative to positive (e.g. remove(arr, -1) = remove last)
            removeIdx = normalizeArrayIndex(removeIdx, arrPtr, "rem");

            // (AR) فحص حدود المصفوفة: exit(1) إذا كان الفهرس خارج النطاق
            // (EN) Bounds check: exit(1) if index is out of range
            emitBoundsCheck(removeIdx, arrPtr, "rem");

            auto i64Ty = getInt64Type();
            auto ptrTy = llvm::PointerType::getUnqual(*context_);
            llvm::StructType *arrTy = getArrayStructType(*context_);

            // (AR) تحميل الطول ومؤشر البيانات
            // (EN) Load length and data pointer
            llvm::Value *lenGep = builder_->CreateStructGEP(arrTy, arrPtr, 0, "rem.len.gep");
            llvm::Value *len = builder_->CreateLoad(i64Ty, lenGep, "rem.len");
            llvm::Value *dataGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, "rem.data.gep");
            llvm::Value *dataPtr = builder_->CreateLoad(ptrTy, dataGep, "rem.data");

            // (AR) حلقة إزاحة العناصر: لكل i من removeIdx حتى len-2:
            //      data[i] = data[i+1]
            //      هذا يُزيح كل العناصر بعد الفهرس المحذوف خطوة للخلف
            // (EN) Shift loop: for i from removeIdx to len-2:
            //      data[i] = data[i+1]
            //      This shifts all elements after removed index back by one
            llvm::Value *lastValidIdx = builder_->CreateSub(len, llvm::ConstantInt::get(i64Ty, 1), "rem.last.idx");

            llvm::Function *curFunc = builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *loopCondBB = llvm::BasicBlock::Create(*context_, "rem.loop.cond", curFunc);
            llvm::BasicBlock *loopBodyBB = llvm::BasicBlock::Create(*context_, "rem.loop.body", curFunc);
            llvm::BasicBlock *doneBB = llvm::BasicBlock::Create(*context_, "rem.done", curFunc);
            llvm::BasicBlock *entryBB = builder_->GetInsertBlock();

            builder_->CreateBr(loopCondBB);

            builder_->SetInsertPoint(loopCondBB);
            llvm::PHINode *idx = builder_->CreatePHI(i64Ty, 2, "rem.idx");
            idx->addIncoming(removeIdx, entryBB);
            // (AR) الشرط: i < len - 1 (آخر عنصر لا يحتاج إزاحة)
            // (EN) Condition: i < len - 1 (last element doesn't need shifting)
            llvm::Value *cond = builder_->CreateICmpSLT(idx, lastValidIdx, "rem.cond");
            builder_->CreateCondBr(cond, loopBodyBB, doneBB);

            builder_->SetInsertPoint(loopBodyBB);
            // (AR) data[i] = data[i+1]
            llvm::Value *nextI = builder_->CreateAdd(idx, llvm::ConstantInt::get(i64Ty, 1), "rem.next.i");
            llvm::Value *srcPtr = builder_->CreateGEP(i64Ty, dataPtr, {nextI}, "rem.src");
            llvm::Value *srcVal = builder_->CreateLoad(i64Ty, srcPtr, "rem.val");
            llvm::Value *dstPtr = builder_->CreateGEP(i64Ty, dataPtr, {idx}, "rem.dst");
            builder_->CreateStore(srcVal, dstPtr);

            idx->addIncoming(nextI, loopBodyBB);
            builder_->CreateBr(loopCondBB);

            // (AR) إنقاص الطول بمقدار 1
            // (EN) Decrement length by 1
            builder_->SetInsertPoint(doneBB);
            llvm::Value *newLen = builder_->CreateSub(len, llvm::ConstantInt::get(i64Ty, 1), "rem.new.len");
            builder_->CreateStore(newLen, lenGep);

            return arrPtr;
        }

    } // namespace LLVM
} // namespace Sad
