/*
 * ============================================================================
 * LLVM IR Code Generator - Builtin Functions
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "builders/builtin_funcs_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
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

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {

        llvm::Value *BuiltinFuncsCodeGen::emitBuiltinAssert(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError("ASSERT requires 1 operand (condition)");
                return nullptr;
            }

            llvm::Value *cond = cg_.resolveOperand(inst->operands[0]);
            if (!cond)
                return nullptr;

            // Convert to i1 if not already
            if (!cond->getType()->isIntegerTy(1))
            {
                cond = cg_.builder_->CreateICmpNE(cond,
                                              llvm::ConstantInt::get(cond->getType(), 0), "assert.cond");
            }

            // Create basic blocks
            llvm::Function *fn = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *failBB = llvm::BasicBlock::Create(*cg_.context_, "assert.fail", fn);
            llvm::BasicBlock *contBB = llvm::BasicBlock::Create(*cg_.context_, "assert.cont", fn);

            cg_.builder_->CreateCondBr(cond, contBB, failBB);

            // Fail block: print error and abort
            cg_.builder_->SetInsertPoint(failBB);
            auto *printfType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(*cg_.context_),
                {llvm::PointerType::getUnqual(*cg_.context_)}, true);
            auto printfFunc = cg_.module_->getOrInsertFunction("printf", printfType);

            std::string msg = "Assertion failed";
            if (inst->operands.size() >= 2)
            {
                msg = "Assertion failed: " + inst->operands[1].name;
            }
            llvm::Value *msgStr = cg_.builder_->CreateGlobalStringPtr(msg + "\n", "assert.msg");
            cg_.builder_->CreateCall(printfFunc, {msgStr});

            auto *abortType = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {}, false);
            auto abortFunc = cg_.module_->getOrInsertFunction("abort", abortType);
            cg_.builder_->CreateCall(abortFunc, {});
            cg_.builder_->CreateUnreachable();

            // Continue block
            cg_.builder_->SetInsertPoint(contBB);

            return llvm::ConstantInt::get(cg_.getInt64Type(), 0);
        }

        llvm::Value *BuiltinFuncsCodeGen::emitBuiltinDebug(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                return llvm::ConstantInt::get(cg_.getInt64Type(), 0);
            }

            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            auto *printfType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(*cg_.context_),
                {llvm::PointerType::getUnqual(*cg_.context_)}, true);
            auto printfFunc = cg_.module_->getOrInsertFunction("printf", printfType);

            if (val->getType()->isDoubleTy())
            {
                llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("[DEBUG] %g\n", "debug_fmt_f64");
                cg_.builder_->CreateCall(printfFunc, {fmt, val});
            }
            else if (val->getType()->isPointerTy())
            {
                llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("[DEBUG] %s\n", "debug_fmt_str");
                cg_.builder_->CreateCall(printfFunc, {fmt, val});
            }
            else
            {
                llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("[DEBUG] %lld\n", "debug_fmt_i64");
                cg_.builder_->CreateCall(printfFunc, {fmt, val});
            }

            return val;
        }

        // ============================================================================
        // emitPhi - عقدة فاي / Phi node (SSA form)
        // ============================================================================
        llvm::Value *BuiltinFuncsCodeGen::emitPhi(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2 || inst->operands.size() % 2 != 0)
            {
                cg_.reportError("PHI requires pairs of (value, block) operands");
                return nullptr;
            }

            // Determine phi type from result
            llvm::Type *phiType = cg_.getInt64Type(); // default
            if (inst->result.has_value())
            {
                if (inst->result->dataType == SadTypeKind::Float)
                    phiType = llvm::Type::getDoubleTy(*cg_.context_);
                else if (inst->result->dataType == SadTypeKind::Boolean)
                    phiType = llvm::Type::getInt1Ty(*cg_.context_);
                else if (inst->result->dataType == SadTypeKind::String || inst->result->dataType == SadTypeKind::Pointer)
                    phiType = llvm::PointerType::getUnqual(*cg_.context_);
            }

            unsigned numIncoming = inst->operands.size() / 2;
            llvm::PHINode *phi = cg_.builder_->CreatePHI(phiType, numIncoming, "phi");

            llvm::Function *func = cg_.builder_->GetInsertBlock()->getParent();
            for (unsigned i = 0; i < inst->operands.size(); i += 2)
            {
                const std::string &bbName = inst->operands[i + 1].name;

                // Find the basic block by name
                llvm::BasicBlock *bb = nullptr;
                for (auto &block : *func)
                {
                    if (block.getName() == bbName)
                    {
                        bb = &block;
                        break;
                    }
                }
                if (bb)
                {
                    // (AR) مهم: حل قيمة الـ incoming داخل predecessor نفسه
                    //      لتفادي إنشاء load في merge block مما يكسر قواعد dominance في LLVM.
                    // (EN) Important: resolve incoming value inside its predecessor block
                    //      to avoid generating loads in merge block, which breaks dominance.
                    llvm::Value *val = nullptr;
                    {
                        llvm::IRBuilderBase::InsertPointGuard guard(*cg_.builder_);
                        if (bb->getTerminator())
                        {
                            cg_.builder_->SetInsertPoint(bb->getTerminator());
                        }
                        else
                        {
                            cg_.builder_->SetInsertPoint(bb);
                        }
                        val = cg_.resolveOperand(inst->operands[i]);
                    }

                    if (!val)
                    {
                        // (AR) قيمة افتراضية عند فشل cg_.resolveOperand — يمنع "PHINode must have entry for each predecessor"
                        // (EN) Fallback when cg_.resolveOperand fails — prevents PHI predecessor mismatch error
                        val = llvm::Constant::getNullValue(phiType);
                    }
                    phi->addIncoming(val, bb);
                }
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = phi;
            }
            return phi;
        }

        // ============================================================================
        // emitBuiltinTypeOf - نوع_المتغير / typeof
        // ============================================================================

        // ============================================================================
        // New stdlib builtins - دوال المكتبة القياسية الجديدة
        // ============================================================================

        llvm::Value *BuiltinFuncsCodeGen::emitBuiltinIsType(std::shared_ptr<SIRInstruction> inst, const std::string &typeName)
        {
            // (AR) في الترجمة الساكنة، نوع المتغير معروف وقت الترجمة
            // (EN) In static compilation, variable type is known at compile time
            if (!inst || inst->operands.empty())
            {
                cg_.reportError("is_type requires 1 operand");
                return nullptr;
            }

            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            llvm::Value *result;
            if (typeName == "integer")
            {
                // Check if the operand type is integer
                result = llvm::ConstantInt::get(cg_.getInt64Type(), val->getType()->isIntegerTy() ? 1 : 0);
            }
            else if (typeName == "float")
            {
                result = llvm::ConstantInt::get(cg_.getInt64Type(), val->getType()->isDoubleTy() ? 1 : 0);
            }
            else if (typeName == "string")
            {
                result = llvm::ConstantInt::get(cg_.getInt64Type(), val->getType()->isPointerTy() ? 1 : 0);
            }
            else if (typeName == "array")
            {
                result = llvm::ConstantInt::get(cg_.getInt64Type(), val->getType()->isPointerTy() ? 1 : 0);
            }
            else
            {
                result = llvm::ConstantInt::get(cg_.getInt64Type(), 0);
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *BuiltinFuncsCodeGen::emitBuiltinToBool(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError("to_bool requires 1 operand");
                return nullptr;
            }

            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            llvm::Value *result;
            if (val->getType()->isIntegerTy())
            {
                // Non-zero = true
                llvm::Value *cmp = cg_.builder_->CreateICmpNE(val,
                                                          llvm::ConstantInt::get(val->getType(), 0), "tobool.cmp");
                result = cg_.builder_->CreateZExt(cmp, cg_.getInt64Type(), "tobool");
            }
            else if (val->getType()->isDoubleTy())
            {
                // Non-zero float = true
                llvm::Value *cmp = cg_.builder_->CreateFCmpONE(val,
                                                           llvm::ConstantFP::get(cg_.getDoubleType(), 0.0), "tobool.cmp");
                result = cg_.builder_->CreateZExt(cmp, cg_.getInt64Type(), "tobool");
            }
            else if (val->getType()->isPointerTy())
            {
                // Non-null pointer = true
                llvm::Value *cmp = cg_.builder_->CreateICmpNE(
                    cg_.builder_->CreatePtrToInt(val, cg_.getInt64Type()),
                    llvm::ConstantInt::get(cg_.getInt64Type(), 0), "tobool.cmp");
                result = cg_.builder_->CreateZExt(cmp, cg_.getInt64Type(), "tobool");
            }
            else
            {
                result = llvm::ConstantInt::get(cg_.getInt64Type(), 0);
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *BuiltinFuncsCodeGen::emitBuiltinReadLine(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) قراءة سطر كامل من stdin باستخدام fgets
            // (EN) Read a full line from stdin using fgets

            // char buf[1024]; scanf("%1023[^\n]", buf); getchar(); return strdup(buf);
            auto *charTy = llvm::Type::getInt8Ty(*cg_.context_);
            auto *bufTy = llvm::ArrayType::get(charTy, 1024);
            llvm::Value *buf = cg_.builder_->CreateAlloca(bufTy, nullptr, "readline.buf");
            llvm::Value *bufPtr = cg_.builder_->CreateBitCast(buf, llvm::PointerType::getUnqual(*cg_.context_), "readline.ptr");

            // Use scanf("%1023[^\n]", buf) - read until newline
            auto *scanfTy = llvm::FunctionType::get(cg_.getInt32Type(), {llvm::PointerType::getUnqual(*cg_.context_)}, true);
            auto scanfFn = cg_.module_->getOrInsertFunction("scanf", scanfTy);

            llvm::Value *fmtStr = cg_.builder_->CreateGlobalStringPtr("%1023[^\n]", "readline.fmt");
            cg_.builder_->CreateCall(scanfFn, {fmtStr, bufPtr});

            // Consume the newline
            auto getcTy = llvm::FunctionType::get(cg_.getInt32Type(), {}, false);
            auto getcFn = cg_.module_->getOrInsertFunction("getchar", getcTy);
            cg_.builder_->CreateCall(getcFn, {});

            // Duplicate the string with strdup
            auto *strdupTy = llvm::FunctionType::get(
                llvm::PointerType::getUnqual(*cg_.context_),
                {llvm::PointerType::getUnqual(*cg_.context_)}, false);
            auto strdupFn = cg_.module_->getOrInsertFunction("_strdup", strdupTy);
            llvm::Value *result = cg_.builder_->CreateCall(strdupFn, {bufPtr}, "readline.dup");

            if (inst && inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *BuiltinFuncsCodeGen::emitBuiltinClearScreen(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) مسح الشاشة باستخدام system("cls") على ويندوز
            // (EN) Clear screen using system("cls") on Windows
            auto *systemTy = llvm::FunctionType::get(cg_.getInt32Type(),
                                                     {llvm::PointerType::getUnqual(*cg_.context_)}, false);
            auto systemFn = cg_.module_->getOrInsertFunction("system", systemTy);

#ifdef _WIN32
            llvm::Value *cmd = cg_.builder_->CreateGlobalStringPtr("cls", "cls.str");
#else
            llvm::Value *cmd = cg_.builder_->CreateGlobalStringPtr("clear", "clear.str");
#endif

            cg_.builder_->CreateCall(systemFn, {cmd});
            return nullptr;
        }

        llvm::Value *BuiltinFuncsCodeGen::emitBuiltinSum(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) حساب مجموع عناصر مصفوفة
            // (EN) Calculate sum of array elements
            // For static compilation, we implement a loop over the array
            if (!inst || inst->operands.empty())
            {
                cg_.reportError("SUM requires 1 operand (array)");
                return nullptr;
            }

            llvm::Value *arr = cg_.resolveOperand(inst->operands[0]);
            if (!arr)
                return nullptr;

            // Get array size (stored at offset 0 of array struct)
            llvm::Value *sizePtr = cg_.builder_->CreateStructGEP(
                llvm::StructType::get(*cg_.context_, {cg_.getInt64Type(), llvm::PointerType::getUnqual(*cg_.context_)}),
                arr, 0, "sum.sizeptr");
            llvm::Value *size = cg_.builder_->CreateLoad(cg_.getInt64Type(), sizePtr, "sum.size");

            // Get data pointer (offset 1)
            llvm::Value *dataPtr = cg_.builder_->CreateStructGEP(
                llvm::StructType::get(*cg_.context_, {cg_.getInt64Type(), llvm::PointerType::getUnqual(*cg_.context_)}),
                arr, 1, "sum.dataptr");
            llvm::Value *data = cg_.builder_->CreateLoad(llvm::PointerType::getUnqual(*cg_.context_), dataPtr, "sum.data");

            // Loop to sum all elements
            llvm::Function *fn = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *preheader = cg_.builder_->GetInsertBlock();
            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*cg_.context_, "sum.loop", fn);
            llvm::BasicBlock *exitBB = llvm::BasicBlock::Create(*cg_.context_, "sum.exit", fn);

            cg_.builder_->CreateBr(loopBB);
            cg_.builder_->SetInsertPoint(loopBB);

            llvm::PHINode *idx = cg_.builder_->CreatePHI(cg_.getInt64Type(), 2, "sum.idx");
            llvm::PHINode *acc = cg_.builder_->CreatePHI(cg_.getInt64Type(), 2, "sum.acc");
            idx->addIncoming(llvm::ConstantInt::get(cg_.getInt64Type(), 0), preheader);
            acc->addIncoming(llvm::ConstantInt::get(cg_.getInt64Type(), 0), preheader);

            // Load element at idx
            llvm::Value *elemPtr = cg_.builder_->CreateGEP(cg_.getInt64Type(), data, {idx}, "sum.elemptr");
            llvm::Value *elem = cg_.builder_->CreateLoad(cg_.getInt64Type(), elemPtr, "sum.elem");
            llvm::Value *newAcc = cg_.builder_->CreateAdd(acc, elem, "sum.newacc");

            // Increment idx
            llvm::Value *nextIdx = cg_.builder_->CreateAdd(idx, llvm::ConstantInt::get(cg_.getInt64Type(), 1), "sum.nextidx");
            llvm::Value *done = cg_.builder_->CreateICmpSGE(nextIdx, size, "sum.done");
            cg_.builder_->CreateCondBr(done, exitBB, loopBB);

            idx->addIncoming(nextIdx, loopBB);
            acc->addIncoming(newAcc, loopBB);

            cg_.builder_->SetInsertPoint(exitBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = newAcc;
            }
            return newAcc;
        }

    } // namespace LLVM
} // namespace Sad
