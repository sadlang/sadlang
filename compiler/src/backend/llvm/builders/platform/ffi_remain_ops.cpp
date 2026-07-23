/*
 * ============================================================================
 * FFIRemainCodeGen implementation
 * Phase 7 Step 15
 * ============================================================================
 */
#include "llvm_codegen.h"
#include "builders/platform/ffi_remain_codegen.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>

namespace Sad { namespace LLVM {

        llvm::Value *FFIRemainCodeGen::emitFFISystem(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *cmd = cg_.resolveOperand(inst->operands[0]);
            if (!cmd)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*cg_.context_), {i8p}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("system", ft);
            llvm::Value *result = cg_.builder_->CreateSExt(cg_.builder_->CreateCall(fn, {cmd}, "system.ret"), llvm::Type::getInt64Ty(*cg_.context_));
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *FFIRemainCodeGen::emitFFIGetenv(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *name = cg_.resolveOperand(inst->operands[0]);
            if (!name)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(i8p, {i8p}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("getenv", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {name}, "getenv.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *FFIRemainCodeGen::emitFFIAtoi(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *str = cg_.resolveOperand(inst->operands[0]);
            if (!str)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*cg_.context_), {i8p}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("atoi", ft);
            llvm::Value *result = cg_.builder_->CreateSExt(cg_.builder_->CreateCall(fn, {str}, "atoi.ret"), llvm::Type::getInt64Ty(*cg_.context_));
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *FFIRemainCodeGen::emitFFIAtof(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *str = cg_.resolveOperand(inst->operands[0]);
            if (!str)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*cg_.context_), {i8p}, false);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("atof", ft);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {str}, "atof.ret");
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



        llvm::Value *FFIRemainCodeGen::emitFFISnprintf(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 3)
                return nullptr;
            llvm::Type *i8p = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
            // (AR) وسائط/عائد ‎size_t‎ الهدف (عقد C) — i64 ثابتًا يزيح الخانات على 32-بت.
            llvm::Type *szTy = cg_.getSizeType();
            llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*cg_.context_), {i8p, szTy, i8p}, true);
            llvm::FunctionCallee fn = cg_.module_->getOrInsertFunction("snprintf", ft);
            std::vector<llvm::Value *> args;
            for (auto &op : inst->operands)
            {
                llvm::Value *v = cg_.resolveOperand(op);
                if (v)
                    args.push_back(v);
            }
            llvm::Value *result = cg_.builder_->CreateSExt(cg_.builder_->CreateCall(fn, args, "snprintf.ret"), llvm::Type::getInt64Ty(*cg_.context_));
            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }



}} // namespace Sad::LLVM
