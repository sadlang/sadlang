/**
 * @file dead_code_elimination_pass.cpp
 * @brief تطبيق تمرير حذف الكود الميت
 * @brief Dead Code Elimination Pass Implementation
 * 
 * @details
 * (AR) يزيل التعليمات التي لا تؤثر على نتيجة البرنامج.
 * (EN) Removes instructions that do not affect program output.
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "middle/dead_code_elimination_pass.h"
#include <queue>
#include <algorithm>
#include <unordered_map>

namespace Sad {
namespace Compiler {
namespace Optimizer {

/**
 * @brief Constructor / البناء
 */
DeadCodeEliminationPass::DeadCodeEliminationPass()
    : OptimizationPass("Dead Code Elimination", PassType::TRANSFORMATION)
{
}

/**
 * @brief تشغيل التمرير على دالة
 * @brief Run pass on function
 */
bool DeadCodeEliminationPass::runOnFunction(SIR::SIRFunction* function) {
    if (!function) {
        return false;
    }
    
    bool modified = false;
    
    // المرحلة 1: حذف الكتل غير القابلة للوصول / Phase 1: Remove unreachable blocks
    int unreachableRemoved = removeUnreachableBlocks(function);
    if (unreachableRemoved > 0) {
        modified = true;
        recordModification();
        debug("Removed " + std::to_string(unreachableRemoved) + " unreachable blocks");
    }
    
    // المرحلة 2: تحليل الحيوية / Phase 2: Liveness analysis
    auto liveInstructions = computeLiveInstructions(function);
    debug("Found " + std::to_string(liveInstructions.size()) + " live instructions");
    
    // المرحلة 3: حذف التعليمات الميتة / Phase 3: Remove dead instructions
    const auto& blocks = function->getBasicBlocks();
    for (auto& block : blocks) {
        if (block) {
            int deadRemoved = removeDeadInstructions(block.get(), liveInstructions);
            if (deadRemoved > 0) {
                modified = true;
                debug("Removed " + std::to_string(deadRemoved) + " dead instructions from block");
            }
        }
    }
    
    return modified;
}

/**
 * @brief حساب التعليمات الحية
 * @brief Compute live instructions
 * 
 * @details
 * (AR) يستخدم تحليل الحيوية بالرجوع للخلف لتحديد التعليمات الحية.
 * (EN) Uses backward liveness analysis to determine live instructions.
 */
std::unordered_set<SIR::SIRInstruction*> 
DeadCodeEliminationPass::computeLiveInstructions(SIR::SIRFunction* function) {
    std::unordered_set<SIR::SIRInstruction*> liveSet;
    std::queue<SIR::SIRInstruction*> workList;
    
    // بناء خريطة التعريفات / Build definition map
    auto defMap = buildDefMap(function);
    
    // المرحلة 1: إضافة التعليمات ذات التأثيرات الجانبية / Phase 1: Add instructions with side effects
    const auto& blocks = function->getBasicBlocks();
    for (auto& block : blocks) {
        if (!block) continue;
        
        // تكرار على تعليمات الكتلة / Iterate over block instructions
        for (auto& inst : block->instructions) {
            // التعليمات ذات التأثيرات الجانبية حية دائماً
            // Instructions with side effects are always live
            if (hasSideEffect(&inst)) {
                liveSet.insert(&inst);
                workList.push(&inst);
            }
        }
    }
    
    // المرحلة 2: الانتشار العكسي / Phase 2: Backward propagation
    while (!workList.empty()) {
        auto* inst = workList.front();
        workList.pop();
        
        // وضع علامة على المعاملات كحية / Mark operands as live
        markAsLive(inst, liveSet, workList, defMap);
    }
    
    return liveSet;
}

/**
 * @brief وضع علامة على التعليمة كحية
 * @brief Mark instruction as live
 */
void DeadCodeEliminationPass::markAsLive(
    SIR::SIRInstruction* inst,
    std::unordered_set<SIR::SIRInstruction*>& liveSet,
    std::queue<SIR::SIRInstruction*>& workList,
    const std::unordered_map<std::string, SIR::SIRInstruction*>& defMap
) {
    if (!inst) return;
    
    // الحصول على السجلات المستخدمة / Get used registers
    auto usedRegs = getUsedRegisters(inst);
    
    for (const auto& reg : usedRegs) {
        // البحث عن التعليمة التي تعرّف هذا السجل / Find instruction defining this register
        auto it = defMap.find(reg);
        if (it != defMap.end()) {
            auto* defInst = it->second;
            
            // إذا لم تكن حية بعد، أضفها / If not already live, add it
            if (liveSet.find(defInst) == liveSet.end()) {
                liveSet.insert(defInst);
                workList.push(defInst);
            }
        }
    }
}

