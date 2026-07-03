/*
 * ============================================================================
 * مولد كود LLVM IR - ملف التنفيذ
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "builders/collections/strings_codegen.h"
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
#ifdef _WIN32
#include <filesystem>
#include <windows.h>
#else
#include <cstdio>
#endif

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {

        llvm::Value *StringsCodeGen::emitInlineAsm(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            // (AR) تنفيذ الأسمبلي المدمج — طبقة التراب
            // (EN) Inline assembly implementation — Turab layer
            //
            // الصيغة في SIR:
            //   operands[0] = نص الأسمبلي (STRING constant)
            //   operands[1] = قيود الأسمبلي (constraints, STRING constant) — اختياري
            //   operands[2..N] = المعاملات المدخلة — اختياري
            //   result = المعامل المُخرج — اختياري
            //
            // SIR format:
            //   operands[0] = assembly text (STRING constant)
            //   operands[1] = constraints (STRING constant) — optional
            //   operands[2..N] = input operands — optional
            //   result = output operand — optional

            if (inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_BACKEND_EMIT, {{"detail", "INLINE_ASM"}});
                return nullptr;
            }

            // (AR) الحصول على نص الأسمبلي
            // (EN) Get assembly text
            std::string asmText;
            if (inst->operands[0].type == SIROperandType::CONSTANT &&
                inst->operands[0].dataType == SadTypeKind::String)
            {
                asmText = inst->operands[0].name;
            }
            else
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_BACKEND_EMIT, {{"detail", "INLINE_ASM"}});
                return nullptr;
            }

            // (AR) تحويل صيغة GCC إلى صيغة LLVM للأسمبلي المضمن
            // (EN) Convert GCC-style inline asm syntax to LLVM-style:
            //   GCC: %%reg → LLVM: %reg  (double-percent = literal register name)
            //   GCC: %N   → LLVM: $N    (percent + digit = operand reference)
            {
                std::string converted;
                converted.reserve(asmText.size());
                for (size_t i = 0; i < asmText.size(); i++)
                {
                    if (asmText[i] == '%')
                    {
                        if (i + 1 < asmText.size() && asmText[i + 1] == '%')
                        {
                            // GCC %% → LLVM % (literal percent / register prefix)
                            converted += '%';
                            i++; // skip second %
                        }
                        else if (i + 1 < asmText.size() && asmText[i + 1] >= '0' && asmText[i + 1] <= '9')
                        {
                            // GCC %0, %1, ... → LLVM $0, $1, ...
                            converted += '$';
                            // don't skip digit — it will be added in next iteration
                        }
                        else
                        {
                            // standalone % — keep as-is (already LLVM-style register name)
                            converted += '%';
                        }
                    }
                    else
                    {
                        converted += asmText[i];
                    }
                }
                asmText = converted;
            }

            // (AR) الحصول على قيود الأسمبلي (اختياري)
            // (EN) Get constraints (optional)
            std::string constraints;
            if (inst->operands.size() > 1 &&
                inst->operands[1].type == SIROperandType::CONSTANT &&
                inst->operands[1].dataType == SadTypeKind::String)
            {
                constraints = inst->operands[1].name;
            }

            // (AR) جمع المعاملات المدخلة
            // (EN) Collect input operands
            std::vector<llvm::Value *> inputValues;
            std::vector<llvm::Type *> inputTypes;
            for (size_t i = 2; i < inst->operands.size(); i++)
            {
                llvm::Value *val = cg_.resolveOperand(inst->operands[i]);
                if (val)
                {
                    inputValues.push_back(val);
                    inputTypes.push_back(val->getType());
                }
            }

            // (AR) تحديد نوع الرجوع
            // (EN) Determine return type
            llvm::Type *retType = llvm::Type::getVoidTy(*cg_.context_);
            bool hasResult = inst->result.has_value();
            if (hasResult)
            {
                // (AR) إذا كان هناك نتيجة، نستخدم i64 كنوع افتراضي
                // (EN) If there's a result, use i64 as default type
                retType = llvm::Type::getInt64Ty(*cg_.context_);
            }

            // (AR) بناء نوع الدالة
            // (EN) Build function type
            llvm::FunctionType *asmFuncType = llvm::FunctionType::get(retType, inputTypes, false);

            // (AR) إنشاء الأسمبلي المدمج
            // (EN) Create inline assembly
            bool hasSideEffects = true;
            bool isAlignStack = true;
            llvm::InlineAsm *inlineAsm = llvm::InlineAsm::get(
                asmFuncType, asmText, constraints, hasSideEffects, isAlignStack);

            // (AR) استدعاء الأسمبلي
            // (EN) Call inline assembly
            llvm::Value *result = cg_.builder_->CreateCall(asmFuncType, inlineAsm, inputValues);

            if (hasResult)
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        // ============================================================================
        // (AR) توليد دالة __sad_array_to_string في الموديول
        //      تحوّل مصفوفة SadArray إلى نص مقروء: "[عنصر1, عنصر2, ...]"
        // (EN) Generate __sad_array_to_string function in the module
        //      Converts SadArray to readable string: "[elem1, elem2, ...]"
        // ============================================================================
        void StringsCodeGen::ensureArrayToStringHelper()
        {
            // (AR) إذا الدالة موجودة ولها جسم، لا تُنشئها مرة أخرى
            // (EN) If function exists and has a body, don't recreate
            llvm::Function *existing = cg_.module_->getFunction("__sad_array_to_string");
            if (existing && !existing->empty())
            {
                return;
            }

            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            // Function signature: i8* __sad_array_to_string(i8* buf, i64 len, i8* data)
            llvm::FunctionType *fnTy = llvm::FunctionType::get(ptrTy, {ptrTy, i64Ty, ptrTy}, false);
            llvm::Function *fn = llvm::Function::Create(
                fnTy, llvm::Function::InternalLinkage, "__sad_array_to_string", cg_.module_.get());

            llvm::Argument *bufArg = fn->getArg(0);
            llvm::Argument *lenArg = fn->getArg(1);
            llvm::Argument *dataArg = fn->getArg(2);
            bufArg->setName("buf");
            lenArg->setName("len");
            dataArg->setName("data");

            // Save current insertion point
            llvm::BasicBlock *savedBB = cg_.builder_->GetInsertBlock();
            llvm::BasicBlock::iterator savedPoint = cg_.builder_->GetInsertPoint();

            // Create basic blocks
            llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
            llvm::BasicBlock *loopCheckBB = llvm::BasicBlock::Create(*cg_.context_, "loop.check", fn);
            llvm::BasicBlock *loopBodyBB = llvm::BasicBlock::Create(*cg_.context_, "loop.body", fn);
            llvm::BasicBlock *commaWriteBB = llvm::BasicBlock::Create(*cg_.context_, "comma.write", fn);
            llvm::BasicBlock *elemWriteBB = llvm::BasicBlock::Create(*cg_.context_, "elem.write", fn);
            llvm::BasicBlock *loopEndBB = llvm::BasicBlock::Create(*cg_.context_, "loop.end", fn);

            // Declare sprintf
            llvm::FunctionType *sprintfTy = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, true);
            llvm::FunctionCallee sprintfFn = cg_.module_->getOrInsertFunction("sprintf", sprintfTy);

            // entry: write '[' at buf[0], pos = 1
            cg_.builder_->SetInsertPoint(entryBB);
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, '['), bufArg);
            llvm::Value *initPos = llvm::ConstantInt::get(i64Ty, 1);
            cg_.builder_->CreateBr(loopCheckBB);

            // loop.check: i = phi, pos = phi; if i < len goto body else goto end
            cg_.builder_->SetInsertPoint(loopCheckBB);
            llvm::PHINode *iPhi = cg_.builder_->CreatePHI(i64Ty, 2, "i");
            llvm::PHINode *posPhi = cg_.builder_->CreatePHI(i64Ty, 2, "pos");
            iPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entryBB);
            posPhi->addIncoming(initPos, entryBB);
            llvm::Value *cmp = cg_.builder_->CreateICmpSLT(iPhi, lenArg, "i.lt.len");
            cg_.builder_->CreateCondBr(cmp, loopBodyBB, loopEndBB);

            // loop.body: if i > 0, write ", "
            cg_.builder_->SetInsertPoint(loopBodyBB);
            llvm::Value *needComma = cg_.builder_->CreateICmpSGT(iPhi, llvm::ConstantInt::get(i64Ty, 0), "need.comma");
            cg_.builder_->CreateCondBr(needComma, commaWriteBB, elemWriteBB);

            // comma.write: write ", " at buf+pos
            cg_.builder_->SetInsertPoint(commaWriteBB);
            llvm::Value *commaFmt = cg_.builder_->CreateGlobalStringPtr(", ", "comma.fmt");
            llvm::Value *commaPos = cg_.builder_->CreateGEP(i8Ty, bufArg, posPhi, "comma.ptr");
            llvm::Value *commaLen = cg_.builder_->CreateCall(sprintfFn, {commaPos, commaFmt}, "comma.len");
            llvm::Value *commaLen64 = cg_.builder_->CreateSExt(commaLen, i64Ty, "comma.len64");
            llvm::Value *posAfterComma = cg_.builder_->CreateAdd(posPhi, commaLen64, "pos.after.comma");
            cg_.builder_->CreateBr(elemWriteBB);

            // elem.write: load element, sprintf it, advance pos
            cg_.builder_->SetInsertPoint(elemWriteBB);
            llvm::PHINode *elemPosPhi = cg_.builder_->CreatePHI(i64Ty, 2, "elem.pos");
            elemPosPhi->addIncoming(posPhi, loopBodyBB);
            elemPosPhi->addIncoming(posAfterComma, commaWriteBB);

            // Load element as i64 (all array elements stored as i64 or ptr-sized values)
            llvm::Value *elemGep = cg_.builder_->CreateGEP(i64Ty, dataArg, iPhi, "elem.gep");
            llvm::Value *elemVal = cg_.builder_->CreateLoad(i64Ty, elemGep, "elem.val");

            // sprintf(buf+pos, "%lld", elem)
            llvm::Value *elemFmt = cg_.builder_->CreateGlobalStringPtr("%lld", "elem.fmt");
            llvm::Value *elemDst = cg_.builder_->CreateGEP(i8Ty, bufArg, elemPosPhi, "elem.dst");
            llvm::Value *elemLen = cg_.builder_->CreateCall(sprintfFn, {elemDst, elemFmt, elemVal}, "elem.len");
            llvm::Value *elemLen64 = cg_.builder_->CreateSExt(elemLen, i64Ty, "elem.len64");
            llvm::Value *newPos = cg_.builder_->CreateAdd(elemPosPhi, elemLen64, "new.pos");

            // i++
            llvm::Value *nextI = cg_.builder_->CreateAdd(iPhi, llvm::ConstantInt::get(i64Ty, 1), "next.i");

            // Back to loop check
            iPhi->addIncoming(nextI, elemWriteBB);
            posPhi->addIncoming(newPos, elemWriteBB);
            cg_.builder_->CreateBr(loopCheckBB);

            // loop.end: write ']' and '\0'
            cg_.builder_->SetInsertPoint(loopEndBB);
            llvm::Value *closeBracketPtr = cg_.builder_->CreateGEP(i8Ty, bufArg, posPhi, "close.ptr");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, ']'), closeBracketPtr);
            llvm::Value *endPos = cg_.builder_->CreateAdd(posPhi, llvm::ConstantInt::get(i64Ty, 1), "end.pos");
            llvm::Value *nullPtr = cg_.builder_->CreateGEP(i8Ty, bufArg, endPos, "null.ptr");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0), nullPtr);
            cg_.builder_->CreateRet(bufArg);

            // Restore insertion point
            if (savedBB)
            {
                cg_.builder_->SetInsertPoint(savedBB, savedPoint);
            }
        }

        // ================================================================
        // (AR) نظير نصّيّ: i8* __sad_array_to_string_str(i64 len, i8* data)
        //      يبني «[س0، س1، ...]» حيث كلّ عنصر مؤشّر نصّ (i64 في data[]).
        //      تمريرتان: (1) حساب الحجم بـ strlen، (2) الملء بـ sprintf("%s").
        //      يخصّص المخزن داخليًّا (طول متغيّر) ويُعيده — المستدعي يحرّره.
        // (EN) String variant of the array-to-string helper. Two passes: size via
        //      strlen, then fill via sprintf("%s"). Mallocs its own buffer (variable
        //      length) and returns it — the caller frees it.
        // ================================================================
        void StringsCodeGen::ensureArrayToStringStrHelper()
        {
            llvm::Function *existing = cg_.module_->getFunction("__sad_array_to_string_str");
            if (existing && !existing->empty())
                return;

            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            // i8* __sad_array_to_string_str(i64 len, i8* data)
            llvm::FunctionType *fnTy = llvm::FunctionType::get(ptrTy, {i64Ty, ptrTy}, false);
            llvm::Function *fn = llvm::Function::Create(
                fnTy, llvm::Function::InternalLinkage, "__sad_array_to_string_str", cg_.module_.get());
            llvm::Argument *lenArg = fn->getArg(0);
            llvm::Argument *dataArg = fn->getArg(1);
            lenArg->setName("len");
            dataArg->setName("data");

            llvm::BasicBlock *savedBB = cg_.builder_->GetInsertBlock();
            llvm::BasicBlock::iterator savedPoint = cg_.builder_->GetInsertPoint();

            // (AR) دوال C المساعدة / (EN) C helper declarations
            llvm::FunctionCallee strlenFn = cg_.module_->getOrInsertFunction(
                "strlen", llvm::FunctionType::get(i64Ty, {ptrTy}, false));
            llvm::FunctionCallee mallocFn = cg_.module_->getOrInsertFunction(
                "malloc", llvm::FunctionType::get(ptrTy, {i64Ty}, false));
            llvm::FunctionCallee sprintfFn = cg_.module_->getOrInsertFunction(
                "sprintf", llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, true));

            llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
            llvm::BasicBlock *sizeChkBB = llvm::BasicBlock::Create(*cg_.context_, "size.check", fn);
            llvm::BasicBlock *sizeBodyBB = llvm::BasicBlock::Create(*cg_.context_, "size.body", fn);
            llvm::BasicBlock *allocBB = llvm::BasicBlock::Create(*cg_.context_, "alloc", fn);
            llvm::BasicBlock *fillChkBB = llvm::BasicBlock::Create(*cg_.context_, "fill.check", fn);
            llvm::BasicBlock *fillBodyBB = llvm::BasicBlock::Create(*cg_.context_, "fill.body", fn);
            llvm::BasicBlock *commaBB = llvm::BasicBlock::Create(*cg_.context_, "fill.comma", fn);
            llvm::BasicBlock *writeBB = llvm::BasicBlock::Create(*cg_.context_, "fill.write", fn);
            llvm::BasicBlock *endBB = llvm::BasicBlock::Create(*cg_.context_, "fill.end", fn);

            // (AR) helper: يحمّل عنصر i (مؤشّر نصّ) من data[i]. / (EN) load elem i as char*.
            auto loadElemPtr = [&](llvm::Value *idx, const char *nm) -> llvm::Value *
            {
                llvm::Value *gep = cg_.builder_->CreateGEP(i64Ty, dataArg, idx, "s2s.elem.gep");
                llvm::Value *i64v = cg_.builder_->CreateLoad(i64Ty, gep, "s2s.elem.i64");
                return cg_.builder_->CreateIntToPtr(i64v, ptrTy, nm);
            };

            // entry → size.check
            cg_.builder_->SetInsertPoint(entryBB);
            cg_.builder_->CreateBr(sizeChkBB);

            // ── تمريرة 1: الحجم = 3 (قوسان + '\0') + مجموع (strlen + 2) ──
            cg_.builder_->SetInsertPoint(sizeChkBB);
            llvm::PHINode *si = cg_.builder_->CreatePHI(i64Ty, 2, "s.i");
            llvm::PHINode *sAcc = cg_.builder_->CreatePHI(i64Ty, 2, "s.acc");
            si->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entryBB);
            sAcc->addIncoming(llvm::ConstantInt::get(i64Ty, 3), entryBB);
            llvm::Value *sCmp = cg_.builder_->CreateICmpSLT(si, lenArg, "s.i.lt");
            cg_.builder_->CreateCondBr(sCmp, sizeBodyBB, allocBB);

            cg_.builder_->SetInsertPoint(sizeBodyBB);
            llvm::Value *sElem = loadElemPtr(si, "s2s.size.ptr");
            llvm::Value *sLen = cg_.builder_->CreateCall(strlenFn, {sElem}, "s2s.strlen");
            llvm::Value *sAdd = cg_.builder_->CreateAdd(
                sAcc, cg_.builder_->CreateAdd(sLen, llvm::ConstantInt::get(i64Ty, 2)), "s.acc.next");
            llvm::Value *sNext = cg_.builder_->CreateAdd(si, llvm::ConstantInt::get(i64Ty, 1), "s.i.next");
            si->addIncoming(sNext, sizeBodyBB);
            sAcc->addIncoming(sAdd, sizeBodyBB);
            cg_.builder_->CreateBr(sizeChkBB);

            // ── malloc + كتابة '[' ──
            cg_.builder_->SetInsertPoint(allocBB);
            llvm::Value *buf = cg_.builder_->CreateCall(mallocFn, {sAcc}, "s2s.buf");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, '['), buf);
            cg_.builder_->CreateBr(fillChkBB);

            // ── تمريرة 2: الملء ──
            cg_.builder_->SetInsertPoint(fillChkBB);
            llvm::PHINode *fi = cg_.builder_->CreatePHI(i64Ty, 2, "f.i");
            llvm::PHINode *fPos = cg_.builder_->CreatePHI(i64Ty, 2, "f.pos");
            fi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), allocBB);
            fPos->addIncoming(llvm::ConstantInt::get(i64Ty, 1), allocBB); // بعد '['
            llvm::Value *fCmp = cg_.builder_->CreateICmpSLT(fi, lenArg, "f.i.lt");
            cg_.builder_->CreateCondBr(fCmp, fillBodyBB, endBB);

            cg_.builder_->SetInsertPoint(fillBodyBB);
            llvm::Value *needComma = cg_.builder_->CreateICmpSGT(fi, llvm::ConstantInt::get(i64Ty, 0), "f.need.comma");
            cg_.builder_->CreateCondBr(needComma, commaBB, writeBB);

            // ", " قبل العناصر التالية
            cg_.builder_->SetInsertPoint(commaBB);
            llvm::Value *commaFmt = cg_.builder_->CreateGlobalStringPtr(", ", "s2s.comma");
            llvm::Value *commaDst = cg_.builder_->CreateGEP(i8Ty, buf, fPos, "s2s.comma.dst");
            llvm::Value *commaN = cg_.builder_->CreateCall(sprintfFn, {commaDst, commaFmt}, "s2s.comma.n");
            llvm::Value *posAfterComma = cg_.builder_->CreateAdd(
                fPos, cg_.builder_->CreateSExt(commaN, i64Ty), "f.pos.comma");
            cg_.builder_->CreateBr(writeBB);

            // sprintf(buf+pos, "%s", elem)
            cg_.builder_->SetInsertPoint(writeBB);
            llvm::PHINode *wPos = cg_.builder_->CreatePHI(i64Ty, 2, "w.pos");
            wPos->addIncoming(fPos, fillBodyBB);
            wPos->addIncoming(posAfterComma, commaBB);
            llvm::Value *wElem = loadElemPtr(fi, "s2s.write.ptr");
            llvm::Value *sFmt = cg_.builder_->CreateGlobalStringPtr("%s", "s2s.sfmt");
            llvm::Value *wDst = cg_.builder_->CreateGEP(i8Ty, buf, wPos, "s2s.write.dst");
            llvm::Value *wN = cg_.builder_->CreateCall(sprintfFn, {wDst, sFmt, wElem}, "s2s.write.n");
            llvm::Value *wNewPos = cg_.builder_->CreateAdd(wPos, cg_.builder_->CreateSExt(wN, i64Ty), "w.pos.next");
            llvm::Value *fNext = cg_.builder_->CreateAdd(fi, llvm::ConstantInt::get(i64Ty, 1), "f.i.next");
            fi->addIncoming(fNext, writeBB);
            fPos->addIncoming(wNewPos, writeBB);
            cg_.builder_->CreateBr(fillChkBB);

            // ── الخاتمة: ']' + '\0' ──
            cg_.builder_->SetInsertPoint(endBB);
            llvm::Value *closePtr = cg_.builder_->CreateGEP(i8Ty, buf, fPos, "s2s.close");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, ']'), closePtr);
            llvm::Value *nullPos = cg_.builder_->CreateAdd(fPos, llvm::ConstantInt::get(i64Ty, 1), "s2s.nullpos");
            llvm::Value *nullPtr2 = cg_.builder_->CreateGEP(i8Ty, buf, nullPos, "s2s.nullptr");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0), nullPtr2);
            cg_.builder_->CreateRet(buf);

            if (savedBB)
                cg_.builder_->SetInsertPoint(savedBB, savedPoint);
        }

        // ================================================================
        // (AR) i8* __sad_map_to_string(i8* map)
        //      يبني «{"م0": ق0، "م1": ق1، …}» من خريطة {count,capacity,keys*,values*,types*}.
        //      المفاتيح مقتبسة (نصوص دائمًا)؛ القيم حسب وسم النوع (0=نص %s، 1=رقم %lld،
        //      3=منطقيّ صحيح/خطأ، غيرها ⇒ %lld). يمرّ على الخانات غير الفارغة (مفتاح≠null).
        //      تمريرتان (حجم ثمّ ملء) عبر alloca للعدّادات؛ يخصّص مخزنه ويُعيده — المستدعي يحرّره.
        // (EN) Map-to-string helper mirroring the interpreter's format: quoted keys, values by
        //      type tag. Two passes (size then fill) using allocas for counters. Mallocs and returns.
        // ================================================================
        void StringsCodeGen::ensureMapToStringHelper()
        {
            llvm::Function *existing = cg_.module_->getFunction("__sad_map_to_string");
            if (existing && !existing->empty())
                return;

            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            llvm::FunctionType *fnTy = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
            llvm::Function *fn = llvm::Function::Create(
                fnTy, llvm::Function::InternalLinkage, "__sad_map_to_string", cg_.module_.get());
            llvm::Argument *mapArg = fn->getArg(0);
            mapArg->setName("map");

            llvm::BasicBlock *savedBB = cg_.builder_->GetInsertBlock();
            llvm::BasicBlock::iterator savedPoint = cg_.builder_->GetInsertPoint();

            llvm::FunctionCallee strlenFn = cg_.module_->getOrInsertFunction(
                "strlen", llvm::FunctionType::get(i64Ty, {ptrTy}, false));
            llvm::FunctionCallee mallocFn = cg_.module_->getOrInsertFunction(
                "malloc", llvm::FunctionType::get(ptrTy, {i64Ty}, false));
            llvm::FunctionCallee sprintfFn = cg_.module_->getOrInsertFunction(
                "sprintf", llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, true));

            auto &B = *cg_.builder_;
            auto C0 = [&](int64_t v)
            { return llvm::ConstantInt::get(i64Ty, v); };
            auto C8 = [&](char c)
            { return llvm::ConstantInt::get(i8Ty, c); };

            // كتل / blocks
            llvm::BasicBlock *entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
            llvm::BasicBlock *szChk = llvm::BasicBlock::Create(*cg_.context_, "sz.check", fn);
            llvm::BasicBlock *szBody = llvm::BasicBlock::Create(*cg_.context_, "sz.body", fn);
            llvm::BasicBlock *szAdd = llvm::BasicBlock::Create(*cg_.context_, "sz.add", fn);
            llvm::BasicBlock *szStr = llvm::BasicBlock::Create(*cg_.context_, "sz.str", fn);
            llvm::BasicBlock *szFixed = llvm::BasicBlock::Create(*cg_.context_, "sz.fixed", fn);
            llvm::BasicBlock *szNext = llvm::BasicBlock::Create(*cg_.context_, "sz.next", fn);
            llvm::BasicBlock *doAlloc = llvm::BasicBlock::Create(*cg_.context_, "do.alloc", fn);
            llvm::BasicBlock *flChk = llvm::BasicBlock::Create(*cg_.context_, "fl.check", fn);
            llvm::BasicBlock *flBody = llvm::BasicBlock::Create(*cg_.context_, "fl.body", fn);
            llvm::BasicBlock *flEntry = llvm::BasicBlock::Create(*cg_.context_, "fl.entry", fn);
            llvm::BasicBlock *flSep = llvm::BasicBlock::Create(*cg_.context_, "fl.sep", fn);
            llvm::BasicBlock *flKey = llvm::BasicBlock::Create(*cg_.context_, "fl.key", fn);
            llvm::BasicBlock *flInt = llvm::BasicBlock::Create(*cg_.context_, "fl.int", fn);
            llvm::BasicBlock *flBoolChk = llvm::BasicBlock::Create(*cg_.context_, "fl.bool.chk", fn);
            llvm::BasicBlock *flBool = llvm::BasicBlock::Create(*cg_.context_, "fl.bool", fn);
            llvm::BasicBlock *flStrChk = llvm::BasicBlock::Create(*cg_.context_, "fl.str.chk", fn);
            llvm::BasicBlock *flStr = llvm::BasicBlock::Create(*cg_.context_, "fl.str", fn);
            llvm::BasicBlock *flNext = llvm::BasicBlock::Create(*cg_.context_, "fl.next", fn);
            llvm::BasicBlock *flEnd = llvm::BasicBlock::Create(*cg_.context_, "fl.end", fn);

            // ── entry: تحميل الحقول + alloca العدّادات ──
            B.SetInsertPoint(entry);
            auto loadField = [&](int idx, const char *nm)
            {
                llvm::Value *gep = B.CreateGEP(i64Ty, mapArg, C0(idx), nm);
                return B.CreateLoad(i64Ty, gep, nm);
            };
            llvm::Value *cap = loadField(1, "m2s.cap");
            llvm::Value *keysArr = B.CreateIntToPtr(loadField(2, "m2s.keys.i64"), ptrTy, "m2s.keys");
            llvm::Value *valsArr = B.CreateIntToPtr(loadField(3, "m2s.vals.i64"), ptrTy, "m2s.vals");
            llvm::Value *typesArr = B.CreateIntToPtr(loadField(4, "m2s.types.i64"), ptrTy, "m2s.types");

            llvm::Value *accA = B.CreateAlloca(i64Ty, nullptr, "acc");
            llvm::Value *siA = B.CreateAlloca(i64Ty, nullptr, "si");
            llvm::Value *posA = B.CreateAlloca(i64Ty, nullptr, "pos");
            llvm::Value *firstA = B.CreateAlloca(i64Ty, nullptr, "first");
            llvm::Value *fiA = B.CreateAlloca(i64Ty, nullptr, "fi");
            B.CreateStore(C0(3), accA); // '{' + '}' + '\0'
            B.CreateStore(C0(0), siA);
            B.CreateBr(szChk);

            // helper: يحمّل مؤشّر مفتاح/قيمة من مصفوفة i64 عند فهرس
            auto loadPtrElem = [&](llvm::Value *arr, llvm::Value *idx, const char *nm)
            {
                llvm::Value *gep = B.CreateGEP(i64Ty, arr, idx, "m2s.gep");
                llvm::Value *i64v = B.CreateLoad(i64Ty, gep, "m2s.i64");
                return B.CreateIntToPtr(i64v, ptrTy, nm);
            };
            auto loadI64Elem = [&](llvm::Value *arr, llvm::Value *idx, const char *nm)
            {
                llvm::Value *gep = B.CreateGEP(i64Ty, arr, idx, "m2s.gep");
                return B.CreateLoad(i64Ty, gep, nm);
            };
            llvm::Value *nullP = llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy));

            // ── تمريرة 1: الحجم ──
            B.SetInsertPoint(szChk);
            llvm::Value *si = B.CreateLoad(i64Ty, siA, "si.v");
            B.CreateCondBr(B.CreateICmpSLT(si, cap, "si.lt"), szBody, doAlloc);

            B.SetInsertPoint(szBody);
            llvm::Value *szKey = loadPtrElem(keysArr, si, "sz.key");
            B.CreateCondBr(B.CreateICmpNE(szKey, nullP, "sz.key.nn"), szAdd, szNext);

            B.SetInsertPoint(szAdd);
            // acc += strlen(key) + 6  ("": ", ")
            llvm::Value *szKeyLen = B.CreateCall(strlenFn, {szKey}, "sz.klen");
            llvm::Value *acc1 = B.CreateAdd(B.CreateLoad(i64Ty, accA, "acc.v"),
                                            B.CreateAdd(szKeyLen, C0(6)), "acc.k");
            B.CreateStore(acc1, accA);
            llvm::Value *szType = loadI64Elem(typesArr, si, "sz.type");
            B.CreateCondBr(B.CreateICmpEQ(szType, C0(0), "sz.is.str"), szStr, szFixed);

            B.SetInsertPoint(szStr);
            llvm::Value *szVal = loadPtrElem(valsArr, si, "sz.val");
            llvm::Value *szVLen = B.CreateCall(strlenFn, {szVal}, "sz.vlen");
            B.CreateStore(B.CreateAdd(B.CreateLoad(i64Ty, accA, "acc.v2"), szVLen, "acc.s"), accA);
            B.CreateBr(szNext);

            B.SetInsertPoint(szFixed);
            // رقم/منطقيّ: احجز 24 بايت (يكفي i64 وأطول اسم منطقيّ) / int/bool: reserve 24
            B.CreateStore(B.CreateAdd(B.CreateLoad(i64Ty, accA, "acc.v3"), C0(24), "acc.f"), accA);
            B.CreateBr(szNext);

            B.SetInsertPoint(szNext);
            B.CreateStore(B.CreateAdd(si, C0(1), "si.inc"), siA);
            B.CreateBr(szChk);

            // ── malloc + '{' ──
            B.SetInsertPoint(doAlloc);
            llvm::Value *buf = B.CreateCall(mallocFn, {B.CreateLoad(i64Ty, accA, "acc.final")}, "m2s.buf");
            B.CreateStore(C8('{'), buf);
            B.CreateStore(C0(1), posA);
            B.CreateStore(C0(1), firstA); // 1 = ما زلنا في أوّل مدخل
            B.CreateStore(C0(0), fiA);
            B.CreateBr(flChk);

            // مساعِد كتابة: sprintf(buf+pos, fmt[, arg]) ثمّ pos += n
            auto writeFmt = [&](llvm::Value *fmt, llvm::Value *arg)
            {
                llvm::Value *pos = B.CreateLoad(i64Ty, posA, "pos.v");
                llvm::Value *dst = B.CreateGEP(i8Ty, buf, pos, "wr.dst");
                llvm::Value *n = arg ? B.CreateCall(sprintfFn, {dst, fmt, arg}, "wr.n")
                                     : B.CreateCall(sprintfFn, {dst, fmt}, "wr.n");
                B.CreateStore(B.CreateAdd(pos, B.CreateSExt(n, i64Ty), "pos.adv"), posA);
            };
            auto writeChar = [&](char c)
            {
                llvm::Value *pos = B.CreateLoad(i64Ty, posA, "pos.c");
                B.CreateStore(C8(c), B.CreateGEP(i8Ty, buf, pos, "wr.c.dst"));
                B.CreateStore(B.CreateAdd(pos, C0(1), "pos.c.inc"), posA);
            };
            llvm::Value *fmtS = B.CreateGlobalStringPtr("%s", "m2s.fmt.s");
            llvm::Value *fmtD = B.CreateGlobalStringPtr("%lld", "m2s.fmt.d");
            llvm::Value *fmtSep = B.CreateGlobalStringPtr(", ", "m2s.fmt.sep");
            llvm::Value *strTrue = B.CreateGlobalStringPtr("\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "m2s.true");  // صحيح
            llvm::Value *strFalse = B.CreateGlobalStringPtr("\xd8\xae\xd8\xb7\xd8\xa3", "m2s.false");        // خطأ

            // ── تمريرة 2: الملء ──
            B.SetInsertPoint(flChk);
            llvm::Value *fi = B.CreateLoad(i64Ty, fiA, "fi.v");
            B.CreateCondBr(B.CreateICmpSLT(fi, cap, "fi.lt"), flBody, flEnd);

            B.SetInsertPoint(flBody);
            llvm::Value *flKeyPtr = loadPtrElem(keysArr, fi, "fl.key.ptr");
            B.CreateCondBr(B.CreateICmpNE(flKeyPtr, nullP, "fl.key.nn"), flEntry, flNext);

            B.SetInsertPoint(flEntry);
            llvm::Value *isFirst = B.CreateICmpNE(B.CreateLoad(i64Ty, firstA, "first.v"), C0(0), "is.first");
            B.CreateCondBr(isFirst, flKey, flSep);

            B.SetInsertPoint(flSep);
            writeFmt(fmtSep, nullptr); // ", "
            B.CreateBr(flKey);

            B.SetInsertPoint(flKey);
            B.CreateStore(C0(0), firstA);
            writeChar('"');
            writeFmt(fmtS, flKeyPtr); // اسم المفتاح
            writeChar('"');
            writeChar(':');
            writeChar(' ');
            llvm::Value *flType = loadI64Elem(typesArr, fi, "fl.type");
            B.CreateCondBr(B.CreateICmpEQ(flType, C0(1), "fl.is.int"), flInt, flBoolChk);

            B.SetInsertPoint(flInt);
            writeFmt(fmtD, loadI64Elem(valsArr, fi, "fl.int.val"));
            B.CreateBr(flNext);

            B.SetInsertPoint(flBoolChk);
            B.CreateCondBr(B.CreateICmpEQ(flType, C0(3), "fl.is.bool"), flBool, flStrChk);

            B.SetInsertPoint(flBool);
            llvm::Value *bv = B.CreateICmpNE(loadI64Elem(valsArr, fi, "fl.bool.val"), C0(0), "fl.bool.nz");
            writeFmt(fmtS, B.CreateSelect(bv, strTrue, strFalse, "fl.bool.str"));
            B.CreateBr(flNext);

            // (AR) نصّ **فقط** حين الوسم=0 (مؤشّر char* صالح)؛ أيّ وسم آخر (2 عشريّ/≥4)
            //      يُعامَل عدديًّا بـ%lld — مطابقةً لتمريرة الحجم (غير الوسم0 ⇒ 24 بايت).
            //      يتفادى تفسير قيمة i64 غير مؤشّرة كـchar* (انهيار/تجاوز — ملاحظة Amelia CRITICAL).
            // (EN) String path **only** when tag==0 (a valid char*); any other tag (2 float/≥4) is
            //      formatted numerically via %lld — matching the size pass (non-0 ⇒ 24 bytes). Avoids
            //      dereferencing a non-pointer i64 as char* (crash/overflow — Amelia CRITICAL note).
            B.SetInsertPoint(flStrChk);
            B.CreateCondBr(B.CreateICmpEQ(flType, C0(0), "fl.is.str"), flStr, flInt);

            B.SetInsertPoint(flStr);
            writeFmt(fmtS, loadPtrElem(valsArr, fi, "fl.str.val"));
            B.CreateBr(flNext);

            B.SetInsertPoint(flNext);
            B.CreateStore(B.CreateAdd(fi, C0(1), "fi.inc"), fiA);
            B.CreateBr(flChk);

            // ── الخاتمة: '}' + '\0' ──
            B.SetInsertPoint(flEnd);
            llvm::Value *endPos = B.CreateLoad(i64Ty, posA, "pos.end");
            B.CreateStore(C8('}'), B.CreateGEP(i8Ty, buf, endPos, "cl.dst"));
            B.CreateStore(C8(0), B.CreateGEP(i8Ty, buf, B.CreateAdd(endPos, C0(1), "null.pos"), "nl.dst"));
            B.CreateRet(buf);

            if (savedBB)
                B.SetInsertPoint(savedBB, savedPoint);
        }
    } // namespace LLVM
} // namespace Sad
