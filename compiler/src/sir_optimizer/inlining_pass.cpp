/**
 * @file inlining_pass.cpp
 * @brief (AR) تنفيذ تمرير إدراج الدوال
 * @brief (EN) Function Inlining Pass Implementation
 *
 * @details
 * (AR) يستبدل استدعاءات الدوال الصغيرة بجسم الدالة مباشرة:
 *      1. يفحص كل CALL في الدالة
 *      2. يبحث عن الدالة المستدعاة في الوحدة
 *      3. يتحقق من قابلية الإدراج (حجم، عدم تعاودية)
 *      4. ينسخ التعليمات مع إعادة تسمية السجلات لتجنب التعارض
 *      5. يستبدل RET بـ MOVE للنتيجة و BR للكتلة التالية
 *
 * (EN) Replaces calls to small functions with their body:
 *      1. Scan for CALL instructions
 *      2. Look up callee in module
 *      3. Check inlinability (size, non-recursive)
 *      4. Copy instructions with register renaming
 *      5. Replace RET with MOVE + BR to continuation
 *
 * @author SadLanguage Compiler Team
 * @date January 2026
 */

#include "sir_optimizer/inlining_pass.h"
#include <iostream>
#include <sstream>
#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int

namespace Sad
{
    namespace Compiler
    {
        namespace Optimizer
        {

            // ============================================================================
            // البناء / Constructor
            // ============================================================================

            InliningPass::InliningPass(int threshold)
                : OptimizationPass("Function Inlining", PassType::TRANSFORMATION), inlineThreshold_(threshold), inlinedCount_(0), uniqueCounter_(0), module_(nullptr)
            {
            }

            InliningPass::~InliningPass() = default;

            // ============================================================================
            // نقطة الدخول / Main Entry Point
            // ============================================================================

            bool InliningPass::runOnFunction(SIR::SIRFunction *function)
            {
                if (!function || !module_)
                    return false;

                inlinedCount_ = 0;
                bool modified = false;

                // (AR) فحص كل كتلة وكل تعليمة CALL
                for (size_t bi = 0; bi < function->basicBlocks.size(); bi++)
                {
                    auto *block = function->basicBlocks[bi].get();
                    if (!block)
                        continue;

                    for (size_t i = 0; i < block->instructions.size(); i++)
                    {
                        auto &inst = block->instructions[i];

                        // (AR) فقط CALL العادي (ليس CALL_INDIRECT أو OBJECT_CALL)
                        if (inst.opcode != SIR::SIROpcode::CALL)
                            continue;

                        // (AR) المعامل الأول هو اسم الدالة
                        if (inst.operands.empty())
                            continue;
                        const auto &funcNameOp = inst.operands[0];
                        if (funcNameOp.type != SIR::SIROperandType::FUNCTION &&
                            funcNameOp.type != SIR::SIROperandType::LABEL)
                            continue;

                        // (AR) ابحث عن الدالة المستدعاة
                        auto *callee = findFunction(funcNameOp.name);
                        if (!callee)
                            continue;

                        // (AR) هل يمكن إدراجها؟
                        if (!canInline(callee))
                            continue;

                        // (AR) نفّذ الإدراج
                        if (inlineCallSite(function, block, i, callee))
                        {
                            modified = true;
                            inlinedCount_++;
                            recordModification();
                            // (AR) بعد الإدراج، أعد فحص الكتلة من البداية (تغيرت التعليمات)
                            i = 0;
                        }
                    }
                }

                return modified;
            }

            // ============================================================================
            // (AR) التحقق من قابلية الإدراج
            // (EN) Check if function can be inlined
            // ============================================================================

            bool InliningPass::canInline(SIR::SIRFunction *callee) const
            {
                if (!callee)
                    return false;

                // (AR) فحص الحجم
                int instrCount = countInstructions(callee);
                if (instrCount > inlineThreshold_)
                    return false;
                if (instrCount == 0)
                    return false;

                // (AR) فحص التعاودية
                if (isRecursive(callee))
                    return false;

                return true;
            }

            int InliningPass::countInstructions(SIR::SIRFunction *function) const
            {
                int count = 0;
                for (const auto &block : function->basicBlocks)
                {
                    if (block)
                    {
                        count += Sad::Security::SafeArithmetic::assertSafeCast<int>(block->instructions.size(), "inlining_pass_size");
                    }
                }
                return count;
            }

            bool InliningPass::isRecursive(SIR::SIRFunction *function) const
            {
                const std::string &funcName = function->getName();
                for (const auto &block : function->basicBlocks)
                {
                    if (!block)
                        continue;
                    for (const auto &inst : block->instructions)
                    {
                        if (inst.opcode == SIR::SIROpcode::CALL &&
                            !inst.operands.empty() &&
                            inst.operands[0].name == funcName)
                        {
                            return true;
                        }
                    }
                }
                return false;
            }

            // ============================================================================
            // (AR) تنفيذ الإدراج لاستدعاء واحد
            // (EN) Inline a single call site
            // ============================================================================

