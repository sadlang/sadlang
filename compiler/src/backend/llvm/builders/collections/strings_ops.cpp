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
#include "sad_dyn_repr.h" // (AR) dynToString/unbox لطباعة المصفوفة المختلطة / (EN) heterogeneous array print
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

            // (AR) لهجة التجميع (م١): نصّ مسبوق بـSOH يعني «صيغة LLVM جاهزة»
            //      (‎$$‎ للثابت، ‎$N‎ للمعامل، ‎%reg‎ للسجلّ) ⇒ نتخطّى تحويل GCC —
            //      الثابت المشترك kRawLlvmAsmMarker في sir_constants.h (عقد الطبقتين).
            // (EN) Assembly dialect (M1): a SOH-prefixed text is already LLVM-style —
            //      skip the GCC->LLVM conversion below (shared kRawLlvmAsmMarker).
            bool rawLlvmAsm =
                (!asmText.empty() && asmText[0] == ::Sad::Compiler::kRawLlvmAsmMarker);
            if (rawLlvmAsm)
            {
                asmText.erase(asmText.begin());
            }

            // (AR) تحويل صيغة GCC إلى صيغة LLVM للأسمبلي المضمن
            // (EN) Convert GCC-style inline asm syntax to LLVM-style:
            //   GCC: %%reg → LLVM: %reg  (double-percent = literal register name)
            //   GCC: %N   → LLVM: $N    (percent + digit = operand reference)
            if (!rawLlvmAsm)
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
            llvm::Value *sLen = cg_.emitStrlen(sElem, "s2s.strlen");
            llvm::Value *sAdd = cg_.builder_->CreateAdd(
                sAcc, cg_.builder_->CreateAdd(sLen, llvm::ConstantInt::get(i64Ty, 2)), "s.acc.next");
            llvm::Value *sNext = cg_.builder_->CreateAdd(si, llvm::ConstantInt::get(i64Ty, 1), "s.i.next");
            si->addIncoming(sNext, sizeBodyBB);
            sAcc->addIncoming(sAdd, sizeBodyBB);
            cg_.builder_->CreateBr(sizeChkBB);

            // ── malloc + كتابة '[' ──
            cg_.builder_->SetInsertPoint(allocBB);
            llvm::Value *buf = cg_.emitMalloc(sAcc, "s2s.buf");
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
        // (AR) نظير عشريّ: i8* __sad_array_to_string_float(i64 len, i8* data)
        //      كلّ خانة i64 تحمل بتّات double (bitcast(double→i64) عند التخزين، ISSUE-082).
        //      يبني «[س0, س1, ...]»: لكلّ عنصر bitcast(i64→double) ثمّ __sad_format_double
        //      (أو __sad_ftoa حرًّا) ⇒ يطابق تمثيل المفسّر، ثمّ strlen لتقدّم الموضع.
        //      يخصّص مخزنه (طول×ميزانيّة + قوسان) ويُعيده — المستدعي يحرّره.
        // (EN) Float variant of array-to-string. Each i64 slot holds double bits
        //      (bitcast at store, ISSUE-082). Builds "[x0, x1, ...]": per element
        //      bitcast(i64→double) then __sad_format_double (or __sad_ftoa freestanding),
        //      then strlen to advance. Mallocs its own buffer and returns it — caller frees.
        // ================================================================
        void StringsCodeGen::ensureArrayToStringFloatHelper()
        {
            llvm::Function *existing = cg_.module_->getFunction("__sad_array_to_string_float");
            if (existing && !existing->empty())
                return;

            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            auto dblTy = llvm::Type::getDoubleTy(*cg_.context_);
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            // (AR) ميزانيّة كلّ عنصر: %.6f لـDBL_MAX ~317 حرفًا + «, » ⇒ 340 آمنة.
            // (EN) per-element budget: DBL_MAX %.6f ~317 chars + ", " ⇒ 340 is safe.
            constexpr int64_t kFloatElemBudget = 340;
            constexpr int64_t kBracketsAndNull = 4; // '[' ']' '\0' + هامش

            llvm::FunctionType *fnTy = llvm::FunctionType::get(ptrTy, {i64Ty, ptrTy}, false);
            llvm::Function *fn = llvm::Function::Create(
                fnTy, llvm::Function::InternalLinkage, "__sad_array_to_string_float", cg_.module_.get());
            llvm::Argument *lenArg = fn->getArg(0);
            llvm::Argument *dataArg = fn->getArg(1);
            lenArg->setName("len");
            dataArg->setName("data");

            llvm::BasicBlock *savedBB = cg_.builder_->GetInsertBlock();
            llvm::BasicBlock::iterator savedPoint = cg_.builder_->GetInsertPoint();

            llvm::FunctionCallee sprintfFn = cg_.module_->getOrInsertFunction(
                "sprintf", llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, true));
            // (AR) منسّق العشريّ: حرًّا __sad_ftoa؛ وإلّا __sad_format_double (نفس منسّق القياسيّ)
            // (EN) float formatter: __sad_ftoa freestanding, else __sad_format_double (scalar's own)
            llvm::FunctionCallee fmtFn = cg_.freestanding_
                ? cg_.module_->getOrInsertFunction(
                      "__sad_ftoa", llvm::FunctionType::get(i32Ty, {ptrTy, dblTy}, false))
                : cg_.module_->getOrInsertFunction(
                      "__sad_format_double",
                      llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {ptrTy, dblTy}, false));

            llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
            llvm::BasicBlock *chkBB = llvm::BasicBlock::Create(*cg_.context_, "loop.check", fn);
            llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(*cg_.context_, "loop.body", fn);
            llvm::BasicBlock *commaBB = llvm::BasicBlock::Create(*cg_.context_, "comma.write", fn);
            llvm::BasicBlock *elemBB = llvm::BasicBlock::Create(*cg_.context_, "elem.write", fn);
            llvm::BasicBlock *endBB = llvm::BasicBlock::Create(*cg_.context_, "loop.end", fn);

            // (AR) entry: buf = malloc(len*budget + 4)؛ buf[0]='[' / (EN) alloc + open bracket
            cg_.builder_->SetInsertPoint(entryBB);
            llvm::Value *bufSz = cg_.builder_->CreateAdd(
                cg_.builder_->CreateMul(lenArg, llvm::ConstantInt::get(i64Ty, kFloatElemBudget)),
                llvm::ConstantInt::get(i64Ty, kBracketsAndNull), "f2s.bufsz");
            llvm::Value *buf = cg_.emitMalloc(bufSz, "f2s.buf");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, '['), buf);
            cg_.builder_->CreateBr(chkBB);

            // (AR) loop.check: i<len ? / (EN)
            cg_.builder_->SetInsertPoint(chkBB);
            llvm::PHINode *iPhi = cg_.builder_->CreatePHI(i64Ty, 2, "i");
            llvm::PHINode *posPhi = cg_.builder_->CreatePHI(i64Ty, 2, "pos");
            iPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entryBB);
            posPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 1), entryBB);
            llvm::Value *cmp = cg_.builder_->CreateICmpSLT(iPhi, lenArg, "i.lt.len");
            cg_.builder_->CreateCondBr(cmp, bodyBB, endBB);

            // (AR) loop.body: فاصلة قبل غير الأوّل / (EN) comma before non-first
            cg_.builder_->SetInsertPoint(bodyBB);
            llvm::Value *needComma = cg_.builder_->CreateICmpSGT(iPhi, llvm::ConstantInt::get(i64Ty, 0), "need.comma");
            cg_.builder_->CreateCondBr(needComma, commaBB, elemBB);

            cg_.builder_->SetInsertPoint(commaBB);
            llvm::Value *commaFmt = cg_.builder_->CreateGlobalStringPtr(", ", "f2s.comma");
            llvm::Value *commaDst = cg_.builder_->CreateGEP(i8Ty, buf, posPhi, "f2s.comma.dst");
            llvm::Value *commaN = cg_.builder_->CreateCall(sprintfFn, {commaDst, commaFmt}, "f2s.comma.n");
            llvm::Value *posAfterComma = cg_.builder_->CreateAdd(
                posPhi, cg_.builder_->CreateSExt(commaN, i64Ty), "pos.after.comma");
            cg_.builder_->CreateBr(elemBB);

            // (AR) elem.write: bitcast(slot)→double ⇒ منسّق ⇒ strlen لتقدّم / (EN)
            cg_.builder_->SetInsertPoint(elemBB);
            llvm::PHINode *elemPos = cg_.builder_->CreatePHI(i64Ty, 2, "elem.pos");
            elemPos->addIncoming(posPhi, bodyBB);
            elemPos->addIncoming(posAfterComma, commaBB);
            llvm::Value *elemGep = cg_.builder_->CreateGEP(i64Ty, dataArg, iPhi, "f2s.elem.gep");
            llvm::Value *elemI64 = cg_.builder_->CreateLoad(i64Ty, elemGep, "f2s.elem.i64");
            llvm::Value *elemD = cg_.builder_->CreateBitCast(elemI64, dblTy, "f2s.elem.d");
            llvm::Value *elemDst = cg_.builder_->CreateGEP(i8Ty, buf, elemPos, "f2s.elem.dst");
            cg_.builder_->CreateCall(fmtFn, {elemDst, elemD});
            llvm::Value *wrote = cg_.emitStrlen(elemDst, "f2s.elem.len");
            llvm::Value *newPos = cg_.builder_->CreateAdd(elemPos, wrote, "f2s.new.pos");
            llvm::Value *nextI = cg_.builder_->CreateAdd(iPhi, llvm::ConstantInt::get(i64Ty, 1), "next.i");
            iPhi->addIncoming(nextI, elemBB);
            posPhi->addIncoming(newPos, elemBB);
            cg_.builder_->CreateBr(chkBB);

            // (AR) loop.end: ']' ثمّ '\0' / (EN) close bracket + null
            cg_.builder_->SetInsertPoint(endBB);
            llvm::Value *closePtr = cg_.builder_->CreateGEP(i8Ty, buf, posPhi, "f2s.close");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, ']'), closePtr);
            llvm::Value *nullPos = cg_.builder_->CreateAdd(posPhi, llvm::ConstantInt::get(i64Ty, 1), "f2s.nullpos");
            llvm::Value *nullP = cg_.builder_->CreateGEP(i8Ty, buf, nullPos, "f2s.nullptr");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0), nullP);
            cg_.builder_->CreateRet(buf);

            if (savedBB)
                cg_.builder_->SetInsertPoint(savedBB, savedPoint);
        }

        // ================================================================
        // (AR) i8* __sad_array_to_string_dyn(i64 len, i8* data)
        //      [عناصر موسومة — option A] يبني «[ع0، ع1، ...]» لمصفوفةٍ مختلطةٍ قياسيّة
        //      كلّ خانةٍ فيها مؤشّرُ صندوق %SadDyn. يفكّ كلّ عنصرٍ عبر dynToString (نظير
        //      المفسّر عنصرًا-عنصرًا). تمريرتان: (١) قياسُ مجموع الأطوال، (٢) الملء —
        //      كلاهما يستدعي dynToString (نتائج التمريرة الأولى تُسرَّب، مقبول). يخصّص مخزنه.
        // (EN) [boxed elements — option A] builds "[e0, e1, ...]" for a scalar-heterogeneous
        //      array whose slots are %SadDyn box pointers; decodes each element via
        //      dynToString. Two passes: (1) sum the lengths, (2) fill — both call dynToString
        //      (pass-1 strings leak, acceptable). Mallocs its own buffer and returns it.
        // ================================================================
        void StringsCodeGen::ensureArrayToStringDynHelper()
        {
            llvm::Function *existing = cg_.module_->getFunction("__sad_array_to_string_dyn");
            if (existing && !existing->empty())
                return;

            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            llvm::FunctionType *fnTy = llvm::FunctionType::get(ptrTy, {i64Ty, ptrTy, ptrTy}, false);
            llvm::Function *fn = llvm::Function::Create(
                fnTy, llvm::Function::InternalLinkage, "__sad_array_to_string_dyn", cg_.module_.get());
            llvm::Argument *lenArg = fn->getArg(0);
            llvm::Argument *dataArg = fn->getArg(1);
            llvm::Argument *tagsArg = fn->getArg(2);
            lenArg->setName("len");
            dataArg->setName("data");
            tagsArg->setName("tags");

            llvm::BasicBlock *savedBB = cg_.builder_->GetInsertBlock();
            llvm::BasicBlock::iterator savedPoint = cg_.builder_->GetInsertPoint();

            llvm::FunctionCallee sprintfFn = cg_.module_->getOrInsertFunction(
                "sprintf", llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, true));

            // (AR) [وسم زمن-تشغيل] مساعِدٌ يحمّل الحمولةَ الخام i64 من الخانة i، ووسمَ DynKind
            //      من tags[i] (أو Int إن كانت tags=null احتياطيًّا)، يبني %SadDyn عبر makeDyn،
            //      ثمّ يفكّه نصًّا عبر dynToString. نظيرُ كتابة emitArraySet الموسومة.
            // (EN) [runtime tag] helper: load the raw i64 payload from slot i and the DynKind
            //      from tags[i] (or Int if tags==null, defensive), assemble %SadDyn via makeDyn,
            //      then dynToString. Inverse of emitArraySet's tagged write.
            auto elemToStr = [&](llvm::Value *idx) -> llvm::Value * {
                llvm::Value *slot = cg_.builder_->CreateGEP(i64Ty, dataArg, idx, "d2s.slot");
                llvm::Value *payload = cg_.builder_->CreateLoad(i64Ty, slot, "d2s.payload");
                llvm::Value *tagsNull = cg_.builder_->CreateICmpEQ(
                    tagsArg, llvm::ConstantPointerNull::get(ptrTy), "d2s.tags.isnull");
                llvm::Value *tagPtr = cg_.builder_->CreateGEP(i8Ty, tagsArg, idx, "d2s.tag.slot");
                // (AR) حمّل الوسمَ فقط عند وجود المخزن (select على العنوان يمنع تحميلًا من null)
                // (EN) load the tag only when the buffer exists (address-select avoids a null load)
                llvm::Value *safePtr = cg_.builder_->CreateSelect(tagsNull, dataArg, tagPtr, "d2s.tag.safe");
                llvm::Value *rawTag = cg_.builder_->CreateLoad(i8Ty, safePtr, "d2s.tag.raw");
                llvm::Value *tag = cg_.builder_->CreateSelect(
                    tagsNull, llvm::ConstantInt::get(i8Ty, DynKind::Int), rawTag, "d2s.tag");
                llvm::Value *dyn = makeDyn(cg_, tag, payload);
                return dynToString(cg_, dyn);
            };

            llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
            llvm::BasicBlock *p1chk = llvm::BasicBlock::Create(*cg_.context_, "p1.check", fn);
            llvm::BasicBlock *p1body = llvm::BasicBlock::Create(*cg_.context_, "p1.body", fn);
            llvm::BasicBlock *allocBB = llvm::BasicBlock::Create(*cg_.context_, "alloc", fn);
            llvm::BasicBlock *p2chk = llvm::BasicBlock::Create(*cg_.context_, "p2.check", fn);
            llvm::BasicBlock *p2body = llvm::BasicBlock::Create(*cg_.context_, "p2.body", fn);
            llvm::BasicBlock *p2comma = llvm::BasicBlock::Create(*cg_.context_, "p2.comma", fn);
            llvm::BasicBlock *p2elem = llvm::BasicBlock::Create(*cg_.context_, "p2.elem", fn);
            llvm::BasicBlock *p2end = llvm::BasicBlock::Create(*cg_.context_, "p2.end", fn);

            // (AR) entry: br pass1 / (EN)
            cg_.builder_->SetInsertPoint(entryBB);
            cg_.builder_->CreateBr(p1chk);

            // === PASS 1: قياس المجموع (total يبدأ 3: '[' ']' '\0') ===
            cg_.builder_->SetInsertPoint(p1chk);
            llvm::PHINode *i1 = cg_.builder_->CreatePHI(i64Ty, 2, "i1");
            llvm::PHINode *total = cg_.builder_->CreatePHI(i64Ty, 2, "total");
            i1->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entryBB);
            // (AR) البذرة = طولُ القوسين + بايتُ الإنهاء، محسوبةً من ثوابتِ SoT لا مكتوبةً ٣.
            // (EN) Seed = both brackets + the terminating byte, derived from the SoT constants
            //      rather than a written 3.
            const int64_t kArrayFixedBytes =
                static_cast<int64_t>(::Sad::Types::repr::kArrayOpen.size() +
                                     ::Sad::Types::repr::kArrayClose.size() + 1);
            const int64_t kArraySepBytes = static_cast<int64_t>(::Sad::Types::repr::kArrayElemSep.size());
            total->addIncoming(llvm::ConstantInt::get(i64Ty, kArrayFixedBytes), entryBB);
            llvm::Value *c1 = cg_.builder_->CreateICmpSLT(i1, lenArg, "p1.lt");
            cg_.builder_->CreateCondBr(c1, p1body, allocBB);

            cg_.builder_->SetInsertPoint(p1body);
            llvm::Value *s1 = elemToStr(i1);
            llvm::Value *n1 = cg_.emitStrlen(s1, "p1.len");
            llvm::Value *commaAdd = cg_.builder_->CreateSelect(
                cg_.builder_->CreateICmpSGT(i1, llvm::ConstantInt::get(i64Ty, 0), "p1.gt0"),
                llvm::ConstantInt::get(i64Ty, kArraySepBytes), llvm::ConstantInt::get(i64Ty, 0), "p1.comma");
            llvm::Value *newTotal = cg_.builder_->CreateAdd(
                cg_.builder_->CreateAdd(total, n1, "p1.t1"), commaAdd, "p1.t2");
            llvm::Value *nextI1 = cg_.builder_->CreateAdd(i1, llvm::ConstantInt::get(i64Ty, 1), "p1.next");
            // (AR) dynToString قد يشقّ الكتلة (يولّد فروعًا)، فالمُسبِقُ الفعليّ للعودة هو
            //      الكتلةُ الحاليّة لا p1body الثابتة — نستعملها في PHI تجنّبًا لعدم المطابقة.
            // (EN) dynToString may split the block (emits branches), so the real loop-back
            //      predecessor is the CURRENT block, not the fixed p1body — use it in the PHI.
            llvm::BasicBlock *p1bodyEnd = cg_.builder_->GetInsertBlock();
            i1->addIncoming(nextI1, p1bodyEnd);
            total->addIncoming(newTotal, p1bodyEnd);
            cg_.builder_->CreateBr(p1chk);

            // === alloc: buf = malloc(total)؛ buf[0]='[' ===
            cg_.builder_->SetInsertPoint(allocBB);
            llvm::Value *buf = cg_.emitMalloc(total, "d2s.buf");
            // (AR) المحدِّداتُ من مصدرِ الحقيقة (value_repr.yaml) لا حرفيّاتٍ خام — نفسُ الثوابتِ
            //      التي يستعملها المفسّرُ في Value::toString، فيستحيل انجرافُ العرضين.
            // (EN) Delimiters come from the SoT (value_repr.yaml), not raw literals — the same
            //      constants the interpreter uses in Value::toString, so the two cannot drift.
            cg_.builder_->CreateStore(
                llvm::ConstantInt::get(i8Ty, static_cast<uint8_t>(::Sad::Types::repr::kArrayOpen[0])), buf);
            cg_.builder_->CreateBr(p2chk);

            // === PASS 2: الملء ===
            cg_.builder_->SetInsertPoint(p2chk);
            llvm::PHINode *i2 = cg_.builder_->CreatePHI(i64Ty, 2, "i2");
            llvm::PHINode *pos = cg_.builder_->CreatePHI(i64Ty, 2, "pos");
            i2->addIncoming(llvm::ConstantInt::get(i64Ty, 0), allocBB);
            pos->addIncoming(llvm::ConstantInt::get(i64Ty, 1), allocBB);
            llvm::Value *c2 = cg_.builder_->CreateICmpSLT(i2, lenArg, "p2.lt");
            cg_.builder_->CreateCondBr(c2, p2body, p2end);

            cg_.builder_->SetInsertPoint(p2body);
            llvm::Value *needComma = cg_.builder_->CreateICmpSGT(i2, llvm::ConstantInt::get(i64Ty, 0), "p2.gt0");
            cg_.builder_->CreateCondBr(needComma, p2comma, p2elem);

            cg_.builder_->SetInsertPoint(p2comma);
            llvm::Value *commaFmt =
                cg_.builder_->CreateGlobalStringPtr(::Sad::Types::repr::kArrayElemSep, "d2s.comma");
            llvm::Value *commaDst = cg_.builder_->CreateGEP(i8Ty, buf, pos, "d2s.comma.dst");
            cg_.builder_->CreateCall(sprintfFn, {commaDst, commaFmt});
            // (AR) التقدّمُ بطولِ الفاصلِ نفسِه لا بثابتٍ مكتوب — لو غُيّر الفاصلُ في SoT تبعه الحساب.
            // (EN) Advance by the separator's own length, not a written constant — changing the
            //      separator in the SoT keeps the arithmetic correct.
            llvm::Value *posAfterComma = cg_.builder_->CreateAdd(
                pos,
                llvm::ConstantInt::get(i64Ty, static_cast<int64_t>(::Sad::Types::repr::kArrayElemSep.size())),
                "d2s.pos.comma");
            cg_.builder_->CreateBr(p2elem);

            cg_.builder_->SetInsertPoint(p2elem);
            llvm::PHINode *elemPos = cg_.builder_->CreatePHI(i64Ty, 2, "elem.pos");
            elemPos->addIncoming(pos, p2body);
            elemPos->addIncoming(posAfterComma, p2comma);
            llvm::Value *s2 = elemToStr(i2);
            llvm::Value *sFmt = cg_.builder_->CreateGlobalStringPtr("%s", "d2s.sfmt");
            llvm::Value *elemDst = cg_.builder_->CreateGEP(i8Ty, buf, elemPos, "d2s.elem.dst");
            cg_.builder_->CreateCall(sprintfFn, {elemDst, sFmt, s2});
            llvm::Value *wrote = cg_.emitStrlen(elemDst, "d2s.elem.len");
            llvm::Value *newPos = cg_.builder_->CreateAdd(elemPos, wrote, "d2s.new.pos");
            llvm::Value *nextI2 = cg_.builder_->CreateAdd(i2, llvm::ConstantInt::get(i64Ty, 1), "p2.next");
            // (AR) نفس علّة الشقّ: المُسبِقُ الفعليّ هو الكتلةُ الحاليّة بعد elemToStr.
            // (EN) Same split concern: the real predecessor is the current block after elemToStr.
            llvm::BasicBlock *p2elemEnd = cg_.builder_->GetInsertBlock();
            i2->addIncoming(nextI2, p2elemEnd);
            pos->addIncoming(newPos, p2elemEnd);
            cg_.builder_->CreateBr(p2chk);

            // === p2end: ']' ثمّ '\0' ===
            cg_.builder_->SetInsertPoint(p2end);
            llvm::Value *closePtr = cg_.builder_->CreateGEP(i8Ty, buf, pos, "d2s.close");
            cg_.builder_->CreateStore(
                llvm::ConstantInt::get(i8Ty, static_cast<uint8_t>(::Sad::Types::repr::kArrayClose[0])), closePtr);
            llvm::Value *nullPos = cg_.builder_->CreateAdd(pos, llvm::ConstantInt::get(i64Ty, 1), "d2s.nullpos");
            llvm::Value *nullP = cg_.builder_->CreateGEP(i8Ty, buf, nullPos, "d2s.nullptr");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0), nullP);
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
        void StringsCodeGen::ensureMapToStringHelper(bool quoteKeys)
        {
            // (AR) اسمانِ لنسختَين: المقتبسةُ للطباعةِ وغيرُ المقتبسةِ لـ`نص(خريطة)`
            //      وخاصّيّةِ عنصرِ الواجهة — الفرقُ حرفانِ اثنان لا خوارزميّةٌ ثانية.
            const char *const helperName =
                quoteKeys ? ::Sad::Compiler::kMapToStringQuotedFn
                          : ::Sad::Compiler::kMapToStringPlainFn;
            llvm::Function *existing = cg_.module_->getFunction(helperName);
            if (existing && !existing->empty())
                return;

            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            llvm::FunctionType *fnTy = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
            llvm::Function *fn = llvm::Function::Create(
                fnTy, llvm::Function::InternalLinkage, helperName, cg_.module_.get());
            llvm::Argument *mapArg = fn->getArg(0);
            mapArg->setName("map");

            llvm::BasicBlock *savedBB = cg_.builder_->GetInsertBlock();
            llvm::BasicBlock::iterator savedPoint = cg_.builder_->GetInsertPoint();

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
            // (AR) [م-٠٠١] الحاويةُ لا يحدُّها رقمٌ ثابت: يُنسَّقُ المتداخلُ في تمريرةِ
            //      الحجمِ نفسِها ويُضافُ طولُه الحقيقيّ. حجزٌ ثابتٌ هنا كان يعني تجاوزَ
            //      مخزنٍ عندَ أوّلِ خريطةٍ متداخلةٍ كبيرة.
            // (EN) [card م-٠٠١] A container has no fixed bound: the nested value is formatted in
            //      the size pass itself and its real length added. A fixed reservation here would
            //      mean a buffer overrun on the first large nested map.
            llvm::BasicBlock *szContainerChk =
                llvm::BasicBlock::Create(*cg_.context_, "sz.cont.chk", fn);
            llvm::BasicBlock *szContainerMap =
                llvm::BasicBlock::Create(*cg_.context_, "sz.cont.map", fn);
            llvm::BasicBlock *szContainerArrChk =
                llvm::BasicBlock::Create(*cg_.context_, "sz.cont.arr.chk", fn);
            llvm::BasicBlock *szContainerArr =
                llvm::BasicBlock::Create(*cg_.context_, "sz.cont.arr", fn);
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
            // (AR) [م-٠٠١] ذراعا الوسمَين الجديدَين: العشريُّ (٢) والعدمُ (٤/فراغ ٥).
            //      كانا يسقطانِ في ذراعِ الصحيحِ فتُطبَعُ بتّاتُ الـdouble ومؤشّرُ العدمِ
            //      أعدادًا فلكيّة: `{"س": 4609434218613702656}` بدل `{"س": 1.5}`.
            // (EN) [card م-٠٠١] Arms for the two new tags: float (2) and null (4 / void 5). They
            //      used to fall into the integer arm, printing the double's bits and the null
            //      payload as astronomical integers: `{"س": 4609434218613702656}` for `1.5`.
            llvm::BasicBlock *flFloatChk = llvm::BasicBlock::Create(*cg_.context_, "fl.float.chk", fn);
            llvm::BasicBlock *flFloat = llvm::BasicBlock::Create(*cg_.context_, "fl.float", fn);
            llvm::BasicBlock *flNullChk = llvm::BasicBlock::Create(*cg_.context_, "fl.null.chk", fn);
            llvm::BasicBlock *flNull = llvm::BasicBlock::Create(*cg_.context_, "fl.null", fn);
            llvm::BasicBlock *flContainerChk = llvm::BasicBlock::Create(*cg_.context_, "fl.cont.chk", fn);
            llvm::BasicBlock *flMap = llvm::BasicBlock::Create(*cg_.context_, "fl.map", fn);
            llvm::BasicBlock *flArrayChk = llvm::BasicBlock::Create(*cg_.context_, "fl.arr.chk", fn);
            llvm::BasicBlock *flArray = llvm::BasicBlock::Create(*cg_.context_, "fl.array", fn);
            llvm::BasicBlock *flStr = llvm::BasicBlock::Create(*cg_.context_, "fl.str", fn);
            llvm::BasicBlock *flNext = llvm::BasicBlock::Create(*cg_.context_, "fl.next", fn);
            llvm::BasicBlock *flEnd = llvm::BasicBlock::Create(*cg_.context_, "fl.end", fn);

            // ════════════════════════════════════════════════════════════════
            // (AR) **حارسُ المؤشّرِ العدم — أوّلَ تعليمةٍ لا بعدَها.** الجسمُ يفكُّ
            //   `map` فورًا لقراءةِ السَّعةِ والمصفوفات، فخريطةٌ غيرُ مُهيّأةٍ أو دالّةٌ
            //   مصرَّحةُ الإرجاعِ `خريطة` تُرجِعُ `لاشيء` ⇒ **SIGSEGV** بينما المفسّرُ
            //   يطبعُ «لاشيء». وذراعُ الخريطةِ في `dynToString` تملكُ هذا الحارسَ
            //   أصلًا (dyn.ts.map.null) — فأُخِذَتِ الصيغةُ منها وأُسقِطَ حارسُها.
            //   ويعودُ **مخزنٌ مُخصَّصٌ طازج** لا ثابتًا عامًّا: عقدُ الدالّةِ أنّ
            //   الناتجَ يملكُه المُنادي ويُحرِّرُه، وإعادةُ ثابتٍ هنا تجعلُ `free`
            //   عندَ المستهلِكِ انهيارًا ثانيًا.
            // (EN) Null-pointer guard as the FIRST instruction: the body dereferences
            //   `map` immediately, so an uninitialised map (or a function declared to
            //   return خريطة returning لاشيء) meant SIGSEGV while the interpreter
            //   prints «لاشيء». dynToString's map arm already has this guard — the
            //   spelling was copied from it and the guard was dropped. Returns a FRESH
            //   malloc'd buffer, never a global constant, so the caller's free stays valid.
            // ════════════════════════════════════════════════════════════════
            llvm::BasicBlock *nullArg = llvm::BasicBlock::Create(*cg_.context_, "m2s.null", fn);
            llvm::BasicBlock *liveArg = llvm::BasicBlock::Create(*cg_.context_, "m2s.live", fn);
            B.SetInsertPoint(entry);
            B.CreateCondBr(
                B.CreateICmpEQ(mapArg,
                               llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)),
                               "m2s.isnull"),
                nullArg, liveArg);

            B.SetInsertPoint(nullArg);
            {
                // (AR) نصُّ العرضِ من مصدرِ الحقيقةِ الموحَّد لا حرفيّةً خامّة.
                llvm::Value *nullText =
                    B.CreateGlobalStringPtr(::Sad::Types::repr::kNullDisplay, "m2s.nulltext");
                const size_t nullTextSize = ::Sad::Types::repr::kNullDisplay.size() + 1;
                llvm::Value *nullBuf = cg_.emitMalloc(C0(static_cast<int64_t>(nullTextSize)), "m2s.nullbuf");
                B.CreateCall(sprintfFn,
                             {nullBuf, B.CreateGlobalStringPtr("%s", "m2s.nullfmt"), nullText});
                B.CreateRet(nullBuf);
            }

            // ── entry: تحميل الحقول + alloca العدّادات ──
            B.SetInsertPoint(liveArg);
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

            // (AR) نداءُ `__sad_array_to_string_dyn` يأخذ (طول، بيانات، أوسام) لا مؤشّرَ
            //      البنية، فنستخرجُ الحقولَ الثلاثةَ من ترويسةِ SadArray.
            // (EN) `__sad_array_to_string_dyn` takes (len, data, tags), not the struct pointer,
            //      so the three fields are extracted from the SadArray header.
            auto callArrayToString = [&](llvm::Value *arrValPtr, const char *nm) -> llvm::Value * {
                cg_.ensureArrayToStringDynHelper();
                llvm::Value *lenV = B.CreateLoad(
                    i64Ty, B.CreateGEP(i64Ty, arrValPtr, C0(0), "a2s.len.gep"), "a2s.len");
                llvm::Value *dataV = B.CreateIntToPtr(
                    B.CreateLoad(i64Ty, B.CreateGEP(i64Ty, arrValPtr, C0(2), "a2s.data.gep"),
                                 "a2s.data.i64"),
                    ptrTy, "a2s.data");
                llvm::Value *tagsV = B.CreateIntToPtr(
                    B.CreateLoad(i64Ty, B.CreateGEP(i64Ty, arrValPtr, C0(3), "a2s.tags.gep"),
                                 "a2s.tags.i64"),
                    ptrTy, "a2s.tags");
                auto helperFn = cg_.module_->getOrInsertFunction(
                    "__sad_array_to_string_dyn",
                    llvm::FunctionType::get(ptrTy, {i64Ty, ptrTy, ptrTy}, false));
                return B.CreateCall(helperFn, {lenV, dataV, tagsV}, nm);
            };

            // ── تمريرة 1: الحجم ──
            B.SetInsertPoint(szChk);
            llvm::Value *si = B.CreateLoad(i64Ty, siA, "si.v");
            B.CreateCondBr(B.CreateICmpSLT(si, cap, "si.lt"), szBody, doAlloc);

            B.SetInsertPoint(szBody);
            llvm::Value *szKey = loadPtrElem(keysArr, si, "sz.key");
            B.CreateCondBr(B.CreateICmpNE(szKey, nullP, "sz.key.nn"), szAdd, szNext);

            B.SetInsertPoint(szAdd);
            // acc += strlen(key) + 6  ("": ", ")
            llvm::Value *szKeyLen = cg_.emitStrlen(szKey, "sz.klen");
            llvm::Value *acc1 = B.CreateAdd(B.CreateLoad(i64Ty, accA, "acc.v"),
                                            B.CreateAdd(szKeyLen, C0(6)), "acc.k");
            B.CreateStore(acc1, accA);
            llvm::Value *szType = loadI64Elem(typesArr, si, "sz.type");
            B.CreateCondBr(B.CreateICmpEQ(szType, C0(Sad::Compiler::kMapValueTagString),
                                          "sz.is.str"),
                           szStr, szContainerChk);

            B.SetInsertPoint(szStr);
            llvm::Value *szVal = loadPtrElem(valsArr, si, "sz.val");
            llvm::Value *szVLen = cg_.emitStrlen(szVal, "sz.vlen");
            B.CreateStore(B.CreateAdd(B.CreateLoad(i64Ty, accA, "acc.v2"), szVLen, "acc.s"), accA);
            B.CreateBr(szNext);

            B.SetInsertPoint(szContainerChk);
            B.CreateCondBr(B.CreateICmpEQ(szType, C0(Sad::Compiler::kMapValueTagMap), "sz.is.map"),
                           szContainerMap, szContainerArrChk);

            B.SetInsertPoint(szContainerMap);
            {
                llvm::Value *nested = B.CreateCall(fn, {loadPtrElem(valsArr, si, "sz.map.val")},
                                                   "sz.map.str");
                B.CreateStore(B.CreateAdd(B.CreateLoad(i64Ty, accA, "acc.vm"),
                                          cg_.emitStrlen(nested, "sz.map.len"), "acc.m"),
                              accA);
            }
            B.CreateBr(szNext);

            B.SetInsertPoint(szContainerArrChk);
            B.CreateCondBr(B.CreateICmpEQ(szType, C0(Sad::Compiler::kMapValueTagArray),
                                          "sz.is.array"),
                           szContainerArr, szFixed);

            B.SetInsertPoint(szContainerArr);
            {
                llvm::Value *nested =
                    callArrayToString(loadPtrElem(valsArr, si, "sz.arr.val"), "sz.arr.str");
                B.CreateStore(B.CreateAdd(B.CreateLoad(i64Ty, accA, "acc.va"),
                                          cg_.emitStrlen(nested, "sz.arr.len"), "acc.a"),
                              accA);
            }
            B.CreateBr(szNext);

            B.SetInsertPoint(szFixed);
            // (AR) [م-٠٠١] غيرُ النصِّ: كانت ٢٤ بايتًا تكفي i64 وأطولَ اسمٍ منطقيّ. ومنذ
            //      صار العشريُّ يُنسَّقُ هنا بـ`__sad_format_double` (‏%.6f‏) صار DBL_MAX
            //      يبلغُ نحوَ ٣١٦ محرفًا ⇒ ٥١٢ حدًّا آمنًا موحَّدًا لكلِّ الأوسامِ غيرِ
            //      النصّيّة (ثابتٌ مسمًّى لا رقمٌ عارٍ).
            // (EN) [card م-٠٠١] Non-string: 24 bytes used to cover an i64 and the longest boolean
            //      name. Now that floats are formatted here with `__sad_format_double` (%.6f),
            //      DBL_MAX reaches about 316 characters ⇒ 512 is the safe unified bound for every
            //      non-string tag (a named constant, not a bare number).
            // (AR) والحدُّ **يُفرَّقُ بالوسم** لا يُوحَّدُ على أسوأِ الحالات: العشريُّ
            //      وحدَه هو الذي يبلغُ ٣١٦ محرفًا (DBL_MAX بـ%.6f)، أمّا الصحيحُ فأطولُه
            //      ‑9223372036854775808 (٢٠ محرفًا) والمنطقيُّ والعدمُ أقصرُ. وتوحيدُ ٥١٢
            //      لكلِّ وسمٍ كان يحجزُ نحوَ كيلوبايتٍ لخريطةٍ من مدخلَين عدديَّين نصُّها
            //      ٢٠ محرفًا — والحجزُ هنا **يُقاسُ في الذاكرةِ الحيّة** لأنّ ناتجَ
            //      `نص(خريطة)` يهربُ إلى البرنامجِ ولا يُحرَّر (نموذجُ الملكيّةِ القائم).
            // (EN) The bound is now per-tag rather than worst-case-for-all: only Float can
            //      reach ~316 chars; an i64 needs 20 and bool/null less. The unified 512
            //      reserved ~1 KB for a two-integer map whose text is 20 chars — and the
            //      reservation is live memory, since نص(map)'s result escapes unfreed.
            constexpr int64_t kFloatValueReserveBytes = 512;
            constexpr int64_t kScalarValueReserveBytes = 24;
            llvm::Value *szIsFloatTag = B.CreateICmpEQ(
                szType, C0(Sad::Compiler::kMapValueTagFloat), "sz.is.float.tag");
            llvm::Value *szReserve = B.CreateSelect(szIsFloatTag, C0(kFloatValueReserveBytes),
                                                    C0(kScalarValueReserveBytes), "sz.reserve");
            B.CreateStore(B.CreateAdd(B.CreateLoad(i64Ty, accA, "acc.v3"), szReserve, "acc.f"),
                          accA);
            B.CreateBr(szNext);

            B.SetInsertPoint(szNext);
            B.CreateStore(B.CreateAdd(si, C0(1), "si.inc"), siA);
            B.CreateBr(szChk);

            // ── malloc + '{' ──
            B.SetInsertPoint(doAlloc);
            llvm::Value *buf = cg_.emitMalloc(B.CreateLoad(i64Ty, accA, "acc.final"), "m2s.buf");
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
            // (AR) الاقتباسُ اختياريّ: تمريرةُ الحجمِ تحجزُ للحرفَين دائمًا، فإسقاطُهما
            //      يزيدُ فسحةً ولا ينقصُها — لا تجاوزَ مخزن.
            if (quoteKeys)
                writeChar('"');
            writeFmt(fmtS, flKeyPtr); // اسم المفتاح
            if (quoteKeys)
                writeChar('"');
            writeChar(':');
            writeChar(' ');
            llvm::Value *flType = loadI64Elem(typesArr, fi, "fl.type");
            B.CreateCondBr(B.CreateICmpEQ(flType, C0(1), "fl.is.int"), flInt, flBoolChk);

            B.SetInsertPoint(flInt);
            writeFmt(fmtD, loadI64Elem(valsArr, fi, "fl.int.val"));
            B.CreateBr(flNext);

            B.SetInsertPoint(flBoolChk);
            B.CreateCondBr(B.CreateICmpEQ(flType, C0(Sad::Compiler::kMapValueTagBoolean),
                                          "fl.is.bool"),
                           flBool, flFloatChk);

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
            // (AR) العشريّ: `__sad_format_double` تكتبُ في مكانِها فيُقدَّمُ الموضعُ بطولِ
            //      ما كُتب. الدالّةُ نفسُها التي يستعملُها F64_TO_STRING ⇒ تنسيقٌ واحدٌ
            //      في المصرّفِ كلِّه.
            // (EN) Float: `__sad_format_double` writes in place, so the cursor advances by what
            //      it wrote. The same function F64_TO_STRING uses ⇒ one formatting across the
            //      whole compiler.
            B.SetInsertPoint(flFloatChk);
            B.CreateCondBr(B.CreateICmpEQ(flType, C0(Sad::Compiler::kMapValueTagFloat),
                                          "fl.is.float"),
                           flFloat, flNullChk);

            B.SetInsertPoint(flFloat);
            {
                auto *formatDoubleType = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(*cg_.context_),
                    {ptrTy, llvm::Type::getDoubleTy(*cg_.context_)}, false);
                auto formatDoubleFn =
                    cg_.module_->getOrInsertFunction("__sad_format_double", formatDoubleType);
                llvm::Value *pos = B.CreateLoad(i64Ty, posA, "fl.f.pos");
                llvm::Value *dst = B.CreateGEP(i8Ty, buf, pos, "fl.f.dst");
                llvm::Value *asDouble = B.CreateBitCast(
                    loadI64Elem(valsArr, fi, "fl.f.bits"),
                    llvm::Type::getDoubleTy(*cg_.context_), "fl.f.val");
                B.CreateCall(formatDoubleFn, {dst, asDouble});
                B.CreateStore(B.CreateAdd(pos, cg_.emitStrlen(dst, "fl.f.len"), "fl.f.adv"), posA);
            }
            B.CreateBr(flNext);

            // (AR) العدمُ والفراغُ يُعرَضانِ «لاشيء» من مصدرِ الحقيقةِ نفسِه الذي يستعملُه
            //      المفسّرُ وdynToString.
            // (EN) Null and Void both render «لاشيء» from the same source of truth the
            //      interpreter and dynToString use.
            B.SetInsertPoint(flNullChk);
            B.CreateCondBr(
                B.CreateOr(B.CreateICmpEQ(flType, C0(Sad::Compiler::kMapValueTagNull), "fl.is.null"),
                           B.CreateICmpEQ(flType, C0(Sad::Compiler::kMapValueTagVoid), "fl.is.void"),
                           "fl.is.nullish"),
                flNull, flContainerChk);

            B.SetInsertPoint(flNull);
            writeFmt(fmtS, B.CreateGlobalStringPtr(::Sad::Types::repr::kNullDisplay, "m2s.nulltext"));
            B.CreateBr(flNext);

            // (AR) [م-٠٠١] الحاويات: خريطةٌ متداخلةٌ تُنسَّقُ بهذه الدالّةِ نفسِها
            //      استدعاءً ذاتيًّا، ومصفوفةٌ بـ`__sad_array_to_string_dyn`. كانتا
            //      تُخزَّنانِ بوسمِ النصِّ فتُقرأُ ترويسةُ الخريطةِ `char*`.
            // (EN) [card م-٠٠١] Containers: a nested map is formatted by this very function
            //      recursively, and an array by `__sad_array_to_string_dyn`. Both used to be
            //      stored under the string tag, so a map header was read as a `char*`.
            B.SetInsertPoint(flContainerChk);
            B.CreateCondBr(B.CreateICmpEQ(flType, C0(Sad::Compiler::kMapValueTagMap), "fl.is.map"),
                           flMap, flArrayChk);

            B.SetInsertPoint(flMap);
            writeFmt(fmtS, B.CreateCall(fn, {loadPtrElem(valsArr, fi, "fl.map.val")}, "fl.map.str"));
            B.CreateBr(flNext);

            B.SetInsertPoint(flArrayChk);
            B.CreateCondBr(B.CreateICmpEQ(flType, C0(Sad::Compiler::kMapValueTagArray), "fl.is.array"),
                           flArray, flStrChk);

            B.SetInsertPoint(flArray);
            writeFmt(fmtS, callArrayToString(loadPtrElem(valsArr, fi, "fl.arr.val"), "fl.arr.str"));
            B.CreateBr(flNext);

            B.SetInsertPoint(flStrChk);
            B.CreateCondBr(B.CreateICmpEQ(flType, C0(Sad::Compiler::kMapValueTagString),
                                          "fl.is.str"),
                           flStr, flInt);

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
