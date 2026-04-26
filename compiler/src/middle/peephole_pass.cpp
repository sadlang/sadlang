/**
 * @file peephole_pass.cpp
 * @brief (AR) تنفيذ تمرير التحسين النقطي
 * @brief (EN) Peephole Optimization Pass Implementation
 *
 * @details
 * (AR) يطبّق تبسيطات جبرية محلية لتقليل عدد التعليمات.
 *      يعمل على كل كتلة أساسية بشكل مستقل.
 *      التبسيطات المدعومة:
 *        - هوية الجمع/الطرح: x ± 0 → x
 *        - هوية الضرب/القسمة: x * 1 → x, x / 1 → x
 *        - تصفير الضرب: x * 0 → 0
 *        - تصفير AND: x & 0 → 0
 *        - XOR الذاتي: x ^ x → 0
 *        - عكس مزدوج: NOT(NOT(x)) → x, NEG(NEG(x)) → x
 *        - تحويلات عكسية: I64_TO_F64(F64_TO_I64(x)) → x
 *
 * (EN) Applies local algebraic simplifications to reduce instruction count.
 *
 * @author SadLanguage Compiler Team
 * @date January 2026
 */

#include "middle/peephole_pass.h"
#include <iostream>

namespace Sad
{
    namespace Compiler
    {
        namespace Optimizer
        {

            // ============================================================================
            // البناء / Constructor
            // ============================================================================

            PeepholePass::PeepholePass()
                : OptimizationPass("Peephole Optimization", PassType::TRANSFORMATION), simplificationCount_(0)
            {
            }

            PeepholePass::~PeepholePass() = default;

            // ============================================================================
            // نقطة الدخول / Main Entry Point
            // ============================================================================

            bool PeepholePass::runOnFunction(SIR::SIRFunction *function)
            {
                if (!function)
                    return false;

                simplificationCount_ = 0;
                bool modified = false;

                for (auto &blockPtr : function->basicBlocks)
                {
                    if (processBlock(blockPtr.get()))
                    {
                        modified = true;
                    }
                }

                return modified;
            }

            // ============================================================================
            // معالجة الكتلة / Block Processing
            // ============================================================================

            bool PeepholePass::processBlock(SIR::SIRBasicBlock *block)
            {
                if (!block)
                    return false;

                bool modified = false;

                for (size_t i = 0; i < block->instructions.size(); i++)
                {
                    auto &inst = block->instructions[i];

                    if (tryIdentitySimplification(inst))
                    {
                        modified = true;
                        simplificationCount_++;
                        recordModification();
                        continue;
                    }
                    if (tryZeroSimplification(inst))
                    {
                        modified = true;
                        simplificationCount_++;
                        recordModification();
                        continue;
                    }
                    if (tryDoubleNegation(inst, block))
                    {
                        modified = true;
                        simplificationCount_++;
                        recordModification();
                        continue;
                    }
                    if (tryRedundantCast(inst, block))
                    {
                        modified = true;
                        simplificationCount_++;
                        recordModification();
                        continue;
                    }
                }

                return modified;
            }

            // ============================================================================
            // (AR) قاعدة الهوية: x+0→x, x-0→x, x*1→x, x/1→x
            // (EN) Identity rule: x+0→x, x-0→x, x*1→x, x/1→x
            // ============================================================================