/**
 * @brief بناء خريطة التعريفات
 * @brief Build definition map
 * 
 * @details
 * (AR) يربط كل سجل بالتعليمة التي تعرّفه.
 * (EN) Maps each register to the instruction that defines it.
 */
std::unordered_map<std::string, SIR::SIRInstruction*> 
DeadCodeEliminationPass::buildDefMap(SIR::SIRFunction* function) {
    std::unordered_map<std::string, SIR::SIRInstruction*> defMap;
    
    const auto& blocks = function->getBasicBlocks();
    for (auto& block : blocks) {
        if (!block) continue;
        
        // تكرار على تعليمات الكتلة / Iterate over block instructions
        for (auto& inst : block->instructions) {
            // الحصول على السجل المعرّف / Get defined register
            auto defReg = getDefinedRegister(&inst);
            if (defReg.has_value()) {
                defMap[defReg.value()] = &inst;
            }
        }
    }
    
    return defMap;
}

/**
 * @brief حذف التعليمات الميتة من كتلة
 * @brief Remove dead instructions from block
 */
int DeadCodeEliminationPass::removeDeadInstructions(
    SIR::SIRBasicBlock* block,
    const std::unordered_set<SIR::SIRInstruction*>& liveSet
) {
    if (!block) return 0;
    
    int removedCount = 0;
    
    // إنشاء قائمة جديدة بدون التعليمات الميتة / Create new list without dead instructions
    std::vector<SIR::SIRInstruction> newInstructions;
    
    for (auto& inst : block->instructions) {
        // الاحتفاظ بالتعليمات الحية فقط / Keep only live instructions
        if (liveSet.find(&inst) != liveSet.end()) {
            newInstructions.push_back(inst);
        } else {
            removedCount++;
            recordModification();
            
            // تسجيل التعليمة المحذوفة / Log removed instruction
            std::string instStr = "  ";
            if (inst.hasLabel()) {
                instStr += inst.label + ": ";
            }
            if (inst.hasResult()) {
                instStr += inst.result->name + " = ";
            }
            instStr += SIR::sirOpcodeToString(inst.opcode);
            debug("Removing dead instruction: " + instStr);
        }
    }
    
    // تحديث قائمة التعليمات في الكتلة / Update block's instruction list
    block->instructions = std::move(newInstructions);
    
    return removedCount;
}

/**
 * @brief حذف الكتل غير القابلة للوصول
 * @brief Remove unreachable blocks
 */
int DeadCodeEliminationPass::removeUnreachableBlocks(SIR::SIRFunction* function) {
    if (!function) return 0;
    
    // حساب الكتل القابلة للوصول / Compute reachable blocks
    auto reachableBlocks = computeReachableBlocks(function);
    
    const auto& blocks = function->getBasicBlocks();
    int totalBlocks = blocks.size();
    int reachableCount = reachableBlocks.size();
    
    if (reachableCount == totalBlocks) {
        return 0;  // كل الكتل قابلة للوصول / All blocks are reachable
    }
    
    // إنشاء قائمة جديدة بالكتل القابلة للوصول فقط
    // Create new list with only reachable blocks
    std::vector<std::shared_ptr<SIR::SIRBasicBlock>> newBlocks;
    
    for (auto& block : blocks) {
        if (!block) continue;
        
        if (reachableBlocks.find(block.get()) != reachableBlocks.end()) {
            newBlocks.push_back(block);
        } else {
            recordModification();
            debug("Removing unreachable block: " + block->name);
        }
    }
    
    // تحديث قائمة الكتل في الدالة / Update function's block list
    function->basicBlocks = std::move(newBlocks);
    
    int removedCount = totalBlocks - reachableCount;
    return removedCount;
}

/**
 * @brief حساب الكتل القابلة للوصول
 * @brief Compute reachable blocks
 * 
 * @details
 * (AR) يستخدم BFS من كتلة الدخول. يستنتج الخلفاء من تعليمات القفز
 *      (BR و BR_COND) بدلاً من الاعتماد على قائمة successors المسبقة،
 *      لأن المُبني (SIRBuilder) لا يملأ تلك القائمة حالياً.
 * (EN) Uses BFS from entry block. Derives successors from branch
 *      instructions (BR and BR_COND) rather than relying on pre-populated
 *      successors list, because SIRBuilder does not populate it currently.
 */
