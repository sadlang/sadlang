// بسم الله الرحمن الرحيم
// ===================================================================
// Dead Code Elimination Implementation - تنفيذ إزالة الكود الميت
// Phase 6.1: Dead Code Elimination
// ===================================================================

#include "dead_code_elimination.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <queue>

namespace sad {
namespace optimizer {

// ===================================================================
// Statistics Implementation
// ===================================================================

void DeadCodeElimination::Stats::reset() {
    unreachable_blocks_removed = 0;
    dead_instructions_removed = 0;
    unused_variables_removed = 0;
    dead_stores_removed = 0;
    total_optimizations = 0;
}

std::string DeadCodeElimination::Stats::to_string() const {
    std::ostringstream oss;
    oss << "Dead Code Elimination Statistics:\n";
    oss << "  Unreachable blocks removed: " << unreachable_blocks_removed << "\n";
    oss << "  Dead instructions removed: " << dead_instructions_removed << "\n";
    oss << "  Unused variables removed: " << unused_variables_removed << "\n";
    oss << "  Dead stores removed: " << dead_stores_removed << "\n";
    oss << "  Total optimizations: " << total_optimizations << "\n";
    return oss.str();
}

// ===================================================================
// Main Optimization Entry Point
// ===================================================================

bool DeadCodeElimination::optimize(Function* function) {
    if (!function) {
        return false;
    }

    if (verbose_) {
        std::cout << "Running Dead Code Elimination on function: " 
                  << function->get_name() << std::endl;
    }

    // Build CFG and use-def chains
    cfg_ = build_cfg(function);
    build_use_def_chains(function);

    bool changed = false;

    if (config_.max_iterations > 1) {
        // Iterative optimization
        changed = run_iterative_optimization(function);
    } else {
        // Single pass
        if (config_.remove_unreachable) {
            changed |= remove_unreachable_blocks(function);
        }

        if (config_.remove_dead_stores) {
            changed |= remove_dead_stores(function);
        }

        changed |= remove_dead_instructions(function);

        if (config_.remove_unused_vars) {
            changed |= remove_unused_variables(function);
        }
    }

    if (changed) {
        stats_.total_optimizations++;
    }

    return changed;
}

bool DeadCodeElimination::optimize_module(std::vector<Function*>& functions) {
    bool changed = false;

    for (auto* function : functions) {
        changed |= optimize(function);
    }

    return changed;
}

// ===================================================================
// Iterative Optimization
// ===================================================================

bool DeadCodeElimination::run_iterative_optimization(Function* function) {
    bool global_changed = false;
    size_t iteration = 0;

    while (iteration < config_.max_iterations) {
        bool changed = false;

        if (config_.remove_unreachable) {
            changed |= remove_unreachable_blocks(function);
        }

        if (config_.remove_dead_stores) {
            changed |= remove_dead_stores(function);
        }

        changed |= remove_dead_instructions(function);

        if (config_.remove_unused_vars) {
            changed |= remove_unused_variables(function);
        }

        if (!changed) {
            // No more changes, stop
            break;
        }

        global_changed = true;
        iteration++;

        // Rebuild analysis after changes
        cfg_ = build_cfg(function);
        build_use_def_chains(function);
    }

    if (verbose_ && iteration > 0) {
        std::cout << "DCE converged after " << iteration << " iterations" << std::endl;
    }

    return global_changed;
}

// ===================================================================
// Phase 1: Unreachable Code Removal
// ===================================================================

bool DeadCodeElimination::remove_unreachable_blocks(Function* function) {
    std::unordered_set<BasicBlock*> reachable;
    
    // Mark reachable blocks starting from entry
    BasicBlock* entry = function->get_entry_block();
    if (!entry) {
        return false;
    }

    mark_reachable_blocks(entry, reachable);

    // Remove unreachable blocks
    bool changed = false;
    auto& blocks = function->get_basic_blocks();
    
    for (auto it = blocks.begin(); it != blocks.end(); ) {
        BasicBlock* block = *it;
        
        if (reachable.find(block) == reachable.end()) {
            // Block is unreachable - remove it
            if (verbose_) {
                print_optimization("Removing unreachable block: " + block->get_name());
            }

            remove_block(block, function);
            it = blocks.erase(it);
            
            stats_.unreachable_blocks_removed++;
            changed = true;
        } else {
            ++it;
        }
    }

    return changed;
}

void DeadCodeElimination::mark_reachable_blocks(
    BasicBlock* start,
    std::unordered_set<BasicBlock*>& reachable
) {
    if (!start || reachable.find(start) != reachable.end()) {
        return;  // Already visited
    }

    reachable.insert(start);

    // Visit successors
    auto successors = get_successors(start);
    for (auto* successor : successors) {
        mark_reachable_blocks(successor, reachable);
    }
}

bool DeadCodeElimination::is_reachable(
    BasicBlock* block,
    const std::unordered_set<BasicBlock*>& reachable
) {
    return reachable.find(block) != reachable.end();
}

// ===================================================================
// Phase 2: Dead Instruction Elimination
// ===================================================================

bool DeadCodeElimination::remove_dead_instructions(Function* function) {
    std::unordered_set<Instruction*> live;
    
    // Mark live instructions (backward analysis)
    mark_live_instructions(function, live);

    // Remove dead instructions
    bool changed = false;
    
    for (auto* block : function->get_basic_blocks()) {
        auto& instructions = block->get_instructions();
        
        for (auto it = instructions.begin(); it != instructions.end(); ) {
            Instruction* inst = *it;
            
            if (live.find(inst) == live.end() && !has_side_effects(inst)) {
                // Instruction is dead - remove it
                if (verbose_) {
                    print_optimization("Removing dead instruction: " + inst->to_string());
                }

                it = instructions.erase(it);
                stats_.dead_instructions_removed++;
                changed = true;
            } else {
                ++it;
            }
        }
    }

    return changed;
}

bool DeadCodeElimination::is_dead_instruction(Instruction* inst) {
    if (!inst) {
        return false;
    }

    // Check if instruction has any uses
    if (inst->get_num_uses() > 0) {
        return false;
    }

    // Instructions with side effects are not dead
    if (has_side_effects(inst)) {
        return false;
    }

    return true;
}

bool DeadCodeElimination::has_side_effects(Instruction* inst) {
    if (!inst) {
        return false;
    }

    // Check instruction type
    switch (inst->get_opcode()) {
        // Instructions with side effects
        case Opcode::STORE:
        case Opcode::CALL:
        case Opcode::RET:
        case Opcode::BR:
        case Opcode::COND_BR:
        case Opcode::SWITCH:
        case Opcode::THROW:
            return true;

        // Pure instructions (no side effects)
        case Opcode::LOAD:
        case Opcode::ADD:
        case Opcode::SUB:
        case Opcode::MUL:
        case Opcode::DIV:
        case Opcode::MOD:
        case Opcode::AND:
        case Opcode::OR:
        case Opcode::XOR:
        case Opcode::SHL:
        case Opcode::SHR:
        case Opcode::CMP:
        case Opcode::PHI:
            return false;

        default:
            // Conservative: assume has side effects
            return true;
    }
}

void DeadCodeElimination::mark_live_instructions(
    Function* function,
    std::unordered_set<Instruction*>& live
) {
    std::queue<Instruction*> worklist;

    // Start with instructions that have side effects
    for (auto* block : function->get_basic_blocks()) {
        for (auto* inst : block->get_instructions()) {
            if (has_side_effects(inst)) {
                live.insert(inst);
                worklist.push(inst);
            }
        }
    }

    // Propagate liveness backwards
    while (!worklist.empty()) {
        Instruction* inst = worklist.front();
        worklist.pop();

        // Mark operands as live
        for (auto* operand : inst->get_operands()) {
            if (auto* def_inst = dynamic_cast<Instruction*>(operand)) {
                if (live.find(def_inst) == live.end()) {
                    live.insert(def_inst);
                    worklist.push(def_inst);
                }
            }
        }
    }
}

// ===================================================================
// Phase 3: Unused Variable Elimination
// ===================================================================

bool DeadCodeElimination::remove_unused_variables(Function* function) {
    bool changed = false;

    auto& local_vars = function->get_local_variables();
    
    for (auto it = local_vars.begin(); it != local_vars.end(); ) {
        Value* var = *it;
        
        if (!is_variable_used(var, function)) {
            // Variable is unused - remove it
            if (verbose_) {
                print_optimization("Removing unused variable: " + var->get_name());
            }

            it = local_vars.erase(it);
            stats_.unused_variables_removed++;
            changed = true;
        } else {
            ++it;
        }
    }

    return changed;
}

size_t DeadCodeElimination::count_variable_uses(Value* variable, Function* function) {
    size_t count = 0;

    for (auto* block : function->get_basic_blocks()) {
        for (auto* inst : block->get_instructions()) {
            for (auto* operand : inst->get_operands()) {
                if (operand == variable) {
                    count++;
                }
            }
        }
    }

    return count;
}

bool DeadCodeElimination::is_variable_used(Value* variable, Function* function) {
    return count_variable_uses(variable, function) > 0;
}

// ===================================================================
// Phase 4: Dead Store Elimination
// ===================================================================

bool DeadCodeElimination::remove_dead_stores(Function* function) {
    bool changed = false;

    for (auto* block : function->get_basic_blocks()) {
        auto& instructions = block->get_instructions();
        
        for (auto it = instructions.begin(); it != instructions.end(); ) {
            Instruction* inst = *it;
            
            if (inst->get_opcode() == Opcode::STORE) {
                if (is_dead_store(inst, function)) {
                    // Dead store - remove it
                    if (verbose_) {
                        print_optimization("Removing dead store: " + inst->to_string());
                    }

                    it = instructions.erase(it);
                    stats_.dead_stores_removed++;
                    changed = true;
                    continue;
                }
            }
            
            ++it;
        }
    }

    return changed;
}

bool DeadCodeElimination::is_dead_store(Instruction* store, Function* function) {
    if (!store || store->get_opcode() != Opcode::STORE) {
        return false;
    }

    // Get stored variable
    Value* variable = store->get_operand(1);  // Destination
    if (!variable) {
        return false;
    }

    // Find next use or store of this variable
    BasicBlock* block = store->get_parent_block();
    Instruction* next = find_next_use_or_store(variable, block, store);

    // If next access is a store (not a use), this store is dead
    return next && next->get_opcode() == Opcode::STORE;
}

Instruction* DeadCodeElimination::find_next_use_or_store(
    Value* variable,
    BasicBlock* block,
    Instruction* after
) {
    bool found_after = false;
    
    // Search in current block
    for (auto* inst : block->get_instructions()) {
        if (inst == after) {
            found_after = true;
            continue;
        }
        
        if (!found_after) {
            continue;
        }

        // Check if instruction uses or stores the variable
        for (auto* operand : inst->get_operands()) {
            if (operand == variable) {
                return inst;
            }
        }
    }

    // Search in successor blocks (simplified - BFS)
    auto successors = get_successors(block);
    for (auto* successor : successors) {
        for (auto* inst : successor->get_instructions()) {
            for (auto* operand : inst->get_operands()) {
                if (operand == variable) {
                    return inst;
                }
            }
        }
    }

    return nullptr;
}

// ===================================================================
// Control Flow Analysis
// ===================================================================

std::unique_ptr<CFG> DeadCodeElimination::build_cfg(Function* function) {
    auto cfg = std::make_unique<CFG>();

    if (!function) {
        return cfg;
    }

    // Set entry and exit
    cfg->set_entry(function->get_entry_block());
    cfg->set_exit(function->get_exit_block());

    // Build edges
    for (auto* block : function->get_basic_blocks()) {
        auto successors = get_successors(block);
        for (auto* successor : successors) {
            cfg->add_edge(block, successor);
        }
    }

    return cfg;
}

std::vector<BasicBlock*> DeadCodeElimination::get_successors(BasicBlock* block) {
    if (!block) {
        return {};
    }

    // Get terminator instruction
    auto* terminator = block->get_terminator();
    if (!terminator) {
        return {};
    }

    std::vector<BasicBlock*> successors;

    switch (terminator->get_opcode()) {
        case Opcode::BR:
            // Unconditional branch
            if (auto* target = terminator->get_branch_target()) {
                successors.push_back(target);
            }
            break;

        case Opcode::COND_BR:
            // Conditional branch
            if (auto* true_target = terminator->get_true_target()) {
                successors.push_back(true_target);
            }
            if (auto* false_target = terminator->get_false_target()) {
                successors.push_back(false_target);
            }
            break;

        case Opcode::SWITCH:
            // Switch statement
            for (auto* case_target : terminator->get_switch_targets()) {
                successors.push_back(case_target);
            }
            break;

        case Opcode::RET:
            // Return - no successors
            break;

        default:
            break;
    }

    return successors;
}

std::vector<BasicBlock*> DeadCodeElimination::get_predecessors(BasicBlock* block) {
    if (!block || !cfg_) {
        return {};
    }

    return cfg_->get_predecessors(block);
}

// ===================================================================
// Use-Def Analysis
// ===================================================================

void DeadCodeElimination::build_use_def_chains(Function* function) {
    use_chains_.clear();
    def_map_.clear();

    for (auto* block : function->get_basic_blocks()) {
        for (auto* inst : block->get_instructions()) {
            // Record definition
            if (inst->defines_value()) {
                def_map_[inst->get_defined_value()] = inst;
            }

            // Record uses
            for (auto* operand : inst->get_operands()) {
                use_chains_[operand].push_back(inst);
            }
        }
    }
}

std::vector<Instruction*> DeadCodeElimination::get_uses(Value* value) {
    auto it = use_chains_.find(value);
    if (it != use_chains_.end()) {
        return it->second;
    }
    return {};
}

Instruction* DeadCodeElimination::get_definition(Value* value) {
    auto it = def_map_.find(value);
    if (it != def_map_.end()) {
        return it->second;
    }
    return nullptr;
}

// ===================================================================
// Utility Functions
// ===================================================================

void DeadCodeElimination::remove_instruction(Instruction* inst, BasicBlock* block) {
    // Implementation depends on IR structure
    // This is a placeholder
}

void DeadCodeElimination::remove_block(BasicBlock* block, Function* function) {
    // Update CFG
    update_cfg_after_removal(block);

    // Remove block from function
    // Implementation depends on IR structure
}

void DeadCodeElimination::update_cfg_after_removal(BasicBlock* removed_block) {
    if (!cfg_) {
        return;
    }

    // Remove edges involving this block
    auto predecessors = cfg_->get_predecessors(removed_block);
    auto successors = cfg_->get_successors(removed_block);

    for (auto* pred : predecessors) {
        cfg_->remove_edge(pred, removed_block);
    }

    for (auto* succ : successors) {
        cfg_->remove_edge(removed_block, succ);
    }
}

void DeadCodeElimination::print_optimization(const std::string& message) {
    std::cout << "[DCE] " << message << std::endl;
}

// ===================================================================
// CFG Implementation
// ===================================================================

void CFG::add_edge(BasicBlock* from, BasicBlock* to) {
    if (!from || !to) {
        return;
    }

    successors_[from].push_back(to);
    predecessors_[to].push_back(from);
}

void CFG::remove_edge(BasicBlock* from, BasicBlock* to) {
    auto& succs = successors_[from];
    succs.erase(std::remove(succs.begin(), succs.end(), to), succs.end());

    auto& preds = predecessors_[to];
    preds.erase(std::remove(preds.begin(), preds.end(), from), preds.end());
}

std::vector<BasicBlock*> CFG::get_successors(BasicBlock* block) const {
    auto it = successors_.find(block);
    if (it != successors_.end()) {
        return it->second;
    }
    return {};
}

std::vector<BasicBlock*> CFG::get_predecessors(BasicBlock* block) const {
    auto it = predecessors_.find(block);
    if (it != predecessors_.end()) {
        return it->second;
    }
    return {};
}

bool CFG::has_edge(BasicBlock* from, BasicBlock* to) const {
    auto succs = get_successors(from);
    return std::find(succs.begin(), succs.end(), to) != succs.end();
}

std::vector<BasicBlock*> CFG::get_all_blocks() const {
    std::vector<BasicBlock*> blocks;
    for (const auto& pair : successors_) {
        blocks.push_back(pair.first);
    }
    return blocks;
}

std::vector<BasicBlock*> CFG::get_reachable_blocks() const {
    if (!entry_) {
        return {};
    }

    std::vector<BasicBlock*> reachable;
    std::unordered_set<BasicBlock*> visited;
    std::queue<BasicBlock*> worklist;

    worklist.push(entry_);
    visited.insert(entry_);

    while (!worklist.empty()) {
        BasicBlock* block = worklist.front();
        worklist.pop();
        reachable.push_back(block);

        for (auto* successor : get_successors(block)) {
            if (visited.find(successor) == visited.end()) {
                visited.insert(successor);
                worklist.push(successor);
            }
        }
    }

    return reachable;
}

// ===================================================================
// Factory Functions
// ===================================================================

std::unique_ptr<DeadCodeElimination> create_standard_dce() {
    auto dce = std::make_unique<DeadCodeElimination>();
    
    DeadCodeElimination::Config config;
    config.remove_unreachable = true;
    config.remove_dead_stores = true;
    config.remove_unused_vars = true;
    config.aggressive_mode = false;
    config.max_iterations = 3;
    
    dce->set_config(config);
    return dce;
}

std::unique_ptr<DeadCodeElimination> create_aggressive_dce() {
    auto dce = std::make_unique<DeadCodeElimination>();
    
    DeadCodeElimination::Config config;
    config.remove_unreachable = true;
    config.remove_dead_stores = true;
    config.remove_unused_vars = true;
    config.aggressive_mode = true;
    config.max_iterations = 10;
    
    dce->set_config(config);
    return dce;
}

bool needs_dead_code_elimination(Function* function) {
    if (!function) {
        return false;
    }

    // Quick heuristic check
    size_t total_blocks = function->get_basic_blocks().size();
    size_t total_instructions = 0;

    for (auto* block : function->get_basic_blocks()) {
        total_instructions += block->get_instructions().size();
    }

    // If function is non-trivial, likely benefits from DCE
    return total_blocks > 2 || total_instructions > 10;
}

} // namespace optimizer
} // namespace sad
