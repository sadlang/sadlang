/**
 * @file licm_pass.cpp
 * @brief تنفيذ تمرير نقل الكود الثابت خارج الحلقة (LICM)
 * @brief Loop Invariant Code Motion Pass Implementation
 * 
 * @details
 * (AR) يكتشف الحلقات الطبيعية في CFG ثم ينقل التعليمات الثابتة إلى خارج الحلقة.
 * (EN) Detects natural loops in CFG then hoists invariant instructions out.
 * 
 * @author SadLanguage Compiler Team
 * @date January 2026
 */

#include "middle/licm_pass.h"
#include <algorithm>
#include <iostream>
#include <stack>

namespace Sad {
namespace Compiler {
namespace Optimizer {

// ============================================================================
// Constructor / البناء
// ============================================================================

LICMPass::LICMPass()
    : OptimizationPass("LICM (Loop Invariant Code Motion)", PassType::TRANSFORMATION)
    , hoistedCount_(0)
    , loopsFound_(0)
{
}

LICMPass::~LICMPass() = default;

// ============================================================================
// Main Entry Point
// ============================================================================

bool LICMPass::runOnFunction(SIR::SIRFunction* function) {
    if (!function) return false;
    
    hoistedCount_ = 0;
    loopsFound_ = 0;
    bool modified = false;
    
    // (AR) اكتشاف جميع الحلقات في الدالة
    // (EN) Detect all loops in function
    auto loops = detectLoops(function);
    loopsFound_ = static_cast<int>(loops.size());
    
    // (AR) لكل حلقة، نقل التعليمات الثابتة خارجها
    // (EN) For each loop, hoist invariant instructions out
    for (auto& loop : loops) {
        // (AR) جمع تعريفات الحلقة
        auto loopDefs = collectLoopDefs(loop);
        
        // (AR) التحقق من وجود كتلة قبل الحلقة
        if (!loop.preheader) {
            loop.preheader = findOrCreatePreheader(loop, function);
        }
        
        if (!loop.preheader) continue;
        
        // (AR) فحص التعليمات في جسم الحلقة
        // (EN) Examine instructions in loop body
        bool changed = true;
        while (changed) {
            changed = false;
            for (auto* block : loop.body) {
                if (!block) continue;
                
                auto it = block->instructions.begin();
                while (it != block->instructions.end()) {
                    // (AR) تخطي terminators
                    if (it->isTerminatorInst()) {
                        ++it;
                        continue;
                    }
                    
                    // (AR) فحص إذا كانت ثابتة بالنسبة للحلقة
                    if (isLoopInvariant(*it, loop, loopDefs) && isSafeToHoist(*it)) {
                        // (AR) نقل التعليمة إلى ما قبل الحلقة
                        loop.preheader->instructions.push_back(*it);
                        it = block->instructions.erase(it);
                        hoistedCount_++;
                        recordModification();
                        changed = true;
                        modified = true;
                    } else {
                        ++it;
                    }
                }
            }
        }
    }
    
    return modified;
}

// ============================================================================
// Loop Detection / اكتشاف الحلقات
// ============================================================================

std::vector<LoopInfo> LICMPass::detectLoops(SIR::SIRFunction* function) {
    std::vector<LoopInfo> loops;
    
    if (function->basicBlocks.empty()) return loops;
    
    // (AR) البحث عن حافات الرجوع (back edges)
    // (EN) Find back edges using DFS
    std::unordered_set<SIR::SIRBasicBlock*> visited;
    std::unordered_set<SIR::SIRBasicBlock*> onStack;
    std::vector<std::pair<SIR::SIRBasicBlock*, SIR::SIRBasicBlock*>> backEdges;
    
    findBackEdges(function->basicBlocks[0].get(), visited, onStack, backEdges);
    
    // (AR) بناء حلقة لكل حافة رجوع
    // (EN) Build loop for each back edge
    for (const auto& [latch, header] : backEdges) {
        LoopInfo loop = buildLoopFromBackEdge(latch, header, function);
        if (!loop.body.empty()) {
            loops.push_back(std::move(loop));
        }
    }
    
    return loops;
}

void LICMPass::findBackEdges(
    SIR::SIRBasicBlock* block,
    std::unordered_set<SIR::SIRBasicBlock*>& visited,
    std::unordered_set<SIR::SIRBasicBlock*>& onStack,
    std::vector<std::pair<SIR::SIRBasicBlock*, SIR::SIRBasicBlock*>>& backEdges)
{
    if (!block) return;
    
    visited.insert(block);
    onStack.insert(block);
    
    for (auto* succ : block->successors) {
        if (!succ) continue;
        
        if (onStack.count(succ)) {
            // (AR) وجدنا حافة رجوع: block → succ
            // (EN) Found back edge: block → succ
            backEdges.push_back({block, succ});
        } else if (!visited.count(succ)) {
            findBackEdges(succ, visited, onStack, backEdges);
        }
    }
    
    onStack.erase(block);
}

LoopInfo LICMPass::buildLoopFromBackEdge(
    SIR::SIRBasicBlock* latch,
    SIR::SIRBasicBlock* header,
    SIR::SIRFunction* function)
{
    LoopInfo loop;
    loop.header = header;
    loop.latch = latch;
    loop.preheader = nullptr;
    
    // (AR) بناء جسم الحلقة بالعمل للخلف من latch إلى header
    // (EN) Build loop body by working backwards from latch to header
    std::unordered_set<SIR::SIRBasicBlock*> bodySet;
    std::stack<SIR::SIRBasicBlock*> workList;
    
    bodySet.insert(header);
    
    if (latch != header) {
        bodySet.insert(latch);
        workList.push(latch);
    }
    
    while (!workList.empty()) {
        auto* block = workList.top();
        workList.pop();
        
        for (auto* pred : block->predecessors) {
            if (!pred || bodySet.count(pred)) continue;
            bodySet.insert(pred);
            workList.push(pred);
        }
    }
    
    loop.body.assign(bodySet.begin(), bodySet.end());
    
    // (AR) البحث عن كتلة ما قبل الحلقة (predecessor of header not in loop)
    // (EN) Find preheader (predecessor of header not in loop)
    for (auto* pred : header->predecessors) {
        if (pred && !bodySet.count(pred)) {
            loop.preheader = pred;
            break;
        }
    }
    
    // (AR) البحث عن المخارج (successors of loop blocks that are outside loop)
    // (EN) Find exits (successors of body blocks that are outside loop)
    for (auto* block : loop.body) {
        for (auto* succ : block->successors) {
            if (succ && !bodySet.count(succ)) {
                loop.exits.push_back(succ);
            }
        }
    }
    
    return loop;
}

// ============================================================================
// Invariance Analysis / تحليل الثبات
// ============================================================================

bool LICMPass::isLoopInvariant(
    const SIR::SIRInstruction& inst,
    const LoopInfo& loop,
    const std::unordered_set<std::string>& loopDefs)
{
    // (AR) التعليمات بدون نتيجة ليست مرشحة (مثل store, call)
    // (EN) Instructions without result aren't candidates (store, call, etc.)
    if (!inst.hasResult()) return false;
    
    // (AR) العمليات الجانبية (side effects) غير ثابتة
    // (EN) Side-effectful operations are not invariant
    switch (inst.opcode) {
        case SIR::SIROpcode::CALL:
        case SIR::SIROpcode::CALL_INDIRECT:
        case SIR::SIROpcode::STORE:
        case SIR::SIROpcode::LOAD:
        case SIR::SIROpcode::ALLOC:
        case SIR::SIROpcode::ALLOC_HEAP:
        case SIR::SIROpcode::FREE:
        case SIR::SIROpcode::MEMCPY:
        case SIR::SIROpcode::MEMSET:
            return false;
        default:
            break;
    }
    
    // (AR) تحقق أن جميع المعاملات ليست معرّفة داخل الحلقة
    // (EN) Check that all operands are not defined inside the loop
    for (const auto& op : inst.operands) {
        if (op.type == SIR::SIROperandType::REGISTER) {
            if (loopDefs.count(op.name)) {
                // (AR) هذا المعامل يُعرّف داخل الحلقة
                return false;
            }
        }
    }
    
    return true;
}

std::unordered_set<std::string> LICMPass::collectLoopDefs(const LoopInfo& loop) {
    std::unordered_set<std::string> defs;
    
    for (auto* block : loop.body) {
        if (!block) continue;
        for (const auto& inst : block->instructions) {
            if (inst.hasResult() && inst.result->type == SIR::SIROperandType::REGISTER) {
                defs.insert(inst.result->name);
            }
        }
    }
    
    return defs;
}

// ============================================================================
// Safety & Hoisting / الأمان والنقل
// ============================================================================

bool LICMPass::isSafeToHoist(const SIR::SIRInstruction& inst) const {
    // (AR) العمليات الحسابية البسيطة والمقارنات وتحويل الأنواع آمنة
    // (EN) Simple arithmetic, comparisons, and type conversions are safe
    switch (inst.opcode) {
        // Arithmetic
        case SIR::SIROpcode::ADD_I64:
        case SIR::SIROpcode::ADD_F64:
        case SIR::SIROpcode::SUB_I64:
        case SIR::SIROpcode::SUB_F64:
        case SIR::SIROpcode::MUL_I64:
        case SIR::SIROpcode::MUL_F64:
        case SIR::SIROpcode::DIV_I64:
        case SIR::SIROpcode::DIV_F64:
        case SIR::SIROpcode::MOD_I64:
        case SIR::SIROpcode::NEG:
        // Bitwise
        case SIR::SIROpcode::AND:
        case SIR::SIROpcode::OR:
        case SIR::SIROpcode::XOR:
        case SIR::SIROpcode::NOT:
        case SIR::SIROpcode::SHL:
        case SIR::SIROpcode::SHR:
        case SIR::SIROpcode::SAR:
        case SIR::SIROpcode::ROL:
        // Comparisons
        case SIR::SIROpcode::EQ:
        case SIR::SIROpcode::NE:
        case SIR::SIROpcode::LT:
        case SIR::SIROpcode::LE:
        case SIR::SIROpcode::GT:
        case SIR::SIROpcode::GE:
        // Type conversions
        case SIR::SIROpcode::I64_TO_F64:
        case SIR::SIROpcode::F64_TO_I64:
        case SIR::SIROpcode::I64_TO_BOOL:
        case SIR::SIROpcode::BOOL_TO_I64:
        case SIR::SIROpcode::CAST:
        // Move
        case SIR::SIROpcode::MOVE:
            return true;
            
        default:
            return false;
    }
}

bool LICMPass::hoistInstruction(SIR::SIRInstruction& inst, LoopInfo& loop) {
    if (!loop.preheader) return false;
    
    // (AR) إضافة التعليمة قبل terminator في preheader
    // (EN) Add instruction before terminator in preheader
    auto& preInsts = loop.preheader->instructions;
    
    if (!preInsts.empty() && preInsts.back().isTerminatorInst()) {
        preInsts.insert(preInsts.end() - 1, inst);
    } else {
        preInsts.push_back(inst);
    }
    
    return true;
}

SIR::SIRBasicBlock* LICMPass::findOrCreatePreheader(
    LoopInfo& loop, SIR::SIRFunction* function)
{
    // (AR) البحث عن predecessor خارج الحلقة
    // (EN) Find predecessor outside loop
    std::unordered_set<SIR::SIRBasicBlock*> bodySet(loop.body.begin(), loop.body.end());
    
    for (auto* pred : loop.header->predecessors) {
        if (pred && !bodySet.count(pred)) {
            return pred;
        }
    }
    
    // (AR) إنشاء كتلة جديدة كـ preheader
    // (EN) Create new block as preheader
    auto preheader = std::make_shared<SIR::SIRBasicBlock>("preheader_" + loop.header->name);
    preheader->parent = function;
    
    // (AR) إضافة قفز إلى رأس الحلقة
    preheader->addInstruction(SIR::SIRInstruction::Branch(
        SIR::SIROperand::Label(loop.header->name)));
    
    // (AR) ربط CFG
    preheader->successors.push_back(loop.header);
    loop.header->predecessors.push_back(preheader.get());
    
    // (AR) إضافة الكتلة إلى الدالة
    function->basicBlocks.push_back(preheader);
    
    return preheader.get();
}

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