std::unordered_set<SIR::SIRBasicBlock*> 
DeadCodeEliminationPass::computeReachableBlocks(SIR::SIRFunction* function) {
    std::unordered_set<SIR::SIRBasicBlock*> reachable;
    
    // الحصول على كتلة الدخول / Get entry block
    const auto& blocks = function->getBasicBlocks();
    if (blocks.empty()) {
        return reachable;
    }
    
    // ================================================================
    // (AR) بناء خريطة اسم → كتلة لاستخدامها في حل أهداف القفز
    // (EN) Build name→block map for resolving branch targets
    // ================================================================
    std::unordered_map<std::string, SIR::SIRBasicBlock*> blockMap;
    for (const auto& block : blocks) {
        if (block) {
            blockMap[block->name] = block.get();
        }
    }
    
    auto* entryBlock = blocks[0].get();
    if (!entryBlock) {
        return reachable;
    }
    
    // BFS من كتلة الدخول / BFS from entry block
    std::queue<SIR::SIRBasicBlock*> workList;
    workList.push(entryBlock);
    reachable.insert(entryBlock);
    
    while (!workList.empty()) {
        auto* block = workList.front();
        workList.pop();
        
        // ============================================================
        // (AR) استنتاج الخلفاء من تعليمات القفز في الكتلة
        //      نبحث عن BR (قفز غير شرطي) و BR_COND (قفز شرطي)
        //      ونستخرج أسماء الكتل المستهدفة من المعاملات
        // (EN) Derive successors from branch instructions in the block.
        //      Look for BR (unconditional) and BR_COND (conditional)
        //      and extract target block names from operands.
        // ============================================================
        
        // أولاً: استخدم القائمة المسبقة إن وُجدت / First: use pre-populated list if available
        for (auto* succ : block->successors) {
            if (succ && reachable.find(succ) == reachable.end()) {
                reachable.insert(succ);
                workList.push(succ);
            }
        }
        
        // ثانياً: استنتج الخلفاء من التعليمات / Second: derive successors from instructions
        for (const auto& inst : block->instructions) {
            if (inst.opcode == SIR::SIROpcode::BR) {
                // BR has 1 label operand: target
                if (!inst.operands.empty() && inst.operands[0].type == SIR::SIROperandType::LABEL) {
                    auto it = blockMap.find(inst.operands[0].name);
                    if (it != blockMap.end() && reachable.find(it->second) == reachable.end()) {
                        reachable.insert(it->second);
                        workList.push(it->second);
                    }
                }
            } else if (inst.opcode == SIR::SIROpcode::BR_COND) {
                // BR_COND has operands: condition, thenLabel, elseLabel
                for (const auto& op : inst.operands) {
                    if (op.type == SIR::SIROperandType::LABEL) {
                        auto it = blockMap.find(op.name);
                        if (it != blockMap.end() && reachable.find(it->second) == reachable.end()) {
                            reachable.insert(it->second);
                            workList.push(it->second);
                        }
                    }
                }
            }
        }
    }
    
    return reachable;
}

/**
 * @brief التحقق من وجود تأثير جانبي
 * @brief Check if instruction has side effect
 */
