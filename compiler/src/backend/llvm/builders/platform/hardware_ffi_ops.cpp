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

        llvm::Value *HardwareFFICodeGen::emitPortWrite(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "port_write"}});
                return nullptr;
            }
            llvm::Value *port = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *value = cg_.resolveOperand(inst->operands[1]);
            if (!port || !value)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "port_write"}});
                return nullptr;
            }
            llvm::Value *port16 = cg_.builder_->CreateIntCast(port, llvm::Type::getInt16Ty(*cg_.context_), false, "port");
            std::string asmStr;
            llvm::Type *valType;
            if (inst->opcode == SIROpcode::BUILTIN_PORT_WRITE_16)
            {
                valType = llvm::Type::getInt16Ty(*cg_.context_);
                asmStr = "outw $0, $1";
            }
            else if (inst->opcode == SIROpcode::BUILTIN_PORT_WRITE_32)
            {
                valType = llvm::Type::getInt32Ty(*cg_.context_);
                asmStr = "outl $0, $1";
            }
            else
            {
                valType = llvm::Type::getInt8Ty(*cg_.context_);
                asmStr = "outb $0, $1";
            }
            llvm::Value *val = cg_.builder_->CreateIntCast(value, valType, false, "val");
            llvm::FunctionType *ft = llvm::FunctionType::get(
                llvm::Type::getVoidTy(*cg_.context_), {valType, llvm::Type::getInt16Ty(*cg_.context_)}, false);
            llvm::InlineAsm *ia = llvm::InlineAsm::get(
                ft, asmStr, "{ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
            return cg_.builder_->CreateCall(ft, static_cast<llvm::Value *>(ia), {val, port16});
        }

        llvm::Value *HardwareFFICodeGen::emitPortRead(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "port_read"}});
                return nullptr;
            }
            llvm::Value *port = cg_.resolveOperand(inst->operands[0]);
            if (!port)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "port_read"}});
                return nullptr;
            }
            llvm::Value *port16 = cg_.builder_->CreateIntCast(port, llvm::Type::getInt16Ty(*cg_.context_), false, "port");
            std::string asmStr;
            llvm::Type *retType;
            if (inst->opcode == SIROpcode::BUILTIN_PORT_READ_16)
            {
                retType = llvm::Type::getInt16Ty(*cg_.context_);
                asmStr = "inw $1, $0";
            }
            else if (inst->opcode == SIROpcode::BUILTIN_PORT_READ_32)
            {
                retType = llvm::Type::getInt32Ty(*cg_.context_);
                asmStr = "inl $1, $0";
            }
            else
            {
                retType = llvm::Type::getInt8Ty(*cg_.context_);
                asmStr = "inb $1, $0";
            }
            llvm::FunctionType *ft = llvm::FunctionType::get(retType, {llvm::Type::getInt16Ty(*cg_.context_)}, false);
            llvm::InlineAsm *ia = llvm::InlineAsm::get(
                ft, asmStr, "={ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
            llvm::Value *result = cg_.builder_->CreateCall(ft, static_cast<llvm::Value *>(ia), {port16}, "port.read");
            llvm::Value *extended = cg_.builder_->CreateZExt(result, llvm::Type::getInt64Ty(*cg_.context_), "port.read.ext");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = extended;
            return extended;
        }

        llvm::Value *HardwareFFICodeGen::emitMemWrite(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "mem_write"}});
                return nullptr;
            }
            llvm::Value *addr = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *value = cg_.resolveOperand(inst->operands[1]);
            if (!addr || !value)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "mem_write"}});
                return nullptr;
            }
            llvm::Type *valType;
            if (inst->opcode == SIROpcode::BUILTIN_MEM_WRITE_64)
                valType = llvm::Type::getInt64Ty(*cg_.context_);
            else if (inst->opcode == SIROpcode::BUILTIN_MEM_WRITE_32)
                valType = llvm::Type::getInt32Ty(*cg_.context_);
            else if (inst->opcode == SIROpcode::BUILTIN_MEM_WRITE_16)
                valType = llvm::Type::getInt16Ty(*cg_.context_);
            else
                valType = llvm::Type::getInt8Ty(*cg_.context_);
            llvm::Value *ptr = cg_.builder_->CreateIntToPtr(addr, valType->getPointerTo(), "mem.ptr");
            llvm::Value *val = cg_.builder_->CreateIntCast(value, valType, false);
            auto *store = cg_.builder_->CreateStore(val, ptr);
            store->setVolatile(true);
            return nullptr;
        }

        llvm::Value *HardwareFFICodeGen::emitMemRead(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "mem_read"}});
                return nullptr;
            }
            llvm::Value *addr = cg_.resolveOperand(inst->operands[0]);
            if (!addr)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "mem_read"}});
                return nullptr;
            }
            llvm::Type *valType;
            if (inst->opcode == SIROpcode::BUILTIN_MEM_READ_64)
                valType = llvm::Type::getInt64Ty(*cg_.context_);
            else if (inst->opcode == SIROpcode::BUILTIN_MEM_READ_32)
                valType = llvm::Type::getInt32Ty(*cg_.context_);
            else if (inst->opcode == SIROpcode::BUILTIN_MEM_READ_16)
                valType = llvm::Type::getInt16Ty(*cg_.context_);
            else
                valType = llvm::Type::getInt8Ty(*cg_.context_);
            llvm::Value *ptr = cg_.builder_->CreateIntToPtr(addr, valType->getPointerTo(), "mem.ptr");
            auto *load = cg_.builder_->CreateLoad(valType, ptr, "mem.val");
            load->setVolatile(true);
            llvm::Value *result;
            if (inst->opcode == SIROpcode::BUILTIN_MEM_READ_64)
                result = load; // already i64, no extension needed
            else
                result = cg_.builder_->CreateZExt(load, llvm::Type::getInt64Ty(*cg_.context_), "mem.ext");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *HardwareFFICodeGen::emitInterrupt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "interrupt"}});
                return nullptr;
            }
            llvm::Value *intNum = cg_.resolveOperand(inst->operands[0]);
            if (!intNum)
                return nullptr;
            llvm::Value *num8 = cg_.builder_->CreateIntCast(intNum, llvm::Type::getInt8Ty(*cg_.context_), false);
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {llvm::Type::getInt8Ty(*cg_.context_)}, false);
            llvm::InlineAsm *ia = llvm::InlineAsm::get(ft, "int $0", "N,~{dirflag},~{fpsr},~{flags}", true, false);
            cg_.builder_->CreateCall(ft, static_cast<llvm::Value *>(ia), {num8});
            return nullptr;
        }

        llvm::Value *HardwareFFICodeGen::emitHalt(std::shared_ptr<SIRInstruction> inst)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {}, false);
            llvm::InlineAsm *ia = llvm::InlineAsm::get(ft, "hlt", "", true, false);
            cg_.builder_->CreateCall(ft, static_cast<llvm::Value *>(ia), {});
            return nullptr;
        }

        llvm::Value *HardwareFFICodeGen::emitCli(std::shared_ptr<SIRInstruction> inst)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {}, false);
            llvm::InlineAsm *ia = llvm::InlineAsm::get(ft, "cli", "~{memory}", true, false);
            cg_.builder_->CreateCall(ft, static_cast<llvm::Value *>(ia), {});
            return nullptr;
        }

        llvm::Value *HardwareFFICodeGen::emitSti(std::shared_ptr<SIRInstruction> inst)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {}, false);
            llvm::InlineAsm *ia = llvm::InlineAsm::get(ft, "sti", "~{memory}", true, false);
            cg_.builder_->CreateCall(ft, static_cast<llvm::Value *>(ia), {});
            return nullptr;
        }

        llvm::Value *HardwareFFICodeGen::emitVgaWrite(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 4)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "vga_write"}});
                return nullptr;
            }
            llvm::Value *row = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *col = cg_.resolveOperand(inst->operands[1]);
            llvm::Value *ch = cg_.resolveOperand(inst->operands[2]);
            llvm::Value *color = cg_.resolveOperand(inst->operands[3]);
            if (!row || !col || !ch || !color)
                return nullptr;
            llvm::Type *i16 = llvm::Type::getInt16Ty(*cg_.context_);
            llvm::Type *i64 = llvm::Type::getInt64Ty(*cg_.context_);
            llvm::Value *offset = cg_.builder_->CreateAdd(
                cg_.builder_->CreateMul(row, llvm::ConstantInt::get(i64, 80)), col);
            llvm::Value *byteOffset = cg_.builder_->CreateMul(offset, llvm::ConstantInt::get(i64, 2));
            llvm::Value *baseAddr = llvm::ConstantInt::get(i64, 0xB8000);
            llvm::Value *addr = cg_.builder_->CreateAdd(baseAddr, byteOffset);
            llvm::Value *ptr = cg_.builder_->CreateIntToPtr(addr, i16->getPointerTo(), "vga.ptr");
            llvm::Value *charVal = cg_.builder_->CreateIntCast(ch, i16, false);
            llvm::Value *colorVal = cg_.builder_->CreateShl(cg_.builder_->CreateIntCast(color, i16, false), llvm::ConstantInt::get(i16, 8));
            llvm::Value *entry = cg_.builder_->CreateOr(charVal, colorVal, "vga.entry");
            auto *store = cg_.builder_->CreateStore(entry, ptr);
            store->setVolatile(true);
            return nullptr;
        }

        llvm::Value *HardwareFFICodeGen::emitVgaClear(std::shared_ptr<SIRInstruction> inst)
        {
            llvm::Value *color = llvm::ConstantInt::get(llvm::Type::getInt8Ty(*cg_.context_), 0x07);
            if (inst && !inst->operands.empty())
            {
                llvm::Value *c = cg_.resolveOperand(inst->operands[0]);
                if (c)
                    color = cg_.builder_->CreateIntCast(c, llvm::Type::getInt8Ty(*cg_.context_), false);
            }
            llvm::Type *i16 = llvm::Type::getInt16Ty(*cg_.context_);
            llvm::Value *colorWord = cg_.builder_->CreateShl(
                cg_.builder_->CreateZExt(color, i16), llvm::ConstantInt::get(i16, 8));
            llvm::Value *entry = cg_.builder_->CreateOr(colorWord, llvm::ConstantInt::get(i16, 0x20), "blank");
            llvm::Value *base = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0xB8000);
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*cg_.context_, "vga.loop", curFunc);
            llvm::BasicBlock *doneBB = llvm::BasicBlock::Create(*cg_.context_, "vga.done", curFunc);
            llvm::BasicBlock *entryBB = cg_.builder_->GetInsertBlock();
            cg_.builder_->CreateBr(loopBB);
            cg_.builder_->SetInsertPoint(loopBB);
            llvm::PHINode *idx = cg_.builder_->CreatePHI(llvm::Type::getInt64Ty(*cg_.context_), 2, "i");
            idx->addIncoming(llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0), entryBB);
            llvm::Value *byteOff = cg_.builder_->CreateMul(idx, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 2));
            llvm::Value *addr = cg_.builder_->CreateAdd(base, byteOff);
            llvm::Value *ptr = cg_.builder_->CreateIntToPtr(addr, i16->getPointerTo(), "vga.p");
            auto *st = cg_.builder_->CreateStore(entry, ptr);
            st->setVolatile(true);
            llvm::Value *next = cg_.builder_->CreateAdd(idx, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 1));
            idx->addIncoming(next, loopBB);
            llvm::Value *done = cg_.builder_->CreateICmpUGE(next, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 2000));
            cg_.builder_->CreateCondBr(done, doneBB, loopBB);
            cg_.builder_->SetInsertPoint(doneBB);
            return nullptr;
        }

        llvm::Value *HardwareFFICodeGen::emitAddrOf(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "addr_of"}});
                return nullptr;
            }
            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;
            llvm::Value *alloca = cg_.builder_->CreateAlloca(val->getType(), nullptr, "addr.tmp");
            cg_.builder_->CreateStore(val, alloca);
            llvm::Value *result = cg_.builder_->CreatePtrToInt(alloca, llvm::Type::getInt64Ty(*cg_.context_), "addr.val");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *HardwareFFICodeGen::emitMemCopy(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "mem_copy"}});
                return nullptr;
            }
            llvm::Value *dest = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *src = cg_.resolveOperand(inst->operands[1]);
            llvm::Value *size = cg_.resolveOperand(inst->operands[2]);
            if (!dest || !src || !size)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::Value *destPtr = cg_.builder_->CreateIntToPtr(dest, i8p);
            llvm::Value *srcPtr = cg_.builder_->CreateIntToPtr(src, i8p);
            cg_.builder_->CreateMemCpy(destPtr, llvm::MaybeAlign(1), srcPtr, llvm::MaybeAlign(1), size);
            return nullptr;
        }

        llvm::Value *HardwareFFICodeGen::emitMemSet(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "mem_set"}});
                return nullptr;
            }
            llvm::Value *dest = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *val = cg_.resolveOperand(inst->operands[1]);
            llvm::Value *size = cg_.resolveOperand(inst->operands[2]);
            if (!dest || !val || !size)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::Value *destPtr = cg_.builder_->CreateIntToPtr(dest, i8p);
            llvm::Value *val8 = cg_.builder_->CreateIntCast(val, llvm::Type::getInt8Ty(*cg_.context_), false);
            cg_.builder_->CreateMemSet(destPtr, val8, size, llvm::MaybeAlign(1));
            return nullptr;
        }

        // ============================================================================
        // ذاكرة_املأ32 — ملء بكلمات 32-بت عبر REP STOSD
        // mem_fill32(dest, value, count) — fill memory with 32-bit words using REP STOSD
        // ============================================================================
        llvm::Value *HardwareFFICodeGen::emitMemFill32(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "mem_fill32"}});
                return nullptr;
            }
            llvm::Value *dest = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *val = cg_.resolveOperand(inst->operands[1]);
            llvm::Value *count = cg_.resolveOperand(inst->operands[2]);
            if (!dest || !val || !count)
                return nullptr;

            // Convert dest (i64) to pointer
            llvm::Type *i32PtrTy = llvm::Type::getInt32Ty(*cg_.context_)->getPointerTo();
            llvm::Value *destPtr = cg_.builder_->CreateIntToPtr(dest, i32PtrTy);

            // Truncate value to i32 and count to i64
            llvm::Type *i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            llvm::Type *i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            llvm::Value *val32 = cg_.builder_->CreateIntCast(val, i32Ty, false);
            llvm::Value *count64 = cg_.builder_->CreateIntCast(count, i64Ty, false);

            // Generate: cld; rep stosd
            // Input: rdi=dest, eax=value, rcx=count
            // Output: rdi advanced, rcx=0
            // Clobbers: memory, dirflag
            llvm::FunctionType *asmFT = llvm::FunctionType::get(
                llvm::Type::getVoidTy(*cg_.context_),
                {i32PtrTy, i32Ty, i64Ty},
                false);
            llvm::InlineAsm *repStoreSd = llvm::InlineAsm::get(
                asmFT,
                "cld\n\trep stosl",
                "{di},{ax},{cx},~{memory},~{dirflag}",
                true, // hasSideEffects
                false // isAlignStack
            );
            cg_.builder_->CreateCall(asmFT, static_cast<llvm::Value *>(repStoreSd), {destPtr, val32, count64});

            return nullptr;
        }

        // ============================================================================
        // ذاكرة_انسخ32 — نسخ كلمات 32-بت (آمن لـ MMIO)
        // mem_copy32(dest, src, count) — copy count 32-bit words
        // Uses @llvm.memcpy with 4-byte alignment. LLVM chooses the optimal
        // lowering (rep movsb/q, SIMD, etc.) which works correctly for both
        // RAM and MMIO destinations — unlike rep movsl inline asm which hangs
        // QEMU when targeting MMIO framebuffer addresses.
        // ============================================================================
        llvm::Value *HardwareFFICodeGen::emitMemCopy32(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "mem_copy32"}});
                return nullptr;
            }
            llvm::Value *dest = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *src = cg_.resolveOperand(inst->operands[1]);
            llvm::Value *count = cg_.resolveOperand(inst->operands[2]);
            if (!dest || !src || !count)
                return nullptr;

            llvm::Type *i8PtrTy = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::Type *i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            llvm::Value *destPtr = cg_.builder_->CreateIntToPtr(dest, i8PtrTy);
            llvm::Value *srcPtr = cg_.builder_->CreateIntToPtr(src, i8PtrTy);
            llvm::Value *count64 = cg_.builder_->CreateIntCast(count, i64Ty, false);

            // Convert dword count to byte count: size = count * 4
            llvm::Value *byteSize = cg_.builder_->CreateMul(count64, llvm::ConstantInt::get(i64Ty, 4), "mcpy32.bytes");

            // Use @llvm.memcpy with 4-byte alignment for both source and destination
            cg_.builder_->CreateMemCpy(destPtr, llvm::MaybeAlign(4), srcPtr, llvm::MaybeAlign(4), byteSize);

            return nullptr;
        }

        // ============================================================================
        // Embedded Emit Methods (18)
        // ============================================================================

        llvm::Value *HardwareFFICodeGen::emitSerialInit(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "serial_init"}});
                return nullptr;
            }
            llvm::Value *port = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *baud = cg_.resolveOperand(inst->operands[1]);
            if (!port || !baud)
                return nullptr;
            llvm::Type *i16 = llvm::Type::getInt16Ty(*cg_.context_);
            llvm::Type *i8 = llvm::Type::getInt8Ty(*cg_.context_);
            llvm::Value *port16 = cg_.builder_->CreateIntCast(port, i16, false);
            auto doOutb = [&](llvm::Value *val8, llvm::Value *p16)
            {
                llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {i8, i16}, false);
                llvm::InlineAsm *ia = llvm::InlineAsm::get(ft, "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
                cg_.builder_->CreateCall(ft, static_cast<llvm::Value *>(ia), {val8, p16});
            };
            llvm::Value *p1 = cg_.builder_->CreateAdd(port16, llvm::ConstantInt::get(i16, 1));
            llvm::Value *p2 = cg_.builder_->CreateAdd(port16, llvm::ConstantInt::get(i16, 2));
            llvm::Value *p3 = cg_.builder_->CreateAdd(port16, llvm::ConstantInt::get(i16, 3));
            llvm::Value *p4 = cg_.builder_->CreateAdd(port16, llvm::ConstantInt::get(i16, 4));
            doOutb(llvm::ConstantInt::get(i8, 0x00), p1);
            doOutb(llvm::ConstantInt::get(i8, 0x80), p3);
            llvm::Value *divisor = cg_.builder_->CreateUDiv(llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 115200), baud);
            doOutb(cg_.builder_->CreateIntCast(divisor, i8, false), port16);
            doOutb(cg_.builder_->CreateIntCast(cg_.builder_->CreateLShr(divisor, 8), i8, false), p1);
            doOutb(llvm::ConstantInt::get(i8, 0x03), p3);
            doOutb(llvm::ConstantInt::get(i8, 0xC7), p2);
            doOutb(llvm::ConstantInt::get(i8, 0x0B), p4);
            return nullptr;
        }

        llvm::Value *HardwareFFICodeGen::emitSerialWrite(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "serial_write"}});
                return nullptr;
            }
            llvm::Value *port = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *byte = cg_.resolveOperand(inst->operands[1]);
            if (!port || !byte)
                return nullptr;
            llvm::Type *i16 = llvm::Type::getInt16Ty(*cg_.context_);
            llvm::Type *i8 = llvm::Type::getInt8Ty(*cg_.context_);
            llvm::Value *port16 = cg_.builder_->CreateIntCast(port, i16, false);
            llvm::Value *port5 = cg_.builder_->CreateAdd(port16, llvm::ConstantInt::get(i16, 5));
            llvm::FunctionType *inbFT = llvm::FunctionType::get(i8, {i16}, false);
            llvm::InlineAsm *inbIA = llvm::InlineAsm::get(inbFT, "inb $1, $0", "={ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *waitBB = llvm::BasicBlock::Create(*cg_.context_, "serial.wait", curFunc);
            llvm::BasicBlock *sendBB = llvm::BasicBlock::Create(*cg_.context_, "serial.send", curFunc);
            cg_.builder_->CreateBr(waitBB);
            cg_.builder_->SetInsertPoint(waitBB);
            llvm::Value *lsr = cg_.builder_->CreateCall(inbFT, static_cast<llvm::Value *>(inbIA), {port5}, "lsr");
            llvm::Value *ready = cg_.builder_->CreateAnd(lsr, llvm::ConstantInt::get(i8, 0x20));
            cg_.builder_->CreateCondBr(cg_.builder_->CreateICmpNE(ready, llvm::ConstantInt::get(i8, 0)), sendBB, waitBB);
            cg_.builder_->SetInsertPoint(sendBB);
            llvm::FunctionType *outbFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {i8, i16}, false);
            llvm::InlineAsm *outbIA = llvm::InlineAsm::get(outbFT, "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
            cg_.builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {cg_.builder_->CreateIntCast(byte, i8, false), port16});
            return nullptr;
        }

        llvm::Value *HardwareFFICodeGen::emitSerialRead(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "serial_read"}});
                return nullptr;
            }
            llvm::Value *port = cg_.resolveOperand(inst->operands[0]);
            if (!port)
                return nullptr;
            llvm::Type *i16 = llvm::Type::getInt16Ty(*cg_.context_);
            llvm::Type *i8 = llvm::Type::getInt8Ty(*cg_.context_);
            llvm::Value *port16 = cg_.builder_->CreateIntCast(port, i16, false);
            llvm::Value *port5 = cg_.builder_->CreateAdd(port16, llvm::ConstantInt::get(i16, 5));
            llvm::FunctionType *inbFT = llvm::FunctionType::get(i8, {i16}, false);
            llvm::InlineAsm *inbIA = llvm::InlineAsm::get(inbFT, "inb $1, $0", "={ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *waitBB = llvm::BasicBlock::Create(*cg_.context_, "sr.wait", curFunc);
            llvm::BasicBlock *readBB = llvm::BasicBlock::Create(*cg_.context_, "sr.read", curFunc);
            cg_.builder_->CreateBr(waitBB);
            cg_.builder_->SetInsertPoint(waitBB);
            llvm::Value *lsr = cg_.builder_->CreateCall(inbFT, static_cast<llvm::Value *>(inbIA), {port5}, "lsr");
            cg_.builder_->CreateCondBr(cg_.builder_->CreateICmpNE(cg_.builder_->CreateAnd(lsr, llvm::ConstantInt::get(i8, 0x01)), llvm::ConstantInt::get(i8, 0)), readBB, waitBB);
            cg_.builder_->SetInsertPoint(readBB);
            llvm::Value *data = cg_.builder_->CreateCall(inbFT, static_cast<llvm::Value *>(inbIA), {port16}, "sr.byte");
            llvm::Value *result = cg_.builder_->CreateZExt(data, llvm::Type::getInt64Ty(*cg_.context_), "sr.ext");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *HardwareFFICodeGen::emitSerialReady(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "serial_ready"}});
                return nullptr;
            }
            llvm::Value *port = cg_.resolveOperand(inst->operands[0]);
            if (!port)
                return nullptr;
            llvm::Type *i16 = llvm::Type::getInt16Ty(*cg_.context_);
            llvm::Type *i8 = llvm::Type::getInt8Ty(*cg_.context_);
            llvm::Value *port16 = cg_.builder_->CreateIntCast(port, i16, false);
            llvm::Value *port5 = cg_.builder_->CreateAdd(port16, llvm::ConstantInt::get(i16, 5));
            llvm::FunctionType *inbFT = llvm::FunctionType::get(i8, {i16}, false);
            llvm::InlineAsm *inbIA = llvm::InlineAsm::get(inbFT, "inb $1, $0", "={ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
            llvm::Value *lsr = cg_.builder_->CreateCall(inbFT, static_cast<llvm::Value *>(inbIA), {port5}, "lsr");
            llvm::Value *result = cg_.builder_->CreateZExt(cg_.builder_->CreateAnd(lsr, llvm::ConstantInt::get(i8, 0x01)), llvm::Type::getInt64Ty(*cg_.context_), "sr.rdy");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *HardwareFFICodeGen::emitGpioWrite(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "gpio_write"}});
                return nullptr;
            }
            llvm::Value *addr = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *val = cg_.resolveOperand(inst->operands[1]);
            if (!addr || !val)
                return nullptr;
            llvm::Type *i32 = llvm::Type::getInt32Ty(*cg_.context_);
            llvm::Value *ptr = cg_.builder_->CreateIntToPtr(cg_.builder_->CreateIntCast(addr, llvm::Type::getInt64Ty(*cg_.context_), false), i32->getPointerTo(), "gpio.ptr");
            auto *store = cg_.builder_->CreateStore(cg_.builder_->CreateIntCast(val, i32, false), ptr);
            store->setVolatile(true);
            return nullptr;
        }

        llvm::Value *HardwareFFICodeGen::emitGpioRead(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "gpio_read"}});
                return nullptr;
            }
            llvm::Value *addr = cg_.resolveOperand(inst->operands[0]);
            if (!addr)
                return nullptr;
            llvm::Type *i32 = llvm::Type::getInt32Ty(*cg_.context_);
            llvm::Value *ptr = cg_.builder_->CreateIntToPtr(cg_.builder_->CreateIntCast(addr, llvm::Type::getInt64Ty(*cg_.context_), false), i32->getPointerTo(), "gpio.ptr");
            auto *load = cg_.builder_->CreateLoad(i32, ptr, "gpio.val");
            load->setVolatile(true);
            llvm::Value *result = cg_.builder_->CreateZExt(load, llvm::Type::getInt64Ty(*cg_.context_), "gpio.ext");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *HardwareFFICodeGen::emitGpioMode(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "gpio_mode"}});
                return nullptr;
            }
            llvm::Value *addr = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *mode = cg_.resolveOperand(inst->operands[1]);
            if (!addr || !mode)
                return nullptr;
            llvm::Type *i32 = llvm::Type::getInt32Ty(*cg_.context_);
            llvm::Value *dirAddr = cg_.builder_->CreateAdd(cg_.builder_->CreateIntCast(addr, llvm::Type::getInt64Ty(*cg_.context_), false), llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0x400));
            llvm::Value *ptr = cg_.builder_->CreateIntToPtr(dirAddr, i32->getPointerTo(), "gpio.dir");
            auto *st = cg_.builder_->CreateStore(cg_.builder_->CreateIntCast(mode, i32, false), ptr);
            st->setVolatile(true);
            return nullptr;
        }

        llvm::Value *HardwareFFICodeGen::emitTimerInit(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "timer_init"}});
                return nullptr;
            }
            llvm::Value *freq = cg_.resolveOperand(inst->operands[0]);
            if (!freq)
                return nullptr;
            llvm::Type *i16 = llvm::Type::getInt16Ty(*cg_.context_);
            llvm::Type *i8 = llvm::Type::getInt8Ty(*cg_.context_);
            llvm::FunctionType *outbFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {i8, i16}, false);
            llvm::InlineAsm *outbIA = llvm::InlineAsm::get(outbFT, "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
            cg_.builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {llvm::ConstantInt::get(i8, 0x36), llvm::ConstantInt::get(i16, 0x43)});
            llvm::Value *div = cg_.builder_->CreateUDiv(llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 1193182), freq);
            cg_.builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {cg_.builder_->CreateIntCast(div, i8, false), llvm::ConstantInt::get(i16, 0x40)});
            cg_.builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {cg_.builder_->CreateIntCast(cg_.builder_->CreateLShr(div, 8), i8, false), llvm::ConstantInt::get(i16, 0x40)});
            return nullptr;
        }

        llvm::Value *HardwareFFICodeGen::emitTimerRead(std::shared_ptr<SIRInstruction> inst)
        {
            llvm::Type *i16 = llvm::Type::getInt16Ty(*cg_.context_);
            llvm::Type *i8 = llvm::Type::getInt8Ty(*cg_.context_);
            llvm::FunctionType *outbFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {i8, i16}, false);
            llvm::InlineAsm *outbIA = llvm::InlineAsm::get(outbFT, "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
            llvm::FunctionType *inbFT = llvm::FunctionType::get(i8, {i16}, false);
            llvm::InlineAsm *inbIA = llvm::InlineAsm::get(inbFT, "inb $1, $0", "={ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
            cg_.builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {llvm::ConstantInt::get(i8, 0x00), llvm::ConstantInt::get(i16, 0x43)});
            llvm::Value *lo = cg_.builder_->CreateCall(inbFT, static_cast<llvm::Value *>(inbIA), {llvm::ConstantInt::get(i16, 0x40)}, "pit.lo");
            llvm::Value *hi = cg_.builder_->CreateCall(inbFT, static_cast<llvm::Value *>(inbIA), {llvm::ConstantInt::get(i16, 0x40)}, "pit.hi");
            llvm::Value *result = cg_.builder_->CreateOr(cg_.builder_->CreateZExt(lo, llvm::Type::getInt64Ty(*cg_.context_)), cg_.builder_->CreateShl(cg_.builder_->CreateZExt(hi, llvm::Type::getInt64Ty(*cg_.context_)), 8), "pit.val");
            if (inst && inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *HardwareFFICodeGen::emitTimerWait(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "timer_wait"}});
                return nullptr;
            }
            llvm::Value *us = cg_.resolveOperand(inst->operands[0]);
            if (!us)
                return nullptr;
            llvm::Type *i64 = llvm::Type::getInt64Ty(*cg_.context_);
            llvm::FunctionType *rdtscFT = llvm::FunctionType::get(i64, {}, false);
            llvm::InlineAsm *rdtscIA = llvm::InlineAsm::get(rdtscFT, "rdtsc\n\tshl $$32, %rdx\n\tor %rdx, %rax", "={rax},~{rdx},~{dirflag},~{fpsr},~{flags}", true, false);
            llvm::Value *start = cg_.builder_->CreateCall(rdtscFT, static_cast<llvm::Value *>(rdtscIA), {}, "w.start");
            llvm::Value *target = cg_.builder_->CreateAdd(start, cg_.builder_->CreateMul(us, llvm::ConstantInt::get(i64, 1000)));
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*cg_.context_, "w.loop", curFunc);
            llvm::BasicBlock *doneBB = llvm::BasicBlock::Create(*cg_.context_, "w.done", curFunc);
            cg_.builder_->CreateBr(loopBB);
            cg_.builder_->SetInsertPoint(loopBB);
            llvm::Value *now = cg_.builder_->CreateCall(rdtscFT, static_cast<llvm::Value *>(rdtscIA), {}, "w.now");
            cg_.builder_->CreateCondBr(cg_.builder_->CreateICmpUGE(now, target), doneBB, loopBB);
            cg_.builder_->SetInsertPoint(doneBB);
            return nullptr;
        }

        llvm::Value *HardwareFFICodeGen::emitReset(std::shared_ptr<SIRInstruction> inst)
        {
            llvm::Type *i16 = llvm::Type::getInt16Ty(*cg_.context_);
            llvm::Type *i8 = llvm::Type::getInt8Ty(*cg_.context_);
            llvm::FunctionType *inbFT = llvm::FunctionType::get(i8, {i16}, false);
            llvm::InlineAsm *inbIA = llvm::InlineAsm::get(inbFT, "inb $1, $0", "={ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
            llvm::FunctionType *outbFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {i8, i16}, false);
            llvm::InlineAsm *outbIA = llvm::InlineAsm::get(outbFT, "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *waitBB = llvm::BasicBlock::Create(*cg_.context_, "rst.wait", curFunc);
            llvm::BasicBlock *resetBB = llvm::BasicBlock::Create(*cg_.context_, "rst.do", curFunc);
            cg_.builder_->CreateBr(waitBB);
            cg_.builder_->SetInsertPoint(waitBB);
            llvm::Value *st = cg_.builder_->CreateCall(inbFT, static_cast<llvm::Value *>(inbIA), {llvm::ConstantInt::get(i16, 0x64)}, "kb.st");
            cg_.builder_->CreateCondBr(cg_.builder_->CreateICmpEQ(cg_.builder_->CreateAnd(st, llvm::ConstantInt::get(i8, 0x02)), llvm::ConstantInt::get(i8, 0)), resetBB, waitBB);
            cg_.builder_->SetInsertPoint(resetBB);
            cg_.builder_->CreateCall(outbFT, static_cast<llvm::Value *>(outbIA), {llvm::ConstantInt::get(i8, 0xFE), llvm::ConstantInt::get(i16, 0x64)});
            emitHalt(nullptr);
            return nullptr;
        }

        llvm::Value *HardwareFFICodeGen::emitCpuId(std::shared_ptr<SIRInstruction> inst)
        {
            llvm::Type *i32 = llvm::Type::getInt32Ty(*cg_.context_);
            llvm::Value *leaf = llvm::ConstantInt::get(i32, 0);
            if (inst && !inst->operands.empty())
            {
                llvm::Value *op = cg_.resolveOperand(inst->operands[0]);
                if (op)
                    leaf = cg_.builder_->CreateIntCast(op, i32, false);
            }
            llvm::FunctionType *ft = llvm::FunctionType::get(i32, {i32}, false);
            llvm::InlineAsm *ia = llvm::InlineAsm::get(ft, "cpuid", "={eax},{eax},~{ebx},~{ecx},~{edx}", true, false);
            llvm::Value *eax = cg_.builder_->CreateCall(ft, static_cast<llvm::Value *>(ia), {leaf}, "cpuid.eax");
            llvm::Value *result = cg_.builder_->CreateZExt(eax, llvm::Type::getInt64Ty(*cg_.context_), "cpuid.ext");
            if (inst && inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *HardwareFFICodeGen::emitRdtsc(std::shared_ptr<SIRInstruction> inst)
        {
            llvm::Type *i64 = llvm::Type::getInt64Ty(*cg_.context_);
            llvm::FunctionType *ft = llvm::FunctionType::get(i64, {}, false);
            llvm::InlineAsm *ia = llvm::InlineAsm::get(ft, "rdtsc\n\tshl $$32, %rdx\n\tor %rdx, %rax", "={rax},~{rdx},~{dirflag},~{fpsr},~{flags}", true, false);
            llvm::Value *result = cg_.builder_->CreateCall(ft, static_cast<llvm::Value *>(ia), {}, "rdtsc.val");
            if (inst && inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *HardwareFFICodeGen::emitMfence(std::shared_ptr<SIRInstruction> inst)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {}, false);
            llvm::InlineAsm *ia = llvm::InlineAsm::get(ft, "mfence", "~{memory}", true, false);
            cg_.builder_->CreateCall(ft, static_cast<llvm::Value *>(ia), {});
            return nullptr;
        }

        llvm::Value *HardwareFFICodeGen::emitLfence(std::shared_ptr<SIRInstruction> inst)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {}, false);
            llvm::InlineAsm *ia = llvm::InlineAsm::get(ft, "lfence", "~{memory}", true, false);
            cg_.builder_->CreateCall(ft, static_cast<llvm::Value *>(ia), {});
            return nullptr;
        }

        llvm::Value *HardwareFFICodeGen::emitSfence(std::shared_ptr<SIRInstruction> inst)
        {
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {}, false);
            llvm::InlineAsm *ia = llvm::InlineAsm::get(ft, "sfence", "~{memory}", true, false);
            cg_.builder_->CreateCall(ft, static_cast<llvm::Value *>(ia), {});
            return nullptr;
        }


    } // namespace LLVM
} // namespace Sad

