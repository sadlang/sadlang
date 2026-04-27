/*
 * ============================================================================
 * مولد كود LLVM IR - ملف التنفيذ
 * LLVM IR Code Generator - Implementation File
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

        llvm::Value *LLVMCodeGen::emitInlineAsm(std::shared_ptr<SIRInstruction> inst)
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
                reportError("INLINE_ASM: requires at least 1 operand (assembly text)");
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
                reportError("INLINE_ASM: first operand must be a string constant (assembly text)");
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
                llvm::Value *val = resolveOperand(inst->operands[i]);
                if (val)
                {
                    inputValues.push_back(val);
                    inputTypes.push_back(val->getType());
                }
            }

            // (AR) تحديد نوع الرجوع
            // (EN) Determine return type
            llvm::Type *retType = llvm::Type::getVoidTy(*context_);
            bool hasResult = inst->result.has_value();
            if (hasResult)
            {
                // (AR) إذا كان هناك نتيجة، نستخدم i64 كنوع افتراضي
                // (EN) If there's a result, use i64 as default type
                retType = llvm::Type::getInt64Ty(*context_);
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
            llvm::Value *result = builder_->CreateCall(asmFuncType, inlineAsm, inputValues);

            if (hasResult)
            {
                context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        // ============================================================================
        // (AR) توليد دالة __sad_array_to_string في الموديول
        //      تحوّل مصفوفة SadArray إلى نص مقروء: "[عنصر1, عنصر2, ...]"
        // (EN) Generate __sad_array_to_string function in the module
        //      Converts SadArray to readable string: "[elem1, elem2, ...]"
        // ============================================================================
        void LLVMCodeGen::ensureArrayToStringHelper()
        {
            // (AR) إذا الدالة موجودة ولها جسم، لا تُنشئها مرة أخرى
            // (EN) If function exists and has a body, don't recreate
            llvm::Function *existing = module_->getFunction("__sad_array_to_string");
            if (existing && !existing->empty())
            {
                return;
            }

            auto i64Ty = llvm::Type::getInt64Ty(*context_);
            auto i32Ty = llvm::Type::getInt32Ty(*context_);
            auto i8Ty = llvm::Type::getInt8Ty(*context_);
            auto ptrTy = llvm::PointerType::getUnqual(*context_);

            // Function signature: i8* __sad_array_to_string(i8* buf, i64 len, i8* data)
            llvm::FunctionType *fnTy = llvm::FunctionType::get(ptrTy, {ptrTy, i64Ty, ptrTy}, false);
            llvm::Function *fn = llvm::Function::Create(
                fnTy, llvm::Function::InternalLinkage, "__sad_array_to_string", module_.get());

            llvm::Argument *bufArg = fn->getArg(0);
            llvm::Argument *lenArg = fn->getArg(1);
            llvm::Argument *dataArg = fn->getArg(2);
            bufArg->setName("buf");
            lenArg->setName("len");
            dataArg->setName("data");

            // Save current insertion point
            llvm::BasicBlock *savedBB = builder_->GetInsertBlock();
            llvm::BasicBlock::iterator savedPoint = builder_->GetInsertPoint();

            // Create basic blocks
            llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(*context_, "entry", fn);
            llvm::BasicBlock *loopCheckBB = llvm::BasicBlock::Create(*context_, "loop.check", fn);
            llvm::BasicBlock *loopBodyBB = llvm::BasicBlock::Create(*context_, "loop.body", fn);
            llvm::BasicBlock *commaWriteBB = llvm::BasicBlock::Create(*context_, "comma.write", fn);
            llvm::BasicBlock *elemWriteBB = llvm::BasicBlock::Create(*context_, "elem.write", fn);
            llvm::BasicBlock *loopEndBB = llvm::BasicBlock::Create(*context_, "loop.end", fn);

            // Declare sprintf
            llvm::FunctionType *sprintfTy = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, true);
            llvm::FunctionCallee sprintfFn = module_->getOrInsertFunction("sprintf", sprintfTy);

            // entry: write '[' at buf[0], pos = 1
            builder_->SetInsertPoint(entryBB);
            builder_->CreateStore(llvm::ConstantInt::get(i8Ty, '['), bufArg);
            llvm::Value *initPos = llvm::ConstantInt::get(i64Ty, 1);
            builder_->CreateBr(loopCheckBB);

            // loop.check: i = phi, pos = phi; if i < len goto body else goto end
            builder_->SetInsertPoint(loopCheckBB);
            llvm::PHINode *iPhi = builder_->CreatePHI(i64Ty, 2, "i");
            llvm::PHINode *posPhi = builder_->CreatePHI(i64Ty, 2, "pos");
            iPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entryBB);
            posPhi->addIncoming(initPos, entryBB);
            llvm::Value *cmp = builder_->CreateICmpSLT(iPhi, lenArg, "i.lt.len");
            builder_->CreateCondBr(cmp, loopBodyBB, loopEndBB);

            // loop.body: if i > 0, write ", "
            builder_->SetInsertPoint(loopBodyBB);
            llvm::Value *needComma = builder_->CreateICmpSGT(iPhi, llvm::ConstantInt::get(i64Ty, 0), "need.comma");
            builder_->CreateCondBr(needComma, commaWriteBB, elemWriteBB);

            // comma.write: write ", " at buf+pos
            builder_->SetInsertPoint(commaWriteBB);
            llvm::Value *commaFmt = builder_->CreateGlobalStringPtr(", ", "comma.fmt");
            llvm::Value *commaPos = builder_->CreateGEP(i8Ty, bufArg, posPhi, "comma.ptr");
            llvm::Value *commaLen = builder_->CreateCall(sprintfFn, {commaPos, commaFmt}, "comma.len");
            llvm::Value *commaLen64 = builder_->CreateSExt(commaLen, i64Ty, "comma.len64");
            llvm::Value *posAfterComma = builder_->CreateAdd(posPhi, commaLen64, "pos.after.comma");
            builder_->CreateBr(elemWriteBB);

            // elem.write: load element, sprintf it, advance pos
            builder_->SetInsertPoint(elemWriteBB);
            llvm::PHINode *elemPosPhi = builder_->CreatePHI(i64Ty, 2, "elem.pos");
            elemPosPhi->addIncoming(posPhi, loopBodyBB);
            elemPosPhi->addIncoming(posAfterComma, commaWriteBB);

            // Load element as i64 (all array elements stored as i64 or ptr-sized values)
            llvm::Value *elemGep = builder_->CreateGEP(i64Ty, dataArg, iPhi, "elem.gep");
            llvm::Value *elemVal = builder_->CreateLoad(i64Ty, elemGep, "elem.val");

            // sprintf(buf+pos, "%lld", elem)
            llvm::Value *elemFmt = builder_->CreateGlobalStringPtr("%lld", "elem.fmt");
            llvm::Value *elemDst = builder_->CreateGEP(i8Ty, bufArg, elemPosPhi, "elem.dst");
            llvm::Value *elemLen = builder_->CreateCall(sprintfFn, {elemDst, elemFmt, elemVal}, "elem.len");
            llvm::Value *elemLen64 = builder_->CreateSExt(elemLen, i64Ty, "elem.len64");
            llvm::Value *newPos = builder_->CreateAdd(elemPosPhi, elemLen64, "new.pos");

            // i++
            llvm::Value *nextI = builder_->CreateAdd(iPhi, llvm::ConstantInt::get(i64Ty, 1), "next.i");

            // Back to loop check
            iPhi->addIncoming(nextI, elemWriteBB);
            posPhi->addIncoming(newPos, elemWriteBB);
            builder_->CreateBr(loopCheckBB);

            // loop.end: write ']' and '\0'
            builder_->SetInsertPoint(loopEndBB);
            llvm::Value *closeBracketPtr = builder_->CreateGEP(i8Ty, bufArg, posPhi, "close.ptr");
            builder_->CreateStore(llvm::ConstantInt::get(i8Ty, ']'), closeBracketPtr);
            llvm::Value *endPos = builder_->CreateAdd(posPhi, llvm::ConstantInt::get(i64Ty, 1), "end.pos");
            llvm::Value *nullPtr = builder_->CreateGEP(i8Ty, bufArg, endPos, "null.ptr");
            builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0), nullPtr);
            builder_->CreateRet(bufArg);

            // Restore insertion point
            if (savedBB)
            {
                builder_->SetInsertPoint(savedBB, savedPoint);
            }
        }
    } // namespace LLVM
} // namespace Sad