bool DeadCodeEliminationPass::hasSideEffect(SIR::SIRInstruction* inst) const {
    if (!inst) return false;
    
    auto opcode = inst->opcode;
    
    // التعليمات ذات التأثيرات الجانبية / Instructions with side effects
    switch (opcode) {
        case SIR::SIROpcode::STORE:
        case SIR::SIROpcode::CALL:
        case SIR::SIROpcode::RET:
        case SIR::SIROpcode::RET_VOID:
        case SIR::SIROpcode::BR:
        case SIR::SIROpcode::BR_COND:
        case SIR::SIROpcode::FREE:
        case SIR::SIROpcode::MEMCPY:
        case SIR::SIROpcode::MEMSET:

        // ────────────────────────────────────────────────────────────────
        // (AR) تعليمات الإدخال/الإخراج المدمجة — لا يجب حذفها أبداً
        // (EN) Built-in I/O instructions — must never be removed
        // ────────────────────────────────────────────────────────────────
        case SIR::SIROpcode::BUILTIN_PRINT:
        case SIR::SIROpcode::BUILTIN_READ:

        // ────────────────────────────────────────────────────────────────
        // (AR) تعليمات العتاد/نظام التشغيل — لها تأثيرات جانبية حقيقية
        // (EN) Hardware/OS builtins — have real side effects (I/O, memory-mapped, interrupts)
        // ────────────────────────────────────────────────────────────────
        case SIR::SIROpcode::BUILTIN_PORT_WRITE:   // outb — 8-bit I/O port write
        case SIR::SIROpcode::BUILTIN_PORT_READ:    // inb — 8-bit I/O port read (hardware side effect)
        case SIR::SIROpcode::BUILTIN_PORT_WRITE_16:// outw — 16-bit I/O port write
        case SIR::SIROpcode::BUILTIN_PORT_READ_16: // inw — 16-bit I/O port read
        case SIR::SIROpcode::BUILTIN_PORT_WRITE_32:// outl — 32-bit I/O port write
        case SIR::SIROpcode::BUILTIN_PORT_READ_32: // inl — 32-bit I/O port read
        case SIR::SIROpcode::BUILTIN_MEM_WRITE_8:  // volatile 8-bit memory write (e.g. MMIO)
        case SIR::SIROpcode::BUILTIN_MEM_READ_8:   // volatile 8-bit memory read (e.g. MMIO)
        case SIR::SIROpcode::BUILTIN_MEM_WRITE_16: // volatile 16-bit memory write
        case SIR::SIROpcode::BUILTIN_MEM_READ_16:  // volatile 16-bit memory read
        case SIR::SIROpcode::BUILTIN_MEM_WRITE_32: // volatile 32-bit memory write
        case SIR::SIROpcode::BUILTIN_MEM_READ_32:  // volatile 32-bit memory read
        case SIR::SIROpcode::BUILTIN_MEM_WRITE_64: // volatile 64-bit memory write
        case SIR::SIROpcode::BUILTIN_MEM_READ_64:  // volatile 64-bit memory read
        case SIR::SIROpcode::BUILTIN_VGA_WRITE:    // VGA text memory write
        case SIR::SIROpcode::BUILTIN_VGA_CLEAR:    // VGA screen clear
        case SIR::SIROpcode::BUILTIN_INTERRUPT:    // software interrupt (int N)
        case SIR::SIROpcode::BUILTIN_HALT:         // halt processor (hlt)
        case SIR::SIROpcode::BUILTIN_CLI:          // disable interrupts (cli)
        case SIR::SIROpcode::BUILTIN_STI:          // enable interrupts (sti)
        case SIR::SIROpcode::BUILTIN_MEM_COPY:     // memory copy (memcpy)
        case SIR::SIROpcode::BUILTIN_MEM_SET:      // memory fill (memset)

        // ────────────────────────────────────────────────────────────────
        // (AR) تعليمات تعديل المصفوفات والكائنات
        // (EN) Array/Object mutation instructions
        // ────────────────────────────────────────────────────────────────
        case SIR::SIROpcode::ARRAY_SET:
        case SIR::SIROpcode::ARRAY_APPEND:
        case SIR::SIROpcode::OBJECT_SET:
        case SIR::SIROpcode::BUILTIN_ARRAY_APPEND:  // أضف / push
        case SIR::SIROpcode::BUILTIN_ARRAY_REMOVE:  // أزل / remove

        // ────────────────────────────────────────────────────────────────
        // (AR) نظام الواجهة الموحد — جميعها ذات آثار جانبية (UI side effects)
        // (EN) Unified UI system — all have UI side effects
        // ────────────────────────────────────────────────────────────────
        case SIR::SIROpcode::BUILTIN_UI_COLUMN:
        case SIR::SIROpcode::BUILTIN_UI_ROW:
        case SIR::SIROpcode::BUILTIN_UI_STACK:
        case SIR::SIROpcode::BUILTIN_UI_CONTAINER:
        case SIR::SIROpcode::BUILTIN_UI_TEXT:
        case SIR::SIROpcode::BUILTIN_UI_TEXT_STYLED:
        case SIR::SIROpcode::BUILTIN_UI_BUTTON:
        case SIR::SIROpcode::BUILTIN_UI_BUTTON_VARIANT:
        case SIR::SIROpcode::BUILTIN_UI_ICON_BUTTON:
        case SIR::SIROpcode::BUILTIN_UI_FAB:
        case SIR::SIROpcode::BUILTIN_UI_TEXT_FIELD:
        case SIR::SIROpcode::BUILTIN_UI_CHECKBOX:
        case SIR::SIROpcode::BUILTIN_UI_SWITCH:
        case SIR::SIROpcode::BUILTIN_UI_SLIDER:
        case SIR::SIROpcode::BUILTIN_UI_CARD:
        case SIR::SIROpcode::BUILTIN_UI_SCAFFOLD:
        case SIR::SIROpcode::BUILTIN_UI_APP_BAR:
        case SIR::SIROpcode::BUILTIN_UI_SPACER:
        case SIR::SIROpcode::BUILTIN_UI_DIVIDER:
        case SIR::SIROpcode::BUILTIN_UI_DIALOG:
        case SIR::SIROpcode::BUILTIN_UI_ADD_CHILD:
        case SIR::SIROpcode::BUILTIN_UI_REMOVE_CHILD:
        case SIR::SIROpcode::BUILTIN_UI_CLEAR_CHILDREN:
        case SIR::SIROpcode::BUILTIN_UI_SET_TEXT:
        case SIR::SIROpcode::BUILTIN_UI_SET_SIZE:
        case SIR::SIROpcode::BUILTIN_UI_SET_FLEX:
        case SIR::SIROpcode::BUILTIN_UI_SET_BACKGROUND:
        case SIR::SIROpcode::BUILTIN_UI_SET_FOREGROUND:
        case SIR::SIROpcode::BUILTIN_UI_SET_SPACING:
        case SIR::SIROpcode::BUILTIN_UI_SET_PADDING:
        case SIR::SIROpcode::BUILTIN_UI_SET_ALIGNMENT:
        case SIR::SIROpcode::BUILTIN_UI_SET_BORDER:
        case SIR::SIROpcode::BUILTIN_UI_SET_ELEVATION:
        case SIR::SIROpcode::BUILTIN_UI_SET_OPACITY:
        case SIR::SIROpcode::BUILTIN_UI_SET_VISIBILITY:
        case SIR::SIROpcode::BUILTIN_UI_APP_CREATE:
        case SIR::SIROpcode::BUILTIN_UI_APP_SET_ROOT:
        case SIR::SIROpcode::BUILTIN_UI_APP_LAYOUT:
        case SIR::SIROpcode::BUILTIN_UI_APP_RENDER:
        case SIR::SIROpcode::BUILTIN_UI_APP_DESTROY:
        case SIR::SIROpcode::BUILTIN_UI_WIDGET_DESTROY:

        // ────────────────────────────────────────────────────────────────
        // (AR) تعليمات التحكم بالبرنامج
        // (EN) Program control instructions
        // ────────────────────────────────────────────────────────────────
        case SIR::SIROpcode::SWITCH:

        // ────────────────────────────────────────────────────────────────
        // (AR) تعليمات الكوروتين والمولد — لا يجب حذفها أبداً
        // (EN) Coroutine and generator instructions — must never be removed
        // ────────────────────────────────────────────────────────────────
        case SIR::SIROpcode::CORO_SUSPEND:
        case SIR::SIROpcode::CORO_RETURN:
        case SIR::SIROpcode::GENERATOR_YIELD:
        case SIR::SIROpcode::GENERATOR_CONSUME:
            return true;
            
        default:
            return false;
    }
}

