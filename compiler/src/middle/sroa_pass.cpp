/**
 * @file sroa_pass.cpp
 * @brief (AR) تنفيذ تمرير الاستبدال القيمي للتجميعات
 * @brief (EN) Scalar Replacement of Aggregates Pass Implementation
 *
 * @details
 * (AR) خوارزمية من مرحلتين:
 *      المرحلة 1 — التحليل:
 *        - يجمع كل ALLOC/ALLOC_HEAP التي نتيجتها سجل
 *        - يفحص جميع استخدامات السجل: فقط OBJECT_GET/OBJECT_SET بحقل ثابت
 *        - إذا وُجد استخدام غير مدعوم (CALL، STORE، تمرير كمعامل) → غير مؤهل
 *      المرحلة 2 — الاستبدال:
 *        - لكل تخصيص مؤهل: أنشئ سجل فردي لكل حقل
 *        - OBJECT_GET(alloc, "field") → MOVE(result, field_reg)
 *        - OBJECT_SET(alloc, "field", value) → MOVE(field_reg, value)
 *        - ALLOC الأصلي → يُحذف (أو يُحوّل لـ NOP)
 *
 * (EN) Two-phase algorithm:
 *      Phase 1 — Analysis: collect ALLOCs, verify only OBJECT_GET/SET usage
 *      Phase 2 — Replacement: create per-field registers, rewrite accesses
 *
 * @author SadLanguage Compiler Team
 * @date January 2026
 */

#include "middle/sroa_pass.h"
#include <iostream>
#include <sstream>

namespace Sad
{
    namespace Compiler
    {
        namespace Optimizer
        {

            // ============================================================================
            // البناء / Constructor
            // ============================================================================

            SROAPass::SROAPass()
                : OptimizationPass("Scalar Replacement of Aggregates", PassType::TRANSFORMATION), replacedCount_(0)
            {
            }

            SROAPass::~SROAPass() = default;

            // ============================================================================
            // نقطة الدخول / Main Entry Point
            // ============================================================================

            bool SROAPass::runOnFunction(SIR::SIRFunction *function)
            {
                if (!function)
                    return false;

                replacedCount_ = 0;

                // (AR) المرحلة 1: جمع وتحليل التخصيصات
                auto allocInfo = analyzeAllocations(function);
                if (allocInfo.empty())
                    return false;

                // (AR) المرحلة 2: الاستبدال
                bool modified = replaceAggregates(function, allocInfo);

                return modified;
            }

            // ============================================================================
            // (AR) المرحلة 1: تحليل التخصيصات
            // (EN) Phase 1: Analyze allocations
            // ============================================================================

