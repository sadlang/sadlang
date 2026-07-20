/*
 * ============================================================================
 * SecurityBuiltinsCodeGen - LLVM IR for security built-ins
 * Phase 7 Step 15
 * ============================================================================
 */
#ifndef SAD_LLVM_SECURITY_BUILTINS_CODEGEN_H
#define SAD_LLVM_SECURITY_BUILTINS_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class SecurityBuiltinsCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit SecurityBuiltinsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    SecurityBuiltinsCodeGen(const SecurityBuiltinsCodeGen &) = delete;
    SecurityBuiltinsCodeGen &operator=(const SecurityBuiltinsCodeGen &) = delete;

    llvm::Value *emitBuiltinSecurityAssert(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinSecurityVerify(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinSecurityIsSafe(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinSecurityPanic(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinSecurityHash(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinSecurityEncrypt(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinSecurityDecrypt(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinSecurityAssertType(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinSecurityAssertEqual(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinSecurityAssertGreater(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinSecuritySanitize(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinSecurityTimestamp(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinSecuritySecureRandom(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinSecurityBase64Encode(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinCryptoBlake3Hash(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinCryptoBlake3KeyedHash(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
