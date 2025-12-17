// بسم الله الرحمن الرحيم
// Loop Optimizer Implementation - تنفيذ محسّن الحلقات
// Phase 6.3: Loop Optimizations
// الحمد لله رب العالمين

#include "loop_optimizer.h"
#include <algorithm>
#include <queue>
#include <sstream>

namespace sad {
namespace optimizer {

// ============================================================================
// Loop Detection Implementation - تنفيذ كشف الحلقات
// ============================================================================

std::vector<std::unique_ptr<LoopInfo>> LoopDetector::detect_loops(Function* function) {
    std::vector<std::unique_ptr<LoopInfo>> loops;
    
    if (!function || function->blocks().empty()) {
        return loops;
    }
    
    // Compute dominators - حساب المسيطرات
    compute_dominators(function);
    
    // Find back edges - إيجاد الحواف الراجعة
    auto back_edges = find_back_edges(function);
    
    // Build loop for each back edge - بناء حلقة لكل حافة راجعة
    for (const auto& [header, latch] : back_edges) {
        auto loop = build_loop(header, latch);
        if (loop && !loop->body.empty()) {
            loops.push_back(std::move(loop));
        }
    }
    
    return loops;
}

std::vector<std::pair<BasicBlock*, BasicBlock*>> 
LoopDetector::find_back_edges(Function* function) {
    std::vector<std::pair<BasicBlock*, BasicBlock*>> back_edges;
    
    // A back edge is an edge from B to A where A dominates B
    // حافة راجعة: من B إلى A حيث A يسيطر على B
    for (auto* block : function->blocks()) {
        for (auto* successor : block->successors()) {
            if (dominates(successor, block)) {
                // Found back edge: block -> successor
                back_edges.push_back({successor, block});
            }
        }
    }
    
    return back_edges;
}

std::unique_ptr<LoopInfo> LoopDetector::build_loop(
    BasicBlock* header,
    BasicBlock* latch
) {
    auto loop = std::make_unique<LoopInfo>();
    loop->header = header;
    loop->latch = latch;
    
    // Find loop body using BFS - إيجاد جسم الحلقة
    find_loop_body(loop.get());
    
    return loop;
}

void LoopDetector::find_loop_body(LoopInfo* loop) {
    // Loop body consists of all blocks that can reach the latch
    // without going through the header (except from within the loop)
    // جسم الحلقة: كل الكتل التي تصل للنهاية دون المرور بالرأس
    
    std::queue<BasicBlock*> worklist;
    loop->body.insert(loop->header);
    loop->body.insert(loop->latch);
    
    // Start BFS from latch predecessors
    for (auto* pred : loop->latch->predecessors()) {
        if (pred != loop->header && loop->body.find(pred) == loop->body.end()) {
            worklist.push(pred);
            loop->body.insert(pred);
        }
    }
    
    // BFS to find all loop body blocks
    while (!worklist.empty()) {
        auto* block = worklist.front();
        worklist.pop();
        
        for (auto* pred : block->predecessors()) {
            if (loop->body.find(pred) == loop->body.end()) {
                worklist.push(pred);
                loop->body.insert(pred);
            }
        }
    }
}

void LoopDetector::compute_dominators(Function* function) {
    // Simple dominance algorithm - خوارزمية بسيطة للمسيطرات
    // A dominates B if all paths from entry to B go through A
    // A يسيطر على B إذا كانت كل المسارات من المدخل إلى B تمر عبر A
    
    dominators_.clear();
    
    if (function->blocks().empty()) {
        return;
    }
    
    auto* entry = function->blocks().front();
    
    // Initialize: entry dominates itself
    dominators_[entry].insert(entry);
    
    // All other blocks: dominated by all blocks initially
    for (auto* block : function->blocks()) {
        if (block != entry) {
            for (auto* b : function->blocks()) {
                dominators_[block].insert(b);
            }
        }
    }
    
    // Iterative algorithm - خوارزمية تكرارية
    bool changed = true;
    while (changed) {
        changed = false;
        
        for (auto* block : function->blocks()) {
            if (block == entry) continue;
            
            // Dom(B) = {B} ∪ (∩ Dom(P) for all predecessors P of B)
            std::unordered_set<BasicBlock*> new_doms;
            new_doms.insert(block);
            
            // Intersection of all predecessor dominators
            bool first = true;
            for (auto* pred : block->predecessors()) {
                if (first) {
                    new_doms.insert(
                        dominators_[pred].begin(),
                        dominators_[pred].end()
                    );
                    first = false;
                } else {
                    // Intersection
                    std::unordered_set<BasicBlock*> intersection;
                    for (auto* dom : new_doms) {
                        if (dominators_[pred].find(dom) != dominators_[pred].end()) {
                            intersection.insert(dom);
                        }
                    }
                    new_doms = std::move(intersection);
                }
            }
            
            new_doms.insert(block);
            
            if (new_doms != dominators_[block]) {
                dominators_[block] = std::move(new_doms);
                changed = true;
            }
        }
    }
}

bool LoopDetector::dominates(BasicBlock* a, BasicBlock* b) const {
    auto it = dominators_.find(b);
    if (it == dominators_.end()) {
        return false;
    }
    return it->second.find(a) != it->second.end();
}

// ============================================================================
// Loop Unrolling Implementation - تنفيذ توسيع الحلقات
// ============================================================================

bool LoopUnroller::unroll(LoopInfo* loop) {
    if (!can_unroll(loop)) {
        return false;
    }
    
    int factor = get_unroll_factor(loop);
    if (factor <= 1) {
        return false;
    }
    
    // Clone loop body - نسخ جسم الحلقة
    clone_loop_body(loop, factor);
    
    // Update loop bounds - تحديث حدود الحلقة
    update_loop_bounds(loop, factor);
    
    return true;
}

bool LoopUnroller::can_unroll(LoopInfo* loop) const {
    if (!config_.enable_unrolling) {
        return false;
    }
    
    // Must be innermost loop - يجب أن تكون الحلقة الأعمق
    if (!loop->is_innermost()) {
        return false;
    }
    
    // Must have known trip count or allow partial unroll
    if (!loop->trip_count && !config_.allow_partial_unroll) {
        return false;
    }
    
    // Body must not be too large - الجسم لا يجب أن يكون كبيراً جداً
    if (loop->size() > static_cast<size_t>(config_.max_unroll_body_size)) {
        return false;
    }
    
    return true;
}

int LoopUnroller::get_unroll_factor(LoopInfo* loop) const {
    // If trip count is known, use it - إذا كان عدد التكرارات معروفاً
    if (loop->trip_count) {
        int64_t count = *loop->trip_count;
        
        // Find best power of 2 unroll factor
        for (int factor = config_.max_unroll_factor; factor >= 2; factor /= 2) {
            if (count % factor == 0) {
                return factor;
            }
        }
    }
    
    // Default unroll factor based on body size
    if (loop->size() <= 5) {
        return std::min(8, config_.max_unroll_factor);
    } else if (loop->size() <= 10) {
        return std::min(4, config_.max_unroll_factor);
    } else {
        return std::min(2, config_.max_unroll_factor);
    }
}

void LoopUnroller::clone_loop_body(LoopInfo* loop, int factor) {
    // Clone the loop body (factor - 1) times
    // نسخ جسم الحلقة (factor - 1) مرة
    
    std::vector<BasicBlock*> body_blocks(loop->body.begin(), loop->body.end());
    
    for (int i = 1; i < factor; ++i) {
        for (auto* block : body_blocks) {
            // Clone block and its instructions
            auto* cloned = block->clone();
            
            // Add cloned block to loop body
            loop->body.insert(cloned);
        }
    }
}

void LoopUnroller::update_loop_bounds(LoopInfo* loop, int factor) {
    // Update the induction variable increment
    // تحديث زيادة متغير التحريض
    
    if (!loop->induction_var) {
        return;
    }
    
    // Find the increment instruction
    // إيجاد تعليمة الزيادة
    for (auto* block : loop->body) {
        for (auto* inst : block->instructions()) {
            if (inst->result() == loop->induction_var) {
                // Update increment by factor
                // مثال: i = i + 1 becomes i = i + factor
                if (auto* add_inst = dynamic_cast<BinaryOpInst*>(inst)) {
                    if (add_inst->op() == BinaryOp::ADD) {
                        // Multiply step by factor
                        // ضرب الخطوة بالعامل
                        // This is simplified - real implementation would be more complex
                    }
                }
            }
        }
    }
}

// ============================================================================
// Loop Fusion Implementation - تنفيذ دمج الحلقات
// ============================================================================

bool LoopFusion::fuse(LoopInfo* loop1, LoopInfo* loop2) {
    if (!can_fuse(loop1, loop2)) {
        return false;
    }
    
    // Check compatibility - فحص التوافق
    if (!are_compatible(loop1, loop2)) {
        return false;
    }
    
    // Merge loop bodies - دمج الأجسام
    merge_bodies(loop1, loop2);
    
    return true;
}

bool LoopFusion::can_fuse(LoopInfo* loop1, LoopInfo* loop2) const {
    if (!config_.enable_fusion) {
        return false;
    }
    
    // Loops must be adjacent - يجب أن تكون متجاورة
    // Simplified check - فحص مبسط
    
    // Must have same parent - يجب أن يكون لهما نفس الأب
    if (loop1->parent != loop2->parent) {
        return false;
    }
    
    return true;
}

bool LoopFusion::are_compatible(LoopInfo* loop1, LoopInfo* loop2) const {
    // Loops must have same iteration space - نفس فضاء التكرار
    
    // Same trip count - نفس عدد التكرارات
    if (loop1->trip_count != loop2->trip_count) {
        return false;
    }
    
    // No dependencies between loops - لا توجد تبعيات
    if (config_.check_dependencies) {
        // Simplified dependency check
        // فحص تبعيات مبسط
        // Real implementation would use dependency analysis
    }
    
    return true;
}

void LoopFusion::merge_bodies(LoopInfo* loop1, LoopInfo* loop2) {
    // Merge loop2 body into loop1 - دمج جسم loop2 في loop1
    
    // Add loop2 blocks to loop1
    for (auto* block : loop2->body) {
        loop1->body.insert(block);
    }
    
    // Merge exits
    loop1->exits.insert(
        loop1->exits.end(),
        loop2->exits.begin(),
        loop2->exits.end()
    );
}

// ============================================================================
// Loop Fission Implementation - تنفيذ تقسيم الحلقات
// ============================================================================

std::vector<std::unique_ptr<LoopInfo>> LoopFission::split(LoopInfo* loop) {
    std::vector<std::unique_ptr<LoopInfo>> result;
    
    if (!can_split(loop)) {
        return result;
    }
    
    // Find split points - إيجاد نقاط التقسيم
    auto split_points = find_split_points(loop);
    
    if (split_points.empty()) {
        return result;
    }
    
    // Create split loops - إنشاء حلقات منقسمة
    int start = 0;
    for (int point : split_points) {
        auto split_loop = create_split_loop(loop, start, point);
        if (split_loop) {
            result.push_back(std::move(split_loop));
        }
        start = point + 1;
    }
    
    // Last segment - القطعة الأخيرة
    auto last_loop = create_split_loop(loop, start, loop->size());
    if (last_loop) {
        result.push_back(std::move(last_loop));
    }
    
    return result;
}

bool LoopFission::can_split(LoopInfo* loop) const {
    if (!config_.enable_fission) {
        return false;
    }
    
    // Loop must be large enough - الحلقة يجب أن تكون كبيرة كافية
    if (loop->size() < static_cast<size_t>(config_.min_split_size * 2)) {
        return false;
    }
    
    return true;
}

std::vector<int> LoopFission::find_split_points(LoopInfo* loop) const {
    std::vector<int> points;
    
    // Simplified: split in half - مبسّط: تقسيم للنصف
    if (loop->size() >= static_cast<size_t>(config_.min_split_size * 2)) {
        points.push_back(loop->size() / 2);
    }
    
    return points;
}

std::unique_ptr<LoopInfo> LoopFission::create_split_loop(
    LoopInfo* original,
    int start_idx,
    int end_idx
) {
    auto loop = std::make_unique<LoopInfo>();
    
    // Copy relevant blocks - نسخ الكتل المطلوبة
    std::vector<BasicBlock*> blocks(original->body.begin(), original->body.end());
    
    for (int i = start_idx; i < end_idx && i < static_cast<int>(blocks.size()); ++i) {
        loop->body.insert(blocks[i]);
    }
    
    // Copy other properties - نسخ الخصائص الأخرى
    loop->depth = original->depth;
    loop->parent = original->parent;
    
    return loop;
}

// ============================================================================
// LICM Implementation - تنفيذ نقل الكود الثابت
// ============================================================================

bool LICM::optimize(LoopInfo* loop) {
    if (!config_.enable_licm) {
        return false;
    }
    
    // Find loop invariants - إيجاد الثوابت
    auto invariants = find_invariants(loop);
    
    if (invariants.empty()) {
        return false;
    }
    
    // Create preheader if needed - إنشاء preheader
    auto* preheader = create_preheader(loop);
    if (!preheader) {
        return false;
    }
    
    // Hoist invariant instructions - رفع التعليمات الثابتة
    bool changed = false;
    for (auto* value : invariants) {
        if (auto* inst = dynamic_cast<Instruction*>(value)) {
            if (hoist(inst, loop)) {
                changed = true;
            }
        }
    }
    
    return changed;
}

std::unordered_set<Value*> LICM::find_invariants(LoopInfo* loop) {
    std::unordered_set<Value*> invariants;
    
    // An instruction is loop invariant if:
    // 1. All operands are defined outside the loop, OR
    // 2. All operands are themselves loop invariant
    
    // التعليمة ثابتة إذا:
    // 1. كل المعاملات معرّفة خارج الحلقة، أو
    // 2. كل المعاملات نفسها ثابتة
    
    bool changed = true;
    while (changed) {
        changed = false;
        
        for (auto* block : loop->body) {
            for (auto* inst : block->instructions()) {
                if (invariants.find(inst) != invariants.end()) {
                    continue; // Already marked
                }
                
                if (is_invariant(inst, loop)) {
                    invariants.insert(inst);
                    changed = true;
                }
            }
        }
    }
    
    return invariants;
}

bool LICM::is_invariant(Value* value, LoopInfo* loop) const {
    auto* inst = dynamic_cast<Instruction*>(value);
    if (!inst) {
        return true; // Constants and parameters are invariant
    }
    
    // Check if defined outside loop - فحص إذا معرّف خارج الحلقة
    bool defined_in_loop = false;
    for (auto* block : loop->body) {
        if (block->contains(inst)) {
            defined_in_loop = true;
            break;
        }
    }
    
    if (!defined_in_loop) {
        return true;
    }
    
    // Check if all operands are invariant - فحص إذا كل المعاملات ثابتة
    for (auto* operand : inst->operands()) {
        if (!is_invariant(operand, loop)) {
            return false;
        }
    }
    
    return true;
}

bool LICM::hoist(Instruction* inst, LoopInfo* loop) {
    if (!can_hoist(inst, loop)) {
        return false;
    }
    
    // Move instruction to preheader - نقل التعليمة للـ preheader
    auto* preheader = create_preheader(loop);
    if (!preheader) {
        return false;
    }
    
    // Remove from current block and add to preheader
    inst->parent_block()->remove_instruction(inst);
    preheader->add_instruction(inst);
    
    return true;
}

bool LICM::can_hoist(Instruction* inst, LoopInfo* loop) const {
    // Cannot hoist if instruction has side effects - لا يمكن الرفع إذا كان لها آثار جانبية
    if (inst->has_side_effects()) {
        return false;
    }
    
    // Cannot hoist stores - لا يمكن رفع التخزين
    if (inst->is_store()) {
        return false;
    }
    
    // Cannot hoist calls (unless marked pure) - لا يمكن رفع الاستدعاءات
    if (inst->is_call() && !inst->is_pure()) {
        return false;
    }
    
    return true;
}

BasicBlock* LICM::create_preheader(LoopInfo* loop) {
    // Create a new block before loop header - إنشاء كتلة قبل رأس الحلقة
    // Simplified implementation - تنفيذ مبسّط
    
    if (!loop->header) {
        return nullptr;
    }
    
    // Check if preheader already exists
    // In real implementation, would create and wire up properly
    // في التنفيذ الحقيقي، ننشئ ونربط بشكل صحيح
    
    return loop->header; // Simplified
}

// ============================================================================
// Induction Variable Analysis Implementation - تنفيذ تحليل متغيرات التحريض
// ============================================================================

std::vector<InductionVariableAnalysis::InductionVariable>
InductionVariableAnalysis::analyze(LoopInfo* loop) {
    std::vector<InductionVariable> result;
    
    // Find basic IVs - إيجاد المتغيرات الأساسية
    auto basic_ivs = find_basic_ivs(loop);
    result.insert(result.end(), basic_ivs.begin(), basic_ivs.end());
    
    // Find derived IVs - إيجاد المتغيرات المشتقة
    auto derived_ivs = find_derived_ivs(loop, basic_ivs);
    result.insert(result.end(), derived_ivs.begin(), derived_ivs.end());
    
    return result;
}

std::vector<InductionVariableAnalysis::InductionVariable>
InductionVariableAnalysis::find_basic_ivs(LoopInfo* loop) {
    std::vector<InductionVariable> ivs;
    
    // A basic induction variable:
    // - Has form: i = i + c (where c is loop invariant)
    // - متغير تحريض أساسي: i = i + c (حيث c ثابت)
    
    for (auto* block : loop->body) {
        for (auto* inst : block->instructions()) {
            auto* binary = dynamic_cast<BinaryOpInst*>(inst);
            if (!binary) continue;
            
            // Check if i = i + c pattern
            if (binary->op() == BinaryOp::ADD || binary->op() == BinaryOp::SUB) {
                auto* lhs = binary->lhs();
                auto* rhs = binary->rhs();
                
                // Check if lhs is the same as result (i = i + ...)
                if (lhs == binary->result()) {
                    InductionVariable iv;
                    iv.variable = binary->result();
                    iv.step_value = rhs;
                    iv.op = (binary->op() == BinaryOp::ADD) ? "+" : "-";
                    iv.is_basic = true;
                    
                    ivs.push_back(iv);
                }
            }
        }
    }
    
    return ivs;
}

std::vector<InductionVariableAnalysis::InductionVariable>
InductionVariableAnalysis::find_derived_ivs(
    LoopInfo* loop,
    const std::vector<InductionVariable>& basic_ivs
) {
    std::vector<InductionVariable> derived;
    
    // A derived induction variable is a linear function of a basic IV
    // متغير مشتق: دالة خطية لمتغير أساسي
    // j = a * i + b (where i is basic IV)
    
    for (auto* block : loop->body) {
        for (auto* inst : block->instructions()) {
            // Simplified detection - كشف مبسّط
            // Real implementation would do proper pattern matching
        }
    }
    
    return derived;
}

bool InductionVariableAnalysis::simplify_ivs(
    LoopInfo* loop,
    const std::vector<InductionVariable>& ivs
) {
    bool changed = false;
    
    // Simplify induction variables - تبسيط المتغيرات
    for (const auto& iv : ivs) {
        // Compute trip count if possible - حساب عدد التكرارات
        auto trip_count = compute_trip_count(iv, loop);
        if (trip_count) {
            loop->trip_count = trip_count;
            changed = true;
        }
    }
    
    return changed;
}

std::optional<int64_t> InductionVariableAnalysis::compute_trip_count(
    const InductionVariable& iv,
    LoopInfo* loop
) const {
    // Simplified trip count computation - حساب مبسّط لعدد التكرارات
    // Real implementation would analyze loop bounds
    // التنفيذ الحقيقي يحلل حدود الحلقة
    
    return std::nullopt;
}

bool InductionVariableAnalysis::is_induction_variable(
    Value* value,
    LoopInfo* loop
) const {
    // Simplified check - فحص مبسّط
    return false;
}

// ============================================================================
// Main Loop Optimizer Implementation - تنفيذ المحسّن الرئيسي
// ============================================================================

bool LoopOptimizer::optimize(Function* function) {
    if (!function) {
        return false;
    }
    
    bool changed = false;
    
    // Initialize sub-optimizers - تهيئة المحسّنات الفرعية
    detector_ = std::make_unique<LoopDetector>();
    unroller_ = std::make_unique<LoopUnroller>(config_.unroll_config);
    fusion_ = std::make_unique<LoopFusion>(config_.fusion_config);
    fission_ = std::make_unique<LoopFission>(config_.fission_config);
    licm_ = std::make_unique<LICM>(config_.licm_config);
    iv_analysis_ = std::make_unique<InductionVariableAnalysis>();
    
    // Iterative optimization - تحسين تكراري
    for (int iteration = 0; iteration < config_.max_iterations; ++iteration) {
        bool iter_changed = false;
        
        // Phase 1: Detect loops - كشف الحلقات
        iter_changed |= detect_loops(function);
        
        // Phase 2: Apply LICM - تطبيق LICM
        if (config_.enable_licm) {
            iter_changed |= apply_licm();
        }
        
        // Phase 3: Simplify induction variables - تبسيط متغيرات التحريض
        if (config_.enable_iv_simplification) {
            iter_changed |= simplify_induction_variables();
        }
        
        // Phase 4: Apply unrolling - تطبيق التوسيع
        if (config_.enable_unrolling) {
            iter_changed |= apply_unrolling();
        }
        
        // Phase 5: Apply fusion - تطبيق الدمج
        if (config_.enable_fusion) {
            iter_changed |= apply_fusion();
        }
        
        // Phase 6: Apply fission - تطبيق التقسيم
        if (config_.enable_fission) {
            iter_changed |= apply_fission();
        }
        
        changed |= iter_changed;
        
        // Converged? - تقاربت؟
        if (!iter_changed) {
            break;
        }
    }
    
    return changed;
}

bool LoopOptimizer::detect_loops(Function* function) {
    loops_ = detector_->detect_loops(function);
    stats_.loops_detected = loops_.size();
    return !loops_.empty();
}

bool LoopOptimizer::apply_licm() {
    bool changed = false;
    
    for (auto& loop : loops_) {
        if (licm_->optimize(loop.get())) {
            stats_.invariants_hoisted++;
            changed = true;
        }
    }
    
    return changed;
}

bool LoopOptimizer::apply_unrolling() {
    bool changed = false;
    
    for (auto& loop : loops_) {
        if (unroller_->unroll(loop.get())) {
            stats_.loops_unrolled++;
            changed = true;
        }
    }
    
    return changed;
}

bool LoopOptimizer::apply_fusion() {
    bool changed = false;
    
    // Try to fuse adjacent loops - محاولة دمج الحلقات المتجاورة
    for (size_t i = 0; i < loops_.size(); ++i) {
        for (size_t j = i + 1; j < loops_.size(); ++j) {
            if (fusion_->fuse(loops_[i].get(), loops_[j].get())) {
                stats_.loops_fused++;
                changed = true;
                // Remove second loop - حذف الحلقة الثانية
                loops_.erase(loops_.begin() + j);
                --j;
            }
        }
    }
    
    return changed;
}

bool LoopOptimizer::apply_fission() {
    bool changed = false;
    
    std::vector<std::unique_ptr<LoopInfo>> new_loops;
    
    for (auto& loop : loops_) {
        auto split_loops = fission_->split(loop.get());
        
        if (!split_loops.empty()) {
            stats_.loops_split++;
            changed = true;
            
            // Add split loops - إضافة الحلقات المنقسمة
            for (auto& split : split_loops) {
                new_loops.push_back(std::move(split));
            }
        } else {
            // Keep original - الإبقاء على الأصلية
            new_loops.push_back(std::move(loop));
        }
    }
    
    if (changed) {
        loops_ = std::move(new_loops);
    }
    
    return changed;
}

bool LoopOptimizer::simplify_induction_variables() {
    bool changed = false;
    
    for (auto& loop : loops_) {
        auto ivs = iv_analysis_->analyze(loop.get());
        
        if (iv_analysis_->simplify_ivs(loop.get(), ivs)) {
            stats_.ivs_simplified++;
            changed = true;
        }
    }
    
    return changed;
}

std::string LoopOptimizer::Statistics::to_string() const {
    std::ostringstream oss;
    oss << "Loop Optimization Statistics:\n";
    oss << "  Loops detected: " << loops_detected << "\n";
    oss << "  Loops unrolled: " << loops_unrolled << "\n";
    oss << "  Loops fused: " << loops_fused << "\n";
    oss << "  Loops split: " << loops_split << "\n";
    oss << "  Invariants hoisted: " << invariants_hoisted << "\n";
    oss << "  IVs simplified: " << ivs_simplified << "\n";
    oss << "  Total optimizations: " << total_optimizations() << "\n";
    return oss.str();
}

} // namespace optimizer
} // namespace sad