            bool PeepholePass::tryIdentitySimplification(SIR::SIRInstruction &inst)
            {
                using SIR::SIROpcode;

                if (inst.operands.size() < 2)
                    return false;

                const auto &left = inst.operands[0];
                const auto &right = inst.operands[1];

                switch (inst.opcode)
                {
                // x + 0 → x, 0 + x → x
                case SIROpcode::ADD_I64:
                case SIROpcode::ADD_F64:
                    if (isZeroConstant(right))
                    {
                        replaceWithOperand(inst, left);
                        return true;
                    }
                    if (isZeroConstant(left))
                    {
                        replaceWithOperand(inst, right);
                        return true;
                    }
                    break;

                // x - 0 → x
                case SIROpcode::SUB_I64:
                case SIROpcode::SUB_F64:
                    if (isZeroConstant(right))
                    {
                        replaceWithOperand(inst, left);
                        return true;
                    }
                    break;

                // x * 1 → x, 1 * x → x
                case SIROpcode::MUL_I64:
                case SIROpcode::MUL_F64:
                    if (isOneConstant(right))
                    {
                        replaceWithOperand(inst, left);
                        return true;
                    }
                    if (isOneConstant(left))
                    {
                        replaceWithOperand(inst, right);
                        return true;
                    }
                    break;

                // x / 1 → x
                case SIROpcode::DIV_I64:
                case SIROpcode::DIV_F64:
                case SIROpcode::FLOOR_DIV_I64:
                    if (isOneConstant(right))
                    {
                        replaceWithOperand(inst, left);
                        return true;
                    }
                    break;

                // x | 0 → x, 0 | x → x
                case SIROpcode::OR:
                    if (isZeroConstant(right))
                    {
                        replaceWithOperand(inst, left);
                        return true;
                    }
                    if (isZeroConstant(left))
                    {
                        replaceWithOperand(inst, right);
                        return true;
                    }
                    break;

                // x ^ 0 → x, 0 ^ x → x
                case SIROpcode::XOR:
                    if (isZeroConstant(right))
                    {
                        replaceWithOperand(inst, left);
                        return true;
                    }
                    if (isZeroConstant(left))
                    {
                        replaceWithOperand(inst, right);
                        return true;
                    }
                    break;

                // x << 0 → x, x >> 0 → x
                case SIROpcode::SHL:
                case SIROpcode::SHR:
                case SIROpcode::SAR:
                    if (isZeroConstant(right))
                    {
                        replaceWithOperand(inst, left);
                        return true;
                    }
                    break;

                default:
                    break;
                }

                return false;
            }

            // ============================================================================
            // (AR) قاعدة التصفير: x*0→0, x&0→0, x^x→0
            // (EN) Zero rule: x*0→0, x&0→0, x^x→0
            // ============================================================================

            bool PeepholePass::tryZeroSimplification(SIR::SIRInstruction &inst)
            {
                using SIR::SIROpcode;

                if (inst.operands.size() < 2)
                    return false;

                const auto &left = inst.operands[0];
                const auto &right = inst.operands[1];

                switch (inst.opcode)
                {
                // x * 0 → 0, 0 * x → 0
                case SIROpcode::MUL_I64:
                    if (isZeroConstant(right) || isZeroConstant(left))
                    {
                        replaceWithZero(inst);
                        return true;
                    }
                    break;

                // x & 0 → 0, 0 & x → 0
                case SIROpcode::AND:
                    if (isZeroConstant(right) || isZeroConstant(left))
                    {
                        replaceWithZero(inst);
                        return true;
                    }
                    break;

                // x ^ x → 0 (فقط إذا كانا نفس السجل)
                // x - x → 0
                case SIROpcode::XOR:
                case SIROpcode::SUB_I64:
                    if (left.type == SIR::SIROperandType::REGISTER &&
                        right.type == SIR::SIROperandType::REGISTER &&
                        left.name == right.name)
                    {
                        replaceWithZero(inst);
                        return true;
                    }
                    break;

                default:
                    break;
                }

                return false;
            }

            // ============================================================================
            // (AR) عكس مزدوج: NOT(NOT(x))→x, NEG(NEG(x))→x
            // (EN) Double negation: NOT(NOT(x))→x, NEG(NEG(x))→x
            // ============================================================================

            bool PeepholePass::tryDoubleNegation(SIR::SIRInstruction &inst, SIR::SIRBasicBlock *block)
            {
                using SIR::SIROpcode;

                if (inst.operands.empty())
                    return false;

                // (AR) فقط NOT و NEG أحاديّان
                if (inst.opcode != SIROpcode::NOT && inst.opcode != SIROpcode::NEG)
                    return false;

                const auto &operand = inst.operands[0];
                if (operand.type != SIR::SIROperandType::REGISTER)
                    return false;

                // (AR) ابحث عن التعليمة التي تُعرّف هذا السجل
                auto *defInst = findDefiningInstruction(operand.name, block);
                if (!defInst)
                    return false;

                // (AR) هل التعليمة المعرّفة هي نفس العملية؟ NOT(NOT(x)) أو NEG(NEG(x))
                if (defInst->opcode != inst.opcode)
                    return false;
                if (defInst->operands.empty())
                    return false;

                // (AR) استبدل بالمعامل الأصلي x
                replaceWithOperand(inst, defInst->operands[0]);
                return true;
            }

