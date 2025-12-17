// بسم الله الرحمن الرحيم
// Inlining & CSE Implementation - تنفيذ التوسيع وإزالة التعبيرات المشتركة
// Phase 6.4: Inlining & CSE
// الحمد لله رب العالمين

#include "inlining_cse.h"
#include <algorithm>
#include <queue>
#include <sstream>

namespace sad {
namespace optimizer {

// ============================================================================
// Inlining Heuristics Implementation - تنفيذ معايير التوسيع
// ============================================================================

InliningHeuristics::Metrics InliningHeuristics::calculate_metrics(Function* func) {
    Metrics metrics;
    
    if (!func) {
        return metrics;
    }
    
    metrics.basic_block_count = func->blocks().size();
    
    // Count instructions - حساب التعليمات
    for (auto* block : func->blocks()) {
        metrics.instruction_count += block->instructions().size();
    }
    
    // Check for recursion - فحص التكرار
    for (auto* block : func->blocks()) {
        for (auto* inst : block->instructions()) {
            if (auto* call = dynamic_cast<CallInst*>(inst)) {
                metrics.call_count++;
                if (call->callee() == func) {
                    metrics.has_recursion = true;
                }
            }
        }
    }
    
    // Check for loops (simplified) - فحص الحلقات (مبسّط)
    // Real implementation would use loop detection
    metrics.has_loops = false;
    
    return metrics;
}

bool InliningHeuristics::should_inline(
    const CallSite& site,
    const Metrics& callee_metrics,
    int max_inline_size,
    bool aggressive
) {
    // Cannot inline recursive functions - لا يمكن توسيع الدوال التكرارية
    if (callee_metrics.has_recursion) {
        return false;
    }
    
    // Size check - فحص الحجم
    if (callee_metrics.instruction_count > static_cast<size_t>(max_inline_size)) {
        return false;
    }
    
    // Calculate cost/benefit - حساب التكلفة/الفائدة
    int cost = calculate_inline_cost(site, callee_metrics);
    int benefit = calculate_inline_benefit(site, callee_metrics);
    
    if (aggressive) {
        return benefit >= cost / 2; // More lenient in aggressive mode
    } else {
        return benefit > cost;
    }
}

int InliningHeuristics::calculate_inline_cost(
    const CallSite& site,
    const Metrics& callee_metrics
) {
    // Cost = code size increase - التكلفة = زيادة حجم الكود
    int cost = static_cast<int>(callee_metrics.instruction_count);
    
    // Loops increase cost - الحلقات تزيد التكلفة
    if (callee_metrics.has_loops) {
        cost *= 2;
    }
    
    // Nested calls increase cost - الاستدعاءات المتداخلة تزيد التكلفة
    cost += callee_metrics.call_count * 5;
    
    return cost;
}

int InliningHeuristics::calculate_inline_benefit(
    const CallSite& site,
    const Metrics& callee_metrics
) {
    int benefit = 0;
    
    // Eliminate call overhead - إزالة عبء الاستدعاء
    benefit += 10;
    
    // In loop? Big benefit - في حلقة؟ فائدة كبيرة
    if (site.in_loop) {
        benefit += 30;
    }
    
    // Small function? - دالة صغيرة؟
    if (callee_metrics.instruction_count < 5) {
        benefit += 20;
    }
    
    // Enable other optimizations - تمكين تحسينات أخرى
    benefit += 5;
    
    return benefit;
}

// ============================================================================
// Function Inliner Implementation - تنفيذ موسّع الدوال
// ============================================================================

bool FunctionInliner::inline_call(const CallSite& site) {
    if (!can_inline(site)) {
        return false;
    }
    
    // Clone function body - نسخ جسم الدالة
    clone_function_body(site);
    
    // Map arguments to parameters - ربط المعاملات
    std::unordered_map<Value*, Value*> value_map;
    map_arguments(site, value_map);
    
    return true;
}

bool FunctionInliner::inline_calls(Function* function) {
    bool changed = false;
    
    // Find all call sites - إيجاد كل مواقع الاستدعاء
    std::vector<CallSite> call_sites;
    
    for (auto* block : function->blocks()) {
        for (auto* inst : block->instructions()) {
            if (auto* call = dynamic_cast<CallInst*>(inst)) {
                CallSite site;
                site.call_inst = call;
                site.caller = function;
                site.callee = call->callee();
                site.call_block = block;
                
                call_sites.push_back(site);
            }
        }
    }
    
    // Try to inline each call - محاولة توسيع كل استدعاء
    for (const auto& site : call_sites) {
        if (inline_call(site)) {
            changed = true;
        }
    }
    
    return changed;
}

bool FunctionInliner::can_inline(const CallSite& site) const {
    if (!config_.enable_inlining) {
        return false;
    }
    
    if (!site.callee) {
        return false;
    }
    
    // Check depth - فحص العمق
    if (site.depth >= config_.max_inline_depth) {
        return false;
    }
    
    // Calculate metrics - حساب المقاييس
    auto metrics = InliningHeuristics::calculate_metrics(site.callee);
    
    // Check size - فحص الحجم
    if (metrics.instruction_count > static_cast<size_t>(config_.max_inline_size)) {
        return false;
    }
    
    // Use heuristics - استخدام المعايير
    return InliningHeuristics::should_inline(
        site,
        metrics,
        config_.max_inline_size,
        config_.aggressive
    );
}

void FunctionInliner::clone_function_body(const CallSite& site) {
    // Clone all blocks and instructions from callee
    // نسخ كل الكتل والتعليمات من الدالة المستدعاة
    
    if (!site.callee) {
        return;
    }
    
    std::unordered_map<BasicBlock*, BasicBlock*> block_map;
    
    // Clone blocks - نسخ الكتل
    for (auto* block : site.callee->blocks()) {
        auto* cloned = block->clone();
        block_map[block] = cloned;
    }
    
    // Insert cloned blocks after call site - إدراج الكتل بعد موقع الاستدعاء
    // Simplified - التنفيذ الحقيقي أكثر تعقيداً
}

void FunctionInliner::map_arguments(
    const CallSite& site,
    std::unordered_map<Value*, Value*>& value_map
) {
    // Map function parameters to call arguments
    // ربط معاملات الدالة بمعاملات الاستدعاء
    
    if (!site.callee) {
        return;
    }
    
    auto& params = site.callee->parameters();
    auto& args = site.arguments;
    
    for (size_t i = 0; i < params.size() && i < args.size(); ++i) {
        value_map[params[i]] = args[i];
    }
}

void FunctionInliner::replace_returns(
    const CallSite& site,
    const std::vector<Instruction*>& cloned_instructions
) {
    // Replace return instructions with result assignment
    // استبدال تعليمات الإرجاع بتعيين النتيجة
    
    for (auto* inst : cloned_instructions) {
        if (auto* ret = dynamic_cast<ReturnInst*>(inst)) {
            // Replace return value with call result
            // استبدال قيمة الإرجاع بنتيجة الاستدعاء
            if (ret->value()) {
                // site.call_inst->replace_all_uses_with(ret->value());
            }
        }
    }
}

bool FunctionInliner::is_recursive(Function* func) const {
    // Check if function calls itself directly or indirectly
    // فحص إذا كانت الدالة تستدعي نفسها مباشرة أو بشكل غير مباشر
    
    if (!func) {
        return false;
    }
    
    for (auto* block : func->blocks()) {
        for (auto* inst : block->instructions()) {
            if (auto* call = dynamic_cast<CallInst*>(inst)) {
                if (call->callee() == func) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

// ============================================================================
// CSE Implementation - تنفيذ إزالة التعبيرات المشتركة
// ============================================================================

bool CSE::optimize(Function* function) {
    if (!config_.enable_cse) {
        return false;
    }
    
    bool changed = false;
    
    if (config_.enable_global_cse) {
        // Global CSE across all blocks - CSE عبر كل الكتل
        available_expressions_.clear();
        
        for (auto* block : function->blocks()) {
            changed |= optimize_block(block);
        }
    } else {
        // Local CSE per block - CSE محلي لكل كتلة
        for (auto* block : function->blocks()) {
            available_expressions_.clear();
            changed |= optimize_block(block);
        }
    }
    
    return changed;
}

bool CSE::optimize_block(BasicBlock* block) {
    bool changed = false;
    
    std::vector<Instruction*> to_remove;
    
    for (auto* inst : block->instructions()) {
        if (!can_eliminate(inst)) {
            continue;
        }
        
        // Check if equivalent expression exists - فحص وجود تعبير مكافئ
        Value* equivalent = find_equivalent(inst);
        
        if (equivalent) {
            // Replace with equivalent - الاستبدال بالمكافئ
            inst->replace_all_uses_with(equivalent);
            to_remove.push_back(inst);
            changed = true;
        } else {
            // Add to available expressions - إضافة للتعبيرات المتاحة
            available_expressions_[inst] = inst;
        }
    }
    
    // Remove eliminated instructions - حذف التعليمات المزالة
    for (auto* inst : to_remove) {
        block->remove_instruction(inst);
    }
    
    return changed;
}

size_t CSE::ExpressionHash::operator()(const Instruction* inst) const {
    if (!inst) {
        return 0;
    }
    
    // Hash based on opcode and operands - هاش بناءً على الكود والمعاملات
    size_t hash = std::hash<int>{}(static_cast<int>(inst->opcode()));
    
    for (auto* operand : inst->operands()) {
        hash ^= std::hash<Value*>{}(operand) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    
    return hash;
}

bool CSE::ExpressionEqual::operator()(
    const Instruction* a,
    const Instruction* b
) const {
    if (!a || !b) {
        return a == b;
    }
    
    // Same opcode? - نفس الكود؟
    if (a->opcode() != b->opcode()) {
        return false;
    }
    
    // Same operands? - نفس المعاملات؟
    auto& a_operands = a->operands();
    auto& b_operands = b->operands();
    
    if (a_operands.size() != b_operands.size()) {
        return false;
    }
    
    for (size_t i = 0; i < a_operands.size(); ++i) {
        if (a_operands[i] != b_operands[i]) {
            return false;
        }
    }
    
    return true;
}

Value* CSE::find_equivalent(Instruction* inst) {
    auto it = available_expressions_.find(inst);
    if (it != available_expressions_.end()) {
        return it->second;
    }
    return nullptr;
}

bool CSE::is_available(Instruction* inst) const {
    return available_expressions_.find(inst) != available_expressions_.end();
}

bool CSE::can_eliminate(Instruction* inst) const {
    if (!inst) {
        return false;
    }
    
    // Cannot eliminate instructions with side effects
    // لا يمكن إزالة التعليمات ذات الآثار الجانبية
    if (inst->has_side_effects()) {
        return false;
    }
    
    // Cannot eliminate stores - لا يمكن إزالة التخزين
    if (inst->is_store()) {
        return false;
    }
    
    // Can eliminate loads if enabled - يمكن إزالة التحميلات إذا مفعّل
    if (inst->is_load() && !config_.enable_load_cse) {
        return false;
    }
    
    // Cannot eliminate calls (unless pure) - لا يمكن إزالة الاستدعاءات
    if (inst->is_call() && !inst->is_pure()) {
        return false;
    }
    
    return true;
}

// ============================================================================
// Copy Propagation Implementation - تنفيذ نشر النسخ
// ============================================================================

bool CopyPropagation::optimize(Function* function) {
    if (!config_.enable_copy_propagation) {
        return false;
    }
    
    // Build copy map - بناء خريطة النسخ
    build_copy_map(function);
    
    // Propagate copies - نشر النسخ
    bool changed = propagate_copies(function);
    
    return changed;
}

void CopyPropagation::build_copy_map(Function* function) {
    copy_map_.clear();
    
    for (auto* block : function->blocks()) {
        for (auto* inst : block->instructions()) {
            if (is_copy(inst)) {
                // Add to copy map - إضافة لخريطة النسخ
                // Simplified: x = y becomes copy_map[x] = y
                if (auto* assign = dynamic_cast<AssignInst*>(inst)) {
                    copy_map_[inst->result()] = assign->source();
                }
            }
        }
    }
}

bool CopyPropagation::is_copy(Instruction* inst) const {
    // Check if instruction is a simple copy: x = y
    // فحص إذا كانت التعليمة نسخة بسيطة: x = y
    
    if (auto* assign = dynamic_cast<AssignInst*>(inst)) {
        // Must be direct assignment without operations
        return assign->is_simple_copy();
    }
    
    return false;
}

bool CopyPropagation::propagate_copies(Function* function) {
    bool changed = false;
    
    for (auto* block : function->blocks()) {
        for (auto* inst : block->instructions()) {
            // Replace operands with copy sources - استبدال المعاملات بمصادر النسخ
            for (size_t i = 0; i < inst->operands().size(); ++i) {
                auto* operand = inst->operands()[i];
                auto* source = get_copy_source(operand);
                
                if (source && source != operand) {
                    inst->set_operand(i, source);
                    changed = true;
                }
            }
        }
    }
    
    return changed;
}

Value* CopyPropagation::get_copy_source(Value* value) const {
    auto it = copy_map_.find(value);
    if (it != copy_map_.end()) {
        // Follow copy chain - متابعة سلسلة النسخ
        return get_copy_source(it->second);
    }
    return value;
}

// ============================================================================
// Dead Store Elimination Implementation - تنفيذ إزالة التخزين الميت
// ============================================================================

bool DeadStoreElimination::optimize(Function* function) {
    if (!config_.enable_dse) {
        return false;
    }
    
    bool changed = false;
    std::vector<Instruction*> to_remove;
    
    for (auto* block : function->blocks()) {
        for (auto* inst : block->instructions()) {
            if (inst->is_store() && is_dead_store(inst)) {
                to_remove.push_back(inst);
                changed = true;
            }
        }
    }
    
    // Remove dead stores - حذف التخزين الميت
    for (auto* inst : to_remove) {
        inst->parent_block()->remove_instruction(inst);
    }
    
    return changed;
}

bool DeadStoreElimination::is_dead_store(Instruction* store) const {
    if (!store || !store->is_store()) {
        return false;
    }
    
    // A store is dead if the value is never used before being overwritten
    // التخزين ميت إذا لم تُستخدم القيمة قبل الكتابة عليها
    
    auto* stored_value = store->operands()[0]; // Simplified
    
    // Find next use - إيجاد الاستخدام التالي
    auto* next_use = find_next_use(stored_value, store);
    
    // If next use is another store to same location, this store is dead
    // إذا كان الاستخدام التالي تخزين آخر لنفس الموقع، فالتخزين ميت
    return next_use && next_use->is_store();
}

Instruction* DeadStoreElimination::find_next_use(
    Value* value,
    Instruction* after
) const {
    // Find next instruction that uses this value
    // إيجاد التعليمة التالية التي تستخدم هذه القيمة
    
    bool found_after = false;
    auto* block = after->parent_block();
    
    for (auto* inst : block->instructions()) {
        if (inst == after) {
            found_after = true;
            continue;
        }
        
        if (found_after) {
            for (auto* operand : inst->operands()) {
                if (operand == value) {
                    return inst;
                }
            }
        }
    }
    
    return nullptr;
}

// ============================================================================
// Main Optimizer Implementation - تنفيذ المحسّن الرئيسي
// ============================================================================

bool InliningCSEOptimizer::optimize(Function* function) {
    if (!function) {
        return false;
    }
    
    bool changed = false;
    
    // Initialize sub-optimizers - تهيئة المحسّنات الفرعية
    inliner_ = std::make_unique<FunctionInliner>(config_.inliner_config);
    cse_ = std::make_unique<CSE>(config_.cse_config);
    copy_prop_ = std::make_unique<CopyPropagation>(config_.copy_prop_config);
    dse_ = std::make_unique<DeadStoreElimination>(config_.dse_config);
    
    // Iterative optimization - تحسين تكراري
    for (int iteration = 0; iteration < config_.max_iterations; ++iteration) {
        bool iter_changed = false;
        
        // Phase 1: Inlining - التوسيع
        if (config_.enable_inlining) {
            iter_changed |= apply_inlining(function);
        }
        
        // Phase 2: Copy Propagation - نشر النسخ
        if (config_.enable_copy_propagation) {
            iter_changed |= apply_copy_propagation(function);
        }
        
        // Phase 3: CSE - إزالة التعبيرات المشتركة
        if (config_.enable_cse) {
            iter_changed |= apply_cse(function);
        }
        
        // Phase 4: Dead Store Elimination - إزالة التخزين الميت
        if (config_.enable_dse) {
            iter_changed |= apply_dse(function);
        }
        
        changed |= iter_changed;
        
        // Converged? - تقاربت؟
        if (!iter_changed) {
            break;
        }
    }
    
    return changed;
}

bool InliningCSEOptimizer::optimize_module(Module* module) {
    if (!module) {
        return false;
    }
    
    bool changed = false;
    
    // Optimize each function - تحسين كل دالة
    for (auto* function : module->functions()) {
        changed |= optimize(function);
    }
    
    return changed;
}

bool InliningCSEOptimizer::apply_inlining(Function* function) {
    bool changed = inliner_->inline_calls(function);
    
    if (changed) {
        stats_.functions_inlined++;
        
        // Count inlined call sites - حساب مواقع الاستدعاء الموسّعة
        auto call_sites = find_call_sites(function);
        stats_.call_sites_inlined += call_sites.size();
    }
    
    return changed;
}

bool InliningCSEOptimizer::apply_cse(Function* function) {
    bool changed = cse_->optimize(function);
    
    if (changed) {
        stats_.expressions_eliminated++;
    }
    
    return changed;
}

bool InliningCSEOptimizer::apply_copy_propagation(Function* function) {
    bool changed = copy_prop_->optimize(function);
    
    if (changed) {
        stats_.copies_propagated++;
    }
    
    return changed;
}

bool InliningCSEOptimizer::apply_dse(Function* function) {
    bool changed = dse_->optimize(function);
    
    if (changed) {
        stats_.dead_stores_eliminated++;
    }
    
    return changed;
}

std::vector<CallSite> InliningCSEOptimizer::find_call_sites(Function* function) {
    std::vector<CallSite> sites;
    
    for (auto* block : function->blocks()) {
        for (auto* inst : block->instructions()) {
            if (auto* call = dynamic_cast<CallInst*>(inst)) {
                CallSite site;
                site.call_inst = call;
                site.caller = function;
                site.callee = call->callee();
                site.call_block = block;
                
                sites.push_back(site);
            }
        }
    }
    
    return sites;
}

void InliningCSEOptimizer::build_call_graph(Module* module) {
    // Build call graph for the module
    // بناء رسم الاستدعاء للوحدة
    
    // Simplified implementation - التنفيذ الحقيقي أكثر تعقيداً
}

std::string InliningCSEOptimizer::Statistics::to_string() const {
    std::ostringstream oss;
    oss << "Inlining & CSE Statistics:\n";
    oss << "  Functions inlined: " << functions_inlined << "\n";
    oss << "  Call sites inlined: " << call_sites_inlined << "\n";
    oss << "  Expressions eliminated (CSE): " << expressions_eliminated << "\n";
    oss << "  Copies propagated: " << copies_propagated << "\n";
    oss << "  Dead stores eliminated: " << dead_stores_eliminated << "\n";
    oss << "  Total optimizations: " << total_optimizations() << "\n";
    return oss.str();
}

} // namespace optimizer
} // namespace sad
