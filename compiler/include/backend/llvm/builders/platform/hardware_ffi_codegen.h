/*
 * ============================================================================
 * HardwareFFICodeGen - LLVM IR for OS/Embedded/FFI ops (51 methods)
 * Phase 7 Step 13 - extracted from llvm_codegen_hardware_ffi[2].cpp
 * ============================================================================
 */
#ifndef SAD_LLVM_HARDWARE_FFI_CODEGEN_H
#define SAD_LLVM_HARDWARE_FFI_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class HardwareFFICodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit HardwareFFICodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    HardwareFFICodeGen(const HardwareFFICodeGen &) = delete;
    HardwareFFICodeGen &operator=(const HardwareFFICodeGen &) = delete;

    llvm::Value *emitAddrOf(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitCli(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitCpuId(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitDmaInit(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitDmaStart(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFFICalloc(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFFIFclose(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFFIFopen(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFFIFread(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFFIFree(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFFIFwrite(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFFIMalloc(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFFIMemcpy(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFFIMemset(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFFIPrintf(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFFIRealloc(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFFIStrcat(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFFIStrcmp(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFFIStrcpy(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFFIStrlen(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitGpioMode(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitGpioRead(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitGpioWrite(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitHalt(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitInterrupt(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitLfence(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitMemCopy(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitMemCopy32(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitMemFill32(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitMemRead(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitMemSet(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitMemWrite(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitMfence(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitPortRead(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitPortWrite(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitRdtsc(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitReset(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitSerialInit(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitSerialRead(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitSerialReady(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitSerialWrite(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitSfence(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitSti(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitTimerInit(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitTimerRead(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitTimerWait(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitVgaClear(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitVgaWrite(std::shared_ptr<SIRInstruction> inst);
    llvm::Function *getOrCreateUtf8ByteToChar();
    llvm::Function *getOrCreateUtf8CharToByte();
    llvm::Function *getOrCreateUtf8Strlen();
};

}} // namespace Sad::LLVM
#endif
