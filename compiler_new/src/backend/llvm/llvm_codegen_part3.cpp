/*
 * ============================================================================
 * ┘ו┘ט┘ה╪» ┘ד┘ט╪» LLVM IR - ┘ו┘ה┘ב ╪º┘ה╪¬┘ז┘ב┘ך╪░
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 * 
 * ┘ח╪░╪º ╪º┘ה┘ו┘ה┘ב ┘ך╪¡╪¬┘ט┘ך ╪╣┘ה┘י ╪¬┘ז┘ב┘ך╪░ ┘ו┘ט┘ה╪» ┘ד┘ט╪» LLVM IR ╪º┘ה╪░┘ך ┘ך╪¬╪▒╪¼┘ו SIR ╪Ñ┘ה┘י LLVM IR
 * This file contains the implementation of LLVM IR code generator that
 * translates SIR to LLVM IR
 * 
 * ╪º┘ה╪¬╪▓╪º┘ו ╪¿ STRICT_CODING_RULES.md:
 * - ╪¬┘ו ┘ג╪▒╪º╪í╪⌐ ╪¼┘ו┘ך╪╣ ╪º┘ה┘ו┘ה┘ב╪º╪¬ ╪º┘ה╪▒╪ú╪│┘ך╪⌐ ╪¿╪º┘ה┘ד╪º┘ו┘ה (1746 ╪│╪╖╪▒)
 * - ╪¬┘ו ╪¬┘ט╪½┘ך┘ג ┘ד┘ה ╪º╪│╪¬╪«╪»╪º┘ו API ╪¿┘ו┘ט┘ג╪╣ ╪º┘ה┘ו╪╡╪»╪▒
 * - ┘ה╪º ╪¬┘ט╪¼╪» ╪»┘ט╪º┘ה ╪ú┘ט ┘ו╪¬╪║┘ך╪▒╪º╪¬ ┘ו╪«╪¬╪▒╪╣╪⌐
 * 
 * STRICT_CODING_RULES.md compliance:
 * - All header files read completely (1746 lines)
 * - Every API usage documented with source location
 * - No invented functions or variables
 * 
 * ╪º┘ה┘ו╪ñ┘ה┘ב (Author): SadLanguage Compiler Team
 * ╪º┘ה╪¬╪º╪▒┘ך╪« (Date): December 2024
 * ╪º┘ה╪Ñ╪╡╪»╪º╪▒ (Version): 1.1.4 Phase 1
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include <llvm/Support/TargetSelect.h>
// Source: LLVM 14+ API - llvm/MC/TargetRegistry.h ╪¿╪»┘ה╪º┘כ ┘ו┘ז llvm/Support/TargetRegistry.h
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
using namespace Sad::Compiler::SIR;  // For SIRModule, SIRFunction, SIRBasicBlock, SIRInstruction, SIRType

namespace Sad {
namespace LLVM {

void LLVMCodeGen::emitFunctionParameters(std::shared_ptr<SIRFunction> sirFunc, llvm::Function* llvmFunc) {
    if (!sirFunc || !llvmFunc) {
        return;
    }
    
    // Source: SIRFunction::getParameters() is at sir_module.h:293
    const auto& params = sirFunc->getParameters();
    
    unsigned idx = 0;
    for (auto& arg : llvmFunc->args()) {
        if (idx < params.size()) {
            // Source: SIRParameter::name is PUBLIC member at sir_module.h:229
            std::string paramName = params[idx].name;
            
            // ╪Ñ┘ז╪┤╪º╪í alloca ┘ה┘ה┘ו╪╣╪º┘ו┘ה
            // Create alloca for parameter
            llvm::IRBuilder<> tmpBuilder(&llvmFunc->getEntryBlock(), 
                                        llvmFunc->getEntryBlock().begin());
            llvm::AllocaInst* alloca = tmpBuilder.CreateAlloca(
                arg.getType(),
                nullptr,
                paramName
            );
            
            // ╪¬╪«╪▓┘ך┘ז ┘ג┘ך┘ו╪⌐ ╪º┘ה┘ו╪╣╪º┘ו┘ה
            // Store parameter value
            tmpBuilder.CreateStore(&arg, alloca);
            
            // ╪¡┘ב╪╕ ┘ב┘ך ╪º┘ה┘ג┘ך┘ו ╪º┘ה┘ו╪│┘ו╪º╪⌐
            // Save to named values
            // Source: context_info_.namedValues is at llvm_codegen.h:617
            // (AR) ┘ז╪¡┘ב╪╕ ╪¿┘ד┘ה╪º ╪º┘ה╪º╪│┘ו┘ך┘ז: ╪¿╪»┘ט┘ז % ┘ט╪¿╪º╪»╪ª╪⌐ % ┘ה╪ú┘ז SIR builder ┘ך╪│╪¼┘ה
            //      ╪º┘ה┘ו╪╣╪º┘ו┘ה╪º╪¬ ┘ד╪│╪¼┘ה╪º╪¬ ╪¿╪º╪»╪ª╪⌐ % (sir_builder.cpp:271)
            // (EN) Store under both names: without % and with % prefix because
            //      the SIR builder registers parameters as %param_name registers
            //      (sir_builder.cpp:271)
            context_info_.namedValues[paramName] = alloca;
            context_info_.namedValues["%" + paramName] = alloca;
        }
        idx++;
    }
}

/**
 * ╪Ñ┘ז╪┤╪º╪í ┘ד╪¬┘ה╪⌐ ╪ú╪│╪º╪│┘ך╪⌐ ┘ב╪º╪▒╪║╪⌐
 * Create empty basic block
 * 
 * Source: llvm_codegen.h:400
 * @param name ╪º╪│┘ו ╪º┘ה┘ד╪¬┘ה╪⌐ / Block name
 * @param function ╪º┘ה╪»╪º┘ה╪⌐ ╪º┘ה┘ו╪¡╪¬┘ט┘ך╪⌐ / Containing function
 * @return ┘ד╪¬┘ה╪⌐ LLVM ╪º┘ה╪ú╪│╪º╪│┘ך╪⌐ / LLVM basic block
 */
