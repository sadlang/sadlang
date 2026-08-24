/*
 * ============================================================================
 * InstrLowlevelCodeGen - Phase 8 Step 10 - 1 method(s)
 * ============================================================================
 */
#ifndef SAD_LLVM_INSTRLOWLEVEL_CODEGEN_H
#define SAD_LLVM_INSTRLOWLEVEL_CODEGEN_H

#include <memory>
#include <string>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class InstrLowlevelCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit InstrLowlevelCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    InstrLowlevelCodeGen(const InstrLowlevelCodeGen &) = delete;
    InstrLowlevelCodeGen &operator=(const InstrLowlevelCodeGen &) = delete;

    llvm::Value *emitInstructionLowlevel(std::shared_ptr<SIRInstruction> inst);

private:
    /// (AR) [عقد UEFI — ح٥/أ] الإرسالُ الخامُّ على الرمزِ التشغيليّ. غلافُه العامُّ
    ///      `emitInstructionLowlevel` هو الذي يربطُ الناتجَ بسجلِّه في جدولِ القيم —
    ///      فالربطُ في **طبقةِ الإرسالِ وحدَها** لا في مئةٍ وثلاثةٍ وخمسينَ باعثًا.
    ///      كان سبعون باعثَ UEFI/ACPI/APIC تعيدُ القيمةَ بلا ربطٍ، فيسقطُ المصرّفُ
    ///      بـ«Undefined register» عندَ استهلاكِ العائد (قِيس: `متغير ح = uefi_تهيئة()`).
    /// (EN) [UEFI contract — gap ح٥/a] raw opcode dispatch; the public wrapper binds
    ///      the result into namedValues — one layer, not 153 emitters.
    llvm::Value *dispatchLowlevel(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
