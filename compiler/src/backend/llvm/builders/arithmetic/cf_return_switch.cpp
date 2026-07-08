/*
 * ============================================================================
 * ControlFlowCodeGen — Return + Switch (مستخرجة من llvm_codegen_output.cpp)
 * Phase 7 Step 3
 * ============================================================================
 */
#include "builders/arithmetic/controlflow_codegen.h"
#include "llvm_codegen.h"
#include "sad_dyn_repr.h"
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {
        llvm::Value *ControlFlowCodeGen::emitReturn(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            // Source: SIROpcode::RET_VOID is at sir_types.h:158
            if (inst->opcode == SIROpcode::RET_VOID)
            {
                // (AR) إذا كانت الدالة ترجع قيمة (ليست void)، نرجع قيمة افتراضية
                //      هذا يحدث في الكود الميت بعد ارمي (throw)
                // (EN) If function returns a value (not void), return a default value
                //      This happens in dead code after throw statements
                if (cg_.builder_->GetInsertBlock() && cg_.builder_->GetInsertBlock()->getParent())
                {
                    llvm::Type *retType = cg_.builder_->GetInsertBlock()->getParent()->getReturnType();
                    if (!retType->isVoidTy())
                    {
                        return cg_.builder_->CreateRet(llvm::Constant::getNullValue(retType));
                    }
                }
                return cg_.builder_->CreateRetVoid();
            }

            if (inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "Return"}});
                return nullptr;
            }

            const SIROperand &operand = inst->operands[0];
            llvm::Value *retValue = nullptr;

            // تحقق إذا كان المعامل ثابتاً / Check if operand is a constant
            if (operand.type == SIROperandType::CONSTANT)
            {
                // إنشاء ثابت LLVM / Create LLVM constant
                switch (operand.dataType)
                {
                case SadTypeKind::Integer:
                    retValue = llvm::ConstantInt::get(
                        llvm::Type::getInt64Ty(*cg_.context_),
                        (uint64_t)operand.intValue,
                        true);
                    break;
                case SadTypeKind::Float:
                    retValue = llvm::ConstantFP::get(
                        llvm::Type::getDoubleTy(*cg_.context_),
                        operand.floatValue);
                    break;
                case SadTypeKind::Boolean:
                    retValue = llvm::ConstantInt::get(
                        llvm::Type::getInt1Ty(*cg_.context_),
                        operand.boolValue ? 1 : 0,
                        false);
                    break;
                case SadTypeKind::String:
                    retValue = cg_.builder_->CreateGlobalStringPtr(operand.name, "str.ret");
                    break;
                default:
                    retValue = llvm::ConstantInt::get(
                        llvm::Type::getInt64Ty(*cg_.context_),
                        (uint64_t)0,
                        true);
                    break;
                }
            }
            else
            {
                // استخدم cg_.resolveOperand بدلاً من البحث المباشر — يدعم المتغيرات العامة
                // Use cg_.resolveOperand instead of direct lookup — supports global variables
                retValue = cg_.resolveOperand(operand);
            }

            if (!retValue)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_UNDEFINED_REF, {{"detail", std::string("Return value not found:") + operand.name}});
                return nullptr;
            }

            // (AR) تطابق نوع القيمة مع نوع إرجاع الدالة
            // (EN) Match return value type with function return type
            if (cg_.builder_->GetInsertBlock() && cg_.builder_->GetInsertBlock()->getParent())
            {
                llvm::Function *fn = cg_.builder_->GetInsertBlock()->getParent();
                llvm::Type *retType = fn->getReturnType();

                // (AR) إذا الدالة void لكن هناك قيمة إرجاع — تجاهل القيمة وأرجع void
                // (EN) If function is void but has return value — ignore value and return void
                if (retType->isVoidTy())
                {
                    return cg_.builder_->CreateRetVoid();
                }

                if (retType != retValue->getType())
                {
                    // (AR) ISSUE-076: نوع إرجاع %SadDyn وقيمةٌ محسوسة ⇒ غلّف (toDyn)؛ والعكس ⇒ فُكّ.
                    // (EN) ISSUE-076: %SadDyn return type with a concrete value ⇒ pack (toDyn); the
                    //      converse ⇒ unpack. Driven by type mismatch, like the rest of this chain.
                    llvm::StructType *dynTy = getSadDynType(*cg_.context_);
                    if (retType == dynTy && retValue->getType() != dynTy)
                    {
                        retValue = toDyn(cg_, retValue, operand.dataType);
                    }
                    else if (retValue->getType() == dynTy && retType != dynTy)
                    {
                        if (retType->isDoubleTy())
                            retValue = unpackDouble(cg_, retValue);
                        else if (retType->isPointerTy())
                            retValue = unpackPtr(cg_, retValue);
                        else if (retType->isIntegerTy(1))
                            retValue = cg_.builder_->CreateTrunc(
                                dynPayloadI64(cg_, retValue), llvm::Type::getInt1Ty(*cg_.context_), "ret_dyn_i1");
                        else
                            retValue = dynPayloadI64(cg_, retValue);
                    }
                    else if (retType->isDoubleTy() && retValue->getType()->isIntegerTy())
                    {
                        retValue = cg_.builder_->CreateSIToFP(retValue, retType, "ret_i2f");
                    }
                    else if (retType->isIntegerTy(64) && retValue->getType()->isDoubleTy())
                    {
                        retValue = cg_.builder_->CreateBitCast(retValue, retType, "ret_bitcast");
                    }
                    else if (retType->isIntegerTy(64) && retValue->getType()->isPointerTy())
                    {
                        // (AR) ptr → i64: الدالة تُرجع مؤشر لكن نوعها i64
                        // (EN) ptr → i64: function returns pointer but type is i64
                        retValue = cg_.builder_->CreatePtrToInt(retValue, retType, "ret_p2i");
                    }
                    else if (retType->isIntegerTy(64) && retValue->getType()->isIntegerTy(1))
                    {
                        // (AR) i1 → i64: الدالة تُرجع bool لكن نوعها i64
                        // (EN) i1 → i64: function returns bool but type is i64
                        retValue = cg_.builder_->CreateZExt(retValue, retType, "ret_b2i");
                    }
                    else if (retType->isPointerTy() && retValue->getType()->isIntegerTy(64))
                    {
                        // (AR) i64 → ptr: الدالة تُرجع مؤشر والقيمة i64
                        // (EN) i64 → ptr: function returns ptr and value is i64
                        retValue = cg_.builder_->CreateIntToPtr(retValue, retType, "ret_i2p");
                    }
                    else if (retType->isIntegerTy(1) && retValue->getType()->isIntegerTy(64))
                    {
                        // (AR) i64 → i1: الدالة تُرجع bool والقيمة i64
                        // (EN) i64 → i1: function returns bool and value is i64
                        retValue = cg_.builder_->CreateICmpNE(retValue, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0), "ret_i2b");
                    }
                    else if (retType->isIntegerTy(1) && retValue->getType()->isDoubleTy())
                    {
                        retValue = cg_.builder_->CreateFPToSI(retValue, llvm::Type::getInt64Ty(*cg_.context_), "ret_f2i");
                        retValue = cg_.builder_->CreateICmpNE(retValue, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0), "ret_b");
                    }
                    else if (retType->isIntegerTy(64) && retValue->getType()->isIntegerTy())
                    {
                        // (AR) أي نوع integer آخر (i8, i16, i32) → i64
                        // (EN) Any other integer type → i64
                        retValue = cg_.builder_->CreateZExt(retValue, retType, "ret_zext");
                    }
                }
            }

            return cg_.builder_->CreateRet(retValue);
        }


        /**
         * إصدار تعليمة switch
         * Emit switch instruction
         *
         * Source: llvm_codegen.h:457
         */
        llvm::Value *ControlFlowCodeGen::emitSwitch(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            // (AR) تعليمة switch/طابق - تنفيذ كامل
            // (EN) Switch/match instruction - full implementation
            //
            // الصيغة في SIR:
            //   operands[0] = القيمة المُطابقة (condition)
            //   operands[1..N-1] = قيم الحالات (case values) — CONSTANT
            //   operands الأخيرة = label الافتراضي (default)
            //   labels تأتي من operand.name حيث type == LABEL
            //
            // SIR format:
            //   operands[0] = switch condition value
            //   operands[1..N-1] = case constant values with label names
            //   last label operand = default label

            if (inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "SWITCH"}});
                return nullptr;
            }

            // (AR) حل قيمة الشرط
            // (EN) Resolve condition value
            llvm::Value *condVal = cg_.resolveOperand(inst->operands[0]);
            if (!condVal)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "SWITCH"}});
                return nullptr;
            }

            // (AR) تأكد أن الشرط من نوع integer
            // (EN) Ensure condition is integer type
            if (!condVal->getType()->isIntegerTy())
            {
                if (condVal->getType()->isDoubleTy())
                {
                    condVal = cg_.builder_->CreateFPToSI(condVal, llvm::Type::getInt64Ty(*cg_.context_), "switch.cond.i64");
                }
                else if (condVal->getType()->isPointerTy())
                {
                    condVal = cg_.builder_->CreatePtrToInt(condVal, llvm::Type::getInt64Ty(*cg_.context_), "switch.cond.ptrtoi");
                }
                else
                {
                    cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_TYPE_CONSTRAINT, {{"detail", "SWITCH"}});
                    return nullptr;
                }
            }

            // (AR) توحيد حجم العدد الصحيح إلى i64
            // (EN) Normalize integer size to i64
            if (condVal->getType() != llvm::Type::getInt64Ty(*cg_.context_))
            {
                condVal = cg_.builder_->CreateIntCast(condVal, llvm::Type::getInt64Ty(*cg_.context_), true, "switch.cond.ext");
            }

            // (AR) جمع الحالات والملصقات
            // (EN) Collect cases and labels
            // الهيكل: [condition, case1_val, case1_label, case2_val, case2_label, ..., default_label]
            // أو: [condition, label_operands...]
            // نبحث عن عناصر LABEL و CONSTANT

            std::string defaultLabel;
            std::vector<std::pair<int64_t, std::string>> cases; // (value, label)

            for (size_t i = 1; i < inst->operands.size(); i++)
            {
                const auto &op = inst->operands[i];
                if (op.type == SIROperandType::LABEL)
                {
                    // (AR) إذا كان هناك قيمة ثابتة قبله، فهو ملصق حالة
                    // (EN) If there's a constant value before it, it's a case label
                    if (i > 1 && inst->operands[i - 1].type == SIROperandType::CONSTANT)
                    {
                        cases.push_back({inst->operands[i - 1].intValue, op.name});
                    }
                    else
                    {
                        // (AR) آخر ملصق بدون قيمة ثابتة = الحالة الافتراضية
                        // (EN) Last label without constant value = default case
                        defaultLabel = op.name;
                    }
                }
            }

            // (AR) إذا لم نجد حالة افتراضية، نستخدم آخر ملصق
            // (EN) If no default found, use last label
            if (defaultLabel.empty() && !cases.empty())
            {
                defaultLabel = cases.back().second;
                cases.pop_back();
            }

            // (AR) إذا لا يزال فارغاً، ابحث عن أي ملصق
            // (EN) If still empty, search for any label
            if (defaultLabel.empty())
            {
                for (size_t i = inst->operands.size(); i > 0; i--)
                {
                    if (inst->operands[i - 1].type == SIROperandType::LABEL)
                    {
                        defaultLabel = inst->operands[i - 1].name;
                        break;
                    }
                }
            }

            // (AR) البحث عن الكتل الأساسية أو إنشائها
            // (EN) Find or create basic blocks
            llvm::Function *currentFunc = cg_.builder_->GetInsertBlock()->getParent();

            auto findOrCreateBlock = [&](const std::string &name) -> llvm::BasicBlock *
            {
                auto it = cg_.context_info_.basicBlocks.find(name);
                if (it != cg_.context_info_.basicBlocks.end())
                {
                    return it->second;
                }
                llvm::BasicBlock *bb = llvm::BasicBlock::Create(*cg_.context_, name, currentFunc);
                cg_.context_info_.basicBlocks[name] = bb;
                return bb;
            };

            llvm::BasicBlock *defaultBB = findOrCreateBlock(
                defaultLabel.empty() ? "switch.default" : defaultLabel);

            // (AR) إنشاء تعليمة switch في LLVM
            // (EN) Create LLVM switch instruction
            llvm::SwitchInst *switchInst = cg_.builder_->CreateSwitch(condVal, defaultBB, cases.size());

            for (const auto &[caseVal, caseLabel] : cases)
            {
                llvm::BasicBlock *caseBB = findOrCreateBlock(caseLabel);
                switchInst->addCase(
                    llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), caseVal, true),
                    caseBB);
            }

            return switchInst;
        }


    } // namespace LLVM
} // namespace Sad
