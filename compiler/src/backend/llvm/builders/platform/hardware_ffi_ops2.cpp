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
#include "builders/platform/hardware_ffi_codegen.h"
#include "sad_dyn_repr.h" // (AR) ز.٣٧: توزيعُ «طول» على وسمِ القيمةِ الديناميّة
#include "builtin_registry.h" // (AR) اسمُ «طول» من السجلِّ المولَّد لا حرفيًّا
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

        llvm::Value *HardwareFFICodeGen::emitDmaInit(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 4)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "dma_init"}});
                return nullptr;
            }
            llvm::Value *ch = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *src = cg_.resolveOperand(inst->operands[1]);
            llvm::Value *dest = cg_.resolveOperand(inst->operands[2]);
            llvm::Value *cnt = cg_.resolveOperand(inst->operands[3]);
            if (!ch || !src || !dest || !cnt)
                return nullptr;
            llvm::Type *i16 = llvm::Type::getInt16Ty(*cg_.context_);
            llvm::Type *i8 = llvm::Type::getInt8Ty(*cg_.context_);
            llvm::FunctionType *outbFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {i8, i16}, false);
            llvm::InlineAsm *outbIA = llvm::InlineAsm::get(outbFT, "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
            llvm::Value *ch8 = cg_.builder_->CreateIntCast(ch, i8, false);
            cg_.builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {cg_.builder_->CreateOr(ch8, llvm::ConstantInt::get(i8, 0x04)), llvm::ConstantInt::get(i16, 0x0A)});
            cg_.builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {llvm::ConstantInt::get(i8, 0x00), llvm::ConstantInt::get(i16, 0x0C)});
            llvm::Value *addrPort = cg_.builder_->CreateMul(cg_.builder_->CreateIntCast(ch, i16, false), llvm::ConstantInt::get(i16, 2));
            cg_.builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {cg_.builder_->CreateIntCast(src, i8, false), addrPort});
            cg_.builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {cg_.builder_->CreateIntCast(cg_.builder_->CreateLShr(src, 8), i8, false), addrPort});
            llvm::Value *cntPort = cg_.builder_->CreateAdd(addrPort, llvm::ConstantInt::get(i16, 1));
            llvm::Value *cnt1 = cg_.builder_->CreateSub(cnt, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 1));
            cg_.builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {cg_.builder_->CreateIntCast(cnt1, i8, false), cntPort});
            cg_.builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {cg_.builder_->CreateIntCast(cg_.builder_->CreateLShr(cnt1, 8), i8, false), cntPort});
            cg_.builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {ch8, llvm::ConstantInt::get(i16, 0x0A)});
            // (AR) قيمة إشاريّة «عُولجت»: تعليمة void بلا قيمة، وإرجاع nullptr يُسقط
            //      الموزّع المتدرّج عبر الطبقات فيطبع «Unsupported opcode» بائتًا
            //      (نمط emitMemWrite نفسه — إصلاح جماعيّ لعائلة معالجات void).
            // (EN) "Handled" sentinel: void instruction with no value; returning
            //      nullptr makes the tiered dispatcher fall through and misreport
            //      "Unsupported opcode". Same pattern as emitMemWrite.
            return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0);
        }

        llvm::Value *HardwareFFICodeGen::emitDmaStart(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "dma_start"}});
                return nullptr;
            }
            llvm::Value *ch = cg_.resolveOperand(inst->operands[0]);
            if (!ch)
                return nullptr;
            llvm::Type *i16 = llvm::Type::getInt16Ty(*cg_.context_);
            llvm::Type *i8 = llvm::Type::getInt8Ty(*cg_.context_);
            llvm::FunctionType *outbFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {i8, i16}, false);
            llvm::InlineAsm *outbIA = llvm::InlineAsm::get(outbFT, "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
            llvm::Value *ch8 = cg_.builder_->CreateIntCast(ch, i8, false);
            cg_.builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {cg_.builder_->CreateOr(ch8, llvm::ConstantInt::get(i8, 0x48)), llvm::ConstantInt::get(i16, 0x0B)});
            cg_.builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {ch8, llvm::ConstantInt::get(i16, 0x0A)});
            // (AR) قيمة إشاريّة «عُولجت»: تعليمة void بلا قيمة، وإرجاع nullptr يُسقط
            //      الموزّع المتدرّج عبر الطبقات فيطبع «Unsupported opcode» بائتًا
            //      (نمط emitMemWrite نفسه — إصلاح جماعيّ لعائلة معالجات void).
            // (EN) "Handled" sentinel: void instruction with no value; returning
            //      nullptr makes the tiered dispatcher fall through and misreport
            //      "Unsupported opcode". Same pattern as emitMemWrite.
            return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0);
        }

        // ============================================================================
        // FFI Emit Methods (20)
        // ============================================================================

        llvm::Value *HardwareFFICodeGen::emitFFIPrintf(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "printf"}});
                return nullptr;
            }
            llvm::FunctionType *printfType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*cg_.context_), {llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo()}, true);
            llvm::FunctionCallee printfFunc = cg_.module_->getOrInsertFunction("printf", printfType);
            std::vector<llvm::Value *> args;
            for (auto &op : inst->operands)
            {
                llvm::Value *v = cg_.resolveOperand(op);
                if (v)
                    args.push_back(v);
            }
            if (args.empty())
                return nullptr;
            llvm::Value *result = cg_.builder_->CreateCall(printfFunc, args, "printf.ret");
            llvm::Value *ext = cg_.builder_->CreateSExt(result, llvm::Type::getInt64Ty(*cg_.context_));
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = ext;
            return ext;
        }

        llvm::Value *HardwareFFICodeGen::emitFFIMalloc(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "malloc"}});
                return nullptr;
            }
            llvm::Value *size = cg_.resolveOperand(inst->operands[0]);
            if (!size)
                return nullptr;

            llvm::Value *result = cg_.emitMalloc(size, "malloc.ptr");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *HardwareFFICodeGen::emitFFIFree(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "free"}});
                return nullptr;
            }
            llvm::Value *ptr = cg_.resolveOperand(inst->operands[0]);
            if (!ptr)
                return nullptr;

            if (!ptr->getType()->isPointerTy())
                ptr = cg_.builder_->CreateIntToPtr(ptr, llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo());
            cg_.emitFreeCall(ptr);
            // (AR) قيمة إشاريّة «عُولجت»: تعليمة void بلا قيمة، وإرجاع nullptr يُسقط
            //      الموزّع المتدرّج عبر الطبقات فيطبع «Unsupported opcode» بائتًا
            //      (نمط emitMemWrite نفسه — إصلاح جماعيّ لعائلة معالجات void).
            // (EN) "Handled" sentinel: void instruction with no value; returning
            //      nullptr makes the tiered dispatcher fall through and misreport
            //      "Unsupported opcode". Same pattern as emitMemWrite.
            return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0);
        }

        llvm::Value *HardwareFFICodeGen::emitFFIRealloc(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "realloc"}});
                return nullptr;
            }
            llvm::Value *ptr = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *size = cg_.resolveOperand(inst->operands[1]);
            if (!ptr || !size)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();

            if (!ptr->getType()->isPointerTy())
                ptr = cg_.builder_->CreateIntToPtr(ptr, i8p);
            llvm::Value *result = cg_.emitRealloc(ptr, size, "realloc.ptr");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *HardwareFFICodeGen::emitFFICalloc(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "calloc"}});
                return nullptr;
            }
            llvm::Value *count = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *size = cg_.resolveOperand(inst->operands[1]);
            if (!count || !size)
                return nullptr;
            llvm::Value *result = cg_.emitCalloc(count, size, "calloc.ptr");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
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
        llvm::Function *HardwareFFICodeGen::getOrCreateUtf8Strlen()
        {
            // (AR) إذا الدالة موجودة مسبقاً، أرجعها
            // (EN) If function already exists, return it
            llvm::Function *existing = cg_.module_->getFunction("__sad_utf8_strlen");
            if (existing && !existing->empty())
                return existing;

            // (AR) بناء الدالة: i64 __sad_utf8_strlen(i8* str)
            // (EN) Build function: i64 __sad_utf8_strlen(i8* str)
            llvm::Type *i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            llvm::Type *i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            llvm::Type *i8p = i8Ty->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i64Ty, {i8p}, false);
            llvm::Function *fn = llvm::Function::Create(
                ft, llvm::Function::InternalLinkage, "__sad_utf8_strlen", cg_.module_.get());

            // (AR) حفظ نقطة الإدراج الحالية واستعادتها لاحقاً
            // (EN) Save current insert point and restore it later
            llvm::BasicBlock *savedBB = cg_.builder_->GetInsertBlock();
            llvm::BasicBlock::iterator savedPoint;
            bool hasSavedPoint = false;
            if (savedBB)
            {
                savedPoint = cg_.builder_->GetInsertPoint();
                hasSavedPoint = true;
            }

            // (AR) بناء جسم الدالة
            // (EN) Build function body
            llvm::BasicBlock *entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
            llvm::BasicBlock *loopCheck = llvm::BasicBlock::Create(*cg_.context_, "loop.check", fn);
            llvm::BasicBlock *loopBody = llvm::BasicBlock::Create(*cg_.context_, "loop.body", fn);
            llvm::BasicBlock *countChar = llvm::BasicBlock::Create(*cg_.context_, "count.char", fn);
            llvm::BasicBlock *loopInc = llvm::BasicBlock::Create(*cg_.context_, "loop.inc", fn);
            llvm::BasicBlock *exitBB = llvm::BasicBlock::Create(*cg_.context_, "exit", fn);

            llvm::Argument *strArg = fn->arg_begin();
            strArg->setName("str");

            // entry: idx=0, count=0, null check
            cg_.builder_->SetInsertPoint(entry);
            llvm::Value *nullCheck = cg_.builder_->CreateICmpEQ(strArg,
                                                            llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(i8p)), "is.null");
            cg_.builder_->CreateCondBr(nullCheck, exitBB, loopCheck);

            // loop.check: load byte, check if null terminator
            cg_.builder_->SetInsertPoint(loopCheck);
            llvm::PHINode *idxPhi = cg_.builder_->CreatePHI(i64Ty, 2, "idx");
            llvm::PHINode *countPhi = cg_.builder_->CreatePHI(i64Ty, 2, "count");
            idxPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
            countPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);

            llvm::Value *bytePtr = cg_.builder_->CreateGEP(i8Ty, strArg, {idxPhi}, "byte.ptr");
            llvm::Value *byte = cg_.builder_->CreateLoad(i8Ty, bytePtr, "byte");
            llvm::Value *isNull = cg_.builder_->CreateICmpEQ(byte,
                                                         llvm::ConstantInt::get(i8Ty, 0), "is.null.term");
            cg_.builder_->CreateCondBr(isNull, exitBB, loopBody);

            // loop.body: check if continuation byte (byte & 0xC0) == 0x80
            cg_.builder_->SetInsertPoint(loopBody);
            llvm::Value *masked = cg_.builder_->CreateAnd(byte,
                                                      llvm::ConstantInt::get(i8Ty, 0xC0), "masked");
            llvm::Value *isCont = cg_.builder_->CreateICmpEQ(masked,
                                                         llvm::ConstantInt::get(i8Ty, 0x80), "is.cont");
            cg_.builder_->CreateCondBr(isCont, loopInc, countChar);

            // count.char: increment count (not a continuation byte = start of new char)
            cg_.builder_->SetInsertPoint(countChar);
            llvm::Value *newCount = cg_.builder_->CreateAdd(countPhi,
                                                        llvm::ConstantInt::get(i64Ty, 1), "new.count");
            cg_.builder_->CreateBr(loopInc);

            // loop.inc: increment idx
            cg_.builder_->SetInsertPoint(loopInc);
            llvm::PHINode *countMerge = cg_.builder_->CreatePHI(i64Ty, 2, "count.merge");
            countMerge->addIncoming(countPhi, loopBody);  // continuation byte, count unchanged
            countMerge->addIncoming(newCount, countChar); // new char, count incremented
            llvm::Value *nextIdx = cg_.builder_->CreateAdd(idxPhi,
                                                       llvm::ConstantInt::get(i64Ty, 1), "next.idx");
            cg_.builder_->CreateBr(loopCheck);

            idxPhi->addIncoming(nextIdx, loopInc);
            countPhi->addIncoming(countMerge, loopInc);

            // exit: return count
            cg_.builder_->SetInsertPoint(exitBB);
            llvm::PHINode *retPhi = cg_.builder_->CreatePHI(i64Ty, 2, "ret.count");
            retPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry); // null string
            retPhi->addIncoming(countPhi, loopCheck);                     // normal termination
            cg_.builder_->CreateRet(retPhi);

            // (AR) استعادة نقطة الإدراج الأصلية
            // (EN) Restore original insert point
            if (hasSavedPoint && savedBB)
            {
                cg_.builder_->SetInsertPoint(savedBB, savedPoint);
            }

            return fn;
        }

        // ================================================================
        // (AR) تحويل موقع البايت إلى فهرس الحرف UTF-8
        // (EN) Convert byte offset to UTF-8 character index
        // الدالة: i64 __sad_utf8_byte_to_char(i8* str, i64 byte_offset)
        // تعد عدد الحروف (غير بايتات المتابعة) في أول byte_offset بايت من str
        // ================================================================
        llvm::Function *HardwareFFICodeGen::getOrCreateUtf8ByteToChar()
        {
            llvm::Function *existing = cg_.module_->getFunction("__sad_utf8_byte_to_char");
            if (existing && !existing->empty())
                return existing;

            llvm::Type *i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            llvm::Type *i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            llvm::Type *i8p = i8Ty->getPointerTo();

            llvm::FunctionType *ft = llvm::FunctionType::get(i64Ty, {i8p, i64Ty}, false);
            llvm::Function *fn = llvm::Function::Create(
                ft, llvm::Function::InternalLinkage, "__sad_utf8_byte_to_char", cg_.module_.get());

            // (AR) حفظ نقطة الإدراج الحالية
            llvm::BasicBlock *savedBB = cg_.builder_->GetInsertBlock();
            llvm::BasicBlock::iterator savedPoint;
            bool hasSavedPoint = false;
            if (savedBB)
            {
                savedPoint = cg_.builder_->GetInsertPoint();
                hasSavedPoint = true;
            }

            // (AR) بناء الكتل
            llvm::BasicBlock *entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
            llvm::BasicBlock *loopCheck = llvm::BasicBlock::Create(*cg_.context_, "loop.check", fn);
            llvm::BasicBlock *loopBody = llvm::BasicBlock::Create(*cg_.context_, "loop.body", fn);
            llvm::BasicBlock *countChar = llvm::BasicBlock::Create(*cg_.context_, "count.char", fn);
            llvm::BasicBlock *loopInc = llvm::BasicBlock::Create(*cg_.context_, "loop.inc", fn);
            llvm::BasicBlock *exitBB = llvm::BasicBlock::Create(*cg_.context_, "exit", fn);

            auto argIt = fn->arg_begin();
            llvm::Argument *strArg = &*argIt++;
            llvm::Argument *byteOffsetArg = &*argIt;
            strArg->setName("str");
            byteOffsetArg->setName("byte_offset");

            // entry: فحص أن byte_offset > 0
            cg_.builder_->SetInsertPoint(entry);
            llvm::Value *cmpZero = cg_.builder_->CreateICmpSLE(byteOffsetArg,
                                                           llvm::ConstantInt::get(i64Ty, 0), "cmp.zero");
            cg_.builder_->CreateCondBr(cmpZero, exitBB, loopCheck);

            // loop.check: هل وصلنا byte_offset؟
            cg_.builder_->SetInsertPoint(loopCheck);
            llvm::PHINode *idxPhi = cg_.builder_->CreatePHI(i64Ty, 2, "idx");
            llvm::PHINode *countPhi = cg_.builder_->CreatePHI(i64Ty, 2, "count");
            idxPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
            countPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);

            llvm::Value *cmpEnd = cg_.builder_->CreateICmpSLT(idxPhi, byteOffsetArg, "cmp.end");
            cg_.builder_->CreateCondBr(cmpEnd, loopBody, exitBB);

            // loop.body: فحص هل البايت بايت متابعة (continuation byte)
            cg_.builder_->SetInsertPoint(loopBody);
            llvm::Value *bytePtr = cg_.builder_->CreateGEP(i8Ty, strArg, {idxPhi}, "byte.ptr");
            llvm::Value *byte = cg_.builder_->CreateLoad(i8Ty, bytePtr, "byte");
            llvm::Value *masked = cg_.builder_->CreateAnd(byte,
                                                      llvm::ConstantInt::get(i8Ty, 0xC0), "masked");
            llvm::Value *isCont = cg_.builder_->CreateICmpEQ(masked,
                                                         llvm::ConstantInt::get(i8Ty, 0x80), "is.cont");
            cg_.builder_->CreateCondBr(isCont, loopInc, countChar);

            // count.char: ليس بايت متابعة → حرف جديد
            cg_.builder_->SetInsertPoint(countChar);
            llvm::Value *newCount = cg_.builder_->CreateAdd(countPhi,
                                                        llvm::ConstantInt::get(i64Ty, 1), "new.count");
            cg_.builder_->CreateBr(loopInc);

            // loop.inc: التقدم
            cg_.builder_->SetInsertPoint(loopInc);
            llvm::PHINode *countMerge = cg_.builder_->CreatePHI(i64Ty, 2, "count.merge");
            countMerge->addIncoming(countPhi, loopBody);
            countMerge->addIncoming(newCount, countChar);
            llvm::Value *nextIdx = cg_.builder_->CreateAdd(idxPhi,
                                                       llvm::ConstantInt::get(i64Ty, 1), "next.idx");
            cg_.builder_->CreateBr(loopCheck);

            idxPhi->addIncoming(nextIdx, loopInc);
            countPhi->addIncoming(countMerge, loopInc);

            // exit: إرجاع العداد
            cg_.builder_->SetInsertPoint(exitBB);
            llvm::PHINode *retPhi = cg_.builder_->CreatePHI(i64Ty, 2, "ret.count");
            retPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
            retPhi->addIncoming(countPhi, loopCheck);
            cg_.builder_->CreateRet(retPhi);

            // (AR) استعادة نقطة الإدراج
            if (hasSavedPoint && savedBB)
                cg_.builder_->SetInsertPoint(savedBB, savedPoint);

            return fn;
        }

        // ================================================================
        // (AR) تحويل فهرس الحرف UTF-8 إلى موقع البايت
        // (EN) Convert UTF-8 character index to byte offset
        // الدالة: i64 __sad_utf8_char_to_byte(i8* str, i64 char_index)
        // تجد موقع البايت لبداية الحرف رقم char_index في str
        // ================================================================
        llvm::Function *HardwareFFICodeGen::getOrCreateUtf8CharToByte()
        {
            llvm::Function *existing = cg_.module_->getFunction("__sad_utf8_char_to_byte");
            if (existing && !existing->empty())
                return existing;

            llvm::Type *i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            llvm::Type *i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            llvm::Type *i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            llvm::Type *i8p = i8Ty->getPointerTo();

            llvm::FunctionType *ft = llvm::FunctionType::get(i64Ty, {i8p, i64Ty}, false);
            llvm::Function *fn = llvm::Function::Create(
                ft, llvm::Function::InternalLinkage, "__sad_utf8_char_to_byte", cg_.module_.get());

            // (AR) حفظ نقطة الإدراج الحالية
            llvm::BasicBlock *savedBB = cg_.builder_->GetInsertBlock();
            llvm::BasicBlock::iterator savedPoint;
            bool hasSavedPoint = false;
            if (savedBB)
            {
                savedPoint = cg_.builder_->GetInsertPoint();
                hasSavedPoint = true;
            }

            // (AR) بناء الكتل
            llvm::BasicBlock *entry = llvm::BasicBlock::Create(*cg_.context_, "entry", fn);
            llvm::BasicBlock *loopCheck = llvm::BasicBlock::Create(*cg_.context_, "loop.check", fn);
            llvm::BasicBlock *readByte = llvm::BasicBlock::Create(*cg_.context_, "read.byte", fn);
            llvm::BasicBlock *classify = llvm::BasicBlock::Create(*cg_.context_, "classify", fn);
            llvm::BasicBlock *advance = llvm::BasicBlock::Create(*cg_.context_, "advance", fn);
            llvm::BasicBlock *exitBB = llvm::BasicBlock::Create(*cg_.context_, "exit", fn);

            auto argIt = fn->arg_begin();
            llvm::Argument *strArg = &*argIt++;
            llvm::Argument *charIndexArg = &*argIt;
            strArg->setName("str");
            charIndexArg->setName("char_index");

            // entry: فحص char_index <= 0
            cg_.builder_->SetInsertPoint(entry);
            llvm::Value *cmpZero = cg_.builder_->CreateICmpSLE(charIndexArg,
                                                           llvm::ConstantInt::get(i64Ty, 0), "cmp.zero");
            cg_.builder_->CreateCondBr(cmpZero, exitBB, loopCheck);

            // loop.check: هل عددنا الحروف الكافية؟
            cg_.builder_->SetInsertPoint(loopCheck);
            llvm::PHINode *bytePosP = cg_.builder_->CreatePHI(i64Ty, 2, "byte.pos");
            llvm::PHINode *charsP = cg_.builder_->CreatePHI(i64Ty, 2, "chars");
            bytePosP->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);
            charsP->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry);

            llvm::Value *reached = cg_.builder_->CreateICmpSGE(charsP, charIndexArg, "reached");
            cg_.builder_->CreateCondBr(reached, exitBB, readByte);

            // read.byte: قراءة البايت الأول من الحرف الحالي
            cg_.builder_->SetInsertPoint(readByte);
            llvm::Value *bp = cg_.builder_->CreateGEP(i8Ty, strArg, {bytePosP}, "bp");
            llvm::Value *b = cg_.builder_->CreateLoad(i8Ty, bp, "b");
            llvm::Value *isNullTerm = cg_.builder_->CreateICmpEQ(b,
                                                             llvm::ConstantInt::get(i8Ty, 0), "is.null");
            cg_.builder_->CreateCondBr(isNullTerm, exitBB, classify);

            // classify: تحديد عرض الحرف UTF-8 (1/2/3/4 بايتات)
            cg_.builder_->SetInsertPoint(classify);
            llvm::Value *bExt = cg_.builder_->CreateZExt(b, i32Ty, "b.ext");
            // (AR) إذا >= 0xF0 → 4 بايتات، >= 0xE0 → 3، >= 0xC0 → 2، غير ذلك → 1
            llvm::Value *is4 = cg_.builder_->CreateICmpUGE(bExt, llvm::ConstantInt::get(i32Ty, 0xF0), "is4");
            llvm::Value *is3 = cg_.builder_->CreateICmpUGE(bExt, llvm::ConstantInt::get(i32Ty, 0xE0), "is3");
            llvm::Value *is2 = cg_.builder_->CreateICmpUGE(bExt, llvm::ConstantInt::get(i32Ty, 0xC0), "is2");
            // (AR) اختيار العرض: 4 > 3 > 2 > 1
            llvm::Value *w = cg_.builder_->CreateSelect(is4, llvm::ConstantInt::get(i64Ty, 4),
                                                    cg_.builder_->CreateSelect(is3, llvm::ConstantInt::get(i64Ty, 3),
                                                                           cg_.builder_->CreateSelect(is2, llvm::ConstantInt::get(i64Ty, 2),
                                                                                                  llvm::ConstantInt::get(i64Ty, 1))));
            cg_.builder_->CreateBr(advance);

            // advance: التقدم بالبايت والحرف
            cg_.builder_->SetInsertPoint(advance);
            llvm::Value *newBytePos = cg_.builder_->CreateAdd(bytePosP, w, "new.byte.pos");
            llvm::Value *newChars = cg_.builder_->CreateAdd(charsP,
                                                        llvm::ConstantInt::get(i64Ty, 1), "new.chars");
            cg_.builder_->CreateBr(loopCheck);

            bytePosP->addIncoming(newBytePos, advance);
            charsP->addIncoming(newChars, advance);

            // exit: إرجاع موقع البايت
            cg_.builder_->SetInsertPoint(exitBB);
            llvm::PHINode *retPhi = cg_.builder_->CreatePHI(i64Ty, 3, "ret.byte.pos");
            retPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entry); // char_index <= 0
            retPhi->addIncoming(bytePosP, loopCheck);                     // وصلنا العدد المطلوب
            retPhi->addIncoming(bytePosP, readByte);                      // نهاية النص (null)
            cg_.builder_->CreateRet(retPhi);

            // (AR) استعادة نقطة الإدراج
            if (hasSavedPoint && savedBB)
                cg_.builder_->SetInsertPoint(savedBB, savedPoint);

            return fn;
        }

        llvm::Value *HardwareFFICodeGen::emitFFIStrlen(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *str = cg_.resolveOperand(inst->operands[0]);
            if (!str)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();

            // (AR) ز.٣٧: «طول» على قيمةٍ ديناميّة. الأماميّةُ لا تعرف الوسمَ زمنَ الترجمة
            //      فتختار STRING_LEN، وكان %SadDyn يصل هنا فيُكسَر بـIntToPtr على بنيةٍ
            //      (‏"Invalid cast!") ⇒ انهيارُ المصرّف. الوسمُ لا يُعرَف إلّا زمنَ التشغيل،
            //      فالتوزيعُ هنا لا هناك: مصفوفةٌ ⇒ حقلُ الطول، وما عداها ⇒ عدُّ UTF-8،
            //      وهو ما يفعله المفسّر بالضبط.
            // (EN) ز.٣٧: `طول` on a dynamic value. The frontend cannot know the tag at
            //      compile time so it picks STRING_LEN, and a %SadDyn used to reach here and
            //      break on IntToPtr over a struct ("Invalid cast!") ⇒ compiler crash. The
            //      tag is only known at runtime, so dispatch belongs here, not there: an
            //      array ⇒ its length field, anything else ⇒ UTF-8 count — exactly what the
            //      interpreter does.
            if (isSadDyn(str))
            {
                auto &b = *cg_.builder_;
                auto &ctx = *cg_.context_;
                auto *i64Ty = llvm::Type::getInt64Ty(ctx);
                llvm::Value *kind = dynKindByte(cg_, str);
                llvm::Value *payload = dynPayloadI64(cg_, str);

                auto *parent = b.GetInsertBlock()->getParent();
                auto *arrayBB = llvm::BasicBlock::Create(ctx, "dyn.len.array", parent);
                auto *strBB = llvm::BasicBlock::Create(ctx, "dyn.len.str", parent);
                auto *mergeBB = llvm::BasicBlock::Create(ctx, "dyn.len.merge", parent);

                llvm::Value *isArray = b.CreateICmpEQ(
                    kind,
                    llvm::ConstantInt::get(llvm::Type::getInt8Ty(ctx), DynKind::Array),
                    "dyn.len.isarray");
                b.CreateCondBr(isArray, arrayBB, strBB);

                b.SetInsertPoint(arrayBB);
                llvm::StructType *arrTy = sadArrayStructType(ctx);
                llvm::Value *arrPtr = b.CreateIntToPtr(payload, i8p, "dyn.len.arrptr");
                llvm::Value *lenPtr = b.CreateStructGEP(arrTy, arrPtr, 0, "dyn.len.lenptr");
                llvm::Value *arrLen = b.CreateLoad(i64Ty, lenPtr, "dyn.len.arrlen");
                b.CreateBr(mergeBB);
                arrayBB = b.GetInsertBlock();

                b.SetInsertPoint(strBB);
                // (AR) بابُ **العمليّة** لا بابُ العرض: المفسّرُ يرفعُ RUN033 على `.طول`
                //      لقيمةٍ عدميّة، فلا يجوز أن يُجيب المترجّمُ بطولِ لفظِ «لاشيء».
                //      (كان هذا السطرُ موصولًا ببابِ العرضِ خطأً، فنُقِض بعد القياس.)
                // (EN) Operation door, not the display door: the interpreter raises RUN033
                //      for «.طول» on null, so answering with the null word's length would
                //      be a fabricated result.
                llvm::Value *strPtr = cg_.emitStringPtrOrRaise(
                    payload,
                    LLVMCodeGen::stringMethodOperationLabel(
                        Sad::Builtins::Names::TypeMethods::String::LENGTH),
                    "dyn.len.str");
                llvm::Value *strLen =
                    b.CreateCall(getOrCreateUtf8Strlen(), {strPtr}, "dyn.len.strlen");
                b.CreateBr(mergeBB);
                strBB = b.GetInsertBlock();

                b.SetInsertPoint(mergeBB);
                auto *phi = b.CreatePHI(i64Ty, 2, "dyn.len.result");
                phi->addIncoming(arrLen, arrayBB);
                phi->addIncoming(strLen, strBB);
                if (inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = phi;
                return phi;
            }

            // (AR) 🔑 بابُ العمليّة: هذا هو المسارُ **غيرُ الديناميّ** الذي تسلكه
            //      «س.طول» على خانةٍ نصّيّةٍ مصرَّحة. كان `inttoptr` خامًّا ثمّ عدًّا،
            //      فأعطى `0` لقيمةٍ عدميّةٍ بينما المفسّرُ يرفعُ RUN033 (مقيس).
            //      وحراستي الأولى وقعت على ذراعِ %SadDyn وحدَها فلم تبلغ هذا المسار —
            //      اقرأ المسارَ المأخوذَ فعلًا لا المسارَ المشابه.
            // (EN) Operation door on the NON-dynamic path, which is what «س.طول» on a
            //      declared string slot actually takes: a raw inttoptr then a count,
            //      answering 0 for null while the interpreter raises RUN033 (measured).
            str = cg_.emitStringPtrOrRaise(
                str,
                LLVMCodeGen::stringMethodOperationLabel(
                    Sad::Builtins::Names::TypeMethods::String::LENGTH),
                "strlen.src");

            // (AR) استخدام __sad_utf8_strlen بدلاً من strlen لعد حروف UTF-8
            // (EN) Use __sad_utf8_strlen instead of strlen to count UTF-8 characters
            llvm::Function *utf8Fn = getOrCreateUtf8Strlen();
            llvm::Value *result = cg_.builder_->CreateCall(utf8Fn, {str}, "utf8len.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *HardwareFFICodeGen::emitFFIStrcpy(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *dst = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *src = cg_.resolveOperand(inst->operands[1]);
            if (!dst || !src)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8p, {i8p, i8p}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("strcpy", ft);
            if (!dst->getType()->isPointerTy())
                dst = cg_.builder_->CreateIntToPtr(dst, i8p);
            if (!src->getType()->isPointerTy())
                src = cg_.builder_->CreateIntToPtr(src, i8p);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {dst, src}, "strcpy.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *HardwareFFICodeGen::emitFFIStrcmp(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *s1 = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *s2 = cg_.resolveOperand(inst->operands[1]);
            if (!s1 || !s2)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*cg_.context_), {i8p, i8p}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("strcmp", ft);
            if (!s1->getType()->isPointerTy())
                s1 = cg_.builder_->CreateIntToPtr(s1, i8p);
            if (!s2->getType()->isPointerTy())
                s2 = cg_.builder_->CreateIntToPtr(s2, i8p);
            llvm::Value *result = cg_.builder_->CreateSExt(cg_.builder_->CreateCall(fn, {s1, s2}, "strcmp.ret"), llvm::Type::getInt64Ty(*cg_.context_));
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *HardwareFFICodeGen::emitFFIStrcat(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *dst = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *src = cg_.resolveOperand(inst->operands[1]);
            if (!dst || !src)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8p, {i8p, i8p}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("strcat", ft);
            if (!dst->getType()->isPointerTy())
                dst = cg_.builder_->CreateIntToPtr(dst, i8p);
            if (!src->getType()->isPointerTy())
                src = cg_.builder_->CreateIntToPtr(src, i8p);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {dst, src}, "strcat.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *HardwareFFICodeGen::emitFFIMemcpy(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
                return nullptr;
            llvm::Value *dst = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *src = cg_.resolveOperand(inst->operands[1]);
            llvm::Value *sz = cg_.resolveOperand(inst->operands[2]);
            if (!dst || !src || !sz)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            // (AR) طول ‎mem*‎ بنوع ‎size_t‎ الهدف (i32 على 32-بت): الخلفيّة تولّد
            //      النداء المكتبيّ بهذا العرض، وتعريف وقت التشغيل الحرّ يطابقه.
            llvm::Type *szTy = cg_.getSizeType();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8p, {i8p, i8p, szTy}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("memcpy", ft);
            sz = cg_.builder_->CreateZExtOrTrunc(sz, szTy, "memcpy.size.sz");
            if (!dst->getType()->isPointerTy())
                dst = cg_.builder_->CreateIntToPtr(dst, i8p);
            if (!src->getType()->isPointerTy())
                src = cg_.builder_->CreateIntToPtr(src, i8p);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {dst, src, sz}, "memcpy.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *HardwareFFICodeGen::emitFFIMemset(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
                return nullptr;
            llvm::Value *dst = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *val = cg_.resolveOperand(inst->operands[1]);
            llvm::Value *sz = cg_.resolveOperand(inst->operands[2]);
            if (!dst || !val || !sz)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            // (AR) طول ‎mem*‎ بنوع ‎size_t‎ الهدف (i32 على 32-بت): الخلفيّة تولّد
            //      النداء المكتبيّ بهذا العرض، وتعريف وقت التشغيل الحرّ يطابقه.
            llvm::Type *szTy = cg_.getSizeType();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8p, {i8p, llvm::Type::getInt32Ty(*cg_.context_), szTy}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("memset", ft);
            sz = cg_.builder_->CreateZExtOrTrunc(sz, szTy, "memset.size.sz");
            if (!dst->getType()->isPointerTy())
                dst = cg_.builder_->CreateIntToPtr(dst, i8p);
            llvm::Value *val32 = cg_.builder_->CreateIntCast(val, llvm::Type::getInt32Ty(*cg_.context_), false);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {dst, val32, sz}, "memset.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *HardwareFFICodeGen::emitFFIFopen(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *name = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *mode = cg_.resolveOperand(inst->operands[1]);
            if (!name || !mode)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8p, {i8p, i8p}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("fopen", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {name, mode}, "fopen.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *HardwareFFICodeGen::emitFFIFclose(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *fp = cg_.resolveOperand(inst->operands[0]);
            if (!fp)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*cg_.context_), {i8p}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("fclose", ft);
            if (!fp->getType()->isPointerTy())
                fp = cg_.builder_->CreateIntToPtr(fp, i8p);
            llvm::Value *result = cg_.builder_->CreateSExt(cg_.builder_->CreateCall(fn, {fp}, "fclose.ret"), llvm::Type::getInt64Ty(*cg_.context_));
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *HardwareFFICodeGen::emitFFIFwrite(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *str = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *fp = cg_.resolveOperand(inst->operands[1]);
            if (!str || !fp)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*cg_.context_), {i8p, i8p}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("fputs", ft);
            if (!str->getType()->isPointerTy())
                str = cg_.builder_->CreateIntToPtr(str, i8p);
            if (!fp->getType()->isPointerTy())
                fp = cg_.builder_->CreateIntToPtr(fp, i8p);
            llvm::Value *result = cg_.builder_->CreateSExt(cg_.builder_->CreateCall(fn, {str, fp}, "fputs.ret"), llvm::Type::getInt64Ty(*cg_.context_));
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *HardwareFFICodeGen::emitFFIFread(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
                return nullptr;
            llvm::Value *buf = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *sz = cg_.resolveOperand(inst->operands[1]);
            llvm::Value *fp = cg_.resolveOperand(inst->operands[2]);
            if (!buf || !sz || !fp)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8p, {i8p, llvm::Type::getInt32Ty(*cg_.context_), i8p}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("fgets", ft);
            if (!buf->getType()->isPointerTy())
                buf = cg_.builder_->CreateIntToPtr(buf, i8p);
            if (!fp->getType()->isPointerTy())
                fp = cg_.builder_->CreateIntToPtr(fp, i8p);
            llvm::Value *sz32 = cg_.builder_->CreateIntCast(sz, llvm::Type::getInt32Ty(*cg_.context_), false);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {buf, sz32, fp}, "fgets.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

    } // namespace LLVM
} // namespace Sad