            bool InliningPass::inlineCallSite(SIR::SIRFunction *caller,
                                              SIR::SIRBasicBlock *block,
                                              size_t callIndex,
                                              SIR::SIRFunction *callee)
            {
                if (callIndex >= block->instructions.size())
                    return false;

                auto &callInst = block->instructions[callIndex];

                // (AR) بناء جدول إعادة التسمية: معاملات الدالة → وسائط الاستدعاء
                std::unordered_map<std::string, std::string> nameMap;

                // (AR) ربط معاملات الدالة بوسائط الاستدعاء
                const auto &params = callee->parameters;
                // (AR) المعامل الأول في CALL هو اسم الدالة، الباقي وسائط
                for (size_t pi = 0; pi < params.size() && (pi + 1) < callInst.operands.size(); pi++)
                {
                    const auto &param = params[pi];
                    const auto &arg = callInst.operands[pi + 1]; // +1 لتجاوز اسم الدالة

                    if (arg.type == SIR::SIROperandType::REGISTER)
                    {
                        nameMap[param.name] = arg.name;
                    }
                    else
                    {
                        // (AR) إذا كان الوسيط ثابتاً، أنشئ سجل مؤقت واحفظ القيمة فيه
                        std::string tempName = makeUniqueRegName("_inline_arg");
                        // (AR) سنضيف تعليمة MOVE لاحقاً
                        nameMap[param.name] = tempName;
                    }
                }

                // (AR) إعادة تسمية جميع السجلات المحلية في المستدعاة
                for (const auto &bb : callee->basicBlocks)
                {
                    if (!bb)
                        continue;
                    for (const auto &inst : bb->instructions)
                    {
                        if (inst.hasResult() && inst.result)
                        {
                            const std::string &regName = inst.result->name;
                            if (nameMap.find(regName) == nameMap.end())
                            {
                                nameMap[regName] = makeUniqueRegName(regName);
                            }
                        }
                    }
                }

                // (AR) نسخ التعليمات من المستدعاة (باستثناء RET)
                std::vector<SIR::SIRInstruction> inlinedInsts;

                for (const auto &bb : callee->basicBlocks)
                {
                    if (!bb)
                        continue;
                    for (const auto &inst : bb->instructions)
                    {
                        if (inst.opcode == SIR::SIROpcode::RET)
                        {
                            // (AR) RET يُحوّل إلى MOVE للنتيجة
                            if (callInst.hasResult() && callInst.result && !inst.operands.empty())
                            {
                                SIR::SIRInstruction moveInst;
                                moveInst.opcode = SIR::SIROpcode::MOVE;
                                moveInst.result = callInst.result; // (AR) نتيجة CALL الأصلية
                                auto renamedOp = inst.operands[0];
                                if (renamedOp.type == SIR::SIROperandType::REGISTER)
                                {
                                    auto it = nameMap.find(renamedOp.name);
                                    if (it != nameMap.end())
                                    {
                                        renamedOp.name = it->second;
                                    }
                                }
                                moveInst.operands.push_back(renamedOp);
                                inlinedInsts.push_back(moveInst);
                            }
                            // (AR) لا ننسخ RET_VOID
                        }
                        else if (inst.opcode == SIR::SIROpcode::RET_VOID)
                        {
                            // (AR) لا شيء
                        }
                        else
                        {
                            // (AR) نسخ التعليمة مع إعادة التسمية
                            inlinedInsts.push_back(renameInstruction(inst, nameMap));
                        }
                    }
                }

                // (AR) استبدال CALL بالتعليمات المُدرجة
                block->instructions.erase(block->instructions.begin() + callIndex);
                block->instructions.insert(block->instructions.begin() + callIndex,
                                           inlinedInsts.begin(), inlinedInsts.end());

                return true;
            }

            // ============================================================================
            // (AR) دوال مساعدة / (EN) Helper functions
            // ============================================================================

            std::string InliningPass::makeUniqueRegName(const std::string &original)
            {
                std::ostringstream oss;
                oss << original << "_inl" << uniqueCounter_++;
                return oss.str();
            }

            SIR::SIRInstruction InliningPass::renameInstruction(
                const SIR::SIRInstruction &inst,
                const std::unordered_map<std::string, std::string> &nameMap)
            {
                SIR::SIRInstruction renamed = inst;

                // (AR) إعادة تسمية النتيجة
                if (renamed.result && renamed.result->type == SIR::SIROperandType::REGISTER)
                {
                    auto it = nameMap.find(renamed.result->name);
                    if (it != nameMap.end())
                    {
                        renamed.result->name = it->second;
                    }
                }

                // (AR) إعادة تسمية المعاملات
                for (auto &op : renamed.operands)
                {
                    if (op.type == SIR::SIROperandType::REGISTER)
                    {
                        auto it = nameMap.find(op.name);
                        if (it != nameMap.end())
                        {
                            op.name = it->second;
                        }
                    }
                }

                return renamed;
            }

            SIR::SIRFunction *InliningPass::findFunction(const std::string &name)
            {
                if (!module_)
                    return nullptr;

                for (auto &func : module_->getFunctions())
                {
                    if (func && func->getName() == name)
                    {
                        return func.get();
                    }
                }
                return nullptr;
            }

        } // namespace Optimizer
    } // namespace Compiler
} // namespace Sad