llvm::BasicBlock* LLVMCodeGen::createBasicBlock(const std::string& name, llvm::Function* function) {
    if (!function) {
        reportError("Null function in createBasicBlock");
        return nullptr;
    }
    
    // Source: context_ is defined at llvm_codegen.h:631
    return llvm::BasicBlock::Create(*context_, name, function);
}

// ============================================================================
// ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 4: ╪¬╪╣┘ה┘ך┘ו╪º╪¬ ╪¡╪│╪º╪¿┘ך╪⌐ ┘ט╪½┘ז╪º╪ª┘ך╪⌐
// Phase 4: Arithmetic & Bitwise Instructions
// ============================================================================

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ┘ו┘ז SIR
 * Emit instruction from SIR
 * 
 * Source: llvm_codegen.h:399
 * @param inst ╪¬╪╣┘ה┘ך┘ו╪⌐ SIR / SIR instruction
 * @return ┘ג┘ך┘ו╪⌐ LLVM ╪º┘ה┘ז╪º╪¬╪¼╪⌐ / Resulting LLVM value
 */
llvm::Value* LLVMCodeGen::emitInstruction(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) {
        reportError("Null instruction in emitInstruction");
        return nullptr;
    }
    
    // Source: SIRInstruction::opcode is PUBLIC member at sir_instruction.h:60
    // Source: SIROpcode enum is at sir_types.h:120-215
    switch (inst->opcode) {
        // Arithmetic - Source: sir_types.h:120-129
        case SIROpcode::ADD_I64:
        case SIROpcode::ADD_F64:
            return emitAdd(inst);
        case SIROpcode::SUB_I64:
        case SIROpcode::SUB_F64:
            return emitSub(inst);
        case SIROpcode::MUL_I64:
        case SIROpcode::MUL_F64:
            return emitMul(inst);
        case SIROpcode::DIV_I64:
        case SIROpcode::DIV_F64:
            return emitDiv(inst);
        case SIROpcode::MOD_I64:
            return emitMod(inst);
        case SIROpcode::NEG:
            return emitNeg(inst);
            
        // Bitwise - Source: sir_types.h:134-141
        case SIROpcode::AND:
            return emitAnd(inst);
        case SIROpcode::OR:
            return emitOr(inst);
        case SIROpcode::XOR:
            return emitXor(inst);
        case SIROpcode::NOT:
            return emitNot(inst);
        case SIROpcode::SHL:
            return emitShl(inst);
        case SIROpcode::SHR:
            return emitShr(inst);
            
        // Comparison - Source: sir_types.h:146-151
        case SIROpcode::EQ:
            return emitCmpEq(inst);
        case SIROpcode::NE:
            return emitCmpNe(inst);
        case SIROpcode::LT:
            return emitCmpLt(inst);
        case SIROpcode::LE:
            return emitCmpLe(inst);
        case SIROpcode::GT:
            return emitCmpGt(inst);
        case SIROpcode::GE:
            return emitCmpGe(inst);
            
        // Memory - Source: sir_types.h:168-177
        case SIROpcode::LOAD:
            return emitLoad(inst);
        case SIROpcode::STORE:
            return emitStore(inst);
        case SIROpcode::ALLOC:
            return emitAlloca(inst);
        case SIROpcode::MOVE:
            return emitMove(inst);
        
        // String - ╪»┘ו╪¼ ╪º┘ה┘ז╪╡┘ט╪╡
        case SIROpcode::STRING_CONCAT:
            return emitStringConcat(inst);
        case SIROpcode::STRING_CMP:
            return emitStringCmp(inst);
        case SIROpcode::BUILTIN_STRING_LENGTH:
            return emitFFIStrlen(inst);
        case SIROpcode::BUILTIN_STRING_CHAR_AT:
            return emitStringCharAt(inst);
            
        // Control Flow - Source: sir_types.h:156-163
        case SIROpcode::BR:
            return emitBranch(inst);
        case SIROpcode::BR_COND:
            return emitCondBranch(inst);
        case SIROpcode::CALL:
            return emitCall(inst);
        case SIROpcode::RET:
        case SIROpcode::RET_VOID:
            return emitReturn(inst);
            



        // ===== OS/Hardware - Port I/O =====
        case SIROpcode::INLINE_ASM:
            return emitInlineAsm(inst);
        case SIROpcode::BUILTIN_PORT_WRITE:
        case SIROpcode::BUILTIN_PORT_WRITE_16:
        case SIROpcode::BUILTIN_PORT_WRITE_32:
            return emitPortWrite(inst);
        case SIROpcode::BUILTIN_PORT_READ:
        case SIROpcode::BUILTIN_PORT_READ_16:
        case SIROpcode::BUILTIN_PORT_READ_32:
            return emitPortRead(inst);
        case SIROpcode::BUILTIN_MEM_WRITE_8:
        case SIROpcode::BUILTIN_MEM_WRITE_16:
        case SIROpcode::BUILTIN_MEM_WRITE_32:
        case SIROpcode::BUILTIN_MEM_WRITE_64:
            return emitMemWrite(inst);
        case SIROpcode::BUILTIN_MEM_READ_8:
        case SIROpcode::BUILTIN_MEM_READ_16:
        case SIROpcode::BUILTIN_MEM_READ_32:
        case SIROpcode::BUILTIN_MEM_READ_64:
            return emitMemRead(inst);
        case SIROpcode::BUILTIN_INTERRUPT:
            return emitInterrupt(inst);
        case SIROpcode::BUILTIN_HALT:
            return emitHalt(inst);
        case SIROpcode::BUILTIN_CLI:
            return emitCli(inst);
        case SIROpcode::BUILTIN_STI:
            return emitSti(inst);
        case SIROpcode::BUILTIN_VGA_WRITE:
            return emitVgaWrite(inst);
        case SIROpcode::BUILTIN_VGA_CLEAR:
            return emitVgaClear(inst);
        case SIROpcode::BUILTIN_ADDR_OF:
            return emitAddrOf(inst);
        case SIROpcode::BUILTIN_MEM_COPY:
            return emitMemCopy(inst);
        case SIROpcode::BUILTIN_MEM_SET:
            return emitMemSet(inst);

        // ===== Embedded (18) =====
        case SIROpcode::BUILTIN_SERIAL_INIT:
            return emitSerialInit(inst);
        case SIROpcode::BUILTIN_SERIAL_WRITE:
            return emitSerialWrite(inst);
        case SIROpcode::BUILTIN_SERIAL_READ:
            return emitSerialRead(inst);
        case SIROpcode::BUILTIN_SERIAL_READY:
            return emitSerialReady(inst);
        case SIROpcode::BUILTIN_GPIO_WRITE:
            return emitGpioWrite(inst);
        case SIROpcode::BUILTIN_GPIO_READ:
            return emitGpioRead(inst);
        case SIROpcode::BUILTIN_GPIO_MODE:
            return emitGpioMode(inst);
        case SIROpcode::BUILTIN_TIMER_INIT:
            return emitTimerInit(inst);
        case SIROpcode::BUILTIN_TIMER_READ:
            return emitTimerRead(inst);
        case SIROpcode::BUILTIN_TIMER_WAIT:
            return emitTimerWait(inst);
        case SIROpcode::BUILTIN_RESET:
            return emitReset(inst);
        case SIROpcode::BUILTIN_CPUID:
            return emitCpuId(inst);
        case SIROpcode::BUILTIN_RDTSC:
            return emitRdtsc(inst);
        case SIROpcode::BUILTIN_MFENCE:
            return emitMfence(inst);
        case SIROpcode::BUILTIN_LFENCE:
            return emitLfence(inst);
        case SIROpcode::BUILTIN_SFENCE:
            return emitSfence(inst);
        case SIROpcode::BUILTIN_DMA_INIT:
            return emitDmaInit(inst);
        case SIROpcode::BUILTIN_DMA_START:
            return emitDmaStart(inst);

        // ===== Security (14) =====
        case SIROpcode::BUILTIN_SECURITY_ASSERT:
            return emitBuiltinSecurityAssert(inst);
        case SIROpcode::BUILTIN_SECURITY_VERIFY:
            return emitBuiltinSecurityVerify(inst);
        case SIROpcode::BUILTIN_SECURITY_IS_SAFE:
            return emitBuiltinSecurityIsSafe(inst);
        case SIROpcode::BUILTIN_SECURITY_PANIC:
            return emitBuiltinSecurityPanic(inst);
        case SIROpcode::BUILTIN_SECURITY_HASH:
            return emitBuiltinSecurityHash(inst);
        case SIROpcode::BUILTIN_SECURITY_ENCRYPT:
            return emitBuiltinSecurityEncrypt(inst);
        case SIROpcode::BUILTIN_SECURITY_DECRYPT:
            return emitBuiltinSecurityDecrypt(inst);
        case SIROpcode::BUILTIN_SECURITY_ASSERT_TYPE:
            return emitBuiltinSecurityAssertType(inst);
        case SIROpcode::BUILTIN_SECURITY_ASSERT_EQUAL:
            return emitBuiltinSecurityAssertEqual(inst);
        case SIROpcode::BUILTIN_SECURITY_ASSERT_GREATER:
            return emitBuiltinSecurityAssertGreater(inst);
        case SIROpcode::BUILTIN_SECURITY_SANITIZE:
            return emitBuiltinSecuritySanitize(inst);
        case SIROpcode::BUILTIN_SECURITY_TIMESTAMP:
            return emitBuiltinSecurityTimestamp(inst);
        case SIROpcode::BUILTIN_SECURITY_SECURE_RANDOM:
            return emitBuiltinSecuritySecureRandom(inst);
        case SIROpcode::BUILTIN_SECURITY_BASE64_ENCODE:
            return emitBuiltinSecurityBase64Encode(inst);

        // ===== FFI (20) =====
        case SIROpcode::FFI_PRINTF:
            return emitFFIPrintf(inst);
        case SIROpcode::FFI_MALLOC:
            return emitFFIMalloc(inst);
        case SIROpcode::FFI_FREE:
            return emitFFIFree(inst);
        case SIROpcode::FFI_REALLOC:
            return emitFFIRealloc(inst);
        case SIROpcode::FFI_CALLOC:
            return emitFFICalloc(inst);
        case SIROpcode::FFI_STRLEN:
            return emitFFIStrlen(inst);
        case SIROpcode::FFI_STRCPY:
            return emitFFIStrcpy(inst);
        case SIROpcode::FFI_STRCMP:
            return emitFFIStrcmp(inst);
        case SIROpcode::FFI_STRCAT:
            return emitFFIStrcat(inst);
        case SIROpcode::FFI_MEMCPY:
            return emitFFIMemcpy(inst);
        case SIROpcode::FFI_MEMSET:
            return emitFFIMemset(inst);
        case SIROpcode::FFI_FOPEN:
            return emitFFIFopen(inst);
        case SIROpcode::FFI_FCLOSE:
            return emitFFIFclose(inst);
        case SIROpcode::FFI_FWRITE:
            return emitFFIFwrite(inst);
        case SIROpcode::FFI_FREAD:
            return emitFFIFread(inst);
        case SIROpcode::FFI_SYSTEM:
            return emitFFISystem(inst);
        case SIROpcode::FFI_GETENV:
            return emitFFIGetenv(inst);
        case SIROpcode::FFI_ATOI:
            return emitFFIAtoi(inst);
        case SIROpcode::FFI_ATOF:
            return emitFFIAtof(inst);
        case SIROpcode::FFI_SNPRINTF:
            return emitFFISnprintf(inst);

        // ===== Builtin Core =====
        case SIROpcode::BUILTIN_PRINT:
            return emitBuiltinPrint(inst);
        case SIROpcode::BUILTIN_READ:
            return emitBuiltinRead(inst);
        case SIROpcode::BUILTIN_SQRT:
            return emitBuiltinSqrt(inst);
        case SIROpcode::BUILTIN_SIN:
            return emitBuiltinSin(inst);
        case SIROpcode::BUILTIN_COS:
            return emitBuiltinCos(inst);
        case SIROpcode::BUILTIN_TAN:
            return emitBuiltinTan(inst);
        case SIROpcode::BUILTIN_ABS:
            return emitBuiltinAbs(inst);
        case SIROpcode::BUILTIN_FLOOR:
            return emitBuiltinFloor(inst);
        case SIROpcode::BUILTIN_CEIL:
            return emitBuiltinCeil(inst);
        case SIROpcode::BUILTIN_ROUND:
            return emitBuiltinRound(inst);
        case SIROpcode::BUILTIN_POW:
            return emitBuiltinPow(inst);
        case SIROpcode::BUILTIN_SLEEP:
            return emitBuiltinSleep(inst);
        case SIROpcode::BUILTIN_EXIT:
            return emitBuiltinExit(inst);
        case SIROpcode::BUILTIN_RANDOM:
            return emitBuiltinRandom(inst);

        
        // ====================================================================
        // Async/Await & Concurrency (23 opcodes)
        // ====================================================================
        case SIROpcode::ASYNC_SPAWN:          return emitAsyncSpawn(inst);
        case SIROpcode::ASYNC_AWAIT:          return emitAsyncAwait(inst);
        case SIROpcode::ASYNC_YIELD:          return emitAsyncYield(inst);
        case SIROpcode::ASYNC_SLEEP:          return emitAsyncSleep(inst);
        case SIROpcode::ASYNC_CREATE_FUTURE:  return emitAsyncCreateFuture(inst);
        case SIROpcode::ASYNC_RESOLVE_FUTURE: return emitAsyncResolveFuture(inst);
        case SIROpcode::ASYNC_GET_FUTURE:     return emitAsyncGetFuture(inst);
        case SIROpcode::ASYNC_CREATE_CHANNEL: return emitAsyncCreateChannel(inst);
        case SIROpcode::ASYNC_CHANNEL_SEND:   return emitAsyncChannelSend(inst);
        case SIROpcode::ASYNC_CHANNEL_RECV:   return emitAsyncChannelRecv(inst);
        case SIROpcode::ASYNC_CHANNEL_CLOSE:  return emitAsyncChannelClose(inst);
        case SIROpcode::ASYNC_MUTEX_CREATE:   return emitAsyncMutexCreate(inst);
        case SIROpcode::ASYNC_MUTEX_LOCK:     return emitAsyncMutexLock(inst);
        case SIROpcode::ASYNC_MUTEX_UNLOCK:   return emitAsyncMutexUnlock(inst);
        case SIROpcode::ASYNC_THREAD_SPAWN:   return emitAsyncThreadSpawn(inst);
        case SIROpcode::ASYNC_THREAD_JOIN:    return emitAsyncThreadJoin(inst);
        case SIROpcode::ASYNC_ATOMIC_LOAD:    return emitAsyncAtomicLoad(inst);
        case SIROpcode::ASYNC_ATOMIC_STORE:   return emitAsyncAtomicStore(inst);
        case SIROpcode::ASYNC_ATOMIC_ADD:     return emitAsyncAtomicAdd(inst);
        case SIROpcode::ASYNC_ATOMIC_CAS:     return emitAsyncAtomicCAS(inst);
        case SIROpcode::ASYNC_WAIT_ALL:       return emitAsyncWaitAll(inst);
        case SIROpcode::ASYNC_WAIT_ANY:       return emitAsyncWaitAny(inst);
        case SIROpcode::ASYNC_SELECT:         return emitAsyncSelect(inst);

        // ===== Missing Control Flow =====
        case SIROpcode::SWITCH:
            return emitSwitch(inst);
        case SIROpcode::PHI:
            return emitPhi(inst);
        case SIROpcode::CALL_INDIRECT:
            return emitCallIndirect(inst);

        // ===== Missing Bitwise =====
        case SIROpcode::SAR:
            return emitSar(inst);
        case SIROpcode::ROL:
            return emitRol(inst);

        // ===== Missing Memory =====
        case SIROpcode::ALLOC_HEAP:
            return emitAllocHeap(inst);
        case SIROpcode::FREE:
            return emitFreeMem(inst);
        case SIROpcode::ADDR:
            return emitAddr(inst);
        case SIROpcode::PTR_ADD:
            return emitPtrAdd(inst);
        case SIROpcode::PTR_CAST:
            return emitPtrCast(inst);
        case SIROpcode::MEMCPY:
            return emitMemCopy(inst);
        case SIROpcode::MEMSET:
            return emitMemSet(inst);

        // ===== Arrays (6) =====
        case SIROpcode::ARRAY_NEW:
            return emitArrayNew(inst);
        case SIROpcode::ARRAY_GET:
            return emitArrayGet(inst);
        case SIROpcode::ARRAY_SET:
            return emitArraySet(inst);
        case SIROpcode::ARRAY_LEN:
            return emitArrayLen(inst);
        case SIROpcode::ARRAY_APPEND:
            return emitBuiltinArrayAppend(inst);
        case SIROpcode::ARRAY_REMOVE:
            return emitBuiltinArrayRemove(inst);

        // ===== Strings (8) =====
        case SIROpcode::STRING_NEW:
            return emitStringNew(inst);
        case SIROpcode::STRING_LEN:
            return emitFFIStrlen(inst);
        case SIROpcode::STRING_SUBSTR:
            return emitBuiltinStringSubstring(inst);
        case SIROpcode::STRING_FIND:
            return emitBuiltinStringFind(inst);
        case SIROpcode::STRING_REPLACE:
            return emitBuiltinStringReplace(inst);
        case SIROpcode::STRING_TO_I64:
            return emitStringToI64(inst);
        case SIROpcode::STRING_TO_F64:
            return emitStringToF64(inst);

        // ===== OOP (10) =====
        case SIROpcode::OBJECT_NEW:
            return emitObjectNew(inst);
        case SIROpcode::OBJECT_GET:
            return emitObjectGet(inst);
        case SIROpcode::OBJECT_SET:
            return emitObjectSet(inst);
        case SIROpcode::OBJECT_CALL:
            return emitObjectCall(inst);
        case SIROpcode::INSTANCEOF:
            return emitInstanceOf(inst);
        case SIROpcode::OBJECT_CAST:
            return emitObjectCast(inst);
        case SIROpcode::CLASS_DEF:
            return emitClassDef(inst);
        case SIROpcode::METHOD_DEF:
            return emitMethodDef(inst);
        case SIROpcode::FIELD_DEF:
            return emitFieldDef(inst);
        case SIROpcode::CONSTRUCTOR_CALL:
            return emitConstructorCall(inst);

        // ===== Type Conversion (8) =====
        case SIROpcode::I64_TO_F64:
            return emitI64ToF64(inst);
        case SIROpcode::F64_TO_I64:
            return emitF64ToI64(inst);
        case SIROpcode::I64_TO_BOOL:
            return emitI64ToBool(inst);
        case SIROpcode::BOOL_TO_I64:
            return emitBoolToI64(inst);
        case SIROpcode::I64_TO_STRING:
            return emitI64ToString(inst);
        case SIROpcode::F64_TO_STRING:
            return emitF64ToString(inst);
        case SIROpcode::BOOL_TO_STRING:
            return emitBoolToString(inst);
        case SIROpcode::CAST:
            return emitCast(inst);

        // ===== Builtin String (10 more) =====
        case SIROpcode::BUILTIN_STRING_TO_UPPER:
            return emitBuiltinStringToUpper(inst);
        case SIROpcode::BUILTIN_STRING_TO_LOWER:
            return emitBuiltinStringToLower(inst);
        case SIROpcode::BUILTIN_STRING_FIND:
            return emitBuiltinStringFind(inst);
        case SIROpcode::BUILTIN_STRING_REPLACE:
            return emitBuiltinStringReplace(inst);
        case SIROpcode::BUILTIN_STRING_SUBSTRING:
            return emitBuiltinStringSubstring(inst);
        case SIROpcode::BUILTIN_STRING_TRIM:
            return emitBuiltinStringTrim(inst);
        case SIROpcode::BUILTIN_STRING_SPLIT:
            return emitBuiltinStringSplit(inst);
        case SIROpcode::BUILTIN_STRING_JOIN:
            return emitBuiltinStringJoin(inst);
        case SIROpcode::BUILTIN_STRING_STARTS_WITH:
            return emitBuiltinStringStartsWith(inst);
        case SIROpcode::BUILTIN_STRING_ENDS_WITH:
            return emitBuiltinStringEndsWith(inst);
        case SIROpcode::BUILTIN_STRING_CONTAINS:
            return emitBuiltinStringContains(inst);

        // ===== Builtin Array (10) =====
        case SIROpcode::BUILTIN_ARRAY_APPEND:
            return emitBuiltinArrayAppend(inst);
        case SIROpcode::BUILTIN_ARRAY_REMOVE:
            return emitBuiltinArrayRemove(inst);
        case SIROpcode::BUILTIN_ARRAY_SIZE:
            return emitBuiltinArraySize(inst);
        case SIROpcode::BUILTIN_ARRAY_INDEX_OF:
            return emitBuiltinArrayIndexOf(inst);
        case SIROpcode::BUILTIN_ARRAY_CONTAINS:
            return emitBuiltinArrayContains(inst);
        case SIROpcode::BUILTIN_ARRAY_REVERSE:
            return emitBuiltinArrayReverse(inst);
        case SIROpcode::BUILTIN_ARRAY_SORT:
            return emitBuiltinArraySort(inst);
        case SIROpcode::BUILTIN_ARRAY_FIRST:
            return emitBuiltinArrayFirst(inst);
        case SIROpcode::BUILTIN_ARRAY_LAST:
            return emitBuiltinArrayLast(inst);
        case SIROpcode::BUILTIN_ARRAY_SLICE:
            return emitBuiltinArraySlice(inst);

        // ===== Builtin File I/O (8) =====
        case SIROpcode::BUILTIN_FILE_READ:
            return emitBuiltinFileRead(inst);
        case SIROpcode::BUILTIN_FILE_WRITE:
            return emitBuiltinFileWrite(inst);
        case SIROpcode::BUILTIN_FILE_APPEND:
            return emitBuiltinFileAppend(inst);
        case SIROpcode::BUILTIN_FILE_DELETE:
            return emitBuiltinFileDelete(inst);
        case SIROpcode::BUILTIN_FILE_COPY:
            return emitBuiltinFileCopy(inst);
        case SIROpcode::BUILTIN_FILE_MOVE:
            return emitBuiltinFileMove(inst);
        case SIROpcode::BUILTIN_FILE_CREATE_DIR:
            return emitBuiltinFileCreateDir(inst);
        case SIROpcode::BUILTIN_FILE_LIST_DIR:
            return emitBuiltinFileListDir(inst);

        // ===== Builtin Extra =====
        case SIROpcode::BUILTIN_MIN:
            return emitBuiltinMin(inst);
        case SIROpcode::BUILTIN_MAX:
            return emitBuiltinMax(inst);
        case SIROpcode::BUILTIN_ASSERT:
            return emitBuiltinAssert(inst);
        case SIROpcode::BUILTIN_DEBUG:
            return emitBuiltinDebug(inst);
        case SIROpcode::BUILTIN_TYPE_OF:
            return emitBuiltinTypeOf(inst);

        default:
            reportError("Unsupported opcode: " + std::to_string(static_cast<int>(inst->opcode)));
            return nullptr;
    }
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ╪¼┘ו╪╣
 * Emit add instruction
 * 
 * Source: llvm_codegen.h:411
 * @param inst ╪¬╪╣┘ה┘ך┘ו╪⌐ SIR / SIR instruction
 * @return ┘ג┘ך┘ו╪⌐ LLVM / LLVM value
 */


} // namespace LLVM
} // namespace Sad