/**
 * @brief الحصول على السجل المعرّف
 * @brief Get defined register
 * 
 * (AR) بدلاً من استخدام قائمة بيضاء يدوية للأوامر التي تُعرّف سجلاً،
 *      نستخدم فحصاً عاماً عبر hasResult(). هذا يضمن أن أي تعليمة
 *      تُنتج نتيجة (ALLOC, LOAD, ADD, CALL, STRING_CONCAT, OBJECT_NEW, ...)
 *      يتم إدراجها في defMap تلقائياً — مما يمنع حذفها خطأً بواسطة DCE.
 * 
 * (EN) Instead of a manual whitelist of opcodes that define a register,
 *      we use a generic hasResult() check. This ensures that ANY instruction
 *      producing a result (ALLOC, LOAD, ADD, CALL, STRING_CONCAT, OBJECT_NEW,
 *      ...) is automatically added to defMap — preventing erroneous DCE deletion.
 */
std::optional<std::string> DeadCodeEliminationPass::getDefinedRegister(
    SIR::SIRInstruction* inst
) const {
    if (!inst) return std::nullopt;
    
    // (AR) فحص عام: أي تعليمة لها نتيجة تُعرّف سجلاً
    // (EN) Generic check: any instruction with a result defines a register
    if (inst->hasResult()) {
        return inst->result->name;
    }
    return std::nullopt;
}

/**
 * @brief الحصول على السجلات المستخدمة
 * @brief Get used registers
 */
std::vector<std::string> DeadCodeEliminationPass::getUsedRegisters(
    SIR::SIRInstruction* inst
) const {
    std::vector<std::string> usedRegs;
    
    if (!inst) return usedRegs;
    
    // الحصول على المعاملات الفعلية / Get actual operands
    // تكرار على كل المعاملات واستخراج السجلات / Iterate over operands and extract registers
    for (const auto& operand : inst->operands) {
        // استخراج السجلات فقط (تجاهل الثوابت والتسميات)
        // Extract only registers (ignore constants and labels)
        if (operand.type == SIR::SIROperandType::REGISTER) {
            usedRegs.push_back(operand.name);
        }
    }
    
    return usedRegs;
}

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