            std::unordered_map<std::string, SROAPass::AllocInfo>
            SROAPass::analyzeAllocations(SIR::SIRFunction *function)
            {
                std::unordered_map<std::string, AllocInfo> result;

                // (AR) الخطوة 1: اجمع كل تعليمات ALLOC/ALLOC_HEAP التي لها نتيجة
                for (const auto &block : function->basicBlocks)
                {
                    if (!block)
                        continue;
                    for (const auto &inst : block->instructions)
                    {
                        if ((inst.opcode == SIR::SIROpcode::ALLOC ||
                             inst.opcode == SIR::SIROpcode::ALLOC_HEAP) &&
                            inst.hasResult() && inst.result)
                        {
                            AllocInfo info;
                            info.allocReg = inst.result->name;
                            info.eligible = true;
                            result[info.allocReg] = info;
                        }
                    }
                }

                if (result.empty())
                    return result;

                // (AR) الخطوة 2: افحص جميع الاستخدامات
                for (const auto &block : function->basicBlocks)
                {
                    if (!block)
                        continue;
                    for (const auto &inst : block->instructions)
                    {
                        // (AR) تجاوز تعليمات ALLOC نفسها
                        if (inst.opcode == SIR::SIROpcode::ALLOC ||
                            inst.opcode == SIR::SIROpcode::ALLOC_HEAP)
                            continue;

                        // (AR) افحص كل معامل يشير لسجل مُخصص
                        for (size_t oi = 0; oi < inst.operands.size(); oi++)
                        {
                            const auto &op = inst.operands[oi];
                            if (op.type != SIR::SIROperandType::REGISTER)
                                continue;

                            auto it = result.find(op.name);
                            if (it == result.end())
                                continue;

                            // (AR) هل الاستخدام مدعوم؟
                            if (inst.opcode == SIR::SIROpcode::OBJECT_GET && oi == 0)
                            {
                                // OBJECT_GET(alloc, "field_name") — مدعوم
                                // (AR) المعامل الثاني هو اسم الحقل
                                if (inst.operands.size() >= 2 &&
                                    inst.operands[1].type == SIR::SIROperandType::CONSTANT &&
                                    inst.operands[1].dataType == SIR::SadTypeKind::String)
                                {
                                    it->second.fields.insert(inst.operands[1].name);
                                }
                                else
                                {
                                    // (AR) فهرس ديناميكي → غير مؤهل
                                    it->second.eligible = false;
                                }
                            }
                            else if (inst.opcode == SIR::SIROpcode::OBJECT_SET && oi == 0)
                            {
                                // OBJECT_SET(alloc, "field_name", value) — مدعوم
                                if (inst.operands.size() >= 2 &&
                                    inst.operands[1].type == SIR::SIROperandType::CONSTANT &&
                                    inst.operands[1].dataType == SIR::SadTypeKind::String)
                                {
                                    it->second.fields.insert(inst.operands[1].name);
                                }
                                else
                                {
                                    it->second.eligible = false;
                                }
                            }
                            else
                            {
                                // (AR) أي استخدام آخر (CALL, STORE, تمرير...) → غير مؤهل
                                it->second.eligible = false;
                            }
                        }
                    }
                }

                // (AR) أزل غير المؤهلين والفارغين
                for (auto it = result.begin(); it != result.end();)
                {
                    if (!it->second.eligible || it->second.fields.empty())
                    {
                        it = result.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }

                return result;
            }

            // ============================================================================
            // (AR) المرحلة 2: تنفيذ الاستبدال
            // (EN) Phase 2: Perform replacement
            // ============================================================================

            bool SROAPass::replaceAggregates(
                SIR::SIRFunction *function,
                const std::unordered_map<std::string, AllocInfo> &allocInfo)
            {
                bool modified = false;

                for (auto &block : function->basicBlocks)
                {
                    if (!block)
                        continue;

                    auto &insts = block->instructions;

                    for (size_t i = 0; i < insts.size(); /* في الداخل */)
                    {
                        auto &inst = insts[i];

                        // === (AR) حذف ALLOC القابلة للاستبدال ===
                        if ((inst.opcode == SIR::SIROpcode::ALLOC ||
                             inst.opcode == SIR::SIROpcode::ALLOC_HEAP) &&
                            inst.hasResult() && inst.result)
                        {
                            auto it = allocInfo.find(inst.result->name);
                            if (it != allocInfo.end())
                            {
                                // (AR) حوّل لـ Nop (حذف آمن)
                                inst.opcode = SIR::SIROpcode::Nop;
                                inst.operands.clear();
                                inst.result.reset();
                                modified = true;
                                replacedCount_++;
                                recordModification();
                            }
                        }

                        // === (AR) استبدال OBJECT_GET ===
                        else if (inst.opcode == SIR::SIROpcode::OBJECT_GET &&
                                 inst.operands.size() >= 2 &&
                                 inst.operands[0].type == SIR::SIROperandType::REGISTER)
                        {
                            auto it = allocInfo.find(inst.operands[0].name);
                            if (it != allocInfo.end() &&
                                inst.operands[1].type == SIR::SIROperandType::CONSTANT &&
                                inst.operands[1].dataType == SIR::SadTypeKind::String)
                            {
                                // (AR) حوّل إلى MOVE(result, field_reg)
                                std::string fieldReg = makeFieldRegName(
                                    it->second.allocReg, inst.operands[1].name);

                                inst.opcode = SIR::SIROpcode::MOVE;
                                inst.operands.clear();
                                SIR::SIROperand srcOp;
                                srcOp.type = SIR::SIROperandType::REGISTER;
                                srcOp.name = fieldReg;
                                inst.operands.push_back(srcOp);
                                modified = true;
                                recordModification();
                            }
                        }

                        // === (AR) استبدال OBJECT_SET ===
                        else if (inst.opcode == SIR::SIROpcode::OBJECT_SET &&
                                 inst.operands.size() >= 3 &&
                                 inst.operands[0].type == SIR::SIROperandType::REGISTER)
                        {
                            auto it = allocInfo.find(inst.operands[0].name);
                            if (it != allocInfo.end() &&
                                inst.operands[1].type == SIR::SIROperandType::CONSTANT &&
                                inst.operands[1].dataType == SIR::SadTypeKind::String)
                            {
                                // (AR) حوّل إلى MOVE(field_reg, value)
                                std::string fieldReg = makeFieldRegName(
                                    it->second.allocReg, inst.operands[1].name);

                                SIR::SIROperand valueOp = inst.operands[2]; // (AR) القيمة
                                inst.opcode = SIR::SIROpcode::MOVE;
                                inst.operands.clear();
                                inst.operands.push_back(valueOp);

                                // (AR) عيّن النتيجة لتكون سجل الحقل
                                SIR::SIROperand resultOp;
                                resultOp.type = SIR::SIROperandType::REGISTER;
                                resultOp.name = fieldReg;
                                inst.result = resultOp;

                                modified = true;
                                recordModification();
                            }
                        }

                        i++;
                    }
                }

                return modified;
            }

            // ============================================================================
            // (AR) دوال مساعدة / (EN) Helper functions
            // ============================================================================

            std::string SROAPass::makeFieldRegName(const std::string &allocReg,
                                                   const std::string &fieldName) const
            {
                // (AR) إنشاء اسم سجل فريد بتركيب اسم التخصيص + اسم الحقل
                return allocReg + "_sroa_" + fieldName;
            }

        } // namespace Optimizer
    } // namespace Compiler
} // namespace Sad
