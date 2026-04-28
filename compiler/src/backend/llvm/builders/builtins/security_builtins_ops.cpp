/*
 * ============================================================================
 * SecurityBuiltinsCodeGen implementation
 * Phase 7 Step 15
 * ============================================================================
 */
#include "llvm_codegen.h"
#include "builders/builtins/security_builtins_codegen.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>

namespace Sad { namespace LLVM {

        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityAssert(std::shared_ptr<SIRInstruction> inst)
        {
            // Security assert - check condition and abort if false
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *cond = cg_.resolveOperand(inst->operands[0]);
            if (!cond)
                return nullptr;
            // (AR) [Fix BF-04] فحص نوع المؤشر قبل المقارنة
            // (EN) [Fix BF-04] Check pointer type before ICmp
            llvm::Value *condBool;
            if (cond->getType()->isIntegerTy(1))
            {
                condBool = cond;
            }
            else if (cond->getType()->isPointerTy())
            {
                condBool = cg_.builder_->CreateICmpNE(
                    cond,
                    llvm::ConstantPointerNull::get(
                        llvm::cast<llvm::PointerType>(cond->getType())),
                    "sec.cmp.ptr");
            }
            else
            {
                condBool = cg_.builder_->CreateICmpNE(cond, llvm::ConstantInt::get(cond->getType(), 0), "sec.cmp");
            }
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *failBB = llvm::BasicBlock::Create(*cg_.context_, "sec.fail", curFunc);
            llvm::BasicBlock *passBB = llvm::BasicBlock::Create(*cg_.context_, "sec.pass", curFunc);
            cg_.builder_->CreateCondBr(condBool, passBB, failBB);
            cg_.builder_->SetInsertPoint(failBB);
            llvm::FunctionType *abortFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {}, false);
            llvm::FunctionCallee abortFn = cg_.module_->getOrInsertFunction("abort", abortFT);
            cg_.builder_->CreateCall(abortFn, {});
            cg_.builder_->CreateUnreachable();
            cg_.builder_->SetInsertPoint(passBB);
            return nullptr;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityVerify(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;
            // (AR) [Fix BF-04] فحص نوع المؤشر قبل المقارنة
            // (EN) [Fix BF-04] Check pointer type before ICmp
            llvm::Value *result;
            if (val->getType()->isPointerTy())
            {
                result = cg_.builder_->CreateICmpNE(
                    val,
                    llvm::ConstantPointerNull::get(
                        llvm::cast<llvm::PointerType>(val->getType())),
                    "sec.verify.ptr");
            }
            else
            {
                result = cg_.builder_->CreateICmpNE(val, llvm::ConstantInt::get(val->getType(), 0));
            }
            llvm::Value *ext = cg_.builder_->CreateZExt(result, llvm::Type::getInt64Ty(*cg_.context_));
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = ext;
            return ext;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityIsSafe(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *ptr = cg_.resolveOperand(inst->operands[0]);
            if (!ptr)
                return nullptr;
            llvm::Value *result = cg_.builder_->CreateICmpNE(ptr, llvm::Constant::getNullValue(ptr->getType()));
            llvm::Value *ext = cg_.builder_->CreateZExt(result, llvm::Type::getInt64Ty(*cg_.context_));
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = ext;
            return ext;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityPanic(std::shared_ptr<SIRInstruction> inst)
        {
            llvm::FunctionType *abortFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {}, false);
            llvm::FunctionCallee abortFn = cg_.module_->getOrInsertFunction("abort", abortFT);
            cg_.builder_->CreateCall(abortFn, {});
            cg_.builder_->CreateUnreachable();
            return nullptr;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityHash(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;
            // Call runtime sad_security_hash(const char*) -> int64_t
            llvm::FunctionType *ft = llvm::FunctionType::get(
                llvm::Type::getInt64Ty(*cg_.context_),
                {llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo()}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_hash", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {val}, "hash.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityEncrypt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *text = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *key = cg_.resolveOperand(inst->operands[1]);
            if (!text || !key)
                return nullptr;
            // Call runtime sad_security_encrypt(const char*, const char*) -> char*
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {i8Ptr, i8Ptr}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_encrypt", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {text, key}, "encrypt.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityDecrypt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *text = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *key = cg_.resolveOperand(inst->operands[1]);
            if (!text || !key)
                return nullptr;
            // Call runtime sad_security_decrypt(const char*, const char*) -> char*
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {i8Ptr, i8Ptr}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_decrypt", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {text, key}, "decrypt.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityAssertType(std::shared_ptr<SIRInstruction> inst)
        {
            // Runtime type assertion: check that the value's type tag matches expected
            // Since Sad uses compile-time type checking, emit a runtime no-op but log
            // In debug mode, this could call a runtime check function
            if (!inst || inst->operands.empty())
                return nullptr;

            // Call a runtime helper that prints a warning if type mismatch
            // sad_security_assert_type(const char* expected_type, const char* actual_type)
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);

            // If we have type metadata in the instruction, emit the check
            if (inst->operands.size() >= 2)
            {
                llvm::Value *expectedType = cg_.resolveOperand(inst->operands[0]);
                llvm::Value *actualVal = cg_.resolveOperand(inst->operands[1]);
                if (expectedType && actualVal)
                {
                    auto *ftType = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {ptrTy, ptrTy}, false);
                    auto fn = cg_.module_->getOrInsertFunction("sad_security_assert_type", ftType);
                    cg_.builder_->CreateCall(fn, {expectedType, actualVal});
                }
            }
            // Compile-time type safety is the primary mechanism
            return nullptr;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityAssertEqual(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *a = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *b = cg_.resolveOperand(inst->operands[1]);
            if (!a || !b)
                return nullptr;
            // Check types: if both are pointers (strings), use string comparison
            if (a->getType()->isPointerTy() && b->getType()->isPointerTy())
            {
                llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
                llvm::FunctionType *ft = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(*cg_.context_), {i8Ptr, i8Ptr}, false);
                llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_assert_equal_str", ft);
                cg_.builder_->CreateCall(fn, {a, b});
            }
            else
            {
                // Integer comparison: if a != b, abort
                // Ensure both are same type
                if (a->getType() != b->getType())
                {
                    if (a->getType()->isIntegerTy() && b->getType()->isIntegerTy())
                    {
                        unsigned aBits = a->getType()->getIntegerBitWidth();
                        unsigned bBits = b->getType()->getIntegerBitWidth();
                        if (aBits < bBits)
                            a = cg_.builder_->CreateSExt(a, b->getType());
                        else if (bBits < aBits)
                            b = cg_.builder_->CreateSExt(b, a->getType());
                    }
                }
                llvm::Value *cmp = cg_.builder_->CreateICmpEQ(a, b, "assert.eq");
                llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
                llvm::BasicBlock *failBB = llvm::BasicBlock::Create(*cg_.context_, "aeq.fail", curFunc);
                llvm::BasicBlock *passBB = llvm::BasicBlock::Create(*cg_.context_, "aeq.pass", curFunc);
                cg_.builder_->CreateCondBr(cmp, passBB, failBB);
                cg_.builder_->SetInsertPoint(failBB);
                llvm::FunctionType *abortFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {}, false);
                llvm::FunctionCallee abortFn = cg_.module_->getOrInsertFunction("abort", abortFT);
                cg_.builder_->CreateCall(abortFn, {});
                cg_.builder_->CreateUnreachable();
                cg_.builder_->SetInsertPoint(passBB);
            }
            return nullptr;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityAssertGreater(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *a = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *b = cg_.resolveOperand(inst->operands[1]);
            if (!a || !b)
                return nullptr;
            // Ensure both are same integer type
            if (a->getType() != b->getType())
            {
                if (a->getType()->isIntegerTy() && b->getType()->isIntegerTy())
                {
                    unsigned aBits = a->getType()->getIntegerBitWidth();
                    unsigned bBits = b->getType()->getIntegerBitWidth();
                    if (aBits < bBits)
                        a = cg_.builder_->CreateSExt(a, b->getType());
                    else if (bBits < aBits)
                        b = cg_.builder_->CreateSExt(b, a->getType());
                }
            }
            llvm::Value *cmp = cg_.builder_->CreateICmpSGT(a, b, "assert.gt");
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *failBB = llvm::BasicBlock::Create(*cg_.context_, "agt.fail", curFunc);
            llvm::BasicBlock *passBB = llvm::BasicBlock::Create(*cg_.context_, "agt.pass", curFunc);
            cg_.builder_->CreateCondBr(cmp, passBB, failBB);
            cg_.builder_->SetInsertPoint(failBB);
            llvm::FunctionType *abortFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {}, false);
            llvm::FunctionCallee abortFn = cg_.module_->getOrInsertFunction("abort", abortFT);
            cg_.builder_->CreateCall(abortFn, {});
            cg_.builder_->CreateUnreachable();
            cg_.builder_->SetInsertPoint(passBB);
            return nullptr;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecuritySanitize(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;
            // Call runtime sad_security_sanitize(const char*) -> char*
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {i8Ptr}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_sanitize", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {val}, "sanitize.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityTimestamp(std::shared_ptr<SIRInstruction> inst)
        {
            // time(NULL) -> i64. The C signature is: time_t time(time_t*)
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            llvm::FunctionType *ft = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("time", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy))}, "time.ret");
            if (inst && inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecuritySecureRandom(std::shared_ptr<SIRInstruction> inst)
        {
            // Use BCryptGenRandom on Windows for cryptographic randomness
            // Signature: NTSTATUS BCryptGenRandom(BCRYPT_ALG_HANDLE, PUCHAR, ULONG, ULONG)
            // We use flag BCRYPT_USE_SYSTEM_PREFERRED_RNG = 2 with NULL handle
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            // Allocate 8 bytes on the stack for the random value
            llvm::Value *buf = cg_.builder_->CreateAlloca(i64Ty, nullptr, "rng.buf");

            // Call BCryptGenRandom(NULL, buf, 8, BCRYPT_USE_SYSTEM_PREFERRED_RNG=2)
            auto *bcrType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy, i32Ty, i32Ty}, false);
            auto bcrFunc = cg_.module_->getOrInsertFunction("BCryptGenRandom", bcrType);
            cg_.builder_->CreateCall(bcrFunc, {
                                              llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)),
                                              buf,
                                              llvm::ConstantInt::get(i32Ty, 8),
                                              llvm::ConstantInt::get(i32Ty, 2) // BCRYPT_USE_SYSTEM_PREFERRED_RNG
                                          });

            // Load the random value
            llvm::Value *result = cg_.builder_->CreateLoad(i64Ty, buf, "rng.val");
            // Make it positive by masking off sign bit
            result = cg_.builder_->CreateAnd(result, llvm::ConstantInt::get(i64Ty, 0x7FFFFFFFFFFFFFFF), "rng.pos");
            if (inst && inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *SecurityBuiltinsCodeGen::emitBuiltinSecurityBase64Encode(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;
            // Call runtime sad_security_base64_encode(const char*) -> char*
            llvm::Type *i8Ptr = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8Ptr, {i8Ptr}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("sad_security_base64_encode", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {val}, "base64.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



}} // namespace Sad::LLVM
