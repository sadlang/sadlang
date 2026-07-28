// ============================================================================
// sir_frontend_optimizer_passes2.cpp — ممرات تحسين متقدمة
// (AR) تضمين الدوال، إزالة التعبيرات المتكررة، تبسيط الحلقات، المُحسّن الرئيسي
// (EN) Function inlining, CSE, loop simplification, main optimizer class
// تم استخراج هذا الملف من sir_frontend_optimizer.cpp وفقاً لقاعدة CW-05
// ============================================================================

#include "frontend/sir_frontend_optimizer.h"
#include <algorithm>
#include <sstream>
#include <cmath>
#include <cassert>
#include <iostream>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            // (AR) تصنيف العمليات — منسوخ من الملف الأصلي (static linkage)
            // (EN) Opcode classification helpers — copied from original file
            static bool isArithmeticOp(SIROpcode op)
            {
                switch (op)
                {
                case SIROpcode::ADD_I64:
                case SIROpcode::ADD_F64:
                case SIROpcode::SUB_I64:
                case SIROpcode::SUB_F64:
                case SIROpcode::MUL_I64:
                case SIROpcode::MUL_F64:
                case SIROpcode::DIV_I64:
                case SIROpcode::DIV_F64:
                case SIROpcode::FLOOR_DIV_I64:
                case SIROpcode::MOD_I64:
                case SIROpcode::NEG:
                    return true;
                default:
                    return false;
                }
            }
            static bool isComparisonOp(SIROpcode op)
            {
                switch (op)
                {
                case SIROpcode::EQ:
                case SIROpcode::NE:
                case SIROpcode::LT:
                case SIROpcode::LE:
                case SIROpcode::GT:
                case SIROpcode::GE:
                    return true;
                default:
                    return false;
                }
            }
            static bool isBitwiseOp(SIROpcode op)
            {
                switch (op)
                {
                case SIROpcode::AND:
                case SIROpcode::OR:
                case SIROpcode::XOR:
                case SIROpcode::NOT:
                case SIROpcode::SHL:
                case SIROpcode::SHR:
                case SIROpcode::SAR:
                case SIROpcode::ROL:
                    return true;
                default:
                    return false;
                }
            }

            bool FunctionInliningFrontendPass::shouldInline(const SIRFunction &callee) const
            {
                // ================================================================
                // (AR) [Fix #067] منع تضمين دوال الماكرو (__macro_*):
                //      دوال الماكرو تُنشأ كدوال منفصلة لضمان عزل النطاق (hygiene).
                //      التضمين يكسر العزل لأن المتغيرات المحلية في الماكرو قد تطابق
                //      أسماء متغيرات عامة — فيقوم المُحسّن بعدم إعادة تسميتها
                //      (يعتبرها متغيرات عامة) مما يؤدي لتلويث النطاق الخارجي.
                //      مثال: ماكرو يحتوي `متغير قيمة = "داخلي"` — إذا ضُمّن،
                //      يُكتب "داخلي" على المتغير العام @قيمة بدلاً من alloca محلي.
                // (EN) [Fix #067] Don't inline macro functions (__macro_*):
                //      Macro functions are created as separate functions to ensure scope
                //      isolation (hygiene). Inlining breaks isolation because local variables
                //      in the macro may match global variable names — the optimizer won't
                //      rename them (treats them as globals) causing outer scope pollution.
                //      Example: macro with `var value = "inner"` — if inlined, writes "inner"
                //      to global @value instead of local alloca.
                // ================================================================
                const std::string &name = callee.getName();
                if (name.size() > 8 && name.substr(0, 8) == "__macro_")
                    return false;

                // Don't inline recursive or large functions
                size_t totalInsts = 0;
                for (const auto &block : callee.getBasicBlocks())
                {
                    if (!block)
                        continue;
                    totalInsts += block->instructions.size();
                }
                if (totalInsts > maxInlineSize_)
                    return false;
                if (totalInsts == 0)
                    return false;

                // Don't inline functions with multiple blocks (complex control flow)
                if (callee.getBasicBlocks().size() > 2)
                    return false;

                return true;
            }

            bool FunctionInliningFrontendPass::inlineCallSite(
                SIRFunction &caller, SIRBasicBlock &block,
                size_t instIdx, const SIRFunction &callee,
                size_t &nextReg)
            {

                const auto &callInst = block.instructions[instIdx];

                // Get callee's instructions from entry block
                if (callee.getBasicBlocks().empty())
                    return false;
                const auto &calleeEntry = callee.getBasicBlocks()[0];
                if (!calleeEntry || calleeEntry->instructions.empty())
                    return false;

                // Build parameter mapping: callee param name → caller arg operand
                // (AR) ربط أسماء المعاملات مع وسائط المستدعي — نسجل بالصيغتين: "عدد" و "%عدد"
                // (EN) Map param names to caller args — register both "عدد" and "%عدد" forms
                std::unordered_map<std::string, SIROperand> paramMap;
                const auto &params = callee.getParameters();
                // operands[0] is function name, rest are args
                for (size_t i = 0; i < params.size() && i + 1 < callInst.operands.size(); ++i)
                {
                    paramMap[params[i].name] = callInst.operands[i + 1];
                    // (AR) أيضاً سجّل بصيغة %اسم لأن التعليمات تستخدم هذا الشكل
                    // (EN) Also register with % prefix since instructions use this form
                    paramMap["%" + params[i].name] = callInst.operands[i + 1];
                }

                // Generate unique register prefix for inlined code
                std::string prefix = "_inl" + std::to_string(nextReg++) + "_";

                // (AR) دالة مساعدة لإعادة تسمية السجل مع الحفاظ على % في البداية
                // (EN) Helper to rename register while preserving % prefix
                auto renameReg = [&prefix](const std::string &name) -> std::string
                {
                    if (!name.empty() && name[0] == '%')
                    {
                        return "%" + prefix + name.substr(1);
                    }
                    return prefix + name;
                };

                // Build inlined instructions
                std::vector<SIRInstruction> inlinedInsts;
                for (const auto &calInst : calleeEntry->instructions)
                {
                    if (calInst.opcode == SIROpcode::RET || calInst.opcode == SIROpcode::RET_VOID)
                    {
                        // Replace return with a move to the call result
                        if (calInst.opcode == SIROpcode::RET && !calInst.operands.empty() &&
                            callInst.hasResult())
                        {
                            SIRInstruction moveInst(SIROpcode::MOVE);
                            moveInst.result = callInst.result;
                            auto retOp = calInst.operands[0];
                            // Remap operand
                            if (retOp.type == SIROperandType::REGISTER)
                            {
                                auto pit = paramMap.find(retOp.name);
                                if (pit != paramMap.end())
                                {
                                    retOp = pit->second;
                                }
                                else if (globalVarNames_.find(retOp.name) == globalVarNames_.end())
                                {
                                    retOp.name = renameReg(retOp.name);
                                }
                            }
                            moveInst.operands = {retOp};
                            inlinedInsts.push_back(moveInst);
                        }
                        continue;
                    }

                    SIRInstruction newInst = calInst;
                    // Remap result register
                    if (newInst.hasResult() && newInst.result->type == SIROperandType::REGISTER)
                    {
                        auto pit = paramMap.find(newInst.result->name);
                        if (pit == paramMap.end())
                        {
                            // (AR) لا تعد تسمية المتغيرات العالمية — تبقى بأسمائها الأصلية
                            // (EN) Don't rename global variables — keep original names
                            if (globalVarNames_.find(newInst.result->name) == globalVarNames_.end())
                            {
                                newInst.result->name = renameReg(newInst.result->name);
                            }
                        }
                    }
                    // Remap operand registers
                    for (auto &op : newInst.operands)
                    {
                        if (op.type == SIROperandType::REGISTER)
                        {
                            auto pit = paramMap.find(op.name);
                            if (pit != paramMap.end())
                            {
                                op = pit->second;
                            }
                            else
                            {
                                // (AR) لا تعد تسمية المتغيرات العالمية — تبقى بأسمائها الأصلية
                                // (EN) Don't rename global variables — keep original names
                                if (globalVarNames_.find(op.name) == globalVarNames_.end())
                                {
                                    op.name = renameReg(op.name);
                                }
                            }
                        }
                    }
                    inlinedInsts.push_back(newInst);
                }

                // Replace call instruction with inlined instructions
                block.instructions.erase(block.instructions.begin() + static_cast<ptrdiff_t>(instIdx));
                block.instructions.insert(
                    block.instructions.begin() + static_cast<ptrdiff_t>(instIdx),
                    inlinedInsts.begin(), inlinedInsts.end());

                return true;
            }

            bool FunctionInliningFrontendPass::runOnModule(SIRModule &module)
            {
                bool changed = false;
                size_t nextReg = 0;

                // (AR) بناء مجموعة أسماء المتغيرات العالمية لعدم إعادة تسميتها عند التضمين
                // (EN) Build global variable name set to avoid renaming during inlining
                globalVarNames_.clear();
                for (const auto &gv : module.getGlobalVariables())
                {
                    if (!gv)
                        continue;
                    std::string name = gv->getName();
                    globalVarNames_.insert(name);
                    globalVarNames_.insert("%" + name);
                }

                // Build map of inlineable functions
                std::unordered_map<std::string, const SIRFunction *> inlineableFuncs;
                for (const auto &func : module.getFunctions())
                {
                    if (!func)
                        continue;
                    if (shouldInline(*func))
                    {
                        inlineableFuncs[func->getName()] = func.get();
                    }
                }

                // Process each function
                for (auto &func : module.getFunctions())
                {
                    if (!func)
                        continue;
                    for (auto &block : func->basicBlocks)
                    {
                        if (!block)
                            continue;
                        for (size_t i = 0; i < block->instructions.size(); ++i)
                        {
                            auto &inst = block->instructions[i];
                            if (inst.opcode != SIROpcode::CALL)
                                continue;
                            if (inst.operands.empty())
                                continue;

                            const auto &funcOp = inst.operands[0];
                            std::string calleeName = funcOp.name;
                            // Remove @ prefix if present
                            if (!calleeName.empty() && calleeName[0] == '@')
                            {
                                calleeName = calleeName.substr(1);
                            }

                            auto it = inlineableFuncs.find(calleeName);
                            if (it == inlineableFuncs.end())
                                continue;

                            // Don't inline recursive calls
                            if (calleeName == func->getName())
                                continue;

                            if (inlineCallSite(*func, *block, i, *it->second, nextReg))
                            {
                                changed = true;
                                // Re-process this position since we inserted new instructions
                                --i;
                            }
                        }
                    }
                }
                return changed;
            }

            // ======================================================================
            // ممر 4: إزالة التعبيرات المتكررة / Pass 4: CSE
            // ======================================================================

            bool CommonSubexpressionEliminationPass::ExprKey::operator==(const ExprKey &other) const
            {
                return opcode == other.opcode && operandNames == other.operandNames;
            }

            size_t CommonSubexpressionEliminationPass::ExprKeyHash::operator()(const ExprKey &key) const
            {
                size_t h = std::hash<int>{}(static_cast<int>(key.opcode));
                for (const auto &name : key.operandNames)
                {
                    h ^= std::hash<std::string>{}(name) + 0x9e3779b9 + (h << 6) + (h >> 2);
                }
                return h;
            }

            bool CommonSubexpressionEliminationPass::isPureOperation(SIROpcode opcode) const
            {
                return isArithmeticOp(opcode) || isComparisonOp(opcode) || isBitwiseOp(opcode) ||
                       opcode == SIROpcode::I64_TO_F64 || opcode == SIROpcode::F64_TO_I64 ||
                       opcode == SIROpcode::I64_TO_BOOL || opcode == SIROpcode::BOOL_TO_I64;
            }

            bool CommonSubexpressionEliminationPass::runOnFunction(SIRFunction &func)
            {
                bool changed = false;

                for (auto &block : func.basicBlocks)
                {
                    if (!block)
                        continue;

                    std::unordered_map<ExprKey, std::string, ExprKeyHash> exprMap;
                    // Maps old register name → replacement register name
                    std::unordered_map<std::string, std::string> replacements;

                    auto &instructions = block->instructions;
                    for (size_t i = 0; i < instructions.size(); ++i)
                    {
                        auto &inst = instructions[i];

                        // Apply existing replacements to operands
                        for (auto &op : inst.operands)
                        {
                            if (op.type == SIROperandType::REGISTER)
                            {
                                auto rit = replacements.find(op.name);
                                if (rit != replacements.end())
                                {
                                    op.name = rit->second;
                                }
                            }
                        }

                        // ================================================================
                        // (AR) [إصلاح CSE] عند STORE: أبطل كل التعبيرات التي تعتمد على
                        //      الـ destination register — لأن قيمة المتغير تغيّرت.
                        //      بدون هذا: x += 1 ; print(x) ; x += 1 يُعطي نتيجة خاطئة
                        //      لأن CSE يعيد استخدام نتيجة "add x, 1" الأولى للثانية.
                        // (EN) [CSE Fix] On STORE: invalidate all cached expressions that
                        //      depend on the destination register — its value has changed.
                        //      Without this: x += 1 ; print(x) ; x += 1 gives wrong result
                        //      because CSE reuses the first "add x, 1" for the second.
                        // ================================================================
                        if (inst.opcode == SIROpcode::STORE && inst.operands.size() >= 2)
                        {
                            const auto &dest = inst.operands[1];
                            if (dest.type == SIROperandType::REGISTER)
                            {
                                // أزل كل الـ entries التي تحتوي على dest register في معاملاتها
                                for (auto it = exprMap.begin(); it != exprMap.end();)
                                {
                                    bool dependsOnDest = false;
                                    for (const auto &opName : it->first.operandNames)
                                    {
                                        if (opName == dest.name)
                                        {
                                            dependsOnDest = true;
                                            break;
                                        }
                                    }
                                    if (dependsOnDest)
                                    {
                                        it = exprMap.erase(it);
                                    }
                                    else
                                    {
                                        ++it;
                                    }
                                }
                            }
                        }

                        // Only CSE pure operations with results
                        if (!inst.hasResult() || !isPureOperation(inst.opcode))
                            continue;

                        ExprKey key;
                        key.opcode = inst.opcode;
                        for (const auto &op : inst.operands)
                        {
                            if (op.type == SIROperandType::REGISTER)
                            {
                                key.operandNames.push_back(op.name);
                            }
                            else if (op.type == SIROperandType::CONSTANT)
                            {
                                // Encode constant as string for hashing
                                // (AR) [إصلاح عيب كامن كشفته طبقة طبيعي64 — نظير cse_pass.cpp]
                                //      كلّ الثوابت الصحيحة (بأيّ عرض/إشارة) تُرمَّز بقيمتها.
                                //      UInt64/بايت/الأنواع المحدَّدة كانت تُهمَل تمامًا (لا فرع)
                                //      ⇒ مفتاحٌ بلا تمييز ⇒ CSE الأماميّ (عند O3) يدمج `ك//2`
                                //      و`ك//1` (نفس السجلّ، ثابتان مُهمَلان). ضمُّها يصلح الجذر.
                                // (EN) [Latent-defect fix surfaced by the طبيعي64 layer — sibling
                                //      of cse_pass.cpp] All integer constants (any width/signedness)
                                //      key by value. UInt64/Byte/sized types were skipped entirely
                                //      (no branch) ⇒ an undistinguished key ⇒ the frontend CSE (at
                                //      O3) merged `ك//2` and `ك//1` (same register, both constants dropped).
                                if (op.dataType == SadTypeKind::Integer ||
                                    op.dataType == SadTypeKind::UInt64 ||
                                    op.dataType == SadTypeKind::Byte ||
                                    op.dataType == SadTypeKind::Int8 ||
                                    op.dataType == SadTypeKind::Int16 ||
                                    op.dataType == SadTypeKind::Int32 ||
                                    op.dataType == SadTypeKind::Int64 ||
                                    op.dataType == SadTypeKind::UInt8 ||
                                    op.dataType == SadTypeKind::UInt16 ||
                                    op.dataType == SadTypeKind::UInt32)
                                {
                                    key.operandNames.push_back("$i" + std::to_string(op.intValue));
                                }
                                else if (op.dataType == SadTypeKind::Float)
                                {
                                    key.operandNames.push_back("$f" + std::to_string(op.floatValue));
                                }
                                else if (op.dataType == SadTypeKind::Boolean)
                                {
                                    key.operandNames.push_back(op.boolValue ? "$true" : "$false");
                                }
                            }
                        }

                        auto it = exprMap.find(key);
                        if (it != exprMap.end())
                        {
                            // This expression was computed before — replace with existing
                            replacements[inst.result->name] = it->second;
                            changed = true;
                        }
                        else
                        {
                            exprMap[key] = inst.result->name;
                        }
                    }

                    // Remove instructions whose results have been replaced
                    if (!replacements.empty())
                    {
                        auto newEnd = std::remove_if(instructions.begin(), instructions.end(),
                                                     [&](const SIRInstruction &inst)
                                                     {
                                                         return inst.hasResult() &&
                                                                replacements.count(inst.result->name) > 0;
                                                     });
                        instructions.erase(newEnd, instructions.end());
                    }
                }
                return changed;
            }

            // ======================================================================
            // ممر 5: تبسيط الحلقات / Pass 5: Loop Simplification
            // ======================================================================

            bool LoopSimplificationFrontendPass::simplifyBranches(SIRFunction &func)
            {
                bool changed = false;

                for (auto &block : func.basicBlocks)
                {
                    if (!block)
                        continue;
                    auto &instructions = block->instructions;
                    if (instructions.empty())
                        continue;

                    auto &lastInst = instructions.back();

                    // Simplify conditional branches with constant condition
                    if (lastInst.opcode == SIROpcode::BR_COND && lastInst.operands.size() == 3)
                    {
                        const auto &cond = lastInst.operands[0];
                        if (cond.type == SIROperandType::CONSTANT && cond.dataType == SadTypeKind::Boolean)
                        {
                            // Replace with unconditional branch
                            SIROperand target = cond.boolValue
                                                    ? lastInst.operands[1]  // then label
                                                    : lastInst.operands[2]; // else label
                            lastInst.opcode = SIROpcode::BR;
                            lastInst.operands = {target};
                            changed = true;
                        }
                    }
                }
                return changed;
            }

            bool LoopSimplificationFrontendPass::removeEmptyBlocks(SIRFunction &func)
            {
                bool changed = false;
                auto &blocks = func.basicBlocks;

                // (AR) اجمع أسماء الكتل المشار إليها كسلفٍ في عُقَد PHI. حذفُ كتلةٍ فارغة
                //      تُوجِّه (forwarding) وهي سلفُ PHI يكسر العقدة: يُعاد توجيه فرعها إلى
                //      الوجهة لكنّ وسم الوارد في PHI يبقى على الاسم المحذوف ⇒ «PHINode should
                //      have one entry for each predecessor». مثاله ‏«أ ؟؟ ب» بمتغيّرات:
                //      nc_right فارغة (مرجع المتغيّر لا يُصدر تعليمة) فتُحذف ويبطل الـPHI.
                //      نستثنيها من الطيّ صونًا للعقدة (كتلة فارغة بفرعٍ صالحة في LLVM).
                // (EN) Collect block names referenced as predecessors by PHI nodes. Removing
                //      an empty forwarding block that is a PHI predecessor breaks the node: its
                //      branch is redirected to the target but the PHI's incoming label still
                //      names the deleted block ⇒ «PHINode should have one entry for each
                //      predecessor». E.g. «أ ؟؟ ب» with variables: nc_right is empty (a variable
                //      ref emits no instruction) so it gets deleted and the PHI is invalidated.
                //      Exclude such blocks from folding to preserve the node (an empty block
                //      with a branch is valid LLVM).
                std::unordered_set<std::string> phiPredecessors;
                for (const auto &block : blocks)
                {
                    if (!block)
                        continue;
                    for (const auto &inst : block->instructions)
                    {
                        if (inst.opcode != SIROpcode::PHI)
                            continue;
                        for (const auto &op : inst.operands)
                        {
                            if (op.type == SIROperandType::LABEL)
                                phiPredecessors.insert(op.name);
                        }
                    }
                }

                // Find empty blocks (only have an unconditional branch)
                std::unordered_map<std::string, std::string> redirects;
                for (const auto &block : blocks)
                {
                    if (!block)
                        continue;
                    // (AR) لا تطوِ كتلةً هي سلفٌ لعقدة PHI (يكسر الوسم أعلاه).
                    // (EN) Never fold a block that is a PHI predecessor (breaks the label above).
                    if (phiPredecessors.count(block->name) > 0)
                        continue;
                    const auto &insts = block->instructions;
                    if (insts.size() == 1 && insts[0].opcode == SIROpcode::BR &&
                        !insts[0].operands.empty())
                    {
                        redirects[block->name] = insts[0].operands[0].name;
                    }
                }

                if (redirects.empty())
                    return false;

                // Follow redirect chains
                for (auto &[from, to] : redirects)
                {
                    std::unordered_set<std::string> visited;
                    visited.insert(from);
                    std::string current = to;
                    while (redirects.count(current) && !visited.count(current))
                    {
                        visited.insert(current);
                        current = redirects[current];
                    }
                    to = current;
                }

                // Update branch targets in all blocks
                for (auto &block : blocks)
                {
                    if (!block)
                        continue;
                    for (auto &inst : block->instructions)
                    {
                        if (inst.opcode == SIROpcode::BR || inst.opcode == SIROpcode::BR_COND)
                        {
                            for (auto &op : inst.operands)
                            {
                                if (op.type == SIROperandType::LABEL)
                                {
                                    auto it = redirects.find(op.name);
                                    if (it != redirects.end())
                                    {
                                        op.name = it->second;
                                        changed = true;
                                    }
                                }
                            }
                        }
                    }
                }

                // Remove the empty blocks (don't remove entry block)
                if (changed && blocks.size() > 1)
                {
                    blocks.erase(
                        std::remove_if(blocks.begin() + 1, blocks.end(),
                                       [&](const std::shared_ptr<SIRBasicBlock> &b)
                                       {
                                           return b && redirects.count(b->name) > 0;
                                       }),
                        blocks.end());
                }

                return changed;
            }

            bool LoopSimplificationFrontendPass::mergeLinearBlocks(SIRFunction &func)
            {
                bool changed = false;
                auto &blocks = func.basicBlocks;

                for (size_t i = 0; i + 1 < blocks.size(); ++i)
                {
                    auto &block = blocks[i];
                    auto &nextBlock = blocks[i + 1];
                    if (!block || !nextBlock)
                        continue;

                    auto &insts = block->instructions;
                    if (insts.empty())
                        continue;

                    auto &lastInst = insts.back();

                    // If this block ends with unconditional branch to the next block
                    if (lastInst.opcode == SIROpcode::BR &&
                        !lastInst.operands.empty() &&
                        lastInst.operands[0].type == SIROperandType::LABEL &&
                        lastInst.operands[0].name == nextBlock->name)
                    {

                        // Check no other block branches to nextBlock
                        bool hasOtherPreds = false;
                        for (size_t j = 0; j < blocks.size(); ++j)
                        {
                            if (j == i || !blocks[j])
                                continue;
                            for (const auto &inst : blocks[j]->instructions)
                            {
                                for (const auto &op : inst.operands)
                                {
                                    if (op.type == SIROperandType::LABEL &&
                                        op.name == nextBlock->name)
                                    {
                                        hasOtherPreds = true;
                                        break;
                                    }
                                }
                                if (hasOtherPreds)
                                    break;
                            }
                            if (hasOtherPreds)
                                break;
                        }

                        if (!hasOtherPreds)
                        {
                            // Merge: remove branch, append next block's instructions
                            insts.pop_back(); // remove BR
                            insts.insert(insts.end(),
                                         nextBlock->instructions.begin(),
                                         nextBlock->instructions.end());
                            blocks.erase(blocks.begin() + static_cast<ptrdiff_t>(i + 1));
                            changed = true;
                            --i; // re-check this block
                        }
                    }
                }
                return changed;
            }

            bool LoopSimplificationFrontendPass::runOnFunction(SIRFunction &func)
            {
                bool changed = false;
                changed |= simplifyBranches(func);
                changed |= removeEmptyBlocks(func);
                changed |= mergeLinearBlocks(func);
                return changed;
            }

            // ======================================================================
            // المحسّن الرئيسي / Main Optimizer
            // ======================================================================

            SIRFrontendOptimizer::SIRFrontendOptimizer()
            {
                addPass(std::make_unique<ConstantFoldingFrontendPass>());
                addPass(std::make_unique<DeadCodeEliminationFrontendPass>());
                addPass(std::make_unique<FunctionInliningFrontendPass>());
                addPass(std::make_unique<CommonSubexpressionEliminationPass>());
                addPass(std::make_unique<LoopSimplificationFrontendPass>());
            }

            void SIRFrontendOptimizer::addPass(std::unique_ptr<SIRFrontendPass> pass)
            {
                if (pass)
                {
                    enabledPasses_[pass->getName()] = true;
                    passes_.push_back(std::move(pass));
                }
            }

            void SIRFrontendOptimizer::enablePass(const std::string &name, bool enabled)
            {
                enabledPasses_[name] = enabled;
            }

            bool SIRFrontendOptimizer::optimizeFunction(SIRFunction &func)
            {
                bool anyChanged = false;

                for (size_t iter = 0; iter < maxIterations_; ++iter)
                {
                    bool iterChanged = false;
                    for (auto &pass : passes_)
                    {
                        auto it = enabledPasses_.find(pass->getName());
                        if (it == enabledPasses_.end() || !it->second)
                            continue;

                        bool passChanged = pass->runOnFunction(func);

                        if (passChanged)
                        {
                            iterChanged = true;
                            stats_.totalPassesRun++;

                            // Update specific counters
                            const auto &name = pass->getName();
                            if (name == "ConstantFolding")
                                stats_.constantsFolded++;
                            else if (name == "DeadCodeElimination")
                                stats_.deadInstructionsRemoved++;
                            else if (name == "FunctionInlining")
                                stats_.functionsInlined++;
                            else if (name == "CSE")
                                stats_.commonSubexpressionsEliminated++;
                            else if (name == "LoopSimplification")
                                stats_.loopsSimplified++;
                        }
                    }
                    stats_.totalIterations++;
                    if (!iterChanged)
                        break;
                    anyChanged = true;
                }

                return anyChanged;
            }

            bool SIRFrontendOptimizer::optimizeModule(SIRModule &module)
            {
                bool changed = false;

                // First run module-level passes (inlining)
                for (auto &pass : passes_)
                {
                    auto it = enabledPasses_.find(pass->getName());
                    if (it == enabledPasses_.end() || !it->second)
                        continue;
                    if (pass->getName() == "FunctionInlining")
                    {
                        if (pass->runOnModule(module))
                        {
                            changed = true;
                            stats_.functionsInlined++;
                            stats_.totalPassesRun++;
                        }
                    }
                }

                // Then run per-function passes iteratively
                for (auto &func : module.getFunctions())
                {
                    if (func)
                    {
                        changed |= optimizeFunction(*func);
                    }
                }

                return changed;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