            // ============================================================================
            // (AR) تحويلات عكسية: I64_TO_F64(F64_TO_I64(x))→x, F64_TO_I64(I64_TO_F64(x))→x
            // (EN) Round-trip casts: I64_TO_F64(F64_TO_I64(x))→x
            // ============================================================================

            bool PeepholePass::tryRedundantCast(SIR::SIRInstruction &inst, SIR::SIRBasicBlock *block)
            {
                using SIR::SIROpcode;

                if (inst.operands.empty())
                    return false;

                // (AR) أزواج التحويلات العكسية
                // (AR) ملاحظة: I64_TO_F64(F64_TO_I64(x)) ليس زائداً!
                //      F64_TO_I64 يبتر الجزء العشري (مثلاً: 761.577 → 761)
                //      ثم I64_TO_F64 يحول الناتج (761 → 761.0)
                //      النتيجة 761.0 ≠ 761.577 — لذا لا يجوز حذفهما
                // (EN) Note: I64_TO_F64(F64_TO_I64(x)) is NOT redundant!
                //      F64_TO_I64 truncates the fractional part (e.g. 761.577 → 761)
                //      then I64_TO_F64 converts back (761 → 761.0)
                //      Result 761.0 ≠ 761.577 — so this pair must NOT be eliminated
                SIROpcode expectedInner = SIROpcode::Nop;
                if (inst.opcode == SIROpcode::F64_TO_I64)
                {
                    expectedInner = SIROpcode::I64_TO_F64;
                }
                else if (inst.opcode == SIROpcode::I64_TO_BOOL)
                {
                    expectedInner = SIROpcode::BOOL_TO_I64;
                }
                else if (inst.opcode == SIROpcode::BOOL_TO_I64)
                {
                    expectedInner = SIROpcode::I64_TO_BOOL;
                }
                else
                {
                    return false;
                }

                const auto &operand = inst.operands[0];
                if (operand.type != SIR::SIROperandType::REGISTER)
                    return false;

                auto *defInst = findDefiningInstruction(operand.name, block);
                if (!defInst)
                    return false;

                if (defInst->opcode != expectedInner)
                    return false;
                if (defInst->operands.empty())
                    return false;

                replaceWithOperand(inst, defInst->operands[0]);
                return true;
            }

            // ============================================================================
            // (AR) دوال مساعدة / (EN) Helper functions
            // ============================================================================

            bool PeepholePass::isZeroConstant(const SIR::SIROperand &op) const
            {
                if (op.type == SIR::SIROperandType::CONSTANT && op.dataType == SIR::SadTypeKind::Integer && op.intValue == 0)
                    return true;
                if (op.type == SIR::SIROperandType::CONSTANT && op.dataType == SIR::SadTypeKind::Float && op.floatValue == 0.0)
                    return true;
                return false;
            }

            bool PeepholePass::isOneConstant(const SIR::SIROperand &op) const
            {
                if (op.type == SIR::SIROperandType::CONSTANT && op.dataType == SIR::SadTypeKind::Integer && op.intValue == 1)
                    return true;
                if (op.type == SIR::SIROperandType::CONSTANT && op.dataType == SIR::SadTypeKind::Float && op.floatValue == 1.0)
                    return true;
                return false;
            }

            void PeepholePass::replaceWithOperand(SIR::SIRInstruction &inst, const SIR::SIROperand &op)
            {
                // (AR) حوّل التعليمة إلى MOVE بسيط: %result = MOVE operand
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands.clear();
                inst.operands.push_back(op);
            }

            void PeepholePass::replaceWithZero(SIR::SIRInstruction &inst)
            {
                // (AR) حوّل التعليمة إلى MOVE ثابت: %result = MOVE 0
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands.clear();
                inst.operands.push_back(SIR::SIROperand::ConstantI64(0));
            }

            SIR::SIRInstruction *PeepholePass::findDefiningInstruction(
                const std::string &regName, SIR::SIRBasicBlock *block)
            {
                if (!block)
                    return nullptr;

                // (AR) بحث عكسي في نفس الكتلة عن التعليمة التي تعرّف هذا السجل
                for (auto &inst : block->instructions)
                {
                    if (inst.hasResult() && inst.result && inst.result->name == regName)
                    {
                        return &inst;
                    }
                }
                return nullptr;
            }

        } // namespace Optimizer
    } // namespace Compiler
} // namespace Sad
