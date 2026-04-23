// ============================================================================
// llvm_codegen_hardware_ffi2.cpp — DMA + FFI C runtime operations
// ============================================================================
// (AR) عمليات DMA + دوال FFI للـ C runtime (printf, malloc, str*, file*)
// (EN) DMA operations + C FFI runtime functions
// تم استخراج هذا الملف من llvm_codegen_hardware_ffi.cpp وفقاً لقاعدة CW-05
// ============================================================================
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

// Source: llvm_codegen.h:103-108 - using declarations
using namespace Sad::Compiler::SIR; // For SIRModule, SIRFunction, SIRBasicBlock, SIRInstruction, SadTypeKind

namespace Sad
{
    namespace LLVM
    {

        llvm::Value *LLVMCodeGen::emitDmaInit(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 4)
            {
                reportError("dma_init: needs 4 operands");
                return nullptr;
            }
            llvm::Value *ch = resolveOperand(inst->operands[0]);
            llvm::Value *src = resolveOperand(inst->operands[1]);
            llvm::Value *dest = resolveOperand(inst->operands[2]);
            llvm::Value *cnt = resolveOperand(inst->operands[3]);
            if (!ch || !src || !dest || !cnt)
                return nullptr;
            llvm::Type *i16 = llvm::Type::getInt16Ty(*context_);
            llvm::Type *i8 = llvm::Type::getInt8Ty(*context_);
            llvm::FunctionType *outbFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {i8, i16}, false);
            llvm::InlineAsm *outbIA = llvm::InlineAsm::get(outbFT, "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
            llvm::Value *ch8 = builder_->CreateIntCast(ch, i8, false);
            builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {builder_->CreateOr(ch8, llvm::ConstantInt::get(i8, 0x04)), llvm::ConstantInt::get(i16, 0x0A)});
            builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {llvm::ConstantInt::get(i8, 0x00), llvm::ConstantInt::get(i16, 0x0C)});
            llvm::Value *addrPort = builder_->CreateMul(builder_->CreateIntCast(ch, i16, false), llvm::ConstantInt::get(i16, 2));
            builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {builder_->CreateIntCast(src, i8, false), addrPort});
            builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {builder_->CreateIntCast(builder_->CreateLShr(src, 8), i8, false), addrPort});
            llvm::Value *cntPort = builder_->CreateAdd(addrPort, llvm::ConstantInt::get(i16, 1));
            llvm::Value *cnt1 = builder_->CreateSub(cnt, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 1));
            builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {builder_->CreateIntCast(cnt1, i8, false), cntPort});
            builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {builder_->CreateIntCast(builder_->CreateLShr(cnt1, 8), i8, false), cntPort});
            builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {ch8, llvm::ConstantInt::get(i16, 0x0A)});
            return nullptr;
        }

        llvm::Value *LLVMCodeGen::emitDmaStart(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("dma_start: needs 1 operand");
                return nullptr;
            }
            llvm::Value *ch = resolveOperand(inst->operands[0]);
            if (!ch)
                return nullptr;
            llvm::Type *i16 = llvm::Type::getInt16Ty(*context_);
            llvm::Type *i8 = llvm::Type::getInt8Ty(*context_);
            llvm::FunctionType *outbFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {i8, i16}, false);
            llvm::InlineAsm *outbIA = llvm::InlineAsm::get(outbFT, "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
            llvm::Value *ch8 = builder_->CreateIntCast(ch, i8, false);
            builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {builder_->CreateOr(ch8, llvm::ConstantInt::get(i8, 0x48)), llvm::ConstantInt::get(i16, 0x0B)});
            builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {ch8, llvm::ConstantInt::get(i16, 0x0A)});
            return nullptr;
        }

        // ============================================================================
        // FFI Emit Methods (20)
        // ============================================================================

        llvm::Value *LLVMCodeGen::emitFFIPrintf(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("printf: needs operands");
                return nullptr;
            }
            llvm::FunctionType *printfType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), {llvm::Type::getInt8Ty(*context_)->getPointerTo()}, true);
            llvm::FunctionCallee printfFunc = module_->getOrInsertFunction("printf", printfType);
            std::vector<llvm::Value *> args;
            for (auto &op : inst->operands)
            {
                llvm::Value *v = resolveOperand(op);
                if (v)
                    args.push_back(v);
            }
            if (args.empty())
                return nullptr;
            llvm::Value *result = builder_->CreateCall(printfFunc, args, "printf.ret");
            llvm::Value *ext = builder_->CreateSExt(result, llvm::Type::getInt64Ty(*context_));
            if (inst->result.has_value())
                context_info_.namedValues[inst->result->name] = ext;
            return ext;
        }

        llvm::Value *LLVMCodeGen::emitFFIMalloc(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("malloc: needs 1 operand");
                return nullptr;
            }
            llvm::Value *size = resolveOperand(inst->operands[0]);
            if (!size)
                return nullptr;
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getInt8Ty(*context_)->getPointerTo(), {llvm::Type::getInt64Ty(*context_)}, false);
            llvm::FunctionCallee fn = module_->getOrInsertFunction("malloc", ft);
            llvm::Value *result = builder_->CreateCall(fn, {size}, "malloc.ptr");
            if (inst->result.has_value())
                context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *LLVMCodeGen::emitFFIFree(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("free: needs 1 operand");
                return nullptr;
            }
            llvm::Value *ptr = resolveOperand(inst->operands[0]);
            if (!ptr)
                return nullptr;
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {llvm::Type::getInt8Ty(*context_)->getPointerTo()}, false);
            llvm::FunctionCallee fn = module_->getOrInsertFunction("free", ft);
            if (!ptr->getType()->isPointerTy())
                ptr = builder_->CreateIntToPtr(ptr, llvm::Type::getInt8Ty(*context_)->getPointerTo());
            builder_->CreateCall(fn, {ptr});
            return nullptr;
        }

        llvm::Value *LLVMCodeGen::emitFFIRealloc(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                reportError("realloc: needs 2 operands");
                return nullptr;
            }
            llvm::Value *ptr = resolveOperand(inst->operands[0]);
            llvm::Value *size = resolveOperand(inst->operands[1]);
            if (!ptr || !size)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8p, {i8p, llvm::Type::getInt64Ty(*context_)}, false);
            llvm::FunctionCallee fn = module_->getOrInsertFunction("realloc", ft);
            if (!ptr->getType()->isPointerTy())
                ptr = builder_->CreateIntToPtr(ptr, i8p);
            llvm::Value *result = builder_->CreateCall(fn, {ptr, size}, "realloc.ptr");
            if (inst->result.has_value())
                context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *LLVMCodeGen::emitFFICalloc(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                reportError("calloc: needs 2 operands");
                return nullptr;
            }
            llvm::Value *count = resolveOperand(inst->operands[0]);
            llvm::Value *size = resolveOperand(inst->operands[1]);
            if (!count || !size)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8p, {llvm::Type::getInt64Ty(*context_), llvm::Type::getInt64Ty(*context_)}, false);
            llvm::FunctionCallee fn = module_->getOrInsertFunction("calloc", ft);
            llvm::Value *result = builder_->CreateCall(fn, {count, size}, "calloc.ptr");
            if (inst->result.has_value())
                context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        // ================================================================
        // (AR) __sad_utf8_strlen — يعد حروف UTF-8 بدلاً من البايتات
        //      الخوارزمية: يمر على كل بايت ويعد فقط البايتات التي ليست continuation bytes
        //      (أي البايتات التي لا تطابق النمط 10xxxxxx / 0x80-0xBF)
        //      هذا يعني أن البايتات بالنمط 0xxxxxxx و 11xxxxxx فقط تُعَد
        // (EN) __sad_utf8_strlen — counts UTF-8 characters instead of bytes
        //      Algorithm: iterate bytes, count only non-continuation bytes
        //      (bytes that don't match pattern 10xxxxxx / 0x80-0xBF)
        // ================================================================
        llvm::Function *LLVMCodeGen::getOrCreateUtf8Strlen()
        {
            // (AR) إذا الدالة موجودة مسبقاً، أرجعها
            // (EN) If function already exists, return it
            llvm::Function *existing = module_->getFunction("__sad_utf8_strlen");
            if (existing && !existing->empty())
                return existing;

            // (AR) بناء الدالة: i64 __sad_utf8_strlen(i8* str)
            // (EN) Build function: i64 __sad_utf8_strlen(i8* str)
            llvm::Type *i8Ty = llvm::Type::getInt8Ty(*context_);
            llvm::Type *i64Ty = llvm::Type::getInt64Ty(*context_);
            llvm::Type *i8p = i8Ty->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i64Ty, {i8p}, false);
            llvm::Function *fn = llvm::Function::Create(
                ft, llvm::Function::InternalLinkage, "__sad_utf8_strlen", module_.get());

            // (AR) حفظ نقطة الإدراج الحالية واستعادتها لاحقاً
            // (EN) Save current insert point and restore it later
            llvm::BasicBlock *savedBB = builder_->GetInsertBlock();
            llvm::BasicBlock::iterator savedPoint;
            bool hasSavedPoint = false;
            if (savedBB)
            {
                savedPoint = builder_->GetInsertPoint();
                hasSavedPoint = true;
            }

            // (AR) بناء جسم الدالة
            // (EN) Build function body
            llvm::BasicBlock *entry = llvm::BasicBlock::Create(*context_, "entry", fn);
            llvm::BasicBlock *loopCheck = llvm::BasicBlock::Create(*context_, "loop.check", fn);
            llvm::BasicBlock *loopBody = llvm::BasicBlock::Create(*context_, "loop.body", fn);
            llvm::BasicBlock *countChar = llvm::BasicBlock::Create(*context_, "count.char", fn);
            llvm::BasicBlock *loopInc = llvm::BasicBlock::Create(*context_, "loop.inc", fn);
            llvm::BasicBlock *exitBB = llvm::BasicBlock::Create(*context_, "exit", fn);

            llvm::Argument *strArg = fn->arg_begin();
            strArg->setName("str");

            // entry: idx=0, count=0, null check
            builder_->SetInsertPoint(entry);
            llvm::Value *nullCheck = builder_->CreateICmpEQ(strArg,
                                                            llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(i8p)), "is.null");
            builder_->CreateCondBr(nullCheck, exitBB, loopCheck);

            // loop.check: load byte, check if null terminator
            builder_->SetInsertPoint(loopCheck);
            llvm::PHINode *idxPhi = builder_->CreatePHI(i64Ty, 2, "idx");
            llvm::PHINode *countPhi = builder_->CreatePHI(i64Ty, 2, "count");
            idxPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
            countPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);

            llvm::Value *bytePtr = builder_->CreateGEP(i8Ty, strArg, {idxPhi}, "byte.ptr");
            llvm::Value *byte = builder_->CreateLoad(i8Ty, bytePtr, "byte");
            llvm::Value *isNull = builder_->CreateICmpEQ(byte,
                                                         llvm::ConstantInt::get(i8Ty, 0), "is.null.term");
            builder_->CreateCondBr(isNull, exitBB, loopBody);

            // loop.body: check if continuation byte (byte & 0xC0) == 0x80
            builder_->SetInsertPoint(loopBody);
            llvm::Value *masked = builder_->CreateAnd(byte,
                                                      llvm::ConstantInt::get(i8Ty, 0xC0), "masked");
            llvm::Value *isCont = builder_->CreateICmpEQ(masked,
                                                         llvm::ConstantInt::get(i8Ty, 0x80), "is.cont");
            builder_->CreateCondBr(isCont, loopInc, countChar);

            // count.char: increment count (not a continuation byte = start of new char)
            builder_->SetInsertPoint(countChar);
            llvm::Value *newCount = builder_->CreateAdd(countPhi,
                                                        llvm::ConstantInt::get(i64Ty, 1), "new.count");
            builder_->CreateBr(loopInc);

            // loop.inc: increment idx
            builder_->SetInsertPoint(loopInc);
            llvm::PHINode *countMerge = builder_->CreatePHI(i64Ty, 2, "count.merge");
            countMerge->addIncoming(countPhi, loopBody);  // continuation byte, count unchanged
            countMerge->addIncoming(newCount, countChar); // new char, count incremented
            llvm::Value *nextIdx = builder_->CreateAdd(idxPhi,
                                                       llvm::ConstantInt::get(i64Ty, 1), "next.idx");
            builder_->CreateBr(loopCheck);

            idxPhi->addIncoming(nextIdx, loopInc);
            countPhi->addIncoming(countMerge, loopInc);

            // exit: return count
            builder_->SetInsertPoint(exitBB);
            llvm::PHINode *retPhi = builder_->CreatePHI(i64Ty, 2, "ret.count");
            retPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry); // null string
            retPhi->addIncoming(countPhi, loopCheck);                     // normal termination
            builder_->CreateRet(retPhi);

            // (AR) استعادة نقطة الإدراج الأصلية
            // (EN) Restore original insert point
            if (hasSavedPoint && savedBB)
            {
                builder_->SetInsertPoint(savedBB, savedPoint);
            }

            return fn;
        }

        // ================================================================
        // (AR) تحويل موقع البايت إلى فهرس الحرف UTF-8
        // (EN) Convert byte offset to UTF-8 character index
        // الدالة: i64 __sad_utf8_byte_to_char(i8* str, i64 byte_offset)
        // تعد عدد الحروف (غير بايتات المتابعة) في أول byte_offset بايت من str
        // ================================================================
        llvm::Function *LLVMCodeGen::getOrCreateUtf8ByteToChar()
        {
            llvm::Function *existing = module_->getFunction("__sad_utf8_byte_to_char");
            if (existing && !existing->empty())
                return existing;

            llvm::Type *i8Ty = llvm::Type::getInt8Ty(*context_);
            llvm::Type *i64Ty = llvm::Type::getInt64Ty(*context_);
            llvm::Type *i8p = i8Ty->getPointerTo();

            llvm::FunctionType *ft = llvm::FunctionType::get(i64Ty, {i8p, i64Ty}, false);
            llvm::Function *fn = llvm::Function::Create(
                ft, llvm::Function::InternalLinkage, "__sad_utf8_byte_to_char", module_.get());

            // (AR) حفظ نقطة الإدراج الحالية
            llvm::BasicBlock *savedBB = builder_->GetInsertBlock();
            llvm::BasicBlock::iterator savedPoint;
            bool hasSavedPoint = false;
            if (savedBB)
            {
                savedPoint = builder_->GetInsertPoint();
                hasSavedPoint = true;
            }

            // (AR) بناء الكتل
            llvm::BasicBlock *entry = llvm::BasicBlock::Create(*context_, "entry", fn);
            llvm::BasicBlock *loopCheck = llvm::BasicBlock::Create(*context_, "loop.check", fn);
            llvm::BasicBlock *loopBody = llvm::BasicBlock::Create(*context_, "loop.body", fn);
            llvm::BasicBlock *countChar = llvm::BasicBlock::Create(*context_, "count.char", fn);
            llvm::BasicBlock *loopInc = llvm::BasicBlock::Create(*context_, "loop.inc", fn);
            llvm::BasicBlock *exitBB = llvm::BasicBlock::Create(*context_, "exit", fn);

            auto argIt = fn->arg_begin();
            llvm::Argument *strArg = &*argIt++;
            llvm::Argument *byteOffsetArg = &*argIt;
            strArg->setName("str");
            byteOffsetArg->setName("byte_offset");

            // entry: فحص أن byte_offset > 0
            builder_->SetInsertPoint(entry);
            llvm::Value *cmpZero = builder_->CreateICmpSLE(byteOffsetArg,
                                                           llvm::ConstantInt::get(i64Ty, 0), "cmp.zero");
            builder_->CreateCondBr(cmpZero, exitBB, loopCheck);

            // loop.check: هل وصلنا byte_offset؟
            builder_->SetInsertPoint(loopCheck);
            llvm::PHINode *idxPhi = builder_->CreatePHI(i64Ty, 2, "idx");
            llvm::PHINode *countPhi = builder_->CreatePHI(i64Ty, 2, "count");
            idxPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
            countPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);

            llvm::Value *cmpEnd = builder_->CreateICmpSLT(idxPhi, byteOffsetArg, "cmp.end");
            builder_->CreateCondBr(cmpEnd, loopBody, exitBB);

            // loop.body: فحص هل البايت بايت متابعة (continuation byte)
            builder_->SetInsertPoint(loopBody);
            llvm::Value *bytePtr = builder_->CreateGEP(i8Ty, strArg, {idxPhi}, "byte.ptr");
            llvm::Value *byte = builder_->CreateLoad(i8Ty, bytePtr, "byte");
            llvm::Value *masked = builder_->CreateAnd(byte,
                                                      llvm::ConstantInt::get(i8Ty, 0xC0), "masked");
            llvm::Value *isCont = builder_->CreateICmpEQ(masked,
                                                         llvm::ConstantInt::get(i8Ty, 0x80), "is.cont");
            builder_->CreateCondBr(isCont, loopInc, countChar);

            // count.char: ليس بايت متابعة → حرف جديد
            builder_->SetInsertPoint(countChar);
            llvm::Value *newCount = builder_->CreateAdd(countPhi,
                                                        llvm::ConstantInt::get(i64Ty, 1), "new.count");
            builder_->CreateBr(loopInc);

            // loop.inc: التقدم
            builder_->SetInsertPoint(loopInc);
            llvm::PHINode *countMerge = builder_->CreatePHI(i64Ty, 2, "count.merge");
            countMerge->addIncoming(countPhi, loopBody);
            countMerge->addIncoming(newCount, countChar);
            llvm::Value *nextIdx = builder_->CreateAdd(idxPhi,
                                                       llvm::ConstantInt::get(i64Ty, 1), "next.idx");
            builder_->CreateBr(loopCheck);

            idxPhi->addIncoming(nextIdx, loopInc);
            countPhi->addIncoming(countMerge, loopInc);

            // exit: إرجاع العداد
            builder_->SetInsertPoint(exitBB);
            llvm::PHINode *retPhi = builder_->CreatePHI(i64Ty, 2, "ret.count");
            retPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
            retPhi->addIncoming(countPhi, loopCheck);
            builder_->CreateRet(retPhi);

            // (AR) استعادة نقطة الإدراج
            if (hasSavedPoint && savedBB)
                builder_->SetInsertPoint(savedBB, savedPoint);

            return fn;
        }

        // ================================================================
        // (AR) تحويل فهرس الحرف UTF-8 إلى موقع البايت
        // (EN) Convert UTF-8 character index to byte offset
        // الدالة: i64 __sad_utf8_char_to_byte(i8* str, i64 char_index)
        // تجد موقع البايت لبداية الحرف رقم char_index في str
        // ================================================================
        llvm::Function *LLVMCodeGen::getOrCreateUtf8CharToByte()
        {
            llvm::Function *existing = module_->getFunction("__sad_utf8_char_to_byte");
            if (existing && !existing->empty())
                return existing;

            llvm::Type *i8Ty = llvm::Type::getInt8Ty(*context_);
            llvm::Type *i64Ty = llvm::Type::getInt64Ty(*context_);
            llvm::Type *i32Ty = llvm::Type::getInt32Ty(*context_);
            llvm::Type *i8p = i8Ty->getPointerTo();

            llvm::FunctionType *ft = llvm::FunctionType::get(i64Ty, {i8p, i64Ty}, false);
            llvm::Function *fn = llvm::Function::Create(
                ft, llvm::Function::InternalLinkage, "__sad_utf8_char_to_byte", module_.get());

            // (AR) حفظ نقطة الإدراج الحالية
            llvm::BasicBlock *savedBB = builder_->GetInsertBlock();
            llvm::BasicBlock::iterator savedPoint;
            bool hasSavedPoint = false;
            if (savedBB)
            {
                savedPoint = builder_->GetInsertPoint();
                hasSavedPoint = true;
            }

            // (AR) بناء الكتل
            llvm::BasicBlock *entry = llvm::BasicBlock::Create(*context_, "entry", fn);
            llvm::BasicBlock *loopCheck = llvm::BasicBlock::Create(*context_, "loop.check", fn);
            llvm::BasicBlock *readByte = llvm::BasicBlock::Create(*context_, "read.byte", fn);
            llvm::BasicBlock *classify = llvm::BasicBlock::Create(*context_, "classify", fn);
            llvm::BasicBlock *advance = llvm::BasicBlock::Create(*context_, "advance", fn);
            llvm::BasicBlock *exitBB = llvm::BasicBlock::Create(*context_, "exit", fn);

            auto argIt = fn->arg_begin();
            llvm::Argument *strArg = &*argIt++;
            llvm::Argument *charIndexArg = &*argIt;
            strArg->setName("str");
            charIndexArg->setName("char_index");

            // entry: فحص char_index <= 0
            builder_->SetInsertPoint(entry);
            llvm::Value *cmpZero = builder_->CreateICmpSLE(charIndexArg,
                                                           llvm::ConstantInt::get(i64Ty, 0), "cmp.zero");
            builder_->CreateCondBr(cmpZero, exitBB, loopCheck);

            // loop.check: هل عددنا الحروف الكافية؟
            builder_->SetInsertPoint(loopCheck);
            llvm::PHINode *bytePosP = builder_->CreatePHI(i64Ty, 2, "byte.pos");
            llvm::PHINode *charsP = builder_->CreatePHI(i64Ty, 2, "chars");
            bytePosP->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
            charsP->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);

            llvm::Value *reached = builder_->CreateICmpSGE(charsP, charIndexArg, "reached");
            builder_->CreateCondBr(reached, exitBB, readByte);

            // read.byte: قراءة البايت الأول من الحرف الحالي
            builder_->SetInsertPoint(readByte);
            llvm::Value *bp = builder_->CreateGEP(i8Ty, strArg, {bytePosP}, "bp");
            llvm::Value *b = builder_->CreateLoad(i8Ty, bp, "b");
            llvm::Value *isNullTerm = builder_->CreateICmpEQ(b,
                                                             llvm::ConstantInt::get(i8Ty, 0), "is.null");
            builder_->CreateCondBr(isNullTerm, exitBB, classify);

            // classify: تحديد عرض الحرف UTF-8 (1/2/3/4 بايتات)
            builder_->SetInsertPoint(classify);
            llvm::Value *bExt = builder_->CreateZExt(b, i32Ty, "b.ext");
            // (AR) إذا >= 0xF0 → 4 بايتات، >= 0xE0 → 3، >= 0xC0 → 2، غير ذلك → 1
            llvm::Value *is4 = builder_->CreateICmpUGE(bExt, llvm::ConstantInt::get(i32Ty, 0xF0), "is4");
            llvm::Value *is3 = builder_->CreateICmpUGE(bExt, llvm::ConstantInt::get(i32Ty, 0xE0), "is3");
            llvm::Value *is2 = builder_->CreateICmpUGE(bExt, llvm::ConstantInt::get(i32Ty, 0xC0), "is2");
            // (AR) اختيار العرض: 4 > 3 > 2 > 1
            llvm::Value *w = builder_->CreateSelect(is4, llvm::ConstantInt::get(i64Ty, 4),
                                                    builder_->CreateSelect(is3, llvm::ConstantInt::get(i64Ty, 3),
                                                                           builder_->CreateSelect(is2, llvm::ConstantInt::get(i64Ty, 2),
                                                                                                  llvm::ConstantInt::get(i64Ty, 1))));
            builder_->CreateBr(advance);

            // advance: التقدم بالبايت والحرف
            builder_->SetInsertPoint(advance);
            llvm::Value *newBytePos = builder_->CreateAdd(bytePosP, w, "new.byte.pos");
            llvm::Value *newChars = builder_->CreateAdd(charsP,
                                                        llvm::ConstantInt::get(i64Ty, 1), "new.chars");
            builder_->CreateBr(loopCheck);

            bytePosP->addIncoming(newBytePos, advance);
            charsP->addIncoming(newChars, advance);

            // exit: إرجاع موقع البايت
            builder_->SetInsertPoint(exitBB);
            llvm::PHINode *retPhi = builder_->CreatePHI(i64Ty, 3, "ret.byte.pos");
            retPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry); // char_index <= 0
            retPhi->addIncoming(bytePosP, loopCheck);                     // وصلنا العدد المطلوب
            retPhi->addIncoming(bytePosP, readByte);                      // نهاية النص (null)
            builder_->CreateRet(retPhi);

            // (AR) استعادة نقطة الإدراج
            if (hasSavedPoint && savedBB)
                builder_->SetInsertPoint(savedBB, savedPoint);

            return fn;
        }

        llvm::Value *LLVMCodeGen::emitFFIStrlen(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *str = resolveOperand(inst->operands[0]);
            if (!str)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
            if (!str->getType()->isPointerTy())
                str = builder_->CreateIntToPtr(str, i8p);

            // (AR) استخدام __sad_utf8_strlen بدلاً من strlen لعد حروف UTF-8
            // (EN) Use __sad_utf8_strlen instead of strlen to count UTF-8 characters
            llvm::Function *utf8Fn = getOrCreateUtf8Strlen();
            llvm::Value *result = builder_->CreateCall(utf8Fn, {str}, "utf8len.ret");
            if (inst->result.has_value())
                context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *LLVMCodeGen::emitFFIStrcpy(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *dst = resolveOperand(inst->operands[0]);
            llvm::Value *src = resolveOperand(inst->operands[1]);
            if (!dst || !src)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8p, {i8p, i8p}, false);
            llvm::FunctionCallee fn = module_->getOrInsertFunction("strcpy", ft);
            if (!dst->getType()->isPointerTy())
                dst = builder_->CreateIntToPtr(dst, i8p);
            if (!src->getType()->isPointerTy())
                src = builder_->CreateIntToPtr(src, i8p);
            llvm::Value *result = builder_->CreateCall(fn, {dst, src}, "strcpy.ret");
            if (inst->result.has_value())
                context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *LLVMCodeGen::emitFFIStrcmp(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *s1 = resolveOperand(inst->operands[0]);
            llvm::Value *s2 = resolveOperand(inst->operands[1]);
            if (!s1 || !s2)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), {i8p, i8p}, false);
            llvm::FunctionCallee fn = module_->getOrInsertFunction("strcmp", ft);
            if (!s1->getType()->isPointerTy())
                s1 = builder_->CreateIntToPtr(s1, i8p);
            if (!s2->getType()->isPointerTy())
                s2 = builder_->CreateIntToPtr(s2, i8p);
            llvm::Value *result = builder_->CreateSExt(builder_->CreateCall(fn, {s1, s2}, "strcmp.ret"), llvm::Type::getInt64Ty(*context_));
            if (inst->result.has_value())
                context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *LLVMCodeGen::emitFFIStrcat(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *dst = resolveOperand(inst->operands[0]);
            llvm::Value *src = resolveOperand(inst->operands[1]);
            if (!dst || !src)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8p, {i8p, i8p}, false);
            llvm::FunctionCallee fn = module_->getOrInsertFunction("strcat", ft);
            if (!dst->getType()->isPointerTy())
                dst = builder_->CreateIntToPtr(dst, i8p);
            if (!src->getType()->isPointerTy())
                src = builder_->CreateIntToPtr(src, i8p);
            llvm::Value *result = builder_->CreateCall(fn, {dst, src}, "strcat.ret");
            if (inst->result.has_value())
                context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *LLVMCodeGen::emitFFIMemcpy(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
                return nullptr;
            llvm::Value *dst = resolveOperand(inst->operands[0]);
            llvm::Value *src = resolveOperand(inst->operands[1]);
            llvm::Value *sz = resolveOperand(inst->operands[2]);
            if (!dst || !src || !sz)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8p, {i8p, i8p, llvm::Type::getInt64Ty(*context_)}, false);
            llvm::FunctionCallee fn = module_->getOrInsertFunction("memcpy", ft);
            if (!dst->getType()->isPointerTy())
                dst = builder_->CreateIntToPtr(dst, i8p);
            if (!src->getType()->isPointerTy())
                src = builder_->CreateIntToPtr(src, i8p);
            llvm::Value *result = builder_->CreateCall(fn, {dst, src, sz}, "memcpy.ret");
            if (inst->result.has_value())
                context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *LLVMCodeGen::emitFFIMemset(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
                return nullptr;
            llvm::Value *dst = resolveOperand(inst->operands[0]);
            llvm::Value *val = resolveOperand(inst->operands[1]);
            llvm::Value *sz = resolveOperand(inst->operands[2]);
            if (!dst || !val || !sz)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8p, {i8p, llvm::Type::getInt32Ty(*context_), llvm::Type::getInt64Ty(*context_)}, false);
            llvm::FunctionCallee fn = module_->getOrInsertFunction("memset", ft);
            if (!dst->getType()->isPointerTy())
                dst = builder_->CreateIntToPtr(dst, i8p);
            llvm::Value *val32 = builder_->CreateIntCast(val, llvm::Type::getInt32Ty(*context_), false);
            llvm::Value *result = builder_->CreateCall(fn, {dst, val32, sz}, "memset.ret");
            if (inst->result.has_value())
                context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *LLVMCodeGen::emitFFIFopen(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *name = resolveOperand(inst->operands[0]);
            llvm::Value *mode = resolveOperand(inst->operands[1]);
            if (!name || !mode)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8p, {i8p, i8p}, false);
            llvm::FunctionCallee fn = module_->getOrInsertFunction("fopen", ft);
            llvm::Value *result = builder_->CreateCall(fn, {name, mode}, "fopen.ret");
            if (inst->result.has_value())
                context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *LLVMCodeGen::emitFFIFclose(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *fp = resolveOperand(inst->operands[0]);
            if (!fp)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), {i8p}, false);
            llvm::FunctionCallee fn = module_->getOrInsertFunction("fclose", ft);
            if (!fp->getType()->isPointerTy())
                fp = builder_->CreateIntToPtr(fp, i8p);
            llvm::Value *result = builder_->CreateSExt(builder_->CreateCall(fn, {fp}, "fclose.ret"), llvm::Type::getInt64Ty(*context_));
            if (inst->result.has_value())
                context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *LLVMCodeGen::emitFFIFwrite(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *str = resolveOperand(inst->operands[0]);
            llvm::Value *fp = resolveOperand(inst->operands[1]);
            if (!str || !fp)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), {i8p, i8p}, false);
            llvm::FunctionCallee fn = module_->getOrInsertFunction("fputs", ft);
            if (!str->getType()->isPointerTy())
                str = builder_->CreateIntToPtr(str, i8p);
            if (!fp->getType()->isPointerTy())
                fp = builder_->CreateIntToPtr(fp, i8p);
            llvm::Value *result = builder_->CreateSExt(builder_->CreateCall(fn, {str, fp}, "fputs.ret"), llvm::Type::getInt64Ty(*context_));
            if (inst->result.has_value())
                context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *LLVMCodeGen::emitFFIFread(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
                return nullptr;
            llvm::Value *buf = resolveOperand(inst->operands[0]);
            llvm::Value *sz = resolveOperand(inst->operands[1]);
            llvm::Value *fp = resolveOperand(inst->operands[2]);
            if (!buf || !sz || !fp)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8p, {i8p, llvm::Type::getInt32Ty(*context_), i8p}, false);
            llvm::FunctionCallee fn = module_->getOrInsertFunction("fgets", ft);
            if (!buf->getType()->isPointerTy())
                buf = builder_->CreateIntToPtr(buf, i8p);
            if (!fp->getType()->isPointerTy())
                fp = builder_->CreateIntToPtr(fp, i8p);
            llvm::Value *sz32 = builder_->CreateIntCast(sz, llvm::Type::getInt32Ty(*context_), false);
            llvm::Value *result = builder_->CreateCall(fn, {buf, sz32, fp}, "fgets.ret");
            if (inst->result.has_value())
                context_info_.namedValues[inst->result->name] = result;
            return result;
        }

    } // namespace LLVM
} // namespace Sad
