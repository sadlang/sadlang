// ============================================================================
// llvm_codegen_string_ops2.cpp — عمليات النصوص المتقدمة والمصفوفات
// (AR) تقسيم، ربط، بداية/نهاية، احتواء، إضافة/حذف مصفوفة
// (EN) Split, join, startsWith/endsWith, contains, array append/remove
// تم استخراج هذا الملف من llvm_codegen_string_ops.cpp وفقاً لقاعدة CW-05
// ============================================================================

#include "llvm_codegen.h"
#include "builders/collections/strings_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include "sad_dyn_repr.h" // (AR) تعليب %SadDyn لعناصر المصفوفة المختلطة (الإلحاق) / (EN) %SadDyn boxing for heterogeneous array append
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

        // ============================================================================
        // Phase N: Builtin Array Functions / دوال المصفوفات المضمنة
        // ============================================================================

        llvm::Value *StringsCodeGen::emitBuiltinArrayAppend(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ARRAY_APPEND"}});
                return nullptr;
            }

            llvm::Value *arrPtr = cg_.context_info_.namedValues[inst->operands[0].name];
            if (!arrPtr)
                arrPtr = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *value = cg_.resolveOperand(inst->operands[1]);
            if (!arrPtr || !value)
                return nullptr;

            // (AR) [وسم زمن-تشغيل — الخيار أ الجذريّ] الإلحاقُ لمصفوفةٍ مختلطةٍ قياسيّة
            //      (elementType=Any): بدل تعليب الكومة، نحسب (الوسم، الحمولة) عبر toDyn،
            //      نخزّن الحمولةَ الخام في خانة البيانات، والوسمَ في مخزنِ الوسوم المتوازي
            //      (الحقل ٣) عند الخانة نفسِها — متّسقين مع كتابة emitArraySet الموسومة.
            // (EN) [runtime tag — radical option A] appending to a scalar-heterogeneous array
            //      (elementType=Any): instead of heap boxing, derive (tag, payload) via toDyn,
            //      store the raw payload in the data slot and the tag in the parallel tags
            //      buffer (field 3) at the same slot — consistent with emitArraySet's tagged write.
            bool isDyn = (inst->operands[0].elementType == SadTypeKind::Any);
            llvm::Value *kindByte = nullptr;
            if (isDyn)
            {
                llvm::Value *dyn = toDyn(cg_, value, inst->operands[1].dataType);
                kindByte = dynKindByte(cg_, dyn);
                value = dynPayloadI64(cg_, dyn);
            }

            // (AR) تطبيع مؤشر المصفوفة عبر الدالة الموحّدة
            // (EN) Normalize arrPtr via unified helper
            arrPtr = cg_.normalizeArrayPtr(arrPtr, "append");

            auto i64Ty = cg_.getInt64Type();
            auto i8Ty = cg_.getInt8Type();
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);

            // (AR) [وسم زمن-تشغيل] اضمن مخزنَ الوسوم قبل النموّ (بسعةٍ حاليّة، مُصفَّرًا Null)
            //      كي يُنسَخ محتواه عند النموّ ويُكتَب وسمُ الخانة الجديدة بعده.
            // (EN) [runtime tag] ensure the tags buffer before growth (current capacity, zeroed
            //      Null) so it is copied on grow and the new slot's tag is written afterward.
            llvm::Value *tagsGep0 = nullptr, *curTags = nullptr;
            if (isDyn)
            {
                llvm::Value *capGep0 = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 1, "app.tags.cap0.gep");
                llvm::Value *cap0 = cg_.builder_->CreateLoad(i64Ty, capGep0, "app.tags.cap0");
                tagsGep0 = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 3, "app.tags.gep");
                llvm::Value *t0 = cg_.builder_->CreateLoad(ptrTy, tagsGep0, "app.tags.cur0");
                llvm::Value *isNull0 = cg_.builder_->CreateICmpEQ(
                    t0, llvm::ConstantPointerNull::get(ptrTy), "app.tags.isnull0");
                llvm::Function *fn0 = cg_.builder_->GetInsertBlock()->getParent();
                auto *a0 = llvm::BasicBlock::Create(*cg_.context_, "app.tags.alloc0", fn0);
                auto *c0 = llvm::BasicBlock::Create(*cg_.context_, "app.tags.cont0", fn0);
                llvm::BasicBlock *pre0 = cg_.builder_->GetInsertBlock();
                cg_.builder_->CreateCondBr(isNull0, a0, c0);
                cg_.builder_->SetInsertPoint(a0);
                llvm::Value *nt0 = cg_.emitMalloc(cap0, "app.tags.buf0");
                cg_.builder_->CreateMemSet(nt0, llvm::ConstantInt::get(i8Ty, DynKind::Null), cap0, llvm::MaybeAlign(1));
                cg_.builder_->CreateStore(nt0, tagsGep0);
                llvm::BasicBlock *a0end = cg_.builder_->GetInsertBlock();
                cg_.builder_->CreateBr(c0);
                cg_.builder_->SetInsertPoint(c0);
                llvm::PHINode *tp = cg_.builder_->CreatePHI(ptrTy, 2, "app.tags.ensured");
                tp->addIncoming(t0, pre0);
                tp->addIncoming(nt0, a0end);
                curTags = tp;
            }

            // (AR) تحميل الطول الحالي والسعة ومؤشر البيانات
            // (EN) Load current length, capacity, and data pointer
            llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "app.len.gep");
            llvm::Value *len = cg_.builder_->CreateLoad(i64Ty, lenGep, "app.len");

            llvm::Value *capGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 1, "app.cap.gep");
            llvm::Value *cap = cg_.builder_->CreateLoad(i64Ty, capGep, "app.cap");

            llvm::Value *dataGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "app.data.gep");
            llvm::Value *dataPtr = cg_.builder_->CreateLoad(ptrTy, dataGep, "app.data");

            // (AR) التحقق: هل الطول >= السعة؟ إذا نعم → نحتاج إعادة تخصيص
            // (EN) Check: is length >= capacity? If yes → need reallocation
            llvm::Value *needGrow = cg_.builder_->CreateICmpUGE(len, cap, "app.need.grow");

            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *growBB = llvm::BasicBlock::Create(*cg_.context_, "app.grow", curFunc);
            llvm::BasicBlock *storeBB = llvm::BasicBlock::Create(*cg_.context_, "app.store", curFunc);
            llvm::BasicBlock *entryBB = cg_.builder_->GetInsertBlock();

            cg_.builder_->CreateCondBr(needGrow, growBB, storeBB);

            // === كتلة إعادة التخصيص (grow) ===
            // (AR) مضاعفة السعة + تخصيص مخزن جديد + نسخ البيانات القديمة + تحرير القديم
            // (EN) Double capacity + allocate new buffer + copy old data + free old buffer
            cg_.builder_->SetInsertPoint(growBB);

            llvm::Value *newCap = cg_.builder_->CreateMul(cap, llvm::ConstantInt::get(i64Ty, 2), "app.new.cap");
            // (AR) الحد الأدنى للسعة الجديدة = 8 (في حال كانت السعة القديمة 0)
            // (EN) Minimum new capacity = 8 (in case old capacity is 0)
            llvm::Value *minCap = llvm::ConstantInt::get(i64Ty, 8);
            llvm::Value *capTooSmall = cg_.builder_->CreateICmpULT(newCap, minCap, "app.cap.small");
            newCap = cg_.builder_->CreateSelect(capTooSmall, minCap, newCap, "app.cap.final");

            // (AR) تخصيص مخزن بيانات جديد: السعة_الجديدة × 8 بايت
            // (EN) Allocate new data buffer: new_capacity * 8 bytes
            llvm::Value *newDataSize = cg_.builder_->CreateMul(newCap, llvm::ConstantInt::get(i64Ty, 8), "app.new.size");
            llvm::Value *newData = cg_.emitMalloc(newDataSize, "app.new.data");

            // (AR) نسخ البيانات القديمة: memcpy(newData, oldData, len * 8)
            // (EN) Copy old data: memcpy(newData, oldData, len * 8)
            llvm::Value *copySize = cg_.builder_->CreateMul(len, llvm::ConstantInt::get(i64Ty, 8), "app.copy.size");
            // (AR) طول ‎mem*‎ بنوع ‎size_t‎ الهدف (i32 على 32-بت): الخلفيّة تولّد
            //      النداء المكتبيّ بهذا العرض، وتعريف وقت التشغيل الحرّ يطابقه.
            llvm::Type *szTy = cg_.getSizeType();
            auto *memcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, szTy}, false);
            auto memcpyFunc = cg_.module_->getOrInsertFunction("memcpy", memcpyType);
            cg_.builder_->CreateCall(memcpyFunc, {newData, dataPtr,
                cg_.builder_->CreateZExtOrTrunc(copySize, szTy, "app.copy.size.sz")});

            // (AR) تحرير المخزن القديم
            // (EN) Free old buffer
            cg_.emitFreeCall(dataPtr);

            // (AR) تحديث السعة ومؤشر البيانات في البنية
            // (EN) Update capacity and data pointer in struct
            cg_.builder_->CreateStore(newCap, capGep);
            cg_.builder_->CreateStore(newData, dataGep);

            // (AR) [وسم زمن-تشغيل] نمِّ مخزنَ الوسوم موازيًا للبيانات: خصّص newCap، صفّر Null،
            //      انسخ len وسمًا قديمًا، حرّر القديم، خزّنه. تُحفَظ أوسامُ العناصر السابقة.
            // (EN) [runtime tag] grow the tags buffer in parallel: alloc newCap, zero to Null,
            //      copy len old tags, free old, store. Preserves existing elements' tags.
            llvm::Value *newTags = nullptr;
            if (isDyn)
            {
                newTags = cg_.emitMalloc(newCap, "app.new.tags");
                cg_.builder_->CreateMemSet(newTags, llvm::ConstantInt::get(i8Ty, DynKind::Null), newCap, llvm::MaybeAlign(1));
                llvm::Type *szTy2 = cg_.getSizeType();
                auto *memcpyType2 = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, szTy2}, false);
                auto memcpyFunc2 = cg_.module_->getOrInsertFunction("memcpy", memcpyType2);
                cg_.builder_->CreateCall(memcpyFunc2, {newTags, curTags,
                    cg_.builder_->CreateZExtOrTrunc(len, szTy2, "app.tags.copy.sz")});
                cg_.emitFreeCall(curTags);
                cg_.builder_->CreateStore(newTags, tagsGep0);
            }

            cg_.builder_->CreateBr(storeBB);

            // === كتلة التخزين (store) ===
            // (AR) نستخدم PHI للحصول على مؤشر البيانات الصحيح (القديم أو الجديد)
            // (EN) Use PHI to get correct data pointer (old or new)
            cg_.builder_->SetInsertPoint(storeBB);
            // (AR) كلّ عُقد PHI أوّلًا (يجب أن تتصدّر الكتلة قبل أيّ تعليمة غير-PHI).
            // (EN) All PHI nodes first (must lead the block before any non-PHI instruction).
            llvm::PHINode *finalData = cg_.builder_->CreatePHI(ptrTy, 2, "app.final.data");
            finalData->addIncoming(dataPtr, entryBB); // (AR) لم نغيّر — المخزن القديم
            finalData->addIncoming(newData, growBB);  // (AR) بعد إعادة التخصيص — المخزن الجديد
            llvm::PHINode *finalTags = nullptr;
            if (isDyn)
            {
                finalTags = cg_.builder_->CreatePHI(ptrTy, 2, "app.final.tags");
                finalTags->addIncoming(curTags, entryBB);
                finalTags->addIncoming(newTags, growBB);
            }

            // (AR) تخزين العنصر الجديد في data[len]
            // (EN) Store new element at data[len]
            llvm::Value *elemPtr = cg_.builder_->CreateGEP(i64Ty, finalData, {len}, "app.elem");
            cg_.builder_->CreateStore(value, elemPtr);

            // (AR) [وسم زمن-تشغيل] اكتب وسمَ الخانة الجديدة في المخزن الصحيح (القديم أو المُنمَّى)
            // (EN) [runtime tag] write the new slot's tag into the correct buffer (old or grown)
            if (isDyn)
            {
                llvm::Value *tagPtr = cg_.builder_->CreateGEP(i8Ty, finalTags, {len}, "app.tag.slot");
                cg_.builder_->CreateStore(kindByte, tagPtr);
            }
            else
            {
                // (AR) [homogKind — الحقل ٤] إلحاقٌ ساكن (غير-Any): المصفوفةُ متجانسة ⇒ خزِّن
                //      DynKind المشتقَّ من نوع العنصر (متعادِلٌ: نفس القيمة لكلّ إلحاق). القارئُ
                //      يقرؤه فقط حين tags==null، فلا يُقرأ للمختلطة (لها tags≠null). نظيرُ emitArraySet.
                // (EN) [homogKind — field 4] static (non-Any) append: the array is homogeneous ⇒
                //      store the DynKind derived from the element type (idempotent). The reader
                //      reads it only when tags==null, so it is never read for a mixed array. Inverse of emitArraySet.
                SadTypeKind concreteKind = inst->operands[1].dataType;
                uint8_t k = DynKind::Int;
                switch (concreteKind)
                {
                case SadTypeKind::Float: k = DynKind::Float; break;
                case SadTypeKind::String: case SadTypeKind::Pointer: k = DynKind::Str; break;
                case SadTypeKind::Boolean: k = DynKind::Bool; break;
                case SadTypeKind::Null: k = DynKind::Null; break;
                case SadTypeKind::Array: k = DynKind::Array; break;
                default: k = DynKind::Int; break;
                }
                llvm::Value *hkGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 4, "app.homogkind.gep");
                cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, k), hkGep);
            }

            // (AR) زيادة الطول بمقدار 1
            // (EN) Increment length by 1
            llvm::Value *newLen = cg_.builder_->CreateAdd(len, llvm::ConstantInt::get(i64Ty, 1), "app.new.len");
            cg_.builder_->CreateStore(newLen, lenGep);

            return arrPtr;
        }

        llvm::Value *StringsCodeGen::emitBuiltinArrayRemove(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ARRAY_REMOVE"}});
                return nullptr;
            }

            llvm::Value *arrPtr = cg_.context_info_.namedValues[inst->operands[0].name];
            if (!arrPtr)
                arrPtr = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *removeIdx = cg_.resolveOperand(inst->operands[1]);
            if (!arrPtr || !removeIdx)
                return nullptr;

            // (AR) تطبيع مؤشر المصفوفة عبر الدالة الموحّدة
            // (EN) Normalize arrPtr via unified helper
            arrPtr = cg_.normalizeArrayPtr(arrPtr, "remove");

            // (AR) تطبيع الفهرس: تحويل السالب إلى موجب (مثل احذف(م، -1) = حذف آخر عنصر)
            // (EN) Normalize index: convert negative to positive (e.g. remove(arr, -1) = remove last)
            removeIdx = cg_.normalizeArrayIndex(removeIdx, arrPtr, "rem");

            // (AR) فحص حدود المصفوفة: exit(1) إذا كان الفهرس خارج النطاق
            // (EN) Bounds check: exit(1) if index is out of range
            cg_.emitBoundsCheck(removeIdx, arrPtr, "rem");

            auto i64Ty = cg_.getInt64Type();
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);

            // (AR) تحميل الطول ومؤشر البيانات
            // (EN) Load length and data pointer
            llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "rem.len.gep");
            llvm::Value *len = cg_.builder_->CreateLoad(i64Ty, lenGep, "rem.len");
            llvm::Value *dataGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "rem.data.gep");
            llvm::Value *dataPtr = cg_.builder_->CreateLoad(ptrTy, dataGep, "rem.data");

            // (AR) حلقة إزاحة العناصر: لكل i من removeIdx حتى len-2:
            //      data[i] = data[i+1]
            //      هذا يُزيح كل العناصر بعد الفهرس المحذوف خطوة للخلف
            // (EN) Shift loop: for i from removeIdx to len-2:
            //      data[i] = data[i+1]
            //      This shifts all elements after removed index back by one
            llvm::Value *lastValidIdx = cg_.builder_->CreateSub(len, llvm::ConstantInt::get(i64Ty, 1), "rem.last.idx");

            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *loopCondBB = llvm::BasicBlock::Create(*cg_.context_, "rem.loop.cond", curFunc);
            llvm::BasicBlock *loopBodyBB = llvm::BasicBlock::Create(*cg_.context_, "rem.loop.body", curFunc);
            llvm::BasicBlock *doneBB = llvm::BasicBlock::Create(*cg_.context_, "rem.done", curFunc);
            llvm::BasicBlock *entryBB = cg_.builder_->GetInsertBlock();

            cg_.builder_->CreateBr(loopCondBB);

            cg_.builder_->SetInsertPoint(loopCondBB);
            llvm::PHINode *idx = cg_.builder_->CreatePHI(i64Ty, 2, "rem.idx");
            idx->addIncoming(removeIdx, entryBB);
            // (AR) الشرط: i < len - 1 (آخر عنصر لا يحتاج إزاحة)
            // (EN) Condition: i < len - 1 (last element doesn't need shifting)
            llvm::Value *cond = cg_.builder_->CreateICmpSLT(idx, lastValidIdx, "rem.cond");
            cg_.builder_->CreateCondBr(cond, loopBodyBB, doneBB);

            cg_.builder_->SetInsertPoint(loopBodyBB);
            // (AR) data[i] = data[i+1]
            llvm::Value *nextI = cg_.builder_->CreateAdd(idx, llvm::ConstantInt::get(i64Ty, 1), "rem.next.i");
            llvm::Value *srcPtr = cg_.builder_->CreateGEP(i64Ty, dataPtr, {nextI}, "rem.src");
            llvm::Value *srcVal = cg_.builder_->CreateLoad(i64Ty, srcPtr, "rem.val");
            llvm::Value *dstPtr = cg_.builder_->CreateGEP(i64Ty, dataPtr, {idx}, "rem.dst");
            cg_.builder_->CreateStore(srcVal, dstPtr);

            idx->addIncoming(nextI, loopBodyBB);
            cg_.builder_->CreateBr(loopCondBB);

            // (AR) إنقاص الطول بمقدار 1
            // (EN) Decrement length by 1
            cg_.builder_->SetInsertPoint(doneBB);
            llvm::Value *newLen = cg_.builder_->CreateSub(len, llvm::ConstantInt::get(i64Ty, 1), "rem.new.len");
            cg_.builder_->CreateStore(newLen, lenGep);

            return arrPtr;
        }

    } // namespace LLVM
} // namespace Sad
