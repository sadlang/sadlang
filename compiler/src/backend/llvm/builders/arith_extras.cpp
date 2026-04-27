/*
 * ============================================================================
 * ArithmeticCodeGen Extras — Phase 7 Step 1.5
 * (AR) المقارنات Gt/Ge + 6 LLVM casts (BitCast/IntToPtr/PtrToInt/Trunc/ZExt/SExt)
 * (EN) Gt/Ge comparisons + 6 LLVM cast operations
 * ============================================================================
 */
#include "builders/arithmetic_codegen.h"
#include "llvm_codegen.h"
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {
        llvm::Value *ArithmeticCodeGen::emitCmpGt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                cg_.reportError("CmpGt instruction requires 2 operands");
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                cg_.reportError("Operands not found for cmpgt");
                return nullptr;
            }

            llvm::Type *leftTy = left->getType();
            llvm::Type *rightTy = right->getType();

            // (AR) معالجة عدم تطابق الأنواع
            // (EN) Handle type mismatches
            if (leftTy != rightTy)
            {
                // (AR) تطبيع المؤشرات أولاً: ptr→i64
                // (EN) Normalize pointers first: ptr→i64
                if (leftTy->isPointerTy())
                {
                    left = cg_.builder_->CreatePtrToInt(left, cg_.getInt64Type(), "ptr2i64.l");
                    leftTy = left->getType();
                }
                if (rightTy->isPointerTy())
                {
                    right = cg_.builder_->CreatePtrToInt(right, cg_.getInt64Type(), "ptr2i64.r");
                    rightTy = right->getType();
                }
                if (leftTy->isIntegerTy() && rightTy->isDoubleTy())
                {
                    left = cg_.builder_->CreateSIToFP(left, rightTy, "sitofp_l");
                }
                else if (leftTy->isDoubleTy() && rightTy->isIntegerTy())
                {
                    right = cg_.builder_->CreateSIToFP(right, leftTy, "sitofp_r");
                }
                else if (leftTy->isIntegerTy(1) && rightTy->isIntegerTy(64))
                {
                    left = cg_.builder_->CreateZExt(left, rightTy, "zext_l");
                }
                else if (leftTy->isIntegerTy(64) && rightTy->isIntegerTy(1))
                {
                    right = cg_.builder_->CreateZExt(right, leftTy, "zext_r");
                }
                leftTy = left->getType();
                rightTy = right->getType();
            }

            llvm::Value *result;
            if (leftTy->isDoubleTy() && rightTy->isDoubleTy())
            {
                result = cg_.builder_->CreateFCmpOGT(left, right, "cmpgttmp");
            }
            else
            {
                result = cg_.builder_->CreateICmpSGT(left, right, "cmpgttmp");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }


        /**
         * إصدار تعليمة مقارنة أكبر من أو يساوي
         * Emit greater than or equal comparison instruction
         *
         * Source: llvm_codegen.h:438
         */
        llvm::Value *ArithmeticCodeGen::emitCmpGe(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                cg_.reportError("CmpGe instruction requires 2 operands");
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                cg_.reportError("Operands not found for cmpge");
                return nullptr;
            }

            llvm::Type *leftTy = left->getType();
            llvm::Type *rightTy = right->getType();

            // (AR) معالجة عدم تطابق الأنواع
            // (EN) Handle type mismatches
            if (leftTy != rightTy)
            {
                // (AR) تطبيع المؤشرات أولاً: ptr→i64
                // (EN) Normalize pointers first: ptr→i64
                if (leftTy->isPointerTy())
                {
                    left = cg_.builder_->CreatePtrToInt(left, cg_.getInt64Type(), "ptr2i64.l");
                    leftTy = left->getType();
                }
                if (rightTy->isPointerTy())
                {
                    right = cg_.builder_->CreatePtrToInt(right, cg_.getInt64Type(), "ptr2i64.r");
                    rightTy = right->getType();
                }
                if (leftTy->isIntegerTy() && rightTy->isDoubleTy())
                {
                    left = cg_.builder_->CreateSIToFP(left, rightTy, "sitofp_l");
                }
                else if (leftTy->isDoubleTy() && rightTy->isIntegerTy())
                {
                    right = cg_.builder_->CreateSIToFP(right, leftTy, "sitofp_r");
                }
                else if (leftTy->isIntegerTy(1) && rightTy->isIntegerTy(64))
                {
                    left = cg_.builder_->CreateZExt(left, rightTy, "zext_l");
                }
                else if (leftTy->isIntegerTy(64) && rightTy->isIntegerTy(1))
                {
                    right = cg_.builder_->CreateZExt(right, leftTy, "zext_r");
                }
                leftTy = left->getType();
                rightTy = right->getType();
            }

            llvm::Value *result;
            if (leftTy->isDoubleTy() && rightTy->isDoubleTy())
            {
                result = cg_.builder_->CreateFCmpOGE(left, right, "cmpgetmp");
            }
            else
            {
                result = cg_.builder_->CreateICmpSGE(left, right, "cmpgetmp");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }


        llvm::Value *ArithmeticCodeGen::emitBitCast(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            // In LLVM opaque pointers era, bitcast between pointers is identity
            // For ptr->ptr, just return the value. For other types, use CreateBitCast.
            llvm::Type *destTy = val->getType(); // default: same type
            if (inst->operands.size() >= 2)
            {
                // If second operand specifies the target type name, use ptr
                destTy = llvm::PointerType::getUnqual(*cg_.context_);
            }

            llvm::Value *result = val;
            if (val->getType() != destTy)
            {
                result = cg_.builder_->CreateBitCast(val, destTy, "bitcast");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }


        llvm::Value *ArithmeticCodeGen::emitIntToPtr(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            llvm::Value *result = cg_.builder_->CreateIntToPtr(val, ptrTy, "inttoptr");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }


        llvm::Value *ArithmeticCodeGen::emitPtrToInt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            auto i64Ty = cg_.getInt64Type();
            llvm::Value *result = cg_.builder_->CreatePtrToInt(val, i64Ty, "ptrtoint");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }


        llvm::Value *ArithmeticCodeGen::emitTrunc(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            // Default: truncate to i32
            llvm::Type *destTy = llvm::Type::getInt32Ty(*cg_.context_);
            if (inst->operands.size() >= 2)
            {
                // Check for target bit width in metadata
                auto &meta = inst->operands[1];
                if (meta.name == "i8")
                    destTy = llvm::Type::getInt8Ty(*cg_.context_);
                else if (meta.name == "i16")
                    destTy = llvm::Type::getInt16Ty(*cg_.context_);
                else if (meta.name == "i1")
                    destTy = llvm::Type::getInt1Ty(*cg_.context_);
            }

            llvm::Value *result = cg_.builder_->CreateTrunc(val, destTy, "trunc");
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }


        llvm::Value *ArithmeticCodeGen::emitZExt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            // Default: extend to i64
            llvm::Type *destTy = cg_.getInt64Type();
            if (inst->operands.size() >= 2)
            {
                auto &meta = inst->operands[1];
                if (meta.name == "i32")
                    destTy = llvm::Type::getInt32Ty(*cg_.context_);
                else if (meta.name == "i16")
                    destTy = llvm::Type::getInt16Ty(*cg_.context_);
            }

            llvm::Value *result = cg_.builder_->CreateZExt(val, destTy, "zext");
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }


        llvm::Value *ArithmeticCodeGen::emitSExt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            // Default: extend to i64
            llvm::Type *destTy = cg_.getInt64Type();
            if (inst->operands.size() >= 2)
            {
                auto &meta = inst->operands[1];
                if (meta.name == "i32")
                    destTy = llvm::Type::getInt32Ty(*cg_.context_);
                else if (meta.name == "i16")
                    destTy = llvm::Type::getInt16Ty(*cg_.context_);
            }

            llvm::Value *result = cg_.builder_->CreateSExt(val, destTy, "sext");
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }


    } // namespace LLVM
} // namespace Sad
